#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/session/Session.h"
#include "rsyn/io/Graphics.h"

#include "rsyn/qt/PropertyDialog.h"
#include "rsyn/qt/GraphicsView.h"
#include "rsyn/qt/QtUtils.h"
#include "rsyn/qt/infra/PinMgr.h"

#include "MacroItem.h"

#include <QtWidgets>

namespace Rsyn {

MacroGraphicsItem::MacroGraphicsItem(Rsyn::PhysicalCell physicalCell) :
	CellGraphicsItem(physicalCell)
{
	Rsyn::Session session;

	Rsyn::Cell cell = physicalCell.getInstance().asCell();

    this->physicalCell = physicalCell;

	Rsyn::PhysicalLibraryCell physicalLibraryCell =
			session.getPhysicalDesign().getPhysicalLibraryCell(cell);

	setPos(QPointF(physicalCell.getX(), physicalCell.getY()));
    setZValue(Rsyn::GRAPHICS_LAYER_INSTANCE);

	if (physicalLibraryCell.hasLayerObstacles()) {
		for (const Bounds &obs : physicalLibraryCell.allLayerObstacles()) {
			QRect rect = QtUtils::convert(obs);
			QPainterPath shape;
			shape.addRect(rect);
			clsShape += shape;
		} // end for
	} else {
		Bounds bounds(DBUxy(), physicalCell.getSize());
		QRect rect = QtUtils::convert(bounds);
		QPainterPath shape;
		shape.addRect(rect);
		clsShape += shape;
	} // end else
	clsShape = clsShape.simplified();
} // end constructor

// -----------------------------------------------------------------------------

QRectF MacroGraphicsItem::boundingRect() const {
    return QRectF(0, 0,
			physicalCell.getWidth(), physicalCell.getHeight());
} // end method

// -----------------------------------------------------------------------------

QPainterPath
MacroGraphicsItem::shape() const {
    return clsShape;
} // end method

// -----------------------------------------------------------------------------

void
MacroGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	Q_UNUSED(widget);

	QBrush brush;
	brush.setColor(Qt::lightGray);
	brush.setStyle(Qt::SolidPattern);
	painter->setBrush(brush);

	QPen pen;
	painter->setPen(pen);

	painter->drawPath(clsShape);

	CellGraphicsItem::paint(painter, option, widget);
} // end method

// -----------------------------------------------------------------------------

} // end namespace