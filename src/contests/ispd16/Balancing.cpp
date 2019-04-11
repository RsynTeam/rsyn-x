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

#include "Balancing.h"

#include "tool/library/LibraryCharacterizer.h"
#include "tool/timing/Timer.h"
#include "tool/routing/RoutingEstimator.h"
#include "util/AsciiProgressBar.h"
#include "util/Stepwatch.h"
#include "iccad15/Infrastructure.h"

namespace ICCAD15 {

bool Balancing::run(const Rsyn::Json &params) {
        this->session = session;
        this->infra = session.getService("ufrgs.ispd16.infra");
        this->design = session.getDesign();
        this->timer = session.getService("rsyn.timer");
        this->libc = session.getService("rsyn.libraryCharacterizer");
        this->module = session.getTopModule();
        this->routingEstimator = session.getService("rsyn.routingEstimator");
        this->phDesign = session.getPhysicalDesign();

        if (params["type"] == "cell-steiner") {
                runCellFixSteiner();
        } else if (params["type"] == "cell-driver-sink") {
                runCellFix();
        } else if (params["type"] == "buffer") {
                runBufferFix();
        } else {
                std::cout << "Warning: Invalid optimization type '"
                          << params["type"]
                          << "' "
                             "in balancing optimization step.\n";
        }  // end else

        return true;
}  // end method

// -----------------------------------------------------------------------------

void Balancing::runBufferFix() {
        Stepwatch watch("Buffer Balancing");

        const LegalizationMethod legalizationMethod = LEG_NEAREST_WHITESPACE;
        const CostFunction costFunction = COST_LOCAL_DELAY;

        // Find single buffer chains.
        std::deque<std::tuple<Number, Rsyn::Cell>> buffers;

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (cell.getNumInputPins() != 1 || cell.getNumOutputPins() != 1)
                        continue;

                Rsyn::Pin ipin = nullptr;
                for (Rsyn::Pin pin : cell.allPins(Rsyn::IN)) ipin = pin;

                Rsyn::Pin opin = nullptr;
                for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) opin = pin;

                Rsyn::Net inet = ipin.getNet();
                Rsyn::Net onet = opin.getNet();

                if (!inet || inet.getNumPins() != 2) continue;

                if (!onet || onet.getNumPins() != 2) continue;

                const Number criticality =
                    timer->getCellCriticality(cell, Rsyn::LATE);
                if (!FloatingPoint::approximatelyZero(criticality)) {
                        buffers.push_back(std::make_tuple(criticality, cell));
                }  // end if
        }          // end for

        // Sort buffer by criticality.
        std::sort(buffers.begin(), buffers.end());

        // Equalize buffers.
        int movedBuffers = 0;
        int failed = 0;

        const int numBuffers = buffers.size();

        AsciiProgressBar progressBar(numBuffers);

        for (int i = numBuffers - 1; i >= 0; i--) {
                progressBar.progress();

                Rsyn::Cell buffer = std::get<1>(buffers[i]);
                if (!doBufferBalancing(buffer, legalizationMethod,
                                       costFunction)) {
                        failed++;
                }  // end if
                movedBuffers++;
        }  // end for

        progressBar.finish();
}  // end method

// -----------------------------------------------------------------------------

void Balancing::runCellFix() {
        Stepwatch watch("Cell Balancing (Driver/Sink)");

        const LegalizationMethod legalizationMethod = LEG_NEAREST_WHITESPACE;
        const CostFunction costFunction = COST_LOCAL_DELAY;

        infra->updatePriorities();

        // const bool debug = false; unused variable

        std::deque<std::tuple<Number, Rsyn::Cell>> cells;

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                const Number criticalityLate =
                    timer->getCellCriticality(cell, Rsyn::LATE);
                if (!FloatingPoint::approximatelyZero(criticalityLate)) {
                        const Number criticalityEarly =
                            timer->getCellCriticality(cell, Rsyn::EARLY);
                        if (2 * criticalityEarly < criticalityLate) {
                                if (!cell.isFixed() && !instance.isLCB() &&
                                    !instance.isSequential())
                                        cells.push_back(std::make_tuple(
                                            infra->getPriority(cell,
                                                               Rsyn::LATE),
                                            cell));
                        }  // end if
                }          // end if
        }                  // end for

        // Sort buffer by criticality.
        std::sort(cells.begin(), cells.end());

        // Equalize buffers.
        int movedCells = 0;
        int failed = 0;
        int rolledback = 0;

        const int numCells = cells.size();

        AsciiProgressBar progressBar(numCells);

        for (int i = numCells - 1; i >= 0; i--) {
                progressBar.progress();
                Rsyn::Cell cell = std::get<1>(cells[i]);

                if (!doCellBalancing(cell, legalizationMethod, costFunction)) {
                        failed++;
                }  // end if

                movedCells++;
        }  // end for

        progressBar.finish();
}  // end method

// -----------------------------------------------------------------------------

void Balancing::runCellFixSteiner() {
        Stepwatch watch("Cell Balancing (Steiner)");

        const LegalizationMethod legalizationMethod = LEG_NEAREST_WHITESPACE;
        const CostFunction costFunction = COST_LOCAL_DELAY;

        infra->updatePriorities();

        std::deque<std::tuple<Number, Rsyn::Cell>> cells;

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                const Number criticalityLate =
                    timer->getCellCriticality(cell, Rsyn::LATE);
                if (!FloatingPoint::approximatelyZero(criticalityLate)) {
                        const Number criticalityEarly =
                            timer->getCellCriticality(cell, Rsyn::EARLY);
                        if (2 * criticalityEarly < criticalityLate) {
                                if (!cell.isFixed() && !instance.isLCB() &&
                                    !instance.isSequential())
                                        cells.push_back(std::make_tuple(
                                            infra->getPriority(cell,
                                                               Rsyn::LATE),
                                            cell));
                        }  // end if
                }          // end if
        }                  // end for

        // Sort by criticality.
        std::sort(cells.begin(), cells.end());

        // Equalize buffers.
        int movedCells = 0;
        int failed = 0;
        int rolledback = 0;

        const int numCells = cells.size();

        AsciiProgressBar progressBar(numCells);

        for (int i = numCells - 1; i >= 0; i--) {
                progressBar.progress();
                Rsyn::Cell cell = std::get<1>(cells[i]);

                if (!doCellBalancingSteiner(cell, legalizationMethod,
                                            costFunction)) {
                        failed++;
                }  // end if

                movedCells++;
        }  // end for

        progressBar.finish();
}  // end method

// -----------------------------------------------------------------------------

bool Balancing::doBufferBalancing(Rsyn::Cell buffer,
                                  const LegalizationMethod legalizationMethod,
                                  const CostFunction costFunction) {
        if (buffer.isFixed()) {
                return false;  // don't move fixed cells
        }                      // end if

        Rsyn::Pin ipin = nullptr;
        for (Rsyn::Pin pin : buffer.allPins(Rsyn::IN))
                ipin = pin;  // should be the only input pin

        Rsyn::Pin opin = nullptr;
        for (Rsyn::Pin pin : buffer.allPins(Rsyn::OUT))
                opin = pin;  // should be the only output pin

        Rsyn::Net inet = ipin.getNet();
        Rsyn::Net onet = opin.getNet();

        Rsyn::Pin driver = inet.getAnyDriver();
        if (driver.isPort()) {
                return false;  // [TODO] Skip for now, in the future use input
                               // driver if available...
        }                      // end if

        Rsyn::Pin sink;
        for (Rsyn::Pin pin : onet.allPins(Rsyn::SINK))
                sink = pin;  // should be the only sink pin

        std::tuple<Number, Rsyn::TimingTransition> slackTransition =
            timer->getPinWorstSlackWithTransition(driver, Rsyn::LATE);
        const Rsyn::TimingTransition iedge = std::get<1>(slackTransition);
        const Rsyn::TimingTransition oedge =
            iedge == Rsyn::RISE ? Rsyn::FALL
                                : Rsyn::RISE;  // [TODO] Make sense aware.

        Rsyn::Arc arcDriver =
            timer->getCriticalTimingArcToPin(driver, Rsyn::LATE, iedge);
        Rsyn::Arc arcBuffer = ipin.getArcTo(opin);

        if (!arcDriver || !arcBuffer) {
                return false;
        }  // end if

        const double Cw = routingEstimator->getLocalWireCapPerUnitLength();
        const double Rw = routingEstimator->getLocalWireResPerUnitLength();

        const double R0 =
            libc->getDriverResistance(arcDriver, Rsyn::LATE, iedge);
        const double R1 =
            libc->getDriverResistance(arcBuffer, Rsyn::LATE, oedge);

        const double C1 = timer->getPinInputCapacitance(ipin);
        const double C2 = timer->getPinInputCapacitance(sink);

        const DBUxy driverPos = phDesign.getPinPosition(driver);
        const DBUxy ipinPos = phDesign.getPinPosition(ipin);
        const DBUxy opinPos = phDesign.getPinPosition(opin);
        const DBUxy sinkPos = phDesign.getPinPosition(sink);

        const double d = infra->computeManhattanDistance(driverPos, sinkPos);
        const double a = 0;  // computeManhattanDistance(ipinPos, opinPos);

        if (FloatingPoint::approximatelyZero(d)) {
                return false;
        }  // end if

        const double d1 =
            std::min(d - a, std::max(0.0, (Cw * R1 - Cw * R0 + Rw * C2 -
                                           Rw * C1 + (Cw * d - a * Cw) * Rw) /
                                              (2 * Cw * Rw)));

        const double dx = sinkPos.x - driverPos.x;
        const double dy = sinkPos.y - driverPos.y;
        const double scaling = d1 / d;

        const double px = scaling * dx + driverPos.x;
        const double py = scaling * dy + driverPos.y;

        //	// Debug
        //-------------------------------------------------------------------
        //	std::cout << "\n";
        //	std::cout << "moving buffer " << buffer.getName() << " ("
        //			<< buffer.getLibraryCellName() << ")\n";
        //	std::cout << "R0=" << R0 << "\n";
        //	std::cout << "R1=" << R1 << "\n";
        //	std::cout << "C1=" << C1 << "\n";
        //	std::cout << "C2=" << C2 << "\n";
        //	std::cout << "a=" << a << "\n";
        //	std::cout << "d=" << d << "\n";
        //	std::cout << "d1=" << d1 << " (" << (scaling) << ")" << "\n";
        //
        //	std::cout << "\n";
        //	std::cout << "Rw*d1=" <<
        //(Rw*computeManhattanDistance(getPinPosition(driver),
        // getPinPosition(ipin))) << "\n";
        //	std::cout << "Cw*d1=" <<
        //(Cw*computeManhattanDistance(getPinPosition(driver),
        // getPinPosition(ipin))) << "\n";
        //	std::cout << "load driver net = " << clsTimer->getNetLoad(inet)
        //<< "\n";
        //	std::cout << "load sink net   = " << clsTimer->getNetLoad(onet)
        //<< "\n";
        //	std::cout << "driver: logical effort delay = " <<
        // clsTimer->getLogicalEffortDelay(*arcDriver, Rsyn::LATE, iedge,
        // clsTimer->getNetLoad(inet)[iedge]) << "\n";
        //	std::cout << "buffer: logical effort delay = " <<
        // clsTimer->getLogicalEffortDelay(*arcBuffer, Rsyn::LATE, oedge,
        // clsTimer->getNetLoad(onet)[oedge]) << "\n";
        //	std::cout << "\n";
        //	std::cout << "driver: "; clsTimer->printArcDebug(std::cout,
        //*arcDriver);
        //	std::cout << "buffer: "; clsTimer->printArcDebug(std::cout,
        //*arcBuffer);
        //	std::cout << "\n";
        //	std::cout << "start : ";
        // clsTimer->printPinTimingDebug(std::cout,
        // clsTimer->getFromPinOfTimingArc(*arcDriver), false, true);
        //	std::cout << "driver: ";
        // clsTimer->printPinTimingDebug(std::cout, driver, false, true);
        //	std::cout << "buffer: ";
        // clsTimer->printPinTimingDebug(std::cout, ipin, false, true);
        //	std::cout << "buffer: ";
        // clsTimer->printPinTimingDebug(std::cout, opin, false, true);
        //	std::cout << "sink  : ";
        // clsTimer->printPinTimingDebug(std::cout, sink, false, true);
        //	//
        //-------------------------------------------------------------------------

        double oldCost;
        double newCost;
        const bool success = infra->moveCellWithCostEvaluation(
            buffer, (DBU)px, (DBU)py, legalizationMethod, costFunction,
            Rsyn::LATE, oldCost, newCost);

        //	// Debug
        //-------------------------------------------------------------------
        //	double qor = clsQualityScore;
        //	updateTimingIncremental();
        //	updateQualityScore();
        //	reportSummary("buf");
        //
        //	std::cout << "\n";
        //	std::cout << "Rw*d1=" <<
        //(Rw*computeManhattanDistance(getPinPosition(driver),
        // getPinPosition(ipin))) << "\n";
        //	std::cout << "Cw*d1=" <<
        //(Cw*computeManhattanDistance(getPinPosition(driver),
        // getPinPosition(ipin))) << "\n";
        //	std::cout << "load driver net = " << clsTimer->getNetLoad(inet)
        //<< "\n";
        //	std::cout << "load sink net   = " << clsTimer->getNetLoad(onet)
        //<< "\n";
        //	std::cout << "driver: logical effort delay = " <<
        // clsTimer->getLogicalEffortDelay(*arcDriver, Rsyn::LATE, iedge,
        // clsTimer->getNetLoad(inet)[iedge]) << "\n";
        //	std::cout << "buffer: logical effort delay = " <<
        // clsTimer->getLogicalEffortDelay(*arcBuffer, Rsyn::LATE, oedge,
        // clsTimer->getNetLoad(onet)[oedge]) << "\n";
        //	std::cout << "\n";
        //	std::cout << "driver: "; clsTimer->printArcDebug(std::cout,
        //*arcDriver);
        //	std::cout << "buffer: "; clsTimer->printArcDebug(std::cout,
        //*arcBuffer);
        //	std::cout << "\n";
        //	std::cout << "start : ";
        // clsTimer->printPinTimingDebug(std::cout,
        // clsTimer->getFromPinOfTimingArc(*arcDriver), false, true);
        //	std::cout << "driver: ";
        // clsTimer->printPinTimingDebug(std::cout, driver, false, true);
        //	std::cout << "buffer: ";
        // clsTimer->printPinTimingDebug(std::cout, ipin, false, true);
        //	std::cout << "buffer: ";
        // clsTimer->printPinTimingDebug(std::cout, opin, false, true);
        //	std::cout << "sink  : ";
        // clsTimer->printPinTimingDebug(std::cout, sink, false, true);
        //	//
        //-------------------------------------------------------------------------

        return success;
}  // end method

// -----------------------------------------------------------------------------

bool Balancing::doCellBalancing(Rsyn::Cell cell,
                                const LegalizationMethod legalizationMethod,
                                const CostFunction costFunction) {
        // const bool debug = false; //cell.getName() == "o618145"; unused
        // variable

        if (cell.isFixed()) {
                return false;  // don't move fixed cells
        }                      // end if

        const double Cw = routingEstimator->getLocalWireCapPerUnitLength();
        const double Rw = routingEstimator->getLocalWireResPerUnitLength();

        double avgPx = 0;
        double avgPy = 0;
        double totalWeight = 0;
        int numPos = 0;

        for (Rsyn::Pin opin : cell.allPins(Rsyn::OUT)) {
                Rsyn::Net onet = opin.getNet();

                for (Rsyn::Pin ipin : cell.allPins(Rsyn::IN)) {
                        Rsyn::Net inet = ipin.getNet();
                        if (!inet) continue;

                        Rsyn::Pin driver = inet.getAnyDriver();
                        if (!driver) continue;

                        if (driver.isPort()) {
                                continue;  // [TODO] Skip for now, in the future
                                           // use input driver if available...
                        }                  // end if

                        std::tuple<Number, Rsyn::TimingTransition>
                            slackTransition =
                                timer->getPinWorstSlackWithTransition(
                                    driver, Rsyn::LATE);
                        const Rsyn::TimingTransition iedge =
                            std::get<1>(slackTransition);
                        const Rsyn::TimingTransition oedge =
                            iedge == Rsyn::RISE
                                ? Rsyn::FALL
                                : Rsyn::RISE;  // [TODO] Make sense aware.

                        Rsyn::Arc arcDriver = timer->getCriticalTimingArcToPin(
                            driver, Rsyn::LATE, iedge);
                        Rsyn::Arc arcBuffer = ipin.getArcTo(opin);
                        if (!arcDriver || !arcBuffer) {
                                continue;
                        }  // end if

                        const double R0 = libc->getDriverResistance(
                            arcDriver, Rsyn::LATE, iedge);
                        const double R1 = libc->getDriverResistance(
                            arcBuffer, Rsyn::LATE, oedge);

                        for (Rsyn::Pin sink : onet.allPins(Rsyn::SINK)) {
                                const double C1 =
                                    timer->getPinInputCapacitance(ipin);
                                const double C2 =
                                    timer->getPinInputCapacitance(sink);

                                const DBUxy driverPos =
                                    phDesign.getPinPosition(driver);
                                const DBUxy ipinPos =
                                    phDesign.getPinPosition(ipin);
                                const DBUxy opinPos =
                                    phDesign.getPinPosition(opin);
                                const DBUxy sinkPos =
                                    phDesign.getPinPosition(sink);

                                const double d =
                                    infra->computeManhattanDistance(driverPos,
                                                                    sinkPos);
                                const double a =
                                    0;  // computeManhattanDistance(ipinPos,
                                // opinPos);

                                if (FloatingPoint::approximatelyZero(d)) {
                                        continue;
                                }  // end if

                                const double w0 =
                                    infra->getPinImportance(driver, Rsyn::LATE);
                                const double w1 =
                                    infra->getPinImportance(sink, Rsyn::LATE);

                                if (FloatingPoint::approximatelyZero(w0 + w1))
                                        continue;

                                // const double d1 = std::min(d - a,
                                // std::max(0.0,
                                //		(Cw*R1-Cw*R0+Rw*C2-Rw*C1+(Cw*d+a*Cw)*Rw)/(2*Cw*Rw)));

                                const double actualD =
                                    (Cw * w1 * R1 - Cw * w0 * R0 +
                                     Rw * w1 * C2 - Rw * w0 * C1 +
                                     (Cw * d - a * Cw) * Rw * w1) /
                                    (Cw * Rw * w1 + Cw * Rw * w0);

                                const double d0 =
                                    std::min(d - a, std::max(0.0, actualD));

                                const double dx = sinkPos.x - driverPos.x;
                                const double dy = sinkPos.y - driverPos.y;
                                const double scaling = d0 / d;

                                // const double weight =
                                // clsTimer->getPinCriticality(driver,
                                // Rsyn::LATE) *
                                //	clsTimer->getPinCriticality(sink,
                                // Rsyn::LATE);
                                const double weight = w0 + w1;

                                avgPx += weight * (scaling * dx + driverPos.x);
                                avgPy += weight * (scaling * dy + driverPos.y);
                                totalWeight += weight;
                                numPos++;
                        }  // end for (sink)
                }          // end for (input pin)
        }                  // end for (output pin)

        bool success = false;
        if (numPos > 0 && totalWeight > 0) {
                avgPx /= totalWeight;
                avgPy /= totalWeight;

                double oldCost;
                double newCost;
                success = infra->moveCellWithCostEvaluation(
                    cell, (DBU)avgPx, (DBU)avgPy, legalizationMethod,
                    costFunction, Rsyn::LATE, oldCost, newCost);
        }  // end if

        return success;
}  // end method

// -----------------------------------------------------------------------------

bool Balancing::doCellBalancingSteiner(
    Rsyn::Cell cell, const LegalizationMethod legalizationMethod,
    const CostFunction costFunction) {
        const bool debug = false;

        if (cell.isFixed()) {
                return false;  // don't move fixed cells
        }                      // end if

        const double Cw = routingEstimator->getLocalWireCapPerUnitLength();
        const double Rw = routingEstimator->getLocalWireResPerUnitLength();

        double avgPx = 0;
        double avgPy = 0;
        double totalWeight = 0;
        int numPos = 0;

        for (Rsyn::Pin opin : cell.allPins(Rsyn::OUT)) {
                Rsyn::Net onet = opin.getNet();

                if (!onet || onet.getNumPins() < 2) continue;

                for (Rsyn::Pin ipin : cell.allPins(Rsyn::IN)) {
                        Rsyn::Net inet = ipin.getNet();

                        if (!inet || inet.getNumPins() < 2) continue;

                        Rsyn::Pin driver = inet.getAnyDriver();
                        if (!driver) continue;

                        if (driver.isPort()) {
                                continue;  // [TODO] Skip for now, in the future
                                           // use input driver if available...
                        }                  // end if

                        std::tuple<Number, Rsyn::TimingTransition>
                            slackTransition =
                                timer->getPinWorstSlackWithTransition(
                                    driver, Rsyn::LATE);
                        const Rsyn::TimingTransition iedge =
                            std::get<1>(slackTransition);
                        const Rsyn::TimingTransition oedge =
                            iedge == Rsyn::RISE
                                ? Rsyn::FALL
                                : Rsyn::RISE;  // [TODO] Make sense aware.

                        Rsyn::Arc arcDriver = timer->getCriticalTimingArcToPin(
                            driver, Rsyn::LATE, iedge);
                        Rsyn::Arc arcBuffer = ipin.getArcTo(opin);
                        if (!arcDriver || !arcBuffer) {
                                continue;
                        }  // end if

                        const Rsyn::RCTree &itree =
                            routingEstimator->getRCTree(inet);
                        const Rsyn::RCTree &otree =
                            routingEstimator->getRCTree(onet);

                        const int iNodeIndex =
                            routingEstimator->getRCTreeConnectingNodeIndex(
                                itree, ipin);
                        const int oNodeIndex =
                            routingEstimator->getRCTreeConnectingNodeIndex(
                                otree, opin);

                        if (iNodeIndex < 0) {
                                std::cout << "\n[BUG] Invalid value for "
                                             "iNodeIndex at pin ";
                                std::cout << ipin.getInstanceName() << ":"
                                          << ipin.getName() << "\n";
                                std::cout << " iNodeIndex = " << iNodeIndex
                                          << "\n";
                                std::cout << " Net " << ipin.getNet().getName();
                                std::cout << " has "
                                          << ipin.getNet().getNumPins();
                                std::cout << " pins.\n";
                                return false;
                        }  // end if;

                        if (oNodeIndex < 0) {
                                std::cout << "\n[BUG] Invalid value for "
                                             "oNodeIndex at pin ";
                                std::cout << opin.getInstanceName() << ":"
                                          << opin.getName() << "\n";
                                std::cout << " oNodeIndex = " << oNodeIndex
                                          << "\n";
                                std::cout << " Net " << opin.getNet().getName();
                                std::cout << " has "
                                          << opin.getNet().getNumPins();
                                std::cout << " pins.\n";
                                return false;
                        }  // end if;

                        // Skip nodes added just for sake of large
                        // interconnection slicing.
                        const int iindex = itree.getUpstreamBranchingNodeIndex(
                            itree.getUpstreamSteinerPoint(iNodeIndex));
                        const int oindex =
                            otree.getDownstreamBranchingNodeIndex(
                                otree.getDownstreamNodeIndex(oNodeIndex));

                        const Rsyn::RCTree::Node &inode = itree.getNode(iindex);
                        const Rsyn::RCTree::Node &onode = otree.getNode(oindex);

                        const Rsyn::RCTreeNodeTag &itag =
                            itree.getNodeTag(iindex);
                        const Rsyn::RCTreeNodeTag &otag =
                            otree.getNodeTag(oindex);

                        const double R0 = libc->getDriverResistance(
                            arcDriver, Rsyn::LATE, iedge);
                        const double R1 = libc->getDriverResistance(
                            arcBuffer, Rsyn::LATE, oedge);

                        const double Cwire =
                            routingEstimator->getWireCapToParentNode(otree,
                                                                     oindex);

                        const double Rup = inode.getUpstreamRes() + R0;
                        const double Cdown =
                            onode.getDownstreamCap()[oedge] - Cwire;
                        if (Cdown < 0) {
                                std::cout << "[BUG] Negative downstream "
                                             "capacitance. This may be due to "
                                             "a bug or because the downstream "
                                             "cap is not updated.\n";
                                std::cout << "  Cdown (w/  wire) = "
                                          << onode.getDownstreamCap()[oedge]
                                          << "\n";
                                std::cout << "  Cdown (w/o wire) = " << Cdown
                                          << "\n";
                                std::cout << "  Cwire = " << Cwire << "\n";
                        }  // end if

                        for (Rsyn::Pin sink : onet.allPins(Rsyn::SINK)) {
                                const double C1 =
                                    timer->getPinInputCapacitance(ipin);

                                const DBUxy driverPos(itag.x, itag.y);
                                const DBUxy ipinPos =
                                    phDesign.getPinPosition(ipin);
                                const DBUxy opinPos =
                                    phDesign.getPinPosition(opin);
                                const DBUxy sinkPos(otag.x, otag.y);

                                const double d =
                                    infra->computeManhattanDistance(driverPos,
                                                                    sinkPos);
                                const double a =
                                    0;  // computeManhattanDistance(ipinPos,
                                // opinPos);

                                if (FloatingPoint::approximatelyZero(d)) {
                                        // This may happen but I'm not sure
                                        // when... Well I guess the
                                        // input and output tree have a steiner
                                        // point at same position
                                        // and the cell connects to these
                                        // exactly steiner points.
                                        continue;
                                }  // end if

                                const double w0 =
                                    infra->getPinImportance(driver, Rsyn::LATE);
                                const double w1 =
                                    infra->getPinImportance(opin, Rsyn::LATE);

                                if (FloatingPoint::approximatelyZero(w0 + w1))
                                        continue;

                                const double actualD =
                                    (Cw * w1 * R1 - Rw * w0 * C1 +
                                     (Cw * d - a * Cw + Cdown) * Rw * w1 -
                                     Cw * Rup * w0) /
                                    (Cw * Rw * w1 + Cw * Rw * w0);

                                const double d0 =
                                    std::min(d - a, std::max(0.0, actualD));

                                const double dx = sinkPos.x - driverPos.x;
                                const double dy = sinkPos.y - driverPos.y;
                                const double scaling = d0 / d;

                                const double weight = w0 + w1;

                                avgPx += weight * (scaling * dx + driverPos.x);
                                avgPy += weight * (scaling * dy + driverPos.y);
                                totalWeight += weight;
                                numPos++;

                                if (debug) {
                                        std::cout << "\n";
                                        std::cout << "moving buffer "
                                                  << cell.getName() << " ("
                                                  << cell.getLibraryCellName()
                                                  << ")\n";

                                        std::cout << "iNodeIndex=" << iNodeIndex
                                                  << "\n";
                                        std::cout << "oNodeIndex=" << oNodeIndex
                                                  << "\n";

                                        std::cout << "iindex=" << iindex
                                                  << "\n";
                                        std::cout << "oindex=" << oindex
                                                  << "\n";

                                        std::cout << "driverPos=" << driverPos
                                                  << "\n";
                                        std::cout << "sinkPos=" << sinkPos
                                                  << "\n";
                                        std::cout << "ipinPos=" << ipinPos
                                                  << "\n";
                                        std::cout << "opinPos=" << opinPos
                                                  << "\n";

                                        std::cout << "Rup=" << Rup << "\n";
                                        std::cout << "Cdown=" << Cdown << "\n";
                                        std::cout << "R0=" << R0 << "\n";
                                        std::cout << "R1=" << R1 << "\n";
                                        std::cout << "a=" << a << "\n";
                                        std::cout << "d=" << d << "\n";
                                        std::cout << "d1=" << d0 << " ("
                                                  << (scaling) << ")"
                                                  << "\n";
                                        std::cout << "d1 (actual)=" << actualD
                                                  << " (" << (scaling) << ")"
                                                  << "\n";

                                        std::cout << "\n";
                                        std::cout
                                            << "Rw*d1="
                                            << (Rw *
                                                infra->computeManhattanDistance(
                                                    phDesign.getPinPosition(
                                                        driver),
                                                    phDesign.getPinPosition(
                                                        ipin)))
                                            << "\n";
                                        std::cout
                                            << "Cw*d1="
                                            << (Cw *
                                                infra->computeManhattanDistance(
                                                    phDesign.getPinPosition(
                                                        driver),
                                                    phDesign.getPinPosition(
                                                        ipin)))
                                            << "\n";
                                        std::cout << "load driver net = "
                                                  << timer->getNetLoad(inet)
                                                  << "\n";
                                        std::cout << "load sink net   = "
                                                  << timer->getNetLoad(onet)
                                                  << "\n";
                                        std::cout
                                            << "driver: logical effort delay = "
                                            << libc->getArcLogicalEffortDelay(
                                                   arcDriver, Rsyn::LATE, iedge,
                                                   timer->getNetLoad(
                                                       inet)[iedge])
                                            << "\n";
                                        std::cout
                                            << "buffer: logical effort delay = "
                                            << libc->getArcLogicalEffortDelay(
                                                   arcBuffer, Rsyn::LATE, oedge,
                                                   timer->getNetLoad(
                                                       onet)[oedge])
                                            << "\n";
                                        std::cout << "\n";
                                        std::cout << "driver: ";
                                        timer->printArcDebug(std::cout,
                                                             arcDriver);
                                        std::cout << "buffer: ";
                                        timer->printArcDebug(std::cout,
                                                             arcBuffer);
                                        std::cout << "\n";
                                        std::cout << "start : ";
                                        timer->printPinDebug(
                                            std::cout, arcDriver.getFromPin(),
                                            false, true);
                                        std::cout << "driver: ";
                                        timer->printPinDebug(std::cout, driver,
                                                             false, true);
                                        std::cout << "buffer: ";
                                        timer->printPinDebug(std::cout, ipin,
                                                             false, true);
                                        std::cout << "buffer: ";
                                        timer->printPinDebug(std::cout, opin,
                                                             false, true);
                                        std::cout << "sink  : ";
                                        timer->printPinDebug(std::cout, sink,
                                                             false, true);
                                }  // end if
                        }          // end for (sink)
                }                  // end for (input pin)
        }                          // end for (output pin)

        bool success = false;
        if (numPos > 0 && totalWeight > 0) {
                avgPx /= totalWeight;
                avgPy /= totalWeight;

                double oldCost;
                double newCost;
                success = infra->moveCellWithCostEvaluation(
                    cell, (DBU)avgPx, (DBU)avgPy, legalizationMethod,
                    costFunction, Rsyn::LATE, oldCost, newCost);
        }  // end if

        return success;
}  // end method

}  // end namescape
