#include "faultgen.h"


FaultGenerator::FaultGenerator(Core* core){
}

FaultGenerator::FaultGenerator(){
}

FaultGenerator::~FaultGenerator(){
}

int FaultGenerator::generateFault(Core* core, configuration* config, gate* g){
	return  config->seu_pulse_width ;
}