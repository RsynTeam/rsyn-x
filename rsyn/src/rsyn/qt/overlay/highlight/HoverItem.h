#ifndef RSYN_QT_GRAPHICS_HOVER_ITEM_H
#define RSYN_QT_GRAPHICS_HOVER_ITEM_H

#include <QGraphicsPathItem>
#include <QPainterPath>

namespace Rsyn {

class GraphicsHoverItem : public QGraphicsItem {
public:
		GraphicsHoverItem();

		virtual QRectF boundingRect() const override;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
		
		//! @note Returns an empty shape so that the mouse never intersects with the
		//! overlay.
		virtual QPainterPath shape() const override {return QPainterPath();}

		void setOutline(const QPainterPath &outline);

private:

	QPainterPath clsOutline;

}; // end class

} // end namespace


#endif 

