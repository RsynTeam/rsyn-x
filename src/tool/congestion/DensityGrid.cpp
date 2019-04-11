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

#include "DensityGrid.h"
#include "phy/PhysicalService.h"
#include "util/StreamStateSaver.h"

namespace Rsyn {

void DensityGrid::start(const Json &params) {
        Session session;

        if (!session.isServiceRunning("rsyn.physical")) {
                std::cout << "Warning: rsyn.physical service must be running "
                             "before start DensityGrid service.\n"
                          << "DensityGrid was not initialized.\n";
                return;
        }  // end if

        clsPhDesign = session.getPhysicalDesign();
        clsDesign = session.getDesign();
        clsModule = clsDesign.getTopModule();
        clsPhModule = clsPhDesign.getPhysicalModule(clsModule);
        clsTargetDensity = params.value("density", clsTargetDensity);
        clsBeta = params.value("numRows", clsBeta);
        clsStaticBinDimension =
            params.value("staticBinDimension", clsStaticBinDimension);
        clsShowDetails = params.value("showDetails", clsShowDetails);
        clsKeepRowBounds = params.value("keepRowBounds", clsKeepRowBounds);

        init();
        updateArea(MOVABLE_AREA);

        {  // run legalization of global placement solution to minimize cell
                // displacement.
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportDensityAbu");
                dscp.setDescription("Report ABU from density grid");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            updateAbu();
                            reportAbu(std::cout);
                    });
        }  // end block

        {  // run legalization of global placement solution to minimize cell
                // displacement.
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("reportDensityAbuBins");
                dscp.setDescription("Report ABU from density grid");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            updateAbu();
                            reportAbuBins(std::cout);
                    });
        }  // end block

        {  // run legalization of global placement solution to minimize cell
                // displacement.
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("updateDensityAbu");
                dscp.setDescription("Update ABU from density grid");

                session.registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            updateAbu();
                    });
        }  // end block

}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::stop() {}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::init() {
        computeBinLength();
        updateBinLength();
        clsIsInitialized = true;
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::computeBinLength() {
        if (clsStaticBinDimension) {
                const DBU length =
                    static_cast<DBU>(clsBeta * clsPhDesign.getRowHeight());
                clsBinSize[X] = length;
                clsBinSize[Y] = length;
        } else {
                const int numCells =
                    clsPhDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE);
                std::vector<DBU> cellWidth;
                std::vector<DBU> cellHeight;
                cellWidth.reserve(numCells);
                cellHeight.reserve(numCells);
                for (Rsyn::Instance inst : clsModule.allInstances()) {
                        if (inst.getType() != Rsyn::CELL || inst.isFixed())
                                continue;

                        Rsyn::PhysicalCell phCell =
                            clsPhDesign.getPhysicalCell(inst.asCell());
                        cellWidth.push_back(phCell.getWidth());
                        cellHeight.push_back(phCell.getHeight());
                }  // end for
                std::sort(cellWidth.begin(), cellWidth.end());
                std::sort(cellHeight.begin(), cellHeight.end());

                // bin width is defined at Section IV.B in BONNPLACE
                // Legalization :
                // minimizing movement by iterative augmentation from Ulrich
                // Brenner
                // https://doi.org/10.1109/TCAD.2013.2253834
                DBU medianWidth;   // Using the median cell size to define the
                                   // bin width
                DBU medianHeight;  // Using the median cell size to define the
                                   // bin height
                if (cellWidth.size() % 2 == 0) {
                        int pos = cellWidth.size() / 2;
                        medianWidth = static_cast<DBU>(
                            (cellWidth[pos - 1] + cellWidth[pos]) * 0.5);
                } else {
                        int pos = cellWidth.size() / 2;
                        medianWidth = cellWidth[pos];
                }  // end if

                if (cellHeight.size() % 2 == 0) {
                        int pos = cellHeight.size() / 2;
                        medianHeight = static_cast<DBU>(
                            (cellHeight[pos - 1] + cellHeight[pos]) * 0.5);
                } else {
                        int pos = cellHeight.size() / 2;
                        medianHeight = cellHeight[pos];
                }  // end if

                int pos99_5Width =
                    (int)(cellWidth.size() * 0.995);  // using the cell width
                                                      // that is bigger than
                                                      // 99.5%
                DBU width99_5 = cellWidth[pos99_5Width];

                int pos99_5Height =
                    (int)(cellHeight.size() * 0.995);  // using the cell height
                                                       // that is bigger than
                                                       // 99.5%
                DBU height99_5 = cellHeight[pos99_5Height];

                clsBinSize[X] = std::max(medianWidth * clsBeta, width99_5);
                clsBinSize[Y] = std::max(medianHeight * clsBeta, height99_5);
        }  // end if-else
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::updateBinLength() {
        const Bounds &dieBounds = clsPhModule.getBounds();
        clsNumCols = roundedUpIntegralDivision(dieBounds.computeLength(X),
                                               getBinSize(X));
        clsNumRows = roundedUpIntegralDivision(dieBounds.computeLength(Y),
                                               getBinSize(Y));
        clsBins.clear();
        clsBins.resize(getNumBins());
        clsBins.shrink_to_fit();

        if (clsShowDetails) {
                std::cout << "\tDie Bounds      : " << dieBounds << "\n";
                std::cout << "\tNum of Bins     : " << getNumBins() << " ( "
                          << getNumRows() << " x " << getNumCols() << " )"
                          << "\n";
                std::cout << "\tBin dimension   : " << getBinSize(X) << " x "
                          << getBinSize(Y) << "\n";
        }  // end if

        /* 0. initialize density map */
        DBUxy lower = clsPhModule.getCoordinate(LOWER);
        DBUxy upper = clsPhModule.getCoordinate(UPPER);
        for (int j = 0; j < getNumRows(); j++) {
                for (int k = 0; k < getNumCols(); k++) {
                        unsigned binId = getIndex(j, k);
                        DensityGridBin &bin = clsBins[binId];
                        bin.clsBounds[LOWER][X] = lower[X] + k * getBinSize(X);
                        bin.clsBounds[LOWER][Y] = lower[Y] + j * getBinSize(Y);
                        bin.clsBounds[UPPER][X] = std::min(
                            bin.clsBounds[LOWER][X] + getBinSize(X), upper[X]);
                        bin.clsBounds[UPPER][Y] = std::min(
                            bin.clsBounds[LOWER][Y] + getBinSize(Y), upper[Y]);
                }  // end for
        }          // end for

        updatePlaceableArea();
        updateArea(FIXED_AREA);

}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::updatePlaceableArea() {
        if (clsKeepRowBounds) {
                DBU binSize = getBinSize(Y);
                DBU rowHeight = clsPhDesign.getRowHeight();
                int numRows = roundedUpIntegralDivision(binSize, rowHeight);
                for (int i = 0; i < getNumBins(); i++) {
                        DensityGridBin &bin = clsBins[i];
                        bin.clsRows.reserve(numRows);
                }  // end for
        }          // end if

        const Bounds &dieBounds = clsPhModule.getBounds();
        for (const PhysicalRow phRow : clsPhDesign.allPhysicalRows()) {
                Bounds rowOverlap =
                    dieBounds.overlapRectangle(phRow.getBounds());
                int lcol, rcol, brow, trow;
                getIndex(rowOverlap[LOWER], brow, lcol);
                getIndex(rowOverlap[UPPER], trow, rcol);
                lcol = std::max(lcol, 0);
                rcol = std::min(rcol, getNumCols() - 1);
                brow = std::max(brow, 0);
                trow = std::min(trow, getNumRows() - 1);
                for (int j = brow; j <= trow; j++) {
                        for (int k = lcol; k <= rcol; k++) {
                                unsigned binId = getIndex(j, k);
                                DensityGridBin &bin = clsBins[binId];
                                const Bounds &binBound = bin.clsBounds;
                                Bounds binOverlap =
                                    rowOverlap.overlapRectangle(binBound);
                                bin.addArea(PLACEABLE_AREA,
                                            binOverlap.computeArea());
                                if (clsKeepRowBounds)
                                        bin.clsRows.push_back(binOverlap);
                        }  // end for
                }          // end for
        }                  // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::updateArea(const AreaType type) {
        clearAreaOfBins(type);

        for (Instance instance : clsModule.allInstances()) {
                if (instance.getType() != CELL) {
                        continue;
                }  // end if

                if (instance.isMovable() && type != MOVABLE_AREA) continue;

                if (instance.isFixed() && type != FIXED_AREA) continue;

                Cell cell = instance.asCell();
                const PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);

                if (type == FIXED_AREA && phCell.hasLayerBounds()) {
                        PhysicalLibraryCell phLibCel =
                            clsPhDesign.getPhysicalLibraryCell(cell);
                        for (const Bounds &rect :
                             phLibCel.allLayerObstacles()) {
                                Bounds bound = rect;
                                bound.translate(phCell.getPosition());
                                addArea(type, bound);
                        }  // end for
                } else {
                        const Bounds &bounds = phCell.getBounds();
                        addArea(type, bounds);
                }
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::clearAreaOfBins(const AreaType type) {
        // Clean-up
        for (int j = 0; j < getNumRows(); j++) {
                for (int k = 0; k < getNumCols(); k++) {
                        const int index = getIndex(j, k);
                        DensityGridBin &bin = clsBins[index];
                        bin.clearArea(type);
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::getIndex(const DBUxy pos, int &row, int &col) const {
        const Bounds &dieBounds = clsPhModule.getBounds();
        const DBUxy lower = dieBounds[LOWER];
        col = static_cast<int>((pos[X] - lower[X]) / getBinSize(X));
        row = static_cast<int>((pos[Y] - lower[Y]) / getBinSize(Y));
}  // end method

// -----------------------------------------------------------------------------

bool DensityGrid::hasArea(const int row, const int col,
                          const AreaType type) const {
        int index = getIndex(row, col);
        const DensityGridBin &bin = clsBins[index];
        return bin.hasArea(type);
}  // end method

// -----------------------------------------------------------------------------

DBU DensityGrid::getArea(const int row, const int col,
                         const AreaType type) const {
        int index = getIndex(row, col);
        const DensityGridBin &bin = clsBins[index];
        return bin.getArea(type);
}  // end method

// -----------------------------------------------------------------------------

DBU DensityGrid::getAvailableArea(const int row, const int col,
                                  const AreaType type) const {
        const int index = getIndex(row, col);
        const DensityGridBin &bin = clsBins[index];
        if (type == MOVABLE_AREA)
                return bin.getArea(PLACEABLE_AREA) - bin.getArea(FIXED_AREA);
        if (type == FIXED_AREA) return bin.getArea(PLACEABLE_AREA);
        if (type == PLACEABLE_AREA) return bin.getBounds().computeArea();
        return -std::numeric_limits<DBU>::max();
}

// -----------------------------------------------------------------------------

void DensityGrid::removeArea(const int row, const int col, const AreaType type,
                             const DBU area) {
        const int index = getIndex(row, col);
        DensityGridBin &bin = clsBins[index];
        bin.addArea(type, area);
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::removeArea(const int row, const int col, const AreaType type,
                             const Bounds &rect) {
        const int index = getIndex(row, col);
        DensityGridBin &bin = clsBins[index];
        const Bounds &overlap = rect.overlapRectangle(bin.getBounds());
        bin.addArea(type, overlap.computeArea());
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::addArea(const int row, const int col, const AreaType type,
                          const DBU area) {
        const int index = getIndex(row, col);
        DensityGridBin &bin = clsBins[index];
        bin.addArea(type, area);
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::addArea(const int row, const int col, const AreaType type,
                          const Bounds &rect) {
        const int index = getIndex(row, col);
        DensityGridBin &bin = clsBins[index];
        const Bounds &overlap = rect.overlapRectangle(bin.getBounds());
        bin.addArea(type, overlap.computeArea());
}  // end method

// -----------------------------------------------------------------------------

DBU DensityGrid::getMaxArea(const AreaType type) const {
        const int index = clsMaxAreaBin[type];
        if (index < 0) return -std::numeric_limits<DBU>::max();
        const DensityGridBin &bin = clsBins[index];
        return bin.getArea(type);
}  // end method

// -----------------------------------------------------------------------------

double DensityGrid::getRatioUsage(const int row, const int col,
                                  AreaType type) const {
        const int index = getIndex(row, col);
        const DensityGridBin &bin = clsBins[index];
        if (type == MOVABLE_AREA)
                return (double)bin.getArea(type) /
                       (double)getAvailableArea(row, col, type);
        if (type == FIXED_AREA)
                return (double)bin.getArea(type) /
                       (double)getAvailableArea(row, col, type);
        if (type == PLACEABLE_AREA)
                return (double)bin.getArea(type) /
                       (double)getAvailableArea(row, col, type);
        return -std::numeric_limits<double>::max();
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::updateMaxAreas() {
        for (int i = 0; i < getNumBins(); i++) {
                const DensityGridBin &bin = clsBins[i];
                if (bin.getArea(FIXED_AREA) > getMaxArea(FIXED_AREA))
                        clsMaxAreaBin[FIXED_AREA] = i;
                if (bin.getArea(MOVABLE_AREA) > getMaxArea(MOVABLE_AREA))
                        clsMaxAreaBin[MOVABLE_AREA] = i;
                if (bin.getArea(PLACEABLE_AREA) > getMaxArea(PLACEABLE_AREA))
                        clsMaxAreaBin[PLACEABLE_AREA] = i;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::updatePins() {
        // TODO -> use thread
        updatePins(FIXED_PIN);
        updatePins(MOVABLE_PIN);
        updatePins(BLOCK_PIN);
        updatePins(CONNECTED_PIN);
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::updatePins(const PinType type) {
        clearPinsOfBins(type);
        for (Instance inst : clsModule.allInstances()) {
                if (inst.getType() != CELL) continue;
                PhysicalCell phCell =
                    clsPhDesign.getPhysicalCell(inst.asCell());
                DBUxy pos = phCell.getPosition();

                if (type == BLOCK_PIN && !inst.isMacroBlock()) continue;
                if (type == FIXED_PIN && !inst.isFixed()) continue;
                if (type == MOVABLE_PIN && !inst.isMovable()) continue;
                for (Pin pin : inst.allPins()) {
                        if (type == CONNECTED_PIN && !pin.isConnected())
                                continue;
                        DBUxy disp = clsPhDesign.getPinDisplacement(pin);
                        addPin(pos + disp, type);
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::addPin(const DBUxy pos, const PinType type) {
        int row, col;
        getIndex(pos, row, col);
        const int binIndex = getIndex(row, col);
        DensityGridBin &bin = clsBins[binIndex];
        bin.addPin(type);
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::clearPinsOfBins(const PinType type) {
        for (int i = 0; i < getNumBins(); i++) {
                DensityGridBin &bin = clsBins[i];
                bin.clearPins(type);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

int DensityGrid::getNumPins(const int row, const int col, const PinType type) {
        int index = getIndex(row, col);
        const DensityGridBin &bin = clsBins[index];
        return bin.getNumPins(type);
}  // end method

// -----------------------------------------------------------------------------

int DensityGrid::getMaxPins(const PinType type) const {
        const int index = clsMaxPinBin[type];
        if (index < 0) return -std::numeric_limits<int>::max();
        const DensityGridBin &bin = clsBins[index];
        return bin.getNumPins(type);
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::updateMaxPins() {
        for (int i = 0; i < getNumBins(); i++) {
                const DensityGridBin &bin = clsBins[i];
                if (bin.getNumPins(FIXED_PIN) > getMaxPins(FIXED_PIN))
                        clsMaxPinBin[FIXED_PIN] = i;
                if (bin.getNumPins(MOVABLE_PIN) > getMaxPins(MOVABLE_PIN))
                        clsMaxPinBin[MOVABLE_PIN] = i;
                if (bin.getNumPins(BLOCK_PIN) > getMaxPins(BLOCK_PIN))
                        clsMaxPinBin[BLOCK_PIN] = i;
                if (bin.getNumPins(CONNECTED_PIN) > getMaxPins(CONNECTED_PIN))
                        clsMaxPinBin[CONNECTED_PIN] = i;
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::initBlockages() {
        // TODO -> reserve space in vectors.
        for (Instance inst : clsModule.allInstances()) {
                if (inst.getType() == CELL) {
                        PhysicalCell phCell =
                            clsPhDesign.getPhysicalCell(inst.asCell());
                        if (!inst.isFixed()) continue;
                        PhysicalLibraryCell phLibCell =
                            clsPhDesign.getPhysicalLibraryCell(inst.asCell());
                        if (phLibCell.hasLayerObstacles()) {
                                for (const Bounds &rect :
                                     phLibCell.allLayerObstacles()) {
                                        Bounds bounds = rect;
                                        bounds.translate(phCell.getPosition());
                                        addBlockageBound(bounds, inst);
                                }  // end for
                        } else {
                                const Bounds &bounds = phCell.getBounds();
                                addBlockageBound(bounds, inst);
                        }  // end if-else
                } else {
                        PhysicalInstance phInstance =
                            clsPhDesign.getPhysicalInstance(inst);
                        const Bounds &bounds = phInstance.getBounds();
                        PhysicalModule phModule =
                            clsPhDesign.getPhysicalModule(clsModule);
                        const Bounds &overlap =
                            bounds.overlapRectangle(phModule.getBounds());
                        if (overlap.computeArea() <= 0) continue;
                        addBlockageBound(overlap, inst);
                }  // end if-else
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::clearBlockageOfBins() {
        for (int i = 0; i < getNumBins(); i++) {
                DensityGridBin &bin = clsBins[i];
                bin.clsBlockages.clear();
        }  // end method
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::splitGridBins(const double minBinSize) {
        if (!clsHasRowBounds)
                std::cout
                    << "WARNING: Row bounds was not initialized. Therefore, "
                    << "the placeable area will be evenly split between grid "
                       "area bins.\n";

        const Bounds &dieBounds = clsPhModule.getBounds();
        DBUxy binSize =
            getBinSize() /
            2;  // dividing bin size to two. The grid is split into four.

        if (binSize[X] < minBinSize || binSize[Y] < minBinSize) return;
        int numCols = static_cast<int>(dieBounds.computeLength(X) / binSize[X]);
        int numRows = static_cast<int>(dieBounds.computeLength(Y) / binSize[Y]);
        std::vector<DensityGridBin> grid;
        grid.resize(numCols * numRows);
        DBUxy lower = clsPhModule.getCoordinate(LOWER);
        DBUxy upper = clsPhModule.getCoordinate(UPPER);
        DBU rowHeight = clsPhDesign.getRowHeight();
        int rowSize = static_cast<int>(binSize[Y] / rowHeight);

        for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                        int oldRow = i / 2;
                        int oldCol = j / 2;
                        int oldBinId = getIndex(oldRow, oldCol);
                        DensityGridBin &oldBin = clsBins[oldBinId];

                        int binId = i * numCols + j;
                        DensityGridBin &bin = grid[binId];
                        bin.clsBounds[LOWER][X] = lower[X] + j * binSize[X];
                        bin.clsBounds[LOWER][Y] = lower[Y] + i * binSize[Y];
                        bin.clsBounds[UPPER][X] = std::min(
                            bin.clsBounds[LOWER][X] + binSize[X], upper[X]);
                        bin.clsBounds[UPPER][Y] = std::min(
                            bin.clsBounds[LOWER][Y] + binSize[Y], upper[Y]);
                        const Bounds &binBounds = bin.getBounds();

                        if (clsHasRowBounds) {
                                bin.clsRows.reserve(rowSize);
                                for (const Bounds &row : oldBin.clsRows) {
                                        if (row.overlap(binBounds)) {
                                                Bounds overlapRectangle =
                                                    row.overlapRectangle(
                                                        binBounds);
                                                bin.clsRows.push_back(
                                                    overlapRectangle);
                                                DBU area = overlapRectangle
                                                               .computeArea();
                                                bin.addArea(PLACEABLE_AREA,
                                                            area);
                                        }  // end if
                                }          // end for
                                bin.clsRows.shrink_to_fit();
                        } else {
                                std::cout << "TODO " << __func__ << " at line "
                                          << __LINE__ << "\n";
                        }  // end if-else
                }          // end for
        }                  // end for
        clsNumCols = numCols;
        clsNumRows = numRows;
        clsBinSize = binSize;
        clsBins.swap(grid);  // big-O constant
        updateArea(FIXED_AREA);
        updateArea(MOVABLE_AREA);

}  // end method

// -----------------------------------------------------------------------------

const Bounds &DensityGrid::getBinBound(const int row, const int col) {
        int index = getIndex(row, col);
        const DensityGridBin &bin = clsBins[index];
        return bin.getBounds();
}  // end method

// -----------------------------------------------------------------------------

const DensityGridBin &DensityGrid::getDensityGridBin(const DBUxy pos) const {
        int row, col;
        getIndex(pos, row, col);
        const int index = getIndex(row, col);
        return clsBins[index];
}  // end method

// -----------------------------------------------------------------------------

const std::vector<Rsyn::DensityGridBlockage>
    &DensityGrid::allDensityGridBlockages(const int row, const int col) const {
        int index = getIndex(row, col);
        const DensityGridBin &bin = clsBins[index];
        return bin.clsBlockages;
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::updateAbu() {
        updateArea(MOVABLE_AREA);
        std::vector<double> ratioUsage;
        int numBins = getNumBins();
        ratioUsage.reserve(numBins);
        const double areaThreshold =
            getBinSize(X) * getBinSize(Y) * clsBinAreaThreshold;

        clsAbu1 = 0.0;
        clsAbu2 = 0.0;
        clsAbu5 = 0.0;
        clsAbu10 = 0.0;
        clsAbu20 = 0.0;
        clsAbuOverfilled = 0.0;
        clsAbuPenalty = 0.0;
        clsAbuNPA = 0.0;
        clsAbuMaxNpa = 0.0;

        clsNumAbuBins = 0;
        clsNumAbu1Bins = 0;
        clsNumAbu2Bins = 0;
        clsNumAbu5Bins = 0;
        clsNumAbu10Bins = 0;
        clsNumAbu20Bins = 0;
        clsNumAbu100Bins = 0;
        clsNumAbuNpaBins = 0;

        DBU totalNPA = 0;
        DBU totalBinArea = 0;

        for (const DensityGridBin &bin : clsBins) {
                double binArea = bin.clsBounds.computeArea();
                if (binArea > areaThreshold) {
                        double freeArea = bin.getArea(PLACEABLE_AREA) -
                                          bin.getArea(FIXED_AREA);
                        if (freeArea > clsFreeSpaceThreshold * binArea) {
                                double ratio =
                                    bin.getArea(MOVABLE_AREA) / freeArea;
                                ratioUsage.push_back(ratio);
                                if (ratio > getTargetDensity()) {
                                        clsNumAbu100Bins++;
                                        clsAbuOverfilled += ratio;
                                }  // end if
                        } else if (bin.getArea(MOVABLE_AREA) > 0) {
                                clsNumAbuNpaBins++;
                                DBU areaMovable = bin.getArea(MOVABLE_AREA);
                                DBU binArea = bin.getBounds().computeArea();
                                totalNPA += areaMovable;
                                double areaRatio =
                                    areaMovable / static_cast<double>(binArea);
                                clsAbuMaxNpa =
                                    std::max(clsAbuMaxNpa, areaRatio);
                                totalBinArea += binArea;
                        }  // end if
                }          // end if
        }                  // end for

        numBins = ratioUsage.size();
        std::sort(ratioUsage.begin(), ratioUsage.end(),
                  [](const double ratio1, const double ratio2) {
                          return ratio1 > ratio2;
                  });  // end sort

        clsAbu1 = 0.0;
        clsAbu2 = 0.0;
        clsAbu5 = 0.0;
        clsAbu10 = 0.0;
        clsAbu20 = 0.0;

        clsNumAbuBins = numBins;

        const int index1 = static_cast<int>(0.01 * numBins);
        const int index2 = static_cast<int>(0.02 * numBins);
        const int index5 = static_cast<int>(0.05 * numBins);
        const int index10 = static_cast<int>(0.10 * numBins);
        const int index20 = static_cast<int>(0.20 * numBins);

        for (int j = 0; j < index1; ++j) {
                clsAbu1 += ratioUsage[j];
                if (ratioUsage[j] > getTargetDensity()) {
                        clsNumAbu1Bins++;
                }  // end if
        }          // end for

        clsAbu2 = clsAbu1;
        clsNumAbu2Bins = clsNumAbu1Bins;
        for (int j = index1; j < index2; ++j) {
                clsAbu2 += ratioUsage[j];
                if (ratioUsage[j] > getTargetDensity()) {
                        clsNumAbu2Bins++;
                }  // end if
        }          // end for

        clsAbu5 = clsAbu2;
        clsNumAbu5Bins = clsNumAbu2Bins;
        for (int j = index2; j < index5; ++j) {
                clsAbu5 += ratioUsage[j];
                if (ratioUsage[j] > getTargetDensity()) {
                        clsNumAbu5Bins++;
                }  // end if
        }          // end for

        clsAbu10 = clsAbu5;
        clsNumAbu10Bins = clsNumAbu5Bins;
        for (int j = index5; j < index10; ++j) {
                clsAbu10 += ratioUsage[j];
                if (ratioUsage[j] > getTargetDensity()) {
                        clsNumAbu10Bins++;
                }  // end if
        }          // end for

        clsAbu20 = clsAbu10;
        clsNumAbu20Bins = clsNumAbu10Bins;
        for (int j = index10; j < index20; ++j) {
                clsAbu20 += ratioUsage[j];
                if (ratioUsage[j] > getTargetDensity()) {
                        clsNumAbu20Bins++;
                }  // end if
        }          // end for

        clsAbu1 = (index1) ? clsAbu1 / index1 : 0.0;
        clsAbu2 = (index2) ? clsAbu2 / index2 : 0.0;
        clsAbu5 = (index5) ? clsAbu5 / index5 : 0.0;
        clsAbu10 = (index10) ? clsAbu10 / index10 : 0.0;
        clsAbu20 = (index20) ? clsAbu20 / index20 : 0.0;
        clsAbuOverfilled =
            clsNumAbu100Bins ? clsAbuOverfilled / clsNumAbu100Bins : 0.0;
        clsAbuNPA = clsNumAbuNpaBins
                        ? totalNPA / static_cast<double>(totalBinArea)
                        : 0.0;

        const double wAbu2 = clsAbu2Weight * clsAbu2;
        const double wAbu5 = clsAbu5Weight * clsAbu5;
        const double wAbu10 = clsAbu10Weight * clsAbu10;
        const double wAbu20 = clsAbu20Weight * clsAbu20;

        //	clsAbu = (wAbu2 + wAbu5 + wAbu10 + wAbu20) /
        //		(clsAbu2Weight + clsAbu5Weight + clsAbu10Weight +
        //clsAbu20Weight);

        double penaltyAbu2 = (clsAbu2 / getTargetDensity()) - 1.0;
        penaltyAbu2 = std::max(0.0, penaltyAbu2);

        double penaltyAbu5 = (clsAbu5 / getTargetDensity()) - 1.0;
        penaltyAbu5 = std::max(0.0, penaltyAbu5);

        double penaltyAbu10 = (clsAbu10 / getTargetDensity()) - 1.0;
        penaltyAbu10 = std::max(0.0, penaltyAbu10);

        double penaltyAbu20 = (clsAbu20 / getTargetDensity()) - 1.0;
        penaltyAbu20 = std::max(0.0, penaltyAbu20);

        double wpenaltyAbu2 = clsAbu2Weight * penaltyAbu2;
        double wpenaltyAbu5 = clsAbu5Weight * penaltyAbu5;
        double wpenaltyAbu10 = clsAbu10Weight * penaltyAbu10;
        double wpenaltyAbu20 = clsAbu20Weight * penaltyAbu20;

        clsAbuPenalty =
            (wpenaltyAbu2 + wpenaltyAbu5 + wpenaltyAbu10 + wpenaltyAbu20) /
            (clsAbu2Weight + clsAbu5Weight + clsAbu10Weight + clsAbu20Weight);

        if (clsShowDetails) {
                std::cout << "\ttarget util     : " << getTargetDensity()
                          << "\n";
                std::cout << "\tABU {2,5,10,20} : " << clsAbu2 << ", "
                          << clsAbu5 << ", " << clsAbu10 << ", " << clsAbu20
                          << "\n";
                std::cout << "\tABU penalty     : " << clsAbuPenalty << "\n";
                std::cout << "\tALPHA           : " << clsAlpha << "\n";
        }  // end if

}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::reportAbu(std::ostream &out) {
        StreamStateSaver sss(out);
        const int N = 15;

        out << std::left;
        out << "\n";
        out << "                  ";
        out << std::setw(N) << "Design";
        out << std::setw(N) << "Abu1%";
        out << std::setw(N) << "Abu2%";
        out << std::setw(N) << "Abu5%";
        out << std::setw(N) << "Abu10%";
        out << std::setw(N) << "Abu20%";
        out << std::setw(N) << "AbuOverfilled";
        out << std::setw(N) << "AbuPenalty";
        out << std::setw(N) << "AvgNPA";
        out << std::setw(N) << "maxNPA";

        out << "\n";

        out << "DGrid (ABU):      ";  // make it easy to grep
        out << std::setw(N) << clsDesign.getName();
        out << std::setw(N) << clsAbu1;
        out << std::setw(N) << clsAbu2;
        out << std::setw(N) << clsAbu5;
        out << std::setw(N) << clsAbu10;
        out << std::setw(N) << clsAbu20;
        out << std::setw(N) << clsAbuOverfilled;
        out << std::setw(N) << clsAbuPenalty;
        out << std::setw(N) << clsAbuNPA;
        out << std::setw(N) << clsAbuMaxNpa;

        out << "\n";

        sss.restore();

}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::reportAbuBins(std::ostream &out) {
        StreamStateSaver sss(out);
        const int N = 15;

        out << std::left;
        out << "\n";
        out << "                  ";
        out << std::setw(N) << "Design";
        out << std::setw(N) << "Abu1%";
        out << std::setw(N) << "Abu2%";
        out << std::setw(N) << "Abu5%";
        out << std::setw(N) << "Abu10%";
        out << std::setw(N) << "Abu20%";
        out << std::setw(N) << "Overfilled%";
        out << std::setw(N) << "Valid";
        out << std::setw(N) << "NpaBins";
        out << std::setw(N) << "Total";
        out << "\n";

        out << "DGrid (ABUBins):  ";  // make it easy to grep
        out << std::setw(N) << clsDesign.getName();
        out << std::setw(N) << clsNumAbu1Bins;
        out << std::setw(N) << clsNumAbu2Bins;
        out << std::setw(N) << clsNumAbu5Bins;
        out << std::setw(N) << clsNumAbu10Bins;
        out << std::setw(N) << clsNumAbu20Bins;
        out << std::setw(N) << clsNumAbu100Bins;
        out << std::setw(N) << clsNumAbuBins;
        out << std::setw(N) << clsNumAbuNpaBins;
        out << std::setw(N) << getNumBins();
        out << "\n";

        sss.restore();

}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::addArea(const AreaType type, const Bounds &bound) {
        int lcol, rcol, brow, trow;
        getIndex(bound[LOWER], brow, lcol);
        getIndex(bound[UPPER], trow, rcol);

        lcol = std::max(lcol, 0);
        rcol = std::min(rcol, getNumCols() - 1);
        brow = std::max(brow, 0);
        trow = std::min(trow, getNumRows() - 1);
        for (int j = brow; j <= trow; j++) {
                for (int k = lcol; k <= rcol; k++) {
                        addArea(j, k, type, bound);
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void DensityGrid::addBlockageBound(const Bounds &bound, Instance inst) {
        int brow, lcol, trow, rcol;
        getIndex(bound[LOWER], brow, lcol);
        getIndex(bound[UPPER], trow, rcol);
        for (int i = brow; i <= trow; i++) {
                for (int j = lcol; j <= rcol; j++) {
                        const int index = getIndex(i, j);
                        DensityGridBin &bin = clsBins[index];
                        int blockIndex = bin.clsBlockages.size();
                        std::unordered_map<std::string, int>::iterator it =
                            bin.clsMapBlockages.find(inst.getName());
                        if (it != bin.clsMapBlockages.end()) {
                                blockIndex =
                                    bin.clsMapBlockages[inst.getName()];
                        } else {
                                bin.clsBlockages.push_back(
                                    DensityGridBlockage());
                                bin.clsMapBlockages[inst.getName()] =
                                    blockIndex;
                        }  // end if-else
                        DensityGridBlockage &block =
                            bin.clsBlockages[blockIndex];
                        block.clsBounds.push_back(
                            bound.overlapRectangle(bin.clsBounds));
                        block.clsInstance = inst;
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
