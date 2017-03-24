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
 * File:   PhysicalPinPortData.h
 * Author: jucemar
 *
 * Created on 13 de Setembro de 2016, 21:10
 */

#ifndef PHYSICALDESIGN_PHYSICALPINPORTDATA_H
#define PHYSICALDESIGN_PHYSICALPINPORTDATA_H

namespace Rsyn {

class PhysicalPinPortData : public PhysicalObject {
public:
	//! @brief An enum PhysicalPinPortClass that determines the class of the PhysicalPinPort object.
	PhysicalPinPortClass clsPinPortClass;
	//! @brief The PhysicalPinLayer associated to the PhysicalPinPort.
	PhysicalPinLayer clsLayer;
	//! @brief Default constructor.
	//! @details Users do not have access to this constructor. The reference to 
	//! PhysicalPinPortData is a protected variable in PhysicalPinPort.
	PhysicalPinPortData() {
		clsPinPortClass = PINPORTCLASS_INVALID;
	} // end constructor 
	
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALPINPORTDATA_H */

