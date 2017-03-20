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
 * File:   DefDescriptors.h
 * Author: jucemar
 *
 * Created on 6 de Julho de 2016, 18:24
 */

#ifndef DEFDESCRIPTORS_H
#define DEFDESCRIPTORS_H

#include "rsyn/util/Bounds.h"
#include "rsyn/util/dbu.h"

static const std::string INVALID_DEF_NAME = "*<INVALID_DEF_NAME>*";

class DefRowDscp {
public:
	std::string clsName;
	std::string clsSite;
	DBUxy clsOrigin;
	std::string clsOrientation;
	int clsNumX;
	int clsNumY;
	int clsStepX;
	int clsStepY;
	DefRowDscp() :  clsName(INVALID_DEF_NAME), clsSite(INVALID_DEF_NAME), 
		clsOrientation(INVALID_DEF_NAME), clsNumX(0), clsNumY(0), 
		clsStepX(0), clsStepY(0) {}
}; // end class 

// -----------------------------------------------------------------------------

class DefComponentDscp {
public:
	std::string clsName;
	std::string clsMacroName;
	std::string clsLocationType; // {FIXED | COVER | PLACED | UNPLACED }
	DBUxy clsPos;
	std::string clsOrientation;
	bool clsIsFixed;
	bool clsIsPlaced;
	DefComponentDscp() : clsName(INVALID_DEF_NAME), clsMacroName(INVALID_DEF_NAME),
		clsLocationType(INVALID_DEF_NAME), clsOrientation(INVALID_DEF_NAME), 
		clsIsFixed(false), clsIsPlaced(false) {}
}; // end class 

// -----------------------------------------------------------------------------

class DefPortDscp {
public:
	std::string clsName;
	std::string clsNetName;
	std::string clsDirection; // {INPUT | OUTPUT | INOUT | FEEDTHRU}
	std::string clsLocationType; // {FIXED | COVER | PLACED }
	DBUxy clsPos;
	DBUxy clsICCADPos;
	std::string clsOrientation;
	std::string clsLayerName;
	Bounds clsLayerBounds;
	DefPortDscp() : clsName(INVALID_DEF_NAME), clsNetName(INVALID_DEF_NAME),
		clsDirection(INVALID_DEF_NAME), clsLocationType(INVALID_DEF_NAME),
		clsOrientation(INVALID_DEF_NAME), clsLayerName(INVALID_DEF_NAME) {}
}; // end class 

// -----------------------------------------------------------------------------

class DefNetDscp {
public: 
	std::string clsName;
	DefNetDscp() : clsName(INVALID_DEF_NAME) {}
}; // end class 

// -----------------------------------------------------------------------------

class DefDscp {
public:
	double clsVersion;
	std::string clsDeviderChar;
	std::string clsBusBitChars;
	std::string clsDesignName;
	Bounds clsDieBounds;
	int clsDatabaseUnits;
	std::vector<DefRowDscp> clsRows;
	std::vector<DefComponentDscp> clsComps;
	std::vector<DefPortDscp> clsPorts;
	std::vector<DefNetDscp> clsNets;
	DefDscp() : clsVersion(0.0), clsDeviderChar(INVALID_DEF_NAME), 
		clsBusBitChars(INVALID_DEF_NAME), clsDesignName(INVALID_DEF_NAME), 
		clsDatabaseUnits(0) {}
}; // end class 

// -----------------------------------------------------------------------------

#endif /* DEFDESCRIPTORS_H */

