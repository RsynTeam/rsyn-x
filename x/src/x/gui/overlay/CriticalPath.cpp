/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
//#include <bits/shared_ptr_base.h>

#include "x/gui/overlay/CriticalPath.h"
#include "rsyn/model/timing/Timer.h"
#include "x/visualization/CircuitVisualization.h"


CriticalPathOverlay::CriticalPathOverlay() {
	
} // end constructor

// -----------------------------------------------------------------------------

bool CriticalPathOverlay::init(PhysicalCanvasGL* canvas, nlohmann::json& properties) {
	engine = canvas->getEngine();

	// Jucemar - 03/26/17 -> Initializes overlay only if physical design is running. 
	if(!engine.isServiceRunning("rsyn.physical")) 
		return false;
	
	ICCAD15::Infrastructure* clsInfrastructure = engine.getService("ufrgs.ispd16.infra", Rsyn::SERVICE_OPTIONAL);
	if(!clsInfrastructure)
		return false;
	
	Rsyn::PhysicalService* physical = engine.getService("rsyn.physical", Rsyn::SERVICE_OPTIONAL);
	if(!physical)
		return false;
	
	visual = engine.getService("rsyn.visualization", Rsyn::SERVICE_OPTIONAL);
	if (!visual)
		return false;
	
	
		
	clsPhysicalDesign = physical->getPhysicalDesign();
	clsDesign = engine.getDesign();	
	clsModule = clsDesign.getTopModule();
	timer = engine.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);
	clsInstance = clsDesign.createAttribute();
	
	// Everything seems okay...
	return true;
} // end method 

// -----------------------------------------------------------------------------

void CriticalPathOverlay::render(PhysicalCanvasGL * canvas) {
	
	std::cout << "render" << "\n";
	
	float2 mousePosition = canvas->getMousePosition();
	
	 CriticalPathOverlay::draw();
	
//	CriticalPathOverlay::selectCellAt(cpi, mousePosition);
	

} // end method 

// -----------------------------------------------------------------------------

void CriticalPathOverlay::draw(){
	std::vector<std::vector<CircuitVisualization::CDInstance>> cdPathsCV = visual->getCDPaths();
	for (std::vector<CircuitVisualization::CDInstance> path: cdPathsCV){
		for (CircuitVisualization::CDInstance cdInst: path){
			CriticalPathOverlay::drawInst(cdInst);
		}
	}
	std::vector<std::vector<Rsyn::Timer::PathHop>> hopsPaths = visual->getClsPaths();
	for (std::vector<Rsyn::Timer::PathHop> path: hopsPaths){
		Rsyn::Timer::PathHop lastHop = path.back();
		for (Rsyn::Timer::PathHop hop : path){
			std::cout << "hop: " << hop.getPin().getName() << "\n";
			if (hop.getPin() != lastHop.getPin()){
				std::cout << "next hop: " << hop.getNextPin().getName() << "\n";
				CircuitVisualization::PinPos prevPinPos;
				CircuitVisualization::PinPos nextPinPos;
				Rsyn::Pin prevPin = hop.getPin();
				Rsyn::Pin nextPin = hop.getNextPin();
				Rsyn::Attribute<Rsyn::Instance, CDInstance> instance = visual->getAttribute();
				CircuitVisualization::CDInstance & prevCDInstance = instance[prevPin.getInstance()];
				CircuitVisualization::CDInstance & nextCDInstance = instance[nextPin.getInstance()];
				for (CircuitVisualization::PinPos pin : prevCDInstance.pins){
					if (pin.pin == prevPin){
						prevPinPos = pin;
						break;
					}
				}

				for (CircuitVisualization::PinPos pinNext : nextCDInstance.pins){
					if (pinNext.pin == nextPin){
						nextPinPos = pinNext;
						break;
					}
				}

				CriticalPathOverlay::drawNet(prevPinPos.pos, nextPinPos.pos);
				}
		}
	
	}



}


// -----------------------------------------------------------------------------

void CriticalPathOverlay::selectCellAt(std::vector<std::vector<CircuitVisualization::CDInstance>> &cpi, float2 mousePosition){
	DBUxy mousePos = mousePosition.convertToDbu();
	
	for (std::vector<CircuitVisualization::CDInstance> & path: cpi){
		for (CircuitVisualization::CDInstance inst: path){
			const Bounds & bounds = inst.bound;
			
			if (bounds.inside(mousePos)){
				selectedCell = inst; 
				CriticalPathOverlay::renderSelectedCell();
			}
		
		}//end for

	} //end for
	
}//end method

// -----------------------------------------------------------------------------

void CriticalPathOverlay::renderSelectedCell(){
	
//	CriticalPathOverlay::drawCells();
	


}//end method


// -----------------------------------------------------------------------------

//void CriticalPathOverlay::drawCells(){
//	
//	Rsyn::Instance inst = selectedCell.instance;
//	int numPins = inst.getNumOutputPins();
//	Bounds bound = selectedCell.bound;
//	std::cout << "cell::  " << inst.getName() << "\n";
//	if (numPins > 0){
//		DBU pinSpace = cellSizeY/numPins;
//		DBUxy pos;
//		pos[X] = bound[UPPER][X];
//		pos[Y] = bound[LOWER][Y];
//		pos[Y] += pinSpace/2;
//		for (Rsyn::Pin pin : inst.allPins(Rsyn::OUT)){
//			Rsyn::Net net = pin.getNet();
//			glPointSize(10);
//			glColor3f(1.0, 0.0, 0.0);
//			glBegin(GL_POINTS);
//			glVertex3d(pos[X], pos[Y], 1);
//			glEnd();
//			for (Rsyn::Pin netPin : net.allPins(Rsyn::IN)){
//				Rsyn::Instance netInst = netPin.getInstance();
//			}
//			pos[Y] += pinSpace;
//		} // end for 
//
//
//	} //end if
//
//}//end method


// -----------------------------------------------------------------------------
 void CriticalPathOverlay::drawInst(CircuitVisualization::CDInstance cdInst){
	 Rsyn::Instance inst = cdInst.instance;
	 Bounds bounds = cdInst.bound;
	 DBUxy posLow;
	 DBUxy posUp;
	 posLow[X] = bounds[LOWER][X];
	 posLow[Y] = bounds[LOWER][Y];
	 posUp[X] = bounds[UPPER][X];
	 posUp[Y] = bounds[UPPER][Y];
	if (inst.isSequential())
		glColor3f(1.0, 0.0, 1.0);
	else
		if (inst.getType() == Rsyn::PORT)
			glColor3f(1.0, 0.0, 0.0);
		else
			if(inst.isFixed())
				glColor3f(0.0, 0.0, 0.0);
			else
				glColor3f(0.0, 0.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glVertex3d(posUp[X], posUp[Y], 1);
	glVertex3d(posLow[X], posUp[Y], 1);
	glVertex3d(posLow[X], posLow[Y], 1);
	glVertex3d(posUp[X], posLow[Y], 1);
	glEnd();
} // end method 

// -----------------------------------------------------------------------------

 void CriticalPathOverlay::drawNet (DBUxy posPrev, DBUxy posNext){
 	glLineWidth(2);
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3d(posPrev[X], posPrev[Y], 1);
	glVertex3d(posNext[X], posNext[Y], 1);
	glEnd();
 } // end method 

// -----------------------------------------------------------------------------
 
void CriticalPathOverlay::config(const nlohmann::json &params) {
	
} // end method 

// -----------------------------------------------------------------------------


