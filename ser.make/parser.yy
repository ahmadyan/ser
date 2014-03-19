// ****************************************************************************
// ****************************************************************************
// parser.yy
//
// Yacc input file for Liberty parser.
//
// ****************************************************************************


%{
#include <vector>
#include <stack>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include "liberty.h"
#include "utils.h"
#include "parserFunctions.h"
int yylex(void);

extern liberty::Library   __y_library;
liberty::Cell             __y_cell;
liberty::Pin              __y_pin;
liberty::Timing           __y_timing;
utils::Table              __y_table;

std::string               __y_string;
std::vector<double>       __y_values;
/*
int __y_retVal;
char __y_errString[500];
std::stack<std::string> __y_buffer_stack;
std::vector<std::string> __y_buffer_list;
double __y_value;
std::vector<double> __y_value_list;
tcfTolGroup __y_tolerance_group;
std::vector<tcfLayerTypeEnum> __y_layers;
tcfLayerInfo __y_layer_info;
std::pair<std::string, std::string> __y_labeled_string;
std::vector<std::pair<std::string, std::string> > __y_labeled_strings_list;
std::map<double, std::map<double, std::map<double, double> > >
	__y_min_tolerance_map_entries;
std::map<double, std::map<double, std::map<double, double> > >
	__y_max_tolerance_map_entries;
*/

%}

%union {
int integer;
double decimal;
char *string;
}


%token LBRACE
%token RBRACE
%token LPAREN
%token RPAREN
%token COLON
%token SEMICOLON
%token COMMA
%token INDEX_1
%token INDEX_2
%token INDEX_3
%token PIN
%token LIBRARY
%token CELL_LEAKAGE_POWER
%token CELL_FOOTPRINT
%token AREA
%token LEAKAGE_POWER
%token FUNCTION
%token CAPACITANCE
%token MAX_CAPACITANCE
%token WHEN
%token DIRECTION
%token VALUE
%token LIBRARY_FEATURES
%token CAPACITIVE_LOAD_UNIT
%token OPERATING_CONDITIONS
%token OUTPUT_VOLTAGE
%token INPUT_VOLTAGE
%token WIRE_LOAD
%token CELL
%token INTERNAL_POWER
%token TIMING
%token RISE_POWER
%token FALL_POWER
%token TIMING_SENSE
%token POSITIVE_UNATE
%token NEGATIVE_UNATE
%token NON_UNATE
%token CELL_RISE
%token CELL_FALL
%token RISE_TRANSITION
%token FALL_TRANSITION
%token SDF_COND
%token VALUES
%token RELATED_PIN
%token PROCESS
%token TEMPERATURE
%token VOLTAGE
%token TREE_TYPE
%token VARIABLE_1
%token VARIABLE_2
%token VARIABLE_3
%token VOL
%token VOH
%token VOMIN
%token VOMAX
%token VIL
%token VIH
%token VIMIN
%token VIMAX
%token RESISTANCE
%token SLOPE
%token FANOUT_LENGTH
%token EQUAL_OR_OPPOSITE_OUTPUT
%token TIMING_TYPE
%token RISE_CONSTRAINT
%token FALL_CONSTRAINT
%token CLOCK
%token TRUE
%token FALSE
%token MAX_TRANSITION
%token MIN_PULSE_WIDTH_HIGH
%token MIN_PULSE_WIDTH_LOW
%token FF
%token LATCH
%token DRIVER_TYPE
%token DONT_TOUCH
%token DONT_USE
%token THREE_STATE
%token POWER 
%token SIGNAL_TYPE
%token TEST_CELL
%token CLOCK_GATING_INTEGRATED_CELL
%token CLOCK_GATE_ENABLE_PIN
%token CLOCK_GATE_CLOCK_PIN
%token CLOCK_GATE_OUT_PIN
%token CLOCK_GATE_TEST_PIN
%token STATETABLE 
%token STATE_FUNCTION
%token TABLE
%token INTERNAL_NODE
%token VOLTAGE_MAP
%token PG_PIN
%token VOLTAGE_NAME
%token PG_TYPE 
%token RELATED_POWER_PIN
%token RELATED_GROUND_PIN
%token NEXTSTATE_TYPE
%token MIN_PERIOD
%token TECHNOLOGY
%token DEFINE
%token OUTPUT_CURRENT_TEMPLATE
%token POWER_LUT_TEMPLATE
%token LU_TABLE_TEMPLATE
%token DRIVE_STRENGTH
%token FALL_CAPACITANCE
%token RISE_CAPACITANCE
%token FALL_CAPACITANCE_RANGE
%token RISE_CAPACITANCE_RANGE
%token IS_FILLER_CELL

%token <integer> INTEGER
%token <decimal> DECIMAL
%token <string> STRING
%token <string> QUOTED_STRING

%type <decimal> value
%type <string> string

%%


library: LIBRARY LPAREN string RPAREN LBRACE lib_data RBRACE
{
}
;

lib_data: header cells
;


header: 
| header string COLON string SEMICOLON
| header TECHNOLOGY LPAREN string RPAREN SEMICOLON	
| header DEFINE LPAREN string COMMA string COMMA string RPAREN SEMICOLON	
| header DEFINE LPAREN string COMMA OPERATING_CONDITIONS COMMA string RPAREN SEMICOLON	
| header DEFINE LPAREN string COMMA CELL COMMA string RPAREN SEMICOLON
| header DEFINE LPAREN DRIVE_STRENGTH COMMA CELL COMMA string RPAREN SEMICOLON
| header LIBRARY_FEATURES LPAREN string RPAREN SEMICOLON
| header LIBRARY_FEATURES LPAREN string COMMA string RPAREN SEMICOLON
| header CAPACITIVE_LOAD_UNIT LPAREN string COMMA string RPAREN SEMICOLON
| header VOLTAGE_MAP LPAREN string COMMA string RPAREN SEMICOLON
| header operating_conditions
| header table_template
| header pad_attribute
| header wire_load
| header output_current_template
| header power_lut_template
| header lu_table_template
;

cells: 
{
  __y_cell.clear();
}
| cells cell
{
  __y_library.addCell(__y_cell);
  __y_cell.clear();
}
;



operating_conditions: OPERATING_CONDITIONS LPAREN string RPAREN LBRACE operating_conditions_dataset RBRACE
;

table_template: string LPAREN string RPAREN LBRACE table_data RBRACE
;

pad_attribute: OUTPUT_VOLTAGE LPAREN string RPAREN LBRACE output_voltage_data RBRACE
| INPUT_VOLTAGE LPAREN string RPAREN LBRACE input_voltage_data RBRACE
;

wire_load: WIRE_LOAD LPAREN string RPAREN LBRACE wire_load_data RBRACE
;

pg_pin: PG_PIN LPAREN string RPAREN LBRACE pg_pin_data RBRACE
;

power_lut_template: POWER_LUT_TEMPLATE LPAREN string RPAREN LBRACE template_data RBRACE
;

lu_table_template: LU_TABLE_TEMPLATE LPAREN string RPAREN LBRACE template_data RBRACE
;

output_current_template: OUTPUT_CURRENT_TEMPLATE LPAREN string RPAREN LBRACE template_data RBRACE
;



cell: CELL LPAREN string RPAREN LBRACE cell_data RBRACE
{
  __y_cell.setName($3);
  free($3);
}
;



operating_conditions_dataset:
| operating_conditions_data 
| operating_conditions_data SEMICOLON operating_conditions_dataset
;

operating_conditions_data: PROCESS COLON value
| TEMPERATURE COLON value
| VOLTAGE COLON value
| TREE_TYPE COLON string
| string COLON string
;

table_data:
| table_data VARIABLE_1 COLON string SEMICOLON
| table_data VARIABLE_2 COLON string SEMICOLON
| table_data VARIABLE_3 COLON string SEMICOLON
| table_data INDEX_1 LPAREN multiple_values RPAREN SEMICOLON
| table_data INDEX_2 LPAREN multiple_values RPAREN SEMICOLON
| table_data INDEX_3 LPAREN multiple_values RPAREN SEMICOLON
;

template_data:
| template_data VARIABLE_1 COLON string SEMICOLON
| template_data VARIABLE_2 COLON string SEMICOLON
| template_data VARIABLE_3 COLON string SEMICOLON
| template_data INDEX_1 LPAREN multiple_values RPAREN 
| template_data INDEX_2 LPAREN multiple_values RPAREN 
| template_data INDEX_3 LPAREN multiple_values RPAREN 
;


output_voltage_data: 
| output_voltage_data VOL COLON strings SEMICOLON
| output_voltage_data VOH COLON strings SEMICOLON
| output_voltage_data VOMIN COLON strings SEMICOLON
| output_voltage_data VOMAX COLON strings SEMICOLON
;

input_voltage_data: 
| input_voltage_data VIL COLON strings SEMICOLON
| input_voltage_data VIH COLON strings SEMICOLON
| input_voltage_data VIMIN COLON strings SEMICOLON
| input_voltage_data VIMAX COLON strings SEMICOLON
;

wire_load_data: 
| wire_load_data RESISTANCE COLON value SEMICOLON
| wire_load_data CAPACITANCE COLON value SEMICOLON
| wire_load_data AREA COLON value SEMICOLON
| wire_load_data SLOPE COLON value SEMICOLON
| wire_load_data FANOUT_LENGTH LPAREN value COMMA value RPAREN SEMICOLON
;


cell_data: 
| cell_data CELL_FOOTPRINT COLON string SEMICOLON
{
  __y_cell.setFootprint($4);
  free($4);
}
| cell_data is_filler_cell
| cell_data CLOCK_GATING_INTEGRATED_CELL COLON string SEMICOLON
| cell_data dont_touch
| cell_data dont_use
| cell_data pg_pin
| cell_data DRIVE_STRENGTH COLON value SEMICOLON
| cell_data AREA COLON value SEMICOLON
{
  __y_cell.setArea($4);
}
| cell_data pin
{
  __y_cell.addPin(__y_pin);
  __y_pin.clear();
}
| cell_data cell_leakage_power
| cell_data leakage_power
| cell_data ff
| cell_data latch
| cell_data test_cell
| cell_data statetable
;

pg_pin_data:
| pg_pin_data VOLTAGE_NAME COLON string SEMICOLON
| pg_pin_data PG_TYPE COLON string SEMICOLON
;

is_filler_cell: IS_FILLER_CELL COLON TRUE SEMICOLON
| IS_FILLER_CELL COLON FALSE SEMICOLON
;


dont_touch: DONT_TOUCH COLON TRUE SEMICOLON
| DONT_TOUCH COLON FALSE SEMICOLON
;

dont_use: DONT_USE COLON TRUE SEMICOLON
| DONT_USE COLON FALSE SEMICOLON
;

pin: PIN LPAREN string RPAREN LBRACE pin_data RBRACE
{
  __y_pin.setName($3);
  free($3);
}
;

cell_leakage_power: CELL_LEAKAGE_POWER COLON value SEMICOLON
;

leakage_power: LEAKAGE_POWER LPAREN RPAREN LBRACE leakage_power_data RBRACE
;

ff: FF LPAREN string COMMA string RPAREN LBRACE ff_data RBRACE
;

latch: LATCH LPAREN string COMMA string RPAREN LBRACE latch_data RBRACE
;

test_cell: TEST_CELL LPAREN RPAREN LBRACE test_cell_data RBRACE
;

statetable: STATETABLE LPAREN string COMMA string RPAREN LBRACE statetable_data RBRACE
;

statetable_data: TABLE COLON string SEMICOLON
;

test_cell_data: 
| test_cell_data test_pin
| test_cell_data ff
;

test_pin: PIN LPAREN string RPAREN LBRACE test_pin_data RBRACE
;

test_pin_data: 
| test_pin_data DIRECTION COLON string SEMICOLON
| test_pin_data FUNCTION COLON string SEMICOLON
| test_pin_data SIGNAL_TYPE COLON string SEMICOLON
;

pin_data:
{
  __y_pin.clear();
}
| pin_data DIRECTION COLON string SEMICOLON
{
  __y_pin.setDirection($4);
  free($4);
}
| pin_data INTERNAL_NODE COLON string SEMICOLON
| pin_data STATE_FUNCTION COLON string SEMICOLON
| pin_data clock_gate_enable_pin
| pin_data clock_gate_clock_pin
| pin_data clock_gate_out_pin
| pin_data clock_gate_test_pin
| pin_data clock
| pin_data THREE_STATE COLON string SEMICOLON
| pin_data DRIVER_TYPE COLON string SEMICOLON
| pin_data MAX_TRANSITION COLON value SEMICOLON
| pin_data RISE_CAPACITANCE COLON value SEMICOLON
| pin_data FALL_CAPACITANCE COLON value SEMICOLON
| pin_data RISE_CAPACITANCE_RANGE LPAREN value COMMA value RPAREN SEMICOLON
| pin_data FALL_CAPACITANCE_RANGE LPAREN value COMMA value RPAREN SEMICOLON
| pin_data CAPACITANCE COLON value SEMICOLON
{
  __y_pin.setCapacitance($4);
}
| pin_data FUNCTION COLON string SEMICOLON
| pin_data INTERNAL_POWER LPAREN RPAREN LBRACE internal_power_data RBRACE
| pin_data TIMING LPAREN RPAREN LBRACE timing_data RBRACE
{
  __y_pin.addTiming(__y_timing);
}
| pin_data RELATED_POWER_PIN COLON string SEMICOLON
| pin_data RELATED_GROUND_PIN COLON string SEMICOLON
| pin_data NEXTSTATE_TYPE COLON string SEMICOLON
| pin_data MIN_PERIOD COLON string SEMICOLON
| pin_data MAX_CAPACITANCE COLON value SEMICOLON
| pin_data MIN_PULSE_WIDTH_HIGH COLON value SEMICOLON
| pin_data MIN_PULSE_WIDTH_LOW COLON value SEMICOLON
;

leakage_power_data: when leakage_power_data_value
| leakage_power_data_value when
;

ff_data: 
| ff_data string COLON string SEMICOLON
;

latch_data: 
| latch_data string COLON string SEMICOLON
;

leakage_power_data_value: VALUE COLON value SEMICOLON
;

clock_gate_enable_pin: CLOCK_GATE_ENABLE_PIN COLON TRUE SEMICOLON
| CLOCK_GATE_ENABLE_PIN COLON FALSE SEMICOLON
;

clock_gate_clock_pin: CLOCK_GATE_CLOCK_PIN COLON TRUE SEMICOLON
| CLOCK_GATE_CLOCK_PIN COLON FALSE SEMICOLON
;

clock_gate_out_pin: CLOCK_GATE_OUT_PIN COLON TRUE SEMICOLON
| CLOCK_GATE_OUT_PIN COLON FALSE SEMICOLON
;

clock_gate_test_pin: CLOCK_GATE_TEST_PIN COLON TRUE SEMICOLON
| CLOCK_GATE_TEST_PIN COLON FALSE SEMICOLON
;

clock: CLOCK COLON TRUE SEMICOLON
| CLOCK COLON FALSE SEMICOLON
;

internal_power_data:
| internal_power_data related_pin
| internal_power_data EQUAL_OR_OPPOSITE_OUTPUT COLON string SEMICOLON
| internal_power_data when
| internal_power_data fall_power
| internal_power_data rise_power
| internal_power_data power
;

timing_data: 
{
  __y_timing.clear();
}
| timing_data related_pin
| timing_data timing_type
| timing_data timing_sense
| timing_data when
| timing_data sdf_cond
| timing_data cell_rise
{
  __y_timing.setCellRise(__y_table);
}
| timing_data rise_transition
{
  __y_timing.setRiseTransition(__y_table);
}
| timing_data cell_fall
{
  __y_timing.setCellFall(__y_table);
}
| timing_data fall_transition
{
  __y_timing.setFallTransition(__y_table);
}
| timing_data rise_constraint
| timing_data fall_constraint
;

related_pin: RELATED_PIN COLON string SEMICOLON
;

timing_type: TIMING_TYPE COLON string SEMICOLON
;

when: WHEN COLON string SEMICOLON
;

fall_power: FALL_POWER LPAREN string RPAREN LBRACE table RBRACE
;

rise_power: RISE_POWER LPAREN string RPAREN LBRACE table RBRACE
;

power: POWER LPAREN string RPAREN LBRACE table RBRACE
;

timing_sense: TIMING_SENSE COLON POSITIVE_UNATE SEMICOLON
| TIMING_SENSE COLON NEGATIVE_UNATE SEMICOLON
| TIMING_SENSE COLON NON_UNATE SEMICOLON
;

sdf_cond: SDF_COND COLON string SEMICOLON
;

cell_rise: CELL_RISE LPAREN string RPAREN LBRACE table RBRACE
;

rise_transition: RISE_TRANSITION LPAREN string RPAREN LBRACE table RBRACE
;

cell_fall: CELL_FALL LPAREN string RPAREN LBRACE table RBRACE
;

fall_transition: FALL_TRANSITION LPAREN string RPAREN LBRACE table RBRACE
;

rise_constraint: RISE_CONSTRAINT LPAREN string RPAREN LBRACE table RBRACE
;

fall_constraint: FALL_CONSTRAINT LPAREN string RPAREN LBRACE table RBRACE
;


table: 
{
  __y_table.clear();
}
| table INDEX_1 LPAREN multiple_values RPAREN SEMICOLON
{
  __y_table.setIndex1(__y_values);
}
| table INDEX_2 LPAREN multiple_values RPAREN SEMICOLON
{
  __y_table.setIndex2(__y_values);
}
| table INDEX_3 LPAREN multiple_values RPAREN SEMICOLON
{
  __y_table.setIndex3(__y_values);
}
| table VALUES LPAREN multiple_values RPAREN SEMICOLON
{
  __y_table.setValues(__y_values);
}
;


multiple_values: multiple_quoted_strings
| values_comma
;

multiple_quoted_strings: QUOTED_STRING
{
  __y_values = utils::splitMultipleValues($1);
  free($1);
}
| multiple_quoted_strings COMMA QUOTED_STRING
{
  std::vector<double> tmp = utils::splitMultipleValues($3);
  free($3);
  std::vector<double> tmp2(tmp.size() + __y_values.size());
  copy(tmp.begin(), tmp.end(), 
    copy(__y_values.begin(), __y_values.end(), tmp2.begin()));
  __y_values = tmp2;
}
;


values_comma: value
{
  __y_values.clear();
  __y_values.push_back($1);
}
| values_comma COMMA value
{
  __y_values.push_back($3);
}
;

string: STRING
{
  $$ = strdup($1);
  free($1);
}
| QUOTED_STRING
{
  $$ = strdup($1);
  free($1);
}
| INTEGER
{
  $$ = strdup("INTEGER");
}
| DECIMAL
{
  $$ = strdup("DECIMAL");
}
;


strings: string
{
}
| strings string
{
}
;

value: INTEGER
{
  $$ = $1;
}
| DECIMAL
{
  $$ = $1;
}
;
