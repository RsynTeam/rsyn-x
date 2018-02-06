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
 * File:   DensityGrid.h
 * Author: jucemar
 *
 * Created on 16 de Setembro de 2016, 18:35
 */

#ifndef GRIDAREADECL_H
#define GRIDAREADECL_H

namespace Rsyn {

class DensityGrid : public Proxy<DensityGridData> {
protected:
	DensityGrid(DensityGridData * data) : Proxy(data) {}
public:
	DensityGrid() : Proxy(nullptr) {}
	DensityGrid(std::nullptr_t) : Proxy(nullptr) {}
	DensityGrid &operator=(const DensityGrid &other) {
		data = other.data;
		return *this;
	} // end method
	
	bool isInitialized() const;

	// initialize abu
	void init(Rsyn::PhysicalDesign phDesign, Rsyn::Module module, double targetUtilization, 
		double unit = 9.0, bool showDetails = false, const bool keepRowBounds = false);
	void updateBinLength(const DBU binLength, bool showDetails = false, const bool keepRowBounds = false);
	void updateArea(const AreaType type);
	void clearAreaOfBins(const AreaType type);
	
	int getNumBins() const;
	int getNumCols() const;
	int getNumRows() const;
	void getIndex(const DBUxy pos, int & row, int & col) const;
	int getIndex(const int row, const int col) const;
	DBU getBinSize() const;
	double getTargetDensity() const;
	double getAbu() const;
	
	bool hasArea(const int row, const int col, const AreaType type) const;
	
	/* Returns the type area consumed from the bin for each area type */
	DBU getArea(const int row, const int col, const AreaType type) const;
	
	/* Returns the area available to place cells in the bin for each area type.
	 * MOVABLE_AREA      ->   PLACEABLE_AREA - FIXED_AREA
	 * FIXED_AREA        ->   PLACEABLE_AREA
	 * PLACEABLE_AREA    ->   binArea (Area of the bin bounds)
	 * Others            ->   -std::numeric_limits<double>::max()*/
	DBU getAvailableArea(const int row, const int col, const AreaType type) const;
	
	/* Returns the ratio of each type of area
	 * returns ratio between 0.0 and 1.0 (ratio is a real number in [0, 1])
	 * MOVABLE_AREA      ->   MOVABLE_AREA / (PLACEABLE_AREA - FIXED_AREA)
	 * FIXED_AREA        ->   FIXED_AREA / PLACEABLE_AREA
	 * PLACEABLE_AREA    ->   PLACEABLE_AREA / binArea
	 * Others            ->   -std::numeric_limits<double>::max()*/
	double getRatioUsage(const int row, const int col, AreaType type) const;
	
	DBU getMaxArea(const AreaType type) const;
	void updateMaxAreas();
	
	void removeArea(const int row, const int col, const AreaType type, const DBU area);
	void removeArea(const int row, const int col, const AreaType type, const Bounds & rect);
	void addArea(const int row, const int col, const AreaType type, const DBU area);
	void addArea(const int row, const int col, const AreaType type, const Bounds & rect);
	
	
	void updatePins();
	void updatePins(const PinType type);
	void addPin(const DBUxy pos, const PinType type);
	void clearPinsOfBins(const PinType type);
	int getNumPins(const int row, const int col, const PinType type);
	int getMaxPins(const PinType type) const;
	void updateMaxPins();
	
	void initBlockages();
	void clearBlockageOfBins();
	
	void splitGridBins(const double minBinSize = 0.0);
	
	const Bounds & getBinBound(const int row, const int col);
	const std::vector<DensityGridBin> & allBins() const;
	const DensityGridBin & getDensityGridBin(const int row, const int col) const;
	const DensityGridBin & getDensityGridBin(const int index) const;
	const DensityGridBin & getDensityGridBin(const DBUxy pos) const;
	const std::vector<DensityGridBlockage> & allDensityGridBlockages(const int row, const int col) const;
	const std::vector<Bounds> & getBlockages(const int row, const int col) const;
	
	// update area usage of the bins and the abu violation
	void updateAbu(bool showDetails = false);
	
protected:
	// Adding out of row bound region to fixed area of the bin
	void updatePlaceableArea(const bool storeRowBounds = false);
	void addArea(const AreaType type, const Bounds & bound);
	void addBlockageBound(const Bounds & bound, Rsyn::Instance inst);
}; // end class 

} // end namespace 

#endif /* GRIDAREADECL_H */

