#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "vcd.h"
#include "library.h"
#include "defines.h"
#include "core.h"



// Function prototypes
short  readNetlist_verilog  (FILE*, short, vcd*); // reads circuit netilist and returns number of circuit nodes (nodes-1)

void alloc_init_mem();	// Allocate initial memory

short  extractField (FILE *fp, char *field);	// Extracts a token from input file
void appendPi     (struct inout *headPi, short net); //Forms a linked list of all input nodes
void appendPo     (struct inout *headPo, short net); //Forms a linked list of all output nodes
void aro          (vector <deque<short> > &radjlist, struct inout*, short); // NOT USED
void ReachNodesFromPOi (vector <deque<short> > &radjlist, short POj); // Finds all nodes from Primary OUTPOT POj
void dfs_rvisit   (vector <deque<short> > &radjlist, short u, short op); //Performs DFS visit on the reverse graph to find all
void dfs_fvisit   (vector <deque<struct gate*> > &fadjlist, short u); //Performs DFS visit on the forward graph
void logic_simulation(short,vcd*); //Perfroms logic simulation on the gate_list
void compute_gate_value(struct gate* gateptr); //Computes the gate value
void compute_signal_probability(int step_iteration, long max_iteration, vcd*); // Computes signal probabilities of the circuit
void signal_probability_report(void); //Reports on signal probabilities of the circuit
void system_failure(void); //Computes system failure probability at first clock
void find_TPsort_from_dffi(short nff_index); //finds Topological Sort list of nodes starting from dffi
void compute_D_Probability(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn); //Computes D Probability of the given gate from inputs
void compute_D_Probability_with_variance(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn, float* p0_var, float* p1_var, float* pd_var, float* pdn_var); //Computes D Probability of the given gate from inputs
void arange_gates(void); //Arranges all gates Except DFFs after readNetlist procedure
void arange_gates_extract_feedbacks(void);
void extract_feedbacks(void);
void system_failure_clockC(short clkC); //Computes system failure probability at clock C
void system_failure_clockC_multi_prop(short clkC); //Computes system failure probability at clock C
void system_failure_fpga(short);
void system_failure_simulation(long iteration); // Purpose:  Computes system failure probability at first clock using simulation method (Applying large amount of vectors)
void system_failure_simulation_clockC(long iteration, long clk); // Purpose:  Computes system failure probability at first clock using simulation method (Applying large amount of vectors)
void system_failure_simulation_fpga(long iteration, long clk); // Purpose:  Computes system failure probability at first clock using simulation method (Applying large amount of vectors)
void compute_circuit_delay(bool slack_resizing);
void compute_slack(void);
void initialize_gate_delay(struct gate *gptr1);
void print_slack_info(void);
void resizing1(void);
void resizing2(void);
void resizing3(void);
void compute_vul_reduction(void);
void compute_gate_event_list(struct gate* gateptr);
//void add_event(struct gate* gateptr, float p0, float p1, float pd, float pdn, int event_time);
void add_event(struct gate* gateptr, struct event* eventptr);
void timing_derating_sim(long FI_iteration,long max_iteration);
void timing_derating(void);
void compute_D_gate_event_list(struct gate* gateptr, int targe_gate_node);
void release_all_malloc(void);
void matrix_mult(float** matd, int matdx, int matdy, int matcommonxy, float** mats1, float** mats2);
void matrix_copy(float** matd, float** mats, int matdx, int matdy);
void check_print_release_exit(bool condition, char* message);
void check_print_release_exit(bool condition, char* message1, char* message2);
void print_multi_cycle_results(int clk);
float compute_vomin(float* vomin_values, int input_count);
float compute_vomax(float* vomax_values, int input_count);
string gate_name(struct gate*);
void system_failure_multi_cycle_let(short clkC);
void system_failure_simulation_with_let(long, long, long, vcd*);
void compute_Q_gate_event_list(struct gate*, int);
void compute_Q_Probability(struct gate*, float*, float*, float*, float*);
void compute_q_gate_event_list(struct gate* gateptr);
void reset();
void compute_circuit_delay_from_library(library*);
void migrate_core(Core* core);
void report_netlist();
