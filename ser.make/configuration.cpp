#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include "configuration.h"

#ifdef _WIN32
void srandom(int seed) {srand(seed);} //comment this line for SUN systems
int random() { return rand();}		//comment this line for SUN systems
#endif


configuration::configuration(void){
		//defaults:
		simulation_enable = false ;
		alg = heuristic_wave_flopping ;
		debug=false;
		verbose=false;
		use_technoloy_library = false ;
		use_vcd = false ;
		input_synopsys_style = false;
		t_hold=30; t_setup=18; //nan-45nm
		// tsu=18, th=18; for tsmc250
		seu_pulse_width = 100; // 70 (0.125pC), 170(0.15pC), 280 (0.175pC)
		variance_alarm = false;
		technology = 0 ;
		clk=20 ;
		MAX = 20000 ;
		mbu_enable = false ;
		num_mbu = 2;
		fpga_enable=false;

		output_file_name = (char*) malloc(strlen("ser_report.txt")+1);
		strcpy(output_file_name, "ser_report.txt");

		
		max_iteration =100;
		sp_iteration=10000; // 100000, 100000
   		vector_iteration=1000;
		max_derating_iteration=1000;
		step_iteration = 100; // 100 is minimum iteration for 0.05 error tolerance
		clock=10;

}


configuration::~configuration(void)
{
}
