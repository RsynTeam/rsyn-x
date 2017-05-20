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
 
#include <GL/glew.h>

#include "Region.h"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/io/Graphics.h"
#include "rsyn/gui/canvas/PhysicalCanvasGL.h"

// -----------------------------------------------------------------------------

bool RegionOverlay::init(PhysicalCanvasGL* canvas, nlohmann::json& properties) {
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crashes when a design without physical data is loaded. 
	if(!canvas->isPhysicalDesignInitialized())
		return false;
	clsEngine = canvas->getEngine();
	design = clsEngine.getDesign();
	module = design.getTopModule(); 

	Rsyn::PhysicalService *physical = clsEngine.getService("rsyn.physical");
	if (!physical)
		return false;
	phDesign = physical->getPhysicalDesign();

	// Regions were not defined in the circuit.
	if(phDesign.getNumPhysicalRegions() == 0)
		return false;
	
	return true;
} // end method

// -----------------------------------------------------------------------------

void RegionOverlay::render(PhysicalCanvasGL * canvas) {
	renderRegions(canvas);
} // end method

// -----------------------------------------------------------------------------

void RegionOverlay::config(const nlohmann::json &params) {
} // end method

// -----------------------------------------------------------------------------

void RegionOverlay::renderRegions(PhysicalCanvasGL * canvas) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor3ub(255, 255, 0);
	double layer = PhysicalCanvasGL::LAYER_BACKGROUND;
	for (Rsyn::PhysicalRegion region : phDesign.allPhysicalRegions()) {
		for (const Bounds & bounds : region.allBounds()) {
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
		} // end for 		
	} // end for
	glEnd();
} // end method