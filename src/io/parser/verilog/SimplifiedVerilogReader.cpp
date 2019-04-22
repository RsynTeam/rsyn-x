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

#include <cctype>
#include <fstream>
#include <cassert>

#include "SimplifiedVerilogReader.h"


/* The c++ scanner is a mess. The FlexLexer.h header file relies on the
 * following macro. This is required in order to pass the c++-multiple-scanners
 * test in the regression suite. We get reports that it breaks inheritance.
 * We will address this in a future release of flex, or omit the C++ scanner
 * altogether.
 */
#ifndef yyFlexLexer
#define yyFlexLexer yyScriptFlexLexer
#endif

namespace Parsing {

SimplifiedVerilogReader::~SimplifiedVerilogReader() {
        delete scanner;
        delete parser;
}  // end destructor

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::parseFromFile(const std::string &filename) {
        std::ifstream in_file(filename.c_str());
        if (!in_file.good()) {
                std::exit(1);
        }  // end if
        parseFromStream(in_file);
}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::parseFromString(const std::string &str) {
        std::istringstream iss(str);
        parseFromStream(iss);
}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::parseFromStream(std::istream &stream) {
        if (!stream.good() && stream.eof()) {
                return;
        }  // end if

        currentIdentifierListType = IDENTIFIER_LIST_NONE;

        delete (scanner);
        try {
                scanner = new Parsing::SimplifiedVerilogScanner(&stream);
        } catch (std::bad_alloc &ba) {
                std::cerr << "Failed to allocate scanner: (" << ba.what()
                          << "), exiting!!\n";
                std::exit(1);
        }  // end catch

        delete (parser);
        try {
                parser = new Parsing::SimplifiedVerilogParser(
                    (*scanner) /* scanner */, (*this) /* driver */);
        } catch (std::bad_alloc &ba) {
                std::cerr << "Failed to allocate parser: (" << ba.what()
                          << "), exiting!!\n";
                std::exit(1);
        }  // end catch

        if (parser->parse()) {
                std::cout << "Parse failed.\n";
                std::exit(1);
        } else {
                std::cout << "Parse succeed.\n";
        }  // end else

        workaround();
}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::setCurrentIdentifierListType(
    const IdentifierListType type) {
        // std::cout << "list type " << type << "\n";
        currentIdentifierListType = type;
}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::readModuleName(const std::string &name) {
        // std::cout << "module " << name << "\n";
        clsVerilogDescriptor.name = name;
}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::readIdentifier(const std::string &name) {
        // std::cout << "identifier " << name << "\n";

        switch (currentIdentifierListType) {
                case IDENTIFIER_LIST_INPUT_PORT: {
                        clsVerilogDescriptor.primaryInputs.push_back(name);
                        Legacy::Design::Pin designPin;
                        designPin.name = name;
                        designPin.net = name;
                        designPin.direction = "INPUT";
                        designPin.x = designPin.y = -1;
                        clsVerilogDescriptor.ports.push_back(designPin);

                        if (ports.count(name)) {
                                std::cout << "[WARNING] Multiple definition of "
                                             "primary input '"
                                          << name << "'\n";
                        } else {
                                bool alreadyExisted;
                                createNet(name, alreadyExisted);
                                ports.insert(name);
                        }  // end if

                        break;
                }  // end case

                case IDENTIFIER_LIST_OUTPUT_PORTS: {
                        clsVerilogDescriptor.primaryOutputs.push_back(name);
                        Legacy::Design::Pin designPin;
                        designPin.name = name;
                        designPin.net = name;
                        designPin.direction = "OUTPUT";
                        designPin.x = designPin.y = -1;
                        clsVerilogDescriptor.ports.push_back(designPin);

                        if (ports.count(name)) {
                                std::cout << "[WARNING] Multiple definition of "
                                             "primary output '"
                                          << name << "'\n";
                        } else {
                                bool alreadyExisted;
                                createNet(name, alreadyExisted);
                                ports.insert(name);
                        }  // end if

                        break;
                }  // end case

                case IDENTIFIER_LIST_NETS: {
                        bool alreadyExisted;
                        createNet(name, alreadyExisted);
                        if (alreadyExisted && !ports.count(name)) {
                                std::cout
                                    << "WARNING: Multiple definition of net '"
                                    << name << "'\n";
                        }  // end else

                        break;
                }  // end case
        }          // end switch
}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::readInstance(const std::string &type,
                                           const std::string &name) {
        // std::cout << "instance " << type << " " << name << "\n";

        Legacy::Design::Component designComponent;
        designComponent.id = type;
        designComponent.name = name;
        designComponent.fixed = true;
        designComponent.placed = false;
        designComponent.x = designComponent.y = -1;

        clsVerilogDescriptor.components.push_back(designComponent);

}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::readConnection(const std::string &portName,
                                             const std::string &netName) {
        // std::cout << "connection " << portName << " -> " << &netName << "\n";

        // Add the net if it does not exists.
        bool alreadyExisted;
        const int netId = createNet(netName, alreadyExisted);
        if (!alreadyExisted) {
                std::cout << "WARNING: Net '" << netName
                          << "' is used without being declared.\n";
        }  // end if

        Legacy::Design::Connection connection;
        connection.pin = portName;
        connection.instance = clsVerilogDescriptor.components.back().name;
        clsVerilogDescriptor.nets[netId].connections.push_back(connection);
}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::error(const ErrorCode code) {
        switch (code) {
                case ERROR_UNNAMED_PORT_MAPPING_NOT_SUPPORTED:
                        std::cout << "Parsing Error: Unnamed port mapping is "
                                     "not yet supported.\n";
                        break;
                default:
                        std::cout << "Parsing Error: Unknown.\n";
        }  // end switch
        std::exit(1);
}  // end method

// -----------------------------------------------------------------------------

void SimplifiedVerilogReader::workaround() {
        for (unsigned i = 0; i < clsVerilogDescriptor.primaryInputs.size();
             i++) {
                const std::string &netName =
                    clsVerilogDescriptor.primaryInputs[i];

                auto it = netmap.find(netName);
                if (it == netmap.end()) {
                        std::cout << "WARNING: Primary input without a "
                                     "corresponding net. NetName: "
                                  << netName << "\n";
                } else {
                        Legacy::Design::Connection connection;
                        connection.instance = "PIN";
                        connection.pin = netName;
                        clsVerilogDescriptor.nets[it->second]
                            .connections.push_back(connection);

                        if (netName == "") {
                                std::cout << "[ERROR] Empty net name when "
                                             "adding primary input "
                                             "connections.\n";
                                std::exit(1);
                        }  // end if
                }          // end else
        }                  // end for

        for (unsigned i = 0; i < clsVerilogDescriptor.primaryOutputs.size();
             i++) {
                const std::string &netName =
                    clsVerilogDescriptor.primaryOutputs[i];

                auto it = netmap.find(netName);
                if (it == netmap.end()) {
                        std::cout << "WARNING: Primary output without a "
                                     "corresponding net. NetName: "
                                  << netName << "\n";
                } else {
                        Legacy::Design::Connection connection;
                        connection.instance = "PIN";
                        connection.pin = netName;
                        clsVerilogDescriptor.nets[it->second]
                            .connections.push_back(connection);

                        if (netName == "") {
                                std::cout << "[ERROR] Empty net name when "
                                             "adding primary output "
                                             "connections.\n";
                                std::exit(1);
                        }  // end if
                }          // end else
        }                  // end for
}  // end method

}  // end namespace
