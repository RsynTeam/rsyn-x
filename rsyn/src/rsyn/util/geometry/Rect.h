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

#ifndef RSYN_GEOMETRY_RECT_H
#define RSYN_GEOMETRY_RECT_H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/geometries.hpp>

#include "rsyn/util/dbu.h"
#include "rsyn/util/Bounds.h"

namespace Rsyn {

class Rect {
public:

	Rect() {}
	Rect(const DBU x, const DBU y, const DBU w, const DBU h) : clsX(x), clsY(y), clsWidth(w), clsHeight(h) {}
	Rect(const Bounds &bounds) : clsX(bounds.getX()), clsY(bounds.getY()), clsWidth(bounds.getWidth()), clsHeight(bounds.getHeight()) {}

	DBU getX() const {return clsX;}
	DBU getY() const {return clsY;}
	DBU getWidth() const {return clsWidth;}
	DBU getHeight() const {return clsHeight;}

	DBU getArea() const {return getWidth() * getHeight();}
	
	void setX(const DBU x) {clsX = x;}
	void setY(const DBU y) {clsY = y;}
	void setWidth(const DBU w) {clsWidth = w;}
	void setHeight(const DBU h) {clsHeight = h;}

	operator Bounds() const {return Bounds(clsX, clsY, clsX + clsWidth, clsY + clsHeight);}

private:

	DBU clsX = 0;
	DBU clsY = 0;
	DBU clsWidth = 0;
	DBU clsHeight = 0;

}; // end class

} // end namespace


#endif

