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

#ifndef RSYN_TIMING_PIN_H
#define RSYN_TIMING_PIN_H

#include <cassert>
#include <algorithm>

#include "EdgeArray.h"
#include "TimingTypes.h"

namespace Rsyn {

struct TimingLibraryPin;

struct TimingPinState {
        EdgeArray<Number> a;     // arrival time at input of this timing arc
        EdgeArray<Number> q;     // required time at input of this timing arc
        EdgeArray<Number> slew;  // slew at this timing arc
        EdgeArray<Number>
            wdelay;  // delay due to wire connection (zero for driving nodes)

        // Indicates the adjusted clock period (clock period accounting for
        // skew,
        // setup/hold time) of the worst path passing through this pin. In other
        // words, this is worst endpoint's required time spanned by (in the
        // fanout
        // cone of) this pin.
        EdgeArray<Number> wsq;

        // Unnormalized centrality.
        Number centrality;

        // Constructor.
        TimingPinState() {
                a.set(0, 0);
                q.set(0, 0);
                wsq.set(0, 0);
                slew.set(0, 0);
                wdelay.set(0, 0);

                centrality = 0;
        }  // end constructor

};  // end struct

struct TimingPin {
        // State
        std::array<TimingPinState, NUM_TIMING_MODES> state;

        // Skip this pin computation.
        bool skip : 1;

        // Indicates if this pin is the clock pin of a register.
        bool clocked : 1;

        // For non-sequential pins, always = -1
        // For sequential pins,
        //     if clocked = true; indicates the index of the data pin;
        //     if clocked = false; indicates the index of the clock pin;
        int control : 6;

        // Slack
        Number getWorstSlack(const TimingMode mode) const {
                switch (mode) {
                        case LATE:
                                return (state[LATE].q - state[LATE].a).getMin();
                        case EARLY:
                                return (state[EARLY].a - state[EARLY].q)
                                    .getMin();
                        default:
                                assert(false);
                                return 0;
                }  // end switch
        }          // end method

        Number getSlack(const TimingMode mode,
                        const TimingTransition transition) const {
                switch (mode) {
                        case LATE:
                                return state[LATE].q[transition] -
                                       state[LATE].a[transition];
                        case EARLY:
                                return state[EARLY].a[transition] -
                                       state[EARLY].q[transition];
                        default:
                                assert(false);
                                return 0;
                }  // end switch
        }          // end method

        EdgeArray<Number> getSlack(const TimingMode mode) const {
                switch (mode) {
                        case LATE:
                                return state[LATE].q - state[LATE].a;
                        case EARLY:
                                return state[EARLY].a - state[EARLY].q;
                        default:
                                assert(false);
                                return EdgeArray<Number>(0, 0);
                }  // end switch
        }          // end method

        EdgeArray<Number> getNegativeSlack(const TimingMode mode) const {
                EdgeArray<Number> nslack;
                nslack[RISE] = std::min((Number)0, getSlack(mode, RISE));
                nslack[FALL] = std::min((Number)0, getSlack(mode, FALL));
                return nslack;
        }  // end method

        Number getNegativeSlack(const TimingMode mode,
                                const TimingTransition transition) const {
                return std::min((Number)0, getSlack(mode, transition));
        }  // end method

        Number getMaxArrivalTime(const TimingMode mode) const {
                return state[mode].a.getMax();
        }  // end method

        Number getMinArrivalTime(const TimingMode mode) const {
                return state[mode].a.getMin();
        }  // end method

        bool isClockPin() const { return clocked; }
        bool isDataPin() const { return control != -1 && !clocked; }

        // Assume this is a clock pin and then returns the data pin associated
        // to
        // this clock pin.
        int getDataPinIndex() const { return control; }

        // Assume this is a data pin and then returns the clock pin associated
        // to
        // this data pin.
        int getClockPinIndex() const { return control; }

        // Constructor.
        TimingPin() {
                skip = false;
                clocked = false;
                control = -1;
                state[EARLY].q.setBoth(-UNINITVALUE);
                state[LATE].q.setBoth(+UNINITVALUE);
        }  // end constructor

};  // end struct

}  // end namespace

#endif
