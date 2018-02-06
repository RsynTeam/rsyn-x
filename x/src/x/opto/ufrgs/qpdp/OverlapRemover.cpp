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
 * File:   OverlapRemover.cpp
 * Author: mpfogaca
 * 
 * Created on 26 de Setembro de 2016, 09:52
 */

#include "OverlapRemover.h"
#include "rsyn/model/library/LibraryCharacterizer.h"
#include "rsyn/model/timing/Timer.h"
#include "x/infra/iccad15/Infrastructure.h"
#include <lemon/lp.h>

namespace ICCAD15 {

bool OverlapRemover::run(const Rsyn::Json& params) {
	Rsyn::Session session;

	clsDesign = session.getDesign();
	clsModule = session.getDesign().getTopModule();
	clsTimer = session.getService("rsyn.timer");
	clsPhysicalDesign = session.getPhysicalDesign();
	clsInfrastructure = session.getService("ufrgs.ispd16.infra");
	clsLibCharacterizer = session.getService("rsyn.libraryCharacterizer");
	clsBlockageControl = session.getService("ufrgs.blockageControl");
	
	M = clsPhysicalDesign.getPhysicalModule(clsModule).getSize(X);
	minBlockArea = 5000.0 * clsPhysicalDesign.getRowHeight() * 
		clsPhysicalDesign.getRowSiteWidth();
	
	if (verbosityLevel > 5) { 
		std::cout << "rowHeight = " << clsPhysicalDesign.getRowHeight() << "\n";
		std::cout << "siteWidth = " << clsPhysicalDesign.getRowSiteWidth() << "\n";
		std::cout << "minBlockArea = " << minBlockArea << "\n";
	} // end if
	
	if (params.count("legMode")) {
		if (params["legMode"] == "nearest_whitespace")
			legMode = LegalizationMethod::LEG_NEAREST_WHITESPACE;
		else if (params["legMode"] == "min_linear_displacement")
			legMode = LegalizationMethod::LEG_MIN_LINEAR_DISPLACEMENT;
		else if (params["legMode"] == "exact_location")
			legMode = LegalizationMethod::LEG_EXACT_LOCATION;
		else if ((params["legMode"] == "none"))
			legMode = LegalizationMethod::LEG_NONE;
		else std::cout << "[WARNING] Legalization method not supported\n";
	} // end if
	
	if (!params.count("cell")) {
		doRemoveOverlap();
		return true;
	} // end if
	
	std::string cellName = params["cell"];
	
	Rsyn::Cell cell = clsDesign.findCellByName( cellName );
	if (!cell) {
		std::cout << "[BUG] Cell " << cellName << " not found.\n";
	} // end if
		
	doRemoveOverlap(cell);
	
	return true;
} // end method

//------------------------------------------------------------------------------

void OverlapRemover::doRemoveOverlap() {
	std::vector<Rsyn::Timer::PathHop> path;
	clsTimer->queryTopCriticalPath(Rsyn::LATE, path);
	const double pathLengthBefore = 
		clsInfrastructure->computePathManhattanLength(path);
	
	std::vector<Rsyn::Cell> nonCriticalCells;
	for (Rsyn::Instance instance : clsModule.allInstancesInTopologicalOrder()) {		
		if (instance.getType() != Rsyn::InstanceType::CELL)
			continue;
		
		Rsyn::Cell cell = instance.asCell();
		
		PhysicalCell pCell = 
				clsPhysicalDesign.getPhysicalCell(cell);
		
		if (cell.isFixed() || cell.isMacroBlock() || clsInfrastructure->isLegalized(cell))
			continue;
		
		if (clsTimer->getCellCriticality(cell, Rsyn::LATE) && 
				clsBlockageControl->hasOverlapWithMacro(cell)) {
			clsTimer->updateTimingIncremental();
			clsInfrastructure->updateQualityScore();
			double qos = std::max(clsInfrastructure->getQualityScore(), 0.01);
			doRemoveOverlap(cell);
			std::cout << "\t" << cell.getName() << " ";
			clsTimer->updateTimingIncremental();
			clsInfrastructure->updateQualityScore();
			double newQos = clsInfrastructure->getQualityScore();
			std::cout << qos << " -> " << newQos;
			if ( newQos/qos < 0.95 )
				std::cout << " (QoS deprecated)";
			std::cout << "\n";
		} else {
			clsInfrastructure->legalizeCell(cell, legMode, false);
		} // end if
	} // end for
	
	clsTimer->updatePath(path);
	clsInfrastructure->computePathManhattanLength(path);
	const double pathLengthAfter = 
		clsInfrastructure->computePathManhattanLength(path);
	
	clsInfrastructure->reportPathManhattanLengthBySegment(path);
	std::cout << "**** Path " << path.front().getPin().getFullName();
	std::cout << " -> " << path.back().getPin().getFullName() << " ****\n";
	std::cout << " Length before: " << pathLengthBefore << "\n";
	std::cout << " Length now: " << pathLengthAfter << "\n";
	std::cout << " Change (%): ";
	std::cout << std::fixed;
    std::cout << std::setprecision(2);
	std::cout << 100*pathLengthAfter/pathLengthBefore << "\n";	
} // end method

//------------------------------------------------------------------------------

void OverlapRemover::doRemoveOverlap(const Rsyn::Cell cell) {
	if (verbosityLevel>0)
		Stepwatch watch("Removing overlap (cell: " + cell.getName() + ")");
	
	lemon::Mip mip;
	
	Mip::Col x = mip.addCol();
	Mip::Col y = mip.addCol();
			
	setupMIP(cell, mip, x, y);
	
//	mip.write("problem.LP", "LP");
	if (verbosityLevel > 0)
		std::cout << "Solving MIP...";
	mip.solve();
	if (verbosityLevel > 0)
		std::cout << " Done.\n";
	
	if (mip.type() != Mip::OPTIMAL)
		std::cout << "[BUG] The ILP problem has no optimal solution!\n"; 
	
	if (verbosityLevel > 0)
		std::cout << "MIP returned value: " << mip.solValue() << "\n";	

	updatePosition(cell, DBUxy((DBU) mip.sol(x), (DBU) mip.sol(y)));
} // end method

//------------------------------------------------------------------------------

void OverlapRemover::setupMIP(
		const Rsyn::Cell cell, Mip& mip,
		Mip::Col& x, Mip::Col& y) {
	
	Mip::Expr cost;
	
	PhysicalCell phCell = clsPhysicalDesign.getPhysicalCell(cell);
	Bounds problemBoundingBox( 
			phCell.getCoordinate(LOWER, X),
			phCell.getCoordinate(LOWER, Y),
			phCell.getCoordinate(UPPER, X),
			phCell.getCoordinate(UPPER, Y));
	
	setupCellCosts(cell, mip, cost, x, y, problemBoundingBox);
	
	if (verbosityLevel > 1)
		std::cout << "Neighbors BoundingBox: " << problemBoundingBox << "\n";
	
	problemBoundingBox.scaleCentralized(1.05);
	
	if (verbosityLevel > 1)
		std::cout << "Scaled BoundingBox (+5%): " << problemBoundingBox << "\n";
	
	std::vector<Bounds> blockages;
	findBlockagesOverlaping(problemBoundingBox, blockages);
	
	setupConstraints(mip, x, y, blockages);
	
	mip.min();
	mip.obj(cost);
} // end method

//------------------------------------------------------------------------------

void OverlapRemover::setupCellCosts(
		const Rsyn::Cell cell, 
		Mip& mip,
		Mip::Expr& cost,
		Mip::Col& x, 
		Mip::Col& y, 
		Bounds& problemBoundingBox) {
	
	if (!clsTimer->getCellCriticality(cell, Rsyn::LATE)) {
		std::cout << "[WARNING] Cell " << cell.getName() << " is not critical.\n";
		return;
	}

	Mip::Expr inputExpr;
	Number inputCriticality = -std::numeric_limits<Number>::max();
	Number inputDriverResistance;
	for (Rsyn::Pin cellPin : cell.allPins(Rsyn::IN)) {
		const Number criticality = clsTimer->getPinCriticality(cellPin, Rsyn::LATE);
		
		if (FloatingPoint::approximatelyZero(criticality) ||
			!cellPin.getNet() || 
			!cellPin.getNet().getAnyDriver() ||
			cellPin.getNet().getAnyDriver().getInstance().getType() != Rsyn::InstanceType::CELL) {
			continue; 
		} // end if
				
		const Rsyn::Cell neighbor = 
					cellPin.getNet().getAnyDriver().getInstance().asCell();

		Rsyn::PhysicalCell phNeighbor = 
					clsPhysicalDesign.getPhysicalCell(neighbor);
		
		DBUxy neighborPosition;
		if (neighbor.isMacroBlock()) {
				neighborPosition = clsPhysicalDesign.getPinPosition(cellPin.getNet().getAnyDriver());
		} else {
				neighborPosition = phNeighbor.getCenter();
		} // end if

		problemBoundingBox[LOWER][X] =
			std::min(problemBoundingBox[LOWER][X], neighborPosition.x);
		problemBoundingBox[LOWER][Y] =
			std::min(problemBoundingBox[LOWER][Y], neighborPosition.y);
		problemBoundingBox[UPPER][X] =
			std::max(problemBoundingBox[UPPER][X], neighborPosition.x);
		problemBoundingBox[UPPER][Y] =
			std::max(problemBoundingBox[UPPER][Y], neighborPosition.y);
		
		/* min: |x0 - x| */
		Mip::Col tPlus = mip.addCol();
		Mip::Col tMinus = mip.addCol();
		
		cost += criticality*(tPlus + tMinus);
		mip.addRow(tPlus - tMinus == neighborPosition.x - x);
		mip.addRow(tPlus >= 0);
		mip.addRow(tMinus >= 0);
		
		/* min: |y0 - y| */
		Mip::Col uPlus = mip.addCol();
		Mip::Col uMinus = mip.addCol();
		
		cost += criticality*(uPlus + uMinus);
		mip.addRow(uPlus - uMinus == neighborPosition.y - y);
		mip.addRow(uPlus >= 0);
		mip.addRow(uMinus >= 0);	
		
		if (clsTimer->getCellCriticality(neighbor, Rsyn::LATE) > inputCriticality) {
			inputCriticality = clsTimer->getCellCriticality(neighbor, Rsyn::LATE);
			inputDriverResistance = clsLibCharacterizer->getCellMaxDriverResistance(neighbor, Rsyn::LATE);
			inputExpr = tPlus + tMinus + uPlus + uMinus;
		} // end if
	} // end for
	
	Mip::Expr outputExpr;
	Number outputCriticality = -std::numeric_limits<Number>::max();
	Number outputDriverResistance = clsLibCharacterizer->getCellMaxDriverResistance(cell, Rsyn::LATE);
	for (Rsyn::Pin cellPin : cell.allPins(Rsyn::OUT)) {
		if (FloatingPoint::approximatelyZero(clsTimer->getPinCriticality(cellPin, Rsyn::LATE)) ||
			!cellPin.getNet() ||
			!cellPin.getNet().getAnyDriver()) {
			continue;		
		} // end if
		
		const Rsyn::Net net = cellPin.getNet();
		
		for (Rsyn::Pin neighborPin : net.allPins(Rsyn::IN)) {
			const Number criticality = 
				clsTimer->getPinCriticality(neighborPin, Rsyn::LATE);
			
			if (FloatingPoint::approximatelyZero(criticality) ||
				neighborPin.getInstance().getType() != Rsyn::InstanceType::CELL) {
				continue;
			} // end if
						
			const Rsyn::Cell neighbor = neighborPin.getInstance().asCell();

			Rsyn::PhysicalCell phNeighbor = 
					clsPhysicalDesign.getPhysicalCell(neighbor);
			
			DBUxy neighborPosition;
			if (neighbor.isMacroBlock()) {
				neighborPosition = clsPhysicalDesign.getPinPosition(neighborPin);
			} else {
				neighborPosition = phNeighbor.getCenter();
			} // end if

			problemBoundingBox[LOWER][X] =
				std::min(problemBoundingBox[LOWER][X], neighborPosition.x);
			problemBoundingBox[LOWER][Y] =
				std::min(problemBoundingBox[LOWER][Y], neighborPosition.y);
			problemBoundingBox[UPPER][X] =
				std::max(problemBoundingBox[UPPER][X], neighborPosition.x);
			problemBoundingBox[UPPER][Y] =
				std::max(problemBoundingBox[UPPER][Y], neighborPosition.y);
			
			/* min: |x0 - x| */
			Mip::Col tPlus = mip.addCol();
			Mip::Col tMinus = mip.addCol();
			//cost += R*tPlus + R*tMinus;
			cost += criticality*(tPlus + tMinus);
			mip.addRow(tPlus - tMinus == neighborPosition.x - x);
			mip.addRow(tPlus >= 0);
			mip.addRow(tMinus >= 0);
			
			/* min: |y0 - y| */
			Mip::Col uPlus = mip.addCol();
			Mip::Col uMinus = mip.addCol();
			
			//cost += R*uPlus + R*uMinus;
			cost += criticality*(uPlus + uMinus);
			mip.addRow(uPlus - uMinus == neighborPosition.y - y);
			mip.addRow(uPlus >= 0);
			mip.addRow(uMinus >= 0);
			
			if (clsTimer->getCellCriticality(neighbor, Rsyn::LATE) > outputCriticality) {
				outputCriticality = clsTimer->getCellCriticality(neighbor, Rsyn::LATE);
//				outputDriverResistance = clsLibCharacterizer->getCellMaxDriverResistance(neighbor, Rsyn::LATE);
				outputExpr = tPlus + tMinus + uPlus + uMinus;
			} // end if
		} // end for
	} // end for
	
	Mip::Col zPlus = mip.addCol();
	Mip::Col zMinus = mip.addCol();
	cost += 0.01*(zPlus + zMinus);
	
	inputDriverResistance = std::max(std::min(inputDriverResistance,100.0f),
			clsLibCharacterizer->getLibraryMinDriverResistance(Rsyn::LATE));
	outputDriverResistance = std::max(std::min(outputDriverResistance,100.0f),
			clsLibCharacterizer->getLibraryMinDriverResistance(Rsyn::LATE));
	
	if (verbosityLevel > 2) {
		std::cout << " Input R " << inputDriverResistance << "\n";
		std::cout << " Output R " << outputDriverResistance << "\n";
	} // end if
	
	mip.addRow(zPlus - zMinus == inputDriverResistance * inputExpr - 
								 outputDriverResistance * outputExpr);
	mip.addRow(zPlus >= 0);
	mip.addRow(zMinus >= 0);
} // end method

//------------------------------------------------------------------------------

void OverlapRemover::findBlockagesOverlaping(
		const Bounds area,
		std::vector<Bounds>& blockages) {
	
	/* [TODO] Use blockage infrastructure from ICCAD to optimize the 
	 * code performance. */
	for (Rsyn::Instance instance : clsModule.allInstances()) {
		if (instance.getType() != Rsyn::InstanceType::CELL)
			continue;
		
		PhysicalCell pCell = 
				clsPhysicalDesign.getPhysicalCell(instance.asCell());
		
		if (!pCell.isMacroBlock() || pCell.getArea() <= minBlockArea)
			continue;
				
		if (pCell.getBounds().overlapArea(area) > 0) {
			if (verbosityLevel > 0)
				std::cout<< "Block: " << instance.getName() << "\n";
			if (pCell.hasLayerBounds()) {
				const Rsyn::PhysicalLibraryCell &phLibCell = 
						clsPhysicalDesign.getPhysicalLibraryCell(instance.asCell());
				for (Bounds obs : phLibCell.allLayerObstacles()) {
					obs.translate(pCell.getPosition());
					blockages.push_back(obs);
				}
			} else {
				blockages.push_back(pCell.getBounds());
			} // end if
		}
	} // end for
} // end method

//------------------------------------------------------------------------------

void OverlapRemover::setupConstraints(
		Mip& mip,
		Mip::Col& x,
		Mip::Col& y,
		std::vector<Bounds>& blockages) {
	
	Rsyn::PhysicalModule phModule = clsPhysicalDesign.getPhysicalModule(clsModule);
	mip.colBounds(x, phModule.getCoordinate(LOWER, X), phModule.getCoordinate(UPPER, X));
	mip.colBounds(y, phModule.getCoordinate(LOWER, Y), phModule.getCoordinate(UPPER, Y));
	
	if (verbosityLevel>0) {
		if (blockages.size()) {
			std::cout << "Obstacles:\n";
		} else {
			std::cout << "[WARNING] No blockage constraints found during overlap removal\n";
			return;
		} // end if
	} // end if
	
	for (const Bounds obstacle : blockages) {
		if (verbosityLevel>0)
			std::cout << "\t" << obstacle << "\n";
		
		Mip::Col c1 = mip.addCol(); mip.colType(c1, Mip::INTEGER);
		Mip::Col c2 = mip.addCol(); mip.colType(c2, Mip::INTEGER);
		Mip::Col c3 = mip.addCol(); mip.colType(c3, Mip::INTEGER);
		
		mip.addRow(x >= obstacle[UPPER][X] + 1 - M*( c1 ) - M*( c3 ));
		mip.addRow(x <= obstacle[LOWER][X] - 1 + M*(1-c1) + M*( c3 ));
		mip.addRow(y >= obstacle[UPPER][Y] + 1 - M*( c2 ) - M*(1-c3));
		mip.addRow(y <= obstacle[LOWER][Y] - 1 + M*(1-c2) + M*(1-c3));
		
		mip.addRow(0<= c1 <= 1);
		mip.addRow(0<= c2 <= 1);
		mip.addRow(0<= c3 <= 1);
	} // end for
	
} // end method

//------------------------------------------------------------------------------

void OverlapRemover::updatePosition(Rsyn::Cell cell, const DBUxy pos) {
	const Rsyn::PhysicalCell pCell = clsPhysicalDesign.getPhysicalCell(cell);
			
	if( !clsInfrastructure->moveCell(
			pCell, 
			pos.x - pCell.getWidth()/2, 
			pos.y - pCell.getHeight()/2,
			legMode) ) {
		std::cout << "[BUG] Problem legalizing cell " << cell.getName() << "\n";
	} // end if
} // end method

//------------------------------------------------------------------------------

} // end namespace

