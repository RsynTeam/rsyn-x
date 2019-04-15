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

#include "session/Session.h"

// Registration
namespace Rsyn {
void Session::registerMessages() {
        registerMessage(
            "SESSION-001", WARNING, "Message registration failed.",
            "Cannot register message <message> after initialization.");
        registerMessage("TIMER-001", WARNING, "Unusual timing arc sense.",
                        "Timing arc <arc> has the unusual (for academic tools) "
                        "sense <sense>.");
        registerMessage("TIMER-002", WARNING, "Unusual timing arc type.",
                        "Timing arc <arc> has the unusual (for academic tools) "
                        "type <type>.");

        registerMessage("GRAPHICS-001", INFO, "Unable to initialize GLEW.");
        registerMessage("GRAPHICS-002", INFO,
                        "Rendering to texture is not supported.");
}  // end method
}  // end namespace
