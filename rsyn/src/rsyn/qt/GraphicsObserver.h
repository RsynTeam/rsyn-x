#ifndef RSYN_QT_GRAPHICS_OBSERVER_H
#define RSYN_QT_GRAPHICS_OBSERVER_H

#include "rsyn/phy/PhysicalDesign.h"

namespace Rsyn {
class GraphicsView;

class GraphicsObserver {
friend class GraphicsView;
public:

	virtual void onChangeVisibility(const std::string &key, const bool visible) {}
	virtual void onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) {}

	virtual ~GraphicsObserver();

private:
	GraphicsView *graphicsView = nullptr;
}; // end class
} // end namespace

#endif /* GRAPHICSSTIPPLE_H */

