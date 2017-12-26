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
#ifdef __WXMAC__
	typedef GLvoid (*_GLUfuncptr)();
#endif

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
	for (const Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
		const std::string name = phLayer.getName();

		properties += {
			{"name", name},
			{"type", "bool"},
			{"default", true},
			{"parent", "routing" },
		};

		const GeometryManager::LayerId id = canvas->getTechLayerID(phLayer.getIndex());
		const Color color = geoMgr->getLayerFillColor(id);
		const FillStippleMask mask = geoMgr->getLayerFillPattern(id);

		properties += {
			{"name", "color"},
			{"label", "Color"},
			{"type", "color"},
			{"parent", "routing." + name},
			{"default", {{"r", color.r}, {"g", color.g}, {"b", color.b}}},
			{"stipple", mask}
		};
	}

	// Initialize layer colors on GUI View tab for tracks
	for (const Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
		if (phLayer.getType() != Rsyn::ROUTING)
			continue;
		const std::string name = phLayer.getName();
		properties +={
			{"name", name},
			{"type", "bool"},
			{"default", false},
			{"parent", "floorplan.tracks"},};
		GeometryManager::LayerId id = canvas->getTechLayerID(phLayer.getIndex());
		Color color = geoMgr->getLayerFillColor(id);
		FillStippleMask mask = geoMgr->getLayerFillPattern(id);
		properties += {
			{"name", "color"},
			{"label", "Color"},
			{"type", "color"},
			{"parent", "floorplan.tracks." + name},
			{"default", {{"r", color.r}, {"g", color.g}, {"b", color.b}}},
			{"stipple", mask}
		};
	}; // end for

	// Initialize layer colors on GUI View tab for Metal Blockages
	for (const Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {

		const std::string name = phLayer.getName();
		properties +={
			{"name", name},
			{"type", "bool"},
			{"default", true},
			{"parent", "floorplan.blockages"},};
		GeometryManager::LayerId id = canvas->getTechLayerID(phLayer.getIndex());
		Color color = geoMgr->getLayerFillColor(id);
		FillStippleMask mask = geoMgr->getLayerFillPattern(id);
		properties += {
			{"name", "color"},
			{"label", "Color"},
			{"type", "color"},
			{"parent", "floorplan.blockages." + name},
			{"default", {{"r", color.r}, {"g", color.g}, {"b", color.b}}},
			{"stipple", mask}
		};
	}; // end for

	// HACK. how to get the total number of layers from PhysicalDesign?
	clsViewLayer.assign(phDesign.getNumLayers(), true);
	clsViewTrackLayer.assign(phDesign.getNumLayers(), false);
	clsViewBlockageLayer.assign(phDesign.getNumLayers(), true);
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
	renderPorts(canvas);
	renderRows(canvas);
	renderRowSites(canvas);
	renderTracks(canvas);
	renderBlockages(canvas);
	renderRegions(canvas);

	geoMgr->render();
	geoMgr->renderHighlightedObjects();
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderInterpolated(PhysicalCanvasGL * canvas) {
	renderCellsForInterpolatedMode(canvas);
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
		/*
		if (params.count(layer.getName() + ".color")) {
			Rsyn::Json param = params["routing." + layer.getName() + ".color"];
			Color c(param["r"], param["g"], param["b"]);
		} // end if
		*/

		if (geoMgr) {
			geoMgr->setLayerVisibility(layer.getName(), !clsViewRouting? false : clsViewLayer[layer.getIndex()]);
		} // end if


		// Setting tracks
		if (layer.getType() == Rsyn::ROUTING) {
			clsViewTrackLayer[layer.getIndex()] =
				params.value("floorplan.tracks." + layer.getName(),
				(bool)clsViewTrackLayer[layer.getIndex()]);
		}

		// Setting Blockages
		clsViewBlockageLayer[layer.getIndex()] =
			params.value("floorplan.blockages." + layer.getName(),
			(bool)clsViewBlockageLayer[layer.getIndex()]);
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

		const GeometryManager::LayerId id = canvas->getTechLayerID(phLayer.getIndex());
		const Color &color = geoMgr->getLayerFillColor(id);
		const float z = geoMgr->getLayerZ(id);

		glColor3ub(color.r, color.g, color.b);
		const DBUxy disp = phPort.getPosition();
		const Bounds &bounds = phPort.getBounds();

		glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], z);
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], z);
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], z);
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], z);
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

		const GeometryManager::LayerId id = canvas->getTechLayerID(phLayer.getIndex());
		const Color color = geoMgr->getLayerFillColor(id);
		const FillStippleMask mask = geoMgr->getLayerFillPattern(id);
		const float z = geoMgr->getLayerZ(id);

		glColor3ub(color.r, color.g, color.b);

		const DBUxy disp = phPort.getPosition();
		const Bounds &bounds = phPort.getBounds();
		glPolygonStipple(STIPPLE_MASKS[mask]);
		glBegin(GL_QUADS);
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], z);
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], z);
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], z);
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], z);
		glEnd();
	} // end for

	glDisable(GL_POLYGON_STIPPLE);
} // end method

// -----------------------------------------------------------------------------

void LayoutOverlay::renderCellsForInterpolatedMode(PhysicalCanvasGL * canvas) {
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

void LayoutOverlay::renderRows(PhysicalCanvasGL * canvas) {
	if(!clsViewFloorplan_Rows || !clsViewFloorplan)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3ub(255, 255, 255);
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

void LayoutOverlay::renderTracks(PhysicalCanvasGL * canvas) {
	if(!clsViewFloorplan_Tracks || !clsViewFloorplan)
		return;

	glLineWidth(1);
	glLineStipple(3, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	//glDisable(GL_DEPTH_TEST);

	Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
	const Bounds & module = phDie.getBounds();
	for(Rsyn::PhysicalTrack phTrack : phDesign.allPhysicalTracks()) {
		Dimension dir = phTrack.getDirection();
		Dimension reverse = REVERSE_DIMENSION[dir];
		const DBU space = phTrack.getSpace();
		for (Rsyn::PhysicalLayer phLayer : phTrack.allLayers()) {
			if (!clsViewTrackLayer[phLayer.getIndex()])
				continue;

			if ((dir == X && phLayer.getDirection() == Rsyn::HORIZONTAL) ||
					(dir == Y && phLayer.getDirection() == Rsyn::VERTICAL)) {
				// Don't draw tracks with direction different from the preferred
				// routing direction of the layer.
				continue;
			} // end if

			const GeometryManager::LayerId layerId = canvas->getTechLayerID(phLayer.getIndex());
			const Color &color = geoMgr->getLayerFillColor(layerId);
			const float z = geoMgr->getLayerZ(layerId);

			glColor3ub(color.r, color.g, color.b);
			DBUxy p0;
			DBUxy p1;

			p0[dir] = phTrack.getLocation();
			p1[dir] = phTrack.getLocation();

			p0[reverse] = module[LOWER][reverse];
			p1[reverse] = module[UPPER][reverse];

			for (int i = 0; i < phTrack.getNumberOfTracks(); i++) {
				glBegin(GL_LINES);
				glVertex3d(p0.x, p0.y, z);
				glVertex3d(p1.x, p1.y, z);
				glEnd();

				p0[dir] += space;
				p1[dir] += space;
			} // end for
		} // end for
	} // end for

	glLineWidth(1);
	glDisable(GL_LINE_STIPPLE);
	//glEnable(GL_DEPTH_TEST);
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
				if (!clsViewBlockageLayer[phLayer.getIndex()])
					continue;

				const GeometryManager::LayerId id = canvas->getTechLayerID(phLayer.getIndex());
				const Color color = geoMgr->getLayerFillColor(id);
				layer = geoMgr->getLayerZ(id);

				glColor3ub(color.r, color.g, color.b);
			} else {
				glColor3ub(255, 255, 255);
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