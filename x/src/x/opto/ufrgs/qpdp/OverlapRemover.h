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
 * File:   OverlapRemover.h
 * Author: mpfogaca
 *
 * Created on 26 de Setembro de 2016, 09:52
 */

#ifndef OVERLAPREMOVER_H
#define OVERLAPREMOVER_H

#include "rsyn/session/Session.h"
#include "rsyn/core/Rsyn.h"
#include "rsyn/util/Stepwatch.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "x/util/BlockageControl.h"
#include "x/infra/iccad15/utilICCAD15.h"
#include <lemon/lp.h>

namespace Rsyn {
class LibraryCharacterizer;
class Timer;
} // end namespace

namespace ICCAD15 {

class Infrastructure;
using namespace lemon;
	
class OverlapRemover : public Rsyn::Process {
private:
	Rsyn::Design clsDesign;
	Rsyn::Module clsModule;
	Rsyn::Timer *clsTimer;
	Rsyn::PhysicalDesign clsPhysicalDesign;
	Infrastructure* clsInfrastructure;
	const Rsyn::LibraryCharacterizer* clsLibCharacterizer;
	ICCAD15::BlockageControl* clsBlockageControl;
       
	LegalizationMethod legMode;
	
	int verbosityLevel = 100;
	bool debug = true;
	
	double M;
	double minBlockArea;
	
	void doRemoveOverlap(const Rsyn::Cell cell);
	void doRemoveOverlap();
	
	void setupMIP( 
			const Rsyn::Cell cell, Mip& mip,
			Mip::Col& x,
			Mip::Col& y);
	
	void setupCellCosts(
			const Rsyn::Cell cell,
			Mip& mip,
			Mip::Expr& cost, 
			Mip::Col& x, 
			Mip::Col& y,
			Bounds& problemBoundingBox);
	
	void findBlockagesOverlaping(
			const Bounds area,
			std::vector<Bounds>& blockages);
	
	void setupConstraints(
			Mip& mip,
			Mip::Col& x,
			Mip::Col& y,
			std::vector<Bounds>& blockages);
	
	void updatePosition(Rsyn::Cell cell, const DBUxy pos);	
public:
	OverlapRemover() : legMode(LegalizationMethod::LEG_NEAREST_WHITESPACE) {}
    
	bool run(const Rsyn::Json& params) override;	
}; // end class

} // end namespace

#endif /* OVERLAPREMOVER_H */

