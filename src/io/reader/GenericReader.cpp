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
 * File:   GenericReader.cpp
 * Author: mateus
 *
 * Created on 14 de Abril de 2017, 12:11
 */

#include "GenericReader.h"
#include "core/Rsyn.h"
#include "io/parser/lef_def/LEFControlParser.h"
#include "io/parser/lef_def/DEFControlParser.h"
#include "io/parser/liberty/LibertyControlParser.h"
#include "io/parser/sdc/SDCControlParser.h"
#include "io/parser/verilog/SimplifiedVerilogReader.h"
#include "io/Graphics.h"
#include "tool/timing/DefaultTimingModel.h"
#include "tool/routing/RoutingEstimator.h"
#include "tool/routing/DefaultRoutingEstimationModel.h"
#include "tool/routing/DefaultRoutingExtractionModel.h"
#include "tool/library/LibraryCharacterizer.h"
#include "phy/PhysicalDesign.h"
#include "phy/PhysicalDesign.h"
#include "util/Stepwatch.h"
#include "tool/timing/Timer.h"
#include "tool/routing/RsttRoutingEstimatorModel.h"

namespace Rsyn {

bool GenericReader::load(const Rsyn::Json& params) {
        std::string path = params.value("path", "");

        if (path.back() != '/') path += "/";

        if (!params.count("lefFiles")) {
                std::cout
                    << "[ERROR] at least one LEF file must be specified...\n";
                return false;
        }  // end if

        if (params["lefFiles"].is_array()) {
                const Rsyn::Json fileList = params["lefFiles"];
                for (const std::string file : fileList) {
                        lefFiles.push_back(path + std::string(file));
                }  // end for
        } else {
                lefFiles.push_back(path + params.value("lefFiles", ""));
        }  // end if

        if (!params.count("defFiles")) {
                std::cout
                    << "[ERROR] at least one DEF file must be specified...\n";
                return false;
        }  // end if

        if (params["defFiles"].is_array()) {
                const Rsyn::Json fileList = params["defFiles"];
                for (const std::string file : fileList) {
                        defFiles.push_back(path + file);
                }  // end for
        } else {
                defFiles.push_back(path + params.value("defFiles", ""));
        }  // end if

        if (params.count("verilogFile")) {
                verilogFile = path + params.value("verilogFile", "");
                enableNetlistFromVerilog = true;
        }  // end if

        const bool hasLibFiles =
            params.count("sdcFile") &&
            (params.count("libFile") ||
             params.count("libFileEarly") && params.count("libFileLate"));

        if (hasLibFiles) {
                sdcFile = path + params.value("sdcFile", "");
                sameEarlyLateLibFiles = params.count("libFile") && hasLibFiles;

                if (sameEarlyLateLibFiles) {
                        libertyFileEarly = path + params.value("libFile", "");
                        libertyFileLate = path + params.value("libFile", "");
                } else {
                        libertyFileEarly =
                            path + params.value("libFileEarly", "");
                        libertyFileLate =
                            path + params.value("libFileLate", "");
                }  // end if-else

                enableTiming = true;

                localWireResistancePerMicron =
                    session.getSessionVariableAsFloat(
                        "localWireCapacitancePerMicron", 2.535f);
                localWireCapacitancePerMicron = (Number)std::atof(
                    session
                        .getSessionVariableAsString(
                            "localWireResistancePerMicron", "0.16E-15f")
                        .c_str());
                maxWireSegmentInMicron = session.getSessionVariableAsInteger(
                    "maxWireSegmentInMicron", 100);
                enableRSTT = session.getSessionVariableAsBool("enableRSTT");

                std::cout << "\n";
                std::cout << std::left << std::setw(40)
                          << "Technology parameter";
                std::cout << "Value\n";
                std::cout << std::left << std::setw(40)
                          << "localWireCapacitancePerMicron...";
                std::cout << localWireCapacitancePerMicron << "\n";
                std::cout << std::left << std::setw(40)
                          << "localWireResistancePerMicron...";
                std::cout << localWireResistancePerMicron << "\n";
                std::cout << std::left << std::setw(40)
                          << "maxWireSegmentInMicron...";
                std::cout << maxWireSegmentInMicron << "\n";
                std::cout << std::left << std::setw(40) << "enableRSTT...";
                std::cout << std::boolalpha << enableRSTT << "\n";
                std::cout << "\n";
        }  // end if

        this->session = session;

        parsingFlow();
        return true;
}  // end method

// -----------------------------------------------------------------------------

void GenericReader::parsingFlow() {
        Stepwatch watch("Running generic reader");

        parseLEFFiles();

        parseDEFFiles();

        if (enableNetlistFromVerilog) parseVerilogFile();

        if (enableTiming) parseLibertyFile();

        populateDesign();

        if (enableTiming) parseSDCFile();

        initializeAuxiliarInfrastructure();
}  // end method

// -----------------------------------------------------------------------------

void GenericReader::parseLEFFiles() {
        Stepwatch watch("Parsing LEF files");
        LEFControlParser lefParser;

        for (int i = 0; i < lefFiles.size(); i++) {
                if (!boost::filesystem::exists(lefFiles[i])) {
                        std::cout << "[WARNING] Failed to open file "
                                  << lefFiles[i] << "\n";
                        std::exit(1);
                }  // end if

                lefParser.parseLEF(lefFiles[i], lefDescriptor);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void GenericReader::parseDEFFiles() {
        Stepwatch watch("Parsing DEF files");
        DEFControlParser defParser;

        for (int i = 0; i < defFiles.size(); i++) {
                if (!boost::filesystem::exists(defFiles[i])) {
                        std::cout << "[WARNING] Failed to open file "
                                  << defFiles[i] << "\n";
                        std::exit(1);
                }  // end if

                defParser.parseDEF(defFiles[i], defDescriptor);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void GenericReader::parseVerilogFile() {
        Stepwatch watch("Parsing Verilog file");

        if (!boost::filesystem::exists(verilogFile)) {
                std::cout << "[WARNING] Failed to open file " << verilogFile
                          << "\n";
                std::exit(1);
        }  // end if

        Parsing::SimplifiedVerilogReader parser(verilogDescriptor);
        parser.parseFromFile(verilogFile);
}  // end method

// -----------------------------------------------------------------------------

void GenericReader::populateDesign() {
        Stepwatch watch("Populating the design");

        Rsyn::Design design = session.getDesign();

        if (enableTiming)
                Reader::populateRsynLibraryFromLiberty(libInfoEarly, design);

        if (enableNetlistFromVerilog)
                Reader::populateRsyn(lefDescriptor, defDescriptor,
                                     verilogDescriptor, design);
        else
                Reader::populateRsyn(lefDescriptor, defDescriptor, design);

        Rsyn::Json physicalDesignConfiguration;
        physicalDesignConfiguration["clsEnableMergeRectangles"] = false;
        physicalDesignConfiguration["clsEnableNetPinBoundaries"] = true;
        physicalDesignConfiguration["clsEnableRowSegments"] = true;
        session.startService("rsyn.physical", physicalDesignConfiguration);
        Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
        physicalDesign.loadLibrary(lefDescriptor);
        physicalDesign.loadDesign(defDescriptor);
        physicalDesign.updateAllNetBounds(false);
        watch.finish();
}  // end method

// -----------------------------------------------------------------------------

void GenericReader::parseLibertyFile() {
        Stepwatch parsingLibertyWatch("Parsing Liberty file");

        if (!boost::filesystem::exists(libertyFileEarly)) {
                std::cout << "[WARNING] Failed to open file "
                          << libertyFileEarly << "\n";
                std::exit(1);
        }  // end if

        if (!boost::filesystem::exists(libertyFileLate)) {
                std::cout << "[WARNING] Failed to open file " << libertyFileLate
                          << "\n";
                std::exit(1);
        }  // end if

        LibertyControlParser libertyParserEarly;
        libertyParserEarly.parseLiberty(libertyFileEarly, libInfoEarly);
        if (!sameEarlyLateLibFiles) {
                LibertyControlParser libertyParserLate;
                libertyParserLate.parseLiberty(libertyFileLate, libInfoLate);
        }  // end if
        parsingLibertyWatch.finish();
}  // end method

// -----------------------------------------------------------------------------

void GenericReader::parseSDCFile() {
        Stepwatch watchParsingSdc("Parsing SDC file");
        SDCControlParser sdcParser;

        if (!boost::filesystem::exists(sdcFile)) {
                std::cout << "[WARNING] Failed to open file " << sdcFile
                          << "\n";
                std::exit(1);
        }  // end if

        sdcParser.parseSDC_iccad15(sdcFile, sdcInfo);
        watchParsingSdc.finish();
}  // end method

// -----------------------------------------------------------------------------

void GenericReader::initializeAuxiliarInfrastructure() {
        Rsyn::Design design = session.getDesign();
        Rsyn::Library library = session.getLibrary();

        if (enableTiming) {
                Stepwatch watchScenario("Loading scenario");
                session.startService("rsyn.scenario", {});
                Rsyn::Scenario* scenario = session.getService("rsyn.scenario");
                if (sameEarlyLateLibFiles) {
                        scenario->init(design, library, libInfoEarly,
                                       libInfoEarly, sdcInfo);
                } else {
                        scenario->init(design, library, libInfoEarly,
                                       libInfoLate, sdcInfo);
                }  // end if-else
                watchScenario.finish();

                RoutingEstimationModel* routingEstimationModel;

                if (!enableRSTT) {
                        session.startService(
                            "rsyn.defaultRoutingEstimationModel", {});
                        DefaultRoutingEstimationModel*
                            defaultRoutingEstimationModel = session.getService(
                                "rsyn.defaultRoutingEstimationModel");
                        routingEstimationModel = defaultRoutingEstimationModel;
                } else {
                        session.startService("rsyn.RSTTroutingEstimationModel");
                        RsttRoutingEstimatorModel* rsstRoutingEstimationModel =
                            session.getService(
                                "rsyn.RSTTroutingEstimationModel");
                        routingEstimationModel = rsstRoutingEstimationModel;
                }  // end if

                session.startService("rsyn.defaultRoutingExtractionModel", {});
                DefaultRoutingExtractionModel* routingExtractionModel =
                    session.getService("rsyn.defaultRoutingExtractionModel");

                const Number resPerMicron =
                    (Number)Rsyn::Units::convertToInternalUnits(
                        Rsyn::MEASURE_RESISTANCE, localWireResistancePerMicron,
                        Rsyn::NO_UNIT_PREFIX);

                const Number capPerMicron =
                    (Number)Rsyn::Units::convertToInternalUnits(
                        Rsyn::MEASURE_CAPACITANCE,
                        localWireCapacitancePerMicron, Rsyn::NO_UNIT_PREFIX);

                DBU clsDesignDistanceUnit =
                    (DBU)lefDescriptor.clsLefUnitsDscp.clsDatabase;

                routingExtractionModel->initialize(
                    resPerMicron / clsDesignDistanceUnit,
                    capPerMicron / clsDesignDistanceUnit,
                    maxWireSegmentInMicron * clsDesignDistanceUnit);

                session.startService("rsyn.routingEstimator", {});
                RoutingEstimator* routingEstimator =
                    session.getService("rsyn.routingEstimator");
                Stepwatch updateSteiner("Updating Steiner trees");
                routingEstimator->setRoutingEstimationModel(
                    routingEstimationModel);
                routingEstimator->setRoutingExtractionModel(
                    routingExtractionModel);
                routingEstimator->updateRoutingFull();
                updateSteiner.finish();

                session.startService("rsyn.timer", {});
                Rsyn::Timer* timer = session.getService("rsyn.timer");

                Stepwatch watchInit("Initializing timer");
                if (sameEarlyLateLibFiles) {
                        timer->init(design, session, scenario, libInfoEarly,
                                    libInfoEarly);
                } else {
                        timer->init(design, session, scenario, libInfoEarly,
                                    libInfoLate);
                }  // end if-else
                watchInit.finish();

                Stepwatch watchInitModel("Initializing default timing model");
                session.startService("rsyn.defaultTimingModel", {});
                DefaultTimingModel* timingModel =
                    session.getService("rsyn.defaultTimingModel");
                timer->setTimingModel(timingModel);
                watchInitModel.finish();

                Stepwatch watchInitLogicalEffort("Library characterization");
                session.startService("rsyn.libraryCharacterizer", {});
                LibraryCharacterizer* libc =
                    session.getService("rsyn.libraryCharacterizer");
                libc->runLibraryCharacterization(timingModel);
                watchInitLogicalEffort.finish();

                Stepwatch updateTiming("Updating timing");
                timer->updateTimingFull();
                updateTiming.finish();

                session.startService("rsyn.report", {});
        }  // end if

        // Start graphics service...
        session.startService("rsyn.graphics", {});
        Graphics* graphics = session.getService("rsyn.graphics");
        graphics->coloringByCellType();
        // graphics->coloringRandomGray();

        // Start writer service...
        session.startService("rsyn.writer", {});
}  // end method

// -----------------------------------------------------------------------------

// void GenericReader::parseInputFiles() {
//	LEFControlParser lefParser;
//	DEFControlParser defParser;
//
//	Legacy::Design verilogDesignDescriptor;
//	LefDscp lefDscp;
//	DefDscp defDscp;
//
//	// Start parsing...
//	Stepwatch watchParsing("Parsing Design");
//
//	lefParser.parseLEF(lefFiles[0], lefDscp);
//	defParser.parseDEF(defFiles, defDscp);
//
//	Parsing::SimplifiedVerilogReader parser(verilogDesignDescriptor);
//	parser.parseFromFile(verilogFile);
//
//	watchParsing.finish();
//	// Parsing complete...
//
//	Stepwatch watchRsyn("Populating Rsyn");
//	Rsyn::Design design = session.getDesign();
//
//	ISPD13::LIBInfo libInfo;
//	if (enableTiming) {
//		LibertyControlParser libParser;
//
//		Stepwatch watchParsingLibertyEarly("Parsing Liberty");
//		libParser.parseLiberty(libFile, libInfo);
//		watchParsingLibertyEarly.finish();
//
//		Stepwatch watchLibrary("Loading library into Rsyn");
//		Reader::populateRsynLibraryFromLiberty(libInfo, design);
//		watchLibrary.finish();
//	} // end if
//
//	Reader::populateRsyn(
//		lefDscp,
//		defDscp,
//		verilogDesignDescriptor,
//		design);
//	watchRsyn.finish();
//
//	// Start physical design...
//	Stepwatch watchPopulateLayers("Initializing Physical Layer");
//	Rsyn::Json physicalDesignConfiguratioon;
//	physicalDesignConfiguratioon["clsEnableMergeRectangles"] = true;
//	physicalDesignConfiguratioon["clsEnableNetPinBoundaries"] = true;
//	physicalDesignConfiguratioon["clsEnableRowSegments"] = true;
//	session.startService("rsyn.physical", physicalDesignConfiguratioon);
//	Rsyn::PhysicalService* phService = session.getService("rsyn.physical");
//	Rsyn::PhysicalDesign physicalDesign = phService->getPhysicalDesign();
//	physicalDesign.loadLibrary(lefDscp);
//	physicalDesign.loadDesign(defDscp);
//	physicalDesign.updateAllNetBounds(false);
//	watchPopulateLayers.finish();
//	// Physical design complete...
//
//	if (enableTiming) {
//		SDCControlParser sdcParser;
//		ISPD13::SDCInfo sdcInfo;
//
//		Stepwatch watchParsingSdc("Parsing SDC");
//		sdcParser.parseSDC_iccad15(sdcFile, sdcInfo);
//		watchParsingSdc.finish();
//
//		Stepwatch watchScenario("Loading scenario");
//		session.startService("rsyn.scenario", {});
//		Rsyn::Scenario* scenario = session.getService("rsyn.scenario");
//		scenario->init(design, libInfo, libInfo, sdcInfo);
//		watchScenario.finish();
//
//		session.startService("rsyn.defaultRoutingEstimationModel", {});
//		DefaultRoutingEstimationModel* routingEstimationModel =
//			session.getService("rsyn.defaultRoutingEstimationModel");
//
//		session.startService("rsyn.defaultRoutingExtractionModel", {});
//		DefaultRoutingExtractionModel* routingExtractionModel =
//			session.getService("rsyn.defaultRoutingExtractionModel");
//
//		const Number resPerMicron = (Number)
// Rsyn::Units::convertToInternalUnits(
//				Rsyn::MEASURE_RESISTANCE,
// localWireResistancePerMicron, Rsyn::NO_UNIT_PREFIX);
//
//		const Number capPerMicron = (Number)
// Rsyn::Units::convertToInternalUnits(
//				Rsyn::MEASURE_CAPACITANCE,
// localWireCapacitancePerMicron, Rsyn::NO_UNIT_PREFIX);
//
//		DBU clsDesignDistanceUnit =
//(DBU)lefDscp.clsLefUnitsDscp.clsDatabase;
//
//		routingExtractionModel->initialize(
//				resPerMicron / clsDesignDistanceUnit,
//				capPerMicron / clsDesignDistanceUnit,
//				maxWireSegmentInMicron * clsDesignDistanceUnit);
//
//		session.startService("rsyn.routingEstimator", {});
//		RoutingEstimator *routingEstimator =
// session.getService("rsyn.routingEstimator");
//		Stepwatch updateSteiner("Updating Steiner trees");
//		routingEstimator->setRoutingEstimationModel(routingEstimationModel);
//		routingEstimator->setRoutingExtractionModel(routingExtractionModel);
//		routingEstimator->updateRoutingFull();
//		updateSteiner.finish();
//
//		session.startService("rsyn.timer", {});
//		Rsyn::Timer* timer = session.getService("rsyn.timer");
//
//		Stepwatch watchInit("Initializing timer");
//		timer->init(
//				design,
//				session,
//				scenario,
//				libInfo,
//				libInfo);
//		watchInit.finish();
//
//		Stepwatch watchInitModel("Initializing default timing model");
//		session.startService("rsyn.defaultTimingModel", {});
//		DefaultTimingModel* timingModel =
// session.getService("rsyn.defaultTimingModel");
//		timer->setTimingModel(timingModel);
//		watchInitModel.finish();
//
//		Stepwatch watchInitLogicalEffort("Library characterization");
//		session.startService("rsyn.libraryCharacterizer", {});
//		LibraryCharacterizer *libc =
// session.getService("rsyn.libraryCharacterizer");
//		libc->runLibraryAnalysis(design, timingModel);
//		watchInitLogicalEffort.finish();
//
//		Stepwatch updateTiming("Updating timing");
//		timer->updateTimingFull();
//		updateTiming.finish();
//
//		session.startService("rsyn.report", {});
//	} // end if
//
//	// Start graphics service...
//	session.startService("rsyn.graphics", {});
//	Graphics *graphics = session.getService("rsyn.graphics");
//	graphics->coloringByCellType();
//	// Graphics service complete...
//
//	// Start writer service...
//	session.startService("rsyn.writer", {});
//	// Writer service complete...
//
//	// Start jezz service..
//	session.startService("rsyn.jezz", {});
//	// Jezz complete
//} // end method

}  // end namespace
