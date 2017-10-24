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
 * File:   PhysicalObserver.h
 * Author: jucemar
 *
 * Created on 2 de Julho de 2017, 13:53
 */

#ifndef RSYN_PHYSICALOBSERVER_H
#define RSYN_PHYSICALOBSERVER_H

namespace Rsyn {

class PhysicalObserver {
friend class Rsyn::PhysicalDesign;
private:
	PhysicalDesign clsPhDesign;

public:
	
	// Note: The observer will not be registered to receive notifications for
	// methods that it does not overwrite. Therefore, no runtime overhead for
	// handling undesired notifications.

	virtual void
	onPhysicalDesignDestruction() {}

	virtual void
	onPostCellRemap(Rsyn::Cell cell, Rsyn::LibraryCell oldLibraryCell) {}
	
	virtual void
	onPreMovedInstance(Rsyn::Instance ) {}
	
	virtual void
	onPostMovedInstance(Rsyn::PhysicalInstance ) {}
	
	virtual
	~PhysicalObserver() {
		if (clsPhDesign)
			clsPhDesign.unregisterObserver(this);
	} // end destructor

};

} // end namespace


#endif /* RSYN_PHYSICALOBSERVER_H */

