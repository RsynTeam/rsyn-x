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

#ifndef RSYN_LAYOUT_GRAPHICS_SCENE_H
#define RSYN_LAYOUT_GRAPHICS_SCENE_H

#include <vector>

#include "GraphicsScene.h"

#include "LayoutGraphicsView.h"
#include "LayoutGraphicsLayer.h"

#include "phy/PhysicalDesign.h"
#include "phy/infra/PhysicalObserver.h"
#include "core/infra/Observer.h"

namespace Rsyn {

class RoutingGuide;

class FloorplanLayoutGraphicsLayer;
class InstanceLayoutGraphicsLayer;
class RoutingLayoutGraphicsLayer;

class WireGraphicsItem;

class LayoutGraphicsScene : public GraphicsScene,
                            public DesignObserver,
                            public PhysicalDesignObserver {
       public:
        enum GraphicsLayerEnum {
                LAYER_BACKGROUND,
                LAYER_FLOORPLAN,
                LAYER_BELOW_INSTANCE,
                LAYER_MACRO,
                LAYER_PORT,
                LAYER_STANDARD_CELL,
                LAYER_ABOVE_INSTANCE,
                LAYER_BELOW_ROUTING,
                LAYER_ROUTING,
                LAYER_ABOVE_ROUTING,
                LAYER_FOREGROUND
        };  // end enum

        LayoutGraphicsScene();
        virtual ~LayoutGraphicsScene();

        LayoutGraphicsLayer *getGraphicsLayer(
            const GraphicsLayerEnum &graphicsLayer) const;
        LayoutGraphicsLayer *getRoutingGraphicsLayer(
            Rsyn::PhysicalLayer &layer) const;

        void init();
        bool isInit() const { return clsPhysicalDesign != nullptr; }

        //! @brief Start user graphics layers (overlays) that were not yet
        //! started.
        //! Returns true if at least one overlay was started.
        bool initMissingUserGraphicsLayers();

        //! @brief Render the scene if changes occurred.
        void redrawScene();

        //! @brief
        void redrawRegionCoveredByRoutingGuides(Rsyn::Net net,
                                                const int expandBy);

        //! @brief ...
        const std::vector<GraphicsLayerDescriptor> &getVisibilityItems() const {
                return clsVisibilityItems;
        }  // end method

        bool isColoringEnabled() const {
                return clsRsynGraphics->isColoringEnabled();
        }  // end method

        Color getColor(Rsyn::Instance inst) const {
                return clsRsynGraphics->getCellColor(inst);
        }  // end method

        //! @brief Gets the selected instance.
        Rsyn::Instance getSelectedInstance() const {
                return clsSelectedInstance;
        }  // end method

        //! @brief Gets the selected instance.
        Rsyn::Net getSelectedNet() const {
                return clsSelectedNet;
        }  // end method

        //! @brief Gets the selected instance.
        Rsyn::Pin getSelectedPin() const {
                return clsSelectedPin;
        }  // end method

        //! @brief Selects the object at (x, y) if any.
        bool selectObjectAt(const QPointF pos) {
                QRectF dummy;
                return selectObjectAt(pos, dummy);
        }  // end method

        //! @brief Selects the object at (x, y) if any. Returns the enclosing
        //! rectangle of the selected object.
        bool selectObjectAt(const QPointF pos, QRectF &bounds);

        //! @brief Selects an instance.
        bool selectInstance(Rsyn::Instance instance) {
                QRectF dummy;
                return selectInstance(instance, dummy);
        }  // end method

        //! @brief Selects an instance an returns a the enclosing rectangle of
        //! the
        //! instance in scene coordinates. The enclosing rectangle is useful,
        //! for
        //! instance, to zoom in the selected object.
        bool selectInstance(Rsyn::Instance instance, QRectF &bounds);

        //! @brief Selects a net.
        bool selectNet(Rsyn::Net net) {
                QRectF dummy;
                return selectNet(net, dummy);
        }  // end method

        //! @brief Selects a net an returns a the enclosing rectangle of the pin
        //! in
        //! scene coordinates. The enclosing rectangle is useful, for instance,
        //! to
        //! zoom in the selected object.
        bool selectNet(Rsyn::Net net, QRectF &bounds);

        //! @brief Selects a pin.
        bool selectPin(Rsyn::Pin pin) {
                QRectF dummy;
                return selectPin(pin, dummy);
        }  // end method

        //! @brief Selects a pin an returns a the enclosing rectangle of the pin
        //! in
        //! scene coordinates. The enclosing rectangle is useful, for instance,
        //! to
        //! zoom in the selected object.
        bool selectPin(Rsyn::Pin pin, QRectF &bounds);

        //! @brief Clear selection.
        void clearSelection();

        //! @brief Returns clicked mouse position
        DBUxy getClickedMousePosition() const {
                return clsClickedMousePosition;
        }

        //! @brief Sets the status bar on which to display information.
        void setStatusBar(QStatusBar *bar) { statusBar = bar; }

        //! @brief Register a user graphics layer.
        template <typename T>
        static void registerOverlay();

        // Graphics view notifications.
        virtual void notifyMouseLeaveEvent(GraphicsView *view) override;

        // Qt Events
        virtual void mouseDoubleClickEvent(
            QGraphicsSceneMouseEvent *mouseEvent) override;

        virtual void mouseMoveEvent(
            QGraphicsSceneMouseEvent *mouseEvent) override;

        virtual void mousePressEvent(
            QGraphicsSceneMouseEvent *mouseEvent) override;

        virtual void mouseReleaseEvent(
            QGraphicsSceneMouseEvent *mouseEvent) override;

        // Design Notifications
        virtual void onPostInstanceCreate(Rsyn::Instance instance) override;

        virtual void onPreInstanceRemove(Rsyn::Instance instance) override;

        virtual void onPostNetCreate(Rsyn::Net net) override;

        virtual void onPreNetRemove(Rsyn::Net net) override;

        virtual void onPostCellRemap(Rsyn::Cell cell,
                                     Rsyn::LibraryCell oldLibraryCell) override;

        virtual void onPostInstanceMove(Rsyn::Instance instance) override;

        // PhysicalDesign Notifications
        virtual void onPostNetRoutingChange(
            Rsyn::PhysicalNet physicalNet) override;

       private:
        void initDefaultGraphicsLayers();
        void initUserGraphicsLayers();

        void createOrUpdateInstanceItem(Rsyn::Instance instance);
        void createOrUpdateNetItems(Rsyn::Net net);

        void updateSpotlightObject(const QPointF &pos);
        QPainterPath traceNetOutline(Rsyn::Net net, const bool includePins);

        Rsyn::PhysicalDesign clsPhysicalDesign;

        Rsyn::Graphics *clsRsynGraphics = nullptr;
        Rsyn::RoutingGuide *clsRoutingGuide = nullptr;

        FloorplanLayoutGraphicsLayer *clsFloorplanLayer = nullptr;
        GraphicsLayer *clsStandardCellLayer = nullptr;
        GraphicsLayer *clsMacroLayer = nullptr;
        std::vector<RoutingLayoutGraphicsLayer *> clsPhysicalLayers;

        Rsyn::Attribute<Rsyn::Instance, GraphicsItem *> clsInstanceMap;
        Rsyn::Attribute<Rsyn::Net, std::list<GraphicsItem *>> clsNetMap;

        std::vector<LayoutGraphicsLayer *> clsGraphicsLayers;

        std::set<Rsyn::Instance> clsDirtyInstances;
        std::set<Rsyn::Net> clsDirtyNets;

        // Visibility
        bool clsShowRoutingGuides = true;

        // Used to store the current object under the user attention.
        Rsyn::Instance clsSpotlightInstance;
        Rsyn::Net clsSpotlightNet;
        Rsyn::Pin clsSpotlightPin;
        QRectF clsSpotlightBounds;
        QPainterPath clsSpotlightOutline;
        QString clsSpotlightText;

        // Hover object.
        Rsyn::Instance clsHoverInstance;
        Rsyn::Net clsHoverNet;
        Rsyn::Pin clsHoverPin;

        // Selected Rsyn objects.
        // For now lets allow only one selection per object type.
        Rsyn::Instance clsSelectedInstance;
        Rsyn::Net clsSelectedNet;
        Rsyn::Pin clsSelectedPin;

        DBUxy clsClickedMousePosition;

        // Generic functions used to instantiate overlays.
        typedef std::function<LayoutGraphicsLayer *()>
            OverlayInstantiatonFunction;
        typedef std::vector<OverlayInstantiatonFunction> RegisteredOverlayVec;
        static RegisteredOverlayVec *clsRegisteredOverlays;

        std::map<LayoutGraphicsLayer *, bool> clsUserLayers;
        std::vector<GraphicsLayerDescriptor> clsVisibilityItems;

        // Status bar.
        QStatusBar *statusBar = nullptr;
};  // end class

// -----------------------------------------------------------------------------

template <typename T>
void LayoutGraphicsScene::registerOverlay() {
        if (!clsRegisteredOverlays) {
                // Try to avoid "static variable initialization fiasco".
                clsRegisteredOverlays = new RegisteredOverlayVec();
        }  // end if

        RegisteredOverlayVec &registeredOverlayVec = *clsRegisteredOverlays;

        registeredOverlayVec.push_back(
            []() -> LayoutGraphicsLayer * { return new T(); });
}  // end method

}  // end namespace

#endif
