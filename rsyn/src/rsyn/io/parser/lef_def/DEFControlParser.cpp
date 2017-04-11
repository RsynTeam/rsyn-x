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
 
#include "DEFControlParser.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif /* not WIN32 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

// DEF headers
#include "def5.8/defiComponent.hpp"
#include "def5.8/defiNet.hpp"
#include "def5.8/defiRowTrack.hpp"

#include "def5.8/defwWriter.hpp"

#include "def5.8/defrReader.hpp"

extern void freeCB(void* name);
extern void* mallocCB(size_t size);
extern void* reallocCB(void* name, size_t size);

#include "rsyn/io/legacy/ispd13/global.h"
#include "rsyn/util/Bounds.h"

// -----------------------------------------------------------------------------

DEFControlParser::DEFControlParser() {
	std::setlocale(LC_ALL, "en_US.UTF-8");
} // end constructor

// -----------------------------------------------------------------------------

DEFControlParser::~DEFControlParser() {
	
} // end destructor

// -----------------------------------------------------------------------------

//DEF CALLBACKS
char* defAddress(const char* in);
void defCheckType(defrCallbackType_e c);
int defPin(defrCallbackType_e, defiPin *pin, defiUserData ud);
int defCompf(defrCallbackType_e c, defiComponent* co, defiUserData ud);
int defComponentStart(defrCallbackType_e c, int num, defiUserData ud);
int defDesignName(defrCallbackType_e c, const char* string, defiUserData ud);
int defEndFunc(defrCallbackType_e c, void* dummy, defiUserData ud);
int defExt(defrCallbackType_e t, const char* c, defiUserData ud);
int defNetStart(defrCallbackType_e c, int num, defiUserData ud);
int defNetf(defrCallbackType_e c, defiNet* net, defiUserData ud);
int defUnits(defrCallbackType_e c, double d, defiUserData ud);
int defVersion(defrCallbackType_e c, double d, defiUserData ud);
int defRow(defrCallbackType_e type, defiRow* rowInfo, defiUserData userData);
char* defOrientStr(int orient);
int defOrient(std::string orient);
int defDieArea( defrCallbackType_e typ, defiBox* box, defiUserData ud);
int defTrack (defrCallbackType_e typ, defiTrack * track, defiUserData data);
int defGCellGrid(defrCallbackType_e typ, defiGcellGrid * gCell, defiUserData data);
int defRegion(defrCallbackType_e type, defiRegion* region, defiUserData ud);
int defRegionStart(defrCallbackType_e c, int num, defiUserData ud);
int defGroupStart(defrCallbackType_e c, int num, defiUserData ud);
int defGroupMember (defrCallbackType_e type, const char* name, defiUserData userData);
int defGroupName(defrCallbackType_e type, const char* name, defiUserData ud);
int defGroups(defrCallbackType_e type, defiGroup *group, defiUserData ud);

DefDscp &getDesignFromUserData(defiUserData userData) {
	return *((DefDscp *) userData);
} // end function

// =============================================================================
// DEF Function Implementation
// =============================================================================

void DEFControlParser::parseDEF(const std::string &filename, DefDscp &defDscp) {
	defrInit();
	defrReset();
	
	//defrSetAddPathToNet();
	defrSetComponentCbk(defCompf);
	defrSetDesignCbk(defDesignName);
	defrSetPinCbk(defPin);
	defrSetUnitsCbk(defUnits);
	defrSetVersionCbk(defVersion);
	defrSetRowCbk(defRow);
	defrSetComponentStartCbk(defComponentStart);
	defrSetDieAreaCbk(defDieArea);
	defrSetMallocFunction(mallocCB);
	defrSetReallocFunction(reallocCB);
	defrSetFreeFunction(freeCB);
	defrSetNetStartCbk(defNetStart);
	defrSetNetCbk(defNetf);
	defrSetGroupsStartCbk(defGroupStart);
	defrSetGroupNameCbk(defGroupName);
	defrSetGroupMemberCbk(defGroupMember);
	defrSetGroupCbk(defGroups);
//	defrSetTrackCbk(defTrack);
//	defrSetGcellGridCbk(defGCellGrid);
	defrSetRegionStartCbk(defRegionStart);
	defrSetRegionCbk(defRegion);
	
	//defrSetGcellGridWarnings(3);

	FILE * f;
	int res;

	(void) defrSetOpenLogFileAppend();
	

	
	if ((f = fopen(filename.c_str(), "r")) == 0) {
		printf("Couldn't open input file '%s'\n", filename.c_str());
		return;
	}
	// Set case sensitive to 0 to start with, in History & PropertyDefinition
	// reset it to 1.
	res = defrRead(f, filename.c_str(), (void*) &defDscp, 1);

	if (res)
		printf("Reader returns bad status. %s\n", filename.c_str());

	//(void) defrPrintUnusedCallbacks(fout);
	(void) defrReleaseNResetMemory();
	(void) defrUnsetNonDefaultCbk();
	(void) defrUnsetNonDefaultStartCbk();
	(void) defrUnsetNonDefaultEndCbk();
	defrClear();
	
}

// -----------------------------------------------------------------------------

char* defAddress(const char* in) {
	return ((char*) in);
} // end method

// -----------------------------------------------------------------------------

void defCheckType(defrCallbackType_e c) {
	if (c >= 0 && c <= defrDesignEndCbkType) {
		// OK
	} else {
		printf("ERROR: callback type is out of bounds!\n");
	}
} // end method

// -----------------------------------------------------------------------------

int defRow(defrCallbackType_e type, defiRow* rowInfo, defiUserData userData) {
	DefDscp &defDscp = getDesignFromUserData(userData);
	defDscp.clsRows.resize(defDscp.clsRows.size() +1);
	DefRowDscp &defRow = defDscp.clsRows.back();
	defRow.clsName = rowInfo->name();
	defRow.clsSite = rowInfo->macro();
	defRow.clsOrigin[X] = static_cast<DBU>(rowInfo->x());
	defRow.clsOrigin[Y] = static_cast<DBU>(rowInfo->y());

	if (rowInfo->hasDoStep()) {
		defRow.clsNumX = static_cast<int>(rowInfo->xNum());
		defRow.clsNumY = static_cast<int>(rowInfo->yNum());
		defRow.clsStepX = static_cast<int>(rowInfo->xStep());
		defRow.clsStepY = static_cast<int>(rowInfo->yStep());
	} else if (rowInfo->hasDo()) {
		defRow.clsNumX = static_cast<int>(rowInfo->xNum());
		defRow.clsNumY = static_cast<int>(rowInfo->yNum());
		defRow.clsStepX = 0;
		defRow.clsStepY = 0;
	} else {
		defRow.clsNumX = 1;
		defRow.clsNumY = 1;
		defRow.clsStepX = 0;
		defRow.clsStepY = 0;
	} // end else
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defPin(defrCallbackType_e, defiPin *pin, defiUserData userData) {
	DefDscp &defDscp = getDesignFromUserData(userData);
	defDscp.clsPorts.resize(defDscp.clsPorts.size() + 1);
	DefPortDscp &defPin = defDscp.clsPorts.back();
	
	defPin.clsName = pin->pinName();
	defPin.clsNetName = pin->netName();
	defPin.clsDirection = pin->direction();
	defPin.clsPos[X] = pin->placementX();
	defPin.clsPos[Y] = pin->placementY();
	defPin.clsICCADPos = defPin.clsPos;
	// pin position is correct only for iccad contest
	if(pin->hasLayer()) {
		defPin.clsLayerName = pin->layer(0);
		int xl, yl, xh, yh;
		pin->bounds(0, &xl, &yl, &xh, &yh);
		defPin.clsLayerBounds[LOWER][X] = xl;
		defPin.clsLayerBounds[LOWER][Y] = yl;
		defPin.clsLayerBounds[UPPER][X] = xh;
		defPin.clsLayerBounds[UPPER][Y] = yh;
		defPin.clsICCADPos += defPin.clsLayerBounds.computeCenter();
	} // end if 
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defCompf(defrCallbackType_e c, defiComponent* co, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsComps.push_back(DefComponentDscp());
	DefComponentDscp &defComp = defDscp.clsComps.back();
	
	defComp.clsName = DEFControlParser::unescape(co->id());
	defComp.clsMacroName = co->name();
	defComp.clsIsFixed = co->isFixed();
	defComp.clsIsPlaced = co->isPlaced();
	defComp.clsPos[X] = co->placementX();
	defComp.clsPos[Y] = co->placementY();
	defComp.clsOrientation = co->placementOrientStr();

	return 0;
} // end method

// -----------------------------------------------------------------------------

int defComponentStart(defrCallbackType_e c, int num, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsComps.reserve(num);
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defNetStart(defrCallbackType_e c, int num, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsNets.reserve(num);
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defDesignName(defrCallbackType_e c, const char* string, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsDesignName = string;
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defNetf(defrCallbackType_e c, defiNet* net, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsNets.push_back(DefNetDscp());
	DefNetDscp & netDscp = defDscp.clsNets.back();
	netDscp.clsName = net->name();
	netDscp.clsConnections.resize(net->numConnections());
	for (int i = 0; i < net->numConnections(); i++) {
		DefNetConnection &connection = netDscp.clsConnections[i];
		connection.clsPinName = net->pin(i);
		connection.clsComponentName = net->instance(i);
	} // end for

	return 0;
} // end method

// -----------------------------------------------------------------------------

int defUnits(defrCallbackType_e c, double d, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsDatabaseUnits = static_cast<int>(d);
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defVersion(defrCallbackType_e c, double d, defiUserData ud) {
	DefDscp &defDscp = getDesignFromUserData(ud);
	defDscp.clsVersion = d;
	return 0;
} // end method

// -----------------------------------------------------------------------------

char* defOrientStr(int orient) {
	switch (orient) {
		case 0: return ((char*) "N");
		case 1: return ((char*) "W");
		case 2: return ((char*) "S");
		case 3: return ((char*) "E");
		case 4: return ((char*) "FN");
		case 5: return ((char*) "FW");
		case 6: return ((char*) "FS");
		case 7: return ((char*) "FE");
	};
	return ((char*) "BOGUS");
} // end method

// -----------------------------------------------------------------------------

int defOrient(std::string orient) {
	if(orient.compare("N") == 0) return 0;
	if(orient.compare("W") == 0) return 1;
	if(orient.compare("S") == 0) return 2;
	if(orient.compare("E") == 0) return 3;
	if(orient.compare("FN") == 0) return 4;
	if(orient.compare("FW") == 0) return 5;
	if(orient.compare("FS") == 0) return 6;
	if(orient.compare("FE") == 0) return 7;
	return -1;
} // end method

// -----------------------------------------------------------------------------

int defDieArea( defrCallbackType_e typ, defiBox* box, defiUserData ud) {
	DefDscp &defDscp = getDesignFromUserData(ud);
	defDscp.clsDieBounds[LOWER][X] = box->defiBox::xl();
	defDscp.clsDieBounds[LOWER][Y] = box->defiBox::yl();
	defDscp.clsDieBounds[UPPER][X] = box->defiBox::xh();
	defDscp.clsDieBounds[UPPER][Y] = box->defiBox::yh();
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defTrack (defrCallbackType_e typ, defiTrack * track, defiUserData data) {
	
	
//	Design &design = getDesignFromUserData(data);
//
//	Design::Track designTrack;
//	if (!track->firstTrackMask()) {
//		designTrack.direction = track->macro();
//		designTrack.doStart = track->x();
//		designTrack.doCount = track->xNum();
//		designTrack.doStep = track->xStep();
//	} // end if 
//
//	for (int i = 0; i < track->numLayers(); i++) {
//		designTrack.layers.reserve(track->numLayers());
//		designTrack.layers.push_back(track->layer(i));
//	} // end for 
//	design.tracks.push_back(designTrack);
	
	return 0;
} // end method 

// -----------------------------------------------------------------------------

int defGCellGrid(defrCallbackType_e typ, defiGcellGrid * gCell, defiUserData data) {
	//Design &design = getDesignFromUserData(data);
	//cout<<"callback\n";
	//cout<<gCell->macro()<<"\n";
	/*Design::GCellGrid designGCellGrid;
	designGCellGrid.master = gCell->macro();
	designGCellGrid.doStart = gCell->x();
	designGCellGrid.doCount = gCell->xNum();
	designGCellGrid.doStep = gCell->xStep();
	design.gCellGrids.push_back(designGCellGrid);
	*/
        // printf( "GCELLGRID %s %d DO %d STEP %g ;\n",
          //       gCell->macro(), gCell->x(),
            //     gCell->xNum(), gCell->xStep());
	
	// cout<<" # "<<designGCellGrid.master <<  " " <<designGCellGrid.doStart <<" "<< designGCellGrid.doCount<<
	//	 " " << designGCellGrid.doStep<<"\n";

	return 0;
} // end method 


// -----------------------------------------------------------------------------

int defRegionStart(defrCallbackType_e c, int num, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsRegions.reserve(num);
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defRegion(defrCallbackType_e type, defiRegion* region, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsRegions.push_back(DefRegionDscp());
	DefRegionDscp & regionDscp = defDscp.clsRegions.back();
	regionDscp.clsName = region->name();
	regionDscp.clsType = region->type();
	regionDscp.clsBounds.resize(region->numRectangles());
	
	for (int i = 0; i < region->numRectangles(); i++) {
		Bounds & bounds = regionDscp.clsBounds[i];
		bounds[LOWER][X] = region->xl(i);
		bounds[LOWER][Y] = region->yl(i);
		bounds[UPPER][X] = region->xh(i);
		bounds[UPPER][Y] = region->yh(i);
	}
	return 0;
} // end method 

// -----------------------------------------------------------------------------

int defGroupStart(defrCallbackType_e c, int num, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsGroups.reserve(num);
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defGroupName(defrCallbackType_e type, const char* name, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	defDscp.clsGroups.push_back(DefGroupDscp());
	DefGroupDscp & defGroup = defDscp.clsGroups.back();
	defGroup.clsName = name;
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defGroupMember (defrCallbackType_e type, const char* name, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	DefGroupDscp & defGroup = defDscp.clsGroups.back();
	defGroup.clsPatterns.push_back(name);
	return 0;
} // end method

// -----------------------------------------------------------------------------

int defGroups(defrCallbackType_e type, defiGroup *group, defiUserData ud) {
	DefDscp & defDscp = getDesignFromUserData(ud);
	DefGroupDscp & defGroup = defDscp.clsGroups.back();
	defGroup.clsRegion = group->regionName();
	return 0;
} // end method 

// -----------------------------------------------------------------------------

#define CHECK_STATUS(status) \
	if (status) { \
		defwPrintError(status); \
	} // end if

// -----------------------------------------------------------------------------

void DEFControlParser::writeDEF(const std::string &filename, const std::string designName, const std::vector<DefComponentDscp> &components) {
	FILE * defFile;
	defFile = fopen(filename.c_str(), "w");
	if (defFile == NULL) {
		printf("ERROR: could not open output file: %s \n", filename.c_str());
	}

	int status;
	int numComponents = components.size();

	status = defwInitCbk(defFile);
	CHECK_STATUS(status);
	defwAddComment("ICCAD 2015 contest - CADA085 Team solution");

	status = defwNewLine();
	CHECK_STATUS(status);
	status = defwNewLine();
	CHECK_STATUS(status);

	status = defwVersion(5, 7);
	CHECK_STATUS(status);
	status = defwDesignName(designName.c_str());
	CHECK_STATUS(status);

	status = defwNewLine();
	CHECK_STATUS(status);

	status = defwStartComponents(numComponents);
	CHECK_STATUS(status);

	for (const DefComponentDscp & comp : components) {
		//int defwComponent(const char* name, const char* master, const char* eeq, const char* source, const char* status, int statusX, int statusY, int statusOrient, double weight, const char* region);
		status = defwComponent(comp.clsName.c_str(), comp.clsMacroName.c_str(), 
			0, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, NULL, 
			comp.clsIsFixed ? "FIXED" : "PLACED", (int) comp.clsPos[X], 
			(int) comp.clsPos[Y], 0, 0, NULL, 0, 0, 0, 0);
		CHECK_STATUS(status);
	}
	//defwEndComponents();
	status = defwEndComponents();
	CHECK_STATUS(status);

	status = defwNewLine();
	CHECK_STATUS(status);

	status = defwEnd();
	CHECK_STATUS(status);

	fclose(defFile);
} // end method 

// -----------------------------------------------------------------------------

void DEFControlParser::writeFullDEF(const string &filename, const DefDscp &defDscp) {
	cout<<"TODO " << __func__ << " in file " << __FILE__ <<"\n";
	//Opening 
	FILE * defFile;
	int status;
	defFile = fopen(filename.c_str(), "w");
	if (defFile == NULL) {
		printf("ERROR: could not open output file: %s \n", filename.c_str());
	}
	status = defwInitCbk(defFile);
	CHECK_STATUS(status);
	
	
	//writing header
	defwAddComment("ICCAD 2015 contest - CADA085 Team solution");

	status = defwNewLine();
	CHECK_STATUS(status);
	status = defwNewLine();
	CHECK_STATUS(status);
	status = defwVersion(5, 8); // TODO get from design
	CHECK_STATUS(status);
	status = defwDividerChar("/");
	CHECK_STATUS(status);
	status = defwBusBitChars("[]");
	CHECK_STATUS(status);
	status = defwDesignName(defDscp.clsDesignName.c_str());
	CHECK_STATUS(status);
	status = defwUnits(defDscp.clsDatabaseUnits);
	CHECK_STATUS(status);
	status = defwDieArea(
		(int)defDscp.clsDieBounds[LOWER][X],
		(int)defDscp.clsDieBounds[LOWER][Y],
		(int)defDscp.clsDieBounds[UPPER][X],
		(int)defDscp.clsDieBounds[UPPER][Y]);
	CHECK_STATUS(status);
	status = defwNewLine();
	CHECK_STATUS(status);
	
	for(const DefRowDscp & defRow : defDscp.clsRows){
		status = defwRow(
			defRow.clsName.c_str(),
			defRow.clsSite.c_str(),
			defRow.clsOrigin[X],
			defRow.clsOrigin[Y],
			defOrient(defRow.clsOrientation),
			defRow.clsNumX,
			defRow.clsNumY,
			defRow.clsStepX,
			defRow.clsStepY );
		CHECK_STATUS(status);
	} // end for 
	
	int numComponents = defDscp.clsComps.size();
	status = defwStartComponents(numComponents);
	CHECK_STATUS(status);

	for (const DefComponentDscp & comp : defDscp.clsComps) {
		//int defwComponent(const char* name, const char* master, const char* eeq, const char* source, const char* status, int statusX, int statusY, int statusOrient, double weight, const char* region);
		status = defwComponent(comp.clsName.c_str(), comp.clsMacroName.c_str(), 0, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, NULL, 
			comp.clsIsFixed ? "FIXED" : comp.clsIsPlaced ? "PLACED" : "UNPLACED", 
			(int) comp.clsPos[X], (int) comp.clsPos[Y], 0, 0, NULL, 0, 0, 0, 0);
		CHECK_STATUS(status);
	} // end for 
	
	status = defwEndComponents();
	CHECK_STATUS(status);
	
	status = defwNewLine();
	CHECK_STATUS(status);
	
//	int numPins = defDscp.ports.size();
//	status = defwStartPins(numPins);
//	CHECK_STATUS(status);
//	
//	for(const DefPortDscp & port : defDscp.ports){
//		status =  defwPin(port.portName.c_str(), port.netName.c_str(), port.special, 
//			(port.direction.compare("NULL") == 0 ? NULL : port.direction.c_str()), 
//			(port.use.compare("NULL") == 0 ? NULL : port.use.c_str()), 
//			(port.status.compare("NULL")==0 ? NULL : port.status.c_str()), 
//			port.statusX, port.statusY, port.orient,
//			(port.clsDefPortLayer.layerName.compare("NULL")==0 ? NULL : port.clsDefPortLayer.layerName.c_str()),
//			(int)port.clsDefPortLayer.bounds[LOWER][X],
//			(int)port.clsDefPortLayer.bounds[LOWER][Y],
//			(int)port.clsDefPortLayer.bounds[UPPER][X],
//			(int)port.clsDefPortLayer.bounds[UPPER][Y]);
//		CHECK_STATUS(status);
//	} // end for 
//	
//	status = defwEndPins();
//	CHECK_STATUS(status);
//	
//	status = defwNewLine();
//	CHECK_STATUS(status);

//	int numNets = defStruct.clsDefConnections.size();
//	status = defwStartNets(numNets);
//	CHECK_STATUS(status);
//
//	for (const DefStruct::DefConnection & conn : defStruct.clsDefConnections) {
//		//int defwComponent(const char* name, const char* master, const char* eeq, const char* source, const char* status, int statusX, int statusY, int statusOrient, double weight, const char* region);
//		status = defwNet(conn.netName.c_str()); 
//		CHECK_STATUS(status);
//		for(const DefStruct::DefPin & defPin : conn.pins){
//			status = defwNetConnection(defPin.cellName.c_str(), defPin.pinName.c_str(), 0);
//			CHECK_STATUS(status);
//		}
//		status = defwNetEndOneNet(); 
//		CHECK_STATUS(status);
//	}
//	
//	status =  defwEndNets();
//	CHECK_STATUS(status);

	
	status = defwEnd();
	CHECK_STATUS(status);

	fclose(defFile);
} // end method

// -----------------------------------------------------------------------------

std::string DEFControlParser::unescape(const std::string &str) {
	std::string result;
	
	bool scapeNext = false;
	for (char ch : str) {
		if (scapeNext) {
			result += ch;
			scapeNext = false; 
		} else if (ch == '\\') {
			scapeNext = true;
		} else {
			result += ch;
			scapeNext = false;
		} // end else
	} // end if
	
	return result;
} // end method
