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
 * File:   ICCAD17Reader.cpp
 * Author: jucemar
 *
 * Created on 08 de Abril de 2017, 15:40
 */

#include "ICCAD17Reader.h"
#include "io/parser/lef_def/LEFControlParser.h"
#include "io/parser/lef_def/DEFControlParser.h"

#include "phy/PhysicalDesign.h"
#include "phy/PhysicalDesign.h"
#include "util/Stepwatch.h"
#include "io/Graphics.h"

namespace Rsyn {

bool ICCAD17Reader::load(const Rsyn::Json &options) {
        this->session = session;
        clsDesign = session.getDesign();
        clsModule = clsDesign.getTopModule();

        std::string path = options.value("path", "");
        const char separator = boost::filesystem::path::preferred_separator;

        std::string cellsFilename = options.value("cells", "");
        cellsFilename = boost::filesystem::exists(cellsFilename)
                            ? cellsFilename
                            : path + separator + cellsFilename;

        std::string techFilename = options.value("tech", "");
        techFilename = boost::filesystem::exists(techFilename)
                           ? techFilename
                           : path + separator + techFilename;

        std::string designFilename = options.value("design", "");
        designFilename = boost::filesystem::exists(designFilename)
                             ? designFilename
                             : path + separator + designFilename;

        std::string constraintsFilename = options.value("constraints", "");
        constraintsFilename = boost::filesystem::exists(constraintsFilename)
                                  ? constraintsFilename
                                  : path + separator + constraintsFilename;

        std::cout << "Loading Benchmark from 2017 ICCAD Contest\n";
        std::cout << "Circuit Files:\n";
        std::cout << "\tCells File..........: " << cellsFilename << "\n";
        std::cout << "\tTech File...........: " << techFilename << "\n";
        std::cout << "\tDesign File.........: " << designFilename << "\n";
        std::cout << "\tConstraints File....: " << constraintsFilename << "\n";

        parseConstraints(constraintsFilename);

        LEFControlParser lefParser;
        DEFControlParser defParser;
        LefDscp lefDscp;
        DefDscp defDscp;

        Stepwatch watchParsing("Parsing Circuit");
        lefParser.parseLEF(techFilename, lefDscp);
        lefParser.parseLEF(cellsFilename, lefDscp);
        defParser.parseDEF(designFilename, defDscp);
        watchParsing.finish();

        Stepwatch watchPopulate("Populating Circuit");
        populateRsyn(lefDscp, defDscp, clsDesign);
        watchPopulate.finish();

        Stepwatch watchPhysical("Initializing Physical Layer");
        session.startService("rsyn.physical");
        Rsyn::PhysicalDesign clsPhysicalDesign = session.getPhysicalDesign();
        clsPhysicalDesign.loadLibrary(lefDscp);
        clsPhysicalDesign.loadDesign(defDscp);
        clsPhysicalDesign.updateAllNetBounds(false);
        watchPhysical.finish();

        // session.startService("rsyn.report", {});
        session.startService("rsyn.writer", {});

        session.startService("rsyn.graphics", {});
        Rsyn::Graphics *graphics = session.getService("rsyn.graphics");
        graphics->coloringByCellType();

        return true;
}  // end method

// -----------------------------------------------------------------------------

void ICCAD17Reader::parseConstraints(const std::string &filename) {
        ifstream dot_parm(filename.c_str());
        if (!dot_parm.good()) {
                cerr << "Constraints file cannot be open `" << filename
                     << "' for reading." << endl;
                cerr << "The script will use the default parameters for "
                        "evaluation."
                     << endl;
                return;
        }  // end if

        std::cout << " TODO " << __func__ << "\n";
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace
