/* Copyright 2014-2017 Rsyn
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

#include "ISPD2014Reader.h"
#include "rsyn/io/parser/lef_def/LEFControlParser.h"
#include "rsyn/io/parser/lef_def/DEFControlParser.h"
#include "rsyn/io/parser/verilog/SimplifiedVerilogReader.h"

#include "rsyn/phy/PhysicalService.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/io/Graphics.h"

#include "rsyn/util/Stepwatch.h"

namespace Rsyn {

void ISPD2014Reader::load(const Json & config) {
	Stepwatch openIspd14("Opening benchmark of ISPD 2014");
	
	this->session =  session;
	std::string path = config.value("path", "");
	const char separator = boost::filesystem::path::preferred_separator;
	
	std::string cellsFilename = config.value("cells", "");
	cellsFilename = boost::filesystem::exists(cellsFilename) ?
		cellsFilename : path + separator + cellsFilename;

	std::string techFilename = config.value("tech", "");
	techFilename = boost::filesystem::exists(techFilename) ?
		techFilename : path + separator + techFilename;

	std::string defFilename = config.value("def", "");
	defFilename = boost::filesystem::exists(defFilename) ?
		defFilename : path + separator + defFilename;

	std::string verilogFilename = config.value("verilog", "");
	verilogFilename = boost::filesystem::exists(verilogFilename) ?
		verilogFilename : path + separator + verilogFilename;

	std::cout << "Opening Benchmarks of the ISPD 2014 Contest\n";
	std::cout << "Circuit Files:\n";
	std::cout << "\tCells File......: " << cellsFilename << "\n";
	std::cout << "\tTech File.......: " << techFilename << "\n";
	std::cout << "\tDEF File........: " << defFilename << "\n";
	std::cout << "\tVerilog File....: " << verilogFilename << "\n";
	
	LEFControlParser lefParser;
	DEFControlParser defParser;

	Legacy::Design verilogDesignDescriptor;
	LefDscp lefDscp;
	DefDscp defDscp;

	Stepwatch watchParsing("Parsing Design");
	lefParser.parseLEF(techFilename, lefDscp);
	lefParser.parseLEF(cellsFilename, lefDscp);
	defParser.parseDEF(defFilename, defDscp);
	Parsing::SimplifiedVerilogReader parser(verilogDesignDescriptor);
	parser.parseFromFile(verilogFilename);
	watchParsing.finish();
	
	Stepwatch watchRsyn("Populating Rsyn");
	clsDesign = session.getDesign();
	Reader::populateRsyn(
		lefDscp,
		defDscp,
		verilogDesignDescriptor,
		clsDesign);
	watchRsyn.finish();

	clsModule = clsDesign.getTopModule();
	Stepwatch watchPopulateLayers("Initializing Physical Layer");
	
	Json phDesignJason;
	phDesignJason["clsEnableMergeRectangles"] = true;
	phDesignJason["clsEnableNetPinBoundaries"] = true;
	phDesignJason["clsEnableRowSegments"] = true;
	session.startService("rsyn.physical", phDesignJason);		
	Rsyn::PhysicalService * phService = session.getService("rsyn.physical");
	Rsyn::PhysicalDesign clsPhysicalDesign = phService->getPhysicalDesign();
	clsPhysicalDesign.loadLibrary(lefDscp);
	clsPhysicalDesign.loadDesign(defDscp);
	clsPhysicalDesign.updateAllNetBounds(false);
	watchPopulateLayers.finish();

	session.startService("rsyn.graphics", {});
	Graphics * graphics = session.getService("rsyn.graphics");
	graphics->coloringByCellType();
	openIspd14.finish();
} // end method 

} // end namespace 