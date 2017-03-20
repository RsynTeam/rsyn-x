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
 * File:   PhysicalNet.h
 * Author: jucemar
 *
 * Created on 15 de Setembro de 2016, 19:16
 */

#ifndef PHYSICALDESIGN_PHYSICALNET_H
#define PHYSICALDESIGN_PHYSICALNET_H

namespace Rsyn { 

class PhysicalNet : public Proxy<PhysicalNetData> {
	friend class PhysicalDesign;
protected:
	PhysicalNet(PhysicalNetData * data) : Proxy(data) {}
public:
	PhysicalNet() : Proxy(nullptr) {}
	PhysicalNet(std::nullptr_t) : Proxy(nullptr) {}

	DBUxy getHPWL() const;
	DBU getHPWL(const Dimension dim);
	const Bounds & getBounds() const;
	DBUxy getCoordinate(const Boundary bound) const ;
	DBU getCoordinate(const Boundary bound, const Dimension dim) const;
	Rsyn::Pin getPinBoundary(const Boundary bound, const Dimension dim) const;
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALNET_H */

