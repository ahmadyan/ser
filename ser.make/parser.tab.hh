
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LBRACE = 258,
     RBRACE = 259,
     LPAREN = 260,
     RPAREN = 261,
     COLON = 262,
     SEMICOLON = 263,
     COMMA = 264,
     INDEX_1 = 265,
     INDEX_2 = 266,
     INDEX_3 = 267,
     PIN = 268,
     LIBRARY = 269,
     CELL_LEAKAGE_POWER = 270,
     CELL_FOOTPRINT = 271,
     AREA = 272,
     LEAKAGE_POWER = 273,
     FUNCTION = 274,
     CAPACITANCE = 275,
     MAX_CAPACITANCE = 276,
     WHEN = 277,
     DIRECTION = 278,
     VALUE = 279,
     LIBRARY_FEATURES = 280,
     CAPACITIVE_LOAD_UNIT = 281,
     OPERATING_CONDITIONS = 282,
     OUTPUT_VOLTAGE = 283,
     INPUT_VOLTAGE = 284,
     WIRE_LOAD = 285,
     CELL = 286,
     INTERNAL_POWER = 287,
     TIMING = 288,
     RISE_POWER = 289,
     FALL_POWER = 290,
     TIMING_SENSE = 291,
     POSITIVE_UNATE = 292,
     NEGATIVE_UNATE = 293,
     NON_UNATE = 294,
     CELL_RISE = 295,
     CELL_FALL = 296,
     RISE_TRANSITION = 297,
     FALL_TRANSITION = 298,
     SDF_COND = 299,
     VALUES = 300,
     RELATED_PIN = 301,
     PROCESS = 302,
     TEMPERATURE = 303,
     VOLTAGE = 304,
     TREE_TYPE = 305,
     VARIABLE_1 = 306,
     VARIABLE_2 = 307,
     VARIABLE_3 = 308,
     VOL = 309,
     VOH = 310,
     VOMIN = 311,
     VOMAX = 312,
     VIL = 313,
     VIH = 314,
     VIMIN = 315,
     VIMAX = 316,
     RESISTANCE = 317,
     SLOPE = 318,
     FANOUT_LENGTH = 319,
     EQUAL_OR_OPPOSITE_OUTPUT = 320,
     TIMING_TYPE = 321,
     RISE_CONSTRAINT = 322,
     FALL_CONSTRAINT = 323,
     CLOCK = 324,
     TRUE = 325,
     FALSE = 326,
     MAX_TRANSITION = 327,
     MIN_PULSE_WIDTH_HIGH = 328,
     MIN_PULSE_WIDTH_LOW = 329,
     FF = 330,
     LATCH = 331,
     DRIVER_TYPE = 332,
     DONT_TOUCH = 333,
     DONT_USE = 334,
     THREE_STATE = 335,
     POWER = 336,
     SIGNAL_TYPE = 337,
     TEST_CELL = 338,
     CLOCK_GATING_INTEGRATED_CELL = 339,
     CLOCK_GATE_ENABLE_PIN = 340,
     CLOCK_GATE_CLOCK_PIN = 341,
     CLOCK_GATE_OUT_PIN = 342,
     CLOCK_GATE_TEST_PIN = 343,
     STATETABLE = 344,
     STATE_FUNCTION = 345,
     TABLE = 346,
     INTERNAL_NODE = 347,
     VOLTAGE_MAP = 348,
     PG_PIN = 349,
     VOLTAGE_NAME = 350,
     PG_TYPE = 351,
     RELATED_POWER_PIN = 352,
     RELATED_GROUND_PIN = 353,
     NEXTSTATE_TYPE = 354,
     MIN_PERIOD = 355,
     TECHNOLOGY = 356,
     DEFINE = 357,
     OUTPUT_CURRENT_TEMPLATE = 358,
     POWER_LUT_TEMPLATE = 359,
     LU_TABLE_TEMPLATE = 360,
     DRIVE_STRENGTH = 361,
     FALL_CAPACITANCE = 362,
     RISE_CAPACITANCE = 363,
     FALL_CAPACITANCE_RANGE = 364,
     RISE_CAPACITANCE_RANGE = 365,
     IS_FILLER_CELL = 366,
     INTEGER = 367,
     DECIMAL = 368,
     STRING = 369,
     QUOTED_STRING = 370
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 50 "parser.yy"

int integer;
double decimal;
char *string;



/* Line 1676 of yacc.c  */
#line 175 "parser.tab.hh"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


