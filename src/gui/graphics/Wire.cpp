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

#include "Wire.h"

#include "phy/PhysicalDesign.h"
#include "io/Graphics.h"

#include "gui/graphics/GraphicsView.h"
#include "gui/QtUtils.h"

#include <limits>
#include <QPainter>

namespace Rsyn {

// -----------------------------------------------------------------------------

WireGraphicsItem::WireGraphicsItem(Rsyn::Net net,
                                   const Rsyn::PhysicalRoutingWire &wire)
    : NetGraphicsItem(net) {
        Rsyn::Polygon polygon;
        if (wire.convertToPolygon(polygon)) {
                clsPolygon = QtUtils::convert(polygon);
        }  // end if

        clsPhysicalLayer = wire.getLayer();
}  // end method

// -----------------------------------------------------------------------------

void WireGraphicsItem::render(GraphicsScene *scene, QPainter *painter,
                              const float lod, const QRectF &exposedRect) {
        painter->drawPolygon(clsPolygon);
}  // end method

}  // end namespace
