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
 
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DensityGridOverlay.cpp
 * Author: jucemar
 * 
 * Created on 28 de Setembro de 2016, 16:52
 */

#include "DensityGridOverlay.h"
#include "rsyn/model/congestion/DensityGrid/DensityGridService.h"
#include "rsyn/util/Colorize.h"

DensityGridOverlay::DensityGridOverlay() {
	clsViewUtilization = true;
	clsViewViolations = false;
} // end constuctor 

// -----------------------------------------------------------------------------

bool DensityGridOverlay::init(PhysicalCanvasGL* canvas, nlohmann::json& properties) {
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crashes when a design without physical data is loaded. 
	if(!canvas->isPhysicalDesignInitialized())
		return false;
	Rsyn::Engine clsEnginePtr = canvas->getEngine();
	if(!clsEnginePtr.isServiceRunning("rsyn.densityGrid"))
		return false;
	return true;
} // end method

// -----------------------------------------------------------------------------


void DensityGridOverlay::config(const nlohmann::json &params) {
	clsViewUtilization = params.value("/view/utilization"_json_pointer, clsViewUtilization);
	clsViewViolations = params.value("/view/violations"_json_pointer, clsViewViolations);
} // end method

// -----------------------------------------------------------------------------

void DensityGridOverlay::render(PhysicalCanvasGL * canvas) {
	Rsyn::Engine clsEnginePtr = canvas->getEngine();
	if(!clsEnginePtr.isServiceRunning("rsyn.densityGrid"))
		return;
	
	Rsyn::DensityGridService * server = clsEnginePtr.getService("rsyn.densityGrid");
	Rsyn::DensityGrid grid = server->getDensityGrid();
	if (grid.isInitialized()) {
		renderGrid(canvas);
		//if (clsViewUtilization) renderPinUtilization(canvas);
		//if (clsViewViolations) renderAbuViolations(canvas);
		renderAreaUtilization(canvas);
		renderRows(canvas);
	} else {
		std::cerr<<"**** WARNING: Grid Area was not initialized!\n";
	}
} // end method

// -----------------------------------------------------------------------------

void DensityGridOverlay::renderPinUtilization(PhysicalCanvasGL * canvas) {
	Rsyn::Engine engine = canvas->getEngine();
	if(!engine.isServiceRunning("rsyn.densityGrid"))
		return;
	
	Rsyn::DensityGridService * server = engine.getService("rsyn.densityGrid");
	Rsyn::DensityGrid grid = server->getDensityGrid();
	grid.updateMaxPins();
	const int maxPins = grid.getMaxPins(Rsyn::CONNECTED_PIN);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);

	double layer = PhysicalCanvasGL::LAYER_GRID;
	
	for (unsigned i = 0; i < grid.getNumRows(); i++) {
		for (unsigned int j = 0; j < grid.getNumCols(); j++) {
			const Rsyn::DensityGridBin & bin = grid.getDensityGridBin(i, j);
			double weight =  bin.getNumPins(Rsyn::CONNECTED_PIN) / (double) maxPins;
			int r;
			int g;
			int b;
			Colorize::colorTemperature(weight, r, g, b);
			glColor3ub(r, g, b);
			const Bounds &bounds = bin.getBounds();
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
		} // end for 
	} // end for
	glEnd();
} // end method 

// -----------------------------------------------------------------------------

void DensityGridOverlay::renderAreaUtilization(PhysicalCanvasGL * canvas) {
	Rsyn::Engine engine = canvas->getEngine();
	if(!engine.isServiceRunning("rsyn.densityGrid"))
		return;
	
	Rsyn::DensityGridService * server = engine.getService("rsyn.densityGrid");
	Rsyn::DensityGrid grid = server->getDensityGrid();
	grid.updateMaxAreas();
	double maxArea = grid.getMaxArea(Rsyn::MOVABLE_AREA) + grid.getMaxArea(Rsyn::FIXED_AREA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);

	double layer = PhysicalCanvasGL::LAYER_GRID;
	
	for (unsigned i = 0; i < grid.getNumRows(); i++) {
		for (unsigned int j = 0; j < grid.getNumCols(); j++) {
			const Rsyn::DensityGridBin & bin = grid.getDensityGridBin(i, j);
			double binArea = bin.getArea(Rsyn::MOVABLE_AREA) + bin.getArea(Rsyn::FIXED_AREA);
			double weight =  binArea / maxArea;
			int r, g, b;
			Colorize::colorTemperature(weight, r, g, b);
			glColor3ub(r, g, b);
			const Bounds &bounds = bin.getBounds();
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
		} // end for 
	} // end for
	glEnd();
} // end method 

// -----------------------------------------------------------------------------

void DensityGridOverlay::renderAbuViolations(PhysicalCanvasGL * canvas) {
	std::cout << "TODO " << __func__ << "\n";
} // end method 

// -----------------------------------------------------------------------------

void DensityGridOverlay::renderGrid(PhysicalCanvasGL * canvas) {
	Rsyn::Engine clsEnginePtr = canvas->getEngine();
	if(!clsEnginePtr.isServiceRunning("rsyn.densityGrid"))
		return;
	
	Rsyn::DensityGridService * server = clsEnginePtr.getService("rsyn.densityGrid");
	Rsyn::DensityGrid grid = server->getDensityGrid();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1);
	glBegin(GL_QUADS);

	double layer = PhysicalCanvasGL::LAYER_GRID;
	glColor3ub(0, 0, 0);
	const unsigned numRows = grid.getNumRows();
	const unsigned numCols = grid.getNumCols();
	
	for (unsigned i = 0; i < numRows; i++) {
		for (unsigned j = 0; j < numCols; j++) {
			const Rsyn::DensityGridBin & bin = grid.getDensityGridBin(i, j);
			const Bounds &bounds = bin.getBounds();
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
		} // end for 
	} // end for
	glLineWidth(1);
	glEnd();
} // end method 

// -----------------------------------------------------------------------------

void DensityGridOverlay::renderRows(PhysicalCanvasGL * canvas) {
	Rsyn::Engine engine = canvas->getEngine();
	if(!engine.isServiceRunning("rsyn.densityGrid"))
		return;
	
	Rsyn::DensityGridService * server = engine.getService("rsyn.densityGrid");
	Rsyn::DensityGrid grid = server->getDensityGrid();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);

	double layer = PhysicalCanvasGL::LAYER_GRID;
	glColor3ub(0, 125, 125);
	
	for (unsigned i = 0; i < grid.getNumRows(); i++) {
		for (unsigned int j = 0; j < grid.getNumCols(); j++) {
			const Rsyn::DensityGridBin & bin = grid.getDensityGridBin(i, j);
			for (const Bounds & bounds : bin.allRows()) {
				glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
				glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
				glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
				glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
			} // end for 
		} // end for 
	} // end for
	glEnd();
}