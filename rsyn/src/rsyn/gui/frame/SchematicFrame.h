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
 
#ifndef SCHEMATIC_FRAME_H
#define SCHEMATIC_FRAME_H

#include "rsyn/gui/frame/base/SchematicFrameBase.h"
#include "rsyn/gui/canvas/SchematicCanvasGL.h"
#include "rsyn/gui/Redirector.h"

#include "rsyn/io/parser/bookshelf/BookshelfParser.h"
#include "rsyn/engine/Engine.h"

#include <wx/config.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <vector>
using std::vector;
#include <map>
#include <string>

// -----------------------------------------------------------------------------

class SchematicFrame : public SchematicFrameBase {
protected:
	SchematicCanvasGL * clsCanvasGLPtr;
private:

	Rsyn::Engine clsEnginePtr;

	// Update stats.
	void UpdateStats(const bool redraw);
	
public:

	SchematicFrame();
	~SchematicFrame();

	// Event Handlers.
	virtual void OnQuit(wxCommandEvent &event);

	virtual void OnResetCamera(wxCommandEvent& event);
	virtual void OnZoomIn(wxCommandEvent& event);
	virtual void OnZoomOut(wxCommandEvent& event);

}; // end class

#endif
