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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   DefaultTimingModelWithIdealClock.h
 * Author: mpfogaca
 *
 * Created on 7 de Outubro de 2016, 16:54
 */

#ifndef DEFAULTTIMINGMODELWITHIDEALCLOCK_H
#define DEFAULTTIMINGMODELWITHIDEALCLOCK_H

#include "TimingModel.h"
#include "DefaultTimingModel.h"
#include "phy/PhysicalDesign.h"

namespace ICCAD15 {

class DefaultTimingModelWithIdealClock : public Rsyn::TimingModel,
                                         public Rsyn::Service {
       private:
        Rsyn::DefaultTimingModel* clsDefaultTimingModel;
        Rsyn::Session clsSession;
        Rsyn::PhysicalDesign clsPhysicalDesign;

       public:
        void start(const Rsyn::Json& params) {
                Rsyn::Session session;

                clsPhysicalDesign = session.getPhysicalDesign();
                session.startService("rsyn.defaultTimingModel", {});
                Rsyn::DefaultTimingModel* timingModel =
                    session.getService("rsyn.defaultTimingModel");
                clsDefaultTimingModel = timingModel;
                clsSession = session;
        }

        void stop() {}

        void beforeTimingUpdate() {
                clsDefaultTimingModel->beforeTimingUpdate();
        }  // end method

        void calculateInputDriverTiming(
            Rsyn::Instance port, const Rsyn::TimingMode mode,
            const Rsyn::EdgeArray<Number>& load,
            Rsyn::EdgeArray<Number>& inputDelay,
            Rsyn::EdgeArray<Number>& inputSlew) override {
                clsDefaultTimingModel->calculateInputDriverTiming(
                    port, mode, load, inputDelay, inputSlew);

        }  // end method

        void calculateLibraryArcTiming(const Rsyn::LibraryArc libraryArc,
                                       const Rsyn::TimingMode mode,
                                       const Rsyn::TimingTransition oedge,
                                       const Number islew, const Number load,
                                       Number& delay, Number& slew) override {
                // HARD-CODED...
                if (libraryArc.getLibraryCell().getName().find("CLK") !=
                    std::string::npos) {
                        delay = 0;
                        slew = 0;
                        return;
                }  // end if
                clsDefaultTimingModel->calculateLibraryArcTiming(
                    libraryArc, mode, oedge, islew, load, delay, slew);

                if (delay < 0) delay = 0;
                if (slew < 0) slew = 0;

        }  // end method

        void calculateLoadCapacitance(const Rsyn::Pin pin,
                                      const Rsyn::TimingMode mode,
                                      Rsyn::EdgeArray<Number>& load) override {
                clsDefaultTimingModel->calculateLoadCapacitance(pin, mode,
                                                                load);
        }  // end method

        void calculateNetArcTiming(const Rsyn::Pin driver, const Rsyn::Pin sink,
                                   const Rsyn::TimingMode mode,
                                   const Rsyn::EdgeArray<Number>& slewAtDriver,
                                   Rsyn::EdgeArray<Number>& delay,
                                   Rsyn::EdgeArray<Number>& slew) override {
                if (driver.getInstance().isClockBuffer()) {
                        delay.setBoth(0.0);
                        slew.setBoth(0.0);
                        return;
                }

                clsDefaultTimingModel->calculateNetArcTiming(
                    driver, sink, mode, slewAtDriver, delay, slew);
        }  // end method

        Rsyn::EdgeArray<Number> getHoldTime(Rsyn::Pin data) const override {
                return clsDefaultTimingModel->getHoldTime(data);
        }  // end method

        Rsyn::TimingSense getLibraryArcSense(
            const Rsyn::LibraryArc libraryArc) override {
                return clsDefaultTimingModel->getLibraryArcSense(libraryArc);
        }  // end method

        Number getLibraryPinInputCapacitance(
            Rsyn::LibraryPin lpin) const override {
                return clsDefaultTimingModel->getLibraryPinInputCapacitance(
                    lpin);
        }  // end method

        Number getPinInputCapacitance(Rsyn::Pin pin) const override {
                return clsDefaultTimingModel->getPinInputCapacitance(pin);
        }  // end method

        Rsyn::EdgeArray<Number> getSetupTime(Rsyn::Pin data) const override {
                return clsDefaultTimingModel->getSetupTime(data);
        }  // end method

        void prepareNet(const Rsyn::Net net, const Rsyn::TimingMode mode,
                        const Rsyn::EdgeArray<Number>& slew) override {
                return clsDefaultTimingModel->prepareNet(net, mode, slew);
        }  // end method

        ////////////////////////////////////////////////////////////////////////////
        // Sandbox
        ////////////////////////////////////////////////////////////////////////////

        virtual void calculateLoadCapacitance(const Rsyn::SandboxPin pin,
                                              const Rsyn::TimingMode mode,
                                              Rsyn::EdgeArray<Number>& load) {
                assert(false);
        }  // end method

        virtual void prepareNet(const Rsyn::SandboxNet net,
                                const Rsyn::TimingMode mode,
                                const Rsyn::EdgeArray<Number>& slew) {
                assert(false);
        }  // end method

        virtual void calculateNetArcTiming(
            const Rsyn::SandboxPin driver, const Rsyn::SandboxPin sink,
            const Rsyn::TimingMode mode,
            const Rsyn::EdgeArray<Number>& slewAtDriver,
            Rsyn::EdgeArray<Number>& delay, Rsyn::EdgeArray<Number>& slew) {
                assert(false);
        }  // end method

        virtual Rsyn::EdgeArray<Number> getSetupTime(
            Rsyn::SandboxPin data) const {
                assert(false);
                return Rsyn::EdgeArray<Number>(0, 0);
        }  // end method

        virtual Rsyn::EdgeArray<Number> getHoldTime(
            Rsyn::SandboxPin data) const {
                assert(false);
                return Rsyn::EdgeArray<Number>(0, 0);
        }  // end method

};  // end class

}  // end namespace

#endif /* DEFAULTTIMINGMODELWITHIDEALCLOCK_H */
