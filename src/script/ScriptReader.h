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

#ifndef Script_READER_H
#define Script_READER_H

#include <string>
#include <cstddef>
#include <iostream>
#include <istream>
#include <sstream>
#include <vector>

namespace ScriptParsing {

class ParsedCommand;
class ParsedParamValue;

class ScriptScanner;
class ScriptParser;

class ScriptReader {
        friend class ScriptParser;

       public:
        ScriptReader() : parser(nullptr), scanner(nullptr) {}

        virtual ~ScriptReader();

        void parseFromFile(const std::string &filename);
        void parseFromString(const std::string &str);
        void parseFromStream(std::istream &stream,
                             const std::string &path = "");

        const std::vector<ParsedCommand> &allCommands();

       private:
        std::vector<ParsedCommand> clsCommands;
        std::string clsCurrentPath;

        ////////////////////////////////////////////////////////////////////////////
        // Flex (Lex) / Bison (Yacc)
        ////////////////////////////////////////////////////////////////////////////

        ScriptParser *parser;
        ScriptScanner *scanner;

        ////////////////////////////////////////////////////////////////////////////
        // ScriptParsing callbacks.
        ////////////////////////////////////////////////////////////////////////////

        void readCommandName(const std::string &value);
        void readPositionalParam(const ParsedParamValue &value);
        void readNamedParam(const std::string &name,
                            const ParsedParamValue &value);

};  // end class

}  // end namespace
#endif
