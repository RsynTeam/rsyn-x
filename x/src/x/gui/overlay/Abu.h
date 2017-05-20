#ifndef ABU_OVERLAY_H
#define ABU_OVERLAY_H

#include "rsyn/gui/canvas/PhysicalCanvasGL.h"

namespace Rsyn {
class Engine;
} // end namespace

namespace ICCAD15 {
class Infrastructure;
} // end namespace

class AbuOverlay : public CanvasOverlay {
private:
	
	Rsyn::Engine clsEnginePtr;
	ICCAD15::Infrastructure * infra;
	
	bool clsViewUtilization;
	bool clsViewViolations;	

	void renderGrid(PhysicalCanvasGL * canvas);
	void renderUtilization(PhysicalCanvasGL * canvas);
	void renderViolations(PhysicalCanvasGL * canvas);	
	
public:
	
	AbuOverlay();	

	bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties) override;
	virtual void render(PhysicalCanvasGL * canvas);
	virtual void config(const nlohmann::json &params);
}; // end class

#endif