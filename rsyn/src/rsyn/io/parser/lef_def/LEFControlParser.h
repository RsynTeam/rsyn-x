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
 

#ifndef LEFCONTROLPARSER_H
#define	LEFCONTROLPARSER_H

#include <string>
	using std::string;
#include <iostream>
	using std::cout;
#include <vector>
	using std::vector;

//#include "rsyn/io/legacy/PlacerInternals.h"
#include "rsyn/phy/util/LefDescriptors.h"
	
//! LEF file must be parsed first than DEF file
	
class LEFControlParser {
public:
	LEFControlParser();
	//void parseLEF(const std::string &filename, Library &library) ;
	void parseLEF(const std::string &filename, LefDscp & dscp) ;
	virtual ~LEFControlParser();
	
}; // end class

#endif	/* LEFCONTROLPARSER_H */

