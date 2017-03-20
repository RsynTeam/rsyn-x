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
 


#ifndef PHYSICALDESIGN_PHYSICALUTIL_H
#define PHYSICALDESIGN_PHYSICALUTIL_H

#include <string>

#include "rsyn/phy/util/PhysicalTypes.h"

namespace Rsyn {


PhysicalLayerType getPhysicalLayerType(std::string type);

std::string getPhysicalLayerType(PhysicalLayerType type);

PhysicalLayerDirection getPhysicalLayerDirection(std::string direction);

std::string getPhysicalLayerDirection(PhysicalLayerDirection direction);

PhysicalOrientation getPhysicalOrientation(const std::string &orientation);

std::string getPhysicalOrientation(const PhysicalOrientation orientation);

PhysicalMacroClass getPhysicalMacroClass(const std::string & macroClass);

std::string getPhysicalMacroClass(const PhysicalMacroClass macroClass);

Rsyn::PhysicalSymmetry getPhysicalSymmetry(const std::string &rowSymmetry);

std::string getPhysicalSymmetry(const Rsyn::PhysicalSymmetry rowSymmetry);

Rsyn::PhysicalSiteClass getPhysicalSiteClass(const std::string & siteClass);

std::string getPhysicalSiteClass( const Rsyn::PhysicalSiteClass siteClass);

Rsyn::PhysicalPinDirection getPhysicalPinDirection ( const std::string &direction);

std::string getPhysicalPinDirection (const Rsyn::PhysicalPinDirection pinDirection);

Rsyn::PhysicalPinPortClass getPhysicalPinPortClass ( const std::string & portClass);

std::string getPhysicalPinPortClass ( Rsyn::PhysicalPinPortClass portClass);

std::string getPhysicalGeneratedNamePrefix() ;

std::string getPhysicalInvalidPrefix();

std::string getPhysicalInvalidName();

std::string getPhysicalNullName();



// *****************************************************************************
// *
// *         Overloading operator<<
// *
// *
// *****************************************************************************

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const Rsyn::PhysicalSymmetry symmetry) {
    return out << getPhysicalSymmetry(symmetry);
} // end operator overloading 

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const Rsyn::PhysicalLayerType layerType) {
    return out << getPhysicalLayerType(layerType);
} // end operator overloading 

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const Rsyn::PhysicalLayerDirection direction) {
    return out << getPhysicalLayerDirection(direction);
} // end operator overloading 

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const Rsyn::PhysicalOrientation orientation) {
    return out << getPhysicalOrientation(orientation);
} // end operator overloading 

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const Rsyn::PhysicalMacroClass macroClass) {
    return out << getPhysicalMacroClass(macroClass);
} // end operator overloading 

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const Rsyn::PhysicalSiteClass siteClass) {
    return out << getPhysicalSiteClass(siteClass);
} // end operator overloading 

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const Rsyn::PhysicalPinDirection pinDirection) {
    return out << getPhysicalPinDirection(pinDirection);
} // end operator overloading 

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& out, const Rsyn::PhysicalPinPortClass pinPortClass) {
    return out << getPhysicalPinPortClass(pinPortClass);
} // end operator overloading 

// -----------------------------------------------------------------------------

} // end namespace

#endif /* PHYSICALDESIGN_PHYSICALUTIL_H */

