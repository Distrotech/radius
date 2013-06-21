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

/* All symbols defined below should begin with uyy or YY, to avoid
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
#line 1 "users.y"

/* This file is part of GNU Radius.
   Copyright (C) 2000,2001,2002,2003,2004,2007,
   2008 Free Software Foundation, Inc.

   Written by Sergey Poznyakoff
  
   GNU Radius is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
  
   GNU Radius is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public
   License along with GNU Radius; if not, write to the Free
   Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA. */

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

#include <radlib.h>
#include "intprops.h"
	
#define YYMAXDEPTH 10

static grad_locus_t start_loc;

static void *closure;
static register_rule_fp add_entry;
static grad_avp_t *grad_create_pair0(char *name, int op, char *valstr);

int uyyerror(char *s);
extern int uyylex();
 

/* Line 371 of yacc.c  */
#line 126 "users_gram.c"

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
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int uyydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum uyytokentype {
     EQ = 258,
     LT = 259,
     GT = 260,
     NE = 261,
     LE = 262,
     GE = 263,
     NUL = 264,
     BOGUS = 265,
     STRING = 266,
     QUOTE = 267
   };
#endif
/* Tokens.  */
#define EQ 258
#define LT 259
#define GT 260
#define NE 261
#define LE 262
#define GE 263
#define NUL 264
#define BOGUS 265
#define STRING 266
#define QUOTE 267



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 70 "users.y"

        char *string;
        grad_matching_rule_t *rule;
        struct {
                grad_avp_t *lhs, *rhs;
        } descr;
        grad_avp_t *pair;
        enum grad_operator op;


/* Line 387 of yacc.c  */
#line 204 "users_gram.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define uyystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE uyylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int uyyparse (void *YYPARSE_PARAM);
#else
int uyyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int uyyparse (void);
#else
int uyyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 232 "users_gram.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 uyytype_uint8;
#else
typedef unsigned char uyytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 uyytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char uyytype_int8;
#else
typedef short int uyytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 uyytype_uint16;
#else
typedef unsigned short int uyytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 uyytype_int16;
#else
typedef short int uyytype_int16;
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
YYID (int uyyi)
#else
static int
YYID (uyyi)
    int uyyi;
#endif
{
  return uyyi;
}
#endif

#if ! defined uyyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined uyyoverflow || YYERROR_VERBOSE */


#if (! defined uyyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union uyyalloc
{
  uyytype_int16 uyyss_alloc;
  YYSTYPE uyyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union uyyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (uyytype_int16) + sizeof (YYSTYPE)) \
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
	YYSIZE_T uyynewbytes;						\
	YYCOPY (&uyyptr->Stack_alloc, Stack, uyysize);			\
	Stack = &uyyptr->Stack_alloc;					\
	uyynewbytes = uyystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	uyyptr += uyynewbytes / sizeof (*uyyptr);				\
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
          YYSIZE_T uyyi;                         \
          for (uyyi = 0; uyyi < (Count); uyyi++)   \
            (Dst)[uyyi] = (Src)[uyyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  8
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   26

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  14
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  11
/* YYNRULES -- Number of rules.  */
#define YYNRULES  24
/* YYNRULES -- Number of states.  */
#define YYNSTATES  30

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   267

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? uyytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const uyytype_uint8 uyytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    13,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const uyytype_uint8 uyyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    14,    17,    20,
      22,    25,    27,    29,    31,    35,    39,    43,    45,    47,
      49,    51,    53,    55,    57
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const uyytype_int8 uyyrhs[] =
{
      15,     0,    -1,    -1,    16,    -1,    17,    -1,    16,    17,
      -1,    16,     1,    -1,    18,    19,    -1,    18,     1,    -1,
      24,    -1,    20,    20,    -1,     9,    -1,    21,    -1,    22,
      -1,    21,    13,    22,    -1,    11,    23,    24,    -1,    11,
      23,    10,    -1,     3,    -1,     4,    -1,     5,    -1,     6,
      -1,     7,    -1,     8,    -1,    11,    -1,    12,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const uyytype_uint8 uyyrline[] =
{
       0,    82,    82,    83,    86,    89,    90,    96,   100,   112,
     118,   125,   129,   132,   133,   144,   148,   154,   158,   162,
     166,   170,   174,   180,   181
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const uyytname[] =
{
  "$end", "error", "$undefined", "EQ", "LT", "GT", "NE", "LE", "GE",
  "NUL", "BOGUS", "STRING", "QUOTE", "','", "$accept", "input", "list",
  "entry", "user", "descr", "npairlist", "pairlist", "pair", "op", "value", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const uyytype_uint16 uyytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const uyytype_uint8 uyyr1[] =
{
       0,    14,    15,    15,    16,    16,    16,    17,    17,    18,
      19,    20,    20,    21,    21,    22,    22,    23,    23,    23,
      23,    23,    23,    24,    24
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const uyytype_uint8 uyyr2[] =
{
       0,     2,     0,     1,     1,     2,     2,     2,     2,     1,
       2,     1,     1,     1,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const uyytype_uint8 uyydefact[] =
{
       2,    23,    24,     0,     0,     4,     0,     9,     1,     6,
       5,     8,    11,     0,     7,     0,    12,    13,    17,    18,
      19,    20,    21,    22,     0,    10,     0,    16,    15,    14
};

/* YYDEFGOTO[NTERM-NUM].  */
static const uyytype_int8 uyydefgoto[] =
{
      -1,     3,     4,     5,     6,    14,    15,    16,    17,    24,
       7
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -9
static const uyytype_int8 uyypact[] =
{
      -2,    -9,    -9,     7,     0,    -9,     4,    -9,    -9,    -9,
      -9,    -9,    -9,    13,    -9,    -3,     1,    -9,    -9,    -9,
      -9,    -9,    -9,    -9,    -8,    -9,    11,    -9,    -9,    -9
};

/* YYPGOTO[NTERM-NUM].  */
static const uyytype_int8 uyypgoto[] =
{
      -9,    -9,    -9,    19,    -9,    -9,     9,    -9,    -1,    -9,
       2
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -4
static const uyytype_int8 uyytable[] =
{
      -3,     9,    27,     1,     2,    11,    12,     8,    13,     1,
       2,     1,     2,    12,    26,    13,    18,    19,    20,    21,
      22,    23,    13,    10,    25,    29,    28
};

#define uyypact_value_is_default(Yystate) \
  (!!((Yystate) == (-9)))

#define uyytable_value_is_error(Yytable_value) \
  YYID (0)

static const uyytype_uint8 uyycheck[] =
{
       0,     1,    10,    11,    12,     1,     9,     0,    11,    11,
      12,    11,    12,     9,    13,    11,     3,     4,     5,     6,
       7,     8,    11,     4,    15,    26,    24
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const uyytype_uint8 uyystos[] =
{
       0,    11,    12,    15,    16,    17,    18,    24,     0,     1,
      17,     1,     9,    11,    19,    20,    21,    22,     3,     4,
       5,     6,     7,     8,    23,    20,    13,    10,    24,    22
};

#define uyyerrok		(uyyerrstatus = 0)
#define uyyclearin	(uyychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto uyyacceptlab
#define YYABORT		goto uyyabortlab
#define YYERROR		goto uyyerrorlab


/* Like YYERROR except do call uyyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto uyyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!uyyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (uyychar == YYEMPTY)                                        \
    {                                                           \
      uyychar = (Token);                                         \
      uyylval = (Value);                                         \
      YYPOPSTACK (uyylen);                                       \
      uyystate = *uyyssp;                                         \
      goto uyybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      uyyerror (YY_("syntax error: cannot back up")); \
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


/* YYLEX -- calling `uyylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX uyylex (YYLEX_PARAM)
#else
# define YYLEX uyylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (uyydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (uyydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      uyy_symbol_print (stderr,						  \
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
uyy_symbol_value_print (FILE *uyyoutput, int uyytype, YYSTYPE const * const uyyvaluep)
#else
static void
uyy_symbol_value_print (uyyoutput, uyytype, uyyvaluep)
    FILE *uyyoutput;
    int uyytype;
    YYSTYPE const * const uyyvaluep;
#endif
{
  FILE *uyyo = uyyoutput;
  YYUSE (uyyo);
  if (!uyyvaluep)
    return;
# ifdef YYPRINT
  if (uyytype < YYNTOKENS)
    YYPRINT (uyyoutput, uyytoknum[uyytype], *uyyvaluep);
# else
  YYUSE (uyyoutput);
# endif
  switch (uyytype)
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
uyy_symbol_print (FILE *uyyoutput, int uyytype, YYSTYPE const * const uyyvaluep)
#else
static void
uyy_symbol_print (uyyoutput, uyytype, uyyvaluep)
    FILE *uyyoutput;
    int uyytype;
    YYSTYPE const * const uyyvaluep;
#endif
{
  if (uyytype < YYNTOKENS)
    YYFPRINTF (uyyoutput, "token %s (", uyytname[uyytype]);
  else
    YYFPRINTF (uyyoutput, "nterm %s (", uyytname[uyytype]);

  uyy_symbol_value_print (uyyoutput, uyytype, uyyvaluep);
  YYFPRINTF (uyyoutput, ")");
}

/*------------------------------------------------------------------.
| uyy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
uyy_stack_print (uyytype_int16 *uyybottom, uyytype_int16 *uyytop)
#else
static void
uyy_stack_print (uyybottom, uyytop)
    uyytype_int16 *uyybottom;
    uyytype_int16 *uyytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; uyybottom <= uyytop; uyybottom++)
    {
      int uyybot = *uyybottom;
      YYFPRINTF (stderr, " %d", uyybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (uyydebug)							\
    uyy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
uyy_reduce_print (YYSTYPE *uyyvsp, int uyyrule)
#else
static void
uyy_reduce_print (uyyvsp, uyyrule)
    YYSTYPE *uyyvsp;
    int uyyrule;
#endif
{
  int uyynrhs = uyyr2[uyyrule];
  int uyyi;
  unsigned long int uyylno = uyyrline[uyyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     uyyrule - 1, uyylno);
  /* The symbols being reduced.  */
  for (uyyi = 0; uyyi < uyynrhs; uyyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", uyyi + 1);
      uyy_symbol_print (stderr, uyyrhs[uyyprhs[uyyrule] + uyyi],
		       &(uyyvsp[(uyyi + 1) - (uyynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (uyydebug)				\
    uyy_reduce_print (uyyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int uyydebug;
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

# ifndef uyystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define uyystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
uyystrlen (const char *uyystr)
#else
static YYSIZE_T
uyystrlen (uyystr)
    const char *uyystr;
#endif
{
  YYSIZE_T uyylen;
  for (uyylen = 0; uyystr[uyylen]; uyylen++)
    continue;
  return uyylen;
}
#  endif
# endif

# ifndef uyystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define uyystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
uyystpcpy (char *uyydest, const char *uyysrc)
#else
static char *
uyystpcpy (uyydest, uyysrc)
    char *uyydest;
    const char *uyysrc;
#endif
{
  char *uyyd = uyydest;
  const char *uyys = uyysrc;

  while ((*uyyd++ = *uyys++) != '\0')
    continue;

  return uyyd - 1;
}
#  endif
# endif

# ifndef uyytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for uyyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from uyytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
uyytnamerr (char *uyyres, const char *uyystr)
{
  if (*uyystr == '"')
    {
      YYSIZE_T uyyn = 0;
      char const *uyyp = uyystr;

      for (;;)
	switch (*++uyyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++uyyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (uyyres)
	      uyyres[uyyn] = *uyyp;
	    uyyn++;
	    break;

	  case '"':
	    if (uyyres)
	      uyyres[uyyn] = '\0';
	    return uyyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! uyyres)
    return uyystrlen (uyystr);

  return uyystpcpy (uyyres, uyystr) - uyyres;
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
uyysyntax_error (YYSIZE_T *uyymsg_alloc, char **uyymsg,
                uyytype_int16 *uyyssp, int uyytoken)
{
  YYSIZE_T uyysize0 = uyytnamerr (YY_NULL, uyytname[uyytoken]);
  YYSIZE_T uyysize = uyysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *uyyformat = YY_NULL;
  /* Arguments of uyyformat. */
  char const *uyyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int uyycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in uyychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated uyychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (uyytoken != YYEMPTY)
    {
      int uyyn = uyypact[*uyyssp];
      uyyarg[uyycount++] = uyytname[uyytoken];
      if (!uyypact_value_is_default (uyyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int uyyxbegin = uyyn < 0 ? -uyyn : 0;
          /* Stay within bounds of both uyycheck and uyytname.  */
          int uyychecklim = YYLAST - uyyn + 1;
          int uyyxend = uyychecklim < YYNTOKENS ? uyychecklim : YYNTOKENS;
          int uyyx;

          for (uyyx = uyyxbegin; uyyx < uyyxend; ++uyyx)
            if (uyycheck[uyyx + uyyn] == uyyx && uyyx != YYTERROR
                && !uyytable_value_is_error (uyytable[uyyx + uyyn]))
              {
                if (uyycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    uyycount = 1;
                    uyysize = uyysize0;
                    break;
                  }
                uyyarg[uyycount++] = uyytname[uyyx];
                {
                  YYSIZE_T uyysize1 = uyysize + uyytnamerr (YY_NULL, uyytname[uyyx]);
                  if (! (uyysize <= uyysize1
                         && uyysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  uyysize = uyysize1;
                }
              }
        }
    }

  switch (uyycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        uyyformat = S;                       \
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
    YYSIZE_T uyysize1 = uyysize + uyystrlen (uyyformat);
    if (! (uyysize <= uyysize1 && uyysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    uyysize = uyysize1;
  }

  if (*uyymsg_alloc < uyysize)
    {
      *uyymsg_alloc = 2 * uyysize;
      if (! (uyysize <= *uyymsg_alloc
             && *uyymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *uyymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *uyyp = *uyymsg;
    int uyyi = 0;
    while ((*uyyp = *uyyformat) != '\0')
      if (*uyyp == '%' && uyyformat[1] == 's' && uyyi < uyycount)
        {
          uyyp += uyytnamerr (uyyp, uyyarg[uyyi++]);
          uyyformat += 2;
        }
      else
        {
          uyyp++;
          uyyformat++;
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
uyydestruct (const char *uyymsg, int uyytype, YYSTYPE *uyyvaluep)
#else
static void
uyydestruct (uyymsg, uyytype, uyyvaluep)
    const char *uyymsg;
    int uyytype;
    YYSTYPE *uyyvaluep;
#endif
{
  YYUSE (uyyvaluep);

  if (!uyymsg)
    uyymsg = "Deleting";
  YY_SYMBOL_PRINT (uyymsg, uyytype, uyyvaluep, uyylocationp);

  switch (uyytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int uyychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE uyylval YY_INITIAL_VALUE(uyyval_default);

/* Number of syntax errors so far.  */
int uyynerrs;


/*----------.
| uyyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
uyyparse (void *YYPARSE_PARAM)
#else
int
uyyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
uyyparse (void)
#else
int
uyyparse ()

#endif
#endif
{
    int uyystate;
    /* Number of tokens to shift before error messages enabled.  */
    int uyyerrstatus;

    /* The stacks and their tools:
       `uyyss': related to states.
       `uyyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow uyyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    uyytype_int16 uyyssa[YYINITDEPTH];
    uyytype_int16 *uyyss;
    uyytype_int16 *uyyssp;

    /* The semantic value stack.  */
    YYSTYPE uyyvsa[YYINITDEPTH];
    YYSTYPE *uyyvs;
    YYSTYPE *uyyvsp;

    YYSIZE_T uyystacksize;

  int uyyn;
  int uyyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int uyytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE uyyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char uyymsgbuf[128];
  char *uyymsg = uyymsgbuf;
  YYSIZE_T uyymsg_alloc = sizeof uyymsgbuf;
#endif

#define YYPOPSTACK(N)   (uyyvsp -= (N), uyyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int uyylen = 0;

  uyyssp = uyyss = uyyssa;
  uyyvsp = uyyvs = uyyvsa;
  uyystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  uyystate = 0;
  uyyerrstatus = 0;
  uyynerrs = 0;
  uyychar = YYEMPTY; /* Cause a token to be read.  */
  goto uyysetstate;

/*------------------------------------------------------------.
| uyynewstate -- Push a new state, which is found in uyystate.  |
`------------------------------------------------------------*/
 uyynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  uyyssp++;

 uyysetstate:
  *uyyssp = uyystate;

  if (uyyss + uyystacksize - 1 <= uyyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T uyysize = uyyssp - uyyss + 1;

#ifdef uyyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *uyyvs1 = uyyvs;
	uyytype_int16 *uyyss1 = uyyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if uyyoverflow is a macro.  */
	uyyoverflow (YY_("memory exhausted"),
		    &uyyss1, uyysize * sizeof (*uyyssp),
		    &uyyvs1, uyysize * sizeof (*uyyvsp),
		    &uyystacksize);

	uyyss = uyyss1;
	uyyvs = uyyvs1;
      }
#else /* no uyyoverflow */
# ifndef YYSTACK_RELOCATE
      goto uyyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= uyystacksize)
	goto uyyexhaustedlab;
      uyystacksize *= 2;
      if (YYMAXDEPTH < uyystacksize)
	uyystacksize = YYMAXDEPTH;

      {
	uyytype_int16 *uyyss1 = uyyss;
	union uyyalloc *uyyptr =
	  (union uyyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (uyystacksize));
	if (! uyyptr)
	  goto uyyexhaustedlab;
	YYSTACK_RELOCATE (uyyss_alloc, uyyss);
	YYSTACK_RELOCATE (uyyvs_alloc, uyyvs);
#  undef YYSTACK_RELOCATE
	if (uyyss1 != uyyssa)
	  YYSTACK_FREE (uyyss1);
      }
# endif
#endif /* no uyyoverflow */

      uyyssp = uyyss + uyysize - 1;
      uyyvsp = uyyvs + uyysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) uyystacksize));

      if (uyyss + uyystacksize - 1 <= uyyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", uyystate));

  if (uyystate == YYFINAL)
    YYACCEPT;

  goto uyybackup;

/*-----------.
| uyybackup.  |
`-----------*/
uyybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  uyyn = uyypact[uyystate];
  if (uyypact_value_is_default (uyyn))
    goto uyydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (uyychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      uyychar = YYLEX;
    }

  if (uyychar <= YYEOF)
    {
      uyychar = uyytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      uyytoken = YYTRANSLATE (uyychar);
      YY_SYMBOL_PRINT ("Next token is", uyytoken, &uyylval, &uyylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  uyyn += uyytoken;
  if (uyyn < 0 || YYLAST < uyyn || uyycheck[uyyn] != uyytoken)
    goto uyydefault;
  uyyn = uyytable[uyyn];
  if (uyyn <= 0)
    {
      if (uyytable_value_is_error (uyyn))
        goto uyyerrlab;
      uyyn = -uyyn;
      goto uyyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (uyyerrstatus)
    uyyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", uyytoken, &uyylval, &uyylloc);

  /* Discard the shifted token.  */
  uyychar = YYEMPTY;

  uyystate = uyyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++uyyvsp = uyylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto uyynewstate;


/*-----------------------------------------------------------.
| uyydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
uyydefault:
  uyyn = uyydefact[uyystate];
  if (uyyn == 0)
    goto uyyerrlab;
  goto uyyreduce;


/*-----------------------------.
| uyyreduce -- Do a reduction.  |
`-----------------------------*/
uyyreduce:
  /* uyyn is the number of a rule to reduce with.  */
  uyylen = uyyr2[uyyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  uyyval = uyyvsp[1-uyylen];


  YY_REDUCE_PRINT (uyyn);
  switch (uyyn)
    {
        case 4:
/* Line 1792 of yacc.c  */
#line 87 "users.y"
    {
           }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 91 "users.y"
    {
                   grad_parser_lex_sync(); uyyerrok; uyyclearin;
           }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 97 "users.y"
    {
                   add_entry(closure, &start_loc, (uyyvsp[(1) - (2)].string), (uyyvsp[(2) - (2)].descr).lhs, (uyyvsp[(2) - (2)].descr).rhs);
           }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 101 "users.y"
    {
                   grad_log(GRAD_LOG_ERR, _("discarding user `%s'"), (uyyvsp[(1) - (2)].string));
                   if (grad_parser_lex_sync() <= 0)
			   uyychar = 0; /* force end-of-file */
		   else {
			   uyyerrok;
			   uyyclearin;
		   }
           }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 113 "users.y"
    {
		   start_loc = grad_parser_source_locus;
           }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 119 "users.y"
    {
                   (uyyval.descr).lhs = (uyyvsp[(1) - (2)].pair);
                   (uyyval.descr).rhs = (uyyvsp[(2) - (2)].pair);
           }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 126 "users.y"
    {
                   (uyyval.pair) = NULL;
           }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 134 "users.y"
    {
                   if ((uyyvsp[(1) - (3)].pair)) {
                           if ((uyyvsp[(3) - (3)].pair)) 
                                   grad_avl_add_list(&(uyyvsp[(1) - (3)].pair), (uyyvsp[(3) - (3)].pair));
                           (uyyval.pair) = (uyyvsp[(1) - (3)].pair);
                   } else
                           (uyyval.pair) = (uyyvsp[(3) - (3)].pair);
           }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 145 "users.y"
    {
                   (uyyval.pair) = grad_create_pair0((uyyvsp[(1) - (3)].string), (uyyvsp[(2) - (3)].op), (uyyvsp[(3) - (3)].string));   
           }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 149 "users.y"
    {
                   YYERROR;
           }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 155 "users.y"
    {
                   (uyyval.op) = grad_operator_equal;
           }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 159 "users.y"
    {
                   (uyyval.op) = grad_operator_less_than;
           }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 163 "users.y"
    { 
                   (uyyval.op) = grad_operator_greater_than;
           }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 167 "users.y"
    {
                   (uyyval.op) = grad_operator_not_equal;
           }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 171 "users.y"
    {
                   (uyyval.op) = grad_operator_less_equal;
           }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 175 "users.y"
    {
                   (uyyval.op) = grad_operator_greater_equal;
           }
    break;


/* Line 1792 of yacc.c  */
#line 1574 "users_gram.c"
      default: break;
    }
  /* User semantic actions sometimes alter uyychar, and that requires
     that uyytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of uyytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering uyychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", uyyr1[uyyn], &uyyval, &uyyloc);

  YYPOPSTACK (uyylen);
  uyylen = 0;
  YY_STACK_PRINT (uyyss, uyyssp);

  *++uyyvsp = uyyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  uyyn = uyyr1[uyyn];

  uyystate = uyypgoto[uyyn - YYNTOKENS] + *uyyssp;
  if (0 <= uyystate && uyystate <= YYLAST && uyycheck[uyystate] == *uyyssp)
    uyystate = uyytable[uyystate];
  else
    uyystate = uyydefgoto[uyyn - YYNTOKENS];

  goto uyynewstate;


/*------------------------------------.
| uyyerrlab -- here on detecting error |
`------------------------------------*/
uyyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  uyytoken = uyychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (uyychar);

  /* If not already recovering from an error, report this error.  */
  if (!uyyerrstatus)
    {
      ++uyynerrs;
#if ! YYERROR_VERBOSE
      uyyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR uyysyntax_error (&uyymsg_alloc, &uyymsg, \
                                        uyyssp, uyytoken)
      {
        char const *uyymsgp = YY_("syntax error");
        int uyysyntax_error_status;
        uyysyntax_error_status = YYSYNTAX_ERROR;
        if (uyysyntax_error_status == 0)
          uyymsgp = uyymsg;
        else if (uyysyntax_error_status == 1)
          {
            if (uyymsg != uyymsgbuf)
              YYSTACK_FREE (uyymsg);
            uyymsg = (char *) YYSTACK_ALLOC (uyymsg_alloc);
            if (!uyymsg)
              {
                uyymsg = uyymsgbuf;
                uyymsg_alloc = sizeof uyymsgbuf;
                uyysyntax_error_status = 2;
              }
            else
              {
                uyysyntax_error_status = YYSYNTAX_ERROR;
                uyymsgp = uyymsg;
              }
          }
        uyyerror (uyymsgp);
        if (uyysyntax_error_status == 2)
          goto uyyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (uyyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (uyychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (uyychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  uyydestruct ("Error: discarding",
		      uyytoken, &uyylval);
	  uyychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto uyyerrlab1;


/*---------------------------------------------------.
| uyyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
uyyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label uyyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto uyyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (uyylen);
  uyylen = 0;
  YY_STACK_PRINT (uyyss, uyyssp);
  uyystate = *uyyssp;
  goto uyyerrlab1;


/*-------------------------------------------------------------.
| uyyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
uyyerrlab1:
  uyyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      uyyn = uyypact[uyystate];
      if (!uyypact_value_is_default (uyyn))
	{
	  uyyn += YYTERROR;
	  if (0 <= uyyn && uyyn <= YYLAST && uyycheck[uyyn] == YYTERROR)
	    {
	      uyyn = uyytable[uyyn];
	      if (0 < uyyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (uyyssp == uyyss)
	YYABORT;


      uyydestruct ("Error: popping",
		  uyystos[uyystate], uyyvsp);
      YYPOPSTACK (1);
      uyystate = *uyyssp;
      YY_STACK_PRINT (uyyss, uyyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++uyyvsp = uyylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", uyystos[uyyn], uyyvsp, uyylsp);

  uyystate = uyyn;
  goto uyynewstate;


/*-------------------------------------.
| uyyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
uyyacceptlab:
  uyyresult = 0;
  goto uyyreturn;

/*-----------------------------------.
| uyyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
uyyabortlab:
  uyyresult = 1;
  goto uyyreturn;

#if !defined uyyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| uyyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
uyyexhaustedlab:
  uyyerror (YY_("memory exhausted"));
  uyyresult = 2;
  /* Fall through.  */
#endif

uyyreturn:
  if (uyychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      uyytoken = YYTRANSLATE (uyychar);
      uyydestruct ("Cleanup: discarding lookahead",
                  uyytoken, &uyylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (uyylen);
  YY_STACK_PRINT (uyyss, uyyssp);
  while (uyyssp != uyyss)
    {
      uyydestruct ("Cleanup: popping",
		  uyystos[*uyyssp], uyyvsp);
      YYPOPSTACK (1);
    }
#ifndef uyyoverflow
  if (uyyss != uyyssa)
    YYSTACK_FREE (uyyss);
#endif
#if YYERROR_VERBOSE
  if (uyymsg != uyymsgbuf)
    YYSTACK_FREE (uyymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (uyyresult);
}


/* Line 2055 of yacc.c  */
#line 184 "users.y"


int
uyyerror(char *s)
{
	grad_log_loc(GRAD_LOG_ERR, &grad_parser_source_locus, "%s", s);
	return 0;
}

grad_avp_t *
grad_create_pair0(char *name, int op, char *valstr)
{
	return grad_create_pair(&grad_parser_source_locus, name, op, valstr);
}

grad_avp_t *
grad_create_pair(grad_locus_t *loc, char *name, 
                 enum grad_operator op, char *valstr)
{
        grad_dict_attr_t *attr = NULL;
        grad_dict_value_t *dval;
        grad_avp_t *pair, *pair2;
        char *s;
        int x;
        time_t timeval;
        struct tm *tm, tms;
        
        if ((attr = grad_attr_name_to_dict(name)) == NULL) {
                grad_log_loc(GRAD_LOG_ERR, loc, _("unknown attribute `%s'"),
			     name);
                return NULL;
        }

        pair = grad_avp_alloc();
        
        pair->next = NULL;
        pair->name = attr->name;
        pair->attribute = attr->value;
        pair->type = attr->type;
        pair->prop = attr->prop;
        pair->operator = op;

        if (valstr[0] == '=') {
                pair->eval_type = grad_eval_interpret;
                pair->avp_strvalue = grad_estrdup(valstr+1);
                pair->avp_strlength = strlen(pair->avp_strvalue);
                return pair;
        }

        pair->eval_type = grad_eval_const;
        
        switch (pair->type) {
        case GRAD_TYPE_STRING:
                if (pair->attribute == DA_EXEC_PROGRAM ||
                    pair->attribute == DA_EXEC_PROGRAM_WAIT) {
                        if (valstr[0] != '/' && valstr[0] != '|') {
                                grad_log_loc(GRAD_LOG_ERR, loc,
					     _("%s: not an absolute pathname"),
					     name);
                                grad_avp_free(pair);
                                return NULL;
                        }
                }

		pair->avp_strvalue = grad_estrdup(valstr);
                pair->avp_strlength = strlen(pair->avp_strvalue);
		
		if (attr->parser && attr->parser(pair, &s)) {
			grad_log_loc(GRAD_LOG_ERR, loc,
				     "%s %s: %s",
				     _("attribute"),
				     pair->name, s);
			free(s);
			grad_avp_free(pair);
			return NULL;
		}

                break;

        case GRAD_TYPE_INTEGER:
                /*
                 *      For DA_NAS_PORT_ID, allow a
                 *      port range instead of just a port.
                 */
                if (attr->value == DA_NAS_PORT_ID) {
                        for (s = valstr; *s; s++)
                                if (!isdigit(*s))
                                        break;
                        if (*s) {
                                pair->type = GRAD_TYPE_STRING;
                                pair->avp_strvalue = grad_estrdup(valstr);
                                pair->avp_strlength = strlen(pair->avp_strvalue);
                                break;
                        }
                }
                if (isdigit(*valstr)) {
                        pair->avp_lvalue = atoi(valstr);
                } else if ((dval = grad_value_name_to_value(valstr,
							    pair->attribute))
			         == NULL) {
                        grad_avp_free(pair);
                        grad_log_loc(GRAD_LOG_ERR, loc,
				_("value %s is not declared for attribute %s"),
				     valstr, name);
                        return NULL;
                } else {
                        pair->avp_lvalue = dval->value;
                }
                break;

        case GRAD_TYPE_IPADDR:
                if (pair->attribute != DA_FRAMED_IP_ADDRESS) {
                        pair->avp_lvalue = grad_ip_gethostaddr(valstr);
                } else {
                        /*
                         *      We allow a "+" at the end to
                         *      indicate that we should add the
                         *      portno. to the IP address.
                         */
                        x = 0;
                        if (valstr[0]) {
                                for(s = valstr; s[1]; s++)
                                        ;
                                if (*s == '+') {
                                        *s = 0;
                                        x = 1;
                                }
                        }
                        pair->avp_lvalue = grad_ip_gethostaddr(valstr);

			if (x) {
				char *s;
				char buf[INT_BUFSIZE_BOUND(long)];
				grad_longtostr(pair->avp_lvalue,
					       buf, sizeof buf);
				grad_astrcat(&s, buf, "+", "%{NAS-Port-Id}",
					     NULL);
				pair->avp_strvalue = grad_estrdup(s);
				pair->avp_strlength = strlen(pair->avp_strvalue);
				pair->eval_type = grad_eval_interpret;
				free(s);
			}
                }
                break;
                
        case GRAD_TYPE_DATE:
                timeval = time(0);
                tm = localtime_r(&timeval, &tms);
                if (grad_parse_time_string(valstr, tm)) {
                        grad_log_loc(GRAD_LOG_ERR, loc, _("%s: can't parse date"),
				     name);
                        grad_avp_free(pair);
                        return NULL;
                }
#ifdef TIMELOCAL
                pair->avp_lvalue = (grad_uint32_t)timelocal(tm);
#else /* TIMELOCAL */
                pair->avp_lvalue = (grad_uint32_t)mktime(tm);
#endif /* TIMELOCAL */
                break;

        default:
                grad_log_loc(GRAD_LOG_ERR, loc,
			     _("%s: unknown attribute type %d"),
			     name, pair->type);
                grad_avp_free(pair);
                return NULL;
        }

        return pair;
}

extern int uyydebug;

int
grad_parse_rule_file(char *file, void *c, register_rule_fp f)
{
        int rc;
        
        if (grad_parser_lex_init(file))
                return -1;
        closure = c;
        add_entry = f;

        uyydebug = 0;
        rc = uyyparse();
        grad_parser_lex_finish();
        return rc;
}

void
grad_enable_rule_debug(int val)
{
        uyydebug = val;
	grad_log_loc(GRAD_LOG_NOTICE, &grad_parser_source_locus,
		     uyydebug ? _("enabled userfile parser debugging") :
             	 	       _("disabled userfile parser debugging"));
}
