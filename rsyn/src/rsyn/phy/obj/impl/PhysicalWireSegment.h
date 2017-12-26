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

namespace Rsyn {

inline DBUxy PhysicalWireSegment::getSourcePosition() const {
	return data->clsSourcePosition;
} // end method

// -----------------------------------------------------------------------------

inline DBUxy PhysicalWireSegment::getTargetPosition() const {
	return data->clsTargetPosition;
} // end method

// -----------------------------------------------------------------------------

inline DBU PhysicalWireSegment::getRoutedWidth() const {
	return data->clsRoutedWidth;
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalLayer PhysicalWireSegment::getLayer() const {
	return data->clsPhysicalLayer;
} // end method 

// -----------------------------------------------------------------------------

inline const std::size_t PhysicalWireSegment::getNumRoutingPoints() const {
	return data->clsRoutingPoints.size();
} // end method 

// -----------------------------------------------------------------------------

inline const std::vector<PhysicalRoutingPoint> & PhysicalWireSegment::allRoutingPoints() const {
	return data->clsRoutingPoints;
} // end method 

// -----------------------------------------------------------------------------
} // end namespace 


