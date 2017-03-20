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
 
#ifndef CLUSTERED_MOVE_OPTO_H
#define CLUSTERED_MOVE_OPTO_H

#include "rsyn/engine/Engine.h"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/model/timing/types.h"

namespace Rsyn {
class Timer;
}

namespace ICCAD15 {

class Infrastructure;

class ClusteredMove : public Rsyn::Process {
private:
	Rsyn::Engine engine;
	Infrastructure * infra;
	Rsyn::Design design;
	Rsyn::Module module;
	Rsyn::PhysicalDesign phDesign;
	Rsyn::Timer *timer;
	Rsyn::PhysicalService* physical;
	
	Rsyn::Attribute<Rsyn::Instance, bool> moved;
	
	void runClusteredMovement(const int N);
	void clusterNeighborCriticalNets( Rsyn::Pin criticalPin, const bool dontMoveRegisters = false );
	
	Number getWorstSlack(const Rsyn::Cell cell, const Rsyn::TimingMode mode);
	
public:
	
	virtual bool run(Rsyn::Engine engine, const Rsyn::Json &params);
	
}; // end class

}
#endif