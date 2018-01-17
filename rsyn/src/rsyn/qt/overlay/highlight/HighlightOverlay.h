#ifndef RSYN_QT_HIGHLIGHT_OVERLAY_H
#define RSYN_QT_HIGHLIGHT_OVERLAY_H

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

class HighlightOverlay : public GraphicsOverlay,  public GraphicsObserver {
public:
	
    HighlightOverlay(QGraphicsItem *parent);
	~HighlightOverlay();

	virtual bool init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) override;

	virtual void onChangeVisibility(const std::string &key, const bool visible) override;
	virtual void onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) override;

	virtual QRectF boundingRect() const override {return bounds;}

	//! @note Returns an empty shape so that the mouse never intersects with the
	//! overlay.
	virtual QPainterPath shape() const override {return QPainterPath();}
	
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

	Rsyn::Pin getHoverPin() const {return clsHoverPin;}

	void setHoverOutline(const QPainterPath &outline) {clsHoverOutline = outline; clsHovering = true;}
	void setHoverPin(Rsyn::Pin pin) {clsHoverPin = pin;}

	void clearHover() {
		clsHovering = false;
		clsHoverPin = nullptr;
	} // end method

private:

	QRectF bounds;

	bool clsHovering = false;
	QPainterPath clsHoverOutline;
	Rsyn::Pin clsHoverPin;

}; // end class

} // end namespace

#endif

