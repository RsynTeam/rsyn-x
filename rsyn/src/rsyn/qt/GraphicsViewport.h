#ifndef RSYN_QT_GRAPHICS_VIEWPORT_H
#define RSYN_QT_GRAPHICS_VIEWPORT_H

#include <QFrame>
#include <QGraphicsView>

#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/util/Stipple.h"

#include "GraphicsView.h"

namespace Rsyn {

//! @todo Unify GraphicsView and GraphicsViewport. GraphicsViewport should be
//! eliminated.

class GraphicsViewport : public QFrame {
    Q_OBJECT
public:
    explicit GraphicsViewport(const QString &name, QWidget *parent = 0);

    GraphicsView *getView() const {
		return graphicsView;
	} // end method

	void init();

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

public slots:

	virtual void resizeEvent(QResizeEvent * event) override;

private:

	void updateAdjustedSceneRect();
	void updateViewMatrix();

	bool initialized = false;
    GraphicsView *graphicsView = nullptr;

	qreal zoomScaling;

	// The bounds of the scene (user space) adjusted to the aspect ratio of the
	// viewport. When zoom = 1, it means the adjustedSceneRect fits exactly
	// into the viewport.
	QRectF adjustedSceneRect;

	// The scaling required to map user coordinates to viewport coordinates.
	qreal sceneToViewportScalingFactor;
	qreal rowHeight;

}; // end class

// -----------------------------------------------------------------------------

} // end namespace

#endif 
