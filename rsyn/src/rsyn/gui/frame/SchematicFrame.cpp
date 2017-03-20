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
 
#include "SchematicFrame.h"

#include "rsyn/util/Colorize.h"
#include "rsyn/util/Environment.h"
#include "rsyn/io/legacy/ispd13/global.h"
#include "base/MainFrameBase.h"
#include "rsyn/3rdparty/json/json.hpp"

#include <wx/window.h>
#include <wx/filename.h>

#include <limits>
using std::numeric_limits;
#include <utility>
#include <thread>
using std::pair;
using std::make_pair;


// -----------------------------------------------------------------------------

SchematicFrame::SchematicFrame() : SchematicFrameBase((wxFrame *) nullptr) {
	clsCanvasGLPtr = new SchematicCanvasGL(clsPanelMain);
	clsCanvasGLPtr->Refresh();
	
	clsSizerMesh->Add(clsCanvasGLPtr, 1, wxEXPAND, 0);
	clsSizerMesh->Layout();

	// Define initial selection.s
	clsNotebook->SetSelection(0);
	clsChoiceBookColoring->SetSelection(0);
} // end constructor

// -----------------------------------------------------------------------------

SchematicFrame::~SchematicFrame() {
	delete clsCanvasGLPtr;
} // end destructor 

// -----------------------------------------------------------------------------

void SchematicFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
} // end event

// -----------------------------------------------------------------------------

void SchematicFrame::OnResetCamera(wxCommandEvent &WXUNUSED(event)) {
	clsCanvasGLPtr->resetCamera();
} // end method

// -----------------------------------------------------------------------------

void SchematicFrame::OnZoomIn(wxCommandEvent &WXUNUSED(event)) {
	clsCanvasGLPtr->zoom(0.5);
} // end method

// -----------------------------------------------------------------------------

void SchematicFrame::OnZoomOut(wxCommandEvent &WXUNUSED(event)) {
	clsCanvasGLPtr->zoom(2);
} // end method

// -----------------------------------------------------------------------------

void SchematicFrame::UpdateStats(const bool redraw) {
	// Redraw
	if (redraw)
		clsCanvasGLPtr->Refresh();	
} // end method