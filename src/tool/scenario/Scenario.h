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

#ifndef RSYN_SCENARIO
#define RSYN_SCENARIO

#include "core/Rsyn.h"
#include "session/Service.h"
#include "tool/timing/TimingTypes.h"
#include "tool/timing/EdgeArray.h"
#include "ispd13/global.h"

namespace Rsyn {

class Scenario : public Service, public DesignObserver {
       public:
        Scenario();

        virtual void start(const Rsyn::Json &params);

        virtual void stop() override {}  // end method

       public:
        void init(Rsyn::Design design, Rsyn::Library library,
                  const ISPD13::LIBInfo &libInfosEarly,
                  const ISPD13::LIBInfo &libInfosLate,
                  const ISPD13::SDCInfo &sdc);

        ////////////////////////////////////////////////////////////////////////////////
        // Liberty
        ////////////////////////////////////////////////////////////////////////////////

       public:
        class TimingLibraryArc {
                friend class Scenario;

               private:
                TimingSense sense;

                struct TimingInfo {
                        ISPD13::LibParserLUT delay[NUM_EDGE_TYPES];
                        ISPD13::LibParserLUT oslew[NUM_EDGE_TYPES];
                };  // end struct

                TimingInfo luts[NUM_TIMING_MODES];

               public:
                TimingLibraryArc() : sense(TIMING_SENSE_INVALID) {}

                TimingSense getSense() const { return sense; }
                const ISPD13::LibParserLUT &getDelayLut(
                    const TimingMode &mode, const EdgeType &edge) const {
                        return luts[mode].delay[edge];
                }
                const ISPD13::LibParserLUT &getSlewLut(
                    const TimingMode &mode, const EdgeType &edge) const {
                        return luts[mode].oslew[edge];
                }
        };  // end struct

        class TimingLibraryPin {
                friend class Scenario;

               private:
                int index;

                // Input capacitance associates to this pin.
                Number capacitance;

                // Output max capacitance
                Number maxCapacitance;

                // Input max transition
                Number maxTransition;

                // Indicates if this pin is the clock pin of a register.
                bool clocked;

                // For non-sequential pins, always = -1
                // For sequential pins,
                //     if clocked = true; indicates the index of the data pin;
                //     if clocked = false; indicates the index of the clock pin;
                int control;

                // Look-up tables.
                ISPD13::LibParserLUT setup[NUM_EDGE_TYPES];
                ISPD13::LibParserLUT hold[NUM_EDGE_TYPES];

               public:
                TimingLibraryPin()
                    : index(-1),
                      control(-1),
                      clocked(false),
                      capacitance(0),
                      maxCapacitance(0),
                      maxTransition(0) {}

                const ISPD13::LibParserLUT &getSetupLut(
                    const EdgeType &edge) const {
                        return setup[edge];
                }
                const ISPD13::LibParserLUT &getHoldLut(
                    const EdgeType &edge) const {
                        return hold[edge];
                }

                Number getCapacitance() const { return capacitance; }
                Number getMaxCapacitance() const { return maxCapacitance; }
                Number getMaxTransition() const { return maxTransition; }

                // Pins related to sequential cells.
                bool isClockPin() const { return clocked; }
                bool isDataPin() const { return control != -1 && !clocked; }

                // Assume this is a clock pin and then returns the data pin
                // associated to
                // this clock pin.
                int getDataPinIndex() const { return control; }

                // Assume this is a data pin and then returns the clock pin
                // associated to
                // this data pin.
                int getClockPinIndex() const { return control; }
        };  // end class

        class TimingLibraryCell {
                friend class Scenario;

               private:
                Number leakagePower;

               public:
                TimingLibraryCell() : leakagePower(0.0) {}

                Number getLeakagePower() const { return leakagePower; }
        };  // end class

       private:
        void init_Mode(const TimingMode mode, const ISPD13::LIBInfo &lib);

        Rsyn::Design clsDesign;
        Rsyn::Library clsLibrary;

        // Liberty
        Rsyn::Attribute<Rsyn::LibraryCell, TimingLibraryCell>
            clsTimingLibraryCells;
        Rsyn::Attribute<Rsyn::LibraryPin, TimingLibraryPin>
            clsTimingLibraryPins;
        Rsyn::Attribute<Rsyn::LibraryArc, TimingLibraryArc>
            clsTimingLibraryArcs;

       public:
        TimingSense getTimingSenseFromString(const std::string &sense) {
                if (sense == "non_unate") return NON_UNATE;
                if (sense == "negative_unate") return NEGATIVE_UNATE;
                if (sense == "positive_unate") return POSITIVE_UNATE;
                return TIMING_SENSE_INVALID;
        }  // end function

        bool isUnusualTimingArc(
            const ISPD13::LibParserTimingInfo &libArc) const;

        void initializeTimingLibraryArc(Rsyn::LibraryArc larc,
                                        const TimingSense sense);

        void setTimingLibraryArcDelayModel(TimingLibraryArc &larc,
                                           const TimingMode mode,
                                           const TimingTransition transition,
                                           const ISPD13::LibParserLUT &lut);

        void setTimingLibraryArcSlewModel(TimingLibraryArc &larc,
                                          const TimingMode mode,
                                          const TimingTransition transition,
                                          const ISPD13::LibParserLUT &lut);

        void setTimingLibraryPinCapacitance(TimingLibraryPin &lpin,
                                            const Number cap);

        void setTimingLibraryPinMaxCapacitance(TimingLibraryPin &lpin,
                                               const Number maxCap);

        void setTimingLibraryPinMaxTransition(TimingLibraryPin &lpin,
                                              const Number maxTransition);

        void setTimingLibraryPinHoldConstraint(
            TimingLibraryPin &constrained, TimingLibraryPin &related,
            const TimingTransition transition, const ISPD13::LibParserLUT &lut);

        void setTimingLibraryPinSetupConstraint(
            TimingLibraryPin &constrained, TimingLibraryPin &related,
            const TimingTransition transition, const ISPD13::LibParserLUT &lut);

        inline TimingLibraryCell &getTimingLibraryCell(
            Rsyn::LibraryCell rsynLibraryCell) {
                return clsTimingLibraryCells[rsynLibraryCell];
        }
        inline const TimingLibraryCell &getTimingLibraryCell(
            Rsyn::LibraryCell rsynLibraryCell) const {
                return clsTimingLibraryCells[rsynLibraryCell];
        }

        inline TimingLibraryPin &getTimingLibraryPin(
            Rsyn::LibraryPin rsynLibraryPin) {
                return clsTimingLibraryPins[rsynLibraryPin];
        }
        inline const TimingLibraryPin &getTimingLibraryPin(
            Rsyn::LibraryPin rsynLibraryPin) const {
                return clsTimingLibraryPins[rsynLibraryPin];
        }

        inline TimingLibraryPin &getTimingLibraryPin(Rsyn::Pin rsynPin) {
                return getTimingLibraryPin(rsynPin.getLibraryPin());
        }
        inline const TimingLibraryPin &getTimingLibraryPin(
            Rsyn::Pin rsynPin) const {
                return getTimingLibraryPin(rsynPin.getLibraryPin());
        }

        inline TimingLibraryArc &getTimingLibraryArc(
            Rsyn::LibraryArc rsynLibraryArc) {
                return clsTimingLibraryArcs[rsynLibraryArc];
        }
        inline const TimingLibraryArc &getTimingLibraryArc(
            Rsyn::LibraryArc rsynLibraryArc) const {
                return clsTimingLibraryArcs[rsynLibraryArc];
        }

        inline TimingLibraryArc &getTimingLibraryArc(Rsyn::Arc rsynArc) {
                return getTimingLibraryArc(rsynArc.getLibraryArc());
        }
        inline const TimingLibraryArc &getTimingLibraryArc(
            Rsyn::Arc rsynArc) const {
                return getTimingLibraryArc(rsynArc.getLibraryArc());
        }

        Number getLibraryCellLeakagePower(Rsyn::LibraryCell lcell) {
                TimingLibraryCell &timingLibraryCell =
                    getTimingLibraryCell(lcell);
                return timingLibraryCell.getLeakagePower();
        }  // end method

        ////////////////////////////////////////////////////////////////////////////////
        // Constraints (SDC)
        ////////////////////////////////////////////////////////////////////////////////
       public:
        struct InputDriver {
                Rsyn::Instance port;
                Rsyn::LibraryCell driver;
                Rsyn::LibraryArc libraryArc;
                EdgeArray<Number> inputSlew;
        };  // end struct

       private:
        Number clsClockPeriod;
        Rsyn::Net clsClockNet;
        Rsyn::Instance clsClockPort;

        std::map<Rsyn::Instance, Number> clsInputDelays;
        std::map<Rsyn::Instance, Number> clsOutputDelays;
        std::map<Rsyn::Instance, EdgeArray<Number>> clsInputTransitions;
        std::map<Rsyn::Instance, EdgeArray<Number>>
            clsOutputRequiredTimes[NUM_TIMING_MODES];
        std::map<Rsyn::Instance, Number> clsOutputLoads;
        std::map<Rsyn::Instance, InputDriver> clsInputDrivers;

        void init_Constraints(const ISPD13::SDCInfo &sdc);
        void init_NetTypeTags();
        void init_MissingLibraryCellTags();

        void propagateNetTypeTag(Rsyn::Net seed, Rsyn::NetTypeTag tag);

        void setClockPeriod(const Number period) { clsClockPeriod = period; }

        void setInputDelay(Rsyn::Instance port, const Number value);
        void setInputDriver(Rsyn::Instance port, InputDriver driver);
        void setInputTransition(Rsyn::Instance port,
                                const EdgeArray<Number> value);
        void setOutputRequiredTime(Rsyn::Instance port, const TimingMode &mode,
                                   const EdgeArray<Number> value);
        void setOutputLoad(Rsyn::Instance port, const Number value);
        void setOutputDelay(Rsyn::Instance port, const Number value);

       public:
        Number getInputDelay(Rsyn::Instance port,
                             const Number defaultValue) const;
        EdgeArray<Number> getInputTransition(
            Rsyn::Instance port, const EdgeArray<Number> defaultValue) const;
        EdgeArray<Number> getOutputRequiredTime(
            Rsyn::Instance port, const TimingMode &mode,
            const EdgeArray<Number> defaultValue) const;
        Number getOutputLoad(Rsyn::Instance port,
                             const Number defaultValue) const;
        const InputDriver *getInputDriver(Rsyn::Instance cell) const;

        Number getClockPeriod() const { return clsClockPeriod; }  // end method

        Rsyn::Net getClockNet() const { return clsClockNet; }  // end method

        virtual void onPostNetCreate(Rsyn::Net net) override;

        virtual void onPostPinConnect(Rsyn::Pin pin) override;

        virtual void onPrePinDisconnect(Rsyn::Pin pin) override;

};  // end class

}  // end namespace

#endif
