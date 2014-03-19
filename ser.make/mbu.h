#pragma once
//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>	
#include "configuration.h"
#include "core.h"
#include "defines.h"
#include "faultgen.h"
#include "faultsite.h"

struct ser{
	short type;	//0: only gate itself, 1: gate+ common fan-out, 2: gate+ common-fan-in 3:gate+ both common-fanout & common-fanin
	short index;
	float derating;

	ser(short t, short i, float v){	type=t; index=i, derating=v; }
};


//using namespace boost;
//using namespace std;
//typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;
class MBU
{
	//for DFS alg. in topological sort.
	short* pi; // it is used for dfs algorithm
	short* dfs_fnum; // To detecte finalize number (black) after dfs completion
	short final_time_stamp; // is used for topological sorting number during DFS algorithm

	float* S_sim;			//System failure probability vector obtained from simulation (for POs only), multi cycle
	float* S_sim_var;		//Variances of system failure probability vector obtained from simulation
	float* S_sim_ff;		//System failure probability if we choose only DFFs at fault site only at first cycle
	float* S_sim_ff_po;		//System failure probability if we choose both POs DFFs at fault site only at first cycle
	float* S_sim_po;		//System failure probability if we choose only POs at fault site only at first cycle
	
	float* S_sim_clk;//[MAX]; //System failure probability vector obtained from simulation

	float** SatCK;//[MAX][MAX_CLK];  //SatCK[i][j] is the failure probability of the i'th gate in the gate_list at cycle j-1
	float** S1_to_CK;//[MAX][MAX_CLK];
	
	vector< vector<float> > reachable_FF_CK ;
	vector< vector<float> > reachable_PO_CK ;
	
	vector< vector<ser> > S_sim_vec ;
	vector< vector<ser> > S_mlet_vec;

	void compute_D_Probability(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn);
	void topological_sorting(Core* core,configuration* config, short source1, short source2);
	void dfs_fvisit (vector <deque<struct gate*> > &fadjlist, short u, Core* core);
	void compute_gate_event_list(struct gate* gateptr, int targe_gate_node, int target_gate_node2, configuration* config, Core* core);
	void compute_gate_event_list(struct gate* gateptr, Core* core, configuration* config);
	float compute_vomin(float* vomin_values, int input_count);
	float compute_vomax(float* vomax_values, int input_count);
	void met_masking_effect(event* e);
	void dump_the_gate(gate* g);
public:
	MBU(Core* core, configuration* config);
	~MBU(void);

	void system_failure_simulation(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp);
	void system_failure_simulation_single(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp);
	void system_failure_simulation_report_ff_po(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp);
	void system_failure_analysis(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp);
	void system_failure_analysis_single(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp);
	void system_failure_simulation_single_cycle(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp);
	void system_failure_analysis_single_cycle(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp);
	bool fault_simulation_set_logic(Core* core, configuration* config, FILE* resultfp, int session, int ff_index, char c);
	bool fault_simulation_met_logic(Core* core, configuration* config, FILE* resultfp, int session, int ff_index, int ff_index2, char c);
	bool fault_simulation_set(Core* core, configuration* config, FILE* resultfp, int session, int ff_index, char c);
	bool fault_simulation_met(Core* core, configuration* config, FILE* resultfp, int session, int ff_index, int ff_index2, char c);
	void extract_TCAD_motivation_result(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE*);
	
	void report(Core* core, FILE* resultfp);
	void statistical_fault_injection(struct gate *gateptr, int time, int width);
	void analytical_fault_injection(struct gate *gateptr,int width);
};

