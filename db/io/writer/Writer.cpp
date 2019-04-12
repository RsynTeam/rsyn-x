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

#include "Writer.h"

#include <iostream>
#include <string>

#include "session/Session.h"
#include "io/legacy/Legacy.h"

// Services
#include "phy/PhysicalDesign.h"
#include "tool/timing/Timer.h"
#include "tool/routing/RoutingEstimator.h"

#include "io/parser/lef_def/DEFControlParser.h"
namespace Rsyn {

void Writer::start(const Rsyn::Json &params) {
        Rsyn::Session session;

        clsTimer = session.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);
        clsRoutingEstimator =
            session.getService("rsyn.routingEstimator", Rsyn::SERVICE_OPTIONAL);

        clsDesign = session.getDesign();
        clsLibrary = session.getLibrary();
        clsModule = session.getTopModule();
        clsPhysicalDesign = session.getPhysicalDesign();

        {  // writeDEF
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("writeDEF");
                dscp.setDescription("Write a DEF file with the design.");

                dscp.addPositionalParam(
                    "fileName", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_OPTIONAL, "DEF file name.", "");

                dscp.addPositionalParam("full",
                                        ScriptParsing::PARAM_TYPE_BOOLEAN,
                                        ScriptParsing::PARAM_SPEC_OPTIONAL,
                                        "Write full def file. An 2015 ICCAD "
                                        "contest format is wrote if false.",
                                        "false");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string fileName =
                                command.getParam("fileName");
                            const bool full = command.getParam("full");
                            writeDEF(fileName, full);
                    });  // end command
        }                // end block

        {  // writeVerilog
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("writeVerilog");
                dscp.setDescription("Write a Verilog file with the design.");

                dscp.addPositionalParam("fileName",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_OPTIONAL,
                                        "Verilog file name.", "");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string fileName =
                                command.getParam("fileName");

                            if (fileName != "")
                                    writeVerilog(fileName);
                            else
                                    writeVerilog();
                    });
        }  // end block

        {  // writeSPEF
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("writeSPEF");
                dscp.setDescription("Write a SPEF file with the design.");

                dscp.addPositionalParam(
                    "fileName", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_OPTIONAL, "SPEF file name.", "");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string fileName =
                                command.getParam("fileName");

                            if (!clsRoutingEstimator) {
                                    clsRoutingEstimator = session.getService(
                                        "rsyn.routingEstimator",
                                        Rsyn::SERVICE_MANDATORY);
                            }  // end if

                            if (fileName != "")
                                    writeSPEF(fileName);
                            else
                                    writeSPEF();
                    });
        }  // end block

        {  // writeBookshelf
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("writeBookshelf");
                dscp.setDescription(
                    "Write the Bookshelf files to represent the design.");

                dscp.addPositionalParam(
                    "path", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "The root directory for the files.", ".");

                //		dscp.addNamedParam( "enablePinDisplacement",
                //				ScriptParsing::PARAM_TYPE_BOOLEAN,
                //				ScriptParsing::PARAM_SPEC_OPTIONAL,
                //				"Enable pin displacement in
                //.nets
                // file",
                //				"true");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string path = command.getParam("path");
                            //			const bool enablePinDisplacement
                            //=
                            // command.getParam("enablePinDisplacement");
                            writeBookshelf2(path);
                    });
        }  // end block
}  // end method

// -----------------------------------------------------------------------------

void Writer::stop() {}  // end method

// -----------------------------------------------------------------------------

void Writer::writeDEF(const std::string &filename, const bool full) {
        if (filename != "") {
                writeFullDEF(filename);
        } else {
                writeFullDEF(clsDesign.getName() + ".def");
        }  // end if-else

        /*if (full) {
                if (filename != "") {
                        writeFullDEF(filename);
                } else {
                        writeFullDEF(clsDesign.getName() + "-cada085.def");
                } // end if-else
        } else {
                if (filename != "") {
                        writeICCAD15DEF(filename);
                } else {
                        writeICCAD15DEF(clsDesign.getName() + "-cada085.def");
                } // end if-else
        } // end if-else
         */
}  // end method

// -----------------------------------------------------------------------------

void Writer::writeFullDEF(string filename) {
        DEFControlParser defParser;
        DefDscp def;
        def.clsHasDieBounds = true;
        def.clsDieBounds = clsPhysicalDesign.getPhysicalDie().getBounds();
        def.clsHasDatabaseUnits = true;
        def.clsDatabaseUnits =
            clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        def.clsDesignName = clsDesign.getName();

        int numCells = clsDesign.getNumInstances(Rsyn::CELL);
        def.clsComps.reserve(numCells);
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                if (instance.getType() != Rsyn::CELL) continue;

                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                PhysicalCell ph = clsPhysicalDesign.getPhysicalCell(cell);
                def.clsComps.push_back(DefComponentDscp());
                DefComponentDscp &defComp = def.clsComps.back();
                defComp.clsName = cell.getName();
                defComp.clsMacroName = cell.getLibraryCellName();
                defComp.clsPos = ph.getPosition();
                defComp.clsIsFixed = instance.isFixed();
                defComp.clsOrientation =
                    Rsyn::getPhysicalOrientation(ph.getOrientation());
                defComp.clsIsPlaced = ph.isPlaced();
        }  // end for

        int numNets = clsDesign.getNumNets();
        def.clsNets.reserve(numNets);
        for (Rsyn::Net net : clsModule.allNets()) {
                def.clsNets.push_back(DefNetDscp());
                DefNetDscp &defNet = def.clsNets.back();
                defNet.clsName = net.getName();
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

                Rsyn::PhysicalNet phNet = clsPhysicalDesign.getPhysicalNet(net);
                const PhysicalRouting &phRouting = phNet.getRouting();
                if (!phRouting.isValid()) continue;

                std::vector<DefWireDscp> &wires = defNet.clsWires;
                wires.push_back(DefWireDscp());
                DefWireDscp &wire = wires.back();

                for (const PhysicalRoutingWire &phWire : phRouting.allWires()) {
                        std::vector<DefWireSegmentDscp> &segments =
                            wire.clsWireSegments;
                        segments.push_back(DefWireSegmentDscp());
                        DefWireSegmentDscp &segment = segments.back();
                        segment.clsLayerName = phWire.getLayer().getName();
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

        int numPorts =
            clsModule.getNumPorts(Rsyn::IN) + clsModule.getNumPorts(Rsyn::OUT);
        def.clsPorts.reserve(numPorts);
        for (Rsyn::Port port : clsModule.allPorts()) {
                Rsyn::PhysicalPort phPort =
                    clsPhysicalDesign.getPhysicalPort(port);
                def.clsPorts.push_back(DefPortDscp());
                DefPortDscp &defPort = def.clsPorts.back();
                defPort.clsName = port.getName();
                defPort.clsNetName = port.getName();
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

        int numRows = clsPhysicalDesign.getNumRows();
        def.clsRows.reserve(numRows);
        for (Rsyn::PhysicalRow phRow : clsPhysicalDesign.allPhysicalRows()) {
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

        // write def tracks
        int numTracks = clsPhysicalDesign.getNumPhysicalTracks();
        def.clsTracks.reserve(numTracks);
        for (Rsyn::PhysicalTracks phTrack :
             clsPhysicalDesign.allPhysicalTracks()) {
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

        defParser.writeFullDEF(filename, def);
}  // end method

// -----------------------------------------------------------------------------

void Writer::writeICCAD15DEF(std::string defFile) {
        DEFControlParser defParser;
        std::vector<DefComponentDscp> comps;

        int numCells = clsDesign.getNumInstances(Rsyn::CELL);
        comps.reserve(numCells);
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell

                if (cell.isPort()) continue;

                const PhysicalCell &ph =
                    clsPhysicalDesign.getPhysicalCell(cell);

                comps.push_back(DefComponentDscp());
                DefComponentDscp &defComp = comps.back();
                defComp.clsName = cell.getName();
                defComp.clsMacroName = cell.getLibraryCellName();
                defComp.clsPos = ph.getPosition();
                defComp.clsIsFixed = instance.isFixed();
                defComp.clsIsPlaced = true;
        }  // end for
        defParser.writeDEF(defFile, clsDesign.getName(), comps);
}  // end method

// -----------------------------------------------------------------------------

void Writer::writeVerilog(string filename) {
        Stepwatch watch("Writing Verilog File ...");
        cout << __func__ << "\n";
        cout << "filename: " << filename << "\n";
        ofstream opsFile;
        opsFile.open(filename);
        opsFile << "module " << clsDesign.getName() << " (\n";
        int i = 0;
        for (Rsyn::Port port : clsModule.allPorts(Rsyn::IN)) {
                opsFile << port.getName();
                // if(i < getNumInstances(Rsyn::PORT)(Rsyn::IN) -1)
                opsFile << ",";
                i++;
                opsFile << "\n";
        }  // end for
        for (Rsyn::Port port : clsModule.allPorts(Rsyn::OUT)) {
                opsFile << port.getName();
                if (i < clsModule.getNumPorts(Rsyn::OUT) - 1) opsFile << ",";
                i++;
                opsFile << "\n";
        }  // end for
        opsFile << ");\n";

        opsFile << "\n";
        opsFile << "// Start PIs\n";
        for (Rsyn::Port port : clsModule.allPorts(Rsyn::IN)) {
                opsFile << "input " << port.getName() << ";\n";
        }  // end for
        opsFile << "\n";

        opsFile << "// Start POs\n";
        for (Rsyn::Port port : clsModule.allPorts(Rsyn::OUT)) {
                opsFile << "output " << port.getName() << ";\n";
        }  // end for
        opsFile << "\n";

        opsFile << "// Start wires\n";
        for (Rsyn::Net net : clsModule.allNets()) {
                opsFile << "wire " << net.getName() << ";\n";
        }  // end for
        opsFile << "\n";

        opsFile << "// Start cells\n";
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (cell.isPort()) continue;
                if (cell.getNumPins() < 1) continue;
                opsFile << cell.getLibraryCellName() << " " << cell.getName()
                        << " "
                        << "( ";
                int i = 0;
                for (Rsyn::Pin pin : cell.allPins(Rsyn::IN)) {
                        if (!pin.isConnected()) continue;
                        opsFile << "." << pin.getName() << "("
                                << pin.getNetName() << ")";
                        if (i < cell.getNumPins() - 1) opsFile << ", ";
                        i++;
                }  // end for
                i = 0;
                for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
                        if (!pin.isConnected()) continue;
                        opsFile << "." << pin.getName() << "("
                                << pin.getNetName() << ")";
                        if (i < cell.getNumPins(Rsyn::OUT) - 1) opsFile << ", ";
                        i++;
                }  // end for
                opsFile << " );\n";
        }  // end for
        opsFile << "\n";

        opsFile << "endmodule\n";

        opsFile.close();

        watch.finish();
}  // end method

// -----------------------------------------------------------------------------

void Writer::writeOPS(string filename) {
        // write a empty file for timer operations.
        ofstream opsFile;
        opsFile.open(filename);
        opsFile << flush;
        opsFile.close();
}

// -----------------------------------------------------------------------------

void Writer::writeSPEF(const std::string fileName) {
        Stepwatch writer("Writing SPEF File");
        //	string fileName = clsDesign.getName() + ".spef";
        ofstream spefFile(fileName);
        spefFile.precision(5);
        writeSPEFFile(spefFile, true);
        spefFile.close();
        writer.finish();
}  // end method

// -----------------------------------------------------------------------------

void Writer::writeTimingFile() {
        Stepwatch writer("Writing Timing File");
        string fileName = clsDesign.getName() + ".timing";
        ofstream timingFile(fileName);
        timingFile.precision(5);
        clsTimer->writeTimingFile(timingFile);
        timingFile.close();
        writer.finish();
}  // end method

// -----------------------------------------------------------------------------

void Writer::writeBookshelf(const std::string &path, const bool enablePinDisp) {
        const std::string circuitName = clsDesign.getName();
        const std::string baseName = path + "/" + circuitName;
        const int designUnits =
            clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        const double scaleFactor = 1.0 / designUnits;
        ofstream file;
        std::string gen_name = Rsyn::getPhysicalGeneratedNamePrefix();
        // writing auxFile
        file.open(baseName + ".aux");
        file << "RowBasedPlacement : " << circuitName << ".nodes "
             << circuitName << ".nets " << circuitName << ".wts " << circuitName
             << ".pl " << circuitName << ".scl";
        file.close();

        // writing nodes files
        file.open(baseName + ".nodes");
        file << "UCLA nodes 1.0\n";
        file << "\n";
        file << "NumNodes : " << clsDesign.getNumInstances(Rsyn::CELL) << "\n";
        int numPorts = clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_BLOCK);
        file << "NumTerminals : " << numPorts << "\n";

        for (Rsyn::LibraryCell libCell : clsDesign.allLibraryCells()) {
                Rsyn::PhysicalLibraryCell phLibCell =
                    clsPhysicalDesign.getPhysicalLibraryCell(libCell);
                DBUxy cellSize = phLibCell.getSize();
                double width = cellSize[X] * scaleFactor;
                double height = cellSize[Y] * scaleFactor;
                file << "\t" << libCell.getName() << "\t" << width << "\t"
                     << height;
                if (phLibCell.isMacroBlock()) file << "\tterminal";
                file << "\n";
        }  // end for
        file.close();

        // writing nets
        file.open(baseName + ".nets");
        file << "UCLA nets 1.0\n";
        file << "\n";

        int numPins =
            clsDesign.getNumPins() -
            clsDesign.getNumInstances(
                Rsyn::PORT);  // ports do not have pins in bookshelf file
        file << "NumNets : " << clsDesign.getNumNets() << "\n";
        file << "NumPins : " << numPins << "\n";
        file << std::fixed << std::setprecision(6);
        for (Rsyn::Net net : clsModule.allNets()) {
                std::string netName = net.getName();
                file << "NetDegree : " << net.getNumPins();
                if (netName.find(gen_name) ==
                    std::string::npos)  // do not writing auto generated net
                                        // names
                        file << " " << netName;
                file << "\n";
                for (Rsyn::Pin pin : net.allPins()) {
                        file << "\t" << pin.getInstanceName() << "\t"
                             << Legacy::bookshelfPinDirectionToString(
                                    pin.getDirection());
                        if (enablePinDisp) {
                                Rsyn::PhysicalCell phCell =
                                    clsPhysicalDesign.getPhysicalCell(pin);
                                DBUxy center = phCell.getSize();
                                DBUxy pinDisp =
                                    clsPhysicalDesign.getPinDisplacement(pin);
                                double dx = (pinDisp[X] - (center[X] * 0.5)) *
                                            scaleFactor;
                                double dy = (pinDisp[Y] - (center[Y] * 0.5)) *
                                            scaleFactor;
                                file << " : " << dx << " " << dy;
                        }  // end if
                        file << "\n";
                }  // end for
        }          // end for
        file << std::fixed << std::setprecision(0);
        file.close();

        // writing place file
        file.open(baseName + ".pl");
        file << "UCLA pl 1.0\n";
        file << "\n";

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                Rsyn::PhysicalCell phCell =
                    clsPhysicalDesign.getPhysicalCell(cell);
                DBUxy pos = phCell.getPosition();
                double posx = pos[X] * scaleFactor;
                double posy = pos[Y] * scaleFactor;
                Rsyn::PhysicalOrientation phOrientation =
                    phCell.getOrientation();
                file << cell.getName() << "\t" << posx << "\t" << posy
                     << "\t: " << Rsyn::getPhysicalOrientation(phOrientation);
                if (instance.isFixed()) file << " /FIXED";
                file << "\n";
        }  // end for
        file.close();

        // writing scl file
        file.open(baseName + ".scl");
        file << "UCLA scl 1.0\n";
        file << "\n";

        file << "NumRows : " << clsPhysicalDesign.getNumRows() << "\n";
        file << "\n";
        for (const Rsyn::PhysicalRow phRow :
             clsPhysicalDesign.allPhysicalRows()) {
                double coordinate = phRow.getCoordinate(LOWER, Y) * scaleFactor;
                double height = phRow.getHeight() * scaleFactor;
                double width = phRow.getSiteWidth() * scaleFactor;
                double subRowOrigin =
                    phRow.getCoordinate(LOWER, X) * scaleFactor;
                file << "CoreRow Horizontal\n";
                file << "  Coordinate    : " << coordinate << "\n";
                file << "  Height        : " << height << "\n";
                file << "  Sitewidth     : " << width << "\n";
                file << "  Sitespacing   : " << 1 << "\n";
                Rsyn::PhysicalOrientation phOrientation =
                    phRow.getSiteOrientation();
                file << "  Siteorient    : " << 1
                     << "\n";  // Rsyn::getPhysicalOrientation(phOrientation) <<
                               // "\n";
                Rsyn::PhysicalSymmetry phSymmetry = phRow.getSymmetry();
                file << "  Sitesymmetry  : " << 1
                     << "\n";  // phSymmetry << "\n";
                file << "  SubrowOrigin  : " << subRowOrigin
                     << "\tNumSites : " << phRow.getNumSites(X) << "\n";
                file << "End\n";
        }  // end for
        file.close();

        // writing wts file
        file.open(baseName + ".wts");
        file << "UCLA wts 1.0\n";
        file << "\n";
        file.close();
}  // end method

// -----------------------------------------------------------------------------

void Writer::writeBookshelf2(const std::string &path) {
        boost::filesystem::path dir(path);
        boost::filesystem::create_directory(dir);

        const std::string circuitName = clsDesign.getName();
        const std::string baseName = path + "/" + circuitName;

        std::cout << "Basename: " << baseName << "\n";

        ofstream file;
        std::string gen_name = Rsyn::getPhysicalGeneratedNamePrefix();
        // writing auxFile
        file.open(baseName + ".aux");
        file << "RowBasedPlacement : " << circuitName << ".nodes "
             << circuitName << ".nets " << circuitName << ".wts " << circuitName
             << ".pl " << circuitName << ".scl";
        file.close();

        // writing nodes files
        file.open(baseName + ".nodes");
        file << "UCLA nodes 1.0\n";
        file << "\n";
        file << "NumNodes : " << clsDesign.getNumInstances(Rsyn::CELL) << "\n";
        int numPorts = clsDesign.getNumInstances(Rsyn::PORT);
        file << "NumTerminals : " << numPorts << "\n";

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                Rsyn::PhysicalCell phCell =
                    clsPhysicalDesign.getPhysicalCell(cell);
                DBUxy size = phCell.getSize();
                file << "\t" << cell.getName() << "\t" << (int)size[X] << "\t"
                     << (int)size[Y] << "";

                if (cell.isPort()) file << " terminal";
                file << "\n";
        }  // end for
        file.close();

        // writing nets
        std::ostringstream oss;
        int counterPins = 0;
        int counterNets = 0;
        for (Rsyn::Net net : clsModule.allNets()) {
                std::string netName = net.getName();
                counterNets++;
                oss << "NetDegree : " << net.getNumPins();
                if (netName.find(gen_name) ==
                    std::string::npos)  // do not writing auto generated net
                                        // names
                        oss << " " << netName;
                oss << "\n";
                for (Rsyn::Pin pin : net.allPins()) {
                        oss << "\t" << pin.getInstanceName() << "\t"
                            << Legacy::bookshelfPinDirectionToString(
                                   pin.getDirection())
                            << "\n";
                        counterPins++;
                }  // end for
        }          // end for

        std::cout << "#nets " << counterNets << " " << clsDesign.getNumNets()
                  << "\n";
        std::cout << "#pins " << counterPins << " " << clsDesign.getNumPins()
                  << "\n";

        file.open(baseName + ".nets");
        file << "UCLA nets 1.0\n";
        file << "\n";
        file << "NumNets : " << counterNets << "\n";
        file << "NumPins : " << counterPins
             << "\n";  // Note:  clsDesign.getNumPins() seems bogus :/
        file << "\n";
        file << oss.str();
        file.close();

        // writing place file
        file.open(baseName + ".pl");
        file << "UCLA pl 1.0\n";
        file << "\n";

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                const PhysicalCell &phCell =
                    clsPhysicalDesign.getPhysicalCell(cell);
                DBUxy pos = phCell.getPosition();
                file << cell.getName() << "\t" << (int)pos[X] << "\t"
                     << (int)pos[Y] << "\t: N";
                file << "\n";
        }  // end for
        file.close();

        // writing scl file
        file.open(baseName + ".scl");
        file << "UCLA scl 1.0\n";
        file << "\n";

        file << "NumRows : " << clsPhysicalDesign.getNumRows() << "\n";
        file << "\n";
        for (const Rsyn::PhysicalRow phRow :
             clsPhysicalDesign.allPhysicalRows()) {
                file << "CoreRow Horizontal\n";
                file << "Coordinate    : " << phRow.getCoordinate(LOWER, X)
                     << "\n";
                file << "Height : " << phRow.getHeight() << "\n";
                file << "Sitewidth : " << 1 << "\n";
                file << "Sitespacing : " << 1 << "\n";
                file << "Siteorient : "
                     << "N"
                     << "\n";
                file << "Sitesymmetry : "
                     << "Y"
                     << "\n";
                file << "SubrowOrigin : " << phRow.getCoordinate(LOWER, X)
                     << " NumSites : "
                     << (phRow.getNumSites(X) * phRow.getSiteWidth()) << "\n";
                file << "End\n";
        }  // end for
        file.close();
}  // end method

// -----------------------------------------------------------------------------

void Writer::writePlacedBookshelf(const std::string &path) {
        const int designUnits =
            clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        const double scaleFactor = 1.0 / designUnits;
        std::ofstream file;
        file.open(path);
        file << "UCLA pl 1.0\n";
        file << "# created by UPlace\n";
        file << "\n";

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                Rsyn::PhysicalCell phCell =
                    clsPhysicalDesign.getPhysicalCell(cell);
                DBUxy pos = phCell.getPosition();
                double posx = pos[X] * scaleFactor;
                double posy = pos[Y] * scaleFactor;
                Rsyn::PhysicalOrientation phOrientation =
                    phCell.getOrientation();
                file << cell.getName() << "\t" << posx << "\t" << posy
                     << "\t: " << Rsyn::getPhysicalOrientation(phOrientation);
                if (instance.isFixed()) file << " /FIXED";
                file << "\n";
        }  // end for
        file.close();
}  // end method

// -----------------------------------------------------------------------------

void Writer::writeSPEFFile(ostream &out, bool onlyFixed) {
        std::time_t result = std::time(nullptr);

        /* TODO - Receive the head as Rsyn::Json parameters */
        out << "*SPEF \""
            << "IEEE 1481-1998"
            << "\"\n";
        out << "*DESIGN \"" << clsDesign.getName() << "\"\n";
        out << "*DATE \"" << std::asctime(std::localtime(&result)) << "\"";
        out << "*VENDOR \"ICCAD 2015 Contest Team 085 Solution\"\n";
        out << "*PROGRAM \"UPlacer UFRGS\"\n";
        out << "*VERSION \"0.0\""
            << "\n";
        out << "*DESIGN_FLOW \"PIN_CAP NONE\" \"NAME_SCOPE LOCAL\" "
               "\"MISSING_NETS\"\n";
        out << "*DIVIDER /"
            << "\n";
        out << "*DELIMITER :"
            << "\n";
        out << "*BUS_DELIMITER [ ]"
            << "\n";
        out << "*T_UNIT 1 PS"
            << "\n";  // TODO unit Hard Coded
        out << "*C_UNIT 1 FF"
            << "\n";  // TODO unit Hard Coded
        out << "*R_UNIT 1 KOHM"
            << "\n";  // TODO unit Hard Coded
        out << "*L_UNIT 1 UH"
            << "\n";  // TODO unit Hard Coded
        out << "\n";
        for (const Rsyn::Net net : clsModule.allNets()) {
                if (net.getNumPins() < 2) continue;
                if (onlyFixed) {
                        bool fixedCells = true;
                        for (Rsyn::Pin pin : net.allPins()) {
                                Rsyn::Instance inst = pin.getInstance();
                                bool isFixed = inst.isFixed();
                                if (!isFixed) {
                                        fixedCells = false;
                                        break;
                                }  // end if
                        }          // end for
                        if (!fixedCells) continue;
                }  // end if

                RCTree &tree = clsRoutingEstimator->getRCTree(net);

                out << "*D_NET " << net.getName() << " "
                    << (tree.getTotalWireCap()) << "\n";
                out << "*CONN"
                    << "\n";

                for (Rsyn::Pin pin : net.allPins()) {
                        if (pin.isPort()) {
                                out << "*P ";
                                out << pin.getInstanceName();
                                if (pin.isPort(Rsyn::IN))
                                        out << " I"
                                            << "\n";
                                else
                                        out << " O"
                                            << "\n";
                        } else {
                                out << "*I ";
                                out << pin.getFullName();
                                if (pin.isDriver())
                                        out << " O"
                                            << "\n";
                                else
                                        out << " I"
                                            << "\n";
                        }  // end if
                }          // end for

                if (net == clsTimer->getClockNet()) {
                        out << "*END"
                            << "\n";
                        out << "\n";
                        continue;
                }  // end  if

                auto getNodeName = [&](Rsyn::Pin pin,
                                       const int index) -> std::string {
                        string pinName;
                        if (!pin) {
                                pinName =
                                    net.getName() + ":" + to_string(index);
                        } else {
                                pinName = pin.isPort() ? pin.getInstanceName()
                                                       : pin.getFullName();
                        }  // end else
                        return pinName;
                };  // end method

                out << "*CAP"
                    << "\n";
                std::map<string, int> mapPinNumb;
                for (int i = 0; i < tree.getNumNodes(); i++) {
                        Rsyn::Pin pin = tree.getNodeTag(i).getPin();
                        const std::string pinName = getNodeName(pin, i);
                        mapPinNumb.insert(
                            std::pair<string, int>(pinName, i + 1));
                        out << (i + 1) << " " << pinName << " "
                            << (tree.getNode(i).getWireCap()) << "\n";
                }  // end for

                out << "*RES"
                    << "\n";
                for (int i = 1; i < tree.getNumNodes(); i++) {
                        Rsyn::Pin parent =
                            tree.getNodeTag(tree.getNode(i).propParent)
                                .getPin();
                        Rsyn::Pin pin = tree.getNodeTag(i).getPin();
                        string pinNameParent =
                            getNodeName(parent, tree.getNode(i).propParent);
                        string pinName = getNodeName(pin, i);
                        out << mapPinNumb[pinNameParent] << " " << pinNameParent
                            << " " << pinName << " "
                            << tree.getNode(i).propDrivingResistance << "\n";
                }  // end for
                out << "*END"
                    << "\n";
                out << "\n";
        }  // end for
        out.flush();
}  // end method

}  // end namespace
