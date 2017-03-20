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
 
#ifndef RSYN_TYPES_H
#define RSYN_TYPES_H

typedef float Number;

namespace Rsyn {

enum TimingMode {
	EARLY = 0,
	LATE  = 1,
	
	NUM_TIMING_MODES
}; // end enum

enum TimingTransition {
	TIMING_TRANSITION_INVALID = -1,
	RISE = 0,
	FALL = 1,
	
	NUM_EDGE_TYPES
}; // end enum

enum TimingSense {
	TIMING_SENSE_INVALID = -1,
	
	NEGATIVE_UNATE = 0,
	POSITIVE_UNATE = 1,
	NON_UNATE      = 2,

	NUM_TIMING_SENSES
}; // end enum

const TimingTransition REVERSE_EDGE_TYPE[2] = {
	FALL,
	RISE
};

extern const Number UNINITVALUE; 
extern const Number DEFAULT_UNINIT_VALUE[NUM_TIMING_MODES];

} // end namespace

#endif

