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

// HOW-TO
// To register process (e.g. optimization step), just include its .h below and
// add a call to "registerProcess<T>(name)" where T is the process class name
// and name how the process will be referred to.

#include "rsyn/session/Session.h"

// Optimizations
#include "x/opto/ufrgs/ispd16/Balancing.h"
#include "x/opto/ufrgs/ispd16/LoadOptimization.h"
#include "x/opto/ufrgs/ispd16/ClusteredMove.h"
#include "x/opto/ufrgs/ispd16/AbuReduction.h"
#include "x/opto/ufrgs/ispd16/EarlyOpto.h"
#include "x/opto/ufrgs/ispd16/ISPD16Flow.h"
#include "x/opto/ufrgs/qpdp/IncrementalTimingDrivenQP.h"
#include "x/opto/ufrgs/qpdp/TDQuadraticFlow.h"
#include "x/opto/ext/FastPlace.h"
#include "x/opto/example/QuadraticPlacement.h"
#include "x/opto/example/RandomPlacement.h"
#include "x/opto/example/LemonLP.h"
#include "x/opto/ufrgs/qpdp/OverlapRemover.h"
#include "x/opto/example/SandboxTest.h"

// Registration
namespace Rsyn {
void Session::registerProcesses() {
	// ISPD16
	registerProcess<ICCAD15::Balancing>("ufrgs.balancing");
	registerProcess<ICCAD15::LoadOptimization>("ufrgs.loadOpto");
	registerProcess<ICCAD15::ClusteredMove>("ufrgs.clusteredMove");
	registerProcess<ICCAD15::AbuReduction>("ufrgs.abuReduction");
	registerProcess<ICCAD15::EarlyOpto>("ufrgs.earlyOpto");
	registerProcess<ICCAD15::ISPD16Flow>("ufrgs.ISPD16Flow");

	// Misc
	registerProcess<ICCAD15::IncrementalTimingDrivenQP>("ufrgs.incrementalTimingDrivenQP");
	registerProcess<ICCAD15::TDQuadraticFlow>("ufrgs.TDQuadraticFlow");
	registerProcess<ICCAD15::OverlapRemover>("ufrgs.overlapRemover");

	// External
	registerProcess<ICCAD15::FastPlace>("ext.FastPlace");

	// Example
	registerProcess<ICCAD15::QuadraticPlacementExample>("example.quadraticPlacement");
	registerProcess<ICCAD15::RandomPlacementExample>("example.randomPlacement");
	registerProcess<ICCAD15::LemonLP>("example.lemonLP");

	// Testing
	registerProcess<Testing::SandboxTest>("testing.sandbox");
} // end method
} // end namespace

