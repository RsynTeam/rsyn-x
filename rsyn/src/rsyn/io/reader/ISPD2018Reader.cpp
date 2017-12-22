/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ISPD2018Reader.cpp
 * Author: mateus
 * 
 * Created on December 21, 2017, 9:13 PM
 */

#include "ISPD2018Reader.h"
#include "rsyn/util/Stepwatch.h"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/ispd18/Guide.h"
#include "rsyn/ispd18/RoutingGuide.h"
#include "rsyn/io/parser/guide-ispd18/GuideParser.h"
#include "rsyn/io/parser/lef_def/LEFControlParser.h"
#include "rsyn/io/parser/lef_def/DEFControlParser.h"
#include "rsyn/io/Graphics.h"

namespace Rsyn {
	
void ISPD2018Reader::load(const Json& params) {
	std::string path = params.value("path", "");
			
	if (path.back() != '/')
		path += "/";
	
	if (!params.count("lefFile")) {
		std::cout << "[ERROR] LEF file not specified...\n";
		return;
	} // end if
	
	lefFile = path + params.value("lefFile", "");
	
	if (!params.count("defFile")) {
		std::cout << "[ERROR] DEF file not specified...\n";
		return;
	} // end if
	
	defFile = path + params.value("defFile", "");
	
	if (!params.count("guideFile")) {
		std::cout << "[ERROR] Guide file not specified...\n";
		return;
	} // end if
	
	guideFile = path + params.value("guideFile", "");
	
	parsingFlow();
} // end method

// -----------------------------------------------------------------------------

void ISPD2018Reader::parsingFlow() {
	parseLEFFile();
	parseDEFFile();
	populateDesign();
	parseGuideFile();
	initializeAuxiliarInfrastructure();
} // end method

// -----------------------------------------------------------------------------

void ISPD2018Reader::parseLEFFile() {
	Stepwatch watch("Parsing LEF file");
	LEFControlParser lefParser;
	lefParser.parseLEF(lefFile, lefDescriptor);
} // end method

// -----------------------------------------------------------------------------

void ISPD2018Reader::parseDEFFile() {
	Stepwatch watch("Parsing DEF file");
	DEFControlParser defParser;
	defParser.parseDEF(defFile, defDescriptor);
} // end method

// -----------------------------------------------------------------------------

void ISPD2018Reader::parseGuideFile() {
	Stepwatch watch("Parsing guide file");
	GuideDscp guideDescriptor;
	GuideParser guideParser;
	guideParser.parse(guideFile, guideDescriptor);
	session.startService("rsyn.routingGuide");
	routingGuide = (RoutingGuide*) session.getService("rsyn.routingGuide");
	routingGuide->loadGuides(guideDescriptor);
} // end method

void ISPD2018Reader::populateDesign() {
	Stepwatch watch("Populating the design");

	Rsyn::Design design = session.getDesign();

	Reader::populateRsyn(lefDescriptor, defDescriptor, design);

	Json physicalDesignConfiguration;
	physicalDesignConfiguration["clsEnableMergeRectangles"] = true;
	physicalDesignConfiguration["clsEnableNetPinBoundaries"] = true;
	physicalDesignConfiguration["clsEnableRowSegments"] = true;
	session.startService("rsyn.physical", physicalDesignConfiguration);
	Rsyn::PhysicalService* phService = session.getService("rsyn.physical");
	Rsyn::PhysicalDesign physicalDesign = phService->getPhysicalDesign();
	physicalDesign.loadLibrary(lefDescriptor);
	physicalDesign.loadDesign(defDescriptor);
	physicalDesign.updateAllNetBounds(false);
} // end method

void ISPD2018Reader::initializeAuxiliarInfrastructure() {
	// Start graphics service...
	session.startService("rsyn.graphics",{});
	Graphics *graphics = session.getService("rsyn.graphics");
	graphics->coloringByCellType();
	//graphics->coloringRandomGray();

	// Start writer service...
	session.startService("rsyn.writer",{});
} // end method

} // end namespace