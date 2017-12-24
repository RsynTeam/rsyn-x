/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RoutingGuidesOverlay.h
 * Author: mateus
 *
 * Created on December 24, 2017, 11:05 AM
 */

#ifndef ROUTINGGUIDESOVERLAY_H
#define ROUTINGGUIDESOVERLAY_H

#include "rsyn/core/Rsyn.h"
#include "rsyn/gui/canvas/PhysicalCanvasGL.h"
#include "rsyn/ispd18/RoutingGuide.h"

class RoutingGuidesOverlay : public CanvasOverlay {
private:
	Rsyn::Session session;
	Rsyn::Design design;
	Rsyn::Module module;
	
	PhysicalCanvasGL* canvas;
	Rsyn::RoutingGuide* routingGuide;
	GeometryManager *geometryManager;
	
	Rsyn::Attribute<Rsyn::Net, bool> visible;
	
	void drawGuide(const Rsyn::Net net) const;
	void setAll(const bool visible);
public:
	RoutingGuidesOverlay() = default;

	virtual bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties);
	virtual void render(PhysicalCanvasGL * canvas);
	virtual void config(const nlohmann::json &params);
};

#endif /* ROUTINGGUIDESOVERLAY_H */

