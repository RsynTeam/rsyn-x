#ifndef RSYN_QT_INSTANCE_OVERLAY_H
#define RSYN_QT_INSTANCE_OVERLAY_H

#include "rsyn/qt/infra/GraphicsOverlay.h"
#include "rsyn/qt/GraphicsObserver.h"

#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"
#include "rsyn/util/Stipple.h"

#include <QColor>
#include <QGraphicsItem>

#include <array>
#include <vector>
#include <map>

namespace Rsyn {

class GraphicsView;

class InstanceOverlay : public GraphicsOverlay, public GraphicsObserver {
public:
	
    InstanceOverlay(QGraphicsItem *parent);
	~InstanceOverlay();

	virtual bool init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) override;

	virtual void onChangeVisibility(const std::string &key, const bool visible) override;

	QGraphicsItem *getItem(Rsyn::Instance instance) const {
		return clsInstanceMap[instance];
	} // end method

private:

	GraphicsView *clsView = nullptr;
	Rsyn::Attribute<Rsyn::Instance, QGraphicsItem *> clsInstanceMap;

}; // end class

} // end namespace

#endif

