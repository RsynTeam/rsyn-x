#ifndef RSYN_QT_TRACK_LAYER_H
#define RSYN_QT_TRACK_LAYER_H

#include "rsyn/qt/infra/GraphicsOverlay.h"
#include "rsyn/qt/GraphicsObserver.h"

#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"
#include "rsyn/util/Stipple.h"

#include <QColor>
#include <QGraphicsItem>
#include <QLine>

#include <array>
#include <vector>
#include <map>

namespace Rsyn {

class Graphics;
class GraphicsView;

class TrackOverlay : public GraphicsOverlay, public GraphicsObserver {
public:
	
    TrackOverlay(QGraphicsItem *parent);
	~TrackOverlay();

	virtual bool init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) override;

	virtual void onChangeVisibility(const std::string &key, const bool visible) override;
	virtual void onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) override;

    virtual QRectF boundingRect() const override {return bounds;}
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

private:

	GraphicsView *view = nullptr;
	Graphics *rsynGraphics = nullptr;
	bool preferredDirectionVisibility = true;
	bool nonPreferredDirectionVisibility = false;
	QRectF bounds;
	Rsyn::PhysicalDesign physicalDesign = nullptr;

}; // end class

} // end namespace

#endif

