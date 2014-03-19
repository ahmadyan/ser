
#include "core.h"
#include "main.h"

Core::Core(
		vector < deque<struct gate*> > _fadjlist,	// forward graph
		deque <struct gate*> _fadjlist_ptr,			// a graph to point to the gate in fadjlist
		deque <struct event*> _event_list_ptr,
		vector < deque<short> > _radjlist,			// reverse graph
		map <string, short> _netlist,               // Used for mapping net names to short
		vector < deque<short> > _PI2POlist,			// Include all nodes on the paths from PIi to POj
		deque < struct gate* > _gate_list,			// Gate list
		deque < struct gate* > _po_list,			// Primary output list
		deque < struct dff* >  _dfflist_ptr,			// DFFs list
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
		){
		
		fadjlist =  _fadjlist;					// forward graph
		fadjlist_ptr = _fadjlist_ptr;			// a graph to point to the gate in fadjlist
		event_list_ptr = _event_list_ptr;
		radjlist = _radjlist;					// reverse graph
		netlist =  _netlist;					// Used for mapping net names to short
		PI2POlist =  _PI2POlist;				// Include all nodes on the paths from PIi to POj
		gate_list = _gate_list;					// Gate list
		po_list =  _po_list;					// Primary output list
		dfflist_ptr = _dfflist_ptr;				// DFFs list


		headPi	= _headPi;
		headPo	= _headPo;
		numPi	= _numPi;
		numPo	= _numPo;
		PI_list = _PI_list;						// used for assigning random values to the inputs during logic simulation
		PI_list_sp = _PI_list_sp;				// used for input values signal probabilities !
		nodes	= _nodes;							// Number of nodes in the circuit ** Number of nodes of the circuit is nodes-1
		gate_numbers = _gate_numbers;			// number of gates in the circuit including flop-flops
		gate_numbers_noff = _gate_numbers_noff; //number of gates excluding flip-flops
		sim_gate_numbers = _sim_gate_numbers;	// the number of gates being simulated
		dff_num = _dff_num;						// number of flop-flops in the circuit

}

Core::Core(){
	//epp_ranking=NULL;// the sorted list of gate_list w.r.t their EPP values
	//epp_area_ranking=NULL; // the sorted list of gate_list w.r.t their EPP over area
	//pi=NULL; // it is used for dfs algorithm
	//dfs_fnum=NULL; // To detecte finalize number (black) after dfs completion
	//final_time_stamp = 0; // is used for topological sorting number during DFS algorithm
	//PI_list=NULL;				// used for assigning random values to the inputs during logic simulation
	//PI_list_sp=NULL;				// used for input values signal probabilities !
	gate_numbers=0; // number of gates in the circuit including flop-flops
	gate_numbers_noff=0; //number of gates excluding flip-flops
	sim_gate_numbers=0; // the number of gates being simulated
	dff_num=0;	  // number of flop-flops in the circuit
	//SFO=NULL;//[MAX_DFF][MAX_PO];
	//SFF=NULL;//[MAX_DFF][MAX_DFF]; //Flip-flops probabaility Mij = P(fault appear in FFi | FFj is faulty)
	//SGO=NULL;//[MAX][MAX_PO];
	//SGF=NULL;//[MAX][MAX_DFF];
	//SatCK=NULL;//[MAX][MAX_CLK];  //SatCK[i][j] is the failure probability of the i'th gate in the gate_list at cycle j-1
	//S1_to_CK=NULL;//[MAX][MAX_CLK];
	//S=NULL;//[MAX],
	//eppPerGate_CLK1=NULL;
	//Sck=NULL;//[MAX]; //System failure probability vector Si = P(system failure | FFi is faulty)
	//Mtemp = NULL;
	//MC = NULL;
	//SGOatCK = NULL;
	//tmp_mat = NULL;


	//S_sim=NULL;//[MAX]; //System failure probability vector obtained from simulation
	//S_sim_clk=NULL;//[MAX]; //System failure probability vector obtained from simulation
	//S_sim_var=NULL;//[MAX]; //Variances of system failure probability vector obtained from simulation
	//S_sim_clk_var=NULL;//[MAX]; //Variances of system failure probability vector obtained from simulation

	
}


Core::Core(configuration* config){
	srandom(time(0));
	
	deque <short> tmp, tmp_constant;
	deque <struct gate*> tmp1;
	struct gate* temp_gate_ptr=NULL;
	struct dff* temp_dff_ptr=NULL;
	

	// Generate adjacency lists for storing graph representations of the circuit
	for(int i=0; i<config->MAX; i++){
		fadjlist.push_back(tmp1);
		radjlist.push_back(tmp);
		reachOp.push_back(tmp);
		fadjlist_ptr.push_back(temp_gate_ptr);
		dfflist_ptr.push_back(temp_dff_ptr);
		gate_list.push_back(temp_gate_ptr);
		po_list.push_back(temp_gate_ptr);
	}

	// nodes is the number of nodes in the circuit
	nodes = 1;
	
	PI_list=NULL;				// used for assigning random values to the inputs during logic simulation
	PI_list_sp=NULL;				// used for input values signal probabilities !
	
	gate_numbers=0; // number of gates in the circuit including flop-flops
	gate_numbers_noff=0; //number of gates excluding flip-flops
	sim_gate_numbers=0; // the number of gates being simulated
	dff_num=0;	  // number of flop-flops in the circuit
	

}
//////////////////////////////////////////////////////////////////////////////////////
// Function: appendPi
// Purpose: Forms a linked list of all input nodes
void Core::appendPi(struct inout *headPi, short net){
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
void Core::appendPo(struct inout *headPo, short net){
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

void Core::report_netlist(){
	
		map<string,short>::iterator it;

  // show content:
	 for ( it=netlist.begin() ; it != netlist.end(); it++ )
    cout << (*it).first << " => " << (*it).second << endl;

  cout << "-----------------------" << endl ;
		for(int i=0;i<gate_numbers;i++){
			struct gate* gptr1 = gate_list[i];
			cout << i << " " << gptr1->gate_no << " " <<   gptr1->type << "\t OUT=" << gptr1->out <<   " \t " << gptr1->input_count << " \t IN= " ;
			for(int j=0;j< gptr1->input_count;j++) cout << gptr1->input_list[j] << " " ; cout << endl ;
		}
		
}



// Function: readNetlist_verilog
// Purpose:  Function to read netlist and generate a graph of the circuit
short Core::readNetlist_verilog(FILE *fp, vcd* VCD, configuration* config, FILE* resultfp){
	char c, field[200], field_Gate_Instance_Name[200];
	short x, out, type, input_count;
	short input_list [MAX_GATE_INPUTS];
	struct inout *ptr;
	struct gate *gateptr;
	struct dff *dffptr;
	short i;
	typedef pair <string, short> p1;
	map <string, short> :: iterator iter;

	numPi = numPo = 0;

	do{// skip if x=3 (blank space, end of line, etc)
		do{
			x = extractField(fp, field);
			if ((x==EOF)||(x<0)) break;;
		} while ( (x == 3) || (x==2));

		if ((x==EOF)||(x<0)) break;;
	    // if 'module' - skip
		if(strcmp(field, "module") == 0){
			do{
				c = fgetc(fp);
			} while (c != ';');
			c = fgetc(fp);
		}
		// end of file
		else if(strcmp(field, "endmodule") == 0) break;
		//return nodes;
		// If 'input'
		else if(strcmp(field, "input") == 0){
			if(config->verbose) cout << endl << "starts inputs!" << endl;
			do{
				x = extractField(fp, field);
				if ((x==EOF)||(x<0))
					break;;
				if (x==3) continue;
				if(strcmp(field, "GND") == 0) continue;
				if(strcmp(field, "VDD") == 0) continue;
				if(strcmp(field, "CK") == 0) continue;

				if(config->verbose) cout << "    Input: " << field << " Mapping: " << nodes << endl ;
				fprintf(resultfp, "\tInput: %s  Mapping: %d\n", field, nodes);

				// Map net name to integer
				netlist.insert(p1(field, nodes));
				
				if(config->use_vcd){
					VCD->addPI(string(field), nodes);
				}

		        // Add mapped net in input linked list
        		if(numPi == 0){
					//ptr = (struct inout*) malloc(sizeof(struct inout));
					ptr = new (struct inout);
					ptr->num = nodes;
					ptr->next = NULL;
					headPi = ptr;
					numPi++;
        		}else{
					// Append primary input to PI linked list
					numPi++;
					appendPi(headPi, nodes);
				}
				nodes++;
			} while (x != 2);
			if ((x==EOF)||(x<0))
				break;;
			if(config->verbose) cout << "inputs done!" << endl;
		}
		// If 'output'
		else if(strcmp(field, "output") == 0){
			if(config->verbose) cout << "starts outputs!" << endl;
			do {
				x = extractField(fp, field);
				if ((x==EOF)||(x<0))
					break;;
				if (x==3)
					continue;
				if(config->verbose) cout << "    Output: " << field << " Mapping: " << nodes << endl ;
				fprintf(resultfp, "\tOutput: %s  Mapping: %d\n", field, nodes);

				// Map net name to integer
				netlist.insert(p1(field, nodes));
				nodes++;

				// Add mapped net to input linked list
				if(numPo == 0){
					//ptr = (struct inout*) malloc(sizeof(struct inout));
					ptr = new(struct inout);
					ptr->num = nodes-1;
					ptr->next = NULL;
					headPo = ptr;
					numPo++;
				}
				else
				{
          		// Append primary output to output linked list
					numPo++;
					appendPo(headPo, nodes-1);
				}
			} while (x != 2);
			if ((x==EOF)||(x<0))
				break;;
			if(config->verbose) cout << "outputs done!" << endl;
		}

		// If 'wire', map all net names to integers
		else if(strcmp(field, "wire") == 0){
			if(config->verbose) cout << "starts wires!" << endl;
			do{
				x = extractField(fp, field);
				if ((x==EOF)||(x<0))
					break;;
				if (x==3)
					continue;
				if(config->verbose) cout << "    Wire: " << field << " Mapping: " << nodes << endl ;
				fprintf(resultfp, "\tWire: %s  Mapping: %d\n", field, nodes);

				// Map net name to integer
				netlist.insert(p1(field, nodes));
				nodes++;

			} while (x != 2);
			if ((x==EOF)||(x<0))
				break;;
			if(config->verbose) cout << "wires done!" << endl;
    	}// Gate
		else{
			//cout << "starting gates!" << endl;		
			// Extract gate type
			if(strcmp(field, "nand") == 0)
			  type = 0;
			else if(strcmp(field, "and") == 0)
			  type = 1;
			else if(strcmp(field, "nor") == 0)
			  type = 2;
			else if(strcmp(field, "or") == 0)
			  type = 3;
			else if(strcmp(field, "xor") == 0)
			  type = 4;
			else if(strcmp(field, "xnor") == 0)
			  type = 5;
			else if(strcmp(field, "not") == 0)
			  type = 6;
			else if(strcmp(field, "dff") == 0)
			  type = 7;
			else if(strcmp(field, "buf") == 0)
			  type = 9; //BUFF
			else{
				cout << endl << "Unrecognized gate ==> Exit" << endl;
				cout << "gate number: " << gate_numbers << endl;
				cout << "gate name: " << field << endl;
				exit(1);
			}
			cout << "gate number: " << gate_numbers << endl;
			cout << "gate name: " << field << endl;
			if(config->verbose)  cout <<  "  Type is: " << type;

			// Extract instance name
			x = extractField(fp, field_Gate_Instance_Name);
			//cout << x << endl ;
			if ((x==EOF)||(x<0))
				break;;
			if(config->verbose) cout << endl << "Instance: " << field <<"::" << field_Gate_Instance_Name;

			if (type == 7) // skip clock for DFF
				short xtemp = extractField(fp, field);
			// Extract output
			x = extractField(fp, field);
			iter = netlist.find(field);
			//cout << endl << field << " "  << iter->first << "  " << iter->second;

			// we add dff's output as PI, this is used to restore DFF's from VCD file
			if(config->use_vcd){ 
				if(type==7) // is dff?
					VCD->addPI(string(iter->first), iter->second);
			}

			out = iter->second;

			// Extract input1
			x = extractField(fp, field);
			iter = netlist.find(field);
			input_count = 0;
			input_list[input_count++] = iter->second;

			// Extract input2
			if((type>=0)&&(type<=5)) {  // for all gates except "not" and "dff"
				x = extractField(fp, field);
				if ((x==EOF)||(x<0))
					break;;
				while (x != 2){
				  iter = netlist.find(field);
				  input_list[input_count++] = iter->second;
				  x = extractField(fp, field);
				}
			}
			//replacing the first with last! this is inverse :)
			//this style of input is provided from Design Compiler synthesized netlist.
			//if we're running for ISCAS testbench, disable REVERSE_INPUT
			if(config->input_synopsys_style){
				if(type!=7){//except for dffs
					short temp = input_list[input_count-1] ;
					input_list[input_count-1] = out ;
					out = temp ;
					if(config->verbose) cout << endl << "inverse!  input=" << input_list[input_count-1] << " output=" <<  out << endl ;
				}
			}


			// Add net information to forward graph
			//gateptr = (struct gate*) malloc(sizeof(struct gate));
			//gateptr->input_list = (short*) malloc(input_count*sizeof(short));
			//gateptr->input_values = (short*) malloc(input_count*sizeof(short));
			gateptr = new(struct gate);
			gateptr->input_list = new short[input_count];
			gateptr->input_values = new short[input_count];
			gateptr->input_count = input_count;
			gateptr->out = out;
			gateptr->type = type;
			gateptr->sum_values = 0;

			for (i=0; i<input_count; i++){
				gateptr->input_list[i] = input_list [i];
				// Update forward adjacency list
				fadjlist[input_list[i]].push_front(gateptr);
				//struct gate *gate_point = fadjlist[input_list[i]][0];
				// Update reverse adjacency list
				radjlist[out].push_front(input_list[i]);
				short is_empty = radjlist[input_list[i]].size(); // Checks if the input gate is defined before
				if ((input_list[i]>(numPi+numPo)) && (is_empty==0) && (type!=7)){
					/*short temp_number;
					cout << endl;
					cout << "ALARM: The circuit gates are not in order. " << "out: " << out << endl;
					cin >> temp_number;
					fprintf(resultfp, "\n ALARM: The circuit gates are not in order. out: %d", out);*/
				}
			}
			fadjlist_ptr[out] = gateptr;
            gate_list[gate_numbers++] = gateptr;
			int ttts = gateptr->event_list.size();
			gateptr->is_dff = (type==7) ? 1:0;
			if (type == 7){
				//dffptr = (struct dff*) malloc(sizeof(struct dff));
				dffptr = new struct dff;
				dffptr->d = input_list[0];
				dffptr->q = out;
				//dffptr->sum_values = 0;
			  //fadjlist[input_list[0]].push_front(dffptr);
			  //radjlist[out].push_front(input_list[0]);
				dfflist_ptr[dff_num++] = dffptr;
				dffptr->dgateptr = gateptr;
			}
		}
		if ((x==EOF)||(x<0))
			break;;

	} while(1);

	if (nodes>config->MAX){
		cout << "[Error] Circuit has more nodes that the defined Max value, please increase MAX in configuration or by -m option ==> Exit" << endl;
		exit(1);
	}

	if (dff_num>MAX_DFF){
		cout << "MAX_DFF is not large enough!!! ==> Exit" << endl;
		exit(1);
	}
	if (numPo>MAX_PO){
		cout << "MAX_PO is not large enough!!! ==> Exit" << endl;
		exit(1);
	}


	int dff_counter=0, po_counter=0;
    for (i=0; i<gate_numbers; i++){
		gateptr = gate_list[i];
		gateptr->gate_no = i;
		if (gateptr->is_dff)
			gateptr->dff_no = dff_counter++;
		else
			gateptr->dff_no = -1;

		if (gateptr->out <=(numPi+numPo))
		{
			po_list[po_counter] = gateptr;
			gateptr->po_no = po_counter++;
		}
		else
			gateptr->po_no = -1;
	}

	return nodes;
	fflush(resultfp);
}//readNetlist_verilog


/////////////////////////////////////////////////////////////////////////////////////
// Function: arange_gates
// Purpose:  Arranges all gates Except DFFs
// Returns none
void Core::arange_gates(){
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
        exit(1);
    }
}//arange_gates




//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_signal_probability
// Purpose:  Computes signal probabilities of the circuit
// Returns none
void Core::compute_signal_probability(configuration* config, vcd* VCD, FILE* resultfp){
	int step_iteration = config->step_iteration ;
	long max_iteration = config->max_iteration ;
    long i;
    struct gate* gptr1;
	float z_alpha_half = 2.576;
	float error_tolerance = sp_error_tolerance;
	long total_iteration = 0;

	 // Initialize the sum_values
    if (PI_list_sp == NULL) PI_list_sp = new float[numPi+1];
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
	logic_simulation(0,VCD,config);
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
			logic_simulation(1,VCD,config);

		circuit_total_iteration += step_iteration;
		for (i=0; i<gate_numbers; i++){
			gptr1 = gate_list[i];
			sum_values = (float)gptr1->sum_values + (gptr1->signal_probability * (float)gptr1->total_iteration);
			gptr1->total_iteration += step_iteration;
			gptr1->signal_probability = sum_values/(float)gptr1->total_iteration;
			//sum_values = (sum_values==0) ? 1 : sum_values; // this is added to remove variance=0 from calculation
			gptr1->total_variance = sqrt((gptr1->signal_probability * (1 - gptr1->signal_probability)) / (float)gptr1->total_iteration);
			if (gptr1->signal_probability==0)
				gptr1->total_variance = sqrt(((1.0/(float)gptr1->total_iteration) * (1 - (1.0/(float)gptr1->total_iteration))) / (float)gptr1->total_iteration);
		}
		
		long tmp_iter1=0; //,tmp_iter2;
		circuit_total_variance = 0;
		for (i=0; i<gate_numbers; i++){
			gptr1 = gate_list[i];
			if (gptr1->total_variance > circuit_total_variance)
				circuit_total_variance = gptr1->total_variance;
		}

		if(config->verbose) cout << "simulated signal prob. for " << circuit_total_iteration << endl;
	} while (((z_alpha_half*circuit_total_variance) > error_tolerance) && (circuit_total_iteration<max_iteration) );
	

    for (i=1; i<=numPi; i++)
        PI_list_sp[i] = (float) PI_list_sp[i] / (float) circuit_total_iteration;

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
void Core::signal_probability_report(FILE* resultfp){
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
// Function: extractField
// Purpose:  Extracts one field from input netlist file
// Returns 1 if field is success
short Core::extractField(FILE *fp, char *field){
  short i=0;
  char x;

  while(1) {
    x = fgetc(fp);
	if ((x==EOF)||(x<0))	return -1;
    if (i==0){
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
void Core::logic_simulation(short run_for_sp, vcd* signals, configuration* config){
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
			for (i=1; i<=numPi; i++){
				printf("input %d = %d \n", i, PI_list[i]);
			}
			for (i=0; i<gate_numbers; i++)
				printf("(gate=%d) Node %4d = %d \n", i,gate_list[i]->out, gate_list[i]->value);
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
	if (is_in_range==false) printf("ALARM: logic value is not 0 or 1! \n");
}//logic_simulation



//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_gate_value
// Purpose:  Computes the gate value
// Returns none
// Revised on 5-1389, adding support for lut-type gate
void Core::compute_gate_value(struct gate* gateptr){
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




/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Function: compute_circuit_delay_from_library
// Purpose:  Computes the delays of all gates in the circuit
// compute_circuit_delay : linear delay model
// compute_circuit_delay_from_library : non-linear delay model
// Returns none
void Core::compute_circuit_delay_from_library(library* lib){
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
	for (int i=0;i<gate_numbers;i++){
		gptr1= gate_list[i];
		cout << i << "::" << gptr1->delay << endl ; 
	// 	fprintf(resultfp, "%d\t:: %f \n", i, gptr1->delay);
	}
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
void Core::compute_circuit_delay(bool slack_resizing){
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
void Core::initialize_gate_delay(struct gate *gptr1){
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


//////////////////////////////////////////////////////////////////////////////////////
// Function: gate_name
// Purpose:  returns the gate ascii name, for debugging purpose only.
// Returns gate_name(string)
string Core::gate_name(struct gate* gateptr){
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

