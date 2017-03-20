#ifndef NLA
#define NLA

#include<iostream>
#include<vector>
#include<set>
#include<cmath>
#include<string>
//#include<time.h>
#include"Timer.h"
#include"NetDB.h"

using namespace std;


typedef struct net;
typedef struct edge;
typedef struct sol;
typedef struct solVia;
typedef struct node;
typedef struct pin;
typedef struct line;

#define PostRound 1
#define VIA_COST 100
#define WIRE_OF_COST 1000000
//#define MAX_CONG_COST  10
#define MAX_CONG_COST  20
//#define SP_COST 1
//#define SP_COST 0
/*
static bool order_cp(net * a, net *b){
     return (a->score > b->score) ;
}
*/

struct GridEdge3D
{
	int dem;
	int his;
};

struct Graph
{
	//overflow info
	int totalOF;
	int maxOF;
	int totalVia;
	int origiWL;


	int xNum, yNum, zNum;
	int numNet;
	double slope;
	double Amax;
	bool postLA;
	int maxLen;
	int spCost;
	int prunType;
	int binSize;	
	int total2DWL;
	int viaCost;
	int unRouteVia;
	double weight;
	//layer info
	vector<int> wireSpace, wireWidth, wireSize;


	//2D
	int v_cap2D, h_cap2D;
	int **horDemand, **verDemand;
	//int **horDemand, **verDemand;
	//int **horCapacity, **verCapacity;
	int **horCompactOF, **verCompactOF;
	int **horCeilOF, **verCeilOF;
	int **horFloorOF, **verFloorOF;
	int blx,bly, cellW, cellH; 
	int totalWireOF;

	//Timer
	Timer t_initialLeaf;
	Timer t_propagate;
	Timer t_merge;
	Timer t_pruneSol;
	Timer t_topDown;
	Timer t_kernelLA;
	Timer t_utility;

	//3D
	//int *h_cap, *v_cap;
	//int ***horDem, ***verDem;
	//int ***horHis, ***verHis;
	vector< vector<int> > cap3D;
	vector< vector< vector< vector<GridEdge3D> > > > gEdge3D;
	
	//DP-based sol pruning
	int ****binMinCost;
	sol ****binBestSol;

	Graph();
	~Graph();

	vector<net> netArray;
	vector<net *> netAddress;
	vector<net> unRouteNetArray;
	vector< vector< vector<sol> > > setArray;
	vector< vector<solVia> > solArray;
	//vector<solSet> setArray;	
	//IO
	void input2Dresult(char *);
	void input3Dresult(char *);
	void output3Dresult(const char *);
	void outputOverflowInfo(const char *filename);
	void initialGraph();
	void BFS_net(int **nodeGraph, int **horEdgeG, int **verEdgeG,int **herEdgeLay, 
			  int **verEdgeLay, int **pinGraph, net &nn);
	void BFS_net(vector<vector<int> > &nodeGraph, vector<vector<int> > &horEdgeG, vector<vector<int> > &verEdgeG,vector<vector<int> > &horEdgeLay,
				 vector<vector<int> > &verEdgeLay, vector<vector<int> > &pinGraph, vector<vector<int> > &pinMaxLay, vector<vector<int> > &pinMinLay, net &nn);

	
	//LA
	void NVM(int bs, int spc, int pType, bool considerAA, bool print_to_screen);
	void COLA(int bs, int spc, int pType, bool considerAA, bool print_to_screen);
	void fastLA(int bs, int spc, int pType, bool considerAA, bool print_to_screen);
	void greedy_singleNetLA(net &nn, bool considerAA, bool considerCong);
	bool singleNetLA(net &nn, bool considerAA);
	void initialLeaf(net &nn, int nodeIndex, vector< vector< solVia > > &solArray);
	void propagate(net &nn, int nodeIndex, vector< vector< solVia > > &solArray);
	void mergeSubTree(net &nn, int nodeIndex, vector< vector< solVia > > &solArray);
	void solPrunning(vector< vector<sol> > &);
	void topDownAssignment(net &nn, vector< vector< solVia > > &solArray);

	void AA_initialLeaf(net &nn, int nodeIndex, vector< vector< vector<sol> > > &setArray);
	void AA_propagate(net &nn, int nodeIndex, vector< vector< vector<sol> > > &setArray);
	void AA_mergeSubTree(net &nn, int nodeIndex, vector< vector< vector<sol> > > &setArray);
	void AA_topDownAssignment(net &nn, vector< vector< vector<sol> > > &setArray);
	bool AA_solPrunning(vector< vector<sol> > &);
	bool AA_solEasyPrunning(vector<sol> &solVec );
	bool AA_solBinPrunning(vector<sol> &solVec, bool DP );
	bool AA_solBinPrunning1(vector<sol> &solVec, bool DP );
	//int countVioPin()	
		
	//integrate 3D route
	void initialLA(NetDB &netdb, int vc, double pinFactor);
	void convertGRtoLA(NetDB &netdb, bool print_to_screen);
	void reset(NetDB &netdb);
	void convertLAtoGR(NetDB &netDB);

	//tool
	inline int get3DDem(int x, int y, int z, int hori);
	inline void set3DDem(int x, int y, int z, int hori, int value);
	inline void add3DDem(int x, int y, int z, int hori, int value);
	inline void incr3DDem(int x, int y, int z, int hori);  //add a net edge
	inline void decr3DDem(int x, int y, int z, int hori);  //minu a net edge

	inline int get3DHis(int x, int y, int z, int hori);
	inline void set3DHis(int x, int y, int z, int hori, int value);
	inline void incr3DHis(int x, int y, int z, int hori);

	inline int getCost(edge &e, int lay);
	inline void getStraight(vector<line> &lines, vector<edge> &edgeArray, vector<line> &tmpLines, vector< vector<int > > &tmpInfo);
	void updateHistoryCost();
	int getTotalOverflow(int &maxOF, double &wieghtOF);
	int getTotalVia();
	int getSingleNetlVia(net &cn);
	void ripUp(net &nn);
	bool hasOverflow(net &nn);
	int checkAntennaRule(net &nn);
	int checkAntennaRule2(net &nn);
	int checkAntennaRule3(net &nn);
	//bool hasViolation(net &nn);
	void buildOverlapGraph();
	void sort_net();
	int calMinSP();
	int varifyAntenna();
	void showInfo();
	void reportCPUTime();
	//Graph(){ postLA=false; }
};


struct net{
	int netID;
	string name;
	int spNum;
	int numVia;
	double score;
	vector<node> nodeArray;
	vector<edge> edgeArray;
	vector<pin> pinArray;
	set<int> overlapNet;
	net(int id){ netID=id; spNum=0;}
	net(){};
	/*
	bool operator < (const net &a, const net &b)
	{
		return a.score < b.score;
	}
	*/
};

// 4-tuple
struct sol   //v is root, t is all down stream tree, pt is partial tree, Sp is sperator
{
	int vL;
	int maxLay;
	int minLay;
	//int viaNum;

	//Aratio(tv)
	int ptLen;       //AL
	int ptPinNum;
	//MTL(tv)
	int ptTopLayer; //TW 越低越好

	//sperator      //sperator的layer要小於等於 downstream的sperator
	int tBoundSpLay;  //LS 越高越好
	int tSpNum;

	//cost
	int cost;

	//connection
	int chiLayer[4];
	int chiSolIndex[4];

};

struct solVia   //v is root, t is all down stream tree, pt is partial tree, Sp is sperator
{
	int vL;
	int maxLay;
	int minLay;
	//int viaNum;

	//cost
	int cost;

	//connection
	int chiLayer[4];

};



struct node{

	int SpIndex;
	int localTreeSize;
	int localTreePinNum;

	int x, y;
	int degree;
	int chiIndex[4];
	int parIndex;

	bool pin;
//	int pinLayer;
	int pinMaxLay;
	int pinMinLay;

	sol bestSol;
	solVia bestSolVia;
};

struct edge{
	int x, y, z;
	bool hori;
};

struct pin{
	int x, y, z;
	pin(int ax, int ay, int az){x=ax; y=ay; z=az;};
	pin(){};

};
/*
struct solSet
{
	//vector<sol>  laySet[MAX_LAY];
	vector< vector<sol> >  laySet;
};
*/

struct line
{
  int x, y, z;
  int hori;
  int len;
};

#endif
