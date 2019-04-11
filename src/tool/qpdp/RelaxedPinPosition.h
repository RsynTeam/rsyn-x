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
 * File:   RelaxedPinPosition.h
 * Author: jucemar
 *
 * Created on 24 de Fevereiro de 2017, 20:54
 */

#ifndef RELAXEDPINPOSITION_H
#define RELAXEDPINPOSITION_H

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
namespace ICCAD15 {

// Jucemar Monteiro 2017/02/24
// Removed relaxed pin position of the PhysicalService
class RelaxedPinPosition {
       protected:
        Rsyn::PhysicalDesign clsPhysicalDesign;

       public:
        RelaxedPinPosition() {}
        RelaxedPinPosition(Rsyn::PhysicalDesign phDsg)
            : clsPhysicalDesign(phDsg) {}
        virtual ~RelaxedPinPosition() {}

        // Get the net bounding box using the cell positions. However, for
        // macro-
        // blocks the pin position is used instead.
        void getNetBoundingBoxUsingRelaxedPinPositions(Rsyn::Net net, DBU &xmin,
                                                       DBU &ymin, DBU &xmax,
                                                       DBU &ymax);

        // Get the boundary cells of the net.
        void getNetBoundaryPinsUsingRelaxedPinPositions(Rsyn::Net net,
                                                        Rsyn::Pin &left,
                                                        Rsyn::Pin &right,
                                                        Rsyn::Pin &bottom,
                                                        Rsyn::Pin &top);

        void getNetBoundaryPinsUsingRelaxedPinPositions(Rsyn::Net net,
                                                        const Dimension dim,
                                                        Rsyn::Pin &lower,
                                                        Rsyn::Pin &upper);

        // Get the pins of net sorted by their (relaxed) position.
        // If there's a tie in the pin position, the internal pin ordering
        // provided
        // by Rsyn is used.
        //
        //  -------------------
        // | 0 | 1 | 2 |...| n |
        //  -------------------
        // 0        : lower boundary pin
        // [1, n-1] : inner pins
        // n        : upper boundary pin

        void getNetPinsSortedByRelaxedPinPositions(
            Rsyn::Net net, const Dimension dimension,
            std::vector<std::tuple<double, Rsyn::Pin>> &pins);

};  // end class

}  // end namespace

#endif /* RELAXEDPINPOSITION_H */
