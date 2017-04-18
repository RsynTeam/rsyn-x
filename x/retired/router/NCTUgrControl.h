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
 * File:   Router.h
 * Author: jucemar
 *
 * Created on 13 de Agosto de 2016, 11:37
 */

#ifndef NCTUGRROUTERCONTROL_H
#define NCTUGRROUTERCONTRO_H

#include "nctugr/RoutingEstimator.h"
#include "rsyn/phy/PhysicalDesign.h"
namespace UPLACE {

class NCTUgrControl {
private:
	// NCTUgr Routing 
	TRD::RoutingEstimator clsRoutingEstimator;

	//NCTUgr Parameters
	int clsPtRounds;
	int clsMnRounds;
	int clsRarRounds;
	int clslaType;
	double clsMaxGCellUsage;
	bool clsPrintToScreen;
	vector<int> clsLayerDirs;
	vector < vector<double> > clsMetalSpacing; // samenet spacing - used in nctugr router
	vector < vector <double> > clsMetalWidth; // used in nctugr router
	vector< vector < Bounds > > clsObstacles; //list of obstacles for routing
	std::unordered_map<std::string, int> clsMapRoutingLayers;
	std::vector<double> clsHCapacities;
	std::vector<double> clsVCapacities;
	
	int clsNumXGrids; // number of grids in X dimension
	int clsNumYGrids; // number of grids in Y dimension
	double clsTileWidth;
	double clsTileHeight; 
	int clsNumMetals;
	Bounds clsRouterBounds;
	
	Rsyn::Module clsModule;
	Rsyn::Design clsDesign;
	Rsyn::PhysicalDesign clsPhDesign;
	
	void initRouter(const int numRows = 4);
public:	
	NCTUgrControl(Rsyn::Design design, Rsyn::Module module, Rsyn::PhysicalDesign phDsg, const int numRows = 4)
		: clsDesign(design), clsModule(module), clsPhDesign(phDsg), 
		clsRoutingEstimator(module, phDsg) {
		clsMaxGCellUsage = 0.8;
		clsPtRounds = int(1); // default NCTUgr parameter
		clsMnRounds = int(1); // default NCTUgr parameter
		clsRarRounds = int(3); // default NCTUgr parameter
		clslaType = int(0); // default NCTUgr parameter
		clsPrintToScreen = bool(false);  // default NCTUgr parameter
		clsNumXGrids = 0;
		clsNumYGrids = 0;
		clsNumMetals = 0;
		clsTileHeight = 0.0;
		clsTileWidth = 0.0;
		initRouter(numRows);
	} // end constructor 
	
	virtual ~NCTUgrControl();
	
	void runRouter() {
		clsRoutingEstimator.runRouter();
		cout << "NCTUgr #OF: " << clsRoutingEstimator.PrintOverflow(false) << "\n";
	}

};

} // end namespace 
#endif /* ROUTER_H */

