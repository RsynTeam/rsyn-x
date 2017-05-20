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
	RoutingEstimatorOverlay() {}
	bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties) override;
	void config(const nlohmann::json& params) override {};
	void render(PhysicalCanvasGL* canvas) override;
};

#endif /* ROUTINGESTIMATOROVERLAY_H */

