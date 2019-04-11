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
#include <boost/filesystem/path.hpp>

#include "ScriptReader.h"
#include "Script.tab.hh"
#include "ScriptScanner.h"
#include "ScriptCommand.h"

namespace ScriptParsing {

ScriptReader::~ScriptReader() {
        delete scanner;
        delete parser;
}  // end destructor

// -----------------------------------------------------------------------------

void ScriptReader::parseFromFile(const std::string &filename) {
        std::ifstream file(filename.c_str());
        if (!file.good()) {
                std::cout << "ERROR: Script '" << filename
                          << "' could not be opened.\n";
                std::exit(1);
        }  // end if

        parseFromStream(
            file, boost::filesystem::path(filename).parent_path().string());
}  // end method

// -----------------------------------------------------------------------------

void ScriptReader::parseFromString(const std::string &str) {
        std::istringstream iss(str);
        parseFromStream(iss);
}  // end method

// -----------------------------------------------------------------------------

void ScriptReader::parseFromStream(std::istream &stream,
                                   const std::string &path) {
        clsCurrentPath = path;

        if (!stream.good() && stream.eof()) {
                return;
        }  // end if

        delete scanner;
        try {
                scanner = new ScriptParsing::ScriptScanner(&stream);
        } catch (std::bad_alloc &ba) {
                std::cerr << "Failed to allocate scanner: (" << ba.what()
                          << "), exiting!!\n";
                std::exit(1);
        }  // end catch

        delete parser;
        try {
                parser = new ScriptParsing::ScriptParser(*scanner, *this);
        } catch (std::bad_alloc &ba) {
                std::cerr << "Failed to allocate parser: (" << ba.what()
                          << "), exiting!!\n";
                std::exit(1);
        }  // end catch

        if (parser->parse() != 0) {
                // Parse failed.;

                // When parsing fails, clean-up any successfully parsed commands
                // to
                // avoid misleading the user in thinking the script loaded
                // correctly (
                // the parsing error message may be buried in the log and may
                // not be
                // seen by the user).
                clsCommands.clear();
        } else {
                // Parse succeed.
        }  // end else

}  // end method

// -----------------------------------------------------------------------------

const std::vector<ParsedCommand> &ScriptReader::allCommands() {
        return clsCommands;
}  // end method

// -----------------------------------------------------------------------------

void ScriptReader::readCommandName(const std::string &value) {
        clsCommands.resize(clsCommands.size() + 1);
        clsCommands.back().setName(value);
        clsCommands.back().setPath(clsCurrentPath);
}  // end method

// -----------------------------------------------------------------------------

void ScriptReader::readPositionalParam(const ParsedParamValue &value) {
        clsCommands.back().addPositionalParam(value);
}  // end method

// -----------------------------------------------------------------------------

void ScriptReader::readNamedParam(const std::string &name,
                                  const ParsedParamValue &value) {
        clsCommands.back().addNamedParam(name, value);
}  // end method

}  // end namespace
