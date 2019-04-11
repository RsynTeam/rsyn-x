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

#include <thread>

#include "sandbox/Sandbox.h"
#include "tool/timing/SandboxTimer.h"
#include "SandboxTest.h"

namespace Testing {

bool SandboxTest::run(const Rsyn::Json &params) {
        this->session = session;
        this->design = session.getDesign();
        this->module = design.getTopModule();

        test();

        return true;
}  // end method

// -----------------------------------------------------------------------------

void SandboxTest::test() {
        Rsyn::LibraryCell lcell = design.findLibraryCellByName("NAND2_X1");

        Rsyn::Sandbox sandbox;
        sandbox.create(design.getTopModule());

        Rsyn::SandboxAttribute<Rsyn::SandboxInstance, int> attr =
            sandbox.createAttribute(123);

        Rsyn::SandboxPort ip1 = sandbox.createPort(Rsyn::IN);
        Rsyn::SandboxPort ip2 = sandbox.createPort(Rsyn::IN);
        Rsyn::SandboxPort op1 = sandbox.createPort(Rsyn::OUT);

        Rsyn::SandboxCell cell = sandbox.createCell(lcell);

        Rsyn::SandboxPin IN1 = ip1.getInnerPin();
        Rsyn::SandboxPin IN2 = ip2.getInnerPin();
        Rsyn::SandboxPin OUT = op1.getInnerPin();
        Rsyn::SandboxPin A = cell.getPinByName("a");
        Rsyn::SandboxPin B = cell.getPinByName("b");
        Rsyn::SandboxPin O = cell.getPinByName("o");

        Rsyn::SandboxNet n1 = sandbox.createNet("n1");
        Rsyn::SandboxNet n2 = sandbox.createNet("n2");
        Rsyn::SandboxNet n3 = sandbox.createNet("n3");

        IN1.connect(n1);
        A.connect(n1);

        IN2.connect(n2);
        B.connect(n2);

        OUT.connect(n3);
        O.connect(n3);

std:
        cout << "Sandobox...\n";

        for (Rsyn::SandboxPort port : sandbox.allPorts()) {
                std::cout << "Port: " << port.getName() << "\n";
        }  // end for

        for (Rsyn::SandboxInstance instance : sandbox.allInstances()) {
                std::cout << "Instance: " << instance.getName() << " "
                          << attr[instance] << "\n";
        }  // end for

        for (Rsyn::SandboxNet net : sandbox.allNets()) {
                std::cout << "Net: " << net.getName() << "\n";
        }  // end for

        Rsyn::SandboxTimer timer;
        timer.init(session, sandbox);
        timer.setOutputRequiredTime(op1, Rsyn::LATE,
                                    Rsyn::EdgeArray<Number>(0, 0));
        timer.updateTimingFull();

        std::cout << "WNS: " << timer.getWns(Rsyn::LATE) << "\n";
        std::cout << "TNS: " << timer.getTns(Rsyn::LATE) << "\n";
}  // end method

}  // end namescape
