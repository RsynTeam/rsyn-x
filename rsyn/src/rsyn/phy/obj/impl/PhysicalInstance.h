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
 * File:   PhysicalInstance.h
 * Author: jucemar
 *
 * Created on 4 de Outubro de 2016, 19:37
 */


namespace Rsyn {

// -----------------------------------------------------------------------------

inline Rsyn::Instance PhysicalInstance::getInstance() const {
	return data->clsInstance;
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalInstance::getArea() const {
	return data->clsBounds.computeArea();
} // end method 

// -----------------------------------------------------------------------------

inline DBUxy PhysicalInstance::getCenter() const {
	return data->clsBounds.computeCenter();
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalInstance::getCenter(const Dimension dim) const {
	return data->clsBounds.computeCenter(dim);
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalInstance::getHeight() const {
	return data->clsBounds.computeLength(Y);
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalInstance::getWidth() const {
	return data->clsBounds.computeLength(X);
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalInstance::getLength(const Dimension dimension) const {
	return data->clsBounds.computeLength(dimension);
} // end method 

// -----------------------------------------------------------------------------

inline DBUxy PhysicalInstance::getSize() const {
	return DBUxy(getWidth(), getHeight());
} // end method 

// -----------------------------------------------------------------------------

inline DBUxy PhysicalInstance::getPosition() const {
	return data->clsBounds[LOWER];
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalInstance::getPosition(const Dimension dim) const {
	return data->clsBounds[LOWER][dim];
} // end method 

// -----------------------------------------------------------------------------

inline DBUxy PhysicalInstance::getCoordinate(const Boundary bound) const {
	return data->clsBounds[bound];
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalInstance::getCoordinate(const Boundary bound, const Dimension dim) const {
	return data->clsBounds[bound][dim];
} // end method 

// -----------------------------------------------------------------------------

inline const Bounds &PhysicalInstance::getBounds() const {
	return data->clsBounds;
} // end method 

} // end namespace 
