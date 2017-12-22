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
 * File:   PhysicalNetData.h
 * Author: jucemar
 *
 * Created on 15 de Setembro de 2016, 19:15
 */

#ifndef PHYSICALDESIGN_PHYSICALNETDATA_H
#define PHYSICALDESIGN_PHYSICALNETDATA_H

namespace Rsyn {

class PhysicalNetData {
public:
	Bounds clsBounds;
	Rsyn::Pin clsBoundPins[2][2];
	std::vector<Rsyn::PhysicalWire> clsWires;
	PhysicalNetData() {
		clsBoundPins[LOWER][X] = nullptr;
		clsBoundPins[LOWER][Y] = nullptr;
		clsBoundPins[UPPER][X] = nullptr;
		clsBoundPins[UPPER][Y] = nullptr;
	} // end constructor 
}; // end class

} // end class 

#endif /* PHYSICALDESIGN_PHYSICALNETDATA_H */

