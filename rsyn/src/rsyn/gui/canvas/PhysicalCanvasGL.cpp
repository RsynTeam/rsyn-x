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
 
#include "PhysicalCanvasGL.h"

#include "rsyn/phy/PhysicalService.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/model/routing/RoutingEstimator.h"

#include "rsyn/util/float2.h"
#include "rsyn/util/Environment.h"
#include "rsyn/util/Colorize.h"
#include "rsyn/util/Polygon.h"
#include "rsyn/util/FloatRectangle.h"

#include <iostream>
using std::cout;
using std::cerr;
#include <random>
#include <complex>

// -----------------------------------------------------------------------------

DEFINE_LOCAL_EVENT_TYPE(myEVT_CELL_SELECTED)
DEFINE_LOCAL_EVENT_TYPE(myEVT_BIN_SELECTED)
DEFINE_LOCAL_EVENT_TYPE(myEVT_CELL_DRAGGED)

// -----------------------------------------------------------------------------

const float PhysicalCanvasGL::LAYER_BACKGROUND	= 0.0f;
const float PhysicalCanvasGL::LAYER_OBSTACLES	= 0.1f;
const float PhysicalCanvasGL::LAYER_GRADIENTS	= 0.2f;
const float PhysicalCanvasGL::LAYER_MOVABLE	= 0.3f;
const float PhysicalCanvasGL::LAYER_GRID	= 0.4f;
const float PhysicalCanvasGL::LAYER_METAL	= 0.5f;
const float PhysicalCanvasGL::LAYER_FOREGROUND	= 1.0f;

// -----------------------------------------------------------------------------

PhysicalCanvasGL::PhysicalCanvasGL(wxWindow* parent) : CanvasGL(parent) {
	reset();
} // end constructor

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::reset() {
	clsEnableLegalizerInfo = true;
	
	clsViewGrid = false;
	clsViewWarpedGrid = false;
	clsViewRows = false;
	clsViewCoreBounds = true;
	clsViewOccupancyRate = false;
	clsViewPoissonSolution = false;
	clsViewDemandMap = false;
	clsViewSupplyMap = false;
	clsViewGradients = false;
	clsViewNets = false;
	clsViewLateCriticalPath = true;
	clsViewEarlyCriticalPath = true;
	clsViewCriticalNets = false;
	clsViewFanoutTrees = true;
	clsViewFaninTrees = true;
	clsViewRenderBlockages = false;
	clsInterpolatedSnapshot = false;
	clsViewDisplacement = false;

	clsCriticalPathWidth = 5.0;
	
	clsViewSelectedNodeNets = true;
	clsViewSelectedNodeNeighbours = true;
	clsViewSelectedNodeOptimalRegion = true;

	// Selection
	clsSelectedCell = nullptr;
	clsSelectedCellOffset = 0;
	
	clsSelectedBinIndex = -1;
	clsDeltaXY = DBUxy(0);
	clsSelectedCellDragging = false;
	clsInterpolationValue = 0;
	
} // end constructor

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::attachEngine(Rsyn::Engine engine) {
	reset();

	clsEnginePtr = engine;
	timer = clsEnginePtr.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);
	routingEstimator = clsEnginePtr.getService("rsyn.routingEstimator", Rsyn::SERVICE_OPTIONAL);
	physical = clsEnginePtr.getService("rsyn.physical");
	
	design = clsEnginePtr.getDesign();
	module = design.getTopModule();
	phDesign = physical->getPhysicalDesign();
	
	clsCheckpoint = design.createAttribute();
	storeCheckpoint();

	Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
	const FloatRectangle &bound = phDie.getBounds();
	FloatRectangle r = bound;
	r.scaleCentralized(1.01f);
	resetCamera(r[LOWER][X], r[LOWER][Y], r[UPPER][X],r[UPPER][Y]);

	renderCoreBounds();
	
	Refresh();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::CentralizeAtSelectedCell() {
	if (!clsSelectedCell)
		return;
	Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(clsSelectedCell);
	const DBUxy pos = phCell.getCenter();

	zoomTo(pos.x, pos.y, std::min(1/8.0f, getZoom()), true);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::SelectCell(Rsyn::Cell cell) {
	clsSelectedCell = cell;

	wxCommandEvent event(myEVT_CELL_SELECTED); // No specific id
	//event.SetString(wxT("This is the data"));
	wxPostEvent(this, event); // to ourselves

	Refresh();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::SelectBin(const int binIndex) {
	clsSelectedBinIndex = binIndex;
	wxCommandEvent event(myEVT_BIN_SELECTED);
	wxPostEvent(this, event);
	Refresh();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::storeCheckpoint() {
	if (!clsEnginePtr)
		return;
	
	Rsyn::Design design = clsEnginePtr.getDesign();
	for (Rsyn::Instance instance : module.allInstances()) {
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
		clsCheckpoint[cell] = phCell.getPosition();
	} // end for
} // end method

// -----------------------------------------------------------------------------

Rsyn::Cell PhysicalCanvasGL::selectCellAt(const float x, const float y, const bool keepSelection) {
	std::vector<Rsyn::Cell> candidates;
	for (Rsyn::Instance instance : module.allInstances()) {
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell		
		const Rsyn::PhysicalCell &ph = phDesign.getPhysicalCell(cell);
		const float2 displacement = getInterpolatedDisplacement(cell);
			
		if(instance.isPort()) {
			const FloatRectangle & portBounds = ph.getBounds();
			if( portBounds.inside(x+displacement[X], y+displacement[Y]))
				candidates.push_back(cell);
		} else if (instance.isMacroBlock() && ph.hasLayerBounds()) {
			const Rsyn::PhysicalLibraryCell &phLibCell = phDesign.getPhysicalLibraryCell(cell);		
			for (const FloatRectangle &obs : phLibCell.allLayerObstacles()) {
				FloatRectangle rect = obs;
				float2 lower = obs.getCoordinate(LOWER);
				rect.moveTo(float2(ph.getPosition().x, ph.getPosition().y) + lower);
				rect.translate(displacement);
				if (rect.inside(x, y)) {
					candidates.push_back(cell);
				} // end if
			} // end for
		} else {
			FloatRectangle bounds = ph.getBounds();
			bounds.translate(displacement);
			if (bounds.inside(x, y)) {
				candidates.push_back(cell);
			} // end if
		}// end if 
	} // end for

	// Selected one cell among the candidates.
	// Every time a cell is requested at (x, y), a different one is returned if
	// there are more than one at same (x, y). This allows user to select cells
	// behind others.
	
	Rsyn::Cell selectedCell = nullptr;
	if (!candidates.empty()) {
		if (keepSelection) {
			// If the current selected cell is a candidate, keep it as the
			// selected cell.
			for (Rsyn::Cell candidate : candidates) {
				if (candidate == clsSelectedCell) {
					selectedCell = candidate;
					break;
				} // end if
			} // end for
		} // end if
		
		if (!selectedCell) {
			// Loop until we find a candidate cell that is not the current one if 
			// any.
			for (int i = 0; i < candidates.size(); i++) {
				selectedCell = candidates[clsSelectedCellOffset % candidates.size()];
				clsSelectedCellOffset++;
				if (selectedCell != clsSelectedCell)
					break;
			} // end for
		} // end if
	} // end if
	
	// Raise selected cell event.
	SelectCell(selectedCell);
	
	// Store the reference point.
	if (selectedCell) {
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(selectedCell);
		const DBUxy sourcePosition = phCell.getPosition();
		DBUxy targetPosition(float2(x, y).convertToDbu());
		clsDeltaXY = targetPosition - sourcePosition;
	} // end if		

	// Indicates that a selection was performed.
	clsSelectedCellDirty = false;
	
	return selectedCell;
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::onMouseDown(wxMouseEvent& event) {
	if (!clsEnginePtr)
		return;

	clsSelectedCellDragging = false;
	clsSelectedCellDirty = false;
	
	if (event.LeftDown()) {
		clsMouseDown = true;
		clsDragStartX = event.m_x;
		clsDragStartY = event.m_y;

		if (event.ShiftDown()) {
			clsSelectedCellDragging = true;
			clsSelectedCellDirty = true;			
		} else if (event.ControlDown()) {
		} // end else if
	} // end if
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::onMouseReleased(wxMouseEvent& event) {
	if (!clsEnginePtr)
		return;
	
//**************TEMP**************	
	clsMousePosition[X] = translateFromViewportToUserX(event.m_x);
	clsMousePosition[Y] = translateFromViewportToUserY(event.m_y);
//**************TEMP**************
	
	if (clsSelectedCellDirty) {
		const float currentMousePositionX = translateFromViewportToUserX(event.m_x);
		const float currentMousePositionY = translateFromViewportToUserY(event.m_y);
		selectCellAt(currentMousePositionX, currentMousePositionY, false);
		clsSelectedCellDragging = false;
	} // end if
	
	CanvasGL::onMouseReleased(event);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::onMouseMoved(wxMouseEvent& event) {
	if (!clsEnginePtr || !clsMouseDown)
		return;

	const float x1 = translateFromViewportToUserX(event.m_x);
	const float y1 = translateFromViewportToUserY(event.m_y);
	const float x0 = translateFromViewportToUserX(clsDragStartX);
	const float y0 = translateFromViewportToUserY(clsDragStartY);

	if (clsSelectedCellDirty) {
		selectCellAt(x1, y1, true);
	} // end if	
	
	if (!clsSelectedCellDragging) {
		clsDragging = true;

		const float dx = x1 - x0;
		const float dy = y1 - y0;

		clsMinX -= dx;
		clsMinY -= dy;
		clsMaxX -= dx;
		clsMaxY -= dy;

		clsDragStartX = event.m_x;
		clsDragStartY = event.m_y;
	} else {
		if (clsSelectedCell && clsInterpolationValue == 0) {
			phDesign.placeCell(clsSelectedCell, DBUxy(float2(x1, y1).convertToDbu()) - clsDeltaXY);
			wxCommandEvent event(myEVT_CELL_DRAGGED);
			wxPostEvent(this, event);
		} // end if
	} // end else

	Refresh();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::drawArrow(const float x0, const float y0, const float x1, const float y1, const float z) {
	const float PI = 3.14f;

	const float dx = x1 - x0;
	const float dy = y1 - y0;

	float theta;

	if (fabs(dx) < 1e-3 && fabs(dy) < 1e-3)
		return;

	if (dx == 0)
		theta = dy > 0 ? PI / 2 : -PI / 2;
	else if (dy == 0)
		theta = dx > 0 ? 0 : PI;
	else {
		if (dx < 0) {
			theta = PI + atan(dy / dx);
		} else {
			theta = atan(dy / dx);
		} // end else
	} // end else

	const float scalex = getGraphicalPinWidth();
	const float scaley = getGraphicalPinWidth();

	// ( 0.5, 0 )
	const float px0 = x1 + scalex * (0.5f * cos(theta) - 0.0f * sin(theta));
	const float py0 = y1 + scaley * (0.5f * sin(theta) + 0.0f * cos(theta));

	// ( -0.5, 0.25 )
	const float px1 = x1 + scalex * (-0.5f * cos(theta) - 0.25f * sin(theta));
	const float py1 = y1 + scaley * (-0.5f * sin(theta) + 0.25f * cos(theta));

	// ( -0.5, -0.25 )
	const float px2 = x1 + scalex * (-0.5f * cos(theta) - -0.25f * sin(theta));
	const float py2 = y1 + scaley * (-0.5f * sin(theta) + -0.25f * cos(theta));

	glVertex3d(px0, py0, z); // top
	glVertex3d(px1, py1, z); // bottom left
	glVertex3d(px2, py2, z); // bottom right	
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::drawPin(Rsyn::Pin pin) {
	const float alpha = 0.7f;
	Rsyn::Design design = clsEnginePtr.getDesign();
	
	FloatRectangle bounds = getGraphicalPinBoundingBox(pin);
	bounds.translate(getInterpolatedDisplacement(pin.getInstance()));
	
	const float xmin = bounds.getCoordinate(LOWER, X);
	const float ymin = bounds.getCoordinate(LOWER, Y);
	const float xmax = bounds.getCoordinate(UPPER, X);
	const float ymax = bounds.getCoordinate(UPPER, Y);
	const float xmdr = xmin + alpha*bounds.computeLength(X);
	const float xmdl = xmin + (1-alpha)*bounds.computeLength(X);
	const float ymid = ymin + bounds.computeLength(Y) / 2.0f;

	// [NOTE] OpenGl only draws convex filled polygons.
	
	switch (pin.getDirection()) {
		case Rsyn::IN:
			// Top-left triangle
			glBegin(GL_TRIANGLES);
				glVertex3d(xmin, ymax, LAYER_FOREGROUND);
				glVertex3d(xmdl, ymax, LAYER_FOREGROUND);
				glVertex3d(xmdl, ymid, LAYER_FOREGROUND);
			glEnd();	
			
			// Bottom-left triangle
			glBegin(GL_TRIANGLES);
				glVertex3d(xmin, ymin, LAYER_FOREGROUND);
				glVertex3d(xmdl, ymin, LAYER_FOREGROUND);
				glVertex3d(xmdl, ymid, LAYER_FOREGROUND);
			glEnd();			
			
			// Body
			glBegin(GL_POLYGON);
				glVertex3d(xmdl, ymin, LAYER_FOREGROUND);
				glVertex3d(xmdr, ymin, LAYER_FOREGROUND);
				glVertex3d(xmax, ymid, LAYER_FOREGROUND);
				glVertex3d(xmdr, ymax, LAYER_FOREGROUND);
				glVertex3d(xmdl, ymax, LAYER_FOREGROUND);
				glVertex3d(xmdl, ymin, LAYER_FOREGROUND);
			glEnd();
			
			break;
			
		case Rsyn::OUT:
			glBegin(GL_POLYGON);
				glVertex3d(xmin, ymin, LAYER_FOREGROUND);
				glVertex3d(xmdr, ymin, LAYER_FOREGROUND);
				glVertex3d(xmax, ymid, LAYER_FOREGROUND);
				glVertex3d(xmdr, ymax, LAYER_FOREGROUND);
				glVertex3d(xmin, ymax, LAYER_FOREGROUND);
				glVertex3d(xmin, ymin, LAYER_FOREGROUND);
			glEnd();			
			
			break;
			
		case Rsyn::BIDIRECTIONAL:
			
			// left triangle
			glBegin(GL_TRIANGLES);
				glVertex3d(xmdl, ymin, LAYER_FOREGROUND);
				glVertex3d(xmin, ymid, LAYER_FOREGROUND);
				glVertex3d(xmdl, ymax, LAYER_FOREGROUND);
			glEnd();	
			
			// Body
			glBegin(GL_QUADS);
				glVertex3d(xmdl, ymin, LAYER_FOREGROUND);
				glVertex3d(xmdr, ymin, LAYER_FOREGROUND);
				glVertex3d(xmdr, ymax, LAYER_FOREGROUND);
				glVertex3d(xmdl, ymax, LAYER_FOREGROUND);
			glEnd();			
			
			// Right triangle
			glBegin(GL_TRIANGLES);
				glVertex3d(xmdr, ymin, LAYER_FOREGROUND);
				glVertex3d(xmin, ymid, LAYER_FOREGROUND);
				glVertex3d(xmdr, ymax, LAYER_FOREGROUND);
			glEnd();			
			
			break;

		case Rsyn::UNKNOWN_DIRECTION :
			cout<<"TODO PIN_DIRECTION_UNKNOWN in "<<__func__<<" at "<<__FILE__<<"\n";
			break;
		default:
			// It is missing handle pin direction NUM_PIN_DIRECTIONS
			break;
	} // end switch
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderCoreBounds() {

	if (!clsEnginePtr)
		return;
	Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
	const FloatRectangle &bounds = phDie.getBounds();
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	glLineStipple(3, 0xAAAA);
	//	glEnable(GL_LINE_STIPPLE);	

	glBegin(GL_QUADS);
	glColor3ub(0, 0, 0);
	glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], LAYER_BACKGROUND);
	glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], LAYER_BACKGROUND);
	glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], LAYER_BACKGROUND);
	glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], LAYER_BACKGROUND);
	glEnd();

	//glDisable(GL_LINE_STIPPLE);
	glLineWidth(1);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderRows() {
	if(!clsEnginePtr)
		return;
	
	//float lowerX, upperX, lowerY, upperY;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3ub(100, 100, 100);

	for (const Rsyn::PhysicalRow & row : phDesign.allPhysicalRows()) {
		DBUxy lower = row.getCoordinate(LOWER);
		DBUxy upper = row.getCoordinate(UPPER);
		
		glVertex3d(lower[X], lower[Y], LAYER_BACKGROUND);
		glVertex3d(upper[X], lower[Y], LAYER_BACKGROUND);
		glVertex3d(upper[X], upper[Y], LAYER_BACKGROUND);
		glVertex3d(lower[X], upper[Y], LAYER_BACKGROUND);
	} // end for

	glEnd();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderCriticalPath_Mode(
		const Rsyn::TimingMode mode,
		const GLubyte r,
		const GLubyte g,
		const GLubyte b
		) {

	Rsyn::Design design = clsEnginePtr.getDesign();
	
	std::vector<Rsyn::Timer::PathHop> criticalPath;
	timer->queryTopCriticalPath(mode, criticalPath);

	if (criticalPath.empty())
		return;

	glLineWidth( clsCriticalPathWidth );
	glBegin(GL_LINE_STRIP);
	glColor3ub(r, g, b);

	for (auto & hop : criticalPath) {
		Rsyn::Pin pin = hop.getPin();
		Rsyn::Net net = pin.getNet();
		
		if (net == timer->getClockNet())
			continue;

		float2 pos;
		if (clsInterpolationValue > 0) {
			Rsyn::Cell cell  = pin.getInstance().asCell();
			Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
			const DBUxy p0 = phCell.getPosition();
			const DBUxy p1 = clsCheckpoint[cell];
			const DBUxy dpin = phDesign.getPinDisplacement(pin);
			pos = interpolate(p0, p1, clsInterpolationValue) + float2(dpin);
		} else {
			pos = float2(phDesign.getPinPosition(pin));
		} // end else
		
		glVertex3d(pos.x, pos.y, LAYER_FOREGROUND);
	} // end for
	glEnd();
	glLineWidth(1.0);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderCriticalPath() {
	if (clsEnginePtr == nullptr)
		return;

	if (clsViewLateCriticalPath)
		renderCriticalPath_Mode(Rsyn::LATE, 255, 0, 0);

	if (clsViewEarlyCriticalPath)
		renderCriticalPath_Mode(Rsyn::EARLY, 0, 0, 255);		
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderPaths(Rsyn::TimingMode mode) {
	const float minSlack = timer->getWns(mode);
	const float maxSlack = 0;

	const float slackRange = maxSlack - minSlack;
	
	// Sort paths so that the most critical appears on top.
	std::vector<std::tuple<float, int>> sortedPaths(clsPaths[mode].size());
	for (int i = 0; i < clsPaths[mode].size(); i++) {
		const auto &path = clsPaths[mode][i];
		if (path.empty())
			continue; // defensive programming	
		
		const float slack = path.back().getSlack();
		sortedPaths[i] = std::make_tuple(slack, i);
	} // end method
	
	std::sort(sortedPaths.begin(), sortedPaths.end());
	
	// Render paths.
	for (int i = 0; i < sortedPaths.size(); i++) {
		const auto &path = clsPaths[mode][std::get<1>(sortedPaths[i])];
		if (path.empty())
			continue; // defensive programming		
		
		const float slack = path.back().getSlack();
		
		int r, g, b;
		Colorize::colorTemperature(1 - (slack - minSlack)/slackRange, r, g, b);
		glColor3ub(r, g, b);		
		renderPath(path);
	} // end for
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderPath(const std::vector<Rsyn::Timer::PathHop> &path) {
	glLineWidth(2.5);
	glBegin(GL_LINE_STRIP);

	Rsyn::Design design = clsEnginePtr.getDesign();
	Rsyn::Net clockNet = timer->getClockNet();
	
	for (auto & hop : path) {
		if (hop.getPin().getNet() == clockNet)
			continue;

		Rsyn::Pin pin = hop.getPin();
		
		float2 pos;
		if (clsInterpolationValue > 0) {
			Rsyn::Cell cell  = pin.getInstance().asCell();
			Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
			const DBUxy p0 = phCell.getPosition();
			const DBUxy p1 = clsCheckpoint[cell];
			const DBUxy dpin = phDesign.getPinDisplacement(pin);
			pos = interpolate(p0, p1, clsInterpolationValue) + float2(dpin);
		} else {
			pos = float2(phDesign.getPinPosition(pin));
		} // end else
		
		glVertex3d(pos.x, pos.y, LAYER_FOREGROUND);
	} // end for

	glEnd();
	glLineWidth(1.0);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderCriticalNets() {
	if (!routingEstimator || !timer)
		return;	
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	
	const float circuitWNS = timer->getWns( Rsyn::TimingMode::LATE );

	Rsyn::Design design = clsEnginePtr.getDesign();
	for(  auto net: module.allNets()  ) {
		auto driver = net.getAnyDriver();
		float wns = timer->getPinWorstNegativeSlack( driver, Rsyn::TimingMode::LATE );
		
		if( !wns )
			continue;
		
		int r, g, b;
		Colorize::colorTemperature( wns / circuitWNS , r, g, b );
		
		glLineWidth(2.0);	
		glColor4ub(r, g, b, 200);
		
		const Rsyn::RCTree & tree = routingEstimator->getRCTree(net);
		glBegin(GL_LINES);
		const int numNodes = tree.getNumNodes();
		for (int i = 1; i < numNodes; i++) { // start @ 1 to skip root node
			const Rsyn::RCTreeNodeTag &tag0 = tree.getNodeTag(tree.getNode(i).propParent);
			const Rsyn::RCTreeNodeTag &tag1 = tree.getNodeTag(i);
			
			glVertex3d(tag0.x, tag0.y, LAYER_FOREGROUND);
			glVertex3d(tag1.x, tag1.y, LAYER_FOREGROUND);
		} // end for
		glEnd();
		
		glLineWidth(1.0);
	} // end for
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderSelectedCell() {

	if (!clsEnginePtr || !clsSelectedCell)
		return;

	////////////////////////////////////////////////////////////////////////////
	// Highlight selected cell.
	////////////////////////////////////////////////////////////////////////////	
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3);
	glBegin(GL_QUADS);

	glColor3ub(255, 0, 0);
	
	const float layer = LAYER_FOREGROUND;
	const Rsyn::PhysicalCell &phCell = phDesign.getPhysicalCell(clsSelectedCell);
	float2 disp = getInterpolatedDisplacement(clsSelectedCell);

	if (clsSelectedCell.isPort()) {
		const FloatRectangle & bounds = phCell.getBounds();
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
		glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
		glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
	} else {
		if (clsSelectedCell.isMacroBlock() && phCell.hasLayerBounds()) {
			const Rsyn::PhysicalLibraryCell &phLibCell = phDesign.getPhysicalLibraryCell(clsSelectedCell);
			for (const FloatRectangle &obs : phLibCell.allLayerObstacles()) {
				FloatRectangle bounds = obs;
				float2 lower = obs.getCoordinate(LOWER);
				bounds.moveTo(float2(phCell.getPosition()) + lower);
				glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
				glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
				glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
				glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
			} // end for 
		} else {
			const FloatRectangle & bounds = phCell.getBounds();
			glVertex3d(bounds[LOWER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
			glVertex3d(bounds[UPPER][X] + disp[X], bounds[LOWER][Y] + disp[Y], layer);
			glVertex3d(bounds[UPPER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
			glVertex3d(bounds[LOWER][X] + disp[X], bounds[UPPER][Y] + disp[Y], layer);
		} // end if-else 
	} // end if-else 

	glEnd();
	glLineWidth(1);
	
	////////////////////////////////////////////////////////////////////////////
	// Steiner Trees
	////////////////////////////////////////////////////////////////////////////
	if (timer) renderTree();

	////////////////////////////////////////////////////////////////////////////
	// Draw neighbors pins.
	////////////////////////////////////////////////////////////////////////////
	if (clsViewSelectedNodeNeighbours && clsSelectedCell) {
		Rsyn::Design design = clsEnginePtr.getDesign();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		
		glColor3ub(0, 0, 0);
				
		for (Rsyn::Pin sink : clsSelectedCell.allPins(Rsyn::IN)) {
			drawPin(sink);
			
			Rsyn::Net net = sink.getNet();
			if (net) {
				for (Rsyn::Pin pin : net.allPins()) {
					Rsyn::Cell cell  = pin.getInstance().asCell();
					if (cell != clsSelectedCell)
						drawPin(pin);
				} // end for
			} // end if
		} // end for

		for (Rsyn::Pin driver : clsSelectedCell.allPins(Rsyn::OUT)) {
			drawPin(driver);
			
			Rsyn::Net net = driver.getNet();
			if (net) {
				for (Rsyn::Pin pin : net.allPins(Rsyn::SINK)) {
					Rsyn::Cell cell  = pin.getInstance().asCell();
					if (cell != clsSelectedCell)
						drawPin(pin);						
				} // end for
			} // end if
		} // end for
	} // end if
	
	////////////////////////////////////////////////////////////////////////////
	// Highlight neighbors cells.
	////////////////////////////////////////////////////////////////////////////	
	if (clsViewSelectedNodeNeighbours && clsSelectedCell) {
		Rsyn::Design design = clsEnginePtr.getDesign();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(3);
		glColor3ub(0, 0, 0);
		
		glBegin(GL_QUADS);
				
		for (Rsyn::Pin sink : clsSelectedCell.allPins(Rsyn::IN)) {
			Rsyn::Net net = sink.getNet();
			if (!net)
				continue;
			
			for (Rsyn::Pin pin : net.allPins()) {
				Rsyn::Cell cell  = pin.getInstance().asCell();
				if (cell == clsSelectedCell)
					continue;
				
				Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
				const FloatRectangle & rect = phCell.getBounds();
				FloatRectangle bounds = rect;
				bounds.translate(getInterpolatedDisplacement(cell));
				glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], LAYER_FOREGROUND);
				glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], LAYER_FOREGROUND);
				glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], LAYER_FOREGROUND);
				glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], LAYER_FOREGROUND);
			} // end for
		} // end for

		for (Rsyn::Pin driver : clsSelectedCell.allPins(Rsyn::OUT)) {
			Rsyn::Net net = driver.getNet();
			if (!net)
				continue;
			
			for (Rsyn::Pin pin : net.allPins(Rsyn::SINK)) {
				Rsyn::Cell cell  = pin.getInstance().asCell();
				
				Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
				const FloatRectangle &rect = phCell.getBounds();
				FloatRectangle bounds = rect;
				bounds.translate(getInterpolatedDisplacement(cell));
				glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], LAYER_FOREGROUND);
				glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], LAYER_FOREGROUND);
				glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], LAYER_FOREGROUND);
				glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], LAYER_FOREGROUND);
			} // end for
			
			glEnd();
		} // end for
				
		glLineWidth(1);
	} // end if

} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderTree() {
	if (!clsSelectedCell || clsInterpolationValue > 0 || !routingEstimator)
		return;

	Rsyn::Design design = clsEnginePtr.getDesign();

	const int NUM_COLORS = 4;
	const static Color COLORS[NUM_COLORS] = {
		{0, 255, 255},
		{255, 0, 255},
		{255, 255, 0},
		{0, 0, 255},
	};
	
	int colorIndex = 0;
	
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glColor3ub(0, 0, 0);
	for (Rsyn::Pin pin : clsSelectedCell.allPins()) {
		if (!pin.isConnected())
			continue;

		Rsyn::Net net = pin.getNet();
		if (net.getNumPins() < 2)
			continue;

		if (pin.isInput() && !clsViewFaninTrees)
			continue;
		
		if (pin.isOutput() && !clsViewFanoutTrees)
			continue;
		
		if (pin.isInput()) { // Steiner tree for input pins are painted in black
			const Color &color = COLORS[colorIndex % NUM_COLORS];
			glColor3ub(color.r, color.g, color.b);
			colorIndex++;
		} else {
			glColor3ub(0, 0, 0);
		} // end else
			
		const Rsyn::RCTree & tree = routingEstimator->getRCTree(net);

		const int numNodes = tree.getNumNodes();
		for (int i = 1; i < numNodes; i++) { // start @ 1 to skip root node
			const Rsyn::RCTreeNodeTag &tag0 = tree.getNodeTag(tree.getNode(i).propParent);
			const Rsyn::RCTreeNodeTag &tag1 = tree.getNodeTag(i);

			glVertex3d(tag0.x, tag0.y, LAYER_FOREGROUND);
			glVertex3d(tag1.x, tag1.y, LAYER_FOREGROUND);
		} // end for
	} // end for 
	glEnd();
	glLineWidth(1.0);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderBlockages() {
	if( !clsViewRenderBlockages )
		return;
	
	// To be updated after new blockage infrastructure implementation... 
} // end method

// ----------------------------------------------------------------------------- 

void PhysicalCanvasGL::render(const int width, const int height) {
	prepare2DViewport(width, height);
		
	if (clsViewCoreBounds) renderCoreBounds();
		
	for ( CanvasOverlayConfiguration &config : clsOverlays ) {
		if (config.visible) {
			config.overlay->render(this);
		} // end if
	} // end for	
	
	renderSelectedCell();
	
	if (timer) renderTimerInfo();
	
	if (clsViewRows) renderRows();
	
	if (clsViewRenderBlockages) renderBlockages();
	
	if (clsViewRows)
		renderRows();
	
	if( clsViewDisplacement )
		renderDisplacementLines();
	
	glFlush();
	SwapBuffers();
	
	if (clsInterpolatedSnapshot) {
		wxImage image;
		snapshot(image);
		std::string path = design.getName() + "-" + 
				to_string((int)(clsInterpolationValue*10)) + ".png";
		wxString filename = wxString::FromUTF8(path.c_str());
		image.SaveFile(filename, wxBITMAP_TYPE_PNG);
	} // end if
	
	//Refresh();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderTimerInfo() {
	renderCriticalPath();
	renderPaths(Rsyn::LATE);
	renderPaths(Rsyn::EARLY);
	if ( clsViewCriticalNets ) renderCriticalNets();
} // end method 

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderDisplacementLines() {
	
	float maxDisp = 0;
	
	for (Rsyn::Instance instance : module.allInstances()) {
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell( cell );
		const DBUxy currPosition = phCell.getPosition( );
		const DBUxy lastPosition = clsCheckpoint[cell];
		
		const float disp = std::abs( currPosition.x - lastPosition.x ) +
							std::abs( currPosition.y - lastPosition.y );
		
		maxDisp = std::max( disp, maxDisp );
	}
	
	for (Rsyn::Instance instance : module.allInstances()) {
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell( cell );
		const DBUxy currPosition = phCell.getPosition();
		const DBUxy lastPosition = clsCheckpoint[cell];
		
		if( lastPosition == currPosition )
			continue;
		
		const float disp = std::abs( currPosition.x - lastPosition.x ) +
							std::abs( currPosition.y - lastPosition.y );
		
		int r, g, b;
		Colorize::colorTemperature( disp/maxDisp, r, g, b );		
		
		glColor3ub( r, g, b );
		glBegin(GL_LINES);
		glVertex3d(currPosition.x, currPosition.y, LAYER_FOREGROUND);
		glVertex3d(lastPosition.x, lastPosition.y, LAYER_FOREGROUND);
		glEnd();
	}
} // end method

////////////////////////////////////////////////////////////////////////////////
// Events
////////////////////////////////////////////////////////////////////////////////

void PhysicalCanvasGL::onRender(wxPaintEvent& evt) {
	if (!IsShown()) return;
	if (!clsEnginePtr) {
		clearViewport();
	} else {
		render();
	} // end else
} // end method

// -----------------------------------------------------------------------------

// [TODO] Temp function, will be improved and moved to CanvasGL.

void PhysicalCanvasGL::saveSnapshot(wxImage& image) {
// fbo_width and fbo_height are the desired width and height of the FBO.
// For Opengl <= 4.4 or if the GL_ARB_texture_non_power_of_two extension
// is present, fbo_width and fbo_height can be values other than 2^n for
// some integer n.

	float viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);

	const int w = getViewportWidth()*2;
	const int h = getViewportHeight()*2;
		
	const int fbo_width = w;
	const int fbo_height = h;
	
	 glewInit();
	
// Build the texture that will serve as the color attachment for the framebuffer.
GLuint color_renderbuffer;
glGenRenderbuffers(1, &color_renderbuffer);
glBindRenderbuffer( GL_RENDERBUFFER, (GLuint)color_renderbuffer );
glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8, fbo_width, fbo_height );
glBindRenderbuffer( GL_RENDERBUFFER, 0 );

// Build the texture that will serve as the depth attachment for the framebuffer.
GLuint depth_renderbuffer;
glGenRenderbuffers(1, &depth_renderbuffer);
glBindRenderbuffer( GL_RENDERBUFFER, (GLuint)depth_renderbuffer );
glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbo_width, fbo_height );
glBindRenderbuffer( GL_RENDERBUFFER, 0 );

// Build the framebuffer.
GLuint framebuffer;
glGenFramebuffers(1, &framebuffer);
glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)framebuffer);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_renderbuffer);

GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "error\n";
	return;
} // end if

render(w, h);


		GLubyte * pixels = new GLubyte[w * h * 4];
		//glReadBuffer(GL_FRONT);
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

	
glBindFramebuffer(GL_FRAMEBUFFER, 0);	
} // end method

// -----------------------------------------------------------------------------

//Temp 

float2 PhysicalCanvasGL::getMousePosition(){

	return clsMousePosition;

} //end method