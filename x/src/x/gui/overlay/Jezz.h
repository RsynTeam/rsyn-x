#ifndef JEZZ_OVERLAY_H
#define JEZZ_OVERLAY_H

#include "rsyn/gui/canvas/PhysicalCanvasGL.h"

class Jezz;

class JezzOverlay : public CanvasOverlay {
private:
	
	Jezz * jezz;

	void renderNodes(PhysicalCanvasGL * canvas);
	void renderRows(PhysicalCanvasGL * canvas);
	
public:
	JezzOverlay();	
	
	bool init(PhysicalCanvasGL* canvas, nlohmann::json& properties) override;
	virtual void render(PhysicalCanvasGL * canvas);
	virtual void config(const nlohmann::json &params);
}; // end class

#endif