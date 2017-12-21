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
#include "rsyn/gui/canvas/GeometryManager.h"

#include "rsyn/session/Session.h"
#include "rsyn/3rdparty/json/json.hpp"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/model/timing/Timer.h"

#include "rsyn/util/Color.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"
#include "rsyn/util/FloatRectangle.h"

namespace Rsyn {
class RoutingEstimator;
class Graphics;
}

// -----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(myEVT_CELL_SELECTED, -1)
DECLARE_EVENT_TYPE(myEVT_HOVER_OVER_OBJECT, -1)
DECLARE_EVENT_TYPE(myEVT_CELL_DRAGGED, -1)
BEGIN_DECLARE_EVENT_TYPES()

// -----------------------------------------------------------------------------

class PhysicalCanvasGL;

class CanvasOverlay {
public:
	virtual bool init(PhysicalCanvasGL * canvas, nlohmann::json& properties) = 0;
	virtual void render(PhysicalCanvasGL * canvas) = 0;
	virtual void renderInterpolated(PhysicalCanvasGL * canvas) {};
	virtual void config(const nlohmann::json &params) = 0;
}; // end class

// -----------------------------------------------------------------------------

class PhysicalCanvasGL : public CanvasGL, public Rsyn::PhysicalObserver {
public:
	
	static const float LAYER_BACKGROUND;
	static const float LAYER_OBSTACLES;
	static const float LAYER_GRADIENTS;
	static const float LAYER_MOVABLE;
	static const float LAYER_GRID;
	static const float LAYER_METAL;
	static const float LAYER_ROUTING_MIN;
	static const float LAYER_ROUTING_MAX;
	static const float LAYER_FOREGROUND;
	
	float2 clsMousePosition;
	
private:

	enum {
		POPUP_MENU_INVALID = -1,
		POPUP_MENU_PROPERTIES,
		POPUP_MENU_ADD_TO_HIGHLIGHT,
		POPUP_MENU_CLEAR_HIGHLIGHT,
		POPUP_MENU_ZOOM_TO_FIT
	}; // end enum

	// Session
	Rsyn::Session clsSession;

	// Services
	Rsyn::Timer * timer = nullptr;
	Rsyn::RoutingEstimator * routingEstimator = nullptr;
	Rsyn::Graphics * graphics = nullptr;
	Rsyn::PhysicalService * physical = nullptr;

	// Design
	Rsyn::Design design;
	Rsyn::Module module;
	Rsyn::PhysicalDesign phDesign;

	// Messages
	Rsyn::Message msgNoGlew;
	Rsyn::Message msgNoRenderToTexture;

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
	// by accessing some session method.
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

	bool clsInitialized = false;

	GeometryManager::LayerId geoCellLayerId;
	GeometryManager::LayerId geoMacroLayerId;
	GeometryManager::LayerId geoPortLayerId;
	GeometryManager::LayerId geoPinsLayerId;
	
	std::vector<int> techLayerIds;
	
	// We currently support up to 11 tech layer styles
	std::array<FillStippleMask, 11> techLayerMasks = {
		STIPPLE_MASK_DIAGONAL_DOWN_1,
		STIPPLE_MASK_DIAGONAL_UP_1,
		STIPPLE_MASK_DIAGONAL_DOWN_2,
		STIPPLE_MASK_DIAGONAL_UP_2,
		STIPPLE_MASK_DIAGONAL_DOWN_3,
		STIPPLE_MASK_DIAGONAL_UP_3,
		STIPPLE_MASK_DIAGONAL_DOWN_4,
		STIPPLE_MASK_DIAGONAL_UP_4,
		STIPPLE_MASK_DIAGONAL_DOWN_5,
		STIPPLE_MASK_DIAGONAL_UP_5,
		STIPPLE_MASK_CROSS
	}; 
	
	std::array<Color, 11> techLayerColors = {
		//Color(  0,   0,   0), // BLACK
		Color(  0,   0, 255), // BLUE
		Color(255,   0,   0), // RED
		Color(  0, 255,   0), // GREEN
		Color(230, 230,   0), // YELLOW, 
		Color( 42,  42, 165), // DARK_BLUE
		Color(165,  42,  42), // DARK_RED
		Color( 42, 165,  42), // DARK_GREEN
		Color(230,   0, 230), // PINK 
		Color(  0, 230, 230), // CYAN
		Color( 75,   0, 130), // INDIGO
		Color(128,   0, 128) // PURPLE
	};	

public:
	GeometryManager::LayerId getTechLayerID(const int id) { 
		if (id < 0 || id >= techLayerIds.size())
			return -1;		
		return techLayerIds[id];
	};
	
private:
	
	GeometryManager::ObjectId clsHoverObjectId;
	Rsyn::Attribute<Rsyn::Net, GeometryManager::GroupId> clsGeoNets;

	// Path width.
	float clsCriticalPathWidth;

	// Context menu
	wxMenu * clsContextMenu;

public:
	inline bool isOverlay(const std::string name) { 
		return (clsOverlayMapping.find(name) != clsOverlayMapping.end());
	} //end method
	
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
	void renderFocusedObject();
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

	void postDoubleClickCallback();
	void addToHighlight();

public:
	PhysicalCanvasGL(wxWindow* parent);

	virtual void onRender(wxPaintEvent& evt);
	virtual void onResized(wxSizeEvent& evt);

	void init();

	void SelectCell(Rsyn::Cell cellIndex );

	void SelectBin(const int binIndex);
	
	bool isPhysicalDesignInitialized() const {
		return physical != nullptr;
	}

	//! @brief Returns the mouse position in space coordinates.
	float2 getMousePosition() const { return clsMousePosition; }

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
	float getInterpolationValue() const { return clsInterpolationValue; }
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
		Rsyn::Timer *timer = clsSession.getService("rsyn.timer");
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
	virtual void onRightClick(wxMouseEvent& event);
	virtual void onLeftDoubleClick(wxMouseEvent& event);

	void onPopupClick(wxCommandEvent &evt);

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
	
	// TODO move to CanvasGL
	void saveSnapshot(wxImage& image);

	////////////////////////////////////////////////////////////////////////////
	// Shapes
	////////////////////////////////////////////////////////////////////////////
public:
	
	class GeoReference {
	friend class PhysicalCanvasGL;
	public:
		Rsyn::ObjectType getObjectType() const { return type; }
		void * getData() const { return data; }

	private:
		Rsyn::ObjectType type = Rsyn::OBJECT_TYPE_INVALID;
		void * data = nullptr;
	}; // end class

private:

	GeometryManager geoMgr;
	std::deque<GeoReference> geoReferences;

	GeoReference * createGeoReference(Rsyn::Instance instance);
	GeoReference * createGeoReference(Rsyn::Net net);
	GeoReference * createGeoReference(Rsyn::Pin pin);

	void prepareGeometryManager();
	void prepareRenderingTexture();

	void populateGeometryManager();

	void swapBuffers();

	void saveRendering();
	void restoreRendering();

	GLuint rboColorId = 0;
	GLuint rboDepthId = 0;
	GLuint fboId = 0;
	bool clsRenderingToTextureEnabled = false;
	bool clsRepopulateGeometryManager = true;
public:

	GeometryManager * getGeometryManager() { return &geoMgr; }
	GeometryManager::ObjectId searchObjectAt(const float x, const float y) const;

	////////////////////////////////////////////////////////////////////////////
	// Notifications
	////////////////////////////////////////////////////////////////////////////

	virtual void
	onPostMovedInstance(Rsyn::PhysicalInstance phInstance) override {
		clsRepopulateGeometryManager = true;
	}

	////////////////////////////////////////////////////////////////////////////
	// Callbacks
	////////////////////////////////////////////////////////////////////////////
private:
	
	std::function<void(Rsyn::Instance)> clsInstanceDoubleClickCallback;
	std::function<void(Rsyn::Net)> clsNetDoubleClickCallback;
	std::function<void(Rsyn::Pin)> clsPinDoubleClickCallback;
	
public:

	void setInstanceDoubleClickCallback(const std::function<void(Rsyn::Instance)> &callback) {
		clsInstanceDoubleClickCallback = callback;
	} // end method

	void setNetDoubleClickCallback(const std::function<void(Rsyn::Net)> &callback) {
		clsNetDoubleClickCallback = callback;
	} // end method

	void setPinDoubleClickCallback(const std::function<void(Rsyn::Pin)> &callback) {
		clsPinDoubleClickCallback = callback;
	} // end method

}; // end class


#endif
