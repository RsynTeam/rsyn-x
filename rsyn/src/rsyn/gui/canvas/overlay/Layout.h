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
	bool clsViewFloorplan_Rows = true;
	bool clsViewFloorplan_Sites = false;
	bool clsViewFloorplan_SpecialNets = false;
	bool clsViewFloorplan_Tracks = false;
	bool clsViewFloorplan_Blockages = false;
	bool clsViewFloorplan_Regions = false;

	bool clsViewRouting = true;

	GeometryManager * geoMgr = nullptr;
	GLUtriangulatorObj *tess;

	void renderCells(PhysicalCanvasGL * canvas);
	void renderPorts(PhysicalCanvasGL * canvas);
	void renderPins(PhysicalCanvasGL * canvas);
	void renderRouting(PhysicalCanvasGL * canvas);
	void renderRows(PhysicalCanvasGL * canvas);
	void renderRowSites(PhysicalCanvasGL * canvas);
	void renderSpecialNets(PhysicalCanvasGL * canvas);
	void renderTracks(PhysicalCanvasGL * canvas);
	void renderBlockages(PhysicalCanvasGL * canvas);
	void renderRegions(PhysicalCanvasGL * canvas);

	////////////////////////////////////////////////////////////////////////////
	// Experimental
	////////////////////////////////////////////////////////////////////////////
	
	std::vector<bool> clsViewLayer;
	
	class Layer {
	private:
		Color clsFillColor;
		Color clsBorderColor;
		FillStippleMask clsStippleMask;

		float clsZ;
		int clsLayerId;
		string clsName;

		bool clsVisible;
	public:

		Layer() { clsVisible = true; clsZ = 0; }

		bool isVisible() const { return clsVisible; }

		float getZ()const {return clsZ;}
		FillStippleMask getStippleMask()const{ return clsStippleMask; }
		Color getFillColor() const { return clsFillColor; }
		Color getBorderColor() const { return clsBorderColor; }
		const string &getName() const { return clsName; }

		void setZ(const float z) {clsZ = z;}
		void setFillColor(unsigned char r, unsigned char g, unsigned char b) {
			setFillColor(Color( r, g, b));
		} // end method
		void setFillColor(const Color c) { clsFillColor = c; }
		void setBorderColor(unsigned char r, unsigned char g, unsigned char b) {
			setBorderColor(Color( r, g, b));
		} // end method
		void setBorderColor(const Color c) { clsBorderColor = c; }

		void configure( const string name, const int layerId, const Color &fillColor, const Color &borderColor, const FillStippleMask stippleMask, const float z = 0) {
			clsName = name;
			clsLayerId = layerId;
			clsFillColor = fillColor;
			clsBorderColor = borderColor;
			clsStippleMask = stippleMask;
			clsZ = z;
		} // end method
	}; // end class

	std::vector<Layer> clsLayers;
	Layer &getLayer(const int id) {return clsLayers[id]; }

	float2 findIntersection(float2 p1, float2 p2, float2 d1, float2 d2) const;
	bool findMyPoint(float2 v0, float2 v1, float2 v2, const float thickness, const float z) const;
	void findTwoPoints(float2 const v0, float2 const v1, float2 const v2, const float thickness, const float z) const;
	
	void drawWireSegmentFill(const std::vector<DBUxy> &points, const float thickness, const float z) const;
	void drawWireSegmentOutline(const std::vector<DBUxy> &points, const float thickness, const float z) const;
	void drawWireSegmentVia(Rsyn::PhysicalVia phVia, const DBUxy pos);
	
	void setupLayers();

public:
	
	LayoutOverlay();
	virtual bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties);
	virtual void render(PhysicalCanvasGL * canvas);
	virtual void renderInterpolated(PhysicalCanvasGL * canvas);
	virtual void config(const nlohmann::json &params);

}; // end class

#endif