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
 * File:   Node.h
 * Author: isoliveira
 *
 * Created on 12 de Março de 2018, 17:32
 */

#ifndef NFL_NODE_H
#define NFL_NODE_H
#include <Rsyn/PhysicalDesign>

namespace NFL {

class Node {
protected:
	Bounds clsWhitespaceBounds;
	Rsyn::PhysicalCell clsPhCell;
	Node * clsLeft = nullptr;
	Node * clsRight = nullptr;
	int clsId = -1;
	bool clsWhitespace : 1;
	bool clsDeleted : 1;
	bool clsLegalized : 1;
public:

	Node() {
		clsWhitespace = false;
		clsDeleted = false;
		clsLegalized = false;
	} // end constructor 
	//Node(const Node& orig) = delete;
	virtual ~Node() = default;

	void setPhysicalCell(Rsyn::PhysicalCell phCell) {
		clsPhCell = phCell;
	} // end method 

	void setId(const int id) {
		clsId = id;
	} // end method

	void setWhitespace(const bool whitespace) {
		clsWhitespace = whitespace;
	} // end method 

	void setDeletedWhitespace(const bool deleted) {
		clsDeleted = deleted;
	} // end method 

	void setLegalized(const bool legalized) {
		clsLegalized = legalized;
	} // end method 

	void setWhitespaceBounds(const Boundary bound, const Dimension dim, const DBU pos) {
		clsWhitespaceBounds[bound][dim] = pos;
	} // end method 

	void setWhitespaceBounds(const Boundary bound, const DBUxy pos) {
		clsWhitespaceBounds[bound] = pos;
	} // end method 

	void setWhitespaceBounds(const Bounds & bds) {
		clsWhitespaceBounds = bds;
	} // end method 

	void setDisplacementWhitespaceBounds(const Boundary bound, const Dimension dim,
		const DBU disp) {
		clsWhitespaceBounds[bound][dim] += disp;
	} // end method 

	void setLeftNode(Node * left) {
		clsLeft = left;
	} // end method 

	void setRightNode(Node * right) {
		clsRight = right;
	} // end method 

	void disconectNode() {
		clsLeft = nullptr;
		clsRight = nullptr;
	} // end method 

	Rsyn::PhysicalCell getPhCell() const {
		return clsPhCell;
	} // end method

	int getId() const {
		return clsId;
	} // end method

	bool isWhitespace() const {
		return clsWhitespace;
	} // end method 

	bool isDeleted() const {
		return clsDeleted;
	} // end method 

	bool isLegalized() const {
		return clsLegalized;
	} // end method 

	bool hasLeftNode() const {
		return clsLeft;
	} // end method 

	bool hasRightNode() const {
		return clsRight;
	} // end method 

	bool hasOverlap(const DBUxy pos) const {
		const Bounds & bds = getBounds();
		return bds.inside(pos);
	} // end method 

	bool hasOverlap(const DBU pos, const Dimension dim) const {
		const Bounds & bds = getBounds();
		return pos >= bds[LOWER][dim] && pos <= bds[UPPER][dim];
	} // end method 

	DBUxy getPosition() const {
		if (isWhitespace()) {
			return getCoordinate(LOWER);
		} else {
			return clsPhCell.getPosition();
		} // end if-else 
	} // end method

	DBU getPosition(const Dimension dim) const {
		if (isWhitespace()) {
			return getCoordinate(LOWER, dim);
		} else {
			return clsPhCell.getPosition(dim);
		} // end if-else 
	} // end method

	DBUxy getInitialPosition() const {
		if (isWhitespace()) {
			return getCoordinate(LOWER);
		} else {
			return clsPhCell.getInitialPosition();
		} // end if-else 
	} // end method

	DBU getInitialPosition(const Dimension dim) const {
		if (isWhitespace()) {
			return getCoordinate(LOWER, dim);
		} else {
			return clsPhCell.getInitialPosition(dim);
		} // end if-else 
	} // end method

	DBUxy getCoordinate(const Boundary bound) const {
		if (isWhitespace()) {
			return clsWhitespaceBounds[bound];
		} else {
			return clsPhCell.getCoordinate(bound);
		} // end if-else 
	} // end method

	DBU getCoordinate(const Boundary bound, const Dimension dim) const {
		if (isWhitespace()) {
			return clsWhitespaceBounds[bound][dim];
		} else {
			return clsPhCell.getCoordinate(bound, dim);
		}
	} // end method

	DBU getDisplacement() const {
		if (isWhitespace())
			return 0;
		return clsPhCell.getDisplacement();
	} // end method

	DBU getDisplacement(const Dimension dim) const {
		if (isWhitespace())
			return 0;
		return clsPhCell.getDisplacement(dim);
	} // end method

	DBU getWidth() const {
		if (isWhitespace())
			return clsWhitespaceBounds.getWidth();
		return clsPhCell.getWidth();
	} // end method

	const Bounds & getBounds() const {
		if (isWhitespace())
			return clsWhitespaceBounds;
		return clsPhCell.getBounds();
	} // end method

	Rsyn::PhysicalCell getPhysicalCell() const {
		return clsPhCell;
	} // end method

	Rsyn::Instance getInstance() const {
		if (isWhitespace())
			return Rsyn::Instance();
		return clsPhCell.getInstance();
	} // end method

	std::string getName() const {
		if (isWhitespace())
			return "NFL_WS_" + std::to_string(getId());
		return clsPhCell.getInstance().getName();
	} // end method

	Node * getLeftNode() const {
		return clsLeft;
	} // end method 

	Node * getRightNode() const {
		return clsRight;
	} // end method 

}; // end class

} // end namespace 

#endif /* NFL_NODE_H */

