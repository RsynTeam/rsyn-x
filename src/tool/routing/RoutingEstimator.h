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

#ifndef RSYN_ROUTING_ESTIMATOR_H
#define RSYN_ROUTING_ESTIMATOR_H

#include <iostream>

#include "core/Rsyn.h"
#include "session/Service.h"
#include "phy/PhysicalDesign.h"
#include "RCTree.h"
#include "RoutingEstimationModel.h"
#include "RoutingExtractionModel.h"
#include "tool/scenario/Scenario.h"
#include "util/Stopwatch.h"

namespace Rsyn {

class Session;

class RoutingEstimator : public Service, public Rsyn::DesignObserver {
       public:
        // Indicates what type of update is required for a net. Add in order of
        // lowest priority meaning that a type with lower priority won't replace
        // one with a higher priority. For instance, the type "full" should
        // always
        // be the one with the highest priority.
        enum NetUpdateTypeEnum {
                NET_UPDATE_TYPE_INVALID = -1,

                NET_UPDATE_TYPE_DOWNSTREAM_CAP,
                NET_UPDATE_TYPE_FULL,

                NUM_NET_UPDATE_TYPES
        };

       private:
        // This nets automatically handles priorities among different types of
        // update. If one assign a lower priority update type than the current
        // one,
        // the new one is ignored. If the priority of the new update is greater,
        // the old one is replaced.
        class NetUpdateType {
               public:
                NetUpdateType() : type(NET_UPDATE_TYPE_INVALID) {}
                NetUpdateTypeEnum getType() const { return type; }

                void operator=(NetUpdateTypeEnum newType) {
                        if (newType > type) {
                                type = newType;
                        }  // end if
                }          // end method

               private:
                NetUpdateTypeEnum type;
        };  // end class

        Rsyn::Design design;
        Rsyn::Module module;  // top module

        Scenario *clsScenario = nullptr;

        RoutingEstimationModel *routingEstimationModel = nullptr;
        RoutingExtractionModel *routingExtractionModel = nullptr;

        bool clsFullUpdateAlreadyPerformed = false;
        std::map<Rsyn::Net, NetUpdateType> clsDirtyNets;
        Stopwatch clsStopwatchUpdateSteinerTrees;

        struct RoutingNet {
                RCTree rctree;
                DBU wirelength;
        };  // end struct

        DBU clsTotalWirelength;

        Rsyn::Attribute<Rsyn::Net, RoutingNet> clsRoutingNets;

       public:
        virtual void start(const Rsyn::Json &params);
        virtual void stop();

        void setRoutingEstimationModel(RoutingEstimationModel *model) {
                routingEstimationModel = model;
        }
        void setRoutingExtractionModel(RoutingExtractionModel *model) {
                routingExtractionModel = model;
        }

        virtual void onPostNetCreate(Rsyn::Net net) override;

        virtual void onPreNetRemove(Rsyn::Net net) override;

        virtual void onPostCellRemap(Rsyn::Cell cell,
                                     Rsyn::LibraryCell oldLibraryCell) override;

        virtual void onPostPinConnect(Rsyn::Pin pin) override;

        virtual void onPrePinDisconnect(Rsyn::Pin pin) override;

        virtual void onPostInstanceMove(Rsyn::Instance instance) override;

        Number getLocalWireResPerUnitLength() const {
                return routingExtractionModel->getLocalWireResPerUnitLength();
        }
        Number getLocalWireCapPerUnitLength() const {
                return routingExtractionModel->getLocalWireCapPerUnitLength();
        }

        const Rsyn::RCTree &getRCTree(Rsyn::Net net) const {
                return clsRoutingNets[net].rctree;
        }  // end method

        // Timer needs a non-cost tree, for now let's give one to it.
        Rsyn::RCTree &getRCTree(Rsyn::Net net) {
                return clsRoutingNets[net].rctree;
        }  // end method

        void updateRoutingOfNet(Rsyn::Net net,
                                const NetUpdateTypeEnum updateType);
        void updateRoutingFull();
        void updateRouting();

        void dirtyInstance(Rsyn::Instance instance,
                           const NetUpdateTypeEnum updateType) {
                for (Rsyn::Pin pin : instance.allPins()) {
                        Rsyn::Net net = pin.getNet();
                        if (net) {
                                clsDirtyNets[net] = updateType;
                        }  // end if
                }          // end for
        }                  // end method

        void dirtyNet(Rsyn::Net net, const NetUpdateTypeEnum updateType) {
                clsDirtyNets[net] = updateType;
        }  // end method

        DBU getTotalWirelength() const {
                return clsTotalWirelength;
        }  // end method

        DBU getNetWirelength(Rsyn::Pin pin, bool skipClock = true) const {
                if (!pin.isConnected()) return 0;
                return getNetWirelength(pin.getNet(), skipClock);
        }  // end method

        DBU getNetWirelength(Rsyn::Net net, bool skipClock = true) const {
                if (net.getNumPins() < 2 ||
                    (net == clsScenario->getClockNet() && skipClock))
                        return 0;
                const RoutingNet &timingNet = clsRoutingNets[net];
                return timingNet.wirelength;
        }  // end method

        // TODO: Move the following methods elsewhere.

        // Return the tree node position where a pin is connected to the routing
        // tree.
        DBUxy getSteinerPointer(Rsyn::Net net, Rsyn::Pin pin) const;

        // Return the node index where a pin is connected to the routing tree.
        int getSteinerPointerIndex(const RCTree &rcTree, Rsyn::Pin pin) const;

        // Return the index of the node to which the pin connects to the RC
        // tree.
        // Returns -1 if the pin is not connected to the tree.
        int getRCTreeConnectingNodeIndex(Rsyn::Net net, Rsyn::Pin pin) const {
                return getRCTreeConnectingNodeIndex(getRCTree(net), pin);
        }  // end method

        int getRCTreeConnectingNodeIndex(const RCTree &rcTree,
                                         Rsyn::Pin pin) const;

        // Return the wire capacitance at this node due to the wire segment
        // connecting this node to its parent.
        Number getWireCapToParentNode(const RCTree &tree,
                                      const int nodeIndex) const {
                const RCTree::Node &node = tree.getNode(nodeIndex);
                if (node.propParent == -1) return 0;

                const RCTreeNodeTag &tagNode = tree.getNodeTag(nodeIndex);
                const RCTreeNodeTag &tagParent =
                    tree.getNodeTag(node.propParent);

                const DBU distance = std::abs(tagNode.x - tagParent.x) +
                                     std::abs(tagNode.y - tagParent.y);

                return (getLocalWireCapPerUnitLength() * distance) / 2;
        }  // end method

        // Return the downstream capacitance at a node ignoring the wire
        // capacitance
        // due to the parent (driver) wire.
        EdgeArray<Number> getDownstreamCapacitanceIgnoringParentWire(
            const RCTree &tree, const int nodeIndex) const {
                const RCTree::Node &node = tree.getNode(nodeIndex);
                return node.getDownstreamCap() -
                       getWireCapToParentNode(tree, nodeIndex);
        }  // end method

        void resetRuntime() {
                clsStopwatchUpdateSteinerTrees.reset();
        }  // end method

        const Stopwatch &getUpdateSteinerTreeRuntime() const {
                return clsStopwatchUpdateSteinerTrees;
        }  // end method

};  // end class

}  // end namespace

#endif
