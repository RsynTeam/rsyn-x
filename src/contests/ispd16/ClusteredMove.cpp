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

#include "ClusteredMove.h"

#include "tool/timing/Timer.h"
#include "util/AsciiProgressBar.h"
#include "util/Stepwatch.h"
#include "iccad15/Infrastructure.h"

namespace ICCAD15 {

bool ClusteredMove::run(const Rsyn::Json &params) {
        this->session = session;
        this->infra = session.getService("ufrgs.ispd16.infra");
        this->design = session.getDesign();
        this->timer = session.getService("rsyn.timer");
        this->module = session.getTopModule();
        this->phDesign = session.getPhysicalDesign();

        moved = design.createAttribute();

        for (int i = 0; i < 10; i++) {
                runClusteredMovement(1250);
                infra->checkMaxDisplacementViolation("step0");
                infra->checkUnlegalizedCells("step0");

                timer->updateTimingIncremental();
                infra->updateQualityScore();
                infra->reportSummary("clustered-move");

                if (!infra->updateTDPSolution()) {
                        // method worsen the current solution, stop here...
                        break;
                }  // end if
        }          // end for

        return true;
}  // end method

// -----------------------------------------------------------------------------

void ClusteredMove::runClusteredMovement(const int N) {
        Stepwatch watch("Clustered Movement");

        AsciiProgressBar bar(N);

        std::vector<std::pair<Number, Rsyn::Pin> > criticalPins;
        for (Rsyn::Instance instance : module.allInstances()) {
                for (Rsyn::Pin pin : instance.allPins()) {
                        Rsyn::Cell cell = pin.getInstance().asCell();
                        double eSlack = getWorstSlack(cell, Rsyn::EARLY);
                        double lSlack =
                            timer->getPinWorstNegativeSlack(pin, Rsyn::LATE);

                        if (lSlack < 0 && eSlack >= 0)
                                criticalPins.emplace_back(lSlack, pin);
                }  // end for
        }          // end for

        std::sort(criticalPins.begin(), criticalPins.end());

        for (auto cell : module.allInstances()) moved[cell] = false;

        const int num = std::min(N, (int)criticalPins.size());
        for (int i = 0; i < num; i++) {
                bar.progress();

                if (moved[criticalPins[i].second.getInstance()]) continue;

                clusterNeighborCriticalNets(criticalPins[i].second, true);
        }  // end for

        bar.finish();
}  // end method

// -----------------------------------------------------------------------------

void ClusteredMove::clusterNeighborCriticalNets(Rsyn::Pin criticalPin,
                                                const bool dontMoveRegisters) {
        const bool enableWeightening = false;

        //	Rsyn::Pin criticalPin = timer->getWnsPin( TimingMode::Rsyn::LATE
        //);
        Rsyn::Cell criticalCell = criticalPin.getInstance().asCell();
        auto pOrigin = phDesign.getPhysicalCell(criticalCell);

        //	double initSlack = - timer->getPinWorstNegativeSlack(
        // criticalPin, Rsyn::LATE );
        // const double maxDist =  10 * session->getRows()[0].propHeight;
        const double maxDist = 10 * phDesign.getRowHeight();
        //	std::cout << "Max dist = " << maxDist << std::endl;

        Rsyn::Attribute<Rsyn::Net, bool> visited = design.createAttribute();

        for (Rsyn::Net net : module.allNets()) visited[net] = false;

        std::set<Rsyn::Cell> cells;
        std::list<std::pair<Rsyn::Net, int> > netList;

        Rsyn::Net cNet = criticalPin.getNet();

        if (!cNet) return;

        netList.emplace_back(cNet, 0);

        do {
                Rsyn::Net currNet = netList.front().first;
                const int currDepth = netList.front().second;
                netList.pop_front();

                if (currDepth == 10) break;

                for (Rsyn::Pin pin : currNet.allPins()) {
                        Rsyn::Cell cell = pin.getInstance().asCell();
                        auto pCell = phDesign.getPhysicalCell(cell);

                        //			double currSlack = -
                        // timer->getPinWorstNegativeSlack( pin, Rsyn::LATE );
                        //			if( std::abs( currSlack -
                        // initSlack
                        //) / std::max( currSlack, initSlack ) > 1 )
                        //				continue;
                        //
                        DBUxy dist = pCell.getCenter() - pOrigin.getCenter();
                        dist.abs();

                        if (dist.aggregated() > maxDist) continue;

                        if (!cell.isFixed()) cells.emplace(cell);

                        for (Rsyn::Pin cPin : cell.allPins()) {
                                if (cPin == pin) continue;

                                if (!(timer->getPinWorstNegativeSlack(
                                          cPin, Rsyn::LATE) < 0))
                                        continue;

                                Rsyn::Net net = cPin.getNet();

                                if (net == nullptr || visited[net]) continue;

                                visited[net] = true;
                                netList.emplace_back(net, currDepth + 1);
                        }
                }

        } while (!netList.empty());

        DBUxy targetPosition(0, 0);
        double totalSlack = 0;
        /* [TODO] Put in another method */
        for (auto cell = cells.begin(); cell != cells.end(); cell++) {
                for (auto pin : (*cell).allPins()) {
                        const auto net = pin.getNet();

                        if (!net) continue;

                        for (auto neightbor : net.allPins()) {
                                const double slack =
                                    timer->getPinWorstNegativeSlack(neightbor,
                                                                    Rsyn::LATE);
                                if (!(slack < 0)) continue;

                                auto neightborCell = neightbor.getInstance();
                                if (cells.find(neightborCell.asCell()) !=
                                    cells.end())
                                        continue;

                                const double weight =
                                    enableWeightening
                                        ? (1 +
                                           infra->getPinImportance(
                                               neightbor, Rsyn::LATE)) /
                                              2
                                        : 1;
                                totalSlack += weight * slack;
                                auto pNeightbor = phDesign.getPhysicalCell(
                                    neightborCell.asCell());
                                targetPosition.x += (DBU)(
                                    weight * slack * pNeightbor.getCenter(X));
                                targetPosition.y += (DBU)(
                                    weight * slack * pNeightbor.getCenter(Y));
                        }
                }
        }

        //
        //

        targetPosition /= totalSlack;
        //	std::cout << "Target: " << targetPosition << std::endl;

        DBUxy clusterCenter(0, 0);
        double totalWeight = 0;
        for (auto cell = cells.begin(); cell != cells.end(); cell++) {
                auto pCell = phDesign.getPhysicalCell(*cell);

                if (enableWeightening) {
                        double worstWeight = 0;
                        for (Rsyn::Pin pin : (*cell).allPins(Rsyn::OUT)) {
                                const double weight =
                                    (1 +
                                     infra->getPinImportance(pin, Rsyn::LATE) /
                                         2);
                                worstWeight = std::max(worstWeight, weight);
                        }  // end for
                        clusterCenter += worstWeight * pCell.getCenter();
                        totalWeight += worstWeight;
                } else {
                        clusterCenter += pCell.getCenter();
                        totalWeight += 1;
                }
        }

        clusterCenter /= totalWeight;
        DBUxy diff = (targetPosition - clusterCenter);

        for (std::set<Rsyn::Cell>::iterator it = cells.begin();
             it != cells.end(); it++) {
                if (dontMoveRegisters && (*it).isSequential()) continue;

                // auto& pCell = clsPhysicalCells[*cell];
                // DBUxy newPos = pCell.lower() + diff;
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(*it);
                DBUxy newPos = phCell.getPosition() + diff;
                //		session->moveCell( *cell, newPos,
                // LegalizationMethod::LEG_NEAREST_WHITESPACE );
                double oldCost, newCost;

                Rsyn::Instance instance = *it;
                Rsyn::Cell cell = instance.asCell();
                infra->moveCellWithCostEvaluation(
                    cell, newPos.x, newPos.y,
                    LegalizationMethod::LEG_NEAREST_WHITESPACE,
                    COST_LOCAL_DELAY, Rsyn::LATE, oldCost, newCost);
                moved[*it] = true;
        }
}

// -----------------------------------------------------------------------------

Number ClusteredMove::getWorstSlack(const Rsyn::Cell cell,
                                    const Rsyn::TimingMode mode) {
        Number worstSlack = 0.0;

        for (auto pin : cell.allPins())
                worstSlack =
                    std::min(worstSlack, timer->getPinWorstSlack(pin, mode));

        return worstSlack;
}

}  // end namescape
