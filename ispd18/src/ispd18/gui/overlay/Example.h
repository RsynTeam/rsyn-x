#ifndef PRIVATE_EXAMPLE_OVERLAY_H
#define PRIVATE_EXAMPLE_OVERLAY_H

#include "rsyn/gui/canvas/PhysicalCanvasGL.h"
#include "rsyn/core/infra/RangeBasedLoop.h"

class ExampleOverlay : public CanvasOverlay {
private:

	Rsyn::Module clsModule;
	Rsyn::PhysicalDesign clsPhysicalDesign;
	Rsyn::Design clsDesign;
	Rsyn::Timer * timer;
	
public:
	ExampleOverlay();

	bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties) override;
	void render(PhysicalCanvasGL * canvas);
	void config(const nlohmann::json &params);
}; // end class



#endif /* CRITICALPATH_H */

