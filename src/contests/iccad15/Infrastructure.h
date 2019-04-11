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

#ifndef ICCAD15_TDP_INFRASTRUCTURE_H
#define ICCAD15_TDP_INFRASTRUCTURE_H

#include <iostream>
using std::cout;
using std::endl;

#include <map>
using std::map;
#include <unordered_set>
#include <set>
using std::set;
#include <string>
using std::string;

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Session.h"
#include "tool/timing/Timer.h"
#include "ispd16/ABU.h"
#include "tool/jezz/Jezz.h"
#include "iccad15/utilICCAD15.h"

namespace Rsyn {
class LibraryCharacterizer;
class RoutingEstimator;
class WebLogger;
}  // end namespace

namespace ICCAD15 {

////////////////////////////////////////////////////////////////////////////////
// Session (ICCAD15 Framework)
////////////////////////////////////////////////////////////////////////////////

class BlockageControl;

class Infrastructure : public Rsyn::Service {
       public:
        Infrastructure();
        ~Infrastructure();

        virtual void start(const Rsyn::Json &params);
        virtual void stop();

       private:
        void init();

        // Session
        Rsyn::Session clsSession;

        // Physical layer
        Rsyn::PhysicalDesign clsPhysicalDesign;

        // Circuitry
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;

        // Services
        Rsyn::Timer *clsTimer = nullptr;
        ABU clsABU;
        const Rsyn::LibraryCharacterizer *clsLibraryCharacterizer = nullptr;
        BlockageControl *clsBlockageControl = nullptr;
        Rsyn::RoutingEstimator *clsRoutingEstimator = nullptr;
        Jezz *clsJezz = nullptr;

        // keeping fixed data from input file.
        Rsyn::Attribute<Rsyn::Instance, bool> clsFixedInInputFile;

        // Configuration
        // Note: Do not change these flags directly, use the configure methods
        // instead.
        bool clsEnableWarnings;

        ////////////////////////////////////////////////////////////////////////////
        // QoR
        ////////////////////////////////////////////////////////////////////////////

        struct QualityRecord {
                double abu;
                double wns[Rsyn::NUM_TIMING_MODES];
                double tns[Rsyn::NUM_TIMING_MODES];
                double stwl;

                QualityRecord() { clear(); }  // end constructor

                QualityRecord &operator+=(const QualityRecord &other) {
                        return *this = operator+(*this, other);
                }  // end method

                QualityRecord &operator-=(const QualityRecord &other) {
                        return *this = operator-(*this, other);
                }  // end method

                void clear() {
                        abu = 0;
                        wns[Rsyn::EARLY] = 0;
                        wns[Rsyn::LATE] = 0;
                        tns[Rsyn::EARLY] = 0;
                        tns[Rsyn::LATE] = 0;
                        stwl = 0;
                }  // end method

                friend QualityRecord operator+(const QualityRecord &left,
                                               const QualityRecord &right) {
                        QualityRecord result;
                        result.abu = left.abu + right.abu;
                        result.wns[Rsyn::EARLY] =
                            left.wns[Rsyn::EARLY] + right.wns[Rsyn::EARLY];
                        result.wns[Rsyn::LATE] =
                            left.wns[Rsyn::LATE] + right.wns[Rsyn::LATE];
                        result.tns[Rsyn::EARLY] =
                            left.tns[Rsyn::EARLY] + right.tns[Rsyn::EARLY];
                        result.tns[Rsyn::LATE] =
                            left.tns[Rsyn::LATE] + right.tns[Rsyn::LATE];
                        result.stwl = left.stwl + right.stwl;
                        return result;
                }  // end method

                friend QualityRecord operator-(const QualityRecord &left,
                                               const QualityRecord &right) {
                        QualityRecord result;
                        result.abu = left.abu - right.abu;
                        result.wns[Rsyn::EARLY] =
                            left.wns[Rsyn::EARLY] - right.wns[Rsyn::EARLY];
                        result.wns[Rsyn::LATE] =
                            left.wns[Rsyn::LATE] - right.wns[Rsyn::LATE];
                        result.tns[Rsyn::EARLY] =
                            left.tns[Rsyn::EARLY] - right.tns[Rsyn::EARLY];
                        result.tns[Rsyn::LATE] =
                            left.tns[Rsyn::LATE] - right.tns[Rsyn::LATE];
                        result.stwl = left.stwl - right.stwl;
                        return result;
                }  // end method

                friend QualityRecord operator/(const QualityRecord &left,
                                               const QualityRecord &right) {
                        QualityRecord result;
                        result.abu = left.abu / right.abu;
                        result.wns[Rsyn::EARLY] =
                            left.wns[Rsyn::EARLY] / right.wns[Rsyn::EARLY];
                        result.wns[Rsyn::LATE] =
                            left.wns[Rsyn::LATE] / right.wns[Rsyn::LATE];
                        result.tns[Rsyn::EARLY] =
                            left.tns[Rsyn::EARLY] / right.tns[Rsyn::EARLY];
                        result.tns[Rsyn::LATE] =
                            left.tns[Rsyn::LATE] / right.tns[Rsyn::LATE];
                        result.stwl = left.stwl / right.stwl;
                        return result;
                }  // end method

        };  // end struct

        ////////////////////////////////////////////////////////////////////////////
        // Move Track
        ////////////////////////////////////////////////////////////////////////////

        struct MoveRecord {
                // Total moves.
                long count;

                // Total moves that succeed (do not count moves rolled back due
                // to cost
                // increase).
                long success;

                // Total moves that failed (do not count moves rolled back due
                // to cost
                // increase).
                long fail;

                // Count the number of moves that failed due to legalization
                // issues.
                long fail_legalization;

                // Count the number of moves that failed due maximum
                // displacement
                // problems. This should be zero in general as the target
                // position is
                // clamped to meet the maximum displacement before the cell is
                // actually
                // moved.
                long fail_displacement;

                // Count the number of times that a fixed cell was tried to be
                // moved.
                // This should be zero in general.
                long fail_fixed;

                // Count the number of moves that were rolled back due to cost
                // increase.
                long fail_cost;

                // Constructor.
                MoveRecord() { clear(); }  // end constructor

                // Clean-up.
                void clear() {
                        count = 0;
                        success = 0;
                        fail = 0;
                        fail_legalization = 0;
                        fail_displacement = 0;
                        fail_fixed = 0;
                        fail_cost = 0;
                }  // end method

                // Get total number of moves executed.
                long getMoves() const { return count; }  // end method

                // Get total number of fails.
                long getFails(
                    const bool ignoreMovesRolledBackDueToCost = false) const {
                        return fail_displacement + fail_legalization +
                               fail_fixed +
                               (ignoreMovesRolledBackDueToCost ? 0 : fail_cost);
                }  // end method

                // Get number of fails due to legalization.
                long getFailsDueToLegalization() const {
                        return fail_legalization;
                }  // end method

                // Get number of fails due to cost increase.
                long getFailsDueToCostIncrease() const {
                        return fail_cost;
                }  // end method

                // Get total number of success.
                long getSuccesses(
                    const bool ignoreMovesRolledBackDueToCost = false) const {
                        return count - getFails(ignoreMovesRolledBackDueToCost);
                }  // end method

                // Get the miss rate.
                double getMissRate(
                    const bool ignoreMovesRolledBackDueToCost = false) const {
                        return getFails(ignoreMovesRolledBackDueToCost) /
                               double(count);
                }  // end method

                // Get the miss rate due to legalization issues.
                double getMissRateDueToLegalization() const {
                        return fail_legalization / double(count);
                }  // end method

                // Get the miss rate due to cost increase.
                double getMissRateDueToCostIncrease() const {
                        return fail_cost / double(count);
                }  // end method

                MoveRecord &operator+=(const MoveRecord &other) {
                        return *this = operator+(*this, other);
                }  // end method

                MoveRecord &operator-=(const MoveRecord &other) {
                        return *this = operator-(*this, other);
                }  // end method

                friend MoveRecord operator+(const MoveRecord &left,
                                            const MoveRecord &right) {
                        MoveRecord result;
                        result.count = left.count + right.count;
                        result.success = left.success + right.success;
                        result.fail = left.fail + right.fail;
                        result.fail_legalization =
                            left.fail_legalization + right.fail_legalization;
                        result.fail_displacement =
                            left.fail_displacement + right.fail_displacement;
                        result.fail_fixed = left.fail_fixed + right.fail_fixed;
                        result.fail_cost = left.fail_cost + right.fail_cost;
                        return result;
                }  // end method

                friend MoveRecord operator-(const MoveRecord &left,
                                            const MoveRecord &right) {
                        MoveRecord result;
                        result.count = left.count - right.count;
                        result.success = left.success - right.success;
                        result.fail = left.fail - right.fail;
                        result.fail_legalization =
                            left.fail_legalization - right.fail_legalization;
                        result.fail_displacement =
                            left.fail_displacement - right.fail_displacement;
                        result.fail_fixed = left.fail_fixed - right.fail_fixed;
                        result.fail_cost = left.fail_cost - right.fail_cost;
                        return result;
                }  // end method

        };  // end struct

        ////////////////////////////////////////////////////////////////////////////
        // Incremental Timing-Driven Algorithms
        ////////////////////////////////////////////////////////////////////////////

        // Run state
        QualityRecord clsOriginalQoR;
        MoveRecord clsMoves;
        double clsQualityScore;

        double clsTDPQualityScore;
        double clsBestQualityScore;

        // Gain
        double clsMaxGain[Rsyn::NUM_TIMING_MODES];
        Rsyn::Attribute<Rsyn::Instance, double>
            clsGains[Rsyn::NUM_TIMING_MODES];

        ////////////////////////////////////////////////////////////////////////////
        // Configuration
        ////////////////////////////////////////////////////////////////////////////

       private:
        bool clsEnableMaxDisplacementConstraint;

       public:
        bool isMaxDisplacementConstraintEnabled() {
                return clsEnableMaxDisplacementConstraint;
        }
        bool violateMaxDisplacement(Rsyn::PhysicalCell ph);
        bool violateMaxDisplacement(Rsyn::Cell cell);

        void configureMaxDisplacementConstraint(const bool enable) {
                clsEnableMaxDisplacementConstraint = enable;
        }  // end method

        ////////////////////////////////////////////////////////////////////////////
        // Result Trace
        ////////////////////////////////////////////////////////////////////////////

       private:
        // Tracking
        std::vector<std::tuple<std::string, QualityRecord>> clsHistoryQoR;
        std::vector<std::tuple<std::string, MoveRecord>> clsHistoryMoves;

       public:
        inline void resetBestQualityScore() {
                std::cout << "Reseting best QS...\n";
                clsBestQualityScore = 0;
        }  // end method

        bool updateTDPSolution(
            const bool force = false,
            const double expectedImprovementPerIteration = 0.01);

        ////////////////////////////////////////////////////////////////////////////
        // Solution
        ////////////////////////////////////////////////////////////////////////////

       private:
        QualityRecord createQualityRecord() const;
        MoveRecord createMoveRecord() const;

        double computeQualityScore(const QualityRecord &originalQoR,
                                   const QualityRecord &qor) const;
        double computeQualityScore(const QualityRecord &originalQoR) const;

       public:
        void statePush(const std::string &step);
        void stateClear();
        void stateReportQuality();
        void stateReportMoves();

        // Requires up-to-date timing information.
        void updateGains();
        double getGain(Rsyn::Cell cell, const Rsyn::TimingMode mode) const {
                return clsGains[mode][cell];
        }
        double getNormalizedGain(Rsyn::Cell cell,
                                 const Rsyn::TimingMode mode) const {
                return clsGains[mode][cell] / clsMaxGain[mode];
        }

        void updatePriorities() { updateGains(); }
        double getPriority(Rsyn::Cell cell, const Rsyn::TimingMode mode) const {
                return getNormalizedGain(cell, mode);
        }

        double getPinImportance(Rsyn::Pin pin, const Rsyn::TimingMode mode) {
                return (2 * clsTimer->getPinCentrality(pin, mode) +
                        clsTimer->getPinCriticality(pin, mode)) /
                       3;
        }  // end method

       public:
        void updateQualityScore();

        double getQualityScore() const { return clsQualityScore; }

        void restoreBestSolution();
        void storeSolution(const std::string &name);
        void restoreSolution(const std::string &name, const bool full = false);

        ////////////////////////////////////////////////////////////////////////////
        // Placement
        ////////////////////////////////////////////////////////////////////////////

       public:
        bool findNearestValidWhitespaceToPlaceCell(
            Rsyn::PhysicalCell physicalCell, const DBU targetX,
            const DBU targetY, const DBU maxDisplacementFromTargetPosition,
            const DBU maxDisplacementFromSourcePosition, DBU &resultX,
            DBU &resultY);

        bool findNearestValidWhitespaceToPlaceCellAroundBlock(
            Rsyn::PhysicalCell physicalCell, const DBU targetX,
            const DBU targetY, const DBU maxDisplacementFromTargetPosition,
            const DBU maxDisplacementFromSourcePosition, DBU &resultX,
            DBU &resultY);

        void computeBoundedPosition(const Rsyn::PhysicalCell physicalCell,
                                    const DBU x, const DBU y, DBU &boundedx,
                                    DBU &boundedy) const;

        bool moveCell(Rsyn::PhysicalCell physicalCell, const DBU x, const DBU y,
                      const LegalizationMethod legalization,
                      const bool rollback = true);
        bool moveCell(Rsyn::Cell cell, const DBU x, const DBU y,
                      const LegalizationMethod legalization,
                      const bool rollback = true);
        bool moveCell(Rsyn::PhysicalCell physicalCell, const DBUxy pos,
                      const LegalizationMethod legalization,
                      const bool rollback = true);
        bool moveCell(Rsyn::Cell cell, const DBUxy pos,
                      const LegalizationMethod legalization,
                      const bool rollback = true);

        bool translateCell(Rsyn::PhysicalCell physicalCell, const DBU dx,
                           const DBU dy, const LegalizationMethod legalization,
                           const bool rollback = true);
        bool translateCell(Rsyn::Cell cell, const DBU dx, const DBU dy,
                           const LegalizationMethod legalization,
                           const bool rollback = true);
        bool translateCell(Rsyn::Cell cell, const DBUxy displacement,
                           const LegalizationMethod legalization,
                           const bool rollback = true);
        bool translateCell(Rsyn::PhysicalCell physicalCell,
                           const DBUxy displacement,
                           const LegalizationMethod legalization,
                           const bool rollback = true);

        // Similar to move cell, but if the move fails, try again in a slight
        // different position along with the straight line connecting the
        // current
        // cell position and the target position.
        // Step size is should be in (0, 1] range.
        // Cell is kept in its current position if another position is not
        // found.
        void moveCellTowards(Rsyn::PhysicalCell physicalCell,
                             const DBUxy targetPos,
                             const LegalizationMethod legalization,
                             const double stepSize);
        void moveCellTowards(Rsyn::PhysicalCell physicalCell, const DBU x,
                             const DBU y, const LegalizationMethod legalization,
                             const double stepSize);
        void moveCellTowards(Rsyn::Cell cell, const DBUxy targetPos,
                             const LegalizationMethod legalization,
                             const double stepSize);
        void moveCellTowards(Rsyn::Cell cell, const DBU x, const DBU y,
                             const LegalizationMethod legalization,
                             const double stepSize);

        // Methods to move cells and automatically rollback if the move worsen
        // some cost metric. Use the cached version if you will try several
        // moves
        // (e.g. north, south, east, west). In this case you are responsible to
        // provide the current (old) cost.
        // If the move fail, you can check if newCost > oldCost to check if the
        // fail was due to a cost increase (and not due to legalization fail).

        bool moveCellWithCostEvaluation(Rsyn::Cell cell, const DBU x,
                                        const DBU y,
                                        const LegalizationMethod legalization,
                                        const CostFunction costFunction,
                                        const Rsyn::TimingMode mode,
                                        double &oldCost, double &newCost);
        bool moveCellWithCostEvaluationCached(
            Rsyn::Cell cell, const DBU x, const DBU y,
            const LegalizationMethod legalization,
            const CostFunction costFunction, const Rsyn::TimingMode mode,
            const double oldCost, double &newCost);

        bool translateCellWithCostEvaluation(
            Rsyn::Cell cell, const DBU dx, const DBU dy,
            const LegalizationMethod legalization,
            const CostFunction costFunction, const Rsyn::TimingMode mode,
            double &oldCost, double &newCost);
        bool translateCellWithCostEvaluationCached(
            Rsyn::Cell cell, const DBU dx, const DBU dy,
            const LegalizationMethod legalization,
            const CostFunction costFunction, const Rsyn::TimingMode mode,
            const double oldCost, double &newCost);

        ////////////////////////////////////////////////////////////////////////////
        // Cost Evaluation
        ////////////////////////////////////////////////////////////////////////////

       private:
        // Cost Computation
        double computeCost_Wirelength(Rsyn::Cell cell,
                                      const Rsyn::TimingMode mode);
        double computeCost_RC(Rsyn::Cell cell, const Rsyn::TimingMode mode);
        double computeCost_LocalDelay(Rsyn::Cell cell,
                                      const Rsyn::TimingMode mode);

        void computeCost_UpdateAfterMove(Rsyn::Cell cell,
                                         const CostFunction costFunction);

       public:
        double computeCost(Rsyn::Cell cell, const Rsyn::TimingMode mode,
                           const CostFunction costFunction);

        ////////////////////////////////////////////////////////////////////////////
        // ABU
        ////////////////////////////////////////////////////////////////////////////

        const ABU &getAbuAnalyser() const { return clsABU; }
        ABU &getAbuAnalyser() { return clsABU; }
        void coloringABU() { clsABU.coloringABU(); }
        void coloringABUViolations() { clsABU.coloringABUViolations(); }
        unsigned getAbuNumCols() { return clsABU.getAbuNumCols(); }
        unsigned getAbuNumRows() { return clsABU.getAbuNumRows(); }
        Bounds getABUBounds(const unsigned row, const unsigned col) {
                return clsABU.getABUBounds(row, col);
        }
        Color &getABUBinColor(const unsigned row, const unsigned col) {
                return clsABU.getABUBinColor(row, col);
        }  // end method
        double getAbu() const { return clsABU.getAbu(); }
        void removeAbuUtilization(DBUxy lower, DBUxy upper, bool fixedInInput) {
                clsABU.removeAbuUtilization(lower, upper, fixedInInput);
        }
        void addAbuUtilization(DBUxy lower, DBUxy upper, bool fixedInInput) {
                clsABU.addAbuUtilization(lower, upper, fixedInInput);
        }

        void initAbu(Rsyn::PhysicalDesign phDesign, Rsyn::Module module,
                     double targetUtilization, double unit = 9.0) {
                clsABU.initAbu(phDesign, module, targetUtilization, unit);
        }
        void updateAbu(bool showDetails) { clsABU.updateAbu(showDetails); }
        ////////////////////////////////////////////////////////////////////////////
        // Legalization
        ////////////////////////////////////////////////////////////////////////////

       public:
        Jezz *getJezz() { return clsJezz; }

        bool legalizeCell(Rsyn::PhysicalCell physicalCell,
                          const LegalizationMethod legalization,
                          const bool rollback = true);
        bool legalizeCell(Rsyn::Cell cell,
                          const LegalizationMethod legalization,
                          const bool rollback = true);

        void unlegalizeCell(Rsyn::PhysicalCell physicalCell);
        void unlegalizeCell(Rsyn::Cell cell);

        void legalize();

        bool isLegalized(Rsyn::Cell cell);

        // Return available free space at right and left of cell.
        double getAvailableFreespace(Rsyn::Cell cell);

        // Return available free space at right of cell.
        double getAvailableFreespaceRight(Rsyn::Cell cell);

        // Return available free space at left of cell.
        double getAvailableFreespaceLeft(Rsyn::Cell cell);

        void diffJezzSolutions(std::ostream &out, const int numCells,
                               const std::string solution0,
                               const std::string solution1,
                               const bool enableCriticality = false);

        ////////////////////////////////////////////////////////////////////////////
        // Optimization
        ////////////////////////////////////////////////////////////////////////////

       public:
        void run(const std::string &flow);
        void runFluteLoopWorkaround();

        ////////////////////////////////////////////////////////////////////////////
        // Analysis and Statistics
        ////////////////////////////////////////////////////////////////////////////

       public:
        void renderCriticalNetHistogram(const std::string &title);
        void displacementHistrogram(ostream &out);

        bool checkMaxDisplacementViolation(const std::string &errorMsg = "",
                                           const bool hideMessageIfPass = true);
        bool checkUnlegalizedCells(const std::string &errorMsg = "",
                                   const bool hideMessageIfPass = true);

        ////////////////////////////////////////////////////////////////////////////
        // Misc
        ////////////////////////////////////////////////////////////////////////////

       public:
        bool isInverterOrBuffer(Rsyn::Instance cell) {
                return cell.getNumInputPins() == 1 &&
                       cell.getNumOutputPins() == 1 && !cell.isPort();
        }
        bool isInverterOrBuffer(Rsyn::Pin pin) {
                return isInverterOrBuffer(pin.getInstance());
        }

        double computeManhattanDistance(const double x0, const double y0,
                                        const double x1,
                                        const double y1) const {
                return std::abs(x0 - x1) + std::abs(y0 - y1);
        }
        double computeManhattanDistance(const DBUxy p0, const DBUxy p1) const {
                return std::abs(p0.x - p1.x) + std::abs(p0.y - p1.y);
        }
        double computeManhattanDistance(Rsyn::Pin pin0, Rsyn::Pin pin1) {
                return computeManhattanDistance(
                    clsPhysicalDesign.getPinPosition(pin0),
                    clsPhysicalDesign.getPinPosition(pin1));
        }
        double computeManhattanDistanceFromInitialPosition(
            const PhysicalCell &ph) const {
                return computeManhattanDistance(ph.getPosition(),
                                                ph.getInitialPosition());
        }
        double computeManhattanDistanceFromInitialPosition(
            Rsyn::Cell cell) const {
                return computeManhattanDistanceFromInitialPosition(
                    clsPhysicalDesign.getPhysicalCell(cell));
        }
        double computeEuclidianDistance(const DBUxy p0, const DBUxy p1) const {
                return std::sqrt(std::pow(p0.x - p1.x, 2.0) +
                                 std::pow(p0.y - p1.y, 2.0));
        }

        double computePathManhattanLength(
            const std::vector<Rsyn::Timer::PathHop> &path);
        void reportPathManhattanLengthBySegment(
            const std::vector<Rsyn::Timer::PathHop> &path,
            ostream &ostream = std::cout);

        ////////////////////////////////////////////////////////////////////////////
        // Reports
        ////////////////////////////////////////////////////////////////////////////

       public:
        void reportDigest();
        void reportSummary(const std::string &step);
        void reportFinalResult();
        void reportCellPlacement(Rsyn::Cell cell);
        void reportInvsDigest();
        void reportTopCentralCells(const Rsyn::TimingMode mode,
                                   const int maxNumCellsToReport,
                                   const bool showButDontCountFixedCells);
        void reportTopCriticalCells(const Rsyn::TimingMode mode,
                                    const int maxNumCellsToReport,
                                    const bool showButDontCountFixedCells);
        void reportPathLengths(
            const std::string &label,
            const std::vector<std::vector<Rsyn::Timer::PathHop>> &paths);
        void reportNetDegreeHistogram();

        void reportDigestInline(const bool header, const bool newLine);

        /* This method queries the top critical N end points plus the top M
         * critical
         * paths in the circuit and counts how many paths there are in each end
         * point such that does not surpass the WNS on next top end point  */
        void reportCountOfTopCriticalPathsOnCriticalEndPoints(
            std::ostream &out, int numEndPoints = 50, int numPaths = 5000);
        void reportSlackOnTopCriticalEndPoints(std::ostream &out,
                                               const int numEndPoints);

        void reportTotalSlackOnTopCriticalEndPoints(
            std::ostream &out, const int numPathsPerEndPoint);

        void reportSlackOnTopEndPoints(std::ostream &out,
                                       const int numEndPoints);

        void reportCurrLoadOnCriticalPath(
            std::ostream &out, std::vector<Rsyn::Timer::PathHop> path);

        void reportCurrLoadOnTopCriticalPaths(std::ostream &out,
                                              const int numPaths);

        void reportLoadOnTopCriticalNets(std::ostream &out);

        ////////////////////////////////////////////////////////////////////////////
        // Configuration
        ////////////////////////////////////////////////////////////////////////////

       private:
        DBU clsMaxDisplacement;
        double clsTargetUtilization;

       public:
        DBU getMaxDisplacement() const {
                return clsMaxDisplacement *
                       clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        }  // end method

        double getTargetUtilization() const {
                return clsTargetUtilization;
        }  // end method

        void configureWarnings(const bool enable) {
                clsEnableWarnings = enable;
        }  // end method

        void setMaxDisplacement(const DBU value) {
                clsMaxDisplacement = value;
        }  // end method

        void setTargetUtilization(const double value) {
                clsTargetUtilization = value;
        }  // end method

};  // end class

}  // end namespace

#endif
