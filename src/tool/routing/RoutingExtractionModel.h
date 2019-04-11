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

#ifndef ROUTING_EXTRACTOR_H
#define ROUTING_EXTRACTOR_H

#include <iostream>

#include "core/Rsyn.h"
#include "session/Service.h"
#include "phy/PhysicalDesign.h"
#include "RCTree.h"
#include "RoutingTopology.h"
#include "RoutingEstimationModel.h"
#include "tool/scenario/Scenario.h"
#include "util/Stopwatch.h"

namespace Rsyn {

class RoutingExtractionModel {
       private:
        Rsyn::Design design;
        Rsyn::Module module;  // top module

        Scenario *clsScenario = nullptr;

       public:
        virtual void extract(
            const Rsyn::RoutingTopologyDescriptor<int> &topology,
            Rsyn::RCTree &tree) = 0;
        virtual void updateDownstreamCap(Rsyn::RCTree &tree) = 0;

        // TODO: Remove these.
        virtual Number getLocalWireResPerUnitLength() const = 0;
        virtual Number getLocalWireCapPerUnitLength() const = 0;

};  // end class

}  // end namespace

#endif
