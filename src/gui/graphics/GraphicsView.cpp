/* Copyright 2014-2018 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "GraphicsScene.h"
#include "GraphicsView.h"

#include "gui/objects/StandardCell.h"
#include "gui/objects/Wire.h"

#include "gui/QtUtils.h"
#include "gui/objects/PinMgr.h"

#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "util/Stepwatch.h"
#include "GraphicsItem.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QEvent>

#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif

#include <iostream>

// -----------------------------------------------------------------------------

namespace Rsyn {

static const bool DebugRendering = false;

// -----------------------------------------------------------------------------

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent),
      zoomScaling(1),
      sceneToViewportScalingFactor(0),
      rowHeight(0),
      initialized(false) {
        setRenderHint(QPainter::Antialiasing, false);
        setDragMode(QGraphicsView::RubberBandDrag);
        setOptimizationFlags(QGraphicsView::DontSavePainterState |
                             QGraphicsView::DontAdjustForAntialiasing);
        setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        setCacheMode(QGraphicsView::CacheBackground);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        if (DebugRendering) {
                setOptimizationFlag(QGraphicsView::IndirectPainting, true);
        }  // end if

        updateViewMatrix();
}  // end method

// -----------------------------------------------------------------------------

GraphicsView::~GraphicsView() {}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::init() {
        updateAdjustedSceneRect();
        resetView();

        Rsyn::Session session;
        rsynGraphics = session.getService("rsyn.graphics");

        physicalDesign = session.getPhysicalDesign();
        rowHeight = physicalDesign.getRowHeight();

        initialized = true;
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::updateAdjustedSceneRect() {
        const QRectF &sceneRect = scene()->sceneRect();
        adjustedSceneRect = sceneRect;

        // const qreal gap = std::min(sceneRect.width(), sceneRect.height()) *
        // 0.01;
        // adjustedSceneRect.adjust(-gap, -gap, +gap, +gap);

        // Adjust scene view to match viewport dimensions.
        const qreal w = width();
        const qreal W = adjustedSceneRect.width();
        const qreal h = height();
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
        }  // end else

        // std::cout << "w: " << (sceneToViewportScalingFactor *
        // adjustedSceneRect.width()) << " = " << width() << "\n";
        // std::cout << "h: " << (sceneToViewportScalingFactor *
        // adjustedSceneRect.height()) << " = " << height() << "\n";
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::updateViewMatrix() {
        const bool invertY = true;
        const qreal scaling = sceneToViewportScalingFactor * getZoom();

        QMatrix mtx;
        if (invertY) {
                mtx.translate(0, viewport()->size().height());
                mtx.scale(scaling, -scaling);
        } else {
                mtx.scale(scaling, scaling);
        }  // end else
        setMatrix(mtx);
        invertedTransform = transform().inverted();
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::resetView() {
        zoomScaling = initialZoomScaling;
        updateViewMatrix();
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::togglePointerMode(const bool enable) {
        setDragMode(enable ? QGraphicsView::RubberBandDrag
                           : QGraphicsView::ScrollHandDrag);
        setInteractive(enable);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::toggleOpenGL(const bool enable) {
#ifndef QT_NO_OPENGL
        setViewport(enable ? new QGLWidget(QGLFormat(QGL::SampleBuffers))
                           : new QWidget);
#endif
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::toggleAntialiasing(const bool enable) {
        setRenderHint(QPainter::Antialiasing, enable);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::print() {
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
        QPrinter printer;
        QPrintDialog dialog(&printer, this);
        if (dialog.exec() == QDialog::Accepted) {
                QPainter painter(&printer);
                render(&painter);
        }
#endif
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::zoomIn(int level) {
        zoomScaling *= std::pow(2, level);
        updateViewMatrix();
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::zoomOut(int level) {
        zoomScaling /= std::pow(2, level);
        updateViewMatrix();
}  // end method

// -----------------------------------------------------------------------------

GraphicsScene *GraphicsView::getScene() const {
        return (GraphicsScene *)scene();
}  // end method

// -----------------------------------------------------------------------------

qreal GraphicsView::getZoom() const { return zoomScaling; }  // end method

// -----------------------------------------------------------------------------

qreal GraphicsView::getSceneWidth() const {
        return adjustedSceneRect.width() / getZoom();
}  // end method

// -----------------------------------------------------------------------------

qreal GraphicsView::getSceneHeight() const {
        return adjustedSceneRect.height() / getZoom();
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::resizeEvent(QResizeEvent *event) {
        if (!isInitialized()) return;

        setUpdatesEnabled(false);
        updateAdjustedSceneRect();
        updateViewMatrix();
        setUpdatesEnabled(true);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::drawBackground(QPainter *painter, const QRectF &rect) {
        QGraphicsView::drawBackground(painter, rect);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::drawForeground(QPainter *painter, const QRectF &rect) {
        QGraphicsView::drawForeground(painter, rect);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::drawItems(QPainter *painter, int numItems,
                             QGraphicsItem *items[],
                             const QStyleOptionGraphicsItem options[]) {
        Stepwatch watch("Debug rendering #items=" + std::to_string(numItems));
        QGraphicsView::drawItems(painter, numItems, items, options);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::paintEvent(QPaintEvent *event) {
        {
                // Stepwatch watch("Rendering");
                QGraphicsView::paintEvent(event);
        }

        {
                // Stepwatch watch("Caching");
                // updateRenderingCache();
        }
}  // end method

// -----------------------------------------------------------------------------

#ifndef QT_NO_WHEELEVENT
void GraphicsView::wheelEvent(QWheelEvent *e) {
        if (e->modifiers() & Qt::ControlModifier) {
                if (e->delta() > 0)
                        zoomIn(1);
                else
                        zoomOut(1);
                e->accept();
        } else {
                QGraphicsView::wheelEvent(e);
        }  // end else
}  // end method
#endif

// -----------------------------------------------------------------------------

void GraphicsView::mousePressEvent(QMouseEvent *event) {
        QGraphicsView::mousePressEvent(event);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
        QGraphicsView::mouseMoveEvent(event);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
        QGraphicsView::mouseReleaseEvent(event);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
        }  // end if
        QGraphicsView::mouseDoubleClickEvent(event);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::enterEvent(QEvent *event) {
        QGraphicsView::enterEvent(event);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsView::leaveEvent(QEvent *event) {
        if (getScene()) {
                getScene()->notifyMouseLeaveEvent(this);
        }  // end if
        QGraphicsView::leaveEvent(event);
}  // end method

// -----------------------------------------------------------------------------

float GraphicsView::getNumExposedRows() const {
        return std::max(getSceneWidth(), getSceneHeight()) / rowHeight;
}  // end method

// -----------------------------------------------------------------------------

QBrush GraphicsView::getStippleBrush(const FillStippleMask &mask,
                                     const qreal dx, const qreal dy) const {
        GraphicsStippleMgr *mgr = GraphicsStippleMgr::get();
        QBrush brush = mgr->getBrush(mask);
        QTransform transform = invertedTransform;
        transform.translate(-dx, -dy);
        brush.setTransform(transform);
        return brush;
}  // end method

// -----------------------------------------------------------------------------

GraphicsItem *GraphicsView::getItemAt(const int x, const int y) const {
        GraphicsScene *graphicsScene = getScene();
        if (graphicsScene) {
                QPointF mappedPos = mapToScene(x, y);
                return graphicsScene->getItemAt(mappedPos.x(), mappedPos.y());
        } else {
                return nullptr;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
