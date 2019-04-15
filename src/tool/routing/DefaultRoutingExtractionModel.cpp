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

#include "DefaultRoutingExtractionModel.h"

namespace Rsyn {

// -----------------------------------------------------------------------------

const bool DefaultRoutingExtractionModel::ENABLE_LONG_WIRE_SLICING = true;

// -----------------------------------------------------------------------------

void DefaultRoutingExtractionModel::start(const Rsyn::Json &params) {
        Rsyn::Session session;

        design = session.getDesign();
        module = design.getTopModule();

        clsScenario = session.getService("rsyn.scenario");
}  // end method

// -----------------------------------------------------------------------------

void DefaultRoutingExtractionModel::stop() {}  // end method

// -----------------------------------------------------------------------------

void DefaultRoutingExtractionModel::initialize(
    const Number wireResistancePerUnitLength,
    const Number wireCapacitancePerUnitLength, const DBU maxWireSegmentLength) {
        LOCAL_WIRE_RES_PER_UNIT_LENGTH = wireResistancePerUnitLength;
        LOCAL_WIRE_CAP_PER_UNIT_LENGTH = wireCapacitancePerUnitLength;
        MAX_WIRE_SEGMENT_LENGTH = maxWireSegmentLength;
}  // end method

// -----------------------------------------------------------------------------

void DefaultRoutingExtractionModel::extract(
    const Rsyn::RoutingTopologyDescriptor<int> &topology, RCTree &tree) {
        const Number wireCapPerDistanceUnit = getLocalWireCapPerUnitLength();
        const Number wireResPerDistanceUnit = getLocalWireResPerUnitLength();
        const DBU longWirelengthThreshold = MAX_WIRE_SEGMENT_LENGTH;

        // Build RC tree.
        RCTreeDescriptor dscp;
        dscp.applyDefaultNodeTag(RCTreeNodeTag(nullptr, 0, 0));

        int sliceId = 0;  // we use negative indexes for slicing points
        std::deque<SlicingPoint> slicingPoints;

        Rsyn::Pin driver = nullptr;
        int root = -1;

        for (const RoutingTopologyDescriptor<int>::Segment &segment :
             topology.allSegments()) {
                const RoutingTopologyDescriptor<int>::Node &node0 =
                    topology.getNode(segment.propNode0);
                const RoutingTopologyDescriptor<int>::Node &node1 =
                    topology.getNode(segment.propNode1);

                const DBU wirelength = DBUxy::computeManhattanDistance(
                    node0.propPosition, node1.propPosition);

                if (ENABLE_LONG_WIRE_SLICING &&
                    wirelength > longWirelengthThreshold) {
                        const int index0 = segment.propNode0;
                        const int index1 = segment.propNode1;
                        const DBU x0 = node0.propPosition.x;
                        const DBU y0 = node0.propPosition.y;
                        const DBU x1 = node1.propPosition.x;
                        const DBU y1 = node1.propPosition.y;

                        generateSteinerTree_SliceLongWire(
                            longWirelengthThreshold, wireCapPerDistanceUnit,
                            wireResPerDistanceUnit, index0, index1, x0, y0, x1,
                            y1, slicingPoints, dscp, sliceId);
                } else {
                        const Number wireHalfCap =
                            (wireCapPerDistanceUnit * wirelength) / 2.0f;
                        const Number wireRes =
                            wireResPerDistanceUnit * wirelength;

                        dscp.addCapacitor(segment.propNode0, wireHalfCap);
                        dscp.addResistor(segment.propNode0, segment.propNode1,
                                         wireRes);
                        dscp.addCapacitor(segment.propNode1, wireHalfCap);
                }  // end else
        }          // end for

        for (const RoutingTopologyDescriptor<int>::Node &node :
             topology.allNodes()) {
                RCTreeNodeTag *tag;
                try {
                        tag = &dscp.getNodeTag(node.propIndex);
                } catch (RCTreeNodeNotFoundException e) {
                        std::cout
                            << "[BUG] Node of index " << node.propIndex
                            << " and name " << node.propName
                            << " exists in topology, but not in RCTreeNode... "
                               "Aborting...\n";  // Uncomment the lines below
                                                 // for debugging
                                                 //			dscp.print();
                                                 //			for (const
                        // RoutingTopologyDescriptor<int>::Segment &segment :
                        // topology.allSegments()) {
                        //				cout << "There is a
                        // segment
                        // between nodes " <<
                        //					segment.propNode0
                        //<<
                        //"
                        // and " << segment.propNode1 << "\n";
                        //			} // end for
                        std::exit(1);
                }  // end try catch

                tag->x = node.propPosition.x;
                tag->y = node.propPosition.y;
                tag->setPin(node.propPin);

                if (node.propPin && node.propPin.isDriver()) {
                        if (driver) {
                                std::cout << "WARNING: Generating RC-Tree for "
                                             "a multiple-drive net.\n";
                        }  // end if
                        driver = node.propPin;
                        root = node.propIndex;
                }  // end if

        }  // end for

        // Define the tag for slicing points.
        for (auto sp : slicingPoints) {
                RCTreeNodeTag &tag = dscp.getNodeTag(sp.id);
                tag.x = sp.x;
                tag.y = sp.y;
        }  // end for

        // Build the tree.
        if (!tree.build(dscp, root, true)) {
                static bool warned = false;
                if (!warned) {
                        std::cout << "\n[WARNING] A loop was detected (and "
                                     "removed) when building "
                                  << "the tree. "
                                  << "The tree returned by FLUTE may generate "
                                  << "loops when nodes at same position (x, y) "
                                     "are merged. "
                                  << "Typically FLUTE will generate several "
                                     "steiner points at a same "
                                  << "position for large nets. "
                                  << "Note that this is not necessary a bug in "
                                     "the merging scheme, "
                                  << "but just a consequence of the tree "
                                     "topology returned by FLUTE. "
                                  << "Next warnings will be suppressed...\n";
                        warned = true;
                }  // end else
        }          // end else

        // Set load capacitances.
        // [TODO] Optimize, only need to visit tree nodes connected to sinks.
        const int numRCTreeNodes = tree.getNumNodes();
        for (int i = 1; i < numRCTreeNodes;
             i++) {  // start @ 1 to skip root node
                Rsyn::Pin pin = tree.getNodeTag(i).getPin();
                if (!pin) continue;

                EdgeArray<Number> load;

                if (pin.isPort()) {
                        Rsyn::Port port = pin.getInstance().asPort();

                        const Number outputCap =
                            clsScenario->getOutputLoad(port, 0);
                        load[RISE] = outputCap;
                        load[FALL] = outputCap;
                } else {
                        load.setBoth(
                            clsScenario
                                ->getTimingLibraryPin(pin.getLibraryPin())
                                .getCapacitance());
                }  // end else

                tree.setNodeLoadCap(i, load);
        }  // end for

        tree.updateDownstreamCap();
}  // end method

// -----------------------------------------------------------------------------

// Slicing
//
// An interconnection is sliced in several segments. All, but possibly one
// have the same length. The first may be a partial segment when the
// interconnection length is not multiple of the slice length. If they are
// multiple, xk = xi.
//
//  o---o-----o-----o----- ... -----o-----o-----o
//  xi xk                                      xj
//
// Note: UI-Timer seems to connect the partial slice to the driver pin of the
// interconnection. Here we try to mimic that behavior. Maybe we just inverted
// the sink/driver when calling this function, that's why the slicing seemed to
// be inverted. Not sure though.

int DefaultRoutingExtractionModel::generateSteinerTree_SliceLongWire(
    const DBU longWirelengthThreshold, const Number wireCapPerMicron,
    const Number wireResPerMicron, const int index_i, const int index_j,
    const DBU xi, const DBU yi, const DBU xj, const DBU yj,
    std::deque<SlicingPoint> &slicingPoints, RCTreeDescriptor &dscp,
    int &sliceId) const {
        // NOTE. Don't clear the slicing points collection, as a net may have
        // several wires sliced and all slicing points are stored cumulatively
        // in
        // the same collection.

        const DBU totalWirelength = std::abs(xi - xj) + std::abs(yi - yj);
        const DBUxy size(std::abs(xi - xj), std::abs(yi - yj));

        if (totalWirelength <= longWirelengthThreshold) {
                std::cout << "[BUG] Requesting to slice a wire that is short "
                             "or equal to the maximum wire segment.\n";
        }  // end if

        // Compute the number of slicing points.
        const int numSlicingPoints =
            totalWirelength / longWirelengthThreshold -
            ((totalWirelength % longWirelengthThreshold) ? 0 : 1);

        // Create all entire slices, except the last one if that one connects to
        // the endpoint. This happens when the total wirelength is multiple of
        // the
        // maximum (entire) slice wirelength.
        DBU xk = xj;
        DBU yk = yj;
        int prev = index_j;
        DBUxy currDisp(0, 0);
        Dimension currDim = size[X] > size[Y] ? X : Y;

        Number totalCap = 0;  // debug purposes only
        Number totalRes = 0;  // debug purposes only

        const Number entireSliceWirelength = longWirelengthThreshold;
        const Number entireSliceHalfCap =
            wireCapPerMicron * entireSliceWirelength / 2.0f;
        const Number entireSliceRes = wireResPerMicron * entireSliceWirelength;

        const DBU multiplier[2] = {(xi > xj) ? +1 : -1, (yi > yj) ? +1 : -1};

        // Create internal slices (the last slice is handled separately).
        bool reversed = false;
        for (int i = 0; i < numSlicingPoints; i++) {
                // Try to move all we can in the current dimension and change
                // direction
                // if necessary.
                currDisp[currDim] += longWirelengthThreshold;
                if (currDisp[currDim] > size[currDim]) {
                        // Okay, we overshot. Let's adjust and change direction.
                        const DBU overflow = currDisp[currDim] - size[currDim];

                        currDisp[currDim] = size[currDim];
                        currDim = REVERSE_DIMENSION[currDim];
                        currDisp[currDim] += overflow;

                        if (reversed) {
                                std::cout << "[BUG] Reversing direction a "
                                             "second time.\n";
                        }  // end if

                        reversed = true;
                }  // end if

                sliceId--;
                dscp.addCapacitor(prev, entireSliceHalfCap);
                dscp.addResistor(prev, sliceId, entireSliceRes);
                dscp.addCapacitor(sliceId, entireSliceHalfCap);
                prev = sliceId;

                xk = xj + multiplier[X] * currDisp[X];
                yk = yj + multiplier[Y] * currDisp[Y];
                slicingPoints.push_back(SlicingPoint(sliceId, xk, yk));

                // Only for debug purposes...
                totalCap += 2 * entireSliceHalfCap;
                totalRes += entireSliceRes;
        }  // end for

        const DBU wl = currDisp[X] + currDisp[Y];

        // Sanity checks
        if (wl >= totalWirelength) {
                std::cout << "[BUG] Sum of wire segment length is greater or "
                             "equal to the "
                             "total wire segment.\n";
        }  // end if

        if (std::abs(xk - xi) + std::abs(yk - yi) != totalWirelength - wl) {
                std::cout << "[BUG] The distance from the last slice point to "
                             "the endpoint "
                             "does not match the total wire length.\n";
                std::cout << "xj, yj = " << xj << ", " << yj << "\n";
                std::cout << "xk, yk = " << xk << ", " << yk << "\n";
                std::cout << "xi, yi = " << xi << ", " << yi << "\n";
                std::cout << "dist(k, i) = "
                          << (std::abs(xk - xi) + std::abs(yk - yi)) << "\n";
                std::cout << "remainging wl = " << (totalWirelength - wl)
                          << "\n";
        }  // end if

        // Handle last slice, which connects to the endpoint.
        const DBU lastSliceWirelength = totalWirelength - wl;
        const Number lastSliceHalfCap =
            wireCapPerMicron * lastSliceWirelength / 2.0f;
        const Number lastSliceRes = wireResPerMicron * lastSliceWirelength;

        dscp.addCapacitor(sliceId, lastSliceHalfCap);
        dscp.addResistor(sliceId, index_i, lastSliceRes);
        dscp.addCapacitor(index_i, lastSliceHalfCap);

        // Only for debug purposes...
        totalCap += 2 * lastSliceHalfCap;
        totalRes += lastSliceRes;

        // Debug: Sum of capacitance and resistance of slices should match the
        // capacitance and resistance of the original interconnection.
        if (FloatingPoint::notApproximatelyEqual(
                totalCap, wireCapPerMicron * totalWirelength)) {
                std::cout << "[BUG] Total capacitance of sliced "
                             "interconnection does not match the expected "
                             "capacitance.\n";
                std::cout << "  sum cap = " << totalCap << "\n";
                std::cout << "total cap = "
                          << (wireCapPerMicron * totalWirelength) << "\n";
        }  // end if

        if (FloatingPoint::notApproximatelyEqual(
                totalRes, wireResPerMicron * totalWirelength)) {
                std::cout << "[BUG] Total resistance of sliced interconnection "
                             "does not match the expected resistance.\n";
                std::cout << "  sum res = " << totalRes << "\n";
                std::cout << "total res = "
                          << (wireResPerMicron * totalWirelength) << "\n";
        }  // end if

        // Return the number of slices generated. Since the last slice is always
        // handled separated, we always have at least that slice (that's why +
        // 1).
        return numSlicingPoints + 1;
}  // end method

// -----------------------------------------------------------------------------

void DefaultRoutingExtractionModel::updateDownstreamCap(Rsyn::RCTree &tree) {
        const int numRCTreeNodes = tree.getNumNodes();

        for (int i = 1; i < numRCTreeNodes; i++) {
                Rsyn::Pin pin = tree.getNodeTag(i).getPin();

                if (!pin || !pin.isSink()) continue;

                Rsyn::EdgeArray<Number> load;
                load.setBoth(
                    clsScenario->getTimingLibraryPin(pin.getLibraryPin())
                        .getCapacitance());
                tree.setNodeLoadCap(i, load);
        }  // end for

        tree.updateDownstreamCap();
}  // end method

}  // end namespace
