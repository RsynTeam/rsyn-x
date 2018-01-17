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
 * File:   RoutingEstimatorOverlay.cpp
 * Author: mateus
 * 
 * Created on 21 de Janeiro de 2017, 22:01
 */

#include "RoutingEstimatorOverlay.h"
#include "rsyn/model/routing/RoutingEstimator.h"
#include "rsyn/util/Colorize.h"

bool RoutingEstimatorOverlay::init(PhysicalCanvasGL* canvas, nlohmann::json& properties) {
	Rsyn::Session session;
	// Jucemar - 03/26/17 -> Initializes overlay only if physical design is running. 
	if(!session.isServiceRunning("rsyn.physical")) 
		return false;
	clsDesign = session.getDesign();
	clsTopModule = clsDesign.getTopModule();
		
	if (!session.isServiceRunning("rsyn.routingEstimator"))
		return false;
	
	clsRoutingEstimator = session.getService("rsyn.routingEstimator");
	
	clsMaxDegree = 0;
	for (Rsyn::Net net: clsTopModule.allNets()) {
		if (net.isClockNetwork())
			continue;
		clsMaxDegree = std::max(clsMaxDegree, net.getNumPins());
		orderedNets.push_back(make_tuple(net.getNumPins(), net));
	} // end for
	
	std::sort(orderedNets.begin(), orderedNets.end());
	
	return true;
} // end method

//------------------------------------------------------------------------------

void RoutingEstimatorOverlay::render(PhysicalCanvasGL* canvas) {
	Rsyn::Module module = clsDesign.getTopModule();

	glLineWidth(1.0);
	for (Rsyn::Net net: module.allNets()) {
		if (net.isClockNetwork())
			continue;
		
		glBegin(GL_LINES);
		int r, g, b;
		Colorize::colorTemperature(double(net.getNumPins())/clsMaxDegree, r, g, b);
		glColor3ub(r, g, b);
		const Rsyn::RCTree & tree = clsRoutingEstimator->getRCTree(net);

		const int numNodes = tree.getNumNodes();
		for (int i = 1; i < numNodes; i++) { // start @ 1 to skip root node
			const Rsyn::RCTreeNodeTag &tag0 = tree.getNodeTag(tree.getNode(i).propParent);
			const Rsyn::RCTreeNodeTag &tag1 = tree.getNodeTag(i);

			glVertex3d(tag0.x, tag0.y, PhysicalCanvasGL::LAYER_FOREGROUND);
			glVertex3d(tag1.x, tag1.y, PhysicalCanvasGL::LAYER_FOREGROUND);
		} // end for
		glEnd();
	} // end for	
	glLineWidth(1.0);
} // end method

//------------------------------------------------------------------------------

