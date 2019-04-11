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

#ifndef RSYN_MACRO_GRAPHICS_ITEM_H
#define RSYN_MACRO_GRAPHICS_ITEM_H

#include "Cell.h"

#include <QRect>
#include <QPainterPath>

namespace Rsyn {

//! @brief A graphics item representing a macro block.
class MacroGraphicsItem : public CellGraphicsItem {
       public:
        MacroGraphicsItem(Rsyn::Cell cell);

        virtual void render(GraphicsScene *scene, QPainter *painter,
                            const float lod,
                            const QRectF &exposedRect) override;

        virtual QPainterPath getOutline() const override {
                return clsShape;
        }  // end method

       private:
        //! @brief Stores the outline of this macro which can be
        //! non-rectangular.
        //! @note Since the number of macro block usually is small, this won't
        //! increase the memory footprint significantly.
        QPainterPath clsShape;
};  // end class

}  // end namespace

#endif