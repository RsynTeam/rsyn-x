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

#include "rsyn/core/Rsyn.h"
#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/ispd18/RoutingGuide.h"
#include "ispd18/demo/ExampleService.h"

// -----------------------------------------------------------------------------

void ExampleService::start(const Rsyn::Json &params) {
	Rsyn::Session session;

	design = session.getDesign();
	module = design.getTopModule();

	physicalService = session.getService("rsyn.physical");
	guideService = session.getService("rsyn.routingGuide");

	physicalDesign = physicalService->getPhysicalDesign();

	{ // helloWorld command
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("helloWorld");
		dscp.setDescription("Says hello world.");

		dscp.addNamedParam("uppercase",
			ScriptParsing::PARAM_TYPE_BOOLEAN,
			ScriptParsing::PARAM_SPEC_OPTIONAL,
			"Outputs hello world in uppercase.",
			"false");

		session.registerCommand(dscp, [&](const ScriptParsing::Command &command) {
			const bool uppercase = command.getParam("uppercase");
			if (uppercase) {
				std::cout << "HELLO WORLD\n";
			} else {
				std::cout << "hello world\n";
			} // end if
		});
	} // end block
} // end method

// -----------------------------------------------------------------------------

void ExampleService::stop() {

} // end method

// -----------------------------------------------------------------------------

void ExampleService::doSomething() {
	const int N = 10;
	
	int counter;

	// Traverse nets and their routing guides.
	std::cout << "Nets:\n"; 
	counter = 0;
	for (Rsyn::Net net : module.allNets()) {
		std::cout << net.getName() << "\n";

		const Rsyn::NetGuide &netGuide = guideService->getGuide(net);
		for (const Rsyn::LayerGuide &layerGuide : netGuide.allLayerGuides()) {
			std::cout << "  " << layerGuide.getBounds() << " " << layerGuide.getLayer().getName() << "\n";
		} // end for

		if (++counter >= N) {
			break;
		} // end if
	} // end if

	// Traverse cells and print their width and height.
	std::cout << "Cells:\n";
	counter = 0;
	for (Rsyn::Instance instance : module.allInstances()) {
		if (instance.getType() != Rsyn::CELL)
			continue;

		Rsyn::Cell cell = instance.asCell();
		Rsyn::PhysicalCell physicalCell = physicalDesign.getPhysicalCell(cell);

		std::cout << cell.getName() << " (" << physicalCell.getWidth() << ", "
				<< physicalCell.getHeight() << ")\n";

		if (++counter >= N) {
			break;
		} // end if
	} // end for

} // end method