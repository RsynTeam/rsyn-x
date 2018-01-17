#include "HighlightOverlay.h"

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"

#include "rsyn/qt/GraphicsView.h"
#include "rsyn/qt/infra/PinMgr.h"

#include <limits>
#include <QtWidgets>
#include <QColor>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

HighlightOverlay::HighlightOverlay(QGraphicsItem *parent) :
		GraphicsOverlay("Highlight", parent) {
	setZValue(1000);
	setActive(false);
	setAcceptDrops(false);
	setAcceptHoverEvents(false);
//	setAcceptedMouseButtons(false);
	setFlags(0);
} // end method

// -----------------------------------------------------------------------------

HighlightOverlay::~HighlightOverlay() {
} // end destructor

// -----------------------------------------------------------------------------

bool
HighlightOverlay::init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	QtPinMgr::create();

	setVisible(true);

	if (!physicalDesign)
		return false;

	// Store tracks.
	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();

	bounds = QRectF(coreBounds.getX(), coreBounds.getY(),
			coreBounds.getWidth(), coreBounds.getHeight());

	view->registerObserver(this);
	return true;
} // end method

// -----------------------------------------------------------------------------

void
HighlightOverlay::onChangeVisibility(const std::string &key, const bool visible) {
	if (key == "Highlight") {
		setVisible(visible);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
HighlightOverlay::onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) {
	// nothing to be done
} // end method

// -----------------------------------------------------------------------------

void
HighlightOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) {
	if (clsHovering) {
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::black);
		pen.setStyle(Qt::DashLine);
		pen.setCosmetic(true);

		QBrush brush;

		painter->setPen(pen);
		painter->setBrush(brush);
		painter->drawPath(clsHoverOutline);
	} // end if

} // end method

// -----------------------------------------------------------------------------

} // end namespace