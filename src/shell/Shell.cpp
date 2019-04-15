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
 
#include "Shell.h"

namespace Rsyn {

void Shell::runScript(const std::string &startupScript, const bool interactive) {
	if (startupScript.empty() && !interactive) {
		std::cout << "Exiting. Nothing to be done... \n";
		std::cout << " >> Try run with -gui, -interactive or -script <path_to_rsyn_script> flags. <<\n";
		return;
	} // end if

	if (!startupScript.empty()) {
		clsSession.evaluateFile(startupScript);
	} // end if

	if (interactive) {
		while (true) {
			std::string cmd;
			std::cout << "> ";
			std::getline(std::cin, cmd);
			clsSession.evaluateString(cmd);
		} // end while
	} // end while
} // end method

// ----------------------------------------------------------------------------

void Shell::runCommand(const std::string cmd, const bool interactive) {
	clsSession.evaluateString(cmd);
	
	if (interactive) {
		while (true) {
			std::string cmd;
			std::cout << "> ";
			std::getline(std::cin, cmd);
			clsSession.evaluateString(cmd);
		} // end while
	} // end while
} // end method

} // end namespace