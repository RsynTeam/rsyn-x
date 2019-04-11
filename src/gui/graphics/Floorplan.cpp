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

#include "Floorplan.h"

#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include "gui/graphics/GraphicsView.h"
#include "gui/graphics/LayoutGraphicsScene.h"

#include <limits>
#include <QtWidgets>
#include <QColor>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

FloorplanLayoutGraphicsLayer::FloorplanLayoutGraphicsLayer() {}  // end method

// -----------------------------------------------------------------------------

FloorplanLayoutGraphicsLayer::~FloorplanLayoutGraphicsLayer() {
}  // end destructor

// -----------------------------------------------------------------------------

bool FloorplanLayoutGraphicsLayer::init(
    LayoutGraphicsScene *scene,
    std::vector<GraphicsLayerDescriptor> &visibilityItems) {
        Rsyn::Session session;
        Rsyn::PhysicalDesign phDesign = session.getPhysicalDesign();

        if (!phDesign) return false;

        // Store tracks.
        const Bounds &coreBounds = phDesign.getPhysicalDie().getBounds();

        clsCoreBounds = QRect(coreBounds.getX(), coreBounds.getY(),
                              coreBounds.getWidth(), coreBounds.getHeight());

        return true;
}  // end method

// -----------------------------------------------------------------------------

void FloorplanLayoutGraphicsLayer::render(QPainter *painter, const float lod,
                                          const QRectF &exposedRect) {
        const bool renderFloorplan = getScene()->getVisibility("Floorplan");
        if (!renderFloorplan) {
                return;
        }  // end if

        const bool renderCoreBds =
            getScene()->getVisibility("Floorplan/Core Bounds");
        const bool renderRows = getScene()->getVisibility("Floorplan/Rows");
        const bool renderSites = getScene()->getVisibility("Floorplan/Sites");
        const bool renderRegions =
            getScene()->getVisibility("Floorplan/Regions");
        if (renderCoreBds) {
                renderCoreBounds(painter);
        }  // end if

        if (renderRows) {
                renderPhysicalRows(painter);
        }  // end if

        if (renderSites) {
                renderPhysicalSites(painter);
        }  // end if

        if (renderRegions) {
                renderLayoutRegions(painter);
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void FloorplanLayoutGraphicsLayer::renderCoreBounds(QPainter *painter) {
        QPen pen;
        pen.setColor(Qt::darkGray);
        pen.setCosmetic(true);
        painter->setPen(pen);

        QBrush brush;
        painter->setBrush(brush);

        painter->drawRect(clsCoreBounds);
}  // end method

// -----------------------------------------------------------------------------

void FloorplanLayoutGraphicsLayer::renderPhysicalRows(QPainter *painter) {
        Rsyn::Session session;
        Rsyn::PhysicalDesign phDesign = session.getPhysicalDesign();
        QPen pen(Qt::red);
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter->setPen(pen);

        QBrush brush(Qt::white);
        painter->setBrush(brush);

        for (Rsyn::PhysicalRow row : phDesign.allPhysicalRows()) {
                const Bounds &segBounds = row.getBounds();
                const DBUxy length = segBounds.computeLength();
                QRect rect(segBounds[LOWER][X], segBounds[LOWER][Y], length[X],
                           length[Y]);
                painter->drawRect(rect);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void FloorplanLayoutGraphicsLayer::renderPhysicalSites(QPainter *painter) {
        Rsyn::Session session;
        Rsyn::PhysicalDesign phDesign = session.getPhysicalDesign();
        QPen pen(Qt::green);
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter->setPen(pen);

        QBrush brush(Qt::white);
        painter->setBrush(brush);

        for (Rsyn::PhysicalRow row : phDesign.allPhysicalRows()) {
                Rsyn::PhysicalSite site = row.getPhysicalSite();
                DBU width = site.getWidth();
                DBU height = site.getHeight();
                DBUxy pos = row.getCoordinate(LOWER);
                while (pos[X] < row.getCoordinate(UPPER, X)) {
                        QRect rect(pos[X], pos[Y], width, height);
                        painter->drawRect(rect);
                        pos[X] += width;
                }  // end while
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void FloorplanLayoutGraphicsLayer::renderLayoutRegions(QPainter *painter) {
        Rsyn::Session session;
        Rsyn::PhysicalDesign phDesign = session.getPhysicalDesign();
        QPen pen(Qt::green);
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter->setPen(pen);

        QBrush brush(Qt::green);
        painter->setBrush(brush);

        for (Rsyn::PhysicalRegion region : phDesign.allPhysicalRegions()) {
                for (const Bounds &bds : region.allBounds()) {
                        const DBUxy length = bds.computeLength();
                        QRect rect(bds[LOWER][X], bds[LOWER][Y], length[X],
                                   length[Y]);
                        painter->drawRect(rect);
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------
}  // end namespace
