/* Copyright 2014-2018 Rsyn
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
 
#ifndef RSYN_TIMING_LIBRARY_PIN_H
#define	RSYN_TIMING_LIBRARY_PIN_H

#include "types.h"

namespace Rsyn {
	
struct TimingLibraryPin {
	int index{-1};

	// Indicates if this pin is the clock pin of a register.
	bool clocked{false};
	
	// For non-sequential pins, always = -1
	// For sequential pins,
	//     if clocked = true; indicates the index of the data pin;
	//     if clocked = false; indicates the index of the clock pin;
	int control{-1};
}; // end class

} // end namespace


#endif	/* TIMINGLIBRARYPIN_H */

