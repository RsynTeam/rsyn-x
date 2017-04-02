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

#include "rsyn/gui/3rdparty/nanovg/nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "rsyn/gui/3rdparty/nanovg/nanovg_gl.h"

DEFINE_LOCAL_EVENT_TYPE(myEVT_SCHEMATIC_CELL_SELECTED)

// -----------------------------------------------------------------------------

SchematicCanvasGL::SchematicCanvasGL(wxWindow* parent) : CanvasGL(parent) {
	vg = nullptr;
	initialized = false;
			
	timer = new wxTimer(this);
	this->Connect(timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(SchematicCanvasGL::onTimeout));
	timer->Start(16);
	
	setZoomScaling(1.1f);
	reset();
} // end constructor

// -----------------------------------------------------------------------------

SchematicCanvasGL::~SchematicCanvasGL() {
	timer->Stop();
	delete timer;
	nvgDeleteGL2(vg);
} // end destructor

// -----------------------------------------------------------------------------

void SchematicCanvasGL::reset() {
	resetCamera(0, 0, getViewportWidth(), getViewportHeight());
} // end constructor

// -----------------------------------------------------------------------------

void SchematicCanvasGL::attachEngine(Rsyn::Engine engine) {
	reset();

	clsEngine = engine;

	Refresh();
} // end method

////////////////////////////////////////////////////////////////////////////////
// Events
////////////////////////////////////////////////////////////////////////////////

void SchematicCanvasGL::onRender(wxPaintEvent& evt) {
	if (!IsShownOnScreen()) return;
	render();
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::onTimeout(wxTimerEvent& event) {
	if (!IsShownOnScreen()) return;
	render();
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::onMouseMoved(wxMouseEvent& event) {
	// Get mouse in board coordinates.
	mouseX = event.GetX()*getZoom() - (clsOriginalMinX - clsMinX);
	mouseY = event.GetY()*getZoom() - (clsMaxY - clsOriginalMaxY);

	// Propagate event...
	CanvasGL::onMouseMoved(event);
} // end method

////////////////////////////////////////////////////////////////////////////////
// NanoVG Demo
////////////////////////////////////////////////////////////////////////////////

void SchematicCanvasGL::render() {
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
	nvgTranslate(vg, (clsOriginalMinX - clsMinX)/getZoom(), (clsMaxY - clsOriginalMaxY)/getZoom());
	nvgScale(vg, 1/getZoom(), 1/getZoom());
	
	// Why do we need this?
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// Time
	const float t = 10 * (clock() / float(CLOCKS_PER_SEC));
	
	renderDemo(mouseX, mouseY, boardWidth, boardHeight, t, false);
	nvgEndFrame(vg);
	
	glFlush();
	SwapBuffers();	
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::renderDemo(float mx, float my, float width, float height, float t, bool blowup) {
	// Background
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, width, height);
	nvgFillColor(vg, nvgRGB(255, 255,255));
	nvgFill(vg);	
	
	// Stuff
	drawNand(width - 500, 100, 80, 70);
	drawEyes(width - 250, 50, 150, 100, mx, my, t);
	drawGraph(0, height/2, width, height/2, t);
	drawColorwheel(width - 300, height - 300, 250.0f, 250.0f, t);

	// Line joints
	drawLines(120, height-50, 600, 50, t);

	// Line caps
	drawWidths(10, 50, 30);

	// Line caps
	drawCaps(10, 300, 30);
	drawScissor(50, height-80, t);

	// Widgets
	drawWindow("Rsyn", 50, 50, 300, 400);
	
	nvgSave(vg);
	if (blowup) {
		nvgRotate(vg, sinf(t*0.3f)*5.0f/180.0f*NVG_PI);
		nvgScale(vg, 2.0f, 2.0f);
	} // end if

	nvgRestore(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawWindow(const char* title, float x, float y, float w, float h) {
	float cornerRadius = 3.0f;
	NVGpaint shadowPaint;
	NVGpaint headerPaint;

	nvgSave(vg);

	// Window
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x,y, w,h, cornerRadius);
	nvgFillColor(vg, nvgRGBA(28,30,34,192));
	nvgFill(vg);

	// Drop shadow
	shadowPaint = nvgBoxGradient(vg, x,y+2, w,h, cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
	nvgBeginPath(vg);
	nvgRect(vg, x-10,y-10, w+20,h+30);
	nvgRoundedRect(vg, x,y, w,h, cornerRadius);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, shadowPaint);
	nvgFill(vg);

	// Header
	headerPaint = nvgLinearGradient(vg, x,y,x,y+15, nvgRGBA(255,255,255,8), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+1,y+1, w-2,30, cornerRadius-1);
	nvgFillPaint(vg, headerPaint);
	nvgFill(vg);
	nvgBeginPath(vg);
	nvgMoveTo(vg, x+0.5f, y+0.5f+30);
	nvgLineTo(vg, x+0.5f+w-1, y+0.5f+30);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,32));
	nvgStroke(vg);

	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans-bold");
	nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);

	nvgFontBlur(vg,2);
	nvgFillColor(vg, nvgRGBA(0,0,0,128));
	nvgText(vg, x+w/2,y+16+1, title, NULL);

	nvgFontBlur(vg,0);
	nvgFillColor(vg, nvgRGBA(220,220,220,160));
	nvgText(vg, x+w/2,y+16, title, NULL);

	nvgRestore(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawEyes(float x, float y, float w, float h, float mx, float my, float t) {
	NVGpaint gloss, bg;
	float ex = w *0.23f;
	float ey = h * 0.5f;
	float lx = x + ex;
	float ly = y + ey;
	float rx = x + w - ex;
	float ry = y + ey;
	float dx,dy,d;
	float br = (ex < ey ? ex : ey) * 0.5f;
	float blink = 1 - std::pow(sinf(t*0.5f),200.0f)*0.8f;

	bg = nvgLinearGradient(vg, x,y+h*0.5f,x+w*0.1f,y+h, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx+3.0f,ly+16.0f, ex,ey);
	nvgEllipse(vg, rx+3.0f,ry+16.0f, ex,ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	bg = nvgLinearGradient(vg, x,y+h*0.25f,x+w*0.1f,y+h, nvgRGBA(220,220,220,255), nvgRGBA(128,128,128,255));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx,ly, ex,ey);
	nvgEllipse(vg, rx,ry, ex,ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx+dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, lx+dx,ly+dy+ey*0.25f*(1-blink), br,br*blink);
	nvgFillColor(vg, nvgRGBA(32,32,32,255));
	nvgFill(vg);

	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx+dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, rx+dx,ry+dy+ey*0.25f*(1-blink), br,br*blink);
	nvgFillColor(vg, nvgRGBA(32,32,32,255));
	nvgFill(vg);

	gloss = nvgRadialGradient(vg, lx-ex*0.25f,ly-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx,ly, ex,ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);

	gloss = nvgRadialGradient(vg, rx-ex*0.25f,ry-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
	nvgBeginPath(vg);
	nvgEllipse(vg, rx,ry, ex,ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawNand(float x, float y, float w, float h) {
	NVGpaint bg;

	const float alpha = 0.4f;
	const float r = w*0.1f;
	const float aw = alpha * w;
	const float ew = 1.07f * w;
	
	nvgSave(vg);
	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,255));
	
	nvgBeginPath(vg);
	nvgMoveTo(vg, x + aw, y + 0);
	nvgLineTo(vg, x +  0, y + 0);
	nvgLineTo(vg, x +  0, y + h);
	nvgLineTo(vg, x + aw, y + h);
	nvgBezierTo(vg, x + ew, y + h, x + ew, y + 0, x + aw, y + 0);
	nvgStroke(vg);
	
	nvgBeginPath(vg);
	nvgCircle(vg, x+w, y + h/2, r);
	nvgStroke(vg);

	nvgRestore(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawScissor(float x, float y, float t) {
	nvgSave(vg);

	// Draw first rect and set scissor to it's area.
	nvgTranslate(vg, x, y);
	nvgRotate(vg, nvgDegToRad(5));
	nvgBeginPath(vg);
	nvgRect(vg, -20,-20,60,40);
	nvgFillColor(vg, nvgRGBA(255,0,0,255));
	nvgFill(vg);
	nvgScissor(vg, -20,-20,60,40);

	// Draw second rectangle with offset and rotation.
	nvgTranslate(vg, 40,0);
	nvgRotate(vg, t);

	// Draw the intended second rectangle without any scissoring.
	nvgSave(vg);
	nvgResetScissor(vg);
	nvgBeginPath(vg);
	nvgRect(vg, -20,-10,60,30);
	nvgFillColor(vg, nvgRGBA(255,128,0,64));
	nvgFill(vg);
	nvgRestore(vg);

	// Draw second rectangle with combined scissoring.
	nvgIntersectScissor(vg, -20,-10,60,30);
	nvgBeginPath(vg);
	nvgRect(vg, -20,-10,60,30);
	nvgFillColor(vg, nvgRGBA(255,128,0,255));
	nvgFill(vg);

	nvgRestore(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawWidths(float x, float y, float width) {
	int i;

	nvgSave(vg);

	nvgStrokeColor(vg, nvgRGBA(0,0,0,255));

	for (i = 0; i < 20; i++) {
		float w = (i+0.5f)*0.1f;
		nvgStrokeWidth(vg, w);
		nvgBeginPath(vg);
		nvgMoveTo(vg, x,y);
		nvgLineTo(vg, x+width,y+width*0.3f);
		nvgStroke(vg);
		y += 10;
	}

	nvgRestore(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawCaps(float x, float y, float width) {
	int i;
	int caps[3] = {NVG_BUTT, NVG_ROUND, NVG_SQUARE};
	float lineWidth = 8.0f;

	nvgSave(vg);

	nvgBeginPath(vg);
	nvgRect(vg, x-lineWidth/2, y, width+lineWidth, 40);
	nvgFillColor(vg, nvgRGBA(255,255,255,32));
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgRect(vg, x, y, width, 40);
	nvgFillColor(vg, nvgRGBA(255,255,255,32));
	nvgFill(vg);

	nvgStrokeWidth(vg, lineWidth);
	for (i = 0; i < 3; i++) {
		nvgLineCap(vg, caps[i]);
		nvgStrokeColor(vg, nvgRGBA(0,0,0,255));
		nvgBeginPath(vg);
		nvgMoveTo(vg, x, y + i*10 + 5);
		nvgLineTo(vg, x+width, y + i*10 + 5);
		nvgStroke(vg);
	} // end for

	nvgRestore(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawColorwheel(float x, float y, float w, float h, float t) {
	int i;
	float r0, r1, ax,ay, bx,by, cx,cy, aeps, r;
	float hue = sinf(t * 0.12f);
	NVGpaint paint;

	nvgSave(vg);

/*	nvgBeginPath(vg);
	nvgRect(vg, x,y,w,h);
	nvgFillColor(vg, nvgRGBA(255,0,0,128));
	nvgFill(vg);*/

	cx = x + w*0.5f;
	cy = y + h*0.5f;
	r1 = (w < h ? w : h) * 0.5f - 5.0f;
	r0 = r1 - 20.0f;
	aeps = 0.5f / r1;	// half a pixel arc length in radians (2pi cancels out).

	for (i = 0; i < 6; i++) {
		float a0 = (float)i / 6.0f * NVG_PI * 2.0f - aeps;
		float a1 = (float)(i+1.0f) / 6.0f * NVG_PI * 2.0f + aeps;
		nvgBeginPath(vg);
		nvgArc(vg, cx,cy, r0, a0, a1, NVG_CW);
		nvgArc(vg, cx,cy, r1, a1, a0, NVG_CCW);
		nvgClosePath(vg);
		ax = cx + cosf(a0) * (r0+r1)*0.5f;
		ay = cy + sinf(a0) * (r0+r1)*0.5f;
		bx = cx + cosf(a1) * (r0+r1)*0.5f;
		by = cy + sinf(a1) * (r0+r1)*0.5f;
		paint = nvgLinearGradient(vg, ax,ay, bx,by, nvgHSLA(a0/(NVG_PI*2),1.0f,0.55f,255), nvgHSLA(a1/(NVG_PI*2),1.0f,0.55f,255));
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	}

	nvgBeginPath(vg);
	nvgCircle(vg, cx,cy, r0-0.5f);
	nvgCircle(vg, cx,cy, r1+0.5f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,64));
	nvgStrokeWidth(vg, 1.0f);
	nvgStroke(vg);

	// Selector
	nvgSave(vg);
	nvgTranslate(vg, cx,cy);
	nvgRotate(vg, hue*NVG_PI*2);

	// Marker on
	nvgStrokeWidth(vg, 2.0f);
	nvgBeginPath(vg);
	nvgRect(vg, r0-1,-3,r1-r0+2,6);
	nvgStrokeColor(vg, nvgRGBA(255,255,255,192));
	nvgStroke(vg);

	paint = nvgBoxGradient(vg, r0-3,-5,r1-r0+6,10, 2,4, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
	nvgBeginPath(vg);
	nvgRect(vg, r0-2-10,-4-10,r1-r0+4+20,8+20);
	nvgRect(vg, r0-2,-4,r1-r0+4,8);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, paint);
	nvgFill(vg);

	// Center triangle
	r = r0 - 6;
	ax = cosf(120.0f/180.0f*NVG_PI) * r;
	ay = sinf(120.0f/180.0f*NVG_PI) * r;
	bx = cosf(-120.0f/180.0f*NVG_PI) * r;
	by = sinf(-120.0f/180.0f*NVG_PI) * r;
	nvgBeginPath(vg);
	nvgMoveTo(vg, r,0);
	nvgLineTo(vg, ax,ay);
	nvgLineTo(vg, bx,by);
	nvgClosePath(vg);
	paint = nvgLinearGradient(vg, r,0, ax,ay, nvgHSLA(hue,1.0f,0.5f,255), nvgRGBA(255,255,255,255));
	nvgFillPaint(vg, paint);
	nvgFill(vg);
	paint = nvgLinearGradient(vg, (r+ax)*0.5f,(0+ay)*0.5f, bx,by, nvgRGBA(0,0,0,0), nvgRGBA(0,0,0,255));
	nvgFillPaint(vg, paint);
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,64));
	nvgStroke(vg);

	// Select circle on triangle
	ax = cosf(120.0f/180.0f*NVG_PI) * r*0.3f;
	ay = sinf(120.0f/180.0f*NVG_PI) * r*0.4f;
	nvgStrokeWidth(vg, 2.0f);
	nvgBeginPath(vg);
	nvgCircle(vg, ax,ay,5);
	nvgStrokeColor(vg, nvgRGBA(255,255,255,192));
	nvgStroke(vg);

	paint = nvgRadialGradient(vg, ax,ay, 7,9, nvgRGBA(0,0,0,64), nvgRGBA(0,0,0,0));
	nvgBeginPath(vg);
	nvgRect(vg, ax-20,ay-20,40,40);
	nvgCircle(vg, ax,ay,7);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, paint);
	nvgFill(vg);

	nvgRestore(vg);

	nvgRestore(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawLines(float x, float y, float w, float h, float t) {
	int i, j;
	float pad = 5.0f, s = w/9.0f - pad*2;
	float pts[4*2], fx, fy;
	int joins[3] = {NVG_MITER, NVG_ROUND, NVG_BEVEL};
	int caps[3] = {NVG_BUTT, NVG_ROUND, NVG_SQUARE};
	NVG_NOTUSED(h);

	nvgSave(vg);
	pts[0] = -s*0.25f + cosf(t*0.3f) * s*0.5f;
	pts[1] = sinf(t*0.3f) * s*0.5f;
	pts[2] = -s*0.25f;
	pts[3] = 0;
	pts[4] = s*0.25f;
	pts[5] = 0;
	pts[6] = s*0.25f + cosf(-t*0.3f) * s*0.5f;
	pts[7] = sinf(-t*0.3f) * s*0.5f;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			fx = x + s*0.5f + (i*3+j)/9.0f*w + pad;
			fy = y - s*0.5f + pad;

			nvgLineCap(vg, caps[i]);
			nvgLineJoin(vg, joins[j]);

			nvgStrokeWidth(vg, s*0.3f);
			nvgStrokeColor(vg, nvgRGBA(0,0,0,160));
			nvgBeginPath(vg);
			nvgMoveTo(vg, fx+pts[0], fy+pts[1]);
			nvgLineTo(vg, fx+pts[2], fy+pts[3]);
			nvgLineTo(vg, fx+pts[4], fy+pts[5]);
			nvgLineTo(vg, fx+pts[6], fy+pts[7]);
			nvgStroke(vg);

			nvgLineCap(vg, NVG_BUTT);
			nvgLineJoin(vg, NVG_BEVEL);

			nvgStrokeWidth(vg, 1.0f);
			nvgStrokeColor(vg, nvgRGBA(0,192,255,255));
			nvgBeginPath(vg);
			nvgMoveTo(vg, fx+pts[0], fy+pts[1]);
			nvgLineTo(vg, fx+pts[2], fy+pts[3]);
			nvgLineTo(vg, fx+pts[4], fy+pts[5]);
			nvgLineTo(vg, fx+pts[6], fy+pts[7]);
			nvgStroke(vg);
		}
	}

	nvgRestore(vg);
} // end method

// -----------------------------------------------------------------------------

void SchematicCanvasGL::drawGraph(float x, float y, float w, float h, float t) {
	NVGpaint bg;
	float samples[6];
	float sx[6], sy[6];
	float dx = w/5.0f;
	int i;

	samples[0] = (1+sinf(t*1.2345f+cosf(t*0.33457f)*0.44f))*0.5f;
	samples[1] = (1+sinf(t*0.68363f+cosf(t*1.3f)*1.55f))*0.5f;
	samples[2] = (1+sinf(t*1.1642f+cosf(t*0.33457f)*1.24f))*0.5f;
	samples[3] = (1+sinf(t*0.56345f+cosf(t*1.63f)*0.14f))*0.5f;
	samples[4] = (1+sinf(t*1.6245f+cosf(t*0.254f)*0.3f))*0.5f;
	samples[5] = (1+sinf(t*0.345f+cosf(t*0.03f)*0.6f))*0.5f;

	for (i = 0; i < 6; i++) {
		sx[i] = x+i*dx;
		sy[i] = y+h*samples[i]*0.8f;
	}

	// Graph background
	bg = nvgLinearGradient(vg, x,y,x,y+h, nvgRGBA(0,160,192,0), nvgRGBA(0,160,192,64));
	nvgBeginPath(vg);
	nvgMoveTo(vg, sx[0], sy[0]);
	for (i = 1; i < 6; i++)
		nvgBezierTo(vg, sx[i-1]+dx*0.5f,sy[i-1], sx[i]-dx*0.5f,sy[i], sx[i],sy[i]);
	nvgLineTo(vg, x+w, y+h);
	nvgLineTo(vg, x, y+h);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	// Graph line
	nvgBeginPath(vg);
	nvgMoveTo(vg, sx[0], sy[0]+2);
	for (i = 1; i < 6; i++)
		nvgBezierTo(vg, sx[i-1]+dx*0.5f,sy[i-1]+2, sx[i]-dx*0.5f,sy[i]+2, sx[i],sy[i]+2);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,32));
	nvgStrokeWidth(vg, 3.0f);
	nvgStroke(vg);

	nvgBeginPath(vg);
	nvgMoveTo(vg, sx[0], sy[0]);
	for (i = 1; i < 6; i++)
		nvgBezierTo(vg, sx[i-1]+dx*0.5f,sy[i-1], sx[i]-dx*0.5f,sy[i], sx[i],sy[i]);
	nvgStrokeColor(vg, nvgRGBA(0,160,192,255));
	nvgStrokeWidth(vg, 3.0f);
	nvgStroke(vg);

	// Graph sample pos
	for (i = 0; i < 6; i++) {
		bg = nvgRadialGradient(vg, sx[i],sy[i]+2, 3.0f,8.0f, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,0));
		nvgBeginPath(vg);
		nvgRect(vg, sx[i]-10, sy[i]-10+2, 20,20);
		nvgFillPaint(vg, bg);
		nvgFill(vg);
	}

	nvgBeginPath(vg);
	for (i = 0; i < 6; i++)
		nvgCircle(vg, sx[i], sy[i], 4.0f);
	nvgFillColor(vg, nvgRGBA(0,160,192,255));
	nvgFill(vg);
	nvgBeginPath(vg);
	for (i = 0; i < 6; i++)
		nvgCircle(vg, sx[i], sy[i], 2.0f);
	nvgFillColor(vg, nvgRGBA(220,220,220,255));
	nvgFill(vg);

	nvgStrokeWidth(vg, 1.0f);
} // end method

////////////////////////////////////////////////////////////////////////////////
// Drawing (Experimental)
////////////////////////////////////////////////////////////////////////////////

enum ShapeType {
	SHAPE_TYPE_LINE,
	SHAPE_TYPE_RECTANGLE,
	SHAPE_TYPE_CIRCLE,
	SHAPE_TYPE_PATH
};

class Scribble {
public:
	virtual void render(const float ratio, const float z) const = 0;
}; // end class

class CircleScribble : public Scribble {
private:
	float xc;
	float yc;
	float r;

public:

	void set(const float x, const float y, const float radius) {
		xc = x;
		yc = y;
		r = radius;
	} // end method

	virtual void render(const float ratio, const float z) const override {
		const int numSamples = (int) std::max(10.0f, 10.0f / ratio);
		const float degreePerStep = (2.0f * 3.14159265359f) / numSamples;

		glBegin(GL_POLYGON);
		for (int i = 0; i < numSamples; i++) {
			const float theta = i * degreePerStep;
			const float x = xc + (r * std::cos(theta));
			const float y = yc + (r * std::sin(theta));
			glVertex3f(x, y, z);
		} // end for
		glEnd();
	} // end method
}; // end class

class PathScribble : public Scribble {
private:
	enum PathType {
		LINE,
		QUADRATIC,
		CUBIC
	};

	std::vector<float> points;
	std::vector<PathType> cmds;

	void
	renderLineTo(
		const float x, const float y, const float z
	) const {
		glVertex3f(x, y, z);
	} // end method


	void
	renderQuadraticCurveTo(
		const float x0, const float y0,
		const float x1, const float y1,
		const float x2, const float y2,
		const float z
	) const {
		const int N = 25;
		
		for (int i = 1; i <= N; i++) {  // first point was already processed
			const float t1 = float(i)/float(N);
			const float t2 = t1*t1;
			const float a = 1-t1; // (1-t)^1
			const float b = a*a;  // (1-t)^2

			const float xt = b*x0 + 2*a*t1*x1 + t2*x2;
			const float yt = b*y0 + 2*a*t1*y1 + t2*y2;

			glVertex3f(xt, yt, z);
		} // end for
	} // end for

	void
	renderCubicCurveTo(
		const float x0, const float y0,
		const float x1, const float y1,
		const float x2, const float y2,
		const float x3, const float y3,
		const float z
	) const {
		const int N = 25;

		for (int i = 1; i <= N; i++) { // first point was already processed
			const float t1 = float(i)/float(N);
			const float t2 = t1*t1;
			const float t3 = t2*t1;
			const float a = 1-t1; // (1-t)^1
			const float b = a*a;  // (1-t)^2
			const float c = b*a;  // (1-t)^3

			const float xt = c*x0 + 3*b*t1*x1 + 3*a*t2*x2 + t3*x3;
			const float yt = c*y0 + 3*b*t1*y1 + 3*a*t2*y2 + t3*y3;

			glVertex3f(xt, yt, z);
		} // end for
	} // end method

	void 
	processLine(
			const float z,
			int &head
	) const {
		const float x = points[head++];
		const float y = points[head++];
		renderLineTo(x, y, z);
	} // end method

	void 
	processQuadraticCurve(
			const float z,
			int &head
	) const {
		const float x0 = points[head-2];
		const float y0 = points[head-1];
		const float x1 = points[head++];
		const float y1 = points[head++];
		const float x2 = points[head++];
		const float y2 = points[head++];
		renderQuadraticCurveTo(x0, y0, x1, y1, x2, y2, z);
	} // end method

	void 
	processCubicCurve(
			const float z,
			int &head
	) const {
		const float x0 = points[head-2];
		const float y0 = points[head-1];
		const float x1 = points[head++];
		const float y1 = points[head++];
		const float x2 = points[head++];
		const float y2 = points[head++];
		const float x3 = points[head++];
		const float y3 = points[head++];
		renderCubicCurveTo(x0, y0, x1, y1, x2, y2, x3, y3, z);
	} // end method

public:

	virtual void render(
			const float ratio, const float z
	) const override {
		if (cmds.empty())
			return;

		glBegin(GL_POLYGON);
		glVertex3f(points[0], points[1], z);
		int head = 2;
		for (PathType type : cmds) {
			switch (type) {
				case LINE:
					processLine(z, head);
					break;
				case QUADRATIC:
					processQuadraticCurve(z, head);
					break;
				case CUBIC:
					processCubicCurve(z, head);
					break;
				default:
					assert(false);
			} // end switch
		} // end for

		glEnd();
	} // end method

	void moveTo(
			const float x, const float y
	) {
		points.clear();
		points.push_back(x);
		points.push_back(y);
	} // end method

	void lineTo(
			const float x, const float y
	) {
		cmds.push_back(LINE);
		points.push_back(x);
		points.push_back(y);
	} // end method

	void quadraticCurveTo(
			const float x1, const float y1,
			const float x2, const float y2
	) {
		cmds.push_back(QUADRATIC);
		points.push_back(x1);
		points.push_back(y1);
		points.push_back(x2);
		points.push_back(y2);
	} // end method

	void cubicCurveTo(
			const float x1, const float y1,
			const float x2, const float y2,
			const float x3, const float y3
	) {
		cmds.push_back(CUBIC);
		points.push_back(x1);
		points.push_back(y1);
		points.push_back(x2);
		points.push_back(y2);
		points.push_back(x3);
		points.push_back(y3);
	} // end method
	
}; // end class

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::renderExperimental() {
	const float x = 5;
	const float y = 5;
	const float w = 4;
	const float h = 4;
	
	const float alpha = 0.4f;
	const float r = w*0.15f;
	const float aw = alpha * w;
	const float ew = 1.07f * w;

	PathScribble path;
	path.moveTo(x + aw, y + 0);
	path.lineTo(x +  0, y + 0);
	path.lineTo(x +  0, y + h);
	path.lineTo(x + aw, y + h);
	path.cubicCurveTo(x + ew, y + h, x + ew, y + 0, x + aw, y + 0);

	CircleScribble circle;
	circle.set(x+w+r/2-0.1f, y + h/2, r);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3ub(255, 255, 255);
	path.render(0.1f, LAYER_SHAPE_FILLING);
	circle.render(0.1f, LAYER_SHAPE_FILLING);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3ub(0, 0, 0);
	path.render(0.1f, LAYER_SHAPES);
	circle.render(0.1f, LAYER_SHAPES);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
} // end method

// #############################################################################
// New Schematic
// #############################################################################

// -----------------------------------------------------------------------------

const float NewSchematicCanvasGL::LAYER_GRID   = 0.0f;
const float NewSchematicCanvasGL::LAYER_SHAPE_FILLING = 0.05f;
const float NewSchematicCanvasGL::LAYER_SHAPES = 0.1f;
const float NewSchematicCanvasGL::LAYER_SELECTED = 0.4f;

// -----------------------------------------------------------------------------

NewSchematicCanvasGL::NewSchematicCanvasGL(wxWindow* parent) : CanvasGL(parent) {
	resetClickedView();
	
//	setZoomScaling(1.1f);
//
//	Box *box1 = new Box();
//	Box *box2 = new Box();
//	Box *box3 = new Box();
//	box1->move(0, 0);
//	box2->move(0, 8);
//	box3->move(8, 4);
//
//	Line *line1 = new Line();
//	Line *line2 = new Line();
//	Line *line3 = new Line();
//	line1->set(4, 2, 6, 2);
//	line2->set(6, 2, 6, 5);
//	line3->set(6, 5, 8, 5);
//
//	Line *line4 = new Line();
//	Line *line5 = new Line();
//	Line *line6 = new Line();
//	line4->set(4, 10, 6, 10);
//	line5->set(6, 10, 6, 7);
//	line6->set(6, 7, 8, 7);
//
//	clsShapes.push_back(box1);
//	clsShapes.push_back(box2);
//	clsShapes.push_back(box3);
//	clsShapes.push_back(line1);
//	clsShapes.push_back(line2);
//	clsShapes.push_back(line3);
//	clsShapes.push_back(line4);
//	clsShapes.push_back(line5);
//	clsShapes.push_back(line6);
	
} // end constructor

// -----------------------------------------------------------------------------

NewSchematicCanvasGL::~NewSchematicCanvasGL() {
//	for (Shape *shape : clsShapes) {
//		delete shape;
//	} // end for
} // end destructor
//
//// -----------------------------------------------------------------------------
//
//void NewSchematicCanvasGL::renderShapes() {
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glColor3ub(0, 0, 0);
//
//	for (Shape *shape : clsShapes) {
//		glTranslatef(+shape->getX(), +shape->getY(), 0.0f);
//		shape->render();
//		glTranslatef(-shape->getX(), -shape->getY(), 0.0f);
//	} // end for
//} // end method
//
//// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::resetClickedView() {
	clsDrawCriticalPath = false;
	clsDrawSelectedCell = false;
	clsDrawNeighborCells = false;
	clsDrawLogicCone = false;
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::setViewCriticalPaths(const bool visible) {
	clsDrawCriticalPath = visible;
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
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::setNumPathsToDraw(const int numPaths) {
	clsNumPathsToDraw = numPaths;
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::renderGrid() {
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

	for (int y = 0; y <= ymax; y++) {
		glVertex3f(xmin, y, LAYER_GRID);
		glVertex3f(xmax, y, LAYER_GRID);
	} // end for

	for (int y = 0; y >= ymin; y--) {
		glVertex3f(xmin, y, LAYER_GRID);
		glVertex3f(xmax, y, LAYER_GRID);
	} // end for

	for (int x = 0; x <= xmax; x++) {
		glVertex3f(x, ymin, LAYER_GRID);
		glVertex3f(x, ymax, LAYER_GRID);
	} // end for

	for (int x = 0; x >= xmin; x--) {
		glVertex3f(x, ymin, LAYER_GRID);
		glVertex3f(x, ymax, LAYER_GRID);
	} // end for

	glEnd();
	glLineWidth(1);
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::attachEngine(Rsyn::Engine engine){
	clsEngine = engine;
	timer = clsEngine.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);
	clsDesign = clsEngine.getDesign();
	init();
}


// -----------------------------------------------------------------------------
void NewSchematicCanvasGL::init(){
	clsVisualInst = clsDesign.createAttribute();
	criticalPath();

} //end method
// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::onRender(wxPaintEvent& evt) {
	if (!IsShownOnScreen()) return;
	prepare2DViewport(); // required to setup OpenGL context
	
	if (clsFirstRendering) {
		// I'm no sure why resetting in the constructor did not work.
		resetCamera(0, 0, finalPos[X], finalPos[Y]);
		clsFirstRendering = false;
	} // end if

	renderGrid();
	//renderExperimental();
	drawPath();
	selectCellAt();
	openNextCells();
	glFlush();
	SwapBuffers();	
} // end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::criticalPath(){
	int numberPaths = timer->queryTopCriticalPathFromTopCriticalEndpoints(Rsyn::LATE, 1, clsPaths, 0);
	definePathPos();
}

// -----------------------------------------------------------------------------
void NewSchematicCanvasGL::defineInstancePos(Rsyn::Instance instance, DBUxy & lastPos, int y){

	DBU cellSize = 4;
	DBU cellSpace = 2;
	DBUxy posMin;
	DBUxy posMax;
	VisualInstance & cdInstance = clsVisualInst[instance];
	posMin[X] = lastPos[X] + cellSpace;
	posMax[X] = posMin[X] + cellSize;
	cdInstance.setInst(instance);
	switch(y){
		case -1: {posMin[Y] = lastPos[Y] - cellSpace - cellSize; 
			 posMax[Y] = lastPos[Y] - cellSpace;
			 break;}
		case 0: {posMin[Y] = lastPos[Y]; 
			posMax[Y] = lastPos[Y] + cellSize;
			break;}
		case 1: {posMin[Y] = lastPos[Y] + cellSpace; 
			posMax[Y] = posMin[Y] + cellSize;
			break;}
	} //end switch
	cdInstance.setBounds(posMin[X], posMin[Y], posMax[X], posMax[Y]);
	if (instance.isSequential())
		cdInstance.setColor(255, 0, 255);
	else
		if (instance.getType() == Rsyn::PORT)
			cdInstance.setColor(255, 0, 0);
		else
			if(instance.isFixed())
				cdInstance.setColor(0, 0, 0);
			else
				cdInstance.setColor(0, 0, 255);	
	lastPos[X] = posMax[X];
	lastPos[Y] = posMin[Y];	

} //end method
// -----------------------------------------------------------------------------
void NewSchematicCanvasGL::definePathPos(){
	DBUxy lastPos;
	int y = 0;
	DBU finalX = 0;
	
	for (std::vector<Rsyn::Timer::PathHop> path : clsPaths){
		for (Rsyn::Timer::PathHop hop : path){
			Rsyn::Instance instance = hop.getInstance();
			VisualInstance & cdInstance = clsVisualInst[instance];
			if (!cdInstance.getInst()){
				defineInstancePos(instance, lastPos, y);
			}
			y=0;
		} //end for
		if (finalX < lastPos[X])
			finalX = lastPos[X];
		y = 1;
	} //end for

	finalPos[X] = finalX;
	finalPos[Y] = lastPos[Y] + 4;
	
}//end method


// -----------------------------------------------------------------------------
void NewSchematicCanvasGL::drawPath(){
	
	for (std::vector<Rsyn::Timer::PathHop> path : clsPaths){
		for (Rsyn::Timer::PathHop hop : path){
			Rsyn::Instance instance = hop.getInstance();
			drawInstance(instance);
		} //end for
	} //end for

}
// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::drawInstance(Rsyn::Instance instance){
		
		VisualInstance inst = clsVisualInst[instance];
		if (!inst.init){
			clsInstances.push_back(inst);
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
		}


} //end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::openNextCells(){
//	if (selectedInstance.getInst()){
//		Rsyn::Instance inst = selectedInstance.getInst();
//		Bounds bounds = selectedInstance.getBounds();
//		DBUxy pos;
//		pos[X] = bounds[UPPER][X];
//		pos[Y] = bounds[LOWER][Y];
//		int top;
//
//		for (Rsyn::Pin pinOut: inst.allPins(Rsyn::OUT)){
//			Rsyn::Net net = pinOut.getNet();
//			int numPins = net.getNumSinks();
//			if ((numPins % 2) != 0){
//				top = numPins/2 +1;
//			}
//			else{
//				top = numPins/2;
//			}
//			for (Rsyn::Pin pinIn : net.allPins(Rsyn::IN)){
//				Rsyn::Instance instance = pinIn.getInstance();
//				VisualInstance & instVisual = clsVisualInst[instance];
//				if (!instVisual.getInst()){
//					if (top > 0){
//						defineInstancePos(instance, pos, 1);
//					} //end if
//					else{
//						defineInstancePos(instance, pos, -1);
//					} //end else
//					drawInstance(instance);
//				}
//				top--;
//			}//end for 
//		}//end for
//	
//	} //end if	
}//end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::selectCellAt(){

	DBUxy mousePos = clsMousePosition.convertToDbu();

	for (VisualInstance inst : clsInstances) {
		const Bounds & bounds = inst.getBounds();
		if (bounds.inside(mousePos)) {
			selectedInstance = inst;
			selectedInstance.setColor(255, 178, 100);
			NewSchematicCanvasGL::renderSelectedCell();
		} else {
			selectedInstance.setColor(0, 0, 0);
		}
	}//end for
	
	wxCommandEvent event(myEVT_SCHEMATIC_CELL_SELECTED); // No specific id
	//event.SetString(wxT("This is the data"));
	wxPostEvent(this, event); // to ourselves
} //end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::renderSelectedCell(){
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
		
		
		Refresh();		
}


// -----------------------------------------------------------------------------
void NewSchematicCanvasGL::onMouseMoved(wxMouseEvent& event) {
    if ( !clsMouseDown )
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
		} // end if
		else{
			clsIsSelected = false;
		}
	} // end if 
}// end method

// -----------------------------------------------------------------------------

void NewSchematicCanvasGL::onMouseReleased(wxMouseEvent& event) {
	
	if (clsIsSelected){
		clsMousePosition[X] = translateFromViewportToUserX(event.m_x);
		clsMousePosition[Y] = translateFromViewportToUserY(event.m_y);
		selectCellAt();
		openNextCells();
	}
	CanvasGL::onMouseReleased(event);
} // end method

// =============================================================================
// Shapes
// =============================================================================

//void
//NewSchematicCanvasGL::Box::render() {
//	glBegin(GL_QUADS);
//	glVertex3f(0.5, 0.5, LAYER_SHAPES);
//	glVertex3f(3.5, 0.5, LAYER_SHAPES);
//	glVertex3f(3.5, 3.5, LAYER_SHAPES);
//	glVertex3f(0.5, 3.5, LAYER_SHAPES);
//	glEnd();
//
//	glBegin(GL_LINES);
//	glVertex3f(0.0, 1.0, LAYER_SHAPES);
//	glVertex3f(0.5, 1.0, LAYER_SHAPES);
//
//	glVertex3f(0.0, 3.0, LAYER_SHAPES);
//	glVertex3f(0.5, 3.0, LAYER_SHAPES);
//
//	glVertex3f(3.5, 2.0, LAYER_SHAPES);
//	glVertex3f(4.0, 2.0, LAYER_SHAPES);
//	glEnd();
//} // end method
//
//// -----------------------------------------------------------------------------
//
//void
//NewSchematicCanvasGL::Line::render() {
//	glBegin(GL_LINES);
//	glVertex3f(0.0, 0.0, LAYER_SHAPES);
//	glVertex3f(getWidth(), getHeight(), LAYER_SHAPES);
//	glEnd();
//} // end method
//
