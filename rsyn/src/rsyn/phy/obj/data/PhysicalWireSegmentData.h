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
 * File:   PhysicalWirePathSegment.h
 * Author: jucemar
 *
 * Created on 13 de Maio de 2017, 15:55
 */

#ifndef PHYSICALDESIGN_PHYSICALWIRESEGMENTDATA_H
#define PHYSICALDESIGN_PHYSICALWIRESEGMENTDATA_H

namespace Rsyn {

class PhysicalWireSegmentData {
public:
	Rsyn::PhysicalLayer clsPhysicalLayer;
	std::vector<PhysicalRoutingPoint> clsRoutingPoints;
	DBUxy clsSourcePosition;
	DBUxy clsTargetPosition;
	DBU clsRoutedWidth = 0;
	bool clsNew : 1;
	
	PhysicalWireSegmentData() : clsSourcePosition(0, 0), clsTargetPosition(0, 0) {
		clsNew = false;
	} // end constructor 
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALWIRESEGMENTDATA_H */

