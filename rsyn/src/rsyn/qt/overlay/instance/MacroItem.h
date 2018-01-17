#ifndef RSYN_QT_MACROBLOCK_GRAPHICS_ITEM_H
#define RSYN_QT_MACROBLOCK_GRAPHICS_ITEM_H

#include "rsyn/phy/PhysicalDesign.h"
#include "CellItem.h"

#include <QColor>
#include <QGraphicsItem>

namespace Rsyn {

class Graphics;

class MacroGraphicsItem : public CellGraphicsItem {
public:
    MacroGraphicsItem(Rsyn::PhysicalCell physicalCell);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

protected:

	QPainterPath clsShape;
}; // end class

} // end namespace

#endif

