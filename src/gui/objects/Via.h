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

#ifndef RSYN_VIA_GRAPHICS_ITEM_H
#define RSYN_VIA_GRAPHICS_ITEM_H

#include "Net.h"

#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "phy/PhysicalRouting.h"

#include <QRect>

class QPainter;

namespace Rsyn {

class ViaGraphicsItem : public NetGraphicsItem {
       public:
        ViaGraphicsItem(Rsyn::Net net,
                        const PhysicalRoutingVia physicalRoutingVia);

        virtual void render(GraphicsScene *scene, QPainter *painter,
                            const float lod,
                            const QRectF &exposedRect) override;

        virtual QRect getBoundingRect() const override;

        Rsyn::PhysicalVia getVia() const {
                return clsPhysicalRoutingVia.getVia();
        }  // end method

        virtual Rsyn::PhysicalLayer getPhysicalLayer() const override {
                return clsPhysicalRoutingVia.getCutLayer();
        }

        void renderBottomLayer(QPainter *painter);
        void renderCutLayer(QPainter *painter);
        void renderTopLayer(QPainter *painter);

       private:
        void renderLayer(
            QPainter *painter,
            const std::vector<Rsyn::PhysicalViaGeometry> &geometries);

        QRect getBoundingRect(
            const std::vector<Rsyn::PhysicalViaGeometry> &geometries) const;

        PhysicalRoutingVia clsPhysicalRoutingVia;

};  // end class

}  // end namespace

#endif /* VIA_H */
