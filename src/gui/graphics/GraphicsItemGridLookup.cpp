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

#include <functional>

#include "GraphicsItemGridLookup.h"
#include "GraphicsItem.h"

namespace Rsyn {

GraphicsItemGridLookup::GraphicsItemGridLookup() {
        clsBinWidth = 0;
        clsBinHeight = 0;
        clsSceneRect = QRectF(0, 0, 0, 0);
        clsGrid.initialize(1, 1);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsItemGridLookup::defineGrid(const QRectF &sceneRect,
                                        const int binWidth,
                                        const int binHeight) {
        clsSceneRect = sceneRect;
        clsBinWidth = binWidth;
        clsBinHeight = binHeight;
        const int numCols = ((int)(clsSceneRect.width() / clsBinWidth)) + 1;
        const int numRows = ((int)(clsSceneRect.height() / clsBinHeight)) + 1;
        clsGrid.initialize(numCols, numRows);

        // Re-insert items in the grid.
        for (GraphicsItem *item : allItems()) {
                addItem(item);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void GraphicsItemGridLookup::addItem(GraphicsItem *item) {
        const QRectF boundingRect = item->getBoundingRect();
        forEachBin(boundingRect,
                   [&](int col, int row) { clsGrid(col, row).insert(item); });
        clsItems.insert(item);
}  // end method

// -----------------------------------------------------------------------------

void GraphicsItemGridLookup::removeItem(GraphicsItem *item) {
        const QRectF boundingRect = item->getBoundingRect();
        forEachBin(boundingRect,
                   [&](int col, int row) { clsGrid(col, row).erase(item); });
        clsItems.erase(item);
}  // end method

// -----------------------------------------------------------------------------

GraphicsItem *GraphicsItemGridLookup::getItemAt(const QPointF &pos) const {
        const int col = mapToCol(pos.x());
        const int row = mapToRow(pos.y());

        const InternalItemList &list = clsGrid(col, row);
        for (GraphicsItem *item : list) {
                if (item->contains(pos.x(), pos.y())) return item;
        }  // end method
        return nullptr;
}  // end method

// -----------------------------------------------------------------------------

std::list<GraphicsItem *> GraphicsItemGridLookup::getItemsAt(
    const QPointF &pos, const int maxNumItems) const {
        return getItemsAt(QRectF(pos.x(), pos.y(), 0, 0));
}  // end method

// -----------------------------------------------------------------------------

std::list<GraphicsItem *> GraphicsItemGridLookup::getItemsAt(
    const QRectF &region, const int maxNumItems) const {
        std::list<GraphicsItem *> result;
        std::unordered_set<GraphicsItem *> visited;

        forEachBin(region,
                   [this, &result, &visited, &region](int col, int row) {
                           for (GraphicsItem *item : clsGrid(col, row)) {
                                   if (!visited.count(item)) {
                                           if (region.intersects(
                                                   item->getBoundingRect())) {
                                                   visited.insert(item);
                                                   result.push_back(item);
                                           }  // end if
                                   }          // end method
                           }                  // end for
                   });                        // end for each

        if (maxNumItems) {
                result.resize(maxNumItems);
        }  // end if

        return result;
}  // end method

// -----------------------------------------------------------------------------

int GraphicsItemGridLookup::mapToCol(const qreal x) const {
        const int col =
            (int)(clsBinWidth > 0 ? (x - clsSceneRect.x()) / clsBinWidth : 0);
        return std::max(0, std::min(col, clsGrid.getNumCols() - 1));
}  // end method

// -----------------------------------------------------------------------------

int GraphicsItemGridLookup::mapToRow(const qreal y) const {
        const int row =
            (int)(clsBinHeight > 0 ? (y - clsSceneRect.y()) / clsBinHeight : 0);
        return std::max(0, std::min(row, clsGrid.getNumRows() - 1));
}  // end method

// -----------------------------------------------------------------------------

int GraphicsItemGridLookup::getMinCol(const QRectF &region) const {
        return mapToCol(region.x());
}  // end method

// -----------------------------------------------------------------------------

int GraphicsItemGridLookup::getMinRow(const QRectF &region) const {
        return mapToRow(region.y());
}  // end method

// -----------------------------------------------------------------------------

int GraphicsItemGridLookup::getMaxCol(const QRectF &region) const {
        return mapToCol(region.x() + region.width());
}  // end method

// -----------------------------------------------------------------------------

int GraphicsItemGridLookup::getMaxRow(const QRectF &region) const {
        return mapToRow(region.y() + region.height());
}  // end method

// -----------------------------------------------------------------------------

void GraphicsItemGridLookup::forEachBin(const QRectF &region,
                                        std::function<void(int, int)> f) const {
        const int col0 = getMinCol(region);
        const int row0 = getMinRow(region);
        const int col1 = getMaxCol(region);
        const int row1 = getMaxRow(region);

        for (int row = row0; row <= row1; row++) {
                for (int col = col0; col <= col1; col++) {
                        f(col, row);
                }  // end for
        }          // end for
}  // end method

}  // end namespace
