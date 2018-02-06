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
 
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   abu.h
 * Author: jucemar
 *
 * Created on 20 de Setembro de 2016, 15:11
 */

#ifndef DENSITYGRID_H
#define DENSITYGRID_H

#include <Rsyn/PhysicalDesign>
#include "rsyn/core/Rsyn.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/Proxy.h"
#include <vector>


namespace Rsyn {

enum AreaType : std::int8_t {
	INVALID_AREA_TYPE = -1,
	FIXED_AREA = 0, /* Area summation of the FIXED cells that have intersection to the bin */
	MOVABLE_AREA = 1, /* Area summation of the MOVABLE cells that have intersection to the bin */
	PLACEABLE_AREA = 2, /* Area summation of the ROWS that have intersection to the bin */
	NUM_AREAS = 3
}; // end enum 

enum PinType : std::int8_t {
	INVALID_PIN_TYPE = -1,
	FIXED_PIN = 0, /* Pins from a FIXED cell that are inside of the bin */
	MOVABLE_PIN = 1, /* Pins from a MOVABLE cell that are inside of the bin */
	BLOCK_PIN = 2, /* Pins from a BLOCK cell that are inside of the bin */
	CONNECTED_PIN = 3, /* Pins from a cell CONNECTED to a net that are inside of the bin */
	NUM_PINS = 4
}; // end enum 

class DensityGridBlockage;
class DensityGridBin;
class DensityGridData;

class DensityGrid;

} // end namespace 

#include "rsyn/model/congestion/DensityGrid/decl/DensityGrid.h"

#include "rsyn/model/congestion/DensityGrid/data/DensityGridBin.h"
#include "rsyn/model/congestion/DensityGrid/data/DensityGridBlockage.h"
#include "rsyn/model/congestion/DensityGrid/data/DensityGridData.h"

#include "rsyn/model/congestion/DensityGrid/impl/DensityGrid.h"


#endif /* DENSITYGRID_H */

