#ifndef RSYN_QT_ROUTING_GUIDE_OVERLAY_H
#define RSYN_QT_ROUTING_GUIDE_OVERLAY_H

#include "rsyn/qt/infra/GraphicsOverlay.h"
#include "rsyn/qt/GraphicsObserver.h"

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"
#include "rsyn/util/Stipple.h"
#include "rsyn/ispd18/RoutingGuide.h"
#include "rsyn/io/Graphics.h"
#include "rsyn/qt/overlay/routingGuide/RoutingGuideItem.h"

#include <QColor>
#include <QGraphicsItem>
#include <QLine>

#include <array>
#include <vector>
#include <map>

namespace Rsyn {

class RoutingGuideOverlay : public GraphicsOverlay, public GraphicsObserver {
public:
	RoutingGuideOverlay(QGraphicsItem *parent);
	
	virtual bool init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) override;

	virtual void onChangeVisibility(const std::string &key, const bool visible) override;
	virtual void onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) override;

private:
	Rsyn::Session session;
	Rsyn::Design design;
	Rsyn::Module module;
	
	Rsyn::RoutingGuide* routingGuide;
	Rsyn::Graphics *rsynGraphics;	
	
	Rsyn::Attribute<Rsyn::Net, std::vector<QGraphicsItem*>> routingGuideItems;
	void processGlobalGuides();
	void setGuideVisible(const Net net, const bool visible);
	void setAll(const bool visible);
}; // end class

} // end namespace

#endif /* ROUTINGGUIDEOVERLAY_H */

