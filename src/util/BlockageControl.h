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
 * File:   BlockageControl.h
 * Author: mpfogaca
 *
 * Created on 21 de Outubro de 2016, 09:42
 */

#ifndef BLOCKAGECONTROL_H
#define BLOCKAGECONTROL_H

#include "session/Session.h"
#include "Array.h"
#include "phy/PhysicalDesign.h"
using Rsyn::PhysicalCell;
namespace ICCAD15 {

class BlockageControl : public Rsyn::Service {
       protected:
        /* General data */
        Rsyn::Module clsModule;
        Rsyn::PhysicalDesign clsPhysicalDesign;
        Rsyn::PhysicalModule clsPhysicalModule;

        /*	Bin infrastructure */
        struct BinData {
                Bounds bound;
                std::vector<Bounds> blockages;
                std::vector<Rsyn::Cell> blockageCells;

                BinData() {}
                BinData(const Bounds bound) : bound(bound) {}
        };

        int clsMatrixDimension;
        DBU clsBinsHeight;
        DBU clsBinsWidth;
        Array2D<BinData> clsBins;

       public:
        BlockageControl(){};

        void init();

        const bool hasOverlapWithMacro(const Rsyn::Cell cell) const;
        const Rsyn::Cell findBlockOverlappingCell(const Rsyn::Cell cell) const;
        const Rsyn::Cell findBlockOverlappingCell(
            const Rsyn::PhysicalCell phCell) const;
        void buildOverlapList(const std::vector<Rsyn::Cell>& cells,
                              std::vector<Rsyn::Cell>& overlaps) const;

        virtual void start(const Rsyn::Json& params);
        virtual void stop(){};
};
}
#endif /* BLOCKAGECONTROL_H */
