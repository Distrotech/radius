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

/* All symbols defined below should begin with rw_yy or YY, to avoid
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
#line 1 "rewrite.y"

/* This file is part of GNU Radius.
   Copyright (C) 2000,2001,2002,2003,2004,2005,
   2006,2007,2008 Free Software Foundation, Inc.

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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <radiusd.h>
#include <setjmp.h>
#include <rewrite.h>
#ifdef USE_SERVER_GUILE 
# include <libguile.h>
# include <radius/radscm.h>	
#endif
        
typedef long RWSTYPE;
#define RW_MIN(a,b) ((a)<(b)) ? (a) : (b)
 
/*
 * Generalized list structure
 */
typedef struct rw_list RWLIST;
#define RWLIST(type) \
        type     *next;\
        type     *prev

struct rw_list {
        RWLIST(RWLIST);
};

/*
 * Generalized object 
 */
typedef struct object_t OBJECT ;

#define OBJ(type) \
        RWLIST(type);\
        type    *alloc

struct object_t {
        OBJ(OBJECT);
};

typedef struct {
        size_t   size;        /* Size of an element */
        void     (*free)();   /* deallocator */ 
        OBJECT   *alloc_list; /* list of allocated elements */
} OBUCKET;

        

/* ************************************************************
 * Basic data types
 */

typedef int stkoff_t;             /* Offset on stack */
typedef unsigned int pctr_t;      /* Program counter */

#define RW_REG ('z'-'a'+1)

typedef struct {
        RWSTYPE    reg[RW_REG];       /* Registers */
        #define rA reg[0]
        char       *sA;               /* String accumulator */
        pctr_t     pc;                /* Program counter */  
        
        RWSTYPE    *stack;            /* Stack+heap space */
        int        stacksize;         /* Size of stack */
        int        st;                /* Top of stack */
        int        sb;                /* Stack base */
        int        ht;                /* Top of heap */
        
        int        nmatch;
        regmatch_t *pmatch;

        grad_request_t *req;
        
        jmp_buf    jmp;
} RWMACH;

typedef void (*INSTR)();       /* program instruction */
 
/* Compiled regular expression
 */
typedef struct comp_regex COMP_REGEX;
struct comp_regex {
        OBJ(COMP_REGEX);
        regex_t      regex;    /* compiled regex itself */
        int          nmatch;   /* number of \( ... \) groups */
};

/*
 * Binary Operations
 */
typedef enum {
        Eq,
        Ne,
        Lt,
        Le,
        Gt,
        Ge,
        BAnd,
        BXor,
        BOr,
        And,
        Or,
        Shl,
        Shr,
        Add,
        Sub,
        Mul,
        Div,
        Rem,
        Max_opcode
} Bopcode;

/*
 * Unary operations
 */
typedef enum {
        Neg,
        Not,
        Max_unary
} Uopcode;

/*
 * Matrix types
 */
typedef enum {
        Generic,
        Nop,
        Enter,
        Leave,
        Stop,
        Constant,
        Matchref,
        Variable,
        Unary,
        Binary,
        Cond,
        Asgn,
        Match,
        Coercion,
        Expression,
        Return,
        Jump,
        Branch,
        Target,
        Call,
        Builtin,
        Pop,
        Pusha,
        Popa,
        Attr,
        Attr_asgn,
        Attr_check,
	Attr_delete,
        Max_mtxtype
} Mtxtype;

/*
 * Function parameter
 */
typedef struct parm_t PARAMETER;
struct parm_t {
        PARAMETER   *prev;     /* Previous parameter */
        PARAMETER   *next;     /* Next parameter */
        grad_data_type_t    datatype;  /* type */
        stkoff_t    offset;    /* Offset on stack */
};

/*
 * Local variable
 */
typedef struct variable VAR;
struct variable {
        OBJ(VAR);
        VAR       *dcllink;  /* Link to the next variable vithin the
                              * same declaration
                              */
        char      *name;     /* name of the variable */
        int       level;     /* nesting level */
        int       offset;    /* offset on stack */
        grad_data_type_t  datatype;  /* type */
        int       constant;  /* true if assigned a constant value */
        grad_datum_t     datum;     /* constant value itself */
};

/*
 * Function definition
 */
typedef struct function_def {
        struct function_def *next;
        char       *name;        /* Function name */
        grad_data_type_t   rettype;      /* Return type */
        pctr_t     entry;        /* Code entry */
        COMP_REGEX *rx_list;     /* List of compiled regexps */
        int        nparm;        /* Number of parameters */
        PARAMETER  *parm;        /* List of parameters */
        stkoff_t   stack_alloc;  /* required stack allocation */
        grad_locus_t      loc;   /* source location where the function
                                  * was declared
                                  */
} FUNCTION;

#define STACK_BASE 2

/*
 * Built-in function
 */
typedef struct  {
        INSTR    handler;        /* Function itself */
        char     *name;          /* Function name */
        grad_data_type_t rettype;        /* Return type */
        char     *parms;         /* coded parameter types */
} builtin_t;

/*
 * Operation matrices
 */
typedef union mtx MTX;
/*
 * All matrices contain the following common fields:
 *    alloc- link to the previously allocated matrix.
 *           It is used at the end of code generation
 *           pass to free all allocated matrices.
 *    next - link to the next matrix in the subexpression
 *    prev - link to the previous matrix in the subexpression
 * Additionally, all expression matrices contain the field
 * `datatype' which contains the data type for this matrix.
 */
#if defined(MAINTAINER_MODE)
# define COMMON_MTX \
        OBJ(MTX);\
        int      id;\
        grad_locus_t    loc;\
        Mtxtype  type;
#else
# define COMMON_MTX \
        OBJ(MTX);\
        grad_locus_t    loc;\
        Mtxtype  type;
#endif
        
#define COMMON_EXPR_MTX \
        COMMON_MTX\
        grad_data_type_t datatype;\
        MTX      *uplink;\
        MTX      *arglink;

/*
 * Generic matrix: nothing special
 * Type: Generic
 */
typedef struct {
        COMMON_EXPR_MTX
} GEN_MTX;
/*
 * Constant matrix
 * Type: Constant
 */
typedef struct {
        COMMON_EXPR_MTX
        grad_datum_t    datum;     /* Constant value */      
} CONST_MTX;
/*
 * Reference to a previous regexp: corresponds to a \N construct
 * Type: Matchref
 */
typedef struct {
        COMMON_EXPR_MTX
        int      num;       /* Number of \( ... \) to be referenced */
} MATCHREF_MTX;
/*
 * Reference to a variable
 * Type: Variable
 */
typedef struct {
        COMMON_EXPR_MTX
        VAR      *var;      /* Variable being referenced */ 
} VAR_MTX;
/*
 * Unary operation matrix
 * Type: Unary
 */
typedef struct {
        COMMON_EXPR_MTX
        Uopcode  opcode;    /* Operation code */
        MTX      *arg;      /* Argument */
} UN_MTX;
/*
 * Binary operation matrix
 * Type: Binary
 */
typedef struct {
        COMMON_EXPR_MTX
        Bopcode   opcode;   /* Operation code */ 
        MTX      *arg[2];   /* Arguments */ 
} BIN_MTX;
/*
 * Assignment matrix
 * Type: Asgn
 */
typedef struct {
        COMMON_EXPR_MTX
        VAR      *lval;     /* Lvalue */
        MTX      *arg;      /* Rvalue */
} ASGN_MTX;
/*
 * Conditional expression matrix
 * Type: Cond
 */
typedef struct {
        COMMON_MTX
        MTX      *expr;     /* Conditional expression */
        MTX      *if_true;  /* Branch if true */
        MTX      *if_false; /* Branch if false */ 
} COND_MTX;
/*
 * Regexp match
 * Type: Match
 */
typedef struct {
        COMMON_EXPR_MTX
        int        negated; /* Is the match negated ? */
        MTX        *arg;    /* Argument (lhs) */
        COMP_REGEX *rx;     /* Regexp (rhs) */
} MATCH_MTX;
/*
 * Type coercion
 * Type: Coerce
 */
typedef struct {
        COMMON_EXPR_MTX
        MTX      *arg;      /* Argument of the coercion */ 
} COERCE_MTX;
/*
 * Expression
 * Type: Expression
 */
typedef struct {
        COMMON_EXPR_MTX
        MTX      *expr;
} EXPR_MTX;
/*
 * Return from function
 * Type: Return
 */
typedef struct {
        COMMON_EXPR_MTX
        MTX      *expr;     /* Return value */
} RET_MTX;
/*
 * Unconditional branch (jump)
 * Type: Jump
 */
typedef struct {
        COMMON_MTX
        MTX *link;          /* Link to the next jump matrix
                             * (for break and continue matrices)
                             */
        MTX      *dest;     /* Jump destination (usually a NOP matrix) */
} JUMP_MTX;
/*
 * Conditional branch
 * Type: Branch
 */
typedef struct {
        COMMON_MTX
        int      cond;      /* Condition: 1 - equal, 0 - not equal */
        MTX      *dest;     /* Jump destination (usually a NOP matrix) */
} BRANCH_MTX;
/*
 * Stack frame matrix
 * Type: Enter, Leave
 */
typedef struct {
        COMMON_MTX
        stkoff_t  stacksize;/* Required stack size */
} FRAME_MTX;
/*
 * Jump target
 * Type: Target
 */
typedef struct {
        COMMON_MTX
        pctr_t  pc;         /* Target's program counter */
} TGT_MTX;
/*
 * No-op matrix. It is always inserted at the branch destination
 * points. Its purpose is to keep a singly-linked list of jump
 * locations for fixing up jump statements.
 * Type: Nop
 */
typedef struct {
        COMMON_MTX
        TGT_MTX   *tgt;     /* Target list */
        pctr_t     pc;      /* Program counter for backward
                               references */
} NOP_MTX;
/*
 * Function call
 * Type: Call
 */
typedef struct {
        COMMON_EXPR_MTX
        FUNCTION  *fun;     /* Called function */
        int       nargs;    /* Number of arguments */
        MTX       *args;    /* Arguments */
} CALL_MTX;
/*
 * Builtin function call
 * Type: Builtin
 */
typedef struct {
        COMMON_EXPR_MTX
        INSTR     fun;      /* Handler function */
        int       nargs;    /* Number of arguments */   
        MTX       *args;    /* Arguments */
} BTIN_MTX;
/*
 * Attribute matrix
 * Type: Attr, Attr_asgn, Attr_check
 */
typedef struct {
        COMMON_EXPR_MTX
        int       attrno;   /* Attribute number */
	MTX       *index;   /* Index expression */
        MTX       *rval;    /* Rvalue */
} ATTR_MTX;

union mtx {
        GEN_MTX    gen;
        NOP_MTX    nop;
        FRAME_MTX  frame;
        CONST_MTX  cnst;
        MATCHREF_MTX    ref;
        VAR_MTX    var;
        UN_MTX     un;
        BIN_MTX    bin;
        COND_MTX   cond;
        ASGN_MTX   asgn;
        MATCH_MTX  match;
        COERCE_MTX coerce;
        RET_MTX    ret;
        JUMP_MTX   jump;
        BRANCH_MTX branch;
        TGT_MTX    tgt;
        CALL_MTX   call;
        BTIN_MTX   btin;
        ATTR_MTX   attr;
};

/*
 * Stack frame
 */
typedef struct frame_t FRAME;

struct frame_t {
        OBJ(FRAME);
        int       level;        /* nesting level */
        stkoff_t  stack_offset; /* offset in the stack */
};


/* *****************************************************************
 * Static data
 */
/*
 * Stack Frame list
 */
static OBUCKET frame_bkt = { sizeof(FRAME), NULL };
static FRAME *frame_first, *frame_last;
#define curframe frame_last

static int errcnt;         /* Number of errors detected */ 
static FUNCTION *function; /* Function being compiled */
static grad_symtab_t *rewrite_tab;/* Function table */  

static MTX *mtx_first, *mtx_last;  /* Matrix list */
static VAR *var_first, *var_last;  /* Variable list */ 

/*
 * Loops
 */
typedef struct loop_t LOOP;
struct loop_t {
        OBJ(LOOP);
        JUMP_MTX *lp_break;
        JUMP_MTX *lp_cont;
};
static OBUCKET loop_bkt = { sizeof(LOOP), NULL };
static LOOP *loop_first, *loop_last;

void loop_push(MTX *mtx);
void loop_pop();
void loop_fixup(JUMP_MTX *list, MTX *target);
void loop_init();
void loop_free_all();
void loop_unwind_all();

/*
 * Lexical analyzer stuff
 */
static FILE *infile;               /* Input file */ 
static grad_locus_t locus;         /* Input location */

static char *inbuf;                /* Input string */
static char *curp;                 /* Current pointer */
 
static int   rw_yyeof;                /* rised when EOF is encountered */ 
static struct obstack input_stk;   /* Symbol stack */ 

static grad_data_type_t return_type = Undefined;
                                   /* Data type of the topmost expression. */

static int regcomp_flags = 0;      /* Flags to be used with regcomps */

#define regex_init() regcomp_flags = 0
 
/* Runtime */
static size_t rewrite_stack_size = 4096;  /* Size of stack+heap */
static RWSTYPE *runtime_stack;
static RWMACH mach;

/* Default domain for gettext functions. It is initialized to PACKAGE
   by default */
static char *default_gettext_domain;
 

/* ***************************************************************
 * Function declarations
 */

/*
 * Lexical analyzer
 */
static int rw_yylex(); 
static void rw_yysync();
static int rw_yyerror(char *s);
 
/*
 * Frames
 */
static void frame_init();
static void frame_push();
static void frame_pop();
static void frame_unwind_all();
static void frame_free_all();
/*
 * Variables
 */
static void var_init();
static VAR * var_alloc(grad_data_type_t type, char *name, int grow);
static void var_unwind_level();
static void var_unwind_all();
static void var_type(grad_data_type_t type, VAR *var);
static void var_free_all();
static VAR *var_lookup(char *name);
/*
 * Matrices
 */
static void mtx_init();
static void mtx_free_all();
static void mtx_unwind_all();
static MTX * mtx_cur();
static MTX * mtx_nop();
static MTX * mtx_jump();
static MTX * mtx_frame(Mtxtype type, stkoff_t stksize);
static MTX * mtx_stop();
static MTX * mtx_pop();
static MTX * mtx_return();
static MTX * mtx_alloc(Mtxtype type);
static MTX * mtx_const(grad_value_t *val);
static MTX * mtx_ref(int num);
static MTX * mtx_var(VAR *var);
static MTX * mtx_asgn(VAR *var, MTX *arg);
static MTX * mtx_bin(Bopcode opcode, MTX *arg1, MTX *arg2);
static MTX * mtx_un(Uopcode opcode, MTX *arg);
static MTX * mtx_match(int negated, MTX *mtx, COMP_REGEX *);
static MTX * mtx_cond(MTX *cond, MTX *if_true, MTX *if_false);
static MTX * mtx_coerce(grad_data_type_t type, MTX *arg);
static MTX * mtx_call(FUNCTION *fun, MTX *args);
static MTX * mtx_builtin(builtin_t *bin, MTX *args);
static MTX * mtx_attr(grad_dict_attr_t *attr, MTX *index);
static MTX * mtx_attr_asgn(grad_dict_attr_t *attr, MTX *index, MTX *rval);
static MTX * mtx_attr_check(grad_dict_attr_t *attr, MTX *index);
static MTX * mtx_attr_delete(grad_dict_attr_t *attr, MTX *index);

static MTX * coerce(MTX  *arg, grad_data_type_t type);
/*
 * Regular expressions
 */
static COMP_REGEX * rx_alloc(regex_t  *regex, int nmatch);
static void rx_free(COMP_REGEX *rx);
static COMP_REGEX * compile_regexp(char *str);
/*
 * Functions
 */
static FUNCTION * function_install(FUNCTION *fun);
static int  function_free(FUNCTION *fun);
static void function_delete();
static void function_cleanup();
/*
 * Built-in functions
 */
static builtin_t * builtin_lookup(char *name);

/*
 * Code optimizer and generator
 */
static int optimize();
static pctr_t codegen();
static void code_init();
static void code_check();

/*
 * Auxiliary and debugging functions
 */
static void debug_dump_code();
static const char * datatype_str_nom(grad_data_type_t type);
static const char * datatype_str_acc(grad_data_type_t type);
static const char * datatype_str_abl(grad_data_type_t type);
static grad_data_type_t attr_datatype(grad_dict_attr_t *);

/*
 * Run-Time
 */
static void gc();
static void run(pctr_t pc);
static int run_init(pctr_t pc, grad_request_t *req);
static int rw_error(const char *msg);
static int rw_error_free(char *msg);
 
/* These used to lock/unlock access to rw_code array. Now this is
   not needed. However, I left the placeholders for a while... */
#define rw_code_lock() 
#define rw_code_unlock()

#define AVPLIST(m) ((m)->req ? (m)->req->avlist : NULL)

static FUNCTION fmain;

/* Line 371 of yacc.c  */
#line 735 "rewrite.c"

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


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int rw_yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum rw_yytokentype {
     TYPE = 258,
     IF = 259,
     ELSE = 260,
     RETURN = 261,
     WHILE = 262,
     FOR = 263,
     DO = 264,
     BREAK = 265,
     CONTINUE = 266,
     DELETE = 267,
     STRING = 268,
     IDENT = 269,
     NUMBER = 270,
     REFERENCE = 271,
     VARIABLE = 272,
     FUN = 273,
     BUILTIN = 274,
     ATTR = 275,
     BOGUS = 276,
     OR = 277,
     AND = 278,
     NM = 279,
     MT = 280,
     NE = 281,
     EQ = 282,
     GE = 283,
     GT = 284,
     LE = 285,
     LT = 286,
     SHR = 287,
     SHL = 288,
     TYPECAST = 289,
     NOT = 290,
     UMINUS = 291
   };
#endif
/* Tokens.  */
#define TYPE 258
#define IF 259
#define ELSE 260
#define RETURN 261
#define WHILE 262
#define FOR 263
#define DO 264
#define BREAK 265
#define CONTINUE 266
#define DELETE 267
#define STRING 268
#define IDENT 269
#define NUMBER 270
#define REFERENCE 271
#define VARIABLE 272
#define FUN 273
#define BUILTIN 274
#define ATTR 275
#define BOGUS 276
#define OR 277
#define AND 278
#define NM 279
#define MT 280
#define NE 281
#define EQ 282
#define GE 283
#define GT 284
#define LE 285
#define LT 286
#define SHR 287
#define SHL 288
#define TYPECAST 289
#define NOT 290
#define UMINUS 291



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 669 "rewrite.y"

        int   number;
        int   type;
        VAR   *var;
        MTX   *mtx;
        FUNCTION  *fun;
        builtin_t *btin;
        grad_dict_attr_t *attr;
        struct {
                MTX *arg_first;
                MTX *arg_last;
        } arg;
        struct {
                int nmatch;
                regex_t regex;
        } rx;
        char  *string;


/* Line 387 of yacc.c  */
#line 867 "rewrite.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define rw_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE rw_yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int rw_yyparse (void *YYPARSE_PARAM);
#else
int rw_yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int rw_yyparse (void);
#else
int rw_yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 895 "rewrite.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 rw_yytype_uint8;
#else
typedef unsigned char rw_yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 rw_yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char rw_yytype_int8;
#else
typedef short int rw_yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 rw_yytype_uint16;
#else
typedef unsigned short int rw_yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 rw_yytype_int16;
#else
typedef short int rw_yytype_int16;
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
YYID (int rw_yyi)
#else
static int
YYID (rw_yyi)
    int rw_yyi;
#endif
{
  return rw_yyi;
}
#endif

#if ! defined rw_yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined rw_yyoverflow || YYERROR_VERBOSE */


#if (! defined rw_yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union rw_yyalloc
{
  rw_yytype_int16 rw_yyss_alloc;
  YYSTYPE rw_yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union rw_yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (rw_yytype_int16) + sizeof (YYSTYPE)) \
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
	YYSIZE_T rw_yynewbytes;						\
	YYCOPY (&rw_yyptr->Stack_alloc, Stack, rw_yysize);			\
	Stack = &rw_yyptr->Stack_alloc;					\
	rw_yynewbytes = rw_yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	rw_yyptr += rw_yynewbytes / sizeof (*rw_yyptr);				\
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
          YYSIZE_T rw_yyi;                         \
          for (rw_yyi = 0; rw_yyi < (Count); rw_yyi++)   \
            (Dst)[rw_yyi] = (Src)[rw_yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  34
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   535

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  87
/* YYNRULES -- Number of states.  */
#define YYNSTATES  166

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   291

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? rw_yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const rw_yytype_uint8 rw_yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    42,    29,     2,
      50,    51,    40,    38,    49,    39,     2,    41,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    48,
       2,    22,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    28,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,    27,    47,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    23,    24,    25,
      26,    30,    31,    32,    33,    34,    35,    36,    37,    43,
      44,    45
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const rw_yytype_uint16 rw_yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    14,    17,    22,
      26,    30,    32,    35,    37,    39,    41,    43,    46,    50,
      52,    56,    59,    63,    65,    69,    72,    73,    75,    77,
      81,    83,    85,    88,    92,    95,    99,   105,   109,   113,
     114,   121,   124,   127,   131,   138,   140,   142,   144,   148,
     150,   152,   154,   156,   158,   162,   164,   169,   172,   178,
     182,   189,   194,   199,   203,   207,   211,   215,   219,   223,
     227,   231,   235,   239,   243,   247,   250,   253,   256,   260,
     265,   269,   273,   277,   281,   285,   289,   293
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const rw_yytype_int8 rw_yyrhs[] =
{
      53,     0,    -1,    54,    -1,    55,    -1,    78,    -1,    56,
      -1,    55,    56,    -1,    55,     1,    -1,    57,    58,    71,
      59,    -1,     3,    14,    65,    -1,     3,    18,    65,    -1,
      60,    -1,    60,    62,    -1,    61,    -1,    46,    -1,    47,
      -1,    63,    -1,    62,    63,    -1,     3,    64,    48,    -1,
      14,    -1,    64,    49,    14,    -1,    50,    51,    -1,    50,
      66,    51,    -1,    67,    -1,    66,    49,    67,    -1,     3,
      14,    -1,    -1,    69,    -1,    70,    -1,    69,    49,    70,
      -1,    78,    -1,    72,    -1,    71,    72,    -1,    58,    71,
      59,    -1,    78,    48,    -1,     4,    77,    72,    -1,     4,
      77,    72,    76,    72,    -1,     6,    78,    48,    -1,    74,
      77,    72,    -1,    -1,    75,    72,    73,     7,    77,    48,
      -1,    10,    48,    -1,    11,    48,    -1,    12,    20,    48,
      -1,    12,    20,    50,    78,    51,    48,    -1,     7,    -1,
       9,    -1,     5,    -1,    50,    78,    51,    -1,    15,    -1,
      13,    -1,    16,    -1,    17,    -1,    14,    -1,    17,    22,
      78,    -1,    20,    -1,    20,    50,    78,    51,    -1,    40,
      20,    -1,    40,    20,    50,    78,    51,    -1,    20,    22,
      78,    -1,    20,    50,    78,    51,    22,    78,    -1,    18,
      50,    68,    51,    -1,    19,    50,    68,    51,    -1,    78,
      38,    78,    -1,    78,    39,    78,    -1,    78,    40,    78,
      -1,    78,    41,    78,    -1,    78,    42,    78,    -1,    78,
      27,    78,    -1,    78,    29,    78,    -1,    78,    28,    78,
      -1,    78,    37,    78,    -1,    78,    36,    78,    -1,    78,
      24,    78,    -1,    78,    23,    78,    -1,    39,    78,    -1,
      38,    78,    -1,    44,    78,    -1,    50,    78,    51,    -1,
      50,     3,    51,    78,    -1,    78,    31,    78,    -1,    78,
      30,    78,    -1,    78,    35,    78,    -1,    78,    34,    78,
      -1,    78,    33,    78,    -1,    78,    32,    78,    -1,    78,
      26,    13,    -1,    78,    25,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const rw_yytype_uint16 rw_yyrline[] =
{
       0,   719,   719,   728,   732,   754,   755,   756,   773,   799,
     838,   849,   850,   853,   856,   862,   873,   874,   877,   883,
     888,   899,   903,   909,   914,   921,   931,   934,   940,   945,
     953,   959,   960,   963,   967,   972,   977,   984,   989,  1005,
    1005,  1024,  1038,  1051,  1055,  1061,  1068,  1075,  1083,  1094,
    1101,  1109,  1113,  1117,  1123,  1127,  1131,  1135,  1139,  1143,
    1147,  1151,  1155,  1159,  1163,  1167,  1171,  1175,  1179,  1183,
    1187,  1191,  1195,  1199,  1203,  1207,  1211,  1215,  1219,  1223,
    1227,  1231,  1235,  1239,  1243,  1247,  1251,  1260
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const rw_yytname[] =
{
  "$end", "error", "$undefined", "TYPE", "IF", "ELSE", "RETURN", "WHILE",
  "FOR", "DO", "BREAK", "CONTINUE", "DELETE", "STRING", "IDENT", "NUMBER",
  "REFERENCE", "VARIABLE", "FUN", "BUILTIN", "ATTR", "BOGUS", "'='", "OR",
  "AND", "NM", "MT", "'|'", "'^'", "'&'", "NE", "EQ", "GE", "GT", "LE",
  "LT", "SHR", "SHL", "'+'", "'-'", "'*'", "'/'", "'%'", "TYPECAST", "NOT",
  "UMINUS", "'{'", "'}'", "';'", "','", "'('", "')'", "$accept", "program",
  "input", "dcllist", "decl", "fundecl", "begin", "end", "obrace",
  "cbrace", "autodcl", "autovar", "varlist", "dclparm", "parmlist", "parm",
  "args", "arglist", "arg", "list", "stmt", "@1", "while", "do", "else",
  "cond", "expr", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const rw_yytype_uint16 rw_yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    61,   277,   278,   279,   280,   124,    94,    38,
     281,   282,   283,   284,   285,   286,   287,   288,    43,    45,
      42,    47,    37,   289,   290,   291,   123,   125,    59,    44,
      40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const rw_yytype_uint8 rw_yyr1[] =
{
       0,    52,    53,    54,    54,    55,    55,    55,    56,    57,
      57,    58,    58,    59,    60,    61,    62,    62,    63,    64,
      64,    65,    65,    66,    66,    67,    68,    68,    69,    69,
      70,    71,    71,    72,    72,    72,    72,    72,    72,    73,
      72,    72,    72,    72,    72,    74,    75,    76,    77,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const rw_yytype_uint8 rw_yyr2[] =
{
       0,     2,     1,     1,     1,     1,     2,     2,     4,     3,
       3,     1,     2,     1,     1,     1,     1,     2,     3,     1,
       3,     2,     3,     1,     3,     2,     0,     1,     1,     3,
       1,     1,     2,     3,     2,     3,     5,     3,     3,     0,
       6,     2,     2,     3,     6,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     1,     4,     2,     5,     3,
       6,     4,     4,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     3,     4,
       3,     3,     3,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const rw_yytype_uint8 rw_yydefact[] =
{
       0,     0,    50,    53,    49,    51,    52,     0,     0,    55,
       0,     0,     0,     0,     0,     0,     2,     0,     5,     0,
       4,     0,     0,     0,    26,    26,     0,     0,    76,    75,
      57,    77,     0,     0,     1,     7,     6,    14,     0,    11,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     9,    10,    54,     0,    27,    28,    30,     0,    59,
       0,     0,     0,    78,     0,     0,    45,    46,     0,     0,
       0,     0,     0,    31,     0,     0,     0,     0,    12,    16,
      74,    73,    87,    86,    68,    70,    69,    81,    80,    85,
      84,    83,    82,    72,    71,    63,    64,    65,    66,    67,
       0,    21,     0,    23,    61,     0,    62,    56,     0,    79,
       0,     0,     0,    41,    42,     0,     0,    15,     8,    13,
      32,     0,    39,    34,    19,     0,    17,    25,     0,    22,
      29,     0,    58,     0,    35,    37,    43,     0,    33,    38,
       0,    18,     0,    24,    60,    48,    47,     0,     0,     0,
      20,    36,     0,     0,    44,    40
};

/* YYDEFGOTO[NTERM-NUM].  */
static const rw_yytype_int16 rw_yydefgoto[] =
{
      -1,    15,    16,    17,    18,    19,    81,   128,    39,   129,
      88,    89,   135,    61,   112,   113,    64,    65,    66,    82,
      83,   150,    84,    85,   157,   121,    86
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -84
static const rw_yytype_int16 rw_yypact[] =
{
      67,    17,   -84,   -84,   -84,   -84,    -1,   -22,    12,   -20,
     207,   207,    44,   207,   110,    65,   -84,    33,   -84,    20,
     432,    19,    19,   207,   207,   207,   207,   207,   -84,   -84,
      24,   -84,    16,   235,   -84,   -84,   -84,   -84,   169,    74,
     207,   207,    66,    90,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
       9,   -84,   -84,   432,    57,    61,   -84,   432,    58,   432,
     264,   207,   207,   -84,    62,   207,   -84,   -84,    68,    70,
      94,   169,   152,   -84,    62,   169,   380,   105,    74,   -84,
     451,   469,   -84,   -84,    60,   199,   482,   493,   493,   -33,
     -33,   -33,   -33,   -23,   -23,    -3,    -3,   -84,   -84,   -84,
     107,   -84,   -29,   -84,   -84,   207,   -84,   100,   293,   -84,
     207,   169,   406,   -84,   -84,    13,   152,   -84,   -84,   -84,
     -84,   169,   -84,   -84,   -84,    -6,   -84,   -84,   128,   -84,
     -84,   207,   -84,   322,   127,   -84,   -84,   207,   -84,   -84,
     126,   -84,   120,   -84,   432,   -84,   -84,   169,   351,    62,
     -84,   -84,    87,    88,   -84,   -84
};

/* YYPGOTO[NTERM-NUM].  */
static const rw_yytype_int16 rw_yypgoto[] =
{
     -84,   -84,   -84,   -84,   121,   -84,   118,    14,   -84,   -84,
     -84,    51,   -84,   122,   -84,     4,   130,   -84,    28,    64,
     -53,   -84,   -84,   -84,   -84,   -83,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -4
static const rw_yytype_int16 rw_yytable[] =
{
      20,   131,    26,    53,    54,    55,    56,    57,    58,    59,
      28,    29,   110,    31,    33,    55,    56,    57,    58,    59,
     138,    23,   139,    63,    67,    67,    69,    70,    24,   130,
      27,    21,   132,    -3,    35,    22,     1,    57,    58,    59,
      90,    91,   151,   152,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     111,   146,    25,   147,    30,    34,    37,    72,   144,    60,
       1,   118,   119,   130,    71,   122,   163,    87,   149,    92,
       2,     3,     4,     5,     6,     7,     8,     9,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    93,   161,    10,    11,    12,   114,   116,
     115,    13,   120,    32,   125,    67,   123,    14,   124,   134,
     143,   137,   141,     2,     3,     4,     5,     6,     7,     8,
       9,   110,   156,   159,   160,   164,   165,    38,    36,   136,
     148,   154,   153,   140,    62,   126,     0,   158,    10,    11,
      12,     0,     0,     0,    13,    68,    74,     0,    75,    76,
      14,    77,    78,    79,    80,     2,     3,     4,     5,     6,
       7,     8,     9,    74,     0,    75,    76,     0,    77,    78,
      79,    80,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,    13,     0,    37,   127,
       0,     0,    14,     0,     0,     0,     0,    10,    11,    12,
       0,     0,     0,    13,     0,    37,     0,     0,     0,    14,
       2,     3,     4,     5,     6,     7,     8,     9,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,     0,     0,     0,    10,    11,    12,     0,     0,
       0,    13,     0,     0,     0,     0,     0,    14,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,     0,     0,
       0,     0,     0,     0,     0,     0,    73,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,     0,     0,     0,
       0,     0,     0,     0,     0,   117,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,     0,     0,     0,     0,
       0,     0,     0,     0,   142,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,     0,     0,     0,     0,     0,
       0,     0,     0,   155,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,     0,     0,     0,     0,     0,     0,
       0,     0,   162,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,     0,     0,     0,     0,     0,   133,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,     0,
       0,     0,     0,     0,   145,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59
};

#define rw_yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-84)))

#define rw_yytable_value_is_error(Yytable_value) \
  YYID (0)

static const rw_yytype_int16 rw_yycheck[] =
{
       0,    84,    22,    36,    37,    38,    39,    40,    41,    42,
      10,    11,     3,    13,    14,    38,    39,    40,    41,    42,
      49,    22,    51,    23,    24,    25,    26,    27,    50,    82,
      50,    14,    85,     0,     1,    18,     3,    40,    41,    42,
      40,    41,    48,    49,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      51,    48,    50,    50,    20,     0,    46,    51,   121,    50,
       3,    71,    72,   126,    50,    75,   159,     3,   131,    13,
      13,    14,    15,    16,    17,    18,    19,    20,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    13,   157,    38,    39,    40,    51,    51,
      49,    44,    50,     3,    20,   115,    48,    50,    48,    14,
     120,    14,    22,    13,    14,    15,    16,    17,    18,    19,
      20,     3,     5,     7,    14,    48,    48,    19,    17,    88,
     126,   141,   138,   115,    22,    81,    -1,   147,    38,    39,
      40,    -1,    -1,    -1,    44,    25,     4,    -1,     6,     7,
      50,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,     4,    -1,     6,     7,    -1,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      38,    39,    40,    -1,    -1,    -1,    44,    -1,    46,    47,
      -1,    -1,    50,    -1,    -1,    -1,    -1,    38,    39,    40,
      -1,    -1,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      13,    14,    15,    16,    17,    18,    19,    20,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    -1,    -1,    -1,    38,    39,    40,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    -1,    -1,    50,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    51,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    51,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    51,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    48,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    48,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const rw_yytype_uint8 rw_yystos[] =
{
       0,     3,    13,    14,    15,    16,    17,    18,    19,    20,
      38,    39,    40,    44,    50,    53,    54,    55,    56,    57,
      78,    14,    18,    22,    50,    50,    22,    50,    78,    78,
      20,    78,     3,    78,     0,     1,    56,    46,    58,    60,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      50,    65,    65,    78,    68,    69,    70,    78,    68,    78,
      78,    50,    51,    51,     4,     6,     7,     9,    10,    11,
      12,    58,    71,    72,    74,    75,    78,     3,    62,    63,
      78,    78,    13,    13,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
       3,    51,    66,    67,    51,    49,    51,    51,    78,    78,
      50,    77,    78,    48,    48,    20,    71,    47,    59,    61,
      72,    77,    72,    48,    14,    64,    63,    14,    49,    51,
      70,    22,    51,    78,    72,    48,    48,    50,    59,    72,
      73,    48,    49,    67,    78,    51,     5,    76,    78,     7,
      14,    72,    51,    77,    48,    48
};

#define rw_yyerrok		(rw_yyerrstatus = 0)
#define rw_yyclearin	(rw_yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto rw_yyacceptlab
#define YYABORT		goto rw_yyabortlab
#define YYERROR		goto rw_yyerrorlab


/* Like YYERROR except do call rw_yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto rw_yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!rw_yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (rw_yychar == YYEMPTY)                                        \
    {                                                           \
      rw_yychar = (Token);                                         \
      rw_yylval = (Value);                                         \
      YYPOPSTACK (rw_yylen);                                       \
      rw_yystate = *rw_yyssp;                                         \
      goto rw_yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      rw_yyerror (YY_("syntax error: cannot back up")); \
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


/* YYLEX -- calling `rw_yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX rw_yylex (YYLEX_PARAM)
#else
# define YYLEX rw_yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (rw_yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (rw_yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      rw_yy_symbol_print (stderr,						  \
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
rw_yy_symbol_value_print (FILE *rw_yyoutput, int rw_yytype, YYSTYPE const * const rw_yyvaluep)
#else
static void
rw_yy_symbol_value_print (rw_yyoutput, rw_yytype, rw_yyvaluep)
    FILE *rw_yyoutput;
    int rw_yytype;
    YYSTYPE const * const rw_yyvaluep;
#endif
{
  FILE *rw_yyo = rw_yyoutput;
  YYUSE (rw_yyo);
  if (!rw_yyvaluep)
    return;
# ifdef YYPRINT
  if (rw_yytype < YYNTOKENS)
    YYPRINT (rw_yyoutput, rw_yytoknum[rw_yytype], *rw_yyvaluep);
# else
  YYUSE (rw_yyoutput);
# endif
  switch (rw_yytype)
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
rw_yy_symbol_print (FILE *rw_yyoutput, int rw_yytype, YYSTYPE const * const rw_yyvaluep)
#else
static void
rw_yy_symbol_print (rw_yyoutput, rw_yytype, rw_yyvaluep)
    FILE *rw_yyoutput;
    int rw_yytype;
    YYSTYPE const * const rw_yyvaluep;
#endif
{
  if (rw_yytype < YYNTOKENS)
    YYFPRINTF (rw_yyoutput, "token %s (", rw_yytname[rw_yytype]);
  else
    YYFPRINTF (rw_yyoutput, "nterm %s (", rw_yytname[rw_yytype]);

  rw_yy_symbol_value_print (rw_yyoutput, rw_yytype, rw_yyvaluep);
  YYFPRINTF (rw_yyoutput, ")");
}

/*------------------------------------------------------------------.
| rw_yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
rw_yy_stack_print (rw_yytype_int16 *rw_yybottom, rw_yytype_int16 *rw_yytop)
#else
static void
rw_yy_stack_print (rw_yybottom, rw_yytop)
    rw_yytype_int16 *rw_yybottom;
    rw_yytype_int16 *rw_yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; rw_yybottom <= rw_yytop; rw_yybottom++)
    {
      int rw_yybot = *rw_yybottom;
      YYFPRINTF (stderr, " %d", rw_yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (rw_yydebug)							\
    rw_yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
rw_yy_reduce_print (YYSTYPE *rw_yyvsp, int rw_yyrule)
#else
static void
rw_yy_reduce_print (rw_yyvsp, rw_yyrule)
    YYSTYPE *rw_yyvsp;
    int rw_yyrule;
#endif
{
  int rw_yynrhs = rw_yyr2[rw_yyrule];
  int rw_yyi;
  unsigned long int rw_yylno = rw_yyrline[rw_yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     rw_yyrule - 1, rw_yylno);
  /* The symbols being reduced.  */
  for (rw_yyi = 0; rw_yyi < rw_yynrhs; rw_yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", rw_yyi + 1);
      rw_yy_symbol_print (stderr, rw_yyrhs[rw_yyprhs[rw_yyrule] + rw_yyi],
		       &(rw_yyvsp[(rw_yyi + 1) - (rw_yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (rw_yydebug)				\
    rw_yy_reduce_print (rw_yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int rw_yydebug;
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

# ifndef rw_yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define rw_yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
rw_yystrlen (const char *rw_yystr)
#else
static YYSIZE_T
rw_yystrlen (rw_yystr)
    const char *rw_yystr;
#endif
{
  YYSIZE_T rw_yylen;
  for (rw_yylen = 0; rw_yystr[rw_yylen]; rw_yylen++)
    continue;
  return rw_yylen;
}
#  endif
# endif

# ifndef rw_yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define rw_yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
rw_yystpcpy (char *rw_yydest, const char *rw_yysrc)
#else
static char *
rw_yystpcpy (rw_yydest, rw_yysrc)
    char *rw_yydest;
    const char *rw_yysrc;
#endif
{
  char *rw_yyd = rw_yydest;
  const char *rw_yys = rw_yysrc;

  while ((*rw_yyd++ = *rw_yys++) != '\0')
    continue;

  return rw_yyd - 1;
}
#  endif
# endif

# ifndef rw_yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for rw_yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from rw_yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
rw_yytnamerr (char *rw_yyres, const char *rw_yystr)
{
  if (*rw_yystr == '"')
    {
      YYSIZE_T rw_yyn = 0;
      char const *rw_yyp = rw_yystr;

      for (;;)
	switch (*++rw_yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++rw_yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (rw_yyres)
	      rw_yyres[rw_yyn] = *rw_yyp;
	    rw_yyn++;
	    break;

	  case '"':
	    if (rw_yyres)
	      rw_yyres[rw_yyn] = '\0';
	    return rw_yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! rw_yyres)
    return rw_yystrlen (rw_yystr);

  return rw_yystpcpy (rw_yyres, rw_yystr) - rw_yyres;
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
rw_yysyntax_error (YYSIZE_T *rw_yymsg_alloc, char **rw_yymsg,
                rw_yytype_int16 *rw_yyssp, int rw_yytoken)
{
  YYSIZE_T rw_yysize0 = rw_yytnamerr (YY_NULL, rw_yytname[rw_yytoken]);
  YYSIZE_T rw_yysize = rw_yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *rw_yyformat = YY_NULL;
  /* Arguments of rw_yyformat. */
  char const *rw_yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int rw_yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in rw_yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated rw_yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (rw_yytoken != YYEMPTY)
    {
      int rw_yyn = rw_yypact[*rw_yyssp];
      rw_yyarg[rw_yycount++] = rw_yytname[rw_yytoken];
      if (!rw_yypact_value_is_default (rw_yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int rw_yyxbegin = rw_yyn < 0 ? -rw_yyn : 0;
          /* Stay within bounds of both rw_yycheck and rw_yytname.  */
          int rw_yychecklim = YYLAST - rw_yyn + 1;
          int rw_yyxend = rw_yychecklim < YYNTOKENS ? rw_yychecklim : YYNTOKENS;
          int rw_yyx;

          for (rw_yyx = rw_yyxbegin; rw_yyx < rw_yyxend; ++rw_yyx)
            if (rw_yycheck[rw_yyx + rw_yyn] == rw_yyx && rw_yyx != YYTERROR
                && !rw_yytable_value_is_error (rw_yytable[rw_yyx + rw_yyn]))
              {
                if (rw_yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    rw_yycount = 1;
                    rw_yysize = rw_yysize0;
                    break;
                  }
                rw_yyarg[rw_yycount++] = rw_yytname[rw_yyx];
                {
                  YYSIZE_T rw_yysize1 = rw_yysize + rw_yytnamerr (YY_NULL, rw_yytname[rw_yyx]);
                  if (! (rw_yysize <= rw_yysize1
                         && rw_yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  rw_yysize = rw_yysize1;
                }
              }
        }
    }

  switch (rw_yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        rw_yyformat = S;                       \
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
    YYSIZE_T rw_yysize1 = rw_yysize + rw_yystrlen (rw_yyformat);
    if (! (rw_yysize <= rw_yysize1 && rw_yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    rw_yysize = rw_yysize1;
  }

  if (*rw_yymsg_alloc < rw_yysize)
    {
      *rw_yymsg_alloc = 2 * rw_yysize;
      if (! (rw_yysize <= *rw_yymsg_alloc
             && *rw_yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *rw_yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *rw_yyp = *rw_yymsg;
    int rw_yyi = 0;
    while ((*rw_yyp = *rw_yyformat) != '\0')
      if (*rw_yyp == '%' && rw_yyformat[1] == 's' && rw_yyi < rw_yycount)
        {
          rw_yyp += rw_yytnamerr (rw_yyp, rw_yyarg[rw_yyi++]);
          rw_yyformat += 2;
        }
      else
        {
          rw_yyp++;
          rw_yyformat++;
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
rw_yydestruct (const char *rw_yymsg, int rw_yytype, YYSTYPE *rw_yyvaluep)
#else
static void
rw_yydestruct (rw_yymsg, rw_yytype, rw_yyvaluep)
    const char *rw_yymsg;
    int rw_yytype;
    YYSTYPE *rw_yyvaluep;
#endif
{
  YYUSE (rw_yyvaluep);

  if (!rw_yymsg)
    rw_yymsg = "Deleting";
  YY_SYMBOL_PRINT (rw_yymsg, rw_yytype, rw_yyvaluep, rw_yylocationp);

  switch (rw_yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int rw_yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE rw_yylval YY_INITIAL_VALUE(rw_yyval_default);

/* Number of syntax errors so far.  */
int rw_yynerrs;


/*----------.
| rw_yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
rw_yyparse (void *YYPARSE_PARAM)
#else
int
rw_yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
rw_yyparse (void)
#else
int
rw_yyparse ()

#endif
#endif
{
    int rw_yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int rw_yyerrstatus;

    /* The stacks and their tools:
       `rw_yyss': related to states.
       `rw_yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow rw_yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    rw_yytype_int16 rw_yyssa[YYINITDEPTH];
    rw_yytype_int16 *rw_yyss;
    rw_yytype_int16 *rw_yyssp;

    /* The semantic value stack.  */
    YYSTYPE rw_yyvsa[YYINITDEPTH];
    YYSTYPE *rw_yyvs;
    YYSTYPE *rw_yyvsp;

    YYSIZE_T rw_yystacksize;

  int rw_yyn;
  int rw_yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int rw_yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE rw_yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char rw_yymsgbuf[128];
  char *rw_yymsg = rw_yymsgbuf;
  YYSIZE_T rw_yymsg_alloc = sizeof rw_yymsgbuf;
#endif

#define YYPOPSTACK(N)   (rw_yyvsp -= (N), rw_yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int rw_yylen = 0;

  rw_yyssp = rw_yyss = rw_yyssa;
  rw_yyvsp = rw_yyvs = rw_yyvsa;
  rw_yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  rw_yystate = 0;
  rw_yyerrstatus = 0;
  rw_yynerrs = 0;
  rw_yychar = YYEMPTY; /* Cause a token to be read.  */
  goto rw_yysetstate;

/*------------------------------------------------------------.
| rw_yynewstate -- Push a new state, which is found in rw_yystate.  |
`------------------------------------------------------------*/
 rw_yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  rw_yyssp++;

 rw_yysetstate:
  *rw_yyssp = rw_yystate;

  if (rw_yyss + rw_yystacksize - 1 <= rw_yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T rw_yysize = rw_yyssp - rw_yyss + 1;

#ifdef rw_yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *rw_yyvs1 = rw_yyvs;
	rw_yytype_int16 *rw_yyss1 = rw_yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if rw_yyoverflow is a macro.  */
	rw_yyoverflow (YY_("memory exhausted"),
		    &rw_yyss1, rw_yysize * sizeof (*rw_yyssp),
		    &rw_yyvs1, rw_yysize * sizeof (*rw_yyvsp),
		    &rw_yystacksize);

	rw_yyss = rw_yyss1;
	rw_yyvs = rw_yyvs1;
      }
#else /* no rw_yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto rw_yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= rw_yystacksize)
	goto rw_yyexhaustedlab;
      rw_yystacksize *= 2;
      if (YYMAXDEPTH < rw_yystacksize)
	rw_yystacksize = YYMAXDEPTH;

      {
	rw_yytype_int16 *rw_yyss1 = rw_yyss;
	union rw_yyalloc *rw_yyptr =
	  (union rw_yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (rw_yystacksize));
	if (! rw_yyptr)
	  goto rw_yyexhaustedlab;
	YYSTACK_RELOCATE (rw_yyss_alloc, rw_yyss);
	YYSTACK_RELOCATE (rw_yyvs_alloc, rw_yyvs);
#  undef YYSTACK_RELOCATE
	if (rw_yyss1 != rw_yyssa)
	  YYSTACK_FREE (rw_yyss1);
      }
# endif
#endif /* no rw_yyoverflow */

      rw_yyssp = rw_yyss + rw_yysize - 1;
      rw_yyvsp = rw_yyvs + rw_yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) rw_yystacksize));

      if (rw_yyss + rw_yystacksize - 1 <= rw_yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", rw_yystate));

  if (rw_yystate == YYFINAL)
    YYACCEPT;

  goto rw_yybackup;

/*-----------.
| rw_yybackup.  |
`-----------*/
rw_yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  rw_yyn = rw_yypact[rw_yystate];
  if (rw_yypact_value_is_default (rw_yyn))
    goto rw_yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (rw_yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      rw_yychar = YYLEX;
    }

  if (rw_yychar <= YYEOF)
    {
      rw_yychar = rw_yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      rw_yytoken = YYTRANSLATE (rw_yychar);
      YY_SYMBOL_PRINT ("Next token is", rw_yytoken, &rw_yylval, &rw_yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  rw_yyn += rw_yytoken;
  if (rw_yyn < 0 || YYLAST < rw_yyn || rw_yycheck[rw_yyn] != rw_yytoken)
    goto rw_yydefault;
  rw_yyn = rw_yytable[rw_yyn];
  if (rw_yyn <= 0)
    {
      if (rw_yytable_value_is_error (rw_yyn))
        goto rw_yyerrlab;
      rw_yyn = -rw_yyn;
      goto rw_yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (rw_yyerrstatus)
    rw_yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", rw_yytoken, &rw_yylval, &rw_yylloc);

  /* Discard the shifted token.  */
  rw_yychar = YYEMPTY;

  rw_yystate = rw_yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++rw_yyvsp = rw_yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto rw_yynewstate;


/*-----------------------------------------------------------.
| rw_yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
rw_yydefault:
  rw_yyn = rw_yydefact[rw_yystate];
  if (rw_yyn == 0)
    goto rw_yyerrlab;
  goto rw_yyreduce;


/*-----------------------------.
| rw_yyreduce -- Do a reduction.  |
`-----------------------------*/
rw_yyreduce:
  /* rw_yyn is the number of a rule to reduce with.  */
  rw_yylen = rw_yyr2[rw_yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  rw_yyval = rw_yyvsp[1-rw_yylen];


  YY_REDUCE_PRINT (rw_yyn);
  switch (rw_yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 720 "rewrite.y"
    {
                  var_free_all();
                  loop_free_all();
                  frame_free_all();
                  mtx_free_all();
          }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 729 "rewrite.y"
    {
		  return_type = Undefined;
	  }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 733 "rewrite.y"
    {
                  if (errcnt) {
                          YYERROR;
                  }
		  
		  mtx_return((rw_yyvsp[(1) - (1)].mtx));
                  
		  memset(&fmain, 0, sizeof(fmain));
		  fmain.name = "main";
		  fmain.rettype = return_type = (rw_yyvsp[(1) - (1)].mtx)->gen.datatype;
		  function = &fmain;

                  if (optimize() == 0) {
                          codegen();
                          if (errcnt) {
                                  YYERROR;
                          }
                  }
          }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 757 "rewrite.y"
    {
                  /* Roll back all changes done so far */
                  var_unwind_all();
                  loop_unwind_all();
                  frame_unwind_all();
                  mtx_unwind_all();
                  function_delete();
                  /* Synchronize input after error */
                  rw_yysync();
                  /* Clear input and error condition */
                  rw_yyclearin;
                  rw_yyerrok;
                  errcnt = 0;
          }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 774 "rewrite.y"
    {
                  if (errcnt) {
                          function_delete();
                          YYERROR;
                  }
                  
                  if (optimize() == 0) {
                          codegen();
                          if (errcnt) {
                                  function_delete();
                                  YYERROR;
                          }
                  } else {
                          function_delete();
                  }
                  
                  /* clean up things */
                  var_unwind_all();
                  loop_unwind_all();
                  frame_unwind_all();
                  mtx_unwind_all();
                  function_cleanup();
          }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 800 "rewrite.y"
    {
                  VAR *var;
                  PARAMETER *last, *parm;
                  FUNCTION f;
                  
                  if (errcnt)
                          YYERROR;
                  
                  memset(&f, 0, sizeof(f));
                  f.name    = (rw_yyvsp[(2) - (3)].string);
                  f.rettype = (rw_yyvsp[(1) - (3)].type);
                  f.entry   = 0;
                  f.loc     = locus;
                  
                  f.nparm   = 0;
                  f.parm    = NULL;

                  /* Count number of parameters */
                  for (var = (rw_yyvsp[(3) - (3)].var); var; var = var->next) 
                          f.nparm++;

                  f.parm = last = NULL;
                  for (var = (rw_yyvsp[(3) - (3)].var); var; var = var->next) {
                          parm = grad_emalloc(sizeof(*parm));
                          parm->datatype = var->datatype;
                          var->offset = -(STACK_BASE+
                                          f.nparm - var->offset);
                          parm->offset   = var->offset;
                          parm->prev     = last;
                          parm->next     = NULL;
                          if (f.parm == NULL)
                                  f.parm = parm;
                          else 
                                  last->next = parm;
                          last = parm;
                  }
                  function = function_install(&f);
          }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 839 "rewrite.y"
    {
		  grad_log_loc(GRAD_LOG_ERR, &locus,
			       _("redefinition of function `%s'"), (rw_yyvsp[(2) - (3)].fun)->name);
		  grad_log_loc(GRAD_LOG_ERR, &(rw_yyvsp[(2) - (3)].fun)->loc,
			       _("previously defined here"));
		  errcnt++;
		  YYERROR;
          }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 857 "rewrite.y"
    {
                  frame_push();
          }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 863 "rewrite.y"
    {
                  var_unwind_level();
                  frame_pop();
          }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 878 "rewrite.y"
    {
                  var_type((rw_yyvsp[(1) - (3)].type), (rw_yyvsp[(2) - (3)].var));
          }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 884 "rewrite.y"
    {
                  (rw_yyval.var) = var_alloc(Undefined, (rw_yyvsp[(1) - (1)].string), +1);
                  (rw_yyval.var)->dcllink = NULL;
          }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 889 "rewrite.y"
    {
                  VAR *var = var_alloc(Undefined, (rw_yyvsp[(3) - (3)].string), +1);
                  var->dcllink = (rw_yyvsp[(1) - (3)].var);
                  (rw_yyval.var) = var;
          }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 900 "rewrite.y"
    {
                  (rw_yyval.var) = NULL;
          }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 904 "rewrite.y"
    {
                  (rw_yyval.var) = (rw_yyvsp[(2) - (3)].var);
          }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 910 "rewrite.y"
    {
                  /*FIXME*/
                  /*$$->dcllink = NULL;*/
          }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 915 "rewrite.y"
    {
                  /*$1->dcllink = $3;*/
                  (rw_yyval.var) = (rw_yyvsp[(1) - (3)].var);
          }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 922 "rewrite.y"
    {
                  (rw_yyval.var) = var_alloc((rw_yyvsp[(1) - (2)].type), (rw_yyvsp[(2) - (2)].string), +1);
          }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 931 "rewrite.y"
    {
                  (rw_yyval.mtx) = NULL;
          }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 935 "rewrite.y"
    {
                  (rw_yyval.mtx) = (rw_yyvsp[(1) - (1)].arg).arg_first;
          }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 941 "rewrite.y"
    {
                  (rw_yyvsp[(1) - (1)].mtx)->gen.arglink = NULL;
                  (rw_yyval.arg).arg_first = (rw_yyval.arg).arg_last = (rw_yyvsp[(1) - (1)].mtx);
          }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 946 "rewrite.y"
    {
                  (rw_yyvsp[(1) - (3)].arg).arg_last->gen.arglink = (rw_yyvsp[(3) - (3)].mtx);
                  (rw_yyvsp[(1) - (3)].arg).arg_last = (rw_yyvsp[(3) - (3)].mtx);
                  (rw_yyval.arg) = (rw_yyvsp[(1) - (3)].arg);
          }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 964 "rewrite.y"
    {
                  (rw_yyval.mtx) = (rw_yyvsp[(2) - (3)].mtx);
          }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 968 "rewrite.y"
    {
                  mtx_stop();
                  mtx_pop();
          }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 973 "rewrite.y"
    {
                  (rw_yyvsp[(2) - (3)].mtx)->cond.if_false = mtx_nop();
                  (rw_yyval.mtx) = mtx_cur();
          }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 978 "rewrite.y"
    {
                  mtx_stop();
                  (rw_yyvsp[(2) - (5)].mtx)->cond.if_false = (rw_yyvsp[(4) - (5)].mtx);
                  (rw_yyvsp[(4) - (5)].mtx)->nop.prev->jump.dest = mtx_nop();
                  (rw_yyval.mtx) = mtx_cur();
          }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 985 "rewrite.y"
    {
                  /*mtx_stop();*/
                  (rw_yyval.mtx) = mtx_return((rw_yyvsp[(2) - (3)].mtx));
          }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 990 "rewrite.y"
    {
                  MTX *mtx;
                  
                  mtx_stop();
                  mtx = mtx_jump();
                  mtx->jump.dest = (rw_yyvsp[(1) - (3)].mtx);
                  (rw_yyvsp[(2) - (3)].mtx)->cond.if_false = mtx_nop();
                  (rw_yyval.mtx) = mtx_cur();
                  
                  /* Fixup possible breaks */
                  loop_fixup(loop_last->lp_break, (rw_yyval.mtx));
                  /* Fixup possible continues */
                  loop_fixup(loop_last->lp_cont, (rw_yyvsp[(1) - (3)].mtx));
                  loop_pop();
          }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 1005 "rewrite.y"
    { (rw_yyval.mtx) = mtx_nop(); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 1006 "rewrite.y"
    {
                  /* Default cond rule sets if_true to the next NOP matrix
                   * Invert this behaviour.
                   */
                  (rw_yyvsp[(5) - (6)].mtx)->cond.if_false = (rw_yyvsp[(5) - (6)].mtx)->cond.if_true;
                  (rw_yyvsp[(5) - (6)].mtx)->cond.if_true = (rw_yyvsp[(1) - (6)].mtx);
                  (rw_yyval.mtx) = mtx_cur();

                  /* Fixup possible breaks */
                  loop_fixup(loop_last->lp_break, (rw_yyval.mtx));
                  /* Fixup possible continues */
                  loop_fixup(loop_last->lp_cont, (rw_yyvsp[(3) - (6)].mtx));
                  loop_pop();
          }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 1025 "rewrite.y"
    {
                  if (!loop_last) {
                          grad_log_loc(GRAD_LOG_ERR, &locus,
				       "%s",
				       _("nothing to break from"));
                          errcnt++;
                          YYERROR;
                  }

                  (rw_yyval.mtx) = mtx_jump();
                  (rw_yyval.mtx)->jump.link = (MTX*)loop_last->lp_break;
                  loop_last->lp_break = (JUMP_MTX*)(rw_yyval.mtx);
          }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 1039 "rewrite.y"
    {
                  if (!loop_last) {
                          grad_log_loc(GRAD_LOG_ERR, &locus,
				       "%s",
				       _("nothing to continue"));
                          errcnt++;
                          YYERROR;
                  }
                  (rw_yyval.mtx) = mtx_jump();
                  (rw_yyval.mtx)->jump.link = (MTX*)loop_last->lp_cont;
                  loop_last->lp_cont = (JUMP_MTX*)(rw_yyval.mtx);
          }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 1052 "rewrite.y"
    {
		  (rw_yyval.mtx) = mtx_attr_delete((rw_yyvsp[(2) - (3)].attr), NULL);
	  }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 1056 "rewrite.y"
    {
		  (rw_yyval.mtx) = mtx_attr_delete((rw_yyvsp[(2) - (6)].attr), (rw_yyvsp[(4) - (6)].mtx));
	  }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 1062 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_nop();
                  loop_push((rw_yyval.mtx));
          }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 1069 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_nop();
                  loop_push((rw_yyval.mtx));
          }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 1076 "rewrite.y"
    {
                  mtx_stop();
                  mtx_jump();
                  (rw_yyval.mtx) = mtx_nop();
          }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 1084 "rewrite.y"
    {
                  mtx_stop();
                  (rw_yyval.mtx) = mtx_cond((rw_yyvsp[(2) - (3)].mtx), NULL, NULL);
                  (rw_yyval.mtx)->cond.if_true = mtx_nop();
          }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 1095 "rewrite.y"
    {
		  grad_value_t val;
		  val.type = Integer;
		  val.datum.ival = (rw_yyvsp[(1) - (1)].number);
                  (rw_yyval.mtx) = mtx_const(&val);
          }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 1102 "rewrite.y"
    {
		  grad_value_t val;
		  val.type = String;
		  val.datum.sval.size = strlen((rw_yyvsp[(1) - (1)].string));
		  val.datum.sval.data = (rw_yyvsp[(1) - (1)].string);
                  (rw_yyval.mtx) = mtx_const(&val);
          }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 1110 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_ref((rw_yyvsp[(1) - (1)].number));
          }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 1114 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_var((rw_yyvsp[(1) - (1)].var));
          }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 1118 "rewrite.y"
    {
                  grad_log_loc(GRAD_LOG_ERR, &locus, _("undefined variable: %s"), (rw_yyvsp[(1) - (1)].string));
                  errcnt++;
                  YYERROR;
          }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 1124 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_asgn((rw_yyvsp[(1) - (3)].var), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 1128 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_attr((rw_yyvsp[(1) - (1)].attr), NULL);
          }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 1132 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_attr((rw_yyvsp[(1) - (4)].attr), (rw_yyvsp[(3) - (4)].mtx));
          }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 1136 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_attr_check((rw_yyvsp[(2) - (2)].attr), NULL);
          }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 1140 "rewrite.y"
    {
		  (rw_yyval.mtx) = mtx_attr_check((rw_yyvsp[(2) - (5)].attr), (rw_yyvsp[(4) - (5)].mtx));
	  }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 1144 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_attr_asgn((rw_yyvsp[(1) - (3)].attr), NULL, (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 1148 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_attr_asgn((rw_yyvsp[(1) - (6)].attr), (rw_yyvsp[(3) - (6)].mtx), (rw_yyvsp[(6) - (6)].mtx));
          }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 1152 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_call((rw_yyvsp[(1) - (4)].fun), (rw_yyvsp[(3) - (4)].mtx));
          }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 1156 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_builtin((rw_yyvsp[(1) - (4)].btin), (rw_yyvsp[(3) - (4)].mtx));
          }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 1160 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Add, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 1164 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Sub, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 1168 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Mul, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 1172 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Div, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 1176 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Rem, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 1180 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(BOr, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 1184 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(BAnd, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 1188 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(BXor, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 1192 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Shl, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 1196 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Shr, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 1200 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(And, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 1204 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Or, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 1208 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_un(Neg, (rw_yyvsp[(2) - (2)].mtx));
          }
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 1212 "rewrite.y"
    {
                  (rw_yyval.mtx) = (rw_yyvsp[(2) - (2)].mtx);
          }
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 1216 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_un(Not, (rw_yyvsp[(2) - (2)].mtx));
          }
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 1220 "rewrite.y"
    {
                  (rw_yyval.mtx) = (rw_yyvsp[(2) - (3)].mtx);
          }
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 1224 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_coerce((rw_yyvsp[(2) - (4)].type), (rw_yyvsp[(4) - (4)].mtx));
          }
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 1228 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Eq, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 1232 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Ne, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 1236 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Lt, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 1240 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Le, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 1244 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Gt, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 1248 "rewrite.y"
    {
                  (rw_yyval.mtx) = mtx_bin(Ge, (rw_yyvsp[(1) - (3)].mtx), (rw_yyvsp[(3) - (3)].mtx));
          }
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 1252 "rewrite.y"
    {
                  COMP_REGEX *rx;
                  if ((rx = compile_regexp((rw_yyvsp[(3) - (3)].string))) == NULL) {
                          errcnt++;
                          YYERROR;
                  }
                  (rw_yyval.mtx) = mtx_match(0, (rw_yyvsp[(1) - (3)].mtx), rx);
          }
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 1261 "rewrite.y"
    {
                  COMP_REGEX *rx;
                  if ((rx = compile_regexp((rw_yyvsp[(3) - (3)].string))) == NULL) {
                          errcnt++;
                          YYERROR;
                  }
                  (rw_yyval.mtx) = mtx_match(1, (rw_yyvsp[(1) - (3)].mtx), rx);
          }
    break;


/* Line 1792 of yacc.c  */
#line 3084 "rewrite.c"
      default: break;
    }
  /* User semantic actions sometimes alter rw_yychar, and that requires
     that rw_yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of rw_yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering rw_yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", rw_yyr1[rw_yyn], &rw_yyval, &rw_yyloc);

  YYPOPSTACK (rw_yylen);
  rw_yylen = 0;
  YY_STACK_PRINT (rw_yyss, rw_yyssp);

  *++rw_yyvsp = rw_yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  rw_yyn = rw_yyr1[rw_yyn];

  rw_yystate = rw_yypgoto[rw_yyn - YYNTOKENS] + *rw_yyssp;
  if (0 <= rw_yystate && rw_yystate <= YYLAST && rw_yycheck[rw_yystate] == *rw_yyssp)
    rw_yystate = rw_yytable[rw_yystate];
  else
    rw_yystate = rw_yydefgoto[rw_yyn - YYNTOKENS];

  goto rw_yynewstate;


/*------------------------------------.
| rw_yyerrlab -- here on detecting error |
`------------------------------------*/
rw_yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  rw_yytoken = rw_yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (rw_yychar);

  /* If not already recovering from an error, report this error.  */
  if (!rw_yyerrstatus)
    {
      ++rw_yynerrs;
#if ! YYERROR_VERBOSE
      rw_yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR rw_yysyntax_error (&rw_yymsg_alloc, &rw_yymsg, \
                                        rw_yyssp, rw_yytoken)
      {
        char const *rw_yymsgp = YY_("syntax error");
        int rw_yysyntax_error_status;
        rw_yysyntax_error_status = YYSYNTAX_ERROR;
        if (rw_yysyntax_error_status == 0)
          rw_yymsgp = rw_yymsg;
        else if (rw_yysyntax_error_status == 1)
          {
            if (rw_yymsg != rw_yymsgbuf)
              YYSTACK_FREE (rw_yymsg);
            rw_yymsg = (char *) YYSTACK_ALLOC (rw_yymsg_alloc);
            if (!rw_yymsg)
              {
                rw_yymsg = rw_yymsgbuf;
                rw_yymsg_alloc = sizeof rw_yymsgbuf;
                rw_yysyntax_error_status = 2;
              }
            else
              {
                rw_yysyntax_error_status = YYSYNTAX_ERROR;
                rw_yymsgp = rw_yymsg;
              }
          }
        rw_yyerror (rw_yymsgp);
        if (rw_yysyntax_error_status == 2)
          goto rw_yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (rw_yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (rw_yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (rw_yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  rw_yydestruct ("Error: discarding",
		      rw_yytoken, &rw_yylval);
	  rw_yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto rw_yyerrlab1;


/*---------------------------------------------------.
| rw_yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
rw_yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label rw_yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto rw_yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (rw_yylen);
  rw_yylen = 0;
  YY_STACK_PRINT (rw_yyss, rw_yyssp);
  rw_yystate = *rw_yyssp;
  goto rw_yyerrlab1;


/*-------------------------------------------------------------.
| rw_yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
rw_yyerrlab1:
  rw_yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      rw_yyn = rw_yypact[rw_yystate];
      if (!rw_yypact_value_is_default (rw_yyn))
	{
	  rw_yyn += YYTERROR;
	  if (0 <= rw_yyn && rw_yyn <= YYLAST && rw_yycheck[rw_yyn] == YYTERROR)
	    {
	      rw_yyn = rw_yytable[rw_yyn];
	      if (0 < rw_yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (rw_yyssp == rw_yyss)
	YYABORT;


      rw_yydestruct ("Error: popping",
		  rw_yystos[rw_yystate], rw_yyvsp);
      YYPOPSTACK (1);
      rw_yystate = *rw_yyssp;
      YY_STACK_PRINT (rw_yyss, rw_yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++rw_yyvsp = rw_yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", rw_yystos[rw_yyn], rw_yyvsp, rw_yylsp);

  rw_yystate = rw_yyn;
  goto rw_yynewstate;


/*-------------------------------------.
| rw_yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
rw_yyacceptlab:
  rw_yyresult = 0;
  goto rw_yyreturn;

/*-----------------------------------.
| rw_yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
rw_yyabortlab:
  rw_yyresult = 1;
  goto rw_yyreturn;

#if !defined rw_yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| rw_yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
rw_yyexhaustedlab:
  rw_yyerror (YY_("memory exhausted"));
  rw_yyresult = 2;
  /* Fall through.  */
#endif

rw_yyreturn:
  if (rw_yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      rw_yytoken = YYTRANSLATE (rw_yychar);
      rw_yydestruct ("Cleanup: discarding lookahead",
                  rw_yytoken, &rw_yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (rw_yylen);
  YY_STACK_PRINT (rw_yyss, rw_yyssp);
  while (rw_yyssp != rw_yyss)
    {
      rw_yydestruct ("Cleanup: popping",
		  rw_yystos[*rw_yyssp], rw_yyvsp);
      YYPOPSTACK (1);
    }
#ifndef rw_yyoverflow
  if (rw_yyss != rw_yyssa)
    YYSTACK_FREE (rw_yyss);
#endif
#if YYERROR_VERBOSE
  if (rw_yymsg != rw_yymsgbuf)
    YYSTACK_FREE (rw_yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (rw_yyresult);
}


/* Line 2055 of yacc.c  */
#line 1271 "rewrite.y"


int
rw_yyerror(char *s)
{
        grad_log_loc(GRAD_LOG_ERR, &locus, "%s", s);
        errcnt++;
	return 0;
}


/* **************************************************************************
 * Interface functions
 */
int
parse_rewrite(char *path)
{
        locus.file = path;
        infile = fopen(locus.file, "r");
        if (!infile) {
                if (errno != ENOENT) {
                        grad_log(GRAD_LOG_ERR|GRAD_LOG_PERROR,
                                 _("can't open file `%s'"),
                                 locus.file);
			return -1;
                }
                return -2;
        }

	GRAD_DEBUG1(1, "Loading file %s", locus.file);
        rw_code_lock();
        rw_yyeof = 0;
        locus.line = 1;
	errcnt = 0;
        regex_init();
        obstack_init(&input_stk);

        mtx_init();
        var_init();
        loop_init();
        frame_init();
        
        frame_push();
	
        rw_yyparse();

        var_free_all();
        frame_free_all();
        mtx_free_all();
                
        fclose(infile);
        obstack_free(&input_stk, NULL);
        rw_code_unlock();
        return errcnt;
}

static int
parse_rewrite_string(char *str)
{
        rw_code_lock();
	code_check();
        rw_yyeof = 0;
	locus.file = "<string>";
	locus.line = 1;
	errcnt = 0;
        regex_init();
        obstack_init(&input_stk);

        mtx_init();
        var_init();
        loop_init();
        frame_init();
        
        frame_push();
        
        if (GRAD_DEBUG_LEVEL(50))
                rw_yydebug++;

	infile = 0;
	inbuf = curp = str;
	
        rw_yyparse();

#if defined(MAINTAINER_MODE)
        if (GRAD_DEBUG_LEVEL(100))
                debug_dump_code();
#endif
        
        var_free_all();
        frame_free_all();
        mtx_free_all();
                
        obstack_free(&input_stk, NULL);
        rw_code_unlock();
        return errcnt;
}


/* **************************************************************************
 * Lexical analyzer stuff: too simple to be written in lex.
 */
static int
unput(int c)
{
	if (!c)
		return 0;
	if (infile)
		ungetc(c, infile);
	else if (curp > inbuf)
		*--curp = c;
	return c;
}

static int 
input()
{
        if (rw_yyeof)
                rw_yychar = 0;
	else if (infile) {
		if ((rw_yychar = getc(infile)) <= 0) {
			rw_yyeof++;
			rw_yychar = 0;
		}
	} else if (curp) {
		rw_yychar = *curp++;
		if (!rw_yychar)
			rw_yyeof++;
	}
        return rw_yychar;
}

static int  rw_backslash();
static int  c2d(int c);
static int  read_number();
static int  read_num(int n, int base);
static char *read_string();
static char *read_ident(int c);
static char *read_to_delim(int c);
static int  skip_to_nl();
static int c_comment();

/*
 * Convert a character to digit. Only octal, decimal and hex digits are
 * allowed. If any other character is input, c2d() returns 100, which is
 * greater than any number base allowed.
 */
int
c2d(int c)
{
        switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
                return c - '0';
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
                return c - 'A' + 16;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
                return c - 'a' + 10;
        }
        return 100;
}

/*
 * Read a number. Usual C conventions apply. 
 */
int
read_number()
{
        int c;
        int base;
	int res;
	
        c = rw_yychar;
        if (c == '0') {
                if (input() == 'x' || rw_yychar == 'X') {
                        base = 16;
                } else {
                        base = 8;
                        unput(rw_yychar);
                }
        } else
                base = 10;

	res = read_num(c2d(c), base);
	if (base == 10 && rw_yychar == '.') {
		int n;

		for (n = 0; n < 3 && rw_yychar == '.'; n++) {
			int val;
			
			input();
			val = read_num(0, base);
			res = (res << 8) + val;
		}
		if (n != 3)
			res <<= 8 * (3-n);
	}
	return res;
}

int
read_num(int n, int base)
{
        int d;

        while (input() && (d = c2d(rw_yychar)) < 16) 
                n = n*base + d;
        unput(rw_yychar);
        return n;
}

int
rw_backslash()
{
        switch (input()) {
        case '\\':
                return '\\';
        case 'a':
                return '\a';
        case 'b':
                return '\b';
        case 'f':
                return '\f';
        case 'n':
                return '\n';
        case 'r':
                return '\r';
        case 't':
                return '\t';
        case 'e':
                return '\033';
        case '0':
                return read_number();
        case 'x':
        case 'X':
                return read_num(0, 16);
        case '(':
        case ')':
                /* Preserve regular expressions */
                unput(rw_yychar);
                rw_yychar = '\\';
        }
        return rw_yychar;
}

/*
 * Read a string up to the closing doublequote
 */
char *
read_string()
{
        while (input() && rw_yychar != '"') {
                if (rw_yychar == '\\')
                        rw_yychar = rw_backslash();
                obstack_1grow(&input_stk, rw_yychar);
        }
        obstack_1grow(&input_stk, 0);
        return obstack_finish(&input_stk);
}

/*
 * Read everything up to the given delimiter
 */
char *
read_to_delim(int c)
{
        while (input() && rw_yychar != c)
                obstack_1grow(&input_stk, rw_yychar);
        obstack_1grow(&input_stk, 0);
        return obstack_finish(&input_stk);
}

/*
 * Is `c' a part of the word?
 */
#define isword(c) (isalnum(c) || c == '_' || c == '$')

/*
 * Is `c' a whitespace character?
 */
#define isws(c) ((c) == ' ' || (c) == '\t')

/*
 * Read identifier
 */
char *
read_ident(int c)
{
        obstack_1grow(&input_stk, c);
        while (input() && isword(rw_yychar))
                obstack_1grow(&input_stk, rw_yychar);
        obstack_1grow(&input_stk, 0);
        unput(rw_yychar);
        return obstack_finish(&input_stk);
}

/*
 * Skip input up to the next newline
 */
int
skip_to_nl()
{
        while (input() && rw_yychar != '\n')
                ;
        return unput(rw_yychar);
}

/*
 * Skip a C-style comment
 */
int
c_comment()
{
        if (rw_yychar != '/')
                return 0;
        if (input() == '*') {
                size_t keep_line = locus.line;

                do {
                        while (input() != '*') {
                                if (rw_yychar == 0) {
                                        grad_log_loc(GRAD_LOG_ERR, &locus,
		       _("unexpected EOF in comment started at line %lu"),
						     (unsigned long) keep_line);
                                        return 0;
                                } else if (rw_yychar == '\n')
                                        locus.line++;
                        }
                } while (input() != '/');
                return 1;
        }
        unput(rw_yychar);
        rw_yychar = '/';
        return 0;
}


/* Pragmatic comments */
enum pragma_handler_phase {
	pragma_begin,
	pragma_cont,
	pragma_error,
	pragma_end
};

typedef int (*pragma_handler_fp) (enum pragma_handler_phase);

static int
regex_pragma (enum pragma_handler_phase phase)
{
	int disable = 0;
	int bit;
	char *s;
	static int regexp_accum;
	
	switch (phase) {
	case pragma_begin:
		regexp_accum = 0;
		return 0;
		
	case pragma_end:
		regcomp_flags = regexp_accum;
		return 0;

	case pragma_error:
		return 0;
		
	case pragma_cont:
		break;
	}

	switch (rw_yychar) {
	case '+':
		disable = 0;
		input();
		break;

	case '-':
		disable = 1;
		input();
		break;
	}
	if (!isword(rw_yychar)) {
		grad_log_loc(GRAD_LOG_ERR, &locus, _("Malformed pragma"));
		return 1;
	}
	
	s = read_ident(rw_yychar);

	if (strcmp (s, "extended") == 0)
		bit = REG_EXTENDED;
	else if (strcmp (s, "icase") == 0)
		bit = REG_ICASE;
	else if (strcmp (s, "newline") == 0)
		bit = REG_NEWLINE;
	else {
		grad_log_loc(GRAD_LOG_ERR, &locus,
			     _("Unknown regexp flag: %s"), s);
		return 1;
	}

	if (disable)
		regexp_accum &= ~bit;
	else
		regexp_accum |= bit;
	return 0;
}

static pragma_handler_fp
find_pragma_handler(char *s)
{
	if (strcmp(s, "regex") == 0)
		return regex_pragma;
	return NULL;
}

static void
handle_pragma()
{
	int rc;
	pragma_handler_fp pragma_handler;
	
	while (input() && isws(rw_yychar))
		;
	if (rw_yychar == 0)
		return;
			
	pragma_handler = find_pragma_handler (read_ident(rw_yychar));
		
	if (pragma_handler) {
		pragma_handler(pragma_begin);

		do {
			while (input() && isws(rw_yychar))
				;
			if (rw_yychar == 0 || rw_yychar == '\n')
				break;
			rc = pragma_handler(pragma_cont);
		} while (rc == 0 && rw_yychar != '\n' && rw_yychar != 0);
		
		pragma_handler(rc ? pragma_error : pragma_end);
	}
}




/* Parse a 'sharp' (single-line) comment */
void
sharp_comment()
{
	while (input() && isws(rw_yychar))
		;
	if (rw_yychar == 0)
		return;
	else if (rw_yychar == '\n') {
		locus.line++;
		return;
	} else if (isword(rw_yychar)) {
		if (strcmp(read_ident(rw_yychar), "pragma") == 0)
			handle_pragma();
	}
		
	skip_to_nl();
}


#if defined(MAINTAINER_MODE)
# define DEBUG_LEX1(s) if (GRAD_DEBUG_LEVEL(60)) printf("rw_yylex: " s "\n")
# define DEBUG_LEX2(s,v) if (GRAD_DEBUG_LEVEL(60)) printf("rw_yylex: " s "\n", v)
#else
# define DEBUG_LEX1(s)
# define DEBUG_LEX2(s,v)
#endif

static grad_keyword_t rw_kw[] = {
        { "if",       IF },
        { "else",     ELSE },
        { "return",   RETURN },
        { "for",      FOR },
        { "do",       DO },
        { "while",    WHILE },
        { "break",    BREAK },
        { "continue", CONTINUE },
	{ "delete",   DELETE },
        { NULL }
};

int
rw_yylex()
{
        int nl;
        int c;
        VAR *var;
        FUNCTION *fun;
        builtin_t *btin;
        
        /* Skip whitespace and comment lines */
        do {
                nl = 0;
                while (input() && isspace(rw_yychar))
                        if (rw_yychar == '\n')
                                locus.line++;
        
                if (!rw_yychar)
                        return 0;

                if (rw_yychar == '#') {
                        sharp_comment();
                        nl = 1;
                }
        } while (nl || c_comment());

        /*
         * A regexp reference
         */
        if (rw_yychar == '\\') {
                input();
                rw_yylval.number = read_number();
                DEBUG_LEX2("REFERENCE %d", rw_yylval.number);
                return REFERENCE;
        }

        /*
         * A character
         */
        if (rw_yychar == '\'') {
                if (input() == '\\')
                        c = rw_backslash();
                else
                        c = rw_yychar;
                if (input() != '\'') {
                        grad_log_loc(GRAD_LOG_ERR, &locus,
				     "%s",
				     _("unterminated character constant"));
                        errcnt++;
                }
                rw_yylval.number = c;
                DEBUG_LEX2("CHAR %d", c);
                return NUMBER;
        }
        
        /*
         * A number
         */
        if (isdigit(rw_yychar)) {
                rw_yylval.number = read_number();
                DEBUG_LEX2("NUMBER %d", rw_yylval.number);
                return NUMBER;
        }

        /*
         * Quoted string
         */
        if (rw_yychar == '"') {
                rw_yylval.string = read_string();
                DEBUG_LEX2("STRING %s", rw_yylval.string);
                return STRING;
        }

        /* A/V  pair reference.
           We do not allow %<number> sequences, since it would result
           in conflict with binary '%' operator.
           Thanks to Clement Gerouville for noticing.  */
        if (rw_yychar == '%') {
                grad_dict_attr_t *attr = 0;
                char *attr_name;
                
                input();
                if (rw_yychar == '[' || rw_yychar == '{') {
                        attr_name = read_to_delim(rw_yychar == '[' ? ']' : '}');
                        attr = grad_attr_name_to_dict(attr_name);
                } else {
                        unput(rw_yychar);
                        return '%';
                }
                if (!attr) {
                        grad_log_loc(GRAD_LOG_ERR, &locus,
				     _("unknown attribute `%s'"),
				     attr_name);
                        errcnt++;
                        return BOGUS;
                }
                rw_yylval.attr = attr;
                DEBUG_LEX2("ATTR: %s", attr->name);
                return ATTR;
        }
                               
                
        /*
         * Data type or identifier
         */
        if (isword(rw_yychar)) {
                rw_yylval.string = read_ident(rw_yychar);

                if (strcmp(rw_yylval.string, "integer") == 0) {
                        DEBUG_LEX1("TYPE(Integer)");
                        rw_yylval.type = Integer;
                        return TYPE;
                } else if (strcmp(rw_yylval.string, "string") == 0) {
                        DEBUG_LEX1("TYPE(String)");
                        rw_yylval.type = String;
                        return TYPE;
		}
		
                if ((c = grad_xlat_keyword(rw_kw, rw_yylval.string, 0)) != 0) {
                        DEBUG_LEX2("KW: %s", rw_yylval.string);
                        return c;
                }

                if (var = var_lookup(rw_yylval.string)) {
                        DEBUG_LEX2("VARIABLE: %s", rw_yylval.string);
                        rw_yylval.var = var;
                        return VARIABLE;
                }
                
                if (fun = (FUNCTION*) grad_sym_lookup(rewrite_tab, rw_yylval.string)) {
                        DEBUG_LEX2("FUN %s", rw_yylval.string);
                        rw_yylval.fun = fun;
                        return FUN;
                }

                if (btin = builtin_lookup(rw_yylval.string)) {
                        DEBUG_LEX2("BUILTIN %s", rw_yylval.string);
                        rw_yylval.btin = btin;
                        return BUILTIN;
                }
                DEBUG_LEX2("IDENT: %s", rw_yylval.string);
                return IDENT;
        }

        /*
         * Boolean expressions
         */
        if (rw_yychar == '&' || rw_yychar == '|') {
                int c = rw_yychar;

                if (input() == c) { 
                        DEBUG_LEX2("%s", rw_yychar == '&' ? "AND" : "OR"); 
                        return rw_yychar == '&' ? AND : OR;
                }
                unput(rw_yychar);
                
                DEBUG_LEX2("%c", c); 
                return c;
        }
        
        /*
         * Comparison operator
         */
        if (strchr("<>=!", rw_yychar)) {
                int c = rw_yychar;
                if (input() == '=') {
                        switch (c) {
                        case '<':
                                DEBUG_LEX1("LE");
                                return LE;
                        case '>':
                                DEBUG_LEX1("GE");
                                return GE;
                        case '=':
                                DEBUG_LEX1("EQ");
                                return EQ;
                        case '!':
                                DEBUG_LEX1("NE");
                                return NE;
                        }
                } else if (c == rw_yychar) {
                        if (c == '<') {
                                DEBUG_LEX1("SHL");
                                return SHL;
                        }
                        if (c == '>') {
                                DEBUG_LEX1("SHR");
                                return SHR;
                        }
                        unput(rw_yychar);
                        DEBUG_LEX2("%c", rw_yychar);
                        return rw_yychar;
                } else if (rw_yychar == '~') {
                        if (c == '=') {
                                DEBUG_LEX1("MT");
                                return MT;
                        }
                        if (c == '!') {
                                DEBUG_LEX1("NM");
                                return NM;
                        }
                }
                unput(rw_yychar);
                switch (c) {
                case '<':
                        DEBUG_LEX1("LT");
                        return LT;
                case '>':
                        DEBUG_LEX1("GT");
                        return GT;
                case '!':
                        DEBUG_LEX1("NOT");
                        return NOT;
                default:
                        return c;
                }
        }

        DEBUG_LEX2("%c", rw_yychar);
        return rw_yychar;
}

void
rw_yysync()
{
        while (skip_to_nl() == '\n' && !isalpha(input()))
                locus.line++;
        unput(rw_yychar);
}


/* ****************************************************************************
 * Generalized list functions
 */
static RWLIST *_list_insert(RWLIST **first, RWLIST **last, RWLIST *prev,
			    RWLIST *obj, int before);
static RWLIST *_list_remove(RWLIST **first, RWLIST **last, RWLIST *obj);
static RWLIST *_list_append(RWLIST **first, RWLIST **last, RWLIST *obj);

#define rw_list_insert(first, last, prev, obj, before) \
 _list_insert((RWLIST**)first,(RWLIST**)last,(RWLIST*)prev,(RWLIST*)obj, before)
#define rw_list_remove(first, last, obj) \
 _list_remove((RWLIST**)first,(RWLIST**)last,(RWLIST *)obj)
#define rw_list_append(first, last, obj) \
 _list_append((RWLIST**)first, (RWLIST**)last, (RWLIST*)obj)
        
RWLIST *
_list_append(RWLIST **first, RWLIST **last, RWLIST *obj)
{
        return rw_list_insert(first, last, *last, obj, 0);
}

RWLIST *
_list_insert(RWLIST **first, RWLIST **last, RWLIST *prev, RWLIST *obj,
	     int before)
{
        RWLIST   *next;

        /*
         * No first element: initialize whole list
         */
        if (!*first) {
                *first = obj;
                if (last)
                        *last = obj;
                obj->prev = obj->next = NULL;
                return obj;
        }

        /*
         * Insert before `prev'
         */
        if (before) {
                _list_insert(first, last, prev, obj, 0);
                _list_remove(first, last, prev);
                _list_insert(first, last, obj, prev, 0);
                return obj;
        }

        /*
         * Default: insert after prev
         */
        obj->prev = prev;
        obj->next = prev->next;
        
        if (next = prev->next)
                next->prev = obj;

        prev->next = obj;
        if (last && prev == *last)
                *last = obj;

                
        return obj;
}

RWLIST *
_list_remove(RWLIST **first, RWLIST **last, RWLIST *obj)
{
        RWLIST *temp;

        if (temp = obj->prev) 
                temp->next = obj->next;
        else
                *first = obj->next;

        if (temp = obj->next)
                temp->prev = obj->prev;
        else if (last)
                *last = obj->prev;

        obj->prev = obj->next = NULL;
        
        return obj;
}


/* ****************************************************************************
 * Generalized object handling
 */

void *obj_alloc(OBUCKET *bucket);
void obj_free_all(OBUCKET *bucket);
 

void *
obj_alloc(OBUCKET *bucket)
{
        OBJECT *optr;

        optr = grad_emalloc(bucket->size);

        optr->alloc        = bucket->alloc_list;
        bucket->alloc_list = optr;
        
        return optr;
}

void
obj_free_all(OBUCKET *bucket)
{
        OBJECT *obj, *next;

        obj = bucket->alloc_list;

        while (obj) {
                next = obj->alloc;
                if (bucket->free)
                        bucket->free(obj);
                grad_free(obj);
                obj = next;
        }
        bucket->alloc_list = NULL;
}


/* **************************************************************************
 * Frames
 */

void
frame_init()
{
        frame_bkt.alloc_list = NULL;
        frame_first = frame_last = NULL;
}

void
frame_push()
{
        FRAME *this_frame = obj_alloc(&frame_bkt);

        if (!frame_last) {
                this_frame->level = 0;
                this_frame->stack_offset = 0;
        } else {
                if (frame_last->level == 0)
                        this_frame->stack_offset = 1;
                else
                        this_frame->stack_offset = frame_last->stack_offset;
                this_frame->level = frame_last->level + 1;
        } 
        rw_list_append(&frame_first, &frame_last, this_frame);
}

void
frame_pop()
{
        rw_list_remove(&frame_first, &frame_last, frame_last);
}

void
frame_update_alloc()
{
        FRAME *this_frame = frame_last;

        if (this_frame->stack_offset > function->stack_alloc)
                function->stack_alloc = this_frame->stack_offset;
}

void
frame_free_all()
{
        obj_free_all(&frame_bkt);
        frame_first = frame_last = NULL;
}

void
frame_unwind_all()
{
        while (frame_last)
                rw_list_remove(&frame_first, &frame_last, frame_last);
        frame_push();
}


/* **************************************************************************
 * Loops
 */

void
loop_init()
{
        loop_bkt.alloc_list = NULL;
        loop_first = loop_last = NULL;
}

void
loop_free_all()
{
        obj_free_all(&loop_bkt);
        loop_first = loop_last = NULL;
}

void
loop_unwind_all()
{
        loop_first = loop_last = NULL;
}

/*ARGSUSED*/
void
loop_push(MTX *mtx)
{
        LOOP *this_loop = obj_alloc(&loop_bkt);
        rw_list_append(&loop_first, &loop_last, this_loop);
}

void
loop_pop()
{
        rw_list_remove(&loop_first, &loop_last, loop_last);
}

void
loop_fixup(JUMP_MTX *list, MTX *target)
{
        JUMP_MTX *jp;

        for (jp = list; jp; jp = (JUMP_MTX*)jp->link)
                jp->dest = target;
}


/* **************************************************************************
 * Variables
 */
OBUCKET var_bucket = { sizeof(VAR), NULL };

void
var_init()
{
        var_bucket.alloc_list = NULL;
        var_first = var_last = NULL;
}

VAR *
var_alloc(grad_data_type_t type, char *name, int grow)
{
        VAR *var;

        var = (VAR*) obj_alloc(&var_bucket);
        rw_list_append(&var_first, &var_last, var);

        /* Initialize fields */
        var->name     = name;
        var->datatype = type; 
        var->level    = curframe->level;
        var->offset   = curframe->stack_offset;
        curframe->stack_offset += grow;

        return var;
}

void
var_unwind_level()
{
        int cnt = 0;
        
        while (var_last && var_last->level == curframe->level) {
                rw_list_remove(&var_first, &var_last, var_last);
                cnt++;
        }

        if (cnt)
                frame_update_alloc();
}

void
var_unwind_all()
{
        while (var_last)
                rw_list_remove(&var_first, &var_last, var_last);
}

void
var_type(grad_data_type_t type, VAR *var)
{
        for (; var; var = var->dcllink)
                var->datatype = type;
}

void
var_free_all()
{
        obj_free_all(&var_bucket);
        var_first = var_last = NULL;
}

VAR *
var_lookup(char *name)
{
        VAR *var;

        var = var_last;
        while (var && strcmp(var->name, name))
                var = var->prev;
        return var;
}


/* **************************************************************************
 * Matrix generation
 */
OBUCKET mtx_bucket = { sizeof(MTX), NULL };
#if defined(MAINTAINER_MODE)
int mtx_current_id ;
#endif

/*
 * Insert a matrix into list
 */
#define mtx_remove(mtx) rw_list_remove(&mtx_first, &mtx_last, mtx)
#define mtx_append(mtx) rw_list_append(&mtx_first, &mtx_last, mtx)

void
mtx_insert(MTX *prev, MTX *mtx)
{
        MTX *up;

        rw_list_insert(&mtx_first, &mtx_last, prev, mtx, 0);
        if (up = prev->gen.uplink) {
                switch (up->gen.type) {
                case Unary:
                        up->un.arg = mtx;
                        break;
                case Binary:
                        if (up->bin.arg[0] == prev)
                                up->bin.arg[0] = mtx;
                        else
                                up->bin.arg[1] = mtx;
                        break;
                case Return:
                        up->ret.expr = mtx;
                        break;
                default:
                        /*should not happen*/
                        break;
                }
        }
}

void
mtx_init()
{
        mtx_bucket.alloc_list = NULL;
        mtx_first = mtx_last = NULL;
}

void
mtx_unwind_all()
{
        while (mtx_last)
                rw_list_remove(&mtx_first, &mtx_last, mtx_last);
}

void
mtx_free_all()
{
        obj_free_all(&mtx_bucket);
        mtx_first = mtx_last = NULL;
}

MTX *
mtx_cur()
{
        return mtx_last;
}

MTX *
mtx_frame(Mtxtype type, stkoff_t stksize)
{
        FRAME_MTX *mtx = (FRAME_MTX *)mtx_alloc(type);
        mtx_append(mtx);
        mtx->stacksize = stksize;
        return (MTX*)mtx;
}

MTX *
mtx_nop()
{
        MTX *mtx = mtx_alloc(Nop);
        mtx_append(mtx);
        return mtx;
}

MTX *
mtx_jump()
{
        MTX *mtx = mtx_alloc(Jump);
        mtx_append(mtx);
        return mtx;
}

MTX *
mtx_stop()
{
        MTX *mtx = mtx_alloc(Stop);
        mtx_append(mtx);
        return mtx;
}

MTX *
mtx_pop()
{
        MTX *mtx = mtx_alloc(Pop);
        mtx_append(mtx);
        return mtx;
}
        

MTX *
mtx_return(MTX *arg)
{
        MTX *mtx = mtx_alloc(Return);

        mtx_append(mtx);
        mtx->ret.expr = arg;
        arg->gen.uplink = (MTX*)mtx;
        return (MTX*)mtx;
}

/*
 * Allocate a matrix of given type and append it to the list
 */
MTX *
mtx_alloc(Mtxtype type)
{
        MTX *mtx = obj_alloc(&mtx_bucket);

        mtx->gen.type  = type;
        mtx->gen.loc   = locus;
#if defined(MAINTAINER_MODE)
        mtx->gen.id    = mtx_current_id++;
#endif
        return mtx;
}

/*
 * Create a Constant matrix
 */
MTX *
mtx_const(grad_value_t *val)
{
        CONST_MTX *mtx = (CONST_MTX *)mtx_alloc(Constant);
        
        mtx_append(mtx);
        mtx->datatype = val->type;
	mtx->datum = val->datum;
        return (MTX*)mtx;
}

/*
 * Create a Reference matrix
 */
MTX *
mtx_ref(int num)
{
        MATCHREF_MTX *mtx = (MATCHREF_MTX*)mtx_alloc(Matchref);
        mtx_append(mtx);
        mtx->datatype = String;
        mtx->num = num;
        return (MTX*)mtx;
}

MTX *
mtx_var(VAR *var)
{
        VAR_MTX *mtx = (VAR_MTX*)mtx_alloc(Variable);
        mtx_append(mtx);
        mtx->datatype = var->datatype; 
        mtx->var = var;
        return (MTX*)mtx;
}

MTX *
mtx_asgn(VAR *var, MTX *arg)
{
        ASGN_MTX *mtx = (ASGN_MTX*)mtx_alloc(Asgn);

        mtx_append(mtx);
        if (var->datatype != arg->gen.datatype)
                coerce(arg, var->datatype);
        mtx->datatype = var->datatype;
        mtx->lval = var;
        mtx->arg  = arg;
        return (MTX*)mtx;
}


grad_data_type_t
attr_datatype(grad_dict_attr_t *attr)
{
        switch (attr->type) {
        case GRAD_TYPE_STRING:
		/* FIXME: It could be a nice move to do
		   
		     (attr->prop & GRAD_AP_BINARY_STRING) ? Binstr : String;

	           instead... */  
		return String;
        case GRAD_TYPE_DATE:
                return String;
        case GRAD_TYPE_INTEGER:
        case GRAD_TYPE_IPADDR:
                return Integer;
        default:
                grad_insist_fail("unknown attribute type");
        }
        /*NOTREACHED*/
}

MTX *
mtx_attr(grad_dict_attr_t *attr, MTX *index)
{
        ATTR_MTX *mtx = (ATTR_MTX*)mtx_alloc(Attr);
        mtx_append(mtx);
        mtx->attrno   = attr->value;
        mtx->datatype = attr_datatype(attr);
	mtx->index = index;
        return (MTX*)mtx;
}

MTX *
mtx_attr_check(grad_dict_attr_t *attr,	MTX *index)
{
        ATTR_MTX *mtx = (ATTR_MTX*)mtx_alloc(Attr_check);
        mtx_append(mtx);
        mtx->attrno   = attr->value;
        mtx->datatype = Integer;
	mtx->index = index;
        return (MTX*)mtx;
}


void
rw_coercion_warning(grad_data_type_t from, grad_data_type_t to, char *pref)
{
	grad_log_loc(GRAD_LOG_WARN, &locus,
		     _("%s implicit coercion %s %s"),
		     pref ? pref : "",
		     datatype_str_abl(from),
		     datatype_str_acc(to));
}


MTX *
mtx_attr_asgn(grad_dict_attr_t *attr, MTX *index, MTX *rval)
{
        ATTR_MTX *mtx = (ATTR_MTX*)mtx_alloc(Attr_asgn);
        mtx_append(mtx);
        mtx->attrno   = attr->value;
        mtx->datatype = attr_datatype(attr);
        if (rval->gen.datatype != mtx->datatype) {
		rw_coercion_warning(rval->gen.datatype, mtx->datatype, NULL);
                rval = coerce(rval, mtx->datatype);
        }
	mtx->index = index;
        mtx->rval = rval;
        return (MTX*)mtx;
}

MTX *
mtx_attr_delete(grad_dict_attr_t *attr, MTX *index)
{
        ATTR_MTX *mtx = (ATTR_MTX*)mtx_alloc(Attr_delete);
        mtx_append(mtx);
        mtx->attrno   = attr->value;
        mtx->datatype = attr_datatype(attr);
	mtx->index = index;
        return (MTX*)mtx;
}

MTX *
mtx_bin(Bopcode opcode, MTX *arg1, MTX *arg2)
{
        BIN_MTX *mtx = (BIN_MTX*)mtx_alloc(Binary);

        mtx_append(mtx);
        if (arg1->gen.datatype != arg2->gen.datatype) {
		rw_coercion_warning(String, Integer, NULL);
                if (arg1->gen.datatype == String)
                        arg1 = coerce(arg1, Integer);
                else
                        arg2 = coerce(arg2, Integer);
        }

        switch (arg1->gen.datatype) {
        case String:
                switch (opcode) {
                case Add:
                        mtx->datatype = String;
                        break;
                case Eq:
                case Ne:
                case Lt:
                case Le:
                case Gt:
                case Ge:
                        mtx->datatype = Integer;
                        break;
                default:
                        grad_log_loc(GRAD_LOG_ERR, &locus,
				     "%s",
				     _("operation not applicable to strings"));
                        errcnt++;
                        return (MTX*)mtx;
                }
                break;
                
        case Integer:
                mtx->datatype = Integer;
		break;

	default:
		grad_insist_fail("unknown data type");
        }

        mtx->opcode = opcode;
        mtx->arg[0] = arg1;
        mtx->arg[1] = arg2;
        arg1->gen.uplink = arg2->gen.uplink = (MTX*)mtx;
        return (MTX*)mtx;
}

MTX *
mtx_un(Uopcode opcode, MTX *arg)
{
        UN_MTX *mtx = (UN_MTX*)mtx_alloc(Unary);

        mtx_append(mtx);
        if (arg->gen.datatype != Integer) {
		rw_coercion_warning(String, Integer, NULL);
                coerce(arg, Integer);
        }
        mtx->datatype = Integer;
        mtx->opcode = opcode;
        mtx->arg = arg;
        arg->gen.uplink = (MTX*)mtx;
        return (MTX*)mtx;
}

MTX *
mtx_match(int negated, MTX *arg, COMP_REGEX *rx)
{
        MATCH_MTX *mtx = (MATCH_MTX*)mtx_alloc(Match);

        mtx_append(mtx);
        if (arg->gen.datatype != String) {
		rw_coercion_warning(Integer, String, NULL);
                coerce(arg, String);
        }
        mtx->datatype = Integer;
        mtx->negated = negated;
        mtx->arg = arg;
        mtx->rx  = rx;
        return (MTX*)mtx;
}

MTX *
mtx_cond(MTX *cond, MTX *if_true, MTX *if_false)
{
        COND_MTX *mtx = (COND_MTX*)mtx_alloc(Cond);

        mtx_append(mtx);
        mtx->expr = cond;
        mtx->if_true   = if_true;
        mtx->if_false  = if_false;
        return (MTX*)mtx;
}

MTX *
mtx_coerce(grad_data_type_t type, MTX *arg)
{
        if (type == arg->gen.datatype)
                return mtx_cur();
        return coerce(arg, type);
}       

MTX *
coerce(MTX *arg, grad_data_type_t type)
{
        COERCE_MTX *mtx = (COERCE_MTX*)mtx_alloc(Coercion);

        mtx_insert(arg, (MTX*) mtx);
        mtx->datatype = type;
        mtx->arg = arg;
        return (MTX*)mtx;
}

MTX *
mtx_call(FUNCTION *fun, MTX *args)
{
        MTX       *argp;
        CALL_MTX  *call;
        PARAMETER *parmp;
        int       argn;
        
        /*
         * Test the number and types of arguments. Insert reasonable
         * typecasts.
         */
        argn = 0;
        argp = args;
        parmp = fun->parm;
        while (argp && parmp) {
                if (argp->gen.datatype != parmp->datatype) {
			char buf[24];
			snprintf(buf, sizeof buf, _("(argument %d)"), argn);
			rw_coercion_warning(argp->gen.datatype,
					    parmp->datatype, buf);
                        coerce(argp, parmp->datatype);
                }
                argn++;
                argp  = argp->gen.arglink;
                parmp = parmp->next;
        }

        /*
         * Note that the argument count mismatch is not an error!
         */
        if (argp) {
                grad_log_loc(GRAD_LOG_ERR, &locus,
			     _("too many arguments in call to %s"),
			     fun->name);
		errcnt++;
        } else if (parmp) {
                grad_log_loc(GRAD_LOG_ERR, &locus,
			     _("too few arguments in call to %s"),
			     fun->name);
		errcnt++;
        }

        call = (CALL_MTX*) mtx_alloc(Call);
        mtx_append((MTX*)call);
        
        call->datatype = fun->rettype;
        call->fun  = fun;
        call->args = args; 
        call->nargs = argn;
        
        return (MTX*) call;
}

MTX *
mtx_builtin(builtin_t *bin, MTX *args)
{
        MTX          *argp;
        BTIN_MTX     *call;
        int          argn;
        char         *parmp;
        grad_data_type_t     type;
        /*
         * Test the number and types of arguments. Insert reasonable
         * typecasts.
         */
        argn = 0;
        argp = args;
        parmp = bin->parms;
        
        while (argp && parmp) {
                switch (parmp[0]) {
                case 'i':
                        type = Integer;
                        break;
                case 's':
                        type = String;
                        break;
                default:
                        grad_insist_fail("malformed builtin");
                }

                if (argp->gen.datatype != type) {
			char buf[24];
			snprintf(buf, sizeof buf, _("(argument %d)"), argn);
			rw_coercion_warning(argp->gen.datatype, type, buf);
                        coerce(argp, type);
                }
                argn++;
                argp  = argp->gen.arglink;
                parmp++;
        }

        if (argp) {
                grad_log_loc(GRAD_LOG_ERR, &locus,
			     _("too many arguments in call to %s"),
			     bin->name);
                errcnt++;
        } else if (*parmp) {
                grad_log_loc(GRAD_LOG_ERR, &locus,
			     _("too few arguments in call to %s"),
			     bin->name);
                errcnt++;
        }

        call = (BTIN_MTX*) mtx_alloc(Builtin);
        mtx_append((MTX*)call);
        
        call->datatype = bin->rettype;
        call->fun  = bin->handler;
        call->args = args; 
        call->nargs = argn;
        
        return (MTX*) call;
}


/* ****************************************************************************
 * Code optimizer (rudimentary)
 */

const char *
datatype_str_nom(grad_data_type_t type)
{
        switch (type) {
        case Undefined:
                return _("Undefined");
        case Integer:
                return _("integer");
        case String:
                return _("string");
        default:
                return _("UNKNOWN");
        }
}

const char *
datatype_str_abl(grad_data_type_t type)
{
        switch (type) {
        case Undefined:
                return _("from Undefined");
        case Integer:
                return _("from integer");
        case String:
                return _("from string");
        default:
                return _("from UNKNOWN");
        }
}

const char *
datatype_str_acc(grad_data_type_t type)
{
        switch (type) {
        case Undefined:
                return _("to Undefined");
        case Integer:
                return _("to integer");
        case String:
                return _("to string");
        default:
                return _("to UNKNOWN");
        }
}

FILE *
debug_open_file()
{
        FILE *fp;
        char *path;
        
        path = grad_mkfilename(grad_log_dir, "radius.mtx");
        if ((fp = fopen(path, "a")) == NULL) {
                grad_log(GRAD_LOG_ERR|GRAD_LOG_PERROR,
                         _("can't open file `%s'"),
                         path);
        }
        grad_free(path);
        return fp;
}

#if defined(MAINTAINER_MODE)

static void debug_print_datum(FILE *fp, grad_data_type_t type,  grad_datum_t *datum);
static void debug_print_var(FILE *fp, VAR *var);
static void debug_print_unary(FILE *fp, UN_MTX *mtx);
static void debug_print_binary(FILE *fp, BIN_MTX *mtx);
static void debug_print_mtxlist();

static char *b_opstr[] = {
        "Eq",
        "Ne",
        "Lt",
        "Le",
        "Gt",
        "Ge",
        "&",
        "^",
        "|",
        "And",
        "Or",
        "Shl",
        "Shr",
        "Add",
        "Sub",
        "Mul",
        "Div",
        "Rem",
};

static char *u_opstr[] = {
        "Neg",
        "Not"
};

#define LINK(m) (m ? m->gen.id : 0)

void
debug_print_datum(FILE *fp, grad_data_type_t type, grad_datum_t *datum)
{
        fprintf(fp, "%3.3s ", datatype_str_nom(type));
        switch (type) {
        case Integer:
                fprintf(fp, "%d", datum->ival);
                break;
		
        case String:
                fprintf(fp, "%s", datum->sval);
		break;
		
	default:
		grad_insist_fail("unknown data type");
        }
}

void
debug_print_var(FILE *fp, VAR *var)
{
        fprintf(fp, "%3.3s %s L:%d S:%d",
                datatype_str_nom(var->datatype),
                var->name,
                var->level,
                var->offset);
        if (var->constant) {
                fprintf(fp, "CONST ");
                debug_print_datum(fp, var->datatype, &var->datum);
        }
}

void
debug_print_unary(FILE *fp, UN_MTX *mtx)
{
        fprintf(fp, "OP:%s M:%d",
                u_opstr[mtx->opcode], LINK(mtx->arg));
}

void
debug_print_binary(FILE *fp, BIN_MTX *mtx)
{
        fprintf(fp, "OP:%s M1:%d M2:%d",
                b_opstr[mtx->opcode],
                LINK(mtx->arg[0]),
                LINK(mtx->arg[1]));
}


void
debug_print_mtxlist(char *s)
{
        FILE *fp;
        MTX  *mtx, *tmp;
        
        if ((fp = debug_open_file()) == NULL) 
                return;

        #define CASE(c) case c: fprintf(fp, "%-10.10s", #c);

        fprintf(fp, "%s\n", s);
        for (mtx = mtx_first; mtx; mtx = mtx->gen.next) {
                fprintf(fp, "%4d: %4d %4d ",
                        mtx->gen.id,
                        LINK(mtx->gen.prev),
                        LINK(mtx->gen.next));
                switch (mtx->gen.type) {
                CASE (Generic)
                        break;
                CASE (Nop)
                        break;
                CASE (Enter)
                        fprintf(fp, "%3.3s %d",
                                "",
                                mtx->frame.stacksize);
                        break;
                CASE (Leave)
                        fprintf(fp, "%3.3s %d",
                                "",
                                mtx->frame.stacksize);
                        break;
                CASE (Stop)
                        break;
                CASE (Constant)
                        debug_print_datum(fp, mtx->cnst.datatype,
                                          &mtx->cnst.datum);
                        break;
                CASE (Matchref)
                        fprintf(fp, "%3.3s %d",
                                datatype_str_nom(String),
                                mtx->ref.num);
                        break;
                CASE (Variable)
                        debug_print_var(fp, mtx->var.var);
                        break;
                CASE (Unary)
                        fprintf(fp, "%3.3s ", datatype_str_nom(mtx->gen.datatype));
                        debug_print_unary(fp, &mtx->un);
                        break;
                CASE (Binary)
                        fprintf(fp, "%3.3s ", datatype_str_nom(mtx->gen.datatype));
                        debug_print_binary(fp, &mtx->bin);
                        break;
                CASE (Cond)
                        fprintf(fp, "%3.3s ", "");
                        fprintf(fp, "C:%4d T:%4d F:%4d",
                                LINK(mtx->cond.expr),
                                LINK(mtx->cond.if_true),
                                LINK(mtx->cond.if_false));
                        break;
                CASE (Asgn)
                        fprintf(fp, "%3.3s ",
                                datatype_str_nom(mtx->gen.datatype));
                        fprintf(fp, "V:%s,%d,%d M:%4d",
                                mtx->asgn.lval->name,
                                mtx->asgn.lval->level,
                                mtx->asgn.lval->offset,
                                LINK(mtx->asgn.arg));
                                break;
                CASE (Match)
                        fprintf(fp, "    N:%1d M:%4d RX:%p",
                                mtx->match.negated,
                                LINK(mtx->match.arg),
                                mtx->match.rx);
                        break; 
                CASE (Coercion)
                        fprintf(fp, "%3.3s M:%4d",
                                datatype_str_nom(mtx->coerce.datatype),
                                LINK(mtx->coerce.arg));
                        break;
                CASE (Return)
                        fprintf(fp, "%3.3s M:%4d",
                                datatype_str_nom(mtx->ret.expr->gen.datatype),
                                LINK(mtx->ret.expr));
                        break;
                CASE (Jump)
                        fprintf(fp, "%3.3s M:%4d",
                                "",
                                LINK(mtx->jump.dest));
                        break;
                CASE (Branch)
                        fprintf(fp, "%3.3s M:%4d",
                                mtx->branch.cond ? "NE" : "EQ",
                                LINK(mtx->branch.dest));
                        break;
                CASE (Call)
                        fprintf(fp, "%3.3s F:%s, A:%d:",
                                datatype_str_nom(mtx->call.datatype),
                                mtx->call.fun->name,
                                mtx->call.fun->nparm);
                        for (tmp = mtx->call.args; tmp; tmp = tmp->gen.arglink)
                                fprintf(fp, "%d,", tmp->gen.id);
                        break;

                CASE(Builtin)
                        fprintf(fp, "%3.3s F:%p, A:%d:",
                                datatype_str_nom(mtx->btin.datatype),
                                mtx->btin.fun,
                                mtx->btin.nargs);
                        for (tmp = mtx->btin.args; tmp; tmp = tmp->gen.arglink)
                                fprintf(fp, "%d,", tmp->gen.id);
                        break;

                CASE (Pop)
                        break;

                CASE (Pusha)
                        break;

                CASE (Popa)
                        break;
                
                CASE (Attr)
                        fprintf(fp, "%3.3s A:%d I:%d",
                                datatype_str_nom(mtx->gen.datatype),
                                mtx->attr.attrno,
				mtx->attr.index ? mtx->attr.index->gen.id : 0);
                        break;

                CASE (Attr_check)
                        fprintf(fp, "%3.3s A:%d I:%d",
                                datatype_str_nom(mtx->gen.datatype),
                                mtx->attr.attrno,
				mtx->attr.index ? mtx->attr.index->gen.id : 0);
                        break;
                        
                CASE (Attr_asgn)
                        fprintf(fp, "%3.3s A:%d I:%d M:%d",
                                datatype_str_nom(mtx->gen.datatype),
                                mtx->attr.attrno,
				mtx->attr.index ? mtx->attr.index->gen.id : 0,
				LINK(mtx->attr.rval));
                        break;
                        
		CASE (Attr_delete)
			fprintf(fp, "%3.3s A:%d I:%d",
				datatype_str_nom(mtx->gen.datatype),
				mtx->attr.attrno,
				mtx->attr.index ? mtx->attr.index->gen.id : 0);
		        break;
 
                default:
                        fprintf(fp, "UNKNOWN: %d", mtx->gen.type);
                }
                fprintf(fp, "\n");
        }                       
        
        fclose(fp);
}

void
debug_print_function()
{
        FILE      *fp;
        PARAMETER *parm;
        int        n;
        
        if ((fp = debug_open_file()) == NULL) 
                return;

        fprintf(fp, "FUNCTION: %s\n", function->name);
        fprintf(fp, "RETURNS : %s\n", datatype_str_nom(function->rettype));
        fprintf(fp, "NPARMS  : %d\n", function->nparm);
        fprintf(fp, "PARMS   :\n");

        for (parm = function->parm, n = 0; parm; parm = parm->next, n++) 
                fprintf(fp, "    %4d: %s at %4d\n",
                        n, datatype_str_nom(parm->datatype),
                        parm->offset);
        
        fclose(fp);
}

#endif /* MAINTAINER_MODE */
        
#if defined(MAINTAINER_MODE)
# define DEBUG_MTX(c) if (GRAD_DEBUG_LEVEL(30)) debug_print_mtxlist(c);
# define DEBUG_FUN()  if (GRAD_DEBUG_LEVEL(25)) debug_print_function();
#else
# define DEBUG_MTX(c) 
# define DEBUG_FUN()
#endif

static void pass1();
static int pass2_unary(MTX *mtx);
static int pass2_binary(MTX *mtx);

void
pass1()
{
        MTX *mtx;
        MTX *end;
        
        /*
         * Create an entry matrix
         */
        mtx = mtx_alloc(Enter);
        rw_list_insert(&mtx_first, &mtx_last, mtx_first, mtx, 1);
        mtx->frame.stacksize = function->stack_alloc;
        
        /*
         * Provide a default return statement if necessary
         */
        if (mtx_last->gen.type != Return) {
                grad_value_t val;
                grad_log_loc(GRAD_LOG_WARN, &mtx_last->gen.loc,
			     _("missing return statement"));

		val.type = function->rettype;
                switch (function->rettype) {
                case Integer:
                        val.datum.ival = 0;
                        break;
			
                case String:
                        val.datum.sval.data = "";
			val.datum.sval.size = 0;
			break;

		default:
			grad_insist_fail("Unknown data type");
                }
                mtx_const(&val);
                mtx_frame(Leave, function->stack_alloc);
        } else {
                mtx_last->gen.type = Leave;
                mtx_last->frame.stacksize = function->stack_alloc;
        }

        /*
         * Insert a no-op matrix before the `leave' one
         */
        end = mtx_alloc(Nop);
        rw_list_insert(&mtx_first, &mtx_last, mtx_last, end, 1);
        
        for (mtx = mtx_first; mtx; mtx = mtx->gen.next) {
                if (mtx->gen.type == Return) {
                        if (mtx->ret.expr->gen.datatype != function->rettype) {
				rw_coercion_warning(
					mtx->ret.expr->gen.datatype,
					function->rettype, NULL);
                                coerce(mtx->ret.expr, function->rettype);
                        }
                        mtx->gen.type = Jump;
                        mtx->jump.dest = end;
                }
        }
}
        
/*
 * Second pass: elimination of constant sub-expressions
 */

/*
 * Perform immediate unary calculations
 */
int
pass2_unary(MTX *mtx)
{
        MTX *arg = mtx->un.arg;
        
        switch (mtx->un.opcode) {
        case Not:
                arg->cnst.datum.ival = !arg->cnst.datum.ival;
                break;
		
        case Neg:
                arg->cnst.datum.ival = -arg->cnst.datum.ival;
                break;

	default:
		grad_insist_fail("Unexpected opcode");
        }
        mtx->gen.type = Constant;
        mtx->cnst.datum = arg->cnst.datum;
        mtx_remove(arg);
        return 0;
}

/*
 * Perform immediate binary computations
 */
int
pass2_binary(MTX *mtx)
{
        MTX *arg0 = mtx->bin.arg[0];
        MTX *arg1 = mtx->bin.arg[1];
        grad_datum_t dat;
        
        switch (mtx->bin.opcode) {
        case Eq:
                dat.ival = arg0->cnst.datum.ival == arg1->cnst.datum.ival;
                break;
		
        case Ne:
                dat.ival = arg0->cnst.datum.ival != arg1->cnst.datum.ival;
                break;
		
        case Lt:
                dat.ival = arg0->cnst.datum.ival < arg1->cnst.datum.ival;
                break;
		
        case Le:
                dat.ival = arg0->cnst.datum.ival <= arg1->cnst.datum.ival;
                break;
		
        case Gt:
                dat.ival = arg0->cnst.datum.ival > arg1->cnst.datum.ival;
                break;
		
        case Ge:
                dat.ival = arg0->cnst.datum.ival >= arg1->cnst.datum.ival;
                break;
		
        case BAnd:
                dat.ival = arg0->cnst.datum.ival & arg1->cnst.datum.ival;
                break;
		
        case BOr:
                dat.ival = arg0->cnst.datum.ival | arg1->cnst.datum.ival;
                break;
		
        case BXor:
                dat.ival = arg0->cnst.datum.ival ^ arg1->cnst.datum.ival;
                break;
		
        case And:
                dat.ival = arg0->cnst.datum.ival && arg1->cnst.datum.ival;
                break;
		
        case Or:
                dat.ival = arg0->cnst.datum.ival || arg1->cnst.datum.ival;
                break;
		
        case Shl:
                dat.ival = arg0->cnst.datum.ival << arg1->cnst.datum.ival;
                break;
		
        case Shr:
                dat.ival = arg0->cnst.datum.ival >> arg1->cnst.datum.ival;
                break;
		
        case Add:
                dat.ival = arg0->cnst.datum.ival + arg1->cnst.datum.ival;
                break;
		
        case Sub:
                dat.ival = arg0->cnst.datum.ival - arg1->cnst.datum.ival;
                break;
		
        case Mul:
                dat.ival = arg0->cnst.datum.ival * arg1->cnst.datum.ival;
                break;
		
        case Div:
                if (arg1->cnst.datum.ival == 0) {
                        grad_log_loc(GRAD_LOG_ERR, &arg1->cnst.loc,
				     _("divide by zero"));
                        errcnt++;
                } else
                        dat.ival =
                                arg0->cnst.datum.ival / arg1->cnst.datum.ival;
                break;
		
        case Rem:
                if (arg1->cnst.datum.ival == 0) {
                        grad_log_loc(GRAD_LOG_ERR, &arg1->cnst.loc,
				     _("divide by zero"));
                        errcnt++;
                } else
                        dat.ival =
                                arg0->cnst.datum.ival % arg1->cnst.datum.ival;
                break;

	default:
		grad_insist_fail("Unexpected opcode");
        }
        mtx->gen.type = Constant;
        mtx->cnst.datum = dat;
        mtx_remove(arg0);
        mtx_remove(arg1);
        return 0;
}

MTX *
mtx_branch(int cond, MTX *target)
{
        MTX *nop = mtx_alloc(Nop);
        MTX *mtx = mtx_alloc(Branch);
        mtx_insert(target, nop);
        mtx->branch.cond = cond;
        mtx->branch.dest = nop;
        return mtx;
}

void
mtx_bool(MTX *mtx)
{
        MTX *j_mtx, *p, *p1;

        /* Insert after first operand:
	   popa
	   pusha
	   pusha      ;; Duplicate tos value
	   j?e   L10
	   popa       ;; Pop up the unneded value */

	p = mtx_alloc(Popa);
	mtx_insert(mtx->bin.arg[0], p);
	p1 = mtx_alloc(Pusha);
	mtx_insert(p, p1);
	p = mtx_alloc(Pusha);
	mtx_insert(p1, p);
        j_mtx = mtx_branch(mtx->bin.opcode == Or, mtx);
        mtx_insert(p, j_mtx);
	p1 = mtx_alloc(Popa);
	mtx_insert(j_mtx, p1);
        /* Remove the op matrix
	   Note that the mtx->cond.expr is not correct after this
	   operation, but this does not affect the functionality */
        mtx_remove(mtx);
}

/*
 * Second optimization pass: immediate computations
 */
int
pass2()
{
        MTX *mtx, *next;
        int optcnt;
        int errcnt = 0;
        
        do {
                optcnt = 0;
                mtx = mtx_first;
                while (mtx) {
                        next = mtx->gen.next;
                        switch (mtx->gen.type) {
                        case Unary:
                                if (mtx->un.arg->gen.type != Constant)
                                        break;
                                if (pass2_unary(mtx))
                                        errcnt++;
                                else
                                        optcnt++;
                                break;
                        
                        case Binary:
                                if (mtx->bin.arg[0]->gen.type == Constant
				    && mtx->bin.arg[1]->gen.type == Constant) {
                                        switch (mtx->bin.datatype) {
                                        case Integer:
                                                if (pass2_binary(mtx))
                                                        errcnt++;
                                                else
                                                        optcnt++;
                                                break;
						
                                        case String:
                                                /*NO STRING OPS SO FAR */;
					        break;

					default:
						grad_insist_fail("Unknown data type");
                                        }
                                } else if (mtx->bin.opcode == And
					   || mtx->bin.opcode == Or) {
                                        mtx_bool(mtx);
                                }
                                break;
                                /*FIXME: ADD `if (1)'/`if 0' evaluation */
                        case Jump:
                                if (mtx->jump.dest == mtx->jump.next)
                                        mtx_remove(mtx);
				break;

			case Attr:
			case Attr_asgn:
			case Attr_check:
			case Attr_delete:
				/*FIXME: the rw_attr.0 functions should
				  expect an immediate value after the
				  attribute number */
				break;

			default:
				break;
                        }
                        mtx = next;
                }
        } while (errcnt == 0 && optcnt > 0);
        return errcnt;
}

int
optimize()
{
        DEBUG_FUN();
        DEBUG_MTX("on entry to optimize");
        pass1();
        DEBUG_MTX("after first pass");
        if (pass2())
                return -1;
        DEBUG_MTX("after second pass (immediate computations)");
        return 0;
}       


/* ****************************************************************************
 * Code generator
 */


static INSTR *rw_code;          /* Code segment */
static pctr_t rw_pc;            /* PC when compiling the code */
static size_t rw_codesize;      /* Length of code segment */ 

void
code_check()
{
        if (rw_code == NULL) {
                rw_codesize  = 4096;
                rw_code  = grad_emalloc(rw_codesize * sizeof(rw_code[0]));
        }
}

void
code_init()
{
	code_check();
        /* code cell #0 is the default return address */
	rw_code[0] = 0;
	rw_pc = 1;
}

#if defined(MAINTAINER_MODE)
void
debug_dump_code()
{
        FILE    *fp;
        pctr_t  pc;
        int     i;
        
        if ((fp = debug_open_file()) == NULL)
                return;
        fprintf(fp, "Code size: %d\n", rw_codesize);
        fprintf(fp, "Code dump:\n");

        pc = 0;
        do {
                fprintf(fp, "%4d:", pc);
                for (i = 0; i < 8 && pc < rw_codesize; i++, pc++)
                        fprintf(fp, " %8x", (u_int) rw_code[pc]);
                fprintf(fp, "\n");
        } while (pc < rw_codesize);
        
        fclose(fp);
}
#endif
/*
 * Runtime function prototypes
 */
static int pushn(RWSTYPE n);
static int cpopn(RWSTYPE *np);
static RWSTYPE popn();
static void checkpop(int cnt);
static void pushref(char *str, int from, int to);
static RWSTYPE *heap_reserve(int size);
static void pushs(RWSTYPE *sptr, size_t size, int len);
static void pushstr(const char *str, int len);

static void rw_pushn();
static void rw_pushs();
static void rw_pushref();
static void rw_pushv();
static void rw_i2s();
static void rw_s2i();
static void rw_eq();
static void rw_ne();
static void rw_lt();
static void rw_le();
static void rw_gt();
static void rw_ge();
static void rw_eqs();
static void rw_nes();
static void rw_lts();
static void rw_les();
static void rw_gts();
static void rw_ges();
static void rw_b_xor();
static void rw_b_and();
static void rw_b_or();
static void rw_shl();
static void rw_shr();
static void rw_add();
static void rw_sub();
static void rw_mul();
static void rw_div();
static void rw_rem();
static void rw_not();
static void rw_neg();
static void rw_asgn();
static void rw_enter();
static void rw_leave();
static void rw_match();
static void rw_jmp();
static void rw_jne();
static void rw_je();
static void rw_adds();
static void rw_adjstk();
static void rw_popn();
static void rw_pusha();
static void rw_popa();
static void rw_call();
static void rw_builtin();
static void rw_attrs();
static void rw_attrs0();
static void rw_attrn();
static void rw_attrn0();
static void rw_attrcheck();
static void rw_attrcheck0();
static void rw_attrasgn();
static void rw_attrasgn0();
static void rw_attr_delete();
static void rw_attr_delete0();

INSTR bin_codetab[] = {
        rw_eq,              
        rw_ne,
        rw_lt,
        rw_le,
        rw_gt,
        rw_ge,
        rw_b_and,
        rw_b_xor,
        rw_b_or,
        NULL,
        NULL,
        rw_shl,
        rw_shr,
        rw_add,
        rw_sub,
        rw_mul,
        rw_div,
        rw_rem,
};

INSTR bin_string_codetab[] = {
        rw_eqs,                      
        rw_nes,                      
        rw_lts,                      
        rw_les,                      
        rw_gts,                      
        rw_ges,                      
        NULL,                        
        NULL,                        
        NULL,                        
        NULL,                        
        NULL,                        
        NULL,                        
        NULL,                        
        rw_adds,                     
        NULL,                        
        NULL,                        
        NULL,                        
        NULL                         
};                                   

INSTR coerce_tab[Max_datatype][Max_datatype] = {
/*                Undefined  Integer  String */
/* Undefined */ {  NULL,      NULL,    NULL   },
/* Integer */   {  NULL,      NULL,    rw_i2s },  
/* String */    {  NULL,      rw_s2i,  NULL   },
};

static void check_codesize(int delta);
static int  code(INSTR instr);
static int  data(int val);
static int data_str(char *ptr);
static void add_target(NOP_MTX *mtx, pctr_t pc);


void
add_target(NOP_MTX *mtx, pctr_t pc)
{
        TGT_MTX *tgt = (TGT_MTX *)mtx_alloc(Target);
        tgt->next = (MTX*)mtx->tgt;
        mtx->tgt = tgt;
        tgt->pc = pc;
}

void
fixup_target(NOP_MTX *mtx, pctr_t pc)
{
        TGT_MTX   *tgt;
        
        for (tgt = (TGT_MTX*)mtx->tgt; tgt; tgt = (TGT_MTX*)tgt->next) 
                rw_code[tgt->pc] = (INSTR)pc;
        mtx->tgt = NULL;
}

pctr_t
codegen()
{
        MTX       *mtx;

        function->entry = rw_pc;
        for (mtx = mtx_first; mtx; mtx = mtx->gen.next) {
                switch (mtx->gen.type) {
                case Generic:
                case Return:
                default:
                        grad_log(GRAD_LOG_CRIT,
                                 "INTERNAL ERROR: codegen stumbled accross generic matrix!");
                        errcnt++;
                        return 0;
                case Nop:
                        /* Fix-up the references */
                        fixup_target(&mtx->nop, rw_pc);
                        mtx->nop.pc = rw_pc;
                        break;
                case Stop:
                        break;
                case Enter:
                        code(rw_enter);
                        data(mtx->frame.stacksize);
                        break;
                case Leave:
                        code(rw_leave);
                        break;
                case Constant:
                        switch (mtx->cnst.datatype) {
                        case Integer:
                                code(rw_pushn);
                                data(mtx->cnst.datum.ival);
                                break;
				
                        case String:
                                code(rw_pushs);
                                data_str(mtx->cnst.datum.sval.data);
                                break;

			default:
				grad_insist_fail("Unknown data type");
                        }
                        break;
                case Matchref:
                        code(rw_pushref);
                        data(mtx->ref.num);
                        break;
                case Variable:
                        /* Variable dereference.
                         */
                        code(rw_pushv);
                        data(mtx->var.var->offset);
                        break;
                case Unary:
                        switch (mtx->un.opcode) {
                        case Not:
                                code(rw_not);
                                break;
				
                        case Neg:
                                code(rw_neg);
                                break;

			default:
				grad_insist_fail("Unexpected opcode");
                        }
                        break;
                case Binary:
                        if (mtx->bin.arg[0]->gen.datatype == String)
                                code(bin_string_codetab[mtx->bin.opcode]);
                        else
                                code(bin_codetab[mtx->bin.opcode]);
                        break;
                case Cond:
                        /*FIXME: this needs optimization */
                        code(rw_jne);
                        add_target(&mtx->cond.if_true->nop, rw_pc);
                        code(NULL);
                        if (mtx->cond.if_false) {
                                code(rw_jmp);
                                add_target(&mtx->cond.if_false->nop, rw_pc);
                                code(NULL);
                        }
                        break;
                        
                case Asgn:
                        code(rw_asgn);
                        data(mtx->asgn.lval->offset);
                        break;
                        
                case Match:
                        code(rw_match);
                        code((INSTR)mtx->match.rx);
                        if (mtx->match.negated)
                                code(rw_not);
                        break;
                        
                case Coercion:
                        code(coerce_tab[mtx->coerce.arg->gen.datatype][mtx->coerce.datatype]);
                        break;
                        
                case Jump:
                        code(rw_jmp);
                        add_target(&mtx->jump.dest->nop, rw_pc);
                        code(NULL);
                        break;

                case Branch:
                        code(mtx->branch.cond ? rw_jne : rw_je);
                        add_target(&mtx->branch.dest->nop, rw_pc);
                        code(NULL);
                        break;
                        
                case Call:
                        code(rw_call);
                        code((INSTR) mtx->call.fun->entry);
                        code(rw_adjstk);
                        data(mtx->call.nargs);
                        break;

                case Builtin:
                        code(rw_builtin);
                        code(mtx->btin.fun);
                        code(rw_adjstk);
                        data(mtx->btin.nargs);
                        break;

                case Pop:
                        code(rw_popn);
                        break;

                case Popa:
                        code(rw_popa);
                        break;
                        
                case Pusha:
                        code(rw_pusha);
                        break;
                        
                case Attr:
                        switch (mtx->attr.datatype) {
                        case Integer:
				if (mtx->attr.index)
					code(rw_attrn);
				else
					code(rw_attrn0);
                                break;
				
                        case String:
				if (mtx->attr.index)
					code(rw_attrs);
				else
					code(rw_attrs0);
                                break;

			default:
				grad_insist_fail("Unknown data type");
                        }
                        data(mtx->attr.attrno);
                        break;

                case Attr_check:
			if (mtx->attr.index) 
				code(rw_attrcheck);
			else
				code(rw_attrcheck0);
                        data(mtx->attr.attrno);
                        break;
                        
                case Attr_asgn:
			if (mtx->attr.index)
				code(rw_attrasgn);
			else
				code(rw_attrasgn0);
                        data(mtx->attr.attrno);
                        break;
                                
		case Attr_delete:
			if (mtx->attr.index)
				code(rw_attr_delete);
			else
				code(rw_attr_delete0);
			data(mtx->attr.attrno);
			break;
                }
        }

        /*
         * Second pass: fixup backward references
         */
        for (mtx = mtx_first; mtx; mtx = mtx->gen.next) {
                if (mtx->gen.type == Nop)
                        fixup_target(&mtx->nop, mtx->nop.pc);
        }
        
#if defined(MAINTAINER_MODE)    
        if (GRAD_DEBUG_LEVEL(25)) {
                FILE *fp = debug_open_file();
                fprintf(fp, "entry: %d, size %d\n",
                        function->entry, rw_pc - function->entry);
                fclose(fp);
        }
#endif  
        return function->entry;
}

void
check_codesize(int delta)
{
        if (rw_pc + delta >= rw_codesize) {
                INSTR *p = grad_emalloc((rw_codesize + 4096) * sizeof(rw_code[0]));
                memcpy(p, rw_code, rw_codesize * sizeof(rw_code[0]));
                grad_free(rw_code);
                rw_code = p;
                rw_codesize += 4096;
        }
}

int
code(INSTR instr)
{
        check_codesize(1);
        rw_code[rw_pc] = instr;
        return rw_pc++;
}

int
data(int val)
{
        return code((INSTR)(RWSTYPE)val);
}

int
data_str(char *ptr)
{
        int  len   = strlen(ptr) + 1;
        RWSTYPE delta = (len + sizeof(rw_code[0])) / sizeof(rw_code[0]);
        
        check_codesize(delta+1);
        rw_code[rw_pc++] = (INSTR)delta;
        memcpy(rw_code + rw_pc, ptr, len);
        rw_pc += delta;
        return rw_pc;
}
        

/* ****************************************************************************
 * Regular expressions
 */

COMP_REGEX *
rx_alloc(regex_t *regex, int nmatch)
{
        COMP_REGEX *rx;

        rx = grad_emalloc(sizeof(*rx));
        rx->regex  = *regex;
        rx->nmatch = nmatch;
        rw_list_insert(&function->rx_list, NULL, function->rx_list, rx, 1);
        return rx;
}

void
rx_free(COMP_REGEX *rx)
{
        COMP_REGEX *next;

        while (rx) {
                next = rx->next;
                regfree(&rx->regex);
                grad_free(rx);
                rx = next;
        }
}

COMP_REGEX *
compile_regexp(char *str)
{
        char     *p;
        regex_t  regex;
        int      nmatch;
        
        int rc = regcomp(&regex, str, regcomp_flags);
        if (rc) {
                char errbuf[512];
                regerror(rc, &regex, errbuf, sizeof(errbuf));
                grad_log_loc(GRAD_LOG_ERR, &locus,
			     _("regexp error: %s"),
			     errbuf);
                return NULL;
        }
        /* count the number of matches */
        nmatch = 0;
        for (p = str; *p; ) {
                if (*p == '\\')
                        if (p[1] == '(') {
                                nmatch++;
                                p += 2;
                                continue;
                        }
                p++;
        }
        
        return rx_alloc(&regex, nmatch);
}

void
function_delete()
{
        if (function) {
                grad_symtab_delete(rewrite_tab, (grad_symbol_t*)function);
                function_cleanup();
        }
}

void
function_cleanup()
{
        function = NULL;
}


/* ****************************************************************************
 * Runtime functions
 */

/*
 * Push a number on stack
 */
int
pushn(RWSTYPE n)
{
        if (mach.st >= mach.ht) {
                /*FIXME: gc();*/
                GRAD_DEBUG2(1, "st=%d, ht=%d", mach.st, mach.ht);
                rw_error(_("out of pushdown space"));
        }
        mach.stack[mach.st++] = n;
        return 0;
}

/*
 * Push a string on stack
 */
void
pushs(RWSTYPE *sptr, size_t size, int len)
{
	if (mach.ht - len - 1 <= mach.st) {
                /* Heap overrun: */
                /*gc(); */
                rw_error(_("heap overrun"));
        }

        while (len)
                mach.stack[mach.ht--] = sptr[--len];
	mach.stack[mach.ht--] = size;
        pushn((RWSTYPE) (mach.stack + mach.ht + 1));
}

void
pushstr(const char *str, int len)
{
        RWSTYPE *p = heap_reserve(sizeof(RWSTYPE) + len + 1);
	char *s = (char*)(p + 1);
        memcpy(s, str, len);
        s[len] = 0;
	p[0] = len;
        pushn((RWSTYPE)p);
}

#define B2RW(s) (s + sizeof(mach.stack[0]) - 1) / sizeof(mach.stack[0])

RWSTYPE *
heap_reserve(int size)
{
	size_t words = B2RW(size);

        if (mach.ht - words <= mach.st) {
                /* Heap overrun: */
                gc();
                if (mach.ht - words <= mach.st) 
                        rw_error(_("heap overrun"));
        }
        mach.ht -= words;
        return mach.stack + mach.ht--;
}


/* Temporary space functions */
char *
temp_space_create()
{
	return (char*)(mach.stack + mach.st);
}

size_t
temp_space_size()
{
	return (mach.ht - mach.st)*sizeof(mach.stack[0]);
}

void
temp_space_copy(char **baseptr, char *text, size_t size)
{
        size_t len = (size + sizeof(mach.stack[0])) / sizeof(mach.stack[0]);
	if (*baseptr + len >= (char*)(mach.stack + mach.ht))
		rw_error(_("out of heap space"));
	memcpy(*baseptr, text, size);
	*baseptr += size;
}

RWSTYPE *
temp_space_fix(char *end)
{
	size_t len, size;
	char *base = (char*)(mach.stack + mach.st);

	temp_space_copy(&end, "", 0);
	size = end - base;
	len = B2RW(size);
        mach.ht -= len;
	memmove(mach.stack + mach.ht, base, size);
	mach.stack[--mach.ht] = strlen(base);
        return mach.stack + mach.ht--;
}


/*
 * Pop number from stack and store into NP.
 */
int
cpopn(RWSTYPE *np)
{
        if (mach.st <= 0) {
                rw_error(_("out of popup"));
        }
        *np = mach.stack[--mach.st];
        return 0;
}

/*
 * Pop the number from stack without error checking. checkpop() function
 * should be called before calling this one.
 */
RWSTYPE
popn()
{
        return mach.stack[--mach.st];
}

void
mem2string(grad_string_t *p, RWSTYPE *loc)
{
	p->size = loc[0];
	p->data = (unsigned char*) (loc + 1);
}

void
poparr(grad_string_t *p)
{
	mem2string(p, (RWSTYPE*) popn());
}

RWSTYPE
tos()
{
        return mach.stack[mach.st-1];
}

/*
 * Check if the stack contains at list CNT elements.
 */
void
checkpop(int cnt)
{
        if (mach.st < cnt)
		rw_error(_("out of popup"));
}
        
/*
 * Push a backreference value on stack.
 * Arguments: str     --    input string
 *            from    --    start of reference in string
 *            to      --    end of reference in string
 */
void
pushref(char *str, int from, int to)
{
	pushstr(str + from, to - from);
}

/*
 * Create a stack frame and enter the function
 */
void
enter(int n)
{
        pushn(mach.sb);
        mach.sb = mach.st;
        mach.st += n;
}

/*
 * Destroy the stack frame and leave the function
 */
void
leave()
{
        /* Save return value */
        mach.rA = popn();
        /* Restore stack frame */
        mach.st = mach.sb;
        mach.sb = popn();
        mach.pc = (pctr_t) popn();
}

RWSTYPE
getarg(int num)
{
        return mach.stack[mach.sb - (STACK_BASE + num)];
}


/* ****************************************************************************
 * Instructions
 */

static int
rw_error(const char *msg)
{
        grad_log(GRAD_LOG_ERR,
	         "%s: %s",
                 _("rewrite runtime error"), msg);
        longjmp(mach.jmp, 1);
        /*NOTREACHED*/
}
                
static int
rw_error_free(char *msg)
{
        grad_log(GRAD_LOG_ERR,
	         "%s: %s",
                 _("rewrite runtime error"), msg);
	free(msg);
        longjmp(mach.jmp, 1);
        /*NOTREACHED*/
}
                
void
rw_call()
{
        pctr_t  pc = (pctr_t) rw_code[mach.pc++];
        pushn(mach.pc); /* save return address */
        mach.pc = pc;
}

void
rw_adjstk()
{
        int delta = (int) rw_code[mach.pc++];
        mach.st -= delta;
        pushn(mach.rA);   /* Push the return back on stack */
}

void
rw_enter()
{
        /*FIXME: runtime checking */
        int n = (int) rw_code[mach.pc++];
        enter(n);
}

void
rw_leave()
{
        leave();
}

/*
 * Push a number on stack
 */
void
rw_pushn()
{
        RWSTYPE n = (RWSTYPE) rw_code[mach.pc++];
        pushn(n);
}

/*
 * Push a reference value on stack
 */
void
rw_pushref()
{
        int i = (int) rw_code[mach.pc++];

        pushref(mach.sA, mach.pmatch[i].rm_so, mach.pmatch[i].rm_eo);
}

/*
 * Push a variable on stack
 */
void
rw_pushv()
{
        stkoff_t n = (stkoff_t) rw_code[mach.pc++];

        pushn(mach.stack[mach.sb + n]);
}

void
rw_pushs()
{
        int   len = (int) rw_code[mach.pc++];
        RWSTYPE *sptr = (RWSTYPE*) (rw_code + mach.pc);

        mach.pc += len;
        pushs(sptr, strlen((char*)sptr), len);
}

/*
 * Assign a value to a variable
 */
void
rw_asgn()
{
        stkoff_t off = (stkoff_t) rw_code[mach.pc++];
        RWSTYPE n;

        cpopn(&n);
        
        mach.stack[mach.sb + off] = n;
        pushn(n);
}

void
assert_request_presence()
{
	if (!mach.req)
		rw_error(_("no request supplied"));
}

/* Check if the A/V pair is supplied in the request
 */
void
rw_attrcheck0()
{
        int attr = (int) rw_code[mach.pc++];

	pushn(grad_avl_find(AVPLIST(&mach), attr) != NULL);
}

void
rw_attrcheck()
{
        int attr = (int) rw_code[mach.pc++];
	RWSTYPE index;
 
	cpopn(&index);
	pushn(grad_avl_find_n(AVPLIST(&mach), attr, index) != NULL);
}

/*
 * Assign a value to an A/V pair
 */
void
attrasgn_internal(int attr, grad_avp_t *pair, RWSTYPE val)
{
	grad_string_t str;
	
	assert_request_presence();
	if (!pair) {
                 pair = grad_avp_create(attr);
                 if (!pair)
                        rw_error(_("can't create A/V pair"));
                 grad_avl_add_pair(&mach.req->avlist, pair);
         }
		
	switch (pair->type) {
	case GRAD_TYPE_STRING:
	case GRAD_TYPE_DATE:
		mem2string(&str, (RWSTYPE*)val);
		grad_free(pair->avp_strvalue);
		pair->avp_strvalue = grad_malloc(str.size+1);
		memcpy(pair->avp_strvalue, str.data, str.size);
		pair->avp_strvalue[str.size] = 0;
		pair->avp_strlength = str.size;
		break;
		
	case GRAD_TYPE_INTEGER:
	case GRAD_TYPE_IPADDR:
		pair->avp_lvalue = val;
		break;
	}
	
	pushn(val);
}

void
rw_attrasgn0()
{
        int attr = (int) rw_code[mach.pc++];
        RWSTYPE val;
        
        cpopn(&val);
	attrasgn_internal(attr, grad_avl_find(AVPLIST(&mach), attr), val);
}

void
rw_attrasgn()
{
        int attr = (int) rw_code[mach.pc++];
        RWSTYPE val;
	RWSTYPE index;
 
        cpopn(&val);
	cpopn(&index);
	attrasgn_internal(attr, grad_avl_find_n(AVPLIST(&mach), attr, index),
			  val);
}

void
rw_attrs0()
{
        int attr = (int) rw_code[mach.pc++];
        grad_avp_t *pair;
        
        if ((pair = grad_avl_find(AVPLIST(&mach), attr)) == NULL) 
                pushstr("", 0);
        else if (pair->prop & GRAD_AP_ENCRYPT) {
		char string[GRAD_STRING_LENGTH+1];
		int len;
		req_decrypt_password(string, mach.req, pair);
		len = strlen(string);
		pushstr(string, len);
	} else
                pushstr(pair->avp_strvalue, pair->avp_strlength);
}

void
rw_attrn0()
{
        int attr = (int) rw_code[mach.pc++];
        grad_avp_t *pair;

        if ((pair = grad_avl_find(AVPLIST(&mach), attr)) == NULL)
                pushn(0);
        else
                pushn(pair->avp_lvalue);
}

void
rw_attrs()
{
        int attr = (int) rw_code[mach.pc++];
        grad_avp_t *pair;
	RWSTYPE index;

	cpopn(&index);
        if ((pair = grad_avl_find_n(AVPLIST(&mach), attr, index)) == NULL) 
                pushstr("", 0);
        else
                pushstr(pair->avp_strvalue, pair->avp_strlength);
}

void
rw_attrn()
{
        int attr = (int) rw_code[mach.pc++];
        grad_avp_t *pair;
	RWSTYPE index;

	cpopn(&index);
        if ((pair = grad_avl_find_n(AVPLIST(&mach), attr, index)) == NULL)
                pushn(0);
        else
                pushn(pair->avp_lvalue);
}

void
rw_attr_delete0()
{
        int attr = (int) rw_code[mach.pc++];
	grad_avl_delete(&mach.req->avlist, attr);
}

void
rw_attr_delete()
{
        int attr = (int) rw_code[mach.pc++];
	RWSTYPE index;

	assert_request_presence();
	cpopn(&index);
	grad_avl_delete_n(&mach.req->avlist, attr, index);
}

/*
 * Pop (and discard) a value from stack
 */
void
rw_popn()
{
        RWSTYPE n;
        cpopn(&n);
}

/*
 * Pop a value from stack into the accumulator
 */
void
rw_popa()
{
        cpopn(&mach.rA);
}

/*
 * Push accumulator on stack
 */
void
rw_pusha()
{
        pushn(mach.rA);
}

/*
 * String concatenation
 */
void
rw_adds()
{
        grad_string_t s1, s2;
	RWSTYPE *p;
	char *s;
	
        checkpop(2);
        poparr(&s2);
        poparr(&s1);
        p = heap_reserve(sizeof(RWSTYPE) + s1.size + s2.size + 1);
	s = (char*)(p + 1);
	memcpy(s, s1.data, s1.size);
	s += s1.size;
	memcpy(s, s2.data, s2.size);
	s += s2.size;
	*s = 0;
	p[0] = s1.size + s2.size;
        pushn((RWSTYPE)p);
}

/*
 * Unary negation
 */
void
rw_neg()
{
        checkpop(1);
        pushn(-popn());
}

/*
 * Bitwise operations
 */
void
rw_b_and()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 & n2);
}

void
rw_b_or()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 | n2);
}

void
rw_b_xor()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 ^ n2);
}

void
rw_shl()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 << n2);
}

void
rw_shr()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 >> n2);
}

/*
 * Addition
 */
void
rw_add()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1+n2);
}

/*
 * Subtraction
 */
void
rw_sub()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1-n2);
}

/*
 * Multiplication
 */
void
rw_mul()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1*n2);
}

/*
 * Division
 */
void
rw_div()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        if (n2 == 0) 
                rw_error(_("division by zero!"));
        pushn(n1/n2);
}

/*
 * Remainder
 */
void
rw_rem()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        if (n2 == 0) 
                rw_error(_("division by zero!"));
        pushn(n1%n2);
}


/* Type conversion */
void
rw_i2s()
{
        int n = popn();
        char buf[64];
        
        snprintf(buf, sizeof(buf), "%d", n);
        pushstr(buf, strlen(buf));
}

void
rw_s2i()
{
	grad_string_t s;
	mem2string(&s, (RWSTYPE *)popn());
        pushn(strtol(s.data, NULL, 0));
}



void
rw_eq()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 == n2);
}

void
rw_ne()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 != n2);
}

void
rw_lt()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 < n2);
}

void
rw_le()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 <= n2);
}

void
rw_gt()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 > n2);
}

void
rw_ge()
{
        int n1, n2;

        checkpop(2);
        n2 = popn();
        n1 = popn();
        pushn(n1 >= n2);
}

void
rw_eqs()
{
        grad_string_t s1, s2;

        checkpop(2);
	poparr(&s2);
	poparr(&s1);
	
        pushn(s1.size == s2.size && memcmp(s1.data, s2.data, s1.size) == 0);
}

void
rw_nes()
{
        grad_string_t s1, s2;

        checkpop(2);
	poparr(&s2);
	poparr(&s1);
	
        pushn(!(s1.size == s2.size && memcmp(s1.data, s2.data, s1.size) == 0));
}

void
rw_lts()
{
        grad_string_t s1, s2;
	size_t size;
	
        checkpop(2);
	poparr(&s2);
	poparr(&s1);
	size = RW_MIN(s1.size, s2.size);
	pushn(memcmp(s1.data, s2.data, size < 0) || s1.size < s2.size); 
}

void
rw_les()
{
        grad_string_t s1, s2;
	size_t size;
	
        checkpop(2);
	poparr(&s2);
	poparr(&s1);
	size = RW_MIN(s1.size, s2.size);
	pushn(memcmp(s1.data, s2.data, size <= 0) || s1.size <= s2.size); 
}

void
rw_gts()
{
        grad_string_t s1, s2;
	size_t size;
	
        checkpop(2);
	poparr(&s2);
	poparr(&s1);
	size = RW_MIN(s1.size, s2.size);
	pushn(memcmp(s1.data, s2.data, size > 0) || s1.size > s2.size); 
}

void
rw_ges()
{
        grad_string_t s1, s2;
	size_t size;
	
        checkpop(2);
	poparr(&s2);
	poparr(&s1);
	size = RW_MIN(s1.size, s2.size);
	pushn(memcmp(s1.data, s2.data, size >= 0) || s1.size >= s2.size); 
}

void
rw_not()
{
        int n;

        checkpop(1);
        n = popn();
        pushn(!n);
}

static void
need_pmatch(size_t n)
{
	n++;
        if (mach.nmatch < n) {
                grad_free(mach.pmatch);
                mach.nmatch = n;
                mach.pmatch = grad_emalloc(n * sizeof(mach.pmatch[0]));
        }
}

void
rw_match()
{
        COMP_REGEX *rx = (COMP_REGEX *)rw_code[mach.pc++];
        grad_string_t s;
        int rc;

	poparr(&s);
	need_pmatch(rx->nmatch);
        mach.sA = s.data;
        
        rc = regexec(&rx->regex, mach.sA, 
                     rx->nmatch + 1, mach.pmatch, 0);
        if (rc && GRAD_DEBUG_LEVEL(1)) {
                char errbuf[512];
                regerror(rc, &rx->regex,
                         errbuf, sizeof(errbuf));
                grad_log(GRAD_LOG_DEBUG,
		         _("rewrite regex failure: %s. Input: %s"),
                         errbuf, (char*)mach.rA);
        }
        pushn(rc == 0);
}

void
rw_jmp()
{
        pctr_t pc = (pctr_t) rw_code[mach.pc++];
        mach.pc = pc;
} 

void
rw_jne()
{
        int n;
        pctr_t pc = (pctr_t) rw_code[mach.pc++];
        
        n = popn();
        if (n != 0)
                mach.pc = pc;
}

void
rw_je()
{
        int n;
        pctr_t pc = (pctr_t) rw_code[mach.pc++];
        
        n = popn();
        if (n == 0)
                mach.pc = pc;
}

void
rw_builtin()
{
        INSTR fun = (INSTR) rw_code[mach.pc++];
        pushn(mach.pc);
        enter(0);
        fun();
        leave();
}

void
run(pctr_t pc)
{
        mach.pc = pc;
        while (rw_code[mach.pc]) {
                if (mach.pc >= rw_codesize)
                        rw_error(_("pc out of range"));
                (*(rw_code[mach.pc++]))();
        }
}


/* ****************************************************************************
 * A placeholder for the garbage collector
 */

void
gc()
{
}


/* ****************************************************************************
 * Built-in functions
 */

/*
 * integer length(string s)
 */
static void
bi_length()
{
	grad_string_t s;
	mem2string(&s, (RWSTYPE*)getarg(1));
        pushn(s.size);
}

/*
 * integer index(string s, integer a)
 */
static void
bi_index()
{
        grad_string_t s;
	char *p;
        int   c;

        mem2string(&s, (RWSTYPE*) getarg(2));
        c = (int) getarg(1);
        p = memchr(s.data, c, s.size);
        pushn(p ? p - s.data : -1);
}

/*
 * integer rindex(string s, integer a)
 */
static void
bi_rindex()
{
        grad_string_t s;
	int i;
        int c;

	mem2string(&s, (RWSTYPE*) getarg(2));
	for (i = s.size - 1; i >= 0; i--)
		if (s.data[i] == c)
			break;
        pushn(i);
}

/*
 * string substr(string s, int start, int length)
 */
static void
bi_substr()
{
        grad_string_t src;
	RWSTYPE *p;
	char *dest;
        int   start, length;

        mem2string(&src, (RWSTYPE*)getarg(3));
        start  = getarg(2);
        length = getarg(1);
        if (length < 0)
                length = src.size - start;
        
        p = heap_reserve(sizeof(RWSTYPE) + length + 1);
	dest = (char *)(p + 1);
        if (length > 0) 
                memcpy(dest, src.data + start, length);
        dest[length] = 0;
	p[0] = length;
        pushn((RWSTYPE)p);
}

static void
bi_field()
{
        grad_string_t str;
	char *p, *endp;
        int fn = getarg(1);
        char *s = "";
        int len = 1;
	
	mem2string(&str, (RWSTYPE*) getarg(2));
	endp = str.data + str.size;
	for (p = str.data; p < endp && fn--; ) {
                /* skip initial whitespace */
                while (p < endp && isspace(*p))
                        p++;

                s = p;
                len = 0;
                while (p < endp && !isspace(*p)) {
                        p++;
                        len++;
                }
        }

	if (p == endp && fn) 
		pushstr("", 0);
	else 
		pushstr(s, len);
}

static void
bi_logit()
{
        grad_string_t msg;
	mem2string(&msg, (RWSTYPE*) getarg(1));
        grad_log(GRAD_LOG_INFO, "%s", msg.data);
        pushn(0);
}

static void
bi_htonl()
{
	pushn(htonl(getarg(1)));
}

static void
bi_ntohl()
{
	pushn(ntohl(getarg(1)));
}

static void
bi_htons()
{
	pushn(htons(getarg(1) & 0xffff));
}

static void
bi_ntohs()
{
	pushn(ntohs(getarg(1) & 0xffff));
}

static void
bi_inet_ntoa()
{
	char buffer[GRAD_IPV4_STRING_LENGTH];
	char *s = grad_ip_iptostr(getarg(1), buffer);
	pushstr(s, strlen(s));
}

static void
bi_inet_aton()
{
	grad_string_t s;
	mem2string(&s, (RWSTYPE*)getarg(1));
	/* Note: inet_aton is not always present. See lib/iputils.c */
	pushn(grad_ip_strtoip(s.data));
}

static void
bi_tolower()
{
	grad_string_t src;
	grad_string_t dest;
	int i;

	mem2string(&src, (RWSTYPE*) getarg(1));
	pushstr(src.data, src.size);
	mem2string(&dest, (RWSTYPE*) tos());
	for (i = 0; i < dest.size; i++)
		dest.data[i] = tolower(dest.data[i]);
}	

static void
bi_toupper()
{
	grad_string_t src;
	grad_string_t dest;
	int i;

	mem2string(&src, (RWSTYPE*) getarg(1));
	pushstr(src.data, src.size);
	mem2string(&dest, (RWSTYPE*) tos());
	for (i = 0; i < dest.size; i++)
		dest.data[i] = toupper(dest.data[i]);
}	

static void
bi_request_code_string()
{
        int code = (int) getarg(1);
	const char *s = grad_request_code_to_name(code);
	pushstr(s, strlen(s));
}

static void
bi_request_source_ip()
{
	assert_request_presence();
	pushn((RWSTYPE) mach.req->ipaddr);
}

static void
bi_request_source_port()
{
	assert_request_presence();
	pushn((RWSTYPE) mach.req->udp_port);
}

static void
bi_request_id()
{
	assert_request_presence();
	pushn((RWSTYPE) mach.req->id);
}

static void
bi_request_code()
{
	assert_request_presence();
	pushn((RWSTYPE) mach.req->code);
}

static void
bi_nas_name()
{
        grad_nas_t *nas;
	grad_uint32_t ip = (grad_uint32_t) getarg(1);

	if ((nas = grad_nas_lookup_ip(ip)) != NULL) {
		char *s = nas->shortname[0] ? nas->shortname : nas->longname;
		pushstr(s, strlen(s));
        } else {
		char nasname[GRAD_MAX_LONGNAME];
		
		grad_ip_gethostname(ip, nasname, sizeof(nasname));
		pushstr(nasname, strlen(nasname));
	}
}

static void
bi_nas_short_name()
{
        grad_nas_t *nas;
	grad_uint32_t ip = (grad_uint32_t) getarg(1);

	if ((nas = grad_nas_lookup_ip(ip)) && nas->shortname[0]) {
		pushstr(nas->shortname, strlen(nas->shortname));
        } else {
		char nasname[GRAD_MAX_LONGNAME];
		
		grad_ip_gethostname(ip, nasname, sizeof(nasname));
		pushstr(nasname, strlen(nasname));
	}
}

static void
bi_nas_full_name()
{
        grad_nas_t *nas;
	grad_uint32_t ip = (grad_uint32_t) getarg(1);

	if ((nas = grad_nas_lookup_ip(ip)) != NULL) {
		pushstr(nas->longname, strlen(nas->longname));
        } else {
		char nasname[GRAD_MAX_LONGNAME];
		
		grad_ip_gethostname(ip, nasname, sizeof(nasname));
		pushstr(nasname, strlen(nasname));
	}
}

static void
bi_gethostbyaddr()
{
	grad_uint32_t ip = (grad_uint32_t) getarg(1);
	char nasname[GRAD_MAX_LONGNAME];
		
	grad_ip_gethostname(ip, nasname, sizeof(nasname));
	pushstr(nasname, strlen(nasname));
}

static void
bi_gethostbyname()
{
	grad_string_t s;
	mem2string(&s, (RWSTYPE*)getarg(1));
	pushn((RWSTYPE) grad_ip_gethostaddr(s.data));
}

static void
bi_time()
{
	pushn((RWSTYPE) time(NULL));
}

static void
bi_strftime()
{
	struct tm *tm;
	char *base;
	time_t t = (time_t) getarg(1);
	grad_string_t fmt;
	size_t n;
	
	mem2string(&fmt, (RWSTYPE*) getarg(2));
	tm = localtime(&t);
	base = temp_space_create();
	n = strftime(base, temp_space_size(), fmt.data, tm);
	pushstr(base, n);
}

static void
rw_regerror(const char *prefix, regex_t *rx, int rc)
{
	size_t sz = regerror(rc, rx, NULL, 0);
	char *errbuf = malloc(sz + strlen (prefix) + 1);
	if (!errbuf) 
		rw_error(prefix);
	else {
		strcpy (errbuf, prefix);
		regerror(rc, rx, errbuf + strlen(prefix), sz);
		rw_error_free(errbuf);
	}
}

enum subst_segment_type {
	subst_text,       /* pure text */
	subst_ref,        /* back reference (\NN) */
	subst_match       /* substitute whole match (&) */
};

struct subst_segment {
	enum subst_segment_type type;
	union {
		struct {
			char *ptr;
			size_t len; 
		} text;      /* type == subst_text */
		size_t ref;  /* type == subst_ref */
	} v;
};

static void
add_text_segment(grad_list_t *lst, char *ptr, char *end)
{
	struct subst_segment *seg;
	if (ptr >= end)
		return;
	seg = grad_emalloc(sizeof(*seg));
	seg->type = subst_text;
	seg->v.text.ptr = ptr;
	seg->v.text.len = end - ptr;
	grad_list_append(lst, seg);
}

static void
add_match_segment(grad_list_t *lst)
{
	struct subst_segment *seg = grad_emalloc(sizeof(*seg));
	seg->type = subst_match;
	grad_list_append(lst, seg);
}

static void
add_ref_segment(grad_list_t *lst, size_t ref)
{
	struct subst_segment *seg = grad_emalloc(sizeof(*seg));
	seg->type = subst_ref;
	seg->v.ref = ref;
	grad_list_append(lst, seg);
}

grad_list_t *
subst_create(char *text)
{
	char *p;
	grad_list_t *lst = grad_list_create();
	if (!lst)
		return lst;

	p = text;
	while (*p) {
		if (*p == '\\' && p[1]) {
			if (p[1] == '&') {
				add_text_segment(lst, text, p);
				text = ++p;
				p++;
			} else if (p[1] == '\\') {
				add_text_segment(lst, text, p+1);
				p += 2;
				text = p;
			} else if (isdigit(p[1])) {
				size_t ref;
				char *q;
				
				add_text_segment(lst, text, p);
				ref = strtoul(p+1, &q, 10);
				add_ref_segment(lst, ref);
				text = p = q;
			} else {
				add_text_segment(lst, text, p);
				text = ++p;
			}
		} else if (*p == '&') {
			add_text_segment(lst, text, p);
			add_match_segment(lst);
			text = ++p;
		} else
			p++;
	}
	add_text_segment(lst, text, p);
	return lst;	
}

int
seg_free(void *item, void *data ARG_UNUSED)
{
	grad_free(item);
	return 0;
}

void
subst_destroy(grad_list_t *lst)
{
	grad_list_destroy(&lst, seg_free, NULL);
}

void
subst_run(grad_list_t *subst, size_t nsub,
	  char **baseptr, char *arg)
{
	grad_iterator_t *itr = grad_iterator_create(subst);
	struct subst_segment *seg;
	
	for (seg = grad_iterator_first(itr); seg; seg = grad_iterator_next(itr)) {
		switch (seg->type) {
		case subst_text:
			temp_space_copy(baseptr,
					seg->v.text.ptr, seg->v.text.len);
			break;
			
		case subst_ref:
			if (seg->v.ref >= nsub)
				rw_error(_("Invalid backreference"));
			temp_space_copy(baseptr,
					arg + mach.pmatch[seg->v.ref].rm_so,
					mach.pmatch[seg->v.ref].rm_eo -
					  mach.pmatch[seg->v.ref].rm_so);
			break;
			    
		case subst_match:
			temp_space_copy(baseptr,
					arg + mach.pmatch[0].rm_so,
					mach.pmatch[0].rm_eo -
					  mach.pmatch[0].rm_so);
		}
	}
	grad_iterator_destroy(&itr);
}

static void
bi_gsub()
{
	grad_string_t re_str;
	grad_string_t repl;
	grad_string_t arg;
	char *p;
	char *base;
	regex_t rx;
	grad_list_t *subst;
	int rc;
	
	mem2string(&re_str, (RWSTYPE*) getarg(3));
	mem2string(&repl, (RWSTYPE*) getarg(2));
	mem2string(&arg, (RWSTYPE*) getarg(1));
	p = arg.data;
	
        rc = regcomp(&rx, re_str.data, regcomp_flags);
        if (rc) 
		rw_regerror(_("regexp compile error: "), &rx, rc);

	need_pmatch(rx.re_nsub);

	subst = subst_create(repl.data);
	if (!subst)
		rw_error(_("gsub: not enough memory"));
	
	base = temp_space_create();
	while (*p
	       && regexec(&rx, p, rx.re_nsub + 1, mach.pmatch, 0) == 0) {
		temp_space_copy(&base, p, mach.pmatch[0].rm_so);
		subst_run(subst, rx.re_nsub + 1, &base, p);
		p += mach.pmatch[0].rm_eo;
		if (mach.pmatch[0].rm_eo == 0)
			p++;
	}
	temp_space_copy(&base, p, strlen(p) + 1);
	subst_destroy(subst);
	regfree(&rx);

	pushn((RWSTYPE)temp_space_fix(base));
}

static void
bi_sub()
{
	grad_string_t re_str;
	grad_string_t repl;
	grad_string_t arg;
	char *p;
	char *base;
	regex_t rx;
	grad_list_t *subst;
	int rc;
	
	mem2string(&re_str, (RWSTYPE*) getarg(3));
	mem2string(&repl, (RWSTYPE*) getarg(2));
	mem2string(&arg, (RWSTYPE*) getarg(1));
	
        rc = regcomp(&rx, re_str.data, regcomp_flags);
        if (rc) 
		rw_regerror(_("regexp compile error: "), &rx, rc);

	need_pmatch(rx.re_nsub);

	subst = subst_create(repl.data);
	if (!subst)
		rw_error(_("sub: not enough memory"));
	
	base = temp_space_create();
	p = arg.data;
	if (regexec(&rx, p, rx.re_nsub + 1, mach.pmatch, 0) == 0) {
		temp_space_copy(&base, p, mach.pmatch[0].rm_so);
		subst_run(subst, rx.re_nsub + 1, &base, p);
		p += mach.pmatch[0].rm_eo;
	}
	temp_space_copy(&base, p, strlen(p) + 1);
	subst_destroy(subst);
	regfree(&rx);

	pushn((RWSTYPE)temp_space_fix(base));
}

#define ISPRINT(c) (((unsigned char)c) < 128 && (isalnum(c) || c == '-'))

static void
bi_qprn()
{
	grad_string_t arg;
	char *p, *s, *end;
	size_t count;
	RWSTYPE *sp;
	
	mem2string(&arg, (RWSTYPE*)getarg(1));
	end = arg.data + arg.size;
	for (count = 0, p = arg.data; p < end; p++)
		if (!ISPRINT(*p))
			count++;

	/* Each encoded character takes 3 bytes. */
	sp = heap_reserve(sizeof(RWSTYPE) + arg.size + 2*count + 1);
	sp[0] = arg.size + 2*count;
	pushn((RWSTYPE) sp);
	
	for (p = (char*)(sp + 1), s = arg.data; s < end; s++) {
		if (ISPRINT(*s))
			*p++ = *s;
		else {
			char buf[3];
			snprintf(buf, sizeof buf, "%02X", *(unsigned char*)s);
			*p++ = '%';
			*p++ = buf[0];
			*p++ = buf[1];
		}
	}
	*p = 0;
}

static void
bi_quote_string()
{
	int quote;
	grad_string_t arg;
	RWSTYPE *sp;
	char *p;
	size_t size;
	
	mem2string(&arg, (RWSTYPE*)getarg(1));
	size = grad_argcv_quoted_length_n(arg.data, arg.size, &quote);
	sp = heap_reserve(sizeof(RWSTYPE) + size + 1);
	sp[0] = size;
	pushn((RWSTYPE)sp);
	p = (char*)(sp + 1);
	grad_argcv_quote_copy_n(p, arg.data, arg.size);
}

static void
bi_unquote_string()
{
	int quote;
	grad_string_t arg;
	RWSTYPE *sp;
	char *p;
	size_t size;
	
	mem2string(&arg, (RWSTYPE*)getarg(1));
	sp = heap_reserve(sizeof(RWSTYPE) +  arg.size + 1);
	p = (char*)(sp + 1);
	grad_argcv_unquote_copy(p, arg.data, arg.size);
	sp[0] = strlen(p);
	pushn((RWSTYPE)sp);
}

static void
bi_textdomain()
{
	grad_string_t s;
	mem2string(&s, (RWSTYPE*)getarg(1));
	pushstr(default_gettext_domain, strlen (default_gettext_domain));
	grad_string_replace(&default_gettext_domain, s.data);
}

static void
bi_gettext()
{
	grad_string_t s;
	const char *p;
	
	mem2string(&s, (RWSTYPE*)getarg(1));
	p = dgettext(default_gettext_domain, s.data);
	pushstr(p, strlen(p));
}

static void
bi_dgettext()
{
	grad_string_t domain;
	grad_string_t text;
	const char *p;
	
	mem2string(&domain, (RWSTYPE*)getarg(2));
	mem2string(&text, (RWSTYPE*)getarg(1));
	p = dgettext(domain.data, text.data);
	pushstr(p, strlen(p));
}


static void
bi_ngettext()
{
	grad_string_t s;
	grad_string_t pl;
	unsigned long n;
	const char *p;

	mem2string(&s, (RWSTYPE*)getarg(3));
	mem2string(&pl, (RWSTYPE*)getarg(2));
	n = (unsigned long) getarg(1);
	p = dngettext(default_gettext_domain,
		      s.data,
		      pl.data,
		      n);
	pushstr(p, strlen(p));
}

static void
bi_dngettext()
{
	grad_string_t domain;
	grad_string_t s;
	grad_string_t pl;
	unsigned long n;
	const char *p;

	mem2string(&domain, (RWSTYPE*)getarg(4));
	mem2string(&s, (RWSTYPE*)getarg(3));
	mem2string(&pl, (RWSTYPE*)getarg(2));
	n = (unsigned long) getarg(1);

        p = dngettext(domain.data, s.data, pl.data, n);
	pushstr(p, strlen(p));
}

static builtin_t builtin[] = {
        { bi_length,  "length", Integer, "s" },
	{ bi_index,   "index",  Integer, "si" },
        { bi_rindex,  "rindex", Integer, "si" },
        { bi_substr,  "substr", String,  "sii" },
        { bi_logit,   "logit",  Integer, "s" },
        { bi_field,   "field",  String,  "si" },
	{ bi_ntohl, "ntohl", Integer, "i" },
	{ bi_htonl, "htonl", Integer, "i" },
	{ bi_ntohs, "ntohs", Integer, "i" },
	{ bi_htons, "htons", Integer, "i" },
	{ bi_inet_ntoa, "inet_ntoa", String, "i" },
	{ bi_inet_aton, "inet_aton", Integer, "s" },
	{ bi_sub, "sub", String, "sss" },
	{ bi_gsub, "gsub", String, "sss" },
	{ bi_qprn, "qprn", String, "s" },
	{ bi_tolower, "tolower", String, "s" },
	{ bi_toupper, "toupper", String, "s" },
	{ bi_unquote_string, "unquote_string", String, "s" },
	{ bi_quote_string, "quote_string", String, "s" },
	{ bi_request_code_string, "request_code_string", String, "i" },
	/* i18n support */
	{ bi_gettext, "gettext", String, "s" },
	{ bi_gettext, "_", String, "s" },
	{ bi_dgettext, "dgettext", String, "ss" },
	{ bi_ngettext, "ngettext", String, "ssi" },
	{ bi_dngettext, "dngettext", String, "sssi" },
	{ bi_textdomain, "textdomain", String, "s" },
	/* Request internals */
	{ bi_request_source_ip,   "request_source_ip", Integer, "" },
	{ bi_request_source_port, "request_source_port", Integer, "" },
	{ bi_request_id, "request_id", Integer, "" },
	{ bi_request_code, "request_code", Integer, "" },
	/* Radius database */
	{ bi_nas_name, "nas_name", String, "i" },
	{ bi_nas_short_name, "nas_short_name", String, "i" },
	{ bi_nas_full_name, "nas_full_name", String, "i" },
	/* DNS lookups */
	{ bi_gethostbyaddr, "gethostbyaddr", Integer, "s" },
	{ bi_gethostbyname, "gethostbyname", String, "i" },
	/* Time functions */
	{ bi_time, "time", Integer, "" },
	{ bi_strftime, "strftime", String, "si" },
	{ NULL }
};

builtin_t *
builtin_lookup(char *name)
{
        int i;

        for (i = 0; builtin[i].handler; i++)
                if (strcmp(name, builtin[i].name) == 0)
                        return &builtin[i];
        return NULL;
}


/* ****************************************************************************
 * Function registering/unregistering 
 */

int
function_free(FUNCTION *f)
{
        PARAMETER *parm, *next;
        
        rx_free(f->rx_list);
        parm = f->parm;
        while (parm) {
                next = parm->next;
                grad_free(parm);
                parm = next;
        }
        return 0;
}
                
FUNCTION *
function_install(FUNCTION *fun)
{
        FUNCTION *fp;

        if (fp = (FUNCTION *)grad_sym_lookup(rewrite_tab, fun->name)) {
                grad_log_loc(GRAD_LOG_ERR, &fun->loc,
			     _("redefinition of function %s"));
                grad_log_loc(GRAD_LOG_ERR, &fp->loc,
			     _("previously defined here"));
                errcnt++;
                return fp;
        }  
        fp = (FUNCTION*)grad_sym_install(rewrite_tab, fun->name);
        
        fp->rettype = fun->rettype;
        fp->entry   = fun->entry;
        fp->rx_list = fun->rx_list;
        fp->nparm   = fun->nparm;        
        fp->parm    = fun->parm;
        fp->stack_alloc = fun->stack_alloc;
	fp->loc     = fun->loc;
        return fp;
}


/* ****************************************************************************
 * Runtime functions
 */

static char pair_print_prefix[] = "    ";

static void
rw_mach_init()
{
	memset(&mach, 0, sizeof(mach));

	if (!runtime_stack)
		runtime_stack = grad_emalloc(rewrite_stack_size *
					     sizeof(runtime_stack[0]));
	
	mach.stack = runtime_stack;
        mach.st = 0;                      /* Stack top */
        mach.ht = rewrite_stack_size - 1; /* Heap top */

	grad_string_replace(&default_gettext_domain, PACKAGE);
}

static void
rw_mach_destroy()
{
	grad_free(mach.pmatch);
	mach.pmatch = NULL;
}

FUNCTION *
rewrite_check_function(const char *name, grad_data_type_t rettype, char *typestr)
{
	int i;
	PARAMETER *p;
	
	FUNCTION *fun = (FUNCTION*) grad_sym_lookup(rewrite_tab, name);
        if (!fun) {
                grad_log(GRAD_LOG_ERR, _("function %s not defined"), name);
                return NULL;
        }
	if (fun->rettype != rettype) {
		grad_log(GRAD_LOG_ERR, _("function %s returns wrong data type"), name);
		return NULL;
	}

	for (i = 0, p = fun->parm; i < fun->nparm; i++, p = p->next) {
                switch (typestr[i]) {
		case 0:
			grad_log(GRAD_LOG_ERR,
			         _("function %s takes too many arguments"),
			         name);
			return NULL;
			
                case 'i':
                        if (p->datatype != Integer) {
				grad_log(GRAD_LOG_ERR,
				         _("function %s: argument %d must be integer"),
				         name, i+1);
				return NULL;
			}
                        break;
			
                case 's':
                        if (p->datatype != String) {
				grad_log(GRAD_LOG_ERR,
				         _("function %s: argument %d must be string"),
				         name, i+1);
				return NULL;
			}
                        break;
			
                default:
                        grad_insist_fail("bad datatype");
                }
        }

	if (typestr[i]) {
		grad_log(GRAD_LOG_ERR,
		         _("function %s takes too few arguments"),
		         name);
		return NULL;
	}

	return fun;
}

int
run_init(pctr_t pc, grad_request_t *request)
{
        FILE *fp;

	rw_mach_init();
        if (setjmp(mach.jmp)) {
		rw_mach_destroy();
                return -1;
	}
        
        mach.req = request;
        if (GRAD_DEBUG_LEVEL(2)) {
                fp = debug_open_file();
                fprintf(fp, "Before rewriting:\n");
                grad_avl_fprint(fp, pair_print_prefix, 1, AVPLIST(&mach));
                fclose(fp);
        }

        /* Imitate a function call */
        pushn(0);                  /* Return address */
        run(pc);                   /* call function */
        if (GRAD_DEBUG_LEVEL(2)) {
                fp = debug_open_file();
                fprintf(fp, "After rewriting\n");
                grad_avl_fprint(fp, pair_print_prefix, 1, AVPLIST(&mach));
                fclose(fp);
        }
	rw_mach_destroy();
	return 0;
}

static void
return_value(grad_value_t *val)
{
	u_char *p;
		
	switch (val->type) {
	case Integer:   
		val->datum.ival = mach.rA;
		break;
			
	case String:
		mem2string(&val->datum.sval, (RWSTYPE*) mach.rA);
		p = grad_emalloc (val->datum.sval.size + 1);
		memcpy (p, val->datum.sval.data, val->datum.sval.size);
		p[val->datum.sval.size] = 0;
		val->datum.sval.data = p;
		break;
		
	default:
		abort();
	}
}

static int
evaluate(pctr_t pc, grad_request_t *req, grad_value_t *val)
{
        if (run_init(pc, req))
		return -1;
	if (val)
		return_value(val);
        return 0;
}

int
rewrite_invoke(grad_data_type_t rettype, grad_value_t *val,
	       const char *name,
	       grad_request_t *request, char *typestr, ...)
{
        FILE *fp;
        va_list ap;
        FUNCTION *fun;
        int nargs;
        char *s;

        fun = rewrite_check_function(name, rettype, typestr);
	if (!fun)
		return -1;

	rw_mach_init();
        if (setjmp(mach.jmp)) {
                rw_mach_destroy();
                return -1;
        }
        
        mach.req = request;
        if (GRAD_DEBUG_LEVEL(2)) {
                fp = debug_open_file();
                fprintf(fp, "Before rewriting:\n");
                grad_avl_fprint(fp, pair_print_prefix, 1, AVPLIST(&mach));
                fclose(fp);
        }

        /* Pass arguments */
        nargs = 0;

	va_start(ap, typestr);
        while (*typestr) {
                nargs++;
                switch (*typestr++) {
                case 'i':
                        pushn(va_arg(ap, int));
                        break;
                case 's':
                        s = va_arg(ap, char*);
                        pushstr(s, strlen(s));
                        break;
                default:
                        grad_insist_fail("bad datatype");
                }
        }
        va_end(ap);

        /* Imitate a function call */
        pushn(0);                  /* Return address */
        run(fun->entry);           /* call function */
        if (GRAD_DEBUG_LEVEL(2)) {
                fp = debug_open_file();
                fprintf(fp, "After rewriting\n");
                grad_avl_fprint(fp, pair_print_prefix, 1, AVPLIST(&mach));
                fclose(fp);
        }
	val->type = fun->rettype; 
	return_value(val);
        rw_mach_destroy();
        return 0;
}

char *
rewrite_compile(char *expr)
{
	int rc;
	FUNCTION *fun;
	char *name = grad_emalloc(strlen(expr) + 3); 

	sprintf(name, "$%s$", expr);
        fun = (FUNCTION*) grad_sym_lookup(rewrite_tab, name);
        if (!fun) {
		rc = parse_rewrite_string(expr);
		if (rc) {
			grad_free(name);
			return NULL;
		}
		function->name = name;
		function_install(function);
	}
	return name;
}

int
rewrite_interpret(char *expr, grad_request_t *req, grad_value_t *val)
{
	pctr_t save_pc = rw_pc;
	int rc;
	
	rc = parse_rewrite_string(expr);
	rw_pc = save_pc;
	if (rc)
		return rc;

	val->type = return_type;
	if (return_type == Undefined) 
		return -1;

	return evaluate(rw_pc, req, val);
}

int
rewrite_eval(char *symname, grad_request_t *req, grad_value_t *val)
{
        FUNCTION *fun;
	
        fun = (FUNCTION*) grad_sym_lookup(rewrite_tab, symname);
        if (!fun)
		return -1;
	
	if (fun->nparm) {
		grad_log(GRAD_LOG_ERR,
		         ngettext("function %s() requires %d parameter",
				  "function %s() requires %d parameters",
				  fun->nparm),
		         fun->name, fun->nparm);
		return -1;
	}

	if (val)
		val->type = fun->rettype;
	return evaluate(fun->entry, req, val);
}
	

/* ****************************************************************************
 * Configuration
 */

static grad_list_t *source_list;        /* List of loaded source files */
static grad_list_t *rewrite_load_path;  /* Load path list */

/* Add a path to load path */
static void
rewrite_add_load_path(const char *str)
{
	if (!rewrite_load_path)
		rewrite_load_path = grad_list_create();
	grad_list_append(rewrite_load_path, grad_estrdup(str));
}

void
register_source_name(char *path)
{
	if (!source_list)
		source_list = grad_list_create();
	grad_list_append(source_list, path);
}

struct load_data {
	int rc;
	char *name;
};

/* Try to load a source file.
   ITEM is a directory name, DATA is struct load_data.
   Return 1 if the file was found (no matter was it loaded or not) */
static int
try_load(void *item, void *data)
{
	int rc = 0;
	struct load_data *lp = data;
	char *path = grad_mkfilename((char*)item, lp->name);

	lp->rc = parse_rewrite(path);
	if (lp->rc >= 0) {
		register_source_name(path);
		rc = 1;
	} else
		grad_free(path);
	return rc;
}

/* Load the given rewrite module. */
int
rewrite_load_module(char *name)
{
	int rc;
	if (name[0] == '/') {
		register_source_name(grad_estrdup(name));
		rc = parse_rewrite(name);
	} else {
		struct load_data ld;
		ld.rc = 1;
		ld.name = name;
		grad_list_iterate(rewrite_load_path, try_load, &ld);
		rc = ld.rc;
	}
	return rc;
}

static int
free_path(void *item, void *data ARG_UNUSED)
{
	grad_free(item);
	return 0;
}

static grad_list_t *source_candidate_list; /* List of modules that are to
					      be loaded */

int
rewrite_stmt_term(int finish, void *block_data, void *handler_data)
{
	if (!finish) {
		grad_symtab_clear(rewrite_tab);
		
		rw_yydebug = GRAD_DEBUG_LEVEL(50);
		grad_list_destroy(&source_list, free_path, NULL);
		grad_list_destroy(&rewrite_load_path, free_path, NULL);
		rewrite_add_load_path(grad_config_dir);
		rewrite_add_load_path(RADIUS_DATADIR "/rewrite");

		grad_free(runtime_stack);
		runtime_stack = NULL;
	} 
	return 0;
}

static int
rewrite_cfg_add_load_path(int argc, cfg_value_t *argv,
			  void *block_data, void *handler_data)
{
	if (argc > 2) {
		cfg_argc_error(0);
		return 0;
	}

 	if (argv[1].type != CFG_STRING) {
		cfg_type_error(CFG_STRING);
		return 0;
	}

	rewrite_add_load_path(argv[1].v.string);
	return 0;
}

static int
rewrite_cfg_load(int argc, cfg_value_t *argv,
		 void *block_data, void *handler_data)
{
	if (argc > 2) {
		cfg_argc_error(0);
		return 0;
	}

 	if (argv[1].type != CFG_STRING) {
		cfg_type_error(CFG_STRING);
		return 0;
	}

	grad_list_append(source_candidate_list, grad_estrdup(argv[1].v.string));
	return 0;
}

/* Configuration hooks and initialization */

static void
rewrite_before_config_hook(void *a ARG_UNUSED, void *b ARG_UNUSED)
{
	grad_list_destroy(&source_candidate_list, free_path, NULL);
	source_candidate_list = grad_list_create();
	code_init();
}

static int
_load_module(void *item, void *data ARG_UNUSED)
{
	if (rewrite_load_module(item) == -2)
		grad_log(GRAD_LOG_ERR, _("file not found: %s"), item);
	return 0;
}

void
rewrite_load_all(void *a ARG_UNUSED, void *b ARG_UNUSED)
{
	if (!source_candidate_list)
		return;
	
	/* For compatibility with previous versions load the
	   file $grad_config_dir/rewrite, if no explicit "load" statements
	   were given */
	if (grad_list_count(source_candidate_list) == 0)
		rewrite_load_module("rewrite");
	
	grad_list_iterate(source_candidate_list, _load_module, NULL);
#if defined(MAINTAINER_MODE)
        if (GRAD_DEBUG_LEVEL(100))
                debug_dump_code();
#endif
}

void
rewrite_init()
{
	rewrite_tab = grad_symtab_create(sizeof(FUNCTION), function_free);
	radiusd_set_preconfig_hook(rewrite_before_config_hook, NULL, 0);
}


struct cfg_stmt rewrite_stmt[] = {
	{ "stack-size", CS_STMT, NULL, cfg_get_number, &rewrite_stack_size,
	  NULL, NULL },
	{ "load-path", CS_STMT, NULL, rewrite_cfg_add_load_path, NULL, NULL, NULL },
	{ "load", CS_STMT, NULL, rewrite_cfg_load, NULL, NULL, NULL },
	{ NULL, }
};

size_t
rewrite_get_stack_size()
{
	return rewrite_stack_size;
}

void
rewrite_set_stack_size(size_t s)
{
	if (s == rewrite_stack_size)
		return;
	rewrite_stack_size = s;
	grad_free(runtime_stack);
	runtime_stack = NULL;
}
				       
void
grad_value_free(grad_value_t *val)
{
	if (val->type == String)
		grad_free(val->datum.sval.data);
}


/* ****************************************************************************
 * Guile interface
 */
#ifdef USE_SERVER_GUILE

SCM
radscm_datum_to_scm(grad_value_t *val)
{
        switch (val->type) {
        case Integer:
                return scm_from_long(val->datum.ival);

        case String:
		/* FIXME! */
                return scm_makfrom0str(val->datum.sval.data);

	default:
		grad_insist_fail("Unknown data type");
        }
        return SCM_UNSPECIFIED;
}

int
radscm_scm_to_ival(SCM cell, int *val)
{
        if (SCM_IMP(cell)) {
                if (SCM_INUMP(cell))  
                        *val = SCM_INUM(cell);
                else if (SCM_BIGP(cell)) 
                        *val = (grad_uint32_t) scm_i_big2dbl(cell);
                else if (SCM_CHARP(cell))
                        *val = SCM_CHAR(cell);
                else if (cell == SCM_BOOL_F)
                        *val = 0;
                else if (cell == SCM_BOOL_T)
                        *val = 1;
                else if (cell == SCM_EOL)
                        *val =0;
                else
                        return -1;
        } else {
                if (scm_is_string(cell)) {
                        char *p;
                        *val = strtol(scm_i_string_chars(cell), &p, 0);
                        if (*p)
                                return -1;
                } else
                        return -1;
        }
        return 0;
}

SCM
radscm_rewrite_execute(const char *func_name, SCM ARGS)
{
        const char *name;
        FUNCTION *fun;
        PARAMETER *parm;
        int nargs;
        int n, rc;
        grad_value_t value;
        SCM cell;
        SCM FNAME;
	SCM retval;
	
        FNAME = SCM_CAR(ARGS);
        ARGS  = SCM_CDR(ARGS);
        SCM_ASSERT(scm_is_string(FNAME), FNAME, SCM_ARG1, func_name);

        name = scm_i_string_chars(FNAME);
        fun = (FUNCTION*) grad_sym_lookup(rewrite_tab, name);
        if (!fun) 
                scm_misc_error(func_name,
                               _("function ~S not defined"),
                               scm_list_1(FNAME));
	
        rw_mach_init();

        /* Pass arguments */
        nargs = 0;
        parm = fun->parm;
        
        for (cell = ARGS; cell != SCM_EOL;
	     cell = SCM_CDR(cell), parm = parm->next) {
                SCM car = SCM_CAR(cell);

                if (++nargs > fun->nparm) {
                        rw_code_unlock();
                        scm_misc_error(func_name,
                                       _("too many arguments for ~S"),
                                       scm_list_1(FNAME));
                }

                switch (parm->datatype) {
                case Integer:
                        rc = radscm_scm_to_ival(car, &n);
                        if (!rc) 
                                pushn(n);
                        break;
                        
                case String:
                        if (scm_is_string(car)) {
                                const char *p = scm_i_string_chars(car);
                                pushstr(p, strlen(p));
                                rc = 0;
                        } else
                                rc = 1;
			break;

		default:
			grad_insist_fail("Unknown data type");
                }

                if (rc) {
                        rw_mach_destroy();
                        scm_misc_error(func_name,
				       _("type mismatch in argument ~S(~S) in call to ~S"),
                                       scm_list_3(scm_from_int(nargs),
						  car,
						  FNAME));
                }
        }

        if (fun->nparm != nargs) {
		rw_mach_destroy();
                scm_misc_error(func_name,
                               _("too few arguments for ~S"),
                               scm_list_1(FNAME));
        }
        
        /* Imitate a function call */
        if (setjmp(mach.jmp)) {
                rw_mach_destroy();
                return SCM_BOOL_F;
        }

        pushn(0);                         /* Return address */
        run(fun->entry);                  /* call function */

	value.type = fun->rettype;
	return_value(&value);
	retval = radscm_datum_to_scm(&value);
	grad_value_free(&value);
	rw_mach_destroy();
        return retval;
}


#endif

        /*HONY SOIT QUI MAL Y PENSE*/
