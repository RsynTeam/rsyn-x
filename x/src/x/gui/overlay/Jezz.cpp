#include "Jezz.h"
#include "x/infra/iccad15/Infrastructure.h"

// -----------------------------------------------------------------------------

JezzOverlay::JezzOverlay() : jezz(nullptr) {
	
} // end constructor

// -----------------------------------------------------------------------------

bool JezzOverlay::init(PhysicalCanvasGL* canvas, nlohmann::json& properties) {
	if (!canvas->getEngine())
		return false;
	// Jucemar - 03/26/17 -> Initializes overlay only if physical design is running. 
	if(!canvas->getEngine().isServiceRunning("rsyn.physical")) 
		return false;
	
	// Try to get ISPD16 infrastructure.
	ICCAD15::Infrastructure *infra =
			canvas->getEngine().getService("ufrgs.ispd16.infra", Rsyn::SERVICE_OPTIONAL);
	if (!infra)
		return false;
	
	// Try to get Jezz.
	jezz = infra->getJezz();
	if (!jezz)
		return false;
	
	// Everything seems okay...
	return true;
} // end method

// -----------------------------------------------------------------------------

void JezzOverlay::render(PhysicalCanvasGL * canvas) {
	if (!jezz)
		return;
	
	renderRows(canvas);
	renderNodes(canvas);
} // end method

// -----------------------------------------------------------------------------

void JezzOverlay::config(const nlohmann::json &params) {
	
} // end method

// -----------------------------------------------------------------------------

void JezzOverlay::renderNodes(PhysicalCanvasGL * canvas) {
	Rsyn::Engine engine = canvas->getEngine();
	
	// Draw outlines.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 0);

	for (Jezz::JezzNode * jezzNode : jezz->allNodes()) {
		DBU xmin;
		DBU ymin;
		DBU xmax;
		DBU ymax;
		if (!jezz->jezz_dp_GetNodeBounds(jezzNode, xmin, ymin, xmax, ymax))
			continue; // non-legalized node
		
		const double layer = PhysicalCanvasGL::LAYER_BACKGROUND;
		
		glVertex3d(xmin, ymin, layer);
		glVertex3d(xmax, ymin, layer);
		glVertex3d(xmax, ymax, layer);
		glVertex3d(xmin, ymax, layer);
	} // end for
	glEnd();
	
	// Draw filling.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	for (Jezz::JezzNode * jezzNode : jezz->allNodes()) {
		if (jezzNode->isWhitespace())
			continue;
		
		DBU xmin;
		DBU ymin;
		DBU xmax;
		DBU ymax;
		if (!jezz->jezz_dp_GetNodeBounds(jezzNode, xmin, ymin, xmax, ymax))
			continue; // non-legalized node
		
		double layer;
		Color rgb;
		if (jezzNode->isFixed()) {
			layer = PhysicalCanvasGL::LAYER_BACKGROUND;
			rgb.setRGB(100, 100, 100);
		} else {
			layer = PhysicalCanvasGL::LAYER_BACKGROUND;
			rgb.r = 0;
			rgb.g = 0;
			rgb.b = 127;
		} // end else

		glColor3ub(rgb.r, rgb.g, rgb.b);
		
		glVertex3d(xmin, ymin, layer);
		glVertex3d(xmax, ymin, layer);
		glVertex3d(xmax, ymax, layer);
		glVertex3d(xmin, ymax, layer);

	} // end for
	glEnd();
} // end method 

// -----------------------------------------------------------------------------

void JezzOverlay::renderRows(PhysicalCanvasGL * canvas){
	Rsyn::Engine engine = canvas->getEngine();
	
	const vector<Jezz::JezzRow> &jezzRows = jezz->getRows();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3ub(100, 100, 100);
	int rowHight = 3420; //hardcoded based on iccad15 benchmarks. It works only in these benchmarks
	int siteSize = 380; //hardcoded based on iccad15 benchmarks. It works only in these benchmarks

	for (const Jezz::JezzRow & row : jezzRows) {
		int xMin = row.xmin()*siteSize;
		int xMax = row.xmax()*siteSize;
		int yMin = row.origin_y;
		int yMax = yMin + rowHight;
		
		glVertex3d(xMin, yMin, PhysicalCanvasGL::LAYER_BACKGROUND);
		glVertex3d(xMax, yMin, PhysicalCanvasGL::LAYER_BACKGROUND);
		glVertex3d(xMax, yMax, PhysicalCanvasGL::LAYER_BACKGROUND);
		glVertex3d(xMin, yMax, PhysicalCanvasGL::LAYER_BACKGROUND);
		  
	} // end for

	glEnd();
} // end method 
