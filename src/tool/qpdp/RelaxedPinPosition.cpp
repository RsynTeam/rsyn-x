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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RelaxedPinPosition.cpp
 * Author: jucemar
 *
 * Created on 24 de Fevereiro de 2017, 20:54
 */

#include "RelaxedPinPosition.h"

namespace ICCAD15 {

// Get the net bounding box using the cell positions. However, for macro-
// blocks the pin position is used instead.
void RelaxedPinPosition::getNetBoundingBoxUsingRelaxedPinPositions(
    Rsyn::Net net, DBU &xmin, DBU &ymin, DBU &xmax, DBU &ymax) {
        xmin = +std::numeric_limits<DBU>::infinity();
        ymin = +std::numeric_limits<DBU>::infinity();
        xmax = -std::numeric_limits<DBU>::infinity();
        ymax = -std::numeric_limits<DBU>::infinity();

        for (Rsyn::Pin pin : net.allPins()) {
                Rsyn::Cell cell = pin.getInstance().asCell();

                const DBUxy position =
                    clsPhysicalDesign.getRelaxedPinPosition(pin);

                xmin = std::min(xmin, position.x);
                ymin = std::min(ymin, position.y);

                xmax = std::max(xmax, position.x);
                ymax = std::max(ymax, position.y);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

// Get the boundary cells of the net.
void RelaxedPinPosition::getNetBoundaryPinsUsingRelaxedPinPositions(
    Rsyn::Net net, Rsyn::Pin &left, Rsyn::Pin &right, Rsyn::Pin &bottom,
    Rsyn::Pin &top) {
        left = nullptr;
        right = nullptr;
        bottom = nullptr;
        top = nullptr;

        double xmin = +std::numeric_limits<double>::infinity();
        double ymin = +std::numeric_limits<double>::infinity();
        double xmax = -std::numeric_limits<double>::infinity();
        double ymax = -std::numeric_limits<double>::infinity();

        Rsyn::Pin last = nullptr;

        for (Rsyn::Pin pin : net.allPins()) {
                const DBUxy position =
                    clsPhysicalDesign.getRelaxedPinPosition(pin);

                if (position.x < xmin) {
                        left = pin;
                        xmin = position.x;
                }  // end if

                if (position.y < ymin) {
                        bottom = pin;
                        ymin = position.y;
                }  // end if

                if (position.x > xmax) {
                        right = pin;
                        xmax = position.x;
                }  // end if

                if (position.y > ymax) {
                        top = pin;
                        ymax = position.y;
                }  // end if

                last = pin;
        }  // end for

        // Handle degenerate cases where all pins are at the same position.
        // In this case, force the boundary pins to be different.
        if (left == right) {
                right = last;
        }
        if (bottom == top) {
                top = last;
        }
}  // end method

// -----------------------------------------------------------------------------

void RelaxedPinPosition::getNetBoundaryPinsUsingRelaxedPinPositions(
    Rsyn::Net net, const Dimension dim, Rsyn::Pin &lower, Rsyn::Pin &upper) {
        Rsyn::Pin left;
        Rsyn::Pin right;
        Rsyn::Pin bottom;
        Rsyn::Pin top;
        getNetBoundaryPinsUsingRelaxedPinPositions(net, left, right, bottom,
                                                   top);

        lower = dim == X ? left : bottom;
        upper = dim == X ? right : top;
}  // end method

// -----------------------------------------------------------------------------

void RelaxedPinPosition::getNetPinsSortedByRelaxedPinPositions(
    Rsyn::Net net, const Dimension dimension,
    std::vector<std::tuple<double, Rsyn::Pin>> &pins) {
        pins.resize(net.getNumPins());

        int index = 0;
        for (Rsyn::Pin pin : net.allPins()) {
                pins[index++] = std::make_tuple(
                    clsPhysicalDesign.getRelaxedPinPosition(pin)[dimension],
                    pin);
        }  // end
        std::sort(pins.begin(), pins.end());
}  // end method

}  // end namespace
