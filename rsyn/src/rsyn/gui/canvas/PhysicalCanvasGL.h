/* Copyright 2014-2017 Rsyn
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
 
#ifndef PHYSICAL_CANVAS_GL_H
#define	PHYSICAL_CANVAS_GL_H

#include <map>
#include <vector>

#include "rsyn/gui/CanvasGL.h"
#include "rsyn/engine/Engine.h"
#include "rsyn/util/Color.h"
#include "rsyn/3rdparty/json/json.hpp"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/model/timing/Timer.h"

#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"
#include "rsyn/util/FloatRectangle.h"

namespace Rsyn {
class RoutingEstimator;
}

// -----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(myEVT_CELL_SELECTED, -1)
DECLARE_EVENT_TYPE(myEVT_BIN_SELECTED, -1)
DECLARE_EVENT_TYPE(myEVT_CELL_DRAGGED, -1)
BEGIN_DECLARE_EVENT_TYPES()

// -----------------------------------------------------------------------------

class PhysicalCanvasGL;

class CanvasOverlay {
public:
	virtual bool init(PhysicalCanvasGL * canvas) = 0;
	virtual void render(PhysicalCanvasGL * canvas) = 0;
	virtual void config(const nlohmann::json &params) = 0;
}; // end class

// -----------------------------------------------------------------------------

class PhysicalCanvasGL : public CanvasGL {
public:
	
	static const float LAYER_BACKGROUND;
	static const float LAYER_OBSTACLES;
	static const float LAYER_GRADIENTS;
	static const float LAYER_MOVABLE;
	static const float LAYER_GRID;
	static const float LAYER_METAL;
	static const float LAYER_FOREGROUND;
	
//**************TEMP**************
	float2 clsMousePosition;

	
private:
	Rsyn::Engine clsEnginePtr;

	Rsyn::Timer * timer;
	Rsyn::RoutingEstimator * routingEstimator;
	Rsyn::PhysicalService *physical;
	Rsyn::Design design;
	Rsyn::Module module;
	Rsyn::PhysicalDesign phDesign;

	// Overlays
	struct CanvasOverlayConfiguration {
		CanvasOverlay * overlay;
		std::string name;
		bool visible;

		CanvasOverlayConfiguration() :
		overlay(nullptr), visible(false) {}
	}; // end struct

	std::vector<CanvasOverlayConfiguration> clsOverlays;
	std::map<std::string, int> clsOverlayMapping;
	
	// Enable info canvas. TODO: Remove these variable as this should be guessed
	// by accessing some engine method.
	bool clsEnableLegalizerInfo;
	
	// Show/Hide.
	bool clsViewGrid;
	bool clsViewWarpedGrid;
	bool clsViewCoreBounds;
	bool clsViewGradients;
	bool clsViewRows;
	bool clsViewOccupancyRate;
	bool clsViewPoissonSolution;
	bool clsViewDemandMap;
	bool clsViewSupplyMap;
	bool clsViewNets;	
	bool clsViewEarlyCriticalPath;
	bool clsViewLateCriticalPath;
	bool clsViewCriticalNets;
	bool clsViewFanoutTrees;
	bool clsViewFaninTrees;
	bool clsViewRenderBlockages;
	bool clsInterpolatedSnapshot;
	bool clsViewDisplacement;
	
	bool clsViewSelectedNodeNets;
	bool clsViewSelectedNodeNeighbours;
	bool clsViewSelectedNodeOptimalRegion;
	
	float clsInterpolationValue;
	
	// Stores the current selected cell (maybe null). The selected cell offset
	// is used to allow traverse thru cells at same position. For instance,
	// if several cells overlap, every time a different cell is returned.	
	Rsyn::Cell clsSelectedCell;
	bool clsSelectedCellDragging;
	bool clsSelectedCellDirty;	
	unsigned int clsSelectedCellOffset;
	
	int clsSelectedBinIndex;
    
	// Path width.
	float clsCriticalPathWidth;

private:
	DBUxy clsDeltaXY;

	Rsyn::Attribute<Rsyn::Instance, DBUxy> clsCheckpoint;
	std::vector<std::vector<Rsyn::Timer::PathHop>> 
			clsPaths[Rsyn::NUM_TIMING_MODES];
	
	    
	void renderCriticalPath_Mode(
			const Rsyn::TimingMode mode,
			const GLubyte r,
			const GLubyte g,
			const GLubyte b
	);
	
	void renderPath(const std::vector<Rsyn::Timer::PathHop> &path);
	float getGraphicalPinWidth() const {
		return phDesign.getRowHeight() / 7;
	}

	float getGraphicalPinHeight() const {
		return phDesign.getRowHeight() / 14;
	}

	FloatRectangle getGraphicalPinBoundingBox(Rsyn::Pin pin) {
		const DBUxy pos = phDesign.getPinPosition(pin);
		const float hw = getGraphicalPinWidth() / 2.0f;
		const float hh = getGraphicalPinHeight() / 2.0f;
		return FloatRectangle(pos.x - hw, pos.y - hh, pos.x + hw, pos.y + hh);
	} // end method
	
	void drawArrow( const float x0, const float y0, const float x1, const float y1, const float z );
	void drawPin(Rsyn::Pin pin);
	
	void render(const int width = 0, const int height = 0);
	void renderTimerInfo();
	
	void renderCoreBounds();
	void renderInterpolatedCells();
	void renderRows();
	void renderCriticalPath();
	void renderPaths(Rsyn::TimingMode mode);
	void renderCriticalNets();
	void renderSelectedCell();
	void renderTree();
	void renderBlockages();
	void renderDisplacementLines();
	
	void reset();
	
	inline 
	float2 interpolate(const DBUxy p0, const DBUxy p1, const float alpha) const {
		return float2(
				(alpha)*p1.x + (1 - alpha)*p0.x,
				(alpha)*p1.y + (1 - alpha)*p0.y);
	} // end method
	
public:
	PhysicalCanvasGL(wxWindow* parent);

	virtual void onRender(wxPaintEvent& evt);

	void attachEngine(Rsyn::Engine engine);

	void SelectCell(Rsyn::Cell cellIndex );

	void SelectBin(const int binIndex);
	
	
//**************TEMP**************	
	float2 getMousePosition();
//**************TEMP**************

	// Selects one of the cells at (x, y) considering the interpolated cells' 
	// positions. If multiple cells overlap at (x, y), every time a different
	// cell is returned unless keepSelection is true, in which case the priority
	// is given by the already selected cell.
	Rsyn::Cell selectCellAt(const float x, const float y, const bool keepSelection);
	
	// Get the displacement from original and current (interpolated) positions.
	float2
	getInterpolatedDisplacement(Rsyn::Instance cell) const {
		Rsyn::PhysicalInstance phInstance = phDesign.getPhysicalInstance(cell);
		const DBUxy p0 = phInstance.getPosition();
		const DBUxy p1 = clsCheckpoint[cell];
		return interpolate(p0, p1, clsInterpolationValue) - float2(p0);
	} // end method	
	
	// Centralize the camera at the selected node.
	void CentralizeAtSelectedCell();
	
	// Interpolated view mode
	void setInterpolationValue( const float value ) { clsInterpolationValue = value; Refresh(); }
	void storeCheckpoint();
	
	// Sets.
	void SetViewGrid( const bool visible ) { clsViewGrid = visible; }
	void SetViewWarpedGrid( const bool visible ) { clsViewWarpedGrid = visible; }
	void SetViewRows(const bool visible) { clsViewRows = visible; }
	void SetViewGradients( const bool visible ) { clsViewGradients = visible; }
	void SetViewOccupancyRate( const bool visible ) { clsViewOccupancyRate = visible; }
	void SetViewPoissonSolution( const bool visible ) { clsViewPoissonSolution = visible; }
	void SetViewDemandMap( const bool visible ) { clsViewDemandMap = visible; }
	void SetViewSupplyMap( const bool visible ) { clsViewSupplyMap = visible; }
	void SetViewNets( const bool visible ) { clsViewNets = visible; }	
	void SetViewLateCriticalPath( const bool visible ) { clsViewLateCriticalPath = visible; }
	void SetViewEarlyCriticalPath( const bool visible ) { clsViewEarlyCriticalPath = visible; }
	void SetViewCriticalNets( const bool visible ) { clsViewCriticalNets = visible; }
	void SetViewFaninTrees( const bool visible ) { clsViewFaninTrees = visible; }
	void SetViewFanoutTrees( const bool visible ) { clsViewFanoutTrees = visible; }
	void SetViewRenderBlockages ( const bool visible ) { clsViewRenderBlockages = visible; }
	void SetInterpolatedSnapshot ( const bool visible ) { clsInterpolatedSnapshot = visible; }
	void SetViewDisplacement( const bool visible ) { clsViewDisplacement = visible; }

	// Gets.
	Rsyn::Cell getSelectedCell() { return clsSelectedCell; }
	int getSelectedBin() const {return clsSelectedBinIndex; }

	// Paths
	void storePaths(const Rsyn::TimingMode mode, const 
		std::vector<std::vector<Rsyn::Timer::PathHop>> &paths) {
		clsPaths[mode] = paths;
		Refresh();
	} // end method
	
	void clearPaths(const Rsyn::TimingMode mode) {
		clsPaths[mode].clear();
		Refresh();
	} // end method
	
	void updateTimingOfPaths(const Rsyn::TimingMode mode) {
		Rsyn::Timer *timer = clsEnginePtr.getService("rsyn.timer");
		for (auto &path : clsPaths[mode]) {
			timer->updatePath(path);
		} // end for
		Refresh();
	} // end method
	
	void setCriticalPathWidth( const float width ){
		clsCriticalPathWidth = width; 
		Refresh();
	} // end method		
	
	// Events.
	virtual void onMouseReleased(wxMouseEvent& event);
	virtual void onMouseMoved(wxMouseEvent& event);
	virtual void onMouseDown(wxMouseEvent& event);
	
	// Add an overlay to be rendered.
	void addOverlay(const std::string &name, CanvasOverlay * overlay, const bool visible) {
		auto it = clsOverlayMapping.find(name);
		if (it != clsOverlayMapping.end()) {
			std::cout << "WARNING: Overlay '" << name << "' already registered. Skipping...\n";
		} else {
			const std::size_t index = clsOverlays.size() + 1;
			clsOverlayMapping[name] = (int) index;
			clsOverlays.resize(index + 1);
			CanvasOverlayConfiguration &config = clsOverlays.back();
			config.name = name;
			config.overlay = overlay;
			config.visible = visible;
		} // end else
	} // end method
	
	// Show/hide an overlay.
	void setOverlayVisibility(const std::string &name, const bool visible) {
		auto it = clsOverlayMapping.find(name);
		if (it == clsOverlayMapping.end()) {
			std::cout << "WARNING: Overlay '" << name << "' is not registered. Skipping...\n";
		} else {
			CanvasOverlayConfiguration &config = clsOverlays[it->second];
			config.visible = visible;
		} // end else		
	} // end method
	
	Rsyn::Engine getEngine() { return clsEnginePtr; }
	
	// TODO move to CanvasGL
	void saveSnapshot(wxImage& image);
	
};


#endif
