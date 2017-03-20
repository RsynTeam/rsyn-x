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

#include "rsyn/engine/Engine.h"
#include "rsyn/gui/canvas/PhysicalCanvasGL.h"
#include "rsyn/util/Matrix.h"
#include "x/infra/iccad15/Infrastructure.h"
#include "x/util/BlockageControl.h"

class OverlapRemoverOverlay : public CanvasOverlay {	
private:
	Rsyn::Engine clsEngine;
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
	
	bool init(PhysicalCanvasGL* canvas) override;
	void config(const nlohmann::json& params) override;
	void render(PhysicalCanvasGL* canvas) override;
};

#endif /* OVERLAPREMOVEROVERLAY_H */

