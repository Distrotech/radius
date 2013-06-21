/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
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
/* Line 2058 of yacc.c  */
#line 70 "users.y"

        char *string;
        grad_matching_rule_t *rule;
        struct {
                grad_avp_t *lhs, *rhs;
        } descr;
        grad_avp_t *pair;
        enum grad_operator op;


/* Line 2058 of yacc.c  */
#line 92 "users_gram.h"
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
