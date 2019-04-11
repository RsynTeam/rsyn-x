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
 * File:   BlockageControl.cpp
 * Author: mpfogaca
 *
 * Created on 21 de Outubro de 2016, 09:42
 */

#include "BlockageControl.h"
#include "Stepwatch.h"
#include "phy/PhysicalDesign.h"

namespace ICCAD15 {

void BlockageControl::start(const Rsyn::Json& params) {
        Rsyn::Session session;

        clsModule = session.getTopModule();
        clsPhysicalDesign = session.getPhysicalDesign();
        clsPhysicalModule = clsPhysicalDesign.getPhysicalModule(clsModule);

        // params.value() did not work here... don't know why...
        if (params.count("dim"))
                clsMatrixDimension = params["dim"];
        else
                clsMatrixDimension = 50;

        clsBinsWidth = roundedUpIntegralDivision(
            clsPhysicalModule.getBounds().computeLength(X),
            (DBU)clsMatrixDimension);
        clsBinsHeight = roundedUpIntegralDivision(
            clsPhysicalModule.getBounds().computeLength(Y),
            (DBU)clsMatrixDimension);

        init();
}  // end method

//------------------------------------------------------------------------------

void BlockageControl::init() {
        Stepwatch("Initializing blockage control infrastructure");

        clsBins.initialize(clsMatrixDimension);
        for (int i = 0; i < clsMatrixDimension; i++) {
                for (int j = 0; j < clsMatrixDimension; j++) {
                        const DBU x =
                            clsPhysicalModule.getBounds().getCoordinate(LOWER,
                                                                        X) +
                            j * clsBinsWidth;
                        const DBU y =
                            clsPhysicalModule.getBounds().getCoordinate(LOWER,
                                                                        Y) +
                            i * clsBinsHeight;
                        clsBins(i, j) = BinData(
                            Bounds(x, y, x + clsBinsWidth, y + clsBinsHeight));
                }  // end for
        }          // end for

        for (Rsyn::Instance instance : clsModule.allInstances()) {
                if (instance.getType() != Rsyn::InstanceType::CELL) continue;

                PhysicalCell pCell =
                    clsPhysicalDesign.getPhysicalCell(instance.asCell());

                if (!instance.isMacroBlock()) continue;

                const int minCol = clsBins.clampCol(
                    (pCell.getBounds().getCoordinate(LOWER, X) -
                     clsPhysicalModule.getBounds().getCoordinate(LOWER, X)) /
                    clsBinsWidth);
                const int maxCol = clsBins.clampCol(
                    (pCell.getBounds().getCoordinate(UPPER, X) -
                     clsPhysicalModule.getBounds().getCoordinate(LOWER, X)) /
                    clsBinsWidth);
                const int minRow = clsBins.clampRow(
                    (pCell.getBounds().getCoordinate(LOWER, Y) -
                     clsPhysicalModule.getBounds().getCoordinate(LOWER, Y)) /
                    clsBinsHeight);
                const int maxRow = clsBins.clampRow(
                    (pCell.getBounds().getCoordinate(UPPER, Y) -
                     clsPhysicalModule.getBounds().getCoordinate(LOWER, Y)) /
                    clsBinsHeight);

                // Guilherme Flach - 2016/11/05 - DenseMatrix assumes that the
                // first
                // parameter is the col and the second one is the row. But here
                // they
                // are being used inverted. This is okay as the matrix is square
                // and
                // they are being used inverted everywhere. But may lead to
                // problems
                // if a non-square matrix is used.

                for (int i = minRow; i <= maxRow; i++) {
                        for (int j = minCol; j <= maxCol; j++) {
                                if (!pCell.hasLayerBounds()) {
                                        if (clsBins(i, j).bound.overlapArea(
                                                pCell.getBounds())) {
                                                clsBins(i, j).blockages.push_back(
                                                    clsBins(i, j)
                                                        .bound.overlapRectangle(
                                                            pCell.getBounds()));
                                                clsBins(i, j)
                                                    .blockageCells.push_back(
                                                        instance.asCell());
                                        }  // end if
                                } else {
                                        const Rsyn::PhysicalLibraryCell&
                                            phLibCell =
                                                clsPhysicalDesign
                                                    .getPhysicalLibraryCell(
                                                        instance.asCell());
                                        for (Bounds obs :
                                             phLibCell.allLayerObstacles()) {
                                                obs.translate(
                                                    pCell.getPosition());
                                                if (clsBins(i, j)
                                                        .bound.overlapArea(
                                                            obs)) {
                                                        clsBins(i, j)
                                                            .blockages
                                                            .push_back(
                                                                clsBins(i, j)
                                                                    .bound
                                                                    .overlapRectangle(
                                                                        obs));
                                                        clsBins(i, j)
                                                            .blockageCells
                                                            .push_back(
                                                                instance
                                                                    .asCell());
                                                }  // end if
                                        }          // end for
                                }                  // end if
                        }                          // end for
                }                                  // end for
        }                                          // end for
}  // end method

//------------------------------------------------------------------------------

const bool BlockageControl::hasOverlapWithMacro(const Rsyn::Cell cell) const {
        return (findBlockOverlappingCell(cell)) ? (true) : (false);
}  // end method

//------------------------------------------------------------------------------

const Rsyn::Cell BlockageControl::findBlockOverlappingCell(
    const Rsyn::Cell cell) const {
        PhysicalCell phCell = clsPhysicalDesign.getPhysicalCell(cell);
        return findBlockOverlappingCell(phCell);
}  // end method

//------------------------------------------------------------------------------

const Rsyn::Cell BlockageControl::findBlockOverlappingCell(
    const Rsyn::PhysicalCell phCell) const {
        if (phCell.isMacroBlock()) return nullptr;

        const int i = (phCell.getCenter(Y) -
                       clsPhysicalModule.getBounds().getCoordinate(LOWER, Y)) /
                      clsBinsHeight;
        const int j = (phCell.getCenter(X) -
                       clsPhysicalModule.getBounds().getCoordinate(LOWER, X)) /
                      clsBinsWidth;

        for (int k = 0; k < clsBins(i, j).blockages.size(); k++) {
                const Bounds block = clsBins(i, j).blockages[k];
                if (phCell.getBounds().overlapArea(block)) {
                        return clsBins(i, j).blockageCells[k];
                }  // end if
        }          // end for

        //	const bool debug = false;
        //	if (debug) {
        //		std::cout << "[WARNING] No overlap was found for cell ";
        //		std::cout << clsPhysicalDesign.get << ".\n";
        //	}

        return nullptr;
}  // endMethod

//------------------------------------------------------------------------------

void BlockageControl::buildOverlapList(
    const std::vector<Rsyn::Cell>& cells,
    std::vector<Rsyn::Cell>& overlaps) const {
        for (Rsyn::Cell cell : cells) {
                overlaps.push_back(findBlockOverlappingCell(cell));
        }  // end for
}  // end method
}
