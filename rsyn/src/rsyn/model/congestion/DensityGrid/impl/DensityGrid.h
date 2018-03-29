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


} // end namespace 