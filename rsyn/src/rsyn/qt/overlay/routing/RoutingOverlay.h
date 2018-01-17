#ifndef RSYN_QT_ROUTING_OVERLAY_H
#define RSYN_QT_ROUTING_OVERLAY_H

#include "rsyn/qt/infra/GraphicsOverlay.h"
#include "rsyn/qt/GraphicsObserver.h"

#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"
#include "rsyn/io/Graphics.h"

#include <QColor>
#include <QGraphicsItem>

#include <array>
#include <vector>
#include <map>

namespace Rsyn {

class GraphicsView;

class RoutingOverlay : public GraphicsOverlay, public GraphicsObserver {
public:
    RoutingOverlay(QGraphicsItem *parent);
	~RoutingOverlay();

	virtual bool init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) override;

	virtual void onChangeVisibility(const std::string &key, const bool visible) override;
	virtual void onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) override;
private:

	std::vector<GraphicsOverlay *> layers;
	std::map<std::string, int> mapLayers;
}; // end class

} // end namespace

#endif

