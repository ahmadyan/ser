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
