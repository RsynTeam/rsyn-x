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
 * File:   DensityGridData.h
 * Author: jucemar
 *
 * Created on 16 de Setembro de 2016, 17:15
 */

#ifndef GRIDAREADATA_H
#define GRIDAREADATA_H

#include "DensityGridBin.h"
#include "DensityGridBlockage.h"




namespace Rsyn {

class DensityGridData {
	friend class DensityGrid;
protected:
	Rsyn::Design clsDesign;
	Rsyn::PhysicalDesign clsPhDesign;
	Rsyn::Module clsModule;
	Rsyn::PhysicalModule clsPhModule;
	
	std::vector<DensityGridBin> clsBins;

	std::vector<int> clsMaxAreaBin; // stores the bin index that have the highest area by area type
	std::vector<int> clsMaxPinBin; // stores the bin index that have the highest number of pins by pin type
	
	int id;
	int clsNumCols;
	int clsNumRows;

	DBU clsBinSize;
	double clsTargetDensity;
	double clsAbu;
	
	bool clsHasRowBounds : 1;
	bool clsHasBlockages : 1;
public:
	DensityGridData() {
		clsMaxAreaBin.resize(NUM_AREAS, -1);
		clsMaxPinBin.resize(NUM_PINS, -1);
		id = 0;
		clsNumCols = 0;
		clsNumRows = 0;
		clsBinSize = 0;
		clsTargetDensity = 0.0;
		clsAbu = 0.0;
		clsHasRowBounds = false;
		clsHasBlockages = false;
	} // end constructor 
}; // end class 

} // end namespace 

#endif /* GRIDAREADATA_H */

