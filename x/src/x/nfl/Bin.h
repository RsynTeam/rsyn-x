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
 * File:   Bin.h
 * Author: isoliveira
 *
 * Created on 12 de Março de 2018, 17:29
 */

#ifndef NFL_BIN_H
#define NFL_BIN_H
#include "Node.h"
#include "Segment.h"


namespace NFL {

class Segment;

class Bin {
protected:

	struct cmp {

		bool operator()(const Node* inst0, const Node* inst1) {
			const DBU pos0 = inst0->getPosition(X);
			const DBU pos1 = inst1->getPosition(X);
			if (pos0 == pos1)
				return inst0->getId() < inst1->getId();
			return pos0 < pos1;
		} // end operator() 
	}; // end struct 

	Bounds clsBounds;
	DBU clsUsage = 0;
	Bin * clsLeft = nullptr;
	Bin * clsRight = nullptr;
	Node * clsCacheNode = nullptr;
	std::vector <Bin*> clsUpper;
	std::vector <Bin*> clsLower;
	std::vector <Bin*> clsNeighbors;
	std::set<Node*, cmp>clsNodes;
	Segment * clsSegment;
	int clsId = -1;
public:
	Bin() = default;
	virtual ~Bin() = default;

	void setBounds(const Bounds & bds) {
		clsBounds = bds;
	} // end method

	void setId(const int id) {
		clsId = id;
	} // end method

	const Bounds & getBounds() const {
		return clsBounds;
	} // end method

	void setLeft(Bin * left) {
		clsLeft = left;
		clsNeighbors.push_back(left);
	} // end method 

	void setRight(Bin * right) {
		clsRight = right;
		clsNeighbors.push_back(right);
	} // end method 

	Node * getOverlapNode(const DBU posX);

	void addLower(Bin * bin) {
		clsLower.push_back(bin);
		clsNeighbors.push_back(bin);
	} // end method 

	void addUpper(Bin * bin) {
		clsUpper.push_back(bin);
		clsNeighbors.push_back(bin);
	} // end method 

	void setSegment(Segment * seg) {
		clsSegment = seg;
	} // end method

	int getId() const {
		return clsId;
	} // end method

	int getNumNodes() {
		return clsNodes.size();
	} // end method 

	DBUxy getCoordinate(const Boundary bound) const {
		return clsBounds.getCoordinate(bound);
	} // end method 

	DBU getCoordinate(const Boundary bound, const Dimension dim) const {
		return clsBounds.getCoordinate(bound, dim);
	} // end method 

	DBUxy getPosition()const {
		return getCoordinate(LOWER);
	} // end method 

	DBU getPosition(const Dimension dim)const {
		return getCoordinate(LOWER, dim);
	} // end method 

	DBUxy getCenter() const {
		return clsBounds.computeCenter();
	} // end method 

	DBU getCenter(const Dimension dim) const {
		return clsBounds.computeCenter(dim);
	} // end method 

	DBU getWidth() const {
		return clsBounds.computeLength(X);
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
		return clsUsage;
	} // end method 

	Bin * getLeft() const {
		return clsLeft;
	} // end method

	Bin * getRight() const {
		return clsRight;
	} // end method

	std::vector <Bin*>& getLower() {
		return clsLower;
	} // end method

	std::vector<Bin*> &getUpper() {
		return clsUpper;
	} // end method

	std::vector<Bin*> &allNeighbors() {
		return clsNeighbors;
	} // end method
	
	const std::vector<Bin*> &allNeighbors() const {
		return clsNeighbors;
	} // end method

	const std::set<Node*, Bin::cmp> & allNodes() const {
		return clsNodes;
	} // end method 

	const Segment * getSegment() const {
		return clsSegment;
	} // end method

	Segment * getSegment() {
		return clsSegment;
	} // end method

	void insertNode(Node * inst);
	void removeNode(Node * inst);

	std::string getFullId(const std::string & separator = ":") const;

	Node * getCacheNode() const {
		return clsCacheNode;
	} // end method 

	bool hasCacheNode() const {
		return clsCacheNode;
	} // end method 

	void resertCacheNode() {
		clsCacheNode = nullptr;
	} // end method 
	
	void setCacheNode(Node * node);
	void removeCacheNode(Node * node);
	
	bool checkSanityBin();

protected:
	bool checkSanityBinZeroWidthNode();
	bool checkSanityBinNonOverlapNode();
	bool checkSanityBinAreaNodeCoverage();
	bool checkSanityCacheHasOverlap();
}; // end class

} // end namespace 

#endif /* NFL_BIN_H */

