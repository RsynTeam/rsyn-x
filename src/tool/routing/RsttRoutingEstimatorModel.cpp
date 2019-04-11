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
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RsttRoutingEstimatorModel.cpp
 * Author: eder
 *
 * Created on 29 de Junho de 2017, 16:44
 */

#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "flute/flute.h"
#include "util/Stopwatch.h"
#include <algorithm>
#include "RsttRoutingEstimatorModel.h"
#include "RoutingTopology.h"

namespace Rsyn {

RsttRoutingEstimatorModel::RsttRoutingEstimatorModel() {}

void RsttRoutingEstimatorModel::start(const Rsyn::Json &params) {
        Rsyn::Session session;

        design = session.getDesign();
        module = design.getTopModule();

        clsScenario = session.getService("rsyn.scenario");

        clsPhysicalDesign = session.getPhysicalDesign();
        clsNetsTopologys =
            design.createAttribute(Rsyn::RoutingTopologyDescriptor<int>());
        // Flute::readLUT();
        std::cout << "It's running \n";

        generateRsttTopologyForAllNets();
}  // end method

void RsttRoutingEstimatorModel::generateRsttTopologyForAllNets() {
        for (Rsyn::Net net : module.allNets()) {
                if (net.getNumPins() < 2 || net == clsScenario->getClockNet()) {
                        continue;
                }
                DBU wl;
                Rsyn::RoutingTopologyDescriptor<int> topo;
                updateRoutingEstimation(net, topo, wl);
                clsNetsTopologys[net] = topo;
        }
        // std::cout << "RSTT topology generated for all nets!\n";
}

void RsttRoutingEstimatorModel::updateRoutingEstimation(
    Net net, RoutingTopologyDescriptor<int> &topology, DBU &wirelength) {
        wirelength = generateSteinerTree(net, topology);
}  // end method

//------------------------------------------------------------------------------

DBU RsttRoutingEstimatorModel::generateSteinerTree(
    Net net, RoutingTopologyDescriptor<int> &topology) {
        if (net.getNumPins() < 2 || net == clsScenario->getClockNet()) {
                return -1;
        }

        RsttRoutingEstimatorModel::Point med;
        DBU wireLengthH;
        DBU wireLengthV;

        std::vector<RsttRoutingEstimatorModel::Point> Pins,
            Points;  // Pins is used in the creation of the segments. Points is
                     // used to create the topology of the tree on Rsyn
        std::vector<Pin> RsynPins;  // Also used in the creation of the topology
        std::vector<RsttRoutingEstimatorModel::Segment> TrunkH, TrunkV;

        for (Pin pin : net.allPins()) {
                const DBUxy pinPos = clsPhysicalDesign.getPinPosition(pin);
                RsttRoutingEstimatorModel::Point p(pinPos.x, pinPos.y);
                Pins.push_back(p);
                Points.push_back(p);
                RsynPins.push_back(pin);
        }

        // Creation of the tree with horizontal trunk
        std::vector<RsttRoutingEstimatorModel::Segment> horizontalTree =
            returnHorizontalSteinerTree(Pins, TrunkH);
        orderPoints(Pins, ASC, Y);
        med = returnMidPoint(Pins, X);
        correctTrunk(TrunkH, horizontalTree, Pins, med, HORIZONTAL);
        removeLoopSegments(horizontalTree);
        wireLengthH = returnWireLength(horizontalTree);
        //----------------------------------------------------------------------

        // Creation of the tree with vertical trunk
        std::vector<RsttRoutingEstimatorModel::Segment> verticalTree =
            returnVerticalSteinerTree(Pins, TrunkV);
        orderPoints(Pins, ASC, X);
        med = returnMidPoint(Pins, Y);
        correctTrunk(TrunkV, verticalTree, Pins, med, VERTICAL);
        removeLoopSegments(verticalTree);
        wireLengthV = returnWireLength(verticalTree);
        //----------------------------------------------------------------------

        if (wireLengthV <= wireLengthH) {
                createTopology(verticalTree, Points, RsynPins, topology);
                return wireLengthV;
        } else {
                createTopology(horizontalTree, Points, RsynPins, topology);
                return wireLengthH;
        }

}  // end method

const Rsyn::RoutingTopologyDescriptor<int> &RsttRoutingEstimatorModel::getTree(
    Net net) const {
        return clsNetsTopologys[net];
}

//------------------------------------------------------------------------------

RsttRoutingEstimatorModel::Point RsttRoutingEstimatorModel::returnMidPoint(
    std::vector<RsttRoutingEstimatorModel::Point> P, bool XorY) {
        int i, j, k, l;
        int size;
        int mid;
        RsttRoutingEstimatorModel::Point med1, med2;

        orderPoints(P, ASC, XorY);

        size = P.size();

        if (XorY == Y) {
                mid = (P.back().y() + P.front().y()) / 2;

                if (P[0].y() < mid) {
                        k = 0;
                        for (i = 0; i < size; i++) {
                                if (P[i].y() <= mid) {
                                        med1 = P[i];
                                        k++;
                                }
                        }
                        l = 0;
                        for (i = (size - 1); i >= 0; i--) {
                                if (P[i].y() >= mid) {
                                        med2 = P[i];
                                        l++;
                                }
                        }

                        if (l == 0) return med1;

                        if (k == 0) return med2;

                        i = mid - med1.y();
                        j = med2.y() - mid;

                        if (i <= j) {
                                return med1;
                        } else {
                                return med2;
                        }
                } else {
                        return P[0];
                }
        } else {
                mid = (P.back().x() + P.front().x()) / 2;

                if (P[0].x() < mid) {
                        k = 0;
                        for (i = 0; i < size; i++) {
                                if (P[i].x() <= mid) {
                                        med1 = P[i];
                                        k++;
                                }
                        }
                        l = 0;
                        for (i = (size - 1); i >= 0; i--) {
                                if (P[i].x() >= mid) {
                                        med2 = P[i];
                                        l++;
                                }
                        }

                        if (l == 0) return med1;

                        if (k == 0) return med2;

                        i = mid - med1.x();
                        j = med2.x() - mid;

                        if (i <= j) {
                                return med1;
                        } else {
                                return med2;
                        }
                } else {
                        return P[0];
                }
        }
}  // end method

//------------------------------------------------------------------------------

RsttRoutingEstimatorModel::Point RsttRoutingEstimatorModel::returnMinPoint(
    std::vector<RsttRoutingEstimatorModel::Point> const
        &P) {  // the vector is ordered in ascending way, according the
               // coordinate necessary
        return P[0];
}  // end method

//------------------------------------------------------------------------------

RsttRoutingEstimatorModel::Point RsttRoutingEstimatorModel::returnMaxPoint(
    std::vector<RsttRoutingEstimatorModel::Point> const
        &P) {  // the vector is ordered in ascending way, according the
               // coordinate necessary
        int size = P.size();
        return P[size - 1];
}  // end method

//------------------------------------------------------------------------------

void RsttRoutingEstimatorModel::orderPoints(
    std::vector<RsttRoutingEstimatorModel::Point> &myvector, bool AscOrDesc,
    bool XorY) {
        auto orderAscX = [](
            const RsttRoutingEstimatorModel::Point &point1,
            const RsttRoutingEstimatorModel::Point &point2) -> bool {
                if (point1.x() != point2.x())
                        return point1.x() < point2.x();
                else
                        return point1.y() < point2.y();
        };

        auto orderAscY = [](
            const RsttRoutingEstimatorModel::Point &point1,
            const RsttRoutingEstimatorModel::Point &point2) -> bool {
                if (point1.y() != point2.y()) {
                        return point1.y() < point2.y();
                } else {
                        return point1.x() < point2.x();
                }
        };

        auto orderDescX = [](
            const RsttRoutingEstimatorModel::Point &point1,
            const RsttRoutingEstimatorModel::Point &point2) -> bool {
                if (point1.x() != point2.x()) {
                        return point1.x() > point2.x();
                } else {
                        return point1.y() > point2.y();
                }
        };

        auto orderDescY = [](
            const RsttRoutingEstimatorModel::Point &point1,
            const RsttRoutingEstimatorModel::Point &point2) -> bool {
                if (point1.y() != point2.y()) {
                        return point1.y() > point2.y();
                } else {
                        return point1.x() > point2.x();
                }
        };
        // Lambda functions used to sort the vector

        if (AscOrDesc == ASC) {
                if (XorY == X) {
                        std::sort(myvector.begin(), myvector.end(), orderAscX);
                } else {
                        std::sort(myvector.begin(), myvector.end(), orderAscY);
                }  // end if
        } else {
                if (XorY == X) {
                        std::sort(myvector.begin(), myvector.end(), orderDescX);
                } else {
                        std::sort(myvector.begin(), myvector.end(), orderDescY);
                }  // end if
        }          // end if
}  // end method

//------------------------------------------------------------------------------

void RsttRoutingEstimatorModel::orderSegments(
    std::vector<RsttRoutingEstimatorModel::Segment> &stems, bool XorY) {
        auto orderSegmentX = [](
            const RsttRoutingEstimatorModel::Segment a,
            const RsttRoutingEstimatorModel::Segment b) -> bool {
                if (a.first.x() != b.first.x())
                        return a.first.x() < b.first.x();
                else if (a.second.x() != b.second.x())
                        return a.first.x() < b.first.x();
                else if (a.first.y() != b.first.y())
                        return a.first.y() < b.first.y();
                else
                        return a.second.y() < b.second.y();
        };

        auto orderSegmentY = [](
            const RsttRoutingEstimatorModel::Segment a,
            const RsttRoutingEstimatorModel::Segment b) -> bool {
                if (a.first.y() != b.first.y())
                        return a.first.y() < b.first.y();
                else if (a.second.y() != b.second.y())
                        return a.second.y() < b.second.y();
                else if (a.first.x() != b.first.x())
                        return a.first.x() < b.first.x();
                else
                        return a.first.x() < b.first.x();
        };

        if (XorY == X) {
                std::sort(stems.begin(), stems.end(), orderSegmentX);
        } else {
                std::sort(stems.begin(), stems.end(), orderSegmentY);
        }  // end if
}  // end method

//------------------------------------------------------------------------------

std::vector<RsttRoutingEstimatorModel::Point>
RsttRoutingEstimatorModel::returnUpperOrLower(
    std::vector<RsttRoutingEstimatorModel::Point> const &P, int Ymid,
    bool UorL) {
        // Return a set of points whose are under Ymid(1) or above Ymid(0)
        int size = P.size();
        int i;

        std::vector<RsttRoutingEstimatorModel::Point> myvector;

        if (UorL == UP) {
                for (i = 0; i < size; i++) {
                        if (P[i].y() > Ymid) {
                                myvector.push_back(P[i]);
                        }  // end if
                }          // end for
        } else {
                for (i = 0; i < size; i++) {
                        if (P[i].y() < Ymid) {
                                myvector.push_back(P[i]);
                        }  // end if
                }          // end for
        }                  // end if
        return myvector;
}  // end method

//------------------------------------------------------------------------------

std::vector<RsttRoutingEstimatorModel::Point>
RsttRoutingEstimatorModel::returnLeftOrRight(
    std::vector<RsttRoutingEstimatorModel::Point> const &P, int Xmid,
    bool LorR) {
        // Return a set of points whose are at left(0) or at right of Xmid(1)
        int size = P.size();
        int i;

        std::vector<RsttRoutingEstimatorModel::Point> myvector;

        if (LorR == LEFT) {
                for (i = 0; i < size; i++) {
                        if (P[i].x() < Xmid) {
                                myvector.push_back(P[i]);
                        }  // end if
                }          // end for
        } else {
                for (i = 0; i < size; i++) {
                        if (P[i].x() > Xmid) {
                                myvector.push_back(P[i]);
                        }  // end if
                }          // end for
        }                  // end if

        return myvector;
}  // end method

//------------------------------------------------------------------------------

void RsttRoutingEstimatorModel::connectPinV(
    RsttRoutingEstimatorModel::Point Pin,
    std::vector<RsttRoutingEstimatorModel::Segment> &trunk,
    std::vector<RsttRoutingEstimatorModel::Segment> &stems,
    std::vector<RsttRoutingEstimatorModel::Segment> &SegIni, int firstCon,
    bool UorL) {
        // Connect a pin to the trunk or to a stem. firstCon is a flag that
        // indicates when are be connected the first pin of a side
        int i, trunkIndex, stemIndex;
        for (i = 0; i < trunk.size(); i++) {
                if (Pin.x() >= trunk[i].first.x() &&
                    Pin.x() <= trunk[i].second.x()) {
                        trunkIndex = i;
                        break;
                }  // end if
        }          // end for

        RsttRoutingEstimatorModel::Segment trunkSegment = trunk[trunkIndex];

        double trunkDistance = boost::geometry::distance(Pin, trunkSegment);
        double stemDistance;
        RsttRoutingEstimatorModel::Segment stem;
        std::vector<RsttRoutingEstimatorModel::Segment> stemIni;
        if (firstCon == 0 || stems.size() == 0) {
                // std::cout << "Using the SegIni in conncet vertical pin\n";
                stemIni = SegIni;
                for (i = 0; i < stemIni.size(); i++) {
                        if ((Pin.y() <= stemIni[i].first.y() &&
                             Pin.y() >= stemIni[i].second.y() && UorL == UP) ||
                            (Pin.y() >= stemIni[i].first.y() &&
                             Pin.y() <= stemIni[i].second.y() && UorL == LOW)) {
                                stemIndex = i;
                                break;
                        } else if ((Pin.y() > stemIni[0].first.y() &&
                                    UorL == UP) ||
                                   (Pin.y() < stemIni[0].first.y() &&
                                    UorL == LOW)) {
                                stemIndex = 0;
                                break;
                        }
                }  // end for
                stem = stemIni[stemIndex];
        } else {
                stem = stems.back();
        }

        RsttRoutingEstimatorModel::Point stemB;  // The first point of the stem
        RsttRoutingEstimatorModel::Point stemE;
        if (UorL == UP) {
                if (stem.first.y() < stem.second.y()) {
                        stemB = stem.second;  // stemB will be always the point
                                              // that are not in a segment
                        stemE = stem.first;
                } else {
                        stemB = stem.first;
                        stemE = stem.second;
                }  // end if
        } else {
                if (stem.first.y() < stem.second.y()) {
                        stemB = stem.first;
                        stemE = stem.second;
                } else {
                        stemB = stem.second;
                        stemE = stem.first;
                }  // end if
        }          // end if

        RsttRoutingEstimatorModel::Point trunkP =
            trunkSegment.first;  // The first point of the trunk
        RsttRoutingEstimatorModel::Point stemConnection(stemB.x(), Pin.y());

        if ((stem.first.y() == stem.second.y())) {
                stemB = stem.first;
                stemE = stem.second;

                if (Pin.y() == stemB.y()) {
                        stemDistance = boost::geometry::distance(Pin, stem);
                } else {
                        RsttRoutingEstimatorModel::Point aux(Pin.x(),
                                                             stemB.y());
                        stemDistance = boost::geometry::distance(Pin, aux) +
                                       boost::geometry::distance(aux, stem);
                }  // end if

                if (trunkDistance <= stemDistance) {
                        RsttRoutingEstimatorModel::Point trunkCon(Pin.x(),
                                                                  trunkP.y());
                        RsttRoutingEstimatorModel::Segment con(Pin, trunkCon);
                        RsttRoutingEstimatorModel::Segment newTrunk1(
                            trunkSegment.first, trunkCon);
                        RsttRoutingEstimatorModel::Segment newTrunk2(
                            trunkCon, trunkSegment.second);
                        trunk.erase(trunk.begin() + trunkIndex);
                        trunk.push_back(newTrunk1);
                        trunk.push_back(newTrunk2);
                        if (Pin.x() != trunkCon.x() ||
                            Pin.y() != trunkCon.y())   // If the pin is not a
                                                       // point of the trunk
                                stems.push_back(con);  // The connections are
                                                       // saved in a vector of
                                                       // Segments
                } else {
                        if (Pin.y() == stemB.y()) {
                                RsttRoutingEstimatorModel::Point stemConnection(
                                    stemB.x(), Pin.y());
                                RsttRoutingEstimatorModel::Segment con(
                                    Pin, stemConnection);
                                stems.push_back(con);  // The connections are
                                                       // saved in a vector of
                                                       // Segments
                        } else {
                                RsttRoutingEstimatorModel::Point aux(
                                    Pin.x(), stemB.y());  // This point is
                                                          // aligned to the pin
                                                          // and to the
                                                          // beginning of the
                                                          // stem
                                RsttRoutingEstimatorModel::Segment conAux(
                                    aux, stemB);  // This segment is the
                                                  // connection of the stem to
                                                  // the segment started from
                                                  // the pin
                                stems.push_back(
                                    conAux);  // First is saved the aux
                                RsttRoutingEstimatorModel::Segment con(
                                    Pin, aux);  // Here is the segment that
                                                // would be the next stem, that
                                                // beginning from the pin
                                stems.push_back(con);  // This segment is saved
                                                       // at last, because this
                                                       // will be the next stem
                                                       // to be compared
                        }                              // end if
                }                                      // end if
        } else {
                if (((Pin.y() <= stemB.y() && Pin.y() >= stemE.y()) &&
                     UorL == UP) ||
                    ((Pin.y() >= stemB.y() && Pin.y() <= stemE.y()) &&
                     UorL == LOW)) {  // If the pin are at the same height with
                                      // the stem, the distance between them are
                                      // simple:
                        stemDistance = boost::geometry::distance(Pin, stem);
                } else if ((Pin.y() > stemB.y() && UorL == UP) ||
                           (Pin.y() < stemB.y() &&
                            UorL == UP)) {  // If the pin are above the stem
                                            // (for the pins in Upper) or under
                                            // the stem (for the pins in Lower),
                                            // the distance between them are
                                            // calculated by:
                        RsttRoutingEstimatorModel::Point aux(
                            Pin.x(), stemB.y());  // this aux point is used to
                                                  // connect the pin to the stem
                                                  // when they aren't in the
                                                  // same height
                        stemDistance =
                            boost::geometry::distance(Pin, aux) +
                            boost::geometry::distance(
                                aux, stemB);  // the distance between the pin to
                                              // a aux point and the distance
                                              // between the aux point to the
                                              // stem
                } else {
                        RsttRoutingEstimatorModel::Point aux(
                            Pin.x(), stemE.y());  // this aux point is used to
                                                  // connect the pin to the stem
                                                  // when they aren't in the
                                                  // same height
                        stemDistance =
                            boost::geometry::distance(Pin, aux) +
                            boost::geometry::distance(
                                aux, stemE);  // the distance between the pin to
                                              // a aux point and the distance
                                              // between the aux point to the
                                              // stem
                }                             // end if

                if (trunkDistance <= stemDistance) {
                        RsttRoutingEstimatorModel::Point trunkCon(Pin.x(),
                                                                  trunkP.y());
                        RsttRoutingEstimatorModel::Segment con(Pin, trunkCon);
                        RsttRoutingEstimatorModel::Segment newTrunk1(
                            trunkSegment.first, trunkCon);
                        RsttRoutingEstimatorModel::Segment newTrunk2(
                            trunkCon, trunkSegment.second);
                        trunk.erase(trunk.begin() + trunkIndex);
                        trunk.push_back(newTrunk1);
                        trunk.push_back(newTrunk2);
                        if (Pin.x() != trunkCon.x() || Pin.y() != trunkCon.y())
                                stems.push_back(con);  // The connections are
                                                       // saved in a vector of
                                                       // Segments
                } else {
                        if ((Pin.x() == stemB.x() && Pin.y() <= stemB.y() &&
                             UorL == UP) ||
                            (Pin.x() == stemB.x() && Pin.y() >= stemB.y() &&
                             UorL == LOW)) {
                                RsttRoutingEstimatorModel::Point stemConn(
                                    Pin.x(), stemB.y());
                                RsttRoutingEstimatorModel::Segment con(
                                    Pin, stemConn);
                                stems.push_back(con);
                        } else if (((Pin.y() <= stemB.y() &&
                                     Pin.y() >= stemE.y()) &&
                                    UorL == UP) ||
                                   ((Pin.y() >= stemB.y() &&
                                     Pin.y() <= stemE.y()) &&
                                    UorL == LOW)) {
                                if (Pin.x() == stemB.x()) {
                                        RsttRoutingEstimatorModel::Segment
                                            newStem1(stemB, Pin);
                                        RsttRoutingEstimatorModel::Segment
                                            newStem2(Pin, stemE);
                                        if (firstCon == 0 ||
                                            stems.size() == 0) {
                                                SegIni.erase(SegIni.begin() +
                                                             stemIndex);
                                                SegIni.push_back(newStem1);
                                                SegIni.push_back(newStem2);
                                        } else {
                                                stems.erase(
                                                    stems.begin() +
                                                    findSegment(stems, stem));
                                                stems.push_back(newStem1);
                                                stems.push_back(newStem2);
                                        }
                                } else {
                                        RsttRoutingEstimatorModel::Segment con(
                                            Pin, stemConnection);
                                        RsttRoutingEstimatorModel::Segment
                                            newStem1(stemB, stemConnection);
                                        RsttRoutingEstimatorModel::Segment
                                            newStem2(stemConnection, stemE);
                                        if (firstCon == 0 ||
                                            stems.size() == 0) {
                                                SegIni.erase(SegIni.begin() +
                                                             stemIndex);
                                                SegIni.push_back(newStem1);
                                                SegIni.push_back(newStem2);
                                        } else {
                                                stems.erase(
                                                    stems.begin() +
                                                    findSegment(stems, stem));
                                                stems.push_back(newStem1);
                                                stems.push_back(newStem2);
                                        }
                                        stems.push_back(con);  // The
                                                               // connections
                                                               // are saved in a
                                                               // vector of
                                                               // Segments
                                }                              // end if
                        } else if ((Pin.y() > stemB.y() && UorL == UP) ||
                                   (Pin.y() < stemB.y() && UorL == LOW)) {
                                RsttRoutingEstimatorModel::Point aux(
                                    Pin.x(), stemB.y());  // This point is
                                                          // aligned to the pin
                                                          // and to the
                                                          // beginning of the
                                                          // stem
                                RsttRoutingEstimatorModel::Segment conAux(
                                    aux, stemB);  // This segment is the
                                                  // connection of the stem to
                                                  // the segment started from
                                                  // the pin
                                stems.push_back(
                                    conAux);  // First is saved the aux
                                RsttRoutingEstimatorModel::Segment con(
                                    Pin, aux);  // Here is the segment that
                                                // would be the next stem, that
                                                // beginning from the pin
                                // std::cout << "Segment conAux:\n" <<
                                // conAux.first.x() << ", " << conAux.first.y()
                                // << "\n" << conAux.second.x() << ", " <<
                                // conAux.second.y() << "\n";
                                // std::cout << "Segment con:\n" <<
                                // con.first.x() << ", " << con.first.y() <<
                                // "\n" << con.second.x() << ", " <<
                                // con.second.y() << "\n";
                                stems.push_back(con);  // This segment is saved
                                                       // at last, because this
                                                       // will be the next stem
                                                       // to be compared
                        } else if ((Pin.y() < stemE.y() && UorL == UP) ||
                                   (Pin.y() > stemE.y() && UorL == LOW)) {
                                RsttRoutingEstimatorModel::Point aux(Pin.x(),
                                                                     stemE.y());
                                RsttRoutingEstimatorModel::Segment conAux(
                                    aux, stemE);
                                stems.push_back(conAux);
                                RsttRoutingEstimatorModel::Segment con(Pin,
                                                                       aux);
                                stems.push_back(con);
                        }  // end if
                }          // end if
        }                  // end if
}  // end method

//------------------------------------------------------------------------------

void RsttRoutingEstimatorModel::connectPinH(
    RsttRoutingEstimatorModel::Point Pin,
    std::vector<RsttRoutingEstimatorModel::Segment> &trunk,
    std::vector<RsttRoutingEstimatorModel::Segment> &stems,
    std::vector<RsttRoutingEstimatorModel::Segment> &SegIni, int firstCon,
    bool LorR) {
        int i, trunkIndex, stemIndex;
        for (i = 0; i < trunk.size(); i++) {
                if (Pin.y() >= trunk[i].first.y() &&
                    Pin.y() <= trunk[i].second.y()) {
                        trunkIndex = i;
                        break;
                }  // end if
        }          // end for

        RsttRoutingEstimatorModel::Segment trunkSegment = trunk[trunkIndex];

        double trunkDistance = boost::geometry::distance(Pin, trunkSegment);
        double stemDistance;
        RsttRoutingEstimatorModel::Segment stem;
        std::vector<RsttRoutingEstimatorModel::Segment> stemIni;
        if (firstCon == 0 || stems.size() == 0) {
                stemIni = SegIni;
                for (i = 0; i < stemIni.size(); i++) {
                        if ((Pin.x() >= stemIni[i].first.x() &&
                             Pin.x() <= stemIni[i].second.x() &&
                             LorR == LEFT) ||
                            (Pin.x() <= stemIni[i].first.x() &&
                             Pin.x() >= stemIni[i].second.x() &&
                             LorR == RIGHT)) {
                                stemIndex = i;
                                break;
                        } else if ((Pin.x() < stemIni[0].first.x() &&
                                    LorR == LEFT) ||
                                   (Pin.x() > stemIni[0].first.x() &&
                                    LorR == RIGHT)) {
                                stemIndex = 0;
                                break;
                        }  // end if
                }          // end for
                stem = stemIni[stemIndex];
        } else {
                stem = stems[stems.size() - 1];
        }  // end if

        RsttRoutingEstimatorModel::Point stemB;
        RsttRoutingEstimatorModel::Point stemE;

        if (LorR == LEFT) {
                if (stem.first.x() < stem.second.x()) {
                        stemB = stem.first;
                        stemE = stem.second;
                } else {
                        stemB = stem.second;
                        stemE = stem.first;
                }  // end if
        } else {
                if (stem.first.x() < stem.second.x()) {
                        stemB = stem.second;
                        stemE = stem.first;
                } else {
                        stemB = stem.first;
                        stemE = stem.second;
                }  // end if
        }          // end if

        RsttRoutingEstimatorModel::Point trunkP =
            trunkSegment.first;  // The first point of the trunk
        RsttRoutingEstimatorModel::Point stemConnection(Pin.x(), stemB.y());

        if ((stem.first.x() == stem.second.x())) {
                stemB = stem.first;
                stemE = stem.second;

                if (Pin.x() == stemB.x()) {
                        stemDistance = boost::geometry::distance(Pin, stem);
                } else {
                        RsttRoutingEstimatorModel::Point aux(stemB.x(),
                                                             Pin.y());
                        stemDistance = boost::geometry::distance(Pin, aux) +
                                       boost::geometry::distance(aux, stemB);
                }  // end if
                if (trunkDistance <= stemDistance) {
                        RsttRoutingEstimatorModel::Point trunkCon(trunkP.x(),
                                                                  Pin.y());
                        RsttRoutingEstimatorModel::Segment con(Pin, trunkCon);
                        RsttRoutingEstimatorModel::Segment newTrunk1(
                            trunkSegment.first, trunkCon);
                        RsttRoutingEstimatorModel::Segment newTrunk2(
                            trunkCon, trunkSegment.second);
                        trunk.erase(trunk.begin() + trunkIndex);
                        trunk.push_back(newTrunk1);
                        trunk.push_back(newTrunk2);
                        if (Pin.x() != trunkCon.x() || Pin.y() == trunkCon.y())
                                stems.push_back(con);  // The connections are
                                                       // saved in a vector of
                                                       // Segments
                } else {
                        if (Pin.x() == stemB.x()) {
                                RsttRoutingEstimatorModel::Segment con(
                                    Pin, stemConnection);
                                stems.push_back(con);  // The connections are
                                                       // saved in a vector of
                                                       // Segments
                        } else {
                                RsttRoutingEstimatorModel::Point aux(
                                    stemB.x(), Pin.y());  // This point is
                                                          // aligned to the pin
                                                          // and to the
                                                          // beginning of the
                                                          // stem
                                RsttRoutingEstimatorModel::Segment conAux(
                                    aux, stemB);  // This segment is the
                                                  // connection of the stem to
                                                  // the segment started from
                                                  // the pin
                                stems.push_back(
                                    conAux);  // First is saved the aux
                                RsttRoutingEstimatorModel::Segment con(
                                    Pin, aux);  // Here is the segment that
                                                // would be the next stem, that
                                                // beginning from the pin
                                stems.push_back(con);  // This segment is saved
                                                       // at last, because this
                                                       // will be the next stem
                                                       // to be compared
                        }                              // end if
                }                                      // end if
        } else {
                if (((Pin.x() >= stemB.x() && Pin.x() <= stemE.x()) &&
                     LorR == LEFT) ||
                    ((Pin.x() <= stemB.x() && Pin.x() >= stemE.x()) &&
                     LorR == RIGHT)) {  // If the pin are at the same height
                                        // with the stem, the distance between
                                        // them are simple:
                        stemDistance = boost::geometry::distance(Pin, stem);
                } else if ((Pin.x() < stemB.x() && LorR == LEFT) ||
                           (Pin.x() > stemB.y() &&
                            LorR == RIGHT)) {  // If the pin are above the stem
                                               // (for the pins in Upper) or
                                               // under the stem (for the pins
                                               // in Lower), the distance
                                               // between them are calculated
                                               // by:
                        RsttRoutingEstimatorModel::Point aux(
                            Pin.x(), stemB.y());  // this aux point is used to
                                                  // connect the pin to the stem
                                                  // when they aren't in the
                                                  // same height
                        stemDistance =
                            boost::geometry::distance(Pin, aux) +
                            boost::geometry::distance(
                                aux, stemB);  // the distance between the pin to
                                              // a aux point and the distance
                                              // between the aux point to the
                                              // stem
                } else {
                        RsttRoutingEstimatorModel::Point aux(
                            Pin.x(), stemE.y());  // this aux point is used to
                                                  // connect the pin to the stem
                                                  // when they aren't in the
                                                  // same height
                        stemDistance =
                            boost::geometry::distance(Pin, aux) +
                            boost::geometry::distance(
                                aux, stemE);  // the distance between the pin to
                                              // a aux point and the distance
                                              // between the aux point to the
                                              // stem
                }                             // end if
                if (trunkDistance <= stemDistance) {
                        RsttRoutingEstimatorModel::Point trunkCon(trunkP.x(),
                                                                  Pin.y());
                        RsttRoutingEstimatorModel::Segment con(Pin, trunkCon);
                        RsttRoutingEstimatorModel::Segment newTrunk1(
                            trunkSegment.first, trunkCon);
                        RsttRoutingEstimatorModel::Segment newTrunk2(
                            trunkCon, trunkSegment.second);
                        trunk.erase(trunk.begin() + trunkIndex);
                        trunk.push_back(newTrunk1);
                        trunk.push_back(newTrunk2);
                        if (Pin.x() != trunkCon.x() ||
                            Pin.y() != trunkCon.y()) {
                                stems.push_back(con);  // The connections are
                                                       // saved in a vector of
                                                       // Segments
                        }
                } else {
                        if ((Pin.y() == stemB.y() && Pin.x() >= stemB.x() &&
                             LorR == LEFT) ||
                            (Pin.y() == stemB.y() && Pin.x() <= stemB.x() &&
                             LorR == RIGHT)) {
                                RsttRoutingEstimatorModel::Point stemConn(
                                    stemB.x(), Pin.y());
                                RsttRoutingEstimatorModel::Segment con(
                                    Pin, stemConn);
                                stems.push_back(con);
                        } else if (((Pin.x() >= stemB.x() &&
                                     Pin.x() <= stemE.x()) &&
                                    LorR == LEFT) ||
                                   ((Pin.x() <= stemB.x() &&
                                     Pin.x() >= stemE.x()) &&
                                    LorR == RIGHT)) {
                                if (Pin.y() == stemB.y()) {
                                        RsttRoutingEstimatorModel::Segment
                                            newStem1(stemB, Pin);
                                        RsttRoutingEstimatorModel::Segment
                                            newStem2(Pin, stemE);
                                        if (firstCon == 0 ||
                                            stems.size() == 0) {
                                                SegIni.erase(SegIni.begin() +
                                                             stemIndex);
                                                SegIni.push_back(newStem1);
                                                SegIni.push_back(newStem2);
                                        } else {
                                                stems.erase(
                                                    stems.begin() +
                                                    findSegment(stems, stem));
                                                stems.push_back(newStem1);
                                                stems.push_back(newStem2);
                                        }
                                } else {
                                        RsttRoutingEstimatorModel::Segment con(
                                            Pin, stemConnection);
                                        RsttRoutingEstimatorModel::Segment
                                            newStem1(stemB, stemConnection);
                                        RsttRoutingEstimatorModel::Segment
                                            newStem2(stemConnection, stemE);
                                        if (firstCon == 0 ||
                                            stems.size() == 0) {
                                                SegIni.erase(SegIni.begin() +
                                                             stemIndex);
                                                SegIni.push_back(newStem1);
                                                SegIni.push_back(newStem2);
                                        } else {
                                                int stemI =
                                                    findSegment(stems, stem);
                                                stems.erase(stems.begin() +
                                                            stemI);
                                                stems.push_back(newStem1);
                                                stems.push_back(newStem2);
                                        }
                                        stems.push_back(con);  // The
                                                               // connections
                                                               // are saved in a
                                                               // vector of
                                                               // Segments
                                }                              // end if
                        } else if ((Pin.x() < stemB.x() && LorR == LEFT) ||
                                   (Pin.x() > stemB.x() && LorR == RIGHT)) {
                                RsttRoutingEstimatorModel::Point aux(
                                    stemB.x(), Pin.y());  // This point is
                                                          // aligned to the pin
                                                          // and to the
                                                          // beginning of the
                                                          // stem
                                RsttRoutingEstimatorModel::Segment conAux(
                                    aux, stemB);  // This segment is the
                                                  // connection of the stem to
                                                  // the segment started from
                                                  // the pin
                                stems.push_back(
                                    conAux);  // First is saved the aux
                                RsttRoutingEstimatorModel::Segment con(
                                    Pin, aux);  // Here is the segment that
                                                // would be the next stem, that
                                                // beginning from the pin
                                stems.push_back(con);  // This segment is saved
                                                       // at last, because this
                                                       // will be the next stem
                                                       // to be compared
                        } else if (((Pin.x() > stemB.x() &&
                                     Pin.x() > stemE.x()) &&
                                    LorR == LEFT) ||
                                   ((Pin.x() < stemB.x() &&
                                     Pin.x() < stemE.x()) &&
                                    LorR == RIGHT)) {
                                RsttRoutingEstimatorModel::Point aux(stemE.x(),
                                                                     Pin.y());
                                RsttRoutingEstimatorModel::Segment conAux(
                                    aux, stemE);
                                stems.push_back(conAux);
                                RsttRoutingEstimatorModel::Segment con(Pin,
                                                                       aux);
                                stems.push_back(con);
                        }  // end if
                }          // end if
        }                  // end if
}  // end method

//------------------------------------------------------------------------------

DBU RsttRoutingEstimatorModel::returnWireLength(
    std::vector<RsttRoutingEstimatorModel::Segment> const &stems) {
        int size = stems.size();
        DBU wireLength = 0;
        int i;

        for (i = 0; i < size; i++) {
                wireLength += (DBU)boost::geometry::distance(
                    stems[i].first,
                    stems[i].second);  // Using a cast to convert double to DBU
                                       // static_cast < DBU >
                // (boost::geometry::distance(stems[i].first, stems[i].second));
        }  // end for

        return wireLength;
}  // end method

//------------------------------------------------------------------------------

std::vector<RsttRoutingEstimatorModel::Segment>
RsttRoutingEstimatorModel::returnHorizontalSteinerTree(
    std::vector<RsttRoutingEstimatorModel::Point> Pins,
    std::vector<RsttRoutingEstimatorModel::Segment> &Trunk) {
        int i, j, size;
        int Ymed, Xmin, Xmax;  // Used into part A of algorithm
        RsttRoutingEstimatorModel::Point Pini;
        std::vector<RsttRoutingEstimatorModel::Segment> stems;
        std::vector<RsttRoutingEstimatorModel::Point> U, L;
        std::vector<RsttRoutingEstimatorModel::Segment> stemIniU;
        std::vector<RsttRoutingEstimatorModel::Segment> stemIniL;

        orderPoints(Pins, ASC,
                    Y);  // Order pins by Y coordinate to find the median Y

        // Set the median Y, the less X the greater X of the pins in the net
        Ymed = returnMidPoint(Pins, Y).y();

        orderPoints(
            Pins, ASC,
            X);  // Order pins by X coordinate to find the minimum and maximum X

        Xmin = returnMinPoint(Pins).x();

        Xmax = returnMaxPoint(Pins).x();

        // Create two points of the trunk, the beginning and the ending points
        RsttRoutingEstimatorModel::Point trunkB(Xmin, Ymed);

        RsttRoutingEstimatorModel::Point trunkE(Xmax, Ymed);

        // Create the trunk, that is, a segment between the previous two points
        RsttRoutingEstimatorModel::Segment TrunkIni(trunkB, trunkE);

        Trunk.push_back(TrunkIni);

        // Separate the pins in two vectors, where one is with the pins above
        // the trunk and the other is with the under
        U = returnUpperOrLower(Pins, Ymed, UP);

        L = returnUpperOrLower(Pins, Ymed, LOW);

        // Order the points by descending order, by the X coordinate
        orderPoints(U, DESC, X);
        orderPoints(L, DESC, X);

        // ----------------------------------Connection of the pins in
        // U-------------------------------------//
        // Set Pini as the point closer to the mid of the trunk (Pini E U)
        if (U.size() != 0) {
                Pini = returnMidPoint(U, X);

                // Set a point in the trunk to do the first connection
                RsttRoutingEstimatorModel::Point trunkConU(Pini.x(),
                                                           trunkB.y());

                RsttRoutingEstimatorModel::Segment conU(Pini, trunkConU);
                stemIniU.push_back(conU);

                // Save the connection as the first in the vector
                // stems.push_back(conU);

                // Connection of the pins that are in U and to the left of Pini
                size = U.size();

                j = 0;  // j indicates the first connection of the pins in the
                // vectors U, L
                for (i = 0; i < size; i++) {
                        if (U[i].x() <= Pini.x() &&
                            (U[i].x() != Pini.x() || U[i].y() != Pini.y())) {
                                connectPinV(U[i], Trunk, stems, stemIniU, j,
                                            UP);
                                j++;
                        }  // end if
                }          // end for

                // And reorder U to ascending
                orderPoints(U, ASC, X);

                j = 0;
                for (i = 0; i < size; i++) {
                        if (U[i].x() > Pini.x()) {
                                connectPinV(U[i], Trunk, stems, stemIniU, j,
                                            UP);  // For the first pin connected
                                                  // here, is used the first
                                                  // stem saved (that begin with
                                                  // Pini)
                                j++;
                        }  // end if
                }          // end for
        }                  // end if

        //-----------------------------------------Connection of the pins in
        // L------------------------------------//
        // Set Pini as the point closer to the mid of the trunk (Pini E L)
        if (L.size() != 0) {
                Pini = returnMidPoint(L, X);

                // Set a point in the trunk to do the first connection
                RsttRoutingEstimatorModel::Point trunkConL(Pini.x(),
                                                           trunkB.y());

                RsttRoutingEstimatorModel::Segment conL(Pini, trunkConL);
                stemIniL.push_back(conL);

                // Save the connection as the first in the vector
                // stems.push_back(conL);

                // Connection of the pins that are in L and to the left of Pini
                size = L.size();

                j = 0;
                for (i = 0; i < size; i++) {
                        if (L[i].x() <= Pini.x() &&
                            (L[i].x() != Pini.x() || L[i].y() != Pini.y())) {
                                connectPinV(L[i], Trunk, stems, stemIniL, j,
                                            LOW);
                                j++;
                        }  // end if
                }          // end for

                // And reorder L to ascending
                orderPoints(L, ASC, X);

                j = 0;
                for (i = 0; i < size; i++) {
                        if (L[i].x() > Pini.x()) {
                                connectPinV(L[i], Trunk, stems, stemIniL, j,
                                            LOW);  // For the first pin
                                                   // connected here, is used
                                                   // the first stem saved (that
                                                   // begin with Pini)
                                j++;
                        }  // end if
                }          // end for
        }                  // end if

        for (int k = 0; k < stems.size(); k++) {
                if (stems[k].first.x() == stems[k].second.x() &&
                    stems[k].first.y() == stems[k].second.y()) {
                        stems.erase(stems.begin() + k);
                }
        }

        for (int k = 0; k < stemIniU.size(); k++) {
                stems.push_back(stemIniU[k]);
        }

        for (int k = 0; k < stemIniL.size(); k++) {
                stems.push_back(stemIniL[k]);
        }

        return stems;
}  // end method

//------------------------------------------------------------------------------

std::vector<RsttRoutingEstimatorModel::Segment>
RsttRoutingEstimatorModel::returnVerticalSteinerTree(
    std::vector<RsttRoutingEstimatorModel::Point> Pins,
    std::vector<RsttRoutingEstimatorModel::Segment> &Trunk) {
        int i, j, size;
        int Xmed, Ymin, Ymax;  // Used into part A of algorithm
        RsttRoutingEstimatorModel::Point Pini;
        std::vector<RsttRoutingEstimatorModel::Segment> stems;
        std::vector<RsttRoutingEstimatorModel::Point> L, R;
        std::vector<RsttRoutingEstimatorModel::Segment> stemIniL;
        std::vector<RsttRoutingEstimatorModel::Segment> stemIniR;

        orderPoints(Pins, ASC, X);

        // Set the median X, the less Y the greater Y of the pins in the net
        Xmed = returnMidPoint(Pins, X).x();

        orderPoints(Pins, ASC, Y);

        Ymin = returnMinPoint(Pins).y();

        Ymax = returnMaxPoint(Pins).y();

        // Create two points of the trunk, the beginning and the ending poins
        RsttRoutingEstimatorModel::Point trunkB(Xmed, Ymin);

        RsttRoutingEstimatorModel::Point trunkE(Xmed, Ymax);

        // Create the trunk, that is, a segment between the previous two poins
        RsttRoutingEstimatorModel::Segment TrunkIni(trunkB, trunkE);

        Trunk.push_back(TrunkIni);

        // std::cout << "Trunk: From (" << trunkB.x() << ", " << trunkB.y() <<
        // ") to (" << trunkE.x() << ", " << trunkE.y() << ") \n";
        // std::exit(0);

        // Separate the pins in two vectors, where one is with the pins above
        // the trunk and the other is with the under
        L = returnLeftOrRight(Pins, Xmed, LEFT);

        R = returnLeftOrRight(Pins, Xmed, RIGHT);

        // Order the points in descending way, by the Y coordinate
        orderPoints(L, DESC, Y);
        orderPoints(R, DESC, Y);

        // ----------------------------------Connection of the pins in
        // L-------------------------------------//
        // Set Pini as the point closer to the mid of the trunk (Pini E L)
        if (L.size() != 0) {
                Pini = returnMidPoint(L, Y);
                // Set a point in the trunk to do the first connection
                RsttRoutingEstimatorModel::Point trunkConL(trunkB.x(),
                                                           Pini.y());

                RsttRoutingEstimatorModel::Segment conL(Pini, trunkConL);

                stemIniL.push_back(conL);
                // Connection of the pins that are in L and above of Pini
                size = L.size();

                j = 0;  // j indicates the first connection of the pins in L, R
                for (i = 0; i < size; i++) {
                        if (L[i].y() <= Pini.y() &&
                            (L[i].x() != Pini.x() || L[i].y() != Pini.y())) {
                                connectPinH(L[i], Trunk, stems, stemIniL, j,
                                            LEFT);
                                j++;
                        }  // end if
                }          // end for
                // And reorder L to ascending
                orderPoints(L, ASC, Y);

                j = 0;
                for (i = 0; i < size; i++) {
                        if (L[i].y() > Pini.y()) {
                                connectPinH(L[i], Trunk, stems, stemIniL, j,
                                            LEFT);  // For the first pin
                                                    // connected here, is used
                                                    // the first stem saved
                                                    // (that begin with Pini)
                                j++;
                        }  // end if
                }          // end for
        }                  // end if

        // ----------------------------------Connection of the pins in
        // R-------------------------------------//
        // Set Pini as the point closer to the mid of the trunk (Pini E R)
        if (R.size()) {
                Pini = returnMidPoint(R, Y);
                // std::cout << "Pin Initial: (" << Pini.x() << ", " << Pini.y()
                // << ") \n";
                // std::exit(0);

                // Set a point in the trunk to do the first connection
                RsttRoutingEstimatorModel::Point trunkConR(trunkB.x(),
                                                           Pini.y());

                RsttRoutingEstimatorModel::Segment conR(Pini, trunkConR);
                stemIniR.push_back(conR);

                // Connection of the pins that are in R and above of Pini

                size = R.size();

                j = 0;
                for (i = 0; i < size; i++) {
                        if (R[i].y() <= Pini.y() &&
                            (R[i].x() != Pini.x() || R[i].y() != Pini.y())) {
                                connectPinH(R[i], Trunk, stems, stemIniR, j,
                                            RIGHT);
                                j++;
                        }  // end if
                }          // end for
                // And reorder R to ascending
                orderPoints(R, ASC, Y);

                j = 0;
                for (i = 0; i < size; i++) {
                        if (R[i].y() > Pini.y()) {
                                connectPinH(R[i], Trunk, stems, stemIniR, j,
                                            RIGHT);  // For the first pin
                                                     // connected here, is used
                                                     // the first stem saved
                                                     // (that begin with Pini)
                                j++;
                        }  // end if
                }          // end for
        }                  // end if

        for (int k = 0; k < stems.size(); k++) {
                if (stems[k].first.x() == stems[k].second.x() &&
                    stems[k].first.y() == stems[k].second.y()) {
                        stems.erase(stems.begin() + k);
                }
        }

        for (int k = 0; k < stemIniR.size(); k++) {
                stems.push_back(stemIniR[k]);
        }

        for (int k = 0; k < stemIniL.size(); k++) {
                stems.push_back(stemIniL[k]);
        }

        return stems;
}  // end method

//------------------------------------------------------------------------------

void RsttRoutingEstimatorModel::correctTrunk(
    std::vector<RsttRoutingEstimatorModel::Segment> &trunk,
    std::vector<RsttRoutingEstimatorModel::Segment> &stems,
    std::vector<RsttRoutingEstimatorModel::Point> &Pins,
    RsttRoutingEstimatorModel::Point med, bool HorV) {
        RsttRoutingEstimatorModel::Point trunkIni(trunk[0].first.x(),
                                                  trunk[0].first.y());
        int i;

        orderPoints(Pins, ASC, X);

        orderSegments(stems, HorV);

        orderSegments(trunk, HorV);

        auto compare = [&](const RsttRoutingEstimatorModel::Point &p1,
                           const RsttRoutingEstimatorModel::Point &p2) {
                if (p1.x() != p2.x()) {
                        return p1.x() < p2.x();
                } else {
                        return p1.y() < p2.y();
                }
        };  // Function lambda to order the set of points

        std::vector<RsttRoutingEstimatorModel::Point> points;
        std::set<RsttRoutingEstimatorModel::Point, decltype(compare)> Nodes(
            compare);
        // std::set<RsttRoutingEstimatorModel::Point>::iterator it;

        if (HorV == HORIZONTAL) {
                for (i = 0; i < Pins.size(); i++) {
                        if (Pins[i].y() == trunkIni.y()) {
                                if (Nodes.insert(Pins[i]).second == 1) {
                                        points.push_back(Pins[i]);
                                }  // end if
                        }          // end if
                }                  // end for

                for (i = 0; i < stems.size(); i++) {
                        if (stems[i].first.y() == trunkIni.y()) {
                                if (Nodes.insert(stems[i].first).second == 1) {
                                        points.push_back(stems[i].first);
                                }  // end if
                        }          // end if
                }                  // end for

                for (i = 0; i < stems.size(); i++) {
                        if (stems[i].second.y() == trunkIni.y()) {
                                if (Nodes.insert(stems[i].second).second == 1) {
                                        points.push_back(stems[i].second);
                                }  // end if
                        }          // end if
                }                  // end for

                orderPoints(points, ASC, X);
        } else {
                for (i = 0; i < Pins.size(); i++) {
                        if (Pins[i].x() == trunkIni.x()) {
                                if (Nodes.insert(Pins[i]).second == 1) {
                                        points.push_back(Pins[i]);
                                }  // end if
                        }          // end if
                }                  // end for

                for (i = 0; i < stems.size(); i++) {
                        if (stems[i].first.x() == trunkIni.x()) {
                                if (Nodes.insert(stems[i].first).second == 1) {
                                        points.push_back(stems[i].first);
                                }  // end if
                        }          // end if
                }                  // end for

                for (i = 0; i < stems.size(); i++) {
                        if (stems[i].second.x() == trunkIni.x()) {
                                if (Nodes.insert(stems[i].second).second == 1) {
                                        points.push_back(stems[i].second);
                                }  // end if
                        }          // end if
                }                  // end for

                orderPoints(points, ASC, Y);
        }  // end if

        trunk.clear();
        // it = Nodes.begin();

        for (i = 0; i < (points.size() - 1); i++) {
                RsttRoutingEstimatorModel::Point p1 = points[i];
                RsttRoutingEstimatorModel::Point p2 = points[i + 1];
                RsttRoutingEstimatorModel::Segment segTrunk(p1, p2);
                trunk.push_back(segTrunk);
        }  // end for

        for (i = 0; i < trunk.size(); i++) {
                stems.push_back(trunk[i]);
        }  // end for
}  // end method

//------------------------------------------------------------------------------

int RsttRoutingEstimatorModel::findPoint(
    std::vector<RsttRoutingEstimatorModel::Point> const &points,
    RsttRoutingEstimatorModel::Point const &point) {
        int i = 0;

        for (i = 0; i < points.size(); i++) {
                if (point.x() == points[i].x() && point.y() == points[i].y()) {
                        return i;
                }  // end if
        }
        return -1;
}  // end method

//------------------------------------------------------------------------------

int RsttRoutingEstimatorModel::findSegment(
    const std::vector<RsttRoutingEstimatorModel::Segment> &segments,
    const RsttRoutingEstimatorModel::Segment &segment) {
        int i = 0;

        for (i = 0; i < segments.size(); i++) {
                if (segment.first.x() == segments[i].first.x() &&
                    segment.first.y() == segments[i].first.y() &&
                    segment.second.x() == segments[i].second.x() &&
                    segment.second.y() == segments[i].second.y()) {
                        return i;
                }
        }
        return -1;
}

void RsttRoutingEstimatorModel::removeLoopSegments(std::vector<Segment> &tree) {
        std::vector<Segment> vecAux;

        for (int i = 0; i < tree.size(); i++) {
                if (tree[i].first.x() == tree[i].second.x() &&
                    tree[i].first.y() == tree[i].second.y()) {
                        continue;
                } else {
                        vecAux.push_back(tree[i]);
                }
        }

        tree.swap(vecAux);
}

//------------------------------------------------------------------------------

void RsttRoutingEstimatorModel::createTopology(
    std::vector<RsttRoutingEstimatorModel::Segment> const &tree,
    std::vector<RsttRoutingEstimatorModel::Point> const &Pins,
    std::vector<Pin> const &RsynPins,
    RoutingTopologyDescriptor<int> &topology) {
        std::vector<RsttRoutingEstimatorModel::Point> nodes;  // Vector where
                                                              // the steiner
                                                              // nodes and the
                                                              // pins will be
                                                              // stored

        for (int i = 0; i < Pins.size();
             i++) {  // First, create all the pins and store each one in the
                     // vector "nodes"
                DBUxy node(Pins[i].x(), Pins[i].y());
                nodes.push_back(Pins[i]);
                topology.createNode(i, node, RsynPins[i]);
        }  // end for

        for (int i = 0; i < tree.size();
             i++) {  // Here is created the segments of the tree
                int segmentStartNode;  // Index of the first node of the segment
                int segmentFinalNode;  // Index of the final node of the segment
                int nodeIndex;

                nodeIndex = findPoint(
                    nodes, tree[i].first);  // The function findPoint returns
                                            // the index of the point stored if
                                            // it is stored in "nodes". Else,
                                            // returns -1
                // Create the initial and final nodes of the segment
                if (nodeIndex !=
                    -1) {  // If already exists a node in this position
                        segmentStartNode =
                            nodeIndex;  // Just take the index of that node,
                } else {                // If not exists a node in this position
                        DBUxy node(tree[i].first.x(), tree[i].first.y());
                        nodes.push_back(
                            tree[i]
                                .first);  // Add this node to the vector "nodes"
                        segmentStartNode =
                            (nodes.size() - 1);  // And take the index of this
                                                 // node, that will be the last
                                                 // in the vector
                        topology.createNode(segmentStartNode, node);
                }  // end if

                nodeIndex = findPoint(nodes, tree[i].second);
                // Here is the same, but for the second node of the segment
                if (nodeIndex != -1) {
                        segmentFinalNode = nodeIndex;
                } else {
                        DBUxy node(tree[i].second.x(), tree[i].second.y());
                        nodes.push_back(tree[i].second);
                        segmentFinalNode = (nodes.size() - 1);
                        topology.createNode(segmentFinalNode, node);
                }  // end if

                topology.addSegment(segmentStartNode, segmentFinalNode);
        }  // end for
}  // end method

}  // end namespace
