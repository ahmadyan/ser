//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/breadth_first_search.hpp>
//#include <boost/pending/indirect_cmp.hpp>
//#include <boost/pending/integer_range.hpp>
#include "mbu.h"
#include <iostream>
#include <utility>
#include <algorithm> 
#include <vector>
using namespace std;

MBU::MBU(Core* core, configuration* config){
	S_sim = new float[core->gate_numbers];
	S_sim_var = new float[core->gate_numbers];
	S_sim_ff = new float[core->gate_numbers];
	S_sim_po = new float[core->gate_numbers];
	S_sim_ff_po = new  float[core->gate_numbers];

	S_sim_vec = vector< vector<ser> >(core->gate_numbers, vector<ser>());
	S_mlet_vec = vector< vector<ser> >(core->gate_numbers, vector<ser>());
	
	SatCK = new (nothrow) float*[core->gate_numbers];
	S1_to_CK = new (nothrow) float*[core->gate_numbers];
	S_sim_clk = new (nothrow) float[core->gate_numbers];
	
	
	for (int i=0; i<core->gate_numbers; i++){
		SatCK[i] = new (nothrow) float[config->clock];
		S1_to_CK[i] = new (nothrow) float[config->clock];
	}
	
	pi = new short[core->nodes+1];
	dfs_fnum = new short[core->nodes+1];
    //Clear dfs_fnum[], which records blackened timestamps
	for (int i=0; i<=core->nodes; i++)
        dfs_fnum[i] = 0;
}

MBU::~MBU(void){
	delete S_sim ;
	delete S_sim_var ;
	delete S_sim_ff ;
	delete S_sim_po; 
	delete S_sim_ff_po ;
}


int fact(int x){
	//if(x==0) return 1 ;
	//if(x==1) return 1 ;
	//return fact(x-1)*x ;
	if(x<0) return 1 ;
	int f[10] = {1, 1, 2, 6, 24, 120, 720, 5040};
	if(x<8) return f[x] ;
	else	return f[7] ;
}

//No actuall use, for debugging only
void MBU::dump_the_gate(gate* g){
	for(int i=0;i<g->event_list.size();i++){
				cout << "# "<< g->event_list[i]->time
						<< " (" << g->event_list[i]->p0
						<< "," << g->event_list[i]->p1
						<< "," << g->event_list[i]->pd
						<< "," << g->event_list[i]->pdn << ")" << endl ;
			}
}


void MBU::met_masking_effect(event* e){
	//Event Combination
	if( (e->p0==1) || (e->p1==1) ){
		//This is a normal event, inject the pulse on it.
		e->pd = 1-e->p0 ; e->pdn = 1-e->p1 ;
		e->p0 = e->p1 = 0 ;
	}else if( (e->pd==1) || (e->pdn==1) ){
		//MET masking effect, two mets have can cancel each other. this is specially true
		//for special fault sit models like Common-Fanin of gates.
		//However CFI&CFO are not processed here but at the compute_gate_event_list.
		//Here only some very rare of accidental overlap at forward-cones will occur.
		//this line should NOT occur at SET. Updated for TCAD.
		e->p0 = 1-e->pd ; e->p1 = 1-e->pdn ;
		e->pd = e->pdn = 0 ;
	}else{
		cout << "ERROR @ MET-Masking-Effect ... This should never happen." << endl ;
	}
}


//Injecting statistical faults
//in {0,1,0e,1e} fault space, we should do the logic simulation before performing statistical fault injection
//this is not applicable to {0,1,a,a'} fault space.
void MBU::statistical_fault_injection(struct gate *gateptr, int time, int width){
	deque <struct event*> eventList;
	bool a_injected=false, b_injected=false;
	for(int i=0;i<gateptr->event_list.size();i++){
		event* e = gateptr->event_list[i] ;
		if(e->time < time){
			eventList.push_back(e) ;
		}else if (e->time == time){
			met_masking_effect(e);
			eventList.push_back(e) ;
			a_injected=true ;
		}else if ((time < e->time) && (e->time<time+width)){
			if(!a_injected){
				a_injected=true;
				struct event* eventptr = new struct event;
				eventptr->time = time;
				eventptr->p0 = 0; eventptr->p1=0; eventptr->pd=1-gateptr->value; eventptr->pdn=gateptr->value;
				eventList.push_back(eventptr);
			}
			eventList.push_back(e) ;
		}else if (e->time==(time+width)){
			met_masking_effect(e);
			b_injected=true ;
			eventList.push_back(e) ;
		}else { // time+width < x
			if(!b_injected){
				b_injected=true;
				struct event* eventptr2 = new struct event;
				eventptr2->time = time+ width;
				eventptr2->p0 = 1-gateptr->value; eventptr2->p1=gateptr->value; eventptr2->pd=0; eventptr2->pdn=0;
				eventList.push_back(eventptr2);
			}
			eventList.push_back(e) ;
		}
	}

	if(a_injected==false){
		struct event* eventptr = new struct event;
		eventptr->time = time;
		eventptr->p0 = 0; eventptr->p1=0; eventptr->pd=1-gateptr->value; eventptr->pdn=gateptr->value;
		eventList.push_back(eventptr);
	}

	if(b_injected==false){
		struct event* eventptr2 = new struct event;
		eventptr2->time = time+ width;
		eventptr2->p0 = 1-gateptr->value; eventptr2->p1=gateptr->value; eventptr2->pd=0; eventptr2->pdn=0;
		eventList.push_back(eventptr2);
	}

	gateptr->event_list.clear() ;
	gateptr->event_list = eventList ;
}

/*
old fault injector:
void MBU::statistical_fault_injection(struct gate *gateptr, int time, int width){
	struct event* eventptr = new struct event;
	eventptr->time = time;
	eventptr->p0 = 0; eventptr->p1=0; eventptr->pd=1-gateptr->value; eventptr->pdn=gateptr->value;
	gateptr->event_list.push_back(eventptr);

	struct event* eventptr2 = new struct event;
	eventptr2->time = time+ width;
	eventptr2->p0 = 1-gateptr->value; eventptr2->p1=gateptr->value; eventptr2->pd=0; eventptr2->pdn=0;
	gateptr->event_list.push_back(eventptr2);
}
*/


void MBU::analytical_fault_injection(struct gate *gateptr,int width){
	struct event* eventptr = new struct event;
	eventptr->time = 0;
	eventptr->p0 = 1- gateptr->signal_probability; eventptr->p1 = gateptr->signal_probability;
	eventptr->pd = 0; eventptr->pdn = 0;
	gateptr->event_list.push_back(eventptr);
			
	eventptr = new struct event;
	eventptr->time = (int)gateptr->gd;
	//based on 0^e, 1^e fault propagation system
	eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd =  gateptr->signal_probability; eventptr->pdn = 1- gateptr->signal_probability;
	gateptr->event_list.push_back(eventptr);
			
	eventptr = new struct event;
	eventptr->time = (int)gateptr->gd + width;
	//APPROACH 1
	eventptr->p0 = 1 - gateptr->signal_probability ; eventptr->p1 = gateptr->signal_probability; eventptr->pd = 0; eventptr->pdn = 0;
	//APPROACH 2
	//eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 0; eventptr->pdn = 1;
	gateptr->event_list.push_back(eventptr);
}

void trreverse(char* begin, char* end);
void itoa(int value, char* str, int base);


void MBU::system_failure_simulation(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp){
	cout << "Statistical Fault Injection running ... " << endl ;
	int total_failures=0;
	float total_derating_sum=0;
	float total_derating_iter=0;
	float total_sim_variance;
	float total_sim_derating;
	float error_tolerance = derating_sim_error_tolerance;
	long total_iteration;
	float z_alpha_half = 2.576;
	int max_event_size;
	
	total_iteration = 0;
	max_event_size=0;
	total_failures=0;
	
	struct gate *gateptr;
	struct gate tgateptr1, tgateptr2;
	struct gate *pred_gateptr;
	struct event* eventptr;
	
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	for (int ff_index=0; ff_index<core->sim_gate_numbers; ff_index++){
		cout << "Simulating gate# " << ff_index << " ... " << endl ;
		//if ((core->gate_list[ff_index]->is_dff == 1) || ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo))){
		if( core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){
			core->gate_list[ff_index]->sderating = 1;
			S_sim_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			continue;
		}
		
		//------------------------------------------------------------
		//	Fault Site Identification & Generation
		//------------------------------------------------------------		
		vector<faultSite> fault_site_list = fsg->getFaultSiteList(ff_index);
		int total_mbu_simulation_per_gate  = fsg->getTotalFaultSite(ff_index);
		for(int mbu_index=0; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			vector<float> reachable_ff ;
			vector<float> reachable_po ;
			for(int i=0;i<config->clock;i++){
				reachable_ff.push_back(0);
				reachable_po.push_back(0);
			}
			core->gate_list[ff_index]->sderating = 0;
			int total_iteration = 0;
			total_failures =0;
			int max_event_size=0;
			int total_failures=0;
			do{
				for(int i=0;i<config->clock;i++){
					reachable_ff[i]=0;
					reachable_po[i]=0;
				}
				for (int gate_index=0; gate_index<config->step_iteration; gate_index++){
					for (int i=0; i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						gateptr->p0 = 0; gateptr->p1 = 0;
						gateptr->pd = 0; gateptr->pdn = 0;
					}
					//------------------------------------------------------------
					//	set a random vulue to FFs and PIs;
					//------------------------------------------------------------	
					//Assign random values to the inputs
					for (int i=1; i<=core->numPi; i++){
						long rand_value = random();
						//Assigning random loginc to Primary Input
						short rand_logic = rand_value & 1;
						core->PI_list[i] = rand_logic;
						// Forwording the PI's value to its successors
						short node_adj_size = core->fadjlist[i].size();
						for(int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[i][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; 
								if (pred_gate_node_number==i)
									gateptr->input_values[k] = core->PI_list[i];
							}
						}
					}
					//Assign random values to DFFs
					for (int i=0; i<core->dff_num; i++){
						long rand_value = random();
						short rand_logic= rand_value & 0x1; 
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = rand_logic;
						pred_gateptr->input_values[0] = rand_logic; //may be not required
						int fadjnum = core->dfflist_ptr[i]->q;
						short node_adj_size =core->fadjlist[fadjnum].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadjnum][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadjnum)
									gateptr->input_values[k] = pred_gateptr->value;
							}
						}
					}
					//------------------------------------------------------------
					//	Initial logic Simulation
					//	We need to know the value of the net before injecting faults
					//	so we need an initial logic simulation
					//------------------------------------------------------------	
					//Propagating the input values through intermediate gates
					for (int i=0; i<core->gate_numbers; i++){
						pred_gateptr = core->gate_list[i];
						short fadj_number = pred_gateptr->out;
						if (pred_gateptr->is_dff == 1) continue;
						core->compute_gate_value(pred_gateptr);
						short node_adj_size =core->fadjlist[fadj_number].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadj_number][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
           						pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadj_number)
									gateptr->input_values[k] = pred_gateptr->value;
							}	
						}
					}
					
					//Propagating the values through FFs
					for (int i=0; i<core->dff_num; i++){
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = pred_gateptr->input_values[0];
					}
					//------------------------------------------------------------
					//	End of Logic Simulation
					//------------------------------------------------------------	
					//------------------------------------------------------------
					//	Monte Carlo Simulation
					//------------------------------------------------------------	
					// starting from sorted gate list and compute events for each gate in order
					bool fault_not_injected_yet = true ;
					for (int j=0; j<core->gate_numbers; j++){
						gateptr = core->gate_list[j];
						if (gateptr->is_dff == 1) continue;
						
						compute_gate_event_list(gateptr,core,config);
						
						if (gateptr->event_list.size()>max_event_size) max_event_size = gateptr->event_list.size();
						bool this_is_target_gate = (core->gate_list[j]->out == fault_site_list[mbu_index].target1->out) || (core->gate_list[j]->out == fault_site_list[mbu_index].target2->out) ;
						//cout << "*3" << endl ;
						if( fault_not_injected_yet && this_is_target_gate){
							fault_not_injected_yet = false;
							int t1 = random() % config->tperiod; // circuit_period;
							//removing events >t1
							gate* target_gate_1 = fault_site_list[mbu_index].target1 ;
							gate* target_gate_2 = fault_site_list[mbu_index].target2 ;

							int list_size = target_gate_1->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (target_gate_1->event_list[k]->time >= t1){
									eventptr = target_gate_1->event_list[k];
									target_gate_1->event_list.pop_back();
									tgateptr1.event_list.push_front(eventptr);
								}
							}
							list_size = target_gate_2->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (target_gate_2->event_list[k]->time >= t1){
									eventptr = target_gate_2->event_list[k];
									target_gate_2->event_list.pop_back();
									tgateptr2.event_list.push_front(eventptr);
								}
							}
							
							//------------------------------------------------------------
							//	Fault Generation
							//------------------------------------------------------------
							//cout << "[X] Injecting fault events" << endl ;
							if(fault_site_list[mbu_index].target_count==1){//seu
								statistical_fault_injection( fault_site_list[mbu_index].target1, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
							}else{//mbu
								statistical_fault_injection( fault_site_list[mbu_index].target1, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
								statistical_fault_injection( fault_site_list[mbu_index].target2, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target2));
							}
							
							list_size = tgateptr1.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr1.event_list[0];
								tgateptr1.event_list.pop_front();
								if (eventptr->time > (fg->generateFault(core, config, fault_site_list[mbu_index].target1)+t1))
									target_gate_1->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
							
							list_size = tgateptr2.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr2.event_list[0];
								tgateptr2.event_list.pop_front();
								if (eventptr->time > (fault_site_list[mbu_index].target2->out+t1))
									target_gate_2->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
						}
					}
					//Checking if the METs has been latched in DFFs
					int event_time1,event_time2;
					for (int j=0; j<core->dff_num; j++){
						bool is_seu=false ;
						gateptr = core->fadjlist_ptr[core->dfflist_ptr[j]->q];
						
						compute_gate_event_list(gateptr,core,config);
						
						for (int k=0; k<gateptr->event_list.size()-1; k++){
							//if there is an event (ti,D) such that tperiod-(tsu+th)<ti <tperiod
							event_time1 = gateptr->event_list[k]->time;
							event_time2 = gateptr->event_list[k+1]->time;
							//if ((event_time >= (tperiod-tsu)) && (event_time <= (tperiod+th) ) )
							int tperiod = config->tperiod ;
							int tsu		= config->t_setup ;
							int th		= config->t_hold  ;
							if (   ((event_time1 <= (tperiod-tsu)) && (event_time2 >= (tperiod-tsu)))
								|| ((event_time1 <= (tperiod+th)) && (event_time2 >= (tperiod+th)))
								|| ((event_time1 <= (2*tperiod-tsu)) && (event_time2 >= (2*tperiod-tsu)))
								|| ((event_time1 <= (2*tperiod+th)) && (event_time2 >= (2*tperiod+th)))
								|| ((event_time1 <= (3*tperiod-tsu)) && (event_time2 >= (3*tperiod-tsu)))
								|| ((event_time1 <= (3*tperiod+th)) && (event_time2 >= (3*tperiod+th)))
								|| ((event_time1 <= (4*tperiod-tsu)) && (event_time2 >= (4*tperiod-tsu)))
								|| ((event_time1 <= (4*tperiod+th)) && (event_time2 >= (4*tperiod+th)))
								|| ((event_time1 <= (5*tperiod-tsu)) && (event_time2 >= (5*tperiod-tsu)))
								|| ((event_time1 <= (5*tperiod+th)) && (event_time2 >= (5*tperiod+th)))
								){
								if ((gateptr->event_list[k]->pd !=0) || (gateptr->event_list[k]->pdn !=0)){
									// this statement will check for electrical masking effect
									if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0))){
										// error is manifested in DFF
										gateptr->p0=0; gateptr->p1=0;
										gateptr->pd=gateptr->event_list[k]->pd;
										gateptr->pdn=gateptr->event_list[k]->pdn;
										is_seu=true ;
									}
								}
							}
						}
						if(is_seu) reachable_ff[0]++ ;
					}
					
					
					// Checking if error can manifest itself to any POs.
					bool is_failure = false;
					//for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
					//	gateptr = core->fadjlist_ptr[i];
					//	if ((gateptr->pd == 1)||(gateptr->pdn == 1)){
					//		is_failure = true;
					//		break;
					//	}
					//}
					

					for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
						bool is_reached_po = false ;
						gateptr = core->fadjlist_ptr[i];
						if ((gateptr->pd == 1)||(gateptr->pdn == 1)){
							is_failure = true;
							is_reached_po=true;
							//break;
						}
						for(int j=0;j<gateptr->event_list.size()-1;j++){
							if ((gateptr->event_list[j]->pd !=0) || (gateptr->event_list[j]->pdn !=0)){
								is_failure=true;
								is_reached_po=true;
								//break;
							}
						}
						if( is_reached_po ) reachable_po[0]++ ;
					}
					
					//cout << "$" ;
					
					for (int i=0;i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						int list_size = gateptr->event_list.size();
						//cout << " " << list_size << " " ;
						for (int j=0; j<list_size; j++){
							eventptr = gateptr->event_list[j];
						}
						for (int j=list_size-1; j>=0; j--){
							eventptr = gateptr->event_list[j];
							gateptr->event_list.pop_back();
							delete eventptr;
							eventptr = NULL;
						}
						
					}
					//cout << "Y" ;
					if (is_failure==true){
						total_failures++;
						S_sim_clk[ff_index]++;
						//break; //main break, do not continue the simulation
					}
					
					if(is_failure==false){
						for(int clock=2;clock<config->clock;clock++){
							for (int i=0; i<core->gate_numbers; i++){
								gateptr = core->gate_list[i];
								if (gateptr->is_dff == 1) continue;// we will compute the flip-flops at the end
								for (int j=0; j< (gateptr->input_count); j++){// Input traversing
									// if input is Primary Input
									if ((gateptr->input_list[j]) <= core->numPi){
										p1_values[j] = core->PI_list[gateptr->input_list[j]];
										p0_values[j] = 1 - core->PI_list[gateptr->input_list[j]];
										pd_values[j] = 0; pdn_values[j] = 0;
										continue;
									}
									pred_gateptr = core->fadjlist_ptr[gateptr->input_list[j]];
									p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
									pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
								}// end of input traversing
								compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
							}// End of gate_list
							bool is_failure_multi_cycle = false;
							for (int ff_index2=0; ff_index2<core->dff_num; ff_index2++){
								gate* gptr1 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->d];
								gate* gptr2 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->q];
								gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
								gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
								if( (gptr1->pd>0) || (gptr1->pdn>0) ){
									reachable_ff[config->clock-1]++;
								}
							}
		
							for (int i =(core->numPi+1); i<=(core->numPi+core->numPo); i++){
								bool reach_po=false; 
								gate* gptr1 = core->fadjlist_ptr[i];
								if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
									is_failure_multi_cycle = true;
									reach_po=true ;
									//break;
								}
								if(reach_po) reachable_po[config->clock-1]++;
							}
							if (is_failure_multi_cycle==true){
								S_sim_clk[ff_index]++;
								total_failures++;
								break;
							}
						}
					}
				}//for fault injection
				
				total_iteration += config->step_iteration;
				total_sim_derating = (float)total_failures / (float)total_iteration;
				total_sim_variance = sqrt((total_sim_derating * (1 - total_sim_derating))/(float)total_iteration);
				if (total_sim_variance==0)
					total_sim_variance = sqrt(((1.0/(float)total_iteration) * (1 - (1.0/(float)total_iteration)))/(float)total_iteration);
			} while (((z_alpha_half*total_sim_variance) > error_tolerance) && (total_iteration<config->max_iteration) );
			
			for(int i=0;i<config->clock;i++){
				reachable_ff[i] /= total_iteration;
				reachable_po[i] /= total_iteration;
				//cout << reachable_ff[i] << " " << reachable_po[i] << endl ;
			}
			
			reachable_FF_CK.push_back( reachable_ff );
			reachable_PO_CK.push_back( reachable_po );
			core->gate_list[ff_index]->sderating = total_sim_derating;
			
			S_sim_vec[ff_index].push_back( ser(0,0,total_sim_derating) );
			total_derating_sum += total_sim_derating ;
			total_derating_iter++ ;
		}//mbu
		float sum=0;
		for(int i=0;i<total_mbu_simulation_per_gate;i++){
			sum += S_sim_vec[ff_index].at(i).derating ;
		}
		
		sum /= total_mbu_simulation_per_gate;
		core->gate_list[ff_index]->sderating = sum;
		cout << endl;
	}//for (ff_index=0;
	
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Simulation Derating for all gates finished\n");
	
	float overall_sim_der = 0;
	for (int i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		overall_sim_der += gateptr->sderating;
		fprintf(resultfp, "%d %f \t %d\n ", i, gateptr->sderating, gateptr->is_dff);
	}
	overall_sim_der = overall_sim_der / (float)(core->gate_numbers);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall_simulation_Derating = %f \n", overall_sim_der);
	fprintf(resultfp,"Overall_simulation_derating per unit = %f \n", total_derating_sum/total_derating_iter);
	
	
	fprintf(resultfp, "\n\nFault_Reachability_Analysis: Gate= %d, FF=%d, PO=%d\n", core->gate_numbers, core->dff_num, core->numPo);
	float po_reach_incr=0;
	for(int i=0;i<config->clock;i++){
		float ff_reach=0, po_reach=0;
		for(int j=0;j<reachable_FF_CK.size();j++){
			ff_reach += reachable_FF_CK[i][j] ;
		}
		ff_reach /= reachable_FF_CK.size();
		
		for(int j=0;j<reachable_PO_CK.size();j++){
			po_reach += reachable_PO_CK[i][j] ;
		}
		po_reach /= reachable_PO_CK.size();		
		po_reach_incr += po_reach ;
		fprintf(resultfp, "Reachability_(FF,PO)_at_clock %d = %f %f %f\n", i, ff_reach, po_reach, po_reach_incr);
	}
}


void MBU::system_failure_simulation_single(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp){
	cout << "Statistical Fault Injection running ... " << endl ;
	int total_failures=0;
	float total_derating_sum=0;
	float total_derating_iter=0;
	float total_sim_variance;
	float total_sim_derating;
	float error_tolerance = derating_sim_error_tolerance;
	long total_iteration;
	float z_alpha_half = 2.576;
	int max_event_size;
	
	total_iteration = 0;
	max_event_size=0;
	total_failures=0;
	
	struct gate *gateptr;
	struct gate tgateptr1, tgateptr2;
	struct gate *pred_gateptr;
	struct event* eventptr;
	
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	for (int ff_index=0; ff_index<core->sim_gate_numbers; ff_index++){
		cout << "Simulating gate# " << ff_index << " ... " << endl ;
		if ((core->gate_list[ff_index]->is_dff == 1) || ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo))){
		//if( core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){
			core->gate_list[ff_index]->sderating = 1;
			S_sim_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			continue;
		}
		
		//------------------------------------------------------------
		//	Fault Site Identification & Generation
		//------------------------------------------------------------		
		vector<faultSite> fault_site_list = fsg->getFaultSiteList(ff_index);
		int total_mbu_simulation_per_gate  = fsg->getTotalFaultSite(ff_index);
		for(int mbu_index=0; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			vector<float> reachable_ff ;
			vector<float> reachable_po ;
			for(int i=0;i<config->clock;i++){
				reachable_ff.push_back(0);
				reachable_po.push_back(0);
			}
			core->gate_list[ff_index]->sderating = 0;
			int total_iteration = 0;
			total_failures =0;
			int max_event_size=0;
			int total_failures=0;
			do{
				for(int i=0;i<config->clock;i++){
					reachable_ff[i]=0;
					reachable_po[i]=0;
				}
				for (int gate_index=0; gate_index<config->step_iteration; gate_index++){
					for (int i=0; i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						gateptr->p0 = 0; gateptr->p1 = 0;
						gateptr->pd = 0; gateptr->pdn = 0;
					}
					//------------------------------------------------------------
					//	set a random vulue to FFs and PIs;
					//------------------------------------------------------------	
					//Assign random values to the inputs
					for (int i=1; i<=core->numPi; i++){
						long rand_value = random();
						//Assigning random loginc to Primary Input
						short rand_logic = rand_value & 1;
						core->PI_list[i] = rand_logic;
						// Forwording the PI's value to its successors
						short node_adj_size = core->fadjlist[i].size();
						for(int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[i][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; 
								if (pred_gate_node_number==i)
									gateptr->input_values[k] = core->PI_list[i];
							}
						}
					}
					//Assign random values to DFFs
					for (int i=0; i<core->dff_num; i++){
						long rand_value = random();
						short rand_logic= rand_value & 0x1; 
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = rand_logic;
						pred_gateptr->input_values[0] = rand_logic; //may be not required
						int fadjnum = core->dfflist_ptr[i]->q;
						short node_adj_size =core->fadjlist[fadjnum].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadjnum][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadjnum)
									gateptr->input_values[k] = pred_gateptr->value;
							}
						}
					}
					//------------------------------------------------------------
					//	Initial logic Simulation
					//	We need to know the value of the net before injecting faults
					//	so we need an initial logic simulation
					//------------------------------------------------------------	
					//Propagating the input values through intermediate gates
					for (int i=0; i<core->gate_numbers; i++){
						pred_gateptr = core->gate_list[i];
						short fadj_number = pred_gateptr->out;
						if (pred_gateptr->is_dff == 1) continue;
						core->compute_gate_value(pred_gateptr);
						short node_adj_size =core->fadjlist[fadj_number].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadj_number][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
           						pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadj_number)
									gateptr->input_values[k] = pred_gateptr->value;
							}	
						}
					}
					
					//Propagating the values through FFs
					for (int i=0; i<core->dff_num; i++){
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = pred_gateptr->input_values[0];
					}
					//------------------------------------------------------------
					//	End of Logic Simulation
					//------------------------------------------------------------	
					//------------------------------------------------------------
					//	Monte Carlo Simulation
					//------------------------------------------------------------	
					// starting from sorted gate list and compute events for each gate in order
					bool fault_not_injected_yet = true ;
					for (int j=0; j<core->gate_numbers; j++){
						gateptr = core->gate_list[j];
						if (gateptr->is_dff == 1) continue;
						
						compute_gate_event_list(gateptr,core,config);
						
						if (gateptr->event_list.size()>max_event_size) max_event_size = gateptr->event_list.size();
						bool this_is_target_gate = (core->gate_list[j]->out == fault_site_list[mbu_index].target1->out) || (core->gate_list[j]->out == fault_site_list[mbu_index].target2->out) ;
						//cout << "*3" << endl ;
						if( fault_not_injected_yet && this_is_target_gate){
							fault_not_injected_yet = false;
							int t1 = random() % config->tperiod; // circuit_period;
							//removing events >t1
							gate* target_gate_1 = fault_site_list[mbu_index].target1 ;
							gate* target_gate_2 = fault_site_list[mbu_index].target2 ;
							
							int list_size = target_gate_1->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (target_gate_1->event_list[k]->time >= t1){
									eventptr = target_gate_1->event_list[k];
									target_gate_1->event_list.pop_back();
									tgateptr1.event_list.push_front(eventptr);
								}
							}
							list_size = target_gate_2->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (target_gate_2->event_list[k]->time >= t1){
									eventptr = target_gate_2->event_list[k];
									target_gate_2->event_list.pop_back();
									tgateptr2.event_list.push_front(eventptr);
								}
							}
							
							//------------------------------------------------------------
							//	Fault Generation
							//------------------------------------------------------------
							//cout << "[X] Injecting fault events" << endl ;
							if(fault_site_list[mbu_index].target_count==1){//seu
								statistical_fault_injection( fault_site_list[mbu_index].target1, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
							}else{//mbu
								statistical_fault_injection( fault_site_list[mbu_index].target1, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
								statistical_fault_injection( fault_site_list[mbu_index].target2, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target2));
							}
							
							list_size = tgateptr1.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr1.event_list[0];
								tgateptr1.event_list.pop_front();
								if (eventptr->time > (fg->generateFault(core, config, fault_site_list[mbu_index].target1)+t1))
									target_gate_1->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
							
							list_size = tgateptr2.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr2.event_list[0];
								tgateptr2.event_list.pop_front();
								if (eventptr->time > (fault_site_list[mbu_index].target2->out+t1))
									target_gate_2->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
						}
					}
					//Checking if the METs has been latched in DFFs
					int event_time1,event_time2;
					bool is_failure = false;
					for (int j=0; j<core->dff_num; j++){
						bool is_seu=false ;
						gateptr = core->fadjlist_ptr[core->dfflist_ptr[j]->q];
						
						compute_gate_event_list(gateptr,core,config);
						
						for (int k=0; k<gateptr->event_list.size()-1; k++){
							//if there is an event (ti,D) such that tperiod-(tsu+th)<ti <tperiod
							event_time1 = gateptr->event_list[k]->time;
							event_time2 = gateptr->event_list[k+1]->time;
							//if ((event_time >= (tperiod-tsu)) && (event_time <= (tperiod+th) ) )
							int tperiod = config->tperiod ;
							int tsu		= config->t_setup ;
							int th		= config->t_hold  ;
							if (   ((event_time1 <= (tperiod-tsu)) && (event_time2 >= (tperiod-tsu)))
								|| ((event_time1 <= (tperiod+th)) && (event_time2 >= (tperiod+th)))
								|| ((event_time1 <= (2*tperiod-tsu)) && (event_time2 >= (2*tperiod-tsu)))
								|| ((event_time1 <= (2*tperiod+th)) && (event_time2 >= (2*tperiod+th)))
								|| ((event_time1 <= (3*tperiod-tsu)) && (event_time2 >= (3*tperiod-tsu)))
								|| ((event_time1 <= (3*tperiod+th)) && (event_time2 >= (3*tperiod+th)))
								|| ((event_time1 <= (4*tperiod-tsu)) && (event_time2 >= (4*tperiod-tsu)))
								|| ((event_time1 <= (4*tperiod+th)) && (event_time2 >= (4*tperiod+th)))
								|| ((event_time1 <= (5*tperiod-tsu)) && (event_time2 >= (5*tperiod-tsu)))
								|| ((event_time1 <= (5*tperiod+th)) && (event_time2 >= (5*tperiod+th)))
								){
								if ((gateptr->event_list[k]->pd !=0) || (gateptr->event_list[k]->pdn !=0)){
									// this statement will check for electrical masking effect
									if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0))){
										// error is manifested in DFF
										gateptr->p0=0; gateptr->p1=0;
										gateptr->pd=gateptr->event_list[k]->pd;
										gateptr->pdn=gateptr->event_list[k]->pdn;
										is_seu=true ;
										is_failure=true;
									}
								}
							}
						}
						if(is_seu) reachable_ff[0]++ ;
					}
					
					
					// Checking if error can manifest itself to any POs.
					
					//for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
					//	gateptr = core->fadjlist_ptr[i];
					//	if ((gateptr->pd == 1)||(gateptr->pdn == 1)){
					//		is_failure = true;
					//		break;
					//	}
					//}
					
					
					for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
						bool is_reached_po = false ;
						gateptr = core->fadjlist_ptr[i];
						if ((gateptr->pd == 1)||(gateptr->pdn == 1)){
							is_failure = true;
							is_reached_po=true;
							//break;
						}
						for(int j=0;j<gateptr->event_list.size()-1;j++){
							if ((gateptr->event_list[j]->pd !=0) || (gateptr->event_list[j]->pdn !=0)){
								is_failure=true;
								is_reached_po=true;
								//break;
							}
						}
						if( is_reached_po ) reachable_po[0]++ ;
					}
					
					//cout << "$" ;
					
					for (int i=0;i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						int list_size = gateptr->event_list.size();
						//cout << " " << list_size << " " ;
						for (int j=0; j<list_size; j++){
							eventptr = gateptr->event_list[j];
						}
						for (int j=list_size-1; j>=0; j--){
							eventptr = gateptr->event_list[j];
							gateptr->event_list.pop_back();
							delete eventptr;
							eventptr = NULL;
						}
						
					}
					//cout << "Y" ;
					if (is_failure==true){
						total_failures++;
						S_sim_clk[ff_index]++;
						break; //main break, do not continue the simulation
					}
					
					if(is_failure==false){
						for(int clock=2;clock<config->clock;clock++){
							for (int i=0; i<core->gate_numbers; i++){
								gateptr = core->gate_list[i];
								if (gateptr->is_dff == 1) continue;// we will compute the flip-flops at the end
								for (int j=0; j< (gateptr->input_count); j++){// Input traversing
									// if input is Primary Input
									if ((gateptr->input_list[j]) <= core->numPi){
										p1_values[j] = core->PI_list[gateptr->input_list[j]];
										p0_values[j] = 1 - core->PI_list[gateptr->input_list[j]];
										pd_values[j] = 0; pdn_values[j] = 0;
										continue;
									}
									pred_gateptr = core->fadjlist_ptr[gateptr->input_list[j]];
									p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
									pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
								}// end of input traversing
								compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
							}// End of gate_list
							bool is_failure_multi_cycle = false;
							for (int ff_index2=0; ff_index2<core->dff_num; ff_index2++){
								gate* gptr1 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->d];
								gate* gptr2 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->q];
								gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
								gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
								if( (gptr1->pd>0) || (gptr1->pdn>0) ){
									reachable_ff[config->clock-1]++;
								}
							}
							
							for (int i =(core->numPi+1); i<=(core->numPi+core->numPo); i++){
								bool reach_po=false; 
								gate* gptr1 = core->fadjlist_ptr[i];
								if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
									is_failure_multi_cycle = true;
									reach_po=true ;
									//break;
								}
								if(reach_po) reachable_po[config->clock-1]++;
							}
							if (is_failure_multi_cycle==true){
								S_sim_clk[ff_index]++;
								total_failures++;
								break;
							}
						}
					}
				}//for fault injection
				
				total_iteration += config->step_iteration;
				total_sim_derating = (float)total_failures / (float)total_iteration;
				total_sim_variance = sqrt((total_sim_derating * (1 - total_sim_derating))/(float)total_iteration);
				if (total_sim_variance==0)
					total_sim_variance = sqrt(((1.0/(float)total_iteration) * (1 - (1.0/(float)total_iteration)))/(float)total_iteration);
			} while (((z_alpha_half*total_sim_variance) > error_tolerance) && (total_iteration<config->max_iteration) );
			
			for(int i=0;i<config->clock;i++){
				reachable_ff[i] /= total_iteration;
				reachable_po[i] /= total_iteration;
				//cout << reachable_ff[i] << " " << reachable_po[i] << endl ;
			}
			
			reachable_FF_CK.push_back( reachable_ff );
			reachable_PO_CK.push_back( reachable_po );
			core->gate_list[ff_index]->sderating = total_sim_derating;
			
			S_sim_vec[ff_index].push_back( ser(0,0,total_sim_derating) );
			total_derating_sum += total_sim_derating ;
			total_derating_iter++ ;
		}//mbu
		float sum=0;
		for(int i=0;i<total_mbu_simulation_per_gate;i++){
			sum += S_sim_vec[ff_index].at(i).derating ;
		}
		
		sum /= total_mbu_simulation_per_gate;
		core->gate_list[ff_index]->sderating = sum;
		cout << endl;
	}//for (ff_index=0;
	
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Simulation Derating for all gates finished\n");
	
	float overall_sim_der = 0;
	for (int i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		overall_sim_der += gateptr->sderating;
		fprintf(resultfp, "%d %f \t %d\n ", i, gateptr->sderating, gateptr->is_dff);
	}
	overall_sim_der = overall_sim_der / (float)(core->gate_numbers);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall_simulation_Derating = %f \n", overall_sim_der);
	fprintf(resultfp,"Overall_simulation_derating per unit = %f \n", total_derating_sum/total_derating_iter);
	
	
	fprintf(resultfp, "\n\nFault_Reachability_Analysis: Gate= %d, FF=%d, PO=%d\n", core->gate_numbers, core->dff_num, core->numPo);
	float po_reach_incr=0;
	for(int i=0;i<config->clock;i++){
		float ff_reach=0, po_reach=0;
		for(int j=0;j<reachable_FF_CK.size();j++){
			ff_reach += reachable_FF_CK[i][j] ;
		}
		ff_reach /= reachable_FF_CK.size();
		
		for(int j=0;j<reachable_PO_CK.size();j++){
			po_reach += reachable_PO_CK[i][j] ;
		}
		po_reach /= reachable_PO_CK.size();		
		po_reach_incr += po_reach ;
		fprintf(resultfp, "Reachability_(FF,PO)_at_clock %d = %f %f %f\n", i, ff_reach, po_reach, po_reach_incr);
	}
}


void MBU::system_failure_simulation_report_ff_po(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp){
	cout << "Statistical Fault Injection running ... " << endl ;
	int total_failures=0;
	float total_derating_sum=0;
	float total_derating_iter=0;
	float total_sim_variance;
	float total_sim_derating;
	float error_tolerance = derating_sim_error_tolerance;
	long total_iteration;
	float z_alpha_half = 2.576;
	int max_event_size;
	
	total_iteration = 0;
	max_event_size=0;
	total_failures=0;
	
	struct gate *gateptr;
	struct gate tgateptr1, tgateptr2;
	struct gate *pred_gateptr;
	struct event* eventptr;
	
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	int   po_ck[10], ff_ck[10]; 
	int		failure_per_ff[100][100], failure_per_po[100][100] ;
	//int **failure_per_ff, **failure_per_po ;
	//failure_per_ff = new int*[core->dff_num];
	//failure_per_po = new int*[core->numPo];
	for(int i=0;i<core->dff_num;i++){
		for(int j=0;j<config->clock;j++)
			failure_per_ff[i][j]=0;
	}
	for(int i=0;i<core->numPo;i++){
		for(int j=0;j<config->clock;j++)
			failure_per_ff[i][j]=0;
	}	
	cout << "(99999" << endl ;
	for (int ff_index=0; ff_index<core->sim_gate_numbers; ff_index++){
		cout << "Simulating gate# " << ff_index << " ... " << endl ;
		//if ((core->gate_list[ff_index]->is_dff == 1) || ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo))){
		if( core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){
			core->gate_list[ff_index]->sderating = 1;
			fprintf(resultfp, "%d %d 1000 1000\n", ff_index, ff_index);
			S_sim_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			continue;
		}
		
		//------------------------------------------------------------
		//	Fault Site Identification & Generation
		//------------------------------------------------------------		
		vector<faultSite> fault_site_list = fsg->getFaultSiteList(ff_index);
		int total_mbu_simulation_per_gate  = fsg->getTotalFaultSite(ff_index);
		for(int mbu_index=0; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			for(int i=0;i<10;i++){
				po_ck[i]=0;
				ff_ck[i]=0;
			}
			vector<float> reachable_ff ;
			vector<float> reachable_po ;
			for(int i=0;i<config->clock;i++){
				reachable_ff.push_back(0);
				reachable_po.push_back(0);
			}
			core->gate_list[ff_index]->sderating = 0;
			int total_iteration = 0;
			total_failures =0;
			int max_event_size=0;
			int total_failures=0;
			do{
				for(int i=0;i<config->clock;i++){
					reachable_ff[i]=0;
					reachable_po[i]=0;
				}
				for (int gate_index=0; gate_index<config->step_iteration; gate_index++){
					for (int i=0; i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						gateptr->p0 = 0; gateptr->p1 = 0;
						gateptr->pd = 0; gateptr->pdn = 0;
					}
					//------------------------------------------------------------
					//	set a random vulue to FFs and PIs;
					//------------------------------------------------------------	
					//Assign random values to the inputs
					for (int i=1; i<=core->numPi; i++){
						long rand_value = random();
						//Assigning random loginc to Primary Input
						short rand_logic = rand_value & 1;
						core->PI_list[i] = rand_logic;
						// Forwording the PI's value to its successors
						short node_adj_size = core->fadjlist[i].size();
						for(int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[i][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; 
								if (pred_gate_node_number==i)
									gateptr->input_values[k] = core->PI_list[i];
							}
						}
					}
					//Assign random values to DFFs
					for (int i=0; i<core->dff_num; i++){
						long rand_value = random();
						short rand_logic= rand_value & 0x1; 
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = rand_logic;
						pred_gateptr->input_values[0] = rand_logic; //may be not required
						int fadjnum = core->dfflist_ptr[i]->q;
						short node_adj_size =core->fadjlist[fadjnum].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadjnum][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadjnum)
									gateptr->input_values[k] = pred_gateptr->value;
							}
						}
					}
					//------------------------------------------------------------
					//	Initial logic Simulation
					//	We need to know the value of the net before injecting faults
					//	so we need an initial logic simulation
					//------------------------------------------------------------	
					//Propagating the input values through intermediate gates
					for (int i=0; i<core->gate_numbers; i++){
						pred_gateptr = core->gate_list[i];
						short fadj_number = pred_gateptr->out;
						if (pred_gateptr->is_dff == 1) continue;
						core->compute_gate_value(pred_gateptr);
						short node_adj_size =core->fadjlist[fadj_number].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadj_number][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
           						pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadj_number)
									gateptr->input_values[k] = pred_gateptr->value;
							}	
						}
					}
					
					//Propagating the values through FFs
					for (int i=0; i<core->dff_num; i++){
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = pred_gateptr->input_values[0];
					}
					//------------------------------------------------------------
					//	End of Logic Simulation
					//------------------------------------------------------------	
					//------------------------------------------------------------
					//	Monte Carlo Simulation
					//------------------------------------------------------------	
					// starting from sorted gate list and compute events for each gate in order
					bool fault_not_injected_yet = true ;
					for (int j=0; j<core->gate_numbers; j++){
						gateptr = core->gate_list[j];
						if (gateptr->is_dff == 1) continue;
						
						compute_gate_event_list(gateptr,core,config);
						
						if (gateptr->event_list.size()>max_event_size) max_event_size = gateptr->event_list.size();
						bool this_is_target_gate = (core->gate_list[j]->out == fault_site_list[mbu_index].target1->out) || (core->gate_list[j]->out == fault_site_list[mbu_index].target2->out) ;
						//cout << "*3" << endl ;
						if( fault_not_injected_yet && this_is_target_gate){
							fault_not_injected_yet = false;
							int t1 = random() % config->tperiod; // circuit_period;
							//removing events >t1
							gate* target_gate_1 = fault_site_list[mbu_index].target1 ;
							gate* target_gate_2 = fault_site_list[mbu_index].target2 ;
							
							int list_size = target_gate_1->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (target_gate_1->event_list[k]->time >= t1){
									eventptr = target_gate_1->event_list[k];
									target_gate_1->event_list.pop_back();
									tgateptr1.event_list.push_front(eventptr);
								}
							}
							list_size = target_gate_2->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (target_gate_2->event_list[k]->time >= t1){
									eventptr = target_gate_2->event_list[k];
									target_gate_2->event_list.pop_back();
									tgateptr2.event_list.push_front(eventptr);
								}
							}
							
							//------------------------------------------------------------
							//	Fault Generation
							//------------------------------------------------------------
							//cout << "[X] Injecting fault events" << endl ;
							if(fault_site_list[mbu_index].target_count==1){//seu
								statistical_fault_injection( fault_site_list[mbu_index].target1, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
							}else{//mbu
								statistical_fault_injection( fault_site_list[mbu_index].target1, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
								statistical_fault_injection( fault_site_list[mbu_index].target2, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target2));
							}
							
							list_size = tgateptr1.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr1.event_list[0];
								tgateptr1.event_list.pop_front();
								if (eventptr->time > (fg->generateFault(core, config, fault_site_list[mbu_index].target1)+t1))
									target_gate_1->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
							
							list_size = tgateptr2.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr2.event_list[0];
								tgateptr2.event_list.pop_front();
								if (eventptr->time > (fault_site_list[mbu_index].target2->out+t1))
									target_gate_2->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
						}
					}
					//Checking if the METs has been latched in DFFs
					int event_time1,event_time2;
					for (int j=0; j<core->dff_num; j++){
						bool is_seu=false ;
						gateptr = core->fadjlist_ptr[core->dfflist_ptr[j]->q];
						
						compute_gate_event_list(gateptr,core,config);
						
						for (int k=0; k<gateptr->event_list.size()-1; k++){
							//if there is an event (ti,D) such that tperiod-(tsu+th)<ti <tperiod
							event_time1 = gateptr->event_list[k]->time;
							event_time2 = gateptr->event_list[k+1]->time;
							//if ((event_time >= (tperiod-tsu)) && (event_time <= (tperiod+th) ) )
							int tperiod = config->tperiod ;
							int tsu		= config->t_setup ;
							int th		= config->t_hold  ;
							if (   ((event_time1 <= (tperiod-tsu)) && (event_time2 >= (tperiod-tsu)))
								|| ((event_time1 <= (tperiod+th)) && (event_time2 >= (tperiod+th)))
								|| ((event_time1 <= (2*tperiod-tsu)) && (event_time2 >= (2*tperiod-tsu)))
								|| ((event_time1 <= (2*tperiod+th)) && (event_time2 >= (2*tperiod+th)))
								|| ((event_time1 <= (3*tperiod-tsu)) && (event_time2 >= (3*tperiod-tsu)))
								|| ((event_time1 <= (3*tperiod+th)) && (event_time2 >= (3*tperiod+th)))
								|| ((event_time1 <= (4*tperiod-tsu)) && (event_time2 >= (4*tperiod-tsu)))
								|| ((event_time1 <= (4*tperiod+th)) && (event_time2 >= (4*tperiod+th)))
								|| ((event_time1 <= (5*tperiod-tsu)) && (event_time2 >= (5*tperiod-tsu)))
								|| ((event_time1 <= (5*tperiod+th)) && (event_time2 >= (5*tperiod+th)))
								){
								if ((gateptr->event_list[k]->pd !=0) || (gateptr->event_list[k]->pdn !=0)){
									// this statement will check for electrical masking effect
									if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0))){
										// error is manifested in DFF
										gateptr->p0=0; gateptr->p1=0;
										gateptr->pd=gateptr->event_list[k]->pd;
										gateptr->pdn=gateptr->event_list[k]->pdn;
										is_seu=true ;
									}
								}
							}
						}
						if(is_seu){
							failure_per_ff[j][0]++;
							ff_ck[0]++;
							reachable_ff[0]++ ;
						}
					}
					
					
					// Checking if error can manifest itself to any POs.
					bool is_failure = false;
					//for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
					//	gateptr = core->fadjlist_ptr[i];
					//	if ((gateptr->pd == 1)||(gateptr->pdn == 1)){
					//		is_failure = true;
					//		break;
					//	}
					//}
					
					
					for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
						bool is_reached_po = false ;
						gateptr = core->fadjlist_ptr[i];
						if ((gateptr->pd == 1)||(gateptr->pdn == 1)){
							is_failure = true;
							is_reached_po=true;
							//break;
						}
						for(int j=0;j<gateptr->event_list.size()-1;j++){
							if ((gateptr->event_list[j]->pd !=0) || (gateptr->event_list[j]->pdn !=0)){
								is_failure=true;
								is_reached_po=true;
								//break;
							}
						}
						if( is_reached_po ){
							reachable_po[0]++ ;
							po_ck[0]++;
							failure_per_po[i-(core->numPi+1)][0]++;
						}
					}
					
					//cout << "$" ;
					
					for (int i=0;i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						int list_size = gateptr->event_list.size();
						//cout << " " << list_size << " " ;
						for (int j=0; j<list_size; j++){
							eventptr = gateptr->event_list[j];
						}
						for (int j=list_size-1; j>=0; j--){
							eventptr = gateptr->event_list[j];
							gateptr->event_list.pop_back();
							delete eventptr;
							eventptr = NULL;
						}
						
					}
					//cout << "Y" ;
					if (is_failure==true){
						total_failures++;
						S_sim_clk[ff_index]++;
						//break; //main break, do not continue the simulation
					}
						for(int clock=2;clock<config->clock;clock++){
							for (int i=0; i<core->gate_numbers; i++){
								gateptr = core->gate_list[i];
								if (gateptr->is_dff == 1) continue;// we will compute the flip-flops at the end
								for (int j=0; j< (gateptr->input_count); j++){// Input traversing
									// if input is Primary Input
									if ((gateptr->input_list[j]) <= core->numPi){
										p1_values[j] = core->PI_list[gateptr->input_list[j]];
										p0_values[j] = 1 - core->PI_list[gateptr->input_list[j]];
										pd_values[j] = 0; pdn_values[j] = 0;
										continue;
									}
									pred_gateptr = core->fadjlist_ptr[gateptr->input_list[j]];
									p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
									pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
								}// end of input traversing
								compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
							}// End of gate_list
							bool is_failure_multi_cycle = false;
							bool is_ff_multi_cycle = false;
							for (int ff_index2=0; ff_index2<core->dff_num; ff_index2++){
								gate* gptr1 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->d];
								gate* gptr2 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->q];
								gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
								gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
								if( (gptr1->pd>0) || (gptr1->pdn>0) ){
									is_ff_multi_cycle=true;
									reachable_ff[config->clock-1]++;
									failure_per_ff[ff_index2][clock-1]++;
									ff_ck[clock-1]++;
								}
							}
							//if(is_ff_multi_cycle==true) ff_ck[clock-1]++;
							for (int i =(core->numPi+1); i<=(core->numPi+core->numPo); i++){
								bool reach_po=false; 
								gate* gptr1 = core->fadjlist_ptr[i];
								if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
									is_failure_multi_cycle = true;
									reach_po=true ;
									//break;
								}
								if(reach_po){
									po_ck[clock-1]++;
									reachable_po[config->clock-1]++;
									failure_per_po[i-(core->numPi+1)][clock-1]++;
								}
							}
							if(is_failure==false){
								if (is_failure_multi_cycle==true){
									is_failure=true;
									S_sim_clk[ff_index]++;
									total_failures++;
								//break;
								}
							}
						}
					
				}//for fault injection
				
				total_iteration += config->step_iteration;
				total_sim_derating = (float)total_failures / (float)total_iteration;
				total_sim_variance = sqrt((total_sim_derating * (1 - total_sim_derating))/(float)total_iteration);
				if (total_sim_variance==0)
					total_sim_variance = sqrt(((1.0/(float)total_iteration) * (1 - (1.0/(float)total_iteration)))/(float)total_iteration);
			//} while (((z_alpha_half*total_sim_variance) > error_tolerance) && (total_iteration<config->max_iteration) );
			}while(total_iteration <config->max_iteration);
			fprintf(resultfp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n", ff_index, total_iteration, 
					po_ck[0], po_ck[1],po_ck[2],po_ck[3],po_ck[4],po_ck[5],po_ck[6],po_ck[7],po_ck[8],po_ck[9], 
					ff_ck[0], ff_ck[1],ff_ck[2],ff_ck[3],ff_ck[4],ff_ck[5],ff_ck[6],ff_ck[7],ff_ck[8],ff_ck[9]);
			core->gate_list[ff_index]->sderating = total_sim_derating;
			
			S_sim_vec[ff_index].push_back( ser(0,0,total_sim_derating) );
			total_derating_sum += total_sim_derating ;
			total_derating_iter++ ;
		}//mbu
		float sum=0;
		for(int i=0;i<total_mbu_simulation_per_gate;i++){
			sum += S_sim_vec[ff_index].at(i).derating ;
		}
		
		sum /= total_mbu_simulation_per_gate;
		core->gate_list[ff_index]->sderating = sum;
		cout << endl;
	}//for (ff_index=0;
	
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Simulation Derating for all gates finished\n");
	
	float overall_sim_der = 0;
	for (int i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		overall_sim_der += gateptr->sderating;
		fprintf(resultfp, "%d %f \t %d\n ", i, gateptr->sderating, gateptr->is_dff);
	}
	overall_sim_der = overall_sim_der / (float)(core->gate_numbers);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall_simulation_Derating = %f \n", overall_sim_der);
	fprintf(resultfp,"Overall_simulation_derating per unit = %f \n", total_derating_sum/total_derating_iter);
	
	
	fprintf(resultfp, "DFFs:\n" ); 
	for(int i=0;i<core->dff_num;i++){
		fprintf(resultfp, "%d %d %d %d %d %d %d %d %d %d %d \n", i, failure_per_ff[i][0], failure_per_ff[i][1], failure_per_ff[i][2], failure_per_ff[i][3], failure_per_ff[i][4], failure_per_ff[i][5], failure_per_ff[i][6], failure_per_ff[i][7], failure_per_ff[i][8], failure_per_ff[i][9]);
	}
	fprintf(resultfp, "POs:\n" ); 
	for(int i=0;i<core->numPo;i++){
		fprintf(resultfp, "%d %d %d %d %d %d %d %d %d %d %d \n", i, failure_per_po[i][0], failure_per_po[i][1], failure_per_po[i][2], failure_per_po[i][3], failure_per_po[i][4], failure_per_po[i][5], failure_per_po[i][6], failure_per_po[i][7], failure_per_po[i][8], failure_per_po[i][9] );
	}
/*	
	fprintf(resultfp, "\n\nFault_Reachability_Analysis: Gate= %d, FF=%d, PO=%d\n", core->gate_numbers, core->dff_num, core->numPo);
	float po_reach_incr=0;
	for(int i=0;i<config->clock;i++){
		float ff_reach=0, po_reach=0;
		for(int j=0;j<reachable_FF_CK.size();j++){
			ff_reach += reachable_FF_CK[i][j] ;
		}
		ff_reach /= reachable_FF_CK.size();
		
		for(int j=0;j<reachable_PO_CK.size();j++){
			po_reach += reachable_PO_CK[i][j] ;
		}
		po_reach /= reachable_PO_CK.size();		
		po_reach_incr += po_reach ;
		fprintf(resultfp, "Reachability_(FF,PO)_at_clock %d = %f %f %f\n", i, ff_reach, po_reach, po_reach_incr);
	}*/
}


void MBU::system_failure_analysis(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp){
	float total_derating_sum=0;
	float total_derating_iter=0;
	int tsum,tk;
	float tpsum;
	int list_size;
	int max_event_size=0;
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS], pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	vector<float> sat;
	for(int i=0;i<config->clock;i++) sat.push_back(0);
	int sat_stat=0;
	cout << "Starting circuit soft error rate analysis..." << endl ;
	for (int ff_index=0; ff_index<core->gate_numbers; ff_index++){
		cout << ff_index << endl;
		if (core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){
			core->gate_list[ff_index]->derating = 1;
			S_mlet_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			continue;
		}
		
		//------------------------------------------------------------
		//	Fault Site Identification & Generation
		//------------------------------------------------------------		
		cout << "fs-gen" << endl ;
		vector<faultSite> fault_site_list = fsg->getFaultSiteList(ff_index);
		int total_mbu_simulation_per_gate  = fsg->getTotalFaultSite(ff_index);
		//debug:
		//total_mbu_simulation_per_gate=1;
		for(int mbu_index=0; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			//initializing <0,1,0^e,1^e> values for all-gates
			for (int i=0; i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				gateptr->p0 = 1- gateptr->signal_probability;	gateptr->p1 = gateptr->signal_probability;	gateptr->pd = 0;	gateptr->pdn = 0;
				gateptr->is_onpath = false;
			}
			
			// To get PI2POlist starting from flip-flop nff_index excluding it
			if( fault_site_list[mbu_index].target2 == NULL ) topological_sorting(core, config, fault_site_list[mbu_index].target1->out, -1);
			else topological_sorting(core, config, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out);
			int PI2POlist_size = core->PI2POlist.size();
			for (int i=0; i<PI2POlist_size; i++){
				gate* gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
				//cout << gateptr->gate_no << endl ;
				gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;	// Set MAX values to ensure each will get correct value
				gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
				gateptr->is_onpath = true;
			}
			cout << "flt-gen" << endl ;
			//------------------------------------------------------------
			//	Fault Generation
			//------------------------------------------------------------
			// Injecting multiple faults, index 0 is only used in case of single event transients.
			if(fault_site_list[mbu_index].target_count==1){//seu
				analytical_fault_injection( fault_site_list[mbu_index].target1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
			}else{//mbu
				analytical_fault_injection( fault_site_list[mbu_index].target1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
				analytical_fault_injection( fault_site_list[mbu_index].target2, fg->generateFault(core, config, fault_site_list[mbu_index].target2));
			}
			cout << "clk-1" << endl ;
			//------------------------------------------------------------
			//	First CLock Fault Propagation
			//	With Logical, Electrical & Timing derating
			//------------------------------------------------------------
			for (int i=0; i<PI2POlist_size; i++){
				gate* gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
				compute_gate_event_list(gateptr, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out ,config,core);
				if (gateptr->event_list.size()>max_event_size)
					max_event_size = gateptr->event_list.size();
			}// End of PI2POlist
			cout << "Clk1-EPi2po" << endl ;
			for (int i=0; i<core->dff_num; i++){
				gate* gateptr = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
				compute_gate_event_list(gateptr, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out,config,core);
				if (gateptr->event_list.size()>max_event_size)
					max_event_size = gateptr->event_list.size();
			}
			cout << "clk-1*" << endl ;
			//------------------------------------------------------------
			//	Checking if any of the fault's has been latched at DFFs
			//------------------------------------------------------------
			// some DFFs may not be on path!
			//TODO: sort event lists
			
			//cout << "t-period="<< config->tperiod << endl ;
			for(int i=0;i<core->dff_num;i++){
				cout << "*" << i << endl ;
				tsum=0; tk=0;
				gate* gateptr = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
				gate* gptr1 = core->fadjlist_ptr[core->dfflist_ptr[i]->d];
				float tpsum_0=0, tpsum_1=0, tpsum_0e=0, tpsum_1e=0;
				//for(int k=0;k<gateptr->event_list.size();k++){
				//	cout << "event: [" << gateptr->event_list[k]->time << "] -->" << gateptr->event_list[k]->p0 << " " << gateptr->event_list[k]->p1 << " " << gateptr->event_list[k]->pd << " " << gateptr->event_list[k]->pdn << endl ;
				//}
				
				if(gateptr->event_list[0]->time>0){
					tpsum_0 += gateptr->event_list[0]->time * (gateptr->event_list[0]->p0);
					tpsum_1 += gateptr->event_list[0]->time * (gateptr->event_list[0]->p1);
				}
				//cout << tpsum_0 << " " << tpsum_1 << " " << tpsum_0e << " " << tpsum_1e << endl ;
				cout << "Evnt" << endl ;
				for (int k=0; k<gateptr->event_list.size()-1; k++){
					//if( gateptr->event_list[k+1]->time<config->tperiod ){
						tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
						if(tk>0){
							if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0))){
								tpsum_0e += tk * (gateptr->event_list[k]->pd);
								tpsum_1e += tk * (gateptr->event_list[k]->pdn);
							}
							tpsum_0 += tk * (gateptr->event_list[k]->p0);
							tpsum_1 += tk * (gateptr->event_list[k]->p1);
							tsum += tk;
						}
						
					//}
					//cout << " ----> " << tk << " ] "  <<  tpsum_0 << " " << tpsum_1 << " " << tpsum_0e << " " << tpsum_1e << endl ;
				}
				cout << "tk" << endl ;
					//cout << tpsum_0 << " " << tpsum_1 << " " << tpsum_0e << " " << tpsum_1e << endl ;
				tk = config->tperiod - gateptr->event_list[gateptr->event_list.size()-1]->time;
				if(tk>0){
					tpsum_0 += tk * (gateptr->event_list[gateptr->event_list.size()-1]->p0);
					tpsum_1 += tk * (gateptr->event_list[gateptr->event_list.size()-1]->p1);
					tpsum_0e += tk * (gateptr->event_list[gateptr->event_list.size()-1]->pd);
					tpsum_1e += tk * (gateptr->event_list[gateptr->event_list.size()-1]->pdn);
					tsum += tk;
				}
			
				//cout << tpsum_0 << " " << tpsum_1 << " " << tpsum_0e << " " << tpsum_1e << endl ;
				
				cout << "tsum" << endl ;
				if (tsum != 0){
					gateptr->p0 = ((float)tpsum_0+(config->t_setup+config->t_hold)*(float)tpsum_0/(float)tsum)/(float)config->tperiod;
					gateptr->p1 = ((float)tpsum_1+(config->t_setup+config->t_hold)*(float)tpsum_1/(float)tsum)/(float)config->tperiod;
					gateptr->pd = ((float)tpsum_0e+(config->t_setup+config->t_hold)*(float)tpsum_0e/(float)tsum)/(float)config->tperiod;
					gateptr->pdn = ((float)tpsum_1e+(config->t_setup+config->t_hold)*(float)tpsum_1e/(float)tsum)/(float)config->tperiod;
					
					if( gateptr->p0>1 ) gateptr->p0=1 ;
					if( gateptr->p1>1 ) gateptr->p1=1 ;
					if( gateptr->pd>1 ) gateptr->pd=1 ;
					if( gateptr->pdn>1 ) gateptr->pdn=1 ;
				}else{
					gate* gptr1 = core->fadjlist_ptr[core->dfflist_ptr[i]->d];
					gate* gptr2 = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;
					gptr2->pd = 0;
					gptr2->pdn = 0;
				}
			//cout << "<0,1,0e,1e>(" << i <<  ") = < " << gateptr->p0 << " , " << gateptr->p1 << " , " << gateptr->pd << " , " << gateptr->pdn << " > " << endl ;
			}
			cout << "clk-1**" << endl ;
			//cout << "----------------------------" << endl ;
			
			
			SatCK[ff_index][0] = 1;
			for (int i=0; i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				if( gateptr->po_no>=0 ){
					if( gateptr->event_list.size()>0){
						list_size = gateptr->event_list.size();
						tpsum = 0;
						tsum = 0;
						for (int k=0; k<gateptr->event_list.size()-1; k++){
							tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
							if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
								tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
							tsum += tk;
						}
						if(tpsum>0){
							if( ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod >= 1 ) SatCK[ff_index][0] =0;
							else SatCK[ff_index][0] *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
						}
					}
				}
			}			
			SatCK[ff_index][0] =  1 - SatCK[ff_index][0] ;
			for (int i=0;i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				if( !gateptr->is_dff ){
					gateptr->p0 = 0 ;
					gateptr->p1 = 0 ;
					gateptr->pd = 0 ;
					gateptr->pdn = 0 ;
				}
				list_size = gateptr->event_list.size();
				
				for (int j=list_size-1; j>=0; j--){
					event* eventptr = gateptr->event_list[j];
					delete eventptr;
					eventptr = NULL;
					gateptr->event_list.pop_back();
				}
			}
			
			cout << "clk2+" << endl ;
			//------------------------------------------------------------
			//	Second+ Cycle Fault Propagation
			//	With Only Logical Derating
			//------------------------------------------------------------
			for(int clock=1; clock<config->clock; clock++){
				//PART1:
				//	logical simulation for all gates (except DFFs)
				//	for PIs, we use Signal probabilities.
				for (int i=0; i<core->gate_numbers; i++){
					gate* gateptr = core->gate_list[i];
					if (gateptr->is_dff == 1) continue;		// we will compute the flip-flops at the end
					for (int j=0; j<gateptr->input_count; j++){
						if ((gateptr->input_list[j]) <= core->numPi){	// if input is Primary Input
							p0_values[j] = 1- core->PI_list_sp[gateptr->input_list[j]];
							p1_values[j] = core->PI_list_sp[gateptr->input_list[j]];
							pd_values[j] = 0; pdn_values[j] = 0;
							continue;
						}else{
							gate* pred_gateptr = core->fadjlist_ptr[gateptr->input_list[j]];
							p0_values[j] = pred_gateptr->p0;
							p1_values[j] = pred_gateptr->p1;
							pd_values[j] = pred_gateptr->pd;
							pdn_values[j] = pred_gateptr->pdn;
						}
					}
					compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
				}
				
				//PART2: 
				//	Copying input of DFFs to it's output for next cycle
				for (int i=0; i<core->dff_num; i++){
					gate* gptr1 = core->fadjlist_ptr[core->dfflist_ptr[i]->d];
					gate* gptr2 = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;
					gptr2->pd = gptr1->pd;
					gptr2->pdn = gptr1->pdn;
					if(gptr2->p0>1) gptr2->p0=1 ;
					if(gptr2->p1>1) gptr2->p1=1 ;
					if(gptr2->pd>1) gptr2->pd=1 ;
					if(gptr2->pdn>1) gptr2->pdn=1 ;
					//cout << "*** clk=1+ [old values] \t" << gptr2->pd <<  " " <<  gptr2->pdn << endl ;  
					//cout << "*** clk=1+ " << gptr2->pd <<  " " <<  gptr2->pdn << endl ;  
				}
				
				//PART3: 
				//	Soft Error Rate Computation!
				SatCK[ff_index][clock] = 1;
				for (int i=0; i<core->gate_numbers; i++){
					gate* gptr2 = core->gate_list[i];
					if (gptr2->po_no>=0) SatCK[ff_index][clock] *= 1 - (gptr2->pd + gptr2->pdn);
				}
				SatCK[ff_index][clock] = 1 - SatCK[ff_index][clock];
			}//clock loop (clock 2..n)
			
			//------------------------------------------------------------
			//	Soft-Error Rate Computation
			//------------------------------------------------------------
			float mul_fact=1, prev_sum=0;
			for (int clk_index=0; clk_index<config->clock; clk_index++){
				S1_to_CK[ff_index][clk_index] = SatCK[ff_index][clk_index]*mul_fact + prev_sum;
				mul_fact *= 1 - SatCK[ff_index][clk_index];
				prev_sum = S1_to_CK[ff_index][clk_index];
			}

			sat_stat++ ;
			for(int clk_index=0; clk_index<config->clock; clk_index++){
				sat[clk_index] += S1_to_CK[ff_index][clk_index] ;
				//cout << clk_index << " " << S1_to_CK[ff_index][clk_index] << endl ;
			}
			S_mlet_vec[ff_index].push_back( ser(0,0, S1_to_CK[ff_index][config->clock-1]) );
		}//mbu loop		
		
		float sum = 0;
		for(int i=0;i<total_mbu_simulation_per_gate;i++){
			sum += S_mlet_vec[ff_index].at(i).derating ;
		}
		sum /= total_mbu_simulation_per_gate; //-1;		// -1 is for considering only met (not set)
		core->gate_list[ff_index]->derating = sum;
		
		//cout << "----------------------------------------" << endl ;
		//cout << ff_index << "             " << sum << endl ;
		//cout << "----------------------------------------" << endl ;
		
	}//gate loop (ff_index)
	for(int i=0;i<config->clock;i++) sat[i] /= sat_stat; 
	fprintf(resultfp, "Saturation clock report (don't forget to add a PO-connected-gate offset!\n");
	for(int i=0;i<config->clock;i++){
		fprintf(resultfp, "%d  %f\n" , i , sat[i] );
	}
	float overall_sys_der = 0;
	for (int i=0; i<core->gate_numbers; i++){
		fprintf(resultfp, "gate %d = %f\n", i, core->gate_list[i]->derating);
		overall_sys_der += core->gate_list[i]->derating;
	}
	overall_sys_der = overall_sys_der / (float)(core->gate_numbers);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall_Systematic_Derating = %f \n", overall_sys_der);
	fprintf(resultfp,"Overall_Systematic_derating_per_unit = %f \n", total_derating_sum/total_derating_iter);
	fprintf(resultfp, "Max_event_size_in_systematic_derating= %d \n", max_event_size);	
}

void MBU::system_failure_analysis_single(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp){
	float total_derating_sum=0;
	float total_derating_iter=0;
	int tsum,tk;
	float tpsum;
	int list_size;
	int max_event_size=0;
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS], pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	vector<float> sat;
	for(int i=0;i<config->clock;i++) sat.push_back(0);
	int sat_stat=0;
	cout << "Starting circuit soft error rate analysis..." << endl ;
	for (int ff_index=0; ff_index<core->gate_numbers; ff_index++){
		if (core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){
			core->gate_list[ff_index]->derating = 1;
			S_mlet_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			continue;
		}
		
		//------------------------------------------------------------
		//	Fault Site Identification & Generation
		//------------------------------------------------------------		
		vector<faultSite> fault_site_list = fsg->getFaultSiteList(ff_index);
		int total_mbu_simulation_per_gate  = fsg->getTotalFaultSite(ff_index);
		//debug:
		//total_mbu_simulation_per_gate=1;
		for(int mbu_index=0; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			//initializing <0,1,0^e,1^e> values for all-gates
			for (int i=0; i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				gateptr->p0 = 1- gateptr->signal_probability;	gateptr->p1 = gateptr->signal_probability;	gateptr->pd = 0;	gateptr->pdn = 0;
				gateptr->is_onpath = false;
			}
			
			// To get PI2POlist starting from flip-flop nff_index excluding it
			if( fault_site_list[mbu_index].target2 == NULL ) topological_sorting(core, config, fault_site_list[mbu_index].target1->out, -1);
			else topological_sorting(core, config, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out);
			int PI2POlist_size = core->PI2POlist.size();
			for (int i=0; i<PI2POlist_size; i++){
				gate* gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
				//cout << gateptr->gate_no << endl ;
				gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;	// Set MAX values to ensure each will get correct value
				gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
				gateptr->is_onpath = true;
			}
			
			//------------------------------------------------------------
			//	Fault Generation
			//------------------------------------------------------------
			// Injecting multiple faults, index 0 is only used in case of single event transients.
			if(fault_site_list[mbu_index].target_count==1){//seu
				analytical_fault_injection( fault_site_list[mbu_index].target1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
			}else{//mbu
				analytical_fault_injection( fault_site_list[mbu_index].target1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
				analytical_fault_injection( fault_site_list[mbu_index].target2, fg->generateFault(core, config, fault_site_list[mbu_index].target2));
			}
			//------------------------------------------------------------
			//	First CLock Fault Propagation
			//	With Logical, Electrical & Timing derating
			//------------------------------------------------------------
			for (int i=0; i<PI2POlist_size; i++){
				gate* gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
				compute_gate_event_list(gateptr, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out ,config,core);
				if (gateptr->event_list.size()>max_event_size)
					max_event_size = gateptr->event_list.size();
			}// End of PI2POlist
			for (int i=0; i<core->dff_num; i++){
				gate* gateptr = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
				compute_gate_event_list(gateptr, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out,config,core);
				if (gateptr->event_list.size()>max_event_size)
					max_event_size = gateptr->event_list.size();
			}
			//------------------------------------------------------------
			//	Checking if any of the fault's has been latched at DFFs
			//------------------------------------------------------------
			// some DFFs may not be on path!
			//------------------------------------------------------------
			//	Soft-Error Rate Computation
			//------------------------------------------------------------
			short MAXS=3,NMAXS=-1;
			float sys_failure=1;
			for (int i=0; i<core->dff_num; i++){
				gate* gateptr = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
				list_size = gateptr->event_list.size();
				tpsum = 0;
				tsum = 0;
				for (int k=0; k<gateptr->event_list.size()-1; k++){
					tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
					if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
						tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
					tsum += tk;
				}
				//if (tsum != 0) sys_failure *= 1- ((float)tpsum+(config->t_hold + config->t_setup)*(float)tpsum/(float)tsum)/(float)config->tperiod;
				if (tsum != 0) sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				//if (tsum != 0) sys_failure *= 1- ((float)tpsum-(config->t_hold + config->t_setup))/((float)config->tperiod - config->seu_pulse_width );
			}
			//Need revise.
			for (int gate_index=0; gate_index<core->gate_numbers; gate_index++){
				gate* gateptr = core->gate_list[gate_index];
				if (gateptr->po_no>=0) sys_failure *= 1 - (gateptr->pd + gateptr->pdn);
			}
			
						
			SatCK[ff_index][0] = 1;
			for (int i=0; i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				if( gateptr->po_no>=0 ){
					if( gateptr->event_list.size()>0){
						list_size = gateptr->event_list.size();
						tpsum = 0;
						tsum = 0;
						for (int k=0; k<gateptr->event_list.size()-1; k++){
							tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
							if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
								tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
							tsum += tk;
						}
						if(tpsum>0){
							if( ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod >= 1 ) sys_failure=0;
							else sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
						}
					}
				}
			}		
			
			
			sys_failure = 1 - sys_failure;
			//ff_index is gate_list index
			core->gate_list[ff_index]->derating = sys_failure; 
			
			
			for (int i=0;i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				if( !gateptr->is_dff ){
					gateptr->p0 = 0 ;
					gateptr->p1 = 0 ;
					gateptr->pd = 0 ;
					gateptr->pdn = 0 ;
				}
				list_size = gateptr->event_list.size();
				
				for (int j=list_size-1; j>=0; j--){
					event* eventptr = gateptr->event_list[j];
					delete eventptr;
					eventptr = NULL;
					gateptr->event_list.pop_back();
				}
			}
			S_mlet_vec[ff_index].push_back( ser(0,0, sys_failure) );
		}//mbu loop		
		
		float sum = 0;
		for(int i=0;i<total_mbu_simulation_per_gate;i++){
			sum += S_mlet_vec[ff_index].at(i).derating ;
		}
		sum /= total_mbu_simulation_per_gate; //-1;		// -1 is for considering only met (not set)
		core->gate_list[ff_index]->derating = sum;
		
		//cout << "----------------------------------------" << endl ;
		//cout << ff_index << "             " << sum << endl ;
		//cout << "----------------------------------------" << endl ;
		
	}//gate loop (ff_index)
	
	float overall_sys_der = 0;
	for (int i=0; i<core->gate_numbers; i++){
		fprintf(resultfp, "gate %d = %f\n", i, core->gate_list[i]->derating);
		overall_sys_der += core->gate_list[i]->derating;
	}
	overall_sys_der = overall_sys_der / (float)(core->gate_numbers);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall_Systematic_Derating = %f \n", overall_sys_der);
	fprintf(resultfp,"Overall_Systematic_derating_per_unit = %f \n", total_derating_sum/total_derating_iter);
	fprintf(resultfp, "Max_event_size_in_systematic_derating= %d \n", max_event_size);	
}



void MBU::system_failure_simulation_single_cycle(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp){
	int total_failures;
	float total_derating_sum=0;
	float total_derating_iter=0;
	float total_sim_variance;
	float total_sim_derating;
	float error_tolerance = derating_sim_error_tolerance;
	long total_iteration;
	float z_alpha_half = 2.576;
	int max_event_size;
	
	total_iteration = 0;
	max_event_size=0;
	total_failures=0;
	
	struct gate *gateptr;
	struct gate tgateptr1, tgateptr2;
	struct gate *pred_gateptr;
	struct event* eventptr;
	
	for (int ff_index=0; ff_index<core->sim_gate_numbers; ff_index++){
		cout << "Simulating gate# " << ff_index << " ... ";
		if ((core->gate_list[ff_index]->is_dff == 1) || ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo))){
			core->gate_list[ff_index]->sderating = 1;
			S_sim_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			cout << endl;
			continue;
		}
		
		
		//------------------------------------------------------------
		//	Fault Site Identification & Generation
		//------------------------------------------------------------		
		vector<faultSite> fault_site_list = fsg->getFaultSiteList(ff_index);
		int total_mbu_simulation_per_gate  = fsg->getTotalFaultSite(ff_index);
		int number_of_common_fanout_injection= core->gate_list[ff_index]->forward_list.size() * (core->gate_list[ff_index]->forward_list.size()-1) /2 ;
		int number_of_common_fanin_injection  = core->gate_list[ff_index]->backward_list.size() * (core->gate_list[ff_index]->backward_list.size()-1) /2;
		
		for(int mbu_index=1; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			core->gate_list[ff_index]->sderating = 0;
			int total_iteration = 0;
			int max_event_size=0;
			int total_failures=0;
			do{
				for (int gate_index=0; gate_index<config->step_iteration; gate_index++){
					if (core->gate_list[ff_index]->is_dff == 1){
						gate_index--;
						continue;
					}
					//------------------------------------------------------------
					//	set a random vulue to FFs and PIs;
					//------------------------------------------------------------	
					//Assign random values to the inputs
					for (int i=1; i<=core->numPi; i++){
						long rand_value = random();
						//Assigning random loginc to Primary Input
						short rand_logic = rand_value & 1;
						core->PI_list[i] = rand_logic;
						// Forwording the PI's value to its successors
						short node_adj_size = core->fadjlist[i].size();
						for(int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[i][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; 
								if (pred_gate_node_number==i)
									gateptr->input_values[k] = core->PI_list[i];
							}
						}
					}
					//Assign random values to DFFs
					for (int i=0; i<core->dff_num; i++){
						long rand_value = random();
						short rand_logic= rand_value & 0x1; 
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = rand_logic;
						pred_gateptr->input_values[0] = rand_logic; //may be not required
						int fadjnum = core->dfflist_ptr[i]->q;
						short node_adj_size =core->fadjlist[fadjnum].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadjnum][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadjnum)
									gateptr->input_values[k] = pred_gateptr->value;
							}
						}
					}
					//------------------------------------------------------------
					//	Initial logic Simulation
					//	We need to know the value of the net before injecting faults
					//	so we need an initial logic simulation
					//------------------------------------------------------------	
					//Propagating the input values through intermediate gates
					for (int i=0; i<core->gate_numbers; i++){
						pred_gateptr = core->gate_list[i];
						short fadj_number = pred_gateptr->out;
						if (pred_gateptr->is_dff == 1) continue;
						core->compute_gate_value(pred_gateptr);
						short node_adj_size =core->fadjlist[fadj_number].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadj_number][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
           						pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadj_number)
									gateptr->input_values[k] = pred_gateptr->value;
							}	
						}
					}
					
					//Propagating the values through FFs
					for (int i=0; i<core->dff_num; i++){
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = pred_gateptr->input_values[0];
					}
					//------------------------------------------------------------
					//	End of Logic Simulation
					//------------------------------------------------------------	
					
					//------------------------------------------------------------
					//	Monte Carlo Simulation
					//------------------------------------------------------------	
					// starting from sorted gate list and compute events for each gate in order
					bool fault_not_injected_yet = true ;
					for (int j=0; j<core->gate_numbers; j++){
						gateptr = core->gate_list[j];
						if (gateptr->is_dff == 1) continue;
						
						compute_gate_event_list(gateptr,core,config);
						
						if (gateptr->event_list.size()>max_event_size) max_event_size = gateptr->event_list.size();
						bool this_is_target_gate = (core->gate_list[j]->out == fault_site_list[mbu_index].target1->out) || (core->gate_list[j]->out == fault_site_list[mbu_index].target2->out) ;
						//cout << "*3" << endl ;
						if( fault_not_injected_yet && this_is_target_gate){
							fault_not_injected_yet = false;
							int t1 = random() % config->tperiod; // circuit_period;
							//removing events >t1
							gate* target_gate_1 = fault_site_list[mbu_index].target1 ;
							gate* target_gate_2 = fault_site_list[mbu_index].target2 ;
							
							int list_size = target_gate_1->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (target_gate_1->event_list[k]->time >= t1){
									eventptr = target_gate_1->event_list[k];
									target_gate_1->event_list.pop_back();
									tgateptr1.event_list.push_front(eventptr);
								}
							}
							list_size = target_gate_2->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (target_gate_2->event_list[k]->time >= t1){
									eventptr = target_gate_2->event_list[k];
									target_gate_2->event_list.pop_back();
									tgateptr2.event_list.push_front(eventptr);
								}
							}
							
							//------------------------------------------------------------
							//	Fault Generation
							//------------------------------------------------------------
							//cout << "[X] Injecting fault events" << endl ;
							if(fault_site_list[mbu_index].target_count==1){//seu
								statistical_fault_injection( fault_site_list[mbu_index].target1, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
							}else{//mbu
								statistical_fault_injection( fault_site_list[mbu_index].target1, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
								statistical_fault_injection( fault_site_list[mbu_index].target2, t1, fg->generateFault(core, config, fault_site_list[mbu_index].target2));
							}
							
							list_size = tgateptr1.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr1.event_list[0];
								tgateptr1.event_list.pop_front();
								if (eventptr->time > (fg->generateFault(core, config, fault_site_list[mbu_index].target1)+t1))
									target_gate_1->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
							
							list_size = tgateptr2.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr2.event_list[0];
								tgateptr2.event_list.pop_front();
								if (eventptr->time > (fault_site_list[mbu_index].target2->out+t1))
									target_gate_2->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
						}
					}
					bool failure_found = false;
					int event_time1,event_time2;
					for (int j=0; j<core->dff_num; j++){
						gateptr = core->fadjlist_ptr[core->dfflist_ptr[j]->q];
						
						compute_gate_event_list(gateptr,core,config);
						
						for (int k=0; k<gateptr->event_list.size()-1; k++){
							//if there is an event (ti,D) such that tperiod-(tsu+th)<ti <tperiod
							event_time1 = gateptr->event_list[k]->time;
							event_time2 = gateptr->event_list[k+1]->time;
							//if ((event_time >= (tperiod-tsu)) && (event_time <= (tperiod+th) ) )
							int tperiod = config->tperiod ;
							int tsu		= config->t_setup ;
							int th		= config->t_hold  ;
							if (   ((event_time1 <= (tperiod-tsu)) && (event_time2 >= (tperiod-tsu)))
								|| ((event_time1 <= (tperiod+th)) && (event_time2 >= (tperiod+th)))
								|| ((event_time1 <= (2*tperiod-tsu)) && (event_time2 >= (2*tperiod-tsu)))
								|| ((event_time1 <= (2*tperiod+th)) && (event_time2 >= (2*tperiod+th)))
								|| ((event_time1 <= (3*tperiod-tsu)) && (event_time2 >= (3*tperiod-tsu)))
								|| ((event_time1 <= (3*tperiod+th)) && (event_time2 >= (3*tperiod+th)))
								|| ((event_time1 <= (4*tperiod-tsu)) && (event_time2 >= (4*tperiod-tsu)))
								|| ((event_time1 <= (4*tperiod+th)) && (event_time2 >= (4*tperiod+th)))
								|| ((event_time1 <= (5*tperiod-tsu)) && (event_time2 >= (5*tperiod-tsu)))
								|| ((event_time1 <= (5*tperiod+th)) && (event_time2 >= (5*tperiod+th)))
								){
								if ((gateptr->event_list[k]->pd !=0) || (gateptr->event_list[k]->pdn !=0)){
									failure_found = true;
									break;
								}
							}
						}
						if (failure_found == true)
							break;
					}
					if(failure_found == false){
						for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
							gateptr = core->fadjlist_ptr[i];
							for(int j=0;j<gateptr->event_list.size()-1;j++){
								if ((gateptr->event_list[j]->pd !=0) || (gateptr->event_list[j]->pdn !=0)){
									failure_found = true;
								}
							}
							
						}
					}
					
				
					if(failure_found==true){
						total_failures++;
					}
					for (int i=0;i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						int list_size = gateptr->event_list.size();
						//fprintf(resultfp,"\n Gate %d:  type=%d",gateptr->out,gateptr->type);
						for (int j=0; j<list_size; j++){
							eventptr = gateptr->event_list[j];
						}
						for (int j=list_size-1; j>=0; j--){
							eventptr = gateptr->event_list[j];
							delete eventptr;
							eventptr = NULL;
							gateptr->event_list.pop_back();
						}
					}
				}//for fault injection
				total_iteration += config->step_iteration;
				total_sim_derating = (float)total_failures / (float)total_iteration;
				total_sim_variance = sqrt((total_sim_derating * (1 - total_sim_derating))/(float)total_iteration);
				if (total_sim_variance==0)
					total_sim_variance = sqrt(((1.0/(float)total_iteration) * (1 - (1.0/(float)total_iteration)))/(float)total_iteration);
				
			} while (((z_alpha_half*total_sim_variance) > error_tolerance) && (total_iteration<config->max_iteration) );
			core->gate_list[ff_index]->sderating = total_sim_derating;
			if( mbu_index==0 ){//this gate
				S_sim_vec[ff_index].push_back( ser(0,0,total_sim_derating) );
			}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() ){//forward
				S_sim_vec[ff_index].push_back( ser(1,0,total_sim_derating) );
			}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size()){//backward
				S_sim_vec[ff_index].push_back( ser(2,0,total_sim_derating) );
			}else if( mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size() + number_of_common_fanin_injection ){
				S_sim_vec[ff_index].push_back( ser(3,0,total_sim_derating) );
			}else{
				S_sim_vec[ff_index].push_back( ser(4,0,total_sim_derating) );
			}
			total_derating_sum += total_sim_derating ;
			total_derating_iter++ ;
		}//mbu
		float sum=0;
		for(int i=0;i<total_mbu_simulation_per_gate-1;i++){
			sum += S_sim_vec[ff_index].at(i).derating ;
		}
		sum /= total_mbu_simulation_per_gate-1;
		core->gate_list[ff_index]->sderating = sum;
		cout << endl;
		
	}//for (ff_index=0;
	
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Simulation Derating for all gates finished\n");
	
	float overall_sim_der = 0;
	for (int i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		//if (gateptr->is_dff==1)
		//	continue;
		//fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sim_der += gateptr->sderating;
		cout << i << " " <<  gateptr->sderating << endl ;
	}
	//overall_sim_der = overall_sim_der / (float)(core->gate_numbers-core->dff_num);	// ???
	overall_sim_der = overall_sim_der / (float)(core->gate_numbers);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall simulation Derating = %f \n", overall_sim_der);
	fprintf(resultfp,"Overall simulation derating per unit = %f \n", total_derating_sum/total_derating_iter);
	
	float** g = new float *[core->gate_numbers] ;
	int** c = new int *[core->gate_numbers] ;
	int* x = new int[core->gate_numbers];
	for(int i=0;i<core->gate_numbers;i++) g[i] = new float[5];
	for(int i=0;i<core->gate_numbers;i++) c[i] = new int[5];
	for(int i=0;i<core->gate_numbers;i++){
		x[i] = 0 ;
		for(int j=0;j<5;j++){
			g[i][j]=0;
			c[i][j]=0;
		}
	}
	
	for(int i=0;i<core->gate_numbers;i++){
		for(int j=0;j<S_sim_vec[i].size();j++){
			g[i][S_sim_vec[i].at(j).type] += S_sim_vec[i].at(j).derating;
			c[i][S_sim_vec[i].at(j).type]++;
			x[i]++;
		}
	}
	
	float der0=0, der1=0, der2=0, der3=0, der4=0 ;
	for(int i=0;i<core->gate_numbers;i++){		
		der0 += g[i][0]/x[i];
		der1 += g[i][1]/x[i];
		der2 += g[i][2]/x[i];
		der3 += g[i][3]/x[i];
		der4 += g[i][4]/x[i];
	}
	fprintf(resultfp,"D0 = %f \n", der0/core->gate_numbers);
	fprintf(resultfp,"D1 = %f \n", der1/core->gate_numbers);
	fprintf(resultfp,"D2 = %f \n", der2/core->gate_numbers);
	fprintf(resultfp,"D3 = %f \n", der3/core->gate_numbers);
	fprintf(resultfp,"D4 = %f \n", der4/core->gate_numbers);
	
	
	for(int i=0;i<core->gate_numbers;i++){ delete g[i];  delete c[i]; }
	delete g; delete c;

	
}

void MBU::system_failure_analysis_single_cycle(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp){
	float total_derating_sum=0;
	float total_derating_iter=0;
	int tsum,tk;
	float tpsum;
	int list_size;
	int max_event_size=0;
	cout << "Starting circuit soft error rate analysis..." << endl ;
	for (int ff_index=0; ff_index<core->gate_numbers; ff_index++){
		if ((core->gate_list[ff_index]->is_dff == 1) || (core->gate_list[ff_index]->out <= (core->numPi+core->numPo))){ //uncomment this for single cycle
			//if (core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){
			core->gate_list[ff_index]->derating = 1;
			S_mlet_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			continue;
		}
		
		//------------------------------------------------------------
		//	Fault Site Identification & Generation
		//------------------------------------------------------------		
		vector<faultSite> fault_site_list = fsg->getFaultSiteList(ff_index);
		int total_mbu_simulation_per_gate  = fsg->getTotalFaultSite(ff_index);
		
		//int number_of_common_fanout_injection= core->gate_list[ff_index]->forward_list.size() * (core->gate_list[ff_index]->forward_list.size()-1) /2 ;
		//int number_of_common_fanin_injection  = core->gate_list[ff_index]->backward_list.size() * (core->gate_list[ff_index]->backward_list.size()-1) /2;
		
	
		//By starting mbu_index from 1, we exclude injecting SETs, here we only inject METs.
		for(int mbu_index=0; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			//initializing <0,1,0^e,1^e> values for all-gates
			for (int i=0; i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				gateptr->p0 = 1- gateptr->signal_probability;	gateptr->p1 = gateptr->signal_probability;	gateptr->pd = 0;	gateptr->pdn = 0;
				gateptr->is_onpath = false;
			}
			
			// To get PI2POlist starting from flip-flop nff_index excluding it
			if( fault_site_list[mbu_index].target2 == NULL ) topological_sorting(core, config, fault_site_list[mbu_index].target1->out, -1);
			else topological_sorting(core, config, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out);
			int PI2POlist_size = core->PI2POlist.size();
			for (int i=0; i<PI2POlist_size; i++){
				gate* gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
				gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;	// Set MAX values to ensure each will get correct value
				gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
				gateptr->is_onpath = true;
			}
			
			//------------------------------------------------------------
			//	Fault Generation
			//------------------------------------------------------------
			// Injecting multiple faults, index 0 is only used in case of single event transients.
			if(fault_site_list[mbu_index].target_count==1){//seu
				analytical_fault_injection( fault_site_list[mbu_index].target1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
			}else{//mbu
				analytical_fault_injection( fault_site_list[mbu_index].target1, fg->generateFault(core, config, fault_site_list[mbu_index].target1));
				analytical_fault_injection( fault_site_list[mbu_index].target2, fg->generateFault(core, config, fault_site_list[mbu_index].target2));
			}
			
			//------------------------------------------------------------
			//	Fault Propagation
			//------------------------------------------------------------
			for (int i=0; i<PI2POlist_size; i++){
				gate* gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
				compute_gate_event_list(gateptr, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out ,config,core);
				if (gateptr->event_list.size()>max_event_size)
					max_event_size = gateptr->event_list.size();
			}// End of PI2POlist
			
			for (int i=0; i<core->dff_num; i++){
				gate* gateptr = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
				compute_gate_event_list(gateptr, fault_site_list[mbu_index].target1->out, fault_site_list[mbu_index].target2->out ,config,core);
				if (gateptr->event_list.size()>max_event_size)
					max_event_size = gateptr->event_list.size();
			}
			
			//------------------------------------------------------------
			//	Soft-Error Rate Computation
			//------------------------------------------------------------
			short MAXS=3,NMAXS=-1;
			float sys_failure=1;
			for (int i=0; i<core->dff_num; i++){
				gate* gateptr = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
				list_size = gateptr->event_list.size();
				tpsum = 0;
				tsum = 0;
				for (int k=0; k<gateptr->event_list.size()-1; k++){
					tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
					if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
						tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
					tsum += tk;
				}
				//if (tsum != 0) sys_failure *= 1- ((float)tpsum+(config->t_hold + config->t_setup)*(float)tpsum/(float)tsum)/(float)config->tperiod;
				if (tsum != 0) sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				//if (tsum != 0) sys_failure *= 1- ((float)tpsum-(config->t_hold + config->t_setup))/((float)config->tperiod - config->seu_pulse_width );
			}
			//Need revise.
			//for (int gate_index=0; gate_index<core->gate_numbers; gate_index++){
			//	gate* gateptr = core->gate_list[gate_index];
			//	if (gateptr->po_no>=0) sys_failure *= 1 - (gateptr->pd + gateptr->pdn);
			//}
			
			for (int i=0; i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				if( gateptr->po_no>=0 ){
					if( gateptr->event_list.size()>0){
						list_size = gateptr->event_list.size();
						tpsum = 0;
						tsum = 0;
						for (int k=0; k<gateptr->event_list.size()-1; k++){
							tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
							if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
								tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
							tsum += tk;
						}
						if(tpsum>0){
							if( ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod >= 1 ) sys_failure =0;
							else sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
							//else sys_failure *= 1- ((float)tpsum+(config->t_hold + config->t_setup)*(float)tpsum/(float)tsum)/(float)config->tperiod; 
						}
					}
				}
			}			
			
			sys_failure = 1 - sys_failure;
			//ff_index is gate_list index
			core->gate_list[ff_index]->derating = sys_failure;
			S_mlet_vec[ff_index].push_back( ser(0,0,sys_failure) );
			/*
			if( mbu_index==0 ){//this gate
				S_mlet_vec[ff_index].push_back( ser(0,0,sys_failure) );
			}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() ){//forward
				S_mlet_vec[ff_index].push_back( ser(1,0,sys_failure) );
			}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size()){//backward
				S_mlet_vec[ff_index].push_back( ser(2,0,sys_failure) );
			}else if( mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size() + number_of_common_fanin_injection ){
				S_mlet_vec[ff_index].push_back( ser(3,0,sys_failure) );
			}else{
				S_mlet_vec[ff_index].push_back( ser(4,0,sys_failure) );
			}*/
			//S_mlet_vec[ff_index].push_back( ser(0,0,sys_failure) );
			total_derating_sum += sys_failure ;
			total_derating_iter++ ;
			for (int i=0;i<core->gate_numbers; i++){
				gate* gateptr = core->gate_list[i];
				list_size = gateptr->event_list.size();
				
				for (int j=list_size-1; j>=0; j--){
					event* eventptr = gateptr->event_list[j];
					delete eventptr;
					eventptr = NULL;
					gateptr->event_list.pop_back();
				}
			}
		}//mbu		
		
		float sum = 0;
		for(int i=0;i<total_mbu_simulation_per_gate-1;i++){
			cout <<  S_mlet_vec[ff_index].at(i).derating << " " << total_mbu_simulation_per_gate <<  endl ; 
			sum += S_mlet_vec[ff_index].at(i).derating ;
		}
		sum /= total_mbu_simulation_per_gate-1;
		core->gate_list[ff_index]->derating = sum;
		//cout << "sum=" << sum << endl ;
	}//for
	
	float overall_sys_der = 0;
	for (int i=0; i<core->gate_numbers; i++){
		//gateptr = core->gate_list[i];
		//if (gateptr->is_dff==1)
		//	continue;
		//fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sys_der += core->gate_list[i]->derating;
		//cout << i << " " << overall_sys_der << endl ;
	}
	overall_sys_der = overall_sys_der / (float)(core->gate_numbers);	// ???
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Systematic Derating = %f \n", overall_sys_der);
	fprintf(resultfp,"Overall Systematic derating per unit = %f \n", total_derating_sum/total_derating_iter);
	fprintf(resultfp, "Max event size in systematic derating=%d \n", max_event_size);	
	
	/*
	int mbu_total[5] = {0,0,0,0,0};
	float** g = new float *[core->gate_numbers] ;
	int** c = new int *[core->gate_numbers] ;
	int* x = new int[core->gate_numbers] ;
	for(int i=0;i<core->gate_numbers;i++) g[i] = new float[5];
	for(int i=0;i<core->gate_numbers;i++) c[i] = new int[5];
	for(int i=0;i<core->gate_numbers;i++){
		x[i]=0;
		for(int j=0;j<5;j++){
			g[i][j]=0;
			c[i][j]=0;
		}
	}
	
	for(int i=0;i<core->gate_numbers;i++){
		for(int j=0;j<S_mlet_vec[i].size();j++){
			g[i][S_mlet_vec[i].at(j).type] += S_mlet_vec[i].at(j).derating;
			x[i]++;
			c[i][S_mlet_vec[i].at(j).type]++;
			mbu_total[S_mlet_vec[i].at(j).type]++;
		}
	}
	
	float der0=0, der1=0, der2=0, der3=0, der4=0 ;
	for(int i=0;i<core->gate_numbers;i++){
		//		if(c[i][0]!=0) der0 += g[i][0]/c[i][0];
		//	if(c[i][1]!=0) der1 += g[i][1]/c[i][1];
		//	if(c[i][2]!=0) der2 += g[i][2]/c[i][2];
		//	if(c[i][3]!=0) der3 += g[i][3]/c[i][3];
		//	if(c[i][4]!=0) der4 += g[i][4]/c[i][4];
		//cout << g[i][0] << " " << g[i][1] << " " << g[i][2] << " " << g[i][3] << " " << g[i][4] << " -- " << x[i] << endl ;
		der0 += g[i][0]/x[i];
		der1 += g[i][1]/x[i];
		der2 += g[i][2]/x[i];
		der3 += g[i][3]/x[i];
		der4 += g[i][4]/x[i];
	}
	fprintf(resultfp,"D0 = %f \n", der0/core->gate_numbers);
	fprintf(resultfp,"D1 = %f \n", der1/core->gate_numbers);
	fprintf(resultfp,"D2 = %f \n", der2/core->gate_numbers);
	fprintf(resultfp,"D3 = %f \n", der3/core->gate_numbers);
	fprintf(resultfp,"D4 = %f \n", der4/core->gate_numbers);
	
	fprintf(resultfp,"[SIM] %d %d %d %d %d\n", mbu_total[0], mbu_total[1], mbu_total[2], mbu_total[3] , mbu_total[4]);
	
	for(int i=0;i<core->gate_numbers;i++){ delete g[i];  delete c[i]; }
	delete g; delete c;
	*/
}

/*
//////////////////////////////////////////////////////////////////////////////////////
// Function: timing_derating_sim
// Purpose:  timing_derating_sim
// Returns none
void MBU::system_failure_simulation_SingleCycle_LET(Core* core, configuration* config, FILE* resultfp){
	int total_failures;
	float total_derating_sum=0;
	float total_derating_iter=0;
	float total_sim_variance;
	float total_sim_derating;
	float error_tolerance = derating_sim_error_tolerance;
	long total_iteration;
	float z_alpha_half = 2.576;
	int max_event_size;

	total_iteration = 0;
	max_event_size=0;
	total_failures=0;

	struct gate *gateptr;
	struct gate tgateptr;
	struct gate *pred_gateptr;
	struct event* eventptr;

	for (int ff_index=0; ff_index<core->sim_gate_numbers; ff_index++){
		cout << "Simulating gate# " << ff_index << " ... ";
		if ((core->gate_list[ff_index]->is_dff == 1) || ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo))){
			core->gate_list[ff_index]->sderating = 1;
			S_sim_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			cout << endl;
			continue;
		}
		//int number_of_common_fanout_injection= fact( core->gate_list[ff_index]->forward_list.size() ) / 2 * fact( core->gate_list[ff_index]->forward_list.size() - 2 ); ;
		//int number_of_common_fanin_injection  = fact( core->gate_list[ff_index]->backward_list.size() ) / 2 * fact( core->gate_list[ff_index]->backward_list.size() - 2 );;
		
		int number_of_common_fanout_injection= core->gate_list[ff_index]->forward_list.size() * (core->gate_list[ff_index]->forward_list.size()-1) /2 ;
		int number_of_common_fanin_injection  = core->gate_list[ff_index]->backward_list.size() * (core->gate_list[ff_index]->backward_list.size()-1) /2;
		
		int total_mbu_simulation_per_gate = 1 + // inject one fault on this gate/
			core->gate_list[ff_index]->forward_list.size() + // inject two fault, one on this gate and one on it's fan-out 
			core->gate_list[ff_index]->backward_list.size() + // inject two fault, one on this gate and one on it's fan-in 
			number_of_common_fanin_injection +
			number_of_common_fanout_injection ;
		cout << total_mbu_simulation_per_gate << " " << core->gate_list[ff_index]->forward_list.size() << " " << core->gate_list[ff_index]->backward_list.size() << " " << number_of_common_fanin_injection << " " << number_of_common_fanout_injection <<  endl ;
		for(int mbu_index=1; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			core->gate_list[ff_index]->sderating = 0;
			int total_iteration = 0;
			int max_event_size=0;
			int total_failures=0;
			do{
				for (int gate_index=0; gate_index<config->step_iteration; gate_index++){
					int target_gate = ff_index;
					if (core->gate_list[target_gate]->is_dff == 1){
						gate_index--;
						continue;
					}
					target_gate = core->gate_list[target_gate]->out;
					//set a random vulue to FFs and PIs;
					//Assign random values to the inputs
					for (int i=1; i<=core->numPi; i++){
						long rand_value = random();
						//Assigning random loginc to Primary Input
						short rand_logic = rand_value & 1;
						core->PI_list[i] = rand_logic;
						// Forwording the PI's value to its successors
						short node_adj_size = core->fadjlist[i].size();
						for(int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[i][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; 
								if (pred_gate_node_number==i)
									gateptr->input_values[k] = core->PI_list[i];
							}
						}
					}
					//Assign random values to DFFs
					for (int i=0; i<core->dff_num; i++){
						long rand_value = random();
						short rand_logic= rand_value & 0x1; 
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = rand_logic;
						pred_gateptr->input_values[0] = rand_logic; //may be not required
						int fadjnum = core->dfflist_ptr[i]->q;
						short node_adj_size =core->fadjlist[fadjnum].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadjnum][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadjnum)
									gateptr->input_values[k] = pred_gateptr->value;
							}
						}
					}
					//Logic Simulation----
					 //Propagating the input values through intermediate gates
					 for (int i=0; i<core->gate_numbers; i++){
						pred_gateptr = core->gate_list[i];
						short fadj_number = pred_gateptr->out;
						if (pred_gateptr->is_dff == 1) continue;
						core->compute_gate_value(pred_gateptr);
						short node_adj_size =core->fadjlist[fadj_number].size();
						for (int j=0; j<node_adj_size; j++){
							gateptr = core->fadjlist[fadj_number][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
           						pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadj_number)
								gateptr->input_values[k] = pred_gateptr->value;
							}	
						}
					}

					//Propagating the values through FFs
					for (int i=0; i<core->dff_num; i++){
						pred_gateptr = core->dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = pred_gateptr->input_values[0];
					}
					//Logic Simulation----

					// starting from sorted gate list and compute events for each gate in order
					for (int j=0; j<core->gate_numbers; j++){
						gateptr = core->gate_list[j];
						if (gateptr->is_dff == 1)
							continue;

						compute_gate_event_list(gateptr,core,config);

						if (gateptr->event_list.size()>max_event_size)
							max_event_size = gateptr->event_list.size();
						if (core->gate_list[j]->out==target_gate){
							int t1 = random() % config->tperiod; // circuit_period;
							//removing events >t1
							int list_size = gateptr->event_list.size();
							for (int k=list_size-1; k>=0; k--){
								if (gateptr->event_list[k]->time >= t1){
									eventptr = gateptr->event_list[k];
									gateptr->event_list.pop_back();
									tgateptr.event_list.push_front(eventptr);
								}
							}

							if( mbu_index==0 ){//this gate
								inject_fault( gateptr, t1, config->seu_pulse_width);
							}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() ){//forward
								inject_fault( gateptr, t1, config->seu_pulse_width);
								inject_fault( core->gate_list[ff_index]->forward_list[mbu_index-1], t1, config->seu_pulse_width);
							}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size()){//backward
								//target+fan-out
								inject_fault( gateptr, t1, config->seu_pulse_width);
								inject_fault( core->gate_list[ff_index]->backward_list[mbu_index-core->gate_list[ff_index]->forward_list.size()-1], t1, config->seu_pulse_width);
							}else if( mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size() + number_of_common_fanin_injection ){
								//common-fan-in	
								for(int i=0;i<core->gate_list[ff_index]->forward_list.size();i++){
									for(int j=i+1; j<core->gate_list[ff_index]->forward_list.size();j++){
										inject_fault( core->gate_list[ff_index]->forward_list.at(i), t1 , config->seu_pulse_width);
										inject_fault( core->gate_list[ff_index]->forward_list.at(j), t1 , config->seu_pulse_width);
									}
								}
							}else{
								//common-fanout
								for(int i=0;i<core->gate_list[ff_index]->backward_list.size();i++){
									for(int j=i+1; j<core->gate_list[ff_index]->backward_list.size();j++){
										inject_fault( core->gate_list[ff_index]->backward_list.at(i), t1 , config->seu_pulse_width);
										inject_fault( core->gate_list[ff_index]->backward_list.at(j), t1 , config->seu_pulse_width);
									}
								}
							}
	


							list_size = tgateptr.event_list.size();
							for (int k=0; k<list_size; k++){
								eventptr = tgateptr.event_list[0];
								tgateptr.event_list.pop_front();
								if (eventptr->time > (config->seu_pulse_width+t1))
									gateptr->event_list.push_back(eventptr);
								else
									delete eventptr;
							}
						}
					}

					bool failure_found = false;
					int event_time1,event_time2;
					for (int j=0; j<core->dff_num; j++){
						gateptr = core->fadjlist_ptr[core->dfflist_ptr[j]->q];

						compute_gate_event_list(gateptr,core,config);

						for (int k=0; k<gateptr->event_list.size()-1; k++){
							//if there is an event (ti,D) such that tperiod-(tsu+th)<ti <tperiod
							event_time1 = gateptr->event_list[k]->time;
							event_time2 = gateptr->event_list[k+1]->time;
							//if ((event_time >= (tperiod-tsu)) && (event_time <= (tperiod+th) ) )
							int tperiod = config->tperiod ;
							int tsu		= config->t_setup ;
							int th		= config->t_hold  ;
							if (   ((event_time1 <= (tperiod-tsu)) && (event_time2 >= (tperiod-tsu)))
								|| ((event_time1 <= (tperiod+th)) && (event_time2 >= (tperiod+th)))
								|| ((event_time1 <= (2*tperiod-tsu)) && (event_time2 >= (2*tperiod-tsu)))
								|| ((event_time1 <= (2*tperiod+th)) && (event_time2 >= (2*tperiod+th)))
								|| ((event_time1 <= (3*tperiod-tsu)) && (event_time2 >= (3*tperiod-tsu)))
								|| ((event_time1 <= (3*tperiod+th)) && (event_time2 >= (3*tperiod+th)))
								|| ((event_time1 <= (4*tperiod-tsu)) && (event_time2 >= (4*tperiod-tsu)))
								|| ((event_time1 <= (4*tperiod+th)) && (event_time2 >= (4*tperiod+th)))
								|| ((event_time1 <= (5*tperiod-tsu)) && (event_time2 >= (5*tperiod-tsu)))
								|| ((event_time1 <= (5*tperiod+th)) && (event_time2 >= (5*tperiod+th)))
								)
							{
								if ((gateptr->event_list[k]->pd !=0) || (gateptr->event_list[k]->pdn !=0)){
									total_failures++;
									failure_found = true;
									break;
								}
							}
						}
						if (failure_found == true)
							break;
					}
					if(failure_found == false){
							for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
							gateptr = core->fadjlist_ptr[i];
							if ((gateptr->pd == 1)||(gateptr->pdn == 1)){
								total_failures++;
								failure_found = true;
								break;
							}
						}
					}

					for (int i=0;i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						int list_size = gateptr->event_list.size();
						//fprintf(resultfp,"\n Gate %d:  type=%d",gateptr->out,gateptr->type);
						for (int j=0; j<list_size; j++){
							eventptr = gateptr->event_list[j];
						}
						for (int j=list_size-1; j>=0; j--){
							eventptr = gateptr->event_list[j];
							delete eventptr;
							eventptr = NULL;
							gateptr->event_list.pop_back();
						}
					}
				}//for fault injection
				total_iteration += config->step_iteration;
				total_sim_derating = (float)total_failures / (float)total_iteration;
				total_sim_variance = sqrt((total_sim_derating * (1 - total_sim_derating))/(float)total_iteration);
				if (total_sim_variance==0)
					total_sim_variance = sqrt(((1.0/(float)total_iteration) * (1 - (1.0/(float)total_iteration)))/(float)total_iteration);

			} while (((z_alpha_half*total_sim_variance) > error_tolerance) && (total_iteration<config->max_iteration) );
			core->gate_list[ff_index]->sderating = total_sim_derating;
			S_sim_vec[ff_index].push_back( ser(0,0,total_sim_derating) );
			total_derating_sum += total_sim_derating ;
			total_derating_iter++ ;
		}//mbu
		float sum=0;
		for(int i=0;i<total_mbu_simulation_per_gate-1;i++){
			sum += S_sim_vec[ff_index].at(i).derating ;
			}
		sum /= total_mbu_simulation_per_gate-1;
		core->gate_list[ff_index]->sderating = sum;
		cout << endl;
		
	}//for (ff_index=0;

	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Simulation Derating for all gates finished\n");

	float overall_sim_der = 0;
	for (int i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		//if (gateptr->is_dff==1)
		//	continue;
		//fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sim_der += gateptr->sderating;
		cout << i << " " <<  gateptr->sderating << endl ;
	}
	//overall_sim_der = overall_sim_der / (float)(core->gate_numbers-core->dff_num);	// ???
	overall_sim_der = overall_sim_der / (float)(core->gate_numbers);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall simulation Derating = %f \n", overall_sim_der);
	fprintf(resultfp,"Overall simulation derating per unit = %f \n", total_derating_sum/total_derating_iter);
}


void MBU::system_failure_analysis_SingleCycle_LET(configuration* config, Core* core, FILE* resultfp){
	long i,j,k;
	float total_derating_sum=0;
	float total_derating_iter=0;
	//short nff_index;
	short PI2POlist_size;
	struct gate *gateptr;
	int tsum,tk;
	float tpsum;
	struct event* eventptr;
	int list_size;
	int max_event_size=0;

	for (int ff_index=0; ff_index<core->gate_numbers; ff_index++){
		//nff_index = core->gate_list[ff_index]->out;
		//The comment can be removed!!
		if ((core->gate_list[ff_index]->is_dff == 1) || (core->gate_list[ff_index]->out <= (core->numPi+core->numPo))){
			core->gate_list[ff_index]->derating = 1;
			S_mlet_vec[ff_index].push_back( ser(0,0,1) );
			total_derating_sum += 1 ;
			total_derating_iter++ ;
			continue;
		}
		int number_of_common_fanout_injection= core->gate_list[ff_index]->forward_list.size() * (core->gate_list[ff_index]->forward_list.size()-1) /2 ;
		int number_of_common_fanin_injection  = core->gate_list[ff_index]->backward_list.size() * (core->gate_list[ff_index]->backward_list.size()-1) /2;

		int total_mbu_simulation_per_gate = 1 + // inject one fault on this gate/
			core->gate_list[ff_index]->forward_list.size() + // inject two fault, one on this gate and one on it's fan-out /
			core->gate_list[ff_index]->backward_list.size() + // inject two fault, one on this gate and one on it's fan-in /
			number_of_common_fanin_injection +
			number_of_common_fanout_injection ;
		for(int mbu_index=1; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			// To get PI2POlist starting from flip-flop nff_index excluding it
			find_TPsort_from_dffi(core->gate_list[ff_index]->out, core, config);
			PI2POlist_size = core->PI2POlist.size();
			// Set MAX values to ensure each will get correct value
			for (i=0; i<core->gate_numbers; i++){
				gateptr= core->gate_list[i];
				gateptr->p0 = 0; gateptr->p1 = 0;
				gateptr->pd = 0; gateptr->pdn = 0;
				gateptr->is_onpath = false;
			}
			for (i=0; i<PI2POlist_size; i++){
				gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
				gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;
				gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
				gateptr->is_onpath = true;
			}
			gateptr = core->gate_list[ff_index];

			if( mbu_index==0 ){//this gate
				inject_fault_analytical( gateptr, config->seu_pulse_width);
			}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() ){//forward
				//target + fan-in
				inject_fault_analytical( gateptr, config->seu_pulse_width);
				inject_fault_analytical( core->gate_list[ff_index]->forward_list[mbu_index-1], config->seu_pulse_width);
			}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size()){//backward
				//target+fan-out
				inject_fault_analytical( gateptr,  config->seu_pulse_width);
				inject_fault_analytical( core->gate_list[ff_index]->backward_list[mbu_index-core->gate_list[ff_index]->forward_list.size()-1], config->seu_pulse_width);
			}else if( mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size() + number_of_common_fanin_injection ){
				//common-fan-in	
				for(int i=0;i<core->gate_list[ff_index]->forward_list.size();i++){
					for(int j=i+1; j<core->gate_list[ff_index]->forward_list.size();j++){
						inject_fault_analytical( core->gate_list[ff_index]->forward_list.at(i) , config->seu_pulse_width);
						inject_fault_analytical( core->gate_list[ff_index]->forward_list.at(j) , config->seu_pulse_width);
					}
				}
			}else{
				//common-fanout
				for(int i=0;i<core->gate_list[ff_index]->backward_list.size();i++){
					for(int j=i+1; j<core->gate_list[ff_index]->backward_list.size();j++){
						inject_fault_analytical( core->gate_list[ff_index]->backward_list.at(i) , config->seu_pulse_width);
						inject_fault_analytical( core->gate_list[ff_index]->backward_list.at(j) , config->seu_pulse_width);
					}
				}
			}
			
			for (i=0; i<PI2POlist_size; i++){
				gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
				compute_D_gate_event_list(gateptr, core->gate_list[ff_index]->out,config,core);
				if (gateptr->event_list.size()>max_event_size)
					max_event_size = gateptr->event_list.size();
			}// End of PI2POlist


			int ff_index2;
			for (ff_index2=0; ff_index2<core->dff_num; ff_index2++){
				gateptr = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->q];
				compute_D_gate_event_list(gateptr, core->gate_list[ff_index]->out,config,core);
				if (gateptr->event_list.size()>max_event_size)
					max_event_size = gateptr->event_list.size();
			}
			short MAXS=3,NMAXS=-1;
			float sys_failure=1;
			int ff_ind;
			for (ff_ind=0; ff_ind<core->dff_num; ff_ind++){
				gateptr = core->fadjlist_ptr[core->dfflist_ptr[ff_ind]->q];
				list_size = gateptr->event_list.size();
				tpsum = 0;
				tsum = 0;
				for (k=0; k<gateptr->event_list.size()-1; k++){
					tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
					if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
						tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
					tsum += tk;
				}
				//if (tsum != 0) sys_failure *= 1- ((float)tpsum+(config->t_hold + config->t_setup)*(float)tpsum/(float)tsum)/(float)config->tperiod;
				if (tsum != 0) sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				//if (tsum != 0) sys_failure *= 1- ((float)tpsum-(config->t_hold + config->t_setup))/((float)config->tperiod - config->seu_pulse_width );
			}//for (ff_ind=0; ....
			//Need revise.
			for (int gate_index=0; gate_index<core->gate_numbers; gate_index++){
				gateptr = core->gate_list[gate_index];
				if (gateptr->po_no>=0) sys_failure *= 1 - (gateptr->pd + gateptr->pdn);
			}

			sys_failure = 1 - sys_failure;
			//ff_index is gate_list index
			gateptr = core->gate_list[ff_index]; 
			gateptr->derating = sys_failure;
			S_mlet_vec[ff_index].push_back( ser(0,0,sys_failure) );
			total_derating_sum += sys_failure ;
			total_derating_iter++ ;
			for (i=0;i<core->gate_numbers; i++){
				gateptr = core->gate_list[i];
				list_size = gateptr->event_list.size();

				for (j=list_size-1; j>=0; j--){
					eventptr = gateptr->event_list[j];
					delete eventptr;
					eventptr = NULL;
					gateptr->event_list.pop_back();
				}
			}
		}//mbu

		float sum = 0;
		for(int i=0;i<total_mbu_simulation_per_gate-1;i++){
			sum += S_mlet_vec[ff_index].at(i).derating ;
		}
		sum /= total_mbu_simulation_per_gate-1;
		core->gate_list[ff_index]->derating = sum;
	}//for

	float overall_sys_der = 0;
	for (i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		//if (gateptr->is_dff==1)
		//	continue;
		//fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sys_der += gateptr->derating;
	}
	overall_sys_der = overall_sys_der / (float)(core->gate_numbers);	// ???
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Systematic Derating = %f \n", overall_sys_der);
	fprintf(resultfp,"Overall Systematic derating per unit = %f \n", total_derating_sum/total_derating_iter);
	fprintf(resultfp, "Max event size in systematic derating=%d \n", max_event_size);
}

/**	Function: system_failure_simulation_SingleCycle_LogicalDerating
	 Purpose:  Computes system failure probability at first clock using simulation method (Applying large amount of vectors) 
*
void MBU::system_failure_simulation_SingleCycle_LogicalDerating(configuration* config, Core* core, FILE* resultfp){
	long i,j,ck;
	long total_it;
	long rand_value;
	short rand_logic;
	struct gate *gateptr;
	struct gate *pred_gateptr;
	struct gate* gptr1,*gptr2;
	float error_tolerance = 0.05;
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	long step_iteration = 100;
	float sim_epp, sim_ff_epp, sim_ff_po_epp, sim_var, sim_po_epp;
	float z_alpha_half = 2.576;

	if (core->PI_list==NULL) core->PI_list = new short[core->numPi+1];
	for (int ff_index=0; ff_index<core->sim_gate_numbers; ff_index++){
		if(config->verbose)cout << "Simulating gate# " << ff_index << " ... ";
		if (( core->gate_list[ff_index]->is_dff == 1) || (  core->gate_list[ff_index]->out <= ( core->numPi+ core->numPo))){
		//if ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){ //multicycle
			S_sim[ff_index] = 1; //Initialize the S_sim[ff_index]
			if (core->gate_list[ff_index]->is_dff == 1) S_sim_ff[ff_index] = 1;
			else S_sim_ff[ff_index] = 0;
			S_sim_ff_po[ff_index] = 1;
			S_sim_po[ff_index]=1;
			S_sim_var[ff_index] = 0;
			if(config->verbose)cout << endl;
			continue;
		}
		S_sim[ff_index] = 0; //Initialize the S_sim[ff_index]
		S_sim_ff[ff_index] = 0; 
		S_sim_ff_po[ff_index] = 0; 
		S_sim_po[ff_index]=0;
		S_sim_var[ff_index] = 0;
		total_it = 0;
		do{
			for (int it=0; it<step_iteration; it++){// begin iteration for ff_index flip-flop
				for (i=0; i<core->gate_numbers; i++){
					gateptr = core->gate_list[i];
					gateptr->p0 = 0; gateptr->p1 = 0;
					gateptr->pd = 0; gateptr->pdn = 0;
				}
				for (i=1; i<=core->numPi; i++){
					rand_value = random();
					//Assigning random loginc to Primary Input
					rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
					core->PI_list[i] = rand_logic;
				}
				for (i=0; i<core->dff_num; i++){
					rand_value = random();
					rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
					if ((core->dfflist_ptr[i]->q)==(core->gate_list[ff_index]->out)) continue;
					gateptr = core->dfflist_ptr[i]->dgateptr;
					gateptr->p1 = rand_logic;
					gateptr->p0 = 1 - rand_logic;
					gateptr->pd = 0; gateptr->pdn = 0;
				}
				//gateptr = dfflist_ptr[ff_index]->dgateptr;
				gateptr = core->gate_list[ff_index];
				gateptr->p1 = 0; gateptr->p0 = 0;
				gateptr->pd = 1; gateptr->pdn = 0;
				for (ck=1; ck<=config->clock; ck++){// begining of clock ck
					for (i=0; i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
							continue;
						//keep the ff_index still faulty only for the first clock cycle
						//if (gateptr->out == gate_list[ff_index]->out) // change it to && (ck==1)
						if ((i == ff_index) && (ck==1)) // change it to && (ck==1)
							continue;
						for (j=0; j< (gateptr->input_count); j++){// Input traversing
							// if input is Primary Input
							if ((gateptr->input_list[j]) <= core->numPi){
								p1_values[j] = core->PI_list[gateptr->input_list[j]];
								p0_values[j] = 1 - core->PI_list[gateptr->input_list[j]];
								pd_values[j] = 0; pdn_values[j] = 0;
								continue;
							}
							pred_gateptr = core->fadjlist_ptr[gateptr->input_list[j]];
							p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
							pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
						}// end of input traversing
						compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
					}// End of gate_list
					int ff_index2;
					bool is_failure = false ;
					bool is_seu		= false ;
					for (ff_index2=0; ff_index2<core->dff_num; ff_index2++){
						gptr1 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->d];
						gptr2 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->q];
						gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
						gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
						//uncomment when not using multicylce
						if ((gptr2->pd == 1)||(gptr2->pdn == 1)){
							is_seu =true;
							is_failure=true;
						}
					}
					for (i=core->numPi+1; i<=(core->numPi+core->numPo); i++){
						if (is_failure==true)
							break;
						gptr1 = core->fadjlist_ptr[i];
						if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
							is_failure = true;
							break;
						}
					}
					if(ck==1){
						if(is_seu==true){
							S_sim_ff[ff_index]++;
						}
						if((is_seu==true) || (is_failure==true)){
							S_sim_ff_po[ff_index]++;
						}
						if(is_failure==true){
							S_sim_po[ff_index]++;
						}
					}

					if (is_failure==true){
						S_sim[ff_index]++;
						break;
					}
					//S_sim[ff_index][ck]++;
				}// End of clock ck
				
			} //for
			total_it += step_iteration;
			sim_epp  = S_sim[ff_index] / (float)total_it;
			sim_ff_epp = S_sim_ff[ff_index] / (float)total_it;
			sim_ff_po_epp = S_sim_ff_po[ff_index] / (float)total_it;
			sim_po_epp = S_sim_po[ff_index] / (float)total_it;
			sim_var = sqrt((sim_epp * (1 - sim_epp))/(float)total_it);
			if (sim_var==0)
				sim_var = sqrt(((1.0/(float)total_it) * (1 - (1.0/(float)total_it)))/(float)total_it);
		} while ((total_it<config->max_iteration) && ((z_alpha_half*sim_var) > error_tolerance)); //end of iteration for flip-flop ff_index
		fprintf(resultfp, "Node %d: Sims iterated  = %d \n", core->gate_list[ff_index]->out, total_it);
		//for (i=1; i<=clk; i++)
		//S_sim[ff_index][i] = S_sim[ff_index][i] / (float)max_iteration;
		S_sim[ff_index] = sim_epp;
		S_sim_ff[ff_index] = sim_ff_epp;
		S_sim_ff_po[ff_index] = sim_ff_po_epp;
		S_sim_var[ff_index] = sim_var;
		S_sim_po[ff_index] = sim_po_epp;
		if(config->verbose) cout << "simulated for: " << total_it << endl;
	}
}


/**	Function: system_failure_simulation_MultiCycle_LogicalDerating
	 Purpose:  Computes system failure probability at first clock using simulation method (Applying large amount of vectors) 
*
void MBU::system_failure_simulation_MultiCycle_LogicalDerating(configuration* config, Core* core, FILE* resultfp){
	long i,j,ck;
	long total_it;
	long rand_value;
	short rand_logic;
	struct gate *gateptr;
	struct gate *pred_gateptr;
	struct gate* gptr1,*gptr2;
	float error_tolerance = 0.05;
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	long step_iteration = 100;
	float sim_epp, sim_ff_epp, sim_ff_po_epp, sim_var, sim_po_epp;
	float z_alpha_half = 2.576;

	if (core->PI_list==NULL) core->PI_list = new short[core->numPi+1];
	for (int ff_index=0; ff_index<core->sim_gate_numbers; ff_index++){
		if(config->verbose)cout << "Simulating gate# " << ff_index << " ... ";
		//if ((gate_list[ff_index]->is_dff == 1) || ( gate_list[ff_index]->out <= (numPi+numPo)))
		if ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){ //multicycle
			S_sim[ff_index] = 1; //Initialize the S_sim[ff_index]
			if (core->gate_list[ff_index]->is_dff == 1) S_sim_ff[ff_index] = 1;
			else S_sim_ff[ff_index] = 0;
			S_sim_ff_po[ff_index] = 1;
			S_sim_po[ff_index]=1;
			S_sim_var[ff_index] = 0;
			if(config->verbose)cout << endl;
			continue;
		}
		S_sim[ff_index] = 0; //Initialize the S_sim[ff_index]
		S_sim_ff[ff_index] = 0; 
		S_sim_ff_po[ff_index] = 0; 
		S_sim_po[ff_index]=0;
		S_sim_var[ff_index] = 0;
		total_it = 0;
		do{
			for (int it=0; it<step_iteration; it++){// begin iteration for ff_index flip-flop
				for (i=0; i<core->gate_numbers; i++){
					gateptr = core->gate_list[i];
					gateptr->p0 = 0; gateptr->p1 = 0;
					gateptr->pd = 0; gateptr->pdn = 0;
				}
				for (i=1; i<=core->numPi; i++){
					rand_value = random();
					//Assigning random loginc to Primary Input
					rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
					core->PI_list[i] = rand_logic;
				}
				for (i=0; i<core->dff_num; i++){
					rand_value = random();
					rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
					if ((core->dfflist_ptr[i]->q)==(core->gate_list[ff_index]->out)) continue;
					gateptr = core->dfflist_ptr[i]->dgateptr;
					gateptr->p1 = rand_logic;
					gateptr->p0 = 1 - rand_logic;
					gateptr->pd = 0; gateptr->pdn = 0;
				}
				//gateptr = dfflist_ptr[ff_index]->dgateptr;
				gateptr = core->gate_list[ff_index];
				gateptr->p1 = 0; gateptr->p0 = 0;
				gateptr->pd = 1; gateptr->pdn = 0;
				for (ck=1; ck<=config->clock; ck++){// begining of clock ck
					for (i=0; i<core->gate_numbers; i++){
						gateptr = core->gate_list[i];
						if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
							continue;
						//keep the ff_index still faulty only for the first clock cycle
						//if (gateptr->out == gate_list[ff_index]->out) // change it to && (ck==1)
						if ((i == ff_index) && (ck==1)) // change it to && (ck==1)
							continue;
						for (j=0; j< (gateptr->input_count); j++){// Input traversing
							// if input is Primary Input
							if ((gateptr->input_list[j]) <= core->numPi){
								p1_values[j] = core->PI_list[gateptr->input_list[j]];
								p0_values[j] = 1 - core->PI_list[gateptr->input_list[j]];
								pd_values[j] = 0; pdn_values[j] = 0;
								continue;
							}
							pred_gateptr = core->fadjlist_ptr[gateptr->input_list[j]];
							p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
							pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
						}// end of input traversing
						compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
					}// End of gate_list
					int ff_index2;
					bool is_failure = false ;
					bool is_seu		= false ;
					for (ff_index2=0; ff_index2<core->dff_num; ff_index2++){
						gptr1 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->d];
						gptr2 = core->fadjlist_ptr[core->dfflist_ptr[ff_index2]->q];
						gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
						gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
						//uncomment when not using multicylce
						if ((gptr2->pd == 1)||(gptr2->pdn == 1))
							is_seu =true;
					}
					for (i=core->numPi+1; i<=(core->numPi+core->numPo); i++){
						if (is_failure==true)
							break;
						gptr1 = core->fadjlist_ptr[i];
						if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
							is_failure = true;
							break;
						}
					}
					if(ck==1){
						if(is_seu==true){
							S_sim_ff[ff_index]++;
						}
						if((is_seu==true) || (is_failure==true)){
							S_sim_ff_po[ff_index]++;
						}
						if(is_failure==true){
							S_sim_po[ff_index]++;
						}
					}

					if (is_failure==true){
						S_sim[ff_index]++;
						break;
					}
					//S_sim[ff_index][ck]++;
				}// End of clock ck
				
			} //for
			total_it += step_iteration;
			sim_epp  = S_sim[ff_index] / (float)total_it;
			sim_ff_epp = S_sim_ff[ff_index] / (float)total_it;
			sim_ff_po_epp = S_sim_ff_po[ff_index] / (float)total_it;
			sim_po_epp = S_sim_po[ff_index] / (float)total_it;
			sim_var = sqrt((sim_epp * (1 - sim_epp))/(float)total_it);
			if (sim_var==0)
				sim_var = sqrt(((1.0/(float)total_it) * (1 - (1.0/(float)total_it)))/(float)total_it);
		} while ((total_it<config->max_iteration) && ((z_alpha_half*sim_var) > error_tolerance)); //end of iteration for flip-flop ff_index
		fprintf(resultfp, "Node %d: Sims iterated  = %d \n", core->gate_list[ff_index]->out, total_it);
		//for (i=1; i<=clk; i++)
		//S_sim[ff_index][i] = S_sim[ff_index][i] / (float)max_iteration;
		S_sim[ff_index] = sim_epp;
		S_sim_ff[ff_index] = sim_ff_epp;
		S_sim_ff_po[ff_index] = sim_ff_po_epp;
		S_sim_var[ff_index] = sim_var;
		S_sim_po[ff_index] = sim_po_epp;
		if(config->verbose) cout << "simulated for: " << total_it << endl;
	}

}
*/

void MBU::report(Core* core, FILE* resultfp){
	// reporting results:
	int sim_gate_numbers = core->sim_gate_numbers ;
	float sim_epp_total=0, sim_epp_ff=0, sim_epp_ff_po=0, sim_epp_po=0;
	for (int i=0; i<sim_gate_numbers; i++){
		sim_epp_total += S_sim[i];
		sim_epp_ff += S_sim_ff[i] ;
		sim_epp_ff_po += S_sim_ff_po[i] ;
		sim_epp_po += S_sim_po[i];
		printf("gate sim epp: gate[%d]=%f\n",i,S_sim[i]);
	}

	sim_epp_total = sim_epp_total/sim_gate_numbers;    
	sim_epp_ff = sim_epp_ff / sim_gate_numbers;   
	sim_epp_ff_po = sim_epp_ff_po /sim_gate_numbers;  
	sim_epp_po = sim_epp_po/sim_gate_numbers;
	printf("average sim epp: %f \n", sim_epp_total);
	printf("average sim epp (ff): %f \n", sim_epp_ff);	
	printf("average sim epp (ff-po): %f \n", sim_epp_ff_po);
	printf("average sim epp (po): %f \n", sim_epp_po);
}


/////////////////////////////////////////////////////////////////////////////////////
// Function: compute_D_Probability
// Purpose:  Computes D Probability of the given gate from inputs
// Returns none
void MBU::compute_D_Probability(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn){
	short i;
	float fval1, fval2;
	switch (gateptr->type) {
	case 0:     //nand
		gateptr->p1= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->p1 *= p1[i];

		gateptr->pd= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->pd *= (p1[i]+pd[i]);
		gateptr->pd -= gateptr->p1;

		gateptr->pdn= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->pdn *= (p1[i]+pdn[i]);
		gateptr->pdn -= gateptr->p1;

		gateptr->p0 = 1- (gateptr->p1 + gateptr->pd + gateptr->pdn);
		// Now reverse values
		fval1 = gateptr->p0; gateptr->p0 = gateptr->p1; gateptr->p1 = fval1;
		fval2 = gateptr->pd; gateptr->pd = gateptr->pdn; gateptr->pdn = fval2;
		break;
	case 1:     //and
		gateptr->p1= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->p1 *= p1[i];

		gateptr->pd= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->pd *= (p1[i]+pd[i]);
		gateptr->pd -= gateptr->p1;

		gateptr->pdn= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->pdn *= (p1[i]+pdn[i]);
		gateptr->pdn -= gateptr->p1;

		gateptr->p0 = 1- (gateptr->p1 + gateptr->pd + gateptr->pdn);
		break;
	case 2:     //nor
		gateptr->p0= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->p0 *= p0[i];

		gateptr->pd= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->pd *= (p0[i]+pd[i]);
		gateptr->pd -= gateptr->p0;

		gateptr->pdn= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->pdn *= (p0[i]+pdn[i]);
		gateptr->pdn -= gateptr->p0;

		gateptr->p1 = 1- (gateptr->p0 + gateptr->pd + gateptr->pdn);
		// Now reverse values
		fval1 = gateptr->p0; gateptr->p0 = gateptr->p1; gateptr->p1 = fval1;
		fval2 = gateptr->pd; gateptr->pd = gateptr->pdn; gateptr->pdn = fval2;
		break;
	case 3:     //or
		gateptr->p0= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->p0 *= p0[i];

		gateptr->pd= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->pd *= (p0[i]+pd[i]);
		gateptr->pd -= gateptr->p0;

		gateptr->pdn= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->pdn *= (p0[i]+pdn[i]);
		gateptr->pdn -= gateptr->p0;

		gateptr->p1 = 1- (gateptr->p0 + gateptr->pd + gateptr->pdn);
		break;
	case 4:     //xor (2-input xor)
		gateptr->p0 = p0[0]*p0[1] + p1[0]*p1[1] + pd[0]*pd[1] + pdn[0]*pdn[1];
		gateptr->p1 = p0[0]*p1[1] + p1[0]*p0[1] + pd[0]*pdn[1] + pdn[0]*pd[1];
		gateptr->pd = pd[0]*p0[1] + p0[0]*pd[1] + p1[0]*pdn[1] + pdn[0]*p1[1];
		gateptr->pdn = 1- (gateptr->p0 + gateptr->p1 + gateptr->pd);
		break;
	case 5:     //xnor (2-input xnor)
		gateptr->p1 = p0[0]*p0[1] + p1[0]*p1[1] + pd[0]*pd[1] + pdn[0]*pdn[1];
		gateptr->p0 = p0[0]*p1[1] + p1[0]*p0[1] + pd[0]*pdn[1] + pdn[0]*pd[1];
		gateptr->pdn = pd[0]*p0[1] + p0[0]*pd[1] + p1[0]*pdn[1] + pdn[0]*p1[1];
		gateptr->pd = 1- (gateptr->p0 + gateptr->p1 + gateptr->pdn);
		break;
	case 6:     //not
		gateptr->p0 = p1[0];
		gateptr->p1 = p0[0];
		gateptr->pd = pdn[0];
		gateptr->pdn = pd[0];
		break;

	case 8:		//lut
		gateptr->p0 = 0;
		gateptr->p1 = 0;
		gateptr->pd = 0;
		gateptr->pdn = 0;
		for(int i=0;i<pow(4.0,1.0*gateptr->input_count);i++){
			char x[10] ;
			itoa(i,x,4);
			int size= strlen(x);
			if(size<gateptr->input_count){
				for(int j=0;j<gateptr->input_count-size;j++){
					char str[10];
					strcpy(str,"0");
					strcat(str,x);
					strcpy(x,str);
				}
			}
			int target=0, neighbor=0;
			for(int j=0;j<gateptr->input_count;j++){
				if( (x[j]=='1') || (x[j]=='3')/*1^e*/ )
					target += 1 << (gateptr->input_count-j-1) ;
				if( (x[j]=='1') || (x[j]=='2')/*0^e*/ )
					neighbor += 1 << (gateptr->input_count-j-1) ;
			}
			float p = 1.0 ;
			for(int k=0;k<gateptr->input_count;k++){
				switch(x[k]){
				case '0':	p *= p0[k] ;	break ;	//0
				case '1':	p *= p1[k] ;	break ;	//1
				case '2':	p *= pd[k] ;	break ;	//0^e
				case '3':	p *= pdn[k] ;	break ;	//1^e
				default: cout << "wtf?" << endl ; exit(1);
				}
			}
			// permanent fault
			if( gateptr->fault[target] == 0 ){
				if( gateptr->lut[target] == gateptr->lut[neighbor] ){ // Error Masked
					if( gateptr->lut[target]==0 ){	//0 (masked or correct value)
						gateptr->p0 += p;
					}else{	//1 (masked or correct value)
						gateptr->p1 += p;
					}
				}else{ // Error will propagate
					if(gateptr->lut[target]==0){	//0^e
						gateptr->pd += p ;
					}else{	//1^e
						gateptr->pdn += p ;
					}
				}
			}else{
				if( gateptr->lut[target] == gateptr->lut[neighbor] ){ // Permanent Error Propagation
					if( gateptr->lut[target]==0 ){	//0 (masked or correct value)
						gateptr->pdn += p;
					}else{	//1 (masked or correct value)
						gateptr->pd += p;
					}
				}else{ // Error will propagate
					if(gateptr->lut[target]==0){	//0^e
						gateptr->pdn += p ;
					}else{	//1^e
						gateptr->pd += p ;
					}
				}
			}
			/* transient fault:
			if( gateptr->lut[target] == gateptr->lut[neighbor] ){ // Error Masked
				if( gateptr->lut[target]==0 ){	//0 (masked or correct value)
					gateptr->p0 += p;
				}else{	//1 (masked or correct value)
					gateptr->p1 += p;
				}
			}else{ // Error will propagate
				if(gateptr->lut[target]==0){	//0^e
					gateptr->pd += p ;
				}else{	//1^e
					gateptr->pdn += p ;
				}
			}
			*/
		}
	default:    // buff (or dff)
		gateptr->p0 = p0[0];
		gateptr->p1 = p1[0];
		gateptr->pd = pd[0];
		gateptr->pdn = pdn[0];
		break;
	}

}//compute_D_Probability

void MBU::dfs_fvisit (vector <deque<struct gate*> > &fadjlist, short u, Core* core){
    core->color[u] = 'g';
    for(int i=0; i<fadjlist[u].size(); i++){
        struct gate *p = fadjlist[u][i];
        short v = p->out;
        if(core->color[v] == 'w'){
			pi[v] = u;
            dfs_fvisit(core->fadjlist, v, core);    // Recursive call
        }
    }
    core->color[u] = 'b';
    dfs_fnum[u] = ++final_time_stamp;
}

//This version of tp-sort supports METs
void MBU::topological_sorting(Core* core,configuration* config, short source1, short source2){
	//cout << "doing tp-sort for gate:" << source1 << " " << source2 << endl ;
	deque <short> tmp_constant;
	struct gate *p;

	core->PI2POlist.clear();
	core->color.clear();
	for(int i=1; i<=core->nodes; i++){
		core->color.push_back('w');
		pi[i] = config->MAX;
	}
	for (int i=0; i<core->dff_num; i++)
		core->color[core->dfflist_ptr[i]->q]='g';

	// 2nd Apply DFS starting from node nff_index
	pi[source1] = 0;
	final_time_stamp = 0;
	dfs_fvisit(core->fadjlist, source1, core);
	if(source2!=-1)		
		if(core->color[source2]!='g') 
			dfs_fvisit(core->fadjlist, source2, core);
	
	for(int i=1; i<core->nodes; i++){
		if (pi[i] != config->MAX){
			tmp_constant.clear();
			tmp_constant.push_back(i);
			core->PI2POlist.push_back(tmp_constant);
		}
	}

	//sort the PI2POlist
	short PI2POlist_size = core->PI2POlist.size();
	for (int i=0; i < (PI2POlist_size-1); i++){
		for (int j=i+1; j<PI2POlist_size; j++){
			if (dfs_fnum[core->PI2POlist[i][0]] < dfs_fnum[core->PI2POlist[j][0]]){
				short val = core->PI2POlist[i][0];
				core->PI2POlist[i][0] = core->PI2POlist[j][0];
				core->PI2POlist[j][0] = val;
			}
		}
	}

	//Here we creating forward-adjancy list for on-path gates
	for (int i=0; i<core->PI2POlist.size(); i++){
		for (int j=0; j<core->fadjlist[core->PI2POlist[i][0]].size(); j++){
			p = core->fadjlist[core->PI2POlist[i][0]][j];
			if (pi[p->out] != config->MAX){
				core->PI2POlist[i].push_back(p->out);
			}
		}
	}
}

/*
 void MBU::find_TPsort_from_dffi(short nff_index, Core* core,configuration* config){
 int i,j;
 deque <short> tmp_constant;
 struct gate *p;
 
 core->PI2POlist.clear();
 core->color.clear();
 for(i=1; i<=core->nodes; i++){
 core->color.push_back('w');
 pi[i] = config->MAX;
 }
 for (i=0; i<core->dff_num; i++)
 core->color[core->dfflist_ptr[i]->q]='g';
 
 // 2nd Apply DFS starting from node nff_index
 pi[nff_index] = 0;
 final_time_stamp = 0;
 dfs_fvisit(core->fadjlist, nff_index, core);
 
 for(i=1; i<core->nodes; i++){
 if (pi[i] != config->MAX)
 {
 tmp_constant.clear();
 tmp_constant.push_back(i);
 core->PI2POlist.push_back(tmp_constant);
 }
 }
 
 //sort the PI2POlist
 short PI2POlist_size = core->PI2POlist.size();
 for (i=0; i < (PI2POlist_size-1); i++){
 for (j=i+1; j<PI2POlist_size; j++){
 if (dfs_fnum[core->PI2POlist[i][0]] < dfs_fnum[core->PI2POlist[j][0]]){
 short val = core->PI2POlist[i][0];
 core->PI2POlist[i][0] = core->PI2POlist[j][0];
 core->PI2POlist[j][0] = val;
 }
 }
 }
 
 //Here we creating forward-adjancy list for on-path gates
 for (i=0; i<core->PI2POlist.size(); i++){
 for (j=0; j<core->fadjlist[core->PI2POlist[i][0]].size(); j++){
 p = core->fadjlist[core->PI2POlist[i][0]][j];
 if (pi[p->out] != config->MAX){
 core->PI2POlist[i].push_back(p->out);
 }
 }
 }
 }
 
 */

void MBU::compute_gate_event_list(struct gate* gateptr, int targe_gate_node, int target_gate_node2, configuration* config, Core* core){
	int* input_list_index = new int[config->MAX];
	int input_count = gateptr->input_count;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
    float vomin_values[MAX_GATE_INPUTS], vomax_values[MAX_GATE_INPUTS];
	struct gate* pred_gateptr;
	int event_no=0;
	struct event *eventptr, *prev_eventptr;
	int i,pred_gate_number;
	int now_event_time=0, next_event_time=0, prev_event_time=0;
	int max_event_time = 3*config->tperiod + config->seu_pulse_width;
	int next_event;
	float vimin, vimax;
	float xmin, xmax, xused;
	float tran_delay1, tran_delay2;
	bool first_event = true;
	
	if ((gateptr->out==targe_gate_node) || (gateptr->out==target_gate_node2)){ //skip the faulty gate
		return;
	}

	for (i=0; i<input_count; i++){
		input_list_index[i] = 0;
	}

	now_event_time = 0;//gateptr->gd;//gateptr->delay;
	do{
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=core->numPi){
                p0_values[i] = 1- core->PI_list_sp[gateptr->input_list[i]];
                p1_values[i] = core->PI_list_sp[gateptr->input_list[i]];
                pd_values[i] = 0; pdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
				continue;
			}

			pred_gateptr = core->fadjlist_ptr[gateptr->input_list[i]];
            pred_gate_number = pred_gateptr->out;
			bool use_sp = false;
			if (pred_gateptr->is_dff == 1){
				use_sp = true;
			}

			if (pred_gateptr->is_onpath==false)
				use_sp = true;

			if (pred_gateptr->is_onpath==true){
				if (pred_gate_number<=(core->numPi+core->numPo))
					use_sp = true;

				//We ignore the following when gateptr is DFF
				if (gateptr->is_dff == false){
					//IMPORTANT: The following lines can be commented or not
					//it depends whether the propagated Ds are overlapped or not
					/*for (k=0; k<fadjlist[pred_gate_number].size(); k++)
						if (fadjlist[pred_gate_number][k]->type==7) //is DFF
							use_sp = true;
					*/
				}
			}
			if (use_sp==true){
				p0_values[i] = 1- pred_gateptr->signal_probability;
                p1_values[i] = pred_gateptr->signal_probability;
                pd_values[i] = 0;
                pdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
            }
			else //if (pred_gateptr->is_onpath==true)
			{
				pd_values[i] = pred_gateptr->event_list[input_list_index[i]]->pd;
				pdn_values[i] = pred_gateptr->event_list[input_list_index[i]]->pdn;
				p1_values[i] = pred_gateptr->event_list[input_list_index[i]]->p1;
				p0_values[i] = pred_gateptr->event_list[input_list_index[i]]->p0;
				vomin_values[i] = pred_gateptr->event_list[input_list_index[i]]->vomin;
				vomax_values[i] = pred_gateptr->event_list[input_list_index[i]]->vomax;
			}
		}
		compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
		eventptr = new struct event;
		//now_event_time += (int)gateptr->gd;
		eventptr->time = now_event_time + (int)gateptr->gd;
		eventptr->p0 = gateptr->p0; eventptr->p1  = gateptr->p1;
		eventptr->pd = gateptr->pd; eventptr->pdn = gateptr->pdn;
		if (first_event)
			prev_eventptr = eventptr;
		vimin = compute_vomin(vomin_values,input_count);
		vimax = compute_vomax(vomax_values,input_count);
		eventptr->pwi = now_event_time - prev_event_time;
		tran_delay2 = (gateptr->p0==1) ? gateptr->tphl_load : gateptr->tplh_load;
		eventptr->vomin = (eventptr->pwi<gateptr->tphl) ? (1 - eventptr->pwi/(gateptr->tphl*1.25)) : VGD;
		eventptr->vomax = (eventptr->pwi<gateptr->tplh) ? (1 - eventptr->pwi/(gateptr->tplh*1.25)) : VDD;
		xmin = (VDD/2.0 - eventptr->vomin)/(VDD/2.0); //output L2H
		xmax = (eventptr->vomax - VDD/2.0)/(VDD/2.0); //output H2L
		xused = (gateptr->p1==1) ? xmin : xmax;
		eventptr->pwo = (int)(eventptr->pwi - tran_delay1 + xused*tran_delay2);
		if ((eventptr->p0==prev_eventptr->p0)&&(eventptr->p1==prev_eventptr->p1))
			eventptr->pwo = eventptr->pwi; // no change in the output
		eventptr->time += eventptr->pwo - eventptr->pwi;
		gateptr->event_list.push_back(eventptr);
		prev_eventptr = eventptr;
		tran_delay1 = tran_delay2;
		first_event = false;
		//start to find the next event
		next_event_time = max_event_time;
		next_event = MAX_GATE_INPUTS + 1;
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=core->numPi)
				continue;
			pred_gateptr = core->fadjlist_ptr[gateptr->input_list[i]];
			if (pred_gateptr->is_dff == 1)
				continue;
			if (pred_gateptr->event_list.size() > (input_list_index[i]+1)) //there are more events
			{
				if (pred_gateptr->event_list[input_list_index[i]+1]->time < next_event_time){
					next_event_time = pred_gateptr->event_list[input_list_index[i]+1]->time;
					next_event = i;
				}
			}
		}
		//if (next_event_time == max_event_time) // we do not have any event left
			//break;
		input_list_index[next_event]++;
		prev_event_time = now_event_time;
		now_event_time = next_event_time;// + (int)gateptr->gd;
	} while (next_event_time != max_event_time); // we do not have any event left
	delete input_list_index;
}

float MBU::compute_vomin(float* vomin_values, int input_count){
	int i;
	float vomin = VGD;
    for (i=0; i<input_count; i++)
	{
		if (vomin_values[i] > vomin)
			vomin = vomin_values[i];
	}
	return vomin;
}

float MBU::compute_vomax(float* vomax_values, int input_count){
	int i;
	float vomax = VDD;
    for (i=0; i<input_count; i++){
		if (vomax_values[i] < vomax)
			vomax = vomax_values[i];
	}
	return vomax;
}


void MBU::compute_gate_event_list(struct gate* gateptr, Core* core, configuration* config){
	int* input_list_index = new int[config->MAX];
	int input_count = gateptr->input_count;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
    float vomin_values[MAX_GATE_INPUTS], vomax_values[MAX_GATE_INPUTS];
	struct gate* pred_gateptr;
	int event_no=0;
	struct event *eventptr, *prev_eventptr;
	int i;
	int now_event_time,next_event_time,prev_event_time;
	//int max_event_time = tperiod + seu_pulse_width;
	int max_event_time = 3*config->tperiod + config->seu_pulse_width;
	int next_event;
	float vimin, vimax;
	float xmin, xmax, xused;
	float tran_delay1, tran_delay2;
	bool level_one = true;
	bool first_event = true;
	for (i=0; i<input_count; i++){
		input_list_index[i] = 0;
	}
	now_event_time = next_event_time = 0;
	prev_event_time = -100;
	do{
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=core->numPi){
				p0_values[i] = 1 - gateptr->input_values[i];
				p1_values[i] = gateptr->input_values[i];
				pd_values[i] = 0; pdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
				continue;
			}
			pred_gateptr = core->fadjlist_ptr[gateptr->input_list[i]];
			if (pred_gateptr->is_dff == 1){
				p0_values[i] = 1 - gateptr->input_values[i];
				p1_values[i] = gateptr->input_values[i];
				pd_values[i] = 0; pdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
				continue;
			}
			p0_values[i] = pred_gateptr->event_list[input_list_index[i]]->p0;
			p1_values[i] = pred_gateptr->event_list[input_list_index[i]]->p1;
			pd_values[i] = pred_gateptr->event_list[input_list_index[i]]->pd;
			pdn_values[i] = pred_gateptr->event_list[input_list_index[i]]->pdn;
			vomin_values[i] = pred_gateptr->event_list[input_list_index[i]]->vomin;
			vomax_values[i] = pred_gateptr->event_list[input_list_index[i]]->vomax;
			level_one = false;
		}
		if (level_one == true){
			eventptr = new struct event;
			eventptr->time = now_event_time;
			eventptr->p0 = 1; eventptr->p1  = 0;
			eventptr->pd = 0; eventptr->pdn = 0;
			eventptr->vomin = VGD; eventptr->vomax = VDD;
			gateptr->event_list.push_back(eventptr);
			prev_eventptr = eventptr;
			tran_delay1 = gateptr->tphl_load;
			first_event = false;
			//now_event_time = (int)gateptr->gd;
		}
		compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
		eventptr = new struct event;
		//now_event_time += (int)gateptr->gd;
		eventptr->time = now_event_time + (int)gateptr->gd;
		eventptr->p0 = gateptr->p0; eventptr->p1  = gateptr->p1;
		eventptr->pd = gateptr->pd; eventptr->pdn = gateptr->pdn;
		if (first_event)
			prev_eventptr = eventptr;
		vimin = compute_vomin(vomin_values,input_count);
		vimax = compute_vomax(vomax_values,input_count);
		eventptr->pwi = now_event_time - prev_event_time;
		tran_delay2 = (gateptr->p0==1) ? gateptr->tphl_load : gateptr->tplh_load;
		eventptr->vomin = (eventptr->pwi<gateptr->tphl) ? (1 - eventptr->pwi/(gateptr->tphl*1.25)) : VGD;
		eventptr->vomax = (eventptr->pwi<gateptr->tplh) ? (1 - eventptr->pwi/(gateptr->tplh*1.25)) : VDD;
		xmin = (VDD/2.0 - eventptr->vomin)/(VDD/2.0); //output L2H
		xmax = (eventptr->vomax - VDD/2.0)/(VDD/2.0); //output H2L
		xused = (gateptr->p1==1) ? xmin : xmax;
		eventptr->pwo = (int)(eventptr->pwi - tran_delay1 + xused*tran_delay2);
		if ((eventptr->p0==prev_eventptr->p0)&&(eventptr->p1==prev_eventptr->p1))
			eventptr->pwo = eventptr->pwi; // no change in the output
		eventptr->time += eventptr->pwo - eventptr->pwi;
		gateptr->event_list.push_back(eventptr);
		prev_eventptr = eventptr;
		tran_delay1 = tran_delay2;
		first_event = false;
		//start to find the next event
		next_event_time = max_event_time;
		next_event = MAX_GATE_INPUTS + 1;
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=core->numPi)
				continue;
			pred_gateptr = core->fadjlist_ptr[gateptr->input_list[i]];
			if (pred_gateptr->is_dff == 1)
				continue;
			if (pred_gateptr->event_list.size() > (input_list_index[i]+1)) //there are more events
			{
				if (pred_gateptr->event_list[input_list_index[i]+1]->time < next_event_time)
				{
					next_event_time = pred_gateptr->event_list[input_list_index[i]+1]->time;
					next_event = i;
				}
			}
		}

		input_list_index[next_event]++;
		prev_event_time = now_event_time;
		now_event_time = next_event_time; // + (int)gateptr->gd;
	} while (next_event_time != max_event_time); // we do not have any event left
	delete input_list_index;
}
//I used this function to generate a schatter graph describing the affected POs in SET vs MET
bool MBU::fault_simulation_set_logic(Core* core, configuration* config, FILE* resultfp, int session, int ff_index, char c){
	long i,j,ck;
	bool affected = false;
	long total_it;
	long rand_value;
	short rand_logic;
	struct gate *gateptr;
	struct gate *pred_gateptr;
	struct gate* gptr1,*gptr2;
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];

	if (core->PI_list==NULL) core->PI_list = new short[core->numPi+1];
//	cout << "Simulating gate# " << ff_index << " ... ";
	if ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){ //multicycle
		int gate_po_num =  core->gate_list[ff_index]->out - core->numPi;
		cout << c << " " << session << "," << gate_po_num << endl ;
		affected = true ;
		//continue;
	}
	for (i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		gateptr->p0 = 0; gateptr->p1 = 0;
		gateptr->pd = 0; gateptr->pdn = 0;
	}
	for (i=1; i<=core->numPi; i++){
		rand_value = rand();
		rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
		core->PI_list[i] = rand_logic;
	}
	for (i=0; i<core->dff_num; i++){
		rand_value = rand();
		rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
		if ((core->dfflist_ptr[i]->q)==(core->gate_list[ff_index]->out)) continue;
		gateptr = core->dfflist_ptr[i]->dgateptr;
		gateptr->p1 = rand_logic;
		gateptr->p0 = 1 - rand_logic;
		gateptr->pd = 0; gateptr->pdn = 0;
	}

	gateptr = core->gate_list[ff_index];
	gateptr->p1 = 0; gateptr->p0 = 0;
	gateptr->pd = 1; gateptr->pdn = 0;
	for (i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		for (j=0; j< (gateptr->input_count); j++){// Input traversing
			if ((gateptr->input_list[j]) <= core->numPi){
				p1_values[j] = core->PI_list[gateptr->input_list[j]];
				p0_values[j] = 1 - core->PI_list[gateptr->input_list[j]];
				pd_values[j] = 0; pdn_values[j] = 0;
				continue;
			}
		pred_gateptr = core->fadjlist_ptr[gateptr->input_list[j]];
		p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
		pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
		}// end of input traversing
		compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
	}// End of gate_list


	for (i=core->numPi+1; i<=(core->numPi+core->numPo); i++){
		gptr1 = core->fadjlist_ptr[i];
		if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
			int gate_po_num =  i - core->numPi;
			affected = true ;
			cout << c << " " << session << "," << gate_po_num << endl ;
		}
	}
	return affected;
}


bool MBU::fault_simulation_met_logic(Core* core, configuration* config, FILE* resultfp, int session, int ff_index, int ff_index2, char c){
	long i,j,ck;
	bool affected = false;
	long total_it;
	long rand_value;
	short rand_logic;
	struct gate *gateptr;
	struct gate *pred_gateptr;
	struct gate* gptr1,*gptr2;
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];

	if (core->PI_list==NULL) core->PI_list = new short[core->numPi+1];
	//cout << "Simulating gate# " << ff_index << "--" << ff_index2 << " ... ";
	if ( core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){
		int gate_po_num =  core->gate_list[ff_index]->out - core->numPi;
		cout << c << " " << session << "," << gate_po_num << endl ;
		affected = true ;
	}
	if ( core->gate_list[ff_index2]->out <= (core->numPi+core->numPo)){
		int gate_po_num =  core->gate_list[ff_index2]->out - core->numPi;
		cout << c << " " << session << "," << gate_po_num << endl ;
		affected = true ;
	}
	for (i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		gateptr->p0 = 0; gateptr->p1 = 0;
		gateptr->pd = 0; gateptr->pdn = 0;
	}
	for (i=1; i<=core->numPi; i++){
		rand_value = rand();
		rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
		core->PI_list[i] = rand_logic;
	}
	for (i=0; i<core->dff_num; i++){
		rand_value = rand();
		rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
		if ((core->dfflist_ptr[i]->q)==(core->gate_list[ff_index]->out)) continue;
		gateptr = core->dfflist_ptr[i]->dgateptr;
		gateptr->p1 = rand_logic;
		gateptr->p0 = 1 - rand_logic;
		gateptr->pd = 0; gateptr->pdn = 0;
	}

	gateptr = core->gate_list[ff_index];
	gateptr->p1 = 0; gateptr->p0 = 0;
	gateptr->pd = 1; gateptr->pdn = 0;

	gateptr = core->gate_list[ff_index2];
	gateptr->p1 = 0; gateptr->p0 = 0;
	gateptr->pd = 1; gateptr->pdn = 0;

	for (i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		for (j=0; j< (gateptr->input_count); j++){// Input traversing
			if ((gateptr->input_list[j]) <= core->numPi){
				p1_values[j] = core->PI_list[gateptr->input_list[j]];
				p0_values[j] = 1 - core->PI_list[gateptr->input_list[j]];
				pd_values[j] = 0; pdn_values[j] = 0;
				continue;
			}
		pred_gateptr = core->fadjlist_ptr[gateptr->input_list[j]];
		p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
		pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
		}// end of input traversing
		compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
	}// End of gate_list

	for (i=core->numPi+1; i<=(core->numPi+core->numPo); i++){
		gptr1 = core->fadjlist_ptr[i];
		if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
			int gate_po_num =  i - core->numPi;
			affected = true ;
			cout << c << " " << session << "," << gate_po_num << endl ;
		}
	}

	return affected;
}


//I used this function to generate a schatter graph describing the affected POs in SET vs MET
bool MBU::fault_simulation_set(Core* core, configuration* config, FILE* resultfp, int session, int ff_index, char c){
	bool affected = false ;
		struct gate *gateptr;
		struct gate tgateptr1;
		struct gate *pred_gateptr;
		struct event* eventptr;

	int target;

	for(int k=0;k<core->gate_numbers; k++){
		if(core->gate_list[k]->gate_no==ff_index) target=k;
	}

		//------------------------------------------------------------
		//	set a random value to FFs and PIs;
		//------------------------------------------------------------
		//Assign random values to the inputs
		for (int i=1; i<=core->numPi; i++){
			long rand_value = rand();
			//Assigning random loginc to Primary Input
			short rand_logic = rand_value & 1;
			core->PI_list[i] = rand_logic;
			// Forwording the PI's value to its successors
			short node_adj_size = core->fadjlist[i].size();
			for(int j=0; j<node_adj_size; j++){
				gateptr = core->fadjlist[i][j];
				short pred_gate_node_number;
				for (int k=0; k<gateptr->input_count; k++){
					//pred_gate_node_number = radjlist[gateptr->out][k];
					pred_gate_node_number = gateptr->input_list[k];
					if (pred_gate_node_number==i)
						gateptr->input_values[k] = core->PI_list[i];
				}
			}
		}
		//Assign random values to DFFs
		for (int i=0; i<core->dff_num; i++){
			long rand_value = rand();
			short rand_logic= rand_value & 0x1;
			pred_gateptr = core->dfflist_ptr[i]->dgateptr;
			pred_gateptr->value = rand_logic;
			pred_gateptr->input_values[0] = rand_logic; //may be not required
			int fadjnum = core->dfflist_ptr[i]->q;
			short node_adj_size =core->fadjlist[fadjnum].size();
			for (int j=0; j<node_adj_size; j++){
				gateptr = core->fadjlist[fadjnum][j];
				short pred_gate_node_number;
				for (int k=0; k<gateptr->input_count; k++){
					//pred_gate_node_number = radjlist[gateptr->out][k];
					pred_gate_node_number = gateptr->input_list[k]; // this should be right
					if (pred_gate_node_number==fadjnum)
						gateptr->input_values[k] = pred_gateptr->value;
				}
			}
		}
		//------------------------------------------------------------
		//	Initial logic Simulation
		//	We need to know the value of the net before injecting faults
		//	so we need an initial logic simulation
		//------------------------------------------------------------
		//Propagating the input values through intermediate gates
		for (int i=0; i<core->gate_numbers; i++){
			pred_gateptr = core->gate_list[i];
			short fadj_number = pred_gateptr->out;
			if (pred_gateptr->is_dff == 1) continue;
			core->compute_gate_value(pred_gateptr);
			short node_adj_size =core->fadjlist[fadj_number].size();
			for (int j=0; j<node_adj_size; j++){
				gateptr = core->fadjlist[fadj_number][j];
				short pred_gate_node_number;
				for (int k=0; k<gateptr->input_count; k++){
					//pred_gate_node_number = radjlist[gateptr->out][k];
					pred_gate_node_number = gateptr->input_list[k]; // this should be right
					if (pred_gate_node_number==fadj_number)
						gateptr->input_values[k] = pred_gateptr->value;
				}
			}
		}

		//Propagating the values through FFs
		for (int i=0; i<core->dff_num; i++){
			pred_gateptr = core->dfflist_ptr[i]->dgateptr;
			pred_gateptr->value = pred_gateptr->input_values[0];
		}
		//------------------------------------------------------------
		//	End of Logic Simulation
		//------------------------------------------------------------

		//------------------------------------------------------------
		//	Monte Carlo Simulation
		//------------------------------------------------------------
		// starting from sorted gate list and compute events for each gate in order
		int t1 = rand() % config->tperiod; // circuit_period;
		for (int j=0; j<core->gate_numbers; j++){
			gateptr = core->gate_list[j];
			if(gateptr->is_dff) continue ;
			compute_gate_event_list(gateptr,core,config);
			if(j==target) statistical_fault_injection(core->gate_list[target], t1, 100);
		}

		for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
			gateptr = core->fadjlist_ptr[i];
			bool contaminated_po = false ;
			for(int j=0;j<gateptr->event_list.size()-1;j++){
				if ((gateptr->event_list[j]->pd !=0) || (gateptr->event_list[j]->pdn !=0)){
					contaminated_po = true ;
					affected=true;
				}
			}
			if( contaminated_po ){
				int po = i - core->numPi ;
				cout << c << "," <<session << "," << po << endl ;
			}
		}

		for (int i=0;i<core->gate_numbers; i++){
			gate* gateptr = core->gate_list[i];
			int list_size = gateptr->event_list.size();

			for (int j=list_size-1; j>=0; j--){
				event* eventptr = gateptr->event_list[j];
				eventptr = NULL;
				gateptr->event_list.pop_back();
			}
			gateptr->event_list.erase(gateptr->event_list.begin(), gateptr->event_list.end());
		}

		return affected;
}


bool MBU::fault_simulation_met(Core* core, configuration* config, FILE* resultfp, int session, int ff_index1, int ff_index2, char c){
	bool affected = false ;
	struct gate *gateptr;
	struct gate tgateptr1, tgateptr2;
	struct gate *pred_gateptr;
	struct event* eventptr;

	int target1=-1, target2=-1;
	for(int k=0;k<core->gate_numbers; k++){
		if(core->gate_list[k]->gate_no==ff_index1) target1=k;
		if(core->gate_list[k]->gate_no==ff_index2) target1=k;
	}

	//set a random value to FFs and PIs;
	//Assign random values to the inputs
	for (int i=1; i<=core->numPi; i++){
		long rand_value = rand();
		//Assigning random loginc to Primary Input
		short rand_logic = rand_value & 1;
		core->PI_list[i] = rand_logic;
		// Forwording the PI's value to its successors
		short node_adj_size = core->fadjlist[i].size();
		for(int j=0; j<node_adj_size; j++){
			gateptr = core->fadjlist[i][j];
			short pred_gate_node_number;
			for (int k=0; k<gateptr->input_count; k++){
				//pred_gate_node_number = radjlist[gateptr->out][k];
				pred_gate_node_number = gateptr->input_list[k];
				if (pred_gate_node_number==i)
					gateptr->input_values[k] = core->PI_list[i];
			}
		}
	}
	//Assign random values to DFFs
	for (int i=0; i<core->dff_num; i++){
		long rand_value = rand();
		short rand_logic= rand_value & 0x1;
		pred_gateptr = core->dfflist_ptr[i]->dgateptr;
		pred_gateptr->value = rand_logic;
		pred_gateptr->input_values[0] = rand_logic; //may be not required
		int fadjnum = core->dfflist_ptr[i]->q;
		short node_adj_size =core->fadjlist[fadjnum].size();
		for (int j=0; j<node_adj_size; j++){
			gateptr = core->fadjlist[fadjnum][j];
			short pred_gate_node_number;
			for (int k=0; k<gateptr->input_count; k++){
				//pred_gate_node_number = radjlist[gateptr->out][k];
				pred_gate_node_number = gateptr->input_list[k]; // this should be right
				if (pred_gate_node_number==fadjnum)
					gateptr->input_values[k] = pred_gateptr->value;
			}
		}
	}

	//	Initial logic Simulation
	//	We need to know the value of the net before injecting faults
	//	so we need an initial logic simulation
	//Propagating the input values through intermediate gates
	for (int i=0; i<core->gate_numbers; i++){
		pred_gateptr = core->gate_list[i];
		short fadj_number = pred_gateptr->out;
		if (pred_gateptr->is_dff == 1) continue;
		core->compute_gate_value(pred_gateptr);
		short node_adj_size =core->fadjlist[fadj_number].size();
		for (int j=0; j<node_adj_size; j++){
			gateptr = core->fadjlist[fadj_number][j];
			short pred_gate_node_number;
			for (int k=0; k<gateptr->input_count; k++){
				//pred_gate_node_number = radjlist[gateptr->out][k];
				pred_gate_node_number = gateptr->input_list[k]; // this should be right
				if (pred_gate_node_number==fadj_number)
					gateptr->input_values[k] = pred_gateptr->value;
			}
		}
	}

	//Propagating the values through FFs
	for (int i=0; i<core->dff_num; i++){
		pred_gateptr = core->dfflist_ptr[i]->dgateptr;
		pred_gateptr->value = pred_gateptr->input_values[0];
	}
	//	End of Logic Simulation

	//	Monte Carlo Simulation
	// starting from sorted gate list and compute events for each gate in order
	bool fault_not_injected_yet = true ;
	int t1 = rand() % config->tperiod;
	for (int j=0; j<core->gate_numbers; j++){
		gateptr = core->gate_list[j];
		if(gateptr->is_dff) continue ;
		compute_gate_event_list(gateptr,core,config);
		if(j==target1) statistical_fault_injection(core->gate_list[target1], t1, 100);
		if(j==target2) statistical_fault_injection(core->gate_list[target2], t1, 100);
	}

	//SER-Computation
	//Finding affected POs
	for (int i=(core->numPi+1); i<=(core->numPi+core->numPo); i++){
		gateptr = core->fadjlist_ptr[i];
		bool contaminated_po = false ;
		for(int j=0;j<gateptr->event_list.size()-1;j++){
			if ((gateptr->event_list[j]->pd !=0) || (gateptr->event_list[j]->pdn !=0)){
				contaminated_po = true ;
				affected=true;
			}
		}
		if( contaminated_po ){
			int po = i - core->numPi ;
			cout << c << "," <<session << "," << po << endl ;
		}
	}

	//	Cleaning Up
	for (int i=0;i<core->gate_numbers; i++){
		gate* gateptr = core->gate_list[i];
		int list_size = gateptr->event_list.size();

		for (int j=list_size-1; j>=0; j--){
			event* eventptr = gateptr->event_list[j];
			delete eventptr;
			gateptr->event_list.pop_back();

		}
		gateptr->event_list.erase(gateptr->event_list.begin(), gateptr->event_list.end());

	}
	return affected;
}


void MBU::extract_TCAD_motivation_result(Core* core, configuration* config, FaultGenerator* fg, FaultSiteGenerator* fsg, FILE* resultfp){
	//Part1-schatter graph
	int session=0;
	do{
		int i=0;
		do{ i = random()%core->gate_numbers ;
		}while(fsg->getTotalFaultSite(i) == 0);

		int j = random()%fsg->getTotalFaultSite(i);
		vector<faultSite> vfs =  fsg->getFaultSiteList(i) ;
		faultSite fs = vfs[j];

		int target1 = fs.target1->gate_no;
		int target2 = fs.target2->gate_no;

		bool a0=0, a1=0, a2=0;
		srand ( session );
		a0 = fault_simulation_set(core, config, resultfp, session, target1, 'a');
		srand ( session );
		a1 = fault_simulation_set(core, config, resultfp, session, target2, 'b');
		srand ( session );
		a2 = fault_simulation_met(core, config, resultfp, session, target1, target2, 'c');
		bool affected = a0 || a1 || a2 ;
		if(affected) session++ ;
	}while(session<100);
}



