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

#include <iostream>
#include <iomanip>
#include <cassert>

#include "LibraryCharacterizer.h"
#include "util/FloatingPoint.h"
#include "session/Session.h"
#include "util/Debug.h"
#include "tool/scenario/Scenario.h"
#include "util/StreamStateSaver.h"

namespace Rsyn {

void LibraryCharacterizer::start(const Rsyn::Json &params) {}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::stop() {}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::runLibraryCharacterization(
    TimingModel *timingModel) {
        if (clsAnalysisPerformed && clsTimingModel == timingModel) {
                return;
        }  // end if

        clsAnalysisPerformed = false;

        Rsyn::Session session;
        clsDesign = session.getDesign();
        clsLibrary = session.getLibrary();
        clsScenario = session.getService("rsyn.scenario");
        clsTimingModel = timingModel;
        clsLibraryMaxDriverResistance[EARLY] =
            -std::numeric_limits<Number>::max();
        clsLibraryMaxDriverResistance[LATE] =
            -std::numeric_limits<Number>::max();
        clsLibraryMinDriverResistance[EARLY] =
            std::numeric_limits<Number>::max();
        clsLibraryMinDriverResistance[LATE] =
            std::numeric_limits<Number>::max();

        // Creates an attribute to hold the characterization data.
        clsLibraryArcCharacterizations = clsDesign.createAttribute();

        clsTypicalValues = doTypicalAnalysis(4);
        doGainBasedSlewModelAnalysis();
        doLogicalEffortAnalysis();

        clsAnalysisPerformed = true;
}  // end method

// -----------------------------------------------------------------------------

LibraryCharacterizer::TypicalValues LibraryCharacterizer::doTypicalAnalysis(
    const float typicalFanout) {
        TypicalValues result;
        result.fanout = typicalFanout;

        int counter = 0;
        EdgeArray<Number> sumDelay(0, 0);
        EdgeArray<Number> sumDelayPerLeakage(0, 0);
        EdgeArray<Number> sumSlew(0, 0);
        EdgeArray<Number> sumResistance(0, 0);
        EdgeArray<Number> sumDelayToSlewSensitivity(0, 0);

        // Compute the logical effort for each timing arc.
        for (Rsyn::LibraryCell lcell : clsLibrary.allLibraryCells()) {
                Rsyn::LibraryArc larc = lcell.getAnyLibraryArc();

                // Check if this is an inverter.
                const bool isInverter = lcell.getNumInputPins() == 1 &&
                                        lcell.getNumOutputPins() == 1 &&
                                        clsTimingModel->getLibraryArcSense(
                                            larc) == Rsyn::NEGATIVE_UNATE;

                // Skip is is not an inverter.
                if (!isInverter) continue;

                // Compute fanout-of-n.
                EdgeArray<Number> delay;
                EdgeArray<Number> slew;
                EdgeArray<Number> resistance;
                EdgeArray<Number> delayToSlewSensitivity;
                computeFanoutOfNDelay(larc, typicalFanout, delay, slew,
                                      resistance, delayToSlewSensitivity);

                const Number leakage =
                    clsScenario->getLibraryCellLeakagePower(lcell);

                sumDelay += delay;
                sumDelayPerLeakage += delay / leakage;
                sumSlew += slew;
                sumResistance += resistance;
                sumDelayToSlewSensitivity += delayToSlewSensitivity;

                counter++;
        }  // end for cells

        rsynAssert(counter > 0,
                   "No inverters found in the library for characterization.");

        const EdgeArray<Number> avgDelay = sumDelay / counter;
        const EdgeArray<Number> avgDelayPerLeakage =
            sumDelayPerLeakage / counter;
        const EdgeArray<Number> avgSlew = sumSlew / counter;
        const EdgeArray<Number> avgResistance = sumResistance / counter;
        const EdgeArray<Number> avgDelayToSlewSensitivity =
            sumDelayToSlewSensitivity / counter;

        result.delay = avgDelay.getAvg();
        result.delayPerLeakage = avgDelayPerLeakage.getAvg();
        result.slew = avgSlew.getAvg();
        result.resistance = avgResistance.getAvg();
        result.delayToSlewSensitivity = avgDelayToSlewSensitivity.getAvg();
        return result;
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::doGainBasedSlewModelAnalysis() {
        const int N = 32;

        std::vector<Number> gains;
        std::vector<Number> slews;
        for (int gain = 1; gain <= N; gain *= 2) {
                const TypicalValues typicalValues = doTypicalAnalysis(gain);
                gains.push_back(gain);
                slews.push_back(typicalValues.slew);
        }  // end for

        Number a, b;
        logicalEffort_LinearLeastSquares(gains, slews, a, b);
        clsGainBasedSlewSlope = a;
        clsGainBasedSlewConstant = b;
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::doLogicalEffortAnalysis() {
        logicalEffort_FindReferenceLibraryTimingArc();
        logicalEffort_ClaculateReferenceSlew();

        // Define gains where the delay will be computed.
        const int N = 32 + 1;  // 0 ... 32
        clsLogicalEffort_Gains.resize(N);
        for (int i = 0; i < N; i++) {
                clsLogicalEffort_Gains[i] = i;
        }  // end for

        // Compute the logical effort for each timing arc.
        for (Rsyn::LibraryCell lcell : clsLibrary.allLibraryCells()) {
                for (Rsyn::LibraryArc larc : lcell.allLibraryArcs()) {
                        Rsyn::LibraryPin lpin = larc.getFromLibraryPin();

                        LibraryArcCharacterization &timingLibraryArc =
                            getLibraryArcCharacterization(larc);
                        timingLibraryArc.sense =
                            clsTimingModel->getLibraryArcSense(larc);

                        for (std::tuple<TimingMode, TimingTransition> element :
                             allTimingModeAndTransitionPairs()) {
                                const TimingMode mode = std::get<0>(element);
                                const TimingTransition oedge =
                                    std::get<1>(element);

                                LibraryArcCharacterization::LogicalEffort
                                    &arcLe = timingLibraryArc.le[mode];

                                // [NOTE] Since the our infrastructure does not
                                // takes into
                                // account different capacitance of each
                                // transition, there's
                                // nothing to be done in this switch. But we let
                                // it here for
                                // future use.
                                switch (timingLibraryArc.sense) {
                                        case POSITIVE_UNATE:
                                                arcLe.cin[RISE] =
                                                    clsTimingModel
                                                        ->getLibraryPinInputCapacitance(
                                                            lpin);  // [RISE]
                                                arcLe.cin[FALL] =
                                                    clsTimingModel
                                                        ->getLibraryPinInputCapacitance(
                                                            lpin);  // [FALL]

                                                arcLe.slew[RISE] =
                                                    clsLogicalEffort_ReferenceSlew
                                                        [mode][RISE];
                                                arcLe.slew[FALL] =
                                                    clsLogicalEffort_ReferenceSlew
                                                        [mode][FALL];
                                                break;

                                        case NEGATIVE_UNATE:
                                                arcLe.cin[RISE] =
                                                    clsTimingModel
                                                        ->getLibraryPinInputCapacitance(
                                                            lpin);  // [FALL];
                                                arcLe.cin[FALL] =
                                                    clsTimingModel
                                                        ->getLibraryPinInputCapacitance(
                                                            lpin);  // [RISE];

                                                arcLe.slew[RISE] =
                                                    clsLogicalEffort_ReferenceSlew
                                                        [mode][FALL];
                                                arcLe.slew[FALL] =
                                                    clsLogicalEffort_ReferenceSlew
                                                        [mode][RISE];
                                                break;

                                        case NON_UNATE:
                                                arcLe.cin[RISE] = arcLe.cin
                                                                      [FALL] =
                                                    (clsTimingModel
                                                         ->getLibraryPinInputCapacitance(
                                                             lpin) /*[RISE]*/ +
                                                     clsTimingModel
                                                         ->getLibraryPinInputCapacitance(
                                                             lpin) /*[FALL]*/) /
                                                    2;

                                                arcLe.slew[RISE] = arcLe.slew
                                                                       [FALL] =
                                                    (clsLogicalEffort_ReferenceSlew
                                                         [mode][FALL] +
                                                     clsLogicalEffort_ReferenceSlew
                                                         [mode][RISE]) /
                                                    2;
                                                break;

                                        default:
                                                assert(false);
                                }  // end switch

                                logicalEffort_TimingArc(
                                    larc, mode, oedge, arcLe.cin[oedge],
                                    clsLogicalEffort_ReferenceSlew[mode][oedge],
                                    clsLogicalEffort_Gains, arcLe.g[oedge],
                                    arcLe.p[oedge], arcLe.residuum[oedge]);
                                arcLe.valid[oedge] = 1;
                        }  // end for

                        clsLibraryMaxDriverResistance[EARLY] =
                            std::max(clsLibraryMaxDriverResistance[EARLY],
                                     getDriverResistance(larc, EARLY));
                        clsLibraryMaxDriverResistance[LATE] =
                            std::max(clsLibraryMaxDriverResistance[LATE],
                                     getDriverResistance(larc, LATE));
                        if (getDriverResistance(larc, EARLY, RISE)) {
                                clsLibraryMinDriverResistance[EARLY] = std::min(
                                    clsLibraryMinDriverResistance[EARLY],
                                    getDriverResistance(larc, EARLY, RISE));
                        }  // end if
                        if (getDriverResistance(larc, EARLY, FALL)) {
                                clsLibraryMinDriverResistance[EARLY] = std::min(
                                    clsLibraryMinDriverResistance[EARLY],
                                    getDriverResistance(larc, EARLY, FALL));
                        }  // end if
                        if (getDriverResistance(larc, LATE, RISE)) {
                                clsLibraryMinDriverResistance[LATE] = std::min(
                                    clsLibraryMinDriverResistance[LATE],
                                    getDriverResistance(larc, LATE, RISE));
                        }  // end if
                        if (getDriverResistance(larc, LATE, FALL)) {
                                clsLibraryMinDriverResistance[LATE] = std::min(
                                    clsLibraryMinDriverResistance[LATE],
                                    getDriverResistance(larc, LATE, FALL));
                        }  // end if
                }          // end for
        }                  // end for cells
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::logicalEffort_FindReferenceLibraryTimingArc() {
        const bool report = false;

        // Look for the timing arc with the small delay when driven a zero load.
        // Usually this should be the smaller inverter.

        clsLogicalEffort_ReferenceLibraryArcPointer[EARLY] = nullptr;
        clsLogicalEffort_ReferenceLibraryArcPointer[LATE] = nullptr;

        clsLogicalEffort_ReferenceLibraryCell[EARLY] = nullptr;
        clsLogicalEffort_ReferenceLibraryCell[LATE] = nullptr;

        Number smallerDelay[NUM_TIMING_MODES] = {
            +std::numeric_limits<Number>::infinity(),
            +std::numeric_limits<Number>::infinity()};

        for (Rsyn::LibraryCell lcell : clsLibrary.allLibraryCells()) {
                for (const TimingMode mode : allTimingModes()) {
                        for (Rsyn::LibraryArc larc : lcell.allLibraryArcs()) {
                                Number avgDelay = 0;
                                Number delay;
                                Number oslew;

                                clsTimingModel->calculateLibraryArcTiming(
                                    larc, mode, RISE, 0, 0, delay, oslew);
                                avgDelay += delay;
                                clsTimingModel->calculateLibraryArcTiming(
                                    larc, mode, FALL, 0, 0, delay, oslew);
                                avgDelay += delay;

                                avgDelay /= 2;

                                if (avgDelay < smallerDelay[mode]) {
                                        clsLogicalEffort_ReferenceLibraryArcPointer
                                            [mode] = larc;
                                        clsLogicalEffort_ReferenceLibraryCell
                                            [mode] = lcell;
                                        smallerDelay[mode] = avgDelay;
                                        if (report) {
                                                std::cout << "New ref larc '"
                                                          << larc.getFullName()
                                                          << " " << avgDelay
                                                          << "\n";
                                        }  // end if
                                }          // end if
                        }                  // end for
                }                          // end for
        }                                  // end for
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::logicalEffort_ClaculateReferenceSlew() {
        const bool report = false;
        const int maxIterations = 100;
        const Number h = 1;

        for (const TimingMode mode : allTimingModes()) {
                Rsyn::LibraryArc larc =
                    clsLogicalEffort_ReferenceLibraryArcPointer[mode];
                Rsyn::LibraryPin lpin = larc.getFromLibraryPin();

                const LibraryArcCharacterization &timingLibraryArc =
                    getLibraryArcCharacterization(larc);

                for (const TimingTransition edge : allTimingTransitions()) {
                        const Number Cin =
                            clsTimingModel->getLibraryPinInputCapacitance(lpin);
                        const Number load = h * Cin;
                        if (report) {
                                std::cout << "Computing reference slew for "
                                          << (edge == RISE ? "RISE" : "FALL")
                                          << " transition...\n";
                                std::cout
                                    << "reference arc : " << larc.getFullName()
                                    << "\n";
                                std::cout << "reference gain: " << h << "\n";
                                std::cout << "reference load: " << load << "\n";
                                std::cout << "edge: "
                                          << (edge == FALL ? "FALL" : "RISE")
                                          << "\n";
                        }  // end if

                        Number currSlew = 0;
                        Number prevSlew = 0;

                        for (int i = 0; i < maxIterations; i++) {
                                Number dummy;
                                clsTimingModel->calculateLibraryArcTiming(
                                    larc, mode, edge, prevSlew, load, dummy,
                                    currSlew);

                                if (report) {
                                        std::cout
                                            << std::setw(2) << i << " "
                                            << (edge == FALL ? "FALL" : "RISE")
                                            << " " << prevSlew << " "
                                            << currSlew << " "
                                            << "\n";
                                }  // end if
                                if (FloatingPoint::approximatelyEqual(prevSlew,
                                                                      currSlew))
                                        break;
                                prevSlew = currSlew;
                        }  // end for

                        clsLogicalEffort_ReferenceSlew[mode][edge] = currSlew;

                        if (report) {
                                std::cout
                                    << "Reference Slew: "
                                    << (mode == EARLY ? "EARLY" : "LATE") << " "
                                    << (edge == FALL ? "FALL" : "RISE") << " "
                                    << clsLogicalEffort_ReferenceSlew[mode]
                                                                     [edge]
                                    << "\n";
                        }  // end if

                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

// Adapted from https://people.sc.fsu.edu/~jburkardt/cpp_src/llsq/llsq.cpp
// y = a * x + b

void LibraryCharacterizer::logicalEffort_LinearLeastSquares(
    const std::vector<Number> &x, const std::vector<Number> &y, Number &a,
    Number &b) {
        const int n = x.size();

        Number bot;
        int i;
        Number top;
        Number xbar;
        Number ybar;

        if (n == 1) {
                //  Special case.
                a = 0.0;
                b = y[0];
        } else {
                //  Average X and Y.
                xbar = 0.0;
                ybar = 0.0;
                for (i = 0; i < n; i++) {
                        xbar = xbar + x[i];
                        ybar = ybar + y[i];
                }  // end for
                xbar = xbar / (Number)n;
                ybar = ybar / (Number)n;

                //  Compute Beta.
                top = 0.0;
                bot = 0.0;
                for (i = 0; i < n; i++) {
                        top = top + (x[i] - xbar) * (y[i] - ybar);
                        bot = bot + (x[i] - xbar) * (x[i] - xbar);
                }  // end for
                a = top / bot;
                b = ybar - a * xbar;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::logicalEffort_LinearLeastSquaresError(
    const std::vector<Number> &x, const std::vector<Number> &y, const Number a,
    const Number b, Number &residuum) {
        const int n = x.size();

        residuum = 0;
        for (int i = 0; i < n; i++) {
                const Number estimate = a * x[i] + b;
                const Number error = std::pow(y[i] - estimate, 2.0f);
                // cout << "h=" << x[i] << " delay=" << y[i] << " estimate=" <<
                // (estimate) << "\n";
                residuum += error;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::logicalEffort_TimingArc(
    Rsyn::LibraryArc larc, const TimingMode mode, const TimingTransition oedge,
    const Number Cin, const Number inputSlew, const std::vector<Number> &gains,
    Number &g, Number &p, Number &residuum) {
        const int N = gains.size();

        std::vector<Number> delays(N);

        for (int i = 0; i < N; i++) {
                const Number h = gains[i];
                const Number load = h * Cin;

                Number delay;
                Number oslew;
                clsTimingModel->calculateLibraryArcTiming(
                    larc, mode, oedge, inputSlew, load, delay, oslew);
                delays[i] = delay;
        }  // end for

        logicalEffort_LinearLeastSquares(gains, delays, g, p);
        logicalEffort_LinearLeastSquaresError(gains, delays, g, p, residuum);
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::computeFanoutOfNDelay(
    Rsyn::LibraryArc larc, const int n, EdgeArray<Number> &delay,
    EdgeArray<Number> &slew, EdgeArray<Number> &resistance,
    EdgeArray<Number> &delayToSlewSensitivity, const int numIterations) const {
        // Get the input pin.
        Rsyn::LibraryPin lpin = larc.getFromLibraryPin();

        // Compute the load.
        const Number load =
            clsTimingModel->getLibraryPinInputCapacitance(lpin) * n;

        // Get the sense (use negative unate for non-unate arcs for now).
        const TimingSense sense =
            clsTimingModel->getLibraryArcSense(larc) == NON_UNATE
                ? NEGATIVE_UNATE
                : clsTimingModel->getLibraryArcSense(larc);

        // Compute the delay and slew iterating a few times in order to
        // make the slew converge.
        delay.set(0, 0);
        slew.set(0, 0);
        resistance.set(0, 0);
        for (int i = 0; i < numIterations; i++) {
                for (const Rsyn::TimingTransition oedge :
                     allTimingTransitions()) {
                        const TimingTransition iedge =
                            sense == POSITIVE_UNATE
                                ? oedge
                                : Rsyn::REVERSE_EDGE_TYPE[oedge];
                        clsTimingModel->calculateLibraryArcTiming(
                            larc, Rsyn::LATE, oedge, slew[iedge], load,
                            delay[oedge], slew[oedge]);
                        resistance[oedge] =
                            delay[oedge] / load;  // RC = d --> R = d/C
                }                                 // end for
        }                                         // end for

        // Compute delay to (input) slew sensitivity.
        const Number delta = 0.1 * slew.getAvg();

        for (Rsyn::TimingTransition oedge : Rsyn::allTimingTransitions()) {
                const TimingTransition iedge =
                    sense == POSITIVE_UNATE ? oedge
                                            : Rsyn::REVERSE_EDGE_TYPE[oedge];

                Number delay0, delay1, outputSlew0, outputSlew1;
                clsTimingModel->calculateLibraryArcTiming(
                    larc, Rsyn::LATE, oedge, slew[iedge] - delta, load, delay0,
                    outputSlew0);
                clsTimingModel->calculateLibraryArcTiming(
                    larc, Rsyn::LATE, oedge, slew[iedge] + delta, load, delay1,
                    outputSlew1);
                delayToSlewSensitivity[oedge] =
                    (delay1 - delay0) / (2.0f * delta);
                // inputSlewToOutputSlewSensitivity[oedge] = (outputSlew1 -
                // outputSlew0) / (2.0f * delta);
        }  // end for

}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::reportLogicalEffort(std::ostream &out) {
        out << std::string(80, '-') << "\n";
        out << "Logical Effort Report\n";
        out << std::string(80, '-') << "\n";

        // Compute the logical effort for each timing arc.
        for (Rsyn::LibraryCell lcell : clsLibrary.allLibraryCells()) {
                out << "Cell: " << lcell.getName() << "\n";

                for (Rsyn::LibraryArc larc : lcell.allLibraryArcs()) {
                        const LibraryArcCharacterization &timingLibraryArc =
                            getLibraryArcCharacterization(larc);

                        for (std::tuple<TimingMode, TimingTransition> element :
                             allTimingModeAndTransitionPairs()) {
                                const TimingMode mode = std::get<0>(element);
                                const TimingTransition oedge =
                                    std::get<1>(element);

                                const LibraryArcCharacterization::LogicalEffort
                                    &arcLe = timingLibraryArc.le[mode];

                                out << "\t" << larc.getName() << ": "
                                    << "mode="
                                    << (mode == EARLY ? "EARLY" : "LATE") << " "
                                    << "edge="
                                    << (oedge == RISE ? "RISE" : "FALL") << " "
                                    << "g=" << arcLe.g[oedge] << " "
                                    << "p=" << arcLe.p[oedge] << " "
                                    << "cin=" << arcLe.cin[oedge] << " "
                                    << "slew=" << arcLe.slew[oedge] << " "
                                    << "error=" << logicalEffort_MeanMSE(
                                                       arcLe.residuum[oedge])
                                    << " "
                                    << "valid=" << arcLe.valid[oedge] << "\n";

                                out << "\t\t"
                                    << "Delay Estimation via Logical Effort\n";

                                out << "\t\t";
                                out << std::setw(4) << "gain";
                                out << std::setw(9) << "load";
                                out << std::setw(9) << "delay";
                                out << std::setw(9) << "le_delay";
                                out << "\n";

                                const LibraryArcCharacterization::LogicalEffort
                                    &le = timingLibraryArc.le[mode];

                                for (unsigned i = 0;
                                     i < clsLogicalEffort_Gains.size(); i++) {
                                        const Number h =
                                            clsLogicalEffort_Gains[i];
                                        const Number load =
                                            h * le.cin[oedge];  // cin is timing
                                                                // sense aware
                                        const Number le_delay =
                                            le.p[oedge] + le.g[oedge] * h;

                                        Number libraryDelay;
                                        Number librarySlew;
                                        clsTimingModel
                                            ->calculateLibraryArcTiming(
                                                larc, mode, oedge,
                                                le.slew[oedge], load,
                                                libraryDelay,
                                                librarySlew);  // le.slew is
                                                               // timing sense
                                                               // aware

                                        out << "\t\t";
                                        out << std::setw(4) << h;
                                        out << std::setw(9) << load;
                                        out << std::setw(9) << libraryDelay;
                                        out << std::setw(9) << le_delay;
                                        out << "\n";
                                }  // end for
                        }  // end for each timing mode / timing transition
                }          // end for each library arc
        }                  // end for
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::reportTypicalValues(std::ostream &out) {
        out << std::string(80, '-') << "\n";
        out << "Typical Library Values\n";
        out << std::string(80, '-') << "\n";
        out << "Delay : " << getTypicalDelay()
            << Units::getDefaultInternalUnitString(MEASURE_TIME) << "\n";
        out << "Delay per Leakage : " << getTypicalDelayPerLeakage()
            << Units::getDefaultInternalUnitString(MEASURE_TIME) << "/"
            << Units::getDefaultInternalUnitString(MEASURE_POWER) << "\n";
        out << "Slew : " << getTypicalSlew()
            << Units::getDefaultInternalUnitString(MEASURE_TIME) << "\n";
        out << "Resistance : " << getTypicalResistance()
            << Units::getDefaultInternalUnitString(MEASURE_RESISTANCE) << "\n";
        out << "Capacitance : " << getTypicalCapacitance()
            << Units::getDefaultInternalUnitString(MEASURE_CAPACITANCE) << "\n";
        out << "Delay to Slew Sensitivity : "
            << getTypicalDelayToSlewSensitivity() << "\n";
        out << "Leakage : " << getTypicalLeakage()
            << Units::getDefaultInternalUnitString(MEASURE_POWER) << "\n";
        out << "\n";
}  // end method

// -----------------------------------------------------------------------------

void LibraryCharacterizer::reportGainBasedSlewModel(std::ostream &out) {
        StreamStateSaver sss(out);
        out << std::setprecision(2) << std::fixed;

        out << std::string(80, '-') << "\n";
        out << "Gain Based Model\n";
        out << std::string(80, '-') << "\n";
        out << "Slope : " << getGainBasedSlewSlope() << "\n";
        out << "Constant : " << getGainBasedSlewConstant() << "\n";
        out << "\n";

        out << std::setw(4) << "Gain"
            << " " << std::setw(10) << "Slew"
            << "\n";
        for (int i = 1; i <= 32; i *= 2) {
                out << std::setw(4) << i << " " << std::setw(10)
                    << getGainBasedSlew(i) << "\n";
        }  // end for
        out << "\n";
}  // end method

}  // end namespace
