#pragma once
class ResultAnalysis
{
	string name ;
	int	   t ;
public:
	ResultAnalysis(void);
	ResultAnalysis(string, int);
	~ResultAnalysis(void);

	void extract_sim_epp_at_specific_clk(string in,int clk);
	void extract_sys_epp_at_specific_clk(string in,int clk);
	int extract_saturation_clock(string, int);
	void extract_epp_at_saturation_clock(string);
	void extract_sys_epp(string);
	void extract_sim_epp(string);
	void extractGateDFFContributionInSER(string, int);

	string generateFileName(int i);
};

