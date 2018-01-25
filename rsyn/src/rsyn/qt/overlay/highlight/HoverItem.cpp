#include "HoverItem.h"

#include <iostream>

#include <QPen>
#include <QBrush>
#include <QPainter>

namespace Rsyn {

GraphicsHoverItem::GraphicsHoverItem() {
	setZValue(1000);
	setAcceptHoverEvents(false);
	setAcceptTouchEvents(false);
} // end constructor

// -----------------------------------------------------------------------------

QRectF
GraphicsHoverItem::boundingRect() const  {
	return clsOutline.boundingRect();
} // end method

// -----------------------------------------------------------------------------

void
GraphicsHoverItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) {
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::black);
	pen.setStyle(Qt::DashLine);
	pen.setCosmetic(true);

	QBrush brush;

	painter->setPen(pen);
	painter->setBrush(brush);

	painter->drawPath(clsOutline);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsHoverItem::setOutline(const QPainterPath &outline) {
	const QPointF pos(outline.boundingRect().x(), outline.boundingRect().y());

	prepareGeometryChange();
	clsOutline = outline.translated(-pos.x(), -pos.y());
	setPos(pos);
	update();
} // end method

// -----------------------------------------------------------------------------

} // end namespace
