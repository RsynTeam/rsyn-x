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

#ifndef RSYN_PHYSICAL_DESIGN_H
#define RSYN_PHYSICAL_DESIGN_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>
#include <stddef.h>
#include <algorithm>
#include <limits>

#include "util/Point.h"
#include "util/Rect.h"
#include "util/Polygon.h"

#include "core/Rsyn.h"
#include "util/Bounds.h"
#include "util/FloatingPoint.h"
#include "util/DBU.h"
#include "util/Proxy.h"
#include "phy/util/DefDescriptors.h"
#include "phy/util/LefDescriptors.h"
#include "phy/util/PhysicalTypes.h"
#include "phy/util/PhysicalUtil.h"
#include "phy/util/PhysicalTransform.h"
#include "util/Exception.h"
#include "util/Json.h"

namespace Rsyn {

// std::numeric_limts<PhysicalIndex>::max() reserved to invalid initialization
// object.
typedef std::uint32_t PhysicalIndex;
static PhysicalIndex INVALID_PHYSICAL_INDEX =
    std::numeric_limits<PhysicalIndex>::max();

class PhysicalObject;

class PhysicalRoutingPointData;
class PhysicalDieData;
class PhysicalGCellData;
class PhysicalLayerData;
class PhysicalSpacingData;
class PhysicalSpacingRuleData;
class PhysicalSiteData;
class PhysicalRowData;
class PhysicalObstacleData;
class PhysicalPinLayerData;
class PhysicalPinGeometryData;
class PhysicalLibraryPinData;
class PhysicalLibraryCellData;
class PhysicalPinData;
class PhysicalNetData;
class PhysicalInstanceData;
class PhysicalRegionData;
class PhysicalGroupData;
class PhysicalViaData;
class ViaGeometryData;
class ViaRuleData;
class PhysicalSpecialWireData;
class PhysicalSpecialNetData;
class PhysicalTracksData;
class PhysicalRoutingGridData;
class LayerViaManagerData;
class PhysicalDesignData;

class PhysicalRoutingPoint;
class PhysicalDie;
class PhysicalGCell;
class PhysicalLayer;
class PhysicalSpacing;
class PhysicalSpacingRule;
class PhysicalSite;
class PhysicalRow;
class PhysicalObstacle;
class PhysicalPinLayer;
class PhysicalPinGeometry;
class PhysicalLibraryPin;
class PhysicalLibraryCell;
class PhysicalPin;
class PhysicalNet;
class PhysicalInstance;
class PhysicalPort;
class PhysicalCell;
class PhysicalModule;
class PhysicalRegion;
class PhysicalGroup;
class PhysicalVia;
class PhysicalViaGeometry;
class PhysicalViaRuleBase;
class PhysicalViaRule;
class PhysicalViaRuleGenerate;
class PhysicalSpecialWire;
class PhysicalSpecialNet;
class PhysicalTracks;
class PhysicalRoutingGrid;
class LayerViaManager;
class PhysicalDesign;

class PhysicalAttributeInitializer;
template <typename DefaultPhysicalValueType>
class PhysicalAttributeInitializerWithDefaultValue;

class PhysicalDesignObserver;
class PhysicalRouting;

}  // end namespace

// Object's Declarations (Proxies)
#include "phy/obj/decl/PhysicalRoutingPoint.h"
#include "phy/obj/decl/PhysicalDie.h"
#include "phy/obj/decl/PhysicalGCell.h"
#include "phy/obj/decl/PhysicalLayer.h"
#include "phy/obj/decl/PhysicalSpacing.h"
#include "phy/obj/decl/PhysicalSpacingRule.h"
#include "phy/obj/decl/PhysicalSite.h"
#include "phy/obj/decl/PhysicalRow.h"
#include "phy/obj/decl/PhysicalPinLayer.h"
#include "phy/obj/decl/PhysicalPinGeometry.h"
#include "phy/obj/decl/PhysicalObstacle.h"
#include "phy/obj/decl/PhysicalLibraryPin.h"
#include "phy/obj/decl/PhysicalLibraryCell.h"
#include "phy/obj/decl/PhysicalPin.h"
#include "phy/obj/decl/PhysicalNet.h"
#include "phy/obj/decl/PhysicalInstance.h"
#include "phy/obj/decl/PhysicalCell.h"
#include "phy/obj/decl/PhysicalModule.h"
#include "phy/obj/decl/PhysicalPort.h"
#include "phy/obj/decl/PhysicalRegion.h"
#include "phy/obj/decl/PhysicalGroup.h"
#include "phy/obj/decl/PhysicalVia.h"
#include "phy/obj/decl/PhysicalViaGeometry.h"
#include "phy/obj/decl/PhysicalViaRuleBase.h"
#include "phy/obj/decl/PhysicalViaRule.h"
#include "phy/obj/decl/PhysicalViaRuleGenerate.h"
#include "phy/obj/decl/PhysicalSpecialWire.h"
#include "phy/obj/decl/PhysicalSpecialNet.h"
#include "phy/obj/decl/PhysicalTracks.h"
#include "phy/obj/decl/PhysicalRoutingGrid.h"
#include "phy/obj/decl/LayerViaManager.h"
#include "phy/obj/decl/PhysicalDesign.h"

// Routing
#include "phy/PhysicalRouting.h"

// Object's Data
#include "phy/obj/data/PhysicalObject.h"
#include "phy/obj/data/PhysicalRoutingPointData.h"
#include "phy/obj/data/PhysicalDieData.h"
#include "phy/obj/data/PhysicalGCellData.h"
#include "phy/obj/data/PhysicalLayerData.h"
#include "phy/obj/data/PhysicalSpacingData.h"
#include "phy/obj/data/PhysicalSpacingRuleData.h"
#include "phy/obj/data/PhysicalSiteData.h"
#include "phy/obj/data/PhysicalRowData.h"
#include "phy/obj/data/PhysicalPinLayerData.h"
#include "phy/obj/data/PhysicalPinGeometryData.h"
#include "phy/obj/data/PhysicalObstacleData.h"
#include "phy/obj/data/PhysicalLibraryPinData.h"
#include "phy/obj/data/PhysicalLibraryCellData.h"
#include "phy/obj/data/PhysicalPinData.h"
#include "phy/obj/data/PhysicalNetData.h"
#include "phy/obj/data/PhysicalInstanceData.h"
#include "phy/obj/data/PhysicalRegionData.h"
#include "phy/obj/data/PhysicalGroupData.h"
#include "phy/obj/data/PhysicalViaData.h"
#include "phy/obj/data/ViaGeometryData.h"
#include "phy/obj/data/ViaRuleData.h"
#include "phy/obj/data/PhysicalSpecialWireData.h"
#include "phy/obj/data/PhysicalSpecialNetData.h"
#include "phy/obj/data/PhysicalTracksData.h"
#include "phy/obj/data/PhysicalRoutingGridData.h"
#include "phy/obj/data/LayerViaManagerData.h"
#include "phy/obj/data/PhysicalDesign.h"

// Physical Infrastructure
#include "phy/infra/PhysicalAttribute.h"
#include "phy/infra/PhysicalObserver.h"

// Object's Implementations
#include "phy/obj/impl/PhysicalRoutingPoint.h"
#include "phy/obj/impl/PhysicalLayer.h"
#include "phy/obj/impl/PhysicalDie.h"
#include "phy/obj/impl/PhysicalGCell.h"
#include "phy/obj/impl/PhysicalSpacing.h"
#include "phy/obj/impl/PhysicalSpacingRule.h"
#include "phy/obj/impl/PhysicalSite.h"
#include "phy/obj/impl/PhysicalRow.h"
#include "phy/obj/impl/PhysicalPinLayer.h"
#include "phy/obj/impl/PhysicalPinGeometry.h"
#include "phy/obj/impl/PhysicalObstacle.h"
#include "phy/obj/impl/PhysicalLibraryPin.h"
#include "phy/obj/impl/PhysicalLibraryCell.h"
#include "phy/obj/impl/PhysicalPin.h"
#include "phy/obj/impl/PhysicalNet.h"
#include "phy/obj/impl/PhysicalInstance.h"
#include "phy/obj/impl/PhysicalPort.h"
#include "phy/obj/impl/PhysicalCell.h"
#include "phy/obj/impl/PhysicalModule.h"
#include "phy/obj/impl/PhysicalRegion.h"
#include "phy/obj/impl/PhysicalGroup.h"
#include "phy/obj/impl/PhysicalVia.h"
#include "phy/obj/impl/PhysicalViaGeometry.h"
#include "phy/obj/impl/PhysicalViaRuleBase.h"
#include "phy/obj/impl/PhysicalViaRule.h"
#include "phy/obj/impl/PhysicalViaRuleGenerate.h"
#include "phy/obj/impl/PhysicalSpecialWire.h"
#include "phy/obj/impl/PhysicalSpecialNet.h"
#include "phy/obj/impl/PhysicalTracks.h"
#include "phy/obj/impl/PhysicalRoutingGrid.h"
#include "phy/obj/impl/LayerViaManager.h"
#include "phy/obj/impl/PhysicalDesign.h"

#endif /* PHYSICALDESIGN_PHYSICALDESIGN__H */
