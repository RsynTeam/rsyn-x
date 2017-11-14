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
 
#include <thread>

#include "RandomPlacement.h"

#include "rsyn/util/AsciiProgressBar.h"
#include "rsyn/util/Stepwatch.h"

namespace ICCAD15 {

bool RandomPlacementExample::run(const Rsyn::Json &params) {
	this->session = session;
	
	this->timer = session.getService("rsyn.timer");
	this->physical = session.getService("rsyn.physical");
	
	this->design = session.getDesign();
	this->module = design.getTopModule();
	this->phDesign = physical->getPhysicalDesign();
	
	runRandomPlacement();

	return true;
} // end method

// -----------------------------------------------------------------------------

void RandomPlacementExample::runRandomPlacement() {
	Rsyn::PhysicalModule phModule = phDesign.getPhysicalModule(module);
	const Bounds &coreBounds = phModule.getBounds();
	for (Rsyn::Instance instance : module.allInstances()) {
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
		if (!instance.isFixed() && !instance.isMacroBlock()) {
			phDesign.placeCell(cell,
					coreBounds.randomInnerPoint(X),
					coreBounds.randomInnerPoint(Y));
		} // end if
	} // end for	
} // end method

} // end namescape
