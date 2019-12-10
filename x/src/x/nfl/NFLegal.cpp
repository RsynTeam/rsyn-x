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
 * File:   NFLegalBase.cpp
 * Author: isoliveira
 * 
 * Created on 12 de Março de 2018, 21:26
 */

#include <iostream>

#include "NFLegal.h"
#include "rsyn/util/Stepwatch.h"
#include "rsyn/util/StreamStateSaver.h"
#include "x/jezz/Jezz.h"
#include "rsyn/model/congestion/DensityGrid/DensityGrid.h"


namespace NFL {

void NFLegal::start(const Rsyn::Json& params) {
	std::cout << "Starting NFLegal ...\n";
	clsDesign = clsSession.getDesign();
	clsModule = clsDesign.getTopModule();
	clsPhDesign = clsSession.getPhysicalDesign();
	clsPhModule = clsPhDesign.getPhysicalModule(clsModule);
	clsLegalInitPos = clsDesign.createAttribute();

	// setting cell overlap mode optimization
	std::string cellOverlapOpto = params.value("cellOverlapOpto", "nflegal");
	if (cellOverlapOpto.compare("nflegal") == 0) {
		clsOptions.setCellOverlapOpto(CellOverlapOpto::NFLEGAL);
	} else if (cellOverlapOpto.compare("jezz") == 0) {
		clsOptions.setCellOverlapOpto(CellOverlapOpto::JEZZ);
	} else {
		clsOptions.setCellOverlapOpto(CellOverlapOpto::NONE);
	} // end if-else 
	clsMode = params.value("mode", clsMode);
	clsDisableCellSpreading = params.value("disableCellSpreading", clsDisableCellSpreading);


	{ // run legalization of global placement solution to minimize cell displacement.
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("legalizePlace");
		dscp.setDescription("Legalize global placement to minimize cell displacement.");

		clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			legalizePlace();
		});
	} // end block


	{ // run legalization of global placement solution to minimize cell displacement.
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("initNFLegal");
		dscp.setDescription("Initialize NFLegal data structures.");

		clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			if (!clsInitialized) {
				init();
			} // end if 
		});
	} // end block

	{ // report legalized solution
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("reportNFLegal");
		dscp.setDescription("Report NFLegal legalized solution.");

		clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			if (clsInitialized) {
				reportNFLegal();
			} // end if 
		});
	}

	{ // report legalized solution
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("reportAreaOverlap");
		dscp.setDescription("Report area overlap.");

		clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			if (clsInitialized) {
				reportAreaOverlap();
			} // end if 
		});
	}

	{ // report global placement solution
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("reportGlobalPlacement");
		dscp.setDescription("Report global placement solution.");

		clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			reportCircuit(std::cout);
		});

	}

} // end method

// -----------------------------------------------------------------------------

void NFLegal::stop() {

} // end method

// -----------------------------------------------------------------------------

void NFLegal::placeCell(Rsyn::PhysicalCell phCell, const DBUxy pos) {
	// Observers are not notified when cells are moved. 
	//const bool notify = clsNetworkFlowSelection == NetworkFlowSelection::TIMING_DRIVEN;
	clsPhDesign.placeCell(phCell, pos);
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::moveNode(Node * node, Segment * src, Segment *sink, const DBUxy targetPos) {
	removeNode(src, node);
	placeCell(node->getPhysicalCell(), targetPos);
	insertNode(sink, node);
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::getMaxSupply() {
	DBU maxSupply = -std::numeric_limits<DBU>::max();
	for (Row & row : clsRows) {
		for (Segment & segment : row.allSegments()) {
			for (Bin & bin : segment.allBins()) {
				maxSupply = std::max(maxSupply, bin.getSupply());
			} // end for 
		} // end for 
	} // end for 
	return maxSupply;
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::getMaxDemand() {
	DBU maxDemand = -std::numeric_limits<DBU>::max();
	for (Row & row : clsRows) {
		for (Segment & segment : row.allSegments()) {
			for (Bin & bin : segment.allBins()) {
				maxDemand = std::max(maxDemand, bin.getDemand());
			} // end for 
		} // end for 
	} // end for 
	return maxDemand;
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::legalizePlace() {
	if (!clsInitialized) {
		init();
	} // end if 

	computeAbu();

	if (!clsDisableCellSpreading) {
		cellSpreading();
	} // end if 

	cellRemovalOverlap();

	// Update and report metrics
	reportNFLegal();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::init() {
	Stepwatch watch("Initializing Network Flow-based Legalizer (NFL)");
	clsInitialized = true;
	clsInitialHpwl = clsPhDesign.getHPWL();
	computeBinWidth();
	initRows();
	initBlockages();
	connectVerticalSegments();
	connectVerticalBins();
	connectVerticalSegmentsBinsThroughBlockages();
	initNodes(); // Requiring high runtime to initialize instances
	removeBlockageOverlap();
	updateInitLegalPos();
	watch.finish();
	clsRuntime += watch.getElapsedTime();


} // end method

// -----------------------------------------------------------------------------

void NFLegal::computeBinWidth() {
	int numCells = 0;
	DBU totalWidth = 0;
	
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.getType() != Rsyn::CELL)
			continue;
		if (inst.isFixed())
			continue;
		
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(inst.asCell());
		totalWidth += phCell.getWidth();
		numCells++;
	} // end for 
	clsBinLength[Y] = clsPhDesign.getRowHeight();
	clsBinLength[X] = (20 * totalWidth) / static_cast<double>(numCells);
	
	
	
//	
	//	const int numCells = clsPhDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE);
	//	std::vector<DBU> cellWidth;
	//	cellWidth.reserve(numCells);
	//	DBU totalCellWidth = 0;
	//	
	//	for (Rsyn::Instance inst : clsModule.allInstances()) {
	//		if (inst.getType() != Rsyn::CELL)
	//			continue;
	//		if (inst.isFixed())
	//			continue;
	//		
	//		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(inst.asCell());
	//		totalCellWidth += phCell.getWidth();
	//		cellWidth.push_back(phCell.getWidth());
	//	} // end for 
	//	std::sort(cellWidth.begin(), cellWidth.end());
	//
	//	// bin width is defined at Section IV.B in BONNPLACE Legalization : 
	//	// minimizing movement by iterative augmentation from Ulrich Brenner 
	//	// https://doi.org/10.1109/TCAD.2013.2253834
	//	DBU medianWidth; // Using the median cell size to define the bin width
	//	if (cellWidth.size() % 2 == 0) {
	//		int pos = cellWidth.size() / 2;
	//		medianWidth = static_cast<DBU> ((cellWidth[pos - 1] + cellWidth[pos]) * 0.5);
	//	} else {
	//		int pos = cellWidth.size() / 2;
	//		medianWidth = cellWidth[pos];
	//	}
	//	int pos99_5 = (int) (cellWidth.size()*0.995); // using the cell width that is bigger than 99.5% 
	//	DBU width99_5 = cellWidth[pos99_5];
	//
	//	double avg = totalCellWidth / static_cast<double>(cellWidth.size());
	//	std::cout<<"************ medianWidth: "<<medianWidth<<" width99_5: "<<width99_5<<" avg: "<<avg<<"\n";
	//	
	//
	//	
	//clsBinLength[X] = std::max(medianWidth * 40, width99_5);

} // end method 

// -----------------------------------------------------------------------------

void NFLegal::initRows() {
	int numRows = clsPhDesign.getNumRows();
	clsRows.reserve(numRows);

	for (Rsyn::PhysicalRow phRow : clsPhDesign.allPhysicalRows()) {
		const Bounds & rowBds = phRow.getBounds();
		clsRowLowerPos = std::min(clsRowLowerPos, rowBds.getCoordinate(LOWER, Y));
		std::size_t id = clsRows.size();
		clsRows.push_back(Row());
		Row & row = clsRows.back();
		row.setId(id);
		row.setBounds(rowBds);
		row.setBinLength(clsBinLength);
		row.setNFLegal(this);
		row.setPhysicalRow(phRow);
	} // end for 

} // end method

// -----------------------------------------------------------------------------

void NFLegal::initBlockages() {
	const Bounds & modBounds = clsPhModule.getBounds();
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.isMovable() || (inst.getType() != Rsyn::CELL)) {
			continue;
		} // end if 
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(inst.asCell());
		if (phCell.hasLayerBounds()) {
			Rsyn::PhysicalLibraryCell phLibCell = clsPhDesign.getPhysicalLibraryCell(inst.asCell());
			for (Bounds bounds : phLibCell.allLayerObstacles()) {

				DBUxy lower = bounds.getCoordinate(LOWER);
				DBUxy pos = phCell.getPosition();
				lower[X] = lower[X] + pos[X];
				lower[Y] = lower[Y] + pos[Y];
				bounds.moveTo(lower);
				Bounds overlap = bounds.overlapRectangle(modBounds);
				if (overlap.computeLength(X) == 0 || overlap.computeLength(Y) == 0)
					continue;
				addBlockage(overlap);
			} // end for 
		} else {
			const Bounds & cellBounds = phCell.getBounds();
			Bounds overlap = cellBounds.overlapRectangle(modBounds);
			if (overlap.computeLength(X) == 0 || overlap.computeLength(Y) == 0)
				continue;

			addBlockage(overlap);
		} // end if-else 
	} // end for 
	int numBins = 0;
	for (Row & row : clsRows) {
		row.initBlockages();
		row.initSegments();
		numBins += row.getNumBins();
	} // end for 
	clsBins.reserve(numBins);
	for (Row & row : clsRows) {
		for (Segment & segment : row.allSegments()) {
			for (Bin & bin : segment.allBins()) {
				clsBins.push_back(&bin);
			} // end for 
		} // end for 
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::addBlockage(const Bounds& block) {
	const int low = getRowIndex(block.getCoordinate(LOWER, Y));
	const int upp = getRowIndex(block.getCoordinate(UPPER, Y));

	for (int index = low; index < upp; ++index) {
		Row & row = clsRows[index];
		row.addBlockage(block);
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::connectVerticalSegments() {
	Row * lower = &clsRows.front();
	Segment * lowerSegment = lower->getFrontSegment();
	for (int index = 1; index < clsRows.size(); ++index) {
		Row * row = &clsRows[index];
		Segment * first = row->getFrontSegment();
		connectVerticalSegments(lowerSegment, first);
		lowerSegment = first;
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::connectVerticalSegments(Segment * lowerFirst, Segment * upperFirst) {
	Segment * lower = lowerFirst;
	Segment * upper = upperFirst;

	while (lower && upper) {
		const DBU lowerLeft = lower->getCoordinate(LOWER, X);
		const DBU upperLeft = upper->getCoordinate(LOWER, X);
		const DBU lowerRight = lower->getCoordinate(UPPER, X);
		const DBU upperRight = upper->getCoordinate(UPPER, X);

		const bool condition1 = lowerLeft >= upperLeft && lowerLeft < upperRight;
		const bool condition2 = upperLeft >= lowerLeft && upperLeft < lowerRight;

		if (condition1 || condition2) {
			lower->addUpper(upper);
			upper->addLower(lower);
		} // end if 

		if (lowerRight <= upperRight) {
			lower = lower->getRightSegment();
		} // end if 

		if (upperRight <= lowerRight) {
			upper = upper->getRightSegment();
		} // end if 
	} // end while 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::connectVerticalSegmentsBinsThroughBlockages() {
	for (int id = 1; id < getNumRows(); ++id) {
		Row & lowerRow = clsRows[id - 1];
		Row & upperRow = clsRows[id];
		if (!upperRow.hasBlockages()) {
			continue;
		} // end if 

		Segment * lower = lowerRow.getFrontSegment();
		const std::deque<Bounds> & blocks = upperRow.allBlockages();
		int blockId = 0;
		while (lower && blockId < blocks.size()) {
			const Bounds & lowerBds = lower->getBounds();
			const Bounds & blockBds = blocks[blockId];
			const bool leftCondition = blockBds[LOWER][X] > lowerBds[LOWER][X] && blockBds[LOWER][X] < lowerBds[UPPER][X];
			const bool rightCondition = blockBds[UPPER][X] > lowerBds[LOWER][X] && blockBds[UPPER][X] < lowerBds[UPPER][X];

			if (leftCondition || rightCondition) {
				connectVerticalSegmentsBinsThroughBlockages(lower, &upperRow, blockBds);
			} // end if 

			if (blockBds[UPPER][X] <= lowerBds[UPPER][X]) {
				++blockId;
			} // end if 

			if (lowerBds[UPPER][X] < blockBds[UPPER][X]) {
				lower = lower->getRightSegment();
			} // end if 
		} // end while 
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::connectVerticalSegmentsBinsThroughBlockages(Segment * lower, Row * upper, const Bounds & block) {
	int rowId = upper->getId() + 1;
	DBU leftX = std::max(block[LOWER][X], lower->getPosition(X));
	while (rowId < getNumRows() && leftX < block[UPPER][X]) {
		Row * row = getRowByIndex(rowId);
		DBUxy pos = row->getPosition();
		pos[X] = leftX;
		Segment * segment = row->getNearestSegment(pos, 0.1 * getDefaultBinLength(X));
		if (segment) {
			const Bounds & segBds = segment->getBounds();
			if (pos[X] >= segBds[LOWER][X] && pos[X] < segBds[UPPER][X]) {
				segment->addLower(lower);
				lower->addUpper(segment);
				connectVerticalBinsThroughBlockages(lower, segment, leftX, block[UPPER][X]);
				leftX = segBds[UPPER][X];
			} // end if 
		} // end if 
		++rowId;
	} // end while 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::connectVerticalBins() {
	Row * lower = &clsRows.front();
	Segment * lowerSegment = lower->getFrontSegment();
	Bin * lowerBin = nullptr;
	if (lowerSegment)
		lowerBin = lowerSegment->getFrontBin();
	for (int index = 1; index < clsRows.size(); ++index) {
		Row * row = &clsRows[index];
		Segment * first = row->getFrontSegment();
		Bin * firstBin = nullptr;
		if (first)
			firstBin = first->getFrontBin();
		connectVerticalBins(lowerBin, firstBin);
		lowerBin = firstBin;
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::connectVerticalBins(Bin * lowerFirst, Bin * upperFirst) {
	Bin * lower = lowerFirst;
	Bin * upper = upperFirst;

	while (lower && upper) {
		const DBU lowerLeft = lower->getCoordinate(LOWER, X);
		const DBU upperLeft = upper->getCoordinate(LOWER, X);
		const DBU lowerRight = lower->getCoordinate(UPPER, X);
		const DBU upperRight = upper->getCoordinate(UPPER, X);

		const bool condition1 = lowerLeft >= upperLeft && lowerLeft < upperRight;
		const bool condition2 = upperLeft >= lowerLeft && upperLeft < lowerRight;

		if (condition1 || condition2) {
			lower->addUpper(upper);
			upper->addLower(lower);
		} // end if 

		if (lowerRight <= upperRight) {
			lower = lower->getRight();
		} // end if 

		if (upperRight <= lowerRight) {
			upper = upper->getRight();
		} // end if 
	} // end while 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::connectVerticalBinsThroughBlockages(Segment * lower, Segment * upper,
	const DBU leftX, const DBU rightX) {
	Bin * lowerBin = lower->getBinByPosition(leftX);
	Bin * upperBin = upper->getBinByPosition(leftX);

	while (lowerBin && upperBin && lowerBin->getPosition(X) < rightX
		&& upperBin->getPosition(X) < rightX) {
		lowerBin->addUpper(upperBin);
		upperBin->addLower(lowerBin);
		DBU lowerRightX = lower->getCoordinate(UPPER, X);
		DBU upperRightX = upper->getCoordinate(UPPER, X);
		if (lowerRightX <= upperRightX) {
			lowerBin = lowerBin->getRight();
		} // end if 
		if (upperRightX <= lowerRightX) {
			upperBin = upperBin->getRight();
		} // end if 
	} // end while 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::initNodes() {
	const int numElements = clsPhDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE);
	clsNodes.reserve(numElements);
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.isFixed() || inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(inst.asCell());
		alignCellToRow(phCell);
		const int id = clsNodes.size();
		clsNodes.push_back(Node());
		Node & legInst = clsNodes.back();
		legInst.setId(id);
		legInst.setPhysicalCell(phCell);
		if (!insertNode(&legInst))
			clsDirtyNodes.push_back(&legInst);
		clsMapInstancesToNodes[inst] = id;
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::alignCellToRow(Rsyn::PhysicalCell phCell) {
	Row * upperRow = &clsRows.back();
	const DBU upperRowY = upperRow->getPosition(Y);
	DBUxy pos = phCell.getPosition();
	Row * row = getRow(pos[Y]);
	DBU posY = row->getPosition(Y);
	if (posY != pos[Y]) {
		if (pos[Y] >= row->getCenter(Y) && posY < upperRowY)
			pos[Y] = row->getCoordinate(UPPER, Y);
		else
			pos[Y] = posY;
		placeCell(phCell, pos);
	} // end if 
} // end method

// -----------------------------------------------------------------------------

bool NFLegal::insertNode(Node * inst) {
	const DBU yPos = inst->getPosition(Y);
	Row * row = getRow(yPos);
	return row->insertNode(inst);
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::removeBlockageOverlap() {
	for (Node * inst : clsDirtyNodes) {
		Segment * segment = getNearestSegment(inst);
		if (segment) {
			DBUxy pos = inst->getPosition();
			const Bounds & segBds = segment->getBounds();
			pos[Y] = segBds[LOWER][Y];
			if (pos[X] < segBds[LOWER][X])
				pos[X] = segBds[LOWER][X];
			if (pos[X] > segBds[UPPER][X] - inst->getWidth())
				pos[X] = segBds[UPPER][X] - inst->getWidth();
			placeCell(inst->getPhysicalCell(), pos);
			segment->insertNode(inst);
		} else {
			std::cout << "ERROR: Node: " << inst->getName()
				<< " was not removed overlap to blockage."
				<< "\n";
		} // end if else 
	} // end for 
	clsDirtyNodes.clear();
} // end method 

// -----------------------------------------------------------------------------

Segment * NFLegal::getNearestSegment(Node * inst) {
	const DBUxy initialPos = inst->getPosition();
	int initialRowId = getRowIndex(initialPos[Y]);
	Row * row = getRowByIndex(initialRowId);
	Segment * bestSegment = row->getNearestSegment(initialPos, inst->getWidth());

	DBU displacement = std::numeric_limits<DBU>::max();
	if (bestSegment) {
		//		const DBU segWidth = bestSegment->getWidth();
		//		if (segWidth < getDefaultBinLength(X)) {
		//			const DBU usage = bestSegment->getUsage();
		//			if (usage + segWidth <= inst->getWidth()) {
		//				displacement = bestSegment->getDisplacement(initialPos);
		//			} else {
		//				bestSegment = nullptr;
		//			} // end if-else 
		//		} else {
		displacement = bestSegment->getDisplacement(initialPos);
		//		} // end if-else 
	} //  end if 
	std::queue<std::size_t> rowIds;
	if (initialRowId > 0) {
		rowIds.push(initialRowId - 1);
	} // end if 
	if (initialRowId + 1 < getNumRows()) {
		rowIds.push(initialRowId + 1);
	} //  end if 
	while (!rowIds.empty()) {
		std::size_t id = rowIds.front();
		rowIds.pop();
		Row * row = getRowByIndex(id);
		DBU rowDisp = row->getDisplacement(initialPos[Y], Y);
		if (rowDisp > displacement)
			continue;

		if (id > 0 && id < initialRowId)
			rowIds.push(id - 1);
		if (id + 1 < getNumRows() && id > initialRowId)
			rowIds.push(id + 1);
		Segment * segment = row->getNearestSegment(initialPos, inst->getWidth());
		if (segment) {
			DBU disp = segment->getDisplacement(initialPos);
			if (disp < displacement) {
				//				const DBU segWidth = segment->getWidth();
				//				if (segWidth < getDefaultBinLength(X)) {
				//					const DBU usage = segment->getUsage();
				//					if (usage + segWidth <= inst->getWidth()) {
				//						displacement = disp;
				//						bestSegment = segment;
				//					} // end if 
				//				} else {
				displacement = disp;
				bestSegment = segment;
				//} // end if-else 
			} // end if-else 
		} // end if 
	} // end while 
	return bestSegment;
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::cellSpreading(const int maxIteration) {
	Stepwatch cellSpreadingWatch("Spreading Cells");
	clsMaxIterations = maxIteration;
	clsEnableMaxDisplacement = false;
	performNetworkFlow();
	if (clsIteration >= clsMaxIterations) {
		std::cout << "WARNING: stopped cell spreading algorithm. "
			<< " The maximum number of iterations is reached."
			<< " #Iterarions " << clsIteration << " MaxNumberIterations: " << clsMaxIterations
			<< "\n";
	} // end if 
	cellSpreadingWatch.finish();
	clsCellSpreadingRuntime = cellSpreadingWatch.getElapsedTime();
	clsRuntime += cellSpreadingWatch.getElapsedTime();

} // end method 

// -----------------------------------------------------------------------------

void NFLegal::performNetworkFlow() {
	clsIteration = 0;
	clsTotalOverflow = updateOverflowedBins();
	report(std::cout, true);
	while (clsOverflowedBins.size() > 0 && clsIteration <= clsMaxIterations) {
		if(clsMaxOverfilledAreaRatio < 1.0){
			break;
		} // end if

		updateMaxDisplacement();

		if (clsIteration < 10 || (clsIteration + 1) % 50 == 0 || clsIteration > 800) {
			report(std::cout);
		} // end if 

		for (Bin * bin : clsOverflowedBins) {
			if (bin->getSupply() == 0) {
				continue;
			} // end if 
			DBU flow = computeInitialFlow(bin);

			TNode root;
			root.clsBin = bin;
			root.clsFlow = flow;
			std::deque<TNode*> paths;
			TNode * sink = pathAugmentationBranchBound(&root, paths);


			if (sink) {
				moveCells(sink);
			} // end if 

		} // end for 


		clsIteration++;
		clsTotalOverflow = updateOverflowedBins();

	} // end while 

	report();
} // end method

// -----------------------------------------------------------------------------

bool NFLegal::moveCells(TNode * leaf) {
	TNode * nodeSink = leaf;
	TNode * nodeSrc = leaf->clsParent;
	DBU lastFlow = nodeSink->clsFlow;
	while (nodeSrc) {
		Bin * src = nodeSrc->clsBin;
		Bin * sink = nodeSink->clsBin;
		DBU flow = nodeSink->clsFlow;
		const bool isNeighbor = isHorizontalNeighbor(src, sink);

		if (isNeighbor) { // horizontal and partial moves 
			lastFlow = moveHorizontalNeighborFlow(src, sink, flow);
		} else { // vertical moves
			lastFlow = moveFullCellFlow(src, sink, flow);
		} // end if-else 

		nodeSink = nodeSrc;
		nodeSrc = nodeSrc->clsParent;
	} // end while

	return true;
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::updateOverflowedBins() {
	clsOverflowedBins.clear();
	DBU totalOverflow = 0;
	clsMaxOverfilledAreaRatio = 0.0;
	clsAvgOverfilledAreaRatio = 0.0;
	for (Bin * bin : clsBins) {
		if (bin->getSupply() > 0) {
			totalOverflow += bin->getSupply();
			clsOverflowedBins.push_back(bin);
			double ratio = bin->getSupply() / static_cast<double> (bin->getPlaceableSpace());
			clsAvgOverfilledAreaRatio += ratio;
			clsMaxOverfilledAreaRatio = std::max(clsMaxOverfilledAreaRatio, ratio);
		} // end if 
	} // end for 
	if (!clsOverflowedBins.empty()) {
		clsAvgOverfilledAreaRatio /= clsOverflowedBins.size();
	} // end if

	return totalOverflow;
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::updateMaxDisplacement() {
	DBUxy binSize = getDefaultBinLength();
	double displ = clsAlpha * binSize[X] + clsBetha * clsIteration * binSize[X];
	clsMaxDisplacement = std::max(static_cast<DBU> (displ), binSize[X]);
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::computeInitialFlow(Bin * bin) {
	DBU avg = roundedUpIntegralDivision(bin->getUsage(), (DBU) bin->getNumNodes()); // line 7
	DBU supply = bin->getSupply(); // line 7
	DBU flow = std::min(avg, supply); // line 7

	if (bin->getWidth() >= getDefaultBinLength(X)) {
		return flow;
	} // end if 

	// If the bin is surrounded by blockages in the horizontal in the row
	for (Node * inst : bin->allNodes()) {
		DBU cellDisp = inst->getDisplacement();
		if (cellDisp > clsMaxDisplacement && clsEnableMaxDisplacement)
			continue;
		if (inst->getWidth() > flow)
			continue;
		flow = inst->getWidth();
	} // end for 
	return flow;
} // end method 

// -----------------------------------------------------------------------------

TNode * NFLegal::pathAugmentationBranchBound(TNode * root, std::deque<TNode*> &paths) {
	std::set<Bin*> visitedBins; // lines 2 to 4

	// The top TNode* is the node with the lowest cost.
	// source: https://en.cppreference.com/w/cpp/container/priority_queue
	auto cmp = [](const TNode* node0, const TNode * node1) {
		return node0->clsCost > node1->clsCost;
	};
	std::priority_queue < TNode*, std::vector<TNode*>, decltype(cmp) > nodes(cmp);
	nodes.push(root);
	TNode * bestPath = nullptr;

	do {
		TNode * parent = nodes.top();
		nodes.pop();
		if (bestPath && bestPath->clsCost < parent->clsCost) {
			continue;
		} // end if 

		Bin * src = parent->clsBin;
		visitedBins.insert(src);
		std::deque<TNode> & childreen = parent->clsChildreen;

		for (Bin * sink : src->allNeighbors()) {
			if (visitedBins.find(sink) != visitedBins.end()) { // line 14 
				continue; // line 14 
			} // end if 

			DBU requiredFlow = std::max(parent->clsFlow - src->getDemand(), (DBU) 0);
			// computing the flow 
			double cost = 0;
			
			DBU outSupply = computeFlow(src, sink, requiredFlow, cost); // line 15 

			if (outSupply <= 0) { // line 16 
				continue; // line 16 
			} // end if 

			childreen.push_back(TNode());
			TNode & sinkNode = childreen.back();
			sinkNode.clsBin = sink;
			sinkNode.clsCost = cost + parent->clsCost;
			sinkNode.clsFlow = outSupply;
			sinkNode.clsParent = parent;
		} // end for 

		for (int i = 0; i < childreen.size(); ++i) {
			TNode * nd = &childreen[i];
			if (nd->clsFlow <= nd->clsBin->getDemand()) {
				paths.push_back(nd);
				if (bestPath && (bestPath->clsCost > nd->clsCost)) {
					bestPath = nd;
				} // end if 
				if (!bestPath) {
					bestPath = nd;
				} // end if 
			} else {
				nodes.push(nd);
			} // end if-else
		} // end for 

	} while (!nodes.empty());


	return bestPath;
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::computeFlow(Bin * src, Bin * sink, const DBU flow, double & cost) {
	if (flow == 0) {
		return 0;
	} // end if 

	bool isHorNeighbor = isHorizontalNeighbor(src, sink);
	DBU supplyFlow = computeNodeFlow(src, sink, flow, isHorNeighbor, cost);

	if (isHorNeighbor) {
		supplyFlow = std::min(supplyFlow, flow);
	} // end if 

	return supplyFlow;
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::computeNodeFlow(Bin * src, Bin * sink, const DBU flow, const bool isHorNeighbor, double & cost) {
	const DBU binDisp = computeBinDisplacemnet(src, sink);

	if (binDisp > clsMaxDisplacement) {
		return 0;
	} // end if 

	DBU supplyFlow = 0;
	std::vector<NodeFlow> instances;
	instances.reserve(src->getNumNodes());

	selectNodes(src, sink, isHorNeighbor, instances);

	DBU sinkFlow = 0;
	for (NodeFlow & inst : instances) {
		supplyFlow += inst.clsSrcOverlap;
		cost += inst.clsCost;

		if (!isHorNeighbor) {
			sinkFlow += inst.clsNode->getWidth() - inst.clsSinkOverlap;
		} // end if 
		if (supplyFlow >= flow) {
			return isHorNeighbor ? supplyFlow : sinkFlow;
		} // end if 
	} // end for 
	return 0;
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::selectNodes(Bin * src, Bin * sink, const bool isHorNeighbor, std::vector<NodeFlow> &instances) {
	const Bounds & sinkBounds = sink->getBounds();
	const Bounds & srcBounds = src->getBounds();
	const DBU sinkPlaceable = sink->getPlaceableSpace();

	for (Node * inst : src->allNodes()) {
		const DBU width = inst->getWidth();

		if (width > sinkPlaceable) {
			continue;
		} // end if 

		const Bounds & instBounds = inst->getBounds();
		Bounds sinkOverlapBds = sinkBounds.overlapRectangle(instBounds);
		Bounds srcOverlapBds = srcBounds.overlapRectangle(instBounds);
		if (isHorNeighbor) {
			DBU totalOverlap = sinkOverlapBds.computeLength(X) + srcOverlapBds.computeLength(X);
			if (totalOverlap < width) {
				continue;
			} // end if 
		} // end if 
		double cost = 0.0;
		DBU disp = computeDisplacement(sink, inst, cost);
		bool next = clsEnableMaxDisplacement || (!clsEnableMaxDisplacement && !isHorNeighbor);
		if (next && disp > clsMaxDisplacement) {
			continue;
		} // end if 
		instances.push_back(NodeFlow());
		NodeFlow & instFlow = instances.back();
		instFlow.clsDisplacement = disp;
		instFlow.clsNode = inst;
		instFlow.clsCost = cost;
		instFlow.clsSrcOverlap = srcOverlapBds.computeLength(X);
		instFlow.clsSinkOverlap = sinkOverlapBds.computeLength(X);
	} // end for 
	sortNodes(instances);
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::sortNodes(std::vector<NodeFlow> & instances) {
	std::sort(instances.begin(), instances.end(), [](NodeFlow & inst1, NodeFlow & inst2) {
		return inst1.clsCost < inst2.clsCost;
	}); // end sort 
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::computeBinDisplacemnet(Bin * src, Bin *sink) {
	if (isNeighbor(src, sink)) {
		return 0;
	} // end if 

	const Bounds & srcBds = src->getBounds();
	const Bounds & sinkBds = sink->getBounds();
	const DBU llx = std::max(srcBds[LOWER][X], sinkBds[LOWER][X]);
	const DBU urx = std::min(srcBds[UPPER][X], sinkBds[UPPER][X]);
	const DBU lly = std::max(srcBds[LOWER][Y], sinkBds[LOWER][Y]);
	const DBU ury = std::min(srcBds[UPPER][Y], sinkBds[UPPER][Y]);

	return std::abs((urx - llx) + (ury - lly));
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::computeDisplacement(const Bin *sink, const Node * inst, double & cost) {
	const DBUxy targetPos = computeTargetPosition(sink, inst);
	const DBUxy pos = inst->getPosition();
	const DBUxy initPos = inst->getInitialPosition();
	const DBU INVALID_DBU = std::numeric_limits<DBU>::max();
	DBUxy disp(INVALID_DBU, INVALID_DBU);
	disp = targetPos - initPos;
	disp.abs();


	//	//Cost0
	//	disp = targetPos- pos;
	//	disp.abs();
	//	cost = disp.aggregated();

	//Cost1 
	DBUxy currentDisp = pos - initPos;
	DBUxy targetDisp = targetPos - initPos;
	currentDisp.abs();
	targetDisp.abs();
	DBUxy costDisp = targetDisp - currentDisp;
	cost = costDisp.aggregated();
	return costDisp.aggregated();
	//	
	//	// Cost2
	//	dispCost = targetPos- pos;
	//	cost = std::pow(dispCost.aggregated(), 2);
	//	

	//	// Cost3 
	//	DBU costX = std::pow(pos[X] - initPos[X], 2);
	//	DBU costY = std::pow(pos[Y] - initPos[Y], 2);
	//	cost = std::sqrt(costX+costY);

	// Cost4 
	//	DBUxy currentDisp = pos - initPos;
	//	DBUxy targetDisp = targetPos - initPos;
	//	DBU costCurrent = std::pow(currentDisp.aggregated(), 2);
	//	DBU costTarget = std::pow(targetDisp.aggregated(), 2);
	//	cost = std::sqrt(costCurrent) - std::sqrt(costTarget);


	//	// Cost5 
	//	DBUxy currentDisp = pos - initPos;
	//	DBUxy targetDisp = targetPos - initPos;
	//	DBU costCurrent = std::pow(currentDisp.aggregated(), 2);
	//	DBU costTarget = std::pow(targetDisp.aggregated(), 2);
	//	cost = costCurrent - costTarget;
	//	return disp.aggregated();
} // end method 

// -----------------------------------------------------------------------------

bool NFLegal::isHorizontalNeighbor(Bin *src, Bin * sink) {
	const Bounds &srcBounds = src->getBounds();
	const Bounds &sinkBounds = sink->getBounds();
	if (srcBounds[LOWER][Y] != sinkBounds[LOWER][Y])
		return false;
	if (srcBounds[UPPER][X] == sinkBounds[LOWER][X])
		return true;
	if (sinkBounds[UPPER][X] == srcBounds[LOWER][X])
		return true;
	return false;
} // end method 

// -----------------------------------------------------------------------------

bool NFLegal::isNeighbor(Bin *src, Bin * sink) {
	const Bounds &srcBounds = src->getBounds();
	const Bounds &sinkBounds = sink->getBounds();
	if (srcBounds[LOWER][Y] == sinkBounds[UPPER][Y])
		return true;
	if (srcBounds[UPPER][Y] == sinkBounds[LOWER][Y])
		return true;
	if (srcBounds[UPPER][X] == sinkBounds[LOWER][X])
		return true;
	if (sinkBounds[UPPER][X] == srcBounds[LOWER][X])
		return true;
	return false;
} // end method 

// -----------------------------------------------------------------------------

bool NFLegal::isVertical(Bin *src, Bin * sink) {
	const Bounds &srcBounds = src->getBounds();
	const Bounds &sinkBounds = sink->getBounds();
	if (srcBounds[LOWER][Y] == sinkBounds[UPPER][Y])
		return true;
	if (srcBounds[UPPER][Y] == sinkBounds[LOWER][Y])
		return true;
	return false;
} // end method 

// -----------------------------------------------------------------------------

DBUxy NFLegal::computeTargetPosition(const Bin *sink, const Node * inst) {
	DBUxy pos(inst->getPosition(X), sink->getPosition(Y));
	const DBU length = inst->getWidth();
	const DBU upperSink = sink->getCoordinate(UPPER, X) - length;
	if (inst->getPosition(X) > upperSink || inst->getInitialPosition(X) > upperSink)
		pos[X] = upperSink;
	if (inst->getPosition(X) < sink->getPosition(X) || inst->getInitialPosition(X) < sink->getPosition(X))
		pos[X] = sink->getPosition(X);
	return pos;
} // end method 

// -----------------------------------------------------------------------------

DBUxy NFLegal::computeHorizontalPosition(const Bounds & sink, const Bounds & instance, const DBU flow) {
	DBUxy pos(instance[LOWER][X], sink[LOWER][Y]);
	const DBU length = instance.computeLength(X);
	Bounds overlap = sink.overlapRectangle(instance);
	DBU widthOverlap = overlap.computeLength(X);
	DBU candidateFlow = length - widthOverlap;

	candidateFlow = std::min(flow, candidateFlow);
	// Move cell to left
	if (instance[LOWER][X] > sink[LOWER][X]) {
		pos[X] = sink[UPPER][X] - (widthOverlap + candidateFlow);
	} else { // move Cell to right 
		pos[X] = sink[LOWER][X] - (length - widthOverlap);
		pos[X] += candidateFlow;
	} // end if-else 

	return pos;
} // end method 

// -----------------------------------------------------------------------------

//!@breif moves cells only adjacent neighbor bins

DBU NFLegal::moveHorizontalNeighborFlow(Bin * src, Bin * sink, const DBU flow) {
	std::vector<NodeFlow> nodes;
	selectNodes(src, sink, true, nodes);

	DBU movedFlow = 0;
	for (NodeFlow & node : nodes) {
		const DBU requiredMoveFlow = std::min(node.clsSrcOverlap, flow - movedFlow);

		Node * inst = node.clsNode;
		const Bounds & sinkBounds = sink->getBounds();
		const Bounds & instBounds = inst->getBounds();
		const DBUxy targetPos = computeHorizontalPosition(sinkBounds, instBounds, requiredMoveFlow);
		moveNode(inst, src->getSegment(), sink->getSegment(), targetPos);
		movedFlow += requiredMoveFlow;
		if (movedFlow >= flow) {
			break;
		} // end if 
	} // end for 
	return movedFlow;
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::moveFullCellFlow(Bin * src, Bin * sink, const DBU flow) {
	std::vector<NodeFlow> nodes;
	bool isHorNeighbor = isHorizontalNeighbor(src, sink);
	selectNodes(src, sink, isHorNeighbor, nodes);

	DBU movedFlow = 0;
	for (NodeFlow & node : nodes) {
		Node * inst = node.clsNode;
		const DBU width = inst->getWidth() - node.clsSinkOverlap;
		if (movedFlow + width > flow) {
			break;
		} // end if 
		const DBUxy targetPos = computeTargetPosition(sink, inst);
		moveNode(inst, src->getSegment(), sink->getSegment(), targetPos);
		movedFlow += width;
	} // end for 

	return movedFlow;
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::computeCandidateCells(Bin * src, Bin * sink, const DBU flow, std::vector<NodeFlow> & insts, double & cost) {
	DBU supplyFlow = 0;
	const bool isHorNeighbor = isHorizontalNeighbor(src, sink);
	selectNodes(src, sink, isHorNeighbor, insts);

	std::size_t index = 0;
	for (NodeFlow & inst : insts) {
		index++;
		cost += inst.clsCost;
		if (isHorNeighbor) {
			supplyFlow += inst.clsSrcOverlap;
		} else {
			const DBU width = inst.clsNode->getWidth() - inst.clsSinkOverlap;
			supplyFlow += width;
		} // end if-else 

		if (supplyFlow >= flow) {
			if (isHorNeighbor) {
				inst.clsSrcOverlap = inst.clsSrcOverlap - supplyFlow + flow;
			} // end if 
			break;
		} // end if 
	} // end for 


	if (!isHorNeighbor && supplyFlow > flow) {
		NodeFlow & inst = insts.back();
		const DBU width = inst.clsNode->getWidth() - inst.clsSinkOverlap;
		supplyFlow -= width;
		supplyFlow = std::max((DBU) 0, supplyFlow);
		index--;
	} // end if 

	insts.resize(index);
	insts.shrink_to_fit();

	if (isHorNeighbor) {
		return std::min(supplyFlow, flow);
	} // end if 
	return supplyFlow;
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::breakFlowStackUpBins(Bin * src) {
	Bin * bestSink = nullptr;
	Node * bestNode = nullptr;
	double bestCost = std::numeric_limits<double>::max();
	for (Bin * sink : src->allNeighbors()) {
		const bool isHorNeighbor = isHorizontalNeighbor(src, sink);
		std::vector<NodeFlow> instances;
		selectNodes(src, sink, isHorNeighbor, instances);

		if (!instances.empty()) {

			NodeFlow & front = instances.front();
			if (bestCost > front.clsCost) {
				bestSink = sink;
				bestNode = front.clsNode;
				bestCost = front.clsCost;
			} // end if 
		} // end if 
	} // end for 

	if (bestNode) {
		const DBUxy targetPos = computeTargetPosition(bestSink, bestNode);
		moveNode(bestNode, src->getSegment(), bestSink->getSegment(), targetPos);
	} // end if 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::cellRemovalOverlap() {
	CellOverlapOpto opt = clsOptions.getCellOverlapOpto();
	if (opt == CellOverlapOpto::JEZZ) {
		legalizeJezz();
	} else if (opt == CellOverlapOpto::NFLEGAL) {
		legalizeNFLegal();
	} else {
		std::cout << "Cell overlap removal method not defined. Design is kept unlegalized\n";
	} // end if-else 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::legalizeJezz() {
	Stepwatch watchLegalize("Legalizing Circuit");
	Jezz * jezz = nullptr;
	if (clsSession.isServiceRunning("rsyn.jezz")) {
		jezz = clsSession.getService("rsyn.jezz");
		jezz->stop();
	} // end if 
	clsSession.startService("rsyn.jezz");
	jezz = clsSession.getService("rsyn.jezz");
	jezz->jezz_Legalize();
	watchLegalize.finish();
	clsRuntime += watchLegalize.getElapsedTime();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::initLegalizeNFLegal() {
	for (Row & row : clsRows) {
		row.initLegalize();
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::legalizeNFLegal() {
	Stepwatch watchLegalize("Legalizing Circuit");
	initLegalizeNFLegal();

	clsDirtyNodes.clear();
	for (Node & node : clsNodes) {
		if (node.isWhitespace()) {
			continue;
		} // end if 
		clsDirtyNodes.push_back(&node);
	} // end for 

	std::sort(clsDirtyNodes.begin(), clsDirtyNodes.end(), [](Node * node0, Node * node1) {
		return node0->getPosition(X) < node1->getPosition(X);
		//return node0->getDisplacement() > node1->getDisplacement();
	});

	for (Node * node : clsDirtyNodes) {

		bool invalid = legalizeNode(node);
		if (invalid) {
			std::cout << "<<<<<<< Invalid legalization. Debug Required .... <<<<<<<<<\n";
			std::cout << "Debug Node: " << node->getName() << "\n";
			std::cout << "Exiting ... \n";
			continue;
			exit(0);
		} // end if 
	} // end for 
	watchLegalize.finish();
	clsRuntime += watchLegalize.getElapsedTime();



	//DEBUG BEGIN
	DBU disp = 0;
	Node * nd = nullptr;
	//DEBUG BEGIN
	for (Node & node : clsNodes) {
		DBU d = node.getDisplacement();
		if (d > disp) {
			disp = d;
			nd = &node;
		}
	}
	//DEBUG END
	if (nd) {
		std::cout << "Max displacement cell " << nd->getName()
			<< " disp: " << disp
			<< "\n";
	}

} // end method 

// -----------------------------------------------------------------------------

bool NFLegal::legalizeNode(Node * inst) {
	LegalAux aux;
	aux.clsNode = inst;
	computeOptimizedLegalPosition(aux);
	Segment * legalSegment = aux.clsLegalSegment;

	bool invalid = legalSegment->legalizeNode(aux);
	return invalid;
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::computeOptimizedLegalPosition(LegalAux & legalAux) {
	// todo loop to find the best position to legalize the cell 
	Node * inst = legalAux.clsNode;

	DBUxy pos = inst->getPosition();
	Row * row = getRow(pos[Y]);
	Segment * srcSegment = row->getSegment(pos);
	legalAux.clsOriginSegment = srcSegment;
	LegalAux bestLegal = legalAux;
	srcSegment->computeLegalPosition(bestLegal);


	//	// TODO REMOVE TO SEARCH NEIGHBOR SEGMENTS
	//	legalAux = bestLegal;
	//	return;
	//	//END TODO REMOVE TO SEARCH NEIGHBOR SEGMENTS

	std::set<Segment*> visited;
	visited.insert(srcSegment);
	std::deque<Segment*> segments;
	segments.push_back(srcSegment);
	for (Segment * segment : srcSegment->allNeighbors()) {
		DBUxy pos;
		pos[X] = inst->getPosition(X);
		pos[Y] = segment->getPosition(Y);
		if (!segment->isInsidePosition(pos)) {
			continue;
		} // end if 

		const DBU segmentDemand = segment->getDemand();
		if (segmentDemand < inst->getWidth()) {
			continue;
		} // end if 

		// check if neighbor bin has enough area to accept the cell
		const int low = segment->getBinIndex(pos[X], false);
		const int upp = segment->getBinIndex(pos[X] + inst->getWidth(), true);
		DBU totalDemand = 0;
		for (int id = low; id < upp; ++id) {
			Bin * bin = segment->getBinByIndex(id);
			totalDemand += bin->getDemand();
		} // end for 

		if (totalDemand < inst->getWidth()) {
			continue;
		} // end if 


		Bin * bin = segment->getBinByPosition(pos[X]);
		DBU demand = bin->getDemand();
		if (demand < inst->getWidth()) {
			continue;
		} // end if 

		LegalAux temp = legalAux;
		if (!segment->computeLegalPosition(temp)) {
			continue;
		} // end if 

		if (temp.clsCost < bestLegal.clsCost) {
			//	std::cout<<"best segment\n";
			bestLegal = temp;
		} // end if 
	} // end for 

	legalAux = bestLegal;
} // end method 

// -----------------------------------------------------------------------------

Row* NFLegal::getRow(const DBU pos, const bool nearest) {
	int index = getRowIndex(pos);
	if (index >= clsRows.size() || index < 0) {
		if (!nearest)
			return nullptr;
		if (index < 0)
			return &clsRows.front();
		return &clsRows.back();
	} // end if
	return &clsRows[index];
} // end method

// -----------------------------------------------------------------------------

int NFLegal::getRowIndex(const DBU pos) {
	const DBU delta = pos - clsRowLowerPos;
	const DBU rowHeight = clsPhDesign.getRowHeight();
	return static_cast<int> (delta / rowHeight);
} // end method 

// -----------------------------------------------------------------------------

Row * NFLegal::getRowByIndex(const int index) {
	if (index >= 0 && index < getNumRows())
		return &clsRows[index];
	return nullptr;
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::updateInitLegalPos() {
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		clsLegalInitPos[inst] = inst.getPosition();
	} //  end for 
} // end method 

// -----------------------------------------------------------------------------

DBU NFLegal::computeLegalDisplacement(Rsyn::Instance instance) {
	const DBUxy & initPos = clsLegalInitPos[instance];
	const DBUxy & pos = instance.getPosition();
	DBUxy disp = initPos - pos;
	disp.abs();
	return disp.aggregated();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::computeAverageCellDisplacement() {
	int numCells = clsPhDesign.getNumElements(Rsyn::PhysicalType::PHYSICAL_MOVABLE);
	std::vector<DBU> dispCells; // displacement from global placement solution
	std::vector<DBU> legalDispCells; // displacement only from legalizer. The initial pos is reseted after cell overlap is removed from macros.
	dispCells.reserve(numCells);
	legalDispCells.reserve(numCells);
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.isFixed() || inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = inst.asCell();
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);
		DBU disp = phCell.getDisplacement();
		dispCells.push_back(disp);
		DBU legalDisp = computeLegalDisplacement(inst);
		legalDispCells.push_back(legalDisp);
	} // end for 

	std::sort(dispCells.begin(), dispCells.end(), [](const DBU disp0, const DBU disp1) {
		return disp0 > disp1;
	}); // end sort 

	std::sort(legalDispCells.begin(), legalDispCells.end(), [](const DBU disp0, const DBU disp1) {
		return disp0 > disp1;
	}); // end sort 

	const double clsAcd2Weight = 10.0;
	const double clsAcd5Weight = 4.0;
	const double clsAcd10Weight = 2.0;
	const double clsAcd20Weight = 1.0;

	const int numAcd2 = static_cast<int> (numCells * 0.02);
	const int numAcd5 = static_cast<int> (numCells * 0.05);
	const int numAcd10 = static_cast<int> (numCells * 0.1);
	const int numAcd20 = static_cast<int> (numCells * 0.2);

	const bool empty = dispCells.empty();

	clsAcdMaxDisp = empty ? 0.0 : static_cast<double> (dispCells[0]);
	clsAcdAvgDisp = 0.0;
	clsAcd2 = 0.0;
	clsAcd5 = 0.0;
	clsAcd10 = 0.0;
	clsAcd20 = 0.0;


	for (int i = 0; i < numAcd2; ++i) {
		clsAcd2 += dispCells[i];
	} // end for 

	clsAcd5 = clsAcd2;
	for (int i = numAcd2; i < numAcd5; ++i) {
		clsAcd5 += dispCells[i];
	} // end for 

	clsAcd10 = clsAcd5;
	for (int i = numAcd5; i < numAcd10; ++i) {
		clsAcd10 += dispCells[i];
	} // end for 

	clsAcd20 = clsAcd10;
	for (int i = numAcd10; i < numAcd20; ++i) {
		clsAcd20 += dispCells[i];
	} // end for

	clsAcdAvgDisp = clsAcd20;
	for (int i = numAcd20; i < numCells; ++i) {
		clsAcdAvgDisp += dispCells[i];
	} // end for

	clsAcdAvgDisp = empty ? 0.0 : (clsAcdAvgDisp / numCells);

	clsAcd2 = empty ? 0.0 : (clsAcd2 / numAcd2);
	clsAcd5 = empty ? 0.0 : (clsAcd5 / numAcd5);
	clsAcd10 = empty ? 0.0 : (clsAcd10 / numAcd10);
	clsAcd20 = empty ? 0.0 : (clsAcd20 / numAcd20);

	double wAcd2 = clsAcd2Weight * clsAcd2;
	double wAcd5 = clsAcd5Weight * clsAcd5;
	double wAcd10 = clsAcd10Weight * clsAcd10;
	double wAcd20 = clsAcd20Weight * clsAcd20;

	clsWeightedAcd = (wAcd2 + wAcd5 + wAcd10 + wAcd20) / (10.0 + 4.0 + 2.0 + 1.0);

	// Max and Avg displacement without cell overlap removal cell displacement.
	const bool legalEmpty = legalDispCells.empty();
	clsLegalMaxDisp = legalEmpty ? 0.0 : static_cast<double> (legalDispCells[0]);
	clsLegalAvgDisp = 0.0;
	for (const DBU disp : legalDispCells) {
		clsLegalAvgDisp += static_cast<double> (disp);
	} // end for 
	clsLegalAvgDisp = legalEmpty ? 0.0 : clsLegalAvgDisp / legalDispCells.size();

} // end method 

// -----------------------------------------------------------------------------

void NFLegal::updateCircuitMetrics() {
	clsPhDesign.updateAllNetBounds();
	clsLegalHpwl = clsPhDesign.getHPWL();
	computeAverageCellDisplacement();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::computeAbu() {
	Rsyn::Json params;
	params["density"] = 1.0;
	clsSession.startService("rsyn.densityGrid", params);
	Rsyn::DensityGrid * grid = clsSession.getService("rsyn.densityGrid");
	grid->updateAbu();
	clsAbuPreCellSpreadng = grid->getAbuOverfilled();
} // end method 

// -----------------------------------------------------------------------------

double NFLegal::computeStandardDeviation() {
	double totalDisp = 0.0;
	int numNodes = 0;

	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.isFixed() || inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = inst.asCell();
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);
		totalDisp += static_cast<double> (phCell.getDisplacement());
		numNodes++;
	} // end for 
	double avg = totalDisp / numNodes;

	totalDisp = 0.0;
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.isFixed() || inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = inst.asCell();
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);
		double disp = phCell.getDisplacement() - avg;
		totalDisp += std::pow<double>(disp, 2);
	} // end for

	totalDisp /= (numNodes - 1);
	return std::sqrt(totalDisp);
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::reportFinalResults(std::ostream & out) {
	const double designUnits = double(clsPhDesign.getDatabaseUnits(Rsyn::DESIGN_DBU));
	const double rowHeight = double(clsPhDesign.getRowHeight());
	const int N = 10;
	const DBU initHpwl = clsInitialHpwl.aggregated();
	const DBU legalHpwl = clsLegalHpwl.aggregated();
	const double hpwlPercent = 100 * (legalHpwl - initHpwl) / static_cast<double> (initHpwl);

	//	out << "------------ Report Results ------------\n";
	//	out << "Legalization: NFLegal - CellOverlapRemoval " << clsOptions.getCellOverlapOptoString() << "\n";
	//	out << "Design:                    " << clsDesign.getName() << "\n";
	//	out << "HPWL(GP):                  " << initHpwl / (designUnits * 1e6) << "\n";
	//	out << "HPWL(Legal):               " << legalHpwl / (designUnits * 1e6) << "\n";
	//	out << "HPWL Imp.(%):              " << hpwlImp << "\n";
	//	out << "Global Avg. Disp. (#Rows): " << clsAcdAvgDisp / rowHeight << "\n";
	//	out << "Global Max. Disp. (#Rows): " << clsAcdMaxDisp / rowHeight << "\n";
	//	out << "Legal Avg. Disp. (#Rows):  " << clsLegalAvgDisp / rowHeight << "\n";
	//	out << "Legal Max. Disp. (#Rows):  " << clsLegalMaxDisp / rowHeight << "\n";
	//	out << "Weight ACD: (#Rows):       " << clsWeightedAcd / rowHeight << "\n";
	//	out << "Runtime(s):                " << clsRuntime << "\n";
	//	out << "Cell Spreading Iter.:      " << clsIteration << "\n";
	//	out << "------------ Report Results ------------\n";


	StreamStateSaver sss(out);

	out << std::left;
	out << "\n";
	out << "                  ";
	out << std::setw(N + 5) << "Design";
	out << std::setw(N) << "gHPWL(e6)"; // global placement HPWL
	out << std::setw(N) << "lHPWL(e6)"; // legalized placement HPWL
	out << std::setw(N) << "HPWL(%)";
	out << std::setw(N + 5) << "avgDisp"; // average cell displacement in number of rows
	out << std::setw(N + 5) << "maxDisp"; // max cell displacement in number of rows
	out << std::setw(N + 5) << "runtime(s)";
	out << std::setw(N) << "CS(s)"; // cell spreading runtime
	out << std::setw(N) << "#Its"; // number of iterations of cell spreading network flow algorithm
	out << std::setw(N) << "COR"; // Cell Overlap Removal method
	out << std::setw(N) << "avgDisp \u03C3"; // standard deviation
	//out << std::setw(N) << "ABU"; // average bin utilization
	//	out << std::setw(N) <<"#CP";
	//	out << std::setw(N) <<"#VP";
	out << "\n";

	out << "NFLegal Result:   "; // make it easy to grep
	out << std::setw(N + 5) << clsDesign.getName();
	out << std::setw(N) << initHpwl / (designUnits * 1e6);
	out << std::setw(N) << legalHpwl / (designUnits * 1e6);
	out << std::setw(N) << hpwlPercent;
	out << std::setw(N + 5) << clsAcdAvgDisp / rowHeight;
	out << std::setw(N + 5) << clsAcdMaxDisp / rowHeight;
	out << std::setw(N + 5) << clsRuntime;
	out << std::setw(N) << clsCellSpreadingRuntime;
	out << std::setw(N) << clsIteration;
	out << std::setw(N) << clsOptions.getCellOverlapOptoString();
	out << std::setw(N) << computeStandardDeviation() / rowHeight;
	//out << std::setw(N) << clsAbuPreCellSpreadng;
	//	out << std::setw(N) << clsNumCandidatePaths;
	//	out << std::setw(N) << clsNumValidPaths;
	out << "\n";


	sss.restore();

} // end method 

// -----------------------------------------------------------------------------

void NFLegal::report(std::ostream & out, const bool initial) {

	const int N = 15;
	StreamStateSaver sss(out);

	out << std::left;
	if (initial) {
		out << "\n";
		out << std::setw(N) << "Design";
		out << std::setw(N) << "Iteration";
		out << std::setw(N) << "#OF Bins";
		out << std::setw(N) << "Total OF";
		out << std::setw(N) << "Max Disp (DBU)";
		out << std::setw(N) << "Avg OFAR"; // Max Overfilled Area Ratio
		out << std::setw(N) << "Max OFAR";
	} else {
		out << std::setw(N) << clsDesign.getName();
		out << std::setw(N) << (clsIteration + 1);
		out << std::setw(N) << clsOverflowedBins.size();
		out << std::setw(N) << clsTotalOverflow;
		out << std::setw(N) << clsMaxDisplacement;
		out << std::setw(N) << clsAvgOverfilledAreaRatio;
		out << std::setw(N) << clsMaxOverfilledAreaRatio;
	} // end if-else 
	out << "\n";
	sss.restore();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::reportAcd(std::ostream & out) {
	StreamStateSaver sss(out);
	const int N = 15;
	//	const double rowHeight = double(clsPhDesign.getRowHeight());
	const double database = double(clsPhDesign.getDatabaseUnits(Rsyn::DBUType::DESIGN_DBU));
	out << std::left;
	out << "\n";
	out << "                  ";
	out << std::setw(N) << "Design";
	out << std::setw(N) << "ACD_2%";
	out << std::setw(N) << "ACD_5%";
	out << std::setw(N) << "ACD_10%";
	out << std::setw(N) << "ACD_20%";
	out << std::setw(N) << "wACD";
	out << std::setw(N) << "ACD_AVG";
	out << std::setw(N) << "ACD_Max_Disp";
	out << "\n";

	out << "NFLegal ACD(um):  "; // make it easy to grep
	out << std::setw(N) << clsDesign.getName();
	out << std::setw(N) << clsAcd2 / database;
	out << std::setw(N) << clsAcd5 / database;
	out << std::setw(N) << clsAcd10 / database;
	out << std::setw(N) << clsAcd20 / database;
	out << std::setw(N) << clsWeightedAcd / database;
	out << std::setw(N) << clsAcdAvgDisp / database;
	out << std::setw(N) << clsAcdMaxDisp / database;
	out << "\n";

	sss.restore();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::reportNFLegal() {
	// Update and report metrics
	updateCircuitMetrics();
	reportFinalResults(std::cout);
	reportDisplacementDistribution();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::reportDisplacementDistribution(std::ostream & out) {
	DBU maxDisp = -std::numeric_limits<DBU>::max();
	int numNodes = 0;
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.isFixed() || inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = inst.asCell();
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);
		maxDisp = std::max(phCell.getDisplacement(), maxDisp);
		numNodes++;
	} // end for 

	DBU rowHeight = clsPhDesign.getRowHeight();
	DBU length = rowHeight;
	int numSlots = std::ceil(maxDisp / static_cast<double> (rowHeight));
	numSlots = std::max(5, numSlots);
	//DBU length = maxDisp / numSlots;
	int nonMoved = 0;



	std::vector<int> nodes(numSlots, 0);
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.isFixed() || inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = inst.asCell();
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);
		DBU disp = phCell.getDisplacement();
		if (disp == 0) {
			nonMoved++;
		} else {
			int id = disp / length;
			nodes[id]++;
		} // end if-else 
	} // end for 



	//	for (i = 0; i < std::min(numSlots, 5); ++i) {
	//		int total = nodes[i];
	//		double percent = total / static_cast<double> (numNodes);
	//		DBU iRange = i * length / rowHeight;
	//		DBU fRange = (i + 1) * length / rowHeight;
	//		std::cout << iRange << " - " << fRange << " - " << total << " ( " << (percent * 100) << " %)\n";
	//	} // end for 
	//	DBU rangePlus = i * length / rowHeight;
	int total = 0;
	for (int i = std::min(numSlots, 5); i < numSlots; ++i) {
		total += nodes[i];
	}
	//	double percent = total / static_cast<double> (numNodes);
	//	std::cout << rangePlus << " - " << rangePlus << "+ - " << total << " ( " << (percent * 100) << " %)\n";
	//
	//	std::cout << "Total Cells : " << numNodes << "\n";

	StreamStateSaver sss(out);
	const int N = 15;
	out << std::left;
	out << "\n";
	out << "              ";
	out << std::setw(N) << "Design";
	out << std::setw(N) << "#Cells";
	out << std::setw(N) << "noDisp. (%)";
	out << std::setw(N) << "0-1 Row (%)";
	out << std::setw(N) << "1-2 Rows (%)";
	out << std::setw(N) << "2-3 Rows (%)";
	out << std::setw(N) << "3-4 Rows (%)";
	out << std::setw(N) << "4-5 Rows (%)";
	out << std::setw(N) << "5+ Rows (%)";
	out << "\n";


	//	out << "NFLegal Dist: "; // make it easy to grep
	//	out << std::setw(N) << clsDesign.getName();
	//	out << std::setw(N) << numNodes;
	//	out << std::setw(N) << nonMoved;
	//	out << std::setw(N) << nodes[0];
	//	out << std::setw(N) << nodes[1];
	//	out << std::setw(N) << nodes[2];
	//	out << std::setw(N) << nodes[3];
	//	out << std::setw(N) << nodes[4];
	//	out << std::setw(N) << total;
	//	out << "\n";


	out << "NFLegal Dist: "; // make it easy to grep
	out << std::setw(N) << clsDesign.getName();
	out << std::setw(N) << numNodes;
	out << std::setw(N) << 100 * (nonMoved / static_cast<double> (numNodes));
	out << std::setw(N) << 100 * (nodes[0] / static_cast<double> (numNodes));
	out << std::setw(N) << 100 * (nodes[1] / static_cast<double> (numNodes));
	out << std::setw(N) << 100 * (nodes[2] / static_cast<double> (numNodes));
	out << std::setw(N) << 100 * (nodes[3] / static_cast<double> (numNodes));
	out << std::setw(N) << 100 * (nodes[4] / static_cast<double> (numNodes));
	out << std::setw(N) << 100 * (total / static_cast<double> (numNodes));
	out << "\n";

	sss.restore();

} // end method 

// -----------------------------------------------------------------------------

void NFLegal::reportMovedCells(std::ostream & out) {

	std::cout << __func__ << std::endl;

	int numCells = 0;
	int movedCells = 0;
	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.isFixed() || inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = inst.asCell();
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(cell);
		DBU disp = phCell.getDisplacement();
		if (disp > 0) {
			movedCells++;
		} // end if
		numCells++;
	} // end for 


	StreamStateSaver sss(out);
	const int N = 15;
	//	const double rowHeight = double(clsPhDesign.getRowHeight());
	out << std::left;
	out << "\n";
	out << "                  ";
	out << std::setw(N) << "Design";
	out << std::setw(N) << "TotalCells";
	out << std::setw(N) << "MovedCells";
	out << std::setw(N) << "%MovedCells";
	out << "\n";

	out << "NFLegal ACD(um):  "; // make it easy to grep
	out << std::setw(N) << clsDesign.getName();
	out << std::setw(N) << numCells;
	out << std::setw(N) << movedCells;
	out << std::setw(N) << (movedCells / static_cast<double> (numCells)) * 100;
	out << "\n";

	sss.restore();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::reportCircuit(std::ostream & out) {
	int numNets = clsDesign.getNumNets();
	int numMacros = clsPhDesign.getNumElements(Rsyn::PHYSICAL_FIXED);
	int numMovable = clsPhDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE);
	int numCells = clsDesign.getNumInstances();
	int numIos = 0;

	const Bounds & core = clsPhDesign.getPhysicalModule(clsModule).getBounds();
	DBU cellArea = 0;
	DBU fixedArea = 0;

	for (Rsyn::Instance inst : clsModule.allInstances()) {
		if (inst.getType() != Rsyn::CELL) {
			continue;
		} // end if 
		Rsyn::PhysicalCell phCell = clsPhDesign.getPhysicalCell(inst.asCell());
		const Bounds & cellBds = phCell.getBounds();
		DBU area = cellBds.overlapArea(core);
		if (area == 0) {
			numIos++;
			continue;
		} // end if
		if (inst.isFixed()) {
			fixedArea += area;
		} else {
			cellArea += cellBds.computeArea();
		} // end if-else 
	} // end for

	DBU database = clsPhDesign.getDatabaseUnits(Rsyn::DESIGN_DBU);

	StreamStateSaver sss(out);
	const int N = 15;
	//	const double rowHeight = double(clsPhDesign.getRowHeight());
	out << std::left;
	out << "\n";
	out << "                  ";
	out << std::setw(N) << "Design";
	out << std::setw(N) << "#Cells";
	out << std::setw(N) << "#Macros";
	out << std::setw(N) << "#Movable";
	out << std::setw(N) << "#Nets";
	out << std::setw(N) << "#IOs";
	out << std::setw(N) << "dsgArea(um2)";
	out << std::setw(N) << "fixedArea(um2)";
	out << std::setw(N) << "cellArea(um2)";

	out << "\n";

	out << "DesignReport:     "; // make it easy to grep
	out << std::setw(N) << clsDesign.getName();
	out << std::setw(N) << numCells;
	out << std::setw(N) << (numMacros - numIos);
	out << std::setw(N) << numMovable;
	out << std::setw(N) << numNets;
	out << std::setw(N) << numIos;
	out << std::setw(N) << core.computeArea() / (database * database);
	out << std::setw(N) << fixedArea / (database * database);
	out << std::setw(N) << cellArea / (database * database);
	out << "\n";

	sss.restore();
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::reportAreaOverlap(std::ostream & out) {
	double tao = 0.0; // Total Area Overlap
	double aao = 0.0; // Average Area Overlap
	double mao = 0.0; // Maximum Area Overlap
	double tca = 0.0; // Total Cell Area
	double aca = 0.0; // Average Cell Area
	double mca = 0.0; // Maximum Cell Area

	DBU databaseUnits = clsPhDesign.getDatabaseUnits(Rsyn::DBUType::DESIGN_DBU);


	for (Bin * bin : clsBins) {
		std::vector<Node*> nodes;
		nodes.reserve(bin->getNumNodes());
		for (Node * node : bin->allNodes()) {
			nodes.push_back(node);
		} // end for 
		for (int i = 0; i < nodes.size(); ++i) {
			Node * left = nodes[i];
			const Bounds & leftBds = left->getBounds();
			for (int j = i + 1; j < nodes.size(); ++j) {
				Node * right = nodes[j];
				const Bounds & rightBds = right->getBounds();
				double overlap = static_cast<double> (leftBds.overlapArea(rightBds));
				tao += overlap;
				mao = std::max(mao, overlap);
			} // end for 
		} // end for 
	} // end for 


	for (Node & node : clsNodes) {
		const Bounds & bds = node.getBounds();
		double area = static_cast<double> (bds.computeArea());
		tca += area;
		mca = std::max(mca, area);
	} // end for 

	tao /= std::pow(databaseUnits, 2);
	mao /= std::pow(databaseUnits, 2);

	double average = tao / static_cast<double> (clsPhDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE));
	aao = static_cast<DBU> (average);

	tca /= std::pow(databaseUnits, 2);
	mca /= std::pow(databaseUnits, 2);

	average = tca / static_cast<double> (clsPhDesign.getNumElements(Rsyn::PHYSICAL_MOVABLE));
	aca = static_cast<DBU> (average);

	StreamStateSaver sss(out);
	const int N = 15;
	out << std::left;
	out << "\n";
	out << "                  ";
	out << std::setw(N) << "Design";
	out << std::setw(N) << "TAO (um2)";
	out << std::setw(N) << "AAO (um2)";
	out << std::setw(N) << "MAO (um2)";
	out << std::setw(N) << "TCA (um2)";
	out << std::setw(N) << "ACA (um2)";
	out << std::setw(N) << "MCA (um2)";
	out << "\n";

	out << "Area Overlap:     "; // make it easy to grep
	out << std::setw(N) << clsDesign.getName();
	out << std::setw(N) << tao;
	out << std::setw(N) << aao;
	out << std::setw(N) << mao;
	out << std::setw(N) << tca;
	out << std::setw(N) << aca;
	out << std::setw(N) << mca;
	out << "\n";
	out << "\n";

	sss.restore();

} // end method 

// -----------------------------------------------------------------------------

void NFLegal::checkSanity() {
	for (Row & row : clsRows) {
		row.checkSanitySegmentsConnected();
		row.checkSanitySegmentsInside();
		row.checkSanitySegmentsOverlap();
		for (const Segment & seg : row.allSegments()) {
			seg.checkSanityBinsConnected();
			seg.checkSanityBinsInside();
			seg.checkSanityBinsOverlap();
		}
	}
} // end method 

// -----------------------------------------------------------------------------

void NFLegal::checkSanitySegmentList() {
	for (Row & row : clsRows) {
		for (Segment & seg : row.allSegments()) {
			seg.checkSanitySegmentList();
		}
	}
} // end method 

// -----------------------------------------------------------------------------



} // end namespace 


