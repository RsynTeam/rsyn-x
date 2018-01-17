#ifndef RSYN_QT_ROUTING_GUIDE_ITEM_H
#define RSYN_QT_ROUTING_GUIDE_ITEM_H

#include <QColor>
#include <QGraphicsItem>
#include "rsyn/util/Stipple.h"

namespace Rsyn {
	
class RoutingGuideItem : public QGraphicsItem {
public:
	RoutingGuideItem(const QRectF &bbox, const FillStippleMask &stipple, const QColor &color) :
			bbox(bbox), stipple(stipple), color(color) {};

	virtual QRectF boundingRect() const override {return bbox;}
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
private:
	FillStippleMask stipple;
    QColor color;
	QRectF bbox;
};

} // end namespace
#endif /* ROUTINGGUIDEITEM_H */

