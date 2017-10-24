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
	bool enableRSTT = false;
	
public:
	GenericReader() = default;
	
	void load(Engine engine, const Json& params) override;

private:
	LefDscp lefDescriptor;
	DefDscp defDescriptor;
	Legacy::Design verilogDescriptor;
	ISPD13::LIBInfo libInfo;
	ISPD13::SDCInfo sdcInfo;
	
	void parsingFlow();
	void parseLEFFiles();
	void parseDEFFiles();
	void parseVerilogFile();
	void parseLibertyFile();
	void parseSDCFile();
	void populateDesign();
	void initializeAuxiliarInfrastructure();
}; // end class

} // end namespace 

#endif /* GENERICREADER_H */

