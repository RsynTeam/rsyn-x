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

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/model/timing/Timer.h"

namespace Rsyn {

void DefaultTimingModel::start(const Rsyn::Json &params) {
	Rsyn::Session session;
	
	clsDesign = session.getDesign();

	clsScenario = session.getService("rsyn.scenario");
	clsRoutingEstimator = session.getService("rsyn.routingEstimator");
	clsTimer = session.getService("rsyn.timer");

	// TODO: Maybe we should not do this here as this create a soft dependency
	// to physical layer

	// TODO: Maybe we should not do this here as this create a soft dependency
	// to physical layer
	Rsyn::PhysicalDesign phDesign = session.getPhysicalDesign();
	if (phDesign) {
		phDesign.registerObserver(this);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void DefaultTimingModel::stop() {

} // end method

// -----------------------------------------------------------------------------

void DefaultTimingModel::onPostMovedInstance(Rsyn::PhysicalInstance phInstance) {
	clsTimer->dirtyInstance(phInstance.getInstance());
} // end method

// -----------------------------------------------------------------------------

} // end namespace