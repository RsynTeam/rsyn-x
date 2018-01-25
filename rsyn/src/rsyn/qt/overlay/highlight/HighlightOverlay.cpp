#include "HighlightOverlay.h"
#include "HoverItem.h"

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
HighlightOverlay::setHoverOutline(const QPainterPath &outline) {
	if (!clsHoverItem) {
		clsHoverItem = new GraphicsHoverItem();
		clsHoverItem->setParentItem(this);
	} // end if
	clsHoverItem->setOutline(outline);
	clsHoverItem->setVisible(true);
} // end method

// -----------------------------------------------------------------------------

void
HighlightOverlay::clearHover() {
	clsHovering = false;
	clsHoverPin = nullptr;
} // end method

// -----------------------------------------------------------------------------

} // end namespace