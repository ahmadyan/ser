/*
 *  MySideNote.cpp
 *  ser.make
 *
 *  Created by adel on ۸۹/۹/۱.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "MySideNote.h"


MBU::MBU(Core* core){
	S_sim = new float[core->gate_numbers];
	S_sim_var = new float[core->gate_numbers];
	S_sim_ff = new float[core->gate_numbers];
	S_sim_po = new float[core->gate_numbers];
	S_sim_ff_po = new  float[core->gate_numbers];
	
	S_sim_vec = vector< vector<ser> >(core->gate_numbers, vector<ser>());
	S_mlet_vec = vector< vector<ser> >(core->gate_numbers, vector<ser>());
	
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

void MBU::inject_fault(struct gate *gateptr, int time, int width){
	struct event* eventptr = new struct event;
	eventptr->time = time;
	eventptr->p0 = 0; eventptr->p1=0; eventptr->pd=1-gateptr->value; eventptr->pdn=gateptr->value;
	gateptr->event_list.push_back(eventptr);
	
	eventptr = new struct event;
	eventptr->time = time+ width;
	eventptr->p0 = 1-gateptr->value; eventptr->p1=gateptr->value; eventptr->pd=0; eventptr->pdn=0;
	gateptr->event_list.push_back(eventptr);
}


void MBU::inject_fault_analytical(struct gate *gateptr,int width){
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
	core->sim_gate_numbers=1430;
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
		
		int total_mbu_simulation_per_gate = 1 + // inject one fault on this gate
		core->gate_list[ff_index]->forward_list.size() + // inject two fault, one on this gate and one on it's fan-out /
		core->gate_list[ff_index]->backward_list.size() + // inject two fault, one on this gate and one on it's fan-in /
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
					//Logic Simulation end----
					
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
								inject_fault( core->gate_list[ff_index]->forward_list[mbu_index-1], t1, config->seu_pulse_width );
							}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size()){//backward
								//target+fan-out
								inject_fault( gateptr, t1, config->seu_pulse_width);
								inject_fault( core->gate_list[ff_index]->backward_list[mbu_index-core->gate_list[ff_index]->forward_list.size()-1], t1, config->seu_pulse_width);
							}else if( mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size() + number_of_common_fanin_injection ){
								//common-fan-in	
								for(int i=0;i<core->gate_list[ff_index]->forward_list.size();i++){
									for(int j=i+1; j<core->gate_list[ff_index]->forward_list.size();j++){
										inject_fault( core->gate_list[ff_index]->forward_list.at(i), t1 , config->seu_pulse_width);
										inject_fault( core->gate_list[ff_index]->forward_list.at(j), t1 , config->seu_pulse_width );
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


void MBU::system_failure_analysis_SingleCycle_LET_seu(configuration* config, Core* core, FILE* resultfp){
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
	for(int i=0;i<core->gate_numbers;i++){
		core->gate_list[i]->derating = 0 ;
	}
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
		
		int total_mbu_simulation_per_gate=1;
		for(int mbu_index=0; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
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
			float sys_failure=1.0;
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
				//				if (tsum != 0) sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				
				//sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				//if (tsum != 0) sys_failure *= 1- ((float)tpsum-(config->t_hold + config->t_setup))/((float)config->tperiod - config->seu_pulse_width );
				if(tpsum>0){
					if( ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod >= 1 ) sys_failure=0;
					else sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				}
			}//for (ff_ind=0; ....
			//Need revise.
			for (int gate_index=0; gate_index<core->gate_numbers; gate_index++){
				gateptr = core->gate_list[gate_index];
				if (gateptr->po_no>=0) sys_failure *= 1 - (gateptr->pd + gateptr->pdn);
			}
			sys_failure = 1 - sys_failure;
			if( sys_failure<0) sys_failure=0 ;
			//ff_index is gate_list index
			gateptr = core->gate_list[ff_index]; 
			gateptr->derating = sys_failure;
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
			}
			
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
		for(int i=0;i<total_mbu_simulation_per_gate;i++){
			sum += S_mlet_vec[ff_index].at(i).derating ;
		}
		sum /= total_mbu_simulation_per_gate;
		core->gate_list[ff_index]->derating = sum;
	}//for
	
	float overall_sys_der = 0;
	for (i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		//if (gateptr->is_dff==1)
		//	continue;
		fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sys_der += gateptr->derating;
	}
	overall_sys_der = overall_sys_der / (float)(core->gate_numbers);	// ???
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Systematic Derating = %f \n", overall_sys_der);
	fprintf(resultfp,"Overall Systematic derating per unit = %f \n", total_derating_sum/total_derating_iter);
	fprintf(resultfp, "Max event size in systematic derating=%d \n", max_event_size);
	
	
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
	
	
	for(int i=0;i<core->gate_numbers;i++){ delete g[i];  delete c[i]; }
	delete g; delete c;
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
	for(int i=0;i<core->gate_numbers;i++){
		core->gate_list[i]->derating = 0 ;
	}
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
		
		int total_mbu_simulation_per_gate = 1 + //inject one fault on this gate 
		core->gate_list[ff_index]->forward_list.size() + // inject two fault, one on this gate and one on it's fan-out /
		core->gate_list[ff_index]->backward_list.size() + //inject two fault, one on this gate and one on it's fan-in /
		number_of_common_fanin_injection +
		number_of_common_fanout_injection ;
		//cout << "[SIM] " << total_mbu_simulation_per_gate << " " << core->gate_list[ff_index]->forward_list.size() << " " << core->gate_list[ff_index]->backward_list.size() << " " << number_of_common_fanin_injection << " " << number_of_common_fanout_injection <<  endl ;
		
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
			float sys_failure=1.0;
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
				//				if (tsum != 0) sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				
				//sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				//if (tsum != 0) sys_failure *= 1- ((float)tpsum-(config->t_hold + config->t_setup))/((float)config->tperiod - config->seu_pulse_width );
				if(tpsum>0){
					if( ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod >= 1 ) sys_failure=0;
					else sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
				}
			}//for (ff_ind=0; ....
			//Need revise.
			for (int gate_index=0; gate_index<core->gate_numbers; gate_index++){
				gateptr = core->gate_list[gate_index];
				if (gateptr->po_no>=0) sys_failure *= 1 - (gateptr->pd + gateptr->pdn);
			}
			sys_failure = 1 - sys_failure;
			if( sys_failure<0) sys_failure=0 ;
			//ff_index is gate_list index
			gateptr = core->gate_list[ff_index]; 
			gateptr->derating = sys_failure;
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
			}
			
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
		fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sys_der += gateptr->derating;
	}
	overall_sys_der = overall_sys_der / (float)(core->gate_numbers);	// ???
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Systematic Derating = %f \n", overall_sys_der);
	fprintf(resultfp,"Overall Systematic derating per unit = %f \n", total_derating_sum/total_derating_iter);
	fprintf(resultfp, "Max event size in systematic derating=%d \n", max_event_size);
	
	
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
	
}


/**	Function: system_failure_simulation_SingleCycle_LogicalDerating
 Purpose:  Computes system failure probability at first clock using simulation method (Applying large amount of vectors) 
 **/
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
 **/
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
    color[u] = 'g';
    for(int i=0; i<fadjlist[u].size(); i++){
        struct gate *p = fadjlist[u][i];
        short v = p->out;
        if(color[v] == 'w'){
			pi[v] = u;
            dfs_fvisit(core->fadjlist, v, core);    // Recursive call
        }
    }
    color[u] = 'b';
    dfs_fnum[u] = ++final_time_stamp;
}


void MBU::find_TPsort_from_dffi(short nff_index, Core* core,configuration* config){
	deque <short> tmp_constant;
	struct gate *p;
	
	PI2POlist.clear();
	color.clear();
	for(int i=1; i<=core->nodes; i++){
		color.push_back('w');
		pi[i] = config->MAX;
	}
	for (int i=0; i<core->dff_num; i++)
		color[core->dfflist_ptr[i]->q]='g';
	
	//Apply DFS starting from node nff_index
	pi[nff_index] = 0;
	final_time_stamp = 0;
	dfs_fvisit(core->fadjlist, nff_index, core);
	
	for(int i=1; i<core->nodes; i++){
		if (pi[i] != config->MAX){
			tmp_constant.clear();
			tmp_constant.push_back(i);
			PI2POlist.push_back(tmp_constant);
		}
	}
	
	//sort the PI2POlist
	short PI2POlist_size = core->PI2POlist.size();
	for (int i=0; i < (PI2POlist.size()-1); i++){
		for (int j=i+1; j<PI2POlist.size(); j++){
			if (dfs_fnum[PI2POlist[i][0]] < dfs_fnum[PI2POlist[j][0]]){
				short val = PI2POlist[i][0];
				PI2POlist[i][0] = PI2POlist[j][0];
				PI2POlist[j][0] = val;
			}
		}
	}
	
	//Here we creating forward-adjancy list for on-path gates
	for (int i=0; i<PI2POlist.size(); i++){
		for (int j=0; j<core->fadjlist[PI2POlist[i][0]].size(); j++){
			p = core->fadjlist[PI2POlist[i][0]][j];
			if (pi[p->out] != config->MAX){
				PI2POlist[i].push_back(p->out);
			}
		}
	}
}


//for compatibility only
void MBU::compute_D_gate_event_list(struct gate* gateptr, int targe_gate_node_1, configuration* config, Core* core){
	compute_D_gate_event_list(gateptr, targe_gate_node_1, -1, config, core);
}

void MBU::compute_D_gate_event_list(struct gate* gateptr, int targe_gate_node_1, int target_gate_node_2, configuration* config, Core* core){
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
	
	if ((gateptr->out==targe_gate_node_1)||(gateptr->out==target_gate_node_2)){ //skip the faulty gate
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

/*
 *	Multi-Cycle Monte-Carlo Simulation for MBU fault model
 *	Prepared for DSN11
 */
void MBU::system_failure_simulation_multicycle_LET(configuration* config, Core* core, FILE* resultfp, FaultGenerator* fg, FaultSiteGenerator* fsg){
}

void MBU::system_failure_analysis_multicycle_LET(configuration* config, Core* core, FILE* resultfp, FaultGenerator* fg, FaultSiteGenerator* fsg){	
	struct gate *gateptr;
	
	for (int ff_index=0; ff_index<core->gate_numbers; ff_index++){
		//If this gate is directly connected to any of POs, then the ser is 1.
		if (core->gate_list[ff_index]->out <= (core->numPi+core->numPo)){
			core->gate_list[ff_index]->derating = 1;
			continue;
		}
		vector<faultSite> fault_site_list = fsg->getFaultSiteList(ff_index);
		int total_mbu_simulation_per_gate  = fsg->getTotalFaultSite(ff_index);
		//By starting mbu_index from 1, we exclude injecting SETs, here we only inject METs.
		for(int mbu_index=1; mbu_index<total_mbu_simulation_per_gate; mbu_index++){
			//initializing <0,1,0^e,1^e> values for all-gates
			for (int i=0; i<core->gate_numbers; i++){
				gateptr = core->gate_list[i];
				gateptr->p0 = 1- gateptr->signal_probability;
				gateptr->p1 = gateptr->signal_probability;
				gateptr->pd = 0;
				gateptr->pdn = 0;
				gateptr->is_onpath = false;
			}
			
			cout << "[X] Injecting fault events" << endl ;
			// Injecting multiple faults, index 0 is only used in case of single event transients.
			if(fault_site_list[mbu_index].target_count==1){//seu
				inject_fault_analytical( fault_site_list[mbu_index].target1, config->seu_pulse_width);
			}else{//mbu
				cout << "fault sites=" << fault_site_list[mbu_index].target1->out << " " <<  fault_site_list[mbu_index].target2->out  << endl;
				inject_fault_analytical( fault_site_list[mbu_index].target1, config->seu_pulse_width);
				inject_fault_analytical( fault_site_list[mbu_index].target2, config->seu_pulse_width);
			}
			
			//finding on-path gates from fault sites
			find_TPsort_from_dffi(fault_site_list[mbu_index].target1->out, core, config);
			cout << "HERE*1 " << PI2POlist.size() << endl ;
			//int PI2POlist_size = core->PI2POlist.size();
			// Set MAX values to ensure each will get correct value
			for (int i=0; i<PI2POlist.size(); i++){
				cout << i << " " << PI2POlist[i][0] ;
				gateptr = core->fadjlist_ptr[PI2POlist[i][0]];
				gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;
				gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
				gateptr->is_onpath = true;
			}
			
			find_TPsort_from_dffi(fault_site_list[mbu_index].target2->out, core, config);
			cout << "HERE*2 " << PI2POlist.size() << endl ;
			for (int i=0; i<core->gate_numbers; i++) {
				cout << i << "> "  << core->fadjlist_ptr[i] << endl ;
			}
			//PI2POlist_size = core->PI2POlist.size();
			for (int i=0; i<PI2POlist.size(); i++){
				cout << i << " " << PI2POlist[i][0] ;
				gateptr = core->fadjlist_ptr[PI2POlist[i][0]];
				gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;
				gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
				gateptr->is_onpath = true;
			}
			
			for (int i=0; i<core->gate_numbers; i++) {
				cout << "onpath=" << i << " " << core->gate_list[i]->is_onpath << endl ;
			}
			//TODO: huff, till here
			//Simulating first cycle
			//for (int i=0; i<core->gate_numbers; i++){
			//	gateptr = core->gate_list[i];
			//		compute_D_gate_event_list(gateptr, core->gate_list[ff_index]->out,config,core);
			//		if (gateptr->event_list.size()>max_event_size)
			//			max_event_size = gateptr->event_list.size();
			//	}// End of PI2POlist
			
			
			
		}
		
	}
}

/*
 void MBU::system_failure_analysis_multicycle_LET(configuration* config, Core* core, FILE* resultfp){
 short PI2POlist_size;
 struct gate *gateptr;
 int list_size;
 int max_event_size=0;
 
 cout << "Analyzing Circuit Soft Error Rate." << endl ;
 for (int ff_index=0; ff_index<core->gate_numbers; ff_index++) {
 cout << "analyzing gate# " << ff_index << endl ;
 //Extracting Fault Sites
 int number_of_common_fanout_injection= core->gate_list[ff_index]->forward_list.size() * (core->gate_list[ff_index]->forward_list.size()-1) /2 ;
 int number_of_common_fanin_injection  = core->gate_list[ff_index]->backward_list.size() * (core->gate_list[ff_index]->backward_list.size()-1) /2;
 int total_mbu_simulation_per_gate = 1 +			  //inject one fault on this gate 
 core->gate_list[ff_index]->forward_list.size() +  // inject two fault, one on this gate and one on it's fan-out /
 core->gate_list[ff_index]->backward_list.size() + // inject two fault, one on this gate and one on it's fan-in /
 number_of_common_fanin_injection +
 number_of_common_fanout_injection ;
 //cout << "[SIM] " << total_mbu_simulation_per_gate << " " << core->gate_list[ff_index]->forward_list.size() << " " << core->gate_list[ff_index]->backward_list.size() << " " << number_of_common_fanin_injection << " " << number_of_common_fanout_injection <<  endl ;
 
 for (int mbu_index=1; mbu_index<total_mbu_simulation_per_gate; mbu_index++) {
 cout << "mbu_injection_experiment#" << mbu_index << endl ;
 // To get PI2POlist starting from flip-flop nff_index excluding it
 find_TPsort_from_dffi(core->gate_list[ff_index]->out, core, config);
 PI2POlist_size = core->PI2POlist.size();
 cout << PI2POlist_size << endl ;
 // Set MAX values to ensure each will get correct value
 for (int i=0; i<core->gate_numbers; i++){
 cout << i << endl ;
 gateptr= core->gate_list[i];
 gateptr->p0 = 0; gateptr->p1 = 0;
 gateptr->pd = 0; gateptr->pdn = 0;
 gateptr->is_onpath = false;
 }
 cout << "---" << endl ;
 for (int i=0; i<PI2POlist_size; i++){
 cout << i << endl ;
 gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
 gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;
 gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
 gateptr->is_onpath = true;
 }
 gateptr = core->gate_list[ff_index];
 
 cout << "Yang ogonad;j" << endl ;
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
 
 cout << "Firsrt Cycle" << endl ;
 //First Cycle Analyzing...
 for (int i=0; i<PI2POlist_size; i++){
 gateptr = core->fadjlist_ptr[core->PI2POlist[i][0]];
 compute_D_gate_event_list(gateptr, core->gate_list[ff_index]->out,config,core);
 if (gateptr->event_list.size()>max_event_size)
 max_event_size = gateptr->event_list.size();
 }// End of PI2POlist
 
 for (int i=0; i<core->dff_num; i++){
 gateptr = core->fadjlist_ptr[core->dfflist_ptr[i]->q];
 compute_D_gate_event_list(gateptr, core->gate_list[i]->out,config,core);
 if (gateptr->event_list.size()>max_event_size)
 max_event_size = gateptr->event_list.size();
 }
 //Checking if any fault has been latched in any of flip-flops
 for (int ff_ind=0; ff_ind<core->dff_num; ff_ind++){
 gateptr = core->fadjlist_ptr[core->dfflist_ptr[ff_ind]->q];
 list_size = gateptr->event_list.size();
 float tpsum = 0;
 float tsum = 0;
 float sys_failure = 1;
 for (int k=0; k<gateptr->event_list.size()-1; k++){
 int tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
 if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
 tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
 tsum += tk;
 }
 //if (tsum != 0) sys_failure *= 1- ((float)tpsum+(config->t_hold + config->t_setup)*(float)tpsum/(float)tsum)/(float)config->tperiod;
 //				if (tsum != 0) sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
 
 //sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
 //if (tsum != 0) sys_failure *= 1- ((float)tpsum-(config->t_hold + config->t_setup))/((float)config->tperiod - config->seu_pulse_width );
 if(tpsum>0){
 if( ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod >= 1 ) sys_failure=0;
 else sys_failure *= 1- ((float)tpsum+config->t_hold + config->t_setup)/(float)config->tperiod;
 }
 }//for (ff_ind=0; ....
 
 
 //Computing SER for first cycle
 
 
 //Now, the next cycles
 for (int clock=1; clock<config->clock; clock++) {
 cout << "doing clock " << clock << endl; 
 }
 
 }
 }
 }
 */