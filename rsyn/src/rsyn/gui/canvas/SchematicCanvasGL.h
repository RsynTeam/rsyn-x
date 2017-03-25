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
 
#ifndef SCHEMATIC_CANVAS_GL_H
#define	SCHEMATIC_CANVAS_GL_H

#include <map>
#include <vector>

#include <wx/timer.h>

#include "rsyn/gui/CanvasGL.h"
#include "rsyn/engine/Engine.h"
#include "rsyn/util/Color.h"
#include "rsyn/3rdparty/json/json.hpp"

// -----------------------------------------------------------------------------

class NVGcontext;

class SchematicCanvasGL : public CanvasGL {
private:

	Rsyn::Engine clsEngine;

	// NanoVG
	NVGcontext* vg = nullptr;
	bool initialized = false;
	wxTimer * timer = nullptr;
	
	float boardWidth;
	float boardHeight;
	
	float mouseX;
	float mouseY;
	
	void drawEyes(float x, float y, float w, float h, float mx, float my, float t);
	void drawScissor(float x, float y, float t);
	void drawWidths(float x, float y, float width);
	void drawCaps(float x, float y, float width);
	void drawColorwheel(float x, float y, float w, float h, float t);
	void drawLines(float x, float y, float w, float h, float t);
	void drawGraph(float x, float y, float w, float h, float t);
	void drawNand(float x, float y, float w, float h);
	void drawWindow(const char * title, float x, float y, float w, float h);
	void renderDemo(float mx, float my, float width, float height, float t, bool blowup);

	// Rendering	
	void render();
	void reset();

public:
	SchematicCanvasGL(wxWindow* parent);
	virtual ~SchematicCanvasGL();

	// Events.
	virtual void onRender(wxPaintEvent& evt);
	virtual void onTimeout(wxTimerEvent& event);
	//virtual void onMouseReleased(wxMouseEvent& event);
	virtual void onMouseMoved(wxMouseEvent& event);
	//virtual void onMouseDown(wxMouseEvent& event);
	
	void resetCamera() {
		CanvasGL::resetCamera();

		// In addition to reseting the camera, we need to adjust also the board
		// size of the demo accordingly. Note this is only necessary because
		// the demo places some shapes w.r.t. the right and top edges.
		boardWidth = getViewportWidth();
		boardHeight = getViewportHeight();
		
		// The y-dimension is shifted to counter interact with the inverted
		// CanvasGL.		
		resetCamera(0, -boardHeight, boardWidth,  0);		
	} // end method
	
	virtual void resetCamera(const float minx, const float miny, const float maxx, const float maxy) {
		CanvasGL::resetCamera(minx, miny, maxx, maxy);
	} // end method

	// Attach the engine.
	void attachEngine(Rsyn::Engine engine);
}; // end class


#endif
