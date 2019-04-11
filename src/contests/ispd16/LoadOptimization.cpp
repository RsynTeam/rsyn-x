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

#include "LoadOptimization.h"

#include "tool/timing/Timer.h"
#include "tool/routing/RoutingEstimator.h"
#include "util/AsciiProgressBar.h"
#include "util/Stepwatch.h"
#include "iccad15/Infrastructure.h"

namespace ICCAD15 {

bool LoadOptimization::run(const Rsyn::Json &params) {
        this->session = session;
        this->infra = session.getService("ufrgs.ispd16.infra");
        this->design = session.getDesign();
        this->timer = session.getService("rsyn.timer");
        this->routingEstimator = session.getService("rsyn.routingEstimator");
        this->module = session.getTopModule();
        this->phDesign = session.getPhysicalDesign();

        const std::string optType = params.count("dontMoveRegistersAndLCBs")
                                        ? params["type"]
                                        : "driver";
        const bool optionDontMoveRegistersAndLCBs =
            params.count("dontMoveRegistersAndLCBs")
                ? params.count("dontMoveRegistersAndLCBs")
                : true;

        if (optType == "steiner") {
                runMoveNonCriticalSinksOfCriticalNetsToSteinerPoint(
                    optionDontMoveRegistersAndLCBs);
        } else if (optType == "driver") {
                runMoveNonCriticalSinksOfCriticalNetsToDriver(
                    optionDontMoveRegistersAndLCBs);
        } else {
                std::cout << "Warning: Invalid optimization type '" << optType
                          << "' "
                             "in load optimization step.\n";
        }  // end else

        return true;
}  // end method

// -----------------------------------------------------------------------------

void LoadOptimization::runMoveNonCriticalSinksOfCriticalNetsToSteinerPoint(
    const bool dontMoveFlopsAndLCBs) {
        Stepwatch watch("Load optimization");

        Rsyn::Attribute<Rsyn::Instance, bool> visited =
            design.createAttribute();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                visited[cell] = false;
        }  // end for

        const double lateSlackThreshold = 0;
        const double earlySlackThreshold = 0;

        int counterMoved = 0;
        int counterLegalizationFail = 0;

        std::vector<std::tuple<double, Rsyn::Net>> orderedNets;
        orderedNets.reserve(design.getNumNets());
        for (Rsyn::Net net : module.allNets()) {
                orderedNets.push_back(std::make_tuple(
                    timer->getNetCriticality(net, Rsyn::LATE) *
                        routingEstimator->getRCTree(net).getTotalWireCap(),
                    net));
        }  // end for
        std::sort(orderedNets.begin(), orderedNets.end());

        const int numElements = orderedNets.size();

        AsciiProgressBar progressBar(numElements);

        for (int i = numElements - 1; i >= 0; i--) {
                Rsyn::Net net = std::get<1>(orderedNets[i]);

                progressBar.progress();

                // Get the driver of the net.
                Rsyn::Pin driver = net.getAnyDriver();

                if (!driver ||
                    timer->getPinWorstSlack(driver, Rsyn::LATE) >= 0 ||
                    timer->getPinWorstSlack(driver, Rsyn::EARLY) <= 0)
                        continue;

                for (Rsyn::Pin sink : net.allPins(Rsyn::SINK)) {
                        Rsyn::Cell cell = sink.getInstance().asCell();

                        if (cell.isPort()) continue;

                        if (visited[cell])
                                continue;
                        else
                                visited[cell] = true;

                        if (cell.isFixed() ||
                            (dontMoveFlopsAndLCBs &&
                             (cell.isLCB() || cell.isSequential())))

                                continue;

                        bool isCritical = false;

                        for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
                                if (timer->getPinWorstSlack(pin, Rsyn::LATE) <
                                        lateSlackThreshold ||
                                    timer->getPinWorstSlack(pin, Rsyn::EARLY) <
                                        earlySlackThreshold) {
                                        isCritical = true;
                                        break;
                                }  // end if
                        }          // end if

                        if (!isCritical) {
                                const Rsyn::RCTree &tree =
                                    routingEstimator->getRCTree(net);

                                // Find the node in three that connects to this
                                // cell.
                                const int numNodes = tree.getNumNodes();
                                for (int i = 0; i < numNodes; i++) {
                                        const Rsyn::RCTreeNodeTag &tag0 =
                                            tree.getNodeTag(i);
                                        if (tag0.getPin() &&
                                            (tag0.getPin().getInstance() ==
                                             cell)) {
                                                const Rsyn::RCTree::Node &node =
                                                    tree.getNode(i);

                                                if (node.propParent !=
                                                    -1) {  // defensive
                                                           // programming (sink
                                                           // should not be at
                                                           // the root)
                                                        const Rsyn::RCTreeNodeTag
                                                            &tag1 = tree.getNodeTag(
                                                                node.propParent);

                                                        const double dx =
                                                            tag1.x - tag0.x;
                                                        const double dy =
                                                            tag1.y - tag0.y;
                                                        if (!infra->translateCell(
                                                                cell, (DBU)dx,
                                                                (DBU)dy,
                                                                LEG_NEAREST_WHITESPACE))
                                                                counterLegalizationFail++;
                                                        counterMoved++;
                                                }  // end else

                                                break;
                                        }  // end if
                                }          // end for
                        }                  // end if
                }                          // end for
        }                                  // end for

        progressBar.finish();
}  // end method

// -----------------------------------------------------------------------------

void LoadOptimization::runMoveNonCriticalSinksOfCriticalNetsToDriver(
    const bool dontMoveFlopsAndLCBs) {
        Stepwatch watch("Load optimization");

        Rsyn::Attribute<Rsyn::Instance, bool> visited =
            design.createAttribute();
        for (Rsyn::Instance instance : module.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                visited[cell] = false;
        }  // end for

        // const double slackThreshold = -0.1*timer->getWns(Rsyn::LATE);
        const double lateSlackThreshold = 0;
        const double earlySlackThreshold = 0;

        int counterMoved = 0;
        int counterLegalizationFail = 0;

        std::vector<std::tuple<double, Rsyn::Net>> orderedNets;
        orderedNets.reserve(design.getNumNets());
        for (Rsyn::Net net : module.allNets()) {
                orderedNets.push_back(std::make_tuple(
                    timer->getNetCriticality(net, Rsyn::LATE) *
                        routingEstimator->getRCTree(net).getTotalWireCap(),
                    net));
        }  // end for
        std::sort(orderedNets.begin(), orderedNets.end());

        const int numElements = orderedNets.size();

        AsciiProgressBar progressBar(numElements);

        for (int i = numElements - 1; i >= 0; i--) {
                Rsyn::Net net = std::get<1>(orderedNets[i]);

                progressBar.progress();

                // Get the driver of the net.
                Rsyn::Pin driver = net.getAnyDriver();

                if (!driver ||
                    timer->getPinWorstSlack(driver, Rsyn::LATE) >= 0 ||
                    timer->getPinWorstSlack(driver, Rsyn::EARLY) <= 0)
                        continue;

                for (Rsyn::Pin sink : net.allPins(Rsyn::SINK)) {
                        Rsyn::Cell cell = sink.getInstance().asCell();

                        if (cell.isPort()) continue;

                        if (visited[cell])
                                continue;
                        else
                                visited[cell] = true;

                        if (cell.isFixed() ||
                            (dontMoveFlopsAndLCBs &&
                             (cell.isLCB() || cell.isSequential())))
                                continue;

                        Rsyn::PhysicalCell phCell =
                            phDesign.getPhysicalCell(cell);
                        const DBUxy lowerCellPos = phCell.getPosition();

                        bool isCritical = false;

                        for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
                                if (timer->getPinWorstSlack(pin, Rsyn::LATE) <
                                        lateSlackThreshold ||
                                    timer->getPinWorstSlack(pin, Rsyn::EARLY) <
                                        earlySlackThreshold) {
                                        isCritical = true;
                                        break;
                                }  // end if
                        }          // end if

                        if (!isCritical) {
                                Rsyn::PhysicalCell phCellDriver =
                                    phDesign.getPhysicalCell(driver);
                                const DBUxy lowerDriverPos =
                                    phCellDriver.getPosition();

                                const double dx =
                                    lowerDriverPos[X] - lowerCellPos[X];
                                const double dy =
                                    lowerDriverPos[Y] - lowerCellPos[Y];
                                double oldCost;
                                double newCost;

                                if (!infra->translateCellWithCostEvaluation(
                                        cell, (DBU)dx, (DBU)dy,
                                        LEG_NEAREST_WHITESPACE,
                                        COST_LOCAL_DELAY, Rsyn::LATE, oldCost,
                                        newCost)) {
                                        counterLegalizationFail++;
                                }  // end if

                                counterMoved++;
                        }  // end if
                }          // end for
        }                  // end for

        progressBar.finish();
}  // end method

}  // end namescape
