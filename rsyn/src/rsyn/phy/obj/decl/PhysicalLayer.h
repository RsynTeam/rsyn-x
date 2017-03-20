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
 * File:   PhysicalLayer.h
 * Author: jucemar
 *
 * Created on 13 de Setembro de 2016, 19:12
 */

#ifndef PHYSICALDESIGN_PHYSICALLAYER_H
#define PHYSICALDESIGN_PHYSICALLAYER_H

namespace Rsyn {

class PhysicalLayer : public Proxy<PhysicalLayerData> {
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
protected:

	PhysicalLayer(PhysicalLayerData * data) : Proxy(data) {	}
public:
	PhysicalLayer() : Proxy(nullptr) {}
	PhysicalLayer(std::nullptr_t) : Proxy(nullptr) {}
	
	std::string getName() const;
	Rsyn::PhysicalLayerType getType() const;
	Rsyn::PhysicalLayerDirection getDirection() const;
	DBU getWidth() const;
	DBU getPitch() const;
	DBU getSpacing() const;
}; // end class 

} // end namespace 


#endif /* PHYSICALDESIGN_PHYSICALLAYER_H */

