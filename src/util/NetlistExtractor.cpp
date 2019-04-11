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
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   NetlistDecomposer.cpp
 * Author: mpfogaca
 *
 * Created on 31 de Março de 2017, 14:49
 */

#include "NetlistExtractor.h"
#include "phy/PhysicalDesign.h"
#include "phy/util/PhysicalTypes.h"

#include <fstream>
#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <unordered_set>

void NetlistExtractor::start(const Rsyn::Json& params) {
        Rsyn::Session session;

        clsDesign = session.getDesign();
        clsModule = session.getTopModule();
        clsPhysicalDesign = session.getPhysicalDesign();

        if (!params.count("spiceFile")) {
                std::cout << "[ERROR] No spice file specified..\n";
                std::exit(1);
        }  // end if

        clsSpiceFile = params["spiceFile"];

        init();
        extract();
}  // end method

// -----------------------------------------------------------------------------

void NetlistExtractor::init() {
        std::ifstream file(clsSpiceFile);

        if (!file.is_open()) {
                std::cout << "[ERROR] Could not open specified file..\n";
                std::exit(1);
        }  // end if

        std::string line;
        while (file.good()) {
                std::getline(file, line);

                if (!line.size() || line[0] == '*') continue;

                if (line.find(".SUBCKT") == std::string::npos) continue;

                Subckt subckt;
                parseSubcktInterface(line, subckt);

                while (true) {
                        std::getline(file, line);

                        if (!line.size() || line[0] == '*') continue;

                        if (line.find(".ENDS") != std::string::npos) break;

                        Transistor transistor;
                        parseTransistor(line, transistor);
                        subckt.netlist.push_back(transistor);
                }  // end while

                clsEntries[subckt.name] = subckt;
                //		clsEntries[subckt.name + "_LVT"] = subckt;
                //		clsEntries[subckt.name + "_SVT"] = subckt;
                //		clsEntries[subckt.name + "_HVT"] = subckt;
        }  // end while
}  // end method

// -----------------------------------------------------------------------------

void NetlistExtractor::parseSubcktInterface(const std::string input,
                                            Subckt& subckt) {
        std::vector<std::string> parameters;
        boost::algorithm::split(parameters, input,
                                boost::algorithm::is_any_of(" "));

        if (parameters.size() < 3) {
                std::cout
                    << "[ERROR] Subckt without any name or interface...\n";
                std::exit(1);
        }  // end if

        subckt.name = parameters[1];

        for (int i = 2; i < parameters.size(); i++) {
                if (!parameters[i].size()) continue;

                subckt.interface.insert(parameters[i]);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void NetlistExtractor::parseTransistor(const std::string input,
                                       Transistor& transistor) {
        const double dbuFactor =
            0.001 * clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        const double technologyFactor =
            10e5 * clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        const double technologyFactorArea = technologyFactor * technologyFactor;

        std::vector<std::string> parameters;
        boost::algorithm::split(parameters, input,
                                boost::algorithm::is_any_of(" "));

        if (parameters.size() < 8) {
                std::cout << "[WARNING]: Skipping line: " << input << "\n";
                return;
        }  // end if

        transistor.name = parameters[0];
        transistor.drain = parameters[1];
        transistor.gate = parameters[2];
        transistor.source = parameters[3];
        transistor.type = parameters[5];

        if (transistor.type == "NMOS_VTL")
                transistor.bulk = "VSS";
        else if (transistor.type == "PMOS_VTL")
                transistor.bulk = "VDD";

        boost::algorithm::replace_all(parameters[6], "L=", "");
        transistor.l = std::atof(parameters[6].c_str()) * technologyFactor;

        boost::algorithm::replace_all(parameters[7], "W=", "");
        transistor.w = std::atof(parameters[7].c_str()) * technologyFactor;

        boost::algorithm::replace_all(parameters[8], "AD=", "");
        transistor.ad = std::atof(parameters[8].c_str()) * technologyFactorArea;

        boost::algorithm::replace_all(parameters[9], "AS=", "");
        transistor.as = std::atof(parameters[9].c_str()) * technologyFactorArea;

        boost::algorithm::replace_all(parameters[10], "PD=", "");
        transistor.pd = std::atof(parameters[10].c_str()) * technologyFactor;

        boost::algorithm::replace_all(parameters[11], "PS=", "");
        transistor.ps = std::atof(parameters[11].c_str()) * technologyFactor;

        boost::algorithm::replace_all(parameters[12], "$X=", "");
        transistor.x = std::atof(parameters[12].c_str()) * dbuFactor;

        boost::algorithm::replace_all(parameters[13], "$Y=", "");
        transistor.y = std::atof(parameters[13].c_str()) * dbuFactor;

        boost::algorithm::replace_all(parameters[14], "$D=", "");
        if (parameters[14] == "1")
                transistor.d = 1;
        else if (parameters[14] == "0")
                transistor.d = 0;

        //	 DEBUG PRINTS
        //	 std::cout << transistor.name << " ";
        //	 std::cout << transistor.drain << " ";
        //	 std::cout << transistor.gate << " ";
        //	 std::cout << transistor.source << " ";
        //	 std::cout << transistor.bulk << " ";
        //	 std::cout << transistor.type << " ";
        //	 std::cout << "W=" << transistor.w << " ";
        //	 std::cout << "L=" << transistor.l << " ";
        //	 std::cout << "AD=" << transistor.ad << " ";
        //	 std::cout << "AS=" << transistor.as << " ";
        //	 std::cout << "PD=" << transistor.pd << " ";
        //	 std::cout << "PS=" << transistor.ps << " ";
        //	 std::cout << "$X=" << transistor.x << " ";
        //	 std::cout << "$Y=" << transistor.y << " ";
        //	 std::cout << "$D=" << transistor.d << " ";
        //	 std::cout << "\n";
}  // end method
   // end method

// -----------------------------------------------------------------------------

void NetlistExtractor::extract() {
        int numTransistors = 0;
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                if (instance.getType() != Rsyn::CELL) continue;

                const Rsyn::Cell cell = instance.asCell();

                const std::string libCell = cell.getLibraryCellName();

                const auto& entry = clsEntries.find(libCell);

                if (clsEntries.find(libCell) == clsEntries.end()) {
                        std::cout << "[ERROR] Spice of cell " << libCell
                                  << " not found!\n";
                        continue;
                        //			std::exit(1);
                }  // end if

                const Subckt& subckt = entry->second;
                extractCellSubckt(cell, subckt);
                numTransistors += subckt.netlist.size();
        }  // end for

        std::cout << "The layout has " << numTransistors << " transistors.\n";
}  // end method

// -----------------------------------------------------------------------------

void NetlistExtractor::extractCellSubckt(const Rsyn::Cell cell,
                                         const Subckt& subckt) {
        const std::string name = cell.getName();
        const double dbuFactor =
            0.001 * clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        const double technologyFactor =
            10e5 * clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        const double technologyFactorArea = technologyFactor * technologyFactor;

        for (int i = 0; i < subckt.netlist.size(); i++) {
                const Transistor& transistor = subckt.netlist[i];

                Transistor newInstance = transistor;
                newInstance.name = name + "_" + transistor.name;
                newInstance.drain = getNodeName(cell, transistor.drain, subckt);
                newInstance.gate = getNodeName(cell, transistor.gate, subckt);
                newInstance.source =
                    getNodeName(cell, transistor.source, subckt);

                Rsyn::PhysicalCell physicalCell =
                    clsPhysicalDesign.getPhysicalCell(cell);

                if (physicalCell.getOrientation() == Rsyn::ORIENTATION_S ||
                    physicalCell.getOrientation() == Rsyn::ORIENTATION_FS) {
                        newInstance.y = physicalCell.getY() +
                                        physicalCell.getHeight() -
                                        transistor.y - transistor.w;
                } else {
                        newInstance.y = physicalCell.getY() + transistor.y;
                }  // end if

                if (physicalCell.getOrientation() == Rsyn::ORIENTATION_FN ||
                    physicalCell.getOrientation() == Rsyn::ORIENTATION_S) {
                        newInstance.x = physicalCell.getX() +
                                        physicalCell.getWidth() - transistor.x -
                                        transistor.l;
                        newInstance.f = true;
                } else {
                        newInstance.x = physicalCell.getX() + transistor.x;
                }  // end if

                //		newInstance.x = physicalCell.getX() +
                // transistor.x;
                //		newInstance.y = physicalCell.getY() +
                // transistor.y;
                clsExtractedNetlist.push_back(newInstance);

                // Debug...
                std::cout << newInstance.name << " ";
                std::cout << newInstance.drain << " ";
                std::cout << newInstance.gate << " ";
                std::cout << newInstance.source << " ";
                std::cout << newInstance.bulk << " ";
                std::cout << newInstance.type << " ";
                std::cout << "L=" << newInstance.l << " ";
                std::cout << "W=" << newInstance.w << " ";
                std::cout << "AD=" << newInstance.ad << " ";
                std::cout << "AS=" << newInstance.as << " ";
                std::cout << "PD=" << newInstance.pd << " ";
                std::cout << "PS=" << newInstance.ps << " ";
                std::cout << "X=" << newInstance.x << " ";
                std::cout << "Y=" << newInstance.y << " ";
                std::cout << "D=" << newInstance.d << " ";
                std::cout << "F=" << newInstance.f << " ";
                std::cout << "\n";
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

const std::string NetlistExtractor::getNodeName(const Rsyn::Cell cell,
                                                const std::string originalName,
                                                const Subckt& subckt) const {
        if (originalName == "VDD" || originalName == "VSS") {
                return originalName;
        } else if (subckt.interface.find(originalName) !=
                   subckt.interface.end()) {
                const Rsyn::Pin pin = cell.getPinByName(originalName);

                for (Rsyn::Pin sink : pin.getNet().allPins(Rsyn::IN)) {
                        if (sink.isPort()) return sink.getInstanceName();
                }  // end for

                const Rsyn::Pin driver = pin.getNet().getAnyDriver();
                if (driver.isPort())
                        return driver.getInstanceName();
                else
                        return driver.getFullName('_');
        }  // end if

        return cell.getName() + "_" + originalName;
}  // end method
