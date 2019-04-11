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

#ifndef DEFAULT_TIMING_MODEL_H
#define DEFAULT_TIMING_MODEL_H

#include "core/Rsyn.h"
#include "session/Service.h"

#include "tool/scenario/Scenario.h"
#include "tool/routing/RoutingEstimator.h"
#include "TimingModel.h"

namespace Rsyn {

class Timer;

class DefaultTimingModel : public TimingModel,
                           public Service,
                           public Rsyn::DesignObserver {
       public:
        virtual void start(const Rsyn::Json &params) override;
        virtual void stop() override;

        virtual void onPostInstanceMove(Rsyn::Instance instance) override;

       private:
        // Design.
        Rsyn::Design clsDesign;

        // Services
        RoutingEstimator *clsRoutingEstimator = nullptr;
        Scenario *clsScenario = nullptr;
        Timer *clsTimer = nullptr;

        // Look-up table.
        double lookup(const ISPD13::LibParserLUT &lut, const double x,
                      const double y) const {
                const bool tweak = false;

                double weightX, weightY;
                double xLower, xUpper, yLower, yUpper;
                int xLowerIndex, xUpperIndex, yLowerIndex, yUpperIndex, xLimit,
                    yLimit;

                // If the table is empty issue a warning and return 0. Something
                // isn t
                // right.
                if (lut.loadIndices.empty() || lut.transitionIndices.empty()) {
                        std::cout << "WARNING: Empty look-up table. Prepare "
                                     "for things going wrong...\n";
                        return 0;
                }  // end if

                // We store the scalar delay/slew in a 1x1 lookup.
                if (lut.loadIndices.size() == 1 &&
                    lut.transitionIndices.size() == 1) {
                        return lut.tableVals(0, 0);
                }  // end if

                // If the input slew is uninitialized, return uninitialized
                // value.
                if (std::abs(y) == UNINITVALUE) {
                        return y;
                }  // end if

                // Find x, y indices.
                xLowerIndex = xUpperIndex = yLowerIndex = yUpperIndex = 0;
                xLimit = lut.loadIndices.size() - 2;
                yLimit = lut.transitionIndices.size() - 2;

                while ((xLowerIndex < xLimit) &&
                       (lut.loadIndices[xLowerIndex + 1] <= x))
                        ++xLowerIndex;
                xUpperIndex = xLowerIndex + 1;

                while ((yLowerIndex < yLimit) &&
                       (lut.transitionIndices[yLowerIndex + 1] <= y))
                        ++yLowerIndex;
                yUpperIndex = yLowerIndex + 1;

                xLower = lut.loadIndices[xLowerIndex];
                xUpper = lut.loadIndices[xUpperIndex];
                yLower = lut.transitionIndices[yLowerIndex];
                yUpper = lut.transitionIndices[yUpperIndex];

                // Truncate values an warn the user if necessary.
                double truncx = x;
                double truncy = y;

                if (tweak) {
                        if (x < xLower) {
                                truncx = xLower;
                                std::cout << "WARNING: Underflow in "
                                             "x-dimension of lookup table ("
                                          << x << " < " << xLower
                                          << "). Truncating...\n";
                        } else if (x > xUpper) {
                                truncx = xUpper;
                                std::cout << "WARNING: Overflow in x-dimension "
                                             "lookup table ("
                                          << x << " > " << xUpper
                                          << "). Truncating...\n";
                        } else {
                                truncx = x;
                        }  // end else

                        if (y < yLower) {
                                truncy = yLower;
                                std::cout << "WARNING: Underflow in "
                                             "y-dimension of lookup table ("
                                          << y << " < " << yLower
                                          << "). Truncating...\n";
                        } else if (y > yUpper) {
                                truncy = yUpper;
                                std::cout << "WARNING: Overflow in y-dimension "
                                             "lookup table ("
                                          << y << " > " << yUpper
                                          << "). Truncating...\n";
                        } else {
                                truncy = y;
                        }  // end else
                }          // end else

                // Interpolate.
                weightX = (truncx - xLower) / (xUpper - xLower);
                weightY = (truncy - yLower) / (yUpper - yLower);

                double result;
                result = (1.0 - weightX) * (1.0 - weightY) *
                         (lut.tableVals(xLowerIndex, yLowerIndex));
                result += (weightX) * (1.0 - weightY) *
                          (lut.tableVals(xUpperIndex, yLowerIndex));
                result += (1.0 - weightX) * (weightY) *
                          (lut.tableVals(xLowerIndex, yUpperIndex));
                result += (weightX) * (weightY) *
                          (lut.tableVals(xUpperIndex, yUpperIndex));

                return result;
        }  // end method

        EdgeArray<Number> computeNetPinLoad(Rsyn::Net net) {
                Number load = 0;
                for (Rsyn::Pin pin : net.allPins(Rsyn::SINK)) {
                        switch (pin.getInstanceType()) {
                                case Rsyn::CELL:
                                        load += getLibraryPinInputCapacitance(
                                            pin.getLibraryPin());
                                        break;
                                case Rsyn::PORT:
                                        load += clsScenario->getOutputLoad(
                                            pin.getInstance(), 0);
                                        break;
                        }  // end switch
                }          // end for
                return EdgeArray<Number>(load, load);
        }  // end method

        EdgeArray<Number> getSetupTime(
            Scenario::TimingLibraryPin &timingLibraryPin) const {
                // HARD CODED
                EdgeArray<Number> tsetup(0, 0);
                if (!timingLibraryPin.getSetupLut(Rsyn::RISE)
                         .tableVals.isEmpty()) {
                        tsetup[RISE] =
                            (Number)timingLibraryPin.getSetupLut(Rsyn::RISE)
                                .tableVals(0, 0);
                        tsetup[FALL] =
                            (Number)timingLibraryPin.getSetupLut(Rsyn::FALL)
                                .tableVals(0, 0);
                }  // end if
                return tsetup;
        }  // end method

        EdgeArray<Number> getHoldTime(
            const Scenario::TimingLibraryPin &timingLibraryPin) const {
                // HARD CODED
                EdgeArray<Number> thold(0, 0);
                if (!timingLibraryPin.getHoldLut(Rsyn::RISE)
                         .tableVals.isEmpty()) {
                        thold[RISE] =
                            (Number)timingLibraryPin.getHoldLut(Rsyn::RISE)
                                .tableVals(0, 0);
                        thold[FALL] =
                            (Number)timingLibraryPin.getHoldLut(Rsyn::FALL)
                                .tableVals(0, 0);
                }  // end if
                return thold;
        }  // end method

       public:
        DefaultTimingModel() : clsRoutingEstimator(nullptr) {}

        void beforeTimingUpdate() {
                clsRoutingEstimator->updateRouting();
        }  // end method

        virtual TimingSense getLibraryArcSense(
            const Rsyn::LibraryArc libraryArc) {
                return clsScenario->getTimingLibraryArc(libraryArc).getSense();
        }  // end method

        virtual void calculateInputDriverTiming(Rsyn::Instance port,
                                                const TimingMode mode,
                                                const EdgeArray<Number> &load,
                                                EdgeArray<Number> &inputDelay,
                                                EdgeArray<Number> &inputSlew) {}

        virtual void calculateLibraryArcTiming(const Rsyn::LibraryArc larc,
                                               const TimingMode mode,
                                               const TimingTransition oedge,
                                               const Number islew,
                                               const Number load, Number &delay,
                                               Number &slew) {
                const Scenario::TimingLibraryArc &timingLibraryArc =
                    clsScenario->getTimingLibraryArc(larc);
                delay = (Number)lookup(
                    timingLibraryArc.getDelayLut(mode, oedge), load, islew);
                slew = (Number)lookup(timingLibraryArc.getSlewLut(mode, oedge),
                                      load, islew);
        }  // end method

        virtual void calculateLoadCapacitance(const Rsyn::Pin pin,
                                              const TimingMode mode,
                                              EdgeArray<Number> &load) {
                Rsyn::Net net = pin.getNet();
                if (net) {
                        RCTree &tree =
                            clsRoutingEstimator->getRCTree(pin.getNet());

                        if (tree.hasUserSpecifiedWireLoad()) {
                                load.setBoth(tree.getUserSpecifiedWireLoad());
                                load += computeNetPinLoad(net);
                        } else {
                                if (!tree.isIdeal()) {
                                        // Get the cached lumped capacitance
                                        // that already includes
                                        // the pin cap.
                                        load = tree.getLumpedCap();
                                } else {
                                        // No wire load. Just return the pin
                                        // loads.
                                        load = computeNetPinLoad(net);
                                }  // end else
                        }          // end if
                } else {
                        load.set(0, 0);
                }  // end else
        }          // end method

        virtual void prepareNet(const Rsyn::Net net, const TimingMode mode,
                                const EdgeArray<Number> &slew) {
                // WARNING: Note that we use the same tree to compute the early
                // and
                // late Elmore timing. Therefore, if the second simulation will
                // overwrite the timing values of the previous one. For now,
                // this
                // is ok because the timer query the timing values before
                // requesting
                // the next simulation, but may be a problem in the future. A
                // simple
                // solution is to store both early and late values, but then we
                // hit
                // memory usage.
                RCTree &tree = clsRoutingEstimator->getRCTree(net);
                if (tree.getNumNodes() > 0) {
                        tree.setInputSlew(slew);
                        tree.elmore();
                }  // end if
        }          // end method

        // [TODO] Optimize, too many indirections here...
        // [TODO] Optimize, only need to go over tree nodes connected to pins...
        virtual void calculateNetArcTiming(
            const Rsyn::Pin driver, const Rsyn::Pin sink, const TimingMode mode,
            const EdgeArray<Number> &slewAtDriver, EdgeArray<Number> &delay,
            EdgeArray<Number> &oslew) {
                Rsyn::Net net = driver.getNet();
                const RCTree &tree = clsRoutingEstimator->getRCTree(net);
                const int numNodes = tree.getNumNodes();
                if (numNodes > 0 && !tree.isIdeal()) {
                        for (int i = 1; i < numNodes;
                             i++) {  // start @ 1 to skip root node
                                Rsyn::Pin pin = tree.getNodeTag(i).getPin();

                                // TODO: horrible, only a temporary solution,
                                // needs rethinking.
                                // It's only not that bad as most of nets are
                                // small, but still
                                // it's awful.
                                if (pin != sink) continue;

                                const RCTree::Node &node = tree.getNode(i);
                                delay = node.propDelay;
                                oslew = node.propSlew;
                                return;  // ugly
                        }                // end for
                        assert(false);   // ugly
                } else {
                        delay.set(0, 0);
                        oslew = slewAtDriver;
                }  // end else
        }          // end method

        virtual EdgeArray<Number> getSetupTime(Rsyn::Pin data) const {
                return getSetupTime(clsScenario->getTimingLibraryPin(data));
        }  // end method

        virtual EdgeArray<Number> getHoldTime(Rsyn::Pin data) const {
                return getHoldTime(clsScenario->getTimingLibraryPin(data));
        }  // end method

        virtual Number getLibraryPinInputCapacitance(
            Rsyn::LibraryPin lpin) const {
                const Scenario::TimingLibraryPin &timingLibraryPin =
                    clsScenario->getTimingLibraryPin(lpin);
                return timingLibraryPin.getCapacitance();
        }  // end method

        virtual Number getPinInputCapacitance(Rsyn::Pin pin) const {
                if (pin.isPort()) {
                        return 0;
                } else {
                        return getLibraryPinInputCapacitance(
                            pin.getLibraryPin());
                }  // end else
        }          // end method

        ////////////////////////////////////////////////////////////////////////////
        // Sandbox
        ////////////////////////////////////////////////////////////////////////////

        virtual void calculateLoadCapacitance(const Rsyn::SandboxPin pin,
                                              const TimingMode mode,
                                              EdgeArray<Number> &load) {
                load.set(0, 0);
        }  // end method

        virtual void prepareNet(const Rsyn::SandboxNet net,
                                const TimingMode mode,
                                const EdgeArray<Number> &slew) {
                // Note: Nothing to do (interconnections not implemented yet).
        }  // end method

        virtual void calculateNetArcTiming(
            const Rsyn::SandboxPin driver, const Rsyn::SandboxPin sink,
            const TimingMode mode, const EdgeArray<Number> &slewAtDriver,
            EdgeArray<Number> &delay, EdgeArray<Number> &slew) {
                // Note: Assuming ideal interconnect for now.
                delay.set(0, 0);
                slew = slewAtDriver;
        }  // end method

        virtual EdgeArray<Number> getSetupTime(Rsyn::SandboxPin data) const {
                return getSetupTime(
                    clsScenario->getTimingLibraryPin(data.getLibraryPin()));
        }  // end method

        virtual EdgeArray<Number> getHoldTime(Rsyn::SandboxPin data) const {
                return getHoldTime(
                    clsScenario->getTimingLibraryPin(data.getLibraryPin()));
        }  // end method

};  // end class

}  // end namespace

#endif
