#include "vcd.h"
#include "utils.h"

using namespace std;
vcd::vcd(){
	pool = new wavePool();
}

int vcd::read_vcd(char *vname){
	FILE *f;
	char *buf = NULL;
	size_t glen;
	int line = 0;
	size_t ss;
	uint64_t prev_tim = 0;
	char bin_fixbuff[32769];

	f = fopen(vname, "rb");

	if(!f){
		printf("could not open '%s', exiting.\n", vname);
		exit(255);
	}

	while(!feof(f)){
		ss = utils::getline_replace(&buf, &glen, f);
		if(ss == -1){
			break;
		}
		line++;

		if(!strncmp(buf, "$var", 4)){
			char *st = strtok(buf+5, " \t");
			enum fstVarType vartype;
			int len;
			char *nam;
			unsigned int hash;

			vartype = FST_VT_VCD_WIRE;
			switch(st[0]){
			case 'w':
				if(!strcmp(st, "wire")){}
				else if(!strcmp(st, "wand")){
					vartype = FST_VT_VCD_WAND;
				}else if(!strcmp(st, "wor")){
					vartype = FST_VT_VCD_WOR;
				}
				break;

			case 'r':
				if(!strcmp(st, "reg")){
					vartype = FST_VT_VCD_REG;
				}else if(!strcmp(st, "real")){
					vartype = FST_VT_VCD_REAL;
				}else if(!strcmp(st, "real_parameter")){
					vartype = FST_VT_VCD_REAL_PARAMETER;
				}else if(!strcmp(st, "realtime")){
					vartype = FST_VT_VCD_REALTIME;
				}
				break;

			case 'p':
				if(!strcmp(st, "parameter")){
					vartype = FST_VT_VCD_PARAMETER;
				}else if(!strcmp(st, "port")){
					vartype = FST_VT_VCD_PORT;
				}
				break;

			case 'i':
				vartype = FST_VT_VCD_INTEGER;
				break;

			case 'e':
				vartype = FST_VT_VCD_EVENT;
				break;

			case 's':
				if(!strcmp(st, "supply1")){
					vartype = FST_VT_VCD_SUPPLY1;
				}else if(!strcmp(st, "supply0")){
					vartype = FST_VT_VCD_SUPPLY0;
				}
				break;

			case 't':
				if(!strcmp(st, "time")){
					vartype = FST_VT_VCD_TIME;
				}else if(!strcmp(st, "tri")){
					vartype = FST_VT_VCD_TRI;
				}else if(!strcmp(st, "triand")){
					vartype = FST_VT_VCD_TRIAND;
				}else if(!strcmp(st, "trior")){
					vartype = FST_VT_VCD_TRIOR;
				}else if(!strcmp(st, "trireg")) {
					vartype = FST_VT_VCD_TRIREG;
				} else if(!strcmp(st, "tri0")){
					vartype = FST_VT_VCD_TRI0;
				}else if(!strcmp(st, "tri1")){
					vartype = FST_VT_VCD_TRI1;
				}
				break;

			default:
				break;			
			}

			st = strtok(NULL, " \t");
			len = atoi(st);
			if(vartype == FST_VT_VCD_PORT){len = (len * 3) + 2;}

			st = strtok(NULL, " \t"); /* vcdid */
			string c = string(st);
			hash = utils::vcdid_hash(st);

			/* old code:
			nam = strtok(NULL, " \t"); // name 
			st = strtok(NULL, " \t"); // $end 
			if(strncmp(st, "$end", 4)){
				*(st-1) = ' ';
			}
			*/
			//bugfix
			// I wrote this code only to support vectors in vcd file, arrays usually appear as
			// some [10] form, however in verilog we support them like some[10] form, this code will concat
			// them. it was either this or this: string.erase(std::remove_if(string.begin(), string.end(), std::isspace), string.end());
			// but I like this more!
			nam = new char[80];
			nam[0]=0;
			bool lineNotFinished=true;
			do{
				st =  strtok(NULL, " \t") ;
				if(strncmp(st, "$end", 4)==0){ //$end
					lineNotFinished=false;
				}else{
					strcat(nam, st);
				}
			}while(lineNotFinished);
			pool->addwave(hash, c, string(nam));

		}
		else if(!strncmp(buf, "$scope", 6)){
			char *st = strtok(buf+6, " \t");
			enum fstScopeType scopetype;

			if(!strcmp(st, "module"))       {scopetype = FST_ST_VCD_MODULE;}
			else if(!strcmp(st, "task"))    {scopetype = FST_ST_VCD_TASK;}
			else if(!strcmp(st, "function")){scopetype = FST_ST_VCD_FUNCTION;}
			else if(!strcmp(st, "begin"))   {scopetype = FST_ST_VCD_BEGIN;}
			else if(!strcmp(st, "fork"))    {scopetype = FST_ST_VCD_FORK;}
			else                            {scopetype = FST_ST_VCD_MODULE;}

			st = strtok(NULL, " \t");
			}else if(!strncmp(buf, "$upscope", 8)){
			}else if(!strncmp(buf, "$endd", 5)){ break;	}
			else if(!strncmp(buf, "$timescale", 10)) {
				char *pnt;
				char *num = NULL;
				int exp = -9;
				int tv = 1;
				if((pnt = strstr(buf, "$end"))){
					*pnt = 0;
					num = strchr(buf, '1');
				}
				if(!num){
					ss = utils::getline_replace(&buf, &glen, f);
					if(ss == -1){ break; }
						line++;
						num = buf;
					}
					pnt = num;
					while(*pnt){
						int mat = 0;
						switch(*pnt){
							case 'm': exp = -3; mat = 1; break;
							case 'u': exp = -6; mat = 1; break;
							case 'n': exp = -9; mat = 1; break;
							case 'p': exp = -12; mat = 1; break;
							case 'f': exp = -15; mat = 1; break;
							case 's': exp = -0; mat = 1; break;
							default: break;
							}
						if(mat) break;
						pnt++;
					}

					tv = atoi(num);
					if(tv == 10){
						exp++;
					}else if(tv == 100){
						exp+=2;
					}

					////fstWriterSetTimescale(ctx, exp);
					printf("vcd timescale = 10^%d \n", exp);
				}else if(!strncmp(buf, "$date", 5)) {
					char *pnt;
					ss = utils::getline_replace(&buf, &glen, f);
					if(ss == -1){ break; }
						line++;
						pnt = buf;
						while(*pnt == '\t') pnt++;
						////fstWriterSetDate(ctx, pnt);
					}else if(!strncmp(buf, "$version", 8)) {
						char *pnt, *crpnt;
						ss = utils::getline_replace(&buf, &glen, f);
						if(ss == -1){ break; }
							line++;
							pnt = buf;
							while(*pnt == '\t') pnt++;
							crpnt = strchr(pnt, '\n');
							if(crpnt) *crpnt = 0;
							crpnt = strchr(pnt, '\r');
							if(crpnt) *crpnt = 0;
							////fstWriterSetVersion(ctx, pnt);
					}
				}
	while(!feof(f)){
		unsigned int hash;
		uint64_t tim;
		size_t len;
		char *nl, *sp;
		double doub;

		ss = utils::getline_replace(&buf, &len, f);
//		cout << buf << endl ;
		if(ss == -1){ break; }
		nl = strchr(buf, '\n');
		if(nl) *nl = 0;
		nl = strchr(buf, '\r');
		if(nl) *nl = 0;
		//printf("%c %x\n" , buf[0],  buf[0]);
		switch(buf[0]){
		case '0':
		case '1':
		case 'x':
		case 'z':
			hash = utils::vcdid_hash(buf+1);
			pool->addtransient(hash, buf[0]);
			/*
			node = jrb_find_int(vcd_ids, hash);
			if(node)
			{
			////				fstWriterEmitValueChange(ctx, node->val.i, buf);

			printf("ValueChange = %s %s\n", node->val.i, buf);
			}
			else
			{
			}
			*/
			break;

		case 'b':
			sp = strchr(buf, ' ');
			*sp = 0;
			hash = utils::vcdid_hash(sp+1);
/*			node = jrb_find_int(vcd_ids, hash);
			if(node)
			{
				int bin_len = strlen(buf+1);
				int node_len = node->val2.i;
				if(bin_len == node_len)
				{
					////					fstWriterEmitValueChange(ctx, node->val.i, buf+1);
				}
				else
				{
					int delta = node_len - bin_len;
					memset(bin_fixbuff, buf[1] != '1' ? buf[1] : '0', delta);
					memcpy(bin_fixbuff + delta, buf+1, bin_len);
					////				fstWriterEmitValueChange(ctx, node->val.i, bin_fixbuff);
				}
			}
			else
			{
			}*/
			break;

		case 'p':
			{
				char *src = buf+1;
				char *pnt = bin_fixbuff;
				int pchar = 0;

				for(;;)
				{
					if((*src == '\n') || (*src == '\r')) break;
					if(isspace(*src))
					{
						if(pchar != ' ') { *(pnt++) = pchar = ' '; }
						src++;
						continue;
					}
					*(pnt++) = pchar = *(src++);
				}
				*pnt = 0;

				sp = strchr(bin_fixbuff, ' ');
				sp = strchr(sp+1, ' ');
				sp = strchr(sp+1, ' ');
				*sp = 0;
				hash = utils::vcdid_hash(sp+1);
			/*	node = jrb_find_int(vcd_ids, hash);
				if(node)
				{
					////			fstWriterEmitValueChange(ctx, node->val.i, bin_fixbuff);
				}
				else
				{
				}*/
			}
			break;

		case 'r':
			sp = strchr(buf, ' ');
			hash = utils::vcdid_hash(sp+1);
			/*node = jrb_find_int(vcd_ids, hash);
			if(node)
			{
				sscanf(buf+1,"%lg",&doub); 
				////		fstWriterEmitValueChange(ctx, node->val.i, &doub);
			}
			else
			{
			}*/
			break;

		case '#':
			//sscanf(buf+1, "%"SCNu64, &tim);
			sscanf(buf+1, "%d", &tim);
			if((tim >= prev_tim)||(!prev_tim)){
				prev_tim = tim;
				pool->setTime(tim);
			}
			break;

		default:
			if(!strncmp(buf, "$dumpon", 7)){
				printf("$dumpon \n");
			}else if(!strncmp(buf, "$dumpoff", 8)) {
				printf("$dumpoff \n");
			}else if(!strncmp(buf, "$dumpvars", 9)) {
				/* nothing */
			}else {
				/* printf("FST '%s'\n", buf); */
			}
			break;
		}
	}

	if(buf){
		free(buf);
	}
	fclose(f);
	pool->finalle();
}

void vcd::dump(){
	pool->dump();
}

/*
this method is internal, it should be called from read_netlist 
to assign each primary input to an incremental unique id in netlist
*/
void vcd::addPI(string name, int id){
	pi_list.push_back(make_pair(name,id));
	//cout << "VCD --> " << name << " " << id << endl ;
}

short vcd::getNextLogicValue(int id){
	//cout << "*" ;
	try{
		for(int i=0;i<pi_list.size();i++){
			if( pi_list.at(i).second == id ) return pool->getNextLogicValue(pi_list.at(i).first , clk) ;
		}
	}catch(...){
		cout << "[Error] Signal not found in VCD file (" << id << "). does the vcd file match the netlist?" << endl; 
		exit(1);
	}
}

short wavePool::getNextLogicValue(string signalName, int clk){
	//cout << "*" ;
	for(int i=0;i<waves->size();i++){
		if(waves->at(i)->getName() == signalName )
			return waves->at(i)->getNextLogicValue(clk);
	}
	cout << "no signal matched with " << signalName << " in vcd file" << endl ;
	return 0;
}

short wave::getNextLogicValue(int clk){
	int i = 0 ;
	int c = 0 ;
	for(int i=0;i<transients.size();i++){
		if( transients.at(i).first >= (clk*counter) ){
			c=i;
			break;
		}
	}
	
	//while( (transients.at(i).first < (clk*counter) ) 
	//	&& (i<transients.size()) ){
	//		cout << i << " (" << transients.at(i).first << ") " ;
	//		if(i!=transients.size() ) i++;
	//}
	
	//cout << "$" << c << " " << counter << " " << transients.at(c).second  << "$" << endl ;

	if(c==transients.size()-1) counter = 0 ;
	else counter++;

	return transients.at(c).second ;
}

void vcd::randomizeCounter(){
	pool->randomizeCounter();
}

void wavePool::randomizeCounter(){
	for(int i=0;i<waves->size();i++){
		waves->at(i)->randomizeCounter();
	}
}

void wave::randomizeCounter(){
	counter = 9999999*rand()%transients.size();
}



string wave::getName(){
	return name;
}

void wave::dump(){
	cout << "dumping signal" << name << endl ;
	for(int i=0;i<transients.size(); i++){
		cout << "@" << transients.at(i).first << " " << transients.at(i).second << endl ;
	}
	cout << endl ;
}

wave::wave(int _id, string _vcd_id, string _name){
	id = _id ;
	vcd_id = _vcd_id ;
	name = _name;
	counter=0;
}

int wave::getId(){
	return id;
}

void wave::addTransient(int time, int value){
	transients.push_back( make_pair(time,value) );
}

void wave::addTransient(int time, char value){
	if(value=='x' || value=='z' ){
		addTransient(time, rand()%2);
	}else if(value=='0'){
		addTransient(time, 0);
	}else{ // if(value=='1'){
		addTransient(time, 1);
	}
}


wavePool::wavePool(){
	currentTime=0;
	waves = new vector<wave*>();
}
wavePool::~wavePool(){
	for(int i=0;i<waves->size();i++) delete waves->at(i);
	delete waves;
}
void wavePool::addwave(wave* w){
	waves->push_back(w);
}

void wavePool::addwave(int id, string vcd_id, string name){
	waves->push_back(new wave(id, vcd_id, name));
}

void wavePool::addtransient(int id, char value){
	int i=0;
	bool not_yet = true;
	while((i<waves->size()) && not_yet ){
		if(waves->at(i)->getId()==id){
			waves->at(i)->addTransient(currentTime, value);
			not_yet=false;
		}
		i++;
	}
	if(not_yet){
		cout << "VCD Error, wave not found!" <<endl ;
	}
}

void wavePool::setTime(int time){
	currentTime=time;
}

// finalle is required to keep track of the time of the signal's ending
void wavePool::finalle(){
	for(int i=0;i<waves->size();i++) 
		waves->at(i)->addTransient(currentTime, 0);
}

void wavePool::dump(){
	for(int i=0;i<waves->size();i++) 
		waves->at(i)->dump();
}


void vcd::setClk(int c){
	clk = c;
}

void vcd::resetCounter(){
}

string vcd::getSignalName(int id){
	try{
		for(int i=0;i<pi_list.size();i++){
			if( pi_list.at(i).second == id ) return pi_list.at(i).first ;
		}
	}catch(...){
		cout << "[Error] Signal not found in VCD file (" << id << "). does the vcd file match the netlist?" << endl; 
		exit(1);
	}
}
