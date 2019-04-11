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

#include <QPixmap>
#include <QBitmap>
#include <QIcon>
#include <QPainter>

#include "GraphicsStipple.h"

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

GraphicsStippleMgr *GraphicsStippleMgr::instance = nullptr;

// -----------------------------------------------------------------------------

void GraphicsStippleMgr::create() {
        if (!instance) instance = new GraphicsStippleMgr();
}  // end method

// -----------------------------------------------------------------------------

GraphicsStippleMgr::GraphicsStippleMgr() {
        stippleBrushes.clear();
        stippleBrushes.resize(STIPPLE_MASK_NUM_MASKS);

        for (int i = 0; i < STIPPLE_MASK_NUM_MASKS; i++) {
                QBitmap pixmap;
                pixmap = pixmap.fromData(QSize(32, 32), STIPPLE_MASKS[i],
                                         QImage::Format_Mono);
                stippleBrushes[i].setTexture(pixmap);
        }  // end for
}  // end constructor

// -----------------------------------------------------------------------------

QIcon GraphicsStippleMgr::createIcon(const FillStippleMask &mask, const int w,
                                     const int h, const QColor &color) const {
        QPixmap pixmap(w, h);
        QPainter painter(&pixmap);

        painter.fillRect(0, 0, w, h, Qt::white);

        QBrush brush = getBrush(mask);
        brush.setColor(color);

        QPen pen;
        pen.setColor(color);

        painter.setBrush(brush);
        painter.setPen(pen);

        painter.drawRect(0, 0, w - 1, h - 1);

        return QIcon(pixmap);
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace