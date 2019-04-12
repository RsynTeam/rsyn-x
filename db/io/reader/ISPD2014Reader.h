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

#ifndef RSYN_ISPD2014READER_H
#define RSYN_ISPD2014READER_H

#include "session/Session.h"
#include "core/Rsyn.h"

namespace Rsyn {

class ISPD2014Reader : public Reader {
       protected:
        Rsyn::Session session;
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;

       public:
        ISPD2014Reader() {}
        ISPD2014Reader(const ISPD2014Reader& orig) {}
        virtual ~ISPD2014Reader() {}
        virtual bool load(const Rsyn::Json& params) override;

};  // end class

}  // end namespace

#endif /* RSYN_ISPD2014READER_H */
