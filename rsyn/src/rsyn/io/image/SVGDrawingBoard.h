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
 
#include <iostream>
#include <fstream>
#include <vector>

namespace Rsyn {

class SVGDrawingBoard {
public:

	SVGDrawingBoard() {}
	~SVGDrawingBoard();

	struct Style {
		std::string fill;
		std::string stroke;
		int strokeWidth;
		Style() : strokeWidth(0) {}

		void output(std::ostream &out) {
			if (!fill.empty())
				out << "fill='" << fill << "' ";
			if (!stroke.empty())
				out << "stroke='" << stroke << "' ";
			if (strokeWidth > 0)
				out << "stroke-width='" << strokeWidth << "' ";
		} // end method
	}; // end class

	struct Shape {
		Style style;

		virtual void render(std::ostream &out) = 0;
	}; // end class

	struct Circle : Shape {
		float cx;
		float cy;
		float radius;

		virtual void render(std::ostream &out) {
			// <circle cx="100" cy="100" r="100"/>
			out << "<circle " <<
					"cx='" << cx << "' " <<
					"cy='" << cy << "' " <<
					"r='" << radius << "' ";
			style.output(out);
			out << "/>\n";
		} // end method
	}; // end class

	struct Line : Shape {
		float x1;
		float y1;
		float x2;
		float y2;

		Line() {
			style.strokeWidth = 1;
			style.stroke = "black";
		} // end constructor

		virtual void render(std::ostream &out) {
			// <line x1="20" y1="100" x2="100" y2="20"/>
			out << "<line " <<
					"x1='" << x1 << "' " <<
					"y1='" << y1 << "' " <<
					"x2='" << x2 << "' " <<
					"y2='" << y2 << "' ";
			style.output(out);
			out << "vector-effect='non-scaling-stroke' ";
			out << "/>\n";
		} // end method
	}; // end class

	struct Rectangle : Shape {
		float x;
		float y;
		float width;
		float height;

		virtual void render(std::ostream &out) {
			//  <rect x="10" y="10" width="100" height="100"/>
			out << "<rect " <<
					"x='" << x << "' " <<
					"y='" << y << "' " <<
					"width='" << width << "' " <<
					"height='" << height << "' ";
			style.output(out);
			out << "/>\n";
		} // end method
	}; // end class

	void drawLine(
		const float x1,
		const float y1,
		const float x2,
		const float y2);

	void
	drawRectangle(
		const float x,
		const float y,
		const float width,
		const float height);

	void 
	drawCircle(
		const float cx,
		const float cy,
		const float radius);

	void
	render(
		const int width,
		const int height,
		std::ostream &out);

	void 
	render(
		const int width,
		const int height,
		const float viewBoxX,
		const float viewBoxY,
		const float viewBoxWidth,
		const float viewBoxHeight,
		std::ostream &out);

private:

	std::vector<Shape *> clsShapes;

}; // end class

} // end namespace