
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <boost/lexical_cast.hpp>

#include "library.h"
#include "vcd.h"
#include "main.h"
#include "defines.h"
#include "mbu.h"
#include "fpga.h"
#include "core.h"
#include "argumenthandler.h"
#include "configuration.h"
#include "seu.h"
#include "faultgen.h"
#include "faultsite.h"
using namespace std;


FILE *resultfp;		// Output result file
configuration* config;


void strreverse(char* begin, char* end);
void itoa(int value, char* str, int base);
	
		/**
	 * Ansi C "itoa" based on Kernighan & Ritchie's "Ansi C"
	 * with slight modification to optimize for specific architecture:
	 */
	void strreverse(char* begin, char* end) {
		char aux;
		while(end>begin)
			aux=*end, *end--=*begin, *begin++=aux;
	}
	void itoa(int value, char* str, int base) {
		static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
		char* wstr=str;
		int sign;
		div_t res;
	
		// Validate base
		if (base<2 || base>35){ *wstr='\0'; return; }
	
		// Take care of sign
		if ((sign=value) < 0) value = -value;
	
		// Conversion. Number is reversed.
		do {
			res = div(value,base);
			*wstr++ = num[res.rem];
		}while(value=res.quot);
		if(sign<0) *wstr++='-';
		*wstr='\0';
	
		// Reverse string
		strreverse(str,wstr-1);
	}

// Function  main
int main(int argc, char *argv[]){
	FILE *fp;
	
	//----------------------------------------------------------------------------
	//	Handling input arguments and building up configuration class.
	//----------------------------------------------------------------------------
	config = new configuration();
	ArgumentHandler* argHandler = new ArgumentHandler();
	argHandler->parse(argc, argv, config);
	argHandler->initial_checking(config);



	//----------------------------------------------------------------------------
	//	Creating and loading technology library
	//----------------------------------------------------------------------------
	library* lib = new library();
	if(config->use_technoloy_library){
		lib->read_library(config->library_file_name);
		//lib->analyze();
	}

	//----------------------------------------------------------------------------
	//	loading VCD dump for application derating
	//----------------------------------------------------------------------------
	vcd* VCD  = new vcd();
	if( config->use_vcd ){
		VCD->read_vcd(config->vcd_file_name);
		VCD->setClk(config->clk);
		if(config->debug) VCD->dump();
	}
	

	//----------------------------------------------------------------------------
	//	Core initialization, reading netlist, initial arrangement, etc.
	//----------------------------------------------------------------------------
	fp = fopen(config->input_file_name, "r");    
	resultfp = fopen(config->output_file_name, "w");
	// Exit if error in opening file
	if(fp==NULL) {
		cout << "input netlist file not found" << endl;
		exit(1);
	}
	
	Core* core = new Core(config);

	utils::tick();
		// Read netlist and generate a graph of the circuit
		if(config->fpga_enable){	// the input netlist is a fpga, blif format
			fpga* f = new fpga(config);
			f->read_blif(fp, core);
			f->convert_blif_to_core(resultfp, VCD, core);
		}else{				// the input netlist for asic in verilog format
			core->readNetlist_verilog(fp, VCD, config, resultfp);
		}
		//core->report_netlist();
		//alloc_init_mem();	// Allocate initial memory
		core->gate_numbers_noff = core->gate_numbers - core->dff_num;
		config->max_iteration = 2<<(core->numPi+core->dff_num-1);
		core->arange_gates();
		
			
		if(config->verbose){
			cout << "Number of inputs:" << core->numPi << endl;
			fprintf(resultfp,"\nNumber of inputs: %d \n", core->numPi);
			cout << "Number of outputs:" << core->numPo << endl;
			fprintf(resultfp,"Number of outputs: %d \n", core->numPo);
			cout << "Number of flip-flops:" << core->dff_num << endl;
			fprintf(resultfp,"Number of flip-flops: %d \n", core->dff_num);
			cout << "Number of gates (including FFs):" << core->gate_numbers << endl;
			fprintf(resultfp,"Number of gates (including FFs): %d \n", core->gate_numbers);
		}
	utils::tock("extract netlist and arrange it", resultfp);
   

	//----------------------------------------------------------------------------
	//	Computing SPs 
	//----------------------------------------------------------------------------
	utils::tick();
		core->compute_signal_probability(config,VCD,resultfp);
		//core->signal_probability_report();
	utils::tock("signal probability", resultfp);


	//----------------------------------------------------------------------------
	//	Compute circuit delay from either default (0.25u) technology or the library
	//	file provided by the user
	//----------------------------------------------------------------------------
	// Computing gate delay ( based on technology ) and circuit delay.
	if(config->use_technoloy_library){
		core->compute_circuit_delay_from_library(lib);
	}else{
		core->compute_circuit_delay(false);
	}
	config->tperiod = (int)core->max_delay + config->t_setup + config->t_hold;
	fprintf(resultfp, "\n Circuit period = %d \n", config->tperiod);
	delete lib;


	//simulate all gates!
	core->sim_gate_numbers = core->gate_numbers;

	//----------------------------------------------------------------------------
	//	Performing monte carlo simulation for calculating system soft error rate
	//  this is for Single event transint only
	//----------------------------------------------------------------------------
	if(config->simulation_enable){
		utils::tick();
		SEU* seu = new SEU(core);
		seu->system_failure_simulation_MultiCycle_LogicalDerating(config,core,resultfp);
		seu->report(core,resultfp);
		delete seu;
		utils::tock("Simulation", resultfp);
	}

	
	// DSN-2011 & DATE-2010 MBU code:
	MBU* mbu = new MBU(core, config);
	FaultGenerator* fg = new FaultGenerator(core);
	//FaultSiteGenerator* fsg = new FaultSiteGenerator(core, 0);
	FaultSiteGenerator* fsg = new FaultSiteGenerator(core, config->input_fault_list);
	//FaultSiteGenerator* fsg2= new FaultSiteGenerator(core, 1);
	//cout <<"done!" << endl ;
	//fsg->reportFaultSiteCorrelation(core, fsg->faultSiteList, fsg2->faultSiteList);
	//simulate all gates!
	core->sim_gate_numbers = core->gate_numbers;
	config->clock=1; //single cycle
	utils::tick();
		//mbu->system_failure_simulation_single_cycle(core, config, fg, fsg, resultfp);
		//mbu->system_failure_simulation(core, config, fg, fsg, resultfp);
		//mbu->system_failure_simulation_report_ff_po(core, config, fg, fsg, resultfp);
		//mbu->system_failure_simulation_single(core, config, fg, fsg, resultfp);
	utils::tock("MBU-Simulation", resultfp);

	utils::tick();
		//mbu->system_failure_analysis_single_cycle(core, config, fg, fsg, resultfp);
		//mbu->system_failure_analysis(core, config, fg, fsg, resultfp);
		//mbu->system_failure_analysis_single(core, config, fg, fsg, resultfp);
	utils::tock("MBU-Analytical", resultfp);


	//Schatter Graph:
	mbu->extract_TCAD_motivation_result(core, config, fg, fsg,resultfp);
	delete mbu ;

    
	//
	//	These codes include simulation code for MLET paper (DSN'2010).
	//	First it runs M.C. Simulation for Multi-Core, then the analytical approach for 1..50 Clock.
	//

	// this line is used for simulated effect of t_period of clock and corresponding seu pulse witdth
	// instead of using fixed pulse width size.
	//seu_pulse_width = (int)(tperiod * 0.25) ;
	//seu_pulse_width = (int)(tperiod * 0.5) ;
	//seu_pulse_width = (int)(tperiod * 0.75) ;
	//fprintf(resultfp, "\n SEU Pulse width= %d \n", seu_pulse_width);
	//cout << "sim_gate_numbers=" << sim_gate_numbers << endl ;
	/*
	for(int k=0;k<100;k++){
	if(config->use_vcd) VCD->randomizeCounter();
	for (int i=1; i<=(numPi); i++){
		if(config->use_vcd){
			cout << i << "(" << VCD->getSignalName(i) << ")->" ;
			for(int j=0;j<50;j++){
				short x = VCD->getNextLogicValue(i); 	
				cout << x ;
			}
			cout << endl ;
		}
	}
	cout << "----------------------------------" << endl;
	}
*/
	/*
	if(config->simulation_enable){
		utils::tick();
		if(config->fpga_enable){
			system_failure_simulation_fpga(vector_iteration, MAX_CLK);
		}else{
			system_failure_simulation_with_let(vector_iteration, step_iteration, MAX_CLK,VCD);
		}
		utils::tock("Mont Carlo Simulation with LET", resultfp);
	}
	
	
	//print_multi_cycle_results(MAX_CLK);
	sim_gate_numbers=gate_numbers;
	utils::tick();
		for(int i=1;i<MAX_CLK;i++){
			reset();
			if(config->fpga_enable){
				system_failure_fpga(i);
			}else{
				system_failure_multi_cycle_let(i);
			}
			print_multi_cycle_results(i);
		}
	utils::tock("SER Static Analyze with LET", resultfp);

	//system_failure_clockC_multi_prop(10);
	
	//system_failure_multi_cycle_let(5);
	//print_multi_cycle_results(5);
	//system_failure_multi_cycle(MAX_CLK_SYS2);
	//print_multi_cycle_results(MAX_CLK_SYS2);

	//system_failure_multi_cycle(MAX_CLK_SYS3);
	//print_multi_cycle_results(MAX_CLK_SYS3);
	if(config->verbose) cout << "sim_gate_numbers=" << sim_gate_numbers << endl ;

	if (sim_gate_numbers!=gate_numbers){
		cout << "Simulated for ONLY " << sim_gate_numbers << endl;
		fprintf(resultfp, "Simulated for ONLY %d \n", sim_gate_numbers);
	}else{
		cout << "Simulated for ALL gates: " << sim_gate_numbers << endl;
		fprintf(resultfp, "Simulated for ALL gates: %d \n", sim_gate_numbers);
	}
	//compute_vul_reduction();

	if (config->variance_alarm == true) fprintf(resultfp, "Variance ALARM \n");
	print_slack_info();
	
	release_all_malloc(); */
	fclose(resultfp);
	//system("PAUSE");
	return 0;
} 
