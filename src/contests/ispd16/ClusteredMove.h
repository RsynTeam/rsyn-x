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

#ifndef CLUSTERED_MOVE_OPTO_H
#define CLUSTERED_MOVE_OPTO_H

#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "tool/timing/TimingTypes.h"

namespace Rsyn {
class Timer;
}

namespace ICCAD15 {

class Infrastructure;

class ClusteredMove : public Rsyn::Process {
       private:
        Rsyn::Session session;
        Infrastructure *infra;
        Rsyn::Design design;
        Rsyn::Module module;
        Rsyn::PhysicalDesign phDesign;
        Rsyn::Timer *timer;

        Rsyn::Attribute<Rsyn::Instance, bool> moved;

        void runClusteredMovement(const int N);
        void clusterNeighborCriticalNets(Rsyn::Pin criticalPin,
                                         const bool dontMoveRegisters = false);

        Number getWorstSlack(const Rsyn::Cell cell,
                             const Rsyn::TimingMode mode);

       public:
        virtual bool run(const Rsyn::Json &params);

};  // end class
}
#endif
