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

#ifndef RSYN_LIBRARY_CHARACTERIZER_H
#define RSYN_LIBRARY_CHARACTERIZER_H

#include <vector>

#include "core/Rsyn.h"
#include "session/Service.h"
#include "tool/timing/TimingModel.h"

namespace Rsyn {

class Scenario;

class LibraryCharacterizer : public Rsyn::Service {
       private:
        Rsyn::Design clsDesign;
        Rsyn::Library clsLibrary;

        Rsyn::Scenario *clsScenario = nullptr;

        TimingModel *clsTimingModel = nullptr;

        bool clsAnalysisPerformed = false;

        struct LibraryArcCharacterization {
                LibraryArcCharacterization() : sense(TIMING_SENSE_INVALID) {}

                struct LogicalEffort {
                        // Read the edge array as the transition in the output
                        // pin.

                        // Timing sense aware input capacitance, that is, the
                        // capacitance of the
                        // from pin of this timing arc w.r.t. a transition in
                        // the to pin.
                        //
                        // For instance:
                        // - For negative-unate arc, cin[RISE] is the fall
                        // capacitance of the
                        // from pin of this arc.
                        // - For a posititve-unate arc, cin[RISE] is the rise
                        // capacitance of the
                        // from pin of this arc.
                        // - For a non-unate arc, cin[RISE] = cin[FALL] is the
                        // average
                        // capacitance of the from pin of this arc.
                        EdgeArray<Number> cin;

                        // Timing sense aware reference slew used to compute the
                        // logical effort.
                        // Take a look at the cin description to understand what
                        // timing sense
                        // aware means.
                        EdgeArray<Number> slew;

                        // Logical effort.
                        EdgeArray<Number> g;  // logical effort
                        EdgeArray<Number> p;  // parasitic delay

                        // For arcs, this flag is set to one if the arc is valid
                        // and to zero
                        // otherwise. For arc groups, this flag counts the
                        // number of valid arcs.
                        // Note that if the number of valid arcs is zero, this
                        // flag is also zero
                        // so that is safe to ask if(valid).
                        // An invalid arc may happen for some non-timing arcs
                        // (e.g. CK->D).
                        EdgeArray<int> valid;

                        // Residuum from least squares.
                        EdgeArray<Number> residuum;

                        LogicalEffort()
                            : cin(0, 0),
                              g(0, 0),
                              p(0, 0),
                              valid(0, 0),
                              residuum(0, 0) {}  // end constructor
                };                               // end struct

                LogicalEffort le[NUM_TIMING_MODES];

                // Cached timing sense.
                TimingSense sense;
        };  // end arc

        Rsyn::Attribute<Rsyn::LibraryArc, LibraryArcCharacterization>
            clsLibraryArcCharacterizations;

        EdgeArray<Number> clsLogicalEffort_ReferenceSlew[NUM_TIMING_MODES];
        Rsyn::LibraryArc
            clsLogicalEffort_ReferenceLibraryArcPointer[NUM_TIMING_MODES];
        Rsyn::LibraryCell
            clsLogicalEffort_ReferenceLibraryCell[NUM_TIMING_MODES];

        std::vector<Number> clsLogicalEffort_Gains;

        Number clsLibraryMaxDriverResistance[NUM_TIMING_MODES];
        Number clsLibraryMinDriverResistance[NUM_TIMING_MODES];

        struct TypicalValues {
                Number fanout = 0;
                Number delay = 0;
                Number delayPerLeakage = 0;
                Number slew = 0;
                Number resistance = 0;
                Number delayToSlewSensitivity = 0;
        };  // end struct

        TypicalValues clsTypicalValues;
        Number clsGainBasedSlewSlope = 0;
        Number clsGainBasedSlewConstant = 0;

        TypicalValues doTypicalAnalysis(const float typicalFanout);
        void doGainBasedSlewModelAnalysis();
        void doLogicalEffortAnalysis();

        void logicalEffort_FindReferenceLibraryTimingArc();
        void logicalEffort_ClaculateReferenceSlew();

        void logicalEffort_TimingArc(Rsyn::LibraryArc larc,
                                     const TimingMode mode,
                                     const TimingTransition oedge,
                                     const Number Cin, const Number inputSlew,
                                     const std::vector<Number> &gains,
                                     Number &g, Number &p, Number &residuum);

        void logicalEffort_LinearLeastSquares(const std::vector<Number> &x,
                                              const std::vector<Number> &y,
                                              Number &a, Number &b);

        void logicalEffort_LinearLeastSquaresError(const std::vector<Number> &x,
                                                   const std::vector<Number> &y,
                                                   const Number a,
                                                   const Number b,
                                                   Number &residuum);

        Number logicalEffort_MeanMSE(const Number SSE) {
                // see:
                // http://www.stat.purdue.edu/~xuanyaoh/stat350/xyApr6Lec26.pdf
                // SSE: sum of squared errors
                const int N = clsLogicalEffort_Gains.size();
                return std::sqrt(SSE / (N - 2));
        }  // end method

        // TODO: These ranged-based loop methods are copy and paste from the one
        // in the timers. We should come up with some centralized place for
        // these
        // methods.

        std::array<TimingMode, 2> allTimingModes() const {
                return {EARLY, LATE};
        }  // end method

        std::array<TimingTransition, 2> allTimingTransitions() const {
                return {FALL, RISE};
        }  // end method

        std::array<std::tuple<TimingTransition, TimingTransition>, 4>
        allTimingTransitionPairs() const {
                return {
                    std::make_tuple(FALL, FALL), std::make_tuple(FALL, RISE),
                    std::make_tuple(RISE, FALL), std::make_tuple(RISE, RISE)};
        }  // end method

        std::array<std::tuple<TimingMode, TimingTransition>, 4>
        allTimingModeAndTransitionPairs() const {
                return {
                    std::make_tuple(EARLY, FALL), std::make_tuple(EARLY, RISE),
                    std::make_tuple(LATE, FALL), std::make_tuple(LATE, RISE)};
        }  // end method

       public:
        virtual void start(const Rsyn::Json &params);
        virtual void stop();

        //! @Brief Performs the library analysis.
        void runLibraryCharacterization(TimingModel *timingModel);

        //! @brief Reports logical effort analysis.
        void reportLogicalEffort(std::ostream &out);

        //! @brief Reports typical values for the library.
        void reportTypicalValues(std::ostream &out);

        //! @brief Reports linear gain based mode.
        void reportGainBasedSlewModel(std::ostream &out);

        //! @brief Returns the typical leakage of the library.
        Number getTypicalLeakage() const {
                return getTypicalDelay() / getTypicalDelayPerLeakage();
        }

        //! @brief Returns the typical delay for the library. The typical delay
        //! is
        //!        defined as the average fanout-of-4 delay of the inverters in
        //!        the
        //!        library.
        Number getTypicalDelay() const { return clsTypicalValues.delay; }

        //! @brief Returns the typical delay per leakage for the library. The
        //!        typical delay per leakage is defined as the average
        //!        fanout-of-4
        //!        delay divided by the (state-independent) leakage of the
        //!        inverters
        //!        in the library.
        Number getTypicalDelayPerLeakage() const {
                return clsTypicalValues.delayPerLeakage;
        }

        //! @brief Returns the typical slew for the library. The typical slew is
        //!        defined as the average fanout-of-4 slew of the inverters in
        //!        the
        //!        library.
        Number getTypicalSlew() const { return clsTypicalValues.slew; }

        //! @brief Returns the typical resistance derived from RC = d --> R =
        //! d/C,
        //!        where d is the typical delay.
        Number getTypicalResistance() const {
                return clsTypicalValues.resistance;
        }

        //! @brief Returns the typical capacitance derived from RC = d --> C =
        //! d/R,
        //!        where d is the typical delay and R is the typical resistance.
        Number getTypicalCapacitance() const {
                return getTypicalDelay() / getTypicalResistance();
        }

        //! @brief Returns the typical delay to slew sensitivity (i.e. how much
        //!        the delay will change given a change in the input slew) for
        //!        the
        //!        library. The sensitivity is computed based on the typical
        //!        slew.
        Number getTypicalDelayToSlewSensitivity() const {
                return clsTypicalValues.delayToSlewSensitivity;
        }

        //! @brief Returns a slew given a gain.
        Number getGainBasedSlew(const Number gain) const {
                return clsGainBasedSlewSlope * gain + clsGainBasedSlewConstant;
        }

        //! @brief Returns the slope of the line modeling the gain-based slew.
        Number getGainBasedSlewSlope() const { return clsGainBasedSlewSlope; }

        //! @brief Returns the constant of the line modeling the gain-based
        //! slew.
        Number getGainBasedSlewConstant() const {
                return clsGainBasedSlewConstant;
        }

        LibraryArcCharacterization &getLibraryArcCharacterization(
            Rsyn::Arc arc) {
                return clsLibraryArcCharacterizations[arc.getLibraryArc()];
        }
        const LibraryArcCharacterization &getLibraryArcCharacterization(
            Rsyn::Arc arc) const {
                return clsLibraryArcCharacterizations[arc.getLibraryArc()];
        }

        LibraryArcCharacterization &getLibraryArcCharacterization(
            Rsyn::LibraryArc larc) {
                return clsLibraryArcCharacterizations[larc];
        }
        const LibraryArcCharacterization &getLibraryArcCharacterization(
            Rsyn::LibraryArc larc) const {
                return clsLibraryArcCharacterizations[larc];
        }

        Number getDriverResistance(Rsyn::Arc arc, const TimingMode mode,
                                   const TimingTransition oedge) const {
                const LibraryArcCharacterization::LogicalEffort &le =
                    getLibraryArcCharacterization(arc).le[mode];
                return le.g[oedge] / le.cin[oedge];
        }  // end method

        Number getDriverResistance(Rsyn::LibraryArc larc, const TimingMode mode,
                                   const TimingTransition oedge) const {
                const LibraryArcCharacterization::LogicalEffort &le =
                    getLibraryArcCharacterization(larc).le[mode];
                return le.g[oedge] / le.cin[oedge];
        }  // end method

        Number getDriverResistance(Rsyn::Arc arc, const TimingMode mode) const {
                return std::max(getDriverResistance(arc, mode, RISE),
                                getDriverResistance(arc, mode, FALL));
        }  // end method

        Number getDriverResistance(Rsyn::LibraryArc larc,
                                   const TimingMode mode) const {
                return std::max(getDriverResistance(larc, mode, RISE),
                                getDriverResistance(larc, mode, FALL));
        }  // end method

        Number getLibraryMaxDriverResistance(const TimingMode mode) const {
                return clsLibraryMaxDriverResistance[mode];
        }  // end method

        Number getLibraryMinDriverResistance(const TimingMode mode) const {
                return clsLibraryMinDriverResistance[mode];
        }  // end method

        Number getArcLogicalEffortDelay(Rsyn::Arc arc, const TimingMode mode,
                                        const TimingTransition oedge,
                                        const Number load) const {
                const LibraryArcCharacterization &larc =
                    getLibraryArcCharacterization(arc);
                const LibraryArcCharacterization::LogicalEffort &le =
                    larc.le[mode];

                const Number cin = le.cin[oedge];
                const Number h = load / cin;
                return le.p[oedge] + le.g[oedge] * h;
        }  // end method

        Number getArcLogicalEffortDelay(Rsyn::Arc arc, const TimingMode mode,
                                        const TimingTransition oedge) const {
                // return getArcLogicalEffortDelay(arc, mode, oedge,
                // clsTimer.getPinLoadCapacitance(arc.getToPin(), oedge));
                const LibraryArcCharacterization &larc =
                    getLibraryArcCharacterization(arc);
                return getArcLogicalEffortDelay(arc, mode, oedge,
                                                larc.le[mode].cin[oedge]);
        }  // end method

        //! @brief Returns the max driver resistance among all arcs of this
        //! cell.
        Number getCellMaxDriverResistance(Rsyn::Instance cell,
                                          const TimingMode mode) const {
                Number maxR = 0;
                for (Rsyn::Arc arc : cell.allArcs()) {
                        maxR = std::max(maxR, getDriverResistance(arc, mode));
                }  // end for
                return maxR;
        }  // end method

        //! @brief Computes fanout-of-n delay for a library arc. The load is set
        //! to
        //!        n times the input pin capacitance. The delay is computed
        //!        iteratively where the input slew of iteration k is the output
        //!        slew of iteration k-1.
        void computeFanoutOfNDelay(Rsyn::LibraryArc larc, const int n,
                                   EdgeArray<Number> &delay,
                                   EdgeArray<Number> &slew,
                                   EdgeArray<Number> &resistance,
                                   EdgeArray<Number> &delayToSlewSensitivity,
                                   const int numIterations = 5) const;

};  // end class

}  // end namespace

#endif
