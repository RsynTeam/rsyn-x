#include "RoutingGuideOverlay.h"

#include "rsyn/qt/GraphicsView.h"
#include "rsyn/session/Session.h"

namespace Rsyn {

RoutingGuideOverlay::RoutingGuideOverlay(QGraphicsItem *parent) : 
		GraphicsOverlay("RoutingGuides", parent) {
	routingGuide = nullptr;
	rsynGraphics = nullptr;
} // end constructor

// -----------------------------------------------------------------------------

bool RoutingGuideOverlay::init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;
	
	routingGuide = session.getService("rsyn.routingGuide", Rsyn::SERVICE_OPTIONAL);
	if (!routingGuide)
		return false;
	
	rsynGraphics = session.getService("rsyn.graphics");
	if (!rsynGraphics)
		return false;
	
	design = session.getDesign();
	module = design.getTopModule();
	routingGuideItems = design.createAttribute();
	
	processGlobalGuides();
	
	{ // Command: traceGuide
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("traceGuide");
		dscp.setDescription("Enables the drawing of routing guides for a net.");
		
		dscp.addNamedParam("net",
			ScriptParsing::PARAM_TYPE_STRING,
			ScriptParsing::PARAM_SPEC_MANDATORY,
			"Specifies a net to be drawn.");
		
		dscp.addNamedParam("clear",
			ScriptParsing::PARAM_TYPE_BOOLEAN,
			ScriptParsing::PARAM_SPEC_OPTIONAL,
			"Disables the drawing.",
			"false");
		
		session.registerCommand(dscp, [&](const ScriptParsing::Command &command) {
			const std::string netName = command.getParam("net");
			const bool clear = command.getParam("clear");
			
			if (netName == "all") {
				setAll(!clear);
				return;
			} // end if
				
			Rsyn::Net net = design.findNetByName(netName);

			if (!net) {
				std::cout << "[RoutingGuidesOverlay] Net " << netName << " not found.\n";
			} else {
				setGuideVisible(net, !clear);
			} // end if 
		});
	} // end scope

	view->registerObserver(this);
	return true;
} // end method

// -----------------------------------------------------------------------------

void RoutingGuideOverlay::onChangeVisibility(const std::string &key, const bool visible) {
	if (key == "Routing Guides") {
		setVisible(visible);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void RoutingGuideOverlay::onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) {
	// @todo
} // end method

// -----------------------------------------------------------------------------

void RoutingGuideOverlay::processGlobalGuides() {
	for (const Net net : module.allNets()) {
		NetGuide guide = routingGuide->getGuide(net);
		for (const LayerGuide layerGuide : guide.allLayerGuides()) {
			const Rsyn::PhysicalLayer layer = layerGuide.getLayer();
			const int layerId = layer.getRelativeIndex();
			
			const Bounds &bounds = layerGuide.getBounds();
			const QRectF bbox(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
			
			const Color color = rsynGraphics->getRoutingLayerRendering(layerId).getColor();
			const QColor qColor(color.r, color.g, color.b);
			
			FillStippleMask mask = rsynGraphics->getRoutingLayerRendering(layerId).getFillPattern();
			if(layerId == 0) {
				mask = FillStippleMask::STIPPLE_MASK_EMPTY;
			};
			
			QGraphicsItem *item = new RoutingGuideItem(bbox, mask, qColor);
			routingGuideItems[net].push_back(item);
			item->setParentItem(this);
			item->setVisible(false);
		} // end for
	} // end for
} // end method;

// -----------------------------------------------------------------------------

void RoutingGuideOverlay::setGuideVisible(const Net net, const bool visible) {
	for (QGraphicsItem *item : routingGuideItems[net]) {
		item->setVisible(visible);
	} // end for
} // end method

// -----------------------------------------------------------------------------

void RoutingGuideOverlay::setAll(const bool visible) {
	for (const Net net : module.allNets()) {
		setGuideVisible(net, visible);
	} // end for
} // end method

} // end namespace