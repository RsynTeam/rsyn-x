#ifndef RSYN_QT_CELL_GRAPHICS_ITEM_H
#define RSYN_QT_CELL_GRAPHICS_ITEM_H

#include "rsyn/phy/PhysicalDesign.h"

#include <QColor>
#include <QGraphicsItem>

namespace Rsyn {

class Graphics;

class CellGraphicsItem : public QGraphicsItem {
public:
    CellGraphicsItem(Rsyn::PhysicalCell physicalCell);

    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

	Rsyn::PhysicalCell getPhysicalCell() const {return physicalCell;}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	void showProperties();

    Rsyn::PhysicalCell physicalCell;

	// TODO: Remove this reference.
    Rsyn::Graphics *rsynGraphics = nullptr;
};

} // end namespace

#endif

