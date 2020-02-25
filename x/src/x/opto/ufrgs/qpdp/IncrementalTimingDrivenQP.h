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
 
#ifndef QPDP_H
#define	QPDP_H

#include <map>

#include "rsyn/core/Rsyn.h"
#include "x/math/lnalg/scrm.h"
#include "x/math/lnalg/lnalg.h"
#include <Rsyn/PhysicalDesign>
#include "rsyn/model/timing/Timer.h"

namespace ICCAD15 {

class Session;
class Timer;
class Infrastructure;
class LibraryCharacterizer;

class IncrementalTimingDrivenQP : public Rsyn::Process {
private:
	Rsyn::Session session{nullptr};
	Infrastructure *infra{nullptr};
	Rsyn::Design design{nullptr};
	Rsyn::Module module;
	Rsyn::PhysicalDesign phDesign;
	Rsyn::Timer *timer{nullptr};
	Rsyn::RoutingEstimator *routingEstimator;
	const Rsyn::LibraryCharacterizer *libc{nullptr};
	
	bool debugMode{false};
	bool enableRC{false};
	
	double minimumResistance{0.01};
	    
	/* Numberic values for tunning the algorithms */
protected:
	Number alpha{0};
	Number beta;
public:
	const Number getAlpha() const { return alpha; }
	const Number getBeta() const { return beta; }
	void setAlpha( const Number alpha ) { this->alpha = alpha; }
	void setBeta( const Number beta ) { this->beta = beta; }

protected:
	int maxNumPaths{5000};
public:
	const int getMaxNumPaths() const { return maxNumPaths; }
	void setMaxNumPaths( const int maxNumPaths ) { this->maxNumPaths = maxNumPaths; }
	
private:
	/* Linear system state and behavior */
	std::set<Rsyn::Cell> movable;
	std::vector< std::pair< double, Rsyn::Net > > criticalNets;
	
	LegalizationMethod legMode{LegalizationMethod::LEG_NEAREST_WHITESPACE};
	bool roolbackOnMaxDisplacementViolation{true};

//	std::map<Rsyn::Cell, int> mapCellToIndex;
	Rsyn::Attribute<Rsyn::Instance, int> mapCellToIndex;
	Rsyn::Attribute<Rsyn::Instance, double> delayRatio;
 	
	std::vector<Rsyn::Cell> mapIndexToCell;
	std::vector<DBUxy> initialPositions;
	
	SquareCompressedRowMatrix a; // Hessian matrix
	SparseMatrixDescriptor dscp;
	
	std::vector<double> bx; // Fixed forces for x
	std::vector<double> by; // Fixed forces for y
	
	std::vector<double> fw; // Anchor cells sum of weights
	std::vector<double> fx; // Anchor cells for x
	std::vector<double> fy; // Anchor cells for y
	
	std::vector<double> px; // Current solution for x
	std::vector<double> py; // Current solution for y

//	inline double computeCellDelayRatio( const Rsyn::Cell cell ) {
//		double delay = timer->getCellMaxDelay( cell, Rsyn::LATE ) + 
//					   timer->getCellMaxWireDelay( cell, Rsyn::LATE );
//		
//		return delay / timer->getCellWorstSlack( cell, Rsyn::LATE);
//	};
	
	void refreshCellsDelayRatio();
	void buildMapping();
	void copyCellsLocationFromSessionToLinearSystem();
	void copyCellsLocationFromLinearSystemToSession();
	void copyCellsLocationFromLinearSystemToSessionOptimized();
	void neutralizeSystem();
	void neutralizeSystemWithSpreadingForces();
	void generateRetentionForces( std::vector<double>& fx, 
								  std::vector<double>& fy );
	void addAnchor( const Rsyn::Cell c, const DBUxy pos, const double w );
	void removeAnchors();
	void solveLinearSystem( bool spreadingForces = true );
	
	int getMatrixIndex(Rsyn::Cell cell, const std::map<Rsyn::Cell, int> &mapCellToIndex) const {
		const auto it = mapCellToIndex.find(cell);
		return (it != mapCellToIndex.end()) ? it->second : -1;		
	} // end method
	
	/* ---------------------------------------------------------------------- */
	
	void generateLoadAnchors();
	void generateStraighteningForces_DEPRECATED( const bool enableCriticality = false );
	void generateStraighteningForcesTopCriticalPaths();
	void generateStraighteningForces( const bool stress = false  );
	void generateStraighteningForces_ELMORE();
	void generateNetWeights();
	
	void runQuadraticPlacementStraighteningMultiplePaths(const int N);
	void runQuadraticPlacementStraighteningSinglePathAndSideCells(const int N);	
	void runQuadraticPlacement(const std::set<Rsyn::Cell> &movable, 
			const LegalizationMethod legalizationMethod,
			const bool roolbackOnMaxDisplacementViolation);
	
	void buildLinearSystem(	const bool stress );
		
	void markAsMovableAllNonFixedCells(const bool considerCriticalCellsAsFixed);
	void markAsMovableAllNonFixedCriticalCells();
	void markAsMovableAllNonFixedNonCriticalCellsDrivenByCriticalNets();
	void markAsMovableAllNonFixedCriticalCellsAndTheirSinks();
	void markAsMovableAllNonFixedCriticalCellsAndTheirNeighbors();
	void markAsMovableAllNonFixedCellsInTopCriticalPaths(const int numPaths);
	void markAsMovableAllNonFixedCellsInTopCriticalPathsAndTheirNeighbors(const int numPaths);
	
	double pathAvailableArea( std::vector<Rsyn::Timer::PathHop>& pathHops );
	
	void doNoHarm();
public:
	void initIncrementalTimingDrivenQP();
	
	void setSession(Rsyn::Session ptr);
	Rsyn::Cell getCellByIndex(const int index) const { 
		return mapIndexToCell[index]; }

	void runCriticalPathSmoothing();
	
	void runNetLoadReduction( bool withAnchors = true );
	void runNetLoadReductionIteration();
	void runNeutralizeSystemWithSpreadingForces();
	void runPathStraighteningFlow( const bool stress = false );
	void runPathStraighteningFlow_UNDER_TEST();
	void runPathStraighteningFlow_ELMORE();
	void runNetWeighteningFlow();
	
	void computeAnchorPositions(std::vector<double> &x, std::vector<double> &y);
	bool canBeSetAsMovable(Rsyn::Cell cell, const bool considerCriticalCellsAsFixed);
	double estimatePathSize( std::vector<Rsyn::Timer::PathHop>& pathHops );
    
    bool run(const Rsyn::Json& params) override;
}; // end class

} // end namespace

#endif
