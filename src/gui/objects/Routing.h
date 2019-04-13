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

#ifndef RSYN_ROUTING_GRAPHICS_LAYER_H
#define RSYN_ROUTING_GRAPHICS_LAYER_H

#include "gui/graphics/LayoutGraphicsLayer.h"

#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include <QColor>
#include <QGraphicsItem>

namespace Rsyn {

class LayoutGraphicsScene;
class RoutingGuide;

class RoutingLayoutGraphicsLayer : public LayoutGraphicsLayer {
       public:
        RoutingLayoutGraphicsLayer(Rsyn::PhysicalLayer physicalLayer);
        ~RoutingLayoutGraphicsLayer();

        virtual bool init(
            LayoutGraphicsScene *scene,
            std::vector<GraphicsLayerDescriptor> &visibilityItems) override;

        virtual void render(QPainter *painter, const float lod,
                            const QRectF &exposedRect) override;

        void setTopLayerBrush(const QBrush &brush) {
                clsTopLayerBrush = brush;
        }  // end method

        void setTopLayerPen(const QPen &pen) {
                clsTopLayerPen = pen;
        }  // end method

        void setBottomLayerBrush(const QBrush &brush) {
                clsBottomLayerBrush = brush;
        }  // end method

        void setBottomLayerPen(const QPen &pen) {
                clsBottomLayerPen = pen;
        }  // end method

       private:
        void renderRoutingGuides(QPainter *painter, const float lod);
        void renderTracks(QPainter *painter, const float lod,
                          const QRectF &exposedRect);

        Rsyn::PhysicalDesign clsPhysicalDesign;
        Rsyn::PhysicalLayer clsPhysicalLayer;
        Rsyn::PhysicalRoutingGrid clsPhysicalRoutingGrid;

        Rsyn::RoutingGuide *clsRoutingGuide = nullptr;

        QBrush clsTopLayerBrush;
        QPen clsTopLayerPen;

        QBrush clsBottomLayerBrush;
        QPen clsBottomLayerPen;

};  // end class

}  // end namespace

#endif
