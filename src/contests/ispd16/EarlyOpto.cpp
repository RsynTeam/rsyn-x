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

#include "EarlyOpto.h"

#include "util/AsciiProgressBar.h"
#include "util/Stepwatch.h"
#include "util/Array.h"
#include "tool/library/LibraryCharacterizer.h"
#include "tool/timing/Timer.h"
#include "tool/routing/RoutingEstimator.h"
#include "iccad15/Infrastructure.h"
#include "munkres/munkres.h"

namespace ICCAD15 {

bool EarlyOpto::run(const Rsyn::Json &params) {
        this->session = session;
        this->infra = session.getService("ufrgs.ispd16.infra");
        this->design = session.getDesign();
        this->timer = session.getService("rsyn.timer");
        this->routingEstimator = session.getService("rsyn.routingEstimator");
        this->libc = session.getService("rsyn.libraryCharacterizer");
        this->module = session.getTopModule();
        this->phDesign = session.getPhysicalDesign();

        // Define a small clock uncertainty to account for the small mismatches
        // between our built-in timer and ui-timer. In this way our optimization
        // pushes the early slack further down, which than can zero the early
        // slack
        // when measured in the ui-timer. It seems main source of mismatches are
        // due to CCPR, which we currently not implement in our flow.
        const Number oldClockUncertainty =
            timer->getClockUncertainty(Rsyn::EARLY);
        timer->setClockUncertainty(Rsyn::EARLY, 2);
        infra->updateQualityScore();
        infra->reportSummary("set-clock-uncertainty");
        infra->updateTDPSolution(true);

        if (params.count("runOnlyEarlySpreadingIterative") &&
            params["runOnlyEarlySpreadingIterative"]) {
                runEarlySpreadingIterative(true);
        } else {
                stepSkewOptimization();
                infra->statePush("skew-opto");

                stepIterativeSpreading();
                infra->statePush("iterative-spreading");

                stepRegisterSwap();
                infra->statePush("reg-swap");

                stepRegisterToRegisterPathFix();
                infra->statePush("reg-to-reg-path-fix");
        }  // end else

        // Revert clock uncertainty.
        timer->setClockUncertainty(Rsyn::EARLY, oldClockUncertainty);
        infra->updateQualityScore();
        infra->reportSummary("reset-clock-uncertainty");
        infra->updateTDPSolution(true);

        // Restore best solution.
        infra->restoreBestSolution();

        return true;
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::stepSkewOptimization() {
        do {
                runEarlySkewOptimization();
                timer->updateTimingIncremental();
                infra->updateQualityScore();
                infra->reportSummary("skew-opto");
        } while (infra->updateTDPSolution());
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::stepIterativeSpreading() {
        runEarlySpreadingIterative(true);
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::stepRegisterSwap() {
        if (FloatingPoint::notApproximatelyZero(timer->getTns(Rsyn::EARLY))) {
                do {
                        runEarlyLocalSkewOptimizationByFlipFlopSwapping();
                        timer->updateTimingIncremental();
                        infra->updateQualityScore();
                        infra->reportSummary("reg-swap");
                } while (infra->updateTDPSolution());
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::stepRegisterToRegisterPathFix() {
        if (FloatingPoint::notApproximatelyZero(timer->getTns(Rsyn::EARLY))) {
                do {
                        runEarlyWireOptimization();
                        timer->updateTimingIncremental();
                        infra->updateQualityScore();
                        infra->reportSummary("reg-to-reg-path-fix");
                } while (infra->updateTDPSolution());
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::runEarlySkewOptimization() {
        const int N = 1000;

        std::vector<Rsyn::Pin> endpoints;
        timer->queryTopCriticalEndpoints(Rsyn::EARLY, N, endpoints);

        int processed = 0;
        int failed = 0;
        int fixed = 0;

        const int numEndpoints = endpoints.size();
        for (int i = 0; i < numEndpoints; i++) {
                Rsyn::Pin endpoint = endpoints[i];
                Rsyn::Cell flipflop = endpoint.getInstance().asCell();

                processed++;

                // Check if this flipflop is movable.
                if (flipflop.isFixed()) {
                        fixed++;
                        continue;
                }  // end if

                // Check if this cell is a flipflop.
                if (!flipflop.isSequential()) continue;

                // Get the clock pin of the flipflop.
                Rsyn::Pin clockPin = timer->getClockPin(flipflop);
                if (!clockPin) continue;

                // Get the net driving the clock pin (not necessarily the clock
                // net).
                Rsyn::Net localClockNet = clockPin.getNet();
                if (!localClockNet) continue;

                Rsyn::Pin localClockDriverPin = localClockNet.getAnyDriver();
                if (!localClockDriverPin) continue;

                Rsyn::Cell localClockDriverCell =
                    localClockDriverPin.getInstance().asCell();
                Rsyn::PhysicalCell phLocalClockDriverCell =
                    phDesign.getPhysicalCell(localClockDriverCell);
                const DBUxy lcbPos = phLocalClockDriverCell.getPosition();
                if (!localClockDriverCell.isLCB()) continue;

                if (!infra->moveCell(flipflop, lcbPos[X], lcbPos[Y],
                                     LEG_NEAREST_WHITESPACE)) {
                        failed++;
                }  // end if
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::runEarlyWireOptimization() {
        const int N = 100;
        const double maxDisp = infra->getMaxDisplacement();

        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        timer->queryTopCriticalPaths(Rsyn::EARLY, N, paths);

        std::set<Rsyn::Pin> visited;  // process only one register per lcb

        int statsProcessed = 0;
        int statsLegalizationFails = 0;
        int statsFixed = 0;
        int statsRevisited = 0;

        const double Cw = routingEstimator->getLocalWireCapPerUnitLength();
        const double Rw = routingEstimator->getLocalWireResPerUnitLength();

        const int numPaths = paths.size();
        for (int i = 0; i < numPaths; i++) {
                const std::vector<Rsyn::Timer::PathHop> &path = paths[i];

                // Check if this a ff-to-ff path.
                if (path.size() >= 2) {
                        const Rsyn::Timer::PathHop &startHop =
                            path[path.size() - 2];
                        const Rsyn::Timer::PathHop &endHop =
                            path[path.size() - 1];

                        Rsyn::Cell startCell = startHop.getInstance().asCell();
                        Rsyn::Cell endCell = endHop.getInstance().asCell();

                        if (startCell.isSequential() &&
                            endCell.isSequential()) {
                                statsProcessed++;

                                Rsyn::Pin ckpin_i =
                                    timer->getClockPin(startCell);
                                Rsyn::Pin ckpin_o = timer->getClockPin(endCell);

                                Rsyn::Pin qpin_i =
                                    path[path.size() - 2]
                                        .getPin();  // q pin of input register
                                Rsyn::Pin dpin_o =
                                    path[path.size() - 1]
                                        .getPin();  // d pin of output register

                                Rsyn::Pin driver_i =
                                    ckpin_i.getNet()
                                        ? (ckpin_i.getNet().getAnyDriver())
                                        : nullptr;  // lcb or clock pin
                                Rsyn::Pin driver_o =
                                    ckpin_o.getNet()
                                        ? (ckpin_o.getNet().getAnyDriver())
                                        : nullptr;  // lcb or clock pin

                                if (visited.count(driver_i) ||
                                    visited.count(driver_o)) {
                                        statsRevisited++;
                                        continue;
                                }  // end if

                                visited.insert(driver_i);
                                visited.insert(driver_o);

                                if (!startCell.isMovable()) {
                                        statsFixed++;
                                        continue;
                                }  // end if

                                Rsyn::PhysicalCell startPhysicalCell =
                                    phDesign.getPhysicalCell(startCell);
                                const DBUxy startCellPos =
                                    startPhysicalCell.getPosition();

                                Rsyn::PhysicalCell endPhysicalCell =
                                    phDesign.getPhysicalCell(endCell);
                                const DBUxy endCellPos =
                                    endPhysicalCell.getPosition();

                                const DBUxy direction =
                                    (startCellPos - endCellPos)
                                        .safeNormalized();

                                Rsyn::Arc arcRegister =
                                    ckpin_i.getArcTo(qpin_i);
                                const double Ri = libc->getDriverResistance(
                                    arcRegister, Rsyn::EARLY,
                                    startHop.getTransition());

                                const double thold =
                                    timer->getHoldTime(endCell).getMax();
                                const double li_early =
                                    timer->getPinArrivalTime(
                                        ckpin_i, Rsyn::EARLY,
                                        Rsyn::RISE);  // [HARD CODED] Assuming
                                                      // rise triggered
                                                      // register.
                                const double lo_late = timer->getPinArrivalTime(
                                    ckpin_o, Rsyn::LATE,
                                    Rsyn::RISE);  // [HARD CODED] Assuming rise
                                                  // triggered register.
                                const double Co =
                                    timer->getPinInputCapacitance(dpin_o);
                                const double K = lo_late + thold - li_early;

                                const double calculatedDistance =
                                    (std::sqrt(2 * Cw * Rw * K +
                                               Co * Co * Rw * Rw +
                                               Cw * Cw * Ri * Ri) -
                                     Co * Rw - Cw * Ri) /
                                    (Cw * Rw);
                                const DBU distance =
                                    (DBU)std::min(maxDisp, calculatedDistance);

                                // std::cout << "distance=" << distance << " ("
                                // << calculatedDistance << ")" << "\n";

                                // const DBUxy pos = endPhysicalCell.lower() +
                                // distance*direction;
                                const DBUxy pos =
                                    endCellPos + distance * direction;
                                // if (!moveCell(startPhysicalCell, pos.x,
                                // pos.y, LEG_NEAREST_WHITESPACE)) {
                                if (!infra->moveCell(startCell, pos.x, pos.y,
                                                     LEG_NEAREST_WHITESPACE)) {
                                        statsLegalizationFails++;
                                }  // end if

                        }  // end if
                } else {
                        std::cout << "Non reg-to-reg path...\n";
                }
        }  // end for

        std::cout << "Early Wire Optimization:\n";
        std::cout << "#processed : " << statsProcessed << "\n";
        std::cout << "#fixed : " << statsFixed << "\n";
        std::cout << "#revisited : " << statsRevisited << "\n";
        std::cout << "#failed : " << statsLegalizationFails << "\n";
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::runEarlyLocalSkewOptimizationByFlipFlopSwapping() {
        AsciiProgressBar progressBar(design.getNumInstances(Rsyn::CELL));

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                progressBar.progress();

                if (timer->getCellCriticality(cell, Rsyn::EARLY) > 0) {
                        if (cell.isLCB()) {
                                doLocalSkewOptimization(cell);
                        }  // end if
                }          // end for
        }                  // end for

        progressBar.finish();
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::doLocalSkewOptimization(Rsyn::Cell lcb) {
        if (!lcb.isLCB()) return;

        Rsyn::Pin lcpOutputPin = nullptr;
        for (Rsyn::Pin pin : lcb.allPins(Rsyn::OUT))
                lcpOutputPin = pin;  // should be the only output pin

        Rsyn::Net lcbOutputNet = lcpOutputPin.getNet();
        if (!lcbOutputNet) return;

        const int numSinks = lcbOutputNet.getNumSinks();

        struct Slot {
                DBUxy pos;
                double latency[Rsyn::NUM_TIMING_MODES];
        };  // end struct

        std::vector<Slot> slots;
        slots.reserve(numSinks);

        std::vector<Rsyn::Cell> mapping;
        mapping.reserve(numSinks);

        // Annotate the latency for each slot.
        for (Rsyn::Pin pin : lcbOutputNet.allPins(Rsyn::SINK)) {
                Rsyn::Cell flipflop = pin.getInstance().asCell();
                if (!flipflop.isSequential()) {
                        continue;
                }  // end if

                if (flipflop.isFixed()) {
                        continue;
                }  // end method

                Rsyn::Pin clockPin = timer->getClockPin(flipflop);
                mapping.push_back(flipflop);

                const PhysicalCell &physicalFlipflop =
                    phDesign.getPhysicalCell(flipflop);

                // [TODO] Assuming rise-edge triggered flipflops.
                Slot slot;
                slot.pos = physicalFlipflop.getPosition();
                slot.latency[Rsyn::EARLY] =
                    timer->getPinArrivalTime(clockPin, Rsyn::EARLY, Rsyn::RISE);
                slot.latency[Rsyn::LATE] =
                    timer->getPinArrivalTime(clockPin, Rsyn::LATE, Rsyn::RISE);

                slots.push_back(slot);
        }  // end for

        // Create the cost matrix.
        const int numSlots = mapping.size();
        if (numSlots == 0) return;

        Matrix<double> matrix(numSlots, numSlots);

        for (int source = 0; source < numSlots; source++) {
                for (int target = 0; target < numSlots; target++) {
                        Rsyn::Cell flipflop = mapping[source];
                        Rsyn::PhysicalCell physicalFlipflop =
                            phDesign.getPhysicalCell(flipflop);

                        const DBUxy initCellPos =
                            phDesign.getPhysicalCell(flipflop)
                                .getInitialPosition();
                        const double distance = infra->computeManhattanDistance(
                            slots[target].pos,
                            // physicalFlipflop.initialPos);
                            initCellPos);

                        double cost;
                        if (distance < infra->getMaxDisplacement() ||
                            !infra->isMaxDisplacementConstraintEnabled()) {
                                Rsyn::Pin data = timer->getDataPin(flipflop);

                                const double elatency =
                                    slots[target].latency[Rsyn::EARLY];
                                const double llatency =
                                    slots[target].latency[Rsyn::LATE];

                                cost = timer->getPinCriticality(data,
                                                                Rsyn::EARLY) *
                                       llatency;
                                for (Rsyn::Pin pin :
                                     flipflop.allPins(Rsyn::OUT)) {
                                        Rsyn::Net net = pin.getNet();
                                        if (!net) {
                                                // skip unconnected pins (i.e.
                                                // notQ)
                                                continue;
                                        }  // end if

                                        cost -= timer->getPinCriticality(
                                                    pin, Rsyn::EARLY) *
                                                elatency;
                                }  // end for
                        } else {
                                cost = +std::numeric_limits<double>::infinity();
                        }  // end else

                        matrix(source, target) = cost;
                }  // end for
        }          // end for

        // Apply Munkres algorithm to matrix.
        Munkres<double> m;
        m.solve(matrix);

        // Unlegalize all flipflops.
        for (int i = 0; i < numSlots; i++) {
                infra->unlegalizeCell(mapping[i]);
        }  // end for

        // Replace flipflops.
        for (int source = 0; source < numSlots; source++) {
                Rsyn::Cell flipflop = mapping[source];

                // Find target slot.
                int targetSlot = -1;
                for (int target = 0; target < numSlots; target++) {
                        // [NOTE] The floating point comparison using == was in
                        // the example
                        // oh the library, so it should be fine.
                        if (matrix(source, target) == 0) {
                                targetSlot = target;
                                break;
                        }  // end if
                }          // end for

                if (targetSlot != -1) {
                        const DBUxy initCellPos =
                            phDesign.getPhysicalCell(flipflop)
                                .getInitialPosition();
                        if (infra->isMaxDisplacementConstraintEnabled() &&
                            infra->computeManhattanDistance(
                                slots[targetSlot].pos, initCellPos) >
                                infra->getMaxDisplacement()) {
                                std::cout << "[BUG] Assignment problem should "
                                             "not provide a solution that "
                                             "violates max displacement.\n";
                        }  // end if

                        const bool success =
                            infra->moveCell(flipflop, slots[targetSlot].pos,
                                            LEG_NEAREST_WHITESPACE);
                        if (!success) {
                                std::cout
                                    << "[BUG] Ops, unable to move a flipflop "
                                    << flipflop.getName() << " to the slot.\n";
                        }  // end if
                } else {
                        std::cout << "[BUG] Ops, no target slot was found to "
                                     "place the flipflop.\n";
                }  // end else
        }          // end for

#if DEBUG
        for (int row = 0; row < numSlots; row++) {
                int rowcount = 0;
                for (int col = 0; col < numSlots; col++) {
                        if (matrix(row, col) == 0) rowcount++;
                }  // end for
                if (rowcount != 1)
                        std::cerr << "Row " << row << " has " << rowcount
                                  << " columns that have been matched."
                                  << std::endl;
        }  // end for

        for (int col = 0; col < numSlots; col++) {
                int colcount = 0;
                for (int row = 0; row < numSlots; row++) {
                        if (matrix(row, col) == 0) colcount++;
                }  // end for
                if (colcount != 1)
                        std::cerr << "Column " << col << " has " << colcount
                                  << " rows that have been matched."
                                  << std::endl;
        }  // end for
#endif
}  // end method

// -----------------------------------------------------------------------------

void EarlyOpto::runEarlySpreadingIterative(const bool dontMoveLcbs) {
        const int maxNumFails = 10;
        const int maxNumFailsInSequence = 3;

        if (FloatingPoint::approximatelyZero(timer->getTns(Rsyn::EARLY)))
                return;

        // For early optimization let's allow the QoR to degraded in other to
        // provide some hill climbing. Don't worry, the session keep track of
        // the
        // best solution found.
        int fails = 0;
        int failsInSequence = 0;
        while (1) {
                const double previousQoR = infra->getQualityScore();

                if (runEarlySpreading(dontMoveLcbs) == 0) {
                        std::cout << "No cell were moved. Skipping...\n";
                        break;
                }  // end if

                timer->updateTimingIncremental();
                infra->updateQualityScore();
                infra->reportSummary("iterative-spreading");

                infra->updateTDPSolution(true);

                // If the quality score does not change or gets worse, increment
                // the
                // fail count.
                if (!FloatingPoint::definitelyGreaterThan(
                        infra->getQualityScore(), previousQoR)) {
                        ++fails;
                        ++failsInSequence;
                        std::cout << "#fails: " << fails << "/" << maxNumFails
                                  << "\n";
                        std::cout << "#fails in sequence: " << failsInSequence
                                  << "/" << maxNumFailsInSequence << "\n";
                        if (fails >= maxNumFails ||
                            failsInSequence >= maxNumFailsInSequence) {
                                break;
                        }  // end if
                } else {
                        failsInSequence = 0;
                }  // end else

                // Stops if the TNS has been zeroed.
                if (FloatingPoint::approximatelyZero(
                        timer->getTns(Rsyn::EARLY)))
                        break;
        }  // end while

        infra->restoreBestSolution();
}  // end method

// -----------------------------------------------------------------------------

int EarlyOpto::runEarlySpreading(const bool dontMoveLcbs) {
        const bool debug = false;

        std::deque<std::tuple<Number, Rsyn::Cell>> cells;

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (cell.isPort()) continue;

                const Number criticalityEarly =
                    timer->getCellCriticality(cell, Rsyn::EARLY);
                if (FloatingPoint::notApproximatelyZero(criticalityEarly)) {
                        if (!cell.isFixed() &&
                            (!instance.isLCB() || !dontMoveLcbs))
                                cells.push_back(
                                    std::make_tuple(criticalityEarly, cell));
                }  // end if
        }          // end for

        std::sort(cells.begin(), cells.end());

        static const int N = 4;
        static const double DX[N] = {0, 0, +1, -1};
        static const double DY[N] = {+1, -1, 0, 0};

        static const int K = 5;
        static const double STEP[K] = {0.05, 0.1, 0.25, 0.5, 1.0};

        int counter = 0;
        const int threshold = std::max((size_t)100, cells.size() / 100);
        Rsyn::PhysicalModule phModule = phDesign.getPhysicalModule(module);
        const Bounds &coreBounds = phModule.getBounds();
        const int numCells = cells.size();
        for (int i = numCells - 1; i >= 0; i--) {
                Rsyn::Cell cell = std::get<1>(cells[i]);
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const double originalCost = runEarlySpreading_ComputeCost(cell);
                const DBUxy originalPos = phCell.getCenter();

                if (debug) {
                        std::cout << "Cell " << cell.getName() << " ("
                                  << cell.getLibraryCellName() << ") "
                                  << "original_cost=" << originalCost << " "
                                  << "original_pos=" << originalPos << "\n";
                }  // end if

                double bestCost = +std::numeric_limits<double>::infinity();
                DBUxy bestPosition;

                for (int k = 0; k < K; k++) {
                        const double alpha = STEP[k];

                        int counterSuccessfulMoves = 0;
                        for (int i = 0; i < N; i++) {
                                const double dx = DX[i];
                                const double dy = DY[i];

                                const DBUxy targetPos = coreBounds.closestPoint(
                                    originalPos +
                                    (alpha * double2(dx, dy) *
                                     infra->getMaxDisplacement())
                                        .convertToDbu());

                                if (infra->moveCell(cell, targetPos,
                                                    LEG_NEAREST_WHITESPACE)) {
                                        counterSuccessfulMoves++;

                                        // Update timing around the cell.
                                        routingEstimator->updateRouting();
                                        timer->updateTimingLocally(cell);

                                        const double cost =
                                            runEarlySpreading_ComputeCost(cell);
                                        if (cost < bestCost) {
                                                bestCost = cost;
                                                // The final cell position may
                                                // be different than the
                                                // targetPos due to
                                                // legalization. That's why we
                                                // copy
                                                // the cell position to the
                                                // bestPosition.
                                                Rsyn::PhysicalCell phCell =
                                                    phDesign.getPhysicalCell(
                                                        cell);
                                                bestPosition =
                                                    phCell.getPosition();
                                        }  // end else

                                        if (debug) {
                                                Rsyn::PhysicalCell phCell =
                                                    phDesign.getPhysicalCell(
                                                        cell);
                                                std::cout
                                                    << "dx=" << dx << " "
                                                    << "dy=" << dy << " "
                                                    << "target_pos="
                                                    << targetPos << " "
                                                    << "actual_pos="
                                                    << phCell.getPosition()
                                                    << " "
                                                    << "cost=" << cost << " "
                                                    << "best_cost=" << bestCost
                                                    << "\n";
                                        }  // end for

                                }  // end if
                        }          // end for

                        if (bestCost < originalCost) break;
                }  // end for

                if (bestCost < originalCost) {
                        if (debug) {
                                std::cout << "Moving cell to " << bestPosition
                                          << "...\n";
                        }  // end if

                        if (!infra->moveCell(cell, bestPosition,
                                             LEG_NEAREST_WHITESPACE)) {
                                std::cout << "[BUG] Unable to move cell to its "
                                             "target position.\n";
                        }  // end method

                        counter++;
                } else {
                        if (debug) {
                                std::cout << "Keeping the cell in its original "
                                             "position "
                                          << originalPos << "...\n";
                        }  // end if

                        // Move cell back to its original position.
                        if (!infra->moveCell(cell, originalPos,
                                             LEG_NEAREST_WHITESPACE)) {
                                std::cout << "[BUG] Unable to move cell back "
                                             "to its original position.\n";
                        }  // end for
                }          // end else

                routingEstimator->updateRouting();
                timer->updateTimingLocally(cell);

                if (counter >= threshold) {
                        // several cells changed, maybe it's a good idea to
                        // update timing
                        break;
                }  // end if
        }          // end for

        return counter;
}  // end method

// -----------------------------------------------------------------------------

double EarlyOpto::runEarlySpreading_ComputeCost(Rsyn::Cell cell) {
        double cost = 0;

        if (cell.isSequential()) {
                Rsyn::Pin dpin = timer->getDataPin(cell);
                cost += timer->getPinCriticality(dpin, Rsyn::EARLY);
        }  // end if

        for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
                Rsyn::Net net = pin.getNet();
                if (!net) continue;

                cost += timer->getPinCriticality(pin, Rsyn::EARLY);

                //		// Add the criticality of the sink pin if it is
                // an
                // endpoint.
                //		for (Rsyn::Pin sink : net.allPins(Rsyn::SINK)) {
                //			if (sink.isPort() || isSequential(sink))
                //{
                //				cost +=
                // timer.getPinCriticality(sink,
                // Rsyn::EARLY);
                //			} // end if
                //		} // end if
        }  // end for

        //	// Get the wire delay at all local sink pins (side cells and
        // sink cells).
        //	for (Rsyn::Pin pin : cell.allPins()) {
        //		if (pin.isClocked())
        //			continue;
        //
        //		Rsyn::Net net = pin.getNet();
        //		if (net) {
        //			for (Rsyn::Pin sink : net.allPins()) {
        //				cost +=  timer.getPinCriticality(sink,
        // Rsyn::EARLY);
        //			} // end for
        //		} // end if
        //	} // end for

        return cost;
}  // end method

}  // end namescape
