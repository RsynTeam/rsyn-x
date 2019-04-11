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

#ifndef RSYN_GRAPHICS_ITEM_GRID_LOOKUP_H
#define RSYN_GRAPHICS_ITEM_GRID_LOOKUP_H

#include <vector>
#include <list>
#include <unordered_set>
#include <functional>

#include <QRect>

#include "GraphicsItemLookup.h"
#include "util/Array.h"

namespace Rsyn {

class GraphicsItem;

class GraphicsItemGridLookup : public GraphicsItemLookup {
       public:
        typedef std::unordered_set<GraphicsItem *> InternalItemList;

        GraphicsItemGridLookup();

        void defineGrid(const QRectF &sceneRect, const int binWidth,
                        const int binHeight);

        virtual void addItem(GraphicsItem *item) override;
        virtual void removeItem(GraphicsItem *item) override;

        virtual GraphicsItem *getItemAt(const QPointF &pos) const override;
        virtual std::list<GraphicsItem *> getItemsAt(
            const QPointF &pos, const int maxNumItems = 0) const override;
        virtual std::list<GraphicsItem *> getItemsAt(
            const QRectF &region, const int maxNumItems = 0) const override;

        //! @brief Returns a number in the interval [0, 1] where 1 means that
        //! the
        //! regions covers the entire scene.
        virtual float getCoverage(const QRectF &region) const override {
                const QRectF &overlap = clsSceneRect.intersected(region);
                const float sceneArea =
                    clsSceneRect.width() * clsSceneRect.height();
                const float overlapArea = overlap.width() * overlap.height();
                return sceneArea > 0 ? overlapArea / sceneArea : 1;
        }  // end method

        virtual const std::unordered_set<GraphicsItem *> &allItems()
            const override {
                return clsItems;
        }  // end method

       private:
        int mapToCol(const qreal x) const;
        int mapToRow(const qreal y) const;

        int getMinCol(const QRectF &region) const;
        int getMinRow(const QRectF &region) const;
        int getMaxCol(const QRectF &region) const;
        int getMaxRow(const QRectF &region) const;

        void forEachBin(const QRectF &region,
                        std::function<void(int, int)> f) const;

        QRectF clsSceneRect;
        float clsBinWidth = 0;
        float clsBinHeight = 0;

        std::unordered_set<GraphicsItem *> clsItems;
        Array2D<InternalItemList> clsGrid;

};  // end class

}  // end namespace

#endif
