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
 * File:   RoutingEstimator.h
 * Author: Nima Karimpour Darav 
 * Email: nkarimpo@ucalgary.ca
 * Created on October 26, 2015, 4:03 PM
 */

#ifndef ROUTINGESTIMATOR_H
#define ROUTINGESTIMATOR_H

// NCTUgr Router 
#include "nctugr/interface.h"
#include "rsyn/util/Bounds.h"
#include "rsyn/util/dbu.h"
#include "rsyn/core/Rsyn.h"
#include "rsyn/phy/PhysicalDesign.h"

#include <algorithm>
#include <ostream>

namespace TRD {

class RoutingEstimator {
private:
	Rsyn::Module clsModule;
	Rsyn::PhysicalDesign clsPhDesign;
	
public:
	RoutingEstimator(Rsyn::Module module, Rsyn::PhysicalDesign phDsg);
	virtual ~RoutingEstimator();

	virtual void Initialize(const bool print = false);

	virtual void Create();
	virtual int Route();

	inline void runRouter() {
		Create();
		Route();
	}

	// Overloaded class methods for layer 0

	inline double& GetHEdgeUsage(int i, int j) {
		return getHEdge(0, i, j)._usage;
	}

	inline double& GetVEdgeUsage(int i, int j) {
		return getVEdge(0, i, j)._usage;
	}

	inline double& GetHEdgeLastUsage(int i, int j) {
		return getHEdge(0, i, j)._lastUsage;
	}

	inline double& GetVEdgeLastUsage(int i, int j) {
		return getVEdge(0, i, j)._lastUsage;
	}

	inline double& GetHEdgeCapacity(int i, int j) {
		return getHEdge(0, i, j)._cap;
	}

	inline double& GetVEdgeCapacity(int i, int j) {
		return getVEdge(0, i, j)._cap;
	}

	inline double GetTileCong(int i, int j) {
		return GetTileCong(0, i, j);
	}

	inline double GetTileHCong(int i, int j) {
		return GetTileHCong(0, i, j);
	}

	inline double GetTileVCong(int i, int j) {
		return GetTileVCong(0, i, j);
	}

	inline double GetTileVUsage(int i, int j) {
		double gcu_v = 0.0;

		gcu_v += (j == _gridSizeY - 1) ? 0 : this->GetVEdgeUsage(i, j);
		gcu_v += (j == 0) ? 0 : this->GetVEdgeUsage(i, j - 1);

		return gcu_v;
	}

	inline double GetTileHUsage(int i, int j) {
		double gcu_h = 0.0;

		gcu_h += (i == _gridSizeX - 1) ? 0 : this->GetHEdgeUsage(i, j);
		gcu_h += (i == 0) ? 0 : this->GetHEdgeUsage(i - 1, j);

		return gcu_h;

	}

	inline double GetTileVCapacity(int i, int j) {
		double gcc_v = 0.0;

		gcc_v += (j == _gridSizeY - 1) ? 0 : this->GetVEdgeCapacity(i, j);
		gcc_v += (j == 0) ? 0 : this->GetVEdgeCapacity(i, j - 1);

		return gcc_v;
	}

	inline double GetTileHCapacity(int i, int j) {
		double gcc_h = 0.0;

		gcc_h += (i == _gridSizeX - 1) ? 0 : this->GetHEdgeCapacity(i, j);
		gcc_h += (i == 0) ? 0 : this->GetHEdgeCapacity(i - 1, j);

		return gcc_h;

	}

	double GetUseRatio(int i, int j);
	double GetVUseRatio(int i, int j);
	double GetHUseRatio(int i, int j);

	double GetUseRatio(int l, int i, int j);
	double GetVUseRatio(int l, int i, int j);
	double GetHUseRatio(int l, int i, int j);

	double GetUseRatio(double x, double y);
	double GetVUseRatio(double x, double y);
	double GetHUseRatio(double x, double y);

	double GetPointCong(double x, double y);
	double GetPointHCong(double x, double y);
	double GetPointVCong(double x, double y);
	double GetHEdgeUsage(double x, double y);

	void getGCellIndex(const double posX, const double posY, int& i, int& j);
	void getGCellPos(const int i, const int j, double &x, double &y);
	void getGCellCenterPos(const int i, const int j, double &x, double &y);

	inline double& GetHEdgeUsage(int l, int i, int j) {
		return getHEdge(l, i, j)._usage;
	}

	inline double& GetVEdgeUsage(int l, int i, int j) {
		return getVEdge(l, i, j)._usage;
	}

	inline double& GetHEdgeLastUsage(int l, int i, int j) {
		return getHEdge(l, i, j)._lastUsage;
	}

	inline double& GetVEdgeLastUsage(int l, int i, int j) {
		return getVEdge(l, i, j)._lastUsage;
	}

	inline double& GetHEdgeCapacity(int l, int i, int j) {
		return getHEdge(l, i, j)._cap;
	}

	inline double& GetVEdgeCapacity(int l, int i, int j) {
		return getVEdge(l, i, j)._cap;
	}

	double GetTileCong(int l, int i, int j);
	double GetTileHCong(int l, int i, int j);
	double GetTileVCong(int l, int i, int j);

	inline double GetTotalOverflow() {
		return _totalOverflow;
	}
	double getMaxGCellCongestion();
	double getMaxVerticalGCellCongestion();
	double getMaxHorizontalGCellCongestion();
	double getMaxGCellUseRatio();

	// Initialize

	inline void SetOrigX(double x) {
		_originX = x;
	}

	inline void SetOrigY(double y) {
		_originY = y;
	}

	inline void SetGridSizeX(int size) {
		_gridSizeX = size;
	}

	inline void SetGridSizeY(int size) {
		_gridSizeY = size;
	}

	inline void SetNumLayers(int layers) {
		_numLayers = layers;
	}

	inline void SetTileWidth(double width) {
		_tileWidth = width;
	}

	inline void SetTileHeight(double height) {
		_tileHeight = height;
	}
	void SetNctuParams(int PtRounds, int MnRounds, int RarRounds, int laType, int printOut);
	void SetPrintToScreen(const bool print);

	inline void SetLayerDirections(std::vector<int>& dir) {
		_layerDir = dir;
	};
	void SetLayerblockages(std::vector< std::vector<Bounds> >& fixed);

	void setLayerCapRatio(std::vector<double>& hCapRatio, std::vector<double> & vCapRatio) {
		_hMaxCapRatio = hCapRatio;
		_vMaxCapRatio = vCapRatio;
	}
	void SetNDR(std::vector<std::vector<double> >& ndrWidth, std::vector<std::vector<double> >& _ndrSpacing);

	double PrintOverflow(bool print = false);

	int GetNumGrids(const Dimension dim) {
		if (dim == X)
			return _gridSizeX;
		if (dim == Y)
			return _gridSizeY;
		return -1;
	} // end method 

	double GetGridLength(const Dimension dim) {
		if (dim == X)
			return _tileWidth;
		if (dim == Y)
			return _tileHeight;
		return -1;
	} // end method 

	double2 getGridInitPos() {
		return double2(_originX, _originY);
	}

	void PrintNetDB(std::ostream &out);
	void PrintRouterParm(std::ostream & out);
	void PrintGrid(std::ostream &out, bool onlyCongestion = true);
	void ResetNetDB();
	void ResetRouterParm();
protected:

	class GcellEdge {
	public:

		GcellEdge() : _cap(0.0), _usage(0.0), _lastUsage(0.0) {
		}
		double _cap;
		double _usage;
		double _lastUsage;
	};

	inline GcellEdge& getHEdge(int l, int i, int j) {
		if (i < 0 || i >= _gridSizeX - 1 || j < 0 || j >= _gridSizeY || l < 0 || l >= _numLayers) {
			cout << "Error: H(" << l << "," << i << "," << j << ")" << endl;
			exit(-1);
		}
		return _hEdges[l][i][j];
	}

	inline GcellEdge& getVEdge(int l, int i, int j) {
		if (i < 0 || i >= _gridSizeX || j < 0 || j >= _gridSizeY - 1 || l < 0 || l >= _numLayers) {
			cout << "Error: V(" << l << "," << i << "," << j << ")" << endl;
			exit(-1);
		}
		return _vEdges[l][i][j];
	}

	struct compare_blockages {

		inline bool operator()(std::pair<double, double> i1, std::pair<double, double> i2) const {
			if (i1.first == i2.first) {
				return i1.second < i2.second;
			}
			return i1.first < i2.first;
		}
	};

protected:
	void collectRoutingBlockages();

	void computeStatistics();
	//void getGridPosition(double x, double y, int &i, int &j);
private:
	// Related to the routing architecture...
	int _gridSizeX;
	int _gridSizeY;
	int _numLayers;

	double _tileWidth;
	double _tileHeight;

	double _originX;
	double _originY;

	std::vector<double> _vMaxCapRatio; // A number between 0.0 and 1.0 for each layer
	std::vector<double> _hMaxCapRatio; // A number between 0.0 and 1.0 for each layer
	std::vector<int> _layerDir;
	std::vector<std::vector<double> > _ndrWidth;
	std::vector<std::vector<double> > _ndrSpacing;

	std::vector<double> _ndrHWts; // Computed weights for horizontal NDRs (the first rule is assigned to the default rule)
	std::vector<double> _ndrVWts; // Computed weights for vertical NDRs (the first rule is assigned to the default rule)

	//    std::vector< std::vector<Rectangle> > _blockages; // Routing blockages
	std::vector< std::vector< std::vector< std::pair<double, double> > > > _blockages; // Routing Blockages

	std::vector<std::vector<std::vector<GcellEdge> > > _hEdges; // Horizontal edges
	std::vector<std::vector<std::vector<GcellEdge> > > _vEdges; // Vertical edges


	double _avgHW; // Average wire width + wire spacing for horizontal direction.
	double _avgVW; // Average wire width + wire spacing for vertical direction.

	double _minHW; // Minimum wire width + wire spacing for horizontal direction.
	double _minVW; // Minimum wire width + wire spacing for vertical direction.


	double _totalHusage;
	double _totalVusage;
	double _totalHcap;
	double _totalVcap;
	double _hOverflowRatio;
	double _vOverflowRatio;
	double _avgHOverflow;
	double _avgVOverflow;
	double _totalOverflow;

	// NCTUgr 
	NetDB _netdb;
	ParamSet _routerParam;
	GlobalRouter* _gr;

};

} // end namespace 

#endif /* ROUTINGESTIMATOR_H */
