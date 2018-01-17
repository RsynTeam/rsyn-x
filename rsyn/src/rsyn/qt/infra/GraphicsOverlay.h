#ifndef RSYN_QT_GRAPHICS_OVERLAY_H
#define RSYN_QT_GRAPHICS_OVERLAY_H

#include <string>
#include <vector>

#include <QGraphicsItem>

class QWidet;

namespace Rsyn {

class GraphicsView;

class GraphicsLayerDescriptor {
public:
	GraphicsLayerDescriptor();

	GraphicsLayerDescriptor(const std::string name, const bool visible = false) :
		clsName(name), clsVisible(visible) {}

	const std::string &getName() const {return clsName;}
	bool getVisible() const {return clsVisible;}

private:
	std::string clsName;
	bool clsVisible = false;
}; // end class

class GraphicsOverlay  : public QGraphicsItem {
public:

    GraphicsOverlay(const std::string &name, QGraphicsItem *parent) :
			QGraphicsItem(parent) {
		clsName = name;
	} // end constructor

	virtual bool init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) {return true;}

    virtual QRectF boundingRect() const override {return QRectF();}
	virtual QPainterPath shape() const override {return QPainterPath();}
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override {};

	const std::string &getName() const {return clsName;}
	GraphicsOverlay *getParent() const {return clsParent;}

	void addChild(GraphicsOverlay * child) {
		clsChildren.push_back(child);
		child->setParentItem(this);
		child->clsParent = this;
	} // end method

	const std::vector<GraphicsOverlay *> &
	allChildren() const {return clsChildren;}

private:

	std::string clsName;
	GraphicsOverlay *clsParent = nullptr;
	std::vector<GraphicsOverlay *> clsChildren;

}; // end class

} // end namespace

#endif

