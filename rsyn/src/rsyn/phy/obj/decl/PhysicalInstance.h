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
 * Created on 4 de Outubro de 2016, 13:22
 */

#ifndef PHYSICALDESIGN_PHYSICALINSTANCE_H
#define PHYSICALDESIGN_PHYSICALINSTANCE_H

namespace Rsyn {
class PhysicalInstance : public Proxy<PhysicalInstanceData> {
	friend class PhysicalDesign;
protected:
	PhysicalInstance(PhysicalInstanceData * data) : Proxy(data) {}
public:
	PhysicalInstance()  {}
	PhysicalInstance(std::nullptr_t)  {}
	
	Rsyn::Instance getInstance() const;
	DBU getArea() const;
	DBUxy getCenter() const;
	DBU getCenter(const Dimension dim ) const;
	DBU getHeight() const;
	DBU getWidth() const;
	DBU getLength (const Dimension dimension ) const;
	DBUxy getSize() const;
	DBU getX() const;
	DBU getY() const;
	DBUxy getPosition() const;
	DBU getPosition(const Dimension dim) const;
	DBUxy getCoordinate(const Boundary bound) const;
	DBU getCoordinate(const Boundary bound, const Dimension dim) const;
	const Bounds &getBounds() const;
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALINSTANCE_H */

