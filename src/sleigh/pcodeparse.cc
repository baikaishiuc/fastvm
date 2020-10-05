
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

/* Substitute the variable and function names.  */
#define yyparse         pcodeparse
#define yylex           pcodelex
#define yyerror         pcodeerror
#define yylval          pcodelval
#define yychar          pcodechar
#define yydebug         pcodedebug
#define yynerrs         pcodenerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 16 "pcodeparse.y"

#include "pcodeparse.hh"

  //#define YYERROR_VERBOSE
  extern int yylex(void);
  static PcodeSnippet *pcode;
  extern int yydebug;
  extern int yyerror(const char *str );


/* Line 189 of yacc.c  */
#line 92 "pcodeparse.cc"

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
     OP_BOOL_OR = 258,
     OP_BOOL_XOR = 259,
     OP_BOOL_AND = 260,
     OP_FNOTEQUAL = 261,
     OP_FEQUAL = 262,
     OP_NOTEQUAL = 263,
     OP_EQUAL = 264,
     OP_FGREATEQUAL = 265,
     OP_FLESSEQUAL = 266,
     OP_FGREAT = 267,
     OP_FLESS = 268,
     OP_SGREAT = 269,
     OP_SLESSEQUAL = 270,
     OP_SGREATEQUAL = 271,
     OP_SLESS = 272,
     OP_LESSEQUAL = 273,
     OP_GREATEQUAL = 274,
     OP_SRIGHT = 275,
     OP_RIGHT = 276,
     OP_LEFT = 277,
     OP_FSUB = 278,
     OP_FADD = 279,
     OP_FDIV = 280,
     OP_FMULT = 281,
     OP_SREM = 282,
     OP_SDIV = 283,
     OP_ZEXT = 284,
     OP_CARRY = 285,
     OP_BORROW = 286,
     OP_SEXT = 287,
     OP_SCARRY = 288,
     OP_SBORROW = 289,
     OP_NAN = 290,
     OP_ABS = 291,
     OP_SQRT = 292,
     OP_CEIL = 293,
     OP_FLOOR = 294,
     OP_ROUND = 295,
     OP_INT2FLOAT = 296,
     OP_FLOAT2FLOAT = 297,
     OP_TRUNC = 298,
     OP_NEW = 299,
     BADINTEGER = 300,
     GOTO_KEY = 301,
     CALL_KEY = 302,
     RETURN_KEY = 303,
     IF_KEY = 304,
     ENDOFSTREAM = 305,
     LOCAL_KEY = 306,
     INTEGER = 307,
     STRING = 308,
     SPACESYM = 309,
     USEROPSYM = 310,
     VARSYM = 311,
     OPERANDSYM = 312,
     STARTSYM = 313,
     ENDSYM = 314,
     LABELSYM = 315
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 26 "pcodeparse.y"

  uintb *i;
  string *str;
  vector<ExprTree *> *param;
  StarQuality *starqual;
  VarnodeTpl *varnode;
  ExprTree *tree;
  vector<OpTpl *> *stmt;
  ConstructTpl *sem;

  SpaceSymbol *spacesym;
  UserOpSymbol *useropsym;
  LabelSymbol *labelsym;
  StartSymbol *startsym;
  EndSymbol *endsym;
  OperandSymbol *operandsym;
  VarnodeSymbol *varsym;
  SpecificSymbol *specsym;



/* Line 214 of yacc.c  */
#line 210 "pcodeparse.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 222 "pcodeparse.cc"

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1919

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  13
/* YYNRULES -- Number of rules.  */
#define YYNRULES  118
/* YYNRULES -- Number of states.  */
#define YYNSTATES  296

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   315

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    40,     2,     2,     2,    35,     9,     2,
      76,    77,    33,    29,    79,    30,     2,    34,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    74,     7,
      14,    75,    15,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    78,     2,    80,     8,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     6,     2,    41,     2,     2,     2,
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
       5,    10,    11,    12,    13,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    31,    32,
      36,    37,    38,    39,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     7,    10,    15,    22,    27,    33,
      38,    46,    53,    57,    63,    69,    79,    84,    89,    93,
      99,   105,   109,   115,   118,   124,   126,   128,   131,   135,
     139,   143,   147,   151,   155,   159,   163,   167,   171,   175,
     179,   183,   186,   189,   193,   197,   201,   205,   209,   213,
     217,   221,   225,   229,   233,   236,   240,   244,   248,   252,
     256,   260,   264,   268,   272,   276,   280,   284,   288,   291,
     296,   301,   306,   311,   318,   325,   332,   337,   342,   347,
     352,   357,   362,   367,   372,   379,   384,   388,   395,   400,
     407,   412,   416,   418,   420,   422,   424,   426,   431,   433,
     435,   437,   439,   441,   443,   445,   449,   452,   457,   459,
     461,   465,   469,   471,   473,   475,   477,   478,   480
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      82,     0,    -1,    83,    63,    -1,    -1,    83,    84,    -1,
      83,    64,    66,     7,    -1,    83,    64,    66,    74,    65,
       7,    -1,    90,    75,    85,     7,    -1,    64,    66,    75,
      85,     7,    -1,    66,    75,    85,     7,    -1,    64,    66,
      74,    65,    75,    85,     7,    -1,    66,    74,    65,    75,
      85,     7,    -1,    64,    92,    75,    -1,    86,    85,    75,
      85,     7,    -1,    68,    76,    93,    77,     7,    -1,    90,
      78,    65,    79,    65,    80,    75,    85,     7,    -1,    88,
      74,    65,    75,    -1,    88,    76,    65,    77,    -1,    59,
      87,     7,    -1,    62,    85,    59,    87,     7,    -1,    59,
      78,    85,    80,     7,    -1,    60,    87,     7,    -1,    60,
      78,    85,    80,     7,    -1,    61,     7,    -1,    61,    78,
      85,    80,     7,    -1,    91,    -1,    88,    -1,    86,    85,
      -1,    76,    85,    77,    -1,    85,    29,    85,    -1,    85,
      30,    85,    -1,    85,    13,    85,    -1,    85,    12,    85,
      -1,    85,    14,    85,    -1,    85,    25,    85,    -1,    85,
      24,    85,    -1,    85,    15,    85,    -1,    85,    23,    85,
      -1,    85,    22,    85,    -1,    85,    21,    85,    -1,    85,
      20,    85,    -1,    30,    85,    -1,    41,    85,    -1,    85,
       8,    85,    -1,    85,     9,    85,    -1,    85,     6,    85,
      -1,    85,    28,    85,    -1,    85,    27,    85,    -1,    85,
      26,    85,    -1,    85,    33,    85,    -1,    85,    34,    85,
      -1,    85,    39,    85,    -1,    85,    35,    85,    -1,    85,
      38,    85,    -1,    40,    85,    -1,    85,     4,    85,    -1,
      85,     5,    85,    -1,    85,     3,    85,    -1,    85,    11,
      85,    -1,    85,    10,    85,    -1,    85,    19,    85,    -1,
      85,    18,    85,    -1,    85,    17,    85,    -1,    85,    16,
      85,    -1,    85,    32,    85,    -1,    85,    31,    85,    -1,
      85,    37,    85,    -1,    85,    36,    85,    -1,    31,    85,
      -1,    49,    76,    85,    77,    -1,    50,    76,    85,    77,
      -1,    45,    76,    85,    77,    -1,    42,    76,    85,    77,
      -1,    43,    76,    85,    79,    85,    77,    -1,    46,    76,
      85,    79,    85,    77,    -1,    47,    76,    85,    79,    85,
      77,    -1,    55,    76,    85,    77,    -1,    54,    76,    85,
      77,    -1,    48,    76,    85,    77,    -1,    56,    76,    85,
      77,    -1,    51,    76,    85,    77,    -1,    52,    76,    85,
      77,    -1,    53,    76,    85,    77,    -1,    57,    76,    85,
      77,    -1,    57,    76,    85,    79,    85,    77,    -1,    92,
      76,    89,    77,    -1,    92,    74,    65,    -1,    92,    78,
      65,    79,    65,    80,    -1,    68,    76,    93,    77,    -1,
      33,    78,    67,    80,    74,    65,    -1,    33,    78,    67,
      80,    -1,    33,    74,    65,    -1,    33,    -1,    71,    -1,
      72,    -1,    65,    -1,    58,    -1,    65,    78,    67,    80,
      -1,    91,    -1,    66,    -1,    92,    -1,    89,    -1,    66,
      -1,    65,    -1,    58,    -1,    65,    74,    65,    -1,     9,
      88,    -1,     9,    74,    65,    88,    -1,    92,    -1,    66,
      -1,    14,    73,    15,    -1,    14,    66,    15,    -1,    69,
      -1,    70,    -1,    71,    -1,    72,    -1,    -1,    85,    -1,
      93,    79,    85,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   100,   100,   102,   103,   104,   105,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   191,
     192,   193,   194,   196,   197,   198,   199,   200,   201,   202,
     204,   205,   206,   208,   209,   210,   211,   212,   214,   215,
     217,   218,   220,   221,   222,   223,   225,   226,   227
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OP_BOOL_OR", "OP_BOOL_XOR",
  "OP_BOOL_AND", "'|'", "';'", "'^'", "'&'", "OP_FNOTEQUAL", "OP_FEQUAL",
  "OP_NOTEQUAL", "OP_EQUAL", "'<'", "'>'", "OP_FGREATEQUAL",
  "OP_FLESSEQUAL", "OP_FGREAT", "OP_FLESS", "OP_SGREAT", "OP_SLESSEQUAL",
  "OP_SGREATEQUAL", "OP_SLESS", "OP_LESSEQUAL", "OP_GREATEQUAL",
  "OP_SRIGHT", "OP_RIGHT", "OP_LEFT", "'+'", "'-'", "OP_FSUB", "OP_FADD",
  "'*'", "'/'", "'%'", "OP_FDIV", "OP_FMULT", "OP_SREM", "OP_SDIV", "'!'",
  "'~'", "OP_ZEXT", "OP_CARRY", "OP_BORROW", "OP_SEXT", "OP_SCARRY",
  "OP_SBORROW", "OP_NAN", "OP_ABS", "OP_SQRT", "OP_CEIL", "OP_FLOOR",
  "OP_ROUND", "OP_INT2FLOAT", "OP_FLOAT2FLOAT", "OP_TRUNC", "OP_NEW",
  "BADINTEGER", "GOTO_KEY", "CALL_KEY", "RETURN_KEY", "IF_KEY",
  "ENDOFSTREAM", "LOCAL_KEY", "INTEGER", "STRING", "SPACESYM", "USEROPSYM",
  "VARSYM", "OPERANDSYM", "STARTSYM", "ENDSYM", "LABELSYM", "':'", "'='",
  "'('", "')'", "'['", "','", "']'", "$accept", "rtl", "rtlmid",
  "statement", "expr", "sizedstar", "jumpdest", "varnode",
  "integervarnode", "lhsvarnode", "label", "specificsymbol", "paramlist", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   124,    59,    94,    38,
     261,   262,   263,   264,    60,    62,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,    43,
      45,   278,   279,    42,    47,    37,   280,   281,   282,   283,
      33,   126,   284,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,    58,    61,    40,    41,    91,    44,
      93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    81,    82,    83,    83,    83,    83,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    86,
      86,    86,    86,    87,    87,    87,    87,    87,    87,    87,
      88,    88,    88,    89,    89,    89,    89,    89,    90,    90,
      91,    91,    92,    92,    92,    92,    93,    93,    93
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     0,     2,     4,     6,     4,     5,     4,
       7,     6,     3,     5,     5,     9,     4,     4,     3,     5,
       5,     3,     5,     2,     5,     1,     1,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     4,
       4,     4,     4,     6,     6,     6,     4,     4,     4,     4,
       4,     4,     4,     4,     6,     4,     3,     6,     4,     6,
       4,     3,     1,     1,     1,     1,     1,     4,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     4,     1,     1,
       3,     3,     1,     1,     1,     1,     0,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     1,     0,     0,    92,   104,     0,     0,
       0,     0,     2,     0,   103,   102,     0,   112,   113,   114,
     115,     4,     0,     0,   101,     0,    25,   100,   102,     0,
     106,   100,     0,     0,     0,     0,    96,    95,    99,    93,
      94,     0,     0,    98,     0,     0,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    26,   100,     0,     0,     0,     0,     0,   116,     0,
       0,     0,     0,     0,     0,   111,   110,    91,     0,     0,
       0,    18,     0,    21,     0,    41,    68,    54,    42,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   116,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    27,     0,     0,     0,     5,     0,     0,
      12,   105,     0,     0,   117,     0,     0,     0,     0,     0,
       0,   107,    90,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    28,    57,    55,    56,    45,    43,    44,
      59,    58,    32,    31,    33,    36,    63,    62,    61,    60,
      40,    39,    38,    37,    35,    34,    48,    47,    46,    29,
      30,    65,    64,    49,    50,    52,    67,    66,    53,    51,
       0,    86,     0,     0,     0,     0,     0,     9,     0,     0,
       0,    16,    17,     7,     0,     0,    97,    20,    22,    24,
      72,     0,    71,     0,     0,    78,    69,    70,    80,    81,
      82,    77,    76,    79,    83,     0,    88,    19,    85,     0,
       6,     0,     8,     0,    14,   118,    13,     0,    89,     0,
       0,     0,     0,     0,     0,    11,     0,    73,    74,    75,
      84,    87,    10,     0,     0,    15
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    21,   164,    70,    42,    71,    24,    25,
      43,    72,   165
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -67
static const yytype_int16 yypact[] =
{
     -67,     9,   203,   -67,    -2,   -39,   -43,   -67,   135,   137,
      -1,  1490,   -67,   -18,   -66,   -46,   -50,   -67,   -67,   -67,
     -67,   -67,  1490,    76,   -67,    67,   -67,    68,   -67,   -10,
     -67,   -67,    34,    35,    11,    -6,   -67,   -13,   -67,   -67,
     -67,  1490,    75,   -67,  1490,    79,   -67,  1490,  1490,  1490,
    1490,  1490,     7,    27,    28,    65,    71,    78,    84,   118,
     122,   123,   138,   140,   143,   145,   146,   147,  1490,  1564,
    1490,   -67,   -16,     5,    12,   159,   161,  1490,  1490,  1459,
     162,   164,  1490,   165,   187,   -67,   -67,   -67,   151,   167,
     153,   -67,   275,   -67,   312,   -67,   -67,   -67,   -67,  1490,
    1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,
    1490,  1490,  1490,  1490,  1490,   603,  1490,  1490,  1490,  1490,
    1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,
    1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,
    1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,
    1490,  1490,   139,   -67,   170,    16,   172,   -67,   173,  1490,
     -67,   -67,   157,  1621,  1880,    -4,  1490,   166,   169,  1658,
     160,   -67,   174,   171,   237,   240,   242,   640,   422,   710,
     494,   531,   747,   817,   854,   924,   961,  1031,  1068,  1138,
    1175,   385,   141,   -67,   348,   457,   457,   566,   672,   778,
     881,   881,   881,   881,   988,   988,   988,   988,   988,   988,
     988,   988,   988,   988,   988,   988,   -15,   -15,   -15,     8,
       8,     8,     8,   -67,   -67,   -67,   -67,   -67,   -67,   -67,
     243,   -67,   178,   191,     3,  1695,  1490,   -67,   269,  1490,
    1732,   -67,   -67,   -67,   212,   254,   -67,   -67,   -67,   -67,
     -67,  1490,   -67,  1490,  1490,   -67,   -67,   -67,   -67,   -67,
     -67,   -67,   -67,   -67,   -67,  1490,   -67,   -67,   -67,   364,
     -67,  1490,   -67,  1769,   -67,  1880,   -67,   458,   -67,  1245,
    1282,  1352,  1389,   491,  1806,   -67,   497,   -67,   -67,   -67,
     -67,   -67,   -67,  1490,  1843,   -67
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -67,   -67,   -67,   -67,   -11,   645,    -8,     1,   557,   -67,
     752,     0,   704
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -110
static const yytype_int16 yytable[] =
{
      69,    45,    27,    23,    31,    30,    46,     4,    75,     3,
     270,    79,   157,    74,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,     4,    78,    32,    76,    77,
      90,    34,  -109,    92,    33,    35,    94,    95,    96,    97,
      98,   145,   146,   147,   148,   149,   150,   151,    73,    85,
      86,    17,    18,    19,    20,    84,     7,   115,   154,   153,
     155,    88,   156,    14,    28,    89,   163,    17,    18,    19,
      20,   169,    29,   238,     7,   239,    87,    47,   271,   158,
     159,    14,    91,    99,    31,   171,    93,   160,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   100,   101,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   102,    82,  -108,   230,    83,  -108,   103,   235,     5,
      80,     5,    81,     5,   104,   240,   116,   117,   118,   119,
     105,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,    36,   106,    36,     4,    36,   107,   108,
      37,    38,    37,    38,    37,    38,    39,    40,    39,    40,
      39,    40,     4,    41,   109,    44,   110,     5,   266,   111,
     239,   112,   113,   114,   161,   273,   162,   167,   275,   168,
     170,   172,   236,   174,   173,   231,     6,   233,   234,   244,
     279,   241,   280,   281,   247,     7,   242,   248,   245,   249,
     267,   246,    14,    28,   282,   268,    17,    18,    19,    20,
     284,     7,     8,     9,    10,    11,    12,    13,    14,    15,
     269,    16,    17,    18,    19,    20,   274,   277,   116,   117,
     118,   119,   294,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   116,   117,   118,   119,   278,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   117,   118,   119,   175,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   116,   117,
     118,   119,   176,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   116,   117,   118,   119,   283,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   264,   119,   265,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   116,   117,   118,
     119,   251,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   116,   117,   118,   119,   286,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   291,   293,   253,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   116,   117,   118,   119,
     254,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   116,   117,   118,   119,    22,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     193,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   232,   116,   117,   118,   119,   250,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     116,   117,   118,   119,    26,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   252,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   192,     0,
     116,   117,   118,   119,   255,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   116,   117,   118,
     119,     0,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   256,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,     0,     0,     0,     0,     0,     0,   116,   117,   118,
     119,   257,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   116,   117,   118,   119,     0,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   258,  -110,  -110,  -110,  -110,  -110,  -110,  -110,  -110,
    -110,  -110,  -110,  -110,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,     0,     0,
       0,     0,     0,     0,   116,   117,   118,   119,   259,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   116,   117,   118,   119,     0,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   260,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   116,   117,   118,   119,   261,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   116,   117,
     118,   119,     0,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   262,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   116,   117,
     118,   119,   263,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   116,   117,   118,   119,     0,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   287,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   116,   117,   118,   119,   288,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   116,   117,   118,   119,     0,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   289,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   116,   117,   118,   119,   290,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,     4,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,    49,     0,     6,     0,     0,     0,     0,     0,     0,
      50,    51,    52,    53,   166,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,     7,     0,
       0,     0,     0,     0,     0,    14,    28,     0,    67,    17,
      18,    19,    20,     0,     0,     0,    68,   116,   117,   118,
     119,     0,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   152,   116,   117,   118,   119,   237,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   116,   117,   118,   119,   243,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   116,   117,
     118,   119,   272,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   116,   117,   118,   119,   276,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   116,   117,   118,   119,   285,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   116,
     117,   118,   119,   292,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   116,   117,   118,   119,
     295,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   116,   117,   118,   119,     0,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151
};

static const yytype_int16 yycheck[] =
{
      11,     9,     2,     2,     4,     4,     7,     9,    74,     0,
       7,    22,     7,    13,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,     9,    76,    66,    74,    75,
      41,    74,    78,    44,    73,    78,    47,    48,    49,    50,
      51,    33,    34,    35,    36,    37,    38,    39,    66,    15,
      15,    69,    70,    71,    72,    65,    58,    68,    74,    70,
      76,    67,    78,    65,    66,    78,    77,    69,    70,    71,
      72,    82,    74,    77,    58,    79,    65,    78,    75,    74,
      75,    65,     7,    76,    84,    84,     7,    75,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,    76,    76,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,    76,    75,    75,   152,    78,    78,    76,   159,    14,
      74,    14,    76,    14,    76,   166,     3,     4,     5,     6,
      76,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    58,    76,    58,     9,    58,    76,    76,
      65,    66,    65,    66,    65,    66,    71,    72,    71,    72,
      71,    72,     9,    78,    76,    78,    76,    14,    77,    76,
      79,    76,    76,    76,    65,   236,    65,    65,   239,    65,
      65,    80,    75,    80,    67,    65,    33,    65,    65,    79,
     251,    75,   253,   254,     7,    58,    77,     7,    74,     7,
       7,    80,    65,    66,   265,    77,    69,    70,    71,    72,
     271,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      79,    68,    69,    70,    71,    72,     7,    65,     3,     4,
       5,     6,   293,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,     3,     4,     5,     6,    65,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,     4,     5,     6,    80,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,     3,     4,
       5,     6,    80,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,     3,     4,     5,     6,    65,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    77,     6,    79,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,     3,     4,     5,
       6,    79,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,     3,     4,     5,     6,    80,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    80,    75,    79,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,     3,     4,     5,     6,
      79,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,     3,     4,     5,     6,     2,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      77,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,   155,     3,     4,     5,     6,    77,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
       3,     4,     5,     6,     2,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    77,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,   114,    -1,
       3,     4,     5,     6,    77,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,     3,     4,     5,
       6,    -1,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    77,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    77,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,     3,     4,     5,     6,    -1,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    77,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    77,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,     3,     4,     5,     6,    -1,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    77,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    77,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,     3,     4,
       5,     6,    -1,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    77,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    77,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,     3,     4,     5,     6,    -1,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    77,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,     3,     4,     5,     6,    -1,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    77,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    77,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      30,    31,    -1,    33,    -1,    -1,    -1,    -1,    -1,    -1,
      40,    41,    42,    43,    75,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    66,    -1,    68,    69,
      70,    71,    72,    -1,    -1,    -1,    76,     3,     4,     5,
       6,    -1,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    59,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    82,    83,     0,     9,    14,    33,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    68,    69,    70,    71,
      72,    84,    86,    88,    89,    90,    91,    92,    66,    74,
      88,    92,    66,    73,    74,    78,    58,    65,    66,    71,
      72,    78,    87,    91,    78,    87,     7,    78,    30,    31,
      40,    41,    42,    43,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    68,    76,    85,
      86,    88,    92,    66,    92,    74,    74,    75,    76,    85,
      74,    76,    75,    78,    65,    15,    15,    65,    67,    78,
      85,     7,    85,     7,    85,    85,    85,    85,    85,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    85,     3,     4,     5,     6,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    59,    85,    74,    76,    78,     7,    74,    75,
      75,    65,    65,    85,    85,    93,    75,    65,    65,    85,
      65,    88,    80,    67,    80,    80,    80,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    93,    77,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      87,    65,    89,    65,    65,    85,    75,     7,    77,    79,
      85,    75,    77,     7,    79,    74,    80,     7,     7,     7,
      77,    79,    77,    79,    79,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    79,    77,     7,    77,    79,
       7,    75,     7,    85,     7,    85,     7,    65,    65,    85,
      85,    85,    85,    65,    85,     7,    80,    77,    77,    77,
      77,    80,     7,    75,    85,     7
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
      case 65: /* "INTEGER" */

/* Line 1000 of yacc.c  */
#line 90 "pcodeparse.y"
	{ delete (yyvaluep->i); };

/* Line 1000 of yacc.c  */
#line 1694 "pcodeparse.cc"
	break;
      case 66: /* "STRING" */

/* Line 1000 of yacc.c  */
#line 91 "pcodeparse.y"
	{ delete (yyvaluep->str); };

/* Line 1000 of yacc.c  */
#line 1703 "pcodeparse.cc"
	break;
      case 83: /* "rtlmid" */

/* Line 1000 of yacc.c  */
#line 93 "pcodeparse.y"
	{ delete (yyvaluep->sem); };

/* Line 1000 of yacc.c  */
#line 1712 "pcodeparse.cc"
	break;
      case 84: /* "statement" */

/* Line 1000 of yacc.c  */
#line 94 "pcodeparse.y"
	{ if ((yyvaluep->stmt) != (vector<OpTpl *> *)0) { for(int4 i=0;i<(yyvaluep->stmt)->size();++i) delete (*(yyvaluep->stmt))[i]; delete (yyvaluep->stmt);} };

/* Line 1000 of yacc.c  */
#line 1721 "pcodeparse.cc"
	break;
      case 85: /* "expr" */

/* Line 1000 of yacc.c  */
#line 95 "pcodeparse.y"
	{ delete (yyvaluep->tree); };

/* Line 1000 of yacc.c  */
#line 1730 "pcodeparse.cc"
	break;
      case 86: /* "sizedstar" */

/* Line 1000 of yacc.c  */
#line 97 "pcodeparse.y"
	{ delete (yyvaluep->starqual); };

/* Line 1000 of yacc.c  */
#line 1739 "pcodeparse.cc"
	break;
      case 87: /* "jumpdest" */

/* Line 1000 of yacc.c  */
#line 96 "pcodeparse.y"
	{ if ((yyvaluep->varnode) != (VarnodeTpl *)0) delete (yyvaluep->varnode); };

/* Line 1000 of yacc.c  */
#line 1748 "pcodeparse.cc"
	break;
      case 88: /* "varnode" */

/* Line 1000 of yacc.c  */
#line 96 "pcodeparse.y"
	{ if ((yyvaluep->varnode) != (VarnodeTpl *)0) delete (yyvaluep->varnode); };

/* Line 1000 of yacc.c  */
#line 1757 "pcodeparse.cc"
	break;
      case 89: /* "integervarnode" */

/* Line 1000 of yacc.c  */
#line 96 "pcodeparse.y"
	{ if ((yyvaluep->varnode) != (VarnodeTpl *)0) delete (yyvaluep->varnode); };

/* Line 1000 of yacc.c  */
#line 1766 "pcodeparse.cc"
	break;
      case 90: /* "lhsvarnode" */

/* Line 1000 of yacc.c  */
#line 96 "pcodeparse.y"
	{ if ((yyvaluep->varnode) != (VarnodeTpl *)0) delete (yyvaluep->varnode); };

/* Line 1000 of yacc.c  */
#line 1775 "pcodeparse.cc"
	break;
      case 93: /* "paramlist" */

/* Line 1000 of yacc.c  */
#line 92 "pcodeparse.y"
	{ for(int4 i=0;i<(yyvaluep->param)->size();++i) delete (*(yyvaluep->param))[i]; delete (yyvaluep->param); };

/* Line 1000 of yacc.c  */
#line 1784 "pcodeparse.cc"
	break;

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
#line 100 "pcodeparse.y"
    { pcode->setResult((yyvsp[(1) - (2)].sem)); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 102 "pcodeparse.y"
    { (yyval.sem) = new ConstructTpl(); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 103 "pcodeparse.y"
    { (yyval.sem) = (yyvsp[(1) - (2)].sem); if (!(yyval.sem)->addOpList(*(yyvsp[(2) - (2)].stmt))) { delete (yyvsp[(2) - (2)].stmt); yyerror("Multiple delayslot declarations"); YYERROR; } delete (yyvsp[(2) - (2)].stmt); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 104 "pcodeparse.y"
    { (yyval.sem) = (yyvsp[(1) - (4)].sem); pcode->newLocalDefinition((yyvsp[(3) - (4)].str)); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 105 "pcodeparse.y"
    { (yyval.sem) = (yyvsp[(1) - (6)].sem); pcode->newLocalDefinition((yyvsp[(3) - (6)].str),*(yyvsp[(5) - (6)].i)); delete (yyvsp[(5) - (6)].i); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 107 "pcodeparse.y"
    { (yyvsp[(3) - (4)].tree)->setOutput((yyvsp[(1) - (4)].varnode)); (yyval.stmt) = ExprTree::toVector((yyvsp[(3) - (4)].tree)); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 108 "pcodeparse.y"
    { (yyval.stmt) = pcode->newOutput(true,(yyvsp[(4) - (5)].tree),(yyvsp[(2) - (5)].str)); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 109 "pcodeparse.y"
    { (yyval.stmt) = pcode->newOutput(false,(yyvsp[(3) - (4)].tree),(yyvsp[(1) - (4)].str)); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 110 "pcodeparse.y"
    { (yyval.stmt) = pcode->newOutput(true,(yyvsp[(6) - (7)].tree),(yyvsp[(2) - (7)].str),*(yyvsp[(4) - (7)].i)); delete (yyvsp[(4) - (7)].i); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 111 "pcodeparse.y"
    { (yyval.stmt) = pcode->newOutput(true,(yyvsp[(5) - (6)].tree),(yyvsp[(1) - (6)].str),*(yyvsp[(3) - (6)].i)); delete (yyvsp[(3) - (6)].i); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 112 "pcodeparse.y"
    { (yyval.stmt) = (vector<OpTpl *> *)0; string errmsg = "Redefinition of symbol: "+(yyvsp[(2) - (3)].specsym)->getName(); yyerror(errmsg.c_str()); YYERROR; ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 113 "pcodeparse.y"
    { (yyval.stmt) = pcode->createStore((yyvsp[(1) - (5)].starqual),(yyvsp[(2) - (5)].tree),(yyvsp[(4) - (5)].tree)); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 114 "pcodeparse.y"
    { (yyval.stmt) = pcode->createUserOpNoOut((yyvsp[(1) - (5)].useropsym),(yyvsp[(3) - (5)].param)); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 115 "pcodeparse.y"
    { (yyval.stmt) = pcode->assignBitRange((yyvsp[(1) - (9)].varnode),(uint4)*(yyvsp[(3) - (9)].i),(uint4)*(yyvsp[(5) - (9)].i),(yyvsp[(8) - (9)].tree)); delete (yyvsp[(3) - (9)].i), delete (yyvsp[(5) - (9)].i); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 116 "pcodeparse.y"
    { (yyval.stmt) = (vector<OpTpl *> *)0; delete (yyvsp[(1) - (4)].varnode); delete (yyvsp[(3) - (4)].i); yyerror("Illegal truncation on left-hand side of assignment"); YYERROR; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 117 "pcodeparse.y"
    { (yyval.stmt) = (vector<OpTpl *> *)0; delete (yyvsp[(1) - (4)].varnode); delete (yyvsp[(3) - (4)].i); yyerror("Illegal subpiece on left-hand side of assignment"); YYERROR; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 118 "pcodeparse.y"
    { (yyval.stmt) = pcode->createOpNoOut(CPUI_BRANCH,new ExprTree((yyvsp[(2) - (3)].varnode))); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 119 "pcodeparse.y"
    { (yyval.stmt) = pcode->createOpNoOut(CPUI_CBRANCH,new ExprTree((yyvsp[(4) - (5)].varnode)),(yyvsp[(2) - (5)].tree)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 120 "pcodeparse.y"
    { (yyval.stmt) = pcode->createOpNoOut(CPUI_BRANCHIND,(yyvsp[(3) - (5)].tree)); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 121 "pcodeparse.y"
    { (yyval.stmt) = pcode->createOpNoOut(CPUI_CALL,new ExprTree((yyvsp[(2) - (3)].varnode))); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 122 "pcodeparse.y"
    { (yyval.stmt) = pcode->createOpNoOut(CPUI_CALLIND,(yyvsp[(3) - (5)].tree)); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 123 "pcodeparse.y"
    { (yyval.stmt) = (vector<OpTpl *> *)0; yyerror("Must specify an indirect parameter for return"); YYERROR; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 124 "pcodeparse.y"
    { (yyval.stmt) = pcode->createOpNoOut(CPUI_RETURN,(yyvsp[(3) - (5)].tree)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 125 "pcodeparse.y"
    { (yyval.stmt) = pcode->placeLabel( (yyvsp[(1) - (1)].labelsym) ); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 127 "pcodeparse.y"
    { (yyval.tree) = new ExprTree((yyvsp[(1) - (1)].varnode)); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 128 "pcodeparse.y"
    { (yyval.tree) = pcode->createLoad((yyvsp[(1) - (2)].starqual),(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 129 "pcodeparse.y"
    { (yyval.tree) = (yyvsp[(2) - (3)].tree); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 130 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_ADD,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 131 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SUB,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 132 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_EQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 133 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_NOTEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 134 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_LESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 135 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_LESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 136 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_LESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 137 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_LESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 138 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SLESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 139 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SLESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 140 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SLESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 141 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SLESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 142 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_2COMP,(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 143 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_NEGATE,(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 144 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_XOR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 145 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_AND,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 146 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_OR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 147 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_LEFT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 148 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_RIGHT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 149 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SRIGHT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 150 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_MULT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 151 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_DIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 152 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SDIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 153 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_REM,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 154 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SREM,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 155 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_BOOL_NEGATE,(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 156 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_BOOL_XOR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 157 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_BOOL_AND,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 158 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_BOOL_OR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 159 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_EQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 160 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_NOTEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 161 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_LESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 162 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_LESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 163 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_LESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 164 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_LESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 165 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_ADD,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 166 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_SUB,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 167 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_MULT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 168 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_DIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 169 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_NEG,(yyvsp[(2) - (2)].tree)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 170 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_ABS,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 171 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_SQRT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 172 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SEXT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 173 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_ZEXT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 174 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_CARRY,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 175 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SCARRY,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 176 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_INT_SBORROW,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 177 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_FLOAT2FLOAT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 178 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_INT2FLOAT,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 179 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_NAN,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 180 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_TRUNC,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 181 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_CEIL,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 182 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_FLOOR,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 183 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_FLOAT_ROUND,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 184 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_NEW,(yyvsp[(3) - (4)].tree)); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 185 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_NEW,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 186 "pcodeparse.y"
    { (yyval.tree) = pcode->createOp(CPUI_SUBPIECE,new ExprTree((yyvsp[(1) - (4)].specsym)->getVarnode()),new ExprTree((yyvsp[(3) - (4)].varnode))); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 187 "pcodeparse.y"
    { (yyval.tree) = pcode->createBitRange((yyvsp[(1) - (3)].specsym),0,(uint4)(*(yyvsp[(3) - (3)].i) * 8)); delete (yyvsp[(3) - (3)].i); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 188 "pcodeparse.y"
    { (yyval.tree) = pcode->createBitRange((yyvsp[(1) - (6)].specsym),(uint4)*(yyvsp[(3) - (6)].i),(uint4)*(yyvsp[(5) - (6)].i)); delete (yyvsp[(3) - (6)].i), delete (yyvsp[(5) - (6)].i); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 189 "pcodeparse.y"
    { (yyval.tree) = pcode->createUserOp((yyvsp[(1) - (4)].useropsym),(yyvsp[(3) - (4)].param)); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 191 "pcodeparse.y"
    { (yyval.starqual) = new StarQuality; (yyval.starqual)->size = *(yyvsp[(6) - (6)].i); delete (yyvsp[(6) - (6)].i); (yyval.starqual)->id=ConstTpl((yyvsp[(3) - (6)].spacesym)->getSpace()); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 192 "pcodeparse.y"
    { (yyval.starqual) = new StarQuality; (yyval.starqual)->size = 0; (yyval.starqual)->id=ConstTpl((yyvsp[(3) - (4)].spacesym)->getSpace()); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 193 "pcodeparse.y"
    { (yyval.starqual) = new StarQuality; (yyval.starqual)->size = *(yyvsp[(3) - (3)].i); delete (yyvsp[(3) - (3)].i); (yyval.starqual)->id=ConstTpl(pcode->getDefaultSpace()); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 194 "pcodeparse.y"
    { (yyval.starqual) = new StarQuality; (yyval.starqual)->size = 0; (yyval.starqual)->id=ConstTpl(pcode->getDefaultSpace()); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 196 "pcodeparse.y"
    { VarnodeTpl *sym = (yyvsp[(1) - (1)].startsym)->getVarnode(); (yyval.varnode) = new VarnodeTpl(ConstTpl(ConstTpl::j_curspace),sym->getOffset(),ConstTpl(ConstTpl::j_curspace_size)); delete sym; ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 197 "pcodeparse.y"
    { VarnodeTpl *sym = (yyvsp[(1) - (1)].endsym)->getVarnode(); (yyval.varnode) = new VarnodeTpl(ConstTpl(ConstTpl::j_curspace),sym->getOffset(),ConstTpl(ConstTpl::j_curspace_size)); delete sym; ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 198 "pcodeparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(ConstTpl::j_curspace),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (1)].i)),ConstTpl(ConstTpl::j_curspace_size)); delete (yyvsp[(1) - (1)].i); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 199 "pcodeparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(ConstTpl::j_curspace),ConstTpl(ConstTpl::real,0),ConstTpl(ConstTpl::j_curspace_size)); yyerror("Parsed integer is too big (overflow)"); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 200 "pcodeparse.y"
    { AddrSpace *spc = (yyvsp[(3) - (4)].spacesym)->getSpace(); (yyval.varnode) = new VarnodeTpl(ConstTpl(spc),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (4)].i)),ConstTpl(ConstTpl::real,spc->getAddrSize())); delete (yyvsp[(1) - (4)].i); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 201 "pcodeparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(pcode->getConstantSpace()),ConstTpl(ConstTpl::j_relative,(yyvsp[(1) - (1)].labelsym)->getIndex()),ConstTpl(ConstTpl::real,sizeof(uintm))); (yyvsp[(1) - (1)].labelsym)->incrementRefCount(); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 202 "pcodeparse.y"
    { (yyval.varnode) = (VarnodeTpl *)0; string errmsg = "Unknown jump destination: "+*(yyvsp[(1) - (1)].str); delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 204 "pcodeparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].specsym)->getVarnode(); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 205 "pcodeparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].varnode); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 206 "pcodeparse.y"
    { (yyval.varnode) = (VarnodeTpl *)0; string errmsg = "Unknown varnode parameter: "+*(yyvsp[(1) - (1)].str); delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 208 "pcodeparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(pcode->getConstantSpace()),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (1)].i)),ConstTpl(ConstTpl::real,0)); delete (yyvsp[(1) - (1)].i); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 209 "pcodeparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(pcode->getConstantSpace()),ConstTpl(ConstTpl::real,0),ConstTpl(ConstTpl::real,0)); yyerror("Parsed integer is too big (overflow)"); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 210 "pcodeparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(pcode->getConstantSpace()),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (3)].i)),ConstTpl(ConstTpl::real,*(yyvsp[(3) - (3)].i))); delete (yyvsp[(1) - (3)].i); delete (yyvsp[(3) - (3)].i); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 211 "pcodeparse.y"
    { (yyval.varnode) = pcode->addressOf((yyvsp[(2) - (2)].varnode),0); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 212 "pcodeparse.y"
    { (yyval.varnode) = pcode->addressOf((yyvsp[(4) - (4)].varnode),*(yyvsp[(3) - (4)].i)); delete (yyvsp[(3) - (4)].i); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 214 "pcodeparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].specsym)->getVarnode(); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 215 "pcodeparse.y"
    { (yyval.varnode) = (VarnodeTpl *)0; string errmsg = "Unknown assignment varnode: "+*(yyvsp[(1) - (1)].str); delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 217 "pcodeparse.y"
    { (yyval.labelsym) = (yyvsp[(2) - (3)].labelsym); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 218 "pcodeparse.y"
    { (yyval.labelsym) = pcode->defineLabel( (yyvsp[(2) - (3)].str) ); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 220 "pcodeparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].varsym); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 221 "pcodeparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].operandsym); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 222 "pcodeparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].startsym); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 223 "pcodeparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].endsym); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 225 "pcodeparse.y"
    { (yyval.param) = new vector<ExprTree *>; ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 226 "pcodeparse.y"
    { (yyval.param) = new vector<ExprTree *>; (yyval.param)->push_back((yyvsp[(1) - (1)].tree)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 227 "pcodeparse.y"
    { (yyval.param) = (yyvsp[(1) - (3)].param); (yyval.param)->push_back((yyvsp[(3) - (3)].tree)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2908 "pcodeparse.cc"
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
#line 229 "pcodeparse.y"


#define IDENTREC_SIZE 46
const IdentRec PcodeLexer::idents[]= { // Sorted list of identifiers
  { "!=", OP_NOTEQUAL },
  { "&&", OP_BOOL_AND },
  { "<<", OP_LEFT },
  { "<=", OP_LESSEQUAL },
  { "==", OP_EQUAL },
  { ">=", OP_GREATEQUAL },
  { ">>", OP_RIGHT },
  { "^^", OP_BOOL_XOR },
  { "||", OP_BOOL_OR },
  { "abs", OP_ABS },
  { "borrow", OP_BORROW },
  { "call", CALL_KEY },
  { "carry", OP_CARRY },
  { "ceil", OP_CEIL },
  { "f!=", OP_FNOTEQUAL },
  { "f*", OP_FMULT },
  { "f+", OP_FADD },
  { "f-", OP_FSUB },
  { "f/", OP_FDIV },
  { "f<", OP_FLESS },
  { "f<=", OP_FLESSEQUAL },
  { "f==", OP_FEQUAL },
  { "f>", OP_FGREAT },
  { "f>=", OP_FGREATEQUAL },
  { "float2float", OP_FLOAT2FLOAT },
  { "floor", OP_FLOOR },
  { "goto", GOTO_KEY },
  { "if", IF_KEY },
  { "int2float", OP_INT2FLOAT },
  { "local", LOCAL_KEY },
  { "nan", OP_NAN },
  { "return", RETURN_KEY },
  { "round", OP_ROUND },
  { "s%", OP_SREM },
  { "s/", OP_SDIV },
  { "s<", OP_SLESS },
  { "s<=", OP_SLESSEQUAL },
  { "s>", OP_SGREAT },
  { "s>=", OP_SGREATEQUAL },
  { "s>>",OP_SRIGHT },
  { "sborrow", OP_SBORROW },
  { "scarry", OP_SCARRY },
  { "sext", OP_SEXT },
  { "sqrt", OP_SQRT },
  { "trunc", OP_TRUNC },
  { "zext", OP_ZEXT }
};

int4 PcodeLexer::findIdentifier(const string &str) const

{
  int4 low = 0;
  int4 high = IDENTREC_SIZE-1;
  int4 comp;
  do {
    int4 targ = (low+high)/2;
    comp = str.compare(idents[targ].nm);
    if (comp < 0) 		// str comes before targ
      high = targ-1;
    else if (comp > 0)		// str comes after targ
      low = targ + 1;
    else
      return targ;
  } while(low <= high);
  return -1;
}

int4 PcodeLexer::moveState(void)

{
  switch(curstate) {
  case start:
    switch(curchar) {
    case '|':
      if (lookahead1 == '|') {
	starttoken();
	curstate = special2;
	return start;
      }
      return punctuation;
    case '&':
      if (lookahead1 == '&') {
	starttoken();
	curstate = special2;
	return start;
      }
      return punctuation;
    case '^':
      if (lookahead1 == '^') {
	starttoken();
	curstate = special2;
	return start;
      }
      return punctuation;
    case '>':
      if ((lookahead1 == '>')||(lookahead1=='=')) {
	starttoken();
	curstate = special2;
	return start;
      }
      return punctuation;
    case '<':
      if ((lookahead1 == '<')||(lookahead1=='=')) {
	starttoken();
	curstate = special2;
	return start;
      }
      return punctuation;
    case '=':
      if (lookahead1 == '=') {
	starttoken();
	curstate = special2;
	return start;
      }
      return punctuation;
    case '!':
      if (lookahead1 == '=') {
	starttoken();
	curstate = special2;
	return start;
      }
      return punctuation;
    case '(':
    case ')':
    case ',':
    case ':':
    case '[':
    case ']':
    case ';':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '~':
      return punctuation;
    case 's':
    case 'f':
      if (curchar == 's') {
	if ((lookahead1 == '/')||(lookahead1=='%')) {
	  starttoken();
	  curstate = special2;
	  return start;
	}
	else if (lookahead1 == '<') {
	  starttoken();
	  if (lookahead2 == '=')
	    curstate = special3;
	  else
	    curstate = special2;
	  return start;
	}
	else if (lookahead1 == '>') {
	  starttoken();
	  if ((lookahead2=='>')||(lookahead2=='='))
	    curstate = special3;
	  else
	    curstate = special2;
	  return start;
	}
      }
      else {			// curchar == 'f'
	if ((lookahead1=='+')||(lookahead1=='-')||(lookahead1=='*')||(lookahead1=='/')) {
	  starttoken();
	  curstate = special2;
	  return start;
	}
	else if (((lookahead1=='=')||(lookahead1=='!'))&&(lookahead2=='=')) {
	  starttoken();
	  curstate = special3;
	  return start;
	}
	else if ((lookahead1=='<')||(lookahead1=='>')) {
	  starttoken();
	  if (lookahead2 == '=')
	    curstate = special3;
	  else
	    curstate = special2;
	  return start;
	}
      }
      // fall through here, treat 's' and 'f' as ordinary characters
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
    case '.':
      starttoken();
      if (isIdent(lookahead1)) {
	curstate = identifier;
	return start;
      }
      curstate = start;
      return identifier;
    case '0':
      starttoken();
      if (lookahead1 == 'x') {
	curstate = hexstring;
	return start;
      }
      if (isDec(lookahead1)) {
	curstate = decstring;
	return start;
      }
      curstate = start;
      return decstring;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      starttoken();
      if (isDec(lookahead1)) {
	curstate = decstring;
	return start;
      }
      curstate = start;
      return decstring;
    case '\n':
    case ' ':
    case '\t':
    case '\v':
    case '\r':
      return start;		// Ignore whitespace
    case '\0':
      curstate = endstream;
      return endstream;
    default:
      curstate = illegal;
      return illegal;
    }
    break;
  case special2:
    advancetoken();
    curstate = start;
    return identifier;
    break;
  case special3:
    advancetoken();
    curstate = special32;
    return start;
    break;
  case special32:
    advancetoken();
    curstate = start;
    return identifier;
    break;
  case comment:
    if (curchar == '\n')
      curstate = start;
    else if (curchar == '\0') {
      curstate = endstream;
      return endstream;
    }
    break;
  case identifier:
    advancetoken();
    if (isIdent(lookahead1))
      return start;
    curstate = start;
    return identifier;
    break;
  case hexstring:
    advancetoken();
    if (isHex(lookahead1))
      return start;
    curstate = start;
    return hexstring;
    break;
  case decstring:
    advancetoken();
    if (isDec(lookahead1))
      return start;
    curstate = start;
    return decstring;
    break;
  default:
    curstate = endstream;
  }
  return endstream;
}

int4 PcodeLexer::getNextToken(void)

{ // Will return either: identifier, punctuation, hexstring, decstring, endstream, or illegal
  // If identifier, hexstring, or decstring,  curtoken will be filled with the characters
  int4 tok;
  do {
    curchar = lookahead1;
    lookahead1 = lookahead2;
    if (endofstream)
      lookahead2 = '\0';
    else {
      s->get(lookahead2);
      if (!(*s)) {
	endofstream = true;
	lookahead2 = '\0';
      }
    }
    tok = moveState();
  } while(tok == start);
  if (tok == identifier) {
    curtoken[tokpos] = '\0';	// Append null terminator
    curidentifier = curtoken;
    int4 num = findIdentifier(curidentifier);
    if (num < 0)			// Not a keyword
      return STRING;
    return idents[num].id;
  }
  else if ((tok == hexstring)||(tok == decstring)) {
    curtoken[tokpos] = '\0';
    istringstream s1(curtoken);
    s1.unsetf(ios::dec | ios::hex | ios::oct);
    s1 >> curnum;
    if (!s1)
      return BADINTEGER;
    return INTEGER;
  }
  else if (tok == endstream) {
    if (!endofstreamsent) {
      endofstreamsent = true;
      return ENDOFSTREAM;	// Send 'official' end of stream token
    }
    return 0;			// 0 means end of file to parser
  }
  else if (tok == illegal)
    return 0;
  return (int4)curchar;
}

void PcodeLexer::initialize(istream *t)

{ // Set up for new lex
  s = t;
  curstate = start;
  tokpos = 0;
  endofstream = false;
  endofstreamsent = false;
  lookahead1 = 0;
  lookahead2 = 0;
  s->get(lookahead1);		// Buffer the first two characters
  if (!(*s)) {
    endofstream = true;
    lookahead1 = 0;
    return;
  }
  s->get(lookahead2);
  if (!(*s)) {
    endofstream = true;
    lookahead2 = 0;
    return;
  }
}

uintb PcodeSnippet::allocateTemp(void)

{ // Allocate a variable in the unique space and return the offset
  uintb res = tempbase;
  tempbase += 16;
  return res;
}

void PcodeSnippet::addSymbol(SleighSymbol *sym)

{
  pair<SymbolTree::iterator,bool> res;

  res = tree.insert( sym );
  if (!res.second) {
    reportError((const Location *)0,"Duplicate symbol name: "+sym->getName());
    delete sym;		// Symbol is unattached to anything else
  }
}

void PcodeSnippet::clear(void)

{ // Clear everything, prepare for a new parse against the same language
  SymbolTree::iterator iter,tmpiter;
  iter = tree.begin();
  while(iter != tree.end()) {
    SleighSymbol *sym = *iter;
    tmpiter = iter;
    ++iter;			// Increment now, as node may be deleted
    if (sym->getType() != SleighSymbol::space_symbol) {
      delete sym;		// Free any old local symbols
      tree.erase(tmpiter);
    }
  }
  if (result != (ConstructTpl *)0) {
    delete result;
    result = (ConstructTpl *)0;
  }
  // tempbase = 0;
  errorcount = 0;
  firsterror.clear();
  resetLabelCount();
}

PcodeSnippet::PcodeSnippet(const SleighBase *slgh)
  : PcodeCompile()
{
  sleigh = slgh;
  tempbase = 0;
  errorcount = 0;
  result = (ConstructTpl *)0;
  setDefaultSpace(slgh->getDefaultCodeSpace());
  setConstantSpace(slgh->getConstantSpace());
  setUniqueSpace(slgh->getUniqueSpace());
  int4 num = slgh->numSpaces();
  for(int4 i=0;i<num;++i) {
    AddrSpace *spc = slgh->getSpace(i);
    spacetype type = spc->getType();
    if ((type==IPTR_CONSTANT)||(type==IPTR_PROCESSOR)||(type==IPTR_SPACEBASE)||(type==IPTR_INTERNAL))
      tree.insert(new SpaceSymbol(spc));
  }
  addSymbol(new FlowDestSymbol("inst_dest",slgh->getConstantSpace()));
  addSymbol(new FlowRefSymbol("inst_ref",slgh->getConstantSpace()));
}

PcodeSnippet::~PcodeSnippet(void)

{
  SymbolTree::iterator iter;
  for(iter=tree.begin();iter!=tree.end();++iter)
    delete *iter;		// Free ALL temporary symbols
  if (result != (ConstructTpl *)0) {
    delete result;
    result = (ConstructTpl *)0;
  }
}

void PcodeSnippet::reportError(const Location *loc, const string &msg)

{
  if (errorcount == 0)
    firsterror = msg;
  errorcount += 1;
}

int4 PcodeSnippet::lex(void)

{
  int4 tok = lexer.getNextToken();
  if (tok == STRING) {
    SleighSymbol *sym;
    SleighSymbol tmpsym(lexer.getIdentifier());
    SymbolTree::const_iterator iter = tree.find(&tmpsym);
    if (iter != tree.end())
      sym = *iter;		// Found a local symbol
    else
      sym = sleigh->findSymbol(lexer.getIdentifier());
    if (sym != (SleighSymbol *)0) {
      switch(sym->getType()) {
      case SleighSymbol::space_symbol:
	yylval.spacesym = (SpaceSymbol *)sym;
	return SPACESYM;
      case SleighSymbol::userop_symbol:
	yylval.useropsym = (UserOpSymbol *)sym;
	return USEROPSYM;
      case SleighSymbol::varnode_symbol:
	yylval.varsym = (VarnodeSymbol *)sym;
	return VARSYM;
      case SleighSymbol::operand_symbol:
	yylval.operandsym = (OperandSymbol *)sym;
	return OPERANDSYM;
      case SleighSymbol::start_symbol:
	yylval.startsym = (StartSymbol *)sym;
	return STARTSYM;
      case SleighSymbol::end_symbol:
	yylval.endsym = (EndSymbol *)sym;
	return ENDSYM;
      case SleighSymbol::label_symbol:
	yylval.labelsym = (LabelSymbol *)sym;
	return LABELSYM;
      case SleighSymbol::dummy_symbol:
	break;
      default:
	// The translator may have other symbols in it that we don't want visible in the snippet compiler
	break;
      }
    }
    yylval.str = new string(lexer.getIdentifier());
    return STRING;
  }
  if (tok == INTEGER) {
    yylval.i = new uintb(lexer.getNumber());
    return INTEGER;
  }
  return tok;
}

 bool PcodeSnippet::parseStream(istream &s)

{
  lexer.initialize(&s);
  pcode = this;			// Setup global object for yyparse
  int4 res = yyparse();
  if (res != 0) {
    reportError((const Location *)0,"Syntax error");
    return false;
  }
  if (!PcodeCompile::propagateSize(result)) {
    reportError((const Location *)0,"Could not resolve at least 1 variable size");
    return false;
  }
  return true;
}

void PcodeSnippet::addOperand(const string &name,int4 index)

{ // Add an operand symbol for this snippet
  OperandSymbol *sym = new OperandSymbol(name,index,(Constructor *)0);
  addSymbol(sym);
}

int yylex(void) {
  return pcode->lex();
}

int yyerror(const char *s)

{
  pcode->reportError((const Location *)0,s);
  return 0;
}

