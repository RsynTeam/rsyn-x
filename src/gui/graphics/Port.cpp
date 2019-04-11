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

#include "Port.h"

#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include "gui/graphics/GraphicsView.h"
#include "gui/QtUtils.h"

#include <limits>
#include <QPainter>

namespace Rsyn {

// -----------------------------------------------------------------------------

PortGraphicsItem::PortGraphicsItem(Rsyn::Port port) : clsPort(port) {
        // Note: Port geometries are given relative to the port position.
        Bounds rect = clsPort.getBounds();
        Rsyn::PhysicalTransform transform(clsPort.getOrientation());
        rect = transform.apply(rect);
        rect.translate(clsPort.getPosition());
        clsRect = QtUtils::convert(rect);
}  // end method

// -----------------------------------------------------------------------------

void PortGraphicsItem::render(GraphicsScene *scene, QPainter *painter,
                              const float lod, const QRectF &exposedRect) {
        painter->drawRect(clsRect);
}  // end method

}  // end namespace
