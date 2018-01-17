#ifndef RSYN_QT_FLOORPLAN_OVERLAY_H
#define RSYN_QT_FLOORPLAN_OVERLAY_H

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

class GraphicsView;
class TrackOverlay;

class FloorplanOverlay : public GraphicsOverlay, public GraphicsObserver {
public:
	
    FloorplanOverlay(QGraphicsItem *parent);
	~FloorplanOverlay();

	virtual bool init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) override;
	
	virtual void onChangeVisibility(const std::string &key, const bool visible) override;

    virtual QRectF boundingRect() const override {return bounds;}
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

private:

	QRectF bounds;
	TrackOverlay *clsTrackOverlay = nullptr;

}; // end class

} // end namespace

#endif

