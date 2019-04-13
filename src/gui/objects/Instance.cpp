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

#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include "gui/graphics/GraphicsScene.h"
#include "gui/graphics/GraphicsView.h"

#include "gui/graphics/LayoutGraphicsScene.h"

#include "Instance.h"
#include "StandardCell.h"
#include "Macro.h"

#include <limits>
#include <QtWidgets>
#include <QColor>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

InstanceLayoutGraphicsLayer::InstanceLayoutGraphicsLayer() {}  // end method

// -----------------------------------------------------------------------------

InstanceLayoutGraphicsLayer::~InstanceLayoutGraphicsLayer() {
}  // end destructor

// -----------------------------------------------------------------------------

bool InstanceLayoutGraphicsLayer::init(
    LayoutGraphicsScene *scene,
    std::vector<GraphicsLayerDescriptor> &visibilityItems) {
        Rsyn::Session session;
        Rsyn::Design design = session.getDesign();
        Rsyn::Module module = session.getTopModule();
        Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

        if (!physicalDesign) return false;

        // Set style
        QPen pen;
        pen.setColor(QColor(22, 99, 222));
        pen.setCosmetic(true);
        setPen(pen);

        setVisibilityKey("Instances/Cells");
        setZOrder(LayoutGraphicsScene::LAYER_STANDARD_CELL);

        // Create attribute.
        clsInstanceMap = design.createAttribute(nullptr);

        // Add graphics items.
        for (Rsyn::Instance instance : module.allInstances()) {
                if (instance.getType() != Rsyn::CELL) continue;

                Rsyn::Cell cell = instance.asCell();

                GraphicsItem *item = nullptr;
                if (instance.isMacroBlock()) {
                        item = new MacroGraphicsItem(cell);
                } else {
                        item = new StandardCellGraphicsItem(cell);
                }  // end else

                addItem(item);
                clsInstanceMap[instance] = item;
        }  // end for

        // scene->registerObserver(this);
        return true;
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
