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

#ifndef RSYN_PORT_GRAPHICS_ITEM_H
#define RSYN_PORT_GRAPHICS_ITEM_H

#include "Net.h"
#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include "gui/QtUtils.h"

#include <QColor>
#include <QGraphicsItem>

namespace Rsyn {

class PortGraphicsItem : public GraphicsItem {
       public:
        PortGraphicsItem(Rsyn::Port port);

        virtual void render(GraphicsScene *scene, QPainter *painter,
                            const float lod,
                            const QRectF &exposedRect) override;

        virtual QRect getBoundingRect() const override {
                return clsRect;
        }  // end method

        Rsyn::Port getPort() const { return clsPort; }  // end method

       private:
        Rsyn::Port clsPort;
        QRect clsRect;
};  // end class

}  // end namespace

#endif
