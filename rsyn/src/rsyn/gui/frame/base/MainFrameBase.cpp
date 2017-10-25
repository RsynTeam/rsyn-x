///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainFrameBase.h"

///////////////////////////////////////////////////////////////////////////

MainFrameBase::MainFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 640,480 ), wxDefaultSize );
	
	clsMenuBar = new wxMenuBar( 0 );
	clsMenuFile = new wxMenu();
	wxMenuItem* clsMenuItemRunScript;
	clsMenuItemRunScript = new wxMenuItem( clsMenuFile, ID_RUN_SCRIPT, wxString( wxT("Run script...") ) + wxT('\t') + wxT("Ctrl+R"), wxEmptyString, wxITEM_NORMAL );
	clsMenuFile->Append( clsMenuItemRunScript );
	
	clsMenuFile->AppendSeparator();
	
	wxMenuItem* clsMenuItemLoadPlFile;
	clsMenuItemLoadPlFile = new wxMenuItem( clsMenuFile, ID_LOAD_PLACEMENT_PL_FILE, wxString( wxT("Load Placement (PL) File...") ) + wxT('\t') + wxT("Ctrl+L"), wxEmptyString, wxITEM_NORMAL );
	clsMenuFile->Append( clsMenuItemLoadPlFile );
	
	wxMenuItem* clsMenuItemLoadColorsFile;
	clsMenuItemLoadColorsFile = new wxMenuItem( clsMenuFile, ID_LOAD_COLORS_FILE, wxString( wxT("Load Colors File...") ) , wxEmptyString, wxITEM_NORMAL );
	clsMenuFile->Append( clsMenuItemLoadColorsFile );
	
	clsMenuFile->AppendSeparator();
	
	wxMenuItem* clsMenuIteamSavelPlFile;
	clsMenuIteamSavelPlFile = new wxMenuItem( clsMenuFile, ID_SAVE_PLACEMENT_PL_FILE, wxString( wxT("Save Placement (PL) File...") ) , wxEmptyString, wxITEM_NORMAL );
	clsMenuFile->Append( clsMenuIteamSavelPlFile );
	
	wxMenuItem* clsMenuItemSaveDEF;
	clsMenuItemSaveDEF = new wxMenuItem( clsMenuFile, ID_SAVE_DEF, wxString( wxT("Save DEF") ) , wxEmptyString, wxITEM_NORMAL );
	clsMenuFile->Append( clsMenuItemSaveDEF );
	
	clsMenuFile->AppendSeparator();
	
	wxMenuItem* clsMenuItemQuit;
	clsMenuItemQuit = new wxMenuItem( clsMenuFile, ID_Quit, wxString( wxT("Quit") ) + wxT('\t') + wxT("CTRL+q"), wxEmptyString, wxITEM_NORMAL );
	clsMenuFile->Append( clsMenuItemQuit );
	
	clsMenuBar->Append( clsMenuFile, wxT("File") ); 
	
	clsMenuCanvas = new wxMenu();
	wxMenuItem* clsMenuItemSaveSnapshot;
	clsMenuItemSaveSnapshot = new wxMenuItem( clsMenuCanvas, ID_SAVE_SNAPSHOT, wxString( wxT("Save Snapshot...") ) , wxEmptyString, wxITEM_NORMAL );
	clsMenuCanvas->Append( clsMenuItemSaveSnapshot );
	
	clsMenuCanvas->AppendSeparator();
	
	wxMenuItem* clsMenuItemResetCamera;
	clsMenuItemResetCamera = new wxMenuItem( clsMenuCanvas, ID_RESET_CAMERA, wxString( wxT("Reset Camera") ) + wxT('\t') + wxT("Ctrl+0"), wxEmptyString, wxITEM_NORMAL );
	clsMenuCanvas->Append( clsMenuItemResetCamera );
	
	wxMenuItem* clsMenuItemZoomIn;
	clsMenuItemZoomIn = new wxMenuItem( clsMenuCanvas, ID_ZOOM_IN, wxString( wxT("Zoom In") ) + wxT('\t') + wxT("Ctrl+="), wxEmptyString, wxITEM_NORMAL );
	clsMenuCanvas->Append( clsMenuItemZoomIn );
	
	wxMenuItem* clsMenuItemZoomOut;
	clsMenuItemZoomOut = new wxMenuItem( clsMenuCanvas, ID_ZOOM_OUT, wxString( wxT("Zoom Out") ) + wxT('\t') + wxT("Ctrl+-"), wxEmptyString, wxITEM_NORMAL );
	clsMenuCanvas->Append( clsMenuItemZoomOut );
	
	clsMenuCanvas->AppendSeparator();
	
	clsMenuItemPhysicalCanvas = new wxMenuItem( clsMenuCanvas, ID_PHYSICAL, wxString( wxT("Physical") ) + wxT('\t') + wxT("Ctrl+1"), wxEmptyString, wxITEM_RADIO );
	clsMenuCanvas->Append( clsMenuItemPhysicalCanvas );
	clsMenuItemPhysicalCanvas->Check( true );
	
	clsMenuItemSchematicCanvas = new wxMenuItem( clsMenuCanvas, ID_SCHEMATIC, wxString( wxT("Schematic") ) + wxT('\t') + wxT("Ctrl+2"), wxEmptyString, wxITEM_RADIO );
	clsMenuCanvas->Append( clsMenuItemSchematicCanvas );
	
	clsMenuBar->Append( clsMenuCanvas, wxT("Canvas") ); 
	
	clsMenuColoring = new wxMenu();
	clsMenuItemColoringColorful = new wxMenuItem( clsMenuColoring, ID_RAINBOW, wxString( wxT("Colorful") ) , wxEmptyString, wxITEM_NORMAL );
	clsMenuColoring->Append( clsMenuItemColoringColorful );
	
	clsMenuItemColoringRandomBlue = new wxMenuItem( clsMenuColoring, ID_COLORING_RANDOM_BLUE, wxString( wxT("Random Blue") ) , wxEmptyString, wxITEM_NORMAL );
	clsMenuColoring->Append( clsMenuItemColoringRandomBlue );
	
	clsMenuitemColoringGray = new wxMenuItem( clsMenuColoring, wxID_ANY, wxString( wxT("Gray") ) , wxEmptyString, wxITEM_NORMAL );
	clsMenuColoring->Append( clsMenuitemColoringGray );
	
	clsMenuColoring->AppendSeparator();
	
	wxMenuItem* clsMenuItemGenerateColorsFile;
	clsMenuItemGenerateColorsFile = new wxMenuItem( clsMenuColoring, ID_GENERATE_COLORS_FILE, wxString( wxT("Generate Colors File...") ) , wxEmptyString, wxITEM_NORMAL );
	clsMenuColoring->Append( clsMenuItemGenerateColorsFile );
	
	clsMenuBar->Append( clsMenuColoring, wxT("Coloring") ); 
	
	clsHelp = new wxMenu();
	clsAbout = new wxMenuItem( clsHelp, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	clsHelp->Append( clsAbout );
	
	clsMenuBar->Append( clsHelp, wxT("Help") ); 
	
	this->SetMenuBar( clsMenuBar );
	
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableRow( 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	clsPanelMain = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	clsSizerMesh = new wxBoxSizer( wxVERTICAL );
	
	
	clsPanelMain->SetSizer( clsSizerMesh );
	clsPanelMain->Layout();
	clsSizerMesh->Fit( clsPanelMain );
	fgSizer3->Add( clsPanelMain, 1, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );
	
	clsNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	clsNotebook->SetMinSize( wxSize( 300,-1 ) );
	
	design_panel = new wxPanel( clsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizerControls;
	sizerControls = new wxBoxSizer( wxVERTICAL );
	
	sizerControls->SetMinSize( wxSize( 250,-1 ) ); 
	clsTxtSearch = new wxSearchCtrl( design_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	clsTxtSearch->ShowSearchButton( true );
	#endif
	clsTxtSearch->ShowCancelButton( false );
	sizerControls->Add( clsTxtSearch, 0, wxALL|wxEXPAND, 5 );
	
	clsChoicebookProperties = new wxChoicebook( design_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	clsPanelDesign = new wxPanel( clsChoicebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	clsPropertyGridDesign = new wxPropertyGrid(clsPanelDesign, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER);
	clsPropertyGridItemDesignName = clsPropertyGridDesign->Append( new wxStringProperty( wxT("Name"), wxT("Name") ) ); 
	m_propertyGridItem35 = clsPropertyGridDesign->Append( new wxPropertyCategory( wxT("Elements"), wxT("Elements") ) ); 
	clsPropertyGridItemDesignNumCells = clsPropertyGridDesign->Append( new wxIntProperty( wxT("Cells"), wxT("Cells") ) ); 
	clsPropertyGridItemDesignNumMacros = clsPropertyGridDesign->Append( new wxIntProperty( wxT("Macros"), wxT("Macros") ) ); 
	clsPropertyGridItemDesignNumInputPins = clsPropertyGridDesign->Append( new wxIntProperty( wxT("Input Pins"), wxT("Input Pins") ) ); 
	clsPropertyGridItemDesignNumOutputPins = clsPropertyGridDesign->Append( new wxIntProperty( wxT("Output Pins"), wxT("Output Pins") ) ); 
	clsPropertyGridItemDesignNumNets = clsPropertyGridDesign->Append( new wxIntProperty( wxT("Nets"), wxT("Nets") ) ); 
	clsPropertyGridItemDesignNumFixedCells = clsPropertyGridDesign->Append( new wxIntProperty( wxT("Fixed Cells"), wxT("Fixed Cells") ) ); 
	clsPropertyGridItemDesignNumMovableCells = clsPropertyGridDesign->Append( new wxIntProperty( wxT("Movable Cells"), wxT("Movable Cells") ) ); 
	bSizer33->Add( clsPropertyGridDesign, 1, wxALL|wxEXPAND, 5 );
	
	
	clsPanelDesign->SetSizer( bSizer33 );
	clsPanelDesign->Layout();
	bSizer33->Fit( clsPanelDesign );
	clsChoicebookProperties->AddPage( clsPanelDesign, wxT("Design"), false );
	clsPanelCellTiming = new wxPanel( clsChoicebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	clsPropertyGridCellTiming = new wxPropertyGrid(clsPanelCellTiming, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER);
	clsPropertyGridItemCellTimingName = clsPropertyGridCellTiming->Append( new wxStringProperty( wxT("Name"), wxT("Name") ) ); 
	clsPropertyGridItemCellTimingLibCell = clsPropertyGridCellTiming->Append( new wxStringProperty( wxT("LibCell"), wxT("LibCell") ) ); 
	m_propertyGridItem1211 = clsPropertyGridCellTiming->Append( new wxPropertyCategory( wxT("Ranking"), wxT("Ranking") ) ); 
	clsPropertyGridItemCellCriticality = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Criticality"), wxT("Criticality") ) ); 
	clsPropertyGridItemCellCentrality = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Centrality"), wxT("Centrality") ) ); 
	clsPropertyGridItemCellRelativity = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Relativity"), wxT("Relativity") ) ); 
	m_propertyGridItem12111 = clsPropertyGridCellTiming->Append( new wxPropertyCategory( wxT("Delay"), wxT("Delay") ) ); 
	clsPropertyGridItemCellDelay = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Cell"), wxT("Cell") ) ); 
	clsPropertyGridItemCellInputWireDelay = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Wire (In)"), wxT("Wire (In)") ) ); 
	clsPropertyGridItemCellOutputWireDelay = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Wire (Out)"), wxT("Wire (Out)") ) ); 
	clsPropertyGridItemCellRcDelay = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("RC"), wxT("RC") ) ); 
	clsPropertyGridItemCellPercentageDelay = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Path (%)"), wxT("Path (%)") ) ); 
	m_propertyGridItem121113 = clsPropertyGridCellTiming->Append( new wxPropertyCategory( wxT("Slew"), wxT("Slew") ) ); 
	clsPropertyGridItemCellInputSlew = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Input"), wxT("Input") ) ); 
	clsPropertyGridItemCellOutputSlew = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Output"), wxT("Output") ) ); 
	m_propertyGridItem121112 = clsPropertyGridCellTiming->Append( new wxPropertyCategory( wxT("Load"), wxT("Load") ) ); 
	clsPropertyGridItemCellLoad = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Capacitance"), wxT("Capacitance") ) ); 
	clsPropertyGridItemCellGain = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Gain"), wxT("Gain") ) ); 
	m_propertyGridItem121111 = clsPropertyGridCellTiming->Append( new wxPropertyCategory( wxT("Characterization"), wxT("Characterization") ) ); 
	clsPropertyGridItemCellDriverResistance = clsPropertyGridCellTiming->Append( new wxFloatProperty( wxT("Rdrive"), wxT("Rdrive") ) ); 
	bSizer7->Add( clsPropertyGridCellTiming, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5931;
	bSizer5931 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText18031 = new wxStaticText( clsPanelCellTiming, wxID_ANY, wxT("Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18031->Wrap( -1 );
	bSizer5931->Add( m_staticText18031, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	clsOptCellTimingModeEarly = new wxRadioButton( clsPanelCellTiming, wxID_ANY, wxT("Early"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5931->Add( clsOptCellTimingModeEarly, 0, wxALL, 5 );
	
	clsOptCellTimingModeLate = new wxRadioButton( clsPanelCellTiming, wxID_ANY, wxT("Late"), wxDefaultPosition, wxDefaultSize, 0 );
	clsOptCellTimingModeLate->SetValue( true ); 
	bSizer5931->Add( clsOptCellTimingModeLate, 0, wxALL, 5 );
	
	
	bSizer7->Add( bSizer5931, 0, wxEXPAND, 5 );
	
	
	clsPanelCellTiming->SetSizer( bSizer7 );
	clsPanelCellTiming->Layout();
	bSizer7->Fit( clsPanelCellTiming );
	clsChoicebookProperties->AddPage( clsPanelCellTiming, wxT("Cell (Timing)"), false );
	clsPanelCellPhysical = new wxPanel( clsChoicebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxVERTICAL );
	
	clsPropertyGridCellPhysical = new wxPropertyGrid(clsPanelCellPhysical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER);
	clsPropertyGridItemCellPhysicalName = clsPropertyGridCellPhysical->Append( new wxStringProperty( wxT("Name"), wxT("Name") ) ); 
	clsPropertyGridItemCellPhysicalLibCell = clsPropertyGridCellPhysical->Append( new wxStringProperty( wxT("LibCell"), wxT("LibCell") ) ); 
	m_propertyGridItem911 = clsPropertyGridCellPhysical->Append( new wxPropertyCategory( wxT("Placement"), wxT("Placement") ) ); 
	clsPropertyGridItemCellX = clsPropertyGridCellPhysical->Append( new wxFloatProperty( wxT("X"), wxT("X") ) ); 
	clsPropertyGridItemCellY = clsPropertyGridCellPhysical->Append( new wxFloatProperty( wxT("Y"), wxT("Y") ) ); 
	clsPropertyGridItemCellFixedCurrent = clsPropertyGridCellPhysical->Append( new wxBoolProperty( wxT("Fixed (Current)"), wxT("Fixed (Current)") ) ); 
	clsPropertyGridItemCellFixedOriginal = clsPropertyGridCellPhysical->Append( new wxBoolProperty( wxT("Fixed (Original)"), wxT("Fixed (Original)") ) ); 
	clsPropertyGridItemCellLegalized = clsPropertyGridCellPhysical->Append( new wxBoolProperty( wxT("Legalized"), wxT("Legalized") ) ); 
	m_propertyGridItem121 = clsPropertyGridCellPhysical->Append( new wxPropertyCategory( wxT("Dimension"), wxT("Dimension") ) ); 
	clsPropertyGridItemCellWidth = clsPropertyGridCellPhysical->Append( new wxFloatProperty( wxT("Width"), wxT("Width") ) ); 
	clsPropertyGridItemCellHeight = clsPropertyGridCellPhysical->Append( new wxFloatProperty( wxT("Height"), wxT("Height") ) ); 
	bSizer71->Add( clsPropertyGridCellPhysical, 1, wxALL|wxEXPAND, 5 );
	
	
	clsPanelCellPhysical->SetSizer( bSizer71 );
	clsPanelCellPhysical->Layout();
	bSizer71->Fit( clsPanelCellPhysical );
	clsChoicebookProperties->AddPage( clsPanelCellPhysical, wxT("Cell (Physical)"), true );
	clsPanelNet = new wxPanel( clsChoicebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer711;
	bSizer711 = new wxBoxSizer( wxVERTICAL );
	
	clsPropertyGridNet = new wxPropertyGrid(clsPanelNet, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER);
	clsPropertyGridItemNetName = clsPropertyGridNet->Append( new wxStringProperty( wxT("Name"), wxT("Name") ) ); 
	clsPropertyGridItemNetDriverCount = clsPropertyGridNet->Append( new wxIntProperty( wxT("Drivers"), wxT("Drivers") ) ); 
	clsPropertyGridItemNetSinkCount = clsPropertyGridNet->Append( new wxIntProperty( wxT("Sinks"), wxT("Sinks") ) ); 
	bSizer711->Add( clsPropertyGridNet, 1, wxALL|wxEXPAND, 5 );
	
	
	clsPanelNet->SetSizer( bSizer711 );
	clsPanelNet->Layout();
	bSizer711->Fit( clsPanelNet );
	clsChoicebookProperties->AddPage( clsPanelNet, wxT("Net"), false );
	clsPanelPin = new wxPanel( clsChoicebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7111;
	bSizer7111 = new wxBoxSizer( wxVERTICAL );
	
	clsPropertyGridPin = new wxPropertyGrid(clsPanelPin, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER);
	clsPropertyGridItemPinName = clsPropertyGridPin->Append( new wxStringProperty( wxT("Name"), wxT("Name") ) ); 
	clsPropertyGridItemPinInstance = clsPropertyGridPin->Append( new wxStringProperty( wxT("Cell"), wxT("Cell") ) ); 
	clsPropertyGridItemPinDirection = clsPropertyGridPin->Append( new wxStringProperty( wxT("Direction"), wxT("Direction") ) ); 
	bSizer7111->Add( clsPropertyGridPin, 1, wxALL|wxEXPAND, 5 );
	
	
	clsPanelPin->SetSizer( bSizer7111 );
	clsPanelPin->Layout();
	bSizer7111->Fit( clsPanelPin );
	clsChoicebookProperties->AddPage( clsPanelPin, wxT("Pin"), false );
	sizerControls->Add( clsChoicebookProperties, 1, wxEXPAND | wxALL, 5 );
	
	
	design_panel->SetSizer( sizerControls );
	design_panel->Layout();
	sizerControls->Fit( design_panel );
	clsNotebook->AddPage( design_panel, wxT("Design"), false );
	canvas_panel = new wxPanel( clsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer56;
	bSizer56 = new wxBoxSizer( wxVERTICAL );
	
	clsChoicebookView = new wxChoicebook( canvas_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	clsPanelPhysicalView = new wxPanel( clsChoicebookView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	sizerViewMode = new wxStaticBoxSizer( new wxStaticBox( clsPanelPhysicalView, wxID_ANY, wxT("Interpolated Mode (Off)") ), wxVERTICAL );
	
	m_panel10 = new wxPanel( sizerViewMode->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_button51 = new wxButton( m_panel10, wxID_ANY, wxT("Checkpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button51, 0, wxALL|wxEXPAND, 5 );
	
	clsSlider = new wxSlider( m_panel10, wxID_ANY, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	bSizer12->Add( clsSlider, 0, wxALL|wxEXPAND, 5 );
	
	
	m_panel10->SetSizer( bSizer12 );
	m_panel10->Layout();
	bSizer12->Fit( m_panel10 );
	sizerViewMode->Add( m_panel10, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	sizerViewMode->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	
	bSizer24->Add( sizerViewMode, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( clsPanelPhysicalView, wxID_ANY, wxT("Overlays") ), wxVERTICAL );
	
	clsOverlayPropertyGrid = new wxPropertyGrid(sbSizer9->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxWANTS_CHARS);
	sbSizer9->Add( clsOverlayPropertyGrid, 1, wxALL|wxEXPAND, 5 );
	
	
	bSizer24->Add( sbSizer9, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer14;
	sbSizer14 = new wxStaticBoxSizer( new wxStaticBox( clsPanelPhysicalView, wxID_ANY, wxT("Coloring") ), wxVERTICAL );
	
	clsChoicebookColoring = new wxChoicebook( sbSizer14->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	m_panel40 = new wxPanel( clsChoicebookColoring, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer561;
	bSizer561 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer59;
	bSizer59 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText180 = new wxStaticText( m_panel40, wxID_ANY, wxT("Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText180->Wrap( -1 );
	bSizer59->Add( m_staticText180, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	clsOptColoringCriticalityEarly = new wxRadioButton( m_panel40, wxID_ANY, wxT("Early"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer59->Add( clsOptColoringCriticalityEarly, 0, wxALL, 5 );
	
	clsOptColoringCriticalityLate = new wxRadioButton( m_panel40, wxID_ANY, wxT("Late"), wxDefaultPosition, wxDefaultSize, 0 );
	clsOptColoringCriticalityLate->SetValue( true ); 
	bSizer59->Add( clsOptColoringCriticalityLate, 0, wxALL, 5 );
	
	
	bSizer561->Add( bSizer59, 0, wxEXPAND, 5 );
	
	clsBtnColoringCriticality = new wxButton( m_panel40, wxID_ANY, wxT("Color"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer561->Add( clsBtnColoringCriticality, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	m_panel40->SetSizer( bSizer561 );
	m_panel40->Layout();
	bSizer561->Fit( m_panel40 );
	clsChoicebookColoring->AddPage( m_panel40, wxT("Criticality"), true );
	m_panel402 = new wxPanel( clsChoicebookColoring, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5612;
	bSizer5612 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer593;
	bSizer593 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1803 = new wxStaticText( m_panel402, wxID_ANY, wxT("Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1803->Wrap( -1 );
	bSizer593->Add( m_staticText1803, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	clsOptColoringCentralityEarly = new wxRadioButton( m_panel402, wxID_ANY, wxT("Early"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer593->Add( clsOptColoringCentralityEarly, 0, wxALL, 5 );
	
	clsOptColoringCentralityLate = new wxRadioButton( m_panel402, wxID_ANY, wxT("Late"), wxDefaultPosition, wxDefaultSize, 0 );
	clsOptColoringCentralityLate->SetValue( true ); 
	bSizer593->Add( clsOptColoringCentralityLate, 0, wxALL, 5 );
	
	
	bSizer5612->Add( bSizer593, 0, wxEXPAND, 5 );
	
	clsBtnColoringCentrality = new wxButton( m_panel402, wxID_ANY, wxT("Color"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5612->Add( clsBtnColoringCentrality, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	m_panel402->SetSizer( bSizer5612 );
	m_panel402->Layout();
	bSizer5612->Fit( m_panel402 );
	clsChoicebookColoring->AddPage( m_panel402, wxT("Centrality"), false );
	m_panel40111 = new wxPanel( clsChoicebookColoring, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer561111;
	bSizer561111 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer59121;
	bSizer59121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText180121 = new wxStaticText( m_panel40111, wxID_ANY, wxT("Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText180121->Wrap( -1 );
	bSizer59121->Add( m_staticText180121, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	clsOptColoringRelativityEarly = new wxRadioButton( m_panel40111, wxID_ANY, wxT("Early"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer59121->Add( clsOptColoringRelativityEarly, 0, wxALL, 5 );
	
	clsOptColoringRelativityLate1 = new wxRadioButton( m_panel40111, wxID_ANY, wxT("Late"), wxDefaultPosition, wxDefaultSize, 0 );
	clsOptColoringRelativityLate1->SetValue( true ); 
	bSizer59121->Add( clsOptColoringRelativityLate1, 0, wxALL, 5 );
	
	
	bSizer561111->Add( bSizer59121, 0, wxEXPAND, 5 );
	
	clsBtnColoringRelativity = new wxButton( m_panel40111, wxID_ANY, wxT("Color"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer561111->Add( clsBtnColoringRelativity, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	m_panel40111->SetSizer( bSizer561111 );
	m_panel40111->Layout();
	bSizer561111->Fit( m_panel40111 );
	clsChoicebookColoring->AddPage( m_panel40111, wxT("Relativity"), false );
	sbSizer14->Add( clsChoicebookColoring, 1, wxEXPAND | wxALL, 5 );
	
	
	bSizer24->Add( sbSizer14, 0, wxEXPAND, 5 );
	
	
	clsPanelPhysicalView->SetSizer( bSizer24 );
	clsPanelPhysicalView->Layout();
	bSizer24->Fit( clsPanelPhysicalView );
	clsChoicebookView->AddPage( clsPanelPhysicalView, wxT("Physical"), true );
	clsPanelSchematicView = new wxPanel( clsChoicebookView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText17 = new wxStaticText( clsPanelSchematicView, wxID_ANY, wxT("Under development..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	m_staticText17->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	m_staticText17->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	
	bSizer23->Add( m_staticText17, 0, wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( clsPanelSchematicView, wxID_ANY, wxT("Drawing paths") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer6->AddGrowableCol( 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText15 = new wxStaticText( sbSizer10->GetStaticBox(), wxID_ANY, wxT("# Critical Paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer6->Add( m_staticText15, 0, wxALL, 5 );
	
	clsNumCriticalPaths = new wxTextCtrl( sbSizer10->GetStaticBox(), wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !clsNumCriticalPaths->HasFlag( wxTE_MULTILINE ) )
	{
	clsNumCriticalPaths->SetMaxLength( 4 );
	}
	#else
	clsNumCriticalPaths->SetMaxLength( 4 );
	#endif
	fgSizer6->Add( clsNumCriticalPaths, 0, wxALL, 5 );
	
	
	sbSizer10->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	clsSchematicDrawPaths = new wxCheckBox( sbSizer10->GetStaticBox(), wxID_ANY, wxT("Draw Paths"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer10->Add( clsSchematicDrawPaths, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer23->Add( sbSizer10, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( clsPanelSchematicView, wxID_ANY, wxT("Draw Selected Cells") ), wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer12;
	sbSizer12 = new wxStaticBoxSizer( new wxStaticBox( sbSizer11->GetStaticBox(), wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText18 = new wxStaticText( sbSizer12->GetStaticBox(), wxID_ANY, wxT("Selected Instance"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	sbSizer12->Add( m_staticText18, 1, wxALL, 5 );
	
	clsSelectedCellName = new wxTextCtrl( sbSizer12->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT|wxTE_READONLY );
	#ifdef __WXGTK__
	if ( !clsSelectedCellName->HasFlag( wxTE_MULTILINE ) )
	{
	clsSelectedCellName->SetMaxLength( 100 );
	}
	#else
	clsSelectedCellName->SetMaxLength( 100 );
	#endif
	sbSizer12->Add( clsSelectedCellName, 1, wxALL|wxEXPAND, 5 );
	
	
	sbSizer11->Add( sbSizer12, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	clsSchematicSelectedCell = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, wxT("Selected Cell"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( clsSchematicSelectedCell, 0, wxALL, 5 );
	
	clsSchematicNeighborCells = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, wxT("Neighbor Cells"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( clsSchematicNeighborCells, 0, wxALL, 5 );
	
	clsSchematicLogicCone = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, wxT("Logic Cone"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( clsSchematicLogicCone, 0, wxALL, 5 );
	
	
	sbSizer11->Add( fgSizer7, 0, wxEXPAND, 5 );
	
	
	bSizer23->Add( sbSizer11, 0, wxEXPAND, 5 );
	
	
	clsPanelSchematicView->SetSizer( bSizer23 );
	clsPanelSchematicView->Layout();
	bSizer23->Fit( clsPanelSchematicView );
	clsChoicebookView->AddPage( clsPanelSchematicView, wxT("Schematic"), false );
	bSizer56->Add( clsChoicebookView, 1, wxEXPAND | wxALL, 5 );
	
	
	canvas_panel->SetSizer( bSizer56 );
	canvas_panel->Layout();
	bSizer56->Fit( canvas_panel );
	clsNotebook->AddPage( canvas_panel, wxT("View"), true );
	opto_panel = new wxPanel( clsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( opto_panel, wxID_ANY, wxT("Flow") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer21->AddGrowableCol( 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	wxString clsFlowChoices[] = { wxT("Default"), wxT("Congestion"), wxT("QP") };
	int clsFlowNChoices = sizeof( clsFlowChoices ) / sizeof( wxString );
	clsFlow = new wxChoice( sbSizer7->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, clsFlowNChoices, clsFlowChoices, 0 );
	clsFlow->SetSelection( 0 );
	fgSizer21->Add( clsFlow, 1, wxALL|wxEXPAND, 5 );
	
	clsBtnRun = new wxButton( sbSizer7->GetStaticBox(), wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer21->Add( clsBtnRun, 0, wxALL|wxEXPAND, 5 );
	
	
	sbSizer7->Add( fgSizer21, 0, wxEXPAND, 5 );
	
	
	bSizer6->Add( sbSizer7, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer141;
	sbSizer141 = new wxStaticBoxSizer( new wxStaticBox( opto_panel, wxID_ANY, wxT("Routing/Timing") ), wxVERTICAL );
	
	wxBoxSizer* bSizer5911;
	bSizer5911 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText18011 = new wxStaticText( sbSizer141->GetStaticBox(), wxID_ANY, wxT("Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18011->Wrap( -1 );
	bSizer5911->Add( m_staticText18011, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	clsOptUpdateFull = new wxRadioButton( sbSizer141->GetStaticBox(), wxID_ANY, wxT("Full"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5911->Add( clsOptUpdateFull, 0, wxALL, 5 );
	
	clsOptUpdateIncremental = new wxRadioButton( sbSizer141->GetStaticBox(), wxID_ANY, wxT("Incremental"), wxDefaultPosition, wxDefaultSize, 0 );
	clsOptUpdateIncremental->SetValue( true ); 
	bSizer5911->Add( clsOptUpdateIncremental, 0, wxALL, 5 );
	
	
	sbSizer141->Add( bSizer5911, 0, wxEXPAND, 5 );
	
	clsBtnUpdateSteinerTrees = new wxButton( sbSizer141->GetStaticBox(), wxID_ANY, wxT("Update Routing Estimation"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer141->Add( clsBtnUpdateSteinerTrees, 0, wxALL|wxEXPAND, 5 );
	
	clsBtnUpdateTiming = new wxButton( sbSizer141->GetStaticBox(), wxID_ANY, wxT("Update Timing"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer141->Add( clsBtnUpdateTiming, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer6->Add( sbSizer141, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer151;
	sbSizer151 = new wxStaticBoxSizer( new wxStaticBox( opto_panel, wxID_ANY, wxT("Legalization (Jezz)") ), wxVERTICAL );
	
	clsBtnLegalize = new wxButton( sbSizer151->GetStaticBox(), wxID_ANY, wxT("Legalize (Full)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer151->Add( clsBtnLegalize, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline13 = new wxStaticLine( sbSizer151->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	sbSizer151->Add( m_staticline13, 0, wxEXPAND | wxALL, 5 );
	
	wxString clsSingleCellLegalizationMethodChoices[] = { wxT("Exact Location"), wxT("Minimum Displacement"), wxT("Nearest Whitespace") };
	int clsSingleCellLegalizationMethodNChoices = sizeof( clsSingleCellLegalizationMethodChoices ) / sizeof( wxString );
	clsSingleCellLegalizationMethod = new wxChoice( sbSizer151->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, clsSingleCellLegalizationMethodNChoices, clsSingleCellLegalizationMethodChoices, 0 );
	clsSingleCellLegalizationMethod->SetSelection( 2 );
	sbSizer151->Add( clsSingleCellLegalizationMethod, 0, wxALL|wxEXPAND, 5 );
	
	clsBtnLegalizeSelectedCell = new wxButton( sbSizer151->GetStaticBox(), wxID_ANY, wxT("Legalize (Selected Cell)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer151->Add( clsBtnLegalizeSelectedCell, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer6->Add( sbSizer151, 0, wxEXPAND, 5 );
	
	
	opto_panel->SetSizer( bSizer6 );
	opto_panel->Layout();
	bSizer6->Fit( opto_panel );
	clsNotebook->AddPage( opto_panel, wxT("Opto"), false );
	
	fgSizer3->Add( clsNotebook, 1, wxALL|wxEXPAND, 5 );
	
	clsTxtCommand = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB );
	#ifdef __WXGTK__
	if ( !clsTxtCommand->HasFlag( wxTE_MULTILINE ) )
	{
	clsTxtCommand->SetMaxLength( 10000 );
	}
	#else
	clsTxtCommand->SetMaxLength( 10000 );
	#endif
	fgSizer3->Add( clsTxtCommand, 0, wxALL|wxEXPAND, 5 );
	
	clsLblHover = new wxStaticText( this, wxID_ANY, wxT("Rsyn"), wxDefaultPosition, wxDefaultSize, 0 );
	clsLblHover->Wrap( -1 );
	fgSizer3->Add( clsLblHover, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	bSizer37->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer37 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrameBase::OnCloseWindow ) );
	this->Connect( clsMenuItemRunScript->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnRunScript ) );
	this->Connect( clsMenuItemLoadPlFile->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnLoadPlFile ) );
	this->Connect( clsMenuItemLoadColorsFile->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnLoadColorsFile ) );
	this->Connect( clsMenuIteamSavelPlFile->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnSavePlFile ) );
	this->Connect( clsMenuItemSaveDEF->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnSaveDEF ) );
	this->Connect( clsMenuItemQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnQuit ) );
	this->Connect( clsMenuItemSaveSnapshot->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnSaveSnapshot ) );
	this->Connect( clsMenuItemResetCamera->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnResetCamera ) );
	this->Connect( clsMenuItemZoomIn->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnZoomIn ) );
	this->Connect( clsMenuItemZoomOut->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnZoomOut ) );
	this->Connect( clsMenuItemPhysicalCanvas->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnChangeView ) );
	this->Connect( clsMenuItemSchematicCanvas->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnChangeView ) );
	this->Connect( clsMenuItemColoringColorful->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnColoringColorful ) );
	this->Connect( clsMenuItemColoringRandomBlue->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnColoringRandomBlue ) );
	this->Connect( clsMenuitemColoringGray->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnColoringGray ) );
	this->Connect( clsMenuItemGenerateColorsFile->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnGenerateColorsFile ) );
	this->Connect( clsAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnAbout ) );
	clsPanelMain->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( MainFrameBase::OnLeftDoubleClick ), NULL, this );
	clsPanelMain->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MainFrameBase::OnLeftDown ), NULL, this );
	clsPanelMain->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( MainFrameBase::OnLeftUp ), NULL, this );
	clsPanelMain->Connect( wxEVT_MOTION, wxMouseEventHandler( MainFrameBase::OnMouseMotion ), NULL, this );
	clsOptCellTimingModeEarly->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrameBase::OnCheckCellTimingMode ), NULL, this );
	clsOptCellTimingModeLate->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrameBase::OnCheckCellTimingMode ), NULL, this );
	clsChoicebookView->Connect( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler( MainFrameBase::OnChangeView ), NULL, this );
	m_button51->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnCheckpoint ), NULL, this );
	clsSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( MainFrameBase::OnScroll ), NULL, this );
	clsOverlayPropertyGrid->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( MainFrameBase::OnOverlayPropertyGridChanged ), NULL, this );
	clsBtnColoringCriticality->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnColoringCriticality ), NULL, this );
	clsBtnColoringCentrality->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnColoringCentrality ), NULL, this );
	clsBtnColoringRelativity->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnColoringRelativity ), NULL, this );
	clsNumCriticalPaths->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrameBase::OnSchematicNumCriticalPaths ), NULL, this );
	clsSchematicDrawPaths->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnSchematicClickView ), NULL, this );
	clsSchematicSelectedCell->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnSchematicClickView ), NULL, this );
	clsSchematicNeighborCells->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnSchematicClickView ), NULL, this );
	clsSchematicLogicCone->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnSchematicClickView ), NULL, this );
	clsBtnRun->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnRun ), NULL, this );
	clsBtnUpdateSteinerTrees->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnUpdateSteinerTrees ), NULL, this );
	clsBtnUpdateTiming->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnUpdateTiming ), NULL, this );
	clsBtnLegalize->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnLegalize ), NULL, this );
	clsBtnLegalizeSelectedCell->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnLegalizeSelectedCell ), NULL, this );
	clsTxtCommand->Connect( wxEVT_CHAR, wxKeyEventHandler( MainFrameBase::OnExecuteCommandChar ), NULL, this );
	clsTxtCommand->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::OnExecuteCommandKeyDown ), NULL, this );
	clsTxtCommand->Connect( wxEVT_KEY_UP, wxKeyEventHandler( MainFrameBase::OnExecuteCommandKeyUp ), NULL, this );
	clsTxtCommand->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( MainFrameBase::OnExecuteCommand ), NULL, this );
}

MainFrameBase::~MainFrameBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrameBase::OnCloseWindow ) );
	this->Disconnect( ID_RUN_SCRIPT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnRunScript ) );
	this->Disconnect( ID_LOAD_PLACEMENT_PL_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnLoadPlFile ) );
	this->Disconnect( ID_LOAD_COLORS_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnLoadColorsFile ) );
	this->Disconnect( ID_SAVE_PLACEMENT_PL_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnSavePlFile ) );
	this->Disconnect( ID_SAVE_DEF, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnSaveDEF ) );
	this->Disconnect( ID_Quit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnQuit ) );
	this->Disconnect( ID_SAVE_SNAPSHOT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnSaveSnapshot ) );
	this->Disconnect( ID_RESET_CAMERA, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnResetCamera ) );
	this->Disconnect( ID_ZOOM_IN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnZoomIn ) );
	this->Disconnect( ID_ZOOM_OUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnZoomOut ) );
	this->Disconnect( ID_PHYSICAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnChangeView ) );
	this->Disconnect( ID_SCHEMATIC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnChangeView ) );
	this->Disconnect( ID_RAINBOW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnColoringColorful ) );
	this->Disconnect( ID_COLORING_RANDOM_BLUE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnColoringRandomBlue ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnColoringGray ) );
	this->Disconnect( ID_GENERATE_COLORS_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnGenerateColorsFile ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnAbout ) );
	clsPanelMain->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( MainFrameBase::OnLeftDoubleClick ), NULL, this );
	clsPanelMain->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MainFrameBase::OnLeftDown ), NULL, this );
	clsPanelMain->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( MainFrameBase::OnLeftUp ), NULL, this );
	clsPanelMain->Disconnect( wxEVT_MOTION, wxMouseEventHandler( MainFrameBase::OnMouseMotion ), NULL, this );
	clsOptCellTimingModeEarly->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrameBase::OnCheckCellTimingMode ), NULL, this );
	clsOptCellTimingModeLate->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrameBase::OnCheckCellTimingMode ), NULL, this );
	clsChoicebookView->Disconnect( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler( MainFrameBase::OnChangeView ), NULL, this );
	m_button51->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnCheckpoint ), NULL, this );
	clsSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( MainFrameBase::OnScroll ), NULL, this );
	clsOverlayPropertyGrid->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( MainFrameBase::OnOverlayPropertyGridChanged ), NULL, this );
	clsBtnColoringCriticality->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnColoringCriticality ), NULL, this );
	clsBtnColoringCentrality->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnColoringCentrality ), NULL, this );
	clsBtnColoringRelativity->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnColoringRelativity ), NULL, this );
	clsNumCriticalPaths->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrameBase::OnSchematicNumCriticalPaths ), NULL, this );
	clsSchematicDrawPaths->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnSchematicClickView ), NULL, this );
	clsSchematicSelectedCell->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnSchematicClickView ), NULL, this );
	clsSchematicNeighborCells->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnSchematicClickView ), NULL, this );
	clsSchematicLogicCone->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnSchematicClickView ), NULL, this );
	clsBtnRun->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnRun ), NULL, this );
	clsBtnUpdateSteinerTrees->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnUpdateSteinerTrees ), NULL, this );
	clsBtnUpdateTiming->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnUpdateTiming ), NULL, this );
	clsBtnLegalize->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnLegalize ), NULL, this );
	clsBtnLegalizeSelectedCell->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnLegalizeSelectedCell ), NULL, this );
	clsTxtCommand->Disconnect( wxEVT_CHAR, wxKeyEventHandler( MainFrameBase::OnExecuteCommandChar ), NULL, this );
	clsTxtCommand->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::OnExecuteCommandKeyDown ), NULL, this );
	clsTxtCommand->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( MainFrameBase::OnExecuteCommandKeyUp ), NULL, this );
	clsTxtCommand->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( MainFrameBase::OnExecuteCommand ), NULL, this );
	
}

AboutDialogBase::AboutDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 500,700 ), wxDefaultSize );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxVERTICAL );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer9->SetMinSize( wxSize( 500,700 ) ); 
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_notebook2->SetMinSize( wxSize( 500,650 ) );
	
	about_panel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer13;
	sbSizer13 = new wxStaticBoxSizer( new wxStaticBox( about_panel, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	
	about_panel->SetSizer( sbSizer13 );
	about_panel->Layout();
	sbSizer13->Fit( about_panel );
	m_notebook2->AddPage( about_panel, wxT("About"), true );
	
	fgSizer9->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1->Realize();
	
	fgSizer9->Add( m_sdbSizer1, 1, wxALIGN_BOTTOM|wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer9 );
	this->Layout();
	fgSizer9->Fit( this );
	
	this->Centre( wxBOTH );
}

AboutDialogBase::~AboutDialogBase()
{
}

SaveSnapshotBase::SaveSnapshotBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer13;
	sbSizer13 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Size and Quality") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText29 = new wxStaticText( sbSizer13->GetStaticBox(), wxID_ANY, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	fgSizer9->Add( m_staticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_spinCtrl2 = new wxSpinCtrl( sbSizer13->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 10000, 0 );
	m_spinCtrl2->Enable( false );
	
	fgSizer9->Add( m_spinCtrl2, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText30 = new wxStaticText( sbSizer13->GetStaticBox(), wxID_ANY, wxT("Interpolation:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	bSizer31->Add( m_staticText30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_comboBox1 = new wxComboBox( sbSizer13->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN ); 
	m_comboBox1->Enable( false );
	m_comboBox1->SetMinSize( wxSize( 200,-1 ) );
	
	bSizer31->Add( m_comboBox1, 0, wxALL, 5 );
	
	
	fgSizer9->Add( bSizer31, 1, wxEXPAND, 5 );
	
	m_staticText31 = new wxStaticText( sbSizer13->GetStaticBox(), wxID_ANY, wxT("Height:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizer9->Add( m_staticText31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_spinCtrl3 = new wxSpinCtrl( sbSizer13->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 10000, 0 );
	m_spinCtrl3->Enable( false );
	
	fgSizer9->Add( m_spinCtrl3, 0, wxALL, 5 );
	
	m_checkBox7 = new wxCheckBox( sbSizer13->GetStaticBox(), wxID_ANY, wxT("Keep aspect ratio"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox7->SetValue(true); 
	m_checkBox7->Enable( false );
	
	fgSizer9->Add( m_checkBox7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	sbSizer13->Add( fgSizer9, 1, wxEXPAND, 5 );
	
	
	fgSizer8->Add( sbSizer13, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer14;
	sbSizer14 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Path") ), wxHORIZONTAL );
	
	m_staticText26 = new wxStaticText( sbSizer14->GetStaticBox(), wxID_ANY, wxT("URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	sbSizer14->Add( m_staticText26, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl15 = new wxTextCtrl( sbSizer14->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_textCtrl15->HasFlag( wxTE_MULTILINE ) )
	{
	m_textCtrl15->SetMaxLength( 10000 );
	}
	#else
	m_textCtrl15->SetMaxLength( 10000 );
	#endif
	m_textCtrl15->Enable( false );
	m_textCtrl15->SetMinSize( wxSize( 280,-1 ) );
	
	sbSizer14->Add( m_textCtrl15, 0, wxALL, 5 );
	
	m_button33 = new wxButton( sbSizer14->GetStaticBox(), wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_button33->Enable( false );
	m_button33->SetMinSize( wxSize( 40,-1 ) );
	
	sbSizer14->Add( m_button33, 0, wxALL, 5 );
	
	m_button34 = new wxButton( sbSizer14->GetStaticBox(), wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button34->Enable( false );
	m_button34->SetMinSize( wxSize( 50,-1 ) );
	
	sbSizer14->Add( m_button34, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	fgSizer8->Add( sbSizer14, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer8 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_spinCtrl2->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SaveSnapshotBase::adjustHeight ), NULL, this );
	m_spinCtrl3->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SaveSnapshotBase::adjustWidth ), NULL, this );
	m_button33->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SaveSnapshotBase::findPath ), NULL, this );
	m_button34->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SaveSnapshotBase::saveSnapshot ), NULL, this );
}

SaveSnapshotBase::~SaveSnapshotBase()
{
	// Disconnect Events
	m_spinCtrl2->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SaveSnapshotBase::adjustHeight ), NULL, this );
	m_spinCtrl3->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SaveSnapshotBase::adjustWidth ), NULL, this );
	m_button33->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SaveSnapshotBase::findPath ), NULL, this );
	m_button34->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SaveSnapshotBase::saveSnapshot ), NULL, this );
	
}
