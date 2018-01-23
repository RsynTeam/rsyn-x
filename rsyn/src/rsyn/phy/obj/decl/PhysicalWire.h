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
 * File:   PhysicalWire.h
 * Author: jucemar
 *
 * Created on 12 de Maio de 2017, 22:49
 */

#ifndef PHYSICALDESIGN_PHYSICALWIRE_H
#define PHYSICALDESIGN_PHYSICALWIRE_H

namespace Rsyn {

//! @brief Physical wire class stores wire segments of routed nets. 
class PhysicalWire : public Proxy<PhysicalWireData> {
	friend class PhysicalDesign;
protected:
	//! @brief Constructs a Rsyn::PhysicalWire object with a pointer to Rsyn::PhysicalWireData.

	PhysicalWire(PhysicalWireData * data) : Proxy(data) {
	}
public:
	//! @brief Constructs a Rsyn::PhysicalWire object with a null pointer to Rsyn::PhysicalWireData.

	PhysicalWire() : Proxy(nullptr) {
	}
	//! @brief Constructs a Rsyn::PhysicalWire object with a null pointer to Rsyn::PhysicalWireData.

	PhysicalWire(std::nullptr_t) : Proxy(nullptr) {
	}

	//! @brief Returns a vector reference to path of the wire.
	const std::vector<Rsyn::PhysicalWireSegment> & allWireSegments() const;
	
	//! @brief Returns the number of wire segments
	std::size_t getNumWireSegments() const;
}; // end class 

} // end namespace 


#endif /* PHYSICALDESIGN_PHYSICALWIRE_H */

