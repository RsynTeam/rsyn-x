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
 * File:   ISPD2012Reader.h
 * Author: jucemar
 *
 * Created on 20 de Fevereiro de 2017, 19:09
 */

#ifndef RSYN_ISPD2012READER_H
#define RSYN_ISPD2012READER_H

#include "rsyn/engine/Engine.h"
#include "rsyn/core/Rsyn.h"
#include "rsyn/3rdparty/json/json.hpp"
#include "rsyn/util/RCTreeExtractor.h"

namespace Rsyn {

class SizingISPDReader : public Reader {
protected:
	Rsyn::Engine engine;
	Rsyn::Design clsDesign;
	Rsyn::Module clsModule;

public:
	SizingISPDReader() {}
	SizingISPDReader(const SizingISPDReader& orig) {}
	virtual ~SizingISPDReader() {}
	void load(Engine engine, const Json &params);

}; // end class 

} // end namespace 

#endif /* RSYN_ISPD2012READER_H */

