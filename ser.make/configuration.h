#pragma once

#ifdef _WIN32
void srandom(int seed); //comment this line for SUN systems
int random();	//comment this line for SUN systems
#endif


enum algorithm { maze, heuristic_dsn_10, heuristic_wave_flopping };

class fpga_conf{
public:
	int lut_size ;
	int cluster_size ;
	int input_per_cluster ;
	fpga_conf(){
		 lut_size = 4;
		 cluster_size = 1;
		 input_per_cluster = 4 ;
	}
};


class configuration
{
public:
	char *vcd_file_name    , 
		 *alg_name          ,
		 *library_file_name, 
		 *input_file_name  , 
		 *input_fault_list ,
		 *output_file_name ;
	
	bool debug ;
	bool simulation_enable ;
	bool verbose;
	bool use_technoloy_library ;
	bool use_vcd ;
	/*	the verilog netlist format is different between ISCAS'89 testbenches,
	(where the gate output comes first ) and the synthesized netlist from
	synopsys design compiler ( where the gate output comes last). this 
	variable defines we read input from ISCAS or DC. */
	bool input_synopsys_style ;
	int tperiod, t_setup, t_hold ;

	int seu_pulse_width ;
	bool variance_alarm ;
	
	// define process technology, default is 0 (tsmc-0.25um)
	int technology;
	// define clock period in nano seconds.
	int clk;
	algorithm alg;
	int MAX ; // Maximum nodes


	//fpga vairables
	bool fpga_enable ;
	fpga_conf fpga ;

	//mbu variables
	bool mbu_enable ;
	int num_mbu ;
	int mbu_depth ;


	long max_iteration;
    long sp_iteration; 
   	long vector_iteration;
	long max_derating_iteration;
	int step_iteration; // 100 is minimum iteration for 0.05 error tolerance
	
	int clock; //number of clock iteration for simulation or analysis

	configuration(void);
	~configuration(void);
};


