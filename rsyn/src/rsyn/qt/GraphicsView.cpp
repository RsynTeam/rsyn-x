#include "GraphicsView.h"
#include "GraphicsViewport.h"

#include "rsyn/qt/overlay/highlight/HighlightOverlay.h"
#include "rsyn/qt/overlay/instance/CellItem.h"
#include "rsyn/qt/overlay/routing/RoutingItem.h"

#include "rsyn/qt/QtUtils.h"
#include "rsyn/qt/infra/PinMgr.h"

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QEvent>
#include <QToolTip>
#include <QStatusBar>

#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif

#include <iostream>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

GraphicsView::GraphicsView(GraphicsViewport *v) :
		QGraphicsView(), viewContainer(v) {
} // end method

// -----------------------------------------------------------------------------

GraphicsView::~GraphicsView() {
	// Nil the observers's reference to this graphics view to avoid problems
	// when the observer is destroyed and tries to unregister itself.
	for (GraphicsObserver *observer : clsObservers) {
		observer->graphicsView = nullptr;
	} // end for
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::init() {
	Rsyn::Session session;
	physicalDesign = session.getPhysicalDesign();
	rsynGraphics = session.getService("rsyn.graphics");
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::drawBackground(QPainter *painter, const QRectF &rect) {
	QGraphicsView::drawBackground(painter, rect);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::drawForeground(QPainter *painter, const QRectF &rect) {
	QGraphicsView::drawForeground(painter, rect);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::drawItems(QPainter *painter, int numItems,
					   QGraphicsItem *items[],
					   const QStyleOptionGraphicsItem options[]) {
	QGraphicsView::drawItems(painter, numItems, items, options);
} // end method

// -----------------------------------------------------------------------------

#ifndef QT_NO_WHEELEVENT
void
GraphicsView::wheelEvent(QWheelEvent *e) {
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0)
            viewContainer->zoomIn(1);
        else
            viewContainer->zoomOut(1);
		e->accept();
	} else {
		QGraphicsView::wheelEvent(e);
    }
} // end method
#endif

// -----------------------------------------------------------------------------

void
GraphicsView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::mouseMoveEvent(QMouseEvent *event) {
	if (clsHighlightOverlay) {
		QGraphicsItem *item = itemAt(event->pos());

		QString tooltip;
		QPainterPath hoverShape;
		bool needsUpdate = false;

		if (item) {
			if (true /*&& item != clsPreviousHoverItem*/) {
				const QPointF &mappedPos = mapToScene(event->pos());

				if (CellGraphicsItem *cellItem = dynamic_cast<CellGraphicsItem *>(item)) {
					Rsyn::PhysicalCell physicalCell = cellItem->getPhysicalCell();
					Rsyn::Cell cell = physicalCell.getCell();

					Rsyn::Pin hoverPin = nullptr;
					for (Rsyn::Pin pin : cell.allPins()) {
						const QPainterPath &pinShape = QtPinMgr::get()->getShape(pin.getLibraryPin(),
								physicalCell.getOrientation());
						if (pinShape.contains(mappedPos - item->pos())) {
							hoverShape = pinShape.translated(cellItem->pos());
							hoverPin = pin;
							break;
						} // end if
					} // end for

					if (hoverPin && hoverPin != clsHighlightOverlay->getHoverPin()) {
						needsUpdate = true;
						tooltip = QString::fromStdString(hoverPin.getFullName());
					} else if (!hoverPin || item != clsPreviousHoverItem) {
						needsUpdate = true;
						hoverShape = hoverShape = item->shape().translated(item->pos());
						tooltip = QString::fromStdString(cell.getName());
					} // end else

					clsHighlightOverlay->setHoverPin(hoverPin);

				} else if (RoutingGraphicsItem *routingItem = dynamic_cast<RoutingGraphicsItem *>(item)) {
					needsUpdate = true;

					if (event->modifiers() & Qt::ShiftModifier) {
						hoverShape = item->shape().translated(item->pos());
					} else {
						Rsyn::Net net = routingItem->getNet();
						if (net) {
							hoverShape = traceNet(net, true);
							tooltip = QString::fromStdString(net.getName());
						} // end if
					} // end if
				} // end else
			} else {
				// no item
			} // end else
		} else {
			// No item...
		} // end else

		if (clsPreviousHoverItem != item || needsUpdate) {
			clsHighlightOverlay->clearHover();
			clsHighlightOverlay->setHoverOutline(hoverShape);
			clsHighlightOverlay->update();
			if (statusBar) {
				statusBar->showMessage(tooltip);
			} // end if
			clsHighlightOverlay->update();
		} // end if

		clsPreviousHoverItem = item;
	} // end if
	
    QGraphicsView::mouseMoveEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::mouseDoubleClickEvent(QMouseEvent * event) {
	if (event->button() == Qt::LeftButton) {
	} // end if
	QGraphicsView::mouseDoubleClickEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::enterEvent(QEvent *event) {
	QGraphicsView::enterEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::leaveEvent(QEvent *event) {
	if (clsHighlightOverlay) {
		clsHighlightOverlay->clearHover();
		clsHighlightOverlay->update();
	} // end if
	QGraphicsView::leaveEvent(event);
} // end method

// -----------------------------------------------------------------------------

float
GraphicsView::getLevelOfDetail() const {
	return viewContainer->getNumVisibleRowsInViewport();
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::setVisibility(const std::string &key, const bool visible) {
	auto it = clsVisibility.find(key);
	if (visible != it->second) {
		clsVisibility[key] = visible;
		for (GraphicsObserver *observer : clsObservers) {
			observer->onChangeVisibility(key, visible);
		} // end for
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::setPhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) {
	auto it = clsPhysicalLayerVisibility.find(layer);
	if (visible != it->second) {
		clsPhysicalLayerVisibility[layer] = visible;
		for (GraphicsObserver *observer : clsObservers) {
			observer->onChangePhysicalLayerVisibility(layer, visible);
		} // end for
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::setPhysicalLayerVisibility(const std::string &layerName, const bool visible) {
	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
	Rsyn::PhysicalLayer layer = physicalDesign.getPhysicalLayerByName(layerName);
	if (layer) {
		setPhysicalLayerVisibility(layer, visible);
	} // end if
} // end method

// -----------------------------------------------------------------------------

QBrush
GraphicsView::getStippleBrush(const FillStippleMask &mask, const qreal dx, const qreal dy) const {
	GraphicsStippleMgr *mgr = GraphicsStippleMgr::get();
	QBrush brush = mgr->getBrush(mask);
	QTransform transform = invertedTransform;
	transform.translate(-dx, -dy);
	brush.setTransform(transform);
	return brush;
} // end method

// -----------------------------------------------------------------------------

QPainterPath
GraphicsView::traceNet(Rsyn::Net net, const bool includePins) {
	QPainterPath netShape;

	Rsyn::PhysicalNet phNet = physicalDesign.getPhysicalNet(net);
	for (Rsyn::PhysicalWire phWire : phNet.allWires()) {
		for (Rsyn::PhysicalWireSegment phWireSegment : phWire.allSegments()) {
			if (phWireSegment.getNumRoutingPoints() < 2)
				continue;

			const std::vector<Rsyn::PhysicalRoutingPoint> & routingPts = phWireSegment.allRoutingPoints();
			const int numRoutingPoint = routingPts.size();
			if (numRoutingPoint >= 2) {
				std::vector<DBUxy> points;
				points.reserve(numRoutingPoint);

				points.push_back(phWireSegment.getSourcePosition());
				for (int i = 1; i < numRoutingPoint - 1; i++) {
					points.push_back(routingPts[i].getPosition());
				} // end for
				points.push_back(phWireSegment.getTargetPosition());

				Rsyn::PhysicalLayer phLayer = phWireSegment.getLayer();
				const DBU width = phLayer.getWidth();

				std::vector<QPointF> outline;
				QtUtils::tracePathOutline(points, width/2.0f, outline);

				// @todo maybe tracePathOutline should return a polygon directly.
				QPolygonF poly;
				for (const QPointF &p : outline) {
					poly.append(p);
				} // end for

				QPainterPath path;
				path.addPolygon(poly);
				netShape += path;
			} // end if
		} // end for
	} // end for

	if (includePins) {
		for (Rsyn::Pin pin : net.allPins()) {
			Rsyn::Instance instance = pin.getInstance();
			if (instance.getType() == Rsyn::CELL) {
				Rsyn::PhysicalCell physicalCell = physicalDesign.getPhysicalCell(pin);
				Rsyn::LibraryPin lpin = pin.getLibraryPin();
				if (lpin) {
					netShape += QtPinMgr::get()->getShape(lpin,
							physicalCell.getOrientation())
							.translated(physicalCell.getX(), physicalCell.getY());
				} // end for
			} // end if
		} // end for
	} // end if

	return netShape.simplified();;
} // end method

} // end namespace