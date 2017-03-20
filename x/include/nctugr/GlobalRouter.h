#ifndef GLOBALROUTER_H
#define GLOBALROUTER_H

#include <iostream>
#include <list>
#include <cmath>
#include <vector>
#include <queue>
#include <tr1/unordered_set>
#include <algorithm>
#define MAX_GRID 1300

#include "Placement.h"
#include "pheap.h"
#include "GridGraph.h"
#include "Point.h"
#include "Net.h"
#include "Timer.h"
#include "MST.h"
#include "BoundingBox.h"
#define MONOTONIC 0
#define SNAKING 1

#define WIRE_OVERFLOW_PENALTY 1000000
#define BREAK_IT 5
#define BREAK_RATIO 0.95
#define LAST_GAP 50

#define OUTPUT_SIM_INFO 0
#ifdef GPU
#define CHK_BLK_THRESHOLD 100

#include "cuda.h"
#include "cuda_runtime.h"
//#include "main_minblock_3d.cu"
#include "Optimization.h"
bool gpu_construct(int layer, int row, int col);
bool gpu_destruct();
float gpu_maze_route(GridGraph &gg, vector<Point_i> &path, Point_i source, Point_i target, int layer, int row, int col, int minlayer, int maxlayer, float baseCost, bool route2D, int& active_times);
void copy2DMemory(Point_f *func_2Dmap, int row, int col);
void copy3DMemory(Point_f *func_map, int layer, int row, int col);
bool assignThreadNumber(int layer, int row, int col, int threadx, int thready, int threadz, bool route2D);
int totalCrossedBlock(int source_x, int source_y, int target_x, int target_y);
static int twopin_active_gpu_block_times;
#endif

//#define USE_STL_HEAP 
#ifdef USE_STL_HEAP
using std::push_heap;
using std::pop_heap;
using std::make_heap;
#endif


#define GPU_ROUTE_LENGTH 20//19
#define HISTORY_GAP 100 
#define LARGE_CONSTANT 1000000000


//#define SINGLE_VIA_COST 1
extern Timer t_ripupPath, t_addPath, t_modifyCost, t_mazeKernel, t_monoKernel, t_snakeKernel, t_humKernel, t_update_cpu;

struct controlGR {
	int maxReRouteRounds;
	float maxReRouteTime;

	bool print_to_screen;

	//log infomation
	vector<int> wireLength;
	vector<int> totalOF;
	vector<float> runtime;

};

/*
#define FROM_LEFT 2
#define FROM_RIGHT 4
#define FROM_UP 3
#define FROM_DOWN 1
#define FROM_TOP 5
#define FROM_BOTTOM 6
 */

enum {
	FROM_DOWN, FROM_LEFT, FROM_UP, FROM_RIGHT, FROM_TOP, FROM_BOTTOM
};

enum {
	PATTERN_STAGE, MONOTONIC_STAGE, RR_STAGE, POST_STAGE
};

#define MON_3D_STAGE 1
#define FAST_3D_STAGE 2
#define POST_3D_STAGE 3
#define FINAL_3D_STAGE 4

class GlobalRouter {
private:
	//int xNum, yNum ;
	//fibheap *myheap;
	//fibque *myque ;
	pheap myheap;
	//dque myque;

	int netCount;

public:

	vector< vector< vector< vector<int> > > > passNetID;
	int xNum, yNum, zNum;
	double slope;
	GridGraph g;
	Placement pl;
	// GridGraph history ;
	bool initHis;
	bool cacheMap;
	double historyPow;
	bool careFlute;
	int h_cap, v_cap;
	int tof;
	//cost
	float via_cost;
	float factor;
	float baseCost;
	float of_cost;
	float hisBase;
	float pseudoViaCost;
	float totalRuntime;

	int **horHis;
	int **verHis;
	float costArray[1000];
	int maxHis;
	int costArrayOffset;
	int step;
	int gpuRouteNet;
	int totalWL;
	int totalVia;
	int estiCongMethod;
	//new
	vector< Point > T1, T2;
	vector< int> wireSize;
	vector<int> horCap;
	vector<int> verCap;
	vector< vector< vector<float> > > routingCost2DMap;
	vector< vector< vector<float> > > routingCost2DMapTrans;
	//		int** horCap2D;
	//		int** verCap2D;
	BoundingBox **horBox;
	BoundingBox **verBox;
	//float **horBase;
	//float **verBase;
	//int problemCell;

	int **back;
	float **dp;
	int **back2;
	float **dp2;
	int **backTrans;
	float **dpTrans;

	//3D
	int step3D;
	int singleViaCost;
	int initHis3D;
	int ***back3D;
	float ***dp3D;
	int ***back3D2;
	float ***dp3D2;
	int BLOCK_HEIGHT;
	int BLOCK_TOTAL;
	/*
	vector<float> length_g, length_c, length_2p_g, length_2p_c;
	vector<float> blength_g, blength_c, blength_2p_g, blength_2p_c;
	vector<float> barea_g, barea_c, barea_2p_g, barea_2p_c;
	vector<float> final_g, final_c, final_2p_g, final_2p_c;
	vector<int> num_length, num_2p_length;
	vector<int> num_blength, num_2p_blength;
	vector<int> num_barea, num_2p_barea;
	vector<int> num_final, num_2p_final;	
	 */
#ifdef USE_STL_HEAP
	vector<GridCell *> queHeap;
#endif

	GlobalRouter(int x, int y, int layer, vector<int> horCap, vector<int> verCap)
	: xNum(x), yNum(y), zNum(layer), horCap(horCap), verCap(verCap), g(x, y, layer) {
		totalWL = totalVia = tof = 0;
		totalRuntime = 0;

		h_cap = v_cap = 0;
		for (int i = 0; i < zNum; i++) {
			h_cap += horCap[i];
			v_cap += verCap[i];
		}
		g.h_cap = h_cap;
		g.v_cap = v_cap;


		BLOCK_HEIGHT = 5;
		BLOCK_TOTAL = BLOCK_HEIGHT*xNum;
		netCount = 0;
		costArrayOffset = max(h_cap, v_cap);
		myheap.initial(xNum * yNum);
		//myque = new fibque ;


		horHis = new int*[xNum - 1];
		for (int i = 0; i < xNum - 1; i++)
			horHis[i] = new int[yNum];

		verHis = new int*[xNum];
		for (int i = 0; i < xNum; i++)
			verHis[i] = new int[yNum - 1];

		back = new int *[xNum];
		dp = new float *[xNum];
		back2 = new int *[xNum];
		dp2 = new float *[xNum];
		for (int i = 0; i < xNum; i++) {
			back[i] = new int [yNum];
			dp[i] = new float [yNum];
			back2[i] = new int [yNum];
			dp2[i] = new float [yNum];
		}

		backTrans = new int *[yNum];
		dpTrans = new float *[yNum];
		for (int i = 0; i < yNum; i++) {
			backTrans[i] = new int [xNum];
			dpTrans[i] = new float [xNum];
		}


		routingCost2DMap.resize(2);
		for (int dir = 0; dir < 2; dir++) {
			routingCost2DMap[dir].resize(xNum);
			for (int i = 0; i < xNum; i++)
				routingCost2DMap[dir][i].resize(yNum, 0);
		}

		routingCost2DMapTrans.resize(2);
		for (int dir = 0; dir < 2; dir++) {
			routingCost2DMapTrans[dir].resize(yNum);
			for (int i = 0; i < yNum; i++)
				routingCost2DMapTrans[dir][i].resize(xNum, 0);
		}

		back3D = NULL;
		dp3D = NULL;
		back3D2 = NULL;
		dp3D2 = NULL;

		/*
		g.recordNum = 0;
		g.record = new FuncMapChangeRecord[xNum*yNum*zNum*3+1]; // index 0 will be reserved.
		g.record[0].data = 0;
		g.record[0].position = 0;
		g.record[0].direction = 0;
		 */
	}

	~GlobalRouter() {
		//printf("start ~globalrouter\n");
		//delete [] g.record;
		//delete myque;
		//fh_deleteheap(myheap);

		for (int i = 0; i < xNum - 1; i++) {
			delete [] horHis[i];
			//       delete [] horCap2D[i];
		}
		delete [] horHis;
		//delete [] horCap2D;

		for (int i = 0; i < xNum; i++) {
			delete [] verHis[i];
			//          delete [] verCap2D[i];
		}
		delete [] verHis;
		//   delete [] verCap2D;

		for (int i = 0; i < xNum; i++) {
			delete [] back[i];
			delete [] dp[i];
			delete [] back2[i];
			delete [] dp2[i];
		}
		delete [] back;
		delete [] dp;
		delete [] back2;
		delete [] dp2;

		for (int i = 0; i < yNum; i++) {
			delete [] backTrans[i];
			delete [] dpTrans[i];
		}
		delete [] backTrans;
		delete [] dpTrans;

		//delete [] dpLay2;
		//delete [] backLay2;

		if (back3D != NULL) {
			for (int i = 0; i < xNum; i++) {
				for (int j = 0; j < yNum; j++) {
					delete [] back3D[i][j];
					delete [] dp3D[i][j];
					delete [] back3D2[i][j];
					delete [] dp3D2[i][j];

				}
				delete [] back3D[i];
				delete [] dp3D[i];
				delete [] back3D2[i];
				delete [] dp3D2[i];
			}
			delete [] back3D;
			delete [] dp3D;
			delete [] back3D2;
			delete [] dp3D2;
		}

	}

	/// GPU functions
#ifdef GPU
	void initialGPU();
	void copyMemoryToGPU2D();
	void copyMemoryToGPU3D();
	void assignGPUThread(int threadx, int thready, int threadz, bool route2D);
	void insertPath(vector<Point_i> &gpuPath, vector<RPoint> &cpuPath);
#endif



	/// initial functions
	void initial2DGraph(vector<AdjustCapEdge> &adjArray);
	void initialFunc_3Dmap();
	void initialFunc_2Dmap(int type);
	void initialCostArray();
	void initHistory();
	void initialVirtualDem(vector< vector<int> > &numLocalPin, double pinFactor);
	void initblockAwareModle(double blockFactor);

	void initial3DRouting(vector<int> &horCap, vector<int> &verCap, vector<int> &wSize, vector<AdjustCapEdge> &adjArray
		, vector< vector<int> > &numLocalPin, double pinFactor);

	void initialTargetSource(TwoPin *two, Net& net, bool twoPin, vector< Point > &T1, vector< Point > &T2, GridCell ***GCell, BoundingBox &targetBox);
	void buildPassNetInfo(vector<Net> &netArray);
	/// initial functions (end)

	/// set/reset functions
	void updateHistoryCost();
	void UpdateHistory(bool increament);
	void clear3Dhistory();
	void reset3DGraph();
	/// set/reset functions (end)

	/// get functions
	int getIntrinsicOverflow();
	int getMaxHistory(int &x, int &y, int &maxOF);
	int getOverflow(void);
	int getWirelength(int &maxOf);
	int getWirelength();
	void getScore(int& maxDemand, int& violation, int& via, int& length, TwoPin* &n);
	void get3DScore(int& maxDemand, int& violation, int& via, int& length, TwoPin* &n);
	void showPath(vector<RPoint> &path);
	int showWL();
	void outputOverflowInfo(const char *filename);
	/// get functions (end)

	/// estimate congestion functions
	void estimateCongestion(vector<Net> &net);
	void extendVer(vector<RPoint> &path, int x, int y, int &maxY, int &minY, int bottom, int top, int maxLength);
	void getSearchRegionVer(int maxY, int minY, int centX, int &maxX, int &minX, int left, int right, int expandSize);
	void pseudoRoutingVer(vector<RPoint> &tmpPath, int centX, int maxX, int minX, int maxY, int minY
		, int **back, float **dp, int **back2, float **dp2);
	void extendHor(vector<RPoint> &path, int x, int y, int &maxX, int &minX, int left, int right, int maxLength);
	void getSearchRegionHor(int maxX, int minX, int centY, int &maxY, int &minY, int bottom, int top, int expandSize);
	void pseudoRoutingHor(vector<RPoint> &tmpPath, int centY, int maxX, int minX, int maxY, int minY
		, int **back, float **dp, int **back2, float **dp2);


	//void identifyPath(vector<RPoint> &tmpPath, int centX, int maxX, int minX, int maxY, int minY, 
	//				int **back, float **dp, int **back2, float **dp2);
	//void identifyPath(vector<RPoint> &tmpPath, BoundingBox &br, int **back, float **dp, int **back2, float **dp2);
	void identifyPath(TwoPin *twopin, int **back, float **dp, int **back2, float **dp2);
	void identifyPathTrans(TwoPin *twopin, int **back, float **dp, int **backTrans, float **dpTrans);
	void monTraceBack(vector<RPoint> &tmpPath, int x, int y, int **back);
	void monTraceBackTrans(vector<RPoint> &tmpPath, int x, int y, int **back);

	void pseudoRoutePath(vector<RPoint> &path, int hori, BoundingBox &bb);
	bool isRipUpble(int i, int j);
	void initialBoundingBox(vector<Net> &net);
	void reRouteOverflow(vector<RPoint> &tmpPath, int r, int x, int y, bool hori, int **back, float **dp, int **back2, float **dp2);
	/// estimate congestion functions (end)

	/// fast routing
	void buildMST(vector<Net> &net, bool print_to_screen);
	void buildCongAwareMST(vector<Net> &net);
	void initialRouting(vector<Net> &net, vector<Order> &order, int ptRounds, int dpRounds, bool print_to_screen);
	void fastRouting(vector<Net> &net, vector<Order> &order, int mazeRounds, int mazeTimeout, bool print_to_screen);
	inline void getHorVerOverflow(vector<RPoint> &path, int &horOF, int &verOF);
	void escapRouting(TwoPin *two, BoundingBox &br, int **back, float **dp, int **back2, float **dp2);
	void escapSnakeRouting(TwoPin *two, BoundingBox &br, int **back, float **dp, int **back2, float **dp2);
	void snakingRouting(TwoPin *two, BoundingBox &br, bool horiSnake);
	void spreadOF();
	void predictOF(int baseCost);
	void pseudoMonotonic(Point &from, Point &to, int **back, float **dp);
	void monotonicExpand(Point& a, BoundingBox &br, int **parent, float **dp);
	void monotonic2D(TwoPin *two, int **back, float **dp);
	void snakingRoutingHor(Point &from, Point &to, BoundingBox &br, int **back, float **dp);
	void snakingRoutingVer(Point &from, Point &to, BoundingBox &br, int **back, float **dp);
	inline float minCostTopBottom(int x1, int x2, int y, float **dp, float **dp2);
	inline float minCostLeftRight(int x, int y1, int y2, float **dp, float **dp2);
	inline float minCostTopBottomTrans(int x1, int x2, int y, float **dp, float **dp2);
	inline float minCostLeftRightTrans(int x, int y1, int y2, float **dp, float **dp2);
	void showCongestionInfo();
	inline void specializeRoutingCostMap(BoundingBox &br, Net &net, int hori);
	inline void normalizeRoutingCostMap(BoundingBox &br, Net &net, int hori);
	inline void updateCongMap(vector< vector<double> > &horCost, vector< vector<double> > &verCost);
	inline double getEstiPatternCost(vector< vector<double> > &horCost, vector< vector<double> > &verCost, Point &from, Point &to);
	/// end fast routing

	///Placement function
	void initialPlacement(vector<placeCell> &cellArray, vector<Row> &rowArray, int oriX, int oriY, int tileW, int tileH, double pinF, int be);
	void getPinGcell(pinLocate &pinLoc, int &x, int &y, int &z);
	void getPinLocation(pinLocate &pinLoc, int &x, int &y, int &z);
	void place_and_route(vector<Net> &net, vector<Order> &order, controlGR &control, int maxRound, int iteration);
	void identifyMoveLocation(int cellIndex, BoundingBox &moveBox, int binX, int binY, int &newBinX, int &newBinY, int &newRealX, int &newRealY);
	inline void getBinIndex(int realX, int realY, int &binX, int &binY);
	void updateNetInfo(int cellIndex);
	void buildCostMap(placeCell &cc, pinLocate &pp, TwoPin *two, Point &from, BoundingBox &br, int **back, float **dp, int **back2, float **dp2);
	void buildPinSet(Net &n);
	inline void getLocalPlace(int binX, int binY, int &realX, int &realY);
	inline bool isInside(int xlow, int ylow, int width, int height);
	void legalize();
	void clearCellInfo();
	void snakingHorVer(TwoPin *two, BoundingBox &br, int **back, float **dp, int **back2, float **dp2);
	void sortMoveCell(vector<placeCell *> &moveArray);
	void showCellWidthInfo();
	void updateVirtualCost(placeCell &cc);
	int getVirtualOF(int &maxVof);
	void addViatoBin(vector<RPoint> &path);
	void updateViatoBin(vector<RPoint> &path, int incr);
	void initialBranch(vector<Order> &order);
	void localDetailedPlace(int rounds);
	///endl place

	/// utility functions
	int isNeedRoute(TwoPin *two, Net& net);
	void sortHistory();
	void checkCost(int type);
	void check3DCost(int type);
	void convertHistoryPow(double newHisPow);
	void convertHistoryBase(float newHisBase);
	void historyInc(vector<RPoint>& n);
	void FluteFlat(const Point& from, const Point& to, Net& net);
	void addAdjustment(int x, int y, int preRoute, bool hori);
	void RipUp3D(vector<RPoint>& n, Net& net);
	void RipUp(Point& from, Point& to, vector<RPoint>& n, Net& net);
	int RoutePath(const Point& from, const Point& to, vector<RPoint>& n, Net& net);
	int Route3DPath(vector<RPoint>& n, Net& net);
	int Flat(Point& from, Point& to, vector<RPoint>& n, Net& net);
	float PathScore(TwoPin *two, Net& net);
	float PathScore3D(TwoPin *two, Net& net);
	int MultiMaze(TwoPin *two, int maxWLength, BoundingBox &mazeBox, bool star);
	void traceBack(TwoPin *two, bool found, GridCell *fromCell, bool route2D);
	int Pattern(Point& from, Point& to, TwoPin *two, int &maxDemand);
	int DDPattern(Point& from, Point& to, TwoPin *two, int &maxDemand);
	/// utility functions (end)


	/// 3D routing functions
	void initialRouting3D(vector<Net> &net, vector<Order> &order, controlGR &control);
	void fastRouting3D(vector<Net> &net, vector<Order> &order, controlGR &control);
	int MultiMaze3D(TwoPin *two, int maxWLength, BoundingBox &mazeBox);
	void monotonic3D(Point &from, Point &to, int ***back, float ***dp);
	void initial3Dmalloc();
	void monoRouting3D(TwoPin *two);
	inline void changeLayer(int x, int y, int ***back, float ***dp);
	inline void initialSourceLayer(int x, int y, int z, int ***back3D, float ***dp3D);
	void monTraceBack3D(vector<RPoint> &tmpPath, int x, int y, int z, int ***back);
	float escapRouting3D(TwoPin *two, BoundingBox &br, int ***back3D, float ***dp3D, int ***back3D2, float ***dp3D2);
	float escapSnakeRouting3D(TwoPin *two, BoundingBox &br, int ***back3D, float ***dp3D, int ***back3D2, float ***dp3D2);
	float identifyPath3D(TwoPin *twopin, int ***back3D, float ***dp3D, int ***back3D2, float ***dp3D2);
	void getHorVerOverflow3D(vector<RPoint> &path, int &horOF, int &verOF, float &horCost, float &verCost);
	void snakingRouting3D(TwoPin *two, BoundingBox &br, int ***back3D, float ***dp3D, bool horiSnake);
	void snakingRoutingVer3D(Point& a, Point& b, BoundingBox &br, int ***back3D, float ***dp3D);
	void snakingRoutingHor3D(Point& a, Point& b, BoundingBox &br, int ***back3D, float ***dp3D);

	/// inline functions
	inline void update2DmapCost(int x, int y, int hori);
	inline void update3DmapCost(int x, int y, int z, int hori);
	inline int changeIndex(int x, int y, int z);
	inline int changeIndex2(int x, int y, int z);
	inline void minu_func(bool route2D, TwoPin *two);
	inline void add_func(bool route2D, TwoPin *two);
	inline void reOrder(const Point& from, const Point& to, Point& a, Point& b);
	inline void setAdd(GridCell *tmp);
	inline GridCell* setFind(GridCell *findCell);
	inline void setUnion(GridCell *xRoot, GridCell *yRoot);
	// the DATASTRUCTURE of data is GCELL 
	//inline void insertQueues( struct fibque *myque, struct fibheap *myheap, void *data );
	//inline void replaceQueues( struct fibque *myque, struct fibheap *myheap, GridCell *toCell, float key );
	//inline void* extractQueues( struct fibque *myque, struct fibheap *myheap );
	inline void insertQueues(GridCell *toCell);
	inline void replaceQueues(GridCell *toCell, float key);
	inline GridCell* extractQueues();
	inline int getCost(int x, int y, bool hori);
	inline int cost2(int demand, int capacity);
	inline float cost3D(int x, int y, int z, int hori);
	inline float DPCost(int x, int y, int hori);
	inline float DPCost(int x, int y, bool hori, Net& net);
	inline float PatternCost(int x, int y, int hori, Net& net);
	inline float cost(int demand, int capacity);
	inline float NewCost(int x, int y, int hori);
	inline float NewCost(int x, int y, int hori, Net& net);
	inline int getxNum();
	inline int getyNum();
	inline void RipUp(vector<RPoint>& n, Net& net);
	inline int RoutePath(vector<RPoint>& n, Net& net);
	inline float get2DRoutingCost(int x, int y, int hori);
	inline float get2DRoutingCostTrans(int x, int y, int hori);
	inline void set2DRoutingCost(int x, int y, int hori, float value);
	inline int getIndex(int x, int y, int lay);
	/// inline functions (end)

};

/// static functions
static bool heap_cp(GridCell * a, GridCell *b);
/// static functions (end)

#endif // #ifndef GLOBALROUTER_H

