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

/*
 * File:   DensityGridOverlay.cpp
 * Author: jucemar
 *
 * Created on June 1, 2018, 11:02 AM
 */

#include "DensityGridOverlay.h"

#include "gui/graphics/GraphicsView.h"
#include "gui/graphics/GraphicsScene.h"
#include "gui/graphics/LayoutGraphicsScene.h"
#include "gui/QtUtils.h"

namespace Rsyn {

// -----------------------------------------------------------------------------

bool DensityGridOverlay::init(
    Rsyn::LayoutGraphicsScene* scene,
    std::vector<Rsyn::GraphicsLayerDescriptor>& visibilityItems) {
        Rsyn::Session session;

        clsDesign = session.getDesign();
        clsModule = clsDesign.getTopModule();

        //	clsRsynGraphics = session.getService("rsyn.graphics");
        //	if (!clsRsynGraphics)
        //		return false;

        if (!session.isServiceRunning("rsyn.densityGrid")) return false;

        clsDensityGrid = session.getService("rsyn.densityGrid");

        clsPhDesign = session.getPhysicalDesign();
        if (!clsPhDesign) return false;

        // Define item's bound.
        const Bounds& coreBounds = clsPhDesign.getPhysicalDie().getBounds();
        bounds = QRectF(coreBounds.getX(), coreBounds.getY(),
                        coreBounds.getWidth(), coreBounds.getHeight());

        visibilityItems.push_back(Rsyn::GraphicsLayerDescriptor(
            "DensityGrid", true));  // @todo this should not be necessary
        visibilityItems.push_back(
            Rsyn::GraphicsLayerDescriptor("DensityGrid/Bins", false));
        visibilityItems.push_back(Rsyn::GraphicsLayerDescriptor(
            "DensityGrid/Heat Map Overflow", false));
        visibilityItems.push_back(Rsyn::GraphicsLayerDescriptor(
            "DensityGrid/Heat Map Free Area", false));

        return true;

}  // end method

// -----------------------------------------------------------------------------

void DensityGridOverlay::render(QPainter* painter, const float lod,
                                const QRectF& exposedRect) {
        if (!clsDensityGrid) return;

        if (getScene()->getVisibility("DensityGrid/Bins")) renderBins(painter);
        if (getScene()->getVisibility("DensityGrid/Heat Map Overflow"))
                renderOverflow(painter);
        if (getScene()->getVisibility("DensityGrid/Heat Map Free Area"))
                renderFreeArea(painter);
}  // end method

// -----------------------------------------------------------------------------

void DensityGridOverlay::renderBins(QPainter* painter) {
        QPen pen(Qt::black);
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter->setPen(pen);

        QBrush brush(Qt::white);
        painter->setBrush(brush);

        for (const DensityGridBin& bin : clsDensityGrid->allBins()) {
                const Bounds& binBounds = bin.getBounds();
                const DBUxy length = binBounds.computeLength();
                QRect rect(binBounds[LOWER][X], binBounds[LOWER][Y], length[X],
                           length[Y]);
                painter->drawRect(rect);

        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGridOverlay::renderOverflow(QPainter* painter) {
        QPen pen(Qt::black);
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter->setPen(pen);

        QBrush brush(Qt::red);
        painter->setBrush(brush);

        for (const DensityGridBin& bin : clsDensityGrid->allBins()) {
                DBU freeArea =
                    bin.getArea(PLACEABLE_AREA) - bin.getArea(FIXED_AREA);
                DBU instArea = bin.getArea(MOVABLE_AREA);
                if (instArea < freeArea) continue;

                const Bounds& binBounds = bin.getBounds();
                const DBUxy length = binBounds.computeLength();
                QRect rect(binBounds[LOWER][X], binBounds[LOWER][Y], length[X],
                           length[Y]);
                painter->drawRect(rect);

        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGridOverlay::renderFreeArea(QPainter* painter) {
        QPen pen(Qt::black);
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter->setPen(pen);

        QBrush brush(Qt::blue);
        painter->setBrush(brush);

        for (const DensityGridBin& bin : clsDensityGrid->allBins()) {
                DBU freeArea =
                    bin.getArea(PLACEABLE_AREA) - bin.getArea(FIXED_AREA);
                DBU instArea = bin.getArea(MOVABLE_AREA);
                if (instArea > freeArea) continue;

                const Bounds& binBounds = bin.getBounds();
                const DBUxy length = binBounds.computeLength();
                QRect rect(binBounds[LOWER][X], binBounds[LOWER][Y], length[X],
                           length[Y]);
                painter->drawRect(rect);

        }  // end for

}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
