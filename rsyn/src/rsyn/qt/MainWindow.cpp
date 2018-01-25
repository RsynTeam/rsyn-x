#include <exception>

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/io/Graphics.h"
#include "rsyn/util/StreamLogger.h"

#include "rsyn/model/routing/RoutingEstimator.h"
#include "rsyn/model/timing/Timer.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QString>
#include <QKeyEvent>
#include <QScrollBar>
#include <QThread>
#include <qt5/QtWidgets/qstatusbar.h>

#include "MainWindow.h"
#include "GraphicsStipple.h"
#include "GraphicsScene.h"
#include "GraphicsView.h"
#include "GraphicsLayer.h"
#include "GraphicsItem.h"

#include "rsyn/qt/overlay/routing/RoutingOverlay.h"
#include "rsyn/qt/overlay/instance/InstanceOverlay.h"
#include "rsyn/qt/overlay/highlight/HighlightOverlay.h"
#include "rsyn/qt/overlay/floorplan/FloorplanOverlay.h"
#include "rsyn/qt/overlay/routingGuide/RoutingGuideOverlay.h"

#include "rsyn/qt/QtUtils.h"
#include "rsyn/qt/infra/PinMgr.h"

const bool UseAlternativeSceneMgr = false;

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

MainWindow::RegisteredOverlayVec *MainWindow::clsRegisteredOverlays = nullptr;

// -----------------------------------------------------------------------------

void
WorkerThread::run() {
	QString result;

	try {
		Rsyn::Session session;
		session.evaluateString(command);
	} catch (std::exception &e) {
		std::cout << "EXCEPTION: " << e.what() << "\n";
	} catch (...) {
		std::cout << "EXCEPTION: " << "unknown" << "\n";
	} // end catch

	emit resultReady(result);
} // end method

// -----------------------------------------------------------------------------

static const int MAX_COMMAND_HISTORY_LENGTH = 100;

// -----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
		view(nullptr),
		scene(nullptr)
{
	setupUi(this);
	statusBar()->show();

	onUpdateLog();
	QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onUpdateLog()));
    timer->start(1000);

	GraphicsStippleMgr::create();
	
	txtCommand->installEventFilter(this);

	scene = new GraphicsScene(this);

	view = new GraphicsView(this);
	view->setStatusBar(statusBar());
	view->window()->layout()->setMargin(0);

	gridLayout->addWidget(view, 0, 0, 1, 1);

	createOverlays();
	restoreCommandHistory();

	// Register to receive notifications about session changes.
	Rsyn::Session session;
	session.registerObserver(this);
} // end constructor

// -----------------------------------------------------------------------------

void 
MainWindow::onDesignLoaded() {

} // end method

// -----------------------------------------------------------------------------

void
MainWindow::restoreCommandHistory() {
	Rsyn::Session session;

	ScriptParsing::CommandManager &commandManager =
			session.getCommandManager();

	const int length = std::min(MAX_COMMAND_HISTORY_LENGTH,
			clsSettings.beginReadArray("CommandHistory"));
	for (int i = 0; i < length; i++) {
		clsSettings.setArrayIndex(i);

		const std::string &cmd = clsSettings.value("command").toString().toStdString();
		if (!cmd.empty())
			commandManager.pushHistory(cmd);
	} // end for
	clsSettings.endArray();
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::storeCommandHistory(const std::string &command) {
	if (clsCurrentCommandHistoryIndex >= MAX_COMMAND_HISTORY_LENGTH) {
		clsCurrentCommandHistoryIndex = 0;
	} else {
		clsCurrentCommandHistoryIndex++;
	} // end else

	clsSettings.beginWriteArray("CommandHistory");
	clsSettings.setArrayIndex(clsCurrentCommandHistoryIndex);
	clsSettings.setValue("command", QString::fromStdString(command));
	clsSettings.endArray();
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::populate() {
	Rsyn::Session session;
	rsynGraphics = session.getService("rsyn.graphics");

	QtPinMgr::create();

	{ // Define best bsp depth for the scene based on design area
		Rsyn::Session session;
		Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

		const float rowHeight = physicalDesign.getRowHeight();
		const float w = physicalDesign.getPhysicalDie().getBounds().getWidth();
		const float h = physicalDesign.getPhysicalDie().getBounds().getHeight();
		const int optimalDepth = (int) std::ceil(std::log2(std::max(w, h)
				/ float(4*rowHeight)) + 1);

		std::cout << "BSP Depth = " <<  (optimalDepth) << "\n";
		scene->setBspTreeDepth(optimalDepth);
	} // end block

	{ // Initialize overlays
		if (!UseAlternativeSceneMgr) {
			for (GraphicsOverlay *overlay : clsOverlays) {
				Stepwatch watch("Initialize overlay " + overlay->getName());
				clsGraphicsLayerDescriptors.clear();
				const bool success = initOverlayRecursive(overlay, clsGraphicsLayerDescriptors);
				if (success) {
					scene->addItem(overlay);
				} // end if
			} // end for
		} else {
			populateAlternative();
		} // end else
	} // end block

	{ // Populate widgets
		populateObjectVisibility();
		populateLayerVisibility();
		populateDesignProperties();
	} // end block

	{ // Initialize view
		Stepwatch watchOverlays("Initializing scene");
		view->setScene(scene);
		view->init();
	} // end block
} // end method

// -----------------------------------------------------------------------------

class AlternativeCellGraphicsItem : public GraphicsItem  {
public:

	AlternativeCellGraphicsItem(Rsyn::Cell cell) : clsCell(cell) {
	} // end constructor

	virtual void render(QPainter *painter) {
		painter->drawRect(clsCell.getX(), clsCell.getY(), clsCell.getWidth(), clsCell.getHeight());
	} // end method

	virtual QRect getBoundingRect() const {
		return QRect(clsCell.getX(), clsCell.getY(), clsCell.getWidth(), clsCell.getHeight());
	} // end method

private:
	Rsyn::Cell clsCell;
}; // end class

void
MainWindow::populateAlternative() {
	Rsyn::Session session;
	Rsyn::Design design = session.getDesign();
	Rsyn::Module module = session.getTopModule();
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	if (!physicalDesign)
		return;

	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();
	const QRectF sceneRect(coreBounds.getX(), coreBounds.getY(),
			coreBounds.getWidth(), coreBounds.getHeight());
	
	scene->setSceneRect(sceneRect);

	GraphicsLayer *instanceLayer = new GraphicsLayer;
	
	QPen pen;
	pen.setColor(QColor(0, 210, 210));
	pen.setCosmetic(true);
	instanceLayer->setPen(pen);

	for (Rsyn::Instance instance : module.allInstances()) {
		if (instance.getType() != Rsyn::CELL)
			continue;

		instanceLayer->addItem(new AlternativeCellGraphicsItem(instance.asCell()));
	} // end for

	scene->addLayer(instanceLayer);
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::populateDesignProperties() {
	Rsyn::Session session;
	Rsyn::Design design = session.getDesign();
	if (!design)
		return;

	QtUtils::QTreeDescriptor tree;

	tree.addHeader(0, "Property");
	tree.addHeader(1, "Value");

	tree.add("Name", 1,
			design.getName());
	
	tree.add("Instances", 1,
			std::to_string(design.getNumInstances()));
	tree.add("Instances/Cells", 1,
			std::to_string(design.getNumInstances(Rsyn::CELL)));
	tree.add("Instances/Ports", 1,
			std::to_string(design.getNumInstances(Rsyn::PORT)));
	tree.add("Instances/Modules", 1,
			std::to_string(design.getNumInstances(Rsyn::MODULE)));

	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
	if (physicalDesign) {
		tree.add("Floorplan/Rows", 1,
				std::to_string(physicalDesign.getNumRows()));
		tree.add("Floorplan/Layers", 1,
				std::to_string(physicalDesign.getNumLayers()));
		tree.add("Floorplan/Layers/Routing", 1,
				std::to_string(physicalDesign.getNumLayers(Rsyn::ROUTING)));
		tree.add("Floorplan/Layers/Cut", 1,
				std::to_string(physicalDesign.getNumLayers(Rsyn::CUT)));
	} // end if
	
	QtUtils::populateQTreeWidget(tree, treeWidgetDesignProperties);
} // end method

// -----------------------------------------------------------------------------

void MainWindow::populateObjectVisibility() {
	std::vector<std::tuple<std::string, bool>> objects;
	std::map<std::string, QIcon> icons;

	QtUtils::QTreeDescriptor tree;

	tree.addHeader(0, "Object");
	tree.addHeader(1, "", QIcon(":/glyphicons/png/glyphicons-52-eye-open.png"));
	
	tree.add("Highlight", 1, "", QtUtils::Checked);
	tree.add("Instances", 1, "", QtUtils::Checked);
	tree.add("Instances/Cells", 1, "", QtUtils::Checked);
	tree.add("Instances/Macros", 1, "", QtUtils::Checked);
	tree.add("Pins", 1, "", QtUtils::Checked);
	tree.add("Tracks", 1, "", QtUtils::Unchecked);
	tree.add("Tracks/Preferred", 1, "", QtUtils::Checked);
	tree.add("Tracks/Non-Preferred", 1, "", QtUtils::Unchecked);
	tree.add("Routing Guides", 1, "", QtUtils::Unchecked);
	tree.add("Routing", 1, "", QtUtils::Checked);

	for (const GraphicsLayerDescriptor &dscp : clsGraphicsLayerDescriptors) {
		tree.add(dscp.getName(), 1, "", dscp.getVisible()?
			QtUtils::Checked : QtUtils::Unchecked);
	} // end method

	QtUtils::populateQTreeWidget(tree, treeWidgetObjectVisibility);
} // end method

// -----------------------------------------------------------------------------

void MainWindow::populateLayerVisibility() {
	std::vector<std::tuple<std::string, bool>> objects;
	std::map<std::string, QIcon> icons;

	QtUtils::QTreeDescriptor tree;

	tree.addHeader(0, "Layer");
	tree.addHeader(1, "", QIcon(":/glyphicons/png/glyphicons-52-eye-open.png"));

	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
	Rsyn::Graphics *rsynGraphics = session.getService("rsyn.graphics");
	for (Rsyn::PhysicalLayer layer : physicalDesign.allPhysicalLayers()) {
		const std::string &key = layer.getName();

		QIcon icon;
		if (layer.getType() == Rsyn::ROUTING) {
			const FillStippleMask &stipple = rsynGraphics->getRoutingLayerRendering(layer.getRelativeIndex()).getFillPattern();
			const QColor &color = QtUtils::convert(rsynGraphics->getRoutingLayerRendering(layer.getRelativeIndex()).getColor());

			QPen pen;
			pen.setColor(color);
			pen.setCosmetic(true);

			QBrush brush = GraphicsStippleMgr::get()->getBrush(stipple);
			brush.setColor(color);

			icon = QtUtils::createIcon(pen, brush, 16, 16);
		} else {
			QPen pen;
			pen.setColor(Qt::black);
			pen.setCosmetic(true);

			QBrush brush;

			icon = QtUtils::createIcon(pen, brush, 16, 16);
		} // end if

		tree.add(key, 1, "", QtUtils::Checked, icon);
	} // end for

	QtUtils::populateQTreeWidget(tree, treeWidgetLayerVisibility);
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::createOverlays() {
	// Clean-up
	for (GraphicsOverlay *overlay : clsOverlays) {
		delete overlay;
	} // end for
	clsOverlays.clear();

	// Create default overlays.
	clsOverlays.push_back(new HighlightOverlay(nullptr));
	clsOverlays.push_back(new FloorplanOverlay(nullptr));
	clsOverlays.push_back(new InstanceOverlay(nullptr));
	clsOverlays.push_back(new RoutingOverlay(nullptr));
	clsOverlays.push_back(new RoutingGuideOverlay(nullptr));

	// **temp**
	view->setHighlightOverlay(
			(HighlightOverlay *) clsOverlays.front());

	// Create user overlays.
	if (clsRegisteredOverlays) {
		RegisteredOverlayVec &registeredOverlayVec = *clsRegisteredOverlays;
		for (auto &f : registeredOverlayVec) {
			clsOverlays.push_back(f());
		} // end for
	} // end if
} // end method

// -----------------------------------------------------------------------------

void 
MainWindow::keyPressEvent(QKeyEvent * event) {
	if (event->key() == Qt::Key_Control) {
		//viewport->setDragMode(QGraphicsView::RubberBandDrag);
	}
	QMainWindow::keyPressEvent(event);
} // end method

// -----------------------------------------------------------------------------

void 
MainWindow::keyReleaseEvent(QKeyEvent * event) {
	if (event->key() == Qt::Key_Control) {
		//viewport->setDragMode(QGraphicsView::ScrollHandDrag);
	} // end if
	QMainWindow::keyReleaseEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onExit() {
	close();
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onUpdateLog() {
	Rsyn::StreamLogger *streamLogger = Rsyn::StreamLogger::get();

	if (clsNextLogLineIndex < streamLogger->getLastLineIndex()) {
		QScrollBar *scrollBar = textLog->verticalScrollBar();
		const bool autoScroll = scrollBar->value() == scrollBar->maximum();

		int nextLine = -1;
		const std::string &log = streamLogger->getLog(clsNextLogLineIndex, nextLine);
		clsNextLogLineIndex = nextLine;
		//std::cerr << "Log: " << log << "\n";

		textLog->moveCursor(QTextCursor::End);
		textLog->appendPlainText(QString::fromStdString(log));
		if (autoScroll) {
			scrollBar->setValue(scrollBar->maximum());
		} // end if
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onRunScript() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Rsyn Script"),
			"",
			tr("Rsyn Script (*.rsyn)"));

	if (!fileName.isEmpty()) {
		runRsynCommand("source \"" + fileName.toStdString() + "\"");
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onChangeObjectVisibility(QTreeWidgetItem *item, int column) {
	const bool visible = item->checkState(1) == Qt::Checked;

	std::string key;
	std::vector<std::string> names;

	QTreeWidgetItem *current = item;
	do {
		names.push_back(current->text(0).toStdString());
		current = current->parent();
	} while (current);

	bool first = true;
	for (int i = (int) names.size() - 1; i >=0; i--) {
		if (!first) key += "/"; else first = false;
		key += names[i];
	} // end for

	view->setVisibility(key, visible);
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onChangeLayerVisibility(QTreeWidgetItem *item, int column) {
	const std::string &layerName = item->text(0).toStdString();
	const bool visible = item->checkState(1) == Qt::Checked;

	view->setPhysicalLayerVisibility(layerName, visible);
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onUnselectAllRoutingLayers() {
	for (int i = 0; i < treeWidgetLayerVisibility->topLevelItemCount(); i++) {
		QTreeWidgetItem *item = treeWidgetLayerVisibility->topLevelItem(i);
		item->setCheckState(1, Qt::Unchecked);
	} // end for
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onZoomIn() {
	if (view)
		view->zoomIn();
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onZoomOut() {
	if (view)
		view->zoomOut();
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onZoomToFit() {
	if (view)
		view->resetView();
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onSaveSnapshot() {
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Snapshot"), "",
        tr("Portable Network Graphics (*.png)"));

	if (!fileName.endsWith(".png")) {
		fileName.append(".png");
	} // end if

	if (!fileName.isEmpty()) {
		QPixmap pixMap = view->grab();
		pixMap.save(fileName);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onToggleOpenGL(bool enable) {
	view->toggleOpenGL(enable);
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onToggleConsole(bool enable) {
	mainWidget->layout()->setEnabled(false);
	if (enable) {
		frame->hide();
		textLog->setMaximumHeight(16777215);
	} else {
		frame->show();
		textLog->setMaximumHeight(textLog->minimumHeight());
		textLog->verticalScrollBar()->setValue(
				textLog->verticalScrollBar()->maximum());
	} // end else
	mainWidget->layout()->setEnabled(true);
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onExecuteCommand() {
	Rsyn::Session session;

	const std::string cmd = txtCommand->text().toStdString();

	// Skip if the command is blank.
	if (cmd.find_first_not_of(' ') == std::string::npos)
		return;

	try {
		// Execute the command.
		runRsynCommand(cmd);

		//  Store command in the history.
		const ScriptParsing::CommandManager &commandManager =
			session.getCommandManager();

		const int index = commandManager.currentHistoryCommandIndex();
		if (index >= 0) {
			const std::string &currentCommand = commandManager.currentHistoryCommand();
			const std::string &previousCommand = index > 0?
				commandManager.getHistory(index - 1) : "";

			if (currentCommand != previousCommand) {
				storeCommandHistory(currentCommand);
			} // end if
		} // end if
	} catch (const std::exception &e) {
		std::cout << "EXCEPTION: " << e.what() << "\n";
	} // end catch

	txtCommand->clear();
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onSearch() {
	Rsyn::Session session;
	Rsyn::Design design = session.getDesign();

	const std::string objectName = txtSearch->text().toStdString();

	bool success = false;

	Rsyn::Instance instance = design.findInstanceByName(objectName);
	if (instance) {
		success = true;
	} else {
		Rsyn::Net net = design.findNetByName(objectName);
		if (net) {
			success = true;
		} // end if
	} // end else

	if (success) {
		std::cout << "TODO: Object '" << objectName << "' found.\n";
		txtSearch->clear();
	} else {
		std::cout << "Object '" << objectName << "' not found.\n";
	} // end else
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onRunFlow() {
	const std::string flowName = comboBoxFlow->currentText().toStdString();

	Rsyn::Session session;
	if (flowName == "ICCAD15") {
		runRsynCommand("run \"ufrgs.ISPD16Flow\"");
	} else {
		std::cout << "TODO: Run flow " << flowName << ".\n";
	} // end else
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onUpdateRoutingEstimation() {
	Rsyn::Session session;
	Rsyn::RoutingEstimator *routingEstimator =
			session.getService("rsyn.routingEstimator", Rsyn::SERVICE_OPTIONAL);

	if (routingEstimator) {
		routingEstimator->updateRouting();
	} else {
		std::cout << "Routing estimator is not started.\n";
	} // end else
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onUpdateTiming() {
	Rsyn::Session session;
	Rsyn::Timer *timer =
			session.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);

	if (timer) {
		timer->updateTimingIncremental();
	} else {
		std::cout << "Timer is not started.\n";
	} // end else
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onRunGlobalPlacement() {
	std::cout << "TODO: onRunGlobalPlacement\n";
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onRunLegalization() {
	std::cout << "TODO: onRunLegalization\n";
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onRunDetailedPlacement() {
	std::cout << "TODO: onRunDetailedPlacement\n";
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onRunGlobalRouting() {
	std::cout << "TODO: onRunGlobalRouting\n";
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::onRunDetailedRouting() {
	std::cout << "TODO: onRunDetailedRouting\n";
} // end method

// -----------------------------------------------------------------------------

bool
MainWindow::eventFilter(QObject* obj, QEvent *event) {
    if (obj == txtCommand)  {
		Rsyn::Session session;

		ScriptParsing::CommandManager &commandManager =
			session.getCommandManager();

        if (event->type() == QEvent::KeyPress) {
			bool historyChanged = false;

            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Up) {
				txtCommand->setText(QString::fromStdString(commandManager.previousHistory()));
				historyChanged = true;
            } else if (keyEvent->key() == Qt::Key_Down) {
                txtCommand->setText(QString::fromStdString(commandManager.nextHistory()));
				historyChanged = true;
            } else if (keyEvent->key() == Qt::Key_Escape) {
				txtCommand->clear();
			} // end if

			if (historyChanged) {
				txtCommand->setCursorPosition(txtCommand->text().length());
				return true;
			} // end if
        } // end if
        return false;
    } // end if
    return QMainWindow::eventFilter(obj, event);
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::runRsynCommand(const std::string& cmd) {
	scene->update();
	setEnabled(false);
	
	progressBar->setMaximum(0);
    WorkerThread *workerThread = new WorkerThread(cmd);
    connect(workerThread, &WorkerThread::resultReady, this, &MainWindow::handleResults);
    connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
} // end method

// -----------------------------------------------------------------------------

void
MainWindow::handleResults(const QString &) {
	{ // @todo move to onDesignLoaded event...
		Rsyn::Session session;
		if (!view->isInitialized() && session.isDesignLoaded()) {
			Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

			view->setVisibility("Floorplan", true);
			view->setVisibility("Instances", true);
			view->setVisibility("Instances.Cells", true);
			view->setVisibility("Instances.Macros", true);
			view->setVisibility("Instances.Preferred", true);
			view->setVisibility("Routing", true);
			view->setVisibility("Routing Guides", true);

			for (Rsyn::PhysicalLayer layer : physicalDesign.allPhysicalLayers()) {
				view->setPhysicalLayerVisibility(layer, true);
			} // end for

			populate();
		} // end if
	} // end block

	progressBar->setMaximum(100);
	progressBar->setValue(100);
	setEnabled(true);
} // end method

// -----------------------------------------------------------------------------

bool
MainWindow::initOverlayRecursive(GraphicsOverlay *overlay, std::vector<GraphicsLayerDescriptor> &visibilityItems) {
	bool success = overlay->init(view, visibilityItems);
	for (GraphicsOverlay *child : overlay->allChildren()) {
		std::vector<GraphicsLayerDescriptor> childVisibilityItems;
		success &= initOverlayRecursive(child, childVisibilityItems);
		visibilityItems.insert(std::end(visibilityItems),
				std::begin(childVisibilityItems), std::end(childVisibilityItems));
	} // end for
	return success;
} // end method

// -----------------------------------------------------------------------------

} // end namespae