#ifndef RSYN_QT_GRAPHICS_VIEW_H
#define RSYN_QT_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QPixmap>

#include "rsyn/io/Graphics.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/util/Stipple.h"

#include "GraphicsStipple.h"
#include "GraphicsObserver.h"

class QStatusBar;

namespace Rsyn {

class GraphicsViewport;
class HighlightOverlay; // **temp**

class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = 0);
	~GraphicsView();

	void init();

	virtual void drawBackground(QPainter *painter, const QRectF &rect) override;
    virtual void drawForeground(QPainter *painter, const QRectF &rect) override;
    virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[]) override;

	virtual void paintEvent(QPaintEvent *event) override;

	qreal getNumVisibleRowsInViewport() const;

	qreal getZoom() const;
	qreal getSceneWidth() const;
	qreal getSceneHeight() const;

    void zoomIn(int level = 1);
    void zoomOut(int level = 1);

	void resetView();
    void togglePointerMode(const bool enable);
    void toggleOpenGL(const bool enable);
    void toggleAntialiasing(const bool enable);
    void print();

	bool isInitialized() const {return initialized;}

	//! @brief Returns the level of detail (lod), in number of physical rows, to
	//! be used to draw objects. An lod = 10, means that current 10 rows fit in
	//! the viewport.
	float getNumExposedRows() const;

	//! @brief Gets the current visibility of an object.
	bool getVisibility(const std::string &key) const {
		auto it = clsVisibility.find(key);
		return it != clsVisibility.end()? it->second : false;
	} // end method

	//! @brief Gets the current visibility of a physical layer.
	bool getPhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer) const {
		auto it = clsPhysicalLayerVisibility.find(layer);
		return it != clsPhysicalLayerVisibility.end()? it->second : false;
	} // end method

	//! @brief Sets the visibility of an object.
	void setVisibility(const std::string &key, const bool visible);

	//! @brief Sets the visibility of a physical layer.
	void setPhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible);

	//! @brief Sets the visibility of a physical layer.
	void setPhysicalLayerVisibility(const std::string &layerName, const bool visible);

	//! @brief Sets the status bar on which to display information.
	void setStatusBar(QStatusBar *bar) {statusBar = bar;}

	//! @brief Gets a brush respective to a fill pattern. You need to provide
	//! the position, (dx, dy), of the object that will be draw, in order to
	//! make the fill pattern invariant with the object's position.
	QBrush getStippleBrush(const FillStippleMask &mask, const qreal dx, const qreal dy) const;

	//! @brief Register an observer to get notified about graphics events.
	void registerObserver(GraphicsObserver *observer) {
		clsObservers.push_back(observer);
		observer->graphicsView = this;
	} // end method

	//! @brief Unregister an observer.
	void unregisterObserver(GraphicsObserver *observer) {
		clsObservers.remove(observer);
		observer->graphicsView = nullptr;
	} // end method

	//! @todo move to GraphicsScene
	FillStippleMask getRoutingLayerStipple(const int routingLayerId) const {
		return rsynGraphics->getRoutingLayerRendering(routingLayerId).getFillPattern();
	} // end method

	//! @todo move to GraphicsScene
	QColor getRoutingLayerColor(const int routingLayerId) const  {
		const Color &c = rsynGraphics->getRoutingLayerRendering(routingLayerId).getColor();
		return QColor(c.r, c.g, c.b);
	} // end method

	// temp
	void setHighlightOverlay(HighlightOverlay *hightlighOverlay) {clsHighlightOverlay = hightlighOverlay;}

protected:
	
#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent(QWheelEvent *) override;
#endif

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent * e) override;

	virtual void enterEvent(QEvent *event) override;
	virtual void leaveEvent(QEvent *event) override;

	virtual void resizeEvent(QResizeEvent * event) override;
private:

	QPainterPath traceNet(Rsyn::Net net, const bool includePins);

	void updateAdjustedSceneRect();

	//! @brief Updates the transformation matrix of the view.
	void updateViewMatrix();

	//! @brief Updates the rendering cache.
	void updateRenderingCache();

	int rederingCacheSign = 0;

	//! @brief Indicates whether or not this graphics view was already
	//! initialized.
	bool initialized = false;

	// Cached rendering.
	QPixmap cachedRendering;

	// Current zoom.
	qreal zoomScaling;

	// The bounds of the scene (user space) adjusted to the aspect ratio of the
	// viewport. When zoom = 1, it means the adjustedSceneRect fits exactly
	// into the viewport.
	QRectF adjustedSceneRect;

	// The scaling required to map user coordinates to viewport coordinates.
	qreal sceneToViewportScalingFactor;
	qreal rowHeight;

	QTransform invertedTransform;

	std::map<std::string, bool> clsVisibility;
	std::map<Rsyn::PhysicalLayer, bool> clsPhysicalLayerVisibility;

	std::list<GraphicsObserver *> clsObservers;

	HighlightOverlay *clsHighlightOverlay = nullptr;
	QGraphicsItem *clsPreviousHoverItem = nullptr;

	Rsyn::PhysicalDesign physicalDesign;
	Rsyn::Graphics *rsynGraphics = nullptr;

	QStatusBar *statusBar = nullptr;

}; // end method

// -----------------------------------------------------------------------------

} // end namespace

#endif 
