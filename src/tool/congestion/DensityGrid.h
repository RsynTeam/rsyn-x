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
 * File:   DensityGrid.h
 * Author: jucemar
 *
 * Created on 16 de Setembro de 2016, 18:35
 */

#ifndef RSYN_DENSITY_GRID
#define RSYN_DENSITY_GRID

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Service.h"
#include "session/Session.h"

#include "DensityGridBin.h"
#include "DensityGridBlockage.h"

#include <vector>

namespace Rsyn {

class DensityGrid : public Rsyn::Service {
       protected:
        Design clsDesign;
        PhysicalDesign clsPhDesign;
        Module clsModule;
        PhysicalModule clsPhModule;

        std::vector<Rsyn::DensityGridBin> clsBins;

        std::vector<int> clsMaxAreaBin;  // stores the bin index that have the
                                         // highest area by area type
        std::vector<int> clsMaxPinBin;   // stores the bin index that have the
                                         // highest number of pins by pin type

        DBUxy clsBinSize;

        int clsNumCols = 0;
        int clsNumRows = 0;

        // Beta is the number of row heights which is set to compute bin
        // dimensions.
        DBU clsBeta = 9;

        double clsTargetDensity = 1.0;

        /* density profiling parameters */
        const double clsAlpha = 1.0;
        const double clsBinAreaThreshold = 0.2;
        const double clsFreeSpaceThreshold = 0.2;
        const double clsAbu2Weight = 10.0;
        const double clsAbu5Weight = 4.0;
        const double clsAbu10Weight = 2.0;
        const double clsAbu20Weight = 1.0;
        double clsAbu1 = 0.0;
        double clsAbu2 = 0.0;
        double clsAbu5 = 0.0;
        double clsAbu10 = 0.0;
        double clsAbu20 = 0.0;
        double clsAbuOverfilled = 0.0;
        double clsAbuPenalty = 0.0;
        double clsAbuNPA = 0.0;  // abu of bins with non placeable area. A bin
                                 // which has huge amount of fixed are is a non
                                 // placeable bin.
        double clsAbuMaxNpa = 0.0;

        int clsNumAbuBins = 0;
        int clsNumAbu1Bins = 0;
        int clsNumAbu2Bins = 0;
        int clsNumAbu5Bins = 0;
        int clsNumAbu10Bins = 0;
        int clsNumAbu20Bins = 0;
        int clsNumAbu100Bins = 0;
        int clsNumAbuNpaBins = 0;  // abu of bins with non placeable area. A bin
                                   // which has huge amount of fixed are is a
                                   // non placeable bin.

        bool clsShowDetails : 1;
        bool clsKeepRowBounds : 1;
        bool clsHasRowBounds : 1;
        bool clsHasBlockages : 1;
        bool clsIsInitialized : 1;
        // If true, the bin dimension is computed as introduced in
        // https://doi.org/10.1145/2160916.2160958
        // Otherwise, the bin dimension is computed considering cell width and
        // hight
        bool clsStaticBinDimension : 1;

       public:
        DensityGrid() {
                clsMaxAreaBin.resize(NUM_AREAS, -1);
                clsMaxPinBin.resize(NUM_PINS, -1);
                clsShowDetails = false;
                clsKeepRowBounds = false;
                clsHasRowBounds = false;
                clsHasBlockages = false;
                clsIsInitialized = false;
                clsStaticBinDimension = true;
        }  // end constructor

        void start(const Json &params) override;
        void stop() override;

        bool isInitialized() const { return clsIsInitialized; }  // end method

        // initialize abu
        void init();
        void computeBinLength();
        void updateBinLength();
        void updateArea(const AreaType type);
        void clearAreaOfBins(const AreaType type);

        int getNumBins() const {
                return getNumCols() * getNumRows();
        }  // end method

        int getNumCols() const { return clsNumCols; }  // end method

        int getNumRows() const { return clsNumRows; }  // end method

        void getIndex(const DBUxy pos, int &row, int &col) const;

        int getIndex(const int row, const int col) const {
                return row * getNumCols() + col;
        }  // end method

        DBUxy getBinSize() const { return clsBinSize; }  // end method

        DBU getBinSize(const Dimension dim) const {
                return clsBinSize[dim];
        }  // end method

        double getTargetDensity() const {
                return clsTargetDensity;
        }  // end method

        double getAbuOverfilled() const {
                return clsAbuOverfilled;
        }  // end method

        double getAbuPenalty() const { return clsAbuPenalty; }  // end method

        bool hasArea(const int row, const int col, const AreaType type) const;

        /* Returns the type area consumed from the bin for each area type */
        DBU getArea(const int row, const int col, const AreaType type) const;

        /* Returns the area available to place cells in the bin for each area
         * type.
         * MOVABLE_AREA      ->   PLACEABLE_AREA - FIXED_AREA
         * FIXED_AREA        ->   PLACEABLE_AREA
         * PLACEABLE_AREA    ->   binArea (Area of the bin bounds)
         * Others            ->   -std::numeric_limits<double>::max()*/
        DBU getAvailableArea(const int row, const int col,
                             const AreaType type) const;

        /* Returns the ratio of each type of area
         * returns ratio between 0.0 and 1.0 (ratio is a real number in [0, 1])
         * MOVABLE_AREA      ->   MOVABLE_AREA / (PLACEABLE_AREA - FIXED_AREA)
         * FIXED_AREA        ->   FIXED_AREA / PLACEABLE_AREA
         * PLACEABLE_AREA    ->   PLACEABLE_AREA / binArea
         * Others            ->   -std::numeric_limits<double>::max()*/
        double getRatioUsage(const int row, const int col, AreaType type) const;

        DBU getMaxArea(const AreaType type) const;
        void updateMaxAreas();

        void removeArea(const int row, const int col, const AreaType type,
                        const DBU area);
        void removeArea(const int row, const int col, const AreaType type,
                        const Bounds &rect);
        void addArea(const int row, const int col, const AreaType type,
                     const DBU area);
        void addArea(const int row, const int col, const AreaType type,
                     const Bounds &rect);

        void updatePins();
        void updatePins(const PinType type);
        void addPin(const DBUxy pos, const PinType type);
        void clearPinsOfBins(const PinType type);
        int getNumPins(const int row, const int col, const PinType type);
        int getMaxPins(const PinType type) const;
        void updateMaxPins();

        void initBlockages();
        void clearBlockageOfBins();

        void splitGridBins(const double minBinSize = 0.0);

        const Bounds &getBinBound(const int row, const int col);

        const std::vector<DensityGridBin> &allBins() const {
                return clsBins;
        }  // end method

        const DensityGridBin &getDensityGridBin(const int row,
                                                const int col) const {
                int index = getIndex(row, col);
                return getDensityGridBin(index);
        }  // end method

        const DensityGridBin &getDensityGridBin(const int index) const {
                return clsBins[index];
        }  // end method

        const DensityGridBin &getDensityGridBin(const DBUxy pos) const;
        const std::vector<Rsyn::DensityGridBlockage> &allDensityGridBlockages(
            const int row, const int col) const;
        const std::vector<Bounds> &getBlockages(const int row,
                                                const int col) const;

        // update area usage of the bins and the abu violation
        void updateAbu();

        void reportAbu(std::ostream &out);
        void reportAbuBins(std::ostream &out);

       protected:
        // Adding out of row bound region to fixed area of the bin
        void updatePlaceableArea();
        void addArea(const AreaType type, const Bounds &bound);
        void addBlockageBound(const Bounds &bound, Instance inst);
};  // end class

}  // end namespace

#endif /* RSYN_DENSITY_GRID */
