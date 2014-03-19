/*
	ScriptGenerator class can generate scripts necessary for simulation.
	for modelsim, ser and ser analysis.
	the reason I created this class was that I was tired of using bash to write scripts, C++ is much better.
	to run it, use
		make sim
*/
//4ms / 50us = 80 (1 mil inst)

#define MAX_SIM_RUN	100 //250
#define TIME		0.5 //50

#define NAME_CONST	"OR1200-control-"
#define EXTENSION	".rpt"
#define TIMEUNIT	"us"

#define RPT	0
#define VCD	1

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

#include "ScriptGenerator.h"


ScriptGenerator::ScriptGenerator(void){}
ScriptGenerator::ScriptGenerator(string _name, int _t){
	name=_name;
	time=_t;
}
ScriptGenerator::~ScriptGenerator(void){}
string ScriptGenerator::generateFileName(int i, int ext){
	ostringstream ss ;
	if(ext==RPT) ss << NAME_CONST << name << "-" << time*(i+1) << TIMEUNIT << ".rpt" ;
	if(ext==VCD) ss << NAME_CONST << name << "-" << time*(i+1) << TIMEUNIT << ".vcd" ;
	return ss.str();
}

void ScriptGenerator::generateModelSimScript(){
	stringstream ss ;
	ss << "modelsim-script-" << name << ".do" ;
	ofstream out(ss.str().c_str());
	//ofstream out("modelsim-script.do");
	out << "vsim +nowarnTFMPC -L orpsoc orpsoc_testbench -c -quiet +nowarnTFMPC -novopt" << endl ;
	out << "run 500us ;"<< endl ; //***
	out << "vcd file " << generateFileName(0, VCD) << ";" << endl ;
	out << "vcd add -in /orpsoc_testbench/dut/i_or1k/i_or1200_top/or1200_cpu/or1200_ctrl/* ;" << endl ;
	out << "run 50us ;"<< endl ;
	out << "quit -sim ;"<< endl ;
	out << endl << endl << endl ;

	for(int i=1;i<MAX_SIM_RUN;i++){
		out << "vsim +nowarnTFMPC -L orpsoc orpsoc_testbench -c -quiet +nowarnTFMPC -novopt" << endl ;
		out << "run 500us ;"<< endl ; //***
		out << "run " << time*i << TIMEUNIT << " ;" << endl ;
		out << "vcd file " << generateFileName(i, VCD) << ";" << endl ;
		out << "vcd add -in /orpsoc_testbench/dut/i_or1k/i_or1200_top/or1200_cpu/or1200_ctrl/* ;" << endl ;
		out << "run 50us ;"<< endl ;
		out << "quit -sim ;"<< endl ;
		out << endl << endl << endl ;
	}

	out.close();
}

void ScriptGenerator::generateMLETScript(){
	stringstream ss ;
	ss << "mlet-ser-script-" << name << ".sh" ;
	ofstream out(ss.str().c_str());
	//ofstream out("mlet-ser-script.sh");

	out << "#!/bin/sh" << endl << endl ;
	for(int i=0;i<MAX_SIM_RUN;i++){
		out << "./ser.exe -i or1200_ctrl_syn.v -s -c 40 -a maze -v "
		    << generateFileName(i, VCD)
		    << " -o "
		    << generateFileName(i, RPT)
		    << endl << endl ;
	}
	out.close();
}

int main(int argc, char *argv[]){
	cout << "SER Simulation Script Generator Utility" << endl ;
	if(argc<2) cout << "USAGE: Scriptgenerator.exe testname" << endl ;
	string test = string(argv[1]) ;
	cout << "Test name=" << test << endl ;
	ScriptGenerator SG(test,TIME);
	SG.generateModelSimScript();
	SG.generateMLETScript();
	return 0 ;
}
