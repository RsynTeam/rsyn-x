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

#include "DesignPositionReader.h"

#include "io/parser/lef_def/DEFControlParser.h"

#include "io/parser/bookshelf/BookshelfParser.h"

#include "phy/util/BookshelfDscp.h"
#include "phy/util/BookshelfMapper.h"

#include "phy/PhysicalDesign.h"
#include "phy/PhysicalDesign.h"

#include "util/Stepwatch.h"

namespace Rsyn {

bool DesignPositionReader::run(const Rsyn::Json &config) {
        return load(config);
}  // end method

// -----------------------------------------------------------------------------

bool DesignPositionReader::load(const Rsyn::Json &config) {
        this->session = session;
        std::string path = config.value("path", "");
        clsDesign = session.getDesign();
        clsModule = session.getTopModule();
        clsPhysicalDesign = session.getPhysicalDesign();
        std::string ext = boost::filesystem::extension(path);

        // checking the extension file
        if (ext.compare(".pl") == 0) {
                openBookshelf(path);
        } else if (ext.compare(".def") == 0) {
                openDef(path);
        } else {
                throw Exception("Invalid extension file in the path " + path);
        }  // end if-else

        return true;
}  // end method

// -----------------------------------------------------------------------------

void DesignPositionReader::openDef(std::string &path) {
        DEFControlParser defParser;
        DefDscp defDscp;
        defParser.parseDEF(path, defDscp);
        Rsyn::PhysicalDesign clsPhysicalDesign = session.getPhysicalDesign();

        for (const DefComponentDscp &component : defDscp.clsComps) {
                Rsyn::Cell cell = clsDesign.findCellByName(component.clsName);

                if (!cell) {
                        throw Exception("Library cell '" + component.clsName +
                                        "' not found.\n");
                }  // end if

                if (cell.isFixed()) continue;
                PhysicalCell physicalCell =
                    clsPhysicalDesign.getPhysicalCell(cell);

                clsPhysicalDesign.placeCell(physicalCell, component.clsPos);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void DesignPositionReader::openBookshelf(std::string &path) {
        BookshelfParser parser;
        BookshelfDscp dscp;

        parser.parsePlaced(path, dscp);
        Stepwatch watchParsing("Parsing Bookshelf Placed Design");
        watchParsing.finish();
        DBU scale = clsPhysicalDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);
        for (const BookshelfNode &node : dscp.clsNodes) {
                Rsyn::Cell cell = clsDesign.findCellByName(node.clsName);
                if (!cell) {
                        throw Exception("Library cell '" + node.clsName +
                                        "' not found.\n");
                }  // end if

                if (cell.isFixed()) continue;
                PhysicalCell physicalCell =
                    clsPhysicalDesign.getPhysicalCell(cell);
                DBUxy pos = node.clsPos.convertToDbu();
                pos.scale(scale);
                clsPhysicalDesign.placeCell(physicalCell, pos);
        }  // end for

        clsPhysicalDesign.updateAllNetBounds(false);
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
