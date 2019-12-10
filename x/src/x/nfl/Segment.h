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
 * File:   Segment.h
 * Author: isoliveira
 *
 * Created on 12 de Março de 2018, 17:32
 */

#ifndef NFL_SEGMENT_H
#define NFL_SEGMENT_H

#include "Node.h"
#include "NFLegalUtil.h"

namespace NFL {

class Row;
class Bin;

class Segment {
private:
	Bounds clsBounds;
	DBUxy clsBinLength;
	Segment * clsLeft = nullptr;
	Segment * clsRight = nullptr;
	Row * clsRow = nullptr;
	Node * clsLeftNode = nullptr;
	Node * clsRighNode = nullptr;
	std::vector <Bin> clsBins;
	std::vector <Segment*> clsUpper;
	std::vector <Segment*> clsLower;
	std::vector <Segment*> clsNeighbors;
	std::vector <Segment*> clsVerticalNeighbors;
	std::deque<Node> clsWhitespaceNodes;
	std::set<int> clsDeletedWhitespaceNodes;
	std::set<std::string> clsSegments;

	int clsId = -1;
	const int clsWhitespaceThreshold = 100000000;
	int clsWhitespaceLastId = 100000000;
	DBU clsNodeUsage = 0;
public:
	Segment() = default;
	virtual ~Segment() = default;

	void initBins();

	//! @brief initialize segment to legalize instances
	void initLegalize();

	void setBounds(const Bounds & bds) {
		clsBounds = bds;
	} // end method

	void setId(const int id) {
		clsId = id;
	} // end method

	void setBinLength(const DBUxy length) {
		clsBinLength = length;
	} // end method 

	DBU getWidth() const {
		return clsBounds.computeLength(X);
	} // end method

	DBUxy getDefaultBinLength() const {
		return clsBinLength;
	} // end method 

	DBU getDefaultBinLength(const Dimension dim) const {
		return clsBinLength[dim];
	} // end method 

	DBUxy getCoordinate(const Boundary bound) const {
		return clsBounds.getCoordinate(bound);
	} // end method 

	DBU getCoordinate(const Boundary bound, const Dimension dim) const {
		return clsBounds.getCoordinate(bound, dim);
	} // end method

	DBUxy getPosition() const {
		return getCoordinate(LOWER);
	} // end method

	DBU getPosition(const Dimension dim) const {
		return getCoordinate(LOWER, dim);
	} // end method

	DBU getPlaceableSpace() const {
		return getWidth();
	} // end method 

	DBU getSupply() const {
		return std::max((DBU) 0, getUsage() - getPlaceableSpace());
	} // end method 

	DBU getDemand() const {
		return std::max((DBU) 0, getPlaceableSpace() - getUsage());
	} // end method 

	DBU getUsage() const {
		return clsNodeUsage;
	} // end method 

	const Bounds & getBounds() const {
		return clsBounds;
	} // end method

	void setLeft(Segment * left);
	void setRight(Segment * right);
	void addLower(Segment * lower);
	void addUpper(Segment * upper);

	int getId() const {
		return clsId;
	} // end method

	int getNumBins() const {
		return clsBins.size();
	} // end method 

	bool hasNeighborSegments() const {
		return !clsNeighbors.empty();
	} // end method

	bool hasLefSegment() const {
		return clsLeft;
	} // end method

	bool hasRightSegment() const {
		return clsRight;
	} // end method

	bool hasLowerSegments() const {
		return !clsLower.empty();
	} // end method

	bool hasUpperSegments() const {
		return !clsUpper.empty();
	} // end method

	bool hasVerticalNeighbors() const {
		return !clsVerticalNeighbors.empty();
	} // end method 

	bool isInsidePosition(const DBUxy & pos) const;

	Segment * getLeftSegment() {
		return clsLeft;
	} // end method

	Segment * getRightSegment() {
		return clsRight;
	} // end method

	Bin * getFrontBin() {
		return &clsBins.front();
	} // end method

	Bin * getBackBin() {
		return &clsBins.back();
	} // end method 

	Node * getLeftNode() {
		return clsLeftNode;
	} // end method 

	Node * getRightNode() {
		return clsRighNode;
	} // end method 

	const std::vector <Segment*>& allLower() const {
		return clsLower;
	} // end method

	const std::vector<Segment*>& allUpper() const {
		return clsUpper;
	} // end method

	const std::vector<Segment*>& allNeighbors() const {
		return clsNeighbors;
	} // end method

	const std::vector<Segment*> & allVerticalNeighbors() const {
		return clsVerticalNeighbors;
	} // end method 

	const std::vector<Bin>& allBins() const {
		return clsBins;
	} // end method

	std::vector<Bin>& allBins() {
		return clsBins;
	} // end method

	void setRow(Row * row) {
		clsRow = row;
	} // end method

	Row * getRow() {
		return clsRow;
	}// end method

	std::string getFullId(const std::string & separator = ":") const;

	void insertNode(Node * inst);

	void removeNode(Node * inst);

	Bin * getBinByPosition(const DBU posX);

	Bin * getBinByIndex(const int index);

	int getBinIndex(const DBU posX, const bool roundUp = false);

	DBU getDisplacement(const DBUxy pos);

	bool computeLegalPosition(LegalAux & legalAux);
	bool legalizeNode(LegalAux & legalAux);

protected:
	bool isEnclosedNodeInWhitespace(LegalAux & legalAux);
	void getOverlapNode(LegalAux & legalAux);

	//TODO REMOVE Methods
	void computeLeftShift(LegalAux & legalAux, std::vector<DBU> & cost);
	void computeRightShift(LegalAux & legalAux, std::vector<DBU> & cost);
	//END TODO 

	void computeShiftToSide(Node * refNode, const DBU requiredMove,
		DBU & overflowMove, std::vector<DBU> &cost, const bool isToLeft = true);

	//TODO REMOVE Method
	void computeOptimizedPosition(LegalAux & legalAux, std::vector<DBU> & leftCost,
		std::vector<DBU> &rightCost);
	DBU computeDifferencePositionCost(const DBU fromPosX, const DBU toPosX, const DBU initPosX);
	void computeLegalizationCost(LegalAux & legalAux);
	//END TODO 

	DBU computePositionCost(const DBU initPos, const DBU currentPos, const DBU targetPos);

	//TODO REMOVE Method
	void openRequiredSpace(LegalAux & legalAux);
	void moveLeftCells(LegalAux & legalAux);
	void moveRightCells(LegalAux & legalAux);
	//END TODO 

	void moveLegalizedNodes(Node * refNode, const DBU requiredMove, const bool isToLeft = true);
	void devideWhitespace(Node * inst, Node * reference);
	void connectNodes(Node * left, Node * node, Node * right);
	void connectNodes(Node * left, Node *right);
	DBU computeDisplacementCost(Node * inst, DBUxy targetPos);
	void optimizeCellDisplacement(Node * node);
	void swapNodes(Node * node0, Node * node1);

	void updateLeftAndRightSegmentNodes(Node * left, Node * node, Node * right);
	void updateLeftSegmentNode(Node * left);
	void updateRightSegmentNode(Node * right);
	void setLeftSegmentNode(Node * left);
	void setRightSegmentNode(Node * right);

	// Legalization methods
	Node * createWhitespaceNode(const Bounds & bounds);
	void deleteWhitespaceNode(Node * inst);
	void disconectNode(Node * inst);


	// Auxiliar methods 
	int getNextWhitespaceId();
	DBU getSiteWidth();
	int getNumSites(const DBU length);
	bool isSiteAligned(Node * node);
	bool isInsideSegment(Node * node);
	bool hasZeroWidth(Node * node);








public:
	void checkSanityBinId();
	const void checkSanityBinsConnected() const;
	const void checkSanityBinsInside() const;
	const void checkSanityBinsOverlap()const;
	bool checkSanitySegmentList();
	bool checkSanitySegmentInstances();
	bool checkSanitySiteAligment();
	bool checkSanityZeroWidth();
	bool checkSanityNodeBounds();
	bool checkSanityNodesInsideSegment();
	bool checkSanityCache();
	bool checkSanityBins();
	bool checkSanityNodes();

}; // end class

} // end namespace 

#endif /* NFL_SEGMENT_H */

