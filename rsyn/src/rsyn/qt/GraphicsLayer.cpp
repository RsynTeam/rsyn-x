#include <QPainter>
#include <QStyleOption>

#include "GraphicsScene.h"
#include "GraphicsLayer.h"
#include "GraphicsItem.h"

namespace Rsyn {

// -----------------------------------------------------------------------------

GraphicsLayer::GraphicsLayer() {
	setAcceptHoverEvents(false);
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
} // end method

// -----------------------------------------------------------------------------

GraphicsLayer::~GraphicsLayer() {
	for (GraphicsItem *item : clsItems) {
		delete item;
	} // end for
} // end method

// -----------------------------------------------------------------------------

QRectF
GraphicsLayer::boundingRect() const {
	return clsScene->sceneRect();
} // end method

// -----------------------------------------------------------------------------

QPainterPath 
GraphicsLayer::shape() const {
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
} // end method

// -----------------------------------------------------------------------------

void 
GraphicsLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) {
	//render(painter, item->exposedRect);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsLayer::render(QPainter *painter, const QRectF &exposedRect) {
	painter->setBrush(clsBrush);
	painter->setPen(clsPen);
	for (GraphicsItem *item : clsItems) {
		item->render(painter);
	} // end for
} // end method

// -----------------------------------------------------------------------------


} // end namespace