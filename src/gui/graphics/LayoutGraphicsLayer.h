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

#ifndef RSYN_LAYOUT_GRAPHIC_SLAYER_H
#define RSYN_LAYOUT_GRAPHIC_SLAYER_H

#include "GraphicsLayer.h"

namespace Rsyn {

class LayoutGraphicsScene;

class LayoutGraphicsLayer : public GraphicsLayer {
       public:
        virtual void render(QPainter *painter, const float lod,
                            const QRectF &exposedRect) override {
                GraphicsLayer::render(painter, lod, exposedRect);
        }  // end method

        virtual bool init(
            LayoutGraphicsScene *scene,
            std::vector<GraphicsLayerDescriptor> &visibilityItems) = 0;
};  // end class

}  // end method

#endif
