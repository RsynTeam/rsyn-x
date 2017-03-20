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



#include "DensityGridService.h"
#include "rsyn/engine/Engine.h"
#include "rsyn/phy/PhysicalService.h"

namespace Rsyn {


void DensityGridService::start(Rsyn::Engine engine, const Rsyn::Json& params) {
	if (!engine.isServiceRunning("rsyn.physical")) {
		std::cout << "Warning: rsyn.physical service must be running before start DensityGrid service.\n"
			<< "DensityGrid was not initialized.\n";
		return;
	} // end if 

	Rsyn::PhysicalService * ph = engine.getService("rsyn.physical");
	Rsyn::PhysicalDesign phDsg = ph->getPhysicalDesign();
	Rsyn::Design dsg = engine.getDesign();
	Rsyn::Module module = dsg.getTopModule();
	double targetUtil = 0.0;
	int numRows = 9; 
	bool showDetails = false;
	bool keepRowBounds = false;
	targetUtil = params.value("density", targetUtil);
	numRows = params.value("numRows", numRows);
	showDetails = params.value("showDetails", showDetails);
	keepRowBounds = params.value("keepRowBounds", keepRowBounds);
	//module = params.value("module", module);
	clsDensityGrid.init(phDsg, module, targetUtil, numRows, showDetails, keepRowBounds);
	clsDensityGrid.updateArea(MOVABLE_AREA);
} // end method 

// -----------------------------------------------------------------------------

void DensityGridService::stop() {
	
} // end method 

} // end namespace