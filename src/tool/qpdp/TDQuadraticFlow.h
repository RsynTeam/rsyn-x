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
 * File:   TDQuadraticFlow.h
 * Author: mpfogaca
 *
 * Created on 13 de Setembro de 2016, 09:25
 */

#ifndef TD_QUADRATIC_FLOW_H
#define TD_QUADRATIC_FLOW_H

#include "core/Rsyn.h"
#include "session/Session.h"
#include "tool/jezz/Jezz.h"

namespace Rsyn {
class Timer;
}

namespace ICCAD15 {

class Infrastructure;

class TDQuadraticFlow : public Rsyn::Process {
        Rsyn::Session clsSession;
        Rsyn::Timer* clsTimer;
        Infrastructure* clsInfra;
        Jezz* clsJezz;

       public:
        TDQuadraticFlow(){};

        bool run(const Rsyn::Json& params);
        void defaultFlow();
        void overlapFlow();
        void overlapFlowMIP();
};

}  // end class

#endif /* TDQUADRATICFLOW_H */
