=====================================================================================
    MultiCycle Logical-Electrical-Timing Soft-Error-Estimation Tool (MLET-SER)
=====================================================================================
	Version 0.1.0
	(C) 2009-2010 Dependable System Lab [DSL], 
			Computer Engineering department	
			Sharif University of technology.
	(C) 2004-2008 Hossein Asadi, NorthEastern University.
								 EMC Corp.
	
	first developed by Hossein Asadi at EMC Corp/Northeastern University, June 2004 ~ January 2008		
	maintainer		NematAllah "adel" Ahmadyan [ahmadyan at gmail dot com]
					Mehdi "babak" Fazeli  [m_fazeli at ce dot sharif dot edu]
					 
					 
Dependancies:
	our compilation system is Makefile based. just make.
	you need bison/flex.
	I use GNU bison version 2.4.1 from gnuwin32 project.
		  GNU flex  version 2.5.4 from gnuwin32 project
		  GNU gcc version 4.2.1 (mingw32-2) 


	I also tested with with gcc-3.3, gcc-4.1 and gcc-4.3 on ubuntu linux and debian linux.
	this program can only be compiled with gcc, seems like Microsoft Visual C++ 2008 has a problem with the
	extern C's that I used for bridging between our main code and bison's code. however gcc is fine.
	
	I use some of the boost header, boost >= 1.41.0
	for regex, I need headers and binaries, for other includes, only header is fine.
	remember to set the path to boosts header in Makefile.
		
	adel




Version History:
0.0.0 2004-2010 [...]
original version, written and maintained by Hossein Asadi.
=====================================================================
0.0.1 - 1.8.2010 @adel [DSN]
We use this version to extract results for DSN-2010 paper.
now we support LET in both SER analysis and M.C. simulation in Multi-Cycle.
=====================================================================
0.0.2 - 6.31.2010 @adel [TC, TCAD]
This version is used for extracting TC paper.
* general code refactoring (utils, defines), major modification of main
* Bugfix: Our tool used to crash or produce unexpected results. I performed a Valgrind to check for memory leaks or invalid accesses.
* I add the support for Non-linear-delay model (liberty, library, bison (parser), flex(lex) ) and extract result with Nan-45nm library. use -l option
	it was used to re-extract the results for Mehdi's TCAD paper in ISCA testbench.
* we support application derating and we can connect to modelsim's interface by reading in VCD files (class vcd)
for example, to generate vcd file in modelsim:
we can read this file by suppling -v and defining clock period as -c
in VSIM
	vsim -novopt work.test
	vcd file s27.vcd
	vcd add -in /test/s/*
	run 100 us
in SER
	ser -i input-netlist.v -v vcdfile.vcd -c 10
* I add two new individual class, ScriptGenerator and ResultAnalysis to make my life easier ;) do make sim and make analysis
( Now we (kinda) support synopsys generated synthesized netlist. but with slight modification:
first do this regular expression to direct-input mapping in verilog:
Find:   \.Y((\({[^"][^\)]*}\)))
Replace: \1
for all inputs, i.e. replace .Y with .A, .B. etc.
I think it needed something else too but I don't remember what! maybe I ask MJ.
we don't (yet) support arrays. so replace something like array[10] at input/output/wire with array[0], array[1], etc.
By the way, we should define synopsys style input netlist wit -s input options, like
ser -i input.v -s
with this, I analyse the MIPS and OpenRISC.
* maze algorithm:
	we have a new champion algorithm that replaces the old heuristic algorithm (DSN10). the default is still heurestic, however
	running -a maze will swith to maze witch is more accurate.
=====================================================================
0.0.3 - 7.5.2010 @adel [MBU]
We added the priliminary support for multiply event transient (MET) and multiply bit upset (MBU) to our toolset.
Class mbu contains algorithms for finding mbu site (neighbor gates using BFS alg. )
=====================================================================
0.0.3 - 7.5.2010 @adel [FPGA]
Now we support FPGA architecture.
	a. supporting blif input file
	b. supporting LUT as a gate type
	c. supporting interconnection network

filogy:
    blif : technology mapped netlist, contains lut, ff and maybe some black box
	net  : 

how to generate:
	t-vpack input.blif output.net [-options]


=====================================================================
0.1.0 - 7.5.2010 @adel [refactoring]