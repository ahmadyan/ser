#pragma once
#include <iostream> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <vector>

using namespace std;

typedef uint32_t fstHandle;

enum fstBlockType {
	FST_BL_HDR		       = 0,
	FST_BL_VCDATA              = 1,
	FST_BL_BLACKOUT	       = 2,
	FST_BL_GEOM                = 3,
	FST_BL_HIER                = 4,

	FST_BL_ZWRAPPER	       = 254,   /* indicates that whole trace is gz wrapped */
	FST_BL_SKIP		       = 255	/* used while block is being written */
};

enum fstScopeType {
	FST_ST_VCD_MIN             = 0,
	FST_ST_VCD_MODULE          = 0,
	FST_ST_VCD_TASK            = 1,
	FST_ST_VCD_FUNCTION        = 2,
	FST_ST_VCD_BEGIN           = 3,
	FST_ST_VCD_FORK            = 4,
	FST_ST_VCD_MAX             = 4,

	FST_ST_MAX                 = 4,

	FST_ST_VCD_SCOPE           = 254,
	FST_ST_VCD_UPSCOPE         = 255
};

enum fstVarType {
	FST_VT_VCD_MIN             = 0,	/* start of VCD datatypes */
	FST_VT_VCD_EVENT           = 0,
	FST_VT_VCD_INTEGER         = 1,
	FST_VT_VCD_PARAMETER       = 2,
	FST_VT_VCD_REAL            = 3,
	FST_VT_VCD_REAL_PARAMETER  = 4,
	FST_VT_VCD_REG             = 5,
	FST_VT_VCD_SUPPLY0         = 6,
	FST_VT_VCD_SUPPLY1         = 7,
	FST_VT_VCD_TIME            = 8,  
	FST_VT_VCD_TRI             = 9,
	FST_VT_VCD_TRIAND          = 10,
	FST_VT_VCD_TRIOR           = 11,
	FST_VT_VCD_TRIREG          = 12,
	FST_VT_VCD_TRI0            = 13,
	FST_VT_VCD_TRI1            = 14,
	FST_VT_VCD_WAND            = 15,
	FST_VT_VCD_WIRE            = 16,
	FST_VT_VCD_WOR             = 17,
	FST_VT_VCD_PORT            = 18,
	FST_VT_VCD_ARRAY           = 19,	/* used to define the rownum (index) port on the array */
	FST_VT_VCD_REALTIME        = 20,
	FST_VT_VCD_MAX             = 20,	/* end of VCD datatypes */

	FST_VT_GEN_STRING	       = 254,	/* generic string type   (max len is defined as the len in fstWriterCreateVar() */
	FST_VT_GEN_MEMBLOCK	       = 255	/* generic memblock type (max len is defined as the len in fstWriterCreateVar() */
};

enum fstVarDir {
	FST_VD_IMPLICIT    = 0,
	FST_VD_INPUT       = 1,
	FST_VD_OUTPUT      = 2,
	FST_VD_INOUT       = 3,

	FST_VD_MAX         = 3
};

enum fstHierType {
	FST_HT_SCOPE       = 0,
	FST_HT_UPSCOPE     = 1,
	FST_HT_VAR         = 2,

	FST_HT_MAX         = 2
};

struct fstHier
{
	unsigned char htyp;

	union {
		/* if htyp == FST_HT_SCOPE */
		struct fstHierScope {
			unsigned char typ; /* FST_ST_VCD_MODULE ... FST_ST_VCD_FORK */
			const char *name;
			const char *component;
		} scope;

		/* if htyp == FST_HT_VAR */
		struct fstHierVar {
			unsigned char typ; /* FST_VT_VCD_EVENT ... FST_VT_VCD_WOR */
			unsigned char direction; /* FST_VD_IMPLICIT ... FST_VD_INOUT */
			const char *name;
			uint32_t length;
			fstHandle handle;
			unsigned is_alias : 1;
		} var;
	} u;
};



class wave{
	int id ;
	string vcd_id ;
	string name ;	 
	vector<pair<int,int> > transients ;
	int counter;
public:
	wave(int _id, string _vcd_id, string _name);
	int getId();
	void addTransient(int time, int value);
	void addTransient(int time, char value);
	void dump();
	short getNextLogicValue(int);
	string getName();
	void randomizeCounter();
};

class wavePool {
	int currentTime;
	vector<wave*> *waves ;
public:
	wavePool();
	~wavePool();
	void addwave(wave* w);
	void addwave(int id, string vcd_id, string name);
	void addtransient(int id, char value);
	void setTime(int time);
	// finalle is required to keep track of the time of the signal's ending
	void finalle();
	void dump();
	short getNextLogicValue(string signalName,int);
	void randomizeCounter();
};


class vcd{
	wavePool *pool;
	vector<pair<string,int> > pi_list;
	int clk;
public:
	vcd();
	int read_vcd(char *vname);
	void dump();
	void addPI(string name, int id);
	short getNextLogicValue(int id);
	void setClk(int clk);
	void resetCounter();
	string getSignalName(int id);
	void randomizeCounter();
};