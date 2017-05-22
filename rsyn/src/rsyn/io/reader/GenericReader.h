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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GenericReader.h
 * Author: mateus
 *
 * Created on 14 de Abril de 2017, 12:11
 */

#ifndef GENERICREADER_H
#define GENERICREADER_H

#include "rsyn/engine/Engine.h"
#include "rsyn/model/timing/types.h"

namespace Rsyn {

class GenericReader : public Reader {
	Engine engine;	
	
	std::vector<std::string> lefFiles;
	std::vector<std::string> defFiles;
	std::string verilogFile;
	std::string sdcFile;
	std::string libertyFile;
	
	Number localWireCapacitancePerMicron;
	Number localWireResistancePerMicron;
	DBU maxWireSegmentInMicron;
	
	bool enableTiming = false;
	bool enableNetlistFromVerilog = false;
	
public:
	GenericReader() = default;
	
	//! @brief Overriden method, responsible for processing the input parameters
	//!		   and calling the parsing flow.
	void load(Engine engine, const Json& params) override;
private:
	LefDscp lefDescriptor;
	DefDscp defDescriptor;
	Legacy::Design verilogDescriptor;
	ISPD13::LIBInfo libInfo;
	ISPD13::SDCInfo sdcInfo;
	
	//! @brief	Defines the order in which the input files are going to be parsed.
	void parsingFlow();
	
	//! @brief	Verifies the consistency of the input URLs and runs the LEF parser.
	void parseLEFFiles();
	//! @brief	Verifies the consistency of the input URLs and runs the DEF parser.
	void parseDEFFiles();
	//! @brief	Verifies the consistency of the input URL and runs the Verilog parser.
	void parseVerilogFile();
	//! @brief	Verifies the consistency of the input URL and runs the Liberty parser.
	void parseLibertyFile();
	//! @brief	Verifies the consistency of the input URL and runs the SDC parser.
	void parseSDCFile();
	//! @brief	Populates Rsyn core data structures using the information read in the input files.
	void populateDesign();
	//! @brief  Initializes some services, depending on which files were specified:
	//!			Scenario is dependent on Liberty and SDC files;
	//!			RoutingEstimator is dependent on variables localWireCapacitancePerMicron, 
	//!			localWireResistancePerMicron and maxWireSegmentInMicron;
	//!			Timer is dependant on Scenario and Routing Estimator;
	//!			Report is dependant on Timer;
	//!			Graphics and Writer do not have dependencies.
	void initializeAuxiliarInfrastructure();
}; // end class

} // end namespace 

#endif /* GENERICREADER_H */

