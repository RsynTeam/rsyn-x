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
 * OperationsControlParser.h
 *
 *  Created on: Feb 9, 2015
 *      Author: jucemar
 */

#ifndef PARSER_OPERATIONS_OPERATIONSCONTROLPARSER_H_
#define PARSER_OPERATIONS_OPERATIONSCONTROLPARSER_H_

#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;

#include "ispd13/global.h"

class OperationsControlParser {
       public:
        OperationsControlParser();
        void parseOperations(const string& opsFile, ISPD13::OpsInfo& opsInfo);
        virtual ~OperationsControlParser();
};

#endif /* PARSER_OPERATIONS_OPERATIONSCONTROLPARSER_H_ */
