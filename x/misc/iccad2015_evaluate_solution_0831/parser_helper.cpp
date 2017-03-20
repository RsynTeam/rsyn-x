/*---------------------------------------------------------------------------------------------*/
/*  Desc:     Parser Helper for ICCAD 2015 format (verilog, sdc, LEF/DEF)                      */
/*                                                                                             */
/*  Authors:  Myung-Chul Kim, IBM Corporation (mckima@us.ibm.com)                              */
/*            Jin Hu, IBM Corporation (jinhu@us.ibm.com)                                       */
/*                                                                                             */
/*  Created:  04/29/2015                                                                       */
/*---------------------------------------------------------------------------------------------*/

#include "evaluate.h"
#define _DEBUG

const char* DEFCommentChar    = "#";
const char* DEFLineEndingChar = ";";
const char* LEFCommentChar    = "#";
const char* LEFLineEndingChar = ";";
const char* FFClkPortName = "ck";

inline bool operator<(const row &a, const row &b)
{
	return (a.origY < b.origY) || (a.origY == b.origY && a.origX < b.origX);
}

void circuit::read_parameters(const char* input)
{
  cout << endl << "Reading parameter file .." <<endl;
  cout << "-------------------------------------------------------------------------------" <<endl;
  ifstream dot_parm(input);
  if (!dot_parm.good())
	{
    cerr << "read_parameters:: cannot open `" << input << "' for reading." << endl;
		cerr << "the script will use the default parameters for evaluation." <<endl;
	}
	else
	{
		string tmpStr;
		dot_parm >> tmpStr;
		while(!dot_parm.eof())
		{
			if(tmpStr == "LOCAL_WIRE_CAPACITANCE_PER_MICRON")
				dot_parm >> LOCAL_WIRE_CAP_PER_MICRON;
			else if(tmpStr == "LOCAL_WIRE_RESISTANCE_PER_MICRON")
				dot_parm >> LOCAL_WIRE_RES_PER_MICRON;
			else if(tmpStr == "MAX_WIRE_SEGMENT_LENGTH_IN_MICRON")
				dot_parm >> MAX_WIRE_SEGMENT_IN_MICRON;
			else if(tmpStr == "MAX_LCB_FANOUTS")
				dot_parm >> MAX_LCB_FANOUTS;
			else {}
			dot_parm >> tmpStr;
		}
	}
	dot_parm.close();
	cout <<  "  LOCAL_WIRE_RESISTANCE   : " << LOCAL_WIRE_RES_PER_MICRON  << " Ohm/um"   << endl;
	cout <<  "  LOCAL_WIRE_CAPACITANCE  : " << LOCAL_WIRE_CAP_PER_MICRON  << " Farad/um" << endl;
	cout <<  "  MAX_WIRE_SEGMENT_LENGTH : " << MAX_WIRE_SEGMENT_IN_MICRON << " um"       << endl;
	cout <<  "  MAX_LCB_FANOUTS         : " << MAX_LCB_FANOUTS            << endl;
  cout << "-------------------------------------------------------------------------------" <<endl;
	return;
}

void circuit::read_iccad2015_file(const char* input)
{
  cout << endl << "Reading .iccad2015 file .." <<endl;
	benchmark.assign(input);
  size_t found=benchmark.find_last_of("/\\");
  if(found==benchmark.npos)
  {
    directory="./";
    benchmark=benchmark.substr(0, benchmark.find_last_of("."));
  }
  else
  {
    directory=benchmark.substr(0,found);
    directory+="/";
    benchmark=benchmark.substr(found+1, benchmark.find_last_of(".")-(found+1));
  }
  cout << "-------------------------------------------------------------------------------" <<endl;
  cout << "  benchmark name  : "<< benchmark <<endl;
  cout << "  directory       : "<< directory <<endl;
  cout << "  .iccad2015 file : "<< input <<endl;
  cout << "-------------------------------------------------------------------------------" <<endl;
	
  ifstream dot_iccad2015(input);
  if (!dot_iccad2015.good())
  {
    cerr << "read_iccad2015_file:: cannot open `" << input << "' for reading." << endl;
    exit(1);
  }

	string dot_sdc, dot_lef, dot_def;
  string filename;
	dot_iccad2015 >> filename;
	while (!dot_iccad2015.eof())
  {
      string ext = filename.substr(filename.find_last_of("."));
      if (ext == ".v")
				dot_verilog = directory+filename;
      else if (ext == ".sdc")
				dot_sdc = directory+filename;
      else if (ext == ".lef")
        dot_lef = directory+filename;
      else if (ext == ".def")
        dot_def = directory+filename;
      else if (ext == ".lib")
			{
				if(filename.find("_Early")!=string::npos)
	        dot_lib_early = directory+filename;
				else if(filename.find("_Late")!=string::npos)
	        dot_lib_late = directory+filename;
			}
      else
      {
        cerr << "read_iccad2015_file:: unsupported filetype is specified : " << filename << endl;
        exit(1);
      }
			dot_iccad2015 >> filename;
  }
  dot_iccad2015.close();
  cout << "  directory       : "<< directory <<endl;
  cout << "  early .lib file : "<< dot_lib_early << endl; 
  cout << "  late  .lib file : "<< dot_lib_late << endl;

	read_lef(dot_lef);
	read_verilog(dot_verilog);
	read_sdc(dot_sdc);
	read_def(dot_def, INIT);
	calc_design_area_stats();
	return;
}

void circuit::calc_design_area_stats()
{
	num_fixed_nodes=0;
	total_mArea=total_fArea=designArea=0.0;
  for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(theCell->isFixed)
		{
			total_fArea += theCell->width * theCell->height;
			num_fixed_nodes++;
		}
		else
			total_mArea += theCell->width * theCell->height;
	}

	for(vector<row>::iterator theRow=rows.begin() ; theRow != rows.end() ; ++theRow)
		designArea += theRow->stepX * theRow->numSites * sites[ theRow->site ].height * static_cast<double>(LEFdist2Microns);

	rowHeight = sites[ site2id["core"] ].height * static_cast<double>(LEFdist2Microns);

  cout << "-------------------------------------------------------------------------------" <<endl;
  cout <<"  total cells     : " << cells.size() <<endl;
  cout <<"  total nets      : " << nets.size() <<endl;;
  cout <<"  design area     : " << designArea <<endl;
  cout <<"  total f_area    : " << total_fArea << endl;
  cout <<"  total m_area    : " << total_mArea << endl;
  if(designArea-total_fArea > 1.0e-5)
    cout <<"  design util     : " << total_mArea/(designArea-total_fArea) <<endl;
  cout <<"  num rows        : " << rows.size() <<endl;
  cout <<"  row height      : " << rowHeight <<endl;
  cout << "--------------------------------------------------------------------------------" <<endl;
	return;
}

void circuit::read_verilog(const string &input)
{
  cout << "  .v file         : "<< input <<endl;
  ifstream dot_verilog(input.c_str());
  if (!dot_verilog.good())
  {
    cerr << "read_verilog:: cannot open `" << input << "' for reading" << endl;
    exit(1);
  }

  cell* myCell;
  pin*  myPin;
  net*  myNet;
	macro* myMacro;
	macro_pin* myMacroPin;

	string module;
  bool valid = read_module(dot_verilog, module);
  assert(valid);
#ifdef DEBUG
  cout << "  Module name : "<< module <<endl;
#endif

  map<string, unsigned>::iterator it;
  do
  {
    string primaryInput;
    valid = read_primary_input(dot_verilog, primaryInput);
    if (valid)
    {
#ifdef DEBUG
      cout << "Primary input: " << primaryInput << endl;
#endif
      myPin         = locateOrCreatePin(primaryInput);
      myNet         = locateOrCreateNet(primaryInput);
      myNet->source = myPin->id;
      myPin->net    = net2id[myNet->name];
			myPin->type   = PI_PIN;
      PIs.push_back(myPin->id);
    }
  } while (valid);

  do
  {
    string primaryOutput;
    valid = read_primary_output(dot_verilog, primaryOutput);
    if (valid)
    {
#ifdef DEBUG
      cout << "Primary output: " << primaryOutput << endl;
#endif
      myPin        = locateOrCreatePin(primaryOutput);
      myNet        = locateOrCreateNet(primaryOutput);
      myNet->sinks.push_back(myPin->id);
      myPin->net   = net2id[myNet->name];
			myPin->type  = PO_PIN;
      POs.push_back(myPin->id);
    }
  } while (valid);

  do
  {
    string netname;
    valid = read_wire(dot_verilog, netname);
    if (valid)
    {
#ifdef DEBUG
      cout << "Net: " << netname << endl;
#endif
      locateOrCreateNet(netname);
    }
  } while (valid);

#ifdef DEBUG
  cout << "Cell insts: " << endl;
#endif
  do
  {
    string cellType, cellInst;
    vector<pair<string, string> > pinNetPairs;   /* (port name, net name) */
    valid = read_cell_inst(dot_verilog, cellType, cellInst, pinNetPairs);
    if (valid)
    {
#ifdef DEBUG
      cout << cellType << " " << cellInst << " " ;
#endif
      myCell       = locateOrCreateCell(cellInst);
      myCell->type = macro2id[ cellType ];
			myMacro      = &macros[ macro2id[ cellType ] ];
			myCell->width  = myMacro->width  * static_cast<double>(LEFdist2Microns);
			myCell->height = myMacro->height * static_cast<double>(LEFdist2Microns);
      for (unsigned i = 0; i < pinNetPairs.size(); ++i)
      {
#ifdef DEBUG
        cout << "(" << pinNetPairs[i].first << " " << pinNetPairs[i].second << ") ";
#endif
				// NOTE: pin name = cell instance name + ":" + port name e.g., u1:ZN
        string pinName = cellInst + ":" + pinNetPairs[i].first;
        myPin = locateOrCreatePin(pinName);
        myNet = locateOrCreateNet(pinNetPairs[i].second);
        if(myPin->net != numeric_limits<unsigned>::max())
        {
          cout << "ERROR: pin " <<pinNetPairs[i].first << " is driven by/driving multiple nets! exiting.. ";
          exit(2);
        }

        myCell->ports [ pinNetPairs[i].first ] = myPin->id;

        myPin->net   = net2id[myNet->name];
        myPin->owner = cell2id[myCell->name];
				myPin->type  = NONPIO_PIN;
				myPin->isFlopInput = (macros[ cells[myPin->owner].type ].isFlop && 
															macros[ cells[myPin->owner].type ].pins[ pinNetPairs[i].first ].direction == "INPUT");

				myPin->isFlopCkPort = (myPin->isFlopInput && pinNetPairs[i].first == FFClkPortName);

				// NOTE: in this contest, all LCBs are INV_Z80 and connected to iccad_clk
				if(cellType == "INV_Z80" && myNet->name == "iccad_clk")
					myCell->isLCB = true;

				assert(macro2id.find(cellType) != macro2id.end());
				assert(macros[ macro2id[cellType] ].pins.find(pinNetPairs[i].first) != macros[ macro2id[cellType] ].pins.end());

				myMacroPin   = &macros[ macro2id[ cellType ] ].pins[ pinNetPairs[i].first ];

				// NOTE : pin offsets are set to the center of the pin
				myPin->x_offset = 0.5 * (myMacroPin->xLL + myMacroPin->xUR) * static_cast<double>(LEFdist2Microns);
				myPin->y_offset = 0.5 * (myMacroPin->yLL + myMacroPin->yUR) * static_cast<double>(LEFdist2Microns);

        if (myMacroPin->direction != "OUTPUT" )
          myNet->sinks.push_back(myPin->id);
        else
        {
          if(nets[myPin->net].source != numeric_limits<unsigned>::max())
          {
            cout << "ERROR: net "<< myNet->name << " is driven by multiple pins! existing.. ";
            exit(3);
          }
          myNet->source = myPin->id;
        }
      }
    }
  } while (valid);

  dot_verilog.close();
}

bool circuit::read_module(ifstream &is, string &moduleName)
{
  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);

  while (valid)
  {
    if (tokens.size() == 2 && tokens[0] == "module")
    {
      moduleName = tokens[1];
      break;
    }
    valid = read_line_as_tokens(is, tokens);
  }

  // Read and skip the port names in the module definition
  // until we encounter the tokens {"Start", "PIs"}
  while (valid && !(tokens.size() == 2 && tokens[0] == "Start" && tokens[1] == "PIs"))
  {
    valid = read_line_as_tokens(is, tokens);
    assert(valid);
  }

  return valid;
}

bool circuit::read_primary_input(ifstream &is, string &primaryInput)
{
  primaryInput = "";
  
  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);

  assert(valid);
  assert(tokens.size() == 2);

  if (valid && tokens[0] == "input")
  {
    primaryInput = tokens[1];
  }
  else
  {
    assert (tokens[0] == "Start" && tokens[1] == "POs");
    return false;
  }

  return valid; 
}

bool circuit::read_primary_output(ifstream &is, string &primaryOutput)
{
  primaryOutput = "";
  
  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);

  assert(valid);
  assert(tokens.size() == 2);

  if (valid && tokens[0] == "output")
  {
    primaryOutput = tokens[1];
  }
  else
  {
    assert(tokens[0] == "Start" && tokens[1] == "wires");
    return false;
  }

  return valid; 
}

bool circuit::read_wire(ifstream &is, string &wire)
{
  wire = "";

  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);
  assert (valid);
  assert (tokens.size() == 2);

  if (valid && tokens[0] == "wire")
  {
    wire = tokens[1];
  }
  else
  {
    assert (tokens[0] == "Start" && tokens[1] == "cells");
    return false;
  }

  return valid;
}

bool circuit::read_cell_inst(ifstream &is, string &cellType, string &cellInstName,
                             vector<pair<string, string> > &pinNetPairs)
{
  cellType     = "";
  cellInstName = "";
  pinNetPairs.clear();

  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);
  assert (valid);

  if (tokens.size() == 1)
  {
    assert (tokens[0] == "endmodule");
    return false;
  }

  assert (tokens.size() >= 4) ; // We should have cellType, instName, and at least one pin-net pair

  cellType     = tokens[0];
  cellInstName = tokens[1];

  for (unsigned i = 2; i < tokens.size()-1; i += 2)
  {
    assert(tokens[i][0] == '.');          // pin names start with '.'
    string pinName = tokens[i].substr(1); // skip the first character of tokens[i] 

    pinNetPairs.push_back(make_pair(pinName, tokens[i+1]));
  }

  return valid;
}

void circuit::read_sdc(const string &input)
{
  cout << "  .sdc file       : "<< input <<endl;
  ifstream dot_sdc(input.c_str());
  if (!dot_sdc.good())
  {
    cerr << "read_sdc:: cannot open `" << input << "' for reading." << endl;
    exit(1);
  }
  bool valid = read_clock(dot_sdc, clock_name, clock_port, clock_period);
#ifdef DEBUG
      cout << "Clock period is " << clock_period << " ps" << endl;
#endif
	//clock_period *= 1.e-12;
  assert(valid);
  do
  {
    string portName;
    double delay;
    valid = read_input_delay(dot_sdc, portName, delay);
    if (valid)
    {
#ifdef DEBUG
      cout << "Input port " << portName << " has delay " << delay << " ps" << endl;
#endif
			assert(pins[ pin2id[portName] ].type == PI_PIN);
      pins[ pin2id[portName] ].delay = delay ;//* 1e-12;
    }
  } while (valid);

  do
  {
    string portName;
    string driverType;
    string driverPin;
    double inputTransitionFall;
    double inputTransitionRise;

    valid = read_driver_info(dot_sdc, portName, driverType, driverPin, inputTransitionFall, inputTransitionRise);
    if (valid)
    {
			assert(pins[ pin2id[portName] ].type == PI_PIN);
      pins[ pin2id[portName] ].driverType = macro2id[ driverType ];
      pins[ pin2id[portName] ].rTran = inputTransitionRise ;// * 1e-12;
      pins[ pin2id[portName] ].fTran = inputTransitionFall ;//* 1e-12;
    }
  } while (valid);

  do
  {
    string portName;
    double delay;
    valid = read_output_delay(dot_sdc, portName, delay);
    if (valid)
    {
#ifdef DEBUG
     cout << "Output port " << portName << " has delay " << delay << endl;
#endif
		 assert(pins[ pin2id[portName] ].type == PO_PIN);
     pins[ pin2id[portName] ].delay = delay ;//* 1e-12;
    }
  } while (valid);

  do
  {
    string portName;
    double load;
    valid = read_output_load(dot_sdc, portName, load);
    if (valid)
    {
#ifdef DEBUG
      cout << "Output port " << portName << " has load " << load << " fF" <<endl;
#endif
			assert(pins[ pin2id[portName] ].type == PO_PIN);
      pins[ pin2id[portName] ].cap = load * 1e-15;
    }
  } while (valid);

  dot_sdc.close();
}

// Read clock definition
// Return value indicates if the last read was successful or not.  
bool circuit::read_clock(ifstream &is, string &clockName, string &clockPort, double &period)
{
  clockName = "";
  clockPort = "";
  period    = 0.0;

  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);

  while (valid)
  {
    if (tokens.size() == 7 && tokens[0] == "create_clock" && tokens[1] == "-name")
    {
      clockName = tokens[2];
      assert(tokens[3] == "-period");
      period = atof(tokens[4].c_str());

      assert (tokens[5] == "get_ports") ;
      clockPort = tokens[6];
      break;
    }

    valid = read_line_as_tokens(is, tokens);
  }

  // Skip the next comment line to prepare for the next stage
  bool valid2 = read_line_as_tokens(is, tokens);
  assert(valid2);
  assert(tokens.size() == 2);
  assert(tokens[0] == "input" && tokens[1] == "delays");

  return valid;
}

// Read input delay
// Return value indicates if the last read was successful or not.
bool circuit::read_input_delay(ifstream &is, string &portName, double &delay)
{
  portName = "";
  delay    = 0.0;

  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);
  assert(valid);
  assert(tokens.size() >= 2);

  if (valid && tokens[0] == "set_input_delay")
  {
    assert (tokens.size() == 6);
    delay = atof(tokens[1].c_str());
    assert (tokens[2] == "get_ports") ;
    portName = tokens[3];
    assert (tokens[4] == "-clock");
  }
  else
  {
    assert(tokens.size() == 2) ;
    assert(tokens[0] == "input" && tokens[1] == "drivers");
    return false;
  }

  return valid;
}

// Read output delay
// Return value indicates if the last read was successful or not.
bool circuit::read_output_delay(ifstream &is, string &portName, double &delay)
{
  portName = "";
  delay    = 0.0;

  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);
  assert(valid);
  assert(tokens.size() >= 2);

  if (valid && tokens[0] == "set_output_delay")
  {
    assert(tokens.size() == 6);
    delay = atof(tokens[1].c_str());
    assert(tokens[2] == "get_ports");
    portName = tokens[3];
    assert(tokens[4] == "-clock");
  }
  else
  {
    assert(tokens.size() == 2);
    assert(tokens[0] == "output" && tokens[1] == "loads");
    return false;
  }

  return valid;
}

// Read driver info for the input port
// Return value indicates if the last read was successful or not.
bool circuit::read_driver_info(ifstream &is, string &inPortName, string &driverType, string &driverPin,
                               double &inputTransitionFall, double &inputTransitionRise)
{
  inPortName          = "";
  driverType          = "";
  driverPin           = "";
  inputTransitionFall = 0.0;
  inputTransitionRise = 0.0;

  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);

  assert(valid);
  assert(tokens.size() >= 2);

  if (valid && tokens[0] == "set_driving_cell")
  {
    assert(tokens.size() == 11);
    assert(tokens[1] == "-lib_cell");

    driverType = tokens[2];
    assert (tokens[3] == "-pin");

    driverPin = tokens[4];
    assert (tokens[5] == "get_ports");
    
    inPortName = tokens[6] ;
    assert (tokens[7] == "-input_transition_fall");
    inputTransitionFall = atof(tokens[8].c_str());

    assert (tokens[9] == "-input_transition_rise") ;
    inputTransitionRise = atof(tokens[10].c_str()) ;    
  }
  else
  {
    assert(tokens.size() == 2);
    assert(tokens[0] == "output" && tokens[1] == "delays");

    return false;
  }

  return valid;
}

// Read output load
// Return value indicates if the last read was successful or not.  
bool circuit::read_output_load(ifstream &is, string &outPortName, double &load)
{
  outPortName = "";
  load        = 0.0;

  vector<string> tokens;
  bool valid = read_line_as_tokens(is, tokens);

  if (valid && tokens[0] == "set_load")
  {
    assert(tokens.size() == 5);
    assert(tokens[1] == "-pin_load");
    load = atof(tokens[2].c_str());

    assert(tokens[3] == "get_ports");
    outPortName = tokens[4];
  }
  else
  {
    assert (!valid);
    return false;
  }

  return valid;
}

void circuit::read_def(const string &input, bool mode)
{
	if(mode == INIT)
	  cout << "  .def file       : "<< input <<endl;
	else
		cout << "  final .def file : "<< input <<endl;

  ifstream dot_def(input.c_str());
  if (!dot_def.good())
  {
    cerr << "read_def:: cannot open `" << input << "' for reading." << endl;
    exit(1);
  }

  vector<string> tokens(1);
  while (!dot_def.eof())
  {
    get_next_token(dot_def, tokens[0], DEFCommentChar);

    if (tokens[0] == DEFLineEndingChar)
      continue;

    if (tokens[0] == "VERSION")
    {
      get_next_token(dot_def, tokens[0], DEFCommentChar);
      DEFVersion = tokens[0];
#ifdef DEBUG
      cout << "def version: " << DEFVersion << endl;
#endif
    }
    else if (tokens[0] == "DIVIDERCHAR")
    {
      get_next_token(dot_def, tokens[0], DEFCommentChar);
      unsigned index1 = tokens[0].find_first_of("\"");
      unsigned index2 = tokens[0].find_last_of("\"");
      assert(index1 != string::npos);
      assert(index2 != string::npos);
      assert(index2 > index1);
      DEFDelimiter = tokens[0].substr(index1+1,index2-index1-1);
#ifdef DEBUG
      cout << "divide character: " << DEFDelimiter << endl;
#endif
    }
    else if (tokens[0] == "BUSBITCHARS")
    {
      get_next_token(dot_def, tokens[0], DEFCommentChar);
      unsigned index1 = tokens[0].find_first_of("\"");
      unsigned index2 = tokens[0].find_last_of("\"");
      assert(index1 != string::npos);
      assert(index2 != string::npos);
      assert(index2 > index1);
      DEFBusCharacters = tokens[0].substr(index1+1,index2-index1-1);
#ifdef DEBUG
      cout << "bus bit characters: " << DEFBusCharacters << endl;
#endif
    }
    else if (tokens[0] == "DESIGN")
    {
      get_next_token(dot_def, tokens[0], DEFCommentChar);
      design_name = tokens[0];
#ifdef DEBUG
      cout << "design name: " << design_name << endl;
#endif
    }
    else if (tokens[0] == "UNITS")
    {
      get_next_n_tokens(dot_def, tokens, 3, DEFCommentChar);
      assert(tokens.size() == 3);
      assert(tokens[0] == "DISTANCE");
      assert(tokens[1] == "MICRONS");
      DEFdist2Microns = atoi(tokens[2].c_str());
			assert(DEFdist2Microns <= LEFdist2Microns);
#ifdef DEBUG
      cout << "unit distance to microns: " << DEFdist2Microns << endl;
#endif
    }
    else if (tokens[0] == "DIEAREA" && mode == INIT)
    {
      get_next_n_tokens(dot_def, tokens, 8, DEFCommentChar);
			assert(tokens.size() == 8);
      assert(tokens[0] == "(" && tokens[3] == ")");
      assert(tokens[4] == "(" && tokens[7] == ")");
			lx = atof(tokens[1].c_str());
			by = atof(tokens[2].c_str());
			rx = atof(tokens[5].c_str());
			ty = atof(tokens[6].c_str());
    }
    else if (tokens[0] == "ROW" && mode == INIT)
    {
      get_next_n_tokens(dot_def, tokens, 5, DEFCommentChar);
      row* myRow        = locateOrCreateRow(tokens[0]);
      myRow->name       = tokens[0];
      myRow->site       = site2id[ tokens[1] ];
      myRow->origX      = atoi(tokens[2].c_str());
      myRow->origY      = atoi(tokens[3].c_str());
      myRow->siteorient = tokens[4];
			// NOTE: this contest does not allow flipping/rotation
			assert(myRow->siteorient == "N");

      get_next_token(dot_def, tokens[0], DEFCommentChar);
      if (tokens[0] == "DO")
      {
        get_next_n_tokens(dot_def, tokens, 3, DEFCommentChar);
        assert(tokens[1] == "BY");
        myRow->numSites = max(atoi(tokens[0].c_str()), atoi(tokens[2].c_str()));
				// NOTE: currenlty we only handle horizontal row sites
				assert(tokens[2] == "1");
        get_next_token(dot_def, tokens[0], DEFCommentChar);
        if (tokens[0] == "STEP")
        {
          get_next_n_tokens(dot_def, tokens, 2, DEFCommentChar);
          myRow->stepX = atoi(tokens[0].c_str());
          myRow->stepY = atoi(tokens[1].c_str());
					// NOTE: currenlty we only handle horizontal row sites & spacing = 0
					assert(myRow->stepX == sites[ myRow->site ].width * LEFdist2Microns );
					assert(myRow->stepY == 0);
        }
      }
      // else we do not currently store properties
    }
    else if (tokens[0] == "COMPONENTS")
    {
			if(mode == INIT)
	      read_init_def_components(dot_def);
			else if(mode == FINAL)
	      read_final_def_components(dot_def);
    }
    else if (tokens[0] == "PINS" && mode == INIT)
    {
      read_def_pins(dot_def);
    }
    else if (tokens[0] == "NETS" && mode == INIT)
    {
      read_def_nets(dot_def);
    }
		else if (tokens[0] == "PROPERTYDEFINITIONS")
		{
      get_next_token(dot_def, tokens[0], DEFCommentChar);
			while(tokens[0] != "PROPERTYDEFINITIONS")
				get_next_token(dot_def, tokens[0], DEFCommentChar);
		}
		else if (tokens[0] == "SPECIALNETS")
		{
      get_next_token(dot_def, tokens[0], DEFCommentChar);
			while(tokens[0] != "SPECIALNETS")
				get_next_token(dot_def, tokens[0], DEFCommentChar);
		}
    else if (tokens[0] == "END")
    {
      get_next_token(dot_def, tokens[0], DEFCommentChar);
      assert(tokens[0] == "DESIGN");
      break;
    }
		else
		{
      get_next_token(dot_def, tokens[0], DEFCommentChar);
		}
  }
  dot_def.close();
}

// assumes the COMPONENTS keyword has already been read in
void circuit::read_init_def_components(ifstream &is)
{
  cell* myCell = NULL;
  vector<string> tokens(1);
  
  get_next_n_tokens(is, tokens, 2, DEFCommentChar);
  assert(tokens[1] == DEFLineEndingChar);

  unsigned countComponents = 0;
  unsigned numComponents = atoi(tokens[0].c_str());

  // can do with while(1)
  while (countComponents <= numComponents)
  {
    get_next_token(is, tokens[0], DEFCommentChar);
    if (tokens[0] == "-")
    {
      ++countComponents;
      get_next_n_tokens(is, tokens, 2, DEFCommentChar);
			//assert(cell2id.find(tokens[0]) != cell2id.end());
			if(cell2id.find(tokens[0]) == cell2id.end())
			{
				myCell = locateOrCreateCell(tokens[0]);
				myCell->type = macro2id[ tokens[1] ];
				macro* myMacro      = &macros[ macro2id[ tokens[1] ] ];
				myCell->width  = myMacro->width  * static_cast<double>(LEFdist2Microns);
				myCell->height = myMacro->height * static_cast<double>(LEFdist2Microns);
			}
			else
				myCell = locateOrCreateCell(tokens[0]);
    }
    else if (tokens[0] == "+")
    {
      assert(myCell != NULL);
      get_next_token(is, tokens[0], DEFCommentChar);

      if (tokens[0] == "PLACED" || tokens[0] == "FIXED")
      {
				myCell->isFixed = (tokens[0] == "FIXED");
        get_next_n_tokens(is, tokens, 5, DEFCommentChar);
        assert(tokens[0] == "(");
        assert(tokens[3] == ")");
				myCell->init_x_coord = atoi(tokens[1].c_str());
				myCell->init_y_coord = atoi(tokens[2].c_str());
				myCell->cellorient = tokens[4];
				// NOTE: this contest does not allow flipping/rotation
				assert(myCell->cellorient == "N");
      }
    }
    else if (!strcmp(tokens[0].c_str(), DEFLineEndingChar))
    {
      myCell = NULL;
    }
    else if (tokens[0] == "END")
    {
      get_next_token(is, tokens[0], DEFCommentChar);
      assert(tokens[0] == "COMPONENTS");
      break;
    }
  }
}

// assumes the COMPONENTS keyword has already been read in
void circuit::read_final_def_components(ifstream &is)
{
  cell* myCell = NULL;
  vector<string> tokens(1);
  
  get_next_n_tokens(is, tokens, 2, DEFCommentChar);
  assert(tokens[1] == DEFLineEndingChar);

  unsigned countComponents = 0;
  unsigned numComponents = atoi(tokens[0].c_str());

  // can do with while(1)
  while (countComponents <= numComponents)
  {
    get_next_token(is, tokens[0], DEFCommentChar);
    if (tokens[0] == "-")
    {
      ++countComponents;
      get_next_n_tokens(is, tokens, 2, DEFCommentChar);
			assert(cell2id.find(tokens[0]) != cell2id.end());
      myCell = locateOrCreateCell(tokens[0]);
    }
    else if (tokens[0] == "+")
    {
      assert(myCell != NULL);
      get_next_token(is, tokens[0], DEFCommentChar);

      if (tokens[0] == "PLACED" || tokens[0] == "FIXED")
      {
				//myCell->isFixed = (tokens[0] == "FIXED");
        get_next_n_tokens(is, tokens, 5, DEFCommentChar);
        assert(tokens[0] == "(");
        assert(tokens[3] == ")");
        myCell->x_coord = atoi(tokens[1].c_str());
        myCell->y_coord = atoi(tokens[2].c_str());
        myCell->cellorient = tokens[4];
				//NOTE: this contest does not allow flipping/rotation
				//assert(myCell->cellorient == "N");
      }
    }
    else if (!strcmp(tokens[0].c_str(), DEFLineEndingChar))
    {
      myCell = NULL;
    }
    else if (tokens[0] == "END")
    {
      get_next_token(is, tokens[0], DEFCommentChar);
      assert(tokens[0] == "COMPONENTS");
      break;
    }
  }
}

// assumes the PINS keyword has already been read in
// we already read pins from .verilog, 
// thus this update locations / performs sanity checks
void circuit::read_def_pins(ifstream &is)
{
  pin* myPin = NULL;
  vector<string> tokens(1);
  
  get_next_n_tokens(is, tokens, 2, DEFCommentChar);
  assert(tokens[1] == DEFLineEndingChar);

  unsigned countPins = 0;
  unsigned numPins = atoi(tokens[0].c_str());

  while (countPins <= numPins)
  {
    get_next_token(is, tokens[0], DEFCommentChar);
    if (tokens[0] == "-")
    {
      ++countPins;
      get_next_token(is, tokens[0], DEFCommentChar);
			assert(pin2id.find(tokens[0]) != pin2id.end());
      myPin = locateOrCreatePin(tokens[0]);
			// NOTE: pins in .def are only for PI/POs that are fixed/placed
			assert(myPin->type == PI_PIN || myPin->type == PO_PIN);
    }
    else if (tokens[0] == "+")
    {
      assert(myPin != NULL);
      get_next_token(is, tokens[0], DEFCommentChar);

      // NOTE: currently, we just store NET, DIRECTION, LAYER, FIXED/PLACED
      if (tokens[0] == "NET")
      {
        get_next_token(is, tokens[0], DEFCommentChar);
				assert(net2id.find(tokens[0]) != net2id.end());
      }
      else if (tokens[0] == "DIRECTION")
      {
        assert(myPin != NULL);
        get_next_token(is, tokens[0], DEFCommentChar);
				if(myPin->type == PI_PIN)
					assert(tokens[0] == "INPUT");
				else if(myPin->type == PO_PIN)
					assert(tokens[0] == "OUTPUT");
      }
      else if (tokens[0] == "FIXED" || tokens[0] == "PLACED")
      {
        assert(myPin != NULL);
				myPin->isFixed = (tokens[0] == "FIXED");
        get_next_n_tokens(is, tokens, 5, DEFCommentChar);
        assert(tokens[0] == "(");
        assert(tokens[3] == ")");
        myPin->x_coord = atof(tokens[1].c_str());
        myPin->y_coord = atof(tokens[2].c_str());
				// NOTE: this contest does not allow flipping/rotation
				assert(tokens[4] == "N");
      }
      else if (tokens[0] == "LAYER")
      {
        assert(myPin != NULL);
        get_next_token(is, tokens[0], DEFCommentChar);
				// NOTE: we assume the layer is previously defined from .lef
				// we don't save layer for a pin instance.
				assert(layer2id.find(tokens[0]) != layer2id.end());
        get_next_n_tokens(is, tokens, 8, DEFCommentChar);
        assert(tokens[0] == "(");
        assert(tokens[3] == ")");
        assert(tokens[4] == "(");
        assert(tokens[7] == ")");
        myPin->x_coord += 0.5 * (atof(tokens[1].c_str()) + atof(tokens[5].c_str()));
        myPin->y_coord += 0.5 * (atof(tokens[2].c_str()) + atof(tokens[6].c_str()));
      }
      else if (!strcmp(tokens[0].c_str(), DEFLineEndingChar))
      {
        myPin = NULL;
      }
    }
    else if (tokens[0] == "END")
    {
      get_next_token(is, tokens[0], DEFCommentChar);
      assert(tokens[0] == "PINS");
      break;
    }
  }
}

// assumes the NETS keyword has already been read in
// we already read nets from .verilog, 
// thus this only performs sanity checks
void circuit::read_def_nets(ifstream &is)
{
  net* myNet = NULL;
  pin* myPin = NULL;
  vector<string> tokens(1);

  get_next_n_tokens(is, tokens, 2, DEFCommentChar);
  assert(tokens[1] == DEFLineEndingChar);
  
  unsigned countNets = 0;
  unsigned numNets = atoi(tokens[0].c_str());

  while (countNets <= numNets)
  {
    get_next_token(is, tokens[0], DEFCommentChar);
    if (tokens[0] == "-")
    {
      get_next_token(is, tokens[0], DEFCommentChar);

			// all nets should be already found in .verilog
			assert(net2id.find(tokens[0]) != net2id.end());
      myNet = locateOrCreateNet(tokens[0]);
      unsigned myNetId = net2id.find(myNet->name)->second;

      // first is always source, rest are sinks
      get_next_n_tokens(is, tokens, 4, DEFCommentChar);
      assert(tokens[0] == "(");
      assert(tokens[3] == ")");
      // ( PIN PI/PO ) or ( cell_instance internal_pin )
      string pinName = tokens[1] == "PIN" ? tokens[2] : tokens[1] + ":" + tokens[2];
			assert(pin2id.find(pinName) != pin2id.end());
      myPin          = locateOrCreatePin(pinName);
			assert(myPin->net == myNetId);

      do
      {
        get_next_token(is, tokens[0], DEFCommentChar);
        if (tokens[0] == DEFLineEndingChar)
          break;
        assert(tokens[0] == "(");
        get_next_n_tokens(is, tokens, 3, DEFCommentChar);
        assert(tokens.size() == 3);
        assert(tokens[2] == ")");
        pinName    = tokens[0] == "PIN" ? tokens[1] : tokens[0] + ":" + tokens[1];
				assert(pin2id.find(pinName) != pin2id.end());
        myPin      = locateOrCreatePin(pinName);
        myPin->net = myNetId;
				assert(myPin->net == myNetId);
      } while (tokens[2] == ")");
    }
    else if (!strcmp(tokens[0].c_str(), DEFLineEndingChar))
    {
      myNet = NULL;
      myPin = NULL;
    }
    else if (tokens[0] == "END")
    {
      get_next_token(is, tokens[0], DEFCommentChar);
      assert(tokens[0] == "NETS");
      break;
    }
  }
}

void circuit::read_lef(const string &input)
{
  cout << "  .lef file       : "<< input <<endl;
  ifstream dot_lef(input.c_str());
  if (!dot_lef.good())
  {
    cerr << "read_lef:: cannot open `" << input << "' for reading." << endl;
    exit(1);
  }

  vector<string> tokens(1);
  while (!dot_lef.eof())
  {
    get_next_token(dot_lef, tokens[0], LEFCommentChar);

    if (tokens[0] == LEFLineEndingChar)
      continue;

    if (tokens[0] == "VERSION")
    {
      get_next_token(dot_lef, tokens[0], LEFCommentChar);
      LEFVersion = tokens[0];
#ifdef DEBUG
      cout << "lef version: " << LEFVersion << endl;
#endif
    }
    else if (tokens[0] == "NAMECASESENSITIVE")
    {
      get_next_token(dot_lef, tokens[0], LEFCommentChar);
      LEFNamesCaseSensitive = tokens[0];
#ifdef DEBUG
      cout << "names case sensitive: " << LEFNamesCaseSensitive << endl;
#endif
    }
    else if (tokens[0] == "BUSBITCHARS")
    {
      get_next_token(dot_lef, tokens[0], LEFCommentChar);
      unsigned index1 = tokens[0].find_first_of("\"");
      unsigned index2 = tokens[0].find_last_of("\"");
      assert(index1 != string::npos);
      assert(index2 != string::npos);
      assert(index2 > index1);
      LEFBusCharacters = tokens[0].substr(index1+1,index2-index1-1);
#ifdef DEBUG
      cout << "bus bit characters: " << LEFBusCharacters << endl;
#endif
    }
    else if (tokens[0] == "DIVIDERCHAR")
    {
      get_next_token(dot_lef, tokens[0], LEFCommentChar);
      unsigned index1 = tokens[0].find_first_of("\"");
      unsigned index2 = tokens[0].find_last_of("\"");
      assert(index1 != string::npos);
      assert(index2 != string::npos);
      assert(index2 > index1);
      LEFDelimiter = tokens[0].substr(index1+1,index2-index1-1);
#ifdef DEBUG
      cout << "divide character: " << LEFDelimiter << endl;
#endif
    }
    else if (tokens[0] == "UNITS")
    {
      get_next_n_tokens(dot_lef, tokens, 3, LEFCommentChar);
      assert(tokens.size() == 3);
      assert(tokens[0] == "DATABASE");
      assert(tokens[1] == "MICRONS");
      LEFdist2Microns = atoi(tokens[2].c_str());
#ifdef DEBUG
      cout << "unit distance to microns: " << LEFdist2Microns << endl;
#endif
      get_next_n_tokens(dot_lef, tokens, 3, LEFCommentChar);
      assert(tokens[0] == LEFLineEndingChar);
      assert(tokens[1] == "END");
      assert(tokens[2] == "UNITS");
    }
    else if (tokens[0] == "MANUFACTURINGGRID")
    {
      get_next_token(dot_lef, tokens[0], LEFLineEndingChar);
      LEFManufacturingGrid = atof(tokens[0].c_str());
#ifdef DEBUG
      cout << "manufacturing grid: " << LEFManufacturingGrid << endl;
#endif
    }
    else if (tokens[0] == "SITE")
      read_lef_site(dot_lef);
    else if (tokens[0] == "LAYER")
      read_lef_layer(dot_lef);
    else if (tokens[0] == "MACRO")
      read_lef_macro(dot_lef);
    else if (tokens[0] == "END")
    {
      get_next_token(dot_lef, tokens[0], LEFCommentChar);
      assert(tokens[0] == "LIBRARY");
      break;
    }
  }
  dot_lef.close();
}

// assumes the SITE keyword has already been read in
void circuit::read_lef_site(ifstream &is)
{
#ifdef DEBUG
  cerr << "read_lef_site:: begin\n" << endl;
#endif
  site* mySite = NULL;
  vector<string> tokens(1);

  get_next_token(is, tokens[0], LEFCommentChar);
  mySite = locateOrCreateSite(tokens[0]);

  get_next_token(is, tokens[0], LEFCommentChar);
  while (tokens[0] != "END")
  {
    if (tokens[0] == "SIZE")
    {
      assert(mySite != NULL);
      get_next_n_tokens(is, tokens, 4, LEFCommentChar);
      assert(tokens[1] == "BY");
      assert(tokens[3] == LEFLineEndingChar);
      mySite->width  = atof(tokens[0].c_str());
      mySite->height = atof(tokens[2].c_str());
    }
    else if (tokens[0] == "CLASS")
    {
      get_next_n_tokens(is, tokens, 2, LEFCommentChar);
      assert(tokens[1] == LEFLineEndingChar);
      mySite->type = tokens[0];
    }
    else if (tokens[0] == "SYMMETRY")
    {
			// NOTE: this contest does not allow flipping/rotation
			// even though symmetries are specified for a site
      get_next_token(is, tokens[0], LEFCommentChar);
			while(tokens[0] != LEFLineEndingChar)
			{
				mySite->symmetries.push_back(tokens[0]);
				get_next_token(is, tokens[0], LEFCommentChar);
			}
      assert(tokens[0] == LEFLineEndingChar);
    }
    else
    {
      cout << "read_lef_site::unsupported keyword " << tokens[0] << endl;
//      exit(1);
    }
    get_next_token(is, tokens[0], LEFCommentChar);
  }
  get_next_token(is, tokens[0], LEFCommentChar);
  assert(tokens[0] == "core");
}

// assumes the LAYER keyword has already been read in
void circuit::read_lef_layer(ifstream &is)
{
  layer* myLayer;
  vector<string> tokens(1);

  get_next_token(is, tokens[0], LEFCommentChar);
  myLayer = locateOrCreateLayer(tokens[0]);

  get_next_token(is, tokens[0], LEFCommentChar);
  while (tokens[0] != "END")
  {
    if (tokens[0] == "TYPE")
    {
      get_next_n_tokens(is, tokens, 2, LEFCommentChar);
      assert(tokens[1] == LEFLineEndingChar);
      myLayer->type = tokens[0];
    }
    else if (tokens[0] == "DIRECTION")
    {
      get_next_n_tokens(is, tokens, 2, LEFCommentChar);
      assert(tokens[1] == LEFLineEndingChar);
      myLayer->direction = tokens[0];
    }
    else if (tokens[0] == "PITCH")
    {
      get_next_n_tokens(is, tokens, 2, LEFCommentChar);
      myLayer->xPitch = atof(tokens[0].c_str());
      if (tokens[1] == LEFLineEndingChar)
        myLayer->yPitch = myLayer->xPitch;
      else
      {
        myLayer->yPitch = atof(tokens[1].c_str());
        get_next_token(is, tokens[0], LEFCommentChar);
        assert(tokens[0] == LEFLineEndingChar);
      }
    }
    else if (tokens[0] == "OFFSET")
    {
      get_next_n_tokens(is, tokens, 2, LEFCommentChar);
      myLayer->xOffset = atof(tokens[0].c_str());
      if (tokens[1] == LEFLineEndingChar)
        myLayer->yOffset = myLayer->xOffset;
      else
      {
        myLayer->yOffset = atof(tokens[1].c_str());
        get_next_token(is, tokens[0], LEFCommentChar);
        assert(tokens[0] == LEFLineEndingChar);
      }
    }
    else if (tokens[0] == "WIDTH")
    {
      get_next_n_tokens(is, tokens, 2, LEFCommentChar);
      assert(tokens[1] == LEFLineEndingChar);
      myLayer->width = atof(tokens[0].c_str());
    }
    else
    {
      cerr << "read_lef_layer:: unsupported keyword " << tokens[0] << endl;
//      exit(4);
    }
    get_next_token(is, tokens[0], LEFCommentChar);
  }

  get_next_token(is, tokens[0], LEFCommentChar);
  assert(myLayer->name == tokens[0]);
}


// assumes the keyword MACRO has already been read in
void circuit::read_lef_macro(ifstream &is)
{
  macro* myMacro;
  vector<string> tokens(1);

  get_next_token(is, tokens[0], LEFCommentChar);
  myMacro = locateOrCreateMacro(tokens[0]);

  get_next_token(is, tokens[0], LEFCommentChar);
  while (tokens[0] != "END")
  {
    if (tokens[0] == "CLASS")
    {
      get_next_n_tokens(is, tokens, 2, LEFCommentChar);
      assert(tokens[1] == LEFLineEndingChar);
      myMacro->type = tokens[0];
    }
    else if (tokens[0] == "ORIGIN")
    {
      get_next_n_tokens(is, tokens, 3, LEFCommentChar);
      assert(tokens[2] == LEFLineEndingChar);
      myMacro->xOrig = atof(tokens[0].c_str());
      myMacro->yOrig = atof(tokens[1].c_str());
    }
    else if (tokens[0] == "SIZE")
    {
      get_next_n_tokens(is, tokens, 4, LEFCommentChar);
      assert(tokens[1] == "BY");
      assert(tokens[3] == LEFLineEndingChar);
      myMacro->width  = atof(tokens[0].c_str());
      myMacro->height = atof(tokens[2].c_str());
    }
    else if (tokens[0] == "SITE")
      read_lef_macro_site(is, myMacro);
    else if (tokens[0] == "PIN")
      read_lef_macro_pin(is, myMacro);
    else if (tokens[0] == "SYMMETRY")
    {
			// NOTE: this contest does not allow flipping/rotation
			// even though symmetries are specified for a macro
      get_next_token(is, tokens[0], LEFCommentChar);
			while(tokens[0] != LEFLineEndingChar)
				get_next_token(is, tokens[0], LEFCommentChar);
      assert(tokens[0] == LEFLineEndingChar);
    }
		else if (tokens[0] == "FOREIGN") 
		{
      get_next_n_tokens(is, tokens, 4, LEFCommentChar);
      assert(tokens[3] == LEFLineEndingChar);
		}
		else if (tokens[0] == "OBS") 
		{
			get_next_token(is, tokens[0], LEFCommentChar);
			// NOTE: this contest does not handle other than LAYER keyword for OBS 
			while(tokens[0] != "END")
			{
				if(tokens[0] == "LAYER")
				{
					get_next_n_tokens(is, tokens, 2, LEFCommentChar);
					assert(tokens[1] == LEFLineEndingChar);
					// NOTE: this contest only checks legality on metal 1 layer
					if(tokens[0] == "metal1")
					{
						get_next_token(is, tokens[0], LEFCommentChar);
						while(tokens[0] != "END" && tokens[0] != "LAYER")
						{
							assert(tokens[0] == "RECT");
							get_next_n_tokens(is, tokens, 5, LEFCommentChar);
							rect theRect;
							theRect.xLL=atof(tokens[0].c_str());
							theRect.yLL=atof(tokens[1].c_str());
							theRect.xUR=atof(tokens[2].c_str());
							theRect.yUR=atof(tokens[3].c_str());
							myMacro->obses.push_back(theRect);
							get_next_token(is, tokens[0], LEFCommentChar);
						}
					}
					else
						get_next_token(is, tokens[0], LEFCommentChar);
				}
				else
					get_next_token(is, tokens[0], LEFCommentChar);
			}
		}
    else
    {
      cerr << "read_lef_macro:: unsupported keyword " << tokens[0] << endl;
      exit(2);
    }
    get_next_token(is, tokens[0], LEFCommentChar);
  }
  get_next_token(is, tokens[0], LEFCommentChar);
  assert(myMacro->name == tokens[0]);
	return;
}

// assumes the keyword SITE has already been read in
void circuit::read_lef_macro_site(ifstream &is, macro* myMacro)
{
#ifdef DEBUG
  cerr << "read_lef_macro_site:: begin\n" << endl;
#endif
  site* mySite = NULL;
  vector<string> tokens(1);
  get_next_token(is, tokens[0], LEFCommentChar);

  mySite = locateOrCreateSite(tokens[0]);
  myMacro->sites.push_back(site2id.find(mySite->name)->second);

  // does not support [sitePattern]
  unsigned numArgs = 0;
  do
  {
    get_next_token(is, tokens[0], LEFCommentChar);
    ++numArgs;
  } while (tokens[0] != LEFLineEndingChar);
  
  if (numArgs > 1)
  {
    cout << "read_lef_macro_site:: WARNING -- bypassing " << numArgs << " additional field in " 
         << "MACRO " << myMacro->name << " SITE " << sites[site2id.find(mySite->name)->second].name << "." << endl;
    cout << "It is likely that the fields are from [sitePattern], which are currently not expected." << endl;
  }
#ifdef DEBUG
  cerr << "read_lef_macro_site:: end\n" << endl;
#endif
}

// assumes the keyword PIN has already been read in
void circuit::read_lef_macro_pin(ifstream &is, macro* myMacro)
{
#ifdef DEBUG
  cerr << "read_lef_macro_pin:: begin\n" << endl;
#endif
	macro_pin myPin;
  vector<string> tokens(1);
  get_next_token(is, tokens[0], LEFCommentChar);
  string pinName = tokens[0];
  get_next_token(is, tokens[0], LEFCommentChar);
  while (tokens[0] != "END")
  {
    if (tokens[0] == "DIRECTION")
    {
      get_next_n_tokens(is, tokens, 2, LEFCommentChar);
      assert(tokens[1] == LEFLineEndingChar);
      myPin.direction = tokens[0];
    }
    else if (tokens[0] == "PORT")
    {
      get_next_token(is, tokens[0], LEFCommentChar);
      while (tokens[0] != "END")
      {
        if (tokens[0] == "LAYER")
        {
          get_next_n_tokens(is, tokens, 2, LEFCommentChar);
          assert(tokens[1] == LEFLineEndingChar);
          layer* myLayer = locateOrCreateLayer(tokens[0]);
          myPin.layer   = layer2id.find(myLayer->name)->second;
				}
        else if (tokens[0] == "RECT")
				{
					get_next_n_tokens(is, tokens, 2, LEFCommentChar);
					myPin.xLL = min(atof(tokens[0].c_str()), myPin.xLL);
					if (tokens[1] == "ITERATE")
					{
						get_next_n_tokens(is, tokens, 3, LEFCommentChar);
						myPin.yLL = min(atof(tokens[0].c_str()), myPin.yLL);
						myPin.xUR = max(atof(tokens[1].c_str()), myPin.xUR);
						myPin.yUR = max(atof(tokens[2].c_str()), myPin.yUR);
					}
					else
					{
						myPin.yLL = min(atof(tokens[1].c_str()), myPin.yLL);
						get_next_n_tokens(is, tokens, 2, LEFCommentChar);
						myPin.xUR = max(atof(tokens[0].c_str()), myPin.xUR);
						myPin.yUR = max(atof(tokens[1].c_str()), myPin.yUR);
					}
					get_next_token(is, tokens[0], LEFCommentChar);
					assert(tokens[0] == LEFLineEndingChar);
				}
        else
        {
          cerr << "read_lef_macro_pin:: unsupported keyword " << tokens[0] << endl;
//          exit(4);
        }
        get_next_token(is, tokens[0], LEFCommentChar);
      }
    }
    else if (tokens[0] == "USE")
		{
			while(tokens[0] != LEFLineEndingChar)
        get_next_token(is, tokens[0], LEFCommentChar);
		}
    else
    {
      cerr << "read_lef_macro_pin:: unsupported keyword " << tokens[0] << endl;
      exit(5);
    }
    get_next_token(is, tokens[0], LEFCommentChar);
  }
  get_next_token(is, tokens[0], LEFCommentChar);
  assert(pinName == tokens[0]);
	myMacro->pins[pinName] = myPin;
	if(pinName == FFClkPortName)
		myMacro->isFlop = true;
}

// requires full name, e.g., cell_instance/pin
pin* circuit::locateOrCreatePin(const string &pinName)
{
  map<string, unsigned>::iterator it = pin2id.find(pinName);
  if (it == pin2id.end())
  {
    pin thePin;
		thePin.name = pinName;
    thePin.id   = pins.size();
    pin2id.insert(make_pair(pinName, thePin.id));
    pins.push_back(thePin);
    return &pins[pins.size()-1];
  }
  else
    return &pins[it->second];
}

cell* circuit::locateOrCreateCell(const string &cellName)
{
  map<string, unsigned>::iterator it = cell2id.find(cellName);
  if (it == cell2id.end())
  {
    cell theCell;
    theCell.name = cellName;
    cell2id.insert(make_pair(theCell.name, cells.size()));
    cells.push_back(theCell);
    return &cells[cells.size()-1];
  }
  else
    return &cells[it->second];
}

macro* circuit::locateOrCreateMacro(const string &macroName)
{
  map<string, unsigned>::iterator it = macro2id.find(macroName);
  if (it == macro2id.end())
  {
    macro theMacro;
    theMacro.name = macroName;
    macro2id.insert(make_pair(theMacro.name, macros.size()));
    macros.push_back(theMacro);
    return &macros[macros.size()-1];
  }
  else
    return &macros[it->second];
}

net* circuit::locateOrCreateNet(const string &netName)
{
  map<string, unsigned>::iterator it = net2id.find(netName);
  if (it == net2id.end())
  {
    net theNet;
    theNet.name = netName;
    net2id.insert(make_pair(theNet.name, nets.size()));
    nets.push_back(theNet);
    return &nets[nets.size()-1];
  }
  else
    return &nets[it->second];
}

row* circuit::locateOrCreateRow(const string &rowName)
{
  map<string, unsigned>::iterator it = row2id.find(rowName);
  if (it == row2id.end())
  {
    row theRow;
    theRow.name = rowName;
    row2id.insert(make_pair(theRow.name, rows.size()));
    rows.push_back(theRow);
    return &rows[rows.size()-1];
  }
  else
    return &rows[it->second];
}

site* circuit::locateOrCreateSite(const string &siteName)
{
  map<string, unsigned>::iterator it = site2id.find(siteName);
  if (it == site2id.end())
  {
    site theSite;
    theSite.name = siteName;
    site2id.insert(make_pair(theSite.name, sites.size()));
    sites.push_back(theSite);
    return &sites[sites.size()-1];
  }
  else
    return &sites[it->second];
}

layer* circuit::locateOrCreateLayer(const string &layerName)
{
  map<string, unsigned>::iterator it = layer2id.find(layerName);
  if (it == layer2id.end())
  {
    layer theLayer;
    theLayer.name = layerName;
    layer2id.insert(make_pair(theLayer.name, layers.size()));
    layers.push_back(theLayer);
    return &layers[layers.size()-1];
  }
  else
    return &layers[it->second];
}

/* generic helper functions */
bool is_special_char(char c)
{
  static const char specialChars[] = {'(', ')', ',', ':', ';', '/', '#', '[', ']', '{', '}', '*', '\"', '\\'};
  
  for (unsigned i = 0; i < sizeof(specialChars); ++i)
  {
    if (c == specialChars[i])
      return true;
  }

  return false;
}

bool read_line_as_tokens(istream &is, vector<string> &tokens)
{
  tokens.clear();

  string line;
  getline(is, line);

  while (is && tokens.empty())
  {
    string token = "";
    for (unsigned i = 0; i < line.size(); ++i)
    {
      char currChar = line[i];
      if (isspace(currChar) || is_special_char(currChar))
      {
        if (!token.empty())
        {
          // Add the current token to the list of tokens
          tokens.push_back(token);
          token.clear();
        }
        // else if the token is empty, simply skip the whitespace or special char
      }
      else
      {
        // Add the char to the current token
        token.push_back(currChar);
      }
    }

    if (!token.empty())
      tokens.push_back(token);

    if (tokens.empty())
      // Previous line read was empty. Read the next one.
      getline(is, line);    
  }

  return !tokens.empty();
}

void get_next_token(ifstream &is, string &token, const char* beginComment)
{
  do
  {
    is >> token;
    if (!strcmp(token.substr(0,strlen(beginComment)).c_str(), beginComment))
    {
      getline(is, token);
      token = "";
    }
  } while (!is.eof() && (token.empty() || isspace(token[0])));
}

void get_next_n_tokens(ifstream &is, vector<string> &tokens, const unsigned numTokens, const char* beginComment)
{
  tokens.clear();
  string token;
  unsigned count = 0;
  do
  {
    is >> token;
    if (!strcmp(token.substr(0,strlen(beginComment)).c_str(), beginComment))
    {
      getline(is, token);
      token = "";
    }
    if (!token.empty() && !isspace(token[0]))
    {
      tokens.push_back(token);
      ++count;
    }
  } while (!is.eof() && count < numTokens);
}

void pin::print()
{
  cout << "|=== BEGIN PIN ===|  " << endl;
  cout << "name:                " << name << endl;
  cout << "id:                  " << id << endl;
  cout << "net:                 " << net << endl;
  cout << "pin owner:           " << owner << endl;
  cout << "isFixed?             " << (isFixed ? "yes" : "no") << endl;
  cout << "(x_coord,y_coord):   " << x_coord  << ", " << y_coord  << endl;
  cout << "(x_offset,y_offset): " << x_offset << ", " << y_offset << endl;
  cout << "|===  END  PIN ===|  " << endl;
}

void macro::print()
{
  cout << "|=== BEGIN MACRO ===|" << endl;
  cout << "name:                " << name << endl;
  cout << "type:                " << type << endl;
  cout << "(xOrig,yOrig):       " << xOrig << ", " << yOrig << endl;
  cout << "[width,height]:      " << width << ", " << height << endl;
  for (unsigned i = 0; i < sites.size(); ++i)
  {
    cout << "sites[" << i << "]: " << sites[i] << endl;
  }
  cout << "|=== BEGIN MACRO ===|" << endl;
}

void cell::print()
{
  cout << "|=== BEGIN CELL ===|" << endl;
  cout << "name:               " << name << endl;
  cout << "type:               " << type << endl;
  cout << "orient:             " << cellorient << endl;
  cout << "isFixed?            " << (isFixed ? "true" : "false") << endl;
	for (map<string, unsigned>::iterator it=ports.begin() ; it!=ports.end() ; it++)
		cout << "port: "<< (*it).first  << " - " << (*it).second <<endl;
  cout << "(init_x,  init_y):  " << init_x_coord << ", " << init_y_coord << endl;
  cout << "(x_coord,y_coord):  " << x_coord << ", " << y_coord << endl;
  cout << "[width,height]:      " << width << ", " << height << endl;
  cout << "|===  END  CELL ===|" << endl;
}

void net::print()
{
  cout << "|=== BEGIN NET ===|" << endl;
  cout << "name:              " << name << endl;
  cout << "source pin:        " << source << endl;
  for (unsigned i = 0; i < sinks.size(); ++i)
    cout << "sink pin [" << i << "]:      " << sinks[i] << endl;
  cout << "|===  END  NET ===|" << endl;
}

void row::print()
{
  cout << "|=== BEGIN ROW ===|" << endl;
  cout << "name:              " << name << endl;
  cout << "site:              " << site << endl;
  cout << "(origX,origY):     " << origX << ", " << origY << endl;
  cout << "(stepX,stepY):     " << stepX << ", " << stepY << endl;
  cout << "numSites:          " << numSites << endl;
  cout << "orientation:       " << siteorient << endl;
  cout << "|===  END  ROW ===|" << endl;
}

void site::print()
{
  cout << "|=== BEGIN SITE ===|" << endl;
  cout << "name:               " << name << endl;
  cout << "width:              " << width << endl;
  cout << "height:             " << height << endl;
  cout << "type:               " << type << endl;
	for(vector<string>::iterator it=symmetries.begin() ; it!=symmetries.end() ; ++it)
  cout << "symmetries:         " << *it << endl;
  cout << "|===  END  SITE ===|" << endl;
}

void layer::print()
{
  cout << "|=== BEGIN LAYER ===|" << endl;
  cout << "name:               " << name << endl;
  cout << "type:               " << type << endl;
  cout << "direction:          " << direction << endl;
  cout << "[xPitch,yPitch]:    " << xPitch << ", " << yPitch << endl;
  cout << "[xOffset,yOffset]:  " << xOffset << ", " << yOffset << endl;
  cout << "width:              " << width << endl;
  cout << "|===  END  LAYER ===|" << endl;
}

void circuit::print()
{
  for (unsigned i = 0; i < layers.size(); ++i)
    layers[i].print();
  for (unsigned i = 0; i < sites.size(); ++i)
    sites[i].print();
  for (unsigned i = 0; i < rows.size(); ++i)
    rows[i].print();
  for (unsigned i = 0; i < pins.size(); ++i)
    pins[i].print();
  for (unsigned i = 0; i < cells.size(); ++i)
    cells[i].print();
  for (unsigned i = 0; i < nets.size(); ++i)
    nets[i].print();
}

/* *************************************************************** */
/*  Desc: write a bookshelf-format circuit to run academic placers */
/* *************************************************************** */
void circuit::write_bookshelf()
{
	const double BOOKSHELF_X_SCALE = 760;
	const double BOOKSHELF_Y_SCALE = rowHeight / 9;
  //write .aux
	string bookshelf_dir="./bookshelf-"+benchmark+"/";
	system(("mkdir -p "+bookshelf_dir).c_str());
	ofstream dot_aux((bookshelf_dir+benchmark+".aux").c_str());
	if(!dot_aux.good())
		return;
	dot_aux << "RowBasedPlacement : "<< benchmark << ".nodes " << benchmark << ".nets "<< benchmark << ".wts " << benchmark << ".pl " << benchmark << ".scl " << benchmark << ".shapes";
	dot_aux.close();

	//write .nodes (including PI & PO pins as instances)
	ofstream dot_nodes((bookshelf_dir+benchmark+".nodes").c_str());
	if(!dot_nodes.good())
		return;
	dot_nodes << "UCLA nodes 1.0" << endl << endl;
	dot_nodes << "NumNodes : " << cells.size() + PIs.size() + POs.size()-1 << endl;
	dot_nodes << "NumTerminals : " << PIs.size() + POs.size() + num_fixed_nodes - 1 << endl <<endl;

	for(vector<unsigned>::iterator PI=PIs.begin() ; PI!=PIs.end() ; ++PI)
	{
		if(pins[ *PI ].name == clock_port)
			continue;
		dot_nodes << "\t\t" << pins[ *PI ].name << "\t\t" << 0 << "\t\t" << 0 << "\t terminal" <<endl;
	}
	for(vector<unsigned>::iterator PO=POs.begin() ; PO!=POs.end() ; ++PO)
		dot_nodes << "\t\t" << pins[ *PO ].name << "\t\t" << 0 << "\t\t" << 0 << "\t terminal" << endl;
	for(vector<cell>::iterator theCell = cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(theCell->isFixed)
			dot_nodes << "\t\t" << theCell->name << "\t\t" << theCell->width / BOOKSHELF_X_SCALE << "\t\t" << theCell->height / BOOKSHELF_Y_SCALE << "\t terminal" <<endl;
		else
			dot_nodes << "\t\t" << theCell->name << "\t\t" << theCell->width / BOOKSHELF_X_SCALE << "\t\t" << theCell->height / BOOKSHELF_Y_SCALE <<endl;
	}
	dot_nodes.close();

	//write .nets
	ofstream dot_nets((bookshelf_dir+benchmark+".nets").c_str());
	if(!dot_nets.good())
		return;
	dot_nets << "UCLA nets 1.0" << endl << endl;
	dot_nets << "NumNets : "<< nets.size()-1 << endl;
	unsigned pin_cnt=0;
	for(vector<pin>::iterator thePin = pins.begin() ; thePin != pins.end() ; ++thePin)
	{
		if(nets[ thePin->net ].name  == clock_port)
			continue;
		pin_cnt++;
	}
	dot_nets << "NumPins : " << pin_cnt << endl << endl;
	for(vector<net>::iterator theNet = nets.begin() ; theNet != nets.end() ; ++theNet)
	{
		if(theNet->name == clock_port)
			continue;
		dot_nets << "NetDegree : "<< theNet->sinks.size()+1 << "\t" << theNet->name << endl;
		if(pins[ theNet->source ].type == PI_PIN)
			dot_nets << "\t\t" << pins[ theNet->source ].name << " I  : \t" << 0 << "\t" << 0 << endl;
		else
		{
			cell *pinOwner = &cells[ pins[ theNet->source ].owner ];
			dot_nets << "\t\t" << pinOwner->name << " I  : ";
			dot_nets << pins[ theNet->source ].x_offset / BOOKSHELF_X_SCALE - 0.5 * pinOwner->width / BOOKSHELF_X_SCALE << "\t" << pins[ theNet->source ].y_offset / BOOKSHELF_Y_SCALE - 0.5 * pinOwner->height / BOOKSHELF_Y_SCALE <<endl;
		}
		for(vector<unsigned>::iterator theSink = theNet->sinks.begin() ; theSink != theNet->sinks.end() ; ++theSink)
		{
			if(pins[ *theSink ].type == PO_PIN)
				dot_nets << "\t\t" << pins[ *theSink ].name << " O  : \t" << 0 << "\t" << 0 << endl;
			else
			{
				cell *pinOwner = &cells[ pins[ *theSink ].owner ];
				dot_nets << "\t\t" << pinOwner->name << " O  : ";
				dot_nets << pins[ *theSink ].x_offset / BOOKSHELF_X_SCALE - 0.5 * pinOwner->width / BOOKSHELF_X_SCALE << "\t" << pins[ *theSink ].y_offset / BOOKSHELF_Y_SCALE - 0.5 * pinOwner->height / BOOKSHELF_Y_SCALE << endl;
			}
		}
	}
	dot_nets.close();

  //write .wts
	ofstream dot_wts((bookshelf_dir+benchmark+".wts").c_str());
	if(!dot_wts.good())
		return;
	dot_wts << "UCLA wts 1.0" << endl <<endl ;
	for(vector<net>::iterator theNet = nets.begin() ; theNet != nets.end() ; ++theNet)
		dot_wts << theNet->name << " " << 1.0 <<endl;
	dot_wts.close();

  //write .scl
	sort(rows.begin(), rows.end());
	ofstream dot_scl(bookshelf_dir+benchmark+".scl");
	if(!dot_scl.good())
		return;
	dot_scl << "UCLA scl 1.0" << endl << endl;
	dot_scl << "NumRows : " << rows.size() << endl << endl;
	for(unsigned i=0 ; i<rows.size() ; i++)
	{
		row *theRow = &rows[i];
		dot_scl << "CoreRow Horizontal" <<endl;
		dot_scl << "   Coordinate     : "  << theRow->origY / BOOKSHELF_Y_SCALE << endl;
		dot_scl << "   Height         : "  << rowHeight / BOOKSHELF_Y_SCALE << endl;
		dot_scl << "   Sitewidth      : " << sites[ theRow->site ].width * LEFdist2Microns / BOOKSHELF_X_SCALE << endl;
		// NOTE: some academic placers do not handle this properly, so set it to 1 here, but multiply it to NumSites
		dot_scl << "   Sitespacing    : " << sites[ theRow->site ].width * LEFdist2Microns / BOOKSHELF_X_SCALE << endl;
		dot_scl << "   Siteorient     : N" << endl;
		dot_scl << "   Sitesymmetry   : Y" << endl;
		dot_scl << "   SubrowOrigin   : "  << theRow->origX / BOOKSHELF_X_SCALE << "\t\t\tNumSites : " << theRow->numSites << endl;//* theRow->stepX << endl;
		while(i+1 <= rows.size()-1)
		{
			if(rows[i+1].origY == theRow->origY)
			{
				dot_scl << "   SubrowOrigin   : "  << rows[i+1].origX / BOOKSHELF_X_SCALE << "\t\t\tNumSites : " << rows[i+1].numSites * rows[i+1].stepX << endl;
				i++;
			}
			else
				break;
		}
		dot_scl << "End" <<endl;
	}
	dot_scl.close();

  //write .pl
	ofstream dot_pl((bookshelf_dir+benchmark+".pl").c_str());
	if(!dot_pl.good())
		return;
	dot_pl << "UCLA pl 1.0" <<endl << endl;
	for(vector<unsigned>::iterator PI=PIs.begin() ; PI!=PIs.end() ; ++PI)
	{
		if(pins[ *PI ].name == clock_port)
			continue;
		dot_pl << "\t\t" << pins[ *PI ].name << "\t\t" << pins[ *PI ].x_coord / BOOKSHELF_X_SCALE << "\t\t" << pins[ *PI ].y_coord / BOOKSHELF_Y_SCALE << "\t : N" <<endl;
	}
	for(vector<unsigned>::iterator PO=POs.begin() ; PO!=POs.end() ; ++PO)
		dot_pl << "\t\t" << pins[ *PO ].name << "\t\t" << pins[ *PO ].x_coord / BOOKSHELF_X_SCALE << "\t\t" << pins[ *PO ].y_coord / BOOKSHELF_Y_SCALE << "\t : N" <<endl;
	for(vector<cell>::iterator theCell = cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(theCell->isFixed)
			dot_pl << "\t\t" << theCell->name << "\t\t" << theCell->init_x_coord / BOOKSHELF_X_SCALE << "\t\t" << theCell->init_y_coord / BOOKSHELF_Y_SCALE << "\t : N" << endl;
		else
			dot_pl << "\t\t" << theCell->name << "\t\t" << theCell->init_x_coord / BOOKSHELF_X_SCALE << "\t\t" << theCell->init_y_coord / BOOKSHELF_Y_SCALE << "\t : N" << endl;
	}
	dot_pl.close();

  //write .shapes
	ofstream dot_shapes((bookshelf_dir+benchmark+".shapes").c_str());
	if(!dot_shapes.good())
		return;

	unsigned num_nonrect_nodes=0;
	for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
		if(macros[ theCell->type ].obses.size() > 1)
			num_nonrect_nodes++;

	dot_shapes << "shapes 1.0" <<endl << endl;
	dot_shapes << "NumNonRectangularNodes : " << num_nonrect_nodes << endl <<endl;
	for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(!theCell->isFixed || macros[ theCell->type ].obses.size() <= 1)
			continue;
		int cnt=0;
		dot_shapes << theCell->name << " : " << macros[ theCell->type ].obses.size() << endl;
		for(vector<rect>::iterator theRect=macros[ theCell->type ].obses.begin() ; theRect != macros[ theCell->type ].obses.end() ; ++theRect, ++cnt)
			dot_shapes << "		Shape_"<< cnt << " " 
			<< ((unsigned)(theCell->init_x_coord + (unsigned)(theRect->xLL * static_cast<double>(LEFdist2Microns))))/BOOKSHELF_X_SCALE << " " 
		  << ((unsigned)(theCell->init_y_coord + (unsigned)(theRect->yLL * static_cast<double>(LEFdist2Microns))))/BOOKSHELF_Y_SCALE << " " 
			<< ((unsigned)((theRect->xUR - theRect->xLL) * static_cast<double>(LEFdist2Microns)))/BOOKSHELF_X_SCALE << " " << rowHeight/BOOKSHELF_Y_SCALE<< endl ;
	}
	dot_shapes.close();

	return;
}

void circuit::convert_pl_to_def(const string thePlacer, const string identifier)
{
	const double BOOKSHELF_X_SCALE = 760;
	const double BOOKSHELF_Y_SCALE = rowHeight / 9;

	ofstream dot_def((benchmark+"_"+thePlacer+".def").c_str());
	if(!dot_def.good())
		return;
	dot_def << "VERSION 5.7 ; " << endl;
	dot_def << "DESIGN "<< benchmark << " ;" <<endl << endl;
	dot_def << "COMPONENTS " << cells.size() << " ;" << endl;
	dot_def << fixed;

  /* parsing .pl file */
  ifstream dot_pl((benchmark+identifier+".pl").c_str());
  if(!dot_pl.good())
  {
    cout << benchmark+identifier+".pl" << " does not exist. exiting .." <<endl;
    exit(1);
  }

	string tmpStr;
  do {
    dot_pl >> tmpStr;
  } while(cell2id.find(tmpStr)==cell2id.end());

  bool isMovable=true;
  while(!dot_pl.eof())//for(unsigned i=0;i<cells.size()+PIs.size()+POs.size()-1;i++)
  {
    if(!isMovable)
      dot_pl >> tmpStr;

		if(pin2id.find(tmpStr)!=pin2id.end())
		{
			double x_coord, y_coord;
			dot_pl >> x_coord >> y_coord >> tmpStr >> tmpStr;
		}
		else
		{
			map<string, unsigned>::iterator it=cell2id.find(tmpStr);
			if(it==cell2id.end())
			{
				cout << "can't find a cell " << tmpStr <<". exiting .." <<endl;
				exit(1);
			}
			double x_coord, y_coord;
			dot_pl >> x_coord >> y_coord >> tmpStr >> tmpStr;
			dot_def << "   - "<< cells[it->second].name << " " << macros[cells[it->second].type].name << endl;
			if(cells[it->second].isFixed)
				dot_def << "      + FIXED ( " << cells[it->second].init_x_coord << " " << cells[it->second].init_y_coord << " ) N ;" << endl;
			else
				dot_def << "      + PLACED ( " << x_coord * BOOKSHELF_X_SCALE << " " << y_coord * BOOKSHELF_Y_SCALE << " ) N ;" <<endl;
		}
    dot_pl >> tmpStr;
    if(tmpStr=="/FIXED")
      isMovable=false;
    else
      isMovable=true;
  }
  dot_pl.close();
	dot_def << "END COMPONENTS" << endl << endl;
	dot_def << "END DESIGN" << endl;
	dot_def.close();
}

void circuit::copy_init_to_final()
{
  for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		theCell->x_coord = theCell->init_x_coord;
		theCell->y_coord = theCell->init_y_coord;
	}

	return;
}

void circuit::read_ops(const string &input)
{
	cout << "  .ops file       : "<< input <<endl;
  ifstream dot_ops(input.c_str());
  if (!dot_ops.good())
	{
    cerr << "read_ops:: cannot open `" << input << "' for reading." << endl;
		cerr << "the script will use the original netlist for evaluation." <<endl;
		return;
	}
	string temp, arg1, arg2;
	dot_ops >> temp;
	while(!dot_ops.eof())
	{
		if(temp == "disconnect_pin")
		{
			dot_ops >> arg1;
			assert(pin2id.find(arg1) != pin2id.end());
			pin *thePin = &pins[ pin2id[ arg1 ] ];
			// NOTE: the operation is allowed for flop input pins
			assert(thePin->isFlopCkPort);
			net *theNet= &nets[ thePin->net ];
			bool found=false;
			for(vector<unsigned>::iterator it=theNet->sinks.begin(); it!=theNet->sinks.end(); ++it)
			{
				if(*it == thePin->id)
				{
					found=true;
					theNet->sinks.erase(it);
					break;
				}
			}
			// NOTE: flop's input must be found in the sink list. cannot be a source of a net.
			assert(found);
			thePin->net=numeric_limits<unsigned>::max();
		}
		else if(temp == "connect_pin")
		{
			dot_ops >> arg1 >> arg2;
			assert(pin2id.find(arg1) != pin2id.end());
			assert(net2id.find(arg2) != net2id.end());
			pin *thePin = &pins[ pin2id[ arg1 ] ];
			// NOTE: the operation is allowed for flop input pins
			assert(thePin->isFlopCkPort);
			assert(thePin->net == numeric_limits<unsigned>::max());
			nets[ net2id[ arg2 ] ].sinks.push_back( thePin->id );
			thePin->net = net2id [ arg2 ];
		}
		else 
		{
			cerr << "read_ops:: keyword "<< temp << " is not defined." << endl; 
			exit(1);
		}
		dot_ops >> temp;
	}

	cout << endl << "Sanity checking .ops .." <<endl;
  cout << "--------------------------------------------------------------------------------" <<endl;
	bool passed=true;
	for(vector<pin>::iterator thePin = pins.begin() ; thePin != pins.end() ; ++thePin)
	{
		if(!thePin->isFlopCkPort)
			continue;
		// if a pin is FF's clk pin, sanity check for: 
		// (a) the pin shouldn't be floating
		// assert(thePin.net != numeric_limits<unsigned>::max());
		if(thePin->net == numeric_limits<unsigned>::max())
		{
			cerr << "  The pin "<< thePin->name << " is floating." <<endl;
			passed=false;
		}
		// (b) the clock signal must be fed via one of LCB
		// assert(cells[ pins[ nets[ thePin->net ].source ].owner ].isLCB);
		else if(!cells[ pins[ nets [ thePin->net ].source ].owner ].isLCB)
		{
			cerr << "  The pin "<< thePin->name << " is not connected to a LCB." <<endl;
			passed=false;
		}
	}
	if(!passed) exit(1);

	for(vector<cell>::iterator theCell = cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(!theCell->isLCB)
			continue;
		// if a cell is LCB, it shouldn't drive more than MAX_LCB_FANOUTS
		unsigned outpin = theCell->ports["o"];
		net *fanout = &nets[ pins[ outpin ].net ];
		if(fanout->sinks.size() > MAX_LCB_FANOUTS)
			cout << "  The LCB " << theCell->name << " is driving " << fanout->sinks.size() << " FFs ( >" << MAX_LCB_FANOUTS <<" )" <<endl;
	}
	return;
}
