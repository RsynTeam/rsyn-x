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

#ifndef PHYSICALDESIGNEXAMPLE_H
#define PHYSICALDESIGNEXAMPLE_H

#include "session/Process.h"
#include <Rsyn/Session>
#include <Rsyn/PhysicalDesign>

class PhysicalDesignExample : public Rsyn::Process {
       protected:
        Rsyn::Session clsSession;
        Rsyn::PhysicalDesign clsPhDesign;
        Rsyn::Design clsDesign;
        Rsyn::Module clsTopModule;

       public:
        PhysicalDesignExample() = default;
        PhysicalDesignExample(const PhysicalDesignExample& orig) = delete;
        virtual ~PhysicalDesignExample() = default;
        bool run(const Rsyn::Json& params);

       private:
        void visitAllPhysicalInstances();
        void visitAllPhysicalRows();
        void assigningUserDefinedDataToRows();
        void moveCells();
        void visitAllPhysicalLayers();
};

#endif /* PHYSICALDESIGNEXAMPLE_H */
