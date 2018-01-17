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
#include <deque>

#include <wx/timer.h>

#include "rsyn/gui/CanvasGL.h"
#include "rsyn/session/Session.h"
#include "rsyn/util/Color.h"
#include "rsyn/util/Json.h"
#include "rsyn/util/float2.h"
#include "rsyn/model/timing/Timer.h"



BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(myEVT_SCHEMATIC_CELL_SELECTED, -1)
BEGIN_DECLARE_EVENT_TYPES()
 
// -----------------------------------------------------------------------------

class NVGcontext;

class SchematicCanvasGL : public CanvasGL {
private:

	Rsyn::Session clsSession;

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
	void drawNor(float x, float y, float w, float h);
	void drawAnd(float x, float y, float w, float h);
	void drawOr(float x, float y, float w, float h);
	void drawInv(float x, float y, float w, float h);
	void drawXor(float x, float y, float w, float h);
	void drawXnor(float x, float y, float w, float h);
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

	// Attach the session.
	void attachSession(Rsyn::Session session);
}; // end class

// -----------------------------------------------------------------------------
class VisualInstancePin{
public:
	Rsyn::Pin pin;
	DBUxy pos;
	//VisualInstancePin();
};

class VisualNet{
private:
	DBUxy pinOut;
	DBUxy pinIn;
public:
	VisualNet(DBUxy pinO, DBUxy pinI){ pinOut = pinO; pinIn = pinI;}
	DBUxy getPOut(){return pinOut;}
	DBUxy getPIn(){return pinIn;}
};


class VisualInstance{
private:
	Rsyn::Instance inst;
	Bounds bounds;
	std::vector<VisualInstancePin> pins;
	Color color;
	
public:
	Rsyn::Instance getInst(){return inst;}
	const Bounds & getBounds() const {return bounds;}
	std::vector<VisualInstancePin> getPins(){return pins;}
	Color getColor(){return color;}
	void setInst(Rsyn::Instance instance){inst = instance;}
	void setBounds(DBU posXmin, DBU posYmin, DBU posXmax, DBU posYmax){bounds = Bounds(posXmin, posYmin, posXmax, posYmax);}
	void setColor (unsigned char r, unsigned char g, unsigned char b){color.setRGB(r, g, b);}
	void addPin (VisualInstancePin pin) {pins.push_back(pin);}

}; //end class


class NewSchematicCanvasGL : public CanvasGL {
private:
	
	static const float LAYER_GRID;
	static const float LAYER_SHAPE_FILLING;
	static const float LAYER_SHAPES;
	static const float LAYER_SELECTED;
	static const int SAME_LEVEL = 0;
	static const int NEXT_LEVEL = 1;
	static const DBU ROW_SIZE;
	static const DBU LVL_SIZE;
	static const DBU CELL_SIZE;
	static const DBU SPACE_SIZE;
	
	bool clsDrawCriticalPath : 1;
	bool clsDrawSelectedCell : 1;
	bool clsDrawNeighborCells : 1;
	bool clsDrawLogicCone : 1;
	bool clsIsSelected : 1;
	bool clsFirstRendering : 1;
	bool clsDrawCPChanged : 1;
	
	int clsNumPathsToDraw = 1;
	
	float2 clsMousePosition;
	
	Rsyn::Session clsSession;
	Rsyn::Timer * timer;
	Rsyn::Design clsDesign;
	Rsyn::Attribute<Rsyn::Instance, VisualInstance> clsVisualInst;
	Rsyn::Attribute<Rsyn::Pin, VisualInstancePin> clsVisualInstPin;
	
	VisualInstance selectedInstance;
	std::vector<std::vector<Rsyn::Timer::PathHop>> clsCriticalPaths;
	DBUxy finalPos;
	std::deque<Rsyn::Instance> instancesOnScreen;
	std::deque<VisualNet> netsOnScreen;
	std::deque<std::vector<VisualInstance*>> overlap;
	

	void resetClickedView();

	void renderGrid(const int scaleGrid = CELL_SIZE);
	void defineInstancePos(Rsyn::Instance instance, int row, DBU & lastPos, int level);
	void insertCell(int index, int row, VisualInstance & cdInstance);
	void getNeighbours();
	void definePinPos(VisualInstance & cdInstance);
	void makeConnectionsCP(VisualInstancePin & pinOut, VisualInstancePin & pinIn);
	void makeConnectionsNeighbours(VisualInstance & cdInstance);
	void makeConnectionsLogicCone(VisualInstancePin & pinOut);
	void drawConnections(DBUxy posOut, DBUxy posIn);
	void drawConnectionsOnScreen();
	
	
	void renderExperimental();
	
	
	virtual void onMouseReleased(wxMouseEvent& event);
	virtual void onMouseDown(wxMouseEvent& event);
	virtual void onMouseMoved(wxMouseEvent& event);
	virtual void selectCellAt();
	void renderSelectedCell();
	void getCriticalPath();
	void defineCriticalPathPos();
	void drawInstance(Rsyn::Instance instance);
	void drawPin(VisualInstance & inst);
	void drawInstancesOnScreen();
	void defineNeighboursPos(const VisualInstance & selectedInst, Rsyn::Pin selectedPin, std::vector<Rsyn::Pin> & neighbours);
	void getLogicCone(VisualInstance selectedInst, Rsyn::Pin selectedPin);
	


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
	void init();
	
	// Events.
	virtual void onRender(wxPaintEvent& evt);

	// Attach the session.
	void attachSession(Rsyn::Session session);
}; // end class


#endif
