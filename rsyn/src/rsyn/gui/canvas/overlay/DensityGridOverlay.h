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
 * File:   DensityGridOverlay.h
 * Author: jucemar
 *
 * Created on 28 de Setembro de 2016, 16:52
 */

#ifndef GRIDAREAOVERLAY_H
#define GRIDAREAOVERLAY_H

#include "rsyn/gui/canvas/PhysicalCanvasGL.h"

class DensityGridOverlay : public CanvasOverlay {
protected:
	bool clsViewUtilization;
	bool clsViewViolations;
	void renderPinDensity(PhysicalCanvasGL * canvas);
	void renderGrid(PhysicalCanvasGL * canvas);
	void renderPinUtilization(PhysicalCanvasGL * canvas);
	void renderAreaUtilization(PhysicalCanvasGL * canvas);
	void renderAbuViolations(PhysicalCanvasGL * canvas);
	void renderRows(PhysicalCanvasGL * canvas);
public:
	
	DensityGridOverlay();
	
	virtual bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties);
	virtual void render(PhysicalCanvasGL * canvas);
	virtual void config(const nlohmann::json &params);

};

#endif /* GRIDAREAOVERLAY_H */

