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

#ifndef FASTPLACE_EXT_OPTO_H
#define FASTPLACE_EXT_OPTO_H

#include "session/Session.h"

namespace Rsyn {
class Timer;
class Writer;
}  // end namespace

namespace ICCAD15 {

class FastPlace : public Rsyn::Process {
       private:
        Rsyn::Session session;
        Rsyn::Design design;
        Rsyn::Module module;

        Rsyn::Timer *timer;
        Rsyn::Writer *writer;

        std::string createTemporaryDirectory();
        bool exec(const std::string &cmd, std::ostream &out = std::cout);

       public:
        virtual bool run(const Rsyn::Json &params);

};  // end class
}
#endif