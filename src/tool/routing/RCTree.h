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

#ifndef RSYN_RCTREE_H
#define RSYN_RCTREE_H

#include <string>
using std::string;
#include <iostream>
using std::cout;
#include <cassert>
#include <cmath>

#include <vector>
using std::vector;
#include <map>
using std::map;
#include <queue>
using std::queue;

//#include "NewtonRaphson.h"

#include "tool/timing/TimingTypes.h"
#include "tool/timing/EdgeArray.h"
#include "util/Exception.h"
#include "util/DBU.h"

namespace Rsyn {

class RCTreeLoopException : Exception {
       public:
        RCTreeLoopException() : Exception("RCTree: Loop detected in RC tree") {}
};

class RCTreeOtherNodeNotFound : Exception {
       public:
        RCTreeOtherNodeNotFound() : Exception("RCTree: Other node not found") {}
};

class RCTreeNodeNotFoundException : Exception {
       public:
        RCTreeNodeNotFoundException()
            : Exception("RCTree: Node not found in RC tree") {}
};

////////////////////////////////////////////////////////////////////////////////
// RC Tree Descriptor
////////////////////////////////////////////////////////////////////////////////

template <class NameType, class TagType>
class RCTreeDescriptorTemplate {
       public:
        struct Node {
                Node() { totalCap = 0; }  // end constructor

                NameType propName;
                std::vector<int> propResistors;
                Number totalCap;
                TagType propTag;
        };  // end struct

        struct Resistor {
                Resistor() {
                        propNode0 = -1;
                        propNode1 = -1;
                        propValue = 0;
                }  // end constructor

                int propNode0;
                int propNode1;
                Number propValue;

                int getOtherNode(const int n) const {
                        if (n == propNode0)
                                return propNode1;
                        else if (n == propNode1)
                                return propNode0;
                        else
                                throw RCTreeOtherNodeNotFound();
                }  // end method

        };  // end struct

        struct Capacitor {
                Capacitor() {
                        propNode = -1;
                        propValue = 0.0;
                }  // end constructor

                int propNode;
                Number propValue;
        };  // end struct

       private:
        map<NameType, int> clsNodeMap;

        std::vector<Node> clsNodes;
        std::vector<Resistor> clsResistors;
        std::vector<Capacitor> clsCapacitors;

        Number clsTotalTreeCapacitance;

        // TODO: Add description.
        int createNode(const NameType &name);

       public:
        // TODO: Add description.
        RCTreeDescriptorTemplate();

        // TODO: Add description.
        void addResistor(const NameType &sourceNode, const NameType &targetNode,
                         const Number resistance);

        // TODO: Add description.
        void addCapacitor(const NameType &node, const Number capacitance);

        // TODO: Add description.
        Number getTotalTreeCapacitance() const;

        // TODO: Add description.
        int getNumNodes() const;

        // TODO: Add description.
        int getNumResistors() const;

        // TODO: Add description.
        int getNumCapacitors() const;

        // TODO: Add description.
        const Node &getNode(const int index) const;

        // TODO: Add description.
        const Resistor &getResistor(const int index) const;

        // TODO: Add description.
        const Capacitor &getCapacitor(const int index) const;

        // TODO: Add description.
        int findNode(const NameType &name) const;

        // TODO: Add description.
        int findNodeOrException(const NameType &name);

        // TODO: Add description.
        TagType &getNodeTag(const NameType nodeName);

        // TODO: Add description.
        const TagType &getNodeTag(const NameType nodeName) const;

        // TODO: Add description.
        void setNodeTag(const NameType nodeName, const TagType &tag);

        // TODO: Add description.
        void applyDefaultNodeTag(const TagType &tag);

        // TODO: Add description.
        void applyDefaultCap(const Number cap);

        // TODO: Add description.
        void print() const;
};  // end class

////////////////////////////////////////////////////////////////////////////////
// RC Tree
////////////////////////////////////////////////////////////////////////////////

template <class NameType, class TagType>
class RCTreeBaseTemplate {
       public:
        struct Node {
                // Topology
                // --------

                // Number of outgoing edges from this node.
                int propDownstreamBrachingCount;

                Number propDrivingResistance;
                int propParent;
                bool propEndpoint;

                std::vector<int> propSinks;

                // Extraction
                // ----------

                // Stores the cap related to wires.
                Number propWireCap;

                // Stores the cap related to design pins (e.g. gate inputs or
                // primary
                // output pins).
                EdgeArray<Number> propPinCap;

                // Downstream capacitance.
                EdgeArray<Number> propDownstreamCap;

                // Upstream resistance.
                Number propUpstreamRes;

                // Elmore
                // ------

                EdgeArray<Number> propDownstreamCapDelay;  // used to slew
                                                           // computation (ICCAD
                                                           // 2014 Contest)

                EdgeArray<Number> propDelay;
                EdgeArray<Number> propSlew;

                // Misc
                // ----

                EdgeArray<Number> propSecondMoment;
                EdgeArray<Number> propEffectiveCap;

                EdgeArray<Number> propY1;
                EdgeArray<Number> propY2;
                EdgeArray<Number> propY3;

                // Accessors
                // ---------

                // Returns the sum of wire and pin cap associated to this node.

                EdgeArray<Number> getTotalCap() const {
                        return EdgeArray<Number>(propWireCap, propWireCap) +
                               propPinCap;
                }  // end method

                // Returns only the wire cap associated to this node.

                Number getWireCap() const { return propWireCap; }  // end method

                // Returns only the pin cap associated to this node.

                EdgeArray<Number> getPinCap() const {
                        return propPinCap;
                }  // end method

                // Returns the downstream cap i.e. the sum of the wire and pin
                // cap of
                // this node and all nodes driven, direct or indirectly, by this
                // node.

                EdgeArray<Number> getDownstreamCap() const {
                        return propDownstreamCap;
                }  // end method

                // Returns the upstream res i.e. the sum of the wire segment
                // resistance
                // from this node up to the root.

                Number getUpstreamRes() const {
                        return propUpstreamRes;
                }  // end method

                // Constructor
                // -----------

                Node() {
                        propWireCap = 0;
                        propPinCap.set(0, 0);
                        propParent = -1;
                        propUpstreamRes = 0;
                        propDownstreamBrachingCount = 0;
                }  // end method
        };         // end struct

       protected:
        struct Ref {
                int propParentNodeIndex;
                int propDrivingResistorIndex;

                Ref() {
                        propParentNodeIndex = -1;
                        propDrivingResistorIndex = -1;
                }

                Ref(const int r, const int n) {
                        propParentNodeIndex = n;
                        propDrivingResistorIndex = r;
                }  // end constructor
        };

        std::vector<NameType> clsNodeNames;
        std::vector<TagType> clsNodeTags;
        std::vector<Node> clsNodes;
        std::vector<EdgeArray<Number> > clsCeffs;

        bool clsDirty : 1;
        bool clsLoopDetectedAndRemoved : 1;
        bool clsIdeal : 1;
        bool clsHasUserSpecifiedWireLoad : 1;
        bool clsHasUserSpecifiedRouting : 1;

        Number clsTotalWireCap;
        Number clsUserSpecifiedWireLoad;
        EdgeArray<Number> clsLumpedCap;
        EdgeArray<Number> clsLoadCap;

        // TODO: Add description.
        // Source:
        // http://java2s.com/Tutorial/Cpp/0040__Data-Types/Testswhethertwofloatingpointnumbersareapproximatelyequal.htm
        static bool nearlyEqual(const Number x, const Number y,
                                const Number precision);

        // TODO: Add description.
        static EdgeArray<Number> pow2(const EdgeArray<Number> v);

        // TODO: Add description.
        static EdgeArray<Number> pow3(const EdgeArray<Number> v);

        // Build tree from tree descriptor given a root node.
        // If removeLoops is false, throws RCTreeLoopException exception if a
        // loop
        // is detected.
        void buildTopology(
            const RCTreeDescriptorTemplate<NameType, TagType> &dscp,
            const int root, const bool removeLoops, bool &loopDetected);

        // TODO: Add description.
        template <class RCTreeDriver>
        void stepForward(const RCTreeDriver &driver);

        // TODO: Add description.
        void stepBackward();

       public:
        // Constructor.
        RCTreeBaseTemplate();

        // Reset
        void clear();

        // Return true if this net is ideal (i.e. zero load, zero delay and zero
        // slew degradation).
        bool isIdeal() const;

        // Return true if a wire load value was manually specified by the user.
        bool hasUserSpecifiedWireLoad() const;

        // Return true if the routing of this tree was user specified (i.e. not
        // automatically generated).
        bool hasUserSpecifiedRouting() const;

        // Return the user specified wire load.
        Number getUserSpecifiedWireLoad() const;

        // Set the user specified wire load.
        void setUserSpecifiedWireLoad(const float load);

        // Reset the user specified wire load.
        void resetUserSpecifiedWireLoad();

        // Mark or unmark this net as having ideal timing propagation.
        void setIdeal(const bool enable);

        // If removeLoops is true, it returns false if a loop is detected and
        // true
        // otherwise.
        // If removeLoops is false, throw RCTreeLoopException exception if a
        // loop
        // is detected.
        bool build(const RCTreeDescriptorTemplate<NameType, TagType> &dscp,
                   const NameType &rootNodeName, const bool removeLoops = false,
                   const bool userSpecified = false);

        // TODO: Add description.
        void updateDownstreamCap();

        // TODO: Add description.
        void updateDownstreamBranchingCount();

        // TODO: Add description.
        void updateUpstreamRes();

        // TODO: Add description.
        void updateDrivingPoint();

        // [PAPER] Fast and Accurate Wire Delay Estimation for Physical
        // Synthesis
        // of Large ASICs.
        template <class RCTreeDriver>
        void simulate(const RCTreeDriver &driver, const Number epsilon = 1e-6,
                      const int maxIterations = 100);

        // [PAPER] Modeling the Effective Capacitance for the RC Interconnect of
        // CMOS Gates

        template <class RCTreeDriver>
        EdgeArray<Number> computeEffectiveCapacitanceBasedOnJessica(
            const RCTreeDriver &driver, const Number epsilon = 1e-6,
            const int maxIterations = 100);

        // [PAPER] Performance Computation for Pre-characterized
        // CMOS Gates with RC Loads

        template <class RCTreeDriver>
        EdgeArray<Number> computeEffectiveCapacitanceBasedOnMenezes(
            const RCTreeDriver &driver, const Number epsilon = 1e-6,
            const int maxIterations = 10);

        // [PAPER] Modeling the Driving-Point Characteristic of Resistive
        // Interconnect for Accurate Delay Estimation

        void reduceToPiModel(EdgeArray<Number> &C1, EdgeArray<Number> &R,
                             EdgeArray<Number> &C2);

        // Update Elmore delay/slew.
        void elmore();

        // TODO: Add description.
        void setNodeLoadCap(const int index, const EdgeArray<Number> cap);

        // TODO: Add description.
        void setInputSlew(const EdgeArray<Number> slew);

        // TODO: Add description.
        int getNumNodes() const;

        // TODO: Add description.
        const Node &getNode(const int index) const;

        // TODO: Add description.
        const Node &getRoot() const;

        // TODO: Add description.
        const NameType &getNodeName(const int index) const;

        // TODO: Add description.
        const TagType &getNodeTag(const int index) const;

        // TODO: Add description.
        TagType &getNodeTag(const int index);

        // TODO: Add description.
        EdgeArray<Number> getLumpedCap() const;

        // TODO: Add description.
        EdgeArray<Number> getLoadCap() const;

        // TODO: Add description.
        Number getTotalWireCap() const;

        // TODO: Add description.
        bool getLoopDetectedAndRemovedFlag() const;

        // TODO: Add description.
        bool getDirtyFlag() const;

        // Returns the immediate next downstream node from the node passed as
        // argument. If there are more than one downstream node, the node passed
        // as
        // argument is returned.
        int getDownstreamNodeIndex(const int nodeIndex) const;

        // Returns the immediate previous upstream node from the node passed as
        // argument. If the node passed as argument is the root, returns -1.
        int getUpstreamNodeIndex(const int nodeIndex) const;

        // Returns the index of the upstream steiner point that the node passed
        // as
        // argument connects to. If the node passed as argument is not an
        // endpoint
        // returns the node index itself.
        int getUpstreamSteinerPoint(const int nodeIndex) const;

        // Returns the node index of the fist downstream node that spans more
        // than
        // one other node (i.e. branches). Useful to skip a series of resistors
        // added to slice a long interconnection.
        int getDownstreamBranchingNodeIndex(const int nodeIndex) const;

        // Returns the node index of the fist upstream node that spans more than
        // one other node (i.e. branches). Useful to skip a series of resistors
        // added to slice a long interconnection.
        int getUpstreamBranchingNodeIndex(const int nodeIndex) const;

        // Returns a collection of downstream node indexes spanned by a node.
        std::vector<int> allDownstreamNodes(const int startNodeIndex,
                                            const bool includeStartNode) const;
};  // end class

////////////////////////////////////////////////////////////////////////////////
// Node Tag
////////////////////////////////////////////////////////////////////////////////

class RCTreeNodeTag {
       private:
        Rsyn::Pin pin;

       public:
        RCTreeNodeTag() : pin(nullptr) {}
        RCTreeNodeTag(Rsyn::Pin pin) : pin(pin) {}
        RCTreeNodeTag(Rsyn::Pin pin, const DBU x, const DBU y)
            : pin(pin), x(x), y(y) {}

        void setPin(Rsyn::Pin pin) { this->pin = pin; }

        Rsyn::Pin getPin() const { return this->pin; }

        DBU x;
        DBU y;
};  // end class

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

typedef RCTreeDescriptorTemplate<int, RCTreeNodeTag> RCTreeDescriptor;
typedef RCTreeBaseTemplate<int, RCTreeNodeTag> RCTree;

////////////////////////////////////////////////////////////////////////////////
// RC Tree Descriptor: Implementation
////////////////////////////////////////////////////////////////////////////////

template <class NameType, class TagType>
inline RCTreeDescriptorTemplate<NameType, TagType>::RCTreeDescriptorTemplate() {
        clsTotalTreeCapacitance = 0.0;
}  // end constructor

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeDescriptorTemplate<NameType, TagType>::createNode(
    const NameType &name) {
        typename map<NameType, int>::iterator it = clsNodeMap.find(name);
        if (it != clsNodeMap.end()) {
                return it->second;
        } else {
                const int index = clsNodes.size();
                clsNodes.resize(clsNodes.size() + 1);
                clsNodes.back().propName = name;
                clsNodeMap[name] = index;
                return index;
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeDescriptorTemplate<NameType, TagType>::addResistor(
    const NameType &sourceNode, const NameType &targetNode,
    const Number resistance) {
        const int index = clsResistors.size();
        clsResistors.resize(clsResistors.size() + 1);

        Resistor &r = clsResistors.back();
        r.propNode0 = createNode(sourceNode);
        r.propNode1 = createNode(targetNode);
        r.propValue = resistance;

        clsNodes[r.propNode0].propResistors.push_back(index);
        clsNodes[r.propNode1].propResistors.push_back(index);
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeDescriptorTemplate<NameType, TagType>::addCapacitor(
    const NameType &node, const Number capacitance) {
        clsCapacitors.resize(clsCapacitors.size() + 1);

        Capacitor &c = clsCapacitors.back();
        c.propNode = createNode(node);
        c.propValue = capacitance;

        clsNodes[c.propNode].totalCap += c.propValue;

        clsTotalTreeCapacitance += c.propValue;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline Number
RCTreeDescriptorTemplate<NameType, TagType>::getTotalTreeCapacitance() const {
        return clsTotalTreeCapacitance;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeDescriptorTemplate<NameType, TagType>::getNumNodes() const {
        return clsNodes.size();
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeDescriptorTemplate<NameType, TagType>::getNumResistors()
    const {
        return clsResistors.size();
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeDescriptorTemplate<NameType, TagType>::getNumCapacitors()
    const {
        return clsCapacitors.size();
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline const typename RCTreeDescriptorTemplate<NameType, TagType>::Node &
RCTreeDescriptorTemplate<NameType, TagType>::getNode(const int index) const {
        return clsNodes[index];
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline const typename RCTreeDescriptorTemplate<NameType, TagType>::Resistor &
RCTreeDescriptorTemplate<NameType, TagType>::getResistor(
    const int index) const {
        return clsResistors[index];
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline const typename RCTreeDescriptorTemplate<NameType, TagType>::Capacitor &
RCTreeDescriptorTemplate<NameType, TagType>::getCapacitor(
    const int index) const {
        return clsCapacitors[index];
}

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeDescriptorTemplate<NameType, TagType>::findNode(
    const NameType &name) const {
        typename map<NameType, int>::const_iterator it = clsNodeMap.find(name);
        return (it != clsNodeMap.end() ? it->second : -1);
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeDescriptorTemplate<NameType, TagType>::findNodeOrException(
    const NameType &name) {
        typename std::map<NameType, int>::const_iterator it =
            clsNodeMap.find(name);

        if (it == clsNodeMap.end()) throw RCTreeNodeNotFoundException();
        return it->second;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline TagType &RCTreeDescriptorTemplate<NameType, TagType>::getNodeTag(
    const NameType nodeName) {
        return clsNodes[findNodeOrException(nodeName)].propTag;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline const TagType &RCTreeDescriptorTemplate<NameType, TagType>::getNodeTag(
    const NameType nodeName) const {
        return clsNodes[findNodeOrException(nodeName)].propTag;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeDescriptorTemplate<NameType, TagType>::setNodeTag(
    const NameType nodeName, const TagType &tag) {
        clsNodes[findNodeOrException(nodeName)].propTag = tag;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeDescriptorTemplate<NameType, TagType>::applyDefaultNodeTag(
    const TagType &tag) {
        const int numNodes = clsNodes.size();
        for (int i = 0; i < numNodes; i++) {
                clsNodes[i].propTag = tag;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeDescriptorTemplate<NameType, TagType>::applyDefaultCap(
    const Number cap) {
        const int numCaps = clsCapacitors.size();
        for (int i = 0; i < numCaps; i++) {
                Capacitor &capacitor = clsCapacitors[i];
                if (capacitor.propValue == 0.0) {
                        capacitor.propValue += cap;
                        clsTotalTreeCapacitance += cap;
                }  // end if
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeDescriptorTemplate<NameType, TagType>::print() const {
        //		const int numNodes = clsNodes.size();
        //		for ( int i = 0; i < numNodes; i++ ) {
        //			const auto &node = clsNodes[i];
        //
        //			std::cout << "node " << node.propName << "\n";
        //			for (int k = 0; k < node.propResistors.size();
        // k++)
        //{
        //				const auto &res =
        // clsResistors[node.propResistors[k]];
        //				const auto &name0 =
        // clsNodes[res.propNode0].propName;
        //				const auto &name1 =
        // clsNodes[res.propNode1].propName;
        //
        //				std::cout << "\tres " << name0 << "<-->"
        //<<
        // name1 << " " << res.propValue;
        //				if (node.propName != name0  &&
        // node.propName
        //!= name1) {
        //					std::cout << " *** BUUUUUUUUUUUG
        //***";
        //				} // end if
        //				std::cout << "\n";
        //			} // end for
        //			//<< " (tag=" << node.propTag << ")\n"
        //		} // end for

        const int numResistors = clsResistors.size();
        for (int i = 0; i < numResistors; i++) {
                const Resistor &res = clsResistors[i];
                cout << "res " << clsNodes[res.propNode0].propName << " "
                     << clsNodes[res.propNode1].propName << " " << res.propValue
                     << "\n";
        }  // end for

        const int numCapacitors = clsCapacitors.size();
        for (int i = 0; i < numCapacitors; i++) {
                const Capacitor &cap = clsCapacitors[i];
                cout << "cap " << clsNodes[cap.propNode].propName << " "
                     << cap.propValue << "\n";
        }  // end for
}  // end method

////////////////////////////////////////////////////////////////////////////////
// RC Tree: Implementation
////////////////////////////////////////////////////////////////////////////////

template <class NameType, class TagType>
inline RCTreeBaseTemplate<NameType, TagType>::RCTreeBaseTemplate() {
        clear();
}  // end constructor

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline bool RCTreeBaseTemplate<NameType, TagType>::nearlyEqual(
    const Number x, const Number y, const Number precision) {
        if (x == 0) return fabs(y) <= precision;
        if (y == 0) return fabs(x) <= precision;
        return fabs(x - y) / std::max(fabs(x), fabs(y)) <= precision;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline EdgeArray<Number> RCTreeBaseTemplate<NameType, TagType>::pow2(
    const EdgeArray<Number> v) {
        return v * v;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline EdgeArray<Number> RCTreeBaseTemplate<NameType, TagType>::pow3(
    const EdgeArray<Number> v) {
        return v * v * v;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::buildTopology(
    const RCTreeDescriptorTemplate<NameType, TagType> &dscp, const int root,
    const bool removeLoops, bool &loopDetected) {
        const int numNodes = dscp.getNumNodes();

        loopDetected = false;

        // Compute topological order.
        std::vector<bool> visited(numNodes, false);  // loop detect

        std::vector<int> topology(numNodes, -1);
        std::vector<int> reverseTopology(numNodes, -1);

        clsTotalWireCap = 0;
        clsLumpedCap.set(0, 0);
        clsLoadCap.set(0, 0);

        queue<Ref> q;

        const typename RCTreeDescriptorTemplate<NameType, TagType>::Node
            &rootNodeDescriptor = dscp.getNode(root);

        Node &rootNode = clsNodes[0];
        rootNode.propDelay.set(0.0, 0.0);
        rootNode.propWireCap = rootNodeDescriptor.totalCap;

        rootNode.propDrivingResistance = -1;
        rootNode.propParent = -1;
        clsNodeNames[0] = rootNodeDescriptor.propName;
        clsNodeTags[0] = rootNodeDescriptor.propTag;

        clsTotalWireCap += rootNode.getWireCap();
        clsLumpedCap += rootNode.getTotalCap();

        for (int k = 0; k < rootNodeDescriptor.propResistors.size(); k++) {
                const int r = rootNodeDescriptor.propResistors[k];
                q.push(Ref(r, root));
        }  // end method
        rootNode.propEndpoint = q.empty();

        topology[0] = root;
        reverseTopology[root] = 0;

        int counter = 1;

        while (!q.empty()) {
                const int parent = q.front().propParentNodeIndex;
                const int r = q.front().propDrivingResistorIndex;
                q.pop();

                const typename RCTreeDescriptorTemplate<
                    NameType, TagType>::Resistor &resistorDescriptor =
                    dscp.getResistor(r);

                const int n = resistorDescriptor.getOtherNode(parent);
                if (visited[n]) {
                        // loop detected
                        loopDetected = true;

                        if (!removeLoops)
                                throw RCTreeLoopException();
                        else
                                continue;
                }  // end if
                visited[n] = true;

                const typename RCTreeDescriptorTemplate<NameType, TagType>::Node
                    &nodeDescriptor = dscp.getNode(n);

                Node &node = clsNodes[counter];
                node.propWireCap = nodeDescriptor.totalCap;
                node.propDrivingResistance = resistorDescriptor.propValue;
                node.propParent =
                    reverseTopology[resistorDescriptor.getOtherNode(n)];
                clsNodeNames[counter] = nodeDescriptor.propName;
                clsNodeTags[counter] = nodeDescriptor.propTag;

                Node &parentNode = clsNodes[node.propParent];
                parentNode.propSinks.push_back(counter);

                clsTotalWireCap += node.getWireCap();
                clsLumpedCap += node.getTotalCap();
                clsLoadCap += node.getPinCap();

                topology[counter] = n;
                reverseTopology[n] = counter;

                int counterNeighbours = 0;

                const int numResistors = nodeDescriptor.propResistors.size();
                for (int k = 0; k < numResistors; k++) {
                        const int r = nodeDescriptor.propResistors[k];
                        if (dscp.getResistor(r).getOtherNode(n) != parent) {
                                q.push(Ref(r, n));
                                counterNeighbours++;
                        }  // end if
                }          // end method

                node.propEndpoint = (counterNeighbours == 0);

                counter++;
        }  // end while

        clsLoopDetectedAndRemoved = loopDetected;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
template <class RCTreeDriver>
inline void RCTreeBaseTemplate<NameType, TagType>::stepForward(
    const RCTreeDriver &driver) {
        Node &rootState = clsNodes[0];

        rootState.propSlew = driver.computeSlew(rootState.propEffectiveCap);

        const int numNodes = clsNodes.size();
        for (int n = 1; n < numNodes; n++) {  // 1 => skips root node
                Node &node = clsNodes[n];
                const Node &parent = clsNodes[node.propParent];

                const EdgeArray<Number> S0 = parent.propSlew;
                const EdgeArray<Number> Ceff1 = node.propEffectiveCap;
                const Number R1 = node.propDrivingResistance;

                const EdgeArray<Number> RCeff = R1 * Ceff1;
                node.propDelay = parent.propDelay + RCeff;
                node.propSlew =
                    S0 / (1.0 - ((RCeff) / S0) * (1.0 - exp(-S0 / (RCeff))));
        }  // end for
}  // end for

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::stepBackward() {
        const int numNodes = clsNodes.size();

        for (int i = 0; i < numNodes; i++) {
                Node &node = clsNodes[i];

                clsCeffs[i] = node.propEffectiveCap;
                node.propEffectiveCap = node.getTotalCap();
        }  // end for

        for (int n = numNodes - 1; n > 0; n--) {  // n > 0 skips root node
                const Node &sink = clsNodes[n];
                Node &driver = clsNodes[sink.propParent];

                const EdgeArray<Number> S0 = driver.propSlew;
                const EdgeArray<Number> Ceff1 = clsCeffs[n];
                const Number R1 = sink.propDrivingResistance;

                const EdgeArray<Number> RCeff = R1 * Ceff1;
                const EdgeArray<Number> K1 =
                    1.0 -
                    ((2.0 * RCeff) / S0) * (1.0 - exp(-S0 / (2.0 * RCeff)));
                driver.propEffectiveCap += K1 * sink.propDownstreamCap;
        }  // end for
}  // end for

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::clear() {
        clsDirty = false;
        clsLoopDetectedAndRemoved = false;
        clsIdeal = false;
        clsHasUserSpecifiedWireLoad = false;
        clsHasUserSpecifiedRouting = false;

        clsNodes.clear();
        clsNodeNames.clear();
        clsNodeTags.clear();
        clsCeffs.clear();

        clsTotalWireCap = 0;
        clsUserSpecifiedWireLoad = 0;
        clsLumpedCap.set(0, 0);
        clsLoadCap.set(0, 0);
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline bool RCTreeBaseTemplate<NameType, TagType>::isIdeal() const {
        return clsIdeal;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline bool RCTreeBaseTemplate<NameType, TagType>::hasUserSpecifiedWireLoad()
    const {
        return clsHasUserSpecifiedWireLoad;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline bool RCTreeBaseTemplate<NameType, TagType>::hasUserSpecifiedRouting()
    const {
        return clsHasUserSpecifiedRouting;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline Number RCTreeBaseTemplate<NameType, TagType>::getUserSpecifiedWireLoad()
    const {
        return clsUserSpecifiedWireLoad;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::setUserSpecifiedWireLoad(
    const float load) {
        clsUserSpecifiedWireLoad = load;
        clsHasUserSpecifiedWireLoad = true;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void
RCTreeBaseTemplate<NameType, TagType>::resetUserSpecifiedWireLoad() {
        clsUserSpecifiedWireLoad = 0;
        clsHasUserSpecifiedWireLoad = false;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::setIdeal(const bool enable) {
        clsIdeal = enable;
}  // end if

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline bool RCTreeBaseTemplate<NameType, TagType>::build(
    const RCTreeDescriptorTemplate<NameType, TagType> &dscp,
    const NameType &rootNodeName, const bool removeLoops,
    const bool userSpecified) {
        const int numNodes = dscp.getNumNodes();

        // Clean up
        clear();

        clsNodes.resize(numNodes);
        clsNodeNames.resize(numNodes);
        clsNodeTags.resize(numNodes);
        clsCeffs.resize(numNodes);

        // Map topological index (e.g. 0 = root) to respective node index in the
        // descriptor.
        bool loopDetected;
        buildTopology(dscp, dscp.findNode(rootNodeName), removeLoops,
                      loopDetected);

        // Mark this tree as user specified if requested.
        clsHasUserSpecifiedRouting = userSpecified;

        // Update downstream cap.
        updateDownstreamCap();

        // Update upstream res.
        updateUpstreamRes();

        // Update branching count.
        updateDownstreamBranchingCount();

        // Return
        return !loopDetected;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::updateDownstreamCap() {
        const int numNodes = getNumNodes();
        for (int i = 0; i < numNodes; i++) {
                Node &node = clsNodes[i];
                node.propDownstreamCap = node.getTotalCap();
        }  // end for

        for (int n = numNodes - 1; n > 0; n--) {  // n > 0 skips root node
                const Node &node = clsNodes[n];

                // Temporary debug. This should never happen, but it was leading
                // RC-tree extractor to cause memory corruption
                if (node.propParent < 0 || node.propParent >= clsNodes.size()) {
                        std::cout << "[ERROR] Invalid parent index ("
                                  << node.propParent << ") of node ("
                                  << clsNodeNames[n] << ") the tree is..\n";
                        // Printing all nodes of the tree and its parents...
                        for (int i = 0; i < numNodes; i++) {
                                std::cout
                                    << "\t[" << i
                                    << "] Node: " << clsNodeNames[i]
                                    << " parent: " << clsNodes[i].propParent
                                    << "\n";
                        }
                        std::exit(1);
                }

                clsNodes[node.propParent].propDownstreamCap +=
                    node.propDownstreamCap;
        }  // end for

        clsDirty = false;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void
RCTreeBaseTemplate<NameType, TagType>::updateDownstreamBranchingCount() {
        const int numNodes = getNumNodes();
        for (int i = 0; i < numNodes; i++) {
                Node &node = clsNodes[i];
                node.propDownstreamBrachingCount = 0;
        }  // end for

        for (int n = numNodes - 1; n > 0; n--) {  // n > 0 skips root node
                const Node &node = clsNodes[n];
                clsNodes[node.propParent].propDownstreamBrachingCount++;
        }  // end for

        clsDirty = false;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::updateUpstreamRes() {
        const int numNodes = getNumNodes();
        if (numNodes == 0) return;

        clsNodes[0].propUpstreamRes = 0;
        for (int i = 1; i < numNodes; i++) {  // skip root
                Node &node = clsNodes[i];
                const Node &parent = clsNodes[node.propParent];
                node.propUpstreamRes =
                    parent.propUpstreamRes + node.propDrivingResistance;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::updateDrivingPoint() {
        const int numNodes = getNumNodes();
        for (int i = 0; i < numNodes; i++) {
                Node &node = clsNodes[i];
                node.propY1.set(0, 0);
                node.propY2.set(0, 0);
                node.propY3.set(0, 0);
        }  // end for

        // Compute pi-model of the RC Tree.
        for (int n = numNodes - 1; n > 0; n--) {  // n > 0 skips root node
                const Node &node = clsNodes[n];

                const EdgeArray<Number> C = node.getTotalCap();
                const EdgeArray<Number> R = EdgeArray<Number>(
                    node.propDrivingResistance, node.propDrivingResistance);

                const EdgeArray<Number> yD1 = node.propY1;
                const EdgeArray<Number> yD2 = node.propY2;
                const EdgeArray<Number> yD3 = node.propY3;

                const EdgeArray<Number> yU1 = yD1 + C;
                const EdgeArray<Number> yU2 =
                    yD2 - R * (pow2(yD1) + C * yD1 + (1.0 / 3.0) * pow2(C));
                const EdgeArray<Number> yU3 =
                    yD3 - R * (2 * yD1 * yD2 + C * yD2) +
                    pow2(R) *
                        (pow3(yD1) + (4.0 / 3.0) * C * pow2(yD1) +
                         (2.0 / 3.0) * pow2(C) * yD1 + (2.0 / 15.0) * pow3(C));

                Node &parent = clsNodes[node.propParent];
                parent.propY1 += yU1;
                parent.propY2 += yU2;
                parent.propY3 += yU3;

                // cout << "Resistor: " << clsNodeNames[node.propParent] << " ->
                // " << clsNodeNames[n] << "\n";
        }  // end for

        Node &root = clsNodes[0];
        root.propY1 += root.getTotalCap();
}  // end method

// -----------------------------------------------------------------------------

// [PAPER] Fast and Accurate Wire Delay Estimation for Physical Synthesis
// of Large ASICs.

template <class NameType, class TagType>
template <class RCTreeDriver>
inline void RCTreeBaseTemplate<NameType, TagType>::simulate(
    const RCTreeDriver &driver, const Number epsilon, const int maxIterations) {
        if (clsDirty) updateDownstreamCap();

        const int numNodes = clsNodes.size();

        // Initially set effective capacitance equals to downstream capacitance.
        for (int i = 0; i < numNodes; i++) {
                Node &node = clsNodes[i];
                node.propEffectiveCap =
                    EdgeArray<Number>(clsNodes[i].propDownstreamCap,
                                      clsNodes[i].propDownstreamCap);
        }  // end for

        bool converged = false;
        EdgeArray<Number> previousRootEffectiveCapacitance =
            clsNodes[0].propEffectiveCap;
        for (int i = 0; i < maxIterations; i++) {
                stepForward(driver);
                stepBackward();

                const Node &root = clsNodes[0];

                if (nearlyEqual(previousRootEffectiveCapacitance[RISE],
                                root.propEffectiveCap[RISE], epsilon) &&
                    nearlyEqual(previousRootEffectiveCapacitance[FALL],
                                root.propEffectiveCap[FALL], epsilon)) {
                        converged = true;
                        break;
                }  // end if

                previousRootEffectiveCapacitance = root.propEffectiveCap;
        }  // end for

        if (!converged)
                cout << "[WARNING] Simulation for RC tree driven by node '"
                     << clsNodeNames[0] << "' did not converge within "
                     << maxIterations << " iterations.\n";
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
template <class RCTreeDriver>
inline EdgeArray<Number> RCTreeBaseTemplate<NameType, TagType>::
    computeEffectiveCapacitanceBasedOnJessica(const RCTreeDriver &driver,
                                              const Number epsilon,
                                              const int maxIterations) {
        bool converged = false;

        const EdgeArray<Number> tr = driver.getInputSlew();

        EdgeArray<Number> C2, R, C1;
        reduceToPiModel(C1, R, C2);

        EdgeArray<Number> Ceff(getLumpedCap());
        EdgeArray<Number> previousCeff = Ceff;

        for (int i = 0; i < maxIterations; i++) {
                const EdgeArray<Number> td = driver.computeDelay(Ceff);
                const EdgeArray<Number> tf = driver.computeSlew(Ceff);

                const EdgeArray<Number> tD = td + tr / 2.0;
                const EdgeArray<Number> tx = tD - 0.5 * tf;

                const EdgeArray<Number> shielding =
                    (1 - (R * C1) / (tD - tx / 2.0) +
                     (pow(R * C1, 2.0) / (tx * (tD - tx / 2.0))) *
                         (exp(-(tD - tx) / (R * C1))) *
                         (1 - exp((-tx) / (R * C1))));

                Ceff = C2 + C1 * (1.0 * shielding + 1.0) / 2.0;

                if (nearlyEqual(previousCeff[RISE], Ceff[RISE], epsilon) &&
                    nearlyEqual(previousCeff[FALL], Ceff[FALL], epsilon)) {
                        converged = true;
                        break;
                }  // end if

                previousCeff = Ceff;

        }  // end for
           /*
            if ( !converged )
            cout << "[WARNING] Simulation for RC tree driven by node '" <<
            clsNodeNames[0] << "' did not converge within " << maxIterations << "
            iterations.\n";
            */
        return Ceff;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
template <class RCTreeDriver>
inline EdgeArray<Number> RCTreeBaseTemplate<NameType, TagType>::
    computeEffectiveCapacitanceBasedOnMenezes(const RCTreeDriver &driver,
                                              const Number epsilon,
                                              const int maxIterations) {
        //	bool converged = false;
        //
        //	const EdgeArray<Number> tr = driver.getInputSlew();
        //
        //	Number C2, R, C1;
        //	reduceToPiModel(C1, R, C2);
        //
        //	EdgeArray<Number> Ceff(getLumpedCap(),getLumpedCap());
        //	EdgeArray<Number> Rd, previousCeff = Ceff;
        //	Number t0, dt;
        //
        //	for ( int i = 0; i < maxIterations; i ++ ) {
        //
        //		const EdgeArray<Number> td = driver.computeDelay(Ceff);
        //		const EdgeArray<Number> deltatd =
        //(driver.computeDelay(Ceff) - driver.computeDelay(Ceff*0.99));
        //		const EdgeArray<Number> tf = driver.computeSlew(Ceff);
        //
        //		const EdgeArray<Number> tD = td + tr/2;
        //		const EdgeArray<Number> tx = tD - 0.5*tf;
        //
        //		Rd =  deltatd/(Ceff*0.01);
        //		Newton_functor functorRise( Rd[RISE], R, C1, C2,
        // tf[RISE]);
        //		// {
        //		NewtonRaphsonSolve0< Newton_functor, Number >
        //		newtonRise(
        //				   Ceff[RISE],               // X0
        //				   1e-30,                 // Epsilon
        //				   true,                 // Twice_df
        //				   100,                  // max_iter
        //				   functorRise,              // F
        //				   &Newton_functor::f,   // f
        //				   &Newton_functor::df); // df
        //
        //		//   cout << "running iteration with f and df defined,
        // over all Number Numbers" << endl;
        //		newtonRise.set_check_boundary( true);
        //		newtonRise.set_max_x( C1 + C2 );
        //		newtonRise.set_min_x( C2 );
        //
        //		Newton_functor functorFall( Rd[FALL], R, C1, C2,
        // tf[FALL]);
        //		// {
        //		NewtonRaphsonSolve0< Newton_functor, Number >
        //		newtonFall(
        //				   Ceff[FALL],               // X0
        //				   1e-30,                 // Epsilon
        //				   true,                 // Twice_df
        //				   100,                  // max_iter
        //				   functorFall,              // F
        //				   &Newton_functor::f,   // f
        //				   &Newton_functor::df); // df
        //
        //		//   cout << "running iteration with f and df defined,
        // over all Number Numbers" << endl;
        //		newtonFall.set_check_boundary( true);
        //		newtonFall.set_max_x( C1 + C2 );
        //		newtonFall.set_min_x( C2 );
        //
        //		//Ceff = EdgeArray<Number>(newtonRise.do_iteration(
        //&cout), newtonFall.do_iteration( &cout)) ;
        //		Ceff = EdgeArray<Number>(newtonRise.do_iteration(
        // NULL/*&cout*/), newtonFall.do_iteration( NULL/*&cout*/)) ;
        //		//cout << "Ceff: " << Ceff << endl;
        //
        //		if ( nearlyEqual(previousCeff[RISE], Ceff[RISE], epsilon
        //)
        //&&
        //			nearlyEqual(previousCeff[FALL], Ceff[FALL],
        // epsilon
        //)	)
        //		{
        //			converged = true;
        //			break;
        //		} // end if
        //
        //		previousCeff = Ceff;
        //
        //	} // end for
        //	/*
        //	 if ( !converged )
        //	 cout << "[WARNING] Simulation for RC tree driven by node '" <<
        // clsNodeNames[0] << "' did not converge within " << maxIterations << "
        // iterations.\n";
        //	 */
        //	return Ceff;

        assert(false);
        return EdgeArray<Number>(0, 0);
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::reduceToPiModel(
    EdgeArray<Number> &C1, EdgeArray<Number> &R, EdgeArray<Number> &C2) {
        if (clsDirty) updateDownstreamCap();

        updateDrivingPoint();

        const Node &root = clsNodes[0];
        C1 = pow2(root.propY2) / root.propY3;
        C2 = root.propY1 - C1;
        R = -pow2(root.propY3) / pow3(root.propY2);
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::elmore() {
        const int numNodes = clsNodes.size();

        updateDownstreamCap();

        Node &rootState = clsNodes[0];
        rootState.propDelay.set(0, 0);
        rootState.propSecondMoment.set(0, 0);

        // Compute delay.
        for (int n = 1; n < numNodes; n++) {  // 1 => skips root node
                Node &node = clsNodes[n];
                const Node &parent = clsNodes[node.propParent];

                node.propDelay =
                    parent.propDelay +
                    node.propDrivingResistance * node.propDownstreamCap;
        }  // end for

        // Compute slew - first pass: update downstream cap-delay
        for (int i = 0; i < numNodes; i++) {
                Node &node = clsNodes[i];
                node.propDownstreamCapDelay =
                    node.getTotalCap() * node.propDelay;
        }  // end for

        for (int n = numNodes - 1; n > 0; n--) {  // n > 0 skips root node
                const Node &node = clsNodes[n];
                clsNodes[node.propParent].propDownstreamCapDelay +=
                    node.propDownstreamCapDelay;
        }  // end for

        // Compute slew - second pass: compute slew
        const EdgeArray<Number> si = pow2(rootState.propSlew);

        for (int n = 1; n < numNodes; n++) {  // 1 => skips root node
                Node &node = clsNodes[n];
                const Node &parent = clsNodes[node.propParent];

                node.propSecondMoment =
                    parent.propSecondMoment +
                    node.propDrivingResistance * node.propDownstreamCapDelay;

                node.propSlew = sqrt(
                    si + abs(2 * node.propSecondMoment - pow2(node.propDelay)));
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::setNodeLoadCap(
    const int index, const EdgeArray<Number> cap) {
        Node &node = clsNodes[index];
        clsLumpedCap -= node.getPinCap();
        clsLoadCap -= node.getPinCap();
        node.propPinCap = cap;
        clsLumpedCap += node.getPinCap();
        clsLoadCap += node.getPinCap();

        clsDirty = true;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline void RCTreeBaseTemplate<NameType, TagType>::setInputSlew(
    const EdgeArray<Number> slew) {
        clsNodes[0].propSlew = slew;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeBaseTemplate<NameType, TagType>::getNumNodes() const {
        return clsNodes.size();
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline const typename RCTreeBaseTemplate<NameType, TagType>::Node &
RCTreeBaseTemplate<NameType, TagType>::getNode(const int index) const {
        return clsNodes[index];
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline const typename RCTreeBaseTemplate<NameType, TagType>::Node &
RCTreeBaseTemplate<NameType, TagType>::getRoot() const {
        return clsNodes[0];
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline const NameType &RCTreeBaseTemplate<NameType, TagType>::getNodeName(
    const int index) const {
        return clsNodeNames[index];
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline const TagType &RCTreeBaseTemplate<NameType, TagType>::getNodeTag(
    const int index) const {
        return clsNodeTags[index];
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline TagType &RCTreeBaseTemplate<NameType, TagType>::getNodeTag(
    const int index) {
        return clsNodeTags[index];
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline EdgeArray<Number> RCTreeBaseTemplate<NameType, TagType>::getLumpedCap()
    const {
        return clsLumpedCap;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline EdgeArray<Number> RCTreeBaseTemplate<NameType, TagType>::getLoadCap()
    const {
        return clsLoadCap;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline Number RCTreeBaseTemplate<NameType, TagType>::getTotalWireCap() const {
        return clsTotalWireCap;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline bool
RCTreeBaseTemplate<NameType, TagType>::getLoopDetectedAndRemovedFlag() const {
        return clsLoopDetectedAndRemoved;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline bool RCTreeBaseTemplate<NameType, TagType>::getDirtyFlag() const {
        return clsDirty;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeBaseTemplate<NameType, TagType>::getDownstreamNodeIndex(
    const int nodeIndex) const {
        const Node &node = getNode(nodeIndex);
        return node.propSinks.size() == 1 ? node.propSinks[0] : nodeIndex;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeBaseTemplate<NameType, TagType>::getUpstreamNodeIndex(
    const int nodeIndex) const {
        return getNode(nodeIndex).propParent;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeBaseTemplate<NameType, TagType>::getUpstreamSteinerPoint(
    const int nodeIndex) const {
        const Node &node = getNode(nodeIndex);
        return node.propEndpoint ? node.propParent : nodeIndex;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int
RCTreeBaseTemplate<NameType, TagType>::getDownstreamBranchingNodeIndex(
    const int nodeIndex) const {
        int i = nodeIndex;
        while (true) {
                const Node &node = getNode(i);
                if (node.propSinks.size() != 1) {
                        break;
                }  // end if
                i = node.propSinks[0];
        }  // end while

        return i;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline int RCTreeBaseTemplate<NameType, TagType>::getUpstreamBranchingNodeIndex(
    const int nodeIndex) const {
        int i = nodeIndex;
        while (i != -1) {
                const Node &node = getNode(i);
                if (node.propSinks.size() != 1) {
                        break;
                }  // end if
                i = node.propParent;
        }  // end while

        // If no upstream branching point was found, returns the root node.
        return i == -1 ? 0 : i;
}  // end method

// -----------------------------------------------------------------------------

template <class NameType, class TagType>
inline std::vector<int>
RCTreeBaseTemplate<NameType, TagType>::allDownstreamNodes(
    const int startNodeIndex, const bool includeStartNode) const {
        std::vector<int> result;
        result.reserve(getNumNodes());

        std::queue<int> open;

        // Add immediate sinks as the first elements of the queue.
        if (includeStartNode) {
                open.push(startNodeIndex);
        } else {
                for (int sink : getNode(startNodeIndex).propSinks) {
                        open.push(sink);
                }  // end for
        }          // end else

        // BFS
        while (!open.empty()) {
                // Remove the first element of the queue.
                const int n = open.front();
                open.pop();

                // Store this node as a downstrem node.
                result.push_back(n);

                // Add sinks to the queue.
                for (int sink : getNode(n).propSinks) {
                        open.push(sink);
                }  // end for
        }          // end while

        return result;
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace

#endif
