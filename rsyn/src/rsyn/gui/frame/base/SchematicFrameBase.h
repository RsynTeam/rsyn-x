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
 
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 16 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SCHEMATICFRAMEBASE_H__
#define __SCHEMATICFRAMEBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/propgrid/propgrid.h>
#include <wx/stattext.h>
#include <wx/radiobut.h>
#include <wx/choicebk.h>
#include <wx/button.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/checklst.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/notebook.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SchematicFrameBase
///////////////////////////////////////////////////////////////////////////////
class SchematicFrameBase : public wxFrame 
{
	private:
	
	protected:
		wxPanel* clsPanelMain;
		wxBoxSizer* clsSizerMesh;
		wxNotebook* clsNotebook;
		wxPanel* design_panel;
		wxSearchCtrl* clsTxtSearch;
		wxChoicebook* clsChoicebookProperties;
		wxPanel* clsPanelDesign;
		wxPropertyGrid* clsPropertyGridDesign;
		wxPGProperty* clsPropertyGridItemDesignName;
		wxPGProperty* m_propertyGridItem35;
		wxPGProperty* clsPropertyGridItemDesignNumCells;
		wxPGProperty* clsPropertyGridItemDesignNumMacros;
		wxPGProperty* clsPropertyGridItemDesignNumInputPins;
		wxPGProperty* clsPropertyGridItemDesignNumOutputPins;
		wxPGProperty* clsPropertyGridItemDesignNumNets;
		wxPGProperty* clsPropertyGridItemDesignNumFixedCells;
		wxPGProperty* clsPropertyGridItemDesignNumMovableCells;
		wxPanel* clsPanelCellTiming;
		wxPropertyGrid* clsPropertyGridCellTiming;
		wxPGProperty* clsPropertyGridItemCellTimingName;
		wxPGProperty* clsPropertyGridItemCellTimingLibCell;
		wxPGProperty* m_propertyGridItem1211;
		wxPGProperty* clsPropertyGridItemCellCriticality;
		wxPGProperty* clsPropertyGridItemCellCentrality;
		wxPGProperty* clsPropertyGridItemCellRelativity;
		wxPGProperty* m_propertyGridItem12111;
		wxPGProperty* clsPropertyGridItemCellDelay;
		wxPGProperty* clsPropertyGridItemCellInputWireDelay;
		wxPGProperty* clsPropertyGridItemCellOutputWireDelay;
		wxPGProperty* clsPropertyGridItemCellRcDelay;
		wxPGProperty* clsPropertyGridItemCellPercentageDelay;
		wxPGProperty* m_propertyGridItem121113;
		wxPGProperty* clsPropertyGridItemCellInputSlew;
		wxPGProperty* clsPropertyGridItemCellOutputSlew;
		wxPGProperty* m_propertyGridItem121112;
		wxPGProperty* clsPropertyGridItemCellLoad;
		wxPGProperty* clsPropertyGridItemCellGain;
		wxPGProperty* m_propertyGridItem121111;
		wxPGProperty* clsPropertyGridItemCellDriverResistance;
		wxStaticText* m_staticText18031;
		wxRadioButton* clsOptCellTimingModeEarly;
		wxRadioButton* clsOptCellTimingModeLate;
		wxPanel* m_panel81;
		wxPropertyGrid* clsPropertyGridCellPhysical;
		wxPGProperty* clsPropertyGridItemCellPhysicalName;
		wxPGProperty* clsPropertyGridItemCellPhysicalLibCell;
		wxPGProperty* m_propertyGridItem911;
		wxPGProperty* clsPropertyGridItemCellX;
		wxPGProperty* clsPropertyGridItemCellY;
		wxPGProperty* clsPropertyGridItemCellFixedCurrent;
		wxPGProperty* clsPropertyGridItemCellFixedOriginal;
		wxPGProperty* clsPropertyGridItemCellLegalized;
		wxPGProperty* m_propertyGridItem121;
		wxPGProperty* clsPropertyGridItemCellWidth;
		wxPGProperty* clsPropertyGridItemCellHeight;
		wxPanel* canvas_panel;
		wxStaticBoxSizer* sizerViewMode;
		wxPanel* m_panel10;
		wxButton* m_button51;
		wxSlider* clsSlider;
		wxCheckListBox* clsLstOverlays;
		wxStaticText* m_staticText59;
		wxCheckBox* clsChkShowEarlyCriticalPath;
		wxCheckBox* clsChkShowLateCriticalPath;
		wxStaticText* m_staticText21;
		wxStaticText* m_staticText22;
		wxStaticText* m_staticText60;
		wxCheckBox* clsChkShowFaninTrees;
		wxCheckBox* clsChkShowFanoutTrees;
		wxCheckBox* clsItemSnapshot;
		wxChoicebook* clsChoiceBookColoring;
		wxPanel* m_panel40;
		wxStaticText* m_staticText180;
		wxRadioButton* clsOptColoringCriticalityEarly;
		wxRadioButton* clsOptColoringCriticalityLate;
		wxButton* clsBtnColoringCriticality;
		wxPanel* m_panel402;
		wxStaticText* m_staticText1803;
		wxRadioButton* clsOptColoringCentralityEarly;
		wxRadioButton* clsOptColoringCentralityLate;
		wxButton* clsBtnColoringCentrality;
		wxPanel* m_panel40111;
		wxStaticText* m_staticText180121;
		wxRadioButton* clsOptColoringRelativityEarly;
		wxRadioButton* clsOptColoringRelativityLate1;
		wxButton* clsBtnColoringRelativity;
		wxPanel* opto_panel;
		wxChoice* clsFlow;
		wxButton* clsBtnRun;
		wxStaticText* m_staticText18011;
		wxRadioButton* clsOptUpdateFull;
		wxRadioButton* clsOptUpdateIncremental;
		wxButton* clsBtnUpdateSteinerTrees;
		wxButton* clsBtnUpdateTiming;
		wxButton* clsBtnLegalize;
		wxStaticLine* m_staticline13;
		wxChoice* clsSingleCellLegalizationMethod;
		wxButton* clsBtnLegalizeSelectedCell;
		wxButton* clsBtnActionJucemar;
		wxButton* clsBtnActionMateus;
		wxButton* clsBtnActionGuilherme;
		wxButton* clsBtnActionHenrique;
		wxStaticLine* m_staticline41;
		wxTextCtrl* clsTextUtilTarget;
		wxTextCtrl* clsTextMaxDisp;
		wxTextCtrl* clsTxtCommand;
		wxStaticText* clsLblQualityScore;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseWindow( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnLeftDoubleClick( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLeftUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseMotion( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnCheckCellTimingMode( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckpoint( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnOverlayToggle( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowEarlyCriticalPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowLateCriticalPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSpinChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnShowFaninTrees( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowFanoutTrees( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckView( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnColoringCriticality( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnColoringCentrality( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnColoringRelativity( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRun( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateSteinerTrees( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateTiming( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLegalize( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLegalizeSelectedCell( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActionJucemar( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActionMateus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActionGuilherme( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActionHenrique( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExecuteCommandChar( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnExecuteCommandKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnExecuteCommandKeyUp( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnExecuteCommand( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxSpinCtrl* m_spinCtrl1;
		
		SchematicFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Schematic View"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1280,720 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~SchematicFrameBase();
	
};

#endif //__SCHEMATICFRAMEBASE_H__
