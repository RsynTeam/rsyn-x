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
 
#include "MainFrame.h"

#include "rsyn/util/Colorize.h"
#include "rsyn/util/Environment.h"
#include "rsyn/util/Json.h"

// Services
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/model/timing/Timer.h"
#include "rsyn/model/library/LibraryCharacterizer.h"
#include "rsyn/model/routing/RoutingEstimator.h"
#include "rsyn/io/Report.h"
#include "rsyn/io/Writer.h"
#include "rsyn/io/Graphics.h"
#include "rsyn/gui/canvas/SchematicCanvasGL.h"
#include "rsyn/util/Stipple.h"
#include "rsyn/core/infra/RawPointer.h"

#include <wx/window.h>
#include <wx/filename.h>

#include <limits>
using std::numeric_limits;
#include <utility>
#include <thread>
using std::pair;
using std::make_pair;

// -----------------------------------------------------------------------------

const std::string MainFrame::DEFAULT_STORED_SOLUTION_NAME = "_gui";
MainFrame::RegisteredOverlayMap *MainFrame::clsRegisteredOverlays = nullptr;

// -----------------------------------------------------------------------------

MainFrame::MainFrame() : MainFrameBase((wxFrame *) nullptr), clsConfig("UPlace") {
	clsSession.registerGraphicsCallback([&](const Rsyn::GraphicsEvent event){
		switch (event) {
			case Rsyn::GRAPHICS_EVENT_DESIGN_LOADED:
				processGraphicsEventDesignLoaded();
				break;
			case Rsyn::GRAPHICS_EVENT_REFRESH:
				processGraphicsEventRefresh();
				break;
			case Rsyn::GRAPHICS_EVENT_UPDATE_OVERLAY_LIST:
				processGraphicsEventUpdateOverlayList();
				break;
			default:
				std::cout << "WARNING: Graphics event handler not implemented.\n";
		} // end switch
	}); // end lambda

	clsPhysicalCanvasGLPtr = new PhysicalCanvasGL(clsPanelMain);
	clsSchematicCanvasGLPtr = nullptr;
	clsSchematicCanvas = nullptr;
	
	clsCanvasGLPtr = clsPhysicalCanvasGLPtr;
	clsCanvasGLPtr->Refresh();
	
	clsSaveSnapshot = new SaveSnapshot( this, clsPhysicalCanvasGLPtr );
	clsAboutDialog = new AboutDialog(this);
	
	clsSizerMesh->Add(clsCanvasGLPtr, 1, wxEXPAND, 0);
	clsSizerMesh->Layout();

	// Define initial choicebook pages.
	clsNotebook->ChangeSelection(0);
	clsChoicebookColoring->ChangeSelection(0);
	clsChoicebookView->ChangeSelection(0);
	clsChoicebookProperties->ChangeSelection(1);
	
	// Put the focus on the command text box.
	clsTxtCommand->SetFocus();
	
	// Events
	clsCanvasGLPtr->Connect(myEVT_CELL_SELECTED, wxCommandEventHandler(MainFrame::OnCellSelected), NULL, this);
	clsCanvasGLPtr->Connect(myEVT_SCHEMATIC_CELL_SELECTED, wxCommandEventHandler(MainFrame::OnSchematicCellSelected), NULL, this);
	clsCanvasGLPtr->Connect(myEVT_HOVER_OVER_OBJECT, wxCommandEventHandler(MainFrame::OnHoverOverObject), NULL, this);
	clsTxtSearch->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::OnSearch), NULL, this);
	clsCanvasGLPtr->Connect(myEVT_CELL_DRAGGED, wxCommandEventHandler(MainFrame::OnSelectedCellDrag), NULL, this);

	// Callbacks
	clsPhysicalCanvasGLPtr->setInstanceDoubleClickCallback([&](Rsyn::Instance instance) {
		updateInstanceProperties(instance);
	});

	clsPhysicalCanvasGLPtr->setNetDoubleClickCallback([&](Rsyn::Net net) {
		updateNetProperties(net);
	});

	clsPhysicalCanvasGLPtr->setPinDoubleClickCallback([&](Rsyn::Pin pin) {
		updatePinProperties(pin);
	});

	// Restore command history.
	ScriptParsing::CommandManager &commandManager =
			clsSession.getCommandManager();
	int counter = 0;
	while (true) {
		std::ostringstream oss;
		oss <<"CommandHistory/" << counter;
		
		if (!clsConfig.HasEntry(oss.str()))
			break;
		
		wxString commandHistory = clsConfig.Read(oss.str(), "");
		if (commandHistory != "") {
			commandManager.pushHistory(commandHistory.ToStdString());
		} // end if
		
		counter++;
	} // end while
} // end constructor

// -----------------------------------------------------------------------------

MainFrame::~MainFrame() {
	delete clsCanvasGLPtr;
	delete clsSaveSnapshot;
	delete clsAboutDialog;
	
	deleteAllOverlays();
} // end destructor

// -----------------------------------------------------------------------------

void MainFrame::startAllOverlays() {
	RegisteredOverlayMap &registeredOverlayMap = *clsRegisteredOverlays;
	for (auto overlaphDescriptor : registeredOverlayMap) {
		startOverlay(overlaphDescriptor.first, std::get<1>(overlaphDescriptor.second));
	} // end for
} // end method

// -----------------------------------------------------------------------------

bool MainFrame::startOverlay(const std::string &name, const bool visibility) {
	RegisteredOverlayMap &registeredOverlayMap = *clsRegisteredOverlays;

	CanvasOverlay * overlay = nullptr;

	auto it0 = registeredOverlayMap.find(name);
	if (it0 == registeredOverlayMap.end()) {
		std::cout << "WARNING: Overlay '" << name << "' was not "
				"registered.\n";
		return false;
	} else {
		// Check if it was already started.
		auto it1 = clsOverlays.find(name);
		if (it1 != clsOverlays.end()) {
			std::cout << "WARNING: Overlay '" << name << "' is already "
					"started.\n";
			return false;
		} else {
			overlay = std::get<0>(it0->second)();
		} // end else
	} // end else

	Rsyn::Json properties;
	const bool success = overlay->init(clsPhysicalCanvasGLPtr, properties);
	if (success) {
		clsOverlays[name] = overlay;
		clsPhysicalCanvasGLPtr->addOverlay(name, overlay, visibility);

		wxPGProperty* overlay = clsOverlayPropertyGrid->Append(
			new wxBoolProperty(name,
			wxPG_LABEL,
			visibility));

		for (const Rsyn::Json prop : properties) {
			if (!prop.count("name") || !prop.count("type")) {
				std::cout << "ERROR: Overlay properties must have 'name' and 'type' specified.\n";
				std::cout << prop << "\n";
				std::exit(1);
			} // end if

			wxPGProperty* parent = overlay;
			if (prop.count("parent")) {
				parent = overlay->GetPropertyByName(prop["parent"]);
				if (!parent) {
					std::cout << "ERROR: Parent property " << prop["parent"] <<
						" not found.\n";
					std::exit(1);
				} // end if
			} // end if

			const std::string name = prop.at("name");
			const std::string label = prop.value("label", name);
			const std::string type = prop.at("type");

			wxPGProperty* child = nullptr;
			if (type == "bool") {
				const bool defaultValue = prop.value("default", false);
				child = parent->AppendChild(new wxBoolProperty(label, name, defaultValue));
			} else if (type == "int") {
				const int defaultValue = prop.value("default", 0);
				child = parent->AppendChild(new wxIntProperty(label, name,	defaultValue));
			} else if (type == "color") {
				Rsyn::Json defaultValue = {{"r", 0}, {"g", 0}, {"b", 0}};

				if (prop.count("default"))
					defaultValue = prop["default"];

				wxColour color(
					defaultValue["r"],
					defaultValue["g"],
					defaultValue["b"]
				);

				const int stipple = prop.value("stipple", 0);
				wxBitmap bitmap =
					createBitmapFromMask(STIPPLE_MASKS[stipple], color, color);
				parent->SetValueImage(bitmap);

				child = parent->AppendChild(new wxColourProperty(label, name, color));
				// Awful... a way to store the stipple from a mask...
				propToStipple[child] = stipple;
			} else {
				std::cout << "ERROR: Property type '" << type <<
					" not supported.\n";
				std::exit(1);
			} // end if

			if (child) {
				child->SetExpanded(false);
			} // end if

		} // end for
	} else {
		if (visibility) {
			std::cout << "WARNING: Overlay '" + name + "' was not successfully initialized.\n";
		} // end if
	} // end else

	// Why do we need this? Add a comment...
	clsOverlayPropertyGrid->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);

	return success;
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnExecuteCommand(wxCommandEvent& event) {
	const std::string cmd = clsTxtCommand->GetValue().ToStdString();

	// Skip if the command is blank.
	if (cmd.find_first_not_of(' ') == std::string::npos)
		return;
	
	try {
		clsSession.evaluateString(cmd);
		UpdateStats(true); // FIXME: Don't need to redraw for some commands.

		//  Store command in the history.
		const ScriptParsing::CommandManager &commandManager =
			clsSession.getCommandManager();

		const int index = commandManager.currentHistoryCommandIndex();
		if (index >= 0) {
			const std::string &currentCommand = commandManager.currentHistoryCommand();
			const std::string &previousCommand = index > 0? 
				commandManager.getHistory(index - 1) : "";

			if (currentCommand != previousCommand) {
				std::ostringstream oss;
				oss << "CommandHistory/" << index;
				clsConfig.Write(wxString(oss.str()), wxString(currentCommand));
			} // end if
		} // end if
		
	} catch (const std::exception &e) {
		std::cout << "EXCEPTION: " << e.what() << "\n";
	} // end catch
	
	clsTxtCommand->Clear();
} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnExecuteCommandChar(wxKeyEvent& event) {
	ScriptParsing::CommandManager &commandManager =
		clsSession.getCommandManager();

	switch (event.GetKeyCode()) {
		case WXK_TAB: {
			const std::string cmd = clsTxtCommand->GetValue().ToStdString();
			const std::string remaining = commandManager.autoComplete(cmd);
				clsTxtCommand->AppendText(remaining);
			break;			
		} // end case
		
		case WXK_UP: {
			clsTxtCommand->SetValue(commandManager.previousHistory());
			clsTxtCommand->SetInsertionPointEnd();
			break;
		} // end case
		
		case WXK_DOWN: {
			clsTxtCommand->SetValue(commandManager.nextHistory());
			clsTxtCommand->SetInsertionPointEnd();
			break;
		} // end case
		
		case WXK_ESCAPE: {
			clsTxtCommand->Clear();
			break;
		} // end case
		
		default:
			event.Skip();
	} // end switch

} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnExecuteCommandKeyDown(wxKeyEvent& event) {
	event.Skip();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnExecuteCommandKeyUp(wxKeyEvent& event) {
	event.Skip();
} // end method


// -----------------------------------------------------------------------------

void MainFrame::OnKeyDown(wxKeyEvent &WXUNUSED(event)) {
} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnCheckCellTimingMode(wxCommandEvent &event) {
	UpdateStats(false);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnOverlayPropertyGridChanged(wxPropertyGridEvent& event) {
	wxPGProperty* property = event.GetProperty();
	
	const std::string name = property->GetName().ToStdString();
	
	if (clsPhysicalCanvasGLPtr->isOverlay(name)) {
		const bool visible = property->GetValue();
		clsPhysicalCanvasGLPtr->setOverlayVisibility(name, visible);
	} else {
		const std::string overlayName = name.substr(0, name.find_first_of('.'));
		const std::string propertyName = 
			name.substr(name.find_first_of('.')+1, name.size());
		
		Rsyn::Json param;
		if (property->GetValueType() == "bool") {
			param[propertyName] = property->GetValue().GetBool(); 
		} else if (property->GetValueType() == "long") {
			param[propertyName] = property->GetValue().GetInteger(); 
		} else if (property->GetValueType() == "wxColour") {
			const wxVariant variant = property->GetValue();
			wxColor color;
			// Awful..
			color << variant;
			const Rsyn::Json value = {
				{"r", int(color.Red())},
				{"g", int(color.Green())},
				{"b", int(color.Blue())}
			};
			
			// Refresh stipple icon
			const int stipple = propToStipple[property];
			wxBitmap bitmap = 
				createBitmapFromMask(STIPPLE_MASKS[stipple], color, color);
			property->GetParent()->SetValueImage(bitmap);
			
			param[propertyName] = value;
		} else {
			std::cout << "ERROR: Property type '" << property->GetValueType() << 
							" not supported\n";
			std::exit(1);
		} // end if
		
		configOverlay(overlayName, param);
	} // end if
	
	if (clsCanvasGLPtr == clsPhysicalCanvasGLPtr)
		clsCanvasGLPtr->Refresh();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::updateCircuitInfo(){
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crashes when a design without physical data is loaded. 
	if (clsPhysicalDesign) {
		clsPropertyGridItemDesignNumMacros->SetValue(clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_BLOCK));
		clsPropertyGridItemDesignNumFixedCells->SetValue(clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_FIXED));
		clsPropertyGridItemDesignNumMovableCells->SetValue(clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE));
	} else {
		clsPropertyGridItemDesignNumMacros->SetValueToUnspecified();
		clsPropertyGridItemDesignNumFixedCells->SetValueToUnspecified();
		clsPropertyGridItemDesignNumMovableCells->SetValueToUnspecified();
	}// end if-else  
	clsPropertyGridItemDesignName->SetValue(clsSession.getDesign().getName());
	clsPropertyGridItemDesignNumCells->SetValue(clsSession.getDesign().getNumInstances(Rsyn::CELL));
	clsPropertyGridItemDesignNumNets->SetValue(clsSession.getDesign().getNumNets());
	clsPropertyGridItemDesignNumInputPins->SetValue(-1);
	clsPropertyGridItemDesignNumOutputPins->SetValue(-1);

	// Update stats
	UpdateStats(false);
} // end method 

// -----------------------------------------------------------------------------

void MainFrame::DoRunScript(const string &filename) {
	try {
		clsSession.evaluateFile(filename);
	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e << "\n";
	} // end catch
} // end method

// -----------------------------------------------------------------------------

void MainFrame::DoSaveSnapshot(const wxString &filename) {
	wxImage image;
	clsCanvasGLPtr->snapshot(image);
	image.SaveFile(filename, wxBITMAP_TYPE_PNG);
} // end method


// -----------------------------------------------------------------------------

void MainFrame::DoChangeView(const View view) {
	CanvasGL *canvas = nullptr;

	// Select the right canvas to show.
	switch (view) {
		case VIEW_PHYSICAL:
			canvas = clsPhysicalCanvasGLPtr;
			clsChoicebookView->ChangeSelection(0);
			break;
		case VIEW_SCHEMATIC: {
			Rsyn::Design design = clsSession.getDesign();
			//workaround to detect if design is not initialized 
			//Isadora 2017-04-26
//			if (design.getNumPins() == 0)
//				break;
			
			if (!clsSchematicCanvas) {
				clsSchematicCanvas = new SchematicCanvasGL(clsPanelMain);
				//clsSchematicCanvasGLPtr = new NewSchematicCanvasGL(clsPanelMain);
				clsSchematicCanvas->attachSession(clsSession);
			} // end if
			else if (clsSchematicCanvas && design != nullptr){
				clsSchematicCanvas->attachSession(clsSession);
			} //end else
			canvas = clsSchematicCanvas;
			clsChoicebookView->ChangeSelection(1);
		} // end case
	} // end switch

	if (canvas && canvas != clsCanvasGLPtr) {
		clsSizerMesh->Clear();
		clsSizerMesh->Add(canvas, 1, wxEXPAND, 0);
		clsSizerMesh->Layout();

		// All canvas actually are drawing on the same pane, so we need to hide
		// them otherwise the last created one will always take precedence.
		if (clsCanvasGLPtr) clsCanvasGLPtr->Hide();

		// Update the current canvas and show it.
		clsCanvasGLPtr = canvas;
		clsCanvasGLPtr->Show();
	} // end if
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnRunScript(wxCommandEvent &WXUNUSED(event)) {
	wxString auxFilename = wxFileSelector(wxT("Run Rsyn Script"),
			wxT(""), wxT(""), wxT(""), wxT("Rsyn Script (*.rsyn)|*.rsyn"));
	if (!auxFilename.empty())
		DoRunScript(auxFilename.ToStdString());

	clsSaveSnapshot->enableWindow();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSavePlFile(wxCommandEvent &WXUNUSED(event)) {
	wxString plFilename = wxFileSelector(wxT("Save Bookshelf PL (Position) File"),
			wxT(""), wxT(""), wxT(""), wxT("Bookshelf PL files (*.pl)|*.pl"), wxFD_SAVE);
	if (!plFilename.empty()) {
		clsWriterPtr->writePlacedBookshelf(plFilename.ToStdString());
	} // end if
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSaveDEF(wxCommandEvent &WXUNUSED(event)) {
	wxString defFilename = wxFileSelector(wxT("Save DEF File"),
			wxT(""), wxT(""), wxT(""), wxT("DEF files (*.def)|*.def"), wxFD_SAVE);
	if (!defFilename.empty()) {
		clsWriterPtr->writeDEF(defFilename.ToStdString());
	} // end if
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSaveSnapshot(wxCommandEvent &WXUNUSED(event)) {
	clsSaveSnapshot->ShowModal();
//	wxString filename = wxFileSelector(wxT("Save Snapshot"), wxT(""), wxT(""), wxT(""), wxT("*.*"), wxFD_SAVE);
//	if (!filename.empty()) {
//		DoSaveSnapshot(filename);
//	} // end if
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnResetCamera(wxCommandEvent &WXUNUSED(event)) {
	clsCanvasGLPtr->resetCamera();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnZoomIn(wxCommandEvent &WXUNUSED(event)) {
	clsCanvasGLPtr->zoom(0.5);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnZoomOut(wxCommandEvent &WXUNUSED(event)) {
	clsCanvasGLPtr->zoom(2);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnChangeView(wxCommandEvent &event) {
	// Select the right canvas to show.
	if (clsMenuItemSchematicCanvas->IsChecked()) {
		DoChangeView(VIEW_SCHEMATIC);
	} else if (clsMenuItemPhysicalCanvas->IsChecked()) {
		DoChangeView(VIEW_PHYSICAL);
	} // end else
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnChangeView(wxChoicebookEvent &event) {
	// Select the right canvas to show.
	if (clsChoicebookView->GetSelection() == 0) {
		DoChangeView(VIEW_PHYSICAL);
	} else if (clsChoicebookView->GetSelection() == 1) {
		DoChangeView(VIEW_SCHEMATIC);
	} // end else
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnLoadPlFile(wxCommandEvent &WXUNUSED(event)) {
	wxString plFilename = wxFileSelector(wxT("Load DEF Placed File"),
			wxT(""), wxT(""), wxT(""), wxT("Load DEF or Bookshelf Placed File (*.def;*.pl)|*.def;*.pl"));
	if (!plFilename.empty()) {
		Rsyn::Json config;
		config["path"] = plFilename.ToStdString();
		clsSession.runReader("loadDesignPosition", config);
		// Update stats
		UpdateStats(true);
	} // end if
	clsSaveSnapshot->enableWindow();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnLoadColorsFile(wxCommandEvent &WXUNUSED(event)) {
	wxString plFilename = wxFileSelector(wxT("Load Colors File"),
			wxT(""), wxT(""), wxT(""), wxT("Bookshelf PL files (*.colors)|*.colors"));
	if (!plFilename.empty()) {

	cout<<"TODO function: "<<__func__<< " at "<<__FILE__<<endl;
		string error;
		/*if (!clsPlacerPtr->loadColors(plFilename.ToStdString(), error))
			wxMessageBox(wxString::From8BitData(error.c_str()), wxT("Parsing Error"));
*/
		// Update stats
		UpdateStats(true);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnLoadPartFile(wxCommandEvent &WXUNUSED(event)) {
	wxString plFilename = wxFileSelector(wxT("Load Partition File"));
	if (!plFilename.empty()) {

		cout<<"TODO function: "<<__func__<< " at "<<__FILE__<<endl;
		string error;
		/*if (!clsPlacerPtr->loadPart(plFilename.ToStdString(), error)) {
			wxMessageBox(wxString::From8BitData(error.c_str()), wxT("Parsing Error"));
		} else {
			clsPlacerPtr->coloringPartition();
		} // end else
*/
		// Update stats
		UpdateStats(true);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnLoadFiedlerFile(wxCommandEvent &WXUNUSED(event)) {
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnLoadLogicalCoreFile(wxCommandEvent &WXUNUSED(event)) {
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringColorful(wxCommandEvent &WXUNUSED(event)) {
	if (!clsSession)
		return;
	clsGraphicsPtr->coloringColorful();
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringRandomBlue(wxCommandEvent &WXUNUSED(event)) {
	if (!clsSession)
		return;
	clsGraphicsPtr->coloringRandomBlue();
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringGray(wxCommandEvent &WXUNUSED(event)) {
	if (!clsSession)
		return;
	clsGraphicsPtr->coloringGray();
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringLateSlack(wxCommandEvent& event) {
	if (!clsSession)
		return;
	clsGraphicsPtr->coloringSlack(Rsyn::LATE);
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringEarlySlack(wxCommandEvent& event) {
	if (!clsSession)
		return;
	clsGraphicsPtr->coloringSlack(Rsyn::EARLY);
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringCriticality( wxCommandEvent& event ){
	if (!clsSession)
		return;

	const Rsyn::TimingMode mode =
		clsOptColoringCriticalityEarly->GetValue()? Rsyn::EARLY : Rsyn::LATE;
	clsGraphicsPtr->coloringCriticality(mode, 0);
	
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringRelativity( wxCommandEvent& event ){
	if (!clsSession)
		return;

	const Rsyn::TimingMode mode =
		clsOptColoringRelativityEarly->GetValue()? Rsyn::EARLY : Rsyn::LATE;
	clsGraphicsPtr->coloringRelativity(mode);
	
	UpdateStats(true);
} // end method


// -----------------------------------------------------------------------------

void MainFrame::OnColoringCentrality( wxCommandEvent& event ){
	if (!clsSession)
		return;

	const Rsyn::TimingMode mode =
		clsOptColoringCentralityEarly->GetValue()? Rsyn::EARLY : Rsyn::LATE;
	clsGraphicsPtr->coloringCentrality(mode, 0);
	
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnGenerateColorsFile(wxCommandEvent &event) {
	//DoGenerateColorsFile();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSearch(wxCommandEvent &WXUNUSED(event)) {
	if (!clsSession) return;

	const string nodeName = clsTxtSearch->GetValue().To8BitData().data();
	Rsyn::Cell cell = clsSession.getDesign().findCellByName(nodeName);
	if (cell == nullptr)
		return;

	clsPhysicalCanvasGLPtr->SelectCell(cell);
	clsPhysicalCanvasGLPtr->CentralizeAtSelectedCell();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnCellSelected(wxCommandEvent &WXUNUSED(event)) {
	updateInstanceProperties(clsPhysicalCanvasGLPtr->getSelectedCell());
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnHoverOverObject(wxCommandEvent &event) {
	const PhysicalCanvasGL::GeoReference * geoRef =
			static_cast<PhysicalCanvasGL::GeoReference *>(event.GetClientData());
	
	std::string label;
	
	if (geoRef) {
		Rsyn::RawPointer rawPointer(geoRef->getData());
		switch (geoRef->getObjectType()) {
			case Rsyn::OBJECT_TYPE_INSTANCE: {
				label = "Cell: " + rawPointer.asInstance().getName();
				break;
			} // end case
			case Rsyn::OBJECT_TYPE_NET: {
				label = "Net: " + rawPointer.asNet().getName();
				break;
			} // end case

			case Rsyn::OBJECT_TYPE_PIN: {
				label = "Pin: " + rawPointer.asPin().getFullName();
				break;
			} // end case
		} // end switch
	} else {
		label = "";
	} // end method
	
	clsLblHover->SetLabel(label);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSelectedCellDrag(wxCommandEvent &WXUNUSED(event)) {
	updateInstanceProperties(clsPhysicalCanvasGLPtr->getSelectedCell(), true);
	// Don't need to redraw here as a draw event was already issued.
	// Don't update stats to avoid too much overhead.
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnUpdateSteinerTrees(wxCommandEvent &WXUNUSED(event)) {
	if (clsOptUpdateFull->GetValue()) {
		clsRoutingEstimatorPtr->updateRoutingFull();
	} else{
		clsRoutingEstimatorPtr->updateRouting();
	} // end else
	
	UpdateStats(true);
} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnUpdateTiming(wxCommandEvent &WXUNUSED(event)) {
	if (clsOptUpdateFull->GetValue()) {
		clsTimerPtr->updateTimingFull();
	} else{
		clsTimerPtr->updateTimingIncremental();
	} // end else
	
	try {
		clsSession.evaluateString("reportDigest");
	} catch (...) {}
	UpdateStats(true);
} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnLegalize(wxCommandEvent &WXUNUSED(event)) {
	try {
		clsSession.evaluateString("legalize");
	} catch (...) {}
	UpdateStats(true);
} // end event 

// -----------------------------------------------------------------------------

void MainFrame::OnLegalizeSelectedCell(wxCommandEvent &WXUNUSED(event)) {
	Rsyn::Cell cell = clsPhysicalCanvasGLPtr->getSelectedCell();
	if (cell) {
		std::cout << "Legalization method: ";
		switch (clsSingleCellLegalizationMethod->GetSelection()) {
			case 0: 
				std::cout << "LEG_EXACT_LOCATION";
				break;
			case 1: 
				std::cout << "LEG_MIN_LINEAR_DISPLACEMENT";
				break;
			case 2: 
				std::cout << "LEG_NEAREST_WHITESPACE";
				break;
				
			default:
				std::cout << "Invalid selection.";
		} // end switch
		std::cout << "\n";

		std::cout << "DISABLED\n";
		UpdateStats(true);
	} // end if
} // end event 

// -----------------------------------------------------------------------------

void MainFrame::OnScroll(wxScrollEvent& WXUNUSED(event)) {
	if (!clsCanvasGLPtr) return;

	if (clsSlider->GetValue() == 0) {
		sizerViewMode->GetStaticBox()->SetLabel(wxT("Interpolated Mode (Off)"));
		clsPhysicalCanvasGLPtr->setInterpolationValue(0.0);
	} else {
		sizerViewMode->GetStaticBox()->SetLabel(wxT("Interpolated Mode (On)"));
		clsPhysicalCanvasGLPtr->setInterpolationValue(clsSlider->GetValue() / float(clsSlider->GetMax()));
	} // end else
} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnCheckpoint(wxCommandEvent& WXUNUSED(event)) {
	if (!clsCanvasGLPtr) return;
		clsPhysicalCanvasGLPtr->storeCheckpoint();
} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnMouseMotion(wxMouseEvent& WXUNUSED(event)) {
	/*
			if ( event.GetEventObject() != clsCanvasPtr ) return;
	
			wxPoint pScreen = event.GetPosition();
			const int x = pScreen.x;
			const int y = pScreen.y;
	
			// Set mouse cursor.
			clsSelectionHover = SelectionBoxHover(x, y);

			if ( !event.ControlDown() ) {
					switch ( clsSelectionHover ) {
							case HOVER_INSIDE      : SetCursor( clsCursorMoving   ); break;
							case HOVER_OUTSIDE     : SetCursor( clsCursorArrow    ); break;
							case HOVER_BORDER_NS   : SetCursor( clsCursorSizeNS   ); break;
							case HOVER_BORDER_WE   : SetCursor( clsCursorSizeWE   ); break;
							case HOVER_BORDER_NW_SE: SetCursor( clsCursorSizeNWSE ); break;
							case HOVER_BORDER_NE_SW: SetCursor( clsCursorSizeNESW ); break;
					} // end switch
			} else
					SetCursor( clsCursorArrow );

			// Perform operation (very useful comment).
			if ( event.Dragging() ) {
					switch ( clsSelectionBoxOperation ) {
							case SELECTION_BOX_OP_MOVING:
									clsSelectionBox.x += x - clsClickX;
									clsSelectionBox.y += y - clsClickY;
				
									clsClickX = x;
									clsClickY = y;
									break;
			
							case SELECTION_BOX_OP_DRAWING:
									clsSelectionBox.x = clsClickX;
									clsSelectionBox.y = clsClickY;
									clsSelectionBox.width  = x - clsClickX;
									clsSelectionBox.height = y - clsClickY;		
									break;
					} // end switch
			} // end if

			// Refresh canvas.
			if ( clsSelectionBoxOperation != SELECTION_BOX_OP_NOP ) {
					clsCanvasPtr->SetSelectionBox( clsSelectionBox );
					clsCanvasPtr->Refresh( true );
			} // end if
	 */
} //end event

// -----------------------------------------------------------------------------

void MainFrame::OnLeftUp(wxMouseEvent& WXUNUSED(event)) {
	/*
			if ( event.GetEventObject() != clsCanvasPtr ) return;
	
			// Avoid negative dimensions.
			if ( clsSelectionBox.width < 0 ) {
					clsSelectionBox.x += clsSelectionBox.width;
					clsSelectionBox.width = -clsSelectionBox.width;
			} // end if
	
			if ( clsSelectionBox.height < 0 ) {
					clsSelectionBox.y += clsSelectionBox.height;
					clsSelectionBox.height = -clsSelectionBox.height;
			} // end if
	
			clsCanvasPtr->SetSelectionBox( clsSelectionBox );
	
			// Collect region information.
			if ( clsNetlistPtr ) {
					const double xrgn = clsCanvasPtr->GetSpaceX( clsSelectionBox.x );
					const double yrgn = clsCanvasPtr->GetSpaceY( clsSelectionBox.y );
					const double wrgn = clsCanvasPtr->GetSpaceX( clsSelectionBox.x + clsSelectionBox.width  ) - xrgn;
					const double hrgn = clsCanvasPtr->GetSpaceY( clsSelectionBox.y + clsSelectionBox.height ) - yrgn;

					int numInternalConnections;
					int numExternalConnections;
					int numInternalCells;
					clsNetlistPtr->collectInformation( xrgn, yrgn, wrgn, hrgn, *clsCurPlacePtr, numInternalConnections, numExternalConnections, numInternalCells );
		
					cerr << "Collected Information\n";
					cerr << "\tNum Cels............: " << numInternalCells << "\n";
					cerr << "\tInternal Connections: " << numInternalConnections << "; avg = " << (numInternalConnections/double(numInternalCells)) << "\n";
					cerr << "\tExternal Connections: " << numExternalConnections << "; avg = " << (numExternalConnections/double(numInternalCells)) << "\n";
					cerr << "\n";
			} // end if
		
			// Clear any current operation.
			clsSelectionBoxOperation = SELECTION_BOX_OP_NOP ;
	 */
} //end event

// -----------------------------------------------------------------------------

void MainFrame::OnLeftDown(wxMouseEvent& WXUNUSED(event)) {
	/*
			if ( event.GetEventObject() != clsCanvasPtr ) return;
		
			if ( clsSelectionHover == HOVER_INSIDE && !event.ControlDown() )
					clsSelectionBoxOperation = SELECTION_BOX_OP_MOVING;
			else
					clsSelectionBoxOperation = SELECTION_BOX_OP_DRAWING;

			clsClickX = event.GetX();
			clsClickY = event.GetY();
	 */
} //end event

// -----------------------------------------------------------------------------

void MainFrame::UpdateStats(const bool redraw) {
//	Rsyn::Timer *timer = clsSessionPtr.getService("rsyn.timer");
//	
//	const double initialSteinerWirelength = clsSessionPtr.getInitialSteinterWirelength();
//	const double initialAbu = clsSessionPtr.getInitialAbu();
//	const double initialEarlyWns = clsSessionPtr.getInitialWns(Rsyn::EARLY);
//	const double initialEarlyTns = clsSessionPtr.getInitialTns(Rsyn::EARLY);
//	const double initialLateWns = clsSessionPtr.getInitialWns(Rsyn::LATE);
//	const double initialLateTns = clsSessionPtr.getInitialTns(Rsyn::LATE);
//	
//	const double currentSteinerWirelength = clsSessionPtr.getTotalSteinerTreeWirelength();
//	const double currentAbu = clsSessionPtr.getAbu();
//	const double currentEarlyWns = timer->getWns(Rsyn::EARLY);
//	const double currentEarlyTns = timer->getTns(Rsyn::EARLY);
//	const double currentLateWns = timer->getWns(Rsyn::LATE);
//	const double currentLateTns = timer->getTns(Rsyn::LATE);
//	
//	const std::string circuitName = "Circuit: "+clsSessionPtr.getName();
//	const double changeSteinerWirelength = 100*((currentSteinerWirelength/initialSteinerWirelength) - 1);
//	const double changeAbu = 100*((currentAbu/initialAbu) - 1);
//	const double changeEarlyWns = 100*((currentEarlyWns/initialEarlyWns) - 1);
//	const double changeEarlyTns = 100*((currentEarlyTns/initialEarlyTns) - 1);
//	const double changeLateWns = 100*((currentLateWns/initialLateWns) - 1);
//	const double changeLateTns = 100*((currentLateTns/initialLateTns) - 1);	
//	
//	SetStatusText( wxString::FromUTF8(circuitName.c_str()), 0);
//	SetStatusText(wxString::Format(wxT("StWL: %.2e (%+.1f%%)"), currentSteinerWirelength, changeSteinerWirelength), 1);
//	SetStatusText(wxString::Format(wxT("ABU: %.5f (%+.1f%%)"), currentAbu, changeAbu), 2);
//	SetStatusText(wxString::Format(wxT("eWNS: %.2e (%+.1f%%)"), currentEarlyWns, changeEarlyWns), 3);
//	SetStatusText(wxString::Format(wxT("eTNS: %.2e (%+.1f%%)"), currentEarlyTns, changeEarlyTns), 4);
//	SetStatusText(wxString::Format(wxT("lWNS: %.2e (%+.1f%%)"), currentLateWns, changeLateWns), 5);
//	SetStatusText(wxString::Format(wxT("lTNS: %.2e (%+.1f%%)"), currentLateTns, changeLateTns), 6);
//	
//	SetStatusText(wxString::Format(wxT("QoR: %.2f"),
//			clsSessionPtr.getQualityScore()), 7);
		
	// Update properties of the selected node.	
	//updateInstanceProperties(clsPhysicalCanvasGLPtr->getSelectedCell());
	
	if (clsSchematicCanvasGLPtr) {
		UpdateSchematicProperties();
	}
		
	// Redraw
	if (redraw)
		clsCanvasGLPtr->Refresh();	
} // end method

// -----------------------------------------------------------------------------

void MainFrame::updateInstanceProperties(Rsyn::Instance instance, const bool updateOnlyPropertiesAffectedByPlacementChange) {
	if (instance && instance.getType() == Rsyn::CELL) {
		Rsyn::Cell cell = instance.asCell();

		// Jucemar - 2017/03/25 -> Updating cell information only if the physical design service is running.
		// This is a protection to crashes when design without physical data are loaded. 
		if (clsPhysicalDesign) {
			Rsyn::PhysicalCell phCell = clsPhysicalDesign.getPhysicalCell(cell);
			const Bounds &rect = phCell.getBounds();
			clsPropertyGridItemCellX->SetValue((int) rect[LOWER][X]);
			clsPropertyGridItemCellY->SetValue((int) rect[LOWER][Y]);
			clsPropertyGridItemCellWidth->SetValue((int) rect.computeLength(X));
			clsPropertyGridItemCellHeight->SetValue((int) rect.computeLength(Y));
		} else {
			clsPropertyGridItemCellX->SetValueToUnspecified();
			clsPropertyGridItemCellY->SetValueToUnspecified();
			clsPropertyGridItemCellWidth->SetValueToUnspecified();
			clsPropertyGridItemCellHeight->SetValueToUnspecified();
		} // end if-else

		if (!updateOnlyPropertiesAffectedByPlacementChange) {
			clsPropertyGridItemCellTimingName->SetValue(wxString::FromAscii(cell.getName().c_str()));
			clsPropertyGridItemCellTimingLibCell->SetValue(wxString::FromAscii(cell.getLibraryCellName().c_str()));
			
			clsPropertyGridItemCellPhysicalName->SetValue(wxString::FromAscii(cell.getName().c_str()));
			clsPropertyGridItemCellPhysicalLibCell->SetValue(wxString::FromAscii(cell.getLibraryCellName().c_str()));

			clsPropertyGridItemCellFixedCurrent->SetValue(cell.isFixed());
			if (clsTimerPtr) {
				Rsyn::Timer *timer = clsSession.getService("rsyn.timer");

				const Rsyn::TimingMode mode =
					clsOptCellTimingModeEarly->GetValue() ? Rsyn::EARLY : Rsyn::LATE;

				clsPropertyGridItemCellCriticality->SetValue(timer->getCellCriticality(cell, mode));
				clsPropertyGridItemCellCentrality->SetValue(timer->getCellCentrality(cell, mode));
				clsPropertyGridItemCellRelativity->SetValue(timer->getCellRelativity(cell, mode));

				std::tuple<Rsyn::Arc, Rsyn::TimingTransition, Rsyn::TimingTransition> t
					= timer->getCellCriticalArc(cell, mode);
				Rsyn::Arc arc = std::get<0>(t);
				const Rsyn::TimingTransition iedge = std::get<1>(t);
				const Rsyn::TimingTransition oedge = std::get<2>(t);

				if (arc) {
					Rsyn::Pin to = arc.getToPin();

					Rsyn::LibraryCharacterizer *libc = 
							clsSession.getService("rsyn.libraryCharacterizer", Rsyn::SERVICE_OPTIONAL);
					
					clsPropertyGridItemCellDelay->SetValue(timer->getArcDelay(arc, mode, oedge));
					clsPropertyGridItemCellInputWireDelay->SetValue(timer->getArcInputWireDelay(arc, mode, iedge));
					clsPropertyGridItemCellOutputWireDelay->SetValue(timer->getArcOutputWireDelay(arc, mode, oedge));
					clsPropertyGridItemCellDriverResistance->SetValue(libc? libc->getDriverResistance(arc, mode, oedge) : 0);
					clsPropertyGridItemCellRcDelay->SetValue(libc? libc->getArcLogicalEffortDelay(arc, mode, oedge) : 0);
					clsPropertyGridItemCellPercentageDelay->SetValue(wxString::Format(wxT("%.2f"), 100 * timer->getArcWorstPercentageDelay(arc, mode, oedge)));

					clsPropertyGridItemCellLoad->SetValue(timer->getPinLoadCapacitance(to, oedge));
					clsPropertyGridItemCellGain->SetValue(timer->getArcGain(arc, mode, oedge));

					clsPropertyGridItemCellInputSlew->SetValue(timer->getArcInputSlew(arc, mode, iedge));
					clsPropertyGridItemCellOutputSlew->SetValue(timer->getArcOutputSlew(arc, mode, oedge));

				} else {

					clsPropertyGridItemCellDelay->SetValueToUnspecified();
					clsPropertyGridItemCellInputWireDelay->SetValueToUnspecified();
					clsPropertyGridItemCellOutputWireDelay->SetValueToUnspecified();
					clsPropertyGridItemCellDriverResistance->SetValueToUnspecified();
					clsPropertyGridItemCellRcDelay->SetValueToUnspecified();

					clsPropertyGridItemCellLoad->SetValueToUnspecified();
					clsPropertyGridItemCellGain->SetValueToUnspecified();

					clsPropertyGridItemCellInputSlew->SetValueToUnspecified();
					clsPropertyGridItemCellOutputSlew->SetValueToUnspecified();
				} // end else
			} // end if 
		} // end if
		
		clsPropertyGridItemCellLegalized->SetValueToUnspecified();
		
		clsChoicebookProperties->SetSelection(2);
		clsNotebook->SetSelection(0);
	} else {
		for (wxPropertyGridIterator it = clsPropertyGridCellPhysical->GetIterator(); !it.AtEnd(); it++) {
			wxPGProperty *property = *it;
			property->SetValueToUnspecified();
		} // end for
		
		for (wxPropertyGridIterator it = clsPropertyGridCellTiming->GetIterator(); !it.AtEnd(); it++) {
			wxPGProperty *property = *it;
			property->SetValueToUnspecified();
		} // end for
	} // end else
} // end method

// -----------------------------------------------------------------------------

void MainFrame::updateNetProperties(Rsyn::Net net) {
	if (net) {
		clsPropertyGridItemNetName->SetValue(net.getName());
		clsPropertyGridItemNetDriverCount->SetValue(net.getNumDrivers());
		clsPropertyGridItemNetSinkCount->SetValue(net.getNumSinks());

		clsChoicebookProperties->SetSelection(3);
		clsNotebook->SetSelection(0);
	} else {
		for (wxPropertyGridIterator it = clsPropertyGridNet->GetIterator(); !it.AtEnd(); it++) {
			wxPGProperty *property = *it;
			property->SetValueToUnspecified();
		} // end for
	} // end else
} // end method

// -----------------------------------------------------------------------------

void MainFrame::updatePinProperties(Rsyn::Pin pin) {
	if (pin) {
		clsPropertyGridItemPinName->SetValue(pin.getName());
		clsPropertyGridItemPinInstance->SetValue(pin.getInstanceName());
		clsPropertyGridItemPinDirection->SetValue(pin.getDirectionName());

		clsChoicebookProperties->SetSelection(4);
		clsNotebook->SetSelection(0);
	} else {
		for (wxPropertyGridIterator it = clsPropertyGridPin->GetIterator(); !it.AtEnd(); it++) {
			wxPGProperty *property = *it;
			property->SetValueToUnspecified();
		} // end for
	} // end else
} // end method

// -----------------------------------------------------------------------------

void MainFrame::UpdateSchematicProperties() {
	
	std::cout<<"update prop\n";
	
	if(!clsSchematicCanvasGLPtr)
		return;
	Rsyn::Instance selected = clsSchematicCanvasGLPtr->getSelectedInstance();
	if(selected != nullptr)
		clsSelectedCellName->ChangeValue(selected.getName());	
	
	clsSchematicCanvasGLPtr->Refresh();
}

void MainFrame::OnRun(wxCommandEvent& event) {
	if (!clsSession)
		return;

	clsSession.runProcess("ufrgs.ISPD16Flow");
	
	UpdateStats(true);
} // end event 

////////////////////////////////////////////////////////////////////////////////
// Session Graphics Events
////////////////////////////////////////////////////////////////////////////////

void MainFrame::processGraphicsEventDesignLoaded() {
	if (!clsSession.getDesign() || clsGraphicsPtr)
		return;

	// Mandatory services.
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crashes when a design without physical data is loaded. 
	if (clsSession.isServiceRunning("rsyn.physical")) {
		clsPhysicalDesign = clsSession.getPhysicalDesign();
	} // end if

	clsGraphicsPtr = clsSession.getService("rsyn.graphics");

	// Optional services.
	clsTimerPtr = clsSession.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);
	clsRoutingEstimatorPtr = clsSession.getService("rsyn.routingEstimator", Rsyn::SERVICE_OPTIONAL);
	clsReportPtr = clsSession.getService("rsyn.report", Rsyn::SERVICE_OPTIONAL);
	clsWriterPtr = clsSession.getService("rsyn.writer", Rsyn::SERVICE_OPTIONAL);

	
	if (clsPhysicalCanvasGLPtr)
		clsPhysicalCanvasGLPtr->init();

	//if (clsSchematicCanvasGLPtr)
	//	clsSchematicCanvasGLPtr->attachSession(clsSession);
		
	startAllOverlays();

	updateCircuitInfo();
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSchematicCellSelected(wxCommandEvent &event) {
	std::cout<<"Running "<<__func__<< "\n";
	UpdateSchematicProperties();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSchematicClickView(wxCommandEvent &event) {
	if(clsSchematicCanvasGLPtr) {
		clsSchematicCanvasGLPtr->setViewCriticalPaths(clsSchematicDrawPaths->IsChecked());
		clsSchematicCanvasGLPtr->setViewSelectedCell(clsSchematicSelectedCell->IsChecked());
		clsSchematicCanvasGLPtr->setViewNighborCells(clsSchematicNeighborCells->IsChecked());
		clsSchematicCanvasGLPtr->setViewLogicCone(clsSchematicLogicCone->IsChecked());
 	} // end if 
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSchematicNumCriticalPaths(wxCommandEvent & event) {
	if(clsSchematicCanvasGLPtr) {
		double val;
		if(clsNumCriticalPaths->GetValue().ToDouble(&val)) {
			clsSchematicCanvasGLPtr->setNumPathsToDraw(int(val));
		}
	}
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnAbout(wxCommandEvent& event) {
	clsAboutDialog->ShowModal();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::processGraphicsEventRefresh() {
	if (!clsSession.getDesign() || clsGraphicsPtr)
		return;

	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::processGraphicsEventUpdateOverlayList() {
	if (!clsGraphicsPtr)
		return;
	
	startAllOverlays();
} // end method

// -----------------------------------------------------------------------------

wxImage MainFrame::createBitmapFromMask(const unsigned char * mask, wxColor line, wxColor fill) {
	struct RGB {
		unsigned char r, g, b;
		RGB() {
			r = g = b = 0;
		}
		RGB(const Color &color) {
			r = color.r;
			g = color.g;
			b = color.b;
		}
	};
	static_assert(sizeof(RGB) == 3,  "RGB must have exactly 3 bytes.");

	RGB *data = new RGB[ 32*32 ];

	Color backgroundColor = Color();
	Color borderColor = Color(line.Red(), line.Green(), line.Blue());

	Color fillColor = Color(fill.Red(), fill.Green(), fill.Blue()) ;

	// Interior
	for ( int i = 0; i < 128; i++ ) {
		unsigned char byte = mask[ i ];
		for ( int k = 0; k < 8; k++ ) {
			const int index = i * 8;
			if ( byte & ( ( (unsigned char) 128 ) >> k ) )
				data[ index + k ] = fillColor;
			else
				data[ index + k ] = backgroundColor;
		} // end for k
	} // end for i

	wxImage image;
	image.SetData( (unsigned char*) data, 32, 32 );

	const int w = 19;
	const int h = 19;

	image.Resize( wxSize( w, h ), wxPoint( 0, 0 ) ); // Remark: this delete the contents of data

	// Border
	data = (RGB * ) image.GetData();
	for ( int i1 =       0; i1 <       w; i1 += 1 ) data[ i1 ] = borderColor; // top
	for ( int i2 =       w; i2 < w*(h-1); i2 += w ) data[ i2 ] = borderColor; // left
	for ( int i3 =   w*2-1; i3 <     w*h; i3 += w ) data[ i3 ] = borderColor; // right
	for ( int i4 = w*(h-1); i4 <     w*h; i4 += 1 ) data[ i4 ] = borderColor; // bottom

	return image;
} // end method