/*---------------------------------------------------------------------------------------------*/
/*  Desc:     Functions to analyze placement density (penalty), timing & displacement          */
/*                                                                                             */
/*  Authors:  Myung-Chul Kim, IBM Corporation (mckima@us.ibm.com)                              */
/*                                                                                             */
/*  Created:  03/12/2014                                                                       */
/*---------------------------------------------------------------------------------------------*/

#include "evaluate.h"

//SPEF: FF
#define CAP_SCALE 1.0e-15
//SPEF: kOhm
#define RES_SCALE 1.0e3
#define _DEBUG

const bool ENABLE_SLICING = true;
const bool DISABLE_INPUT_DRIVERS = false; // not working
const bool USE_UPLACER_SPEF = false;
const bool ENABLE_WRITE_SPEF = true;
#include <iomanip>


/* ******************************************************* */
/*  Desc: measure timing (TNS, WNS) of the given circuit   */
/* ******************************************************* */
bool circuit::measure_timing()
{
  update_pinlocs();
  build_steiner();
  slice_longwires(MAX_WIRE_SEGMENT_IN_MICRON * static_cast<double>(DEFdist2Microns));

  // 1. write .spef, .tau2015, .timing, .ops to run a timer
  string filename = design_name;
  write_tau2015(filename+".tau2015");
  if(ENABLE_WRITE_SPEF) {
    write_incremental_spef(filename+".spef", nets);
  } // end if
  write_timing(filename+".timing");
  
  // 2. run UI-Timer
	char* arguments[5];
	for(int i=0 ; i<5 ; i++)
		arguments[i]=new char[80];
	strcpy(arguments[0], "ui-timer2.0");
	strcpy(arguments[1], (filename+".tau2015").c_str());
	strcpy(arguments[2], (filename+".timing").c_str());

	using namespace uit;
	Timer timer;
	timer.init_iccad2015_timer(5, arguments);
	timer.set_num_threads(8);
	update_timer_FF2LCB_association(&timer);

	// we assume PIs are driven by PI drivers from .sdc file during timer invocation
	for(vector<unsigned>::iterator PI=PIs.begin() ; PI!=PIs.end() ; ++PI)
	{
		pin *thePI = &pins[ *PI ];
		if(thePI->name == clock_port)
			continue;

		macro *theDriverMac = &macros[ thePI->driverType ];
		string driverName = thePI->name+"_drv";
    timer.splice_driver(thePI->name, driverName, theDriverMac->name);

		// NOTE: assume PI drivers are inverters
		timer.set_slew(driverName+":a", EARLY, RISE, thePI->rTran);
		timer.set_slew(driverName+":a", EARLY, FALL, thePI->fTran);
		timer.set_slew(driverName+":a", LATE, RISE, thePI->rTran);
		timer.set_slew(driverName+":a", LATE, FALL, thePI->fTran);
		timer.set_at(driverName+":a", EARLY, RISE, 0.);
		timer.set_at(driverName+":a", EARLY, FALL, 0.);
		timer.set_at(driverName+":a", LATE, RISE, 0.);
		timer.set_at(driverName+":a", LATE, FALL, 0.);
	}

	// 3. read out results
	std::set<string> myEndpoints;

	eWNS=eTNS=0.0;
	lWNS=lTNS=0.0;
	for(vector<pin>::iterator thePin=pins.begin() ; thePin!=pins.end() ; ++thePin)
	{
		if(thePin->type == PI_PIN && thePin->name != clock_port)
		{
			thePin->earlySlk=min(timer.report_slack(thePin->name+"_drv:o", EARLY, RISE), timer.report_slack(thePin->name+"_drv:o", EARLY, FALL)); 
			thePin->lateSlk =min(timer.report_slack(thePin->name+"_drv:o", LATE,  RISE), timer.report_slack(thePin->name+"_drv:o", LATE,  FALL)); 

		}
		else
		{
			thePin->earlySlk=min(timer.report_slack(thePin->name, EARLY, RISE), timer.report_slack(thePin->name, EARLY, FALL)); 
			thePin->lateSlk =min(timer.report_slack(thePin->name, LATE,  RISE), timer.report_slack(thePin->name, LATE,  FALL)); 
		}

		// collect slacks @ timing end points
		if(thePin->type == PO_PIN || (thePin->isFlopInput && !thePin->isFlopCkPort))
		{
#ifdef DEBUG
			cout << thePin->name << " " << thePin->earlySlk << " " << thePin->lateSlk <<endl;
#endif
			eWNS = min(eWNS, thePin->earlySlk);
			lWNS = min(lWNS, thePin->lateSlk);
			eTNS += min(0.0, thePin->earlySlk);
			lTNS += min(0.0, thePin->lateSlk);

			myEndpoints.insert(thePin->name);
		}
	}

	
// #########################################################################	
	
	const bool opt_hide_input_drivers = false;

	const bool opt_show_caps = !true;

	const bool opt_show_time = true;
	const bool opt_show_arrival_times = true;
	const bool opt_show_required_times = true;
	const bool opt_show_slews = !true;
	const bool opt_show_slacks = true;

	const bool opt_show_wdelay = !true;

	const bool opt_show_pin_offset = !true;
	const bool opt_show_cell_core = !true;
		
	const bool opt_show_stwl = !true;

	const bool opt_show_eary = true;
	const bool opt_show_late = false;

	const bool opt_show_only_endpoints = true;

	std::set<string> myPrimaryInputs;
	std::set<string> myInputDriverOutputPins;

	for(int idx : PIs){
		if(pins[idx].name == clock_port)
			continue;
		myPrimaryInputs.insert(pins[idx].name);
		myInputDriverOutputPins.insert(pins[idx].name+"_drv:o");	
	}
	
	bool newline = false;
	//std::ostream & out = cout; 
	std::ofstream ofs ("uitimer.tr", std::ofstream::out);
	std::ostream & out = ofs;
	out << std::setprecision(2);
	out << std::fixed;
	
	cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"<<endl;

	uit::IndexedSet<uit::Node> *alnd =  timer.nodeset_ptr();
	if(!opt_show_cell_core)
	if(!opt_show_stwl)
	for (auto &nd : *alnd) {
		uit::Pin *pn = nd->pin_ptr();
		pin_direction_e pdir =  pn->direction();

		if (opt_hide_input_drivers && myPrimaryInputs.count(pn->name())) {
			continue;
		} // end if

		if (opt_show_only_endpoints && !myEndpoints.count(pn->name())) {
			continue;
		} // end if

		uit::Net *net  = pn->net_ptr();
		
		if(!net)
			continue;		
		
		uit::RCTree *rctree = NULL;
		if(net != NULL)
			rctree = net->rctree_ptr();

		if(myInputDriverOutputPins.count(pn->name()) > 0) {
			string str = pn->name();
			
			str = str.substr(0, str.find("_drv"));
			
			out<<str<<" - " ;
			out<<str;
		} else{
			out<<pn->name()<<" - ";
			if(net != NULL)
				out <<net->name() ;
			else
				out << "NULL";
			if(newline) out << endl << "\t";
		}

		if (opt_show_pin_offset) {
			const pin &p = pins[pin2id[pn->name()]];
			out <<" " << p.x_offset << " " << p.y_offset;			
		} // end if

		if(opt_show_time){
			if (opt_show_arrival_times) {
				out << " at: ";
				if (opt_show_eary) {
					out << nd->at(EARLY, FALL) << " "
						<< nd->at(EARLY, RISE) << (opt_show_late?" ": "");
				}
				if (opt_show_late) {
					out	<< nd->at(LATE, FALL) << " "
						<< nd->at(LATE, RISE);
				}
			}

			if (opt_show_required_times) {
				out << " rat: ";
				if (opt_show_eary) {
					out	<< nd->rat(EARLY, FALL) << " "
						<< nd->rat(EARLY, RISE) << (opt_show_late?" ": "");
				}
				if (opt_show_late) {
					out	<< nd->rat(LATE, FALL) << " "
						<< nd->rat(LATE, RISE);
				}
			} 

			if (opt_show_slacks) {
				out	<< " slack: ";
				if (opt_show_eary) {
					out	<< nd->slack(EARLY, FALL) << " "
						<< nd->slack(EARLY, RISE) << (opt_show_late?" ": "");
				}
				if (opt_show_late) {
					out	<< nd->slack(LATE, FALL) << " "
						<< nd->slack(LATE, RISE);
				}
			}

			if (opt_show_slews) {
				out << " slew: ";
				if (opt_show_eary) {
					out	<< nd->slew(EARLY, FALL) << " "
						<< nd->slew(EARLY, RISE) << (opt_show_late?" ": "");
				}
				if (opt_show_late) {
					out	<< nd->slew(LATE, FALL) << " "
						<< nd->slew(LATE, RISE);
				}
			}
		}
		
		if (opt_show_wdelay) {
			uit::Pin * pinRoot = net->root_pin_ptr(); 
			uit::Node * nodeRoot = pinRoot->node_ptr();
			out << " wdelay: ";
			if (opt_show_eary) {
				out << (nd->at(EARLY, FALL) - nodeRoot->at(EARLY, FALL)) << " "
					<< (nd->at(EARLY, RISE) - nodeRoot->at(EARLY, RISE)) << (opt_show_late?" ": "");
			}
			if (opt_show_late) {
				out << (nd->at(LATE, FALL) - nodeRoot->at(LATE, FALL)) << " "
					<< (nd->at(LATE, RISE) - nodeRoot->at(LATE, RISE));
			}
		}

		if (opt_show_caps) {
			out << " dwcap: ";
			if (opt_show_eary) {
				out << pn->lcap(EARLY, FALL) << " "
					<< pn->lcap(EARLY, RISE) << (opt_show_late?" ": "");
			}
			if (opt_show_late) {				
				out << pn->lcap(LATE, FALL) << " "
					<< pn->lcap(LATE, RISE);
			}
		} // end if
		
		out << "\n";
	} // end for
	
	
	if(opt_show_cell_core){
		
		for(vector<cell>::iterator theCell=cells.begin() ; theCell!=cells.end() ; ++theCell){

			out << theCell->name << " "
				<< " pos: ("
				<< theCell->x_coord << ", " << theCell->y_coord <<  ") (" << (theCell->x_coord+theCell->width) << ", " << (theCell->y_coord+theCell->height) <<") "
				<< " dimension: "
				<< theCell->width << " " << theCell->height << " "
				<< " area: " 
				<< (theCell->width*theCell->height)
				<< "\n";
		}
		
	}

	if(opt_show_stwl)
	for(vector<net>::iterator theNet = nets.begin() ; theNet != nets.end() ; ++theNet){
		out << "stwl: " << theNet->name << " "<<theNet->stwl<<"\n";
	} // end for 
	
	// #########################################################################
	

  // Example: Draw top-three critical paths in the design
	ofstream pathplot((benchmark+"_critical.plt").c_str());
	pathplot << "set term png\n";
	pathplot << "set output \""<< benchmark << "_critical.png\"\n";
	pathplot << "set autoscale fix\n";
	pathplot << "set nokey\n";
	pathplot << "\n";
	pathplot << "plot [:][:] '-' w l 0, '-' w filledcurves 0, '-' w d 5, '-' w d 1, '-' w lp 1\n";
	pathplot << lx << " " << by << endl;
	pathplot << rx << " " << by << endl;
	pathplot << rx << " " << ty << endl;
	pathplot << lx << " " << ty << endl;
	pathplot << lx << " " << by << endl;
	pathplot << "EOF" <<endl <<endl;
	for(vector<cell>::iterator theCell=cells.begin() ; theCell!=cells.end() ; ++theCell)
	{
		if(theCell->isFixed)
		{
			pathplot << theCell->x_coord << " " << theCell->y_coord <<endl;
			pathplot << theCell->x_coord + theCell->width << " " << theCell->y_coord <<endl;
			pathplot << theCell->x_coord + theCell->width << " " << theCell->y_coord + theCell->height <<endl;
			pathplot << theCell->x_coord << " " << theCell->y_coord + theCell->height <<endl;
			pathplot << theCell->x_coord << " " << theCell->y_coord <<endl <<endl;
		}
	}
	pathplot << "EOF" <<endl <<endl;

	for(vector<cell>::iterator theCell=cells.begin() ; theCell!=cells.end() ; ++theCell)
	{
		if(!theCell->isFixed)
			pathplot << theCell->x_coord << " " << theCell->y_coord <<endl;
	}
	pathplot << "EOF" <<endl <<endl;

	for(vector<cell>::iterator theCell=cells.begin() ; theCell!=cells.end() ; ++theCell)
	{
		if(theCell->isLCB)
			pathplot << theCell->x_coord << " " << theCell->y_coord <<endl;
	}
	pathplot << "EOF" <<endl <<endl;
  
	ofstream pathlog((benchmark+"_critical.log").c_str());
  path_trace_vt path_traces;
  timer.report_worst_paths("", 3, path_traces);
	for(unsigned j=0; j<path_traces.size(); ++j) {
		path_trace_rt p = path_traces[j];
		for(PathTraceIter i(p); i(); ++i) 
		{
			if(pin2id.find( i.pin_ptr()->name() ) != pin2id.end())
			{
				pin *thePin=&pins[ pin2id[ i.pin_ptr()->name() ] ];
				
				pathplot << thePin->x_coord << " " << thePin->y_coord << endl;
				pathlog << "path "<< j <<  ": " << thePin->name << " ( " << thePin->x_coord << ", "<< thePin->y_coord
								<< " ) in net " << nets[ thePin->net ].name << " E:" << thePin->earlySlk << " L:" << thePin->lateSlk << endl;
			}

		}
		pathplot << endl;
		pathlog << endl;
	}
	pathplot << "EOF" << endl;
	pathplot.close();
	pathlog.close();
  return true;
}

/* ************************************************************************* */
/*  Desc: update pin locations based on owner cell locations & pin offsets   */
/* ************************************************************************* */
void circuit::update_pinlocs()
{
	for(vector<pin>::iterator thePin = pins.begin() ; thePin != pins.end() ; ++thePin)
	{
		if(thePin->isFixed || thePin->owner == numeric_limits<unsigned>::max())
		{
			thePin->x_coord = thePin->x_coord + thePin->x_offset;
			thePin->y_coord = thePin->y_coord + thePin->y_offset;
		}
		else
		{
			thePin->x_coord = cells[ thePin->owner ].x_coord + thePin->x_offset;
			thePin->y_coord = cells[ thePin->owner ].y_coord + thePin->y_offset;
		}
	}
  return;
}

/* ******************************************************************* */
/*  Desc: call FLUTE and populate nets->fluteTree (pair of loctaions)  */
/* ******************************************************************* */
void circuit::build_steiner()
{
  unsigned steiner_points_cnt=0;
  total_StWL=0.0;

  readLUT();

  for(vector<net>::iterator theNet = nets.begin() ; theNet != nets.end() ; ++theNet)
  {
    // clear theNet->wire_segs before population
    theNet->wire_segs.clear();
		if(theNet->name == clock_port)
			continue;

    unsigned numpins=theNet->sinks.size()+1; 
    // two pin nets
    if(numpins==2)
    {
      double wirelength = fabs(pins[ theNet->source ].x_coord - pins[ theNet->sinks[0] ].x_coord) +
        fabs(pins[ theNet->source ].y_coord - pins[ theNet->sinks[0] ].y_coord);
      total_StWL += wirelength;
			theNet->wire_segs.push_back( make_pair (make_pair(pins[ theNet->source ].name, pins[ theNet->sinks[0] ].name), wirelength) );
	  theNet->stwl += wirelength; // Added by Jucemar
    }

    // otherwise, let's build a FLUTE tree
    else if(numpins > 2)
    {
			map< pair<string, string>, bool> pinpair_covered;
      unsigned *x = new unsigned[numpins];
      unsigned *y = new unsigned[numpins];

      map< pair<unsigned, unsigned>, string > pinmap;  // pinmap : map from pair< x location, y location > to pinName 
      x[0]=(unsigned)(max(pins[ theNet->source ].x_coord, 0.0));
      y[0]=(unsigned)(max(pins[ theNet->source ].y_coord, 0.0));
      pinmap[ make_pair(x[0], y[0]) ] = pins[ theNet->source ].name;

      unsigned j=1;
      for(vector<unsigned>::iterator theSink=theNet->sinks.begin() ; theSink != theNet->sinks.end() ; ++theSink, j++)
      {
        x[j]=(unsigned)(max(pins[ *theSink ].x_coord, 0.0));
        y[j]=(unsigned)(max(pins[ *theSink ].y_coord, 0.0));
        pinmap[ make_pair(x[j], y[j]) ] = pins[ *theSink ].name;
      }
      Tree flutetree = flute(numpins, x, y, ACCURACY);
      delete [] x;
      delete [] y;

      int branchnum = 2*flutetree.deg - 2; 
      for(int j = 0; j < branchnum; ++j) 
      {
        int n = flutetree.branch[j].n;
        if(j == n) continue;

        double wirelength = fabs((double)flutetree.branch[j].x - (double)flutetree.branch[n].x) +
          fabs((double)flutetree.branch[j].y - (double)flutetree.branch[n].y);

				total_StWL += wirelength;
				theNet->stwl += wirelength; // Added by Jucemar
				// any internal Steiner points will have blank names, so assign them to sp_<counter>
				string pin1=pinmap[ make_pair(flutetree.branch[j].x, flutetree.branch[j].y) ]; 
				if(pin1 == "")
				{
					steiner_points_cnt++;
					pin1="sp_"+to_string(static_cast<long long unsigned>(steiner_points_cnt));
					pinmap[ make_pair(flutetree.branch[j].x, flutetree.branch[j].y) ] = pin1;					
				}
				string pin2=pinmap[ make_pair(flutetree.branch[n].x, flutetree.branch[n].y) ]; 
				if(pin2 == "")
				{
					steiner_points_cnt++;
					pin2="sp_"+to_string(static_cast<long long unsigned>(steiner_points_cnt));
					pinmap[ make_pair(flutetree.branch[n].x, flutetree.branch[n].y) ] = pin2;					
				}
				if(pin1 == pin2) // NOTE: this only happens when two pin locations are the same (i.e., critically stacked)
				{
					for(vector<unsigned>::iterator theSink2=theNet->sinks.begin() ; theSink2 != theNet->sinks.end() ; ++theSink2)
					{
						if(pins[ *theSink2 ].name == pin1)
							continue;
						unsigned x=(unsigned)(max(pins[ *theSink2 ].x_coord, 0.0));
						unsigned y=(unsigned)(max(pins[ *theSink2 ].y_coord, 0.0));
						if(flutetree.branch[j].x == x && flutetree.branch[j].y == y)
						{
							if(pinpair_covered[ make_pair(pin1, pins[ *theSink2 ].name) ])
								continue;
							else
							{
								// find another pin's name
								pin2 = pins[ *theSink2 ].name;
								break;
							}
						}
					}
				}
				if(pin1 != pin2)
				theNet->wire_segs.push_back( make_pair( make_pair(pin1, pin2), wirelength ) );
				pinpair_covered[ make_pair(pin1, pin2) ]=true;
      }
			pinpair_covered.clear();
      pinmap.clear();
      free(flutetree.branch);
    }
  }
  cout << "  FLUTE: Total "<< steiner_points_cnt << " internal Steiner points are found." <<endl;
	total_StWL /= static_cast<double>(DEFdist2Microns);

#ifdef DEBUG
  for(vector<net>::iterator theNet=nets.begin() ; theNet != nets.end() ; ++theNet)
    for(vector< pair< pair<string, string>, double > >::iterator theSeg=theNet->wire_segs.begin() ; theSeg != theNet->wire_segs.end() ; theSeg++)
			cout << theSeg->first.first << ", "<< theSeg->first.second << " - "<< theSeg->second <<endl;
#endif

  return;
}

/* ******************************************************** */
/*  Desc: slice long wires into wire segements < threshold  */
/* ******************************************************** */
void circuit::slice_longwires(unsigned threshold)
{
	unsigned org_tot_wire_segments=0;
  unsigned tot_slice_cnt=0;
  unsigned tot_wire_segments=0;
  for(vector<net>::iterator theNet=nets.begin() ; theNet != nets.end() ; ++theNet)
  {
    unsigned orig_wire_seg_cnt=theNet->wire_segs.size();
		org_tot_wire_segments+=orig_wire_seg_cnt;
		for(unsigned i=0 ; i<orig_wire_seg_cnt ; i++)
    {
			pair< pair<string, string>, double > *theSeg = &theNet->wire_segs[i];
      // if the length of this wire segment is longer than the threshold, 
      // let's slice it equally and add proper nodes
      if(theSeg->second  > threshold)
      {
        string start_pin_name=theSeg->first.first;
        string end_pin_name=theSeg->first.second;
        unsigned slice_cnt = ceil(theSeg->second/(double)threshold);
        double sliced_wirelength=theSeg->second/(double)slice_cnt;

        string prev_pin_name=start_pin_name+":1";
        theSeg->first.second=prev_pin_name;
        theSeg->second=sliced_wirelength;
        for(unsigned j=1 ; j<slice_cnt ; j++, tot_slice_cnt++)
        {
          string cur_pin_name=start_pin_name+":"+to_string(static_cast<long long unsigned>(j+1));
          theNet->wire_segs.push_back( make_pair( make_pair(prev_pin_name, cur_pin_name), sliced_wirelength ) );
					prev_pin_name = cur_pin_name;
        }
        theNet->wire_segs[ theNet->wire_segs.size()-1 ].first.second=end_pin_name;
      }
    }
    tot_wire_segments += theNet->wire_segs.size();
  }
  cout << "  Slicing wire segments: " << org_tot_wire_segments << " --> " << tot_wire_segments << " ( < "<< MAX_WIRE_SEGMENT_IN_MICRON << " um )" << endl;
  return;
}

/* ************************************************************* */
/*  Desc: measure max cell displacement from an input placement  */
/* ************************************************************* */
void circuit::measure_displacement()
{
	displacement=0.0;
	for(vector<cell>::iterator theCell=cells.begin() ; theCell!=cells.end() ; ++theCell)
		if(!theCell->isFixed)
	    displacement=max(displacement, fabs(theCell->init_x_coord - theCell->x_coord) + fabs(theCell->init_y_coord - theCell->y_coord));
	
	displacement /= static_cast<double>(DEFdist2Microns);
	return;
}

/* *************************************** */
/*  Desc: measure ABU and density penalty  */
/* *************************************** */
void circuit::measure_ABU(double unit, double targUt)
{
  double gridUnit = unit * rowHeight;
  int x_gridNum = (int) ceil ((rx - lx)/gridUnit);
  int y_gridNum = (int) ceil ((ty - by)/gridUnit);
  int numBins = x_gridNum*y_gridNum;

  cout << "  numBins         : "<<numBins << " ( "<< x_gridNum << " x "<< y_gridNum << " )"<< endl;
  cout << "  bin dimension   : "<< gridUnit << " x " << gridUnit <<endl;

  /* 0. initialize density map */
  vector<density_bin> bins(numBins);
  for(int j=0;j<y_gridNum;j++)
    for(int k=0;k<x_gridNum;k++)
    {
      unsigned binId= j*x_gridNum+k;
      bins[binId].lx=lx + k*gridUnit;
      bins[binId].ly=by + j*gridUnit;
      bins[binId].hx=bins[binId].lx + gridUnit;
      bins[binId].hy=bins[binId].ly + gridUnit;

      bins[binId].hx=min(bins[binId].hx, rx);
      bins[binId].hy=min(bins[binId].hy, ty);

      bins[binId].area=max((bins[binId].hx - bins[binId].lx) * (bins[binId].hy - bins[binId].ly), 0.0);
      bins[binId].m_util=0.0;
      bins[binId].f_util=0.0;
      bins[binId].free_space=0.0;
    }

  /* 1. build density map */
	/* (a) calculate overlaps with row sites, and add them to free_space */
	for(vector<row>::iterator theRow=rows.begin() ; theRow != rows.end() ; ++theRow)
	{
 	 	int lcol=max((int)floor((theRow->origX - lx)/gridUnit), 0);
    int rcol=min((int)floor((theRow->origX + theRow->numSites*theRow->stepX - lx)/gridUnit), x_gridNum-1);
    int brow=max((int)floor((theRow->origY - by)/gridUnit), 0);
    int trow=min((int)floor((theRow->origY + rowHeight - by)/gridUnit), y_gridNum-1);

    for(int j=brow;j<=trow;j++)
      for(int k=lcol;k<=rcol;k++)
      {
        unsigned binId= j*x_gridNum+k;

        /* get intersection */
        double lx=max(bins[binId].lx, (double)theRow->origX);
        double hx=min(bins[binId].hx, (double)theRow->origX + theRow->numSites*theRow->stepX);
        double ly=max(bins[binId].ly, (double)theRow->origY);
        double hy=min(bins[binId].hy, (double)theRow->origY + rowHeight);

        if((hx-lx) > 1.0e-5 && (hy-ly) > 1.0e-5)
        {
          double common_area = (hx-lx) * (hy-ly);
          bins[binId].free_space+= common_area;
					bins[binId].free_space = min(bins[binId].free_space, bins[binId].area);
        }
      }
	}

  /* (b) add utilization by fixed/movable objects */
  for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
  {
		if(macros[ theCell->type ].obses.size() <= 1)
		{
			int lcol=max((int)floor((theCell->x_coord-lx)/gridUnit), 0);
			int rcol=min((int)floor((theCell->x_coord+theCell->width-lx)/gridUnit), x_gridNum-1);
			int brow=max((int)floor((theCell->y_coord-by)/gridUnit), 0);
			int trow=min((int)floor((theCell->y_coord+theCell->height-by)/gridUnit), y_gridNum-1);

			for(int j=brow;j<=trow;j++)
				for(int k=lcol;k<=rcol;k++)
				{
					unsigned binId= j*x_gridNum+k;

					/* get intersection */
					double lx=max(bins[binId].lx, (double)theCell->x_coord);
					double hx=min(bins[binId].hx, (double)theCell->x_coord+theCell->width);
					double ly=max(bins[binId].ly, (double)theCell->y_coord);
					double hy=min(bins[binId].hy, (double)theCell->y_coord+theCell->height);

					if((hx-lx) > 1.0e-5 && (hy-ly) > 1.0e-5)
					{
						double common_area = (hx-lx) * (hy-ly);
						if(theCell->isFixed)
							bins[binId].f_util+=common_area;
						else 
							bins[binId].m_util+=common_area;
					}
				}
		}
		// non-rectangular shapes
		else
		{
			for(vector<rect>::iterator theRect=macros[ theCell->type ].obses.begin() ; theRect != macros[ theCell->type ].obses.end() ; ++theRect)
			{
				int lcol=max((int)floor((theCell->x_coord+(unsigned)(theRect->xLL * static_cast<double>(LEFdist2Microns))-lx)/gridUnit), 0);
				int rcol=min((int)floor((theCell->x_coord+(unsigned)(theRect->xUR * static_cast<double>(LEFdist2Microns))-lx)/gridUnit), x_gridNum-1);
				int brow=max((int)floor((theCell->y_coord+(unsigned)(theRect->yLL * static_cast<double>(LEFdist2Microns))-by)/gridUnit), 0);
				int trow=min((int)floor((theCell->y_coord+(unsigned)(theRect->yUR * static_cast<double>(LEFdist2Microns))-by)/gridUnit), y_gridNum-1);

				for(int j=brow;j<=trow;j++)
					for(int k=lcol;k<=rcol;k++)
					{
						unsigned binId= j*x_gridNum+k;

						/* get intersection */
						double lx=max(bins[binId].lx, (double)theCell->x_coord+(unsigned)(theRect->xLL * static_cast<double>(LEFdist2Microns)));
						double hx=min(bins[binId].hx, (double)theCell->x_coord+(unsigned)(theRect->xUR * static_cast<double>(LEFdist2Microns)));
						double ly=max(bins[binId].ly, (double)theCell->y_coord+(unsigned)(theRect->yLL * static_cast<double>(LEFdist2Microns)));
						double hy=min(bins[binId].hy, (double)theCell->y_coord+(unsigned)(theRect->yUR * static_cast<double>(LEFdist2Microns)));

						if((hx-lx) > 1.0e-5 && (hy-ly) > 1.0e-5)
						{
							double common_area = (hx-lx) * (hy-ly);
							if(theCell->isFixed)
								bins[binId].f_util+=common_area;
							else 
								bins[binId].m_util+=common_area;
						}
					}
			}
		}
  }

  int skipped_bin_cnt=0;
  vector<double> util_array(numBins, 0.0);
  /* 2. determine the free space & utilization per bin */
  for(int j=0;j<y_gridNum;j++)
    for(int k=0;k<x_gridNum;k++)
    {
      unsigned binId= j*x_gridNum+k;
      if(bins[binId].area > gridUnit*gridUnit*BIN_AREA_THRESHOLD)
      {
        bins[binId].free_space-= bins[binId].f_util;
        if(bins[binId].free_space > FREE_SPACE_THRESHOLD*bins[binId].area)
          util_array[binId] = bins[binId].m_util / bins[binId].free_space;
        else
          skipped_bin_cnt++;
#ifdef DEBUG
        if(util_array[binId] > 1.0)
        {
          cout << binId  << " is not legal. "<< endl;
          cout << " m_util: " << bins[binId].m_util << " f_util " << bins[binId].f_util << " free_space: " << bins[binId].free_space<< endl;
          exit(1);
        }
#endif
      }
    }

// Plot --------------------------------------------------------------------
	ofstream plotfile;
	// Plot a the utilization map.
	plotfile.open((benchmark+"_grid.plt").c_str());
    
	plotfile << "set term png\n";
	plotfile << "set output \""<<benchmark << "_grid.png\"\n";
	plotfile << "set autoscale fix\n";
	plotfile << "\n";
	plotfile << 
		"set palette defined ( 0 '#000090',\\\n"
		"                      1 '#000fff',\\\n"
		"                      2 '#0090ff',\\\n"
		"                      3 '#0fffee',\\\n"
		"                      4 '#90ff70',\\\n"
		"                      5 '#ffee00',\\\n"
		"                      6 '#ff7000',\\\n"
		"                      7 '#ee0000',\\\n"
		"                      8 '#7f0000')\n\n"
	;
	
	plotfile << "plot '-' matrix with image t ''\n";

	for (int j = 0; j < y_gridNum; j++) {
		for (int k = 0; k < x_gridNum; k++) {
			const unsigned binId = j*x_gridNum+k;
			plotfile << util_array[binId] << " ";
		} // end for
		plotfile << "\n";
	} // end for

	plotfile.close();

  // -------------------------------------------------------------------------
  bins.clear();
  sort(util_array.begin(), util_array.end());

  /* 3. obtain ABU numbers */
  double abu1=0.0, abu2=0.0, abu5=0.0, abu10=0.0, abu20=0.0;
  int clip_index = 0.01*(numBins-skipped_bin_cnt);
  for(int j=numBins-1;j>numBins-1-clip_index;j--) {
    abu1+=util_array[j];
  }
  abu1=(clip_index) ? abu1/clip_index : util_array[numBins-1];

  clip_index = 0.02*(numBins-skipped_bin_cnt);
  for(int j=numBins-1;j>numBins-1-clip_index;j--) {
    abu2+=util_array[j];
  }
  abu2=(clip_index) ? abu2/clip_index : util_array[numBins-1];

  clip_index = 0.05*(numBins-skipped_bin_cnt);
  for(int j=numBins-1;j>numBins-1-clip_index;j--) {
    abu5+=util_array[j];
  }
  abu5=(clip_index) ? abu5/clip_index : util_array[numBins-1];

  clip_index = 0.10*(numBins-skipped_bin_cnt);
  for(int j=numBins-1;j>numBins-1-clip_index;j--) {
    abu10+=util_array[j];
  }
  abu10=(clip_index) ? abu10/clip_index : util_array[numBins-1];

  clip_index = 0.20*(numBins-skipped_bin_cnt);
  for(int j=numBins-1;j>numBins-1-clip_index;j--) {
    abu20+=util_array[j];
  }
  abu20=(clip_index) ? abu20/clip_index : util_array[numBins-1];
  util_array.clear();

  cout << "  target util     : "<< targUt <<endl;
  cout << "  ABU_2,5,10,20   : "<< abu2 <<", "<< abu5 <<", "<< abu10 <<", "<< abu20 <<endl;

  /* calculate overflow & ABU_penalty */
  abu1=max(0.0, abu1/targUt-1.0);
  abu2=max(0.0, abu2/targUt-1.0);
  abu5=max(0.0, abu5/targUt-1.0);
  abu10=max(0.0, abu10/targUt-1.0);
  abu20=max(0.0, abu20/targUt-1.0);
  ABU_penalty=(ABU2_WGT*abu2+ABU5_WGT*abu5+ABU10_WGT*abu10+ABU20_WGT*abu20)/(double)(ABU2_WGT+ABU5_WGT+ABU10_WGT+ABU20_WGT);
  return;
}

/* ***************************************** */
/*  Desc: measure Half-perimeter Wirelength  */
/* ***************************************** */
void circuit::measure_HPWL()
{
  update_pinlocs();
  double totalX=0.0;
  double totalY=0.0;

  for(vector<net>::iterator theNet=nets.begin() ; theNet != nets.end() ; ++theNet)
  {
    double netMaxX, netMinX;
    double netMaxY, netMinY;
    netMaxX=netMinX=pins[ theNet->source ].x_coord;
    netMaxY=netMinY=pins[ theNet->source ].y_coord;
    for(vector<unsigned>::iterator theSink=theNet->sinks.begin() ; theSink != theNet->sinks.end() ; ++theSink)
    {
      netMaxX=max(netMaxX, pins[ *theSink ].x_coord);
      netMinX=min(netMinX, pins[ *theSink ].x_coord);
      netMaxY=max(netMaxY, pins[ *theSink ].y_coord);
      netMinY=min(netMinY, pins[ *theSink ].y_coord);
    }
    totalX+=netMaxX-netMinX;
    totalY+=netMaxY-netMinY;
  }

  total_HPWL=(totalX+totalY) / static_cast<double>(DEFdist2Microns);
  return;
}

void circuit::write_tau2015(const string &output)
{
	ofstream dot_tau2015(output.c_str());
	if(!dot_tau2015.good())
	{
    cerr << "write_tau2015:: cannot open `" << output << "' for writing." << endl;
		exit(1);
	}
	assert((dot_lib_early != "") && (dot_lib_late != ""));
	dot_tau2015 << dot_lib_early << " " << dot_lib_late << " "<< design_name <<".spef " << dot_verilog;
	if(USE_UPLACER_SPEF){
		cout << " ############ ==> Using spef generated by UPlacer\n";
		dot_tau2015 << "../bin/";
	}
	return;
}

void circuit::write_incremental_spef(const string &output, vector<net> &partial_nets)
{
  ofstream dot_spef(output.c_str());
	if(!dot_spef.good())
	{
    cerr << "write_incremental_spef:: cannot open `" << output << "' for writing." << endl;
		exit(1);
	}
	stringstream feed;
	feed.precision(5);

	time_t rawtime;
	time ( &rawtime );
	string t( ctime(&rawtime) );
	feed<< "*SPEF \"IEEE 1481-1998\"" <<endl;
	feed<< "*DESIGN \"" << design_name << "\"" <<endl;
	feed<< "*DATE \"" << t.substr(0, t.length()-1) <<  "\"" <<endl;
	feed<< "*VENDOR \"ICCAD 2015 Contest\"" <<endl;
	feed<< "*PROGRAM \"ICCAD 2015 Contest Spef Generator\"" <<endl;
	feed<< "*VERSION \"0.0\"" <<endl;
	feed<< "*DIVIDER /" <<endl;
	feed<< "*DELIMITER :" <<endl;
	feed<< "*BUS_DELIMITER [ ]" <<endl;
	feed<< "*T_UNIT 1 PS" <<endl;
	feed<< "*C_UNIT 1 FF" <<endl;
	feed<< "*R_UNIT 1 KOHM" <<endl;
	feed<< "*L_UNIT 1 UH" <<endl << endl;

  map<string, bool> pin_cap_written;
  map<string, double> lumped_cap_at_pin;              /* map from pin name -> cap */

  // 1. calc. lump sum caps from wire segments (PI2-model) + load caps
  for(vector<net>::iterator theNet = partial_nets.begin() ; theNet != partial_nets.end() ; ++theNet)
	{
		theNet->lumped_cap = 0.0;
		if(theNet->name == clock_port)
			continue;

		int cnt=0;
    for(vector< pair< pair<string, string>, double > >::iterator theSeg=theNet->wire_segs.begin();
        theSeg != theNet->wire_segs.end() ; ++theSeg, ++cnt)
    {
			theNet->lumped_cap += theSeg->second / static_cast<double>(DEFdist2Microns) * LOCAL_WIRE_CAP_PER_MICRON;
			lumped_cap_at_pin [ theSeg->first.first ] += theSeg->second  / static_cast<double>(DEFdist2Microns) * LOCAL_WIRE_CAP_PER_MICRON * 0.5;
			lumped_cap_at_pin [ theSeg->first.second ] += theSeg->second / static_cast<double>(DEFdist2Microns) * LOCAL_WIRE_CAP_PER_MICRON * 0.5;
      pin_cap_written [ theSeg->first.first ] = false;
      pin_cap_written [ theSeg->first.second ] = false;
    }
    for(vector<unsigned>::iterator theSink=theNet->sinks.begin() ; theSink != theNet->sinks.end() ; ++theSink)
		{
			pin *thePin=&pins[ *theSink ];
			if(thePin->type == PO_PIN)
			{
				theNet->lumped_cap += thePin->cap;
				lumped_cap_at_pin [ thePin->name ] += thePin->cap;
			}
		}
	}

  // 2. write parasitics 
  for(vector<net>::iterator theNet = partial_nets.begin() ; theNet != partial_nets.end() ; ++theNet)
  {
		// 0. write net name and lumped sum of downstream cap 
		feed << "*D_NET "<< theNet->name << " " << theNet->lumped_cap / CAP_SCALE << endl;

		// 1. write connections
		feed << "*CONN" << endl;
		pin *thePin=&pins[ theNet->source ];
		feed << ((thePin->type != NONPIO_PIN) ? "*P " : "*I ") << thePin->name << ((thePin->type == PI_PIN) ? " I" : " O") <<endl;
    for(vector<unsigned>::iterator theSink=theNet->sinks.begin() ; theSink != theNet->sinks.end() ; ++theSink)
		{
			pin *thePin=&pins[ *theSink ];
			feed << ((thePin->type != NONPIO_PIN) ? "*P " : "*I ") << thePin->name << ((thePin->type == PO_PIN) ? " O" : " I") <<endl;
		}

		// no parasitics bewteen clock source and LCBs
		if(theNet->name == clock_port)
		{
			feed << "*END" <<endl <<endl;
			continue;
		}

		// 2. write wire cap + load cap
		unsigned cnt=1;
		feed << "*CAP" <<endl;
		for(vector< pair< pair<string, string>, double > >::iterator theSeg=theNet->wire_segs.begin() ; 
        theSeg != theNet->wire_segs.end() ; ++theSeg)
		{
			if(!pin_cap_written[ theSeg->first.first ])
			{
				feed << cnt++ << " " << theSeg->first.first << " " << lumped_cap_at_pin[ theSeg->first.first ] / CAP_SCALE<<endl;
				pin_cap_written[ theSeg->first.first ]=true;
			}
			if(!pin_cap_written[ theSeg->first.second ])
			{
				feed << cnt++ << " " << theSeg->first.second << " " << lumped_cap_at_pin[ theSeg->first.second ] / CAP_SCALE<<endl;
				pin_cap_written[ theSeg->first.second ]=true;
			}
		}

		// 3. write wire resistance
		cnt=1;
		feed << "*RES" <<endl;
    for(vector< pair< pair<string, string>, double > >::iterator theSeg=theNet->wire_segs.begin() ; 
        theSeg != theNet->wire_segs.end() ; ++theSeg)
			feed << cnt++ << " " << theSeg->first.first << " " << theSeg->first.second << " " << theSeg->second / static_cast<double>(DEFdist2Microns) * LOCAL_WIRE_RES_PER_MICRON / RES_SCALE<<endl;

		feed << "*END" <<endl <<endl;
  }
  pin_cap_written.clear();
  lumped_cap_at_pin.clear();

	dot_spef << feed.str();
	feed.clear();
	dot_spef.close();
	return;
}

void circuit::write_timing(const string &output)
{
  ofstream dot_timing(output.c_str());
	if(!dot_timing.good())
	{
    cerr << "write_timing:: cannot open `" << output << "' for writing." << endl;
		exit(1);
	}
	stringstream feed;
	feed.precision(5);

	// 1. writing clock period
	feed << "clock "<< clock_port << " " << clock_period << " 50" <<endl;

	// 2. writing actual arrival times
	/* these timing assertions now are unnecessary due to PI drivers' set_slew(), set_at() */
	
	// 3. writing slew
	/* these timing assertions now are unnecessary due to PI drivers' set_slew(), set_at() */

	// 4. writing require arrival times for outputs
	for(vector<unsigned>::iterator PO=POs.begin() ; PO!=POs.end() ; ++PO)
	{
		feed << "rat " << pins[ *PO ].name << " ";
		feed << "0.0 0.0 " << clock_period - pins[ *PO ].delay << " " << clock_period - pins[ *PO ].delay << endl;
	}

	// 5. write load cap 
	/* this is now included in .spef */

	dot_timing << feed.str();
	feed.clear();
  dot_timing.close();
	return;
}

void circuit::update_timer_FF2LCB_association(uit::Timer *timer)
{
	ifstream dot_ops(final_ops.c_str());
  if (!dot_ops.good())
		return;

	string temp, arg1, arg2;
	dot_ops >> temp;

	set<unsigned> unique_modified_nets;
	while(!dot_ops.eof())
	{
		if(temp == "disconnect_pin")
		{
			dot_ops >> arg1;
			assert(pin2id.find(arg1) != pin2id.end());
			pin *thePin = &pins[ pin2id[ arg1 ] ];
			// NOTE: the operation is allowed for flop input pins
			assert(thePin->isFlopCkPort);
			timer->disconnect_pin(arg1);
			unique_modified_nets.insert(thePin->net);
		}
		else if(temp == "connect_pin")
		{
			dot_ops >> arg1 >> arg2;
			assert(pin2id.find(arg1) != pin2id.end());
			assert(net2id.find(arg2) != net2id.end());
			pin *thePin = &pins[ pin2id[ arg1 ] ];
			// NOTE: the operation is allowed for flop input pins
			assert(thePin->isFlopCkPort);
			timer->connect_pin(arg1, arg2);
			unique_modified_nets.insert(net2id[ arg2 ]);
		}
		else 
		{}
		dot_ops >> temp;
	}
	dot_ops.close();

	if(unique_modified_nets.empty())
		return;

	vector<net> modified_nets;
	for(set<unsigned>::iterator it=unique_modified_nets.begin() ; it!=unique_modified_nets.end() ; ++it)
		modified_nets.push_back( nets[*it] );
	unique_modified_nets.clear();

	// write .spef for the modified nets.
	write_incremental_spef("new_lcb_to_ff.spef", modified_nets);
	timer->read_spef("new_lcb_to_ff.spef");
	modified_nets.clear();
	return;
}
