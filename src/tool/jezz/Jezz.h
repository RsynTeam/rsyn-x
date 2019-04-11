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

#ifndef RSYN_JEZZ_H
#define RSYN_JEZZ_H

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Service.h"
#include "session/Session.h"
#include "util/DBU.h"
#include "util/RangeBasedLoop.h"

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <limits>
#include <cmath>
#include <cassert>

namespace Rsyn {
class Session;
}

namespace Rsyn {
class PhysicalService;
}

class Jezz : public Rsyn::Service, public Rsyn::DesignObserver {
       public:
        typedef std::function<void(Rsyn::Cell cell, const DBU x, const DBU y,
                                   Rsyn::PhysicalOrientation orient)>
            UpdatePositionCallback;

        enum ListLink { HEAD, TAIL };
        const static ListLink REVERSE_LIST_LINK[2];

        // As neighbor rows share their horizontal edge, it's ambiguous to ask
        // to
        // each row a point at the shared-edge belongs to. This enum allows one
        // to
        // specify how this ambiguity should be resolved.
        enum SharedRowEdgeResolution {
                // When the point lies at the shared edge, selects the bottom
                // row.
                BOTTOM,

                // When the point lies at the shared edge, selects the top row.
                TOP
        };

        virtual void start(const Rsyn::Json &params);
        virtual void stop();

        // Events
        virtual void onPostCellRemap(Rsyn::Cell cell,
                                     Rsyn::LibraryCell oldLibraryCell) override;
        virtual void onPostInstanceCreate(Rsyn::Instance instance) override;
        virtual void onPostInstanceMove(Rsyn::Instance instance) override;

       private:
        // Physical layer
        Rsyn::PhysicalDesign clsPhysicalDesign;

        // Circuitry
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;

        // Indicates whether or not Jezz was already initialized.
        bool clsInitialized;

        // Enable debugging messages.
        bool clsEnableDebugMessages;

        DBUxy clsInitialHpwl;
        double clsRuntime;

        // Helper function to initialize Jezz.
        void initJezz();

        // Define obstacles (blockages, macro blocks) handling potential
        // overlaps
        // among them.
        void initJezz_Obstacles();

        ////////////////////////////////////////////////////////////////////////////
        // Linked List
        ////////////////////////////////////////////////////////////////////////////

        // We implement our own double-linked list to allow storing the nodes
        // in a contiguous array although this is not necessary.

        template <class T>
        struct LegalListNode : T {
                union {
                        LegalListNode *link[2];
                        struct {
                                LegalListNode *prev;
                                LegalListNode *next;
                        };
                };

                LegalListNode() {
                        link[HEAD] = nullptr;
                        link[TAIL] = nullptr;
                }  // end constructor
        };         // end struct

        template <class T>
        struct LegalList {
                int counter;
                LegalListNode<T> root;

                LegalList() { reset(); }  // end constructor

                class NodeCollection {
                       private:
                        LegalList &list;
                        ListLink direction;
                        LegalListNode<T> *node;

                       public:
                        NodeCollection(LegalList &list,
                                       const ListLink direction)
                            : list(list), direction(direction) {
                                node = list.root.link[direction];
                        }  // end constructor

                        bool filter() { return false; }
                        bool stop() { return list.isRoot(node); }
                        void next() { node = node->link[direction]; }
                        T *current() { return (T *)node; }
                };  // end class

                // Reset this list.
                void reset() {
                        counter = 0;
                        root.link[HEAD] = &root;
                        root.link[TAIL] = &root;
                }  // end method

                // Insert a node in this list at HEAD our TAIL position.
                void insert(const ListLink link, LegalListNode<T> *node) {
                        counter++;

                        const ListLink reverseLink = REVERSE_LIST_LINK[link];
                        node->link[link] = &root;
                        node->link[reverseLink] = root.link[reverseLink];
                        node->link[reverseLink]->link[link] = node;
                        root.link[reverseLink] = node;
                }  // end method

                // Remove a node from this list. The client is responsible to
                // ensure
                // that the node being removed belongs actually to this lists.
                void remove(LegalListNode<T> *node) {
                        node->prev->next = node->next;
                        node->next->prev = node->prev;
                        counter--;
                }  // end method

                // Merge nodes from absorbed lists to this list (absorber).
                void merge(LegalList<T> *absorbed) {
                        //[TODO] Use link with HEAD/TAIL instead of prev/next.
                        LegalList<T> *absorber = this;

                        absorber->tail()->next = absorbed->head();
                        absorbed->head()->prev = absorber->tail();
                        absorber->root.prev = absorbed->tail();
                        absorber->root.prev->next = &absorber->root;
                        absorber->counter += absorbed->counter;

                        absorbed->counter = 0;
                        absorbed->root.link[HEAD] = &absorbed->root;
                        absorbed->root.link[TAIL] = &absorbed->root;
                }  // end method

                // Indicate if this node is the root node of this list.
                bool isRoot(const LegalListNode<T> *node) const {
                        return node == &root;
                }

                // Shortcuts to access head and tail nodes.
                LegalListNode<T> *head() { return root.link[TAIL]; }
                LegalListNode<T> *tail() { return root.link[HEAD]; }

                // Return the guts of the root node.
                T &getRootGuts() { return (T &)root; }

                // Indicate if this list is empty.
                bool empty() const { return counter == 0; }

                // Return the number of elements.
                int size() const { return counter; }

                // For each methods.
                Range<NodeCollection> allNodes(const ListLink direction) {
                        return NodeCollection(*this, direction);
                }  // end method

                Range<NodeCollection> allNodesFromHeadToTail() {
                        return allNodes(TAIL);
                }  // end method

                Range<NodeCollection> allNodesFromTailToHead() {
                        return allNodes(HEAD);
                }  // end method

                // Auxiliary method. Exchange two neighbor nodes. node0 must be
                // at the
                // left of node1.
                static void swap_neighbors(LegalListNode<T> *node0,
                                           LegalListNode<T> *node1) {
                        node0->link[HEAD]->link[TAIL] = node1;
                        node1->link[TAIL]->link[HEAD] = node0;

                        node0->link[TAIL] = node1->link[TAIL];
                        node1->link[HEAD] = node0->link[HEAD];

                        node0->link[HEAD] = node1;
                        node1->link[TAIL] = node0;
                }  // end for

                // Auxiliary method. Exchange two nodes which must not be
                // neighbors.
                static void swap_non_neighbors(LegalListNode<T> *node0,
                                               LegalListNode<T> *node1) {
                        std::swap(node0->link, node1->link);

                        node0->link[HEAD]->link[TAIL] = node0;
                        node0->link[TAIL]->link[HEAD] = node0;

                        node1->link[HEAD]->link[TAIL] = node1;
                        node1->link[TAIL]->link[HEAD] = node1;
                }  // end for

                // Exchange two nodes from their lists. It doesn't matter if
                // they belong
                // to the same list or not.
                static void swap(LegalListNode<T> *node0,
                                 LegalListNode<T> *node1) {
                        // Remember that the both nodes may be direct neighbors.

                        // [TODO] Is there a way to perform the swap without
                        // caring if the
                        // nodes are neighbour or not?

                        if (node0->link[TAIL] == node1) {
                                swap_neighbors(node0, node1);
                        } else if (node0->link[HEAD] == node1) {
                                swap_neighbors(node1, node0);
                        } else {
                                swap_non_neighbors(node0, node1);
                        }  // end else
                }          // end method

                // Debug method. Check if the double-linked list is correctly
                // chained.
                void checkChain() {
                        LegalListNode<T> *curr;
                        LegalListNode<T> *prev;
                        LegalListNode<T> *next;

                        curr = root.link[TAIL];
                        prev = &root;
                        while (curr != &root) {
                                assert(curr->prev == prev);
                                prev = curr;
                                curr = curr->link[TAIL];
                        }  // end while

                        curr = root.link[HEAD];
                        next = &root;
                        while (curr != &root) {
                                assert(curr->next == next);
                                next = curr;
                                curr = curr->link[HEAD];
                        }  // end while
                }          // end method
        };                 // end struct

        ////////////////////////////////////////////////////////////////////////////
        // Jezz Legalizer
        ////////////////////////////////////////////////////////////////////////////

        // Description:
        //   Legalize the design.
        //
        // Limitations:
        //   - Assumes all rows have the same site width.
        //   - Assumes all rows have the same height.
        //   - No multi-row cells.
        //
        // TODO:
        //   - Improve the displacement of surrounding node when a new node is
        //     inserted.
        //   - Handle overflow through obstacles.
        //   - Eliminate the interdependence between Jezz and the Placer. Jezz
        //     should be transformed in an independent class.

        struct JezzNodeGuts;  // forward declaration

       public:
        struct JezzRow;  // forward declaration
        typedef LegalListNode<JezzNodeGuts> JezzNode;

       private:
        // Map Jezz nodes to instances.
        Rsyn::Attribute<Rsyn::Instance, JezzNode *> clsMapInstancesToJezzNodes;

        struct JezzNodeGuts {
                Rsyn::Cell reference;  // reference to design cell
                int id;                // index in the jezz node list
                int x;                 // x position
                int w;                 // width
                bool ws;               // whitespace
                bool fixed;            // fixed
                bool obstacle;         // obstacle node
                bool deleted;          // indicate that this node was deleted
                double weight;

                // [TODO] Consider keeping only one of these...
                DBU refx;          // reference x position
                DBU refy;          // reference y position
                int snapped_refx;  // snapped reference x
                int snnaped_refy;  // snapped reference y

                JezzRow *row;

                int xmin() const { return x; }
                int xmax() const { return x + w; }
                int width() const { return w; }

                bool isWhitespace() const { return ws; }
                bool isFixed() const { return fixed; }
                bool isObstacle() const { return obstacle; }
                bool isDeleted() const { return deleted; }

                // Reset most of properties of this node, but keep its type (eg.
                // cell,
                // whitespace, obstacle).
                void reset() {
                        reference = nullptr;
                        row = nullptr;
                        w = 0;
                        weight = 1;
                        fixed = false;
                }  // end method

                JezzNodeGuts() {
                        reset();

                        id = -1;
                        deleted = false;
                        ws = false;
                        obstacle = false;
                }  // end constructor

        };  // end struct

       public:
        struct JezzRow : LegalList<JezzNodeGuts> {
                DBU origin_x;  // min x in design coordinates
                DBU origin_y;  // min y in design coordinates

                Rsyn::PhysicalOrientation clsOrientation;
                int x;
                int y;  // y in Jezz coordinates (basically row index)
                int w;  // width

                std::vector<JezzNode *> slots;

                int xmin() const { return x; }
                int xmax() const { return x + w; }
                int width() const { return w; }
                Rsyn::PhysicalOrientation getOrientation() const {
                        return clsOrientation;
                }
                JezzRow() {
                        origin_x = 0;
                        origin_y = 0;
                        x = 0;
                        y = 0;
                        w = 0;
                        clsOrientation =
                            Rsyn::PhysicalOrientation::ORIENTATION_INVALID;
                }  // end constructor
        };         // end struct

       private:
        UpdatePositionCallback clsJezzUpdatePositionCallback;

        bool clsJezzInitialized;
        bool clsJezzTieBreak;

        // This flag is set to true if legalize (full legalization) was called
        // at
        // least once.
        bool clsJezzIncrementalMode;

        DBU clsJezzSiteWidth;
        DBU clsJezzRowHeight;
        // DBU clsJezzOriginX;
        // DBU clsJezzOriginY;

        int clsJezzNumSites;
        int clsJezzNumRows;

        DBU clsJezzSlotLengthInUserUnits;
        int clsJezzSlotLengthInJezzUnits;
        Bounds clsJezzBounds;

        std::vector<JezzRow> clsJezzRows;
        std::deque<JezzNode> clsJezzNodes;
        std::deque<JezzNode> clsJezzObstacles;
        std::deque<JezzNode> clsJezzWhitespaces;

        // Used to recycle nodes.
        std::list<JezzNode *> clsJezzDeletedWhitespaces;
        std::list<JezzNode *> clsJezzDeletedNodes;

        std::map<std::string, std::deque<JezzNode>> clsJezzStoredSolution;

        // <cumulative cost, maximum cost (tie break)>
        std::vector<std::pair<double, double>> compoundOffsetToLeft;
        std::vector<std::pair<double, double>> compoundOffsetToRight;

        void jezz_InitRows();
        void jezz_DeleteWhitespaceNodes();

        double jezz_InsertNode(JezzRow *jezzRow, JezzNode *jezzNode,
                               const int x, const bool trial, bool &overflow,
                               double &smallestMaxDisplacementCost);
        double jezz_InsertNode(JezzRow *jezzRow, JezzNode *jezzNode,
                               const int x, const bool trial, bool &overflow) {
                double dummy;
                return jezz_InsertNode(jezzRow, jezzNode, x, trial, overflow,
                                       dummy);
        }  // end method

        void jezz_RemoveZeroLengthWhitespace(JezzNode *jezzNode);

        void jezz_AdjustShiftingCostToSide(
            JezzRow *jezzRow, JezzNode *jezzNode, const int offset,
            const ListLink direction, int &overflow,
            std::vector<std::pair<double, double>> &disrupt);
        int jezz_AdjustPositionConsolidate(JezzRow *jezzRow, JezzNode *jezzNode,
                                           const int offset,
                                           const ListLink direction);

        int jezz_isNodeApproachingOriginal(JezzNode *jezzNode, int shifts);

        JezzNode *jezz_CreateWhitespaceNode();
        void jezz_DeleteWhitespaceNode(JezzNode *whitespace);

        void jezz_ResetNode(JezzNode *jezzNode);

       public:
        JezzNode *getJezzNode(Rsyn::Instance instance) {
                return clsMapInstancesToJezzNodes[instance];
        }
        bool isInitialized() const { return clsInitialized; }

        int jezz_SnapPositionX(const DBU x) const {
                return (x - clsJezzBounds[LOWER][X]) / clsJezzSiteWidth;
        }
        int jezz_SnapPositionY(const DBU y) const {
                return jezz_GetRowIndex(y);
        }

        int jezz_SnapSizeX(const DBU w) const { return (w / clsJezzSiteWidth); }
        int jezz_SnapSizeY(const DBU h) const { return (h / clsJezzRowHeight); }

        // Compute the width/height using the extremities (not the width in
        // design
        // units) to avoid precision issues.
        int jezz_SnapSizeX(const DBU xmin, const DBU xmax) const {
                return jezz_SnapPositionX(xmax) - jezz_SnapPositionX(xmin);
        }
        int jezz_SnapSizeY(const DBU ymin, const DBU ymax) const {
                return jezz_SnapPositionY(ymax) - jezz_SnapPositionY(ymin);
        }

        DBU jezz_UnsnapPositionX(const int x) const {
                return clsJezzBounds[LOWER][X] + x * clsJezzSiteWidth;
        }
        DBU jezz_UnsnapPositionY(const int y) const {
                return clsJezzBounds[LOWER][Y] + y * clsJezzRowHeight;
        }

        DBU jezz_UnsnapSizeX(const int w) const { return w * clsJezzSiteWidth; }
        DBU jezz_UnsnapSizeY(const int h) const { return h * clsJezzRowHeight; }

       private:
        bool jezz_ValidRowIndex(const int row) const {
                return (row >= 0 && row < clsJezzRows.size());
        }
        bool jezz_ValidSiteIndex(const int col) const {
                return (col >= 0 && col < clsJezzNumSites);
        }

        // Returns the index of the row associated to the y coordinate in user
        // space. For points that lie exactly on the shared horizontal edge of
        // rows,
        // resolve the ambiguity using the resolution parameter. If top is
        // provided,
        // the top row will be returned; if bottom, the bottom row.
        int jezz_GetRowIndex(
            const DBU y, const SharedRowEdgeResolution resolution = TOP) const {
                const int index =
                    (y - clsJezzBounds[LOWER][Y]) / clsJezzRowHeight;
                if ((resolution == BOTTOM) &&
                    ((y - clsJezzBounds[LOWER][Y]) % clsJezzRowHeight == 0)) {
                        return index - 1;
                } else {
                        return index;
                }  // end else
        }          // end method

        // Similar to jezz_GetRowIndex() but clamp the row indexes to be in the
        // valid range [0, num rows - 1].
        int jezz_GetRowIndexBounded(
            const DBU y, const SharedRowEdgeResolution resolution = TOP) const {
                return std::min(clsJezzNumRows - 1,
                                std::max(0, jezz_GetRowIndex(y, resolution)));
        }  // end method

        JezzRow *jezz_GetRowByIndex(const int row) { return &clsJezzRows[row]; }
        DBU jezz_GetRowY(const int row) const {
                return clsJezzBounds[LOWER][Y] + (clsJezzRowHeight * row);
        }

        bool jezz_Overlap(const int x, JezzNode *jezzNode);
        bool jezz_Enclosed(JezzNode *jezzNodeOutter, JezzNode *jezzNodeInner);

        int jezz_GetSlot(JezzRow *jezzRow, const int x);
        bool jezz_ValidCachedNode(JezzRow *jezzRow, JezzNode *jezzNode);
        void jezz_UpdateCache(JezzNode *jezzNode);
        void jezz_UpdateCacheAll();

        JezzNode *jezz_FindOverlappingNodeNonCached(JezzRow *jezzRow,
                                                    const int x);
        JezzNode *jezz_FindOverlappingNodeCached(JezzRow *jezzRow, const int x);

        JezzNode *jezz_FindOverlappingNode(JezzRow *jezzRow, const int x) {
                return clsJezzIncrementalMode
                           ? jezz_FindOverlappingNodeCached(jezzRow, x)
                           : jezz_FindOverlappingNodeNonCached(jezzRow, x);

                //		// *** Debug ****
                //		JezzNode *jezzNodeCached =
                // jezz_FindOverlappingNodeCached(jezzRow, x);
                //		JezzNode *jezzNodeNonCached =
                // jezz_FindOverlappingNodeNonCached(jezzRow, x);
                //
                //		if (jezzNodeCached != jezzNodeNonCached) {
                //			std::cout << "Jezz: debug cache
                // system\n";
                //			std::cout << "x = " << x << "\n";
                //			std::cout << "row = " << jezzRow <<
                //"\n";
                //			jezz_PrintNode(jezzNodeCached,
                //"cached");
                //			jezz_PrintNode(jezzNodeNonCached,
                //"non-cached");
                //		} // end if
                //
                //		return jezzNodeCached;
        }  // end method

        void jezz_UpdatePositionsAll();
        void jezz_UpdatePositions(JezzRow *legRow);

        void reportFinalResults(std::ostream &out = std::cout);
        void reportAverageCellDisplacement(std::ostream &out = std::cout);

       public:
        Jezz();

        bool jezz_LegalizeNode(JezzNode *jezzNode);
        bool jezz_Legalize();

        void jezz_Cleanup();

        JezzNode *jezz_DefineNode(Rsyn::Cell cell, const DBU xmin,
                                  const DBU ymin, const DBU xmax,
                                  const DBU ymax);

        void jezz_DefineObstacleAtRowUsingJezzCoordinates(const int row,
                                                          const int x,
                                                          const int w);

        void jezz_DefineObstacle(const DBU xmin, const DBU ymin, const DBU xmax,
                                 const DBU ymax);

        void jezz_RemoveNode(JezzNode *jezzNode);

        bool jezz_IsInitialized() { return clsJezzInitialized; }

        ////////////////////////////////////////////////////////////////////////////
        // Jezz: Storing and Recovery a Legalization Solution
        ////////////////////////////////////////////////////////////////////////////
        void jezz_storeSolution(const std::string name = "_default");

        // Call this when most cells have changed.
        void jezz_restoreSolutionFull(const std::string name = "_default");

        // Call this when few cells have changed.
        void jezz_restoreSolutionIncremental(
            const std::string name = "_default");

        void jezz_diffSolutions(
            const std::string solution0, const std::string solution1,
            std::vector<std::tuple<Rsyn::Cell, double, double>> &diff);

        ////////////////////////////////////////////////////////////////////////////
        // Jezz: Debugging
        ////////////////////////////////////////////////////////////////////////////

        bool jezz_CheckRowConsistency_HeadToTail(JezzRow *jezzRow);
        bool jezz_CheckRowConsistency_TailToHead(JezzRow *jezzRow);
        bool jezz_CheckRowConsistency(JezzRow *jezzRow);

        bool jezz_CheckNodeIsInTheRow(JezzRow *jezzRow, JezzNode *jezzNode);

        void jezz_StressTest();
        void jezz_StressTest_CacheSystem();

        void jezz_PrintNode(JezzNode *jezzNode, const std::string &lable = "");

        ////////////////////////////////////////////////////////////////////////////
        // Jezz: Detailed Placement Methods
        ////////////////////////////////////////////////////////////////////////////

        // Return the jezz node at (x, y). May be null if out of bounds.
        JezzNode *jezz_dp_FindNodeAtPosition(const DBU x, const DBU y);

        // Try to insert a jezz node at around (x, y). It may fail due to
        // overflow.
        void jezz_dp_InsertNode(JezzNode *jezzNode, const DBU x, const DBU y,
                                bool &overflow);
        void jezz_dp_InsertNodeUsingJezzCoordinates(JezzNode *jezzNode,
                                                    const int jx, const int jy,
                                                    bool &overflow);

        // Remove a jezz node from the jezz data structure.
        void jezz_dp_RemoveNode(JezzNode *jezzNode);

        // Swap node with its neighbor. Client is responsible to check if the
        // neighbor is not the root node.
        void jezz_dp_SwapNeighbours(JezzNode *jezzNode0, const ListLink link);

        // Change jezz node to fixed or movable
        void jezz_dp_Change_Fixed(JezzNode *jezzNode, const bool fixed);

        // Get neighbor.
        JezzNode *jezz_dp_GetNeighbor(JezzNode *jezzNode, const ListLink link) {
                return jezzNode->link[link];
        }  // end method

        // Get x position in design coordinates.
        DBU jezz_dp_GetX(JezzNode *jezzNode) {
                return jezz_UnsnapPositionX(jezzNode->x);
        }  // end method

        // Get y position in design coordinates.
        DBU jezz_dp_GetY(JezzNode *jezzNode) {
                return jezzNode->row->origin_y;
        }  // end method

        // Get node bounds in design coordinates and return true if the node is
        // legalized, return false otherwise.
        bool jezz_dp_GetNodeBounds(JezzNode *jezzNode, DBU &xmin, DBU &ymin,
                                   DBU &xmax, DBU &ymax) {
                if (jezz_dp_IsLegalized(jezzNode)) {
                        xmin = jezz_UnsnapPositionX(jezzNode->xmin());
                        ymin = jezzNode->row->origin_y;
                        xmax = jezz_UnsnapPositionX(jezzNode->xmax());
                        ymax = jezzNode->row->origin_y + clsJezzRowHeight;
                        return true;
                } else {
                        return false;
                }  // end else
        }          // end method

        // Verify if the jezz node is a valid node (not the root node).
        bool jezz_dp_IsValid(JezzNode *jezzNode) {
                return !jezzNode->row->isRoot(jezzNode);
        }  // end method

        // Verify if this jezz node is legalized.
        bool jezz_dp_IsLegalized(const JezzNode *jezzNode) {
                return jezzNode->row != nullptr;
        }  // end method

        // Update the reference position of an unlegalized node. The reference
        // position is later used during the legalization.
        void jezz_dp_UpdateReferencePosition(JezzNode *jezzNode, const DBU x,
                                             const DBU y) {
                jezzNode->refx = x;
                jezzNode->refy = y;
        }  // end method

        // Update the weight associate to a node.
        void jezz_dp_UpdateWeight(JezzNode *jezzNode, const double weight) {
                jezzNode->weight = weight;
        }  // end method

        DBU jezz_GetAvailableFreespace(JezzNode *jezzNode) {
                if (!jezz_dp_IsLegalized(jezzNode)) return 0;
                JezzNode *left = jezzNode->prev;
                JezzNode *right = jezzNode->next;
                int writeSpace = left->ws ? left->w : 0;
                writeSpace += right->ws ? right->w : 0;
                return jezz_UnsnapSizeX(writeSpace);
        }  // end method

        DBU jezz_GetAvailableFreespaceLeft(JezzNode *jezzNode) {
                if (!jezz_dp_IsLegalized(jezzNode)) return 0;
                JezzNode *left = jezzNode->prev;
                int writeSpace = left->ws ? left->w : 0;
                return jezz_UnsnapSizeX(writeSpace);
        }  // end method

        DBU jezz_GetAvailableFreespaceRight(JezzNode *jezzNode) {
                if (!jezz_dp_IsLegalized(jezzNode)) return 0;
                JezzNode *right = jezzNode->next;
                int writeSpace = right->ws ? right->w : 0;
                return jezz_UnsnapSizeX(writeSpace);
        }  // end method

        ////////////////////////////////////////////////////////////////////////////
        // Whitespaces
        ////////////////////////////////////////////////////////////////////////////
       public:
        struct WhitespaceDescriptor {
                JezzNode *node;  // whitespace node
                DBU x;           // minimum x of the whitespace
                DBU y;           // minumum y of the whitespace
                DBU w;           // whitespace width

                // Position where the cell should be inserted inside whitespace
                // accounting for snapping noise. Note that the whitespace can
                // be much
                // larger than the node.
                DBU x_insertion;  // x_insertion >= x

                // Distance from target position (computed based on the
                // insertion
                // position).
                DBU distance;

                WhitespaceDescriptor() {
                        node = nullptr;
                        x = std::numeric_limits<DBU>::quiet_NaN();
                        y = std::numeric_limits<DBU>::quiet_NaN();
                        x_insertion = std::numeric_limits<DBU>::quiet_NaN();
                        w = std::numeric_limits<DBU>::quiet_NaN();
                        distance = std::numeric_limits<DBU>::quiet_NaN();
                }  // end constructor

                WhitespaceDescriptor(JezzNode *node, const DBU x, const DBU y,
                                     const DBU x_insertion, const DBU w,
                                     const DBU distance)
                    : node(node),
                      x(x),
                      y(y),
                      x_insertion(x_insertion),
                      w(w),
                      distance(distance) {}  // end constructor

                bool operator<(const WhitespaceDescriptor &rhs) const {
                        return distance < rhs.distance;
                }  // end operator

                bool operator==(const WhitespaceDescriptor &rhs) const {
                        return node == rhs.node;
                }  // end operator
        };         // end struct

       private:
        // Default whitespace filter functor that does nothing.
        struct DefaultWhitespaceFilterFunctor {
                inline bool operator()(const WhitespaceDescriptor &dscp) const {
                        return false;
                }
        };

        template <class Functor>
        inline void jezz_dp_SearchWhitespaces_Row(
            JezzRow *row, const DBU targetX, const DBU targetY,
            const DBU maxDisplacementFromTarget, int thresholdWidth,
            int maxWhitespaces, std::set<WhitespaceDescriptor> &whitespaces,
            Functor &filter);

        template <class Functor>
        inline void jezz_dp_SearchWhitespaces_Sweep(
            JezzNode *referenceNode, const DBU targetX, const DBU targetY,
            const DBU maxDisplacementFromTarget, ListLink direction,
            int thresholdWidth, int maxWhitespaces,
            std::set<WhitespaceDescriptor> &whitespaces, Functor &filter);

       public:
        template <class Functor = DefaultWhitespaceFilterFunctor>
        inline void jezz_dp_SearchWhitespaces(
            const DBU targetX, const DBU targetY, const DBU minWidth,
            const DBU maxDisplacementFromTarget, const int maxWhitespaces,
            std::set<WhitespaceDescriptor> &whitespaces,
            Functor filter = DefaultWhitespaceFilterFunctor());

        ////////////////////////////////////////////////////////////////////////////
        // General methods
        ////////////////////////////////////////////////////////////////////////////

        int getNumRows() const { return clsJezzNumRows; }

        const std::vector<JezzRow> &getRows() { return clsJezzRows; }
        const std::deque<JezzNode> &getNodes() { return clsJezzNodes; }

        ////////////////////////////////////////////////////////////////////////////
        // Ranged Loops Collections
        ////////////////////////////////////////////////////////////////////////////

        class NodeCollection {
               private:
                Jezz &jezz;
                LegalListNode<JezzNodeGuts> *node;
                JezzRow *row;
                int k;

                void moveToTheKthRow() {
                        row = &jezz.clsJezzRows[k];
                        node = row->root.link[TAIL];
                }  // end method

               public:
                NodeCollection(Jezz &jezz) : jezz(jezz) {
                        k = 0;
                        moveToTheKthRow();
                }  // end constructor

                bool filter() { return false; }

                bool stop() { return k == jezz.getNumRows(); }

                void next() {
                        node = node->link[TAIL];
                        if (row->isRoot(node)) {
                                k++;
                                if (k < jezz.getNumRows()) moveToTheKthRow();
                        }  // end if
                }          // end method

                JezzNode *current() { return node; }
        };  // end class

        ////////////////////////////////////////////////////////////////////////////
        // For each methods
        ////////////////////////////////////////////////////////////////////////////

        Range<NodeCollection> allNodes() {
                return NodeCollection(*this);
        }  // end method

        ////////////////////////////////////////////////////////////////////////////
        // Destructor
        ////////////////////////////////////////////////////////////////////////////
        // virtual ~Jezz();

};  // end class

//##############################################################################
// Implementation
//##############################################################################

template <class Functor>
void Jezz::jezz_dp_SearchWhitespaces_Row(
    JezzRow *row, const DBU targetX, const DBU targetY,
    const DBU maxDisplacementFromTarget, int thresholdWidth, int maxWhitespaces,
    std::set<WhitespaceDescriptor> &whitespaces, Functor &filter) {
        const int x = jezz_SnapPositionX(targetX);

        JezzNode *referenceNode = jezz_FindOverlappingNode(row, x);
        if (referenceNode) {
                jezz_dp_SearchWhitespaces_Sweep(
                    referenceNode, targetX, targetY, maxDisplacementFromTarget,
                    HEAD, thresholdWidth, maxWhitespaces, whitespaces, filter);
                jezz_dp_SearchWhitespaces_Sweep(
                    referenceNode->link[TAIL], targetX, targetY,
                    maxDisplacementFromTarget, TAIL, thresholdWidth,
                    maxWhitespaces, whitespaces, filter);
        } else {
                std::cout << "[BUG] Overlapping node not found.\n";
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

template <class Functor>
void Jezz::jezz_dp_SearchWhitespaces_Sweep(
    JezzNode *referenceNode, const DBU targetX, const DBU targetY,
    const DBU maxDisplacementFromTarget, ListLink direction, int thresholdWidth,
    int maxWhitespaces, std::set<WhitespaceDescriptor> &whitespaces,
    Functor &filter) {
        JezzNode *node = referenceNode;
        JezzRow *row = node->row;

        const DBU ymin = row->origin_y;
        const DBU dy = std::abs(ymin - targetY);
        if (dy > maxDisplacementFromTarget) return;

        while (!row->isRoot(node)) {
                const DBU xmin = jezz_UnsnapPositionX(node->xmin());
                const DBU xmax =
                    jezz_UnsnapPositionX(node->xmax() - thresholdWidth);

                // Insertion position indicates where the cell should be
                // inserted inside
                // whitespace. Remember that a whitespace can be larger than the
                // cell
                // being inserted. We call snap and unsnap to account to use the
                // actual
                // x position after snapping to the Jezz grid.
                const DBU xinsertion = jezz_UnsnapPositionX(jezz_SnapPositionX(
                    std::min(std::max(xmin, targetX), xmax)));

                // To compute the distance to the target position, assume that
                // the node
                // will be placed abutted to the whitespace edge closer to the
                // target
                // position.
                const DBU dx = std::abs(xinsertion - targetX);
                const DBU distance = dx + dy;
                if (distance > maxDisplacementFromTarget) break;

                // If we already have the number of requested whitespaces, stop
                // this
                // search if the distance of the upper bound is greater than the
                // largest
                // distance in the set.
                if (whitespaces.size() >= maxWhitespaces &&
                    distance >= whitespaces.rbegin()->distance) {
                        break;
                }  // end if

                if (node->isWhitespace()) {
                        if (node->width() >= thresholdWidth) {
                                const DBU w = jezz_UnsnapSizeX(node->width());

                                const WhitespaceDescriptor dscp(
                                    node, xmin, ymin, xinsertion, w, distance);
                                if (!filter(dscp)) {
                                        // If we already have the number of
                                        // requested whitespaces we
                                        // remove the farthest whitespace
                                        // (w.r.t. the target position)
                                        // from the set. Note that we don't need
                                        // to check if
                                        // "distance < farthest distance" as
                                        // this was asserted before.
                                        // By removing the farthest whitespace
                                        // we keep the set size
                                        // limited to the maximum number of
                                        // whitespaces requested.
                                        if (whitespaces.size() >=
                                            maxWhitespaces) {
                                                whitespaces.erase(
                                                    --whitespaces.end());
                                        }  // end if

                                        whitespaces.insert(dscp);
                                }  // end if
                        }          // end if
                }                  // end if

                // Advance
                node = node->link[direction];
        }  // end while
}  // end method

// -----------------------------------------------------------------------------

template <class Functor>
void Jezz::jezz_dp_SearchWhitespaces(
    const DBU targetX, const DBU targetY, const DBU minWidth,
    const DBU maxDisplacementFromTarget, const int maxWhitespaces,
    std::set<WhitespaceDescriptor> &whitespaces, Functor filter) {
        whitespaces.clear();

        const int ymin =
            jezz_GetRowIndexBounded(targetY - maxDisplacementFromTarget);
        const int ymax =
            jezz_GetRowIndexBounded(targetY + maxDisplacementFromTarget);

        const int ycenter =
            std::min(ymax, std::max(ymin, jezz_GetRowIndex(targetY)));

        const int thresholdWidth = jezz_SnapSizeX(minWidth);

        jezz_dp_SearchWhitespaces_Row(&clsJezzRows[ycenter], targetX, targetY,
                                      maxDisplacementFromTarget, thresholdWidth,
                                      maxWhitespaces, whitespaces, filter);

        bool keepGoing;
        int y0 = ycenter + 1;
        int y1 = ycenter - 1;
        do {
                keepGoing = false;

                if (y0 <= ymax) {
                        jezz_dp_SearchWhitespaces_Row(
                            &clsJezzRows[y0], targetX, targetY,
                            maxDisplacementFromTarget, thresholdWidth,
                            maxWhitespaces, whitespaces, filter);
                        keepGoing = true;
                        y0++;
                }  // end if

                if (y1 >= ymin) {
                        jezz_dp_SearchWhitespaces_Row(
                            &clsJezzRows[y1], targetX, targetY,
                            maxDisplacementFromTarget, thresholdWidth,
                            maxWhitespaces, whitespaces, filter);
                        keepGoing = true;
                        y1--;
                }  // end if
        } while (keepGoing);
}  // end method

#endif
