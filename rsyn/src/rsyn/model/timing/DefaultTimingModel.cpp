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
 
#include "DefaultTimingModel.h"

#include "rsyn/engine/Engine.h"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/model/timing/Timer.h"

namespace Rsyn {

void DefaultTimingModel::start(Engine engine, const Json &params) {
	clsDesign = engine.getDesign();

	clsScenario = engine.getService("rsyn.scenario");
	clsRoutingEstimator = engine.getService("rsyn.routingEstimator");
	clsTimer = engine.getService("rsyn.timer");

	// TODO: Maybe we should not do this here as this create a soft dependency
	// to physical layer
	Rsyn::PhysicalService *physical =
			engine.getService("rsyn.physical", Rsyn::SERVICE_OPTIONAL);
	if (physical) {
		Rsyn::PhysicalDesign phDesign;
		phDesign = physical->getPhysicalDesign();
		phDesign.addPostInstanceMovedCallback(0, [&](Rsyn::PhysicalInstance instance) {
			clsTimer->dirtyInstance(instance.getInstance());
		});
	} // end if
} // end method

// -----------------------------------------------------------------------------

void DefaultTimingModel::stop() {

} // end method

} // end namespace