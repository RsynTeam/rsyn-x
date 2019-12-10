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
 * File:   NFLegalUtil.h
 * Author: isoliveira
 *
 * Created on 12 de Março de 2018, 18:20
 */

#ifndef NFL_NFLEGALUTIL_H
#define NFL_NFLEGALUTIL_H

#include "rsyn/util/dbu.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp> 

//#include "Node.h"

namespace NFL {

class Node;
class Segment;

// Boost defines
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace boostGeometry = boost::geometry;
typedef boostGeometry::model::point<DBU, 2, boostGeometry::cs::cartesian> PhysicalPolygonPoint;
typedef bg::model::box<PhysicalPolygonPoint> Box;
typedef std::pair<Box, int> BoxPair;
typedef boost::geometry::index::rtree< BoxPair, boost::geometry::index::quadratic < 8, 2 >> BoostRTree;


// -----------------------------------------------------------------------------

struct LegalAux {
	Node * clsNode = nullptr;
	Node * clsReferenceNode = nullptr;
	Segment * clsLegalSegment = nullptr;
	Segment * clsOriginSegment = nullptr;
	DBU clsLeftMove = 0;
	DBU clsRightMove = 0;
	DBU clsCost = 0;
	
	// TODO REMOVE
	DBUxy clsPos;
	DBU clsRequiredMove = 0;
	bool clsMoveOverlapToLeft : 1;
	bool clsMoveOverlapToRight : 1;
	// END TODO 
	LegalAux() {
		clsMoveOverlapToLeft = false;
		clsMoveOverlapToRight = false;
	} // end constructor 
}; // end struct 

// -----------------------------------------------------------------------------

} // end namespace 

#endif /* NFL_NFLEGALUTIL_H */

