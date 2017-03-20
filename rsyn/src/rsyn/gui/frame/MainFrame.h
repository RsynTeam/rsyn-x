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
 
#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include "rsyn/gui/frame/base/MainFrameBase.h"
#include "rsyn/gui/canvas/PhysicalCanvasGL.h"
#include "rsyn/gui/Redirector.h"

#include "rsyn/io/parser/script/ScriptReader.h"
#include "rsyn/io/parser/bookshelf/BookshelfParser.h"
#include "rsyn/engine/Engine.h"

#include <wx/config.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <vector>
using std::vector;
#include <map>
#include <string>

// -----------------------------------------------------------------------------

namespace Rsyn {
class Report;
class Writer;
class RoutingEstimator;
class Timer;
class Graphics;
}

namespace ICCAD15 {
class Infrastructure;
}

class SaveSnapshot;

class MainFrame : public MainFrameBase {
protected:
	PhysicalCanvasGL *clsCanvasGLPtr;
private:

	Rsyn::Engine clsEngine = nullptr;

	// Physical Design
	Rsyn::PhysicalDesign clsPhysicalDesign;

	// Default Services
	Rsyn::PhysicalService * clsPhysicalPtr = nullptr;
	Rsyn::RoutingEstimator * clsRoutingEstimatorPtr = nullptr;
	Rsyn::Timer * clsTimerPtr = nullptr;
	Rsyn::Report * clsReportPtr = nullptr;
	Rsyn::Writer * clsWriterPtr = nullptr;
	Rsyn::Graphics * clsGraphicsPtr = nullptr;

	SaveSnapshot *clsSaveSnapshot = nullptr;
	wxConfig clsConfig;
	
	static const std::string DEFAULT_STORED_SOLUTION_NAME;
	
	//Update circuit informations
	void updateCircuitInfo();

	// Update stats.
	void UpdateStats(const bool redraw);
	
	// Update properties of selected node.
	void UpdateSelectedCellProperties(const bool updateOnlyPropertiesAffectedByPlacementChange = false);

	// Open Configure file for ICCAD 2015 Contest.
	void DoRunScript(const string &filename);

	// Save a snapshot.
	void DoSaveSnapshot(const wxString &filename);
	
	// When window is closed, call engine Engine destructor.
	void OnCloseWindow(wxCloseEvent& event) {
		Destroy();
	} // end method
	
	// Canvas overlay management.
	std::map<std::string, CanvasOverlay *> clsOverlays;
	
	template<typename T>
	void registerOverlay(const std::string &name, const bool visibility = false) {
		auto it = clsOverlays.find(name);
		if (it == clsOverlays.end()) {
			CanvasOverlay * overlay = new T();
			const bool success = overlay->init(clsCanvasGLPtr);
			if (success) {
				clsOverlays[name] = overlay;
				clsCanvasGLPtr->addOverlay(name, overlay, visibility);

				const int id = clsLstOverlays->Append(name);
				clsLstOverlays->Check(id, visibility);
			} else {
				if (visibility) {
					std::cout << "WARNING: Overlay '" + name + "' was not successfully initialized.\n";
				} // end if
				delete overlay;
			} // end else
		} // end else
	} // end method
	
	void registerAllOverlays();

	void unregisterAllOverlays() {
		for (std::pair<std::string, CanvasOverlay *> p : clsOverlays) {
			delete p.second;
		} // end for		
	} // end method
	
	void configOverlay(const std::string &name, const nlohmann::json &params) {
		auto it = clsOverlays.find(name);
		if (it == clsOverlays.end()) {
			std::cout << "ERROR: Canvas overlay '" << name << "' does not exists.\n";
		} else {
			CanvasOverlay * overlay = it->second;
			overlay->config(params);
		} // end else		
	} // end method

	// Engine events.
	void processGraphicsEventDesignLoaded();
	void processGraphicsEventRefresh();
	void processGraphicsEventUpdateOverlayList();

public:

	MainFrame();
	~MainFrame();

	void openBenchmark(const wxString &filename);

	// Event Handlers.
	virtual void OnQuit(wxCommandEvent &event);

	virtual void OnExecuteCommand(wxCommandEvent& event); 
	virtual void OnExecuteCommandChar(wxKeyEvent& event);
	virtual void OnExecuteCommandKeyDown(wxKeyEvent& event);
	virtual void OnExecuteCommandKeyUp(wxKeyEvent& event);	
	
	virtual void OnRunScript(wxCommandEvent &event);

	virtual void OnLoadPlFile(wxCommandEvent &event);
	virtual void OnLoadColorsFile(wxCommandEvent &event);
	virtual void OnLoadPartFile(wxCommandEvent &event);
	virtual void OnLoadFiedlerFile(wxCommandEvent &event);
	virtual void OnLoadLogicalCoreFile(wxCommandEvent &event);

	virtual void OnSavePlFile(wxCommandEvent &event);
	virtual void OnSaveDEF (wxCommandEvent &event);
	virtual void OnSaveSnapshot(wxCommandEvent &event);

	virtual void OnResetCamera(wxCommandEvent& event);
	virtual void OnZoomIn(wxCommandEvent& event);
	virtual void OnZoomOut(wxCommandEvent& event);

	virtual void OnColoringColorful(wxCommandEvent &event);
	virtual void OnColoringRandomBlue(wxCommandEvent &event);
	virtual void OnColoringGray(wxCommandEvent &event);
	virtual void OnColoringLateSlack(wxCommandEvent& event);
	virtual void OnColoringEarlySlack(wxCommandEvent& event);
	virtual void OnColoringCriticality(wxCommandEvent& event);
	virtual void OnColoringCentrality(wxCommandEvent& event);
	virtual void OnColoringRelativity(wxCommandEvent& event);

	virtual void OnShowEarlyCriticalPath(wxCommandEvent& event);
	virtual void OnShowLateCriticalPath(wxCommandEvent& event);
	virtual void OnShowFaninTrees(wxCommandEvent& event);
	virtual void OnShowFanoutTrees(wxCommandEvent& event);
	
	virtual void OnGenerateColorsFile(wxCommandEvent &event);

	virtual void OnSearch(wxCommandEvent &event);

	virtual void OnCheckView(wxCommandEvent &event);
	virtual void OnCheckCellTimingMode(wxCommandEvent &event);
	virtual void OnOverlayToggle(wxCommandEvent &event);

	virtual void OnKeyDown(wxKeyEvent &event);
	virtual void OnMouseMotion(wxMouseEvent &event);
	virtual void OnLeftUp(wxMouseEvent& event);
	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnSelectedCellDrag(wxCommandEvent &event);

	virtual void OnCellSelected(wxCommandEvent &event);
	virtual void OnBinSelected(wxCommandEvent &event);

	virtual void OnCheckpoint(wxCommandEvent& event);
	virtual void OnScroll(wxScrollEvent &event);

	virtual void OnUpdateSteinerTrees(wxCommandEvent& event);
	virtual void OnUpdateTiming(wxCommandEvent& event);
	
	virtual void OnLegalize(wxCommandEvent& event);
	virtual void OnLegalizeSelectedCell(wxCommandEvent& event);
	
	virtual void OnRun( wxCommandEvent& event );
	
	virtual void OnSpinChange(wxSpinEvent& event) override;
}; // end class

// -----------------------------------------------------------------------------

class SaveSnapshot : public SaveSnapshotBase {
private:	
	float ratioWH;
	PhysicalCanvasGL* canvas;
public:
	
	/* Constructor */
	SaveSnapshot(wxWindow* window, PhysicalCanvasGL* canvas) : canvas(canvas),
	SaveSnapshotBase( window ) {
		m_comboBox1->Append( "Nearest (Suggested)" );
		m_comboBox1->Append( "Bilinear" );
		m_comboBox1->Append( "Bicubic" );
		m_comboBox1->Append( "Box average" );
		m_comboBox1->Append( "Normal" );
		m_comboBox1->Append( "High" );
		m_comboBox1->Select( 0 );
	} // end constructor
	
	//--------------------------------------------------------------------------
	
	/* Change the default dimensions and ratio of the image */
	void setDimensions( const int w, const int h ) { 
		m_spinCtrl2->SetValue( w );
		m_spinCtrl3->SetValue( h );
		ratioWH = w / (float) h;
	} // end method
	
	//--------------------------------------------------------------------------
	
	/* Enables the user to save snapshots after the design is loaded */
	void enableWindow() {
		m_spinCtrl2->Enable( true );
		m_spinCtrl3->Enable( true );
		m_textCtrl15->Enable( true );
		m_comboBox1->Enable( true );
		m_checkBox7->Enable( true );
		m_button33->Enable( true );
		m_button34->Enable( true );
		
		if (canvas) {
			m_spinCtrl2->SetValue(canvas->GetSize().x);
			m_spinCtrl3->SetValue(canvas->GetSize().y);
		}
	} // end method
	
	//--------------------------------------------------------------------------
	
	/* Method to keep the aspect ratio */
	void adjustHeight(wxSpinEvent& event) override {
		if( !m_checkBox7->IsChecked() )
			return;
		
		const int width = m_spinCtrl2->GetValue();
		
		 m_spinCtrl3->SetValue( (int) (width / ratioWH) );
	}; // end method
	
	//--------------------------------------------------------------------------

	/* Method to keep the aspect ratio */
	void adjustWidth(wxSpinEvent& event) override {
		if( !m_checkBox7->IsChecked() )
			return;
		
		const int height = m_spinCtrl3->GetValue();
		
		 m_spinCtrl2->SetValue( (int) (height * ratioWH) );
	}; // end method
	
	//--------------------------------------------------------------------------
	
	/* Select the image file */
	void findPath(wxCommandEvent& event) override {
		wxString filename = wxFileSelector(wxT("Save Snapshot"), wxT(""), 
			wxT(""), wxT(""), wxT("*.*"), wxFD_SAVE);
		
		if (!filename.empty())
			m_textCtrl15->SetValue( filename );
	};
	
	//--------------------------------------------------------------------------

	/* Create the snapshot file */
	void saveSnapshot(wxCommandEvent& event) override {
		wxFileName filename( m_textCtrl15->GetValue() );
		
		if( !filename.IsOk() ) {
			wxMessageBox( "Error writting file" );
			return;
		} // end if
		
		if( filename.Exists() && 
			wxMessageBox( "Do you want to overwrite the existing file?", "",  wxYES_NO | wxCENTRE ) != wxYES ) {
			return;
		} // end if
		
		wxImage image;
		canvas->snapshot( image );
		
		const int width = m_spinCtrl2->GetValue();
		const int height = m_spinCtrl3->GetValue();
		
		wxImageResizeQuality resizeQuality;
		
		switch( m_comboBox1->GetSelection() ) {
			case 0:
				resizeQuality = wxImageResizeQuality::wxIMAGE_QUALITY_NEAREST;
				break;
			case 1:
				resizeQuality = wxImageResizeQuality::wxIMAGE_QUALITY_BILINEAR;
				break;
			case 2: 
				resizeQuality = wxImageResizeQuality::wxIMAGE_QUALITY_BICUBIC;
				break;
			case 3:
				resizeQuality = 
					wxImageResizeQuality::wxIMAGE_QUALITY_BOX_AVERAGE;
				break;
			case 4: 
				resizeQuality = wxImageResizeQuality::wxIMAGE_QUALITY_NORMAL;
				break;
			case 5: 
				resizeQuality = wxImageResizeQuality::wxIMAGE_QUALITY_HIGH;
				break;
		}
		
		image.Rescale( width, height, resizeQuality);
		
		image.SaveFile( filename.GetFullPath(), wxBITMAP_TYPE_PNG );
		
		this->Hide();
	};
};

#endif
