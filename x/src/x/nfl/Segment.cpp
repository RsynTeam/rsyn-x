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
 * File:   Segment.cpp
 * Author: isoliveira
 * 
 * Created on 12 de Março de 2018, 17:32
 */
#include <iostream>
#include "Segment.h"
#include "Bin.h"
#include "Row.h"
#include "Blockage.h"

namespace NFL {

void Segment::initBins() {
	const DBU binWidth = clsBinLength[X];
	const int numBins = roundedUpIntegralDivision(getWidth(), binWidth);
	clsBins.reserve(numBins);
	Bounds bounds = getBounds();
	const DBU rightPos = getCoordinate(UPPER, X);
	Bin * left = nullptr;
	do {
		bounds[UPPER][X] = std::min(bounds[LOWER][X] + binWidth, rightPos);
		// Merging remain space to the current bin.
		if (rightPos - bounds[UPPER][X] < binWidth)
			bounds[UPPER][X] = rightPos;
		const int id = clsBins.size();
		clsBins.push_back(Bin());
		Bin & bin = clsBins.back();
		bin.setBounds(bounds);
		bin.setId(id);
		bin.setSegment(this);
		bounds[LOWER][X] = bounds[UPPER][X];
		if (left) {
			left->setRight(&bin);
			bin.setLeft(left);
		} // end if 
		left = &bin;
		// connecting horizontal bins 
	} while (bounds[UPPER][X] + binWidth <= rightPos);
} // end method 

// -----------------------------------------------------------------------------

void Segment::initLegalize() {
	Node * inst = createWhitespaceNode(getBounds());
	clsLeftNode = inst;
	clsRighNode = inst;
	insertNode(inst);
} // end method 

// -----------------------------------------------------------------------------

void Segment::setLeft(Segment * left) {
	const std::string & id = left->getFullId();
	if (clsSegments.find(id) == clsSegments.end()) {
		clsLeft = left;
		clsNeighbors.push_back(left);
		clsSegments.insert(id);
	} // end if 
} // end method

// -----------------------------------------------------------------------------

void Segment::setRight(Segment * right) {
	const std::string & id = right->getFullId();
	if (clsSegments.find(id) == clsSegments.end()) {
		clsRight = right;
		clsNeighbors.push_back(right);
		clsSegments.insert(id);
	} // end if 
} // end method

// -----------------------------------------------------------------------------

void Segment::addLower(Segment * lower) {
	const std::string & id = lower->getFullId();
	if (clsSegments.find(id) == clsSegments.end()) {
		clsLower.push_back(lower);
		clsNeighbors.push_back(lower);
		clsSegments.insert(id);
		if (lower->getCoordinate(UPPER, Y) == getPosition(Y)) {
			clsVerticalNeighbors.push_back(lower);
		} // end if 
	} // end if 
} // end method

// -----------------------------------------------------------------------------

void Segment::addUpper(Segment * upper) {
	const std::string & id = upper->getFullId();
	if (clsSegments.find(id) == clsSegments.end()) {
		clsUpper.push_back(upper);
		clsNeighbors.push_back(upper);
		clsSegments.insert(id);
		if (upper->getPosition(Y) == getCoordinate(UPPER, Y)) {
			clsVerticalNeighbors.push_back(upper);
		} // end if 
	} // end if 
} // end method

// -----------------------------------------------------------------------------

bool Segment::isInsidePosition(const DBUxy & pos) const {
	const Bounds & bds = getBounds();
	if (pos[X] < bds[LOWER][X] || pos[X] > bds[UPPER][X]
		|| pos[Y] < bds[LOWER][Y] || pos[Y] > bds[UPPER][Y])
		return false;
	return true;
} // end method 

// -----------------------------------------------------------------------------

std::string Segment::getFullId(const std::string & separator) const {
	int rowId = clsRow->getId();
	return std::to_string(rowId) + separator + std::to_string(getId());
} // end method 

// -----------------------------------------------------------------------------

void Segment::insertNode(Node * inst) {
	int left = getBinIndex(inst->getCoordinate(LOWER, X), false);
	int right = getBinIndex(inst->getCoordinate(UPPER, X), true);
	for (int index = left; index < right; ++index) {
		Bin * bin = getBinByIndex(index);
		bin->insertNode(inst);
	} //end for 
	if (!inst->isWhitespace()) {
		clsNodeUsage += inst->getWidth();
	} // end if 
} // end method

// -----------------------------------------------------------------------------

void Segment::removeNode(Node * inst) {
	int left = getBinIndex(inst->getCoordinate(LOWER, X), false);
	int right = getBinIndex(inst->getCoordinate(UPPER, X), true);
	for (int index = left; index < right; ++index) {
		Bin * bin = getBinByIndex(index);
		bin->removeNode(inst);
	} //end for 
	if (!inst->isWhitespace()) {
		clsNodeUsage -= inst->getWidth();
	} // end if 
} // end method

// -----------------------------------------------------------------------------

Bin * Segment::getBinByPosition(const DBU posX) {
	const int id = getBinIndex(posX);
	return getBinByIndex(id);
} // end method 

// -----------------------------------------------------------------------------

Bin * Segment::getBinByIndex(const int index) {
	if (index >= 0 && index < getNumBins())
		return &clsBins[index];
	return nullptr;
} // end method 

// -----------------------------------------------------------------------------

int Segment::getBinIndex(const DBU posX, const bool roundUp) {
	const DBU delta = posX - getPosition(X);
	const DBU length = getDefaultBinLength(X);
	int numBins = roundUp ? getNumBins() : getNumBins() - 1;
	int index = static_cast<int> (delta / length);
	if (roundUp) {
		index = delta % length ? index + 1 : index;
	} // end if 
	return std::min(index, numBins);
} // end method 

// -----------------------------------------------------------------------------

DBU Segment::getDisplacement(const DBUxy pos) {
	const DBU dispY = std::abs(pos[Y] - clsBounds[LOWER][Y]);
	if (pos[X] >= clsBounds[LOWER][X] && pos[X] <= clsBounds[UPPER][X])
		return dispY;
	const DBU dispX = std::min(std::abs(clsBounds[LOWER][X] - pos[X]), std::abs(clsBounds[UPPER][X] - pos[X]));
	return dispY + dispX;
} // end method 

// -----------------------------------------------------------------------------

bool Segment::computeLegalPosition(LegalAux & legalAux) {
	
	Node * inst = legalAux.clsNode;
	DBUxy & pos = legalAux.clsPos;
	pos[X] = inst->getPosition(X);
	pos[Y] = getPosition(Y);
	pos[X] = clsRow->getSiteAdjustedPosition(inst->getPosition(X));
	legalAux.clsLegalSegment = this;




	const Bounds & segBds = getBounds();
	if (pos[X] < segBds[LOWER][X] || pos[X] > segBds[UPPER][X]) {
		legalAux.clsCost = std::numeric_limits<DBU>::max();
		return false;
	} // end if 

	getOverlapNode(legalAux);
	Node * refNode = legalAux.clsReferenceNode;

	const Bounds & overlapBds = refNode->getBounds();
	const DBU nodeWidth = inst->getWidth();


	if (!isEnclosedNodeInWhitespace(legalAux)) {

		DBU requiredMove = inst->getWidth();
		std::vector<DBU> leftCost;
		std::vector<DBU> rightCost;
		DBU leftOverflowMove = 0;
		DBU rightOverflowMove = 0;
		Node * leftRefNode = legalAux.clsReferenceNode;
		Node * rightRefNode = leftRefNode->getRightNode();
		computeShiftToSide(leftRefNode, requiredMove, leftOverflowMove,
			leftCost, true);
		computeShiftToSide(rightRefNode, requiredMove, rightOverflowMove,
			rightCost, false);


		const DBU maxOffsetLeft = requiredMove - leftOverflowMove;
		const DBU maxOffsetRight = requiredMove - rightOverflowMove;

		if (maxOffsetLeft + maxOffsetRight < requiredMove) {
			std::cout << "ERROR required move\n";
			return false;
			exit(0);
			//overflow = true;
			//return 0; // we don't return int max to avoid overflow			
		} // end for

		DBU displacementCost = std::numeric_limits<DBU>::max();

		int numRequiredSites = getNumSites(requiredMove);
		int numLeftsites = getNumSites(maxOffsetLeft);
		int numRightSites = getNumSites(maxOffsetRight);
		int shiftToLeft = -1;
		const DBU initPosX = inst->getInitialPosition(X);
		const DBU posX = inst->getPosition(X);
		const DBU siteWidth = getSiteWidth();
		//	std::cout << "--------\n";
		for (int i = numLeftsites; i >= numRequiredSites - numRightSites; --i) {
			DBU targetPosX = refNode->getCoordinate(UPPER, X) - (siteWidth * i);
			DBU nodeCost = computePositionCost(initPosX, posX, targetPosX);
			DBU currentCost = nodeCost
				+ leftCost[i]
				+ rightCost[numRequiredSites - i]
				;
			if (currentCost < displacementCost) {
				shiftToLeft = i;
				displacementCost = currentCost;
			}
		} //  end for 


		legalAux.clsLeftMove = shiftToLeft * siteWidth;
		legalAux.clsRightMove = requiredMove - legalAux.clsLeftMove;
		legalAux.clsPos[X] = refNode->getCoordinate(UPPER, X) - legalAux.clsLeftMove;
		legalAux.clsPos[Y] = refNode->getPosition(Y);
		legalAux.clsCost = displacementCost;
	} // end if-else 
	return true;
} // end method 

// -----------------------------------------------------------------------------

bool Segment::legalizeNode(LegalAux & legalAux) {

	Node * node = legalAux.clsNode;
	Node * reference = legalAux.clsReferenceNode;
	Node * left = reference;
	Node * right = reference->getRightNode();

	bool enclosed = isEnclosedNodeInWhitespace(legalAux);
	clsRow->moveNode(legalAux.clsNode, legalAux.clsOriginSegment, legalAux.clsLegalSegment, legalAux.clsPos);

	if (enclosed) {
		if (reference->getWidth() > node->getWidth()) {
			devideWhitespace(node, reference);
			DBU posX = legalAux.clsPos[X];
			if (reference->getPosition(X) > posX) {
				left = reference->getLeftNode();
				right = reference;
			} else {
				left = reference;
				right = reference->getRightNode();
			} // end if-else 
		} else {
			removeNode(reference);
			left = reference->getLeftNode();
			right = reference->getRightNode();
			deleteWhitespaceNode(reference);

		} // end if-else 
		connectNodes(left, node, right);
	} else {
		connectNodes(left, node, right);

		if (legalAux.clsLeftMove > 0) {
			moveLegalizedNodes(left, legalAux.clsLeftMove, true);
		} // end if 
		if (legalAux.clsRightMove > 0) {
			moveLegalizedNodes(right, legalAux.clsRightMove, false);
		} // end if 
	}
	updateLeftAndRightSegmentNodes(node->getLeftNode(), node, node->getRightNode());
	node->setLegalized(true);
	return false;
} // end method 

// -----------------------------------------------------------------------------

bool Segment::isEnclosedNodeInWhitespace(LegalAux & legalAux) {
	Node * nodeRef = legalAux.clsReferenceNode;
	if (!nodeRef->isWhitespace())
		return false;
	Node * node = legalAux.clsNode;
	const DBU width = node->getWidth();
	const DBUxy & pos = legalAux.clsPos;
	const Bounds &refBds = nodeRef->getBounds();
	if (refBds[LOWER][X] <= pos[X] && refBds[UPPER][X] >= pos[X] + width) {
		return true;
	} // end if 
	return false;
} // end method 

// -----------------------------------------------------------------------------

void Segment::getOverlapNode(LegalAux & legalAux) {
	DBUxy pos = legalAux.clsPos;
	Bin * bin = getBinByPosition(pos[X]);
	Node * binInst = bin->getOverlapNode(pos[X]);
	legalAux.clsReferenceNode = binInst;
} // end method 

// -----------------------------------------------------------------------------

void Segment::computeLeftShift(LegalAux & legalAux, std::vector<DBU> & cost) {
	Node * left = legalAux.clsReferenceNode; // the overlap node is the left one.

	const DBU siteWidth = getSiteWidth();

	DBU requiredLeftMove = legalAux.clsRequiredMove;
	if (left->isWhitespace()) {
		requiredLeftMove = std::max((DBU) 0, requiredLeftMove - left->getWidth());
		left = left->getLeftNode();
	} // end if 

	DBU leftMove = requiredLeftMove;

	while (left && leftMove > 0) {
		if (left->isWhitespace()) {
			leftMove -= left->getWidth();
			left = left->getLeftNode();
			continue;
		} // end if 

		DBUxy pos = left->getPosition();
		const DBU diffCost = computeDifferencePositionCost(pos[X], pos[X] - siteWidth, left->getInitialPosition(X));
		int numSites = getNumSites(leftMove) + 1;
		for (int i = 1; i < numSites; ++i) {
			cost[i] += i * diffCost;
		} // end for 
		left = left->getLeftNode();
	} // end while 
	//	} // end if-else 
	leftMove = std::max((DBU) 0, leftMove);
	legalAux.clsLeftMove = requiredLeftMove - leftMove;

} // end method 

// -----------------------------------------------------------------------------

void Segment::computeRightShift(LegalAux & legalAux, std::vector<DBU> & cost) {
	Node * right = legalAux.clsReferenceNode; // the overlap node is the left one.
	const DBU siteWidth = getSiteWidth();

	DBU requiredRightMove = legalAux.clsRequiredMove;
	if (right->isWhitespace()) {
		Node * node = legalAux.clsNode;
		const Bounds & nodeBds = node->getBounds();
		const Bounds & rightBds = right->getBounds();
		Bounds overlap = nodeBds.overlapRectangle(rightBds);
		const DBU length = overlap.computeLength(X);
		requiredRightMove = std::max((DBU) 0, requiredRightMove - length);
	} // end if 
	DBU rightMove = requiredRightMove;

	right = right->getRightNode();

	while (right && rightMove > 0) {
		if (right->isWhitespace()) {
			rightMove -= right->getWidth();
			right = right->getRightNode();
			continue;
		} // end if 

		DBUxy pos = right->getPosition();
		const DBU diffCost = computeDifferencePositionCost(pos[X], pos[X] + siteWidth, right->getInitialPosition(X));
		int numSites = getNumSites(rightMove) + 1;
		for (int i = 1; i < numSites; ++i) {
			cost[i] += i * diffCost;
		} // end for 
		right = right->getRightNode();
	} // end while 
	rightMove = std::max((DBU) 0, rightMove);
	legalAux.clsRightMove = requiredRightMove - rightMove;

} // end method 

// -----------------------------------------------------------------------------

void Segment::computeShiftToSide(Node * refNode, const DBU requiredMove,
	DBU & overflowMove, std::vector<DBU> &cost, const bool isToLeft) {

	overflowMove = requiredMove;
	int numRequiredSites = getNumSites(overflowMove);
	cost.resize(numRequiredSites + 1, 0);
	int sites = 0;

	Node * ref = refNode;

	while (ref && overflowMove > 0) {
		if (ref->isWhitespace()) {
			const DBU maxOverflowAllowed = ref->getWidth() > overflowMove ?
				overflowMove : ref->getWidth();
			sites += getNumSites(maxOverflowAllowed);
			overflowMove -= maxOverflowAllowed;
		} else {
			const DBU siteWidth = isToLeft ? -getSiteWidth() : +getSiteWidth();
			const DBU initPosX = ref->getInitialPosition(X);
			const DBU legalPosX = ref->getPosition(X);
			for (int i = sites + 1; i <= numRequiredSites; ++i) {
				DBU targetPosX = legalPosX + (i * siteWidth);
				const DBU costX = computePositionCost(initPosX, legalPosX, targetPosX);
				cost[i] += costX;
			} // end for 
		} // end if-else
		ref = isToLeft ? ref->getLeftNode() : ref->getRightNode();
	} // end while 
} // end method 

// -----------------------------------------------------------------------------

void Segment::computeOptimizedPosition(LegalAux & legalAux, std::vector<DBU> & leftCost,
	std::vector<DBU> &rightCost) {

	Node * node = legalAux.clsNode;
	DBUxy pos = legalAux.clsPos;
	DBU initPos = node->getInitialPosition(X);
	DBU cost = std::numeric_limits<DBU>::max();
	int numRequiredSites = getNumSites(legalAux.clsRequiredMove);
	int numAvailableLeftSites = getNumSites(legalAux.clsLeftMove);
	int numAvailableRightSites = getNumSites(legalAux.clsRightMove);
	int lowerLimit = numRequiredSites - numAvailableRightSites;
	int leftSiteMove = 0;
	int rightSiteMove = 0;

	for (int leftId = numAvailableLeftSites; leftId >= lowerLimit; --leftId) {
		int rightId = numRequiredSites - leftId;
		DBU moveCost = leftCost[leftId] + rightCost[rightId];
		DBU leftPos = pos[X] - (leftId * getSiteWidth());
		DBU cellCost = computeDifferencePositionCost(pos[X], leftPos, initPos);
		DBU totalCost = moveCost + cellCost;
		if (totalCost <= cost) {
			leftSiteMove = leftId;
			rightSiteMove = rightId;
			cost = totalCost;
		} // end if 
	} // end for 

	DBU leftMove = leftSiteMove * getSiteWidth();
	DBU rightMove = rightSiteMove * getSiteWidth();
	legalAux.clsLeftMove = leftMove;
	legalAux.clsRightMove = rightMove;

	Node * left = legalAux.clsReferenceNode;
	if (left->isWhitespace()) {
		Bounds nodeBds = node->getBounds();
		nodeBds.moveTo(pos[X], X);
		const Bounds & rightBds = left->getBounds();
		Bounds overlap = nodeBds.overlapRectangle(rightBds);
		const DBU length = overlap.computeLength(X);
		const DBU nodeWidth = node->getWidth();
		if (nodeWidth > length) {
			DBU diff = nodeWidth - length;
			diff -= rightMove;
			if (diff < 0) {
				std::cout << "DEBUG REQUIRED. DIFF < 0. diff: " << diff << " node: " << node->getName() << "\n";
			}
			pos[X] -= diff;
		} //  end if 
	} else {
		pos[X] = left->getCoordinate(UPPER, X) - leftMove;
	} // end if-else 


	legalAux.clsPos[X] = pos[X];
} // end method 

// -----------------------------------------------------------------------------

DBU Segment::computeDifferencePositionCost(const DBU fromPosX, const DBU toPosX, const DBU initPosX) {
	const DBU toCost = std::abs(toPosX - initPosX);
	const DBU fromCost = std::abs(fromPosX - initPosX);
	return toCost - fromCost;
} // end method 

// -----------------------------------------------------------------------------

void Segment::computeLegalizationCost(LegalAux & legalAux) {
	Node * node = legalAux.clsNode;
	Node * left = legalAux.clsReferenceNode;
	Node * right = left->getRightNode();
	legalAux.clsCost = computeDisplacementCost(node, legalAux.clsPos);
	DBU leftMove = legalAux.clsLeftMove;
	DBU rightMove = legalAux.clsRightMove;
	while (left && leftMove >= 0) {
		if (left->isWhitespace()) {
			leftMove -= left->getWidth();
		} else {
			DBUxy pos = left->getPosition();
			pos[X] -= leftMove;
			legalAux.clsCost += computeDisplacementCost(left, pos);
		} // end if-else 
		left = left->getLeftNode();
	} // end while
	while (right && rightMove >= 0) {
		if (right->isWhitespace()) {
			rightMove -= right->getWidth();
		} else {
			DBUxy pos = right->getPosition();
			pos[X] += rightMove;
			legalAux.clsCost += computeDisplacementCost(right, pos);
		} // end if-else 
		right = right->getRightNode();
	} // end while
} // end method 

// -----------------------------------------------------------------------------

DBU Segment::computePositionCost(const DBU initPos, const DBU currentPos, const DBU targetPos) {
	return std::abs(targetPos - initPos) - std::abs(currentPos - initPos);
} // end method 

// -----------------------------------------------------------------------------

void Segment::openRequiredSpace(LegalAux & legalAux) {
	if (legalAux.clsLeftMove > 0)
		moveLeftCells(legalAux);
	if (legalAux.clsRightMove > 0)
		moveRightCells(legalAux);
} // end method 

// -----------------------------------------------------------------------------

void Segment::moveLeftCells(LegalAux & legalAux) {
	DBU reqMove = legalAux.clsLeftMove;
	Node * left = legalAux.clsReferenceNode;
	std::deque<Node *> deleteInsts;
	if (left->isWhitespace())
		left = left->getLeftNode();

	while (left && reqMove > 0) {
		if (left->isWhitespace()) {
			DBU width = left->getWidth();
			if (reqMove >= width) {
				// reset whitespace width 
				removeNode(left);
				left->setWhitespaceBounds(UPPER, X, left->getPosition(X));
				reqMove -= width;
				deleteInsts.push_back(left);
			} else {
				removeNode(left);
				DBU upperX = left->getCoordinate(UPPER, X) - reqMove;
				left->setWhitespaceBounds(UPPER, X, upperX);
				reqMove = 0;
				insertNode(left);
			} // end if-else 
		} else {
			DBUxy pos = left->getPosition();
			pos[X] -= reqMove;
			clsRow->moveNode(left, this, this, pos);
		} // end if-else 
		left = left->getLeftNode();
	} // end while 
	for (Node * node : deleteInsts) {
		deleteWhitespaceNode(node);
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void Segment::moveRightCells(LegalAux & legalAux) {
	DBU reqMove = legalAux.clsRightMove;
	Node * right = legalAux.clsReferenceNode;
	right = right->getRightNode();
	std::deque<Node *> deleteInsts;

	while (right && reqMove > 0) {
		if (right->isWhitespace()) {
			DBU width = right->getWidth();
			if (reqMove >= width) {
				removeNode(right);
				reqMove -= width;
				right->setWhitespaceBounds(LOWER, X, right->getCoordinate(UPPER, X));
				deleteInsts.push_back(right);
			} else {
				removeNode(right);
				DBU lowerX = right->getPosition(X) + reqMove;
				right->setWhitespaceBounds(LOWER, X, lowerX);
				reqMove = 0;
				insertNode(right);
			} // end if-else 
		} else {
			DBUxy pos = right->getPosition();
			pos[X] += reqMove;
			clsRow->moveNode(right, this, this, pos);
		} // end if-else 
		right = right->getRightNode();
	} // end while 
	for (Node * node : deleteInsts) {
		deleteWhitespaceNode(node);
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void Segment::moveLegalizedNodes(Node * refNode, const DBU requiredMove, const bool isToLeft) {
	DBU overflow = requiredMove;
	Node * node = refNode;
	std::deque<Node *> deleteInsts;
	while (overflow > 0) {
		if (node->isWhitespace()) {
			DBU width = node->getWidth();
			if (width <= overflow) {
				removeNode(node);
				overflow -= width;
				deleteInsts.push_back(node);
			} else {
				removeNode(node);
				if (isToLeft) {
					node->setDisplacementWhitespaceBounds(UPPER, X, -overflow);
				} else {
					node->setDisplacementWhitespaceBounds(LOWER, X, overflow);
				} // end if-else 
				overflow = 0;
				insertNode(node);
			} // end if-else 
		} else {
			DBUxy pos = node->getPosition();
			pos[X] = isToLeft ? pos[X] - overflow : pos[X] + overflow;
			clsRow->moveNode(node, this, this, pos);
		} // end if-else 

		//BEGIN DEBUG
		bool pt = node->getName().compare("g214985_u0") == 0;
		if (pt) {
			std::cout << "refNode: " << refNode->getName()
				<< " maxDispNd: " << node->getName()
				<< " disp: " << node->getDisplacement()
				<< " pos: " << node->getPosition()
				<< "\n";
		}
		//END DEBUG

		node = isToLeft ? node->getLeftNode() : node->getRightNode();
	} // end while

	for (Node * node : deleteInsts) {
		deleteWhitespaceNode(node);
	} // end for 

} // end method 

// -----------------------------------------------------------------------------

void Segment::devideWhitespace(Node * node, Node * reference) {
	const Bounds & instBds = node->getBounds();
	const Bounds & refBds = reference->getBounds();
	removeNode(reference);
	if (instBds[LOWER][X] == refBds[LOWER][X]) {
		reference->setWhitespaceBounds(LOWER, X, instBds[UPPER][X]);
	} else if (instBds[UPPER][X] == refBds[UPPER][X]) {
		reference->setWhitespaceBounds(UPPER, X, instBds[LOWER][X]);
	} else {
		Bounds space = refBds;
		space[LOWER][X] = instBds[UPPER][X];
		reference->setWhitespaceBounds(UPPER, X, instBds[LOWER][X]);
		Node * right = createWhitespaceNode(space);
		connectNodes(reference, right, reference->getRightNode());
		insertNode(right);
	} // end if-else 
	insertNode(reference);
} // end method 

// -----------------------------------------------------------------------------

void Segment::connectNodes(Node * left, Node * node, Node * right) {
	connectNodes(left, node);
	connectNodes(node, right);
} // end method 

// -----------------------------------------------------------------------------

void Segment::connectNodes(Node * left, Node *right) {
	if (left)
		left->setRightNode(right);
	if (right)
		right->setLeftNode(left);
} // end method 

// -----------------------------------------------------------------------------

DBU Segment::computeDisplacementCost(Node * inst, DBUxy targetPos) {
	const DBUxy pos = inst->getPosition();
	const DBUxy initPos = inst->getInitialPosition();
	DBUxy currentDisp = pos - initPos;
	DBUxy targetDisp = targetPos - initPos;
	currentDisp.abs();
	targetDisp.abs();
	DBUxy costDisp = targetDisp - currentDisp;
	return costDisp.aggregated();
} // end method 

// -----------------------------------------------------------------------------

void Segment::optimizeCellDisplacement(Node * node) {
	Node * left = node->getLeftNode();
	Node * right = node->getRightNode();
	bool swapLeft = false;
	bool swapRight = false;
	DBU righCost = std::numeric_limits<DBU>::max();
	DBU leftCost = std::numeric_limits<DBU>::max();
	DBU nodeSwapRightCost = std::numeric_limits<DBU>::max();
	DBU nodeSwapLeftCost = std::numeric_limits<DBU>::max();
	if (right && !right->isWhitespace()) {
		DBUxy pos = node->getPosition();
		righCost = computeDisplacementCost(right, pos);
		pos[X] += right->getWidth();
		nodeSwapRightCost = computeDisplacementCost(node, pos);
		if (righCost < 0 && nodeSwapRightCost < 0) {
			swapRight = true;
		} // end if 
	} // end if 
	if (left && !left->isWhitespace()) {
		DBUxy pos = node->getPosition();
		leftCost = computeDisplacementCost(left, pos);
		pos[X] += left->getWidth();
		nodeSwapLeftCost = computeDisplacementCost(node, pos);
		if (leftCost < 0 && nodeSwapLeftCost < 0) {
			swapLeft = true;
		} // end if 
	} // end if 

	if (swapLeft && swapRight) {
		DBU leftTotalCost = leftCost + nodeSwapLeftCost;
		DBU rightTotalCost = righCost + nodeSwapRightCost;
		if (leftTotalCost < rightTotalCost) {
			swapRight = false;
		} else {
			swapLeft = false;
		} // end if-else 
	} // end if

	if (swapLeft) {
		swapNodes(left, node);
	} // end if 
	if (swapRight) {
		swapNodes(node, right);
	} // end if 
} // end method 

// -----------------------------------------------------------------------------

void Segment::swapNodes(Node * node0, Node * node1) {
	if (!node0 || !node1)
		return;

	if (node0->getPosition(X) > node1->getPosition(X)) {
		Node * temp = node0;
		node0 = node1;
		node1 = temp;
	} // end if 
	if (node0->getCoordinate(UPPER, X) != node1->getPosition(X)) {
		return;
	} // end if 

	Node * node0Left = node0->getLeftNode();
	Node * node1Right = node1->getRightNode();
	DBUxy nodePos = node0->getPosition();
	clsRow->moveNode(node1, this, this, nodePos);
	nodePos[X] += node1->getWidth();
	clsRow->moveNode(node0, this, this, nodePos);
	node0->disconectNode();
	node1->disconectNode();
	if (node0Left) {
		node0Left->setRightNode(node1);
	} // end if
	node1->setLeftNode(node0Left);
	node1->setRightNode(node0);
	node0->setLeftNode(node1);
	node0->setRightNode(node1Right);
	if (node1Right) {
		node1Right->setLeftNode(node0);
	} // end if 
	updateLeftSegmentNode(node1);
	updateRightSegmentNode(node0);
} // end method 

// -----------------------------------------------------------------------------

void Segment::updateLeftAndRightSegmentNodes(Node * left, Node * node, Node * right) {
	updateLeftSegmentNode(node);
	updateRightSegmentNode(node);
	updateLeftSegmentNode(left);
	updateRightSegmentNode(right);
} // end method 

// -----------------------------------------------------------------------------

void Segment::updateLeftSegmentNode(Node * left) {
	if (left && left->getPosition(X) < clsLeftNode->getPosition(X)) {
		clsLeftNode = left;
	} // end if 
} // end method 

// -----------------------------------------------------------------------------

void Segment::updateRightSegmentNode(Node * right) {
	if (right && right->getPosition(X) > clsRighNode->getPosition(X)) {
		clsRighNode = right;
	} // end if 
} // end method 

// -----------------------------------------------------------------------------

void Segment::setLeftSegmentNode(Node * left) {
	if (left) {
		clsLeftNode = left;
	} // end if 
} // end method 

// -----------------------------------------------------------------------------

void Segment::setRightSegmentNode(Node * right) {
	if (right) {
		clsRighNode = right;
	} // end if 
} // end method 

// -----------------------------------------------------------------------------

Node * Segment::createWhitespaceNode(const Bounds & bounds) {
	Node * inst = nullptr;
	if (clsDeletedWhitespaceNodes.empty()) {
		clsWhitespaceNodes.push_back(Node());
		inst = &clsWhitespaceNodes.back();
		const int id = getNextWhitespaceId();
		inst->setId(id);
	} else {
		std::set<int>::iterator it = clsDeletedWhitespaceNodes.begin();
		const int id = *it - clsWhitespaceThreshold;
		clsDeletedWhitespaceNodes.erase(it);
		inst = &clsWhitespaceNodes[id];
	} // end if-else
	inst->setWhitespaceBounds(bounds);
	inst->setWhitespace(true);
	inst->setDeletedWhitespace(false);
	return inst;
} // end method 

// -----------------------------------------------------------------------------

void Segment::deleteWhitespaceNode(Node * inst) {
	disconectNode(inst);
	clsDeletedWhitespaceNodes.insert(inst->getId());
	inst->setDeletedWhitespace(true);
} // end method 

// -----------------------------------------------------------------------------

void Segment::disconectNode(Node * inst) {
	if (!inst) {
		return;
	} // end if 
	Node * left = inst->getLeftNode();
	Node * right = inst->getRightNode();
	if (left) {
		left->setRightNode(right);
	} // end if 
	if (right) {
		right->setLeftNode(left);
	} // end if 
	Node * segmentLeft = getLeftNode();
	Node * segmentRight = getRightNode();
	if (inst->getId() == segmentLeft->getId()) {
		setLeftSegmentNode(right);
	} // end if 
	if (inst->getId() == segmentRight->getId()) {
		setRightSegmentNode(left);
	} // end if 
	inst->disconectNode();
} // end method 

// -----------------------------------------------------------------------------

int Segment::getNextWhitespaceId() {
	clsWhitespaceLastId++;
	return clsWhitespaceLastId - 1;
} // end method 

// -----------------------------------------------------------------------------

DBU Segment::getSiteWidth() {
	return clsRow->getSiteWidth();
} // end method 

// -----------------------------------------------------------------------------

int Segment::getNumSites(const DBU length) {
	return length / clsRow->getSiteWidth();
} // end method 

// -----------------------------------------------------------------------------

bool Segment::isSiteAligned(Node * node) {
	DBUxy pos = node->getPosition();
	return !(pos[X] % getSiteWidth());
} // end method 

// -----------------------------------------------------------------------------

bool Segment::isInsideSegment(Node * node) {
	const Bounds & nodeBds = node->getBounds();
	const Bounds & segmentBds = getBounds();
	if (nodeBds[LOWER][X] < segmentBds[LOWER][X]) {
		return true;
	} // end if 
	if (nodeBds[UPPER][X] > segmentBds[UPPER][X]) {
		return true;
	} // end if 
	if (nodeBds[LOWER][Y] < segmentBds[LOWER][Y]) {
		return true;
	} // end if 
	if (nodeBds[UPPER][Y] > segmentBds[UPPER][Y]) {
		return true;
	} // end if 
	return false;
} // end method 

// -----------------------------------------------------------------------------

// negative width is also considered zero width

bool Segment::hasZeroWidth(Node * node) {
	if (node->isWhitespace()) {
		return node->getWidth() <= 0;
	} // end if 
	return false;
} // end method 

// -----------------------------------------------------------------------------

const void Segment::checkSanityBinsConnected() const {
	int i = 1;
	const Bin * currentBin = &clsBins.front();
	while (currentBin->getRight() != nullptr) {
		if (currentBin->getRight()->getSegment() == currentBin->getSegment()) {
			currentBin = currentBin->getRight();
			i++;
		} else
			break;
	} // end while
	if (i != clsBins.size())
		std::cout << "A bin in segment " << getFullId() << " is not connected\n";

} // end method

// -----------------------------------------------------------------------------

const void Segment::checkSanityBinsInside() const {
	for (const Bin & bin : clsBins) {
		if (!clsBounds.inside(bin.getBounds())) {
			std::cout << "Bin " << bin.getFullId() << " is outside its parent\n";
		}
	} // end for
} // end method

// -----------------------------------------------------------------------------

const void Segment::checkSanityBinsOverlap() const {
	const Bin * currentBin = &clsBins.front();
	const Bin * leftBin;
	while (currentBin->getRight() != nullptr) {
		if (currentBin->getRight()->getSegment() == currentBin->getSegment()) {
			leftBin = currentBin;
			currentBin = currentBin->getRight();
			if (leftBin->getBounds().overlapArea(currentBin->getBounds()) > 1)
				std::cout << "Bins " << currentBin->getFullId() << " and " << leftBin->getFullId() << " overlap\n";
			if (currentBin == leftBin)
				std::cout << "Bin " << currentBin->getFullId() << " is repeated\n";
		} else
			break;

	} // end while
} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanitySegmentList() {
	int numInstBins = 0;
	for (Bin & bin : clsBins)
		numInstBins += bin.getNumNodes();

	bool hasZeroWidthWhitespace = false;
	int numInstSegmentFront = 0;
	Node * front = getLeftNode();
	while (front) {
		if (!front->isWhitespace())
			numInstSegmentFront++;
		if (front->isWhitespace() && front->getWidth() <= 0) {
			std::cout << "Node " << front->getName() << " has zero width in segment " << getFullId()
				<< " NodeBds: " << front->getBounds()
				<< "\n";
			hasZeroWidthWhitespace = true;

		} // end if 

		front = front->getRightNode();
	} // end while 

	int numInstSegmentBack = 0;
	Node * back = getRightNode();
	while (back) {
		if (!back->isWhitespace())
			numInstSegmentBack++;
		back = back->getLeftNode();
	}

	//	std::cout << ">>>>>>>>>>>>>>> checkSanity: " << getFullId();
	//	std::cout << "\t#binInsts: " << numInstBins << " #instFront: " << numInstSegmentFront
	//		<< " #instBack: " << numInstSegmentBack << " has0Width: " << hasZeroWidthWhitespace
	//		<< "\n";
	if (hasZeroWidthWhitespace) {
		std::cout << "Debug Required. Segment " << getFullId() << " has whitespace with zero width\n";
	}

	return hasZeroWidthWhitespace;

} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanitySegmentInstances() {
	DBU segmentWidth = clsBounds.computeLength(X);

	DBU leftWidthInstaces = 0;
	DBU rightWidthInstances = 0;

	Node * left = getLeftNode();
	Node * right = getRightNode();
	while (left) {
		leftWidthInstaces += left->getWidth();
		left = left->getRightNode();
	}

	while (right) {
		rightWidthInstances += right->getWidth();
		right = right->getLeftNode();
	}


	if (leftWidthInstaces != segmentWidth) {
		std::cout << "Segment bounds: " << getBounds() << "\n";
		left = getLeftNode();
		int count = 0;
		while (left) {
			std::cout << "bds: " << left->getBounds() << " name: " << left->getName() << " WS: " << left->isWhitespace() << "\n";
			left = left->getRightNode();
			count++;
			if (count > 15)
				break;
		}
		std::cout << "Width left to right in segment " << getFullId() << " is not equal to total cell width. SegmentW: "
			<< segmentWidth << " nodeW: " << leftWidthInstaces
			<< "\n";
		std::cout << "Exiting....\n";
		//exit(0);
		return true;
	}

	if (rightWidthInstances != segmentWidth) {
		std::cout << "Segment bounds: " << getBounds() << "\n";
		right = getRightNode();
		int count = 0;
		while (right) {
			std::cout << "bds: " << right->getBounds() << " name: " << right->getName() << " WS: " << right->isWhitespace() << "\n";
			right = right->getLeftNode();
			count++;
			if (count > 10)
				break;
		}
		std::cout << "Width right to left  in segment " << getFullId() << " is not equal to total cell width. SegmentW: "
			<< segmentWidth << " nodeW: " << rightWidthInstances
			<< "\n";
		std::cout << "Exiting....\n";
		//exit(0);
		return true;
	}
	return false;

} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanitySiteAligment() {
	Node * left = getLeftNode();
	while (left) {
		bool siteAligned = isSiteAligned(left);
		if (!siteAligned) {
			std::cout << "Instance is not aligned to site. Inst: " << left->getName() << " pos: " << left->getPosition()
				<< " segment: " << getFullId()
				<< "\n";
			std::cout << "Exiting.....\n";
			//exit(0);
			return true;
		}
		left = left->getRightNode();
	}
	return false;
} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanityZeroWidth() {
	Node * left = getLeftNode();
	bool zeroWidth = false;
	while (left) {
		zeroWidth = hasZeroWidth(left);
		if (zeroWidth)
			break;

		left = left->getRightNode();
	}
	if (zeroWidth) {
		std::cout << "Zero Width node detected\n";
		Node * left = getLeftNode();
		while (left) {
			std::cout << "bds: " << left->getBounds() << " name: " << left->getName() << " WS: " << left->isWhitespace() << "\n";
			left = left->getRightNode();
		}
		std::cout << "Instance has zero width. Inst: " << left->getName()
			<< " bounds: " << left->getBounds()
			<< " pos: " << left->getPosition()
			<< " segment: " << getFullId()
			<< "\n";
		std::cout << "Exiting.....\n";
		return true;
		//exit(0);
	}

	return false;
} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanityNodeBounds() {
	Node * left = getLeftNode();
	Node * right = left->getRightNode();
	while (right) {
		const Bounds & leftBds = left->getBounds();
		const Bounds & rightBds = right->getBounds();
		if (leftBds[UPPER][X] != rightBds[LOWER][X]) {
			std::cout << "Instances do not share bounds. left: " << left->getName()
				<< " right: " << right->getName() << " leftBds: " << leftBds
				<< " rightBds: " << rightBds
				<< "\n";
			std::cout << "Exiting.....\n";
			//exit(0);
			return true;
		} // end if 
		left = right;
		right = right->getRightNode();
	} // end while 
	return false;
} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanityNodesInsideSegment() {
	Node * left = getLeftNode();
	const Bounds & segmentBds = getBounds();
	while (left) {
		const Bounds & bds = left->getBounds();
		if (bds[LOWER][X] < segmentBds[LOWER][X]
			|| bds[UPPER][X] > segmentBds[UPPER][X]
			|| bds[LOWER][Y] < segmentBds[LOWER][Y]
			|| bds[UPPER][Y] > segmentBds[UPPER][Y]) {
			std::cout << "Instance is outside of segment bounds. inst: " << left->getName()
				<< " instBds: " << bds << " segBds: " << segmentBds
				<< "\n";
			std::cout << "Exiting.....\n";
			return true;
			//exit(0);
		} // end if 
		left = left->getRightNode();
	} // end while 
	return false;
} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanityCache() {

	return false;
} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanityBins() {

	for (Bin & bin : clsBins) {
		if (bin.checkSanityBin())
			return true;
	} // end for 	
	return false;
} // end method

// -----------------------------------------------------------------------------

bool Segment::checkSanityNodes() {
	if (checkSanitySegmentList()) {
		std::cout << "Debug required on checkSanitySegmentList\n";
		return true;
	} // end if 
	if (checkSanitySegmentInstances()) {
		std::cout << "Debug required on checkSanitySegmentInstances\n";
		return true;
	} // end if 
	if (checkSanitySiteAligment()) {
		std::cout << "Debug required on checkSanitySiteAligment\n";
		return true;
	} // end if 
	if (checkSanityZeroWidth()) {
		std::cout << "Debug required on checkSanityZeroWidth\n";
		return true;
	} // end if 
	if (checkSanityNodeBounds()) {
		std::cout << "Debug required on checkSanityNodeBounds\n";
		return true;
	} // end if 
	if (checkSanityNodesInsideSegment()) {
		std::cout << "Debug required on checkSanityNodesInsideSegment\n";
		return true;
	} // end if 
	if (checkSanityCache()) {
		std::cout << "Debug required on checkSanityCache\n";
		return true;
	} // end if 
	if (checkSanityBins()) {
		std::cout << "Debug required on checkSanityBins\n";
		return true;
	} // end if 
	return false;
} // end method

// -----------------------------------------------------------------------------

} // end namespace 
