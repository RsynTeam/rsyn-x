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

#include "AbuReduction.h"

#include "tool/timing/Timer.h"
#include "tool/routing/RoutingEstimator.h"
#include "util/AsciiProgressBar.h"
#include "util/Stepwatch.h"
#include "iccad15/Infrastructure.h"

namespace ICCAD15 {

bool AbuReduction::run(const Rsyn::Json &params) {
        this->session = session;
        this->infra = session.getService("ufrgs.ispd16.infra");
        this->design = session.getDesign();
        this->timer = session.getService("rsyn.timer");
        this->routingEstimator = session.getService("rsyn.routingEstimator");
        this->module = session.getTopModule();
        this->phDesign = session.getPhysicalDesign();

        if (infra->getAbu() > 0) {
                double previousABU;
                double previousStWL;
                do {
                        previousABU = infra->getAbu();
                        previousStWL = routingEstimator->getTotalWirelength();

                        runAbuReduction();
                        timer->updateTimingIncremental();
                        infra->updateQualityScore();
                        infra->reportSummary("abu-fix");

                        // Stop if the TDP degrades.
                        if (!infra->updateTDPSolution(false, 0)) {
                                break;
                        }  // end if
                } while (
                    infra->getAbu() > 0 &&
                    (infra->getAbu() / previousABU <= 0.9) &&
                    (routingEstimator->getTotalWirelength() / previousStWL <=
                     1.01));
        }  // end if

        return true;
}  // end method

// -----------------------------------------------------------------------------

void AbuReduction::runAbuReduction() {
        computeABUVioaltionCells(0.01);
        moveABUViolationCells();
}  // end method

// -----------------------------------------------------------------------------

void AbuReduction::computeABUVioaltionCells(const double alpha) {
        const ABU &abu = infra->getAbuAnalyser();

        clsABUViolationCells.clear();
        clsABUViolationCells.reserve(design.getNumInstances(Rsyn::CELL));
        const double slackThreshold = alpha * timer->getClockPeriod();

        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (cell.isFixed() || cell.isPort() || cell.isLCB()) continue;
                Number slack = timer->getDriverWorstSlack(cell, Rsyn::LATE);

                // TODO define a threshold for slack
                if (slack < slackThreshold) continue;
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const DBUxy lowerPos = phCell.getCoordinate(LOWER);
                const DBUxy upperPos = phCell.getCoordinate(UPPER);
                int lowerCol, lowerRow, upperCol, upperRow;
                abu.getABUIndex(lowerPos, lowerCol, lowerRow);
                abu.getABUIndex(upperPos, upperCol, upperRow);
                for (int i = lowerRow; i <= upperRow; i++) {
                        double density = 0;
                        for (int j = lowerCol; j <= upperCol; j++) {
                                int binId = abu.getABUBinIndex(j, i);

                                density = abu.getBinUtil(binId);
                                if (density > infra->getTargetUtilization()) {
                                        clsABUViolationCells.push_back(
                                            std::make_tuple(
                                                cell,
                                                std::make_tuple(slack, binId)));
                                        break;
                                }  // end if
                        }          // end for
                        if (density > infra->getTargetUtilization()) break;
                }  // end for
        }          // end for
        clsABUViolationCells.resize(clsABUViolationCells.size());
}  // end method

// -----------------------------------------------------------------------------

void AbuReduction::moveABUViolationCells() {
        ABU &abu = infra->getAbuAnalyser();

        std::vector<std::tuple<Rsyn::Cell, DBUxy>> movedCells;
        movedCells.reserve(clsABUViolationCells.size());
        for (std::tuple<Rsyn::Cell, std::tuple<Number, int>> tp :
             clsABUViolationCells) {
                Rsyn::Cell cell = std::get<0>(tp);
                Number slack = std::get<0>(std::get<1>(tp));
                const int binId = std::get<1>(std::get<1>(tp));
                double bin_util = abu.getBinUtil(binId);
                if (bin_util != abu.getBinUtil(binId))
                        std::cout << "binUtil: " << bin_util
                                  << " obj: " << abu.getBinUtil(binId) << "\n";
                if (bin_util < infra->getTargetUtilization()) continue;
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                DBUxy initLowerPos = phCell.getCoordinate(LOWER);
                DBUxy initUpperPos = phCell.getCoordinate(UPPER);
                DBUxy pos = abu.getNearstNonViolationABU(phCell.getPosition());
                const bool isFixed = cell.isFixed();

                if (pos[X] < 0 || pos[Y] < 0) continue;

                bool moved =
                    infra->moveCell(phCell, pos, LEG_NEAREST_WHITESPACE);

                if (moved) {
                        if (infra->violateMaxDisplacement(phCell)) {
                                infra->moveCell(phCell, initLowerPos,
                                                LEG_NEAREST_WHITESPACE);
                        } else {
                                movedCells.push_back(
                                    std::make_tuple(cell, initLowerPos));

                                abu.removeAbuUtilization(initLowerPos,
                                                         initUpperPos, isFixed);
                                abu.addAbuUtilization(
                                    phCell.getCoordinate(LOWER),
                                    phCell.getCoordinate(UPPER), isFixed);
                        }  // end if-else
                }          // end if
        }                  // end for

        timer->updateTimingIncremental();

        for (std::tuple<Rsyn::Cell, DBUxy> tp : movedCells) {
                Rsyn::Cell cell = std::get<0>(tp);
                Number slack = timer->getDriverWorstSlack(cell, Rsyn::LATE);

                if (slack >= 0)
                        slack = timer->getDriverWorstSlack(cell, Rsyn::EARLY);
                if (slack < 0) {
                        DBUxy pos = std::get<1>(tp);
                        bool move =
                            infra->moveCell(cell, pos, LEG_NEAREST_WHITESPACE);
                }  // end if
        }          // end for
}  // end method

}  // end namescape
