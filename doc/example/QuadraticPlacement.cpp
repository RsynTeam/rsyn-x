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

#include "QuadraticPlacement.h"

#include "util/AsciiProgressBar.h"
#include "util/Stepwatch.h"
#include "tool/qpdp/RelaxedPinPosition.h"

namespace ICCAD15 {

bool QuadraticPlacementExample::run(const Rsyn::Json &params) {
        this->session = session;
        this->infra = session.getService("ufrgs.ispd16.infra");
        this->design = session.getDesign();
        this->timer = session.getService("rsyn.timer");
        this->module = session.getTopModule();
        this->phDesign = session.getPhysicalDesign();

        // Jucemar Monteiro 2017/02/24
        // Removed relaxed pin position of the PhysicalService
        clsRelaxedPinPos = new ICCAD15::RelaxedPinPosition(phDesign);

        const std::string model =
            params.count("model") ? params["model"] : "hybrid";

        if (model == "b2b") {
                runQuadraticPlacementUsinbBound2BoundNetModel();
        } else if (model == "hybrid") {
                runQuadraticPlacementUsingHybridNetModel();
        } else {
                std::cout << "Warning: Invalid quadratic placement model '"
                          << model << "'. "
                                      "Doing nothing...\n";
        }  // end else

        return true;
}  // end method

// -----------------------------------------------------------------------------

void QuadraticPlacementExample::runQuadraticPlacementUsingHybridNetModel() {
        std::cout << "*** Quadratic Placement (Hybrid Net Model) ***\n";

        // Session configuration
        infra->configureMaxDisplacementConstraint(
            false);  // TODO: remove this dependency

        // Store cells that should be considered as movable.
        std::set<Rsyn::Instance> movable;

        // Map a cell to the respective index in the linear system (matrix).
        std::map<Rsyn::Instance, int> mapCellToIndex;

        // Store the number of star nodes created.
        int numStarNodes;

        // Define movable cells.
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                if (!cell.isFixed() && !instance.isLCB() &&
                    !instance.isSequential()) {
                        movable.insert(cell);
                }  // end if
        }          // end for

        // Linear system matrices and right-side-vectors.
        SymmetricCompressedRowMatrix A;
        std::vector<double> bx;
        std::vector<double> by;

        // Construct linear systems.
        Stepwatch watchBuilder("Building linear system");
        buildLinearSystemUsingHybridNetModel(movable, A, bx, by, mapCellToIndex,
                                             numStarNodes);
        watchBuilder.finish();

        const int numMovableElements = movable.size() + numStarNodes;
        std::cout << "#Cells (Movable) : " << movable.size() << "\n";
        std::cout << "#Stars           : " << numStarNodes << "\n";
        std::cout << "#Nodes           : " << numMovableElements << "\n";

        // Position vector.
        std::vector<double> px(numMovableElements, 0);
        std::vector<double> py(numMovableElements, 0);

        // Solve linear systems.
        Stepwatch watchSolver("Solving linear system");
        SolveByConjugateGradientWithDiagonalPreConditioner(A, bx, px, 1000);
        SolveByConjugateGradientWithDiagonalPreConditioner(A, by, py, 1000);
        watchSolver.finish();

        // Update cell positions.
        for (std::tuple<Rsyn::Instance, int> e : mapCellToIndex) {
                Rsyn::Cell cell = std::get<0>(e).asCell();
                const int index = std::get<1>(e);

                const double x = px[index];
                const double y = py[index];
                const DBUxy pos = double2(x, y).convertToDbu();
                infra->moveCell(cell, pos, LEG_NONE, false);
        }  // end for

        // Session configuration.
        infra->configureMaxDisplacementConstraint(
            true);  // TODO: remove this dependency
}  // end method

// -----------------------------------------------------------------------------

void QuadraticPlacementExample::
    runQuadraticPlacementUsinbBound2BoundNetModel() {
        std::cout << "*** Quadratic Placement (Bound2Bound Net Model) ***\n";

        // Note: This value defines the minimum allowed distance between two
        // nodes.
        //
        // A reasonable value is very important for convergence. The main cause
        // for
        // weak convergence is when one mix large and small connectivity
        // weights.
        // If epsilon is two small, a very large weight may be generated, which
        // will
        // affect the convergence.
        //
        // Good values for epsilon are:
        // 1) row height;
        // 2) site width;
        // 3) minimum cell width.

        const double epsilon = phDesign.getRowHeight();

        // Session configuration
        infra->configureMaxDisplacementConstraint(
            false);  // TODO: remove this dependency

        // Store cells that should be considered as movable.
        std::set<Rsyn::Instance> movable;

        // Map a cell to the respective index in the linear system (matrix).
        Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex;
        std::vector<Rsyn::Instance> mapIndexToCell;

        // Define movable cells.
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (!instance.isFixed() && !instance.isLCB() &&
                    !instance.isSequential()) {
                        movable.insert(cell);
                }  // end if
        }          // end for

        // Mapping
        mapCellToIndex = design.createAttribute();
        mapIndexToCell.assign(movable.size(), nullptr);

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                mapCellToIndex[cell] = -1;
        }  // end for

        int counter = 0;
        for (Rsyn::Instance cell : movable) {
                mapCellToIndex[cell] = counter;
                mapIndexToCell[counter] = cell;
                counter++;
        }  // end for

        // Linear system matrices and right-side-vectors.
        SquareCompressedRowMatrix Ax;
        SquareCompressedRowMatrix Ay;

        std::vector<double> bx;
        std::vector<double> by;

        // Construct linear systems.
        Stepwatch watchBuilder("Building linear system");
        buildLinearSystemUsingBound2BoundModel(mapCellToIndex, mapIndexToCell,
                                               Ax, Ay, bx, by, epsilon);
        watchBuilder.finish();

        std::cout << "#Cells (Movable) : " << movable.size() << "\n";

        // Position vector.
        std::vector<double> px(counter, 0);
        std::vector<double> py(counter, 0);

        // Set guess position as the current position. Since B2B system is built
        // based on the current position of movable cells, this speeds up the
        // solver convergence.
        for (int i = 0; i < mapIndexToCell.size(); i++) {
                Rsyn::Instance instance = mapIndexToCell[i];
                if (instance.getType() != Rsyn::CELL) continue;
                Rsyn::PhysicalCell phCell =
                    phDesign.getPhysicalCell(instance.asCell());
                const DBUxy pos = phCell.getPosition();
                px[i] = pos.x;
                py[i] = pos.y;
        }  // end for

        // Solve linear systems.
        Stepwatch watchSolver("Solving linear system");
        std::thread tx([&]() {
                ::SolveByConjugateGradientWithDiagonalPreConditioner(Ax, bx, px,
                                                                     1000);
        });
        std::thread ty([&]() {
                ::SolveByConjugateGradientWithDiagonalPreConditioner(Ay, by, py,
                                                                     1000);
        });
        tx.join();
        ty.join();
        watchSolver.finish();

        // Update cell positions.
        for (int i = 0; i < mapIndexToCell.size(); i++) {
                Rsyn::Cell cell = mapIndexToCell[i].asCell();

                const double x = px[i];
                const double y = py[i];
                const DBUxy pos = double2(x, y).convertToDbu();
                infra->moveCell(cell, pos, LEG_NONE, false);
        }  // end for

        // Session configuration.
        infra->configureMaxDisplacementConstraint(
            true);  // TODO: remove this dependency
}  // end method

// -----------------------------------------------------------------------------

void QuadraticPlacementExample::buildLinearSystemUsingHybridNetModel(
    const std::set<Rsyn::Instance> &movable, SymmetricCompressedRowMatrix &A,
    std::vector<double> &bx, std::vector<double> &by,
    std::map<Rsyn::Instance, int> &mapCellToIndex, int &numStarNodes) {
        const int starModelThreshold = 4;

        SparseMatrixDescriptor dscp;

        // Map cells to indexes.
        mapCellToIndex.clear();

        int numCellNodes = 0;
        for (Rsyn::Instance cell : movable) {
                mapCellToIndex[cell] = numCellNodes;
                numCellNodes++;
        }  // end for

        // Define the nets that connect at least one movable element.
        std::set<Rsyn::Net> nset;

        for (auto &it : mapCellToIndex) {
                Rsyn::Instance cell = std::get<0>(it);
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

        // Count number of stars.
        numStarNodes = 0;
        for (Rsyn::Net net : nset) {
                if (net.getNumPins() >= starModelThreshold) {
                        numStarNodes++;
                }  // end if
        }          // end for

        // Resize righ-hand-side vectors.
        bx.assign(numCellNodes + numStarNodes, 0);
        by.assign(numCellNodes + numStarNodes, 0);

        // Populate the matrix and the right-hand side vectors.
        int starCounter = 0;
        for (Rsyn::Net net : nset) {
                Rsyn::Pin driver = net.getAnyDriver();

                const bool useStarModel =
                    net.getNumPins() >= starModelThreshold;

                if (useStarModel) {
                        const double weight =
                            net.getNumPins() /
                            double(std::max(1, (net.getNumPins() - 1)));

                        const int index1 = numCellNodes + starCounter;

                        for (Rsyn::Pin pin0 : net.allPins()) {
                                Rsyn::Cell cell0 = pin0.getInstance().asCell();
                                const auto it0 = mapCellToIndex.find(cell0);
                                const int index0 = (it0 != mapCellToIndex.end())
                                                       ? it0->second
                                                       : -1;

                                if (index0 != -1) {
                                        // add movable-to-movable connection
                                        dscp.AddElement(index0, index1,
                                                        -weight);
                                        dscp.AddElement(index1, index0,
                                                        -weight);

                                        dscp.AddDiagonalElement(index0, weight);
                                        dscp.AddDiagonalElement(index1, weight);
                                } else {
                                        // add fixed-to-movable connection
                                        const DBUxy pos0 =
                                            phDesign.getPinPosition(pin0);
                                        bx[index1] += weight * pos0.x;
                                        by[index1] += weight * pos0.y;

                                        dscp.AddDiagonalElement(index1, weight);
                                }  // end if

                        }  // end for

                        starCounter++;
                } else {
                        for (Rsyn::Pin pin0 : net.allPins()) {
                                Rsyn::Cell cell0 = pin0.getInstance().asCell();
                                const auto it0 = mapCellToIndex.find(cell0);
                                const int index0 = (it0 != mapCellToIndex.end())
                                                       ? it0->second
                                                       : -1;

                                if (index0 == -1) continue;

                                for (Rsyn::Pin pin1 : net.allPins()) {
                                        if (pin0 == pin1) continue;

                                        Rsyn::Cell cell1 =
                                            pin1.getInstance().asCell();
                                        const auto it1 =
                                            mapCellToIndex.find(cell1);
                                        const int index1 =
                                            (it1 != mapCellToIndex.end())
                                                ? it1->second
                                                : -1;

                                        if ((index0 != -1 && index1 != -1) &&
                                            index0 >= index1)
                                                continue;

                                        const double weight =
                                            1.0 /
                                            double(std::max(
                                                1, (net.getNumPins() - 1)));

                                        if (index0 != -1 && index1 != -1) {
                                                // add movable-to-movable
                                                // connection
                                                dscp.AddElement(index0, index1,
                                                                -weight);
                                                dscp.AddElement(index1, index0,
                                                                -weight);

                                                dscp.AddDiagonalElement(index0,
                                                                        weight);
                                                dscp.AddDiagonalElement(index1,
                                                                        weight);
                                        } else if (index0 != -1) {
                                                // add movable-to-fixed
                                                // connection
                                                const DBUxy pos1 =
                                                    phDesign.getPinPosition(
                                                        pin1);
                                                bx[index0] += weight * pos1.x;
                                                by[index0] += weight * pos1.y;

                                                dscp.AddDiagonalElement(index0,
                                                                        weight);
                                        } else if (index1 != -1) {
                                                // add fixed-to-movable
                                                // connection
                                                const DBUxy pos0 =
                                                    phDesign.getPinPosition(
                                                        pin0);
                                                bx[index1] += weight * pos0.x;
                                                by[index1] += weight * pos0.y;

                                                dscp.AddDiagonalElement(index1,
                                                                        weight);
                                        }  // end if
                                }          // end for
                        }                  // end else
                }                          // end for
        }                                  // end for

        A.Initialize(dscp);
}  // end method

// -----------------------------------------------------------------------------

void QuadraticPlacementExample::buildLinearSystemUsingBound2BoundModel_Add(
    const int numPins,

    const int index0, const double pos0, const int index1, const double pos1,

    const double epsilon,

    SparseMatrixDescriptor &dscp, std::vector<double> &b, double &hpwl) {
        const double distance = std::max(epsilon, std::abs(pos0 - pos1));

        const double weightB2B = 1.0 / ((numPins - 1.0) * distance);
        const double weight = weightB2B;

        if (index0 != -1 && index1 != -1) {
                dscp.AddElement(index0, index1, -weight);
                dscp.AddElement(index1, index0, -weight);

                dscp.AddDiagonalElement(index0, weight);
                dscp.AddDiagonalElement(index1, weight);
        } else if (index0 != -1) {
                b[index0] += weight * pos1;
                dscp.AddDiagonalElement(index0, weight);
        } else if (index1 != -1) {
                b[index1] += weight * pos0;
                dscp.AddDiagonalElement(index1, weight);
        }  // end else if

        // For debug purposes. Notice that we use distance^2 instead of
        // (pos0 - pos1)^2 as the distance accounts for pins too close to each
        // other.
        hpwl += weightB2B * (distance * distance);
}  // end method

// -----------------------------------------------------------------------------

void QuadraticPlacementExample::
    buildLinearSystemUsingBound2BoundModel_AddConnections(
        const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
        const double epsilon, Rsyn::Net net,
        const std::vector<std::tuple<double, Rsyn::Pin>> &pins,
        SparseMatrixDescriptor &dscp, std::vector<double> &b, double &hpwl) {
        const int numPins = net.getNumPins();

        Rsyn::Pin lpin = std::get<1>(pins.front());
        Rsyn::Pin upin = std::get<1>(pins.back());
        if (lpin == upin) return;

        const double lpos = std::get<0>(pins.front());
        const double upos = std::get<0>(pins.back());

        const int lindex = mapCellToIndex[lpin.getInstance()];
        const int uindex = mapCellToIndex[upin.getInstance()];

        // Add boundary connection.
        buildLinearSystemUsingBound2BoundModel_Add(
            numPins, lindex, lpos, uindex, upos, epsilon, dscp, b, hpwl);

        // Add boundary-to-inner connections.
        for (int i = 1; i < numPins - 1; i++) {
                const int iindex =
                    mapCellToIndex[std::get<1>(pins[i]).getInstance()];
                const double ipos = std::get<0>(pins[i]);

                buildLinearSystemUsingBound2BoundModel_Add(
                    numPins, lindex, lpos, iindex, ipos, epsilon, dscp, b,
                    hpwl);
                buildLinearSystemUsingBound2BoundModel_Add(
                    numPins, uindex, upos, iindex, ipos, epsilon, dscp, b,
                    hpwl);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void QuadraticPlacementExample::buildLinearSystemUsingBound2BoundModel(
    const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
    const std::vector<Rsyn::Instance> &mapIndexToCell,
    SquareCompressedRowMatrix &Ax, SquareCompressedRowMatrix &Ay,
    std::vector<double> &bx, std::vector<double> &by, const double epsilon) {
        SparseMatrixDescriptor dscpx;
        SparseMatrixDescriptor dscpy;

        // Clear fixed forces vector
        bx.assign(mapIndexToCell.size(), 0);
        by.assign(mapIndexToCell.size(), 0);

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

        // Build linear system.
        double totalHPWLx = 0;
        double totalHPWLy = 0;

        double totalB2BHPWLx = 0;
        double totalB2BHPWLy = 0;

        std::vector<std::tuple<double, Rsyn::Pin>> xPins;
        std::vector<std::tuple<double, Rsyn::Pin>> yPins;

        for (Rsyn::Net net : nset) {
                clsRelaxedPinPos->getNetPinsSortedByRelaxedPinPositions(net, X,
                                                                        xPins);
                clsRelaxedPinPos->getNetPinsSortedByRelaxedPinPositions(net, Y,
                                                                        yPins);

                const double xmin = std::get<0>(xPins.front());
                const double xmax = std::get<0>(xPins.back());

                const double ymin = std::get<0>(yPins.front());
                const double ymax = std::get<0>(yPins.back());

                const double netHPWLx = xmax - xmin;
                const double netHPWLy = ymax - ymin;

                double netB2BHPWLx = 0;
                double netB2BHPWLy = 0;

                buildLinearSystemUsingBound2BoundModel_AddConnections(
                    mapCellToIndex, epsilon, net, xPins, dscpx, bx,
                    netB2BHPWLx);
                buildLinearSystemUsingBound2BoundModel_AddConnections(
                    mapCellToIndex, epsilon, net, yPins, dscpy, by,
                    netB2BHPWLy);

                // Debug
                totalHPWLx += netHPWLx;
                totalHPWLy += netHPWLy;

                totalB2BHPWLx += netB2BHPWLx;
                totalB2BHPWLy += netB2BHPWLy;
        }  // end for

        // Debug
        std::cout << "HPWLx=" << totalHPWLx << "\n";
        std::cout << "HPWLy=" << totalHPWLy << "\n";

        std::cout << "B2B HPWLx=" << totalB2BHPWLx << "\n";
        std::cout << "B2B HPWLy=" << totalB2BHPWLy << "\n";

        std::cout << "HPWLx == B2B HPWLx? "
                  << FloatingPoint::approximatelyEqual(totalHPWLx,
                                                       totalB2BHPWLx)
                  << "\n";
        std::cout << "HPWLy == B2B HPWLy? "
                  << FloatingPoint::approximatelyEqual(totalHPWLy,
                                                       totalB2BHPWLy)
                  << "\n";

        // Initialize matrixes.
        Ax.Initialize(dscpx, true);
        Ay.Initialize(dscpy, true);

        // Debug
        buildLinearSystemUsingBound2BoundModel_CheckConsistency(
            "Ax", mapIndexToCell, Ax);
        buildLinearSystemUsingBound2BoundModel_CheckConsistency(
            "Ay", mapIndexToCell, Ay);
}  // end method

// -----------------------------------------------------------------------------

void QuadraticPlacementExample::
    buildLinearSystemUsingBound2BoundModel_CheckConsistency(
        const std::string &label,
        const std::vector<Rsyn::Instance> &mapIndexToCell,
        const SquareCompressedRowMatrix &A) {
        int row;

        std::cout << label << "\n";

        std::cout << "  has null diagonal element : ";
        if (A.HasNullDiagonalElement(row)) {
                std::cout << "yes (" << mapIndexToCell[row].getName() << ")";
        } else {
                std::cout << "no";
        }  // end if
        std::cout << "\n";

        std::cout << "  has empty row             : ";
        if (A.HasEmptyRow(row)) {
                std::cout << "yes (" << mapIndexToCell[row].getName() << ")";
        } else {
                std::cout << "no";
        }  // end if
        std::cout << "\n";

        std::cout << "  - A null diagonal creates a singular system (no unique "
                     "solution) and should be considered as a bug.\n";
        std::cout << "  - An empty row (no off-diagonal elements) with no null "
                     "diagonal indicates a node that is connected only to "
                     "fixed nodes.\n";

        std::cout << "\n";
}  // end method

}  // end namescape
