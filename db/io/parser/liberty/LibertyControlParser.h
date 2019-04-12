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
 * LibertyControlParser.h
 *
 *  Created on: Feb 6, 2015
 *      Author: jucemar
 */

#ifndef LIBERTYCONTROLPARSER_H_
#define LIBERTYCONTROLPARSER_H_

#include <string>
using std::string;

#include "io/parser/parser_helper.h"
#include "ispd13/global.h"
#include "util/Units.h"
#include <string.h>
using std::string;

class LibertyControlParser {
       private:
        enum LutType {
                LUT_TYPE_INVALID = -1,

                LUT_TYPE_DELAY,
                LUT_TYPE_SLEW,
                LUT_TYPE_SETUP,
                LUT_TYPE_HOLD
        };  // end enum

        enum LutVariableType {
                LUT_VARIABLE_INVALID = -1,

                LUT_VARIABLE_INPUT_NET_TRANSITION,
                LUT_VARIABLE_TOTAL_OUTPUT_NET_CAPACITANCE,
                LUT_VARIABLE_RELATED_PIN_TRANSITION,
                LUT_VARIABLE_CONSTRAINED_PIN_TRANSITION
        };  // end enum

        struct LookUpTableTemplate {
                LutVariableType var1;
                LutVariableType var2;

                int loadIndex;
                int slewIndex;

                LookUpTableTemplate()
                    : loadIndex(-1),
                      slewIndex(-1),
                      var1(LUT_VARIABLE_INVALID),
                      var2(LUT_VARIABLE_INVALID) {}
        };  // end struct

        LutVariableType getLutVariableTypeFromString(const std::string &str) {
                if (str == "input_net_transition")
                        return LUT_VARIABLE_INPUT_NET_TRANSITION;
                if (str == "total_output_net_capacitance")
                        return LUT_VARIABLE_TOTAL_OUTPUT_NET_CAPACITANCE;
                if (str == "related_pin_transition")
                        return LUT_VARIABLE_RELATED_PIN_TRANSITION;
                if (str == "constrained_pin_transition")
                        return LUT_VARIABLE_CONSTRAINED_PIN_TRANSITION;
                return LUT_VARIABLE_INVALID;
        }  // end if

        void parseLiberty_LookUpTable(si2drGroupIdT &gtiming2, si2drErrorT &err,
                                      ISPD13::LibParserLUT &lut,
                                      const LutType &lutType);

        std::map<std::string, LookUpTableTemplate> lutTemplates;
        Rsyn::UnitPrefix unitPrefixForTime;
        Rsyn::UnitPrefix unitPrefixForCapacitance;
        Rsyn::UnitPrefix unitPrefixForLeakagePower;

       public:
        bool print;
        LibertyControlParser();
        void parseLiberty(const string &filename, ISPD13::LIBInfo &lib);
        virtual ~LibertyControlParser();

        const Rsyn::UnitPrefix getUnitPrefixForCapacitance() const {
                return unitPrefixForCapacitance;
        }

        const Rsyn::UnitPrefix getUnitPrefixForTime() const {
                return unitPrefixForTime;
        }
};

#endif /* LIBERTYCONTROLPARSER_H_ */
