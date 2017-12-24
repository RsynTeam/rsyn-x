/* Copyright 2014-2017 Rsyn
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
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PhysicalDesign.h
 * Author: jucemar
 *
 * Created on 12 de Setembro de 2016, 20:09
 */

#ifndef PHYSICALDESIGN_PHYSICALDESIGN__H
#define PHYSICALDESIGN_PHYSICALDESIGN__H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <stddef.h>
#include <algorithm>
#include <limits>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>

#include "rsyn/core/Rsyn.h"
#include "rsyn/util/Bounds.h"
#include "rsyn/util/FloatingPoint.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/Proxy.h"
#include "rsyn/phy/util/DefDescriptors.h"
#include "rsyn/phy/util/LefDescriptors.h"
#include "rsyn/phy/util/PhysicalTypes.h"
#include "rsyn/phy/util/PhysicalUtil.h"
#include "rsyn/phy/util/PhysicalTransform.h"
#include "rsyn/util/Exception.h"
#include "rsyn/3rdparty/json/json.hpp"

namespace Rsyn {

// std::numeric_limts<PhysicalIndex>::max() reserved to invalid initialization object.
typedef std::uint32_t PhysicalIndex;
static PhysicalIndex INVALID_PHYSICAL_INDEX = std::numeric_limits<PhysicalIndex>::max();

typedef nlohmann::json Json;

// Boost Polygon typedef 
namespace boostGeometry = boost::geometry;
typedef boostGeometry::model::point<DBU, 2, boostGeometry::cs::cartesian> PhysicalPolygonPoint;
// The first type is the polygon point definition. The second type disable clockwise direction of polygon. 
// The third type disable include again at polygon end the first point. Polygon is open. 
typedef boostGeometry::model::polygon<PhysicalPolygonPoint, false, false> PhysicalPolygon; 

class PhysicalObject;

class PhysicalRoutingPointData;
class PhysicalDieData;
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
class PhysicalViaLayerData;
class PhysicalViaData;
class PhysicalWireSegmentData;
class PhysicalWireData;
class PhysicalSpecialWireData;
class PhysicalSpecialNetData;
class PhysicalTrackData;
class PhysicalDesignData;

class PhysicalRoutingPoint;
class PhysicalDie;
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
class PhysicalViaLayer;
class PhysicalVia;
class PhysicalWireSegment;
class PhysicalWire;
class PhysicalSpecialWire;
class PhysicalSpecialNet;
class PhysicalTrack;
class PhysicalDesign;


class PhysicalAttributeInitializer;
template<typename DefaultPhysicalValueType>
class PhysicalAttributeInitializerWithDefaultValue;

class PhysicalObserver;

} // end namespace 


// Object's Declarations (Proxies)
#include "rsyn/phy/obj/decl/PhysicalRoutingPoint.h"
#include "rsyn/phy/obj/decl/PhysicalDie.h"
#include "rsyn/phy/obj/decl/PhysicalLayer.h"
#include "rsyn/phy/obj/decl/PhysicalSpacing.h"
#include "rsyn/phy/obj/decl/PhysicalSpacingRule.h"
#include "rsyn/phy/obj/decl/PhysicalSite.h"
#include "rsyn/phy/obj/decl/PhysicalRow.h"
#include "rsyn/phy/obj/decl/PhysicalPinLayer.h"
#include "rsyn/phy/obj/decl/PhysicalPinGeometry.h"
#include "rsyn/phy/obj/decl/PhysicalObstacle.h"
#include "rsyn/phy/obj/decl/PhysicalLibraryPin.h"
#include "rsyn/phy/obj/decl/PhysicalLibraryCell.h"
#include "rsyn/phy/obj/decl/PhysicalPin.h"
#include "rsyn/phy/obj/decl/PhysicalNet.h"
#include "rsyn/phy/obj/decl/PhysicalInstance.h"
#include "rsyn/phy/obj/decl/PhysicalCell.h"
#include "rsyn/phy/obj/decl/PhysicalModule.h"
#include "rsyn/phy/obj/decl/PhysicalPort.h"
#include "rsyn/phy/obj/decl/PhysicalRegion.h"
#include "rsyn/phy/obj/decl/PhysicalGroup.h"
#include "rsyn/phy/obj/decl/PhysicalViaLayer.h"
#include "rsyn/phy/obj/decl/PhysicalVia.h"
#include "rsyn/phy/obj/decl/PhysicalWireSegment.h"
#include "rsyn/phy/obj/decl/PhysicalWire.h"
#include "rsyn/phy/obj/decl/PhysicalSpecialWire.h"
#include "rsyn/phy/obj/decl/PhysicalSpecialNet.h"
#include "rsyn/phy/obj/decl/PhysicalTrack.h"
#include "rsyn/phy/obj/decl/PhysicalDesign.h"

// Object's Data
#include "rsyn/phy/obj/data/PhysicalObject.h"
#include "rsyn/phy/obj/data/PhysicalRoutingPointData.h"
#include "rsyn/phy/obj/data/PhysicalDieData.h"
#include "rsyn/phy/obj/data/PhysicalLayerData.h"
#include "rsyn/phy/obj/data/PhysicalSpacingData.h"
#include "rsyn/phy/obj/data/PhysicalSpacingRuleData.h"
#include "rsyn/phy/obj/data/PhysicalSiteData.h"
#include "rsyn/phy/obj/data/PhysicalRowData.h"
#include "rsyn/phy/obj/data/PhysicalPinLayerData.h"
#include "rsyn/phy/obj/data/PhysicalPinGeometryData.h"
#include "rsyn/phy/obj/data/PhysicalObstacleData.h"
#include "rsyn/phy/obj/data/PhysicalLibraryPinData.h"
#include "rsyn/phy/obj/data/PhysicalLibraryCellData.h"
#include "rsyn/phy/obj/data/PhysicalPinData.h"
#include "rsyn/phy/obj/data/PhysicalNetData.h"
#include "rsyn/phy/obj/data/PhysicalInstanceData.h"
#include "rsyn/phy/obj/data/PhysicalRegionData.h"
#include "rsyn/phy/obj/data/PhysicalGroupData.h"
#include "rsyn/phy/obj/data/PhysicalViaLayerData.h"
#include "rsyn/phy/obj/data/PhysicalViaData.h"
#include "rsyn/phy/obj/data/PhysicalWireSegmentData.h"
#include "rsyn/phy/obj/data/PhysicalWireData.h"
#include "rsyn/phy/obj/data/PhysicalSpecialWireData.h"
#include "rsyn/phy/obj/data/PhysicalSpecialNetData.h"
#include "rsyn/phy/obj/data/PhysicalTrackData.h"
#include "rsyn/phy/obj/data/PhysicalDesign.h"

// Physical Infrastructure
#include "rsyn/phy/infra/PhysicalAttribute.h"
#include "rsyn/phy/infra/PhysicalObserver.h"

// Object's Implementations
#include "rsyn/phy/obj/impl/PhysicalRoutingPoint.h"
#include "rsyn/phy/obj/impl/PhysicalLayer.h"
#include "rsyn/phy/obj/impl/PhysicalDie.h"
#include "rsyn/phy/obj/impl/PhysicalSpacing.h"
#include "rsyn/phy/obj/impl/PhysicalSpacingRule.h"
#include "rsyn/phy/obj/impl/PhysicalSite.h"
#include "rsyn/phy/obj/impl/PhysicalRow.h"
#include "rsyn/phy/obj/impl/PhysicalPinLayer.h"
#include "rsyn/phy/obj/impl/PhysicalPinGeometry.h"
#include "rsyn/phy/obj/impl/PhysicalObstacle.h"
#include "rsyn/phy/obj/impl/PhysicalLibraryPin.h"
#include "rsyn/phy/obj/impl/PhysicalLibraryCell.h"
#include "rsyn/phy/obj/impl/PhysicalPin.h"
#include "rsyn/phy/obj/impl/PhysicalNet.h"
#include "rsyn/phy/obj/impl/PhysicalInstance.h"
#include "rsyn/phy/obj/impl/PhysicalPort.h"
#include "rsyn/phy/obj/impl/PhysicalCell.h"
#include "rsyn/phy/obj/impl/PhysicalModule.h"
#include "rsyn/phy/obj/impl/PhysicalRegion.h"
#include "rsyn/phy/obj/impl/PhysicalGroup.h"
#include "rsyn/phy/obj/impl/PhysicalViaLayer.h"
#include "rsyn/phy/obj/impl/PhysicalVia.h"
#include "rsyn/phy/obj/impl/PhysicalWireSegment.h"
#include "rsyn/phy/obj/impl/PhysicalWire.h"
#include "rsyn/phy/obj/impl/PhysicalSpecialWire.h"
#include "rsyn/phy/obj/impl/PhysicalSpecialNet.h"
#include "rsyn/phy/obj/impl/PhysicalTrack.h"
#include "rsyn/phy/obj/impl/PhysicalDesign.h"




#endif /* PHYSICALDESIGN_PHYSICALDESIGN__H */

