#ifndef PARSER_H
#define PARSER_H
	#include <iostream>
	#include <vector>
	#include <set>
     #include"Timer.h"
     #include"NetDB.h"
	#define DISPLAY_SCREEN true

	using namespace std ;

	void Input_DAC(const char *input_ciruit_file, NetDB& netdb, ParamSet &par);
	void Input_ISPD( const char* filename, NetDB& netdb, ParamSet &par);
	void Input_Setting( const char* filename, NetDB& netdb, ParamSet &pa);
	void outputFile(const char *outFile, NetDB &netDB);
	void outputFile(const char *outFile, NetDB &netDB);
	void outputLEFDEF(const char *outFile, const char *designFile, NetDB &netDB);
	void outputTrackBench(const char *outFile, NetDB &netDB);
	void outputTwoPin(const char *outFile, NetDB &db);

	void build_netlist_set(NetDB& netdb, ParamSet &par, vector<placeCell> &cellArray);
     void read_place_file(const char *placeFile, NetDB &netDB);
	void outputCell(const char *outFile, NetDB &netDB);

#endif
