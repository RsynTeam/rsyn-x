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

#ifndef RSYN_CELL_GRAPHICS_ITEM_H
#define RSYN_CELL_GRAPHICS_ITEM_H

#include "gui/graphics/GraphicsItem.h"
#include "session/Session.h"

#include <QRect>

namespace Rsyn {

//! @brief A graphics item representing a cell.
class CellGraphicsItem : public GraphicsItem {
       public:
        CellGraphicsItem(Rsyn::Cell cell) : clsCell(cell) {}  // end constructor

        virtual QRect getBoundingRect() const override {
                if (clsCell.getOrientation() == Rsyn::ORIENTATION_E ||
                    clsCell.getOrientation() == Rsyn::ORIENTATION_FE ||
                    clsCell.getOrientation() == Rsyn::ORIENTATION_W ||
                    clsCell.getOrientation() == Rsyn::ORIENTATION_FW) {
                        return QRect(clsCell.getX(), clsCell.getY(),
                                     clsCell.getHeight(), clsCell.getWidth());
                } else {
                        return QRect(clsCell.getX(), clsCell.getY(),
                                     clsCell.getWidth(), clsCell.getHeight());
                }
        }  // end method

        Rsyn::Cell getCell() const { return clsCell; }  // end method

       protected:
        void renderPins(QPainter* painter) const;
        void renderName(QPainter* painter, const QRectF& exposedRect) const;

       private:
        //! @brief The cell associated to this graphics item.
        Rsyn::Cell clsCell;
};  // end class

}  // end namespace

#endif
