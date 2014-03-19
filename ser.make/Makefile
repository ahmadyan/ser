# DSL-Analytic Makefile version 0.0.2
CC=g++
CFLAGS=-O2 -w -Wno-write-strings
#BOOST=C:/gcc/boost_1_41_0
BOOST=/opt/local/var/macports/software/boost/1.44.0_0/opt/local/include/
SRC=main.cpp seu.cpp parser.tab.cc utils.cpp liberty.cpp lex.yy.c library.cpp core.cpp vcd.cpp mbu.cpp faultsite.cpp faultgen.cpp fpga.cpp configuration.cpp argumenthandler.cpp set-vs-met.cpp
OUT=-o ser.exe
all: main

main: bison flex $(SRC)
	$(CC)  $(OUT) $(CFLAGS) $(SRC)  -I$(BOOST)

bison: parser.yy
#	bison -d parser.yy

flex: tokenizer.l
#	flex tokenizer.l

sim:
	$(CC) $(CFLAGS) ScriptGenerator.cpp -o ScriptGenerator.exe -I$(BOOST)

analysis:
	$(CC) $(CFLAGS) ResultAnalysis.cpp -o ResultAnalysis.exe -I$(BOOST)
