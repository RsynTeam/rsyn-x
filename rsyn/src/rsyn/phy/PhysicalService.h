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
 
#ifndef RSYN_PHYSICAL_SERVICE_H
#define RSYN_PHYSICAL_SERVICE_H

#include "rsyn/session/Service.h"
#include "rsyn/phy/PhysicalDesign.h"

namespace Rsyn {

class PhysicalService : public Rsyn::Service, public Observer {
private:
	
	Rsyn::PhysicalDesign clsPhysicalDesign;
	Rsyn::Design clsDesign;
	
public:
	/*!
	  start method to the Rsyn::PhysicalDesign service
	  \param  Rsyn::Session session
	  \param  const Rsyn::Json &params
	  \return void 
	 */
	virtual void start(const Json &params);
	
	/*!
	  stop method to the Rsyn::PhysicalDesign service
	 */
	virtual void stop();
	
	/*!
	  Getting the Rsyn::PhysicalDesign object
	  \return clsPhysicalDesign 
	 */
	Rsyn::PhysicalDesign getPhysicalDesign() { return clsPhysicalDesign; }
	
	// Events
	virtual void
	onPostCellRemap(Rsyn::Cell cell, Rsyn::LibraryCell oldLibraryCell) override;
	
	virtual void 
	onPostInstanceCreate(Rsyn::Instance instance) override;
}; // end class

} // end namespace

#endif /* PHYSICALLAYER_H */

