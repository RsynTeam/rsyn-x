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
 * File:   RoutingGuide.cpp
 * Author: jucemar
 *
 * Created on 21 de Dezembro de 2016, 17:47
 */

#include "RoutingGuide.h"
#include "rsyn/phy/PhysicalService.h"

namespace Rsyn {

void RoutingGuide::start(const Rsyn::Json &params) {
	if(clsInitialized)
		return;
	clsDesign = clsSession.getDesign();
	clsModule = clsDesign.getTopModule();
	clsGuides = clsDesign.createAttribute();
	
	Rsyn::PhysicalService* phService = clsSession.getService("rsyn.physical");
	clsPhDesign = phService->getPhysicalDesign();
	clsInitialized = true;
} // end method

// -----------------------------------------------------------------------------

void RoutingGuide::stop() {
} // end method

// -----------------------------------------------------------------------------

void RoutingGuide::loadGuides(const GuideDscp & dscp) {
	for (const GuideNetDscp & netDscp : dscp.clsNetGuides) {
		Rsyn::Net net = clsDesign.findNetByName(netDscp.clsNetName);
		NetGuide & netGuide = clsGuides[net];
		std::vector<LayerGuide> & layerGuides= netGuide.clsLayerGuides;
		layerGuides.reserve(netDscp.clsLayerDscps.size());
		for(const GuideLayerDscp & layerDscp : netDscp.clsLayerDscps) {
			layerGuides.push_back(LayerGuide());
			LayerGuide & layerGuide = layerGuides.back();
			layerGuide.clsBounds = layerDscp.clsLayerGuide;
			layerGuide.clsPhLayer = clsPhDesign.getPhysicalLayerByName(layerDscp.clsLayer);
		} // end for 
	} // end for 
} // end method

// -----------------------------------------------------------------------------

} // end namespace 