/* Copyright 2014-2018 Rsyn
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
 *  global.h
 *  sizer
 *
 *  Created by Tiago Reimann on 11/01/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <string>

#include "io/parser/parser_helper.h"

using namespace std;

namespace ISPD13 {

class comp_cellLeakage {  // make heap
       public:
        bool operator()(const LibParserCellInfo c1,
                        const LibParserCellInfo c2) const {
                if ((c2.leakagePower > c1.leakagePower) ||
                    ((c2.leakagePower == c1.leakagePower) &&
                     (c2.pins[0].maxCapacitance < c1.pins[0].maxCapacitance)))
                        return true;
                else
                        return false;
        }
};

class OrdCells {
       public:
        string footprint;
        double minimumFallSlew;  // minimum slew value for each footprint (type
                                 // of cell)
        double minimumRiseSlew;

        double maximumFallSlew;  // maximum slew value for each footprint (type
                                 // of cell)
        double maximumRiseSlew;

        vector<LibParserCellInfo> cells;
};

class OrgCells {
       public:
        vector<OrdCells> oCells;

        LibParserCellInfo *findCellInst(string instType);
};

class Wire {
       public:
        string wire_name;
        double cap;

        bool operator<(const Wire &w1) const {
                return (wire_name > w1.wire_name);
        }

        bool operator==(const Wire &w1) const {
                return (wire_name == w1.wire_name);
        }

        bool operator>(const Wire &w1) const {
                return (wire_name < w1.wire_name);
        }

        bool operator!=(const Wire &w1) const {
                return (wire_name != w1.wire_name);
        }
};

class InputDelay {
       public:
        string port_name;
        double delay;
};

class OutputDelay {
       public:
        string port_name;
        double delay;
};

class InputDriver {
       public:
        string port_name;
        string driver;
        double rise, fall;
};

class OutputLoad {
       public:
        string port_name;
        double load;
};

class SDCInfo {
       private:
        template <typename T>
        static int search(const vector<T> &v, const string &portName) {
                for (int i = 0; i < v.size(); i++) {
                        if (v[i].port_name == portName) return i;
                }  // end for
                return -1;
        }  // end method

       public:
        string clk_name;
        string clk_port;
        double clk_period;
        vector<InputDelay> input_delays;
        vector<InputDriver> input_drivers;
        vector<OutputDelay> output_delays;
        vector<OutputLoad> output_loads;

        int searchInputDriver(const string &portName) const {
                return search(input_drivers, portName);
        }

        int searchInputDelay(const string &portName) const {
                return search(input_delays, portName);
        }

        int searchOutputLoad(const string &portName) const {
                return search(output_loads, portName);
        }

        int searchOutputDelay(const string &portName) const {
                return search(output_delays, portName);
        }
};

class Net {
       public:
        string netName;

        int numPins;

        bool operator<(const Net &w1) const { return (netName < w1.netName); }

        bool operator==(const Net &w1) const { return (netName == w1.netName); }

        bool operator>(const Net &w1) const { return (netName > w1.netName); }

        bool operator!=(const Net &w1) const { return (netName != w1.netName); }
};

class AddrCell {
       public:
        string instName;
        int vectorIndex;

        AddrCell() : vectorIndex(0) {}

        bool operator<(const AddrCell &w1) const {
                return (instName < w1.instName);
        }

        bool operator==(const AddrCell &w1) const {
                return (instName == w1.instName);
        }

        bool operator>(const AddrCell &w1) const {
                return (instName > w1.instName);
        }

        bool operator!=(const AddrCell &w1) const {
                return (instName != w1.instName);
        }
};

// -----------------------------------------------------------------------------

//#define debug(tag, x) cout << x

// class App {
// private:
//	typedef map< string, set<string> > OptionMap;
//	static OptionMap clsOptions;
//
// public:
//
//	// Returns the number of occurrence of the option specified by key.
//	static int hasOption( const string &key ) {
//		return clsOptions.count(key);
//	} // end method
//
//	// Returns true if there are an option specified by key, and one of its
//	// values matches value.
//	static bool hasOptionValuePair( const string &key, const string &value )
//{
//		OptionMap::iterator it = clsOptions.find(key);
//		if ( it != clsOptions.end() )
//			return it->second.count(value);
//		else
//			return false;
//	} // end method
//
//	// Return the ith value defined for an option specified by key. If no
// option
//	// exists or an invalid index is used, an empty string is returned.
//	static string getOptionValue( const string &key, const int index = 0 ) {
//		OptionMap::iterator it = clsOptions.find(key);
//		if ( it != clsOptions.end() ) {
//			set<string> &optionParameters = it->second;
//
//			int counter = 0;
//			for (set<string >::iterator its =
// optionParameters.begin();
// its!=optionParameters.end(); ++its) {
//				if ( counter++ == index )
//					return *its;
//			} // end for
//		} // end if
//		return "";
//	} // end method
//
//	// Parse the command line arguments.
//	static void parseCommandLineArguments( const int argc, char ** argv, int
// offset ) {
//		string currentOption;
//
//		for ( int r=offset; r < argc; r++ ) {
//			const string arg( argv[r] );
//
//			if ( arg[0] == '-' ) {
//				currentOption = arg.substr(1);
//
//				OptionMap::iterator it =
// clsOptions.find(currentOption);
//				if ( it == clsOptions.end() )
//					clsOptions.insert(
// make_pair(currentOption,
// set<string>() ));
//			} else {
//				OptionMap::iterator it =
// clsOptions.find(currentOption);
//				if ( it != clsOptions.end() )
//					it->second.insert(arg);
//			} // end else
//		} // end for
//	} // end method
//
//
//	// Print options.
//	static void printCommandLineArguments( ostream &out ) {
//		out << "Command line arguments:\n";
//		for (OptionMap::iterator it = clsOptions.begin();
// it!=clsOptions.end(); ++it) {
//			out << "\t" << it->first << "\t[";
//
//			bool first = true;
//			set<string> &optionParameters = it->second;
//			for (set<string >::iterator its =
// optionParameters.begin();
// its!=optionParameters.end(); ++its) {
//				if ( !first )
//					out << " ";
//				out << (*its);
//			} // end for
//			out << "]\n";
//		} // end for
//	} // end method
//}; // end class
//
// extern App app;

inline double lookup(const LibParserLUT &lut, const double x, const double y) {
        double weightX, weightY;
        double xLower, xUpper, yLower, yUpper;
        int xLowerIndex, xUpperIndex, yLowerIndex, yUpperIndex, xLimit, yLimit;

        xLowerIndex = xUpperIndex = yLowerIndex = yUpperIndex = 0;
        xLimit = lut.loadIndices.size() - 2;
        yLimit = lut.transitionIndices.size() - 2;

        // no loads viol. are accepted -> not anymore
        while ((xLowerIndex < xLimit) &&
               (lut.loadIndices[xLowerIndex + 1] <= x))
                ++xLowerIndex;
        xUpperIndex = min(xLimit + 1, xLowerIndex + 1);

        while ((yLowerIndex < yLimit) &&
               (lut.transitionIndices[yLowerIndex + 1] <= y))
                ++yLowerIndex;
        yUpperIndex = min(yLimit + 1, yLowerIndex + 1);

        xLower = lut.loadIndices[xLowerIndex];
        xUpper = lut.loadIndices[xUpperIndex];
        yLower = lut.transitionIndices[yLowerIndex];
        yUpper = lut.transitionIndices[yUpperIndex];

        weightX = (x - xLower) / (xUpper - xLower);
        weightY = (y - yLower) / (yUpper - yLower);

        double result;
        result = (1.0 - weightX) * (1.0 - weightY) *
                 (lut.tableVals(xLowerIndex, yLowerIndex));
        result += (weightX) * (1.0 - weightY) *
                  (lut.tableVals(xUpperIndex, yLowerIndex));
        result += (1.0 - weightX) * (weightY) *
                  (lut.tableVals(xLowerIndex, yUpperIndex));
        result +=
            (weightX) * (weightY) * (lut.tableVals(xUpperIndex, yUpperIndex));

        return result;
}  // end method

class SPEFInfo {
        map<string, int> SPEFNetMap;
        vector<SpefNet> SPEFNets;

       public:
        void clear() {
                SPEFNets.clear();
                SPEFNetMap.clear();
        };  // end method

        const SpefNet &getNet(const int i) const { return SPEFNets[i]; };

        int getSize() const { return SPEFNets.size(); };

        void addNet(const SpefNet &net) {
                SPEFNetMap.insert(make_pair(net.netName, SPEFNets.size()));
                SPEFNets.push_back(net);
        };

        int search(const string &netName) const {
                map<string, int>::const_iterator it = SPEFNetMap.find(netName);
                if (it != SPEFNetMap.end())
                        return it->second;
                else
                        return -1;
                //            const int numNets = SPEFNets.size();
                //            for (int i = 0; i < numNets; i++) {
                //                if (SPEFNets[i].netName == netName)
                //                    return i;
                //            } // end for
                //            return -1;
        };  // end method
};

class LIBInfo {
       public:
        double default_max_transition;
        vector<LibParserCellInfo> libCells;

        void clear() { libCells.clear(); };
};

class TimingStruct {
       public:
        TimingStruct(string port, double earlyR, double earlyF, double lateR,
                     double lateF) {
                portName = port;
                earlyRise = earlyR;
                earlyFall = earlyF;
                lateRise = lateR;
                lateFall = lateF;
        };

        TimingStruct() {
                portName = "";
                earlyRise = earlyFall = lateRise = lateFall = -1.0;
        };
        string portName;
        double earlyRise;
        double earlyFall;
        double lateRise;
        double lateFall;
};

class CommandStruct {
       public:
        CommandStruct(string pin, string net, string file, string gate,
                      string cell, CommandTypeEnum cmdType) {
                netName = net;
                pinName = pin;
                fileName = file;
                gateName = gate;
                cellType = cell;
                type = cmdType;
        };

        CommandStruct() {
                netName = fileName = pinName = cellType = "";
                type = UNKNOWN;
        };
        CommandTypeEnum type;
        string netName;
        string pinName;
        string fileName;
        string gateName;
        string cellType;
};

class QueryStruct {
       public:
        QueryStruct(string pin, bool isEarly, bool isRise, int numPaths,
                    QueryTypeEnum qType) {
                pinName = pin;
                early = isEarly;
                rise = isRise;
                paths = numPaths;
                type = qType;
        };

        QueryStruct() {
                pinName = "";
                early = rise = true;
                paths = 1;
                type = NOT_VALID;
        };
        QueryTypeEnum type;
        string pinName;
        int paths;
        bool early;
        bool rise;
};

class TimingCap {
       public:
        TimingCap(string port, double capacitance) {
                portName = port;
                cap = capacitance;
        }
        string portName;
        double cap;
};

class TimingInfo {
        template <typename T>
        static int search(const vector<T> &v, const string &portName) {
                for (int i = 0; i < v.size(); i++) {
                        if (v[i].portName == portName) return i;
                }  // end for
                return -1;
        }  // end method

       public:
        void clear() {
                ats.clear();
                slews.clear();
                rats.clear();
                caps.clear();
        };  // end method

        string clockPort;
        double clockPeriod;
        double clockLow;
        vector<TimingStruct> ats;
        vector<TimingStruct> slews;
        vector<TimingStruct> rats;
        vector<TimingCap> caps;

        int searchInputDelay(const string &portName) const {
                return search(ats, portName);
        }

        int searchOutputLoad(const string &portName) const {
                return search(caps, portName);
        }

        int searchOutputRAT(const string &portName) const {
                return search(rats, portName);
        }

        int searchInputSlew(const string &portName) const {
                return search(slews, portName);
        }
};

struct OrderInfo {
        OpsType type;
        int index;

        OrderInfo() {
                type = NONE;
                index = -1;
        };
};

class OpsInfo {
        template <typename T>
        static int search(const vector<T> &v, const string &portName) {
                for (int i = 0; i < v.size(); i++) {
                        if (v[i].portName == portName) return i;
                }  // end for
                return -1;
        }  // end method

       public:
        vector<OrderInfo> order;

        void clear() {
                queries.clear();
                cmds.clear();
        };  // end method

        vector<CommandStruct> cmds;
        vector<QueryStruct> queries;
};

}  // end namespace

#endif  //_GLOBAL_H_
