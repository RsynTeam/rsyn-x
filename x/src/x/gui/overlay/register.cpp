#include "rsyn/gui/frame/MainFrame.h"

// HOW-TO
// To register a canvas overlay, just include its .h below and add a call
// to "registerOverlay<T>(name, visibility)" where T is the canvas overlay class 
// name, name is the overlay label and visibility is the initial visibility 
// state of the overlay.

#include "rsyn/gui/canvas/overlay/Layout.h"
#include "rsyn/gui/canvas/overlay/Region.h"
#include "rsyn/gui/canvas/overlay/DensityGridOverlay.h"

#include "x/gui/overlay/Abu.h"
#include "x/gui/overlay/Jezz.h"
#include "x/gui/overlay/OverlapRemoverOverlay.h"
#include "x/gui/overlay/RoutingEstimatorOverlay.h"

// Overlays
void MainFrame::registerAllOverlays() {
	// Default overlays.
	registerOverlay<LayoutOverlay>("Layout", true);
	registerOverlay<RegionOverlay>("Region", true);
	registerOverlay<AbuOverlay>("Abu");
	registerOverlay<DensityGridOverlay>("Density Grid");
	
	// Non-default overlays.
	registerOverlay<JezzOverlay>("Jezz");
	registerOverlay<OverlapRemoverOverlay>("Macro Overlap");
	registerOverlay<RoutingEstimatorOverlay>("Routing Estimation");
} // end method