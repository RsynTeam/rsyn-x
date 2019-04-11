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

#ifndef RSYN_TIMING_NET_H
#define RSYN_TIMING_NET_H

#include "EdgeArray.h"
#include "TimingTypes.h"

namespace Rsyn {

struct TimingArc;
struct TimingPin;

struct TimingNetState {
        // Indicates which timing arc we should walk thru when backtracking to
        // the from pin of the timing arc.
        EdgeArray<TimingArc *> backtrackArc;

        // Constructor
        TimingNetState() { backtrackArc.set(NULL, NULL); }  // end constructor
};                                                          // end struct

struct TimingNet {
        // Incremental timing analysis signature.
        int sign;

        // Indicates that the timing of this net was update outside a complete
        // timing update. For instance, when timing is update only locally.
        // If the timing was updated locally, it may happen that the current
        // timing
        // matches the propagated timing, which lead to the propagation to be
        // pruned. However, as the timing was updated only locally, that means
        // that
        // it was not propagated, therefore we should not prune. So we need a
        // flag
        // to avoid pruning in such cases.
        bool dirty;

        // State
        TimingNetState state[NUM_TIMING_MODES];

        // Constructor
        TimingNet() {
                sign = 0;
                dirty = false;
        }  // end constructor

};  // end struct

}  // end namespace

#endif
