#ifndef RSYN_EXAMPLE_OVERLAY_H
#define RSYN_EXAMPLE_OVERLAY_H

#include "rsyn/qt/infra/GraphicsOverlay.h"
#include "rsyn/qt/GraphicsObserver.h"

#include "rsyn/phy/PhysicalDesign.h"

#include <QColor>
#include <QGraphicsItem>
#include <QLine>

#include <array>
#include <vector>
#include <map>

namespace Rsyn {
class GraphicsView;
} // end namespace

namespace RsynExample {

class ExampleOverlay : public Rsyn::GraphicsOverlay, public Rsyn::GraphicsObserver {
public:

    ExampleOverlay();
	~ExampleOverlay();

	virtual bool init(Rsyn::GraphicsView *view, std::vector<Rsyn::GraphicsLayerDescriptor> &visibilityItems) override;

	virtual void onChangeVisibility(const std::string &key, const bool visible) override;
	virtual void onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) override;

    virtual QRectF boundingRect() const override {return bounds;}
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

private:

	QRectF bounds;

}; // end class

} // end namespace

#endif