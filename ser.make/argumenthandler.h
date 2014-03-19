#pragma once
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <inttypes.h>
#include <getopt.h>
#include <cstring> 
#include "configuration.h"

using namespace std;
class ArgumentHandler
{
public:
	ArgumentHandler(void);
	~ArgumentHandler(void);
	void parse(int argc, char *argv[], configuration* config);
	void print_help(char *nam);
	void initial_checking(configuration*);
};

