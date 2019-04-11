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
 * File:   BookshelfMapper.cpp
 * Author: jucemar
 *
 * Created on 2 de Novembro de 2016, 10:20
 */

#include "BookshelfMapper.h"
#include "PhysicalUtil.h"
#include "io/legacy/Legacy.h"

#include <iostream>

// -----------------------------------------------------------------------------

// the order which methods are called is mandatory to keep, due to dependence of
// mapping result between methods.
void BookshelfMapper::mapLefDef(const BookshelfDscp &bookshelf, LefDscp &lef,
                                DefDscp &def) {
        createMetal(lef);
        // createVia(lef);

        mapLefHeader(lef);
        mapDefHeader(bookshelf, def);

        mapLefSites(bookshelf, lef);
        mapDefRows(bookshelf, def);

        updateNets(bookshelf);
        updateNodePins(bookshelf);
        mapLefCells(bookshelf, lef);

        mapDefComponents(bookshelf, def);
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::updateNets(const BookshelfDscp &dscp) {
        nets.reserve(dscp.clsNets.size());
        mapNets.reserve(dscp.clsNets.size());
        for (const BookshelfNet &net : dscp.clsNets) {
                std::string netName;
                if (net.clsHasName) {
                        netName = net.clsName;
                } else {
                        netName = Rsyn::getPhysicalGeneratedNamePrefix() +
                                  "net" + std::to_string(nets.size());
                }  // if-else
                mapNets[netName] = nets.size();
                nets.push_back(Net());
                Net &netPin = nets.back();
                netPin.name = netName;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::updateNodePins(const BookshelfDscp &dscp) {
        macros.resize(dscp.clsNodes.size());

        // defining if a node is a lef's pin or macro
        for (int i = 0; i < dscp.clsNodes.size(); i++) {
                const BookshelfNode &node = dscp.clsNodes[i];
                Macro &macro = macros[i];
                macro.clsName = node.clsName;
        }  // end for

        // adding pin to macros
        pins.reserve(dscp.clsNumPins);
        for (Net &netPin : nets) {
                int index = mapNets[netPin.name];
                const BookshelfNet &net = dscp.clsNets[index];
                netPin.pins.reserve(net.clsPins.size());
                for (const BookshelfPin &pin : net.clsPins) {
                        std::string nodeName = pin.clsNodeName;
                        int index = dscp.clsMapNodes.at(nodeName);
                        Macro &macro = macros[index];

                        int pinIndex = pins.size();
                        pins.push_back(Pin());
                        Pin &macropin = pins.back();
                        macropin.clsName =
                            "pin" + std::to_string(macro.pins.size());
                        macro.pins.push_back(pinIndex);
                        netPin.pins.push_back(pinIndex);
                        Pin &macroPin = pins.back();
                        macroPin.clsNet = net.clsName;
                        macroPin.clsNode = pin.clsNodeName;
                        macroPin.displ = pin.clsDisplacement;
                        if (pin.clsDirection.compare("I") == 0) {
                                macroPin.clsDirection = "INPUT";
                        } else if (pin.clsDirection.compare("O") == 0) {
                                macroPin.clsDirection = "OUTPUT";
                        } else if (pin.clsDirection.compare("B") == 0) {
                                macroPin.clsDirection =
                                    "INPUT";  // TODO -> change to bidirection
                                              // when rsyn has such support
                        } else {
                                std::cout << "ERROR: pin direction "
                                          << pin.clsDirection
                                          << " not recognized!\n";
                        }  // end if-else
                }          // end for
        }                  // end for
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::mapLefHeader(LefDscp &lef) {
        lef.clsMajorVersion = 5;
        lef.clsMinorVersion = 8;
        lef.clsBusBitChars = "[]";
        lef.clsDivideChar = "/";
        lef.clsLefUnitsDscp.clsHasDatabase = true;
        lef.clsLefUnitsDscp.clsDatabase = clsDesignUnits;
        lef.clsManufactGrid = 0.1;
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::mapLefSites(const BookshelfDscp &dscp, LefDscp &lef) {
        // I'm assuming all the rows in bookshelf have the same site. // TODO
        // for sites not equal
        const BookshelfRow &row = dscp.clsRows[0];
        lef.clsLefSiteDscps.push_back(LefSiteDscp());
        LefSiteDscp &site = lef.clsLefSiteDscps.back();
        site.clsName = "bookshelfSite";
        site.clsSize = double2(row.clsSiteWidth, row.clsHeight);
        site.clsHasClass = true;
        site.clsSiteClass = "CORE";
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::mapDefRows(const BookshelfDscp &dscp, DefDscp &def) {
        // mapping bookshelf rows to def rows
        clsDieBounds[UPPER].apply(-std::numeric_limits<double>::max());
        clsDieBounds[LOWER].apply(+std::numeric_limits<double>::max());
        def.clsRows.reserve(dscp.clsRows.size());
        for (const BookshelfRow &row : dscp.clsRows) {
                int rowId = def.clsRows.size();
                def.clsRows.push_back(DefRowDscp());
                DefRowDscp &defRow = def.clsRows.back();
                defRow.clsName = Rsyn::getPhysicalGeneratedNamePrefix() +
                                 "Row" + std::to_string(rowId);
                defRow.clsSite = "bookshelfSite";
                defRow.clsOrigin =
                    DBUxy(row.clsSubRowOrigin, row.clsCoordinate);
                defRow.clsOrigin.scale(clsDesignUnits);
                defRow.clsOrientation = row.clsSiteOrientation;
                defRow.clsNumX = row.clsNumSites;
                defRow.clsNumY = 1;
                defRow.clsStepX = 1;

                double2 upper(
                    upper[X] = defRow.clsOrigin[X] +
                               (row.clsNumSites - 1) * row.clsSiteSpacing *
                                   clsDesignUnits +
                               row.clsSiteWidth * clsDesignUnits,
                    upper[Y] =
                        defRow.clsOrigin[Y] + row.clsHeight * clsDesignUnits);

                clsDieBounds[LOWER][X] = std::min(clsDieBounds[LOWER][X],
                                                  (double)defRow.clsOrigin[X]);
                clsDieBounds[LOWER][Y] = std::min(clsDieBounds[LOWER][Y],
                                                  (double)defRow.clsOrigin[Y]);
                clsDieBounds[UPPER][X] =
                    std::max(clsDieBounds[UPPER][X], upper[X]);
                clsDieBounds[UPPER][Y] =
                    std::max(clsDieBounds[UPPER][Y], upper[Y]);
        }  // end for
        def.clsDieBounds = clsDieBounds.convertToDbu();
}  // end method

// -----------------------------------------------------------------------------

// each node results in one lib cell. TODO, creating lib cell depending of
// bookshelf characteristics.
void BookshelfMapper::mapLefCells(const BookshelfDscp &dscp, LefDscp &lef) {
        const int numNodes = dscp.clsNodes.size();
        clsMapNodeToMacros.reserve(numNodes);
        lef.clsLefMacroDscps.reserve(numNodes);

        for (const BookshelfNode &node : dscp.clsNodes) {
                int index = dscp.clsMapNodes.at(node.clsName);
                Macro &macroNode = macros[index];

                clsNumComponents++;
                clsMapNodeToMacros[node.clsName] = lef.clsLefMacroDscps.size();
                lef.clsLefMacroDscps.push_back(LefMacroDscp());
                LefMacroDscp &macro = lef.clsLefMacroDscps.back();
                macro.clsMacroName = node.clsName;
                if (node.clsIsTerminal) {
                        macro.clsMacroClass = "BLOCK";
                } else {
                        macro.clsMacroClass = "CORE";
                }  // end if-else
                macro.clsSite = "bookshelfSite";
                macro.clsSize = node.clsSize;
                macro.clsSymmetry = "X Y";

                int numPins = macroNode.pins.size();
                macro.clsPins.reserve(numPins);
                double2 center = macro.clsSize;
                center.scale(0.5);  // center of cell
                // adding pins
                for (int index : macroNode.pins) {
                        Pin &macroPin = pins[index];
                        std::string pinName =
                            "pin" + std::to_string(macro.clsPins.size());
                        macro.clsPins.push_back(LefPinDscp());
                        LefPinDscp &lefPin = macro.clsPins.back();
                        lefPin.clsPinName = pinName;
                        double2 displ = center + macroPin.displ;
                        lefPin.clsBounds[LOWER] = displ;
                        lefPin.clsBounds[UPPER] = displ;
                        lefPin.clsPinDirection = macroPin.clsDirection;
                        lefPin.clsHasPort = true;
                        lefPin.clsPorts.push_back(LefPortDscp());
                        LefPortDscp &port = lefPin.clsPorts.back();
                        port.clsLefPortGeoDscp.push_back(LefPortGeometryDscp());
                        LefPortGeometryDscp &geoDscp =
                            port.clsLefPortGeoDscp.back();
                        geoDscp.clsMetalName = "metal1";
                        geoDscp.clsBounds.push_back(lefPin.clsBounds);
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::mapDefHeader(const BookshelfDscp &dscp, DefDscp &def) {
        def.clsVersion = 5.8;
        def.clsDeviderChar = "/";
        def.clsBusBitChars = "[]";
        def.clsDesignName = dscp.clsDesignName;
        def.clsDatabaseUnits = clsDesignUnits;
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::mapDefComponents(const BookshelfDscp &dscp,
                                       DefDscp &def) {
        def.clsComps.reserve(clsNumComponents);
        for (const BookshelfNode &node : dscp.clsNodes) {
                int index = dscp.clsMapNodes.at(node.clsName);
                Macro &macroNode = macros[index];
                def.clsComps.push_back(DefComponentDscp());
                DefComponentDscp &comp = def.clsComps.back();
                comp.clsIsFixed = node.clsIsFixed;
                comp.clsName = node.clsName;
                comp.clsMacroName = node.clsName;
                comp.clsOrientation = node.clsOrientation;
                comp.clsPos = node.clsPos.convertToDbu();
                comp.clsPos.scale(clsDesignUnits);
                comp.clsLocationType = node.clsIsFixed ? "FIXED" : "PLACED";
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::populateRsyn(const BookshelfDscp &dscp, LefDscp &lef,
                                   DefDscp &def, Rsyn::Design design) {
        Rsyn::Module top = design.getTopModule();

        design.updateName(def.clsDesignName);

        // Create libCells
        for (const LefMacroDscp &macro : lef.clsLefMacroDscps) {
                Rsyn::CellDescriptor dscp;
                dscp.setName(macro.clsMacroName);
                for (const LefPinDscp &pin : macro.clsPins) {
                        dscp.addPin(pin.clsPinName,
                                    Legacy::lefPinDirectionFromString(
                                        pin.clsPinDirection));
                }  // end for
                design.createLibraryCell(dscp, true);
        }  // end for

        // Creates ports.
        for (const DefPortDscp &port : def.clsPorts) {
                const Rsyn::Direction direction =
                    (port.clsDirection == "INPUT") ? Rsyn::IN : Rsyn::OUT;
                top.createPort(direction, port.clsName);
        }  // end for

        // Creates cells.
        for (const DefComponentDscp &component : def.clsComps) {
                Rsyn::LibraryCell lcell =
                    design.findLibraryCellByName(component.clsMacroName);

                if (!lcell) {
                        std::string str = "Library cell " +
                                          component.clsMacroName +
                                          " not found\n";
                        exit(1);
                }  // end if
                top.createCell(lcell, component.clsName);
        }  // end for

        // Creates nets and connections.
        for (Net &netPin : nets) {
                int netIndex = mapNets[netPin.name];
                const BookshelfNet &net = dscp.clsNets[netIndex];

                Rsyn::Net rsynNet = top.createNet(netPin.name);

                for (int index : nets[netIndex].pins) {
                        Pin &pin = pins[index];

                        Rsyn::Cell rsynCell =
                            design.findCellByName(pin.clsNode);

                        if (!rsynCell) {
                                std::cout << "[ERROR] Cell '" << pin.clsNode
                                          << "' not found.\n";
                                exit(1);
                        }  // end if

                        Rsyn::Pin rsynPin = rsynCell.getPinByName(pin.clsName);

                        rsynPin.connect(rsynNet);
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void BookshelfMapper::createMetal(LefDscp &lef) {
        // metal 1
        lef.clsLefLayerDscps.push_back(LefLayerDscp());
        LefLayerDscp &layer1 = lef.clsLefLayerDscps.back();
        layer1.clsName = "metal1";
        layer1.clsDirection = "HORIZONTAL";
        layer1.clsType = "ROUTING";
        layer1.clsOffset = 0.1;
        layer1.clsPitch[X] = 0.2;
        layer1.clsPitch[Y] = 0.2;
        layer1.clsSpacingRules.push_back(LefSpacingRuleDscp());
        LefSpacingRuleDscp &spc1 = layer1.clsSpacingRules.back();
        spc1.clsSpacing = 0.15;
        layer1.clsWidth = 0.1;

        // metal 2
        lef.clsLefLayerDscps.push_back(LefLayerDscp());
        LefLayerDscp &layer2 = lef.clsLefLayerDscps.back();
        layer2.clsName = "metal2";
        layer2.clsDirection = "HORIZONTAL";
        layer2.clsType = "ROUTING";
        layer2.clsOffset = 0.2;
        layer2.clsPitch[X] = 0.4;
        layer2.clsPitch[Y] = 0.4;
        layer2.clsSpacingRules.push_back(LefSpacingRuleDscp());
        LefSpacingRuleDscp &spc2 = layer2.clsSpacingRules.back();
        spc2.clsSpacing = 0.30;
        layer2.clsWidth = 0.2;

        // metal 3
        lef.clsLefLayerDscps.push_back(LefLayerDscp());
        LefLayerDscp &layer3 = lef.clsLefLayerDscps.back();
        layer3.clsName = "metal3";
        layer3.clsDirection = "HORIZONTAL";
        layer3.clsType = "ROUTING";
        layer3.clsOffset = 0.3;
        layer3.clsPitch[X] = 0.6;
        layer3.clsPitch[Y] = 0.6;
        layer3.clsSpacingRules.push_back(LefSpacingRuleDscp());
        LefSpacingRuleDscp &spc3 = layer3.clsSpacingRules.back();
        spc3.clsSpacing = 0.45;
        layer3.clsWidth = 0.3;

        // metal 4
        lef.clsLefLayerDscps.push_back(LefLayerDscp());
        LefLayerDscp &layer4 = lef.clsLefLayerDscps.back();
        layer4.clsName = "metal4";
        layer4.clsDirection = "HORIZONTAL";
        layer4.clsType = "ROUTING";
        layer4.clsOffset = 0.4;
        layer4.clsPitch[X] = 0.8;
        layer4.clsPitch[Y] = 0.8;
        layer4.clsSpacingRules.push_back(LefSpacingRuleDscp());
        LefSpacingRuleDscp &spc4 = layer4.clsSpacingRules.back();
        spc4.clsSpacing = 0.6;
        layer4.clsWidth = 0.4;
}  // end method

// -----------------------------------------------------------------------------
