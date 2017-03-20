#ifndef PLACEMENT_H
#define PLACEMENT_H

#include <iostream>
#include <vector>
#include <algorithm>

#include "Point.h"
#include "Net.h"
#include "Cell.h"
#include "Timer.h"
#include <set>
using namespace std ;
/*
struct placeCell{
     int id;
     int xlow, ylow, width, height, moveType, pinLayer;
     vector<shape> shapeArray;
     vector<int> blockLayer;
     set<TwoPin *> twoSet;
};
*/
struct Bin{

	int capacity;
	int demand;
	int history;
	int blockArea;

	double blockTrackNum;	
	double routeRatio;

	int numPin;
	int numVia;
	//int numBranch;
	vector<LocalRow> localArray;
	set<int> cellIndex;
	vector<SubRow*> subArray; 
};

class Placement {
	
private:
	vector<Row> rowArray;
	int xNum, yNum;
	int numMoveCell;
	double viaFactor;
	int totalBinCap, totalRowCap;
public:
	
	vector< vector< Bin > > binArray;
	vector< placeCell> cellArray;
	vector< vector<TwoPin *> > twoSet;
	vector< vector<int > > hor_vdem;
	vector< vector<int > > ver_vdem;
	vector<int> track;

	double pinFactor;
	int rightMost, topMost;
	int tileH, tileW;
	int oriX, oriY;
	bool initHis;

	//initial
	void initial(vector<placeCell> &cellArray, vector<Row> &rowArray, vector<int> &tr, int x, int y, int z
			, int oX, int oY, int tW, int tH, double pinF, int be);
	void insertMacro(int cell, int moveType);

	void initialRowCap();
	void increaseCap(int xlow, int ylow, int right, int top);	
	void initVirtualDem();

	//ultility
	void insertBin(int cell, int x, int y);
	void ripupBin(int cell);
	void inline getPinBin(pinLocate &pin, int &x, int &y);
	void inline placeRec(int xlow, int ylow, int right, int top, int isPlace);
	void inline blockArea(int xlow, int ylow, int right, int top, vector<int> layer);
	float binDesCost(int binX, int binY, int size);
	float pinDesCost(int binX, int binY, int numPin);
	void inline getBin(int &binX, int &binY, int realX, int realY);
	int updateHistory();
	int resetHistory();
	void updateVirtualDemandHor(int x, int y);
	void updateVirtualDemandVer(int x, int y);
	void showPinNum(int tTileW, int tTileH);
	void showViaNum();
	void checkVirtualCap();

	//legalization
	void initialSubRow(int be);
	void initialSubRowinBin();
	inline int overlapType(int cleft, int cDown, int cRight, int cTop, int rleft, int rDown, int rRight, int rTop);
	void cutRow(int cLeft, int cDown, int cRight, int cTop, Row &rr);
	void legalizeAllCell();
	void legalizeCell(placeCell &cc, vector<SubRow *> subArray, float &minCost, SubRow *&minRow, int &placeX);
	float tryPlaceCell(placeCell &cc, SubRow &sub);
	inline void addCell(Cluster &clus, placeCell &cc, int i);
	void resetSubRow();
	float collapse(placeCell &cc, SubRow &sub);
	inline void addCluster(Cluster &pre, Cluster &next);
	void placeCell2SubRow();
	//void extendBin(int binX, int binY, vector<SubRow*> &subArray);
	void getSubRowSet(int binX, int binY, vector<SubRow*> &subArray, int dis, int cellId);
	inline float getMoveCost(placeCell &cc, SubRow &sub, int moveX, int moveY);
	inline int bestPlaceX(placeCell &cc, SubRow &sub);
	int getManhDis(placeCell &cc, int newX, int newY);
	void getTotaloptimalDis();

	//Detailed placement
	void localCellMove();
	inline void project(int &x, int &y, int left, int right, int down, int top);
	int calcLocalLength(placeCell &cc, int moveX, int moveY, int unConnectCellIndex);	
	int insideBinChange(float &time);
	inline bool isInsideBin(placeCell &cc);
	int getChangeGain(placeCell &cc1, placeCell &cc2);
	int slidingWindow(float &time);
	int changeOrder(LocalRow &lr, vector<int>::iterator it, int order_size, vector<int> &order, vector<int> &bestOrder);
	int emunerateCost(vector<int>::iterator it, vector<int> &order, int left, vector<int> &bestOrder, int &bestCost, int depth);
	int gainOfChangeTwoCell(placeCell &cc1, placeCell &cc2);
	void buildLocalRow();
	inline int getMoveCost(placeCell &cc, int fromX, int toX, int ylow, int optiX, int &placeX);
	void getBestPoint(placeCell &cc, int &bestX, int &bestY, vector<int> &xLocate, vector<int> &yLocate);
	int replace2WhitePlace(float &time);
	//void seperateCell();


	//varification
	void testOverlap();
	int showPlaceInfo();
	void checkCellinRow();
};
 



#endif
