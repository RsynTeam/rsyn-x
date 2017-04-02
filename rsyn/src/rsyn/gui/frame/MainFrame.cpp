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
#include "rsyn/3rdparty/json/json.hpp"

// Services
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/model/timing/Timer.h"
#include "rsyn/model/library/LibraryCharacterizer.h"
#include "rsyn/model/routing/RoutingEstimator.h"
#include "rsyn/io/Report.h"
#include "rsyn/io/Writer.h"
#include "rsyn/io/Graphics.h"
#include "rsyn/gui/canvas/SchematicCanvasGL.h"

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

// -----------------------------------------------------------------------------

MainFrame::MainFrame() : MainFrameBase((wxFrame *) nullptr), clsConfig("UPlace") {
	clsEngine.create();
	clsEngine.registerGraphicsCallback([&](const Rsyn::GraphicsEvent event){
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
	clsCanvasGLPtr->Connect(myEVT_BIN_SELECTED, wxCommandEventHandler(MainFrame::OnBinSelected), NULL, this);
	clsTxtSearch->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::OnSearch), NULL, this);
	clsCanvasGLPtr->Connect(myEVT_CELL_DRAGGED, wxCommandEventHandler(MainFrame::OnSelectedCellDrag), NULL, this);
	
	// Restore command history.
	ScriptParsing::CommandManager &commandManager =
			clsEngine.getCommandManager();
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
	
	clsEngine.destroy();
	
	unregisterAllOverlays();
} // end destructor 

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
		clsEngine.evaluateString(cmd);
		UpdateStats(true); // FIXME: Don't need to redraw for some commands.

		//  Store command in the history.
		const ScriptParsing::CommandManager &commandManager =
			clsEngine.getCommandManager();

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
		clsEngine.getCommandManager();

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

void MainFrame::OnCheckView(wxCommandEvent &WXUNUSED(event)) {
	nlohmann::json instanceOverlayParams;
	instanceOverlayParams["view"]["movable"] = clsMenuItemViewMovableNodes->IsChecked();
	instanceOverlayParams["view"]["fixed"] = clsMenuItemViewFixedNodes->IsChecked();
	instanceOverlayParams["view"]["port"] = clsMenuItemViewPortNodes->IsChecked();
	instanceOverlayParams["view"]["sequential"] = clsMenuItemViewSequentialNodes->IsChecked();
	configOverlay("Instances", instanceOverlayParams);

	if (clsCanvasGLPtr == clsPhysicalCanvasGLPtr)
		clsCanvasGLPtr->Refresh();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnCheckCellTimingMode(wxCommandEvent &event) {
	UpdateStats(false);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnOverlayToggle(wxCommandEvent &event) {
	const std::string overlayName = clsLstOverlays->GetString(event.GetInt()).ToStdString();
	clsPhysicalCanvasGLPtr->setOverlayVisibility(overlayName, clsLstOverlays->IsChecked(event.GetInt()));
	if (clsCanvasGLPtr == clsPhysicalCanvasGLPtr)
		clsCanvasGLPtr->Refresh();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::updateCircuitInfo(){
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crashes when a design without physical data is loaded. 
	if (clsPhysicalPtr) {
		clsPropertyGridItemDesignNumMacros->SetValue(clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_BLOCK));
		clsPropertyGridItemDesignNumFixedCells->SetValue(clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_FIXED));
		clsPropertyGridItemDesignNumMovableCells->SetValue(clsPhysicalDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE));
	} else {
		clsPropertyGridItemDesignNumMacros->SetValueToUnspecified();
		clsPropertyGridItemDesignNumFixedCells->SetValueToUnspecified();
		clsPropertyGridItemDesignNumMovableCells->SetValueToUnspecified();
	}// end if-else  
	clsPropertyGridItemDesignName->SetValue(clsEngine.getDesign().getName());
	clsPropertyGridItemDesignNumCells->SetValue(clsEngine.getDesign().getNumInstances(Rsyn::CELL));
	clsPropertyGridItemDesignNumNets->SetValue(clsEngine.getDesign().getNumNets());
	clsPropertyGridItemDesignNumInputPins->SetValue(-1);
	clsPropertyGridItemDesignNumOutputPins->SetValue(-1);

	// Update stats
	UpdateStats(false);
} // end method 

// -----------------------------------------------------------------------------

void MainFrame::DoRunScript(const string &filename) {
	try {
		clsEngine.evaluateFile(filename);
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
			if (!clsSchematicCanvasGLPtr) {
				//clsSchematicCanvasGLPtr = new SchematicCanvasGL(clsPanelMain);
				clsSchematicCanvasGLPtr = new NewSchematicCanvasGL(clsPanelMain);
				clsSchematicCanvasGLPtr->attachEngine(clsEngine);
			} // end if
			canvas = clsSchematicCanvasGLPtr;
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
		clsEngine.runReader("loadDesignPosition", config);
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
	if (!clsEngine)
		return;
	clsGraphicsPtr->coloringColorful();
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringRandomBlue(wxCommandEvent &WXUNUSED(event)) {
	if (!clsEngine)
		return;
	clsGraphicsPtr->coloringRandomBlue();
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringGray(wxCommandEvent &WXUNUSED(event)) {
	if (!clsEngine)
		return;
	clsGraphicsPtr->coloringGray();
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringLateSlack(wxCommandEvent& event) {
	if (!clsEngine)
		return;
	clsGraphicsPtr->coloringSlack(Rsyn::LATE);
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringEarlySlack(wxCommandEvent& event) {
	if (!clsEngine)
		return;
	clsGraphicsPtr->coloringSlack(Rsyn::EARLY);
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringCriticality( wxCommandEvent& event ){
	if (!clsEngine)
		return;

	const Rsyn::TimingMode mode =
		clsOptColoringCriticalityEarly->GetValue()? Rsyn::EARLY : Rsyn::LATE;
	clsGraphicsPtr->coloringCriticality(mode, 0);
	
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnColoringRelativity( wxCommandEvent& event ){
	if (!clsEngine)
		return;

	const Rsyn::TimingMode mode =
		clsOptColoringRelativityEarly->GetValue()? Rsyn::EARLY : Rsyn::LATE;
	clsGraphicsPtr->coloringRelativity(mode);
	
	UpdateStats(true);
} // end method


// -----------------------------------------------------------------------------

void MainFrame::OnColoringCentrality( wxCommandEvent& event ){
	if (!clsEngine)
		return;

	const Rsyn::TimingMode mode =
		clsOptColoringCentralityEarly->GetValue()? Rsyn::EARLY : Rsyn::LATE;
	clsGraphicsPtr->coloringCentrality(mode, 0);
	
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnShowEarlyCriticalPath(wxCommandEvent& event) {
	if (!clsCanvasGLPtr)
		return;
	clsPhysicalCanvasGLPtr->SetViewEarlyCriticalPath(clsChkShowEarlyCriticalPath->GetValue());
	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnShowLateCriticalPath(wxCommandEvent& event) {
	if (!clsCanvasGLPtr)
		return;
	clsPhysicalCanvasGLPtr->SetViewLateCriticalPath(clsChkShowLateCriticalPath->GetValue());
	UpdateStats(true);	
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnShowFaninTrees(wxCommandEvent& event) {
	if (!clsCanvasGLPtr)
		return;
	clsPhysicalCanvasGLPtr->SetViewFaninTrees(clsChkShowFaninTrees->GetValue());
	UpdateStats(true);	
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnShowFanoutTrees(wxCommandEvent& event) {
	if (!clsCanvasGLPtr)
		return;
	clsPhysicalCanvasGLPtr->SetViewFanoutTrees(clsChkShowFanoutTrees->GetValue());
	UpdateStats(true);		
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnGenerateColorsFile(wxCommandEvent &event) {
	//DoGenerateColorsFile();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSearch(wxCommandEvent &WXUNUSED(event)) {
	if (!clsEngine) return;

	const string nodeName = clsTxtSearch->GetValue().To8BitData().data();
	Rsyn::Cell cell = clsEngine.getDesign().findCellByName(nodeName);
	if (cell == nullptr)
		return;

	clsPhysicalCanvasGLPtr->SelectCell(cell);
	clsPhysicalCanvasGLPtr->CentralizeAtSelectedCell();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnCellSelected(wxCommandEvent &WXUNUSED(event)) {
	UpdateSelectedCellProperties();
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnBinSelected(wxCommandEvent &WXUNUSED(event)) {
	/*Grid grid = clsPlacerPtr->getGrid();

	const int selectedBinIndex = clsCanvasGLPtr->getSelectedBin();
	const Grid::Bin &selectedBin = grid.getBin(selectedBinIndex);

	//double binArea = selectedBin.computeArea();
	clsChoicebookProperties->SetSelection(3);

	if (selectedBinIndex != -1) {
		clsLblBinOccupancyValue->SetLabel(wxString::Format(wxT("%.2f"), selectedBin.occupancyRate));
		clsLblBinMovableUValue->SetLabel(wxString::Format(wxT("%.2f"), selectedBin.layeredU[Grid::LAYER_MOVABLE]));
		clsLblBinBaseUValue->SetLabel(wxString::Format(wxT("%.2f"), selectedBin.layeredU[Grid::LAYER_FIXED]));
	} else {
		clsLblBinOccupancyValue->SetLabel(wxString::Format(wxT("-")));
		clsLblBinMovableUValue->SetLabel(wxString::Format(wxT("-")));
		clsLblBinBaseUValue->SetLabel(wxString::Format(wxT("-")));
	} //end else
	 * */
} // end method

// -----------------------------------------------------------------------------

void MainFrame::OnSelectedCellDrag(wxCommandEvent &WXUNUSED(event)) {
	UpdateSelectedCellProperties(true);
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
		clsEngine.evaluateString("reportDigest");
	} catch (...) {}
	UpdateStats(true);
} // end event

// -----------------------------------------------------------------------------

void MainFrame::OnLegalize(wxCommandEvent &WXUNUSED(event)) {
	try {
		clsEngine.evaluateString("legalize");
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
		clsPhysicalCanvasGLPtr->setInterpolationValue(clsSlider->GetValue() / double(clsSlider->GetMax()));
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
//	Rsyn::Timer *timer = clsEnginePtr.getService("rsyn.timer");
//	
//	const double initialSteinerWirelength = clsEnginePtr.getInitialSteinterWirelength();
//	const double initialAbu = clsEnginePtr.getInitialAbu();
//	const double initialEarlyWns = clsEnginePtr.getInitialWns(Rsyn::EARLY);
//	const double initialEarlyTns = clsEnginePtr.getInitialTns(Rsyn::EARLY);
//	const double initialLateWns = clsEnginePtr.getInitialWns(Rsyn::LATE);
//	const double initialLateTns = clsEnginePtr.getInitialTns(Rsyn::LATE);
//	
//	const double currentSteinerWirelength = clsEnginePtr.getTotalSteinerTreeWirelength();
//	const double currentAbu = clsEnginePtr.getAbu();
//	const double currentEarlyWns = timer->getWns(Rsyn::EARLY);
//	const double currentEarlyTns = timer->getTns(Rsyn::EARLY);
//	const double currentLateWns = timer->getWns(Rsyn::LATE);
//	const double currentLateTns = timer->getTns(Rsyn::LATE);
//	
//	const std::string circuitName = "Circuit: "+clsEnginePtr.getName();
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
//			clsEnginePtr.getQualityScore()), 7);
		
	// Update properties of the selected node.	
	UpdateSelectedCellProperties();
	
	if(clsSchematicCanvasGLPtr) {
		UpdateSchematicProperties();
	}
		
	// Redraw
	if (redraw)
		clsCanvasGLPtr->Refresh();	
} // end method

// -----------------------------------------------------------------------------

void MainFrame::UpdateSelectedCellProperties(const bool updateOnlyPropertiesAffectedByPlacementChange) {
	Rsyn::Cell cell = clsPhysicalCanvasGLPtr->getSelectedCell();

	if (cell != nullptr) {
		// Jucemar - 2017/03/25 -> Updating cell information only if the physical design service is running.
		// This is a protection to crashes when design without physical data are loaded. 
		if (clsPhysicalPtr) {
			Rsyn::PhysicalCell phCell = clsPhysicalDesign.getPhysicalCell(cell);
			const Bounds &rect = phCell.getBounds();
			clsPropertyGridItemCellX->SetValue(rect[LOWER][X]);
			clsPropertyGridItemCellY->SetValue(rect[LOWER][Y]);
			clsPropertyGridItemCellWidth->SetValue(rect.computeLength(X));
			clsPropertyGridItemCellHeight->SetValue(rect.computeLength(Y));
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
				Rsyn::Timer *timer = clsEngine.getService("rsyn.timer");

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
							clsEngine.getService("rsyn.libraryCharacterizer", Rsyn::SERVICE_OPTIONAL);
					
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
		
		clsChoicebookProperties->SetSelection(1);
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
	if (!clsEngine)
		return;

	clsEngine.runProcess("ufrgs.ISPD16Flow");
	
	UpdateStats(true);
} // end event 

// -----------------------------------------------------------------------------

void MainFrame::OnSpinChange(wxSpinEvent& event) {
	if (!clsCanvasGLPtr)
		return;

	clsPhysicalCanvasGLPtr->setCriticalPathWidth( (double) m_spinCtrl1->GetValue() );
} // end method

////////////////////////////////////////////////////////////////////////////////
// Engine Graphics Events
////////////////////////////////////////////////////////////////////////////////

void MainFrame::processGraphicsEventDesignLoaded() {
	if (!clsEngine.getDesign() || clsGraphicsPtr)
		return;

	// Mandatory services.
	// Jucemar - 2017/03/25 -> Physical variable are initialized only when physical service was started.
	// It avoids crashes when a design without physical data is loaded. 
	if (clsEngine.isServiceRunning("rsyn.physical")) {
		clsPhysicalPtr = clsEngine.getService("rsyn.physical");
		// Physical design.
		clsPhysicalDesign = clsPhysicalPtr->getPhysicalDesign();
	} // end if

	clsGraphicsPtr = clsEngine.getService("rsyn.graphics");

	// Optional services.
	clsTimerPtr = clsEngine.getService("rsyn.timer", Rsyn::SERVICE_OPTIONAL);
	clsRoutingEstimatorPtr = clsEngine.getService("rsyn.routingEstimator", Rsyn::SERVICE_OPTIONAL);
	clsReportPtr = clsEngine.getService("rsyn.report", Rsyn::SERVICE_OPTIONAL);
	clsWriterPtr = clsEngine.getService("rsyn.writer", Rsyn::SERVICE_OPTIONAL);

	
	if (clsPhysicalCanvasGLPtr)
		clsPhysicalCanvasGLPtr->attachEngine(clsEngine);

	//if (clsSchematicCanvasGLPtr)
	//	clsSchematicCanvasGLPtr->attachEngine(clsEngine);
		
	registerAllOverlays();

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
	if (!clsEngine.getDesign() || clsGraphicsPtr)
		return;

	UpdateStats(true);
} // end method

// -----------------------------------------------------------------------------

void MainFrame::processGraphicsEventUpdateOverlayList() {
	if (!clsGraphicsPtr)
		return;
	
	registerAllOverlays();
} // end method