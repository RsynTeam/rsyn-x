#include "RoutingGuideItem.h"
#include "rsyn/qt/GraphicsView.h"
#include <QPen>

namespace Rsyn {

void RoutingGuideItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget) {
	Q_UNUSED(widget);

	// TODO: Need a more efficient way to do this.
	const GraphicsView *view = (GraphicsView *) scene()->views().first();
	const qreal lod = view->getNumExposedRows();

	// Pen
	QPen pen;
	pen.setColor(color);
	pen.setCosmetic(true);

	// Brush
	const QTransform &wt = painter->worldTransform();
	QBrush brush = view->getStippleBrush(stipple, wt.dx(), wt.dy());
	brush.setColor(color);

	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawRect(bbox);
} // end method

} // end namespace


