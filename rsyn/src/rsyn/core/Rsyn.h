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
 
#ifndef RSYN_H
#define RSYN_H

#include <cstdint>
#include <array>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <vector>
#include <bitset>
#include <memory>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>
#include <type_traits>
#include <cassert>
#include <stdexcept>

#include <rsyn/core/infra/List.h>
#include <rsyn/core/infra/RangeBasedLoop.h>
#include <rsyn/core/infra/Exception.h>

#include <rsyn/util/Proxy.h>
#include <rsyn/util/TristateFlag.h>

#define RSYN_KERNEL
#define RSYN_LIST_CHUNCK_SIZE 1000

#define RSYN_FRIEND_OF_GENERIC_LIST_COLLECTION \
template<class Object, class Reference, unsigned int CHUNK_SIZE> \
friend class GenericListCollection;

namespace Rsyn {

class ObjectData;
class NetData;
class PinData;
class ArcData;
class InstanceData;
class CellData;
class PortData;
class ModuleData;
class LibraryPinData;
class LibraryArcData;
class LibraryCellData;
class LibraryModuleData;
class DesignData;

class NetTagData;
class InstanceTagData;
class LibraryCellTagData;

class Net;
class Pin;
class Arc;
class Instance;
class Cell;
class Port;
class Module;
class LibraryPin;
class LibraryArc;
class LibraryCell;
class LibraryModule;
class Design;

class AttributeInitializer;
template<typename DefaultValueType> class AttributeInitializerWithDefaultValue;

class Observer;

template<class Object, class Reference, unsigned int CHUNK_SIZE> class GenericListCollection;
template<class Reference, unsigned int CHUNK_SIZE> class GenericReferenceListCollection;

template<class Object, class Reference>
using ListCollection = GenericListCollection<Object, Reference, RSYN_LIST_CHUNCK_SIZE>;

template<class Reference>
using ReferenceListCollection = GenericReferenceListCollection<Reference, RSYN_LIST_CHUNCK_SIZE>;

class CollectionOfLibraryPinsFilteredByDirection;
class CollectionOfLibraryPins;
class CollectionOfLibraryArcs;
class CollectionOfLibraryArcsToLibraryPin;
class CollectionOfLibraryArcsFromLibraryPin;

template<class PinType>
class CollectionOfGenericPinsFilteredByDirection;
template<class PinType>
class CollectionOfGenericPins;
template<class ArcType>
class CollectionOfGenericArcs;
template<class PinType, class NetType, class ArcType>
class CollectionOfGenericPredecessorPins;
template<class PinType, class NetType, class ArcType>
class CollectionOfGenericSuccessorPins;

typedef CollectionOfGenericPinsFilteredByDirection<Pin>
		CollectionOfPinsFilteredByDirection;
typedef CollectionOfGenericPins<Pin>
		CollectionOfPins;
typedef CollectionOfGenericArcs<Arc>
		CollectionOfArcs;
typedef CollectionOfGenericPredecessorPins<Pin, Net, Arc>
		CollectionOfPredecessorPins;
typedef CollectionOfGenericSuccessorPins<Pin, Net, Arc>
		CollectionOfSuccessorPins;

typedef std::uint32_t Index;
typedef int TopologicalIndex;

enum ObjectType {
	OBJECT_TYPE_INVALID = -1,

	OBJECT_TYPE_NET,
	OBJECT_TYPE_PIN,
	OBJECT_TYPE_ARC,

	OBJECT_TYPE_INSTANCE,
	OBJECT_TYPE_CELL,
	OBJECT_TYPE_PORT,
	OBJECT_TYPE_MODULE,

	OBJECT_TYPE_LIBRARY_PIN,
	OBJECT_TYPE_LIBRARY_ARC,
	OBJECT_TYPE_LIBRARY_CELL,

	NUM_OBJECT_TYPES
}; // end enum

enum InstanceType {
	UNKNOWN_INSTANCE_TYPE = 0,  // used in bit fields (needs to start at zero)
	
	CELL,
	PORT,
	MODULE,

	NUM_INSTANCE_TYPES
}; // end enum

enum ArcType {
	UNKNOWN_ARC_TYPE = 0, // used in bit fields (needs to start at zero)
	
	INSTANCE_ARC,
	NET_ARC,

	NUM_ARC_TYPES
}; // end enum

enum TraverseType {
	INVALID_TRAVERSE_TYPE = -1,

	FORWARD,  // from inputs to outputs (topological order)
	BACKWARD, // from outputs to inputs (reverse topological order)

	NUM_TRAVERSE_TYPES
}; // end enum

enum Direction { // TODO: Many rename to SignalDirection
	UNKNOWN_DIRECTION  = 0, // used in bit fields (needs to start at zero)

	IN = 1,
	SINK = 1,

	OUT = 2,
	DRIVER = 2,

	BIDIRECTIONAL = 3,

	NUM_SIGNAL_DIRECTIONS
}; // end enum

// =============================================================================
// Type Requests
// =============================================================================

enum BufferType {
	NON_INVERTING,
	INVERTING,
	ANY_BUFFER_TYPE
}; // end enum

enum TieType {
	TIE_LOW,
	TIE_HIGH,
	ANY_TIE_TYPE
}; // end enum

// =============================================================================
// Tags
// =============================================================================

class TagNotSpecifiedException : public std::runtime_error {
public:
	TagNotSpecifiedException(const std::string &tagName) : 
		std::runtime_error("Trying to use the value of the tag '" + tagName + "', which was not set.") {}
}; // end class

enum LogicTypeTag {
	LOGIC_TYPE_TAG_NOT_SPECIFIED,

	LOGIC_TYPE_TAG_NONE,
	LOGIC_TYPE_TAG_COMBINATIONAL,
	LOGIC_TYPE_TAG_SEQUENTIAL
}; // end enum

enum BufferTypeTag {
	BUFFER_TYPE_TAG_NOT_SPECIFIED,

	BUFFER_TYPE_TAG_NONE,
	BUFFER_TYPE_TAG_NON_INVERTING,
	BUFFER_TYPE_TAG_INVERTING
}; // end enum

enum TieTypeTag {
	TIE_TYPE_TAG_NOT_SPECIFIED,

	TIE_TYPE_TAG_NONE,
	TIE_TYPE_TAG_LOW,
	TIE_TYPE_TAG_HIGH
}; // end enum

enum NetTypeTag {
	NET_TYPE_TAG_NOT_SPECIFIED,

	NET_TYPE_TAG_NONE,
	NET_TYPE_TAG_DATA,
	NET_TYPE_TAG_CLOCK,
	NET_TYPE_TAG_SCAN,
	NET_TYPE_TAG_RESET
}; // end enum

// =============================================================================
// Events
// =============================================================================

enum EventType {
	EVENT_DESTRUCTION,
	EVENT_POST_INSTANCE_CREATE,
	EVENT_PRE_INSTANCE_REMOVE,
	EVENT_POST_NET_CREATE,
	EVENT_PRE_NET_REMOVE,
	EVENT_POST_CELL_REMAP,
	EVENT_POST_PIN_CONNECT,
	EVENT_PRE_PIN_DISCONNECT,

	NUM_EVENTS
}; // end enum

// =============================================================================
// Exception
// =============================================================================

class SafeModeException : public Exception {
public:
	SafeModeException(const std::string &msg) : Exception(msg) {}
}; // end class

// =============================================================================
// Constants
// =============================================================================

// Some constants...
enum {
	TOPOLOGICAL_SORTING_SMALL_GAP = 10,
	TOPOLOGICAL_SORTING_LARGE_GAP = 1000
}; // end enum

static const std::string NullName = "<null>";
static const TopologicalIndex MIN_TOPOLOGICAL_INDEX =
	-std::numeric_limits<TopologicalIndex>::infinity();

} // end namespace

// =============================================================================
// Descriptors
// =============================================================================

#include "rsyn/core/dscp/LibraryCell.h"
#include "rsyn/core/dscp/LibraryModule.h"

// =============================================================================
// Global
// =============================================================================

namespace Rsyn {

// -----------------------------------------------------------------------------

class Global {
public:
	
	static
	const std::string &
	getDirectionName(const Direction direction) {
		static const std::array<std::string, NUM_SIGNAL_DIRECTIONS> NAMES =
			{ "UNKNOWN", "INPUT", "OUTPUT", "BIDIRECTIONAL" };
		return NAMES[direction];
	} // end method

	static
	const std::string &
	getDirectionShortName(const Direction direction) {
		static const std::array<std::string, NUM_SIGNAL_DIRECTIONS> NAMES =
			{ "UNKNOWN", "IN", "OUT", "BIDI" };
		return NAMES[direction];
	} // end method

	static
	const std::string &
	getDirectionSingleCharName(const Direction direction) {
		static const std::array<std::string, NUM_SIGNAL_DIRECTIONS> NAMES =
			{ "?", "I", "O", "B" };
		return NAMES[direction];	
	} // end method
	
	static
	const Direction 
	getReverseDirection(const Direction direction) {
		static const Direction DIRECTION_REVERSED[NUM_SIGNAL_DIRECTIONS] = {
			UNKNOWN_DIRECTION, OUT, IN, BIDIRECTIONAL };
		return DIRECTION_REVERSED[direction];
	} // end method	
	
}; // end class

} // end namespace

// =============================================================================
// Objects
// =============================================================================

// Object's Declarations (Proxies)
#include "rsyn/core/obj/decl/Object.h"
#include "rsyn/core/obj/decl/Net.h"
#include "rsyn/core/obj/decl/Pin.h"
#include "rsyn/core/obj/decl/Arc.h"
#include "rsyn/core/obj/decl/Instance.h"
#include "rsyn/core/obj/decl/Cell.h"
#include "rsyn/core/obj/decl/Port.h"
#include "rsyn/core/obj/decl/Module.h"
#include "rsyn/core/obj/decl/LibraryPin.h"
#include "rsyn/core/obj/decl/LibraryArc.h"
#include "rsyn/core/obj/decl/LibraryCell.h"
#include "rsyn/core/obj/decl/LibraryModule.h"
#include "rsyn/core/obj/decl/Design.h"

// Object's Data
#include "rsyn/core/obj/data/Object.h"
#include "rsyn/core/obj/data/Net.h"
#include "rsyn/core/obj/data/Pin.h"
#include "rsyn/core/obj/data/Arc.h"
#include "rsyn/core/obj/data/Instance.h"
#include "rsyn/core/obj/data/Cell.h"
#include "rsyn/core/obj/data/Port.h"
#include "rsyn/core/obj/data/Module.h"
#include "rsyn/core/obj/data/LibraryPin.h"
#include "rsyn/core/obj/data/LibraryArc.h"
#include "rsyn/core/obj/data/LibraryCell.h"
#include "rsyn/core/obj/data/LibraryModule.h"
#include "rsyn/core/obj/data/Design.h"

// Infra
#include "rsyn/core/infra/Attribute.h"
#include "rsyn/core/infra/Observer.h"

// Object's Implementations
#include "rsyn/core/obj/impl/Object.h"
#include "rsyn/core/obj/impl/Net.h"
#include "rsyn/core/obj/impl/Pin.h"
#include "rsyn/core/obj/impl/Arc.h"
#include "rsyn/core/obj/impl/Instance.h"
#include "rsyn/core/obj/impl/Cell.h"
#include "rsyn/core/obj/impl/Port.h"
#include "rsyn/core/obj/impl/Module.h"
#include "rsyn/core/obj/impl/LibraryPin.h"
#include "rsyn/core/obj/impl/LibraryArc.h"
#include "rsyn/core/obj/impl/LibraryCell.h"
#include "rsyn/core/obj/impl/LibraryModule.h"
#include "rsyn/core/obj/impl/Design.h"

// =============================================================================
// Hashes
// =============================================================================

namespace std {
#define RSYN_CREATE_HASH(OBJ) \
template <> \
struct hash<OBJ> { \
	size_t operator()(const OBJ &obj) const { \
		return hash<Rsyn::Proxy<OBJ##Data>>()(obj); \
	} \
};

RSYN_CREATE_HASH(Rsyn::Net);
RSYN_CREATE_HASH(Rsyn::Pin);
RSYN_CREATE_HASH(Rsyn::Arc);
RSYN_CREATE_HASH(Rsyn::Instance);
RSYN_CREATE_HASH(Rsyn::LibraryPin);
RSYN_CREATE_HASH(Rsyn::LibraryArc);
RSYN_CREATE_HASH(Rsyn::LibraryCell);
} // end namespace

#endif