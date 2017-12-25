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

#include "RoutingGuidesOverlay.h"
#include "rsyn/ispd18/Guide.h"

bool RoutingGuidesOverlay::init(PhysicalCanvasGL* canvas, nlohmann::json& properties) {
	design = session.getDesign();
	module = design.getTopModule();
	geometryManager = canvas->getGeometryManager();
	this->canvas = canvas;
	
	if (!session.isServiceRunning("rsyn.routingGuide"))
		return false;
	
	routingGuide = session.getService("rsyn.routingGuide");
	
	visible = design.createAttribute(false);
	
	{ // Command: traceGuide
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("traceGuide");
		dscp.setDescription("Enables the drawing of routing guides for a net.");
		
		dscp.addNamedParam("net",
			ScriptParsing::PARAM_TYPE_STRING,
			ScriptParsing::PARAM_SPEC_MANDATORY,
			"Specifies a net to be drawn.");
		
		dscp.addNamedParam("clear",
			ScriptParsing::PARAM_TYPE_BOOLEAN,
			ScriptParsing::PARAM_SPEC_OPTIONAL,
			"Disables the drawing.",
			"false");
		
		session.registerCommand(dscp, [&](const ScriptParsing::Command &command) {
			const std::string netName = command.getParam("net");
			const bool clear = command.getParam("clear");
			
			if (netName == "all") {
				setAll(!clear);
				return;
			} // end if
				
			Rsyn::Net net = design.findNetByName(netName);

			if (!net) {
				std::cout << "[RoutingGuidesOverlay] Net " << netName << " not found.\n";
			} else {
				visible[net] = !clear;
			} // end if 
		});
	} // end scope
	
	return true;
} // end method

// -----------------------------------------------------------------------------

void RoutingGuidesOverlay::render(PhysicalCanvasGL* canvas) {
	for (const Rsyn::Net net : module.allNets()) {
		if (!visible[net])
			continue;
		
		drawGuide(net);
	} // end for
} // end method

// -----------------------------------------------------------------------------

void RoutingGuidesOverlay::config(const nlohmann::json& params) {

} // end method

// -----------------------------------------------------------------------------

void RoutingGuidesOverlay::drawGuide(const Rsyn::Net net) const {
	const float alpha = -0.001f;
	const Rsyn::NetGuide &netGuide = routingGuide->getGuide(net);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0);
	for(const Rsyn::LayerGuide &layerGuide : netGuide.allLayerGuides()) {		
		const Bounds& bounds = layerGuide.getBounds();
		const GeometryManager::LayerId layerId = canvas->getTechLayerID(layerGuide.getLayer().getIndex());		
		const float z = geometryManager->getLayerZ(layerId);
		const Color color = geometryManager->getLayerLineColor(layerId);
		glColor3ub(color.r, color.g, color.b);
		glBegin(GL_LINE_LOOP);
		glVertex3f(bounds[LOWER][X], bounds[LOWER][Y], z + alpha);
		glVertex3f(bounds[UPPER][X], bounds[LOWER][Y], z + alpha);
		glVertex3f(bounds[UPPER][X], bounds[UPPER][Y], z + alpha);
		glVertex3f(bounds[LOWER][X], bounds[UPPER][Y], z + alpha);
		glEnd();
	} // end for
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_STIPPLE);
	for(const Rsyn::LayerGuide &layerGuide : netGuide.allLayerGuides()) {
		// For a better visualization experience, the first metal layers will
		// not be filled.. 
		if (layerGuide.getLayer().getIndex() == 0)
			continue;
		
		const Bounds& bounds = layerGuide.getBounds();
		const GeometryManager::LayerId layerId = canvas->getTechLayerID(layerGuide.getLayer().getIndex());
		const float z = geometryManager->getLayerZ(layerId);
		const Color color = geometryManager->getLayerFillColor(layerId);
		glColor3ub(color.r, color.g, color.b);
		
		glPolygonStipple(STIPPLE_MASKS[geometryManager->getLayerFillPattern(layerId)]);
		
		glBegin(GL_QUADS);
		glVertex3f(bounds[LOWER][X], bounds[LOWER][Y], z + alpha);
		glVertex3f(bounds[UPPER][X], bounds[LOWER][Y], z + alpha);
		glVertex3f(bounds[UPPER][X], bounds[UPPER][Y], z + alpha);
		glVertex3f(bounds[LOWER][X], bounds[UPPER][Y], z + alpha);
		glEnd();
	} // end for
	glDisable(GL_POLYGON_STIPPLE);
} // end method

void RoutingGuidesOverlay::setAll(const bool isVisible) {
	for (const Rsyn::Net net : module.allNets()) {
		visible[net] = isVisible;
	} // end for
} // end method

// -----------------------------------------------------------------------------
