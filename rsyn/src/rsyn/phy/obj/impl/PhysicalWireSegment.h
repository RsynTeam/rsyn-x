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
 * File:   PhysicalWireSegment.h
 * Author: jucemar
 *
 * Created on 13 de Maio de 2017, 16:01
 */


namespace Rsyn {

inline bool PhysicalWireSegment::hasPoints() const {
	return data->clsPoints.size() > 0;
} // end method 

// -----------------------------------------------------------------------------

inline const std::vector<DBUxy> & PhysicalWireSegment::allSegmentPoints() const {
	return data->clsPoints;
} // end method 

// -----------------------------------------------------------------------------

inline DBUxy PhysicalWireSegment::getPoint(const std::size_t index) const {
	return data->clsPoints[index];
} // end method 

// -----------------------------------------------------------------------------

inline const std::size_t PhysicalWireSegment::getNumPoints() const {
	return data->clsPoints.size();
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalWireSegment::getExtension() const {
	return data->clsWireExtension;
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalLayer PhysicalWireSegment::getLayer() const {
	return data->clsPhysicalLayer;
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalVia PhysicalWireSegment::getVia() const {
	return data->clsPhysicalVia;
} // end method 

// -----------------------------------------------------------------------------

inline bool PhysicalWireSegment::hasVia() const {
	return data->clsPhysicalVia != nullptr;
} // end method 

// -----------------------------------------------------------------------------

inline const Bounds & PhysicalWireSegment::getRectangle() const {
	return data->clsRectangle;
} // end method 

// -----------------------------------------------------------------------------

inline const bool PhysicalWireSegment::hasRectangle() const {
	return data->clsHasRectangle;
} // end method 

// -----------------------------------------------------------------------------
} // end namespace 


