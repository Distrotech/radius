/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "gram.y"

/* This file is part of GNU Radius.
   Copyright (C) 2000,2001,2002,2003,2004,2006,
   2007 Free Software Foundation, Inc.

   Written by Sergey Poznyakoff
  
   GNU Radius is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
  
   GNU Radius is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with GNU Radius; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#if defined(HAVE_CONFIG_H)        
# include <config.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <netinet/in.h>
         
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#include <common.h>
#include <radtest.h>

#define YYERROR_VERBOSE 1
	
extern int yylex();

/* Local variables */ 
static int current_nesting_level; /* Nesting level of WHILE/DO statements */
static int error_count;           /* In interactive mode: number of errors
				     in the statement being compiled. Gets
				     reset to 0 after each statement.
				     In batch mode: total number of errors
				     encountered so far */
 
static struct {                   /* Definition of the function currently
				     being compiled */
	char *function;           /* Function name */
	grad_locus_t locus;       /* Location of the beginning of
				     the definition */
} defn;

/* Context stack support. This is used for improving diagnostic
   messages and after-error synchronization */
   
struct context_stack {
	struct context_stack *next;
	enum context ctx;
};

static struct context_stack *context_stack;
 
static enum context 
push_ctx(enum context ctx)
{
	struct context_stack *p = grad_emalloc(sizeof(*p));
	p->ctx = ctx;
	p->next = context_stack;
	context_stack = p;
	return ctx;
}

enum context
pop_ctx()
{
	enum context ctx;
	struct context_stack *p = context_stack;
	
	if (!context_stack)
		return ctx_none;
	ctx = p->ctx;
	context_stack = p->next;
	grad_free(p);
	return ctx;
}

enum context
peek_ctx()
{
	return context_stack ? context_stack->ctx : ctx_none;
}

/* Forward declarations */
static void run_statement(radtest_node_t *node);
static void errsync();
int yyerror(char *s);
 

/* Line 371 of yacc.c  */
#line 178 "gram.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_GRAM_H_INCLUDED
# define YY_YY_GRAM_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EOL = 258,
     AUTH = 259,
     ACCT = 260,
     SEND = 261,
     EXPECT = 262,
     T_BEGIN = 263,
     T_END = 264,
     IF = 265,
     ELSE = 266,
     WHILE = 267,
     DO = 268,
     BREAK = 269,
     CONTINUE = 270,
     INPUT = 271,
     SHIFT = 272,
     GETOPT = 273,
     CASE = 274,
     IN = 275,
     T_RETURN = 276,
     SET = 277,
     PRINT = 278,
     EXIT = 279,
     T_BOGUS = 280,
     ARGCOUNT = 281,
     IDENT = 282,
     PARM = 283,
     NAME = 284,
     NUMBER = 285,
     QUOTE = 286,
     BSTRING = 287,
     IPADDRESS = 288,
     PRITEM = 289,
     OR = 290,
     AND = 291,
     NE = 292,
     EQ = 293,
     GE = 294,
     GT = 295,
     LE = 296,
     LT = 297,
     NOT = 298,
     UMINUS = 299
   };
#endif
/* Tokens.  */
#define EOL 258
#define AUTH 259
#define ACCT 260
#define SEND 261
#define EXPECT 262
#define T_BEGIN 263
#define T_END 264
#define IF 265
#define ELSE 266
#define WHILE 267
#define DO 268
#define BREAK 269
#define CONTINUE 270
#define INPUT 271
#define SHIFT 272
#define GETOPT 273
#define CASE 274
#define IN 275
#define T_RETURN 276
#define SET 277
#define PRINT 278
#define EXIT 279
#define T_BOGUS 280
#define ARGCOUNT 281
#define IDENT 282
#define PARM 283
#define NAME 284
#define NUMBER 285
#define QUOTE 286
#define BSTRING 287
#define IPADDRESS 288
#define PRITEM 289
#define OR 290
#define AND 291
#define NE 292
#define EQ 293
#define GE 294
#define GT 295
#define LE 296
#define LT 297
#define NOT 298
#define UMINUS 299



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 149 "gram.y"

	int i;
	long number;
	char *string;
	radtest_node_t *node;
	radtest_node_deref_var_t deref;
	radtest_node_deref_parm_t parm;
	grad_uint32_t ipaddr;
	enum grad_operator op;
	radtest_pair_t *pair;
	grad_list_t *list;
	radtest_variable_t *var;
	grad_symtab_t *symtab;
	struct {
		int argc;
		char **argv;
	} set;
	radtest_case_branch_t *case_branch;
	radtest_function_t *fun;
	radtest_bstring_t bstring;


/* Line 387 of yacc.c  */
#line 332 "gram.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

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

#endif /* !YY_YY_GRAM_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 360 "gram.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
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

# define YYCOPY_NEEDED 1

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

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  66
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   513

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  124
/* YYNRULES -- Number of states.  */
#define YYNSTATES  199

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   299

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    47,     2,     2,
      51,    52,    45,    43,    54,    44,     2,    46,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    50,     2,    53,     2,     2,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    48,    49
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     9,    11,    14,    16,    19,
      21,    24,    27,    28,    30,    34,    39,    46,    52,    53,
      59,    60,    61,    70,    73,    77,    83,    87,    90,    93,
      96,    98,   102,   104,   107,   111,   114,   119,   124,   126,
     128,   130,   132,   135,   138,   140,   143,   148,   150,   153,
     154,   156,   158,   160,   162,   163,   165,   167,   169,   171,
     173,   174,   176,   178,   181,   185,   186,   188,   190,   192,
     195,   199,   203,   207,   211,   215,   219,   223,   227,   231,
     233,   235,   240,   244,   250,   254,   258,   262,   266,   270,
     273,   276,   277,   279,   281,   283,   285,   287,   292,   293,
     295,   297,   299,   301,   303,   305,   309,   310,   312,   313,
     315,   317,   320,   324,   327,   331,   333,   335,   337,   339,
     341,   343,   345,   349,   352
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      56,     0,    -1,    -1,    57,    -1,    57,    61,    -1,    59,
      -1,    57,    59,    -1,    59,    -1,    58,    59,    -1,     3,
      -1,    61,     3,    -1,     1,     3,    -1,    -1,     3,    -1,
       8,    58,     9,    -1,    66,    84,    60,    61,    -1,    66,
      84,    60,    61,    70,    61,    -1,    67,    86,    71,    72,
       9,    -1,    -1,    69,    62,    84,     3,    61,    -1,    -1,
      -1,    68,     3,    63,    61,     3,    12,    64,    84,    -1,
      23,    96,    -1,    29,    38,    86,    -1,     6,    80,    78,
      79,    83,    -1,     7,    79,    83,    -1,    24,    87,    -1,
      14,    77,    -1,    15,    77,    -1,    16,    -1,    16,    86,
      29,    -1,    22,    -1,    17,    87,    -1,    65,    58,     9,
      -1,    21,    87,    -1,    29,    51,    91,    52,    -1,    29,
       3,     8,     3,    -1,    10,    -1,    19,    -1,    13,    -1,
      12,    -1,    11,    60,    -1,    20,    60,    -1,    73,    -1,
      72,    73,    -1,    86,    52,    61,    74,    -1,     3,    -1,
      74,     3,    -1,    -1,    29,    -1,    29,    -1,    31,    -1,
      32,    -1,    -1,    30,    -1,     4,    -1,     5,    -1,    30,
      -1,    29,    -1,    -1,    81,    -1,    82,    -1,    81,    82,
      -1,    29,    38,    30,    -1,    -1,    93,    -1,    86,    -1,
      85,    -1,    48,    84,    -1,    51,    84,    52,    -1,    84,
      36,    84,    -1,    84,    35,    84,    -1,    84,    38,    84,
      -1,    84,    42,    84,    -1,    84,    40,    84,    -1,    84,
      37,    84,    -1,    84,    41,    84,    -1,    84,    39,    84,
      -1,    86,    -1,    88,    -1,    18,    76,    75,    75,    -1,
      51,    86,    52,    -1,    86,    50,    29,    89,    53,    -1,
      86,    43,    86,    -1,    86,    44,    86,    -1,    86,    45,
      86,    -1,    86,    46,    86,    -1,    86,    47,    86,    -1,
      44,    86,    -1,    43,    86,    -1,    -1,    86,    -1,    90,
      -1,    27,    -1,    28,    -1,    26,    -1,    29,    51,    91,
      52,    -1,    -1,    45,    -1,    30,    -1,    33,    -1,    31,
      -1,    32,    -1,    29,    -1,    51,    92,    52,    -1,    -1,
      96,    -1,    -1,    93,    -1,    94,    -1,    93,    94,    -1,
      93,    54,    94,    -1,    93,     1,    -1,    29,    95,    86,
      -1,    38,    -1,    42,    -1,    40,    -1,    37,    -1,    41,
      -1,    39,    -1,    97,    -1,    96,    54,    97,    -1,    96,
      97,    -1,    86,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   173,   173,   174,   175,   178,   182,   188,   194,   202,
     206,   217,   225,   226,   229,   234,   242,   250,   258,   258,
     267,   267,   267,   275,   280,   286,   294,   300,   305,   313,
     321,   330,   339,   345,   350,   357,   365,   380,   409,   415,
     421,   427,   433,   439,   442,   447,   454,   463,   464,   468,
     471,   474,   475,   476,   484,   487,   493,   497,   503,   507,
     516,   519,   522,   531,   540,   549,   552,   559,   562,   563,
     569,   573,   580,   587,   594,   601,   608,   615,   622,   631,
     634,   635,   653,   657,   673,   680,   687,   694,   701,   708,
     714,   721,   724,   727,   732,   737,   742,   746,   764,   767,
     773,   778,   783,   788,   793,   798,   806,   809,   813,   816,
     819,   824,   829,   834,   837,   850,   854,   858,   862,   866,
     870,   876,   881,   886,   893
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EOL", "AUTH", "ACCT", "SEND", "EXPECT",
  "T_BEGIN", "T_END", "IF", "ELSE", "WHILE", "DO", "BREAK", "CONTINUE",
  "INPUT", "SHIFT", "GETOPT", "CASE", "IN", "T_RETURN", "SET", "PRINT",
  "EXIT", "T_BOGUS", "ARGCOUNT", "IDENT", "PARM", "NAME", "NUMBER",
  "QUOTE", "BSTRING", "IPADDRESS", "PRITEM", "OR", "AND", "NE", "EQ", "GE",
  "GT", "LE", "LT", "'+'", "'-'", "'*'", "'/'", "'%'", "NOT", "UMINUS",
  "'['", "'('", "')'", "']'", "','", "$accept", "program", "input", "list",
  "lstmt", "maybe_eol", "stmt", "$@1", "$@2", "$@3", "function_def", "if",
  "case", "do", "while", "else", "in", "caselist", "casecond", "nls",
  "name", "string", "nesting_level", "port_type", "req_code", "send_flags",
  "send_flag_list", "send_flag", "expr_or_pair_list", "cond", "bool",
  "expr", "maybe_expr", "value", "closure", "imm_value", "maybe_prlist",
  "maybe_pair_list", "pair_list", "pair", "op", "prlist", "pritem", YY_NULL
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
     295,   296,   297,    43,    45,    42,    47,    37,   298,   299,
      91,    40,    41,    93,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    56,    56,    57,    57,    58,    58,    59,
      59,    59,    60,    60,    61,    61,    61,    61,    62,    61,
      63,    64,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    65,    66,    67,
      68,    69,    70,    71,    72,    72,    73,    74,    74,    75,
      75,    76,    76,    76,    77,    77,    78,    78,    79,    79,
      80,    80,    81,    81,    82,    83,    83,    83,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    85,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    87,    87,    88,    88,    88,    88,    88,    89,    89,
      90,    90,    90,    90,    90,    90,    91,    91,    92,    92,
      93,    93,    93,    93,    94,    95,    95,    95,    95,    95,
      95,    96,    96,    96,    97
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     2,     1,     2,     1,     2,     1,
       2,     2,     0,     1,     3,     4,     6,     5,     0,     5,
       0,     0,     8,     2,     3,     5,     3,     2,     2,     2,
       1,     3,     1,     2,     3,     2,     4,     4,     1,     1,
       1,     1,     2,     2,     1,     2,     4,     1,     2,     0,
       1,     1,     1,     1,     0,     1,     1,     1,     1,     1,
       0,     1,     1,     2,     3,     0,     1,     1,     1,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     4,     3,     5,     3,     3,     3,     3,     3,     2,
       2,     0,     1,     1,     1,     1,     1,     4,     0,     1,
       1,     1,     1,     1,     1,     3,     0,     1,     0,     1,
       1,     2,     3,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     2,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     9,    60,     0,     0,    38,    41,    40,    54,
      54,    30,    91,    39,    91,    32,     0,    91,     0,     0,
       0,     5,     0,     0,     0,     0,     0,    18,    11,     0,
       0,    61,    62,    59,    58,    65,     0,     7,    55,    28,
      29,     0,    96,    94,    95,   104,   100,   102,   103,   101,
       0,     0,   108,     0,    80,    93,    92,    33,    35,   124,
      23,   121,    27,     0,     0,   106,     1,     6,     4,    10,
       0,     0,   108,    12,    68,    79,     0,    20,     0,     0,
      56,    57,     0,    63,   104,    26,    67,     0,   110,    14,
       8,    51,    52,    53,    49,   106,    90,    89,     0,     0,
       0,    31,     0,     0,     0,     0,     0,     0,     0,   123,
       0,    24,     0,   107,    34,    69,     0,    79,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
       0,     0,    64,    65,   118,   115,   120,   117,   119,   116,
       0,   113,     0,     0,   111,    50,    49,     0,    82,   105,
      84,    85,    86,    87,    88,    98,   122,    37,    36,    70,
      72,    71,    76,    73,    78,    75,    77,    74,    15,    43,
       0,    44,     0,     0,     0,    25,   114,   112,    81,    97,
      99,     0,    12,     0,    17,    45,     0,     0,    19,    83,
      42,    16,     0,    21,    47,    46,     0,    48,    22
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    19,    20,    36,    37,   127,    22,    78,   130,   196,
      23,    24,    25,    26,    27,   183,   129,   170,   171,   195,
     146,    94,    39,    82,    35,    30,    31,    32,    85,    73,
      74,    75,    57,    54,   181,    55,   112,    99,    87,    88,
     140,   113,    61
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -126
static const yytype_int16 yypact[] =
{
     326,    21,  -126,    -3,   -16,   423,  -126,  -126,  -126,    13,
      13,   253,   253,  -126,   253,  -126,   253,   253,    48,    39,
     351,  -126,    50,   423,   199,   253,    53,  -126,  -126,    14,
      25,    -3,  -126,  -126,  -126,   280,   375,  -126,  -126,  -126,
    -126,    16,  -126,  -126,  -126,     7,  -126,  -126,  -126,  -126,
     253,   253,   280,   161,  -126,  -126,   244,  -126,  -126,   244,
     170,  -126,  -126,    51,   253,   253,  -126,  -126,    50,  -126,
     399,   199,   226,   137,  -126,   244,   190,  -126,   199,    30,
    -126,  -126,   -16,  -126,    -5,  -126,   244,     9,  -126,  -126,
    -126,  -126,  -126,  -126,    44,   253,    26,    26,   439,    27,
      20,  -126,   253,   253,   253,   253,   253,    52,   253,  -126,
      74,   244,    45,   170,  -126,  -126,   438,   439,  -126,   199,
     199,   199,   199,   199,   199,   199,   199,   443,    97,   253,
     443,   145,  -126,   280,  -126,  -126,  -126,  -126,  -126,  -126,
     253,  -126,   105,    72,  -126,  -126,    44,    54,  -126,  -126,
      37,    37,    26,    26,    26,    57,  -126,  -126,  -126,  -126,
     278,   465,   471,   471,    49,    49,    49,    49,    93,  -126,
     106,  -126,   449,   102,   443,  -126,   244,  -126,  -126,  -126,
    -126,    55,    97,   443,  -126,  -126,   443,    99,  -126,  -126,
    -126,  -126,   109,  -126,  -126,   110,   199,  -126,   225
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -126,  -126,  -126,    86,     8,  -125,   -20,  -126,  -126,  -126,
    -126,  -126,  -126,  -126,  -126,  -126,  -126,  -126,   -56,  -126,
     -30,  -126,   107,  -126,    36,  -126,  -126,    89,   -12,   -55,
    -126,   -10,     5,  -126,  -126,  -126,    31,  -126,   -41,   -82,
    -126,   111,   -33
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -110
static const yytype_int16 yytable[] =
{
      68,    53,    56,   169,    56,   144,    59,    56,    21,   -66,
     141,   100,   -66,    33,    34,    76,   115,   116,   144,    58,
     -66,   141,    62,   131,    28,    86,    29,   109,    67,    80,
      81,   100,   134,   135,   136,   137,   138,   139,   142,    66,
      96,    97,    98,    38,    90,    91,    95,    92,    93,   142,
      59,    63,    79,    69,   111,    59,    77,   190,    95,   110,
     132,   177,   117,   143,   160,   161,   162,   163,   164,   165,
     166,   167,  -109,   145,   143,   156,   107,   157,    90,   149,
     109,   155,   104,   105,   106,    59,    64,   107,  -110,  -110,
    -110,  -110,   150,   151,   152,   153,   154,   158,    59,    65,
     118,   142,   180,    59,   182,   187,   179,   168,   189,    70,
     173,   193,   194,   197,   185,   184,   178,    40,   133,   172,
      83,   175,     0,    86,    41,     0,   147,    60,     0,     0,
     176,     0,    42,    43,    44,    45,    46,    47,    48,    49,
     118,   198,   134,   135,   136,   137,   138,   139,   174,    50,
      51,     0,     0,     0,   188,     0,     0,    52,     0,     0,
     172,     0,     0,   191,     0,     0,   192,     0,     0,     0,
       0,     0,   119,   120,   121,   122,   123,   124,   125,   126,
     119,   120,   121,   122,   123,   124,   125,   126,    41,     0,
     101,     0,     0,     0,     0,     0,    42,    43,    44,    45,
      46,    47,    48,    49,   102,   103,   104,   105,   106,     0,
     128,   107,     0,    50,    51,     0,     0,    41,     0,     0,
       0,    52,     0,     0,   108,    42,    43,    44,    45,    46,
      47,    48,    49,   102,   103,   104,   105,   106,     0,     0,
     107,     0,    50,    51,    41,     0,     0,    71,     0,     0,
      72,     0,    42,    43,    44,    84,    46,    47,    48,    49,
     119,   120,   121,   122,   123,   124,   125,   126,     0,    50,
      51,    41,     0,     0,    71,     0,     0,    72,     0,    42,
      43,    44,    45,    46,    47,    48,    49,   102,   103,   104,
     105,   106,     0,     0,   107,     0,    50,    51,    41,     0,
       0,     0,     0,     0,    52,     0,    42,    43,    44,    84,
      46,    47,    48,    49,   120,   121,   122,   123,   124,   125,
     126,     0,     0,    50,    51,     0,    -2,     1,     0,     2,
       0,    52,     3,     4,     5,     0,     6,     0,     7,     8,
       9,    10,    11,    12,     0,    13,     0,    14,    15,    16,
      17,    -3,     1,     0,     2,    18,     0,     3,     4,     5,
       0,     6,     0,     7,     8,     9,    10,    11,    12,     0,
      13,     0,    14,    15,    16,    17,     1,     0,     2,     0,
      18,     3,     4,     5,    89,     6,     0,     7,     8,     9,
      10,    11,    12,     0,    13,     0,    14,    15,    16,    17,
       1,     0,     2,     0,    18,     3,     4,     5,   114,     6,
       0,     7,     8,     9,    10,    11,    12,     0,    13,     0,
      14,    15,    16,    17,     1,     0,     2,     0,    18,     3,
       4,     5,     0,     6,     0,     7,     8,     9,    10,    11,
      12,     0,    13,     0,    14,    15,    16,    17,     0,     3,
       4,     5,    18,     6,     0,     7,     8,     9,    10,    11,
      12,     0,    13,     0,    14,    15,    16,    17,     0,     0,
       0,     0,    18,   119,   120,   121,   122,   123,   124,   125,
     126,     0,   102,   103,   104,   105,   106,     0,     0,   107,
     159,   148,   102,   103,   104,   105,   106,     0,     0,   107,
       0,   186,   121,   122,   123,   124,   125,   126,  -110,  -110,
     123,   124,   125,   126
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-126)))

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-110)))

static const yytype_int16 yycheck[] =
{
      20,    11,    12,   128,    14,    87,    16,    17,     0,     0,
       1,    52,     3,    29,    30,    25,    71,    72,   100,    14,
      11,     1,    17,    78,     3,    35,    29,    60,    20,     4,
       5,    72,    37,    38,    39,    40,    41,    42,    29,     0,
      50,    51,    52,    30,    36,    29,    51,    31,    32,    29,
      60,     3,    38,     3,    64,    65,     3,   182,    51,     8,
      30,   143,    72,    54,   119,   120,   121,   122,   123,   124,
     125,   126,    52,    29,    54,   108,    50,     3,    70,    52,
     113,    29,    45,    46,    47,    95,    38,    50,    39,    40,
      41,    42,   102,   103,   104,   105,   106,    52,   108,    51,
       3,    29,    45,   113,    11,     3,    52,   127,    53,    23,
     130,    12,     3,     3,   170,     9,   146,    10,    82,   129,
      31,   133,    -1,   133,    18,    -1,    95,    16,    -1,    -1,
     140,    -1,    26,    27,    28,    29,    30,    31,    32,    33,
       3,   196,    37,    38,    39,    40,    41,    42,     3,    43,
      44,    -1,    -1,    -1,   174,    -1,    -1,    51,    -1,    -1,
     170,    -1,    -1,   183,    -1,    -1,   186,    -1,    -1,    -1,
      -1,    -1,    35,    36,    37,    38,    39,    40,    41,    42,
      35,    36,    37,    38,    39,    40,    41,    42,    18,    -1,
      29,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,    29,
      30,    31,    32,    33,    43,    44,    45,    46,    47,    -1,
      20,    50,    -1,    43,    44,    -1,    -1,    18,    -1,    -1,
      -1,    51,    -1,    -1,    54,    26,    27,    28,    29,    30,
      31,    32,    33,    43,    44,    45,    46,    47,    -1,    -1,
      50,    -1,    43,    44,    18,    -1,    -1,    48,    -1,    -1,
      51,    -1,    26,    27,    28,    29,    30,    31,    32,    33,
      35,    36,    37,    38,    39,    40,    41,    42,    -1,    43,
      44,    18,    -1,    -1,    48,    -1,    -1,    51,    -1,    26,
      27,    28,    29,    30,    31,    32,    33,    43,    44,    45,
      46,    47,    -1,    -1,    50,    -1,    43,    44,    18,    -1,
      -1,    -1,    -1,    -1,    51,    -1,    26,    27,    28,    29,
      30,    31,    32,    33,    36,    37,    38,    39,    40,    41,
      42,    -1,    -1,    43,    44,    -1,     0,     1,    -1,     3,
      -1,    51,     6,     7,     8,    -1,    10,    -1,    12,    13,
      14,    15,    16,    17,    -1,    19,    -1,    21,    22,    23,
      24,     0,     1,    -1,     3,    29,    -1,     6,     7,     8,
      -1,    10,    -1,    12,    13,    14,    15,    16,    17,    -1,
      19,    -1,    21,    22,    23,    24,     1,    -1,     3,    -1,
      29,     6,     7,     8,     9,    10,    -1,    12,    13,    14,
      15,    16,    17,    -1,    19,    -1,    21,    22,    23,    24,
       1,    -1,     3,    -1,    29,     6,     7,     8,     9,    10,
      -1,    12,    13,    14,    15,    16,    17,    -1,    19,    -1,
      21,    22,    23,    24,     1,    -1,     3,    -1,    29,     6,
       7,     8,    -1,    10,    -1,    12,    13,    14,    15,    16,
      17,    -1,    19,    -1,    21,    22,    23,    24,    -1,     6,
       7,     8,    29,    10,    -1,    12,    13,    14,    15,    16,
      17,    -1,    19,    -1,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    35,    36,    37,    38,    39,    40,    41,
      42,    -1,    43,    44,    45,    46,    47,    -1,    -1,    50,
      52,    52,    43,    44,    45,    46,    47,    -1,    -1,    50,
      -1,    52,    37,    38,    39,    40,    41,    42,    37,    38,
      39,    40,    41,    42
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     6,     7,     8,    10,    12,    13,    14,
      15,    16,    17,    19,    21,    22,    23,    24,    29,    56,
      57,    59,    61,    65,    66,    67,    68,    69,     3,    29,
      80,    81,    82,    29,    30,    79,    58,    59,    30,    77,
      77,    18,    26,    27,    28,    29,    30,    31,    32,    33,
      43,    44,    51,    86,    88,    90,    86,    87,    87,    86,
      96,    97,    87,     3,    38,    51,     0,    59,    61,     3,
      58,    48,    51,    84,    85,    86,    86,     3,    62,    38,
       4,     5,    78,    82,    29,    83,    86,    93,    94,     9,
      59,    29,    31,    32,    76,    51,    86,    86,    86,    92,
      93,    29,    43,    44,    45,    46,    47,    50,    54,    97,
       8,    86,    91,    96,     9,    84,    84,    86,     3,    35,
      36,    37,    38,    39,    40,    41,    42,    60,    20,    71,
      63,    84,    30,    79,    37,    38,    39,    40,    41,    42,
      95,     1,    29,    54,    94,    29,    75,    91,    52,    52,
      86,    86,    86,    86,    86,    29,    97,     3,    52,    52,
      84,    84,    84,    84,    84,    84,    84,    84,    61,    60,
      72,    73,    86,    61,     3,    83,    86,    94,    75,    52,
      45,    89,    11,    70,     9,    73,    52,     3,    61,    53,
      60,    61,    61,    12,     3,    74,    64,     3,    84
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
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
  FILE *yyo = yyoutput;
  YYUSE (yyo);
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
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




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

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

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
        case 5:
/* Line 1792 of yacc.c  */
#line 179 "gram.y"
    {
			run_statement((yyvsp[(1) - (1)].node));
		}
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 183 "gram.y"
    {
			run_statement((yyvsp[(2) - (2)].node));
		}
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 189 "gram.y"
    {
			(yyval.list) = grad_list_create();
			if ((yyvsp[(1) - (1)].node))
				grad_list_append((yyval.list), (yyvsp[(1) - (1)].node));
		}
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 195 "gram.y"
    {
			if ((yyvsp[(2) - (2)].node)) 
				grad_list_append((yyvsp[(1) - (2)].list), (yyvsp[(2) - (2)].node));
			(yyval.list) = (yyvsp[(1) - (2)].list);
		}
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 203 "gram.y"
    {
			(yyval.node) = NULL;
		}
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 207 "gram.y"
    {
			switch (peek_ctx()) {
			case ctx_iferr:
			case ctx_doerr:
				pop_ctx();
				break;
			default:
				break;
			}
		}
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 218 "gram.y"
    {
			errsync();
                        yyclearin;
                        yyerrok;
                }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 230 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_stmt);
			(yyval.node)->v.list = (yyvsp[(2) - (3)].list);			
		}
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 235 "gram.y"
    {
			pop_ctx();
			(yyval.node) = radtest_node_alloc(radtest_node_cond);
			(yyval.node)->v.cond.cond = (yyvsp[(2) - (4)].node);
			(yyval.node)->v.cond.iftrue = (yyvsp[(4) - (4)].node);
			(yyval.node)->v.cond.iffalse = NULL;
		}
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 243 "gram.y"
    {
			pop_ctx();
			(yyval.node) = radtest_node_alloc(radtest_node_cond);
			(yyval.node)->v.cond.cond = (yyvsp[(2) - (6)].node);
			(yyval.node)->v.cond.iftrue = (yyvsp[(4) - (6)].node);
			(yyval.node)->v.cond.iffalse = (yyvsp[(6) - (6)].node);
		}
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 251 "gram.y"
    {
			pop_ctx();
			(yyval.node) = radtest_node_alloc(radtest_node_case);
			(yyval.node)->locus = (yyvsp[(2) - (5)].node)->locus;
			(yyval.node)->v.branch.expr = (yyvsp[(2) - (5)].node);
			(yyval.node)->v.branch.branchlist = (yyvsp[(4) - (5)].list);
		}
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 258 "gram.y"
    { current_nesting_level++; }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 259 "gram.y"
    {
			pop_ctx();
			current_nesting_level--;
			(yyval.node) = radtest_node_alloc(radtest_node_loop);
			(yyval.node)->v.loop.cond = (yyvsp[(3) - (5)].node);
			(yyval.node)->v.loop.body = (yyvsp[(5) - (5)].node);
			(yyval.node)->v.loop.first_pass = 0;
		}
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 267 "gram.y"
    { current_nesting_level++; }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 267 "gram.y"
    { current_nesting_level--; }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 268 "gram.y"
    {
			pop_ctx();
			(yyval.node) = radtest_node_alloc(radtest_node_loop);
			(yyval.node)->v.loop.cond = (yyvsp[(8) - (8)].node);
			(yyval.node)->v.loop.body = (yyvsp[(4) - (8)].node);
			(yyval.node)->v.loop.first_pass = 1;
		}
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 276 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_print);
			(yyval.node)->v.list = (yyvsp[(2) - (2)].list);
		}
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 281 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_asgn);
			(yyval.node)->v.asgn.name = (yyvsp[(1) - (3)].string);
			(yyval.node)->v.asgn.expr = (yyvsp[(3) - (3)].node);
		}
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 287 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_send);
			(yyval.node)->v.send.cntl = (yyvsp[(2) - (5)].symtab);
			(yyval.node)->v.send.port_type = (yyvsp[(3) - (5)].i);
			(yyval.node)->v.send.code = (yyvsp[(4) - (5)].i);
			(yyval.node)->v.send.expr = (yyvsp[(5) - (5)].node);
		}
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 295 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_expect);
			(yyval.node)->v.expect.code = (yyvsp[(2) - (3)].i);
			(yyval.node)->v.expect.expr = (yyvsp[(3) - (3)].node);
		}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 301 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_exit);
			(yyval.node)->v.expr = (yyvsp[(2) - (2)].node);
		}
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 306 "gram.y"
    {
			if ((yyvsp[(2) - (2)].i) > current_nesting_level) {
				parse_error(_("not enough 'while's to break from"));
			}
			(yyval.node) = radtest_node_alloc(radtest_node_break);
			(yyval.node)->v.level = (yyvsp[(2) - (2)].i);
		}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 314 "gram.y"
    {
			if ((yyvsp[(2) - (2)].i) > current_nesting_level) {
				parse_error(_("not enough 'while's to continue"));
			}
			(yyval.node) = radtest_node_alloc(radtest_node_continue);
			(yyval.node)->v.level = (yyvsp[(2) - (2)].i);
		}
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 322 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_input);
			(yyval.node)->v.input.expr = NULL;
			(yyval.node)->v.input.var = (radtest_variable_t*)
				grad_sym_lookup_or_install(vartab,
							   "INPUT",
							   1);
		}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 331 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_input);
			(yyval.node)->v.input.expr = (yyvsp[(2) - (3)].node);
			(yyval.node)->v.input.var = (radtest_variable_t*)
				grad_sym_lookup_or_install(vartab,
							   (yyvsp[(3) - (3)].string),
							   1);
		}
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 340 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_set);
			(yyval.node)->v.set.argc = (yyvsp[(1) - (1)].set).argc;
			(yyval.node)->v.set.argv = (yyvsp[(1) - (1)].set).argv;
		}
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 346 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_shift);
			(yyval.node)->v.expr = (yyvsp[(2) - (2)].node);
		}
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 351 "gram.y"
    {
			(yyvsp[(1) - (3)].fun)->body = (yyvsp[(2) - (3)].list);
			radtest_fix_mem();
			(yyval.node) = NULL;
			defn.function = NULL;
		}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 358 "gram.y"
    {
			if (!defn.function) {
				parse_error(_("return outside of a function definition"));
			}
			(yyval.node) = radtest_node_alloc(radtest_node_return);
			(yyval.node)->v.expr = (yyvsp[(2) - (2)].node);
		}
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 366 "gram.y"
    {
			radtest_function_t *fun;
			
			fun = (radtest_function_t*)
				grad_sym_lookup(functab, (yyvsp[(1) - (4)].string));
			if (!fun) {
				parse_error(_("undefined function `%s'"), (yyvsp[(1) - (4)].string));
			}
			(yyval.node) = radtest_node_alloc(radtest_node_call);
			(yyval.node)->v.call.fun = fun;
			(yyval.node)->v.call.args = (yyvsp[(3) - (4)].list);
		}
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 381 "gram.y"
    {
			radtest_function_t *fun;
			
			if (defn.function) {
				parse_error(_("nested function definitions "
					      "are not allowed"));
				parse_error_loc(&defn.locus,
						_("the current function "
						  "definition begins here"));
				YYERROR; /* FIXME */
			}
			defn.function = (yyvsp[(1) - (4)].string);
			defn.locus = source_locus;
			fun = (radtest_function_t*)
				grad_sym_lookup_or_install(functab, (yyvsp[(1) - (4)].string), 1);
			if (fun->body) {
				parse_error(_("redefinition of function `%s'"), (yyvsp[(1) - (4)].string));
				parse_error_loc(&fun->locus,
					     _("`%s' previously defined here"),
						(yyvsp[(1) - (4)].string));

				YYERROR; /* FIXME */
			}
			fun->locus = source_locus;
			(yyval.fun) = fun;
		}
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 410 "gram.y"
    {
			push_ctx(ctx_if);
		}
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 416 "gram.y"
    {
			push_ctx(ctx_case);
		}
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 422 "gram.y"
    {
			push_ctx(ctx_do);
		}
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 428 "gram.y"
    {
			push_ctx(ctx_while);
		}
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 434 "gram.y"
    {
			pop_ctx();
		}
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 443 "gram.y"
    {
			(yyval.list) = grad_list_create();
			grad_list_append((yyval.list), (yyvsp[(1) - (1)].case_branch));
		}
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 448 "gram.y"
    {
			grad_list_append((yyvsp[(1) - (2)].list), (yyvsp[(2) - (2)].case_branch));
			(yyval.list) = (yyvsp[(1) - (2)].list);
		}
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 455 "gram.y"
    {
			radtest_case_branch_t *p = radtest_branch_alloc();
			p->cond = (yyvsp[(1) - (4)].node);
			p->node = (yyvsp[(3) - (4)].node);
			(yyval.case_branch) = p;
		}
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 468 "gram.y"
    {
			(yyval.string) = NULL;
		}
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 477 "gram.y"
    {
			parse_error(_("warning: truncating binary string"));
			(yyval.string) = (yyvsp[(1) - (1)].bstring).ptr;
		}
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 484 "gram.y"
    {
			(yyval.i) = 1;
		}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 488 "gram.y"
    {
			(yyval.i) = (yyvsp[(1) - (1)].number);
		}
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 494 "gram.y"
    {
			(yyval.i) = GRAD_PORT_AUTH;
		}
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 498 "gram.y"
    {
			(yyval.i) = GRAD_PORT_ACCT;
		}
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 504 "gram.y"
    {
			(yyval.i) = (yyvsp[(1) - (1)].number);
		}
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 508 "gram.y"
    {
			(yyval.i) = grad_request_name_to_code((yyvsp[(1) - (1)].string));
			if ((yyval.i) == 0) 
				parse_error(_("expected integer value or request code name"));
		}
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 516 "gram.y"
    {
			(yyval.symtab) = NULL;
		}
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 523 "gram.y"
    {
			radtest_variable_t *var;
			
			(yyval.symtab) = grad_symtab_create(sizeof(*var), var_free);
			var = (radtest_variable_t*) grad_sym_install((yyval.symtab),
								     (yyvsp[(1) - (1)].var)->name);
			radtest_var_copy (var, (yyvsp[(1) - (1)].var));
		}
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 532 "gram.y"
    {
			radtest_variable_t *var;
			var = (radtest_variable_t*) grad_sym_install((yyvsp[(1) - (2)].symtab),
								     (yyvsp[(2) - (2)].var)->name);
			radtest_var_copy (var, (yyvsp[(2) - (2)].var)); /* FIXME: check this */
		}
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 541 "gram.y"
    {
			(yyval.var) = radtest_var_alloc(rtv_integer);
			(yyval.var)->name = (yyvsp[(1) - (3)].string);
			(yyval.var)->datum.number = (yyvsp[(3) - (3)].number);
		}
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 549 "gram.y"
    {
			(yyval.node) = NULL;
		}
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 553 "gram.y"
    {
			radtest_variable_t *var = radtest_var_alloc(rtv_pairlist);
			var->datum.list = (yyvsp[(1) - (1)].list);
			(yyval.node) = radtest_node_alloc(radtest_node_value);
			(yyval.node)->v.var = var;
		}
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 564 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_unary);
			(yyval.node)->v.unary.op = radtest_op_not;
			(yyval.node)->v.unary.operand = (yyvsp[(2) - (2)].node);
		}
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 570 "gram.y"
    {
			(yyval.node) = (yyvsp[(2) - (3)].node);
		}
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 574 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_and;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 581 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_or;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 588 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_eq;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 595 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_lt;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 602 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_gt;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 609 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_ne;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 616 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_le;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 623 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_ge;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 636 "gram.y"
    {
			char *name = (yyvsp[(3) - (4)].string) ? (yyvsp[(3) - (4)].string) : "OPTVAR";
			(yyval.node) = radtest_node_alloc(radtest_node_getopt);
			(yyval.node)->v.gopt.last = 0;
			(yyval.node)->v.gopt.optstr = (yyvsp[(2) - (4)].string);
			(yyval.node)->v.gopt.var = (radtest_variable_t*)
				grad_sym_lookup_or_install(vartab, name, 1);

			name = (yyvsp[(4) - (4)].string) ? (yyvsp[(4) - (4)].string) : "OPTARG";
			(yyval.node)->v.gopt.arg = (radtest_variable_t*)
				grad_sym_lookup_or_install(vartab, name, 1);

			name = (yyvsp[(4) - (4)].string) ? (yyvsp[(4) - (4)].string) : "OPTIND";
			(yyval.node)->v.gopt.ind = (radtest_variable_t*)
				grad_sym_lookup_or_install(vartab, name, 1);
			
		}
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 654 "gram.y"
    {
			(yyval.node) = (yyvsp[(2) - (3)].node);
		}
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 658 "gram.y"
    {
			grad_dict_attr_t *dict = grad_attr_name_to_dict((yyvsp[(3) - (5)].string));
			if (!dict) {
				parse_error(_("unknown attribute `%s'"), (yyvsp[(3) - (5)].string));
				(yyval.node) = NULL;
			} else {
				(yyval.node) = radtest_node_alloc(radtest_node_attr);
				(yyval.node)->v.attr.node = (yyvsp[(1) - (5)].node);
				(yyval.node)->v.attr.dict = dict;
				(yyval.node)->v.attr.all = (yyvsp[(4) - (5)].i);
				if ((yyvsp[(4) - (5)].i) && dict->type != GRAD_TYPE_STRING) 
					parse_error(
		     _("warning: '*' is meaningless for this attribute type"));
			}
		}
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 674 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_add;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 681 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_sub;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 688 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_mul;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 695 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_div;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 702 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_bin);
			(yyval.node)->v.bin.op = radtest_op_mod;
			(yyval.node)->v.bin.left = (yyvsp[(1) - (3)].node);
			(yyval.node)->v.bin.right = (yyvsp[(3) - (3)].node);
		}
    break;

  case 89:
/* Line 1792 of yacc.c  */
#line 709 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_unary);
			(yyval.node)->v.unary.op = radtest_op_neg;
			(yyval.node)->v.unary.operand = (yyvsp[(2) - (2)].node);
		}
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 715 "gram.y"
    {
			(yyval.node) = (yyvsp[(2) - (2)].node);
		}
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 721 "gram.y"
    {
			(yyval.node) = NULL;
		}
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 728 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_value);
			(yyval.node)->v.var = (yyvsp[(1) - (1)].var);
		}
    break;

  case 94:
/* Line 1792 of yacc.c  */
#line 733 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_deref);
			(yyval.node)->v.deref = (yyvsp[(1) - (1)].deref);
		}
    break;

  case 95:
/* Line 1792 of yacc.c  */
#line 738 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_parm);
			(yyval.node)->v.parm = (yyvsp[(1) - (1)].parm);
		}
    break;

  case 96:
/* Line 1792 of yacc.c  */
#line 743 "gram.y"
    {
			(yyval.node) = radtest_node_alloc(radtest_node_argcount);
		}
    break;

  case 97:
/* Line 1792 of yacc.c  */
#line 747 "gram.y"
    {
			radtest_function_t *fun;
			
			fun = (radtest_function_t*)
				grad_sym_lookup(functab, (yyvsp[(1) - (4)].string));
			if (!fun) {
				parse_error(_("undefined function `%s'"), (yyvsp[(1) - (4)].string));
				(yyval.node) = NULL;
			} else {
				(yyval.node) = radtest_node_alloc(radtest_node_call);
				(yyval.node)->v.call.fun = fun;
				(yyval.node)->v.call.args = (yyvsp[(3) - (4)].list);
			}
		}
    break;

  case 98:
/* Line 1792 of yacc.c  */
#line 764 "gram.y"
    {
			(yyval.i) = 0;
		}
    break;

  case 99:
/* Line 1792 of yacc.c  */
#line 768 "gram.y"
    {
			(yyval.i) = 1;
		}
    break;

  case 100:
/* Line 1792 of yacc.c  */
#line 774 "gram.y"
    {
			(yyval.var) = radtest_var_alloc(rtv_integer);
			(yyval.var)->datum.number = (yyvsp[(1) - (1)].number);
		}
    break;

  case 101:
/* Line 1792 of yacc.c  */
#line 779 "gram.y"
    {
			(yyval.var) = radtest_var_alloc(rtv_ipaddress);
			(yyval.var)->datum.ipaddr = (yyvsp[(1) - (1)].ipaddr);
		}
    break;

  case 102:
/* Line 1792 of yacc.c  */
#line 784 "gram.y"
    {
			(yyval.var) = radtest_var_alloc(rtv_string);
			(yyval.var)->datum.string = (yyvsp[(1) - (1)].string);
		}
    break;

  case 103:
/* Line 1792 of yacc.c  */
#line 789 "gram.y"
    {
			(yyval.var) = radtest_var_alloc(rtv_bstring);
			(yyval.var)->datum.bstring = (yyvsp[(1) - (1)].bstring);
		}
    break;

  case 104:
/* Line 1792 of yacc.c  */
#line 794 "gram.y"
    {
			(yyval.var) = radtest_var_alloc(rtv_string);
			(yyval.var)->datum.string = (yyvsp[(1) - (1)].string);
		}
    break;

  case 105:
/* Line 1792 of yacc.c  */
#line 799 "gram.y"
    {
			(yyval.var) = radtest_var_alloc(rtv_pairlist);
			(yyval.var)->datum.list = (yyvsp[(2) - (3)].list);
		}
    break;

  case 106:
/* Line 1792 of yacc.c  */
#line 806 "gram.y"
    {
			(yyval.list) = NULL;
		}
    break;

  case 108:
/* Line 1792 of yacc.c  */
#line 813 "gram.y"
    {
			(yyval.list) = NULL;
		}
    break;

  case 110:
/* Line 1792 of yacc.c  */
#line 820 "gram.y"
    {
			(yyval.list) = grad_list_create();
			grad_list_append((yyval.list), (yyvsp[(1) - (1)].pair));
		}
    break;

  case 111:
/* Line 1792 of yacc.c  */
#line 825 "gram.y"
    {
			grad_list_append((yyvsp[(1) - (2)].list), (yyvsp[(2) - (2)].pair));
			(yyval.list) = (yyvsp[(1) - (2)].list);
		}
    break;

  case 112:
/* Line 1792 of yacc.c  */
#line 830 "gram.y"
    {
			grad_list_append((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].pair));
			(yyval.list) = (yyvsp[(1) - (3)].list);
		}
    break;

  case 114:
/* Line 1792 of yacc.c  */
#line 838 "gram.y"
    {
			grad_dict_attr_t *attr = grad_attr_name_to_dict((yyvsp[(1) - (3)].string));
			if (!attr) 
				parse_error(_("unknown attribute `%s'"), (yyvsp[(1) - (3)].string));

			(yyval.pair) = radtest_pair_alloc();
			(yyval.pair)->attr = attr;
			(yyval.pair)->op = (yyvsp[(2) - (3)].op);
			(yyval.pair)->node = (yyvsp[(3) - (3)].node);
		}
    break;

  case 115:
/* Line 1792 of yacc.c  */
#line 851 "gram.y"
    {
                        (yyval.op) = grad_operator_equal;
                }
    break;

  case 116:
/* Line 1792 of yacc.c  */
#line 855 "gram.y"
    {
                        (yyval.op) = grad_operator_less_than;
                }
    break;

  case 117:
/* Line 1792 of yacc.c  */
#line 859 "gram.y"
    { 
                        (yyval.op) = grad_operator_greater_than;
                }
    break;

  case 118:
/* Line 1792 of yacc.c  */
#line 863 "gram.y"
    {
                        (yyval.op) = grad_operator_not_equal;
                }
    break;

  case 119:
/* Line 1792 of yacc.c  */
#line 867 "gram.y"
    {
                        (yyval.op) = grad_operator_less_equal;
                }
    break;

  case 120:
/* Line 1792 of yacc.c  */
#line 871 "gram.y"
    {
                        (yyval.op) = grad_operator_greater_equal;
                }
    break;

  case 121:
/* Line 1792 of yacc.c  */
#line 877 "gram.y"
    {
			(yyval.list) = grad_list_create();
			grad_list_append((yyval.list), (yyvsp[(1) - (1)].node));
		}
    break;

  case 122:
/* Line 1792 of yacc.c  */
#line 882 "gram.y"
    {
			grad_list_append((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].node));
			(yyval.list) = (yyvsp[(1) - (3)].list);
		}
    break;

  case 123:
/* Line 1792 of yacc.c  */
#line 887 "gram.y"
    {
			grad_list_append((yyvsp[(1) - (2)].list), (yyvsp[(2) - (2)].node));
			(yyval.list) = (yyvsp[(1) - (2)].list);
		}
    break;


/* Line 1792 of yacc.c  */
#line 2843 "gram.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
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
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
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


/* Line 2055 of yacc.c  */
#line 896 "gram.y"


int
yyerror(char *s)
{
	if (strcmp(s, "parse error") == 0
	    || strcmp(s, "syntax error") == 0) {
		if (yychar == T_END)
			parse_error(_("Misplaced `end'"));
		else if (yychar == EOL) {
			grad_locus_t loc = source_locus;
			loc.line--;
			parse_error_loc(&loc, _("Unexpected end of line"));
		} else if (peek_ctx() == ctx_doerr)
			;
		else 
			parse_error(s);
	} else if (yychar == EOL) {
		grad_locus_t loc = source_locus;
		loc.line--;
		parse_error_loc(&loc, s);
	} else
		parse_error(s);
}

static char *funcname_displayed = NULL;

static int
namecmp(char *a, char *b)
{
	if (!a || !b)
		return a != b;
	return strcmp(a, b);
}

static void
print_function_name()
{
	if (namecmp(funcname_displayed, defn.function)) {
		if (defn.function)
			fprintf(stderr, _("In function `%s':\n"),
				defn.function);
		else
			fprintf(stderr, _("At top level:\n"));
		funcname_displayed = defn.function;
	}
}	

void
parse_error(const char *fmt, ...)
{
        va_list ap;

	print_function_name();
	va_start(ap, fmt);
        fprintf(stderr, "%s:%lu: ",
		source_locus.file,
		(unsigned long) source_locus.line);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, "\n");
	error_count++;
}

void
parse_error_loc(grad_locus_t *locus, const char *fmt, ...)
{
        va_list ap;

	print_function_name();
	va_start(ap, fmt);
        fprintf(stderr, "%s:%lu: ",
		locus->file, (unsigned long) locus->line);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, "\n");
	error_count++;
}

void
set_yydebug()
{
        extern int yydebug;
        if (GRAD_DEBUG_LEVEL(1)) {
                yydebug = 1;
        }
}

static void
run_statement(radtest_node_t *node)
{
	if (!dry_run) {
		if (!error_count)
			radtest_eval(node, toplevel_env);
		/* Clear error_count only if we are in interactive
		   mode. There is no use continuing executing of a script
		   after an erroneous statement. */
		if (interactive)
			error_count = 0;
	}
	radtest_free_mem();
}

int
read_and_eval(char *filename)
{
        if (open_input(filename))
                return 1;
        return (yyparse() || error_count) ? 1 : 0;
}

static void
errsync()
{
	enum context ctx = pop_ctx();
	switch (ctx) {
	case ctx_none:
		break;
		
	case ctx_if:
		push_ctx(ctx_iferr);
		break;
		
	case ctx_do:
		current_nesting_level--;
		push_ctx(ctx_doerr);
		break;

	case ctx_while:
		current_nesting_level--;
		break;
		
	case ctx_case:
		;
	}
}
