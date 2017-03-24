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
 * File:   PhysicalPinPort.h
 * Author: jucemar
 *
 * Created on 13 de Setembro de 2016, 21:13
 */

#ifndef PHYSICALDESIGN_PHYSICALPINPORT_H
#define PHYSICALDESIGN_PHYSICALPINPORT_H

namespace Rsyn {

class PhysicalPinPort : public Proxy<PhysicalPinPortData> {
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
protected:
	//! @brief Constructs a Rsyn::PhysicalPinPort object with a pointer to Rsyn::PhysicalPinPortData.
	PhysicalPinPort(PhysicalPinPortData * data) : Proxy(data) {}
public:
	//! @brief Constructs a Rsyn::PhysicalPinPort object with a null pointer to Rsyn::PhysicalPinPortData.
	PhysicalPinPort() : Proxy(nullptr) {}
	//! @brief Constructs a Rsyn::PhysicalPinPort object with a null pointer to Rsyn::PhysicalPinPortData.
	PhysicalPinPort(std::nullptr_t) : Proxy(nullptr) {}
	
	//! @brief Returns the PhysicalPinLayer associated to the PhysicalPinPort.
	Rsyn::PhysicalPinLayer getPinLayer() const;
	//! @brief Returns the PhysicalPinPortClass type of the PhysicalPinLayer.
	//! @details A PhysicalPinPortClass may be: 1) PINPORTCLASS_NONE (default), 2) PINPORTCLASS_CORE, or PINPORTCLASS_BUMP.
	Rsyn::PhysicalPinPortClass getPinPortClass() const;
	//! @brief Returns true if a PhysicalPinLayer was associated to the PhysicalPinPort. 
	//! Otherwise, returns false.
	bool hasPinLayer() const ;
}; // end class 

} // end namespace 


#endif /* PHYSICALDESIGN_PHYSICALPINPORT_H */

