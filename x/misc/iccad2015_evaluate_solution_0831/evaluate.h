/*--------------------------------------------------------------------------------------------------------*/
/*  Desc:     Functions to analyze placement density, measure (scaled) total_StWL, displacement, timing   */
/*                                                                                                        */
/*  Authors:  Myung-Chul Kim, IBM Coporation (mckima@us.ibm.com)                                          */
/*            Jin Hu, IBM Coporation (jinhu@us.ibm.com)                                                   */
/*                                                                                                        */
/*  Created:  03/12/2015                                                                                  */
/*--------------------------------------------------------------------------------------------------------*/

#ifndef EVALUATE_H
#define EVALUATE_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <climits>
#include <algorithm>
#include <limits>
#include <assert.h>

#include "uit_timer.h"
#include "Flute/flute.h"

/* density profiling related parms */
#define BIN_DIM              9.0
#define ALPHA                1.0
#define BIN_AREA_THRESHOLD   0.2
#define FREE_SPACE_THRESHOLD 0.2

#define ABU2_WGT 10
#define ABU5_WGT  4
#define ABU10_WGT 2
#define ABU20_WGT 1

#define INIT false
#define FINAL true

#define PI_PIN 1
#define PO_PIN 2
#define NONPIO_PIN 3

using namespace std;

struct site
{
  string name;
  double width;             /* in microns */
  double height;            /* in microns */
  string type;              /* equivalent to class, I/O pad or CORE */
  vector<string> symmetries;        /* {X | Y | R90} */

  site() : name(""), width(0.0), height(0.0), type("") {}
  site(const site &s) : name(s.name), width(s.width), height(s.height), type(s.type), symmetries(s.symmetries) {}
  void print();
};

struct layer
{
  string name;
  string type;
  string direction;
  double xPitch;           /* in microns */
  double yPitch;           /* in microns */
  double xOffset;          /* in microns */
  double yOffset;          /* in microns */
  double width;            /* in microns */

  layer() : name(""), type(""), direction(""), 
            xPitch(0.0), yPitch(0.0),
            xOffset(0.0), yOffset(0.0), width(0.0) {}
  void print();
};

struct macro_pin
{
  string direction;                      
  unsigned layer;
  double xLL, yLL;         /* in microns */
  double xUR, yUR;         /* in microns */

	macro_pin() : direction(""), layer(0),
				      	xLL(numeric_limits<double>::max()), yLL(numeric_limits<double>::max()), 
								xUR(numeric_limits<double>::min()), yUR(numeric_limits<double>::min()) {}
};

struct rect
{
	double xLL, yLL;
	double xUR, yUR;
};

struct macro
{
  string name;
  string type;                               /* equivalent to class, I/O pad or CORE */
	bool isFlop;                               /* clocked element or not */
  double xOrig;                              /* in microns */
  double yOrig;                              /* in microns */
  double width;                              /* in microns */
  double height;                             /* in microns */
  vector<unsigned> sites;
	map<string, macro_pin> pins;
	vector<rect> obses;                        /* keyword OBS for non-rectangular shapes in micros */

  macro() : name(""), type(""), isFlop(false), xOrig(0.0), yOrig(0.0), width(0.0), height(0.0) {}
  void print();
};

struct pin
{
	// from verilog
  string name;                         /* Name of pins : instance name + "_" + port_name */
  unsigned id;
  unsigned owner;                      /* The owners of PIs or POs are UINT_MAX */
  unsigned net;
	unsigned type;                       /* 1=PI_PIN, 2=PO_PIN, 3=others */
	bool isFlopInput;                    /* is this pin an input  of a clocked element? */
	bool isFlopCkPort;

	// from .sdc
  double cap;                          /* PO load if exists (in Farad) */
	double delay;                        /* PI/PO input/output delay if exists (in sec) */
  double rTran, fTran;                 /* input rise/fall transition if exits (in sec) */
	int driverType;                      /* input driver for PIs */

	// from .def
  double x_coord, y_coord;             /* (in DBU) */
  double x_offset, y_offset;           /* COG of VIA relative to the origin of a cell, (in DBU) */
  bool isFixed;                        /* is this node fixed? */

	// from timer
	double earlySlk, lateSlk;

  pin() : name(""),
          id(numeric_limits<unsigned>::max()), 
					owner(numeric_limits<unsigned>::max()),
          net(numeric_limits<unsigned>::max()), 
          type(numeric_limits<unsigned>::max()), 
					isFlopInput(false), isFlopCkPort(false),
					cap(0.0), delay(0.0), rTran(0.0), fTran(0.0), driverType(numeric_limits<unsigned>::max()),
					x_coord(0.0), y_coord(0.0), x_offset(0.0), y_offset(0.0), 
					isFixed(false), earlySlk(0.0), lateSlk(0.0) {}
  void print();
};

struct cell
{
  string name;
	unsigned type;                              /* index to some predefined macro */
  int x_coord, y_coord;                       /* (in DBU) */  
  int init_x_coord, init_y_coord;             /* (in DBU) */
	double width, height;                       /* (in DBU) */
  bool isFixed;                               /* fixed cell or not */
	bool isLCB;
  map<string, unsigned> ports;                /* <port name, index to the pin> */
  string cellorient;

  cell () : name(""), type(numeric_limits<unsigned>::max()), 
	          x_coord(0), y_coord(0), init_x_coord(0), init_y_coord(0), 
						width(0.0), height(0.0),
            isFixed(false), isLCB(false), cellorient("") {}
  void print();
};

struct net
{
  string name;
  unsigned source;             /* input pin index to the net */
	double lumped_cap;           /* total lump sum cap for the net*/
  vector<unsigned> sinks;      /* sink pins indices of the net */
  vector< pair< pair <string, string>, double > > wire_segs;   /* connecting pin (source, sink) names & length */
  double stwl; // Added by Jucemar

  net() : name(""), source(numeric_limits<unsigned>::max()), lumped_cap(0.0), stwl(0.0) {}
  void print();
};

struct row
{
  /* from DEF file */
  string name;
  unsigned site;
  int origX;               /* (in DBU) */
  int origY;               /* (in DBU) */
  int stepX;               /* (in DBU) */
  int stepY;               /* (in DBU) */
  int numSites;
  string siteorient;

  row() : name(""), site(numeric_limits<unsigned>::max()), 
	        origX(0), origY(0), stepX(0), stepY(0), numSites(0), siteorient("") {}
  void print();
};

struct density_bin
{
  double lx, hx;              /* low/high x coordinate */
  double ly, hy;              /* low/high y coordinate */
  double area;                /* bin area */
  double m_util;              /* bin's movable cell area */
  double f_util;              /* bin's fixed cell area */
  double free_space;          /* bin's freespace area */
};

class circuit
{
  private:
		string directory, dot_verilog;
		string dot_lib_early, dot_lib_late;
    map<string, unsigned> macro2id; /* map between macro name and ID */
    map<string, unsigned> cell2id;  /* map between cell  name and ID */
    map<string, unsigned> pin2id;   /* map between pin   name and ID */
    map<string, unsigned> net2id;   /* map between net   name and ID */
    map<string, unsigned> row2id;   /* map between row   name and ID */
    map<string, unsigned> site2id;  /* map between site  name and ID */
    map<string, unsigned> layer2id; /* map between layer name and ID */

		unsigned num_fixed_nodes; 
    double total_mArea;             /* total movable cell area */
    double total_fArea;             /* total fixed cell area (excluding terminal NIs) */
    double designArea;              /* total placeable area (excluding row blockages) */
    double rowHeight;
    double lx, rx, by, ty;          /* placement image's left/right/bottom/top end coordintes */

    // used for SDC assertions
    string clock_name;
    string clock_port;

    double LOCAL_WIRE_CAP_PER_MICRON,  LOCAL_WIRE_RES_PER_MICRON;        /* Ohm & Farad per micro meter */
		double MAX_WIRE_SEGMENT_IN_MICRON;                                   /* in micro meter  */
		unsigned MAX_LCB_FANOUTS;

    // used for LEF file
    string LEFVersion;
    string LEFNamesCaseSensitive;
    string LEFDelimiter;
    string LEFBusCharacters;
    unsigned LEFdist2Microns;
    double LEFManufacturingGrid;

    // used for DEF file
    string DEFVersion;
    string DEFDelimiter;
    string DEFBusCharacters;
    string design_name;
    unsigned DEFdist2Microns;
    vector< pair<unsigned,unsigned> > dieArea;

    vector<site> sites;            /* site list */
    vector<layer> layers;          /* layer list */
    vector<macro> macros;          /* macro list */
    vector<cell> cells;            /* cell list */
    vector<net> nets;              /* net list */
    vector<pin> pins;              /* pin list */
    vector<row> rows;              /* row list */

    vector<unsigned> PIs;          /* PI pin list (by id) */
    vector<unsigned> POs;          /* PO pin list (by id) */

   /* locateOrCreate helper functions */
    macro* locateOrCreateMacro(const string &macroName);
    cell*  locateOrCreateCell(const string &cellName);
    net*   locateOrCreateNet(const string &netName);
    pin*   locateOrCreatePin(const string &pinName);
    row*   locateOrCreateRow(const string &rowName);
    site*  locateOrCreateSite(const string &siteName);
    layer* locateOrCreateLayer(const string &layerName);

    /* IO helper for verilog */
    bool read_module(ifstream &is, string &moduleName);
    bool read_primary_input(ifstream &is, string &primaryInput);
    bool read_primary_output(ifstream &is, string &primaryOutput);
    bool read_wire(ifstream &is, string &wire);
    bool read_cell_inst(ifstream &is, string &cellType, string &cellInstName, 
                        vector<pair<string, string> > &pinNetPairs);

    /* IO helper for SDC */
    bool read_clock(ifstream &is, string &clockName, string &clockPort, double &period);
    bool read_input_delay(ifstream &is, string &portName, double &delay);
    bool read_output_delay(ifstream &is, string &portName, double &delay);
    bool read_driver_info(ifstream &is, string &inPortName, string &driverType, string &driverPin,
                          double &inputTransitionFall, double &inputTransitionRise);
    bool read_output_load(ifstream &is, string &outPortName, double &load);

    /* IO helpers for LEF */
    void read_lef_site(ifstream &is);
    void read_lef_layer(ifstream &is);
    void read_lef_macro(ifstream &is);
    void read_lef_macro_site(ifstream &is, macro* myMacro);
    void read_lef_macro_pin(ifstream &is, macro* myMacro);

    /* IO helpers for DEF */
    void read_init_def_components(ifstream &is);
    void read_final_def_components(ifstream &is);
    void read_def_pins(ifstream &is);
    void read_def_nets(ifstream &is);

		/* for timing evaluation */
    void update_pinlocs();
    void build_steiner();
    void slice_longwires(unsigned threshold);
		void write_tau2015(const string &output);
		void write_incremental_spef(const string &output, vector<net> &partial_nets);
		void update_timer_FF2LCB_association(uit::Timer *timer);
		void write_timing(const string &output);
		
	    /* auxiliaries */
    bool findFirstPin(const macro * const libCell, const string &direction, string &pinName) {
      for (map<string, macro_pin>::const_iterator it = libCell->pins.begin(); it != libCell->pins.end(); it++) {
        if (it->second.direction == direction) {
          pinName = it->first;
          return true;
        } // end if
      } // end for
      return false;
    } // end method  

  public:
    circuit(): num_fixed_nodes(0), 
		           LOCAL_WIRE_CAP_PER_MICRON(0.20e-15), LOCAL_WIRE_RES_PER_MICRON(0.60), 
	             MAX_WIRE_SEGMENT_IN_MICRON(20.0), MAX_LCB_FANOUTS(40),
							 DEFVersion(""), DEFDelimiter("/"), DEFBusCharacters("[]"), 
               design_name(""), DEFdist2Microns(0), total_HPWL(1e8), total_StWL(1e8), 
               ABU_penalty(100.0), displacement(0.0) {}

    /* primary IO functions */
		void read_parameters(const char* input);
    void read_iccad2015_file(const char* input);
    void read_verilog(const string &input);
    void read_sdc(const string &input);
    void read_lef(const string &input);
    void read_def(const string &input, bool init_or_final);
		void read_ops(const string &input);
		void copy_init_to_final();

    void measure_HPWL();
    void measure_ABU(double bin_dim, double targUt);
    void measure_displacement();
    bool measure_timing();

    void print();
		void calc_design_area_stats();
		bool check_legality();
		void sort_and_slice_objects();

		/* benchmark generation */
    string benchmark;               /* benchmark name */
		string final_ops;
		void write_bookshelf();
		void convert_pl_to_def(const string thePlacer, const string Identifier);

    /* evaluation related variables */
    double clock_period;            /* (in sec) */
    double total_HPWL, total_StWL, ABU_penalty, displacement;
    double eWNS, eTNS;  // early
    double lWNS, lTNS;  // late
};

bool is_special_char(char c);
bool read_line_as_tokens(istream &is, vector<string> &tokens);
void get_next_token(ifstream &is, string &token, const char* beginComment);
void get_next_n_tokens(ifstream &is, vector<string> &tokens, const unsigned n, const char* beginComment);
#endif

