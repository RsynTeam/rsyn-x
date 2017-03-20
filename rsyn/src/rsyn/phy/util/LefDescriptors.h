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
 * File:   PhysicalDescriptors.h
 * Author: jucemar
 *
 * Created on 6 de Julho de 2016, 15:29
 */


#ifndef LEFDESCRIPTORS_H
#define LEFDESCRIPTORS_H

#include <map>

#include "rsyn/util/DoubleRectangle.h"
#include "rsyn/util/double2.h"

// NOTE: Guilherme Flach - 2016/11/04
// I did not change here from double to dbu because the data is load from
// lef as is and therefore it's in micron. The conversion to dbu is currently
// done in the physical layer initialization.

static const std::string INVALID_LEF_NAME = "*<INVALID_LEF_NAME>*";

// -----------------------------------------------------------------------------

class LefPolygonDscp {
public:
	std::vector<double2> clsPolygonPoints;
}; // end class 

// -----------------------------------------------------------------------------

class LefPortDscp {
public:
	std::string clsMetalName;
	std::vector<DoubleRectangle> clsBounds;
	std::vector<LefPolygonDscp> clsLefPolygonDscp;
	LefPortDscp() : clsMetalName(INVALID_LEF_NAME) {}
}; // end class 

// -----------------------------------------------------------------------------

class LefPinDscp {
public:
	std::string clsPinName;
	std::string clsPinDirection;
	DoubleRectangle clsBounds;
	bool clsHasPort;
	std::vector<LefPortDscp> clsPorts;
	LefPinDscp() : clsPinName(INVALID_LEF_NAME), 
		clsPinDirection(INVALID_LEF_NAME), clsHasPort(false) {}
}; // end class 

// -----------------------------------------------------------------------------

class LefObsDscp {
public:
	std::string clsMetalLayer;
	std::vector<DoubleRectangle> clsBounds;
	LefObsDscp() : clsMetalLayer(INVALID_LEF_NAME) {}
};

// -----------------------------------------------------------------------------

class LefMacroDscp {
public:
	std::string clsMacroName;
	std::string clsMacroClass;
	double2 clsOrigin;
	double2 clsSize;
	std::vector<std::string> clsSymmetry;
	std::string clsSite;
	std::vector<LefPinDscp> clsPins;
	std::vector<LefObsDscp> clsObs;
	LefMacroDscp() : clsMacroName(INVALID_LEF_NAME), clsMacroClass(INVALID_LEF_NAME),
		clsSite(INVALID_LEF_NAME) {}
}; // end class 

// -----------------------------------------------------------------------------

class LefLayerDscp {
public:
	std::string clsName;
	std::string clsType;
	std::string clsDirection;
	double clsPitch;
	double clsOffset;
	double clsWidth;
	double clsSpacing;
	LefLayerDscp() : clsName(INVALID_LEF_NAME), clsType(INVALID_LEF_NAME), clsDirection(INVALID_LEF_NAME),
	clsPitch(0.0), clsOffset(0.0), clsWidth(0.0), clsSpacing(0.0) {}
}; // end class 

// -----------------------------------------------------------------------------

class LefSiteDscp {
public:
	std::string clsName;
	double2 clsSize;
	bool clsHasClass;
	std::string clsSiteClass;
	LefSiteDscp() : clsName(INVALID_LEF_NAME), clsHasClass(false), 
		clsSiteClass(INVALID_LEF_NAME) {}
}; // end class 

// -----------------------------------------------------------------------------

class LefSpacingDscp {
public:
	std::string clsLayer1;
	std::string clsLayer2;
	double clsDistance;
	LefSpacingDscp() : clsLayer1(INVALID_LEF_NAME), clsLayer2(INVALID_LEF_NAME),
		clsDistance(0.0) {}
}; // end class 

// -----------------------------------------------------------------------------

class LefUnitsDscp {
public:
	bool clsHasTime;
	bool clsHasCapacitance;
	bool clsHasResitance;
	bool clsHasPower;
	bool clsHasCurrent;
	bool clsHasVoltage;
	bool clsHasDatabase;
	bool clsHasFrequency;
	
	int clsTime;
	int clsCapacitance;
	int clsResitance;
	int clsPower;
	int clsCurrent;
	int clsVoltage;
	int clsDatabase;
	int clsFrequency;

	LefUnitsDscp() {
		clsHasTime = false;
		clsHasCapacitance = false;
		clsHasResitance = false;
		clsHasPower = false;
		clsHasCurrent = false;
		clsHasVoltage = false;
		clsHasDatabase = false;
		clsHasFrequency = false;
		clsTime = 0;
		clsCapacitance = 0;
		clsResitance = 0;
		clsPower = 0;
		clsCurrent = 0;
		clsVoltage = 0;
		clsDatabase = 100; // default value at LEF/DEF reference
		clsFrequency = 0;
	} // end constructor
}; // end class 

// -----------------------------------------------------------------------------

class LefDscp {
public:
	int clsMajorVersion;
	int clsMinorVersion;
	std::string clsCaseSensitive;
	std::string clsBusBitChars;
	std::string clsDivideChar;
	double clsManufactGrid;
	LefUnitsDscp clsLefUnitsDscp;
	std::vector<LefSiteDscp> clsLefSiteDscps;
	std::vector<LefLayerDscp> clsLefLayerDscps;
	std::vector<LefMacroDscp> clsLefMacroDscps;
	std::vector<LefSpacingDscp> clsLefSpacingDscps;
	LefDscp() : clsMajorVersion(0), clsMinorVersion(0), clsCaseSensitive(INVALID_LEF_NAME),
		clsBusBitChars(INVALID_LEF_NAME), clsManufactGrid(0.0) {}
}; // end class 

// -----------------------------------------------------------------------------

#endif /* PHYSICALDESCRIPTORS_H */

