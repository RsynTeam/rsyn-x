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
 * File:   ABU.h
 * Author: jucemar
 *
 * Created on 27 de Agosto de 2016, 11:40
 */

#ifndef ABU_H
#define ABU_H

#include "util/Color.h"
#include "util/Colorize.h"
#include "phy/PhysicalDesign.h"
namespace ICCAD15 {

class ABU {
       protected:
        struct density_bin {
                double lx, hx;             /* low/high x coordinate */
                double ly, hy;             /* low/high y coordinate */
                double area;               /* bin area */
                double initial_free_space; /* bin's initial freespace area */

                double m_util;     /* bin's movable cell area */
                double f_util;     /* bin's fixed cell area */
                double free_space; /* bin's freespace area */
        };                         // end struct

        Rsyn::PhysicalDesign clsPhDesign;
        Rsyn::Module clsModule;
        Rsyn::PhysicalDie clsPhDie;

        int clsAbuNumCols;
        int clsAbuNumRows;
        int clsAbuNumBins;
        double clsAbu;
        double clsTargetUtilization;
        double clsAbuGridUnit;

        const double clsBinDimension = 9.0;
        const double clsAlpha = 1.0;
        const double clsBinAreaThreshold = 0.2;
        const double clsFreeSpaceThreshold = 0.2;
        const double clsAbu2Weight = 10.0;
        const double clsAbu5Weight = 4.0;
        const double clsAbu10Weight = 2.0;
        const double clsAbu20Weight = 1.0;
        // const int clsNumBinsX = 500;
        // const int clsNumBinsY = 500;

        std::vector<density_bin> bins;
        std::vector<double> util_array;
        std::vector<Color> clsABUColors;

       public:
        ABU();

        virtual ~ABU() {}
        void initAbu(Rsyn::PhysicalDesign phDesign, Rsyn::Module module,
                     double targetUtilization, const double unit = 9.0);
        void updateAbu(bool showDetails = false);

        double getAbu() const { return clsAbu; }

        double getAlpha() const { return clsAlpha; }

        double getAbuGridUnit() const { return clsAbuGridUnit; }

        double getBinArea() const { return bins[0].area; }

        int getAbuNumCols() const { return clsAbuNumCols; }

        int getAbuNumRows() const { return clsAbuNumRows; }

        int getAbuNumBins() const { return clsAbuNumBins; }
        void coloringABU();
        void coloringABUViolations();

        Color &getABUBinColor(const unsigned row, const unsigned col) {
                unsigned binId = row * clsAbuNumCols + col;
                return clsABUColors[binId];
        }  // end method

        void removeAbuUtilization(const DBUxy lowerPos, const DBUxy upperPos,
                                  const bool isFixed);
        void addAbuUtilization(const DBUxy lowerPos, const DBUxy upperPos,
                               const bool isFixed);

        double getBinUtil(const int binId) const {
                double m_util = bins[binId].m_util;
                double free_space = bins[binId].free_space;
                return free_space == 0 ? std::numeric_limits<double>::max()
                                       : (m_util / free_space);
        }  // end method

        double getBinUtil(const int col, const int row) const {
                const int index = getABUBinIndex(col, row);
                return getBinUtil(index);
        }  // end method

        void getABUIndex(const DBUxy pos, int &col, int &row) const;

        inline int getABUBinIndex(const int col, const int row) const {
                return row * clsAbuNumCols + col;
        }
        double getBinMovableUtil(const int col, const int row) const;
        double getBinMovableRatioUtil(const int col, const int row) const;
        double getBinFreeSpace(const unsigned col, const unsigned row) const;
        double getBinMovableArea(const unsigned col, const unsigned row) const;
        double getBinFixedArea(const unsigned col, const unsigned row) const;
        double getBinArea(const unsigned col, const unsigned row) const;
        void getABUBinPos(const int col, const int row, DBU &x, DBU &y) const;
        DBUxy getBinLower(const int col, const int row) const;
        DBUxy getBinUpper(const int col, const int row) const;
        DBUxy getNearstNonViolationABU(DBUxy currentPos) const;
        bool hasFreeSpace(const int col, const int row) const;
        Bounds getABUBounds(const unsigned row, const unsigned col) const;

        void writeABU(std::ostream &out);

};  // end class

}  // end namespace

#endif /* ABU_H */
