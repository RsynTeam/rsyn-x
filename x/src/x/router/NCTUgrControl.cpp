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
 * File:   Router.cpp
 * Author: jucemar
 * 
 * Created on 13 de Agosto de 2016, 11:37
 */

#include "NCTUgrControl.h"
#include "rsyn/phy/PhysicalDesign.h"

namespace UPLACE {


NCTUgrControl::~NCTUgrControl() {
}

// -----------------------------------------------------------------------------

void NCTUgrControl::initRouter(const int numRows) {
	std::cout<<"__GNUG__  "<<__GNUG__<< " __GNUC_MINOR__: "<<__GNUC_MINOR__<<"\n";
	Rsyn::PhysicalModule phModule = clsPhDesign.getPhysicalModule(clsModule);
	clsRouterBounds = phModule.getBounds();
	int rowHeight = clsPhDesign.getRowHeight();
	clsNumMetals = clsPhDesign.getNumRoutingLayers();
	clsNumXGrids = static_cast<int>(std::ceil(clsRouterBounds.computeLength(X)/(rowHeight*numRows)));
	clsNumYGrids = static_cast<int>(std::ceil(clsRouterBounds.computeLength(Y)/(rowHeight*numRows)));
	clsTileWidth = clsRouterBounds.computeLength(X)/clsNumXGrids;
	clsTileHeight = clsRouterBounds.computeLength(Y)/clsNumYGrids;
	
	clsMetalSpacing.resize(1);
	clsMetalSpacing[0].reserve(clsPhDesign.getNumSpacing());
	for(const Rsyn::PhysicalSpacing spc : clsPhDesign.allSpacing()){
		clsMetalSpacing[0].push_back(spc.getDistance());
	} // end for 
	
	clsMetalWidth.resize(1);
	clsMetalWidth[0].reserve(clsNumMetals);
	for(Rsyn::PhysicalLayer phLayer : clsPhDesign.allRoutingPhysicalLayers()){
		clsMetalWidth[0].push_back(phLayer.getWidth());
	} // end for 
	
	clsRoutingEstimator.SetNctuParams(clsPtRounds, clsMnRounds, clsRarRounds, clslaType, clsPrintToScreen);
	clsRoutingEstimator.SetNumLayers(clsNumMetals); 
	clsRoutingEstimator.SetGridSizeX(clsNumXGrids);
	clsRoutingEstimator.SetGridSizeY(clsNumYGrids);
	clsRoutingEstimator.SetOrigX(clsRouterBounds[LOWER][X]);
	clsRoutingEstimator.SetOrigY(clsRouterBounds[LOWER][Y]);
	clsRoutingEstimator.SetTileHeight(clsTileHeight);
	clsRoutingEstimator.SetTileWidth(clsTileWidth);
	
	clsVCapacities.resize(clsNumMetals);
	clsHCapacities.resize(clsNumMetals);
	
	clsLayerDirs.clear();
	clsLayerDirs.reserve(clsNumMetals);
	//0 for horizontal and 1 vertical. 
	int index = 0;
	for(Rsyn::PhysicalLayer phLayer : clsPhDesign.allRoutingPhysicalLayers()){
		clsLayerDirs.push_back(phLayer.getDirection());
		clsMapRoutingLayers[phLayer.getName()] = index; // Mapping routing layer to matrix index;
		
		//initializing layers capacities
		if(phLayer.getDirection() == Rsyn::VERTICAL){
			clsVCapacities[index] = 1.0;
			clsHCapacities[index] = 0.0;
		} else if (phLayer.getDirection() == Rsyn::HORIZONTAL) {
			clsVCapacities[index] = 0.0;
			clsHCapacities[index] = 1.0;
		}
		index++;
	} // end for 
	clsVCapacities[0] = 0.0;
	clsHCapacities[0] = 0.0;
	
	// Initializing obstacles 
	std::vector<int> numObs;
	clsObstacles.resize(clsNumMetals);
	numObs.resize(clsNumMetals, 0);
	for(Rsyn::Instance inst : clsModule.allInstances()) {
		if(inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = inst.asCell();
		const Rsyn::PhysicalCell &phCell = clsPhDesign.getPhysicalCell(cell);
		if(!cell.isMacroBlock())
			continue;
		const Rsyn::PhysicalLibraryCell &phLibCell = clsPhDesign.getPhysicalLibraryCell(cell);
		for(const Rsyn::PhysicalObstacle phObs : phLibCell.allObstacles()){
			const int index = clsMapRoutingLayers[phObs.getLayer().getName()];
			numObs[index] += phObs.getNumObs();
		} // end for 
	} // end for 
	for(int i = 0; i < numObs.size(); i++){
		if(numObs[i] <= 0)
			continue;
		clsObstacles[i].reserve(numObs[i]);
	} // end for 
	for(Rsyn::Instance inst : clsModule.allInstances()) {
		if(inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = inst.asCell();
		const Rsyn::PhysicalCell &phCell = clsPhDesign.getPhysicalCell(cell);
		if(!cell.isMacroBlock())
			continue;
		const DBUxy cellPos = phCell.getPosition();
		const Rsyn::PhysicalLibraryCell &phLibCell = clsPhDesign.getPhysicalLibraryCell(cell);
		for(const Rsyn::PhysicalObstacle phObs : phLibCell.allObstacles()){
			const int index = clsMapRoutingLayers[phObs.getLayer().getName()];
			for(const Bounds &rect : phObs.allBounds()){
				clsObstacles[index].push_back(rect);
				Bounds & r = clsObstacles[index].back();
				r[LOWER] += cellPos;
				r[UPPER] += cellPos;
			} // end for 
		} // end for 
	} // end for 
#ifdef DEBUG 
	for(int i = 0; i < numObs.size(); i++){
		if(numObs[i] != clsObstacles[i].size())
			std::cout<<"number of obstacle is not equal\n";
	}
#endif
	
	clsRoutingEstimator.SetLayerDirections(clsLayerDirs);
	clsRoutingEstimator.SetNDR(clsMetalWidth, clsMetalSpacing);
	clsRoutingEstimator.SetLayerblockages(clsObstacles);
	clsRoutingEstimator.setLayerCapRatio(clsHCapacities, clsVCapacities);
	
	clsRoutingEstimator.Initialize(true); // Last call 
} // end method 

} // end namespace 
