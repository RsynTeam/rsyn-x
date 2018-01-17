#include "RoutingItem.h"

#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/io/Graphics.h"

#include "rsyn/qt/GraphicsView.h"
#include "rsyn/qt/QtUtils.h"

#include <limits>
#include <QtWidgets>

namespace Rsyn {

// -----------------------------------------------------------------------------

RoutingGraphicsItem::RoutingGraphicsItem(Rsyn::Net net, Rsyn::PhysicalWireSegment phWireSegment) {
    setZValue(Rsyn::GRAPHICS_LAYER_ROUTING);

    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(false); // hover events handled in the view

	qreal minx = +std::numeric_limits<qreal>::infinity();
	qreal miny = +std::numeric_limits<qreal>::infinity();
	qreal maxx = -std::numeric_limits<qreal>::infinity();
	qreal maxy = -std::numeric_limits<qreal>::infinity();

	std::vector<QPointF> outline;

	const std::vector<Rsyn::PhysicalRoutingPoint> &routingPts = phWireSegment.allRoutingPoints();
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

		QtUtils::tracePathOutline(points, width/2.0f, outline);
	} // end if

	if (!outline.empty()) {
		for (const QPointF &p : outline) {
			minx = std::min(minx, p.x());
			miny = std::min(miny, p.y());
			maxx = std::max(maxx, p.x());
			maxy = std::max(maxy, p.y());
			poly.append(p);
		} // end for
	} else {
		minx = miny = maxx = maxy = 0;
	} // end else
	
	bbox.setCoords(minx, miny, maxx, maxy);

	bbox.translate(-minx, -miny);
	poly.translate(-minx, -miny);
	
	clsRoutingLayerIndex = phWireSegment.getLayer().getRelativeIndex();
	clsNet = net;

	setPos(minx, miny);
} // end method

// -----------------------------------------------------------------------------

QRectF RoutingGraphicsItem::boundingRect() const {
    return bbox;
} // end method

// -----------------------------------------------------------------------------

QPainterPath RoutingGraphicsItem::shape() const {
    QPainterPath path;
    path.addPolygon(poly);
    return path;
} // end method

// -----------------------------------------------------------------------------

void RoutingGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);

	// TODO: Need a more efficient way to do this.
	const GraphicsView *view = (GraphicsView *) scene()->views().first();
	const qreal lod = view->getLevelOfDetail();

	// Pen
	QPen pen;
	if (option->state & QStyle::State_Selected) {
		pen.setWidth(2);
		pen.setColor(Qt::yellow);
	} else if (option->state & QStyle::State_MouseOver) {
		pen.setWidth(2);
		pen.setColor(Qt::black);
		pen.setStyle(Qt::DashLine);
	} else {
		pen.setColor(view->getRoutingLayerColor(clsRoutingLayerIndex));
	} // end else
	pen.setCosmetic(true);

	// Brush
	const QTransform &wt = painter->worldTransform();
	QBrush brush = view->getStippleBrush(
			view->getRoutingLayerStipple(clsRoutingLayerIndex), wt.dx(), wt.dy());
	brush.setColor(view->getRoutingLayerColor(clsRoutingLayerIndex));

	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawPolygon(poly);
} // end method

// -----------------------------------------------------------------------------

void
RoutingGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
    update();
} // end method

// -----------------------------------------------------------------------------

void
RoutingGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->modifiers() & Qt::ShiftModifier) {
        update();
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
} // end method

// -----------------------------------------------------------------------------

void RoutingGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
    update();
} // end method

} // end namespace