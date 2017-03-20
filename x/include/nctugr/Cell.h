#ifndef CELL_H
#define CELL_H

//#include <cmath>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <set>
#include "Point.h"
using namespace std;

#define MOVE_CELL 0
#define FIXED_CELL 1
#define FIXED_PAD 2
//typedef struct TwoPin;


struct Cluster{

	int first, end;
	int left, right;
	//double e, q;//, w;
	double e, q;//, w;
};

struct SubRow{
	int label; 
     int xlow, ylow, xTop, yTop;
	int demandX;
	vector<int> indexArray;
	vector<Cluster> clusterArray;
	vector<Cluster> tmpClustArray;
	
	//list<int> ;
};

struct LocalRow{
	int xlow, xTop;
	int xDemand;
	int ylow;
	//list<int> indexList;
	vector<int> indexList;
};

struct Row{
     int xlow, ylow, width, height;
     int siteWidth, siteSpace;
	list<SubRow> subList;
};

struct shape{
      int xlow, ylow, width, height;
};


struct pinLocate{
     int cellIndex;
	int netIndex;
	int pinType;
     //float xOffset;
     //float yOffset;
	int offsetX;
	int offsetY;
};


struct netInfo{
     int id;
	string name;
     vector<pinLocate> pinArray;
     vector<int> pinIndex;
};


struct placeCell{
     int id, index, macroType;
     int xlow, ylow, width, height, moveType, pinLayer;
	int legX, legY;
	int legTmpX;
//	int manDis;
	float minLegCost;
	float sortCost;

	int isFixed;
	int nextX;
	//int legTmpY;
//	int *inRowIndex;
	//list<int>  *mylist;
	//list<int>::iterator myit;
	bool isTrace;
	//bool isCompute;

     vector<shape> shapeArray;
     vector<int> blockLayer;
     vector<pinLocate> pinArray;
	//set<TwoPin *> twoSet;
//vector<TwoPin>
};

struct Macro{
	int key;
	int moveType;
	int macroType;
	int width, height;
     vector<shape> shapeArray;
	vector<Point> pins;
     vector<int> blockLayer;
};


#endif
