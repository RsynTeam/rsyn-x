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

#ifndef RSYN_QT_GRAPHICS_VIEW_H
#define RSYN_QT_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QPixmap>

#include "io/Graphics.h"
#include "phy/PhysicalDesign.h"
#include "util/Stipple.h"

#include "GraphicsStipple.h"

class QStatusBar;

namespace Rsyn {

class GraphicsScene;
class GraphicsLayer;
class GraphicsItem;

class GraphicsView : public QGraphicsView {
        Q_OBJECT
       public:
        GraphicsView(QWidget *parent = 0);
        ~GraphicsView();

        void init();

        virtual void drawBackground(QPainter *painter,
                                    const QRectF &rect) override;
        virtual void drawForeground(QPainter *painter,
                                    const QRectF &rect) override;
        virtual void drawItems(
            QPainter *painter, int numItems, QGraphicsItem *items[],
            const QStyleOptionGraphicsItem options[]) override;

        virtual void paintEvent(QPaintEvent *event) override;

        GraphicsScene *getScene() const;

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

        bool isInitialized() const { return initialized; }

        //! @brief Returns the level of detail (lod), in number of physical
        //! rows, to
        //! be used to draw objects. An lod = 10, means that current 10 rows fit
        //! in
        //! the viewport.
        float getNumExposedRows() const;

        //! @brief Gets a brush respective to a fill pattern. You need to
        //! provide
        //! the position, (dx, dy), of the object that will be draw, in order to
        //! make the fill pattern invariant with the object's position.
        QBrush getStippleBrush(const FillStippleMask &mask, const qreal dx,
                               const qreal dy) const;

        //! @brief Returns the top most item at position (x, y) in view
        //! coordinates.
        GraphicsItem *getItemAt(const int x, const int y) const;

        //! @todo move to GraphicsScene
        FillStippleMask getRoutingLayerStipple(const int routingLayerId) const {
                return rsynGraphics->getRoutingLayerRendering(routingLayerId)
                    .getFillPattern();
        }  // end method

        //! @todo move to GraphicsScene
        QColor getRoutingLayerColor(const int routingLayerId) const {
                const Color &c =
                    rsynGraphics->getRoutingLayerRendering(routingLayerId)
                        .getColor();
                return QColor(c.r, c.g, c.b);
        }  // end method

       protected:
#ifndef QT_NO_WHEELEVENT
        virtual void wheelEvent(QWheelEvent *) override;
#endif

        virtual void mousePressEvent(QMouseEvent *event) override;
        virtual void mouseMoveEvent(QMouseEvent *event) override;
        virtual void mouseReleaseEvent(QMouseEvent *event) override;
        virtual void mouseDoubleClickEvent(QMouseEvent *e) override;

        virtual void enterEvent(QEvent *event) override;
        virtual void leaveEvent(QEvent *event) override;

        virtual void resizeEvent(QResizeEvent *event) override;

       private:
        void updateAdjustedSceneRect();

        //! @brief Updates the transformation matrix of the view.
        void updateViewMatrix();

        //! @brief Indicates whether or not this graphics view was already
        //! initialized.
        bool initialized = false;

        // Current zoom.
        qreal zoomScaling;
        qreal initialZoomScaling = 1;

        // The bounds of the scene (user space) adjusted to the aspect ratio of
        // the
        // viewport. When zoom = 1, it means the adjustedSceneRect fits exactly
        // into the viewport.
        QRectF adjustedSceneRect;

        // The scaling required to map user coordinates to viewport coordinates.
        qreal sceneToViewportScalingFactor;
        qreal rowHeight;

        QTransform invertedTransform;

        Rsyn::PhysicalDesign physicalDesign;
        Rsyn::Graphics *rsynGraphics = nullptr;

};  // end method

// -----------------------------------------------------------------------------

}  // end namespace

#endif
