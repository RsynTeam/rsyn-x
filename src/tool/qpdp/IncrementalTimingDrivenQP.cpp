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

#include <limits>
#include <thread>

#include "session/Session.h"
#include "tool/library/LibraryCharacterizer.h"
#include "tool/routing/RoutingEstimator.h"
#include "iccad15/Infrastructure.h"
#include "IncrementalTimingDrivenQP.h"
#include "util/FloatingPoint.h"
#include "util/AsciiProgressBar.h"
#include "util/Environment.h"
#include "util/StreamStateSaver.h"

namespace ICCAD15 {

void IncrementalTimingDrivenQP::setSession(Rsyn::Session ptr) {
        session = ptr;
        infra = session.getService("ufrgs.ispd16.infra");
        timer = session.getService("rsyn.timer");
        routingEstimator = session.getService("rsyn.routingEstimator");
        libc = session.getService("rsyn.libraryCharacterizer");
        design = session.getDesign();
        module = design.getTopModule();
        mapCellToIndex = design.createAttribute();
        delayRatio = design.createAttribute();
        phDesign = session.getPhysicalDesign();

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                mapCellToIndex[cell] = -1;
                delayRatio[cell] = 1.0;
        }
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::refreshCellsDelayRatio() {
        // Ignore for now...
        return;
        //	for (Rsyn::Instance instance : design.allCells()) {
        //		Rsyn::Cell cell  = instance.asCell(); // TODO: hack,
        // assuming that the instance is a cell
        //		delayRatio[ cell ] = computeCellDelayRatio( cell );
        //	} // end for
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::buildMapping() {
        mapIndexToCell.clear();

        mapIndexToCell.resize(movable.size());

        int index = 0;
        for (Rsyn::Cell cell : movable) {
                mapCellToIndex[cell] = index;
                mapIndexToCell[index] = cell;
                index++;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::copyCellsLocationFromSessionToLinearSystem() {
        Stepwatch watch(
            "[Quadratic Placement] Copying positions to Linear System.");
        const int numMovableElements = movable.size();

        px.clear();
        py.clear();
        px.assign(numMovableElements, 0);
        py.assign(numMovableElements, 0);

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                const int index = mapCellToIndex[cell];

                if (index < 0) continue;

                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const DBUxy cellPos = phCell.getPosition();
                px[index] = cellPos[X];
                py[index] = cellPos[Y];
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::copyCellsLocationFromLinearSystemToSession() {
        Stepwatch watch("[Quadratic Placement] Copying positions to design.");

        std::vector<Rsyn::Timer::PathHop> path;
        timer->queryTopCriticalPath(Rsyn::LATE, path);
        const double pathLengthBefore = infra->computePathManhattanLength(path);

        AsciiProgressBar progressBar(mapIndexToCell.size());

        const bool moveTentatively = false;

        // double minDisplacement = 4 * session.getRows()[0].height();
        double minDisplacement = 4 * phDesign.getRowHeight();
        std::cout << "[Quadratic Placement] minDisplacement = "
                  << minDisplacement << "\n";

        initialPositions.assign(mapIndexToCell.size(),
                                DBUxy(std::numeric_limits<DBU>::quiet_NaN(),
                                      std::numeric_limits<DBU>::quiet_NaN()));

        std::vector<std::tuple<Number, Rsyn::Cell, int>> orderedCells;
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                const int index = mapCellToIndex[cell];

                if (index < 0) continue;

                const Number slack = timer->getCellWorstSlack(cell, Rsyn::LATE);
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const double x = px[index];
                const double y = py[index];
                const double2 newPos(x, y);
                const double2 oldPos = double2(phCell.getPosition());

                if (oldPos.approximatelyEqual(newPos, minDisplacement)) {
                        continue;
                }  // enf if

                //		double2 displacement = newPos - oldPos;
                //		displacement.abs();
                //		displacement.x = -displacement.x;
                //		displacement.y = -displacement.y;
                orderedCells.push_back(std::make_tuple(slack, cell, index));
        }

        std::sort(orderedCells.begin(), orderedCells.end());

        double averageDisplacement = 0.0;
        int numMoved = 0;
        for (const std::tuple<Number, Rsyn::Cell, int>& e : orderedCells) {
                Rsyn::Cell cell = std::get<1>(e);
                const int index = std::get<2>(e);
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const double x = px[index];
                const double y = py[index];
                const double2 newPos(x, y);
                const DBUxy oldPos = phCell.getPosition();
                initialPositions[index] = oldPos;

                if (moveTentatively) {
                        infra->moveCellTowards(cell, newPos.convertToDbu(),
                                               legMode, 0.1);
                } else {
                        if (!infra->moveCell(cell, newPos.convertToDbu(),
                                             legMode, true)) {
                                if (!infra->moveCell(cell, oldPos, legMode,
                                                     true)) {
                                        if (!infra->moveCell(
                                                cell, oldPos,
                                                LegalizationMethod::
                                                    LEG_MIN_LINEAR_DISPLACEMENT,
                                                true)) {
                                                std::cout
                                                    << "[WARNING]: "
                                                       "Legalization error at "
                                                    << __FILE__ << ": "
                                                    << __LINE__ << "\n";
                                        }  // end if
                                }          // end if
                        }                  // end if
                }                          // end if

                Rsyn::PhysicalCell phCell2 = phDesign.getPhysicalCell(cell);
                const double2 cellPos = double2(phCell2.getPosition());
                averageDisplacement += std::abs(x - cellPos[X]);
                averageDisplacement += std::abs(y - cellPos[Y]);

                numMoved++;
                progressBar.progress();

                if (debugMode) {
                        std::cout << "x=(" << cellPos[X] << ", " << x << ") "
                                  << "y=(" << cellPos[Y] << ", " << y << ")\n";
                }  // end if
        }          // end for

        averageDisplacement /= numMoved;
        // averageDisplacement /= session.getRows()[0].height();
        averageDisplacement /= (double)phDesign.getRowHeight();

        cout << "\n\tAverage legalization disp = " << averageDisplacement
             << " rows\n";

        timer->updatePath(path);
        infra->computePathManhattanLength(path);
        const double pathLengthAfter = infra->computePathManhattanLength(path);

        if (averageDisplacement) {
                std::cout << "**** Path "
                          << path.front().getPin().getFullName();
                std::cout << " -> " << path.back().getPin().getFullName()
                          << " ****\n";
                std::cout << " Length before: " << pathLengthBefore << "\n";
                std::cout << " Length now: " << pathLengthAfter << "\n";
                std::cout << " Change (%): ";
                std::cout << std::fixed;
                std::cout << std::setprecision(2);
                std::cout << 100 * pathLengthAfter / pathLengthBefore << "\n";
        }  // end if

        progressBar.progress();
        std::cout << "\n";
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::
    copyCellsLocationFromLinearSystemToSessionOptimized() {
        Stepwatch watch("[Quadratic Placement] Copying positions to design.");

        AsciiProgressBar progressBar(mapIndexToCell.size());

        double minDisplacement = 4 * phDesign.getRowHeight();
        std::cout << "[Quadratic Placement] minDisplacement = "
                  << minDisplacement << "\n";

        initialPositions.assign(mapIndexToCell.size(),
                                DBUxy(std::numeric_limits<DBU>::quiet_NaN(),
                                      std::numeric_limits<DBU>::quiet_NaN()));

        std::vector<std::tuple<Number, Rsyn::Cell, int>> orderedCells;
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                const int index = mapCellToIndex[cell];

                if (index < 0) continue;

                const Number slack = timer->getCellWorstSlack(cell, Rsyn::LATE);
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const double x = px[index];
                const double y = py[index];
                const double2 newPos(x, y);
                const double2 oldPos = double2(phCell.getPosition());

                if (oldPos.approximatelyEqual(newPos, minDisplacement)) {
                        continue;
                }  // enf if

                orderedCells.push_back(std::make_tuple(slack, cell, index));
        }

        std::sort(orderedCells.begin(), orderedCells.end());

        double averageDisplacement = 0.0;
        int numMoved = 0;
        for (const std::tuple<Number, Rsyn::Cell, int>& e : orderedCells) {
                const Number slack = std::get<0>(e);
                Rsyn::Cell cell = std::get<1>(e);
                const int index = std::get<2>(e);
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const double x = px[index];
                const double y = py[index];
                double2 newPos(x, y);
                const DBUxy oldPos = phCell.getPosition();
                initialPositions[index] = oldPos;

                if (!infra->moveCell(cell, newPos.convertToDbu(), legMode,
                                     true)) {
                        if (!infra->moveCell(cell, oldPos, legMode, true)) {
                                if (!infra->moveCell(
                                        cell, oldPos,
                                        LegalizationMethod::
                                            LEG_MIN_LINEAR_DISPLACEMENT,
                                        true)) {
                                        std::cout << "[WARNING]: Legalization "
                                                     "error at "
                                                  << __FILE__ << ": "
                                                  << __LINE__ << "\n";
                                }  // end if
                        }          // end if
                }                  // end if

                Rsyn::PhysicalCell phCell2 = phDesign.getPhysicalCell(cell);
                const double2 cellPos = double2(phCell2.getPosition());
                averageDisplacement += std::abs(x - cellPos[X]);
                averageDisplacement += std::abs(y - cellPos[Y]);

                numMoved++;
                progressBar.progress();

                if (debugMode) {
                        std::cout << "x=(" << cellPos[X] << ", " << x << ") "
                                  << "y=(" << cellPos[Y] << ", " << y << ")\n";
                }  // end if
        }          // end for

        averageDisplacement /= numMoved;
        averageDisplacement /= (double)phDesign.getRowHeight();

        cout << "\n\tAverage legalization disp = " << averageDisplacement
             << " rows\n";

        const bool debugPathImprovement = true;
        if (debugPathImprovement) {
                std::cout << "\"debugPathImprovement\" enabled. It will cause "
                             "performance loss..\n";
                timer->updateTimingIncremental();
                if (averageDisplacement) {
                        std::vector<Rsyn::Timer::PathHop> path;
                        timer->queryTopCriticalPath(Rsyn::LATE, path);
                        //			Rsyn::Pin endpoint =
                        // design.findPinByName("A2_B4_C2_D17_o724686:d");
                        //			timer->queryTopCriticalPathsFromEndpoint(Rsyn::LATE,
                        // endpoint, 1, path );
                        const double pathLength =
                            infra->computePathManhattanLength(path);

                        infra->reportPathManhattanLengthBySegment(path);
                        std::cout << "**** Path "
                                  << path.front().getPin().getFullName();
                        std::cout << " -> "
                                  << path.back().getPin().getFullName()
                                  << " ****\n";
                        std::cout << " Length : " << pathLength << "\n";
                }  // end if
        }

        progressBar.progress();
        std::cout << "\n";
}

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::neutralizeSystem() {
        Stepwatch watch("[Quadratic Placement] Neutralizing system");
        ::Mul(a, px, bx);
        ::Mul(a, py, by);
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::neutralizeSystemWithSpreadingForces() {
        Stepwatch watch("[Quadratic Placement] Neutralizing system");

        for (int i = 0; i < a.GetDimension(); i++) a.AddDiagonalElement(i, .1);

        std::vector<double> nx(px.size(), 0.0);
        std::vector<double> ny(py.size(), 0.0);

        ::Mul(a, px, nx);
        ::Mul(a, py, ny);

        std::vector<double> fx(px.size(), 0.0);
        std::vector<double> fy(py.size(), 0.0);

        ::Sub(nx, bx, fx);
        ::Sub(ny, by, fy);

        ::Add(fx, bx, bx);
        ::Add(fy, by, by);
}

// -----------------------------------------------------------------------------

void generateRetentionForces(std::vector<double>& fx, std::vector<double>& fy) {

}

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::addAnchor(const Rsyn::Cell c, const DBUxy pos,
                                          const double w) {
        const int index = mapCellToIndex[c];

        if (index < 0) return;

        fw[index] += w;
        fx[index] += w * pos.x;
        fy[index] += w * pos.y;
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runQuadraticPlacement(
    const std::set<Rsyn::Cell>& movable,
    const LegalizationMethod legalizationMethod,
    const bool roolbackOnMaxDisplacementViolation) {
        std::cout << "Deprecated..." << std::endl;
        return;

        // Compute retention forces.
        //::Sub(nx, bx0, fx);
        //::Sub(ny, by0, fy);

        // Add the retention forces to the stressed linear system.
        //::Add(bx1, fx, bx1);
        //::Add(by1, fy, by1);

        // Solve 'not' stressed linear system.
        std::thread tx([&]() { SolveByConjugateGradient(a, bx, px, 1000); });
        std::thread ty([&]() { SolveByConjugateGradient(a, by, py, 1000); });

        tx.join();
        ty.join();
}  // end method

// -----------------------------------------------------------------------------

/* Remove all spreading forces from the system. */
void IncrementalTimingDrivenQP::removeAnchors() {
        fw.clear();
        fx.clear();
        fy.clear();
        fw.assign(mapIndexToCell.size(), 0);
        fx.assign(mapIndexToCell.size(), 0);
        fy.assign(mapIndexToCell.size(), 0);
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::buildLinearSystem(const bool stress) {
        Stepwatch watch("[Quadratic Placement] Building Linear system");

        // Clear fixed forces vector
        bx.clear();
        by.clear();
        bx.assign(mapIndexToCell.size(), 0);
        by.assign(mapIndexToCell.size(), 0);

        // Clear anchors
        removeAnchors();

        // Define the nets that connect at least one movable element.
        std::set<Rsyn::Net> nset;

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell

                if (mapCellToIndex[cell] < 0) continue;

                for (Rsyn::Pin pin : cell.allPins()) {
                        Rsyn::Net net = pin.getNet();
                        if (net) {
                                if (net == timer->getClockNet()) continue;

                                Rsyn::Pin driver = net.getAnyDriver();
                                Rsyn::Instance instance = driver.getInstance();
                                if (driver && instance.isLCB()) continue;

                                nset.insert(net);
                        }  // end if
                }          // end for
        }                  // end for

        for (Rsyn::Net net : nset) {
                Rsyn::Pin driver = net.getAnyDriver();

                double R = 1;

                if (enableRC) {
                        const double driverResistance =
                            driver
                                ? libc->getCellMaxDriverResistance(
                                      driver.getInstance(), Rsyn::LATE)
                                : 1;
                        R = 0.1 * driverResistance;  // * loadCapacitance;
                }

                R = std::max(R, minimumResistance);

                for (Rsyn::Pin pin0 : net.allPins()) {
                        Rsyn::Cell cell0 = pin0.getInstance().asCell();
                        const int index0 = mapCellToIndex[cell0];

                        if (index0 == -1) continue;

                        for (Rsyn::Pin pin1 : net.allPins()) {
                                if (pin0 == pin1) continue;

                                Rsyn::Cell cell1 = pin1.getInstance().asCell();
                                const int index1 = mapCellToIndex[cell1];

                                if ((index0 != -1 && index1 != -1) &&
                                    index0 >= index1)
                                        continue;

                                double weight =
                                    R * double(1) /
                                    std::max(1, (net.getNumPins() - 1));

                                if (stress) {
                                        weight *= 1 +
                                                  timer->getNetCriticality(
                                                      net, Rsyn::LATE);
                                }  // end if

                                if (index0 != -1 && index1 != -1) {
                                        // add movable-to-movable connection
                                        dscp.AddElement(index0, index1,
                                                        -weight);
                                        dscp.AddElement(index1, index0,
                                                        -weight);

                                        dscp.AddDiagonalElement(index0, weight);
                                        dscp.AddDiagonalElement(index1, weight);
                                } else if (index0 != -1) {
                                        // add movable-to-fixed connection
                                        const DBUxy pos1 =
                                            phDesign.getPinPosition(pin1);
                                        bx[index0] += weight * pos1.x;
                                        by[index0] += weight * pos1.y;

                                        dscp.AddDiagonalElement(index0, weight);
                                } else if (index1 != -1) {
                                        // add fixed-to-movable connection
                                        const DBUxy pos0 =
                                            phDesign.getPinPosition(pin0);
                                        bx[index1] += weight * pos0.x;
                                        by[index1] += weight * pos0.y;

                                        dscp.AddDiagonalElement(index1, weight);
                                }  // end if
                        }          // end for
                }                  // end for
        }                          // end for

        a.Reset();

#ifdef DEBUG
        a.Initialize(dscp, true);
#else
        a.Initialize(dscp, false);
#endif

        dscp.Clear();
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::solveLinearSystem(bool spreadingForces) {
        Stepwatch watch("[Quadratic Placement] Solving Linear System.");
        // Creates local variables for the system...
        SquareCompressedRowMatrix& a = this->a;
        std::vector<double>& bx = this->bx;
        std::vector<double>& by = this->by;

        // Add spreading forces to the system if enabled...
        if (spreadingForces) {
                for (int i = 0; i < bx.size(); i++) {
                        a.AddDiagonalElement(i, fw[i]);
                        bx[i] += fx[i];
                        by[i] += fy[i];
                }  // end for
        }          // end if

        // Solves linear system...
        //	SquareCompressedRowMatrix u, l;
        //	Cholesky( a, u, l );
        //	SolveByConjugateGradientWithPCond(a, bx, px, 1000, 1e-5, u, l);
        //	SolveByConjugateGradientWithPCond(a, by, py, 1000, 1e-5, u, l);

        std::thread tx([&]() {
                ::SolveByConjugateGradientWithDiagonalPreConditioner(a, bx, px,
                                                                     1000);
        });

        std::thread ty([&]() {
                ::SolveByConjugateGradientWithDiagonalPreConditioner(a, by, py,
                                                                     1000);
        });

        tx.join();
        ty.join();
}

// -----------------------------------------------------------------------------

bool IncrementalTimingDrivenQP::canBeSetAsMovable(
    Rsyn::Cell cell, const bool considerCriticalCellsAsFixed) {
        Rsyn::PhysicalCell physicalCell = phDesign.getPhysicalCell(cell);
        return !cell.isPort() && !cell.isFixed() && !cell.isSequential() &&
               !cell.isLCB() && !cell.isMacroBlock() &&
               !(considerCriticalCellsAsFixed &&
                 timer->getCellCriticality(cell, Rsyn::LATE) > 0);
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::markAsMovableAllNonFixedCells(
    const bool considerCriticalCellsAsFixed) {
        movable.clear();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (canBeSetAsMovable(cell, considerCriticalCellsAsFixed)) {
                        movable.insert(cell);
                }  // end if
        }          // end for
}  // end for

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::markAsMovableAllNonFixedCriticalCells() {
        movable.clear();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (canBeSetAsMovable(cell, false) &&
                    timer->getCellCriticality(cell, Rsyn::LATE) > 0) {
                        movable.insert(cell);
                }  // end if
        }          // end for
}  // end for

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::
    markAsMovableAllNonFixedNonCriticalCellsDrivenByCriticalNets() {
        movable.clear();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (canBeSetAsMovable(cell, false) &&
                    FloatingPoint::approximatelyZero(
                        timer->getCellCriticality(cell, Rsyn::LATE))) {
                        // Check if its driven by a critical net.
                        bool drivenByCriticalNet = false;
                        for (Rsyn::Pin pin : cell.allPins(Rsyn::IN)) {
                                Rsyn::Net net = pin.getNet();
                                if (net &&
                                    timer->getNetCriticality(net, Rsyn::LATE) >
                                        0) {
                                        drivenByCriticalNet = true;
                                        break;
                                }  // end if
                        }          // end for

                        if (drivenByCriticalNet) movable.insert(cell);
                }  // end if
        }          // end for
}  // end for

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::
    markAsMovableAllNonFixedCriticalCellsAndTheirSinks() {
        movable.clear();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell currentCell =
                    instance.asCell();  // TODO: hack, assuming that the
                                        // instance is a cell
                if (timer->getCellCriticality(currentCell, Rsyn::LATE) > 0) {
                        for (Rsyn::Pin driver :
                             currentCell.allPins(Rsyn::OUT)) {
                                Rsyn::Net net = driver.getNet();
                                if (net) {
                                        for (Rsyn::Pin pin : net.allPins()) {
                                                Rsyn::Cell cell =
                                                    pin.getInstance().asCell();
                                                if (canBeSetAsMovable(cell,
                                                                      false)) {
                                                        movable.insert(cell);
                                                }  // end if
                                        }          // end for
                                }                  // end if
                        }                          // end for
                }                                  // end if
        }                                          // end for
}  // end for

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::
    markAsMovableAllNonFixedCriticalCellsAndTheirNeighbors() {
        movable.clear();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell currentCell =
                    instance.asCell();  // TODO: hack, assuming that the
                                        // instance is a cell
                if (timer->getCellCriticality(currentCell, Rsyn::LATE) > 0) {
                        for (Rsyn::Pin cellPin : currentCell.allPins()) {
                                Rsyn::Net net = cellPin.getNet();
                                if (net) {
                                        for (Rsyn::Pin netPin : net.allPins()) {
                                                Rsyn::Cell cell =
                                                    netPin.getInstance()
                                                        .asCell();
                                                if (canBeSetAsMovable(cell,
                                                                      false)) {
                                                        movable.insert(cell);
                                                }  // end if
                                        }          // end for
                                }                  // end if
                        }                          // end for
                }                                  // end if
        }                                          // end for
}  // end for

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::markAsMovableAllNonFixedCellsInTopCriticalPaths(
    const int numPaths) {
        movable.clear();

        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        timer->queryTopCriticalPaths(Rsyn::LATE, numPaths, paths);

        const int actualNumPaths = paths.size();
        for (int i = 0; i < actualNumPaths; i++) {
                std::vector<Rsyn::Timer::PathHop>& path = paths[i];
                for (const Rsyn::Timer::PathHop& hop : path) {
                        Rsyn::Cell cell = hop.getInstance().asCell();
                        if (canBeSetAsMovable(cell, false)) {
                                movable.insert(cell);
                        }  // end if
                }          // end for
        }                  // end for
}  // end for

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::
    markAsMovableAllNonFixedCellsInTopCriticalPathsAndTheirNeighbors(
        const int numPaths) {
        movable.clear();

        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        timer->queryTopCriticalPaths(Rsyn::LATE, numPaths, paths);

        const int actualNumPaths = paths.size();
        for (int i = 0; i < actualNumPaths; i++) {
                std::vector<Rsyn::Timer::PathHop>& path = paths[i];
                for (const Rsyn::Timer::PathHop& hop : path) {
                        Rsyn::Cell currentCell = hop.getInstance().asCell();
                        for (Rsyn::Pin cellPin : currentCell.allPins()) {
                                Rsyn::Net net = cellPin.getNet();
                                if (net) {
                                        for (Rsyn::Pin netPin : net.allPins()) {
                                                Rsyn::Cell cell =
                                                    netPin.getInstance()
                                                        .asCell();
                                                if (canBeSetAsMovable(cell,
                                                                      false)) {
                                                        movable.insert(cell);
                                                }  // end if
                                        }          // end for
                                }                  // end if
                        }                          // end for
                }                                  // end for
        }                                          // end for
}  // end for

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runCriticalPathSmoothing() {
        std::cout << "Deprecated..." << std::endl;

        return;

        const int N = 300;

        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        timer->queryTopCriticalPaths(Rsyn::LATE, N, paths);
        // timer->queryTopCriticalPathFromTopCriticalEndpoints(Rsyn::LATE, N,
        // paths);

        const int numPaths = paths.size();
        if (numPaths > 0) {
                StreamStateSaver sss(std::cout);

                markAsMovableAllNonFixedCellsInTopCriticalPathsAndTheirNeighbors(
                    numPaths);
                // markAsMovableCellsInTopCriticalPaths(numPaths);
                // markAsMovableAllNonFixedCellsInTopCriticalPathsAndTheirNeighbors(numPaths);
                // markAsMovableAllNonFixedCells();
                // markAsMovableAllNonFixedCriticalCellsAndTheirSinks();
                // markAsMovableAllNonFixedCriticalCellsAndTheirNeighbors();

                std::cout << "Quadratic placement smoothing...\n";
                buildMapping();
                runQuadraticPlacement(movable, LEG_NEAREST_WHITESPACE, true);
                timer->updateTimingIncremental();
                infra->updateQualityScore();
                std::cout << "Quadratic placement smoothing... Done\n";

                for (int i = 0; i < numPaths; i++) {
                        std::vector<Rsyn::Timer::PathHop>& path = paths[i];
                        const double oldSlack = path.back().getSlack();
                        timer->updatePath(path);
                        const double newSlack = path.back().getSlack();
                        std::cout << std::setw(10) << oldSlack << " "
                                  << std::setw(10) << newSlack << std::setw(10)
                                  << (100 * (1 - newSlack / oldSlack)) << "%\n";
                }  // end for

                sss.restore();
                infra->reportDigest();
        } else {
                std::cout << "No critical path to be fixed.\n";
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::generateLoadAnchors() {
        Stepwatch watch("[Quadratic Placement] Generating anchors.");

        criticalNets.clear();

        //	for ( Rsyn::Cell cell : design.allCells() ) {
        //		if( !canBeSetAsMovable( cell, true ) )
        //				continue;
        //
        //		DBUxy target =
        //			session.getCellBounds( cell ).lower();
        //
        //		addAnchor( cell, target, 0.5 );
        //	}

        for (Rsyn::Net net : module.allNets()) {
                double criticality = timer->getNetCriticality(net, Rsyn::LATE);

                if (criticality > 0) {
                        Rsyn::Cell driver =
                            net.getAnyDriver().getInstance().asCell();
                        double slack = timer->getCellWorstNegativeSlack(
                            driver, Rsyn::LATE);
                        criticalNets.push_back(std::make_pair(slack, net));
                }  // end if
        }          // end for

        std::sort(criticalNets.begin(), criticalNets.end());

        int violations = 0;
        int total = 0;
        for (auto& it : criticalNets) {
                Rsyn::Net net = std::get<1>(it);
                Rsyn::Pin dPin = net.getAnyDriver();
                double criticality = timer->getPinCriticality(dPin, Rsyn::LATE);
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(dPin);
                DBUxy target = phCell.getCenter();

                for (Rsyn::Pin pin : net.allPins(Rsyn::IN)) {
                        if (timer->getPinCriticality(pin, Rsyn::LATE) > 0)
                                continue;

                        Rsyn::Cell cell = pin.getInstance().asCell();

                        if (!canBeSetAsMovable(cell, true)) continue;

                        total++;
                        addAnchor(cell, target, 1);
                }  // end for
        }          // end for
}

// -----------------------------------------------------------------------------

double IncrementalTimingDrivenQP::estimatePathSize(
    std::vector<Rsyn::Timer::PathHop>& pathHops) {
        double size = 0.0;

        const int numHops = pathHops.size();

        for (int i = 4; i < numHops - 1; i++) {
                const Rsyn::Pin currPin = pathHops[i].getPin();
                const Rsyn::Cell currCell = currPin.getInstance().asCell();
                const Rsyn::Pin nextPin = pathHops[i + 1].getPin();
                const Rsyn::Cell nextCell = nextPin.getInstance().asCell();

                // PhysicalPin pCurrPin = session.getPhysicalPin( currPin );
                // PhysicalCell pCurrCell = session.getPhysicalCell( currCell );
                // PhysicalPin pNextPin = session.getPhysicalPin( nextPin );
                // PhysicalCell pNextCell = session.getPhysicalCell( nextCell );

                // DBUxy currPinPosition = pCurrCell.lower() +
                // pCurrPin.displacement;
                // DBUxy nextPinPosition = pNextCell.lower() +
                // pNextPin.displacement;

                DBUxy currPinPosition = phDesign.getPinPosition(currPin);
                DBUxy nextPinPosition = phDesign.getPinPosition(nextPin);

                size += std::abs(currPinPosition.x - nextPinPosition.x) +
                        std::abs(currPinPosition.y - nextPinPosition.y);
        }  // end for

        return size;
}  // end method

// -----------------------------------------------------------------------------

double IncrementalTimingDrivenQP::pathAvailableArea(
    std::vector<Rsyn::Timer::PathHop>& pathHops) {
        DBU minX = std::numeric_limits<DBU>::max();
        DBU maxX = -std::numeric_limits<DBU>::max();
        DBU minY = std::numeric_limits<DBU>::max();
        DBU maxY = -std::numeric_limits<DBU>::max();

        for (int i = 0; i < pathHops.size(); i++) {
                // const PhysicalPin pin = session.getPhysicalPin(
                // pathHops[i].getPin() );
                // const PhysicalCell cell = session.getPhysicalCell(
                // pathHops[i].getPin() );

                // const double2 pinPos = cell.lower() + pin.displacement;
                const DBUxy pinPos =
                    phDesign.getPinPosition(pathHops[i].getPin());

                minX = std::min(minX, pinPos.x);
                maxX = std::max(maxX, pinPos.x);
                minY = std::min(minY, pinPos.y);
                maxY = std::max(maxY, pinPos.y);
        }  // end for

        Bounds pathBB(minX, minY, maxX, maxY);
        double totalArea = pathBB.computeArea();
        double blockedArea = 0.0;

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                PhysicalCell pCell = phDesign.getPhysicalCell(cell);
                if (!cell.isMacroBlock()) continue;

                blockedArea += pCell.getBounds().overlapArea(pathBB);
        }

        return 1 - blockedArea / totalArea;
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::generateStraighteningForces_DEPRECATED(
    const bool enableCriticality) {
        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        std::vector<std::vector<Rsyn::Timer::PathHop>> paths2;

        const Number slackThresshold = -alpha * timer->getClockPeriod();
        timer->queryTopCriticalPaths(Rsyn::LATE, maxNumPaths, paths,
                                     slackThresshold);
        std::cout << "[Quadratic Placement] #Paths " << paths.size() << "\n";

        const int numCriticalEndPoints =
            timer->getNumCriticalEndpoints(Rsyn::LATE);
        timer->queryTopCriticalPathFromTopCriticalEndpoints(
            Rsyn::LATE, numCriticalEndPoints, paths2);

        //	paths.insert( paths.end(), paths2.begin(), paths2.end() );

        std::set<std::pair<Rsyn::Cell, Rsyn::Cell>> connections;

        for (int i = 0; i < paths.size(); i++) {
                auto& criticalPathHops = paths[i];

                if (criticalPathHops.size() < 10) continue;

                double criticality = 1;

                if (enableCriticality)
                        criticality = criticalPathHops[0].getSlack() /
                                      timer->getWns(Rsyn::LATE);

                for (int i = 4; i < criticalPathHops.size() - 2; i += 2) {
                        const Rsyn::Timer::PathHop& hop = criticalPathHops[i];

                        const Rsyn::Cell currCell =
                            hop.getPin().getInstance().asCell();
                        const Rsyn::Cell nextCell =
                            hop.getNextPin().getInstance().asCell();

                        const bool connectionExists =
                            connections.find(std::make_pair(
                                currCell, nextCell)) != connections.end();

                        if (connectionExists) continue;

                        double RC = 1;

                        if (enableRC) {
                                const double driverResistance =
                                    libc->getCellMaxDriverResistance(
                                        currCell, Rsyn::LATE);
                                const double loadCapacitance =
                                    routingEstimator
                                        ->getRCTree(hop.getPin().getNet())
                                        .getLumpedCap()
                                        .getMax();
                                RC = 0.1 *
                                     driverResistance;  //  * loadCapacitance;
                        }

                        RC = std::max(RC, minimumResistance);

                        const bool currIsMovable =
                            canBeSetAsMovable(currCell, false);
                        const bool nextIsMovable =
                            canBeSetAsMovable(nextCell, false);

                        if (!currIsMovable && nextIsMovable) {
                                /*...*/
                                const int index = mapCellToIndex[nextCell];
                                // const PhysicalCell pCell =
                                // session.getPhysicalCell( currCell );
                                // const PhysicalPin pPin =
                                // session.getPhysicalPin( hop.getPin() );
                                const DBUxy pinPos =
                                    phDesign.getPinPosition(hop.getPin());
                                a.AddDiagonalElement(index, RC * 5);
                                // bx[ index ] += RC * 5 * (pCell.lower().x +
                                // pPin.dx);
                                // by[ index ] += RC * 5 * (pCell.lower().y +
                                // pPin.dy);
                                bx[index] += RC * 5 * (pinPos[X]);
                                by[index] += RC * 5 * (pinPos[Y]);

                                continue;
                        } else if (currIsMovable && !nextIsMovable) {
                                const int index = mapCellToIndex[currCell];
                                // const PhysicalCell pCell =
                                // session.getPhysicalCell( nextCell );
                                // const PhysicalPin pPin =
                                // session.getPhysicalPin( hop.getNextPin() );

                                const DBUxy pinPos =
                                    phDesign.getPinPosition(hop.getNextPin());

                                a.AddDiagonalElement(index, RC * 5);
                                // bx[ index ] += RC * 5 * (pCell.lower().x +
                                // pPin.dx);
                                // by[ index ] += RC * 5 * (pCell.lower().y +
                                // pPin.dy);
                                bx[index] += RC * 5 * (pinPos[X]);
                                by[index] += RC * 5 * (pinPos[Y]);

                                continue;
                        } else if (!currIsMovable && !nextIsMovable)
                                continue;

                        connections.insert(std::make_pair(currCell, nextCell));

                        const int index1 = mapCellToIndex[currCell];
                        const int index2 = mapCellToIndex[nextCell];

                        const double weight = RC * 5 * (1 + criticality);

                        a.AddElement(std::max(index1, index2),
                                     std::min(index1, index2), -weight);
                        a.AddElement(std::min(index1, index2),
                                     std::max(index1, index2), -weight);
                        a.AddDiagonalElement(index1, weight);
                        a.AddDiagonalElement(index2, weight);
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::generateStraighteningForcesTopCriticalPaths() {
        std::vector<Rsyn::Pin> topCriticalEndPoints;
        timer->queryTopCriticalEndpoints(
            Rsyn::LATE, timer->getNumCriticalEndpoints(Rsyn::LATE),
            topCriticalEndPoints, 0.01f * timer->getWns(Rsyn::LATE));

        std::map<std::pair<Rsyn::Pin, Rsyn::Pin>, double> edges;

        for (int i = 0; i < topCriticalEndPoints.size(); i++) {
                std::vector<std::vector<Rsyn::Timer::PathHop>> criticalPaths;
                timer->queryTopCriticalPathsFromEndpoint(
                    Rsyn::LATE, topCriticalEndPoints[i], 500, criticalPaths);

                for (int j = 0; j < criticalPaths.size(); j++) {
                        const std::vector<Rsyn::Timer::PathHop>& path =
                            criticalPaths[j];

                        const Number pathCriticality =
                            path.back().getSlack() / timer->getWns(Rsyn::LATE);

                        for (int k = 0; k < path.size() - 1; k++) {
                                const Rsyn::Timer::PathHop& hop = path[k];

                                if (hop.getPin().getDirection() == Rsyn::IN)
                                        continue;

                                const Rsyn::Cell currCell =
                                    hop.getInstance().asCell();

                                const double driverResistance =
                                    libc->getCellMaxDriverResistance(
                                        currCell, Rsyn::LATE);
                                const double R = std::max(
                                    0.1 * driverResistance, minimumResistance);

                                const double weight =
                                    5 * R * (1 + pathCriticality);

                                const std::pair<Rsyn::Pin, Rsyn::Pin> key(
                                    hop.getPin(), hop.getNextPin());

                                const auto edge = edges.find(key);

                                if (edge != edges.end())
                                        edge->second =
                                            std::max(edge->second, weight);
                                else
                                        edges.emplace(key, weight);
                        }  // end for
                }          // end for
        }                  // end for

        for (const auto& edge : edges) {
                const Rsyn::Pin p0 = edge.first.first;
                const Rsyn::Pin p1 = edge.first.second;
                const double weight = edge.second;

                const int index0 = mapCellToIndex[p0.getInstance()];
                const int index1 = mapCellToIndex[p1.getInstance()];

                if (index0 != -1 && index1 != -1) {
                        // add movable-to-movable connection
                        a.AddElement(index0, index1, -weight);
                        a.AddElement(index1, index0, -weight);

                        a.AddDiagonalElement(index0, weight);
                        a.AddDiagonalElement(index1, weight);
                } else if (index0 != -1) {
                        // add movable-to-fixed connection
                        const DBUxy pos1 = phDesign.getPinPosition(p1);
                        bx[index0] += weight * pos1.x;
                        by[index0] += weight * pos1.y;

                        a.AddDiagonalElement(index0, weight);
                } else if (index1 != -1) {
                        // add fixed-to-movable connection
                        const DBUxy pos0 = phDesign.getPinPosition(p0);
                        bx[index1] += weight * pos0.x;
                        by[index1] += weight * pos0.y;

                        a.AddDiagonalElement(index1, weight);
                }  // end if
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::generateStraighteningForces(const bool stress) {
        Stepwatch watch(__func__);

        fx.clear();
        fy.clear();
        fw.clear();
        fx.assign(mapIndexToCell.size(), 0.0);
        fy.assign(mapIndexToCell.size(), 0.0);
        fw.assign(mapIndexToCell.size(), 0.0);

        for (Rsyn::Net net : module.allNets()) {
                Rsyn::Pin driver = net.getAnyDriver();

                if (!driver) {
                        std::cout << "[WARNING] Net without a driver.\n";
                        continue;
                }

                Rsyn::Cell driverCell =
                    net.getAnyDriver().getInstance().asCell();

                const Number driverCriticality =
                    timer->getSmoothedCriticality(driver, Rsyn::LATE);

                if (driverCriticality > beta) continue;

                if (driverCriticality < alpha ||
                    FloatingPoint::approximatelyEqual(driverCriticality, alpha))
                        continue;

                const bool driverIsMovable =
                    canBeSetAsMovable(driverCell, false);

                bool enableRC = true;
                double R = 1;

                if (enableRC) {
                        if (!libc) {
                                std::cout << "No libc specified! Aborting..."
                                          << std::endl;
                                exit(1);
                        }

                        double driverResistance =
                            libc->getCellMaxDriverResistance(driverCell,
                                                             Rsyn::LATE);

                        R = 0.1 * driverResistance;  // e * loadCapacitance;
                }                                    // end if

                R = std::max(R, 0.01);

                for (Rsyn::Pin pin : net.allPins(Rsyn::IN)) {
                        const Number pinCriticality =
                            timer->getSmoothedCriticality(pin, Rsyn::LATE);
                        // const Number pinCriticality =
                        // timer->getCellCentrality(pin.getCell(), Rsyn::LATE );
                        const Number pinCentrality =
                            timer->getPinCentrality(pin, Rsyn::LATE);

                        if (pinCriticality < alpha ||
                            FloatingPoint::approximatelyEqual(pinCriticality,
                                                              alpha))
                                continue;

                        if (pinCriticality > beta) continue;

                        double weight =  // 10 * RC;
                            5 * R * (1 + pinCriticality);
                        // 10 * RC * ( 1 + 0.5 * pinCriticality );
                        // RC * ( 1 + 3 * pinCentrality + 6 * pinCriticality );
                        // RC * ( 1 + 0.2 * std::exp( std::pow( pinCentrality,
                        // 3) - 1 ) );
                        // RC * ( 0.7 + 0.3 * pinCentrality );
                        // RC * ( 0.3 * pinCentrality + 0.7 * pinCriticality );

                        if (stress) weight *= R;

                        Rsyn::Cell cell = pin.getInstance().asCell();

                        if (!cell) {
                                std::cout << "[WARNING] Pin without a cell.\n";
                                continue;
                        }  // end if

                        const bool cellIsMovable =
                            canBeSetAsMovable(cell, false);

                        if (driverIsMovable && !cellIsMovable) {
                                const int index = mapCellToIndex[driverCell];
                                // const PhysicalCell pCell =
                                // session.getPhysicalCell( cell );
                                // const PhysicalPin pPin =
                                // session.getPhysicalPin( pin );
                                const DBUxy pinPos =
                                    phDesign.getPinPosition(pin);

                                fw[index] += weight;
                                // fx[ index ] += weight * ( pCell.lower().x +
                                // pPin.dx );
                                // fy[ index ] += weight * ( pCell.lower().y +
                                // pPin.dy );
                                fx[index] += weight * (pinPos[X]);
                                fy[index] += weight * (pinPos[Y]);

                                continue;

                        } else if (!driverIsMovable && cellIsMovable) {
                                const int index = mapCellToIndex[cell];
                                // const PhysicalCell pCell =
                                // session.getPhysicalCell( driverCell );
                                // const PhysicalPin pPin =
                                // session.getPhysicalPin( design.getDriver( net
                                // ) );
                                const DBUxy pinPos =
                                    phDesign.getPinPosition(net.getAnyDriver());

                                fw[index] += weight;
                                // fx[ index ] += weight * ( pCell.lower().x +
                                // pPin.dx );
                                // fy[ index ] += weight * ( pCell.lower().y +
                                // pPin.dy );
                                fx[index] += weight * (pinPos[X]);
                                fy[index] += weight * (pinPos[Y]);

                                continue;

                        } else if (!driverIsMovable && !cellIsMovable)
                                continue;

                        const int index1 = mapCellToIndex[driverCell];
                        const int index2 = mapCellToIndex[cell];

                        try {
                                a.AddElement(std::max(index1, index2),
                                             std::min(index1, index2), -weight);
                                a.AddElement(std::min(index1, index2),
                                             std::max(index1, index2), -weight);
                        } catch (exception& e) {
                                std::cout << "Exception at line: " << __LINE__
                                          << std::endl;
                        }  // end try-catch

                        a.AddDiagonalElement(index1, weight);
                        a.AddDiagonalElement(index2, weight);
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::generateStraighteningForces_ELMORE() {
        Stepwatch watch(__func__);

        fx.clear();
        fy.clear();
        fw.clear();
        fx.assign(mapIndexToCell.size(), 0.0);
        fy.assign(mapIndexToCell.size(), 0.0);
        fw.assign(mapIndexToCell.size(), 0.0);

        const double Cwire = routingEstimator->getLocalWireCapPerUnitLength();
        const double Rwire = routingEstimator->getLocalWireResPerUnitLength();

        for (Rsyn::Net net : module.allNets()) {
                Rsyn::Pin driver = net.getAnyDriver();
                Rsyn::Cell driverCell =
                    net.getAnyDriver().getInstance().asCell();

                if (!driverCell) {
                        std::cout << "[WARNING] Net without a driver.\n";
                        continue;
                }  // end if

                const Number driverCriticality =
                    timer->getSmoothedCriticality(driver, Rsyn::LATE);

                if (driverCriticality > beta) continue;

                if (driverCriticality < alpha)  //||
                        // FloatingPoint::approximatelyEqual(driverCriticality,
                        // alpha ) )
                        continue;

                const bool driverIsMovable =
                    canBeSetAsMovable(driverCell, false);

                double Rdrive =
                    libc->getCellMaxDriverResistance(driverCell, Rsyn::LATE);
                Rdrive = std::max(Rdrive, 0.01);

                DBUxy driverPos = phDesign.getPinPosition(driver);

                for (Rsyn::Pin pin : net.allPins(Rsyn::IN)) {
                        const Number pinCriticality =
                            timer->getPinCriticality(pin, Rsyn::LATE);
                        // const Number pinCriticality =
                        // timer->getCellCentrality(pin.getCell(), Rsyn::LATE );
                        const Number pinCentrality =
                            timer->getPinCentrality(pin, Rsyn::LATE);

                        if (pinCriticality < alpha &&
                            !FloatingPoint::approximatelyEqual(pinCriticality,
                                                               alpha))
                                continue;

                        if (pinCriticality > beta) continue;

                        double Cload = timer->getPinInputCapacitance(pin);

                        DBUxy sinkPos = phDesign.getPinPosition(pin);
                        double l = (sinkPos - driverPos).norm();
                        double n = std::max(1, net.getNumPins() - 1);
                        double weight = Rwire * Cwire / (2 * n * n) +
                                        Rwire * Cload / (n * l);

                        weight *=
                            1e11 * Rdrive * 0.1;  // * ( 1 + pinCriticality );

                        Rsyn::Cell cell = pin.getInstance().asCell();

                        if (!cell) {
                                std::cout << "[WARNING] Pin without a cell.\n";
                                continue;
                        }  // end if

                        const bool cellIsMovable =
                            canBeSetAsMovable(cell, false);

                        if (driverIsMovable && !cellIsMovable) {
                                const int index = mapCellToIndex[driverCell];
                                // const PhysicalCell pCell =
                                // session.getPhysicalCell( cell );
                                // const PhysicalPin pPin =
                                // session.getPhysicalPin( pin );
                                const DBUxy pinPos =
                                    phDesign.getPinPosition(pin);

                                fw[index] += weight;
                                // fx[ index ] += weight * ( pCell.lower().x +
                                // pPin.dx );
                                // fy[ index ] += weight * ( pCell.lower().y +
                                // pPin.dy );
                                fx[index] += weight * (pinPos[X]);
                                fy[index] += weight * (pinPos[Y]);

                                continue;

                        } else if (!driverIsMovable && cellIsMovable) {
                                const int index = mapCellToIndex[cell];
                                // const PhysicalCell pCell =
                                // session.getPhysicalCell( driverCell );
                                // const PhysicalPin pPin =
                                // session.getPhysicalPin( net.getDriver() );
                                const DBUxy pinPos =
                                    phDesign.getPinPosition(net.getAnyDriver());

                                fw[index] += weight;
                                // fx[ index ] += weight * ( pCell.lower().x +
                                // pPin.dx );
                                // fy[ index ] += weight * ( pCell.lower().y +
                                // pPin.dy );
                                fx[index] += weight * (pinPos[X]);
                                fy[index] += weight * (pinPos[Y]);

                                continue;

                        } else if (!driverIsMovable && !cellIsMovable)
                                continue;

                        const int index1 = mapCellToIndex[driverCell];
                        const int index2 = mapCellToIndex[cell];

                        try {
                                a.AddElement(std::max(index1, index2),
                                             std::min(index1, index2), -weight);
                                a.AddElement(std::min(index1, index2),
                                             std::max(index1, index2), -weight);
                        } catch (exception& e) {
                                std::cout << "Exception at line: " << __LINE__
                                          << std::endl;
                        }  // end try-catch

                        a.AddDiagonalElement(index1, weight);
                        a.AddDiagonalElement(index2, weight);
                }  // end for
        }          // end for

}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runNetLoadReduction(bool withAnchors) {
        markAsMovableAllNonFixedCells(true);

        buildMapping();

        copyCellsLocationFromSessionToLinearSystem();

        buildLinearSystem(false);

        if (withAnchors)
                neutralizeSystemWithSpreadingForces();
        else
                neutralizeSystem();

        generateLoadAnchors();

        solveLinearSystem();

        copyCellsLocationFromLinearSystemToSession();

        timer->updateTimingIncremental();
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runNetLoadReductionIteration() {
        //	removeAnchors();

        generateLoadAnchors();

        solveLinearSystem();

        copyCellsLocationFromLinearSystemToSession();

        timer->updateTimingIncremental();
}

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runNeutralizeSystemWithSpreadingForces() {
        markAsMovableAllNonFixedCells(false);

        buildMapping();

        copyCellsLocationFromSessionToLinearSystem();

        buildLinearSystem(false);

        neutralizeSystemWithSpreadingForces();

        solveLinearSystem();

        copyCellsLocationFromLinearSystemToSession();

        timer->updateTimingIncremental();
}

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::computeAnchorPositions(std::vector<double>& x,
                                                       std::vector<double>& y) {
        markAsMovableAllNonFixedCells(false);
        buildMapping();
        copyCellsLocationFromSessionToLinearSystem();
        buildLinearSystem(false);

        const double anchorWeight = 1;

        for (int i = 0; i < a.GetDimension(); i++)
                a.AddDiagonalElement(i, anchorWeight);

        std::vector<double> nx(px.size(), 0.0);
        std::vector<double> ny(py.size(), 0.0);

        ::Mul(a, px, nx);
        ::Mul(a, py, ny);

        x.assign(px.size(), 0.0);
        y.assign(py.size(), 0.0);

        ////////////////////////////////////////////////////////////////////////////
        //	std::vector<double> resultx( px.size(), 0.0 );
        //	std::vector<double> resulty( py.size(), 0.0 );
        //
        //	std::vector<double> diffx( px.size(), 0.0 );
        //	std::vector<double> diffy( py.size(), 0.0 );
        //
        //	::SolveByConjugateGradient(a, nx, resultx);
        //	::SolveByConjugateGradient(a, ny, resulty);
        //
        //	::Sub(px, resultx, diffx);
        //	::Sub(py, resulty, diffy);
        //
        //	std::cout << "normx: " << ::Norm(diffx);
        //	std::cout << "normy: " << ::Norm(diffy);
        ////////////////////////////////////////////////////////////////////////////

        ::Sub(nx, bx, x);
        ::Sub(ny, by, y);

        ::Div(x, anchorWeight, x);
        ::Div(y, anchorWeight, y);

        // Histogram
        const int numElements = px.size();
        Rsyn::PhysicalModule phModule = phDesign.getPhysicalModule(module);
        const Bounds& coreBounds = phModule.getBounds();
        const Number range = coreBounds.computeSemiperimeter() * 0.01f;

        std::vector<int> histogram;

        for (int i = 0; i < numElements; i++) {
                const double distance =
                    std::abs(px[i] - x[i]) + std::abs(py[i] - y[i]);
                const int slot = (int)std::floor(distance / range);
                if (slot >= histogram.size()) {
                        histogram.resize(slot + 1, 0);
                }  // end if
                histogram[slot]++;
        }  // end for

        std::cout << "Histogram:\n ";
        for (int i = 0; i < histogram.size(); i++) {
                std::cout << (i * range) << " " << histogram[i] << "\n";
        }  // end for

}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runQuadraticPlacementStraighteningMultiplePaths(
    const int N) {
        std::cout << "Not implemented yet..." << std::endl;
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::
    runQuadraticPlacementStraighteningSinglePathAndSideCells(const int N) {
        std::cout << "Not implemented yet..." << std::endl;
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runPathStraighteningFlow(const bool stress) {
        Stepwatch qp("QP");
        markAsMovableAllNonFixedCells(false);

        buildMapping();

        copyCellsLocationFromSessionToLinearSystem();

        buildLinearSystem(false);

        //	copyCellsLocationFromSessionToLinearSystem();

        neutralizeSystemWithSpreadingForces();

        // Add Straightening forces...
        //	generateStraighteningForces( true );
        generateStraighteningForces(stress);

        solveLinearSystem();

        //	removeCellsFromBlockedAreas();

        //	solveLinearSystem();

        copyCellsLocationFromLinearSystemToSession();
        qp.finish();

        Stepwatch watchTimer("timer");
        timer->updateTimingIncremental();
        watchTimer.finish();
}

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runPathStraighteningFlow_UNDER_TEST() {
        Stepwatch qp("QP");
        markAsMovableAllNonFixedCells(false);

        buildMapping();

        copyCellsLocationFromSessionToLinearSystem();

        buildLinearSystem(false);

        //	copyCellsLocationFromSessionToLinearSystem();

        neutralizeSystemWithSpreadingForces();

        // Add Straightening forces...
        generateStraighteningForcesTopCriticalPaths();

        solveLinearSystem();

        //	removeCellsFromBlockedAreas();

        //	solveLinearSystem();

        copyCellsLocationFromLinearSystemToSession();
        qp.finish();

        Stepwatch watchTimer("timer");
        timer->updateTimingIncremental();
        watchTimer.finish();
}

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runPathStraighteningFlow_ELMORE() {
        Stepwatch qp("QP");
        markAsMovableAllNonFixedCells(false);

        buildMapping();

        copyCellsLocationFromSessionToLinearSystem();

        buildLinearSystem(false);

        //	copyCellsLocationFromSessionToLinearSystem();

        neutralizeSystemWithSpreadingForces();

        // Add Straightening forces...
        generateStraighteningForces_ELMORE();

        solveLinearSystem();

        //	removeCellsFromBlockedAreas();

        //	solveLinearSystem();

        copyCellsLocationFromLinearSystemToSessionOptimized();
        qp.finish();
        Stepwatch watchTimer("timer");
        timer->updateTimingIncremental();
        watchTimer.finish();
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::runNetWeighteningFlow() {
        Stepwatch qp("QP");
        markAsMovableAllNonFixedCells(false);

        buildMapping();

        copyCellsLocationFromSessionToLinearSystem();

        buildLinearSystem(false);

        neutralizeSystemWithSpreadingForces();

        // Generate net weights
        generateNetWeights();

        solveLinearSystem();

        copyCellsLocationFromLinearSystemToSession();
        qp.finish();

        refreshCellsDelayRatio();

        Stepwatch watchTimer("timer");
        timer->updateTimingIncremental();
        watchTimer.finish();
}  // end method

// -----------------------------------------------------------------------------

void IncrementalTimingDrivenQP::generateNetWeights() {
        for (Rsyn::Net net : module.allNets()) {
                Rsyn::Pin driver = net.getAnyDriver();

                if (!driver) continue;

                double driverCriticallity =
                    timer->getPinCriticality(driver, Rsyn::LATE);

                if (driverCriticallity < alpha) continue;

                double R = 1;
                double enableRC = true;

                if (enableRC) {
                        Rsyn::Cell driverCell = driver.getInstance().asCell();
                        double driverResistance =
                            libc->getCellMaxDriverResistance(driverCell,
                                                             Rsyn::LATE);

                        R = 0.1 * driverResistance;  // e * loadCapacitance;
                }

                R = std::max(R, 0.01);

                //		double C = 1;
                //		C = std::max( timer->getNetPinLoad( net
                //)[Rsyn::RISE],
                //				      timer->getNetPinLoad( net
                //)[Rsyn::FALL] );
                //		C = std::max( C, 0.01);
                //
                for (Rsyn::Pin pin0 : net.allPins()) {
                        Rsyn::Cell cell0 = pin0.getInstance().asCell();
                        const int index0 = mapCellToIndex[cell0];

                        if (index0 == -1) continue;

                        for (Rsyn::Pin pin1 : net.allPins()) {
                                if (pin0 == pin1) continue;

                                Rsyn::Cell cell1 = pin1.getInstance().asCell();
                                const int index1 = mapCellToIndex[cell1];

                                if ((index0 != -1 && index1 != -1) &&
                                    index0 >= index1)
                                        continue;

                                double weight =
                                    R * 5 * (1 + driverCriticallity) /
                                    std::max(1, (net.getNumPins() - 1));

                                if (index0 != -1 && index1 != -1) {
                                        // add movable-to-movable connection
                                        a.AddElement(index0, index1, -weight);
                                        a.AddElement(index1, index0, -weight);

                                        a.AddDiagonalElement(index0, weight);
                                        a.AddDiagonalElement(index1, weight);
                                } else if (index0 != -1) {
                                        // add movable-to-fixed connection
                                        const DBUxy pos1 =
                                            phDesign.getPinPosition(pin1);
                                        bx[index0] += weight * pos1.x;
                                        by[index0] += weight * pos1.y;

                                        a.AddDiagonalElement(index0, weight);
                                } else if (index1 != -1) {
                                        // add fixed-to-movable connection
                                        const DBUxy pos0 =
                                            phDesign.getPinPosition(pin0);
                                        bx[index1] += weight * pos0.x;
                                        by[index1] += weight * pos0.y;

                                        a.AddDiagonalElement(index1, weight);
                                }  // end if
                        }          // end for
                }                  // end for
        }                          // end for

}  // end method

// -----------------------------------------------------------------------------

// void IncrementalTimingDrivenQP::doNoHarm() {
//	for( auto& netInfo : criticalNets ) {
//		Rsyn::Net net = std::get<1>( netInfo );
//		Rsyn::Pin driver = net.getDriver();
//		double initialSlack = std::get<0>( netInfo );
//		double currentSlack = timer->getPinWorstNegativeSlack( driver,
// Rsyn::LATE );
//
//		if( initialSlack > currentSlack ) {
//			for( auto pin : net.allPins(Rsyn::IN) ) {
////				auto cell = pin.getCell();
//				session.moveCell( pin.getInstance().asCell(),
//					//session.getPhysicalCell( pin.getCell()
//).initialPos,
//					session.getInitialCellPosition(
// pin.getInstance() ),
//					LegalizationMethod::LEG_NONE);
//			}
//		}
//	}
//} // end method

// -----------------------------------------------------------------------------

bool IncrementalTimingDrivenQP::run(const Rsyn::Json& params) {
        this->session = session;
        libc = session.getService("rsyn.libraryCharacterizer");
        timer = session.getService("rsyn.timer");
        design = session.getDesign();
        module = design.getTopModule();
        routingEstimator = session.getService("rsyn.routingEstimator");
        mapCellToIndex = design.createAttribute();
        delayRatio = design.createAttribute();
        infra = session.getService("ufrgs.ispd16.infra");
        phDesign = session.getPhysicalDesign();

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                mapCellToIndex[cell] = -1;
                delayRatio[cell] = 1.0;
        }

        alpha = params.value("alpha", 0.0f);
        beta = params.value("beta", 1.0f);

        const std::string legModeName =
            params.value("legMode", "NEAREST_WHITESPACE");

        if (legModeName == "NEAREST_WHITESPACE")
                legMode = LegalizationMethod::LEG_NEAREST_WHITESPACE;
        else if (legModeName == "EXACT_LOCATION")
                legMode = LegalizationMethod::LEG_EXACT_LOCATION;
        else if (legModeName == "MIN_LINEAR_DISPLACEMENT")
                legMode = LegalizationMethod::LEG_MIN_LINEAR_DISPLACEMENT;
        else if (legModeName == "NONE")
                legMode = LegalizationMethod::LEG_NONE;
        else
                std::cout << "Invalid legalization method " << legModeName
                          << "."
                          << " Assuming default value.\n";

        if (params.count("flow") && params["flow"] == "default") {
                runPathStraighteningFlow();
        } else if (params["flow"] == "netWeighting") {
                runNetWeighteningFlow();
        } else if (params["flow"] == "elmore") {
                runPathStraighteningFlow_ELMORE();
        } else {
                std::cout << " Warning, invalid QP flow '" << params["flow"]
                          << "'.";
                std::cout << " Skipping QP...";
                return false;
        }  // end if

        return true;
}  // end method

}  // end namespace
