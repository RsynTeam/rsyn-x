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
 * File:   Row.cpp
 * Author: isoliveira
 * 
 * Created on 12 de Março de 2018, 17:32
 */
#include <algorithm>

#include "Row.h"
#include "Blockage.h"
#include "Segment.h"
#include "Bin.h"
#include "NFLegal.h"

namespace NFL {

void Row::initLegalize() {
	for (Segment & segment : clsSegments) {
		segment.initLegalize();
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void Row::addBlockage(const Bounds & block) {
	clsBlockages.push_back(block);
} // end method 

// -----------------------------------------------------------------------------

void Row::initBlockages() {
	std::sort(clsBlockages.begin(), clsBlockages.end(),
		[](const Bounds & block0, const Bounds & block1) {
			return block0[LOWER][X] < block1[LOWER][X];
		}); // end sort 
	mergeNeighborBlockages();
} // end method 

// -----------------------------------------------------------------------------

void Row::initSegments() {
	if (clsBlockages.empty()) {
		const int id = clsSegments.size();
		clsSegments.push_back(Segment());
		Segment & segment = clsSegments.back();
		segment.setBounds(getBounds());
		segment.setId(id);
		segment.setBinLength(clsBinLength);
		segment.setRow(this);
	} else {
		//const DBU length = 0.2 * clsNFLegal->getDefaultBinLength(X);

		DBU left = clsBounds.getCoordinate(LOWER, X);
		DBU right;
		clsSegments.reserve(clsBlockages.size());
		for (Bounds & block : clsBlockages) {
			right = block[LOWER][X];
			Bounds segmentBounds = getBounds();
			segmentBounds[LOWER][X] = left;
			segmentBounds[UPPER][X] = right;
			if (right <= left) {
				left = block[UPPER][X];
				continue;
			} // end if 
		//	if (segmentBounds.computeLength(X) > length) {
				const int id = clsSegments.size();
				clsSegments.push_back(Segment());
				Segment & segment = clsSegments.back();
				segment.setBounds(segmentBounds);
				segment.setId(id);
				segment.setBinLength(clsBinLength);
				segment.setRow(this);
			//} // end if 
			left = block[UPPER][X];
		} // end for 
		right = clsBounds.getCoordinate(UPPER, X);
		if (left < right) {
			Bounds segmentBounds = getBounds();
			segmentBounds[LOWER][X] = left;
			segmentBounds[UPPER][X] = right;
			//if (segmentBounds.computeLength(X) > length) {
				const int id = clsSegments.size();
				clsSegments.push_back(Segment());
				Segment & segment = clsSegments.back();
				segment.setBounds(segmentBounds);
				segment.setId(id);
				segment.setBinLength(clsBinLength);
				segment.setRow(this);
			//} // end if 
		} // end if 
	} // end if-else 

	// initialize bins and connect horizontal bin through blockages
	Bin * leftBin = nullptr;
	Segment * leftSegment = nullptr;
	for (Segment & segment : clsSegments) {
		// initializing bins
		segment.initBins();
		clsNumBins += segment.getNumBins();

		// connecting horizontal bins through blockages
		Bin * rightBin = segment.getFrontBin();
		if (leftBin) {
			rightBin->setLeft(leftBin);
			leftBin->setRight(rightBin);
		} // end if 
		leftBin = segment.getBackBin();

		// initializing RTee structure
		const int index = segment.getId();
		const Bounds & bounds = segment.getBounds();
		PhysicalPolygonPoint lowerPoint(bounds[LOWER][X], bounds[LOWER][Y]);
		PhysicalPolygonPoint upperPoint(bounds[UPPER][X], bounds[UPPER][Y]);
		Box box(lowerPoint, upperPoint);
		clsRTreeSegments.insert(std::make_pair(box, index));

		// Connecting horizontal segments
		if (leftSegment) {
			leftSegment->setRight(&segment);
			segment.setLeft(leftSegment);
		} // end if 
		leftSegment = &segment;
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

bool Row::insertNode(Node * inst) {
	DBUxy pos = inst->getPosition();
	PhysicalPolygonPoint point(pos[X], pos[Y]);
	std::vector<BoxPair> neighbors;
	clsRTreeSegments.query(bgi::nearest(point, 1), std::back_inserter(neighbors));
	std::size_t segmentIndex = neighbors.front().second;
	Segment & segment = clsSegments[segmentIndex];
	if (segment.getWidth() < inst->getWidth()) {
		return false;
	} // end if 
	if (pos[X] <= segment.getCoordinate(UPPER, X) &&
		inst->getCoordinate(UPPER, X) >= segment.getCoordinate(LOWER, X)) {
		if (inst->getCoordinate(UPPER, X) > segment.getCoordinate(UPPER, X)) {
			DBUxy pos = inst->getPosition();
			pos[X] = segment.getCoordinate(UPPER, X) - inst->getWidth();
			placeCell(inst->getPhysicalCell(), pos);
		} // end if 
		// instance has lower X partial overlap with segment
		if (inst->getCoordinate(LOWER, X) < segment.getCoordinate(LOWER, X)) {
			DBUxy pos = inst->getPosition();
			pos[X] = segment.getCoordinate(LOWER, X);
			placeCell(inst->getPhysicalCell(), pos);
		} // end if
		segment.insertNode(inst);
		return true;
	} // end if 
	return false;
} // end method 

// -----------------------------------------------------------------------------

Segment * Row::getNearestSegment(const DBUxy pos, const DBU width) {
	PhysicalPolygonPoint point(pos[X], pos[Y]);
	std::vector<BoxPair> neighbors;
	clsRTreeSegments.query(bgi::nearest(point, 1), std::back_inserter(neighbors));
	std::size_t segmentIndex = neighbors.front().second;
	Segment * segment = &clsSegments[segmentIndex];

	if (segment->getWidth() >= width) {
		return segment;
	} // end if 

	// if the segment can not accept the cell, i.e., width(segment) < width(cell)
	DBU disp = std::numeric_limits<DBU>::max();
	Segment * best = nullptr;
	Segment * left = segment->getLeftSegment();
	Segment * right = segment->getRightSegment();
	while (left || right) {
		if (left) {
			DBU segDisp = left->getDisplacement(pos);
			if (left->getWidth() >= width && segDisp < disp) {
				best = left;
				disp = segDisp;
				left = nullptr;
			} else {
				left = left->getLeftSegment();
			} // end if-else 
		} // end if 
		if (right) {
			DBU segDisp = right->getDisplacement(pos);
			if (right->getWidth() >= width && segDisp < disp) {
				best = right;
				disp = segDisp;
				right = nullptr;
			} else {
				right = right->getRightSegment();
			} // end if-else 
		} // end if 
	} // end while 
	return best;
} // end method 

// -----------------------------------------------------------------------------

Segment * Row::getSegment(const DBUxy pos) {
	PhysicalPolygonPoint point(pos[X], pos[Y]);
	std::vector<BoxPair> neighbors;
	clsRTreeSegments.query(bgi::nearest(point, 1), std::back_inserter(neighbors));
	std::size_t segmentIndex = neighbors.front().second;
	return &clsSegments[segmentIndex];
} // end method 

// -----------------------------------------------------------------------------

DBU Row::getDisplacement(const DBUxy pos) {
	const DBU dispY = std::abs(pos[Y] - clsBounds[LOWER][Y]);
	if (pos[X] >= clsBounds[LOWER][X] && pos[X] <= clsBounds[UPPER][X]) {
		return dispY;
	} // end if 
	const DBU dispX = std::min(std::abs(clsBounds[LOWER][X] - pos[X]), std::abs(clsBounds[UPPER][X] - pos[X]));
	return dispY + dispX;
} // end method 

// -----------------------------------------------------------------------------

DBU Row::getDisplacement(const DBU pos, const Dimension dim) {
	return std::abs(clsBounds[LOWER][dim] - pos);
} // end method 

// -----------------------------------------------------------------------------

DBU Row::getSiteAdjustedPosition(const DBU posX, const bool roundup) {
	const DBU siteWidth = getSiteWidth();
	DBU left = getPosition(X);
	int numSites = static_cast<int> ((posX - left) / siteWidth);
	DBU adjustedPosX = numSites * siteWidth;
	if (roundup && adjustedPosX != posX) {
		adjustedPosX += siteWidth;
	} // end if 

	return left + adjustedPosX;
} // end method 

// -----------------------------------------------------------------------------

void Row::moveNode(Node * node, Segment * src, Segment *sink, const DBUxy targetPos) {
	clsNFLegal->moveNode(node, src, sink, targetPos);
} // end method 

// -----------------------------------------------------------------------------

void Row::placeCell(Rsyn::PhysicalCell phCell, DBUxy pos) {
	clsNFLegal->placeCell(phCell, pos);
} // end method 

// -----------------------------------------------------------------------------

void Row::mergeNeighborBlockages() {
	if (clsBlockages.size() < 2) {
		return;
	} // end if 
	std::deque<Bounds> merged;
	int numBlocks = getNumBlockages();
	Bounds previousBds = clsBlockages[0];
	for (int index = 1; index < numBlocks; ++index) {
		Bounds blockBds = clsBlockages[index];
		if (previousBds[UPPER][X] >= blockBds[LOWER][X]) {
			previousBds[UPPER][X] = blockBds[UPPER][X];
		} else {
			Bounds mergedBlock(previousBds[LOWER], previousBds[UPPER]);
			merged.push_back(mergedBlock);
			previousBds = blockBds;
		} // end if-else 
	} // end for 
	// adding last blockage
	Bounds mergedBlock(previousBds[LOWER], previousBds[UPPER]);
	merged.push_back(mergedBlock);
	clsBlockages = merged;
} // end method 

// -----------------------------------------------------------------------------

void Row::checkSanitySegmentsConnected() {
	int i = 1;

	Segment * currentSeg = &clsSegments.front();

	while (currentSeg->getRightSegment() != nullptr) {
		currentSeg = currentSeg->getRightSegment();
		i++;
	} // end while

	if (i != clsSegments.size())
		std::cout << "A segment in row " << clsId << " is not connected\n";


	i = 1;
	currentSeg = &clsSegments.back();

	while (currentSeg->getLeftSegment() != nullptr) {
		currentSeg = currentSeg->getLeftSegment();
		i++;
	} // end while

	if (i != clsSegments.size())
		std::cout << "A segment in row " << clsId << " is not connected\n";

} // end method

// -----------------------------------------------------------------------------

void Row::checkSanitySegmentsInside() {
	for (Segment & seg : clsSegments) {
		if (!clsBounds.inside(seg.getBounds())) {
			std::cout << "Segment " << seg.getFullId() << " is outside its parent\n";
		}
	} // end for
} // end method

// -----------------------------------------------------------------------------

void Row::checkSanitySegmentsOverlap() {
	Segment * currentSeg = &clsSegments.front();
	Segment * leftSeg;
	while (currentSeg->getRightSegment() != nullptr) {
		leftSeg = currentSeg;
		currentSeg = currentSeg->getRightSegment();
		if (leftSeg->getBounds().overlapArea(currentSeg->getBounds()) > 1)
			std::cout << "Bins " << currentSeg->getFullId() << " and " << leftSeg->getFullId() << " overlap\n";
		if (currentSeg == leftSeg)
			std::cout << "Bin " << currentSeg->getFullId() << " is repeated\n";
	} // end while
} // end method

// -----------------------------------------------------------------------------

} // end namespace
