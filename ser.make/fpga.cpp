#include "fpga.h"
#include "defines.h"
using namespace std;

fpga::fpga(configuration* _config){
	config = _config;
	cout << "[init] circuit type: fpga" << endl ;
}


fpga::~fpga(void){}

/* This source file will read in a FLAT blif netlist consisting     *
* of .inputs, .outputs, .names and .latch commands.  It currently   *
* does not handle hierarchical blif files.  Hierarchical            *
* blif files can be flattened via the read_blif and write_blif      *
* commands of sis.  LUT circuits should only have .names commands;  *
* there should be no gates.  This parser performs limited error     *
* checking concerning the consistency of the netlist it obtains.    *
* .inputs and .outputs statements must be given; this parser does   *
* not infer primary inputs and outputs from non-driven and fanout   *
* free nodes.  This parser can be extended to do this if necessary, *
* or the sis read_blif and write_blif commands can be used to put a *
* netlist into the standard format.                                 *
* V. Betz, August 25, 1994.                                         *
* Modified by Adel, July 2010										*/
void fpga::read_blif (FILE* blif, Core* core) {
	cout << "reading blif file" << endl ;
	int lut_size = config->fpga.lut_size;
	char buffer[BUFSIZE];
	int pass, done, doall;
	//blif = my_fopen (blif_file, "r", 0);

	for (doall=0;doall<=1;doall++) {
		init_parse(doall);
		/* Three passes to ensure inputs are first blocks, outputs second and    *
		* LUTs and latches third.  Just makes the output netlist more readable. */
		for (pass=1;pass<=3;pass++) { 
			linenum = 0;   /* Reset line number. */
			done = 0;
			while((my_fgets(buffer,BUFSIZE,blif) != NULL) && !done) {
				get_tok(buffer, pass, doall, &done, lut_size);
			}
			rewind (blif);  /* Start at beginning of file again */
		}
	} 
	fclose(blif);
	check_net(lut_size);
	free_parse();
}

void trreverse(char* begin, char* end);
void itoa(int value, char* str, int base);


int fpga::getNodeId(char* nodeName){
	for(int i=0;i<num_nets;i++){
		char str[80] ;
		strcpy (str,"out:");
		strcat (str,net[i].name);
		if( (strcmp(nodeName, net[i].name)==0) || (strcmp(nodeName, str)==0) ) return i ;
	}
	return -1;
}

int fpga::convert_blif_to_core(FILE* resultfp, vcd* VCD, Core* core){
	core->nodes=1;
	int lut_size = config->fpga.lut_size ;
	typedef pair <string, short> node;
	map <string, short> :: iterator iter;
	struct inout *ptr;
	struct gate* gateptr;
	struct dff* dffptr;
	cout << "number of blocks=" << num_blocks << endl ;
	cout << "number of nets=" << num_nets << endl ;
	
	core->numPi = 0 ;
	core->numPo = 0 ;
	
	for (int i=0;i<num_nets;i++) {
		core->netlist.insert(node(string(net[i].name), core->nodes));
		core->nodes++;
	}

	for(int i=0;i<num_blocks;i++) {
		if(block[i].type == INPAD){
				int nodeID = core->netlist.find( block[i].name )->second ;
				if(config->use_vcd){
					VCD->addPI(string(block[i].name), nodeID);
				}
				// Add mapped net in input linked list
				if(core->numPi == 0){
					ptr = new (struct inout);
					ptr->num =  nodeID;
					ptr->next = NULL;
					core->headPi = ptr;
					core->numPi++;
				}else{
					// Append primary input to PI linked list
					core->numPi++;
					core->appendPi(core->headPi,  nodeID);
				}
			}
		if(block[i].type == OUTPAD){
				int nodeID =  getNodeId(block[i].name) ;
				// Add mapped net to input linked list
				if(core->numPo == 0){
					ptr = new(struct inout);
					ptr->num = nodeID;
					ptr->next = NULL;
					core->headPo = ptr;
					core->numPo++;
				}else{
					// Append primary output to output linked list
					core->numPo++;
					core->appendPo(core->headPo, nodeID);
				}
			}
	
		if( (block[i].type == LATCH) || (block[i].type == LUT ) ){
			if(config->verbose){
				cout << i << " " <<  block[i].name << " " << block[i].type << " " << block[i].num_nets   ;
				if( block[i].type == LATCH ){
					cout << " type:ff " ;
				}else{
					cout << " type:lut " ;
				}
				cout << " output= " << block[i].nets[0] ;
				cout << " input= " ;
				for(int j=1;j< block[i].num_nets; j++){
					cout << " " <<  block[i].nets[j] ; 
				}
				cout << endl ;
			}
			int out = core->netlist.find(net[block[i].nets[0]].name)->second;
			gateptr = new(struct gate);
			gateptr->input_list = new short[block[i].num_nets-1];
			gateptr->input_values = new short[block[i].num_nets-1];
			gateptr->input_count = block[i].num_nets-1;
			gateptr->out = out ; 
			gateptr->sum_values = 0;
			
			for (int j=1; j<block[i].num_nets; j++){
				int x =  core->netlist.find( net[block[i].nets[j]].name )->second ;
				gateptr->input_list[j-1] = x;
				core->fadjlist[x].push_front(gateptr);
				core->radjlist[out].push_front(x);
			}
			core->fadjlist_ptr[out] = gateptr;
			core->gate_list[ core->gate_numbers++] = gateptr;
			if( block[i].type == LUT ){
				gateptr->is_lut=true;
				gateptr->is_dff=false ;
				gateptr->type = 8; //LUT
				gateptr->lut_size = 1 << block[i].num_nets-1;
				gateptr->lut = new short[gateptr->lut_size] ;
				gateptr->fault = new short[gateptr->lut_size] ;
				for(int j=0;j<gateptr->lut_size;j++){
					gateptr->lut[j]=0;
					gateptr->fault[j]=0;
					for(int k=0;k<block[i].value->size();k++){
						if( match( block[i].value->at(k), j ) ){
							gateptr->lut[j]=1;
						}	
					}
				}
			}	
			
			if( block[i].type == LATCH ){
				gateptr->is_dff = true;
				gateptr->is_lut = false ;
				gateptr->type = 7; //FF
				dffptr = new struct dff;
				dffptr->d = block[i].nets[1];
				dffptr->q = out;
				core->dfflist_ptr[core->dff_num++] = dffptr;
				dffptr->dgateptr = gateptr;
			}
		}
	}

	if (core->nodes>config->MAX){
		cout << "[Error] Circuit has more nodes that the defined Max value, please increase MAX in configuration or by -m option ==> Exit" << endl;
		std::exit(1);
	}

	if (core->dff_num>MAX_DFF){
		cout << "MAX_DFF is not large enough!!! ==> Exit" << endl;
		std::exit(1);
	}
	if (core->numPo>MAX_PO){
		cout << "MAX_PO is not large enough!!! ==> Exit" << endl;
		std::exit(1);
	}
		
	int dff_counter=0, po_counter=0;
    for (int i=0; i<core->gate_numbers; i++){
		gateptr = core->gate_list[i];
		gateptr->gate_no = i;
		if (gateptr->is_dff) gateptr->dff_no = dff_counter++;
		else gateptr->dff_no = -1;
		if (gateptr->out <=(core->numPi+core->numPo)){
			core->po_list[po_counter] = gateptr;
			gateptr->po_no = po_counter++;
		}else{
			gateptr->po_no = -1;
		}
	}
	return core->nodes;
}


void fpga::init_parse(int doall) {
	/* Allocates and initializes the data structures needed for the parse. */

	int i, len;
	struct hash_nets *h_ptr;

	if (!doall) {  /* Initialization before first (counting) pass */
		num_nets = 0;  
		hash = (struct hash_nets **) calloc(sizeof(struct hash_nets *), HASHSIZE);
	} else {   /* Allocate memory for second (load) pass */ 
		net = (struct s_net *) malloc(num_nets*sizeof(struct s_net));
		block = (struct s_block *) malloc(num_blocks*sizeof(struct s_block));  
		for(int i=0;i<num_blocks;i++){
			block[i].value = new vector<string>();
		}
		num_driver = (int *) malloc(num_nets * sizeof(int));
		temp_num_pins = (int *) malloc(num_nets*sizeof(int));

		for (i=0;i<num_nets;i++) {
			num_driver[i] = 0;
			net[i].num_pins = 0;
		}

		for (i=0;i<HASHSIZE;i++) {
			h_ptr = hash[i];   
			while (h_ptr != NULL) {
				net[h_ptr->index].pins = (int *) malloc(h_ptr->count*sizeof(int));
				/* For avoiding assigning values beyond end of pins array. */
				temp_num_pins[h_ptr->index] = h_ptr->count;
				len = strlen (h_ptr->name);
				net[h_ptr->index].name = (char *) malloc ((len + 1)* sizeof(char));
				strcpy (net[h_ptr->index].name, h_ptr->name);
				h_ptr = h_ptr->next;
			}
		}
		/*    printf("i\ttemp_num_pins\n\n");
		for (i=0;i<num_nets;i++) {
		printf("%d\t%d\n",i,temp_num_pins[i]);
		}  */
	}

	/* Initializations for both passes. */

	ilines = 0;
	olines = 0;
	model_lines = 0;
	endlines = 0;
	num_p_inputs = 0;
	num_p_outputs = 0;
	num_luts = 0;
	num_latches = 0;
	num_blocks = 0;
}

int lut=0;
void fpga::get_tok (char *buffer, int pass, int doall, int *done, int lut_size) {
	/* Figures out which, if any token is at the start of this line and *
	* takes the appropriate action.                                    */
#define TOKENS " \t\n"
	char *ptr; 
	//lut's look-up-table!
	if ((doall==1) && (buffer[0] != '.') ){
		//cout << num_blocks-1 << " " << buffer << " " <<  block[num_blocks-1].value->size() << endl ;
		if(buffer[strlen(buffer)-1]=='\n') buffer[strlen(buffer)-1]='\0';
		block[num_blocks-1].value->push_back(string(buffer));
	}
	ptr = my_strtok(buffer,TOKENS,blif,buffer);


	if (ptr == NULL) return; 

	if (strcmp(ptr,".names") == 0) {
		if (pass == 3) {
			lut++ ;
			add_lut(doall, lut_size);
		} else {
			dum_parse(buffer);
		}
		return;
	}

	if (strcmp(ptr,".latch") == 0) {
		if (pass == 3) {
			add_latch (doall, lut_size);
		}else {
			dum_parse(buffer);
		}
		return;
	}

	if (strcmp(ptr,".model") == 0) {
		ptr = my_strtok(NULL,TOKENS,blif,buffer);

		if (doall && pass == 3) { /* Only bother on main second pass. */
			if (ptr != NULL) {
				model = (char *) malloc ((strlen(ptr)+1) * sizeof(char));
				strcpy(model,ptr);
			}else {
				model = (char *) malloc (sizeof(char));
				model[0] = '\0';
			}
			model_lines++;              /* For error checking only */
		}
		return;
	}

	if (strcmp(ptr,".inputs") == 0){
		if (pass == 1) {
			io_line(DRIVER, doall);
			*done = 1;
		}else{
			dum_parse(buffer);
			if (pass == 3 && doall) ilines++;    /* Error checking only */
		}
		return;
	}

	if (strcmp(ptr,".outputs") == 0) {
		if (pass == 2) {
			io_line(RECEIVER, doall);
			*done = 1;
		} else {
			dum_parse(buffer);
			if (pass == 3 && doall) olines++;  /* Make sure only one .output line */
		}                            /* For error checking only */
		return;
	}

	if (strcmp(ptr,".end") == 0) {
		if (pass == 3 && doall) endlines++;   /* Error checking only */
		return;
	}

	

	
		
	/* Could have numbers following a .names command, so not matching any *
	* of the tokens above is not an error.                               */

}


void fpga::dum_parse(char *buf) {
/* Continue parsing to the end of this (possibly continued) line. */
 while (my_strtok(NULL,TOKENS,blif,buf) != NULL);
}


void fpga::add_lut (int doall, int lut_size) {
	/* Adds a LUT (.names) currently being parsed to the block array.  Adds *
	* its pins to the nets data structure by calling add_net.  If doall is *
	* zero this is a counting pass; if it is 1 this is the final (loading) *
	* pass.                                                                */

	char *ptr, saved_names[MAXLUT+2][BUFSIZE], buf[BUFSIZE];
	int i, j, len;

	num_blocks++;

	/* Count # nets connecting */
	i=0;
	while ((ptr = my_strtok(NULL,TOKENS,blif,buf)) != NULL)  {
		if (i == MAXLUT+1) {
			fprintf(stderr,"Error:  LUT #%d has %d inputs.  Increase MAXLUT or"
				" check the netlist, line %d.\n",num_blocks-1,i-1,linenum);
			exit(1);
		}
		strcpy (saved_names[i], ptr);
		i++;
	}

	if (!doall) {          /* Counting pass only ... */
		for (j=0;j<i;j++) 
			add_net(saved_names[j],RECEIVER,num_blocks-1,doall);
		return;
	}

	block[num_blocks-1].num_nets = i;
	block[num_blocks-1].type = LUT;
	for (i=0;i<block[num_blocks-1].num_nets-1;i++){   /* Do inputs */
		block[num_blocks-1].nets[i+1] = add_net (saved_names[i],RECEIVER,num_blocks-1,doall); 
		//cout << "\t IN=" << saved_names[i] << " " << block[num_blocks-1].nets[i+1] ;
	}
	block[num_blocks-1].nets[0] = add_net (saved_names[block[num_blocks-1].num_nets-1], DRIVER,num_blocks-1,doall);
	//cout << "\t OUT=" << saved_names[block[num_blocks-1].num_nets-1] << " " << block[num_blocks-1].nets[0] << endl ;
	for (i=block[num_blocks-1].num_nets; i<lut_size+2; i++)
		block[num_blocks-1].nets[i] = OPEN;

	len = strlen (saved_names[block[num_blocks-1].num_nets-1]);
	block[num_blocks-1].name = (char *) malloc ((len+1) * sizeof (char));
	strcpy(block[num_blocks-1].name, saved_names[block[num_blocks-1].num_nets-1]);
	num_luts++;
}


void fpga::add_latch (int doall, int lut_size) {
	/* Adds the flipflop (.latch) currently being parsed to the block array.  *
	* Adds its pins to the nets data structure by calling add_net.  If doall *
	* is zero this is a counting pass; if it is 1 this is the final          * 
	* (loading) pass.  Blif format for a latch is:                           *
	* .latch <input> <output> <type (latch on)> <control (clock)> <init_val> *
	* The latch pins are in .nets 0 to 2 in the order: Q D CLOCK.            */

	char *ptr, buf[BUFSIZE], saved_names[6][BUFSIZE];
	int i, len;

	num_blocks++;

	/* Count # parameters, making sure we don't go over 6 (avoids memory corr.) */
	/* Note that we can't rely on the tokens being around unless we copy them.  */

	for (i=0;i<3;i++) {
		ptr = my_strtok (NULL,TOKENS,blif,buf);
		if (ptr == NULL) 
			break;
		strcpy (saved_names[i], ptr);
	}

	//if (i != 5) {
	//	fprintf(stderr,"Error:  .latch does not have 5 parameters.\n"
	//		"check the netlist, line %d.\n",linenum);
	//	exit(1);
	//}

	if (!doall) {   /* If only a counting pass ... */
		add_net(saved_names[0],RECEIVER,num_blocks-1,doall);  /* D */
		add_net(saved_names[1],DRIVER,num_blocks-1,doall);    /* Q */
		//add_net(saved_names[3],RECEIVER,num_blocks-1,doall);  /* Clock */
		return;
	}

	//block[num_blocks-1].num_nets = 3;
	block[num_blocks-1].num_nets = 2; //we don't do clock
	block[num_blocks-1].type = LATCH;

	block[num_blocks-1].nets[0] = add_net(saved_names[1],DRIVER,num_blocks-1,doall);  /* Q */
	block[num_blocks-1].nets[1] = add_net(saved_names[0],RECEIVER,num_blocks-1,doall); /* D */
	//cout << "FF(" << num_blocks-1 <<") =>  D=" << saved_names[0] << " " << block[num_blocks-1].nets[1]<< " \t Q=" << saved_names[1] << " " << block[num_blocks-1].nets[0]<< endl ;
	//block[num_blocks-1].nets[lut_size+1] = add_net(saved_names[3],RECEIVER,num_blocks-1,doall); /* Clock */

	for (i=2;i<lut_size+1;i++) 
		block[num_blocks-1].nets[i] = OPEN;

	len = strlen (saved_names[1]);
	block[num_blocks-1].name = (char *) malloc ((len+1) * sizeof (char));
	strcpy(block[num_blocks-1].name,saved_names[1]);
	num_latches++;
}


void fpga::io_line(int in_or_out, int doall) {  
	/* Adds an input or output block to the block data structures.           *
	* in_or_out:  DRIVER for input, RECEIVER for output.                    *
	* doall:  1 for final pass when structures are loaded.  0 for           *
	* first pass when hash table is built and pins, nets, etc. are counted. */ 

	char *ptr;
	char buf2[BUFSIZE];
	int nindex, len;
	while (1) {
		ptr = my_strtok(NULL,TOKENS,blif,buf2);
		if (ptr == NULL) return;
		num_blocks++;

		nindex = add_net(ptr,in_or_out,num_blocks-1,doall); 
		/* zero offset indexing */
		if (!doall) continue;   /* Just counting things when doall == 0 */

		len = strlen (ptr);
		if (in_or_out == RECEIVER) {  /* output pads need out: prefix 
									  to make names unique from LUTs */
			block[num_blocks-1].name = (char *) malloc ((len+1+4) *
				sizeof (char));          /* Space for out: at start */
			strcpy(block[num_blocks-1].name,"out:");
			strcat(block[num_blocks-1].name,ptr);
		}else{
			block[num_blocks-1].name = (char *) malloc ((len+1) * sizeof (char));     
			strcpy(block[num_blocks-1].name,ptr);
		}

		block[num_blocks-1].num_nets = 1;
		block[num_blocks-1].nets[0] = nindex;  /* Put in driver position for */
		/*  OUTPAD, since it has only one pin (even though it's a receiver */

		if (in_or_out == DRIVER) {             /* processing .inputs line */
			num_p_inputs++;
			block[num_blocks-1].type = INPAD;
		}else{                                 /* processing .outputs line */
			num_p_outputs++;
			block[num_blocks-1].type = OUTPAD;
		}
	} 
}


int fpga::add_net (char *ptr, int type, int bnum, int doall) {   
/* This routine is given a net name in *ptr, either DRIVER or RECEIVER *
 * specifying whether the block number given by bnum is driving this   *
 * net or in the fan-out and doall, which is 0 for the counting pass   *
 * and 1 for the loading pass.  It updates the net data structure and  *
 * returns the net number so the calling routine can update the block  *
 * data structure.                                                     */

 struct hash_nets *h_ptr, *prev_ptr;
 int index, j, nindex;

 index = hash_value(ptr);
 h_ptr = hash[index]; 
 prev_ptr = h_ptr;

 while (h_ptr != NULL) {
    if (strcmp(h_ptr->name,ptr) == 0) { /* Net already in hash table */
       nindex = h_ptr->index;

       if (!doall) {   /* Counting pass only */
          (h_ptr->count)++;
          return (nindex);
       }

       net[nindex].num_pins++;
       if (type == DRIVER) {
          num_driver[nindex]++;
          j=0;           /* Driver always in position 0 of pinlist */
       }
       else {
          j = net[nindex].num_pins - num_driver[nindex]; 
   /* num_driver is the number of signal drivers of this net. *
    * should always be zero or 1 unless the netlist is bad.   */
          if (j >= temp_num_pins[nindex]) {
             printf("Error:  Net #%d (%s) has no driver and will cause\n",
                nindex, ptr);
             printf("memory corruption.\n");
             exit(1);
          }
       }
       net[nindex].pins[j] = bnum;
       return (nindex);
    }
    prev_ptr = h_ptr;
    h_ptr = h_ptr->next;
 }

 /* Net was not in the hash table. */

 if (doall == 1) {
    printf("Error in add_net:  the second (load) pass could not\n");
    printf("find net %s in the symbol table.\n", ptr);
    exit(1);
 }

/* Add the net (only counting pass will add nets to symbol table). */

 num_nets++;
 h_ptr = (struct hash_nets *) malloc (sizeof(struct hash_nets));
 if (prev_ptr == NULL) {
    hash[index] = h_ptr;
 }     
 else {  
    prev_ptr->next = h_ptr;
 }    
 h_ptr->next = NULL;
 h_ptr->index = num_nets - 1;
 h_ptr->count = 1;
 h_ptr->name = (char *) malloc((strlen(ptr)+1)*sizeof(char));
 strcpy(h_ptr->name,ptr);
 return (h_ptr->index);
}


int fpga::hash_value (char *name) {
 int i,k;
 int val=0, mult=1;
 
 i = strlen(name);
 k = max (i-7,0);
 for (i=strlen(name)-1;i>=k;i--) {
    val += mult*((int) name[i]);
    mult *= 10;
 }
 val += (int) name[0];
 val %= HASHSIZE;
 return(val);
}


void fpga::echo_input (char *blif_file, int lut_size, char *echo_file) {
/* Echo back the netlist data structures to file input.echo to *
 * allow the user to look at the internal state of the program *
 * and check the parsing.                                      */

 int i, j, max_pin; 
 FILE *fp;

 printf("Input netlist file: %s  Model: %s\n", blif_file, model);
 printf("Primary Inputs: %d.  Primary Outputs: %d.\n", num_p_inputs, num_p_outputs);
 printf("LUTs: %d.  Latches: %d.\n", num_luts, num_latches);
 printf("Total Blocks: %d.  Total Nets: %d\n", num_blocks, num_nets);
 
 fp = my_fopen (echo_file,"w",0); 

 fprintf(fp,"Input netlist file: %s  Model: %s\n",blif_file,model);
 fprintf(fp,"num_p_inputs: %d, num_p_outputs: %d, num_luts: %d,"
            " num_latches: %d\n",num_p_inputs,num_p_outputs,num_luts,
             num_latches);
 fprintf(fp,"num_blocks: %d, num_nets: %d\n",num_blocks,num_nets);

 fprintf(fp,"\nNet\tName\t\t#Pins\tDriver\tRecvs.\n");
 for (i=0;i<num_nets;i++) {
    fprintf(fp,"\n%d\t%s\t", i, net[i].name);
    if (strlen(net[i].name) < 8)
       fprintf(fp,"\t");         /* Name field is 16 chars wide */
    fprintf(fp,"%d", net[i].num_pins);
    for (j=0;j<net[i].num_pins;j++) 
        fprintf(fp,"\t%d",net[i].pins[j]);
 }

 fprintf(fp,"\n\n\nBlocks\t\t\tBlock Type Legend:\n");
 fprintf(fp,"\t\t\tINPAD = %d\tOUTPAD = %d\n", INPAD, OUTPAD);
 fprintf(fp,"\t\t\tLUT = %d\t\tLATCH = %d\n", LUT, LATCH);
 fprintf(fp,"\t\t\tEMPTY = %d\tLUT_AND_LATCH = %d\n\n", EMPTY, 
       LUT_AND_LATCH);
 
 fprintf(fp,"\nBlock\tName\t\tType\t#Nets\tOutput\tInputs");
 for (i=0;i<lut_size;i++) 
    fprintf(fp,"\t");
 fprintf(fp,"Clock\n\n");

 for (i=0;i<num_blocks;i++) { 
    fprintf(fp,"\n%d\t%s\t",i, block[i].name);
    if (strlen(block[i].name) < 8)
       fprintf(fp,"\t");         /* Name field is 16 chars wide */
    fprintf(fp,"%d\t%d", block[i].type, block[i].num_nets);

   /* I'm assuming EMPTY blocks are always INPADs when I print               *
    * them out.  This is true right after the netlist is read in, and again  *
    * after ff_packing and compression of the netlist.  It's not true after  *
    * ff_packing and before netlist compression.                             */

    if (block[i].type == INPAD || block[i].type == OUTPAD || 
          block[i].type == EMPTY)
       max_pin = 1;
    else
       max_pin = lut_size+2;

    for (j=0;j<max_pin;j++) {
        if (block[i].nets[j] == OPEN) 
           fprintf(fp,"\tOPEN");
        else
           fprintf(fp,"\t%d",block[i].nets[j]);
    }
 }  

 fprintf(fp,"\n");
 fclose(fp);
}


void fpga::check_net (int lut_size) {

	/* Checks the input netlist for obvious errors. */

	int i, error, iblk;

	error = 0;

	if (ilines != 1) {
		printf("Warning:  found %d .inputs lines; expected 1.\n",
			ilines);
		error++;
	}

	if (olines != 1) {
		printf("Warning:  found %d .outputs lines; expected 1.\n",
			olines);
		error++;
	}

	if (model_lines != 1) {
		printf("Warning:  found %d .model lines; expected 1.\n",
			model_lines);
		error++;
	}

	if (endlines != 1) {
		printf("Warning:  found %d .end lines; expected 1.\n",
			endlines);
		error++;
	}

	for (i=0;i<num_nets;i++) {

		if (num_driver[i] != 1) {
			printf ("Warning:  net %s has"
				" %d signals driving it.\n",net[i].name,num_driver[i]);
			error++;
		}

		if ((net[i].num_pins - num_driver[i]) < 1) {

			/* If this is an input pad, it is unused and I just remove it with  *
			* a warning message.  Lots of the mcnc circuits have this problem. */

			iblk = net[i].pins[0];
			if (block[iblk].type == INPAD) {
				printf("Warning:  Input %s is unused; removing it.\n",
					block[iblk].name);
				net[i].pins[0] = OPEN;
				block[iblk].type = EMPTY;
			}

			else {
				printf("Warning:  net %s has no fanout.\n",net[i].name);
				error++;
			}
		}

		if (strcmp(net[i].name, "open") == 0) {
			printf("Warning:  net #%d has the reserved name %s.\n",i,net[i].name);
			error++;
		}
	}

	for (i=0;i<num_blocks;i++) {
		if (block[i].type == LUT) {
			if (block[i].num_nets < 2) {
				printf("Warning:  logic block #%d with output %s has only %d pin.\n",
					i,block[i].name,block[i].num_nets);

				/* LUTs with 1 pin (an output)  can be a constant generator.  Warn the   *
				* user, but don't exit.                                                 */

				if (block[i].num_nets != 1) {
					error++;
				}
				else {
					printf("\tPin is an output -- may be a constant generator.\n");
					printf("\tNon-fatal error.\n");
				}
			}

			if (block[i].num_nets > lut_size + 1) {
				printf("Warning:  logic block #%d with output %s has %d pins.\n",
					i,block[i].name,block[i].num_nets);
				error++;
			}
		}

		else if (block[i].type == LATCH) {
			if ( (block[i].num_nets != 2) ) {
				printf("Warning:  Latch #%d with output %s has %d pin(s).\n",
					i, block[i].name, block[i].num_nets);
				//error++;
			}
		}

		else {
			if (block[i].num_nets != 1) {
				printf("Warning:  io block #%d with output %s of type %d"
					"has %d pins.\n", i, block[i].name, block[i].type,
					block[i].num_nets);
				error++;
			}
		}
	}

	if (error != 0) {
		printf("Found %d fatal errors in the input netlist.\n",error);
		//exit(1);
	}
}


void fpga::free_parse (void) {  

/* Release memory needed only during blif network parsing. */

 int i;
 struct hash_nets *h_ptr, *temp_ptr;

 for (i=0;i<HASHSIZE;i++) {
    h_ptr = hash[i];
    while (h_ptr != NULL) {
       free ((void *) h_ptr->name);
       temp_ptr = h_ptr->next;
       free ((void *) h_ptr);
       h_ptr = temp_ptr;
    }
 }
 free ((void *) num_driver);
 free ((void *) hash);
 free ((void *) temp_num_pins);
}


FILE* fpga::my_fopen (char *fname, char *flag, int prompt) {
 FILE *fp;   /* prompt = 1: prompt user.  prompt=0: use fname */

 while (1) {
    if (prompt) 
       scanf("%s",fname);
    if ((fp = fopen(fname,flag)) != NULL)
       break; 
    printf("Error opening file %s for %s access.\n",fname,flag);
    if (!prompt) 
       exit(1);
    printf("Please enter another filename.\n");
 }
 return (fp);
}


char* fpga::my_fgets_old(char *buf, int max_size, FILE *fp) {
	stringstream ss ;
	char *val;
	int i;
	bool cont_line=false;
	do{
		cont=0;
		char* val = fgets(buf,max_size,fp);
		linenum++;
		if (val == NULL) return(val);

		for (i=0;i<max_size;i++) {
			if (buf[i] == '\n') break;
			if (buf[i] == '\0') {
				printf("Error on line %d -- line is too long for input buffer.\n", linenum);
				printf("All lines must be at most %d characters long.\n",BUFSIZE-2);
				printf("The problem could also be caused by a missing newline.\n");
				exit (1);
			}
		}
		for (i=0;i<max_size && buf[i] != '\0';i++) {
			if (buf[i] == '#') {
				buf[i] = '\0';
				break;
			}
		}

		if (i<2) return (val);
		if (buf[i-1] == '\n' && buf[i-2] == '\\') { 
			cont_line=true;
			cont = 1;   /* line continued */
			buf[i-2] = ' ';  /* May need this for tokens */
			buf[i-1] = ' ';
			ss << buf << "   ";
			val = (char*)(ss.str().c_str());
		}else{
			ss << val << "   " ;
		}
	}while(cont==1);
	val =  (char*)(ss.str().c_str()) ;
	for (i=0;i<max_size && buf[i] != '\0';i++){}
	buf[i-2] = '\n';  /* May need this for tokens */
	buf[i-1] = '\0';
	return (char*)(ss.str().c_str());
}


char* fpga::my_fgets(char *buf, int max_size, FILE *fp) {
 /* Get an input line, update the line number and cut off *
  * any comment part.  A \ at the end of a line with no   *
  * comment part (#) means continue.                      */

 char *val;
 int i;
 
 cont = 0;
 val = fgets(buf,max_size,fp);
 linenum++;
 if (val == NULL) return(val);

/* Check that line completely fit into buffer.  (Flags long line   *
 * truncation).                                                    */

 for (i=0;i<max_size;i++) {
    if (buf[i] == '\n') 
       break;
    if (buf[i] == '\0') {
       printf("Error on line %d -- line is too long for input buffer.\n",
          linenum);
       printf("All lines must be at most %d characters long.\n",BUFSIZE-2);
       printf("The problem could also be caused by a missing newline.\n");
       exit (1);
    }
 }


 for (i=0;i<max_size && buf[i] != '\0';i++) {
    if (buf[i] == '#') {
        buf[i] = '\0';
        break;
    }
 }

 if (i<2) return (val);
 if (buf[i-1] == '\n' && buf[i-2] == '\\') { 
	cont = 1;   /* line continued */
    buf[i-2] = '\n';  /* May need this for tokens */
    buf[i-1] = '\0';
 }
 return(val);
}


char* fpga::my_strtok(char *ptr, char *tokens, FILE *fp, char *buf) {
/* Get next token, and wrap to next line if \ at end of line.    *
 * There is a bit of a "gotcha" in strtok.  It does not make a   *
 * copy of the character array which you pass by pointer on the  *
 * first call.  Thus, you must make sure this array exists for   *
 * as long as you are using strtok to parse that line.  Don't    *
 * use local buffers in a bunch of subroutines calling each      *
 * other; the local buffer may be overwritten when the stack is  *
 * restored after return from the subroutine.                    */

 char *val;

 val = strtok(ptr,tokens);
 while (1) {
    if (val != NULL || cont == 0) return(val);
   /* return unless we have a null value and a continuation line */
    if (my_fgets(buf,BUFSIZE,fp) == NULL) 
       return(NULL);
    val = strtok(buf,tokens);
 }
}

bool fpga::match(string in, int value){
	bool match_result = true;
	char* buf = (char*)(in.c_str()) ;
	char* pattern;
	pattern = strtok (buf," ");
	char buffer[10] ;
	itoa (value,buffer,2);
	if(  strlen(buffer) < strlen(pattern) ){
		for(int i=0;i<strlen(pattern)-strlen(buffer);i++){
			char str[10];
			strcpy(str,"0");
			strcat(str,buffer);
			strcpy(buffer,str);
		}
	}

	for(int i=0;i<strlen(buffer);i++){
		if( (buffer[i] != pattern[i]) && (pattern[i]!='-') ){
			match_result=false;
		}
	}
	return match_result ; 
}