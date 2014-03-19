#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#include "configuration.h"
#include "library.h"
#include "vcd.h"
using namespace std;



struct inout
{
	short num;
	struct inout *next;
};


struct event
{
	int time;
	//short type; //0:0 , 1:1, 2:D, 3:Dn
	float p0,p1,pd,pdn;
	float q0,q1,qd,qdn;
	float vomin, vomax; //, vimin, vimax;
	int pwi, pwo;
};
// Data structure used for storing gate information
struct gate
{
	deque <struct event*> event_list;
	short input_count;
	short* input_list;
	short* input_values;
	short out;
	short type; // indicates gate's type including NAND(0),AND(1),NOR(2),OR(3),XOR(4),XNOR(5),NOT(6),DFF(7),LUT(8)
	short value;
	long sum_values;
	short is_dff; // if gate is flip-flop, is_dff=1, otherwise is_dff=0
	float signal_probability, sample_signal_probability;
	float epp,epp_var;
	float p0,p1,pd,pdn;
	float q0,q1,qd,qdn;
	bool is_onpath;

	//These variables are used for slack comp., actualy delays
	float gd, gd_min, delay, slack; //gd: gate delay,  delay: delay from input to the gate, slack: gate slack
	float load; //fanout load
	float inputc; //input capacitance; we assume that the input capacitances are the same
	float tplh_fix, tphl_fix; //Intrinsic delay for
	float tplh_load, tphl_load; //Load dependent delay
	float tplh, tphl; //TPLH, TPHL delay
	float rsize; //relative size. Initially rsize=1
	bool is_resized; //Initially is_resized=false
	float area; //W, L


	float derating; // Systematic Derating
	float sderating; // Simulation Derating
	float sample_variance,total_variance;
	long total_iteration;

	float p0_var,p1_var,pd_var,pdn_var; //variances of p0,p1,pd,pdn
	short site_out_level; //number of levels between error site and current gate
	int gate_no, dff_no, po_no;  //po_no is a PO number is po_list
	float vomin, vomax, vimin, vimax;
	//deque <int> event_time;
	//int *event_list
	//short* event_type;

	// look-up table struct, can be defined as a gate when we're dealing with fpga.
	bool is_lut ;
	int lut_size; 
	short* lut ;
	short* fault ;

	vector<gate*> forward_list ;
	vector<gate*> backward_list ;

};
// Data structure used for DFFs
struct dff {
	short d;
	short q;
	//short value;
	//long sum_values;
	struct gate* dgateptr; // pointer to the its equivalent gate
	//float signal_probability;
};

class Core{
public:
	// fadjlist[i] contains  adjacency list of node i (i ranges 1...(nodes-1))
	// fadjlist[i][j] is a pointer to one of adjacent nodes of node i (j ranges 0 .. # of adjlist[i]->size()-1)
	vector < deque<struct gate*> > fadjlist;  // forward graph

	// fadjlist_ptr[i] points to the gate whose output adjacency list is fadjlist[i]
	// fadjlist_ptr[i] is not valid for all i's because every node isn't output of a gate
	deque <struct gate*> fadjlist_ptr;       // a graph to point to the gate in fadjlist

	// radjlist[i] contains reverse adjacency list of node i (i ranges 1...(nodes-1))
	deque <struct event*> event_list_ptr;

	// radjlist[i][j] is node number of one of reverse adjacent nodes of node i (j ranges 0 .. # of rdjlist[i]->size()-1)
	vector < deque<short> > radjlist;           // reverse graph
	
	// color[i] is color of node i
	vector <char> color;                      // Used for DFS search

	// reachOp[i] is list of all reachable outputs from node i
	// Note if ReachNodesFromPOi is called for POj, reachOp is list of all reachable nodes from POj
	vector < deque<short> > reachOp;            // reachable nodes from outputs

	// netlist is a one-to-one mapping between <wires,inputs,outputs> and <node numbers>
	// map's items ranges from 1 to nodes-1
	map <string, short> netlist;                // Used for mapping net names to short

	// PI2POlist is list of all nodes from specific nodes to all outputs or specific output
	// it ranges from 0 to PI2POlist.size()-1
	// In case of primary input PIi, PIi is also added to the list but
	// in case of other inputs (outputs of flip-flops), it is not in the list
	vector < deque<short> > PI2POlist;          // Include all nodes on the paths from PIi to POj

	// gate_list is list of all circuit gates including flip-flops. it ranges from 0 to gate_numbers-1
	// Note that this list is not initially levelized but using arange_gates we arrange all gates (not DFFs)
	deque < struct gate* > gate_list;  // Gate list

	deque < struct gate* > po_list;		// Primary output list
	// dfflist_ptr is list of all flip-flops of the circuit. it ranges from 0 to dff_num-1
	// Note: each gate has a pointer to its equivalent gate
	deque < struct dff* >  dfflist_ptr;  // DFFs list

	struct inout *headPi, *headPo;
	short numPi, numPo;
	short* PI_list;				// used for assigning random values to the inputs during logic simulation
	float* PI_list_sp;				// used for input values signal probabilities !
	short nodes; // Number of nodes in the circuit ** Number of nodes of the circuit is nodes-1
	short gate_numbers; // number of gates in the circuit including flop-flops
	short gate_numbers_noff; //number of gates excluding flip-flops
	short sim_gate_numbers; // the number of gates being simulated
	short dff_num;	  // number of flop-flops in the circuit
	float max_delay;
	
	Core();
	~Core();
	Core(configuration*);
	Core(
		vector < deque<struct gate*> > _fadjlist,	// forward graph
		deque <struct gate*> _fadjlist_ptr,			// a graph to point to the gate in fadjlist
		deque <struct event*> _event_list_ptr,
		vector < deque<short> > _radjlist,			// reverse graph
		map <string, short> _netlist,				// Used for mapping net names to short
		vector < deque<short> > _PI2POlist,			// Include all nodes on the paths from PIi to POj
		deque < struct gate* > _gate_list,			// Gate list
		deque < struct gate* > _po_list,			// Primary output list
		deque < struct dff* >  _dfflist_ptr,		// DFFs list
		
		struct inout* _headPi, 
		struct inout* _headPo,
		short _numPi, 
		short _numPo,
		short* _PI_list,								// used for assigning random values to the inputs during logic simulation
		float* _PI_list_sp,							// used for input values signal probabilities !
		short _nodes,								// Number of nodes in the circuit ** Number of nodes of the circuit is nodes-1
		short _gate_numbers,							// number of gates in the circuit including flop-flops
		short _gate_numbers_noff,					//number of gates excluding flip-flops
		short _sim_gate_numbers,						// the number of gates being simulated
		short _dff_num								// number of flop-flops in the circuit

		);


	void appendPo(struct inout *headPo, short net);
	void appendPi(struct inout *headPi, short net);
	void report_netlist();
	short readNetlist_verilog(FILE *fp, vcd* VCD,configuration* config, FILE* resultfp);
	void arange_gates();
	void compute_signal_probability(configuration* config, vcd* VCD,  FILE*);
	void signal_probability_report(FILE* resultfp);
	short extractField(FILE *fp, char *field);
	void logic_simulation(short run_for_sp, vcd* signals,  configuration*);
	void compute_gate_value(struct gate* gateptr);
	void compute_circuit_delay_from_library(library* lib);
	void compute_circuit_delay(bool slack_resizing);
	void initialize_gate_delay(struct gate *gptr1);
	string gate_name(struct gate* gateptr);
};

