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

#ifndef RSYN_TIMING_MODEL_INTERFACE_H
#define RSYN_TIMING_MODEL_INTERFACE_H

#include "core/Rsyn.h"
#include "sandbox/Sandbox.h"
#include "TimingTypes.h"
#include "EdgeArray.h"

namespace Rsyn {

////////////////////////////////////////////////////////////////////////////////
// Abstract Timing Model Interface
////////////////////////////////////////////////////////////////////////////////

// TODO: Update description...
//
// User must override these methods to implement any desired timing model,
// including very simple ones (e.g. lumped capacitance) to complex ones (e.g
// Arnoldi).
//
// The timer will call these methods in a specific order during a topological
// traverse of netlist (from inputs to output). The protocol for timing
// calculation:
//
// 1) calculateArcTiming(...)
// 2) calculateLoadCapacitance(...)
// 3) prepareForDriverToSinkTimingCalculation(...)
// 4) calculateDriverToSinkTiming(...)
//
// For instance, a timing model that uses LUT (from Liberty) for cell delay/slew
// calculation and Elmore delay for interconnections would be implemented
// like this:
//
// 1) calculateArcTiming(...): Accesses the respective LUT associated to the
// timing arc and returns the delay/slew for the input slew and load capacitance
// requested.
//
// 2) calculateLoadCapacitance(...): Returns the lumped capacitance of the RC
// tree driven by the pin.
//
// 3) prepareForDriverToSinkTimingCalculation(...): Performs the Elmore delay
// computation of the RC tree.
//
// 4) calculateDriverToSinkTiming(...): Returns the delay/slew computed in the
// step (3) for a specific sink pair.

class TimingModel {
       public:
        // This method is called before a timing update is issued.
        virtual void beforeTimingUpdate() = 0;

        ////////////////////////////////////////////////////////////////////////////
        // Library
        ////////////////////////////////////////////////////////////////////////////

        virtual TimingSense getLibraryArcSense(
            const Rsyn::LibraryArc libraryArc) = 0;

        virtual void calculateInputDriverTiming(
            Rsyn::Instance port,  // TODO changed to Rsyn::Port after migration
                                  // to the new Rsyn
            const TimingMode mode, const EdgeArray<Number> &load,
            EdgeArray<Number> &inputDelay, EdgeArray<Number> &inputSlew) = 0;

        virtual void calculateLibraryArcTiming(
            const Rsyn::LibraryArc libraryArc, const TimingMode mode,
            const TimingTransition oedge, const Number islew, const Number load,
            Number &delay, Number &slew) = 0;

        virtual Number getPinInputCapacitance(Rsyn::Pin pin) const = 0;

        ////////////////////////////////////////////////////////////////////////////
        // Design
        ////////////////////////////////////////////////////////////////////////////

        virtual void calculateLoadCapacitance(const Rsyn::Pin pin,
                                              const TimingMode mode,
                                              EdgeArray<Number> &load) = 0;

        virtual void prepareNet(const Rsyn::Net net, const TimingMode mode,
                                const EdgeArray<Number> &slew) = 0;

        virtual void calculateCellArcTiming(const Rsyn::Arc arc,
                                            const TimingMode mode,
                                            const TimingTransition oedge,
                                            const Number islew,
                                            const Number load, Number &delay,
                                            Number &slew) {
                calculateLibraryArcTiming(arc.getLibraryArc(), mode, oedge,
                                          islew, load, delay, slew);
        }

        virtual void calculateNetArcTiming(
            const Rsyn::Pin driver, const Rsyn::Pin sink, const TimingMode mode,
            const EdgeArray<Number> &slewAtDriver, EdgeArray<Number> &delay,
            EdgeArray<Number> &slew) = 0;

        virtual EdgeArray<Number> getSetupTime(Rsyn::Pin data) const = 0;

        virtual EdgeArray<Number> getHoldTime(Rsyn::Pin data) const = 0;

        virtual Number getLibraryPinInputCapacitance(
            Rsyn::LibraryPin lpin) const = 0;

        ////////////////////////////////////////////////////////////////////////////
        // Sandbox
        ////////////////////////////////////////////////////////////////////////////

        virtual void calculateLoadCapacitance(const Rsyn::SandboxPin pin,
                                              const TimingMode mode,
                                              EdgeArray<Number> &load) = 0;

        virtual void prepareNet(const Rsyn::SandboxNet net,
                                const TimingMode mode,
                                const EdgeArray<Number> &slew) = 0;

        virtual void calculateCellArcTiming(const Rsyn::SandboxArc arc,
                                            const TimingMode mode,
                                            const TimingTransition oedge,
                                            const Number islew,
                                            const Number load, Number &delay,
                                            Number &slew) {
                calculateLibraryArcTiming(arc.getLibraryArc(), mode, oedge,
                                          islew, load, delay, slew);
        }

        virtual void calculateNetArcTiming(
            const Rsyn::SandboxPin driver, const Rsyn::SandboxPin sink,
            const TimingMode mode, const EdgeArray<Number> &slewAtDriver,
            EdgeArray<Number> &delay, EdgeArray<Number> &slew) = 0;

        virtual EdgeArray<Number> getSetupTime(Rsyn::SandboxPin data) const = 0;

        virtual EdgeArray<Number> getHoldTime(Rsyn::SandboxPin data) const = 0;

};  // end class

}  // end namespace

#endif
