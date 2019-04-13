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

#ifndef RSYN_FLOORPLAN_GRAPHICS_LAYER_H
#define RSYN_FLOORPLAN_GRAPHICS_LAYER_H

#include "gui/graphics/LayoutGraphicsLayer.h"

#include <QRect>

namespace Rsyn {

class LayoutGraphicsScene;

class FloorplanLayoutGraphicsLayer : public LayoutGraphicsLayer {
       private:
        QRect clsCoreBounds;

       public:
        FloorplanLayoutGraphicsLayer();
        ~FloorplanLayoutGraphicsLayer();

        virtual bool init(
            LayoutGraphicsScene *scene,
            std::vector<GraphicsLayerDescriptor> &visibilityItems) override;

        virtual void render(QPainter *painter, const float lod,
                            const QRectF &exposedRect) override;

       private:
        void renderCoreBounds(QPainter *painter);
        void renderPhysicalRows(QPainter *painter);
        void renderPhysicalSites(QPainter *painter);
        void renderLayoutRegions(QPainter *painter);

};  // end class

}  // end namespace

#endif
