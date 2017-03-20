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
 * File:   ABU.h
 * Author: jucemar
 *
 * Created on 27 de Agosto de 2016, 11:40
 */

#ifndef ABU_H
#define ABU_H

#include "rsyn/util/Color.h"
#include "rsyn/util/Colorize.h"
#include "rsyn/phy/PhysicalDesign.h"
namespace UPLACE {

/* density profiling related parms */
#define BIN_DIM              9.0
#define ALPHA                1.0
#define BIN_AREA_THRESHOLD   0.2
#define FREE_SPACE_THRESHOLD 0.2
	
#define ABU2_WGT 10
#define ABU5_WGT  4
#define ABU10_WGT 2
#define ABU20_WGT 1
	
#define NUM_BLOCK_BINS_X 500
#define NUM_BLOCK_BINS_Y 500	

struct density_bin {
	double lx, hx; /* low/high x coordinate */
	double ly, hy; /* low/high y coordinate */
	double area; /* bin area */
	double initial_free_space; /* bin's initial freespace area */

	double m_util; /* bin's movable cell area */
	double f_util; /* bin's fixed cell area */
	double free_space; /* bin's freespace area */
};

class ABU {
protected:
	Rsyn::PhysicalDesign clsPhDesign;
	Rsyn::Module clsModule;
	Rsyn::PhysicalDie clsPhDie;
	
	double clsTargetUtilization;
	
	double clsAbuGridUnit;
	int clsAbuNumCols;
	int clsAbuNumRows;
	int clsAbuNumBins;
	double clsAbu;
	
	std::vector<density_bin> bins;
	std::vector<double> util_array;
	std::vector<Color> clsABUColors;
public:

	ABU();
	virtual ~ABU() {}
	void initAbu(Rsyn::PhysicalDesign phDesign, Rsyn::Module module, double targetUtilization, double unit = BIN_DIM);
	void updateAbu(bool showDetails = false);
	double getAbu() const { return clsAbu; }
	double getAbuGridUnit() const { return clsAbuGridUnit;}
	double getBinArea() const { return bins[0].area; }
	int getAbuNumCols() const { return clsAbuNumCols; }
	int getAbuNumRows() const { return clsAbuNumRows; }
	int getAbuNumBins() const { return clsAbuNumBins; }
	void coloringABU();
	void coloringABUViolations();
	Color &getABUBinColor ( const unsigned row, const unsigned col) { 
		unsigned binId = row * clsAbuNumCols + col;
		return clsABUColors[binId];
	} // end method
	
	void removeAbuUtilization(const DBUxy lowerPos, const DBUxy upperPos, const bool isFixed);
	void addAbuUtilization(const DBUxy lowerPos, const DBUxy upperPos, const bool isFixed);
	
	double getBinUtil(const int binId) const {
		double m_util = bins[binId].m_util;
		double free_space = bins[binId].free_space;
		return free_space == 0 ? std::numeric_limits<double>::max() : (m_util / free_space);
	} // end method 
	
	double getBinUtil(const int col, const int row) const { 
		const int index = getABUBinIndex(col, row);
		return getBinUtil(index);
	} // end method 
	
	void getABUIndex(const DBUxy pos, int & col, int & row) const;
	inline int getABUBinIndex( const int col, const int row) const { return row * clsAbuNumCols + col; }
	double getBinMovableUtil(const int col, const int row) const;
	double getBinMovableRatioUtil(const int col, const int row) const;
	double getBinFreeSpace(const unsigned col, const unsigned row) const;
	double getBinMovableArea(const unsigned col, const unsigned row) const;
	double getBinFixedArea(const unsigned col, const unsigned row) const;
	double getBinArea(const unsigned col, const unsigned row) const;
	void getABUBinPos(const int col, const int row, DBU &x, DBU &y) const;
	DBUxy getBinLower(const int col, const int row) const;
	DBUxy getBinUpper(const int col, const int row) const;
	DBUxy getNearstNonViolationABU(DBUxy currentPos) const;
	bool hasFreeSpace(const int col, const int row) const;
	Bounds getABUBounds(const unsigned row, const unsigned col) const;
	
	void writeABU(std::ostream & out);
	
}; // end class 

} // end namespace  

#endif /* ABU_H */

