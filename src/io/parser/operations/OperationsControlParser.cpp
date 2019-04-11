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

/*
 * OperationsControlParser.cpp
 *
 *  Created on: Feb 9, 2015
 *      Author: jucemar
 */

#include "OperationsControlParser.h"

#include <iomanip>

OperationsControlParser::OperationsControlParser() {}

// =============================================================================
// Operations
// =============================================================================

void OperationsControlParser::parseOperations(const string& opsFile,
                                              ISPD13::OpsInfo& opsInfo) {
        ISPD13::OperationsParser tp(opsFile);

        cout << "\tOperations file: " << opsFile << "\n";
        bool valid, early, rise;
        string gate = "", pin = "", net = "", file = "", cell = "";
        int paths;
        ISPD13::CommandTypeEnum cmdType;
        ISPD13::QueryTypeEnum queryType;

        do {
                valid = tp.read_ops_tau15(cmdType, queryType, pin, net, file,
                                          gate, cell, early, rise, paths);

                if (!valid) break;

                ISPD13::OrderInfo newOp;

                if (cmdType != ISPD13::UNKNOWN) {
                        newOp.index = opsInfo.cmds.size();
                        newOp.type = ISPD13::CMD;
                        ISPD13::CommandStruct cs(pin, net, file, gate, cell,
                                                 cmdType);
                        opsInfo.cmds.push_back(cs);
                        opsInfo.order.push_back(newOp);

                } else if (queryType != ISPD13::NOT_VALID) {
                        newOp.index = opsInfo.queries.size();
                        newOp.type = ISPD13::QUERY;
                        ISPD13::QueryStruct qs(pin, early, rise, paths,
                                               queryType);
                        opsInfo.queries.push_back(qs);
                        opsInfo.order.push_back(newOp);

                } else {
                        cerr << "[WARNING] Ignoring unknown operation from "
                                ".ops file.\n";
                }

        } while (valid);

        cout << "\tRead operations: " << setw(10)
             << opsInfo.cmds.size() + opsInfo.queries.size() << endl;
        cout << "\t\tcommands.........: " << setw(10) << opsInfo.cmds.size()
             << endl;
        cout << "\t\ttiming queries...: " << setw(10) << opsInfo.queries.size()
             << endl;
}

// -----------------------------------------------------------------------------

OperationsControlParser::~OperationsControlParser() {
        // TODO Auto-generated destructor stub
}
