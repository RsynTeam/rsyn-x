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

#ifndef ABU_REDUCTION_OPTO_H
#define ABU_REDUCTION_OPTO_H

#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "tool/timing/TimingTypes.h"

namespace Rsyn {
class Timer;
class RoutingEstimator;
}

namespace ICCAD15 {

class Infrastructure;

class AbuReduction : public Rsyn::Process {
       private:
        Rsyn::Session session;
        Infrastructure *infra;
        Rsyn::Design design;
        Rsyn::Module module;
        Rsyn::PhysicalDesign phDesign;
        Rsyn::Timer *timer;
        Rsyn::RoutingEstimator *routingEstimator;

        std::vector<std::tuple<Rsyn::Cell, std::tuple<Number, int>>>
            clsABUViolationCells;
        void computeABUVioaltionCells(const double alpha = 0);
        void moveABUViolationCells();

        void runAbuReduction();

       public:
        virtual bool run(const Rsyn::Json &params);

};  // end class
}
#endif
