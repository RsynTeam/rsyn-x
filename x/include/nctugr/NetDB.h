#ifndef NETDB_H
#define NETDB_H

#include "Net.h"
#include <vector>
#include <set>
#include "Cell.h"
/*
struct shape{
      int xlow, ylow, width, height;
};

struct placeCell{
     int id;
     int xlow, ylow, width, height, moveType, pinLayer;
     vector<shape> shapeArray;
     vector<int> blockLayer;
	set<TwoPin *> twoSet;
	//
//vector<TwoPin>
};


struct pinLocate{
     int cellIndex;
     float xOffset;
     float yOffset;
};

struct netInfo{
     int id;
     vector<pinLocate> pinArray;
};

struct Row{
	int xlow, ylow, width, height;
	int siteWidth, siteSpace;
};
*/
struct ParamSet
{
	//base
	int viaCost;
	int ignorePin;
	int laType;
	bool outputResult;	
	bool outputOverflow;
	bool print_to_screen;	
	double pinFactor;

	int PtRounds;
	int MnRounds;
	int RarRounds;
	int mazeTimeout;
	int ofReduceRounds;
	int PostRounds;
	int Optimize3DRounds;

	int begeinHis;
	int reAssignRounds;
	int targetOverflow;
	int decrBaseCostIt;
	//Maze
	int costFunction;
	int baseCost;
	int wl_base;
	double wl_max;

	//placement
	int placeIt;
	int placeRound;
	int localIt;
	double blockFactor;
	int blockExpand;
};


class NetDB {
	public:
		//int gridX, gridY ;
		int gridX, gridY ;
		int layer;
		int blockage_porosity;
		//int ver, hor ;
		int objectNum;
		vector<Net> netArray ;
		vector<Net> unRouteNet;
		vector < AdjustCapEdge > adjArray;
		vector<int> horTrack, verTrack ;
		vector<int> wireWidth, wireSpace, viaSpace, wireSize;
		vector<int> horCap, verCap ;
		vector< vector<int> > numLocalPin ;
		//vector< vector<int> > numGpin ;
		//DAC info
		vector<placeCell> cellArray;
		vector<netInfo> netInfoArray;
		vector<Row> rowArray;	


		int lower_left_x, lower_left_y;
		int cell_width, cell_height;
		int RealX( int x ) {
			return lower_left_x + x * cell_width ; 
		}
		int RealY( int y ) {
			return lower_left_y + y * cell_height ;
		}
/*
		NetDB(){} ;
		NetDB( const NetDB &t ){
			gridX = t.gridX ;
			gridY = t.gridY ;
			net_info = t.net_info ;
			ver = t.ver ;
			hor = t.hor ;
			net = t.net ;
			adjVec = t.adjVec ;
			adjAmount = t.adjAmount ;
			lower_left_x = t.lower_left_x ;
			lower_left_y = t.lower_left_y ;
			cell_width = t.cell_width ;
			cell_height = t.cell_height ;
			
		} ;
*/


		NetDB & operator=( const NetDB &t ){
			gridX = t.gridX ;
			gridY = t.gridY ;
			//ver = t.ver ;
			//hor = t.hor ;
			netArray = t.netArray ;
			//adjVec = t.adjVec ;
			//adjAmount = t.adjAmount ;
			lower_left_x = t.lower_left_x ;
			lower_left_y = t.lower_left_y ;
			cell_width = t.cell_width ;
			cell_height = t.cell_height ;
			return *this ;	
		} ;

	private:
		

};

#endif
