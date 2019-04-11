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

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <QPainter>
#include <QGraphicsView>

#include "GraphicsScene.h"
#include "GraphicsLayer.h"

namespace Rsyn {

const bool debugRendering = false;

// -----------------------------------------------------------------------------

GraphicsScene::GraphicsScene(QObject *parent)
    : QGraphicsScene(parent) {}  // end method

// -----------------------------------------------------------------------------

GraphicsScene::~GraphicsScene() {
        for (std::tuple<float, GraphicsLayer *> t : clsLayers) {
                delete std::get<1>(t);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {
        const float lod = getLevelOfDetail(painter);

        if (debugRendering) {
                std::cout << "draw background (lod=" << lod << ")\n";
        }  // end if

        // Clear exposed rectangle.
        painter->fillRect(rect, Qt::white);

        // Draw layers.
        const int numLayers = clsLayers.size();
        for (int i = 0; i < numLayers; i++) {
                GraphicsLayer *layer = std::get<1>(clsLayers[i]);
                layer->render(painter, lod, rect);
        }  // end for

        {  // Draw highlights.
                QColor penColor(0, 0, 0);
                penColor.setAlphaF(0.6);
                QPen pen;
                pen.setColor(penColor);
                pen.setWidth(clsHighligthOutlineThickness);
                pen.setCosmetic(true);
                painter->setPen(pen);

                QBrush brush;
                painter->setBrush(brush);

                for (const QPainterPath &outline : clsHighlightOutlines) {
                        painter->drawPath(outline);
                }  // end method
        }          // end block

        {  // Draw drawings.
                QPen pen;
                pen.setColor(QColor(0, 0, 0));
                pen.setWidth(clsDrawingOutlineThickness);
                pen.setCosmetic(true);
                painter->setPen(pen);

                QBrush brush;
                painter->setBrush(brush);

                for (const QPainterPath &outline : clsDrawingOutlines) {
                        painter->drawPath(outline);
                }  // end method
        }          // end block
}  // end method

// -----------------------------------------------------------------------------

void GraphicsScene::drawForeground(QPainter *painter, const QRectF &rect) {
        if (debugRendering) {
                std::cout << "draw foreground\n";
        }  // end if

        if (clsHover) {
                QPen pen;
                pen.setWidth(clsHoverOutlineThickness);
                pen.setColor(Qt::black);
                pen.setStyle(Qt::DashLine);
                pen.setCosmetic(true);

                QBrush brush;

                painter->setPen(pen);
                painter->setBrush(brush);
                painter->drawPath(clsHoverOutline);
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void GraphicsScene::redraw(QGraphicsScene::SceneLayers layers) {
        // This requests a full rendering (i.e. exposed rect = scene rect...
        // We should render just the porting that is being viewed.
        // invalidate(sceneRect(), layers); // <-- bad

        // Invalidate only the regions being viewed.
        for (QGraphicsView *view : views()) {
                const QRect viewRect =
                    QRect(0, 0, view->viewport()->size().width(),
                          view->viewport()->size().height());
                const QRectF mappedViewRect =
                    view->mapToScene(viewRect).boundingRect();
                invalidate(mappedViewRect, layers);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void GraphicsScene::redraw(const QRectF &rect,
                           QGraphicsScene::SceneLayers layers,
                           const int expandBy) {
        for (QGraphicsView *view : views()) {
                const QRect mappedRect =
                    view->mapFromScene(rect).boundingRect();
                const QRect adjustedMappedRect = mappedRect.adjusted(
                    -expandBy, -expandBy, +expandBy, +expandBy);
                const QRectF adjustedRect =
                    view->mapToScene(adjustedMappedRect).boundingRect();
                invalidate(adjustedRect, layers);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void GraphicsScene::addLayer(GraphicsLayer *layer) {
        layer->setScene(this);
        clsLayers.push_back(std::make_tuple(layer->getZOrder(), layer));
        resortLayers();
}  // end method

// -----------------------------------------------------------------------------

float GraphicsScene::getLevelOfDetail(QPainter *painter) const {
        const QTransform &transform = painter->worldTransform();

        const QRectF typicalRectInViewportCoordinates = transform.mapRect(
            QRectF(0, 0, getTypicalLength(), getTypicalLength()));

        return std::min(typicalRectInViewportCoordinates.width(),
                        typicalRectInViewportCoordinates.height());
}  // end method

// -----------------------------------------------------------------------------

void GraphicsScene::ensureVisible(const QRectF &rect) {
        for (QGraphicsView *view : views()) {
                view->ensureVisible(rect);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

GraphicsItem *GraphicsScene::getItemAt(const qreal x, const qreal y) const {
        const int numLayers = clsLayers.size();
        for (int i = numLayers - 1; i >= 0; i--) {
                GraphicsLayer *layer = std::get<1>(clsLayers[i]);
                GraphicsItem *item = layer->getItemAt(x, y);
                if (item) {
                        return item;
                }  // end if
        }          // end for
        return nullptr;
}  // end method

// -----------------------------------------------------------------------------

GraphicsItem *GraphicsScene::getItemAt(const QPointF &point) const {
        return getItemAt(point.x(), point.y());
}  // end method

// -----------------------------------------------------------------------------

void GraphicsScene::resortLayers() {
        for (std::tuple<float, GraphicsLayer *> &t : clsLayers) {
                std::get<0>(t) = std::get<1>(t)->getZOrder();
        }  // end for
        std::sort(clsLayers.begin(), clsLayers.end());
}  // end method

// -----------------------------------------------------------------------------

bool GraphicsScene::getVisibility(const std::string &key) const {
        const std::string separator = "/";

        typedef boost::char_separator<char> ChSep;
        typedef boost::tokenizer<ChSep> TknChSep;
        ChSep sep(separator.c_str());
        TknChSep tok(key, sep);

        auto it = clsVisibility.end();
        std::string hierarchicalName;
        for (TknChSep::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
                const std::string &name = *beg;
                if (!hierarchicalName.empty()) {
                        hierarchicalName += separator;
                }  // end if
                hierarchicalName += name;

                it = clsVisibility.find(hierarchicalName);

                if (it != clsVisibility.end() && !it->second) {
                        return false;
                }  // end if
        }          // end for

        return it != clsVisibility.end() ? it->second : true;
}  // end method

// -----------------------------------------------------------------------------

void GraphicsScene::setVisibility(const std::string &key, const bool visible) {
        auto it = clsVisibility.find(key);
        if (it == clsVisibility.end() || it->second != visible) {
                clsVisibility[key] = visible;
                redraw();
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace