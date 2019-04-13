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

#include "LayoutGraphicsScene.h"
#include "LayoutGraphicsLayer.h"

#include "session/Session.h"
#include "ispd18/RoutingGuide.h"

#include "gui/objects/Routing.h"

#include "gui/objects/StandardCell.h"
#include "gui/objects/Macro.h"
#include "gui/objects/Port.h"
#include "gui/objects/Wire.h"
#include "gui/objects/Via.h"
#include "gui/objects/Rect.h"
#include "gui/objects/PinMgr.h"
#include "gui/objects/Floorplan.h"

#include "gui/QtUtils.h"

#include <QPen>
#include <QBrush>
#include <QColor>
#include <QGraphicsSceneEvent>
#include <QToolTip>
#include <QStatusBar>

namespace Rsyn {

// -----------------------------------------------------------------------------

LayoutGraphicsScene::RegisteredOverlayVec
    *LayoutGraphicsScene::clsRegisteredOverlays = nullptr;

// -----------------------------------------------------------------------------

LayoutGraphicsScene::LayoutGraphicsScene() {}  // end constructor

// -----------------------------------------------------------------------------

LayoutGraphicsScene::~LayoutGraphicsScene() {}  // end destructor

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::init() {
        Rsyn::Session session;
        clsPhysicalDesign = session.getPhysicalDesign();

        clsRsynGraphics = session.getService("rsyn.graphics");
        clsRoutingGuide =
            session.getService("rsyn.routingGuide", Rsyn::SERVICE_OPTIONAL);

        const Bounds &coreBounds =
            clsPhysicalDesign.getPhysicalDie().getBounds();
        QRectF sceneRect(coreBounds.getX(), coreBounds.getY(),
                         coreBounds.getWidth(), coreBounds.getHeight());

        const qreal gap =
            std::min(sceneRect.width(), sceneRect.height()) * 0.01;
        sceneRect.adjust(-gap, -gap, +gap, +gap);

        setSceneRect(sceneRect);
        setTypicalLength(clsPhysicalDesign.getRowHeight() > 0
                             ? clsPhysicalDesign.getRowHeight()
                             : 1000);

        initDefaultGraphicsLayers();
        initUserGraphicsLayers();

        session.getDesign().registerObserver(this);
        session.getPhysicalDesign().registerObserver(this);
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::initDefaultGraphicsLayers() {
        Rsyn::Session session;
        Rsyn::Design design = session.getDesign();
        Rsyn::Module module = session.getTopModule();

        clsFloorplanLayer = new FloorplanLayoutGraphicsLayer;
        clsStandardCellLayer = new GraphicsLayer;
        clsMacroLayer = new GraphicsLayer;

        clsInstanceMap = design.createAttribute(nullptr);
        clsNetMap = design.createAttribute();

        {  // Layer style
                QPen pen;
                pen.setColor(QColor(22, 99, 222));
                pen.setCosmetic(true);
                clsStandardCellLayer->setPen(pen);

                clsStandardCellLayer->setVisibilityKey("Instances/Cells");
                clsStandardCellLayer->setZOrder(LAYER_STANDARD_CELL);
        }  // end block

        {  // Layer style
                QColor outlineColor = Qt::darkGray;
                QColor fillColor = Qt::lightGray;

                QPen pen;
                pen.setColor(outlineColor);
                pen.setCosmetic(true);
                clsMacroLayer->setPen(pen);

                QBrush brush;
                brush.setColor(fillColor);
                brush.setStyle(Qt::SolidPattern);
                clsMacroLayer->setBrush(brush);

                clsMacroLayer->setVisibilityKey("Instances/Macros");
                clsMacroLayer->setZOrder(LAYER_MACRO);
        }  // end block

        // Add routing.
        clsPhysicalLayers.resize(clsPhysicalDesign.getNumLayers());
        for (Rsyn::PhysicalLayer layer :
             clsPhysicalDesign.allPhysicalLayers()) {
                RoutingLayoutGraphicsLayer *graphicsLayer =
                    new RoutingLayoutGraphicsLayer(layer);

                QBrush brush;
                QPen pen;

                if (layer.getType() == Rsyn::ROUTING) {
                        const FillStippleMask pattern =
                            clsRsynGraphics
                                ->getRoutingLayerRendering(
                                    layer.getRelativeIndex())
                                .getFillPattern();
                        const Color color = clsRsynGraphics
                                                ->getRoutingLayerRendering(
                                                    layer.getRelativeIndex())
                                                .getColor();

                        GraphicsStippleMgr *mgr = GraphicsStippleMgr::get();
                        brush = mgr->getBrush(pattern);
                        brush.setColor(QtUtils::convert(color));

                        pen.setColor(QtUtils::convert(color));
                        pen.setCosmetic(true);
                } else if (layer.getType() == Rsyn::CUT) {
                        FillStippleMask topPattern = STIPPLE_MASK_EMPTY;
                        Color topColor(0, 0, 0);

                        FillStippleMask bottomPattern = STIPPLE_MASK_EMPTY;
                        Color bottomColor(0, 0, 0);

                        if (layer.getPhysicalLayerUpper()) {
                                topPattern =
                                    clsRsynGraphics
                                        ->getRoutingLayerRendering(
                                            layer.getPhysicalLayerUpper()
                                                .getRelativeIndex())
                                        .getFillPattern();
                                topColor = clsRsynGraphics
                                               ->getRoutingLayerRendering(
                                                   layer.getPhysicalLayerUpper()
                                                       .getRelativeIndex())
                                               .getColor();
                        }  // end if

                        if (layer.getPhysicalLayerLower()) {
                                bottomPattern =
                                    clsRsynGraphics
                                        ->getRoutingLayerRendering(
                                            layer.getPhysicalLayerLower()
                                                .getRelativeIndex())
                                        .getFillPattern();
                                bottomColor =
                                    clsRsynGraphics
                                        ->getRoutingLayerRendering(
                                            layer.getPhysicalLayerLower()
                                                .getRelativeIndex())
                                        .getColor();
                        }  // end if

                        GraphicsStippleMgr *mgr = GraphicsStippleMgr::get();

                        QBrush topBrush = mgr->getBrush(topPattern);
                        topBrush.setColor(QtUtils::convert(topColor));
                        QPen topPen(QtUtils::convert(topColor));
                        topPen.setCosmetic(true);

                        QBrush bottomBrush = mgr->getBrush(bottomPattern);
                        bottomBrush.setColor(QtUtils::convert(bottomColor));
                        QPen bottomPen(QtUtils::convert(bottomColor));
                        bottomPen.setCosmetic(true);

                        graphicsLayer->setTopLayerBrush(topBrush);
                        graphicsLayer->setTopLayerPen(topPen);
                        graphicsLayer->setBottomLayerBrush(bottomBrush);
                        graphicsLayer->setBottomLayerPen(bottomPen);

                        pen.setColor(Qt::black);
                        pen.setCosmetic(true);
                } else {
                        pen.setColor(Qt::darkGray);
                        pen.setCosmetic(true);
                }  // end else

                graphicsLayer->setBrush(brush);
                graphicsLayer->setPen(pen);
                graphicsLayer->setVisibilityKey("PhysicalLayer/" +
                                                layer.getName());
                graphicsLayer->setZOrder(LAYER_ROUTING);

                clsPhysicalLayers[layer.getIndex()] = graphicsLayer;
        }  // end for

        for (Rsyn::Net net : module.allNets()) {
                createOrUpdateNetItems(net);
        }  // end for

        // Add instances.
        for (Rsyn::Instance instance : module.allInstances()) {
                createOrUpdateInstanceItem(instance);
        }  // end for

        // Add floorplan.
        std::vector<GraphicsLayerDescriptor> visibilityItems;
        clsFloorplanLayer->init(this, visibilityItems);
        clsFloorplanLayer->setVisibilityKey("Floorplan");

        addLayer(clsFloorplanLayer);
        addLayer(clsMacroLayer);
        addLayer(clsStandardCellLayer);

        for (GraphicsLayer *graphicsLayer : clsPhysicalLayers)
                addLayer(graphicsLayer);
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::initUserGraphicsLayers() {
        clsVisibilityItems.clear();

        if (clsRegisteredOverlays) {
                RegisteredOverlayVec &registeredOverlayVec =
                    *clsRegisteredOverlays;
                for (auto &f : registeredOverlayVec) {
                        LayoutGraphicsLayer *graphicsLayer = f();
                        std::vector<GraphicsLayerDescriptor> visibilityItems;
                        if (graphicsLayer->init(this, visibilityItems)) {
                                clsVisibilityItems.insert(
                                    std::end(clsVisibilityItems),
                                    std::begin(visibilityItems),
                                    std::end(visibilityItems));
                                addLayer(graphicsLayer);
                                clsUserLayers[graphicsLayer] = true;
                        } else {
                                clsUserLayers[graphicsLayer] = false;
                        }  // end else
                }          // end for
        }                  // end if
}  // end method

// -----------------------------------------------------------------------------

bool LayoutGraphicsScene::initMissingUserGraphicsLayers() {
        bool started = false;
        for (auto it : clsUserLayers) {
                if (it.second) continue;

                std::vector<GraphicsLayerDescriptor> visibilityItems;
                LayoutGraphicsLayer *graphicsLayer = it.first;
                if (graphicsLayer->init(this, visibilityItems)) {
                        clsVisibilityItems.insert(std::end(clsVisibilityItems),
                                                  std::begin(visibilityItems),
                                                  std::end(visibilityItems));
                        addLayer(graphicsLayer);
                        clsUserLayers[graphicsLayer] = true;
                        started = true;
                } else {
                        clsUserLayers[graphicsLayer] = false;
                }  // end else
        }          // end method
        return started;
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::createOrUpdateInstanceItem(Rsyn::Instance instance) {
        // Remove old items if any.
        GraphicsItem *item = clsInstanceMap[instance];
        if (item) {
                switch (instance.getType()) {
                        case Rsyn::CELL:
                                if (instance.isMacroBlock()) {
                                        clsMacroLayer->removeItem(item);
                                } else {
                                        clsStandardCellLayer->removeItem(item);
                                }  // end else
                                break;
                        case Rsyn::PORT: {
                                Rsyn::Port port = instance.asPort();
                                Rsyn::PhysicalPort physicalPort =
                                    clsPhysicalDesign.getPhysicalPort(port);
                                if (physicalPort && physicalPort.getLayer()) {
                                        clsPhysicalLayers
                                            [physicalPort.getLayer().getIndex()]
                                                ->removeItem(item);
                                }  // end if
                                break;
                        }
                        case Rsyn::MODULE:
                                break;
                }  // end switch
        }          // end for

        // Add wires.
        item = nullptr;
        switch (instance.getType()) {
                case Rsyn::CELL:
                        if (instance.isMacroBlock()) {
                                item = new MacroGraphicsItem(instance.asCell());
                                clsMacroLayer->addItem(item);
                        } else {
                                item = new StandardCellGraphicsItem(
                                    instance.asCell());
                                clsStandardCellLayer->addItem(item);
                        }  // end else
                        break;
                case Rsyn::PORT: {
                        Rsyn::Port port = instance.asPort();
                        Rsyn::PhysicalPort physicalPort =
                            clsPhysicalDesign.getPhysicalPort(port);
                        if (physicalPort && physicalPort.getLayer()) {
                                item = new PortGraphicsItem(instance.asPort());
                                clsPhysicalLayers[physicalPort.getLayer()
                                                      .getIndex()]
                                    ->addItem(item);
                        }  // end if
                        break;
                }  // end case
                case Rsyn::MODULE:
                        break;
        }  // end switch

        if (item) {
                clsInstanceMap[instance] = item;
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::createOrUpdateNetItems(Rsyn::Net net) {
        Rsyn::PhysicalNet phNet = clsPhysicalDesign.getPhysicalNet(net);
        std::list<GraphicsItem *> &items = clsNetMap[net];
        const Rsyn::PhysicalRouting &routing = phNet.getRouting();

        // Remove old items if any.
        for (GraphicsItem *item : items) {
                if (NetGraphicsItem *netItem =
                        dynamic_cast<NetGraphicsItem *>(item)) {
                        clsPhysicalLayers[netItem->getPhysicalLayer()
                                              .getIndex()]
                            ->removeItem(item);
                }  // end if
        }          // end for
        items.clear();

        // Add wires.
        for (const Rsyn::PhysicalRoutingWire &wire : routing.allWires()) {
                if (!wire.isValid()) continue;

                WireGraphicsItem *item = new WireGraphicsItem(net, wire);
                items.push_back(item);
                clsPhysicalLayers[wire.getLayer().getIndex()]->addItem(item);
        }  // end for

        // Add vias.
        for (const Rsyn::PhysicalRoutingVia &via : routing.allVias()) {
                if (!via.isValid()) continue;

                ViaGraphicsItem *item = new ViaGraphicsItem(net, via);
                items.push_back(item);
                clsPhysicalLayers[via.getCutLayer().getIndex()]->addItem(item);
        }  // end for

        // Add rectangles.
        for (const Rsyn::PhysicalRoutingRect &rect : routing.allRects()) {
                RectGraphicsItem *item = new RectGraphicsItem(net, rect);
                items.push_back(item);
                clsPhysicalLayers[rect.getLayer().getIndex()]->addItem(item);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::redrawScene() {
        // Update nets.
        for (Rsyn::Net net : clsDirtyNets) {
                createOrUpdateNetItems(net);
        }  // end for
        clsDirtyNets.clear();

        // Update rendering.
        redraw();
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::redrawRegionCoveredByRoutingGuides(
    Rsyn::Net net, const int expandBy) {
        if (clsRoutingGuide && clsShowRoutingGuides) {
                const Rsyn::NetGuide &guide = clsRoutingGuide->getGuide(net);
                QRect rect;
                for (const Rsyn::LayerGuide &segment : guide.allLayerGuides()) {
                        rect =
                            rect.united(QtUtils::convert(segment.getBounds()));
                }  // end for
                redraw(rect, QGraphicsScene::AllLayers, expandBy);
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::updateSpotlightObject(const QPointF &pos) {
        GraphicsItem *item = getItemAt(pos);

        clsSpotlightNet = nullptr;
        clsSpotlightPin = nullptr;
        clsSpotlightInstance = nullptr;

        clsSpotlightBounds = QRectF();
        clsSpotlightOutline = QPainterPath();
        clsSpotlightText = "";

        if (item) {
                if (CellGraphicsItem *cellItem =
                        dynamic_cast<CellGraphicsItem *>(item)) {
                        Rsyn::Cell cell = cellItem->getCell();
                        const QPoint cellPos(cell.getX(), cell.getY());

                        for (Rsyn::Pin pin : cell.allPins()) {
                                if (pin.isPowerOrGround()) {
                                        continue;
                                }  // end if
                                const QPainterPath &pinShape =
                                    QtPinMgr::get()->getShape(
                                        pin.getLibraryPin(),
                                        cell.getOrientation());
                                if (pinShape.contains(pos - cellPos)) {
                                        clsSpotlightPin = pin;
                                        clsSpotlightOutline =
                                            pinShape.translated(cellPos);
                                        clsSpotlightBounds =
                                            pinShape.boundingRect().translated(
                                                cellPos);
                                        clsSpotlightText =
                                            QString::fromStdString(
                                                pin.getFullName());
                                        break;
                                }  // end if
                        }          // end for

                        if (!clsSpotlightPin) {
                                clsSpotlightInstance = cell;
                                clsSpotlightBounds = item->getBoundingRect();
                                clsSpotlightOutline = item->getOutline();
                                clsSpotlightText =
                                    QString::fromStdString(cell.getName());
                        }  // end if

                        Rsyn::LibraryCell libCell = cell.getLibraryCell();
                        if (libCell) {
                                clsSpotlightText +=
                                    " | Libcell: " +
                                    QString::fromStdString(libCell.getName());
                        }  // end if
                } else if (PortGraphicsItem *portItem =
                               dynamic_cast<PortGraphicsItem *>(item)) {
                        Rsyn::Port port = portItem->getPort();
                        clsSpotlightInstance = port;
                        clsSpotlightBounds = item->getBoundingRect();
                        clsSpotlightOutline = item->getOutline();
                        clsSpotlightText =
                            QString::fromStdString(port.getName());
                } else if (NetGraphicsItem *netItem =
                               dynamic_cast<NetGraphicsItem *>(item)) {
                        Rsyn::Net net = netItem->getNet();

                        clsSpotlightNet = net;
                        clsSpotlightOutline = traceNetOutline(net, true);
                        clsSpotlightText =
                            QString::fromStdString(net.getName());

                        // Compute the spotlight bounds.
                        if (clsRoutingGuide && clsShowRoutingGuides) {
                                // Need to expand the redraw bounds to account
                                // for routing guides.
                                const Rsyn::NetGuide &guide =
                                    clsRoutingGuide->getGuide(net);
                                clsSpotlightBounds = item->getBoundingRect();
                                for (const Rsyn::LayerGuide &segment :
                                     guide.allLayerGuides()) {
                                        clsSpotlightBounds =
                                            clsSpotlightBounds.united(
                                                QtUtils::convert(
                                                    segment.getBounds()));
                                }  // end for
                        } else {
                                clsSpotlightBounds = item->getBoundingRect();
                        }  // end else
                }          // end else-if
        }                  // end if
}  // end method

// -----------------------------------------------------------------------------

bool LayoutGraphicsScene::selectObjectAt(const QPointF pos,
                                         QRectF &selectionBounds) {
        updateSpotlightObject(pos);

        bool success = false;

        if (clsSpotlightInstance) {
                if (clsSpotlightInstance != clsSelectedInstance) {
                        success = selectInstance(clsSpotlightInstance);
                } else {
                        clearSelection();
                }  // end else
        } else if (clsSpotlightNet) {
                if (clsSpotlightNet != clsSelectedNet) {
                        success = selectNet(clsSpotlightNet);
                } else {
                        clearSelection();
                }  // end else
        } else if (clsSpotlightPin) {
                if (clsSpotlightPin != clsSelectedPin) {
                        success = selectPin(clsSpotlightPin);
                } else {
                        clearSelection();
                }  // end else
        } else {
                clearSelection();
        }  // end else

        return success;
}  // end method

// -----------------------------------------------------------------------------

bool LayoutGraphicsScene::selectInstance(Rsyn::Instance instance,
                                         QRectF &bounds) {
        if (!instance) return false;

        clearSelection();
        clsSelectedInstance = instance;

        GraphicsItem *item = clsInstanceMap[instance];
        if (item) {
                addHighlight(item->getOutline());
                bounds = item->getBoundingRect();
        }  // end if

        return item;
}  // end method

// -----------------------------------------------------------------------------

bool LayoutGraphicsScene::selectNet(Rsyn::Net net, QRectF &bounds) {
        if (!net) return false;

        clearSelection();
        clsSelectedNet = net;

        QPainterPath outline;

        std::list<GraphicsItem *> &items = clsNetMap[net];

        for (GraphicsItem *item : items) {
                outline += item->getOutline();
        }  // end for

        outline = outline.simplified();
        addHighlight(outline);
        bounds = outline.boundingRect();
        redrawRegionCoveredByRoutingGuides(clsSelectedNet, 2);

        return true;
}  // end method

// -----------------------------------------------------------------------------

bool LayoutGraphicsScene::selectPin(Rsyn::Pin pin, QRectF &bounds) {
        if (!pin) return false;

        clearSelection();
        clsSelectedPin = pin;

        // Get pin outline.
        QPainterPath outline;
        Rsyn::Instance instance = pin.getInstance();
        if (instance.getType() == Rsyn::CELL) {
                Rsyn::PhysicalCell physicalCell =
                    clsPhysicalDesign.getPhysicalCell(pin);
                Rsyn::LibraryPin lpin = pin.getLibraryPin();
                if (lpin) {
                        outline =
                            QtPinMgr::get()
                                ->getShape(lpin, physicalCell.getOrientation())
                                .translated(physicalCell.getX(),
                                            physicalCell.getY());
                }  // end for
        }          // end if

        addHighlight(outline);
        bounds = outline.boundingRect();

        return true;
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::clearSelection() {
        if (clsSelectedNet)
                redrawRegionCoveredByRoutingGuides(clsSelectedNet, 2);
        clsSelectedInstance = nullptr;
        clsSelectedNet = nullptr;
        clsSelectedPin = nullptr;
        clearHighlight();
}  // end method

// -----------------------------------------------------------------------------

QPainterPath LayoutGraphicsScene::traceNetOutline(Rsyn::Net net,
                                                  const bool includePins) {
        QPainterPath netOutline;

        const std::list<GraphicsItem *> &items = clsNetMap[net];
        for (const GraphicsItem *item : items) {
                netOutline += item->getOutline();
        }  // end for

        if (includePins) {
                for (Rsyn::Pin pin : net.allPins()) {
                        Rsyn::Instance instance = pin.getInstance();
                        if (instance.getType() == Rsyn::CELL) {
                                Rsyn::PhysicalCell physicalCell =
                                    clsPhysicalDesign.getPhysicalCell(pin);
                                Rsyn::LibraryPin lpin = pin.getLibraryPin();
                                if (lpin) {
                                        netOutline +=
                                            QtPinMgr::get()
                                                ->getShape(
                                                    lpin, physicalCell
                                                              .getOrientation())
                                                .translated(
                                                    physicalCell.getX(),
                                                    physicalCell.getY());
                                }  // end for
                        }          // end if
                }                  // end for
        }                          // end if

        return netOutline.simplified();
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::notifyMouseLeaveEvent(GraphicsView *view) {
        clearHoverOutline();
        if (statusBar) {
                statusBar->showMessage("");
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::mouseDoubleClickEvent(
    QGraphicsSceneMouseEvent *mouseEvent) {
        GraphicsScene::mouseDoubleClickEvent(mouseEvent);
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
        const QPointF &pos = mouseEvent->scenePos();

        updateSpotlightObject(pos);

        const bool needsUpdate = (clsHoverInstance != clsSpotlightInstance) ||
                                 (clsHoverNet != clsSpotlightNet) ||
                                 (clsHoverPin != clsSpotlightPin);

        if (needsUpdate) {
                const bool clear = !clsSpotlightInstance && !clsSpotlightNet &&
                                   !clsSpotlightPin;
                if (clear) {
                        clearHoverOutline();
                } else {
                        setHoverOutline(clsSpotlightOutline);
                }  // end else

                clsHoverInstance = clsSpotlightInstance;
                clsHoverNet = clsSpotlightNet;
                clsHoverPin = clsSpotlightPin;
        }  // end if

        if (statusBar) {
                // @todo Separate object name and position into two widgets.
                QString tooltip = "(" + QString::number(pos.x(), 'f', 2) +
                                  ", " + QString::number(pos.y(), 'f', 2) +
                                  ") " + clsSpotlightText;
                statusBar->showMessage(tooltip);
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::mousePressEvent(
    QGraphicsSceneMouseEvent *mouseEvent) {
        GraphicsScene::mousePressEvent(mouseEvent);
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::mouseReleaseEvent(
    QGraphicsSceneMouseEvent *mouseEvent) {
        QPointF mousePos = mouseEvent->scenePos();
        clsClickedMousePosition[X] = mousePos.x();
        clsClickedMousePosition[Y] = mousePos.y();
        selectObjectAt(mousePos);
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::onPostInstanceCreate(Rsyn::Instance instance) {
        std::cout << "WARNING: Layout view does not handle dynamically created "
                     "cells (instances) yet.\n";
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::onPreInstanceRemove(Rsyn::Instance instance) {
        std::cout << "WARNING: Layout view does not handle dynamically deleted "
                     "cells (instances) yet.\n";
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::onPostNetCreate(Rsyn::Net net) {
        std::cout << "WARNING: Layout view does not handle dynamically created "
                     "net yet.\n";
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::onPreNetRemove(Rsyn::Net net) {
        std::cout << "WARNING: Layout view does not handle dynamically deleted "
                     "net yet.\n";
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::onPostCellRemap(Rsyn::Cell cell,
                                          Rsyn::LibraryCell oldLibraryCell) {
        createOrUpdateInstanceItem(cell);
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::onPostInstanceMove(Rsyn::Instance instance) {
        createOrUpdateInstanceItem(instance);
}  // end method

// -----------------------------------------------------------------------------

void LayoutGraphicsScene::onPostNetRoutingChange(
    Rsyn::PhysicalNet physicalNet) {
        clsDirtyNets.insert(physicalNet.getNet());
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
