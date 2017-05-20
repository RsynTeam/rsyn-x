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
 * File:   PhysicalViaInstance.h
 * Author: jucemar
 *
 * Created on 15 de Maio de 2017, 12:43
 */

#ifndef PHYSICALVIAINSTANCE_H
#define PHYSICALVIAINSTANCE_H

namespace Rsyn {

class PhysicalViaInstance : public Proxy<PhysicalViaInstanceData> {
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
	RSYN_FRIEND_OF_GENERIC_LIST_COLLECTION;
protected:
	//! @brief Constructs a Rsyn::PhysicalViaInstance object with a pointer to Rsyn::PhysicalViaInstanceData.

	PhysicalViaInstance(PhysicalViaInstanceData * data) : Proxy(data) {
	}
public:
	//! @brief Constructs a Rsyn::PhysicalViaInstance object with a null pointer to Rsyn::PhysicalViaInstanceData.

	PhysicalViaInstance() : Proxy(nullptr) {
	}
	//! @brief Constructs a Rsyn::PhysicalViaInstance object with a null pointer to Rsyn::PhysicalViaInstanceData.

	PhysicalViaInstance(std::nullptr_t) : Proxy(nullptr) {
	}

	//! @brief Returns library (lef) Via instantiated in PhysicalViaInstance object 
	Rsyn::PhysicalVia getLibraryVia() const;
	//! @brief Returns PhysicalViaInstance position. It is the last position of the Segment. 
	DBUxy getPosition() const;
	//! @brief Returns PhysicalViaInstance position for given dimension. It is the last position of the Segment.
	DBU getPosition(const Dimension dim) const;
}; // end class 

} // end namespace 

#endif /* PHYSICALVIAINSTANCE_H */

