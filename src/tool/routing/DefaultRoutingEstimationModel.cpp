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

#include <random>

#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "flute/flute.h"
#include "util/Stopwatch.h"

#include "DefaultRoutingEstimationModel.h"
#include "RoutingTopology.h"

namespace Rsyn {

// -----------------------------------------------------------------------------

const bool
    DefaultRoutingEstimationModel::ENABLE_DO_NOT_USE_FLUTE_FOR_2_PIN_NETS =
        true;

// -----------------------------------------------------------------------------

void DefaultRoutingEstimationModel::start(const Rsyn::Json &params) {
        Rsyn::Session session;

        design = session.getDesign();
        module = design.getTopModule();

        clsScenario = session.getService("rsyn.scenario");

        clsPhysicalDesign = session.getPhysicalDesign();

        Flute::readLUT();
}  // end method

// -----------------------------------------------------------------------------

void DefaultRoutingEstimationModel::stop() {}  // end method

// -----------------------------------------------------------------------------

void DefaultRoutingEstimationModel::updateRoutingEstimation(
    Rsyn::Net net, Rsyn::RoutingTopologyDescriptor<int> &topology,
    DBU &wirelength) {
        wirelength = generateSteinerTree(net, topology);
}  // end method

// -----------------------------------------------------------------------------

DBU DefaultRoutingEstimationModel::generateSteinerTree(
    Rsyn::Net net, Rsyn::RoutingTopologyDescriptor<int> &topology) {
        // [NOTE] A pin not necessarily will be connected to a tree endpoint.

        const unsigned numPins = net.getNumPins();

        int *mapPinNodeIndexToFluteNodeIndex = new int[numPins];
        int counter = 0;
        int offset2driver = -1;

        topology.clear();
        DBU netSteinerWirelength = 0;
        std::set<std::tuple<int, int>> edges;

        // Build the Steiner tree.
        if (ENABLE_DO_NOT_USE_FLUTE_FOR_2_PIN_NETS && numPins == 2) {
                // [NOTE] For 2-pin nets, ICCAD 2014 Contest evaluation script
                // does
                // not round the pin positions.

                DBU x[2];
                DBU y[2];
                for (Rsyn::Pin pin : net.allPins()) {
                        const DBUxy pinPos =
                            clsPhysicalDesign.getPinPosition(pin);
                        x[counter] = pinPos[X];
                        y[counter] = pinPos[Y];

                        mapPinNodeIndexToFluteNodeIndex[counter] = counter;

                        if (pin.isDriver()) {
                                offset2driver = counter;
                        }  // end if

                        counter++;
                }  // end for

                const DBU wirelength =
                    (std::abs(x[0] - x[1]) + std::abs(y[0] - y[1]));

                // Create segment.
                topology.addSegment(0, 1);
                netSteinerWirelength = wirelength;

                // Set node positions.
                topology.setNodePosition(0, x[0], y[0]);
                topology.setNodePosition(1, x[1], y[1]);

        } else {
                FLUTE_DTYPE *x = new FLUTE_DTYPE[numPins];
                FLUTE_DTYPE *y = new FLUTE_DTYPE[numPins];
                for (Rsyn::Pin pin : net.allPins()) {
                        const DBUxy pinPos =
                            clsPhysicalDesign.getPinPosition(pin);
                        x[counter] = (FLUTE_DTYPE)(pinPos[X]);
                        y[counter] = (FLUTE_DTYPE)(pinPos[Y]);

                        if (pin.isOutput()) {
                                offset2driver = counter;
                        }  // end if

                        counter++;
                }  // end for

                // Build the FLUTE tree.
                Flute::Tree flutetree =
                    Flute::flute(numPins, x, y, FLUTE_ACCURACY,
                                 mapPinNodeIndexToFluteNodeIndex);

                // FLUTE may return steiner points at the same position of
                // regular
                // points and also more than one steiner point at same position,
                // so we
                // translate such steiner points to a regular point or an
                // already
                // processed steiner point in order to avoid zero resistance and
                // to be
                // compliant to ICCAD 2014 evaluation script. Note, however,
                // that we
                // never merge to regular points at same position.

                map<pair<FLUTE_DTYPE, FLUTE_DTYPE>, int>
                    mapCoordToFluteNodeIndex;

                counter = 0;
                for (Rsyn::Pin pin : net.allPins()) {
                        mapCoordToFluteNodeIndex[std::make_pair(x[counter],
                                                                y[counter])] =
                            mapPinNodeIndexToFluteNodeIndex[counter];
                        counter++;
                }  // end for

                const int numBranches = 2 * flutetree.deg - 2;
                for (int j = 0; j < numBranches; j++) {
                        const int i = flutetree.branch[j].n;

                        // FLUTE indicates that there's no branching from the
                        // current node
                        // by setting the neighbor node index equals to the
                        // current node
                        // index.
                        if (j == i) continue;

                        const DBU wirelength = std::abs(flutetree.branch[j].x -
                                                        flutetree.branch[i].x) +
                                               std::abs(flutetree.branch[j].y -
                                                        flutetree.branch[i].y);

                        // We use maps to remove points at same position and
                        // hence zero
                        // resistance connections. Remember, FLUTE may return
                        // several
                        // points at same position.

                        const std::pair<FLUTE_DTYPE, FLUTE_DTYPE> point_j =
                            std::make_pair(flutetree.branch[j].x,
                                           flutetree.branch[j].y);
                        const std::pair<FLUTE_DTYPE, FLUTE_DTYPE> point_i =
                            std::make_pair(flutetree.branch[i].x,
                                           flutetree.branch[i].y);

                        // Handling nodes at same position... If it's a regular
                        // node, use
                        // the node index itself and ignore if there are more
                        // than one
                        // regular point at same position. If it's a steiner
                        // node, try to
                        // find a regular node or other already processed
                        // steiner point at
                        // same position.
                        int index_j;
                        if (j < numPins) {
                                // regular node
                                index_j = j;
                        } else {
                                // steiner node
                                auto it_j =
                                    mapCoordToFluteNodeIndex.find(point_j);
                                if (it_j != mapCoordToFluteNodeIndex.end()) {
                                        index_j = it_j->second;
                                } else {
                                        index_j = j;
                                        mapCoordToFluteNodeIndex[point_j] = j;
                                }  // end else
                        }          // end else

                        // Handling nodes at same position... Similar as above.
                        // However note
                        // that this search happens after the previous point was
                        // added to
                        // the map so that if this next point is at exactly the
                        // same of the
                        // previous one we are aware of that.
                        int index_i;
                        if (i < numPins) {
                                // regular node
                                index_i = i;
                        } else {
                                // steiner node
                                auto it_i =
                                    mapCoordToFluteNodeIndex.find(point_i);
                                if (it_i != mapCoordToFluteNodeIndex.end()) {
                                        index_i = it_i->second;
                                } else {
                                        index_i = i;
                                        mapCoordToFluteNodeIndex[point_i] = i;
                                }  // end else
                        }          // end else

                        if (index_j == index_i) continue;

                        // Check if this edge was previously added. This may
                        // happen since
                        // Flute may return one more Steiner points at the same
                        // position and
                        // we merge that points.
                        const auto e =
                            std::make_tuple(std::min(index_j, index_i),
                                            std::max(index_j, index_i));

                        if (edges.count(e)) {
                                continue;
                        } else {
                                edges.insert(e);
                        }  // end else

                        // Create segment.
                        topology.addSegment(index_i, index_j);
                        netSteinerWirelength += wirelength;
                }  // end for

                // Define default iterator and annotated node positions.
                for (auto item : mapCoordToFluteNodeIndex) {
                        topology.setNodePosition(item.second, item.first.first,
                                                 item.first.second);
                }  // end method

#ifdef DIAGNOSTIC
                assert(
                    x[offset2driver] ==
                    flutetree
                        .branch[mapPinNodeIndexToFluteNodeIndex[offset2driver]]
                        .x);
                assert(
                    y[offset2driver] ==
                    flutetree
                        .branch[mapPinNodeIndexToFluteNodeIndex[offset2driver]]
                        .y);

                // Testing for the mapping which was a tweak we did in the FLUTE
                // code
                // to retrieve the mapping between pins and tree nodes.
                vector<bool> touched(numPins, false);
                for (int i = 0; i < numPins; i++) {
                        touched[mapPinNodeIndexToFluteNodeIndex[i]] = true;
                        if ((x[i] !=
                             flutetree
                                 .branch[mapPinNodeIndexToFluteNodeIndex[i]]
                                 .x) ||
                            (y[i] !=
                             flutetree
                                 .branch[mapPinNodeIndexToFluteNodeIndex[i]]
                                 .y))
                                std::cout << "[BUG] "
                                          << "Pin and FLUTE tree node do not "
                                             "match\n.";
                }  // end for

                for (int i = 0; i < numPins; i++) {
                        if (!touched[i])
                                std::cout << "[BUG] "
                                          << "Pin not reached by any FLUTE "
                                             "branch.\n.";
                }  // end for
#endif

                delete[] x;
                delete[] y;
                free(flutetree.branch);
        }  // end else

        // Replace default iterator with the iterator connected to the tree
        // node.
        // Note that a pin not necessarily will be connected to a tree endpoint.
        counter = 0;
        for (Rsyn::Pin pin : net.allPins()) {
                topology.setAttachedPin(
                    mapPinNodeIndexToFluteNodeIndex[counter], pin);
                counter++;
        }  // end for

        delete[] mapPinNodeIndexToFluteNodeIndex;
        return netSteinerWirelength;
}  // end method

// -----------------------------------------------------------------------------

void DefaultRoutingEstimationModel::generateFluteAvgRuntimeTable(
    const int N, const double maxSecondsPerDegree,
    const int minNumTreesPerDegree, const int largeStepStart,
    const int largeStep) {
        std::default_random_engine generator;
        std::uniform_int_distribution<int> randomNumber(0, 1000);

        Stopwatch watch;

        FLUTE_DTYPE *x = new FLUTE_DTYPE[N];
        FLUTE_DTYPE *y = new FLUTE_DTYPE[N];
        int *mapping = new int[N];

        const int W1 = 7;
        const int W2 = 14;

        std::cout << std::setw(W1) << "Degree" << std::setw(W2) << "Counter"
                  << std::setw(W2) << "Avg Runtime" << std::setw(W2) << "Avg WL"
                  << std::setw(W2) << "StDev WL"
                  << "\n";

        for (int n = 2; n <= N; (n >= largeStepStart ? n += largeStep : n++)) {
                double avgWirelength = 0;
                double sigmaWirelength = 0;
                int counter = 0;

                watch.reset();
                watch.start();
                do {
                        counter++;

                        for (int i = 0; i < n; i++) {
                                x[i] = (FLUTE_DTYPE)(randomNumber(generator));
                                y[i] = (FLUTE_DTYPE)(randomNumber(generator));
                        }  // end for

                        // Build the FLUTE tree.
                        Flute::Tree tree =
                            Flute::flute(n, x, y, FLUTE_ACCURACY, mapping);

                        // Compute wirelength.
                        const DBU wl = Flute::wirelength(tree);
                        avgWirelength += wl;
                        sigmaWirelength += wl * wl;
                } while (counter < minNumTreesPerDegree ||
                         watch.getElapsedTime() < maxSecondsPerDegree);
                watch.stop();

                avgWirelength /= counter;
                sigmaWirelength /= counter;
                sigmaWirelength = std::sqrt(sigmaWirelength -
                                            (avgWirelength * avgWirelength));

                std::cout << std::setw(W1) << n << std::setw(W2) << counter
                          << std::setw(W2)
                          << (watch.getElapsedTime() / Number(counter))
                          << std::setw(W2) << avgWirelength << std::setw(W2)
                          << sigmaWirelength << "\n";
        }  // end for

        delete[] x;
        delete[] y;
        delete[] mapping;
}  // end method

}  // end namespace
