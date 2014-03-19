#pragma once
#include "liberty.h"

using namespace  std;

struct timingData{
	double rise, fall;
};


class library{
	liberty::Library lib;

public:
	void read_library(string);
	void analyze();
	liberty::Library getLib();
	timingData getTimingForCell(string, int, double);
	float getCapacitanceForCell(string, int);
};

