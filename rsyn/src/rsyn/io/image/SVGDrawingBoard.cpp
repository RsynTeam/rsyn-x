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
 
#include "SVGDrawingBoard.h"

namespace Rsyn {

SVGDrawingBoard::~SVGDrawingBoard() {
	for (Shape *shape : clsShapes)
		delete shape;
} // end destructor

// -----------------------------------------------------------------------------

void
SVGDrawingBoard::render(
	const int width,
	const int height,
	const float viewBoxX,
	const float viewBoxY,
	const float viewBoxWidth,
	const float viewBoxHeight,
	std::ostream &out
) {
	out << "<svg "<<
			"width='" << width << "' " <<
			"height='" << height << "' " <<
			"viewBox='" << viewBoxX << " " << viewBoxY << " " <<
			viewBoxWidth << " " << viewBoxHeight << "'>\n";
	for (Shape *shape : clsShapes)
		shape->render(out);
	out << "</svg>\n";
} // end method

// -----------------------------------------------------------------------------

void
SVGDrawingBoard::render(
	const int width,
	const int height,
	std::ostream &out
) {
	render(width, height, 0, 0, width, height, out);
} // end method

// -----------------------------------------------------------------------------

void
SVGDrawingBoard::drawLine(
	const float x1,
	const float y1,
	const float x2,
	const float y2
) {
	Line *line = new Line();
	line->x1 = x1;
	line->y1 = y1;
	line->x2 = x2;
	line->y2 = y2;
	clsShapes.push_back(line);
} // end method

// -----------------------------------------------------------------------------

void
SVGDrawingBoard::drawCircle(
	const float cx,
	const float cy,
	const float radius
) {
	Circle *circle = new Circle();
	circle->cx = cx;
	circle->cy = cy;
	circle->radius = radius;
	clsShapes.push_back(circle);
} // end method

// -----------------------------------------------------------------------------

void
SVGDrawingBoard::drawRectangle(
	const float x,
	const float y,
	const float width,
	const float height
) {
	Rectangle *rectangle = new Rectangle();
	rectangle->x = x;
	rectangle->y = y;
	rectangle->width = width;
	rectangle->height = height;
	clsShapes.push_back(rectangle);
} // end method



} // end namespace