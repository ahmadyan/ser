
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 10 "parser.yy"

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



/* Line 189 of yacc.c  */
#line 114 "parser.tab.cc"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
#line 50 "parser.yy"

int integer;
double decimal;
char *string;



/* Line 214 of yacc.c  */
#line 273 "parser.tab.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 285 "parser.tab.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   738

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  116
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  70
/* YYNRULES -- Number of rules.  */
#define YYNRULES  210
/* YYNRULES -- Number of states.  */
#define YYNSTATES  687

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   370

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,    11,    14,    15,    21,    28,    39,    50,
      61,    72,    79,    88,    97,   106,   109,   112,   115,   118,
     121,   124,   127,   128,   131,   139,   147,   155,   163,   171,
     179,   187,   195,   203,   211,   212,   214,   218,   222,   226,
     230,   234,   238,   239,   245,   251,   257,   264,   271,   278,
     279,   285,   291,   297,   303,   309,   315,   316,   322,   328,
     334,   340,   341,   347,   353,   359,   365,   366,   372,   378,
     384,   390,   399,   400,   406,   409,   415,   418,   421,   424,
     430,   436,   439,   442,   445,   448,   451,   454,   457,   458,
     464,   470,   475,   480,   485,   490,   495,   500,   508,   513,
     520,   530,   540,   547,   557,   562,   563,   566,   569,   577,
     578,   584,   590,   596,   597,   603,   609,   615,   618,   621,
     624,   627,   630,   636,   642,   648,   654,   660,   669,   678,
     684,   690,   698,   706,   712,   718,   724,   730,   736,   742,
     748,   751,   754,   755,   761,   762,   768,   773,   778,   783,
     788,   793,   798,   803,   808,   813,   818,   823,   824,   827,
     833,   836,   839,   842,   845,   846,   849,   852,   855,   858,
     861,   864,   867,   870,   873,   876,   879,   884,   889,   894,
     902,   910,   918,   923,   928,   933,   938,   946,   954,   962,
     970,   978,   986,   987,   994,  1001,  1008,  1015,  1017,  1019,
    1021,  1025,  1027,  1031,  1033,  1035,  1037,  1039,  1041,  1044,
    1046
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     117,     0,    -1,    14,     5,   183,     6,     3,   118,     4,
      -1,   119,   120,    -1,    -1,   119,   183,     7,   183,     8,
      -1,   119,   101,     5,   183,     6,     8,    -1,   119,   102,
       5,   183,     9,   183,     9,   183,     6,     8,    -1,   119,
     102,     5,   183,     9,    27,     9,   183,     6,     8,    -1,
     119,   102,     5,   183,     9,    31,     9,   183,     6,     8,
      -1,   119,   102,     5,   106,     9,    31,     9,   183,     6,
       8,    -1,   119,    25,     5,   183,     6,     8,    -1,   119,
      25,     5,   183,     9,   183,     6,     8,    -1,   119,    26,
       5,   183,     9,   183,     6,     8,    -1,   119,    93,     5,
     183,     9,   183,     6,     8,    -1,   119,   121,    -1,   119,
     122,    -1,   119,   123,    -1,   119,   124,    -1,   119,   128,
      -1,   119,   126,    -1,   119,   127,    -1,    -1,   120,   129,
      -1,    27,     5,   183,     6,     3,   130,     4,    -1,   183,
       5,   183,     6,     3,   132,     4,    -1,    28,     5,   183,
       6,     3,   134,     4,    -1,    29,     5,   183,     6,     3,
     135,     4,    -1,    30,     5,   183,     6,     3,   136,     4,
      -1,    94,     5,   183,     6,     3,   138,     4,    -1,   104,
       5,   183,     6,     3,   133,     4,    -1,   105,     5,   183,
       6,     3,   133,     4,    -1,   103,     5,   183,     6,     3,
     133,     4,    -1,    31,     5,   183,     6,     3,   137,     4,
      -1,    -1,   131,    -1,   131,     8,   130,    -1,    47,     7,
     185,    -1,    48,     7,   185,    -1,    49,     7,   185,    -1,
      50,     7,   183,    -1,   183,     7,   183,    -1,    -1,   132,
      51,     7,   183,     8,    -1,   132,    52,     7,   183,     8,
      -1,   132,    53,     7,   183,     8,    -1,   132,    10,     5,
     180,     6,     8,    -1,   132,    11,     5,   180,     6,     8,
      -1,   132,    12,     5,   180,     6,     8,    -1,    -1,   133,
      51,     7,   183,     8,    -1,   133,    52,     7,   183,     8,
      -1,   133,    53,     7,   183,     8,    -1,   133,    10,     5,
     180,     6,    -1,   133,    11,     5,   180,     6,    -1,   133,
      12,     5,   180,     6,    -1,    -1,   134,    54,     7,   184,
       8,    -1,   134,    55,     7,   184,     8,    -1,   134,    56,
       7,   184,     8,    -1,   134,    57,     7,   184,     8,    -1,
      -1,   135,    58,     7,   184,     8,    -1,   135,    59,     7,
     184,     8,    -1,   135,    60,     7,   184,     8,    -1,   135,
      61,     7,   184,     8,    -1,    -1,   136,    62,     7,   185,
       8,    -1,   136,    20,     7,   185,     8,    -1,   136,    17,
       7,   185,     8,    -1,   136,    63,     7,   185,     8,    -1,
     136,    64,     5,   185,     9,   185,     6,     8,    -1,    -1,
     137,    16,     7,   183,     8,    -1,   137,   139,    -1,   137,
      84,     7,   183,     8,    -1,   137,   140,    -1,   137,   141,
      -1,   137,   125,    -1,   137,   106,     7,   185,     8,    -1,
     137,    17,     7,   185,     8,    -1,   137,   142,    -1,   137,
     143,    -1,   137,   144,    -1,   137,   145,    -1,   137,   146,
      -1,   137,   147,    -1,   137,   148,    -1,    -1,   138,    95,
       7,   183,     8,    -1,   138,    96,     7,   183,     8,    -1,
     111,     7,    70,     8,    -1,   111,     7,    71,     8,    -1,
      78,     7,    70,     8,    -1,    78,     7,    71,     8,    -1,
      79,     7,    70,     8,    -1,    79,     7,    71,     8,    -1,
      13,     5,   183,     6,     3,   153,     4,    -1,    15,     7,
     185,     8,    -1,    18,     5,     6,     3,   154,     4,    -1,
      75,     5,   183,     9,   183,     6,     3,   155,     4,    -1,
      76,     5,   183,     9,   183,     6,     3,   156,     4,    -1,
      83,     5,     6,     3,   150,     4,    -1,    89,     5,   183,
       9,   183,     6,     3,   149,     4,    -1,    91,     7,   183,
       8,    -1,    -1,   150,   151,    -1,   150,   145,    -1,    13,
       5,   183,     6,     3,   152,     4,    -1,    -1,   152,    23,
       7,   183,     8,    -1,   152,    19,     7,   183,     8,    -1,
     152,    82,     7,   183,     8,    -1,    -1,   153,    23,     7,
     183,     8,    -1,   153,    92,     7,   183,     8,    -1,   153,
      90,     7,   183,     8,    -1,   153,   158,    -1,   153,   159,
      -1,   153,   160,    -1,   153,   161,    -1,   153,   162,    -1,
     153,    80,     7,   183,     8,    -1,   153,    77,     7,   183,
       8,    -1,   153,    72,     7,   185,     8,    -1,   153,   108,
       7,   185,     8,    -1,   153,   107,     7,   185,     8,    -1,
     153,   110,     5,   185,     9,   185,     6,     8,    -1,   153,
     109,     5,   185,     9,   185,     6,     8,    -1,   153,    20,
       7,   185,     8,    -1,   153,    19,     7,   183,     8,    -1,
     153,    32,     5,     6,     3,   163,     4,    -1,   153,    33,
       5,     6,     3,   164,     4,    -1,   153,    97,     7,   183,
       8,    -1,   153,    98,     7,   183,     8,    -1,   153,    99,
       7,   183,     8,    -1,   153,   100,     7,   183,     8,    -1,
     153,    21,     7,   185,     8,    -1,   153,    73,     7,   185,
       8,    -1,   153,    74,     7,   185,     8,    -1,   167,   157,
      -1,   157,   167,    -1,    -1,   155,   183,     7,   183,     8,
      -1,    -1,   156,   183,     7,   183,     8,    -1,    24,     7,
     185,     8,    -1,    85,     7,    70,     8,    -1,    85,     7,
      71,     8,    -1,    86,     7,    70,     8,    -1,    86,     7,
      71,     8,    -1,    87,     7,    70,     8,    -1,    87,     7,
      71,     8,    -1,    88,     7,    70,     8,    -1,    88,     7,
      71,     8,    -1,    69,     7,    70,     8,    -1,    69,     7,
      71,     8,    -1,    -1,   163,   165,    -1,   163,    65,     7,
     183,     8,    -1,   163,   167,    -1,   163,   168,    -1,   163,
     169,    -1,   163,   170,    -1,    -1,   164,   165,    -1,   164,
     166,    -1,   164,   171,    -1,   164,   167,    -1,   164,   172,
      -1,   164,   173,    -1,   164,   174,    -1,   164,   175,    -1,
     164,   176,    -1,   164,   177,    -1,   164,   178,    -1,    46,
       7,   183,     8,    -1,    66,     7,   183,     8,    -1,    22,
       7,   183,     8,    -1,    35,     5,   183,     6,     3,   179,
       4,    -1,    34,     5,   183,     6,     3,   179,     4,    -1,
      81,     5,   183,     6,     3,   179,     4,    -1,    36,     7,
      37,     8,    -1,    36,     7,    38,     8,    -1,    36,     7,
      39,     8,    -1,    44,     7,   183,     8,    -1,    40,     5,
     183,     6,     3,   179,     4,    -1,    42,     5,   183,     6,
       3,   179,     4,    -1,    41,     5,   183,     6,     3,   179,
       4,    -1,    43,     5,   183,     6,     3,   179,     4,    -1,
      67,     5,   183,     6,     3,   179,     4,    -1,    68,     5,
     183,     6,     3,   179,     4,    -1,    -1,   179,    10,     5,
     180,     6,     8,    -1,   179,    11,     5,   180,     6,     8,
      -1,   179,    12,     5,   180,     6,     8,    -1,   179,    45,
       5,   180,     6,     8,    -1,   181,    -1,   182,    -1,   115,
      -1,   181,     9,   115,    -1,   185,    -1,   182,     9,   185,
      -1,   114,    -1,   115,    -1,   112,    -1,   113,    -1,   183,
      -1,   184,   183,    -1,   112,    -1,   113,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   178,   178,   183,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   208,   211,   220,   223,   226,   227,   230,   233,
     236,   239,   242,   247,   256,   257,   258,   261,   262,   263,
     264,   265,   268,   269,   270,   271,   272,   273,   274,   277,
     278,   279,   280,   281,   282,   283,   287,   288,   289,   290,
     291,   294,   295,   296,   297,   298,   301,   302,   303,   304,
     305,   306,   310,   311,   316,   317,   318,   319,   320,   321,
     322,   326,   331,   332,   333,   334,   335,   336,   339,   340,
     341,   344,   345,   349,   350,   353,   354,   357,   364,   367,
     370,   373,   376,   379,   382,   385,   386,   387,   390,   393,
     394,   395,   396,   400,   403,   408,   409,   410,   411,   412,
     413,   414,   415,   416,   417,   418,   419,   420,   421,   422,
     426,   427,   428,   432,   433,   434,   435,   436,   437,   438,
     441,   442,   445,   446,   449,   450,   453,   456,   457,   460,
     461,   464,   465,   468,   469,   472,   473,   476,   477,   478,
     479,   480,   481,   482,   486,   489,   490,   491,   492,   493,
     494,   498,   502,   506,   510,   511,   514,   517,   520,   523,
     526,   529,   532,   533,   534,   537,   540,   543,   546,   549,
     552,   555,   560,   563,   567,   571,   575,   582,   583,   586,
     591,   603,   608,   614,   619,   624,   628,   635,   638,   643,
     647
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LBRACE", "RBRACE", "LPAREN", "RPAREN",
  "COLON", "SEMICOLON", "COMMA", "INDEX_1", "INDEX_2", "INDEX_3", "PIN",
  "LIBRARY", "CELL_LEAKAGE_POWER", "CELL_FOOTPRINT", "AREA",
  "LEAKAGE_POWER", "FUNCTION", "CAPACITANCE", "MAX_CAPACITANCE", "WHEN",
  "DIRECTION", "VALUE", "LIBRARY_FEATURES", "CAPACITIVE_LOAD_UNIT",
  "OPERATING_CONDITIONS", "OUTPUT_VOLTAGE", "INPUT_VOLTAGE", "WIRE_LOAD",
  "CELL", "INTERNAL_POWER", "TIMING", "RISE_POWER", "FALL_POWER",
  "TIMING_SENSE", "POSITIVE_UNATE", "NEGATIVE_UNATE", "NON_UNATE",
  "CELL_RISE", "CELL_FALL", "RISE_TRANSITION", "FALL_TRANSITION",
  "SDF_COND", "VALUES", "RELATED_PIN", "PROCESS", "TEMPERATURE", "VOLTAGE",
  "TREE_TYPE", "VARIABLE_1", "VARIABLE_2", "VARIABLE_3", "VOL", "VOH",
  "VOMIN", "VOMAX", "VIL", "VIH", "VIMIN", "VIMAX", "RESISTANCE", "SLOPE",
  "FANOUT_LENGTH", "EQUAL_OR_OPPOSITE_OUTPUT", "TIMING_TYPE",
  "RISE_CONSTRAINT", "FALL_CONSTRAINT", "CLOCK", "TRUE", "FALSE",
  "MAX_TRANSITION", "MIN_PULSE_WIDTH_HIGH", "MIN_PULSE_WIDTH_LOW", "FF",
  "LATCH", "DRIVER_TYPE", "DONT_TOUCH", "DONT_USE", "THREE_STATE", "POWER",
  "SIGNAL_TYPE", "TEST_CELL", "CLOCK_GATING_INTEGRATED_CELL",
  "CLOCK_GATE_ENABLE_PIN", "CLOCK_GATE_CLOCK_PIN", "CLOCK_GATE_OUT_PIN",
  "CLOCK_GATE_TEST_PIN", "STATETABLE", "STATE_FUNCTION", "TABLE",
  "INTERNAL_NODE", "VOLTAGE_MAP", "PG_PIN", "VOLTAGE_NAME", "PG_TYPE",
  "RELATED_POWER_PIN", "RELATED_GROUND_PIN", "NEXTSTATE_TYPE",
  "MIN_PERIOD", "TECHNOLOGY", "DEFINE", "OUTPUT_CURRENT_TEMPLATE",
  "POWER_LUT_TEMPLATE", "LU_TABLE_TEMPLATE", "DRIVE_STRENGTH",
  "FALL_CAPACITANCE", "RISE_CAPACITANCE", "FALL_CAPACITANCE_RANGE",
  "RISE_CAPACITANCE_RANGE", "IS_FILLER_CELL", "INTEGER", "DECIMAL",
  "STRING", "QUOTED_STRING", "$accept", "library", "lib_data", "header",
  "cells", "operating_conditions", "table_template", "pad_attribute",
  "wire_load", "pg_pin", "power_lut_template", "lu_table_template",
  "output_current_template", "cell", "operating_conditions_dataset",
  "operating_conditions_data", "table_data", "template_data",
  "output_voltage_data", "input_voltage_data", "wire_load_data",
  "cell_data", "pg_pin_data", "is_filler_cell", "dont_touch", "dont_use",
  "pin", "cell_leakage_power", "leakage_power", "ff", "latch", "test_cell",
  "statetable", "statetable_data", "test_cell_data", "test_pin",
  "test_pin_data", "pin_data", "leakage_power_data", "ff_data",
  "latch_data", "leakage_power_data_value", "clock_gate_enable_pin",
  "clock_gate_clock_pin", "clock_gate_out_pin", "clock_gate_test_pin",
  "clock", "internal_power_data", "timing_data", "related_pin",
  "timing_type", "when", "fall_power", "rise_power", "power",
  "timing_sense", "sdf_cond", "cell_rise", "rise_transition", "cell_fall",
  "fall_transition", "rise_constraint", "fall_constraint", "table",
  "multiple_values", "multiple_quoted_strings", "values_comma", "string",
  "strings", "value", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   116,   117,   118,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   120,   120,   121,   122,   123,   123,   124,   125,
     126,   127,   128,   129,   130,   130,   130,   131,   131,   131,
     131,   131,   132,   132,   132,   132,   132,   132,   132,   133,
     133,   133,   133,   133,   133,   133,   134,   134,   134,   134,
     134,   135,   135,   135,   135,   135,   136,   136,   136,   136,
     136,   136,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   138,   138,
     138,   139,   139,   140,   140,   141,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   150,   150,   151,   152,
     152,   152,   152,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     154,   154,   155,   155,   156,   156,   157,   158,   158,   159,
     159,   160,   160,   161,   161,   162,   162,   163,   163,   163,
     163,   163,   163,   163,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   165,   166,   167,   168,
     169,   170,   171,   171,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   179,   179,   179,   179,   180,   180,   181,
     181,   182,   182,   183,   183,   183,   183,   184,   184,   185,
     185
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     7,     2,     0,     5,     6,    10,    10,    10,
      10,     6,     8,     8,     8,     2,     2,     2,     2,     2,
       2,     2,     0,     2,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     0,     1,     3,     3,     3,     3,
       3,     3,     0,     5,     5,     5,     6,     6,     6,     0,
       5,     5,     5,     5,     5,     5,     0,     5,     5,     5,
       5,     0,     5,     5,     5,     5,     0,     5,     5,     5,
       5,     8,     0,     5,     2,     5,     2,     2,     2,     5,
       5,     2,     2,     2,     2,     2,     2,     2,     0,     5,
       5,     4,     4,     4,     4,     4,     4,     7,     4,     6,
       9,     9,     6,     9,     4,     0,     2,     2,     7,     0,
       5,     5,     5,     0,     5,     5,     5,     2,     2,     2,
       2,     2,     5,     5,     5,     5,     5,     8,     8,     5,
       5,     7,     7,     5,     5,     5,     5,     5,     5,     5,
       2,     2,     0,     5,     0,     5,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     0,     2,     5,
       2,     2,     2,     2,     0,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     4,     4,     4,     7,
       7,     7,     4,     4,     4,     4,     7,     7,     7,     7,
       7,     7,     0,     6,     6,     6,     6,     1,     1,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     2,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     1,   205,   206,   203,   204,     0,
       0,     4,     0,    22,     2,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     3,    15,    16,
      17,    18,    20,    21,    19,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     5,    11,     0,     0,    34,    56,
      61,    66,     0,     6,     0,     0,     0,     0,    49,    49,
      49,     0,    42,     0,     0,     0,     0,     0,     0,     0,
      35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    72,     0,    12,    13,     0,     0,     0,
       0,    24,    34,     0,    26,     0,     0,     0,     0,    27,
       0,     0,     0,     0,    28,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,    32,     0,     0,     0,     0,
       0,     0,    30,    31,     0,    25,     0,     0,     0,     0,
       0,     0,   209,   210,    37,    38,    39,    40,    36,    41,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    78,
      74,    76,    77,    81,    82,    83,    84,    85,    86,    87,
       0,     0,     0,     0,     0,     0,   207,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      10,     8,     9,     7,   199,     0,   197,   198,   201,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    57,   208,    58,    59,    60,
      62,    63,    64,    65,    69,    68,    67,    70,     0,    53,
       0,     0,    54,    55,    50,    51,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    43,    44,
      45,     0,   200,   202,     0,    98,    73,    80,     0,     0,
       0,    93,    94,    95,    96,   105,    75,     0,     0,    79,
      91,    92,    46,    47,    48,     0,   113,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    88,    71,     0,     0,
       0,    99,   141,   140,     0,     0,   102,     0,   107,   106,
       0,     0,    97,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   117,
     118,   119,   120,   121,     0,     0,   142,   144,     0,     0,
      29,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   178,
     146,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   100,
       0,   101,     0,     0,     0,   103,     0,     0,   130,   129,
     137,   114,   157,   164,   155,   156,   124,   138,   139,   123,
     122,   147,   148,   149,   150,   151,   152,   153,   154,   116,
     115,   133,   134,   135,   136,   126,   125,     0,     0,     0,
       0,   109,     0,    89,    90,     0,     0,     0,     0,     0,
       0,     0,   104,   131,     0,     0,     0,     0,     0,   158,
     160,   161,   162,   163,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   165,   166,   168,   167,   169,   170,
     171,   172,   173,   174,   175,     0,     0,   143,   145,   108,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   128,   127,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,   159,     0,   182,   183,   184,     0,
       0,     0,     0,   185,   177,     0,     0,   111,   110,   112,
     192,   192,   192,   192,   192,   192,   192,   192,   192,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   180,     0,
       0,     0,     0,   179,   181,   186,   188,   187,   189,   190,
     191,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   193,   194,   195,   196
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,    12,    13,    27,    28,    29,    30,    31,   219,
      32,    33,    34,    49,   109,   110,   124,   120,   112,   113,
     114,   164,   381,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   455,   364,   379,   541,   368,   359,   451,
     452,   360,   409,   410,   411,   412,   413,   535,   536,   549,
     565,   361,   551,   552,   553,   567,   568,   569,   570,   571,
     572,   573,   574,   649,   255,   256,   257,   236,   237,   258
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -360
static const yytype_int16 yypact[] =
{
     -10,     6,    64,   -11,  -360,  -360,  -360,  -360,  -360,    12,
      67,  -360,    45,   249,  -360,    69,    82,   130,   144,   175,
     177,   187,   210,   228,   240,   244,   252,    74,  -360,  -360,
    -360,  -360,  -360,  -360,  -360,    16,   -11,   -11,   -11,   -11,
     -11,   -11,   -11,   -11,   286,   -11,   -11,   -11,   253,  -360,
     -11,   -11,     8,   219,   151,   254,   256,   259,   250,   263,
     261,   271,   276,   280,   281,   -11,   282,   277,   285,   -11,
     -11,   291,   296,   298,   299,   -11,   304,   283,   -15,   313,
     314,   315,   301,   322,  -360,  -360,   323,   328,    33,  -360,
    -360,  -360,   329,  -360,   330,   331,   332,   334,  -360,  -360,
    -360,   335,  -360,   340,   350,   352,   353,   358,   360,   364,
     363,   375,     3,   148,   264,   379,   -11,   -11,   -11,   -11,
     132,   200,   294,  -360,   365,  -360,  -360,   -87,   -87,   -87,
     -11,  -360,    33,   -11,  -360,   376,   382,   384,   386,  -360,
     396,   397,   398,   404,  -360,   406,   407,   415,   418,   367,
    -360,   420,   429,   436,   437,  -360,   419,   441,   442,   438,
     448,   449,  -360,  -360,    75,  -360,   454,   473,   474,   451,
     475,   476,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,
     -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,   -87,   -87,
     -87,   -87,   -87,   472,   478,   479,   480,  -107,  -107,  -107,
     -11,   -11,   -11,  -360,   484,   477,   483,   489,   492,   494,
     495,   503,   504,   507,   506,   509,   510,   511,   512,  -360,
    -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,
    -107,  -107,  -107,   -11,   -11,   -11,  -360,    -6,    19,    48,
      53,    60,    76,    81,    88,   508,   513,   514,   515,   452,
    -360,  -360,  -360,  -360,  -360,   518,   516,   519,  -360,   523,
     524,   525,   526,   527,   -11,   -87,   -11,   -87,   530,   -11,
     -11,   -39,   -25,   531,   -11,   -11,   -11,   -87,   -16,   532,
     533,   534,   535,   536,   537,  -360,  -360,  -360,  -360,  -360,
    -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,   -87,  -360,
     402,   -87,  -360,  -360,  -360,  -360,  -360,   540,   539,   541,
     542,   517,   522,   543,   545,   546,   547,   548,   529,   549,
     550,   552,   553,   554,   555,   556,   557,   558,  -360,  -360,
    -360,   561,  -360,  -360,   538,  -360,  -360,  -360,    49,   -11,
     -11,  -360,  -360,  -360,  -360,  -360,  -360,   -11,   565,  -360,
    -360,  -360,  -360,  -360,  -360,   562,  -360,   544,   564,   568,
     459,   551,   563,   567,    11,   570,  -360,  -360,   223,   -11,
     -87,  -360,  -360,  -360,   571,   574,  -360,   573,  -360,  -360,
     576,    -1,  -360,   578,   587,   588,   592,   595,   596,   597,
     598,   599,   600,   601,   602,   603,   604,   605,   606,   607,
     608,   609,   610,   611,   612,   613,   614,   617,   618,  -360,
    -360,  -360,  -360,  -360,   594,   616,  -360,  -360,   -11,   457,
    -360,   619,   620,   -11,   -87,   -87,   -11,   622,   623,    55,
     -87,   -87,   -87,   -11,   -11,    85,   100,   108,   152,   -11,
     -11,   -11,   -11,   -11,   -11,   -87,   -87,   -87,   -87,  -360,
    -360,     5,     9,   624,   625,   621,   -11,   -11,   626,   627,
     628,   629,   630,   635,   631,   632,   633,   634,   636,   637,
     638,   639,   640,   641,   642,   643,   644,   645,   646,   647,
     648,   649,   650,   651,   652,   653,   654,   655,   656,  -360,
     659,  -360,   660,   665,   -11,  -360,   661,   662,  -360,  -360,
    -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,
    -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,
    -360,  -360,  -360,  -360,  -360,  -360,  -360,   -87,   -87,   -11,
     -11,  -360,   663,  -360,  -360,   183,   366,   657,   666,   667,
     668,    46,  -360,  -360,   669,   672,   671,   673,   674,  -360,
    -360,  -360,  -360,  -360,  -360,   675,   676,   678,   679,   680,
     681,   682,   685,   686,  -360,  -360,  -360,  -360,  -360,  -360,
    -360,  -360,  -360,  -360,  -360,   684,   687,  -360,  -360,  -360,
     689,   690,   691,   -11,   -11,   -11,   -11,   -11,    73,   -11,
     -11,   -11,   -11,   -11,   -11,   -11,   -11,  -360,  -360,   -11,
     -11,   -11,   688,   693,   692,   694,   695,   696,   697,   698,
     701,   702,   703,   704,   705,   706,   709,   710,   711,   712,
     713,   670,   683,  -360,  -360,   700,  -360,  -360,  -360,   708,
     714,   715,   719,  -360,  -360,   720,   721,  -360,  -360,  -360,
    -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,    18,
     209,   279,   345,   374,   417,   440,   453,   456,  -360,   707,
     722,   723,   724,  -360,  -360,  -360,  -360,  -360,  -360,  -360,
    -360,  -107,  -107,  -107,  -107,   725,   726,   727,   728,   717,
     718,   729,   730,  -360,  -360,  -360,  -360
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,
    -360,  -360,  -360,  -360,   410,  -360,  -360,   125,  -360,  -360,
    -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,   196,
    -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,
    -360,   242,  -360,  -360,  -360,  -360,  -360,  -360,  -360,    95,
    -360,  -359,  -360,  -360,  -360,  -360,  -360,  -360,  -360,  -360,
    -360,  -360,  -360,  -139,  -179,  -360,  -360,    -3,   288,   -51
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
       9,   372,   285,   420,     1,   172,   173,   134,   254,   489,
      35,     3,    95,   491,    68,   376,    96,    69,    10,   259,
     260,    50,   658,    51,   377,   172,   173,   287,   659,   660,
     661,   314,   315,    52,    53,    54,    55,    56,    57,    58,
      59,    61,    62,    63,    64,   316,   317,    66,    67,    14,
     579,   279,   280,   281,   323,   324,   288,   135,   136,   137,
     138,   289,    82,   662,     4,   580,    86,    87,   290,   581,
      11,   357,    92,   358,    36,    97,   174,   175,   176,   203,
     105,   106,   107,   108,   291,   111,   209,    37,   204,   292,
     205,   206,   207,   208,   421,   422,   293,     5,     6,     7,
       8,     5,     6,     7,     8,    48,     5,     6,     7,     8,
     607,   608,   609,   151,   152,   153,   154,     5,     6,     7,
       8,     5,     6,     7,     8,   464,   465,   177,   582,   111,
     179,     5,     6,     7,     8,    38,   155,   245,   246,   247,
     248,   249,   156,   157,   158,     5,     6,     7,     8,    39,
     209,   210,   139,   211,   212,   471,   472,    71,   213,   214,
       5,     6,     7,     8,   215,     5,     6,     7,     8,   216,
     473,   474,     5,     6,     7,     8,   550,   566,   475,   476,
      40,   217,    41,   159,   160,   161,   218,   543,     5,     6,
       7,     8,    42,     5,     6,     7,     8,   261,   262,   263,
       5,     6,     7,     8,   162,   357,   140,   141,   142,   143,
     156,   157,   158,   663,   308,    43,   310,   544,   545,   659,
     660,   661,   477,   478,   121,   122,   322,   382,    70,   546,
     282,   283,   284,    44,   286,   286,   286,   286,   286,   286,
     286,   286,   383,   384,   385,    45,   386,   331,   547,    46,
     333,   159,   160,   161,   662,   387,   388,    47,    65,    75,
      72,   307,    73,   309,   548,    74,   312,   313,   144,    76,
      77,   319,   320,   321,    15,    16,    17,    18,    19,    20,
      78,   145,    79,   664,   146,    84,    80,    81,    83,   659,
     660,   661,   389,    85,    88,   390,   391,   392,   163,    89,
     393,    90,    91,   394,   156,   157,   158,   101,   395,   396,
     397,   398,    93,   399,    94,   400,    98,    99,   100,   415,
     401,   402,   403,   404,   662,   102,   147,   148,   149,   103,
     405,   406,   407,   408,   104,   115,   362,   363,   123,   116,
     117,   118,    21,   119,   365,   159,   160,   161,   125,   665,
      22,    23,    24,    25,    26,   659,   660,   661,   126,   127,
     128,     5,     6,     7,     8,   129,   414,   130,   131,   165,
     554,   132,   192,   459,   460,   166,   167,   168,   666,   466,
     467,   468,   133,   180,   659,   660,   661,   150,   357,   181,
     662,   182,    60,   183,   485,   486,   487,   488,     5,     6,
       7,     8,   555,   184,   185,   186,   556,   557,   558,   559,
     560,   187,   546,   188,   189,   453,   169,   170,   171,   662,
     458,   667,   190,   461,   197,   191,   193,   659,   660,   661,
     469,   470,   561,   562,   563,   194,   479,   480,   481,   482,
     483,   484,   195,   196,   668,   200,   198,   199,   490,   492,
     659,   660,   661,   496,   497,   201,   202,   669,   233,   230,
     670,   298,   662,   659,   660,   661,   659,   660,   661,   238,
     239,   240,   241,   242,   243,   244,   537,   538,   231,   232,
     250,   357,   234,   235,   265,   662,   251,   252,   253,   264,
     266,   532,   675,   676,   677,   678,   267,   268,   662,   269,
     270,   662,   650,   651,   652,   653,   654,   655,   656,   657,
     271,   272,   273,   274,   275,   276,   294,   332,   277,   278,
     338,   295,   296,   297,   299,   300,   539,   540,   301,   302,
     303,   339,   345,   304,   305,   306,   311,   318,   325,   326,
     327,   356,   178,   328,   329,   330,   334,   335,   454,   336,
     337,   369,   340,   341,   342,   343,   344,   346,   348,   347,
     378,   349,   350,   351,   352,   353,   354,   355,   366,   374,
     367,   370,   371,   375,   416,   358,   380,   417,   418,   419,
     602,   603,   604,   605,   606,   423,   610,   611,   612,   613,
     614,   615,   616,   617,   424,   425,   618,   619,   620,   426,
     427,   428,   449,   373,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   450,   495,   456,   457,   462,   463,
     493,   564,   494,   502,   498,   499,   500,   501,   503,   504,
     505,   506,   507,     0,   508,   509,   510,   511,   512,   513,
     514,   515,   516,   517,   518,   519,   520,   521,   522,   523,
     524,   525,   526,   575,   527,   528,   529,   530,   531,   533,
     534,   542,   576,   640,   583,   577,   578,   584,   585,   587,
     586,   589,   588,   590,   591,   592,   641,     0,   593,   594,
     595,   596,   597,     0,   621,   598,   599,   600,   601,   622,
     623,   625,   624,   642,   626,   627,   628,   629,   630,   631,
     632,   643,   671,   633,   634,   635,   636,   644,   645,   637,
     638,   639,   646,   647,   648,   683,   684,   672,   673,   674,
       0,   679,   680,   681,   682,     0,     0,   685,   686
};

static const yytype_int16 yycheck[] =
{
       3,   360,     8,     4,    14,   112,   113,     4,   115,     4,
      13,     5,    27,     4,     6,     4,    31,     9,     6,   198,
     199,     5,     4,     7,    13,   112,   113,     8,    10,    11,
      12,    70,    71,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    70,    71,    50,    51,     4,
       4,   230,   231,   232,    70,    71,     8,    54,    55,    56,
      57,     8,    65,    45,     0,    19,    69,    70,     8,    23,
       3,    22,    75,    24,     5,    78,   127,   128,   129,     4,
      47,    48,    49,    50,     8,    88,    75,     5,    13,     8,
      15,    16,    17,    18,    95,    96,     8,   112,   113,   114,
     115,   112,   113,   114,   115,    31,   112,   113,   114,   115,
      37,    38,    39,   116,   117,   118,   119,   112,   113,   114,
     115,   112,   113,   114,   115,    70,    71,   130,    82,   132,
     133,   112,   113,   114,   115,     5,     4,   188,   189,   190,
     191,   192,    10,    11,    12,   112,   113,   114,   115,     5,
      75,    76,     4,    78,    79,    70,    71,     6,    83,    84,
     112,   113,   114,   115,    89,   112,   113,   114,   115,    94,
      70,    71,   112,   113,   114,   115,   535,   536,    70,    71,
       5,   106,     5,    51,    52,    53,   111,     4,   112,   113,
     114,   115,     5,   112,   113,   114,   115,   200,   201,   202,
     112,   113,   114,   115,     4,    22,    58,    59,    60,    61,
      10,    11,    12,     4,   265,     5,   267,    34,    35,    10,
      11,    12,    70,    71,    99,   100,   277,     4,     9,    46,
     233,   234,   235,     5,   237,   238,   239,   240,   241,   242,
     243,   244,    19,    20,    21,     5,    23,   298,    65,     5,
     301,    51,    52,    53,    45,    32,    33,     5,     5,     9,
       6,   264,     6,   266,    81,     6,   269,   270,     4,     6,
       9,   274,   275,   276,    25,    26,    27,    28,    29,    30,
       9,    17,     6,     4,    20,     8,     6,     6,     6,    10,
      11,    12,    69,     8,     3,    72,    73,    74,     4,     3,
      77,     3,     3,    80,    10,    11,    12,     6,    85,    86,
      87,    88,     8,    90,    31,    92,     3,     3,     3,   370,
      97,    98,    99,   100,    45,     3,    62,    63,    64,     6,
     107,   108,   109,   110,     6,     6,   339,   340,     3,     9,
       9,     9,    93,     9,   347,    51,    52,    53,     8,     4,
     101,   102,   103,   104,   105,    10,    11,    12,     8,     7,
       7,   112,   113,   114,   115,     7,   369,     7,     4,     4,
       4,     8,     5,   424,   425,    10,    11,    12,     4,   430,
     431,   432,     7,     7,    10,    11,    12,     8,    22,     7,
      45,     7,   106,     7,   445,   446,   447,   448,   112,   113,
     114,   115,    36,     7,     7,     7,    40,    41,    42,    43,
      44,     7,    46,     7,     7,   418,    51,    52,    53,    45,
     423,     4,     7,   426,     5,     7,     6,    10,    11,    12,
     433,   434,    66,    67,    68,     6,   439,   440,   441,   442,
     443,   444,     6,     6,     4,     7,     5,     5,   451,   452,
      10,    11,    12,   456,   457,     7,     7,     4,     7,     5,
       4,     9,    45,    10,    11,    12,    10,    11,    12,   181,
     182,   183,   184,   185,   186,   187,   527,   528,     5,     5,
       8,    22,     7,     7,     7,    45,     8,     8,     8,     5,
       7,   494,   671,   672,   673,   674,     7,     5,    45,     5,
       5,    45,   641,   642,   643,   644,   645,   646,   647,   648,
       7,     7,     5,     7,     5,     5,     8,   115,     7,     7,
       3,     8,     8,     8,     6,     9,   529,   530,     9,     6,
       6,     9,     3,     8,     8,     8,     6,     6,     6,     6,
       6,     3,   132,     8,     8,     8,     6,     8,    91,     8,
       8,     7,     9,     8,     8,     8,     8,     8,     6,     9,
     364,     8,     8,     8,     8,     8,     8,     6,     3,     6,
       8,     7,     4,     6,     3,    24,     6,     3,     5,     3,
     583,   584,   585,   586,   587,     7,   589,   590,   591,   592,
     593,   594,   595,   596,     7,     7,   599,   600,   601,     7,
       5,     5,     8,   361,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     5,     5,     8,     4,     7,     7,     6,     6,
       6,   536,     7,     3,     8,     8,     8,     8,     3,     8,
       8,     8,     8,    -1,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     6,     9,     9,     7,     7,     3,     8,
       8,     8,     6,     3,     5,     8,     8,     5,     7,     5,
       7,     5,     7,     5,     5,     5,     3,    -1,     7,     7,
       5,     5,     8,    -1,     6,     8,     7,     7,     7,     6,
       8,     6,     8,     3,     8,     8,     8,     6,     6,     6,
       6,     3,     5,     8,     8,     6,     6,     3,     3,     8,
       8,     8,     3,     3,     3,     8,     8,     5,     5,     5,
      -1,     6,     6,     6,     6,    -1,    -1,     8,     8
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,   117,     5,     0,   112,   113,   114,   115,   183,
       6,     3,   118,   119,     4,    25,    26,    27,    28,    29,
      30,    93,   101,   102,   103,   104,   105,   120,   121,   122,
     123,   124,   126,   127,   128,   183,     5,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     5,     5,    31,   129,
       5,     7,   183,   183,   183,   183,   183,   183,   183,   183,
     106,   183,   183,   183,   183,     5,   183,   183,     6,     9,
       9,     6,     6,     6,     6,     9,     6,     9,     9,     6,
       6,     6,   183,     6,     8,     8,   183,   183,     3,     3,
       3,     3,   183,     8,    31,    27,    31,   183,     3,     3,
       3,     6,     3,     6,     6,    47,    48,    49,    50,   130,
     131,   183,   134,   135,   136,     6,     9,     9,     9,     9,
     133,   133,   133,     3,   132,     8,     8,     7,     7,     7,
       7,     4,     8,     7,     4,    54,    55,    56,    57,     4,
      58,    59,    60,    61,     4,    17,    20,    62,    63,    64,
       8,   183,   183,   183,   183,     4,    10,    11,    12,    51,
      52,    53,     4,     4,   137,     4,    10,    11,    12,    51,
      52,    53,   112,   113,   185,   185,   185,   183,   130,   183,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     5,     6,     6,     6,     6,     5,     5,     5,
       7,     7,     7,     4,    13,    15,    16,    17,    18,    75,
      76,    78,    79,    83,    84,    89,    94,   106,   111,   125,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
       5,     5,     5,     7,     7,     7,   183,   184,   184,   184,
     184,   184,   184,   184,   184,   185,   185,   185,   185,   185,
       8,     8,     8,     8,   115,   180,   181,   182,   185,   180,
     180,   183,   183,   183,     5,     7,     7,     7,     5,     5,
       5,     7,     7,     5,     7,     5,     5,     7,     7,   180,
     180,   180,   183,   183,   183,     8,   183,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     9,     6,
       9,     9,     6,     6,     8,     8,     8,   183,   185,   183,
     185,     6,   183,   183,    70,    71,    70,    71,     6,   183,
     183,   183,   185,    70,    71,     6,     6,     6,     8,     8,
       8,   185,   115,   185,     6,     8,     8,     8,     3,     9,
       9,     8,     8,     8,     8,     3,     8,     9,     6,     8,
       8,     8,     8,     8,     8,     6,     3,    22,    24,   154,
     157,   167,   183,   183,   150,   183,     3,     8,   153,     7,
       7,     4,   167,   157,     6,     6,     4,    13,   145,   151,
       6,   138,     4,    19,    20,    21,    23,    32,    33,    69,
      72,    73,    74,    77,    80,    85,    86,    87,    88,    90,
      92,    97,    98,    99,   100,   107,   108,   109,   110,   158,
     159,   160,   161,   162,   183,   185,     3,     3,     5,     3,
       4,    95,    96,     7,     7,     7,     7,     5,     5,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     5,     5,     8,
       8,   155,   156,   183,    91,   149,     7,     7,   183,   185,
     185,   183,     6,     6,    70,    71,   185,   185,   185,   183,
     183,    70,    71,    70,    71,    70,    71,    70,    71,   183,
     183,   183,   183,   183,   183,   185,   185,   185,   185,     4,
     183,     4,   183,     6,     7,     4,   183,   183,     8,     8,
       8,     8,     3,     3,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     9,     9,     7,
       7,     3,   183,     8,     8,   163,   164,   185,   185,   183,
     183,   152,     8,     4,    34,    35,    46,    65,    81,   165,
     167,   168,   169,   170,     4,    36,    40,    41,    42,    43,
      44,    66,    67,    68,   165,   166,   167,   171,   172,   173,
     174,   175,   176,   177,   178,     6,     6,     8,     8,     4,
      19,    23,    82,     5,     5,     7,     7,     5,     7,     5,
       5,     5,     5,     7,     7,     5,     5,     8,     8,     7,
       7,     7,   183,   183,   183,   183,   183,    37,    38,    39,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,     6,     6,     8,     8,     6,     8,     8,     8,     6,
       6,     6,     6,     8,     8,     6,     6,     8,     8,     8,
       3,     3,     3,     3,     3,     3,     3,     3,     3,   179,
     179,   179,   179,   179,   179,   179,   179,   179,     4,    10,
      11,    12,    45,     4,     4,     4,     4,     4,     4,     4,
       4,     5,     5,     5,     5,   180,   180,   180,   180,     6,
       6,     6,     6,     8,     8,     8,     8
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 179 "parser.yy"
    {
;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 208 "parser.yy"
    {
  __y_cell.clear();
;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 212 "parser.yy"
    {
  __y_library.addCell(__y_cell);
  __y_cell.clear();
;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 248 "parser.yy"
    {
  __y_cell.setName((yyvsp[(3) - (7)].string));
  free((yyvsp[(3) - (7)].string));
;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 312 "parser.yy"
    {
  __y_cell.setFootprint((yyvsp[(4) - (5)].string));
  free((yyvsp[(4) - (5)].string));
;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 323 "parser.yy"
    {
  __y_cell.setArea((yyvsp[(4) - (5)].decimal));
;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 327 "parser.yy"
    {
  __y_cell.addPin(__y_pin);
  __y_pin.clear();
;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 358 "parser.yy"
    {
  __y_pin.setName((yyvsp[(3) - (7)].string));
  free((yyvsp[(3) - (7)].string));
;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 400 "parser.yy"
    {
  __y_pin.clear();
;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 404 "parser.yy"
    {
  __y_pin.setDirection((yyvsp[(4) - (5)].string));
  free((yyvsp[(4) - (5)].string));
;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 423 "parser.yy"
    {
  __y_pin.setCapacitance((yyvsp[(4) - (5)].decimal));
;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 429 "parser.yy"
    {
  __y_pin.addTiming(__y_timing);
;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 486 "parser.yy"
    {
  __y_timing.clear();
;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 495 "parser.yy"
    {
  __y_timing.setCellRise(__y_table);
;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 499 "parser.yy"
    {
  __y_timing.setRiseTransition(__y_table);
;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 503 "parser.yy"
    {
  __y_timing.setCellFall(__y_table);
;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 507 "parser.yy"
    {
  __y_timing.setFallTransition(__y_table);
;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 560 "parser.yy"
    {
  __y_table.clear();
;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 564 "parser.yy"
    {
  __y_table.setIndex1(__y_values);
;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 568 "parser.yy"
    {
  __y_table.setIndex2(__y_values);
;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 572 "parser.yy"
    {
  __y_table.setIndex3(__y_values);
;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 576 "parser.yy"
    {
  __y_table.setValues(__y_values);
;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 587 "parser.yy"
    {
  __y_values = utils::splitMultipleValues((yyvsp[(1) - (1)].string));
  free((yyvsp[(1) - (1)].string));
;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 592 "parser.yy"
    {
  std::vector<double> tmp = utils::splitMultipleValues((yyvsp[(3) - (3)].string));
  free((yyvsp[(3) - (3)].string));
  std::vector<double> tmp2(tmp.size() + __y_values.size());
  copy(tmp.begin(), tmp.end(), 
    copy(__y_values.begin(), __y_values.end(), tmp2.begin()));
  __y_values = tmp2;
;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 604 "parser.yy"
    {
  __y_values.clear();
  __y_values.push_back((yyvsp[(1) - (1)].decimal));
;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 609 "parser.yy"
    {
  __y_values.push_back((yyvsp[(3) - (3)].decimal));
;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 615 "parser.yy"
    {
  (yyval.string) = strdup((yyvsp[(1) - (1)].string));
  free((yyvsp[(1) - (1)].string));
;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 620 "parser.yy"
    {
  (yyval.string) = strdup((yyvsp[(1) - (1)].string));
  free((yyvsp[(1) - (1)].string));
;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 625 "parser.yy"
    {
  (yyval.string) = strdup("INTEGER");
;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 629 "parser.yy"
    {
  (yyval.string) = strdup("DECIMAL");
;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 636 "parser.yy"
    {
;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 639 "parser.yy"
    {
;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 644 "parser.yy"
    {
  (yyval.decimal) = (yyvsp[(1) - (1)].integer);
;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 648 "parser.yy"
    {
  (yyval.decimal) = (yyvsp[(1) - (1)].decimal);
;}
    break;



/* Line 1455 of yacc.c  */
#line 2394 "parser.tab.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



