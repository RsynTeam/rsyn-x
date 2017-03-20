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
 * File:   RoutingEstimatorOverlay.h
 * Author: mateus
 *
 * Created on 21 de Janeiro de 2017, 22:01
 */

#ifndef ROUTINGESTIMATOROVERLAY_H
#define ROUTINGESTIMATOROVERLAY_H


#include "rsyn/gui/canvas/PhysicalCanvasGL.h"

class RoutingEstimatorOverlay : public CanvasOverlay {
	Rsyn::Design clsDesign;
	Rsyn::Module clsTopModule;
	Rsyn::RoutingEstimator* clsRoutingEstimator;
	int clsMaxDegree;
	std::vector<std::tuple<int,Rsyn::Net>> orderedNets;
	
public:
	RoutingEstimatorOverlay() {};
	bool init(PhysicalCanvasGL* canvas) override;
	void config(const nlohmann::json& params) override {};
	void render(PhysicalCanvasGL* canvas) override;
};

#endif /* ROUTINGESTIMATOROVERLAY_H */

