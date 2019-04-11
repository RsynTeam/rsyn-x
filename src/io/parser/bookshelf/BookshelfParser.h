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

#ifndef BOOKSHELF_PARSER_H
#define BOOKSHELF_PARSER_H

#include "phy/util/BookshelfDscp.h"

#include <string>
#include <fstream>
#include <vector>

// -----------------------------------------------------------------------------

class BookshelfParser {
       private:
        std::string clsBaseFilename;  // obtained from aux filename
        std::string clsPath;          // path to aux file (without filename)

        std::ifstream clsCurrentFile;
        std::string clsCurrentFilename;  // the file currently being parsed
        int clsCurrentLine;

        std::vector<std::string> clsTokens;
        int clsCurrentToken;

        std::string clsFilenameNodes;
        std::string clsFilenameNets;
        std::string clsFilenamePL;
        std::string clsFilenameSCL;
        std::string clsFilenameWTS;
        std::string clsFilenameColors;

        void parseAuxiliary(const std::string &filename, BookshelfDscp &dscp);
        void parsePL(const std::string &filename, BookshelfDscp &dscp);
        void parseSCL(const std::string &filename, BookshelfDscp &dscp);
        void parseNets(const std::string &filename, BookshelfDscp &dscp);
        void parseNodes(const std::string &filename, BookshelfDscp &dscp);
        void parseWTS(const std::string &filename, BookshelfDscp &dscp);

        void parsingError(const std::string &msg);

        bool openFile(const std::string &filename);

        void nextLine(const bool skipDummyLines = true,
                      const bool tokenize =
                          true);  // dummy lines = comment lines, blank lines
        bool tryNextLine(const bool skipDummyLines = true,
                         const bool tokenize =
                             true);  // dummy lines = comment lines, blank lines
        bool tryNextToken(std::string &token);

        void readSign(const std::string &sign);
        void readString(std::string &value);
        void readInteger(int &value);
        void readDouble(double &value);
        void readColon();
        void readKeyword(const std::string &keyword);
        void readIntegerAssignment(const std::string &keyword, int &value);
        void readStringAssignment(const std::string &keyword,
                                  std::string &value);

        bool tryReadString(std::string &value);
        bool tryReadInteger(int &value);
        bool tryReadDouble(double &value);
        bool tryReadColon();
        bool tryReadKeyword(const std::string &keyword);

        // Remove path from a path + filename string.
        std::string removePath(const std::string &str) const;

        // Remove extension from filename string.
        std::string removeExtension(const std::string &str) const;

        // Obtain the extension from a filename.
        std::string obtainExtension(const std::string &str) const;

        // Obtain the path from a filename.
        std::string obtainPath(const std::string &str) const;

       public:
        const std::string &getBaseFilename() const { return clsBaseFilename; }
        const std::string &getPath() const { return clsPath; }
        void parsePlaced(const std::string &filename, BookshelfDscp &dscp) {
                parsePL(filename, dscp);
        }
        void parse(const std::string &filename, BookshelfDscp &dscp);
};  // end class

#endif
