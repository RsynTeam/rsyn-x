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
 * File:   PhysicalLibraryCell.h
 * Author: jucemar
 *
 * Created on 12 de Setembro de 2016, 22:10
 */

#ifndef PHYSICALDESIGN_PHYSICALLIBRARYCELL_H
#define PHYSICALDESIGN_PHYSICALLIBRARYCELL_H

namespace Rsyn {

class PhysicalLibraryCell : public Proxy<PhysicalLibraryCellData> {
	friend class PhysicalDesign;
protected:
	PhysicalLibraryCell(PhysicalLibraryCellData * data) : Proxy(data) {}
public:
	PhysicalLibraryCell() : Proxy(nullptr) {}
	PhysicalLibraryCell(std::nullptr_t) : Proxy(nullptr) {}
	
	DBUxy getSize() const;
	DBU getWidth() const;
	DBU getHeight() const;
	DBU getLength(const Dimension dim) const ;
	bool isMacroBlock() const;
	bool hasLayerObs() const;
	const std::vector<Bounds> & allLayerObs() const;
	Rsyn::PhysicalMacroClass getClass() const;
	Rsyn::PhysicalSite getSite() const;
	const std::vector<Rsyn::PhysicalObstacle> & allObs() const ;
	std::size_t getNumObs() const; 
	
	// return 0  if the cell boundaries is not defined by one of layer metal. 
	// Otherwise returns the number of rectangles that composes the cell boundary
	std::size_t getNumPhysicalCellLayerBoundaries() const; 
	
	Rsyn::PhysicalObstacle getLayerObs() const;
	
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALLIBRARYCELL_H */

