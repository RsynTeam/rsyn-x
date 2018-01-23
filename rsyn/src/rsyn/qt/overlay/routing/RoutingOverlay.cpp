
#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"

#include "rsyn/qt/GraphicsView.h"
#include "RoutingOverlay.h"
#include "RoutingItem.h"

#include <limits>
#include <QtWidgets>
#include <QColor>
#include <QGraphicsSceneMouseEvent>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

RoutingOverlay::RoutingOverlay(QGraphicsItem *parent) :
		GraphicsOverlay("Routing", parent) {
} // end method

// -----------------------------------------------------------------------------

RoutingOverlay::~RoutingOverlay() {
} // end destructor

// -----------------------------------------------------------------------------

bool RoutingOverlay::init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;
	Rsyn::Module module = session.getTopModule();
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	if (!physicalDesign)
		return false;

	for (Rsyn::PhysicalLayer layer : physicalDesign.allPhysicalLayers()) {
		GraphicsOverlay *graphicsOverlay = new GraphicsOverlay(layer.getName(), this);
		
		mapLayers[layer.getName()] = layers.size();
		layers.push_back(graphicsOverlay);

		addChild(graphicsOverlay);
	} // end for

	const bool useNewPhysicalRouting = false;

	for (Rsyn::Net net : module.allNets()) {
		Rsyn::PhysicalNet phNet = physicalDesign.getPhysicalNet(net);

		if (useNewPhysicalRouting) {
			const Rsyn::PhysicalRouting &routing = phNet.getRouting();
			for (const Rsyn::PhysicalRoutingWire &wire : routing.allWires()) {
				if (!wire.isValid())
					continue;

				QGraphicsItem *item = new RoutingGraphicsItem(net, wire);
				item->setParentItem(layers[wire.getLayer().getIndex()]);
			} // end for
		} else {
		
			for (Rsyn::PhysicalWire phWire : phNet.allWires()) {
				for (Rsyn::PhysicalWireSegment phWireSegment : phWire.allWireSegments()) {
					if (phWireSegment.getNumRoutingPoints() < 2)
						continue;

					QGraphicsItem *item = new RoutingGraphicsItem(net, phWireSegment);
					item->setParentItem(layers[phWireSegment.getLayer().getIndex()]);
				} // end for
			} // end for
		} // end else
	} // end for

	view->registerObserver(this);
	return true;
} // end method

// -----------------------------------------------------------------------------

void
RoutingOverlay::onChangeVisibility(const std::string &key, const bool visible) {
	if (key == "Routing") {
		setVisible(visible);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
RoutingOverlay::onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) {
	layers[layer.getIndex()]->setVisible(visible);
} // end method

// -----------------------------------------------------------------------------

} // end namespace