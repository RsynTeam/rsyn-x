/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OverlapRemoverOverlay.h
 * Author: mpfogaca
 *
 * Created on 17 de Outubro de 2016, 14:55
 */

#ifndef OVERLAPREMOVEROVERLAY_H
#define OVERLAPREMOVEROVERLAY_H

#include "rsyn/session/Session.h"
#include "rsyn/gui/canvas/PhysicalCanvasGL.h"
#include "rsyn/util/Matrix.h"
#include "x/infra/iccad15/Infrastructure.h"
#include "x/util/BlockageControl.h"

class OverlapRemoverOverlay : public CanvasOverlay {	
private:
	Rsyn::Module clsModule;
	Rsyn::PhysicalDesign clsPhysicalDesign;

	ICCAD15::BlockageControl* clsBlockageControl;

	void markOverlap(float cx, float cy, float r, int num_segments=50) {	 
		glLineWidth(3.5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_POLYGON);
		glColor3ub(255, 255, 0);
		
		for(int ii = 0; ii < num_segments; ii++) { 
			float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);

			float x = r * cosf(theta);//calculate the x component 
			float y = r * sinf(theta);//calculate the y component 

			glVertex3f(x + cx, y + cy, PhysicalCanvasGL::LAYER_FOREGROUND);//output vertex 
		} // end for
		
		glEnd(); 
		glLineWidth(1.0);
	} // end method
	
	void renderOverlaps();
	
public:
	OverlapRemoverOverlay();
	
	bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties) override;
	void config(const nlohmann::json& params) override;
	void render(PhysicalCanvasGL* canvas) override;
};

#endif /* OVERLAPREMOVEROVERLAY_H */

