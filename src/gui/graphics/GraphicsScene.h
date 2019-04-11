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

#ifndef RSYN_GRAPHICS_SCENE_H
#define RSYN_GRAPHICS_SCENE_H

#include <QGraphicsScene>
#include <QPainterPath>
#include <vector>
#include <tuple>

namespace Rsyn {

class GraphicsView;
class GraphicsLayer;
class GraphicsItem;

//! @brief This class implements similar functionalities as QGraphicsScene, but
//! with focus on handling hundred of thousand of elements.
//!
//! The main drawback of the current version of QGraphicsScene (Qt 5), which led
//! us to implement our own graphics system is that it does not implement
//! caching of the item layer. Only the background layer can be cached in the
//! current version of Qt. This implies that one need to redraw potentially a
//! several objects when a overlay is laid over the scene (for instance hover
//! outline of the item under the mouse).
//!
//! To work around this current limitation, the items are drawn in the
//! background of QGraphicsScene, which can be cached.

class GraphicsScene : public QGraphicsScene {
       public:
        GraphicsScene(QObject *parent = nullptr);
        ~GraphicsScene();

        virtual void drawBackground(QPainter *painter,
                                    const QRectF &rect) override;
        virtual void drawForeground(QPainter *painter,
                                    const QRectF &rect) override;

        //! @brief Update all the scene regions currently being viewed.
        void redraw(
            QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);

        //! @brief Update a specific a region of the scene. The region, which is
        //! given in scene coordinates, is first mapped to view coordinates and
        //! then
        //! expanded by the number of pixels defined by expandBy. This is useful
        //! when using cosmetic outlines (i.e. outline whose width is invariant
        //! to
        //! the view scaling) to avoid artifacts around the borders of shapes.
        void redraw(const QRectF &rect, QGraphicsScene::SceneLayers layers,
                    const int expandBy = 0);

        //! @brief Adds a graphics layer to this graphics scene. The graphics
        //! scene
        //! takes ownership of the graphics layer (i.e. it is responsible to
        //! delete
        //! it).
        void addLayer(GraphicsLayer *layer);

        //! @brief Returns the level-of-detail (lod) to be used to render the
        //! scene.
        //! The lod is defined as the value of of the typical length (default =
        //! 1)
        //! in viewport coordinates. In other words, if you are rendering to the
        //! screen, lod is the number of pixels covered by the typical length.
        float getLevelOfDetail(QPainter *painter) const;

        //! @brief Returns the typical length used in the level-of-detail
        //! calculation.
        float getTypicalLength() const {
                return clsTypicalLength;
        }  // end method

        //! @brief Sets the typical scene length. This is used as a scaling
        //! factor
        //! when defining the level-of-detail (
        void setTypicalLength(const float typicalLength) {
                clsTypicalLength = typicalLength;
        }  // end method

        //! @brief Sets the hover outline.
        void setHoverOutline(const QPainterPath &outline) {
                clsHover = true;
                redraw(clsHoverOutline.boundingRect(),
                       QGraphicsScene::ForegroundLayer,
                       clsHoverOutlineThickness);
                clsHoverOutline = outline;
                redraw(clsHoverOutline.boundingRect(),
                       QGraphicsScene::ForegroundLayer,
                       clsHoverOutlineThickness);
        }  // end method

        //! @brief Clears the hover outline.
        void clearHoverOutline() {
                if (clsHover) {
                        redraw(clsHoverOutline.boundingRect(),
                               QGraphicsScene::ForegroundLayer,
                               clsHoverOutlineThickness);
                }  // end if
                clsHover = false;
        }  // end method

        //! @brief Adds an outline to the highlight.
        void addHighlight(const QPainterPath &outline) {
                clsHighlightOutlines.push_back(outline);
                redraw(outline.boundingRect(), QGraphicsScene::BackgroundLayer,
                       clsHighligthOutlineThickness);
        }  // end method

        //! @brief Adds an outline to the highlight.
        void removeHighlight(const QPainterPath &outline) {
                clsHighlightOutlines.remove(outline);
                redraw(outline.boundingRect(), QGraphicsScene::BackgroundLayer,
                       clsHighligthOutlineThickness);
        }  // end method

        //! @brief Clears the highlight
        void clearHighlight() {
                for (const QPainterPath &outline : clsHighlightOutlines)
                        redraw(outline.boundingRect(),
                               QGraphicsScene::BackgroundLayer,
                               clsHighligthOutlineThickness);
                clsHighlightOutlines.clear();
        }  // end method

        //! @brief Adds an outline to the highlight.
        void addDrawing(const QPainterPath &outline) {
                clsDrawingOutlines.push_back(outline);
                redraw(outline.boundingRect(), QGraphicsScene::BackgroundLayer,
                       clsDrawingOutlineThickness);
        }  // end method

        //! @brief Adds an outline to the highlight.
        void removeDrawing(const QPainterPath &outline) {
                clsDrawingOutlines.remove(outline);
                redraw(outline.boundingRect(), QGraphicsScene::BackgroundLayer,
                       clsDrawingOutlineThickness);
        }  // end method

        //! @brief Clears the highlight
        void clearDrawing() {
                for (const QPainterPath &outline : clsDrawingOutlines)
                        redraw(outline.boundingRect(),
                               QGraphicsScene::BackgroundLayer,
                               clsDrawingOutlineThickness);
                clsDrawingOutlines.clear();
        }  // end method

        //! @brief Notify views to ensure visibility of a region.
        void ensureVisible(const QRectF &rect);

        //! @brief Returns the top most item at position (x, y).
        GraphicsItem *getItemAt(const qreal x, const qreal y) const;

        //! @brief Returns the top most item at the point.
        GraphicsItem *getItemAt(const QPointF &point) const;

        //! @brief Resort layers based on their z-order.
        void resortLayers();

        //! @brief Gets the current visibility of an object. The key can be
        //! hierarchical. For instance, "Parent/Child" may return false if
        //! either
        //! "Parent" or "Parent/Child" is set to false. By default, if no
        //! visibility is set for the key, true is returned.
        bool getVisibility(const std::string &key) const;

        //! @brief Sets the visibility of an object. The key can be
        //! hierarchical,
        //! with name separated by "/". For instance, "Parent/Child".
        void setVisibility(const std::string &key, const bool visible);

        //! @brief Notify the scene that the mouse left the scene.
        virtual void notifyMouseLeaveEvent(GraphicsView *view) {}

       private:
        bool clsHover = false;
        float clsTypicalLength = 1;

        int clsHighligthOutlineThickness = 2;
        int clsHoverOutlineThickness = 2;
        int clsDrawingOutlineThickness = 2;

        QPainterPath clsHoverOutline;
        std::list<QPainterPath> clsHighlightOutlines;
        std::list<QPainterPath> clsDrawingOutlines;

        std::vector<std::tuple<float, GraphicsLayer *>> clsLayers;
        std::map<std::string, bool> clsVisibility;

};  // end class

}  // end namespace

#endif