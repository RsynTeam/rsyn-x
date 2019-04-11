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

#ifndef RSYN_WRITER_H
#define RSYN_WRITER_H

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Service.h"
#include "session/Session.h"
#include "WriterDEF.h"

namespace Rsyn {
class PhysicalService;
}

namespace Rsyn {

class Timer;
class RoutingEstimator;

class Writer : public Service {
       private:
        // Circuitry
        Rsyn::Design clsDesign;
        Rsyn::Library clsLibrary;
        Rsyn::Module clsModule;

        // Physical Design
        Rsyn::PhysicalDesign clsPhysicalDesign;

        // Services
        Timer *clsTimer = nullptr;
        RoutingEstimator *clsRoutingEstimator = nullptr;

       public:
        virtual void start(const Rsyn::Json &params);
        virtual void stop();

        void writeVerilog() {
                writeVerilog(clsDesign.getName() + "-cada085.v");
        }  // write verilog file
        void writeTimingFile();
        void writeTimingFile(ostream &out);
        void writeDEF(const std::string &filename = "",
                      const bool full = false);
        void writeICCAD15DEF(std::string fileName);
        void writeFullDEF(std::string filename);
        void writeSPEF() { writeSPEF(clsDesign.getName() + ".spef"); };
        void writeSPEF(const std::string fileName);
        void writeOPS(std::string filename);
        void writeVerilog(std::string filename);
        void writeBookshelf(const std::string &path = ".",
                            const bool enablePinDisp = true);
        void writeBookshelf2(const std::string &path = ".");
        void writePlacedBookshelf(const std::string &path = ".");
        void writeSPEFFile(ostream &out, const bool onlyFixed = false);

        // Debug function. Should be rethought...
        void printTimingPropagation(ostream &out, bool newLine = false);

};  // end class

}  // end namespace

#endif
