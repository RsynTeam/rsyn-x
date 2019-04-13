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

#include "Cell.h"

#include "gui/QtUtils.h"
#include "PinMgr.h"

#include <QPainter>

namespace Rsyn {

// -----------------------------------------------------------------------------

void CellGraphicsItem::renderPins(QPainter* painter) const {
        painter->save();
        painter->translate(clsCell.getX(), clsCell.getY());
        Rsyn::LibraryCell lcell = clsCell.getLibraryCell();
        for (Rsyn::LibraryPin lpin : lcell.allLibraryPins()) {
                if (lpin.isPowerOrGround()) {
                        continue;
                }  // end if
                painter->drawPath(
                    QtPinMgr::get()->getShape(lpin, clsCell.getOrientation()));
        }  // end for
        painter->restore();
}  // end method

// -----------------------------------------------------------------------------

void CellGraphicsItem::renderName(QPainter* painter,
                                  const QRectF& exposedRect) const {
        // Save painter state.
        painter->save();

        // Get cell position and size in pixels.
        const QRect cellRect = painter->worldTransform().mapRect(
            QRect(clsCell.getX(), clsCell.getY(), clsCell.getWidth(),
                  clsCell.getHeight()));

        // Set the world transform to the identity matrix so that everything is
        // rendered in viewport (i.e. pixels) coordinates.
        painter->setWorldTransform(QTransform());

        // Compute the width of the text (in pixels).
        const QString text(QString::fromStdString(clsCell.getName()));
        const int fontSize = 10;

        const QFont font("Times", fontSize);
        painter->setFont(font);

        const QFontMetrics fm = painter->fontMetrics();
        const int textWidth = fm.width(text);

        // Draw cell name if it fits the cell widths.
        if (textWidth < cellRect.width()) {
                const int offsetX = 2;             // in pixels
                const int offsetY = fontSize + 2;  // in pixels

                painter->setPen(QColor(0, 0, 0));
                painter->drawText(cellRect.x() + offsetX,
                                  cellRect.y() + offsetY, text);
        }  // end if

        // Restore painter state.
        painter->restore();
}  // end method

}  // end namespace
