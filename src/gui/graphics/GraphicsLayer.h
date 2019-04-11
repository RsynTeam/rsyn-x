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

#ifndef RSYN_GRAPHICS_LAYER_H
#define RSYN_GRAPHICS_LAYER_H

#include <QRectF>
#include <QBrush>
#include <QPen>

namespace Rsyn {

class GraphicsScene;
class GraphicsItem;
class GraphicsItemLookup;
class GraphicsItemGridLookup;

class GraphicsLayerDescriptor {
       public:
        GraphicsLayerDescriptor();

        GraphicsLayerDescriptor(const std::string name,
                                const bool visible = false)
            : clsName(name), clsVisible(visible) {}

        const std::string &getName() const { return clsName; }
        bool getVisible() const { return clsVisible; }

       private:
        std::string clsName;
        bool clsVisible = false;
};  // end class

class GraphicsLayer {
        friend class GraphicsScene;

       public:
        GraphicsLayer();
        ~GraphicsLayer();

        virtual void render(QPainter *painter, const float lod,
                            const QRectF &exposedRect);

        virtual void setScene(GraphicsScene *scene);

        GraphicsScene *getScene() const { return clsScene; }
        const GraphicsItemLookup *getLookup() const;

        //! @brief Adds an item to this graphics layer. The graphics layer takes
        //! ownership of the item (i.e. it is responsible to delete it).
        void addItem(GraphicsItem *item);

        //! @brief Removes an item from this layer. The item is not deleted and
        //! the
        //! layer loses ownership over it.
        void removeItem(GraphicsItem *item);

        //! @brief
        const QBrush &getBrush() const { return clsBrush; }  // end method

        //! @brief
        const QPen &getPen() const { return clsPen; }  // end method

        //! @brief
        void setBrush(const QBrush &brush) { clsBrush = brush; }  // end method

        //! @brief
        void setPen(const QPen &pen) { clsPen = pen; }  // end method

        //! @brief
        float getZOrder() { return clsZOrder; }  // end method

        //! @brief
        void setZOrder(const float z);

        //! @biref
        void setVisibilityKey(const std::string &key) {
                clsVisibilityKey = key;
        }  // end method

        //! @brief Returns the top most item at position (x, y).
        GraphicsItem *getItemAt(const qreal x, const qreal y) const;

       protected:
        QBrush clsBrush;
        QPen clsPen;

       private:
        GraphicsScene *clsScene = nullptr;
        GraphicsItemGridLookup *clsLookup = nullptr;

        float clsZOrder = 0;
        std::string clsVisibilityKey;
};  // end clas

}  // end namespae

#endif
