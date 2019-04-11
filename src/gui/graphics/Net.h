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

#ifndef RSYN_NET_GRAPHICS_ITEM_H
#define RSYN_NET_GRAPHICS_ITEM_H

#include "gui/graphics/GraphicsItem.h"
#include "session/Session.h"

namespace Rsyn {

class Net;
class PhysicalLayer;

//! @brief A graphics item representing a cell.
class NetGraphicsItem : public GraphicsItem {
       public:
        NetGraphicsItem(Rsyn::Net net) : clsNet(net) {}

        Rsyn::Net getNet() const { return clsNet; }
        virtual Rsyn::PhysicalLayer getPhysicalLayer() const = 0;

       private:
        Rsyn::Net clsNet;
};  // end class

}  // end namespace

#endif
