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
 * File:   PhysicalRow.h
 * Author: jucemar
 *
 * Created on 14 de Setembro de 2016, 22:29
 */

#ifndef PHYSICALDESIGN_PHYSICALROW_H
#define PHYSICALDESIGN_PHYSICALROW_H

namespace Rsyn {

class PhysicalRow : public Proxy<PhysicalRowData> {
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
	RSYN_FRIEND_OF_GENERIC_LIST_COLLECTION;
protected:
	PhysicalRow(PhysicalRowData * data) : Proxy(data) {}
public:
	PhysicalRow() : Proxy(nullptr) {}
	PhysicalRow(std::nullptr_t) : Proxy(nullptr) {}

	std::string getName() const;
	std::string getSiteName() const;
	DBU getWidth() const;
	DBU getHeight() const ;
	DBU getOrigin(const Dimension dim) const;
	DBUxy getOrigin() const;
	DBU getStep(const Dimension dim) const;
	DBUxy getStep() const;
	int getNumSites(const Dimension dim) const ;
	Rsyn::PhysicalSite getPhysicalSite() const;
	DBU getSiteWidth() const ;
	DBU getSiteHeight() const;
	DBUxy getCoordinate(const Boundary bound) const;
	DBU getCoordinate(const Boundary bound, const Dimension dim) const ;
	const Bounds &getBounds() const;
	Rsyn::PhysicalOrientation getSiteOrientation() const ;
	Rsyn::PhysicalSymmetry getSymmetry() const;
	const std::vector<Bounds> & allSegments() const;

}; // end class 

} // end namespace 


#endif /* PHYSICALDESIGN_PHYSICALROW_H */

