#pragma once
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>	
#include "configuration.h"
#include "core.h"
#include "defines.h"
#include <vector.h>

#define FS_GATE		0
#define	FS_PHY_0		1
#define	FS_PHY_1		2
#define	FS_DBG		5
#define FS_RND		10

#define FAULT_SITE_TYPE_SEU	0
#define	FAULT_SITE_TYPE_GFI	1
#define	FAULT_SITE_TYPE_GFO	2
#define	FAULT_SITE_TYPE_CFI	3
#define	FAULT_SITE_TYPE_CFO	4
#define FAULT_SITE_TYPE_EXT	5
#define FAULT_SITE_TYPE_RND	5

using namespace boost;
using namespace std;
typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;

struct faultSite{
	int type ;
	int target_count; 
	struct gate* target1;
	struct gate* target2;
	faultSite(int _type, int  _target_count, gate* _target1, gate* _target2){	
		type=_type; 
		target_count=_target_count;
		target1=_target1;
		target2=_target2;
	}
};

class FaultSiteGenerator
{
public:
	Graph g;
	int faultSiteType;
	vector< vector<faultSite> > faultSiteList ;
	int number_of_common_fanout_injection ;
	int number_of_common_fanin_injection ;
	int total_mbu_simulation_per_gate ;
	
	FaultSiteGenerator(Core* core, int faultSiteType);
	FaultSiteGenerator(Core* core, char* input_fault_list);

	~FaultSiteGenerator(void);
	
	int getFaultSiteType();
	vector<faultSite> getFaultSiteList(int ff_index);
	int	getTotalFaultSite(int ff_index);
	
	//gate-level
	void analyzeCircuitGateLevel(Core* core);
	void generateGateLevelFaultSiteList(Core* core);
	int getNumberOfFaultSite(Core* core, int ff_index);
	void generateLayoutLevelFaultSiteListSimulatedAnnealing(Core* core);
	void generateLayoutLevelFaultSiteListMinCut(Core* core);
	void generateRandomFaultSite(Core* core);
	
	void createGraph(Core* core);
	void report_vertex(Graph g);
	void report_edge(Graph g);
	void recursiveMinCutBisection(vector<int> input,int, Core* core, int x1, int x2, int y1, int y2, int f, int**);
	void reportFaultSiteCorrelation(Core* core, vector< vector<faultSite> > faultSiteList1, vector< vector<faultSite> > faultSiteList2);
	//vector<graph_traits<Graph>::vertices_size_type > bfs(Graph g, int source, int N);
	//vector<graph_traits<Graph>::vertices_size_type > bfs(int source);
	
	
};

