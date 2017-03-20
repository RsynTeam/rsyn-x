
//#ifndef INTERFACE_H
//#define INTERFACE_H


#include <boost/timer.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <queue>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <algorithm>

#include "GlobalRouter.h"
#include "NetDB.h"
#include "Timer.h"
//#include "LA.h"

//#define VIA_AWARE_RATIO 1.5
#define VIA_AWARE_RATIO 1
#define SHOW_INFO

using namespace std ;

// for ISPD14 placement contest
int main_congestion_estimator(NetDB &netdb, ParamSet &par, GlobalRouter &router, const char *outFile);
void settingAndChecking(NetDB& netdb, ParamSet &par, bool showWarning);
void setDefaultParameter(ParamSet &par, bool useFlute);
void clearRoutingResult(NetDB &netdb);
int getOverflowOfNet( GlobalRouter &router, Net& n);
vector<int> getPassingNet(GlobalRouter &router, vector<Net> &nets, int x, int y, bool hori);
int getGapOfDemAndCap( GlobalRouter &router, int x, int y, bool hori);
int  getWLofNet( Net&  n);

// not for contest
void output_ISPD_GUI(const char *gui,  GlobalRouter& router,  NetDB& netdb );
void getNetsOfEachEdge( vector< vector< vector<int> > > &NetID_horEdge, vector< vector< vector<int> > > &NetID_verEdge, NetDB &netdb);
int main_ispd( NetDB &netdb, ParamSet &par, const char *outFile, GlobalRouter &router, controlGR &control);
int main_ropt(NetDB &netdb, ParamSet &par, const char *outFile, GlobalRouter &router, controlGR &control, bool flute, bool doLA);
int main_legalizer(NetDB &netdb, ParamSet &par, const char *outFile, GlobalRouter &router, controlGR &control);
int main_get_two_pin(NetDB &netdb, ParamSet &par, const char *outFile, GlobalRouter &router);
void look_LOG(const char *outFile, NetDB &netDB, GlobalRouter &router);
void report_HPWL(NetDB &netDB);

/*
void update_local_congestion(vector< vector<int> > &numLocalPin, vector< vector<int> > &numGpin, double pinFactor);

int main_congestion_estimator(NetDB &netdb, ParamSet &par, GlobalRouter &router  );
void Input_DAC(const char *input_ciruit_file, const char *input_place_file, NetDB& netdb, ParamSet &par);
void Input_ISPD( const char* filename, NetDB& netdb, ParamSet &par );
void Input_Setting( const char* filename, NetDB& netdb, ParamSet &par );
void setDefaultParameter(NetDB& netdb, ParamSet &par);
void outputFile(const char *outFile, NetDB &netDB);
*/

//#endif
