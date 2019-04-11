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

#include "PhysicalDesignEx.h"
#include <Rsyn/PhysicalDesign>
#include <iostream>

// initializing an process

bool PhysicalDesignExample::run(const Rsyn::Json &params) {
        // Getting physical design object reference
        clsPhDesign = clsSession.getPhysicalDesign();
        // Getting design reference (netlist)
        clsDesign = clsSession.getDesign();
        // Getting top module reference
        clsTopModule = clsDesign.getTopModule();

        visitAllPhysicalInstances();

        visitAllPhysicalRows();

        assigningUserDefinedDataToRows();

        moveCells();

        visitAllPhysicalLayers();
        return true;
}  // end method

// -----------------------------------------------------------------------------

// Visiting all physical cell

void PhysicalDesignExample::visitAllPhysicalInstances() {
        // Visiting all instances in the top module
        std::cout << "Visiting all physical cells\n";
        int count = 0;
        for (Rsyn::Instance instance : clsTopModule.allInstances()) {
                // skipping non cell elements
                if (instance.getType() != Rsyn::CELL) continue;

                // changing instance inheritance to cell type
                Rsyn::Cell cell = instance.asCell();
                // Getting physical cell reference
                Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);

                // Printing cell name, position and boundaries.
                // For more methods, please see the physical cell header
                std::cout << "Cell name: " << instance.getName()
                          << " pos: " << phCell.getPosition()
                          << " boundaries: " << phCell.getBounds() << "\n";
                if (count == 100) {
                        std::cout << "breaking print instances\n";
                        break;
                }
                count++;
        }  // end for

        // Visiting all physical Ports
        count = 0;
        std::cout << "Visiting all physical ports\n";
        for (Rsyn::Instance instance : clsTopModule.allInstances()) {
                if (instance.getType() != Rsyn::PORT) continue;
                Rsyn::Port port = instance.asPort();
                Rsyn::PhysicalPort phPort = clsPhDesign.getPhysicalPort(port);
                std::cout << "Port name: " << port.getName()
                          << " pos: " << phPort.getPosition()
                          << " boundaries: " << phPort.getBounds() << "\n";
                if (count == 100) {
                        std::cout << "breaking print ports\n";
                        break;
                }
                count++;
        }  // end for

        // Visiting all physical Modules
        std::cout << "Visiting all physical modules\n";
        count = 0;
        for (Rsyn::Instance instance : clsTopModule.allInstances()) {
                if (instance.getType() != Rsyn::MODULE) continue;
                Rsyn::Module module = instance.asModule();
                Rsyn::PhysicalModule phModule =
                    clsPhDesign.getPhysicalModule(module);
                std::cout << "Module name: " << module.getName()
                          << " pos: " << phModule.getPosition()
                          << " boundaries: " << phModule.getBounds() << "\n";

                if (count == 100) {
                        std::cout << "breaking print modules\n";
                        break;
                }
                count++;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

// Visiting all physical rows

void PhysicalDesignExample::visitAllPhysicalRows() {
        // Iterating by all physical rows
        std::cout << "Visiting all physical rows\n";
        int count = 0;
        for (Rsyn::PhysicalRow phRow : clsPhDesign.allPhysicalRows()) {
                // Getting physical site reference
                Rsyn::PhysicalSite phSite = phRow.getPhysicalSite();
                // Getting enum physical site class
                Rsyn::PhysicalSiteClass phSiteClass = phSite.getClass();
                // Getting string related to the site class from an enum
                std::string siteClass = Rsyn::getPhysicalSiteClass(phSiteClass);
                // Getting enum to the site class from a string
                Rsyn::PhysicalSiteClass phSiteClassEqual =
                    Rsyn::getPhysicalSiteClass(siteClass);

                // Printing some row data.
                std::cout << "row " << phRow.getName()
                          << " bounds: " << phRow.getBounds()
                          << " site: " << phSite.getName()
                          << " siteClass: " << siteClass << "\n";
                if (count == 100) {
                        std::cout << "breaking print rows\n";
                        break;
                }
                count++;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void PhysicalDesignExample::assigningUserDefinedDataToRows() {
        // Declaring row attributes variable
        // Primitive values and objects may be associated to rows.
        Rsyn::PhysicalAttribute<Rsyn::PhysicalRow, std::string> rowAtts;
        // Initializing row attributes variable
        rowAtts = clsPhDesign.createPhysicalAttribute();

        int count = 0;
        // Associating user defined attributes to the rows;
        for (Rsyn::PhysicalRow phRow : clsPhDesign.allPhysicalRows()) {
                rowAtts[phRow] = "userDefineAtt_" + std::to_string(count);
                count++;
        }  // end for

        count = 0;
        for (Rsyn::PhysicalRow phRow : clsPhDesign.allPhysicalRows()) {
                // Accessing user defined data associated to rows
                std::string str = rowAtts[phRow];

                std::cout << "Rows: " << phRow.getName() << " userData: " << str
                          << "\n";
                if (count == 100) {
                        std::cout << "breaking print assigning user defined "
                                     "data to rows\n";
                        break;
                }
                count++;
        }  // end for

}  // end method

// -----------------------------------------------------------------------------

// Example how to move physical cells.
// If the cell is fixed, then it is not moved
void PhysicalDesignExample::moveCells() {
        for (Rsyn::Instance instance : clsTopModule.allInstances()) {
                // skipping non cell elements
                if (instance.getType() != Rsyn::CELL) continue;
                // changing instance inheritance to cell type
                Rsyn::Cell cell = instance.asCell();
                // Getting physical cell reference
                Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);
                // getting current cell position.
                DBUxy pos = phCell.getPosition();
                // the cells are going to be moved to 100 units to right (x+100)
                // and 100 units (y-100) to bottom.
                pos[X] += 100;
                pos[Y] -= 100;

                // moving physical cell
                // The only way to move cell is requesting to the physical
                // design.
                clsPhDesign.placeCell(phCell, pos);
        }  // end for

        // printing new position of physical cells
        int count = 0;
        for (Rsyn::Instance instance : clsTopModule.allInstances()) {
                // skipping non cell elements
                if (instance.getType() != Rsyn::CELL) continue;

                // changing instance inheritance to cell type
                Rsyn::Cell cell = instance.asCell();
                // Getting physical cell reference
                Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);

                // Printing cell name, position and boundaries.
                // For more methods, please see the physical cell header
                std::cout << "Cell name: " << instance.getName()
                          << " intial pos: " << phCell.getInitialPosition()
                          << " current pos: " << phCell.getPosition() << "\n";
                if (count == 100) {
                        std::cout << "breaking print moved cell positions\n";
                        break;
                }
                count++;
        }  // end for
}

void PhysicalDesignExample::visitAllPhysicalLayers() {
        // Visiting all routing physical layers
        for (Rsyn::PhysicalLayer phLayer : clsPhDesign.allPhysicalLayers()) {
                // skipping non routing types
                if (phLayer.getType() != Rsyn::PhysicalLayerType::ROUTING)
                        continue;
                std::cout << phLayer.getName()
                          << " direction: " << Rsyn::getPhysicalLayerDirection(
                                                   phLayer.getDirection())
                          << " type: "
                          << Rsyn::getPhysicalLayerType(phLayer.getType())
                          << "\n";
        }  // end for

        // Visiting all cut physical layers
        for (Rsyn::PhysicalLayer phLayer : clsPhDesign.allPhysicalLayers()) {
                // skipping non routing types
                if (phLayer.getType() != Rsyn::PhysicalLayerType::CUT) continue;
                std::cout << phLayer.getName() << " type: "
                          << Rsyn::getPhysicalLayerType(phLayer.getType())
                          << "\n";
        }  // end for
}  // end method

// -----------------------------------------------------------------------------