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
 * File:   OverlapRemoverOverlay.cpp
 * Author: mpfogaca
 * 
 * Created on 17 de Outubro de 2016, 14:55
 */

#include "OverlapRemoverOverlay.h"
#include "rsyn/phy/PhysicalService.h"
OverlapRemoverOverlay::OverlapRemoverOverlay() {
} // ens constructor

//------------------------------------------------------------------------------

bool OverlapRemoverOverlay::init(PhysicalCanvasGL* canvas) {
	Rsyn::Engine clsEngine = canvas->getEngine();

	ICCAD15::Infrastructure* clsInfrastructure = clsEngine.getService("ufrgs.ispd16.infra", Rsyn::SERVICE_OPTIONAL);
	if(!clsInfrastructure)
		return false;
	
	Rsyn::PhysicalService* physical = clsEngine.getService("rsyn.physical", Rsyn::SERVICE_OPTIONAL);
	if(!physical)
		return false;
	
	clsBlockageControl = clsEngine.getService("ufrgs.blockageControl", Rsyn::SERVICE_OPTIONAL);
	if(!clsBlockageControl)
		return false;
	
	clsPhysicalDesign = physical->getPhysicalDesign();	
	clsModule = clsEngine.getDesign().getTopModule();
	
	return true;
} // end method

//------------------------------------------------------------------------------

void OverlapRemoverOverlay::config(const nlohmann::json& params) {
} // end method

//------------------------------------------------------------------------------

void OverlapRemoverOverlay::render(PhysicalCanvasGL* canvas) {
	if (!canvas->getEngine())
		return;
	
	renderOverlaps();
} // end method

//------------------------------------------------------------------------------

void OverlapRemoverOverlay::renderOverlaps() {
	
	for (Rsyn::Instance instance : clsModule.allInstances()) {
		if (instance.getType() != Rsyn::InstanceType::CELL ) 
			continue;
			
		if(clsBlockageControl->hasOverlapWithMacro(instance.asCell())) {
			Rsyn::PhysicalCell pCell = clsPhysicalDesign.getPhysicalCell(instance.asCell());
			markOverlap(
				pCell.getCenter(X),
				pCell.getCenter(Y),
				pCell.getLength(X) + 5);
		}
	} // end for
} // end method



