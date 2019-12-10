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
 * File:   Blockage.h
 * Author: isoliveira
 *
 * Created on 12 de Março de 2018, 18:19
 */

#ifndef NFL_BLOCKAGE_H
#define NFL_BLOCKAGE_H

#include "rsyn/util/Bounds.h"
#include <vector>

namespace NFL {

class Segment;
class Bin;

class Blockage {
private:
	Bounds clsBounds;
	std::vector<Segment*> clsLeftSegments;
	std::vector<Segment*> clsRightSegments;
	std::vector<Segment*> clsUpperSegments;
	std::vector<Segment*> clsLowerSegments;
	std::vector <Blockage*> clsUpperBlockages;
	std::vector <Blockage*> clsLowerBlockages;
	std::vector <Segment*> clsNeighborSegments;
	int clsId;
public:
	Blockage() = default;
	virtual ~Blockage() = default;
	
	void setId(const int id) {
		clsId = id;
	} // end method
	
	void setBounds(const Bounds & bounds) {
		clsBounds = bounds;
	} // end method
	
	const Bounds & getBounds() const {
		return clsBounds;
	} // end method

	DBUxy getPosition() const {
		return clsBounds.getCoordinate(LOWER);
	} // end method
	
	DBU getPosition(const Dimension dim) const {
		return clsBounds.getCoordinate(LOWER, dim);
	} // end method
	
	DBUxy getCoordinate(const Boundary bound) const {
		return clsBounds.getCoordinate(bound);
	} // end method
	
	DBU getCoordinate(const Boundary bound, const Dimension dim) const {
		return clsBounds.getCoordinate(bound, dim);
	} // end method
	
	std::vector<Segment*> & getLeftSegments() {
		return clsLeftSegments;
	} // end method

	std::vector<Segment*> & getRightSegments() {
		return clsRightSegments;
	} // end method

	std::vector<Segment*> &getLowerSegments() {
		return clsLowerSegments;
	} // end method

	std::vector<Segment*> &getUpperSegments() {
		return clsUpperSegments;
	} // end method

	std::vector<Segment*> &getAllNeighborSegments() {
		return clsNeighborSegments;
	} // end method

	std::vector<Blockage*> &getUpperBlockages() {
		return clsUpperBlockages;
	} // end method

	std::vector<Blockage*> &getLowerBlockages() {
		return clsLowerBlockages;
	} // end method

	std::size_t getId() const {
		return clsId;
	} // end method

}; // end class

} // end namespace 

#endif /* NFL_BLOCKAGE_H */

