#include "seu.h"


SEU::SEU(Core* core){
	S_sim = new float[core->gate_numbers];
	S_sim_var = new float[core->gate_numbers];
	S_sim_ff = new float[core->gate_numbers];
	S_sim_po = new float[core->gate_numbers];
	S_sim_ff_po = new  float[core->gate_numbers];
}

SEU::~SEU(void){
	delete S_sim ;
	delete S_sim_var ;
	delete S_sim_ff ;
	delete S_sim_po; 
	delete S_sim_ff_po ;
}


/**	Function: system_failure_simulation_MultiCycle_LogicalDerating
	 Purpose:  Computes system failure probability at first clock using simulation method (Applying large amount of vectors) 
**/
void SEU::system_failure_simulation_MultiCycle_LogicalDerating(configuration* config, Core* core, FILE* resultfp){
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


void SEU::report(Core* core, FILE* resultfp){
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
void trreverse(char* begin, char* end);
void itoa(int value, char* str, int base);
/////////////////////////////////////////////////////////////////////////////////////
// Function: compute_D_Probability
// Purpose:  Computes D Probability of the given gate from inputs
// Returns none
void SEU::compute_D_Probability(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn){
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


