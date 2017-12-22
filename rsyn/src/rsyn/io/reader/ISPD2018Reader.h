/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ISPD2018Reader.h
 * Author: mateus
 *
 * Created on December 21, 2017, 9:13 PM
 */

#ifndef ISPD2018READER_H
#define ISPD2018READER_H

#include "rsyn/session/Session.h"

namespace Rsyn  {
	
class RoutingGuide;
	
class ISPD2018Reader : public Reader {
public:
	ISPD2018Reader() = default;
	void load(const Json& params) override;
	
private:
	Session session;
	
	std::string lefFile;
	std::string defFile;
	std::string guideFile;
	LefDscp lefDescriptor;
	DefDscp defDescriptor;
	RoutingGuide *routingGuide;
	
	void parsingFlow();
	void parseLEFFile();
	void parseDEFFile();
	void parseGuideFile();
	void populateDesign();
	void initializeAuxiliarInfrastructure();
};

}

#endif /* ISPD2018READER_H */

