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

#ifndef RSYN_DEFAULT_ROUTING_ESTIMATION_MODEL_H
#define RSYN_DEFAULT_ROUTING_ESTIMATION_MODEL_H

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Service.h"
#include "session/Session.h"
#include "tool/scenario/Scenario.h"
#include "RoutingEstimationModel.h"

namespace Rsyn {
class PhysicalService;
template <class NameType>
class RoutingTopologyDescriptor;
}

namespace Rsyn {

class DefaultRoutingEstimationModel : public RoutingEstimationModel,
                                      public Service {
       public:
        virtual void start(const Rsyn::Json &params) override;
        virtual void stop() override;

       private:
        // Design.
        Rsyn::Design design;
        Rsyn::Module module;  // top module
        Rsyn::PhysicalDesign clsPhysicalDesign;

        // Services
        Scenario *clsScenario = nullptr;

        // Config
        static const bool ENABLE_DO_NOT_USE_FLUTE_FOR_2_PIN_NETS;

        // Call FLUTE to generate a routing topology.
        DBU generateSteinerTree(Rsyn::Net net,
                                Rsyn::RoutingTopologyDescriptor<int> &topology);

       public:
        DefaultRoutingEstimationModel() {}

        virtual void updateRoutingEstimation(
            Rsyn::Net net, Rsyn::RoutingTopologyDescriptor<int> &topology,
            DBU &wirelength);

        ////////////////////////////////////////////////////////////////////////////
        // Analysis: Flute
        ////////////////////////////////////////////////////////////////////////////

        // Reports the average runtime to generate Steiner trees using Flute for
        // net
        // degrees from 2 up to N. After "minNumTreesPerDegree" trees are
        // generated
        // for a degree, stops when the elapsed time exceeds
        // maxSecondsPerDegree.
        // After "degree > largeStepStart", the current degree is incremented by
        // "largeStep".

        void generateFluteAvgRuntimeTable(const int N,
                                          const double maxSecondsPerDegree = 5,
                                          const int minNumTreesPerDegree = 1000,
                                          const int largeStepStart = 100,
                                          const int largeStep = 10);

};  // end class

}  // end namespace

#endif
