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
 
#ifndef RSYN_DRAWING_BOARD_H
#define RSYN_DRAWING_BOARD_H

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>

#include <iostream>

namespace Rsyn {
class DrawingBoard {
public:

	enum DrawingStyle {
		STROKE,
		FILL,
		STROKE_AND_FILL
	};

	struct Rgb {
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;

		Rgb() {clear();}
		Rgb(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b) : r(r), g(g), b(b) {}
		void clear() {r = g = b = 0;}
	}; // end struct

	struct Rgba : Rgb {
		std::uint8_t a;

		Rgba() {clear();}
		Rgba(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) : Rgb(r, g, b), a(a) {}
		void clear() { Rgb::clear(); a = 255;}
		void operator=(const Rgb &rgb) {r = rgb.r; g = rgb.g; b = rgb.b; a = 255;}
	}; // end struct

	struct RasterPoint {
		int x;
		int y;
		RasterPoint() : x(0), y(0) {}
		RasterPoint(const int x, const int y) : x(x), y(y) {}
		bool operator==(const RasterPoint &p) const {return x == p.x && y == p.y;}
		bool operator!=(const RasterPoint &p) const {return x != p.x || y != p.y;}
	}; // end struct

	static const Rgb BLACK;
	static const Rgb WHITE;
	static const Rgb SMOOTH_RED;
	static const Rgb SMOOTH_GREEN;
	static const Rgb SMOOTH_YELLOW;
	static const Rgb SMOOTH_GRAY;
	static const Rgb SMOOTH_BLUE;

	DrawingBoard();

	void test() {
		using namespace boost::gil;
		boost::gil::png_write_view("redsquare.png", const_view(clsImage));
	}

	void setPixel(const int x, const int y, const Rgba &rgba) {
		if (x < 0 || y < 0 || x >= clsImage.width() || y >= clsImage.height())
			return;

		boost::gil::view(clsImage)(x, y) =
				boost::gil::rgba8_pixel_t(rgba.r, rgba.g, rgba.b, rgba.a);
	} // end method

	void setPixelBlend(const int x, const int y, const Rgba &rgba, const float itensity) {
		if (x < 0 || y < 0 || x >= clsImage.width() || y >= clsImage.height())
			return;

		auto &pixels = boost::gil::view(clsImage);

		// Blend colors.
		const auto &background = pixels(x, y);
		const float alpha = itensity * (rgba.a / 255.0f);

		Rgb color;
		color.r = std::min((std::uint8_t) 255u, (std::uint8_t) (alpha*rgba.r + (1-alpha)*background[0]));
		color.g = std::min((std::uint8_t) 255u, (std::uint8_t) (alpha*rgba.g + (1-alpha)*background[1]));
		color.b = std::min((std::uint8_t) 255u, (std::uint8_t) (alpha*rgba.b + (1-alpha)*background[2]));

		// Set pixel color.
		pixels(x, y) =
				boost::gil::rgba8_pixel_t(color.r, color.g, color.b, (std::uint8_t) (255u * alpha));
	} // end method

	void drawLine(int x0, int y0, int x1, int y1);
	void drawLineAntiAlias(int x0, int y0, int x1, int y1);
	void drawCircle(int xm, int ym, int r);
	void drawRectangle(int x, int y, int width, int height);
	void drawCharacter(const int x, const int y, char ch);
	void drawText(const int x, const int y, const std::string &text);

	void drawQuadraticCurve(
		const int x0, const int y0,
		const int x1, const int y1,
		const int x2, const int y2);

	void drawCubicCurve(
		const int x0, const int y0,
		const int x1, const int y1,
		const int x2, const int y2,
		const int x3, const int y3);

	void setDrawingStyle(const DrawingStyle style) { clsDrawingStyle = style; }

	void setAntiAlias(const bool enable) {}

	void setStrokeColor(const Rgb &rgb) { clsStrokeColor = rgb; }
	void setStrokeColor(const Rgba &rgba) { clsStrokeColor = rgba; }
	void setStrokeThickness(const int thickness);
	void setStrokeDashType();

	void setFillColor(const Rgb &rgb) { clsFillColor = rgb; }
	void setFillColor(const Rgba &rgba) { clsFillColor = rgba; }

private:

	enum Direction {
		POSITIVE,
		NEGATIVE,
		NEUTRAL,
		NONE
	};

	void moveTo(const int x, const int y) {clsPath.clear(); clsPath.push_back(RasterPoint(x, y));}
	void lineTo(const int x, const int y) {clsPath.push_back(RasterPoint(x, y));}

	void render();
	void render_createOuline(int x0, int y0, int x1, int y1, const bool skipFirst);
	void render_doFilling();

	Direction getDirection(const int previousY, const int currentY) const {
		if (previousY < currentY)
			return POSITIVE;
		else if (previousY > currentY)
			return NEGATIVE;
		else
			return NEUTRAL;
	} // end if

	boost::gil::rgb8_image_t clsImage;
	DrawingStyle clsDrawingStyle;

	int clsStrokeWidth;
	Rgba clsStrokeColor;
	Rgba clsFillColor;

	// Drawing states.
	std::vector<RasterPoint> clsPath;

	std::vector<std::vector<int>> clsOutline;
	int clsOutlineMinY;
	int clsOutlineMaxY;

	static const std::uint8_t ASCII[95][13];

}; // end class
} // end namespace



#endif /* DRAWINGBOARD_H */

