/* Copyright 2014-2018 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <exception>

#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "io/Graphics.h"
#include "util/StreamLogger.h"

#include "tool/routing/RoutingEstimator.h"
#include "tool/timing/Timer.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QString>
#include <QKeyEvent>
#include <QScrollBar>
#include <QThread>
#include <QStatusBar>

#include "MainWindow.h"

#include "gui/graphics/GraphicsStipple.h"
#include "gui/graphics/GraphicsScene.h"
#include "gui/graphics/GraphicsView.h"
#include "gui/graphics/GraphicsLayer.h"
#include "gui/graphics/GraphicsItem.h"

#include "gui/graphics/LayoutGraphicsScene.h"

#include "gui/QtUtils.h"
#include "gui/objects/PinMgr.h"
#include "gui/graphics/LayoutGraphicsScene.h"
#include "gui/graphics/LayoutGraphicsView.h"

const bool UseAlternativeSceneMgr = true;

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

void WorkerThread::run() {
        QString result;

        try {
                Rsyn::Session session;
                session.evaluateString(command);
        } catch (std::exception &e) {
                std::cout << "EXCEPTION: " << e.what() << "\n";
        } catch (...) {
                std::cout << "EXCEPTION: "
                          << "unknown"
                          << "\n";
        }  // end catch

        emit resultReady(result);
}  // end method

// -----------------------------------------------------------------------------

static const int MAX_COMMAND_HISTORY_LENGTH = 100;

// -----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), view(nullptr), scene(nullptr) {
        setupUi(this);
        statusBar()->show();

        onUpdateLog();
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(onUpdateLog()));
        timer->start(1000);

        GraphicsStippleMgr::create();

        txtCommand->installEventFilter(this);

        view = new GraphicsView(this);
        view->window()->layout()->setMargin(0);
        view->activateWindow();
        view->setFocus();

        gridLayout->addWidget(view, 0, 0, 1, 1);

        loadCommandHistory();

        // Register to receive notifications about session changes.
        Rsyn::Session session;
        session.registerObserver(this);
}  // end constructor

// -----------------------------------------------------------------------------

MainWindow::~MainWindow() { saveCommandHistory(); }  // end destructor

// -----------------------------------------------------------------------------

void MainWindow::onDesignLoaded() {
        clsNotificationDesignLoaded = true;
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onServiceStarted(const std::string &serviceName) {
        clsNotificationServiceStarted = true;
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::loadCommandHistory() {
        Rsyn::Session session;

        ScriptParsing::CommandManager &commandManager =
            session.getCommandManager();

        std::cout << "Settings: " << clsSettings.fileName().toStdString()
                  << "\n";

        clsSettings.beginReadArray("CommandHistory");
        for (int i = 0; i < MAX_COMMAND_HISTORY_LENGTH; i++) {
                clsSettings.setArrayIndex(i);

                const std::string &cmd =
                    clsSettings.value("command").toString().toStdString();
                if (!cmd.empty()) {
                        commandManager.pushHistory(cmd);
                }
        }  // end for
        clsSettings.endArray();
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::saveCommandHistory() {
        Rsyn::Session session;
        const ScriptParsing::CommandManager &commandManager =
            session.getCommandManager();

        const int length = commandManager.getHistorySize();
        const int start = std::max(0, length - MAX_COMMAND_HISTORY_LENGTH);

        clsSettings.beginWriteArray("CommandHistory");
        int counter = 0;
        for (int i = start; i < length; i++, counter++) {
                clsSettings.setArrayIndex(counter);
                const std::string &cmd = commandManager.getHistory(i);
                if (!cmd.empty()) {
                        clsSettings.setValue("command",
                                             QString::fromStdString(cmd));
                }  // end if
        }          // end for
        clsSettings.endArray();
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::createCommands() {
        Rsyn::Session session;

        {  // Command: clearHighlight
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("clearHighlight");
                dscp.setDescription("Erase all highlights.");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            if (scene) {
                                    scene->clearHighlight();
                            }  // end if
                    });
        }  // end block

        {  // Command: clearDrawings
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("clearDrawings");
                dscp.setDescription("Erase all drawings.");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            if (scene) {
                                    scene->clearDrawing();
                            }  // end if
                    });
        }  // end block

        {  // Command: clearDrawings
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("drawFocusPoint");
                dscp.setDescription(
                    "Draw a circle around a point of interest.");

                dscp.addPositionalParam("x", ScriptParsing::PARAM_TYPE_INTEGER,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Position x of the point of interest.");

                dscp.addPositionalParam("y", ScriptParsing::PARAM_TYPE_INTEGER,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Position y of the point of interest.");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            if (scene) {
                                    Rsyn::Session session;
                                    Rsyn::PhysicalDesign physicalDesign =
                                        session.getPhysicalDesign();

                                    const QRect typicalRect(0, 0, 25, 25);
                                    const QRectF mappedTypicalRect =
                                        view->mapToScene(typicalRect)
                                            .boundingRect();

                                    const DBU xc = (int)command.getParam("x");
                                    const DBU yc = (int)command.getParam("y");

                                    const float w = mappedTypicalRect.width();
                                    const float h = mappedTypicalRect.height();

                                    const float x = xc - w / 2.0f;
                                    const float y = yc - h / 2.0f;

                                    const QRectF rect(x, y, w, h);

                                    QPainterPath drawing;
                                    // drawing.addRect(rect);

                                    drawing.moveTo(rect.x(), rect.y());
                                    drawing.lineTo(rect.x() + rect.width(),
                                                   rect.y() + rect.height());

                                    drawing.moveTo(rect.x(),
                                                   rect.y() + rect.height());
                                    drawing.lineTo(rect.x() + rect.width(),
                                                   rect.y());

                                    scene->addDrawing(drawing);
                                    scene->ensureVisible(rect);
                            }  // end if
                    });
        }  // end block

}  // end method

// -----------------------------------------------------------------------------

void MainWindow::populate() {
        Rsyn::Session session;
        rsynGraphics = session.getService("rsyn.graphics");

        QtPinMgr::create();

        populateScene();

        {  // Populate widgets
                populateObjectVisibility();
                populateLayerVisibility();
                populateDesignProperties();
        }  // end block

        {  // Initialize view
                Stepwatch watchOverlays("Initializing scene");
                view->setScene(scene);
                view->init();

                // TODO: Make the graphics view get the keyboard focus. The
                // following
                // is not working.
                view->activateWindow();
                view->setFocus();
        }  // end block

        createCommands();
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::populateScene() {
        LayoutGraphicsScene *layoutScene = new LayoutGraphicsScene();
        layoutScene->init();
        layoutScene->setStatusBar(statusBar());

        scene = layoutScene;
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::populateDesignProperties() {
        Rsyn::Session session;
        Rsyn::Design design = session.getDesign();
        if (!design) return;

        QtUtils::QTreeDescriptor tree;

        tree.addHeader(0, "Property");
        tree.addHeader(1, "Value");

        tree.add("Name", 1, design.getName());

        tree.add("Instances", 1, std::to_string(design.getNumInstances()));
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
                tree.add(
                    "Floorplan/Layers/Routing", 1,
                    std::to_string(physicalDesign.getNumLayers(Rsyn::ROUTING)));
                tree.add(
                    "Floorplan/Layers/Cut", 1,
                    std::to_string(physicalDesign.getNumLayers(Rsyn::CUT)));
        }  // end if

        QtUtils::populateQTreeWidget(tree, treeWidgetDesignProperties);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::populateObjectVisibility() {
        std::vector<std::tuple<std::string, bool>> objects;
        std::map<std::string, QIcon> icons;

        QtUtils::QTreeDescriptor tree;

        tree.addHeader(0, "Object");
        tree.addHeader(1, "",
                       QIcon(":/glyphicons/png/glyphicons-52-eye-open.png"));

        tree.add("Instances", 1, "", QtUtils::Checked);
        tree.add("Instances/Cells", 1, "", QtUtils::Checked);
        tree.add("Instances/Macros", 1, "", QtUtils::Checked);
        tree.add("Instances/Ports", 1, "", QtUtils::Checked);
        tree.add("Pins", 1, "", QtUtils::Checked);
        tree.add("Floorplan", 1, "", QtUtils::Checked);
        tree.add("Floorplan/Core Bounds", 1, "", QtUtils::Checked);
        tree.add("Floorplan/Rows", 1, "", QtUtils::Unchecked);
        tree.add("Floorplan/Sites", 1, "", QtUtils::Unchecked);
        tree.add("Floorplan/Regions", 1, "", QtUtils::Unchecked);
        tree.add("Tracks", 1, "", QtUtils::Unchecked);
        tree.add("Tracks/Preferred", 1, "", QtUtils::Checked);
        tree.add("Tracks/Non-Preferred", 1, "", QtUtils::Unchecked);
        tree.add("Routing Guides", 1, "", QtUtils::Unchecked);
        tree.add("Routing", 1, "", QtUtils::Checked);

        LayoutGraphicsScene *layoutScene =
            dynamic_cast<LayoutGraphicsScene *>(scene);
        for (const GraphicsLayerDescriptor &dscp :
             layoutScene->getVisibilityItems()) {
                tree.add(dscp.getName(), 1, "", dscp.getVisible()
                                                    ? QtUtils::Checked
                                                    : QtUtils::Unchecked);
        }  // end method

        treeWidgetObjectVisibility->clear();
        treeWidgetObjectVisibility->setEnabled(false);
        QtUtils::populateQTreeWidget(tree, treeWidgetObjectVisibility);
        treeWidgetObjectVisibility->setEnabled(true);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::populateLayerVisibility() {
        std::vector<std::tuple<std::string, bool>> objects;
        std::map<std::string, QIcon> icons;

        QtUtils::QTreeDescriptor tree;

        tree.addHeader(0, "Layer");
        tree.addHeader(1, "",
                       QIcon(":/glyphicons/png/glyphicons-52-eye-open.png"));

        Rsyn::Session session;
        Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
        Rsyn::Graphics *rsynGraphics = session.getService("rsyn.graphics");
        for (Rsyn::PhysicalLayer layer : physicalDesign.allPhysicalLayers()) {
                const std::string &key = layer.getName();

                QIcon icon;
                if (layer.getType() == Rsyn::ROUTING) {
                        const FillStippleMask &stipple =
                            rsynGraphics
                                ->getRoutingLayerRendering(
                                    layer.getRelativeIndex())
                                .getFillPattern();
                        const QColor &color =
                            QtUtils::convert(rsynGraphics
                                                 ->getRoutingLayerRendering(
                                                     layer.getRelativeIndex())
                                                 .getColor());

                        QPen pen;
                        pen.setColor(color);
                        pen.setCosmetic(true);

                        QBrush brush =
                            GraphicsStippleMgr::get()->getBrush(stipple);
                        brush.setColor(color);

                        icon = QtUtils::createIcon(pen, brush, 16, 16);
                } else {
                        QPen pen;
                        pen.setColor(Qt::black);
                        pen.setCosmetic(true);

                        QBrush brush;

                        icon = QtUtils::createIcon(pen, brush, 16, 16);
                }  // end if

                tree.add(key, 1, "", QtUtils::Checked, icon);
        }  // end for

        QtUtils::populateQTreeWidget(tree, treeWidgetLayerVisibility);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::keyPressEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Control) {
                // viewport->setDragMode(QGraphicsView::RubberBandDrag);
        }
        QMainWindow::keyPressEvent(event);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Control) {
                // viewport->setDragMode(QGraphicsView::ScrollHandDrag);
        }  // end if
        QMainWindow::keyReleaseEvent(event);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onExit() { close(); }  // end method

// -----------------------------------------------------------------------------

void MainWindow::onUpdateLog() {
        Rsyn::StreamLogger *streamLogger = Rsyn::StreamLogger::get();

        if (clsNextLogLineIndex < streamLogger->getLastLineIndex()) {
                QScrollBar *scrollBar = textLog->verticalScrollBar();
                const bool autoScroll =
                    scrollBar->value() == scrollBar->maximum();

                int nextLine = -1;
                const std::string &log =
                    streamLogger->getLog(clsNextLogLineIndex, nextLine);
                clsNextLogLineIndex = nextLine;
                // std::cerr << "Log: " << log << "\n";

                textLog->moveCursor(QTextCursor::End);
                textLog->appendPlainText(QString::fromStdString(log));
                if (autoScroll) {
                        scrollBar->setValue(scrollBar->maximum());
                }  // end if
        }          // end if
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onRunScript() {
        QString fileName = QFileDialog::getOpenFileName(
            this, tr("Open Rsyn Script"), "", tr("Rsyn Script (*.rsyn)"));

        if (!fileName.isEmpty()) {
                runRsynCommand("source \"" + fileName.toStdString() + "\"");
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onChangeObjectVisibility(QTreeWidgetItem *item, int column) {
        const bool visible = item->checkState(1) == Qt::Checked;
        std::string key;
        std::vector<std::string> names;

        QTreeWidgetItem *current = item;
        do {
                names.push_back(current->text(0).toStdString());
                current = current->parent();
        } while (current);

        bool first = true;
        for (int i = (int)names.size() - 1; i >= 0; i--) {
                if (!first)
                        key += "/";
                else
                        first = false;
                key += names[i];
        }  // end for

        scene->setVisibility(key, visible);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onChangeLayerVisibility(QTreeWidgetItem *item, int column) {
        const std::string &layerName = item->text(0).toStdString();
        const bool visible = item->checkState(1) == Qt::Checked;

        scene->setVisibility("PhysicalLayer/" + layerName, visible);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onUnselectAllRoutingLayers() {
        for (int i = 0; i < treeWidgetLayerVisibility->topLevelItemCount();
             i++) {
                QTreeWidgetItem *item =
                    treeWidgetLayerVisibility->topLevelItem(i);
                item->setCheckState(1, Qt::Unchecked);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onZoomIn() {
        if (view) view->zoomIn();
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onZoomOut() {
        if (view) view->zoomOut();
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onZoomToFit() {
        if (view) view->resetView();
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onShowConnectivity(bool enable) {
        std::cout << "TODO: show connectivity = " << enable << "\n";
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onShowRoutingGuides(bool enable) {
        std::cout << "TODO: show routing guides = " << enable << "\n";
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onSaveSnapshot() {
        QString fileName = QFileDialog::getSaveFileName(
            this, tr("Save Snapshot"), "",
            tr("Portable Network Graphics (*.png)"));

        if (!fileName.endsWith(".png")) {
                fileName.append(".png");
        }  // end if

        if (!fileName.isEmpty()) {
                QPixmap pixMap = view->grab();
                pixMap.save(fileName);
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onToggleOpenGL(bool enable) {
        view->toggleOpenGL(enable);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onToggleConsole(bool enable) {
        mainWidget->layout()->setEnabled(false);
        if (enable) {
                frame->hide();
                textLog->setMaximumHeight(16777215);
        } else {
                frame->show();
                textLog->setMaximumHeight(textLog->minimumHeight());
                textLog->verticalScrollBar()->setValue(
                    textLog->verticalScrollBar()->maximum());
        }  // end else
        mainWidget->layout()->setEnabled(true);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onExecuteCommand() {
        Rsyn::Session session;

        const std::string cmd = txtCommand->text().toStdString();

        // Skip if the command is blank.
        if (cmd.find_first_not_of(' ') == std::string::npos) return;

        try {
                // Execute the command.
                runRsynCommand(cmd);
        } catch (const std::exception &e) {
                std::cout << "EXCEPTION: " << e.what() << "\n";
        }  // end catch

        txtCommand->clear();
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onSearch() {
        Rsyn::Session session;
        Rsyn::Design design = session.getDesign();

        const std::string objectName = txtSearch->text().toStdString();

        bool success = false;

        // Temporary workaround while still have the two graphics
        // infrastructures.
        LayoutGraphicsScene *layoutScene =
            dynamic_cast<LayoutGraphicsScene *>(scene);
        if (!layoutScene) return;

        Rsyn::Instance instance = design.findInstanceByName(objectName);
        if (instance) {
                std::cout << "Instance " << instance.getName()
                          << " selected.\n";
                QRectF bounds;
                if (success = layoutScene->selectInstance(instance, bounds)) {
                        view->ensureVisible(bounds);
                } else {
                        std::cout
                            << "BUG: Unable to find instance in the scene.\n";
                }
        } else {
                Rsyn::Net net = design.findNetByName(objectName);
                if (net) {
                        QRectF bounds;
                        if (success = layoutScene->selectNet(net, bounds)) {
                                view->ensureVisible(bounds);
                        }  // end if
                }          // end if
        }                  // end else

        if (success) {
                txtSearch->clear();
        } else {
                std::cout << "Object '" << objectName << "' not found.\n";
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onRunFlow() {
        const std::string flowName = comboBoxFlow->currentText().toStdString();

        Rsyn::Session session;
        if (flowName == "ICCAD15") {
                runRsynCommand("run \"ufrgs.ISPD16Flow\"");
        } else {
                std::cout << "TODO: Run flow " << flowName << ".\n";
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onUpdateRoutingEstimation() {
        Rsyn::Session session;
        Rsyn::RoutingEstimator *routingEstimator =
            session.getService("rsyn.routingEstimator", Rsyn::SERVICE_OPTIONAL);

        if (routingEstimator) {
                routingEstimator->updateRouting();
        } else {
                std::cout << "Routing estimator is not started.\n";
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onUpdateTiming() {
        Rsyn::Session session;
        Rsyn::Timer *timer =
            session.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);

        if (timer) {
                timer->updateTimingIncremental();
        } else {
                std::cout << "Timer is not started.\n";
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onRunGlobalPlacement() {
        std::cout << "TODO: onRunGlobalPlacement\n";
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onRunLegalization() {
        std::cout << "TODO: onRunLegalization\n";
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onRunDetailedPlacement() {
        std::cout << "TODO: onRunDetailedPlacement\n";
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onRunGlobalRouting() {
        std::cout << "TODO: onRunGlobalRouting\n";
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::onRunDetailedRouting() {
        std::cout << "TODO: onRunDetailedRouting\n";
}  // end method

// -----------------------------------------------------------------------------

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
        if (obj == txtCommand) {
                Rsyn::Session session;

                ScriptParsing::CommandManager &commandManager =
                    session.getCommandManager();

                if (event->type() == QEvent::KeyPress) {
                        bool historyChanged = false;

                        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
                        if (keyEvent->key() == Qt::Key_Up) {
                                txtCommand->setText(QString::fromStdString(
                                    commandManager.previousHistory()));
                                historyChanged = true;
                        } else if (keyEvent->key() == Qt::Key_Down) {
                                txtCommand->setText(QString::fromStdString(
                                    commandManager.nextHistory()));
                                historyChanged = true;
                        } else if (keyEvent->key() == Qt::Key_Escape) {
                                txtCommand->clear();
                        } else if (keyEvent->key() == Qt::Key_Tab) {
                                const std::string cmd =
                                    txtCommand->text().toStdString();
                                const std::string remaining =
                                    commandManager.autoComplete(cmd);
                                txtCommand->setText(
                                    txtCommand->text() +
                                    QString::fromStdString(remaining));
                                historyChanged = true;
                        }  // end else-if

                        if (historyChanged) {
                                txtCommand->setCursorPosition(
                                    txtCommand->text().length());
                                return true;
                        }  // end if
                }          // end if
                return false;
        }  // end if
        return QMainWindow::eventFilter(obj, event);
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::runRsynCommand(const std::string &cmd) {
        setEnabled(false);

        progressBar->setMaximum(0);
        WorkerThread *workerThread = new WorkerThread(cmd);
        connect(workerThread, &WorkerThread::resultReady, this,
                &MainWindow::handleResults);
        connect(workerThread, &WorkerThread::finished, workerThread,
                &QObject::deleteLater);
        workerThread->start();
}  // end method

// -----------------------------------------------------------------------------

void MainWindow::handleResults(const QString &) {
        {  // @todo move to onDesignLoaded event...
                Rsyn::Session session;
                if (!view->isInitialized() && session.isDesignLoaded()) {
                        Rsyn::PhysicalDesign physicalDesign =
                            session.getPhysicalDesign();

                        populate();
                }  // end if
        }          // end block

        LayoutGraphicsScene *layoutScene =
            dynamic_cast<LayoutGraphicsScene *>(scene);

        if (layoutScene) {
                if (clsNotificationServiceStarted) {
                        if (layoutScene->initMissingUserGraphicsLayers()) {
                                populateObjectVisibility();
                        }  // end if
                }          // end if
                layoutScene->redrawScene();
        }  // end if

        // Clear notifications.
        clsNotificationDesignLoaded = false;
        clsNotificationServiceStarted = false;

        // Stops progress bar and re-enable the window.
        progressBar->setMaximum(100);
        progressBar->setValue(100);
        setEnabled(true);
        txtCommand->setFocus();
}  // end method

// -----------------------------------------------------------------------------

}  // end namespae
