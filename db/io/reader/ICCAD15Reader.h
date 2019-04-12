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
 * File:   ICCAD15Reader.h
 * Author: jucemar
 *
 * Created on 20 de Fevereiro de 2017, 18:59
 */

#ifndef RSYN_ICCAD15READER_H
#define RSYN_ICCAD15READER_H

#include "session/Session.h"

#include "core/Rsyn.h"

#include "tool/timing/Timer.h"
#include "tool/scenario/Scenario.h"
#include "tool/timing/DefaultTimingModel.h"
#include "tool/routing/RoutingEstimator.h"

namespace Rsyn {

////////////////////////////////////////////////////////////////////////////////
// Reader to ICCAD 2015 benchmarks
////////////////////////////////////////////////////////////////////////////////
class ICCAD15Reader : public Reader {
       protected:
        Rsyn::Session session;
        Timer *clsTimer;
        Scenario *clsScenario;
        Rsyn::Design clsDesign;
        Rsyn::Library clsLibrary;
        Rsyn::Module clsModule;
        TimingModel *clsTimingModel = nullptr;
        RoutingEstimator *clsRoutingEstimator = nullptr;

        // files name
        std::string clsFilenameV;
        std::string clsFilenameSDC;
        std::string clsFilenameLEF;
        std::string clsFilenameTechLEF;
        std::string clsFilenameDEF;
        std::string clsFilenameSPEF;
        std::string clsFilenameLibertyEarly;
        std::string clsFilenameLibertyLate;

        std::string optionSetting;
        std::string optionBenchmark;
        double optionTargetUtilization;
        double optionMaxDisplacement;

        ////////////////////////////////////////////////////////////////////////////
        // Routing
        ////////////////////////////////////////////////////////////////////////////
        Number LOCAL_WIRE_CAP_PER_MICRON;
        Number LOCAL_WIRE_RES_PER_MICRON;
        Number MAX_WIRE_SEGMENT_IN_MICRON;
        int MAX_LCB_FANOUTS;

       public:
        ICCAD15Reader() = default;
        ICCAD15Reader(const ICCAD15Reader &orig) = delete;
        virtual ~ICCAD15Reader() = default;
        virtual bool load(const Rsyn::Json &params) override;

       protected:
        void parseConfigFileICCAD15(boost::filesystem::path &path);
        void parseParams(const std::string &filename);

        void openBenchmarkFromICCAD15();
        void openBenchmarkFromICCAD15ForGlobalPlacementOnly();
};  // end class

}  // end namespace
#endif /* RSYN_ICCAD15READER_H */
