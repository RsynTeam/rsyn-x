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

#include "Scenario.h"
#include "session/Session.h"

namespace Rsyn {

void Scenario::start(const Rsyn::Json &params) {
        Rsyn::Session session;
        Rsyn::Design design = session.getDesign();

        // Observe changes in the netlist.
        design.registerObserver(this);
}  // end method

// -----------------------------------------------------------------------------

Scenario::Scenario() { clsClockPeriod = 0; }  // end method

// -----------------------------------------------------------------------------

void Scenario::init(Rsyn::Design design, Rsyn::Library library,
                    const ISPD13::LIBInfo &libInfosEarly,
                    const ISPD13::LIBInfo &libInfosLate,
                    const ISPD13::SDCInfo &sdc) {
        clsDesign = design;
        clsLibrary = library;

        // Create timing arc attributes.
        clsTimingLibraryCells = clsDesign.createAttribute();
        clsTimingLibraryPins = clsDesign.createAttribute();
        clsTimingLibraryArcs = clsDesign.createAttribute();

        // Initialize timing modes.
        init_Mode(EARLY, libInfosEarly);
        init_Mode(LATE, libInfosLate);

        // Initialize constraints.
        init_Constraints(sdc);

        // Initialize net type tags.
        init_NetTypeTags();
        init_MissingLibraryCellTags();
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Liberty
////////////////////////////////////////////////////////////////////////////////

void Scenario::init_Mode(const TimingMode mode, const ISPD13::LIBInfo &lib) {
        for (const ISPD13::LibParserCellInfo &libCell : lib.libCells) {
                Rsyn::LibraryCell rsynLibraryCell =
                    clsDesign.findLibraryCellByName(libCell.name);
                Rsyn::LibraryCellTag rsynLibraryCellTag =
                    clsDesign.getTag(rsynLibraryCell);

                if (libCell.isSequential) {
                        rsynLibraryCellTag.setLogicType(
                            Rsyn::LOGIC_TYPE_TAG_SEQUENTIAL);
                } else {
                        rsynLibraryCellTag.setLogicType(
                            Rsyn::LOGIC_TYPE_TAG_COMBINATIONAL);
                }  // end else

                if (libCell.isTieLow) {
                        rsynLibraryCellTag.setTieType(Rsyn::TIE_TYPE_TAG_LOW);
                } else if (libCell.isTieHigh) {
                        rsynLibraryCellTag.setTieType(Rsyn::TIE_TYPE_TAG_HIGH);
                } else {
                        rsynLibraryCellTag.setTieType(Rsyn::TIE_TYPE_TAG_NONE);
                }  // end else

                if (libCell.timingArcs.size() == 1) {
                        rsynLibraryCellTag.setBufferType(
                            libCell.timingArcs[0].timingSense ==
                                    "positive_unate"
                                ? Rsyn::BUFFER_TYPE_TAG_NON_INVERTING
                                : Rsyn::BUFFER_TYPE_TAG_INVERTING);
                } else {
                        rsynLibraryCellTag.setBufferType(
                            Rsyn::BUFFER_TYPE_TAG_NONE);
                }  // end if

                for (const ISPD13::LibParserTimingInfo &libArc :
                     libCell.timingArcs) {
                        if (isUnusualTimingArc(libArc)) continue;

                        TimingLibraryArc &timingLibraryArc =
                            getTimingLibraryArc(
                                rsynLibraryCell.getLibraryArcByPinNames(
                                    libArc.fromPin, libArc.toPin));

                        timingLibraryArc.sense =
                            getTimingSenseFromString(libArc.timingSense);

                        setTimingLibraryArcDelayModel(timingLibraryArc, mode,
                                                      RISE, libArc.riseDelay);
                        setTimingLibraryArcDelayModel(timingLibraryArc, mode,
                                                      FALL, libArc.fallDelay);

                        setTimingLibraryArcSlewModel(timingLibraryArc, mode,
                                                     RISE,
                                                     libArc.riseTransition);
                        setTimingLibraryArcSlewModel(timingLibraryArc, mode,
                                                     FALL,
                                                     libArc.fallTransition);
                }  // end for

                for (const ISPD13::LibParserPinInfo &libPin : libCell.pins) {
                        Rsyn::LibraryPin rsynLibraryPin =
                            rsynLibraryCell.getLibraryPinByName(libPin.name);
                        Rsyn::LibraryCell rsynLibraryCell =
                            rsynLibraryPin.getLibraryCell();

                        TimingLibraryPin &timingLibraryPin =
                            getTimingLibraryPin(rsynLibraryPin);
                        setTimingLibraryPinCapacitance(
                            timingLibraryPin, (Number)libPin.capacitance);

                        if (rsynLibraryPin.getDirection() == Rsyn::OUT) {
                                setTimingLibraryPinMaxCapacitance(
                                    timingLibraryPin,
                                    (Number)libPin.maxCapacitance);
                        }  // end if

                        if (rsynLibraryPin.getDirection() == Rsyn::IN) {
                                if (libPin.maxTransition ==
                                    -std::numeric_limits<Number>::infinity())
                                        setTimingLibraryPinMaxTransition(
                                            timingLibraryPin,
                                            (Number)lib.default_max_transition);
                                else
                                        setTimingLibraryPinMaxTransition(
                                            timingLibraryPin,
                                            (Number)libPin.maxTransition);
                        }  // end if

                        if (libPin.isTimingEndpoint) {
                                Rsyn::LibraryPin rsynRelated =
                                    rsynLibraryCell.getLibraryPinByName(
                                        libPin.related);
                                TimingLibraryPin &timingLibraryPinRelated =
                                    getTimingLibraryPin(rsynRelated);

                                if (mode == EARLY) {
                                        setTimingLibraryPinHoldConstraint(
                                            timingLibraryPin,
                                            timingLibraryPinRelated, RISE,
                                            libPin.riseHold);
                                        setTimingLibraryPinHoldConstraint(
                                            timingLibraryPin,
                                            timingLibraryPinRelated, FALL,
                                            libPin.fallHold);
                                } else {
                                        setTimingLibraryPinSetupConstraint(
                                            timingLibraryPin,
                                            timingLibraryPinRelated, RISE,
                                            libPin.riseSetup);
                                        setTimingLibraryPinSetupConstraint(
                                            timingLibraryPin,
                                            timingLibraryPinRelated, FALL,
                                            libPin.fallSetup);
                                }  // end else
                        }          // end if
                }                  // end for

                TimingLibraryCell &timingLibraryCell =
                    getTimingLibraryCell(rsynLibraryCell);
                timingLibraryCell.leakagePower = (Number)libCell.leakagePower;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

bool Scenario::isUnusualTimingArc(
    const ISPD13::LibParserTimingInfo &libArc) const {
        if (libArc.timingSense != "non_unate" &&
            libArc.timingSense != "positive_unate" &&
            libArc.timingSense != "negative_unate") {
                std::cout << "unusual sense: " << libArc.timingSense << "\n";
                return true;
        }  // end if

        if (libArc.timingType != "" && libArc.timingType != "combinational" &&
            libArc.timingType != "rising_edge" &&
            libArc.timingType != "falling_edge") {
                std::cout << "unusual type: " << libArc.timingType << "\n";
                return true;
        }  // end if

        return false;
}  // end if

// -----------------------------------------------------------------------------

void Scenario::setTimingLibraryArcDelayModel(TimingLibraryArc &larc,
                                             const TimingMode mode,
                                             const TimingTransition transition,
                                             const ISPD13::LibParserLUT &lut) {
        larc.luts[mode].delay[transition] = lut;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setTimingLibraryArcSlewModel(TimingLibraryArc &larc,
                                            const TimingMode mode,
                                            const TimingTransition transition,
                                            const ISPD13::LibParserLUT &lut) {
        larc.luts[mode].oslew[transition] = lut;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setTimingLibraryPinCapacitance(TimingLibraryPin &lpin,
                                              const Number cap) {
        lpin.capacitance = cap;
}  // end method

//------------------------------------------------------------------------------

void Scenario::setTimingLibraryPinMaxCapacitance(TimingLibraryPin &lpin,
                                                 const Number maxCap) {
        lpin.maxCapacitance = maxCap;
}  // end method

//------------------------------------------------------------------------------

void Scenario::setTimingLibraryPinMaxTransition(TimingLibraryPin &lpin,
                                                const Number maxTransition) {
        lpin.maxTransition = maxTransition;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setTimingLibraryPinHoldConstraint(
    TimingLibraryPin &constrained, TimingLibraryPin &related,
    const TimingTransition transition, const ISPD13::LibParserLUT &lut) {
        constrained.control = related.index;
        constrained.hold[transition] = lut;
        related.control = constrained.index;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setTimingLibraryPinSetupConstraint(
    TimingLibraryPin &constrained, TimingLibraryPin &related,
    const TimingTransition transition, const ISPD13::LibParserLUT &lut) {
        constrained.control = related.index;
        constrained.setup[transition] = lut;
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Constraints (SDC)
////////////////////////////////////////////////////////////////////////////////

void Scenario::init_Constraints(const ISPD13::SDCInfo &sdc) {
        clsClockPeriod = (Number)sdc.clk_period;

        clsClockNet = clsDesign.findNetByName(sdc.clk_port);
        if (!clsClockNet) {
                throw Rsyn::Exception("Clock net " + sdc.clk_port +
                                      " not found.");
        }  // end if

        clsClockPort = clsDesign.findPortByName(sdc.clk_port);
        if (!clsClockPort) {
                throw Rsyn::Exception("Clock port " + sdc.clk_port +
                                      " not found.");
        }  // end if

        for (const ISPD13::InputDelay &data : sdc.input_delays) {
                Rsyn::Port port = clsDesign.findPortByName(data.port_name);
                if (port) {
                        setInputDelay(port, (Number)data.delay);
                } else {
                        std::cout << "[WARNING] Port " << data.port_name
                                  << " referenced in SDC file not found [Input "
                                     "Delay].\n";
                }  // end else
        }          // end for

        for (const ISPD13::OutputDelay &data : sdc.output_delays) {
                Rsyn::Port port = clsDesign.findPortByName(data.port_name);
                if (port) {
                        setOutputDelay(port, (Number)data.delay);
                } else {
                        std::cout << "[WARNING] Port " << data.port_name
                                  << " referenced in SDC file not found "
                                     "[Output Delay].\n";
                }  // end else
        }          // end for

        for (const ISPD13::OutputLoad &data : sdc.output_loads) {
                Rsyn::Port port = clsDesign.findPortByName(data.port_name);
                if (port) {
                        setOutputLoad(port, (Number)data.load);
                } else {
                        std::cout << "[WARNING] Port " << data.port_name
                                  << " referenced in SDC file not found "
                                     "[Output Load].\n";
                }  // end else
        }          // end for

        for (const ISPD13::InputDriver &data : sdc.input_drivers) {
                Rsyn::Port port = clsDesign.findPortByName(data.port_name);

                if (port) {
                        Rsyn::LibraryCell driver =
                            clsDesign.findLibraryCellByName(data.driver);

                        InputDriver inputDriver;
                        inputDriver.driver = driver;
                        inputDriver.inputSlew.set((Number)data.rise,
                                                  (Number)data.fall);
                        inputDriver.port = port;

                        // [HARDCODED] assuming only one timing arc
                        inputDriver.libraryArc = nullptr;
                        for (Rsyn::LibraryArc larc : driver.allLibraryArcs()) {
                                inputDriver.libraryArc = larc;
                                break;
                        }  // end for

                        if (!inputDriver.libraryArc) {
                                std::cout << "[BUG] No timing arc found for "
                                             "the input driver.\n";
                        }  // end if

                        setInputDriver(port, inputDriver);
                } else {
                        std::cout << "[WARNING] Port " << data.port_name
                                  << " referenced in SDC file not found [Input "
                                     "Driver].\n";
                }  // end else
        }          // end for

}  // end method

// -----------------------------------------------------------------------------

void Scenario::setInputDelay(Rsyn::Instance port, const Number value) {
        clsInputDelays[port] = value;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setInputDriver(Rsyn::Instance port, InputDriver driver) {
        clsInputDrivers[port] = driver;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setInputTransition(Rsyn::Instance port,
                                  const EdgeArray<Number> value) {
        clsInputTransitions[port] = value;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setOutputRequiredTime(Rsyn::Instance port,
                                     const TimingMode &mode,
                                     const EdgeArray<Number> value) {
        clsOutputRequiredTimes[mode][port] = value;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setOutputLoad(Rsyn::Instance port, const Number value) {
        clsOutputLoads[port] = value;
}  // end method

// -----------------------------------------------------------------------------

void Scenario::setOutputDelay(Rsyn::Instance port, const Number value) {
        clsOutputDelays[port] = value;
}  // end method

// -----------------------------------------------------------------------------

Number Scenario::getInputDelay(Rsyn::Instance port,
                               const Number defaultValue) const {
        auto it = clsInputDelays.find(port);
        return it == clsInputDelays.end() ? defaultValue : it->second;
}  // end method

// -----------------------------------------------------------------------------

EdgeArray<Number> Scenario::getInputTransition(
    Rsyn::Instance port, const EdgeArray<Number> defaultValue) const {
        auto it = clsInputTransitions.find(port);
        return it == clsInputTransitions.end() ? defaultValue : it->second;
}  // end method

// -----------------------------------------------------------------------------

EdgeArray<Number> Scenario::getOutputRequiredTime(
    Rsyn::Instance port, const TimingMode &mode,
    const EdgeArray<Number> defaultValue) const {
        auto it = clsOutputRequiredTimes[mode].find(port);
        return it == clsOutputRequiredTimes[mode].end() ? defaultValue
                                                        : it->second;
}  // end method

// -----------------------------------------------------------------------------

Number Scenario::getOutputLoad(Rsyn::Instance port,
                               const Number defaultValue) const {
        auto it = clsOutputLoads.find(port);
        return it == clsOutputLoads.end() ? defaultValue : it->second;
}  // end method

// -----------------------------------------------------------------------------

const Scenario::InputDriver *Scenario::getInputDriver(
    Rsyn::Instance cell) const {
        auto it = clsInputDrivers.find(cell);
        return (it == clsInputDrivers.end()) ? nullptr : &it->second;
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Notifications
////////////////////////////////////////////////////////////////////////////////

void Scenario::onPostNetCreate(Rsyn::Net net) {
        Rsyn::NetTag tag = clsDesign.getTag(net);
        tag.setType(Rsyn::NET_TYPE_TAG_NONE);
}  // end method

// -----------------------------------------------------------------------------

void Scenario::onPostPinConnect(Rsyn::Pin pin) {
        // NOTE: Untested...

        Rsyn::Net net = pin.getNet();
        Rsyn::Instance instance = pin.getInstance();

        if (instance.isSequential() && pin.getDirection() == Rsyn::OUT) {
                Rsyn::NetTag tag = clsDesign.getTag(net);
                tag.setType(Rsyn::NET_TYPE_TAG_DATA);
        } else {
                propagateNetTypeTag(net, net.getNetTypeTag());
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void Scenario::onPrePinDisconnect(Rsyn::Pin pin) {
        Rsyn::Net net = pin.getNet();
        Rsyn::NetTag tag = clsDesign.getTag(net);
        if (net.getNumPins() == 1) {
                // This net will become empty.
                tag.setType(Rsyn::NET_TYPE_TAG_NONE);
        } else {
                // Okay, if a clock subtree gets disconnected from the clock
                // tree, let's
                // keep the nets marked as belonging to a clock network.
        }  // end else
}  // end method

////////////////////////////////////////////////////////////////////////////////
// Tags
////////////////////////////////////////////////////////////////////////////////

void Scenario::init_NetTypeTags() {
        Rsyn::Module topModule = clsDesign.getTopModule();

        // First set all nets as data type.
        for (Rsyn::Net net : topModule.allNets()) {
                Rsyn::NetTag tag = clsDesign.getTag(net);
                tag.setType(Rsyn::NET_TYPE_TAG_DATA);
        }  // end method

        // Propagate clock tag thru the clock network.
        propagateNetTypeTag(getClockNet(), Rsyn::NET_TYPE_TAG_CLOCK);
}  // end method

// -----------------------------------------------------------------------------

void Scenario::init_MissingLibraryCellTags() {
        // Some library cells may no appear in the liberty, so initialize them
        // here
        // to avoid exceptions.
        for (Rsyn::LibraryCell lcell : clsLibrary.allLibraryCells()) {
                Rsyn::LibraryCellTag tag = clsDesign.getTag(lcell);

                if (tag.getBufferTypeTag() == BUFFER_TYPE_TAG_NOT_SPECIFIED)
                        tag.setBufferType(BUFFER_TYPE_TAG_NONE);

                if (tag.getLogicType() == LOGIC_TYPE_TAG_NOT_SPECIFIED)
                        tag.setLogicType(LOGIC_TYPE_TAG_NONE);

                if (tag.getTieType() == TIE_TYPE_TAG_NOT_SPECIFIED)
                        tag.setTieType(TIE_TYPE_TAG_NONE);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Scenario::propagateNetTypeTag(Rsyn::Net seed, Rsyn::NetTypeTag tag) {
        if (!seed) return;

        Rsyn::NetTag seedNetTag = clsDesign.getTag(seed);
        if (seedNetTag.getType() == tag) return;

        // Propagate the tag till we find registers or nets that already have
        // the
        // tag being propagated.

        std::queue<Rsyn::Net> open;
        open.push(seed);

        while (!open.empty()) {
                Rsyn::Net currentNet = open.front();
                open.pop();

                Rsyn::NetTag currenNetTag = clsDesign.getTag(currentNet);
                currenNetTag.setType(tag);

                for (Rsyn::Pin sink : currentNet.allPins(Rsyn::SINK)) {
                        Rsyn::Instance instance = sink.getInstance();
                        if (!instance.isSequential()) {
                                for (Rsyn::Pin pin :
                                     instance.allPins(Rsyn::OUT)) {
                                        Rsyn::Net nextNet = pin.getNet();
                                        if (nextNet) {
                                                Rsyn::NetTag nextNetTag =
                                                    clsDesign.getTag(nextNet);
                                                if (nextNetTag.getType() !=
                                                    tag) {
                                                        open.push(pin.getNet());
                                                }  // end if
                                        }          // end if
                                }                  // end for
                        }                          // end if
                }                                  // end for
        }                                          // end while
}  // end method

}  // end namespace