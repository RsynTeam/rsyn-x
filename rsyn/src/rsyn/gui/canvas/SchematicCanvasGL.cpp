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

#include "SchematicCanvasGL.h"

#include <iostream>
#include <random>
#include <bits/stl_deque.h>

#include "rsyn/gui/3rdparty/nanovg/nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "rsyn/gui/3rdparty/nanovg/nanovg_gl.h"

DEFINE_LOCAL_EVENT_TYPE(myEVT_SCHEMATIC_CELL_SELECTED)

//// -----------------------------------------------------------------------------

SchematicCanvasGL::SchematicCanvasGL(wxWindow* parent) : CanvasGL(parent) {
	vg = nullptr;
	initialized = false;

	timer = new wxTimer(this);
	this->Connect(timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(SchematicCanvasGL::onTimeout));
	timer->Start(16);

	setZoomScaling(1.1f);
	reset();
} // end constructor
//
//// -----------------------------------------------------------------------------
//
SchematicCanvasGL::~SchematicCanvasGL() {
	timer->Stop();
	delete timer;
	nvgDeleteGL2(vg);
} // end destructor

//// -----------------------------------------------------------------------------

void SchematicCanvasGL::reset() {
	resetCamera(0, 0, getViewportWidth(), getViewportHeight());
} // end constructor

//// -----------------------------------------------------------------------------
//
void SchematicCanvasGL::attachSession(Rsyn::Session session) {
	reset();

	clsSession = session;

	Refresh();
} // end method

//////////////////////////////////////////////////////////////////////////////////
//// Events
//////////////////////////////////////////////////////////////////////////////////

void SchematicCanvasGL::onRender(wxPaintEvent& evt) {
	if (!IsShownOnScreen()) return;
	render();
} // end method

//// -----------------------------------------------------------------------------

void SchematicCanvasGL::onTimeout(wxTimerEvent& event) {
	if (!IsShownOnScreen()) return;
	render();
} // end method

//// -----------------------------------------------------------------------------

void SchematicCanvasGL::onMouseMoved(wxMouseEvent& event) {
	// Get mouse in board coordinates.
	mouseX = event.GetX() * getZoom() - (clsOriginalMinX - clsMinX);
	mouseY = event.GetY() * getZoom() - (clsMaxY - clsOriginalMaxY);

	// Propagate event...
	CanvasGL::onMouseMoved(event);
} // end method
//
//////////////////////////////////////////////////////////////////////////////////
//// NanoVG Demo
//////////////////////////////////////////////////////////////////////////////////

void SchematicCanvasGL::render() {
	std::cout << "render" << "\n";
	prepare2DViewport(); // required to setup OpenGL context

	if (!initialized) {
		if (glewInit() != GLEW_OK) {
			std::cout << "ERROR: Could not init glew.\n";
		} // end if			

#ifdef DEMO_MSAA
		vg = nvgCreateGL2(NVG_STENCIL_STROKES);
#else
		vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#endif
		if (vg) {
			nvgCreateFont(vg, "sans", "../project/install/fonts/Roboto-Regular.ttf");
			nvgCreateFont(vg, "sans-bold", "../project/install/fonts/Roboto-Bold.ttf");
		} else {
			std::cout << "ERROR: Could not init NanoVG.\n";
		} // end else

		resetCamera();
		initialized = true;
	} else {
		if (!vg) {
			return;
		} // end if
	} // end else

	const int winWidth = getViewportWidth();
	const int winHeight = getViewportHeight();
	const float pxRatio = (float) winWidth / (float) winHeight;

	nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
	nvgTranslate(vg, (clsOriginalMinX - clsMinX) / getZoom(), (clsMaxY - clsOriginalMaxY) / getZoom());
	nvgScale(vg, 1 / getZoom(), 1 / getZoom());

	// Why do we need this?
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Time
	const float t = 10 * (clock() / float(CLOCKS_PER_SEC));

	renderDemo(mouseX, mouseY, boardWidth, boardHeight, t, false);
	nvgEndFrame(vg);

	glFlush();
	SwapBuffers();
} // end method
//
//// -----------------------------------------------------------------------------

void SchematicCanvasGL::renderDemo(float mx, float my, float width, float height, float t, bool blowup) {
	// Background
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, width, height);
	nvgFillColor(vg, nvgRGB(255, 255, 255));
	nvgFill(vg);

	// Stuff
	drawXnor(width - 500, 100, 80, 70);
	drawNand(width - 400, 100, 80, 70);
	drawAnd(width - 600, 100, 80, 70);
	drawXor(width - 500, 200, 80, 70);
	drawNor(width - 400, 200, 80, 70);
	drawOr(width - 600, 200, 80, 70);
	drawInv(width - 500, 300, 80, 70);
	
	

	nvgSave(vg);
	if (blowup) {
		nvgRotate(vg, sinf(t * 0.3f)*5.0f / 180.0f * NVG_PI);
		nvgScale(vg, 2.0f, 2.0f);
	} // end if

	nvgRestore(vg);
} // end method
//
//// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawNand(float x, float y, float w, float h) {
	NVGpaint bg;

	const float alpha = 0.4f;
	const float r = w * 0.1f;
	const float aw = alpha * w;
	const float ew = 1.07f * w;

	nvgSave(vg);
	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));

	nvgBeginPath(vg);
	nvgMoveTo(vg, x + aw, y + 0);
	nvgLineTo(vg, x + 0, y + 0);
	nvgLineTo(vg, x + 0, y + h);
	nvgLineTo(vg, x + aw, y + h);
	nvgBezierTo(vg, x + ew, y + h, x + ew, y + 0, x + aw, y + 0);
	nvgStroke(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, x + w, y + h / 2, r);
	nvgStroke(vg);

	nvgRestore(vg);
}

void SchematicCanvasGL::drawAnd(float x, float y, float w, float h) {
	NVGpaint bg;

	const float alpha = 0.4f;
	const float r = w * 0.1f;
	const float aw = alpha * w;
	const float ew = 1.07f * w;

	nvgSave(vg);
	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));

	nvgBeginPath(vg);
	nvgMoveTo(vg, x + aw, y + 0);
	nvgLineTo(vg, x + 0, y + 0);
	nvgLineTo(vg, x + 0, y + h);
	nvgLineTo(vg, x + aw, y + h);
	nvgBezierTo(vg, x + ew, y + h, x + ew, y + 0, x + aw, y + 0);
	nvgStroke(vg);

	nvgRestore(vg);
}

void SchematicCanvasGL::drawOr(float x, float y, float w, float h) {
	NVGpaint bg;

	const float alpha = 0.4f;
	const float r = w * 0.1f;
	const float aw = alpha * w;
	const float bw = 0.3f * w;
	const float ew = 0.80f * w;

	nvgSave(vg);
	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));

	nvgBeginPath(vg);
	nvgMoveTo(vg, x + aw, y + 0);
	nvgLineTo(vg, x + 0, y + 0);
	nvgBezierTo(vg, x + bw, y + 0, x + bw, y + h, x + 0, y + h);
	nvgLineTo(vg, x + aw, y + h);
	nvgBezierTo(vg, x + aw, y + h, x + ew, y + h, x + w, y + h/2);
	nvgBezierTo(vg, x + w, y + h/2, x + ew, y + 0, x + aw, y + 0);
	nvgStroke(vg);

	nvgRestore(vg);
} 

void SchematicCanvasGL::drawNor(float x, float y, float w, float h) {
	NVGpaint bg;

	const float alpha = 0.4f;
	const float r = w * 0.1f;
	const float aw = alpha * w;
	const float bw = 0.3f * w;
	const float ew = 0.70f * w;
	const float ow = 0.89f * w;

	nvgSave(vg);
	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));

	nvgBeginPath(vg);
	nvgMoveTo(vg, x + aw, y + 0);
	nvgLineTo(vg, x + 0, y + 0);
	nvgBezierTo(vg, x + bw, y + 0, x + bw, y + h, x + 0, y + h);
	nvgLineTo(vg, x + aw, y + h);
	nvgBezierTo(vg, x + aw, y + h, x + ew, y + h, x + ow, y + h/2);
	nvgBezierTo(vg, x + ow, y + h/2, x + ew, y + 0, x + aw, y + 0);
	nvgStroke(vg);
	
	nvgBeginPath(vg);
	nvgCircle(vg, x + w, y + h / 2, r);
	nvgStroke(vg);

	nvgRestore(vg);
} 

void SchematicCanvasGL::drawInv(float x, float y, float w, float h) {
	NVGpaint bg;

	const float alpha = 0.4f;
	const float r = w * 0.1f;
	const float aw = alpha * w;
	const float bw = 0.3f * w;
	const float ew = 0.70f * w;
	const float ow = 0.89f * w;

	nvgSave(vg);
	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));

	nvgBeginPath(vg);
	nvgMoveTo(vg, x + ow, y + h / 2);
	nvgLineTo(vg, x + 0, y + 0);
	nvgLineTo(vg, x + 0, y + h);
	nvgLineTo(vg, x + ow, y + h / 2);
	nvgStroke(vg);
	
	nvgBeginPath(vg);
	nvgCircle(vg, x + w, y + h / 2, r);
	nvgStroke(vg);

	nvgRestore(vg);
} 

void SchematicCanvasGL::drawXnor(float x, float y, float w, float h) {
	NVGpaint bg;
	const float alpha = 0.4f;
	const float r = w * 0.1f;
	const float aw = alpha * w;
	const float bw = 0.3f * w;
	const float ew = 0.80f * w;
	const float ow = 0.09f * w;
	const float xw = 0.2f * w;
	const float cw = 0.01f * h;
	const float dw = 0.99f * h;
	const float fw = 0.90f * w;

	nvgSave(vg);
	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));

	nvgBeginPath(vg);
	nvgMoveTo(vg, x + aw, y + 0);
	nvgLineTo(vg, x + ow, y + 0);
	nvgBezierTo(vg, x + bw, y + 0, x + bw, y + h, x + ow, y + h);
	nvgLineTo(vg, x + aw, y + h);
	nvgBezierTo(vg, x + aw, y + h, x + ew, y + h, x + fw, y + h/2);
	nvgBezierTo(vg, x + fw, y + h/2, x + ew, y + 0, x + aw, y + 0);
	nvgStroke(vg);
	
	nvgBeginPath(vg);
	nvgMoveTo(vg, x + 0, y + cw);
	nvgBezierTo(vg, x + xw, y + cw, x + xw, y + dw, x + 0, y + dw);
	nvgStroke(vg);
	
	nvgBeginPath(vg);
	nvgCircle(vg, x + w, y + h / 2, r);
	nvgStroke(vg);

	nvgRestore(vg);
} 

void SchematicCanvasGL::drawXor(float x, float y, float w, float h) {
	NVGpaint bg;
	const float alpha = 0.4f;
	const float aw = alpha * w;
	const float bw = 0.3f * w;
	const float ew = 0.80f * w;
	const float ow = 0.09f * w;
	const float xw = 0.2f * w;
	const float cw = 0.01f * h;
	const float dw = 0.99f * h;

	nvgSave(vg);
	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));

	nvgBeginPath(vg);
	nvgMoveTo(vg, x + aw, y + 0);
	nvgLineTo(vg, x + ow, y + 0);
	nvgBezierTo(vg, x + bw, y + 0, x + bw, y + h, x + ow, y + h);
	nvgLineTo(vg, x + aw, y + h);
	nvgBezierTo(vg, x + aw, y + h, x + ew, y + h, x + w, y + h/2);
	nvgBezierTo(vg, x + w, y + h/2, x + ew, y + 0, x + aw, y + 0);
	nvgStroke(vg);
	
	nvgBeginPath(vg);
	nvgMoveTo(vg, x + 0, y + cw);
	nvgBezierTo(vg, x + xw, y + cw, x + xw, y + dw, x + 0, y + dw);
	nvgStroke(vg);

	nvgRestore(vg);
}

// end method
//
//
//void SchematicCanvasGL::drawGraph(float x, float y, float w, float h, float t) {
//	NVGpaint bg;
//	float samples[6];
//	float sx[6], sy[6];
//	float dx = w / 5.0f;
//	int i;
//
//	samples[0] = (1 + sinf(t * 1.2345f + cosf(t * 0.33457f)*0.44f))*0.5f;
//	samples[1] = (1 + sinf(t * 0.68363f + cosf(t * 1.3f)*1.55f))*0.5f;
//	samples[2] = (1 + sinf(t * 1.1642f + cosf(t * 0.33457f)*1.24f))*0.5f;
//	samples[3] = (1 + sinf(t * 0.56345f + cosf(t * 1.63f)*0.14f))*0.5f;
//	samples[4] = (1 + sinf(t * 1.6245f + cosf(t * 0.254f)*0.3f))*0.5f;
//	samples[5] = (1 + sinf(t * 0.345f + cosf(t * 0.03f)*0.6f))*0.5f;
//
//	for (i = 0; i < 6; i++) {
//		sx[i] = x + i*dx;
//		sy[i] = y + h * samples[i]*0.8f;
//	}
//
//	// Graph background
//	bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(0, 160, 192, 0), nvgRGBA(0, 160, 192, 64));
//	nvgBeginPath(vg);
//	nvgMoveTo(vg, sx[0], sy[0]);
//	for (i = 1; i < 6; i++)
//		nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
//	nvgLineTo(vg, x + w, y + h);
//	nvgLineTo(vg, x, y + h);
//	nvgFillPaint(vg, bg);
//	nvgFill(vg);
//
//	// Graph line
//	nvgBeginPath(vg);
//	nvgMoveTo(vg, sx[0], sy[0] + 2);
//	for (i = 1; i < 6; i++)
//		nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1] + 2, sx[i] - dx * 0.5f, sy[i] + 2, sx[i], sy[i] + 2);
//	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
//	nvgStrokeWidth(vg, 3.0f);
//	nvgStroke(vg);
//
//	nvgBeginPath(vg);
//	nvgMoveTo(vg, sx[0], sy[0]);
//	for (i = 1; i < 6; i++)
//		nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
//	nvgStrokeColor(vg, nvgRGBA(0, 160, 192, 255));
//	nvgStrokeWidth(vg, 3.0f);
//	nvgStroke(vg);
//
//	// Graph sample pos
//	for (i = 0; i < 6; i++) {
//		bg = nvgRadialGradient(vg, sx[i], sy[i] + 2, 3.0f, 8.0f, nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 0));
//		nvgBeginPath(vg);
//		nvgRect(vg, sx[i] - 10, sy[i] - 10 + 2, 20, 20);
//		nvgFillPaint(vg, bg);
//		nvgFill(vg);
//	}
//
//	nvgBeginPath(vg);
//	for (i = 0; i < 6; i++)
//		nvgCircle(vg, sx[i], sy[i], 4.0f);
//	nvgFillColor(vg, nvgRGBA(0, 160, 192, 255));
//	nvgFill(vg);
//	nvgBeginPath(vg);
//	for (i = 0; i < 6; i++)
//		nvgCircle(vg, sx[i], sy[i], 2.0f);
//	nvgFillColor(vg, nvgRGBA(220, 220, 220, 255));//void NewSchematicCanvasGL::renderExperimental() {
//	const float x = 5;
//	const float y = 5;
//	const float w = 4;
//	const float h = 4;
//
//	const float alpha = 0.4f;
//	const float r = w * 0.15f;
//	const float aw = alpha * w;
//	const float ew = 1.07f * w;
//
//	PathScribble path;
//	path.moveTo(x + aw, y + 0);
//	path.lineTo(x + 0, y + 0);
//	path.lineTo(x + 0, y + h);
//	path.lineTo(x + aw, y + h);
//	path.cubicCurveTo(x + ew, y + h, x + ew, y + 0, x + aw, y + 0);
//
//	CircleScribble circle;
//	circle.set(x + w + r / 2 - 0.1f, y + h / 2, r);
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glColor3ub(255, 255, 255);
//	path.render(0.1f, LAYER_SHAPE_FILLING);
//	circle.render(0.1f, LAYER_SHAPE_FILLING);
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glColor3ub(0, 0, 0);
//	path.render(0.1f, LAYER_SHAPES);
//	circle.render(0.1f, LAYER_SHAPES);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//} // end method
//	nvgFill(vg);
////void NewSchematicCanvasGL::renderExperimental() {
//	const float x = 5;
//	const float y = 5;
//	const float w = 4;
//	const float h = 4;
//
//	const float alpha = 0.4f;
//	const float r = w * 0.15f;
//	const float aw = alpha * w;
//	const float ew = 1.07f * w;
//
//	PathScribble path;
//	path.moveTo(x + aw, y + 0);
//	path.lineTo(x + 0, y + 0);
//	path.lineTo(x + 0, y + h);
//	path.lineTo(x + aw, y + h);
//	path.cubicCurveTo(x + ew, y + h, x + ew, y + 0, x + aw, y + 0);
//
//	CircleScribble circle;
//	circle.set(x + w + r / 2 - 0.1f, y + h / 2, r);
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glColor3ub(255, 255, 255);
//	path.render(0.1f, LAYER_SHAPE_FILLING);
//	circle.render(0.1f, LAYER_SHAPE_FILLING);
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glColor3ub(0, 0, 0);
//	path.render(0.1f, LAYER_SHAPES);
//	circle.render(0.1f, LAYER_SHAPES);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//} // end method
//	nvgStrokeWidth(vg, 1.0f);
//} // end method
//
//////////////////////////////////////////////////////////////////////////////////
//// Drawing (Experimental)
//////////////////////////////////////////////////////////////////////////////////
//
//enum ShapeType {
//	SHAPE_TYPE_LINE,
//	SHAPE_TYPE_RECTANGLE,
//	SHAPE_TYPE_CIRCLE,
//	SHAPE_TYPE_PATH
//};
//
//class Scribble {
//public:
//	virtual void render(const float ratio, const float z) const = 0;
//}; // end class
//
//class CircleScribble : public Scribble {
//private:
//	float xc;
//	float yc;
//	float r;
//
//public:
//
//	void set(const float x, const float y, const float radius) {
//		xc = x;
//		yc = y;
//		r = radius;
//	} // end method
//
//	virtual void render(const float ratio, const float z) const override {
//		const int numSamples = (int) std::max(10.0f, 10.0f / ratio);
//		const float degreePerStep = (2.0f * 3.14159265359f) / numSamples;
//
//		glBegin(GL_POLYGON);
//		for (int i = 0; i < numSamples; i++) {
//			const float theta = i * degreePerStep;
//			const float x = xc + (r * std::cos(theta));
//			const float y = yc + (r * std::sin(theta));
//			glVertex3f(x, y, z);
//		} // end for
//		glEnd();
//	} // end method
//}; // end class
//
//class PathScribble : public Scribble {
//private:
//
//	enum PathType {
//		LINE,
//		QUADRATIC,
//		CUBIC
//	};
//
//	std::vector<float> points;
//	std::vector<PathType> cmds;
//
//	void
//	renderLineTo(
//		const float x, const float y, const float z
//		) const {
//		glVertex3f(x, y, z);
//	} // end method
//
//	void
//	renderQuadraticCurveTo(
//		const float x0, const float y0,
//		const float x1, const float y1,
//		const float x2, const float y2,
//		const float z
//		) const {
//		const int N = 25;
//
//		for (int i = 1; i <= N; i++) { // first point was already processed
//			const float t1 = float(i) / float(N);
//			const float t2 = t1*t1;
//			const float a = 1 - t1; // (1-t)^1
//			const float b = a*a; // (1-t)^2
//
//			const float xt = b * x0 + 2 * a * t1 * x1 + t2*x2;
//			const float yt = b * y0 + 2 * a * t1 * y1 + t2*y2;
//
//			glVertex3f(xt, yt, z);
//		} // end for
//	} // end for
//
//	void
//	renderCubicCurveTo(
//		const float x0, const float y0,
//		const float x1, const float y1,
//		const float x2, const float y2,
//		const float x3, const float y3,
//		const float z
//		) const {
//		const int N = 25;
//
//		for (int i = 1; i <= N; i++) { // first point was already processed
//			const float t1 = float(i) / float(N);
//			const float t2 = t1*t1;
//			const float t3 = t2*t1;
//			const float a = 1 - t1; // (1-t)^1
//			const float b = a*a; // (1-t)^2
//			const float c = b*a; // (1-t)^3
//
//			const float xt = c * x0 + 3 * b * t1 * x1 + 3 * a * t2 * x2 + t3*x3;
//			const float yt = c * y0 + 3 * b * t1 * y1 + 3 * a * t2 * y2 + t3*y3;
//
//			glVertex3f(xt, yt, z);
//		} // end for
//	} // end method
//
//	void
//	processLine(
//		const float z,
//		int &head
//		) const {
//		const float x = points[head++];
//		const float y = points[head++];
//		renderLineTo(x, y, z);
//	} // end method
//
//	void
//	processQuadraticCurve(
//		const float z,
//		int &head
//		) const {
//		const float x0 = points[head - 2];
//		const float y0 = points[head - 1];
//		const float x1 = points[head++];
//		const float y1 = points[head++];
//		const float x2 = points[head++];
//		const float y2 = points[head++];
//		renderQuadraticCurveTo(x0, y0, x1, y1, x2, y2, z);
//	} // end method
//
//	void
//	processCubicCurve(
//		const float z,
//		int &head
//		) const {
//		const float x0 = points[head - 2];
//		const float y0 = points[head - 1];
//		const float x1 = points[head++];
//		const float y1 = points[head++];
//		const float x2 = points[head++];
//		const float y2 = points[head++];
//		const float x3 = points[head++];
//		const float y3 = points[head++];
//		renderCubicCurveTo(x0, y0, x1, y1, x2, y2, x3, y3, z);
//	} // end method
//
//public:
//
//	virtual void render(
//		const float ratio, const float z
//		) const override {
//		if (cmds.empty())
//			return;
//
//		glBegin(GL_POLYGON);
//		glVertex3f(points[0], points[1], z);
//		int head = 2;
//		for (PathType type : cmds) {
//			switch (type) {
//				case LINE:
//					processLine(z, head);
//					break;
//				case QUADRATIC:
//					processQuadraticCurve(z, head);
//					break;
//				case CUBIC:
//					processCubicCurve(z, head);
//					break;
//				default:
//					assert(false);
//			} // end switch
//		} // end for
//
//		glEnd();
//	} // end method
//
//	void moveTo(
//		const float x, const float y
//		) {
//		points.clear();
//		points.push_back(x);
//		points.push_back(y);
//	} // end method
//
//	void lineTo(
//		const float x, const float y
//		) {
//		cmds.push_back(LINE);
//		points.push_back(x);
//		points.push_back(y);
//	} // end method
//
//	void quadraticCurveTo(
//		const float x1, const float y1,
//		const float x2, const float y2
//		) {
//		cmds.push_back(QUADRATIC);
//		points.push_back(x1);
//		points.push_back(y1);
//		points.push_back(x2);
//		points.push_back(y2);
//	} // end method
//
//	void cubicCurveTo(
//		const float x1, const float y1,
//		const float x2, const float y2,
//		const float x3, const float y3
//		) {
//		cmds.push_back(CUBIC);
//		points.push_back(x1);
//		points.push_back(y1);
//		points.push_back(x2);
//		points.push_back(y2);
//		points.push_back(x3);
//		points.push_back(y3);
//	} // end method
//
//}; // end class
//
//// -----------------------------------------------------------------------------
//
//void NewSchematicCanvasGL::renderExperimental() {
//	const float x = 5;
//	const float y = 5;
//	const float w = 4;
//	const float h = 4;
//
//	const float alpha = 0.4f;
//	const float r = w * 0.15f;
//	const float aw = alpha * w;
//	const float ew = 1.07f * w;
//
//	PathScribble path;
//	path.moveTo(x + aw, y + 0);
//	path.lineTo(x + 0, y + 0);
//	path.lineTo(x + 0, y + h);
//	path.lineTo(x + aw, y + h);
//	path.cubicCurveTo(x + ew, y + h, x + ew, y + 0, x + aw, y + 0);
//
//	CircleScribble circle;
//	circle.set(x + w + r / 2 - 0.1f, y + h / 2, r);
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glColor3ub(255, 255, 255);
//	path.render(0.1f, LAYER_SHAPE_FILLING);
//	circle.render(0.1f, LAYER_SHAPE_FILLING);
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glColor3ub(0, 0, 0);
//	path.render(0.1f, LAYER_SHAPES);
//	circle.render(0.1f, LAYER_SHAPES);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//} // end method

// #############################################################################
// New Schematic
// #############################################################################

// -----------------------------------------------------------------------------

const float NewSchematicCanvasGL::LAYER_GRID = 0.0f;
const float NewSchematicCanvasGL::LAYER_SHAPE_FILLING = 0.05f;
const float NewSchematicCanvasGL::LAYER_SHAPES = 0.1f;
const float NewSchematicCanvasGL::LAYER_SELECTED = 0.4f;
const DBU NewSchematicCanvasGL::CELL_SIZE = 100;
const DBU NewSchematicCanvasGL::SPACE_SIZE = 50;
const DBU NewSchematicCanvasGL::LVL_SIZE = CELL_SIZE + 2 * SPACE_SIZE;
const DBU NewSchematicCanvasGL::ROW_SIZE = CELL_SIZE + 2 * SPACE_SIZE;

// -----------------------------------------------------------------------------

NewSchematicCanvasGL::NewSchematicCanvasGL(wxWindow* parent) : CanvasGL(parent) {
	resetClickedView();

} // end constructor

// -----------------------------------------------------------------------------

NewSchematicCanvasGL::~NewSchematicCanvasGL() {

} // end destructor

void NewSchematicCanvasGL::resetClickedView() {
	clsDrawCriticalPath = false;
	clsDrawSelectedCell = false;
	clsDrawNeighborCells = false;
	clsDrawLogicCone = false;
	clsIsSelected = false;
	clsFirstRendering = true;
	clsDrawCPChanged = false;
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::setViewCriticalPaths(const bool visible) {
	clsDrawCriticalPath = visible;
	if (clsDrawCPChanged != clsDrawCriticalPath) {
		if (clsDrawCriticalPath) {
			if (timer)
				init();
			else
				std::cout << "****WARNING**** No timer";
		}
		clsDrawCPChanged = clsDrawCriticalPath;
	}
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::setViewSelectedCell(const bool visible) {
	clsDrawSelectedCell = visible;
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::setViewNighborCells(const bool visible) {
	clsDrawNeighborCells = visible;
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::setViewLogicCone(const bool visible) {
	clsDrawLogicCone = visible;
	//	Rsyn::Instance instance = selectedInstance.getInst();
	//	if (clsDrawLogicCone && instance) {
	//		for (Rsyn::Pin pin : instance.allPins(Rsyn::OUT))
	//			getLogicCone(selectedInstance, pin);
	//		std::cout << "end" << "\n";
	//	}
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::setNumPathsToDraw(const int numPaths) {
	clsNumPathsToDraw = numPaths;
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::renderGrid(const int scaleGrid) {
	const float defaultIntensity = 0.9f;
	if (getZoom() >= 2.5) {
		return;
	} // end if

	const float xmin = getMinX();
	const float ymin = getMinY();
	const float xmax = getMaxX();
	const float ymax = getMaxY();

	// Draw horizontal lines
	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(defaultIntensity, defaultIntensity, defaultIntensity);

	for (int y = 0; y <= ymax / scaleGrid; y++) {
		glVertex3f(xmin, y*scaleGrid, LAYER_GRID);
		glVertex3f(xmax, y*scaleGrid, LAYER_GRID);
	} // end for

	for (int y = 0; y >= ymin / scaleGrid; y--) {
		glVertex3f(xmin, y*scaleGrid, LAYER_GRID);
		glVertex3f(xmax, y*scaleGrid, LAYER_GRID);
	} // end for

	for (int x = 0; x <= xmax / scaleGrid; x++) {
		glVertex3f(x*scaleGrid, ymin, LAYER_GRID);
		glVertex3f(x*scaleGrid, ymax, LAYER_GRID);
	} // end for

	for (int x = 0; x >= xmin / scaleGrid; x--) {
		glVertex3f(x*scaleGrid, ymin, LAYER_GRID);
		glVertex3f(x*scaleGrid, ymax, LAYER_GRID);
	} // end for
	glEnd();
	glLineWidth(1);
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::attachSession(Rsyn::Session session) {
	clsSession = session;
	timer = clsSession.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);
	clsDesign = clsSession.getDesign();
	if (timer)
		init();
} // end method 


// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::init() {
	clsVisualInst = clsDesign.createAttribute();
	clsVisualInstPin = clsDesign.createAttribute();
	instancesOnScreen.clear();
	clsIsSelected = false;
	getCriticalPath();

} // end method
// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::onRender(wxPaintEvent& evt) {

	if (!IsShownOnScreen()) return;
	prepare2DViewport(); // required to setup OpenGL context

	if (clsFirstRendering) {
		// I'm no sure why resetting in the constructor did not work.
		resetCamera(0, 0, finalPos[X], finalPos[Y]);
		clsFirstRendering = 0;
	} // end if

	renderGrid();
	//renderExperimental();
	drawInstancesOnScreen();
	drawConnectionsOnScreen();
	selectCellAt();
	glFlush();
	SwapBuffers();
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::getCriticalPath() {
	int numberPaths = timer->queryTopCriticalPathFromTopCriticalEndpoints(Rsyn::LATE, clsNumPathsToDraw, clsCriticalPaths, 0);
	if (numberPaths > 0) {
		overlap.clear();
		defineCriticalPathPos();
		if (clsDrawCriticalPath || clsFirstRendering) {
			resetCamera(0, 0, finalPos[X], finalPos[Y]);
			Refresh();
		} //end if
	} // end if 
} // end method 

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::defineInstancePos(Rsyn::Instance instance, int row, DBU & lastPos, int level) {

	DBUxy posMin;
	DBUxy posMax;
	VisualInstance & cdInstance = clsVisualInst[instance];
	cdInstance.setInst(instance);
	posMin[X] = lastPos + SPACE_SIZE;
	posMax[X] = posMin[X] + CELL_SIZE;

	switch (level) {
		case NEXT_LEVEL:
		{
			int index = lastPos / LVL_SIZE + 1;
			insertCell(index, row, cdInstance);
			break;
		} // end case 
		case SAME_LEVEL:
		{
			posMin[Y] = row * ROW_SIZE + SPACE_SIZE;
			posMax[Y] = posMin[Y] + CELL_SIZE;
			cdInstance.setBounds(posMin[X], posMin[Y], posMax[X], posMax[Y]);
			break;
		} // end case 
	} // end switch

	definePinPos(cdInstance);

	if (instance.isSequential()) {
		cdInstance.setColor(255, 0, 255);
	} else if (instance.isLCB()) {
		cdInstance.setColor(0, 255, 0);
	} else if (instance.getType() == Rsyn::PORT) {
		cdInstance.setColor(255, 0, 0);
	} else if (instance.isFixed()) {
		cdInstance.setColor(0, 0, 0);
	} else {
		cdInstance.setColor(0, 0, 255);
	}

	posMax[X] += SPACE_SIZE;
	lastPos = posMax[X];

	instancesOnScreen.push_back(instance);

} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::definePinPos(VisualInstance & cdInstance) {
	Rsyn::Instance inst = cdInstance.getInst();
	Bounds instBounds = cdInstance.getBounds();
	int inPinsNum = inst.getNumInputPins();
	int outPinsNum = inst.getNumOutputPins();
	DBU pinSpaceIn;
	DBU pinSpaceOut;
	DBU inPos;
	DBU outPos;

	if (inPinsNum > 0) {
		pinSpaceIn = CELL_SIZE / inPinsNum;
		inPos = pinSpaceIn / 2;
	}

	if (outPinsNum > 0) {
		pinSpaceOut = CELL_SIZE / outPinsNum;
		outPos = pinSpaceOut / 2;
	}

	for (Rsyn::Pin pin : inst.allPins()) {
		VisualInstancePin & cdPin = clsVisualInstPin[pin];
		if (pin.getDirection() == Rsyn::IN) {
			cdPin.pin = pin;
			cdPin.pos[X] = instBounds[LOWER][X];
			cdPin.pos[Y] = instBounds[LOWER][Y] + inPos;
			inPos += pinSpaceIn;
		} else {
			cdPin.pin = pin;
			cdPin.pos[X] = instBounds[UPPER][X];
			cdPin.pos[Y] = instBounds[LOWER][Y] + outPos;
			outPos += pinSpaceOut;
		}
		cdInstance.addPin(cdPin);
	}

}
// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::insertCell(int index, int row, VisualInstance & cdInstance) {
	bool inserted = false;
	int overlapSize = overlap.size();
	int countRow = 1;
	int rowSize;
	DBUxy posMin;
	DBUxy posMax;
	std::vector<VisualInstance*> newRow;


	while (!inserted) {

		if (countRow % 2 == 0) {
			row -= countRow;
		} else {
			row += countRow;
		} // end if-else 

		if (row >= 0) {
			if (overlapSize > row) {
				std::vector<VisualInstance*> & singleRow = overlap.at(row);
				rowSize = singleRow.size();
				if (rowSize > index) {
					if (singleRow.at(index) == NULL) {
						singleRow.at(index) = &cdInstance;
						inserted = true;
					} // end if 
				} else {
					for (int i = rowSize; i < index; i++) {
						singleRow.push_back(NULL);
					} // end for 
					singleRow.push_back(&cdInstance);
					inserted = true;
				} // end else 
			} else {
				for (int i = 0; i < index; i++) {
					newRow.push_back(NULL);
				} // end for 
				newRow.push_back(&cdInstance);
				inserted = true;
				overlap.push_back(newRow);
			} // end else
		} // end if
		countRow++;
	} // end while


	posMin[X] = index * LVL_SIZE + SPACE_SIZE;
	posMax[X] = posMin[X] + CELL_SIZE;
	posMin[Y] = row * ROW_SIZE + SPACE_SIZE;
	posMax[Y] = posMin[Y] + CELL_SIZE;

	cdInstance.setBounds(posMin[X], posMin[Y], posMax[X], posMax[Y]);

} // end method
// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::defineCriticalPathPos() {
	DBU lastPos = 0;
	DBU finalX = 0;
	int rowNumber;
	int middleRow;
	Rsyn::Instance instance;
	int count = 0;
	Bounds lastBounds;
	Rsyn::Instance prevInstance;
	Rsyn::Pin pin;
	Rsyn::Pin prevPin;
	std::vector<VisualInstance*> row;


	if (clsCriticalPaths.size() % 2 == 0) {
		rowNumber = (clsCriticalPaths.size() - 2) / 2;
	} else {
		middleRow = (clsCriticalPaths.size() - 1) / 2;
		rowNumber = middleRow;
	} // end if-else 

	for (std::vector<Rsyn::Timer::PathHop> & path : clsCriticalPaths) {
		for (Rsyn::Timer::PathHop & hop : path) {
			instance = hop.getInstance();
			pin = hop.getPin();
			VisualInstance & cdInstance = clsVisualInst[instance];
			if (prevInstance != instance) {
				if (!cdInstance.getInst()) {
					defineInstancePos(instance, rowNumber, lastPos, SAME_LEVEL);
					row.push_back(&cdInstance);
				} else {
					lastBounds = cdInstance.getBounds();
					lastPos = lastBounds[UPPER][X] + SPACE_SIZE;
					row.push_back(NULL);
				} // end else
				VisualInstancePin & cdPin = clsVisualInstPin[pin];
				if (prevPin){
					VisualInstancePin & cdPrevPin = clsVisualInstPin[prevPin];
					makeConnectionsCP(cdPrevPin, cdPin);
				}
				prevPin = pin;
				prevInstance = instance;
			} // end if 
		} // end for 
		prevInstance = nullptr;
		count++;
		if (finalX < lastPos)
			finalX = lastPos;

		if (count % 2 == 0) {
			overlap.push_back(row);
			rowNumber -= count;
		} else {
			overlap.push_front(row);
			rowNumber += count;
		} // end else 

		row.clear();
	} // end for

	finalPos[X] = finalX + SPACE_SIZE;
	finalPos[Y] = overlap.size() * ROW_SIZE + SPACE_SIZE;

}// end method


// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::drawInstancesOnScreen() {

	for (Rsyn::Instance inst : instancesOnScreen) {
		drawInstance(inst);
	} // end for 

} // end method 
// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::drawInstance(Rsyn::Instance instance) {

	VisualInstance & inst = clsVisualInst[instance];
	Bounds bounds = inst.getBounds();
	DBUxy posLower;
	DBUxy posUpper;
	posLower[X] = bounds[LOWER][X];
	posLower[Y] = bounds[LOWER][Y];
	posUpper[X] = bounds[UPPER][X];
	posUpper[Y] = bounds[UPPER][Y];

	Color color = inst.getColor();

	glColor3ub(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	glVertex3f(posLower[X], posLower[Y], LAYER_SHAPES);
	glVertex3f(posUpper[X], posLower[Y], LAYER_SHAPES);
	glVertex3f(posUpper[X], posUpper[Y], LAYER_SHAPES);
	glVertex3f(posLower[X], posUpper[Y], LAYER_SHAPES);
	glEnd();

} // end method 

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::drawPin(VisualInstance & inst) {
	DBU halfX = 3;
	DBU halfY = 3;
	for (VisualInstancePin pin : inst.getPins()) {
		glPointSize(10);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_QUADS);
		glVertex3f(pin.pos[X] - halfX, pin.pos[Y] - halfY, LAYER_SHAPES);
		glVertex3f(pin.pos[X] + halfX, pin.pos[Y] - halfY, LAYER_SHAPES);
		glVertex3f(pin.pos[X] + halfX, pin.pos[Y] + halfY, LAYER_SHAPES);
		glVertex3f(pin.pos[X] - halfX, pin.pos[Y] + halfY, LAYER_SHAPES);
		glEnd();
	} // end for 

} // end method 

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::makeConnectionsCP(VisualInstancePin & pinOut, VisualInstancePin & pinIn) {
	DBUxy posOut = pinOut.pos;
	DBUxy posIn = pinIn.pos;
	netsOnScreen.push_back(VisualNet(posOut, posIn));

} // end method 

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::drawConnections(DBUxy posOut, DBUxy posIn) {

	glLineWidth(2);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(posOut[X], posOut[Y], LAYER_SHAPES);
	glVertex3f(posIn[X], posIn[Y], LAYER_SHAPES);
	glEnd();

}

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::drawConnectionsOnScreen() {

	for (VisualNet net : netsOnScreen) {
		drawConnections(net.getPOut(), net.getPIn());
	} // end for 

} // end method 

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::getNeighbours() {
	std::vector<Rsyn::Pin> neighbours;
	Rsyn::Instance instance = selectedInstance.getInst();
	if (instance) {
		for (Rsyn::Pin pin : instance.allPins(Rsyn::OUT)) {
			defineNeighboursPos(selectedInstance, pin, neighbours);
		} // end for 
	} // end if 
}// end method 
// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::getLogicCone(VisualInstance selectedInst, Rsyn::Pin selectedPin) {
	std::vector<Rsyn::Pin> neighbours;
	Rsyn::Instance instance;
	defineNeighboursPos(selectedInst, selectedPin, neighbours);

	std::cout << "cell: " << selectedInst.getInst().getName() << "\n";

	for (Rsyn::Pin pin : neighbours) {
		instance = pin.getInstance();
		if (instance == nullptr)
			std::cout << "obj is null ptr " << pin.getFullName() << std::endl;
		std::cout << "inst: " << instance.getName() << "\n";
		if (!instance.isSequential() || !instance.isMacroBlock()) {
			VisualInstance & inst = clsVisualInst[instance];
			std::cout << "isnt visual: " << inst.getInst().getName() << "\n";
			getLogicCone(inst, pin);
		} //end if
	} //end for                      

} //end method
// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::defineNeighboursPos(const VisualInstance & selectedInst, Rsyn::Pin selectedPin, std::vector<Rsyn::Pin> & neighbours) {
	const Bounds & bounds = selectedInst.getBounds();
	DBUxy pos;
	pos[X] = bounds[UPPER][X];
	pos[Y] = bounds[LOWER][Y];
	Rsyn::Net net = selectedPin.getNet();
	DBU finalX = finalPos[X];
	int overlapSizeBf;
	int overlapSizeAf;
	bool changeYSize = false;
	bool changeXSize = false;
	if (net != nullptr) {
		for (Rsyn::Pin pinIn : net.allPins(Rsyn::IN)) {
			Rsyn::Instance instance = pinIn.getInstance();
			VisualInstance & instVisual = clsVisualInst[instance];
			if (!instVisual.getInst()) {
				int row = pos[Y] / ROW_SIZE;
				DBU lastPos = pos[X];
				overlapSizeBf = overlap.size();
				defineInstancePos(instance, row, lastPos, NEXT_LEVEL);
				overlapSizeAf = overlap.size();
				if (overlapSizeBf != overlapSizeAf) {
					changeYSize = true;
					finalPos[Y] = overlap.size() * ROW_SIZE + SPACE_SIZE;
				} // end if 
				if (finalX < lastPos) {
					changeXSize = true;
					finalX = lastPos;
					finalPos[X] = finalX + SPACE_SIZE;
				} // end if 
				if (changeYSize || changeXSize) {
					resetCamera(0, 0, finalPos[X], finalPos[Y]);
				} // end if 
			} // end if
			for (Rsyn::Pin pinOut : pinIn.allSucessorPins())
				neighbours.push_back(pinOut);
			changeXSize = false;
			changeYSize = false;
		}// end for 
	}// end if 

	Refresh();
} // end method 

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::selectCellAt() {

	if (clsIsSelected) {
		DBUxy mousePos = clsMousePosition.convertToDbu();

		for (Rsyn::Instance instance : instancesOnScreen) {
			VisualInstance & inst = clsVisualInst[instance];
			const Bounds & bounds = inst.getBounds();
			if (bounds.inside(mousePos)) {
				selectedInstance = inst;
				selectedInstance.setColor(255, 178, 100);
				NewSchematicCanvasGL::renderSelectedCell();
			} else {
				selectedInstance.setColor(0, 0, 0);
			} // end if-else
		}// end for

		wxCommandEvent event(myEVT_SCHEMATIC_CELL_SELECTED); // No specific id
		//event.SetString(wxT("This is the data"));
		wxPostEvent(this, event); // to ourselves
	} // end if
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::renderSelectedCell() {
	Bounds bounds = selectedInstance.getBounds();
	DBUxy posLower;
	DBUxy posUpper;
	posLower[X] = bounds[LOWER][X];
	posLower[Y] = bounds[LOWER][Y];
	posUpper[X] = bounds[UPPER][X];
	posUpper[Y] = bounds[UPPER][Y];

	Color color = selectedInstance.getColor();

	glColor3ub(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	glVertex3f(posLower[X], posLower[Y], LAYER_SELECTED);
	glVertex3f(posUpper[X], posLower[Y], LAYER_SELECTED);
	glVertex3f(posUpper[X], posUpper[Y], LAYER_SELECTED);
	glVertex3f(posLower[X], posUpper[Y], LAYER_SELECTED);
	glEnd();

	if (clsDrawNeighborCells)
		getNeighbours();
	drawPin(selectedInstance);


	Refresh();
} // end method


// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::onMouseMoved(wxMouseEvent& event) {
	if (!clsMouseDown)
		return;

	clsDragging = true;

	const float x0 = translateFromViewportToUserX(clsDragStartX);
	const float y0 = translateFromViewportToUserY(clsDragStartY);

	const float x1 = translateFromViewportToUserX(event.m_x);
	const float y1 = translateFromViewportToUserY(event.m_y);

	const float dx = x1 - x0;
	const float dy = y1 - y0;

	clsMinX -= dx;
	clsMinY -= dy;
	clsMaxX -= dx;
	clsMaxY -= dy;

	clsDragStartX = event.m_x;
	clsDragStartY = event.m_y;

	Refresh(); // Note: this happens only when dragging...
} // end method


// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::onMouseDown(wxMouseEvent& event) {

	if (event.LeftDown()) {
		clsMouseDown = true;
		clsDragStartX = event.m_x;
		clsDragStartY = event.m_y;
		if (event.ShiftDown()) {
			clsIsSelected = true;
		}// end if
		else {
			clsIsSelected = false;
		}
	} // end if 
}// end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::onMouseReleased(wxMouseEvent& event) {

	if (clsIsSelected) {
		clsMousePosition[X] = translateFromViewportToUserX(event.m_x);
		clsMousePosition[Y] = translateFromViewportToUserY(event.m_y);
		selectCellAt();
	}
	CanvasGL::onMouseReleased(event);
} // end method
