/* Copyright 2014-2018 Rsyn
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

#ifndef RSYN_GEOMETRY_POINT_H
#define RSYN_GEOMETRY_POINT_H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/geometries.hpp>

#include "rsyn/util/dbu.h"

namespace Rsyn {

class Point {
public:

	Point() {}
	Point(const DBU x, const DBU y) : clsX(x), clsY(y) {}
	Point(const DBUxy p) : clsX(p.x), clsY(p.y) {}

	DBU getX() const {return clsX;}
	DBU getY() const {return clsY;}

	void setX(const DBU x) {clsX = x;}
	void setY(const DBU y) {clsY = y;}

	operator DBUxy() const {return DBUxy(clsX, clsY);}

private:

	DBU clsX = 0;
	DBU clsY = 0;

}; // end class

} // end namespace

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Rsyn::Point, DBU, 
		boost::geometry::cs::cartesian, getX, getY, setX, setY);

#endif

