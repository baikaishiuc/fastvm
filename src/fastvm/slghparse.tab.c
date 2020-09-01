
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
#line 16 "slghparse.y"

#include "slgh_compile.h"

#define YYERROR_VERBOSE
#define YYMALLOC
#define YYFREE

  extern SleighCompile *slgh;
  extern int4 actionon;
  extern FILE *yyin;
  extern int yydebug;
  extern int yylex(void);
  extern int yyerror(const char *str, ...);


/* Line 189 of yacc.c  */
#line 89 "slghparse.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
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
     OP_BOOL_OR = 258,
     OP_BOOL_XOR = 259,
     OP_BOOL_AND = 260,
     OP_OR = 261,
     OP_XOR = 262,
     OP_AND = 263,
     OP_FNOTEQUAL = 264,
     OP_FEQUAL = 265,
     OP_NOTEQUAL = 266,
     OP_EQUAL = 267,
     OP_FGREATEQUAL = 268,
     OP_FLESSEQUAL = 269,
     OP_FGREAT = 270,
     OP_FLESS = 271,
     OP_SGREAT = 272,
     OP_SLESSEQUAL = 273,
     OP_SGREATEQUAL = 274,
     OP_SLESS = 275,
     OP_LESSEQUAL = 276,
     OP_GREATEQUAL = 277,
     OP_SRIGHT = 278,
     OP_RIGHT = 279,
     OP_LEFT = 280,
     OP_FSUB = 281,
     OP_FADD = 282,
     OP_FDIV = 283,
     OP_FMULT = 284,
     OP_SREM = 285,
     OP_SDIV = 286,
     OP_ZEXT = 287,
     OP_CARRY = 288,
     OP_BORROW = 289,
     OP_SEXT = 290,
     OP_SCARRY = 291,
     OP_SBORROW = 292,
     OP_NAN = 293,
     OP_ABS = 294,
     OP_SQRT = 295,
     OP_CEIL = 296,
     OP_FLOOR = 297,
     OP_ROUND = 298,
     OP_INT2FLOAT = 299,
     OP_FLOAT2FLOAT = 300,
     OP_TRUNC = 301,
     OP_CPOOLREF = 302,
     OP_NEW = 303,
     OP_POPCOUNT = 304,
     BADINTEGER = 305,
     GOTO_KEY = 306,
     CALL_KEY = 307,
     RETURN_KEY = 308,
     IF_KEY = 309,
     DEFINE_KEY = 310,
     ATTACH_KEY = 311,
     MACRO_KEY = 312,
     SPACE_KEY = 313,
     TYPE_KEY = 314,
     RAM_KEY = 315,
     DEFAULT_KEY = 316,
     REGISTER_KEY = 317,
     ENDIAN_KEY = 318,
     WITH_KEY = 319,
     ALIGN_KEY = 320,
     OP_UNIMPL = 321,
     TOKEN_KEY = 322,
     SIGNED_KEY = 323,
     NOFLOW_KEY = 324,
     HEX_KEY = 325,
     DEC_KEY = 326,
     BIG_KEY = 327,
     LITTLE_KEY = 328,
     SIZE_KEY = 329,
     WORDSIZE_KEY = 330,
     OFFSET_KEY = 331,
     NAMES_KEY = 332,
     VALUES_KEY = 333,
     VARIABLES_KEY = 334,
     PCODEOP_KEY = 335,
     IS_KEY = 336,
     LOCAL_KEY = 337,
     DELAYSLOT_KEY = 338,
     CROSSBUILD_KEY = 339,
     EXPORT_KEY = 340,
     BUILD_KEY = 341,
     CONTEXT_KEY = 342,
     ELLIPSIS_KEY = 343,
     GLOBALSET_KEY = 344,
     BITRANGE_KEY = 345,
     CH = 346,
     INTEGER = 347,
     INTB = 348,
     STRING = 349,
     SYMBOLSTRING = 350,
     SPACESYM = 351,
     SECTIONSYM = 352,
     TOKENSYM = 353,
     USEROPSYM = 354,
     VALUESYM = 355,
     VALUEMAPSYM = 356,
     CONTEXTSYM = 357,
     NAMESYM = 358,
     VARSYM = 359,
     BITSYM = 360,
     SPECSYM = 361,
     VARLISTSYM = 362,
     OPERANDSYM = 363,
     STARTSYM = 364,
     ENDSYM = 365,
     MACROSYM = 366,
     LABELSYM = 367,
     SUBTABLESYM = 368
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 31 "slghparse.y"

  char ch;
  intb i;
  intb big;
  CString *str;
  //vector<string> *strlist;
  struct dynarray   *strlist;
  // vector<intb> *biglist;
  struct dynarray   *biglist;
  //vector<ExpTree *> *param;
  struct dynarray   *param;
  SpaceQuality *spacequal;
  FieldQuality *fieldqual;
  StarQuality *starqual;
  VarnodeTpl *varnode;
  ExpTree *tree;
  struct dynarray  *stmt;
  ConstructTpl *sem;
  SectionVector *sectionstart;
  Constructor *construct;
  PatternEquation *pateq;
  PatternExpression *patexp;

  //vector<SleighSymbol *> *symlist;
  struct dynarray   *symlist;
  //vector<ContextChange *> *contop;
  struct dynarray  *contop;
  SleighSymbol *anysym;
  SpaceSymbol *spacesym;
  SectionSymbol *sectionsym;
  TokenSymbol *tokensym;
  UserOpSymbol *useropsym;
  MacroSymbol *macrosym;
  LabelSymbol *labelsym;
  SubtableSymbol *subtablesym;
  StartSymbol *startsym;
  EndSymbol *endsym;
  OperandSymbol *operandsym;
  VarnodeListSymbol *varlistsym;
  VarnodeSymbol *varsym;
  BitrangeSymbol *bitsym;
  NameSymbol *namesym;
  ValueSymbol *valuesym;
  ValueMapSymbol *valuemapsym;
  ContextSymbol *contextsym;
  FamilySymbol *famsym;
  SpecificSymbol *specsym;



/* Line 214 of yacc.c  */
#line 289 "slghparse.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 301 "slghparse.tab.c"

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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2559

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  137
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  71
/* YYNRULES -- Number of rules.  */
#define YYNRULES  336
/* YYNRULES -- Number of states.  */
#define YYNSTATES  709

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   368

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   136,    43,     2,     2,     2,    38,    11,     2,
     129,   130,    36,    32,   131,    33,     2,    37,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   135,     8,
      17,   128,    18,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   132,     2,   133,     9,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   134,     6,   127,    44,     2,     2,     2,
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
       5,     7,    10,    12,    13,    14,    15,    16,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    34,    35,    39,    40,    41,    42,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    14,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    35,    37,    39,    41,
      44,    50,    56,    62,    65,    72,    75,    79,    82,    86,
      89,    97,   105,   108,   111,   114,   122,   130,   133,   136,
     139,   142,   145,   149,   153,   158,   163,   168,   173,   176,
     187,   193,   198,   200,   203,   212,   217,   223,   229,   235,
     240,   247,   249,   252,   255,   258,   259,   261,   263,   264,
     266,   272,   276,   281,   283,   289,   295,   298,   301,   304,
     307,   310,   313,   316,   319,   322,   325,   328,   330,   333,
     335,   337,   339,   343,   347,   351,   355,   359,   363,   367,
     371,   375,   379,   382,   385,   387,   391,   395,   399,   402,
     404,   407,   409,   411,   415,   419,   423,   427,   431,   435,
     439,   443,   445,   447,   449,   451,   452,   456,   457,   463,
     472,   481,   487,   490,   494,   498,   501,   503,   507,   509,
     514,   520,   524,   529,   530,   533,   538,   545,   550,   556,
     561,   569,   576,   580,   586,   592,   602,   607,   612,   617,
     621,   627,   633,   639,   643,   649,   655,   659,   665,   668,
     674,   680,   682,   684,   687,   691,   695,   699,   703,   707,
     711,   715,   719,   723,   727,   731,   735,   739,   742,   745,
     749,   753,   757,   761,   765,   769,   773,   777,   781,   785,
     789,   792,   796,   800,   804,   808,   812,   816,   820,   824,
     828,   832,   836,   840,   844,   847,   852,   857,   862,   867,
     874,   881,   888,   893,   898,   903,   908,   913,   918,   923,
     928,   935,   940,   945,   949,   956,   958,   963,   968,   975,
     980,   984,   986,   988,   990,   992,   994,   996,  1001,  1003,
    1005,  1007,  1009,  1011,  1013,  1015,  1017,  1021,  1024,  1029,
    1031,  1033,  1035,  1039,  1043,  1045,  1048,  1053,  1057,  1059,
    1061,  1063,  1065,  1067,  1069,  1071,  1073,  1075,  1077,  1079,
    1081,  1083,  1086,  1090,  1092,  1095,  1097,  1100,  1102,  1105,
    1109,  1112,  1116,  1118,  1120,  1123,  1126,  1130,  1132,  1134,
    1137,  1140,  1144,  1146,  1148,  1150,  1152,  1155,  1158,  1161,
    1165,  1167,  1169,  1171,  1174,  1177,  1178,  1180,  1184,  1185,
    1187,  1191,  1193,  1195,  1197,  1199,  1201,  1203,  1205,  1207,
    1209,  1211,  1213,  1215,  1217,  1219,  1221
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     138,     0,    -1,   141,    -1,   138,   142,    -1,   138,   139,
      -1,   138,   140,    -1,   143,    -1,   145,    -1,   149,    -1,
     151,    -1,   152,    -1,   155,    -1,   156,    -1,   157,    -1,
     158,    -1,     1,     8,    -1,   167,    -1,   159,    -1,   162,
      -1,     1,   127,    -1,    68,    76,   128,    85,     8,    -1,
      68,    76,   128,    86,     8,    -1,    68,    78,   128,   105,
       8,    -1,   144,     8,    -1,    68,    80,   107,   129,   105,
     130,    -1,   144,   147,    -1,    68,    80,   207,    -1,   146,
       8,    -1,    68,   100,   117,    -1,   146,   148,    -1,   107,
     128,   129,   105,   131,   105,   130,    -1,   207,   128,   129,
     105,   131,   105,   130,    -1,   147,    81,    -1,   147,    83,
      -1,   147,    84,    -1,   107,   128,   129,   105,   131,   105,
     130,    -1,   207,   128,   129,   105,   131,   105,   130,    -1,
     148,    81,    -1,   148,    82,    -1,   148,    83,    -1,   148,
      84,    -1,   150,     8,    -1,    68,    71,   107,    -1,    68,
      71,   207,    -1,   150,    72,   128,    73,    -1,   150,    72,
     128,    75,    -1,   150,    87,   128,   105,    -1,   150,    88,
     128,   105,    -1,   150,    74,    -1,    68,   109,    89,   128,
     105,    87,   128,   105,   197,     8,    -1,    68,   109,    89,
     128,    63,    -1,    68,   103,   153,     8,    -1,   154,    -1,
     153,   154,    -1,   107,   128,   117,   132,   105,   131,   105,
     133,    -1,    68,    93,   197,     8,    -1,    69,    91,   201,
     195,     8,    -1,    69,    90,   201,   199,     8,    -1,    69,
      92,   201,   203,     8,    -1,   165,   134,   181,   127,    -1,
      77,   163,   135,   164,   176,   134,    -1,   160,    -1,   161,
     139,    -1,   161,   140,    -1,   161,   127,    -1,    -1,   126,
      -1,   107,    -1,    -1,   171,    -1,    70,   107,   129,   206,
     130,    -1,   134,   181,   127,    -1,   134,   180,   182,   127,
      -1,    79,    -1,   168,    94,   171,   176,   166,    -1,   169,
      94,   171,   176,   166,    -1,   169,   107,    -1,   169,   194,
      -1,   169,   108,    -1,   169,     9,    -1,   168,     9,    -1,
     168,   107,    -1,   168,   194,    -1,   168,   136,    -1,   168,
     108,    -1,   126,   135,    -1,   107,   135,    -1,   135,    -1,
     169,   136,    -1,   106,    -1,   192,    -1,   193,    -1,   129,
     170,   130,    -1,   170,    32,   170,    -1,   170,    33,   170,
      -1,   170,    36,   170,    -1,   170,    31,   170,    -1,   170,
      30,   170,    -1,   170,    12,   170,    -1,   170,     7,   170,
      -1,   170,    10,   170,    -1,   170,    37,   170,    -1,    33,
     170,    -1,    44,   170,    -1,   172,    -1,   171,    11,   171,
      -1,   171,     6,   171,    -1,   171,     8,   171,    -1,   101,
     173,    -1,   173,    -1,   174,   101,    -1,   174,    -1,   175,
      -1,   129,   171,   130,    -1,   192,   128,   170,    -1,   192,
      15,   170,    -1,   192,    17,   170,    -1,   192,    27,   170,
      -1,   192,    18,   170,    -1,   192,    28,   170,    -1,   121,
     128,   170,    -1,   121,    -1,   119,    -1,   192,    -1,   126,
      -1,    -1,   132,   177,   133,    -1,    -1,   177,   115,   128,
     170,     8,    -1,   177,   102,   129,   192,   131,   115,   130,
       8,    -1,   177,   102,   129,   193,   131,   115,   130,     8,
      -1,   177,   121,   128,   170,     8,    -1,   177,   107,    -1,
      31,   107,    30,    -1,    31,   110,    30,    -1,   181,   178,
      -1,   179,    -1,   180,   182,   178,    -1,   182,    -1,   182,
      98,   191,     8,    -1,   182,    98,   185,   189,     8,    -1,
     182,    98,   107,    -1,   182,    98,   185,   107,    -1,    -1,
     182,   183,    -1,   182,    95,   107,     8,    -1,   182,    95,
     107,   135,   105,     8,    -1,   189,   128,   184,     8,    -1,
      95,   107,   128,   184,     8,    -1,   107,   128,   184,     8,
      -1,    95,   107,   135,   105,   128,   184,     8,    -1,   107,
     135,   105,   128,   184,     8,    -1,    95,   193,   128,    -1,
     185,   184,   128,   184,     8,    -1,   112,   129,   205,   130,
       8,    -1,   189,   132,   105,   131,   105,   133,   128,   184,
       8,    -1,   118,   128,   184,     8,    -1,   187,   135,   105,
     128,    -1,   187,   129,   105,   130,    -1,    99,   121,     8,
      -1,    97,   187,   131,   110,     8,    -1,    97,   187,   131,
     107,     8,    -1,    96,   129,   105,   130,     8,    -1,    64,
     186,     8,    -1,    67,   184,    64,   186,     8,    -1,    64,
     132,   184,   133,     8,    -1,    65,   186,     8,    -1,    65,
     132,   184,   133,     8,    -1,    66,     8,    -1,    66,   132,
     184,   133,     8,    -1,   124,   129,   205,   130,     8,    -1,
     190,    -1,   187,    -1,   185,   184,    -1,   129,   184,   130,
      -1,   184,    32,   184,    -1,   184,    33,   184,    -1,   184,
      16,   184,    -1,   184,    15,   184,    -1,   184,    17,   184,
      -1,   184,    28,   184,    -1,   184,    27,   184,    -1,   184,
      18,   184,    -1,   184,    26,   184,    -1,   184,    25,   184,
      -1,   184,    24,   184,    -1,   184,    23,   184,    -1,    33,
     184,    -1,    44,   184,    -1,   184,     9,   184,    -1,   184,
      11,   184,    -1,   184,     6,   184,    -1,   184,    31,   184,
      -1,   184,    30,   184,    -1,   184,    29,   184,    -1,   184,
      36,   184,    -1,   184,    37,   184,    -1,   184,    42,   184,
      -1,   184,    38,   184,    -1,   184,    41,   184,    -1,    43,
     184,    -1,   184,     4,   184,    -1,   184,     5,   184,    -1,
     184,     3,   184,    -1,   184,    14,   184,    -1,   184,    13,
     184,    -1,   184,    22,   184,    -1,   184,    21,   184,    -1,
     184,    20,   184,    -1,   184,    19,   184,    -1,   184,    35,
     184,    -1,   184,    34,   184,    -1,   184,    40,   184,    -1,
     184,    39,   184,    -1,    34,   184,    -1,    52,   129,   184,
     130,    -1,    53,   129,   184,   130,    -1,    48,   129,   184,
     130,    -1,    45,   129,   184,   130,    -1,    46,   129,   184,
     131,   184,   130,    -1,    49,   129,   184,   131,   184,   130,
      -1,    50,   129,   184,   131,   184,   130,    -1,    58,   129,
     184,   130,    -1,    57,   129,   184,   130,    -1,    51,   129,
     184,   130,    -1,    59,   129,   184,   130,    -1,    54,   129,
     184,   130,    -1,    55,   129,   184,   130,    -1,    56,   129,
     184,   130,    -1,    61,   129,   184,   130,    -1,    61,   129,
     184,   131,   184,   130,    -1,    62,   129,   184,   130,    -1,
     193,   129,   188,   130,    -1,   193,   135,   105,    -1,   193,
     132,   105,   131,   105,   133,    -1,   118,    -1,   112,   129,
     205,   130,    -1,    60,   129,   205,   130,    -1,    36,   132,
     109,   133,   135,   105,    -1,    36,   132,   109,   133,    -1,
      36,   135,   105,    -1,    36,    -1,   122,    -1,   123,    -1,
     105,    -1,    63,    -1,   121,    -1,   105,   132,   109,   133,
      -1,   190,    -1,   107,    -1,   193,    -1,   188,    -1,   107,
      -1,   126,    -1,   105,    -1,    63,    -1,   105,   135,   105,
      -1,    11,   187,    -1,    11,   135,   105,   187,    -1,   193,
      -1,   107,    -1,   126,    -1,    17,   125,    18,    -1,    17,
     107,    18,    -1,   193,    -1,    11,   187,    -1,    11,   135,
     105,   187,    -1,   105,   135,   105,    -1,   107,    -1,   126,
      -1,   113,    -1,   114,    -1,   115,    -1,   116,    -1,   120,
      -1,   117,    -1,   119,    -1,   121,    -1,   122,    -1,   123,
      -1,   104,    -1,   194,   104,    -1,   132,   196,   133,    -1,
     105,    -1,    33,   105,    -1,   105,    -1,    33,   105,    -1,
     107,    -1,   196,   105,    -1,   196,    33,   105,    -1,   196,
     107,    -1,   132,   198,   133,    -1,   107,    -1,   107,    -1,
     198,   107,    -1,   198,   207,    -1,   132,   200,   133,    -1,
     107,    -1,   207,    -1,   200,   107,    -1,   200,   207,    -1,
     132,   202,   133,    -1,   113,    -1,   115,    -1,   113,    -1,
     115,    -1,   202,   113,    -1,   202,   115,    -1,   202,   107,
      -1,   132,   204,   133,    -1,   117,    -1,   117,    -1,   107,
      -1,   204,   117,    -1,   204,   107,    -1,    -1,   184,    -1,
     205,   131,   184,    -1,    -1,   107,    -1,   206,   131,   107,
      -1,   109,    -1,   110,    -1,   111,    -1,   112,    -1,   124,
      -1,   126,    -1,   113,    -1,   114,    -1,   115,    -1,   116,
      -1,   117,    -1,   120,    -1,   121,    -1,   122,    -1,   123,
      -1,   118,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   163,   163,   164,   165,   166,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   179,   180,   181,   182,
     184,   185,   187,   189,   191,   192,   193,   195,   197,   198,
     201,   202,   203,   204,   205,   207,   208,   209,   210,   211,
     212,   214,   216,   217,   218,   219,   220,   221,   222,   224,
     226,   228,   230,   231,   233,   237,   239,   241,   243,   245,
     248,   250,   251,   252,   254,   256,   257,   258,   261,   262,
     265,   267,   268,   269,   271,   272,   274,   275,   276,   280,
     281,   282,   283,   284,   285,   287,   288,   290,   291,   293,
     295,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   313,   314,   315,   316,   318,   319,
     321,   322,   324,   325,   327,   328,   329,   330,   331,   332,
     333,   336,   337,   338,   339,   341,   342,   344,   345,   347,
     348,   349,   350,   352,   353,   355,   357,   358,   360,   361,
     362,   363,   364,   366,   367,   368,   369,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   380,   384,   385,   386,
     387,   388,   389,   390,   391,   392,   393,   394,   395,   396,
     397,   398,   400,   401,   402,   403,   404,   405,   406,   407,
     408,   409,   410,   411,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,   425,   426,   427,
     428,   429,   430,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   463,   467,   468,   470,   471,
     472,   473,   475,   477,   479,   480,   482,   483,   485,   487,
     489,   490,   491,   492,   494,   496,   498,   500,   501,   503,
     504,   505,   507,   508,   510,   511,   512,   513,   515,   516,
     518,   519,   520,   521,   522,   524,   525,   526,   527,   528,
     530,   531,   533,   534,   535,   537,   538,   539,   541,   542,
     543,   546,   547,   549,   550,   551,   553,   555,   556,   557,
     558,   560,   561,   562,   564,   565,   566,   567,   568,   570,
     571,   573,   574,   576,   577,   580,   581,   582,   584,   585,
     586,   588,   589,   590,   591,   592,   593,   594,   595,   596,
     597,   598,   599,   600,   601,   602,   603
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OP_BOOL_OR", "OP_BOOL_XOR",
  "OP_BOOL_AND", "'|'", "OP_OR", "';'", "'^'", "OP_XOR", "'&'", "OP_AND",
  "OP_FNOTEQUAL", "OP_FEQUAL", "OP_NOTEQUAL", "OP_EQUAL", "'<'", "'>'",
  "OP_FGREATEQUAL", "OP_FLESSEQUAL", "OP_FGREAT", "OP_FLESS", "OP_SGREAT",
  "OP_SLESSEQUAL", "OP_SGREATEQUAL", "OP_SLESS", "OP_LESSEQUAL",
  "OP_GREATEQUAL", "OP_SRIGHT", "OP_RIGHT", "OP_LEFT", "'+'", "'-'",
  "OP_FSUB", "OP_FADD", "'*'", "'/'", "'%'", "OP_FDIV", "OP_FMULT",
  "OP_SREM", "OP_SDIV", "'!'", "'~'", "OP_ZEXT", "OP_CARRY", "OP_BORROW",
  "OP_SEXT", "OP_SCARRY", "OP_SBORROW", "OP_NAN", "OP_ABS", "OP_SQRT",
  "OP_CEIL", "OP_FLOOR", "OP_ROUND", "OP_INT2FLOAT", "OP_FLOAT2FLOAT",
  "OP_TRUNC", "OP_CPOOLREF", "OP_NEW", "OP_POPCOUNT", "BADINTEGER",
  "GOTO_KEY", "CALL_KEY", "RETURN_KEY", "IF_KEY", "DEFINE_KEY",
  "ATTACH_KEY", "MACRO_KEY", "SPACE_KEY", "TYPE_KEY", "RAM_KEY",
  "DEFAULT_KEY", "REGISTER_KEY", "ENDIAN_KEY", "WITH_KEY", "ALIGN_KEY",
  "OP_UNIMPL", "TOKEN_KEY", "SIGNED_KEY", "NOFLOW_KEY", "HEX_KEY",
  "DEC_KEY", "BIG_KEY", "LITTLE_KEY", "SIZE_KEY", "WORDSIZE_KEY",
  "OFFSET_KEY", "NAMES_KEY", "VALUES_KEY", "VARIABLES_KEY", "PCODEOP_KEY",
  "IS_KEY", "LOCAL_KEY", "DELAYSLOT_KEY", "CROSSBUILD_KEY", "EXPORT_KEY",
  "BUILD_KEY", "CONTEXT_KEY", "ELLIPSIS_KEY", "GLOBALSET_KEY",
  "BITRANGE_KEY", "CH", "INTEGER", "INTB", "STRING", "SYMBOLSTRING",
  "SPACESYM", "SECTIONSYM", "TOKENSYM", "USEROPSYM", "VALUESYM",
  "VALUEMAPSYM", "CONTEXTSYM", "NAMESYM", "VARSYM", "BITSYM", "SPECSYM",
  "VARLISTSYM", "OPERANDSYM", "STARTSYM", "ENDSYM", "MACROSYM", "LABELSYM",
  "SUBTABLESYM", "'}'", "'='", "'('", "')'", "','", "'['", "']'", "'{'",
  "':'", "' '", "$accept", "spec", "definition", "constructorlike",
  "endiandef", "aligndef", "tokendef", "tokenprop", "contextdef",
  "contextprop", "fielddef", "contextfielddef", "spacedef", "spaceprop",
  "varnodedef", "bitrangedef", "bitrangelist", "bitrangesingle",
  "pcodeopdef", "valueattach", "nameattach", "varattach", "macrodef",
  "withblockstart", "withblockmid", "withblock", "id_or_nil",
  "bitpat_or_nil", "macrostart", "rtlbody", "constructor",
  "constructprint", "subtablestart", "pexpression", "pequation", "elleq",
  "ellrt", "atomic", "constraint", "contextblock", "contextlist",
  "section_def", "rtlfirstsection", "rtlcontinue", "rtl", "rtlmid",
  "statement", "expr", "sizedstar", "jumpdest", "varnode",
  "integervarnode", "lhsvarnode", "label", "exportvarnode", "familysymbol",
  "specificsymbol", "charstring", "intblist", "intbpart", "stringlist",
  "stringpart", "anystringlist", "anystringpart", "valuelist", "valuepart",
  "varlist", "varpart", "paramlist", "oplist", "anysymbol", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   124,   261,    59,    94,
     262,    38,   263,   264,   265,   266,   267,    60,    62,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,    43,    45,   281,   282,    42,    47,    37,   283,
     284,   285,   286,    33,   126,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,   351,
     352,   353,   354,   355,   356,   357,   358,   359,   360,   361,
     362,   363,   364,   365,   366,   367,   368,   125,    61,    40,
      41,    44,    91,    93,   123,    58,    32
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   137,   138,   138,   138,   138,   139,   139,   139,   139,
     139,   139,   139,   139,   139,   139,   140,   140,   140,   140,
     141,   141,   142,   143,   144,   144,   144,   145,   146,   146,
     147,   147,   147,   147,   147,   148,   148,   148,   148,   148,
     148,   149,   150,   150,   150,   150,   150,   150,   150,   151,
     151,   152,   153,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   161,   161,   162,   163,   163,   163,   164,   164,
     165,   166,   166,   166,   167,   167,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   169,   169,   169,   169,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   171,   171,   171,   171,   172,   172,
     173,   173,   174,   174,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   176,   176,   177,   177,   177,
     177,   177,   177,   178,   178,   179,   180,   180,   181,   181,
     181,   181,   181,   182,   182,   182,   182,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   185,   185,
     185,   185,   186,   186,   186,   186,   186,   186,   186,   186,
     187,   187,   187,   187,   188,   188,   188,   188,   188,   189,
     189,   189,   190,   190,   191,   191,   191,   191,   191,   191,
     192,   192,   192,   192,   192,   193,   193,   193,   193,   193,
     194,   194,   195,   195,   195,   196,   196,   196,   196,   196,
     196,   197,   197,   198,   198,   198,   199,   200,   200,   200,
     200,   201,   201,   201,   202,   202,   202,   202,   202,   203,
     203,   204,   204,   204,   204,   205,   205,   205,   206,   206,
     206,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     2,
       5,     5,     5,     2,     6,     2,     3,     2,     3,     2,
       7,     7,     2,     2,     2,     7,     7,     2,     2,     2,
       2,     2,     3,     3,     4,     4,     4,     4,     2,    10,
       5,     4,     1,     2,     8,     4,     5,     5,     5,     4,
       6,     1,     2,     2,     2,     0,     1,     1,     0,     1,
       5,     3,     4,     1,     5,     5,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     2,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     1,     3,     3,     3,     2,     1,
       2,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     1,     0,     3,     0,     5,     8,
       8,     5,     2,     3,     3,     2,     1,     3,     1,     4,
       5,     3,     4,     0,     2,     4,     6,     4,     5,     4,
       7,     6,     3,     5,     5,     9,     4,     4,     4,     3,
       5,     5,     5,     3,     5,     5,     3,     5,     2,     5,
       5,     1,     1,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     4,     4,     4,     4,     6,
       6,     6,     4,     4,     4,     4,     4,     4,     4,     4,
       6,     4,     4,     3,     6,     1,     4,     4,     6,     4,
       3,     1,     1,     1,     1,     1,     1,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     2,     4,     1,
       1,     1,     3,     3,     1,     2,     4,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     3,     1,     2,     1,     2,     1,     2,     3,
       2,     3,     1,     1,     2,     2,     3,     1,     1,     2,
       2,     3,     1,     1,     1,     1,     2,     2,     2,     3,
       1,     1,     1,     2,     2,     0,     1,     3,     0,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,     2,     0,     1,     0,     0,     0,     0,
      65,     0,     0,    87,     4,     5,     3,     6,     0,     7,
       0,     8,     0,     9,    10,    11,    12,    13,    14,    17,
      61,     0,    18,     0,    16,     0,     0,     0,    15,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    67,    66,     0,    86,    85,    23,     0,   321,   322,
     323,   324,   327,   328,   329,   330,   331,   336,   332,   333,
     334,   335,   325,   326,    25,     0,    27,     0,    29,     0,
      41,     0,    48,     0,     0,     0,    64,    62,    63,   143,
      80,     0,   280,    81,    84,    83,    82,    79,     0,    76,
      78,    88,    77,     0,     0,    42,    43,     0,     0,    26,
     292,     0,     0,    28,     0,     0,    52,     0,   302,   303,
       0,     0,     0,     0,   318,    68,     0,    32,    33,    34,
       0,     0,    37,    38,    39,    40,     0,     0,     0,     0,
       0,   138,     0,   270,   271,   272,   273,   122,   274,   121,
     124,     0,   125,   104,   109,   111,   112,   123,   281,   125,
      20,    21,     0,     0,   293,     0,    55,     0,    51,    53,
       0,   304,   305,     0,     0,     0,     0,   283,     0,     0,
     310,     0,     0,   319,     0,   125,    69,     0,     0,     0,
       0,    44,    45,    46,    47,    59,     0,     0,   241,   255,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   254,
     252,     0,   275,     0,   276,   277,   278,   279,     0,   253,
     144,     0,     0,   251,     0,   171,   250,   108,     0,     0,
       0,     0,     0,   127,     0,   110,     0,     0,     0,     0,
       0,     0,     0,    22,     0,   294,   291,   295,     0,    50,
       0,   308,   306,   307,   301,   297,     0,   298,    57,   284,
       0,   285,   287,     0,    56,   312,   311,     0,    58,    70,
       0,     0,     0,     0,     0,     0,   252,   253,     0,   257,
     250,     0,     0,     0,     0,   245,   244,   249,   246,   242,
     243,     0,     0,   248,     0,     0,   168,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     235,     0,     0,     0,   172,   250,     0,     0,     0,     0,
       0,     0,   141,   269,     0,     0,   264,     0,     0,     0,
       0,   315,     0,   315,     0,     0,     0,     0,     0,     0,
       0,    89,     0,   120,    90,    91,   113,   106,   107,   105,
       0,    73,   143,    74,   115,   116,   118,   117,   119,   114,
      75,    24,     0,     0,   299,   296,   300,   286,     0,   288,
     290,   282,   314,   313,   309,   320,    60,     0,     0,     0,
       0,     0,   263,   262,     0,   240,     0,     0,   163,     0,
     166,     0,   187,   214,   200,   188,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     315,     0,     0,   315,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   173,     0,     0,     0,   145,     0,     0,   152,
       0,     0,     0,   265,     0,   142,   261,     0,   259,   139,
     159,   256,     0,     0,   316,     0,     0,     0,     0,     0,
       0,     0,     0,   102,   103,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   132,     0,     0,   126,
     136,   143,     0,     0,     0,   289,     0,     0,     0,     0,
     258,   239,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   174,   203,   201,   202,   191,   189,
     190,   205,   204,   178,   177,   179,   182,   209,   208,   207,
     206,   186,   185,   184,   183,   181,   180,   194,   193,   192,
     175,   176,   211,   210,   195,   196,   198,   213,   212,   199,
     197,     0,     0,     0,   233,     0,     0,     0,     0,     0,
       0,   267,   140,   149,     0,     0,     0,   156,     0,     0,
     158,   157,   147,     0,    92,    99,   100,    98,    97,    96,
      93,    94,    95,   101,     0,     0,     0,     0,     0,    71,
     135,     0,     0,    30,    31,    35,    36,     0,   247,   165,
     167,   169,   218,     0,   217,     0,     0,   224,   215,   216,
     226,   227,   228,   223,   222,   225,   237,   229,     0,   231,
     236,   164,   232,     0,   148,   146,     0,   162,   161,   160,
     266,     0,   154,   317,   170,   153,     0,     0,     0,     0,
       0,    72,   137,     0,     0,     0,     0,   238,     0,     0,
       0,     0,     0,     0,   151,     0,     0,     0,   128,   131,
     133,   134,    54,    49,   219,   220,   221,   230,   234,   150,
       0,     0,     0,     0,     0,     0,   155,   129,   130
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,    14,    15,     3,    16,    17,    18,    19,    20,
      74,    78,    21,    22,    23,    24,   115,   116,    25,    26,
      27,    28,    29,    30,    31,    32,    53,   185,    33,   363,
      34,    35,    36,   353,   152,   153,   154,   155,   156,   234,
     360,   620,   510,   511,   140,   141,   220,   484,   323,   292,
     324,   223,   224,   293,   335,   354,   325,    96,   179,   263,
     112,   165,   175,   256,   121,   173,   182,   267,   485,   184,
      75
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -331
static const yytype_int16 yypact[] =
{
     -26,    -6,     3,  -331,   -29,  -331,     4,   409,   244,   -25,
     -38,   -48,    34,  -331,  -331,  -331,  -331,  -331,   420,  -331,
     449,  -331,   215,  -331,  -331,  -331,  -331,  -331,  -331,  -331,
    -331,    39,  -331,    92,  -331,    28,    54,   122,  -331,  -331,
    2386,    25,  2404,   -46,    82,   183,   206,    62,    62,    62,
     189,  -331,  -331,   208,  -331,  -331,  -331,   223,  -331,  -331,
    -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,
    -331,  -331,  -331,  -331,    37,   263,  -331,   268,   258,   273,
    -331,   293,  -331,   297,   305,   -35,  -331,  -331,  -331,  -331,
    -331,  1637,  -331,  -331,  -331,  -331,   313,  -331,  1637,  -331,
    -331,  -331,   313,   397,   427,  -331,  -331,   332,   311,  -331,
    -331,   338,   439,  -331,   328,    56,  -331,   330,  -331,  -331,
      63,   320,   -13,   -79,   368,  1637,   350,  -331,  -331,  -331,
     352,   353,  -331,  -331,  -331,  -331,   355,   118,   381,   383,
     363,  1678,  1497,  -331,  -331,  -331,  -331,  -331,  -331,   365,
    -331,  1637,    15,  -331,  -331,   404,  -331,    29,  -331,    15,
    -331,  -331,   490,   401,  -331,  2282,  -331,   390,  -331,  -331,
     -28,  -331,  -331,   -74,  2422,   500,   405,  -331,    10,   503,
    -331,   -41,   511,  -331,   178,   392,   277,   417,   421,   434,
     440,  -331,  -331,  -331,  -331,  -331,   281,    18,   165,  -331,
      -9,    74,     1,  1546,  1443,   396,   327,   301,   426,   413,
     195,   422,  -331,   424,  -331,  -331,  -331,  -331,   428,    57,
    -331,  1546,   -40,  -331,   139,  -331,   196,  -331,  1519,    19,
    1637,  1637,  1637,  -331,   -50,  -331,  1519,  1519,  1519,  1519,
    1519,  1519,   -50,  -331,   419,  -331,  -331,  -331,   418,  -331,
     466,  -331,  -331,  -331,  -331,  -331,  2307,  -331,  -331,  -331,
     463,  -331,  -331,   -22,  -331,  -331,  -331,   -55,  -331,  -331,
     467,   442,   446,   455,   456,   458,  -331,  -331,   486,  -331,
    -331,   608,   609,   520,   526,  -331,   534,  -331,  -331,  -331,
    -331,  1546,   661,  -331,  1546,   663,  -331,  1546,  1546,  1546,
    1546,  1546,   538,   573,   578,   581,   618,   621,   654,   655,
     665,   696,   701,   704,   706,   741,   744,   746,   781,   786,
    -331,  1546,  1802,  1546,  -331,   137,    24,   580,   643,   780,
     378,   657,   942,  -331,  1665,   947,  -331,   982,   846,  1546,
     886,  1546,  1546,  1546,  1503,   890,   925,  1546,   926,  1519,
    1519,  -331,  1519,  1691,  -331,  -331,  -331,   342,  1024,  -331,
      65,  -331,  -331,  -331,  1691,  1691,  1691,  1691,  1691,  1691,
    -331,  -331,   965,   987,  -331,  -331,  -331,  -331,   966,  -331,
    -331,  -331,  -331,  -331,  -331,  -331,  -331,   970,  1005,  1006,
    1045,   327,  -331,  -331,  1018,  -331,  1046,   343,  -331,   579,
    -331,   619,  -331,  -331,  -331,  -331,  1546,  1546,  1546,  1546,
    1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,
    1546,  1546,  1546,  1546,   823,  1546,  1546,  1546,  1546,  1546,
    1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,
    1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,
    1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,  1546,
    1546,   210,  -331,   282,  1085,  1086,  -331,  1546,  1090,  -331,
    1100,   285,  1126,  -331,  1130,  1262,  -331,  1267,  -331,  -331,
    -331,  -331,  1854,  1182,  2174,   300,  1894,   312,  1546,  1141,
    1187,  1934,  1180,  -331,  -331,   289,  1519,  1519,  1519,  1519,
    1519,  1519,  1519,  1519,  1519,  1221,  -331,  1227,  1302,  -331,
    -331,  -331,   -21,  1220,  1285,  -331,  1261,  1265,  1301,  1305,
    -331,  1335,  1338,  1467,  1502,  1507,   863,   700,   903,   740,
     782,   943,   983,  1023,  1063,  1103,  1143,  1183,  1223,  1263,
     324,   659,  1303,   339,  -331,  2213,  2250,  2250,  2284,  2316,
    2346,  2424,  2424,  2424,  2424,  2450,  2450,  2450,  2450,  2450,
    2450,  2450,  2450,  2450,  2450,  2450,  2450,  1813,  1813,  1813,
     373,   373,   373,   373,  -331,  -331,  -331,  -331,  -331,  -331,
    -331,  1540,  1381,  1424,  -331,  1974,    23,  1548,  1550,  1551,
     327,  -331,  -331,  -331,  1546,  1553,  1546,  -331,  1559,  2014,
    -331,  -331,  -331,  1449,  -331,   274,  1654,  1624,   151,   151,
     435,   435,  -331,  -331,  2436,  1519,  1519,  1613,   319,  -331,
    -331,  1468,   -46,  -331,  -331,  -331,  -331,  1469,  -331,  -331,
    -331,  -331,  -331,  1546,  -331,  1546,  1546,  -331,  -331,  -331,
    -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  1546,  -331,
    -331,  -331,  -331,  1472,  -331,  -331,  1546,  -331,  -331,  -331,
    -331,  2054,  -331,  2174,  -331,  -331,  1435,  1447,  1450,   484,
    1539,  -331,  -331,  1554,  1555,  1453,  1575,  -331,  1343,  1383,
    1423,  1463,  1454,  2094,  -331,  1460,  1499,  1500,  -331,  -331,
    -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,
    1546,  1489,  1490,  2134,  1614,  1619,  -331,  -331,  -331
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -331,  -331,  1590,  1597,  -331,  -331,  -331,  -331,  -331,  -331,
    -331,  -331,  -331,  -331,  -331,  -331,  -331,  1514,  -331,  -331,
    -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  1395,
    -331,  -331,  -331,  -222,   -76,  -331,  1501,  -331,  -331,  -104,
    -331,  1028,  -331,  -331,  1284,  1136,  -331,  -197,  -140,  -196,
     -62,  1189,  1316,  -139,  -331,   -91,   -36,  1623,  -331,  -331,
    1040,  -331,  -331,  -331,   425,  -331,  -331,  -331,  -330,  -331,
       8
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -269
static const yytype_int16 yytable[] =
{
     157,   221,   225,     5,     6,   295,   322,   157,   197,   296,
     618,   378,    38,   487,   364,   365,   366,   367,   368,   369,
     176,   230,   159,   231,   344,   230,   232,   231,    79,   361,
     232,   655,   466,   251,   157,   249,    40,    90,   180,   252,
       6,   253,     1,   260,   236,    42,   237,   238,   106,   186,
     109,   157,   382,   181,   285,   242,   239,   240,    43,   254,
     157,   110,   383,    97,   168,    44,   265,   334,    45,    51,
       4,     7,     8,     9,    46,   229,   266,   250,   384,   222,
      10,   271,    50,   379,   362,   380,   111,    54,    52,   345,
     540,   197,   177,   543,   397,   346,   286,   399,   287,    37,
     401,   402,   403,   404,   405,   226,   619,    85,     8,     9,
      11,   381,   288,   289,   290,   261,    10,   262,   127,   178,
     128,   129,    91,   291,   424,   281,   462,   493,   494,    12,
     495,    39,    92,   297,   279,    93,    94,   285,    13,   157,
     157,   157,   482,   282,   329,   486,    11,   233,    98,   356,
     491,   656,   467,   107,   357,   358,   359,   241,    92,   468,
     280,    99,   100,   114,    95,    12,    86,   505,   327,    55,
     280,   336,   506,   247,    13,   118,   171,   119,   172,   286,
     507,   287,   257,   501,   502,  -261,   508,   503,   504,  -261,
     101,   191,   355,   192,   120,   288,   289,   290,   509,   113,
     355,   355,   355,   355,   355,   355,   294,   103,   104,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,    80,   541,   542,    89,   197,   545,   546,
     547,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,   559,   560,   561,   562,   563,   564,   565,   566,
     567,   568,   569,   570,   571,   572,   573,   574,   575,   576,
     577,   578,   579,   580,   376,   581,   463,   347,   473,   464,
     585,   348,   465,   285,   605,   606,   607,   608,   609,   610,
     611,   612,   613,   230,   497,   231,   498,    81,   232,    82,
     114,   599,   196,   196,   280,   117,   496,   283,   478,   497,
     284,   498,    83,    84,   499,   500,   501,   502,   269,   270,
     503,   504,   330,   355,   355,   286,   355,   287,   124,   499,
     500,   501,   502,   339,  -259,   503,   504,  -260,  -259,   520,
     340,   288,   289,   290,    47,    48,    49,   198,   196,   132,
     133,   134,   135,   125,   199,   199,   425,   426,   427,   428,
     231,   126,   429,   232,   430,   280,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   209,   209,   276,   196,
     199,   130,   588,   669,   670,   589,   131,   661,   212,   663,
     214,   136,   215,   216,   217,   160,   331,   277,   332,   454,
     455,   456,   457,   458,   459,   460,   278,   158,   212,   604,
     214,   137,   215,   216,   217,   138,   673,   333,    56,   674,
     595,   596,   209,   139,   276,   161,   678,   162,   679,   680,
     163,   199,   598,   596,   212,   164,   214,   166,   215,   216,
     217,   681,   174,   277,   646,   596,   167,    76,   170,   683,
     355,   355,   355,   355,   355,   355,   355,   355,   355,   650,
     596,   503,   504,   122,   123,   183,   523,   221,   225,   187,
      40,   188,   189,   209,   190,   276,   193,    41,   194,    42,
     195,   496,   688,   228,   497,   212,   498,   214,   243,   215,
     216,   217,    43,   703,   277,   235,   244,   248,   258,    44,
     259,   264,    45,   472,   499,   500,   501,   502,    46,   268,
     503,   504,   272,   667,   233,   328,   273,    57,   660,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,   274,
      68,    69,    70,    71,    72,   275,    73,   337,   338,   371,
     372,   341,   342,   373,   280,   222,    77,   343,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   377,    68,
      69,    70,    71,    72,   385,    73,   386,   387,   668,   355,
     355,   226,   425,   426,   427,   428,   388,   389,   429,   390,
     430,   391,   431,   432,   433,   434,   435,   436,   437,   438,
     439,   440,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   425,   426,   427,   428,   392,   393,   429,   394,
     430,   395,   431,   432,   433,   434,   435,   436,   437,   438,
     439,   440,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   425,   426,   427,   428,   396,   406,   429,   398,
     430,   400,   431,   432,   433,   434,   435,   436,   437,   438,
     439,   440,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   407,   425,   426,   427,   428,   408,   469,   429,
     409,   430,   524,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   425,   426,   427,   428,   410,   470,   429,
     411,   430,   525,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   412,   413,   425,   426,   427,   428,   647,
     648,   429,   474,   430,   414,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   415,   425,   426,   427,   428,
     416,   633,   429,   417,   430,   418,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     419,   635,   429,   420,   430,   421,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     422,   471,   429,   636,   430,   423,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
    -268,   481,   429,   544,   430,   479,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     480,   483,   429,   632,   430,   489,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     490,   492,   429,   634,   430,   232,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     513,   515,   429,   637,   430,   516,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     517,   518,   429,   638,   430,   514,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     519,   521,   429,   639,   430,   522,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     583,   584,   429,   640,   430,   586,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     587,   590,   429,   641,   430,   591,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
    -260,   600,   429,   642,   430,   592,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     594,   603,   429,   643,   430,   601,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     614,   621,   429,   644,   430,   615,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     622,   623,   429,   645,   430,   624,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     616,   625,   429,   649,   430,   626,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     627,   628,   429,   694,   430,   629,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   425,   426,   427,   428,
     630,   652,   429,   695,   430,   631,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   496,   689,   651,   497,
     326,   498,   349,   696,   666,   653,   657,   196,   658,   659,
     212,   662,   214,   350,   215,   216,   217,   664,   685,   499,
     500,   501,   502,   675,   677,   503,   504,   682,   686,   298,
     299,   687,   198,   693,   690,   691,   692,   698,   700,   300,
     301,   302,   303,   697,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   199,
     143,   144,   145,   146,   701,   702,   147,   148,   149,   704,
     705,    87,   707,   150,   196,   351,   151,   708,    88,   169,
     197,   488,   143,   144,   145,   146,   212,   370,   214,   148,
     215,   216,   217,   227,   618,   672,   512,   617,   352,   198,
     477,   209,   582,   276,   499,   500,   501,   502,   319,   102,
     503,   504,   676,   212,   320,   214,   498,   215,   216,   217,
       0,     0,   277,     0,     0,   321,   199,   200,   201,   202,
     203,     0,     0,     0,   499,   500,   501,   502,     0,   196,
     503,   504,     0,     0,     0,   197,     0,     0,   496,     0,
       0,   497,     0,   498,     0,     0,     0,     0,   204,   205,
     206,     0,   208,     0,   198,     0,     0,     0,   209,     0,
     210,   499,   500,   501,   502,   211,     0,   503,   504,     0,
     212,   213,   214,     0,   215,   216,   217,   218,   142,   219,
     671,   199,   200,   201,   202,   203,     0,     0,     0,     0,
     143,   144,   145,   146,     0,     0,   147,   148,   149,     0,
       0,     0,     0,   150,     0,     0,   151,     0,     0,     0,
       0,     0,   475,   204,   205,   206,   207,   208,     0,     0,
       0,     0,   212,   209,   214,   210,   215,   216,   217,     0,
     211,   476,     0,     0,     0,   212,   213,   214,     0,   215,
     216,   217,   218,     0,   219,   425,   426,   427,   428,     0,
       0,   429,     0,   430,     0,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,     0,   425,   426,   427,
     428,     0,   593,   429,     0,   430,   461,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   425,   426,   427,
     428,     0,   597,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   425,   426,   427,
     428,     0,   602,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   425,   426,   427,
     428,     0,   654,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   425,   426,   427,
     428,     0,   665,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   425,   426,   427,
     428,     0,   684,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   425,   426,   427,
     428,     0,   699,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   425,   426,   427,
     428,     0,   706,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   425,   426,   427,
     428,     0,     0,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   426,   427,   428,
       0,     0,   429,     0,   430,     0,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   428,     0,     0,   429,
       0,   430,     0,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   429,     0,   430,     0,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   430,     0,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   459,   460,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   459,   460,   245,
       0,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,    71,    72,     0,    73,     0,
       0,     0,     0,     0,   374,   246,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    69,    70,
      71,    72,     0,    73,     0,     0,     0,     0,     0,     0,
     375,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,  -269,  -269,  -269,
    -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   105,     0,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,    69,    70,    71,
      72,   108,    73,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,    69,    70,    71,    72,   255,
      73,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,    71,    72,     0,    73,   143,
     144,   145,   146,   212,     0,   214,   148,   215,   216,   217
};

static const yytype_int16 yycheck[] =
{
      91,   141,   141,     0,     1,   201,   203,    98,    17,     8,
      31,    33,     8,   343,   236,   237,   238,   239,   240,   241,
      33,     6,    98,     8,   221,     6,    11,     8,    20,    79,
      11,     8,     8,   107,   125,    63,    71,     9,   117,   113,
       1,   115,    68,    33,    15,    80,    17,    18,    40,   125,
      42,   142,   107,   132,    63,   159,    27,    28,    93,   133,
     151,   107,   117,     9,     8,   100,   107,   207,   103,   107,
      76,    68,    69,    70,   109,   151,   117,   105,   133,   141,
      77,   185,   107,   105,   134,   107,   132,   135,   126,   129,
     420,    17,   105,   423,   291,   135,   105,   294,   107,   128,
     297,   298,   299,   300,   301,   141,   127,    68,    69,    70,
     107,   133,   121,   122,   123,   105,    77,   107,    81,   132,
      83,    84,    94,   132,   321,   107,   323,   349,   350,   126,
     352,   127,   104,   132,   196,   107,   108,    63,   135,   230,
     231,   232,   339,   125,   206,   342,   107,   132,    94,   130,
     347,   128,   128,   128,   230,   231,   232,   128,   104,   135,
     196,   107,   108,   107,   136,   126,   127,   102,   204,   135,
     206,   207,   107,   165,   135,   113,   113,   115,   115,   105,
     115,   107,   174,    32,    33,   128,   121,    36,    37,   132,
     136,    73,   228,    75,   132,   121,   122,   123,   133,   117,
     236,   237,   238,   239,   240,   241,   132,    85,    86,   406,
     407,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   419,     8,   421,   422,   134,    17,   425,   426,
     427,   428,   429,   430,   431,   432,   433,   434,   435,   436,
     437,   438,   439,   440,   441,   442,   443,   444,   445,   446,
     447,   448,   449,   450,   451,   452,   453,   454,   455,   456,
     457,   458,   459,   460,   256,   461,   129,   128,   330,   132,
     467,   132,   135,    63,   496,   497,   498,   499,   500,   501,
     502,   503,   504,     6,    10,     8,    12,    72,    11,    74,
     107,   488,    11,    11,   330,    89,     7,   132,   334,    10,
     135,    12,    87,    88,    30,    31,    32,    33,   130,   131,
      36,    37,    11,   349,   350,   105,   352,   107,   129,    30,
      31,    32,    33,   128,   128,    36,    37,   132,   132,   391,
     135,   121,   122,   123,    90,    91,    92,    36,    11,    81,
      82,    83,    84,   135,    63,    63,     3,     4,     5,     6,
       8,   128,     9,    11,    11,   391,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,   105,   105,   107,    11,
      63,   128,   107,   615,   616,   110,   128,   594,   117,   596,
     119,   128,   121,   122,   123,     8,   105,   126,   107,    36,
      37,    38,    39,    40,    41,    42,   135,   104,   117,   130,
     119,   128,   121,   122,   123,   128,   107,   126,     8,   110,
     130,   131,   105,   128,   107,     8,   633,   105,   635,   636,
     129,    63,   130,   131,   117,   107,   119,     8,   121,   122,
     123,   648,   132,   126,   130,   131,   128,     8,   128,   656,
     496,   497,   498,   499,   500,   501,   502,   503,   504,   130,
     131,    36,    37,    48,    49,   107,   133,   617,   617,   129,
      71,   129,   129,   105,   129,   107,   105,    78,   105,    80,
     127,     7,     8,   128,    10,   117,    12,   119,     8,   121,
     122,   123,    93,   700,   126,   101,   105,   117,     8,   100,
     105,     8,   103,   135,    30,    31,    32,    33,   109,     8,
      36,    37,   105,   614,   132,   129,   105,   107,   590,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   105,
     120,   121,   122,   123,   124,   105,   126,   121,   135,   130,
     132,   129,   128,    87,   590,   617,   107,   129,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   105,   120,
     121,   122,   123,   124,   107,   126,   134,   131,   614,   615,
     616,   617,     3,     4,     5,     6,   131,   131,     9,   131,
      11,   105,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     3,     4,     5,     6,    18,    18,     9,   109,
      11,   105,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     3,     4,     5,     6,   132,   129,     9,     8,
      11,     8,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,   129,     3,     4,     5,     6,   129,   128,     9,
     129,    11,   133,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     3,     4,     5,     6,   129,   105,     9,
     129,    11,   133,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,   129,   129,     3,     4,     5,     6,   130,
     131,     9,   135,    11,   129,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,   129,     3,     4,     5,     6,
     129,   131,     9,   129,    11,   129,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     129,   131,     9,   129,    11,   129,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     129,   131,     9,   131,    11,   129,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
       8,   105,     9,   130,    11,     8,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
       8,   105,     9,   130,    11,   105,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     105,   105,     9,   130,    11,    11,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     105,   105,     9,   130,    11,   105,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     105,   105,     9,   130,    11,   128,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     105,   133,     9,   130,    11,   109,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     105,   105,     9,   130,    11,   105,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     130,   105,     9,   130,    11,   105,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
       8,   130,     9,   130,    11,     8,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     128,   131,     9,   130,    11,   128,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     129,   131,     9,   130,    11,   128,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     105,   130,     9,   130,    11,   130,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     128,   130,     9,   130,    11,   130,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     135,   133,     9,   130,    11,     8,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
       8,   130,     9,   130,    11,     8,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     7,     8,     8,    10,
     107,    12,    33,   130,   105,   131,     8,    11,     8,     8,
     117,     8,   119,    44,   121,   122,   123,     8,   133,    30,
      31,    32,    33,   105,   105,    36,    37,   105,   131,    33,
      34,   131,    36,     8,    30,    30,   133,   133,   128,    43,
      44,    45,    46,   130,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
     113,   114,   115,   116,   115,   115,   119,   120,   121,   130,
     130,    31,     8,   126,    11,   106,   129,     8,    31,   115,
      17,   128,   113,   114,   115,   116,   117,   242,   119,   120,
     121,   122,   123,   142,    31,   617,   362,   511,   129,    36,
     334,   105,   463,   107,    30,    31,    32,    33,   112,    36,
      36,    37,   622,   117,   118,   119,    12,   121,   122,   123,
      -1,    -1,   126,    -1,    -1,   129,    63,    64,    65,    66,
      67,    -1,    -1,    -1,    30,    31,    32,    33,    -1,    11,
      36,    37,    -1,    -1,    -1,    17,    -1,    -1,     7,    -1,
      -1,    10,    -1,    12,    -1,    -1,    -1,    -1,    95,    96,
      97,    -1,    99,    -1,    36,    -1,    -1,    -1,   105,    -1,
     107,    30,    31,    32,    33,   112,    -1,    36,    37,    -1,
     117,   118,   119,    -1,   121,   122,   123,   124,   101,   126,
     127,    63,    64,    65,    66,    67,    -1,    -1,    -1,    -1,
     113,   114,   115,   116,    -1,    -1,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,    -1,    -1,   129,    -1,    -1,    -1,
      -1,    -1,   107,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,   117,   105,   119,   107,   121,   122,   123,    -1,
     112,   126,    -1,    -1,    -1,   117,   118,   119,    -1,   121,
     122,   123,   124,    -1,   126,     3,     4,     5,     6,    -1,
      -1,     9,    -1,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    -1,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    64,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,    -1,    -1,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     4,     5,     6,
      -1,    -1,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     6,    -1,    -1,     9,
      -1,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     9,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,   107,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,   120,   121,   122,   123,   124,    -1,   126,    -1,
      -1,    -1,    -1,    -1,   107,   133,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,    -1,   120,   121,   122,
     123,   124,    -1,   126,    -1,    -1,    -1,    -1,    -1,    -1,
     133,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,   107,    -1,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    -1,   120,   121,   122,   123,
     124,   107,   126,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,   120,   121,   122,   123,   124,   107,
     126,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,   120,   121,   122,   123,   124,    -1,   126,   113,
     114,   115,   116,   117,    -1,   119,   120,   121,   122,   123
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    68,   138,   141,    76,     0,     1,    68,    69,    70,
      77,   107,   126,   135,   139,   140,   142,   143,   144,   145,
     146,   149,   150,   151,   152,   155,   156,   157,   158,   159,
     160,   161,   162,   165,   167,   168,   169,   128,     8,   127,
      71,    78,    80,    93,   100,   103,   109,    90,    91,    92,
     107,   107,   126,   163,   135,   135,     8,   107,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   120,   121,
     122,   123,   124,   126,   147,   207,     8,   107,   148,   207,
       8,    72,    74,    87,    88,    68,   127,   139,   140,   134,
       9,    94,   104,   107,   108,   136,   194,     9,    94,   107,
     108,   136,   194,    85,    86,   107,   207,   128,   107,   207,
     107,   132,   197,   117,   107,   153,   154,    89,   113,   115,
     132,   201,   201,   201,   129,   135,   128,    81,    83,    84,
     128,   128,    81,    82,    83,    84,   128,   128,   128,   128,
     181,   182,   101,   113,   114,   115,   116,   119,   120,   121,
     126,   129,   171,   172,   173,   174,   175,   192,   104,   171,
       8,     8,   105,   129,   107,   198,     8,   128,     8,   154,
     128,   113,   115,   202,   132,   199,    33,   105,   132,   195,
     117,   132,   203,   107,   206,   164,   171,   129,   129,   129,
     129,    73,    75,   105,   105,   127,    11,    17,    36,    63,
      64,    65,    66,    67,    95,    96,    97,    98,    99,   105,
     107,   112,   117,   118,   119,   121,   122,   123,   124,   126,
     183,   185,   187,   188,   189,   190,   193,   173,   128,   171,
       6,     8,    11,   132,   176,   101,    15,    17,    18,    27,
      28,   128,   176,     8,   105,   107,   133,   207,   117,    63,
     105,   107,   113,   115,   133,   107,   200,   207,     8,   105,
      33,   105,   107,   196,     8,   107,   117,   204,     8,   130,
     131,   176,   105,   105,   105,   105,   107,   126,   135,   187,
     193,   107,   125,   132,   135,    63,   105,   107,   121,   122,
     123,   132,   186,   190,   132,   186,     8,   132,    33,    34,
      43,    44,    45,    46,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,   112,
     118,   129,   184,   185,   187,   193,   107,   193,   129,   187,
      11,   105,   107,   126,   185,   191,   193,   121,   135,   128,
     135,   129,   128,   129,   184,   129,   135,   128,   132,    33,
      44,   106,   129,   170,   192,   193,   130,   171,   171,   171,
     177,    79,   134,   166,   170,   170,   170,   170,   170,   170,
     166,   130,   132,    87,   107,   133,   207,   105,    33,   105,
     107,   133,   107,   117,   133,   107,   134,   131,   131,   131,
     131,   105,    18,    18,   109,   105,   132,   184,     8,   184,
       8,   184,   184,   184,   184,   184,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   184,     3,     4,     5,     6,     9,
      11,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    64,   184,   129,   132,   135,     8,   128,   135,   128,
     105,   131,   135,   187,   135,   107,   126,   189,   193,     8,
       8,   105,   184,   105,   184,   205,   184,   205,   128,   105,
     105,   184,   105,   170,   170,   170,     7,    10,    12,    30,
      31,    32,    33,    36,    37,   102,   107,   115,   121,   133,
     179,   180,   181,   105,   128,   105,   105,   105,   105,   105,
     187,   133,   109,   133,   133,   133,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     205,   184,   184,   205,   130,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   186,   188,   105,   105,   184,   105,   130,   107,   110,
     105,   105,     8,     8,   128,   130,   131,     8,   130,   184,
     130,   128,     8,   131,   130,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   129,   128,   128,   182,    31,   127,
     178,   131,   105,   130,   130,   130,   130,   135,   133,     8,
       8,     8,   130,   131,   130,   131,   131,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   131,   130,
     130,     8,   130,   131,     8,     8,   128,     8,     8,     8,
     187,   184,     8,   184,     8,     8,   105,   192,   193,   170,
     170,   127,   178,   107,   110,   105,   197,   105,   184,   184,
     184,   184,   105,   184,     8,   133,   131,   131,     8,     8,
      30,    30,   133,     8,   130,   130,   130,   130,   133,     8,
     128,   115,   115,   184,   130,   130,     8,     8,     8
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
        case 19:

/* Line 1455 of yacc.c  */
#line 182 "slghparse.y"
    { SleighCompile_resetConstructor(slgh); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 184 "slghparse.y"
    { SleighCompile_setEndian(slgh, 1); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 185 "slghparse.y"
    { SleighCompile_setEndian(slgh, 0); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 187 "slghparse.y"
    { SleighCompile_setAlignment(slgh, (int)(yyvsp[(4) - (5)].i)); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 189 "slghparse.y"
    {;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 191 "slghparse.y"
    { (yyval.tokensym) = SleighCompile_defineToken(slgh, (yyvsp[(3) - (6)].str)->data, (yyvsp[(5) - (6)].i)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 192 "slghparse.y"
    { (yyval.tokensym) = (yyvsp[(1) - (2)].tokensym); SleighCompile_addTokenField(slgh, (yyvsp[(1) - (2)].tokensym), (yyvsp[(2) - (2)].fieldqual)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 193 "slghparse.y"
    { yyerror("%s: redefined as a token", SleighSymbol_getName((yyvsp[(3) - (3)].anysym))); YYERROR; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 195 "slghparse.y"
    {;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 197 "slghparse.y"
    { (yyval.varsym) = (yyvsp[(3) - (3)].varsym); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 198 "slghparse.y"
    { (yyval.varsym) = (yyvsp[(1) - (2)].varsym); if (!SleighCompile_addContextField(slgh, (yyvsp[(1) - (2)].varsym), (yyvsp[(2) - (2)].fieldqual)))
                                            { yyerror("All context definitions must come before constructors"); YYERROR; } ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 201 "slghparse.y"
    { (yyval.fieldqual) = FieldQuality_new((yyvsp[(1) - (7)].str)->data,(yyvsp[(4) - (7)].i),(yyvsp[(6) - (7)].i)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 202 "slghparse.y"
    { yyerror("%s: redefined as field", SleighSymbol_getName((yyvsp[(1) - (7)].anysym))); YYERROR; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 203 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->signext = true; ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 204 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->hex = true; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 205 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->hex = false; ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 207 "slghparse.y"
    { (yyval.fieldqual) = FieldQuality_new((yyvsp[(1) - (7)].str)->data,(yyvsp[(4) - (7)].i),(yyvsp[(6) - (7)].i)); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 208 "slghparse.y"
    { yyerror("%s: redefined as field", SleighSymbol_getName((yyvsp[(1) - (7)].anysym))); YYERROR; ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 209 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->signext = true; ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 210 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->flow = false; ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 211 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->hex = true; ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 212 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->hex = false; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 214 "slghparse.y"
    { SleighCompile_newSpace(slgh, (yyvsp[(1) - (2)].spacequal)); SpaceQuality_delete((yyvsp[(1) - (2)].spacequal)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 216 "slghparse.y"
    { (yyval.spacequal) = SpaceQuality_new((yyvsp[(3) - (3)].str)->data); cstr_delete((yyvsp[(3) - (3)].str)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 217 "slghparse.y"
    { yyerror("%s: redefined as space", SleighSymbol_getName((yyvsp[(3) - (3)].anysym))); YYERROR; ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 218 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (4)].spacequal); (yyval.spacequal)->type = RAMTYPE; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 219 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (4)].spacequal); (yyval.spacequal)->type = REGISTERTYPE; ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 220 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (4)].spacequal); (yyval.spacequal)->size = (uint4)(yyvsp[(4) - (4)].i); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 221 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (4)].spacequal); (yyval.spacequal)->wordsize = (uint4)(yyvsp[(4) - (4)].i); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 222 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (2)].spacequal); (yyval.spacequal)->isdefault = true; ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 224 "slghparse.y"
    {
               SleighCompile_defineVarnodes(slgh, (yyvsp[(2) - (10)].spacesym), (yyvsp[(5) - (10)].i), (yyvsp[(8) - (10)].i), (yyvsp[(9) - (10)].strlist)); dynarray_delete((yyvsp[(9) - (10)].strlist)); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 226 "slghparse.y"
    { yyerror("Parsed integer is too big (overflow)"); YYERROR; ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 233 "slghparse.y"
    {
               SleighCompile_defineBitrange(slgh, (yyvsp[(1) - (8)].str)->data,(yyvsp[(3) - (8)].varsym),(uint4)(yyvsp[(5) - (8)].i),(uint4)(yyvsp[(7) - (8)].i)); 
               ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 237 "slghparse.y"
    { SleighCompile_addUserOp(slgh, (yyvsp[(3) - (4)].strlist)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 239 "slghparse.y"
    { SleighCompile_attachValues(slgh, (yyvsp[(3) - (5)].symlist), (yyvsp[(4) - (5)].biglist)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 241 "slghparse.y"
    { SleighCompile_attachNames(slgh, (yyvsp[(3) - (5)].symlist), (yyvsp[(4) - (5)].strlist)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 243 "slghparse.y"
    { SleighCompile_attachVarnodes(slgh, (yyvsp[(3) - (5)].symlist), (yyvsp[(4) - (5)].symlist)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 245 "slghparse.y"
    {  SleighCompile_buildMacro(slgh, (yyvsp[(1) - (4)].macrosym), (yyvsp[(3) - (4)].sem)); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 248 "slghparse.y"
    {  SleighCompile_pushWith(slgh, (yyvsp[(2) - (6)].subtablesym), (yyvsp[(4) - (6)].pateq), (yyvsp[(5) - (6)].contop)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 254 "slghparse.y"
    { SleighCompile_popWith(slgh); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 256 "slghparse.y"
    { (yyval.subtablesym) = (SubtableSymbol *)0; ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 257 "slghparse.y"
    { (yyval.subtablesym) = (yyvsp[(1) - (1)].subtablesym); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 258 "slghparse.y"
    { (yyval.subtablesym) = SleighCompile_newTable(slgh, (yyvsp[(1) - (1)].str)->data); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 261 "slghparse.y"
    { (yyval.pateq) = (PatternEquation *)0; ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 262 "slghparse.y"
    { (yyval.pateq) = (yyvsp[(1) - (1)].pateq); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 265 "slghparse.y"
    { (yyval.macrosym) = SleighCompile_createMacro(slgh, (yyvsp[(2) - (5)].str)->data, (yyvsp[(4) - (5)].strlist)); dynarray_delete((yyvsp[(4) - (5)].strlist)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 267 "slghparse.y"
    { (yyval.sectionstart) = SleighCompile_standaloneSection(slgh, (yyvsp[(2) - (3)].sem)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 268 "slghparse.y"
    { (yyval.sectionstart) = SleighCompile_finalNamedSection(slgh, (yyvsp[(2) - (4)].sectionstart), (yyvsp[(3) - (4)].sem)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 269 "slghparse.y"
    { (yyval.sectionstart) = (SectionVector *)0; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 271 "slghparse.y"
    { SleighCompile_buildConstructor(slgh, (yyvsp[(1) - (5)].construct), (yyvsp[(3) - (5)].pateq), (yyvsp[(4) - (5)].contop), (yyvsp[(5) - (5)].sectionstart)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 272 "slghparse.y"
    { SleighCompile_buildConstructor(slgh, (yyvsp[(1) - (5)].construct), (yyvsp[(3) - (5)].pateq), (yyvsp[(4) - (5)].contop), (yyvsp[(5) - (5)].sectionstart)); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 274 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); Constructor_addSyntax((yyval.construct), (yyvsp[(2) - (2)].str)->data); cstr_delete((yyvsp[(2) - (2)].str)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 275 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); Constructor_addSyntax((yyval.construct), (yyvsp[(2) - (2)].str)->data); cstr_delete((yyvsp[(2) - (2)].str)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 276 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); if (SleighCompile_isInRoot(slgh, (yyvsp[(1) - (2)].construct))) { 
                                        Constructor_addSyntax((yyval.construct), (yyvsp[(2) - (2)].str)->data); cstr_delete((yyvsp[(2) - (2)].str)); } 
                                      else SleighCompile_newOperand(slgh, (yyvsp[(1) - (2)].construct),(yyvsp[(2) - (2)].str)->data); 
                                      ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 280 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); if (!SleighCompile_isInRoot(slgh, (yyvsp[(1) - (2)].construct))) { yyerror("Unexpected '^' at start of print pieces");  YYERROR; } ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 281 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 282 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); Constructor_addSyntax((yyval.construct), (yyvsp[(2) - (2)].str)->data); cstr_delete((yyvsp[(2) - (2)].str)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 283 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); Constructor_addSyntax((yyval.construct), (yyvsp[(2) - (2)].str)->data); cstr_delete((yyvsp[(2) - (2)].str)); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 284 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); Constructor_addSyntax((yyval.construct), " "); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 285 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); SleighCompile_newOperand(slgh, (yyvsp[(1) - (2)].construct), (yyvsp[(2) - (2)].str)->data); cstr_delete((yyvsp[(2) - (2)].str)); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 287 "slghparse.y"
    { (yyval.construct) = SleighCompile_createConstructor(slgh, (yyvsp[(1) - (2)].subtablesym)); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 288 "slghparse.y"
    { SubtableSymbol *sym= SleighCompile_newTable(slgh, (yyvsp[(1) - (2)].str)->data); 
                          (yyval.construct) = SleighCompile_createConstructor(slgh, sym);  cstr_delete((yyvsp[(1) - (2)].str)); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 290 "slghparse.y"
    { (yyval.construct) = SleighCompile_createConstructor(slgh, (SubtableSymbol *)0); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 291 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 293 "slghparse.y"
    { (yyval.patexp) = ConstantValue_newB((yyvsp[(1) - (1)].big));  ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 295 "slghparse.y"
    { if ((actionon==1)&&((yyvsp[(1) - (1)].famsym)->type != context_symbol))
                                             { yyerror("Global symbol %s is not allowed in action expression", (yyvsp[(1) - (1)].famsym)->name); } 
                                             (yyval.patexp) = (yyvsp[(1) - (1)].famsym)->value.patval; ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 299 "slghparse.y"
    { (yyval.patexp) = SleighSymbol_getPatternExpression((yyvsp[(1) - (1)].specsym)); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 300 "slghparse.y"
    { (yyval.patexp) = (yyvsp[(2) - (3)].patexp); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 301 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_plusExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 302 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_subExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 303 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_multExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 304 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_leftShiftExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 305 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_rightShiftExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 306 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_andExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 307 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_orExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 308 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_xorExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 309 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_divExp, (yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 310 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_minusExp, (yyvsp[(2) - (2)].patexp)); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 311 "slghparse.y"
    { (yyval.patexp) = PatternExpression_new(a_notExp, (yyvsp[(2) - (2)].patexp)); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 314 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_andEq, (yyvsp[(1) - (3)].pateq),(yyvsp[(3) - (3)].pateq)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 315 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_orEq, (yyvsp[(1) - (3)].pateq),(yyvsp[(3) - (3)].pateq)); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 316 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_catEq, (yyvsp[(1) - (3)].pateq),(yyvsp[(3) - (3)].pateq)); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 318 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_leftEllipsisEq, (yyvsp[(2) - (2)].pateq)); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 321 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_rightEllipsisEq, (yyvsp[(1) - (2)].pateq)); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 325 "slghparse.y"
    { (yyval.pateq) = (yyvsp[(2) - (3)].pateq); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 327 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_equalEq, SleighSymbol_getPatternValue((yyvsp[(1) - (3)].famsym)), (yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 328 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_notEqualEq, SleighSymbol_getPatternValue((yyvsp[(1) - (3)].famsym)),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 329 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_lessEq, SleighSymbol_getPatternValue((yyvsp[(1) - (3)].famsym)),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 330 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_lessEqualEq, SleighSymbol_getPatternValue((yyvsp[(1) - (3)].famsym)),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 331 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_greaterEq, SleighSymbol_getPatternValue((yyvsp[(1) - (3)].famsym)),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 332 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_greaterEqualEq, SleighSymbol_getPatternValue((yyvsp[(1) - (3)].famsym)),(yyvsp[(3) - (3)].patexp)); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 333 "slghparse.y"
    { (yyval.pateq) = SleighCompile_constrainOperand(slgh, (yyvsp[(1) - (3)].operandsym), (yyvsp[(3) - (3)].patexp));
                                          if ((yyval.pateq) == (PatternEquation *)0) 
                                            { yyerror("Constraining currently undefined operand %s", SleighSymbol_getName((yyvsp[(1) - (3)].operandsym))); } ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 336 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_operandEq, OperandSymbol_getIndex((yyvsp[(1) - (1)].operandsym))); SleighCompile_selfDefine(slgh, (yyvsp[(1) - (1)].operandsym)); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 337 "slghparse.y"
    { (yyval.pateq) = PatternEquation_new(a_unconstrainedEq, SleighSymbol_getPatternExpression((yyvsp[(1) - (1)].specsym))); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 338 "slghparse.y"
    { (yyval.pateq) = SleighCompile_defineInvisibleOperand(slgh, (yyvsp[(1) - (1)].famsym)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 339 "slghparse.y"
    { (yyval.pateq) = SleighCompile_defineInvisibleOperand(slgh, (yyvsp[(1) - (1)].subtablesym)); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 341 "slghparse.y"
    { (yyval.contop) = (struct dynarray *)0; ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 342 "slghparse.y"
    { (yyval.contop) = (yyvsp[(2) - (3)].contop); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 344 "slghparse.y"
    { (yyval.contop) = dynarray_new(NULL, NULL); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 345 "slghparse.y"
    { (yyval.contop) = (yyvsp[(1) - (5)].contop); if (!SleighCompile_contextMod(slgh, (yyvsp[(1) - (5)].contop),(yyvsp[(2) - (5)].contextsym),(yyvsp[(4) - (5)].patexp))) {
      yyerror("Cannot use 'inst_next' to set context variable: %s", SleighSymbol_getName((yyvsp[(2) - (5)].contextsym))); YYERROR; } ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 347 "slghparse.y"
    { (yyval.contop) = (yyvsp[(1) - (8)].contop); SleighCompile_contextSet(slgh, (yyvsp[(1) - (8)].contop),(yyvsp[(4) - (8)].famsym),(yyvsp[(6) - (8)].contextsym)); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 348 "slghparse.y"
    { (yyval.contop) = (yyvsp[(1) - (8)].contop); SleighCompile_contextSet(slgh, (yyvsp[(1) - (8)].contop),(yyvsp[(4) - (8)].specsym),(yyvsp[(6) - (8)].contextsym)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 349 "slghparse.y"
    { (yyval.contop) = (yyvsp[(1) - (5)].contop); SleighCompile_defineOperand(slgh, (yyvsp[(2) - (5)].operandsym),(yyvsp[(4) - (5)].patexp)); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 350 "slghparse.y"
    { yyerror("Expecting context symbol, not %s", *(yyvsp[(2) - (2)].str)); YYERROR; ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 352 "slghparse.y"
    { (yyval.sectionsym) = SleighCompile_newSectionSymbol( slgh, (yyvsp[(2) - (3)].str)->data ); cstr_delete((yyvsp[(2) - (3)].str)); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 353 "slghparse.y"
    { (yyval.sectionsym) = (yyvsp[(2) - (3)].sectionsym); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 355 "slghparse.y"
    { (yyval.sectionstart) = SleighCompile_firstNamedSection(slgh, (yyvsp[(1) - (2)].sem),(yyvsp[(2) - (2)].sectionsym)); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 357 "slghparse.y"
    { (yyval.sectionstart) = (yyvsp[(1) - (1)].sectionstart); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 358 "slghparse.y"
    { (yyval.sectionstart) = SleighCompile_nextNamedSection(slgh, (yyvsp[(1) - (3)].sectionstart),(yyvsp[(2) - (3)].sem),(yyvsp[(3) - (3)].sectionsym)); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 360 "slghparse.y"
    { (yyval.sem) = (yyvsp[(1) - (1)].sem); if (!ConstructTpl_getOpvec((yyvsp[(1) - (1)].sem))->len && (ConstructTpl_getResult((yyval.sem)) == (HandleTpl *)0)) SleighCompile_recordNop(slgh); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 361 "slghparse.y"
    { (yyval.sem) = SleighCompile_setResultVarnode(slgh, (yyvsp[(1) - (4)].sem),(yyvsp[(3) - (4)].varnode)); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 362 "slghparse.y"
    { (yyval.sem) = SleighCompile_setResultStarVarnode(slgh, (yyvsp[(1) - (5)].sem),(yyvsp[(3) - (5)].starqual),(yyvsp[(4) - (5)].varnode)); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 363 "slghparse.y"
    { yyerror("Unknown export varnode: %s", *(yyvsp[(3) - (3)].str)); YYERROR; ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 364 "slghparse.y"
    { yyerror("Unknown pointer varnode: %s", *(yyvsp[(4) - (4)].str)); vm_free((yyvsp[(3) - (4)].starqual)); vm_free((yyvsp[(4) - (4)].str)); YYERROR; ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 366 "slghparse.y"
    { (yyval.sem) = ConstructTpl_new(); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 367 "slghparse.y"
    { (yyval.sem) = (yyvsp[(1) - (2)].sem); if (!ConstructTpl_addOpList((yyval.sem), (yyvsp[(2) - (2)].stmt))) { vm_free((yyvsp[(2) - (2)].stmt)); yyerror("Multiple delayslot declarations"); YYERROR; } dynarray_delete((yyvsp[(2) - (2)].stmt)); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 368 "slghparse.y"
    { (yyval.sem) = (yyvsp[(1) - (4)].sem); PcodeCompile_newLocalDefinition(slgh->pcode, (yyvsp[(3) - (4)].str)->data, 0); cstr_delete((yyvsp[(3) - (4)].str)); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 369 "slghparse.y"
    { (yyval.sem) = (yyvsp[(1) - (6)].sem); PcodeCompile_newLocalDefinition(slgh->pcode, (yyvsp[(3) - (6)].str)->data, (uint32_t)(yyvsp[(5) - (6)].i));  ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 371 "slghparse.y"
    { ExpTree_setOutput((yyvsp[(3) - (4)].tree), (yyvsp[(1) - (4)].varnode)); (yyval.stmt) = ExpTree_toVector((yyvsp[(3) - (4)].tree)); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 372 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_newOutput(slgh->pcode, true, (yyvsp[(4) - (5)].tree), (yyvsp[(2) - (5)].str)->data, 0); cstr_free((yyvsp[(2) - (5)].str)); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 373 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_newOutput(slgh->pcode, false,(yyvsp[(3) - (4)].tree),(yyvsp[(1) - (4)].str)->data, 0); ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 374 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_newOutput(slgh->pcode, true,(yyvsp[(6) - (7)].tree), (yyvsp[(2) - (7)].str)->data, (u4)(yyvsp[(4) - (7)].i)); cstr_delete((yyvsp[(2) - (7)].str)); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 375 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_newOutput(slgh->pcode, true,(yyvsp[(5) - (6)].tree),(yyvsp[(1) - (6)].str)->data,(u4)(yyvsp[(3) - (6)].i)); ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 376 "slghparse.y"
    { (yyval.stmt) = (struct dynarray *)0; yyerror("Redefinition of symbol: %s", SleighSymbol_getName((yyvsp[(2) - (3)].specsym))); YYERROR; ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 377 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createStore(slgh->pcode, (yyvsp[(1) - (5)].starqual),(yyvsp[(2) - (5)].tree),(yyvsp[(4) - (5)].tree)); StarQuality_delete((yyvsp[(1) - (5)].starqual)); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 378 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createUserOpNoOut(slgh->pcode, (yyvsp[(1) - (5)].useropsym),(yyvsp[(3) - (5)].param)); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 379 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_assignBitRange(slgh->pcode, (yyvsp[(1) - (9)].varnode),(uint4)(yyvsp[(3) - (9)].i),(uint4)(yyvsp[(5) - (9)].i),(yyvsp[(8) - (9)].tree)); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 380 "slghparse.y"
    { (yyval.stmt)= PcodeCompile_assignBitRange(slgh->pcode, 
                                          SleighSymbol_getVarnode (BitrangeSymbol_getParentSymbol((yyvsp[(1) - (4)].bitsym))), 
                                          BitrangeSymbol_getBitOffset((yyvsp[(1) - (4)].bitsym)),
                                          BitrangeSymbol_numBits((yyvsp[(1) - (4)].bitsym)),(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 384 "slghparse.y"
    { VarnodeTpl_delete((yyvsp[(1) - (4)].varnode)); yyerror("Illegal truncation on left-hand side of assignment"); YYERROR; ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 385 "slghparse.y"
    { VarnodeTpl_delete((yyvsp[(1) - (4)].varnode)); yyerror("Illegal subpiece on left-hand side of assignment"); YYERROR; ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 386 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createOpConst(slgh->pcode, BUILD, OperandSymbol_getIndex((yyvsp[(2) - (3)].operandsym))); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 387 "slghparse.y"
    { (yyval.stmt) = SleighCompile_createCrossBuild(slgh, (yyvsp[(2) - (5)].varnode),(yyvsp[(4) - (5)].sectionsym)); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 388 "slghparse.y"
    { (yyval.stmt) = SleighCompile_createCrossBuild(slgh, (yyvsp[(2) - (5)].varnode), SleighCompile_newSectionSymbol(slgh, (yyvsp[(4) - (5)].str)->data)); cstr_delete((yyvsp[(4) - (5)].str)); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 389 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createOpConst(slgh->pcode, DELAY_SLOT,(yyvsp[(3) - (5)].i)); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 390 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_BRANCH,ExpTree_newV((yyvsp[(2) - (3)].varnode))); ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 391 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createOpNoOut2(slgh->pcode, CPUI_CBRANCH,ExpTree_newV((yyvsp[(4) - (5)].varnode)),(yyvsp[(2) - (5)].tree)); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 392 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_BRANCHIND,(yyvsp[(3) - (5)].tree)); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 393 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_CALL,ExpTree_newV((yyvsp[(2) - (3)].varnode))); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 394 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_CALLIND,(yyvsp[(3) - (5)].tree)); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 395 "slghparse.y"
    { yyerror("Must specify an indirect parameter for return"); YYERROR; ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 396 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_RETURN,(yyvsp[(3) - (5)].tree)); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 397 "slghparse.y"
    { (yyval.stmt) = SleighCompile_createMacroUse(slgh, (yyvsp[(1) - (5)].macrosym),(yyvsp[(3) - (5)].param)); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 398 "slghparse.y"
    { (yyval.stmt) = PcodeCompile_placeLabel(slgh->pcode, (yyvsp[(1) - (1)].labelsym)); ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 400 "slghparse.y"
    { (yyval.tree) = ExpTree_newV((yyvsp[(1) - (1)].varnode)); ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 401 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createLoad(slgh->pcode, (yyvsp[(1) - (2)].starqual),(yyvsp[(2) - (2)].tree)); StarQuality_delete((yyvsp[(1) - (2)].starqual)); ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 402 "slghparse.y"
    { (yyval.tree) = (yyvsp[(2) - (3)].tree); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 403 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_ADD,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 404 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SUB,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 405 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_EQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 406 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_NOTEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 407 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 408 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 409 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 410 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 411 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SLESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 412 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SLESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 413 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SLESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 414 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SLESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 415 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_INT_2COMP,(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 416 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_INT_NEGATE,(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 417 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_XOR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 418 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_AND,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 419 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_OR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 420 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LEFT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 421 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_RIGHT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 422 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SRIGHT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 423 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_MULT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 424 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_DIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 425 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SDIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 426 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_REM,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 427 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SREM,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 428 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_BOOL_NEGATE,(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 429 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_BOOL_XOR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 430 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_BOOL_AND,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 431 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_BOOL_OR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 432 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_EQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 433 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_NOTEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 434 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_LESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 435 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_LESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 436 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_LESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 437 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_LESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 438 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_ADD,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 439 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_SUB,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 440 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_MULT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 441 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_DIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 442 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_NEG,(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 443 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_ABS,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 444 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_SQRT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 445 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_INT_SEXT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 446 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_INT_ZEXT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 447 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_CARRY,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 448 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SCARRY,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 449 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SBORROW,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 450 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_FLOAT2FLOAT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 451 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_INT2FLOAT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 452 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_NAN,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 453 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_TRUNC,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 454 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_CEIL,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 455 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_FLOOR,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 456 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_ROUND,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 457 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_NEW,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 458 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_NEW,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 459 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp(slgh->pcode, CPUI_POPCOUNT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 460 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createOp2(slgh->pcode, CPUI_SUBPIECE,ExpTree_newV(SleighSymbol_getVarnode((yyvsp[(1) - (4)].specsym))),ExpTree_newV((yyvsp[(3) - (4)].varnode))); ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 461 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createBitRange(slgh->pcode, (yyvsp[(1) - (3)].specsym),0,(u4)((yyvsp[(3) - (3)].i) * 8)); ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 462 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createBitRange(slgh->pcode, (yyvsp[(1) - (6)].specsym),(uint4)(yyvsp[(3) - (6)].i),(uint4)(yyvsp[(5) - (6)].i)); ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 463 "slghparse.y"
    { (yyval.tree)=PcodeCompile_createBitRange(slgh->pcode, 
                                  BitrangeSymbol_getParentSymbol((yyvsp[(1) - (1)].bitsym)),
                                  BitrangeSymbol_getBitOffset((yyvsp[(1) - (1)].bitsym)),
                                  BitrangeSymbol_numBits((yyvsp[(1) - (1)].bitsym))); ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 467 "slghparse.y"
    { (yyval.tree) = PcodeCompile_createUserOp(slgh->pcode, (yyvsp[(1) - (4)].useropsym),(yyvsp[(3) - (4)].param)); ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 468 "slghparse.y"
    { if ((yyvsp[(3) - (4)].param)->len < 2) { yyerror("Must at least two inputs to cpool");  YYERROR; } (yyval.tree) = PcodeCompile_createVariadic(slgh->pcode, CPUI_CPOOLREF,(yyvsp[(3) - (4)].param)); ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 470 "slghparse.y"
    { (yyval.starqual) = StarQuality_new(); (yyval.starqual)->size = (u4)(yyvsp[(6) - (6)].i); (yyval.starqual)->id=ConstTpl_newA(SleighSymbol_getSpace((yyvsp[(3) - (6)].spacesym))); ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 471 "slghparse.y"
    { (yyval.starqual) = StarQuality_new(); (yyval.starqual)->size = 0; (yyval.starqual)->id=ConstTpl_newA(SleighSymbol_getSpace((yyvsp[(3) - (4)].spacesym))); ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 472 "slghparse.y"
    { (yyval.starqual) = StarQuality_new(); (yyval.starqual)->size = (u4)(yyvsp[(3) - (3)].i); (yyval.starqual)->id=ConstTpl_newA(SleighCompile_getDefaultCodeSpace(slgh)); ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 473 "slghparse.y"
    { (yyval.starqual) = StarQuality_new(); (yyval.starqual)->size = 0; (yyval.starqual)->id=ConstTpl_newA(SleighCompile_getDefaultCodeSpace(slgh)); ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 475 "slghparse.y"
    { VarnodeTpl *sym = SleighSymbol_getVarnode((yyvsp[(1) - (1)].startsym)); 
                      (yyval.varnode) = VarnodeTpl_new(ConstTpl_new1(j_curspace),VarnodeTpl_getOffset(sym), ConstTpl_new1(j_curspace_size)); SleighSymbol_delete((yyvsp[(1) - (1)].startsym)) ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 477 "slghparse.y"
    { VarnodeTpl *sym = SleighSymbol_getVarnode((yyvsp[(1) - (1)].endsym)); 
                    (yyval.varnode) = VarnodeTpl_new(ConstTpl_new1(j_curspace), VarnodeTpl_getOffset(sym),ConstTpl_new1(j_curspace_size)); SleighSymbol_delete((yyvsp[(1) - (1)].endsym)); ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 479 "slghparse.y"
    { (yyval.varnode) = VarnodeTpl_new(ConstTpl_new1(j_curspace),ConstTpl_new2(real,(yyvsp[(1) - (1)].i)),ConstTpl_new1(j_curspace_size));  ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 480 "slghparse.y"
    { (yyval.varnode) = VarnodeTpl_new(ConstTpl_new1(j_curspace),ConstTpl_new2(real,0),ConstTpl_new1(j_curspace_size)); 
                    yyerror("Parsed integer is too big (overflow)"); ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 482 "slghparse.y"
    { (yyval.varnode) = SleighSymbol_getVarnode((yyvsp[(1) - (1)].operandsym)); SleighSymbol_setCodeAddress((yyvsp[(1) - (1)].operandsym)); ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 483 "slghparse.y"
    { AddrSpace *spc = SleighSymbol_getSpace((yyvsp[(3) - (4)].spacesym)); 
                                (yyval.varnode) = VarnodeTpl_new(ConstTpl_newA(spc),ConstTpl_new2(real,(yyvsp[(1) - (4)].i)),ConstTpl_new2(real,AddrSpace_getAddrSize(spc))); ;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 485 "slghparse.y"
    { (yyval.varnode) = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),
                                    ConstTpl_new2(j_relative, LabelSymbol_getIndex((yyvsp[(1) - (1)].labelsym))),ConstTpl_new2(real,sizeof(uintm))); SleighSymbol_incrementRefCount((yyvsp[(1) - (1)].labelsym)); ;}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 487 "slghparse.y"
    { yyerror("Unknown jump destination: %s", *(yyvsp[(1) - (1)].str)); cstr_delete((yyvsp[(1) - (1)].str)); YYERROR; ;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 489 "slghparse.y"
    { (yyval.varnode) = SleighSymbol_getVarnode((yyvsp[(1) - (1)].specsym)); ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 490 "slghparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].varnode); ;}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 491 "slghparse.y"
    { yyerror("Unknown varnode parameter: %s", (yyvsp[(1) - (1)].str)->data); cstr_delete((yyvsp[(1) - (1)].str)); ; YYERROR; ;}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 492 "slghparse.y"
    { yyerror("Subtable not attached to operand: %s", SleighSymbol_getName((yyvsp[(1) - (1)].subtablesym))); ; YYERROR; ;}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 494 "slghparse.y"
    { (yyval.varnode) = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),
                              ConstTpl_new2(real,(yyvsp[(1) - (1)].i)),ConstTpl_new2(real,0)); ;}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 496 "slghparse.y"
    { (yyval.varnode) = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),ConstTpl_new2(real,0),ConstTpl_new2(real,0)); 
                                  yyerror("Parsed integer is too big (overflow)"); ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 498 "slghparse.y"
    { (yyval.varnode) = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),
                                      ConstTpl_new2(real,(yyvsp[(1) - (3)].i)),ConstTpl_new2(real,(yyvsp[(3) - (3)].i))); ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 500 "slghparse.y"
    { (yyval.varnode) = PcodeCompile_addressOf(slgh->pcode, (yyvsp[(2) - (2)].varnode),0); ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 501 "slghparse.y"
    { (yyval.varnode) = PcodeCompile_addressOf(slgh->pcode, (yyvsp[(4) - (4)].varnode),(u4)(yyvsp[(3) - (4)].i)); ;}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 503 "slghparse.y"
    { (yyval.varnode) = SleighSymbol_getVarnode((yyvsp[(1) - (1)].specsym)); ;}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 504 "slghparse.y"
    { yyerror("Unknown assignment varnode: %s", (yyvsp[(1) - (1)].str)->data); cstr_delete((yyvsp[(1) - (1)].str)); YYERROR; ;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 505 "slghparse.y"
    { yyerror("Subtable not attached to operand: %s", SleighSymbol_getName((yyvsp[(1) - (1)].subtablesym))); YYERROR; ;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 507 "slghparse.y"
    { (yyval.labelsym) = (yyvsp[(2) - (3)].labelsym); ;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 508 "slghparse.y"
    { (yyval.labelsym) = PcodeCompile_defineLabel( slgh->pcode, (yyvsp[(2) - (3)].str)->data ); ;}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 510 "slghparse.y"
    { (yyval.varnode) = SleighSymbol_getVarnode((yyvsp[(1) - (1)].specsym)); ;}
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 511 "slghparse.y"
    { (yyval.varnode) = PcodeCompile_addressOf(slgh->pcode, (yyvsp[(2) - (2)].varnode),0); ;}
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 512 "slghparse.y"
    { (yyval.varnode) = PcodeCompile_addressOf(slgh->pcode, (yyvsp[(4) - (4)].varnode),(u4)(yyvsp[(3) - (4)].i)); ;}
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 513 "slghparse.y"
    { (yyval.varnode) = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),
                            ConstTpl_new2(real,(yyvsp[(1) - (3)].i)),ConstTpl_new2(real,(yyvsp[(3) - (3)].i))); ;}
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 515 "slghparse.y"
    { yyerror("Unknown export varnode: %s", (yyvsp[(1) - (1)].str)->data); cstr_delete((yyvsp[(1) - (1)].str));  YYERROR; ;}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 516 "slghparse.y"
    { yyerror("Subtable not attached to operand: %s", SleighSymbol_getName((yyvsp[(1) - (1)].subtablesym))); YYERROR; ;}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 518 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].valuesym); ;}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 519 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].valuemapsym); ;}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 520 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].contextsym); ;}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 521 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].namesym); ;}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 522 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].varlistsym); ;}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 524 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].varsym); ;}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 525 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].specsym); ;}
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 526 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].operandsym); ;}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 527 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].startsym); ;}
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 528 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].endsym); ;}
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 530 "slghparse.y"
    { (yyval.str) = cstr_new(NULL, 0); cstr_ccat((yyval.str), (yyvsp[(1) - (1)].ch)); ;}
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 531 "slghparse.y"
    { (yyval.str) = (yyvsp[(1) - (2)].str); cstr_ccat((yyval.str), (yyvsp[(2) - (2)].ch)); ;}
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 533 "slghparse.y"
    { (yyval.biglist) = (yyvsp[(2) - (3)].biglist); ;}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 534 "slghparse.y"
    { (yyval.biglist) = dynarray_new(int64_cmp, int64_delete); dynarray_add((yyval.biglist), int64_new((yyvsp[(1) - (1)].i))); ;}
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 535 "slghparse.y"
    { (yyval.biglist) = dynarray_new(int64_cmp, int64_delete); dynarray_add((yyval.biglist), int64_new(-(yyvsp[(2) - (2)].i))); ;}
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 537 "slghparse.y"
    { (yyval.biglist) = dynarray_new(int64_cmp, int64_delete); dynarray_add((yyval.biglist), int64_new((yyvsp[(1) - (1)].i))); ;}
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 538 "slghparse.y"
    { (yyval.biglist) = dynarray_new(int64_cmp, int64_delete); dynarray_add((yyval.biglist), int64_new(-(yyvsp[(2) - (2)].i))); ;}
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 539 "slghparse.y"
    { if (strcmp ((yyvsp[(1) - (1)].str)->data, "_")) { yyerror("Expecting integer but saw: %s", (yyvsp[(1) - (1)].str)->data); cstr_delete((yyvsp[(1) - (1)].str)); YYERROR; }
                                  (yyval.biglist) = dynarray_new(int64_cmp, int64_delete); dynarray_add((yyval.biglist), int64_new((intb)0xBADBEEF)); cstr_delete((yyvsp[(1) - (1)].str)); ;}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 541 "slghparse.y"
    { (yyval.biglist) = (yyvsp[(1) - (2)].biglist); dynarray_add((yyvsp[(1) - (2)].biglist), int64_new((yyvsp[(2) - (2)].i))); ;}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 542 "slghparse.y"
    { (yyval.biglist) = (yyvsp[(1) - (3)].biglist); dynarray_add((yyvsp[(1) - (3)].biglist), int64_new(-(yyvsp[(3) - (3)].i))); ;}
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 543 "slghparse.y"
    { if (strcmp((yyvsp[(2) - (2)].str)->data, "_")) { yyerror("Expecting integer but saw: %s", (yyvsp[(2) - (2)].str)->data); cstr_delete((yyvsp[(2) - (2)].str));  YYERROR; }
                                  (yyval.biglist) = (yyvsp[(1) - (2)].biglist); dynarray_add((yyvsp[(1) - (2)].biglist), int64_new((intb)0xBADBEEF)); cstr_delete((yyvsp[(2) - (2)].str)); ;}
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 546 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(2) - (3)].strlist); ;}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 547 "slghparse.y"
    { (yyval.strlist) = dynarray_new(NULL, cstr_delete); dynarray_add((yyval.strlist), (yyvsp[(1) - (1)].str)); ;}
    break;

  case 293:

/* Line 1455 of yacc.c  */
#line 549 "slghparse.y"
    { (yyval.strlist) = dynarray_new(NULL, cstr_delete); dynarray_add((yyval.strlist), (yyvsp[(1) - (1)].str) ); ;}
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 550 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(1) - (2)].strlist); dynarray_add((yyval.strlist), (yyvsp[(2) - (2)].str));  ;}
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 551 "slghparse.y"
    { yyerror( "%s redefined", SleighSymbol_getName((yyvsp[(2) - (2)].anysym))); YYERROR; ;}
    break;

  case 296:

/* Line 1455 of yacc.c  */
#line 553 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(2) - (3)].strlist); ;}
    break;

  case 297:

/* Line 1455 of yacc.c  */
#line 555 "slghparse.y"
    { (yyval.strlist) = dynarray_new(NULL, str_free); dynarray_add((yyval.strlist), (yyvsp[(1) - (1)].str)); ;}
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 556 "slghparse.y"
    { (yyval.strlist) = dynarray_new(NULL, NULL); dynarray_add((yyval.strlist), SleighSymbol_getName((yyvsp[(1) - (1)].anysym))); ;}
    break;

  case 299:

/* Line 1455 of yacc.c  */
#line 557 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(1) - (2)].strlist); dynarray_add((yyval.strlist), (yyvsp[(2) - (2)].str)); ;}
    break;

  case 300:

/* Line 1455 of yacc.c  */
#line 558 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(1) - (2)].strlist); dynarray_add((yyval.strlist), SleighSymbol_getName((yyvsp[(2) - (2)].anysym))); ;}
    break;

  case 301:

/* Line 1455 of yacc.c  */
#line 560 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(2) - (3)].symlist); ;}
    break;

  case 302:

/* Line 1455 of yacc.c  */
#line 561 "slghparse.y"
    { (yyval.symlist) = dynarray_new(NULL, NULL); dynarray_add((yyval.symlist), (yyvsp[(1) - (1)].valuesym)); ;}
    break;

  case 303:

/* Line 1455 of yacc.c  */
#line 562 "slghparse.y"
    { (yyval.symlist) = dynarray_new(NULL, NULL); dynarray_add((yyval.symlist), (yyvsp[(1) - (1)].contextsym)); ;}
    break;

  case 304:

/* Line 1455 of yacc.c  */
#line 564 "slghparse.y"
    { (yyval.symlist) = dynarray_new(NULL, NULL); dynarray_add( (yyval.symlist), (yyvsp[(1) - (1)].valuesym) ); ;}
    break;

  case 305:

/* Line 1455 of yacc.c  */
#line 565 "slghparse.y"
    { (yyval.symlist) = dynarray_new(NULL, NULL); dynarray_add((yyval.symlist), (yyvsp[(1) - (1)].contextsym)); ;}
    break;

  case 306:

/* Line 1455 of yacc.c  */
#line 566 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(1) - (2)].symlist); dynarray_add((yyval.symlist), (yyvsp[(2) - (2)].valuesym)); ;}
    break;

  case 307:

/* Line 1455 of yacc.c  */
#line 567 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(1) - (2)].symlist); dynarray_add((yyval.symlist), (yyvsp[(2) - (2)].contextsym)); ;}
    break;

  case 308:

/* Line 1455 of yacc.c  */
#line 568 "slghparse.y"
    { yyerror("%s: is not a value pattern", *(yyvsp[(2) - (2)].str)); vm_free((yyvsp[(2) - (2)].str)); YYERROR; ;}
    break;

  case 309:

/* Line 1455 of yacc.c  */
#line 570 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(2) - (3)].symlist); ;}
    break;

  case 310:

/* Line 1455 of yacc.c  */
#line 571 "slghparse.y"
    { (yyval.symlist) = dynarray_new(NULL, NULL); dynarray_add((yyval.symlist), (yyvsp[(1) - (1)].varsym)); ;}
    break;

  case 311:

/* Line 1455 of yacc.c  */
#line 573 "slghparse.y"
    { (yyval.symlist) = dynarray_new(NULL, NULL); dynarray_add((yyval.symlist), (yyvsp[(1) - (1)].varsym)); ;}
    break;

  case 312:

/* Line 1455 of yacc.c  */
#line 574 "slghparse.y"
    { if (strcmp((yyvsp[(1) - (1)].str)->data, "_")) { yyerror("%s: is not a varnode symbol", (yyvsp[(1) - (1)].str)->data); cstr_delete((yyvsp[(1) - (1)].str)); YYERROR; }
				  (yyval.symlist) = dynarray_new(NULL, NULL); dynarray_add((yyval.symlist), (SleighSymbol *)0); vm_free((yyvsp[(1) - (1)].str)); ;}
    break;

  case 313:

/* Line 1455 of yacc.c  */
#line 576 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(1) - (2)].symlist); dynarray_add((yyval.symlist), (yyvsp[(1) - (2)].symlist)); ;}
    break;

  case 314:

/* Line 1455 of yacc.c  */
#line 577 "slghparse.y"
    { if (strcmp((yyvsp[(2) - (2)].str)->data, "_")) { yyerror("%s: is not a varnode symbol", (yyvsp[(2) - (2)].str)->data); cstr_delete((yyvsp[(2) - (2)].str)); YYERROR; }
                                  (yyval.symlist) = (yyvsp[(1) - (2)].symlist); dynarray_add((yyval.symlist), (SleighSymbol *)0); cstr_delete((yyvsp[(2) - (2)].str)); ;}
    break;

  case 315:

/* Line 1455 of yacc.c  */
#line 580 "slghparse.y"
    { (yyval.param) = dynarray_new(NULL, NULL); ;}
    break;

  case 316:

/* Line 1455 of yacc.c  */
#line 581 "slghparse.y"
    { (yyval.param) = dynarray_new(NULL, NULL); dynarray_add((yyval.param), (yyvsp[(1) - (1)].tree)); ;}
    break;

  case 317:

/* Line 1455 of yacc.c  */
#line 582 "slghparse.y"
    { (yyval.param) = (yyvsp[(1) - (3)].param); dynarray_add((yyval.param), (yyvsp[(3) - (3)].tree)); ;}
    break;

  case 318:

/* Line 1455 of yacc.c  */
#line 584 "slghparse.y"
    { (yyval.strlist) = dynarray_new(NULL, NULL); ;}
    break;

  case 319:

/* Line 1455 of yacc.c  */
#line 585 "slghparse.y"
    { (yyval.strlist) = dynarray_new(NULL, NULL); dynarray_add((yyval.strlist), (yyvsp[(1) - (1)].str)); ;}
    break;

  case 320:

/* Line 1455 of yacc.c  */
#line 586 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(1) - (3)].strlist); dynarray_add((yyval.strlist), (yyvsp[(3) - (3)].str));  ;}
    break;

  case 321:

/* Line 1455 of yacc.c  */
#line 588 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].spacesym); ;}
    break;

  case 322:

/* Line 1455 of yacc.c  */
#line 589 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].sectionsym); ;}
    break;

  case 323:

/* Line 1455 of yacc.c  */
#line 590 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].tokensym); ;}
    break;

  case 324:

/* Line 1455 of yacc.c  */
#line 591 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].useropsym); ;}
    break;

  case 325:

/* Line 1455 of yacc.c  */
#line 592 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].macrosym); ;}
    break;

  case 326:

/* Line 1455 of yacc.c  */
#line 593 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].subtablesym); ;}
    break;

  case 327:

/* Line 1455 of yacc.c  */
#line 594 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].valuesym); ;}
    break;

  case 328:

/* Line 1455 of yacc.c  */
#line 595 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].valuemapsym); ;}
    break;

  case 329:

/* Line 1455 of yacc.c  */
#line 596 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].contextsym); ;}
    break;

  case 330:

/* Line 1455 of yacc.c  */
#line 597 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].namesym); ;}
    break;

  case 331:

/* Line 1455 of yacc.c  */
#line 598 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].varsym); ;}
    break;

  case 332:

/* Line 1455 of yacc.c  */
#line 599 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].varlistsym); ;}
    break;

  case 333:

/* Line 1455 of yacc.c  */
#line 600 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].operandsym); ;}
    break;

  case 334:

/* Line 1455 of yacc.c  */
#line 601 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].startsym); ;}
    break;

  case 335:

/* Line 1455 of yacc.c  */
#line 602 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].endsym); ;}
    break;

  case 336:

/* Line 1455 of yacc.c  */
#line 603 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].bitsym); ;}
    break;



/* Line 1455 of yacc.c  */
#line 4715 "slghparse.tab.c"
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



/* Line 1675 of yacc.c  */
#line 605 "slghparse.y"


int yyerror(const char *s, ...)

{
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "Error(%s:%d):", SleighCompile_grabCurrentFilePath(slgh), SleighCompile_curLineNo(slgh));
  vfprintf(stderr, s, ap);
  va_end(ap);
  exit(1);

  return 0;
}

