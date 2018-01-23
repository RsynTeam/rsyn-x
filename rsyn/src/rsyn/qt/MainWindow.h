#ifndef RSYN_QT_MAIN_WINDOW_H
#define RSYN_QT_MAIN_WINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QThread>
#include "ui_MainWindow.h"

#include <iostream>
#include <functional>
#include <vector>

#include "rsyn/session/SessionObserver.h"
#include "rsyn/qt/infra/GraphicsOverlay.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"

namespace Rsyn {
class Graphics;

class GraphicsScene;
class GraphicsView;
class GraphicsOverlay;

// -----------------------------------------------------------------------------

//! @bried Worker to run Rsyn commands in a separate thread allowing the
//! interface to stay responsive.
class WorkerThread : public QThread {
	Q_OBJECT
public:
	WorkerThread(const std::string &cmd) : command(cmd) {}
	void run() override;
signals:
	void resultReady(const QString &s);

private:
	std::string command;
}; // end class

// -----------------------------------------------------------------------------

class ObjectVisibilityItem;

class MainWindow : public QMainWindow, public Ui::MainWindow, public Rsyn::SessionObserver {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);

	// Rsyn::Session notifications
	virtual void onDesignLoaded() override;

	template<typename T>
	static void registerOverlay();

protected:

	virtual void keyPressEvent(QKeyEvent * event) override;
	virtual void keyReleaseEvent(QKeyEvent * event) override;

public slots:

	void onExit();
	void onUpdateLog();
	void onRunScript();
	void onChangeObjectVisibility(QTreeWidgetItem* item, int column);
	void onChangeLayerVisibility(QTreeWidgetItem* item, int column);
	void onUnselectAllRoutingLayers();
	void onZoomIn();
	void onZoomOut();
	void onZoomToFit();
	void onSaveSnapshot();
	void onToggleOpenGL(bool enable);
	void onToggleConsole(bool enable);
	void onExecuteCommand();
	void onSearch();
	void onRunFlow();
	void onUpdateRoutingEstimation();
	void onUpdateTiming();
	void onRunGlobalPlacement();
	void onRunLegalization();
	void onRunDetailedPlacement();
	void onRunGlobalRouting();
	void onRunDetailedRouting();

	void handleResults(const QString &);

	bool eventFilter(QObject* obj, QEvent *event);

private:

	void restoreCommandHistory();
	void storeCommandHistory(const std::string &command);

    void setupMatrix();
    void populate();
	void populateDesignProperties();
	void populateObjectVisibility();
	void populateLayerVisibility();

	void runRsynCommand(const std::string &cmd);

	void createOverlays();
	bool initOverlayRecursive(GraphicsOverlay *overlay, std::vector<GraphicsLayerDescriptor> &visibilityItems);
	
	Rsyn::Graphics *rsynGraphics = nullptr;

    GraphicsScene *scene = nullptr;
	GraphicsView *view = nullptr;

	std::vector<GraphicsOverlay *> clsOverlays;
	std::vector<GraphicsLayerDescriptor> clsGraphicsLayerDescriptors;

	QSettings clsSettings;
	int clsCurrentCommandHistoryIndex = 0;

	int clsNextLogLineIndex = 0;

	// Generic functions used to instantiate overlays.
	typedef std::function<GraphicsOverlay *()> OverlayInstantiatonFunction;
	typedef std::vector<OverlayInstantiatonFunction> RegisteredOverlayVec;
	static RegisteredOverlayVec *clsRegisteredOverlays;
}; // end class

// -----------------------------------------------------------------------------

template<typename T>
void
MainWindow::registerOverlay() {
	if (!clsRegisteredOverlays) {
		// Try to avoid "static variable initialization fiasco".
		clsRegisteredOverlays = new RegisteredOverlayVec();
	} // end if

	RegisteredOverlayVec &registeredOverlayVec = *clsRegisteredOverlays;

	registeredOverlayVec.push_back([]() -> GraphicsOverlay *{
		return new T();
	});
} // end method

} // end namespace

#endif
