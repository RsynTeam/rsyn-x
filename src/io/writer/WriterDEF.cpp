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
 * File:   WriterDEF.cpp
 * Author: jucemar
 *
 * Created on August 27, 2018, 8:53 PM
 */

#include "WriterDEF.h"
#include "io/parser/lef_def/DEFControlParser.h"

namespace Rsyn {

void WriterDEF::start(const Rsyn::Json &params) {
        clsDesign = clsSession.getDesign();
        clsModule = clsDesign.getTopModule();
        clsPhDesign = clsSession.getPhysicalDesign();

        if (!params.empty()) {
                std::string path = getPath();
                path = params.value("path", path);
                setPath(path);

                std::string filename = getFilename();
                filename = params.value("filename", filename);
                setFilename(filename);
        }  // end if

        {  // writeDEF
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("writeDEFFile");
                dscp.setDescription("Write a DEF file.");

                dscp.addPositionalParam(
                    "filename", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_OPTIONAL, "DEF file name.", "");

                dscp.addPositionalParam(
                    "path", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_OPTIONAL, "DEF path name.", "");

                dscp.addPositionalParam(
                    "reset", ScriptParsing::PARAM_TYPE_BOOLEAN,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "Reset all writer DEF options.", "false");

                dscp.addPositionalParam(
                    "full", ScriptParsing::PARAM_TYPE_BOOLEAN,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "Enable to write full DEF file.", "false");

                clsSession.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            //			if (command.hasParam("filename"))
                            //{
                            //				std::string filename2 =
                            // command.getParam("filename");
                            //				setFilename(filename2);
                            //			} // end if
                            //
                            //			if (command.hasParam("path")) {
                            //				std::string path2 =
                            // command.getParam("path");
                            //				setPath(path2);
                            //			} // end if
                            //
                            //			if (command.hasParam("full")) {
                            //				const bool full =
                            // command.getParam("full");
                            //				if (full) {
                            //					enableAll();
                            //				} // end if
                            //			} // end if
                            writeDEF();
                    });  // end command
        }                // end block
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::stop() {}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::writeDEF() {
        DEFControlParser defParser;
        DefDscp def;
        loadDEFHeaderDscp(def);
        if (isPropertyDefinitionsEnabled()) {
                loadDEFPropertyDefinitions(def);
        }  // end if
        if (isDieAreaEnabled()) {
                loadDEFDieArea(def);
        }  // end if
        if (isRowsEnabled()) {
                loadDEFRows(def);
        }  // end if
        if (isTracksEnabled()) {
                loadDEFTracks(def);
        }  // end if
        if (isGCellGridsEnabled()) {
                loadDEFGCellGrid(def);
        }  // end if
        if (isViasEnabled()) {
                loadDEFVias(def);
        }  // end if
        if (isStylesEnabled()) {
                loadDEFStyles(def);
        }  // end if
        if (isNonDefaultRulesEnabled()) {
                loadDEFNonDefaultRules(def);
        }  // end if
        if (isRegionsEnabled()) {
                loadDEFRegions(def);
        }  // end if
        if (isComponentMaskShiftEnabled()) {
                loadDEFComponentMaskShift(def);
        }  // end if
        if (isComponentsEnabled()) {
                loadDEFComponents(def);
        }  // end if
        if (isPinsEnabled()) {
                loadDEFPins(def);
        }  // end if
        if (isPinPropertiesEnabled()) {
                loadDEFPinProperties(def);
        }  // end if
        if (isBlockagesEnabled()) {
                loadDEFBlockages(def);
        }  // end if
        if (isSlotsEnabled()) {
                loadDEFSlots(def);
        }  // end if
        if (isFillsEnabled()) {
                loadDEFFills(def);
        }  // end if
        if (isSpecialNetsEnabled()) {
                loadDEFSpecialNets(def);
        }  // end if
        if (isNetsEnabled()) {
                loadDEFNets(def);
        }  // end if
        if (isScanChainsEnabled()) {
                loadDEFScanChains(def);
        }  // end if
        if (isGroupsEnabled()) {
                loadDEFGroups(def);
        }  // end if

        std::string path = clsPath.empty() ? "./" : clsPath;
        path += isFilenameSet() ? getFilename() : clsDesign.getName();
        defParser.writeFullDEF(path, def);
}  // end method

// -----------------------------------------------------------------------------

// contest mode

void WriterDEF::writeICCAD15() {
        enableICCAD15();
        clsFilename = clsDesign.getName() + "-cad085";
        writeDEF();
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::writeISPD18() {}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::enableAll() {
        setVersion(true);
        setDeviderChar(true);
        setBusBitChar(true);
        setTechnlogy(true);
        setUnits(true);
        setHistory(true);
        setExt(true);
        setPropertyDefinitions(true);
        setDieArea(true);
        setRows(true);
        setTracks(true);
        setGCellGrids(true);
        setVias(true);
        setStyles(true);
        setNonDefaultRules(true);
        setRegions(true);
        setComponentMaskShift(true);
        setComponents(true);
        setUplacedComponents(false);
        setPins(true);
        setPinProperties(true);
        setBlockages(true);
        setSlots(true);
        setFills(true);
        setSpecialNets(true);
        setNets(true);
        setRoutedNets(true);
        setScanChains(true);
        setGroups(true);
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::disableAll() {
        setVersion(false);
        setDeviderChar(false);
        setBusBitChar(false);
        setTechnlogy(false);
        setUnits(false);
        setHistory(false);
        setExt(false);
        setPropertyDefinitions(false);
        setDieArea(false);
        setRows(false);
        setTracks(false);
        setGCellGrids(false);
        setVias(false);
        setStyles(false);
        setNonDefaultRules(false);
        setRegions(false);
        setComponentMaskShift(false);
        setComponents(false);
        setUplacedComponents(false);
        setPins(false);
        setPinProperties(false);
        setBlockages(false);
        setSlots(false);
        setFills(false);
        setSpecialNets(false);
        setNets(false);
        setRoutedNets(false);
        setScanChains(false);
        setGroups(false);
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::enableFloorplan() {}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::enablePlacement() {}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::enableICCAD15() {
        clsFilename.clear();
        disableAll();
        setVersion(true);
        setDeviderChar(true);
        setBusBitChar(true);
        setTechnlogy(true);
        setUnits(true);
        setDieArea(true);
        setRows(true);
        setComponents(true);
        setPins(true);
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::enableISPD18() {}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFHeaderDscp(DefDscp &def) {
        if (isVersion()) {
                def.clsHasVersion = true;
                def.clsVersion = getDefVersion();
        }  // end if

        if (isDeviderChar()) {
                // TODO
        }  // end if

        if (isBusBitChar()) {
                // TODO
        }  // end if

        def.clsDesignName = clsDesign.getName();

        if (isTechnlogy()) {
                // TODO
        }  // end if

        if (isUnits()) {
                def.clsHasDatabaseUnits = true;
                def.clsDatabaseUnits = clsPhDesign.getDatabaseUnits(DESIGN_DBU);
        }  // end if

        if (isHistory()) {
                // TODO
        }  // end if
        if (isExt()) {
                // TODO
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFPropertyDefinitions(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFDieArea(DefDscp &def) {
        def.clsHasDieBounds = true;
        def.clsDieBounds = clsPhDesign.getPhysicalDie().getBounds();
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFRows(DefDscp &def) {
        int numRows = clsPhDesign.getNumRows();
        def.clsRows.reserve(numRows);
        for (Rsyn::PhysicalRow phRow : clsPhDesign.allPhysicalRows()) {
                def.clsRows.push_back(DefRowDscp());
                DefRowDscp &defRow = def.clsRows.back();
                defRow.clsName = phRow.getName();
                defRow.clsSite = phRow.getSiteName();
                defRow.clsOrigin = phRow.getOrigin();
                defRow.clsStepX = phRow.getStep(X);
                defRow.clsStepY =
                    0;  // phRow.getStep(Y); // Assuming all Y steps are 0
                defRow.clsNumX = phRow.getNumSites(X);
                defRow.clsNumY = phRow.getNumSites(Y);
                defRow.clsOrientation =
                    Rsyn::getPhysicalOrientation(phRow.getSiteOrientation());
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFTracks(DefDscp &def) {
        int numTracks = clsPhDesign.getNumPhysicalTracks();
        def.clsTracks.reserve(numTracks);
        for (Rsyn::PhysicalTracks phTrack : clsPhDesign.allPhysicalTracks()) {
                def.clsTracks.push_back(DefTrackDscp());
                DefTrackDscp &defTrack = def.clsTracks.back();
                defTrack.clsDirection =
                    Rsyn::getPhysicalTrackDirectionDEF(phTrack.getDirection());
                defTrack.clsLocation = phTrack.getLocation();
                defTrack.clsSpace = phTrack.getSpace();
                int numLayers = phTrack.getNumberOfLayers();
                defTrack.clsLayers.reserve(numLayers);
                for (Rsyn::PhysicalLayer phLayer : phTrack.allLayers())
                        defTrack.clsLayers.push_back(phLayer.getName());
                defTrack.clsNumTracks = phTrack.getNumberOfTracks();
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFGCellGrid(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFVias(DefDscp &def) {
        std::vector<DefViaDscp> &defVias = def.clsVias;
        defVias.reserve(clsPhDesign.getNumPhysicalVias());
        for (Rsyn::PhysicalVia phVia : clsPhDesign.allPhysicalVias()) {
                if (!phVia.isViaDesign()) {
                        continue;
                }  // end if
                defVias.push_back(DefViaDscp());
                DefViaDscp &dscpVia = defVias.back();
                dscpVia.clsName = phVia.getName();
                if (phVia.isViaRule()) {
                        dscpVia.clsHasViaRule = true;
                        dscpVia.clsViaRuleName = phVia.getViaRule().getName();
                        dscpVia.clsXCutSize = phVia.getCutSize(X);
                        dscpVia.clsYCutSize = phVia.getCutSize(Y);
                        dscpVia.clsBottomLayer =
                            phVia.getBottomLayer().getName();
                        dscpVia.clsCutLayer = phVia.getCutLayer().getName();
                        dscpVia.clsTopLayer = phVia.getTopLayer().getName();
                        dscpVia.clsXCutSpacing = phVia.getSpacing(X);
                        dscpVia.clsYCutSpacing = phVia.getSpacing(Y);
                        dscpVia.clsXBottomEnclosure =
                            phVia.getEnclosure(BOTTOM_VIA_LEVEL, X);
                        dscpVia.clsYBottomEnclosure =
                            phVia.getEnclosure(BOTTOM_VIA_LEVEL, Y);
                        dscpVia.clsXTopEnclosure =
                            phVia.getEnclosure(TOP_VIA_LEVEL, X);
                        dscpVia.clsYTopEnclosure =
                            phVia.getEnclosure(TOP_VIA_LEVEL, Y);
                        if (phVia.hasRowCol()) {
                                dscpVia.clsHasRowCol = true;
                                dscpVia.clsNumCutCols = phVia.getNumCols();
                                dscpVia.clsNumCutRows = phVia.getNumRows();
                        }  // end if
                        if (phVia.hasOrigin()) {
                                dscpVia.clsHasOrigin = true;
                                dscpVia.clsXOffsetOrigin = phVia.getOrigin(X);
                                dscpVia.clsYOffsetOrigin = phVia.getOrigin(Y);
                        }  // end if
                        if (phVia.hasoffset()) {
                                dscpVia.clsHasOffset = true;
                                dscpVia.clsXBottomOffset =
                                    phVia.getOffset(BOTTOM_VIA_LEVEL, X);
                                dscpVia.clsYBottomOffset =
                                    phVia.getOffset(BOTTOM_VIA_LEVEL, Y);
                                dscpVia.clsXTopOffset =
                                    phVia.getOffset(TOP_VIA_LEVEL, X);
                                dscpVia.clsYTopOffset =
                                    phVia.getOffset(TOP_VIA_LEVEL, Y);
                        }  // end if
                } else {
                        std::map<std::string, std::deque<DefViaGeometryDscp>>
                            &mapGeos = dscpVia.clsGeometries;

                        const std::string bottomLayerName =
                            phVia.getBottomLayer().getName();
                        for (Rsyn::PhysicalViaGeometry phGeometry :
                             phVia.allBottomGeometries()) {
                                std::deque<DefViaGeometryDscp> &geos =
                                    mapGeos[bottomLayerName];
                                geos.push_back(DefViaGeometryDscp());
                                DefViaGeometryDscp &geoDscp = geos.back();
                                geoDscp.clsBounds = phGeometry.getBounds();
                                geoDscp.clsHasMask =
                                    phGeometry.getMaskNumber() != -1;
                                geoDscp.clsMask = phGeometry.getMaskNumber();
                                geoDscp.clsIsRect = true;
                        }  // end for

                        const std::string cutLayerName =
                            phVia.getCutLayer().getName();
                        for (Rsyn::PhysicalViaGeometry phGeometry :
                             phVia.allCutGeometries()) {
                                std::deque<DefViaGeometryDscp> &geos =
                                    mapGeos[cutLayerName];
                                geos.push_back(DefViaGeometryDscp());
                                DefViaGeometryDscp &geoDscp = geos.back();
                                geoDscp.clsBounds = phGeometry.getBounds();
                                geoDscp.clsHasMask =
                                    phGeometry.getMaskNumber() != -1;
                                geoDscp.clsMask = phGeometry.getMaskNumber();
                                geoDscp.clsIsRect = true;
                        }  // end for

                        const std::string topLayerName =
                            phVia.getTopLayer().getName();
                        for (Rsyn::PhysicalViaGeometry phGeometry :
                             phVia.allTopGeometries()) {
                                std::deque<DefViaGeometryDscp> &geos =
                                    mapGeos[topLayerName];
                                geos.push_back(DefViaGeometryDscp());
                                DefViaGeometryDscp &geoDscp = geos.back();
                                geoDscp.clsBounds = phGeometry.getBounds();
                                geoDscp.clsHasMask =
                                    phGeometry.getMaskNumber() != -1;
                                geoDscp.clsMask = phGeometry.getMaskNumber();
                                geoDscp.clsIsRect = true;
                        }  // end for
                }          // end if
        }                  // end for
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFStyles(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFNonDefaultRules(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFRegions(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFComponentMaskShift(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFComponents(DefDscp &def) {
        int numCells = clsDesign.getNumInstances(Rsyn::CELL);
        def.clsComps.reserve(numCells);
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                if (instance.getType() != Rsyn::CELL) {
                        continue;
                }  // end if

                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                PhysicalCell ph = clsPhDesign.getPhysicalCell(cell);
                def.clsComps.push_back(DefComponentDscp());
                DefComponentDscp &defComp = def.clsComps.back();
                defComp.clsName = cell.getName();
                defComp.clsMacroName = cell.getLibraryCellName();
                if (isUplacedComponentsEnabled()) {
                        defComp.clsIsPlaced = false;
                } else {
                        defComp.clsPos = ph.getPosition();
                        defComp.clsIsFixed = instance.isFixed();
                        defComp.clsOrientation =
                            Rsyn::getPhysicalOrientation(ph.getOrientation());
                        defComp.clsIsPlaced = ph.isPlaced();
                }  // end if-else
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFPins(DefDscp &def) {
        int numPorts =
            clsModule.getNumPorts(Rsyn::IN) + clsModule.getNumPorts(Rsyn::OUT);
        def.clsPorts.reserve(numPorts);
        for (Rsyn::Port port : clsModule.allPorts()) {
                Rsyn::PhysicalPort phPort = clsPhDesign.getPhysicalPort(port);
                def.clsPorts.push_back(DefPortDscp());
                DefPortDscp &defPort = def.clsPorts.back();
                defPort.clsName = port.getName();
                // Mateus @ 190316 -- bug fix -- get the actual net name...
                defPort.clsNetName = port.getName();
                if (port.getDirection() == Rsyn::IN) {
                        for (Rsyn::Pin pin : port.allPins(Rsyn::OUT)) {
                                Rsyn::Net net = pin.getNet();
                                if (net != nullptr) {
                                        defPort.clsNetName = net.getName();
                                        break;
                                }
                        }
                } else if (port.getDirection() == Rsyn::OUT) {
                        for (Rsyn::Pin pin : port.allPins(Rsyn::IN)) {
                                Rsyn::Net net = pin.getNet();
                                if (net != nullptr) {
                                        defPort.clsNetName = net.getName();
                                        break;
                                }
                        }
                }
                // --
                if (port.getDirection() == Rsyn::IN)
                        defPort.clsDirection = "INPUT";
                else if (port.getDirection() == Rsyn::OUT)
                        defPort.clsDirection = "OUTPUT";

                defPort.clsLocationType = "FIXED";
                defPort.clsOrientation =
                    Rsyn::getPhysicalOrientation(phPort.getOrientation());
                defPort.clsLayerName = phPort.getLayer().getName();
                defPort.clsLayerBounds = phPort.getBounds();
                defPort.clsPos = phPort.getPosition();

        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFPinProperties(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFBlockages(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFSlots(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFFills(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFSpecialNets(DefDscp &def) {
        for (Rsyn::Net net : clsModule.allNets()) {
                if (net.getUse() != Rsyn::GROUND &&
                    net.getUse() != Rsyn::POWER) {
                        continue;
                }  // end if

                def.clsSpecialNets.push_back(DefSpecialNetDscp());
                DefSpecialNetDscp &defNet = def.clsSpecialNets.back();
                defNet.clsName = net.getName();

                switch (net.getUse()) {
                        case Rsyn::ANALOG:
                                defNet.clsUse = "ANALOG";
                                break;
                        case Rsyn::CLOCK:
                                defNet.clsUse = "CLOCK";
                                break;
                        case Rsyn::GROUND:
                                defNet.clsUse = "GROUND";
                                break;
                        case Rsyn::POWER:
                                defNet.clsUse = "POWER";
                                break;
                        case Rsyn::RESET:
                                defNet.clsUse = "RESET";
                                break;
                        case Rsyn::SCAN:
                                defNet.clsUse = "SCAN";
                                break;
                        case Rsyn::SIGNAL:
                                defNet.clsUse = "SIGNAL";
                                break;
                        case Rsyn::TIEOFF:
                                defNet.clsUse = "TIEOFF";
                                break;
                        default:
                                defNet.clsUse = INVALID_DEF_NAME;
                }  // end switch

                defNet.clsConnections.reserve(net.getNumPins());
                for (Rsyn::Pin pin : net.allPins()) {
                        if (!pin.isPort()) continue;
                        defNet.clsConnections.push_back(DefNetConnection());
                        DefNetConnection &netConnection =
                            defNet.clsConnections.back();
                        netConnection.clsComponentName = "PIN";
                        netConnection.clsPinName = pin.getInstanceName();
                }  // end for
                for (Rsyn::Pin pin : net.allPins()) {
                        if (pin.isPort()) continue;
                        defNet.clsConnections.push_back(DefNetConnection());
                        DefNetConnection &netConnection =
                            defNet.clsConnections.back();
                        netConnection.clsComponentName = pin.getInstanceName();
                        netConnection.clsPinName = pin.getName();
                }  // end for

                if (!isRoutedNetsEnabled()) {
                        continue;
                }  // end if

                Rsyn::PhysicalNet phNet = clsPhDesign.getPhysicalNet(net);
                const PhysicalRouting &phRouting = phNet.getRouting();
                if (!phRouting.isValid()) {
                        continue;
                }  // end if

                std::vector<DefWireDscp> &wires = defNet.clsWires;
                wires.push_back(DefWireDscp());
                DefWireDscp &wire = wires.back();

                for (const PhysicalRoutingWire &phWire : phRouting.allWires()) {
                        std::vector<DefWireSegmentDscp> &segments =
                            wire.clsWireSegments;
                        segments.push_back(DefWireSegmentDscp());
                        DefWireSegmentDscp &segment = segments.back();
                        segment.clsLayerName = phWire.getLayer().getName();
                        segment.clsRoutedWidth = phWire.getWidth();
                        std::vector<DefRoutingPointDscp> &points =
                            segment.clsRoutingPoints;
                        points.reserve(phWire.getNumPoints());
                        for (const DBUxy point : phWire.allPoints()) {
                                points.push_back(DefRoutingPointDscp());
                                DefRoutingPointDscp &routing = points.back();
                                routing.clsPos = point;
                        }  // end for
                        if (phWire.hasNonDefaultSourceExtension()) {
                                DefRoutingPointDscp &routing = points.front();
                                DBUxy ext = phWire.getExtendedSourcePosition();
                                ext -= routing.clsPos;
                                routing.clsExtension = ext[X] ? ext[X] : ext[Y];
                                routing.clsHasExtension = true;
                        }  // end if

                        if (phWire.hasNonDefaultTargetExtension()) {
                                DefRoutingPointDscp &routing = points.back();
                                DBUxy ext = phWire.getExtendedTargetPosition();
                                ext -= routing.clsPos;
                                routing.clsExtension = ext[X] ? ext[X] : ext[Y];
                                routing.clsHasExtension = true;
                        }  // end if
                }          // end for

                for (const PhysicalRoutingVia &phVia : phRouting.allVias()) {
                        if (!phVia.isValid()) continue;

                        std::vector<DefWireSegmentDscp> &segments =
                            wire.clsWireSegments;
                        segments.push_back(DefWireSegmentDscp());
                        DefWireSegmentDscp &segment = segments.back();
                        segment.clsLayerName = phVia.getTopLayer().getName();
                        std::vector<DefRoutingPointDscp> &points =
                            segment.clsRoutingPoints;
                        points.push_back(DefRoutingPointDscp());
                        DefRoutingPointDscp &routing = points.back();
                        routing.clsPos = phVia.getPosition();
                        routing.clsHasVia = true;
                        routing.clsViaName = phVia.getVia().getName();
                }  // end for

                for (const PhysicalRoutingRect &rect : phRouting.allRects()) {
                        std::vector<DefWireSegmentDscp> &segments =
                            wire.clsWireSegments;
                        segments.push_back(DefWireSegmentDscp());
                        DefWireSegmentDscp &segment = segments.back();
                        segment.clsLayerName = rect.getLayer().getName();
                        segment.clsNew = true;
                        segment.clsRoutingPoints.push_back(
                            DefRoutingPointDscp());
                        DefRoutingPointDscp &point =
                            segment.clsRoutingPoints.back();
                        point.clsHasRectangle = true;
                        const Bounds &bds = rect.getRect();
                        point.clsPos = bds[LOWER];
                        point.clsRect[UPPER] = bds[UPPER] - point.clsPos;
                }  // end for

        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFNets(DefDscp &def) {
        int numNets = clsDesign.getNumNets();
        def.clsNets.reserve(numNets);
        for (Rsyn::Net net : clsModule.allNets()) {
                if (net.getUse() == Rsyn::GROUND ||
                    net.getUse() == Rsyn::POWER) {
                        continue;
                }  // end if

                def.clsNets.push_back(DefNetDscp());
                DefNetDscp &defNet = def.clsNets.back();
                defNet.clsName = net.getName();

                switch (net.getUse()) {
                        case Rsyn::ANALOG:
                                defNet.clsUse = "ANALOG";
                                break;
                        case Rsyn::CLOCK:
                                defNet.clsUse = "CLOCK";
                                break;
                        case Rsyn::GROUND:
                                defNet.clsUse = "GROUND";
                                break;
                        case Rsyn::POWER:
                                defNet.clsUse = "POWER";
                                break;
                        case Rsyn::RESET:
                                defNet.clsUse = "RESET";
                                break;
                        case Rsyn::SCAN:
                                defNet.clsUse = "SCAN";
                                break;
                        case Rsyn::SIGNAL:
                                defNet.clsUse = "SIGNAL";
                                break;
                        case Rsyn::TIEOFF:
                                defNet.clsUse = "TIEOFF";
                                break;
                        default:
                                defNet.clsUse = INVALID_DEF_NAME;
                }  // end switch

                defNet.clsConnections.reserve(net.getNumPins());
                for (Rsyn::Pin pin : net.allPins()) {
                        if (!pin.isPort()) continue;
                        defNet.clsConnections.push_back(DefNetConnection());
                        DefNetConnection &netConnection =
                            defNet.clsConnections.back();
                        netConnection.clsComponentName = "PIN";
                        netConnection.clsPinName = pin.getInstanceName();
                }  // end for
                for (Rsyn::Pin pin : net.allPins()) {
                        if (pin.isPort()) continue;
                        defNet.clsConnections.push_back(DefNetConnection());
                        DefNetConnection &netConnection =
                            defNet.clsConnections.back();
                        netConnection.clsComponentName = pin.getInstanceName();
                        netConnection.clsPinName = pin.getName();
                }  // end for

                if (!isRoutedNetsEnabled()) {
                        continue;
                }  // end if

                Rsyn::PhysicalNet phNet = clsPhDesign.getPhysicalNet(net);
                const PhysicalRouting &phRouting = phNet.getRouting();
                if (!phRouting.isValid()) {
                        continue;
                }  // end if

                std::vector<DefWireDscp> &wires = defNet.clsWires;
                wires.push_back(DefWireDscp());
                DefWireDscp &wire = wires.back();

                for (const PhysicalRoutingWire &phWire : phRouting.allWires()) {
                        std::vector<DefWireSegmentDscp> &segments =
                            wire.clsWireSegments;
                        segments.push_back(DefWireSegmentDscp());
                        DefWireSegmentDscp &segment = segments.back();
                        segment.clsLayerName = phWire.getLayer().getName();
                        segment.clsRoutedWidth = phWire.getWidth();
                        std::vector<DefRoutingPointDscp> &points =
                            segment.clsRoutingPoints;
                        points.reserve(phWire.getNumPoints());
                        for (const DBUxy point : phWire.allPoints()) {
                                points.push_back(DefRoutingPointDscp());
                                DefRoutingPointDscp &routing = points.back();
                                routing.clsPos = point;
                        }  // end for
                        if (phWire.hasNonDefaultSourceExtension()) {
                                DefRoutingPointDscp &routing = points.front();
                                DBUxy ext = phWire.getExtendedSourcePosition();
                                ext -= routing.clsPos;
                                routing.clsExtension = ext[X] ? ext[X] : ext[Y];
                                routing.clsHasExtension = true;
                        }  // end if

                        if (phWire.hasNonDefaultTargetExtension()) {
                                DefRoutingPointDscp &routing = points.back();
                                DBUxy ext = phWire.getExtendedTargetPosition();
                                ext -= routing.clsPos;
                                routing.clsExtension = ext[X] ? ext[X] : ext[Y];
                                routing.clsHasExtension = true;
                        }  // end if
                }          // end for

                for (const PhysicalRoutingVia &phVia : phRouting.allVias()) {
                        if (!phVia.isValid()) continue;

                        std::vector<DefWireSegmentDscp> &segments =
                            wire.clsWireSegments;
                        segments.push_back(DefWireSegmentDscp());
                        DefWireSegmentDscp &segment = segments.back();
                        segment.clsLayerName = phVia.getTopLayer().getName();
                        std::vector<DefRoutingPointDscp> &points =
                            segment.clsRoutingPoints;
                        points.push_back(DefRoutingPointDscp());
                        DefRoutingPointDscp &routing = points.back();
                        routing.clsPos = phVia.getPosition();
                        routing.clsHasVia = true;
                        routing.clsViaName = phVia.getVia().getName();
                }  // end for

                for (const PhysicalRoutingRect &rect : phRouting.allRects()) {
                        std::vector<DefWireSegmentDscp> &segments =
                            wire.clsWireSegments;
                        segments.push_back(DefWireSegmentDscp());
                        DefWireSegmentDscp &segment = segments.back();
                        segment.clsLayerName = rect.getLayer().getName();
                        segment.clsNew = true;
                        segment.clsRoutingPoints.push_back(
                            DefRoutingPointDscp());
                        DefRoutingPointDscp &point =
                            segment.clsRoutingPoints.back();
                        point.clsHasRectangle = true;
                        const Bounds &bds = rect.getRect();
                        point.clsPos = bds[LOWER];
                        point.clsRect[UPPER] = bds[UPPER] - point.clsPos;
                }  // end for

        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFScanChains(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

void WriterDEF::loadDEFGroups(DefDscp &def) {
        // TODO
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
