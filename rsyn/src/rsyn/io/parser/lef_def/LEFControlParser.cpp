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
 


#include "LEFControlParser.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif /* not WIN32 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <map>


//#include "rsyn/io/legacy/ispd13/global.h"
#include "rsyn/util/DoubleRectangle.h"
#include "rsyn/util/double2.h"
//LEF headers

#include "lef5.8/lefrReader.hpp"
#include "lef5.8/lefwWriter.hpp"
#include "lef5.8/lefiDebug.hpp"
#include "lef5.8/lefiUtil.hpp"

// -----------------------------------------------------------------------------

LEFControlParser::LEFControlParser() {
	std::setlocale(LC_ALL, "en_US.UTF-8");

} // end constructor

// -----------------------------------------------------------------------------

LEFControlParser::~LEFControlParser() {

} // end destructor

// -----------------------------------------------------------------------------

// =============================================================================
// LEF/DEF Common Function for parser
// =============================================================================

//void freeCB(void* name);
//void* mallocCB(size_t size);
//void* reallocCB(void* name, int size);

void* mallocCB(size_t size) { // DEF Parser version 5.8 
	return malloc(size);
}

void* mallocCB(int size) { // LEF Parser version 5.8 
	return malloc(size);
}

// -----------------------------------------------------------------------------

void* reallocCB(void* name, size_t size) { // DEF Parser version 5.8 
	return realloc(name, size);
}

void* reallocCB(void* name, int size) { // LEF Parser version 5.8 
	return realloc(name, size);
}

// -----------------------------------------------------------------------------

void freeCB(void* name) {
	free(name);
	return;
}

// -----------------------------------------------------------------------------



//LEF CALLBACKS
void lefCheckType(lefrCallbackType_e c);
int lefMacroBeginCB(lefrCallbackType_e c, const char* macroName, lefiUserData ud);
int lefMacroEndCB(lefrCallbackType_e c, const char* macroName, lefiUserData ud);
int lefMacroCB(lefrCallbackType_e c, lefiMacro* macro, lefiUserData ud);
int lefPinCB(lefrCallbackType_e c, lefiPin* pin, lefiUserData ud);
int lefSiteCB(lefrCallbackType_e c, lefiSite* site, lefiUserData ud);
int lefUnits(lefrCallbackType_e c, lefiUnits* units, lefiUserData ud);
int lefObstructionCB(lefrCallbackType_e c, lefiObstruction* obs, lefiUserData ud);
int lefLayerCB(lefrCallbackType_e c, lefiLayer* layer, lefiUserData ud);
int lefSpacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData ud);


LefDscp &getLibraryFromUserData(lefiUserData userData) {
	return *((LefDscp *) userData);
} // end function

// =============================================================================
// LEF Function Implementation
// =============================================================================

void LEFControlParser::parseLEF(const std::string &filename, LefDscp & dscp) {
//	int retStr = 0;  unused variable

	FILE* lefFile;
	int res;

	lefrInit();

	lefrReset();

	(void) lefrSetOpenLogFileAppend();

	(void) lefrSetShiftCase(); // will shift name to uppercase if caseinsensitive
	// is set to off or not set

	lefrSetMacroBeginCbk(lefMacroBeginCB);
	lefrSetMacroEndCbk(lefMacroEndCB);
	lefrSetMacroCbk(lefMacroCB);
	lefrSetPinCbk(lefPinCB);
	lefrSetSiteCbk(lefSiteCB);
	lefrSetUnitsCbk(lefUnits);
	lefrSetUserData((void*) 3);
	lefrSetMallocFunction(mallocCB);
	lefrSetReallocFunction(reallocCB);
	lefrSetFreeFunction(freeCB);
	lefrSetObstructionCbk(lefObstructionCB);
	lefrSetLayerCbk(lefLayerCB); 
	lefrSetSpacingCbk(lefSpacingCB);
	
	lefrSetRegisterUnusedCallbacks();

	// Open the lef file for the reader to read
	if ((lefFile = fopen(filename.c_str(), "r")) == 0) {
		printf("Couldn’t open input file ’%s’\n", filename.c_str());
		exit(1);
	}

	// Invoke the parser
	res = lefrRead(lefFile, filename.c_str(), (void*) &dscp);
	if (res != 0) {
		printf("LEF parser returns an error. #: %d \n", res);

		return;
	}
	//(void) lefrPrintUnusedCallbacks(fout);
	(void) lefrReleaseNResetMemory();
	fclose(lefFile);
} // end method 

// -----------------------------------------------------------------------------

void lefCheckType(lefrCallbackType_e c) {
	if (c >= 0 && c <= lefrLibraryEndCbkType) {
		// OK
	} else {

		printf("ERROR: callback type is out of bounds!\n");
	}
} // end call back 

// -----------------------------------------------------------------------------

int lefMacroBeginCB(lefrCallbackType_e c, const char* macroName, lefiUserData ud) {
	LefDscp & dscp = getLibraryFromUserData(ud);
	dscp.clsLefMacroDscps.resize(dscp.clsLefMacroDscps.size() + 1);
	return 0;
} // end function

// -----------------------------------------------------------------------------

int lefMacroEndCB(lefrCallbackType_e c, const char* macroName, lefiUserData ud) {
	return 0;
} // end function

// -----------------------------------------------------------------------------

int lefMacroCB(lefrCallbackType_e c, lefiMacro* macro, lefiUserData ud) {
	LefDscp & dscp = getLibraryFromUserData(ud);
	
	LefMacroDscp & lefMacro = dscp.clsLefMacroDscps.back();
	
	lefMacro.clsMacroClass = macro->macroClass();
	lefMacro.clsMacroName = macro->name();
	lefMacro.clsSite = macro->siteName();
	lefMacro.clsSize[X] = macro->sizeX();
	lefMacro.clsSize[Y] = macro->sizeY();
	
	return 0;
} // end function

// -----------------------------------------------------------------------------

int lefPinCB(lefrCallbackType_e c, lefiPin* pin, lefiUserData ud) {
	//Library &library = getLibraryFromUserData(ud);
	
	// Skip power and ground pins...
	if (strcmp(pin->use(), "GROUND") == 0) return 0;
	if (strcmp(pin->use(), "POWER") == 0) return 0;
		
	LefDscp & dscp = getLibraryFromUserData(ud);
	LefMacroDscp & lefMacro = dscp.clsLefMacroDscps.back();
	lefMacro.clsPins.resize(lefMacro.clsPins.size() + 1);
	LefPinDscp & lefPin = lefMacro.clsPins.back();
	
	lefPin.clsPinName = pin->name();
	lefPin.clsPinDirection = pin->direction();
	lefPin.clsHasPort = pin->numPorts() > 0 ;
	
	if(lefPin.clsHasPort)
		lefPin.clsPorts.reserve(pin->numPorts());
	
	for (int j = 0; j < pin->numPorts(); j++) {
		const lefiGeometries* geometry = pin->port(j);
		lefPin.clsPorts.push_back(LefPortDscp());
		for (int i = 0; i < geometry->numItems(); i++) {
			LefPortDscp & lefPort = lefPin.clsPorts.back();
			if (geometry->itemType(i) == lefiGeomLayerE)
				lefPort.clsMetalName = geometry->lefiGeometries::getLayer(i);
			else if (geometry->itemType(i) == lefiGeomRectE) {

				const lefiGeomRect* rect = geometry->getRect(i);
				lefPort.clsBounds.resize(lefPort.clsBounds.size() + 1);
				DoubleRectangle &bound = lefPort.clsBounds.back();
				bound.updatePoints(rect->xl, rect->yl, rect->xh, rect->yh);
			} else if(geometry->itemType(i) == lefiGeomPolygonE) {
				const lefiGeomPolygon * poly = geometry->lefiGeometries::getPolygon(i);
				lefPort.clsLefPolygonDscp.push_back(LefPolygonDscp());
				LefPolygonDscp & polyDscp = lefPort.clsLefPolygonDscp.back();
				polyDscp.clsPolygonPoints.resize(poly->numPoints, 
					double2(std::numeric_limits<double>::infinity(),
					std::numeric_limits<double>::infinity()));
				for(int i = 0; i < poly->numPoints; i++) {
					double2 & pt = polyDscp.clsPolygonPoints[i];
					pt[X] = poly->x[i];
					pt[Y] = poly->y[i];
				}  // end for 
			} else {
				std::cout<<"WARNING: function "<<__func__<<" does not supports pin geometry type in the LEF Parser Control.\n";
			} // end else-if 
		} // end for
	} // end for 
	return 0;
} // end function

// -----------------------------------------------------------------------------

int lefSiteCB(lefrCallbackType_e c, lefiSite* site, lefiUserData ud) {
	
	LefDscp & dscp = getLibraryFromUserData(ud);
	dscp.clsLefSiteDscps.resize(dscp.clsLefSiteDscps.size() + 1);
	LefSiteDscp &lefSite = dscp.clsLefSiteDscps.back();
	lefSite.clsName = site->name();
	
	lefSite.clsHasClass = site->hasClass();
	if(site->hasClass())
		lefSite.clsSiteClass = site->siteClass();
	lefSite.clsSize[X] = site->sizeX();
	lefSite.clsSize[Y] = site->sizeY();

	return 0;
} // end function

// -----------------------------------------------------------------------------

int lefUnits(lefrCallbackType_e c, lefiUnits* units, lefiUserData ud) {
	LefDscp & dscp = getLibraryFromUserData(ud);
	LefUnitsDscp & lefUnits = dscp.clsLefUnitsDscp;
	
	if(units->hasDatabase()){
		lefUnits.clsDatabase = (int) units->databaseNumber();
		lefUnits.clsHasDatabase = true;
	}

	return 0;
} // end function

// -----------------------------------------------------------------------------

int lefObstructionCB(lefrCallbackType_e c, lefiObstruction* obs, lefiUserData ud) {
	LefDscp & dscp = getLibraryFromUserData(ud);
	LefMacroDscp & lefMacro = dscp.clsLefMacroDscps.back();
	
	lefiGeometries *geometry;
	lefiGeomRect *rect;
	
	geometry = obs->lefiObstruction::geometries();
	const int numItems = geometry->lefiGeometries::numItems();
	for (int i = 0; i < numItems; i++) {
		if(geometry->lefiGeometries::itemType(i) == lefiGeomLayerE) {
			lefMacro.clsObs.resize(lefMacro.clsObs.size() + 1);
			LefObsDscp & lefObs = lefMacro.clsObs.back();
			lefObs.clsMetalLayer = geometry->getLayer(i);
		}else if(geometry->lefiGeometries::itemType(i) == lefiGeomRectE){
			LefObsDscp & lefObs = lefMacro.clsObs.back();
			rect = geometry->lefiGeometries::getRect(i);
			DoubleRectangle libRect = DoubleRectangle (rect->xl, rect->yl, rect->xh, rect->yh);
			lefObs.clsBounds.push_back(libRect);
		} // end if-else 
	} // end for 
	return 0;
} // end method 

// -----------------------------------------------------------------------------

int lefLayerCB(lefrCallbackType_e c, lefiLayer* layer, lefiUserData ud) {
	LefDscp & dscp = getLibraryFromUserData(ud);
	dscp.clsLefLayerDscps.resize(dscp.clsLefLayerDscps.size() + 1);
	LefLayerDscp & lefLayer = dscp.clsLefLayerDscps.back();
	lefLayer.clsName = layer->lefiLayer::name();
	if (layer->lefiLayer::hasType())
		lefLayer.clsType = layer->lefiLayer::type();
	if (layer->lefiLayer::hasPitch())
		lefLayer.clsPitch = layer->lefiLayer::pitch();
	if (layer->lefiLayer::hasWidth())
		lefLayer.clsWidth = layer->lefiLayer::width();
	if (layer->lefiLayer::hasDirection())
		lefLayer.clsDirection = layer->lefiLayer::direction();
	if (layer->lefiLayer::hasSpacingNumber())
		lefLayer.clsSpacing = layer->lefiLayer::spacing(0);
	return 0;
} // end method 

// -----------------------------------------------------------------------------

int lefSpacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData ud) {
	LefDscp & dscp = getLibraryFromUserData(ud);
	dscp.clsLefSpacingDscps.resize(dscp.clsLefSpacingDscps.size() + 1);
	LefSpacingDscp & lefSpacing = dscp.clsLefSpacingDscps.back();
	lefSpacing.clsLayer1 = spacing->lefiSpacing::name1();
	lefSpacing.clsLayer2 = spacing->lefiSpacing::name2();
	lefSpacing.clsDistance = spacing->lefiSpacing::distance();
	return 0;
} // end method 

// -----------------------------------------------------------------------------