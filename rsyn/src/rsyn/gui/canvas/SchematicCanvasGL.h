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
#include "rsyn/util/float2.h"
#include "rsyn/util/Color.h"
#include "rsyn/model/timing/Timer.h"


BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(myEVT_SCHEMATIC_CELL_SELECTED, -1)
BEGIN_DECLARE_EVENT_TYPES()
 
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

// -----------------------------------------------------------------------------
class PinPosition{
public:
	Rsyn::Pin pin;
	DBUxy pos;
	PinPosition();
};


class VisualInstance{
private:
	Rsyn::Instance inst;
	Bounds bounds;
	std::vector<PinPosition> pins;
	Color color;
	
public:
	bool init;
	
	VisualInstance(){init = false;}
	Rsyn::Instance getInst(){return inst;}
	Bounds getBounds(){return bounds;}
	std::vector<PinPosition> getPins(){return pins;}
	Color getColor(){return color;}
	void setInst(Rsyn::Instance instance){inst = instance;}
	void setBounds(DBU posXmin, DBU posYmin, DBU posXmax, DBU posYmax){bounds = Bounds(posXmin, posYmin, posXmax, posYmax);}
	void setColor (unsigned char r, unsigned char g, unsigned char b){color.setRGB(r, g, b);}

}; //end class






class NewSchematicCanvasGL : public CanvasGL {
private:
	
	bool clsDrawCriticalPath : 1;
	bool clsDrawSelectedCell : 1;
	bool clsDrawNeighborCells : 1;
	bool clsDrawLogicCone : 1;
	
	int clsNumPathsToDraw = 1;
	
	float2 clsMousePosition;
	bool clsIsSelected;
	Rsyn::Engine clsEngine;
	Rsyn::Timer * timer;
	Rsyn::Design clsDesign;
	Rsyn::Attribute<Rsyn::Instance, VisualInstance> clsVisualInst;
	bool clsFirstRendering = true;
//	std::vector<Shape *> clsShapes;
	std::vector<VisualInstance> clsInstances;
	VisualInstance selectedInstance;
	std::vector<std::vector<Rsyn::Timer::PathHop>> clsPaths;
	DBUxy finalPos;
	

	static const float LAYER_GRID;
	static const float LAYER_SHAPE_FILLING;
	static const float LAYER_SHAPES;
	static const float LAYER_SELECTED;

	void resetClickedView();
	
//	class Shape {
//	protected:
//		int x = 0;
//		int y = 0;
//		int w = 0;
//		int h = 0;
//	public:
//
//		Shape(const int width, const int height) : w(width), h(height) {}
//
//		virtual void render() = 0;		
//		int getX() const { return x; }
//		int getY() const { return y; }
//		int getWidth() const { return w; }
//		int getHeight() const { return h; }
//		void move(const int x, const int y) { this->x = x; this->y = y; }
//	}; // end class
//
//	class Box : public Shape {
//	public:
//		Box() : Shape(4, 4) {}
//		virtual void render() override;
//	}; // end class
//
//	class Line : public Shape {
//	private:
//
//	public:
//		Line() : Shape(0, 0) {}
//		virtual void render() override;
//		void set(const int x0, const int y0, const int x1, const int y1) {
//			this->x = std::min(x0, x1);
//			this->y = std::min(y0, y1);
//			this->w = std::abs(x1 - x0);
//			this->h = std::abs(y1 - y0);
//		} // end method
//	}; // end class



//	void renderShapes();
	void renderGrid();
	void defineInstancePos(Rsyn::Instance instance, DBUxy & lastPos, int y);
	void openNextCells();

	void renderExperimental();
	


public:
	NewSchematicCanvasGL(wxWindow* parent);

	void setViewCriticalPaths(const bool visible);
	void setViewSelectedCell(const bool visible);
	void setViewNighborCells(const bool visible);
	void setViewLogicCone(const bool visible);
	void setNumPathsToDraw(const int numPaths);
	
	Rsyn::Instance getSelectedInstance() {
		return selectedInstance.getInst();
	}
	
	virtual ~NewSchematicCanvasGL();
	void drawInstance(Rsyn::Instance instance);
	virtual void onMouseReleased(wxMouseEvent& event);
	virtual void onMouseDown(wxMouseEvent& event);
	virtual void onMouseMoved(wxMouseEvent& event);
	virtual void selectCellAt();
	void renderSelectedCell();
	void init();
	void criticalPath();
	void definePathPos();
	void drawPath();
	
	// Events.
	virtual void onRender(wxPaintEvent& evt);

	// Attach the engine.
	void attachEngine(Rsyn::Engine engine);
}; // end class


#endif
