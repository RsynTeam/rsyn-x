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

#ifndef REGION_OVERLAY_H
#define REGION_OVERLAY_H

#include "rsyn/core/Rsyn.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/gui/canvas/PhysicalCanvasGL.h"

namespace Rsyn {
class Session;
class Graphics;
} // end namespace

class RegionOverlay : public CanvasOverlay {
private:
	Rsyn::Design design;
	Rsyn::Module module;
	Rsyn::PhysicalDesign phDesign;

	void renderRegions(PhysicalCanvasGL * canvas);

public:

	RegionOverlay() = default;

	virtual bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties);
	virtual void render(PhysicalCanvasGL * canvas);
	virtual void config(const nlohmann::json &params);

}; // end class

#endif