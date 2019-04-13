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

#ifndef RSYN_WIRE_GRAPHICS_ITEM_H
#define RSYN_WIRE_GRAPHICS_ITEM_H

#include "Net.h"
#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include <QColor>
#include <QGraphicsItem>

namespace Rsyn {

class WireGraphicsItem : public NetGraphicsItem {
       public:
        WireGraphicsItem(Rsyn::Net net, const Rsyn::PhysicalRoutingWire &wire);

        virtual Rsyn::PhysicalLayer getPhysicalLayer() const override {
                return clsPhysicalLayer;
        }

        virtual void render(GraphicsScene *scene, QPainter *painter,
                            const float lod,
                            const QRectF &exposedRect) override;

        virtual QRect getBoundingRect() const override {
                return clsPolygon.boundingRect().toRect();
        }  // end method

        virtual QPainterPath getOutline() const {
                QPainterPath outline;
                outline.addPolygon(clsPolygon);
                return outline;
        }  // end method

       private:
        Rsyn::PhysicalLayer clsPhysicalLayer;
        QPolygonF clsPolygon;
};  // end class

}  // end namespace

#endif