#ifndef RSYN_QT_ROUTING_H
#define RSYN_QT_ROUTING_H

#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/util/Stipple.h"

#include <QColor>
#include <QGraphicsItem>

namespace Rsyn {

class RoutingGraphicsItem : public QGraphicsItem {
public:
    RoutingGraphicsItem(Rsyn::Net net, Rsyn::PhysicalWireSegment phWireSegment);
	RoutingGraphicsItem(Rsyn::Net net, const Rsyn::PhysicalRoutingWire &wire);

	Rsyn::Net getNet() const {return clsNet;}

    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
	Rsyn::Net clsNet;
	int clsRoutingLayerIndex = -1;
	QRectF bbox;
	QPolygonF poly;
}; // end class

} // end namespace

#endif /* ROUTING_H */

