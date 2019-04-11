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

#include "RCTreeExtractor.h"

namespace Rsyn {

RCTreeExtractor::RCTreeExtractor(ISPD13::SPEFInfo *info) {
        Rsyn::Session session;
        clsScenario = session.getService("rsyn.scenario");
        spefInfo = info;
}  // end method

// Returns the pin name in the format used in SPEF (cell:pinName or portName)
std::string RCTreeExtractor::getPinNameInSPEFFormat(Rsyn::Pin pin) {
        Rsyn::Instance instance = pin.getInstance();
        string pinName = pin.getName();
        string instName = instance.getName();
        return (instance.getType() == Rsyn::CELL) ? (instName + ":" + pinName)
                                                  : pinName;

}  // end method

// Maps pin name in SPEF format to Rsyn::Pin
void RCTreeExtractor::mapSPEFPinNameToRsynPin(Rsyn::Net net) {
        clsPinMap.clear();

        for (Rsyn::Pin pin : net.allPins()) {
                string pinName = getPinNameInSPEFFormat(pin);
                clsPinMap[pinName] = pin;
        }

}  // end method

// For a given node, returns its index used in the RC Tree descriptor
int RCTreeExtractor::getNodeIndex(const std::string &nodeName) {
        // The first node has name 1
        int name;
        if (!clsNodeMap[nodeName]) {
                name = clsNodeMap.size();
                clsNodeMap[nodeName] = name;
        } else {
                name = clsNodeMap[nodeName];
        }
        return name;
}  // end method

// Add a default value capacitance to nodes at which a capacitance
// was not specified.
void RCTreeExtractor::addDefaultCapacitance(int node, RCTreeDescriptor &dscp,
                                            bool useDefaultCapacitance) {
        if (useDefaultCapacitance) {
                int index = dscp.findNode(node);
                if (index != -1) {
                        if (dscp.getNode(index).totalCap == 0.0f) {
                                Number cap =
                                    (Number)Rsyn::Units::convertToInternalUnits(
                                        Rsyn::MEASURE_CAPACITANCE, 1.0,
                                        unitPrefixForCapacitance);
                                dscp.addCapacitor(node, cap);
                        }  // end if
                }          // end if
        }                  // end if

}  // end method

void RCTreeExtractor::extractRCTreeFromSPEF(SPEFNetModel netModel,
                                            Rsyn::Net net, Rsyn::RCTree &tree) {
        // TODO: read from SPEF file the unit used.
        UnitPrefix unitPrefixForResistance = Rsyn::KILO;
        UnitPrefix unitPrefixForCapacitance = Rsyn::FEMTO;

        const ISPD13::SpefNet &info =
            spefInfo->getNet(spefInfo->search(net.getName()));

        // Lumped Capacitance net model

        if (netModel == LUMPED_CAPACITANCE_NET_MODEL) {
                double parsedCapacitance = info.netLumpedCap;
                Number capacitance =
                    (Number)Rsyn::Units::convertToInternalUnits(
                        Rsyn::MEASURE_CAPACITANCE, parsedCapacitance,
                        unitPrefixForCapacitance);
                tree.setUserSpecifiedWireLoad(capacitance);
                return;
        }

        // RC Tree net model

        RCTreeDescriptor dscp;
        clsNodeMap.clear();

        mapSPEFPinNameToRsynPin(net);

        {  // builds the topology

                for (const ISPD13::SpefResistance &resistance :
                     info.resistances) {
                        int fromNode = getNodeIndex(resistance.fromNodeName.n1);
                        int toNode = getNodeIndex(resistance.toNodeName.n1);
                        double parsedResistance = resistance.resistance;
                        Number res =
                            (Number)Rsyn::Units::convertToInternalUnits(
                                Rsyn::MEASURE_RESISTANCE, parsedResistance,
                                unitPrefixForResistance);
                        dscp.addResistor(fromNode, toNode, res);
                }  // end for

                for (const ISPD13::SpefCapacitance &capacitance :
                     info.capacitances) {
                        string nodeName = capacitance.nodeName.n1;
                        int nodeIndex = getNodeIndex(nodeName);

                        double parsedCapacitance = capacitance.capacitance;
                        Number cap =
                            (Number)Rsyn::Units::convertToInternalUnits(
                                Rsyn::MEASURE_CAPACITANCE, parsedCapacitance,
                                unitPrefixForCapacitance);
                        dscp.addCapacitor(nodeIndex, cap);

                }  // end for

                dscp.applyDefaultNodeTag(RCTreeNodeTag(nullptr, 0, 0));

                for (const auto &node : clsNodeMap) {
                        if (clsPinMap[node.first]) {
                                Rsyn::Pin pin = clsPinMap[node.first];
                                RCTreeNodeTag &tag =
                                    dscp.getNodeTag(node.second);
                                tag.setPin(pin);
                        }  // end if

                        addDefaultCapacitance(node.second, dscp, false);

                }  // end for

                Rsyn::Pin driver = net.getAnyDriver();
                int root = clsNodeMap[getPinNameInSPEFFormat(driver)];

                // build the tree
                if (!tree.build(dscp, root, true))
                        std::cout << "Net: " << net.getName()
                                  << ": loop detected.\n";

        }  // end block

        // Set load capacitances
        setLoadCapacitances(tree);
        tree.updateDownstreamCap();

}  // end method

// Set the load capacitances
void RCTreeExtractor::setLoadCapacitances(Rsyn::RCTree &tree) {
        const int numRCTreeNodes = tree.getNumNodes();

        for (int i = 1; i < numRCTreeNodes; i++) {
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

}  // end method

}  // end namespace