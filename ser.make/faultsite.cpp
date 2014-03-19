#include "faultsite.h"

using namespace boost;
using namespace std;

FaultSiteGenerator::FaultSiteGenerator(Core* core, int _faultSiteType){
	faultSiteType = _faultSiteType ;
	switch (faultSiteType) {
		case FS_RND:
			generateRandomFaultSite(core);
			break;
		case FS_GATE:
			analyzeCircuitGateLevel(core);
			generateGateLevelFaultSiteList(core);
			break;
		case FS_PHY_0:
			analyzeCircuitGateLevel(core);
			generateLayoutLevelFaultSiteListSimulatedAnnealing(core);
			break;
		case FS_PHY_1:
			analyzeCircuitGateLevel(core);
			generateLayoutLevelFaultSiteListMinCut(core);
			break;
			
		case FS_DBG:
			break;

		default:
			break;
	}
	}

void FaultSiteGenerator::generateRandomFaultSite(Core* core){
	for(int i=0;i<core->gate_numbers;i++){
		vector<faultSite> fault_site_list_per_gate;
		int target1 = random() % core->gate_numbers ;
		int target2 = random() % core->gate_numbers ;
		if ( target1 == target2 ) target1 = ( target1 + 100 ) % core->gate_numbers ;
		fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_EXT, 2, core->gate_list[target1], core->gate_list[target2]));
	}
}

FaultSiteGenerator::FaultSiteGenerator(Core* core, char* input_fault_list){
	int gateNumber=0 ;
	int target1=0, target2=0;

	FILE *fp = fopen(input_fault_list, "r");
	if(fp==NULL) {
		cout << "input fault-site list file not found" << endl;
		exit(1);
	}else{
		cout <<  "Extracting fault site list ..." << endl ;
		fscanf(fp, "%d", &gateNumber );
		faultSiteList = vector< vector<faultSite> >(gateNumber);
		for(int i=0;i<gateNumber;i++){
			int fsNumber=0, g=0;
			vector<faultSite> fault_site_list_per_gate;
			fscanf(fp, "%d", &g );
			fscanf(fp, "%d", &fsNumber );
			for(int j=0;j<fsNumber;j++){
				fscanf(fp, "%d %d", &target1, &target2);
				int ff_index1=0, ff_index2=0;
				for(int k=0;k<core->gate_numbers; k++){
					if(core->gate_list[k]->gate_no==target1) ff_index1=k;
					if(core->gate_list[k]->gate_no==target2) ff_index2=k;
				}
				fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_EXT, 2, core->gate_list[ff_index1], core->gate_list[ff_index2]));
			}
			faultSiteList[i] = fault_site_list_per_gate;
		}
	}

	fclose(fp);
}



FaultSiteGenerator::~FaultSiteGenerator(void){}


int FaultSiteGenerator::getFaultSiteType(){
	return faultSiteType;
}

vector<faultSite> FaultSiteGenerator::getFaultSiteList(int ff_index){
	return faultSiteList.at(ff_index);
}

int	FaultSiteGenerator::getTotalFaultSite(int ff_index){
	return faultSiteList.at(ff_index).size();
}

//	This is used for extracting neighbor gates at gate-level
//	it will extract it's forward and backward neighbor and stores them in the gate list at core
void FaultSiteGenerator::analyzeCircuitGateLevel(Core* core){
	for(int i=0;i<core->gate_numbers;i++){
		for(int j=0;j<core->fadjlist[i].size();j++){
			core->gate_list[i]->forward_list.push_back(core->fadjlist[i][j]);
		}
		for(int j=0;j<core->gate_numbers;j++){
			for(int k=0;k<core->fadjlist[j].size();k++){
				if( core->fadjlist[j][k]->gate_no==i){
					core->gate_list[i]->backward_list.push_back(core->gate_list[j]);
				}
			}
		}
	}
	
	/*
	 for(int i=0;i<core->gate_numbers;i++){
	 cout << i << " " << core->gate_list[i]->forward_list.size() 
	 << " " << core->gate_list[i]->backward_list.size() << endl ;
	 for(int j=0;j<core->gate_list[i]->forward_list.size();j++){
	 cout << core->gate_list[i]->forward_list[j]->gate_no << " " ;
	 }
	 cout << endl ;
	 for(int j=0;j<core->gate_list[i]->backward_list.size();j++){
	 cout << core->gate_list[i]->backward_list[j]->gate_no << " " ;
	 }
	 cout << endl << "---------------" << endl ;
	 }*/
}


int FaultSiteGenerator::getNumberOfFaultSite(Core* core, int ff_index){
	number_of_common_fanout_injection= core->gate_list[ff_index]->forward_list.size() * (core->gate_list[ff_index]->forward_list.size()-1) /2 ;
	number_of_common_fanin_injection  = core->gate_list[ff_index]->backward_list.size() * (core->gate_list[ff_index]->backward_list.size()-1) /2;	
	total_mbu_simulation_per_gate = 1 + //inject one fault on this gate 
										core->gate_list[ff_index]->forward_list.size() + // inject two fault, one on this gate and one on it's fan-out /
										core->gate_list[ff_index]->backward_list.size() + //inject two fault, one on this gate and one on it's fan-in /
										number_of_common_fanin_injection +
										number_of_common_fanout_injection ;
	//cout << "[X] " << total_mbu_simulation_per_gate << " " << core->gate_list[ff_index]->forward_list.size() << " " << core->gate_list[ff_index]->backward_list.size() << " " << number_of_common_fanin_injection << " " << number_of_common_fanout_injection <<  endl ;
	return total_mbu_simulation_per_gate ;
}

//computing total number of fault site pair for target gate using only gate-level netlist
//generated the stored pair of fault site in a vector
void FaultSiteGenerator::generateGateLevelFaultSiteList(Core* core){
	for(int ff_index=0;ff_index<core->gate_numbers;ff_index++){
		vector<faultSite> fault_site_list_per_gate;
		int totalFaultSite = getNumberOfFaultSite(core, ff_index);
		struct gate *gateptr = core->gate_list[ff_index];
		for(int mbu_index=0;mbu_index<totalFaultSite;mbu_index++){
			if( mbu_index==0 ){//this gate
				//TODO: seu: to be or not to be!
				fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_SEU, 1, gateptr, NULL));
			}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() ){//forward
				//target + fan-in
				fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_GFI, 2, gateptr, core->gate_list[ff_index]->forward_list[mbu_index-1]));
			}else if(mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size()){//backward
				//target+fan-out
				fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_GFO, 2, gateptr, core->gate_list[ff_index]->backward_list[mbu_index-core->gate_list[ff_index]->forward_list.size()-1]));
			}else if( mbu_index <= core->gate_list[ff_index]->forward_list.size() + core->gate_list[ff_index]->backward_list.size() + number_of_common_fanin_injection ){
				//common-fan-in					
				for(int i=0;i<core->gate_list[ff_index]->forward_list.size();i++){
					for(int j=i+1; j<core->gate_list[ff_index]->forward_list.size();j++){
						fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFI, 2, core->gate_list[ff_index]->forward_list.at(i), core->gate_list[ff_index]->forward_list.at(j)));
					}
				}
			}else{
				//common-fanout
				for(int i=0;i<core->gate_list[ff_index]->backward_list.size();i++){
					for(int j=i+1; j<core->gate_list[ff_index]->backward_list.size();j++){
						fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index]->backward_list.at(i), core->gate_list[ff_index]->backward_list.at(j)));
					}
				}
			}
		}	
		faultSiteList.push_back(fault_site_list_per_gate);
	}
}

 
 
void FaultSiteGenerator::createGraph(Core* core){
	g = Graph(core->gate_numbers);
	for(int i=0;i<core->gate_numbers;i++){
		for(int j=0;j<core->fadjlist[i].size();j++){
			gate* v = core->fadjlist[i][j];
			// int out = v->out ;
			//cout << i << " - " << v->gate_no << endl ;
			add_edge(i, v->gate_no , g);
			//TO: add edge for each two vertex in the same adj. list.
			//gate connected to same wire are neighbor as well.
		}
	}
	//report_vertex(g);
	//report_edge(g);
}
 
void FaultSiteGenerator::report_vertex(Graph g){
	typedef property_map<Graph, vertex_index_t>::type IndexMap;
	IndexMap index = get(vertex_index, g);
 
	cout << "vertices(g) = ";
	typedef graph_traits<Graph>::vertex_iterator vertex_iter;
	pair<vertex_iter, vertex_iter> vp;
	for (vp = vertices(g); vp.first != vp.second; ++vp.first)
		cout << index[*vp.first] <<  " ";
	cout << endl;
}
 
void FaultSiteGenerator::report_edge(Graph g){
	typedef property_map<Graph, vertex_index_t>::type IndexMap;
	IndexMap index = get(vertex_index, g);
 
	cout << "edges(g) = ";
	graph_traits<Graph>::edge_iterator ei, ei_end;
	for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
		cout << "(" << index[source(*ei, g)] << "," << index[target(*ei, g)] << ") ";
	cout << endl;
}
 


#define INIT_TEMP	4000000
#define FINAL_TEMP	0.1
void FaultSiteGenerator::generateLayoutLevelFaultSiteListSimulatedAnnealing(Core* core){
	float t = INIT_TEMP * core->gate_numbers;
	if(t<0) t=INIT_TEMP;
	vector< pair<int,int> > placement_info ;
	//defining a floorplan, it's a square.
	int size = floor ( sqrt( core->gate_numbers ) ) ;
	if (size*size != core->gate_numbers) size++ ;
	int size2 = size*size;
	int** placement = new int*[size];
	for(int i=0;i<size;i++) placement[i] = new int[size];
	
	for(int i=0;i<size;i++)
		for(int j=0;j<size;j++)
			placement[i][j]=-1;
	
	cout << "Defining floorplan: " << size << "x" << size << endl ;

	//---------------------------------------
	//random placement at begining
	//---------------------------------------
	for(int i=0;i<core->gate_numbers;i++){
		int x = floor( i/size ) ; 
		int y = i%size; 
		placement[x][y] = core->gate_list[i]->gate_no ;
		placement_info.push_back ( make_pair(-1,-1) ) ;
	}
	for(int i=0;i< 4*core->gate_numbers; i++){
		int x1 = random()%size;	int y1 = random()%size;
		int x2 = random()%size; int y2 = random()%size;
		int s = placement[x1][y1] ;
		placement[x1][y1] = placement[x2][y2] ;
		placement[x2][y2] = s ;
	}
	
	for(int i=0;i<size;i++){
		for(int j=0;j<size;j++){
			if(placement[i][j]!=-1) placement_info[ placement[i][j] ] = make_pair(i,j);
		}
	}
	
	
	int cost=0;
	for(int i=0;i<core->gate_numbers;i++){
		for(int j=0;j<core->gate_list[i]->forward_list.size();j++){
			cost += abs( placement_info[i].first - placement_info[core->gate_list[i]->forward_list.at(j)->gate_no].first);
			cost += abs( placement_info[i].second - placement_info[core->gate_list[i]->forward_list.at(j)->gate_no].second);
		}
		for(int j=0;j<core->gate_list[i]->backward_list.size();j++){
			cost += abs( placement_info[i].first - placement_info[core->gate_list[i]->backward_list.at(j)->gate_no].first);
			cost += abs( placement_info[i].second - placement_info[core->gate_list[i]->backward_list.at(j)->gate_no].second);
		}
	}
	cout << "initial wire cost=" << cost << endl ;
	//---------------------------------------
	// simulated annealing
	//---------------------------------------
	int c=0;
	while(t>FINAL_TEMP){
		int x1 = random()%size;	int y1 = random()%size;
		int x2 = random()%size; int y2 = random()%size;
		if( (placement[x1][y1] == -1) || (placement[x2][y2] == -1) ) continue; 
		
		//computing wire cost ...
		int gate1=placement[x1][y1], gate2=placement[x2][y2];
		int cost_old=0, cost_new=0;
		for(int i=0;i<core->gate_list[gate1]->forward_list.size();i++){
			cost_old += abs( x1 - placement_info[core->gate_list[gate1]->forward_list.at(i)->gate_no].first);
			cost_old += abs( y1 - placement_info[core->gate_list[gate1]->forward_list.at(i)->gate_no].second);
		}
		for(int i=0;i<core->gate_list[gate1]->backward_list.size();i++){
			cost_old += abs( x1 - placement_info[core->gate_list[gate1]->backward_list.at(i)->gate_no].first);
			cost_old += abs( y1 - placement_info[core->gate_list[gate1]->backward_list.at(i)->gate_no].second);
		}
		for(int i=0;i<core->gate_list[gate2]->forward_list.size();i++){
			cost_old += abs( x2 - placement_info[core->gate_list[gate2]->forward_list.at(i)->gate_no].first);
			cost_old += abs( y2 - placement_info[core->gate_list[gate2]->forward_list.at(i)->gate_no].second);
		}
		for(int i=0;i<core->gate_list[gate2]->backward_list.size();i++){
			cost_old += abs( x2 - placement_info[core->gate_list[gate2]->backward_list.at(i)->gate_no].first);
			cost_old += abs( y2 - placement_info[core->gate_list[gate2]->backward_list.at(i)->gate_no].second);
		}
		
		for(int i=0;i<core->gate_list[gate1]->forward_list.size();i++){
			cost_new += abs( x2 - placement_info[core->gate_list[gate1]->forward_list.at(i)->gate_no].first);
			cost_new += abs( y2 - placement_info[core->gate_list[gate1]->forward_list.at(i)->gate_no].second);
		}
		for(int i=0;i<core->gate_list[gate1]->backward_list.size();i++){
			cost_new += abs( x2 - placement_info[core->gate_list[gate1]->backward_list.at(i)->gate_no].first);
			cost_new += abs( y2 - placement_info[core->gate_list[gate1]->backward_list.at(i)->gate_no].second);
		}
		for(int i=0;i<core->gate_list[gate2]->forward_list.size();i++){
			cost_new += abs( x1 - placement_info[core->gate_list[gate2]->forward_list.at(i)->gate_no].first);
			cost_new += abs( y1 - placement_info[core->gate_list[gate2]->forward_list.at(i)->gate_no].second);
		}
		for(int i=0;i<core->gate_list[gate2]->backward_list.size();i++){
			cost_new += abs( x1 - placement_info[core->gate_list[gate2]->backward_list.at(i)->gate_no].first);
			cost_new += abs( y1 - placement_info[core->gate_list[gate2]->backward_list.at(i)->gate_no].second);
		}
	
		float a_random_number = ( random() % 1000 ) / 1000.0 ; 
		if( cost_new < cost_old ){
			int s = placement[x1][y1] ;
			placement[x1][y1] = placement[x2][y2] ;
			placement[x2][y2] = s ;
			placement_info[placement[x1][y1]] = make_pair(x1,y1);
			placement_info[placement[x2][y2]] = make_pair(x2,y2);
			//cout << " Accepting movement -> swapping " << x1 << " " << y1 << " -> " << x2 << " " << y2 <<  "     [" << cost_new << " " << cost_old << endl ;
		}else if( 1.5*a_random_number > exp( (cost_new - cost_old)/ (1.0*t) ) ){
			int s = placement[x1][y1] ;
			placement[x1][y1] = placement[x2][y2] ;
			placement[x2][y2] = s ;
			placement_info[placement[x1][y1]] = make_pair(x1,y1);
			placement_info[placement[x2][y2]] = make_pair(x2,y2);
			//cout << " Accepting movement because of temp."<< endl ;
		}else {
			int d = (cost_new - cost_old) ;
			float z = (cost_new - cost_old)/ (1.0*t) ;
			float x = exp( (cost_new - cost_old)/ (1.0*t) );
			//cout << "do not accepting movement   " << a_random_number <<  " " << x << '\t' << d << " " << z << endl ;
		}
		
		t = t * 0.9999;
		c++;
	}
	
	cout << "Total iteration= " << c << endl ;
	cost=0;
	for(int i=0;i<core->gate_numbers;i++){
		for(int j=0;j<core->gate_list[i]->forward_list.size();j++){
			cost += abs( placement_info[i].first - placement_info[core->gate_list[i]->forward_list.at(j)->gate_no].first);
			cost += abs( placement_info[i].second - placement_info[core->gate_list[i]->forward_list.at(j)->gate_no].second);
		}
		for(int j=0;j<core->gate_list[i]->backward_list.size();j++){
			cost += abs( placement_info[i].first - placement_info[core->gate_list[i]->backward_list.at(j)->gate_no].first);
			cost += abs( placement_info[i].second - placement_info[core->gate_list[i]->backward_list.at(j)->gate_no].second);
		}
	}
	cout << "Final wire cost=" << cost << endl ;
	
	//---------------------------------------
	//extract fault sites
	//---------------------------------------
	
	for(int ff_index=0;ff_index<core->gate_numbers;ff_index++){
		vector<faultSite> fault_site_list_per_gate;
		//  1 2 3
		//  4 * 5
		//  6 7 8
		int x = placement_info[ core->gate_list[ff_index]->gate_no ].first ;
		int y = placement_info[ core->gate_list[ff_index]->gate_no ].second ;
		cout << x << " " << y << endl ;
		if(x!=0){
			if(y!=0){
				if(placement[x-1][y-1]!=-1){ //1
					fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index], core->gate_list[placement[x-1][y-1]]));
				}
			}
			if(y!=size-1){
				if(placement[x-1][y+1]!=-1){ //3
					fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index], core->gate_list[placement[x-1][y+1]]));
				}
			}
			if(placement[x-1][y]!=-1){ //2
				fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index], core->gate_list[placement[x-1][y]]));
			}
		}
		if(x!=size-1){
			if(y!=0){
				if(placement[x+1][y-1]!=-1){ //6
					fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index], core->gate_list[placement[x+1][y-1]]));
				}
			}
			if(y!=size-1){
				if(placement[x+1][y+1]!=-1){ //8
					fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index], core->gate_list[placement[x+1][y+1]]));
				}
			}
			if(placement[x+1][y]!=-1){ //7
				fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index], core->gate_list[placement[x+1][y]]));
			}
		}
		
		if(y!=0){
			if(placement[x][y-1]!=-1){ //4
				fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index], core->gate_list[placement[x][y-1]]));
			}
		}
		if(y!=size-1){
			if(placement[x][y+1]!=-1){ //5
				fault_site_list_per_gate.push_back(faultSite(FAULT_SITE_TYPE_CFO, 2, core->gate_list[ff_index], core->gate_list[placement[x][y+1]]));
			}
		}
		
		faultSiteList.push_back(fault_site_list_per_gate);
	}
}


void FaultSiteGenerator::recursiveMinCutBisection(vector<int> input, int step, Core* core, int x1, int x2, int y1, int y2, int f, int** placement){
	cout << "doing mincut in " << x1 << "," << y1 << " " << x2 << "," << y2 << endl ;
	if(input.size()<3){
		//TODO: assign space
		cout << "do assignment on " << step << " |";
		for(int i=0;i<input.size();i++){
			cout << input[i] << " " ;
		}
		cout << endl ;
		
		if( input.size()==1 ){
			placement[x1][y1] = input[0];
		}
		if( input.size()==2 ){
			placement[x1][y1] = input[0] ;
			placement[x2][y2] = input[1] ;
		}
		return ;
	}else{
		//Bisection algorithm
		//int s = (step%2==1)? step-1: step ;
		vector<int> section1 ;
		vector<int> section2 ;
		
		
		vector<int> node_locked ;
		vector<int> node_weight ;
		for(int i=0;i<input.size();i++) node_locked.push_back(0);
		for(int i=0;i<input.size();i++) node_weight.push_back(0);
		
		for(int i=0;i<input.size()/2;i++){
			section1.push_back(input[i]);
		}
		for(int i=input.size()/2; i<input.size();i++){
			section2.push_back(input[i]);
		}
		
		int t=0;
		//calculating weights
		for(int i=0;i<section1.size();i++, t++){
			int w_stay=0, w_move=0;
			for(int j=0;j<section1.size();j++){
				if(i!=j){
					// if node i and j are connected, w_stay++ ;
					int is_node_i_j_connected = false ;
					for(int k=0;k<core->gate_list[ section1[i] ]->forward_list.size();k++){
						if( core->gate_list[ section1[i] ]->forward_list.at(k)->gate_no == section1[j] ) is_node_i_j_connected = true ;
					}
					for(int k=0;k<core->gate_list[ section1[i] ]->backward_list.size();k++){
						if( core->gate_list[ section1[i] ]->backward_list.at(k)->gate_no == section1[j] ) is_node_i_j_connected = true ;
					}
					if(is_node_i_j_connected) w_stay++ ;
				}
			}
			for(int j=0;j<section2.size();j++){
				// if node i and j are connected, w_move++ ;
				int is_node_i_j_connected = false ;
				for(int k=0;k<core->gate_list[ section1[i] ]->forward_list.size();k++){
					if( core->gate_list[ section1[i] ]->forward_list.at(k)->gate_no == section2[j] ) is_node_i_j_connected = true ;
				}
				for(int k=0;k<core->gate_list[ section1[i] ]->backward_list.size();k++){
					if( core->gate_list[ section1[i] ]->backward_list.at(k)->gate_no == section2[j] ) is_node_i_j_connected = true ;
				}
				if(is_node_i_j_connected) w_move++ ;
			}
			node_weight[t] = w_move-w_stay;
		}

		for(int i=0;i<section2.size();i++,t++){
			int w_stay=0, w_move=0;
			for(int j=0;j<section1.size();j++){
				if(i!=j){
					// if node i and j are connected, w_stay++ ;
					int is_node_i_j_connected = false ;
					for(int k=0;k<core->gate_list[ section2[i] ]->forward_list.size();k++){
						if( core->gate_list[ section2[i] ]->forward_list.at(k)->gate_no == section1[j] ) is_node_i_j_connected = true ;
					}
					for(int k=0;k<core->gate_list[ section2[i] ]->backward_list.size();k++){
						if( core->gate_list[ section2[i] ]->backward_list.at(k)->gate_no == section1[j] ) is_node_i_j_connected = true ;
					}
					if(is_node_i_j_connected) w_stay++ ;
				}
			}
			for(int j=0;j<section2.size();j++){
				// if node i and j are connected, w_move++ ;
				int is_node_i_j_connected = false ;
				for(int k=0;k<core->gate_list[ section2[i] ]->forward_list.size();k++){
					if( core->gate_list[ section2[i] ]->forward_list.at(k)->gate_no == section2[j] ) is_node_i_j_connected = true ;
				}
				for(int k=0;k<core->gate_list[ section2[i] ]->backward_list.size();k++){
					if( core->gate_list[ section2[i] ]->backward_list.at(k)->gate_no == section2[j] ) is_node_i_j_connected = true ;
				}
				if(is_node_i_j_connected) w_move++ ;
			}
			node_weight[t] = w_move-w_stay;
		}
		
		
		cout << "weights=" ;
		for(int i=0;i<node_weight.size();i++) cout << node_weight[i] << " " ;
		cout << endl ;
		
		
		int move_candidate1=-1, move_candidate2=-1;
		do{
			move_candidate1=-1, move_candidate2=-1;
			int max; 
			t=0;
			for(int i=0, max=-9999;i<section1.size();i++,t++){
				if( (node_weight[t]>0) && (node_weight[i]>max) && (node_locked[t]==false) ){
					move_candidate1 = i ;
					max = node_weight[t] ;
				}
			}
			
			for(int i=0,max=-9999;i<section2.size();i++,t++){
				if( (node_weight[t]>0) && (node_weight[t]>max) && (node_locked[t]==false) ){
					move_candidate2 = i ;
					max = node_weight[t] ;
				}
			}
			
			if(( move_candidate1!=-1) && (move_candidate2!=-1)){
				cout << "Found  a match! " << move_candidate1 << " " << move_candidate2 << endl ;
				//updating weights:
				for(int i=0;i<core->gate_list[ section1[move_candidate1] ]->forward_list.size();i++){
					for(int j=0;j<section1.size();j++){
						if(section1[j]==core->gate_list[ section1[move_candidate1] ]->forward_list[i]->gate_no){
							node_weight[j]+=2; 
						}
					}
					for(int j=0;j<section2.size();j++){
						if(section2[j]==core->gate_list[ section1[move_candidate1] ]->forward_list[i]->gate_no){
							node_weight[input.size()/2+j]-=2; 
						}
					}
				}
				for(int i=0;i<core->gate_list[ section1[move_candidate1] ]->backward_list.size();i++){
					for(int j=0;j<section1.size();j++){
						if(section1[j]==core->gate_list[ section1[move_candidate1] ]->backward_list[i]->gate_no){
							node_weight[j]+=2; 
						}
					}
					for(int j=0;j<section2.size();j++){
						if(section2[j]==core->gate_list[ section1[move_candidate1] ]->backward_list[i]->gate_no){
							node_weight[input.size()/2+j]-=2; 
						}
					}
				}			
				
				for(int i=0;i<core->gate_list[ section2[ move_candidate2 ] ]->forward_list.size();i++){
					for(int j=0;j<section1.size();j++){
						if(section1[j]==core->gate_list[ section2[move_candidate2] ]->forward_list[i]->gate_no){
							node_weight[j]-=2; 
						}
					}
					for(int j=0;j<section2.size();j++){
						if(section2[j]==core->gate_list[ section2[move_candidate2] ]->forward_list[i]->gate_no){
							node_weight[input.size()/2+j]+=2; 
						}
					}
				}
				for(int i=0;i<core->gate_list[ section2[move_candidate2] ]->backward_list.size();i++){
					for(int j=0;j<section1.size();j++){
						if(section1[j]==core->gate_list[ section2[move_candidate2] ]->backward_list[i]->gate_no){
							node_weight[j]-=2; 
						}
					}
					for(int j=0;j<section2.size();j++){
						if(section2[j]==core->gate_list[ section2[move_candidate2] ]->backward_list[i]->gate_no){
							node_weight[input.size()/2+j]+=2; 
						}
					}
				}	
				
				
				//swapping!
				int x = section1[move_candidate1] ;
				section1[move_candidate1] = section2[move_candidate2];
				section2[move_candidate2] = x; 
				node_locked[move_candidate1]=true ;
				node_locked[input.size()/2 + move_candidate2]=true;
			}
			
			cout << "lockes=" ;
			for(int i=0;i<node_weight.size();i++) cout << node_locked[i] << " " ;
			cout << endl ;
			
			
		}while(( move_candidate1!=-1) && (move_candidate2!=-1));
		
		
		int x3, y3, x4, y4 ;
		//recursive call
		if(f==1){
			x3 = ceil( (x1+x2)/2 )+1 ;	y3 = y1 ;
			x4 = ceil( (x1+x2)/2 ) ;	y4 = y2 ;
		}else{
			x3 = x1 ;	y3 = ceil( (y1+y2)/2 )+1 ;
			x4 = x2 ;	y4 = ceil( (y1+y2)/2 ) ;
		}
		cout << "doing recursive: box is " << x1 << "," << y1 << " " << x4 << "," << y4 << endl ;
		cout << "d             e: box is " << x3 << "," << y3 << " " << x2 << "," << y2 << endl ;
		
		recursiveMinCutBisection(section1, 2*step, core, x1, x4, y1, y4, f*-1, placement );
		recursiveMinCutBisection(section2, 2*step+1, core, x3, x2, y3, y2, f*-1, placement );
	}
}

void FaultSiteGenerator::generateLayoutLevelFaultSiteListMinCut(Core* core){
	vector< pair<int,int> > placement_info ;
	//defining a floorplan, it's a square.
	int size = floor ( sqrt( core->gate_numbers ) ) ;
	if (size*size != core->gate_numbers) size++ ;
	int size2 = size*size;
	int** placement = new int*[size];
	for(int i=0;i<size;i++) placement[i] = new int[size];
	
	for(int i=0;i<size;i++)
		for(int j=0;j<size;j++)
			placement[i][j]=-1;
	
	cout << "Defining floorplan: " << size << "x" << size << endl ;

	
	vector<int> gate_list ;
	for(int i=0;i<core->gate_numbers;i++){
		gate_list.push_back(i);
	}
	recursiveMinCutBisection(gate_list,1, core, 0, size-1, 0,  size-1, 1, placement);
	
	for(int i=0;i<core->gate_numbers;i++){
		placement_info.push_back ( make_pair(-1,-1) ) ;
	}
	for(int i=0;i<size;i++){
		for(int j=0;j<size;j++){
			if(placement[i][j]!=-1){
				placement_info[ placement[i][j] ] = make_pair(i,j);
			}
			cout << placement[i][j] << "\t" ;
		}
		cout << endl ;
	}
	
	int cost=0;
	for(int i=0;i<core->gate_numbers;i++){
		for(int j=0;j<core->gate_list[i]->forward_list.size();j++){
			cost += abs( placement_info[i].first - placement_info[core->gate_list[i]->forward_list.at(j)->gate_no].first);
			cost += abs( placement_info[i].second - placement_info[core->gate_list[i]->forward_list.at(j)->gate_no].second);
		}
		for(int j=0;j<core->gate_list[i]->backward_list.size();j++){
			cost += abs( placement_info[i].first - placement_info[core->gate_list[i]->backward_list.at(j)->gate_no].first);
			cost += abs( placement_info[i].second - placement_info[core->gate_list[i]->backward_list.at(j)->gate_no].second);
		}
	}
	cout << "wire cost=" << cost << endl ;
}


void FaultSiteGenerator::reportFaultSiteCorrelation(Core* core, vector< vector<faultSite> > faultSiteList1, vector< vector<faultSite> > faultSiteList2){
	int c=0;
	int d=0;
	cout << "1" << endl ;
	for(int i=0;i<faultSiteList1.size();i++){
		cout << "2" << endl ;
		vector<faultSite> fs1 = faultSiteList1[i];
		for(int j=0;j<fs1.size();j++){
			bool exists=false;
			int gate1 = fs1[j].target1->gate_no ;
			int gate2 = fs1[j].target2->gate_no ;
				cout << "3" << endl ;
			for(int m=0;m<faultSiteList2.size();m++){
				vector<faultSite> fs2 = faultSiteList2[m];
				for(int n=0;n<fs2.size();n++){
					int gate3 = fs2[n].target1->gate_no ;
					int gate4 = fs2[n].target2->gate_no ;
					
					if( ((gate1==gate3)&&(gate2==gate4)) || ((gate1==gate4)&&(gate2==gate3)) ){
						exists=true ;
					}
				}
			}
			
			if(exists)c++;
			else d++;
				cout << "4" << endl ;
		}
	}
	float cor = (float)c/(float)(c+d);

	cout << "Correlation=" << cor << endl;
}
/*

template < typename TimeMap > class bfs_time_visitor:public default_bfs_visitor {
	typedef typename property_traits < TimeMap >::value_type T;
public:
	bfs_time_visitor(TimeMap tmap, T & t):m_timemap(tmap), m_time(t) { }
	template < typename Vertex, typename Graph > void discover_vertex(Vertex u, const Graph & g) const {
		put(m_timemap, u, m_time++);
	}
	TimeMap m_timemap;
	T & m_time;
};

vector<graph_traits<Graph>::vertices_size_type > FaultSiteGenerator::bfs(Graph g, int source, int N){
	typedef graph_traits < Graph >::vertex_descriptor Vertex;
	typedef graph_traits < Graph >::vertices_size_type Size;
	typedef Size* Iiter;
 
	// a vector to hold the discover time property for each vertex
	std::vector < Size > dtime(num_vertices(g));
	vector<int> results ;
 
	Size time = 0;
	bfs_time_visitor <Size*> vis(&dtime[0], time);
	breadth_first_search(g, vertex(source, g), visitor(vis));
 
	// Use std::sort to order the vertices by their discover time
	vector<graph_traits<Graph>::vertices_size_type > discover_order(N);
	integer_range < int >range(0, N);
	copy(range.begin(), range.end(), discover_order.begin());
	sort(discover_order.begin(), discover_order.end(),indirect_cmp < Iiter, std::less < Size > >(&dtime[0]));
	return discover_order ;
}
 
vector<graph_traits<Graph>::vertices_size_type > FaultSiteGenerator::bfs(int source){
	return bfs(g, source, gate_numbers);
}
 
//next_permutation() 
vector<int> FaultSiteGenerator::findFaultSite(int target, int n, int k){
	vector<graph_traits<Graph>::vertices_size_type > discover_order = bfs(target);
	vector<int> n_closest_neighbors, k_selected_neighbors ;
	for(int i=0;i<discover_order.size();i++){
		if(discover_order[i]<=n){//it means the target gate is included. discover_order[i]!=0
			n_closest_neighbors.push_back(i);
		}
	}
	for(int i=0;i<n_closest_neighbors.size();i++){
		cout << n_closest_neighbors[i] << " " ;
	}
	cout << endl ;
	random_shuffle(n_closest_neighbors.begin(), n_closest_neighbors.end());
	for(int i=0;i<k;i++){
		k_selected_neighbors.push_back(n_closest_neighbors[i]);
		cout << n_closest_neighbors[i] << " " ;
	}
	cout << endl ;
	return k_selected_neighbors ;
}
*/
