
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
#define yyparse         xmlparse
#define yylex           xmllex
#define yyerror         xmlerror
#define yylval          xmllval
#define yychar          xmlchar
#define yydebug         xmldebug
#define yynerrs         xmlnerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 16 "xml.y"

#include "xml.hh"
// CharData mode   look for '<' '&' or "]]>"
// Name mode       look for non-name char
// CData mode      looking for "]]>"
// Entity mode     looking for ending ';'
// AttValue mode   looking for endquote  or '&'
// Comment mode    looking for "--"

#include <iostream>
#include <string>

string Attributes::bogus_uri("http://unused.uri");

/// \brief The XML character scanner
///
/// Tokenize a byte stream suitably for the main XML parser.  The scanner expects an ASCII or UTF-8
/// encoding.  Characters is XML tag and attribute names are restricted to ASCII "letters", but
/// extended UTF-8 characters can be used in any other character data: attribute values, content, comments. 
class XmlScan {
public:
  /// \brief Modes of the scanner
  enum mode { CharDataMode, CDataMode, AttValueSingleMode,
	      AttValueDoubleMode, CommentMode, CharRefMode,
	      NameMode, SNameMode, SingleMode };
  /// \brief Additional tokens returned by the scanner, in addition to byte values 00-ff
  enum token { CharDataToken = 258,
	       CDataToken = 259,
	       AttValueToken = 260,
	       CommentToken =261,
	       CharRefToken = 262,
	       NameToken = 263,
	       SNameToken = 264,
	       ElementBraceToken = 265,
	       CommandBraceToken = 266 };
private:
  mode curmode;			///< The current scanning mode
  istream &s;			///< The stream being scanned
  string *lvalue;		///< Current string being built
  int4 lookahead[4];	///< Lookahead into the byte stream
  int4 pos;				///< Current position in the lookahead buffer
  bool endofstream;		///< Has end of stream been reached
  void clearlvalue(void);	///< Clear the current token string

  /// \brief Get the next byte in the stream
  ///
  /// Maintain a lookahead of 4 bytes at all times so that we can check for special
  /// XML character sequences without consuming.
  /// \return the next byte value as an integer
  int4 getxmlchar(void) {
    char c;	    
    int4 ret=lookahead[pos];
    if (!endofstream) {
      s.get(c); 
      if (s.eof()||(c=='\0')) {
	endofstream = true;
	lookahead[pos] = '\n';
      }
      else
	lookahead[pos] = c;
    }
    else
      lookahead[pos] = -1;
    pos = (pos+1)&3;
    return ret;
  }
  int4 next(int4 i) { return lookahead[(pos+i)&3]; }	///< Peek at the next (i-th) byte without consuming
  bool isLetter(int4 val) { return (((val>=0x41)&&(val<=0x5a))||((val>=0x61)&&(val<=0x7a))); }	///< Is the given byte a \e letter
  bool isInitialNameChar(int4 val);		///< Is the given byte/character the valid start of an XML name
  bool isNameChar(int4 val);			///< Is the given byte/character valid for an XML name	
  bool isChar(int4 val);				///< Is the given byte/character valid as an XML character
  int4 scanSingle(void);				///< Scan for the next token in Single Character mode
  int4 scanCharData(void);				///< Scan for the next token is Character Data mode
  int4 scanCData(void);					///< Scan for the next token in CDATA mode
  int4 scanAttValue(int4 quote);		///< Scan for the next token in Attribute Value mode
  int4 scanCharRef(void);				///< Scan for the next token in Character Reference mode
  int4 scanComment(void);				///< Scan for the next token in Comment mode
  int4 scanName(void);					///< Scan a Name or return single non-name character
  int4 scanSName(void);					///< Scan Name, allow white space before
public:
  XmlScan(istream &t);					///< Construct scanner given a stream
  ~XmlScan(void);						///< Destructor
  void setmode(mode m) { curmode = m; }	///< Set the scanning mode
  int4 nexttoken(void);					///< Get the next token
  string *lval(void) { string *ret = lvalue; lvalue = (string *)0; return ret; }	///< Return the last \e lvalue string
};

/// \brief A parsed name/value pair
struct NameValue {
  string *name;		///< The name
  string *value;	///< The value
};

extern int yylex(void);							///< Interface to the scanner
extern int yyerror(const char *str);			///< Interface for registering an error in parsing
extern void print_content(const string &str);	///< Send character data to the ContentHandler
extern int4 convertEntityRef(const string &ref);	///< Convert an XML entity to its equivalent character
extern int4 convertCharRef(const string &ref);	///< Convert an XML character reference to its equivalent character
static XmlScan *global_scan;					///< Global reference to the scanner
static ContentHandler *handler;					///< Global reference to the content handler
extern int yydebug;								///< Debug mode


/* Line 189 of yacc.c  */
#line 185 "xml.cc"

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
     CHARDATA = 258,
     CDATA = 259,
     ATTVALUE = 260,
     COMMENT = 261,
     CHARREF = 262,
     NAME = 263,
     SNAME = 264,
     ELEMBRACE = 265,
     COMMBRACE = 266
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 119 "xml.y"

  int4 i;
  string *str;
  Attributes *attr;
  NameValue *pair;



/* Line 214 of yacc.c  */
#line 241 "xml.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 253 "xml.cc"

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
#define YYFINAL  25
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   205

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  70
/* YYNRULES -- Number of states.  */
#define YYNSTATES  151

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   266

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,    15,
      13,     2,     2,    14,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    12,    18,    17,    48,     2,     2,    47,    16,
       2,     2,     2,     2,     2,    19,     2,    46,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    49,
       2,    32,    20,    21,     2,    25,     2,    23,    24,    31,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    28,
      30,     2,     2,     2,    26,     2,     2,     2,     2,    29,
       2,    22,     2,    27,     2,     2,     2,     2,     2,    40,
      41,    34,     2,    42,     2,    37,     2,     2,    45,    44,
      39,    38,     2,     2,    35,    36,     2,     2,    33,     2,
      43,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,    10,    12,    14,    16,    18,    20,
      23,    25,    28,    31,    33,    36,    39,    42,    45,    47,
      52,    58,    61,    65,    75,    79,    81,    84,    86,    88,
      91,    94,    96,   106,   108,   111,   114,   116,   118,   120,
     131,   143,   150,   154,   159,   164,   170,   172,   176,   179,
     183,   187,   192,   195,   198,   202,   205,   209,   214,   215,
     218,   221,   224,   227,   230,   233,   235,   237,   239,   242,
     246
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      51,     0,    -1,    74,    69,    -1,    66,    74,    69,    -1,
      12,    -1,    13,    -1,    14,    -1,    15,    -1,    52,    -1,
      53,    52,    -1,    16,    -1,    54,     5,    -1,    54,    82,
      -1,    17,    -1,    55,     5,    -1,    55,    82,    -1,    54,
      16,    -1,    55,    17,    -1,    10,    -1,    11,    18,    19,
      19,    -1,    58,     6,    19,    19,    20,    -1,    11,    21,
      -1,    62,     4,    63,    -1,    11,    18,    22,    23,    24,
      25,    26,    25,    22,    -1,    27,    27,    20,    -1,    67,
      -1,    64,    69,    -1,    73,    -1,    69,    -1,    65,    69,
      -1,    65,    64,    -1,    65,    -1,    11,    18,    24,    28,
      23,    26,    29,    30,    31,    -1,    32,    -1,    53,    32,
      -1,    68,    53,    -1,    59,    -1,    60,    -1,    53,    -1,
      53,    33,    34,    35,    36,    37,    38,    39,    68,    56,
      -1,    53,    34,    39,    40,    38,    41,    37,    39,    42,
      68,    56,    -1,    11,    21,    43,    44,    45,    70,    -1,
      72,    21,    20,    -1,    72,    53,    21,    20,    -1,    72,
      71,    21,    20,    -1,    72,    71,    53,    21,    20,    -1,
      76,    -1,    75,    81,    80,    -1,    77,    20,    -1,    77,
      53,    20,    -1,    77,    46,    20,    -1,    77,    53,    46,
      20,    -1,    57,     8,    -1,    77,    78,    -1,     9,    68,
      56,    -1,    11,    46,    -1,    79,     8,    20,    -1,    79,
       8,    53,    20,    -1,    -1,    81,     3,    -1,    81,    74,
      -1,    81,    82,    -1,    81,    61,    -1,    81,    60,    -1,
      81,    59,    -1,    86,    -1,    85,    -1,    47,    -1,    83,
      48,    -1,    84,     7,    49,    -1,    83,     8,    49,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   135,   135,   136,   137,   138,   139,   140,   141,   142,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   160,   161,   162,   163,   164,
     165,   166,   168,   169,   170,   171,   172,   173,   174,   176,
     177,   178,   179,   180,   181,   182,   184,   185,   187,   188,
     189,   190,   192,   193,   194,   195,   196,   197,   199,   200,
     201,   202,   203,   204,   205,   207,   208,   210,   211,   212,
     213
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CHARDATA", "CDATA", "ATTVALUE",
  "COMMENT", "CHARREF", "NAME", "SNAME", "ELEMBRACE", "COMMBRACE", "' '",
  "'\\n'", "'\\r'", "'\\t'", "'\\''", "'\"'", "'!'", "'-'", "'>'", "'?'",
  "'['", "'C'", "'D'", "'A'", "'T'", "']'", "'O'", "'Y'", "'P'", "'E'",
  "'='", "'v'", "'e'", "'r'", "'s'", "'i'", "'o'", "'n'", "'c'", "'d'",
  "'g'", "'x'", "'m'", "'l'", "'/'", "'&'", "'#'", "';'", "$accept",
  "document", "whitespace", "S", "attsinglemid", "attdoublemid",
  "AttValue", "elemstart", "commentstart", "Comment", "PI", "CDSect",
  "CDStart", "CDEnd", "doctypepro", "prologpre", "prolog", "doctypedecl",
  "Eq", "Misc", "VersionInfo", "EncodingDecl", "xmldeclstart", "XMLDecl",
  "element", "STag", "EmptyElemTag", "stagstart", "SAttribute",
  "etagbrace", "ETag", "content", "Reference", "refstart", "charrefstart",
  "CharRef", "EntityRef", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,    32,    10,    13,     9,    39,    34,    33,    45,
      62,    63,    91,    67,    68,    65,    84,    93,    79,    89,
      80,    69,    61,   118,   101,   114,   115,   105,   111,   110,
      99,   100,   103,   120,   109,   108,    47,    38,    35,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    50,    51,    51,    52,    52,    52,    52,    53,    53,
      54,    54,    54,    55,    55,    55,    56,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    64,    65,    65,    65,
      66,    66,    67,    68,    68,    68,    69,    69,    69,    70,
      71,    72,    73,    73,    73,    73,    74,    74,    75,    75,
      76,    76,    77,    77,    78,    79,    80,    80,    81,    81,
      81,    81,    81,    81,    81,    82,    82,    83,    84,    85,
      86
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     3,     1,     1,     1,     1,     1,     2,
       1,     2,     2,     1,     2,     2,     2,     2,     1,     4,
       5,     2,     3,     9,     3,     1,     2,     1,     1,     2,
       2,     1,     9,     1,     2,     2,     1,     1,     1,    10,
      11,     6,     3,     4,     4,     5,     1,     3,     2,     3,
       3,     4,     2,     2,     3,     2,     3,     4,     0,     2,
       2,     2,     2,     2,     2,     1,     1,     1,     2,     3,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    18,     0,     4,     5,     6,     7,     0,     8,    38,
       0,     0,    36,    37,    31,     0,    28,     0,    27,     0,
      58,    46,     0,     0,    21,     1,     9,    52,     0,     0,
      30,    25,    29,     0,     0,     0,     0,     0,     2,     0,
       0,    48,     0,     0,    53,     0,     0,     0,     0,    21,
      26,     3,    42,     0,     0,     0,     0,    59,     0,    67,
      64,    63,    62,     0,    60,     0,    47,    61,     0,     0,
      66,    65,    33,     0,     0,    50,    49,     0,    19,     0,
       0,     0,    43,     0,    44,     0,     0,    55,     0,     0,
       0,    68,     0,    34,    10,    13,    35,     0,     0,    54,
      51,     0,    20,     0,     0,    45,     0,     0,    22,    56,
       0,    70,    69,    11,    16,    12,    14,    17,    15,     0,
      41,     0,     0,     0,     0,    57,     0,     0,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,     0,     0,     0,     0,    23,     0,    40,     0,     0,
      39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     7,     8,     9,    97,    98,    99,    10,    11,    12,
      13,    62,    63,   108,    30,    14,    15,    31,    74,    16,
     120,    36,    17,    18,    19,    20,    21,    22,    44,    65,
      66,    39,    67,    68,    69,    70,    71
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -136
static const yytype_int16 yypact[] =
{
     132,  -136,    42,  -136,  -136,  -136,  -136,    22,  -136,   125,
       9,    20,  -136,  -136,   143,    28,  -136,    79,  -136,   148,
    -136,  -136,    16,    18,     6,  -136,  -136,  -136,    32,    65,
     148,  -136,  -136,   148,    38,    40,    93,    91,  -136,    -1,
      63,  -136,    39,    27,  -136,    45,    26,    52,   -12,  -136,
    -136,  -136,  -136,    69,    57,    77,   104,  -136,    -3,  -136,
    -136,  -136,  -136,    94,  -136,    95,  -136,  -136,    -4,   103,
    -136,  -136,  -136,    67,   136,  -136,  -136,   106,  -136,    68,
     109,    87,  -136,    90,  -136,   144,     2,  -136,   138,   108,
     117,  -136,   118,  -136,  -136,  -136,   125,    -2,     3,  -136,
    -136,   125,  -136,   145,   131,  -136,   147,   146,  -136,  -136,
     121,  -136,  -136,  -136,  -136,  -136,  -136,  -136,  -136,    54,
    -136,   149,   130,   150,   152,  -136,   142,   151,   140,   153,
    -136,   154,   155,   156,   157,   158,   159,   137,   161,   160,
    -136,    63,   162,   163,   136,  -136,   164,  -136,    63,   136,
    -136
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -136,  -136,    -8,   -17,  -136,  -136,  -133,  -136,  -136,   165,
     166,  -136,  -136,  -136,  -136,  -136,  -136,  -136,  -135,    71,
    -136,  -136,  -136,  -136,    17,  -136,  -136,  -136,  -136,  -136,
    -136,  -136,   -64,  -136,  -136,  -136,  -136
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      35,    26,    57,   113,    90,    43,   144,    45,   116,     1,
      58,   147,    81,   149,   114,    86,   150,    27,    49,    56,
     117,    45,    25,    73,   106,    40,    28,    26,     3,     4,
       5,     6,    33,   115,   118,    26,    41,    45,     1,     3,
       4,     5,     6,    87,    91,    59,    59,    76,    26,    46,
      59,    47,     3,     4,     5,     6,    64,    96,    52,    75,
      23,    53,    42,    24,    78,    26,     3,     4,     5,     6,
      79,    80,   110,    77,    54,     3,     4,     5,     6,     3,
       4,     5,     6,    48,   119,    32,    49,   126,    26,    82,
      38,     3,     4,     5,     6,    72,    83,    84,    88,    93,
      34,    50,    26,    89,    51,     3,     4,     5,     6,    23,
      92,    26,    49,   101,    55,   103,     3,     4,     5,     6,
       3,     4,     5,     6,    73,    85,   100,    96,   109,   102,
     104,    73,    96,     3,     4,     5,     6,     3,     4,     5,
       6,   125,     1,     2,     3,     4,     5,     6,     3,     4,
       5,     6,    94,    95,    29,     3,     4,     5,     6,    37,
       3,     4,     5,     6,   105,   107,   111,   112,   121,   122,
     123,   128,   130,   124,   129,   127,   131,   133,   134,   141,
     132,     0,     0,   138,   145,   136,   142,     0,     0,   135,
     140,     0,     0,     0,   139,   137,     0,   143,     0,     0,
       0,   146,     0,   148,    60,    61
};

static const yytype_int16 yycheck[] =
{
      17,     9,     3,     5,     8,    22,   141,    19,     5,    10,
      11,   144,    24,   148,    16,    18,   149,     8,    21,    36,
      17,    19,     0,    40,    22,     9,     6,    35,    12,    13,
      14,    15,    15,    97,    98,    43,    20,    19,    10,    12,
      13,    14,    15,    46,    48,    47,    47,    20,    56,    43,
      47,    19,    12,    13,    14,    15,    39,    74,    20,    20,
      18,    21,    46,    21,    19,    73,    12,    13,    14,    15,
      44,    19,    89,    46,    34,    12,    13,    14,    15,    12,
      13,    14,    15,    18,   101,    14,    21,    33,    96,    20,
      19,    12,    13,    14,    15,    32,    39,    20,     4,    32,
      21,    30,   110,     8,    33,    12,    13,    14,    15,    18,
       7,   119,    21,    45,    21,    28,    12,    13,    14,    15,
      12,    13,    14,    15,   141,    21,    20,   144,    20,    20,
      40,   148,   149,    12,    13,    14,    15,    12,    13,    14,
      15,    20,    10,    11,    12,    13,    14,    15,    12,    13,
      14,    15,    16,    17,    11,    12,    13,    14,    15,    11,
      12,    13,    14,    15,    20,    27,    49,    49,    23,    38,
      23,    41,    20,    27,    24,    26,    34,    37,    25,    42,
      29,    -1,    -1,    26,    22,    30,    25,    -1,    -1,    35,
      31,    -1,    -1,    -1,    36,    39,    -1,    37,    -1,    -1,
      -1,    38,    -1,    39,    39,    39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    10,    11,    12,    13,    14,    15,    51,    52,    53,
      57,    58,    59,    60,    65,    66,    69,    72,    73,    74,
      75,    76,    77,    18,    21,     0,    52,     8,     6,    11,
      64,    67,    69,    74,    21,    53,    71,    11,    69,    81,
       9,    20,    46,    53,    78,    19,    43,    19,    18,    21,
      69,    69,    20,    21,    34,    21,    53,     3,    11,    47,
      59,    60,    61,    62,    74,    79,    80,    82,    83,    84,
      85,    86,    32,    53,    68,    20,    20,    46,    19,    44,
      19,    24,    20,    39,    20,    21,    18,    46,     4,     8,
       8,    48,     7,    32,    16,    17,    53,    54,    55,    56,
      20,    45,    20,    28,    40,    20,    22,    27,    63,    20,
      53,    49,    49,     5,    16,    82,     5,    17,    82,    53,
      70,    23,    38,    23,    27,    20,    33,    26,    41,    24,
      20,    34,    29,    37,    25,    35,    30,    39,    26,    36,
      31,    42,    25,    37,    68,    22,    38,    56,    39,    68,
      56
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
        case 10:

/* Line 1455 of yacc.c  */
#line 144 "xml.y"
    { (yyval.str) = new string; global_scan->setmode(XmlScan::AttValueSingleMode); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 145 "xml.y"
    { (yyval.str) = (yyvsp[(1) - (2)].str); *(yyval.str) += *(yyvsp[(2) - (2)].str); delete (yyvsp[(2) - (2)].str); global_scan->setmode(XmlScan::AttValueSingleMode); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 146 "xml.y"
    { (yyval.str) = (yyvsp[(1) - (2)].str); *(yyval.str) += (yyvsp[(2) - (2)].i); global_scan->setmode(XmlScan::AttValueSingleMode); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 147 "xml.y"
    { (yyval.str) = new string; global_scan->setmode(XmlScan::AttValueDoubleMode); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 148 "xml.y"
    { (yyval.str) = (yyvsp[(1) - (2)].str); *(yyval.str) += *(yyvsp[(2) - (2)].str); delete (yyvsp[(2) - (2)].str); global_scan->setmode(XmlScan::AttValueDoubleMode); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 149 "xml.y"
    { (yyval.str) = (yyvsp[(1) - (2)].str); *(yyval.str) += (yyvsp[(2) - (2)].i); global_scan->setmode(XmlScan::AttValueDoubleMode); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 150 "xml.y"
    { (yyval.str) = (yyvsp[(1) - (2)].str); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 151 "xml.y"
    { (yyval.str) = (yyvsp[(1) - (2)].str); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 152 "xml.y"
    { global_scan->setmode(XmlScan::NameMode); delete (yyvsp[(1) - (1)].str); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 153 "xml.y"
    { global_scan->setmode(XmlScan::CommentMode); delete (yyvsp[(1) - (4)].str); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 154 "xml.y"
    { delete (yyvsp[(2) - (5)].str); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 155 "xml.y"
    { delete (yyvsp[(1) - (2)].str); yyerror("Processing instructions are not supported"); YYERROR; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 156 "xml.y"
    { (yyval.str) = (yyvsp[(2) - (3)].str); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 157 "xml.y"
    { global_scan->setmode(XmlScan::CDataMode); delete (yyvsp[(1) - (9)].str); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 168 "xml.y"
    { delete (yyvsp[(1) - (9)].str); yyerror("DTD's not supported"); YYERROR; ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 176 "xml.y"
    { handler->setVersion(*(yyvsp[(10) - (10)].str)); delete (yyvsp[(10) - (10)].str); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 177 "xml.y"
    { handler->setEncoding(*(yyvsp[(11) - (11)].str)); delete (yyvsp[(11) - (11)].str); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 184 "xml.y"
    { handler->endElement((yyvsp[(1) - (1)].attr)->getelemURI(),(yyvsp[(1) - (1)].attr)->getelemName(),(yyvsp[(1) - (1)].attr)->getelemName()); delete (yyvsp[(1) - (1)].attr); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 185 "xml.y"
    { handler->endElement((yyvsp[(1) - (3)].attr)->getelemURI(),(yyvsp[(1) - (3)].attr)->getelemName(),(yyvsp[(1) - (3)].attr)->getelemName()); delete (yyvsp[(1) - (3)].attr); delete (yyvsp[(3) - (3)].str); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 187 "xml.y"
    { handler->startElement((yyvsp[(1) - (2)].attr)->getelemURI(),(yyvsp[(1) - (2)].attr)->getelemName(),(yyvsp[(1) - (2)].attr)->getelemName(),*(yyvsp[(1) - (2)].attr)); (yyval.attr) = (yyvsp[(1) - (2)].attr); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 188 "xml.y"
    { handler->startElement((yyvsp[(1) - (3)].attr)->getelemURI(),(yyvsp[(1) - (3)].attr)->getelemName(),(yyvsp[(1) - (3)].attr)->getelemName(),*(yyvsp[(1) - (3)].attr)); (yyval.attr) = (yyvsp[(1) - (3)].attr); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 189 "xml.y"
    { handler->startElement((yyvsp[(1) - (3)].attr)->getelemURI(),(yyvsp[(1) - (3)].attr)->getelemName(),(yyvsp[(1) - (3)].attr)->getelemName(),*(yyvsp[(1) - (3)].attr)); (yyval.attr) = (yyvsp[(1) - (3)].attr); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 190 "xml.y"
    { handler->startElement((yyvsp[(1) - (4)].attr)->getelemURI(),(yyvsp[(1) - (4)].attr)->getelemName(),(yyvsp[(1) - (4)].attr)->getelemName(),*(yyvsp[(1) - (4)].attr)); (yyval.attr) = (yyvsp[(1) - (4)].attr); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 192 "xml.y"
    { (yyval.attr) = new Attributes((yyvsp[(2) - (2)].str)); global_scan->setmode(XmlScan::SNameMode); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 193 "xml.y"
    { (yyval.attr) = (yyvsp[(1) - (2)].attr); (yyval.attr)->add_attribute( (yyvsp[(2) - (2)].pair)->name, (yyvsp[(2) - (2)].pair)->value); delete (yyvsp[(2) - (2)].pair); global_scan->setmode(XmlScan::SNameMode); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 194 "xml.y"
    { (yyval.pair) = new NameValue; (yyval.pair)->name = (yyvsp[(1) - (3)].str); (yyval.pair)->value = (yyvsp[(3) - (3)].str); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 195 "xml.y"
    { global_scan->setmode(XmlScan::NameMode); delete (yyvsp[(1) - (2)].str); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 196 "xml.y"
    { (yyval.str) = (yyvsp[(2) - (3)].str); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 197 "xml.y"
    { (yyval.str) = (yyvsp[(2) - (4)].str); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 199 "xml.y"
    { global_scan->setmode(XmlScan::CharDataMode); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 200 "xml.y"
    { print_content( *(yyvsp[(2) - (2)].str) ); delete (yyvsp[(2) - (2)].str); global_scan->setmode(XmlScan::CharDataMode); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 201 "xml.y"
    { global_scan->setmode(XmlScan::CharDataMode); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 202 "xml.y"
    { string *tmp=new string(); *tmp += (yyvsp[(2) - (2)].i); print_content(*tmp); delete tmp; global_scan->setmode(XmlScan::CharDataMode); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 203 "xml.y"
    { print_content( *(yyvsp[(2) - (2)].str) ); delete (yyvsp[(2) - (2)].str); global_scan->setmode(XmlScan::CharDataMode); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 204 "xml.y"
    { global_scan->setmode(XmlScan::CharDataMode); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 205 "xml.y"
    { global_scan->setmode(XmlScan::CharDataMode); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 207 "xml.y"
    { (yyval.i) = convertEntityRef(*(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 208 "xml.y"
    { (yyval.i) = convertCharRef(*(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 210 "xml.y"
    { global_scan->setmode(XmlScan::NameMode); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 211 "xml.y"
    { global_scan->setmode(XmlScan::CharRefMode); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 212 "xml.y"
    { (yyval.str) = (yyvsp[(2) - (3)].str); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 213 "xml.y"
    { (yyval.str) = (yyvsp[(2) - (3)].str); ;}
    break;



/* Line 1455 of yacc.c  */
#line 1883 "xml.cc"
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
#line 214 "xml.y"


XmlScan::XmlScan(istream &t) : s(t)

{
  curmode = SingleMode;
  lvalue = (string *)0;
  pos = 0;
  endofstream = false;
  getxmlchar(); getxmlchar(); getxmlchar(); getxmlchar(); // Fill lookahead buffer
}

XmlScan::~XmlScan(void)

{
  clearlvalue();
}

void XmlScan::clearlvalue(void)

{
  if (lvalue != (string *)0)
    delete lvalue;
}

int4 XmlScan::scanSingle(void)

{
  int4 res = getxmlchar();
  if (res == '<') {
    if (isInitialNameChar(next(0))) return ElementBraceToken;
    return CommandBraceToken;
  }
  return res;
}

int4 XmlScan::scanCharData(void)

{
  clearlvalue();
  lvalue = new string();
  
  while(next(0) != -1) {		// look for '<' '&' or ']]>'
    if (next(0) == '<') break;
    if (next(0) == '&') break;
    if (next(0) == ']')
      if (next(1)== ']')
	if (next(2)=='>')
	  break;
    *lvalue += getxmlchar();
  }
  if (lvalue->size()==0)
    return scanSingle();
  return CharDataToken;
}

int4 XmlScan::scanCData(void)

{
  clearlvalue();
  lvalue = new string();

  while(next(0) != -1) {	// Look for "]]>" and non-Char
    if (next(0)==']')
      if (next(1)==']')
	if (next(2)=='>')
	  break;
    if (!isChar(next(0))) break;
    *lvalue += getxmlchar();
  }
  return CDataToken;		// CData can be empty
}

int4 XmlScan::scanCharRef(void)

{
  int4 v;
  clearlvalue();
  lvalue = new string();
  if (next(0) == 'x') {
    *lvalue += getxmlchar();
    while(next(0) != -1) {
      v = next(0);
      if (v < '0') break;
      if ((v>'9')&&(v<'A')) break;
      if ((v>'F')&&(v<'a')) break;
      if (v>'f') break;
      *lvalue += getxmlchar();
    }
    if (lvalue->size()==1)
      return 'x';		// Must be at least 1 hex digit
  }
  else {
    while(next(0) != -1) {
      v = next(0);
      if (v<'0') break;
      if (v>'9') break;
      *lvalue += getxmlchar();
    }
    if (lvalue->size()==0)
      return scanSingle();
  }
  return CharRefToken;
}

int4 XmlScan::scanAttValue(int4 quote)

{
  clearlvalue();
  lvalue = new string();
  while(next(0) != -1) {
    if (next(0) == quote) break;
    if (next(0) == '<') break;
    if (next(0) == '&') break;
    *lvalue += getxmlchar();
  }
  if (lvalue->size() == 0)
    return scanSingle();
  return AttValueToken;
}

int4 XmlScan::scanComment(void)

{
  clearlvalue();
  lvalue = new string();

  while(next(0) != -1) {
    if (next(0)=='-')
      if (next(1)=='-')
	break;
    if (!isChar(next(0))) break;
    *lvalue += getxmlchar();
  }
  return CommentToken;
}

int4 XmlScan::scanName(void)

{
  clearlvalue();
  lvalue = new string();

  if (!isInitialNameChar(next(0)))
    return scanSingle();
  *lvalue += getxmlchar();
  while(next(0) != -1) {
    if (!isNameChar(next(0))) break;
    *lvalue += getxmlchar();
  }
  return NameToken;
}

int4 XmlScan::scanSName(void)

{
  int4 whitecount = 0;
  while((next(0)==' ')||(next(0)=='\n')||(next(0)=='\r')||(next(0)=='\t')) {
    whitecount += 1;
    getxmlchar();
  }
  clearlvalue();
  lvalue = new string();
  if (!isInitialNameChar(next(0))) {	// First non-whitespace is not Name char
    if (whitecount > 0)
      return ' ';
    return scanSingle();
  }
  *lvalue += getxmlchar();
  while(next(0) != -1) {
    if (!isNameChar(next(0))) break;
    *lvalue += getxmlchar();
  }
  if (whitecount>0)
    return SNameToken;
  return NameToken;
}

bool XmlScan::isInitialNameChar(int4 val)

{
  if (isLetter(val)) return true;
  if ((val=='_')||(val==':')) return true;
  return false;
}

bool XmlScan::isNameChar(int4 val)

{
  if (isLetter(val)) return true;
  if ((val>='0')&&(val<='9')) return true;
  if ((val=='.')||(val=='-')||(val=='_')||(val==':')) return true;
  return false;
}

bool XmlScan::isChar(int4 val)

{
  if (val>=0x20) return true;
  if ((val == 0xd)||(val==0xa)||(val==0x9)) return true;
  return false;
}

int4 XmlScan::nexttoken(void)

{
  mode mymode = curmode;
  curmode = SingleMode;
  switch(mymode) {
  case CharDataMode:
    return scanCharData();
  case CDataMode:
    return scanCData();
  case AttValueSingleMode:
    return scanAttValue('\'');
  case AttValueDoubleMode:
    return scanAttValue('"');
  case CommentMode:
    return scanComment();
  case CharRefMode:
    return scanCharRef();
  case NameMode:
    return scanName();
  case SNameMode:
    return scanSName();
  case SingleMode:
    return scanSingle();
  }
  return -1;
}

void print_content(const string &str)

{
  uint4 i;
  for(i=0;i<str.size();++i) {
    if (str[i]==' ') continue;
    if (str[i]=='\n') continue;
    if (str[i]=='\r') continue;
    if (str[i]=='\t') continue;
    break;
  }
  if (i==str.size())
    handler->ignorableWhitespace(str.c_str(),0,str.size());
  else
    handler->characters(str.c_str(),0,str.size());  
}

int4 convertEntityRef(const string &ref)

{
  if (ref == "lt") return '<';
  if (ref == "amp") return '&';
  if (ref == "gt") return '>';
  if (ref == "quot") return '"';
  if (ref == "apos") return '\'';
  return -1;
}

int4 convertCharRef(const string &ref)

{
  uint4 i;
  int4 mult,val,cur;

  if (ref[0]=='x') {
    i = 1;
    mult = 16;
  }
  else {
    i = 0;
    mult = 10;
  }
  val = 0;
  for(;i<ref.size();++i) {
    if (ref[i]<='9') cur = ref[i]-'0';
    else if (ref[i]<='F') cur = 10+ref[i]-'A';
    else cur=10+ref[i]-'a';
    val *= mult;
    val += cur;
  }
  return val;
}

int yylex(void)

{
  int res = global_scan->nexttoken();
  if (res>255)
    yylval.str = global_scan->lval();
  return res;
}

int yyerror(const char *str)

{
  handler->setError(str);
  return 0;
}

int4 xml_parse(istream &i,ContentHandler *hand,int4 dbg)

{
#if YYDEBUG
  yydebug = dbg;
#endif
  global_scan = new XmlScan(i);
  handler = hand;
  handler->startDocument();
  int4 res = yyparse();
  if (res == 0)
    handler->endDocument();
  delete global_scan;
  return res;
}

void TreeHandler::startElement(const string &namespaceURI,const string &localName,
			       const string &qualifiedName,const Attributes &atts)
{
  Element *newel = new Element(cur);
  cur->addChild(newel);
  cur = newel;
  newel->setName(localName);
  for(int4 i=0;i<atts.getLength();++i)
    newel->addAttribute(atts.getLocalName(i),atts.getValue(i));
}

void TreeHandler::endElement(const string &namespaceURI,const string &localName,
			     const string &qualifiedName)
{
  cur = cur->getParent();
}

void TreeHandler::characters(const char *text,int4 start,int4 length)

{
  cur->addContent(text,start,length);
}

Element::~Element(void)

{
  List::iterator iter;
  
  for(iter=children.begin();iter!=children.end();++iter)
    delete *iter;
}

const string &Element::getAttributeValue(const string &nm) const

{
  for(uint4 i=0;i<attr.size();++i)
    if (attr[i] == nm)
      return value[i];
  throw XmlError("Unknown attribute: "+nm);
}

DocumentStorage::~DocumentStorage(void)

{
  for(int4 i=0;i<doclist.size();++i) {
    if (doclist[i] != (Document *)0)
      delete doclist[i];
  }
}

Document *DocumentStorage::parseDocument(istream &s)

{
  doclist.push_back((Document *)0);
  doclist.back() = xml_tree(s);
  return doclist.back();
}

Document *DocumentStorage::openDocument(const string &filename)

{
  ifstream s(filename.c_str());
  if (!s)
    throw XmlError("Unable to open xml document "+filename);
  Document *res = parseDocument(s);
  s.close();
  return res;
}

void DocumentStorage::registerTag(const Element *el)

{
  tagmap[el->getName()] = el;
}

const Element *DocumentStorage::getTag(const string &nm) const

{
  map<string,const Element *>::const_iterator iter;

  iter = tagmap.find(nm);
  if (iter != tagmap.end())
    return (*iter).second;
  return (const Element *)0;
}

Document *xml_tree(istream &i)

{
  Document *doc = new Document();
  TreeHandler handle(doc);
  if (0!=xml_parse(i,&handle)) {
    delete doc;
    throw XmlError(handle.getError());
  }
  return doc;
}

void xml_escape(ostream &s,const char *str)

{
  while(*str!='\0') {
    if (*str < '?') {
      if (*str=='<') s << "&lt;";
      else if (*str=='>') s << "&gt;";
      else if (*str=='&') s << "&amp;";
      else if (*str=='"') s << "&quot;";
      else if (*str=='\'') s << "&apos;";
      else s << *str;
    }
    else
      s << *str;
    str++;
  }
}

