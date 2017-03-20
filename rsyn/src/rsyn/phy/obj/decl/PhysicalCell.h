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
 
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PhysicalCell.h
 * Author: jucemar
 *
 * Created on 12 de Setembro de 2016, 21:04
 */

#ifndef PHYSICALDESIGN_PHYSICALCELL_H
#define PHYSICALDESIGN_PHYSICALCELL_H

namespace Rsyn {

class PhysicalCell : public PhysicalInstance {
	friend class PhysicalDesign;
protected:
	PhysicalCell(PhysicalInstanceData * data) : PhysicalInstance(data) {}
public:
	PhysicalCell() : PhysicalInstance(nullptr) {}
	PhysicalCell(std::nullptr_t): PhysicalInstance(nullptr)   {} 

	bool isFixed() const;
	bool isMacroBlock() const;
	bool isPlaced() const;
	//bool isFixedInInputFile() const;
	DBUxy getInitialPosition() const;
	DBU getInitialPosition(const Dimension dim) const;
	DBU getDisplacement() const;
	DBU getDisplacement(const Dimension dim) const;
	DBU getDisplacement(const DBUxy pos) const;
	DBU getDisplacement(const DBU pos, const Dimension dim) const;
	PhysicalOrientation getOrientation() const;
	bool hasLayerBounds() const; // The bounds of a cell is defined by one of the layers. 
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALCELL_H */

