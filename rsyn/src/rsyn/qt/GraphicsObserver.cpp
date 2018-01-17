#include "GraphicsObserver.h"
#include "GraphicsView.h"

namespace Rsyn {
GraphicsObserver::~GraphicsObserver() {
	if (graphicsView)
		graphicsView->unregisterObserver(this);
} // end destructor
} // end namespace