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

#include "Polygon.h"

#include <cmath>
#include <boost/geometry/geometries/point_xy.hpp>

namespace Rsyn {

Polygon
Polygon::createFromLineString(const std::vector<DBUxy> &points, const DBU width) {
	Polygon rsynPolygon;

	typedef double coordinate_type;
	typedef boost::geometry::model::d2::point_xy<coordinate_type> point;
	typedef boost::geometry::model::polygon<point> polygon;

	// Declare strategies
	const double buffer_distance = width/2.0;
	boost::geometry::strategy::buffer::distance_symmetric<coordinate_type> distance_strategy(buffer_distance);
	boost::geometry::strategy::buffer::join_miter join_strategy;
	boost::geometry::strategy::buffer::end_flat end_strategy;
	boost::geometry::strategy::buffer::point_square point_strategy;
	boost::geometry::strategy::buffer::side_straight side_strategy;

	// Declare output
	boost::geometry::model::multi_polygon<polygon> result;

	// Declare/fill a linestring
	boost::geometry::model::linestring<point> ls;
	for (const DBUxy &p : points)
		boost::geometry::append(ls, point(p.x, p.y));

	// Create the buffer of a linestring
	boost::geometry::buffer(ls, result,
				distance_strategy, side_strategy,
				join_strategy, end_strategy, point_strategy);

	for (const polygon &poly : result) {
		for (const point &p : poly.outer()) {
			rsynPolygon.addPoint(DBUxy(
					static_cast<DBU>(std::round(p.x())),
					static_cast<DBU>(std::round(p.y()))));
		} // end for
		
		// Defensive programming: this operation should not return more than one
		// polygon.
		break;
	} // end for

	return rsynPolygon;
} // end method

} // end namespace