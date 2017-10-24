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
 * File:   PhysicalRoutingPoint.h
 * Author: jucemar
 *
 * Created on 23 de Maio de 2017, 20:32
 */

#ifndef PHYSICALDESIGN_PHYSICALROUTINGPOINT_H
#define PHYSICALDESIGN_PHYSICALROUTINGPOINT_H

namespace Rsyn {

class PhysicalRoutingPoint : public Proxy<PhysicalRoutingPointData> {
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
protected:
	//! @brief Constructs a Rsyn::PhysicalRoutingPoint object with a pointer to Rsyn::PhysicalRoutingPointData.

	PhysicalRoutingPoint(PhysicalRoutingPointData * data) : Proxy(data) {
	}
public:
	//! @brief Constructs a Rsyn::PhysicalRoutingPoint object with a null pointer to Rsyn::PhysicalRoutingPointData.

	PhysicalRoutingPoint() : Proxy(nullptr) {
	}
	//! @brief Constructs a Rsyn::PhysicalRoutingPoint object with a null pointer to Rsyn::PhysicalRoutingPointData.

	PhysicalRoutingPoint(std::nullptr_t) : Proxy(nullptr) {
	}

	DBUxy getPosition() const;
	DBU getPosition(const Dimension dim) const;
	DBU getExtension() const;
	Rsyn::PhysicalVia getVia() const;
	
	const Bounds & getRectangle() const;
	bool hasExtension() const;
	bool hasRectangle() const;
	bool hasVia() const;
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALROUTINGPOINT_H */

