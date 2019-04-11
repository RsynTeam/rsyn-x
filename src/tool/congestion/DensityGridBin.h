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
 * File:   DensityGridBin.h
 * Author: jucemar
 *
 * Created on 16 de Setembro de 2016, 18:14
 */

#ifndef RSYN_DENSITY_GRID_BIN_H
#define RSYN_DENSITY_GRID_BIN_H

#include "util/Bounds.h"
#include "DensityGridBlockage.h"
#include "DensityGridType.h"

#include <vector>
#include <unordered_map>

namespace Rsyn {

class DensityGridBin {
       public:
        Bounds clsBounds;
        std::vector<Bounds> clsRows;
        std::vector<int> clsNumPins;
        std::vector<Rsyn::DensityGridBlockage> clsBlockages;
        // protected:
        std::vector<DBU> clsAreas;
        std::unordered_map<std::string, int> clsMapBlockages;

        void addArea(const AreaType type, const DBU area) {
                clsAreas[type] += area;
        }  // end method

        void removeArea(const AreaType type, const DBU area) {
                clsAreas[type] -= area;
        }  // end method

        void setArea(const AreaType type, const DBU area) {
                clsAreas[type] = area;
        }  // end method

        void clearArea(const AreaType type) {
                clsAreas[type] = 0.0;
        }  // end method

        void addPins(const PinType type, const int numPins) {
                clsNumPins[type] += numPins;
        }  // end method

        void addPin(const PinType type) { clsNumPins[type]++; }  // end method

        void removePins(const PinType type, const int numPins) {
                clsNumPins[type] -= numPins;
        }  // end method

        void removePin(const PinType type) {
                clsNumPins[type]--;
        }  // end method

        void setPins(const PinType type, int numPins) {
                clsNumPins[type] = numPins;
        }  // end method

        void clearPins(const PinType type) {
                clsNumPins[type] = 0;
        }  // end method

        void resetBounds() { clsBounds.clear(); }  // end method

       public:
        DensityGridBin() {
                clsAreas.resize(NUM_AREAS, 0.0);
                clsNumPins.resize(NUM_PINS, 0);
        }  // end method

        const Bounds& getBounds() const { return clsBounds; }  // end method

        DBUxy getPos() const { return clsBounds[LOWER]; }  // end method

        DBU getPos(const Dimension dim) const {
                return clsBounds[LOWER][dim];
        }  // end method

        DBUxy getCoordinate(const Boundary bound) const {
                return clsBounds[bound];
        }  // end method

        DBU getCoordinate(const Boundary bound, const Dimension dim) const {
                return clsBounds[bound][dim];
        }  // end method

        DBU getArea(const AreaType type) const {
                return clsAreas[type];
        }  // end method

        bool hasArea(const AreaType type) const {
                return clsAreas[type] > 0;
        }  // end method

        int getNumPins(const PinType type) const {
                return clsNumPins[type];
        }  // end method

        const std::vector<Rsyn::DensityGridBlockage>& allDensityGridBlockage()
            const {
                return clsBlockages;
        }  // end method

        const std::vector<Bounds>& allRows() const {
                return clsRows;
        }  // end method
};         // end class

}  // end namespace

#endif /* RSYN_DENSITY_GRID_BIN_H */
