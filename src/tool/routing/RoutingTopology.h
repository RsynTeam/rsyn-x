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

#ifndef RSYN_ROUTING_TOPOLOGY_H
#define RSYN_ROUTING_TOPOLOGY_H

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

#include "util/Exception.h"
#include "util/DBU.h"

namespace Rsyn {

class RoutingTopologyLoopException : Exception {
       public:
        RoutingTopologyLoopException()
            : Exception("RoutingTopology: Loop detected.") {}
};

class RoutingTopologyOtherNodeNotFound : Exception {
       public:
        RoutingTopologyOtherNodeNotFound()
            : Exception("RoutingTopology: Other node not found") {}
};

class RoutingTopologyNodeNotFoundException : Exception {
       public:
        RoutingTopologyNodeNotFoundException()
            : Exception("RoutingTopology: Node not found in routing topology") {
        }
};

template <class NameType>
class RoutingTopologyDescriptor {
       public:
        struct Node {
                Node() { propIndex = -1; }  // end constructor

                int propIndex;
                NameType propName;
                std::vector<int> propSegments;
                DBUxy propPosition;
                Rsyn::Pin propPin;
        };  // end struct

        struct Segment {
                Segment() {
                        propNode0 = -1;
                        propNode1 = -1;
                        propRoutingLayer = -1;
                }  // end constructor

                int propNode0;
                int propNode1;
                int propRoutingLayer;

                int getOtherNode(const int n) const {
                        if (n == propNode0)
                                return propNode1;
                        else if (n == propNode1)
                                return propNode0;
                        else
                                throw RoutingTopologyOtherNodeNotFound();
                }  // end method

        };  // end struct

       private:
        std::map<NameType, int> clsNodeMap;

        std::vector<Node> clsNodes;
        std::vector<Segment> clsSegments;

       public:
        RoutingTopologyDescriptor() { clear(); }  // end constructor

        void clear() {
                clsNodeMap.clear();
                clsNodes.clear();
                clsSegments.clear();
        }  // end method

        int createNode(const NameType &name, const DBUxy pos = DBUxy(0, 0),
                       Rsyn::Pin attachedPin = nullptr) {
                typename std::map<NameType, int>::iterator it =
                    clsNodeMap.find(name);
                if (it != clsNodeMap.end()) {
                        return it->second;
                } else {
                        const int index = clsNodes.size();
                        clsNodes.resize(clsNodes.size() + 1);
                        clsNodes.back().propIndex = index;
                        clsNodes.back().propName = name;
                        clsNodes.back().propPosition = pos;
                        clsNodes.back().propPin = attachedPin;
                        clsNodeMap[name] = index;
                        return index;
                }  // end if
        }          // end method

        void addSegment(const NameType &sourceNode,
                        const NameType &targetNode) {
                const int index = clsSegments.size();
                clsSegments.resize(clsSegments.size() + 1);

                Segment &r = clsSegments.back();
                r.propNode0 = createNode(sourceNode);
                r.propNode1 = createNode(targetNode);

                clsNodes[r.propNode0].propSegments.push_back(index);
                clsNodes[r.propNode1].propSegments.push_back(index);
        }  // end method

        int getNumNodes() const { return clsNodes.size(); }

        int getNumSegments() const { return clsSegments.size(); }

        const Node &getNode(const int index) const { return clsNodes[index]; }

        const Segment &getSegment(const int index) const {
                return clsSegments[index];
        }

        int findNode(const NameType &name) const {
                typename map<NameType, int>::const_iterator it =
                    clsNodeMap.find(name);
                return (it != clsNodeMap.end() ? it->second : -1);
        }  // end method

        int findNodeOrException(const NameType &name) {
                typename std::map<NameType, int>::const_iterator it =
                    clsNodeMap.find(name);

                if (it == clsNodeMap.end())
                        throw RoutingTopologyNodeNotFoundException();
                return it->second;
        }  // end method

        DBUxy getNodePosition(const NameType nodeName) const {
                return clsNodes[findNodeOrException(nodeName)].propPosition;
        }  // end method

        Rsyn::Pin getAttachedPin(const NameType nodeName) const {
                return clsNodes[findNodeOrException(nodeName)].propPin;
        }  // end method

        void setNodePosition(const NameType nodeName, const DBUxy pos) {
                clsNodes[findNodeOrException(nodeName)].propPosition = pos;
        }  // end method

        void setNodePosition(const NameType nodeName, const DBU x,
                             const DBU y) {
                clsNodes[findNodeOrException(nodeName)].propPosition =
                    DBUxy(x, y);
        }  // end method

        void setAttachedPin(const NameType nodeName, Rsyn::Pin pin) {
                clsNodes[findNodeOrException(nodeName)].propPin = pin;
        }  // end method

        const std::vector<Node> &allNodes() const {
                return clsNodes;
        }  // end method

        const std::vector<Segment> &allSegments() const {
                return clsSegments;
        }  // end method
};         // end class

}  // end namespace

#endif
