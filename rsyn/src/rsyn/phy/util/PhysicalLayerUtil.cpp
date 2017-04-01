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

#include "PhysicalUtil.h"

namespace Rsyn {

static const std::string NULL_PHY_NAME = "*<NULL_PHY_NAME>*";
static const std::string GEN_NAME = "*<GEN_NAME>*_";
static const std::string INVALID_NAME = "*<INVALID_NAME>*";

Rsyn::PhysicalLayerType getPhysicalLayerType(const std::string & type){
	if(type.compare("ROUTING") == 0) return ROUTING;
	if(type.compare("CUT") == 0) return CUT;
	if(type.compare("OVERLAP") == 0) return OVERLAP;
	return INVALID_PHY_LAYER_TYPE;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalLayerType(const Rsyn::PhysicalLayerType type) {
 	switch(type) {
		case ROUTING: return "ROUTING";
		case CUT: return "CUT";
		case OVERLAP: return "OVERLAP";
		default: return Rsyn::getPhysicalInvalidName();
	} // end switch 
} // end method 

// -----------------------------------------------------------------------------

Rsyn::PhysicalLayerDirection getPhysicalLayerDirection(const std::string & direction) {
	if(direction.compare("HORIZONTAL") == 0) return HORIZONTAL;
	if(direction.compare("VERTICAL") == 0) return VERTICAL;
	return INVALID_PHY_LAYER_DIRECTION;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalLayerDirection(const PhysicalLayerDirection direction) {
	switch(direction) {
		case HORIZONTAL : return "HORIZONTAL";
		case VERTICAL : return "VERTICAL";
		default: return Rsyn::getPhysicalInvalidName();
	} // end switch 
} // end method 

// -----------------------------------------------------------------------------

Rsyn::PhysicalOrientation getPhysicalOrientation(const std::string &orientation) {
	if(orientation.compare("N") == 0) return ORIENTATION_N;
	if(orientation.compare("S") == 0) return ORIENTATION_S;
	if(orientation.compare("E") == 0) return ORIENTATION_E;
	if(orientation.compare("W") == 0) return ORIENTATION_W;
	
	if(orientation.compare("FN") == 0) return ORIENTATION_FN;
	if(orientation.compare("FS") == 0) return ORIENTATION_FS;
	if(orientation.compare("FE") == 0) return ORIENTATION_FE;
	if(orientation.compare("FW") == 0) return ORIENTATION_FW;
	
	return ORIENTATION_INVALID;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalOrientation(const Rsyn::PhysicalOrientation orientation) {
	switch (orientation) {
		case ORIENTATION_N: return "N";
		case ORIENTATION_S: return "S";
		case ORIENTATION_E: return "E";
		case ORIENTATION_W: return "W";
			
		case ORIENTATION_FN: return "FN";
		case ORIENTATION_FS: return "FS";
		case ORIENTATION_FE: return "FE";
		case ORIENTATION_FW: return "FW";

		default: return Rsyn::getPhysicalInvalidName();
	} // end switch 
} // end method 

// -----------------------------------------------------------------------------

Rsyn::PhysicalMacroClass getPhysicalMacroClass(const std::string & macroClass) {
	if(macroClass.compare("COVER") == 0) return MACRO_COVER;
	if(macroClass.compare("RING") == 0) return MACRO_RING;
	if(macroClass.compare("BLOCK") == 0) return MACRO_BLOCK;
	if(macroClass.compare("PAD") == 0) return MACRO_PAD;
	if(macroClass.compare("CORE") == 0) return MACRO_CORE;
	if(macroClass.compare("ENDCAP") == 0) return MACRO_ENDCAP;
	
	return MACRO_INVALID_CLASS;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalMacroClass(const Rsyn::PhysicalMacroClass macroClass) {
	switch(macroClass) {
		case MACRO_COVER : return "COVER";
		case MACRO_RING: return "RING";
		case MACRO_BLOCK : return "BLOCK";
		case MACRO_PAD : return "PAD";
		case MACRO_CORE : return "CORE";
		case MACRO_ENDCAP : return "ENDCAP";
		default: return Rsyn::getPhysicalInvalidName();
	} // end switch
} // end method 

// -----------------------------------------------------------------------------

Rsyn::PhysicalSymmetry getPhysicalSymmetry(const std::string &rowSymmetry) {
	if(rowSymmetry.compare("X") == 0) return SYMMETRY_X;
	if(rowSymmetry.compare("Y") == 0) return SYMMETRY_Y;
	
	return SYMMETRY_INVALID;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalSymmetry(const Rsyn::PhysicalSymmetry rowSymmetry) {
	switch(rowSymmetry) {
		case SYMMETRY_X : return "X";
		break;
		case SYMMETRY_Y : return "Y";
		break;
		default : return Rsyn::getPhysicalInvalidName();
	} // end switch 
} // end method 


Rsyn::PhysicalSiteClass getPhysicalSiteClass(const std::string & siteClass) {
	if(siteClass.compare("CORE") == 0) return PhysicalSiteClass::CORE;
	if(siteClass.compare("PAD") == 0) return PhysicalSiteClass::PAD;
	
	return PhysicalSiteClass::INVALID_SITECLASS;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalSiteClass( const Rsyn::PhysicalSiteClass siteClass) { 
	switch(siteClass) {
		case CORE : return "CORE";
		break;
		case PAD : return "PAD";
		break;
		default : return Rsyn::getPhysicalInvalidName();
	} // end switch 
} // end method 


Rsyn::PhysicalPinDirection getPhysicalPinDirection ( const std::string &direction) {
	if(direction.compare("INPUT") == 0) return PIN_INPUT;
	if(direction.compare("OUTPUT") == 0) return PIN_OUTPUT;
	
	return PIN_INVALID_DIRECTION;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalPinDirection (const Rsyn::PhysicalPinDirection pinDirection) {
	switch(pinDirection) {
		case PIN_INPUT : return "INPUT";
		case PIN_OUTPUT : return "OUTPUT";
		default : return Rsyn::getPhysicalInvalidName();
	} // end switch 
} // end method 


Rsyn::PhysicalPinPortClass getPhysicalPinPortClass ( const std::string & portClass) {
	if(portClass.compare("NONE") == 0) return PINPORTCLASS_NONE;
	if(portClass.compare("CORE") == 0) return PINPORTCLASS_CORE;
	if(portClass.compare("BUMP") == 0) return PINPORTCLASS_BUMP;

	return PINPORTCLASS_INVALID;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalPinPortClass ( const Rsyn::PhysicalPinPortClass portClass) {
	switch(portClass) {
		case PINPORTCLASS_NONE : return "NONE";
		case PINPORTCLASS_CORE : return "CORE";
		case PINPORTCLASS_BUMP : return "BUMP";
		default : return Rsyn::getPhysicalInvalidName();
	} // end switch 
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalGeneratedNamePrefix() {
	return GEN_NAME;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalInvalidPrefix() {
	return INVALID_NAME;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalInvalidName() {
	return INVALID_NAME;
} // end method 

// -----------------------------------------------------------------------------

std::string getPhysicalNullName() {
	return NULL_PHY_NAME;
} // end method 

// -----------------------------------------------------------------------------

} // end namespace 

