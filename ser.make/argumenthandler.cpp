#include "argumenthandler.h"


ArgumentHandler::ArgumentHandler(void){}


ArgumentHandler::~ArgumentHandler(void){}


void ArgumentHandler::parse(int argc, char *argv[], configuration* config){
	int c;
	char opt_errors_encountered=0;	
	while (1){
		/*
#ifdef __linux__
		int option_index = 0;

		static struct option long_options[] =
		{
			{"input"    , 1, 0, 'i'},
			{"lib"      , 0, 0, 'l'},
			{"vcdname"  , 0, 0, 'v'},
			{"output"   , 0, 0, 'o'},
			{"clkperiod", 0, 0, 'c'},
			{"help"     , 0, 0, 'h'},
			{"debug"    , 0, 0, 'd'},
			{0, 0, 0, 0}  
		};
		// on how to use getopt: http://www.gnu.org/s/libc/manual/html_node/Getopt.html
		c = getopt_long (argc, argv, "wsi:l:v:o:c:h:d", long_options, &option_index);
#else
*/
		c = getopt      (argc, argv, "wmfsi:b:l:v:o:c:h:d:a:");
//#endif

		if (c == -1) break;     /* no more args */

		switch (c){
		case 'i':
			config->input_file_name = (char*) malloc(strlen(optarg)+1);
			strcpy(config->input_file_name, optarg);
			break;

		case 'b':
					config->input_fault_list = (char*) malloc(strlen(optarg)+1);
					strcpy(config->input_fault_list, optarg);
					break;

		case 'l':
			config->use_technoloy_library=true;
			config->library_file_name = (char*) malloc(strlen(optarg)+1);
			strcpy(config->library_file_name, optarg);
			break;

		case 'v':
			config->use_vcd = true ;
			config->vcd_file_name = (char*) malloc(strlen(optarg)+1);
			strcpy(config->vcd_file_name, optarg);
			break;

		case 'o':
			config->output_file_name = (char*) malloc(strlen(optarg)+1);
			strcpy(config->output_file_name, optarg);
			break;

		case 'a':
			config->alg_name = (char*) malloc(strlen(optarg)+1);
			strcpy(config->alg_name, optarg);
			cout << "op2="<< string(config->alg_name) << "  " <<string(config->alg_name).compare("maze") <<   endl ;
			if( string(config->alg_name).compare("maze")==0){
				cout << "maze!" << endl ;
				config->alg = maze ;
			}else{
				config->alg = heuristic_wave_flopping ;
			}
			//free(config->alg_name);
			break;

		case 'c':
			try {
				config->clk = boost::lexical_cast<int>(optarg);
				cout << "circuit clock period is " << config->clk << endl ;
				cout << "warning: this is only used for reading vcd signals, the actual clock period for simulating circuit will be extracted from critical path." << endl ;
			} catch (boost::bad_lexical_cast &e) {
				cerr << "Error: invalid clock period (-c) " << endl ;
				exit(1);
			}
			break;

		case 'h':
			print_help(argv[0]);
			break;
		case 's':
			config->input_synopsys_style=true;
			break;

		case 'f':
			config->fpga_enable=true;
			break;

		case 'w':
			config->verbose=true;
			break;
		case '?':
			opt_errors_encountered=1;
			break;

		case 'm':
			config->simulation_enable=true;
			break;

		
		case 'd':
			config->debug=true;
			break;
		default:
			opt_errors_encountered=1;
			/* unreachable */
			break;
		}
	}

	if(opt_errors_encountered){
		print_help(argv[0]);
		exit(1);
	}
}


void ArgumentHandler::print_help(char *nam){
#ifdef __linux__ 
	printf(
		"Usage: %s [OPTION]... [CIRCUIT] [LIBRARY] [VCDFILE] [CLK_PERIOD]\n\n"
		"  -i, --input=circuit      specify input circuit (verilog) filename\n"
		"  -l, --lib=library        specify technology library filename\n"
		"  -v, --vcdname=FILE       specify input signal activity in vcd format filename\n"
		"  -o, --output=output      specify output filename to dump report\n"
		"  -c, --clkperiod=clk      specify circuit clock period in nanoseconds\n"
		"  -h                       display this help then exit\n\n"

		"Note that VCDFILE and FSTFILE are optional provided the\n"
		"--vcdname and --fstname options are specified.\n\n"
		"Report bugs to <bybell@nc.rr.com>.\n",nam);
#else
	printf(
		"soft-error-rate estimation analyzer\n"
		"(C) 2010 Dependable System Lab (dsl.ce.sharif.edu)\n\n"
		"Usage: %s [OPTION]... [CIRCUIT] [LIBRARY] [VCDFILE] [CLK_PERIOD]\n\n"
		"  -i circuit              specify input circuit (verilog) filename\n"
		"  -l library              specify technology library filename\n"
		"  -v vcd_file             specify input signal activity in vcd format filename\n"
		"  -o output               specify output filename to dump report\n"
		"  -c clk                  specify circuit clock period in nanoseconds\n"
		"  -h                      display this help then exit\n\n"

		"Note that VCDFILE and CLK_PERIOD are optional provided the\n"
		"--vcdname and --clkperiod options are specified.\n\n"
		"Report bugs to <ahmadyan@ce.sharif.edu>.\n"
		"               <asadi@sharif.edu>.\n",nam);
#endif

	exit(0);
}

/* simple init*/
void ArgumentHandler::initial_checking(configuration* config){
	//Chesk if long int contains at least 32 bits
	if (sizeof(long)<4){
		short chert_num;
		cout << endl << "ATTENTION: size of long is not 32-bits!" << endl;
		cout << "Press Any Key  (Num+Enter) :-)";
		cin >> chert_num;
	}
	cout << endl << "Transition fault propagation to all reachable outputs!";
	cout << endl << "Maximum number of nets supported: " << config->MAX;
	cout << endl << "Please change MAX for larger circuits by adding -max option in ser";
	cout << endl;

	if(config->use_technoloy_library){
		cout << "[init] liberty is enabled. cell information will be provided through liberty technology file." << endl ;
	}else{
		cout << "[init] default library (tsmc-0.25u) will be used for cell information." << endl ;
		config->t_setup = 18;
		config->t_hold = 18;
	}

	if(config->use_vcd){
		cout << "[init] VCD is enabled. signal values will be provided through vcd file." << endl ;
	}else{
		cout << "[init] signal values will be assigned randomly." << endl ;
	}
	if(config->input_synopsys_style){
		cout << "[init] synopsys style input netlist is selected." << endl ;
	}

	switch(config->alg){
		case maze:
			cout << "[init] static analyze selected algorithm : maze" << endl ;
			break;
		case heuristic_dsn_10:
			cout << "[init] static analyze selected algorithm : heuristic_dsn_10" << endl ;
			cout << "[init] we used this stuff for dsn-2010 paper, why you want to use it again?" << endl ;
			break;
		case heuristic_wave_flopping:
			cout << "[init] static analyze selected algorithm : wave_flopping (heuristic)" << endl ;
			break;
		default:
			cout << "[init] static analyze algorithm not defined." << endl ;
	}
}
