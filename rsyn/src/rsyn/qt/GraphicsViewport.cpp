#include "GraphicsViewport.h"

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"

#include <QPrinter>
#include <QPrintDialog>
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif
#include <qmath.h>

#include <iostream>

// -----------------------------------------------------------------------------

namespace Rsyn {

GraphicsViewport::GraphicsViewport(const QString &name, QWidget *parent) :
		QFrame(parent),
		zoomScaling(1),
		sceneToViewportScalingFactor(0),
		rowHeight(0),
		initialized(false)
{
	setFrameStyle(0);

    graphicsView = new GraphicsView(this);
    graphicsView->setRenderHint(QPainter::Antialiasing, false);
    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setOptimizationFlags(
			QGraphicsView::DontSavePainterState |
			QGraphicsView::DontAdjustForAntialiasing);
	graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	graphicsView->setCacheMode(QGraphicsView::CacheBackground);

    graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(graphicsView, 0, 0);
    setLayout(topLayout);

    updateViewMatrix();
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::init() {
	updateAdjustedSceneRect();
	resetView();

	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
	rowHeight = physicalDesign.getRowHeight();

	graphicsView->init();
	initialized = true;
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::updateAdjustedSceneRect() {
	const QRectF &sceneRect = graphicsView->scene()->sceneRect();
	adjustedSceneRect = sceneRect;

	const qreal gap = std::min(sceneRect.width(), sceneRect.height()) * 0.01;
	adjustedSceneRect.adjust(-gap, -gap, +gap, +gap);

	// Adjust scene view to match viewport dimensions.
	const qreal w = graphicsView->width();
	const qreal W = adjustedSceneRect.width();
	const qreal h = graphicsView->height();
	const qreal H = adjustedSceneRect.height();

	const qreal scalingW = w / W;
	const qreal scalingH = h / H;
	sceneToViewportScalingFactor = std::min(scalingW, scalingH);

	if (scalingW < scalingH) {
		const qreal adjustedHeight = h / scalingW;
		const qreal d = (adjustedHeight - H) / 2;
		const qreal y0 = adjustedSceneRect.bottom();
		const qreal y1 = adjustedSceneRect.top();
		adjustedSceneRect.setBottom(y0 - d);
		adjustedSceneRect.setTop(y1 + d);
	} else {
		const qreal adjustedWidth = w / scalingH;
		const qreal d = (adjustedWidth - W) / 2;
		const qreal x0 = adjustedSceneRect.left();
		const qreal x1 = adjustedSceneRect.right();
		adjustedSceneRect.setLeft(x0 - d);
		adjustedSceneRect.setRight(x1 + d);
	} // end else

	//std::cout << "w: " << (sceneToViewportScalingFactor * adjustedSceneRect.width()) << " = " << graphicsView->width() << "\n";
	//std::cout << "h: " << (sceneToViewportScalingFactor * adjustedSceneRect.height()) << " = " << graphicsView->height() << "\n";
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::updateViewMatrix() {
    const qreal scaling = sceneToViewportScalingFactor*getZoom();

	QMatrix mtx;
	mtx.translate(0, rect().bottom());
	mtx.scale(scaling, -scaling);
	graphicsView->updateViewMatrix(mtx);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::resetView() {
	zoomScaling = 1;
    updateViewMatrix();

    graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::togglePointerMode(const bool enable) {
    graphicsView->setDragMode(enable
                              ? QGraphicsView::RubberBandDrag
                              : QGraphicsView::ScrollHandDrag);
    graphicsView->setInteractive(enable);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::toggleOpenGL(const bool enable) {
#ifndef QT_NO_OPENGL
    graphicsView->setViewport(enable ?
		new QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::toggleAntialiasing(const bool enable) {
    graphicsView->setRenderHint(QPainter::Antialiasing, enable);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::print() {
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        graphicsView->render(&painter);
    }
#endif
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::zoomIn(int level) {
    zoomScaling *= std::pow(2, level);
	updateViewMatrix();
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::zoomOut(int level) {
    zoomScaling /= std::pow(2, level);
	updateViewMatrix();
} // end method

// -----------------------------------------------------------------------------

qreal
GraphicsViewport::getZoom() const {
	return zoomScaling;
} // end method

// -----------------------------------------------------------------------------

qreal
GraphicsViewport::getSceneWidth() const {
	return adjustedSceneRect.width() / getZoom();
} // end method

// -----------------------------------------------------------------------------

qreal
GraphicsViewport::getSceneHeight() const {
	return adjustedSceneRect.height() / getZoom();
} // end method

// -----------------------------------------------------------------------------

qreal
GraphicsViewport::getNumVisibleRowsInViewport() const {
	return std::max(getSceneWidth(), getSceneHeight()) / rowHeight;
} // end method

// -----------------------------------------------------------------------------

void
GraphicsViewport::resizeEvent(QResizeEvent * event) {
	if (!isInitialized())
		return;

	graphicsView->setUpdatesEnabled(false);
	updateAdjustedSceneRect();
	updateViewMatrix();
	graphicsView->setUpdatesEnabled(true);
} // end method

} // end namespace