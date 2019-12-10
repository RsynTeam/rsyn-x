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
 * File:   NFLegalBase.h
 * Author: isoliveira
 *
 * Created on 12 de Março de 2018, 21:26
 */

#ifndef NFL_NFLEGAL_H
#define NFL_NFLEGAL_H

#include <vector>
#include <deque>

#include <Rsyn/Session>
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/session/Service.h"
#include "Row.h"
#include "Bin.h"
#include "Blockage.h"
#include "Segment.h"
#include "Node.h"
#include "NFLegalUtil.h"
#include "NFLegalOptions.h"

namespace NFL {

struct TNode {
	DBU clsFlow = 0;
	double clsCost = 0.0;
	Bin * clsBin = nullptr;
	TNode * clsParent = nullptr;
	std::deque<TNode> clsChildreen;

}; // end struct 

// -----------------------------------------------------------------------------

struct NodeFlow {
	Node * clsNode;
	DBU clsSrcOverlap = 0;
	DBU clsSinkOverlap = 0;
	DBU clsDisplacement = 0;
	double clsCost = 0.0;
}; // end struct 

// -----------------------------------------------------------------------------

class NFLegal : public Rsyn::Service {
protected:
	// Rsyn variables
	Rsyn::Session clsSession;
	Rsyn::PhysicalDesign clsPhDesign;
	Rsyn::PhysicalModule clsPhModule;
	Rsyn::Module clsModule;
	Rsyn::Design clsDesign;

	// Data structures 
	std::vector<Row> clsRows;
	std::vector<Bin*> clsBins;
	std::vector<Node> clsNodes;
	std::deque<Node*>clsDirtyNodes;
	std::deque<Bin*> clsOverflowedBins;
	std::map<Rsyn::Instance, int> clsMapInstancesToNodes;
	Rsyn::Attribute<Rsyn::Instance, DBUxy> clsLegalInitPos;

	// NFLegal Options
	NFLegalOptions clsOptions;

	// NFLegal variables
	std::string clsMode = "*<None>*";
	DBUxy clsBinLength;
	DBU clsRowLowerPos = std::numeric_limits<DBU>::max();
	DBU clsMaxDisplacement = 0;
	DBU clsTotalOverflow = 0;
	int clsIteration = 0;
	int clsMaxIterations = 1000;
	double clsAlpha = 0.6;
	double clsBetha = 0.05;
	double clsRuntime = 0.0;
	double clsAbuPreCellSpreadng = 0.0;
	double clsCellSpreadingRuntime = 0.0;
	bool clsEnableMaxDisplacement = false;
	bool clsInitialized = false;
	bool clsDisableCellSpreading = false;

	// HPWL
	DBUxy clsInitialHpwl;
	DBUxy clsLegalHpwl;
	
	// Area Metrics
	double clsMaxOverfilledAreaRatio = 0.0;
	double clsAvgOverfilledAreaRatio = 0.0;

	// Average Cell Displacement 
	double clsWeightedAcd = 0.0;
	double clsAcd2 = 0.0;
	double clsAcd5 = 0.0;
	double clsAcd10 = 0.0;
	double clsAcd20 = 0.0;
	double clsAcdMaxDisp = 0.0;
	double clsAcdAvgDisp = 0.0;
	double clsLegalMaxDisp = 0.0; // Max displacement without cell overlap removal displacement.
	double clsLegalAvgDisp = 0.0; // Avg displacement without cell overlap removal displacement.
	
public:
	NFLegal() = default;
	NFLegal(const NFLegal& orig) = delete;
	virtual ~NFLegal() = default;
	void start(const Rsyn::Json &params) override;
	void stop() override;

	int getNumRows() const {
		return clsRows.size();
	} // end method 

	int getNumBins() const {
		return clsBins.size();
	} // end method 

	std::vector<Row> & allRows() {
		return clsRows;
	} // end method

	const std::vector<Node> & allNodes() {
		return clsNodes;
	} // end method 

	const std::vector<Bin*> & allBins() const {
		return clsBins;
	} // end method 

	void placeCell(Rsyn::PhysicalCell phCell, const DBUxy pos);
	void moveNode(Node * node, Segment * src, Segment *sink, const DBUxy targetPos);
	DBU getMaxSupply();
	DBU getMaxDemand();

	double getWeightedAcd() const {
		return clsWeightedAcd;
	} // end method 

	double getAcd2() const {
		return clsAcd2;
	} // end method 

	double getAcd5() const {
		return clsAcd5;
	} // end method 

	double getAcd10() const {
		return clsAcd10;
	} // end method 

	double getAcd20() const {
		return clsAcd20;
	} // end method 

	DBUxy getDefaultBinLength() const {
		return clsBinLength;
	} // end method 

	DBU getDefaultBinLength(const Dimension dim) const {
		return clsBinLength[dim];
	} // end method 
	
	

protected:

	void insertNode(Segment * segment, Node * inst) {
		segment->insertNode(inst);
	} // end method 

	void removeNode(Segment * segment, Node * inst) {
		segment->removeNode(inst);
	} // end method 

	// Run legalizer
	void legalizePlace();
	// Initialize NFL
	void init();
	void computeBinWidth();
	void initRows();
	void initBlockages();
	void addBlockage(const Bounds & block);
	void connectVerticalSegments();
	void connectVerticalSegments(Segment * lowerFirst, Segment * upperFirst);
	void connectVerticalSegmentsBinsThroughBlockages();
	void connectVerticalSegmentsBinsThroughBlockages(Segment * lower, Row * upper, const Bounds & block);
	void connectVerticalBins();
	void connectVerticalBins(Bin * lowerFirst, Bin * upperFirst);
	void connectVerticalBinsThroughBlockages(Segment * lower, Segment * upper, const DBU leftX, const DBU rightX);
	void initNodes(); // Requiring high runtime to initialize instances
	void alignCellToRow(Rsyn::PhysicalCell phCell);
	bool insertNode(Node * inst);
	void removeBlockageOverlap();
	Segment * getNearestSegment(Node * inst);

	// Cell Spreading
	void cellSpreading(const int maxIteration = 1000);
	void performNetworkFlow();
	bool moveCells(TNode * sink);
	DBU updateOverflowedBins();
	void updateMaxDisplacement();
	DBU computeInitialFlow(Bin * bin);
	TNode * pathAugmentationBranchBound(TNode * root, std::deque<TNode*> &paths);
	DBU computeFlow(Bin * src, Bin * sink, const DBU flow, double & cost);
	DBU computeNodeFlow(Bin * src, Bin * sink, const DBU flow, const bool isHorNeighbor, double & cost);
	void selectNodes(Bin * src, Bin * sink, const bool isHorNeighbor, std::vector<NodeFlow> &instances);
	void sortNodes(std::vector<NodeFlow> & instances);
	DBU computeBinDisplacemnet(Bin * src, Bin *sink);
	DBU computeDisplacement(const Bin *sink, const Node * inst, double & cost);
	bool isHorizontalNeighbor(Bin *src, Bin * sink);
	bool isNeighbor(Bin *src, Bin * sink);
	bool isVertical(Bin *src, Bin * sink);
	// compute target position 
	DBUxy computeTargetPosition(const Bin *sink, const Node * inst);
	//! @brief computes the partial movement of instance in horizontal neighbor bins.
	DBUxy computeHorizontalPosition(const Bounds & sink, const Bounds & instance, const DBU flow);
	// move Cells
	//! @brief move flow of partial cells that are in neighbor horizontal bins.
	// It moves the cells only in the horizontal. 
	DBU moveHorizontalNeighborFlow(Bin * src, Bin * sink, const DBU flow);
	//! @brief move entire cell from source to sink. It may be used to move cells in vertical or horizontal.
	DBU moveFullCellFlow(Bin * src, Bin * sink, const DBU flow);
	// compute candidate cells to move
	DBU computeCandidateCells(Bin * src, Bin * sink, const DBU flow, std::vector<NodeFlow> & insts, double & cost);
	void breakFlowStackUpBins(Bin * src);


	// Legalize circuit
	void cellRemovalOverlap();
	void legalizeJezz();
	void initLegalizeNFLegal();
	void legalizeNFLegal();
	bool legalizeNode(Node * inst);
	void computeOptimizedLegalPosition(LegalAux & legalAux);

	// Util methods
	int getRowIndex(const DBU pos);
	Row * getRowByIndex(const int index);
	Row * getRow(const DBU pos, const bool nearest = false);
	void updateInitLegalPos();

	// compute cell displacement from legalization stage.
	// The initial position is reseted after cell overlap are removed from macros.
	DBU computeLegalDisplacement(Rsyn::Instance instance);

	void computeAverageCellDisplacement();
	void updateCircuitMetrics();
	void computeAbu();
	double computeStandardDeviation();
	
	void reportFinalResults(std::ostream & out = std::cout);
	void report(std::ostream & out = std::cout, const bool initial = false);
	void reportAcd(std::ostream & out = std::cout);
	void reportNFLegal();
	void reportDisplacementDistribution(std::ostream & out = std::cout);
	void reportMovedCells(std::ostream & out = std::cout);
	void reportCircuit(std::ostream & out = std::cout);
	
	void reportAreaOverlap(std::ostream & out = std::cout);

	void checkSanity();
	void checkSanitySegmentList();
}; // end class

} // end namespace

#endif /* NFL_NFLEGAL_H */

