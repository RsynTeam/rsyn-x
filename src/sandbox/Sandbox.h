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

#ifndef RSYN_SANDBOX_H
#define RSYN_SANDBOX_H

#include "core/Rsyn.h"

#define RSYN_SANDBOX_LIST_CHUNCK_SIZE 5

namespace Rsyn {

class SandboxNetData;
class SandboxPinData;
class SandboxArcData;
class SandboxInstanceData;
class SandboxCellData;
class SandboxPortData;
class SandboxData;

class SandboxNet;
class SandboxPin;
class SandboxArc;
class SandboxInstance;
class SandboxCell;
class SandboxPort;
class Sandbox;

class SandboxAttributeInitializer;
template <typename DefaultValueType>
class SandboxAttributeInitializerWithDefaultValue;

typedef CollectionOfGenericPinsFilteredByDirection<SandboxPin>
    CollectionOfSandboxPinsFilteredByDirection;
typedef CollectionOfGenericPins<SandboxPin> CollectionOfSandboxPins;
typedef CollectionOfGenericArcs<SandboxArc> CollectionOfSandboxArcs;
typedef CollectionOfGenericPredecessorPins<SandboxPin, SandboxNet, SandboxArc>
    CollectionOfPredecessorSandboxPins;
typedef CollectionOfGenericSuccessorPins<SandboxPin, SandboxNet, SandboxArc>
    CollectionOfSuccessorSandboxPins;

template <class Object, class Reference>
using SandboxListCollection =
    GenericListCollection<Object, Reference, RSYN_SANDBOX_LIST_CHUNCK_SIZE>;

template <class Reference>
using SandboxReferenceListCollection =
    GenericReferenceListCollection<Reference, RSYN_SANDBOX_LIST_CHUNCK_SIZE>;

}  // end namespace

// =============================================================================
// Objects
// =============================================================================

// Object's Declarations (Proxies)
#include "sandbox/obj/decl/Object.h"
#include "sandbox/obj/decl/Net.h"
#include "sandbox/obj/decl/Pin.h"
#include "sandbox/obj/decl/Arc.h"
#include "sandbox/obj/decl/Instance.h"
#include "sandbox/obj/decl/Cell.h"
#include "sandbox/obj/decl/Port.h"
#include "sandbox/obj/decl/Sandbox.h"

// Object's Data
#include "sandbox/obj/data/Object.h"
#include "sandbox/obj/data/Net.h"
#include "sandbox/obj/data/Pin.h"
#include "sandbox/obj/data/Arc.h"
#include "sandbox/obj/data/Instance.h"
#include "sandbox/obj/data/Cell.h"
#include "sandbox/obj/data/Port.h"
#include "sandbox/obj/data/Sandbox.h"

// Infra
#include "sandbox/infra/Attribute.h"

// Object's Implementations
#include "sandbox/obj/impl/Object.h"
#include "sandbox/obj/impl/Net.h"
#include "sandbox/obj/impl/Pin.h"
#include "sandbox/obj/impl/Arc.h"
#include "sandbox/obj/impl/Instance.h"
#include "sandbox/obj/impl/Cell.h"
#include "sandbox/obj/impl/Port.h"
#include "sandbox/obj/impl/Sandbox.h"

#endif
