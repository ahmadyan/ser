#pragma once
#include "configuration.h"
#include "core.h"
#include "defines.h"
class SEU
{
	float* S_sim;			//System failure probability vector obtained from simulation (for POs only), multi cycle
	float* S_sim_var;		//Variances of system failure probability vector obtained from simulation
	float* S_sim_ff;		//System failure probability if we choose only DFFs at fault site only at first cycle
	float* S_sim_ff_po;		//System failure probability if we choose both POs DFFs at fault site only at first cycle
	float* S_sim_po;		//System failure probability if we choose only POs at fault site only at first cycle

	void compute_D_Probability(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn);
public:
	SEU(Core* core);
	~SEU(void);

	void system_failure_simulation_MultiCycle_LogicalDerating(configuration* config, Core* core, FILE*);
	void report(Core* core, FILE* resultfp);
	
};

