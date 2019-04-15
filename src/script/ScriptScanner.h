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

#ifndef Script_SCANNER_H
#define Script_SCANNER_H

/* From Flex documentation:
 *   The c++ scanner is a mess. The FlexLexer.h header file relies on the
 *   following macro. This is required in order to pass the
 * c++-multiple-scanners
 *   test in the regression suite. We get reports that it breaks inheritance.
 *   We will address this in a future release of flex, or omit the C++ scanner
 *   altogether.
 *
 * So, for now, let's define this variable too...
 */

#define yyFlexLexer yyScriptFlexLexer

#if !defined(yyFlexLexerOnce)
// Use a copy from the enviroment where the parser was generated in order to
// improve portability.
#include "FlexLexerCopy.h"
#endif

#undef yylex

#include "Script.tab.hh"

namespace ScriptParsing {

class ScriptScanner : public yyFlexLexer {
       public:
        ScriptScanner(std::istream *in) : yyFlexLexer(in) {
                loc = new ScriptParser::location_type();
        };
        virtual ~ScriptScanner() { delete loc; };

        // Get rid of override virtual function warning.
        using FlexLexer::yylex;

        virtual int yylex(ScriptParser::semantic_type *const lval,
                          ScriptParser::location_type *location);

       private:
        ScriptParser::semantic_type *yylval = nullptr;
        ScriptParser::location_type *loc = nullptr;
};

}  // end namespace

#endif
