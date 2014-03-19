#pragma once
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string.h>

#include "configuration.h"
#include "core.h"
#include "vcd.h"

#define MAXLUT 7        /* Maximum number of inputs per LUT */
#define HASHSIZE 4095
#define NAMELENGTH 16   /* Length of the name stored for each net */ 
#define DEBUG 1         /* Echoes input & checks error conditions */
/*#define VERBOSE 1*/   /* Prints all sorts of intermediate data */

#ifndef TRUE      /* Some compilers predefine TRUE, FALSE */
typedef enum {FALSE, TRUE} boolean;
#else
typedef int boolean;
#endif

#define BUFSIZE 3000     /* Maximum line length for various parsing proc. */
#define max(a,b) (((a) > (b))? (a) : (b))
#define min(a,b) ((a) > (b)? (b) : (a))
#define nint(a) ((int) floor (a + 0.5))

#define NO_CLUSTER -1
#define NEVER_CLUSTER -2
#define NOT_VALID -10000  /* Marks gains that aren't valid */
                          /* Ensure no gain can ever be this negative! */
#define UNDEFINED -1    

#define DRIVER 0     /* Is a pin driving a net or in the fanout? */
#define RECEIVER 1
#define OPEN -1      /* Pin is unconnected. */

enum block_types {INPAD = -2, OUTPAD, LUT, LATCH, EMPTY, LUT_AND_LATCH};
enum e_cluster_seed {TIMING, MAX_INPUTS};

struct hash_nets {char *name; int index; int count; 
   struct hash_nets *next;}; 
/* count is the number of pins on this net so far. */

struct s_net {char *name; int num_pins; int *pins;};
/* name:  ASCII net name for informative annotations in the output.  *
 * num_pins:  Number of pins on this net.                            *
 * pins[]: Array containing the blocks to which the pins of this net *
 *         connect.  Output in pins[0], inputs in other entries.     */

struct s_block {
	char *name; 
	enum block_types type; 
	int num_nets;
    int nets[MAXLUT+2];
	vector<string> *value ;
}; 
/* name:  Taken from the net which it drives.                        *
 * type:  LUT, INPAD, OUTPAD or LATCH.                               *
 * num_nets:  number of nets connected to this block.                *
 * nets[]:  List of nets connected to this block.  Net[0] is the     *
 *          output, others are inputs, except for OUTPAD.  OUTPADs   *
 *          only have an input, so this input is in net[0].          */


using namespace std;
class fpga
{
	int *num_driver, *temp_num_pins;
	/* # of .input, .output, .model and .end lines */
	int ilines, olines, model_lines, endlines;  
	struct hash_nets **hash;
	char *model;
	FILE *blif;
	int cont;  /* line continued? */
	int linenum;  /* line in file being parsed */
	/* Netlist description data structures. */
	int num_nets, num_blocks;     
	int num_p_inputs, num_p_outputs;
	struct s_net *net;
	struct s_block *block;

	/* Number in original netlist, before FF packing. */
	int num_luts, num_latches; 

	//functions---------------------------------------------------------------------
	int add_net (char *ptr, int type, int bnum, int doall); 
	void get_tok(char *buffer, int pass, int doall, int *done, int lut_size);
	void init_parse(int doall);
	void check_net (int lut_size);
	void free_parse (void);
	void io_line (int in_or_out, int doall);
	void add_lut (int doall, int lut_size);
	void add_latch (int doall, int lut_size);
	void dum_parse (char *buf);
	int hash_value (char *name);
	FILE* my_fopen (char *fname, char *flag, int prompt);
	char *my_fgets(char *buf, int max_size, FILE *fp); 
	char *my_fgets_old(char *buf, int max_size, FILE *fp); 
	char *my_strtok(char *ptr, char *tokens, FILE *fp, char *buf);
public:
	configuration* config;
	fpga(configuration*);
	~fpga(void);
	void read_blif (FILE* blif_file, Core* c);
	void echo_input (char* blif_file, int lut_size, char *echo_file);
	int convert_blif_to_core(FILE*, vcd*, Core* core);
	bool match(string in, int value);
	int getNodeId(char* nodeName);
};

