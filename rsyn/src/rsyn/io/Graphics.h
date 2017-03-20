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
 
#ifndef RSYN_GRAPHICS_H
#define RSYN_GRAPHICS_H

// TODO: This class needs a deep clean up... 2016-10-31

#include "rsyn/core/Rsyn.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/engine/Service.h"
#include "rsyn/model/timing/types.h"
#include "rsyn/util/Color.h"
#include "rsyn/engine/Engine.h"

namespace Rsyn {
class PhysicalService;
}

namespace Rsyn {

class Timer;

class Graphics : public Service, public Rsyn::Observer {
private:
	// Engine
	Engine clsEngine;
	
	// Circuitry
	Rsyn::Design clsDesign;
	Rsyn::Module clsModule;	
	
	// Physical Design
	Rsyn::PhysicalDesign clsPhysicalDesign;

	// Services
	Rsyn::PhysicalService * clsPhysical = nullptr;
	Timer * clsTimer = nullptr;

	// Store instance colors.
	Rsyn::Attribute<Rsyn::Instance, Color> clsColorCells;

public:

	virtual void start(Engine engine, const Json &params);
	virtual void stop();
	
	virtual void onPostInstanceCreate(Rsyn::Instance instance) override;

	Rsyn::Instance searchCellAt(const DBU x, const DBU y);

	Color &getCellColor (Rsyn::Instance cell ){ return clsColorCells[cell]; }
	
	// Coloring schemes.	
	void coloringRandomBlue();
	void coloringByCellType();
	void coloringColorful();
	void coloringGray();
	void coloringSlack(TimingMode mode);
	void coloringCriticality(const TimingMode mode, const double threshold);
	void coloringRelativity(const TimingMode mode);
	void coloringCentrality(const TimingMode mode, const double threshold);
	void coloringCriticalPath(const TimingMode mode, const bool showNeighbors);
	void coloringQDPMoved(); 
	void colorNeighbours(Rsyn::Instance);
	
}; // end class

} // end namespace

#endif /* GRAPHICS_H */

