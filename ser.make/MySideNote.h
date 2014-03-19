
struct ser{
	short type;	//0: only gate itself, 1: gate+ common fan-out, 2: gate+ common-fan-in 3:gate+ both common-fanout & common-fanin
	short index;
	float derating;
	
	ser(short t, short i, float v){	type=t; index=i, derating=v; }
};


class MBU
{
	//for DFS alg. in topological sort.
	short* pi; // it is used for dfs algorithm
	short* dfs_fnum; // To detecte finalize number (black) after dfs completion
	short final_time_stamp; // is used for topological sorting number during DFS algorithm
	vector < deque<short> > PI2POlist;          // Include all nodes on the paths from PIi to POj
	vector <char> color;
	
	float* S_sim;			//System failure probability vector obtained from simulation (for POs only), multi cycle
	float* S_sim_var;		//Variances of system failure probability vector obtained from simulation
	float* S_sim_ff;		//System failure probability if we choose only DFFs at fault site only at first cycle
	float* S_sim_ff_po;		//System failure probability if we choose both POs DFFs at fault site only at first cycle
	float* S_sim_po;		//System failure probability if we choose only POs at fault site only at first cycle
	
	vector< vector<ser> > S_sim_vec ;
	vector< vector<ser> > S_mlet_vec;
	
	void compute_D_Probability(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn);
	void find_TPsort_from_dffi(short,Core*,configuration*);
	void dfs_fvisit (vector <deque<struct gate*> > &fadjlist, short u, Core* core);
	void compute_D_gate_event_list(struct gate* gateptr, int targe_gate_node_1, int target_gate_node_2, configuration* config, Core* core);
	void compute_D_gate_event_list(struct gate* gateptr, int targe_gate_node_1, configuration* config, Core* core);
	float compute_vomin(float* vomin_values, int input_count);
	float compute_vomax(float* vomax_values, int input_count);
public:
	MBU(Core* core);
	~MBU(void);
	
	//dsn10
	void system_failure_simulation_multicycle_LET(configuration* config, Core* core, FILE* resultfp, FaultGenerator* fg, FaultSiteGenerator* fsg);
	void system_failure_analysis_multicycle_LET(configuration* config, Core* core, FILE* resultfp, FaultGenerator* fg, FaultSiteGenerator* fsg);
	
	//date10
	void system_failure_simulation_MultiCycle_LogicalDerating(configuration* config, Core* core, FILE*);
	void system_failure_simulation_SingleCycle_LogicalDerating(configuration* config, Core* core, FILE* );
	void system_failure_analysis_SingleCycle_LET(configuration* config, Core* core, FILE* resultfp);
	void system_failure_analysis_SingleCycle_LET_seu(configuration* config, Core* core, FILE* resultfp);
	void system_failure_simulation_SingleCycle_LET(Core* core, configuration* config, FILE* resultfp);
	
	void report(Core* core, FILE* resultfp);
	void compute_gate_event_list(struct gate* gateptr, Core* core, configuration* config);
	void inject_fault(struct gate *gateptr, int time, int width);
	void inject_fault_analytical(struct gate *gateptr,int width);
};
