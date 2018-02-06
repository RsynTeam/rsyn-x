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
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PhysicalViaLayer.h
 * Author: jucemar
 *
 * Created on 14 de Maio de 2017, 15:02
 */

#ifndef PHYSICALDESIGN_PHYSICALVIALAYER_H
#define PHYSICALDESIGN_PHYSICALVIALAYER_H

namespace Rsyn {

class PhysicalViaLayer : public Proxy<PhysicalViaLayerData> {
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
	friend class PhysicalVia;
	RSYN_FRIEND_OF_GENERIC_LIST_COLLECTION;
protected:
	//! @brief Constructs a Rsyn::PhysicalViaLayer object with a pointer to Rsyn::PhysicalViaLayerData.

	PhysicalViaLayer(PhysicalViaLayerData * data) : Proxy(data) {
	}
public:
	//! @brief Constructs a Rsyn::PhysicalViaLayer object with a null pointer to Rsyn::PhysicalViaLayerData.

	PhysicalViaLayer() : Proxy(nullptr) {
	}
	//! @brief Constructs a Rsyn::PhysicalViaLayer object with a null pointer to Rsyn::PhysicalViaLayerData.

	PhysicalViaLayer(std::nullptr_t) : Proxy(nullptr) {
	}

	//! @brief Returns Physical Layer object associated to physical via layer.
	Rsyn::PhysicalLayer getLayer() const;
	//! @brief Returns a constant reference to the Boundaries vector.
	const std::vector<Bounds> & allBounds() const;
	//! @brief Returns number of Boundaries.
	std::size_t getNumBounds() const;
	Rsyn::PhysicalVia getVia() const;
}; // end class 


} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALVIALAYER_H */

