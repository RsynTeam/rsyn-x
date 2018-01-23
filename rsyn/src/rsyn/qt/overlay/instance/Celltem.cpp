
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/session/Session.h"
#include "rsyn/io/Graphics.h"

#include "rsyn/qt/PropertyDialog.h"
#include "rsyn/qt/GraphicsView.h"
#include "rsyn/qt/QtUtils.h"

#include "CellItem.h"
#include "rsyn/qt/infra/PinMgr.h"

#include <QGraphicsSceneMouseEvent>
#include <QMenu>

namespace Rsyn {

CellGraphicsItem::CellGraphicsItem(Rsyn::PhysicalCell physicalCell) {
	Rsyn::Session session;

    this->physicalCell = physicalCell;
	this->rsynGraphics = session.getService("rsyn.graphics");

	setPos(QPointF(physicalCell.getX(), physicalCell.getY()));
    setZValue(Rsyn::GRAPHICS_LAYER_INSTANCE);

    setFlags(ItemIsSelectable /*| ItemIsMovable*/);
    setAcceptHoverEvents(false); // hover events handled by the view
} // end constructor

// -----------------------------------------------------------------------------

QRectF CellGraphicsItem::boundingRect() const {
    return QRectF(0, 0,
			physicalCell.getWidth(), physicalCell.getHeight());
} // end method

// -----------------------------------------------------------------------------

QPainterPath
CellGraphicsItem::shape() const {
    QPainterPath path;
    path.addRect(0, 0,
			physicalCell.getWidth(), physicalCell.getHeight());
    return path;
} // end method

// -----------------------------------------------------------------------------

void
CellGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	Q_UNUSED(widget);

	// TODO: Need a more efficient way to do this.
	const GraphicsView *view = (GraphicsView *) scene()->views().first();
	const qreal lod = view->getNumExposedRows();

	// Draw cell.
	QPen pen;
	if (option->state & QStyle::State_Selected) {
		pen.setWidth(2);
		pen.setColor(QtUtils::convert(
				rsynGraphics->getSelectionRendering().getColor()));
	} else if (option->state & QStyle::State_MouseOver) {
		pen.setWidth(2);
		pen.setColor(QtUtils::convert(
				rsynGraphics->getHighlightRendering().getColor()));
		pen.setStyle(Qt::DashLine);
	} else {
		pen.setWidth(0);
		pen.setColor(QtUtils::convert(
				rsynGraphics->getInstanceRendering().getColor()));
	} // end else
	pen.setCosmetic(true);

	QBrush brush;
	painter->setBrush(brush);

	painter->setPen(pen);
	painter->drawRect(0, 0,
			physicalCell.getWidth(), physicalCell.getHeight());

	// Draw pins
	if (lod <= 20) {
		Rsyn::LibraryCell lcell = physicalCell.getInstance().asCell().getLibraryCell();
		for (Rsyn::LibraryPin lpin : lcell.allLibraryPins()) {
			painter->drawPath(QtPinMgr::get()->getShape(lpin, physicalCell.getOrientation()));
		} // end for
	} // end if

    // Draw text
    if (lod <= 10) {
		const int fontSize = 12;
        QFont font("Times", fontSize);
        //font.setStyleStrategy(QFont::ForceOutline);
        painter->setFont(font);

		const double scaleValue = scale()/painter->transform().m11();
		painter->save();
		painter->translate(0, physicalCell.getHeight());
		painter->scale(scaleValue, -scaleValue);
		painter->setPen(QColor(0, 0, 0));
	    painter->drawText(fontSize * 0.5, fontSize * 1.5, QString(QString::fromStdString(
				physicalCell.getInstance().getName())));
		painter->restore();
    } // end if
} // end method

// -----------------------------------------------------------------------------

void
CellGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->modifiers() & Qt::ShiftModifier) {
        update();
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
CellGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	if (event->button() & Qt::LeftButton) {
		QGraphicsItem::mousePressEvent(event);
		update();
	}
} // end method

// -----------------------------------------------------------------------------

void
CellGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
    update();
} // end method

// -----------------------------------------------------------------------------

void
CellGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		showProperties();
	} // end if
	QGraphicsItem::mouseDoubleClickEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
CellGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    menu.addAction("Properties");
    QAction *selectedAction = menu.exec(event->screenPos());

	if (selectedAction) {
		if (selectedAction->text() == "Properties") {
			showProperties();
		} // end if
	} // end if

    //QGraphicsItem::contextMenuEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
CellGraphicsItem::showProperties() {
	PropertyDialog *dialog = new PropertyDialog;

	QtUtils::QTreeDescriptor tree;
	tree.add("Name", 1,
			physicalCell.getInstance().getName());
	tree.add("Placement/X", 1,
			std::to_string(physicalCell.getX()));
	tree.add("Placement/Y", 1,
			std::to_string(physicalCell.getY()));
	tree.add("Dimension/Width", 1,
			std::to_string(physicalCell.getWidth()));
	tree.add("Dimension/Height", 1,
			std::to_string(physicalCell.getHeight()));

	dialog->setProperties(tree);
	dialog->show();
} // end method

// -----------------------------------------------------------------------------

} // end namespace
