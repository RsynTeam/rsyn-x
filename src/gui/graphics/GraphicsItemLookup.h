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

#ifndef RSYN_GRAPHICS_ITEM_LOOKUP_H
#define RSYN_GRAPHICS_ITEM_LOOKUP_H

#include <list>
#include <unordered_set>

namespace Rsyn {

class GraphicsItem;

class GraphicsItemLookup {
       public:
        virtual void addItem(GraphicsItem *item) = 0;
        virtual void removeItem(GraphicsItem *item) = 0;

        virtual GraphicsItem *getItemAt(const QPointF &pos) const = 0;
        virtual std::list<GraphicsItem *> getItemsAt(
            const QPointF &pos, const int maxNumItems = 0) const = 0;
        virtual std::list<GraphicsItem *> getItemsAt(
            const QRectF &region, const int maxNumItems = 0) const = 0;

        //! @brief Returns a number in the interval [0, 1] where 1 means that
        //! the
        //! regions covers the entire scene.
        virtual float getCoverage(const QRectF &region) const = 0;

        // @todo Kinda weird to force to use an unordered_set here, but for now
        // let's
        // ignore this.
        virtual const std::unordered_set<GraphicsItem *> &allItems() const = 0;

        //! @brief Indicates that this lookup is a dummy i.e. has no special
        //! data
        //! structure to do the lookup, which may be useful when a layer has few
        //! elements.
        virtual bool isDummy() const { return false; }
};  // end class

}  // end namespace

#endif
