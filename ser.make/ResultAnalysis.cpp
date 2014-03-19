/*
	ScriptGenerator class can generate scripts necessary for simulation.
	for modelsim, ser and ser analysis.
	the reason I created this class was that I was tired of using bash to write scripts, C++ is much better.
	to run it, use
		make analysis
*/
	// Run SER with -w switch to extract these results.
#define MAX_SIM_RUN	5
#define PI			41
#define PO			239
#define GATE		888
#define DFF			152
#define TOTAL		1040

#define NAME_CONST	"OR1200-control-"
#define EXTENSION	".rpt"
#define TIMEUNIT	"us"


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;


#include "ResultAnalysis.h"


ResultAnalysis::ResultAnalysis(void){}
ResultAnalysis::ResultAnalysis(string _name, int _t){
	name=_name;
	t=_t;
}
ResultAnalysis::~ResultAnalysis(void){}


string ResultAnalysis::generateFileName(int i){
	ostringstream ss ;
	ss << NAME_CONST << name << "-" << t*(i+1) << TIMEUNIT << EXTENSION ;
	return ss.str();
}

void ResultAnalysis::extract_sys_epp(string fileName){
	string line;
	ifstream in(fileName.c_str());
	in.clear();
	in.seekg(0, ios::beg);
	while (! in.eof() ){
		getline (in,line);
		if(line.find("average sys epp: ")!=string::npos){
			cout << line.substr(17, line.length()) << endl ;
		}
    }
	in.close();
}

void ResultAnalysis::extract_sim_epp(string fileName){
	string line;
	ifstream in(fileName.c_str());
	in.clear();
	in.seekg(0, ios::beg);
	while (! in.eof() ){
		getline (in,line);
		if(line.find("average sim epp: ")!=string::npos){
			cout << line.substr(17, line.length()) << endl ;
		}
    }
	in.close();
}


void ResultAnalysis::extract_sys_epp_at_specific_clk(string fileName, int clk){
	string line;
	ifstream in(fileName.c_str());
	in.clear();
	in.seekg(0, ios::beg);
	int current_clock;
	while (! in.eof() ){
		getline (in,line);
		if(line.find(" CLK=")!=string::npos){
			line = line.substr(0,line.length()-1); // This fix EOL bug in linux, it's a C++ bug
			current_clock=boost::lexical_cast<int>(line.substr(6, line.length()));
		}
		if(line.find("average sys epp: ")!=string::npos){
			if( current_clock == clk )
				cout << line.substr(17, line.length()) << endl ;
		}
    }
	in.close();
}



void ResultAnalysis::extract_sim_epp_at_specific_clk(string fileName, int clk){
	string line;
	ifstream in(fileName.c_str());
	in.clear();
	in.seekg(0, ios::beg);
	int current_clock;
	while (! in.eof() ){
		getline (in,line);
		if(line.find(" CLK=")!=string::npos){
			line = line.substr(0,line.length()-1); // This fix EOL bug in linux, it's a C++ bug
			current_clock=boost::lexical_cast<int>(line.substr(6, line.length()));
		}
		if(line.find("average sim epp: ")!=string::npos){
			if( current_clock == clk )
				cout << line.substr(17, line.length()) << endl ;
		}
    }
	in.close();
}

int ResultAnalysis::extract_saturation_clock(string fileName, int v){
	string line;
	ifstream in(fileName.c_str());
	in.clear();
	in.seekg(0, ios::beg);
	double ser2 = 0 ;
	int current_clock=0;
	while (! in.eof() ){
		getline (in,line);
		if(line.find(" CLK=")!=string::npos){
			line = line.substr(0,line.length()-1); // This fix EOL bug in linux, it's a C++ bug
			current_clock=boost::lexical_cast<int>(line.substr(6, line.length()));
		}
		if(line.find("average sys epp: ")!=string::npos){
			double ser=boost::lexical_cast<double>(line.substr(17, 5));
			if(ser-ser2 < 0.01){
				if(v==1) cout << current_clock <<endl ;
				return current_clock;
			}
			ser2=ser;
			//cout << ser << endl ;
		}
    }
	in.close();
	return current_clock;
}

void  ResultAnalysis::extract_epp_at_saturation_clock(string fileName){
	extract_sys_epp_at_specific_clk(fileName, extract_saturation_clock(fileName, 0));
}

void ResultAnalysis::extractGateDFFContributionInSER(string fileName, int clk){
	if(clk==1){ cout << "ERROR, Clock can't be 1"<< endl; return; }
	string line;
	ifstream in(fileName.c_str());
	in.clear();
	in.seekg(0, ios::beg);
	int current_clock;
	while (! in.eof() ){
		getline (in,line);
		// first find the line  CLK= (i-1)
		if(line.find(" CLK=")!=string::npos){
			line = line.substr(0,line.length()-1); // This fix EOL bug in linux, it's a C++ bug
			current_clock=boost::lexical_cast<int>(line.substr(6, line.length()));
		}
		if( current_clock == clk-1){
			// go ahead till line node num :: clk= 1 :: clk= 2 ...
			do{
				getline (in,line);
			}while(line.find("node num :: ")==string::npos);

			//now, extract information!
			double DFF_ser_total  = 0 ;
			double gate_ser_total = 0 ;
			for(int i=0;i<TOTAL;i++){
				double* ser = new double[clk];
				int j=0;
				//extract parameters
				getline (in,line);
				line = line.substr(0,line.length()-1); // This fix EOL bug in linux, it's a C++ bug
				typedef boost::tokenizer<boost::char_separator<char> >	tokenizer;
				boost::char_separator<char> sep("::");
				boost::tokenizer<boost::char_separator<char> > tokens(line, sep);
				for (tokenizer::iterator iter = tokens.begin(); iter != tokens.end(); ++iter){
					if(iter!=tokens.begin()){
						string s = string(*iter);
						boost::trim(s);
						ser[j++] = boost::lexical_cast<double>(s);
					}
				}
				if( i< DFF ){ // it's a dff
					DFF_ser_total += ser[clk-1];
				}else{// it's a gate
					gate_ser_total += ser[clk-1];
				}
			}
			cout << "DFF = " << DFF_ser_total/DFF << " , " <<  DFF_ser_total/TOTAL << endl ;
			cout << "gate= " << gate_ser_total/GATE << " , " <<  gate_ser_total/TOTAL << endl ;
			in.close();
			return ;
		}
    }
	in.close();
	return ;
}

int main(int argc, char *argv[]){
	cout << "SER Result Analyzer Utility" << endl ;
	if(argc<2) cout << "USAGE: Resultanalysis.exe testname" << endl ;
	string test = string(argv[1]) ;
	cout << "Test name=" << test << endl ;
	ResultAnalysis R(test, 50);
	string fileName = R.generateFileName(3);
	cout << "SIM_EPP" << endl ;
	R.extract_sim_epp(fileName);
	cout << "SYS_EPP" << endl ;
	R.extract_sys_epp(fileName);
	cout << "SIM_EPP#5" << endl ;
	R.extract_sim_epp_at_specific_clk(fileName,5);
	cout << "SYS_EPP#5" << endl ;
	R.extract_sys_epp_at_specific_clk(fileName,5);
	cout << "SAT_CLK" << endl ;
	R.extract_saturation_clock(fileName, 1);
	cout << "EPP@SAT_CLK" << endl ;
	R.extract_epp_at_saturation_clock(fileName);
	cout << R.generateFileName(5) << endl ;


	cout << "#################################################" << endl ;
	cout << "# Detailed MLET SER for an application " << test << endl ;
	cout << "#################################################" << endl ;
	for(int i=0;i<MAX_SIM_RUN;i++){
		R.extract_sys_epp_at_specific_clk(R.generateFileName(i),10);
	}
	cout << "#################################################" << endl ;


	cout << "#################################################" << endl ;
	cout << "# Detailed M.C. SER for an application " << test << endl ;
	cout << "#################################################" << endl ;
	for(int i=0;i<MAX_SIM_RUN;i++){
		R.extract_sim_epp_at_specific_clk(R.generateFileName(i),10);
	}
	cout << "#################################################" << endl ;

	cout << "#################################################" << endl ;
	cout << "# Saturation Clock for test =  " << test << endl ;
	cout << "#################################################" << endl ;
	for(int i=0;i<MAX_SIM_RUN;i++){
		R.extract_saturation_clock(R.generateFileName(i), 1);
	}
	cout << "#################################################" << endl ;


	cout << "#################################################" << endl ;
	cout << "# EPP at Saturation Clock for test =  " << test << endl ;
	cout << "#################################################" << endl ;
	for(int i=0;i<MAX_SIM_RUN;i++){
		R.extract_epp_at_saturation_clock(R.generateFileName(i));
	}
	cout << "#################################################" << endl ;

	R.extractGateDFFContributionInSER(R.generateFileName(4), 10);
	return 0 ;

}
