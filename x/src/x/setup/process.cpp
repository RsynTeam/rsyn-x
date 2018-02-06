/* Copyright 2014-2018 Rsyn
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

#include <Rsyn/Session>

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
#include "x/opto/example/SandboxTest.h"
#include "x/opto/example/PhysicalDesignEx.h"

#ifdef RSYN_ENABLE_OVERLAP_REMOVER
#include "x/opto/example/LemonLP.h"
#include "x/opto/ufrgs/qpdp/OverlapRemover.h"
#endif

// Registration
namespace Rsyn {
static Startup registerProcesses([]{
	Rsyn::Session session;

	// ISPD16
	session.registerProcess<ICCAD15::Balancing>("ufrgs.balancing");
	session.registerProcess<ICCAD15::LoadOptimization>("ufrgs.loadOpto");
	session.registerProcess<ICCAD15::ClusteredMove>("ufrgs.clusteredMove");
	session.registerProcess<ICCAD15::AbuReduction>("ufrgs.abuReduction");
	session.registerProcess<ICCAD15::EarlyOpto>("ufrgs.earlyOpto");
	session.registerProcess<ICCAD15::ISPD16Flow>("ufrgs.ISPD16Flow");

	// Misc
	session.registerProcess<ICCAD15::IncrementalTimingDrivenQP>("ufrgs.incrementalTimingDrivenQP");
	session.registerProcess<ICCAD15::TDQuadraticFlow>("ufrgs.TDQuadraticFlow");

	// External
	session.registerProcess<ICCAD15::FastPlace>("ext.FastPlace");

	// Example
	session.registerProcess<ICCAD15::QuadraticPlacementExample>("example.quadraticPlacement");
	session.registerProcess<ICCAD15::RandomPlacementExample>("example.randomPlacement");
	session.registerProcess<PhysicalDesignExample>("example.physicalDesign");

	// Testing
	session.registerProcess<Testing::SandboxTest>("testing.sandbox");

	// Optionals
#ifdef RSYN_ENABLE_OVERLAP_REMOVER
	session.registerProcess<ICCAD15::LemonLP>("example.lemonLP");
	session.registerProcess<ICCAD15::OverlapRemover>("ufrgs.overlapRemover");
#endif
});
} // end namespace

