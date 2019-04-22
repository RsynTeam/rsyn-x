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

#ifndef SIMPLIFIED_VERILOG_READER_H
#define SIMPLIFIED_VERILOG_READER_H

#include <string>
#include <cstddef>
#include <istream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "io/legacy/PlacerInternals.h"

#include "SimplifiedVerilogScanner.h"
#include "SimplifiedVerilog.tab.hh"

namespace Parsing {

enum IdentifierListType {
        IDENTIFIER_LIST_NONE,
        IDENTIFIER_LIST_INPUT_PORT,
        IDENTIFIER_LIST_OUTPUT_PORTS,
        IDENTIFIER_LIST_NETS
};  // end enum

enum ErrorCode { ERROR_UNNAMED_PORT_MAPPING_NOT_SUPPORTED };  // end enum

class SimplifiedVerilogReader {
        friend class SimplifiedVerilogParser;

       public:
        SimplifiedVerilogReader(Legacy::Design &verilogDescriptor)
            : clsVerilogDescriptor(verilogDescriptor),
              parser(nullptr),
              scanner(nullptr) {}

        virtual ~SimplifiedVerilogReader();

        void parseFromFile(const std::string &filename);
        void parseFromString(const std::string &str);
        void parseFromStream(std::istream &stream);

       private:
        Legacy::Design &clsVerilogDescriptor;
        std::unordered_map<std::string, int> netmap;
        std::unordered_set<std::string> ports;

        int createNet(const std::string &name, bool &alreadyExisted) {
                int index;

                auto it = netmap.find(name);
                if (it == netmap.end()) {
                        index = netmap.size();
                        clsVerilogDescriptor.nets.resize(index + 1);
                        clsVerilogDescriptor.nets[index].name = name;
                        netmap[name] = index;
                        alreadyExisted = false;
                } else {
                        index = it->second;
                        alreadyExisted = true;
                }  // end else

                return index;
        }  // end if

        ////////////////////////////////////////////////////////////////////////////
        // Flex (Lex) / Bison (Yacc)
        ////////////////////////////////////////////////////////////////////////////

        Parsing::SimplifiedVerilogParser *parser;
        Parsing::SimplifiedVerilogScanner *scanner;

        virtual void setBusRange(int &, int &) {}

        ////////////////////////////////////////////////////////////////////////////
        // Parsing state and callbacks.
        ////////////////////////////////////////////////////////////////////////////
        IdentifierListType currentIdentifierListType;

        void setCurrentIdentifierListType(const IdentifierListType type);

        void readModuleName(const std::string &name);
        void readIdentifier(const std::string &name);
        void readInstance(const std::string &type, const std::string &name);
        void readConnection(const std::string &portName,
                            const std::string &netName);

        void error(const ErrorCode code);

        // Note: Workaround to add primary output/input as connections to nets.
        void workaround();

};  // end class

}  // end namespace
#endif
