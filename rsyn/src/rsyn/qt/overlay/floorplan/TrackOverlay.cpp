
#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/io/Graphics.h"

#include "rsyn/qt/GraphicsView.h"
#include "rsyn/qt/QtUtils.h"
#include "TrackOverlay.h"

#include <limits>
#include <QtWidgets>
#include <QColor>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

TrackOverlay::TrackOverlay(QGraphicsItem *parent) :
		GraphicsOverlay("Tracks", parent) {
	setVisible(false);
} // end constructor

// -----------------------------------------------------------------------------

TrackOverlay::~TrackOverlay() {
} // end destructor

// -----------------------------------------------------------------------------

bool
TrackOverlay::init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;

	physicalDesign = session.getPhysicalDesign();
	if (!physicalDesign)
		return false;

	rsynGraphics = session.getService("rsyn.graphics");
	if (!rsynGraphics)
		return false;

	// Store tracks.
	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();

	bounds = QRectF(coreBounds.getX(), coreBounds.getY(),
			coreBounds.getWidth(), coreBounds.getHeight());

	this->view = view;
	view->registerObserver(this);
	return true;
} // end method

// -----------------------------------------------------------------------------

void
TrackOverlay::onChangeVisibility(const std::string &key, const bool visible) {
	if (key == "Tracks") {
		setVisible(visible);
	} else if (key == "Tracks/Preferred") {
		preferredDirectionVisibility = visible;
		update();
	} else if (key == "Tracks/Non-Preferred") {
		nonPreferredDirectionVisibility = visible;
		update();
	} // end else-if
} // end method

// -----------------------------------------------------------------------------

void
TrackOverlay::onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) {
	update();
} // end method

// -----------------------------------------------------------------------------

void 
TrackOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) {
	QPen pen;
	pen.setWidth(0);
	pen.setColor(Qt::gray);
	pen.setStyle(Qt::DashLine);

	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();

	for (Rsyn::PhysicalTrack phTrack : physicalDesign.allPhysicalTracks()) {
		Rsyn::PhysicalTrackDirection dir = phTrack.getDirection();
		Rsyn::PhysicalTrackDirection reverse;
		if(dir == Rsyn::PhysicalTrackDirection::TRACK_HORIZONTAL)
			reverse = Rsyn::PhysicalTrackDirection::TRACK_VERTICAL;
		else 
			reverse = Rsyn::PhysicalTrackDirection::TRACK_HORIZONTAL;
		const DBU space = phTrack.getSpace();
		for (Rsyn::PhysicalLayer phLayer : phTrack.allLayers()) {
			if (!view->getPhysicalLayerVisibility(phLayer))
				continue;

			const bool isPreferredDirection =
					(dir == TRACK_VERTICAL && phLayer.getDirection() == Rsyn::VERTICAL) ||
					(dir == TRACK_HORIZONTAL && phLayer.getDirection() == Rsyn::HORIZONTAL);
			const bool isNonPreferredDiraction =
					(dir == TRACK_VERTICAL && phLayer.getDirection() == Rsyn::HORIZONTAL) ||
					(dir == TRACK_HORIZONTAL && phLayer.getDirection() == Rsyn::VERTICAL);

			const bool show =
				(nonPreferredDirectionVisibility && isNonPreferredDiraction) ||
				(preferredDirectionVisibility && isPreferredDirection);

			if (!show)
				continue;

			DBUxy p0;
			DBUxy p1;

			p0[dir] = phTrack.getLocation();
			p1[dir] = phTrack.getLocation();

			p0[reverse] = coreBounds[LOWER][reverse];
			p1[reverse] = coreBounds[UPPER][reverse];

			pen.setColor(QtUtils::convert(
				rsynGraphics->getRoutingLayerRendering(phLayer.getRelativeIndex()).getColor()));
			painter->setPen(pen);

			for (int i = 0; i < phTrack.getNumberOfTracks(); i++) {
				painter->drawLine(p0.x, p0.y, p1.x, p1.y);
				p0[dir] += space;
				p1[dir] += space;
			} // end for
		} // end for
	} // end for

} // end method

// -----------------------------------------------------------------------------

} // end namespace