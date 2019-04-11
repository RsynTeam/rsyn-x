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

#include <QPainter>
#include <QStyleOption>

#include "GraphicsScene.h"
#include "GraphicsLayer.h"
#include "GraphicsItem.h"

// @note Temporary.
#include "GraphicsItemGridLookup.h"

#include "gui/QtUtils.h"

namespace Rsyn {

// -----------------------------------------------------------------------------

GraphicsLayer::GraphicsLayer() {
        clsLookup = new GraphicsItemGridLookup();
}  // end method

// -----------------------------------------------------------------------------

GraphicsLayer::~GraphicsLayer() {
        for (GraphicsItem *item : clsLookup->allItems()) {
                delete item;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void GraphicsLayer::render(QPainter *painter, const float lod,
                           const QRectF &exposedRect) {
        if (!getScene()->getVisibility(clsVisibilityKey)) return;

        painter->setBrush(clsBrush);
        painter->setPen(clsPen);

        const float coverage = clsLookup->getCoverage(exposedRect);
        if (coverage > 0.8 || clsLookup->isDummy()) {
                for (GraphicsItem *item : clsLookup->allItems()) {
                        item->render(getScene(), painter, lod, exposedRect);
                }  // end for
        } else {
                std::list<GraphicsItem *> items =
                    clsLookup->getItemsAt(exposedRect);
                for (GraphicsItem *item : items) {
                        item->render(getScene(), painter, lod, exposedRect);
                }  // end for
        }          // end method
}  // end method

// -----------------------------------------------------------------------------

void GraphicsLayer::setScene(GraphicsScene *scene) {
        clsScene = scene;
        clsLookup->defineGrid(scene->sceneRect(),
                              scene->sceneRect().width() / 10,
                              scene->sceneRect().height() / 10);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsLayer::setZOrder(const float z) {
        clsZOrder = z;
        if (clsScene) {
                clsScene->resortLayers();
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

const GraphicsItemLookup *GraphicsLayer::getLookup() const {
        return clsLookup;
}  // end method

// -----------------------------------------------------------------------------

void GraphicsLayer::addItem(GraphicsItem *item) {
        clsLookup->addItem(item);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsLayer::removeItem(GraphicsItem *item) {
        clsLookup->removeItem(item);
}  // end method

// -----------------------------------------------------------------------------

GraphicsItem *GraphicsLayer::getItemAt(const qreal x, const qreal y) const {
        return clsLookup->getItemAt(QPointF(x, y));
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
