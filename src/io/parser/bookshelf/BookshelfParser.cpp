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

#include "BookshelfParser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>

// -----------------------------------------------------------------------------

void BookshelfParser::parse(const std::string &filename, BookshelfDscp &dscp) {
        // Parse each bookshelf file.
        parseAuxiliary(filename, dscp);

        parseNodes(clsFilenameNodes, dscp);
        parsePL(clsFilenamePL, dscp);
        parseNets(clsFilenameNets, dscp);
        parseSCL(clsFilenameSCL, dscp);
        // parseWTS  ( clsFilenameWTS   );
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::parsingError(const std::string &msg) {
        std::ostringstream oss;
        oss << "[PARSING ERROR] " << removePath(clsCurrentFilename) << ":"
            << clsCurrentLine << ": " << msg;

        throw std::string(oss.str());
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::readString(std::string &value) {
        if (!tryReadString(value)) parsingError("Expecting <String>.");
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::readInteger(int &value) {
        if (!tryReadInteger(value)) parsingError("Expecting <Integer>.");
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::readDouble(double &value) {
        if (!tryReadDouble(value)) parsingError("Expecting <Integer>.");
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::readKeyword(const std::string &keyword) {
        if (!tryReadKeyword(keyword))
                parsingError("Expecting keyword '" + keyword + "'.");
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::readColon() {
        if (!tryReadColon()) parsingError("Expecting ':'.");
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::readIntegerAssignment(const std::string &keyword,
                                            int &value) {
        readKeyword(keyword);
        readColon();
        readInteger(value);
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::readStringAssignment(const std::string &keyword,
                                           std::string &value) {
        readKeyword(keyword);
        readColon();
        readString(value);
}  // end method

// -----------------------------------------------------------------------------

bool BookshelfParser::tryReadString(std::string &value) {
        return tryNextToken(value);
}  // end method

// -----------------------------------------------------------------------------

bool BookshelfParser::tryReadKeyword(const std::string &keyword) {
        std::string token;
        if (!tryNextToken(token)) return false;

        if (token != keyword)
                parsingError("Expecting '" + keyword + "' got '" + token +
                             "'.");

        return true;
}  // end method

// -----------------------------------------------------------------------------

bool BookshelfParser::tryReadColon() {
        return tryReadKeyword(":");
}  // end method

// -----------------------------------------------------------------------------

bool BookshelfParser::tryReadInteger(int &value) {
        std::string token;
        if (tryNextToken(token)) {
                std::istringstream iss(token);
                iss >> value;
                return !iss.fail();
        } else {
                return false;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

bool BookshelfParser::tryReadDouble(double &value) {
        std::string token;
        if (tryNextToken(token)) {
                std::istringstream iss(token);
                iss >> value;
                return !iss.fail();
        } else {
                return false;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

bool BookshelfParser::openFile(const std::string &filename) {
        clsCurrentFilename = filename;
        clsCurrentLine = 0;

        if (clsCurrentFile.is_open()) clsCurrentFile.close();

        clsCurrentFile.open(filename.c_str());
        if (!clsCurrentFile) throw std::string("File could not be opened!");

        return tryNextLine(false, true);
}  // end method

// -----------------------------------------------------------------------------

bool BookshelfParser::tryNextToken(std::string &token) {
        if (clsCurrentToken >= clsTokens.size()) return false;
        token = clsTokens[clsCurrentToken++];
        return true;
}  // end method

// -----------------------------------------------------------------------------

bool BookshelfParser::tryNextLine(const bool skipDummyLines,
                                  const bool tokenize) {
        std::string line;
        bool retry;

        clsTokens.clear();
        clsCurrentToken = 0;

        do {
                retry = false;

                getline(clsCurrentFile, line);

                if (clsCurrentFile.bad())
                        parsingError("Can't read from the file.");

                if (clsCurrentFile.fail()) return false;

                if (skipDummyLines) {
                        // Skip empty lines.
                        if (line.size() == 0) retry = true;

                        // Skip comment lines.
                        if (line[0] == '#') retry = true;

                        // Skip blank lines.
                        if (line.find_first_not_of(' ') == std::string::npos)
                                retry = true;
                }  // end if

                clsCurrentLine++;
        } while (retry);

        // Tokenize
        if (tokenize) {
                std::istringstream iss(line);

                std::string token;
                while (iss >> token) clsTokens.push_back(token);
        } else {
                clsTokens.push_back(line);
        }  // end else

        return true;
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::nextLine(const bool skipDummyLines,
                               const bool dontTokenize) {
        if (!tryNextLine(skipDummyLines))
                parsingError("Unexpected end of file.");
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::parseAuxiliary(const std::string &filename,
                                     BookshelfDscp &dscp) {
        clsPath = obtainPath(filename);
        clsBaseFilename = removeExtension(removePath(filename));

        dscp.clsDesignName = clsBaseFilename;

        std::ifstream file(filename.c_str());
        if (!file) {
                // [TODO] throw!!!
                std::cerr << "[ERROR] Auxiliary file " << filename
                          << " could not be opened.\n";
                return;
        }  // end if

        const std::string path = obtainPath(filename);

        std::string token;
        std::string colon;
        file >> token >> colon;

        if (token != "RowBasedPlacement")
                parsingError("Expecting 'RowBasedPlacement'");

        if (colon != ":") parsingError("Expecting ':'.");

        while (file >> token) {
                const std::string extension = obtainExtension(token);

                if (extension == "nodes")
                        clsFilenameNodes = path + token;
                else if (extension == "pl")
                        clsFilenamePL = path + token;
                else if (extension == "nets")
                        clsFilenameNets = path + token;
                else if (extension == "scl")
                        clsFilenameSCL = path + token;
                else if (extension == "wts")
                        clsFilenameWTS = path + token;
                else
                        std::cerr << "[WARNING] File " << token
                                  << " in auxiliary file was ignored.\n";
        };  // end while
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::parseNodes(const std::string &filename,
                                 BookshelfDscp &dscp) {
        if (!openFile(filename)) parsingError("Empty file!");

        readKeyword("UCLA");
        readKeyword("nodes");
        readKeyword("1.0");

        int numNodes;
        int numTerminals;

        nextLine();
        readIntegerAssignment("NumNodes", numNodes);
        if (numNodes <= 0) parsingError("Invalid number of nodes");

        nextLine();
        readIntegerAssignment("NumTerminals", numTerminals);
        if (numTerminals < 0) parsingError("Invalid number of terminals");

        dscp.clsNodes.reserve(numNodes);
        dscp.clsMapNodes.reserve(numNodes);

        int counterNodes = 0;
        int counterTerminals = 0;

        std::string token;
        std::string nodeName;
        double nodeWidth;
        double nodeHeight;
        bool nodeIsTerminal;

        while (tryNextLine() && counterNodes < numNodes) {
                readString(nodeName);
                readDouble(nodeWidth);
                readDouble(nodeHeight);

                if (tryReadString(token)) {
                        if (token != "terminal")
                                parsingError("Expected 'terminal'");
                        nodeIsTerminal = true;
                } else {
                        nodeIsTerminal = false;
                }  // end else

                dscp.clsMapNodes[nodeName] = dscp.clsNodes.size();
                dscp.clsNodes.push_back(BookshelfNode());
                BookshelfNode &node = dscp.clsNodes.back();
                node.clsName = nodeName;
                node.clsSize = double2(nodeWidth, nodeHeight);
                node.clsIsTerminal = nodeIsTerminal;

                counterNodes++;
                if (nodeIsTerminal) counterTerminals++;
        }  // end while

        if (counterNodes != numNodes)
                parsingError("Number of nodes dosen't match.");

        if (counterTerminals != numTerminals)
                parsingError("Number of terminals dosen't match.");
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::parsePL(const std::string &filename,
                              BookshelfDscp &dscp) {
        if (!openFile(filename)) parsingError("Empty file!");

        readKeyword("UCLA");
        readKeyword("pl");
        readKeyword("1.0");

        int counterNodes = 0;

        std::string token;
        std::string nodeName;
        std::string nodeOrientation;
        double nodeX;
        double nodeY;
        bool nodeIsFixed;

        while (tryNextLine()) {
                readString(nodeName);
                readDouble(nodeX);
                readDouble(nodeY);
                readColon();
                readString(nodeOrientation);

                nodeIsFixed = tryReadKeyword("/FIXED");

                if (dscp.clsMapNodes.find(nodeName) == dscp.clsMapNodes.end()) {
                        dscp.clsMapNodes[nodeName] = dscp.clsNodes.size();
                        dscp.clsNodes.push_back(BookshelfNode());
                }  // end if

                const int index = dscp.clsMapNodes[nodeName];
                BookshelfNode &node = dscp.clsNodes[index];
                node.clsName = nodeName;
                node.clsPos = double2(nodeX, nodeY);
                node.clsOrientation = nodeOrientation;
                node.clsIsFixed = nodeIsFixed;
                node.clsSetPosition = true;

                counterNodes++;
        }  // end while

        // Check if the position of all nodes have been defined.
        const int numNodes = dscp.clsNodes.size();

        if (counterNodes != numNodes) {
                for (const BookshelfNode &node : dscp.clsNodes) {
                        if (!node.clsSetPosition)
                                parsingError("The position of node '" +
                                             node.clsName +
                                             "' was not defined.");
                }  // end for
        }          // end if
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::parseNets(const std::string &filename,
                                BookshelfDscp &dscp) {
        if (!openFile(filename)) parsingError("Empty file!");

        readKeyword("UCLA");
        readKeyword("nets");
        readKeyword("1.0");

        int numNets;
        int numPins;

        nextLine();
        readIntegerAssignment("NumNets", numNets);
        if (numNets <= 0) parsingError("Invalid number of nets.");

        nextLine();
        readIntegerAssignment("NumPins", numPins);
        if (numPins <= 0) parsingError("Invalid number of pins.");

        dscp.clsNets.reserve(numNets);
        dscp.clsMapNets.reserve(numNets);

        int counterNets = 0;
        int counterPins = 0;

        std::string token;

        std::string netName;
        int netDegree;
        int netIndex;

        std::string edgeNodeName;
        std::string edgeDirection;
        double edgeDx;
        double edgeDy;

        while (tryNextLine() && counterNets < numNets) {
                readKeyword("NetDegree");
                readColon();
                readInteger(netDegree);

                dscp.clsNumPins += netDegree;

                bool hasNetName = tryReadString(netName);

                if (hasNetName) {
                        if (dscp.clsMapNets.find(netName) !=
                            dscp.clsMapNets.end())
                                parsingError("Net " + netName +
                                             " already defined.");
                }  // end if

                dscp.clsMapNets[netName] = dscp.clsNets.size();
                dscp.clsNets.push_back(BookshelfNet());
                BookshelfNet &net = dscp.clsNets.back();
                net.clsHasName = hasNetName;
                if (hasNetName) net.clsName = netName;

                net.clsPins.reserve(netDegree);

                for (int i = 0; i < netDegree; i++) {
                        nextLine();

                        readString(edgeNodeName);
                        readString(edgeDirection);
                        bool hasDisp = tryReadColon();
                        if (hasDisp) {
                                readDouble(edgeDx);
                                readDouble(edgeDy);
                        } else {
                                edgeDx = 0;
                                edgeDy = 0;
                        }  // end else

                        net.clsPins.push_back(BookshelfPin());
                        BookshelfPin &pin = net.clsPins.back();
                        pin.clsNodeName = edgeNodeName;
                        pin.clsDirection = edgeDirection;
                        if (hasDisp)
                                pin.clsDisplacement = double2(edgeDx, edgeDy);

                        counterPins++;
                }  // end for

                counterNets++;
        }  // end while

        if (counterNets != numNets)
                parsingError("Number of nets dosen't match.");

        if (counterPins != numPins)
                parsingError("Number of pins dosen't match.");
}  // end method

// -----------------------------------------------------------------------------

void BookshelfParser::parseSCL(const std::string &filename,
                               BookshelfDscp &dscp) {
        if (!openFile(filename)) parsingError("Empty file!");

        readKeyword("UCLA");
        readKeyword("scl");
        readKeyword("1.0");

        int numRows;

        nextLine();
        std::string token;
        readString(token);
        if (token != "NumRows" && token != "Numrows")
                parsingError("Expecting 'NumRows' or 'Numrows' got '" + token +
                             "'.");
        readColon();
        readInteger(numRows);
        if (numRows <= 0) parsingError("Invalid number of rows.");

        dscp.clsRows.reserve(numRows);

        int rowCoordinate;
        int rowHeight;
        int rowSiteWidth;
        int rowSiteSpacing;
        int rowSubrowOrigin;
        int rowNumSites;
        std::string rowSiteOrient;
        std::string rowSiteSymmetry;

        int counterRows = 0;

        while (tryNextLine() && counterRows < numRows) {
                readKeyword("CoreRow");
                readKeyword("Horizontal");

                nextLine();
                readIntegerAssignment("Coordinate", rowCoordinate);
                nextLine();
                readIntegerAssignment("Height", rowHeight);
                nextLine();
                readIntegerAssignment("Sitewidth", rowSiteWidth);
                nextLine();
                readIntegerAssignment("Sitespacing", rowSiteSpacing);

                nextLine();
                readStringAssignment("Siteorient", rowSiteOrient);
                nextLine();
                readStringAssignment("Sitesymmetry", rowSiteSymmetry);

                nextLine();
                readIntegerAssignment("SubrowOrigin", rowSubrowOrigin);

                std::string token;
                readString(token);
                if (token != "NumSites" && token != "Numsites")
                        parsingError(
                            "Expecting 'NumSites' or 'Numsites' got '" + token +
                            "'.");

                readColon();
                readInteger(rowNumSites);

                nextLine();
                readKeyword("End");

                dscp.clsRows.push_back(BookshelfRow());
                BookshelfRow &row = dscp.clsRows.back();
                row.clsCoordinate = rowCoordinate;
                row.clsDirection = "Horizontal";
                row.clsHeight = rowHeight;
                row.clsNumSites = rowNumSites;
                row.clsSiteOrientation = rowSiteOrient;
                row.clsSiteSpacing = rowSiteSpacing;
                row.clsSiteSymmetry = rowSiteSymmetry;
                row.clsSiteWidth = rowSiteWidth;
                row.clsSubRowOrigin = rowSubrowOrigin;

                counterRows++;
        }  // end while

        if (counterRows != numRows)
                parsingError("Number of rows dosen't match.");
}  // end method

// -----------------------------------------------------------------------------

std::string BookshelfParser::removePath(const std::string &str) const {
        int i;
        for (i = str.size() - 1; i > 0; i--)
                if (str[i] == '\\' || str[i] == '/') break;
        if (i == 0)
                return str;
        else
                return str.substr(i + 1, str.size() - i);
}  // end method

// ----------------------------------------------------------------------------

std::string BookshelfParser::removeExtension(const std::string &str) const {
        for (int i = str.size() - 1; i > 0; i--)
                if (str[i] == '.') return str.substr(0, i);
        return str;
}  // end method

// ----------------------------------------------------------------------------

std::string BookshelfParser::obtainExtension(const std::string &str) const {
        for (int i = str.size() - 1; i > 0; i--)
                if (str[i] == '.') return str.substr(i + 1, str.size() - i);
        return "";
}  // end method

// ----------------------------------------------------------------------------

std::string BookshelfParser::obtainPath(const std::string &str) const {
        for (int i = str.size() - 1; i > 0; i--)
                if (str[i] == '\\' || str[i] == '/')
                        return str.substr(0, i + 1);
        return "";
}  // end method
