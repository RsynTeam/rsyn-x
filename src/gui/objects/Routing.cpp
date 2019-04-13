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

#include "Routing.h"

#include "session/Session.h"
#include "phy/PhysicalDesign.h"
#include "ispd18/RoutingGuide.h"

#include "gui/graphics/GraphicsScene.h"
#include "gui/graphics/GraphicsView.h"
#include "gui/graphics/GraphicsItemLookup.h"

#include "Instance.h"
#include "StandardCell.h"
#include "Macro.h"
#include "gui/QtUtils.h"
#include "Via.h"
#include "gui/graphics/LayoutGraphicsScene.h"
#include "ispd18/Guide.h"

#include <limits>
#include <QtWidgets>
#include <QColor>
#include <QPen>
#include <QBrush>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

RoutingLayoutGraphicsLayer::RoutingLayoutGraphicsLayer(
    Rsyn::PhysicalLayer physicalLayer) {
        clsPhysicalLayer = physicalLayer;

        Rsyn::Session session;
        clsPhysicalDesign = session.getPhysicalDesign();
        clsPhysicalRoutingGrid =
            clsPhysicalDesign.getPhysicalRoutingGrid(physicalLayer);

        clsRoutingGuide =
            session.getService("rsyn.routingGuide", Rsyn::SERVICE_OPTIONAL);
}  // end method

// -----------------------------------------------------------------------------

RoutingLayoutGraphicsLayer::~RoutingLayoutGraphicsLayer() {}  // end destructor

// -----------------------------------------------------------------------------

bool RoutingLayoutGraphicsLayer::init(
    LayoutGraphicsScene *scene,
    std::vector<GraphicsLayerDescriptor> &visibilityItems) {
        return clsPhysicalDesign;
}  // end method

// -----------------------------------------------------------------------------

void RoutingLayoutGraphicsLayer::render(QPainter *painter, const float lod,
                                        const QRectF &exposedRect) {
        if (!getScene()->getVisibility("PhysicalLayer/" +
                                       clsPhysicalLayer.getName())) {
                return;
        }  // end if

        // Render tracks.
        renderTracks(painter, lod, exposedRect);

        // Render wires.
        if (getScene()->getVisibility("Routing")) {
                // Transformation to map from scene to viewport (pixels).
                const QTransform &wt = painter->worldTransform();

                // Transformation to map from viewport (pixels) to scene.
                const QTransform &iwt = wt.inverted();

                // Compute how much the center of view has shifted in scene
                // coordinates.
                const QPointF centerOfScene = getScene()->sceneRect().center();
                const QPointF centerOfViewportInSceneCoords =
                    iwt.map(QPointF(painter->viewport().width() / 2.0f,
                                    painter->viewport().height() / 2.0f));

                const QPointF displacement =
                    centerOfScene - centerOfViewportInSceneCoords;

                // Adjusts to make the stipple pattern independent of scaling
                // and
                // translation.
                clsBrush.setTransform(iwt);
                painter->setBrushOrigin(displacement);

                // Render layer.
                if (clsPhysicalLayer.getType() == Rsyn::CUT) {
                        // Kinda hacky... We bypass the default rendering
                        // mechanism, but
                        // for efficiency sake, ugly is beautiful.
                        if (lod > 50 &&
                            getScene()->getVisibility(
                                "PhysicalLayer/" +
                                clsPhysicalLayer.getName())) {
                                // Query items to be rendered.
                                std::list<GraphicsItem *> items =
                                    getLookup()->getItemsAt(exposedRect);

                                // Bottom layer.
                                if (clsBottomLayerBrush.style() !=
                                    Qt::NoBrush) {
                                        clsBottomLayerBrush.setTransform(
                                            clsBrush.transform());
                                        painter->setBrush(clsBottomLayerBrush);
                                        painter->setPen(clsBottomLayerPen);
                                        for (GraphicsItem *item : items) {
                                                ((ViaGraphicsItem *)item)
                                                    ->renderBottomLayer(
                                                        painter);
                                        }  // end for
                                }          // end if

                                // Cut layer.
                                for (GraphicsItem *item : items) {
                                        painter->setBrush(clsBrush);
                                        painter->setPen(clsPen);
                                        ((ViaGraphicsItem *)item)
                                            ->renderCutLayer(painter);
                                }  // end for

                                // Bottom layer.
                                if (clsTopLayerBrush.style() != Qt::NoBrush) {
                                        clsTopLayerBrush.setTransform(
                                            clsBrush.transform());
                                        painter->setBrush(clsTopLayerBrush);
                                        painter->setPen(clsTopLayerPen);
                                        for (GraphicsItem *item : items) {
                                                ((ViaGraphicsItem *)item)
                                                    ->renderTopLayer(painter);
                                        }  // end for
                                }          // end if
                        }                  // end if
                } else {
                        LayoutGraphicsLayer::render(painter, lod, exposedRect);
                }  // end method

                // Move back the brush origin to (0, 0).
                painter->setBrushOrigin(0, 0);
        }  // end if

        // Render tracks.
        renderRoutingGuides(painter, lod);
}  // end method

// -----------------------------------------------------------------------------

void RoutingLayoutGraphicsLayer::renderRoutingGuides(QPainter *painter,
                                                     const float lod) {
        if (!clsRoutingGuide) return;

        LayoutGraphicsScene *scene = (LayoutGraphicsScene *)getScene();

        // Gets the selected net.
        Rsyn::Net net = scene->getSelectedNet();
        if (!net) return;

        // Gets the routing guide of the net.
        const Rsyn::NetGuide &guide = clsRoutingGuide->getGuide(net);

        QPainterPath outline;
        // Mateus @ 180824: Fixed bug with overlapping routing guides in a same
        // layer
        outline.setFillRule(Qt::WindingFill);
        for (const Rsyn::LayerGuide &segment : guide.allLayerGuides()) {
                const Rsyn::PhysicalLayer layer = segment.getLayer();
                if (layer != clsPhysicalLayer) continue;
                outline.addRect(QtUtils::convert(segment.getBounds()));
        }  // end for
        outline = outline.simplified();

        // Render
        QColor penColor = getPen().color();
        penColor.setAlphaF(0.65);
        QPen pen;
        pen.setColor(penColor);
        pen.setStyle(Qt::SolidLine);
        pen.setCosmetic(true);
        pen.setWidth(2);
        painter->setPen(pen);

        QColor brushColor = getBrush().color();
        brushColor.setAlphaF(0.1);
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(brushColor);
        painter->setBrush(brush);

        painter->drawPath(outline);
}  // end method

// -----------------------------------------------------------------------------

void RoutingLayoutGraphicsLayer::renderTracks(QPainter *painter,
                                              const float lod,
                                              const QRectF &exposedRect) {
        if (lod < 50) {
                return;
        }  // end if

        if (clsPhysicalLayer.getType() != Rsyn::ROUTING) {
                return;
        }  // end of

        const bool showPreferredDirection =
            getScene()->getVisibility("Tracks/Preferred");
        const bool showNonPreferredDirection =
            getScene()->getVisibility("Tracks/Non-Preferred");
        if (!showPreferredDirection && !showNonPreferredDirection) {
                return;
        }  // end if

        QPen pen = clsPen;
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);

        // Currently I don't clip the tracks (see below) as this creates
        // artifacts
        // when scrolling the viewport. The artifacts come from the fact that
        // the
        // dash pattern does not start at the same position.
        if ((showPreferredDirection &&
             (clsPhysicalLayer.getDirection() == Rsyn::VERTICAL)) ||
            showNonPreferredDirection) {
                const int numCols = clsPhysicalRoutingGrid.getNumCols();
                for (int col = 0; col < numCols; col++) {
                        const DBUxy p0 =
                            clsPhysicalRoutingGrid.getTrackMinPosition(
                                Rsyn::VERTICAL, col);
                        const DBUxy p1 =
                            clsPhysicalRoutingGrid.getTrackMaxPosition(
                                Rsyn::VERTICAL, col);
                        painter->drawLine(p0.x, p0.y, p1.x, p1.y);
                }  // end for
        }          // end if

        if ((showPreferredDirection &&
             (clsPhysicalLayer.getDirection() == Rsyn::HORIZONTAL)) ||
            showNonPreferredDirection) {
                // Note: bottom and top are inverted as we inverted the y-axis.
                const int numRows = clsPhysicalRoutingGrid.getNumRows();
                for (int row = 0; row < numRows; row++) {
                        const DBUxy p0 =
                            clsPhysicalRoutingGrid.getTrackMinPosition(
                                Rsyn::HORIZONTAL, row);
                        const DBUxy p1 =
                            clsPhysicalRoutingGrid.getTrackMaxPosition(
                                Rsyn::HORIZONTAL, row);
                        painter->drawLine(p0.x, p0.y, p1.x, p1.y);
                }  // end for
        }          // end if

        // The issue with the implementation below is that the dashes do not
        // coincide and so artifacts appear when scrolling the viewport. For
        // now,
        // just draw all lines without clipping as this does not seem to affect
        // much the rendering time.
        /*
        if ((showPreferredDirection && (clsPhysicalLayer.getDirection() ==
        Rsyn::VERTICAL)) || showNonPreferredDirection) {
                const int numCols = clsPhysicalRoutingGrid.getNumCols();
                const int col0 =
        std::max(clsPhysicalRoutingGrid.getCol(exposedRect.left()) - 1, 0);
                const int col1 =
        std::min(clsPhysicalRoutingGrid.getCol(exposedRect.right()) + 1, numCols
        - 1);

                for (int col = col0; col <= col1; col++) {
                        const DBUxy p0 =
        clsPhysicalRoutingGrid.getTrackMinPosition(Rsyn::VERTICAL, col);
                        const DBUxy p1 =
        clsPhysicalRoutingGrid.getTrackMaxPosition(Rsyn::VERTICAL, col);
                        painter->drawLine(p0.x, p0.y, p1.x, p1.y);
                } // end for
        } // end if

        if ((showPreferredDirection && (clsPhysicalLayer.getDirection() ==
        Rsyn::HORIZONTAL)) || showNonPreferredDirection) {
                // Note: bottom and top are inverted as we inverted the y-axis.
                const int numRows = clsPhysicalRoutingGrid.getNumRows();
                const int row0 =
        std::max(clsPhysicalRoutingGrid.getRow(exposedRect.top()) - 1, 0);
                const int row1 =
        std::min(clsPhysicalRoutingGrid.getRow(exposedRect.bottom()) + 1,
        numRows - 1);

                for (int row = row0; row <= row1; row++) {
                        const DBUxy p0 =
        clsPhysicalRoutingGrid.getTrackMinPosition(Rsyn::HORIZONTAL, row);
                        const DBUxy p1 =
        clsPhysicalRoutingGrid.getTrackMaxPosition(Rsyn::HORIZONTAL, row);
                        painter->drawLine(p0.x, p0.y, p1.x, p1.y);
                } // end for
        } // end if
        */
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
