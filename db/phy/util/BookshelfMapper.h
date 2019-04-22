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
 * File:   BookshelfMapper.h
 * Author: jucemar
 *
 * Created on 2 de Novembro de 2016, 10:20
 */

#ifndef BOOKSHELFMAPPER_H
#define BOOKSHELFMAPPER_H

#include "LefDescriptors.h"
#include "DefDescriptors.h"
#include "BookshelfDscp.h"
#include "util/DoubleRectangle.h"
#include "core/Rsyn.h"

#include <unordered_map>
//! Mapper of bookshelf circuit description to LEF/DEF formats.
class BookshelfMapper {
       protected:
        std::unordered_map<std::string, int> clsMapNodeToMacros;
        DoubleRectangle clsDieBounds;
        //! Associating bookshelf net to its pins.
        struct Net {
                std::string name;
                std::vector<int> pins;
        };
        //! Recovering bookshelf pin data
        struct Pin {
                std::string clsNode;
                std::string clsNet;
                std::string clsDirection;
                double2 displ;
                std::string clsName;
        };
        //! Associating bookshelf macro to its pins.
        struct Macro {
                std::string clsName;
                std::vector<int> pins;
        };
        std::vector<Macro> macros;
        std::vector<Pin> pins;
        std::vector<Net> nets;
        std::unordered_map<std::string, int> mapNets;
        int clsDesignUnits = 100;
        int clsNumComponents = 0;

       public:
        //! @brief Default C++11 constructor
        BookshelfMapper() = default;
        //! @brief Default C++11 destructor
        virtual ~BookshelfMapper() = default;
        //! @brief Converting bookshelf description into LEF/DEF formats.
        void mapLefDef(const BookshelfDscp &bookshelf, LefDscp &lef,
                       DefDscp &def);
        //! @brief Populates Rsyn design from bookshelf format.
        void populateRsyn(const BookshelfDscp &dscp, LefDscp &lef, DefDscp &def,
                          Rsyn::Design design);

       protected:
        //! @brief Generates synthetic net name for the circuits with nets not
        //! named.
        void updateNets(const BookshelfDscp &dscp);
        //! @brief Associating pin names to nodes and nets.
        void updateNodePins(const BookshelfDscp &dscp);
        //! @brief Generates a synthetic LEF header.
        void mapLefHeader(LefDscp &lef);
        //! @brief Generates a site used in rows.
        //! @warning Assuming all the rows in bookshelf have the same site.
        void mapLefSites(const BookshelfDscp &dscp, LefDscp &lef);
        //! @brief Generates Library cells to LEF from bookshelf macros.
        //! @warning Each macro from Bookshelf generates a LEF library cell.
        //! @todo Creates common LEF library cells from bookshelf macros.
        void mapLefCells(const BookshelfDscp &dscp, LefDscp &lef);
        //! @brief Creates 4 layers synthetic metal data.
        void createMetal(LefDscp &lef);
        //! @brief Mapping Row from Bookshelf to DEF.
        void mapDefRows(const BookshelfDscp &dscp, DefDscp &def);
        //! @brief Generates a synthetic DEF header.
        void mapDefHeader(const BookshelfDscp &dscp, DefDscp &def);
        //! @brief Mapping macros from Bookshelf to DEF.
        void mapDefComponents(const BookshelfDscp &dscp, DefDscp &def);
};  // end class

#endif /* BOOKSHELFMAPPER_H */
