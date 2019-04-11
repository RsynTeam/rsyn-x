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

#include "StandardCell.h"
#include "gui/graphics/LayoutGraphicsScene.h"
#include "gui/QtUtils.h"

#include <QPainter>
#include <QFont>
#include <QFontMetrics>

namespace Rsyn {

void StandardCellGraphicsItem::render(GraphicsScene *scene, QPainter *painter,
                                      const float lod,
                                      const QRectF &exposedRect) {
        Rsyn::Cell cell = getCell();

        LayoutGraphicsScene *layoutScene = (LayoutGraphicsScene *)scene;
        if (layoutScene->isColoringEnabled()) {
                painter->setPen(QtUtils::convert(
                    layoutScene->getColor((Instance)getCell())));
                painter->fillRect(
                    cell.getX(), cell.getY(), cell.getWidth(), cell.getHeight(),
                    QtUtils::convert(
                        layoutScene->getColor((Instance)getCell())));
        }  // end if

        if (cell.getOrientation() == Rsyn::ORIENTATION_E ||
            cell.getOrientation() == Rsyn::ORIENTATION_FE ||
            cell.getOrientation() == Rsyn::ORIENTATION_W ||
            cell.getOrientation() == Rsyn::ORIENTATION_FW) {
                painter->drawRect(cell.getX(), cell.getY(), cell.getHeight(),
                                  cell.getWidth());
        } else {
                painter->drawRect(cell.getX(), cell.getY(), cell.getWidth(),
                                  cell.getHeight());
        }

        // Draw pins.
        if (lod > 50) {
                renderPins(painter);
        }  // end if

        // Draw name.
        if (lod > 100) {
                renderName(painter, exposedRect);
        }  // end method
}  // end method

}  // end namespace
