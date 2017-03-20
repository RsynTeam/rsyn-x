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
 * File:   RoutingEstimator.cpp
 * Author: Nima Karimpour Darav 
 * Email: nkarimpo@ucalgary.ca
 * Created on October 26, 2015, 4:03 PM
 */

#include "RoutingEstimator.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <string.h>
#include <algorithm>


#ifdef USE_OPENMP
#include <parallel/algorithm>
#include <omp.h>
#endif

namespace TRD {

RoutingEstimator::RoutingEstimator(Rsyn::Module module, Rsyn::PhysicalDesign phDsg) : 
	clsModule(module), clsPhDesign(phDsg), _gridSizeX(-1), _gridSizeY(-1),
	_numLayers(-1), _tileWidth(-1.0), _tileHeight(-1.0), _originX(-1.0), 
	_originY(-1.0), _avgHW(0.0), _avgVW(0.0), _minHW(0.0), _minVW(0.0),
	_totalHusage(0.0), _totalVusage(0.0), _totalHcap(0.0), _totalVcap(0.0),
	_hOverflowRatio(0.0), _vOverflowRatio(0.0), _avgHOverflow(0.0), 
	_avgVOverflow(0.0), _totalOverflow(0.0), _gr(0) {
	_hEdges.clear();
	_vEdges.clear();
	_ndrHWts.clear();
	_ndrVWts.clear();
}

RoutingEstimator::~RoutingEstimator() {
	_hEdges.clear();
	_vEdges.clear();
	if (_gr != NULL) delete _gr;
}

void RoutingEstimator::SetNDR(std::vector<std::vector<double> >& ndrWidth, std::vector<std::vector<double> >& ndrSpacing) {
	int numNDR = ndrSpacing.size();
	if (numNDR == 0) {
		cout << "No routing info is defined" << endl;
		exit(-1);
	}

	if (ndrSpacing.size() != ndrWidth.size()) {
		cout << "Error in initializing NDRs" << endl;
		exit(-1);
	}

	_ndrWidth.resize(numNDR);
	_ndrSpacing.resize(numNDR);

	for (int i = 0; i < numNDR; i++) {
		if (ndrSpacing[i].size() != ndrWidth[i].size()) {
			cout << "Error in initializing NDRs" << endl;
			exit(-1);
		}
		_ndrWidth[i] = ndrWidth[i];
		_ndrSpacing[i] = ndrSpacing[i];
	}


	_ndrHWts.clear();
	_ndrVWts.clear();
	_ndrHWts.resize(numNDR);
	_ndrVWts.resize(numNDR);

	_ndrHWts[0] = 1.0; // First weight is related to the default rule
	_ndrVWts[0] = 1.0; // First weight is related to the default rule
	_minHW = std::numeric_limits<double>::max();
	_minVW = std::numeric_limits<double>::max();
	for (int i = 1; i < numNDR; i++) {
		// Determine weights for  hori and vert directions.  Average over the different layers.
		int nHori = 0;
		int nVert = 0;
		double sumHori = 0.0, denHori = 0.0;
		double sumVert = 0.0, denVert = 0.0;
		for (int l = 0; l < _numLayers; l++) {
			if (_layerDir[l] == 0) {
				if (_hMaxCapRatio[l] > 1.0e-6) {
					++nHori;
					sumHori += (_ndrWidth[i][l] + _ndrSpacing[i][l]);
					denHori += (_ndrWidth[0][l] + _ndrSpacing[0][l]);
					_minHW = std::min(_ndrWidth[i][l] + _ndrSpacing[i][l], _minHW);
				}
			}
			if (_layerDir[l] == 1) {
				if (_vMaxCapRatio[l] > 1.0e-6) {
					++nVert;
					sumVert += (_ndrWidth[i][l] + _ndrSpacing[i][l]);
					denVert += (_ndrWidth[0][l] + _ndrSpacing[0][l]);
					_minVW = std::min((_ndrWidth[0][l] + _ndrSpacing[0][l]), _minVW);
				}
			}
		}
		_ndrHWts[i] = (nHori == 0) ? 1.0 : (sumHori / denHori);
		_ndrVWts[i] = (nVert == 0) ? 1.0 : (sumVert / denVert);
	}

}

void RoutingEstimator::SetLayerblockages(std::vector< std::vector<Bounds> >& fixed) {

	if (_numLayers != fixed.size()) {
		cout << "Error in the number of layers of blockages" << endl;
		exit(-1);
	}

	int numRows = clsPhDesign.getNumRows();
	Rsyn::PhysicalModule phModule = clsPhDesign.getPhysicalModule(clsModule);
	const Bounds & bounds = phModule.getBounds();
	double dieXmin = bounds[LOWER][X];
	double dieXmax = bounds[UPPER][X];
	double dieYmin = bounds[LOWER][Y];
	double dieYmax = bounds[UPPER][Y];
	double rowHeight = bounds.computeLength(Y) / (double) numRows;

	// Allocate proper space for the blockages.
	_blockages.clear();
	_blockages.resize(_numLayers);
	//cout << "Collecting blockages" << endl;
#ifdef USE_OPENMP
	omp_set_num_threads(8);
#endif

#ifdef USE_OPENMP
#pragma omp parallel for
#endif 
	for (int l = 0; l < _numLayers; l++) {
		_blockages[l] = std::vector<std::vector<std::pair<double, double> > >();
		_blockages[l].clear();
		_blockages[l].resize(numRows);
		for (int r = 0; r < numRows; r++) {
			_blockages[l][r] = std::vector<std::pair<double, double> >();
		}


		// Identify the blockages intersecting with rows
		for (int i = 0; i < fixed[l].size(); i++) {
			Bounds& rect = fixed[l][i];

			//cout << "Identify intersecting blockages with rows" << "Layer: " << l << " fixed: " << i << "out of " << fixed[l].size() << endl;

			double xmin = std::max(dieXmin, (double) rect[LOWER][X]);
			double xmax = std::min(dieXmax, (double) rect[UPPER][X]);
			double ymin = std::max(dieYmin, (double) rect[LOWER][Y]);
			double ymax = std::min(dieYmax, (double) rect[UPPER][Y]);

			for (int r = 0; r < numRows; r++) {
				double lb = dieYmin + r * rowHeight;
				double ub = lb + rowHeight;
				std::vector<std::pair<double, double> >& currentRow = _blockages[l][r];
				if (!(ymax - 1.0e-3 <= lb || ymin + 1.0e-3 >= ub)) {
					currentRow.push_back(std::pair<double, double>(xmin, xmax));
				}
			}
		}


		// Check overlap and merge intervals
		//cout << "Check overlap and merge intervals" << endl;

		for (int r = 0; r < numRows; r++) {
			std::vector<std::pair<double, double> >& currentRow = _blockages[l][r];

			if (currentRow.size() == 0) {
				continue;
			}

			std::sort(currentRow.begin(), currentRow.end(), compare_blockages());

			std::stack< std::pair<double, double> > filo;

			filo.push(currentRow[0]);
			for (int i = 1; i < currentRow.size(); i++) {
				std::pair<double, double> top = filo.top(); // copy.
				if (top.second < currentRow[i].first) {
					filo.push(currentRow[i]); // new interval.
				} else {
					if (top.second < currentRow[i].second) {
						top.second = currentRow[i].second; // extend interval.
					}
					filo.pop(); // remove old.
					filo.push(top); // expanded interval.
				}
			}

			currentRow.clear();
			while (!filo.empty()) {
				std::pair<double, double> temp = filo.top(); // copy.
				currentRow.push_back(temp);
				filo.pop();
			}

			//cout << "Sort Intervals" << endl;
			// Sort the blockages from left-to-right.
			std::sort(currentRow.begin(), currentRow.end(), compare_blockages());
		}
	}
}

void RoutingEstimator::Initialize(const bool print) {

	if (_numLayers < 1 || _gridSizeX < 1 || _gridSizeY < 1 ||
		_ndrWidth.size() < 1 || _ndrSpacing.size() < 1) {
		cout << "Error: routing parameters are not initialized" << endl;
		exit(-1);
	}
	_hEdges.clear();
	_hEdges.resize(_numLayers);
#ifdef USE_OPENMP
#pragma omp parallel for
#endif 
	for (int l = 0; l < _numLayers; l++) {
		_hEdges[l].clear();
		_hEdges[l].resize(_gridSizeX - 1);
		for (int i = 0; i < _gridSizeX - 1; i++) {
			_hEdges[l][i].clear();
			_hEdges[l][i].resize(_gridSizeY);
			for (int j = 0; j < _gridSizeY; j++) {
				_hEdges[l][i][j]._cap = 0.0;
				_hEdges[l][i][j]._usage = 0.0;
				_hEdges[l][i][j]._lastUsage = 0.0;
			}
		}
	}

	_vEdges.clear();
	_vEdges.resize(_numLayers);
#ifdef USE_OPENMP
#pragma omp parallel for
#endif 
	for (int l = 0; l < _numLayers; l++) {
		_vEdges[l].clear();
		_vEdges[l].resize(_gridSizeX);

		for (int i = 0; i < _gridSizeX; i++) {
			_vEdges[l][i].clear();
			_vEdges[l][i].resize(_gridSizeY - 1);
			for (int j = 0; j < _gridSizeY - 1; j++) {
				_vEdges[l][i][j]._cap = 0.0;
				_vEdges[l][i][j]._usage = 0.0;
				_vEdges[l][i][j]._lastUsage = 0.0;
			}
		}
	}

	// Initialize NCTUgr data structures
	ResetNetDB();

	_netdb.gridX = _gridSizeX;
	_netdb.gridY = _gridSizeY;
	_netdb.layer = _numLayers;

	_netdb.horCap.clear();
	_netdb.verCap.clear();

	_netdb.horCap.resize(_numLayers);
	_netdb.verCap.resize(_numLayers);

	for (int l = 0; l < _numLayers; l++) {
		_netdb.verCap[l] = static_cast<int>((_vMaxCapRatio[l] * _tileWidth) / (_ndrWidth[0][l] + _ndrSpacing[0][l]));
		_netdb.horCap[l] = static_cast<int>((_hMaxCapRatio[l] * _tileHeight) / (_ndrWidth[0][l] + _ndrSpacing[0][l]));
		if (print)
			cout << "Layer: " << l << " V: " << _netdb.verCap[l] << " H: " << _netdb.horCap[l] << "\n";
	}

	// Now adjust Capacities based on routing blockages
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Edge capacities...
	// I am not sure that dieXmin and dieYmin are always equal to _xOrigin and _yOrigin;so I just take care of them

	Rsyn::PhysicalModule phModule = clsPhDesign.getPhysicalModule(clsModule);
	const Bounds & bounds = phModule.getBounds();
	int numRows = clsPhDesign.getNumRows();
	double dieXmin = bounds[LOWER][X];
	double dieXmax = bounds[UPPER][X];
	double dieYmin = bounds[LOWER][Y];
	double dieYmax = bounds[UPPER][Y];
	double rowHeight = bounds.computeLength(Y) / (double) numRows;

	_netdb.adjArray.clear();
	for (int l = 0; l < _blockages.size(); l++) {
		int dir = _layerDir[l];
		if ((dir == 0 && _netdb.horCap[l] < 1) || (dir == 1 && _netdb.verCap[l] < 1)) {
			continue;
		}
		for (int r = 0; r < numRows; r++) {
			int numBlockages = _blockages[l][r].size();
			for (int b = 0; b < numBlockages; b++) {
				std::pair<double, double> & rect = _blockages[l][r][b];
				int lx = std::min(std::max(static_cast<int> (floor((rect.first - _originX - 1e-5) / _tileWidth)), 0), (_gridSizeX - 1));
				int rx = std::min(std::max(static_cast<int> (floor((rect.second - _originX + 1e-5) / _tileWidth)), 0), (_gridSizeX - 1));
				int by = std::min(std::max(static_cast<int> (floor((r * rowHeight + dieYmin - _originY - 1e-5) / _tileHeight)), 0), (_gridSizeY - 1));
				int ty = std::min(std::max(static_cast<int> (floor(((r + 1) * rowHeight + dieYmin - _originY + 1e-5) / _tileHeight)), 0), (_gridSizeY - 1));
				if (dir == 0) // Horizontal tracks
				{
					for (int x = lx; x < rx; x++) {
						for (int y = by; y <= ty; y++) {
							double _hBlocked = std::max(std::min((r + 1) * rowHeight + dieYmin, (y + 1) * _tileHeight + _originY) -
								std::max(r * rowHeight + dieYmin, y * _tileHeight + _originY), 0.0);
							double _wBlocked = std::max(std::min(rect.second, (x + 1) * _tileWidth + _originX) -
								std::max(rect.first, x * _tileWidth + _originX), 0.0);

							if (_wBlocked == 0) {
								continue;
							}
							int numTracks = (int) (std::max(_tileHeight - _hBlocked, 0.0) / (_ndrWidth[0][l] + _ndrSpacing[0][l]));
							AdjustCapEdge e;
							e.x = x;
							e.y = y;
							e.z = l;
							e.hori = true;
							e.value = numTracks;
							_netdb.adjArray.push_back(e);

							if (numTracks > _netdb.horCap[l]) {
								cout << "numTracks: " << numTracks << " Hcap: " << _netdb.horCap[l] << "\n";
							}

							//printf( "V: %d %d %d : %d %d\n", x, y, layer, num_tracks, adjusted_capacity );
						}
					}
				} else {
					for (int y = by; y < ty; y++) {
						for (int x = lx; x <= rx; x++) {
							double _hBlocked = std::max(std::min((r + 1) * rowHeight + dieYmin, (y + 1) * _tileHeight + _originY) -
								std::max(r * rowHeight + dieYmin, y * _tileHeight + _originY), 0.0);
							double _wBlocked = std::max(std::min(rect.second, (x + 1) * _tileWidth + _originX) -
								std::max(rect.first, x * _tileWidth + _originX), 0.0);

							if (_hBlocked == 0) {
								continue;
							}
							int numTracks = (int) (std::max(_tileWidth - _wBlocked, 0.0) / (_ndrWidth[0][l] + _ndrSpacing[0][l]));
							AdjustCapEdge e;
							e.x = x;
							e.y = y;
							e.z = l;
							e.hori = false;
							e.value = numTracks;
							_netdb.adjArray.push_back(e);

							if (numTracks > _netdb.verCap[l]) {
								cout << "numTracks: " << numTracks << " Vcap: " << _netdb.verCap[l] << "\n";
							}

							//                        printf( "V: %d %d %d : %d %d\n", x, y, layer, num_tracks, adjusted_capacity );
						}
					}
				}
			}
		}
	}

}

double RoutingEstimator::PrintOverflow(bool print) {
	if (print == false) return _totalOverflow;

	cout << "OF: " << _totalOverflow << " "
		<< "%H_OF: " << _hOverflowRatio << " "
		<< "avgH_OF: " << _avgHOverflow << " "
		<< "HD: " << _totalHusage / _totalHcap << " "
		<< "%V_OF: " << _vOverflowRatio << " "
		<< "avgV_OF: " << _avgVOverflow << " "
		<< "VD: " << _totalVusage / _totalVcap << " "
		<< "TD: " << (_totalVusage + _totalHusage) / (_totalHcap + _totalVcap) << " "
		<< endl;

	return _totalOverflow;
}

void RoutingEstimator::Create() {

	//Clear the routing result
	clearRoutingResult(_netdb);

	int numNets = clsModule.getDesign().getNumNets();
	// XXX: BUG: what about nets with numPins <= 1
	_netdb.netArray.clear();
	_netdb.netArray.reserve(numNets);


	// Here I should use OpenMP to loop over nets. Right now I just use a sequential version
	//
	int netCount = 0;
#ifdef USE_OPENMP
	omp_set_num_threads(8);
#endif
	for (Rsyn::Net edge : clsModule.allNets()) {
		int numPins = edge.getNumPins();


		if (numPins <= 1) {
			continue;
		}

		_netdb.netArray.push_back(Net());

		Net& net = _netdb.netArray[netCount];
		//net.id = edge->getId(); // Guilherme Flach: This is a private information...
		net.id = netCount;
		netCount++;

		net.pin3D.clear();
		net.pin3D.reserve(numPins);

		int pp = 0;
		Point pinPoint;

		for (Rsyn::Pin pin : edge.allPins()) {
			//const Rsyn::PhysicalPin&  phyPin  = _engine->getPhysicalPin(pin);
			//const Rsyn::PhysicalCell& phyCell = _engine->getPhysicalCell(pin);
			//DBUxy pinPos = phyCell.lower() + phyPin.displacement;
			DBUxy pinPos = clsPhDesign.getPinPosition(pin);

			pinPoint.x = std::max(std::min((int) ((pinPos.x - _originX) / _tileWidth), _gridSizeX - 1), 0);
			pinPoint.y = std::max(std::min((int) ((pinPos.y - _originY) / _tileHeight), _gridSizeY - 1), 0);
			pinPoint.z = 0; // XXX:, I just set the pin layer to zero because the code does not support the pin layer
			bool skip = false;

			/*
				    for(int k = 0; k < net.pin3D.size() && !skip; k++)
				    {
					// XXX: Should I ignore the layer of the pin?
					if(pinPoint.x == net.pin3D[k].x && pinPoint.y == net.pin3D[k].y)
					{
					    skip = true;
					}
				    }
			 */
			if (skip == false) {
				net.pin3D.push_back(pinPoint);

				/*
				{
				    cout <<  "---------------------------------------------------" << endl;
				    cout << "Pin: " << endl; 
				    cout << pinPoint.x << endl;
				    cout << pinPoint.y << endl;
				    cout << pinPoint.z << endl;
				}
				 */
				pp += 1;
			}
		}

	}
	_netdb.netArray.resize(_netdb.netArray.size());
	
	//setDefaultParameter(_routerParam, false);
	settingAndChecking(_netdb, _routerParam, false);
	if (_gr != NULL) delete _gr;
	_gr = new GlobalRouter(_netdb.gridX, _netdb.gridY, _netdb.layer, _netdb.horCap, _netdb.verCap);
}

int RoutingEstimator::Route() {

	clearRoutingResult(_netdb);
	main_congestion_estimator(_netdb, _routerParam, *_gr, NULL);


	// NDR rules should be considered for the horizontal passing nets.
	for (int x = 0; x < _gridSizeX - 1; x++) {
		for (int y = 0; y < _gridSizeY; y++) {
			/*                
				    double wtSum = 0.0;
				    std::vector<Net> nctu_nets;
				    std::vector<int> passingNetIds = getPassingNet(*m_gr, nctu_nets, x, y, true);
				    for (int e = 0; e < passingNetIds.size(); e++)
				    {
					Edge& edi = m_network->m_edges[ passingNetIds[e] ];
					wtSum += (_NDRwtsH[edi.m_ndr] - 1.0);
				    }

				    getHEdge(x,y).setUsage( m_gr->g.getCost(x, y, true) + wtSum );
			 */
			getHEdge(0, x, y)._usage = _gr->g.getCost(x, y, true);
			getHEdge(0, x, y)._cap = _gr->g.getHCap();
			//getHEdge(0,x,y)._usage = 1;
			//getHEdge(0,x,y)._cap = 1;
			//cout << "HUsage: " << getHEdge(0,x,y)._usage << endl;
		}
	}

	// NDR rules should be considered for the vertical passing nets.
	for (int x = 0; x < _gridSizeX; x++) {
		for (int y = 0; y < _gridSizeY - 1; y++) {
			/*                
				    double wtSum = 0.0;
				    std::vector<Net> nctu_nets;
				    std::vector<int> passingNetIds = getPassingNet(*m_gr, nctu_nets, x, y, false);
				    for (int e = 0; e < passingNetIds.size(); e++)
				    {
					Edge& edi = m_network->m_edges[ passingNetIds[e] ];
					wtSum += (_NDRwtsV[edi.m_ndr] - 1.0);
				    }
				    getVEdge(x,y).setUsage( m_gr->g.getCost(x, y, false) + wtSum );
			 */
			getVEdge(0, x, y)._usage = _gr->g.getCost(x, y, false);
			getVEdge(0, x, y)._cap = _gr->g.getVCap();
			//getVEdge(0,x,y)._usage = 1;
			//getVEdge(0,x,y)._cap = 1;
			//cout << "VUsage: " << getVEdge(0,x,y)._usage << endl;
		}

	}

	computeStatistics();
	//PrintOverflow(); 
	return 0;
}

void RoutingEstimator::computeStatistics() {
	double totHOverflow = 0.0;
	double totVOverflow = 0.0;
#ifdef USE_OPENMP 
#pragma omp parallel sections
#endif    
	{ // start of parallel sections              
#ifdef USE_OPENMP 
#pragma omp section
#endif    
		{ // start of section 0
			_totalHusage = 0.0;
			_totalHcap = 0.0;
			_hOverflowRatio = 0.0;
			_avgHOverflow = 0.0;

			double h_usage, h_cap;
			int num_hOF = 0;
			for (int i = 0; i < _gridSizeX - 1; i++) {
				for (int j = 0; j < _gridSizeY; j++) {
					h_cap = GetHEdgeCapacity(i, j);
					h_usage = GetHEdgeUsage(i, j);
					_totalHcap += h_cap;
					_totalHusage += h_usage;
					if (h_usage > h_cap) {
						num_hOF++;
						_avgHOverflow += (h_cap > 1e-3) ? h_usage / h_cap : 1.0;
						totHOverflow += (h_usage - h_cap);
					}
				}
			}
			_hOverflowRatio = (double) (num_hOF) / (double) ((_gridSizeX - 1) * _gridSizeY);
			if (num_hOF > 0) {
				_avgHOverflow /= (double) (num_hOF);
			}
		}

#ifdef USE_OPENMP 
#pragma omp section
#endif    
		{// start of section 1 
			_totalVusage = 0.0;
			_totalVcap = 0.0;
			_vOverflowRatio = 0.0;
			_avgVOverflow = 0.0;

			double v_usage, v_cap;
			int num_vOF = 0;
			for (int i = 0; i < _gridSizeX; i++) {
				for (int j = 0; j < _gridSizeY - 1; j++) {
					v_cap = GetVEdgeCapacity(i, j);
					v_usage = GetVEdgeUsage(i, j);
					_totalVcap += v_cap;
					_totalVusage += v_usage;
					if (v_usage > v_cap) {
						num_vOF++;
						_avgVOverflow += (v_cap > 1e-3) ? v_usage / v_cap : 1.0;
						totVOverflow += (v_usage - v_cap);
					}
				}
			}
			_vOverflowRatio = (double) (num_vOF) / (double) (_gridSizeX * (_gridSizeY - 1));
			if (num_vOF > 0) {
				_avgVOverflow /= (double) (num_vOF);
			}
		}
	}

	_totalOverflow = totVOverflow + totHOverflow;
}

// -----------------------------------------------------------------------------

double RoutingEstimator::getMaxGCellCongestion() {
	double maxCong = 1.0;

	for (int i = 0; i < _gridSizeX; i++)
		for (int j = 0; j < _gridSizeY; j++)
			maxCong = std::max(maxCong, GetTileCong(i, j));

	return maxCong;
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::getMaxVerticalGCellCongestion() {
	double maxCong = 1.0;

	for (int i = 0; i < _gridSizeX; i++)
		for (int j = 0; j < _gridSizeY; j++)
			maxCong = std::max(maxCong, GetTileVCong(i, j));

	return maxCong;
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::getMaxHorizontalGCellCongestion() {
	double maxCong = 1.0;

	for (int i = 0; i < _gridSizeX; i++)
		for (int j = 0; j < _gridSizeY; j++)
			maxCong = std::max(maxCong, GetTileHCong(i, j));

	return maxCong;
} // end method


// -----------------------------------------------------------------------------

double RoutingEstimator::getMaxGCellUseRatio() {
	double maxUse = 0;
	for (int i = 0; i < _gridSizeX - 1; i++)
		for (int j = 0; j < _gridSizeY - 1; j++)
			maxUse = std::max(maxUse, GetUseRatio(i, j));

	return maxUse;
} // end method 

// -----------------------------------------------------------------------------

void RoutingEstimator::SetNctuParams(int PtRounds, int MnRounds, int RarRounds, int laType, int printOut) {
	ResetRouterParm();
	_routerParam.PtRounds = PtRounds;
	_routerParam.MnRounds = MnRounds;
	_routerParam.RarRounds = RarRounds;
	_routerParam.laType = laType;
	_routerParam.print_to_screen = printOut;
}

void RoutingEstimator::SetPrintToScreen(const bool print) {
	_routerParam.print_to_screen = print;
}

double RoutingEstimator::GetTileCong(int l, int x, int y) {
	double gcu_h = 0.0;
	double gcu_v = 0.0;
	double gcc_h = 0.0;
	double gcc_v = 0.0;


	gcc_h += (x == _gridSizeX - 1) ? 0 : this->GetHEdgeCapacity(l, x, y);
	gcc_h += (x == 0) ? 0 : this->GetHEdgeCapacity(l, x - 1, y);

	gcc_v += (y == _gridSizeY - 1) ? 0 : this->GetVEdgeCapacity(l, x, y);
	gcc_v += (y == 0) ? 0 : this->GetVEdgeCapacity(l, x, y - 1);

	gcu_h += (x == _gridSizeX - 1) ? 0 : std::max(this->GetHEdgeUsage(l, x, y), this->GetHEdgeCapacity(l, x, y));
	gcu_h += (x == 0) ? 0 : std::max(this->GetHEdgeUsage(l, x - 1, y), this->GetHEdgeCapacity(l, x - 1, y));

	gcu_v += (y == _gridSizeY - 1) ? 0 : std::max(this->GetVEdgeUsage(l, x, y), this->GetVEdgeCapacity(l, x, y));
	gcu_v += (y == 0) ? 0 : std::max(this->GetVEdgeUsage(l, x, y - 1), this->GetVEdgeCapacity(l, x, y - 1));

	double congestion = (gcc_h + gcc_v < 0.01) ? 1.0 : ((gcu_h + gcu_v) / (gcc_h + gcc_v));

	return congestion;
}

double RoutingEstimator::GetTileHCong(int l, int i, int j) {
	double TGCU_H = 0.0;
	double TGCC_H = 0.0;

	double cap, usage;

	if (i < _gridSizeX - 1) {
		cap = GetHEdgeCapacity(l, i, j);
		usage = GetHEdgeUsage(l, i, j);
		TGCC_H += cap;
		TGCU_H += std::max(usage, cap);
	}

	if (i != 0) {
		cap = GetHEdgeCapacity(l, i - 1, j);
		usage = GetHEdgeUsage(l, i - 1, j);
		TGCC_H += cap;
		TGCU_H += std::max(usage, cap);
	}

	return (TGCC_H > 1e-3) ? TGCU_H / TGCC_H : 1.0;
}

double RoutingEstimator::GetTileVCong(int l, int i, int j) {
	double TGCU_V = 0.0;
	double TGCC_V = 0.0;

	double cap, usage;

	if (j < _gridSizeY - 1) {
		cap = GetVEdgeCapacity(l, i, j);
		usage = GetVEdgeUsage(l, i, j);
		TGCC_V += cap;
		TGCU_V += std::max(usage, cap);
	}

	if (j != 0) {
		cap = GetVEdgeCapacity(l, i, j - 1);
		usage = GetVEdgeUsage(l, i, j - 1);
		TGCC_V += cap;
		TGCU_V += std::max(usage, cap);
	}

	return (TGCC_V > 1e-3) ? TGCU_V / TGCC_V : 1.0;
}

// -----------------------------------------------------------------------------

double RoutingEstimator::GetUseRatio(int i, int j) {

	double vcap = 0;
	double vuse = 0;
	double hcap = 0;
	double huse = 0;

	vcap += GetVEdgeCapacity(i, j);
	hcap += GetHEdgeCapacity(i, j);

	if (j < _gridSizeY - 1)
		vuse = GetVEdgeUsage(i, j);

	if (j != 0)
		vuse = std::max(vuse, GetVEdgeUsage(i, j - 1));

	if (i < _gridSizeX - 1)
		huse = GetHEdgeUsage(i, j);

	if (i != 0)
		huse = std::max(huse, GetHEdgeUsage(i - 1, j));

	return (vuse + huse) / (vcap + hcap);
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetVUseRatio(int i, int j) {
	double cap = 0;
	double use = 0;

	cap += GetVEdgeCapacity(i, j);

	if (j < _gridSizeY - 1)
		use = GetVEdgeUsage(i, j);

	if (j != 0)
		use = std::max(use, GetVEdgeUsage(i, j - 1));
	return use / cap;
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetHUseRatio(int i, int j) {
	double cap = 0;
	double use = 0;

	cap += GetHEdgeCapacity(i, j);

	if (i < _gridSizeX - 1)
		use = GetHEdgeUsage(i, j);
	if (i != 0)
		use = std::max(use, GetHEdgeUsage(i - 1, j));
	return use / cap;
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetUseRatio(int l, int i, int j) {

	double vcap = 0;
	double vuse = 0;
	double hcap = 0;
	double huse = 0;

	vcap += GetVEdgeCapacity(l, i, j);
	hcap += GetHEdgeCapacity(l, i, j);

	if (j < _gridSizeY - 1)
		vuse = GetVEdgeUsage(l, i, j);

	if (j != 0)
		vuse = std::max(vuse, GetVEdgeUsage(l, i, j - 1));

	if (i < _gridSizeX - 1)
		huse = GetHEdgeUsage(l, i, j);

	if (i != 0)
		huse = std::max(huse, GetHEdgeUsage(l, i - 1, j));

	return (vuse + huse) / (vcap + hcap);
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetVUseRatio(int l, int i, int j) {
	double cap = 0;
	double use = 0;

	cap += GetVEdgeCapacity(l, i, j);

	if (j < _gridSizeY - 1)
		use = GetVEdgeUsage(l, i, j);

	if (j != 0)
		use = std::max(use, GetVEdgeUsage(l, i, j - 1));

	return use / cap;
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetHUseRatio(int l, int i, int j) {
	double cap = 0;
	double use = 0;

	cap += GetHEdgeCapacity(l, i, j);

	if (i < _gridSizeX - 1)
		use = GetHEdgeUsage(l, i, j);

	if (i != 0)
		use = std::max(use, GetHEdgeUsage(l, i - 1, j));

	return use / cap;

} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetUseRatio(double x, double y) {
	int i, j;
	getGCellIndex(x, y, i, j);
	return GetUseRatio(i, j);
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetVUseRatio(double x, double y) {
	int i, j;
	getGCellIndex(x, y, i, j);
	return GetVUseRatio(i, j);
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetHUseRatio(double x, double y) {
	int i, j;
	getGCellIndex(x, y, i, j);
	return GetHUseRatio(i, j);
} // end method 

// -----------------------------------------------------------------------------

double RoutingEstimator::GetPointCong(double x, double y) {
	int i = std::max(std::min((int) ((x - _originX) / _tileWidth), _gridSizeX - 1), 0);
	int j = std::max(std::min((int) ((y - _originY) / _tileHeight), _gridSizeY - 1), 0);
	return GetTileCong(i, j);
}

double RoutingEstimator::GetPointHCong(double x, double y) {
	int i = std::max(std::min((int) ((x - _originX) / _tileWidth), _gridSizeX - 1), 0);
	int j = std::max(std::min((int) ((y - _originY) / _tileHeight), _gridSizeY - 1), 0);
	return GetTileHCong(i, j);
}

double RoutingEstimator::GetHEdgeUsage(double x, double y) {
	int i = std::max(std::min((int) ((x - _originX) / _tileWidth), _gridSizeX - 1), 0);
	int j = std::max(std::min((int) ((y - _originY) / _tileHeight), _gridSizeY - 1), 0);
	return GetHEdgeUsage(i, j);
}

double RoutingEstimator::GetPointVCong(double x, double y) {
	int i = std::max(std::min((int) ((x - _originX) / _tileWidth), _gridSizeX - 1), 0);
	int j = std::max(std::min((int) ((y - _originY) / _tileHeight), _gridSizeY - 1), 0);
	return GetTileVCong(i, j);
}

// -----------------------------------------------------------------------------

void RoutingEstimator::getGCellIndex(const double posX, const double posY, int& i, int& j) {
	i = (int) ((posX - _originX) / _tileWidth);
	j = (int) ((posY - _originY) / _tileHeight);
} // end method 

// -----------------------------------------------------------------------------

void RoutingEstimator::getGCellPos(const int i, const int j, double& x, double& y) {
	x = (i * _tileWidth) + _originX;
	y = (j * _tileHeight) + _originY;
} // end method 

// -----------------------------------------------------------------------------

void RoutingEstimator::getGCellCenterPos(const int i, const int j, double& x, double& y) {
	x = (i * _tileWidth) + _originX + _tileWidth * 0.5;
	y = (j * _tileHeight) + _originY + _tileHeight * 0.5;
} // end method 

// -----------------------------------------------------------------------------
/*
void RoutingEstimator::getGridPosition(double x, double y, int &i, int &j) {
	i = (int)((x - _originX)/_tileWidth);
	j = (int)((y - _originY)/_tileHeight);
} // end method 
 */
// -----------------------------------------------------------------------------

void RoutingEstimator::ResetNetDB() {
	_netdb.gridX = 0;
	_netdb.gridY = 0;
	_netdb.layer = 0;
	_netdb.blockage_porosity = 0;
	_netdb.objectNum = 0;
	_netdb.netArray.clear();
	_netdb.unRouteNet.clear();
	_netdb.adjArray.clear();
	_netdb.horCap.clear();
	_netdb.verCap.clear();
	_netdb.wireWidth.clear();
	_netdb.wireSpace.clear();
	_netdb.viaSpace.clear();
	_netdb.wireSize.clear();
	_netdb.horCap.clear();
	_netdb.verCap.clear();
	_netdb.numLocalPin.clear();
	_netdb.cellArray.clear();
	_netdb.netInfoArray.clear();
	_netdb.rowArray.clear();

	_netdb.lower_left_x = 0;
	_netdb.lower_left_y = 0;
	_netdb.cell_height = 0;
	_netdb.cell_width = 0;

} // end method 

// -----------------------------------------------------------------------------

void RoutingEstimator::ResetRouterParm() {
	_routerParam.MnRounds = 0;
	_routerParam.Optimize3DRounds = 0;
	_routerParam.PostRounds = 0;
	_routerParam.PtRounds = 0;
	_routerParam.RarRounds = 0;
	_routerParam.baseCost = 0;
	_routerParam.begeinHis = 0;
	_routerParam.blockExpand = 0;
	_routerParam.blockFactor = 0.0;
	_routerParam.costFunction = 0;
	_routerParam.decrBaseCostIt = 0;
	_routerParam.ignorePin = 0;
	_routerParam.laType = 0;
	_routerParam.localIt = 0;
	_routerParam.mazeTimeout = 0;
	_routerParam.ofReduceRounds = 0;
	_routerParam.outputOverflow = false;
	_routerParam.outputResult = false;
	_routerParam.pinFactor = 0.0;
	_routerParam.placeIt = 0;
	_routerParam.placeRound = 0;
	_routerParam.print_to_screen = false;
	_routerParam.reAssignRounds = 0;
	_routerParam.targetOverflow = 0;
	_routerParam.viaCost = 0;
	_routerParam.wl_base = 0;
	_routerParam.wl_max = 0.0;
} // end method 

// -----------------------------------------------------------------------------

void RoutingEstimator::PrintNetDB(std::ostream& out) {
	out << "netDB -> adjArray.size() " << _netdb.adjArray.size()
		<< "\nnetDB -> blockage_porosity " << _netdb.blockage_porosity
		<< "\nnetDB -> cellArray.size() " << _netdb.cellArray.size()
		<< "\nnetDB -> cell_height " << _netdb.cell_height
		<< "\nnetDB -> cell_width " << _netdb.cell_width
		<< "\nnetDB -> gridX " << _netdb.gridX
		<< "\nnetDB -> gridY " << _netdb.gridY
		<< "\nnetDB -> horCap.size() " << _netdb.horCap.size()
		<< "\nnetDB -> horTrack.size() " << _netdb.horTrack.size()
		<< "\nnetDB -> layer " << _netdb.layer
		<< "\nnetDB -> lower_left_x " << _netdb.lower_left_x
		<< "\nnetDB -> lower_left_y " << _netdb.lower_left_y
		<< "\nnetDB -> netArray.size() " << _netdb.netArray.size()
		<< "\nnetDB -> netInfoArray.size() " << _netdb.netInfoArray.size()
		<< "\nnetDB -> numLocalPin.size() " << _netdb.numLocalPin.size()
		<< "\nnetDB -> objectNum " << _netdb.objectNum
		<< "\nnetDB -> rowArray.size() " << _netdb.rowArray.size()
		<< "\nnetDB -> unRouteNet.size() " << _netdb.unRouteNet.size()
		<< "\nnetDB -> verCap.size() " << _netdb.verCap.size()
		<< "\nnetDB -> verTrack.size() " << _netdb.verTrack.size()
		<< "\nnetDB -> viaSpace.size() " << _netdb.viaSpace.size()
		<< "\nnetDB -> wireSize.size() " << _netdb.wireSize.size()
		<< "\nnetDB -> wireSpace.size() " << _netdb.wireSpace.size()
		<< "\nnetDB -> wireWidth.size() " << _netdb.wireWidth.size()
		<< "\n";
} // end method 

// -----------------------------------------------------------------------------

void RoutingEstimator::PrintRouterParm(std::ostream& out) {

	out << "_routerParam - > MnRounds: " << _routerParam.MnRounds
		<< "\n_routerParam - > Optimize3DRounds: " << _routerParam.Optimize3DRounds
		<< "\n_routerParam - > PostRounds: " << _routerParam.PostRounds
		<< "\n_routerParam - > PtRounds: " << _routerParam.PtRounds
		<< "\n_routerParam - > RarRounds: " << _routerParam.RarRounds
		<< "\n_routerParam - > baseCost: " << _routerParam.baseCost
		<< "\n_routerParam - > begeinHis: " << _routerParam.begeinHis
		<< "\n_routerParam - > blockExpand: " << _routerParam.blockExpand
		<< "\n_routerParam - > blockFactor: " << _routerParam.blockFactor
		<< "\n_routerParam - > costFunction: " << _routerParam.costFunction
		<< "\n_routerParam - > decrBaseCostIt: " << _routerParam.decrBaseCostIt
		<< "\n_routerParam - > ignorePin: " << _routerParam.ignorePin
		<< "\n_routerParam - > laType: " << _routerParam.laType
		<< "\n_routerParam - > localIt: " << _routerParam.localIt
		<< "\n_routerParam - > mazeTimeout: " << _routerParam.mazeTimeout
		<< "\n_routerParam - > ofReduceRounds: " << _routerParam.ofReduceRounds
		<< "\n_routerParam - > outputOverflow: " << _routerParam.outputOverflow
		<< "\n_routerParam - > outputResult: " << _routerParam.outputResult
		<< "\n_routerParam - > pinFactor: " << _routerParam.pinFactor
		<< "\n_routerParam - > placeIt: " << _routerParam.placeIt
		<< "\n_routerParam - > placeRound: " << _routerParam.placeRound
		<< "\n_routerParam - > print_to_screen: " << _routerParam.print_to_screen
		<< "\n_routerParam - > reAssignRounds: " << _routerParam.reAssignRounds
		<< "\n_routerParam - > targetOverflow: " << _routerParam.targetOverflow
		<< "\n_routerParam - > viaCost: " << _routerParam.viaCost
		<< "\n_routerParam - > wl_base: " << _routerParam.wl_base
		<< "\n_routerParam - > wl_max: " << _routerParam.wl_max
		<< "\n";
} // end method 

// -----------------------------------------------------------------------------

void RoutingEstimator::PrintGrid(std::ostream & out, bool onlyCongestion) {
	for (int l = 0; l < _numLayers; l++) {
		for (int i = 0; i < _gridSizeX - 1; i++) {
			for (int j = 0; j < _gridSizeY - 1; j++) {
				if (onlyCongestion && GetTileCong(l, i, j) == 1)
					continue;
				out << "Layer: " << l << " Grid: " << i << "," << j << " Cong: " << GetTileCong(l, i, j)
					<< " VCong: " << GetTileVCong(l, i, j) << " HCong: " << GetTileHCong(l, i, j)
					<< " VUsage: " << GetVEdgeUsage(l, i, j) << " HUsage: " << GetHEdgeUsage(l, i, j)
					<< " VCap: " << GetVEdgeCapacity(l, i, j) << " HCap: " << GetHEdgeCapacity(l, i, j)
					<< " URatio: " << GetUseRatio(l, i, j) << " VURatio: " << GetVUseRatio(l, i, j)
					<< " HURatio: " << GetHUseRatio(l, i, j)
					<< "\n";
			} // end for 
		} // end for 
	} // end for 
} // end method 

// -----------------------------------------------------------------------------
} // end namespace 



