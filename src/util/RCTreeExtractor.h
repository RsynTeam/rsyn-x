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
 * File:   SPEFRoutingEstimationModel.h
 * Author: Henrique
 *
 * Created on 22 de Setembro de 2017
 */

#ifndef RCTREEEXTRACTOR_H
#define RCTREEEXTRACTOR_H

#include "core/Rsyn.h"
#include "session/Session.h"
#include "tool/scenario/Scenario.h"
#include "tool/routing/RCTree.h"
#include "tool/routing/RoutingTopology.h"
#include "io/parser/spef/SPEFControlParser.h"
#include "io/parser/parser_helper.h"

namespace Rsyn {

class RCTreeExtractor {
       private:
        Rsyn::Scenario *clsScenario;
        ISPD13::SPEFInfo *spefInfo;

        std::unordered_map<std::string, int> clsNodeMap;
        std::unordered_map<std::string, Rsyn::Pin> clsPinMap;

        Rsyn::UnitPrefix unitPrefixForResistance;
        Rsyn::UnitPrefix unitPrefixForCapacitance;

        std::string getPinNameInSPEFFormat(Rsyn::Pin pin);
        void mapSPEFPinNameToRsynPin(Rsyn::Net net);
        int getNodeIndex(const std::string &nodeName);
        void addDefaultCapacitance(int node, RCTreeDescriptor &dscp,
                                   bool useDefaultCapacitance);
        void setLoadCapacitances(Rsyn::RCTree &tree);

       public:
        enum SPEFNetModel {
                LUMPED_CAPACITANCE_NET_MODEL = 0,
                RC_TREE_NET_MODEL = 1
        };

        RCTreeExtractor() = default;
        RCTreeExtractor(ISPD13::SPEFInfo *info);
        ~RCTreeExtractor() = default;
        void extractRCTreeFromSPEF(SPEFNetModel netModel, Rsyn::Net net,
                                   Rsyn::RCTree &tree);
};

}  // end namespace

#endif
