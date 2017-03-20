/*-----------------------------------------------------------------*/
/*  Desc:     A main function to read and evaluate solutions       */
/*                                                                 */
/*  Authors:  Myung-Chul Kim, IBM Corporation (mckima@us.ibm.com)  */
/*            Jin Hu, IBM Corporation (jinhu@us.ibm.com)           */
/*                                                                 */
/*  Created:  03/12/2015                                           */
/*-----------------------------------------------------------------*/

#include "evaluate.h"

int main(int argc, char** argv)
{
  cout << "================================================================================" <<endl;
  cout << "    ICCAD 2015 Incremental Timing-Driven Placement Contest Evaluation Script    " <<endl;
  cout << "    Authors : Myung-Chul Kim, Jin Hu {mckima, jinhu}@us.ibm.com                 " <<endl;
  cout << "================================================================================" <<endl;

  if(argc < 4 || argc > 6)
  {
    cout << "Incorrect arguments. exiting .."<<endl;
    cout << "Usage : iccad2015_evaluation ICCAD15.parm [.iccad2015] [target_util] (optional)[final .def] (optional)[final .ops]" << endl ;
    return 0;
  }
	cout << "Command line : " << endl;
	for(int i=0 ; i<argc ; i++)
		cout << " " << argv[i];
	cout << endl;

  circuit ckt;
	ckt.read_parameters(argv[1]);
  ckt.read_iccad2015_file(argv[2]);
#ifdef BENCHMARK_GEN
	string AcademicPlacer = "ComPLx";
	string Identifier = "-ComPLx";
	ckt.write_bookshelf();
	// insert command line to run the placer here
	//exit(1);
	ckt.convert_pl_to_def(AcademicPlacer, Identifier);
	string final_def=ckt.benchmark+"_"+AcademicPlacer+".def";
	ckt.read_def(final_def, FINAL);
#else
	if(argc >= 5)
	{
		string final_def(argv[4]);
		ckt.read_def(final_def, FINAL);
		if(argc == 6)
		{
			ckt.final_ops=argv[5];
			ckt.read_ops(ckt.final_ops);
		}
	}
	else
		ckt.copy_init_to_final();
#endif
#ifdef DEBUG
  ckt.print();
#endif

  cout << endl;
  cout << "Evaluating the solution file ..                                                 " <<endl;
  cout << "--------------------------------------------------------------------------------" <<endl;

  cout << "Analyzing placement .. "<<endl;
  ckt.measure_displacement();
  cout << "  max displ. (um) : " << ckt.displacement << endl;
  ckt.measure_ABU(BIN_DIM, atof(argv[3]));
  cout << "  ABU penalty     : " << ckt.ABU_penalty <<endl;
  cout << "  alpha           : " << ALPHA  <<endl;
  cout << endl;

  cout << "Analyzing timing .. "<<endl;
  ckt.measure_HPWL();
  bool timer_done = ckt.measure_timing();
  cout << "  HPWL, StWL (um) : " << ckt.total_HPWL << ", " <<ckt.total_StWL << endl;
  cout << "  Scaled StWL     : " << ckt.total_StWL * (1 + ALPHA * ckt.ABU_penalty) << " ( "<< ALPHA * ckt.ABU_penalty*100 << "% )"<<endl;
	cout << "  Clock period    : " << ckt.clock_period << endl;
	if(timer_done)
	{
		cout << "  early WNS, TNS  : " << ckt.eWNS << ", " << ckt.eTNS <<endl;
		cout << "  late  WNS, TNS  : " << ckt.lWNS << ", " << ckt.lTNS <<endl;
	}
	else
		cout << "  WNS, TNS        : Timer failed. The values are not available." <<endl;

  cout << "--------------------------------------------------------------------------------" <<endl;
	cout << endl;
	cout << "Checking legality .. " <<endl;
  cout << "--------------------------------------------------------------------------------" <<endl;
  if(ckt.check_legality())
  	cout << "Placement is LEGAL."<<endl;
	else
  	cout << "Placement is ILLEGAL. see check_legality.log."<<endl;
  cout << "--------------------------------------------------------------------------------" <<endl;

  return 1;
}
