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

#ifndef RSYN_GRAPHICS_ITEM_H
#define RSYN_GRAPHICS_ITEM_H

#include <QRect>
#include <QPainterPath>

class QPainter;

namespace Rsyn {

class GraphicsScene;

class GraphicsItem {
        friend class GraphicsScene;
        friend class GraphicsLayer;

       public:
        //! @brief Renders this graphics item. The rendering should be done in
        //! scene
        //! coordinates.
        virtual void render(GraphicsScene *scene, QPainter *painter,
                            const float lod, const QRectF &exposedRect) = 0;

        //! @brief Defines the bounding box of this graphics item in scene
        //! coordinates.
        //! @note The bounding box does not need to be stored as this function
        //! is
        //! called only when the item is inserted in the graphics layer.
        virtual QRect getBoundingRect() const = 0;

        //! @brief Defines the outline of this graphics item in scene
        //! coordinates.
        //! This is used, for instance, to highlight the object when the mouse
        //! hovers over it.
        virtual QPainterPath getOutline() const {
                QPainterPath outline;
                outline.addRect(getBoundingRect());
                return outline;
        }  // end method

        //! @brief Should return true if (x, y), given in scene coordinates, is
        //! insides (intersects) this graphics item.
        virtual bool contains(const int x, const int y) const {
                return getOutline().contains(QPointF(x, y));
        }  // end method

        //! @brief Gets the user data.
        void *getUserData() const { return clsUserData; }  // end method

        //! @brief Sets the user data;
        void setUserData(void *data) { clsUserData = data; }  // end method

       private:
        void *clsUserData = nullptr;

};  // end class

}  // end namespace

#endif
