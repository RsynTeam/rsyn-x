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

