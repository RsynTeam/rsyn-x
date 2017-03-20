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
 * File:   PhysicalPinLayer.h
 * Author: jucemar
 *
 * Created on 13 de Setembro de 2016, 21:19
 */

#ifndef PHYSICALDESIGN_PHYSICALPINLAYER_H
#define PHYSICALDESIGN_PHYSICALPINLAYER_H

namespace Rsyn {

class PhysicalPinLayer : public Proxy<PhysicalPinLayerData> {
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
protected:
	PhysicalPinLayer(PhysicalPinLayerData * data) : Proxy(data) {}
public:
	PhysicalPinLayer() : Proxy(nullptr) {}
	PhysicalPinLayer(std::nullptr_t) : Proxy(nullptr) {}

	const std::vector<Bounds> & allBounds() const;
	const std::vector<Polygon> & allPolygons() const;
	PhysicalLayer getLayer() const;
	std::size_t getNumBounds() const;
	std::size_t getNumPolygons() const;
	bool hasRectangleBounds() const;
	bool hasPolygonBounds() const;
	bool isRectangleBoundsEmpty() const;
	bool isPolygonBoundsEmpty() const;
	bool hasLayer() const;
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALPINLAYER_H */

