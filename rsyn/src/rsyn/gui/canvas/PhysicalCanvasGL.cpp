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

#include "rsyn/core/infra/RawPointer.h"

#include "rsyn/phy/PhysicalService.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/model/routing/RoutingEstimator.h"
#include "rsyn/io/Graphics.h"

#include "rsyn/util/float2.h"
#include "rsyn/util/Environment.h"
#include "rsyn/util/Colorize.h"
#include "rsyn/util/FloatRectangle.h"

#include <iostream>
using std::cout;
using std::cerr;
#include <random>
#include <complex>

#include <wx/menu.h>

// -----------------------------------------------------------------------------

#define BLACK    Color(  0,   0,   0)
#define RED      Color(255,   0,   0)
#define GREEN    Color(  0, 255,   0)
#define BLUE     Color(  0,   0, 255)
#define YELLOW   Color(230, 230,   0)
#define DARK_RED Color(165,  42,  42)

// -----------------------------------------------------------------------------

DEFINE_LOCAL_EVENT_TYPE(myEVT_CELL_SELECTED)
DEFINE_LOCAL_EVENT_TYPE(myEVT_HOVER_OVER_OBJECT)
DEFINE_LOCAL_EVENT_TYPE(myEVT_CELL_DRAGGED)

// -----------------------------------------------------------------------------

const float PhysicalCanvasGL::LAYER_BACKGROUND	= 0.0f;
const float PhysicalCanvasGL::LAYER_OBSTACLES	= 0.1f;
const float PhysicalCanvasGL::LAYER_GRADIENTS	= 0.2f;
const float PhysicalCanvasGL::LAYER_MOVABLE	= 0.3f;
const float PhysicalCanvasGL::LAYER_GRID	= 0.4f;
const float PhysicalCanvasGL::LAYER_METAL	= 0.5f;
const float PhysicalCanvasGL::LAYER_ROUTING_MIN = 0.6f;
const float PhysicalCanvasGL::LAYER_ROUTING_MAX = 0.7f;
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

	// Hightlight
	clsHoverObjectId = GeometryManager::INVALID_OBJECT_ID;

	// Context Menu
	clsContextMenu = new wxMenu();
    clsContextMenu->Append(POPUP_MENU_PROPERTIES, wxT("Properties"));
	clsContextMenu->Append(wxID_SEPARATOR, wxT("-"));
	clsContextMenu->Append(POPUP_MENU_ADD_TO_HIGHLIGHT, wxT("Add to Highlight"));
	clsContextMenu->Append(POPUP_MENU_CLEAR_HIGHLIGHT, wxT("Clear Highlight"));
	clsContextMenu->Append(wxID_SEPARATOR, wxT("-"));
	clsContextMenu->Append(POPUP_MENU_ZOOM_TO_FIT, wxT("Zoom to Fit"));
	clsContextMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PhysicalCanvasGL::onPopupClick), NULL, this);

} // end constructor

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::init() {
	if (clsInitialized)
		return;
	
	reset();

	msgNoGlew = clsSession.getMessage("GRAPHICS-001");
	msgNoRenderToTexture = clsSession.getMessage("GRAPHICS-002");
	
	timer = clsSession.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);
	routingEstimator = clsSession.getService("rsyn.routingEstimator", Rsyn::SERVICE_OPTIONAL);
	graphics = clsSession.getService("rsyn.graphics");
	design = clsSession.getDesign();
	module = design.getTopModule();
	clsCheckpoint = design.createAttribute();
	
	// Jucemar - 2017/03/25 -> Initializing and rendering only if physical service is initialized.
	// This approach avoids crash caused by loading design without physical data.
	if (clsSession.isServiceRunning("rsyn.physical")) {
		physical = clsSession.getService("rsyn.physical");
		phDesign = physical->getPhysicalDesign();

		storeCheckpoint();

		Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
		const FloatRectangle &bound = phDie.getBounds();
		FloatRectangle r = bound;
		r.scaleCentralized(1.01f);
		resetCamera(r[LOWER][X], r[LOWER][Y], r[UPPER][X], r[UPPER][Y]);

		renderCoreBounds();

		prepareGeometryManager();
		prepareRenderingTexture();
		populateGeometryManager();

		phDesign.registerObserver(this);
	} // end if

	clsInitialized = true;
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
	wxCommandEvent event(myEVT_HOVER_OVER_OBJECT);
	wxPostEvent(this, event);
	Refresh();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::storeCheckpoint() {
	if (!clsInitialized)
		return;
	
	Rsyn::Design design = clsSession.getDesign();
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
	if (!clsInitialized)
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
	if (!clsInitialized)
		return;
	
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
	if (!clsInitialized)
		return;

	const float x1 = translateFromViewportToUserX(event.m_x);
	const float y1 = translateFromViewportToUserY(event.m_y);
	const float x0 = translateFromViewportToUserX(clsDragStartX);
	const float y0 = translateFromViewportToUserY(clsDragStartY);

	if (!clsMouseDown) {
		GeometryManager::ObjectId objectId = searchObjectAt(x1, y1);

		if (objectId != clsHoverObjectId) {
			clsHoverObjectId = objectId;
			prepare2DViewport(GetSize().x, GetSize().y, true);
			restoreRendering();
			geoMgr.renderFocusedObject(objectId);
			glFlush();
			SwapBuffers();

			wxCommandEvent event(myEVT_HOVER_OVER_OBJECT);
			event.SetClientData(geoMgr.getObjectData(objectId));
			wxPostEvent(this, event);
		} // end if
	} // end if

	// Test
	if (!clsMouseDown) return;

	clsMousePosition[X] = x1;
	clsMousePosition[Y] = y1;

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

void PhysicalCanvasGL::onRightClick(wxMouseEvent& event) {
    if (clsContextMenu && geoMgr.isObjectIdValid(clsHoverObjectId))
		PopupMenu(clsContextMenu, event.GetPosition());
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::onLeftDoubleClick(wxMouseEvent& event) {
	postDoubleClickCallback();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::onPopupClick(wxCommandEvent& event) {
	switch (event.GetId()) {
 		case POPUP_MENU_PROPERTIES:
			postDoubleClickCallback();
 			break;
		case POPUP_MENU_ADD_TO_HIGHLIGHT:
			addToHighlight();
			Refresh();
			break;
		case POPUP_MENU_CLEAR_HIGHLIGHT:
			geoMgr.clearHighlight();
			Refresh();
			break;
		case POPUP_MENU_ZOOM_TO_FIT:
			resetCamera();
			Refresh();
			break;
 	} // end switch
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::postDoubleClickCallback() {
	if (!geoMgr.isObjectIdValid(clsHoverObjectId))
		return;

	const PhysicalCanvasGL::GeoReference * geoRef =
			static_cast<PhysicalCanvasGL::GeoReference *>(geoMgr.getObjectData(clsHoverObjectId));

	Rsyn::RawPointer rawPointer(geoRef->getData());
	switch (geoRef->getObjectType()) {
		case Rsyn::OBJECT_TYPE_INSTANCE: {
			if (clsInstanceDoubleClickCallback)
				clsInstanceDoubleClickCallback(rawPointer.asInstance());
			break;
		} // end case
		case Rsyn::OBJECT_TYPE_NET: {
			if (clsNetDoubleClickCallback)
				clsNetDoubleClickCallback(rawPointer.asNet());
			break;
		} // end case

		case Rsyn::OBJECT_TYPE_PIN: {
			if (clsPinDoubleClickCallback)
				clsPinDoubleClickCallback(rawPointer.asPin());
			break;
		} // end case
	} // end switch
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::addToHighlight() {
	if (!geoMgr.isObjectIdValid(clsHoverObjectId))
		return;

	const PhysicalCanvasGL::GeoReference * geoRef =
			static_cast<PhysicalCanvasGL::GeoReference *>(geoMgr.getObjectData(clsHoverObjectId));

	Rsyn::RawPointer rawPointer(geoRef->getData());
	switch (geoRef->getObjectType()) {
		case Rsyn::OBJECT_TYPE_INSTANCE:
			geoMgr.addObjectToHighlight(clsHoverObjectId);
			break;
		case Rsyn::OBJECT_TYPE_NET:
			geoMgr.addGroupOfObjectToHighlight(clsHoverObjectId);
			break;
		case Rsyn::OBJECT_TYPE_PIN:
			geoMgr.addObjectToHighlight(clsHoverObjectId);
			break;
	} // end switch
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
	Rsyn::Design design = clsSession.getDesign();
	
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

	if (!clsInitialized)
		return;
	Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
	const FloatRectangle &bounds = phDie.getBounds();
	glLineWidth(1);
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
	if(!clsInitialized)
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

	Rsyn::Design design = clsSession.getDesign();
	
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
	if (clsSession == nullptr)
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

	Rsyn::Design design = clsSession.getDesign();
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

	Rsyn::Design design = clsSession.getDesign();
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
	if (!clsInitialized || !clsSelectedCell)
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
		Rsyn::Design design = clsSession.getDesign();
		
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

void PhysicalCanvasGL::renderFocusedObject() {
	if (!clsInitialized || (!geoMgr.isObjectIdValid(clsHoverObjectId)))
		return;
	geoMgr.renderFocusedObject(clsHoverObjectId);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::renderTree() {
	if (!clsSelectedCell || clsInterpolationValue > 0 || !routingEstimator)
		return;

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
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crashes when a design without physical data is loaded. 
	if(!isPhysicalDesignInitialized())
		return;

	if (clsRepopulateGeometryManager)
		populateGeometryManager();

	prepare2DViewport(width, height);
		
	if (clsViewCoreBounds) renderCoreBounds();
		
	for (CanvasOverlayConfiguration &config : clsOverlays) {
		if (config.visible) {
			if (getInterpolationValue() > 0) {
				config.overlay->renderInterpolated(this);
			} else {
				config.overlay->render(this);
			} // end else
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

	swapBuffers();

	// Save snapshot.
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
	if (!IsShownOnScreen()) return;
	if (!clsInitialized) {
		clearViewport();
	} else {
		render();
	} // end else
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::onResized(wxSizeEvent& evt) {
	CanvasGL::onResized(evt);

	if (IsShownOnScreen() && clsInitialized) {
		prepareRenderingTexture();
	} // end if
} // end method

////////////////////////////////////////////////////////////////////////////////
// Snapshot
////////////////////////////////////////////////////////////////////////////////

// [TODO] Temp function, will be improved and moved to CanvasGL.

void PhysicalCanvasGL::saveSnapshot(wxImage& image) {
	const int w = GetSize().x;
	const int h = GetSize().y;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	GLubyte * pixels = new GLubyte[w * h * 4];
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

	delete[] pixels;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
} // end method

////////////////////////////////////////////////////////////////////////////////
// Hover Selection
////////////////////////////////////////////////////////////////////////////////

PhysicalCanvasGL::GeoReference *
PhysicalCanvasGL::createGeoReference(Rsyn::Instance instance) {
	GeoReference ref;
	ref.type = Rsyn::OBJECT_TYPE_INSTANCE;
	ref.data = Rsyn::RawPointer(instance);
	geoReferences.push_back(ref);
	return &geoReferences.back();
} // end method

// -----------------------------------------------------------------------------

PhysicalCanvasGL::GeoReference *
PhysicalCanvasGL::createGeoReference(Rsyn::Net net) {
	GeoReference ref;
	ref.type = Rsyn::OBJECT_TYPE_NET;
	ref.data = Rsyn::RawPointer(net);
	geoReferences.push_back(ref);
	return &geoReferences.back();
} // end method

// -----------------------------------------------------------------------------

PhysicalCanvasGL::GeoReference *
PhysicalCanvasGL::createGeoReference(Rsyn::Pin pin) {
	GeoReference ref;
	ref.type = Rsyn::OBJECT_TYPE_PIN;
	ref.data = Rsyn::RawPointer(pin);
	geoReferences.push_back(ref);
	return &geoReferences.back();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::prepareGeometryManager() {
	const bool debug = false;	

	clsGeoNets = design.createAttribute();

	// Instances
	geoCellLayerId = geoMgr.createLayer("cells", 0, Color(0, 210, 210), Color(0, 210, 210), LINE_STIPPLE_SOLID, STIPPLE_MASK_EMPTY);
	geoMacroLayerId = geoMgr.createLayer("macros", 0, Color(0, 0, 0), Color(0, 0, 0), LINE_STIPPLE_NONE, STIPPLE_MASK_FILL);
	geoPortLayerId = geoMgr.createLayer("ports", 0, Color(0, 0, 0), Color(0, 0, 0), LINE_STIPPLE_NONE, STIPPLE_MASK_EMPTY);

	// Pins
	geoPinsLayerId = geoMgr.createLayer("pins", 5, Color(0, 0, 255), Color(0, 0, 255), LINE_STIPPLE_SOLID, STIPPLE_MASK_DOT);
	geoMgr.setLayerVisibility(geoPinsLayerId, true);

	// Routing
	const int offset = 10;
	int z = 2 + offset;
	size_t currStyle = 0;
	techLayerIds.clear();
	techLayerIds.resize(phDesign.getNumLayers(), -1);
	for (const Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
		Rsyn::PhysicalLayerType type = phLayer.getType();
		FillStippleMask mask = techLayerMasks[currStyle];
		Color c = techLayerColors[currStyle];
		
		if (type == Rsyn::ROUTING) { // If is a routing layer, increment style
			currStyle = std::min(currStyle+1, techLayerMasks.size()-1);	
		} else if (type == Rsyn::CUT) { // If is a via, force empty mask
			mask = STIPPLE_MASK_EMPTY;
		} else { // Other tech layers are not supported 
			mask = STIPPLE_MASK_EMPTY;
			c = Color(0, 0 ,0);
		}
		
		if(debug) {
			std::cout << "Adding a layer called " << phLayer.getName()
					  << " (id=" << phLayer.getIndex() << ") "
					  << " with mask " << mask << " and color (" << (int) c.r 
					  << ", " << (int) c.g << ", " << (int) c.b << ")\n"; 
		}
		
		GeometryManager::LayerId id = geoMgr.createLayer(phLayer.getName(), z++, c, c, LINE_STIPPLE_SOLID, mask);
		techLayerIds[phLayer.getIndex()] = id;
	} // end for
	
	for (GeometryManager::LayerId& id : techLayerIds) {
		if (id < 0)
			continue;
		geoMgr.setLayerVisibility(id, true);
	}
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::populateGeometryManager() {
	if (!clsRepopulateGeometryManager)
		return;
	clsRepopulateGeometryManager = false;

	// TODO: Make the update incremental.
	geoMgr.removeAllObjects();
	
	//
	// Instances
	//
	for (Rsyn::Instance instance : module.allInstances()) {
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);

		if (cell.isPort()) {
			const Bounds & bounds = phCell.getBounds();
			GeometryManager::Point p0(bounds[LOWER][X], bounds[LOWER][Y]);
			GeometryManager::Point p1(bounds[UPPER][X], bounds[UPPER][Y]);
			geoMgr.addRectangle(geoPortLayerId, GeometryManager::Box(p0, p1), createGeoReference(instance));
		} else {
			if (cell.isMacroBlock()) {
				const Rsyn::PhysicalLibraryCell &phLibCell = phDesign.getPhysicalLibraryCell(cell);
				if (phLibCell.hasLayerObstacles()) {
					for (const Bounds & obs : phLibCell.allLayerObstacles()) {
						Bounds bounds = obs;
						DBUxy lower = obs.getCoordinate(LOWER);
						bounds.moveTo(phCell.getPosition() + lower);

						GeometryManager::Point p0(bounds[LOWER][X], bounds[LOWER][Y]);
						GeometryManager::Point p1(bounds[UPPER][X], bounds[UPPER][Y]);
						geoMgr.addRectangle(geoMacroLayerId, GeometryManager::Box(p0, p1), createGeoReference(instance));
					} // end for
				} else {
					Bounds bounds(DBUxy(), phLibCell.getSize());
					bounds.moveTo(phCell.getPosition());

					GeometryManager::Point p0(bounds[LOWER][X], bounds[LOWER][Y]);
					GeometryManager::Point p1(bounds[UPPER][X], bounds[UPPER][Y]);
					geoMgr.addRectangle(geoMacroLayerId, GeometryManager::Box(p0, p1), createGeoReference(instance));
				} // end else
			} else {
				const Bounds & bounds = phCell.getBounds();

				GeometryManager::BoxOrientation orientation;
				switch (phCell.getOrientation()) {
					case Rsyn::ORIENTATION_N:
					case Rsyn::ORIENTATION_FW:
						orientation = GeometryManager::BOX_ORIENTATION_SW;
						break;
					case Rsyn::ORIENTATION_S:
					case Rsyn::ORIENTATION_FE:
						orientation = GeometryManager::BOX_ORIENTATION_NE;
						break;
					case Rsyn::ORIENTATION_W:
					case Rsyn::ORIENTATION_FN:
						orientation = GeometryManager::BOX_ORIENTATION_SE;
						break;
					case Rsyn::ORIENTATION_E:
					case Rsyn::ORIENTATION_FS:
						orientation = GeometryManager::BOX_ORIENTATION_NW;
						break;
					default:
						orientation = GeometryManager::BOX_ORIENTATION_INVALID;
				} // end switch

				GeometryManager::Point p0(bounds[LOWER][X], bounds[LOWER][Y]);
				GeometryManager::Point p1(bounds[UPPER][X], bounds[UPPER][Y]);
				GeometryManager::ObjectId objectId =
						geoMgr.addRectangle(geoCellLayerId, GeometryManager::Box(p0, p1), createGeoReference(instance), orientation);
				const Color rgb = graphics->getCellColor(instance);
				geoMgr.setObjectFillColor(objectId,rgb);
			} // end if-else
		} // end if-else
	} // end for

	//
	// Nets.
	//
	for (Rsyn::Net net : module.allNets()) {
		const GeometryManager::GroupId groupId = geoMgr.createGroup();
		clsGeoNets[net] = groupId;
		Rsyn::PhysicalNet phNet = phDesign.getPhysicalNet(net);
		
		// TEMPORARY
		const bool debug = false;
		if (debug) {
			std::cout << "Net [" << net.getName() << "] has " << 
				phNet.allWires().size() << " wires...\n"; 
		}	
		
		
		for (Rsyn::PhysicalWire phWire : phNet.allWires()) {
			for (Rsyn::PhysicalWireSegment phWireSegment : phWire.allSegments()) {
				const std::vector<Rsyn::PhysicalRoutingPoint> & routingPts = phWireSegment.allRoutingPoints();
				if (phWireSegment.getNumRoutingPoints() > 1) {
					
					std::vector<DBUxy> points;
					points.reserve(routingPts.size());
					for (Rsyn::PhysicalRoutingPoint phRoutingPt : routingPts)
						points.push_back(phRoutingPt.getPosition());
					Rsyn::PhysicalLayer phLayer = phWireSegment.getLayer();
					const DBU width = phLayer.getWidth();
					const GeometryManager::LayerId layerId =
						techLayerIds[std::min(phLayer.getIndex(), (int) techLayerIds.size() - 1)];
					geoMgr.addPath(layerId, points, width, createGeoReference(net), groupId);
				}
				for (Rsyn::PhysicalRoutingPoint phRoutingPt : routingPts) {

					if (!phRoutingPt.hasVia())
						continue;
					Rsyn::PhysicalVia phVia = phRoutingPt.getVia();

					const DBUxy pos = phRoutingPt.getPosition();

					for (Rsyn::PhysicalViaLayer phViaLayer : phVia.allLayers()) {
						Rsyn::PhysicalLayer phLayer = phViaLayer.getLayer();

						for (Bounds bounds : phViaLayer.allBounds()) {
							bounds.translate(pos);

							const GeometryManager::LayerId layerId =
								techLayerIds[std::min(phLayer.getIndex(), (int) techLayerIds.size() - 1)];

							GeometryManager::Point p0(bounds[LOWER][X], bounds[LOWER][Y]);
							GeometryManager::Point p1(bounds[UPPER][X], bounds[UPPER][Y]);
							geoMgr.addRectangle(layerId, GeometryManager::Box(p0, p1), createGeoReference(net), 
									GeometryManager::BOX_ORIENTATION_INVALID, groupId);

							// Stripe Lines
							if (phLayer.getType() == Rsyn::ROUTING) {
								const DBU width = static_cast<DBU> (bounds.computeLength(X) / 2.0);
								std::vector<DBUxy> points(2);
								points[0] = DBUxy(pos[X], bounds[LOWER][Y]);
								points[1] = DBUxy(pos[X], bounds[UPPER][Y]);
								geoMgr.addPath(layerId, points, width, createGeoReference(net), groupId);
							} // end if
						} // end for
					} // end for
				} // end if
			} // end for
		} // end for
	} // end for

	//
	// Special Nets
	//

	for (Rsyn::PhysicalSpecialNet phSpecialNet : phDesign.allPhysicalSpecialNets()) {
		for (Rsyn::PhysicalWire phWire : phSpecialNet.allWires()){
			for (Rsyn::PhysicalWireSegment phSegment : phWire.allSegments()) {
				for (Rsyn::PhysicalRoutingPoint phRouting : phSegment.allRoutingPoints()) {
					if (phRouting.hasVia()){
						Rsyn::PhysicalVia phVia = phRouting.getVia();
						//drawWireSegmentVia(phVia, phRouting.getPosition());
					} // end if
				} // end for

				if (phSegment.getNumRoutingPoints() < 2)
					continue;

				Rsyn::PhysicalLayer phLayer = phSegment.getLayer();

				const DBU width = phSegment.getRoutedWidth();
				const GeometryManager::LayerId layerId =
						techLayerIds[std::min(phLayer.getIndex(), (int) techLayerIds.size() - 1)];

				std::vector<DBUxy> points;
				points.reserve(phSegment.getNumRoutingPoints());
				for (Rsyn::PhysicalRoutingPoint phRouting : phSegment.allRoutingPoints()) {
					points.push_back(phRouting.getPosition());
				} // end for

				geoMgr.addPath(layerId, points, width);
			} // end for
		} // end for
	} // end for

	//
	// Pins
	//

	for (Rsyn::Instance instance : module.allInstances()) {
		if (instance.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
		Rsyn::PhysicalTransform transform = phCell.getTransform();
		const DBUxy displacement = phCell.getPosition();

		for (Rsyn::Pin pin : instance.allPins()) {
			if (pin.isPort())
				continue;

			Rsyn::PhysicalLibraryPin phLibPin = phDesign.getPhysicalLibraryPin(pin);

			if (!phLibPin.hasPinGeometries())
				continue;

			for (Rsyn::PhysicalPinGeometry phPinPort : phLibPin.allPinGeometries()) {
				Rsyn::PhysicalPinLayer phPinLayer = phPinPort.getPinLayer();
				if (phPinLayer.hasPolygonBounds()) {
					for (const Rsyn::PhysicalPolygon &polygon : phPinLayer.allPolygons()) {
						std::vector<DBUxy> points;
						for (auto it1 = boost::begin(boost::geometry::exterior_ring(polygon));
							it1 != boost::end(boost::geometry::exterior_ring(polygon)); ++it1) {
							const Rsyn::PhysicalPolygonPoint &p = *it1;
							points.push_back(transform.apply(
								p.get<0>() + displacement.x, p.get<1>() + displacement.y));
						} // end for
						geoMgr.addPolygon(geoPinsLayerId, points, float2(0, 0), createGeoReference(pin));
					} // end for
				} // end if
				if (phPinLayer.hasRectangleBounds()) {
					for (Bounds bounds : phPinLayer.allBounds()) {
						bounds.translate(displacement);
						bounds = transform.apply(bounds);
						GeometryManager::Point p0(bounds[LOWER][X], bounds[LOWER][Y]);
						GeometryManager::Point p1(bounds[UPPER][X], bounds[UPPER][Y]);
						geoMgr.addRectangle(geoPinsLayerId, GeometryManager::Box(p0, p1), createGeoReference(pin));
					} // end for
				} // end if
			} // end for
		} // end for
	} // end if
	
	//
	// Ports
	//
	
//	for (Rsyn::Instance instance : module.allInstances()) {
//		if (instance.getType() != Rsyn::CELL)
//			continue;
//		Rsyn::Port port = instance.asPort(); 
//		Rsyn::PhysicalPort phPort = phDesign.getPhysicalPort(port);
//		const DBUxy &pos = phPort.getPosition();
//		
//	}
	
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::prepareRenderingTexture() {
	// Clean up
	if (clsRenderingToTextureEnabled) {
		glDeleteFramebuffers(1, &fboId);
		glDeleteRenderbuffers(1, &rboColorId);
		glDeleteRenderbuffers(1, &rboDepthId);
		fboId = 0;
		rboColorId = 0;
		rboDepthId = 0;
	} // end if

	if (glewInit() == GLEW_OK) {
		const int w = GetSize().x;
		const int h = GetSize().y;

		// Build the texture that will serve as the color attachment for the framebuffer.
		glGenRenderbuffers(1, &rboColorId);
		glBindRenderbuffer(GL_RENDERBUFFER, (GLuint)rboColorId);
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER, 0 );

		// Build the texture that will serve as the depth attachment for the framebuffer.
		glGenRenderbuffers(1, &rboDepthId);
		glBindRenderbuffer(GL_RENDERBUFFER, (GLuint)rboDepthId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER, 0 );

		// Build the framebuffer.
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)fboId);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColorId);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthId);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			clsRenderingToTextureEnabled = false;
			msgNoRenderToTexture.print();
		} else {
			clsRenderingToTextureEnabled = true;
		} // end else

////////////////////////////////////////////////////////////////////////////////
//		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
//		glGenFramebuffers(1, &fboId);
//		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
//
//		// The texture we're going to render to
//		glGenTextures(1, &renderedTexture);
//
//		// "Bind" the newly created texture : all future texture functions will modify this texture
//		glBindTexture(GL_TEXTURE_2D, renderedTexture);
//
//		// Give an empty image to OpenGL ( the last "0" )
//		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, w, h, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
//
//		// Poor filtering. Needed !
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//		// The depth buffer
//		GLuint depthrenderbuffer;
//		glGenRenderbuffers(1, &depthrenderbuffer);
//		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
//		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//		// Set "renderedTexture" as our colour attachement #0
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
//
//		// Set the list of draw buffers.
//		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
//		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//		// check FBO status
//		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//		if (status == GL_FRAMEBUFFER_COMPLETE) {
//			clsRenderingToTextureEnabled = true;
//		} else {
//			clsRenderingToTextureEnabled = false;
//			msgNoRenderToTexture.print();
//		} // end else
//
////////////////////////////////////////////////////////////////////////////////

		// If not render buffer is available use old inverted colors as a
		// workaround.
		if (clsRenderingToTextureEnabled) {
		//glLogicOp(GL_INVERT);
		//glEnable(GL_COLOR_LOGIC_OP);
		}

		// Make back buffer the current buffer.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	} else {
		msgNoGlew.print();
		//std::exit(1);
	} // end else

} // end method

// -----------------------------------------------------------------------------

GeometryManager::ObjectId
PhysicalCanvasGL::searchObjectAt(const float x, const float y) const {
	return geoMgr.searchObjectAt(x, y);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::swapBuffers() {
	glFlush();
	saveRendering();

	renderFocusedObject();
	glFlush();
	
	SwapBuffers();
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::saveRendering() {
	if (!clsRenderingToTextureEnabled)
		return;

	// std::cout << "SAVE *****" << std::endl;

	// Copy contents (pixels) from back buffer to the render buffer.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_BACK);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboId);
	glBlitFramebuffer(
			0, 0, GetSize().x, GetSize().y,
			0, 0, GetSize().x, GetSize().y,
			GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
} // end method

// -----------------------------------------------------------------------------

void PhysicalCanvasGL::restoreRendering() {
	if (!clsRenderingToTextureEnabled)
		return;

	glDisable(GL_DEPTH_TEST);

	// Copy contents from render buffer to the back buffer.
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer      (GL_BACK);              

	glBindFramebuffer (GL_READ_FRAMEBUFFER, fboId);
	glReadBuffer      (GL_COLOR_ATTACHMENT0);

	glBlitFramebuffer(
			0, 0, GetSize().x, GetSize().y,
			0, 0, GetSize().x, GetSize().y,
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
			GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
} // end method
