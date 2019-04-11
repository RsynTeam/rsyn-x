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

#ifndef RSYN_SHELL_H
#define RSYN_SHELL_H

#include "core/Rsyn.h"
#include "session/Session.h"

namespace Rsyn {

class Shell {
       private:
        Rsyn::Session clsSession;

       public:
        void runScript(const std::string &script,
                       const bool interactive = false);
        void runCommand(const std::string cmd, const bool interactive = false);
};  // end class

}  // end namespace

#endif
