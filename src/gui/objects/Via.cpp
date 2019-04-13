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

#include "Via.h"
#include "gui/QtUtils.h"

#include <QPainter>

namespace Rsyn {

// -----------------------------------------------------------------------------

ViaGraphicsItem::ViaGraphicsItem(Rsyn::Net net,
                                 const PhysicalRoutingVia physicalRoutingVia)
    : NetGraphicsItem(net),
      clsPhysicalRoutingVia(physicalRoutingVia) {}  // end constructor

// -----------------------------------------------------------------------------

void ViaGraphicsItem::render(GraphicsScene *scene, QPainter *painter,
                             const float lod, const QRectF &exposedRect) {
        assert(false);
}  // end method

// -----------------------------------------------------------------------------

void ViaGraphicsItem::renderBottomLayer(QPainter *painter) {
        renderLayer(painter, getVia().allBottomGeometries());
}  // end method

// -----------------------------------------------------------------------------

void ViaGraphicsItem::renderCutLayer(QPainter *painter) {
        renderLayer(painter, getVia().allCutGeometries());
}  // end method

// -----------------------------------------------------------------------------

void ViaGraphicsItem::renderTopLayer(QPainter *painter) {
        renderLayer(painter, getVia().allTopGeometries());
}  // end method

// -----------------------------------------------------------------------------

void ViaGraphicsItem::renderLayer(
    QPainter *painter,
    const std::vector<Rsyn::PhysicalViaGeometry> &geometries) {
        const DBUxy pos = clsPhysicalRoutingVia.getPosition();
        for (Rsyn::PhysicalViaGeometry geometry : geometries) {
                Bounds bounds = geometry.getBounds();
                bounds.translate(pos);
                painter->drawRect(QtUtils::convert(bounds));
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

QRect ViaGraphicsItem::getBoundingRect() const {
        Rsyn::PhysicalVia via = clsPhysicalRoutingVia.getVia();
        QRect rect = getBoundingRect(via.allCutGeometries());
        rect = rect.united(getBoundingRect(via.allBottomGeometries()));
        rect = rect.united(getBoundingRect(via.allTopGeometries()));
        return rect;
}  // end method

// -----------------------------------------------------------------------------

QRect ViaGraphicsItem::getBoundingRect(
    const std::vector<Rsyn::PhysicalViaGeometry> &geometries) const {
        QRect rect;
        const DBUxy pos = clsPhysicalRoutingVia.getPosition();
        for (Rsyn::PhysicalViaGeometry geometry : geometries) {
                Bounds bounds = geometry.getBounds();
                bounds.translate(pos);
                rect = rect.united(QtUtils::convert(bounds));
        }  // end for
        return rect;
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
