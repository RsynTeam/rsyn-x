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
 
#ifndef RSYN_INSTANCE_OVERLAY_H
#define RSYN_INSTANCE_OVERLAY_H

#define RED      Color(255,   0,   0)
#define GREEN    Color(  0, 255,   0)
#define BLUE     Color(  0,   0, 255)
#define YELLOW   Color(255, 255,   0)
#define DARK_RED Color(165,  42,  42)

#include "rsyn/core/Rsyn.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/gui/canvas/PhysicalCanvasGL.h"
#include "rsyn/gui/canvas/Stipple.h"

namespace Rsyn {
class Session;
class Graphics;
} // end namespace

class LayoutOverlay : public CanvasOverlay {
private:

	Rsyn::Design design;
	Rsyn::Module module;
	Rsyn::PhysicalDesign phDesign;

	Rsyn::Graphics * clsGraphics = nullptr;

	bool clsViewInstances = true;
	bool clsViewInstances_Cells = true;
	bool clsViewInstances_Macros = true;
	bool clsViewInstances_Ports = true;
	bool clsViewInstances_Pins = true;

	bool clsViewFloorplan = false;
	bool clsViewFloorplan_Rows = false;
	bool clsViewFloorplan_Sites = false;
	bool clsViewFloorplan_SpecialNets = false;
	bool clsViewFloorplan_Tracks = true;
	bool clsViewFloorplan_Blockages = false;
	bool clsViewFloorplan_Regions = false;

	bool clsViewRouting = true;

	GeometryManager * geoMgr = nullptr;
	GLUtriangulatorObj *tess;

	void renderCellsForInterpolatedMode(PhysicalCanvasGL * canvas);
	void renderPorts(PhysicalCanvasGL * canvas);
	void renderRows(PhysicalCanvasGL * canvas);
	void renderRowSites(PhysicalCanvasGL * canvas);
	void renderTracks(PhysicalCanvasGL * canvas);
	void renderBlockages(PhysicalCanvasGL * canvas);
	void renderRegions(PhysicalCanvasGL * canvas);

	void showGeoLayers(const bool visible, const std::set<std::string> &layers);

	////////////////////////////////////////////////////////////////////////////
	// Experimental
	////////////////////////////////////////////////////////////////////////////
	
	std::vector<bool> clsViewLayer;
	std::vector<bool> clsViewTrackLayer;
	std::vector<bool> clsViewBlockageLayer;
		
public:
	
	LayoutOverlay();
	virtual bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties);
	virtual void render(PhysicalCanvasGL * canvas);
	virtual void renderInterpolated(PhysicalCanvasGL * canvas);
	virtual void config(const nlohmann::json &params);

}; // end class

#endif