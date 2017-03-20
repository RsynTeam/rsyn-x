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
 * File:   PhysicalPinLayerData.h
 * Author: jucemar
 *
 * Created on 13 de Setembro de 2016, 21:16
 */

#ifndef PHYSICALDESIGN_PHYSICALPINLAYERDATA_H
#define PHYSICALDESIGN_PHYSICALPINLAYERDATA_H


namespace Rsyn {

class PhysicalPinLayerData : public PhysicalObject {
public:
	std::vector<Bounds> clsBounds;
	std::vector<Polygon> clsPolygons;
	Rsyn::PhysicalLayer clsLibLayer;
	PhysicalPinLayerData() {}
}; // end class 

} // end namespace 
 
// TODO
//#include <boost/geometry.hpp>
//#include <boost/geometry/geometries/point_xy.hpp>
//#include <boost/geometry/geometries/polygon.hpp>
//#include <boost/geometry/core/exterior_ring.hpp>
//namespace bg = boost::geometry;
	// source: http://www.boost.org/doc/libs/1_61_0/libs/geometry/doc/html/geometry/reference/models/model_polygon.html
//	typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
//	typedef bg::model::polygon<point_t> polygon_t;
//	typedef bg::model::ring<point_t> ring_t;
//	polygon_t poly1; 

#endif /* PHYSICALDESIGN_PHYSICALPINLAYERDATA_H */

