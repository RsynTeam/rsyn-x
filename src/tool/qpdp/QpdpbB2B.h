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

#ifndef QPDP2_H
#define QPDP2_H

#include <map>
#include <vector>

#include "core/Rsyn.h"
#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include "tool/routing/RCTree.h"

#include "iccad15/utilICCAD15.h"
#include "tool/lnalg/scrm.h"
#include "tool/lnalg/lnalg.h"
#include "RelaxedPinPosition.h"

namespace Rsyn {
class LibraryCharacterizer;
class Timer;
class RoutingEstimator;
}

namespace ICCAD15 {

class Infrastructure;

class QpdpB2B {
       private:
        Rsyn::Session session;
        Infrastructure *infra;
        Rsyn::Design design;
        Rsyn::Module module;
        Rsyn::PhysicalDesign phDesign;
        Rsyn::Timer *timer;
        Rsyn::RoutingEstimator *routingEstimator;
        RelaxedPinPosition *relaxedPinPos;

        double verySmallResistance;
        double verySmallNetWeight;

       private:
        typedef std::function<double(Rsyn::Pin pin0, Rsyn::Pin pin1,
                                     const double edgeWeight)>
            Weightening;

        void buildMapping(const std::set<Rsyn::Cell> &movable,
                          Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex,
                          std::vector<Rsyn::Cell> &mapIndexToCell);

        void copyCellsLocationFromSessionToLinearSystem(
            const std::set<Rsyn::Cell> &movable,
            const Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex,
            std::vector<double> &px, std::vector<double> &py);

        void copyCellsLocationFromLinearSystemToSession(
            const Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex,
            const std::vector<Rsyn::Cell> &mapIndexToCell,
            const std::vector<double> &px, const std::vector<double> &py,
            const LegalizationMethod legMode);

        void neutralizeSystemUsingAnchors(const SquareCompressedRowMatrix &A,
                                          const std::vector<double> &b,
                                          const std::vector<double> &position,
                                          const double weight,
                                          std::vector<double> &digIncrement,
                                          std::vector<double> &rhsIncrement);

        void neutralizeSystemUsingForces(const SquareCompressedRowMatrix &A,
                                         const std::vector<double> &b,
                                         const std::vector<double> &position,
                                         std::vector<double> &digIncrement,
                                         std::vector<double> &rhsIncrement);

        // Return the driver resistance of an output pin.
        double getDriverResistance(Rsyn::Net net);

        // Creates a map from tree node indexes to matrix indexes.
        void createNodeToIndexMapping(
            Rsyn::Net net, const Rsyn::RCTree &tree,
            const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
            std::vector<int> &mapNodeToIndex, int &nextNodeIndex);

        // Build linear system using clique model.
        void buildLinearSystem(
            const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
            const std::vector<Rsyn::Cell> &mapIndexToCell,
            SquareCompressedRowMatrix &A, std::vector<double> &bx,
            std::vector<double> &by,

            const Weightening &weightening);

        void buildLinearSystem_Add(const int index0,  // driver
                                   const int index1,  // sink
                                   const double pos0, const double pos1,
                                   const double Rw, const double Cw,
                                   const double Cdown,

                                   const double netWeight,

                                   const double epsilon,

                                   SparseMatrixDescriptor &dscp,
                                   std::vector<double> &b);

        // Build linear system using b2b model.
        void buildLinearSystemUsingBound2BoundModel(
            const std::set<Rsyn::Cell> &movable,
            const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
            const bool stress, std::vector<Rsyn::Cell> &mapIndexToCell,
            SquareCompressedRowMatrix &Ax, SquareCompressedRowMatrix &Ay,
            std::vector<double> &bx, std::vector<double> &by,
            std::vector<double> &px, std::vector<double> &py);

        void buildLinearSystemUsingBound2BoundModel_AddConnections(
            const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
            const bool stress, const double epsilon, const double netWeight,
            Rsyn::Net net,
            const std::vector<std::tuple<double, Rsyn::Pin>> &pins,
            SparseMatrixDescriptor &dscp, std::vector<double> &b, double &hpwl);

        void buildLinearSystemUsingBound2BoundModel_Nodes(
            const Dimension dimension,
            const std::map<std::tuple<Rsyn::Net, int>, int> &mapNodeToIndex,
            Rsyn::Net net, const Rsyn::RCTree &tree, const int treeNodeIndex,
            std::vector<std::tuple<double, int>> &nodes);

        void buildLinearSystemUsingBound2BoundModel_AddConnections2(
            const std::vector<std::tuple<double, int>> &nodes,
            const double epsilon, SparseMatrixDescriptor &dscp,
            std::vector<double> &b);

        void buildLinearSystemUsingBound2BoundModel_Add(
            const int numPins,

            const int index0, const double pos0, const int index1,
            const double pos1,

            const double epsilon, const double weightScaling,

            SparseMatrixDescriptor &dscp, std::vector<double> &b, double &hpwl);

        void buildLinearSystemUsingBound2BoundModel_CheckConsistency(
            const std::string &label,
            const std::vector<Rsyn::Cell> &mapIndexToCell,
            const SquareCompressedRowMatrix &A);

        // Returns the net weight.
        double getNetWeight(Rsyn::Net net) const;

        // Define the movable cells.
        void markAsMovableAllNonFixedCells(
            const bool considerCriticalCellsAsFixed,
            std::set<Rsyn::Cell> &movable);
        void markAsMovableAllNonFixedCriticalCells(
            std::set<Rsyn::Cell> &movable);
        void markAsMovableAllNonFixedNonCriticalCellsDrivenByCriticalNets(
            std::set<Rsyn::Cell> &movable);
        void markAsMovableAllNonFixedCellsConnectedToCriticalNets(
            std::set<Rsyn::Cell> &movable);
        void markAsMovableAllNonFixedCriticalCellsAndTheirSinks(
            std::set<Rsyn::Cell> &movable);
        void markAsMovableAllNonFixedCriticalCellsAndTheirNeighbors(
            std::set<Rsyn::Cell> &movable);
        void markAsMovableAllNonFixedCellsInTopCriticalPaths(
            const int numPaths, std::set<Rsyn::Cell> &movable);
        void markAsMovableAllNonFixedCellsInKthTopCriticalPath(
            const int k, std::set<Rsyn::Cell> &movable);
        void
        markAsMovableAllNonFixedCellsInTopCriticalPathsFromTopCriticalEndpoints(
            const int numEndpoints, const int numPathsPerEndpoings,
            std::set<Rsyn::Cell> &movable);
        void markAsMovableAllNonFixedCellsInTopCriticalPathsAndTheirNeighbors(
            const int numPaths, std::set<Rsyn::Cell> &movable);

       public:
        QpdpB2B()
            : session(nullptr),
              design(nullptr),
              timer(nullptr),
              verySmallResistance(0.01),
              verySmallNetWeight(0.01) {}

        void init(Rsyn::Session ptr);

        void runPathStraightening();
        void runPathStraighteningUsingCliqueModel();

        bool canBeSetAsMovable(Rsyn::Cell cell,
                               const bool considerCriticalCellsAsFixed);
};  // end class

}  // end namespace

#endif
