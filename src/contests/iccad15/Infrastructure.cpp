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

/*
 * Session.cpp
 *
 *  Created on: May 8, 2015
 *      Author: jucemar
 */

#include <thread>
#include <iosfwd>
#include <mutex>

#include "iccad15/Infrastructure.h"
#include "session/Session.h"
#include "tool/library/LibraryCharacterizer.h"
#include "tool/routing/RoutingEstimator.h"

#include "util/Json.h"

#include "util/Array.h"
#include "util/AsciiProgressBar.h"
#include "util/StreamStateSaver.h"
#include "util/Stepwatch.h"
#include "util/FloatingPoint.h"
#include "util/Environment.h"

#include "phy/PhysicalDesign.h"
#include "phy/util/PhysicalUtil.h"
#include "phy/util/LefDescriptors.h"
#include "phy/util/LefDescriptors.h"

#include "io/parser/verilog/SimplifiedVerilogReader.h"
#include "util/BlockageControl.h"

namespace ICCAD15 {

Infrastructure::Infrastructure() {
        clsTimer = nullptr;
        clsLibraryCharacterizer = nullptr;
        clsJezz = nullptr;

        clsEnableWarnings = true;
        clsEnableMaxDisplacementConstraint = true;
}  // end constructor

// -----------------------------------------------------------------------------

Infrastructure::~Infrastructure() { delete clsJezz; }  // end destructor

// -----------------------------------------------------------------------------

void Infrastructure::start(const Rsyn::Json &params) {
        Rsyn::Session session;

        this->clsDesign = session.getDesign();

        const double defaultTargetUtilization = 0.85;
        if (!params.count("targetUtilization")) {
                std::cout << "[WARNING] Parameter \"targetUtilization\" not "
                             "specified "
                          << "assuming default value "
                          << defaultTargetUtilization << ".\n";
        }
        clsTargetUtilization =
            params.value("targetUtilization", defaultTargetUtilization);

        const DBU defaultMaxDisplacement = 400;
        if (!params.count("maxDisplacement")) {
                std::cout
                    << "[WARNING] Parameter \"maxDisplacement\" not specified "
                    << "assuming default value " << defaultMaxDisplacement
                    << ".\n";
        }
        clsMaxDisplacement =
            params.value("maxDisplacement", defaultMaxDisplacement);

        Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
        initAbu(physicalDesign, clsDesign.getTopModule(), clsTargetUtilization);

        updateAbu(true);
        init();

        {  // reportDigest
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportDigest");
                dscp.setDescription("Reports the QoR of the current solution.");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            reportDigest();
                    });
        }  // end block

        {  // moveCell
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("moveCell");
                dscp.setDescription("Moves a cell to a given position.");

                dscp.addPositionalParam(
                    "cell", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_MANDATORY, "Target cell.");

                dscp.addPositionalParam("x", ScriptParsing::PARAM_TYPE_INTEGER,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Target X position.");

                dscp.addPositionalParam("y", ScriptParsing::PARAM_TYPE_INTEGER,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Target Y position.");

                dscp.addNamedParam(
                    "legalize", ScriptParsing::PARAM_TYPE_BOOLEAN,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "If enabled, calls legalization after the movement.",
                    "false");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string cellName =
                                command.getParam("cell");
                            const Rsyn::Cell cell =
                                clsDesign.findCellByName(cellName);
                            const DBU x = (int)command.getParam("x");
                            const DBU y = (int)command.getParam("y");
                            const bool legalize = command.getParam("legalize");

                            if (!cell) {
                                    std::cout << "[ERROR] Cell \"" << cellName
                                              << "\" not found.\n";
                                    return;
                            }

                            moveCell(cell, x, y, (legalize)
                                                     ? LEG_NEAREST_WHITESPACE
                                                     : LEG_NONE);
                    });
        }  // end block

        reportDigest();
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::stop() {}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::init() {
        Rsyn::Session session;

        // Services.
        clsTimer = session.getService("rsyn.timer");
        clsLibraryCharacterizer =
            session.getService("rsyn.libraryCharacterizer");
        clsRoutingEstimator = session.getService("rsyn.routingEstimator");
        clsJezz = session.getService("rsyn.jezz");
        clsBlockageControl = nullptr;

        // Circuitry.
        clsPhysicalDesign = session.getPhysicalDesign();
        clsDesign = session.getDesign();
        clsModule = session.getTopModule();

        // Initial values.
        clsTDPQualityScore = 0;
        clsBestQualityScore = 0;

        // Store the original QoR.
        clsOriginalQoR = createQualityRecord();

        // Gains
        clsGains[Rsyn::EARLY] = clsDesign.createAttribute();
        clsGains[Rsyn::LATE] = clsDesign.createAttribute();

        clsFixedInInputFile = clsDesign.createAttribute(false);
        for (Rsyn::Instance inst : clsModule.allInstances())
                clsFixedInInputFile[inst] = inst.isFixed();

        // Legalize.
        Stepwatch watchJezz("Legalizing");
        checkMaxDisplacementViolation("before initial legalization");
        clsJezz->jezz_Legalize();
        checkMaxDisplacementViolation("after initial legalization");
        watchJezz.finish();

        // Store initial solution as the best one.
        clsJezz->jezz_storeSolution("best");
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::updateQualityScore() {
        clsABU.updateAbu();
        clsQualityScore = computeQualityScore(clsOriginalQoR);
}  // end method

// -----------------------------------------------------------------------------

// backup current solution
bool Infrastructure::updateTDPSolution(
    const bool force, const double expectedImprovementPerIteration) {
        const bool displacementViolation = !checkMaxDisplacementViolation();
        const bool legalizationViolation =
            !checkUnlegalizedCells("@LegCheck", true);

        bool keep_going;

        updateQualityScore();
        const double qorImprovement =
            (clsQualityScore - clsTDPQualityScore) / 1800;
        if (force || (FloatingPoint::definitelyGreaterThan(
                          clsQualityScore, clsTDPQualityScore) &&
                      !displacementViolation)) {
                // Store solution.
                clsJezz->jezz_storeSolution();

                // Stop if the quality score improved just a little bit.
                keep_going =
                    force ||
                    (clsTDPQualityScore > 0
                         ? (clsQualityScore - clsTDPQualityScore) / 1800 >=
                               expectedImprovementPerIteration
                         : true);

                // Store the new quality score.
                clsTDPQualityScore = clsQualityScore;

        } else {
                // Restore previous better solution.
                clsJezz->jezz_restoreSolutionIncremental();
                clsQualityScore = clsTDPQualityScore;

                clsTimer->updateTimingIncremental();
                reportSummary("rollback");

                keep_going = false;
        }  // end if-else

        // Store the best solution.
        if (clsQualityScore > clsBestQualityScore) {
                clsJezz->jezz_storeSolution("best");
                clsBestQualityScore = clsQualityScore;
        }  // end if

        StreamStateSaver sss(std::cout);
        std::cout << "QoR improvement: " << std::setprecision(2) << std::fixed
                  << std::showpos << std::boolalpha << (100 * qorImprovement)
                  << "% "
                  << "(keep going = " << keep_going << ")\n";

        return keep_going;
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::restoreBestSolution() {
        restoreSolution("best");
        reportSummary("restore-best-solution");
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::storeSolution(const std::string &name) {
        if (clsJezz) {
                clsJezz->jezz_storeSolution(name);
        } else {
                std::cout << "WARNING: Jezz is not initialized. Unable to "
                             "store the current solution.\n";
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::restoreSolution(const std::string &name, const bool full) {
        // Restore previous better solution.
        if (full) {
                clsJezz->jezz_restoreSolutionFull(name);
                clsTimer->updateTimingFull();
        } else {
                clsJezz->jezz_restoreSolutionIncremental(name);
                clsTimer->updateTimingIncremental();
        }  // end method

        // Update quality score.
        updateQualityScore();

        // Update current solution as well.
        clsJezz->jezz_storeSolution();
        clsTDPQualityScore = clsQualityScore;
}  // end method

// -----------------------------------------------------------------------------

double Infrastructure::computeQualityScore(const QualityRecord &originalQoR,
                                           const QualityRecord &qor) const {
        const double w_tns = 2.0;
        const double w_wns = 1.0;
        const double w_abu = 1.0;
        const double w_e = 1.0;
        const double w_l = 5.0;

        const double eTnsImprovement =
            1.0 -
            (originalQoR.tns[Rsyn::EARLY] == 0
                 ? 1
                 : qor.tns[Rsyn::EARLY] /
                       originalQoR
                           .tns[Rsyn::EARLY]);  // avoiding division by zero
        const double lTnsImprovement =
            1.0 -
            (originalQoR.tns[Rsyn::LATE] == 0
                 ? 1
                 : qor.tns[Rsyn::LATE] /
                       originalQoR
                           .tns[Rsyn::LATE]);  // avoiding division by zero
        const double eWnsImprovement =
            1.0 -
            (originalQoR.wns[Rsyn::EARLY] == 0
                 ? 1
                 : qor.wns[Rsyn::EARLY] /
                       originalQoR
                           .wns[Rsyn::EARLY]);  // avoiding division by zero
        const double lWnsImprovement =
            1.0 -
            (originalQoR.wns[Rsyn::LATE] == 0
                 ? 1
                 : qor.wns[Rsyn::LATE] /
                       originalQoR
                           .wns[Rsyn::LATE]);  // avoiding division by zero

        const double slackImprovement =
            100.0 * (w_tns * (w_e * eTnsImprovement + w_l * lTnsImprovement) +
                     w_wns * (w_e * eWnsImprovement + w_l * lWnsImprovement));
        const double deltaAbu = qor.abu - originalQoR.abu;

        return slackImprovement * (1.0 - w_abu * deltaAbu);
}  // end method

// -----------------------------------------------------------------------------

double Infrastructure::computeQualityScore(
    const QualityRecord &originalQoR) const {
        return computeQualityScore(originalQoR, createQualityRecord());
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::updateGains() {
        clsMaxGain[Rsyn::EARLY] = 0;
        clsMaxGain[Rsyn::LATE] = 0;

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                //	bool debug = false; unused variable
                double avgGain[Rsyn::NUM_TIMING_MODES] = {0, 0};
                for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
                        Rsyn::Net net = pin.getNet();
                        if (!net) continue;

                        // Only count the cap that can be reduced by a better
                        // placement.
                        const double load = clsRoutingEstimator->getRCTree(net)
                                                .getTotalWireCap();

                        for (const Rsyn::TimingMode mode :
                             Rsyn::allTimingModes()) {
                                Rsyn::Arc arc =
                                    clsTimer->getCriticalTimingArcToPin(pin,
                                                                        mode);
                                if (!arc) continue;

                                const double rdriver =
                                    clsLibraryCharacterizer
                                        ->getDriverResistance(arc, mode);
                                avgGain[mode] += rdriver * load *
                                                 getPinImportance(pin, mode);
                        }  // end for
                }          // end for

                if (cell.getNumOutputPins() > 0) {
                        avgGain[Rsyn::EARLY] /= cell.getNumOutputPins();
                        avgGain[Rsyn::LATE] /= cell.getNumOutputPins();
                }  // end if

                for (const Rsyn::TimingMode mode : Rsyn::allTimingModes()) {
                        clsGains[mode][cell] = avgGain[mode];
                        clsMaxGain[mode] =
                            std::max(clsMaxGain[mode], avgGain[mode]);
                }  // end for
        }          // end method

        std::cout << "max gain " << clsMaxGain[Rsyn::LATE] << "\n";
}  // end method

////////////////////////////////////////////////////////////////////////////////
// QoR and Optimization Tracking
////////////////////////////////////////////////////////////////////////////////

Infrastructure::MoveRecord Infrastructure::createMoveRecord() const {
        return clsMoves;  // just a copy of current stats
}  // end method

// -----------------------------------------------------------------------------

Infrastructure::QualityRecord Infrastructure::createQualityRecord() const {
        QualityRecord qor;
        qor.abu = getAbu();
        qor.tns[Rsyn::EARLY] = clsTimer->getTns(Rsyn::EARLY);
        qor.tns[Rsyn::LATE] = clsTimer->getTns(Rsyn::LATE);
        qor.wns[Rsyn::EARLY] = clsTimer->getWns(Rsyn::EARLY);
        qor.wns[Rsyn::LATE] = clsTimer->getWns(Rsyn::LATE);
        qor.stwl = clsRoutingEstimator->getTotalWirelength();
        return qor;
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::statePush(const std::string &step) {
        clsHistoryQoR.push_back(std::make_tuple(step, createQualityRecord()));
        clsHistoryMoves.push_back(std::make_tuple(step, clsMoves));
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::stateClear() {
        clsHistoryQoR.clear();
        clsHistoryMoves.clear();
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::stateReportQuality() {
        const int numRecords = clsHistoryQoR.size();
        if (numRecords <= 2) {
                // First and last records should store the initial and final
                // results.
                return;
        }  // end if

        std::map<std::string, QualityRecord> cummulativeDeltas;

        const QualityRecord initialRecord = std::get<1>(clsHistoryQoR.front());
        const QualityRecord finalRecord = std::get<1>(clsHistoryQoR.back());
        const QualityRecord improvementRecord = finalRecord - initialRecord;

        QualityRecord previousQoR = initialRecord;
        for (int i = 1; i < numRecords - 1; i++) {
                const std::string &step = std::get<0>(clsHistoryQoR[i]);
                const QualityRecord &currentQoR = std::get<1>(clsHistoryQoR[i]);

                cummulativeDeltas[step] += currentQoR - previousQoR;
                previousQoR = currentQoR;
        }  // end for

        StreamStateSaver sss(std::cout);
        std::cout << std::setprecision(2) << std::fixed;

        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Report: Quality Improvement per Step (%)\n";
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << std::setw(7) << "ABU"
                  << " ";
        std::cout << std::setw(7) << "StWL"
                  << " ";
        std::cout << std::setw(7) << "eWNS"
                  << " ";
        std::cout << std::setw(7) << "eTNS"
                  << " ";
        std::cout << std::setw(7) << "lWNS"
                  << " ";
        std::cout << std::setw(7) << "lTNS"
                  << " ";
        std::cout << "Step"
                  << "\n";

        QualityRecord sanityRecord = initialRecord;
        for (const std::tuple<std::string, QualityRecord> &t :
             cummulativeDeltas) {
                const std::string &step = std::get<0>(t);
                const QualityRecord &aggregated = std::get<1>(t);
                const QualityRecord &change = aggregated / improvementRecord;

                std::cout << std::setw(7) << (100 * change.abu) << " ";
                std::cout << std::setw(7) << (100 * change.stwl) << " ";
                std::cout << std::setw(7) << (100 * change.wns[Rsyn::EARLY])
                          << " ";
                std::cout << std::setw(7) << (100 * change.tns[Rsyn::EARLY])
                          << " ";
                std::cout << std::setw(7) << (100 * change.wns[Rsyn::LATE])
                          << " ";
                std::cout << std::setw(7) << (100 * change.tns[Rsyn::LATE])
                          << " ";
                std::cout << step << "\n";

                sanityRecord += aggregated;
        }  // end for

        std::cout << "QS (Final)        : "
                  << computeQualityScore(initialRecord, finalRecord) << "\n";
        std::cout << "QS (Sanity Check) : "
                  << computeQualityScore(initialRecord, sanityRecord) << "\n";

        std::cout << "---------------------------------------------------------"
                     "-----------------------\n";
        std::cout << std::flush;
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::stateReportMoves() {
        const int numRecords = clsHistoryMoves.size();
        if (numRecords <= 2) {
                // First and last records should store the initial and final
                // results.
                return;
        }  // end if

        std::map<std::string, MoveRecord> cummulativeDeltas;

        const MoveRecord initialRecord = std::get<1>(clsHistoryMoves.front());
        const MoveRecord finalRecord = std::get<1>(clsHistoryMoves.back());

        MoveRecord previousRecord = initialRecord;
        for (int i = 1; i < numRecords - 1; i++) {
                const std::string &step = std::get<0>(clsHistoryMoves[i]);
                const MoveRecord &currentRecord =
                    std::get<1>(clsHistoryMoves[i]);

                cummulativeDeltas[step] += currentRecord - previousRecord;
                previousRecord = currentRecord;
        }  // end for

        StreamStateSaver sss(std::cout);
        std::cout << std::setprecision(2) << std::fixed;

        std::cout << "========================================================="
                     "=======================\n";
        std::cout << "Report: Moves per Step (%)\n";
        std::cout << "========================================================="
                     "=======================\n";
        std::cout << std::setw(7) << "Miss(1)"
                  << " ";
        std::cout << std::setw(7) << "Miss(2)"
                  << " ";
        std::cout << std::setw(8) << "Total"
                  << " ";
        std::cout << std::setw(8) << "Fails(1)"
                  << " ";
        std::cout << std::setw(8) << "Fails(2)"
                  << " ";
        std::cout << "Step"
                  << "\n";

        MoveRecord sanityRecord = initialRecord;
        for (const std::tuple<std::string, MoveRecord> &t : cummulativeDeltas) {
                const std::string &step = std::get<0>(t);
                const MoveRecord &aggregated = std::get<1>(t);

                if (aggregated.getMoves() > 0)
                        std::cout
                            << std::setw(7)
                            << (100 * aggregated.getMissRateDueToLegalization())
                            << " ";
                else
                        std::cout << std::setw(7) << "-"
                                  << " ";

                if (aggregated.getMoves() > 0)
                        std::cout
                            << std::setw(7)
                            << (100 * aggregated.getMissRateDueToCostIncrease())
                            << " ";
                else
                        std::cout << std::setw(7) << "-"
                                  << " ";

                std::cout << std::setw(8) << aggregated.getMoves() << " ";
                std::cout << std::setw(8)
                          << aggregated.getFailsDueToLegalization() << " ";
                std::cout << std::setw(8)
                          << aggregated.getFailsDueToCostIncrease() << " ";

                std::cout << step << "\n";

                sanityRecord += aggregated;
        }  // end for

        std::cout << "(1) Due to legalization.\n";
        std::cout << "(2) Due to cost increase.\n";
        std::cout << "\n";
        std::cout << "Move Count (Final)        : " << finalRecord.getMoves()
                  << "\n";
        std::cout << "Move Count (Sanity Check) : " << sanityRecord.getMoves()
                  << "\n";

        std::cout << finalRecord.getMoves() << " = "
                  << (finalRecord.getFails(true) +
                      finalRecord.getSuccesses(true))
                  << "\n";
        std::cout << "---------------------------------------------------------"
                     "-----------------------\n";
        std::cout << std::flush;
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Incremental Placement
////////////////////////////////////////////////////////////////////////////////

bool Infrastructure::findNearestValidWhitespaceToPlaceCell(
    Rsyn::PhysicalCell physicalCell, const DBU targetX, const DBU targetY,
    const DBU maxDisplacementFromTargetPosition,
    const DBU maxDisplacementFromSourcePosition, DBU &resultX, DBU &resultY) {
        struct Filter {
                DBU x;  // initial position
                DBU y;  // initial position
                DBU d;  // maximum allowed displacement
                Filter(const DBU initialX, const DBU initialY,
                       const DBU maxDisplacement)
                    : x(initialX), y(initialY), d(maxDisplacement) {}
                inline bool operator()(
                    const Jezz::WhitespaceDescriptor &whitespace) const {
                        // Compute the distance assuming the node is placed
                        // abutted to the left
                        // edge of the whitespace.
                        const DBU distance =
                            std::abs(whitespace.x_insertion - x) +
                            std::abs(whitespace.y - y);

                        return distance > d;
                }  // end operator
        };         // end struct

        const DBUxy initPos = physicalCell.getInitialPosition();
        const DBU initialX = initPos[X];
        const DBU initialY = initPos[Y];
        const DBU cellWidth = physicalCell.getWidth();

        std::set<Jezz::WhitespaceDescriptor> whitespaces;
        if (isMaxDisplacementConstraintEnabled()) {
                clsJezz->jezz_dp_SearchWhitespaces(
                    targetX, targetY, cellWidth,
                    maxDisplacementFromTargetPosition, 1, whitespaces,
                    Filter(initialX, initialY,
                           maxDisplacementFromSourcePosition));
        } else {
                clsJezz->jezz_dp_SearchWhitespaces(
                    targetX, targetY, cellWidth,
                    maxDisplacementFromTargetPosition, 1, whitespaces);
        }  // end else

        if (!whitespaces.empty()) {
                const Jezz::WhitespaceDescriptor &ws = *whitespaces.begin();
                resultX = ws.x_insertion;
                resultY = ws.y;
                return true;
        } else {
                return false;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::findNearestValidWhitespaceToPlaceCellAroundBlock(
    Rsyn::PhysicalCell physicalCell, const DBU targetX, const DBU targetY,
    const DBU maxDisplacementFromTargetPosition,
    const DBU maxDisplacementFromSourcePosition, DBU &resultX, DBU &resultY) {
        clsBlockageControl = clsSession.getService("ufrgs.blockageControl",
                                                   Rsyn::SERVICE_OPTIONAL);

        if (!clsBlockageControl) return false;

        Rsyn::Cell obstacle =
            clsBlockageControl->findBlockOverlappingCell(physicalCell);

        if (!obstacle) return false;

        std::vector<std::tuple<DBU, DBU, DBU>> borders;

        const Bounds bounds =
            clsPhysicalDesign.getPhysicalCell(obstacle).getBounds();
        borders.push_back(make_tuple(targetX - bounds.getCoordinate(LOWER, X),
                                     bounds.getCoordinate(LOWER, X), targetY));
        borders.push_back(make_tuple(bounds.getCoordinate(UPPER, X) - targetX,
                                     bounds.getCoordinate(UPPER, X), targetY));
        borders.push_back(make_tuple(targetY - bounds.getCoordinate(LOWER, Y),
                                     targetX, bounds.getCoordinate(LOWER, Y)));
        borders.push_back(make_tuple(bounds.getCoordinate(UPPER, Y) - targetY,
                                     targetX, bounds.getCoordinate(UPPER, Y)));

        std::sort(borders.begin(), borders.end());

        for (int i = 0; i < 4; i++) {
                DBU boundedx, boundedy;

                if (isMaxDisplacementConstraintEnabled()) {
                        computeBoundedPosition(
                            physicalCell, std::get<1>(borders[i]),
                            std::get<2>(borders[i]), boundedx, boundedy);
                } else {
                        boundedy = std::get<1>(borders[i]);
                        boundedy = std::get<2>(borders[i]);
                }  // end if

                if (findNearestValidWhitespaceToPlaceCell(
                        physicalCell, boundedx, boundedy,
                        maxDisplacementFromTargetPosition,
                        maxDisplacementFromSourcePosition, resultX, resultY)) {
                        return true;
                }  // end if
        }          // end if

        return false;
}  // end method;

// -----------------------------------------------------------------------------

void Infrastructure::computeBoundedPosition(
    const Rsyn::PhysicalCell physicalCell, const DBU x, const DBU y,
    DBU &boundedx, DBU &boundedy) const {
        // Defensive programming. To avoid any max displacement violation due
        // to rounding errors, let's reduce the max displacement by the site
        // width.
        const DBU maxDisplacement = getMaxDisplacement();
        const DBUxy initCellPos = physicalCell.getInitialPosition();
        const DBU ox = initCellPos[X];
        const DBU oy = initCellPos[Y];

        const DBU dx = x - ox;
        const DBU dy = y - oy;
        const DBU d = std::abs(dx) + std::abs(dy);

        const FloatingPointDBU ratio = maxDisplacement / FloatingPointDBU(d);
        if (ratio < 1) {
                boundedx = (DBU)(ox + ratio * dx);
                boundedy = (DBU)(oy + ratio * dy);
        } else {
                boundedx = x;
                boundedy = y;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::moveCell(Rsyn::PhysicalCell physicalCell, const DBU x,
                              const DBU y,
                              const LegalizationMethod legalization,
                              const bool rollback) {
        clsMoves.count++;

        Jezz::JezzNode *jezzNode =
            clsJezz->getJezzNode(physicalCell.getInstance());

        if (clsFixedInInputFile[physicalCell.getInstance()]) {
                if (clsEnableWarnings) {
                        std::cout << "\n[BUG] Moving a cell which is fixed in "
                                     "the input file. Skipping...\n";
                }  // end if
                clsMoves.fail_fixed++;
                return false;
        }  // end if

        // These multipliers used to expand iteratively the whitespace search
        // window. They are used only because the Jezz's whitespace search
        // method
        // still needs refinements and becomes slow when large search windows
        // are
        // used.
        const static int K = 5;
        const static int MULTIPLIERS[K] = {5, 10, 25, 50, 100};

        const bool previousLegalized =
            clsJezz->isInitialized() ? clsJezz->jezz_dp_IsLegalized(jezzNode)
                                     : false;
        const DBU preivousX = physicalCell.getCoordinate(LOWER, X);
        const DBU preivousY = physicalCell.getCoordinate(LOWER, Y);

        // Get actual target x, y that do not violate max displacement rule.
        DBU boundedx;
        DBU boundedy;
        if (isMaxDisplacementConstraintEnabled()) {
                computeBoundedPosition(physicalCell, x, y, boundedx, boundedy);
        } else {
                boundedx = x;
                boundedy = y;
        }  // end else

// Debug
#if DEBUG
        Rsyn::PhysicalModule phModule =
            clsPhysicalDesign.getPhysicalModule(clsModule);
        const Bounds &coreBounds = phModule.getBounds();
        if (clsEnableWarnings && !coreBounds.inside(boundedx, boundedy)) {
                std::cout << "\n[WARNING] Trying to move cell ["
                          << jezzNode->reference.getName()
                          << "]outside the core bounds: " << boundedx << " "
                          << boundedy << "\n";
                std::cout << "Core Bounds: " << coreBounds << "\n";
        }  // end if
#endif

        // Update cell position to the target location.
        clsPhysicalDesign.placeCell(physicalCell, boundedx, boundedy,
                                    physicalCell.getOrientation(), true);

        if (clsJezz->isInitialized()) {
                // Un-legalize.
                clsJezz->jezz_dp_RemoveNode(jezzNode);

                // Update the reference position to be used during legalization.
                clsJezz->jezz_dp_UpdateReferencePosition(jezzNode, boundedx,
                                                         boundedy);
        }  // end if

        // Re-legalize if requested and possible.
        bool failed = true;

        switch (legalization) {
                case LEG_NONE:
                        failed = false;
                        break;

                case LEG_EXACT_LOCATION:
                        clsJezz->jezz_dp_InsertNode(jezzNode, boundedx,
                                                    boundedy, failed);
                        if (failed) clsMoves.fail_legalization++;
                        break;

                case LEG_MIN_LINEAR_DISPLACEMENT:
                        // [NOTE] This method uses the reference position which
                        // was updated
                        // above.
                        failed = !clsJezz->jezz_LegalizeNode(jezzNode);
                        if (failed) clsMoves.fail_legalization++;
                        break;

                case LEG_NEAREST_WHITESPACE:
                        DBU resultX;
                        DBU resultY;

                        for (int k = 0; k < K; k++) {
                                const int multiplier = MULTIPLIERS[k];

                                failed = !findNearestValidWhitespaceToPlaceCell(
                                    physicalCell, boundedx, boundedy,
                                    physicalCell.getSize(X) * multiplier,
                                    getMaxDisplacement(), resultX, resultY);

                                if (!failed) {
                                        break;
                                }  // end if
                        }          // end for

                        if (failed) {
                                failed =
                                    !findNearestValidWhitespaceToPlaceCellAroundBlock(
                                        physicalCell, boundedx, boundedy,
                                        physicalCell.getSize(X) *
                                            MULTIPLIERS[K - 1],
                                        getMaxDisplacement(), resultX, resultY);
                        }  // end if

                        if (!failed) {
                                clsJezz->jezz_dp_InsertNode(jezzNode, resultX,
                                                            resultY, failed);

                                if (!failed) {
                                        failed = violateMaxDisplacement(
                                            jezzNode->reference);
                                        if (failed) {
                                                clsMoves.fail_displacement++;
                                        }  // end if
                                } else {
                                        clsMoves.fail_legalization++;
                                }  // end else
                        } else {
                                clsMoves.fail_legalization++;
                        }  // end else

#if DEBUG
                        if (failed && jezzNode &&
                            clsJezz->jezz_dp_IsLegalized(jezzNode)) {
                                std::cout << "\n[DEBUG] Max displacement "
                                             "violation on cell "
                                          << jezzNode->reference.getName()
                                          << "\n";

                                const DBU legal_x =
                                    clsJezz->jezz_dp_GetX(jezzNode);
                                const DBU legal_y =
                                    clsJezz->jezz_dp_GetY(jezzNode);

                                std::cout << "\t   target_x=" << x << "\t"
                                          << "   target_y=" << y << "\n"
                                          << "\t  bounded_x=" << boundedx
                                          << "\t"
                                          << "  bounded_y=" << boundedy << "\n"
                                          << "\tinsertion_x=" << resultX << "\t"
                                          << "insertion_y=" << resultY << "\n"
                                          << "\t    legal_x=" << legal_x << "\t"
                                          << "    legal_y=" << legal_y << "\n";
                        }  // end if
#endif

                        break;
        }  // end switch

        if (failed && rollback) {
                // Move cell back to its previous position.
                clsPhysicalDesign.placeCell(physicalCell, preivousX, preivousY,
                                            physicalCell.getOrientation(),
                                            true);

                // Update the reference position to be used during legalization.
                clsJezz->jezz_dp_UpdateReferencePosition(jezzNode, preivousX,
                                                         preivousY);

                // Re-insert cell in its previous position.
                if (previousLegalized) {
                        bool overflow;
                        clsJezz->jezz_dp_InsertNode(jezzNode, preivousX,
                                                    preivousY, overflow);
                        if (overflow) {
                                std::cout << "\n[BUG] Unable to re-insert cell "
                                             "back in its previous position.\n";
                                std::exit(1);
                        }  // end if
                }          // end if
        } else {
                // Mark this cell as dirty (don't forget to add this also in the
                // legalization callback, as the legalizer is one of the few
                // things that
                // change cell positions without using moveCell).

                // Only mark this cell as dirty if it actually moved. We noted
                // that many
                // times the cell end up in the exactly same position. Don't
                // worry is
                // safe to use == (!=) operators to compare floating points in
                // this
                // case.
                if (preivousX != physicalCell.getCoordinate(LOWER, X) ||
                    preivousY != physicalCell.getCoordinate(LOWER, Y)) {
                        // Note it's only necessary to "dirty" cells by hand
                        // because we
                        // explicitly asked "placeCell" to not notify observer.
                        // Don't notify Jezz as it was already updated.
                        clsPhysicalDesign.notifyInstancePlaced(
                            physicalCell.getInstance(), clsJezz);
                }  // end if
        }          // end else

        return !failed;
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::moveCell(Rsyn::Cell cell, const DBU x, const DBU y,
                              const LegalizationMethod legalization,
                              const bool rollback) {
        return moveCell(clsPhysicalDesign.getPhysicalCell(cell), x, y,
                        legalization, rollback);
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::moveCell(Rsyn::PhysicalCell physicalCell, const DBUxy pos,
                              const LegalizationMethod legalization,
                              const bool rollback) {
        return moveCell(physicalCell, pos.x, pos.y, legalization, rollback);
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::moveCell(Rsyn::Cell cell, const DBUxy pos,
                              const LegalizationMethod legalization,
                              const bool rollback) {
        return moveCell(clsPhysicalDesign.getPhysicalCell(cell), pos.x, pos.y,
                        legalization, rollback);
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::translateCell(Rsyn::PhysicalCell physicalCell,
                                   const DBU dx, const DBU dy,
                                   const LegalizationMethod legalization,
                                   const bool rollback) {
        const DBU x = physicalCell.getCoordinate(LOWER, X);
        const DBU y = physicalCell.getCoordinate(LOWER, Y);
        return moveCell(physicalCell, x + dx, y + dy, legalization, rollback);
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::translateCell(Rsyn::Cell cell, const DBU dx, const DBU dy,
                                   const LegalizationMethod legalization,
                                   const bool rollback) {
        return translateCell(clsPhysicalDesign.getPhysicalCell(cell), dx, dy,
                             legalization, rollback);
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::translateCell(Rsyn::PhysicalCell physicalCell,
                                   const DBUxy displacement,
                                   const LegalizationMethod legalization,
                                   const bool rollback) {
        return translateCell(physicalCell, displacement.x, displacement.y,
                             legalization, rollback);
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::translateCell(Rsyn::Cell cell, const DBUxy displacement,
                                   const LegalizationMethod legalization,
                                   const bool rollback) {
        return translateCell(cell, displacement.x, displacement.y, legalization,
                             rollback);
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Move Towards
////////////////////////////////////////////////////////////////////////////////

void Infrastructure::moveCellTowards(Rsyn::PhysicalCell physicalCell,
                                     const DBUxy targetPos,
                                     const LegalizationMethod legalization,
                                     const double stepSize) {
        const DBUxy sourcePos = physicalCell.getCoordinate(LOWER);

        bool success = false;

        if (stepSize > 0 && stepSize <= 1) {
                for (double t = 1; t > 0; t -= stepSize) {
                        const DBUxy pos = DBUxy(
                            (DBU)std::round(sourcePos.x +
                                            t * (targetPos.x - sourcePos.x)),
                            (DBU)std::round(sourcePos.y +
                                            t * (targetPos.y - sourcePos.y)));
                        if (success = moveCell(physicalCell, pos, legalization,
                                               false))
                                break;
                }  // end for
        } else {
                std::cout << "[WARNING] Invalid step size.\n";
        }  // end else

        if (!success) {
                // If not able to find a position to  the cell, move it back to
                // its
                // original position.
                if (!moveCell(physicalCell, sourcePos, LEG_NEAREST_WHITESPACE,
                              true)) {
                        std::cout << "[BUG] Unable to move cell back to its "
                                     "original position.\n";
                }  // end if
        }          // end if
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::moveCellTowards(Rsyn::Cell cell, const DBUxy targetPos,
                                     const LegalizationMethod legalization,
                                     const double stepSize) {
        moveCellTowards(clsPhysicalDesign.getPhysicalCell(cell), targetPos,
                        legalization, stepSize);
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::moveCellTowards(Rsyn::Cell cell, const DBU x, const DBU y,
                                     const LegalizationMethod legalization,
                                     const double stepSize) {
        moveCellTowards(clsPhysicalDesign.getPhysicalCell(cell), DBUxy(x, y),
                        legalization, stepSize);
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::moveCellTowards(Rsyn::PhysicalCell physicalCell,
                                     const DBU x, const DBU y,
                                     const LegalizationMethod legalization,
                                     const double stepSize) {
        moveCellTowards(physicalCell, DBUxy(x, y), legalization, stepSize);
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Cost Aware Cell Movement
////////////////////////////////////////////////////////////////////////////////

bool Infrastructure::moveCellWithCostEvaluationCached(
    Rsyn::Cell cell, const DBU x, const DBU y,
    const LegalizationMethod legalization, const CostFunction costFunction,
    const Rsyn::TimingMode mode, const double oldCost, double &newCost) {
        Rsyn::PhysicalCell phCell = clsPhysicalDesign.getPhysicalCell(cell);
        const DBUxy cellPos = phCell.getPosition();
        const DBU oldx = cellPos[X];
        const DBU oldy = cellPos[Y];

        // Try to move the cell.
        const bool success = moveCell(cell, x, y, legalization);

        if (success) {
                computeCost_UpdateAfterMove(cell, costFunction);
                newCost = computeCost(cell, mode, costFunction);

                // Don't move if the cost has increased for late mode or
                // decreased for
                // early mode.

                if ((mode == Rsyn::LATE && newCost > oldCost) ||
                    (mode == Rsyn::EARLY && newCost < oldCost)) {
                        // [NOTE] Here we use nearest whitespace legalization
                        // method to
                        // ensure that the cell will be placed exactly at its
                        // old position.
                        // LEG_NEAREST_WHITESPACE IS OK, DONT CHANGE!
                        if (!moveCell(cell, oldx, oldy,
                                      LEG_NEAREST_WHITESPACE)) {
                                std::cout << "[BUG] It was not possible to "
                                             "move back the cell to its old "
                                             "position.\n";
                        }  // end if

                        // Since the cell was moved back, we need to update the
                        // Steiner
                        // trees, timing, etc again.
                        computeCost_UpdateAfterMove(cell, costFunction);

                        // Indicates to the user that the move was rolled back.
                        clsMoves.fail_cost++;
                        return false;
                } else {
                        return true;
                }  // end else
        } else {
                // No much to do here as moveCell() should have already rolled
                // back the
                // move. Just signal that the move failed due to legalization
                // setting
                // the new cost equal to the old one.
                newCost = oldCost;
                return false;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::moveCellWithCostEvaluation(
    Rsyn::Cell cell, const DBU x, const DBU y,
    const LegalizationMethod legalization, const CostFunction costFunction,
    const Rsyn::TimingMode mode, double &oldCost, double &newCost) {
        oldCost = computeCost(cell, mode, costFunction);
        return moveCellWithCostEvaluationCached(
            cell, x, y, legalization, costFunction, mode, oldCost, newCost);
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::translateCellWithCostEvaluationCached(
    Rsyn::Cell cell, const DBU dx, const DBU dy,
    const LegalizationMethod legalization, const CostFunction costFunction,
    const Rsyn::TimingMode mode, const double oldCost, double &newCost) {
        Rsyn::PhysicalCell phCell = clsPhysicalDesign.getPhysicalCell(cell);
        const DBUxy cellPos = phCell.getPosition();
        const DBU x = cellPos[X];
        const DBU y = cellPos[Y];
        return moveCellWithCostEvaluationCached(cell, x + dx, y + dy,
                                                legalization, costFunction,
                                                mode, oldCost, newCost);
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::translateCellWithCostEvaluation(
    Rsyn::Cell cell, const DBU dx, const DBU dy,
    const LegalizationMethod legalization, const CostFunction costFunction,
    const Rsyn::TimingMode mode, double &oldCost, double &newCost) {
        Rsyn::PhysicalCell phCell = clsPhysicalDesign.getPhysicalCell(cell);
        const DBUxy cellPos = phCell.getPosition();
        const DBU x = cellPos[X];
        const DBU y = cellPos[Y];
        return moveCellWithCostEvaluation(cell, x + dx, y + dy, legalization,
                                          costFunction, mode, oldCost, newCost);
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Cost Evaluation
////////////////////////////////////////////////////////////////////////////////

double Infrastructure::computeCost_Wirelength(Rsyn::Cell cell,
                                              const Rsyn::TimingMode mode) {
        double cost = 0;

        for (Rsyn::Pin pin : cell.allPins()) {
                Rsyn::Net net = pin.getNet();
                if (net) {
                        const Number criticality =
                            clsTimer->getNetCriticality(net, mode);
                        cost += criticality *
                                clsRoutingEstimator->getNetWirelength(net);
                }  // end if
        }          // end for

        return cost;
}  // end method

// -----------------------------------------------------------------------------

double Infrastructure::computeCost_RC(Rsyn::Cell cell,
                                      const Rsyn::TimingMode mode) {
        double cost = 0;

        for (Rsyn::Pin pin : cell.allPins()) {
                Rsyn::Net net = pin.getNet();
                if (net) {
                        Rsyn::Pin driver = net.getAnyDriver();
                        if (driver) {
                                const double rDriver =
                                    clsLibraryCharacterizer
                                        ->getCellMaxDriverResistance(
                                            driver.getInstance(), mode);
                                const Number criticality =
                                    clsTimer->getNetCriticality(net, mode);

                                cost += criticality *
                                        (rDriver *
                                         clsTimer->getNetLoad(net).getMax());
                        }  // end if
                }          // end if
        }                  // end for

        return cost;
}  // end method

// -----------------------------------------------------------------------------

double Infrastructure::computeCost_LocalDelay(Rsyn::Cell cell,
                                              const Rsyn::TimingMode mode) {
        double cost = 0;

        // [NOTE] After a local timing update, criticalities may change. I think
        // the most appropriate approach would be to store the current
        // criticalities and keep them as constants.

        // Get the wire delay at all local sink pins (side cells and sink
        // cells).
        for (Rsyn::Pin pin : cell.allPins()) {
                Rsyn::Net net = pin.getNet();
                if (net) {
                        for (Rsyn::Pin sink : net.allPins(Rsyn::SINK)) {
                                if (sink.getInstance() == cell) continue;

                                cost += getPinImportance(sink, mode) *
                                        std::max(clsTimer->getPinArrivalTime(
                                                     sink, mode, Rsyn::FALL),
                                                 clsTimer->getPinArrivalTime(
                                                     sink, mode, Rsyn::RISE));
                        }  // end for
                }          // end if
        }                  // end for

        return cost;
}  // end method

// -----------------------------------------------------------------------------

double Infrastructure::computeCost(Rsyn::Cell cell, const Rsyn::TimingMode mode,
                                   const CostFunction costFunction) {
        switch (costFunction) {
                case COST_NONE:
                        return 0;
                case COST_WIRELENGTH:
                        return computeCost_Wirelength(cell, mode);
                case COST_RC:
                        return computeCost_RC(cell, mode);
                case COST_LOCAL_DELAY:
                        return computeCost_LocalDelay(cell, mode);
                default:
                        assert(false);
                        return 0;
        }  // end switch
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::computeCost_UpdateAfterMove(
    Rsyn::Cell cell, const CostFunction costFunction) {
        switch (costFunction) {
                case COST_NONE:
                        break;
                case COST_WIRELENGTH:
                        clsRoutingEstimator->updateRouting();
                        break;
                case COST_RC:
                        clsRoutingEstimator->updateRouting();
                        break;
                case COST_LOCAL_DELAY:
                        clsRoutingEstimator->updateRouting();
                        clsTimer->updateTimingLocally(cell);
                        break;
                default:
                        assert(false);
        }  // end switch
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Legalization
////////////////////////////////////////////////////////////////////////////////

void Infrastructure::legalize() { clsJezz->jezz_Legalize(); }  // end method
// -----------------------------------------------------------------------------

bool Infrastructure::legalizeCell(Rsyn::PhysicalCell physicalCell,
                                  const LegalizationMethod legalization,
                                  const bool rollback) {
        Jezz::JezzNode *jezzNode =
            clsJezz->getJezzNode(physicalCell.getInstance());
        if (!clsJezz->jezz_dp_IsLegalized(jezzNode)) {
                return moveCell(physicalCell,
                                physicalCell.getCoordinate(LOWER, X),
                                physicalCell.getCoordinate(LOWER, Y),
                                legalization, rollback);
        }  // end if
        return true;
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::legalizeCell(Rsyn::Cell cell,
                                  const LegalizationMethod legalization,
                                  const bool rollback) {
        return legalizeCell(clsPhysicalDesign.getPhysicalCell(cell),
                            legalization, rollback);
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::unlegalizeCell(Rsyn::PhysicalCell physicalCell) {
        Jezz::JezzNode *jezzNode =
            clsJezz->getJezzNode(physicalCell.getInstance());
        clsJezz->jezz_dp_RemoveNode(jezzNode);
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::unlegalizeCell(Rsyn::Cell cell) {
        unlegalizeCell(clsPhysicalDesign.getPhysicalCell(cell));
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::isLegalized(Rsyn::Cell cell) {
        Rsyn::PhysicalCell ph = clsPhysicalDesign.getPhysicalCell(cell);
        Jezz::JezzNode *jezzNode = clsJezz->getJezzNode(ph.getInstance());
        return jezzNode ? clsJezz->jezz_dp_IsLegalized(jezzNode) : true;
}  // end method

// -----------------------------------------------------------------------------

double Infrastructure::getAvailableFreespace(Rsyn::Cell cell) {
        Rsyn::PhysicalCell ph = clsPhysicalDesign.getPhysicalCell(cell);
        Jezz::JezzNode *jezzNode = clsJezz->getJezzNode(ph.getInstance());
        return jezzNode ? clsJezz->jezz_GetAvailableFreespace(jezzNode) : 0;
}  // end method

// -----------------------------------------------------------------------------

double Infrastructure::getAvailableFreespaceRight(Rsyn::Cell cell) {
        Rsyn::PhysicalCell ph = clsPhysicalDesign.getPhysicalCell(cell);
        Jezz::JezzNode *jezzNode = clsJezz->getJezzNode(ph.getInstance());
        return jezzNode ? clsJezz->jezz_GetAvailableFreespaceRight(jezzNode)
                        : 0;
}  // end method

// -----------------------------------------------------------------------------

double Infrastructure::getAvailableFreespaceLeft(Rsyn::Cell cell) {
        Rsyn::PhysicalCell ph = clsPhysicalDesign.getPhysicalCell(cell);
        Jezz::JezzNode *jezzNode = clsJezz->getJezzNode(ph.getInstance());
        return jezzNode ? clsJezz->jezz_GetAvailableFreespaceLeft(jezzNode) : 0;
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::diffJezzSolutions(std::ostream &out, const int numCells,
                                       const std::string solution0,
                                       const std::string solution1,
                                       const bool enableCriticality) {
        std::vector<std::tuple<Rsyn::Cell, double, double>> diff;
        clsJezz->jezz_diffSolutions(solution0, solution1, diff);

        if (enableCriticality) {
                for (int i = 0; i < diff.size(); i++) {
                        std::get<2>(diff[i]) =
                            0.6 * std::get<2>(diff[i]) +
                            0.4 *
                                clsTimer->getCellCriticality(
                                    std::get<0>(diff[i]), Rsyn::LATE);
                }  // end for
        }

        std::sort(diff.begin(), diff.end(),
                  [](const std::tuple<Rsyn::Cell, double, double> &a,
                     const std::tuple<Rsyn::Cell, double, double> &b) -> bool {
                          return std::get<2>(a) > std::get<2>(b);
                  }  // end lambda
                  );

        StreamStateSaver sss(std::cout);

        out << std::setprecision(2) << std::fixed;
        out << std::right;

        out << std::setw(5) << "Rank"
            << " ";
        out << std::setw(12) << "Displacement"
            << " ";
        out << std::setw(12) << "Relativity"
            << " ";
        out << "Cell"
            << "\n";

        const register int numPrints = std::min(numCells, (int)diff.size());
        for (int i = 0; i < numPrints; i++) {
                Rsyn::Cell cell = std::get<0>(diff[i]);

                out << std::setw(5) << std::get<2>(diff[i]) << " ";
                out << std::setw(12) << std::get<1>(diff[i]) << " ";
                out << std::setw(12)
                    << clsTimer->getCellRelativity(cell, Rsyn::LATE);
                out << cell.getName();
        }  // end for
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Analysis and Statistics
////////////////////////////////////////////////////////////////////////////////

void Infrastructure::renderCriticalNetHistogram(const std::string &title) {
        const int numBins = 10;

        std::vector<int> histogramCriticalNets(numBins, 0);
        std::vector<int> histogramCriticalNetsWithNonCriticalSinks(numBins, 0);

        int counterCriticalNetsWithNonCriticalSinks = 0;
        int counterCriticalNets = 0;

        for (Rsyn::Net net : clsModule.allNets()) {
                Rsyn::Pin driver = net.getAnyDriver();

                if (!driver ||
                    clsTimer->getPinWorstSlack(driver, Rsyn::LATE) >= 0)
                        continue;

                counterCriticalNets++;
                const double criticality =
                    clsTimer->getPinCriticality(driver, Rsyn::LATE);
                const int bin = (int)(criticality * (numBins - 1));

                histogramCriticalNets[bin]++;

                for (Rsyn::Pin sink : net.allPins(Rsyn::SINK)) {
                        Rsyn::Cell cell = sink.getInstance().asCell();

                        bool isCritical = false;
                        for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
                                if (clsTimer->getPinWorstSlack(
                                        pin, Rsyn::LATE) < 0) {
                                        isCritical = true;
                                        break;
                                }  // end if
                        }          // end if

                        if (!isCritical) {
                                counterCriticalNetsWithNonCriticalSinks++;
                                histogramCriticalNetsWithNonCriticalSinks
                                    [bin]++;
                                break;
                        }  // end if
                }          // end for
        }                  // end for

        std::ostringstream oss1;
        oss1 << "Num critical nets: " << counterCriticalNets << " "
             << "("
             << (100 * counterCriticalNets / double(clsDesign.getNumNets()))
             << "%)\n";

        std::ostringstream oss2;
        oss2 << "Num critical nets with non critical sinks: "
             << counterCriticalNetsWithNonCriticalSinks << " "
             << "(" << (100 * counterCriticalNetsWithNonCriticalSinks /
                        double(clsDesign.getNumNets()))
             << "%)\n";

        /*
        for (int i = 0; i < numBins; i++) {
                std::cout << setw(3) << (i*binSize) << " ";
                std::cout << histogramCriticalNets[i] << " ";
                std::cout << histogramCriticalNetsWithNonCriticalSinks[i] <<
        "\n";
        } // end for
        */
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::displacementHistrogram(ostream &out) {
        int numMovableCells = 0;
        int numMovableCellsMoved = 0;

        std::cout << "[INFO] Computing Displacement Histogram (Only for cells "
                     "that actually moved.)\n";
        const int numbCells = clsPhysicalDesign.getNumMovedCells();
        const int numbSample = (int)std::ceil(std::sqrt(numbCells));
        const Number range = getMaxDisplacement() / numbSample;
        std::vector<int> histogram;
        histogram.resize(numbSample + 1, 0);

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                Rsyn::PhysicalCell phCell =
                    clsPhysicalDesign.getPhysicalCell(cell);
                if (instance.isFixed() || instance.isMacroBlock() ||
                    instance.isPort())
                        continue;

                const DBUxy initialPos = phCell.getInitialPosition();
                const DBUxy currentPos = phCell.getPosition();
                if (initialPos[X] != currentPos[X] ||
                    initialPos[Y] != initialPos[Y]) {
                        Number displace =
                            std::abs(initialPos[X] - currentPos[X]) +
                            std::abs(initialPos[Y] - currentPos[Y]);
                        int pos = (int)(displace / range);
                        histogram[pos]++;
                        numMovableCellsMoved++;
                }  // end if
                numMovableCells++;

        }  // end for

        std::cout << "#Movable cells: " << numMovableCells << "\n";
        std::cout << "#Movable cells that moved: " << numMovableCellsMoved
                  << "\n";

        out << "dist.\t#cells\n";
        out << std::fixed << std::setprecision(2);
        for (int i = 0; i < numbSample; i++)
                out << (i * range /
                        clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU))
                    << "\t" << histogram[i] << "\n";
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Checkings
////////////////////////////////////////////////////////////////////////////////

bool Infrastructure::violateMaxDisplacement(Rsyn::PhysicalCell ph) {
        if (clsEnableMaxDisplacementConstraint) {
                if (clsFixedInInputFile[ph.getInstance()] || ph.isMacroBlock())
                        return false;

                const double displacement = computeManhattanDistance(
                    ph.getPosition(), ph.getInitialPosition());
                return displacement > getMaxDisplacement();
        } else {
                return false;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::violateMaxDisplacement(Rsyn::Cell cell) {
        return !cell.isPort() &&
               violateMaxDisplacement(clsPhysicalDesign.getPhysicalCell(cell));
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::checkMaxDisplacementViolation(
    const std::string &errorMsg, const bool hideMessageIfPass) {
        if (!clsEnableMaxDisplacementConstraint) return true;

        const int maxViolatorsToReport = 10;

        vector<Rsyn::Cell> violators;
        violators.reserve(maxViolatorsToReport);

        bool moreViolators = false;
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (violateMaxDisplacement(cell)) {
                        if (violators.size() >= maxViolatorsToReport) {
                                moreViolators = true;
                                break;
                        } else {
                                violators.push_back(cell);
                        }  // end if
                }          // end if
        }                  // end for

        const bool pass = violators.empty();
        if (!pass || !hideMessageIfPass) {
                std::cout << "[" << (violators.size() > 0 ? "FAIL" : "PASS")
                          << "] ";
                std::cout << "Max displacement violation checking"
                          << (errorMsg == "" ? "." : (" @ " + errorMsg))
                          << "\n";

                for (unsigned i = 0; i < violators.size(); i++) {
                        Rsyn::Cell cell = violators[i];
                        Rsyn::PhysicalCell phCell =
                            clsPhysicalDesign.getPhysicalCell(cell);
                        DBUxy posCell = phCell.getPosition();
                        DBUxy initial = phCell.getInitialPosition();
                        const double ox = initial[X];
                        const double oy = initial[Y];
                        const double x = posCell[X];
                        const double y = posCell[Y];

                        const double displacement =
                            computeManhattanDistance(ox, oy, x, y);

                        std::cout << "\t";
                        std::cout << cell.getName() << " "
                                  << "ox=" << ox << " "
                                  << "oy=" << oy << " "
                                  << "x=" << x << " "
                                  << "y=" << y << " "
                                  << "displacement=" << displacement << " "
                                  << "(" << getMaxDisplacement() << ")\n";
                }  // end for

                if (moreViolators) {
                        std::cout << "\t";
                        std::cout
                            << "At least one more violator... Stopping here.\n";
                }  // end if
        }          // end if

        return violators.size() == 0;
}  // end method

// -----------------------------------------------------------------------------

bool Infrastructure::checkUnlegalizedCells(const std::string &errorMsg,
                                           const bool hideMessageIfPass) {
        const int maxViolatorsToReport = 10;

        vector<Rsyn::Cell> violators;
        violators.reserve(maxViolatorsToReport);

        bool moreViolators = false;
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                Jezz::JezzNode *jezzNode = clsJezz->getJezzNode(cell);
                if (jezzNode && !clsJezz->jezz_dp_IsLegalized(jezzNode)) {
                        if (violators.size() >= maxViolatorsToReport) {
                                moreViolators = true;
                                break;
                        } else {
                                violators.push_back(jezzNode->reference);
                        }  // end if
                }          // end if
        }                  // end for

        const bool pass = violators.empty();
        if (!pass || !hideMessageIfPass) {
                std::cout << "[" << (violators.size() > 0 ? "FAIL" : "PASS")
                          << "] ";
                std::cout << "Legalization violation checking"
                          << (errorMsg == "" ? "." : (" @ " + errorMsg))
                          << "\n";

                for (unsigned i = 0; i < violators.size(); i++) {
                        Rsyn::Cell cell = violators[i];
                        Rsyn::PhysicalCell phCell =
                            clsPhysicalDesign.getPhysicalCell(cell);
                        const DBUxy cellPos = phCell.getPosition();
                        const double x = cellPos[X];
                        const double y = cellPos[Y];

                        std::cout << "\t";
                        std::cout << cell.getName() << " "
                                  << "x=" << x << " "
                                  << "y=" << y << "\n";
                }  // end for

                if (moreViolators) {
                        std::cout << "\t";
                        std::cout
                            << "At least one more violator... Stopping here.\n";
                }  // end if
        }          // end if

        return violators.size() == 0;
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Optimization
////////////////////////////////////////////////////////////////////////////////

void Infrastructure::run(const std::string &flow) {
        const Stopwatch &stopwatchUpdateSteinerTrees =
            clsRoutingEstimator->getUpdateSteinerTreeRuntime();
        const Stopwatch &stopwatchUpdateTiming =
            clsTimer->getUpdateTimingRuntime();
        clsTimer->resetRuntime();
        clsRoutingEstimator->resetRuntime();
        stateClear();
        statePush("initial");

        Stopwatch stopwatchFlow;
        stopwatchFlow.start();
        if (flow == "ispd16") {
                clsSession.runProcess("ufrgs.ISPD16Flow");
        } else if (flow == "jucemar") {
                clsSession.runProcess("gp.simpl");
        } else if (flow == "qp") {
                clsSession.runProcess("ufrgs.TDQuadraticFlow");
        } else if (flow == "default" || flow == "iccad15" || flow == "") {
                clsSession.runProcess("ufrgs.ISPD16Flow");
        } else {
                clsSession.runProcess(flow, {});
        }  // end else
        stopwatchFlow.stop();

        // Report QoR breakdown
        statePush("final");
        stateReportQuality();
        stateReportMoves();

        // Report final results.
        reportFinalResult();

        // Report runtime breakdown.
        StreamStateSaver sss(std::cout);
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Runtime Breakdown (s)\n";
        std::cout << "Total               : " << stopwatchFlow.getElapsedTime()
                  << "\n";
        std::cout << "Steiner Tree Update : "
                  << stopwatchUpdateSteinerTrees.getElapsedTime() << " ("
                  << (100 * stopwatchUpdateSteinerTrees.getElapsedTime() /
                      stopwatchFlow.getElapsedTime())
                  << "%)\n";
        std::cout << "Timing Update       : "
                  << stopwatchUpdateTiming.getElapsedTime() << " ("
                  << (100 * stopwatchUpdateTiming.getElapsedTime() /
                      stopwatchFlow.getElapsedTime())
                  << "%)\n";
        std::cout << std::endl;
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::runFluteLoopWorkaround() {
        // Although Flute does not return cycles (loops) in the Steiners it
        // generates, loops may be generated when nodes at same (x, y) position
        // are
        // merged. Flute typically generates several Steiner points at same
        // location
        // for large nets and depending on the underlining tree topology the
        // merge
        // will cause a loop.

        // This workaround just tries to move some cells a little bit so that a
        // new tree is generated, hopefully without loops.

        Stepwatch watch("Checking and fixing loops");

        for (int pass = 0; pass < 3; pass++) {
                bool keepGoing = false;

                for (Rsyn::Net net : clsModule.allNets()) {
                        const Rsyn::RCTree &tree =
                            clsRoutingEstimator->getRCTree(net);
                        if (tree.getLoopDetectedAndRemovedFlag()) {
                                keepGoing = true;

                                std::vector<std::tuple<double, Rsyn::Cell>>
                                    cells;

                                // It may happen that the number of cells is
                                // lower than the
                                // number of pins for instance when a net
                                // connect a same cell
                                // at different pins.
                                cells.reserve(net.getNumPins());

                                for (Rsyn::Pin pin : net.allPins()) {
                                        Rsyn::Cell cell =
                                            pin.getInstance().asCell();
                                        const double slack =
                                            clsTimer->getCellWorstSlack(
                                                cell, Rsyn::LATE);
                                        cells.push_back(
                                            std::make_tuple(-slack, cell));
                                }  // end for
                                std::sort(cells.begin(), cells.end());

                                std::cout << "[INFO] Trying to remove loop in "
                                             "routing tree after node merging "
                                             "for net '"
                                          << net.getName() << "'...\n";
                                for (int i = 0; i < cells.size(); i++) {
                                        std::cout << "Attempt " << i << ": ";

                                        Rsyn::Cell cell = std::get<1>(cells[i]);
                                        Rsyn::PhysicalCell phCell =
                                            clsPhysicalDesign.getPhysicalCell(
                                                cell);
                                        const DBUxy p = phCell.getPosition();
                                        const DBUxy d = phCell.getSize();

                                        moveCell(cell, p + (i + 1) * d,
                                                 LEG_NEAREST_WHITESPACE);
                                        clsRoutingEstimator->updateRouting();

                                        if (tree.getLoopDetectedAndRemovedFlag()) {
                                                std::cout << "Nope\n";
                                        } else {
                                                std::cout << "Done\n";
                                                break;
                                        }  // end else
                                }          // end for
                        }                  // end if
                }                          // end for

                if (!keepGoing) break;
        }  // end for

        for (Rsyn::Net net : clsModule.allNets()) {
                const Rsyn::RCTree &tree = clsRoutingEstimator->getRCTree(net);
                if (tree.getLoopDetectedAndRemovedFlag()) {
                        std::cout << "[WARNING] This solution still contains "
                                     "loops.\n";
                        break;
                }  // end if
        }          // end if
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Misc
////////////////////////////////////////////////////////////////////////////////

double Infrastructure::computePathManhattanLength(
    const std::vector<Rsyn::Timer::PathHop> &path) {
        double length = 0.0;

        const int numHops = path.size();

        for (int i = 0; i < numHops - 1; i++) {
                const Rsyn::Timer::PathHop &hop = path[i];

                const Rsyn::Pin currPin = hop.getPin();
                const Rsyn::Pin nextPin = hop.getNextPin();
                const Rsyn::Cell currCell = currPin.getInstance().asCell();
                const Rsyn::Cell nextCell = nextPin.getInstance().asCell();
                if (currCell == nextCell) continue;
                Rsyn::PhysicalCell phCurrCell =
                    clsPhysicalDesign.getPhysicalCell(currCell);
                if (currPin.getNet() == clsTimer->getClockNet() ||
                    currCell.isLCB())
                        continue;

                const DBUxy currPos = clsPhysicalDesign.getPinPosition(currPin);
                const DBUxy nextPos = clsPhysicalDesign.getPinPosition(nextPin);

                length += computeManhattanDistance(currPos, nextPos);
        }  // end for

        return length;
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportPathManhattanLengthBySegment(
    const std::vector<Rsyn::Timer::PathHop> &path, ostream &ostream) {
        std::cout << "***** Reporting Path "
                  << path.front().getPin().getFullName();
        std::cout << "->" << path.back().getPin().getFullName() << " *****\n";
        for (int i = 0; i < path.size() - 1; i++) {
                Rsyn::Timer::PathHop hop = path[i];
                std::cout << hop.getPin().getFullName() << "->";
                std::cout << hop.getNextPin().getFullName() << "\t";
                std::cout << computeManhattanDistance(hop.getPin(),
                                                      hop.getNextPin());
                std::cout << "\n";
        }  //
}

////////////////////////////////////////////////////////////////////////////////
// Reports
////////////////////////////////////////////////////////////////////////////////

void Infrastructure::reportDigest() {
        updateQualityScore();

        const double abu = getAbu();
        const double alpha = clsABU.getAlpha();

        StreamStateSaver sss(cout);

        const float hpwlInUm =
            clsPhysicalDesign.getHPWL().aggregated() /
            (float)clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        const float steinierWlInUm =
            (clsRoutingEstimator->getTotalWirelength() /
             (float)clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU));
        const double scaledSteinierWlInUm = steinierWlInUm * (1 + alpha * abu);

        cout << "Report Digest"
             << "\n";
        cout << "--------------------------------------------------------------"
                "------------------\n";
        cout << "Circuit              : " << clsDesign.getName() << "\n";
        cout << std::fixed << setprecision(2);
        cout << "Steiner WL (um)      : " << steinierWlInUm << "\n";
        cout << "Scaled Steiner WL    : " << scaledSteinierWlInUm << " ( "
             << alpha * abu * 100 << "% )\n";
        cout << "HPWL (um)            : " << hpwlInUm << "\n";
        cout << "Clock period (ps)    : " << clsTimer->getClockPeriod() << "\n";
        cout << std::scientific << setprecision(5);
        cout << "Early WNS, TNS (ps)  : " << clsTimer->getWns(Rsyn::EARLY)
             << ", " << clsTimer->getTns(Rsyn::EARLY) << " #Endpoints: "
             << clsTimer->getNumCriticalEndpoints(Rsyn::EARLY) << "\n";
        cout << "Late  WNS, TNS (ps)  : " << clsTimer->getWns(Rsyn::LATE)
             << ", " << clsTimer->getTns(Rsyn::LATE)
             << " #Endpoints: " << clsTimer->getNumCriticalEndpoints(Rsyn::LATE)
             << "\n";

        sss.restore();

        cout << "ABU Penalty          : " << abu << "\n";
        cout << "Quality Score        : " << getQualityScore() << "\n";
        cout << "--------------------------------------------------------------"
                "------------------\n";

}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportSummary(const std::string &step) {
        updateQualityScore();

        StreamStateSaver sss(cout);

        cout << "Summary:";

        cout << std::fixed << std::setprecision(2);
        cout << " QoR=" << getQualityScore();

        cout << std::fixed << std::setprecision(5);
        cout << " ABU=" << getAbu();

        cout << std::scientific << std::setprecision(5);
        cout << " StWL="
             << (clsRoutingEstimator->getTotalWirelength() /
                 (float)clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU));
        cout << " eWNS=" << clsTimer->getWns(Rsyn::EARLY);
        cout << " eTNS=" << clsTimer->getTns(Rsyn::EARLY);
        cout << " lWNS=" << clsTimer->getWns(Rsyn::LATE);
        cout << " lTNS=" << clsTimer->getTns(Rsyn::LATE);
        // cout << " TTNS=" << clsTimer->computeTrueTns(Rsyn::LATE);
        cout << " @" << step << "\n";

        sss.restore();
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportFinalResult() {
        clsTimer->updateTimingIncremental();
        updateQualityScore();

        StreamStateSaver sss(cout);

        const int N = 13;

        cout << std::left;
        cout << "\n";
        cout << "              ";  // reserve space for "Final Result: "
        cout << std::setw(15) << "Design";
        cout << std::setw(5) << "MaxD";
        cout << std::setw(5) << "Util";
        cout << std::setw(N) << "eWNS";
        cout << std::setw(N) << "eTNS";
        cout << std::setw(N) << "lWNS";
        cout << std::setw(N) << "lTNS";
        cout << std::setw(N) << "ABU";
        cout << std::setw(8) << "QoR";
        cout << std::setw(N) << "StWL (um)";
        cout << "\n";

        cout << "Final Result: ";  // make it easy to grep
        cout << std::setw(15) << clsDesign.getName();
        cout << std::fixed << std::setprecision(2);
        cout << std::setw(5) << ((int)clsMaxDisplacement);
        cout << std::setw(5) << clsTargetUtilization;
        cout << std::scientific << std::setprecision(5);
        cout << std::setw(N) << clsTimer->getWns(Rsyn::EARLY);
        cout << std::setw(N) << clsTimer->getTns(Rsyn::EARLY);
        cout << std::setw(N) << clsTimer->getWns(Rsyn::LATE);
        cout << std::setw(N) << clsTimer->getTns(Rsyn::LATE);
        cout << std::fixed << std::setprecision(5);
        cout << std::setw(N) << getAbu();
        cout << std::fixed << std::setprecision(2);
        cout << std::setw(8) << getQualityScore();
        cout << std::scientific << std::setprecision(5);
        cout << std::setw(N)
             << (clsRoutingEstimator->getTotalWirelength() /
                 (float)clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU));

        cout << "\n";
        cout << std::endl;

        sss.restore();
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportDigestInline(const bool header, const bool newLine) {
        updateQualityScore();

        if (header) {
                const int M = 82;

                cout << std::setw(M) << std::setfill('=') << "\n"
                     << std::setfill(' ');

                cout << std::setw(8) << "";
                cout << std::setw(8) << "";
                cout << std::setw(13) << "Steiner";
                cout << std::setw(13) << "Early";
                cout << std::setw(13) << "Early";
                cout << std::setw(13) << "Late";
                cout << std::setw(13) << "Late";
                cout << "\n";
                cout << std::setw(8) << "Score";
                cout << std::setw(8) << "Abu";
                cout << std::setw(13) << "Wirelength";
                cout << std::setw(13) << "WNS";
                cout << std::setw(13) << "TNS";
                cout << std::setw(13) << "WNS";
                cout << std::setw(13) << "TNS";
                cout << "\n";

                cout << std::setw(M) << std::setfill('=') << "\n"
                     << std::setfill(' ');
        }  // end if

        StreamStateSaver sss(cout);

        cout << std::fixed << std::setprecision(2);
        cout << std::setw(8) << getQualityScore();

        cout << std::fixed << std::setprecision(5);
        cout << std::setw(8) << getAbu();

        cout << std::scientific << std::setprecision(5);
        cout << std::setw(13) << clsRoutingEstimator->getTotalWirelength();
        cout << std::setw(13) << clsTimer->getWns(Rsyn::EARLY);
        cout << std::setw(13) << clsTimer->getTns(Rsyn::EARLY);
        cout << std::setw(13) << clsTimer->getWns(Rsyn::LATE);
        cout << std::setw(13) << clsTimer->getTns(Rsyn::LATE);

        if (newLine) cout << "\n";

        sss.restore();
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportCellPlacement(Rsyn::Cell cell) {
        std::cout << "cell: ";

        if (!cell) {
                std::cout << "<null>\n";
        } else {
                Rsyn::PhysicalCell physicalCell =
                    clsPhysicalDesign.getPhysicalCell(cell);
                DBUxy initialPos = physicalCell.getInitialPosition();
                DBUxy cellPos = physicalCell.getPosition();
                std::cout
                    << cell.getName() << " "
                    //<< "x=" << physicalCell.xmin() << " "
                    //<< "y=" << physicalCell.ymin() << " "
                    //<< "ox=" << physicalCell.initialPos.x << " "
                    //<< "oy=" << physicalCell.initialPos.y << " "
                    //<< "disp=" <<
                    // computeManhattanDistanceFromInitialPosition(physicalCell)
                    //<< " "
                    << "x=" << cellPos[X] << " "
                    << "y=" << cellPos[Y] << " "
                    << "ox=" << initialPos[X] << " "
                    << "oy=" << initialPos[Y] << " "
                    << "disp="
                    << computeManhattanDistanceFromInitialPosition(cell) << " "

                    << "(" << getMaxDisplacement() << ")\n";
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportInvsDigest() {
        int numInvs = 0;
        int numCells = clsDesign.getNumInstances(Rsyn::CELL);
        int numConnectToInv = 0;

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (cell.isPort()) continue;
                if (isInverterOrBuffer(cell))
                        numInvs++;
                else
                        continue;  // cell is not an inversor

                for (Rsyn::Pin outPin : cell.allPins(Rsyn::OUT)) {
                        if (!outPin.isConnected()) continue;

                        Rsyn::Net net = outPin.getNet();
                        if (net.getNumPins() != 2) continue;

                        for (Rsyn::Pin netPin : net.allPins(Rsyn::IN)) {
                                if (netPin.isPort()) continue;
                                if (isInverterOrBuffer(netPin))
                                        numConnectToInv++;
                        }  // end for
                }          // end for
        }                  // end for

        cout << "Report Inverters/Buffers"
             << "\n";
        cout << "--------------------------------------------------------------"
                "------------------\n";
        cout << "NumCells                    : " << numCells << "\n";
        cout << "NumInvs                     : " << numInvs << "\n";
        cout << "NumConToInv                 : " << numConnectToInv << "\n";

        cout << std::fixed << setprecision(2);
        cout << "NumInvs/NumCells         (%): " << (100.0 * numInvs / numCells)
             << "\n";
        cout << "NumConnectToInv/NumCells (%): "
             << (100.0 * numConnectToInv / numCells) << "\n";
        cout << "NumConnectToInv/NumInvs  (%): "
             << (100.0 * numConnectToInv / numInvs) << "\n";
        cout << "--------------------------------------------------------------"
                "------------------\n";
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportTopCentralCells(
    const Rsyn::TimingMode mode, const int maxNumCellsToReport,
    const bool showButDontCountFixedCells) {
        std::deque<std::tuple<Number, Rsyn::Cell>> cells;

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                cells.push_back(std::make_tuple(
                    clsTimer->getCellCentrality(cell, mode), cell));
        }  // end for

        std::sort(cells.begin(), cells.end());

        int counter = 0;

        const int numCells = cells.size();
        for (int i = numCells - 1; i >= 0; i--) {
                Number centrality = std::get<0>(cells[i]);
                Rsyn::Cell cell = std::get<1>(cells[i]);
                if (!cell.isFixed() || !showButDontCountFixedCells) counter++;
                if (counter > maxNumCellsToReport) break;

                std::cout << cell.getName() << " " << centrality
                          << " fixed=" << cell.isFixed() << "\n";
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportTopCriticalCells(
    const Rsyn::TimingMode mode, const int maxNumCellsToReport,
    const bool showButDontCountFixedCells) {
        std::deque<std::tuple<Number, Rsyn::Cell>> cells;

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                cells.push_back(std::make_tuple(
                    clsTimer->getCellCriticality(cell, mode), cell));
        }  // end for

        std::sort(cells.begin(), cells.end());

        int counter = 0;

        const int numCells = cells.size();
        for (int i = numCells - 1; i >= 0; i--) {
                Number centrality = std::get<0>(cells[i]);
                Rsyn::Cell cell = std::get<1>(cells[i]);
                if (!cell.isFixed() || !showButDontCountFixedCells) counter++;
                if (counter > maxNumCellsToReport) break;

                std::cout << cell.getName() << " " << centrality
                          << " fixed=" << cell.isFixed() << "\n";
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportPathLengths(
    const std::string &label,
    const std::vector<std::vector<Rsyn::Timer::PathHop>> &paths) {
        double totalLength = 0;
        double totalScaledLength = 0;
        double maxLength = -std::numeric_limits<double>::infinity();
        double minLength = +std::numeric_limits<double>::infinity();
        double maxScaledLength = -std::numeric_limits<double>::infinity();
        double minScaledLength = +std::numeric_limits<double>::infinity();

        const int numPaths = paths.size();
        for (int i = 0; i < numPaths; i++) {
                const std::vector<Rsyn::Timer::PathHop> &path = paths[i];
                if (path.empty()) {
                        continue;  // defensive programming
                }                  // end method

                const Rsyn::Timer::PathHop &endpoint = path.back();
                const double criticality =
                    clsTimer->getCriticality(endpoint.getSlack(), Rsyn::LATE);

                const double length = computePathManhattanLength(path);
                const double scaledLength = criticality * length;

                totalLength += length;
                totalScaledLength += scaledLength;

                maxLength = std::max(maxLength, length);
                minLength = std::min(minLength, length);

                maxScaledLength = std::max(maxScaledLength, scaledLength);
                minScaledLength = std::min(minScaledLength, scaledLength);
        }  // end for

        StreamStateSaver sss(cout);
        cout << std::scientific << setprecision(5);

        std::cout << "---------------------------------------------------------"
                     "-----------------------\n";
        std::cout << "Path Length Report: " << label << "\n";
        std::cout << "---------------------------------------------------------"
                     "-----------------------\n";

        const int W0 = 15;
        const int W1 = 13;

        std::cout << setw(W0) << "" << setw(W1) << "Total" << setw(W1) << "Avg"
                  << setw(W1) << "Min" << setw(W1) << "Max"
                  << "\n";

        std::cout << setw(W0) << "Length" << setw(W1) << totalLength << setw(W1)
                  << (totalLength / numPaths) << setw(W1) << minLength
                  << setw(W1) << maxLength << "\n";

        std::cout << setw(W0) << "Scaled Length" << setw(W1)
                  << totalScaledLength << setw(W1)
                  << (totalScaledLength / numPaths) << setw(W1)
                  << minScaledLength << setw(W1) << maxScaledLength << "\n";

        std::cout << "\n";
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportNetDegreeHistogram() {
        std::vector<int> histogram;

        StreamStateSaver sss(std::cout);
        std::cout << std::fixed << std::setprecision(2);

        int totalNets = 0;
        for (Rsyn::Net net : clsModule.allNets()) {
                const int degree = net.getNumPins();

                if (degree >= histogram.size()) {
                        histogram.resize(degree + 1, 0);
                }  // end if

                histogram[degree]++;
                totalNets++;
        }  // end for

        int cumulative = 0;
        for (int i = 0; i < histogram.size(); i++) {
                cumulative += histogram[i];
                if (i < 20 || histogram[i] > 0)
                        std::cout << std::setw(6) << i << " " << std::setw(6)
                                  << (100 * histogram[i] / double(totalNets))
                                  << "% " << std::setw(6)
                                  << (100 * cumulative / double(totalNets))
                                  << "% " << histogram[i] << "\n";
        }  // end for

}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportCountOfTopCriticalPathsOnCriticalEndPoints(
    std::ostream &out, int numEndPoints, int numPaths) {
        vector<Rsyn::Pin> criticalEndPoints;
        vector<int> numTopCritical(numEndPoints, 0);
        vector<int> numTotalPaths(numEndPoints, 0);

        clsTimer->queryTopCriticalEndpoints(Rsyn::LATE, numEndPoints,
                                            criticalEndPoints);

        vector<vector<Rsyn::Timer::PathHop>> paths;
        for (int i = 0; i < numEndPoints - 1; i++) {
                const Rsyn::Pin endPoint = criticalEndPoints[i];

                clsTimer->queryTopCriticalPathsFromEndpoint(
                    Rsyn::LATE, endPoint, numPaths, paths);

                const Number nextEndPointWorstSlack =
                    clsTimer->getPinWorstSlack(criticalEndPoints[i + 1],
                                               Rsyn::LATE);

                numTotalPaths[i] = paths.size();

                for (int j = 0; j < paths.size(); j++) {
                        const Number pathSlack = paths[j].back().getSlack();

                        if (pathSlack < nextEndPointWorstSlack)
                                numTopCritical[i]++;
                        else
                                break;
                }  // end for
        }          // end for

        out << "Top critical paths per endpoint\n";
        out << "End point,#top,#total\n";
        for (int i = 0; i < numEndPoints; i++) {
                out << criticalEndPoints[i].getFullName() << ","
                    << numTopCritical[i] << "," << numTotalPaths[i] << "\n";
        }  // end for

        //	out << "\n\nSlacks\n";
        //	for( auto path : paths ) {
        //		out << path.back().getSlack() << "\n";
        //	} // end for
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportSlackOnTopCriticalEndPoints(std::ostream &out,
                                                       const int numEndPoints) {
        out << "Pin name,Slack\n";

        vector<Rsyn::Pin> criticalEndPoints;

        clsTimer->queryTopCriticalEndpoints(Rsyn::LATE, numEndPoints,
                                            criticalEndPoints,
                                            std::numeric_limits<float>::max());

        for (Rsyn::Pin pin : criticalEndPoints) {
                double slack = clsTimer->getPinWorstSlack(pin, Rsyn::LATE);

                if (slack >= 1e+15) continue;

                out << pin.getFullName() << "," << slack << "\n";
        }  // end for

        out.flush();
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportTotalSlackOnTopCriticalEndPoints(
    std::ostream &out, const int numPathsPerEndPoint) {
        std::vector<Rsyn::Pin> endPoints;

        clsTimer->queryTopCriticalEndpoints(
            Rsyn::LATE, clsTimer->getNumCriticalEndpoints(Rsyn::LATE),
            endPoints);

        out << "EndPoint, #critical paths,tns\n";
        for (Rsyn::Pin endPoint : endPoints) {
                std::vector<std::vector<Rsyn::Timer::PathHop>> paths;
                clsTimer->queryTopCriticalPathsFromEndpoint(
                    Rsyn::LATE, endPoint, numPathsPerEndPoint, paths);

                const int numPaths = paths.size();
                double tns = 0.0;
                for (int i = 0; i < numPaths; i++) {
                        const Number currTns = paths[i].back().getSlack();

                        if (currTns <= 0)  // defensive programming
                                tns += currTns;
                }  // end for

                out << endPoint.getFullName() << "," << numPaths << "," << tns
                    << "\n";
        }  // end for

        out.flush();
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportSlackOnTopEndPoints(std::ostream &out,
                                               const int numEndPoints) {
        std::vector<Rsyn::Pin> endPoints;

        clsTimer->queryTopCriticalEndpoints(Rsyn::LATE, numEndPoints, endPoints,
                                            std::numeric_limits<float>::max());

        out << "EndPoint, WNS\n";
        for (Rsyn::Pin endPoint : endPoints) {
                const Number endPointWNS =
                    clsTimer->getPinWorstSlack(endPoint, Rsyn::LATE);

                if (FloatingPoint::approximatelyEqual(endPointWNS, 1e16f))
                        continue;

                out << endPoint.getFullName() << ", " << endPointWNS << "\n";
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportCurrLoadOnCriticalPath(
    std::ostream &out, std::vector<Rsyn::Timer::PathHop> path) {
        double totalLoad = 0;
        double totalLoadSquared = 0;
        for (Rsyn::Timer::PathHop &hop : path) {
                const Rsyn::Pin pin = hop.getPin();

                if (!pin.isDriver()) continue;

                Rsyn::Net net = pin.getNet();

                if (!net) continue;

                const double load =
                    clsTimer->getNetLoad(net)[hop.getTransition()];

                const Rsyn::Arc arc = hop.getArcToThisPin();

                if (!arc) continue;

                const double dStrength =
                    clsLibraryCharacterizer->getDriverResistance(
                        arc, Rsyn::LATE, hop.getTransition());

                totalLoad += load * dStrength;
        }  // end for

        out << "\tTotal load on path: " << totalLoad << "\n";

}  // end method

// -----------------------------------------------------------------------------

void Infrastructure::reportLoadOnTopCriticalNets(std::ostream &out) {
        std::vector<std::tuple<Number, Rsyn::Net>> criticalNets;

        for (Rsyn::Net net : clsModule.allNets()) {
                const Rsyn::Pin driver = net.getAnyDriver();

                if (!driver) continue;

                const Number driverCriticality =
                    clsTimer->getPinCriticality(driver, Rsyn::LATE);

                if (driverCriticality < 0.7) continue;

                const Number load = clsTimer->getNetLoad(net).getMax();

                const Number dStrength =
                    clsLibraryCharacterizer->getCellMaxDriverResistance(
                        driver.getInstance(), Rsyn::LATE);

                const Number RC = load * dStrength;

                criticalNets.emplace_back(RC * driverCriticality, net);
        }  // end for

        std::sort(criticalNets.begin(), criticalNets.end());

        out << "Net,Driver,Weighted Load\n";
        for (auto &element : criticalNets) {
                const Rsyn::Net net = std::get<1>(element);
                out << net.getName() << ", ";
                out << (net.getAnyDriver().getFullName()) << ", ";
                out << std::get<0>(element) << "\n";
        }  // end for
}  // end method

} /* namespace Rsyn */
