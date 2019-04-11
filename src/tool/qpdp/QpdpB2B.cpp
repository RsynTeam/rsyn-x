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

#include "QpdpbB2B.h"
#include "iccad15/Infrastructure.h"

#include "tool/library/LibraryCharacterizer.h"
#include "tool/timing/Timer.h"
#include "tool/routing/RoutingEstimator.h"

#include "session/Session.h"
#include "util/FloatingPoint.h"
#include "util/AsciiProgressBar.h"
#include "util/StreamStateSaver.h"

namespace ICCAD15 {

void QpdpB2B::init(Rsyn::Session ptr) {
        session = ptr;
        infra = session.getService("ufrgs.ispd16.infra");
        timer = session.getService("rsyn.timer");
        routingEstimator = session.getService("rsyn.routingEstimator");
        design = session.getDesign();
        module = session.getTopModule();
        phDesign = session.getPhysicalDesign();

        // Jucemar Monteiro 2017/02/24
        // Removed relaxed pin position of the PhysicalService
        relaxedPinPos = new ICCAD15::RelaxedPinPosition(phDesign);

}  // end method

// -----------------------------------------------------------------------------

double QpdpB2B::getNetWeight(Rsyn::Net net) const {
        // Guilherme Flach - 2016/09/08 -  The method
        // session.getNetHistoricCriticality(net) is not supported anymore...
        // Returning driver's importance.
        return infra->getPinImportance(net.getAnyDriver(), Rsyn::LATE);
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::buildMapping(const std::set<Rsyn::Cell> &movable,
                           Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex,
                           std::vector<Rsyn::Cell> &mapIndexToCell) {
        // Clean-up.
        mapIndexToCell.assign(movable.size(), nullptr);

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                mapCellToIndex[cell] = -1;
        }  // end for

        // Map
        int index = 0;
        for (Rsyn::Cell cell : movable) {
                mapCellToIndex[cell] = index;
                mapIndexToCell[index] = cell;
                index++;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::copyCellsLocationFromSessionToLinearSystem(
    const std::set<Rsyn::Cell> &movable,
    const Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex,
    std::vector<double> &px, std::vector<double> &py) {
        const int numMovableElements = movable.size();

        px.assign(numMovableElements, 0);
        py.assign(numMovableElements, 0);

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                const int index = mapCellToIndex[cell];

                if (index < 0) continue;
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const double2 cellPos = double2(phCell.getPosition());
                px[index] = cellPos[X];
                py[index] = cellPos[Y];
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::copyCellsLocationFromLinearSystemToSession(
    const Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex,
    const std::vector<Rsyn::Cell> &mapIndexToCell,
    const std::vector<double> &px, const std::vector<double> &py,
    const LegalizationMethod legMode) {
        const bool dontTryToMoveCellsThatDidNotMoveSignificantly = true;
        const bool moveTentatively = false;

        double minDisplacement = 4 * phDesign.getRowHeight();

        std::vector<DBUxy> initialPositions;
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

                if (!dontTryToMoveCellsThatDidNotMoveSignificantly) {
                        const double x = px[index];
                        const double y = py[index];
                        const double2 newPos(x, y);
                        Rsyn::PhysicalCell phCell =
                            phDesign.getPhysicalCell(cell);
                        const double2 oldPos = double2(phCell.getPosition());

                        if (oldPos.approximatelyEqual(newPos,
                                                      minDisplacement)) {
                                continue;
                        }  // enf if
                }          // end if

                orderedCells.push_back(std::make_tuple(slack, cell, index));
        }

        std::sort(orderedCells.begin(), orderedCells.end());

        double averageDisplacement = 0.0;
        int numMoved = 0;
        for (const std::tuple<Number, Rsyn::Cell, int> &e : orderedCells) {
                Rsyn::Cell cell = std::get<1>(e);
                const int index = std::get<2>(e);

                const double x = px[index];
                const double y = py[index];
                const double2 newPos(x, y);
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const DBUxy oldPos = phCell.getPosition();
                initialPositions[index] = oldPos;

                if (moveTentatively) {
                        infra->moveCellTowards(cell, newPos.convertToDbu(),
                                               legMode, 0.2);
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
        }  // end for

        averageDisplacement /= numMoved;
        averageDisplacement /= (double)phDesign.getRowHeight();
}  // end method

// -----------------------------------------------------------------------------

double QpdpB2B::getDriverResistance(Rsyn::Net net) {
        const Rsyn::LibraryCharacterizer *libc =
            session.getService("rsyn.libraryCharacterizer");

        Rsyn::Pin driver = net.getAnyDriver();
        double rDriver = verySmallResistance;
        if (driver) {
                rDriver = libc->getCellMaxDriverResistance(driver.getInstance(),
                                                           Rsyn::LATE);
                if (rDriver < verySmallResistance) {
                        rDriver = verySmallResistance;
                }  // end if
        }          // end if
        return rDriver;
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::createNodeToIndexMapping(
    Rsyn::Net net, const Rsyn::RCTree &tree,
    const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
    std::vector<int> &mapNodeToIndex, int &nextNodeIndex) {
        const int numNodes = tree.getNumNodes();
        mapNodeToIndex.resize(numNodes);

        for (int i = 0; i < numNodes; i++) {
                const Rsyn::RCTreeNodeTag &tag = tree.getNodeTag(i);
                Rsyn::Pin pin = tag.getPin();

                if (pin) {
                        mapNodeToIndex[i] = mapCellToIndex[pin.getInstance()];
                } else {
                        mapNodeToIndex[i] = nextNodeIndex++;
                }  // end else
        }          // end method

}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::neutralizeSystemUsingAnchors(const SquareCompressedRowMatrix &A,
                                           const std::vector<double> &b,
                                           const std::vector<double> &position,
                                           const double weight,
                                           std::vector<double> &digIncrement,
                                           std::vector<double> &rhsIncrement) {
        digIncrement.assign(A.GetDimension(), 0);
        for (int i = 0; i < A.GetDimension(); i++) {
                digIncrement[i] = weight * A.GetDiagonalElement(i);
        }  // end for

        std::vector<double> nb(position.size(),
                               0.0);  // neutrilization b (rhs vector)
        ::Mul(A, position, nb);       // nb = A*position

        // Add to nb the increment due to the change in the diagonal, which was
        // not accounted by the multiplication A*position as A is not changed.
        for (int i = 0; i < digIncrement.size(); i++) {
                nb[i] += digIncrement[i] * position[i];
        }  // end for

        rhsIncrement.assign(position.size(), 0.0);  // anchor_pos * weight
        ::Sub(nb, b, rhsIncrement);
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::neutralizeSystemUsingForces(const SquareCompressedRowMatrix &A,
                                          const std::vector<double> &b,
                                          const std::vector<double> &position,
                                          std::vector<double> &digIncrement,
                                          std::vector<double> &rhsIncrement) {
        digIncrement.assign(A.GetDimension(), 0);

        std::vector<double> nb(position.size(),
                               0.0);  // neutrilization b (rhs vector)
        ::Mul(A, position, nb);       // nb = A*position

        rhsIncrement.assign(A.GetDimension(), 0.0);
        ::Sub(nb, b, rhsIncrement);
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::buildLinearSystem(
    const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
    const std::vector<Rsyn::Cell> &mapIndexToCell, SquareCompressedRowMatrix &A,
    std::vector<double> &bx, std::vector<double> &by,

    const Weightening &weightening) {
        // Clear
        A.Reset();
        bx.assign(mapIndexToCell.size(), 0);
        by.assign(mapIndexToCell.size(), 0);

        SparseMatrixDescriptor dscp;

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
                const int numPins = net.getNumPins();
                if (numPins < 2) continue;

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

                                const double edgeWeight = 1.0 / double(numPins);
                                const double weight =
                                    weightening(pin0, pin1, edgeWeight);

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
                                        const double2 pos1 = double2(
                                            phDesign.getRelaxedPinPosition(
                                                pin1));
                                        bx[index0] += weight * pos1.x;
                                        by[index0] += weight * pos1.y;

                                        dscp.AddDiagonalElement(index0, weight);
                                } else if (index1 != -1) {
                                        // add fixed-to-movable connection
                                        const double2 pos0 = double2(
                                            phDesign.getRelaxedPinPosition(
                                                pin0));
                                        bx[index1] += weight * pos0.x;
                                        by[index1] += weight * pos0.y;

                                        dscp.AddDiagonalElement(index1, weight);
                                }  // end if
                        }          // end for
                }                  // end for
        }                          // end for

        A.Initialize(dscp, true);
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::buildLinearSystem_Add(const int index0,  // driver
                                    const int index1,  // sink
                                    const double pos0, const double pos1,
                                    const double Rw, const double Cw,
                                    const double Cdown,

                                    const double netWeight,

                                    const double epsilon,

                                    SparseMatrixDescriptor &dscp,
                                    std::vector<double> &b) {
        const bool useForces = false;

        const double d = std::max(epsilon, std::abs(pos1 - pos0));

        const double weightQuadratic = Rw * (Cw / 2);
        const double weightLinear = useForces ? 0 : Rw * Cdown / d;
        const double weight = netWeight * (weightQuadratic + weightLinear);

        const double force1 =
            useForces ? netWeight * (Rw * Cdown * (pos1 > pos0 ? -1 : +1)) : 0;
        const double force0 = useForces ? -force1 : 0;

        if (index0 != -1 && index1 != -1) {
                dscp.AddElement(index0, index1, -weight);
                dscp.AddElement(index1, index0, -weight);

                dscp.AddDiagonalElement(index0, weight);
                dscp.AddDiagonalElement(index1, weight);

                b[index0] += force0;
                b[index1] += force1;
        } else if (index0 != -1) {
                b[index0] += weight * pos1 + force0;
                dscp.AddDiagonalElement(index0, weight);
        } else if (index1 != -1) {
                b[index1] += weight * pos0 + force1;
                dscp.AddDiagonalElement(index1, weight);
        }  // end else if

}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::buildLinearSystemUsingBound2BoundModel_Add(
    const int numPins,

    const int index0, const double pos0, const int index1, const double pos1,

    const double epsilon,

    const double weightScaling,

    SparseMatrixDescriptor &dscp, std::vector<double> &b, double &hpwl) {
        const double distance = std::max(epsilon, std::abs(pos0 - pos1));

        const double weightB2B = 1.0 / ((numPins - 1.0) * distance);
        const double weight = weightScaling * weightB2B;

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

void QpdpB2B::buildLinearSystemUsingBound2BoundModel_AddConnections(
    const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
    const bool stress, const double epsilon, const double netWeight,
    Rsyn::Net net, const std::vector<std::tuple<double, Rsyn::Pin>> &pins,
    SparseMatrixDescriptor &dscp, std::vector<double> &b, double &hpwl) {
        const bool enableRC = true;

        const int numPins = net.getNumPins();

        Rsyn::Pin lpin = std::get<1>(pins.front());
        Rsyn::Pin upin = std::get<1>(pins.back());
        if (lpin == upin) return;

        const double lpos = std::get<0>(pins.front());
        const double upos = std::get<0>(pins.back());

        const int lindex = mapCellToIndex[lpin.getInstance()];
        const int uindex = mapCellToIndex[upin.getInstance()];

        // Weight scaling.
        const double rDriver =
            net.getAnyDriver() && enableRC ? getDriverResistance(net) : 1.0;
        const double Cw = routingEstimator->getLocalWireCapPerUnitLength();

        const double weight = netWeight * rDriver * Cw;

        // Add boundary connection.
        buildLinearSystemUsingBound2BoundModel_Add(numPins, lindex, lpos,
                                                   uindex, upos, epsilon,
                                                   weight, dscp, b, hpwl);

        // Add boundary-to-inner connections.
        for (int i = 1; i < numPins - 1; i++) {
                const int iindex =
                    mapCellToIndex[std::get<1>(pins[i]).getInstance()];
                const double ipos = std::get<0>(pins[i]);

                buildLinearSystemUsingBound2BoundModel_Add(
                    numPins, lindex, lpos, iindex, ipos, epsilon, weight, dscp,
                    b, hpwl);
                buildLinearSystemUsingBound2BoundModel_Add(
                    numPins, uindex, upos, iindex, ipos, epsilon, weight, dscp,
                    b, hpwl);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::buildLinearSystemUsingBound2BoundModel(
    const std::set<Rsyn::Cell> &movable,
    const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
    const bool stress, std::vector<Rsyn::Cell> &mapIndexToCell,
    SquareCompressedRowMatrix &Ax, SquareCompressedRowMatrix &Ay,
    std::vector<double> &bx, std::vector<double> &by, std::vector<double> &px,
    std::vector<double> &py) {
        // Clean-up.
        Ax.Reset();
        Ay.Reset();

        // Workaround. Remove steiner point mapping as they will be created
        // again... workaround
        mapIndexToCell.resize(movable.size());

        const double epsilon = phDesign.getRowHeight();

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

        std::vector<int> mapping;
        int nextNodeIndex = mapIndexToCell.size();

        const double Rw = routingEstimator->getLocalWireResPerUnitLength();
        const double Cw = routingEstimator->getLocalWireCapPerUnitLength();

        for (Rsyn::Net net : nset) {
                const double netWeight =
                    std::max(verySmallNetWeight, getNetWeight(net));

                relaxedPinPos->getNetPinsSortedByRelaxedPinPositions(net, X,
                                                                     xPins);
                relaxedPinPos->getNetPinsSortedByRelaxedPinPositions(net, Y,
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
                    mapCellToIndex, stress, epsilon, netWeight, net, xPins,
                    dscpx, bx, netB2BHPWLx);
                buildLinearSystemUsingBound2BoundModel_AddConnections(
                    mapCellToIndex, stress, epsilon, netWeight, net, yPins,
                    dscpy, by, netB2BHPWLy);

                if (true) {
                        const Rsyn::RCTree &tree =
                            routingEstimator->getRCTree(net);

                        const int prevNodeIndex = nextNodeIndex;
                        createNodeToIndexMapping(net, tree, mapCellToIndex,
                                                 mapping, nextNodeIndex);

                        // Resize
                        if (nextNodeIndex > prevNodeIndex) {
                                bx.resize(nextNodeIndex, 0);
                                by.resize(nextNodeIndex, 0);
                                px.resize(nextNodeIndex, 0);
                                py.resize(nextNodeIndex, 0);
                                mapIndexToCell.resize(nextNodeIndex, nullptr);
                        }  // end if

                        const int numNodes = tree.getNumNodes();
                        for (int i = 1; i < numNodes;
                             i++) {  // starts at 1 to skip root node
                                const Rsyn::RCTree::Node &node =
                                    tree.getNode(i);
                                const Rsyn::RCTreeNodeTag &tag0 =
                                    tree.getNodeTag(node.propParent);
                                const Rsyn::RCTreeNodeTag &tag1 =
                                    tree.getNodeTag(i);

                                const int index0 = mapping[node.propParent];
                                const int index1 = mapping[i];

                                const double x0 = tag0.x;
                                const double x1 = tag1.x;

                                const double y0 = tag0.y;
                                const double y1 = tag1.y;

                                const double d =
                                    std::abs(x0 - x1) + std::abs(y0 - y1);
                                const double Csegment = Cw * d / 2;

                                const double Cdown =
                                    node.getDownstreamCap().getMax() - Csegment;
                                if (Cdown < 0) {
                                        std::cout << "[BUG] Negative "
                                                     "donwnstream cap.\n";
                                }  // end if

                                buildLinearSystem_Add(index0, index1, x0, x1,
                                                      Rw, Cw, Cdown, netWeight,
                                                      epsilon, dscpx, bx);
                                buildLinearSystem_Add(index0, index1, y0, y1,
                                                      Rw, Cw, Cdown, netWeight,
                                                      epsilon, dscpy, by);

                                // Assign steiner points positions.
                                if (index1 >= prevNodeIndex &&
                                    index1 < nextNodeIndex) {
                                        px[index1] = x1;
                                        py[index1] = y1;
                                }  // end if
                        }          // end for
                }                  // end if

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

        // Initialize matrixes.
        Ax.Initialize(dscpx, true);
        Ay.Initialize(dscpy, true);

        buildLinearSystemUsingBound2BoundModel_CheckConsistency(
            "Ax", mapIndexToCell, Ax);
        buildLinearSystemUsingBound2BoundModel_CheckConsistency(
            "Ay", mapIndexToCell, Ay);

        // Ax.Print(std::cout);
        // Ay.Print(std::cout);
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::buildLinearSystemUsingBound2BoundModel_CheckConsistency(
    const std::string &label, const std::vector<Rsyn::Cell> &mapIndexToCell,
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

// -----------------------------------------------------------------------------

bool QpdpB2B::canBeSetAsMovable(Rsyn::Cell cell,
                                const bool considerCriticalCellsAsFixed) {
        const PhysicalCell physicalCell = phDesign.getPhysicalCell(cell);
        return !cell.isPort() && !cell.isFixed() && !cell.isSequential() &&
               !cell.isLCB() && !cell.isMacroBlock() &&
               !(considerCriticalCellsAsFixed &&
                 timer->getCellCriticality(cell, Rsyn::LATE) > 0);
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedCells(
    const bool considerCriticalCellsAsFixed, std::set<Rsyn::Cell> &movable) {
        movable.clear();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (canBeSetAsMovable(cell, considerCriticalCellsAsFixed)) {
                        movable.insert(cell);
                }  // end if
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedCriticalCells(
    std::set<Rsyn::Cell> &movable) {
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
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedNonCriticalCellsDrivenByCriticalNets(
    std::set<Rsyn::Cell> &movable) {
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
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedCellsConnectedToCriticalNets(
    std::set<Rsyn::Cell> &movable) {
        movable.clear();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (!canBeSetAsMovable(cell, false)) continue;

                // Check if cell is connected to at least one critical net.
                bool isConnectedToAtLeastOneCriticalNet = false;
                for (Rsyn::Pin pin : cell.allPins()) {
                        Rsyn::Net net = pin.getNet();
                        if (net &&
                            timer->getNetCriticality(net, Rsyn::LATE) > 0) {
                                isConnectedToAtLeastOneCriticalNet = true;
                                break;
                        }  // end if
                }          // end for

                if (isConnectedToAtLeastOneCriticalNet) movable.insert(cell);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedCriticalCellsAndTheirSinks(
    std::set<Rsyn::Cell> &movable) {
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
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedCriticalCellsAndTheirNeighbors(
    std::set<Rsyn::Cell> &movable) {
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
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedCellsInTopCriticalPaths(
    const int numPaths, std::set<Rsyn::Cell> &movable) {
        movable.clear();

        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        timer->queryTopCriticalPaths(Rsyn::LATE, numPaths, paths);

        const int actualNumPaths = paths.size();
        for (int i = 0; i < actualNumPaths; i++) {
                std::vector<Rsyn::Timer::PathHop> &path = paths[i];
                for (const Rsyn::Timer::PathHop &hop : path) {
                        Rsyn::Cell cell = hop.getInstance().asCell();
                        if (canBeSetAsMovable(cell, false)) {
                                movable.insert(cell);
                        }  // end if
                }          // end for
        }                  // end for
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedCellsInKthTopCriticalPath(
    const int k, std::set<Rsyn::Cell> &movable) {
        movable.clear();

        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        timer->queryTopCriticalPaths(Rsyn::LATE, k, paths);
        if (paths.size() < k) return;

        std::vector<Rsyn::Timer::PathHop> &path = paths[k - 1];
        for (const Rsyn::Timer::PathHop &hop : path) {
                Rsyn::Cell cell = hop.getInstance().asCell();
                if (canBeSetAsMovable(cell, false)) {
                        movable.insert(cell);
                }  // end if
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::
    markAsMovableAllNonFixedCellsInTopCriticalPathsFromTopCriticalEndpoints(
        const int numEndpoints, const int numPathsPerEndpoings,
        std::set<Rsyn::Cell> &movable) {
        movable.clear();

        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        timer->queryTopCriticalPathsFromTopCriticalEndpointsConsolidated(
            Rsyn::LATE, numEndpoints, numPathsPerEndpoings, paths);

        const int actualNumPaths = paths.size();
        for (int i = 0; i < actualNumPaths; i++) {
                std::vector<Rsyn::Timer::PathHop> &path = paths[i];
                for (const Rsyn::Timer::PathHop &hop : path) {
                        Rsyn::Cell cell = hop.getInstance().asCell();
                        if (canBeSetAsMovable(cell, false)) {
                                movable.insert(cell);
                        }  // end if
                }          // end for
        }                  // end for
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::markAsMovableAllNonFixedCellsInTopCriticalPathsAndTheirNeighbors(
    const int numPaths, std::set<Rsyn::Cell> &movable) {
        movable.clear();

        std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
        timer->queryTopCriticalPaths(Rsyn::LATE, numPaths, paths);

        const int actualNumPaths = paths.size();
        for (int i = 0; i < actualNumPaths; i++) {
                std::vector<Rsyn::Timer::PathHop> &path = paths[i];
                for (const Rsyn::Timer::PathHop &hop : path) {
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
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::runPathStraightening() {
        Number delayDueToCell;
        Number delayDueToWire;
        std::vector<Rsyn::Timer::PathHop> path;

        StreamStateSaver sss(std::cout);
        // std::cout << std::fixed << std::setprecision(2);

        timer->queryTopCriticalPath(Rsyn::LATE, path);
        timer->getPathDelayDueToCellsAndWires(path, delayDueToCell,
                                              delayDueToWire);
        std::cout << "---------------------------------------------------------"
                     "----\n";
        std::cout << "path delay (before) : " << delayDueToCell << " "
                  << delayDueToWire << " " << (delayDueToCell + delayDueToWire)
                  << " " << timer->getPathHash(path, false) << "\n";

        // Configuration.
        const bool useNeutralization = false;
        const bool useAnchors = true;

        const double anchorWeight = 1;

        const LegalizationMethod legalizationMethod =
            // LEG_NONE;
            LEG_NEAREST_WHITESPACE;
        // LEG_MIN_LINEAR_DISPLACEMENT;

        // State
        std::set<Rsyn::Cell> movable;

        Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex;
        std::vector<Rsyn::Cell> mapIndexToCell;

        SquareCompressedRowMatrix Ax;
        SquareCompressedRowMatrix Ay;

        std::vector<double> bx;  // right-hand side (rhs) vectors
        std::vector<double> by;

        std::vector<double> px;  // result (position) vectors
        std::vector<double> py;

        std::vector<double> incDx;  // diagonal neutralization
        std::vector<double> incDy;

        std::vector<double> incBx;  // rhs neutralization
        std::vector<double> incBy;

        // Initialization
        mapCellToIndex = design.createAttribute();

        // Define movable cells.
        // markAsMovableAllNonFixedCellsInTopCriticalPaths(1, movable);
        // markAsMovableAllNonFixedCellsInKthTopCriticalPath(1, movable);
        // markAsMovableAllNonFixedCellsInTopCriticalPathsFromTopCriticalEndpoints(10,
        // 1, movable);
        // markAsMovableAllNonFixedCellsInTopCriticalPathsAndTheirNeighbors(10,
        // movable);
        // markAsMovableAllNonFixedCriticalCells(movable);
        // markAsMovableAllNonFixedCells(false, movable);
        markAsMovableAllNonFixedCellsConnectedToCriticalNets(movable);

        // Map movable cells to indexes.
        buildMapping(movable, mapCellToIndex, mapIndexToCell);

        // Copy current cell positions.
        copyCellsLocationFromSessionToLinearSystem(movable, mapCellToIndex, px,
                                                   py);

        // Build plain linear system.
        buildLinearSystemUsingBound2BoundModel(movable, mapCellToIndex, false,
                                               mapIndexToCell, Ax, Ay, bx, by,
                                               px, py);

        // Compute neutralization anchors for the plain system.
        if (useNeutralization) {
                if (useAnchors) {
                        neutralizeSystemUsingAnchors(Ax, bx, px, anchorWeight,
                                                     incDx, incBx);
                        neutralizeSystemUsingAnchors(Ay, by, py, anchorWeight,
                                                     incDy, incBy);
                } else {
                        neutralizeSystemUsingForces(Ax, bx, px, incDx, incBx);
                        neutralizeSystemUsingForces(Ay, by, py, incDy, incBy);
                }  // end else
        }          // end if

        // Build the optimization linear system.
        if (useNeutralization) {
                // Build the linear system with the new weights.
                buildLinearSystemUsingBound2BoundModel(movable, mapCellToIndex,
                                                       false, mapIndexToCell,
                                                       Ax, Ay, bx, by, px, py);

                // Apply the neutralization anchors computed for the plain
                // system to the
                // optimization system.
                ::Add(Ax.GetDiagonalVector(), incDx, Ax.GetDiagonalVector());
                ::Add(Ay.GetDiagonalVector(), incDy, Ay.GetDiagonalVector());

                ::Add(bx, incBx, bx);
                ::Add(by, incBy, by);
        }  // end if

        // Solve the optimization linear system.
        std::thread tx([&]() {
                ::SolveByConjugateGradientWithDiagonalPreConditioner(Ax, bx, px,
                                                                     5000);
        });
        std::thread ty([&]() {
                ::SolveByConjugateGradientWithDiagonalPreConditioner(Ay, by, py,
                                                                     5000);
        });

        tx.join();
        ty.join();

        // Update cell position in the session.
        copyCellsLocationFromLinearSystemToSession(
            mapCellToIndex, mapIndexToCell, px, py, legalizationMethod);

        // Update timing.
        timer->updateTimingIncremental();
        infra->reportSummary("qpdpb2b");

        timer->updatePath(path);
        timer->getPathDelayDueToCellsAndWires(path, delayDueToCell,
                                              delayDueToWire);
        std::cout << "path delay (after)  : " << delayDueToCell << " "
                  << delayDueToWire << " " << (delayDueToCell + delayDueToWire)
                  << " " << timer->getPathHash(path, false) << "\n";

        // timer->reportPath(path);
}  // end method

// -----------------------------------------------------------------------------

void QpdpB2B::runPathStraighteningUsingCliqueModel() {
        Number delayDueToCell;
        Number delayDueToWire;
        std::vector<Rsyn::Timer::PathHop> path;

        StreamStateSaver sss(std::cout);
        // std::cout << std::fixed << std::setprecision(2);

        timer->queryTopCriticalPath(Rsyn::LATE, path);
        timer->getPathDelayDueToCellsAndWires(path, delayDueToCell,
                                              delayDueToWire);
        std::cout << "---------------------------------------------------------"
                     "----\n";
        std::cout << "path delay (before) : " << delayDueToCell << " "
                  << delayDueToWire << " " << (delayDueToCell + delayDueToWire)
                  << " " << timer->getPathHash(path, false) << "\n";

        // Configuration.
        const double anchorWeight = 1;
        const bool useAnchors = false;
        const LegalizationMethod legalizationMethod = LEG_NEAREST_WHITESPACE;
        // const LegalizationMethod legalizationMethod = LEG_NONE;

        // State
        std::set<Rsyn::Cell> movable;

        Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex;
        std::vector<Rsyn::Cell> mapIndexToCell;

        SquareCompressedRowMatrix A;

        std::vector<double> bx;  // right-hand side (rhs) vectors
        std::vector<double> by;

        std::vector<double> px;  // result (position) vectors
        std::vector<double> py;

        std::vector<double> incDx;  // diagonal neutralization
        std::vector<double> incDy;

        std::vector<double> incBx;  // rhs neutralization
        std::vector<double> incBy;

        // Initialization
        mapCellToIndex = design.createAttribute();

        // Weightening
        Weightening weightening = [&](Rsyn::Pin pin0, Rsyn::Pin pin1,
                                      const double edgeWeight) {
                const double driverResistance =
                    getDriverResistance(pin0.getNet());
                const double w0 = infra->getPinImportance(pin0, Rsyn::LATE);
                const double w1 = infra->getPinImportance(pin1, Rsyn::LATE);
                return driverResistance *
                       std::max(verySmallNetWeight, std::max(w0, w1));
        };

        // Define movable cells.
        // markAsMovableAllNonFixedCellsInTopCriticalPaths(1, movable);
        // markAsMovableAllNonFixedCellsInTopCriticalPathsAndTheirNeighbors(1,
        // movable);
        // markAsMovableAllNonFixedCriticalCells(movable);
        // markAsMovableAllNonFixedCells(false, movable);
        markAsMovableAllNonFixedCellsConnectedToCriticalNets(movable);

        // Map movable cells to indexes.
        buildMapping(movable, mapCellToIndex, mapIndexToCell);

        // Copy current cell positions.
        copyCellsLocationFromSessionToLinearSystem(movable, mapCellToIndex, px,
                                                   py);

        // Build the optimization linear system.
        buildLinearSystem(mapCellToIndex, mapIndexToCell, A, bx, by,
                          weightening);

        // Compute neutralization anchors for the stressed system.
        if (useAnchors) {
                neutralizeSystemUsingAnchors(A, bx, px, anchorWeight, incDx,
                                             incBx);
                neutralizeSystemUsingAnchors(A, by, py, anchorWeight, incDy,
                                             incBy);
        } else {
                neutralizeSystemUsingForces(A, bx, px, incDx, incBx);
                neutralizeSystemUsingForces(A, by, py, incDy, incBy);
        }  // end else

        // Build plain linear system.
        buildLinearSystem(mapCellToIndex, mapIndexToCell, A, bx, by,
                          weightening);

        // Apply the neutralization anchors computed for the plain system to the
        // optimization system.
        ::Add(A.GetDiagonalVector(), incDx, A.GetDiagonalVector());
        ::Add(A.GetDiagonalVector(), incDy, A.GetDiagonalVector());

        ::Add(bx, incBx, bx);
        ::Add(by, incBy, by);

        // Solve the optimization linear system.
        std::thread tx([&]() {
                ::SolveByConjugateGradientWithDiagonalPreConditioner(A, bx, px,
                                                                     1000);
        });
        std::thread ty([&]() {
                ::SolveByConjugateGradientWithDiagonalPreConditioner(A, by, py,
                                                                     1000);
        });

        tx.join();
        ty.join();

        // Update cell position in the session.
        copyCellsLocationFromLinearSystemToSession(
            mapCellToIndex, mapIndexToCell, px, py, legalizationMethod);

        // Update timing.
        timer->updateTimingIncremental();
        infra->reportSummary("qpdpb2b");

        timer->updatePath(path);
        timer->getPathDelayDueToCellsAndWires(path, delayDueToCell,
                                              delayDueToWire);
        std::cout << "path delay (after)  : " << delayDueToCell << " "
                  << delayDueToWire << " " << (delayDueToCell + delayDueToWire)
                  << " " << timer->getPathHash(path, false) << "\n";

}  // end method

}  // end namespace
