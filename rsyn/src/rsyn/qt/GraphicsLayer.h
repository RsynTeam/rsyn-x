#ifndef RSYN_GRAPHICS_LAYER_H
#define RSYN_GRAPHICS_LAYER_H

#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QBrush>
#include <QPen>

#include <tuple>
#include <map>
#include <vector>
#include <deque>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

namespace Rsyn {

class GraphicsScene;
class GraphicsItem;

class GraphicsLayer : public QGraphicsItem {
friend class GraphicsScene;
public:

	GraphicsLayer();
	~GraphicsLayer();

    virtual QRectF boundingRect() const final;
    virtual QPainterPath shape() const final;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) final;

	void render(QPainter *painter, const QRectF &exposedRect);

	//! @brief Adds an item to this graphics layer. The graphics layer takes
	//! ownership of the item (i.e. it is responsible to delete it).
	void addItem(GraphicsItem *item) {
		clsItems.push_back(item);
	} // end method

	//! @brief
	void setBrush(const QBrush &brush) {
		clsBrush = brush;
	} // end method

	//! @brief
	void setPen(const QPen &pen) {
		clsPen = pen;
	} // end method

private:

	GraphicsScene *clsScene = nullptr;
	std::deque<GraphicsItem *> clsItems;

	QBrush clsBrush;
	QPen clsPen;
}; // end clas

} // end namespae

#endif

