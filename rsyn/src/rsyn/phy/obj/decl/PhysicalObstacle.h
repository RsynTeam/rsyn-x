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
 * File:   PhysicalObstacle.h
 * Author: jucemar
 *
 * Created on 13 de Setembro de 2016, 20:08
 */

#ifndef PHYSICALDESIGN_PHYSICALOBSTACLE_H
#define PHYSICALDESIGN_PHYSICALOBSTACLE_H

namespace Rsyn {

class PhysicalObstacle : public Proxy < PhysicalObstacleData > { 
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
protected:
	PhysicalObstacle(PhysicalObstacleData * data) : Proxy(data) {}
public:
	PhysicalObstacle() : Proxy(nullptr) {}
	PhysicalObstacle(std::nullptr_t) : Proxy(nullptr) {}
	
	Rsyn::PhysicalLayer getLayer() const;
	const std::vector<Bounds> & allBounds() const;
	std::size_t getNumObs() const;
	bool hasLayer() const;
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALOBSTACLE_H */

