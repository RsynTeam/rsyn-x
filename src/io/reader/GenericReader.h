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

#ifndef RSYN_GENERIC_READER_H
#define RSYN_GENERIC_READER_H

#include "session/Session.h"
#include "tool/timing/TimingTypes.h"

namespace Rsyn {

class GenericReader : public Reader {
        Session session;

        std::vector<std::string> lefFiles;
        std::vector<std::string> defFiles;
        std::string verilogFile;
        std::string sdcFile;
        std::string libertyFileEarly;
        std::string libertyFileLate;

        Number localWireCapacitancePerMicron;
        Number localWireResistancePerMicron;
        DBU maxWireSegmentInMicron;

        bool enableTiming = false;
        bool enableNetlistFromVerilog = false;
        bool enableRSTT = false;
        bool sameEarlyLateLibFiles = false;

       public:
        GenericReader() = default;

        virtual bool load(const Rsyn::Json& params) override;

       private:
        LefDscp lefDescriptor;
        DefDscp defDescriptor;
        Legacy::Design verilogDescriptor;
        ISPD13::LIBInfo libInfoEarly;
        ISPD13::LIBInfo libInfoLate;
        ISPD13::SDCInfo sdcInfo;

        void parsingFlow();
        void parseLEFFiles();
        void parseDEFFiles();
        void parseVerilogFile();
        void parseLibertyFile();
        void parseSDCFile();
        void populateDesign();
        void initializeAuxiliarInfrastructure();
};  // end class

}  // end namespace

#endif /* GENERICREADER_H */
