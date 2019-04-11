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

#ifndef RSYN_REPORT_H
#define RSYN_REPORT_H

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Service.h"
#include "tool/timing/TimingTypes.h"
#include "session/Session.h"

namespace Rsyn {
class PhysicalService;
}

namespace Rsyn {

class Timer;
class LibraryCharacterizer;
class RoutingEstimator;

class Report : public Service {
       private:
        // Session
        Session clsSession;

        // Circuitry
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;

        // Physical Design
        Rsyn::PhysicalDesign clsPhysicalDesign;

        // Services
        Timer *clsTimer;
        const LibraryCharacterizer *clsLibraryCharacterizer;
        RoutingEstimator *clsRoutingEstimator;

        // Auxiliary function for reporting objects.
        void reportPin_Header();
        void reportPin_Data(Rsyn::Pin pin, const TimingMode mode);

        void reportArc_Header();
        void reportArc_Data(Rsyn::Arc arc, const TimingMode mode);

        void reportPin_TimingInformation(Rsyn::Pin pin, const TimingMode mode);
        void reportCell_TimingInformation(Rsyn::Cell cell,
                                          const TimingMode mode);
        void reportNet_TimingInformation(Rsyn::Net net, const TimingMode mode);

       public:
        virtual void start(const Rsyn::Json &params);
        virtual void stop();

        // Design
        void reportLogicDesign();
        void reportPhysicalDesign();

        // Objects
        void reportPin(Rsyn::Pin pin, const bool late = true,
                       const bool early = false);
        void reportCell(Rsyn::Cell cell, const bool late = true,
                        const bool early = false);
        void reportNet(Rsyn::Net net, const bool late = true,
                       const bool early = false);
        void reportTree(Rsyn::Net net);

};  // end class

}  // end namespace

#endif
