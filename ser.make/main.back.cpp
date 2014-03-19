
/*
	legacy code, don't use these global variables anymore, instead use the core class.
*/
//DELAY Characterization
vector<float> lib_inputc, lib_tplh_fix, lib_tphl_fix, lib_tplh_load, lib_tphl_load, lib_area;
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
// reachOp[i] is list of all reachable outputs from node i
// Note if ReachNodesFromPOi is called for POj, reachOp is list of all reachable nodes from POj
vector < deque<short> > reachOp;            // reachable nodes from outputs
// color[i] is color of node i
vector <char> color;                      // Used for DFS search
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
// Linked list node to form a linked list of inputs and outputs

// Global data
struct inout *headPi, *headPo;
short numPi, numPo;
short noofClauses;
int* epp_ranking=NULL;// the sorted list of gate_list w.r.t their EPP values
int* epp_area_ranking=NULL; // the sorted list of gate_list w.r.t their EPP over area
short* pi=NULL; // it is used for dfs algorithm
short* dfs_fnum=NULL; // To detecte finalize number (black) after dfs completion
short final_time_stamp = 0; // is used for topological sorting number during DFS algorithm
short* PI_list=NULL;				// used for assigning random values to the inputs during logic simulation
float* PI_list_sp=NULL;				// used for input values signal probabilities !
short nodes; // Number of nodes in the circuit ** Number of nodes of the circuit is nodes-1
short gate_numbers=0; // number of gates in the circuit including flop-flops
short gate_numbers_noff=0; //number of gates excluding flip-flops
short sim_gate_numbers=0; // the number of gates being simulated
short dff_num=0;	  // number of flop-flops in the circuit
float** SFO=NULL;//[MAX_DFF][MAX_PO];
float** SFF=NULL;//[MAX_DFF][MAX_DFF]; //Flip-flops probabaility Mij = P(fault appear in FFi | FFj is faulty)
float** SGO=NULL;//[MAX][MAX_PO];
float** SGF=NULL;//[MAX][MAX_DFF];
float** SatCK=NULL;//[MAX][MAX_CLK];  //SatCK[i][j] is the failure probability of the i'th gate in the gate_list at cycle j-1
float** S1_to_CK=NULL;//[MAX][MAX_CLK];
float* S=NULL;//[MAX],
float* eppPerGate_CLK1=NULL;
float* Sck=NULL;//[MAX]; //System failure probability vector Si = P(system failure | FFi is faulty)
//float S_FF_ck[MAX_DFF][MAX_CLK+1];
//int mttm[MAX_DFF];
float** Mtemp = NULL;
float** MC = NULL;
float** SGOatCK = NULL;
float** tmp_mat = NULL;

//float S_sim[MAX][MAX_CLK]; //System failure probability vector obtained from simulation
float* S_sim=NULL;//[MAX]; //System failure probability vector obtained from simulation
float* S_sim_clk=NULL;//[MAX]; //System failure probability vector obtained from simulation
float* S_sim_single_cycle_po=NULL ;
float* S_sim_single_cycle_ff=NULL ;
float* S_sim_single_cycle_po_ff=NULL ;
float* S_sim_var=NULL;//[MAX]; //Variances of system failure probability vector obtained from simulation
float* S_sim_clk_var=NULL;//[MAX]; //Variances of system failure probability vector obtained from simulation



FILE *resultfp;		// Output result file
long ex_number;
float max_delay;
float circuit_sim_derating, circuit_sys_derating;
float circuit_area, max_area;// = 1.1;
configuration* config;
mbu* mbu_site ;
#ifdef _WIN32
void srandom(int seed) {srand(seed);} //comment this line for SUN systems
int random() { return rand();}		//comment this line for SUN systems
#endif






//====================================================================================================
//====================================================================================================
//====================================================================================================
//====================================================================================================
//====================================================================================================
void check_netlist(){
	int val1,val2;
	struct gate *gptr1;
	for (int i= (numPi+1); i<nodes; i++){
		gptr1 = fadjlist_ptr[i]; 
		if( gptr1 != NULL ){
		}else{
			cout << i << endl ;
		}
	}
}

void reset(){
	for (int i=0; i<gate_numbers; i++){
		struct gate *gateptr= gate_list[i];
		gateptr->p0 = 0;
		gateptr->p1 = 0;
		gateptr->pd = 0;
		gateptr->pdn = 0;

		gateptr->q0 = 0 ;
		gateptr->q1 = 0 ;
		gateptr->qd = 0 ;
		gateptr->qdn = 0 ;


		gateptr->derating= 0 ;
		gateptr->sderating= 0 ;
		gateptr->total_iteration= 0 ;

		gateptr->p0_var = 0 ; 
		gateptr->p1_var = 0 ;
		gateptr->pd_var= 0 ;
		gateptr->pdn_var= 0 ; 
		gateptr->vomin= 0 ;
		gateptr->vomax= 0 ;
		gateptr->vimin= 0 ;
		gateptr->vimax= 0 ;
	}
	for (int i=0;i<gate_numbers; i++){
		struct gate *gateptr = gate_list[i];
		int list_size = gateptr->event_list.size();
		for (int j=list_size-1; j>=0; j--){
			struct event* eventptr = gateptr->event_list[j];
			delete eventptr;
			eventptr = NULL;
			gateptr->event_list.pop_back();
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: aro
// Purpose: Finds all reachable outputs from each node in the circuit
// Stores the information in the form of an adjacency list
void aro(vector <deque<short> > &radjist, struct inout *head, short nodes)
{
  short i;
  struct inout *p;

  p = head;

  do {
    // Clear
    color.clear();
    for(i=1; i<=config->MAX; i++)
      color.push_back('w');

    dfs_rvisit(radjlist, p->num, p->num);
    p = p->next;
  } while (p != NULL);
}//aro


//////////////////////////////////////////////////////////////////////////////////////
// Function: ReachNodesFromPOi
// Purpose: Finds all reachable nodes in the circuit from Primary OUTPUT POj
// Stores the information in the form of an adjacency list
void ReachNodesFromPOi(vector <deque<short> > &radjist, short POj)
{
  short i;
  //struct inout *p;

    // Clear
    color.clear();
    for(i=1; i<=config->MAX; i++)
      color.push_back('w');

    dfs_rvisit(radjlist, POj, POj);
}//ReachNodesFromPOi


//////////////////////////////////////////////////////////////////////////////////////
// Function: dfs_rvisit
// Purpose:  Performs DFS visit on the reverse graph to find all
// reachable outputs
void dfs_rvisit(vector <deque<short> > &radjlist, short u, short op)
{
    short i, v;

    color[u] = 'g';

    for(i=0; i<radjlist[u].size(); i++)
    {
        v = radjlist[u][i];
        if (v>numPi)
            if (fadjlist_ptr[v]->is_dff==1) continue;
        if(color[v] == 'w')
            dfs_rvisit(radjlist, v, op);    // Recursive call
    }
    color[u] = 'b';
    reachOp[u].push_front(op);
}//dfs_rvisit


//////////////////////////////////////////////////////////////////////////////////////
// Function: dfs_fvisit
// Purpose:  Performs DFS visit on the forward graph. Used to extract
// faulty circuit from the fault site to the outputs
void dfs_fvisit (vector <deque<struct gate*> > &fadjlist, short u)
{
    short i, v;
    struct gate *p;

    color[u] = 'g';

    for(i=0; i<fadjlist[u].size(); i++)
    {
        p = fadjlist[u][i];
        v = p->out;
        //if (fadjlist_ptr[v]->is_dff==1) continue;
        if(color[v] == 'w')
        {
			//cout << "colored node=" << v << endl;
            pi[v] = u;
            dfs_fvisit(fadjlist, v);    // Recursive call
        }
    }
    color[u] = 'b';
    dfs_fnum[u] = ++final_time_stamp;
}//dfs_fvisit



//////////////////////////////////////////////////////////////////////////////////////
// Function: appendPi
// Purpose: Forms a linked list of all input nodes
void appendPi(struct inout *headPi, short net)
{
  struct inout *p, *q, *ptr;

  p = headPi;

  // Move to end of list
  do {
    q = p;
    p = p->next;
  } while (p != NULL);

  // Create new node and append at the end of the list
  //ptr = (struct inout*) malloc(sizeof(struct inout));
  ptr = new struct inout;
  ptr->num = net;
  ptr->next = NULL;
  q->next = ptr;
}//appendPi


//////////////////////////////////////////////////////////////////////////////////////
// Function: appendPo
// Purpose: Forms a linked list of all output nodes
void appendPo(struct inout *headPo, short net)
{
  struct inout *p, *q, *ptr;

  p = headPo;

  // Move to end of list
  do {
    q = p;
    p = p->next;
  } while (p != NULL);

  // Create new node and append at the end of the list
  //ptr = (struct inout*) malloc(sizeof(struct inout));
  ptr = new struct inout;
  ptr->num = net;
  ptr->next = NULL;
  q->next = ptr;
}//appendPo

//////////////////////////////////////////////////////////////////////////////////////
// Function: extractField
// Purpose:  Extracts one field from input netlist file
// Returns 1 if field is success
short extractField(FILE *fp, char *field)
{
  short i=0;
  char x;

  while(1) {
    x = fgetc(fp);
	
	if ((x==EOF)||(x<0))
		return -1;
    if (i==0)
	{
		//printf("%c %x\n", x , x);
		if (x == ';') return 2 ;
	      //return 3;
    	if ((x==' ')||(x=='\t')||(x==':')||(x=='\n')||(x==',')||(x=='(')||(x== ')'))
			continue;
	}
	//char separators1[20] = " :=-><(),\"\t\n";
    // Detects ';' (End of line)
    if(x == ';') {
      field[i] = '\0';
      return 2;
    }

    // End of string
    else if((x==' ')||(x=='\t')||(x=='\n')||(x==',')||(x==':')||(x=='(')||(x== ')'))
	{
      field[i] = '\0';
      return 1;
    }

    // Append character
    else
      field[i++] = x;
  }
}//extractField

//////////////////////////////////////////////////////////////////////////////////////
// Function: logic_simulation
// Purpose:  Perfroms logic simulation on the gate_list
// Returns none
void logic_simulation(short run_for_sp, vcd* signals){
    short i,j,k;
    long rand_value;
    short rand_logic;
    struct gate *gateptr;
    struct gate *pred_gateptr;
	struct gate *gptr1;
	for (i= (numPi+1); i<nodes; i++){
		gptr1 = fadjlist_ptr[i]; 
		if( gptr1 != NULL ){
			gptr1->value=0;
		}
	}
		
    if (PI_list == NULL) PI_list = new short[numPi+1];
        //PI_list = (short*)malloc((numPi+1)*sizeof(short));
	if(config->verbose){
		if (!run_for_sp){
			cout << endl << "RAND_MAX is: " << RAND_MAX << endl;
			fprintf(resultfp, "\n RAND_MAX is: %d \n", RAND_MAX);
		}
	}
	
	for (i=0; i<gate_numbers; i++)
		gate_list[i]->value = config->MAX;
    //Assign random values to the inputs
    for (i=1; i<=(numPi); i++){
		if(config->use_vcd){ //read signal values from value change dump (vcd) file generated from modelsim or other hdl simulator
			short signal_value = signals->getNextLogicValue(i); 
			PI_list[i] = signal_value;
			PI_list_sp[i] += signal_value;
		}else{ // randomly assign values to PIs.
			rand_value = random();                    // random function on Unix (Better Performance)
			//rand_value = rand();                    // random function on Windows &Unix
			//Assigning random loginc to Primary Input
			rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
			PI_list[i] = rand_logic;
			PI_list_sp[i] += rand_logic;
		}
		
		// Forwording the PI's value to its successors
        short node_adj_size =fadjlist[i].size();
        for (j=0; j<node_adj_size; j++){
            gateptr = fadjlist[i][j];
            short pred_gate_node_number;
            for (k=0; k<gateptr->input_count; k++){
                //pred_gate_node_number = radjlist[gateptr->out][k];
                pred_gate_node_number = gateptr->input_list[k]; // this should be right
                if (pred_gate_node_number==i)
                    gateptr->input_values[k] = PI_list[i];
            }
        }
		if(config->verbose){
			if (!run_for_sp){
				fprintf(resultfp,"%d \n", PI_list[i]);
			}
		}
    }

    //Assign random values to DFFs
    for (i=0; i<dff_num; i++){
        rand_value = random();                // random function on Unix (Better Performance)
        //rand_value = rand();                    // random function on Windows &Unix
        rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
        pred_gateptr = dfflist_ptr[i]->dgateptr;
        pred_gateptr->value = rand_logic;
        pred_gateptr->input_values[0] = rand_logic; //may be not required
        int fadjnum = dfflist_ptr[i]->q;
        short node_adj_size = fadjlist[fadjnum].size();
        for (j=0; j<node_adj_size; j++){
            gateptr = fadjlist[fadjnum][j];
            short pred_gate_node_number;
            for (k=0; k<gateptr->input_count; k++){
                //pred_gate_node_number = radjlist[gateptr->out][k];
                pred_gate_node_number = gateptr->input_list[k]; // this should be right
                if (pred_gate_node_number==fadjnum)
                    gateptr->input_values[k] = pred_gateptr->value;
            }
        }
		if(config->verbose){
			if (!run_for_sp){
				cout << " :: "<< pred_gateptr->value << endl;
				fprintf(resultfp,"%d \n", pred_gateptr->value);
			}		
		}
    }

    //Propagating the input values through intermediate gates
    for (i=0; i<gate_numbers; i++){
        pred_gateptr = gate_list[i];
        short fadj_number = pred_gateptr->out;
        if (pred_gateptr->is_dff == 1) continue;
        compute_gate_value(pred_gateptr);
        short node_adj_size =fadjlist[fadj_number].size();
        for (j=0; j<node_adj_size; j++){
            gateptr = fadjlist[fadj_number][j];
            short pred_gate_node_number;
            for (k=0; k<gateptr->input_count; k++){
                //pred_gate_node_number = radjlist[gateptr->out][k];
           	    pred_gate_node_number = gateptr->input_list[k]; // this should be right
                if (pred_gate_node_number==fadj_number)
                    gateptr->input_values[k] = pred_gateptr->value;
            }
        }
    }

    //Propagating the values through FFs
    for (i=0; i<dff_num; i++){
        pred_gateptr = dfflist_ptr[i]->dgateptr;
		pred_gateptr->value = pred_gateptr->input_values[0];
    }

	////// Print result of logic simulation
	if(config->verbose){
		if (!run_for_sp){
			cout << "//////////////////////////////////////" << endl;
			cout << "result of logic simulation" << endl;
			cout << "//////////////////////////////////////" << endl;
			fprintf(resultfp,"\n //////////////////////////////////////\n");
			fprintf(resultfp,"result of logic simulation\n");
			fprintf(resultfp,"//////////////////////////////////////\n" );
			for (i=1; i<=numPi; i++){
				fprintf(resultfp, "input %d = %d \n", i, PI_list[i]);
			}
			//for (i= (numPi+1); i<nodes; i++)
			//   if (fadjlist_ptr[i]!=NULL)
			//fprintf(resultfp, "Node %d = %d \n", i, fadjlist_ptr[i]->value);
			for (i=0; i<gate_numbers; i++)
				fprintf(resultfp, "(gate=%d) Node %4d = %d \n", i,gate_list[i]->out, gate_list[i]->value);
		}
	}
	int val1,val2;
	bool is_in_range = true;
	for (i= (numPi+1); i<nodes; i++){
		gptr1 = fadjlist_ptr[i]; 
		if( gptr1 != NULL ){
			if ((gptr1->value!=0) && (gptr1->value!=1)){
				cout << i <<" gptr1->value=" << gptr1->value << endl ;
				is_in_range = false;
			}	
			val1 = fadjlist_ptr[i]->sum_values;
			val2 = fadjlist_ptr[i]->value;
			fadjlist_ptr[i]->sum_values += fadjlist_ptr[i]->value;
		}else{
			cout << "." << i << endl ;
		}
	}
	if (is_in_range==false) fprintf(resultfp, "ALARM: logic value is not 0 or 1! \n");
	ex_number++;
}//logic_simulation

//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_gate_value
// Purpose:  Computes the gate value
// Returns none
// Revised on 5-1389, adding support for lut-type gate
void compute_gate_value(struct gate* gateptr){
	int input = 0 ;
    for (int i=0; i<(gateptr->input_count); i++)
        if ((gateptr->input_values[i]!=0) && (gateptr->input_values[i]!=1)){
            //cout << "ALARM:" << gateptr->input_list[i] << endl;
            //fprintf(resultfp, "ALARM: %d  \n", gateptr->input_list[i]);
        }

    switch (gateptr->type) {
        case 0:     //nand
                gateptr->value = gateptr->input_values[0];
                for (int i=1; i<(gateptr->input_count); i++)
                    gateptr->value &=  gateptr->input_values[i];
                if (gateptr->value==0)
                    gateptr->value = 1;
                else
                    gateptr->value = 0;
                break;
        case 1:     //and
                gateptr->value = gateptr->input_values[0];
                for (int i=1; i<(gateptr->input_count); i++)
                    gateptr->value &=  gateptr->input_values[i];
                break;
        case 2:     //nor
                gateptr->value = gateptr->input_values[0];
                for (int i=1; i<(gateptr->input_count); i++)
                    gateptr->value |=  gateptr->input_values[i];
                if (gateptr->value==0)
                    gateptr->value = 1;
                else
                    gateptr->value = 0;
                break;
        case 3:     //or
                gateptr->value = gateptr->input_values[0];
                for (int i=1; i<(gateptr->input_count); i++)
                    gateptr->value |=  gateptr->input_values[i];
                break;
        case 4:     //xor
                gateptr->value = gateptr->input_values[0];
                for (int i=1; i<(gateptr->input_count); i++)
                    gateptr->value ^=  gateptr->input_values[i];
                break;
        case 5:     //xnor
                gateptr->value = gateptr->input_values[0];
                for (int i=1; i<(gateptr->input_count); i++)
                    gateptr->value ^=  gateptr->input_values[i];
                if (gateptr->value==0)
                    gateptr->value = 1;
                else
                    gateptr->value = 0;
                break;
        case 6:     //not
                gateptr->value = gateptr->input_values[0];
                if (gateptr->value==0)
                    gateptr->value = 1;
                else
                    gateptr->value = 0;
                break;
		case 8:	//lut
			//cout << "___________________________________" << endl ;
			//cout << gateptr->gate_no <<  " " << gateptr->is_dff << " " << gateptr->is_lut << " "  << endl ;
			
			//	for(int i=0;i<gateptr->input_count;i++){
			//		if( gateptr->input_values[gateptr->input_count-i-1]>1) gateptr->input_values[gateptr->input_count-i-1]=1;
			//		input = 2*input + gateptr->input_values[gateptr->input_count-i-1] ;
			//	}
				
			for(int i=0;i<gateptr->input_count;i++){
					if( gateptr->input_values[i]>1) gateptr->input_values[i]=1;
					input = 2*input + gateptr->input_values[i] ;
				}
				//for(int i=0;i<gateptr->input_count;i++){
				//	cout << i << " " << gateptr->input_values[i] << endl ;
				//}
				//cout << "input=" << input << endl ;
				//cout << "---" << endl ;
				if(input>gateptr->lut_size) gateptr->value=0;
				if( gateptr->fault[input] == 0 ){
					gateptr->value = gateptr->lut[input] ;
				}else{
					gateptr->value = 1-gateptr->lut[input] ;
				}
				//cout << "lut:" << endl; 
				//for(int i=0;i<gateptr->lut_size;i++){
				//	cout << gateptr->lut[i] << endl ;
				//}
				//cout << "---->value="<< gateptr->value << endl ;
			//cout << "___________________________________" << endl ;
				break;
        default:    // buff
                gateptr->value = gateptr->input_values[0];
                break;
    }

}//compute_gate_value



//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_signal_probability
// Purpose:  Computes signal probabilities of the circuit
// Returns none
void compute_signal_probability(int step_iteration, long max_iteration, vcd* VCD){
    long i;
    struct gate* gptr1;//,*gptr2;
	//int gate_index;
	//float total_variance;
	//float total_derating;
	float z_alpha_half = 2.576;
	float error_tolerance = sp_error_tolerance;
	long total_iteration = 0;

    // Initialize the sum_values
    if (PI_list_sp == NULL) PI_list_sp = new float[numPi+1];
		//PI_list_sp = (float*)malloc((numPi+1)*sizeof(float));
    for (i=1; i<=numPi; i++) PI_list_sp[i] = 0;

    for (i=0; i<gate_numbers; i++){
        gptr1 = gate_list[i];
		gptr1->total_variance  = 0;
		gptr1->signal_probability = 0;
		gptr1->sum_values = 0;
		gptr1->total_iteration = 0;
	}
    /*for (i= (numPi+1); i<nodes; i++)
        fadjlist_ptr[i]->sum_values = 0; //<should be changed> to set values i=0; i< gate_numbers
	*/
    ex_number = 0;
	logic_simulation(0,VCD);
	int circuit_total_iteration=0;
	float circuit_total_variance;
	float sum_values;
	// we are assuming that sp(PI)=0.5 and variance(PI)=0
	do{
		for (i=0; i<gate_numbers; i++){
			gptr1 = gate_list[i];
			gptr1->sum_values = 0;
		}

		for (i=0; i<step_iteration; i++)
			logic_simulation(1,VCD);

		/*fprintf(resultfp,"\n SUM VALUES with step iteraton = %d \n", step_iteration);
		for (i=0; i<gate_numbers; i++)
			fprintf(resultfp, "Node %d: sum_values = %d \n", gate_list[i]->out, gate_list[i]->sum_values);
		*/

		circuit_total_iteration += step_iteration;
		//fprintf(resultfp, "\n:::::: Iteration = %d \n", circuit_total_iteration);
		for (i=0; i<gate_numbers; i++){
			gptr1 = gate_list[i];
			sum_values = (float)gptr1->sum_values + (gptr1->signal_probability * (float)gptr1->total_iteration);
			gptr1->total_iteration += step_iteration;
			gptr1->signal_probability = sum_values/(float)gptr1->total_iteration;
			//sum_values = (sum_values==0) ? 1 : sum_values; // this is added to remove variance=0 from calculation
			gptr1->total_variance = sqrt((gptr1->signal_probability * (1 - gptr1->signal_probability)) / (float)gptr1->total_iteration);
			if (gptr1->signal_probability==0)
				gptr1->total_variance = sqrt(((1.0/(float)gptr1->total_iteration) * (1 - (1.0/(float)gptr1->total_iteration))) / (float)gptr1->total_iteration);

			//fprintf(resultfp, ":::::: SP(Node %4d\t) = %f,  Variance = %f, node's total iteration = %d\n", gptr1->out, gptr1->signal_probability, gptr1->total_variance, gptr1->total_iteration);
		}
		//fprintf(resultfp, "\n");

		long tmp_iter1=0; //,tmp_iter2;
		circuit_total_variance = 0;
		for (i=0; i<gate_numbers; i++){
			gptr1 = gate_list[i];
			if (gptr1->total_variance > circuit_total_variance)
				circuit_total_variance = gptr1->total_variance;
		}

		//long tmp_iter = (int)ceil(circuit_total_iteration * ((3*total_variance/error_tolerance)-1) * ((3*total_variance/error_tolerance)-1) );
//		if (tmp_iter1 < step_iteration)
//			step_iteration = tmp_iter1;
//		if (step_iteration < 1000)
//			step_iteration = 1000;
//		if (step_iteration > 10000)
//			step_iteration = 10000;
		if(config->verbose) cout << "simulated signal prob. for " << circuit_total_iteration << endl;
		//system_failure =  (float)failures/(float)step_iteration;
	} while (((z_alpha_half*circuit_total_variance) > error_tolerance) && (circuit_total_iteration<max_iteration) );
	//} while (0);


    for (i=1; i<=numPi; i++)
        PI_list_sp[i] = (float) PI_list_sp[i] / (float) circuit_total_iteration;
    //float sp_result;

/*    for (i=0; i<dff_num; i++)
    {
        gptr1 = fadjlist_ptr[dfflist_ptr[i]->d];
        gptr2 = fadjlist_ptr[dfflist_ptr[i]->q];
		if (gptr1->is_dff == true)
		{
			gptr2->signal_probability = 0.5;
			gptr2->total_variance = 0;
		}
		else
		{
        	gptr2->signal_probability = gptr1->signal_probability;
			gptr2->total_variance = gptr1->total_variance;
		}
    }
*/
	if(config->verbose){
		cout << "//////////////////////////////////////" << endl;
		cout << "Signal Probabilities of circuit nodes" << endl;
		cout << "//////////////////////////////////////" << endl;	
	}
    fprintf(resultfp,"\n//////////////////////////////////////\n");
    fprintf(resultfp,"result of Signal Probability Simulation\n");
	fprintf(resultfp, "Total iteration = %d \n", circuit_total_iteration);
    fprintf(resultfp,"//////////////////////////////////////\n" );

	for (i=0; i<gate_numbers; i++){
		gptr1 = gate_list[i];
       	fprintf(resultfp, "SP(Node %4d\t) = %f, Gate Type = %d, Variance = %f, \n", gptr1->out, gptr1->signal_probability, gptr1->type, gptr1->total_variance);
		if ((gptr1->signal_probability < 0) || (gptr1->signal_probability > 1)){
			fprintf(resultfp, " Signal Prob. ALARM \n");
			gptr1->signal_probability = 0.5;
		}
	}
}//compute_signal_probability
/////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// Function: signal_probability_report
// Purpose:  Reports on signal probabilities of the circuit
// Returns none
void signal_probability_report(void){
	int i;
    struct gate *gateptr;
	float sp5=0, sp10=0, sp15=0, sp20=0, sp25=0;
	int ngates = 0;
    for (i=0; i<gate_numbers; i++)
    {
        gateptr = gate_list[i];
		if (gateptr->type ==7) //Ignore DFFs
			continue;
		if ((gateptr->type >=6) && (gateptr->input_list[0]<numPi)) //Ignore PI buffers (INV or BUF)
			continue;
		ngates++;
		if ((gateptr->signal_probability<=0.05) || (gateptr->signal_probability>=0.95))
			sp5++;
		if ((gateptr->signal_probability<=0.10) || (gateptr->signal_probability>=0.90))
			sp10++;
		if ((gateptr->signal_probability<=0.15) || (gateptr->signal_probability>=0.85))
			sp15++;
		if ((gateptr->signal_probability<=0.20) || (gateptr->signal_probability>=0.80))
			sp20++;
		if ((gateptr->signal_probability<=0.25) || (gateptr->signal_probability>=0.75))
			sp25++;
	}
	sp5	 = sp5  * 100 / (float) ngates;
	sp10 = sp10 * 100 / (float) ngates;
	sp15 = sp15 * 100 / (float) ngates;
	sp20 = sp20 * 100 / (float) ngates;
	sp25 = sp25 * 100 / (float) ngates;

	fprintf(resultfp,"\n RESULTS FOR VLSI PROJECT: Signal probability reports\n");
	fprintf(resultfp,"\n sp5 = %f percent of gates", sp5);
	fprintf(resultfp,"\n sp10 = %f percent of gates", sp10);
	fprintf(resultfp,"\n sp15 = %f percent of gates", sp15);
	fprintf(resultfp,"\n sp20 = %f percent of gates", sp20);
	fprintf(resultfp,"\n sp25 = %f percent of gates", sp25);
	fprintf(resultfp,"\n \n");
}//signal_probability_report

//////////////////////////////////////////////////////////////////////////////////////
// Function: initialize_gate_delay
// Purpose:  initializ the gate delays based on specified process technology.
// Returns none
//
//
//	refer to CMOS VLSI Design ( West, Harris ) 3rd Ed. P. 165
//       and Application-Specific Integrated Circuits ( Michael John Sebastian Smith ) Chapter 13.6
//
//  we use linear delay model ( based on unlinear delay data for DSM process! )
//	delay_rise = intristic_rise + rise_resistance * capacitance
//	delay_fall = intristic_fall + fall_resistance * capacitance
// 
//  the intristic delay is usually called the parasitic delay in litrature, here we call it tp**_fix
//  on the other hand, we call the effort delay ( fall_resistance ) tp**_load. 
void initialize_gate_delay(struct gate *gptr1){

	if ((gptr1->type==4) || (gptr1->type==5)) //XOR or XNOR
		gptr1->inputc = 5.321f;
	else // All other gates
		gptr1->inputc = 2.661f;

	switch (gptr1->type) {
		case 0:     //NAND
			if (gptr1->input_count == 2) //NAND2
			{
				gptr1->tplh_fix  = 3.0f;		gptr1->tphl_fix = 5.0f;
				gptr1->tplh_load = 0.444f;	gptr1->tphl_load = 0.706f;
				gptr1->area = 1458;
			}
			else if (gptr1->input_count == 3) //NAND3
			{
				gptr1->tplh_fix  = 6.0f;		gptr1->tphl_fix = 6.0f;
				gptr1->tplh_load = 0.420f;	gptr1->tphl_load = 0.661f;
				gptr1->area = 1944;
			}
			else if (gptr1->input_count == 4) //NAND4
			{
				gptr1->tplh_fix  = 7;		gptr1->tphl_fix = 7;
				gptr1->tplh_load = 0.451f;	gptr1->tphl_load = 0.663f;
				gptr1->area = 2430;
			}
			else if (gptr1->input_count == 5) //NAND5: constructed by AND5 + INV
			{
				gptr1->tplh_fix = 6+(0.420*2.661)+3+3+(0.414*2.661);
				gptr1->tphl_fix = 6+(0.661*2.661)+7+3+(0.694*2.661);
				gptr1->tplh_load = 0.516;
				gptr1->tphl_load = 0.322;
				gptr1->area = 1458 + 1944 + 1458 + 1215;
			}
			else if (gptr1->input_count == 8) //NAND8: constructed by AND8 + INV
			{
				gptr1->tplh_fix = 7+(0.451*2.661)+3+3+(0.414*2.661);
				gptr1->tphl_fix = 7+(0.663*2.661)+7+3+(0.694*2.661);
				gptr1->tplh_load = 0.516;
				gptr1->tphl_load = 0.322;
				gptr1->area = 2430 + 2430 + 1458 + 1215;
			}
			else // The same as NAND8
			{
				gptr1->tplh_fix = 7+(0.451*2.661)+3+3+(0.414*2.661);
				gptr1->tphl_fix = 7+(0.663*2.661)+7+3+(0.694*2.661);
				gptr1->tplh_load = 0.516;
				gptr1->tphl_load = 0.322;
				gptr1->area = 2430 + 2430 + 1458 + 1215;
				cout << "ALARM: NAND gate has more than 4 inputs" << endl;
			}
			break;
		case 1:     //and
			if (gptr1->input_count == 2) //AND2: NAND2 + INV
			{
				gptr1->tplh_fix = 3+(0.706*2.661)+3;
				gptr1->tphl_fix = 5+(0.444*2.661)+3;
				gptr1->tplh_load = 0.516;
				gptr1->tphl_load = 0.322;
				gptr1->area = 2673;
			}
			else
				if (gptr1->input_count == 3) //AND3: NAND3 + INV
				{
					gptr1->tplh_fix = 6+(0.661*2.661)+3;
					gptr1->tphl_fix = 6+(0.420*2.661)+3;
					gptr1->tplh_load = 0.516;
					gptr1->tphl_load = 0.322;
					gptr1->area = 3159;
				}
				else
					if (gptr1->input_count == 4) //AND4: NAND4 + INV
					{
						gptr1->tplh_fix = 7+(0.663*2.661)+3;
						gptr1->tphl_fix = 7+(0.451*2.661)+3;
						gptr1->tplh_load = 0.516;
						gptr1->tphl_load = 0.322;
						gptr1->area = 3645;
					}
					else
						if (gptr1->input_count == 5) //AND5: NAND3 + NAND2 + NOR2
						{
							gptr1->tplh_fix = 6+(0.661*2.661)+7;
							gptr1->tphl_fix = 6+(0.420*2.661)+3;
							gptr1->tplh_load = 0.694;
							gptr1->tphl_load = 0.414;
							gptr1->area = 1458 + 1944 + 1458;
						}
						else
							if (gptr1->input_count == 6) //AND6: NAND3 + NAND3 + NOR2
							{
								gptr1->tplh_fix = 6+(0.661*2.661)+7;
								gptr1->tphl_fix = 6+(0.420*2.661)+3;
								gptr1->tplh_load = 0.694;
								gptr1->tphl_load = 0.414;
								gptr1->area = 1944 + 1944 + 1458;
							}
							else
								if (gptr1->input_count == 7) //AND7: NAND4 + NAND3 + NOR2
								{
									gptr1->tplh_fix = 7+(0.663*2.661)+7;
									gptr1->tphl_fix = 7+(0.451*2.661)+3;
									gptr1->tplh_load = 0.694;
									gptr1->tphl_load = 0.414;
									gptr1->area = 1944 + 2430 + 1458;
								}
								else
									if (gptr1->input_count == 8) //AND8: NAND4 + NAND4 + NOR2
									{
										gptr1->tplh_fix = 7+(0.663*2.661)+7;
										gptr1->tphl_fix = 7+(0.451*2.661)+3;
										gptr1->tplh_load = 0.694;
										gptr1->tphl_load = 0.414;
										gptr1->area = 2430 + 2430 + 1458;
									}
									else
										if (gptr1->input_count == 9) //AND9: NAND3 + NAND3 + NAND3 + NOR3
										{
											gptr1->tplh_fix = 6+(0.661*2.661)+9;
											gptr1->tphl_fix = 6+(0.420*2.661)+2;
											gptr1->tplh_load = 1.016;
											gptr1->tphl_load = 0.297;
											gptr1->area = 1944 + 1944 + 1944 + 1944;
										}
										else
										{//The same as AND9
											gptr1->tplh_fix = 6+(0.661*2.661)+9;
											gptr1->tphl_fix = 6+(0.420*2.661)+2;
											gptr1->tplh_load = 1.016;
											gptr1->tphl_load = 0.297;
											gptr1->area = 1944 + 1944 + 1944 + 1944;
											cout << "ALARM: AND gate has more than 8 inputs" << endl;
										}
										break;
		case 2:     //NOR
			if (gptr1->input_count == 2) //NOR2
			{
				gptr1->tplh_fix  = 7;		gptr1->tphl_fix = 3;
				gptr1->tplh_load = 0.694;	gptr1->tphl_load = 0.414;
				gptr1->area = 1458;
			}
			else if (gptr1->input_count == 3) //NOR3
			{
				gptr1->tplh_fix  = 9;		gptr1->tphl_fix = 2;
				gptr1->tplh_load = 1.016;	gptr1->tphl_load = 0.297;
				gptr1->area = 1944;
			}
			else if (gptr1->input_count == 4) //NOR4
			{
				gptr1->tplh_fix  = 17;		gptr1->tphl_fix = 2;
				gptr1->tplh_load = 1.292;	gptr1->tphl_load = 0.494;
				gptr1->area = 2430;
			}
			else if (gptr1->input_count == 8) //NOR8: Constructed by OR8 + INV
			{
				gptr1->tplh_fix = 17+(1.292*2.661)+3+3+(0.706*2.661);
				gptr1->tphl_fix = 2+(0.494*2.661)+5+3+(0.444*2.661);
				gptr1->tplh_load = 0.516;
				gptr1->tphl_load = 0.322;
				gptr1->area = 2430 + 2430 + 1458 + 1215;
			}
			else //The same as NOR8
			{
				gptr1->tplh_fix = 17+(1.292*2.661)+3+3+(0.706*2.661);
				gptr1->tphl_fix = 2+(0.494*2.661)+5+3+(0.444*2.661);
				gptr1->tplh_load = 0.516;
				gptr1->tphl_load = 0.322;
				gptr1->area = 2430 + 2430 + 1458 + 1215;
				cout << "ALARM: NOR gate has more than 8 inputs" << endl;
			}
			break;
		case 3:     //OR
			if (gptr1->input_count == 2) //OR2: NOR2+INV
			{
				gptr1->tplh_fix = 3+(0.414*2.661)+3;
				gptr1->tphl_fix = 7+(0.694*2.661)+3;
				gptr1->tplh_load = 0.516;
				gptr1->tphl_load = 0.322;
				gptr1->area = 2673;
			}
			else
				if (gptr1->input_count == 3) //OR3: NOR3+INV
				{
					gptr1->tplh_fix = 2+(0.297*2.661)+3;
					gptr1->tphl_fix = 9+(1.016*2.661)+3;
					gptr1->tplh_load = 0.516;
					gptr1->tphl_load = 0.322;
					gptr1->area = 3159;
				}
				else
					if (gptr1->input_count == 4) //OR4: NOR4+INV
					{
						gptr1->tplh_fix = 2+(0.494*2.661)+3;
						gptr1->tphl_fix = 17+(1.292*2.661)+3;
						gptr1->tplh_load = 0.516;
						gptr1->tphl_load = 0.322;
						gptr1->area = 3645;
					}
					else if (gptr1->input_count == 5) //OR5: NOR3+NOR2+NAND2
					{
						gptr1->tplh_fix = 2+(0.297*2.661)+5;
						gptr1->tphl_fix = 9+(1.016*2.661)+3;
						gptr1->tplh_load = 0.444;
						gptr1->tphl_load = 0.706;
						gptr1->area = 1458 + 1944 + 1458;
					}
					else if (gptr1->input_count == 6) //OR6: NOR3+NOR3+NAND2
					{
						gptr1->tplh_fix = 2+(0.297*2.661)+5;
						gptr1->tphl_fix = 9+(1.016*2.661)+3;
						gptr1->tplh_load = 0.444;
						gptr1->tphl_load = 0.706;
						gptr1->area = 1944 + 1944 + 1458;
					}
					else if (gptr1->input_count == 7) //OR7: NOR4+NOR3+NAND2
					{
						gptr1->tplh_fix = 2+(0.494*2.661)+5;
						gptr1->tphl_fix = 17+(1.292*2.661)+3;
						gptr1->tplh_load = 0.444;
						gptr1->tphl_load = 0.706;
						gptr1->area = 1944 + 2430 + 1458;
					}
					else if (gptr1->input_count == 8) //OR8: NOR4+NOR4+NAND2
					{
						gptr1->tplh_fix = 2+(0.494*2.661)+5;
						gptr1->tphl_fix = 17+(1.292*2.661)+3;
						gptr1->tplh_load = 0.444;
						gptr1->tphl_load = 0.706;
						gptr1->area = 2430 + 2430 + 1458;
					}
					else // The same as OR8
					{
						gptr1->tplh_fix = 2+(0.494*2.661)+5;
						gptr1->tphl_fix = 17+(1.292*2.661)+3;
						gptr1->tplh_load = 0.444;
						gptr1->tphl_load = 0.706;
						gptr1->area = 2430 + 2430 + 1458;
						cout << "ALARM: OR gate has more than 4 inputs" << endl;
					}
					break;
		case 4:     //XOR
			gptr1->tplh_fix  = 12;		gptr1->tphl_fix = 13;
			gptr1->tplh_load = 0.668;	gptr1->tphl_load = 0.266;
			gptr1->area = 3159;
			break;
		case 5:     //XNOR
			gptr1->tplh_fix  = 12;		gptr1->tphl_fix = 12;
			gptr1->tplh_load = 0.551;	gptr1->tphl_load = 0.105;
			gptr1->area = 2916;
			break;
		case 6:     //INV
			gptr1->tplh_fix  = 3;		gptr1->tphl_fix = 3;
			gptr1->tplh_load = 0.516;	gptr1->tphl_load = 0.322;
			gptr1->area = 1215;
			break;
		case 7:     //DFF: Should be corrected
			gptr1->tplh_fix  = 7;		gptr1->tphl_fix = 7;
			gptr1->tplh_load = 0.387;	gptr1->tphl_load = 0.432;
			gptr1->area = 8505;
			break;
		case 8:		//lut: should be corrected, delay must be calculated from pldm
			gptr1->tplh_fix  = 7;		gptr1->tphl_fix = 7;
			gptr1->tplh_load = 0.387;	gptr1->tphl_load = 0.432;
			gptr1->area = 8505;
			break;
			break;
		default:    // BUF
			gptr1->tplh_fix  = 7;		gptr1->tphl_fix = 7;
			gptr1->tplh_load = 0.370;	gptr1->tphl_load = 0.363;
			gptr1->area = 1458;
			break;
	}//switch
}//initialize_gate_delay
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_circuit_delay_from_library
// Purpose:  Computes the delays of all gates in the circuit
// compute_circuit_delay : linear delay model
// compute_circuit_delay_from_library : non-linear delay model
// Returns none
void compute_circuit_delay_from_library(library* lib){
	cout << "compute circuit delay from library." << endl ;
	struct gate *gptr1, *gptr2;
	for(int i=0;i<gate_numbers;i++) gate_list[i]->delay = 0;
	
	//compute output load
	//Initialize the gate delay and also compute the load capacitance
	for(int i=0;i<gate_numbers;i++){
		//cout << "-------------------------------------" << endl << i << " " << gate_name(gate_list[i]) << endl ;
		float outputload=0;
		gptr1= gate_list[i];
		if (gptr1->type==7) continue; //dff
		for(int j=0; j<fadjlist[gptr1->out].size(); j++){ //compute output load capacitance
			struct gate *gptr2 = fadjlist[gptr1->out][j];
			float cap = lib->getCapacitanceForCell(gate_name(gptr2), gptr2->input_count);
			outputload += cap ;
			//cout << "output cap= "<< cap << endl ;
		}
		gptr1->load = outputload;
		//cout << "output_load = " << outputload << endl ;
		//cout << " *****8 Timing!" << endl ;
		//non-linear delay model:
		timingData d = lib->getTimingForCell(gate_name(gptr1), gptr1->input_count, gptr1->load);
		gptr1->tplh = d.rise ;
		gptr1->tphl = d.fall ;
		//gptr1->tplh = gptr1->tplh_fix + (gptr1->tplh_load * gptr1->load);
		//gptr1->tphl = gptr1->tphl_fix + (gptr1->tphl_load * gptr1->load);
		if (gptr1->tplh > gptr1->tphl){
			gptr1->gd = gptr1->tplh;
			gptr1->gd_min = gptr1->tphl;
		}else{
			gptr1->gd = gptr1->tphl;
			gptr1->gd_min = gptr1->tplh;
		}
		//cout << "timing info = "<< gptr1->gate_no << " " << gate_name(gptr1) << " " << gptr1->tphl << " " << gptr1->tplh << endl ;

		// compute the maximum input delays
		float inputs_delay = 0;
        for(int j=0; j<gptr1->input_count; j++){
			if (gptr1->input_list[j] > numPi){
				gptr2 = fadjlist_ptr[gptr1->input_list[j]];
				if (gptr2->type==7) // if input is dff
					continue;
				if (gptr2->delay > inputs_delay)
					inputs_delay = gptr2->delay;
			}
 		}
		gptr1->delay = gptr1->gd + inputs_delay;
	}

	//compute delays for FFs
    for (int i=0; i<dff_num; i++){
        gptr1 = fadjlist_ptr[dfflist_ptr[i]->q];
		float outputload=0;
		for(int j=0; j<fadjlist[gptr1->out].size(); j++){ //compute output load capacitance
			gptr2 = fadjlist[gptr1->out][j];
			outputload += lib->getCapacitanceForCell(gate_name(gptr2), gptr2->input_count);
		}
		gptr1->load = outputload;
		//cout << "DFF->" << outputload << endl ;
		//fprintf(resultfp, "load dependent delay of gate %d = %f \n",gptr1->out, gptr1->load);
		//fprintf(resultfp, "%d outputload %f ", i, gptr1->load);
		
		//Bugfix:adel
		timingData d = lib->getTimingForCell(gate_name(gptr1), gptr1->input_count, gptr1->load);
		gptr1->tplh = d.rise ;
		gptr1->tphl = d.fall ;

		//gptr1->tplh = gptr1->tplh_fix + (gptr1->tplh_load * gptr1->load);
		//gptr1->tphl = gptr1->tphl_fix + (gptr1->tphl_load * gptr1->load);
		if (gptr1->tplh > gptr1->tphl){
			gptr1->gd = gptr1->tplh;
			gptr1->gd_min = gptr1->tphl;
		}else{
			gptr1->gd = gptr1->tphl;
			gptr1->gd_min = gptr1->tplh;
		}
		
		//cout << "timing info = "<< gptr1->gate_no << " " << gate_name(gptr1) << " " << gptr1->tphl << " " << gptr1->tplh << endl ;
		//fprintf(resultfp, "gd = %f, gd_min = %f \n ", gptr1->gd, gptr1->gd_min);
		// compute the maximum input delays
		float inputs_delay = 0;
        for (int j=0; j<gptr1->input_count; j++){
			if (gptr1->input_list[j] > numPi){
				gptr2 = fadjlist_ptr[gptr1->input_list[j]];
				if (gptr2->type==7) // if input is dff
					continue;
				if (gptr2->delay > inputs_delay)
					inputs_delay = gptr2->delay;
			}
 		} 
		//gate->delay means critial delay from input up-to this gate
		gptr1->delay = gptr1->gd + inputs_delay;
    }

	//fprintf(resultfp, "\n \n GATE DELAYS \n");
	//for (i=0;i<gate_numbers;i++)
	//{
	//	gptr1= gate_list[i];
	//	fprintf(resultfp, "%d\t:: %f \n", i, gptr1->delay);
	//}
		// max_delay is critical path delay from PI to PO.
		max_delay = 0;
		for (int i=0;i<gate_numbers;i++){
			gptr1 = gate_list[i];
			if ((gptr1->delay > max_delay)) // && (gptr1->type!=7))
				max_delay = gptr1->delay;
		}
	cout << "Finished Computing circuit delay" << endl ;
}// compute_circuit_delay_from_library
/////////////////////////////////////////////////////////////////////////////////////.


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_circuit_delay
// Purpose:  Computes the delays of all gates in the circuit
// if false: First, does initialize_gate_delay then computes max_delay
// Returns none
void compute_circuit_delay(bool slack_resizing){
	struct gate *gptr1, *gptr2;

	//Initialize gate delays
	if(slack_resizing==false){
		for(int i=0;i<gate_numbers;i++){
			gptr1= gate_list[i];
			initialize_gate_delay(gptr1); //Initialize gptr1 delay
		}
	}

	for(int i=0;i<gate_numbers;i++) gate_list[i]->delay = 0;

	//compute output load
	float outputload;
	//Initialize the gate delay and also compute the load capacitance
	for(int i=0;i<gate_numbers;i++){
		outputload=0;
		gptr1= gate_list[i];
		if (gptr1->type==7) continue; //dff
		for(int j=0; j<fadjlist[gptr1->out].size(); j++){ //compute output load capacitance
			gptr2 = fadjlist[gptr1->out][j];
			outputload += gptr2->inputc;
		}
		gptr1->load = outputload;
		//fprintf(resultfp, "load dependent delay of gate %d = %f \n",gptr1->out, gptr1->load);
		//fprintf(resultfp, "%d outputload %f ", i, gptr1->load);

		//BugFix:adel
		gptr1->tplh = gptr1->tplh_fix + (gptr1->tplh_load * gptr1->load);
		gptr1->tphl = gptr1->tphl_fix + (gptr1->tphl_load * gptr1->load);
		if (gptr1->tplh > gptr1->tphl){
			gptr1->gd = gptr1->tplh;
			gptr1->gd_min = gptr1->tphl;
		}else{
			gptr1->gd = gptr1->tphl;
			gptr1->gd_min = gptr1->tplh;
		}

		/* old code:
		float gd1, gd2;
		gd1 = gptr1->tplh_fix + (gptr1->tplh_load * gptr1->load);
		gd2 = gptr1->tphl_fix + (gptr1->tphl_load * gptr1->load);
		if (gd1 > gd2)
		{
			gptr1->gd = gd1;
			gptr1->gd_min = gd2;
		}
		else
		{
			gptr1->gd = gd2;
			gptr1->gd_min = gd1;
		}
		*/
		//fprintf(resultfp, "gd = %f, gd_min = %f \n ", gptr1->gd, gptr1->gd_min);
		// compute the maximum input delays
		float inputs_delay = 0;
        for(int j=0; j<gptr1->input_count; j++){
			if (gptr1->input_list[j] > numPi){
				gptr2 = fadjlist_ptr[gptr1->input_list[j]];
				if (gptr2->type==7) // if input is dff
					continue;
				if (gptr2->delay > inputs_delay)
					inputs_delay = gptr2->delay;
			}
 		}
		gptr1->delay = gptr1->gd + inputs_delay;
	}

	//compute delays for FFs
    for (int i=0; i<dff_num; i++){
        gptr1 = fadjlist_ptr[dfflist_ptr[i]->q];
		outputload=0;
		for(int j=0; j<fadjlist[gptr1->out].size(); j++){ //compute output load capacitance
			gptr2 = fadjlist[gptr1->out][j];
			outputload += gptr2->inputc;
		}
		gptr1->load = outputload;
		//fprintf(resultfp, "load dependent delay of gate %d = %f \n",gptr1->out, gptr1->load);
		//fprintf(resultfp, "%d outputload %f ", i, gptr1->load);
		
		//Bugfix:adel
		gptr1->tplh = gptr1->tplh_fix + (gptr1->tplh_load * gptr1->load);
		gptr1->tphl = gptr1->tphl_fix + (gptr1->tphl_load * gptr1->load);
		if (gptr1->tplh > gptr1->tphl){
			gptr1->gd = gptr1->tplh;
			gptr1->gd_min = gptr1->tphl;
		}else{
			gptr1->gd = gptr1->tphl;
			gptr1->gd_min = gptr1->tplh;
		}
		/* old code:
		float gd1, gd2;
		gd1 = gptr1->tplh_fix + (gptr1->tplh_load * gptr1->load);
		gd2 = gptr1->tphl_fix + (gptr1->tphl_load * gptr1->load);
		if (gd1 > gd2)
		{
			gptr1->gd = gd1;
			gptr1->gd_min = gd2;
		}
		else
		{
			gptr1->gd = gd2;
			gptr1->gd_min = gd1;
		}*/
		//fprintf(resultfp, "gd = %f, gd_min = %f \n ", gptr1->gd, gptr1->gd_min);
		// compute the maximum input delays
		float inputs_delay = 0;
        for (int j=0; j<gptr1->input_count; j++){
			if (gptr1->input_list[j] > numPi){
				gptr2 = fadjlist_ptr[gptr1->input_list[j]];
				if (gptr2->type==7) // if input is dff
					continue;
				if (gptr2->delay > inputs_delay)
					inputs_delay = gptr2->delay;
			}
 		}
		gptr1->delay = gptr1->gd + inputs_delay;
    }

	//fprintf(resultfp, "\n \n GATE DELAYS \n");
	//for (i=0;i<gate_numbers;i++)
	//{
	//	gptr1= gate_list[i];
	//	fprintf(resultfp, "%d\t:: %f \n", i, gptr1->delay);
	//}
	if (slack_resizing == false){
		max_delay = 0;
		for (int i=0;i<gate_numbers;i++){
			gptr1 = gate_list[i];
			if ((gptr1->delay > max_delay)) // && (gptr1->type!=7))
				max_delay = gptr1->delay;
		}
	}
}//compute_circuit_delay

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_slack
// Purpose:  Computes the slack of all gates in the circuit
// Returns none
void compute_slack(void){
	float minslack,fslack;
	int i,j;
	struct gate *gptr1, *gptr2;

	for (i=gate_numbers-1;i>=0;i--){
		gptr1= gate_list[i];
		if (gptr1->type==7)
			gptr1->slack = 0;
		else
		if (fadjlist[gptr1->out].size()==0)	//if the gate is output gate and it is not an input to another gate
			gptr1->slack = max_delay - gptr1->delay;
		else
		{
			minslack = max_delay - gptr1->delay;
			for (j=0; j< fadjlist[gptr1->out].size(); j++)
			{
				gptr2 = fadjlist[gptr1->out][j];
				if (gptr2->type==7) //flip-flop
				{
					fslack = max_delay - gptr1->delay;
					continue;
				}
				fslack = gptr2->delay - gptr2->gd - gptr1->delay + gptr2->slack;
				if (fslack < minslack)
					minslack = fslack;
			}
			if ((minslack<0.1) && (minslack>-0.1))
				minslack = 0;
			gptr1->slack = minslack;
		}
	}
}//compute_slack
//////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: print_slack_info
// Purpose:  print slack info
// Returns none
void print_slack_info(void){
	int i;//,j;
	struct gate *gptr1;//, *gptr2;

	//fprintf(resultfp, "\n \n GATE SLACK \n");
	float total_slacks=0;
	float cEPP=0, ncEPP=0;
	int slacked_gates=0;
	int epp_list10[10];
	for (i=0;i<10;i++)
		epp_list10[i]=0;

	for (i=0;i<gate_numbers;i++)
	{
		gptr1= gate_list[i];
		//fprintf(resultfp, "%d\t:: %f ::GD= %f, gate_type: %d, gate_area: %f\n", gptr1->out, gptr1->slack, gptr1->gd, gptr1->type, gptr1->area);
		if (gptr1->type==7) //If FF : Important: change != to ==
			continue;
		total_slacks += gptr1->slack;
		if (gptr1->slack > 0)
		{
			slacked_gates++;
			ncEPP += gptr1->epp;
		}
		else
		{
			cEPP += gptr1->epp;
		}
		int tindex = (int)floor(gptr1->epp*10);
		if (tindex==10) tindex=9;
//		cout<< "test " << gptr1->epp << " "<<tindex <<"  ";
		epp_list10[tindex]++;
	}
	ncEPP = ncEPP / slacked_gates;
	cEPP = cEPP / (gate_numbers_noff-slacked_gates);
/*	fprintf(resultfp, "Total gates: %d , slacked gates: %d,  critical path gates: %d\n", gate_numbers_noff, slacked_gates, gate_numbers_noff-slacked_gates);
	fprintf(resultfp, "slacked gates (percent): %f \n", (float)slacked_gates/(float)gate_numbers_noff);
	fprintf(resultfp, "Maximum Delay: %f \n", max_delay );
	fprintf(resultfp, "Average EPP for non-critical paths: %f \n", ncEPP);
	fprintf(resultfp, "Average EPP for critical paths: %f \n\n", cEPP);
*/
	fprintf(resultfp, "Normalized EPP Results \n");
	for (i=0;i<10;i++) //10 indicates 0~0.1 0.1~0.2   .... 0.9~1
		fprintf(resultfp, "%f \n", (float)epp_list10[i] / (float)gate_numbers_noff); // !(if FF)
		//fprintf(resultfp, "%f \n", (float)epp_list10[i] / (float)dff_num); // (if FF)

}//print_slack_info
//////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: resizing1
// Purpose:  resizes all non-critical gates according to thier slacks (Method 2 in ISCAS06 paper)
// Returns none
void resizing1(void)
{
	int i,j;
	struct gate *gptr1, *gptr2;
	//sort gates according to thier epp values
	float t_area; //total area


	//resize the gates according to epp_ranking
	for (i=0;i<gate_numbers;i++)
	{

		gptr1 = gate_list[epp_area_ranking[i]];
		//if ((gptr1->slack==0) || (gptr1->is_resized==true))
		if (gptr1->is_resized==true)
			continue;
		float old_inputc = gptr1->inputc;
		float old_area = gptr1->area;
		gptr1->inputc = gptr1->inputc * SCALE;
		gptr1->area = gptr1->area * AREA_SCALE;
		//fprintf(resultfp, "Old input c = %f,   New input c = %f", old_inputc, gptr1->inputc);
		compute_circuit_delay(true);
		compute_slack();
		bool resize_successful = true;

		t_area = 0;
		for (j=0;j<gate_numbers;j++)
		{
			gptr2 = gate_list[j];
			t_area += gptr2->area;
		}
		if (t_area > max_area)
			resize_successful = false;


		if (resize_successful==true)
		{
			//gptr1->area = gptr1->area * AREA_SCALE;
			gptr1->is_resized = true;
			//fprintf(resultfp, "\n \n Gate %d was resized SUCCESSFULLY. \n", gptr1->out);
			//print_slack_info();
		}
		else
		{
			gptr1->inputc = old_inputc;
			gptr1->area = old_area;
			compute_circuit_delay(true);
			compute_slack();
			//fprintf(resultfp, "\n \n UNSUCCESSFUL resizing for gate %d. \n", gptr1->out);
			//print_slack_info();
		}
	}
}//resizing1
//////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: resizing2
// Purpose:  resizes all non-critical gates according to thier slacks (Method 2 in ISCAS06 paper)
// Returns none
void resizing2(void)
{
	int i,j;
	struct gate *gptr1, *gptr2;
	//sort gates according to thier epp values



	//resize the gates according to epp_ranking
	for (i=0;i<gate_numbers;i++)
	{

		gptr1 = gate_list[epp_ranking[i]];
		if ((gptr1->is_resized==true)) //|| (gptr1->slack<=0)
			continue;
		float old_inputc = gptr1->inputc;
		gptr1->inputc = gptr1->inputc * SCALE;
		//fprintf(resultfp, "Old input c = %f,   New input c = %f", old_inputc, gptr1->inputc);
		compute_circuit_delay(true);
		compute_slack();
		bool resize_successful = true;
		for (j=0;j<gate_numbers;j++)
		{
			gptr2 = gate_list[j];
			if (gptr2->slack < 0)
			{
				resize_successful = false;
				break;
			}
		}
		if (resize_successful==true)
		{
			gptr1->area = gptr1->area * AREA_SCALE;
			gptr1->is_resized = true;
			//fprintf(resultfp, "\n \n Gate %d was resized SUCCESSFULLY. \n", gptr1->out);
			//print_slack_info();
		}
		else
		{
			gptr1->inputc = old_inputc;
			compute_circuit_delay(true);
			compute_slack();
			//fprintf(resultfp, "\n \n UNSUCCESSFUL resizing for gate %d. \n", gptr1->out);
			//print_slack_info();
		}
		//print_slack_info();
	}
}//resizing2
//////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: resizing3
// Purpose:  resizes all non-critical gates according to thier slacks (Method 3 in ISCAS06 paper)
// Returns none
void resizing3(void)
{
	int i,j;
	struct gate *gptr1, *gptr2;
	//sort gates according to thier epp values
	float t_area; //total area


	//resize the gates according to epp_ranking
	for (i=0;i<gate_numbers;i++)
	{

		gptr1 = gate_list[epp_area_ranking[i]];
		//if ((gptr1->slack<=0) || (gptr1->is_resized==true))
		if (gptr1->is_resized==true)
			continue;
		float old_inputc = gptr1->inputc;
		float old_area = gptr1->area;
		gptr1->inputc = gptr1->inputc * SCALE;
		gptr1->area = gptr1->area * AREA_SCALE;
		//fprintf(resultfp, "Old input c = %f,   New input c = %f", old_inputc, gptr1->inputc);
		compute_circuit_delay(true);
		compute_slack();
		bool resize_successful = true;

		t_area = 0;
		for (j=0;j<gate_numbers;j++)
		{
			gptr2 = gate_list[j];
			if (gptr2->slack < 0)
			{
				resize_successful = false;
				break;
			}
			t_area += gptr2->area;
		}
		if (t_area > max_area)
			resize_successful = false;


		if (resize_successful==true)
		{
			//gptr1->area = gptr1->area * AREA_SCALE;
			gptr1->is_resized = true;
			//fprintf(resultfp, "\n \n Gate %d was resized SUCCESSFULLY. \n", gptr1->out);
			//print_slack_info();
		}
		else
		{
			gptr1->inputc = old_inputc;
			gptr1->area = old_area;
			compute_circuit_delay(true);
			compute_slack();
			//fprintf(resultfp, "\n \n UNSUCCESSFUL resizing for gate %d. \n", gptr1->out);
			//print_slack_info();
		}
	}
}//resizing3
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_vul_reduction
// Purpose:  computes vulnerability reduction ratio
// Returns none
void compute_vul_reduction(void)
{
	int i,j;
	struct gate *gptr1, *gptr2;
	int temp;


	for (i=0;i<gate_numbers;i++)
	{
		gptr1= gate_list[i];
		initialize_gate_delay(gptr1); //Initialize gptr1 delay
	}
	compute_circuit_delay(false);
	//max_delay = 0;
	//for (i=0;i<gate_numbers;i++)
	//{
	//	if ((gate_list[i]->delay > max_delay) && (gate_list[i]->type!=7))
	//		max_delay = gate_list[i]->delay;
	//}
	compute_slack();
	//print_slack_info();

	//float circuit_area=0;
	circuit_area = 0;
	for (i=0;i<gate_numbers;i++)
	{
		//gate_list[i]->is_resized = false;
		epp_ranking[i] = i;
		epp_area_ranking[i] = i;
		circuit_area += gate_list[i]->area;
	}
	max_area = circuit_area;

	//sort epp_ranking based on EPP
	for (i=0;i<gate_numbers;i++)
	{
		for (j=i+1;j<gate_numbers;j++)
		{
			gptr1= gate_list[epp_ranking[i]];
            gptr2= gate_list[epp_ranking[j]];
			if (gptr1->epp < gptr2->epp)
			{
				temp = epp_ranking[i];
				epp_ranking[i] = epp_ranking[j];
				epp_ranking[j] = temp;
			}
		}
	}

	//sort epp_ranking based on EPP over Area
	for (i=0;i<gate_numbers;i++)
	{
		for (j=i+1;j<gate_numbers;j++)
		{
			gptr1= gate_list[epp_area_ranking[i]];
            gptr2= gate_list[epp_area_ranking[j]];
			if ((gptr1->epp/gptr1->area) < (gptr2->epp/gptr2->area))
			{
				temp = epp_area_ranking[i];
				epp_area_ranking[i] = epp_area_ranking[j];
				epp_area_ranking[j] = temp;
			}
		}
	}

	fprintf(resultfp, "EPP ranking \n");
	for (i=0;i<gate_numbers;i++)
		fprintf(resultfp, "%f :: ", gate_list[epp_ranking[i]]->epp);
	fprintf(resultfp, "\n\n");

	fprintf(resultfp, "\n///////////////////////////////////////////////////////////////////////////////////////// \n");
	fprintf(resultfp, "/////////////////////////////// DELAY-BASED resizing /////////////////////////////// \n");
	fprintf(resultfp, "///////////////////////////////////////////////////////////////////////////////////////// \n");

	float circuit_area_new = 0;
	float circuit_vul = 0;
	float circuit_vul_resized = 0;
	for (i=0; i<gate_numbers; i++)
	{
		gptr1 = gate_list[i];
		if (gptr1->is_dff==0) // gptr1 is not ff
			circuit_vul += gptr1->area * gptr1->epp;
		//fprintf(resultfp, "	gptr1->area = %f, gptr1->epp = %f \n", gptr1->area , gptr1->epp);
	}
	fprintf(resultfp, "\n METHOD2: Area before delay-based resizing = %f \n \n", circuit_area);

	for (i=0;i<gate_numbers;i++)
	{
		gate_list[i]->is_resized = false;
		if (gate_list[i]->is_dff==1)
			gate_list[i]->is_resized = true;
	}

	float original_max_delay = max_delay;
	int delay_overhead;
	for (delay_overhead=0; delay_overhead<=30; delay_overhead=delay_overhead+5)
	{
		//max_delay += original_max_delay * delay_overhead * 0.01;
		max_delay = original_max_delay + original_max_delay * delay_overhead * 0.01;
		fprintf(resultfp, "METHOD2: Vulnerability reduction results with %d%% delay overhead (max_delay=%f)\n", delay_overhead, max_delay);
		compute_slack();
		resizing2();
		//compute_circuit_delay(true); // ????????????????????????????????
		max_delay = 0;
		for (i=0;i<gate_numbers;i++)
		{
			gptr1 = gate_list[i];
			if ((gptr1->delay > max_delay)) // && (gptr1->type!=7))
				max_delay = gptr1->delay;
		}
		//compute_slack();
		circuit_area_new = 0;
		for (i=0;i<gate_numbers;i++)
			circuit_area_new += gate_list[i]->area;
		fprintf(resultfp, "\nMETHOD2: Area After resizing = %f \n", circuit_area_new);
		fprintf(resultfp, "METHOD2: increase in area = %f%% \n", (circuit_area_new-circuit_area)*100/circuit_area );

		fprintf(resultfp, "\nMETHOD2: Delay After resizing = %f \n", max_delay);
		fprintf(resultfp, "METHOD2: increase in delay = %f%% \n", (max_delay-original_max_delay)*100/original_max_delay);

		circuit_vul_resized = 0;
		for (i=0;i<gate_numbers;i++)
		{
			gptr1 = gate_list[i];
			if (gptr1->is_resized == false)
				circuit_vul_resized += gptr1->area * gptr1->epp;
		}
		fprintf(resultfp, "\nMETHOD2: Vulnerability before sizing = %f \n", circuit_vul);
		fprintf(resultfp, "\nMETHOD2: Vulnerability after sizing = %f \n", circuit_vul_resized);
		fprintf(resultfp, "METHOD2: Reduced vul = %f \n", (circuit_vul-circuit_vul_resized)*100/circuit_vul);
		fprintf(resultfp, "METHOD2: Reduction factor = %f \n", circuit_vul/circuit_vul_resized);
		fprintf(resultfp, "================================= \n \n");
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////AREA ////////////////////////////////////////////////

	fprintf(resultfp, "\n///////////////////////////////////////////////////////////////////////////////////////// \n");
	fprintf(resultfp, "/////////////////////////////// AREA-BASED resizing //////////////////////////////// \n");
	fprintf(resultfp, "///////////////////////////////////////////////////////////////////////////////////////// \n");
	for (i=0;i<gate_numbers;i++)
	{
		gptr1= gate_list[i];
		initialize_gate_delay(gptr1); //Initialize gptr1 delay
		//fprintf
	}
	compute_circuit_delay(false);
	compute_slack();
	//print_slack_info();
	/*circuit_vul = 0;
	for (i=0; i<gate_numbers; i++)
	{
		gptr1 = gate_list[i];
		circuit_vul += gptr1->area * gptr1->epp;
		//fprintf(resultfp, "	gptr1->area = %f, gptr1->epp = %f \n", gptr1->area , gptr1->epp);
	}*/

	fprintf(resultfp, "\nMETHOD1: Area before area-based resizing = %f \n \n", circuit_area);
	fprintf(resultfp, "\nMETHOD1: Delay before area-based resizing = %f \n \n", max_delay);

	for (i=0;i<gate_numbers;i++)
	{
		gate_list[i]->is_resized = false;
		if (gate_list[i]->is_dff==1)
			gate_list[i]->is_resized = true;
	}

	int area_overhead;
	float original_max_area = max_area;
	for (area_overhead=5; area_overhead<=45; area_overhead=area_overhead+10)
	{
		max_area = original_max_area + original_max_area * area_overhead * 0.01;
		fprintf(resultfp, "METHOD1:Vulnerability reduction results with %d%% area overhead (max area=%f)\n", area_overhead, max_area);
		//compute_slack();
		resizing1();
		compute_circuit_delay(true);
		max_delay = 0;
		for (i=0;i<gate_numbers;i++)
		{
			gptr1 = gate_list[i];
			if ((gptr1->delay > max_delay)) // && (gptr1->type!=7))
				max_delay = gptr1->delay;
		}
		compute_slack();
		circuit_area_new = 0;
		for (i=0;i<gate_numbers;i++)
			circuit_area_new += gate_list[i]->area;
		fprintf(resultfp, "\nMETHOD1: Area After resizing = %f \n", circuit_area_new);
		fprintf(resultfp, "METHOD1: increase in area = %f%% \n", (circuit_area_new-circuit_area)*100/circuit_area);

		fprintf(resultfp, "\nMETHOD1: Delay After resizing = %f \n", max_delay);
		fprintf(resultfp, "METHOD1: increase in delay = %f%% \n", (max_delay-original_max_delay)*100/original_max_delay);

		circuit_vul_resized = 0;
		for (i=0;i<gate_numbers;i++)
		{
			gptr1 = gate_list[i];
			if (gptr1->is_resized == false)
				circuit_vul_resized += gptr1->area * gptr1->epp;
		}
		fprintf(resultfp, "\nMETHOD1: Vulnerability before sizing = %f \n", circuit_vul);
		fprintf(resultfp, "\nMETHOD1: Vulnerability after sizing = %f \n", circuit_vul_resized);
		fprintf(resultfp, "METHOD1: Reduced vul = %f \n", (circuit_vul-circuit_vul_resized)*100/circuit_vul);
		fprintf(resultfp, "METHOD1: Reduction factor = %f \n", circuit_vul/circuit_vul_resized);
		fprintf(resultfp, "================================= \n \n");
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////AREA & DELAY ////////////////////////////////////////////////

	fprintf(resultfp, "\n///////////////////////////////////////////////////////////////////////////////////////// \n");
	fprintf(resultfp, "/////////////////////////////// AREA-DELAY-BASED resizing ///////////////////////////// \n");
	fprintf(resultfp, "///////////////////////////////////////////////////////////////////////////////////////// \n");

	for (i=0;i<gate_numbers;i++)
	{
		gptr1= gate_list[i];
		initialize_gate_delay(gptr1); //Initialize gptr1 delay
		//fprintf
	}
	compute_circuit_delay(false);
	compute_slack();
	//print_slack_info();
	/*circuit_vul = 0;
	for (i=0; i<gate_numbers; i++)
	{
		gptr1 = gate_list[i];    //??? FFs should be excluded
		circuit_vul += gptr1->area * gptr1->epp;
		//fprintf(resultfp, "	gptr1->area = %f, gptr1->epp = %f \n", gptr1->area , gptr1->epp);
	}*/
	fprintf(resultfp, "\nMETHOD3: Area before delay-area-based resizing = %f \n \n", circuit_area);
	fprintf(resultfp, "\nMETHOD3: Delay before delay-area-based resizing = %f \n \n", max_delay);

	for (i=0;i<gate_numbers;i++)
	{
		gate_list[i]->is_resized = false;
		if (gate_list[i]->is_dff==1)
			gate_list[i]->is_resized = true;
	}

	//float original_max_area = max_area;
	//original_max_delay = max_delay;

	//for (area_overhead=5; area_overhead<=20; area_overhead=area_overhead+5)
	for (delay_overhead=5; delay_overhead<=35; delay_overhead=delay_overhead+10)
	{

		for (i=0;i<gate_numbers;i++)
		{
			gptr1= gate_list[i];
			initialize_gate_delay(gptr1); //Initialize gptr1 delay
			//fprintf
		}
		compute_circuit_delay(false);
		compute_slack();
		/*circuit_vul = 0;
		for (i=0; i<gate_numbers; i++)
		{
			gptr1 = gate_list[i];    //??? FFs should be excluded
			circuit_vul += gptr1->area * gptr1->epp;
			//fprintf(resultfp, "	gptr1->area = %f, gptr1->epp = %f \n", gptr1->area , gptr1->epp);
		}*/
		fprintf(resultfp, "\nMETHOD3: Area before delay-area-based resizing = %f \n \n", circuit_area);
		fprintf(resultfp, "\nMETHOD3: Delay before delay-area-based resizing = %f \n \n", max_delay);

		for (i=0;i<gate_numbers;i++)
		{
			gate_list[i]->is_resized = false;
			if (gate_list[i]->is_dff==1)
				gate_list[i]->is_resized = true;
		}


		//max_area = original_max_area + original_max_area * area_overhead * 0.01;
		//for (delay_overhead=20; delay_overhead<=60; delay_overhead=delay_overhead+10)
		for (area_overhead=5; area_overhead<=45; area_overhead=area_overhead+10)
		{
			max_delay = original_max_delay + original_max_delay * delay_overhead * 0.01;
			max_area = original_max_area + original_max_area * area_overhead * 0.01;
			fprintf(resultfp, "METHOD3: Vulnerability reduction results with %d%% area overhead (max area=%f)\n", area_overhead, max_area);
			fprintf(resultfp, "METHOD3: Vulnerability reduction results with %d%% delay overhead (max_delay=%f)\n", delay_overhead, max_delay);
			compute_slack();
			resizing3();
			compute_circuit_delay(true); // ????????????????????????????????
			max_delay = 0;
			for (i=0;i<gate_numbers;i++)
			{
				gptr1 = gate_list[i];
				if ((gptr1->delay > max_delay)) // && (gptr1->type!=7))
					max_delay = gptr1->delay;
			}
			compute_slack();
			circuit_area_new = 0;
			for (i=0;i<gate_numbers;i++)
				circuit_area_new += gate_list[i]->area;
			fprintf(resultfp, "\nMETHOD3: Area After resizing = %f \n", circuit_area_new);
			fprintf(resultfp, "METHOD3: increase in area = %f%% \n", (circuit_area_new-circuit_area)*100/circuit_area);

			fprintf(resultfp, "\nMETHOD3: Delay After resizing = %f \n", max_delay);
			fprintf(resultfp, "METHOD3: increase in delay = %f%% \n", (max_delay-original_max_delay)*100/original_max_delay);

			circuit_vul_resized = 0;
			for (i=0;i<gate_numbers;i++)
			{
				gptr1 = gate_list[i];
				if (gptr1->is_resized == false)
					circuit_vul_resized += gptr1->area * gptr1->epp;
			}
			fprintf(resultfp, "\nMETHOD3: Vulnerability before sizing = %f \n", circuit_vul);
			fprintf(resultfp, "\nMETHOD3: Vulnerability after sizing = %f \n", circuit_vul_resized);
			fprintf(resultfp, "METHOD3: Reduced vul = %f \n", (circuit_vul-circuit_vul_resized)*100/circuit_vul);
			fprintf(resultfp, "METHOD3: Reduction factor = %f \n", circuit_vul/circuit_vul_resized);
			fprintf(resultfp, "================================= \n \n");
		}
	}


}//compute_vul_reduction
//////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////
// Function: system_failure
// Purpose:  Computes system failure probability at first clock
// Returns none
void system_failure(void)
{
    long i,j,k;
    short ff_index;
    short nff_index;
    short PI2POlist_size;
    struct gate *gateptr;
    struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
    short pred_gate_number;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];

    float p0_var_values[MAX_GATE_INPUTS], p1_var_values[MAX_GATE_INPUTS];
    float pd_var_values[MAX_GATE_INPUTS], pdn_var_values[MAX_GATE_INPUTS];

	int out_iter=0;
	float max_d_var=0, max_dn_var=0, ave_d_var=0, ave_dn_var=0;
	float max_error_var=0, ave_error_var=0;

    for (ff_index=0; ff_index<gate_numbers; ff_index++)
    {
        nff_index = gate_list[ff_index]->out;
		/*if (gate_list[ff_index]->is_dff != 1) // !(if FF)
			continue;
		*/
		if ((gate_list[ff_index]->is_dff == 1) || (nff_index <= (numPi+numPo))) // if FF or if PO
		//if (nff_index <= (numPi+numPo)) //multicycle
		{
			S[ff_index] = 1;
			gate_list[ff_index]->epp = 1;
			gate_list[ff_index]->epp_var = 0;
			continue;
		}

        // To get PI2POlist starting from flip-flop nff_index excluding it
        find_TPsort_from_dffi(nff_index);
        PI2POlist_size = PI2POlist.size();
        // Set MAX values to ensure each will get correct value
        for (i=0; i<gate_numbers; i++)
        {
            gateptr= gate_list[i];
            gateptr->p0 = 0; gateptr->p1 = 0;
            gateptr->pd = 0; gateptr->pdn = 0;
			gateptr->is_onpath = false;
			gateptr->p0_var = gateptr->total_variance;
			gateptr->p1_var = gateptr->total_variance;
			gateptr->pd_var = 0; gateptr->pdn_var = 0;
        }

	    for (i=0; i<PI2POlist_size; i++)
	    {
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
            gateptr->p0 = config->MAX;
            gateptr->p1 = config->MAX;
            gateptr->pd = config->MAX;
            gateptr->pdn = config->MAX;
			gateptr->is_onpath = true;
			gateptr->site_out_level = 0;
		}
		gateptr = gate_list[ff_index];
		gateptr->pd = 1;gateptr->pdn = 0;gateptr->p1 = 0;gateptr->p0 = 0;

		gateptr->pd_var = gateptr->pdn_var = gateptr->p0_var = gateptr->p1_var = 0;
        //
		//fprintf(resultfp, "NODE %d/////////////////////////////////////////\n", nff_index);
		short site_level;
	    for (i=0; i<PI2POlist_size; i++)
	    {
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
			//gateptr->site_out_level++;
			if (gateptr->out==nff_index) //skip the faulty gate
				continue;
			site_level = 0;
            for (j=0; j< (gateptr->input_count); j++)
            {
                // if input is Primary Input
                if ((gateptr->input_list[j]) <= numPi)
                {
//                    p0_values[j] = 0.5; p1_values[j] = 0.5;
                    p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
                    p1_values[j] = PI_list_sp[gateptr->input_list[j]];
                    pd_values[j] = 0; pdn_values[j] = 0;

					p0_var_values[j] = p1_var_values[j] = 0;
					pd_var_values[j] = pdn_var_values[j] = 0;
                    continue;
                }
                // if input is the faulty gate
                if ((gateptr->input_list[j]) == nff_index)
                {
                    p0_values[j] = 0; p1_values[j] = 0;
                    pd_values[j] = 1; pdn_values[j] = 0;

					p0_var_values[j] = p1_var_values[j] = 0;
					pd_var_values[j] = pdn_var_values[j] = 0;
                    continue;
                }
                //VERY IMPORTANT: if (pred_gateptr->type == 7) ==> use_sp = true;  //for FFs
                pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
				if ((pred_gateptr->is_onpath == true) && (pred_gateptr->site_out_level > site_level))
					site_level = pred_gateptr->site_out_level;
                pred_gate_number = pred_gateptr->out;
                //short m,n;
                //int is_found =0;
				bool use_sp=false;
				if (pred_gateptr->is_onpath==false)
					use_sp = true;
				if (pred_gateptr->is_onpath==true)
				{
					if (pred_gate_number<=(numPi+numPo))
						use_sp = true;
					for (k=0; k<fadjlist[pred_gate_number].size(); k++)
						if (fadjlist[pred_gate_number][k]->type==7) //is DFF
							use_sp = true;
				}
				if (use_sp==true)
                {
					p0_values[j] = 1- pred_gateptr->signal_probability;
                    p1_values[j] = pred_gateptr->signal_probability;
                    pd_values[j] = 0;
                    pdn_values[j] = 0;

					p0_var_values[j] = pred_gateptr->total_variance;
					p1_var_values[j] = pred_gateptr->total_variance;
					pd_var_values[j] = pdn_var_values[j] = 0;

                }
				else //if (pred_gateptr->is_onpath==true)
				{
					p0_values[j] = pred_gateptr->p0;
                    p1_values[j] = pred_gateptr->p1;
                    pd_values[j] = pred_gateptr->pd;
                    pdn_values[j] = pred_gateptr->pdn;

					p0_var_values[j] = pred_gateptr->p0_var;
					p1_var_values[j] = pred_gateptr->p1_var;
					pd_var_values[j] = pred_gateptr->pd_var;
					pdn_var_values[j] = pred_gateptr->pdn_var;
				}
            }
			gateptr->site_out_level = site_level + 1;
            compute_D_Probability_with_variance(gateptr, p0_values, p1_values, pd_values, pdn_values, p0_var_values, p1_var_values, pd_var_values, pdn_var_values); //system_failure
			//fprintf(resultfp, "vars of node %4d\t, level = %2d\t, gate_type = %d, input_count = %2d: \t", gateptr->out, gateptr->site_out_level, gateptr->type, gateptr->input_count);
			//fprintf(resultfp, "%f, %f, %f, %f ::::::::", gateptr->p0_var, gateptr->p1_var, gateptr->pd_var, gateptr->pdn_var);
			//for (j=0; j< (gateptr->input_count); j++)
			//	fprintf(resultfp, "input %2d, vars = %f, %f, %f, %f ::", j+1, p0_var_values[j], p1_var_values[j], pd_var_values[j], pdn_var_values[j]);
			//fprintf(resultfp, "\n");

	    }// End of PI2POlist
        int ff_index2;
        for (ff_index2=0; ff_index2<dff_num; ff_index2++)
        {
            gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
            gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
            gptr2->p0 = gptr1->p0;	gptr2->p0_var = gptr1->p0_var;
            gptr2->p1 = gptr1->p1;	gptr2->p1_var = gptr1->p1_var;
            gptr2->pd = gptr1->pd;	gptr2->pd_var = gptr1->pd_var;
            gptr2->pdn= gptr1->pdn;	gptr2->pdn_var = gptr1->pdn_var;
        }
        // Filling M Matrix
        /*for (ff_index2=0; ff_index2<dff_num; ff_index2++)
        {
            gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
            M[ff_index][ff_index2] = 0.5 * (gptr1->pd +gptr1->pdn);
            if (ff_index2==ff_index)
                M[ff_index][ff_index2] = 1 - gptr1->pdn;
        }*/
        // Computing System Failure and Filling S Vector
		float error_var;
        float sys_failure=1;
		float sys_var=0,tmp_var;

        for (i=(numPi+1); i<=(numPi+numPo); i++) //if PO
        {
            gptr1 = fadjlist_ptr[i];
            sys_failure *= 1-(gptr1->pd+gptr1->pdn);

			ave_d_var += gptr1->pd_var;
			ave_dn_var += gptr1->pdn_var;
			error_var = sqrt(gptr1->pd_var*gptr1->pd_var + gptr1->pdn_var*gptr1->pd_var);
			ave_error_var += error_var;
			if (gptr1->pd_var > max_d_var)		max_d_var = gptr1->pd_var;
			if (gptr1->pdn_var > max_dn_var)	max_dn_var = gptr1->pdn_var;
			if (error_var > max_error_var)		max_error_var = error_var;
			out_iter++;
			//tmp_var = sqrt(gptr1->pd_var*gptr1->pd_var + gptr1->pdn_var*gptr1->pdn_var);
			//sys_var += (tmp_var/(1-(gptr1->pd+gptr1->pdn)))*(tmp_var/(1-(gptr1->pd+gptr1->pdn)));
		}

		for (i=0; i<dff_num; i++) // if FF
		{
			gptr1 = fadjlist_ptr[dfflist_ptr[i]->q];
			if ((dfflist_ptr[i]->q) > (numPi+numPo))
			{
				sys_failure *= 1-(gptr1->pd+gptr1->pdn);

				ave_d_var += gptr1->pd_var;
				ave_dn_var += gptr1->pdn_var;
				error_var = sqrt(gptr1->pd_var*gptr1->pd_var + gptr1->pdn_var*gptr1->pd_var);
				ave_error_var += error_var;
				if (gptr1->pd_var > max_d_var)		max_d_var = gptr1->pd_var;
				if (gptr1->pdn_var > max_dn_var)	max_dn_var = gptr1->pdn_var;
				if (error_var > max_error_var)		max_error_var = error_var;
				out_iter++;

				//tmp_var = sqrt(gptr1->pd_var*gptr1->pd_var + gptr1->pdn_var*gptr1->pdn_var);
				//sys_var += (tmp_var/(1-(gptr1->pd+gptr1->pdn)))*(tmp_var/(1-(gptr1->pd+gptr1->pdn)));
			}
		}

		if (sys_failure==0)
			sys_var = 0;
		else
		{
	        for (i=(numPi+1); i<=(numPi+numPo); i++)
		    {
			    gptr1 = fadjlist_ptr[i];
				//sys_failure *= 1-(gptr1->pd+gptr1->pdn);
				tmp_var = sqrt(gptr1->pd_var*gptr1->pd_var + gptr1->pdn_var*gptr1->pdn_var);
				sys_var += sys_failure * (tmp_var/(1-(gptr1->pd+gptr1->pdn))) * sys_failure * (tmp_var/(1-(gptr1->pd+gptr1->pdn)));
			}


			for (i=0; i<dff_num; i++) // if FF
			{
				gptr1 = fadjlist_ptr[dfflist_ptr[i]->q];
				if ((dfflist_ptr[i]->q) > (numPi+numPo))
				{
					//sys_failure *= 1-(gptr1->pd+gptr1->pdn);
					tmp_var = sqrt(gptr1->pd_var*gptr1->pd_var + gptr1->pdn_var*gptr1->pdn_var);
					sys_var += sys_failure * (tmp_var/(1-(gptr1->pd+gptr1->pdn))) * sys_failure * (tmp_var/(1-(gptr1->pd+gptr1->pdn)));
				}
			}

		}
		sys_var = sqrt(sys_var);
        sys_failure = 1 - sys_failure;
		gateptr = gate_list[ff_index]; //ff_index is gate_list index
		gateptr->epp = sys_failure;
		gateptr->epp_var = sys_var;
        S[ff_index] = sys_failure;
    }
    // Prints all the results M & S
    /*cout << "MATRIX M" << endl;
    fprintf(resultfp, "MATRIX M\n");
    for (i=0; i<dff_num; i++)
    {
        cout << "Row " << i << ": ";
        fprintf(resultfp, "Row %d: ", i);
        for (j=0; j<dff_num; j++)
        {
            cout << M[i][j] << " ";
            fprintf(resultfp, "%f    ", M[i][j]);
        }
        cout << endl;
        fprintf(resultfp,"\n");
    }*/

	ave_d_var = ave_d_var / (float)out_iter;
	ave_dn_var = ave_dn_var / (float)out_iter;
	ave_error_var = ave_error_var / (float)out_iter;

	short not_ff_po=0;
	float max_epp_var = 0, ave_epp_var=0;
    //cout << "VECTOR S" << endl;
    fprintf(resultfp, "VECTOR S\n");
    float overall_epp = 0;
    for (i=0; i<gate_numbers; i++)
    {
		gateptr = gate_list[i];
		if (gateptr->type==7) continue; //!(if FF)
        //cout << "node " << gateptr->out << ": EPP=" << gateptr->epp <<", Variance = " << gateptr->epp_var << endl;
        fprintf(resultfp, "node %4d\t: Epp= %f, gate_type=%d Variance = %f \n", gateptr->out, gateptr->epp, gateptr->type, gateptr->epp_var);
		if (gateptr->epp_var > max_epp_var) 	max_epp_var = gateptr->epp_var;
		ave_epp_var += gateptr->epp_var;
		if ((gateptr->is_dff == 0) && ( gateptr->out > (numPi+numPo))) // !(if FF) && !(if PO)
		{
			not_ff_po++;
			overall_epp += gateptr->epp;
		}
    }
	ave_epp_var = ave_epp_var / (float)not_ff_po;  // !(if FF)
	//ave_epp_var = ave_epp_var / (float)dff_num;	 // (if FF)

	overall_epp = overall_epp / (float)not_ff_po;

	fprintf(resultfp,"\n /////////////////////////////////////////////////////////////////\n");
	fprintf(resultfp," Results Summary on Variances : \n");
	fprintf(resultfp,"\n Overall EPP = %f \n", overall_epp);
	fprintf(resultfp," ave_d_var = %f \n", ave_d_var);
	fprintf(resultfp," ave_dn_var = %f \n", ave_dn_var);
	fprintf(resultfp," ave_error_var = %f \n", ave_error_var);
	fprintf(resultfp," max_d_var = %f \n", max_d_var);
	fprintf(resultfp," max_dn_var = %f \n", max_dn_var);
	fprintf(resultfp," max_error_var = %f \n", max_error_var);
	fprintf(resultfp," ave_epp_var = %f \n", ave_epp_var);
	fprintf(resultfp," max_epp_var = %f \n", max_epp_var);
	fprintf(resultfp,"\n ///////////////////////////////////////////////////////////////// \n");

}//system_failure

//////////////////////////////////////////////////////////////////////////////////////
// Function: system_failure_clockC
// Purpose:  Computes system failure probability at clock C
// Returns none
void system_failure_clockC(short clkC)
{

    long i,j;//,k,m;
    short ff_index;
    short nff_index;
    short PI2POlist_size;
    struct gate *gateptr;
    struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
    short pred_gate_number;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
    float p0_var_values[MAX_GATE_INPUTS], p1_var_values[MAX_GATE_INPUTS];
    float pd_var_values[MAX_GATE_INPUTS], pdn_var_values[MAX_GATE_INPUTS];

	for (ff_index=0; ff_index<gate_numbers; ff_index++)
	{
        nff_index = gate_list[ff_index]->out;
		cout << "gate:" << ff_index << endl;
        // To get PI2POlist starting from flip-flop nff_index excluding it
        find_TPsort_from_dffi(nff_index);
        PI2POlist_size = PI2POlist.size();
        // Set MAX values to ensure each will get correct value
        for (i=0; i<gate_numbers; i++)
        {
            gateptr= gate_list[i];
            gateptr->p0 = 0;
            gateptr->p1 = 0;
            gateptr->pd = 0;
            gateptr->pdn = 0;
			gateptr->is_onpath = false;
        }
	    for (i=0; i<PI2POlist_size; i++)
	    {
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
            gateptr->p0 = config->MAX;
            gateptr->p1 = config->MAX;
            gateptr->pd = config->MAX;
            gateptr->pdn = config->MAX;
			gateptr->is_onpath = true;
		}
		gateptr = gate_list[ff_index];
		gateptr->pd = 1;gateptr->pdn = 0;gateptr->p1 = 0;gateptr->p0 = 0;
	    for (i=0; i<PI2POlist_size; i++)
	    {
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
			if (gateptr->out==nff_index) //skip the faulty gate
				continue;
            for (j=0; j< (gateptr->input_count); j++)
            {
                // if input is Primary Input
                if ((gateptr->input_list[j]) <= numPi)
                {
                    p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
                    p1_values[j] = PI_list_sp[gateptr->input_list[j]];
                    pd_values[j] = 0; pdn_values[j] = 0;
                    continue;
                }
                // if input is the faulty gate
                if ((gateptr->input_list[j]) == nff_index)
                {
                    p0_values[j] = 0; p1_values[j] = 0;
                    pd_values[j] = 1; pdn_values[j] = 0;
                    continue;
                }
                pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
                pred_gate_number = pred_gateptr->out;
                //short m,n;
                //int is_found =0;
				bool use_sp=false;
				if (pred_gateptr->is_onpath==false)
					use_sp = true;

				/*if (pred_gateptr->is_onpath==true)
				{
					if (pred_gate_number<=(numPi+numPo))
						use_sp = true;
					for (k=0; k<fadjlist[pred_gate_number].size(); k++)
						if ((fadjlist[pred_gate_number][k]->type==7) && (pred_gate_number!=nff_index)) //is DFF
							use_sp = true;
				}*/

				if (use_sp==true)
                {
					p0_values[j] = 1- pred_gateptr->signal_probability;
                    p1_values[j] = pred_gateptr->signal_probability;
                    pd_values[j] = 0;
                    pdn_values[j] = 0;
                }
				else //if (pred_gateptr->is_onpath==true)
				{
					p0_values[j] = pred_gateptr->p0;
                    p1_values[j] = pred_gateptr->p1;
                    pd_values[j] = pred_gateptr->pd;
                    pdn_values[j] = pred_gateptr->pdn;
				}
            }
            compute_D_Probability_with_variance(gateptr, p0_values, p1_values, pd_values, pdn_values, p0_var_values, p1_var_values, pd_var_values, pdn_var_values); //system_failure
	    }// End of PI2POlist
        int ff_index2;
        for (ff_index2=0; ff_index2<dff_num; ff_index2++)
        {
            gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
            gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
            gptr2->p0 = gptr1->p0;
            gptr2->p1 = gptr1->p1;
            gptr2->pd = gptr1->pd;
            gptr2->pdn = gptr1->pdn;
        }
        // Filling M Matrix
		//int ff_number=ff_index;
        /*for (ff_index2=0; ff_index2<dff_num; ff_index2++)
		{
			if ((dfflist_ptr[ff_index2]->q) == (gate_list[ff_index]->out))
			{
				ff_number = ff_index2;
				break;
			}
		}*/
		int gate_index;
        for (gate_index=0; gate_index<gate_numbers; gate_index++)
        {
            gptr2 = gate_list[gate_index];
			if (gptr2->po_no>=0)
				SGO[ff_index][gptr2->po_no] = gptr2->pd + gptr2->pdn;
			if (gptr2->dff_no>=0)
				SGF[ff_index][gptr2->dff_no] = gptr2->pd + gptr2->pdn;
		}
		gptr1 = gate_list[ff_index];
		if (gptr1->is_dff)
		{
	        for (gate_index=0; gate_index<gate_numbers; gate_index++)
    	    {
        	    gptr2 = gate_list[gate_index];
				if (gptr2->po_no>=0)
					SFO[gptr1->dff_no][gptr2->po_no] = gptr2->pd + gptr2->pdn;
				if (gptr2->dff_no>=0)
					SFF[gptr1->dff_no][gptr2->dff_no] = (gptr2->pd + gptr2->pdn);
			}

		}
		//cout << "inja7";
        //for (ff_index2=0; ff_index2<dff_num; ff_index2++)
        //{
            //gptr1 = gate_list[ff_index2];//fadjlist_ptr[dfflist_ptr[ff_index2]->q]; //may be wrong
			//fprintf(resultfp, "ff_index = %d, ff_index2 = %d, g->pd = %f, g->pdn = %f \n", ff_index, ff_index2, gptr1->pd, gptr1->pdn);
            //SFF[ff_index][ff_index2] = 0.5 * (gptr1->pd + gptr1->pdn);
            //if (ff_index2==ff_index)
                //SFF[ff_index][ff_index2] = 1 - 0.5 * gptr1->pdn;
			//fprintf(resultfp, "M[][]=%f::",M[ff_index][ff_index2]);
        //}

        // Computing System Failure and Filling S Vector
        //float sys_failure=1;
        //for (i=(numPi+1); i<=(numPi+numPo); i++)
        //{
            //gptr1 = fadjlist_ptr[i];
			//fprintf(resultfp, "output->pd = %f, output->pdn = %f \n",gptr1->pd, gptr1->pdn);
            //sys_failure *= 1-(gptr1->pd+gptr1->pdn);

        //}
        //sys_failure = 1 - sys_failure;
		//gateptr = gate_list[ff_index]; //ff_index is gate_list index
		//gateptr->epp = sys_failure;
        //S[ff_index] = sys_failure;
		//S_FF_ck[ff_index][1] = sys_failure;
		//fprintf(resultfp, "sys_failure =%f::",sys_failure);

    }//outmost FOR

 ///////////////////////////////////////////////////////////////
	/*fprintf(resultfp,"\n\nMATRIX SFF\n");
	for (i=0; i<dff_num; i++)
	{
		for (j=0; j<dff_num; j++)
			fprintf(resultfp, "%f::", SFF[i][j]);
		fprintf(resultfp,"\n");
	}*/
	cout << "MATRIX SGO (SatCK=1)" << endl;
	fprintf(resultfp,"\n\nMATRIX SGO (SatCK=1)\n");
	for (i=0; i<gate_numbers; i++)
	{
		gateptr = gate_list[i];
		SatCK[i][0] = 1;
		for (j=0; j<numPo; j++)
			SatCK[i][0] *= 1 - SGO[i][j];
			//SatCK[i][0] *= 1 - (po_list[j]->pd + po_list[j]->pdn);
		SatCK[i][0] = 1 - SatCK[i][0];
		fprintf(resultfp, "node %4d: SatCK[i][0]=%9f::==>", gateptr->out, SatCK[i][0]);
		for (j=0; j<numPo; j++)
			fprintf(resultfp, "%f::", SGO[i][j]);
		fprintf(resultfp,"\n");
	}

	cout << "MATRIX SFO" << endl;
	fprintf(resultfp,"\n\nMATRIX SFO\n");
	for (i=0; i<dff_num; i++)
	{
		fprintf(resultfp, "FF node %4d: ", dfflist_ptr[i]->q);
		for (j=0; j<numPo; j++)
			fprintf(resultfp, "%f::", SFO[i][j]);
		fprintf(resultfp,"\n");
	}
	fprintf(resultfp,"\n");

	cout << "MATRIX SFF" << endl;
	fprintf(resultfp,"\nMATRIX SFF\n");
	for (i=0; i<dff_num; i++)
	{
		fprintf(resultfp, "FF node %4d: ", dfflist_ptr[i]->q);
		for (j=0; j<dff_num; j++)
			fprintf(resultfp, "%f::", SFF[i][j]);
		fprintf(resultfp,"\n");
	}
	fprintf(resultfp,"\n");

    //float Mtemp[MAX_DFF][MAX_DFF];
    //float MC[MAX_DFF][MAX_DFF];
	//float** Mtemp = NULL;
	//float** MC = NULL;
	Mtemp = new (nothrow) float*[dff_num];
	MC = new (nothrow) float*[dff_num];
	check_print_release_exit(Mtemp==NULL, "Couldn't allocate memory for Mtemp!!");
	check_print_release_exit(MC==NULL, "Couldn't allocate memory for MC!!");

	for (i=0; i<dff_num; i++) {
		Mtemp[i] = NULL;
		MC[i] = NULL;
	}
	for (i=0; i<dff_num; i++) {
		Mtemp[i] = new (nothrow) float[dff_num];
		MC[i] = new (nothrow) float[dff_num];
		check_print_release_exit(Mtemp[i]==NULL, "Couldn't allocate memory for Mtemp[i]!!");
		check_print_release_exit(MC[i]==NULL, "Couldn't allocate memory for MC[i]!!");
	}

	int clk_index;
	for (i=0; i<dff_num; i++)
	{
		for (j=0; j<dff_num; j++)
			MC[i][j] = 0;
		MC[i][i] = 1;
	}

	cout << "MATRIX SGOatCK" << endl;

	//float SGOatCK[MAX][MAX_PO];
	//float tmp_mat[MAX][MAX_DFF];
	//float** SGOatCK = NULL;
	//float** tmp_mat = NULL;
	SGOatCK = new (nothrow) float*[gate_numbers];
	tmp_mat = new (nothrow) float*[gate_numbers];
	check_print_release_exit(SGOatCK==NULL, "Couldn't allocate memory for SGOatCK!!");
	check_print_release_exit(tmp_mat==NULL, "Couldn't allocate memory for SGOatCK!!");

	for (i=0; i<gate_numbers; i++) {
		SGOatCK[i] = NULL;
		tmp_mat[i] = NULL;
	}
	for (i=0; i<gate_numbers; i++) {
		SGOatCK[i] = new (nothrow) float[numPo];
		tmp_mat[i] = new (nothrow) float[dff_num];
		check_print_release_exit(SGOatCK[i]==NULL, "Couldn't allocate memory for SGOatCK[i]!!");
		check_print_release_exit(tmp_mat[i]==NULL, "Couldn't allocate memory for tmp_mat[i]!!");
	}

	for (clk_index=2; clk_index<=clkC; clk_index++)
	{
		matrix_mult(tmp_mat,gate_numbers,dff_num, dff_num, SGF,MC);
		matrix_mult(SGOatCK,gate_numbers,numPo, dff_num, tmp_mat, SFO);

		//computing SFF^(clk_index)
		matrix_copy(Mtemp,MC,dff_num,dff_num);
		matrix_mult(MC,dff_num,dff_num,dff_num, Mtemp,SFF);

		//fprintf(resultfp, "\nSGOatCK=%d\n",clk_index);

		for (i=0; i<gate_numbers; i++)
		{
			gateptr = gate_list[i];
			SatCK[i][clk_index-1] = 1;
			for (j=0; j<numPo; j++)
				SatCK[i][clk_index-1] *= 1 - SGOatCK[i][j];
			SatCK[i][clk_index-1] = 1 - SatCK[i][clk_index-1];
			//fprintf(resultfp, "node %4d==> SatCK[i][clk_index-1]=%f \n", gateptr->out,SatCK[i][clk_index-1]);
		}
	}

	float mul_fact, prev_sum;
	fprintf(resultfp, "Analytical System Failure Results: Cycles 1 through n\n");
	fprintf(resultfp, "node num ::");
	for (clk_index=0; clk_index<clkC; clk_index++)
		//if (clk_index%5==0)
		fprintf(resultfp, " clk=%2d ::", clk_index+1);
	fprintf(resultfp, "\n");

	for (i=0; i<gate_numbers; i++)
	{
		gateptr = gate_list[i];
		fprintf(resultfp, "node %4d::",gateptr->out);
		prev_sum = 0;
		mul_fact = 1;
		for (clk_index=0; clk_index<clkC; clk_index++)
		{
			S1_to_CK[i][clk_index] = SatCK[i][clk_index]*mul_fact + prev_sum;
			mul_fact *= 1 - SatCK[i][clk_index];
			prev_sum = S1_to_CK[i][clk_index];
			//if (clk_index%5==0)
			fprintf(resultfp, "%f::", S1_to_CK[i][clk_index]);
		}
		fprintf(resultfp,"\n");
		Sck[i] = S1_to_CK[i][clkC-1];
	}
	fprintf(resultfp,"\n");



	try {
		cout << "deleting Mtemp ... ";
		for (i=0;i<dff_num; i++)
			delete[] Mtemp[i];
		delete[] Mtemp;
	}//try
	catch (...) {
		cout << "Couldn't delete all Mtemp!" << endl;
	}

	try {
		cout << "deleting MC ... ";
		for (i=0;i<dff_num; i++)
			delete[] MC[i];
		delete[] MC;
	}//try
	catch (...) {
		cout << "Couldn't delete all MC!" << endl;
	}


	try {
		cout << "deleting SGOatCK ... ";
		for (i=0;i<gate_numbers; i++)
			delete[] SGOatCK[i];
		delete[] SGOatCK;
	}//try
	catch (...) {
		cout << "Couldn't delete all SGOatCK!" << endl;
	}


	try {
		cout << "deleting tmp_mat ... ";
		for (i=0;i<gate_numbers; i++)
			delete[] tmp_mat[i];
		delete[] tmp_mat;
	}//try
	catch (...) {
		cout << "Couldn't delete all tmp_mat!" << endl;
	}

	//================

	fprintf(resultfp, "Check if  FFs are in the first part of gate_list \n");
	for (i=0; i<dff_num; i++)
	{
		if (gate_list[i]->type!=7)
		{
			fprintf(resultfp, "gate %d = type %d \n", i, gate_list[i]->type);
			fprintf(resultfp, "ALARM!");
		}
	}


}//system_failure_clockC
/////////////////////////////////////////////////////////////////////////////////////


void old_timing_derating_code(short clkC){

	//tperiod = t_crit_path + tsu + th + tffd;

	long i,j,k;
	short ff_index;
	short nff_index;
	short PI2POlist_size;
	struct gate *gateptr;
	//struct gate *pred_gateptr;
	//struct gate* gptr1,*gptr2;
	//short pred_gate_number;
	//float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	//float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	int tsum,tk;
	float tpsum;
	struct event* eventptr;
	int list_size;
	int max_event_size=0;

	for (ff_index=0; ff_index<gate_numbers; ff_index++)
	{
		nff_index = gate_list[ff_index]->out;
		//The comment can be removed!!
		if ((gate_list[ff_index]->is_dff == 1))// || (nff_index <= (numPi+numPo)))
		{
			gate_list[ff_index]->derating = 1;
			continue;
		}
		// To get PI2POlist starting from flip-flop nff_index excluding it
		find_TPsort_from_dffi(nff_index);
		PI2POlist_size = PI2POlist.size();
		// Set MAX values to ensure each will get correct value
		for (i=0; i<gate_numbers; i++)
		{
			gateptr= gate_list[i];
			gateptr->p0 = 0; gateptr->p1 = 0;
			gateptr->pd = 0; gateptr->pdn = 0;
			gateptr->is_onpath = false;
		}
		for (i=0; i<PI2POlist_size; i++)
		{
			gateptr = fadjlist_ptr[PI2POlist[i][0]];
			gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;
			gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
			gateptr->is_onpath = true;
		}
		gateptr = gate_list[ff_index];
		//gateptr->pd = 1; gateptr->pdn = 0; gateptr->p1 = 0; gateptr->p0 = 0;
		//
		//add event D
		//eventptr = (struct event*) malloc(sizeof(struct event));
		eventptr = new struct event;
		eventptr->time = 0;
		eventptr->p0 = 1- gateptr->signal_probability; eventptr->p1 = gateptr->signal_probability;
		eventptr->pd = 0; eventptr->pdn = 0;
		add_event(gateptr,eventptr);
		eventptr = new struct event;
		eventptr->time = (int)gateptr->gd;
		eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 1; eventptr->pdn = 0;
		add_event(gateptr,eventptr);
		eventptr = new struct event;
		eventptr->time = (int)gateptr->gd + config->seu_pulse_width;
		//APPROACH 1
		eventptr->p0 = 1 - gateptr->signal_probability ; eventptr->p1 = gateptr->signal_probability; eventptr->pd = 0; eventptr->pdn = 0;
		//APPROACH 2
		//eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 0; eventptr->pdn = 1;
		add_event(gateptr,eventptr);

		for (i=0; i<PI2POlist_size; i++)
		{
			gateptr = fadjlist_ptr[PI2POlist[i][0]];
			compute_D_gate_event_list(gateptr, nff_index);
			if (gateptr->event_list.size()>max_event_size)
				max_event_size = gateptr->event_list.size();

		}// End of PI2POlist
		///////////////////////////////////



		int ff_index2;
		for (ff_index2=0; ff_index2<dff_num; ff_index2++)
		{
			gateptr = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
			compute_D_gate_event_list(gateptr, nff_index);
			if (gateptr->event_list.size()>max_event_size)
				max_event_size = gateptr->event_list.size();
		}
		short MAXS=3,NMAXS=-1;
		float sys_failure=1;
		int ff_ind;
		for (ff_ind=0; ff_ind<dff_num; ff_ind++)
		{
			gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];

			list_size = gateptr->event_list.size();
			///*
			tpsum = 0;
			tsum = 0;
			for (k=0; k<gateptr->event_list.size()-1; k++)
			{
				tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;

				if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
					tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
				tsum += tk;
			}
			if (tsum != 0)
				sys_failure *= 1- ((float)tpsum+(config->t_setup+config->t_hold)*(float)tpsum/(float)tsum)/(float)config->tperiod;
			//sys_failure *= 1- ((float)tpsum+tsu+th)/(float)tperiod;
			//*/
			//cout << "**for "<< ff_ind << " " << ff_index << " TSUM= " << tsum<< endl ;
		}//for (ff_ind=0; ....
		sys_failure = 1 - sys_failure;
		gateptr = gate_list[ff_index]; //ff_index is gate_list index
		gateptr->derating = sys_failure;
		cout << gate_name(gateptr) << " gate["<< ff_index << "] sys_failure ="<< sys_failure << endl ;
		//fprintf(resultfp,"\n ================================= \n");
		//fprintf(resultfp,"Eventlist when ngate %d is faulty: \n", nff_index);


		for (i=0;i<gate_numbers; i++)
		{
			gateptr = gate_list[i];
			list_size = gateptr->event_list.size();
			//fprintf(resultfp,"\n Gate %d:  type=%d",gateptr->out,gateptr->type);
			//for (j=0; j<list_size; j++)
			//{	eventptr = gateptr->event_list[j];
			//	fprintf(resultfp, "\n\t %d \t\t", eventptr->time);
			//	fprintf(resultfp, "%f %f %f %f", eventptr->p0,eventptr->p1,eventptr->pd,eventptr->pdn);
			//}
			for (j=list_size-1; j>=0; j--)
			{
				eventptr = gateptr->event_list[j];
				//fprintf(resultfp, "%d, ", eventptr->time);
				delete eventptr;
				eventptr = NULL;
				gateptr->event_list.pop_back();
			}
		}
	}//for

	//fprintf(resultfp,"\n ================================= \n");
	//fprintf(resultfp,"Systemtic Derating Results\n");
	float overall_sys_der = 0;
	for (i=0; i<gate_numbers; i++)
	{
		gateptr = gate_list[i];
		if (gateptr->is_dff==1)
			continue;
		//fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sys_der += gateptr->derating;
	}
	overall_sys_der = overall_sys_der / (float)(gate_numbers-dff_num);
	circuit_sys_derating = overall_sys_der;
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Systematic Derating = %f \n", overall_sys_der);
	fprintf(resultfp, "Max event size in systematic derating=%d \n", max_event_size);
}//system_failure_multi_cycle


//
//	Systematic approach to calculate SER
//	supports logical, electrical & timing derating
//  this is a method we used for our DSN-2010 & TC paper.
//
void system_failure_multi_cycle_let(short clkC){
	float test = 0 ;
	long i,j,k;
	short ff_index;
	short nff_index;
	short PI2POlist_size;
	struct gate *gateptr;
	struct gate *pred_gateptr;
	struct gate* gptr1,*gptr2; 
	short pred_gate_number;
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	bool use_sp;
	int tsum,tk;

	float tpsum;
	struct event* eventptr;
	int list_size;
	int max_event_size=0;


	for (ff_index=0; ff_index<gate_numbers; ff_index++){
		test=0;
		nff_index = gate_list[ff_index]->out;
		if(config->verbose) cout << "gate:" << ff_index << endl;
		// To get PI2POlist starting from flip-flop nff_index excluding it
		find_TPsort_from_dffi(nff_index);		
		PI2POlist_size = PI2POlist.size();
		// Set MAX values to ensure each will get correct value
		for (i=0; i<gate_numbers; i++){
			gateptr= gate_list[i];
			gateptr->p0 = 1 - gateptr->signal_probability;
			gateptr->p1 = gateptr->signal_probability;
			gateptr->pd = 0;
			gateptr->pdn = 0;
		}

		//fault injection
		gateptr = gate_list[ff_index];
		gateptr->pd = 1;gateptr->pdn = 0;gateptr->p1 = 0;gateptr->p0 = 0;
		if(config->mbu_enable){
			vector<int> targets = mbu_site->findFaultSite(ff_index, config->mbu_depth, config->num_mbu-1);
			for(int i=0;i<targets.size();i++){
				cout << targets[i] << endl ;
				gateptr = gate_list[targets[i]];
				gateptr->pd = 1;gateptr->pdn = 0;gateptr->p1 = 0;gateptr->p0 = 0;
			}
		}
		for (int ck=1; ck<=clkC; ck++){
			if(ck==1){
				for (i=0; i<gate_numbers; i++){
					gateptr = gate_list[i];
					if (gateptr->is_dff == 1) continue; // we will compute the flip-flops at the end
						
					if (i == ff_index) continue;   //skip the faulty gate
					for (j=0; j< (gateptr->input_count); j++){
						// if input is Primary Input
						if ((gateptr->input_list[j]) <= numPi){
							p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
							p1_values[j] = PI_list_sp[gateptr->input_list[j]];
							pd_values[j] = 0; pdn_values[j] = 0;
							continue;
						}
						pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
						pred_gate_number = pred_gateptr->out;

						use_sp = false;
						//if (pred_gate_number<=(numPi+numPo)) use_sp = true;
						if (pred_gate_number<=(numPi)) use_sp = true;
						//for (k=0; k<fadjlist[pred_gate_number].size(); k++)
						//	if (fadjlist[pred_gate_number][k]->type==7) //is DFF
						//		use_sp = true;
						if (use_sp==true){
							p0_values[j] = 1- pred_gateptr->signal_probability;
							p1_values[j] = pred_gateptr->signal_probability;
							pd_values[j] = 0;
							pdn_values[j] = 0;
						}else{
							p0_values[j] = pred_gateptr->p0;
							p1_values[j] = pred_gateptr->p1;
							pd_values[j] = pred_gateptr->pd;
							pdn_values[j] = pred_gateptr->pdn;
						}
					}
					compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
				}
				// Now Calculating Latching Probability .
				// first, initialization
				for (i=0; i<gate_numbers; i++){
					gateptr= gate_list[i];

					//Modified by Babak, he insisted that it should be sp
					//gateptr->q0 = 1 - gateptr->signal_probability;
					//gateptr->q1 = gateptr->signal_probability;

					// original code, does not change the results
					gateptr->q0 = 0; gateptr->q1 = 0;
					gateptr->qd = 0; gateptr->qdn = 0;
					gateptr->is_onpath = false;
				}
				for (i=0; i<PI2POlist_size; i++){
					gateptr = fadjlist_ptr[PI2POlist[i][0]];
					gateptr->q0 = config->MAX; gateptr->q1 = config->MAX;
					gateptr->qd = config->MAX; gateptr->qdn = config->MAX;
					gateptr->is_onpath = true;
				}

				//Adding events
			
				//Adding events
				gateptr = gate_list[ff_index];
	
				eventptr = new struct event;
				eventptr->time = 0;
				eventptr->q0 = 1- gateptr->signal_probability; 
				eventptr->q1 = gateptr->signal_probability;
				eventptr->qd = 0; 
				eventptr->qdn = 0;
				add_event(gateptr,eventptr);

				eventptr = new struct event;
				eventptr->time = (int)gateptr->gd;
				eventptr->q0 = 0; 
				eventptr->q1 = 0;
				eventptr->qd = 1; 
				eventptr->qdn = 0;
				add_event(gateptr,eventptr);
				
				eventptr = new struct event;
				eventptr->time = (int)gateptr->gd + config->seu_pulse_width;
				//APPROACH 1, buggy! don't use
				//eventptr->q0 = 1 - gateptr->signal_probability ; 
				//eventptr->q1 = gateptr->signal_probability; 
				//eventptr->qd = 0; 
				//eventptr->qdn = 0;
				//APPROACH 2
				eventptr->q0 = 0; 
				eventptr->q1 = 0; 
				eventptr->qd = 0; 
				eventptr->qdn = 1;
				add_event(gateptr,eventptr);
				for (int i=0; i<PI2POlist_size; i++){
					gateptr = fadjlist_ptr[PI2POlist[i][0]];
					compute_Q_gate_event_list(gateptr, nff_index);	
					if (gateptr->event_list.size()>max_event_size){ 	
						max_event_size = gateptr->event_list.size();
					}
					
				}// End of PI2POlist
								 
				for (int ff_index2=0; ff_index2<dff_num; ff_index2++){
					gateptr = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
					compute_Q_gate_event_list(gateptr, nff_index);
					if (gateptr->event_list.size()>max_event_size)
						max_event_size = gateptr->event_list.size();
				}

				
/*				for (int ff_index2=0; ff_index2<dff_num; ff_index2++){
					gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
					gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;
					gptr2->pd = gptr1->pd;
					gptr2->pdn = gptr1->pdn;
				}
*/
				// Now, time for calculating results!
				// if it's a dff, derating=1 ??
				// calculating ser over dffs or POs?
				
				if(config->alg==heuristic_wave_flopping){
					// Algorithm 1: DSN2010 Alg. Wave flopping!
					float lp=1;
					// for each DFFs, we calcualte the LP * PP.
					for (int ff_ind=0; ff_ind<dff_num; ff_ind++){
						gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
						list_size = gateptr->event_list.size();
						//cout << ">>>>>>>>>>>>>>>>>>>>>>>>..DFF= "<< ff_ind << " " << gateptr->event_list.size()-1<< endl ;
						float let_a=1, let_an=1;
						//float let_a=0, let_an=0;
						//float latch_prop = 0 ;
						for(int i=0; i<gateptr->event_list.size()-1;i++){
							if( gateptr->event_list[i]->qd > 0 ){
								for(int j=i+1;j<gateptr->event_list.size()-1;j++){
									if( gateptr->event_list[j]->qdn > 0 ){// we found a wave
										float no_tran_pre=1, no_tran_post=1;
										float elpp_a = 1;
										elpp_a = gateptr->event_list[i]->qd ;
										//elpp_a = gateptr->event_list[i]->qd * gateptr->event_list[j]->qdn ;
										float wave_duration =  gateptr->event_list[j]->time - gateptr->event_list[i]->time ;
										float lp_a = (float)(config->t_setup+config->t_hold+wave_duration)/(float)config->tperiod ;
										lp_a=(lp_a>1?1:lp_a);
										let_a = let_a * (1-elpp_a*lp_a);
									}
								}
							}
							//let_a=1-let_a;
							if( gateptr->event_list[i]->qdn > 0  ){
								for(int j=i+1;j<gateptr->event_list.size()-1;j++){
									if( gateptr->event_list[j]->qd > 0 ){// we found a wave
										float no_tran_pre=1, no_tran_post=1;
										float elpp_an = 1;
										elpp_an = gateptr->event_list[i]->qdn ;
										//elpp_an = gateptr->event_list[i]->qdn * gateptr->event_list[j]->qd;
										float wave_duration =  gateptr->event_list[j]->time - gateptr->event_list[i]->time ;
										float lp_an = (float)(config->t_setup+config->t_hold+wave_duration)/(float)config->tperiod ;
										lp_an=(lp_an>1?1:lp_an);
										let_an = let_an * (1-elpp_an*lp_an);
									}
								}
							}
						}

						gptr1 = fadjlist_ptr[dfflist_ptr[ff_ind]->d];
						gptr2 = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
						gptr2->p0 = gptr1->p0;
						gptr2->p1 = gptr1->p1;
						//gptr2->pd  = (let_a==0)? gptr1->pd :1-let_a ;
						//gptr2->pdn = (let_an==0)?gptr1->pdn : 1-let_an;
						gptr2->pd = 1-let_a ;//gptr1->pd;
						gptr2->pdn = 1-let_an ; //gptr1->pdn;
					}//for (ff_ind=0; ....
				}else if( config->alg==maze){
					//Algorithm 6 : Maze
					// author: Adel (ahmadyan@gmail.com)
					// original author: Hossein Asadi (asadi@sharif.edu)
					// date: April-30-2010
					int MAXS=3;
					
//					float* lp_avg = new float[dff_num];
//					for(int i=0;i<dff_num;i++) lp_avg[i]=0;

//					int* lp_total = new int[dff_num];
//					for(int i=0;i<dff_num;i++) lp_avg[i]=0;
					
					float tpsum_a=0, tpsum_an=0, tsum_a=0, tsum_an=0;
					for(int ff_ind=0; ff_ind<dff_num; ff_ind++){
						float let_a=1, let_an=1;
						gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
						int list_size = gateptr->event_list.size();
						int *maze = new int[list_size];
						for (i=0; i<list_size; i++)
							maze[i] = -1;
						i = 0;
						tsum_a = 0, tsum_an = 0;;
						while (i>=0){
							maze[i]++;
							if (maze[i]>=MAXS){
								maze[i] = -1;
								i--;
								continue;
							}
							j = i-1;
							while (j>=0){
								if (maze[j]!=0)
									break;
								j--;
							}
							if (j>=0){
								if (maze[i]==maze[j])
									maze[i]++;
							}
							if (maze[i]>=MAXS){
								maze[i] = -1;
								i--;
								continue;
							}
							if (i==list_size-1)	{
								int fi,si;
								fi = 0;
								while (fi<list_size){
									if (maze[fi]==0){
										fi++;
										continue;
									}else
										break;
								}
								si = list_size-1;
								while (si>=0){
									if (maze[si]==0){
										si--;
										continue;
									}else
										break;
								}

								if ((fi>=list_size)||(si<0))
									continue;
								if (maze[fi]==maze[si])
									continue;
								float prob=1;
								for (j=0; j<list_size; j++){
									if (maze[j]==0)
										prob *= (gateptr->event_list[j]->q0 + gateptr->event_list[j]->q1);
									if (maze[j]==1)
										prob *= gateptr->event_list[j]->qd;
									if (maze[j]==2)
										prob *= gateptr->event_list[j]->qdn;
								}
								tk = 0;
								fi = 0;
								si = 0;
								while (fi<list_size){
									if (maze[fi]==0){
										fi++;
										continue;
									}
									if (fi>=list_size)
										break;
									si = fi+1;
									while (si<list_size){
										if (maze[si]!=0)
											break;
										si++;
									}
									if (si>=list_size)
										break;
									tk += gateptr->event_list[si]->time - gateptr->event_list[fi]->time;
									fi = si;
								}//inner while
								
								float lp=0 ;
								
								int c=0;
								while(maze[c]==0){
									c++;
								}
								if( maze[c]==1 ){
									lp = (float)(config->t_setup+config->t_hold+tk)/(float)config->tperiod ;
									if(lp>=1) lp=1;
																	
									let_a *= ( 1- lp * prob ) ;
								}else{ //a'
									lp = (float)(config->t_setup+config->t_hold+tk)/(float)config->tperiod ;
									if(lp>=1) lp=1;
									let_an *= ( 1- lp * prob ) ;
								}
					//			lp_avg[i] += lp ;
					//			lp_total[i]++ ;
								continue;
							}//if (i==list_size-1)
							i++;
						}//outer while
						
						gptr1 = fadjlist_ptr[dfflist_ptr[ff_ind]->d];
						gptr2 = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
						gptr2->p0 = gptr1->p0;
						gptr2->p1 = gptr1->p1;
						gptr2->pd = 1-let_a ;//gptr1->pd;
						gptr2->pdn = 1-let_an ; //gptr1->pdn;
						delete [] maze;
					}
//					for(int i=0;i<dff_num;i++) if(lp_total[i]!=0) lp_avg[i] /= lp_total[i];
//									else lp_avg[i]=0;
//					for(int i=0;i<dff_num;i++){
//						cout << "LP average for dff(" << i << ") = " << lp_avg[i] << endl;
//					}
//					delete[] lp_avg ;
//					delete[] lp_total ;
				}else{
					cout << "ERROR: algorithm for static analyze is not defined!" << endl ;
					exit(1);
				}			
				

				SatCK[ff_index][ck-1] = 1;
				for (int gate_index=0; gate_index<gate_numbers; gate_index++){
					gptr2 = gate_list[gate_index];
					//if(gptr2->is_dff==1) test *= 1 - (gptr2->qd + gptr2->qdn);
					if(gptr2->pd<0)gptr2->pd=0;
					if(gptr2->pd>1)gptr2->pd=1;
					if(gptr2->pdn<0)gptr2->pdn=0;
					if(gptr2->pdn>1)gptr2->pdn=1;

					if (gptr2->po_no>=0) SatCK[ff_index][ck-1] *= 1 - (gptr2->pd + gptr2->pdn);
				//	cout << ck << " " << SatCK[ff_index][ck-1]  << " " << (gptr2->pd + gptr2->pdn) << endl ;
				}
				SatCK[ff_index][ck-1] =  1 - SatCK[ff_index][ck-1] ;
				//cout << " *** " << SatCK[ff_index][ck-1] << endl ;
				//test=1-test;
				//cout << endl << endl << ff_index << " test= " << test << endl ;
				/*
				for(int ff_ind=0;ff_ind<dff_num; ff_ind++){
					lp=1;
					gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
					list_size = gateptr->event_list.size();
					tpsum = 0; tsum = 0;
					for (k=0; k<gateptr->event_list.size()-1; k++){
						tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
						//tpsum += tk * (gateptr->event_list[k]->qd+gateptr->event_list[k]->qdn);
						tsum += tk;
					}
					if (tsum != 0) lp *= 1- ((float)tpsum+(tsu+th)*(float)tpsum/(float)tsum)/(float)tperiod;
					sys_failure = 1 - sys_failure;
					
					SatCK[gateptr->gate_no][0] = SatCK[gateptr->gate_no][0] * sys_failure  ;
					cout << ff_ind << " " << gateptr->gate_no << " | "<< sys_failure << " " << SatCK[gateptr->gate_no][0] << endl ;
				}
				*/
				//sys_failure = 1 - sys_failure;
				//cout << "System Failure for each gate=" << ff_index << " " << sys_failure << endl ;

				for (int i=0;i<gate_numbers; i++){
					gateptr = gate_list[i];
					list_size = gateptr->event_list.size();
					for (j=list_size-1; j>=0; j--){
						eventptr = gateptr->event_list[j];
						delete eventptr;
						eventptr = NULL;
						gateptr->event_list.pop_back();
					}
				}
				
			}else{// for clk=2..n
				//PART1: Computing D Probs.
				for (int i=0; i<gate_numbers; i++){
					gateptr = gate_list[i];
					if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
						continue;
					for (int j=0; j< (gateptr->input_count); j++){
						if ((gateptr->input_list[j]) <= numPi){	// if input is Primary Input
							p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
							p1_values[j] = PI_list_sp[gateptr->input_list[j]];
							pd_values[j] = 0; pdn_values[j] = 0;
							continue;
						}
						pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
						pred_gate_number = pred_gateptr->out;
						use_sp = false;
						//if (pred_gate_number<=(numPi+numPo))
						if (pred_gate_number<=(numPi))
							use_sp = true;
						if (use_sp==true){
							p0_values[j] = 1- pred_gateptr->signal_probability;
							p1_values[j] = pred_gateptr->signal_probability;
							pd_values[j] = 0;
							pdn_values[j] = 0;
						}else{
							p0_values[j] = pred_gateptr->p0;
							p1_values[j] = pred_gateptr->p1;
							pd_values[j] = pred_gateptr->pd;
							pdn_values[j] = pred_gateptr->pdn;
						}
					}
					compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
				}

				
				//PART2: Copying input of DFFs to it's output for next cycle
				for (int ff_index2=0; ff_index2<dff_num; ff_index2++){
					gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
					gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
					gptr2->p0 = gptr1->p0;
					gptr2->p1 = gptr1->p1;
					
					//cout << "*** clk=1+ [old values] \t" << gptr2->pd <<  " " <<  gptr2->pdn << endl ;  

					gptr2->pd = gptr1->pd;
					gptr2->pdn = gptr1->pdn;
					
					if(gptr2->pd<0)gptr2->pd=0;
					if(gptr2->pd>1)gptr2->pd=1;
					if(gptr2->pdn<0)gptr2->pdn=0;
					if(gptr2->pdn>1)gptr2->pdn=1;

					//cout << "*** clk=1+ " << gptr2->pd <<  " " <<  gptr2->pdn << endl ;  
				}


				
				//PART3: Computing Deratings.
				SatCK[ff_index][ck-1] = 1;
				for (int gate_index=0; gate_index<gate_numbers; gate_index++){
					gptr2 = gate_list[gate_index];

					if(gptr2->pd<0)gptr2->pd=0;
					if(gptr2->pd>1)gptr2->pd=1;
					if(gptr2->pdn<0)gptr2->pdn=0;
					if(gptr2->pdn>1)gptr2->pdn=1;
					if (gptr2->po_no>=0){
						//cout << " *$" << gptr2->gate_no << " " << gptr2->pd << " " << gptr2->pdn << endl ;
					}
					if (gptr2->po_no>=0)
						SatCK[ff_index][ck-1] *= 1 - (gptr2->pd + gptr2->pdn);
					//cout << ck << " " << SatCK[ff_index][ck-1]  << " " << (gptr2->pd + gptr2->pdn) << endl ;

				}
				SatCK[ff_index][ck-1] = 1 - SatCK[ff_index][ck-1];
				//cout << " *** " << SatCK[ff_index][ck-1] << endl ;

				//fprintf( resultfp, " [Clock=%d] derating[ %d ]= %f \n", ck, ff_index, SatCK[ff_index][ck-1] );
			}//end clk=2
		}//for (ck=1; ...
		
	}//outmost FOR

	// Finally, calculating total circuit derating
	float mul_fact, prev_sum;
	fprintf(resultfp, "Analytical System Failure Results: Cycles 1 through n\n");
	fprintf(resultfp, "node num ::");
	for (int clk_index=0; clk_index<clkC; clk_index++)
		fprintf(resultfp, " clk=%2d ::", clk_index+1);
	fprintf(resultfp, "\n");

	for (i=0; i<gate_numbers; i++){
		gateptr = gate_list[i];
		fprintf(resultfp, "node %4d::",gateptr->out);
		prev_sum = 0;
		mul_fact = 1;
		for (int clk_index=0; clk_index<clkC; clk_index++){
			S1_to_CK[i][clk_index] = SatCK[i][clk_index]*mul_fact + prev_sum;
			mul_fact *= 1 - SatCK[i][clk_index];
			prev_sum = S1_to_CK[i][clk_index];
			fprintf(resultfp, "%f::", S1_to_CK[i][clk_index]);
		}
		fprintf(resultfp,"\n");
		Sck[i] = S1_to_CK[i][clkC-1];
	}
	fprintf(resultfp,"\n");
	//for (int ck=1; ck<=clkC; ck++){
	//	cout << "clk[" << ck << "] " ; 
	//	for (ff_index=0; ff_index<gate_numbers; ff_index++){
	//		printf("%f\t", SatCK[ff_index][ck-1] );
	//	}
	//	cout << endl ;
	//}


	fprintf(resultfp, "Check if  FFs are in the first part of gate_list \n");
	for (i=0; i<dff_num; i++){
		if (gate_list[i]->type!=7){
			fprintf(resultfp, "gate %d = type %d \n", i, gate_list[i]->type);
			fprintf(resultfp, "ALARM!");
		}
	}
}

//
//	Systematic approach to calculate SER
//	supports logical, electrical & timing derating
//
void system_failure_multi_cycle_old(short clkC){
    long i,j,k;
    short ff_index;
    short nff_index;
    short PI2POlist_size;
    struct gate *gateptr;
   	int tsum,tk;
	float tpsum;
	struct event* eventptr;
	int list_size;
	int max_event_size=0;

	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	

	struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
    short pred_gate_number;
	bool use_sp;
	

    for (ff_index=0; ff_index<gate_numbers; ff_index++){
        nff_index = gate_list[ff_index]->out;
		if ((gate_list[ff_index]->is_dff == 1)){
			gate_list[ff_index]->derating = 1;
			continue;
		}
        // To get PI2POlist starting from flip-flop nff_index excluding it
        find_TPsort_from_dffi(nff_index);
        PI2POlist_size = PI2POlist.size();
        // Set MAX values to ensure each will get correct value
        for (i=0; i<gate_numbers; i++){
            gateptr= gate_list[i];
            gateptr->p0 = 0; gateptr->p1 = 0;
            gateptr->pd = 0; gateptr->pdn = 0;
			gateptr->is_onpath = false;
        }
	    for (i=0; i<PI2POlist_size; i++){
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
            gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;
            gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
			gateptr->is_onpath = true;
		}
		gateptr = gate_list[ff_index];
		//gateptr->pd = 1; gateptr->pdn = 0; gateptr->p1 = 0; gateptr->p0 = 0;
        //
		//add event D
		//eventptr = (struct event*) malloc(sizeof(struct event));
		eventptr = new struct event;
		eventptr->time = 0;
		eventptr->p0 = 1- gateptr->signal_probability; eventptr->p1 = gateptr->signal_probability;
		eventptr->pd = 0; eventptr->pdn = 0;
		add_event(gateptr,eventptr);
		eventptr = new struct event;
		eventptr->time = (int)gateptr->gd;
		eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 1; eventptr->pdn = 0;
		add_event(gateptr,eventptr);
		eventptr = new struct event;
		eventptr->time = (int)gateptr->gd + config->seu_pulse_width;
		//APPROACH 1
		eventptr->p0 = 1 - gateptr->signal_probability ; eventptr->p1 = gateptr->signal_probability; eventptr->pd = 0; eventptr->pdn = 0;
		//APPROACH 2
		//eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 0; eventptr->pdn = 1;
		add_event(gateptr,eventptr);

	    for (i=0; i<PI2POlist_size; i++)
	    {
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
            compute_D_gate_event_list(gateptr, nff_index);
			if (gateptr->event_list.size()>max_event_size)
				max_event_size = gateptr->event_list.size();

	    }// End of PI2POlist

        int ff_index2;
        for (ff_index2=0; ff_index2<dff_num; ff_index2++){
            gateptr = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
            compute_D_gate_event_list(gateptr, nff_index);
			if (gateptr->event_list.size()>max_event_size)
				max_event_size = gateptr->event_list.size();
        }
		short MAXS=3,NMAXS=-1;
        float sys_failure=1;
		int ff_ind;
		for (ff_ind=0; ff_ind<dff_num; ff_ind++){
			gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];

			list_size = gateptr->event_list.size();
			tpsum = 0;
			tsum = 0;
			for (k=0; k<gateptr->event_list.size()-1; k++){
				tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
				if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
					tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
				tsum += tk;
			}
			//cout << "tsum= " << tsum << "  tpsum= " << tpsum << " tsu+th"<<  endl ;
			if (tsum != 0) sys_failure *= 1- ((float)tpsum+(config->t_setup+config->t_hold)*(float)tpsum/(float)tsum)/(float)config->tperiod;
		
			//cout << "**for "<< ff_ind << " "  << gate_name(gateptr) << " "<< ff_index << " TSUM= " << tsum<< endl ;
		}//for (ff_ind=0; ....
        sys_failure = 1 - sys_failure;
		gateptr = gate_list[ff_index]; //ff_index is gate_list index
		gateptr->derating = sys_failure;
		cout << gate_name(gateptr) << " gate["<< ff_index << "] sys_failure ="<< sys_failure << endl ;
		SatCK[ff_index][0] = sys_failure ; 
		for (i=0;i<gate_numbers; i++){
			gateptr = gate_list[i];
			list_size = gateptr->event_list.size();
		
			for (j=list_size-1; j>=0; j--)
			{ 
				eventptr = gateptr->event_list[j];
				delete eventptr;
				eventptr = NULL;
				gateptr->event_list.pop_back();
			}
		}

			//-----------------------------------------------------------------

			for (int ff_index3=0; ff_index3<dff_num; ff_index3++)
			{
				gptr1 = fadjlist_ptr[dfflist_ptr[ff_index3]->d];
				gptr2 = fadjlist_ptr[dfflist_ptr[ff_index3]->q];
				gptr2->p0 = gptr1->p0;
				gptr2->p1 = gptr1->p1;
				gptr2->pd = gptr1->pd;
				gptr2->pdn = gptr1->pdn;
			}
			//-----------------------------------------------------------------

    }//for
	cout << "**************" << endl ;
	for (ff_index=0; ff_index<gate_numbers; ff_index++){
		cout << "gate[" << ff_index << "]=" << gate_name(gate_list[ff_index]) << " =" << SatCK[ff_index][0] << endl ; 
	}

	//Now we Cycle through clock pulse 2 to n
	//at this stage, we don't need to calculate timing or electrical derating, only logical derating
	for (ff_index=0; ff_index<gate_numbers; ff_index++)
	{
        nff_index = gate_list[ff_index]->out;
		cout << "gate:" << ff_index << endl;
        // To get PI2POlist starting from flip-flop nff_index excluding it
        //find_TPsort_from_dffi(nff_index);
        //PI2POlist_size = PI2POlist.size();
        // Set MAX values to ensure each will get correct value
        for (i=0; i<gate_numbers; i++)
        {
            gateptr= gate_list[i];
            gateptr->p0 = 1 - gateptr->signal_probability;
            gateptr->p1 = gateptr->signal_probability;
            gateptr->pd = 0;
            gateptr->pdn = 0;
        }
		gateptr = gate_list[ff_index];
		gateptr->pd = 1;gateptr->pdn = 0;gateptr->p1 = 0;gateptr->p0 = 0;

		for (int ck=2; ck<=clkC; ck++)
		{

			for (i=0; i<gate_numbers; i++)
			{
				gateptr = gate_list[i];
				if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
					continue;
				if ((i == ff_index) && (ck==1))  //skip the faulty gate
					continue;
				for (j=0; j< (gateptr->input_count); j++)
				{
					// if input is Primary Input
					if ((gateptr->input_list[j]) <= numPi)
					{
						p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
						p1_values[j] = PI_list_sp[gateptr->input_list[j]];
						pd_values[j] = 0; pdn_values[j] = 0;
						continue;
					}
					pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
					pred_gate_number = pred_gateptr->out;

					use_sp = false;
					//BugFix:asadi
					//if (pred_gate_number<=(numPi+numPo))
					//	use_sp = true;
					if (pred_gate_number<=(numPi))
						use_sp = true;

					//for (k=0; k<fadjlist[pred_gate_number].size(); k++)
					//	if (fadjlist[pred_gate_number][k]->type==7) //is DFF
					//		use_sp = true;
					if (use_sp==true)
					{
						p0_values[j] = 1- pred_gateptr->signal_probability;
						p1_values[j] = pred_gateptr->signal_probability;
						pd_values[j] = 0;
						pdn_values[j] = 0;
					}
					else
					{
						p0_values[j] = pred_gateptr->p0;
						p1_values[j] = pred_gateptr->p1;
						pd_values[j] = pred_gateptr->pd;
						pdn_values[j] = pred_gateptr->pdn;
					}
				}
				compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
			}
			int ff_index2;
			for (ff_index2=0; ff_index2<dff_num; ff_index2++)
			{
				gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
				gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
				gptr2->p0 = gptr1->p0;
				gptr2->p1 = gptr1->p1;
				gptr2->pd = gptr1->pd;
				gptr2->pdn = gptr1->pdn;
			}
			SatCK[ff_index][ck-1] = 1;
			int gate_index;
			for (gate_index=0; gate_index<gate_numbers; gate_index++)
			{
				gptr2 = gate_list[gate_index];
				if (gptr2->po_no>=0)
					SatCK[ff_index][ck-1] *= 1 - (gptr2->pd + gptr2->pdn);
			}
			SatCK[ff_index][ck-1] = 1 - SatCK[ff_index][ck-1];
		}//for (ck=1; ...
    }//outmost FOR



	float mul_fact, prev_sum;
	//fprintf(resultfp, "Analytical System Failure Results: Cycles 1 through n\n");
	//fprintf(resultfp, "node num ::");
	//int clk_index;
	//for (clk_index=0; clk_index<clkC; clk_index++)
		//if (clk_index%5==0)
	//	fprintf(resultfp, " clk=%2d ::", clk_index+1);
	//fprintf(resultfp, "\n");

	for (i=0; i<gate_numbers; i++)
	{
		gateptr = gate_list[i];
		//fprintf(resultfp, "node %4d::",gateptr->out);
		prev_sum = 0;
		mul_fact = 1;
		for (int clk_index=0; clk_index<clkC; clk_index++)
		{
			S1_to_CK[i][clk_index] = SatCK[i][clk_index]*mul_fact + prev_sum;
			mul_fact *= 1 - SatCK[i][clk_index];
			prev_sum = S1_to_CK[i][clk_index];
			//if (clk_index%5==0)
			//fprintf(resultfp, "%f::", S1_to_CK[i][clk_index]);
		}
		//fprintf(resultfp,"\n");
		Sck[i] = S1_to_CK[i][clkC-1];
	}
	//fprintf(resultfp,"\n");


	for (i=0; i<gate_numbers; i++)
	{
		cout << i << " "  << Sck[i] << endl ;
	}


/*	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Systemtic Derating Results\n");
	float overall_sys_der = 0;
	for (i=0; i<gate_numbers; i++)
	{
		gateptr = gate_list[i];
		if (gateptr->is_dff==1)
			continue;
		fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sys_der += gateptr->derating;
	}
	overall_sys_der = overall_sys_der / (float)(gate_numbers-dff_num);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Systematic Derating = %f \n", overall_sys_der);
	fprintf(resultfp, "Max event size in systematic derating=%d \n", max_event_size);
*/
}
//
//	Systematic approach to calculate SER
//	supports logical, electrical & timing derating
//
void system_failure_clockC_multi_prop_new(short clkC){
    long i,j,k;//,k,m;
    short ff_index;
    short nff_index;
    short PI2POlist_size;
    struct gate *gateptr;
    struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
    short pred_gate_number;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	int ck;
	bool use_sp;
	
	int tsum,tk;
	float tpsum;
	struct event* eventptr;
	int list_size;
	int max_event_size=0;


	for (ff_index=0; ff_index<gate_numbers; ff_index++){
        nff_index = gate_list[ff_index]->out;
		cout << "gate:" << ff_index << endl;
        // To get PI2POlist starting from flip-flop nff_index excluding it
        //find_TPsort_from_dffi(nff_index);
        //PI2POlist_size = PI2POlist.size();
        // Set MAX values to ensure each will get correct value
        for (i=0; i<gate_numbers; i++){
            gateptr= gate_list[i];
            gateptr->p0 = 1 - gateptr->signal_probability;
            gateptr->p1 = gateptr->signal_probability;
            gateptr->pd = 0;
            gateptr->pdn = 0;
        }
		gateptr = gate_list[ff_index];
		gateptr->pd = 1;gateptr->pdn = 0;gateptr->p1 = 0;gateptr->p0 = 0;

		//CK=1
		ck=1;
		if ((gate_list[ff_index]->is_dff == 1))// || (nff_index <= (numPi+numPo)))
		{
			gate_list[ff_index]->derating = 1;
			continue;
		}
		find_TPsort_from_dffi(nff_index);
        PI2POlist_size = PI2POlist.size();

        for (i=0; i<gate_numbers; i++){
            gateptr= gate_list[i];
            gateptr->p0 = 0; gateptr->p1 = 0;
            gateptr->pd = 0; gateptr->pdn = 0;
			gateptr->is_onpath = false;
        }
	    for (i=0; i<PI2POlist_size; i++){
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
            gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;
            gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
			gateptr->is_onpath = true;
		}
		gateptr = gate_list[ff_index];

		//add event D
		//eventptr = (struct event*) malloc(sizeof(struct event));
		eventptr = new struct event;
		eventptr->time = 0;
		eventptr->p0 = 1- gateptr->signal_probability; eventptr->p1 = gateptr->signal_probability;
		eventptr->pd = 0; eventptr->pdn = 0;
		add_event(gateptr,eventptr);
		eventptr = new struct event;
		eventptr->time = (int)gateptr->gd;
		eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 1; eventptr->pdn = 0;
		add_event(gateptr,eventptr);
		eventptr = new struct event;
		eventptr->time = (int)gateptr->gd + config->seu_pulse_width;
		//cout << " fault injection event @ " << eventptr->time << endl ;
		//APPROACH 1
		eventptr->p0 = 1 - gateptr->signal_probability ; eventptr->p1 = gateptr->signal_probability; eventptr->pd = 0; eventptr->pdn = 0;
		//APPROACH 2
		//eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 0; eventptr->pdn = 1;
		
		add_event(gateptr,eventptr);
	
		//cout << "gate event=" << gateptr->type << " " << gateptr->event_list.size() << endl ;;
	
		//cout << "****************** after adding event" << endl ;
		//for (k=0; k<gateptr->event_list.size()-1; k++){
		//	if ( (gateptr->event_list[k+1]->time - gateptr->event_list[k]->time) > 0 ){
		//		cout << "gotcha!" << endl ;
		//		cout << gateptr->event_list[k+1]->time << "  - " << gateptr->event_list[k]->time << endl ;
		//		cout << "k="<< k<< " | "<<  gateptr->event_list[k]->p0 << " " << gateptr->event_list[k]->p1 << " " << gateptr->event_list[k]->pd << " " << gateptr->event_list[k]->pdn << " " << gateptr->event_list[k]->vomax << " " << gateptr->event_list[k]->vomin << " " << endl ; 
		//	}
		//}
		
		for (i=0; i<PI2POlist_size; i++){
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
            compute_D_gate_event_list(gateptr, nff_index);
			if (gateptr->event_list.size()>max_event_size)
				max_event_size = gateptr->event_list.size();
	    }// End of PI2POlist

		

		for (int i=0; i<dff_num; i++){
            gateptr = fadjlist_ptr[dfflist_ptr[i]->q];
            compute_D_gate_event_list(gateptr, nff_index);
			if (gateptr->event_list.size()>max_event_size) max_event_size = gateptr->event_list.size();
        }
		//cout << "****************** Compute D Gate Event" << endl ;
        
		//for (k=0; k<gateptr->event_list.size()-1; k++){
		//	if ( (gateptr->event_list[k+1]->time - gateptr->event_list[k]->time) > 0 ){
		//		cout << "gotcha!" << endl ;
		//		cout << gateptr->event_list[k+1]->time << "  - " << gateptr->event_list[k]->time << endl ;
		//		cout << "k="<< k<< " | "<<  gateptr->event_list[k]->p0 << " " << gateptr->event_list[k]->p1 << " " << gateptr->event_list[k]->pd << " " << gateptr->event_list[k]->pdn << " " << gateptr->event_list[k]->vomax << " " << gateptr->event_list[k]->vomin << " " << endl ; 
		//	}
		//}
		
		short MAXS=3,NMAXS=-1;
        float sys_failure=1;
		int ff_ind;
		for (ff_ind=0; ff_ind<dff_num; ff_ind++){
			gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];
			list_size = gateptr->event_list.size();
			tpsum = 0;
			tsum = 0;
			
			for (k=0; k<gateptr->event_list.size()-1; k++){
				tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
				if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0)))
					tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
				tsum += tk;
			}
			
			if (tsum != 0)
				sys_failure *= 1- ((float)tpsum+(config->t_setup+config->t_hold)*(float)tpsum/(float)tsum)/(float)config->tperiod;
			cout << "**for "<< ff_ind << " " << ff_index << " TSUM= " << tsum<< endl ;
		}//for (ff_ind=0; ....
		
		SatCK[ff_index][0] = sys_failure / (float)(gate_numbers-dff_num);
		
		for (j=list_size-1; j>=0; j--)
			{
				eventptr = gateptr->event_list[j];
				//fprintf(resultfp, "%d, ", eventptr->time);
				delete eventptr;
				eventptr = NULL;
				gateptr->event_list.pop_back();
			}
		//for (int gate_index=0; gate_index<gate_numbers; gate_index++){
		//	gptr2 = gate_list[gate_index];
		//	if (gptr2->po_no>=0)
		//		SatCK[ff_index][0] *= 1 - (gptr2->pd + gptr2->pdn);
		//}
		SatCK[ff_index][0] = 1 - SatCK[ff_index][0];

		cout << "System Failure for gate index= " << ff_index << " = "<< SatCK[ff_index][0] << endl ;
		cout << " fine for now! " << ff_index << endl ;
		/*
			for (i=0; i<gate_numbers; i++){
				gateptr = gate_list[i];
				if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
					continue;
				if (i == ff_index) // at first clock, skip the faulty gate
					continue;
				for (j=0; j< (gateptr->input_count); j++){
					// if input is Primary Input
					if ((gateptr->input_list[j]) <= numPi){
						p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
						p1_values[j] = PI_list_sp[gateptr->input_list[j]];
						pd_values[j] = 0; pdn_values[j] = 0;
						continue;
					}
					pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
					pred_gate_number = pred_gateptr->out;

					use_sp = false;
					if (pred_gate_number<=(numPi+numPo))
						use_sp = true;
					//for (k=0; k<fadjlist[pred_gate_number].size(); k++)
					//	if (fadjlist[pred_gate_number][k]->type==7) //is DFF
					//		use_sp = true;
					if (use_sp==true){
						p0_values[j] = 1- pred_gateptr->signal_probability;
						p1_values[j] = pred_gateptr->signal_probability;
						pd_values[j] = 0;
						pdn_values[j] = 0;
					}else{
						p0_values[j] = pred_gateptr->p0;
						p1_values[j] = pred_gateptr->p1;
						pd_values[j] = pred_gateptr->pd;
						pdn_values[j] = pred_gateptr->pdn;
					}
				}
				compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
			}
			int ff_index2;
			for (ff_index2=0; ff_index2<dff_num; ff_index2++){
				gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
				gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
				gptr2->p0 = gptr1->p0;
				gptr2->p1 = gptr1->p1;
				gptr2->pd = gptr1->pd;
				gptr2->pdn = gptr1->pdn;
			}
			SatCK[ff_index][0] = 1;
			int gate_index;
			for (gate_index=0; gate_index<gate_numbers; gate_index++){
				gptr2 = gate_list[gate_index];
				if (gptr2->po_no>=0)
					SatCK[ff_index][0] *= 1 - (gptr2->pd + gptr2->pdn);
			}
			SatCK[ff_index][0] = 1 - SatCK[ff_index][0];
		*/
		// END CK=1

		for (ck=2; ck<=clkC; ck++){
			for (int i=0; i<gate_numbers; i++){
				gateptr = gate_list[i];
				if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
					continue;
				for (int j=0; j< (gateptr->input_count); j++){
					// if input is Primary Input
					if ((gateptr->input_list[j]) <= numPi){
						p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
						p1_values[j] = PI_list_sp[gateptr->input_list[j]];
						pd_values[j] = 0; pdn_values[j] = 0;
						continue;
					}
					pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
					pred_gate_number = pred_gateptr->out;

					use_sp = false;
					if (pred_gate_number<=(numPi+numPo))
						use_sp = true;
					//for (k=0; k<fadjlist[pred_gate_number].size(); k++)
					//	if (fadjlist[pred_gate_number][k]->type==7) //is DFF
					//		use_sp = true;
					if (use_sp==true){
						p0_values[j] = 1- pred_gateptr->signal_probability;
						p1_values[j] = pred_gateptr->signal_probability;
						pd_values[j] = 0;
						pdn_values[j] = 0;
					}else{
						p0_values[j] = pred_gateptr->p0;
						p1_values[j] = pred_gateptr->p1;
						pd_values[j] = pred_gateptr->pd;
						pdn_values[j] = pred_gateptr->pdn;
					}
				}
				compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
			}
			int ff_index2;
			for (ff_index2=0; ff_index2<dff_num; ff_index2++){
				gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
				gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
				gptr2->p0 = gptr1->p0;
				gptr2->p1 = gptr1->p1;
				gptr2->pd = gptr1->pd;
				gptr2->pdn = gptr1->pdn;
			}
			SatCK[ff_index][ck-1] = 1;
			int gate_index;
			for (gate_index=0; gate_index<gate_numbers; gate_index++){
				gptr2 = gate_list[gate_index];
				if (gptr2->po_no>=0)
					SatCK[ff_index][ck-1] *= 1 - (gptr2->pd + gptr2->pdn);
			}
			SatCK[ff_index][ck-1] = 1 - SatCK[ff_index][ck-1];
		}//for (ck=1; ...
    }//outmost FOR



	float mul_fact, prev_sum;
	fprintf(resultfp, "Analytical System Failure Results: Cycles 1 through n\n");
	fprintf(resultfp, "node num ::");
	int clk_index;
	for (clk_index=0; clk_index<clkC; clk_index++)
		//if (clk_index%5==0)
		fprintf(resultfp, " clk=%2d ::", clk_index+1);
	fprintf(resultfp, "\n");

	for (i=0; i<gate_numbers; i++){
		gateptr = gate_list[i];
		fprintf(resultfp, "node %4d::",gateptr->out);
		prev_sum = 0;
		mul_fact = 1;
		for (clk_index=0; clk_index<clkC; clk_index++){
			S1_to_CK[i][clk_index] = SatCK[i][clk_index]*mul_fact + prev_sum;
			mul_fact *= 1 - SatCK[i][clk_index];
			prev_sum = S1_to_CK[i][clk_index];
			//if (clk_index%5==0)
			fprintf(resultfp, "%f::", S1_to_CK[i][clk_index]);
		}
		fprintf(resultfp,"\n");
		Sck[i] = S1_to_CK[i][clkC-1];
	}
	fprintf(resultfp,"\n");


	//================

	fprintf(resultfp, "Check if  FFs are in the first part of gate_list \n");
	for (i=0; i<dff_num; i++){
		if (gate_list[i]->type!=7)
		{
			fprintf(resultfp, "gate %d = type %d \n", i, gate_list[i]->type);
			fprintf(resultfp, "ALARM!");
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////
// Function: system_failure_fpga
// Purpose:  Computes system failure probability at clock C
// Returns none
void system_failure_fpga(short clkC){ 
    long i,j,k;//,k,m;
    short ff_index;
    short nff_index;
    short PI2POlist_size;
    struct gate *gateptr;
    struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
    short pred_gate_number;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	int ck;
	bool use_sp;
	for (ff_index=0; ff_index<gate_numbers; ff_index++){
        nff_index = gate_list[ff_index]->out;
		cout << "gate:" << ff_index << endl;
        // To get PI2POlist starting from flip-flop nff_index excluding it
        //find_TPsort_from_dffi(nff_index);
        //PI2POlist_size = PI2POlist.size();
        // Set MAX values to ensure each will get correct value
        for (i=0; i<gate_numbers; i++){
            gateptr= gate_list[i];
            gateptr->p0 = 1 - gateptr->signal_probability;
            gateptr->p1 = gateptr->signal_probability;
            gateptr->pd = 0;
            gateptr->pdn = 0;
        }
		gateptr = gate_list[ff_index];
		if (gateptr->is_lut) gateptr->fault[rand()%gateptr->lut_size] = 1 ;
		gateptr->pd = 1;gateptr->pdn = 0;gateptr->p1 = 0;gateptr->p0 = 0;

		for (ck=1; ck<=clkC; ck++){
			for (i=0; i<gate_numbers; i++){
				gateptr = gate_list[i];
				if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
					continue;
				if ((i == ff_index) && (ck==1))  //skip the faulty gate
					continue;
				for (j=0; j< (gateptr->input_count); j++){
					if ((gateptr->input_list[j]) <= numPi){// if input is Primary Input
						p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
						p1_values[j] = PI_list_sp[gateptr->input_list[j]];
						pd_values[j] = 0; pdn_values[j] = 0;
						continue;
					}
					pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
					pred_gate_number = pred_gateptr->out;

					use_sp = false;
					if (pred_gate_number<=(numPi+numPo))
						use_sp = true;
					//for (k=0; k<fadjlist[pred_gate_number].size(); k++)
					//	if (fadjlist[pred_gate_number][k]->type==7) //is DFF
					//		use_sp = true;
					if (use_sp==true){
						p0_values[j] = 1- pred_gateptr->signal_probability;
						p1_values[j] = pred_gateptr->signal_probability;
						pd_values[j] = 0;
						pdn_values[j] = 0;
					}else{
						p0_values[j] = pred_gateptr->p0;
						p1_values[j] = pred_gateptr->p1;
						pd_values[j] = pred_gateptr->pd;
						pdn_values[j] = pred_gateptr->pdn;
					}
				}
				compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
			}
			int ff_index2;
			for (ff_index2=0; ff_index2<dff_num; ff_index2++){
				gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
				gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
				gptr2->p0 = gptr1->p0;
				gptr2->p1 = gptr1->p1;
				gptr2->pd = gptr1->pd;
				gptr2->pdn = gptr1->pdn;
			}
			SatCK[ff_index][ck-1] = 1;
			int gate_index;
			for (gate_index=0; gate_index<gate_numbers; gate_index++){
				gptr2 = gate_list[gate_index];
				if (gptr2->po_no>=0)
					SatCK[ff_index][ck-1] *= 1 - (gptr2->pd + gptr2->pdn);
			}
			SatCK[ff_index][ck-1] = 1 - SatCK[ff_index][ck-1];
			//if(ck==1) cout << "*** sys-failure-prob for "<< ff_index << " " <<  SatCK[ff_index][ck-1] << endl ;
		}//for (ck=1; ...
		if(gate_list[ff_index]->is_lut){
					for(int i=0;i<gate_list[ff_index]->lut_size;i++){
						gate_list[ff_index]->fault[i] = 0 ;
					}
				}
    }//outmost FOR

	float mul_fact, prev_sum;
	fprintf(resultfp, "Analytical System Failure Results: Cycles 1 through n\n");
	fprintf(resultfp, "node num ::");
	int clk_index;
	for (clk_index=0; clk_index<clkC; clk_index++)
		//if (clk_index%5==0)
		fprintf(resultfp, " clk=%2d ::", clk_index+1);
	fprintf(resultfp, "\n");

	for (i=0; i<gate_numbers; i++){
		gateptr = gate_list[i];
		fprintf(resultfp, "node %4d::",gateptr->out);
		prev_sum = 0;
		mul_fact = 1;
		for (clk_index=0; clk_index<clkC; clk_index++){
			S1_to_CK[i][clk_index] = SatCK[i][clk_index]*mul_fact + prev_sum;
			mul_fact *= 1 - SatCK[i][clk_index];
			prev_sum = S1_to_CK[i][clk_index];
			//if (clk_index%5==0)
			fprintf(resultfp, "%f::", S1_to_CK[i][clk_index]);
		}
		fprintf(resultfp,"\n");
		Sck[i] = S1_to_CK[i][clkC-1];
	}
	fprintf(resultfp,"\n");


	//================

	fprintf(resultfp, "Check if  FFs are in the first part of gate_list \n");
	for (i=0; i<dff_num; i++){
		if (gate_list[i]->type!=7){
			fprintf(resultfp, "gate %d = type %d \n", i, gate_list[i]->type);
			fprintf(resultfp, "ALARM!");
		}
	}
}//system_failure_fpga
/////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: system_failure_clockC_multi_prop
// Purpose:  Computes system failure probability at clock C
// Returns none
void system_failure_clockC_multi_prop(short clkC){ 
    long i,j,k;//,k,m;
    short ff_index;
    short nff_index;
    short PI2POlist_size;
    struct gate *gateptr;
    struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
    short pred_gate_number;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	int ck;
	bool use_sp;
	for (ff_index=0; ff_index<gate_numbers; ff_index++)
	{
        nff_index = gate_list[ff_index]->out;
		cout << "gate:" << ff_index << endl;
        // To get PI2POlist starting from flip-flop nff_index excluding it
        //find_TPsort_from_dffi(nff_index);
        //PI2POlist_size = PI2POlist.size();
        // Set MAX values to ensure each will get correct value
        for (i=0; i<gate_numbers; i++)
        {
            gateptr= gate_list[i];
            gateptr->p0 = 1 - gateptr->signal_probability;
            gateptr->p1 = gateptr->signal_probability;
            gateptr->pd = 0;
            gateptr->pdn = 0;
        }
		gateptr = gate_list[ff_index];
		gateptr->pd = 1;gateptr->pdn = 0;gateptr->p1 = 0;gateptr->p0 = 0;

		for (ck=1; ck<=clkC; ck++)
		{

			for (i=0; i<gate_numbers; i++)
			{
				gateptr = gate_list[i];
				if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
					continue;
				if ((i == ff_index) && (ck==1))  //skip the faulty gate
					continue;
				for (j=0; j< (gateptr->input_count); j++)
				{
					// if input is Primary Input
					if ((gateptr->input_list[j]) <= numPi)
					{
						p0_values[j] = 1- PI_list_sp[gateptr->input_list[j]];
						p1_values[j] = PI_list_sp[gateptr->input_list[j]];
						pd_values[j] = 0; pdn_values[j] = 0;
						continue;
					}
					pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
					pred_gate_number = pred_gateptr->out;

					use_sp = false;
					if (pred_gate_number<=(numPi+numPo))
						use_sp = true;
					//for (k=0; k<fadjlist[pred_gate_number].size(); k++)
					//	if (fadjlist[pred_gate_number][k]->type==7) //is DFF
					//		use_sp = true;
					if (use_sp==true)
					{
						p0_values[j] = 1- pred_gateptr->signal_probability;
						p1_values[j] = pred_gateptr->signal_probability;
						pd_values[j] = 0;
						pdn_values[j] = 0;
					}
					else
					{
						p0_values[j] = pred_gateptr->p0;
						p1_values[j] = pred_gateptr->p1;
						pd_values[j] = pred_gateptr->pd;
						pdn_values[j] = pred_gateptr->pdn;
					}
				}
				compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
			}
			int ff_index2;
			for (ff_index2=0; ff_index2<dff_num; ff_index2++)
			{
				gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
				gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
				gptr2->p0 = gptr1->p0;
				gptr2->p1 = gptr1->p1;
				gptr2->pd = gptr1->pd;
				gptr2->pdn = gptr1->pdn;
			}
			SatCK[ff_index][ck-1] = 1;
			int gate_index;
			for (gate_index=0; gate_index<gate_numbers; gate_index++)
			{
				gptr2 = gate_list[gate_index];
				if (gptr2->po_no>=0)
					SatCK[ff_index][ck-1] *= 1 - (gptr2->pd + gptr2->pdn);
			}
			SatCK[ff_index][ck-1] = 1 - SatCK[ff_index][ck-1];
			if(ck==1) cout << "*** sys-failure-prob for "<< ff_index << " " <<  SatCK[ff_index][ck-1] << endl ;
		}//for (ck=1; ...
    }//outmost FOR



	float mul_fact, prev_sum;
	fprintf(resultfp, "Analytical System Failure Results: Cycles 1 through n\n");
	fprintf(resultfp, "node num ::");
	int clk_index;
	for (clk_index=0; clk_index<clkC; clk_index++)
		//if (clk_index%5==0)
		fprintf(resultfp, " clk=%2d ::", clk_index+1);
	fprintf(resultfp, "\n");

	for (i=0; i<gate_numbers; i++)
	{
		gateptr = gate_list[i];
		fprintf(resultfp, "node %4d::",gateptr->out);
		prev_sum = 0;
		mul_fact = 1;
		for (clk_index=0; clk_index<clkC; clk_index++)
		{
			S1_to_CK[i][clk_index] = SatCK[i][clk_index]*mul_fact + prev_sum;
			mul_fact *= 1 - SatCK[i][clk_index];
			prev_sum = S1_to_CK[i][clk_index];
			//if (clk_index%5==0)
			fprintf(resultfp, "%f::", S1_to_CK[i][clk_index]);
		}
		fprintf(resultfp,"\n");
		Sck[i] = S1_to_CK[i][clkC-1];
	}
	fprintf(resultfp,"\n");


	//================

	fprintf(resultfp, "Check if  FFs are in the first part of gate_list \n");
	for (i=0; i<dff_num; i++)
	{
		if (gate_list[i]->type!=7)
		{
			fprintf(resultfp, "gate %d = type %d \n", i, gate_list[i]->type);
			fprintf(resultfp, "ALARM!");
		}
	}


}//system_failure_clockC_multi_prop
/////////////////////////////////////////////////////////////////////////////////////

// most up-to-dated novel version of D-Calculus, The Q-Calculus.
// can you guess what the Q stands for? :)
void compute_Q_Probability(struct gate* gateptr, float* q0, float* q1, float* qd, float* qdn){
	short i;
	float fval1, fval2;
	switch (gateptr->type) {
	case 0:     //nand
		gateptr->q1= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->q1 *= q1[i];

		gateptr->qd= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->qd *= (q1[i]+qd[i]);
		gateptr->qd -= gateptr->q1;

		gateptr->qdn= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->qdn *= (q1[i]+qdn[i]);
		gateptr->qdn -= gateptr->q1;

		gateptr->q0 = 1- (gateptr->q1 + gateptr->qd + gateptr->qdn);
		// Now reverse values
		fval1 = gateptr->q0; gateptr->q0 = gateptr->q1; gateptr->q1 = fval1;
		fval2 = gateptr->qd; gateptr->qd = gateptr->qdn; gateptr->qdn = fval2;
		break;
	case 1:     //and
		gateptr->q1= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->q1 *= q1[i];

		gateptr->qd= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->qd *= (q1[i]+qd[i]);
		gateptr->qd -= gateptr->q1;

		gateptr->qdn= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->qdn *= (q1[i]+qdn[i]);
		gateptr->qdn -= gateptr->q1;

		gateptr->q0 = 1- (gateptr->q1 + gateptr->qd + gateptr->qdn);
		break;
	case 2:     //nor
		gateptr->q0= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->q0 *= q0[i];

		gateptr->qd= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->qd *= (q0[i]+qd[i]);
		gateptr->qd -= gateptr->q0;

		gateptr->qdn= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->qdn *= (q0[i]+qdn[i]);
		gateptr->qdn -= gateptr->q0;

		gateptr->q1 = 1- (gateptr->q0 + gateptr->qd + gateptr->qdn);
		// Now reverse values
		fval1 = gateptr->q0; gateptr->q0 = gateptr->q1; gateptr->q1 = fval1;
		fval2 = gateptr->qd; gateptr->qd = gateptr->qdn; gateptr->qdn = fval2;
		break;
	case 3:     //or
		gateptr->q0= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->q0 *= q0[i];

		gateptr->qd= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->qd *= (q0[i]+qd[i]);
		gateptr->qd -= gateptr->q0;

		gateptr->qdn= 1;
		for (i=0; i<(gateptr->input_count); i++)
			gateptr->qdn *= (q0[i]+qdn[i]);
		gateptr->qdn -= gateptr->q0;

		gateptr->q1 = 1- (gateptr->q0 + gateptr->qd + gateptr->qdn);
		break;
	case 4:     //xor (2-input xor)
		gateptr->q0 = q0[0]*q0[1] + q1[0]*q1[1] + qd[0]*qd[1] + qdn[0]*qdn[1];
		gateptr->q1 = q0[0]*q1[1] + q1[0]*q0[1] + qd[0]*qdn[1] + qdn[0]*qd[1];
		gateptr->qd = qd[0]*q0[1] + q0[0]*qd[1] + q1[0]*qdn[1] + qdn[0]*q1[1];
		gateptr->qdn = 1- (gateptr->q0 + gateptr->q1 + gateptr->qd);
		break;
	case 5:     //xnor (2-input xnor)
		gateptr->q1 = q0[0]*q0[1] + q1[0]*q1[1] + qd[0]*qd[1] + qdn[0]*qdn[1];
		gateptr->q0 = q0[0]*q1[1] + q1[0]*q0[1] + qd[0]*qdn[1] + qdn[0]*qd[1];
		gateptr->qdn = qd[0]*q0[1] + q0[0]*qd[1] + q1[0]*qdn[1] + qdn[0]*q1[1];
		gateptr->qd = 1- (gateptr->q0 + gateptr->q1 + gateptr->qdn);
		break;
	case 6:     //not
		gateptr->q0 = q1[0];
		gateptr->q1 = q0[0];
		gateptr->qd = qdn[0];
		gateptr->qdn = qd[0];
		break;

	default:    // buff (or dff)
		gateptr->q0 = q0[0];
		gateptr->q1 = q1[0];
		gateptr->qd = qd[0];
		gateptr->qdn = qdn[0];
		break;
	}
}//compute_D_Probability


/////////////////////////////////////////////////////////////////////////////////////
// Function: compute_D_Probability
// Purpose:  Computes D Probability of the given gate from inputs
// Returns none
void compute_D_Probability(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn){
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



/////////////////////////////////////////////////////////////////////////////////////
// Function: compute_D_Probability_with_variance
// Purpose:  Computes D Probability of the given gate from inputs along with variances
// Returns none
void compute_D_Probability_with_variance(struct gate* gateptr, float* p0, float* p1, float* pd, float* pdn, float* p0_var, float* p1_var, float* pd_var, float* pdn_var)
{

    short i;
    float fval1, fval2;
	float tvar1,tvar2,tvar3,tvar4;
    switch (gateptr->type) {
        case 0:     //nand
                gateptr->p1 = 1;
                gateptr->p1_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->p1 *= p1[i];

				if (gateptr->p1==0)
					gateptr->p1_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
			 	  		gateptr->p1_var += gateptr->p1 * (p1_var[i]/p1[i])* gateptr->p1 * (p1_var[i]/p1[i]);

					gateptr->p1_var = sqrt(gateptr->p1_var);
				}


                gateptr->pd = 1;
                gateptr->pd_var = 0;
				for (i=0; i<(gateptr->input_count); i++)
                    gateptr->pd *= (p1[i]+pd[i]);
                //gateptr->pd -= gateptr->p1;

				if (gateptr->pd==0)
					gateptr->pd_var = 0;
				else
				{
					for (i=0; i<(gateptr->input_count); i++)
					{
						tvar1 = sqrt(p1_var[i]*p1_var[i] + pd_var[i]*pd_var[i]);
						gateptr->pd_var += gateptr->pd * (tvar1/(p1[i]+pd[i])) * gateptr->pd *(tvar1/(p1[i]+pd[i]));
					}
					gateptr->pd_var = sqrt(gateptr->pd_var);
        	        gateptr->pd -= gateptr->p1;
					gateptr->pd_var = sqrt(gateptr->pd_var*gateptr->pd_var + gateptr->p1_var*gateptr->p1_var);
				}


                gateptr->pdn= 1;
				gateptr->pdn_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->pdn *= (p1[i]+pdn[i]);

				if (gateptr->pdn==0)
					gateptr->pdn_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
					{
						tvar1 = sqrt(p1_var[i]*p1_var[i] + pdn_var[i]*pdn_var[i]);
						gateptr->pdn_var += gateptr->pdn * (tvar1/(p1[i]+pdn[i])) * gateptr->pdn * (tvar1/(p1[i]+pdn[i]));
					}
					gateptr->pdn_var = sqrt(gateptr->pdn_var);
        	        gateptr->pdn -= gateptr->p1;
					gateptr->pdn_var = sqrt(gateptr->pdn_var*gateptr->pdn_var + gateptr->p1_var*gateptr->p1_var);
				}

                //gateptr->p0 = 1- (gateptr->p1 + gateptr->pd + gateptr->pdn);
				//gateptr->p0_var = sqrt(gateptr->p1_var*gateptr->p1_var + gateptr->pd_var*gateptr->pd_var + gateptr->pdn_var*gateptr->pdn_var);
                gateptr->p0 = 1;
                gateptr->p0_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->p0 *= 1 - p0[i];

				if (gateptr->p0==0)
					gateptr->p0_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
			 	  		gateptr->p0_var += gateptr->p0 * (p0_var[i]/(1-p0[i])) * gateptr->p0 * (p0_var[i]/(1-p0[i]));
					gateptr->p0_var = sqrt(gateptr->p0_var);
				}
				gateptr->p0 = 1 - gateptr->p0;

                // Now reverse values
                fval1 = gateptr->p0; gateptr->p0 = gateptr->p1; gateptr->p1 = fval1;
                fval2 = gateptr->pd; gateptr->pd = gateptr->pdn; gateptr->pdn = fval2;

                fval1 = gateptr->p0_var; gateptr->p0_var = gateptr->p1_var; gateptr->p1_var = fval1;
                fval2 = gateptr->pd_var; gateptr->pd_var = gateptr->pdn_var; gateptr->pdn_var = fval2;

                break;
        case 1:     //and
                gateptr->p1 = 1;
                gateptr->p1_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->p1 *= p1[i];

				if (gateptr->p1==0)
					gateptr->p1_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
			 	  		gateptr->p1_var += gateptr->p1 * (p1_var[i]/p1[i])* gateptr->p1 * (p1_var[i]/p1[i]);

					gateptr->p1_var = sqrt(gateptr->p1_var);
				}


                gateptr->pd = 1;
                gateptr->pd_var = 0;
				for (i=0; i<(gateptr->input_count); i++)
                    gateptr->pd *= (p1[i]+pd[i]);
                //gateptr->pd -= gateptr->p1;

				if (gateptr->pd==0)
					gateptr->pd_var = 0;
				else
				{
					for (i=0; i<(gateptr->input_count); i++)
					{
						tvar1 = sqrt(p1_var[i]*p1_var[i] + pd_var[i]*pd_var[i]);
						gateptr->pd_var += gateptr->pd * (tvar1/(p1[i]+pd[i])) * gateptr->pd *(tvar1/(p1[i]+pd[i]));
					}
					gateptr->pd_var = sqrt(gateptr->pd_var);
        	        gateptr->pd -= gateptr->p1;
					gateptr->pd_var = sqrt(gateptr->pd_var*gateptr->pd_var + gateptr->p1_var*gateptr->p1_var);
				}


                gateptr->pdn= 1;
				gateptr->pdn_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->pdn *= (p1[i]+pdn[i]);

				if (gateptr->pdn==0)
					gateptr->pdn_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
					{
						tvar1 = sqrt(p1_var[i]*p1_var[i] + pdn_var[i]*pdn_var[i]);
						gateptr->pdn_var += gateptr->pdn * (tvar1/(p1[i]+pdn[i])) * gateptr->pdn * (tvar1/(p1[i]+pdn[i]));
					}
					gateptr->pdn_var = sqrt(gateptr->pdn_var);
        	        gateptr->pdn -= gateptr->p1;
					gateptr->pdn_var = sqrt(gateptr->pdn_var*gateptr->pdn_var + gateptr->p1_var*gateptr->p1_var);
				}

                //gateptr->p0 = 1- (gateptr->p1 + gateptr->pd + gateptr->pdn);
				//gateptr->p0_var = sqrt(gateptr->p1_var*gateptr->p1_var + gateptr->pd_var*gateptr->pd_var + gateptr->pdn_var*gateptr->pdn_var);
                gateptr->p0 = 1;
                gateptr->p0_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->p0 *= 1 - p0[i];

				if (gateptr->p0==0)
					gateptr->p0_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
			 	  		gateptr->p0_var += gateptr->p0 * (p0_var[i]/(1-p0[i])) * gateptr->p0 * (p0_var[i]/(1-p0[i]));
					gateptr->p0_var = sqrt(gateptr->p0_var);
				}
				gateptr->p0 = 1 - gateptr->p0;

                // No reversing
                break;
        case 2:     //nor
                gateptr->p0 = 1;
				gateptr->p0_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->p0 *= p0[i];

				if (gateptr->p0==0)
					gateptr->p0_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
					{
						gateptr->p0_var += p0_var[i] * (gateptr->p0/p0[i]) * p0_var[i] * (gateptr->p0/p0[i]);
					}
					gateptr->p0_var = sqrt(gateptr->p0_var);
				}


                gateptr->pd= 1;
				gateptr->pd_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->pd *= (p0[i]+pd[i]);

				if (gateptr->pd==0)
					gateptr->pd_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
					{
						tvar1 = sqrt(p0_var[i]*p0_var[i] + pd_var[i]*pd_var[i]);
						gateptr->pd_var += tvar1 * (gateptr->pd/(p0[i]+pd[i])) * tvar1 * (gateptr->pd/(p0[i]+pd[i]));							}
					gateptr->pd_var = sqrt(gateptr->pd_var);
        	        gateptr->pd -= gateptr->p0;
					gateptr->pd_var = sqrt(gateptr->pd_var*gateptr->pd_var + gateptr->p0_var*gateptr->p0_var);
				}

                gateptr->pdn= 1;
				gateptr->pdn_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->pdn *= (p0[i]+pdn[i]);

				if (gateptr->pdn==0)
					gateptr->pdn_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
					{
						tvar1 = sqrt(p0_var[i]*p0_var[i] + pdn_var[i]*pdn_var[i]);
						gateptr->pdn_var += tvar1 * (gateptr->pdn/(p0[i]+pdn[i])) * tvar1 * (gateptr->pdn/(p0[i]+pdn[i]));
					}
					gateptr->pdn_var = gateptr->pdn * sqrt(gateptr->pdn_var);
					gateptr->pdn -= gateptr->p0;
					gateptr->pdn_var = sqrt(gateptr->pdn_var*gateptr->pdn_var + gateptr->p0_var*gateptr->p0_var);
				}

                //gateptr->p1 = 1- (gateptr->p0 + gateptr->pd + gateptr->pdn);
				//gateptr->p1_var = sqrt(gateptr->p0_var*gateptr->p0_var + gateptr->pd_var*gateptr->pd_var + gateptr->pdn_var*gateptr->pdn_var);
                gateptr->p1 = 1;
                gateptr->p1_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->p1 *= 1 - p1[i];

				if (gateptr->p1==0)
					gateptr->p1_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
			 	  		gateptr->p1_var += gateptr->p1 * (p1_var[i]/(1-p1[i])) * gateptr->p1 * (p1_var[i]/(1-p1[i]));
					gateptr->p1_var = sqrt(gateptr->p1_var);
				}
				gateptr->p1 = 1 - gateptr->p1;

                // Now reverse values
                fval1 = gateptr->p0; gateptr->p0 = gateptr->p1; gateptr->p1 = fval1;
                fval2 = gateptr->pd; gateptr->pd = gateptr->pdn; gateptr->pdn = fval2;

                fval1 = gateptr->p0_var; gateptr->p0_var = gateptr->p1_var; gateptr->p1_var = fval1;
                fval2 = gateptr->pd_var; gateptr->pd_var = gateptr->pdn_var; gateptr->pdn_var = fval2;

                break;
        case 3:     //or
                gateptr->p0 = 1;
				gateptr->p0_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->p0 *= p0[i];

				if (gateptr->p0==0)
					gateptr->p0_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
					{
						gateptr->p0_var += gateptr->p0 * (p0_var[i]/p0[i]) * gateptr->p0 *(p0_var[i]/p0[i]);
					}
					gateptr->p0_var = sqrt(gateptr->p0_var);
				}


                gateptr->pd= 1;
				gateptr->pd_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->pd *= (p0[i]+pd[i]);

				if (gateptr->pd==0)
					gateptr->pd_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
					{
						tvar1 = sqrt(p0_var[i]*p0_var[i] + pd_var[i]*pd_var[i]);
						gateptr->pd_var += gateptr->pd * (tvar1/(p0[i]+pd[i])) * gateptr->pd * (tvar1/(p0[i]+pd[i]));							}
					gateptr->pd_var = sqrt(gateptr->pd_var);
        	        gateptr->pd -= gateptr->p0;
					gateptr->pd_var = sqrt(gateptr->pd_var*gateptr->pd_var + gateptr->p0_var*gateptr->p0_var);
				}

                gateptr->pdn= 1;
				gateptr->pdn_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->pdn *= (p0[i]+pdn[i]);

				if (gateptr->pdn==0)
					gateptr->pdn_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
					{
						tvar1 = sqrt(p0_var[i]*p0_var[i] + pdn_var[i]*pdn_var[i]);
						gateptr->pdn_var += gateptr->pdn * (tvar1/(p0[i]+pdn[i])) * gateptr->pdn * (tvar1/(p0[i]+pdn[i]));
					}
					gateptr->pdn_var = gateptr->pdn * sqrt(gateptr->pdn_var);
					gateptr->pdn -= gateptr->p0;
					gateptr->pdn_var = sqrt(gateptr->pdn_var*gateptr->pdn_var + gateptr->p0_var*gateptr->p0_var);
				}

                //gateptr->p1 = 1- (gateptr->p0 + gateptr->pd + gateptr->pdn);
				//gateptr->p1_var = sqrt(gateptr->p0_var*gateptr->p0_var + gateptr->pd_var*gateptr->pd_var + gateptr->pdn_var*gateptr->pdn_var);
                gateptr->p1 = 1;
                gateptr->p1_var = 0;
                for (i=0; i<(gateptr->input_count); i++)
                    gateptr->p1 *= 1 - p1[i];

				if (gateptr->p1==0)
					gateptr->p1_var = 0;
				else
				{
	                for (i=0; i<(gateptr->input_count); i++)
			 	  		gateptr->p1_var += gateptr->p1 * (p1_var[i]/(1-p1[i])) * gateptr->p1 * (p1_var[i]/(1-p1[i]));
					gateptr->p1_var = sqrt(gateptr->p1_var);
				}
				gateptr->p1 = 1 - gateptr->p1;
                // No reversing

                break;

        case 4:     //xor (2-input xor)
                gateptr->p0 = p0[0]*p0[1] + p1[0]*p1[1] + pd[0]*pd[1] + pdn[0]*pdn[1];
				tvar1 = p0[0]*p0[1]*sqrt((p0_var[0]/p0[0])*(p0_var[0]/p0[0]) + (p0_var[1]/p0[1])*(p0_var[1]/p0[1]));
				tvar2 = p1[0]*p1[1]*sqrt((p1_var[0]/p1[0])*(p1_var[0]/p1[0]) + (p1_var[1]/p1[1])*(p1_var[1]/p1[1]));
				tvar3 = pd[0]*pd[1]*sqrt((pd_var[0]/pd[0])*(pd_var[0]/pd[0]) + (pd_var[1]/pd[1])*(pd_var[1]/pd[1]));
				tvar4 = pdn[0]*pdn[1]*sqrt((pdn_var[0]/pdn[0])*(pdn_var[0]/pdn[0]) + (pdn_var[1]/pdn[1])*(pdn_var[1]/pdn[1]));
				gateptr->p0_var = sqrt(tvar1*tvar1 + tvar2*tvar2 + tvar3*tvar3 + tvar4*tvar4);

                gateptr->p1 = p0[0]*p1[1] + p1[0]*p0[1] + pd[0]*pdn[1] + pdn[0]*pd[1];
				tvar1 = p0[0]*p1[1]*sqrt((p0_var[0]/p0[0])*(p0_var[0]/p0[0]) + (p1_var[1]/p1[1])*(p1_var[1]/p1[1]));
				tvar2 = p1[0]*p0[1]*sqrt((p1_var[0]/p1[0])*(p1_var[0]/p1[0]) + (p0_var[1]/p0[1])*(p0_var[1]/p0[1]));
				tvar3 = pd[0]*pdn[1]*sqrt((pd_var[0]/pd[0])*(pd_var[0]/pd[0]) + (pdn_var[1]/pdn[1])*(pdn_var[1]/pdn[1]));
				tvar4 = pdn[0]*pd[1]*sqrt((pdn_var[0]/pdn[0])*(pdn_var[0]/pdn[0]) + (pd_var[1]/pd[1])*(pd_var[1]/pd[1]));
				gateptr->p1_var = sqrt(tvar1*tvar1 + tvar2*tvar2 + tvar3*tvar3 + tvar4*tvar4);

                gateptr->pd = pd[0]*p0[1] + p0[0]*pd[1] + p1[0]*pdn[1] + pdn[0]*p1[1];
				tvar1 = pd[0]*p0[1]*sqrt((pd_var[0]/pd[0])*(pd_var[0]/pd[0]) + (p0_var[1]/p0[1])*(p0_var[1]/p0[1]));
				tvar2 = p0[0]*pd[1]*sqrt((p0_var[0]/p0[0])*(p0_var[0]/p0[0]) + (pd_var[1]/pd[1])*(pd_var[1]/pd[1]));
				tvar3 = p1[0]*pdn[1]*sqrt((p1_var[0]/p1[0])*(p1_var[0]/p1[0]) + (pdn_var[1]/pdn[1])*(pdn_var[1]/pdn[1]));
				tvar4 = pdn[0]*p1[1]*sqrt((pdn_var[0]/pdn[0])*(pdn_var[0]/pdn[0]) + (p1_var[1]/p1[1])*(p1_var[1]/p1[1]));
				gateptr->p1_var = sqrt(tvar1*tvar1 + tvar2*tvar2 + tvar3*tvar3 + tvar4*tvar4);

                gateptr->pdn = 1- (gateptr->p0 + gateptr->p1 + gateptr->pd);
				gateptr->pdn_var = sqrt(gateptr->p0_var*gateptr->p0_var + gateptr->p1_var*gateptr->p1_var + gateptr->pd_var*gateptr->pd_var);
                break;

        case 5:     //xnor (2-input xnor)
                gateptr->p1 = p0[0]*p0[1] + p1[0]*p1[1] + pd[0]*pd[1] + pdn[0]*pdn[1];
				tvar1 = p0[0]*p0[1]*sqrt((p0_var[0]/p0[0])*(p0_var[0]/p0[0]) + (p0_var[1]/p0[1])*(p0_var[1]/p0[1]));
				tvar2 = p1[0]*p1[1]*sqrt((p1_var[0]/p1[0])*(p1_var[0]/p1[0]) + (p1_var[1]/p1[1])*(p1_var[1]/p1[1]));
				tvar3 = pd[0]*pd[1]*sqrt((pd_var[0]/pd[0])*(pd_var[0]/pd[0]) + (pd_var[1]/pd[1])*(pd_var[1]/pd[1]));
				tvar4 = pdn[0]*pdn[1]*sqrt((pdn_var[0]/pdn[0])*(pdn_var[0]/pdn[0]) + (pdn_var[1]/pdn[1])*(pdn_var[1]/pdn[1]));
				gateptr->p1_var = sqrt(tvar1*tvar1 + tvar2*tvar2 + tvar3*tvar3 + tvar4*tvar4);

                gateptr->p0 = p0[0]*p1[1] + p1[0]*p0[1] + pd[0]*pdn[1] + pdn[0]*pd[1];
				tvar1 = p0[0]*p1[1]*sqrt((p0_var[0]/p0[0])*(p0_var[0]/p0[0]) + (p1_var[1]/p1[1])*(p1_var[1]/p1[1]));
				tvar2 = p1[0]*p0[1]*sqrt((p1_var[0]/p1[0])*(p1_var[0]/p1[0]) + (p0_var[1]/p0[1])*(p0_var[1]/p0[1]));
				tvar3 = pd[0]*pdn[1]*sqrt((pd_var[0]/pd[0])*(pd_var[0]/pd[0]) + (pdn_var[1]/pdn[1])*(pdn_var[1]/pdn[1]));
				tvar4 = pdn[0]*pd[1]*sqrt((pdn_var[0]/pdn[0])*(pdn_var[0]/pdn[0]) + (pd_var[1]/pd[1])*(pd_var[1]/pd[1]));
				gateptr->p0_var = sqrt(tvar1*tvar1 + tvar2*tvar2 + tvar3*tvar3 + tvar4*tvar4);

                gateptr->pdn = pd[0]*p0[1] + p0[0]*pd[1] + p1[0]*pdn[1] + pdn[0]*p1[1];
				tvar1 = pd[0]*p0[1]*sqrt((pd_var[0]/pd[0])*(pd_var[0]/pd[0]) + (p0_var[1]/p0[1])*(p0_var[1]/p0[1]));
				tvar2 = p0[0]*pd[1]*sqrt((p0_var[0]/p0[0])*(p0_var[0]/p0[0]) + (pd_var[1]/pd[1])*(pd_var[1]/pd[1]));
				tvar3 = p1[0]*pdn[1]*sqrt((p1_var[0]/p1[0])*(p1_var[0]/p1[0]) + (pdn_var[1]/pdn[1])*(pdn_var[1]/pdn[1]));
				tvar4 = pdn[0]*p1[1]*sqrt((pdn_var[0]/pdn[0])*(pdn_var[0]/pdn[0]) + (p1_var[1]/p1[1])*(p1_var[1]/p1[1]));
				gateptr->pdn_var = sqrt(tvar1*tvar1 + tvar2*tvar2 + tvar3*tvar3 + tvar4*tvar4);

                gateptr->pd = 1- (gateptr->p0 + gateptr->p1 + gateptr->pdn);
				gateptr->pd_var = sqrt(gateptr->p0_var*gateptr->p0_var + gateptr->p1_var*gateptr->p1_var + gateptr->pdn_var*gateptr->pdn_var);

				break;
        case 6:     //not
                gateptr->p0 = p1[0];	gateptr->p0_var = p1_var[0];
                gateptr->p1 = p0[0];	gateptr->p1_var = p0_var[0];
                gateptr->pd = pdn[0];	gateptr->pd_var = pdn_var[0];
                gateptr->pdn = pd[0];	gateptr->pdn_var = pd_var[0];
                break;
        default:    // buff (or dff)
                gateptr->p0 = p0[0];	gateptr->p0_var = p0_var[0];
                gateptr->p1 = p1[0];	gateptr->p1_var = p1_var[0];
                gateptr->pd = pd[0];	gateptr->pd_var = pd_var[0];
                gateptr->pdn = pdn[0];	gateptr->pdn_var = pdn_var[0];
                break;
    }
	//fprintf(resultfp, "node %d: , gate_type=%d p0_var=%f, p1_var=%f, pd_var=%f, pdn_var=%f\n", gateptr->out, gateptr->type, gateptr->p0_var, gateptr->p1_var, gateptr->pd_var, gateptr->pdn_var);
	if ((gateptr->p0_var > 1) || (gateptr->p0_var < 0)) config->variance_alarm = true; //fprintf(resultfp, "Variance ALARM \n");
	if ((gateptr->p1_var > 1) || (gateptr->p1_var < 0)) config->variance_alarm = true; //fprintf(resultfp, "Variance ALARM \n");
	if ((gateptr->pd_var > 1) || (gateptr->pd_var < 0)) config->variance_alarm = true; //fprintf(resultfp, "Variance ALARM \n");
	if ((gateptr->pdn_var > 1) || (gateptr->pdn_var < 0)) config->variance_alarm = true; //fprintf(resultfp, "Variance ALARM \n");

}//compute_D_Probability_with_variance

/////////////////////////////////////////////////////////////////////////////////////
// Function: find_TPsort_from_dffi
// Purpose:  finds Topological Sort list of nodes starting from dffi
// Returns none
void find_TPsort_from_dffi(short nff_index)
{

    int i,j;
    deque <short> tmp_constant;
	struct gate *p;

    PI2POlist.clear();
	//fprintf(resultfp, "\n\nfinds Topological Sort list of nodes starting from dff %d\n", nff_index);
	color.clear();
	for(i=1; i<=nodes; i++)
	{
		color.push_back('w');
		pi[i] = config->MAX;
	}
    for (i=0; i<dff_num; i++)
        color[dfflist_ptr[i]->q]='g';

    //cout << endl << "Reachable nodes from Input i: " << endl;
	//fprintf(resultfp, "Reachable nodes from Input i:\n");
	// 2nd Apply DFS starting from node nff_index
	pi[nff_index] = 0;
    final_time_stamp = 0;
	dfs_fvisit(fadjlist, nff_index);
//  cout << endl << "PrinPredecessor matrix for fault at " << sa << endl;

/*	for(i=1; i<nodes; i++)
	{
		if (pi[i] != MAX)
		{
			cout << i << " ";
			fprintf(resultfp, "%d, ", i);
		}
	}
	fprintf(resultfp, "\n");
    cout << endl;
*/
    //ReachNodesFromPOi(radjlist, POj);
    //Find all reachable nodes from all reachable outputs of nff_index
    //1st check Primary Outputs


/*    for (i=(numPi+1); i<=(numPi+numPo); i++)
        if (pi[i] != MAX)
            ReachNodesFromPOi(radjlist, i);
    //2nd check outputs entering flip-flops
    int g2dnumber;
    for (i=0; i<dff_num; i++)
    {
        g2dnumber = dfflist_ptr[i]->d;
        if (pi[g2dnumber] != MAX)
            ReachNodesFromPOi(radjlist, g2dnumber);
    }
*/
    //cout << endl << "Reachable Nodes from all reachable outputs of nff_index " << endl;
	//fprintf(resultfp, "\nReachable Nodes from from all reachable outputs of nff_index:\n");
	for(i=1; i<nodes; i++)
	{
	    //cout << i << " ";
		//fprintf(resultfp, "%d ", i);
		if (pi[i] != config->MAX)
		{
			tmp_constant.clear();
			tmp_constant.push_back(i);
			PI2POlist.push_back(tmp_constant);
		}
        //cout << reachOp[i][j] << " ";
	}
    //cout << endl;
    //fprintf(resultfp, "\n");

	/*cout << "Common Nodes are:" << endl;
	fprintf(resultfp, "Common Nodes are: \n");
	for (i=0; i<PI2POlist.size(); i++)
	{
		cout << PI2POlist[i][0] << " ";
		fprintf(resultfp,"%d, ", PI2POlist[i][0]);
	}
	cout << endl;
	fprintf(resultfp, "\n");*/


	//sort the PI2POlist
	short PI2POlist_size = PI2POlist.size();
    //cout << "dfs_fnum BEFORE sorting" << endl;
    //for (i=0; i < (PI2POlist_size); i++)
        //cout << dfs_fnum[PI2POlist[i][0]] << " ";
    //cout << endl << endl;
    for (i=0; i < (PI2POlist_size-1); i++)
    {
		for (j=i+1; j<PI2POlist_size; j++)
		{
			if (dfs_fnum[PI2POlist[i][0]] < dfs_fnum[PI2POlist[j][0]])
			{
                short val = PI2POlist[i][0];
				PI2POlist[i][0] = PI2POlist[j][0];
                PI2POlist[j][0] = val;
			}
		}
    }
    /*cout << "Topological sort of common Nodes are:" << endl;
	fprintf(resultfp,"Topological sort of common Nodes are:\n");
	for (i=0; i<PI2POlist.size(); i++)
	{
	    cout << PI2POlist[i][0] << " ";
		fprintf(resultfp, "%d, ", PI2POlist[i][0]);
	}
	cout << endl;
	fprintf(resultfp, "\n");*/


	//Here we creating forward-adjancy list for on-path gates
	for (i=0; i<PI2POlist.size(); i++)
	{
        for (j=0; j<fadjlist[PI2POlist[i][0]].size(); j++)
        {
            //cout << PI2POlist[i][0] << ":: ";
            p = fadjlist[PI2POlist[i][0]][j];
            if (pi[p->out] != config->MAX)
            {
                PI2POlist[i].push_back(p->out);
				//cout << p->out << " ";
            }
			//cout << endl;
        }
	}
/////////////////////Print/////////////////////////////
/*    cout << endl <<"-------------------" << endl << "all common nodes from node:" << nff_index  << endl;

    fprintf(resultfp,"\n -------------------\n All common nodes from node: %d \n", nff_index);
    PI2POlist_size = PI2POlist.size();
    for (i=0; i<PI2POlist_size; i++)
    {
        cout<< PI2POlist[i][0] << " :: ";
        fprintf(resultfp, "%d\t:: ", PI2POlist[i][0]);
        for (j=1; j<PI2POlist[i].size(); j++)
        {
            cout<< PI2POlist[i][j] << " ";
            fprintf(resultfp,"%d, ", PI2POlist[i][j]);
        }
        cout << endl;
        fprintf(resultfp, "\n");
    }*/

}//find_TPsort_from_dffi


/////////////////////////////////////////////////////////////////////////////////////
// Function: arange_gates
// Purpose:  Arranges all gates Except DFFs
// Returns none
void arange_gates(void){
    short i,j,k;
    struct gate *gptr1, *gptr2;
    short last_swap=0;
    for (short snum=0; snum<(gate_numbers+2); snum++){
        for (i=dff_num; i<gate_numbers; i++){
            for (j=i+1; j<gate_numbers; j++){
                gptr1= gate_list[i];
                gptr2= gate_list[j];
                for (k=0; k<gptr1->input_count; k++)
                    if (gptr1->input_list[k] == gptr2->out){
						gate_list[i] = gptr2;
                        gate_list[j] = gptr1;
                        last_swap = snum;
                        break;
                    }
            }
        }
        if (last_swap != snum)
            break;
    }
    if (last_swap >= gate_numbers){
        cout << "[Warning] circuit is not arranged yet, probably asyncronized loops or floated IO. check input netlist." << endl;
		cout << "          did you add -s option for DC-generated netlist??" << endl;
        fprintf(resultfp, "ALARM: It needs so more loop to be arranged!!!!!\n");
        //cin >> k;
		exit(1);
    }
}//arange_gates


/////////////////////////////////////////////////////////////////////////////////////
// Function: arange_gates_extract_feedbacks
// Purpose:  Arranges all gates Except DFFs
// Returns none
void arange_gates_extract_feedbacks(void){
    short i,j,k;
    short snum;
    struct gate *gptr1, *gptr2;
    short last_swap=0;


	for (i=0; i<gate_numbers; i++)
		gate_list[i]->site_out_level = 0;

	bool first_level;
    for (i=0; i<dff_num; i++)
    {
        gptr1 = dfflist_ptr[i]->dgateptr;
		gptr1->site_out_level = 1;
	}

	for (i=0; i<gate_numbers; i++)
	{
		first_level = true;
		gptr1= gate_list[i];
		for (k=0; k<gptr1->input_count; k++)
	        if (gptr1->input_list[k] > numPi)
			{
				if (fadjlist_ptr[gptr1->input_list[k]]->type!=7)
				{
					first_level = false;
					break;
				}
			}

		if (first_level == true)
			gptr1->site_out_level = 1;
	}

	//cout << "inja1" << endl;
	bool second_level;
	int next_level;
	for (j=0; j<gate_numbers; j++)
	{
		for (i=0; i<gate_numbers; i++)
		{
			second_level = false;
			next_level = 0;
			gptr1= gate_list[i];
			if (gptr1->site_out_level>0)
				continue;
			//cout << "inja2" << endl;
			for (k=0; k<gptr1->input_count; k++)
			{
				//cout << "inja3" << endl;
	    	    if (gptr1->input_list[k] <= numPi)
				{
					second_level = true;
					next_level = 1;
					continue;
					//cout << "inja4" << endl;
				}
				//cout << "inja5" << ", gptr1->out=" << gptr1->out << ", gptr1->input_list[k]=" << gptr1->input_list[k] << endl;
				if (fadjlist_ptr[gptr1->input_list[k]]->site_out_level > next_level)
				{
					//cout << "inja6" << endl;
					next_level = fadjlist_ptr[gptr1->input_list[k]]->site_out_level + 1;
					second_level = true;
					//cout << "inja7" << endl;
				}
				//cout << "inja8" << endl;

			}
			//cout << "inja9" << endl;

			if (second_level == true)
				gptr1->site_out_level = next_level;
		}
	}



    for (snum=0; snum<(gate_numbers+5); snum++)
    {
        for (i=dff_num; i<gate_numbers; i++)
        {
            for (j=i+1; j<gate_numbers; j++)
            {
                gptr1= gate_list[i];
                gptr2= gate_list[j];
                for (k=0; k<gptr1->input_count; k++)
                    if (gptr1->input_list[k] == gptr2->out)
                    {
                        gate_list[i] = gptr2;
                        gate_list[j] = gptr1;
						cout << "G1:" << gptr1->out << ",level=" << gptr1->site_out_level << ",     G2:" << gptr2->out  << ",level=" << gptr2->site_out_level << endl;
                        last_swap = snum;
                        break;
                    }
            }
        }
        if (last_swap != snum)
            break;
    }
    if (last_swap >= gate_numbers)
    {
        cout << "ALARM: It needs so more loop to be arranged!!!!!" << endl;
        fprintf(resultfp, "ALARM: It needs so more loop to be arranged!!!!!\n");
        cin >> k;
		fclose(resultfp);
		release_all_malloc();
		exit(1);
    }
}//arange_gates_extract_feedbacks
/////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////
// Function: extract_feedbacks
// Purpose:  Arranges all gates Except DFFs
// Returns none
void extract_feedbacks(void){
    int i,j;
    class gate *gptr1, *gptr2;
    int last_swap=0;

    time_t t1,t2,t3;
	double initial_time, final_time, total_time;
    initial_time = clock(); time(&t1);

    deque <int> tmp_constant;

    PI2POlist.clear();
	color.clear();

	for(i=1; i<=nodes; i++)
	{
		color.push_back('w');
		pi[i] = config->MAX;
	}

	//pi[node_index] = 0;
    final_time_stamp = 0;

	for(i=1; i<=nodes; i++)
	{
		if (color[i]=='w')
		{
			pi[i] = 0;
			dfs_fvisit(fadjlist, i);
		}
	}
	cout << "pi's" << endl;
	fprintf(resultfp, "pi's\n\n");
	for(i=1; i<=nodes; i++)
	{
		cout << "pi[" << i << "]=" << pi[i] << endl;
		fprintf(resultfp, "pi[%d] = %d \n", i, pi[i]); 
	}
	fprintf(resultfp, "\n dfs_fnum's \n");
	cout << "dfs_fnum's" << endl;
	for(i=1; i<=nodes; i++)
	{
		cout << "dfs_fnum[" << i << "]=" << dfs_fnum[i] << endl;
		fprintf(resultfp, "dfs_fnum[%d] = %d \n", i, dfs_fnum[i]); 
	}
	//release_all_malloc();
	//exit(1);
}//extract_feedbacks
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: system_failure_simulation
// Purpose:  Computes system failure probability at first clock using simulation method (Applying large amount of vectors)
// Returns none
void system_failure_simulation(long max_iteration){
    long i,j,ck;
	long it, total_it;
    short ff_index;
    //short nff_index;
    long rand_value;
    short rand_logic;
    struct gate *gateptr;
    struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
	float error_tolerance = 0.01;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	long step_iteration = 100;
	float sim_epp, sim_var;

    sim_gate_numbers = gate_numbers;
/*	if ((gate_numbers-dff_num) > 700)
		sim_gate_numbers = 700 + dff_num;
*/
    if (PI_list==NULL)
        PI_list = new short[numPi+1];
        //PI_list = (short*)malloc((numPi+1)*sizeof(short));
	cout << "Number of FFs " << dff_num << endl;

	///////////////////////////////////////////////////////////////
    for (ff_index=0; ff_index<sim_gate_numbers; ff_index++)
    {
		cout << "Simulating gate# " << ff_index << " ... ";
		if ((gate_list[ff_index]->is_dff == 1) || ( gate_list[ff_index]->out <= (numPi+numPo)))
		{
    	    S_sim[ff_index] = 1; //Initialize the S_sim[ff_index]
			S_sim_var[ff_index] = 0;
			cout << endl;
			continue;
		}
		S_sim[ff_index] = 0; //Initialize the S_sim[ff_index]
		S_sim_var[ff_index] = 0;


        //nff_index = dfflist_ptr[ff_index]->q;
        // Set MAX values to ensure each will get correct value
		total_it = 0;
        ex_number = 0;
        //srand(0);
        //srandom(0);
		do
		{
	        for (it=0; it<step_iteration; it++)
    	    {// begin iteration for ff_index flip-flop
        	    for (i=0; i<gate_numbers; i++)
            	{
    	            gateptr = gate_list[i];
	    	        gateptr->p0 = 0; gateptr->p1 = 0;
		            gateptr->pd = 0; gateptr->pdn = 0;
	            }
    	        for (i=1; i<=(numPi); i++)
        	    {
	                rand_value = random();                // random function on Unix (Better Performance)
    	            //rand_value = rand();                    // random function on Windows &Unix
        	        //Assigning random loginc to Primary Input
            	    rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
                	PI_list[i] = rand_logic;
	            }
    	        for (i=0; i<dff_num; i++)
        	    {
            	    rand_value = random();                // random function on Unix (Better Performance)
	                //rand_value = rand();                    // random function on Windows &Unix
    	            rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
        	        if ((dfflist_ptr[i]->q)==(gate_list[ff_index]->out))
            	        continue;
	                gateptr = dfflist_ptr[i]->dgateptr;
    	            gateptr->p1 = rand_logic;
        	        gateptr->p0 = 1 - rand_logic;
            	    gateptr->pd = 0; gateptr->pdn = 0;
	            }
    	        //gateptr = dfflist_ptr[ff_index]->dgateptr;
	            gateptr = gate_list[ff_index];
    	        gateptr->p1 = 0; gateptr->p0 = 0;
	            gateptr->pd = 1; gateptr->pdn = 0;
				for (i=0; i<gate_numbers; i++)
				{
					gateptr = gate_list[i];
					if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
						continue;
					//keep the ff_index still faulty only for the first clock cycle
					//if (gateptr->out == gate_list[ff_index]->out) // change it to && (ck==1)
					if (i == ff_index)// && (ck==1)) // change it to && (ck==1)
						continue;
					for (j=0; j< (gateptr->input_count); j++)
					{// Input traversing
						// if input is Primary Input
						if ((gateptr->input_list[j]) <= numPi)
						{
							p1_values[j] = PI_list[gateptr->input_list[j]];
							p0_values[j] = 1 - PI_list[gateptr->input_list[j]];
							pd_values[j] = 0; pdn_values[j] = 0;
							continue;
						}
						pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
						p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
						pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
					}// end of input traversing
					compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values); //system_failure_simulation
				}// End of gate_list
				int ff_index2;
				bool is_failure = false;
				for (ff_index2=0; ff_index2<dff_num; ff_index2++)
				{
					gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
					gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
					gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
					gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
					//uncomment when not using multicylce
					if ((gptr2->pd == 1)||(gptr2->pdn == 1))
						is_failure =true;
				}
				for (i=(numPi+1); i<=(numPi+numPo); i++)
				{
					if (is_failure==true)
						break;
					gptr1 = fadjlist_ptr[i];
					if ((gptr1->pd == 1)||(gptr1->pdn == 1))
					{
						is_failure = true;
						break;
					}
				}
				if (is_failure==true)
					S_sim[ff_index]++;
	            ex_number++;
			} //for
			total_it += step_iteration;
			sim_epp  = S_sim[ff_index] / (float)total_it;
			sim_var = sqrt((sim_epp * (1 - sim_epp))/(float)total_it);
        } while ((total_it<max_iteration) && ((3*sim_var) > error_tolerance)); //end of iteration for flip-flop ff_index
		fprintf(resultfp, "Node %d: Sims iterated  = %d \n", gate_list[ff_index]->out, total_it);
		S_sim[ff_index] = sim_epp;
		S_sim_var[ff_index] = sim_var;

		cout << "simulated for: " << total_it << endl;
    }
	///////////////////////////////////////////////////////////////


	float diff_partial=0, diff_total=0;
	short not_ff_po=0;

    cout << endl << "SIMULATED VECTOR S for firs clock" << endl;
    fprintf(resultfp, "\n NODES \t\t S[]\t\t S_sim[]\t\t Difference\n");
    for (j=0; j<sim_gate_numbers; j++)
    {
        cout << "node " << gate_list[j]->out << ": " << S_sim[j] << "::" << S[j] << endl;
        fprintf(resultfp, "node %4d: \t%f :: \t%f :: \t%f\n", gate_list[j]->out, S[j], S_sim[j], fabs(S[j] - S_sim[j]));
		diff_total += fabs(S_sim[j]-S[j]);
		if ((gate_list[j]->is_dff == 0) && ( gate_list[j]->out > (numPi+numPo)))
		{
			diff_partial += fabs(S_sim[j]-S[j]);
			not_ff_po++;
		}
    }


	diff_partial = diff_partial / not_ff_po;
	diff_total = diff_total/sim_gate_numbers;
	fprintf(resultfp,"///////////////////////// \n");
	fprintf(resultfp,"Single Cycle \n");
    cout << "Total difference: " << diff_total<< endl;
    fprintf(resultfp, "Total difference: %f \n", diff_total);
    cout << "Parital Difference: " << diff_partial<< endl;
    fprintf(resultfp, "Partial Difference: %f \n", diff_partial);
	// we include ffs in accuracy computation but we do not include dff simulation in our sim time
	fprintf(resultfp, "\n not_ff_po = %d, sim_gate_numbers = %d \n", not_ff_po, sim_gate_numbers);
	if (sim_gate_numbers != gate_numbers)
		sim_gate_numbers -= dff_num;

    fprintf(resultfp, "\n\\\\\\\\\\\\\\\\\\\n");
    fprintf(resultfp, "Variance Difference\n");
    fprintf(resultfp, "\n NODES \t\t SysVar\t\t\t SimVar \t\tVar. Diff. \t\t Error \t\t\t EPP Diff.\n");

	float z_alpha_half = 2.576;
    for (j=0; j<sim_gate_numbers; j++)
    {
        fprintf(resultfp, "node %4d: \t%f :: \t%f :: \t%f :: \t%f :: \t%f\n", gate_list[j]->out, gate_list[j]->epp_var, S_sim_var[j], fabs(gate_list[j]->epp_var - S_sim_var[j]), gate_list[j]->epp_var*z_alpha_half, fabs(S[j] - S_sim[j]));
    }

    fprintf(resultfp, "\n///////////////////////////////////////////////////////////////////");
    fprintf(resultfp, "\n/////////////////////////////////////////////////////////////////// \n");

}//system_failure_simulation
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: system_failure_simulation_with_let
// Purpose:  provides mont-carlo multi-cycle simulation which supports LET at first cycle
// we used this function to validate our results for dsn-2010 & TC-paper
// update: april-2010: added vcd support. (enabled through config->use_vcd )
// Returns none
void system_failure_simulation_with_let(long max_iteration, long step_iteration, long clk, vcd* signals){
	if(config->verbose){
		cout << "-----------------------------------------------------------" << endl ;
		cout << "	system_failure_simulation_with_let" << endl ;
		cout << "-----------------------------------------------------------" << endl ;
		cout << "Number of FFs " << dff_num << endl;
	}
	
	float SER_PER_GATE_PO_FF_CLK1 = 0, SER_PER_GATE_FF_CLK1=0, SER_PER_GATE_PO_CLK1=0;
	bool is_seu = false;
	int i,j,first_clock_failure=0,first_clock_total_failure=0,total_iteration=0, total_failure=0, first_clock_iteration=0;
	long it, total_it;
	int seu=0,lseu;
	short ff_index;
	long rand_value;

	short rand_logic;
	struct gate *gateptr;
	struct gate *pred_gateptr;
	struct gate* gptr1,*gptr2;
	float error_tolerance = 0.01;
	float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
	float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	float sim_epp, sim_var;

	
	struct gate tgateptr;
	struct event* eventptr;
	int max_event_size;

	int **PI_list_MATRIX = new int *[numPi+1] ;
	for ( int i = 0 ; i <= numPi ; i++ ){
		PI_list_MATRIX[i] = new int[clk] ;
	}

	sim_gate_numbers = gate_numbers;
	if (PI_list==NULL) PI_list = new short[numPi+1];
	for (ff_index=0; ff_index<sim_gate_numbers; ff_index++){
		if(config->verbose) cout << "Simulating gate# " << ff_index << " type= " << gate_name(gate_list[ff_index])<< " ... "  ;
		if ( gate_list[ff_index]->out <= (numPi+numPo)){ //multicycle
			S_sim_clk[ff_index] = 1; //Initialize the S_sim_clk[ff_index]
			S_sim_single_cycle_po[ff_index] = 1;
			S_sim_single_cycle_ff[ff_index] = 1;
			S_sim_single_cycle_po_ff[ff_index] = 1;
			S_sim_clk_var[ff_index] = 0;	
			if(config->verbose) cout << endl ;
			continue;
		}
		S_sim_clk[ff_index] = 0; //Initialize the S_sim_clk[ff_index]
		S_sim_clk_var[ff_index] = 0;
		S_sim_single_cycle_po[ff_index] = 0;
		S_sim_single_cycle_ff[ff_index] = 0;
		S_sim_single_cycle_po_ff[ff_index] = 0;
		

		// Set MAX values to ensure each will get correct value
		total_it = 0;
		ex_number = 0;
		//srandom(0);
		do{
			for (it=0; it<step_iteration; it++){// begin iteration for ff_index flip-flop
				for (i=0; i<gate_numbers; i++){
					gateptr = gate_list[i];
					gateptr->p0 = 0; gateptr->p1 = 0;
					gateptr->pd = 0; gateptr->pdn = 0;
				}
				//cout << endl ;
				if(config->use_vcd) signals->randomizeCounter();
				for (i=1; i<=(numPi); i++){
					if(config->use_vcd){
						// snippet-A: assigning signal values from vcd, ignoring sequence.
						//short signal_value = signals->getNextLogicValue(i); 
						//PI_list[i] = signal_value;
						//cout << "dbg)" << i << " " << PI_list[i] << " " << PI_list_sp[i] << endl ;

						//snippet-B: assigning signal values from vcd, respecting sequence.
						//cout << i << "(" << signals->getSignalName(i) << ")->" ;
						//for(int j=0;j<clk;j++){
						//	PI_list_MATRIX[i][j] = signals->getNextLogicValue(i); 	
						//	//cout << PI_list_MATRIX[i][j] ;
						//}
						PI_list_MATRIX[i][0] = signals->getNextLogicValue(i); 	
						//cout << endl ;
						//PI_list[i] = PI_list_MATRIX[i][0];
					}else{
						rand_value = random();
						//Assigning random loginc to Primary Input
						rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
						PI_list[i] = rand_logic;
					}
					
					// Forwording the PI's value to its successors
					short node_adj_size =fadjlist[i].size();
					for (j=0; j<node_adj_size; j++){
						gateptr = fadjlist[i][j];
						short pred_gate_node_number;
						for (int k=0; k<gateptr->input_count; k++){
							//pred_gate_node_number = radjlist[gateptr->out][k];
							pred_gate_node_number = gateptr->input_list[k]; // this should be right
							if (pred_gate_node_number==i)
								if(config->use_vcd){
									gateptr->input_values[k] = PI_list_MATRIX[i][0];
								}else{
									gateptr->input_values[k] = PI_list[i];
								}
								
						}
					}
				}

				//Assign random values to DFFs
				for (i=0; i<dff_num; i++){
					if(false){//Assigning DFF randomly or from VCD (for low clock simulation) file does not make any significance change in soft error rate (~1-3%), we can ignore DFFs for now.
					//if(config->use_vcd){
						//code:C with VCD : April-30-2010
						short v=signals->getNextLogicValue(dfflist_ptr[i]->q);
						//cout << i << ") " << v << " " << dfflist_ptr[i]->q << " " << signals->getSignalName(dfflist_ptr[i]->q) << endl ;
						if ((dfflist_ptr[i]->q)==(gate_list[ff_index]->out))
							continue;
						gateptr = dfflist_ptr[i]->dgateptr;
						gateptr->p1 = v;
						gateptr->p0 = 1 - v;
						gateptr->pd = 0; gateptr->pdn = 0;
						//Q is used (only) for first cycle timing simulation
						gateptr->q1 = v;
						gateptr->q0 = 1 - v;
						gateptr->qd = 0; gateptr->qdn = 0;
					}else{
						// code:A ( may cause higher epp , dunno why)
						/*	
						rand_value = random();
						rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
						pred_gateptr = dfflist_ptr[i]->dgateptr;
						pred_gateptr->value = rand_logic;
						pred_gateptr->input_values[0] = rand_logic; //may be not required
						int fadjnum = dfflist_ptr[i]->q;
						short node_adj_size =fadjlist[fadjnum].size();
						for (j=0; j<node_adj_size; j++){
							gateptr = fadjlist[fadjnum][j];
							short pred_gate_node_number;
							for (int k=0; k<gateptr->input_count; k++){
								//pred_gate_node_number = radjlist[gateptr->out][k];
								pred_gate_node_number = gateptr->input_list[k]; // this should be right
								if (pred_gate_node_number==fadjnum)
									gateptr->input_values[k] = pred_gateptr->value;
							}
						}
						*/

						//code:B
						rand_value = random();
						rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
						if ((dfflist_ptr[i]->q)==(gate_list[ff_index]->out))
							continue;
						gateptr = dfflist_ptr[i]->dgateptr;
						gateptr->p1 = rand_logic;
						gateptr->p0 = 1 - rand_logic;
						gateptr->pd = 0; gateptr->pdn = 0;

						gateptr->q1 = rand_logic;
						gateptr->q0 = 1 - rand_logic;
						gateptr->qd = 0; gateptr->qdn = 0;

					}
				}
				//gateptr = dfflist_ptr[ff_index]->dgateptr;
				//select & initialize the gate
				gateptr = gate_list[ff_index];
				//gateptr->p1 = 0; gateptr->p0 = 0;
				//gateptr->pd = 1; gateptr->pdn = 0;
				//now!
				// perform the fault injection, at first clock cycle We consider LET derating,
				// but from cycle 2 through n, we only consider Logic derating, because the SET should be 
				// already masked or manifested as SEU after first cycle.
				is_seu=false;
				for (int ck=1; ck<=clk; ck++){// begining of clock ck
					if( ck==1 ){
						// starting from sorted gate list and compute events for each gate in order
						for (j=0; j<gate_numbers; j++){
							gateptr = gate_list[j];
							if (gateptr->is_dff == 1) continue;
							compute_q_gate_event_list(gateptr);
							if (gateptr->event_list.size()>max_event_size)
								max_event_size = gateptr->event_list.size();
							if (gate_list[j]->out==ff_index){
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
								
								//add event D
								eventptr = new struct event;
									eventptr->time = t1;
									eventptr->q0 = 0; 
									eventptr->q1=0; 
									eventptr->qd=1; 
									eventptr->qdn=0;
								add_event(gateptr,eventptr);
								eventptr = new struct event;
									eventptr->time = t1+config->seu_pulse_width;
									eventptr->q0 = 0; 
									eventptr->q1=0; 
									eventptr->qd=0; 
									eventptr->qdn=1;
								add_event(gateptr,eventptr);
								if(config->mbu_enable){
									vector<int> targets = mbu_site->findFaultSite(j, config->mbu_depth, config->num_mbu-1);
									for(int i=0;i<targets.size();i++){
										cout << targets[i] << endl ;
										struct gate *gate = gate_list[targets[i]];

										//add event D
										eventptr = new struct event;
											eventptr->time = t1;
											eventptr->q0 = 0; 
											eventptr->q1=0; 
											eventptr->qd=1; 
											eventptr->qdn=0;
										add_event(gate,eventptr);
										eventptr = new struct event;
											eventptr->time = t1+config->seu_pulse_width;
											eventptr->q0 = 0; 
											eventptr->q1=0; 
											eventptr->qd=0; 
											eventptr->qdn=1;
										add_event(gate,eventptr);
										}
								}
								//list_size = tgateptr.event_list.size();
								//for (int k=0; k<list_size; k++){
								//	eventptr = tgateptr.event_list[0];
								//	tgateptr.event_list.pop_front();
								//	if (eventptr->time > (seu_pulse_width+t1))
								//		gateptr->event_list.push_back(eventptr);
								//	else
								//		delete eventptr;
								//}
							}
						}

						int event_time1,event_time2;

						// Checking if error has been manifested in DFF
						for (j=0; j<dff_num; j++){
							gateptr = fadjlist_ptr[dfflist_ptr[j]->q];
							compute_q_gate_event_list(gateptr);
							for (int k=0; k<gateptr->event_list.size()-1; k++){
								// only logic derating
								if ((gateptr->event_list[k]->qd !=0) || (gateptr->event_list[k]->qdn !=0)){
									lseu++;
								}
								// complete package! logic, electrical & timing
								event_time1 = gateptr->event_list[k]->time;
								event_time2 = gateptr->event_list[k+1]->time;
								if(((event_time1 <= (config->tperiod-config->t_setup))  && (event_time2 >= (config->tperiod-config->t_setup)))
									|| ((event_time1 <= (config->tperiod+config->t_hold))   && (event_time2 >= (config->tperiod+config->t_hold)))
									|| ((event_time1 <= (2*config->tperiod-config->t_setup))&& (event_time2 >= (2*config->tperiod-config->t_setup)))
									|| ((event_time1 <= (2*config->tperiod+config->t_hold)) && (event_time2 >= (2*config->tperiod+config->t_hold)))
									|| ((event_time1 <= (3*config->tperiod-config->t_setup))&& (event_time2 >= (3*config->tperiod-config->t_setup)))
									|| ((event_time1 <= (3*config->tperiod+config->t_hold)) && (event_time2 >= (3*config->tperiod+config->t_hold)))
									|| ((event_time1 <= (4*config->tperiod-config->t_setup))&& (event_time2 >= (4*config->tperiod-config->t_setup)))
									|| ((event_time1 <= (4*config->tperiod+config->t_hold)) && (event_time2 >= (4*config->tperiod+config->t_hold)))
									|| ((event_time1 <= (5*config->tperiod-config->t_setup))&& (event_time2 >= (5*config->tperiod-config->t_setup)))
									|| ((event_time1 <= (5*config->tperiod+config->t_hold)) && (event_time2 >= (5*config->tperiod+config->t_hold)))
									){
										if ((gateptr->event_list[k]->qd !=0) || (gateptr->event_list[k]->qdn !=0)){
											// this statement will check for electrical masking effect
											if ( (gateptr->event_list[k]->vomax > (VDD/2.0)) && (gateptr->event_list[k]->vomin < (VDD/2.0))){
												// error is manifested in DFF
												gateptr->p1 = 0; gateptr->p0 = 0;
												gateptr->pd = 1; gateptr->pdn = 0;
												is_seu=true;
												gptr1 = fadjlist_ptr[dfflist_ptr[j]->d];
												gptr2 = fadjlist_ptr[dfflist_ptr[j]->q];
												gptr2->p0 = gptr1->q0; 
												gptr2->p1 = gptr1->q1;
												gptr2->pd  =  gateptr->event_list[k]->qd;//gptr1->pd; 
												gptr2->pdn = gateptr->event_list[k]->qdn  ;//gptr1->pdn;
											}
										}
								}
							}
						}//dff

						for (i=0;i<gate_numbers; i++){
							gateptr = gate_list[i];
							int list_size = gateptr->event_list.size();
							for (j=list_size-1; j>=0; j--){
								eventptr = gateptr->event_list[j];
								delete eventptr;
								eventptr = NULL;
								gateptr->event_list.pop_back();
							}
						}

						// Checking if error can manifest itself to any POs.
						
						//manually injecting a fault on input, this faults will reach PO ( and not DFF )
						//with logical derating. here I don't consider timing,because I don't want to latch the error
						//on DFF. I already done that previously.
						gateptr = gate_list[ff_index];
						gateptr->p1 = 0; gateptr->p0 = 0;
						gateptr->pd = 1; gateptr->pdn = 0;

						for (i=0; i<gate_numbers; i++){
							gateptr = gate_list[i];
							if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
								continue;
							//keep the ff_index still faulty only for the first clock cycle
							//if (gateptr->out == gate_list[ff_index]->out) // change it to && (ck==1)
							if (i == ff_index) // change it to && (ck==1)
								continue;
							
							for (j=0; j< (gateptr->input_count); j++){// Input traversing
								// if input is Primary Input
								if ((gateptr->input_list[j]) <= numPi){
									if(config->use_vcd){
										//p1_values[j] = PI_list_MATRIX[gateptr->input_list[j]][ck];
										//p0_values[j] = 1 - PI_list_MATRIX[gateptr->input_list[j]][ck];
										p1_values[j] = PI_list_MATRIX[gateptr->input_list[j]][0];
										p0_values[j] = 1 - PI_list_MATRIX[gateptr->input_list[j]][0];
									}else{
										p1_values[j] = PI_list[gateptr->input_list[j]];
										p0_values[j] = 1 - PI_list[gateptr->input_list[j]];
									}
									pd_values[j] = 0; pdn_values[j] = 0;
									continue;
								}
								pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
								p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
								pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
							}// end of input traversing
							compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values); //system_failure_simulation_clockC
							
						}// End of gate_list
						int ff_index2;
						bool is_failure = false;

						// uncommenting this part, will disable electrical & timing derating on FFs from Cycle 2 onward.
						//for (ff_index2=0; ff_index2<dff_num; ff_index2++){
						//    gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
						//    gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
						//    gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
						//    gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
						//}


						for (i=(numPi+1); i<=(numPi+numPo); i++){
							gptr1 = fadjlist_ptr[i];
							if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
								is_failure = true;
							}
						}
						if( is_seu==true ) S_sim_single_cycle_ff[ff_index]++;
						if( (is_failure==true) || (is_seu==true) ) S_sim_single_cycle_po_ff[ff_index]++;

						if (is_failure==true){
							S_sim_clk[ff_index]++;
							S_sim_single_cycle_po[ff_index]++;
							break; //main break, do not continue the simulation
						}

						//first clock
					}else{ // from clock 2..n
						for (i=0; i<gate_numbers; i++){
							gateptr = gate_list[i];
							if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
								continue;
							//keep the ff_index still faulty only for the first clock cycle
							//if (gateptr->out == gate_list[ff_index]->out) // change it to && (ck==1)

							for (j=0; j< (gateptr->input_count); j++){// Input traversing
								// if input is Primary Input
								if ((gateptr->input_list[j]) <= numPi){
									if(config->use_vcd){
										p1_values[j] = PI_list_MATRIX[gateptr->input_list[j]][ck];
										p0_values[j] = 1 - PI_list_MATRIX[gateptr->input_list[j]][ck];
									}else{
										p1_values[j] = PI_list[gateptr->input_list[j]];
										p0_values[j] = 1 - PI_list[gateptr->input_list[j]];
									}
									pd_values[j] = 0; pdn_values[j] = 0;
									continue;
								}
								pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
								p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
								pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
							}// end of input traversing
							compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values); //system_failure_simulation_clockC
						}// End of gate_list
						int ff_index2;
						bool is_failure = false;
						for (ff_index2=0; ff_index2<dff_num; ff_index2++){
							gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
							gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
							gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
							gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
							//uncomment when not using multicylce
							//if ((gptr2->pd == 1)||(gptr2->pdn == 1))
							//is_failure =true;
						}

						for(i=0;i<dff_num;i++){
							gateptr = fadjlist_ptr[dfflist_ptr[i]->q];
							if ((gptr1->qd == 1)||(gptr1->qdn == 1)){
								is_failure = true;
								break;
							}
						}
						for (i=(numPi+1); i<=(numPi+numPo); i++){
							gptr1 = fadjlist_ptr[i];
							if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
								is_failure = true;
								break;
							}
							if ((gptr1->qd == 1)||(gptr1->qdn == 1)){
								is_failure = true;
								break;
							}
						}
						if (is_failure==true){
							S_sim_clk[ff_index]++;
							break;
						}
					}
				}// End of clock ck
				ex_number++;
			} //for
			total_it += step_iteration;
			total_iteration+=total_it;
			sim_epp  = S_sim_clk[ff_index] / (float)total_it;
			sim_var = sqrt((sim_epp * (1 - sim_epp))/(float)total_it);
		
			SER_PER_GATE_FF_CLK1	=	S_sim_single_cycle_ff[ff_index] 		/ (float)total_it;
			SER_PER_GATE_PO_CLK1	=	S_sim_single_cycle_po[ff_index] 		/ (float)total_it;
			SER_PER_GATE_PO_FF_CLK1 =	S_sim_single_cycle_po_ff[ff_index]		/ (float)total_it;
			

			//cout << "po_ff=" << total_failure_po_ff << " \t ff" << totalSEU_CLK1 << " \t po" << totalFAILURE_CLK1 << " \t total=" << totalSim_CLK1 <<  endl ;
			//cout << "step_iteration= "<< step_iteration << " \ttotal_it=" << total_it << " \ttotal_iteration=" << total_iteration << " \ttotalSim_CLK1= " << totalSim_CLK1 <<  endl ;
			//cout << "S_sim_clk[ff_index]= " << S_sim_clk[ff_index] << " \tsim_epp= " << sim_epp << endl ;
			
		
		} while ( (total_it<max_iteration) && ((3*sim_var) > error_tolerance) ); //end of iteration for flip-flop ff_index
		S_sim_clk[ff_index] = sim_epp;
		S_sim_clk_var[ff_index] = sim_var;
		S_sim_single_cycle_po[ff_index]		=	SER_PER_GATE_PO_CLK1 ;
		S_sim_single_cycle_ff[ff_index]		=	SER_PER_GATE_FF_CLK1 ;
		S_sim_single_cycle_po_ff[ff_index]	=	SER_PER_GATE_PO_FF_CLK1 ;
	}
	//printf("Total iteration= %d first_clock_iteration= %d first_clock_total_failure= %d \n", total_iteration ,  first_clock_iteration, first_clock_total_failure);

	// reporting results:
	float sim_epp_total=0, sim_epp_po_single_cycle=0, sim_epp_ff_single_cycle=0, sim_epp_po_ff_single_cycle=0;
	for (int i=0; i<sim_gate_numbers; i++){
		sim_epp_total += S_sim_clk[i];
		sim_epp_po_single_cycle		+=	S_sim_single_cycle_po[i];
		sim_epp_ff_single_cycle		+=	S_sim_single_cycle_ff[i];
		sim_epp_po_ff_single_cycle	+=	S_sim_single_cycle_po_ff[i];
		if(config->verbose) printf("gate sim epp: gate[%d]=%f\n",i,S_sim_clk[i]);
	}

	float sim_epp_total_ff_clk1=0, sim_epp_total_po_clk1=0,sim_epp_total_po_ff_clk1=0 ;
	
	sim_epp_total = sim_epp_total/sim_gate_numbers;    
	sim_epp_total_po_clk1	= sim_epp_po_single_cycle / sim_gate_numbers ;
	sim_epp_total_ff_clk1	 = sim_epp_ff_single_cycle / sim_gate_numbers ;
	sim_epp_total_po_ff_clk1 = sim_epp_po_ff_single_cycle / sim_gate_numbers ;
	  
	printf("average simulation epp: %f \n", sim_epp_total);
	fprintf(resultfp, "average sim epp(single cycle, ff only): %f \n", sim_epp_total_ff_clk1);
	fprintf(resultfp, "average sim epp(single cycle, ff+PO): %f \n", sim_epp_total_po_ff_clk1);
	fprintf(resultfp, "average sim epp(single cycle, POs Only): %f \n", sim_epp_total_po_clk1 );

	if(config->verbose){
		cout << "-----------------------------------------------------------" << endl ;
		cout << "	system_failure_simulation_with_let	: FiNiSHED" << endl ;
		cout << "-----------------------------------------------------------" << endl ;
	}
	for (i=0;i<gate_numbers; i++){
		gateptr = gate_list[i];
		int list_size = gateptr->event_list.size();
		for (j=list_size-1; j>=0; j--){
			eventptr = gateptr->event_list[j];
			delete eventptr;
			eventptr = NULL;
			gateptr->event_list.pop_back();
		}
	}
	for(int i=0;i<=numPi;i++) delete PI_list_MATRIX[i];	
	delete PI_list_MATRIX;
	return;
}//system_failure_simulation_with_let
//////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: system_failure_simulation_fpga
// Purpose:  Computes system failure probability using simulation method (Applying large amount of vectors)
// Returns none
void system_failure_simulation_fpga(long max_iteration, long clk){
    long i,j,ck,first_clock_failure=0,first_clock_total_failure=0,total_iteration=0, total_failure=0, first_clock_iteration=0;
	long it, total_it;
    short ff_index;
    //short nff_index;
    long rand_value;
    short rand_logic;
    struct gate *gateptr;
    struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
	float error_tolerance = 0.01;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	long step_iteration = 100;
	float sim_epp, sim_var;

    sim_gate_numbers = gate_numbers;
    if (PI_list==NULL)
        PI_list = new short[numPi+1];
       
	cout << "Number of FFs " << dff_num << endl;

	///////////////////////////////////////////////////////////////
    for (ff_index=0; ff_index<sim_gate_numbers; ff_index++){
		first_clock_failure=0;
		cout << "Simulating gate# " << ff_index << " ... ";
		if ( gate_list[ff_index]->out <= (numPi+numPo)){
    	    S_sim_clk[ff_index] = 1; //Initialize the S_sim_clk[ff_index]
			S_sim_clk_var[ff_index] = 0;
			cout << endl;
			continue;
		}

		S_sim_clk[ff_index] = 0; //Initialize the S_sim_clk[ff_index]
		S_sim_clk_var[ff_index] = 0;

        // Set MAX values to ensure each will get correct value
		total_it = 0;
        ex_number = 0;
		do{
	        for (it=0; it<step_iteration; it++){// begin iteration for ff_index flip-flop
        	    for (i=0; i<gate_numbers; i++){
    	            gateptr = gate_list[i];
	    	        gateptr->p0 = 0; gateptr->p1 = 0;
		            gateptr->pd = 0; gateptr->pdn = 0;
	            }
    	        for (i=1; i<=(numPi); i++){
	                rand_value = random();
        	        //Assigning random loginc to Primary Input
            	    rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
                	PI_list[i] = rand_logic;
	            }
    	        for (i=0; i<dff_num; i++){
            	    rand_value = random();
    	            rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
        	        if ((dfflist_ptr[i]->q)==(gate_list[ff_index]->out))
            	        continue;
	                gateptr = dfflist_ptr[i]->dgateptr;
    	            gateptr->p1 = rand_logic;
        	        gateptr->p0 = 1 - rand_logic;
            	    gateptr->pd = 0; gateptr->pdn = 0;
	            }
    	        //gateptr = dfflist_ptr[ff_index]->dgateptr;
	            gateptr = gate_list[ff_index];

				if(gateptr->is_lut){
					int faultCell = random() % gateptr->lut_size ;
					gateptr->fault[faultCell] = 1 ;
				}else{ 
					gateptr->p1 = 0; gateptr->p0 = 0;
					gateptr->pd = 1; gateptr->pdn = 0;
				}
				
	            for (ck=1; ck<=clk; ck++){// begining of clock ck
		            for (i=0; i<gate_numbers; i++){
        	            gateptr = gate_list[i];
            	        if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
                	        continue;
						//keep the ff_index still faulty only for the first clock cycle
						//if (gateptr->out == gate_list[ff_index]->out) // change it to && (ck==1)
						if ((i == ff_index) && (ck==1)) // change it to && (ck==1)
							continue;
                	    for (j=0; j< (gateptr->input_count); j++){// Input traversing
	                        // if input is Primary Input
    	                    if ((gateptr->input_list[j]) <= numPi){
            	                p1_values[j] = PI_list[gateptr->input_list[j]];
                	            p0_values[j] = 1 - PI_list[gateptr->input_list[j]];
                    	        pd_values[j] = 0; pdn_values[j] = 0;
                        	    continue;
	                        }
    	                    pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
        	                p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
                	        pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
	                    }// end of input traversing
						compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values); //system_failure_simulation_clockC
	    	        }// End of gate_list
            	    int ff_index2;
					bool is_failure = false;
    	            for (ff_index2=0; ff_index2<dff_num; ff_index2++){
    	                gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
        	            gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
            	        gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
    	                gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
						//uncomment when not using multicylce
						//if ((gptr2->pd == 1)||(gptr2->pdn == 1))
							//is_failure =true;
	                }
	                for (i=(numPi+1); i<=(numPi+numPo); i++){
						if (is_failure==true)break;
                	    gptr1 = fadjlist_ptr[i];
                    	if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
							is_failure = true;
        	                break;
            	        }
	                }
					if (is_failure==true){
						if(ck==1) first_clock_failure++ ;
						if(ck==1) first_clock_total_failure++;
						
						S_sim_clk[ff_index]++;
						total_failure++;
						break;
					}
					if(ck==1) first_clock_iteration++;
        	    }// End of clock ck
	            ex_number++;
				
				if(gate_list[ff_index]->is_lut){
					for(int i=0;i<gate_list[ff_index]->lut_size;i++){
						gate_list[ff_index]->fault[i] = 0 ;
					}
				}
				
				
			} //for
			total_it += step_iteration;
			total_iteration+=total_it;
			sim_epp  = S_sim_clk[ff_index] / (float)total_it;
			sim_var = sqrt((sim_epp * (1 - sim_epp))/(float)total_it);

        } while ((total_it<max_iteration) && ((3*sim_var) > error_tolerance)); //end of iteration for flip-flop ff_index
		fprintf(resultfp, "Node %d: Sims iterated  = %d \n", gate_list[ff_index]->out, total_it);
		
        //for (i=1; i<=clk; i++)
            //S_sim_clk[ff_index][i] = S_sim_clk[ff_index][i] / (float)max_iteration;
		S_sim_clk[ff_index] = sim_epp;
		S_sim_clk_var[ff_index] = sim_var;
		fprintf(resultfp, "simulated for: %d. first clock failure= %d\n", total_it, first_clock_failure);
		cout << "simulated for: " << total_it << ". first clock failure= " << first_clock_failure << endl;
		
    }
	fprintf(resultfp, "Total iteration= %d first_clock_iteration= %d first_clock_total_failure= %d \n", total_iteration ,  first_clock_iteration, first_clock_total_failure);
	cout << "Total iteration= " << total_iteration << " first_clock_iteration= " << first_clock_iteration << " first_clock_total_failure= " << first_clock_total_failure  << endl ;

	// reporting results:
	float sim_epp_total=0;
	for (int i=0; i<sim_gate_numbers; i++){
		sim_epp_total += S_sim_clk[i];
		printf("gate sim epp: gate[%d]=%f\n",i,S_sim_clk[i]);
	}

	sim_epp_total = sim_epp_total/sim_gate_numbers;    
	printf("average sim epp: %f \n", sim_epp_total);
}//system_failure_simulation_fpga
//////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: system_failure_simulation_clockC
// Purpose:  Computes system failure probability using simulation method (Applying large amount of vectors)
// Returns none
void system_failure_simulation_clockC(long max_iteration, long clk){
    long i,j,ck,first_clock_failure=0,first_clock_total_failure=0,total_iteration=0, total_failure=0, first_clock_iteration=0;
	long it, total_it;
    short ff_index;
    //short nff_index;
    long rand_value;
    short rand_logic;
    struct gate *gateptr;
    struct gate *pred_gateptr;
    struct gate* gptr1,*gptr2;
	float error_tolerance = 0.01;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	long step_iteration = 100;
	float sim_epp, sim_var;

    sim_gate_numbers = gate_numbers;
/*	if ((gate_numbers-dff_num) > 700)
		sim_gate_numbers = 700 + dff_num;
*/
    if (PI_list==NULL)
        PI_list = new short[numPi+1];
        //PI_list = (short*)malloc((numPi+1)*sizeof(short));
	cout << "Number of FFs " << dff_num << endl;

	///////////////////////////////////////////////////////////////
    for (ff_index=0; ff_index<sim_gate_numbers; ff_index++)
    {
		first_clock_failure=0;
		cout << "Simulating gate# " << ff_index << " ... ";
		if ( gate_list[ff_index]->out <= (numPi+numPo)) //multicycle
		{
    	    S_sim_clk[ff_index] = 1; //Initialize the S_sim_clk[ff_index]
			S_sim_clk_var[ff_index] = 0;
			cout << endl;
			continue;
		}
		S_sim_clk[ff_index] = 0; //Initialize the S_sim_clk[ff_index]
		S_sim_clk_var[ff_index] = 0;


        // Set MAX values to ensure each will get correct value
		total_it = 0;
        ex_number = 0;
        //srandom(0);
		do{
	        for (it=0; it<step_iteration; it++){// begin iteration for ff_index flip-flop
        	    for (i=0; i<gate_numbers; i++){
    	            gateptr = gate_list[i];
	    	        gateptr->p0 = 0; gateptr->p1 = 0;
		            gateptr->pd = 0; gateptr->pdn = 0;
	            }
    	        for (i=1; i<=(numPi); i++){
	                rand_value = random();
        	        //Assigning random loginc to Primary Input
            	    rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
                	PI_list[i] = rand_logic;
	            }
    	        for (i=0; i<dff_num; i++){
            	    rand_value = random();
    	            rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
        	        if ((dfflist_ptr[i]->q)==(gate_list[ff_index]->out))
            	        continue;
	                gateptr = dfflist_ptr[i]->dgateptr;
    	            gateptr->p1 = rand_logic;
        	        gateptr->p0 = 1 - rand_logic;
            	    gateptr->pd = 0; gateptr->pdn = 0;
	            }
    	        //gateptr = dfflist_ptr[ff_index]->dgateptr;
	            gateptr = gate_list[ff_index];
    	        gateptr->p1 = 0; gateptr->p0 = 0;
	            gateptr->pd = 1; gateptr->pdn = 0;
	            for (ck=1; ck<=clk; ck++){// begining of clock ck
		            for (i=0; i<gate_numbers; i++){
        	            gateptr = gate_list[i];
            	        if (gateptr->is_dff == 1) // we will compute the flip-flops at the end
                	        continue;
						//keep the ff_index still faulty only for the first clock cycle
						//if (gateptr->out == gate_list[ff_index]->out) // change it to && (ck==1)
						if ((i == ff_index) && (ck==1)) // change it to && (ck==1)
							continue;
                	    for (j=0; j< (gateptr->input_count); j++){// Input traversing
	                        // if input is Primary Input
    	                    if ((gateptr->input_list[j]) <= numPi){
            	                p1_values[j] = PI_list[gateptr->input_list[j]];
                	            p0_values[j] = 1 - PI_list[gateptr->input_list[j]];
                    	        pd_values[j] = 0; pdn_values[j] = 0;
                        	    continue;
	                        }
    	                    pred_gateptr = fadjlist_ptr[gateptr->input_list[j]];
        	                p0_values[j] = pred_gateptr->p0; p1_values[j] = pred_gateptr->p1;
                	        pd_values[j] = pred_gateptr->pd; pdn_values[j] = pred_gateptr->pdn;
	                    }// end of input traversing
    	                compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values); //system_failure_simulation_clockC
	    	        }// End of gate_list
            	    int ff_index2;
					bool is_failure = false;
    	            for (ff_index2=0; ff_index2<dff_num; ff_index2++){
    	                gptr1 = fadjlist_ptr[dfflist_ptr[ff_index2]->d];
        	            gptr2 = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
            	        gptr2->p0 = gptr1->p0; gptr2->p1 = gptr1->p1;
    	                gptr2->pd = gptr1->pd; gptr2->pdn = gptr1->pdn;
						//uncomment when not using multicylce
						//if ((gptr2->pd == 1)||(gptr2->pdn == 1))
							//is_failure =true;
	                }
	                for (i=(numPi+1); i<=(numPi+numPo); i++){
						if (is_failure==true)break;
                	    gptr1 = fadjlist_ptr[i];
                    	if ((gptr1->pd == 1)||(gptr1->pdn == 1)){
							is_failure = true;
        	                break;
            	        }
	                }
					if (is_failure==true){
						if(ck==1) first_clock_failure++ ;
						if(ck==1) first_clock_total_failure++;
						
						S_sim_clk[ff_index]++;
						total_failure++;
						break;
					}
					if(ck==1) first_clock_iteration++;
        	    }// End of clock ck
	            ex_number++;
			} //for
			total_it += step_iteration;
			total_iteration+=total_it;
			sim_epp  = S_sim_clk[ff_index] / (float)total_it;
			sim_var = sqrt((sim_epp * (1 - sim_epp))/(float)total_it);

        } while ((total_it<max_iteration) && ((3*sim_var) > error_tolerance)); //end of iteration for flip-flop ff_index
		fprintf(resultfp, "Node %d: Sims iterated  = %d \n", gate_list[ff_index]->out, total_it);
		
        //for (i=1; i<=clk; i++)
            //S_sim_clk[ff_index][i] = S_sim_clk[ff_index][i] / (float)max_iteration;
		S_sim_clk[ff_index] = sim_epp;
		S_sim_clk_var[ff_index] = sim_var;
		fprintf(resultfp, "simulated for: %d. first clock failure= %d\n", total_it, first_clock_failure);
		cout << "simulated for: " << total_it << ". first clock failure= " << first_clock_failure << endl;
		
    }
	fprintf(resultfp, "Total iteration= %d first_clock_iteration= %d first_clock_total_failure= %d \n", total_iteration ,  first_clock_iteration, first_clock_total_failure);
	cout << "Total iteration= " << total_iteration << " first_clock_iteration= " << first_clock_iteration << " first_clock_total_failure= " << first_clock_total_failure  << endl ;

	// reporting results:
	float sim_epp_total=0;
	for (int i=0; i<sim_gate_numbers; i++){
		sim_epp_total += S_sim_clk[i];
		printf("gate sim epp: gate[%d]=%f\n",i,S_sim_clk[i]);
	}

	sim_epp_total = sim_epp_total/sim_gate_numbers;    
	printf("average sim epp: %f \n", sim_epp_total);

	system("PAUSE");
}//system_failure_simulation_clockC
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: print_multi_cycle_results
// Purpose:  print_multi_cycle_results
// Returns none
void print_multi_cycle_results(int clk){
	fprintf(resultfp, "\n CLK= %d\n", clk);

	int j=0;
	float diff_partial=0, diff_total=0, diff_total2=0, diff_partial_po=0;
	float sim_epp_total=0, sim_epp_no_po_total=0, sim_epp_po_total=0;
	float sys_epp_total=0, sys_epp_no_po_total=0, sys_epp_po_total=0;
	//short not_ff_po=0;
	short not_po=0, po=0;
    // Prints simulated Vector S
	//for (i=1; i<=clk; i++)
	//{
 //   fprintf(resultfp, "\n NODES \t\t Sck[]\t\t S_sim_clk[]\t Difference\n");
	// Dumping gate derating infos for Simulation
//	fprintf(resultfp, "Dumping gate derating infos for Simulation\n");
//	fprintf(resultfp, "sim_gate=%d\n", sim_gate_numbers);
//	for (j=0; j<sim_gate_numbers; j++){
//		fprintf(resultfp, "gate= %d , S_sim_clk= %f \n", j, S_sim_clk[j]);
  //  }

	// Dumping gate derating infos for Analytical approach
//	fprintf(resultfp, "Dumping gate derating infos for Analytical approach\n");
//	fprintf(resultfp, "sim_gate=%d\n", sim_gate_numbers);
///	for (j=0; j<sim_gate_numbers; j++){
//		fprintf(resultfp, "gate= %d , Sck[j]= %f \n", j, Sck[j]);
  //  }


    for (j=0; j<sim_gate_numbers; j++){
        //cout << "node " << gate_list[j]->out << ": " << S_sim_clk[j] << "::" << Sck[j] << endl;
       // fprintf(resultfp, "node %4d: \t%f :: \t%f :: \t%f\n", gate_list[j]->out, Sck[j], S_sim_clk[j], fabs(Sck[j] - S_sim_clk[j]));
		diff_total += fabs(S_sim_clk[j]-Sck[j]);
		sim_epp_total += S_sim_clk[j];

		sys_epp_total += Sck[j];
		if (gate_list[j]->out > (numPi+numPo)){
			sim_epp_no_po_total += S_sim_clk[j];
			sys_epp_no_po_total += Sck[j];
			diff_partial += fabs(S_sim_clk[j]-Sck[j]);
			not_po++;
		}else{
			sim_epp_po_total += S_sim_clk[j];
			sys_epp_po_total += Sck[j];
			diff_partial_po += fabs(S_sim_clk[j]-Sck[j]);
			po++;
		}
		
    }

	diff_partial = diff_partial / not_po;
	diff_total = diff_total/sim_gate_numbers;
	sim_epp_total = sim_epp_total/sim_gate_numbers;
	sys_epp_total = sys_epp_total/sim_gate_numbers;
	sim_epp_no_po_total = sim_epp_no_po_total/not_po;
	sys_epp_no_po_total = sys_epp_no_po_total/not_po;
	diff_total2=fabs(sim_epp_total-sys_epp_total);

	diff_partial_po = diff_partial_po/po;
	sim_epp_po_total = sim_epp_po_total/po;
	sys_epp_po_total = sys_epp_po_total/po;
	


	//fprintf(resultfp,"///////////////////////// \n");
    //fprintf(resultfp,"Multi Cycle Results: No of systematic clocks = %d \n", clk);
	//cout << "Total difference: " << diff_total<< endl;
    fprintf(resultfp, "average sim epp: %f \n", sim_epp_total);
    fprintf(resultfp, "average sys epp: %f \n", sys_epp_total);
    fprintf(resultfp, "average partial sim epp: %f \n", sim_epp_no_po_total);
    fprintf(resultfp, "average partial sys epp: %f \n", sys_epp_no_po_total);
    fprintf(resultfp, "Total difference Fabs: %f \n", diff_total);
	fprintf(resultfp, "Total difference Average: %f \n", diff_total2);
	
	//fprintf(resultfp, "partial_adelized: %f \n", partial_adelized);
    //fprintf(resultfp, "partial_babakized: %f \n", partial_babakized);

    //cout << "Parital Difference: " << diff_partial<< endl;
    fprintf(resultfp, "Partial Difference: %f \n", diff_partial);


	fprintf(resultfp, "diff_partial_po: %f \n", diff_partial_po);
	fprintf(resultfp, "sim_epp_po_total: %f \n", sim_epp_po_total);
	fprintf(resultfp, "sys_epp_po_total: %f \n", sys_epp_po_total);


	// we include ffs in accuracy computation but we do not include dff simulation in our sim time
	//fprintf(resultfp, "\n not_po = %d, sim_gate_numbers = %d \n", not_po, sim_gate_numbers);
	//if (sim_gate_numbers != gate_numbers)
	//	sim_gate_numbers -= dff_num;

    //fprintf(resultfp, "\n\\\\\\\\\\\\\\\\\\\n");
    /*fprintf(resultfp, "Variance Difference\n");
    fprintf(resultfp, "\n NODES \t\t SysVar\t\t\t SimVar \t\tVar. Diff. \t\t Error \t\t\t EPP Diff.\n");

	float z_alpha_half = 2.576;
    for (j=0; j<sim_gate_numbers; j++)
    {
        fprintf(resultfp, "node %4d: \t%f :: \t%f :: \t%f :: \t%f :: \t%f\n", gate_list[j]->out, gate_list[j]->epp_var, S_sim_clk_var[j], fabs(gate_list[j]->epp_var - S_sim_clk_var[j]), gate_list[j]->epp_var*z_alpha_half, fabs(Sck[j] - S_sim_clk[j]));
    }*/

    fprintf(resultfp, "\n/////////////////////////////////////////////////////////////////// \n");

}//print_multi_cycle_results
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: timing_derating_sim
// Purpose:  timing_derating_sim
// Returns none
void timing_derating_sim(long FI_iteration, long max_iteration){
	int i,j,k,gate_index;//fiter,
    long rand_value;
	int target_gate;
    short rand_logic;
    struct gate *gateptr;
	struct gate tgateptr;
    struct gate *pred_gateptr;
	struct event* eventptr;
	int list_size;
	int t1;
	int failures,total_failures=0;
	//float system_failure;
	float total_variance, sample_variance;
	float total_derating, sample_derating;
	float error_tolerance = 0.03;
	long total_iteration = 0;
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Simulation Derating Results\n");

	int max_event_size=0;
	do{
		sample_variance = 0;
		sample_derating = 0;
		failures = 0;
		for (gate_index=0; gate_index<FI_iteration; gate_index++)
		{
			//failures = 0;
			//target_gate = rand() % gate_numbers;
			target_gate = random() % gate_numbers;

			//target_gate = gate_list[gate_index]->out;
			if (gate_list[target_gate]->is_dff == 1)
			{
				gate_index--;
				continue;
			}
			target_gate = gate_list[target_gate]->out;
			//for (fiter=0; fiter<FI_iteration; fiter++)
			//{
				//set a random vulue to FFs and PIs;
				//Assign random values to the inputs
				for (i=1; i<=(numPi); i++)
				{
					rand_value = random();// random function on Unix (Better Performance)
					//rand_value = rand();                    // random function on Windows &Unix
    				//Assigning random loginc to Primary Input
        			rand_logic = rand_value & 1; //(ex_number & (1<<(i-1)) )>> (i-1); //
					PI_list[i] = rand_logic;
        			// Forwording the PI's value to its successors
					short node_adj_size =fadjlist[i].size();
    				for (j=0; j<node_adj_size; j++)
        			{
            			gateptr = fadjlist[i][j];
						short pred_gate_node_number;
    					for (k=0; k<gateptr->input_count; k++)
        				{
            				//pred_gate_node_number = radjlist[gateptr->out][k];
            				pred_gate_node_number = gateptr->input_list[k]; // this should be right
                			if (pred_gate_node_number==i)
                    			gateptr->input_values[k] = PI_list[i];
						}
    				}
				}
				//Assign random values to DFFs
				for (i=0; i<dff_num; i++)
				{
    				rand_value = random();	// random function on Unix (Better Performance)
					//rand_value = rand();                    // random function on Windows &Unix
    				rand_logic= rand_value & 0x1; //(ex_number & (1<<(i+numPi)) )>> (i+numPi); //
        			pred_gateptr = dfflist_ptr[i]->dgateptr;
					pred_gateptr->value = rand_logic;
    				pred_gateptr->input_values[0] = rand_logic; //may be not required
        			int fadjnum = dfflist_ptr[i]->q;
					short node_adj_size =fadjlist[fadjnum].size();
    				for (j=0; j<node_adj_size; j++)
        			{
						gateptr = fadjlist[fadjnum][j];
    					short pred_gate_node_number;
        				for (k=0; k<gateptr->input_count; k++)
            			{
                			//pred_gate_node_number = radjlist[gateptr->out][k];
                			pred_gate_node_number = gateptr->input_list[k]; // this should be right
							if (pred_gate_node_number==fadjnum)
    							gateptr->input_values[k] = pred_gateptr->value;
        				}
					}
				}

				// starting from sorted gate list and compute events for each gate in order
				for (j=0; j<gate_numbers; j++)
				{
					gateptr = gate_list[j];
					if (gateptr->is_dff == 1)
						continue;
					if (gateptr->out==9)
						compute_gate_event_list(gateptr);
					else
						compute_gate_event_list(gateptr);
					if (gateptr->event_list.size()>max_event_size)
						max_event_size = gateptr->event_list.size();
					if (gate_list[j]->out==target_gate)
					{
						//t1 = rand() % tperiod; // circuit_period;
						t1 = random() % config->tperiod; // circuit_period;
						//removing events >t1
						list_size = gateptr->event_list.size();
						for (k=list_size-1; k>=0; k--)
						{
							if (gateptr->event_list[k]->time >= t1)
							{
								eventptr = gateptr->event_list[k];
								gateptr->event_list.pop_back();
								tgateptr.event_list.push_front(eventptr);
							}
						}
						//eventptr = (struct event*) malloc(sizeof(struct event));
						eventptr = new struct event;
						eventptr->time = t1;
						eventptr->p0 = 0; eventptr->p1=0; eventptr->pd=1; eventptr->pdn=0;
						add_event(gateptr,eventptr);
						//eventptr = (struct event*) malloc(sizeof(struct event));
						eventptr = new struct event;
						eventptr->time = t1+config->seu_pulse_width;
						eventptr->p0 = 1; eventptr->p1=0; eventptr->pd=0; eventptr->pdn=0;
						add_event(gateptr,eventptr);
						//delete_event(gateptr,t1,t1+seu_pulse_width);
						list_size = tgateptr.event_list.size();
						for (k=0; k<list_size; k++)
						{
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
				for (j=0; j<dff_num; j++)
				{
					gateptr = fadjlist_ptr[dfflist_ptr[j]->q];
					compute_gate_event_list(gateptr);
					for (k=0; k<gateptr->event_list.size()-1; k++)
					{
						//if there is an event (ti,D) such that tperiod-(tsu+th)<ti <tperiod
						event_time1 = gateptr->event_list[k]->time;
						event_time2 = gateptr->event_list[k+1]->time;
						//if ((event_time >= (tperiod-tsu)) && (event_time <= (tperiod+th) ) )
						if (   ((event_time1 <= (config->tperiod-config->t_setup))   && (event_time2 >= (config->tperiod-config->t_setup)))
							|| ((event_time1 <= (config->tperiod+config->t_hold))    && (event_time2 >= (config->tperiod+config->t_hold)))
							|| ((event_time1 <= (2*config->tperiod-config->t_setup)) && (event_time2 >= (2*config->tperiod-config->t_setup)))
							|| ((event_time1 <= (2*config->tperiod+config->t_hold))  && (event_time2 >= (2*config->tperiod+config->t_hold)))
							|| ((event_time1 <= (3*config->tperiod-config->t_setup)) && (event_time2 >= (3*config->tperiod-config->t_setup)))
							|| ((event_time1 <= (3*config->tperiod+config->t_hold))  && (event_time2 >= (3*config->tperiod+config->t_hold)))
							|| ((event_time1 <= (4*config->tperiod-config->t_setup)) && (event_time2 >= (4*config->tperiod-config->t_setup)))
							|| ((event_time1 <= (4*config->tperiod+config->t_hold))  && (event_time2 >= (4*config->tperiod+config->t_hold)))
							|| ((event_time1 <= (5*config->tperiod-config->t_setup)) && (event_time2 >= (5*config->tperiod-config->t_setup)))
							|| ((event_time1 <= (5*config->tperiod+config->t_hold))  && (event_time2 >= (5*config->tperiod+config->t_hold)))
							)
						{
							if ((gateptr->event_list[k]->pd !=0) || (gateptr->event_list[k]->pdn !=0))
							{
								failures++;
								total_failures++;
								failure_found = true;
								break;
							}
						}
					}
					if (failure_found == true)
						break;
				}
				//fprintf(resultfp,"\n ================================= \n");
				//fprintf(resultfp,"SIM Eventlist when ngate %d is faulty: \n", target_gate);


				for (i=0;i<gate_numbers; i++)
				{
					gateptr = gate_list[i];
					list_size = gateptr->event_list.size();
					//fprintf(resultfp,"\n Gate %d:  type=%d",gateptr->out,gateptr->type);
					for (j=0; j<list_size; j++)
					{
						eventptr = gateptr->event_list[j];
						//fprintf(resultfp, "\n\t %d \t\t", eventptr->time);
						//fprintf(resultfp, "%f %f %f %f", eventptr->p0,eventptr->p1,eventptr->pd,eventptr->pdn);
					}
					for (j=list_size-1; j>=0; j--)
					{
						eventptr = gateptr->event_list[j];
						delete eventptr;
						eventptr = NULL;
						gateptr->event_list.pop_back();
					}
				}
			//} //fiter
			//system_failure =  (float)failures/(float)FI_iteration;
			//fprintf(resultfp,"Gate %d = %f: \n", target_gate, system_failure);
			//fadjlist_ptr[target_gate]->sderating = system_failure;

		}//for fault injection
		sample_variance = (float)failures - (float)failures/(float)FI_iteration;
		sample_variance = sqrt(sample_variance/(float)(FI_iteration-1));
		sample_derating = (float)failures / (float)FI_iteration;
		total_iteration += FI_iteration;
		if (total_iteration == FI_iteration)
		{//first sample
			total_iteration = FI_iteration;
			total_derating = sample_derating;
			total_variance = sample_variance;
		}
		else
		{
			total_derating += pow(total_variance,2)*(sample_derating-total_derating)/(pow(total_variance,2)+pow(sample_variance,2));
			total_variance = pow(total_variance,2)*pow(sample_variance,2)/(pow(total_variance,2)+pow(sample_variance,2));
		}
		FI_iteration = max_iteration - total_iteration;
		long tmp_iter = (int)ceil((float)total_iteration * ((3.0*total_variance/error_tolerance)-1) * ((3.0*total_variance/error_tolerance)-1) );
		if (tmp_iter < FI_iteration)
			FI_iteration = tmp_iter;
		if (FI_iteration < 100)
			FI_iteration = 100;

		//system_failure =  (float)failures/(float)FI_iteration;
	} while (((3*total_variance) > error_tolerance) && (total_iteration<max_iteration) );
	//system_failure =  (float)total_failures/(float)(FI_iteration*(gate_numbers-dff_num));
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Simulation Derating = %f \n", total_derating);
	fprintf(resultfp,"Total iteration = %d \n", total_iteration);
	fprintf(resultfp,"Variance = %f \n", total_variance);
	fprintf(resultfp, "Max event size in sim derating=%d \n", max_event_size);

}//timing_derating_sim
/////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: timing_derating
// Purpose:  timing_derating
// Returns none
void timing_derating(void){
	//tperiod = t_crit_path + tsu + th + tffd;

    long i,j,k;
    short ff_index;
    short nff_index;
    short PI2POlist_size;
    struct gate *gateptr;
    //struct gate *pred_gateptr;
    //struct gate* gptr1,*gptr2;
    //short pred_gate_number;
    //float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    //float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	int tsum,tk;
	float tpsum;
	struct event* eventptr;
	int list_size;
	int max_event_size=0;

    for (ff_index=0; ff_index<gate_numbers; ff_index++)
    {
        nff_index = gate_list[ff_index]->out;
		//The comment can be removed!!
		if ((gate_list[ff_index]->is_dff == 1))// || (nff_index <= (numPi+numPo)))
		{
			gate_list[ff_index]->derating = 1;
			continue;
		}
        // To get PI2POlist starting from flip-flop nff_index excluding it
        find_TPsort_from_dffi(nff_index);
        PI2POlist_size = PI2POlist.size();
        // Set MAX values to ensure each will get correct value
        for (i=0; i<gate_numbers; i++)
        {
            gateptr= gate_list[i];
            gateptr->p0 = 0; gateptr->p1 = 0;
            gateptr->pd = 0; gateptr->pdn = 0;
			gateptr->is_onpath = false;
        }
	    for (i=0; i<PI2POlist_size; i++)
	    {
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
            gateptr->p0 = config->MAX; gateptr->p1 = config->MAX;
            gateptr->pd = config->MAX; gateptr->pdn = config->MAX;
			gateptr->is_onpath = true;
		}
		gateptr = gate_list[ff_index];
		//gateptr->pd = 1; gateptr->pdn = 0; gateptr->p1 = 0; gateptr->p0 = 0;
        //
		//add event D
		//eventptr = (struct event*) malloc(sizeof(struct event));
		eventptr = new struct event;
		eventptr->time = 0;
		eventptr->p0 = 1- gateptr->signal_probability; eventptr->p1 = gateptr->signal_probability;
		eventptr->pd = 0; eventptr->pdn = 0;
		add_event(gateptr,eventptr);
		eventptr = new struct event;
		eventptr->time = (int)gateptr->gd;
		eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 1; eventptr->pdn = 0;
		add_event(gateptr,eventptr);
		eventptr = new struct event;
		eventptr->time = (int)gateptr->gd + config->seu_pulse_width;
		//APPROACH 1
		eventptr->p0 = 1 - gateptr->signal_probability ; eventptr->p1 = gateptr->signal_probability; eventptr->pd = 0; eventptr->pdn = 0;
		//APPROACH 2
		//eventptr->p0 = 0; eventptr->p1 = 0; eventptr->pd = 0; eventptr->pdn = 1;
		add_event(gateptr,eventptr);

	    for (i=0; i<PI2POlist_size; i++)
	    {
            gateptr = fadjlist_ptr[PI2POlist[i][0]];
            compute_D_gate_event_list(gateptr, nff_index);
			if (gateptr->event_list.size()>max_event_size)
				max_event_size = gateptr->event_list.size();

	    }// End of PI2POlist
///////////////////////////////////



        int ff_index2;
        for (ff_index2=0; ff_index2<dff_num; ff_index2++)
        {
            gateptr = fadjlist_ptr[dfflist_ptr[ff_index2]->q];
            compute_D_gate_event_list(gateptr, nff_index);
			if (gateptr->event_list.size()>max_event_size)
				max_event_size = gateptr->event_list.size();
        }
		short MAXS=3,NMAXS=-1;
        float sys_failure=1;
		int ff_ind;
		for (ff_ind=0; ff_ind<dff_num; ff_ind++)
		{
			gateptr = fadjlist_ptr[dfflist_ptr[ff_ind]->q];

			list_size = gateptr->event_list.size();
			/*
			short *maze = new short[list_size];
			for (i=0; i<list_size; i++)
				maze[i] = NMAXS;
			i = 0;
			tpsum = 0;
			tsum = 0;
			while (i>=0)
			{
				maze[i]++;
				if (maze[i]>=MAXS)
				{
					maze[i] = NMAXS;
					i--;
					continue;
				}
				j = i-1;
				while (j>=0)
				{
					if (maze[j]!=0)
						break;
					j--;
				}
				if (j>=0)
				{
					if (maze[i]==maze[j])
						maze[i]++;
				}
				if (maze[i]>=MAXS)
				{
					maze[i] = NMAXS;
					i--;
					continue;
				}
				if (i==list_size-1)
				{
					int fi,si;
					fi = 0;
					while (fi<list_size)
					{
						if (maze[fi]==0)
						{
							fi++;
							continue;
						}
						else
							break;

					}
					si = list_size-1;
					while (si>=0)
					{
						if (maze[si]==0)
						{
							si--;
							continue;
						}
						else
							break;
					}

					if ((fi>=list_size)||(si<0))
						continue;
					if (maze[fi]==maze[si])
						continue;
					float prob=1;
					for (j=0; j<list_size; j++)
					{
						if (maze[j]==0)
							prob *= (gateptr->event_list[j]->p0 + gateptr->event_list[j]->p1);
						if (maze[j]==1)
							prob *= gateptr->event_list[j]->pd;
						if (maze[j]==2)
							prob *= gateptr->event_list[j]->pdn;
					}
					tk = 0;
					fi = 0;
					si = 0;
					while (fi<list_size)
					{
						if (maze[fi]==0)
						{
							fi++;
							continue;
						}
						if (fi>=list_size)
							break;
						si = fi+1;
						while (si<list_size)
						{
							if (maze[si]!=0)
								break;
							si++;
						}
						if (si>=list_size)
							break;
						tk += gateptr->event_list[si]->time - gateptr->event_list[fi]->time;
						fi = si;
					}//inner while
					tpsum += tk * prob;
					tsum += tk;

					continue;
				}//if (i==list_size-1)
				i++;
			}//outer while
			tpsum *= 2; // To account for both rise and fall possibilities
			if (tsum != 0)
				sys_failure *= 1- ((float)tpsum+(tsu+th)*(float)tpsum/(float)tsum)/(float)tperiod;
			delete [] maze;
			*/
			///*
			tpsum = 0;
			tsum = 0;
			for (k=0; k<gateptr->event_list.size()-1; k++)
			{
				tk = gateptr->event_list[k+1]->time - gateptr->event_list[k]->time;
				tpsum += tk * (gateptr->event_list[k]->pd+gateptr->event_list[k]->pdn);
				tsum += tk;
			}
			if (tsum != 0)
				sys_failure *= 1- ((float)tpsum+(config->t_setup+config->t_hold)*(float)tpsum/(float)tsum)/(float)config->tperiod;
			//*/

		}//for (ff_ind=0; ....
        sys_failure = 1 - sys_failure;
		gateptr = gate_list[ff_index]; //ff_index is gate_list index
		gateptr->derating = sys_failure;

		//fprintf(resultfp,"\n ================================= \n");
		//fprintf(resultfp,"Eventlist when ngate %d is faulty: \n", nff_index);


		for (i=0;i<gate_numbers; i++)
		{
			gateptr = gate_list[i];
			list_size = gateptr->event_list.size();
			//fprintf(resultfp,"\n Gate %d:  type=%d",gateptr->out,gateptr->type);
			//for (j=0; j<list_size; j++)
			//{	eventptr = gateptr->event_list[j];
			//	fprintf(resultfp, "\n\t %d \t\t", eventptr->time);
			//	fprintf(resultfp, "%f %f %f %f", eventptr->p0,eventptr->p1,eventptr->pd,eventptr->pdn);
			//}
			for (j=list_size-1; j>=0; j--)
			{
				eventptr = gateptr->event_list[j];
				//fprintf(resultfp, "%d, ", eventptr->time);
				delete eventptr;
				eventptr = NULL;
				gateptr->event_list.pop_back();
			}
		}
    }//for

	//fprintf(resultfp,"\n ================================= \n");
	//fprintf(resultfp,"Systemtic Derating Results\n");
	float overall_sys_der = 0;
	for (i=0; i<gate_numbers; i++)
	{
		gateptr = gate_list[i];
		if (gateptr->is_dff==1)
			continue;
		//fprintf(resultfp, "Gate %d = %f \n",gateptr->out, gateptr->derating);
		overall_sys_der += gateptr->derating;
	}
	overall_sys_der = overall_sys_der / (float)(gate_numbers-dff_num);
	fprintf(resultfp,"\n ================================= \n");
	fprintf(resultfp,"Overall Systematic Derating = %f \n", overall_sys_der);
	fprintf(resultfp, "Max event size in systematic derating=%d \n", max_event_size);

}//timing_derating
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
// Function: compute_vimin_vimax
// Purpose:  Computes vimin_vimax
// Returns none
void compute_vimin_vimax(struct gate* gateptr){
	short i;
	struct gate *pred_gateptr;

	//compute vimin,vimax based on inputs vimin, vimax
	gateptr->vimax = VDD;
	gateptr->vimin = VGD;
    for (i=0; i<gateptr->input_count; i++)
	{
		if (gateptr->input_list[i] > numPi)
		{
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
			if (pred_gateptr->vomax < gateptr->vimax)
				gateptr->vimax = pred_gateptr->vomax;
			if (pred_gateptr->vomin > gateptr->vimin)
				gateptr->vimin = pred_gateptr->vomin;
		}
	}
}//compute_vimin_vimax

/////////////////////////////////////////////////////////////////////////////////////
// Function: compute_vomin
// Purpose:  Computes compute_vomin
// Returns none
float compute_vomin(float* vomin_values, int input_count){
	int i;
	float vomin = VGD;
    for (i=0; i<input_count; i++)
	{
		if (vomin_values[i] > vomin)
			vomin = vomin_values[i];
	}
	return vomin;
}//compute_vomin


/////////////////////////////////////////////////////////////////////////////////////
// Function: compute_vomax

// Purpose:  Computes compute_vimax
// Returns none
float compute_vomax(float* vomax_values, int input_count){
	int i;
	float vomax = VDD;
    for (i=0; i<input_count; i++){
		if (vomax_values[i] < vomax)
			vomax = vomax_values[i];
	}
	return vomax;
}//compute_vomax

//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_gate_event_list
// Purpose:  compute_gate_event_list
// Returns none
void compute_q_gate_event_list(struct gate* gateptr){
	int input_list_index[config->MAX];
	int input_count = gateptr->input_count;
    float q0_values[MAX_GATE_INPUTS], q1_values[MAX_GATE_INPUTS];
    float qd_values[MAX_GATE_INPUTS], qdn_values[MAX_GATE_INPUTS];
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
		for (i=0; i<input_count; i++)
		{
			if (gateptr->input_list[i]<=numPi)
			{
				q0_values[i] = 1 - gateptr->input_values[i];
				q1_values[i] = gateptr->input_values[i];
				qd_values[i] = 0; qdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
				continue;
			}
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
			if (pred_gateptr->is_dff == 1)
			{
				q0_values[i] = 1 - gateptr->input_values[i];
				q1_values[i] = gateptr->input_values[i];
				qd_values[i] = 0; qdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
				continue;
			}
			q0_values[i] = pred_gateptr->event_list[input_list_index[i]]->q0;
			q1_values[i] = pred_gateptr->event_list[input_list_index[i]]->q1;
			qd_values[i] = pred_gateptr->event_list[input_list_index[i]]->qd;
			qdn_values[i] = pred_gateptr->event_list[input_list_index[i]]->qdn;
			vomin_values[i] = pred_gateptr->event_list[input_list_index[i]]->vomin;
			vomax_values[i] = pred_gateptr->event_list[input_list_index[i]]->vomax;
			level_one = false;
		}
		if (level_one == true)
		{
			eventptr = new struct event;
			eventptr->time = now_event_time;
			eventptr->q0 = 1; eventptr->q1  = 0;
			eventptr->qd = 0; eventptr->qdn = 0;
			eventptr->vomin = VGD; eventptr->vomax = VDD;
			add_event(gateptr,eventptr);
			prev_eventptr = eventptr;
			tran_delay1 = gateptr->tphl_load;
			first_event = false;
			//now_event_time = (int)gateptr->gd;
		}
		compute_Q_Probability(gateptr, q0_values, q1_values, qd_values, qdn_values);
		eventptr = new struct event;
		//now_event_time += (int)gateptr->gd;
		eventptr->time = now_event_time + (int)gateptr->gd;
		eventptr->q0 = gateptr->q0; eventptr->q1  = gateptr->q1;
		eventptr->qd = gateptr->qd; eventptr->qdn = gateptr->qdn;
		if (first_event)
			prev_eventptr = eventptr;
		vimin = compute_vomin(vomin_values,input_count);
		vimax = compute_vomax(vomax_values,input_count);
		eventptr->pwi = now_event_time - prev_event_time;
		tran_delay2 = (gateptr->q0==1) ? gateptr->tphl_load : gateptr->tplh_load;
		eventptr->vomin = (eventptr->pwi<gateptr->tphl) ? (1 - eventptr->pwi/(gateptr->tphl*1.25)) : VGD;
		eventptr->vomax = (eventptr->pwi<gateptr->tplh) ? (1 - eventptr->pwi/(gateptr->tplh*1.25)) : VDD;
		xmin = (VDD/2.0 - eventptr->vomin)/(VDD/2.0); //output L2H
		xmax = (eventptr->vomax - VDD/2.0)/(VDD/2.0); //output H2L
		xused = (gateptr->q1==1) ? xmin : xmax;
		eventptr->pwo = (int)(eventptr->pwi - tran_delay1 + xused*tran_delay2);
		if ((eventptr->q0==prev_eventptr->q0)&&(eventptr->q1==prev_eventptr->q1))
			eventptr->pwo = eventptr->pwi; // no change in the output
		eventptr->time += eventptr->pwo - eventptr->pwi;
		add_event(gateptr,eventptr);
		prev_eventptr = eventptr;
		tran_delay1 = tran_delay2;
		first_event = false;
		//start to find the next event
		next_event_time = max_event_time;
		next_event = MAX_GATE_INPUTS + 1;
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=numPi)
				continue;
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
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
}//compute_gate_event_list
/////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_gate_event_list
// Purpose:  compute_gate_event_list
// Returns none
void compute_gate_event_list(struct gate* gateptr){
	int input_list_index[config->MAX];
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
	for (i=0; i<input_count; i++)
	{
		input_list_index[i] = 0;
	}

	now_event_time = next_event_time = 0;
	prev_event_time = -100;
	do
	{
		for (i=0; i<input_count; i++)
		{
			if (gateptr->input_list[i]<=numPi)
			{
				p0_values[i] = 1 - gateptr->input_values[i];
				p1_values[i] = gateptr->input_values[i];
				pd_values[i] = 0; pdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
				continue;
			}
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
			if (pred_gateptr->is_dff == 1)
			{
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
		if (level_one == true)
		{
			eventptr = new struct event;
			eventptr->time = now_event_time;
			eventptr->p0 = 1; eventptr->p1  = 0;
			eventptr->pd = 0; eventptr->pdn = 0;
			eventptr->vomin = VGD; eventptr->vomax = VDD;
			add_event(gateptr,eventptr);
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
		add_event(gateptr,eventptr);
		prev_eventptr = eventptr;
		tran_delay1 = tran_delay2;
		first_event = false;
		//start to find the next event
		next_event_time = max_event_time;
		next_event = MAX_GATE_INPUTS + 1;
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=numPi)
				continue;
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
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
}//compute_gate_event_list
/////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_gate_event_list
// Purpose:  compute_gate_event_list
// Returns none
void compute_gate_event_list_old(struct gate* gateptr){
	int input_list_index[config->MAX];
	int input_count = gateptr->input_count;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	struct gate* pred_gateptr;
	int event_no=0;
	struct event* eventptr;
	int i;
	int now_event_time,next_event_time;
	//int max_event_time = tperiod + seu_pulse_width;
	int max_event_time = 3*config->tperiod + config->seu_pulse_width;
	int next_event;
	bool level_one = true;
	for (i=0; i<input_count; i++)
	{
		input_list_index[i] = 0;
	}

	now_event_time = 0; //gateptr->delay;
	do
	{
		for (i=0; i<input_count; i++)
		{
			if (gateptr->input_list[i]<=numPi)
			{
				p0_values[i] = 1 - gateptr->input_values[i];
				p1_values[i] = gateptr->input_values[i];
				pd_values[i] = 0; pdn_values[i] = 0;
				continue;
			}
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
			if (pred_gateptr->is_dff == 1)
			{
				p0_values[i] = 1 - gateptr->input_values[i];
				p1_values[i] = gateptr->input_values[i];
				pd_values[i] = 0; pdn_values[i] = 0;
				continue;
			}
			p0_values[i] = pred_gateptr->event_list[input_list_index[i]]->p0;
			p1_values[i] = pred_gateptr->event_list[input_list_index[i]]->p1;
			pd_values[i] = pred_gateptr->event_list[input_list_index[i]]->pd;
			pdn_values[i] = pred_gateptr->event_list[input_list_index[i]]->pdn;
			level_one = false;
		}
		if (level_one == true)
		{
			eventptr = new struct event;
			eventptr->time = now_event_time;
			eventptr->p0 = 1; eventptr->p1  = 0;
			eventptr->pd = 0; eventptr->pdn = 0;
			add_event(gateptr,eventptr);
			now_event_time = (int)gateptr->gd;
		}
		compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
		eventptr = new struct event;
		eventptr->time = now_event_time;
		eventptr->p0 = gateptr->p0; eventptr->p1  = gateptr->p1;
		eventptr->pd = gateptr->pd; eventptr->pdn = gateptr->pdn;
		add_event(gateptr,eventptr);
		//start to find the next event
		next_event_time = max_event_time;
		next_event = MAX_GATE_INPUTS + 1;
		for (i=0; i<input_count; i++)
		{
			if (gateptr->input_list[i]<=numPi)
				continue;
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
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
		//if (next_event_time == max_event_time) // we do not have any event left
			//break;
		input_list_index[next_event]++;
		now_event_time = next_event_time + (int)gateptr->gd;
	} while (next_event_time != max_event_time); // we do not have any event left
}//compute_gate_event_list
/////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_Q_gate_event_list
// Purpose:  compute_Q_gate_event_list
// Returns none
void compute_Q_gate_event_list(struct gate* gateptr, int targe_gate_node){
	int input_list_index[config->MAX];
	int input_count = gateptr->input_count;
    	float q0_values[MAX_GATE_INPUTS], q1_values[MAX_GATE_INPUTS];
        float qd_values[MAX_GATE_INPUTS], qdn_values[MAX_GATE_INPUTS];
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

	if (gateptr->out==targe_gate_node) return; //skip the faulty gate
	for (i=0; i<input_count; i++){
		input_list_index[i] = 0;
	}
;
	now_event_time = 0;//gateptr->gd;//gateptr->delay;
	do{
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=numPi){
		                q0_values[i] = 1- PI_list_sp[gateptr->input_list[i]];
                		q1_values[i] = PI_list_sp[gateptr->input_list[i]];
                		qd_values[i] = 0; qdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
				continue;
			}

			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
            		pred_gate_number = pred_gateptr->out;
			bool use_sp = false;
			if (pred_gateptr->is_dff == 1){
				use_sp = true;
			}

			if (pred_gateptr->is_onpath==false)
				use_sp = true;
			if (pred_gateptr->is_onpath==true)
			{
				//if (pred_gate_number<=(numPi+numPo)) use_sp = true;
				if (pred_gate_number<=(numPi)) use_sp = true;

				//We ignore the following when gateptr is DFF
				if (gateptr->is_dff == false)
				{
					//IMPORTANT: The following lines can be commented or not
					//it depends whether the propagated Ds are overlapped or not
					/*for (k=0; k<fadjlist[pred_gate_number].size(); k++)
						if (fadjlist[pred_gate_number][k]->type==7) //is DFF
							use_sp = true;
					*/
				}
			}
			if (use_sp==true){
				q0_values[i] = 1- pred_gateptr->signal_probability;
               			q1_values[i] = pred_gateptr->signal_probability;
		                qd_values[i] = 0;
                		qdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
            		}else{ //if (pred_gateptr->is_onpath==true)
				qd_values[i] = pred_gateptr->event_list[input_list_index[i]]->qd;
				qdn_values[i] = pred_gateptr->event_list[input_list_index[i]]->qdn;
				q1_values[i] = pred_gateptr->event_list[input_list_index[i]]->q1;
				q0_values[i] = pred_gateptr->event_list[input_list_index[i]]->q0;
				vomin_values[i] = pred_gateptr->event_list[input_list_index[i]]->vomin;
				vomax_values[i] = pred_gateptr->event_list[input_list_index[i]]->vomax;
			}
		}

		compute_Q_Probability(gateptr, q0_values, q1_values, qd_values, qdn_values);
		struct event *ev = new event();
		//now_event_time += (int)gateptr->gd;
		ev->time = now_event_time + (int)gateptr->gd;
		ev->q0 = gateptr->q0; ev->q1  = gateptr->q1;
		ev->qd = gateptr->qd; ev->qdn = gateptr->qdn;
		
		if (first_event)
			prev_eventptr = ev;
		vimin = compute_vomin(vomin_values,input_count);
		vimax = compute_vomax(vomax_values,input_count);
			

		ev->pwi = now_event_time - prev_event_time;
		tran_delay2 = (gateptr->q0==1) ? gateptr->tphl_load : gateptr->tplh_load;
		ev->vomin = (ev->pwi<gateptr->tphl) ? (1 - ev->pwi/(gateptr->tphl*1.25)) : VGD;
		ev->vomax = (ev->pwi<gateptr->tplh) ? (1 - ev->pwi/(gateptr->tplh*1.25)) : VDD;
		
		xmin = (VDD/2.0 - ev->vomin)/(VDD/2.0); //output L2H
		xmax = (ev->vomax - VDD/2.0)/(VDD/2.0); //output H2L
		xused = (gateptr->q1==1) ? xmin : xmax;
		ev->pwo = (int)(ev->pwi - tran_delay1 + xused*tran_delay2);
		
		if ((ev->q0==prev_eventptr->q0)&&(ev->q1==prev_eventptr->q1))
			ev->pwo = ev->pwi; // no change in the output
		ev->time += ev->pwo - ev->pwi;
		add_event(gateptr,ev);
		prev_eventptr = ev;
		tran_delay1 = tran_delay2;
		first_event = false;
		//start to find the next event
		next_event_time = max_event_time;
		next_event = MAX_GATE_INPUTS + 1;
	
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=numPi)
				continue;
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
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
}//compute_Q_gate_event_list
/////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_D_gate_event_list
// Purpose:  compute_D_gate_event_list
// Returns none
void compute_D_gate_event_list(struct gate* gateptr, int targe_gate_node){
	int input_list_index[config->MAX];
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

	if (gateptr->out==targe_gate_node) //skip the faulty gate
	{
		return;
	}

	for (i=0; i<input_count; i++){
		input_list_index[i] = 0;
	}

	now_event_time = 0;//gateptr->gd;//gateptr->delay;
	do{
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=numPi){
                p0_values[i] = 1- PI_list_sp[gateptr->input_list[i]];
                p1_values[i] = PI_list_sp[gateptr->input_list[i]];
                pd_values[i] = 0; pdn_values[i] = 0;
				vomin_values[i] = VGD; vomax_values[i] = VDD;
				continue;
			}

			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
            pred_gate_number = pred_gateptr->out;
			bool use_sp = false;
			if (pred_gateptr->is_dff == 1){
				use_sp = true;
			}

			if (pred_gateptr->is_onpath==false)
				use_sp = true;
			if (pred_gateptr->is_onpath==true)
			{
				if (pred_gate_number<=(numPi+numPo))
					use_sp = true;

				//We ignore the following when gateptr is DFF
				if (gateptr->is_dff == false)
				{
					//IMPORTANT: The following lines can be commented or not
					//it depends whether the propagated Ds are overlapped or not
					/*for (k=0; k<fadjlist[pred_gate_number].size(); k++)
						if (fadjlist[pred_gate_number][k]->type==7) //is DFF
							use_sp = true;
					*/
				}
			}
			if (use_sp==true)
            {
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
		add_event(gateptr,eventptr);
		prev_eventptr = eventptr;
		tran_delay1 = tran_delay2;
		first_event = false;
		//start to find the next event
		next_event_time = max_event_time;
		next_event = MAX_GATE_INPUTS + 1;
		for (i=0; i<input_count; i++){
			if (gateptr->input_list[i]<=numPi)
				continue;
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
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
}//compute_D_gate_event_list
/////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_D_gate_event_list
// Purpose:  compute_D_gate_event_list
// Returns none
void compute_D_gate_event_list_old(struct gate* gateptr, int targe_gate_node){
	int input_list_index[config->MAX];
	int input_count = gateptr->input_count;
    float p0_values[MAX_GATE_INPUTS], p1_values[MAX_GATE_INPUTS];
    float pd_values[MAX_GATE_INPUTS], pdn_values[MAX_GATE_INPUTS];
	struct gate* pred_gateptr;
	int event_no=0;
	struct event* eventptr;
	int i,pred_gate_number;
	int now_event_time,next_event_time;
	int max_event_time = 3*config->tperiod + config->seu_pulse_width;
	int next_event;

	if (gateptr->out==targe_gate_node) //skip the faulty gate
	{

		return;
	}

	for (i=0; i<input_count; i++)
	{
		input_list_index[i] = 0;
	}

	now_event_time = 0;//gateptr->gd;//gateptr->delay;
	do
	{
		for (i=0; i<input_count; i++)
		{
			if (gateptr->input_list[i]<=numPi)
			{
                p0_values[i] = 1- PI_list_sp[gateptr->input_list[i]];
                p1_values[i] = PI_list_sp[gateptr->input_list[i]];
                pd_values[i] = 0; pdn_values[i] = 0;
				continue;
			}

			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
            pred_gate_number = pred_gateptr->out;
			bool use_sp = false;
			if (pred_gateptr->is_dff == 1)
			{
				use_sp = true;
			}

			if (pred_gateptr->is_onpath==false)
				use_sp = true;
			if (pred_gateptr->is_onpath==true)
			{
				if (pred_gate_number<=(numPi+numPo))
					use_sp = true;

				//We ignore the following when gateptr is DFF
				if (gateptr->is_dff == false)
				{
					//IMPORTANT: The following lines can be commented or not
					//it depends whether the propagated Ds are overlapped or not
					/*for (k=0; k<fadjlist[pred_gate_number].size(); k++)
						if (fadjlist[pred_gate_number][k]->type==7) //is DFF
							use_sp = true;
					*/
				}
			}
			if (use_sp==true)
            {
				p0_values[i] = 1- pred_gateptr->signal_probability;
                p1_values[i] = pred_gateptr->signal_probability;
                pd_values[i] = 0;
                pdn_values[i] = 0;
				continue;
            }
			else //if (pred_gateptr->is_onpath==true)
			{
				pd_values[i] = pred_gateptr->event_list[input_list_index[i]]->pd;
				pdn_values[i] = pred_gateptr->event_list[input_list_index[i]]->pdn;
				p1_values[i] = pred_gateptr->event_list[input_list_index[i]]->p1;
				p0_values[i] = pred_gateptr->event_list[input_list_index[i]]->p0;
			}
		}
		compute_D_Probability(gateptr, p0_values, p1_values, pd_values, pdn_values);
		//eventptr = (struct event*) malloc(sizeof(struct event));
		eventptr = new struct event;
		eventptr->time = now_event_time;
		eventptr->p0 = gateptr->p0; eventptr->p1=gateptr->p1;
		eventptr->pd = gateptr->pd; eventptr->pdn = gateptr->pdn;
		add_event(gateptr,eventptr);
		//start to find the next event
		next_event_time = max_event_time;
		next_event = MAX_GATE_INPUTS + 1;
		for (i=0; i<input_count; i++)
		{
			if (gateptr->input_list[i]<=numPi)
				continue;
			pred_gateptr = fadjlist_ptr[gateptr->input_list[i]];
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
		//if (next_event_time == max_event_time) // we do not have any event left
			//break;
		input_list_index[next_event]++;
		now_event_time = next_event_time + (int)gateptr->gd;
	} while (next_event_time != max_event_time); // we do not have any event left
}//compute_D_gate_event_list
/////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////
// Function: add_event
// Purpose:  adds an event
// Returns none
void add_event(struct gate* gateptr, struct event* eventptr){
	gateptr->event_list.push_back(eventptr);
}//add_event
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: matrix_mult
// Purpose:  Matrix Multiplication
// Returns none
void matrix_mult(float** matd, int matdx, int matdy, int matcommonxy, float** mats1, float** mats2){
	int i,j,k;
	float fsum;
	float *entry1,*entry2;
	//cout << "matrix mult begins :" << endl;
	for (i=0; i<matdx; i++)
	{
		for (j=0; j<matdy; j++)
		{
			fsum = 0;
			//fsum = 1;
			for (k=0; k<matcommonxy; k++)
			{
				entry1 = mats1[i] + k;
				entry2 = mats2[k] + j;
				fsum += (*entry1) * (*entry2);
				//fsum *= 1 - (*entry1) * (*entry2);
			}
			//fsum = 1 - fsum;
			if (fsum>1)
				fsum = 1;
			entry1 = matd[i] + j;
			*entry1 = fsum;
		}
	}

}//matrix_mult
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// Function: matrix_copy
// Purpose:  Matrix Copy
// Returns none
void matrix_copy(float** matd, float** mats, int matdx, int matdy){
	int i,j;
	float *entry1,*entry2;

	for (i=0; i<matdx; i++)
	{
		for (j=0; j<matdy; j++)
		{
			entry1 = matd[i] + j;
			entry2 = mats[i] + j;
			*entry1 = *entry2;
		}
	}

}//matrix_copy
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Function: release_all_malloc
// Purpose:  release all memory resources
// Returns none
void release_all_malloc(void){
	int i,j,list_size;
	struct gate* gateptr;
	struct inout *p, *q;
	struct event* eventptr;

	//Free HeadPi
	p = headPi;
	while (p != NULL)
	{
		q = p;
		p = p->next;
		delete q;
	}

	//Free HeadPo
	p = headPo;
	// Move to end of list
	while (p != NULL)
	{
		q = p;
		p = p->next;
		delete q;
	}

	// Free gates
	for (i=0;i<gate_numbers; i++)
	{
		gateptr = gate_list[i];
		if (gateptr->input_list != NULL)
			delete [] gateptr->input_list;
		if (gateptr->input_values != NULL)
			delete [] gateptr->input_values;
		list_size = gateptr->event_list.size();
		for (j=0;j<list_size; j++)
		{
			eventptr = gateptr->event_list[j];
			delete eventptr;
		}
		delete gateptr;
	}

	struct dff *dffptr;

	// Free DFFs
	for (i=0;i<dff_num; i++)
	{
		dffptr = dfflist_ptr[i];
		delete dffptr;
	}

    if (PI_list != NULL)
        delete PI_list;

    if (PI_list_sp != NULL)
        delete PI_list_sp;

	try {
		cout << "deleting SFF ... ";
		for (i=0;i<dff_num; i++)
			delete[] SFF[i];
		delete[] SFF;
	}//try
	catch (...) {
		cout << "Couldn't delete all SFF!" << endl;
	}

	try {
		cout << "deleting SFO ... ";
		for (i=0;i<dff_num; i++)
			delete[] SFO[i];
		delete[] SFO;
	}//try
	catch (...) {
		cout << "Couldn't delete all SFO!" << endl;
	}

	try {
		cout << "deleting SGO ... ";
		for (i=0;i<gate_numbers; i++)
			delete[] SGO[i];
		delete[] SGO;
	}//try
	catch (...) {
		cout << "Couldn't delete all SGO!" << endl;
	}


	try {
		cout << "deleting SGF ... ";
		for (i=0;i<gate_numbers; i++)
			delete[] SGF[i];
		delete[] SGF;
	}//try
	catch (...) {
		cout << "Couldn't delete all SGF!" << endl;
	}


	try {
		cout << "deleting SatCK ... ";
		for (i=0;i<gate_numbers; i++)
			delete[] SatCK[i];
		delete[] SatCK;
	}//try
	catch (...) {
		cout << "Couldn't delete all SatCK!" << endl;
	}


	try {
		cout << "deleting S1_to_CK ... ";
		for (i=0;i<gate_numbers; i++)
			delete[] S1_to_CK[i];
		delete[] S1_to_CK;
	}//try
	catch (...) {
		cout << "Couldn't delete all S1_to_CK!" << endl;
	}


	try {
		cout << "deleting S ... ";
		delete[] S;
	}//try
	catch (...) {
		cout << "Couldn't delete all S!" << endl;
	}

	try {
		cout << "deleting Sck ... ";
		delete[] Sck;
	}//try
	catch (...) {
		cout << "Couldn't delete all Sck!" << endl;
	}

	try {
		cout << "deleting S_sim ... ";
		delete[] S_sim;
	}//try
	catch (...) {
		cout << "Couldn't delete all S_sim!" << endl;


	}

	try {
		cout << "deleting S_sim_var ... ";
		delete[] S_sim_var;
	}//try
	catch (...) {
		cout << "Couldn't delete all S_sim_var!" << endl;
	}


	try {
		cout << "deleting S_sim_clk ... ";
		delete[] S_sim_clk;
	}//try
	catch (...) {
		cout << "Couldn't delete all S_sim_clk!" << endl;
	}

	try {
		cout << "deleting S_sim_clk_var ... ";
		delete[] S_sim_clk_var;
	}//try
	catch (...) {
		cout << "Couldn't delete all S_sim_clk_var!" << endl;
	}


}//release_all_malloc
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: message_release_exit
// Purpose:  Prints out the error message, release all memories and exits the program
// Returns none
void check_print_release_exit(bool condition, char* message){
	if (condition==true){
		cout << endl << message << endl;
		release_all_malloc();
		exit(1);
	}
}//message_release_exit

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: message_release_exit
// Purpose:  Prints out the error message, release all memories and exits the program
// Returns none
void check_print_release_exit(bool condition, char* message1, char* message2){
	if (condition==true){
		cout << endl << message1 << message2 << endl;
		release_all_malloc();
		exit(1);
	}
}//message_release_exit


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: alloc_init_mem
// Purpose:  alloc_init_mem
// Returns none
void alloc_init_mem(void){
	int i;
	if(config->verbose) 	cout << endl << "Allocating dynamic memories" << endl;
	epp_ranking = new (nothrow) int[gate_numbers];
	epp_area_ranking = new (nothrow) int[gate_numbers];
	pi = new (nothrow) short[nodes+1];
	dfs_fnum = new (nothrow) short[nodes+1];
    //Clear dfs_fnum[], which records blackened timestamps
	for (i=0; i<=nodes; i++)
        dfs_fnum[i] = 0;

	check_print_release_exit(epp_ranking==NULL, "Couldn't allocate memory for epp_ranking!!");
	check_print_release_exit(epp_area_ranking==NULL, "Couldn't allocate memory for epp_area_ranking!!");
	check_print_release_exit(pi==NULL, "Couldn't allocate memory for pi!!");
	check_print_release_exit(dfs_fnum==NULL, "Couldn't allocate memory for dfs_fnum!!");


	SFF = new (nothrow) float*[dff_num];
	SFO = new (nothrow) float*[dff_num];
	SGO = new (nothrow) float*[gate_numbers];
	SGF = new (nothrow) float*[gate_numbers];
	SatCK = new (nothrow) float*[gate_numbers];
	S1_to_CK = new (nothrow) float*[gate_numbers];

	S = new (nothrow) float[gate_numbers];
	Sck = new (nothrow) float[gate_numbers];
	S_sim = new (nothrow) float[gate_numbers];
	S_sim_var = new (nothrow) float[gate_numbers];
	S_sim_clk = new (nothrow) float[gate_numbers];
	S_sim_single_cycle_po = new (nothrow) float[gate_numbers];
	S_sim_single_cycle_ff = new (nothrow) float[gate_numbers];
	S_sim_single_cycle_po_ff = new (nothrow) float[gate_numbers];
	eppPerGate_CLK1 = new (nothrow) float[gate_numbers];
	S_sim_clk_var = new (nothrow) float[gate_numbers];

	check_print_release_exit(SFF==NULL, "Couldn't allocate memory for SFF!!");
	check_print_release_exit(SFO==NULL, "Couldn't allocate memory for SFO!!");
	check_print_release_exit(SGO==NULL, "Couldn't allocate memory for SGO!!");
	check_print_release_exit(SGF==NULL, "Couldn't allocate memory for SGF!!");
	check_print_release_exit(SatCK==NULL, "Couldn't allocate memory for SatCK!!");
	check_print_release_exit(S1_to_CK==NULL, "Couldn't allocate memory for S1_to_CK!!");
	check_print_release_exit(S==NULL, "Couldn't allocate memory for S!!");
	check_print_release_exit(Sck==NULL, "Couldn't allocate memory for Sck!!");
	check_print_release_exit(S_sim==NULL, "Couldn't allocate memory for S_sim!!");
	check_print_release_exit(S_sim_var==NULL, "Couldn't allocate memory for S_sim_var!!");
	check_print_release_exit(S_sim_clk==NULL, "Couldn't allocate memory for S_sim_clk!!");
	check_print_release_exit(S_sim_clk_var==NULL, "Couldn't allocate memory for S_sim_clk_var!!");

	for (i=0; i<dff_num; i++){
		SFF[i] = new (nothrow) float[dff_num];
		SFO[i] = new (nothrow) float[numPo];
		check_print_release_exit(SFF[i]==NULL, "Couldn't allocate memory for SFF[i]!!");
		check_print_release_exit(SFO[i]==NULL, "Couldn't allocate memory for SFO[i]!!");
	}


	for (i=0; i<gate_numbers; i++){
		SGO[i] = new (nothrow) float[numPo];
		SGF[i] = new (nothrow) float[dff_num];
		SatCK[i] = new (nothrow) float[MAX_CLK];
		S1_to_CK[i] = new (nothrow) float[MAX_CLK];

		check_print_release_exit(SGF[i]==NULL, "Couldn't allocate memory for SGF[i]!!");
		check_print_release_exit(SGO[i]==NULL, "Couldn't allocate memory for SGO[i]!!");
		check_print_release_exit(SatCK[i]==NULL, "Couldn't allocate memory for SatCK[i]!!");
		check_print_release_exit(S1_to_CK[i]==NULL, "Couldn't allocate memory for S1_to_CK[i]!!");
	}

}//alloc_init_mem


//////////////////////////////////////////////////////////////////////////////////////
// Function: gate_name
// Purpose:  returns the gate ascii name, for debugging purpose only.
// Returns gate_name(string)
string gate_name(struct gate* gateptr){
	if(gateptr->is_dff==1) return "dff";
	switch (gateptr->type) {
        case 0:	return "nand" ;
        case 1: return "and" ;    //and
        case 2: return "nor" ;    //nor
        case 3: return "or" ;    //or
        case 4: return "xor" ;    //xor
        case 5: return "xnor" ;    //xnor
        case 6: return "not" ;     //not
		case 8: return "lut" ;
        default: return "buff" ;   // buff
    }

}//gate_name

//
// it is a temporarily function untill the core class is finished.
// this function may not work in the future, don't use this.
void migrate_core(Core* core){
	fadjlist =  core->fadjlist;					// forward graph
		fadjlist_ptr = core->fadjlist_ptr;			// a graph to point to the gate in fadjlist
		event_list_ptr = core->event_list_ptr;
		radjlist = core->radjlist;					// reverse graph
		netlist =  core->netlist;					// Used for mapping net names to short
		PI2POlist =  core->PI2POlist;				// Include all nodes on the paths from PIi to POj
		gate_list = core->gate_list;					// Gate list
		po_list =  core->po_list;					// Primary output list
		dfflist_ptr = core->dfflist_ptr;				// DFFs list

		headPi	= core->headPi;
		headPo	= core->headPo;
		numPi	= core->numPi;
		numPo	= core->numPo;
		PI_list = core->PI_list;						// used for assigning random values to the inputs during logic simulation
		PI_list_sp = core->PI_list_sp;				// used for input values signal probabilities !
		nodes	= core->nodes;							// Number of nodes in the circuit ** Number of nodes of the circuit is nodes-1
		gate_numbers = core->gate_numbers;			// number of gates in the circuit including flop-flops
		gate_numbers_noff = core->gate_numbers_noff; //number of gates excluding flip-flops
		sim_gate_numbers = core->sim_gate_numbers;	// the number of gates being simulated
		dff_num = core->dff_num;		
}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// END OF FILE ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


