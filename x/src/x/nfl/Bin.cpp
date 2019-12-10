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
 * File:   Bin.cpp
 * Author: isoliveira
 * 
 * Created on 12 de Março de 2018, 17:29
 */

#include "Bin.h"
#include "Segment.h"

namespace NFL {

std::string Bin::getFullId(const std::string & separator) const {
	return clsSegment->getFullId(separator) + separator + std::to_string(getId());
} // end method 

// -----------------------------------------------------------------------------

Node * Bin::getOverlapNode(const DBU posX) {
	DBU center = getCenter(X);
	Node * node = getCacheNode();
	if (posX > center) {
		while (node) {
			if (node->hasOverlap(posX, X)) {
				return node;
			} // end if 
			node = node->getRightNode();
		} // end while 
	} else {
		while (node) {
			if (node->hasOverlap(posX, X)) {
				return node;
			} // end if 
			node = node->getLeftNode();
		} // end while 
	} // end if-else 
	return nullptr;
} // end method 

// -----------------------------------------------------------------------------

void Bin::insertNode(Node * node) {
	if (!node->isWhitespace()) {
		const Bounds & instBds = node->getBounds();
		Bounds overlap = clsBounds.overlapRectangle(instBds);
		clsUsage += overlap.computeLength(X);
	} // end if 
	clsNodes.insert(node);
	setCacheNode(node);
} // end method 

// -----------------------------------------------------------------------------

void Bin::removeNode(Node * node) {
	if (!node->isWhitespace()) {
		const Bounds & instBds = node->getBounds();
		Bounds overlap = clsBounds.overlapRectangle(instBds);
		clsUsage -= overlap.computeLength(X);
	} // end if 
	clsNodes.erase(node);
	removeCacheNode(node);
} // end method

// -----------------------------------------------------------------------------

void Bin::setCacheNode(Node * node) {
	if (!node) {
		return;
	} // end if
	const DBU center = getCenter(X);
	const Bounds & bds = node->getBounds();
	if (center >= bds[LOWER][X] && center <= bds[UPPER][X]) {
		clsCacheNode = node;
	} // end if 
} // end method

// -----------------------------------------------------------------------------

void Bin::removeCacheNode(Node * node) {
	if (!node || !hasCacheNode()) {
		return;
	} // end if

	Node * cache = getCacheNode();
	if (cache->getId() == node->getId())
		resertCacheNode();
} // end method

// -----------------------------------------------------------------------------

bool Bin::checkSanityBin() {
	if (checkSanityBinZeroWidthNode())
		return true;
	if (checkSanityBinNonOverlapNode())
		return true;
	if(checkSanityCacheHasOverlap())
		return true;
	if (checkSanityBinAreaNodeCoverage())
		return true;
	return false;
} // end method

// -----------------------------------------------------------------------------

bool Bin::checkSanityBinZeroWidthNode() {
	for (Node * node : clsNodes) {
		if (node->getWidth() <= 0) {
			std::cout << "Node " << node->getName() << " has width zero in bin " << getFullId()
				<< ". NodeBds: " << node->getBounds() << " binBds: " << getBounds()
				<< "\n";
			return true;
		} // end if 
	} // end for 
	return false;
} // end method

// -----------------------------------------------------------------------------

bool Bin::checkSanityBinNonOverlapNode() {
	const Bounds & binBds = getBounds();
	for (Node * node : clsNodes) {
		const Bounds & nodeBds = node->getBounds();
		DBU area = nodeBds.overlapArea(binBds);
		if (area == 0) {
			std::cout << "Node " << node->getName() << " has zero area overlap with bin " << getFullId()
				<< ". NodeBds: " << node->getBounds() << " binBds: " << getBounds()
				<< "\n";
			return true;
		} // end if 
	}
	return false;
} // end method

// -----------------------------------------------------------------------------

bool Bin::checkSanityBinAreaNodeCoverage() {
	Node * cache = getCacheNode();
	DBU cacheArea = 0;
	const Bounds & binBds = getBounds();
	DBU binArea = binBds.computeArea();
	
	if (cache) {
		Node * left = cache->getLeftNode();
		Node * right = cache->getRightNode();
		cacheArea += binBds.overlapArea(cache->getBounds());
		while(left) {
			const Bounds & bds = left->getBounds();
			const DBU area = bds.overlapArea(binBds);
			if(area == 0)
				break;
			cacheArea += area;
			left = left->getLeftNode();
		}
		while(right) {
			const Bounds & bds = right->getBounds();
			const DBU area = bds.overlapArea(binBds);
			if(area == 0)
				break;
			cacheArea += area;
			right = right->getRightNode();
		}
	}

	if (cacheArea != binArea) {
		std::cout << "Bin has node area coverage error. Bin " << getFullId()
			<< " binBds: " << getBounds()
			<<" nodeArea: "<<cacheArea<<" binArea: "<<binArea
			<< "\n";
		return true;
	} // end if 
	return false;
} // end method

// -----------------------------------------------------------------------------

bool Bin::checkSanityCacheHasOverlap() {
	Node * cache = getCacheNode();
	if(!cache)
		return true;
	const Bounds & binBds = getBounds();
	const Bounds & cacheBds = cache->getBounds();
	if(!binBds.overlap(cacheBds)) {
		std::cout<<"Cache has no overlap to bin. Cache: "<<cache->getName()
		<<" bin: "<<getFullId()<<" cacheBds: "<<cacheBds
		<<" binBds: "<<binBds
		<<"\n";
		return true;
	}
	DBU center = binBds.computeCenter(X);
	if(center < cacheBds[LOWER][X] || center > cacheBds[UPPER][X]){
		std::cout<<"Cache has no CENTER overlap to bin. Cache: "<<cache->getName()
		<<" bin: "<<getFullId()<<" cacheBds: "<<cacheBds
		<<" binBds: "<<binBds
		<<"\n";
		return true;
	}
	
	return false;
} // end method

// -----------------------------------------------------------------------------

} // end namespace
