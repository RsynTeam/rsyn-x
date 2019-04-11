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

#include <thread>

#include "Report.h"

#include "session/Session.h"

// Services
#include "phy/PhysicalDesign.h"
#include "tool/timing/Timer.h"
#include "tool/library/LibraryCharacterizer.h"
#include "tool/routing/RoutingEstimator.h"
#include "util/StreamStateSaver.h"

namespace Rsyn {

void Report::start(const Rsyn::Json &params) {
        Rsyn::Session session;

        clsTimer = session.getService("rsyn.timer");
        clsLibraryCharacterizer =
            session.getService("rsyn.libraryCharacterizer");
        clsRoutingEstimator = session.getService("rsyn.routingEstimator");

        if (session.isServiceRunning("rsyn.libraryCharacterizer")) {
                clsLibraryCharacterizer =
                    session.getService("rsyn.libraryCharacterizer");
        }  // end if

        if (session.isServiceRunning("rsyn.routingEstimator")) {
                clsRoutingEstimator =
                    session.getService("rsyn.routingEstimator");
        }  // end if

        clsDesign = session.getDesign();
        clsModule = session.getTopModule();
        clsPhysicalDesign = session.getPhysicalDesign();

        ////////////////////////////////////////////////////////////////////////////
        // Register commands
        ////////////////////////////////////////////////////////////////////////////

        {  // reportCell
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportCell");
                dscp.setDescription("Report informations about a given cell.");

                dscp.addPositionalParam("cellName",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Name of the target cell");

                dscp.addNamedParam("early", ScriptParsing::PARAM_TYPE_BOOLEAN,
                                   ScriptParsing::PARAM_SPEC_OPTIONAL,
                                   "Enable/Disable early timing information",
                                   "false");

                dscp.addNamedParam("late", ScriptParsing::PARAM_TYPE_BOOLEAN,
                                   ScriptParsing::PARAM_SPEC_OPTIONAL,
                                   "Enable/Disable late timing information",
                                   "false");

                clsSession.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string cellName =
                                command.getParam("cellName");
                            const bool early = command.getParam("early");
                            const bool late = command.getParam("late");

                            Rsyn::Instance cell =
                                clsDesign.findCellByName(cellName);

                            if (!cell || cell.getType() != Rsyn::CELL) {
                                    std::cout << "Cell " << cellName
                                              << " not found.\n";
                                    return;
                            }  // end if

                            reportCell(cell.asCell(), late, early);
                    });
        }  // end block

        {  // reportNet
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportNet");
                dscp.setDescription("Report informations about a given net.");

                dscp.addPositionalParam("netName",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Name of the target net");

                dscp.addNamedParam("early", ScriptParsing::PARAM_TYPE_BOOLEAN,
                                   ScriptParsing::PARAM_SPEC_OPTIONAL,
                                   "Enable/Disable early timing information",
                                   "false");

                dscp.addNamedParam("late", ScriptParsing::PARAM_TYPE_BOOLEAN,
                                   ScriptParsing::PARAM_SPEC_OPTIONAL,
                                   "Enable/Disable late timing information",
                                   "false");

                clsSession.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string netName =
                                command.getParam("netName");
                            const bool early = command.getParam("early");
                            const bool late = command.getParam("late");

                            Rsyn::Net net = clsDesign.findNetByName(netName);

                            if (!net) {
                                    std::cout << "Net " << netName
                                              << " not found.\n";
                                    return;
                            }  // end if

                            reportNet(net, late, early);
                    });
        }  // end block

        {  // reportPin
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportPin");
                dscp.setDescription("Report informations about a given pin.");

                dscp.addPositionalParam("pinName",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Name of the target pin");

                dscp.addNamedParam(
                    "separator", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "Separator between instance and pin names (E.g. g101:a)",
                    ":");

                dscp.addNamedParam("early", ScriptParsing::PARAM_TYPE_BOOLEAN,
                                   ScriptParsing::PARAM_SPEC_OPTIONAL,
                                   "Enable/Disable early timing information",
                                   "false");

                dscp.addNamedParam("late", ScriptParsing::PARAM_TYPE_BOOLEAN,
                                   ScriptParsing::PARAM_SPEC_OPTIONAL,
                                   "Enable/Disable late timing information",
                                   "false");

                clsSession.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string pinName =
                                command.getParam("pinName");
                            const std::string separator =
                                command.getParam("separator");
                            const bool early = command.getParam("early");
                            const bool late = command.getParam("late");

                            Rsyn::Pin pin =
                                clsDesign.findPinByName(pinName, separator[0]);

                            if (!pin) {
                                    std::cout << "Pin " << pinName
                                              << " not found.\n";
                                    return;
                            }  // end if

                            reportPin(pin, late, early);
                    });
        }  // end block

        {  // reportTree
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportTree");
                dscp.setDescription(
                    "Report informations about the routing tree of a given "
                    "net.");

                dscp.addPositionalParam("netName",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Name of the target net");

                clsSession.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string netName =
                                command.getParam("netName");

                            Rsyn::Net net = clsDesign.findNetByName(netName);

                            if (!net) {
                                    std::cout << "Net " << netName
                                              << " not found.\n";
                                    return;
                            }  // end if

                            reportTree(net);
                    });
        }  // end block

        {  // reportCriticalPath
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportCriticalPath");
                dscp.setDescription("Report critical path.");

                clsSession.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            if (clsTimer) {
                                    clsTimer->reportCriticalPath(Rsyn::LATE,
                                                                 std::cout);
                            }  // end if
                    });
        }  // end block

        {  // reportLogicDesign
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportLogicDesign");
                dscp.setDescription("Report logic design.");

                clsSession.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            reportLogicDesign();
                    });
        }  // end block

        {  // reportPhysicalDesign
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportPhysicalDesign");
                dscp.setDescription("Report physical design.");

                clsSession.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            reportPhysicalDesign();
                    });
        }  // end block

}  // end method

// -----------------------------------------------------------------------------

void Report::stop() {}  // end method

// -----------------------------------------------------------------------------

void Report::reportPin_Header() {
        std::cout << std::left << std::setw(3) << "Pin"
                  << " " << std::setw(5) << "Dir."
                  << " " << std::right << std::setw(5) << "Crit."
                  << " " << std::setw(7) << "Cap"
                  << " " << std::setw(9) << "Load"
                  << " " << std::setw(7) << "Pin(%)"
                  << " " << std::setw(7) << "Wire(%)"
                  << " " << std::setw(9) << "Slack(F)"
                  << " " << std::setw(9) << "Slack(R)"
                  << " " << std::setw(9) << "At(F)"
                  << " " << std::setw(9) << "At(R)"
                  << " " << std::setw(9) << "Rat(F)"
                  << " " << std::setw(9) << "Rat(R)"
                  << " " << std::setw(9) << "Slew(F)"
                  << " " << std::setw(9) << "Slew(R)"
                  << "\n";

}  // end method

// -----------------------------------------------------------------------------

void Report::reportPin_Data(Rsyn::Pin pin, const TimingMode mode) {
        Rsyn::Net net = pin.isDriver() ? pin.getNet() : nullptr;

        std::cout << std::left;
        std::cout << std::setw(3) << pin.getName() << " ";
        std::cout << std::setw(5)
                  << Rsyn::Global::getDirectionShortName(pin.getDirection())
                  << " ";
        std::cout << std::right;
        std::cout << std::setw(5) << clsTimer->getPinCriticality(pin, mode)
                  << " ";
        std::cout << std::setw(7) << clsTimer->getPinInputCapacitance(pin)
                  << " ";

        if (net && pin.isDriver()) {
                const double totalLoad = clsTimer->getNetLoad(net).getMax();
                const double pinLoad =
                    clsRoutingEstimator->getRCTree(net).getLoadCap().getMax();
                const double wireLoad =
                    clsRoutingEstimator->getRCTree(net).getTotalWireCap();

                const double pinRatio = 100 * (pinLoad / totalLoad);
                const double wireRatio = 100 * (wireLoad / totalLoad);

                std::cout << std::setw(9) << totalLoad << " ";
                std::cout << std::setw(7) << pinRatio << " ";
                std::cout << std::setw(7) << wireRatio << " ";
        } else {
                std::cout << std::setw(9) << "-"
                          << " ";
                std::cout << std::setw(7) << "-"
                          << " ";
                std::cout << std::setw(7) << "-"
                          << " ";
        }  // end else

        std::cout << std::setw(9) << clsTimer->getPinSlack(pin, mode, FALL)
                  << " ";
        std::cout << std::setw(9) << clsTimer->getPinSlack(pin, mode, RISE)
                  << " ";
        std::cout << std::setw(9)
                  << clsTimer->getPinArrivalTime(pin, mode, FALL) << " ";
        std::cout << std::setw(9)
                  << clsTimer->getPinArrivalTime(pin, mode, RISE) << " ";
        std::cout << std::setw(9)
                  << clsTimer->getPinRequiredTime(pin, mode, FALL) << " ";
        std::cout << std::setw(9)
                  << clsTimer->getPinRequiredTime(pin, mode, RISE) << " ";
        std::cout << std::setw(9) << clsTimer->getPinSlew(pin, mode, FALL)
                  << " ";
        std::cout << std::setw(9) << clsTimer->getPinSlew(pin, mode, RISE)
                  << "\n";
}  // end method

// -----------------------------------------------------------------------------

void Report::reportArc_Header() {
        std::cout << std::left << std::setw(10) << "Arc"
                  << " " << std::right << std::setw(9) << "Delay(F)"
                  << " " << std::setw(9) << "Delay(R)"
                  << " " << std::setw(9) << "Slew(F)"
                  << " " << std::setw(9) << "Slew(R)"
                  << " " << std::setw(9) << "Rdrive(F)"
                  << " " << std::setw(9) << "Rdrive(R)"
                  << "\n";
}  // end method

// -----------------------------------------------------------------------------

void Report::reportArc_Data(Rsyn::Arc arc, const TimingMode mode) {
        std::cout << std::left << std::setw(10) << arc.getName() << " "
                  << std::right << std::setw(9)
                  << clsTimer->getArcDelay(arc, mode, FALL) << " "
                  << std::setw(9) << clsTimer->getArcDelay(arc, mode, RISE)
                  << " " << std::setw(9)
                  << clsTimer->getArcOutputSlew(arc, mode, FALL) << " "
                  << std::setw(9) << clsTimer->getArcOutputSlew(arc, mode, RISE)
                  << " ";

        if (arc.getLibraryArc()) {
                // Cell arc.
                std::cout << std::setw(9)
                          << clsLibraryCharacterizer->getDriverResistance(
                                 arc, mode, FALL)
                          << " " << std::setw(9)
                          << clsLibraryCharacterizer->getDriverResistance(
                                 arc, mode, RISE)
                          << "\n";
        } else {
                // Net arc.
                std::cout << std::setw(9) << "N/A"
                          << " " << std::setw(9) << "N/A"
                          << "\n";
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void Report::reportPin_TimingInformation(Rsyn::Pin pin, const TimingMode mode) {
        std::cout << "\n";

        if (mode == LATE) {
                std::cout << "Timing Information (LATE)\n";
                std::cout << "-------------------------\n";
        } else {
                std::cout << "Timing Information (EARLY)\n";
                std::cout << "--------------------------\n";
        }  // end else

        // General
        std::cout << "Criticality : " << clsTimer->getPinCriticality(pin, mode)
                  << "\n";
        std::cout << "Centrality  : " << clsTimer->getPinCentrality(pin, mode)
                  << "\n";
        std::cout << "Relativity  : " << clsTimer->getPinRelativity(pin, mode)
                  << "\n";

        // Pins
        std::cout << "\n";
        reportPin_Header();
        reportPin_Data(pin, mode);

        // Arcs
        std::cout << "\n";
        std::cout << "Incoming Arcs:\n";

        reportArc_Header();
        for (Rsyn::Arc arc : pin.allIncomingArcs()) {
                reportArc_Data(arc, mode);
        }  // end for

        std::cout << "\n";
        std::cout << "Outgoing Arcs:\n";
        reportArc_Header();
        for (Rsyn::Arc arc : pin.allOutgoingArcs()) {
                reportArc_Data(arc, mode);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Report::reportCell_TimingInformation(Rsyn::Cell cell,
                                          const TimingMode mode) {
        std::cout << "\n";

        if (mode == LATE) {
                std::cout << "Timing Information (LATE)\n";
                std::cout << "-------------------------\n";
        } else {
                std::cout << "Timing Information (EARLY)\n";
                std::cout << "--------------------------\n";
        }  // end else

        // General
        std::cout << "Criticality : "
                  << clsTimer->getCellCriticality(cell, mode) << "\n";
        std::cout << "Centrality  : " << clsTimer->getCellCentrality(cell, mode)
                  << "\n";
        std::cout << "Relativity  : " << clsTimer->getCellRelativity(cell, mode)
                  << "\n";

        // Pins
        std::cout << "\n";
        reportPin_Header();

        for (Rsyn::Pin pin : cell.allPins()) {
                reportPin_Data(pin, mode);
        }  // end for

        // Arcs
        std::cout << "\n";
        reportArc_Header();

        for (Rsyn::Arc arc : cell.allArcs()) {
                reportArc_Data(arc, mode);
        }  // end for

}  // end method

// -----------------------------------------------------------------------------

void Report::reportNet_TimingInformation(Rsyn::Net net, const TimingMode mode) {
        std::cout << "\n";

        if (mode == LATE) {
                std::cout << "Timing Information (LATE)\n";
                std::cout << "-------------------------\n";
        } else {
                std::cout << "Timing Information (EARLY)\n";
                std::cout << "--------------------------\n";
        }  // end else

        // General
        std::cout << "Criticality : " << clsTimer->getNetCriticality(net, mode)
                  << "\n";
        std::cout << "Centrality  : " << clsTimer->getNetCentrality(net, mode)
                  << "\n";
        std::cout << "Relativity  : " << clsTimer->getNetRelativity(net, mode)
                  << "\n";
        std::cout << "Wirelength  : "
                  << clsRoutingEstimator->getNetWirelength(net) << "\n";

        // Pins
        std::cout << "\n";
        reportPin_Header();

        for (Rsyn::Pin pin : net.allPins()) {
                reportPin_Data(pin, mode);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

// Design

void Report::reportLogicDesign() {
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Report Logic Design"
                  << "\n";
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Name             : " << clsDesign.getName() << "\n";
        std::cout << "#Cells           : "
                  << clsDesign.getNumInstances(Rsyn::CELL) << "\n";
        std::cout << "#Nets            : " << clsDesign.getNumNets() << "\n";
        std::cout << "#IOs             : "
                  << clsDesign.getNumInstances(Rsyn::PORT) << "\n";
        std::cout << "#Modules         : "
                  << clsDesign.getNumInstances(Rsyn::MODULE) << "\n";
}  // end method

// -----------------------------------------------------------------------------

void Report::reportPhysicalDesign() {
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Report Physical Design"
                  << "\n";
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Name             : " << clsDesign.getName() << "\n";
        std::cout << "#Fixed           : "
                  << clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_FIXED)
                  << "\n";
        std::cout << "#Movables        : "
                  << clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE)
                  << "\n";
        std::cout << "#Blocks          : "
                  << clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_BLOCK)
                  << "\n";
        std::cout << "Design Area (um2): "
                  << clsPhysicalDesign.getPhysicalModule(clsModule).getArea() /
                         std::pow(
                             clsPhysicalDesign.getDatabaseUnits(DESIGN_DBU), 2)
                  << "\n";
        std::cout << "Fixed Area (um2) : "
                  << clsPhysicalDesign.getArea(Rsyn::PHYSICAL_FIXED) /
                         std::pow(
                             clsPhysicalDesign.getDatabaseUnits(DESIGN_DBU), 2)
                  << "\n";
        std::cout << "Cell Area (mm2)  : "
                  << clsPhysicalDesign.getArea(Rsyn::PHYSICAL_MOVABLE) /
                         std::pow(
                             clsPhysicalDesign.getDatabaseUnits(DESIGN_DBU), 2)
                  << "\n";
}  // end method

// -----------------------------------------------------------------------------

void Report::reportPin(Rsyn::Pin pin, const bool late, const bool early) {
        StreamStateSaver sss(std::cout);

        std::cout << std::fixed << std::setprecision(2);

        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Report Pin"
                  << "\n";
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Name         : " << pin.getFullName() << "\n";
        std::cout << "Cell         : " << pin.getInstanceName() << "\n";
        std::cout << "Library Cell : "
                  << (pin.getInstanceType() == Rsyn::CELL
                          ? pin.getInstance().asCell().getLibraryCellName()
                          : std::string("N/A"))
                  << "\n";
        std::cout << "Position     : " << clsPhysicalDesign.getPinPosition(pin)
                  << "\n";
        std::cout << "Displacement : "
                  << clsPhysicalDesign.getPinDisplacement(pin) << "\n";

        if (early) {
                reportPin_TimingInformation(pin, EARLY);
        }  // end if

        if (late) {
                reportPin_TimingInformation(pin, LATE);
        }  // end if

        std::cout << "\n";
        std::cout << std::flush;
}  // end method

// -----------------------------------------------------------------------------

void Report::reportCell(Rsyn::Cell cell, const bool late, const bool early) {
        const Rsyn::PhysicalCell &phCell =
            clsPhysicalDesign.getPhysicalCell(cell);

        StreamStateSaver sss(std::cout);

        std::cout << std::fixed << std::setprecision(2);

        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Report Cell"
                  << "\n";
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Name         : " << cell.getName() << "\n";
        std::cout << "Library Cell : " << cell.getLibraryCellName() << "\n";
        std::cout << "Fixed        : " << cell.isFixed() << "\n";
        std::cout << "Position     : " << phCell.getPosition() << "\n";
        std::cout << "Size         : " << phCell.getSize() << "\n";

        std::cout << "\n";
        std::cout << "Pins\n";
        std::cout << "----\n";

        std::cout << std::left << std::setw(4) << "Name"
                  << " " << std::left << std::setw(9) << "Direction"
                  << " " << std::left << "Net"
                  << "\n";

        for (Rsyn::Pin pin : cell.allPins()) {
                std::cout << std::left << std::setw(4) << pin.getName() << " ";
                std::cout << std::left << std::setw(9)
                          << Rsyn::Global::getDirectionShortName(
                                 pin.getDirection())
                          << " ";
                std::cout << std::left << pin.getNetName() << "\n";
        }  // end for

        if (early) {
                reportCell_TimingInformation(cell, EARLY);
        }  // end if

        if (late) {
                reportCell_TimingInformation(cell, LATE);
        }  // end if

        std::cout << "\n";
        std::cout << std::flush;
}  // end method

// -----------------------------------------------------------------------------

void Report::reportNet(Rsyn::Net net, const bool late, const bool early) {
        StreamStateSaver sss(std::cout);

        std::cout << std::fixed << std::setprecision(2);

        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Report Net"
                  << "\n";
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Name         : " << net.getName() << "\n";

        std::cout << "\n";
        std::cout << "Pins\n";
        std::cout << "----\n";

        std::cout << std::left << std::setw(4) << "Name"
                  << " " << std::left << std::setw(9) << "Direction"
                  << " " << std::left << "Cell"
                  << "\n";

        for (Rsyn::Pin pin : net.allPins()) {
                std::cout << std::left << std::setw(4) << pin.getName() << " ";
                std::cout << std::left << std::setw(9)
                          << Rsyn::Global::getDirectionShortName(
                                 pin.getDirection())
                          << " ";
                std::cout << std::left << pin.getInstanceName() << "\n";
        }  // end for

        if (early) {
                reportNet_TimingInformation(net, EARLY);
        }  // end if

        if (late) {
                reportNet_TimingInformation(net, LATE);
        }  // end if

        std::cout << "\n";
        std::cout << std::flush;
}  // end method

// -----------------------------------------------------------------------------

void Report::reportTree(Rsyn::Net net) {
        StreamStateSaver sss(std::cout);

        std::cout << std::fixed << std::setprecision(2);

        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Report Tree"
                  << "\n";
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Net : " << net.getName() << "\n";

        const RCTree &tree = clsRoutingEstimator->getRCTree(net);

        const int numNodes = tree.getNumNodes();
        for (int i = 0; i < numNodes; i++) {
                const RCTree::Node &node = tree.getNode(i);
                const RCTreeNodeTag &tag = tree.getNodeTag(i);

                std::cout << std::setw(4) << i << " ";
                std::cout << std::setw(4) << node.propParent << " ";
                std::cout << std::setw(15) << tag.x << " ";
                std::cout << std::setw(15) << tag.y << " ";
                std::cout << std::setw(7) << node.getPinCap() << " ";
                std::cout << std::setw(7) << node.getWireCap() << " ";
                std::cout << std::setw(7) << node.getDownstreamCap() << " ";
                std::cout << std::setw(7)
                          << clsRoutingEstimator->getWireCapToParentNode(tree,
                                                                         i)
                          << " ";
                std::cout << std::setw(7)
                          << clsRoutingEstimator
                                 ->getDownstreamCapacitanceIgnoringParentWire(
                                     tree, i)
                          << " ";
                std::cout << (tag.getPin() ? (tag.getPin().getFullName())
                                           : std::string("-"))
                          << "\n";
        }  // end for

        std::cout << "\n";
        std::cout << std::flush;
}  // end method

}  // end namespace
