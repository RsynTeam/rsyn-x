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

namespace Rsyn {

// -----------------------------------------------------------------------------

inline bool DensityGrid::isInitialized() const {
	return data;
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::init(Rsyn::PhysicalDesign phDesign, Rsyn::Module module, double targetUtilization,
	double unit, bool showDetails, const bool keepRowBounds) {
	if (data) {
		std::cout << "WARNING:  ABU was already initialized\n";
		return;
	} // end if 

	data = new DensityGridData();
	data->clsPhDesign = phDesign;
	data->clsModule = module;
	data->clsTargetDensity = targetUtilization;
	data->clsPhModule = phDesign.getPhysicalModule(module);

	const DBU length = static_cast<DBU> (unit * data->clsPhDesign.getRowHeight());

	updateBinLength(length, showDetails, keepRowBounds);
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::updateBinLength(const DBU binLength, bool showDetails, const bool keepRowBounds) {

	const Bounds & dieBounds = data->clsPhModule.getBounds();
	data->clsBinSize = binLength;
	data->clsNumCols = roundedUpIntegralDivision(dieBounds.computeLength(X), binLength);
	data->clsNumRows = roundedUpIntegralDivision(dieBounds.computeLength(Y), binLength);
	data->clsBins.clear();
	data->clsBins.resize(getNumBins());
	data->clsBins.shrink_to_fit();

	if (showDetails) {
		std::cout << "\tDie Bounds      : " << dieBounds << "\n";
		std::cout << "\tNum of Bins     : " << getNumBins() << " ( " << getNumRows() << " x " << getNumCols() << " )" << "\n";
		std::cout << "\tBin dimension   : " << getBinSize() << " x " << getBinSize() << "\n";
	} // end if 

	/* 0. initialize density map */
	DBUxy lower = data->clsPhModule.getCoordinate(LOWER);
	DBUxy upper = data->clsPhModule.getCoordinate(UPPER);
	for (int j = 0; j < getNumRows(); j++) {
		for (int k = 0; k < getNumCols(); k++) {
			unsigned binId = getIndex(j, k);
			DensityGridBin & bin = data->clsBins[binId];
			bin.clsBounds[LOWER][X] = lower[X] + k*binLength;
			bin.clsBounds[LOWER][Y] = lower[Y] + j*binLength;
			bin.clsBounds[UPPER][X] = std::min(bin.clsBounds[LOWER][X] + binLength, upper[X]);
			bin.clsBounds[UPPER][Y] = std::min(bin.clsBounds[LOWER][Y] + binLength, upper[Y]);
		} // end for 
	} // end for 

	updatePlaceableArea(keepRowBounds);
	updateArea(FIXED_AREA);

} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::updatePlaceableArea(const bool storeRowBounds) {
	if (storeRowBounds) {
		data->clsHasRowBounds = storeRowBounds;
		DBU binSize = getBinSize();
		DBU rowHeight = data->clsPhDesign.getRowHeight();
		int numRows = roundedUpIntegralDivision(binSize, rowHeight);
		for (int i = 0; i < getNumBins(); i++) {
			DensityGridBin & bin = data->clsBins[i];
			bin.clsRows.reserve(numRows);
		} // end for 
	} // end if

	const Bounds & dieBounds = data->clsPhModule.getBounds();
	for (const Rsyn::PhysicalRow phRow : data->clsPhDesign.allPhysicalRows()) {
		Bounds rowOverlap = dieBounds.overlapRectangle(phRow.getBounds());
		int lcol, rcol, brow, trow;
		getIndex(rowOverlap[LOWER], brow, lcol);
		getIndex(rowOverlap[UPPER], trow, rcol);
		lcol = std::max(lcol, 0);
		rcol = std::min(rcol, getNumCols() - 1);
		brow = std::max(brow, 0);
		trow = std::min(trow, getNumRows() - 1);
		for (int j = brow; j <= trow; j++) {
			for (int k = lcol; k <= rcol; k++) {
				unsigned binId = getIndex(j, k);
				DensityGridBin & bin = data->clsBins[binId];
				const Bounds & binBound = bin.clsBounds;
				Bounds binOverlap = rowOverlap.overlapRectangle(binBound);
				bin.addArea(PLACEABLE_AREA, binOverlap.computeArea());
				if (storeRowBounds)
					bin.clsRows.push_back(binOverlap);
			} // end for 
		} // end for 
	} // end for
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::updateArea(const AreaType type) {
	clearAreaOfBins(type);

	for (Rsyn::Instance instance : data->clsModule.allInstances()) {
		if (instance.getType() != Rsyn::CELL) {
			continue;
		} // end if 

		if (instance.isMovable() && type != MOVABLE_AREA)
			continue;

		if (instance.isFixed() && type != FIXED_AREA)
			continue;

		Rsyn::Cell cell = instance.asCell();
		const Rsyn::PhysicalCell phCell = data->clsPhDesign.getPhysicalCell(cell);

		if (type == FIXED_AREA && phCell.hasLayerBounds()) {
			Rsyn::PhysicalLibraryCell phLibCel = data->clsPhDesign.getPhysicalLibraryCell(cell);
			for (const Bounds & rect : phLibCel.allLayerObstacles()) {
				Bounds bound = rect;
				bound.translate(phCell.getPosition());
				addArea(type, bound);
			} // end for 
		} else {
			const Bounds &bounds = phCell.getBounds();
			addArea(type, bounds);
		}
	} // end for
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::clearAreaOfBins(const AreaType type) {
	// Clean-up
	for (int j = 0; j < getNumRows(); j++) {
		for (int k = 0; k < getNumCols(); k++) {
			const int index = getIndex(j, k);
			DensityGridBin & bin = data->clsBins[index];
			bin.clearArea(type);
		} // end for
	} // end for
} // end method 

// -----------------------------------------------------------------------------

inline int DensityGrid::getNumBins() const {
	return data->clsNumCols * data->clsNumRows;
} // end method 

// -----------------------------------------------------------------------------

inline int DensityGrid::getNumCols() const {
	return data->clsNumCols;
} // end method 

// -----------------------------------------------------------------------------

inline int DensityGrid::getNumRows() const {
	return data->clsNumRows;
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::getIndex(const DBUxy pos, int & row, int & col) const {
	const Bounds &dieBounds = data->clsPhModule.getBounds();
	const DBUxy lower = dieBounds[LOWER];
	col = static_cast<int> ((pos[X] - lower[X]) / getBinSize());
	row = static_cast<int> ((pos[Y] - lower[Y]) / getBinSize());
} // end method 

// -----------------------------------------------------------------------------

inline int DensityGrid::getIndex(const int row, const int col) const {
	return row * data->clsNumCols + col;
} // end method 

// -----------------------------------------------------------------------------

inline DBU DensityGrid::getBinSize() const {
	return data->clsBinSize;
} // end method 

// -----------------------------------------------------------------------------

inline double DensityGrid::getTargetDensity() const {
	return data->clsTargetDensity;
} // end method 

// -----------------------------------------------------------------------------

inline double DensityGrid::getAbu() const {
	return data->clsAbu;
} // end method 

// -----------------------------------------------------------------------------

inline bool DensityGrid::hasArea(const int row, const int col, const AreaType type) const {
	int index = getIndex(row, col);
	const DensityGridBin & bin = data->clsBins[index];
	return bin.hasArea(type);
} // end method 

// -----------------------------------------------------------------------------

inline DBU DensityGrid::getArea(const int row, const int col, const AreaType type) const {
	int index = getIndex(row, col);
	const DensityGridBin & bin = data->clsBins[index];
	return bin.getArea(type);
} // end method 

// -----------------------------------------------------------------------------

inline DBU DensityGrid::getAvailableArea(const int row, const int col, const AreaType type) const {
	const int index = getIndex(row, col);
	DensityGridBin & bin = data->clsBins[index];
	if (type == MOVABLE_AREA)
		return bin.getArea(PLACEABLE_AREA) - bin.getArea(FIXED_AREA);
	if (type == FIXED_AREA)
		return bin.getArea(PLACEABLE_AREA);
	if (type == PLACEABLE_AREA)
		return bin.getBounds().computeArea();
	return -std::numeric_limits<DBU>::max();
}

// -----------------------------------------------------------------------------

inline void DensityGrid::removeArea(const int row, const int col, const AreaType type, const DBU area) {
	const int index = getIndex(row, col);
	DensityGridBin & bin = data->clsBins[index];
	bin.addArea(type, area);
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::removeArea(const int row, const int col, const AreaType type, const Bounds & rect) {
	const int index = getIndex(row, col);
	DensityGridBin & bin = data->clsBins[index];
	const Bounds & overlap = rect.overlapRectangle(bin.getBounds());
	bin.addArea(type, overlap.computeArea());
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::addArea(const int row, const int col, const AreaType type, const DBU area) {
	const int index = getIndex(row, col);
	DensityGridBin & bin = data->clsBins[index];
	bin.addArea(type, area);
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::addArea(const int row, const int col, const AreaType type, const Bounds & rect) {
	const int index = getIndex(row, col);
	DensityGridBin & bin = data->clsBins[index];
	const Bounds & overlap = rect.overlapRectangle(bin.getBounds());
	bin.addArea(type, overlap.computeArea());
} // end method 

// -----------------------------------------------------------------------------

inline DBU DensityGrid::getMaxArea(const AreaType type) const {
	const int index = data->clsMaxAreaBin[type];
	if (index < 0)
		return -std::numeric_limits<DBU>::max();
	const DensityGridBin & bin = data->clsBins[index];
	return bin.getArea(type);
} // end method 

// -----------------------------------------------------------------------------

inline double DensityGrid::getRatioUsage(const int row, const int col, AreaType type) const {
	const int index = getIndex(row, col);
	DensityGridBin & bin = data->clsBins[index];
	if (type == MOVABLE_AREA)
		return (double) bin.getArea(type) / (double) getAvailableArea(row, col, type);
	if (type == FIXED_AREA)
		return (double) bin.getArea(type) / (double) getAvailableArea(row, col, type);
	if (type == PLACEABLE_AREA)
		return (double) bin.getArea(type) / (double) getAvailableArea(row, col, type);
	return -std::numeric_limits<double>::max();
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::updateMaxAreas() {
	for (int i = 0; i < getNumBins(); i++) {
		const DensityGridBin & bin = data->clsBins[i];
		if (bin.getArea(FIXED_AREA) > getMaxArea(FIXED_AREA))
			data->clsMaxAreaBin[FIXED_AREA] = i;
		if (bin.getArea(MOVABLE_AREA) > getMaxArea(MOVABLE_AREA))
			data->clsMaxAreaBin[MOVABLE_AREA] = i;
		if (bin.getArea(PLACEABLE_AREA) > getMaxArea(PLACEABLE_AREA))
			data->clsMaxAreaBin[PLACEABLE_AREA] = i;
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::updatePins() {
	// TODO -> use thread 
	updatePins(FIXED_PIN);
	updatePins(MOVABLE_PIN);
	updatePins(BLOCK_PIN);
	updatePins(CONNECTED_PIN);
} // end method 


// -----------------------------------------------------------------------------

inline void DensityGrid::updatePins(const PinType type) {
	clearPinsOfBins(type);
	for (Rsyn::Instance inst : data->clsModule.allInstances()) {
		if (inst.getType() != Rsyn::CELL)
			continue;
		Rsyn::PhysicalCell phCell = data->clsPhDesign.getPhysicalCell(inst.asCell());
		DBUxy pos = phCell.getPosition();

		if (type == BLOCK_PIN && !inst.isMacroBlock())
			continue;
		if (type == FIXED_PIN && !inst.isFixed())
			continue;
		if (type == MOVABLE_PIN && !inst.isMovable())
			continue;
		for (Rsyn::Pin pin : inst.allPins()) {
			if (type == CONNECTED_PIN && !pin.isConnected())
				continue;
			DBUxy disp = data->clsPhDesign.getPinDisplacement(pin);
			addPin(pos + disp, type);
		} // end for 
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::addPin(const DBUxy pos, const PinType type) {
	int row, col;
	getIndex(pos, row, col);
	const int binIndex = getIndex(row, col);
	DensityGridBin & bin = data->clsBins[binIndex];
	bin.addPin(type);
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::clearPinsOfBins(const PinType type) {
	for (int i = 0; i < getNumBins(); i++) {
		DensityGridBin & bin = data->clsBins[i];
		bin.clearPins(type);
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

inline int DensityGrid::getNumPins(const int row, const int col, const PinType type) {
	int index = getIndex(row, col);
	const DensityGridBin & bin = data->clsBins[index];
	return bin.getNumPins(type);
} // end method 

// -----------------------------------------------------------------------------

inline int DensityGrid::getMaxPins(const PinType type) const {
	const int index = data->clsMaxPinBin[type];
	if (index < 0)
		return -std::numeric_limits<int>::max();
	const DensityGridBin & bin = data->clsBins[index];
	return bin.getNumPins(type);
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::updateMaxPins() {
	for (int i = 0; i < getNumBins(); i++) {

		const DensityGridBin & bin = data->clsBins[i];
		if (bin.getNumPins(FIXED_PIN) > getMaxPins(FIXED_PIN))
			data->clsMaxPinBin[FIXED_PIN] = i;
		if (bin.getNumPins(MOVABLE_PIN) > getMaxPins(MOVABLE_PIN))
			data->clsMaxPinBin[MOVABLE_PIN] = i;
		if (bin.getNumPins(BLOCK_PIN) > getMaxPins(BLOCK_PIN))
			data->clsMaxPinBin[BLOCK_PIN] = i;
		if (bin.getNumPins(CONNECTED_PIN) > getMaxPins(CONNECTED_PIN))
			data->clsMaxPinBin[CONNECTED_PIN] = i;
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::initBlockages() {
	// TODO -> reserve space in vectors. 
	for (Rsyn::Instance inst : data->clsModule.allInstances()) {
		if (inst.getType() == Rsyn::CELL) {
			Rsyn::PhysicalCell phCell = data->clsPhDesign.getPhysicalCell(inst.asCell());
			if (!inst.isFixed())
				continue;
			Rsyn::PhysicalLibraryCell phLibCell = data->clsPhDesign.getPhysicalLibraryCell(inst.asCell());
			if (phLibCell.hasLayerObstacles()) {
				for (const Bounds & rect : phLibCell.allLayerObstacles()) {
					Bounds bounds = rect;
					bounds.translate(phCell.getPosition());
					addBlockageBound(bounds, inst);
				} // end for 
			} else {
				const Bounds & bounds = phCell.getBounds();
				addBlockageBound(bounds, inst);
			} // end if-else 
		} else {
			Rsyn::PhysicalInstance phInstance = data->clsPhDesign.getPhysicalInstance(inst);
			const Bounds & bounds = phInstance.getBounds();
			Rsyn::PhysicalModule phModule = data->clsPhDesign.getPhysicalModule(data->clsModule);
			const Bounds &overlap = bounds.overlapRectangle(phModule.getBounds());
			if (overlap.computeArea() <= 0)
				continue;
			addBlockageBound(overlap, inst);
		} // end if-else 
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::clearBlockageOfBins() {
	for (int i = 0; i < getNumBins(); i++) {
		DensityGridBin & bin = data->clsBins[i];
		bin.clsBlockages.clear();
	} // end method 
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::splitGridBins(const double minBinSize) {
	if (!data->clsHasRowBounds)
		std::cout << "WARNING: Row bounds was not initialized. Therefore, "
		<< "the placeable area will be evenly split between grid area bins.\n";

	const Bounds & dieBounds = data->clsPhModule.getBounds();
	DBU binSize = getBinSize() / 2; // dividing bin size to two. The grid is split into four.

	if (binSize < minBinSize)
		return;
	int numCols = static_cast<int> (dieBounds.computeLength(X) / binSize);
	int numRows = static_cast<int> (dieBounds.computeLength(Y) / binSize);
	std::vector<DensityGridBin> grid;
	grid.resize(numCols * numRows);
	DBUxy lower = data->clsPhModule.getCoordinate(LOWER);
	DBUxy upper = data->clsPhModule.getCoordinate(UPPER);
	DBU rowHeight = data->clsPhDesign.getRowHeight();
	int rowSize = static_cast<int> (binSize / rowHeight);

	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++) {
			int oldRow = i / 2;
			int oldCol = j / 2;
			int oldBinId = getIndex(oldRow, oldCol);
			DensityGridBin & oldBin = data->clsBins[oldBinId];

			int binId = i * numCols + j;
			DensityGridBin & bin = grid[binId];
			bin.clsBounds[LOWER][X] = lower[X] + j*binSize;
			bin.clsBounds[LOWER][Y] = lower[Y] + i*binSize;
			bin.clsBounds[UPPER][X] = std::min(bin.clsBounds[LOWER][X] + binSize, upper[X]);
			bin.clsBounds[UPPER][Y] = std::min(bin.clsBounds[LOWER][Y] + binSize, upper[Y]);
			const Bounds & binBounds = bin.getBounds();

			if (data->clsHasRowBounds) {
				bin.clsRows.reserve(rowSize);
				for (const Bounds & row : oldBin.clsRows) {
					if (row.overlap(binBounds)) {
						Bounds overlapRectangle = row.overlapRectangle(binBounds);
						bin.clsRows.push_back(overlapRectangle);
						DBU area = overlapRectangle.computeArea();
						bin.addArea(PLACEABLE_AREA, area);
					} // end if 
				} // end for 
				bin.clsRows.shrink_to_fit();
			} else {
				std::cout << "TODO " << __func__ << " at line " << __LINE__ << "\n";
			} // end if-else 
		} // end for 
	} // end for 
	data->clsNumCols = numCols;
	data->clsNumRows = numRows;
	data->clsBinSize = binSize;
	data->clsBins.swap(grid); // big-O constant
	updateArea(FIXED_AREA);
	updateArea(MOVABLE_AREA);

} // end method 

// -----------------------------------------------------------------------------

inline const Bounds & DensityGrid::getBinBound(const int row, const int col) {
	int index = getIndex(row, col);
	const DensityGridBin & bin = data->clsBins[index];
	return bin.getBounds();
} // end method 

// -----------------------------------------------------------------------------

inline const std::vector<DensityGridBin> & DensityGrid::allBins() const {
	return data->clsBins;
} // end method 

// -----------------------------------------------------------------------------

inline const DensityGridBin & DensityGrid::getDensityGridBin(const int row, const int col) const {
	int index = getIndex(row, col);
	return data->clsBins[index];
} // end method 

// -----------------------------------------------------------------------------

inline const DensityGridBin & DensityGrid::getDensityGridBin(const int index) const {
	return data->clsBins[index];
} // end method 

// -----------------------------------------------------------------------------

inline const DensityGridBin & DensityGrid::getDensityGridBin(const DBUxy pos) const {
	int row, col;
	getIndex(pos, row, col);
	const int index = getIndex(row, col);
	return data->clsBins[index];
} // end method 

// -----------------------------------------------------------------------------

inline const std::vector<DensityGridBlockage> & DensityGrid::allDensityGridBlockages(const int row, const int col) const {
	int index = getIndex(row, col);
	const DensityGridBin & bin = data->clsBins[index];
	return bin.clsBlockages;
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::updateAbu(bool showDetails) {
	updateArea(MOVABLE_AREA);


	int skipped_bin_cnt = 0;
	std::vector<double> ratioUsage;
	ratioUsage.resize(getNumBins(), 0.0);
	double areaThreshold = getBinSize() * getBinSize() * data->clsBinAreaThreshold;
	/* 2. determine the free space & utilization per bin */
	for (int j = 0; j < getNumRows(); j++) {
		for (int k = 0; k < getNumCols(); k++) {
			const DensityGridBin & bin = getDensityGridBin(j, k);
			double binArea = bin.clsBounds.computeArea();
			if (binArea > areaThreshold) {
				double freeArea = bin.getArea(PLACEABLE_AREA) - bin.getArea(FIXED_AREA);
				if (freeArea > data->clsFreeSpaceThreshold * binArea) {
					int binId = getIndex(j, k);
					ratioUsage[binId] = bin.getArea(MOVABLE_AREA) / freeArea;
				} else {
					skipped_bin_cnt++;
				}
			} // end if 
		} // end for 
	} // end for  

	std::sort(ratioUsage.begin(), ratioUsage.end());

	/* 3. obtain ABU numbers */
	double abu1 = 0.0, abu2 = 0.0, abu5 = 0.0, abu10 = 0.0, abu20 = 0.0;
	const int numBins = getNumBins();
	int clip_index = static_cast<int> (0.02 * (numBins - skipped_bin_cnt));
	for (int j = numBins - 1; j > numBins - 1 - clip_index; j--) {
		abu2 += ratioUsage[j];
	}
	abu2 = (clip_index) ? abu2 / clip_index : ratioUsage[numBins - 1];

	clip_index = static_cast<int> (0.05 * (numBins - skipped_bin_cnt));
	for (int j = numBins - 1; j > numBins - 1 - clip_index; j--) {
		abu5 += ratioUsage[j];
	}
	abu5 = (clip_index) ? abu5 / clip_index : ratioUsage[numBins - 1];

	clip_index = static_cast<int> (0.10 * (numBins - skipped_bin_cnt));
	for (int j = numBins - 1; j > numBins - 1 - clip_index; j--) {
		abu10 += ratioUsage[j];
	}
	abu10 = (clip_index) ? abu10 / clip_index : ratioUsage[numBins - 1];

	clip_index = static_cast<int> (0.20 * (numBins - skipped_bin_cnt));
	for (int j = numBins - 1; j > numBins - 1 - clip_index; j--) {
		abu20 += ratioUsage[j];
	}
	abu20 = (clip_index) ? abu20 / clip_index : ratioUsage[numBins - 1];
	//	ratioUsage.clear();

	if (showDetails) {
		std::cout << "\ttarget util     : " << getTargetDensity() << "\n";
		std::cout << "\tABU {2,5,10,20} : " << abu2 << ", " << abu5 << ", " << abu10 << ", " << abu20 << "\n";
	} // end if

	/* calculate overflow & ABU_penalty */
	abu2 = std::max(0.0, abu2 / getTargetDensity() - 1.0);
	abu5 = std::max(0.0, abu5 / getTargetDensity() - 1.0);
	abu10 = std::max(0.0, abu10 / getTargetDensity() - 1.0);
	abu20 = std::max(0.0, abu20 / getTargetDensity() - 1.0);
	data->clsAbu = (data->clsAbu2Weight * abu2 + data->clsAbu5Weight * abu5 +
		data->clsAbu10Weight * abu10 + data->clsAbu20Weight * abu20) /
		(data->clsAbu2Weight + data->clsAbu5Weight + data->clsAbu10Weight + data->clsAbu20Weight);

	if (showDetails) {
		std::cout << "\tABU penalty     : " << data->clsAbu << "\n";
		std::cout << "\tALPHA           : " << data->clsAlpha << "\n";
	} // end if
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::addArea(const AreaType type, const Bounds & bound) {
	int lcol, rcol, brow, trow;
	getIndex(bound[LOWER], brow, lcol);
	getIndex(bound[UPPER], trow, rcol);

	lcol = std::max(lcol, 0);
	rcol = std::min(rcol, getNumCols() - 1);
	brow = std::max(brow, 0);
	trow = std::min(trow, getNumRows() - 1);
	for (int j = brow; j <= trow; j++) {
		for (int k = lcol; k <= rcol; k++) {
			addArea(j, k, type, bound);
		} // end for 
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

inline void DensityGrid::addBlockageBound(const Bounds & bound, Rsyn::Instance inst) {
	int brow, lcol, trow, rcol;
	getIndex(bound[LOWER], brow, lcol);
	getIndex(bound[UPPER], trow, rcol);
	for (int i = brow; i <= trow; i++) {
		for (int j = lcol; j <= rcol; j++) {
			const int index = getIndex(i, j);
			DensityGridBin & bin = data->clsBins[index];
			int blockIndex = bin.clsBlockages.size();
			std::unordered_map<std::string, int>::iterator it = bin.clsMapBlockages.find(inst.getName());
			if (it != bin.clsMapBlockages.end()) {
				blockIndex = bin.clsMapBlockages[inst.getName()];
			} else {
				bin.clsBlockages.push_back(DensityGridBlockage());
				bin.clsMapBlockages[inst.getName()] = blockIndex;
			} // end if-else 
			DensityGridBlockage &block = bin.clsBlockages[blockIndex];
			block.clsBounds.push_back(bound.overlapRectangle(bin.clsBounds));
			block.clsInstance = inst;
		} // end for 
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

} // end namespace 