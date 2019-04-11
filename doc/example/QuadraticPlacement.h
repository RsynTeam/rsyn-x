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

#ifndef QUADRATIC_PLACEMENT_EXAMPLE_OPTO_H
#define QUADRATIC_PLACEMENT_EXAMPLE_OPTO_H

#include <iostream>
#include <vector>

#include "tool/lnalg/lnalg.h"
#include "tool/lnalg/scrm.h"
#include "iccad15/Infrastructure.h"
#include <Rsyn/Session>
#include <Rsyn/PhysicalDesign>
#include "tool/qpdp/RelaxedPinPosition.h"

namespace ICCAD15 {

class QuadraticPlacementExample : public Rsyn::Process {
       private:
        Rsyn::Session session;
        Infrastructure *infra;
        Rsyn::Design design;
        Rsyn::Module module;
        Rsyn::PhysicalDesign phDesign;
        Rsyn::Timer *timer;
        ICCAD15::RelaxedPinPosition *clsRelaxedPinPos;

        // These methods are intended as examples of how to build the quadratic
        // placement linear system using the session. They are not necessarily
        // optimized.

        void runQuadraticPlacementUsingHybridNetModel();
        void runQuadraticPlacementUsinbBound2BoundNetModel();

        // Build linear system using the hybrid (clique + star) model.
        void buildLinearSystemUsingHybridNetModel(
            const std::set<Rsyn::Instance> &movable,
            SymmetricCompressedRowMatrix &A, std::vector<double> &bx,
            std::vector<double> &by,
            std::map<Rsyn::Instance, int> &mapCellToIndex, int &numStarNodes);

        // Build linear system using b2b (bound-2-bound) model.
        void buildLinearSystemUsingBound2BoundModel(
            const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
            const std::vector<Rsyn::Instance> &mapIndexToCell,
            SquareCompressedRowMatrix &Ax, SquareCompressedRowMatrix &Ay,
            std::vector<double> &bx, std::vector<double> &by,
            const double epsilon);

        void buildLinearSystemUsingBound2BoundModel_AddConnections(
            const Rsyn::Attribute<Rsyn::Instance, int> &mapCellToIndex,
            const double epsilon, Rsyn::Net net,
            const std::vector<std::tuple<double, Rsyn::Pin>> &pins,
            SparseMatrixDescriptor &dscp, std::vector<double> &b, double &hpwl);

        void buildLinearSystemUsingBound2BoundModel_Add(
            const int numPins,

            const int index0, const double pos0, const int index1,
            const double pos1,

            const double epsilon,

            SparseMatrixDescriptor &dscp, std::vector<double> &b, double &hpwl);

        void buildLinearSystemUsingBound2BoundModel_CheckConsistency(
            const std::string &label,
            const std::vector<Rsyn::Instance> &mapIndexToCell,
            const SquareCompressedRowMatrix &A);

        // Get the net bounding box using the cell positions. However, for
        // macro-
        // blocks the pin position is used instead.
        void getNetBoundingBoxUsingRelaxedPinPositions(Rsyn::Net net, DBU &xmin,
                                                       DBU &ymin, DBU &xmax,
                                                       DBU &ymax);

        // Get the boundary cells of the net.
        void getNetBoundaryPinsUsingRelaxedPinPositions(Rsyn::Net net,
                                                        Rsyn::Pin &left,
                                                        Rsyn::Pin &right,
                                                        Rsyn::Pin &bottom,
                                                        Rsyn::Pin &top);

        void getNetBoundaryPinsUsingRelaxedPinPositions(Rsyn::Net net,
                                                        const Dimension dim,
                                                        Rsyn::Pin &lower,
                                                        Rsyn::Pin &upper);

        // Get the pins of net sorted by their (relaxed) position.
        // If there's a tie in the pin position, the internal pin ordering
        // provided
        // by Rsyn is used.
        //
        //  -------------------
        // | 0 | 1 | 2 |...| n |
        //  -------------------
        // 0        : lower boundary pin
        // [1, n-1] : inner pins
        // n        : upper boundary pin

        void getNetPinsSortedByRelaxedPinPositions(
            Rsyn::Net net, const Dimension dimension,
            std::vector<std::tuple<double, Rsyn::Pin>> &pins);

       public:
        virtual bool run(const Rsyn::Json &params);

};  // end class
}
#endif
