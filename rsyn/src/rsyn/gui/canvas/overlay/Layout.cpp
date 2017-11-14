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

#include "Layout.h"
#include "rsyn/gui/canvas/Stipple.h"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/io/Graphics.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Temporary. I need to understand and find a better way to integrate this into
//            C++.

#ifndef CALLBACK
#define CALLBACK
#endif

void CALLBACK beginCallback(GLenum type) {
	glBegin(type);
} // end function

void CALLBACK endCallback() {
	glEnd();
} // end function

void CALLBACK errorCallback(GLenum errorCode) {
	const GLubyte *errString;
	errString = gluErrorString(errorCode);
	std::cout << "ERROR: " << errString << "\n";
	std::exit(0);
} // end function

void CALLBACK vertexCallback(GLvoid *vertex) {
	glVertex3dv((GLdouble *) vertex);
} // end cuntion

GLUtesselator * initTess() {
	GLUtesselator * tobj = gluNewTess();
	gluTessCallback(tobj, GLU_TESS_VERTEX, (_GLUfuncptr) vertexCallback);
	gluTessCallback(tobj, GLU_TESS_BEGIN, (_GLUfuncptr) beginCallback);
	gluTessCallback(tobj, GLU_TESS_END, (_GLUfuncptr) endCallback);
	gluTessCallback(tobj, GLU_TESS_ERROR, (_GLUfuncptr) errorCallback);
	gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
	return tobj;
} // end function

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

LayoutOverlay::LayoutOverlay() {
	tess = initTess();
	setupLayers();
} // end constructor

// -----------------------------------------------------------------------------

bool LayoutOverlay::init(PhysicalCanvasGL* canvas, nlohmann::json& properties) {
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crash when loading design without physical data (layout). 
	if (!canvas->isPhysicalDesignInitialized())
		return false;
	
	Rsyn::Session session;

	design = session.getDesign();
	module = design.getTopModule();

	clsGraphics = session.getService("rsyn.graphics");

	Rsyn::PhysicalService *physical = session.getService("rsyn.physical");
	if (!physical)
		return false;
	phDesign = physical->getPhysicalDesign();

	geoMgr = canvas->getGeometryManager();

	properties = {
		{{"name", "instances"}, {"label", "Instances"},  {"type", "bool"}, {"default", clsViewInstances}},
		{{"name", "floorplan"}, {"label", "Floorplan"},  {"type", "bool"}, {"default", clsViewFloorplan}},
		{{"name", "routing"}, {"label", "Routing"},  {"type", "bool"}, {"default", clsViewRouting}}
	};

	properties += {{"name", "rows"}, {"label", "Rows"},  {"type", "bool"}, {"default", clsViewFloorplan_Rows}, {"parent", "floorplan"}};
	properties += {{"name", "sites"}, {"label", "Sites"},  {"type", "bool"}, {"default", clsViewFloorplan_Sites}, {"parent", "floorplan"}};
	properties += {{"name", "specialNets"}, {"label", "Special Nets"},  {"type", "bool"}, {"default", clsViewFloorplan_SpecialNets}, {"parent", "floorplan"}};
	properties += {{"name", "tracks"}, {"label", "Tracks"},  {"type", "bool"}, {"default", clsViewFloorplan_Tracks}, {"parent", "floorplan"}};
	properties += {{"name", "blockages"}, {"label", "Blockages"},  {"type", "bool"}, {"default", clsViewFloorplan_Blockages}, {"parent", "floorplan"}};
	properties += {{"name", "regions"}, {"label", "Regions"},  {"type", "bool"}, {"default", clsViewFloorplan_Regions}, {"parent", "floorplan"}};

	properties += {{"name", "cells"}, {"label", "Cells"},  {"type", "bool"}, {"default", clsViewInstances_Cells}, {"parent", "instances"}};
	properties += {{"name", "macros"}, {"label", "Macros"}, {"type", "bool"}, {"default", clsViewInstances_Macros}, {"parent", "instances"}};
	properties += {{"name", "ports"}, {"label", "Ports"}, {"type", "bool"}, {"default", clsViewInstances_Ports}, {"parent", "instances"}};
	properties += {{"name", "pins"}, {"label", "Pins"}, {"type", "bool"}, {"default", clsViewInstances_Pins}, {"parent", "instances"}};

	// Initialize layer colors on GUI View tab
	int index = 0;
	for (const Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
		const std::string name = phLayer.getName();
		
		const Layer layer = getLayer(std::min(index++, (int)clsLayers.size()-1));
		const Color color = layer.getFillColor();

		properties += {
			{"name", name},
			{"type", "bool"},
			{"default", true},
			{"parent", "routing" },
		};

		properties += {
			{"name", "color"},
			{"label", "Color"},
			{"type", "color"},
			{"parent", "routing." + name},
			{"default", {{"r", color.r}, {"g", color.g}, {"b", color.b}}},
			{"stipple", layer.getStippleMask()}
		};
	}; // end for
	
	// HACK. how to get the total number of layers from PhysicalDesign?
	clsViewLayer.assign(index, true);
//	
//	properties += {
//		{"name", "metal1"}, 
//		{"label", "Metal 1"},  
//		{"type", "bool"}, 
//		{"default", true}
//	};
//	
//	properties += {
//		{"name", "color"}, 
//		{"label", "Color"},  
//		{"type", "color"},
//		{"parent", "metal1"},
//		{"default", {{"r", 255}, {"g", 0}, {"b", 0}}}
//	};
	
//	properties += {
//		{"name", "color"}, 
//		{"label", "Color"},  
//		{"type", "color"},
//		{"parent", "metal1.color"},
//		{"default", {{"r", 0}, {"g", 0}, {"b", 255}}}
//	};

	return true;
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::render(PhysicalCanvasGL * canvas) {
	//renderCells(canvas);
	renderPorts(canvas);
	//renderPins(canvas);
	//renderRouting(canvas);
	renderRows(canvas);
	renderRowSites(canvas);
	//renderSpecialNets(canvas);
	renderTracks(canvas);
	renderBlockages(canvas);
	renderRegions(canvas);

	geoMgr->render();
	geoMgr->renderHighlightedObjects();
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderInterpolated(PhysicalCanvasGL * canvas) {
	renderCells(canvas);
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::config(const nlohmann::json &params) {
	clsViewInstances = params.value("instances", clsViewInstances);
	clsViewInstances_Cells = params.value("instances.cells", clsViewInstances_Cells);
	clsViewInstances_Ports = params.value("instances.ports", clsViewInstances_Ports);
	clsViewInstances_Pins = params.value("instances.pins", clsViewInstances_Pins);
	clsViewInstances_Macros = params.value("instances.macros", clsViewInstances_Macros);
	clsViewFloorplan_Rows = params.value("floorplan.rows", clsViewFloorplan_Rows);
	clsViewFloorplan_Sites = params.value("floorplan.sites", clsViewFloorplan_Sites);
	clsViewFloorplan_SpecialNets = params.value("floorplan.specialNets", clsViewFloorplan_SpecialNets);
	clsViewFloorplan_Tracks = params.value("floorplan.tracks", clsViewFloorplan_Tracks);
	clsViewFloorplan_Blockages = params.value("floorplan.blockages", clsViewFloorplan_Blockages);
	clsViewFloorplan_Regions = params.value("floorplan.regions", clsViewFloorplan_Regions);
	
	clsViewFloorplan = params.value("floorplan", clsViewFloorplan);
	clsViewRouting = params.value("routing", clsViewRouting);

	geoMgr->setLayerVisibility("cells", !clsViewInstances? false : clsViewInstances_Cells);
	geoMgr->setLayerVisibility("ports", !clsViewInstances? false : clsViewInstances_Ports);
	geoMgr->setLayerVisibility("macros", !clsViewInstances? false : clsViewInstances_Macros);
	geoMgr->setLayerVisibility("pins", clsViewInstances_Pins);

	for (Rsyn::PhysicalLayer layer : phDesign.allPhysicalLayers()) {			
		clsViewLayer[layer.getIndex()] =
			params.value("routing." + layer.getName(), (bool) clsViewLayer[layer.getIndex()]);

		if (params.count(layer.getName() + ".color")) {
			Rsyn::Json param = params["routing." + layer.getName() + ".color"];
			Color c(param["r"], param["g"], param["b"]);
			getLayer(layer.getIndex()).setBorderColor(c);
			getLayer(layer.getIndex()).setBorderColor(c);
		} // end if

		if (geoMgr) {
			geoMgr->setLayerVisibility(layer.getName(), !clsViewRouting? false : clsViewLayer[layer.getIndex()]);
		} // end if
	} // end for	
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderPorts(PhysicalCanvasGL * canvas) {
	if (!clsViewInstances || !clsViewInstances_Ports)
		return;
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	for(Rsyn::Instance instance : module.allInstances()) {
		if(instance.getType() != Rsyn::PORT)
			continue;
		Rsyn::PhysicalPort phPort = phDesign.getPhysicalPort(instance.asPort());
		Rsyn::PhysicalLayer phLayer = phPort.getLayer();
		if (!clsViewLayer[phLayer.getIndex()])
			continue;
		const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
		const Color &color = graphicsLayer.getBorderColor();
		glColor3ub(color.r, color.g, color.b);
		const DBUxy disp = phPort.getPosition();
		const Bounds &bounds = phPort.getBounds();
		
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], graphicsLayer.getZ());
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], graphicsLayer.getZ());
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], graphicsLayer.getZ());
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], graphicsLayer.getZ());
		
	} // end for 
	
	glEnd();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_STIPPLE);
	for(Rsyn::Instance instance : module.allInstances()) {
		if(instance.getType() != Rsyn::PORT)
			continue;
		Rsyn::PhysicalPort phPort = phDesign.getPhysicalPort(instance.asPort());
		Rsyn::PhysicalLayer phLayer = phPort.getLayer();
		if (!clsViewLayer[phLayer.getIndex()])
			continue;
		const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
		const Color &color = graphicsLayer.getBorderColor();
		glColor3ub(color.r, color.g, color.b);

		const DBUxy disp = phPort.getPosition();
		const Bounds &bounds = phPort.getBounds();
		glPolygonStipple(STIPPLE_MASKS[graphicsLayer.getStippleMask()]);
		glBegin(GL_QUADS);
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], graphicsLayer.getZ());
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], graphicsLayer.getZ());
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], graphicsLayer.getZ());
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], graphicsLayer.getZ());
		glEnd();
	} // end for 
	
	glDisable(GL_POLYGON_STIPPLE);
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderCells(PhysicalCanvasGL * canvas) {
	if (!clsViewInstances)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);

	for (Rsyn::Instance instance : module.allInstances()) {
		if (instance.getType() != Rsyn::CELL)
			continue;

		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		double layer;

		if (cell.isFixed()) {
			layer = PhysicalCanvasGL::LAYER_OBSTACLES;
		} else {
			layer = PhysicalCanvasGL::LAYER_MOVABLE;
		} // end else

		Color rgb = clsGraphics->getCellColor(cell);
		float2 disp = canvas->getInterpolatedDisplacement(cell);
		glColor3ub(rgb.r, rgb.g, rgb.b);
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);

		if (cell.isMacroBlock()) {
			if (clsViewInstances_Macros) {
				const Rsyn::PhysicalLibraryCell &phLibCell = phDesign.getPhysicalLibraryCell(cell);
				if (phLibCell.hasLayerObstacles()) {
					for (const Bounds & obs : phLibCell.allLayerObstacles()) {
						Bounds bounds = obs;
						DBUxy lower = obs.getCoordinate(LOWER);
						bounds.moveTo(phCell.getPosition() + lower);
						glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
						glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
						glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
						glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
					} // end for
				} else {
					Bounds bounds(DBUxy(), phLibCell.getSize());
					bounds.moveTo(phCell.getPosition());
					glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
					glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
					glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
					glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
				} // end else
			} // end else
		} else {
			if (clsViewInstances_Cells) {
				const Bounds & bounds = phCell.getBounds();
				glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
				glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
				glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
				glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
			} // end if
		} // end if-else 
	} // end for
	glEnd();
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderPins(PhysicalCanvasGL* canvas) {
	// Drawing pin shape 
	if (!clsViewInstances_Pins)
		return;

	
	// Drawing rectangle 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	for (Rsyn::Instance instance : module.allInstances()) {
		if (instance.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
		const DBUxy pos = phCell.getPosition();
		for (Rsyn::Pin pin : instance.allPins()) {
			if (pin.isPort())
				continue;
			Rsyn::PhysicalLibraryPin phLibPin = phDesign.getPhysicalLibraryPin(pin);

			for (Rsyn::PhysicalPinGeometry phPinPort : phLibPin.allPinGeometries()) {
				Rsyn::PhysicalPinLayer phPinLayer = phPinPort.getPinLayer();
				Rsyn::PhysicalLayer phLayer = phPinLayer.getLayer();
				if (!clsViewLayer[phLayer.getIndex()])
					continue;

				const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
				const Color &color = graphicsLayer.getBorderColor();
				glColor3ub(color.r, color.g, color.b);

				for (Bounds bounds : phPinLayer.allBounds()) {
					bounds.translate(pos);
					glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], graphicsLayer.getZ());
					glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], graphicsLayer.getZ());
					glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], graphicsLayer.getZ());
					glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], graphicsLayer.getZ());
				} // end for 
			} // end for 
		} // end for 
	} // end for 
	glEnd();
	
	
	glColor3ub(255, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_STIPPLE);
	glPolygonStipple(STIPPLE_MASKS[STIPPLE_MASK_HALFTONE]);

	for (Rsyn::Instance instance : module.allInstances()) {
		if(instance.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
		const DBUxy pos = phCell.getPosition();
		const double pinLayer = PhysicalCanvasGL::LAYER_GRID;
		for (Rsyn::Pin pin : instance.allPins()) {
			if (pin.isPort())
				continue;
			Rsyn::PhysicalLibraryPin phLibPin = phDesign.getPhysicalLibraryPin(pin);

			if(!phLibPin.hasPinGeometries())
				continue;
			
			for (Rsyn::PhysicalPinGeometry phPinPort : phLibPin.allPinGeometries()) {
				Rsyn::PhysicalPinLayer phPinLayer = phPinPort.getPinLayer();
				Rsyn::PhysicalLayer phLayer = phPinLayer.getLayer();
				if (!clsViewLayer[phLayer.getIndex()])
					continue;

				// Polygons
				for (const Rsyn::PhysicalPolygon & phPoly : phPinLayer.allPolygons()) {

					// TODO -> DEPRECATED -> TO REMOVE 
					//std::cout<<"pin: "<<pin.getFullName()<<"\n";
					//for(const Rsyn::PolygonPoint &polyPoint : phPoly.outer()) {
					//	 using boost::geometry::get;
					//	std::cout<<"\tpointX: "<<get<X>(polyPoint)<<" pointY: "<<get<Y>(polyPoint)<<"\n";
					//}
					//std::cout<<"\tarea: "<<boost::geometry::area(phPoly)<<"\n";
					//std::cout<<"\numPoints: "<<boost::geometry::num_points(phPoly)<<"\n";
					// END TODO 


					GLdouble * vertices = new GLdouble[3 * boost::geometry::num_points(phPoly)];

					int index = 0;
					using boost::geometry::get;
					for (const Rsyn::PhysicalPolygonPoint &polyPoint : phPoly.outer()) {
						vertices[index++] = get<X>(polyPoint) + pos[X];
						vertices[index++] = get<Y>(polyPoint) + pos[Y];
						vertices[index++] = pinLayer;
					} // end for

					gluTessBeginPolygon(tess, NULL);
					gluTessBeginContour(tess);
					index = 0;
					for (const Rsyn::PhysicalPolygonPoint &polyPoint : phPoly.outer()) {
						gluTessVertex(tess, vertices + index, vertices + index);
						index += 3;
					} // end for
					gluTessEndContour(tess);
					gluTessEndPolygon(tess);

					delete[] vertices;
				} // end for 
			} // end for 
		} // end for 
	} // end if

	glDisable(GL_POLYGON_STIPPLE);
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderRows(PhysicalCanvasGL * canvas) {
	if(!clsViewFloorplan_Rows || !clsViewFloorplan)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 0);
	double layer = layer = PhysicalCanvasGL::LAYER_OBSTACLES;
	for (Rsyn::PhysicalRow phRow : phDesign.allPhysicalRows()) {
		const Bounds & bounds = phRow.getBounds();
		glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
		glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
		glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
		glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
	}
	glEnd();
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderRowSites(PhysicalCanvasGL * canvas) {
	if(!clsViewFloorplan_Sites || !clsViewFloorplan)
		return;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 255);
	double layer = layer = PhysicalCanvasGL::LAYER_OBSTACLES;
	for (Rsyn::PhysicalRow phRow : phDesign.allPhysicalRows()) {
		int numSites = phRow.getNumSites(X);
		DBU width = phRow.getSiteWidth();
		Bounds bounds = phRow.getBounds();
		for (int i = 0; i < numSites; i++) {
			bounds[UPPER][X] = bounds[LOWER][X] + width;
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
			bounds[LOWER][X] = bounds[UPPER][X];
		} // end for 
	} // end for 
	glEnd();
} // end method

// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// Experimental
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------

void LayoutOverlay::renderSpecialNets(PhysicalCanvasGL* canvas) {
	if(!clsViewFloorplan_SpecialNets || !clsViewFloorplan)
		return;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for(Rsyn::PhysicalSpecialNet phSpecialNet : phDesign.allPhysicalSpecialNets()) {
		for(Rsyn::PhysicalWire phWire : phSpecialNet.allWires()){
			for(Rsyn::PhysicalWireSegment phSegment : phWire.allSegments()) {
				
				for (Rsyn::PhysicalRoutingPoint phRouting : phSegment.allRoutingPoints()) {
					if(phRouting.hasVia()){
						Rsyn::PhysicalVia phVia = phRouting.getVia();
						drawWireSegmentVia(phVia, phRouting.getPosition());
					} // end if 
				} // end for	
				if(phSegment.getNumRoutingPoints() < 2)
					continue;
				
				Rsyn::PhysicalLayer phLayer = phSegment.getLayer();
				
				if (!clsViewLayer[phLayer.getIndex()])
					continue;
				
				const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
				const Color &color = graphicsLayer.getBorderColor();
				glColor3ub(color.r, color.g, color.b);

				DBU width = phSegment.getRoutedWidth();
				
				
				//TEMPORARY 
				// TODO -> changing drawWireSegmentOutline to receive a vector of PhysicalRoutingPoint
				std::vector<DBUxy> points;
				points.reserve(phSegment.getNumRoutingPoints());
				for (Rsyn::PhysicalRoutingPoint phRouting : phSegment.allRoutingPoints()) {
					points.push_back(phRouting.getPosition());
				}
				// END TEMPORARY 
				drawWireSegmentOutline(points, width/2, graphicsLayer.getZ());
			} // end for 
		} // end for 
	} // end for 
	
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_STIPPLE);
	for(Rsyn::PhysicalSpecialNet phSpecialNet : phDesign.allPhysicalSpecialNets()) {
		for(Rsyn::PhysicalWire phWire : phSpecialNet.allWires()){
			for(Rsyn::PhysicalWireSegment phSegment : phWire.allSegments()) {	
				if(phSegment.getNumRoutingPoints() < 2)
					continue;
				
				Rsyn::PhysicalLayer phLayer = phSegment.getLayer();
				
				if (!clsViewLayer[phLayer.getIndex()])
					continue;
				
				const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
				glPolygonStipple(STIPPLE_MASKS[graphicsLayer.getStippleMask()]);
				const Color &color = graphicsLayer.getBorderColor();
				glColor3ub(color.r, color.g, color.b);

				DBU width = phSegment.getRoutedWidth();
				
				
				//TEMPORARY 
				// TODO -> changing drawWireSegmentOutline to receive a vector of PhysicalRoutingPoint
				std::vector<DBUxy> points;
				points.reserve(phSegment.getNumRoutingPoints());
				for (Rsyn::PhysicalRoutingPoint phRouting : phSegment.allRoutingPoints()) {
					points.push_back(phRouting.getPosition());
				}
				// END TEMPORARY 
				drawWireSegmentFill(points, width/2, graphicsLayer.getZ());
			} // end for 
		} // end for 
	} // end for 
	glDisable(GL_POLYGON_STIPPLE);
	glEnd();
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderTracks(PhysicalCanvasGL * canvas) {
	if(!clsViewFloorplan_Tracks || !clsViewFloorplan)
		return;
	std::size_t lower = 0;
	std::size_t upper = 1;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
	const Bounds & module = phDie.getBounds();
	for(Rsyn::PhysicalTrack phTrack : phDesign.allPhysicalTracks()) {
		Dimension dir = phTrack.getDirection();
		Dimension reverse = REVERSE_DIMENSION[dir];
		const DBU space = phTrack.getSpace();
		for (Rsyn::PhysicalLayer phLayer : phTrack.allLayers()) {
			if (!clsViewLayer[phLayer.getIndex()])
				continue;
			const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
			const Color &color = graphicsLayer.getBorderColor();
			glColor3ub(color.r, color.g, color.b);
			std::vector<DBUxy> points = {DBUxy(), DBUxy()};
			points[lower][dir] = points[upper][dir] = phTrack.getLocation();
			points[lower][reverse] = module[LOWER][reverse];
			points[upper][reverse] = module[UPPER][reverse];
			for (int i = 0; i < phTrack.getNumberOfTracks(); i++) {
				DBU width = phLayer.getWidth();
				drawWireSegmentOutline(points, width/2, graphicsLayer.getZ());
				points[lower][dir] += space;
				points[upper][dir] += space;
			} // end for 
		} // end for 
	} // end for 
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_STIPPLE);
	for(Rsyn::PhysicalTrack phTrack : phDesign.allPhysicalTracks()) {
		Dimension dir = phTrack.getDirection();
		Dimension reverse = REVERSE_DIMENSION[dir];
		const DBU space = phTrack.getSpace();
		for (Rsyn::PhysicalLayer phLayer : phTrack.allLayers()) {
			if (!clsViewLayer[phLayer.getIndex()])
				continue;
			const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
			glPolygonStipple(STIPPLE_MASKS[graphicsLayer.getStippleMask()]);
			const Color &color = graphicsLayer.getBorderColor();
			glColor3ub(color.r, color.g, color.b);
			std::vector<DBUxy> points = {DBUxy(), DBUxy()};
			points[lower][dir] = points[upper][dir] = phTrack.getLocation();
			points[lower][reverse] = module[LOWER][reverse];
			points[upper][reverse] = module[UPPER][reverse];
			for (int i = 0; i < phTrack.getNumberOfTracks(); i++) {
				DBU width = phLayer.getWidth();
				drawWireSegmentFill(points, width/2, graphicsLayer.getZ());
				points[lower][dir] += space;
				points[upper][dir] += space;
			} // end for 
		} // end for 
	} // end for 
	glDisable(GL_POLYGON_STIPPLE);
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderBlockages(PhysicalCanvasGL * canvas) {
	if (!clsViewFloorplan_Blockages || !clsViewFloorplan)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 255);
	for (Rsyn::Instance instance : module.allInstances()) {
		if (instance.getType() != Rsyn::CELL)
			continue;

		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
		Rsyn::PhysicalLibraryCell phLibCell = phDesign.getPhysicalLibraryCell(cell);
		const DBUxy pos = phCell.getPosition();
		if (!phLibCell.hasObstacles())
			continue;
		for (Rsyn::PhysicalObstacle phObs : phLibCell.allObstacles()) {
			double layer = PhysicalCanvasGL::LAYER_OBSTACLES;
			if (phObs.hasLayer()) {
				Rsyn::PhysicalLayer phLayer = phObs.getLayer();
				if (!clsViewLayer[phLayer.getIndex()])
					continue;

				const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
				const Color &color = graphicsLayer.getBorderColor();
				glColor3ub(color.r, color.g, color.b);
				layer = graphicsLayer.getZ();
			} else {
				glColor3ub(0, 0, 0);
			} // end if-else 
			for (Bounds bounds : phObs.allBounds()) {
				bounds.translate(pos);
				glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
				glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
				glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
				glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
			} // end for 
		} // end for  
	} // end for 
	glEnd();
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderRegions(PhysicalCanvasGL* canvas) {
	if (!clsViewFloorplan_Regions || !clsViewFloorplan)
		return;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor3ub(255, 255, 0);
	double layer = -0.1; // cells are drawing as layer 0 (z=0.0)
	for (Rsyn::PhysicalRegion region : phDesign.allPhysicalRegions()) {
		for (const Bounds & bounds : region.allBounds()) {
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
		} // end for 		
	} // end for
	glEnd();
}  // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderRouting(PhysicalCanvasGL * canvas) {
	if (!clsViewRouting)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
	glColor3ub(255, 0, 0);
	for (Rsyn::Net net : module.allNets()) {
		Rsyn::PhysicalNet phNet = phDesign.getPhysicalNet(net);
		for (Rsyn::PhysicalWire phWire : phNet.allWires()) {
			for (Rsyn::PhysicalWireSegment phWireSegment : phWire.allSegments()) {
				const std::vector<DBUxy> &points = phWireSegment.allSegmentPoints();
				if (phWireSegment.hasVia())
					drawWireSegmentVia(phWireSegment.getVia(), points.back());
				
				if (phWireSegment.getNumPoints() < 2)
					continue;
				
				Rsyn::PhysicalLayer phLayer = phWireSegment.getLayer();
				
				if (!clsViewLayer[phLayer.getIndex()])
					continue;
				
				const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
				
				const Color &color = graphicsLayer.getBorderColor();
				glColor3ub(color.r, color.g, color.b);

				DBU width = phLayer.getWidth();
				
				drawWireSegmentOutline(points, width/2, graphicsLayer.getZ());
				
			} // end for
		} // end for
	} // end for

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_STIPPLE);
	for (Rsyn::Net net : module.allNets()) {
		Rsyn::PhysicalNet phNet = phDesign.getPhysicalNet(net);
		for (Rsyn::PhysicalWire phWire : phNet.allWires()) {
			for (Rsyn::PhysicalWireSegment phWireSegment : phWire.allSegments()) {
				if (phWireSegment.getNumPoints() < 2)
					continue;
				Rsyn::PhysicalLayer phLayer = phWireSegment.getLayer();
				
				if (!clsViewLayer[phLayer.getIndex()])
					continue;
				
				const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));
				
				glPolygonStipple(STIPPLE_MASKS[graphicsLayer.getStippleMask()]);
				const Color &color = graphicsLayer.getFillColor();
				glColor3ub(color.r, color.g, color.b);

				DBU width = phLayer.getWidth();

				const std::vector<DBUxy> &points = phWireSegment.allSegmentPoints();
				drawWireSegmentFill(points, width/2, graphicsLayer.getZ());
			} // end for
		} // end for
	} // end for
	glDisable(GL_POLYGON_STIPPLE);
} // end method

// -----------------------------------------------------------------------------

float2 LayoutOverlay::findIntersection(float2 p1, float2 p2, float2 d1, float2 d2) const {
	const float a = p1.x;
	const float e = p1.y;
	const float b = d1.x;
	const float f = d1.y;
	const float c = p2.x;
	const float g = p2.y;
	const float d = d2.x;
	const float h = d2.y;

	float t;
	if ((b * h - d * f) == 0) t = 0;
	else t = ((c - a) * h + (e - g) * d) / (b * h - d * f);
	float2 q = p1 + (d1 * t);

	bool bug;
	if ((t < 0 || t > 1)) {
		bug = true;
	} else {
		bug = false;
	}

	return q;
}//end function

// -----------------------------------------------------------------------------

bool LayoutOverlay::findMyPoint(float2 v0, float2 v1, float2 v2, const float thickness, const float z) const {
	// Direction vectors.
	float2 d1 = v1 - v0;
	float2 d2 = v2 - v1;

	// Points over the lines.
	float2 p1 = v1 + (d1.perpendicular().normalized()) * thickness;
	float2 p2 = v1 + (d2.perpendicular().normalized()) * thickness;

	float m1 = (v0.x - v1.x) == 0 ? (v0.y - v1.y) : (v0.y - v1.y) / (v0.x - v1.x);
	float m2 = (v1.x - v2.x) == 0 ? (v1.y - v2.y) : (v1.y - v2.y) / (v1.x - v2.x);

	float tg = (m1 - m2) / (1 + m1 * m2);
	float distancia = std::sqrt(std::pow((p1.x - p2.x), 2.0f) + std::pow((p1.y - p2.y), 2.0f));
	float limite = std::sqrt(2.0f * std::pow(thickness, 2.0f));

	if (distancia > limite && tg > 0.0f) {//dois
		glVertex3f(p1.x, p1.y, z);
		glVertex3f(p2.x, p2.y, z);
		return true;
	} else {
		float2 q = findIntersection(p1, p2, d1, d2); //um
		glVertex3f(q.x, q.y, z);
		return true;
	}//end else
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::findTwoPoints(float2 const v0, float2 const v1, float2 const v2, const float thickness, const float z) const {

	float limite = sqrt( 2 * pow(thickness, 2.0f));
	float2 d1;	float2 d2;	float2 p1;	float2 p2;

	// Direction vectors.
	d1 = v1 - v0;
	d2 = v2 - v1;

	// Points over the lines.
	p1 = v1 + d1.perpendicular().normalized() * thickness ;
	p2 = v1 + d2.perpendicular().normalized() * thickness ;

	float m1 = (v0.x - v1.x) == 0 ? (v0.y - v1.y) : (v0.y - v1.y)/(v0.x - v1.x);
	float m2 = (v1.x - v2.x) == 0 ? (v1.y - v2.y) : (v1.y - v2.y)/(v1.x - v2.x);

	float tg = (m1 - m2) / ( 1 + m1 * m2 ) ;
	float distancia = std::sqrt( std::pow( (p1.x - p2.x), 2.0f ) + std::pow( (p1.y - p2.y), 2.0f ) );

	if( distancia > limite && tg > 0.0f ){
	//	glColor3f(0,1,0);
		glVertex3f( p1.x, p1.y, z);

		float2 d1b = float2( -d1.x, -d1.y);
		float2 d2b = float2( -d2.x, -d2.y);
		float2 p1b = v1 + d1b.perpendicular().normalized() * thickness ;
		float2 p2b = v1 + d2b.perpendicular().normalized() * thickness ;
		float2 l2 = findIntersection(p1b,p2b,d1b,d2b);
	//	glColor3f(1,0,1);

		glVertex3f( l2.x, l2.y, z);

		glVertex3f( p1.x, p1.y, z);//"triangle"
		glVertex3f( p2.x, p2.y, z);//"triangle"
		glVertex3f( l2.x, l2.y, z);//"triangle"
		glVertex3f( l2.x, l2.y, z);

		glVertex3f( l2.x, l2.y, z);
		glVertex3f( p2.x, p2.y, z);
	} else {
		float2 q1 = findIntersection(p1,p2,d1,d2);

		glVertex3f( q1.x, q1.y, z);

		d1 = float2( -d1.x, -d1.y);
		d2 = float2( -d2.x, -d2.y);
		p1 = v1 + d1.perpendicular().normalized() * thickness ;
		p2 = v1 + d2.perpendicular().normalized() * thickness ;
		  m2 = (v0.x - v1.x) == 0 ? (v0.y - v1.y) : (v0.y - v1.y)/(v0.x - v1.x);
		  m1 = (v1.x - v2.x) == 0 ? (v1.y - v2.y) : (v1.y - v2.y)/(v1.x - v2.x);
		  tg = (m1 - m2) / ( 1 + m1 * m2 ) ;
		  distancia = std::sqrt( std::pow( (p1.x - p2.x), 2.0f ) + std::pow( (p1.y - p2.y), 2.0f ) );

		  if( distancia > limite && tg > 0.0f ){
		  		glVertex3f( p1.x, p1.y, z);
				float2 d1b = float2( -d1.x, -d1.y);
				float2 d2b = float2( -d2.x, -d2.y);
				float2 p1b = v1 + d1b.perpendicular().normalized() * thickness ;
				float2 p2b = v1 + d2b.perpendicular().normalized() * thickness ;
				float2 l2 = findIntersection(p1b,p2b,d1b,d2b);

				glVertex3f( l2.x, l2.y, z);
			//	glColor3f(1,0,1);
				glVertex3f( p1.x, p1.y, z);//"triangle"
				glVertex3f( p2.x, p2.y, z);//"triangle"
				glVertex3f( l2.x, l2.y,z);//"triangle"

			//	glColor3f(0,0,1);
				glVertex3f( p2.x, p2.y, z);
			//	glColor3f(1,0,0);
				glVertex3f( l2.x, l2.y, z);
			} else{
				float2 q2 = findIntersection(p1,p2,d1,d2);
			//	glColor3f(1,1,0);
				glVertex3f( q2.x, q2.y, z);
			//	glColor3f(1,0,0);
				glVertex3f( q2.x, q2.y, z);
				glVertex3f( q1.x, q1.y, z);
			}//end else
	}	//end else
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::drawWireSegmentFill(const std::vector<DBUxy> &clsVecWire, const float thickness, const float z) const {
	glBegin(GL_QUADS);
		//first points
		float2 v1 = float2( clsVecWire[0] ) + float2( float2( clsVecWire[0] ) - float2( clsVecWire[1] ) ).perpendicular().normalized() * thickness ;
		glVertex3f( v1.x, v1.y, z);

		v1 = float2( clsVecWire[0] ) + float2( float2( clsVecWire[1] ) - float2( clsVecWire[0] ) ).perpendicular().normalized() * thickness ;
		glVertex3f( v1.x, v1.y, z);

		for( size_t i = 1; i < clsVecWire.size() - 1; i++){
			findTwoPoints( 
					float2(clsVecWire[i-1]),
					float2(clsVecWire[i  ]),
					float2(clsVecWire[i+1]),
					thickness, z);
		}

		v1 = float2(clsVecWire.back()) + float2( float2( clsVecWire.back() ) - float2( clsVecWire[clsVecWire.size()-2] ) ).perpendicular().normalized() * thickness ;
		glVertex3f( v1.x, v1.y, z);
		float2 v2 = float2( clsVecWire.back() ) + float2( float2( clsVecWire[clsVecWire.size()-2] ) - float2( clsVecWire.back() ) ).perpendicular().normalized() * thickness;
		glVertex3f( v2.x, v2.y, z);

	glEnd();
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::drawWireSegmentOutline(const std::vector<DBUxy> &clsVecWire, const float thickness, const float z) const {
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < clsVecWire.size(); i++)
		glVertex3f(clsVecWire[i].x, clsVecWire[i].y, z);
	glEnd();

	glBegin(GL_LINE_STRIP);
		//first point
		float2 v1 = float2( clsVecWire[0] ) +
				( float2( clsVecWire[1] ) - float2( clsVecWire[0] ) ).perpendicular().normalized() * thickness;
		glVertex3f( v1.x, v1.y, z);

		bool control = true;
		//
		for( size_t i = 1; i < clsVecWire.size() - 1; i++){
			control = findMyPoint( 
					float2(clsVecWire[i-1]),
					float2(clsVecWire[i  ]),
					float2(clsVecWire[i+1]),
					thickness, z);
		} // ida

		if(control){
			v1 = float2(clsVecWire.back()) + float2( float2( clsVecWire.back() ) - float2( clsVecWire[clsVecWire.size()-2] ) ).perpendicular().normalized() * thickness ;
			glVertex3f( v1.x, v1.y, z);
		}
		v1 = float2( clsVecWire.back() ) + float2( float2( clsVecWire[clsVecWire.size()-2] ) - float2( clsVecWire.back() ) ).perpendicular().normalized() * thickness;
		glVertex3f( v1.x, v1.y, z);

		for( int i = clsVecWire.size() - 2; i >= 1; i--){
			findMyPoint( 
					float2(clsVecWire[i+1]),
					float2(clsVecWire[i  ]),
					float2(clsVecWire[i-1]),
					thickness, z );
		}//volta

		v1 = float2( clsVecWire[0] ) + float2( float2( clsVecWire[0] ) - float2( clsVecWire[1] ) ).perpendicular().normalized() * thickness ;
		glVertex3f( v1.x, v1.y, z);
		v1 = float2( clsVecWire[0] ) + float2( float2( clsVecWire[1] ) - float2( clsVecWire[0] ) ).perpendicular().normalized() * thickness ;
		glVertex3f( v1.x, v1.y, z);
	glEnd();
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::drawWireSegmentVia(Rsyn::PhysicalVia phVia, const DBUxy pos) {
	for (Rsyn::PhysicalViaLayer phViaLayer : phVia.allLayers()) {
		Rsyn::PhysicalLayer phLayer = phViaLayer.getLayer();
		if (!clsViewLayer[phLayer.getIndex()])
			continue;

		const Layer &graphicsLayer = getLayer(std::min(phLayer.getIndex(), (int) clsLayers.size() - 1));

		const Color &color = graphicsLayer.getBorderColor();
		glColor3ub(color.r, color.g, color.b);
		const double layer = graphicsLayer.getZ();
		
		for (Bounds bounds : phViaLayer.allBounds()) {
			bounds.translate(pos);

			// draw boundary of cut and routing layers.
			// Temporary -> routing (metal) layers are drawn without fill it.
			if (phLayer.getType() == Rsyn::ROUTING)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
			else if (phLayer.getType() == Rsyn::CUT)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_QUADS);
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
			glEnd();
			
			//draw stripe lines
			if (phLayer.getType() == Rsyn::ROUTING) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_POLYGON_STIPPLE);
				DBU width = static_cast<DBU>(bounds.computeLength(X) / 2.0);
				std::vector<DBUxy> points(2);
				points[0] = DBUxy(pos[X], bounds[LOWER][Y]);
				points[1] = DBUxy(pos[X], bounds[UPPER][Y]);
				drawWireSegmentFill(points, width, graphicsLayer.getZ());
				glDisable(GL_POLYGON_STIPPLE);
			} // end if 
		} // end for 
	} // end for 
	
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::setupLayers() {
//	clsLayers.resize(29);
//	getLayer( 28 ).configure( "LABEL", 28, Color( 255, 255, 255 ), Color( 255, 255, 255 ), STIPPLE_MASK_FILL          );
//	getLayer( 27 ).configure( "MET10", 27, Color( 255, 218, 185 ), Color( 255, 218, 185 ), STIPPLE_MASK_FILL          );
//	getLayer( 26 ).configure( "VIA9" , 26, Color( 127, 255, 212 ), Color( 127, 255, 212 ), STIPPLE_MASK_FILL          );
//	getLayer( 25 ).configure( "MET9" , 25, Color( 255, 250, 205 ), Color( 255, 250, 205 ), STIPPLE_MASK_FILL          );
//	getLayer( 24 ).configure( "VIA8" , 24, Color( 139, 131, 120 ), Color( 139, 131, 120 ), STIPPLE_MASK_FILL          );
//	getLayer( 23 ).configure( "MET8" , 23, Color( 165,  42,  42 ), Color( 165,  42,  42 ), STIPPLE_MASK_FILL          );
//	getLayer( 22 ).configure( "VIA7" , 22, Color( 205, 192, 176 ), Color( 205, 192, 176 ), STIPPLE_MASK_FILL          );
//	getLayer( 21 ).configure( "MET7" , 21, Color( 255, 255,   0 ), Color( 255, 255,   0 ), STIPPLE_MASK_FILL          );
//	getLayer( 20 ).configure( "VIA6" , 20, Color( 238, 223, 204 ), Color( 238, 223, 204 ), STIPPLE_MASK_FILL          );
//	getLayer( 19 ).configure( "MET6" , 19, Color(   0, 255,   0 ), Color(   0, 255,   0 ), STIPPLE_MASK_FILL          );
//	getLayer( 18 ).configure( "VIA5" , 18, Color( 255, 239, 219 ), Color( 255, 239, 219 ), STIPPLE_MASK_FILL          );
//	getLayer( 17 ).configure( "MET5" , 17, Color( 216, 191, 216 ), Color( 216, 191, 216 ), STIPPLE_MASK_FILL          );
//	getLayer( 16 ).configure( "VIA4" , 16, Color( 193, 205, 193 ), Color( 193, 205, 193 ), STIPPLE_MASK_FILL          );
//	getLayer( 15 ).configure( "MET4" , 15, Color(  67, 110, 238 ), Color(  67, 110, 238 ), STIPPLE_MASK_FILL          );
//	getLayer( 14 ).configure( "VIA3" , 14, Color( 255, 192, 203 ), Color( 255, 192, 203 ), STIPPLE_MASK_FILL          );
//	getLayer( 13 ).configure( "MET3" , 13, Color(  50, 255, 255 ), Color(  50, 255, 255 ), STIPPLE_MASK_DIAGONAL_UP_2 );
//	getLayer( 12 ).configure( "VIA2" , 12, Color(  57, 191, 255 ), Color( 255,   0, 255 ), STIPPLE_MASK_CROSS         );
//	getLayer( 11 ).configure( "MET2" , 11, Color( 255,   0, 255 ), Color( 255,   0, 255 ), STIPPLE_MASK_DOT           );
//	getLayer( 10 ).configure( "VIA1" , 10, Color( 255,   0, 255 ), Color(   0,   0, 255 ), STIPPLE_MASK_CROSS         );
//	getLayer(  9 ).configure( "MET1" ,  9, Color(   0,   0, 255 ), Color(   0,   0, 255 ), STIPPLE_MASK_DIAGONAL_DOWN );
//	getLayer(  8 ).configure( "POL2" ,  8, Color( 238,  99,  99 ), Color( 238,  99,  99 ), STIPPLE_MASK_FILL          );
//	getLayer(  7 ).configure( "CONT" ,  7, Color( 185, 211, 238 ), Color(   5,   5,   5 ), STIPPLE_MASK_FILL          );
//	getLayer(  6 ).configure( "POL1" ,  6, Color( 255,   0,   0 ), Color( 255,   0,   0 ), STIPPLE_MASK_HALFTONE      );
//	getLayer(  5 ).configure( "PDIF" ,  5, Color( 255, 128,   0 ), Color( 255, 128,   0 ), STIPPLE_MASK_CROSS         );
//	getLayer(  4 ).configure( "NDIF" ,  4, Color(   0, 204, 102 ), Color(   0, 204, 102 ), STIPPLE_MASK_CROSS         );
//	getLayer(  3 ).configure( "PSEL" ,  3, Color( 255, 128,   0 ), Color( 255, 128,   0 ), STIPPLE_MASK_EMPTY         );
//	getLayer(  2 ).configure( "NSEL" ,  2, Color(   0, 204, 102 ), Color(   0, 204, 102 ), STIPPLE_MASK_EMPTY         );
//	getLayer(  1 ).configure( "NWEL" ,  1, Color(   0, 204, 102 ), Color(   0, 204, 102 ), STIPPLE_MASK_DIAGONAL_UP   );
//	getLayer(  0 ).configure( "PWEL" ,  0, Color( 255, 128,   0 ), Color( 255, 128,   0 ), STIPPLE_MASK_DIAGONAL_UP   );

	// usp_phy
	clsLayers.resize(14);
	getLayer( 13 ).configure( "VIA6" ,  9, DARK_RED, DARK_RED, STIPPLE_MASK_EMPTY           );
	getLayer( 12 ).configure( "MET6" ,  8, DARK_RED, DARK_RED, STIPPLE_MASK_DIAGONAL_UP_3   );
	getLayer( 11 ).configure( "VIA5" ,  9, DARK_RED, DARK_RED, STIPPLE_MASK_EMPTY           );
	getLayer( 10 ).configure( "MET5" ,  8, DARK_RED, DARK_RED, STIPPLE_MASK_DIAGONAL_DOWN_3 );
	getLayer(  9 ).configure( "VIA4" ,  9, DARK_RED, DARK_RED, STIPPLE_MASK_EMPTY           );
	getLayer(  8 ).configure( "MET4" ,  8,   YELLOW,   YELLOW, STIPPLE_MASK_DIAGONAL_UP_2   );
	getLayer(  7 ).configure( "VIA3" ,  7,   YELLOW,   YELLOW, STIPPLE_MASK_EMPTY           );
	getLayer(  6 ).configure( "MET3" ,  6,    GREEN,    GREEN, STIPPLE_MASK_DIAGONAL_DOWN_2     );
	getLayer(  5 ).configure( "VIA2" ,  5,    GREEN,    GREEN, STIPPLE_MASK_EMPTY           );
	getLayer(  4 ).configure( "MET2" ,  4,      RED,      RED, STIPPLE_MASK_DIAGONAL_UP_1   );
	getLayer(  3 ).configure( "VIA1" ,  3,      RED,      RED, STIPPLE_MASK_EMPTY           );
	getLayer(  2 ).configure( "MET1" ,  2,     BLUE,     BLUE, STIPPLE_MASK_DIAGONAL_DOWN_1           );
	getLayer(  1 ).configure( "NWEL" ,  1, Color(   0,   0,   0 ), Color(   0,   0,   0 ), STIPPLE_MASK_EMPTY         );
	getLayer(  0 ).configure( "PWEL" ,  0, Color(   0,   0,   0 ), Color(   0,   0,   0 ), STIPPLE_MASK_EMPTY         );


	// Define z.
	const float zmin = PhysicalCanvasGL::LAYER_ROUTING_MIN;
	const float zmax = PhysicalCanvasGL::LAYER_ROUTING_MAX;
	const float zinc = (zmax - zmin) / clsLayers.size();

	float z = zmin;
	for (Layer &layer : clsLayers) {
		layer.setZ(z);
		z += zinc;
	} // end for
} // end method
