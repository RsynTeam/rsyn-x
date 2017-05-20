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

#include "rsyn/engine/Engine.h"


#include "rsyn/io/reader/BookshelfReader.h"
#include "rsyn/io/reader/DesignPositionReader.h"
#include "rsyn/io/reader/ICCAD15Reader.h"
#include "rsyn/io/reader/ICCAD17Reader.h"
#include "rsyn/io/reader/ISPD2012Reader.h"
#include "rsyn/io/reader/ISPD2014Reader.h"

#include "x/io/reader/GenericReader.h"
#include "x/io/reader/ICCAD15Reader.h"

// Registration
namespace Rsyn {

void Engine::registerReaders() {
	//ICCAD 2015 contest 
	registerReader<Rsyn::ICCAD15Reader>("rsyn.iccad2015");
	registerReader<Rsyn::ICCAD17Reader>("rsyn.iccad2017");
	registerReader<Rsyn::BookshelfReader>("bookshelf");
	registerReader<Rsyn::ISPD2014Reader>("ispd14");
	registerReader<Rsyn::ISPD2012Reader>("ispd12");
	registerReader<Rsyn::DesignPositionReader>("loadDesignPosition");

	registerReader<Rsyn::GenericReader>("generic");
	registerReader<ICCAD15::ICCAD15Reader>("iccad2015");
} // end method 
} // end namespace 