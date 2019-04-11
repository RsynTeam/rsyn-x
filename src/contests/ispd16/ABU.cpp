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

#include "ABU.h"
#include "util/FloatingPoint.h"
namespace ICCAD15 {

ABU::ABU() {
        clsModule = nullptr;
        clsTargetUtilization = 0.0;
        clsAbuGridUnit = 0.0;
        clsAbuNumCols = 0;
        clsAbuNumRows = 0;
        clsAbuNumBins = 0;
        clsAbu = 0.0;
}  // end constructor

// -----------------------------------------------------------------------------

void ABU::initAbu(Rsyn::PhysicalDesign phDesign, Rsyn::Module module,
                  double targetUtilization, const double unit) {
        clsPhDesign = phDesign;
        clsModule = module;
        clsTargetUtilization = targetUtilization;
        clsPhDie = phDesign.getPhysicalDie();
        const Bounds &dieBounds = clsPhDie.getBounds();
        const double lx = dieBounds[LOWER][X];
        const double rx = dieBounds[UPPER][X];
        const double by = dieBounds[LOWER][Y];
        const double ty = dieBounds[UPPER][Y];

        clsAbuGridUnit = unit * clsPhDesign.getRowHeight();
        clsAbuNumCols = (int)std::ceil((rx - lx) / clsAbuGridUnit);
        clsAbuNumRows = (int)std::ceil((ty - by) / clsAbuGridUnit);
        clsAbuNumBins = clsAbuNumCols * clsAbuNumRows;
        bins.resize(clsAbuNumBins);
        util_array.resize(clsAbuNumBins);

        std::cout << "\tarea            : " << dieBounds << "\n";
        std::cout << "\tnumBins         : " << clsAbuNumBins << " ( "
                  << clsAbuNumCols << " x " << clsAbuNumRows << " )"
                  << "\n";
        std::cout << "\tbin dimension   : " << clsAbuGridUnit << " x "
                  << clsAbuGridUnit << "\n";

        /* 0. initialize density map */

        for (int j = 0; j < clsAbuNumRows; j++)
                for (int k = 0; k < clsAbuNumCols; k++) {
                        unsigned binId = j * clsAbuNumCols + k;
                        bins[binId].lx = lx + k * clsAbuGridUnit;
                        bins[binId].ly = by + j * clsAbuGridUnit;
                        bins[binId].hx = bins[binId].lx + clsAbuGridUnit;
                        bins[binId].hy = bins[binId].ly + clsAbuGridUnit;

                        bins[binId].hx = std::min(bins[binId].hx, rx);
                        bins[binId].hy = std::min(bins[binId].hy, ty);

                        bins[binId].area =
                            std::max((bins[binId].hx - bins[binId].lx) *
                                         (bins[binId].hy - bins[binId].ly),
                                     0.0);
                        bins[binId].m_util = 0.0;
                        bins[binId].f_util = 0.0;
                        bins[binId].free_space = 0.0;
                        bins[binId].initial_free_space = 0.0;
                }

        /* 1. build density map */
        /* (a) calculate overlaps with row sites, and add them to free_space */
        for (const Rsyn::PhysicalRow phRow : clsPhDesign.allPhysicalRows()) {
                const DBUxy p0 = phRow.getCoordinate(LOWER);
                const DBUxy p1 = phRow.getCoordinate(UPPER);

                int lcol =
                    std::max((int)std::floor((p0.x - lx) / clsAbuGridUnit), 0);
                int rcol =
                    std::min((int)std::floor((p1.x - lx) / clsAbuGridUnit),
                             clsAbuNumCols - 1);
                int brow =
                    std::max((int)std::floor((p0.y - by) / clsAbuGridUnit), 0);
                int trow =
                    std::min((int)std::floor((p1.y - by) / clsAbuGridUnit),
                             clsAbuNumRows - 1);

                for (int j = brow; j <= trow; j++) {
                        for (int k = lcol; k <= rcol; k++) {
                                unsigned binId = j * clsAbuNumCols + k;

                                /* get intersection */
                                double lx =
                                    std::max(bins[binId].lx, (double)p0.x);
                                double hx =
                                    std::min(bins[binId].hx, (double)p1.x);
                                double ly =
                                    std::max(bins[binId].ly, (double)p0.y);
                                double hy =
                                    std::min(bins[binId].hy, (double)p1.y);

                                if ((hx - lx) > 1.0e-5 && (hy - ly) > 1.0e-5) {
                                        double common_area =
                                            (hx - lx) * (hy - ly);
                                        bins[binId].initial_free_space +=
                                            common_area;
                                        bins[binId].initial_free_space =
                                            std::min(
                                                bins[binId].initial_free_space,
                                                bins[binId].area);
                                }  // end if
                        }          // end for
                }                  // end for
        }                          // end for
}  // end method

// -----------------------------------------------------------------------------

void ABU::updateAbu(bool showDetails) {
        const double targUt = clsTargetUtilization;

        const Bounds &dieBounds = clsPhDie.getBounds();
        const double left_x = dieBounds[LOWER][X];
        const double bottom_y = dieBounds[LOWER][Y];

        // Clean-up
        for (int j = 0; j < clsAbuNumRows; j++) {
                for (int k = 0; k < clsAbuNumCols; k++) {
                        unsigned binId = j * clsAbuNumCols + k;
                        bins[binId].m_util = 0.0;
                        bins[binId].f_util = 0.0;
                        bins[binId].free_space = bins[binId].initial_free_space;
                }  // end for
        }          // end for

        /* (b) add utilization by fixed/movable objects */
        for (Rsyn::Instance instance : clsModule.allInstances()) {
                Rsyn::Cell cell = instance.asCell();  // TODO: hack, assuming
                                                      // that the instance is a
                                                      // cell
                const Rsyn::PhysicalCell &phCell =
                    clsPhDesign.getPhysicalCell(cell);
                if (phCell.hasLayerBounds()) {
                        // each obs must be treated as a cell, and the common
                        // area computed using its size
                        const Rsyn::PhysicalLibraryCell &phLibCell =
                            clsPhDesign.getPhysicalLibraryCell(cell);
                        for (const Bounds &obs :
                             phLibCell.allLayerObstacles()) {
                                Bounds rect = obs;
                                DBUxy lower = obs.getCoordinate(LOWER);
                                rect.moveTo(phCell.getPosition() + lower);
                                const DBUxy bottom_left_point =
                                    rect.getCoordinate(LOWER);
                                const DBUxy top_right_point =
                                    rect.getCoordinate(UPPER);
                                int left_column = std::max(
                                    (int)std::floor(
                                        (bottom_left_point.x - left_x) /
                                        clsAbuGridUnit),
                                    0);
                                int right_column =
                                    std::min((int)std::floor(
                                                 (top_right_point.x - left_x) /
                                                 clsAbuGridUnit),
                                             clsAbuNumCols - 1);
                                int bottom_row = std::max(
                                    (int)std::floor(
                                        (bottom_left_point.y - bottom_y) /
                                        clsAbuGridUnit),
                                    0);
                                int top_row = std::min(
                                    (int)std::floor(
                                        (top_right_point.y - bottom_y) /
                                        clsAbuGridUnit),
                                    clsAbuNumRows - 1);
                                for (int j = bottom_row; j <= top_row; j++) {
                                        for (int k = left_column;
                                             k <= right_column; k++) {
                                                unsigned int binId =
                                                    j * clsAbuNumCols + k;
                                                double lower_x = std::max(
                                                    bins[binId].lx,
                                                    (double)
                                                        bottom_left_point.x);
                                                double higher_x = std::min(
                                                    bins[binId].hx,
                                                    (double)top_right_point.x);
                                                double lower_y = std::max(
                                                    bins[binId].ly,
                                                    (double)
                                                        bottom_left_point.y);
                                                double higher_y = std::min(
                                                    bins[binId].hy,
                                                    (double)top_right_point.y);
                                                if ((higher_x - lower_x) >
                                                        1.0e-5 &&
                                                    (higher_y - lower_y) >
                                                        1.0e-5) {
                                                        double common_area =
                                                            (higher_x -
                                                             lower_x) *
                                                            (higher_y -
                                                             lower_y);
                                                        if (instance.isFixed())
                                                                // if
                                                                // (phCell.isFixed(cell))
                                                                bins[binId]
                                                                    .f_util +=
                                                                    common_area;
                                                        else
                                                                bins[binId]
                                                                    .m_util +=
                                                                    common_area;
                                                }
                                        }
                                }
                        }
                } else {
                        const Bounds &bounds = phCell.getBounds();

                        const DBUxy bottom_left_point = bounds[LOWER];
                        const DBUxy top_right_point = bounds[UPPER];

                        int left_column = std::max(
                            (int)std::floor((bottom_left_point.x - left_x) /
                                            clsAbuGridUnit),
                            0);
                        int right_column = std::min(
                            (int)std::floor((top_right_point.x - left_x) /
                                            clsAbuGridUnit),
                            clsAbuNumCols - 1);
                        int bottom_row = std::max(
                            (int)std::floor((bottom_left_point.y - bottom_y) /
                                            clsAbuGridUnit),
                            0);
                        int top_row = std::min(
                            (int)std::floor((top_right_point.y - bottom_y) /
                                            clsAbuGridUnit),
                            clsAbuNumRows - 1);

                        for (int j = bottom_row; j <= top_row; j++) {
                                for (int k = left_column; k <= right_column;
                                     k++) {
                                        unsigned binId = j * clsAbuNumCols + k;
                                        /* get intersection */
                                        double lower_x = std::max(
                                            bins[binId].lx,
                                            (double)bottom_left_point.x);
                                        double higher_x =
                                            std::min(bins[binId].hx,
                                                     (double)top_right_point.x);
                                        double lower_y = std::max(
                                            bins[binId].ly,
                                            (double)bottom_left_point.y);
                                        double higher_y =
                                            std::min(bins[binId].hy,
                                                     (double)top_right_point.y);

                                        if ((higher_x - lower_x) > 1.0e-5 &&
                                            (higher_y - lower_y) > 1.0e-5) {
                                                double common_area =
                                                    (higher_x - lower_x) *
                                                    (higher_y - lower_y);
                                                if (instance.isFixed())
                                                        // if( isFixed(cell) )
                                                        bins[binId].f_util +=
                                                            common_area;
                                                else
                                                        bins[binId].m_util +=
                                                            common_area;
                                        }
                                }
                        }
                }
        }  // end for

        int skipped_bin_cnt = 0;
        util_array.assign(clsAbuNumBins, 0.0);
        /* 2. determine the free space & utilization per bin */
        for (int j = 0; j < clsAbuNumRows; j++) {
                for (int k = 0; k < clsAbuNumCols; k++) {
                        unsigned binId = j * clsAbuNumCols + k;
                        if (bins[binId].area > clsAbuGridUnit * clsAbuGridUnit *
                                                   clsBinAreaThreshold) {
                                bins[binId].free_space -= bins[binId].f_util;
                                if (bins[binId].free_space >
                                    clsFreeSpaceThreshold * bins[binId].area)
                                        util_array[binId] =
                                            bins[binId].m_util /
                                            bins[binId].free_space;
                                else
                                        skipped_bin_cnt++;
#ifdef DEBUG
                                if (util_array[binId] > 1.0) {
                                        std::cout << "[WARNING] Invalid bin "
                                                     "utilization.\n";
                                        std::cout << binId << " is not legal. "
                                                  << std::endl;
                                        std::cout
                                            << " m_util: " << bins[binId].m_util
                                            << " f_util " << bins[binId].f_util
                                            << " free_space: "
                                            << bins[binId].free_space
                                            << std::endl;
                                        // exit(1);
                                }  // end if
#endif
                        }  // end if
                }          // end for
        }                  // end for

        std::sort(util_array.begin(), util_array.end());

        /* 3. obtain ABU numbers */
        double abu1 = 0.0, abu2 = 0.0, abu5 = 0.0, abu10 = 0.0, abu20 = 0.0;
        int clip_index = (int)(0.02 * (clsAbuNumBins - skipped_bin_cnt));
        for (int j = clsAbuNumBins - 1; j > clsAbuNumBins - 1 - clip_index;
             j--) {
                abu2 += util_array[j];
        }
        abu2 = (clip_index) ? abu2 / clip_index : util_array[clsAbuNumBins - 1];

        clip_index = (int)(0.05 * (clsAbuNumBins - skipped_bin_cnt));
        for (int j = clsAbuNumBins - 1; j > clsAbuNumBins - 1 - clip_index;
             j--) {
                abu5 += util_array[j];
        }
        abu5 = (clip_index) ? abu5 / clip_index : util_array[clsAbuNumBins - 1];

        clip_index = (int)(0.10 * (clsAbuNumBins - skipped_bin_cnt));
        for (int j = clsAbuNumBins - 1; j > clsAbuNumBins - 1 - clip_index;
             j--) {
                abu10 += util_array[j];
        }
        abu10 =
            (clip_index) ? abu10 / clip_index : util_array[clsAbuNumBins - 1];

        clip_index = (int)(0.20 * (clsAbuNumBins - skipped_bin_cnt));
        for (int j = clsAbuNumBins - 1; j > clsAbuNumBins - 1 - clip_index;
             j--) {
                abu20 += util_array[j];
        }
        abu20 =
            (clip_index) ? abu20 / clip_index : util_array[clsAbuNumBins - 1];
        util_array.clear();

        if (showDetails) {
                std::cout << "\ttarget util     : " << targUt << "\n";
                std::cout << "\tABU_2,5,10,20   : " << abu2 << ", " << abu5
                          << ", " << abu10 << ", " << abu20 << "\n";
        }  // end if

        /* calculate overflow & ABU_penalty */
        abu2 = std::max(0.0, abu2 / targUt - 1.0);
        abu5 = std::max(0.0, abu5 / targUt - 1.0);
        abu10 = std::max(0.0, abu10 / targUt - 1.0);
        abu20 = std::max(0.0, abu20 / targUt - 1.0);
        clsAbu =
            (clsAbu2Weight * abu2 + clsAbu5Weight * abu5 +
             clsAbu10Weight * abu10 + clsAbu20Weight * abu20) /
            (clsAbu2Weight + clsAbu5Weight + clsAbu10Weight + clsAbu20Weight);

        if (showDetails) {
                std::cout << "\tABU penalty     : " << clsAbu << "\n";
                std::cout << "\tALPHA           : " << clsAlpha << "\n";
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

void ABU::coloringABU() {
        clsABUColors.clear();
        double maxUtil = 0;
        clsABUColors.resize(bins.size());
        Color gray(230, 230, 230);

        for (density_bin bin : bins) {
                // double util =  bin.m_util / bin.area;
                double util = bin.m_util / bin.free_space;
                maxUtil = std::max(maxUtil, util);
        }  // end for

        for (unsigned i = 0; i < bins.size(); i++) {
                density_bin bin = bins[i];
                double util = bin.m_util / bin.free_space;

                if (util == 0.0) {
                        clsABUColors[i] = gray;
                        clsABUColors[i].transparent = true;
                } else {
                        double weight = util / maxUtil;
                        Color &color = clsABUColors[i];
                        int r, g, b;
                        Colorize::colorTemperature(weight, r, g, b);
                        color.setRGB(r, g, b);
                }  // end if-else
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void ABU::coloringABUViolations() {
        double maxUtil = 0;
        clsABUColors.clear();
        clsABUColors.resize(bins.size());
        Color gray(230, 230, 230);

        for (density_bin bin : bins) {
                double util = bin.m_util / bin.free_space;
                maxUtil = std::max(maxUtil, util);
        }  // end for

        for (unsigned i = 0; i < bins.size(); i++) {
                density_bin bin = bins[i];
                double util = bin.m_util / bin.free_space;
                if (util <= clsTargetUtilization) {
                        clsABUColors[i] = gray;
                        clsABUColors[i].transparent = true;
                } else {
                        double weight = util / maxUtil;
                        Color &color = clsABUColors[i];
                        int r, g, b;
                        Colorize::colorTemperature(weight, r, g, b);
                        color.setRGB(r, g, b);
                }  // end if-else
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void ABU::removeAbuUtilization(const DBUxy lowerPos, const DBUxy upperPos,
                               const bool isFixed) {
        int lowerCol, lowerRow, upperCol, upperRow;
        getABUIndex(lowerPos, lowerCol, lowerRow);
        getABUIndex(upperPos, upperCol, upperRow);

        for (int i = lowerRow; i <= upperRow; i++) {
                for (int j = lowerCol; j <= upperCol; j++) {
                        int binId = getABUBinIndex(j, i);
                        if (binId >= bins.size()) continue;
                        /* get intersection */
                        double lower_x =
                            std::max(bins[binId].lx, (double)lowerPos[X]);
                        double higher_x =
                            std::min(bins[binId].hx, (double)upperPos[X]);
                        double lower_y =
                            std::max(bins[binId].ly, (double)lowerPos[Y]);
                        double higher_y =
                            std::min(bins[binId].hy, (double)upperPos[Y]);

                        if ((higher_x - lower_x) > 1.0e-5 &&
                            (higher_y - lower_y) > 1.0e-5) {
                                double common_area =
                                    (higher_x - lower_x) * (higher_y - lower_y);
                                if (isFixed) {
                                        bins[binId].f_util -= common_area;
                                        bins[binId].free_space += common_area;
                                } else {
                                        bins[binId].m_util -= common_area;
                                        bins[binId].free_space += common_area;
                                }  // end if-else
                        }          // end if
                }                  // end for
        }                          // end for
}  // end method

// -----------------------------------------------------------------------------

void ABU::addAbuUtilization(const DBUxy lowerPos, const DBUxy upperPos,
                            const bool isFixed) {
        int lowerCol, lowerRow, upperCol, upperRow;
        getABUIndex(lowerPos, lowerCol, lowerRow);
        getABUIndex(upperPos, upperCol, upperRow);

        for (int i = lowerRow; i <= upperRow; i++) {
                for (int j = lowerCol; j <= upperCol; j++) {
                        int binId = getABUBinIndex(j, i);
                        /* get intersection */
                        double lower_x =
                            std::max(bins[binId].lx, (double)lowerPos[X]);
                        double higher_x =
                            std::min(bins[binId].hx, (double)upperPos[X]);
                        double lower_y =
                            std::max(bins[binId].ly, (double)lowerPos[Y]);
                        double higher_y =
                            std::min(bins[binId].hy, (double)upperPos[Y]);

                        if ((higher_x - lower_x) > 1.0e-5 &&
                            (higher_y - lower_y) > 1.0e-5) {
                                double common_area =
                                    (higher_x - lower_x) * (higher_y - lower_y);
                                if (isFixed) {
                                        bins[binId].f_util += common_area;
                                        bins[binId].free_space -= common_area;
                                } else {
                                        bins[binId].m_util += common_area;
                                        bins[binId].free_space -= common_area;
                                }  // end if-else
                        }          // end if
                }                  // end for
        }                          // end for
}  // end method

// -----------------------------------------------------------------------------

void ABU::getABUIndex(const DBUxy pos, int &col, int &row) const {
        const Bounds &dieBounds = clsPhDie.getBounds();
        const double origin_x = dieBounds[LOWER][X];
        const double origin_y = dieBounds[LOWER][Y];

        col =
            std::max((int)std::floor((pos[X] - origin_x) / clsAbuGridUnit), 0);
        row =
            std::max((int)std::floor((pos[Y] - origin_y) / clsAbuGridUnit), 0);
}  // end method

// -----------------------------------------------------------------------------

void ABU::getABUBinPos(const int col, const int row, DBU &x, DBU &y) const {
        int binId = getABUBinIndex(col, row);
        x = (DBU)((bins[binId].lx + bins[binId].hx) / 2.0f);
        y = (DBU)((bins[binId].ly + bins[binId].hy) / 2.0f);
}  // end method

// -----------------------------------------------------------------------------

DBUxy ABU::getBinLower(const int col, const int row) const {
        int binId = getABUBinIndex(col, row);
        double2 pos(bins[binId].lx, bins[binId].ly);
        return pos.convertToDbu();
}  // end method

// -----------------------------------------------------------------------------

DBUxy ABU::getBinUpper(const int col, const int row) const {
        int binId = getABUBinIndex(col, row);
        double2 pos(bins[binId].hx, bins[binId].hy);
        return pos.convertToDbu();
}  // end method

// -----------------------------------------------------------------------------

double ABU::getBinMovableUtil(const int col, const int row) const {
        const unsigned index = getABUBinIndex(col, row);
        return bins[index].m_util;
}  // end method

// -----------------------------------------------------------------------------

double ABU::getBinMovableRatioUtil(const int col, const int row) const {
        const unsigned index = getABUBinIndex(col, row);
        return bins[index].m_util / bins[index].free_space;
}  // end method

// -----------------------------------------------------------------------------

double ABU::getBinFreeSpace(const unsigned col, const unsigned row) const {
        const unsigned index = getABUBinIndex(col, row);
        return bins[index].free_space;
}  // end method

// -----------------------------------------------------------------------------

double ABU::getBinMovableArea(const unsigned col, const unsigned row) const {
        const unsigned index = getABUBinIndex(col, row);
        return bins[index].area - bins[index].f_util;
}  // end method

// -----------------------------------------------------------------------------

double ABU::getBinFixedArea(const unsigned col, const unsigned row) const {
        const unsigned index = getABUBinIndex(col, row);
        return bins[index].f_util;
}  // end method

// -----------------------------------------------------------------------------

double ABU::getBinArea(const unsigned col, const unsigned row) const {
        const unsigned index = getABUBinIndex(col, row);
        return bins[index].area;
}  // end method

// -----------------------------------------------------------------------------

Bounds ABU::getABUBounds(const unsigned row, const unsigned col) const {
        unsigned binId = row * clsAbuNumCols + col;
        Bounds bound;
        bound[LOWER][X] = (DBU)bins[binId].lx;
        bound[LOWER][Y] = (DBU)bins[binId].ly;
        bound[UPPER][X] = (DBU)bins[binId].hx;
        bound[UPPER][Y] = (DBU)bins[binId].hy;
        return bound;
}  // end method

// -----------------------------------------------------------------------------

DBUxy ABU::getNearstNonViolationABU(DBUxy currentPos) const {
        int x, y, i, j, dist, binId;
        getABUIndex(currentPos, x, y);
        dist = 2;
        int bestI = -1;
        int bestJ = -1;
        double thresholdABU = 0.85 * clsTargetUtilization;
        double util = std::numeric_limits<double>::max();
        while (dist < 5) {
                for (j = y - dist; j <= y + dist; j = j + 2 * dist) {
                        if (j < 0 || j >= clsAbuNumRows) continue;
                        for (i = x - dist; i <= x + dist; i++) {
                                if (i < 0 || i >= clsAbuNumCols) continue;
                                binId = getABUBinIndex(i, j);
                                double binUtil =
                                    std::numeric_limits<double>::max();
                                if (bins[binId].free_space >
                                    clsFreeSpaceThreshold * bins[binId].area)
                                        binUtil = bins[binId].m_util /
                                                  bins[binId].free_space;

                                if (binUtil < thresholdABU) {
                                        bestI = i;
                                        bestJ = j;
                                        util = binUtil;
                                        break;
                                }  // end if
                        }          // end for
                        if (util < thresholdABU) break;
                }  // end for
                if (util < thresholdABU) break;

                for (i = x - dist; i <= x + dist; i = i + (2 * dist)) {
                        if (i < 0 || i >= clsAbuNumCols) continue;
                        for (j = y - dist + 1; j < y + dist; j++) {
                                if (j < 0 || j >= clsAbuNumRows) continue;
                                binId = getABUBinIndex(i, j);
                                double binUtil =
                                    std::numeric_limits<double>::max();
                                if (bins[binId].free_space >
                                    clsFreeSpaceThreshold * bins[binId].area)
                                        binUtil = bins[binId].m_util /
                                                  bins[binId].free_space;

                                if (binUtil < thresholdABU) {
                                        bestI = i;
                                        bestJ = j;
                                        util = binUtil;
                                        break;
                                }  // end if
                        }          // end for
                        if (util < thresholdABU) break;
                }  // end for
                if (util < thresholdABU) break;
                dist++;
        }  // end while
        DBUxy pos(bestI, bestJ);
        if (bestI > -1 && bestJ > -1)
                getABUBinPos(bestI, bestJ, pos[X], pos[Y]);
        return pos;
}  // end method

// -----------------------------------------------------------------------------

bool ABU::hasFreeSpace(const int col, const int row) const {
        const unsigned index = getABUBinIndex(col, row);
        return FloatingPoint::notApproximatelyZero(bins[index].area -
                                                   bins[index].f_util);
}  // end method

// -----------------------------------------------------------------------------

void ABU::writeABU(std::ostream &out) {
        for (int i = 0; i < clsAbuNumCols; i++) {
                for (int j = 0; j < clsAbuNumRows; j++) {
                        int pos = getABUBinIndex(i, j);
                        const density_bin &bin = bins[pos];
                        out << i << " " << j << " lx: " << bin.lx
                            << " ly: " << bin.ly << " hx: " << bin.hx
                            << " hy: " << bin.hy << " area " << bin.area
                            << " initial free space: " << bin.initial_free_space
                            << " f_util: " << bin.f_util
                            << " m_util: " << bin.m_util << " util: "
                            << (bins[pos].m_util / bins[pos].free_space)
                            << "\n";
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
