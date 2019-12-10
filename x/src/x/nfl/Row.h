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
 * File:   Row.h
 * Author: isoliveira
 *
 * Created on 12 de Março de 2018, 17:32
 */

#ifndef NFL_ROW_H
#define NFL_ROW_H

#include "NFLegalUtil.h"
#include "rsyn/util/Bounds.h"
#include "rsyn/phy/PhysicalDesign.h"

namespace NFL {

class NFLegal;
class Segment;
class Node;
class Bin;

class Row {
private:
	int clsId = -1;
	int clsNumBins = 0;
	Bounds clsBounds;
	DBUxy clsBinLength;
	std::vector<Segment> clsSegments;
	std::deque<Bounds> clsBlockages;
	BoostRTree clsRTreeSegments;
	NFLegal * clsNFLegal = nullptr;
	Rsyn::PhysicalRow clsPhRow;

public:
	Row() = default;
	virtual ~Row() = default;

	void initLegalize();
	
	void setNFLegal(NFLegal * nflegal) {
		clsNFLegal = nflegal;
	} // end method 

	void setId(const int id) {
		clsId = id;
	} // end method

	void setBounds(const Bounds & bounds) {
		clsBounds = bounds;
	} // end method

	void setBinLength(const DBUxy length) {
		clsBinLength = length;
	} // end method

	void setPhysicalRow(Rsyn::PhysicalRow phRow) {
		clsPhRow = phRow;
	} // end method 

	DBU getDefaultBinLength(const Dimension dim) const {
		return clsBinLength[dim];
	} // end method 

	DBUxy getDefaultBinLength() const {
		return clsBinLength;
	} // end method 

	DBU getCoordinate(const Boundary bound, const Dimension dim) const {
		return clsBounds[bound][dim];
	} // end method 

	DBUxy getCoordinate(const Boundary bound) const {
		return clsBounds[bound];
	} // end method 

	DBU getPosition(const Dimension dim) const {
		return getCoordinate(LOWER, dim);
	} // end method 

	DBUxy getPosition() const {
		return getCoordinate(LOWER);
	} // end method 

	DBU getCenter(const Dimension dim) const {
		return clsPhRow.getBounds().computeCenter(dim);
	} // end method 

	DBUxy getCenter() const {
		return clsPhRow.getBounds().computeCenter();
	} // end method 

	DBU getSiteWidth() const {
		return clsPhRow.getSiteWidth();
	} // end method 

	int getId() const {
		return clsId;
	} // end method

	int getNumBins() const {
		return clsNumBins;
	} // end method 
	
	const Bounds & getBounds() const {
		return clsBounds;
	}

	int getNumBlockages() const {
		return clsBlockages.size();
	} // end method

	int getNumSegments() const {
		return clsSegments.size();
	} // end method

	const std::deque<Bounds>& allBlockages() const {
		return clsBlockages;
	} // end method

	bool hasBlockages() const {
		return !clsBlockages.empty();
	} // end method 
	
	const std::vector<Segment>& allSegments() const {
		return clsSegments;
	} // end method
	
	std::vector<Segment>& allSegments() {
		return clsSegments;
	} // end method

	Segment * getFrontSegment() {
		return &clsSegments.front();
	} // end method 

	Segment * getBackSegment() {
		return &clsSegments.back();
	} // end method 

	void addBlockage(const Bounds & block);
	void initBlockages();
	void initSegments();

	bool insertNode(Node * inst);

	Segment * getNearestSegment(const DBUxy pos, const DBU width);
	Segment * getSegment(const DBUxy pos);
	

	DBU getDisplacement(const DBUxy pos);
	DBU getDisplacement(const DBU pos, const Dimension dim);

	
	// Legalization
	DBU getSiteAdjustedPosition(const DBU posX, const bool roundUp = false);
	

	void moveNode(Node * node, Segment * src, Segment *sink, const DBUxy targetPos);
protected:
	void placeCell(Rsyn::PhysicalCell phCell, DBUxy pos);
	void mergeNeighborBlockages();


public:
	void checkSanitySegmentsConnected();
	void checkSanitySegmentsInside();
	void checkSanitySegmentsOverlap();

}; // end class

} // end namespace

#endif /* NFL_ROW_H */

