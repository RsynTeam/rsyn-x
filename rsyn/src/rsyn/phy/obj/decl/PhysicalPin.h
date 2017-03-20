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
 * File:   PhysicalPin.h
 * Author: jucemar
 *
 * Created on 14 de Setembro de 2016, 21:26
 */

#ifndef PHYSICALDESIGN_PHYSICALPIN_H
#define PHYSICALDESIGN_PHYSICALPIN_H

namespace Rsyn {

class PhysicalPin : public Proxy<PhysicalPinData> {
	friend class PhysicalDesign;
protected:
	PhysicalPin(PhysicalPinData * data) : Proxy(data) {}
public:
	PhysicalPin() : Proxy(nullptr) {}
	PhysicalPin(std::nullptr_t) : Proxy(nullptr) {}

	DBUxy getDisplacement () const;
	DBU getDisplacement(const Dimension dim);
	std::vector<PhysicalPinPort> & allPinPorts();
	const std::vector<PhysicalPinPort> & allPinPorts() const;
	std::size_t getNumPinPorts() const;
	bool hasPinPorts() const;
	bool isPinPortsEmpty() const;
	PhysicalPinDirection getDirection () const;
	const Bounds & getLayerBounds() const;
	
}; // end class 

} // end namespace 


#endif /* PHYSICALDESIGN_PHYSICALPIN_H */

