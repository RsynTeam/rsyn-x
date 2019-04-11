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
 * File:   TDQuadraticFlow.cpp
 * Author: mpfogaca
 *
 * Created on 13 de Setembro de 2016, 09:25
 */

#include "TDQuadraticFlow.h"
#include "tool/timing/Timer.h"
#include "iccad15/Infrastructure.h"

namespace ICCAD15 {

bool TDQuadraticFlow::run(const Rsyn::Json& params) {
        Rsyn::Session session;

        clsTimer = session.getService("rsyn.timer");
        clsInfra = session.getService("ufrgs.ispd16.infra");
        clsJezz = clsInfra->getJezz();

        if (params.count("overlapFlow")) {
                if (params["overlapFlow"] == "Jezz")
                        overlapFlow();
                else
                        overlapFlowMIP();
                return true;
        }  // end if

        defaultFlow();

        return true;
}  // end method

//------------------------------------------------------------------------------

void TDQuadraticFlow::defaultFlow() {
        clsTimer->updateTimingFull();
        clsJezz->jezz_Legalize();
        clsJezz->jezz_storeSolution("initial");

        clsInfra->updateTDPSolution(true);

        vector<Rsyn::Pin> ep;
        clsTimer->queryTopCriticalEndpoints(Rsyn::LATE, 3, ep);
        const double initAlpha =
            clsTimer->getSmoothedCriticality(ep.back(), Rsyn::LATE);

        while (true) {
                clsSession.runProcess(
                    "ufrgs.incrementalTimingDrivenQP",
                    {{"alpha", initAlpha}, {"beta", 1.0}, {"flow", "default"}});

                clsInfra->reportSummary("QP-Step1");

                if (!clsInfra->updateTDPSolution(false, 1e-5)) break;

                clsSession.runProcess("ufrgs.balancing",
                                      {{"type", "cell-driver-sink"}});
        };  // end while
        clsInfra->statePush("qp-net-weighting");

        while (true) {
                clsSession.runProcess(
                    "ufrgs.incrementalTimingDrivenQP",
                    {{"alpha", 0.01}, {"beta", 1.0}, {"flow", "netWeighting"}});

                clsInfra->reportSummary("QP-Step2");

                if (!clsInfra->updateTDPSolution(false, 1e-5)) break;

                clsSession.runProcess("ufrgs.balancing",
                                      {{"type", "cell-driver-sink"}});
        };  // end while
        clsInfra->statePush("qp-net-weighting");

        //	clsSession.runProcess("ufrgs.ISPD16Flow");
}  // end method

//------------------------------------------------------------------------------

void TDQuadraticFlow::overlapFlow() {
        clsTimer->updateTimingFull();
        clsJezz->jezz_Legalize();
        clsJezz->jezz_storeSolution("initial");

        clsInfra->updateTDPSolution(true);

        while (true) {
                clsSession.runProcess(
                    "ufrgs.incrementalTimingDrivenQP",
                    {{"alpha", 0.9}, {"beta", 1.0}, {"flow", "netWeighting"}});

                clsInfra->reportSummary("QP-NetWeighting-Step1");

                if (!clsInfra->updateTDPSolution(false, 1e-5)) break;
        };  // end while
}  // end method

//------------------------------------------------------------------------------

void TDQuadraticFlow::overlapFlowMIP() {
        clsTimer->updateTimingFull();
        clsJezz->jezz_Legalize();
        clsJezz->jezz_storeSolution("initial");

        clsInfra->updateTDPSolution(true);

        while (true) {
                clsSession.runProcess("ufrgs.incrementalTimingDrivenQP",
                                      {{"alpha", 0.9},
                                       {"beta", 1.0},
                                       {"flow", "elmore"},
                                       {"legMode", "NONE"}});

                clsSession.runProcess("ufrgs.overlapRemover", {});

                clsTimer->updateTimingIncremental();

                clsInfra->reportSummary("QP-NetWeighting-Step1");

                if (!clsInfra->updateTDPSolution(false, 1e-5)) break;
        };  // end while
}  // end method
}
