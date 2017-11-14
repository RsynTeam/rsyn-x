#include "Abu.h"
#include "x/infra/iccad15/Infrastructure.h"

// -----------------------------------------------------------------------------

AbuOverlay::AbuOverlay() {
	clsViewUtilization = true;
	clsViewViolations = false;	
} // end constructor

// -----------------------------------------------------------------------------

bool AbuOverlay::init(PhysicalCanvasGL* canvas, nlohmann::json& properties) {
	clsSessionPtr;
	// Jucemar - 03/26/17 -> Initializes overlay only if physical design is running. 
	if(!clsSessionPtr.isServiceRunning("rsyn.physical")) 
		return false;
	infra = clsSessionPtr.getService("ufrgs.ispd16.infra", Rsyn::SERVICE_OPTIONAL);
	if ( !infra )
		return false;
	
	return true;
} // end method

// -----------------------------------------------------------------------------

void AbuOverlay::config(const nlohmann::json &params) {
	clsViewUtilization = params.value("/view/utilization"_json_pointer, clsViewUtilization);
	clsViewViolations = params.value("/view/violations"_json_pointer, clsViewViolations);
} // end method

// -----------------------------------------------------------------------------

void AbuOverlay::render(PhysicalCanvasGL * canvas) {
	//renderGrid(canvas);
	if ( clsViewUtilization ) renderUtilization(canvas);
	if ( clsViewViolations ) renderViolations(canvas);
} // end method

// -----------------------------------------------------------------------------

void AbuOverlay::renderGrid(PhysicalCanvasGL * canvas) {
	const unsigned numRows = infra->getAbuNumRows();
	const unsigned numCols = infra->getAbuNumCols();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3);
	glBegin(GL_QUADS);

	double layer = PhysicalCanvasGL::LAYER_GRID;
	Color rgb;
	rgb.setRGB(0,0,0);
	glColor3ub(rgb.r, rgb.g, rgb.b);
	for (unsigned i = 0; i < numRows; i++) {
		for (unsigned j = 0; j < numCols; j++) {
			const Bounds &bounds = infra->getABUBounds(i, j);
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
		} // end for 
	} // end for
	glLineWidth(1);
	glEnd();
} // end method 

// -----------------------------------------------------------------------------

void AbuOverlay::renderUtilization(PhysicalCanvasGL * canvas) {
	infra->coloringABU();
	const unsigned numRows = infra->getAbuNumRows();
	const unsigned numCols = infra->getAbuNumCols();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(3);
	glBegin(GL_QUADS);

	double layer = PhysicalCanvasGL::LAYER_GRID;
	for (unsigned i = 0; i < numRows; i++) {
		for (unsigned j = 0; j < numCols; j++) {
			Color &rgb = infra->getABUBinColor( i, j);
			if(rgb.transparent)
				continue;
			glColor3ub(rgb.r, rgb.g, rgb.b);
			const Bounds &bounds = infra->getABUBounds(i, j);
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
		} // end for 
	} // end for
	glLineWidth(1);
	glEnd();
} // end method 

// -----------------------------------------------------------------------------

void AbuOverlay::renderViolations(PhysicalCanvasGL * canvas) {
	infra->coloringABUViolations();
	const unsigned numRows = infra->getAbuNumRows();
	const unsigned numCols = infra->getAbuNumCols();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(3);
	glBegin(GL_QUADS);

	double layer = PhysicalCanvasGL::LAYER_GRID;
	for (unsigned i = 0; i < numRows; i++) {
		for (unsigned j = 0; j < numCols; j++) {
			Color &rgb = infra->getABUBinColor( i, j);
			if(rgb.transparent)
				continue;
			glColor3ub(rgb.r, rgb.g, rgb.b);
			const Bounds &bounds = infra->getABUBounds(i, j);
			glVertex3d(bounds[LOWER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[LOWER][Y], layer);
			glVertex3d(bounds[UPPER][X], bounds[UPPER][Y], layer);
			glVertex3d(bounds[LOWER][X], bounds[UPPER][Y], layer);
		} // end for 
	} // end for
	glLineWidth(1);
	glEnd();
} // end method