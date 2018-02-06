#ifndef RSYN_EXAMPLE_OVERLAY_H
#define RSYN_EXAMPLE_OVERLAY_H

#include "rsyn/qt/graphics/view/layout/LayoutGraphicsLayer.h"

#include <Rsyn/PhysicalDesign>

#include <QColor>
#include <QGraphicsItem>
#include <QLine>

namespace Rsyn {
class LayoutGraphicsScene;
} // end namespace

namespace RsynExample {

class ExampleOverlay : public Rsyn::LayoutGraphicsLayer {
public:

    ExampleOverlay();
	~ExampleOverlay();

	virtual bool init(Rsyn::LayoutGraphicsScene *scene, std::vector<Rsyn::GraphicsLayerDescriptor> &visibilityItems) override;

	virtual void render(QPainter *painter, const float lod, const QRectF &exposedRect) override;

private:

	QRectF clsCoreBounds;

}; // end class

} // end namespace

#endif