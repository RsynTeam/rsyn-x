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
 * File:   newClass.cpp
 * Author: jucemar
 *
 * Created on 20 de Fevereiro de 2017, 19:09
 */

#include "SizingISPDReader.h"

#include "io/parser/liberty/LibertyControlParser.h"
#include "io/parser/sdc/SDCControlParser.h"
#include "io/parser/verilog/SimplifiedVerilogReader.h"
#include "io/parser/spef/SPEFControlParser.h"
#include "tool/timing/Timer.h"
#include "tool/timing/DefaultTimingModel.h"
#include "tool/routing/RoutingEstimator.h"

#include "util/Stepwatch.h"

namespace Rsyn {

bool SizingISPDReader::load(const Rsyn::Json &config) {
        std::string path = config.value("path", "");
        std::string optionBenchmark = config.value("name", "");
        std::string optionLibrary = config.value("library", "");
        std::string optionNetModel = config.value("netModel", "");

        RCTreeExtractor::SPEFNetModel netModel;

        if (optionNetModel == "LumpedCapacitanceNetModel")
                netModel = RCTreeExtractor::LUMPED_CAPACITANCE_NET_MODEL;
        else if (optionNetModel == "RCTreeNetModel")
                netModel = RCTreeExtractor::RC_TREE_NET_MODEL;
        else {
                std::cout << "\nWarning: Invalid net model specified. Assuming "
                             "lumped capacitance net model.\n";
                netModel = RCTreeExtractor::LUMPED_CAPACITANCE_NET_MODEL;
        }

        const char separator = boost::filesystem::path::preferred_separator;

        std::cout << "Path: " << path << "\n";
        std::cout << "Benchmark: " << optionBenchmark << "\n";
        std::cout << "Library: " << optionLibrary << "\n";

        Legacy::Design verilogDesignDescriptor;
        ISPD13::LIBInfo libInfo;
        ISPD13::SPEFInfo spefInfo;
        ISPD13::SDCInfo sdcInfo;

        const std::string fileNameVerilog =
            path + separator + optionBenchmark + ".v";
        const std::string fileNameSdc =
            path + separator + optionBenchmark + ".sdc";
        const std::string fileNameSpef =
            path + separator + optionBenchmark + ".spef";
        const std::string fileNameLiberty = path + separator + optionLibrary;

        clsDesign = session.getDesign();
        clsLibrary = session.getLibrary();
        {  // Liberty
                Stepwatch watchParsingLiberty("Parsing Liberty");
                LibertyControlParser libParser;
                libParser.parseLiberty(fileNameLiberty, libInfo);
                watchParsingLiberty.finish();
                Reader::populateRsynLibraryFromLiberty(libInfo, clsDesign);
        }  // end block

        {  // Verilog
                Stepwatch watchParsingVerilog("Parsing Verilog");
                Parsing::SimplifiedVerilogReader parser(
                    verilogDesignDescriptor);
                parser.parseFromFile(fileNameVerilog);
                watchParsingVerilog.finish();
                Reader::populateRsynNetlistFromVerilog(verilogDesignDescriptor,
                                                       clsDesign);
        }  // end block

        {  // SDC
                Stepwatch watchParsingSdc("Parsing SDC");
                SDCControlParser sdcParser;
                sdcParser.parseSDC_iccad15(fileNameSdc, sdcInfo);
                watchParsingSdc.finish();
        }  // end block

        {  // SPEF
                Stepwatch watchParsingSptef("Parsing SPEF");
                SPEFControlParser spefParser;
                spefParser.parseSPEF(fileNameSpef, spefInfo);
                watchParsingSptef.finish();
        }  // end block

        // Services
        session.startService("rsyn.scenario", {});
        session.startService("rsyn.routingEstimator", {});
        session.startService("rsyn.timer", {});
        session.startService("rsyn.defaultTimingModel", {});

        Timer *timer = session.getService("rsyn.timer");
        RoutingEstimator *routingEstimator =
            session.getService("rsyn.routingEstimator");
        DefaultTimingModel *timingModel =
            session.getService("rsyn.defaultTimingModel");
        Scenario *scenario = session.getService("rsyn.scenario");

        {  // Scenario
                Stepwatch watchScenario("Initializing scenario manager");
                scenario->init(clsDesign, clsLibrary, libInfo, libInfo,
                               sdcInfo);
                watchScenario.finish();
        }  // end block

        {  // Routing Estimator
                Stepwatch updateSteiner("Initializing routing estimator");
                routingEstimator->setRoutingEstimationModel(
                    nullptr);  // not necessary, just for clarity
                routingEstimator->setRoutingExtractionModel(
                    nullptr);  // not necessary, just for clarity
                routingEstimator->updateRoutingFull();
                updateSteiner.finish();
        }  // end block

        {  // Timer Initialization
                Stepwatch watchInit("Initializing timer");
                timer->init(clsDesign, session, scenario, libInfo, libInfo);
                timer->setTimingModel(timingModel);
                timer->setInputDriverDelayMode(
                    Timer::INPUT_DRIVER_DELAY_MODE_PRIME_TIME);
                watchInit.finish();
        }  // end block

        {  // Setup user-specified interconnection

                Stepwatch watchRCTreeInit("Building RC Tree");

                RCTreeExtractor rcTreeExtractor(&spefInfo);

                Rsyn::Design design = session.getDesign();
                const int numSpefNets = spefInfo.getSize();
                for (int i = 0; i < numSpefNets; i++) {
                        const ISPD13::SpefNet &info = spefInfo.getNet(i);
                        Rsyn::Net net = design.findNetByName(info.netName);
                        RCTree &tree = routingEstimator->getRCTree(net);
                        rcTreeExtractor.extractRCTreeFromSPEF(netModel, net,
                                                              tree);
                }  // end for

                watchRCTreeInit.finish();

        }  // end block

        session.startService("rsyn.graphics", {});

        timer->updateTimingFull();
        timer->dumpTiming("dump-rsyn.txt");
        std::cout << "WNS: " << timer->getWns(LATE) << "\n";
        std::cout << "TNS: " << timer->getTns(LATE) << "\n";

        return true;
}  // end method

}  // end namespace
