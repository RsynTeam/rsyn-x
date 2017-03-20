/* Copyright 2014-2017 Rsyn
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
 
#include "App.h"

#include <iostream>
#include <string>

#include "rsyn/gui/frame/MainFrame.h"
#include "rsyn/gui/frame/SchematicFrame.h"
#include "rsyn/util/Environment.h"

// -----------------------------------------------------------------------------

wxIMPLEMENT_APP_NO_MAIN(MyApp);

// -----------------------------------------------------------------------------

std::string MyApp::clsGui;
std::string MyApp::clsScript;

// -----------------------------------------------------------------------------

void MyApp::Init(const std::string &gui, const std::string &script) {
	clsGui = gui;
	clsScript = script;

    int argc = 1;
    const char *argv[1] = {"rsyn"};
	wxEntryStart(argc, const_cast<char **>(argv));
    wxTheApp->CallOnInit();
    wxTheApp->OnRun();
} // end method

// -----------------------------------------------------------------------------

bool MyApp::OnInit(){
	wxInitAllImageHandlers();

	wxFrame *frame = nullptr;
	if (clsGui.empty() || clsGui == "default" || clsGui == "iccad") {
		frame = new MainFrame();
	} else if (clsGui == "schematic") {
		frame = new SchematicFrame();
	}// end else

	if (frame) {
		frame->Show(TRUE);
		SetTopWindow(frame);
	} else {
		std::cout << "Invalid frame. Exiting...\n";
		std::exit(1);
	} // end method

    return true;
} // end method
