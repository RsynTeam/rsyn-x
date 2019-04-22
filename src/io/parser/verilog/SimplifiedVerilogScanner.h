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

#ifndef SIMPLIFIED_VERILOG_SCANNER_H
#define SIMPLIFIED_VERILOG_SCANNER_H

#if !defined(yyFlexLexerOnce)
// Use a copy from the enviroment where the parser was generated in order to
// improve portability.
#include "FlexLexerCopy.h"
#endif

#include "SimplifiedVerilog.tab.hh"
#include "location.hh"

namespace Parsing {

class SimplifiedVerilogScanner : public yyFlexLexer {
       public:
        SimplifiedVerilogScanner(std::istream *in) : yyFlexLexer(in) {
                loc = new Parsing::SimplifiedVerilogParser::location_type();
        };
        virtual ~SimplifiedVerilogScanner() { delete loc; };

        // Get rid of override virtual function warning.
        using FlexLexer::yylex;

        virtual int yylex(
            Parsing::SimplifiedVerilogParser::semantic_type *const lval,
            Parsing::SimplifiedVerilogParser::location_type *location);

       private:
        Parsing::SimplifiedVerilogParser::semantic_type *yylval = nullptr;
        Parsing::SimplifiedVerilogParser::location_type *loc = nullptr;
};

}  // end namespace

#endif
