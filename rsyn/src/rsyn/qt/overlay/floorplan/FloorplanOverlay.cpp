
#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"

#include "FloorplanOverlay.h"
#include "TrackOverlay.h"
#include "rsyn/qt/GraphicsView.h"

#include <limits>
#include <QtWidgets>
#include <QColor>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

FloorplanOverlay::FloorplanOverlay(QGraphicsItem *parent) :
		GraphicsOverlay("Floorplan", parent) {
} // end method

// -----------------------------------------------------------------------------

FloorplanOverlay::~FloorplanOverlay() {
} // end destructor

// -----------------------------------------------------------------------------

bool
FloorplanOverlay::init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	if (!physicalDesign)
		return false;

	// Store tracks.
	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();

	bounds = QRectF(coreBounds.getX(), coreBounds.getY(),
			coreBounds.getWidth(), coreBounds.getHeight());

	clsTrackOverlay = new TrackOverlay(this);
	addChild(clsTrackOverlay);

	return true;
} // end method

// -----------------------------------------------------------------------------

void
FloorplanOverlay::onChangeVisibility(const std::string &key, const bool visible) {
	if (key == "Floorplan") {
		setVisible(visible);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void 
FloorplanOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) {
	QPen pen;
	pen.setWidth(0);
	pen.setColor(Qt::gray);

	painter->setPen(pen);
	painter->drawRect(bounds);
} // end method

// -----------------------------------------------------------------------------

} // end namespace