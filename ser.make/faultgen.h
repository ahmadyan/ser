#pragma once
#include "configuration.h"
#include "core.h"
#include "defines.h"
class FaultGenerator
{
	
public:
	FaultGenerator(Core* core);
	FaultGenerator();
	~FaultGenerator(void);
	int generateFault(Core* core, configuration* config, gate* g);
};

