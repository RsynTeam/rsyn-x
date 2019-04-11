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
 * File:   ICCAD17Reader.h
 * Author: jucemar
 *
 * Created on 08 de Abril de 2017, 15:40
 */

#ifndef RSYN_ICCAD17READER_H
#define RSYN_ICCAD17READER_H

#include "session/Session.h"

namespace Rsyn {

////////////////////////////////////////////////////////////////////////////////
// Reader to ICCAD 2017 benchmarks
////////////////////////////////////////////////////////////////////////////////

class ICCAD17Reader : public Reader {
       protected:
        Rsyn::Session session;
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;
        float clsMaxUtilization =
            1.0;  // Default value is 100%. The script provides max utilization
                  // in percentage. we convert to dimensionless value.
       public:
        ICCAD17Reader() = default;
        ICCAD17Reader(const ICCAD17Reader& orig) = delete;
        virtual ~ICCAD17Reader() = default;
        virtual bool load(const Rsyn::Json& params) override;

       protected:
        void parseConstraints(const std::string& filename);
};  // end class

}  // end namespace
#endif /* RSYN_ICCAD17READER_H */
