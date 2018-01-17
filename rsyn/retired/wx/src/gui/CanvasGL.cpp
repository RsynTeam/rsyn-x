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
 
#include "CanvasGL.h"

#include <iostream>
#include <wx/window.h>
#include <wx/settings.h>
using std::cout;
using std::cerr;
using std::endl;

int CanvasGL::CANVAS_GL_ARGS[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

// -----------------------------------------------------------------------------

CanvasGL::CanvasGL(wxWindow* parent) :
	wxGLCanvas(parent, wxID_ANY, CANVAS_GL_ARGS, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS)
{	
	
	Connect( wxEVT_MOTION, wxMouseEventHandler(CanvasGL::onMouseMoved), NULL, this );
	Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler(CanvasGL::onMouseDown), NULL, this );
	Connect( wxEVT_LEFT_UP, wxMouseEventHandler(CanvasGL::onMouseReleased), NULL, this );
	Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler(CanvasGL::onLeftDoubleClick), NULL, this );
	Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler(CanvasGL::onRightClick), NULL, this );
	Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler(CanvasGL::onMouseLeaveWindow), NULL, this );
	Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler(CanvasGL::onMouseWheelMoved), NULL, this );
	
	Connect( wxEVT_KEY_DOWN, wxKeyEventHandler(CanvasGL::onKeyPressed), NULL, this );
	Connect( wxEVT_KEY_UP, wxKeyEventHandler(CanvasGL::onKeyReleased), NULL, this );
		
	Connect( wxEVT_SIZE, wxSizeEventHandler(CanvasGL::onResized), NULL, this );
	
	Connect( wxEVT_PAINT, wxPaintEventHandler(CanvasGL::onRender ), NULL, this );
	
	clsContext = new wxGLContext(this);

	SetBackgroundStyle(wxBG_STYLE_CUSTOM); // to avoid flashing on MSW

	clsMouseDown = false;
	clsOriginalMinX = 0;
	clsOriginalMinY = 0;
	clsOriginalMaxX = 0;
	clsOriginalMaxY = 0;
	clsZoomScaling = 2;
	resetCamera();
} // end constructor

// -----------------------------------------------------------------------------

CanvasGL::~CanvasGL() {
	delete clsContext;
} // end destructor

// -----------------------------------------------------------------------------

void CanvasGL::onMouseMoved(wxMouseEvent& event) {
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

void CanvasGL::onMouseDown(wxMouseEvent& event) {
	if ( event.m_leftDown ) {
		clsMouseDown = true;
		clsDragStartX = event.m_x;
		clsDragStartY = event.m_y;
	} // end if
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::onMouseWheelMoved(wxMouseEvent& event) {
	// Get mouse position in space coordinates.
	const float sx = translateFromViewportToUserX(event.m_x);
	const float sy = translateFromViewportToUserY(event.m_y);

	// Scaling factor.
	//const float scaling = (event.m_wheelRotation < 0)? 2 : 0.5;
	const float scaling = (event.m_wheelRotation < 0)? (1*clsZoomScaling) : (1/clsZoomScaling);

	// Zoom.
	doZoomTo(sx, sy, scaling);
	Refresh();
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::onMouseReleased(wxMouseEvent& event) {
	clsMouseDown = event.m_leftDown;
	clsDragging = false;
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::onLeftDoubleClick(wxMouseEvent& event) {
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::onRightClick(wxMouseEvent& event) {
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::onMouseLeaveWindow(wxMouseEvent& event) {
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::onKeyPressed(wxKeyEvent& event) {
	event.Skip();
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::onKeyReleased(wxKeyEvent& event) {
	event.Skip();
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::onResized(wxSizeEvent& evt) {
	// Adjust the view region to the new size of the viewport by resizing the
	// view region according to the change in the viewport dimensions.
	
	const wxSize previousSize = clsViewportSize;
	const wxSize currentSize= evt.GetSize();
	
	const float scalingx = currentSize.GetX() / float(previousSize.GetX());
	const float scalingy = currentSize.GetY() / float(previousSize.GetY());
	
	const float xmin = clsMinX;
	const float ymin = clsMinY;
	const float xmax = clsMaxX;
	const float ymax = clsMaxY;
	
	const float xmiddle = (xmax + xmin) / 2;
	const float ymiddle = (ymax + ymin) / 2;

	clsMaxX = (clsMaxX - xmiddle)*scalingx + xmiddle;
	clsMinX = (clsMinX - xmiddle)*scalingx + xmiddle;

	clsMaxY = (clsMaxY - ymiddle)*scalingy + ymiddle;
	clsMinY = (clsMinY - ymiddle)*scalingy + ymiddle;
	
	clsViewportSize = currentSize;
} // end method

// -----------------------------------------------------------------------------

float CanvasGL::translateFromViewportToUserX(const int viewportX ) const {
	const float sx = viewportX / float(GetSize().x);
	return sx*(clsMaxX - clsMinX) + clsMinX;
} // end method

// -----------------------------------------------------------------------------

float CanvasGL::translateFromViewportToUserY(const int viewportY ) const {
	const float sy = ( GetSize().y - viewportY ) / float(GetSize().y);
	return sy*(clsMaxY - clsMinY) + clsMinY;
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::resetCamera() {
	clsMinX = clsOriginalMinX;
	clsMaxX = clsOriginalMaxX;
	clsMinY = clsOriginalMinY;
	clsMaxY = clsOriginalMaxY;
	
	clsZoom = 1;

	adjustAspectRatio();
	clsViewportSize = GetSize();
	
	Refresh();
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::resetCamera(const float minx, const float miny, const float maxx, const float maxy) {
	clsOriginalMinX = minx;
	clsOriginalMinY = miny;
	clsOriginalMaxX = maxx;
	clsOriginalMaxY = maxy;
	resetCamera();
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::doMoveTo(const float x, const float y) {
	const float w = clsMaxX - clsMinX;
	const float h = clsMaxY - clsMinY;
	
	clsMinX = x;
	clsMinY = y;
	
	clsMaxX = x + w;
	clsMaxY = y + h;	
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::doMoveCenterTo(const float x, const float y) {
	const float hw = (clsMaxX - clsMinX) / 2.0f;
	const float hh = (clsMaxY - clsMinY) / 2.0f;
	
	clsMinX = x - hw;
	clsMinY = y - hh;
	
	clsMaxX = x + hw;
	clsMaxY = y + hh;	
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::moveTo(const float x, const float y) {
	doMoveTo(x, y);	
	Refresh();
} // end method


// -----------------------------------------------------------------------------

void CanvasGL::moveCenterTo(const float x, const float y) {
	doMoveCenterTo(x, y);	
	Refresh();
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::doZoomTo(const float x, const float y, const float scaling) {
	const float oldw = clsMaxX - clsMinX;
	const float oldh = clsMaxY - clsMinY;

	const float sx = (x-clsMinX) / oldw;
	const float sy = (y-clsMinY) / oldh;
	
	const float neww = oldw * scaling;
	const float newh = oldh * scaling;
	
	clsMinX = sx * (oldw - neww) + clsMinX;
	clsMinY = sy * (oldh - newh) + clsMinY;
	clsMaxX = clsMinX + neww;
	clsMaxY = clsMinY + newh;
	
	clsZoom *= scaling;
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::zoomTo(const float x, const float y, const float zoom, const bool centralize) {
	if (centralize)
		doMoveCenterTo(x, y);
	doZoomTo(x, y, zoom/clsZoom);
	Refresh();
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::zoom(const float scaling) {
	doZoomTo(getCenterX(), getCenterY(), scaling);
	Refresh();	
} // end 

// -----------------------------------------------------------------------------

void CanvasGL::adjustAspectRatio() {
	const float xmin = clsMinX;
	const float ymin = clsMinY;
	const float xmax = clsMaxX;
	const float ymax = clsMaxY;
	
	const float xmiddle = (xmax + xmin) / 2;
	const float ymiddle = (ymax + ymin) / 2;
	
	const float viewportW = GetSize().x;
	const float viewportH = GetSize().y;
	
	const float userW = (xmax - xmin);
	const float userH = (ymax - ymin);
	
	const float w = userH * (viewportW / viewportH);
	const float scalingx = w / userW;

	const float h = userW * (viewportH / viewportW);
	const float scalingy = h / userH;
	
	if (scalingy > scalingx) {
		clsMaxY = (ymax-ymiddle)*scalingy + ymiddle;
		clsMinY = (ymin-ymiddle)*scalingy + ymiddle;
		clsZoom /= scalingy;
	} else {
		clsMaxX = (xmax-xmiddle)*scalingx + xmiddle;
		clsMinX = (xmin-xmiddle)*scalingx + xmiddle;
		clsZoom /= scalingx;
	} // end else	
} // end method
// -----------------------------------------------------------------------------

/** Inits the OpenGL viewport for drawing in 2D. */
void CanvasGL::prepare2DViewport(const int width, const int height, const bool clear) {
	wxGLCanvas::SetCurrent(*clsContext);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_TEXTURE_2D); // textures
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (width == 0 && height == 0) {
		glViewport(0, 0, GetSize().x, GetSize().y);
	} else {
		glViewport(0, 0, width, height);
	} // end else

	if (clear)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluOrtho2D(clsMinX, clsMaxX, clsMinY, clsMaxY);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::clearViewport() {
	wxGLCanvas::SetCurrent(*clsContext);
	
	wxColor color = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	const float r = color.Red() / 255.0f;
	const float g = color.Green() / 255.0f;
	const float b = color.Blue() / 255.0f;
	
	glClearColor(r, g, b, 1.0f); // white background
	glDisable(GL_TEXTURE_2D); // textures
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, GetSize().x, GetSize().y);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glFlush();
	SwapBuffers();
} // end method

// -----------------------------------------------------------------------------

void CanvasGL::snapshot(wxImage& image) const {
	glFlush();

	float viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);

	const int w = getViewportWidth();
	const int h = getViewportHeight();
	
	GLubyte * pixels = new GLubyte[w * h * 4];
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	char * source = (char *) pixels;
	char * rgb = new char[ w * h * 3 ];
	char * alpha = new char[ w * h * 1 ];

	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			const int indexRaw = (row * w) + col;
			const int indexSource = indexRaw * 4;
			const int indexRGB = indexRaw * 3;
			const int indexAlpha = indexRaw * 1;

			rgb[indexRGB + 0] = source[indexSource + 0];
			rgb[indexRGB + 1] = source[indexSource + 1];
			rgb[indexRGB + 2] = source[indexSource + 2];

			alpha[indexAlpha] = source[indexSource + 3];
		}//end for row
	}//end for line

	image.Create(w, h);
	image.SetData((unsigned char*) rgb);
	image.SetAlpha((unsigned char*) alpha);
	image = image.Mirror(false);
	
//	const float wFit = (float) 1920 / w;
//	const float hFit = (float) 1080 / h;
//	
//	const float wNew = w * std::min( wFit, hFit );
//	const float hNew = h * std::min( wFit, hFit );
//	
//	image.Rescale( wNew, hNew, wxImageResizeQuality::wxIMAGE_QUALITY_HIGH );
		
} // end method
