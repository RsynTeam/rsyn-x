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
#include "rsyn/qt/graphics/infra/GraphicsLayer.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"

extern const bool UseAlternativeSceneMgr;

namespace Rsyn {
class Graphics;

class GraphicsScene;
class GraphicsView;

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

class MainWindow : public QMainWindow, public Ui::MainWindow, public Rsyn::SessionObserver {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

	// Rsyn::Session notifications.
	virtual void onDesignLoaded() override;

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
	void onShowConnectivity(bool enable);
	void onShowRoutingGuides(bool enable);
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

	void loadCommandHistory();
	void saveCommandHistory();

	void createCommands();

    void setupMatrix();
    void populate();
	void populateScene();

	void populateDesignProperties();
	void populateObjectVisibility();
	void populateLayerVisibility();

	void runRsynCommand(const std::string &cmd);
	
	Rsyn::Graphics *rsynGraphics = nullptr;

    GraphicsScene *scene = nullptr;
	GraphicsView *view = nullptr;

	std::vector<GraphicsLayerDescriptor> clsGraphicsLayerDescriptors;

	QSettings clsSettings;
	int clsCurrentCommandHistoryIndex = 0;

	int clsNextLogLineIndex = 0;
}; // end clas

} // end namespace

#endif
