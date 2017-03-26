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
 
#include "Instance.h"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/io/Graphics.h"

InstanceOverlay::InstanceOverlay() {
	clsViewMovableCells = true;
	clsViewFixedCells = true;
	clsViewPortCells = true;
	clsViewSequentialCells = true; 
} // end constructor

// -----------------------------------------------------------------------------

bool InstanceOverlay::init(PhysicalCanvasGL * canvas) {
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crashes when a design without physical data is loaded. 
	if(!canvas->isPhysicalDesignInitialized())
		return false;
	clsEnginePtr = canvas->getEngine();
	design = clsEnginePtr.getDesign();
	module = design.getTopModule(); 

	clsGraphics = clsEnginePtr.getService("rsyn.graphics");

	Rsyn::PhysicalService *physical = clsEnginePtr.getService("rsyn.physical");
	if (!physical)
		return false;
	phDesign = physical->getPhysicalDesign();

	return true;
} // end method

// -----------------------------------------------------------------------------

void InstanceOverlay::render(PhysicalCanvasGL * canvas) {
	renderCells(canvas);
} // end method

// -----------------------------------------------------------------------------

void InstanceOverlay::config(const nlohmann::json &params) {
	clsViewMovableCells = params.value("/view/movable"_json_pointer, clsViewMovableCells);
	clsViewFixedCells = params.value("/view/fixed"_json_pointer, clsViewFixedCells);
	clsViewPortCells = params.value("/view/port"_json_pointer, clsViewPortCells);
	clsViewSequentialCells = params.value("/view/sequential"_json_pointer, clsViewSequentialCells);
} // end method

// -----------------------------------------------------------------------------

void InstanceOverlay::renderCells(PhysicalCanvasGL * canvas) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);

	for (Rsyn::Instance instance : module.allInstances()) {
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		if (cell.isPort() && !clsViewPortCells)
			continue;
		
		double layer;
		
		if (cell.isFixed()) {
			if (!clsViewFixedCells)
				continue;
			layer = PhysicalCanvasGL::LAYER_OBSTACLES;
		} else {
			if (!clsViewMovableCells)
				continue;
			layer = PhysicalCanvasGL::LAYER_MOVABLE;
		} // end else
		
		
		Color rgb = clsGraphics->getCellColor(cell);
		float2 disp = canvas->getInterpolatedDisplacement(cell);
		glColor3ub(rgb.r, rgb.g, rgb.b);
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
		if (cell.isPort()) {
			glColor3ub(35, 10, 70);
			const Bounds & bounds = phCell.getBounds();
			glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
			glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
			glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
			glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
		} else {
			if (cell.isMacroBlock()) {
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
				}
				
			} else {
				const Bounds & bounds = phCell.getBounds();
				glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
				glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
				glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
				glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
			} // end if-else 
		} // end if-else 
	} // end for
	glEnd();
} // end method