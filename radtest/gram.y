%{
/* This file is part of GNU Radius.
   Copyright (C) 2000,2001,2002,2003,2004 Free Software Foundation, Inc.

   Written by Sergey Poznyakoff
  
   GNU Radius is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
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

int yyerror(char *s);

extern int yylex();

static int current_nesting_level; /* Nesting level of WHILE/DO statements */
static int error_count;
 
%}

%token EOL AUTH ACCT SEND EXPECT T_BEGIN T_END
%token IF ELSE WHILE DO BREAK CONTINUE
%token PRINT 
%token EXIT
%token <deref> IDENT
%token <parm> PARM
%token <string> NAME
%token <number> NUMBER
%token <string> QUOTE
%token <ipaddr> IPADDRESS

%left OR
%left AND
%nonassoc EQ NE 
%nonassoc LT LE GT GE
%left '+' '-'
%left '*' '/' '%'
%left UMINUS NOT

%type <op> op
%type <pair> pair
%type <list> pair_list prlist list
%type <i> closure req_code nesting_level port_type
%type <node> stmt lstmt expr maybe_expr value bool cond expr_or_pair_list pritem
%type <var> send_flag imm_value
%type <symtab> send_flags send_flag_list

%union {
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
}

%%

program       : /* empty */
              | input
              ; 

input         : lstmt
                {
			if (!error_count)
				radtest_eval($1);
			radtest_free_mem();
			error_count = 0;
		}
              | input lstmt
                {
			if (!error_count)
				radtest_eval($2);
			radtest_free_mem();
			error_count = 0;
		}
              ;

list          : lstmt
                {
			$$ = grad_list_create();
			if ($1)
				grad_list_append($$, $1);
		}
              | list lstmt
                {
			if ($2) 
				grad_list_append($1, $2);
			$$ = $1;
		}
              ;

lstmt         : /* empty */ EOL
                {
			$$ = NULL;
		}
              | stmt EOL
              | error EOL
                {
                        yyclearin;
                        yyerrok;
                }
	      ;

stmt          : T_BEGIN list T_END
                {
			$$ = radtest_node_alloc(radtest_node_stmt);
			$$->v.list = $2;			
		}
              | IF cond EOL stmt 
                {
			$$ = radtest_node_alloc(radtest_node_cond);
			$$->v.cond.cond = $2;
			$$->v.cond.iftrue = $4;
			$$->v.cond.iffalse = NULL;
		}
              | IF cond EOL stmt ELSE stmt 
                {
			$$ = radtest_node_alloc(radtest_node_cond);
			$$->v.cond.cond = $2;
			$$->v.cond.iftrue = $4;
			$$->v.cond.iffalse = $6;
		}
              | WHILE cond { current_nesting_level++; } stmt
                {
			current_nesting_level--;
			$$ = radtest_node_alloc(radtest_node_loop);
			$$->v.loop.cond = $2;
			$$->v.loop.body = $4;
			$$->v.loop.first_pass = 0;
		}
              | DO { current_nesting_level++; } stmt WHILE {current_nesting_level--;} cond  
                {
			$$ = radtest_node_alloc(radtest_node_loop);
			$$->v.loop.cond = $3;
			$$->v.loop.body = $6;
			$$->v.loop.first_pass = 1;
                } 
              | PRINT prlist 
                {
			$$ = radtest_node_alloc(radtest_node_print);
			$$->v.list = $2;
		}
              | NAME EQ expr 
                {
			$$ = radtest_node_alloc(radtest_node_asgn);
			$$->v.asgn.name = $1;
			$$->v.asgn.expr = $3;
		}
              | SEND send_flags port_type req_code expr_or_pair_list 
                {
			$$ = radtest_node_alloc(radtest_node_send);
			$$->v.send.cntl = $2;
			$$->v.send.port_type = $3;
			$$->v.send.code = $4;
			$$->v.send.expr = $5;
		}			
              | EXPECT req_code expr_or_pair_list 
                {
			$$ = radtest_node_alloc(radtest_node_expect);
			$$->v.expect.code = $2;
			$$->v.expect.expr = $3;
		}
              | EXIT maybe_expr 
                {
			$$ = radtest_node_alloc(radtest_node_exit);
			$$->v.expr = $2;
		}
	      | BREAK nesting_level 
                {
			if ($2 > current_nesting_level) {
				parse_error(_("Not enough 'while's to break from"));
				YYERROR;
			}
			$$ = radtest_node_alloc(radtest_node_break);
			$$->v.level = $2;
		}
	      | CONTINUE nesting_level 
                {
			if ($2 > current_nesting_level) {
				parse_error(_("Not enough 'while's to continue"));
				YYERROR;
			}
			$$ = radtest_node_alloc(radtest_node_continue);
			$$->v.level = $2;
		}
              ;

nesting_level : /* empty */
                {
			$$ = 1;
		}
              | NUMBER
                {
			$$ = $1;
		}
              ;

port_type     : AUTH
                {
			$$ = PORT_AUTH;
		}
              | ACCT
                {
			$$ = PORT_ACCT;
		}
              ;

req_code      : NUMBER
                {
			$$ = $1;
		}
              | NAME
                {
			$$ = grad_request_name_to_code($1);
			if ($$ == 0) {
				yyerror("expected integer value or request code name");
				YYERROR;
			}
		}
              ;

send_flags    : /* empty */
                {
			$$ = NULL;
		}
              | send_flag_list
              ;

send_flag_list: send_flag
                {
			radtest_variable_t *var;
			
			$$ = grad_symtab_create(sizeof(*var), var_free);
			var = (radtest_variable_t*) grad_sym_install($$,
								     $1->name);
			radtest_var_copy (var, $1);
		}
              | send_flag_list send_flag
                {
			radtest_variable_t *var;
			var = (radtest_variable_t*) grad_sym_install($1,
								     $2->name);
			radtest_var_copy (var, $2); /* FIXME: check this */
		}
              ;

send_flag     : NAME EQ NUMBER
                {
			$$ = radtest_var_alloc(rtv_integer);
			$$->name = $1;
			$$->datum.number = $3;
		}
              ;

expr_or_pair_list: /* empty */
                {
			$$ = NULL;
		}
              | pair_list
                {
			radtest_variable_t *var = radtest_var_alloc(rtv_pairlist);
			var->datum.list = $1;
			$$ = radtest_node_alloc(radtest_node_value);
			$$->v.var = var;
		}
	      | expr
	      ;
     
cond          : bool
              | NOT cond
                {
			$$ = radtest_node_alloc(radtest_node_unary);
			$$->v.unary.op = radtest_op_not;
			$$->v.unary.operand = $2;
		}
              | '(' cond ')'
                {
			$$ = $2;
		}
              | cond AND cond
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_and;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | cond OR cond
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_or;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | cond EQ cond
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_eq;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | cond LT cond
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_lt;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | cond GT	cond
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_gt;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | cond NE	cond
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_ne;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | cond LE	cond
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_le;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | cond GE	cond
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_ge;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              ;

bool          : expr
              ;

expr          : value
              | '(' expr ')'
                {
			$$ = $2;
		}
              | expr '[' NAME closure ']' 
                {
			grad_dict_attr_t *dict = grad_attr_name_to_dict($3);
			if (!dict) {
				parse_error(_("Unknown attribute: %s"), $3);
				YYERROR;
			}
			$$ = radtest_node_alloc(radtest_node_attr);
			$$->v.attr.node = $1;
			$$->v.attr.dict = dict;
			$$->v.attr.all = $4;
			if ($4 && dict->type != TYPE_STRING) 
				parse_error(_("warning: '*' is meaningless for this attribute type"));
		}
              | expr '+' expr
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_add;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | expr '-' expr
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_sub;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | expr '*' expr
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_mul;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | expr '/' expr
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_div;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | expr '%' expr
                {
			$$ = radtest_node_alloc(radtest_node_bin);
			$$->v.bin.op = radtest_op_mod;
			$$->v.bin.left = $1;
			$$->v.bin.right = $3;
		}
              | '-' expr %prec UMINUS
                {
			$$ = radtest_node_alloc(radtest_node_unary);
			$$->v.unary.op = radtest_op_neg;
			$$->v.unary.operand = $2;
		}
              ;

maybe_expr    : /* empty */
                {
			$$ = NULL;
		}
              | expr
              ;

value         : imm_value
                {
			$$ = radtest_node_alloc(radtest_node_value);
			$$->v.var = $1;
		}
              | IDENT
                {
			$$ = radtest_node_alloc(radtest_node_deref);
			$$->v.deref = $1;
		}
              | PARM
                {
			$$ = radtest_node_alloc(radtest_node_parm);
			$$->v.parm = $1;
		}
              ;

closure       : /* empty */
                {
			$$ = 0;
		}
              | '*'
                {
			$$ = 1;
		}
              ;

imm_value     : NUMBER
                {
			$$ = radtest_var_alloc(rtv_integer);
			$$->datum.number = $1;
		}
              | IPADDRESS
                {
			$$ = radtest_var_alloc(rtv_ipaddress);
			$$->datum.ipaddr = $1;
		}
              | QUOTE
                {
			$$ = radtest_var_alloc(rtv_string);
			$$->datum.string = $1;
		}
              | NAME
                {
			$$ = radtest_var_alloc(rtv_string);
			$$->datum.string = $1;
		}
              | '(' pair_list ')'
                {
			$$ = radtest_var_alloc(rtv_pairlist);
			$$->datum.list = $2;
		}
              ;

pair_list     : pair
                {
			$$ = grad_list_create();
			grad_list_append($$, $1);
		}
              | pair_list pair
                {
			grad_list_append($1, $2);
			$$ = $1;
		}
              | pair_list ',' pair
                {
			grad_list_append($1, $3);
			$$ = $1;
		}
              | pair_list error
              ;

pair          : NAME op expr
                {
			grad_dict_attr_t *attr = grad_attr_name_to_dict($1);
			if (!attr) {
				parse_error(_("Unknown attribute '%s'"), $1);
				YYERROR;
			}
					    
			$$ = radtest_pair_alloc();
			$$->attr = attr;
			$$->op = $2;
			$$->node = $3;
		}
              ;

op            : EQ
                {
                        $$ = grad_operator_equal;
                } 
              | LT
                {
                        $$ = grad_operator_less_than;
                }
              | GT
                { 
                        $$ = grad_operator_greater_than;
                }
              | NE
                {
                        $$ = grad_operator_not_equal;
                }
              | LE
                {
                        $$ = grad_operator_less_equal;
                }
              | GE
                {
                        $$ = grad_operator_greater_equal;
                }
              ;

prlist        : pritem
                {
			$$ = grad_list_create();
			grad_list_append($$, $1);
		}
              | prlist pritem
                {
			grad_list_append($1, $2);
			$$ = $1;
		}
              ;

pritem        : expr 
              ;

%%

int
yyerror(char *s)
{
	parse_error(s);
}

void
parse_error(const char *fmt, ...)
{
        va_list ap;

	va_start(ap, fmt);
        fprintf(stderr, "%s:%lu: ", source_locus.file, source_locus.line);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, "\n");
	error_count++;
}

void
set_yydebug()
{
        extern int yydebug;
        if (debug_on(1)) {
                yydebug = 1;
        }
}

