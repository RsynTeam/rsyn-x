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
 * Created on 18 de Setembro de 2016, 09:00
 */

namespace Rsyn {

inline DBUxy PhysicalPin::getDisplacement() const {
	return data->clsDisplacement;
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalPin::getDisplacement(const Dimension dim) {
	return data->clsDisplacement[dim];
} // end method 

// -----------------------------------------------------------------------------

inline std::vector<PhysicalPinPort> & PhysicalPin::allPinPorts() {
	return data->clsPhysicalPinPorts;
} // end method 

// -----------------------------------------------------------------------------

inline const std::vector<PhysicalPinPort> & PhysicalPin::allPinPorts() const {
	return data->clsPhysicalPinPorts;
} // end method 

// -----------------------------------------------------------------------------

inline std::size_t PhysicalPin::getNumPinPorts() const {
	return data->clsPhysicalPinPorts.size();
} // end method 

// -----------------------------------------------------------------------------

inline bool PhysicalPin::hasPinPorts() const {
	return !data->clsPhysicalPinPorts.empty();
} // end method 

// -----------------------------------------------------------------------------

inline bool PhysicalPin::isPinPortsEmpty() const {
	return data->clsPhysicalPinPorts.empty();
} // end method 

// -----------------------------------------------------------------------------

inline PhysicalPinDirection PhysicalPin::getDirection() const {
	return data->clsDirection;
} // end method 

// -----------------------------------------------------------------------------

inline const Bounds & PhysicalPin::getLayerBounds() const {
	return data->clsLayerBound;
} // end method 

// -----------------------------------------------------------------------------

} // end namespace 