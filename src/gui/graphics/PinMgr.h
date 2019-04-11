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

#ifndef RSYN_QT_PIN_MGR_H
#define RSYN_QT_PIN_MGR_H

#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include <QPainterPath>

namespace Rsyn {

class QtPinMgr {
       public:
        static void create();
        static QtPinMgr *get() { return instance; }

        const QPainterPath &getShape(Rsyn::Pin pin) const {
                Rsyn::LibraryPin lpin = pin.getLibraryPin();
                if (lpin) {
                        Rsyn::PhysicalCell physicalCell =
                            physicalDesign.getPhysicalCell(pin);
                        return getShape(lpin, physicalCell.getOrientation());
                } else {
                        return EMPTY_SHAPE;
                }  // end else
        }          // end method

        const QPainterPath &getShape(
            Rsyn::LibraryPin lpin,
            const Rsyn::PhysicalOrientation &orientation) const {
                return pins[lpin].shapes[orientation];
        }  // end method

       private:
        struct QtLibraryPin {
                QPainterPath shapes[Rsyn::NUM_PHY_ORIENTATION];
        };

        QtPinMgr();
        QtPinMgr(const QtPinMgr &mgr);

        static QPainterPath createShape(
            Rsyn::LibraryPin lpin,
            const Rsyn::PhysicalOrientation &orientation);

        Rsyn::PhysicalDesign physicalDesign;
        Rsyn::Attribute<Rsyn::LibraryPin, QtLibraryPin> pins;

        static QtPinMgr *instance;
        const QPainterPath EMPTY_SHAPE;
};  // end class

}  // end namespace

#endif