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

#ifndef RSYN_TIMING_ARC_H
#define RSYN_TIMING_ARC_H

#include "core/Rsyn.h"
#include "TimingTypes.h"
#include "EdgeArray.h"

namespace Rsyn {

struct TimingLibraryArc;

struct TimingArcState {
        // Scaling factor in the range [0, 1] indicating how much of the
        // centrality
        // at the "to" pin should be passed to the "from" pin. The sum of
        // scaling
        // factor of timing arcs ending at a same "to" pin should add up to 1.
        Number flow;

        // Timing.
        EdgeArray<Number> delay;  // delay thru this timing arc
        EdgeArray<Number> oslew;  // output slew at this timing arc

        // When walking thru a timing arc the signal may be either inverted or
        // keep the same logical value. This property indicates whether or not
        // the signal must be inverted. Note that for positive and negative
        // unate senses this property is actually constant, however for
        // non-unate timing arc it will depend on the current timing context.
        EdgeArray<TimingTransition> backtrack;

        // Constructor.
        TimingArcState() : backtrack(TIMING_TRANSITION_INVALID) {
                delay.set(0, 0);
                oslew.set(0, 0);
                flow = 0;
        }  // end constructor
};         // end struct

// -----------------------------------------------------------------------------

struct TimingArc {
        // State
        std::array<TimingArcState, NUM_TIMING_MODES> state;

        // Constructor.
        TimingArc() {}  // end constructor
};                      // end struct

}  // end namespace

#endif
