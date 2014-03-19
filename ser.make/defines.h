#pragma once
#pragma warning( disable : 4786)
#pragma warning( disable : 4018)
#pragma warning( disable : 4224)
#pragma warning( disable : 4244)
#pragma warning( disable : 4996)
#pragma warning( disable : 4291) //no matching operator delete
#pragma warning( disable : 4305) // truncation from 'const double' to 'float'

/////// Attention: MAX should be assigned at least to the number of graph nodes.
#define MAX_GATE_INPUTS 2000  // Maximum number of inputs of a typical gate
#define MAX_DFF 250
#define MAX_CLK 12
#define MAX_PO 500
#define sp_error_tolerance   0.05	; //  0.05   0.103; 0.206	// old value=0.05f;


#define derating_sim_error_tolerance 0.10
#define AREA_SCALE 1.8 //This is when W/L is multiplied by 4
#define SCALE 4
#define VDD 1.5
#define VGD 0.0

// Technology Spedicifications
#define DEFAULT	0	//default values, based on TSMC 0.25um (I think)
#define	TSMC250	1	//not supported yet
#define	TSMC180	2	//not supported yet
#define	UMC130	3	//not supported yet
#define UMC110	4	//not supported yet
#define	UMC90	5
#define	UMC65	6
#define	PTM45	7

#define GATE_NAND2	0
#define GATE_NAND3	1
#define GATE_NAND4	2
#define GATE_AND2	3
#define GATE_AND3	4
#define GATE_AND4	5
#define GATE_NOR2	6
#define GATE_NOR3	7
#define GATE_NOR4	8
#define GATE_OR2	9
#define GATE_OR3	10
#define GATE_OR4	11
#define GATE_XOR	12
#define GATE_XNOR	13
#define GATE_INV	14
#define GATE_BUF	15
#define GATE_DFF	16

