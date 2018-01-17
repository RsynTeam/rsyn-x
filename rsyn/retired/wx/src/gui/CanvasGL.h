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
 
#ifndef CIRCUIT_CANVAS_GL_H
#define	CIRCUIT_CANVAS_GL_H

#ifdef __WXMAC__
#include <GL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <wx/image.h>
#include <wx/glcanvas.h>
#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif


class CanvasGL : public wxGLCanvas {
//private:
protected:
	static int CANVAS_GL_ARGS[];
	
    wxGLContext * clsContext;
 
	wxSize clsViewportSize;
	
	float clsMinX;
	float clsMinY;
	float clsMaxX;
	float clsMaxY;
	
	float clsOriginalMinX;
	float clsOriginalMinY;
	float clsOriginalMaxX;
	float clsOriginalMaxY;
		
	float clsZoom;
	float clsZoomScaling;

	bool clsMouseDown;
	
	int clsDragStartX;
	int clsDragStartY;
	bool clsDragging;
    bool clsMoving;
	
	void doMoveTo(const float x, const float y);
	void doMoveCenterTo(const float x, const float y);
	void doZoomTo(const float x, const float y, const float scaling);
	
	void adjustAspectRatio();
	
public:
	CanvasGL(wxWindow* parent);
	virtual ~CanvasGL();
    
	// State
	bool isDragging() const { return clsDragging; }
	
	// Events
	virtual void onMouseMoved(wxMouseEvent& event);
	virtual void onMouseDown(wxMouseEvent& event);
	virtual void onMouseWheelMoved(wxMouseEvent& event);
	virtual void onMouseReleased(wxMouseEvent& event);
	virtual void onLeftDoubleClick(wxMouseEvent& event);
	virtual void onRightClick(wxMouseEvent& event);
	virtual void onMouseLeaveWindow(wxMouseEvent& event);
	virtual void onKeyPressed(wxKeyEvent& event);
	virtual void onKeyReleased(wxKeyEvent& event);
	virtual void onResized(wxSizeEvent& evt);
    
	virtual void onRender(wxPaintEvent& evt) = 0;	
	
	// Camera
	void resetCamera();
	void resetCamera(const float minx, const float miny, const float maxx, const float maxy);
	void moveTo(const float x, const float y);
	void moveCenterTo(const float x, const float y);
	void zoomTo(const float x, const float y, const float scaling, const bool centralize = true);
	void zoom(const float scaling);
	
	// Viewport
	void prepare2DViewport(const int width = 0, const int height = 0, const bool clear = true);
	void clearViewport();
	
	// Configuration
	// scaling=2 means that the zoom will be doubled during zoom in and halved
	// during zoom out.
	void setZoomScaling(const float scaling) { clsZoomScaling = scaling; }

	// Translate viewport (canvas) coordinates to space (user) coordinates.
	float translateFromViewportToUserX(const int viewportX) const;
	float translateFromViewportToUserY(const int viewportY) const;

	// Get the user space coordinates.
	float getMinX() const { return clsMinX; }
	float getMinY() const { return clsMinY; }
	float getMaxX() const { return clsMaxX; }
	float getMaxY() const { return clsMaxY; }
	
	// Get the center of user space.
	float getCenterX() const { return (clsMaxX + clsMinX) / 2.0f; }
	float getCenterY() const { return (clsMaxY + clsMinY) / 2.0f; }
	
	// Get space dimensions.
	float getSpaceWidth() const { return clsMaxX - clsMinX; }
	float getSpaceHeight() const { return clsMaxY - clsMinY; }
	
	// Get the dimension of the viewport (canvas).
	int getViewportWidth() const {return GetSize().x;}
	int getViewportHeight() const { return GetSize().y;}	
	
	// Get current zoom.
	float getZoom() const { return clsZoom; }

	// Snapshot
	void snapshot(wxImage& image) const;
	void snapshot2(wxImage& image);
};


#endif

