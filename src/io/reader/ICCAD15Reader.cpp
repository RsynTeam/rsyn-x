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
 * File:   ICCAD15Reader.cpp
 * Author: jucemar
 *
 * Created on 20 de Fevereiro de 2017, 18:59
 */

#include "ICCAD15Reader.h"

#include "io/parser/lef_def/LEFControlParser.h"
#include "io/parser/lef_def/DEFControlParser.h"
#include "io/parser/sdc/SDCControlParser.h"
#include "io/parser/liberty/LibertyControlParser.h"
#include "io/parser/spef/SPEFControlParser.h"
#include "io/parser/verilog/SimplifiedVerilogReader.h"

#include "phy/PhysicalDesign.h"
#include "phy/PhysicalDesign.h"
#include "io/Graphics.h"
#include "tool/routing/DefaultRoutingEstimationModel.h"
#include "tool/routing/DefaultRoutingExtractionModel.h"
#include "tool/routing/RCTree.h"
#include "tool/library/LibraryCharacterizer.h"

#include "util/StreamStateSaver.h"
#include "util/Environment.h"

namespace Rsyn {

bool ICCAD15Reader::load(const Rsyn::Json& options) {
        this->session = session;
        const bool globalPlacementOnly =
            options.value("globalPlacementOnly", false);

        std::string path = options.value("path", "");
        std::string benchmark = options.value("config", "");
        std::string params = options.value("parms", "ICCAD15.parm");
        optionMaxDisplacement = options.value("maxDisplacement", 400);
        optionTargetUtilization = options.value("targetUtilization", 0.85);
        optionBenchmark = session.findFile(benchmark, path);
        optionSetting = session.findFile(params, path);

        if (globalPlacementOnly) {
                openBenchmarkFromICCAD15ForGlobalPlacementOnly();
        } else {
                if (optionBenchmark.empty()) {
                        std::cout << "Error: File <design>.iccad2015 file not "
                                     "found: '"
                                  << benchmark << "'\n";
                        return false;
                }  // end if

                if (optionSetting.empty()) {
                        std::cout << "Error: File ICCAD15.parm not found: '"
                                  << params << "'\n";
                        return false;
                }  // end if

                openBenchmarkFromICCAD15();
        }  // end else

        return true;
}  // end method

// -----------------------------------------------------------------------------

void ICCAD15Reader::openBenchmarkFromICCAD15() {
        /* Temporary workaround */
        const bool ENABLE_RECTANGLE_MERGE =
            Environment::getBoolean("ENABLE_RECTANGLE_MERGE", false);

        // parsers
        LEFControlParser lefParser;
        DEFControlParser defParser;
        SDCControlParser sdcParser;
        LibertyControlParser libParser;

        LefDscp lefDscp;
        DefDscp defDscp;
        Legacy::Design verilogDesignDescriptor;

        ISPD13::SDCInfo sdcInfos;
        ISPD13::LIBInfo libInfosEarly;
        ISPD13::LIBInfo libInfosLate;

        std::cout << "[INFO] ICCAD 2015 Contest Flow.\n";

        boost::filesystem::path path(optionBenchmark);
        std::string filename = path.filename().generic_string();
        std::cout << "Options\n";
        std::cout << "\tSettings..........: " << optionSetting << "\n";
        std::cout << "\tInput.............: " << filename << "\n";
        std::cout << "\tTarget Utilization: " << optionTargetUtilization
                  << "\n";
        std::cout << "\tMax Displacement..: " << optionMaxDisplacement << "\n";

        Stepwatch watchParsing("Parsing Design");

        parseConfigFileICCAD15(path);
        parseParams(optionSetting);

        Stepwatch watchParsingLibertyEarly("Parsing Liberty (Early)");
        libParser.parseLiberty(clsFilenameLibertyEarly, libInfosEarly);
        watchParsingLibertyEarly.finish();

        Stepwatch watchParsingLibertyLate("Parsing Liberty (Late)");
        libParser.parseLiberty(clsFilenameLibertyLate, libInfosLate);
        watchParsingLibertyLate.finish();

        Stepwatch watchParsingLef("Parsing LEF");
        lefParser.parseLEF(clsFilenameLEF, lefDscp);
        watchParsingLef.finish();

        Stepwatch watchParsingDef("Parsing DEF");
        defParser.parseDEF(clsFilenameDEF, defDscp);
        watchParsingDef.finish();

        Stepwatch watchParsingSdc("Parsing SDC");
        sdcParser.parseSDC_iccad15(clsFilenameSDC, sdcInfos);
        watchParsingSdc.finish();

        Stepwatch watchParsingVerilog("Parsing Verilog");
        Parsing::SimplifiedVerilogReader parser(verilogDesignDescriptor);
        parser.parseFromFile(clsFilenameV);
        watchParsingVerilog.finish();

        watchParsing.finish();

        // Create the design.
        clsDesign = session.getDesign();
        clsLibrary = session.getLibrary();
        clsModule = clsDesign.getTopModule();

        DBU clsDesignDistanceUnit =
            (DBU)lefDscp.clsLefUnitsDscp
                .clsDatabase;  // FIXME: should come from LEF

        Stepwatch watchLibrary("Loading library into Rsyn");
        Reader::populateRsynLibraryFromLiberty(libInfosEarly, clsDesign);
        watchLibrary.finish();

        Stepwatch watchRsyn("Loading design into Rsyn");
        Reader::populateRsyn(lefDscp, defDscp, verilogDesignDescriptor,
                             clsDesign);
        watchRsyn.finish();

        // session.startService("rsyn.webLogger", {});

        Stepwatch watchScenario("Loading scenario");
        session.startService("rsyn.scenario", {});
        clsScenario = session.getService("rsyn.scenario");
        clsScenario->init(clsDesign, clsLibrary, libInfosEarly, libInfosLate,
                          sdcInfos);
        watchScenario.finish();

        Stepwatch watchPopulateLayers("Initializing Physical Layer");
        Rsyn::Json phDesignJason;
        if (ENABLE_RECTANGLE_MERGE) {
                phDesignJason["clsEnableMergeRectangles"] = true;
        }  // end if
        phDesignJason["clsContestMode"] = "ICCAD15";
        session.startService("rsyn.physical", phDesignJason);
        Rsyn::PhysicalDesign clsPhysicalDesign = session.getPhysicalDesign();
        clsPhysicalDesign.loadLibrary(lefDscp);
        clsPhysicalDesign.loadDesign(defDscp);
        clsPhysicalDesign.updateAllNetBounds(false);
        clsPhysicalDesign.setClockNet(clsScenario->getClockNet());
        watchPopulateLayers.finish();

        session.startService("rsyn.defaultRoutingEstimationModel", {});
        DefaultRoutingEstimationModel* routingEstimationModel =
            session.getService("rsyn.defaultRoutingEstimationModel");

        session.startService("rsyn.defaultRoutingExtractionModel", {});
        DefaultRoutingExtractionModel* routingExtractionModel =
            session.getService("rsyn.defaultRoutingExtractionModel");

        const Number resPerMicron = (Number)Rsyn::Units::convertToInternalUnits(
            Rsyn::MEASURE_RESISTANCE, LOCAL_WIRE_RES_PER_MICRON,
            Rsyn::NO_UNIT_PREFIX);

        const Number capPerMicron = (Number)Rsyn::Units::convertToInternalUnits(
            Rsyn::MEASURE_CAPACITANCE, LOCAL_WIRE_CAP_PER_MICRON,
            Rsyn::NO_UNIT_PREFIX);

        routingExtractionModel->initialize(
            resPerMicron / clsDesignDistanceUnit,
            capPerMicron / clsDesignDistanceUnit,
            (DBU)(MAX_WIRE_SEGMENT_IN_MICRON * clsDesignDistanceUnit));

        session.startService("rsyn.routingEstimator", {});
        clsRoutingEstimator = session.getService("rsyn.routingEstimator");
        Stepwatch updateSteiner("Updating Steiner trees");
        clsRoutingEstimator->setRoutingEstimationModel(routingEstimationModel);
        clsRoutingEstimator->setRoutingExtractionModel(routingExtractionModel);
        clsRoutingEstimator->updateRoutingFull();
        updateSteiner.finish();

        Stepwatch watchBlockageControl("Starting blockage control");
        session.startService("ufrgs.blockageControl", {});
        watchBlockageControl.finish();

        session.startService("rsyn.timer", {});
        clsTimer = session.getService("rsyn.timer");

        Stepwatch watchInit("Initializing timer");
        clsTimer->init(clsDesign, session, clsScenario, libInfosEarly,
                       libInfosLate);
        watchInit.finish();

        Stepwatch watchInitModel("Initializing default timing model");
        session.startService("rsyn.defaultTimingModel", {});
        DefaultTimingModel* timingModel =
            session.getService("rsyn.defaultTimingModel");
        clsTimingModel = timingModel;
        clsTimer->setTimingModel(clsTimingModel);
        watchInitModel.finish();

        Stepwatch watchInitLogicalEffort("Library characterization");
        session.startService("rsyn.libraryCharacterizer", {});
        LibraryCharacterizer* libc =
            session.getService("rsyn.libraryCharacterizer");
        libc->runLibraryCharacterization(clsTimingModel);
        watchInitLogicalEffort.finish();

        Stepwatch updateTiming("Updating timing");
        clsTimer->updateTimingFull();
        updateTiming.finish();

        session.startService("rsyn.report", {});
        session.startService("rsyn.writer", {});

        session.startService("rsyn.graphics", {});
        Rsyn::Graphics* graphics = session.getService("rsyn.graphics");
        graphics->coloringByCellType();
}  // end method

// -----------------------------------------------------------------------------

void ICCAD15Reader::openBenchmarkFromICCAD15ForGlobalPlacementOnly() {
        // parsers
        LEFControlParser lefParser;
        DEFControlParser defParser;

        Legacy::Design verilogDesignDescriptor;
        LefDscp lefDscp;
        DefDscp defDscp;

        Stepwatch watchParsing("Parsing Design");

        boost::filesystem::path path(optionBenchmark);
        parseConfigFileICCAD15(path);
        lefParser.parseLEF(clsFilenameLEF, lefDscp);
        defParser.parseDEF(clsFilenameDEF, defDscp);
        Parsing::SimplifiedVerilogReader parser(verilogDesignDescriptor);
        parser.parseFromFile(clsFilenameV);
        watchParsing.finish();

        Stepwatch watchRsyn("Populating Rsyn");
        clsDesign = session.getDesign();
        Reader::populateRsyn(lefDscp, defDscp, verilogDesignDescriptor,
                             clsDesign);
        watchRsyn.finish();

        clsModule = clsDesign.getTopModule();
        Stepwatch watchPopulateLayers("Initializing Physical Layer");

        Rsyn::Json phDesignJason;
        phDesignJason["clsEnableMergeRectangles"] = true;
        phDesignJason["clsEnableNetPinBoundaries"] = true;
        phDesignJason["clsEnableRowSegments"] = true;
        session.startService("rsyn.physical", phDesignJason);
        Rsyn::PhysicalDesign clsPhysicalDesign = session.getPhysicalDesign();
        clsPhysicalDesign.loadLibrary(lefDscp);
        clsPhysicalDesign.loadDesign(defDscp);
        clsPhysicalDesign.updateAllNetBounds(false);
        watchPopulateLayers.finish();

        session.startService("rsyn.graphics", {});
        Graphics* graphics = session.getService("rsyn.graphics");
        graphics->coloringByCellType();

        session.startService("rsyn.writer", {});
}  // end method

////////////////////////////////////////////////////////////////////////////////
// ICCAD 2015 Config file
////////////////////////////////////////////////////////////////////////////////

void ICCAD15Reader::parseConfigFileICCAD15(boost::filesystem::path& path) {
        std::ifstream file(path.string());
        std::vector<std::string> files(std::istream_iterator<std::string>(file),
                                       {});

        std::string directory = path.parent_path().generic_string() +
                                boost::filesystem::path::preferred_separator;

        for (std::string circuit : files) {
                std::string extension = boost::filesystem::extension(circuit);

                if (extension.compare(".v") == 0)
                        clsFilenameV = directory + circuit;
                else if (extension.compare(".lef") == 0)
                        clsFilenameLEF = directory + circuit;
                else if (extension == ".def")
                        clsFilenameDEF = directory + circuit;
                else if (extension == ".sdc")
                        clsFilenameSDC = directory + circuit;
                else if (extension == ".lib" &&
                         (circuit.find("Early") != string::npos))
                        clsFilenameLibertyEarly = directory + circuit;
                else if (extension == ".lib" &&
                         (circuit.find("Late") != string::npos))
                        clsFilenameLibertyLate = directory + circuit;
                else
                        cerr << "[WARNING] File " << circuit
                             << " in auxiliary file was ignored.\n";
        }  // end for
        file.close();

}  // end method

// -----------------------------------------------------------------------------

void ICCAD15Reader::parseParams(const std::string& filename) {
        ifstream dot_parm(filename.c_str());
        if (!dot_parm.good()) {
                cerr << "read_parameters:: cannot open `" << filename
                     << "' for reading." << endl;
                cerr << "the script will use the default parameters for "
                        "evaluation."
                     << endl;
                exit(1);
        } else {
                bool paramWireCap = false;
                bool paramWireRes = false;
                bool paramMaxWireSegment = false;
                bool paramMaxLcbs = false;

                string tmpStr;
                dot_parm >> tmpStr;
                while (!dot_parm.eof()) {
                        if (tmpStr == "LOCAL_WIRE_CAPACITANCE_PER_MICRON") {
                                dot_parm >> LOCAL_WIRE_CAP_PER_MICRON;
                                paramWireCap = true;
                        } else if (tmpStr ==
                                   "LOCAL_WIRE_RESISTANCE_PER_MICRON") {
                                dot_parm >> LOCAL_WIRE_RES_PER_MICRON;
                                paramWireRes = true;
                        } else if (tmpStr ==
                                   "MAX_WIRE_SEGMENT_LENGTH_IN_MICRON") {
                                dot_parm >> MAX_WIRE_SEGMENT_IN_MICRON;
                                paramMaxWireSegment = true;
                        } else if (tmpStr == "MAX_LCB_FANOUTS") {
                                dot_parm >> MAX_LCB_FANOUTS;
                                paramMaxLcbs = true;
                        } else {
                                std::cout << "Error: Unrecognized keyword '"
                                          << tmpStr << "'.\n"
                                          << std::endl;
                                std::exit(1);
                                dot_parm >> tmpStr;
                        }  // end else

                        if (dot_parm.fail()) {
                                std::cout << "Error: Unable to parse .pram "
                                             "file correctly."
                                          << std::endl;
                                std::exit(1);
                        }  // end if

                        dot_parm >> tmpStr;
                }  // end while

                if (!paramWireCap) {
                        std::cout
                            << "Error: Missing "
                               "LOCAL_WIRE_CAPACITANCE_PER_MICRON parameter."
                            << std::endl;
                        std::exit(1);
                }  // end if

                if (!paramWireRes) {
                        std::cout << "Error: Missing LOCAL_WIRE_RES_PER_MICRON "
                                     "parameter."
                                  << std::endl;
                        std::exit(1);
                }  // end if

                if (!paramMaxWireSegment) {
                        std::cout << "Error: Missing "
                                     "MAX_WIRE_SEGMENT_IN_MICRON parameter."
                                  << std::endl;
                        std::exit(1);
                }  // end if

                if (!paramMaxLcbs) {
                        std::cout << "Error: Missing MAX_LCB_FANOUTS parameter."
                                  << std::endl;
                        std::exit(1);
                }  // end if
        }          // end else
        dot_parm.close();

        std::cout << "Timer Parameters:\n";
        std::cout << "\tLOCAL_WIRE_RESISTANCE   : " << LOCAL_WIRE_RES_PER_MICRON
                  << " Ohm/um\n";
        std::cout << "\tLOCAL_WIRE_CAPACITANCE  : " << LOCAL_WIRE_CAP_PER_MICRON
                  << " Farad/um\n";
        std::cout << "\tMAX_WIRE_SEGMENT_LENGTH : "
                  << MAX_WIRE_SEGMENT_IN_MICRON << " um\n";
        std::cout << "\tMAX_LCB_FANOUTS         : " << MAX_LCB_FANOUTS << "\n";
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
