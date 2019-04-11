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

#ifndef RANDOM_PLACEMENT_EXAMPLE_OPTO_H
#define RANDOM_PLACEMENT_EXAMPLE_OPTO_H

#include <iostream>
#include <vector>

#include "tool/lnalg/lnalg.h"
#include "tool/lnalg/scrm.h"
#include <Rsyn/Session>
#include <Rsyn/PhysicalDesign>
#include "tool/timing/Timer.h"

namespace ICCAD15 {

class RandomPlacementExample : public Rsyn::Process {
       private:
        Rsyn::Session session;
        Rsyn::Timer *timer;
        Rsyn::Design design;
        Rsyn::Module module;
        Rsyn::PhysicalDesign phDesign;

        void runRandomPlacement();

       public:
        virtual bool run(const Rsyn::Json &params);

};  // end class
}
#endif
