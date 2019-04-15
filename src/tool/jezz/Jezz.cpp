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

/* Copyright 2014-2017 Rsyn
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

#include <cassert>
#include <algorithm>
#include <limits>
#include "Jezz.h"
#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "util/Bounds.h"
#include "util/StreamStateSaver.h"
#include "util/Stepwatch.h"

// -----------------------------------------------------------------------------

// Static Member Initialization

const Jezz::ListLink Jezz::REVERSE_LIST_LINK[2] = {Jezz::TAIL, Jezz::HEAD};

// -----------------------------------------------------------------------------

Jezz::Jezz() {
        clsJezzInitialized = false;
        clsJezzTieBreak = false;
        clsInitialized = false;
        clsEnableDebugMessages = false;
}  // end constructor

// -----------------------------------------------------------------------------

void Jezz::start(const Rsyn::Json &params) {
        Rsyn::Session session;

        clsDesign = session.getDesign();
        clsModule = session.getTopModule();
        clsPhysicalDesign = session.getPhysicalDesign();

        initJezz();

        // Observe changes in the design.
        clsDesign.registerObserver(this);

        {  // Store solution
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("storeJezzSolution");
                dscp.setDescription(
                    "Stores the current placement solution under a given ID.");

                dscp.addPositionalParam("id", ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Identifier of the solution.");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string id = command.getParam("id");
                            jezz_storeSolution(id);
                    });
        }  // end block

        {  // Restore solution
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("restoreJezzSolution");
                dscp.setDescription(
                    "Restores a previous placement solution stored under a "
                    "given ID.");

                dscp.addPositionalParam("id", ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Identifier of the solution.");

                dscp.addNamedParam("full", ScriptParsing::PARAM_TYPE_BOOLEAN,
                                   ScriptParsing::PARAM_SPEC_OPTIONAL,
                                   "Determines whether to perform a full or "
                                   "incremental restore.",
                                   "false");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string id = command.getParam("id");
                            const bool full = command.getParam("full");

                            if (full)
                                    jezz_restoreSolutionFull(id);
                            else
                                    jezz_restoreSolutionIncremental(id);
                    });
        }  // end block

        {  // legalize
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("legalize");
                dscp.setDescription(
                    "Legalizes the current placement solution.");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            Stepwatch leg("Legalizing circuit", false);
                            jezz_Legalize();
                            leg.finish();
                            clsRuntime += leg.getElapsedTime();
                    });
        }  // end block

        {  // legalizeCell
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("legalizeCell");
                dscp.setDescription("Legalizes the position of a given cell.");

                dscp.addPositionalParam("cellName",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Name of the target cell.");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string cellName =
                                command.getParam("cellName");
                            const Rsyn::Instance cell =
                                clsDesign.findCellByName(cellName);

                            if (!cell) {
                                    std::cout << "[ERROR] Cell \"" << cellName
                                              << "\" not found.\n";
                                    return;
                            }  // end if

                            jezz_LegalizeNode(getJezzNode(cell));
                    });
        }  // end block

        {  // report
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("jezzReport");
                dscp.setDescription("Reports the final results solution.");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            reportFinalResults();
                    });
        }  // end block

        {  // report average cell displacement (ACD)
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("jezzReportAcd");
                dscp.setDescription("Report Average Cell Displacement (ACD).");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            reportAverageCellDisplacement();
                    });
        }  // end block
}  // end method

// -----------------------------------------------------------------------------

void Jezz::stop() {}  // end method

// -----------------------------------------------------------------------------

void Jezz::onPostCellRemap(Rsyn::Cell cell, Rsyn::LibraryCell oldLibraryCell) {
        JezzNode *jezzNode = getJezzNode(cell);
        jezz_dp_RemoveNode(jezzNode);

        Rsyn::PhysicalCell physicalCell =
            clsPhysicalDesign.getPhysicalCell(cell);
        jezzNode->w = jezz_SnapSizeX(physicalCell.getWidth());
}  // end method

// -----------------------------------------------------------------------------

void Jezz::onPostInstanceCreate(Rsyn::Instance instance) {
        Jezz::JezzNode *jezzNode = nullptr;

        Rsyn::Cell cell =
            instance
                .asCell();  // TODO: hack, assuming that the instance is a cell
        if (cell.isPort()) {
                return;
        }  // end if

        Rsyn::PhysicalCell phCell = clsPhysicalDesign.getPhysicalCell(cell);

        if (cell.isMacroBlock() || cell.isFixed()) {
                if (phCell.hasLayerBounds()) {
                        Rsyn::PhysicalLibraryCell phLibCell =
                            clsPhysicalDesign.getPhysicalLibraryCell(cell);
                        for (const Bounds &obs :
                             phLibCell.allLayerObstacles()) {
                                Bounds rect = obs;
                                DBUxy lower = obs.getCoordinate(LOWER);
                                rect.moveTo(phCell.getPosition() + lower);
                                jezz_DefineObstacle(
                                    rect[LOWER][X], rect[LOWER][Y],
                                    rect[UPPER][X], rect[UPPER][Y]);
                        }  // end for
                } else {
                        const Bounds &rect = phCell.getBounds();
                        jezz_DefineObstacle(rect[LOWER][X], rect[LOWER][Y],
                                            rect[UPPER][X], rect[UPPER][Y]);
                }  // end else
        } else {
                const DBUxy lower = phCell.getCoordinate(LOWER);
                const DBUxy upper = phCell.getCoordinate(UPPER);
                jezzNode = jezz_DefineNode(cell, lower[X], lower[Y], upper[X],
                                           upper[Y]);
        }  // end if-else

        clsMapInstancesToJezzNodes[instance] = jezzNode;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::onPostInstanceMove(Rsyn::Instance instance) {
        if (instance.getType() == Rsyn::CELL) {
                JezzNode *jezzNode = getJezzNode(instance);
                if (jezzNode) {
                        const DBUxy position = instance.getPosition();
                        jezz_dp_RemoveNode(jezzNode);
                        jezz_dp_UpdateReferencePosition(jezzNode, position.x,
                                                        position.y);
                }  // end if
        }          // end if
}  // end method

// -----------------------------------------------------------------------------

void Jezz::initJezz() {
        if (clsInitialized) {
                std::cout
                    << "[ERROR] Trying to re-initialize a Jezz legalizer.\n";
                exit(1);
        }  // end if

        // Create a new Jezz instance.
        clsInitialized = true;
        clsInitialHpwl = clsPhysicalDesign.getHPWL();
        clsRuntime = 0.0;
        Stepwatch initJezz("Init Jezz", false);

        // Create an attribute to map instances to jezz nodes.
        clsMapInstancesToJezzNodes = clsDesign.createAttribute();

        // Initialize Jezz Legalizer
        UpdatePositionCallback updatePositionCallback = [&](
            Rsyn::Cell cell, const DBU x, const DBU y,
            Rsyn::PhysicalOrientation orient) {
                Rsyn::PhysicalCell physicalCell =
                    clsPhysicalDesign.getPhysicalCell(cell);

                // Update session.
                if (x != physicalCell.getCoordinate(LOWER, X) ||
                    y != physicalCell.getCoordinate(LOWER, Y)) {
                        clsPhysicalDesign.placeCell(physicalCell, x, y, orient,
                                                    true);
                        clsPhysicalDesign.notifyInstancePlaced(
                            physicalCell.getInstance(), this);
                }  // end if

                // Set the reference cell position as the new legalized one.
                Jezz::JezzNode *jezzNode =
                    clsMapInstancesToJezzNodes[physicalCell.getInstance()];
                jezz_dp_UpdateReferencePosition(jezzNode, x, y);
        };

        clsJezzUpdatePositionCallback = updatePositionCallback;

        clsJezzSiteWidth = clsPhysicalDesign.getRowSiteWidth();
        clsJezzRowHeight = clsPhysicalDesign.getRowHeight();
        clsJezzSlotLengthInUserUnits = (4 * clsJezzRowHeight);
        clsJezzSlotLengthInJezzUnits =
            jezz_SnapSizeX(clsJezzSlotLengthInUserUnits);

        // Make a clean-up before starting.
        jezz_Cleanup();

        // Create a jezz row per design row.
        jezz_InitRows();

        // Flag Jezz as initialized.
        clsJezzInitialized = true;
        clsJezzIncrementalMode = false;

        // Define obstacles.
        initJezz_Obstacles();

        // Define movable nodes.
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Jezz::JezzNode *jezzNode = nullptr;

                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                if (cell.isPort() || cell.isMacroBlock() || cell.isFixed()) {
                        continue;
                }  // end if

                Rsyn::PhysicalCell phCell =
                    clsPhysicalDesign.getPhysicalCell(cell);

                const DBUxy lower = phCell.getCoordinate(LOWER);
                const DBUxy upper = phCell.getCoordinate(UPPER);
                jezzNode = jezz_DefineNode(cell, lower[X], lower[Y], upper[X],
                                           upper[Y]);

                clsMapInstancesToJezzNodes[instance] = jezzNode;
        }  // end for

        initJezz.finish();
        clsRuntime += initJezz.getElapsedTime();

        //	// Debug
        //	for (const Jezz::JezzNode *node : clsJezz->allNodes()) {
        //		cout << "jezz node: "
        //				<< "x=" << std::setw(8) <<
        // clsJezz->jezz_UnsnapPositionX(node->x) << " "
        //				<< "y=" << std::setw(8) <<
        // node->row->origin_y
        //<< " "
        //				<< "ws=" << node->ws << " "
        //				<< "fixed=" << node->fixed << " ";
        //
        //		cout << "cell=" <<
        //((node->reference)?node->reference.getName():"<null>");
        //		cout << "\n";
        //	} // end for

}  // end method

// -----------------------------------------------------------------------------

void Jezz::initJezz_Obstacles() {
        struct Obstacle {
                int x = 0;
                int w = 0;
                Rsyn::Instance instance = nullptr;

                bool operator<(const Obstacle &right) const {
                        return x < right.x;
                }  // end for

                Obstacle() {}

                Obstacle(const DBU x, const DBU w, Rsyn::Instance instance)
                    : x(x), w(w), instance(instance) {}
        };

        //	Rsyn::PhysicalModule phModule =
        // clsPhysicalDesign.getPhysicalModule(clsModule);

        const int numRows = clsPhysicalDesign.getNumRows();
        // const Bounds &coreBounds = phModule.getBounds();

        // Vector to store obstacles per row.
        std::vector<std::vector<Obstacle>> rows(numRows);

        // Add an obstacle.
        auto pushObstacle = [&](const DBU xmin, const DBU ymin, const DBU xmax,
                                const DBU ymax, Rsyn::Instance instance) {
                const int row0 = jezz_GetRowIndexBounded(ymin, TOP);
                const int row1 = jezz_GetRowIndexBounded(ymax, BOTTOM);
                const int x = jezz_SnapPositionX(xmin);
                const int w = jezz_SnapSizeX(xmin, xmax);
                for (int row = row0; row <= row1; row++) {
                        rows[row].push_back(Obstacle(x, w, instance));
                }  // end for

                if (clsEnableDebugMessages &&
                    ((ymin - ymax) % clsJezzRowHeight)) {
                        std::cout << "WARNING: Obstacle height is not integer "
                                     "multiple of the row height ("
                                  << "ymin=" << ymin << " "
                                  << "ymax=" << ymax << " "
                                  << "height=" << (ymax - ymin) << " "
                                  << "row_height=" << clsJezzRowHeight << ")\n";
                }  // end else
        };

        // Currently Jezz does not support rows starting/ending at different
        // xmin,
        // xmax. Therefore we define blockages to mimic uneven rows.
        for (const Rsyn::PhysicalRow phRow :
             clsPhysicalDesign.allPhysicalRows()) {
                const Bounds &bounds = phRow.getBounds();

                if (bounds[LOWER][X] > clsJezzBounds[LOWER][X]) {
                        pushObstacle(clsJezzBounds[LOWER][X], bounds[LOWER][Y],
                                     bounds[LOWER][X], bounds[UPPER][Y],
                                     nullptr);
                }  // end if

                if (bounds[UPPER][X] < clsJezzBounds[UPPER][X]) {
                        pushObstacle(bounds[UPPER][X], bounds[LOWER][Y],
                                     clsJezzBounds[UPPER][X], bounds[UPPER][Y],
                                     nullptr);
                }  // end if
        }          // end for

        // Define obstacles based on macro and fixed cells.
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                if (instance.isPort()) {
                        continue;
                }  // end if

                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                Rsyn::PhysicalCell phCell =
                    clsPhysicalDesign.getPhysicalCell(cell);

                if (cell.isMacroBlock() || cell.isFixed()) {
                        if (phCell.hasLayerBounds()) {
                                Rsyn::PhysicalLibraryCell phLibCell =
                                    clsPhysicalDesign.getPhysicalLibraryCell(
                                        cell);
                                for (const Bounds &obs :
                                     phLibCell.allLayerObstacles()) {
                                        Bounds rect = obs;
                                        DBUxy lower = obs.getCoordinate(LOWER);
                                        rect.moveTo(phCell.getPosition() +
                                                    lower);
                                        pushObstacle(rect[LOWER][X],
                                                     rect[LOWER][Y],
                                                     rect[UPPER][X],
                                                     rect[UPPER][Y], instance);
                                }  // end for
                        } else {
                                const Bounds &rect = phCell.getBounds();
                                if (rect.overlap(clsJezzBounds)) {
                                        pushObstacle(rect[LOWER][X],
                                                     rect[LOWER][Y],
                                                     rect[UPPER][X],
                                                     rect[UPPER][Y], instance);
                                }  // end if
                        }          // end else
                }                  // end if
        }                          // end for

        // Sort obstacles by x.
        for (std::vector<Obstacle> &row : rows) {
                std::sort(row.begin(), row.end());
        }  // end for

        // Define obstacles merging overlapping ones.
        for (int i = 0; i < numRows; i++) {
                const std::vector<Obstacle> &row = rows[i];

                int previousX = 0;
                Rsyn::Instance previousInstance = nullptr;
                const int numElements = row.size();
                for (int k = 0; k < numElements; k++) {
                        const Obstacle &obs = row[k];
                        const int x0 = std::max(previousX, obs.x);
                        const int x1 = obs.x + obs.w;
                        const int w = x1 - x0;
                        if (w > 0) {
                                jezz_DefineObstacleAtRowUsingJezzCoordinates(
                                    i, x0, w);
                        }  // end if

                        // Warn user about overlap.
                        if (clsEnableDebugMessages && (obs.x < previousX)) {
                                // An obstacle with null instance is an obstacle
                                // create to
                                // handle uneven row (starting/ending
                                // after/before the core
                                // bounds).
                                std::cout
                                    << "INFO: Obstacle overlap at row " << i
                                    << " ("
                                    << "previous=" << previousInstance.getName()
                                    << ", "
                                    << "current=" << obs.instance.getName()
                                    << ").\n";
                        }  // end if

                        // Keep track of previous instance.
                        if (x1 > previousX) {
                                // The current node maybe enclosed by the
                                // previous one. Only
                                // change previous node if it does not enclose
                                // the current node.
                                previousX = x1;
                                previousInstance = obs.instance;
                        }  // end if
                }          // end
        }                  // end for
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_InitRows() {
        clsJezzBounds[LOWER][X] = std::numeric_limits<DBU>::max();
        clsJezzBounds[LOWER][Y] = std::numeric_limits<DBU>::max();
        clsJezzBounds[UPPER][X] = -std::numeric_limits<DBU>::max();
        clsJezzBounds[UPPER][Y] = -std::numeric_limits<DBU>::max();

        for (Rsyn::PhysicalRow phRow : clsPhysicalDesign.allPhysicalRows()) {
                const Bounds &bds = phRow.getBounds();
                clsJezzBounds[LOWER][X] =
                    std::min(clsJezzBounds[LOWER][X], bds[LOWER][X]);
                clsJezzBounds[LOWER][Y] =
                    std::min(clsJezzBounds[LOWER][Y], bds[LOWER][Y]);
                clsJezzBounds[UPPER][X] =
                    std::max(clsJezzBounds[UPPER][X], bds[UPPER][X]);
                clsJezzBounds[UPPER][Y] =
                    std::max(clsJezzBounds[UPPER][Y], bds[UPPER][Y]);
        }  // end for

        clsJezzNumRows = clsPhysicalDesign.getNumRows();

        double numSites = static_cast<double>(clsJezzBounds.computeLength(X));
        numSites /= clsJezzSiteWidth;
        numSites = std::ceil(numSites);

        clsJezzNumSites = static_cast<DBU>(numSites);

        // Defensive programming. Call clear to ensure that resize will call the
        // element constructors again. Since clear does not free the memory,
        // this
        // should not mess with row pointers stored elsewhere.
        clsJezzRows.clear();
        clsJezzRows.resize(clsJezzNumRows);
        for (JezzRow &jezzRow : clsJezzRows) {
                jezzRow.reset();
        }  // end for

        int i = 0;
        for (Rsyn::PhysicalRow phRow : clsPhysicalDesign.allPhysicalRows()) {
                JezzRow &jezzRow = clsJezzRows[i];
                jezzRow.origin_x = clsJezzBounds[LOWER][X];
                jezzRow.origin_y =
                    clsJezzBounds[LOWER][Y] + i * clsJezzRowHeight;
                jezzRow.x = jezz_SnapPositionX(jezzRow.origin_x);
                jezzRow.y = i;
                jezzRow.w = clsJezzNumSites;
                jezzRow.clsOrientation = phRow.getSiteOrientation();

                // Define some useful values to the guts of root node so that we
                // don't
                // need to deal with special cases as for instance define the x
                // position
                // of the first node in the list by using its previous node xmax
                // position.
                JezzNodeGuts &rootGuts = jezzRow.getRootGuts();
                rootGuts.fixed = true;
                rootGuts.x = jezzRow.x;
                rootGuts.w = 0;
                rootGuts.row = &jezzRow;

                // Create a whitespace node which fulfills all the row.
                JezzNode *jezzNode = jezz_CreateWhitespaceNode();
                jezzNode->w = jezzRow.w;
                jezzNode->x = jezzRow.x;
                jezzNode->row = &jezzRow;

                jezzRow.insert(TAIL, jezzNode);

                // Initialize cache slots.
                const int numSlots = roundedUpIntegralDivision(
                    jezzRow.width(), clsJezzSlotLengthInJezzUnits);
                jezzRow.slots.assign(numSlots, nullptr);
                ++i;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_DeleteWhitespaceNodes() {
        clsJezzDeletedWhitespaces.clear();
        for (JezzNode &whitespace : clsJezzWhitespaces) {
                jezz_DeleteWhitespaceNode(&whitespace);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

bool Jezz::jezz_LegalizeNode(JezzNode *jezzNode) {
        const int numRows = clsJezzRows.size();

        const DBU x0 = jezzNode->refx;
        const DBU y0 = jezzNode->refy;

        // BUG FIX: jezz_InserNode requires also that the x position to be set
        // to
        // the target x.
        // TODO: Make jezz_InsertNode use always the x position inside the Jezz
        // node
        // and remove the x parameter.
        jezzNode->x = jezz_SnapPositionX(x0);

        bool overflow;

        int bestRow = -1;
        double bestCost = +std::numeric_limits<double>::max();
        double smallestMaxDisplacementCost =
            +std::numeric_limits<double>::max();

        double currMaxDisplacementCost;

        int initialRow = jezz_GetRowIndex(y0);
        if (initialRow < 0) {
                initialRow = 0;
        } else if (initialRow >= numRows) {
                initialRow = numRows - 1;
        }  // end else-if

        for (int row = initialRow; row >= 0; row--) {
                const DBU y1 = jezz_GetRowY(row);
                const double costy = jezzNode->weight * std::abs(y1 - y0);
                const double costx =
                    clsJezzSiteWidth *
                    jezz_InsertNode(&clsJezzRows[row], jezzNode, jezzNode->x,
                                    true, overflow, currMaxDisplacementCost);
                const double cost = costy + costx;

                if (!overflow) {
                        if (cost < bestCost) {
                                bestRow = row;
                                bestCost = cost;
                                smallestMaxDisplacementCost =
                                    currMaxDisplacementCost;
                        } else if (clsJezzTieBreak && cost == bestCost) {
                                if (currMaxDisplacementCost <
                                    smallestMaxDisplacementCost) {
                                        bestRow = row;
                                        smallestMaxDisplacementCost =
                                            currMaxDisplacementCost;
                                }  // end if
                        } else {
                                break;
                        }  // end else
                }          // end if
        }                  // end for

        for (int row = initialRow + 1; row < numRows; row++) {
                const DBU y1 = jezz_GetRowY(row);
                const double costy = jezzNode->weight * std::abs(y1 - y0);
                const double costx =
                    clsJezzSiteWidth *
                    jezz_InsertNode(&clsJezzRows[row], jezzNode, jezzNode->x,
                                    true, overflow, currMaxDisplacementCost);
                const double cost = costy + costx;

                if (!overflow) {
                        if (cost < bestCost) {
                                bestRow = row;
                                bestCost = cost;
                                smallestMaxDisplacementCost =
                                    currMaxDisplacementCost;
                        } else if (clsJezzTieBreak && cost == bestCost) {
                                if (currMaxDisplacementCost <
                                    smallestMaxDisplacementCost) {
                                        bestRow = row;
                                        smallestMaxDisplacementCost =
                                            currMaxDisplacementCost;
                                }  // end if
                        } else {
                                break;
                        }  // end else
                }          // end if
        }                  // end for

        if (bestRow != -1) {
                jezz_InsertNode(&clsJezzRows[bestRow], jezzNode, jezzNode->x,
                                false, overflow, smallestMaxDisplacementCost);
        } else {
                // In this case, there's no more space to insert this node at
                // least
                // keep it with the approximately the same x.
                std::cout << "#";
        }  // end else

        return jezz_dp_IsLegalized(jezzNode);
}  // end method

// -----------------------------------------------------------------------------

bool Jezz::jezz_Legalize() {
        const int numNodes = clsJezzNodes.size();

        // Sort cell by x.
        std::vector<std::pair<DBU, JezzNode *>> sortedCells;
        sortedCells.reserve(numNodes);
        for (int i = 0; i < numNodes; i++) {
                JezzNode *jezzNode = &clsJezzNodes[i];
                if (!jezzNode->isDeleted() && !jezz_dp_IsLegalized(jezzNode))
                        sortedCells.push_back(
                            std::make_pair(jezzNode->refx, jezzNode));
        }  // end for each
        std::sort(sortedCells.begin(), sortedCells.end());

        bool success = true;

        const int numNonLegalizedNodes = sortedCells.size();
        for (int i = 0; i < numNonLegalizedNodes; i++) {
                JezzNode *jezzNode = sortedCells[i].second;
                success &= jezz_LegalizeNode(jezzNode);
        }  // end for

        // Update cache.
        jezz_UpdateCacheAll();

        // Indicates that we entered the incremental mode.
        clsJezzIncrementalMode = true;

        // Returns true if all cells were successfully legalized.
        return success;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_Cleanup() {
        clsJezzRows.clear();
        clsJezzNodes.clear();
        clsJezzObstacles.clear();

        clsJezzDeletedNodes.clear();
        clsJezzDeletedWhitespaces.clear();

        clsJezzBounds.clear();
        clsJezzInitialized = false;
        clsJezzIncrementalMode = false;
}  // end method

// -----------------------------------------------------------------------------

Jezz::JezzNode *Jezz::jezz_DefineNode(Rsyn::Cell cell, const DBU xmin,
                                      const DBU ymin, const DBU xmax,
                                      const DBU ymax) {
        const int x = jezz_SnapPositionX(xmin);
        const int w = jezz_SnapSizeX(xmin, xmax);
        const int h = 1;  // jezz_SnapSizeY(ymin, ymax);

        if (h != 1) {
                assert(false);
        }  // end if

        JezzNode *jezzNode;

        if (clsJezzDeletedNodes.empty()) {
                clsJezzNodes.resize(clsJezzNodes.size() + 1);
                jezzNode = &clsJezzNodes.back();
                jezzNode->id = clsJezzNodes.size() - 1;
        } else {
                jezzNode = clsJezzDeletedNodes.front();
                clsJezzDeletedNodes.pop_front();
                jezzNode->deleted = false;
        }  // end else

        jezzNode->obstacle = false;
        jezzNode->fixed = false;
        jezzNode->reference = cell;
        jezzNode->x = x;
        jezzNode->w = w;
        jezzNode->refx = xmin;
        jezzNode->refy = ymin;
        jezzNode->snapped_refx = jezz_SnapPositionX(jezzNode->refx);
        jezzNode->snnaped_refy = jezz_SnapPositionY(jezzNode->refy);

        return jezzNode;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_DefineObstacleAtRowUsingJezzCoordinates(const int row,
                                                        const int x,
                                                        const int w) {
        JezzRow *jezzRow = &clsJezzRows[row];

        clsJezzObstacles.resize(clsJezzObstacles.size() + 1);

        JezzNode *jezzNode = &clsJezzObstacles.back();
        jezzNode->obstacle = true;
        jezzNode->fixed = true;
        jezzNode->x = std::max(x, jezzRow->x);
        jezzNode->w = w;
        jezzNode->refx = jezz_UnsnapPositionX(x);
        jezzNode->refy = jezz_UnsnapPositionY(row);
        jezzNode->snapped_refx = jezz_SnapPositionX(jezzNode->refx);
        jezzNode->snnaped_refy = jezz_SnapPositionY(jezzNode->refy);

        if (jezzNode->xmax() > jezzRow->xmax())
                jezzNode->w = jezzRow->xmax() - jezzNode->x;

        bool overflow;
        double smallestMaxDisplacement;
        jezz_InsertNode(jezzRow, jezzNode, jezzNode->x, false, overflow,
                        smallestMaxDisplacement);
        if (overflow) {
                std::cout
                    << "[WARNING] Jezz: Overflow when inserting a obstacle.\n";
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_DefineObstacle(const DBU xmin, const DBU ymin, const DBU xmax,
                               const DBU ymax) {
        const int x = jezz_SnapPositionX(xmin);
        const int w = jezz_SnapSizeX(xmin, xmax);
        const int h = jezz_SnapSizeY(ymin, ymax);

        const int row0 = jezz_GetRowIndexBounded(ymin);
        const int row1 = std::min(clsJezzNumRows - 1, row0 + h);
        for (int row = row0; row < row1; row++) {
                JezzRow *jezzRow = &clsJezzRows[row];

                clsJezzObstacles.resize(clsJezzObstacles.size() + 1);

                JezzNode *jezzNode = &clsJezzObstacles.back();
                jezzNode->obstacle = true;
                jezzNode->fixed = true;
                jezzNode->x = std::max(x, jezzRow->x);
                jezzNode->w = w;
                jezzNode->refx = xmin;
                jezzNode->refy = ymin;
                jezzNode->snapped_refx = jezz_SnapPositionX(jezzNode->refx);
                jezzNode->snnaped_refy = jezz_SnapPositionY(jezzNode->refy);

                if (jezzNode->xmax() > jezzRow->xmax())
                        jezzNode->w = jezzRow->xmax() - jezzNode->x;

                bool overflow;
                double smallestMaxDisplacement;
                jezz_InsertNode(jezzRow, jezzNode, jezzNode->x, false, overflow,
                                smallestMaxDisplacement);
                if (overflow) {
                        std::cout << "[WARNING] Jezz: Overflow when inserting "
                                     "a obstacle.\n";
                }  // end if
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_RemoveNode(JezzNode *jezzNode) {
        jezz_dp_RemoveNode(jezzNode);
        jezzNode->deleted = true;
        jezzNode->reset();
        clsJezzDeletedNodes.push_back(jezzNode);
}  // end method

// -----------------------------------------------------------------------------
// Check if the a point x is inside the bounds of a jezz node.

bool Jezz::jezz_Overlap(const int x, JezzNode *jezzNode) {
        // Note that we use "<" instead of "<=" in xmax is not the index of the
        // last
        // site occupied by the node, but it is the first slot of the next node.
        //
        // -----------------
        // |#|#|#|#| | | | | width = 4
        // -----------------
        // ^       ^
        // xmin    xmax = xmin + width (xmax is the xmin of the next node)
        // xmin => fist site
        // xmax => last site + 1

        return (x >= jezzNode->xmin() && x < jezzNode->xmax());
}  // end method

// -----------------------------------------------------------------------------

bool Jezz::jezz_Enclosed(JezzNode *jezzNodeOutter, JezzNode *jezzNodeInner) {
        return (jezzNodeInner->xmin() >= jezzNodeOutter->xmin() &&
                jezzNodeInner->xmax() <= jezzNodeOutter->xmax());
}  // end method

// -----------------------------------------------------------------------------

bool Jezz::jezz_ValidCachedNode(JezzRow *jezzRow, JezzNode *jezzNode) {
        return jezzNode && jezzNode->row == jezzRow;
}  // end method

// -----------------------------------------------------------------------------

int Jezz::jezz_GetSlot(JezzRow *jezzRow, const int x) {
        if (x < jezzRow->xmin()) return -1;
        if (x >= jezzRow->xmax()) return -1;
        return (x - jezzRow->xmin()) / clsJezzSlotLengthInJezzUnits;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_UpdateCache(JezzNode *jezzNode) {
        JezzRow *jezzRow = jezzNode->row;

        const int index = jezz_GetSlot(jezzRow, jezzNode->x);
        if (index != -1) {
                jezzRow->slots[index] = jezzNode;
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_UpdateCacheAll() {
        for (JezzNode *jezzNode : allNodes()) {
                jezz_UpdateCache(jezzNode);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

Jezz::JezzNode *Jezz::jezz_FindOverlappingNodeCached(JezzRow *jezzRow,
                                                     const int x) {
        JezzNode *jezzNode = nullptr;

        // Check if the node is outside the row bounds...
        if (x < jezzRow->xmin()) {
                return &jezzRow
                            ->root;  // root is ok here, should not be the head.
        } else if (x >= jezzRow->xmax()) {
                return jezzRow->tail();
        }  // end else

        // Get the cache slot.
        const int index = jezz_GetSlot(jezzRow, x);

        if (index != -1) {
                // Use the cached node as the starting point for the sequential
                // search.
                jezzNode = jezzRow->slots[index];
        }  // end if

        // Check if we have a valid cached node. We may not have a cached node
        // available or it may be not valid anymore (for instance the node has
        // been mode to another row).
        if (!jezz_ValidCachedNode(jezzRow, jezzNode)) {
                // Look at adjacent slots.
                int i0 = index - 1;
                int i1 = index + 1;

                bool stop = false;

                const int numSlots = jezzRow->slots.size();
                while (!stop && (i0 >= 0 || i1 < numSlots)) {
                        if (i0 >= 0 &&
                            jezz_ValidCachedNode(jezzRow, jezzRow->slots[i0])) {
                                jezzNode = jezzRow->slots[i0];
                                stop = true;
                        } else if (i1 < numSlots &&
                                   jezz_ValidCachedNode(jezzRow,
                                                        jezzRow->slots[i1])) {
                                jezzNode = jezzRow->slots[i1];
                                stop = true;
                        }  // end if

                        i0--;
                        i1++;
                }  // end while

                if (!jezz_ValidCachedNode(jezzRow, jezzNode)) {
                        jezzNode = jezzRow->tail();
                }  // end if
        }          // end if

        // Choose which way to go.
        const ListLink direction = (jezzNode->x > x) ? HEAD : TAIL;

        // Sequential search.
        while (!jezzRow->isRoot(jezzNode)) {
                if (jezz_Overlap(x, jezzNode)) return jezzNode;
                jezzNode = jezzNode->link[direction];
        }  // end while

        // If we reach this point, something went wrong...
        std::cout << "[BUG] An overlapping node was not found.\n";
        return nullptr;
}  // end method

// -----------------------------------------------------------------------------
// Perform a full sequential search. It is meant to be used during full
// legalization process where node are processed in x order.

Jezz::JezzNode *Jezz::jezz_FindOverlappingNodeNonCached(JezzRow *jezzRow,
                                                        const int x) {
        // Check if the node is outside the row bounds...
        if (x < jezzRow->xmin()) {
                return &jezzRow
                            ->root;  // root is ok here, should not be the head.
        } else if (x >= jezzRow->xmax()) {
                return jezzRow->tail();
        } else {
                JezzNode *jezzNode = jezzRow->tail();
                while (!jezzRow->isRoot(jezzNode)) {
                        if (jezz_Overlap(x, jezzNode)) return jezzNode;
                        jezzNode = jezzNode->link[HEAD];
                }  // end while
        }          // end else

        // If we reach this point, something went wrong...
        std::cout << "[BUG] An overlapping node was not found.\n";
        return nullptr;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_RemoveZeroLengthWhitespace(JezzNode *jezzNode) {
        assert(jezzNode->isWhitespace() && jezzNode->width() == 0);
        jezzNode->row->remove(jezzNode);
        jezz_DeleteWhitespaceNode(jezzNode);
}  // end method

// -----------------------------------------------------------------------------

// Insert jezzNode after jezzNodeAnchor.

double Jezz::jezz_InsertNode(JezzRow *jezzRow, JezzNode *jezzNode, const int x,
                             const bool trial, bool &overflow,
                             double &smallestMaxDisplacementCost) {
        JezzNode *jezzReferenceNode = jezz_FindOverlappingNode(jezzRow, x);

        if (!jezzReferenceNode) {
                // Defensive programming:
                // This condition should not happen unless node is outside the
                // core
                // bounds.
                if (true) {
                        std::cout << "Jezz Debug:\n";
                        std::cout << "Trying to place a cell out of the row "
                                     "bounds.\n";
                        std::cout << "Note: This does not necessarily mean a "
                                     "bug inside Jezz neither means a bug "
                                     "elsewhere.\n";
                        std::cout << "Note: Don't forget to check if the cell "
                                     "was not indeed outside the core bounds "
                                     "priori to legalization.\n";

                        if (jezz_CheckRowConsistency(jezzRow)) {
                                std::cout << "Info: Jezz row is consistent.\n";
                        } else {
                                std::cout << "Bug: Jezz row is inconsistent. "
                                             "This may explain the out of "
                                             "bounds issue...\n";
                        }  // end else

                        std::cout << "row  (design coordinates): ";
                        std::cout << "xmin=" << jezzRow->origin_x << " ";
                        std::cout
                            << "xmax=" << (jezzRow->origin_x +
                                           clsJezzNumSites * clsJezzSiteWidth)
                            << "\n";
                        std::cout << "cell (design coordinates): ";
                        std::cout << "x=" << jezzNode->refx << "\n";

                        std::cout << "row  (jezz coordinates): ";
                        std::cout << "xmin=" << jezzRow->xmin() << " ";
                        std::cout << "xmax=" << jezzRow->xmax() << "\n";
                        std::cout << "cell (jezz coordinates): ";
                        std::cout << "x=" << x << "\n";
                }  // end if

                std::cout << " *\n";
                overflow = true;
                return 0;  // we don't return int max to avoid overflow
        }                  // end if

        if (jezzReferenceNode->isFixed()) {
                const int dl = std::abs(jezzReferenceNode->xmin() - x);
                const int dr = std::abs(jezzReferenceNode->xmax() - x);

                if (dl < dr && !jezzRow->isRoot(jezzReferenceNode->prev))
                        jezzReferenceNode = jezzReferenceNode->prev;
        }  // end if

        double bestOverallDisturbance;
        overflow = false;

        // Check if the node is enclosed completely by the whitespace node.
        if (jezzReferenceNode->isWhitespace() &&
            jezz_Enclosed(jezzReferenceNode, jezzNode)) {
                smallestMaxDisplacementCost = 0;

                if (!trial) {
                        // split whitespace
                        JezzNode *ws0 = jezzReferenceNode;
                        JezzNode *ws1 = jezz_CreateWhitespaceNode();

                        ws1->x = jezzNode->xmax();
                        ws1->w = ws0->xmax() - ws1->xmin();

                        ws0->w = jezzNode->xmin() - ws0->xmin();

                        ws0->row = jezzRow;
                        ws1->row = jezzRow;

                        // [TODO] Use list operations to accomplish this...
                        ws1->next = ws0->next;
                        ws1->prev = jezzNode;
                        ws1->next->prev = ws1;
                        ws0->next = jezzNode;
                        jezzNode->prev = ws0;
                        jezzNode->next = ws1;

                        // Update node properties.
                        jezzNode->row = jezzRow;
                        jezzNode->x = ws0->xmax();

                        // Remove zero length whitespace.
                        if (ws0->width() == 0)
                                jezz_RemoveZeroLengthWhitespace(ws0);
                        if (ws1->width() == 0)
                                jezz_RemoveZeroLengthWhitespace(ws1);
                }  // end if

                bestOverallDisturbance = 0;
        } else {
                compoundOffsetToLeft.resize(jezzNode->w + 1);
                compoundOffsetToRight.resize(jezzNode->w + 1);

                JezzNode *nodeToLeft = jezzReferenceNode;
                JezzNode *nodeToRight = jezzReferenceNode->next;

                const int offsetRequired = jezzNode->width();

                int offsetToLeft;
                int offsetToRight;

                int numSitesAvailableLeft = 0;
                int numSitesAvailableRight = 0;

                jezz_AdjustShiftingCostToSide(
                    jezzRow, nodeToLeft, offsetRequired, HEAD,
                    numSitesAvailableLeft, compoundOffsetToLeft);
                jezz_AdjustShiftingCostToSide(
                    jezzRow, nodeToRight, offsetRequired, TAIL,
                    numSitesAvailableRight, compoundOffsetToRight);

                //-------------------------------------
                // optimum shift
                const int maxOffsetLeft =
                    offsetRequired - numSitesAvailableLeft;
                const int maxOffsetRight =
                    offsetRequired - numSitesAvailableRight;
                if (maxOffsetLeft + maxOffsetRight < offsetRequired) {
                        overflow = true;
                        return 0;  // we don't return int max to avoid overflow
                }                  // end for

                double bestCost = std::numeric_limits<double>::max();

                int shiftsToLeft = -1;
                double cost;
                double currentCost;

                smallestMaxDisplacementCost =
                    std::numeric_limits<double>::max();
                for (int i = maxOffsetLeft;
                     i >= offsetRequired - maxOffsetRight; i--) {
                        currentCost = jezzNode->weight *
                                      std::abs((nodeToLeft->xmax() - i) - x);

                        cost = compoundOffsetToLeft[i].first + currentCost +
                               compoundOffsetToRight[offsetRequired - i].first;

                        const double currMaxDisplacementCost = std::max(
                            (double)(std::max(
                                compoundOffsetToLeft[i].second,
                                compoundOffsetToRight[offsetRequired - i]
                                    .second)),
                            jezzNode->weight *
                                    std::abs(jezzRow->y -
                                             jezzNode->snnaped_refy) *
                                    clsJezzRowHeight +
                                currentCost * clsJezzSiteWidth);

                        if (cost < bestCost) {
                                bestCost = cost;
                                shiftsToLeft = i;
                                smallestMaxDisplacementCost =
                                    currMaxDisplacementCost;
                        } else if (clsJezzTieBreak && cost == bestCost) {
                                if (currMaxDisplacementCost <
                                    smallestMaxDisplacementCost) {
                                        shiftsToLeft = i;
                                        smallestMaxDisplacementCost =
                                            currMaxDisplacementCost;
                                }  // end if
                        }          // end if
                }                  // end for

                offsetToLeft = shiftsToLeft;
                offsetToRight = jezzNode->w - shiftsToLeft;
                bestOverallDisturbance = bestCost;

                if (!trial) {
                        // We must insert the node before making room to it as
                        // some
                        // whitespace may get removed.

                        nodeToLeft->next = jezzNode;
                        nodeToRight->prev = jezzNode;
                        jezzNode->prev = nodeToLeft;
                        jezzNode->next = nodeToRight;
                        jezzNode->row = jezzRow;

                        // Make room for the new node.
                        if (offsetToLeft > 0)
                                jezz_AdjustPositionConsolidate(
                                    jezzRow, nodeToLeft, offsetToLeft, HEAD);

                        if (offsetToRight > 0)
                                jezz_AdjustPositionConsolidate(
                                    jezzRow, nodeToRight, offsetToRight, TAIL);

                        // Note that after making room for the new node, node0
                        // and node1 may
                        // not be valid anymore as they may have been removed if
                        // they are
                        // whitespace and their length became zero.

                        // Update node properties.
                        jezzNode->x = jezzNode->prev->xmax();
                }  // end if
        }          // end else

        // Update node position globally. This should be done in a different
        // way (maybe via a callback) as this makes the Jezz dependent on the
        // placer class. Also we don't need to keep global node positions
        // up-to-date during a full legalization. But for now let's do it like
        // this.
        if (!trial && jezzNode->reference) {
                clsJezzUpdatePositionCallback(
                    jezzNode->reference, jezz_UnsnapPositionX(jezzNode->x),
                    jezzRow->origin_y, jezzRow->getOrientation());
        }  // end if

        return bestOverallDisturbance;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_AdjustShiftingCostToSide(
    JezzRow *jezzRow,
    JezzNode *jezzNode,  // overlapping node, not the node to insert
    const int numOfSitesRequired, const ListLink direction, int &overflow,
    std::vector<std::pair<double, double>>
        &sideCompoundShiftingCost  // cost vector for each side
    ) {
        overflow = numOfSitesRequired;
        sideCompoundShiftingCost.assign(sideCompoundShiftingCost.size(),
                                        std::make_pair(0.0, 0.0));

        JezzNode *nodeInTheWay = jezzNode;

        // [TODO] Consider renaming k to spaces or something like that.
        int k;  // stores the number of spaces (node widths) already processed

        k = 0;

        // on current side, left or right
        while (!jezzRow->isRoot(nodeInTheWay) && overflow > 0) {
                if (nodeInTheWay->isWhitespace()) {
                        const int maxNumSitesAllowed =
                            nodeInTheWay->width() > overflow
                                ? overflow
                                : nodeInTheWay->width();

                        k += maxNumSitesAllowed;
                        overflow -= maxNumSitesAllowed;  // diminishes the
                                                         // number of sites
                                                         // available from
                                                         // moving aside
                } else if (nodeInTheWay->isFixed()) {
                        break;
                } else {
                        const int dx = direction == TAIL ? +1 : -1;

                        for (int i = k + 1, shifts = dx;
                             i <= numOfSitesRequired; i++, shifts += dx) {
                                const DBU xDisplacement =
                                    std::abs(nodeInTheWay->x + shifts -
                                             nodeInTheWay->snapped_refx) *
                                    clsJezzSiteWidth;
                                const DBU yDisplacement =
                                    std::abs(jezzRow->y -
                                             nodeInTheWay->snnaped_refy) *
                                    clsJezzRowHeight;
                                const DBU displacement =
                                    xDisplacement + yDisplacement;
                                const double weightedDisplacement =
                                    nodeInTheWay->weight * displacement;

                                if (weightedDisplacement >
                                    sideCompoundShiftingCost[i].second) {
                                        sideCompoundShiftingCost[i].second =
                                            weightedDisplacement;
                                }  // end if

                                sideCompoundShiftingCost[i].first +=
                                    nodeInTheWay->weight *
                                    jezz_isNodeApproachingOriginal(nodeInTheWay,
                                                                   shifts);
                        }  // end for
                }          // end else

                nodeInTheWay = nodeInTheWay->link[direction];
        }  // end while
}  // end method

// -----------------------------------------------------------------------------

int Jezz::jezz_isNodeApproachingOriginal(JezzNode *jezzNode, int shifts) {
        // impact  = |original - shifted| - |current - original|
        // shifted = current + shifts (can be negative)
        return std::abs(jezzNode->snapped_refx - (jezzNode->x + shifts)) -
               std::abs(jezzNode->x - jezzNode->snapped_refx);
}  // end method

// -----------------------------------------------------------------------------

int Jezz::jezz_AdjustPositionConsolidate(JezzRow *jezzRow, JezzNode *jezzNode,
                                         const int offset,
                                         const ListLink direction) {
        int overflow = offset;
        int disturbance = 0;

        const int multiplier = direction == HEAD ? -1 : +1;

        JezzNode *node = jezzNode;
        while (!jezzRow->isRoot(node) && overflow > 0) {
                JezzNode *next = node->link[direction];

                if (node->isWhitespace()) {
                        if (node->w >= overflow) {
                                node->w -= overflow;
                                if (direction == TAIL) {
                                        node->x += overflow;
                                }  // end if

                                if (node->w == 0) {
                                        jezz_RemoveZeroLengthWhitespace(node);
                                }  // end else

                                overflow = 0;
                        } else {
                                overflow -= node->width();

                                if (direction == TAIL) {
                                        node->x += node->w + overflow;
                                } else {
                                        node->x -= overflow;
                                }  // end else

                                node->w = 0;
                                jezz_RemoveZeroLengthWhitespace(node);
                        }  // end if
                } else if (node->isFixed()) {
                        break;
                } else {
                        node->x += multiplier * overflow;
                        disturbance += overflow;

                        // Update node position globally. This should be done in
                        // a different
                        // way (maybe via a callback) as this makes the Jezz
                        // dependent on
                        // the placer class. Also we don't need to keep global
                        // node
                        // positions up-to-date during a full legalization. But
                        // for now
                        // let's do it like this.
                        if (node->reference) {
                                clsJezzUpdatePositionCallback(
                                    node->reference,
                                    jezz_UnsnapPositionX(node->x),
                                    jezzRow->origin_y,
                                    jezzRow->getOrientation());
                        }  // end if
                }          // end else

                // Advance
                node = next;
        }  // end while

        return disturbance;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_UpdatePositionsAll() {
        const int numRows = clsJezzRows.size();
        for (int i = 0; i < numRows; i++) {
                jezz_UpdatePositions(&clsJezzRows[i]);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Jezz::reportFinalResults(std::ostream &out) {
        clsPhysicalDesign.updateAllNetBounds();
        double designUnits =
            double(clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU));
        DBU hpwl = clsPhysicalDesign.getHPWL().aggregated();
        DBU initialHpwl = clsInitialHpwl.aggregated();

        DBU maxDisp = 0;
        DBU totalDisp = 0;
        double rowHeight = double(clsPhysicalDesign.getRowHeight());
        int numCells = clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE);
        for (Rsyn::Instance inst : clsModule.allInstances()) {
                if (inst.getType() != Rsyn::CELL) continue;
                Rsyn::PhysicalCell phCell =
                    clsPhysicalDesign.getPhysicalCell(inst.asCell());
                if (inst.isFixed()) continue;
                DBU disp = phCell.getDisplacement();
                maxDisp = std::max(maxDisp, disp);
                totalDisp += disp;
        }  // end for

        const int N = 15;

        StreamStateSaver sss(out);

        out << std::left;
        out << "\n";
        out << "                  ";  // reserve space for "Final Result: "
        out << std::setw(N) << "Design";
        out << std::setw(N) << "GP_HPWL(e6)";
        out << std::setw(N) << "Leg_HPWL(e6)";
        out << std::setw(N) << "HPWL(%)";
        out << std::setw(N + 5) << "Avg_Disp_#Rows";
        out << std::setw(N + 5) << "Max_Disp_#Rows";
        out << std::setw(N) << "runtime(s)";
        out << "\n";

        out << "Jezz Result:      ";  // make it easy to grep
        out << std::setw(N) << clsDesign.getName();
        out << std::setw(N) << initialHpwl / (designUnits * 1e6);
        out << std::setw(N) << hpwl / (designUnits * 1e6);
        out << std::setw(N) << 100 * (hpwl / double(initialHpwl) - 1);
        out << std::setw(N + 5) << totalDisp / (rowHeight * numCells);
        out << std::setw(N + 5) << maxDisp / rowHeight;
        out << std::setw(N) << clsRuntime;
        out << "\n";
        out << std::endl;

        sss.restore();

}  // end method

// -----------------------------------------------------------------------------

void Jezz::reportAverageCellDisplacement(std::ostream &out) {
        int numCells = clsPhysicalDesign.getNumElements(
            Rsyn::PhysicalType::PHYSICAL_MOVABLE);
        std::vector<DBU> dispCells;
        dispCells.reserve(numCells);
        for (Rsyn::Instance inst : clsModule.allInstances()) {
                if (inst.isFixed() || inst.getType() != Rsyn::CELL) continue;
                Rsyn::Cell cell = inst.asCell();
                Rsyn::PhysicalCell phCell =
                    clsPhysicalDesign.getPhysicalCell(cell);
                DBU disp = phCell.getDisplacement();
                dispCells.push_back(disp);
        }  // end for

        std::sort(dispCells.begin(), dispCells.end(),
                  [](const DBU disp0, const DBU disp1) {
                          return disp0 > disp1;
                  });  // end sort

        const double clsAcd2Weight = 10.0;
        const double clsAcd5Weight = 4.0;
        const double clsAcd10Weight = 2.0;
        const double clsAcd20Weight = 1.0;

        const int numAcd2 = static_cast<int>(numCells * 0.02);
        const int numAcd5 = static_cast<int>(numCells * 0.05);
        const int numAcd10 = static_cast<int>(numCells * 0.1);
        const int numAcd20 = static_cast<int>(numCells * 0.2);

        const bool empty = dispCells.empty();

        double clsAcdMaxDisp = empty ? 0.0 : static_cast<double>(dispCells[0]);
        double clsAcdAvgDisp = 0.0;
        double clsAcd2 = 0.0;
        double clsAcd5 = 0.0;
        double clsAcd10 = 0.0;
        double clsAcd20 = 0.0;

        for (int i = 0; i < numAcd2; ++i) {
                clsAcd2 += dispCells[i];
        }  // end for

        clsAcd5 = clsAcd2;
        for (int i = numAcd2; i < numAcd5; ++i) {
                clsAcd5 += dispCells[i];
        }  // end for

        clsAcd10 = clsAcd5;
        for (int i = numAcd5; i < numAcd10; ++i) {
                clsAcd10 += dispCells[i];
        }  // end for

        clsAcd20 = clsAcd10;
        for (int i = numAcd10; i < numAcd20; ++i) {
                clsAcd20 += dispCells[i];
        }  // end for

        clsAcdAvgDisp = clsAcd20;
        for (int i = numAcd20; i < numCells; ++i) {
                clsAcdAvgDisp += dispCells[i];
        }  // end for

        clsAcdAvgDisp = empty ? 0.0 : (clsAcdAvgDisp / numCells);

        clsAcd2 = empty ? 0.0 : (clsAcd2 / numAcd2);
        clsAcd5 = empty ? 0.0 : (clsAcd5 / numAcd5);
        clsAcd10 = empty ? 0.0 : (clsAcd10 / numAcd10);
        clsAcd20 = empty ? 0.0 : (clsAcd20 / numAcd20);

        double wAcd2 = clsAcd2Weight * clsAcd2;
        double wAcd5 = clsAcd5Weight * clsAcd5;
        double wAcd10 = clsAcd10Weight * clsAcd10;
        double wAcd20 = clsAcd20Weight * clsAcd20;

        double clsWeightedAcd =
            (wAcd2 + wAcd5 + wAcd10 + wAcd20) / (10.0 + 4.0 + 2.0 + 1.0);

        StreamStateSaver sss(out);
        const int N = 15;
        //	const double rowHeight = double(clsPhDesign.getRowHeight());
        const double database = double(
            clsPhysicalDesign.getDatabaseUnits(Rsyn::DBUType::DESIGN_DBU));
        out << std::left;
        out << "\n";
        out << "Jezz              ";
        out << std::setw(N) << "Design";
        out << std::setw(N) << "ACD2%";
        out << std::setw(N) << "ACD5%";
        out << std::setw(N) << "ACD10%";
        out << std::setw(N) << "ACD20%";
        out << std::setw(N) << "wACD";
        out << std::setw(N) << "ACDAVG";
        out << std::setw(N) << "ACDMaxDisp";
        out << "\n";

        out << "ACD(um):          ";  // make it easy to grep
        out << std::setw(N) << clsDesign.getName();
        out << std::setw(N) << clsAcd2 / database;
        out << std::setw(N) << clsAcd5 / database;
        out << std::setw(N) << clsAcd10 / database;
        out << std::setw(N) << clsAcd20 / database;
        out << std::setw(N) << clsWeightedAcd / database;
        out << std::setw(N) << clsAcdAvgDisp / database;
        out << std::setw(N) << clsAcdMaxDisp / database;
        out << "\n";

        sss.restore();
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_UpdatePositions(JezzRow *jezzRow) {
        for (JezzNodeGuts *jezzNode : jezzRow->allNodesFromHeadToTail()) {
                if (jezzNode->reference) {
                        const DBU x = jezz_UnsnapPositionX(jezzNode->x);
                        clsJezzUpdatePositionCallback(
                            jezzNode->reference, x, jezzRow->origin_y,
                            jezzRow->getOrientation());
                }  // end if
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

bool Jezz::jezz_CheckRowConsistency_HeadToTail(JezzRow *jezzRow) {
        int x = jezzRow->xmin();
        int w = 0;
        bool ws = false;

        bool sucess = true;

        for (JezzNodeGuts *jezzNode : jezzRow->allNodesFromHeadToTail()) {
                w += jezzNode->w;

                if (x != jezzNode->xmin()) {
                        std::cout
                            << "[JEZZ EXCEPTION] h2t Inconsistent row ==> "
                            << "Position: " << x << " != " << jezzNode->xmin()
                            << std::endl;
                        sucess = false;
                        break;
                }  // end if

                if (ws && jezzNode->isWhitespace()) {
                        std::cout << "[JEZZ EXCEPTION] h2t Two consecutives "
                                     "whitespaces."
                                  << std::endl;
                        sucess = false;
                        break;
                }  // end if

                if (jezzNode->row != jezzRow) {
                        std::cout
                            << "[JEZZ EXCEPTION] h2t Inconsistent row ==> "
                            << "Jezz node is in the wrong row.\n";
                        sucess = false;
                        break;
                }  // end if

                x += jezzNode->w;
                ws = jezzNode->isWhitespace();
        }  // end for

        if (w != jezzRow->width()) {
                std::cout << "[JEZZ EXCEPTION] h2t Inconsistent row ==>"
                          << "Width: " << w << " != " << jezzRow->width()
                          << std::endl;
                sucess = false;
        }  // end if

        return sucess;
}  // end method

// -----------------------------------------------------------------------------

bool Jezz::jezz_CheckRowConsistency_TailToHead(JezzRow *jezzRow) {
        int x = jezzRow->xmax();
        int w = jezzRow->w;
        bool ws = false;

        bool sucess = true;

        for (JezzNodeGuts *jezzNode : jezzRow->allNodesFromTailToHead()) {
                w -= jezzNode->w;
                x -= jezzNode->w;

                if (x != jezzNode->xmin()) {
                        std::cout
                            << "[JEZZ EXCEPTION] t2h Inconsistent row ==> "
                            << "Position: " << x << " != " << jezzNode->xmin()
                            << std::endl;
                        sucess = false;
                        break;
                }  // end if

                if (ws && jezzNode->isWhitespace()) {
                        std::cout << "[JEZZ EXCEPTION] t2h Two consecutives "
                                     "whitespaces."
                                  << std::endl;
                        sucess = false;
                        break;
                }  // end if

                if (jezzNode->row != jezzRow) {
                        std::cout
                            << "[JEZZ EXCEPTION] t2h Inconsistent row ==> "
                            << "Jezz node is in the wrong row.\n";
                        sucess = false;
                        break;
                }  // end if

                ws = jezzNode->isWhitespace();
        }  // end for

        if (w != 0) {
                std::cout << "[JEZZ EXCEPTION] t2h Inconsistent row ==>"
                          << "w = " << w
                          << " should be zero (width=" << jezzRow->width()
                          << ")" << std::endl;
                sucess = false;
        }  // end if

        return sucess;
}  // end method

// -----------------------------------------------------------------------------

bool Jezz::jezz_CheckRowConsistency(JezzRow *jezzRow) {
        const bool head2tail = jezz_CheckRowConsistency_HeadToTail(jezzRow);
        const bool tail2head = jezz_CheckRowConsistency_TailToHead(jezzRow);

        if (!head2tail) {
                std::cout << "Row is not consistent when traversing from head "
                             "to tail.\n";
        }  // end if

        if (!tail2head) {
                std::cout << "Row is not consistent when traversing from tail "
                             "to head.\n";
        }  // end if

        return head2tail && tail2head;
}  // end method

// -----------------------------------------------------------------------------

bool Jezz::jezz_CheckNodeIsInTheRow(JezzRow *jezzRow, JezzNode *jezzNode) {
        JezzNode *jn = jezzRow->tail();
        while (!jezzRow->isRoot(jn)) {
                if (jezzNode == jn) {
                        return true;
                }  // end if
                jn = jn->link[HEAD];
        }  // end while
        return false;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_StressTest() {
        const int numNodes = clsJezzNodes.size();
        const int numRows = clsJezzRows.size();

        const DBU coreWidth = clsJezzNumSites * clsJezzSiteWidth;
        const DBU coreHeight = clsJezzNumRows * clsJezzRowHeight;

        const int N = 10000;
        for (int i = 0; i < N; i++) {
                const int index = (numNodes - 1) * (rand() / DBU(RAND_MAX));

                JezzNode *jezzNode = &clsJezzNodes[index];

                const DBU x0 = jezzNode->refx;
                const DBU y0 = jezzNode->refy;

                const DBU randomx = rand() / DBU(RAND_MAX);
                const DBU randomy = rand() / DBU(RAND_MAX);
                const DBU x1 = clsJezzBounds[LOWER][X] + (coreWidth * randomx);
                const DBU y1 = clsJezzBounds[LOWER][Y] + (coreHeight * randomy);

                // Due to rounding, some random generated (x, y) may lie outside
                // the
                // row bounds.
                const int col = jezz_SnapPositionX(x1);
                const int row = jezz_SnapPositionY(y1);
                if (!jezz_ValidRowIndex(row) || !jezz_ValidSiteIndex(col))
                        continue;

                JezzRow *jezzRow = jezzNode->row;

                jezz_dp_RemoveNode(jezzNode);
                if (!jezz_CheckRowConsistency(jezzRow)) assert(false);

                bool overflow;
                jezz_dp_InsertNode(jezzNode, x1, y1, overflow);
                if (overflow) {
                        jezz_dp_InsertNode(jezzNode, x0, y0, overflow);
                        assert(!overflow);
                }  // end if
                if (!jezz_CheckRowConsistency(jezzNode->row)) assert(false);
        }  // end for

        for (int i = 0; i < numRows; i++) {
                if (!jezz_CheckRowConsistency(&clsJezzRows[i])) assert(false);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_StressTest_CacheSystem() {
        const DBU coreWidth = clsJezzNumSites * clsJezzSiteWidth;
        const DBU coreHeight = clsJezzNumRows * clsJezzRowHeight;

        const int N = 10000;
        for (int i = 0; i < N; i++) {
                const DBU randomx = rand() / DBU(RAND_MAX);
                const DBU randomy = rand() / DBU(RAND_MAX);
                const DBU x = clsJezzBounds[LOWER][X] + (coreWidth * randomx);
                const DBU y = clsJezzBounds[LOWER][Y] + (coreHeight * randomy);

                const int col = jezz_SnapPositionX(x);
                const int row = jezz_SnapPositionY(y);
                // Due to rounding, some random generated (x, y) may lie outside
                // the
                // row bounds.
                if (!jezz_ValidRowIndex(row) || !jezz_ValidSiteIndex(col))
                        continue;

                JezzRow *jezzRow = jezz_GetRowByIndex(row);

                JezzNode *jezzNode0 = jezz_FindOverlappingNodeNonCached(
                    jezzRow, jezz_SnapPositionX(x));
                JezzNode *jezzNode1 = jezz_FindOverlappingNodeCached(
                    jezzRow, jezz_SnapPositionX(x));

                if (jezzNode0 != jezzNode1) {
                        std::cout << "[BUG] Jezz node at position (" << x
                                  << ", " << y << ") "
                                  << "returned by cached version and "
                                     "non-cached version are different.\n";
                }  // end method
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_PrintNode(JezzNode *jezzNode, const std::string &lable) {
        std::cout << "Jezz Node: " << jezzNode << " " << lable << "\n";
        std::cout << "  "
                  << "xmin"
                  << " : " << jezzNode->xmin() << "\n";
        std::cout << "  "
                  << "xmax"
                  << " : " << jezzNode->xmax() << "\n";
        std::cout << "  "
                  << "width"
                  << " : " << jezzNode->width() << "\n";
        std::cout << "  "
                  << "whitespace"
                  << " : " << jezzNode->isWhitespace() << "\n";
        std::cout << "  "
                  << "obstacle"
                  << " : " << jezzNode->isObstacle() << "\n";
        std::cout << "  "
                  << "fixed"
                  << " : " << jezzNode->isFixed() << "\n";
        std::cout << "  "
                  << "row"
                  << " : " << jezzNode->row << "\n";
}  // end method

// -----------------------------------------------------------------------------

Jezz::JezzNode *Jezz::jezz_dp_FindNodeAtPosition(const DBU x, const DBU y) {
        const int row = jezz_GetRowIndex(y);
        if (!jezz_ValidRowIndex(row)) {
                return NULL;
        }  // end if

        JezzRow *jezzRow = &clsJezzRows[row];
        return jezz_FindOverlappingNode(jezzRow, jezz_SnapPositionX(x));
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_dp_InsertNodeUsingJezzCoordinates(JezzNode *jezzNode,
                                                  const int jx, const int jy,
                                                  bool &overflow) {
        if (jezz_dp_IsLegalized(jezzNode)) {
                jezz_dp_RemoveNode(jezzNode);
        }  // end if

        jezz_dp_UpdateReferencePosition(jezzNode, jezz_UnsnapPositionX(jx),
                                        jezz_UnsnapPositionY(jy));

        const int row = jy;
        if (!jezz_ValidRowIndex(row)) {
                overflow = true;
                return;
        }  // end if

        // BUG FIX: jezz_InserNode requires also that the x position to be set
        // to
        // the target x.
        // TODO: Make jezz_InsertNode use always the x position inside the Jezz
        // node
        // and remove the x parameter.
        jezzNode->x = jx;

        JezzRow *jezzRow = &clsJezzRows[row];
        jezz_InsertNode(jezzRow, jezzNode, jx, false, overflow);
        if (!overflow) {
                jezz_UpdateCache(jezzNode);
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_dp_InsertNode(JezzNode *jezzNode, const DBU x, const DBU y,
                              bool &overflow) {
        if (jezz_dp_IsLegalized(jezzNode)) {
                jezz_dp_RemoveNode(jezzNode);
        }  // end if

        jezz_dp_UpdateReferencePosition(jezzNode, x, y);

        int row = jezz_GetRowIndex(y);
        if (!jezz_ValidRowIndex(row)) {
                row = std::max(0, std::min(row, clsJezzNumRows - 1));
        }  // end if

        // BUG FIX: jezz_InserNode requires also that the x position to be set
        // to
        // the target x.
        // TODO: Make jezz_InsertNode use always the x position inside the Jezz
        // node
        // and remove the x parameter.
        jezzNode->x = jezz_SnapPositionX(x);

        JezzRow *jezzRow = &clsJezzRows[row];
        jezz_InsertNode(jezzRow, jezzNode, jezz_SnapPositionX(x), false,
                        overflow);
        if (!overflow) {
                jezz_UpdateCache(jezzNode);
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

// void Jezz::jezz_dp_InsertNode(JezzNode *jezzNode, const WhitespaceDescriptor
// &ws) {
//	if (jezz_dp_IsLegalized(jezzNode)) {
//		jezz_dp_RemoveNode(jezzNode);
//	} // end if
//
//	jezz_dp_UpdateReferencePosition(jezzNode, x, y);
//
//	int row = jezz_GetRowIndex(y);
//	if (!jezz_ValidRowIndex(row)) {
//		row = std::max(0, std::min(row, clsJezzNumRows - 1));
//	} // end if
//
//	// BUG FIX: jezz_InserNode requires also that the x position to be set
// to
//	// the target x.
//	// TODO: Make jezz_InsertNode use always the x position inside the Jezz
// node
//	// and remove the x parameter.
//	jezzNode->x = jezz_SnapPositionX(x);
//
//	JezzRow *jezzRow = &clsJezzRows[row];
//	jezz_InsertNode(jezzRow, jezzNode, jezz_SnapPositionX(x), false,
// overflow);
//	if (!overflow) {
//		jezz_UpdateCache(jezzNode);
//	} // end if
//} // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_dp_RemoveNode(JezzNode *curr) {
        if (!jezz_dp_IsLegalized(curr)) {
                return;
        }  // end if

        JezzNode *prev = curr->prev;
        JezzNode *next = curr->next;

        if (prev->isWhitespace()) {
                if (next->isWhitespace()) {
                        prev->w += curr->w + next->w;
                        prev->next = next->next;
                        next->next->prev = prev;
                        jezz_DeleteWhitespaceNode(next);
                } else {
                        prev->w += curr->w;
                        prev->next = next;
                        next->prev = prev;
                }  // end else
        } else if (next->isWhitespace()) {
                next->x = curr->x;
                next->w += curr->w;
                prev->next = next;
                next->prev = prev;
        } else {
                JezzNode *ws = jezz_CreateWhitespaceNode();
                ws->x = curr->xmin();
                ws->w = curr->width();
                ws->row = curr->row;
                ws->prev = curr->prev;
                ws->next = curr->next;

                curr->prev->next = ws;
                curr->next->prev = ws;
        }  // end else

        // Un-legalize node
        curr->row = nullptr;
        curr->prev = nullptr;
        curr->next = nullptr;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_dp_SwapNeighbours(JezzNode *jezzNode0, const ListLink link) {
        JezzRow *jezzRow = jezzNode0->row;
        JezzNode *jezzNode1 = jezzNode0->link[link];

        LegalList<JezzNodeGuts>::swap(jezzNode0, jezzNode1);

        // [TODO] Do this without if :)
        if (link == TAIL) {
                jezzNode1->x = jezzNode0->xmin();
                jezzNode0->x = jezzNode1->xmax();
        } else {
                jezzNode0->x = jezzNode1->xmin();
                jezzNode1->x = jezzNode0->xmax();
        }  // end else

        // Ugly. Keep global position up-to-date.
        if (jezzNode0->reference) {
                clsJezzUpdatePositionCallback(
                    jezzNode0->reference, jezz_UnsnapPositionX(jezzNode0->x),
                    jezzRow->origin_y, jezzRow->getOrientation());
        }  // end if

        if (jezzNode1->reference) {
                clsJezzUpdatePositionCallback(
                    jezzNode1->reference, jezz_UnsnapPositionX(jezzNode1->x),
                    jezzRow->origin_y, jezzRow->getOrientation());
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_dp_Change_Fixed(JezzNode *jezzNode, const bool fixed) {
        jezzNode->fixed = fixed;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_storeSolution(const std::string name) {
        clsJezzStoredSolution[name] = clsJezzNodes;
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_restoreSolutionFull(const std::string name) {
        auto it = clsJezzStoredSolution.find(name);
        if (it == clsJezzStoredSolution.end()) {
                std::cout << "[WARNING] Stored Jezz solution '" << name
                          << "' does not exist.\n";
                return;
        }  // end if

        std::deque<JezzNode> &storedSolution = it->second;
        if (storedSolution.empty()) {
                std::cout
                    << "[WARNING] Jezz: Trying to restore an empty solution.\n";
                return;
        }  // end if

        // Some clean-up
        jezz_DeleteWhitespaceNodes();
        jezz_InitRows();

        // Set Jezz to the full mode again.
        clsJezzIncrementalMode = false;

        // Reinsert nodes one-by-one. It should be safe to re-insert them in any
        // order as we are using integer coordinates and the stored solution is
        // legal.

        const int numObstacles = clsJezzObstacles.size();
        for (int i = 0; i < numObstacles; i++) {
                JezzNode *jezzNode = &clsJezzObstacles[i];

                if (!jezzNode->row) {
                        std::cout << "[WARNING] Jezz: Not restoring obstacle "
                                     "with null row.\n";
                        continue;
                }  // end if

                const int x = jezzNode->x;
                const int y = jezzNode->row->y;

                jezzNode->row = nullptr;
                jezzNode->next = nullptr;
                jezzNode->prev = nullptr;

                bool overflow = false;
                jezz_dp_InsertNodeUsingJezzCoordinates(jezzNode, x, y,
                                                       overflow);
                if (overflow) {
                        std::cout << "[BUG] Jezz: Overflow when restoring a "
                                     "solution. This should not happen... "
                                     "Something is broken.\n";
                }  // end if
        }          // end for

        const int numNodes = storedSolution.size();
        for (int i = 0; i < numNodes; i++) {
                JezzNode *jezzNodeStored = &storedSolution[i];
                JezzNode *jezzNode = &clsJezzNodes[i];

                jezzNode->x = jezzNodeStored->x;
                jezzNode->fixed = jezzNodeStored->fixed;
                jezzNode->refx = jezzNodeStored->refx;
                jezzNode->refy = jezzNodeStored->refy;
                jezzNode->snapped_refx = jezz_SnapPositionX(jezzNode->refx);
                jezzNode->snnaped_refy = jezz_SnapPositionY(jezzNode->refy);

                jezzNode->row = nullptr;
                jezzNode->next = nullptr;
                jezzNode->prev = nullptr;

                if (jezz_dp_IsLegalized(jezzNodeStored)) {
                        const int x = jezzNodeStored->x;
                        const int y = jezzNodeStored->row->y;

                        bool overflow = false;
                        jezz_dp_InsertNodeUsingJezzCoordinates(jezzNode, x, y,
                                                               overflow);
                        if (overflow) {
                                std::cout << "[BUG] Jezz: Overflow when "
                                             "restoring a solution. This "
                                             "should not happen... Something "
                                             "is broken.\n";
                        }  // end if
                }          // end if
        }                  // end for
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_restoreSolutionIncremental(const std::string name) {
        auto it = clsJezzStoredSolution.find(name);
        if (it == clsJezzStoredSolution.end()) {
                std::cout << "[WARNING] Stored Jezz solution '" << name
                          << "' does not exist.\n";
                return;
        }  // end if

        std::deque<JezzNode> &storedSolution = it->second;
        if (storedSolution.empty()) {
                std::cout
                    << "[WARNING] Jezz: Trying to restore an empty solution.\n";
                return;
        }  // end if

        // Remove and reinsert nodes one-by-one. It should be safe to remove and
        // re-
        // insert them in any order as we are using integer coordinates and the
        // stored solution is legal.

        const int numNodes = storedSolution.size();
        std::deque<int> changed;

        for (int i = 0; i < numNodes; i++) {
                JezzNode *jezzNodeStored = &storedSolution[i];
                JezzNode *jezzNode = &clsJezzNodes[i];

                // Restore reference position and fixed state.
                jezzNode->fixed = jezzNodeStored->fixed;
                jezzNode->refx = jezzNodeStored->refx;
                jezzNode->refy = jezzNodeStored->refy;
                jezzNode->snapped_refx = jezz_SnapPositionX(jezzNode->refx);
                jezzNode->snnaped_refy = jezz_SnapPositionY(jezzNode->refy);

                if (jezzNode->x == jezzNodeStored->x &&
                    jezzNode->row == jezzNodeStored->row) {
                        // Don't use y here because y is not changed if the node
                        // is
                        // un-legalized. On the other hand, row is set to
                        // nullptr when the
                        // node is un-legalized.

                        // Jezz node did not change position. Keep it legalized.

                        // Nothing to be done...
                } else {
                        // Un-legalize node.
                        jezz_dp_RemoveNode(jezzNode);

                        // Update node.
                        jezzNode->x = jezzNodeStored->x;
                        jezzNode->row = nullptr;
                        jezzNode->next = nullptr;
                        jezzNode->prev = nullptr;

                        // Mark to be re-inserted;
                        if (jezz_dp_IsLegalized(jezzNodeStored)) {
                                changed.push_back(i);
                        }  // end if
                }          // end if
        }                  // end for

        for (int i : changed) {
                JezzNode *jezzNodeStored = &storedSolution[i];
                JezzNode *jezzNode = &clsJezzNodes[i];

                const int x = jezzNodeStored->x;
                const int y = jezzNodeStored->row->y;

                bool overflow = false;
                jezz_dp_InsertNodeUsingJezzCoordinates(jezzNode, x, y,
                                                       overflow);
                if (overflow) {
                        std::cout << "[BUG] Jezz: Overflow when restoring a "
                                     "solution. This should not happen... "
                                     "Something is broken.\n";
                }  // end if
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_diffSolutions(
    const std::string solution0, const std::string solution1,
    std::vector<std::tuple<Rsyn::Cell, double, double>> &diff) {
        const auto &it0 = clsJezzStoredSolution.find(solution0);
        const auto &it1 = clsJezzStoredSolution.find(solution1);

        if (it0 == clsJezzStoredSolution.end() ||
            it1 == clsJezzStoredSolution.end()) {
                std::cout << "[ERROR] Jezz: Solution not found.\n";
                return;
        }  // end if

        const std::deque<JezzNode> &storedSolution0 = it0->second;
        const std::deque<JezzNode> &storedSolution1 = it1->second;

        if (storedSolution0.size() != storedSolution1.size()) {
                std::cout << "[BUG] Jezz: Something broken on line " << __LINE__
                          << "\n";
                return;
        }  // end if

        DBU maxDiff = 0;
        for (int i = 0; i < storedSolution0.size(); i++) {
                const JezzNode &jezzNode0 = storedSolution0[i];
                const JezzNode &jezzNode1 = storedSolution1[i];

                if (jezzNode0.reference != jezzNode1.reference) {
                        std::cout << "[BUG] Jezz: Something broken on line "
                                  << __LINE__ << "\n";
                        return;
                }

                const DBU cellDiff =
                    std::abs(jezzNode0.x - jezzNode1.x) +
                    std::abs(jezzNode0.row->y - jezzNode1.row->y);

                maxDiff = std::max(maxDiff, cellDiff);
                diff.emplace_back(jezzNode0.reference, cellDiff, cellDiff);
        }  // end for

        if (maxDiff) {
                for (int i = 0; i < diff.size(); i++) {
                        std::get<2>(diff[i]) /= maxDiff;
                }  // end for
        }          // end if
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_ResetNode(JezzNode *jezzNode) {
        jezzNode->reset();
        jezzNode->next = nullptr;
        jezzNode->prev = nullptr;
}  // end method

// -----------------------------------------------------------------------------

Jezz::JezzNode *Jezz::jezz_CreateWhitespaceNode() {
        if (clsJezzDeletedWhitespaces.empty()) {
                clsJezzWhitespaces.resize(clsJezzWhitespaces.size() + 1);
                Jezz::JezzNode *whitespace = &clsJezzWhitespaces.back();
                whitespace->ws = true;
                whitespace->deleted = false;
                return whitespace;
        } else {
                Jezz::JezzNode *whitespace = clsJezzDeletedWhitespaces.front();
                clsJezzDeletedWhitespaces.pop_front();
                whitespace->ws = true;  // defensive programming
                whitespace->deleted = false;
                return whitespace;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void Jezz::jezz_DeleteWhitespaceNode(JezzNode *whitespace) {
        jezz_ResetNode(whitespace);
        whitespace->deleted = true;
        clsJezzDeletedWhitespaces.push_back(whitespace);
}  // end method
