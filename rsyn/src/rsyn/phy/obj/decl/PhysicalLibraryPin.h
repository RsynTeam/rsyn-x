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
 * File:   PhysicalLibraryPin.h
 * Author: jucemar
 *
 * Created on 13 de Setembro de 2016, 21:06
 */

#ifndef PHYSICALDESIGN_PHYSICALLIBRARYPIN_H
#define PHYSICALDESIGN_PHYSICALLIBRARYPIN_H

namespace Rsyn {

class PhysicalLibraryPin : public Proxy<PhysicalLibraryPinData> {
	friend class PhysicalDesign;
protected:
	PhysicalLibraryPin(PhysicalLibraryPinData * data) : Proxy(data) {}
public:
	PhysicalLibraryPin() : Proxy(nullptr) {}
	PhysicalLibraryPin(std::nullptr_t) : Proxy(nullptr) {}

	Bounds getICCADBounds();
	const Bounds & getICCADBounds() const;
	std::vector<PhysicalPinPort> & allPinPorts();
	const std::vector<PhysicalPinPort> & allPinPorts() const;
	std::size_t getNumPinPorts() const;
	bool hasPinPorts() const;
	bool isPinPortsEmpty() const;
	bool hasLibraryPin() const;
	PhysicalPinDirection getPinDirection() const;
	
}; // end class  

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALLIBRARYPIN_H */

