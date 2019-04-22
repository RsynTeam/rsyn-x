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
 * File:   ICCAD15Reader.h
 * Author: jucemar
 *
 * Created on 20 de Fevereiro de 2017, 18:59
 */

#ifndef ICCAD15_READER_EXTENDED_H
#define ICCAD15_READER_EXTENDED_H

#include "session/Session.h"

#include "core/Rsyn.h"
#include "ICCAD15Reader.h"
#include "tool/timing/Timer.h"
#include "tool/scenario/Scenario.h"
#include "tool/timing/DefaultTimingModel.h"
#include "tool/routing/RoutingEstimator.h"

namespace ICCAD15 {

////////////////////////////////////////////////////////////////////////////////
// Reader to ICCAD 2015 benchmarks
////////////////////////////////////////////////////////////////////////////////

class ICCAD15ReaderExtended : public Rsyn::ICCAD15Reader {
       protected:
       public:
        ICCAD15ReaderExtended() {}
        ICCAD15ReaderExtended(const ICCAD15ReaderExtended& orig) {}
        virtual ~ICCAD15ReaderExtended() {}
        virtual bool load(const Rsyn::Json& params) override;

       protected:
        void openBenchmarkFromICCAD15();
        void openBenchmarkFromICCAD15ForGlobalPlacementOnly();
};  // end class

}  // end namespace
#endif /* RSYN_ICCAD15READER_EXTENDED_H */
