/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CriticalPath.h
 * Author: isadoraoliveira
 *
 * Created on 19 de Janeiro de 2017, 16:05
 */

#ifndef CRITICALPATH_H
#define CRITICALPATH_H

#include "rsyn/gui/canvas/PhysicalCanvasGL.h"
#include "x/gui/overlay/Abu.h"
#include "x/infra/iccad15/Infrastructure.h"
#include "rsyn/core/infra/RangeBasedLoop.h"
#include "x/visualization/CircuitVisualization.h"



class CriticalPathOverlay : public CanvasOverlay, public CircuitVisualization {
private:

	Rsyn::Module clsModule;
	Rsyn::PhysicalDesign clsPhysicalDesign;
	Rsyn::Design clsDesign;
	Rsyn::Timer * timer;
	Rsyn::Engine engine;
	Rsyn::Attribute<Rsyn::Instance, CircuitVisualization::CDInstance> clsInstance;
	CircuitVisualization* visual;
	

	
public:
	CriticalPathOverlay();	

	bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties) override;
	 void render(PhysicalCanvasGL * canvas);
	 void config(const nlohmann::json &params);
	 void drawInst(CircuitVisualization::CDInstance cdInst);
	 void drawNet(DBUxy posPrev, DBUxy posNext);
	 void selectCellAt(std::vector<std::vector<CircuitVisualization::CDInstance>> &cpi, float2 mousePosition);
	 void renderSelectedCell();
//	 void drawCells();
	 void draw();
	
	private:
		CircuitVisualization::CDInstance selectedCell;
	
}; // end class



#endif /* CRITICALPATH_H */

