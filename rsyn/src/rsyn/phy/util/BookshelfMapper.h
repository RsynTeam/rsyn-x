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
#include "rsyn/util/DoubleRectangle.h"
#include "rsyn/core/Rsyn.h"

#include <unordered_map>

class BookshelfMapper {
protected:
	std::unordered_map<std::string, int> clsMapNodeToMacros;
	DoubleRectangle clsDieBounds;
	struct Net {
		std::string name;
		std::vector<int> pins;
	};
	struct Pin {
		std::string clsNode;
		std::string clsNet;
		std::string clsDirection;
		double2 displ;
		std::string clsName;
	};
	struct Macro {
		std::string clsName;
		std::vector<int> pins;
	};
	std::vector<Macro> macros;
	std::vector<Pin> pins;
	std::vector<Net> nets;
	std::unordered_map<std::string, int> mapNets;
	int clsDesignUnits;
	int clsNumComponents;
public:
	BookshelfMapper();
	virtual ~BookshelfMapper();
	void mapLefDef(const BookshelfDscp & bookshelf, LefDscp & lef, DefDscp & def);
	void populateRsyn(const BookshelfDscp & dscp, LefDscp & lef, DefDscp & def, Rsyn::Design design);
protected:
	void updateNets(const BookshelfDscp & dscp);
	void updateNodePins(const BookshelfDscp & dscp);
	void mapLefHeader(LefDscp & lef);
	void mapLefSites(const BookshelfDscp & dscp, LefDscp & lef);
	void mapLefCells(const BookshelfDscp & dscp, LefDscp & lef);
	void createMetal(LefDscp & lef);
	void mapDefRows(const BookshelfDscp & dscp, DefDscp & def);
	void mapDefHeader(const BookshelfDscp & dscp, DefDscp & def);
	void mapDefComponents(const BookshelfDscp &dscp, DefDscp &def);
}; // end class 

#endif /* BOOKSHELFMAPPER_H */

