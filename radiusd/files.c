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

/* This file deals with contents of /etc/raddb directory (except config and
   dictionaries) */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <sysdep.h>
#include <radiusd.h>
#include <checkrad.h>
#include <radius/raddbm.h>

typedef struct locus_name {
	struct locus_name *next;
	char *filename;
	size_t refcnt;
} Locus_symbol;

static grad_symtab_t *locus_tab;

void
locus_dup(grad_locus_t *dst, grad_locus_t *src)
{
	Locus_symbol *sp;
	
	if (!locus_tab)
                locus_tab = grad_symtab_create(sizeof(Locus_symbol), NULL);
	sp = (Locus_symbol*) grad_sym_lookup_or_install(locus_tab, src->file, 1);
	sp->refcnt++;
	dst->file = sp->filename;
	dst->line = src->line;
}

void
locus_free(grad_locus_t *loc)
{
	Locus_symbol *sp = (Locus_symbol*) grad_sym_lookup(locus_tab, loc->file);
	if (--sp->refcnt == 0)
		grad_symtab_delete(locus_tab, (grad_symbol_t*) sp);
}

/*
 * Symbol tables and lists
 */
grad_symtab_t          *user_tab;     /* raddb/users  */
grad_symtab_t          *deny_tab;     /* raddb/access.deny */

static grad_list_t /* of grad_matching_rule_t */  *huntgroups;   /* raddb/huntgroups */ 
static grad_list_t /* of grad_matching_rule_t */  *hints;        /* raddb/hints */
static grad_list_t /* of CLIENT */ *clients; /* raddb/clients */
static grad_list_t /* of RADCK_TYPE */ *radck_type;   /* raddb/nastypes */

static struct keyword op_tab[] = {
        { "=", grad_operator_equal },
        { "!=", grad_operator_not_equal },
        { ">", grad_operator_greater_than },
        { "<", grad_operator_less_than },
        { ">=", grad_operator_greater_equal },
        { "<=", grad_operator_less_equal },
        { 0 }
};

int paircmp(grad_request_t *req, grad_avp_t *check, char *pusername);
int fallthrough(grad_avp_t *vp);
/*
 * Static declarations
 */
static int portcmp(grad_avp_t *check, grad_avp_t *request);
static int groupcmp(grad_request_t *req, char *groupname, char *username);
static int uidcmp(grad_avp_t *check, char *username);
static void matchrule_free(grad_matching_rule_t **pl);
static int matches(grad_request_t *req, char *name, grad_matching_rule_t *pl, char *matchpart);
static int huntgroup_match(grad_request_t *req, char *huntgroup);
static int user_find_sym(char *name, grad_request_t *req, 
                         grad_avp_t **check_pairs, grad_avp_t **reply_pairs);
#ifdef USE_DBM
int user_find_db(char *name, grad_request_t *req,
                        grad_avp_t **check_pairs, grad_avp_t **reply_pairs);
#endif
static grad_list_t *file_read(int cf_file, char *name);

int
comp_op(enum grad_operator op, int result)
{
        switch (op) {
        default:
        case grad_operator_equal:
                if (result != 0)
                        return -1;
                break;

        case grad_operator_not_equal:
                if (result == 0)
                        return -1;
                break;

        case grad_operator_less_than:
                if (result >= 0)
                        return -1;
                break;

        case grad_operator_greater_than:
                if (result <= 0)
                        return -1;
                break;
                    
        case grad_operator_less_equal:
                if (result > 0)
                        return -1;
                break;
                        
        case grad_operator_greater_equal:
                if (result < 0)
                        return -1;
                break;
        }
        return 0;
}

/* ***************************************************************************
 * raddb/users
 */

/*
 * parser
 */
int
add_user_entry(void *closure, grad_locus_t *loc,
	       char *name, grad_avp_t *check, grad_avp_t *reply)
{
	grad_symtab_t *symtab = closure;
        User_symbol *sym;

        /* Special handling for DEFAULT names: strip any trailing
         * symbols
         */
        if (strncmp(name, "DEFAULT", 7) == 0) 
                name = "DEFAULT";
        if (strncmp(name, "BEGIN", 5) == 0) 
                name = "BEGIN";

        if ((check == NULL && reply == NULL)
            || fix_check_pairs(CF_USERS, loc, name, &check)
            || fix_reply_pairs(CF_USERS, loc, name, &reply)) {
                grad_log_loc(L_ERR, loc,
			     _("discarding user `%s'"),
			     name);
                grad_avl_free(check);
                grad_avl_free(reply);
                return 0;
        }

        sym = grad_sym_install(symtab, name);
        
        sym->check = check;
        sym->reply = reply;
        locus_dup(&sym->loc, loc);
        return 0;
}

static int
free_user_entry(User_symbol *sym)
{
	locus_free(&sym->loc);
        grad_avl_free(sym->check);
        grad_avl_free(sym->reply);
        return 0;
}

struct temp_data {
        int cf_file;
	grad_list_t *list;
};

int
add_pairlist(void *closure, grad_locus_t *loc,
	     char *name, grad_avp_t *lhs, grad_avp_t *rhs)
{
	struct temp_data *data = closure;
        grad_matching_rule_t *rule;
        
        if ((lhs == NULL && rhs == NULL)
            || fix_check_pairs(data->cf_file, loc, name, &lhs)
            || fix_reply_pairs(data->cf_file, loc, name, &rhs)) {
                grad_log_loc(L_ERR, loc, _("discarding entry `%s'"), name);
                grad_avl_free(lhs);
                grad_avl_free(rhs);
                return 0;
        }

        rule = grad_emalloc(sizeof(grad_matching_rule_t));
        rule->name = grad_estrdup(name);
        rule->lhs = lhs;
        rule->rhs = rhs;
        locus_dup(&rule->loc, loc);
	grad_list_append(data->list, rule);
        return  0;
}

int
read_users(char *name)
{
        if (!user_tab)
                user_tab = grad_symtab_create(sizeof(User_symbol), free_user_entry);
        return grad_parse_rule_file(name, user_tab, add_user_entry);
}

static grad_list_t *
file_read(int cf_file, char *name)
{
        struct temp_data tmp;

        tmp.cf_file = cf_file;
        tmp.list = grad_list_create();
        grad_parse_rule_file(name, &tmp, add_pairlist);
        return tmp.list;
}

enum lookup_state {
        LU_begin,
        LU_match,
        LU_default
};

typedef struct {
        char *name;
        enum lookup_state state;
        User_symbol *sym;
} USER_LOOKUP;


static User_symbol * user_lookup(char *name, USER_LOOKUP *lptr);
static User_symbol * user_next(USER_LOOKUP *lptr);

/*
 * Hash lookup
 */
User_symbol *
user_lookup(char *name, USER_LOOKUP *lptr)
{
        lptr->name = name;
        lptr->state = LU_begin;
        lptr->sym = grad_sym_lookup(user_tab, "BEGIN");
        return lptr->sym ? lptr->sym : user_next(lptr);
}

User_symbol *
user_next(USER_LOOKUP *lptr)
{
        if (lptr->sym && (lptr->sym = grad_sym_next((grad_symbol_t*)lptr->sym)))
                return lptr->sym;
        
        switch (lptr->state) {
        case LU_begin:
                lptr->sym = grad_sym_lookup(user_tab, lptr->name);
                if (lptr->sym) {
                        lptr->state = LU_match;
                        break;
                }
                /*FALLTHRU*/
        case LU_match:
                lptr->state = LU_default;
                lptr->sym = grad_sym_lookup(user_tab, "DEFAULT");
                break;
                
        case LU_default:
                break;
        }
        return lptr->sym;
}


static int match_user(User_symbol *sym, grad_request_t *req,
                      grad_avp_t **check_pairs, grad_avp_t **reply_pairs);

/*
 * Find matching profile in the hash table
 */
int
user_find_sym(char *name, grad_request_t *req,
              grad_avp_t **check_pairs, grad_avp_t **reply_pairs)
{
        int found = 0;
        User_symbol *sym;
        USER_LOOKUP lu;
        
        debug(1,("looking for %s", name));
        for (sym = user_lookup(name, &lu); sym; sym = user_next(&lu)) {
                if (match_user(sym, req, check_pairs, reply_pairs)) {
                        found = 1;
                        if (!fallthrough(sym->reply))
                                break;
                        debug(1, ("fall through"));
                        lu.sym = NULL; /* force jump to next state */
                }
        }
        debug(1, ("returning %d", found));
        return found;
}

int
match_user(User_symbol *sym, grad_request_t *req,
           grad_avp_t **check_pairs, grad_avp_t **reply_pairs)
{
        grad_avp_t *p;
        grad_avp_t *check_tmp;
        grad_avp_t *reply_tmp;
        int found;
        
        if (!sym)
                return 0;

        found = 0;
        do {
                check_tmp = grad_avl_dup(sym->check);
                radiusd_sql_check_attr_query(req, &check_tmp);
                if (paircmp(req, check_tmp, NULL)) {
                        grad_avl_free(check_tmp);
                        continue;
                }

		radius_req_register_locus(req, &sym->loc);
                found = 1;

                for (p = grad_avl_find(sym->check, DA_MATCH_PROFILE);
                     p; 
                     p = grad_avl_find(p->next, DA_MATCH_PROFILE)) {
                        debug(1, ("submatch: %s", p->avp_strvalue));

                        found = match_user(grad_sym_lookup(user_tab,
						      p->avp_strvalue),
                                           req, check_pairs, reply_pairs);
                }                       

                if (!found) {
                        grad_avl_free(check_tmp);
                        continue;
                }       

                reply_tmp = grad_avl_dup(sym->reply);
                grad_avl_merge(reply_pairs, &reply_tmp);
                grad_avl_merge(check_pairs, &check_tmp);
                radiusd_sql_reply_attr_query(req, reply_pairs);

                grad_avl_free(reply_tmp);
                grad_avl_free(check_tmp);

                for (p = grad_avl_find(sym->reply, DA_MATCH_PROFILE);
                     p;
                     p = grad_avl_find(p->next, DA_MATCH_PROFILE)) {
                        debug(1, ("next: %s", p->avp_strvalue));
                        match_user(grad_sym_lookup(user_tab, p->avp_strvalue),
                                   req, check_pairs, reply_pairs);
                }
                if (!fallthrough(sym->reply))
                        break;
                debug(1, ("fall through near %s:%lu",
			  sym->loc.file, (unsigned long) sym->loc.line));
        } while (sym = grad_sym_next((grad_symbol_t*)sym));

        return found;
}

/*
 * Find the named user in the database.  Create the
 * set of attribute-value pairs to check and reply with
 * for this user from the database. The password verification
 * is done by the caller. user_find() only compares attributes.
 */
int
user_find(char *name, grad_request_t *req,
          grad_avp_t **check_pairs, grad_avp_t **reply_pairs)
{
        int found = 0;

        /* 
         *      Check for valid input, zero length names not permitted 
         */
        if (name[0] == 0) {
                grad_log(L_ERR, _("zero length username not permitted"));
                return -1;
        }

        /*
         *      Find the entry for the user.
         */
#ifdef USE_DBM
        if (use_dbm) 
                found = user_find_db(name, req, check_pairs, reply_pairs);
        else
#endif
                found = user_find_sym(name, req, check_pairs, reply_pairs);

        /*
         *      See if we succeeded.
         */
        if (!found)
                return -1;

        /*
         *      Remove server internal parameters.
         */
        grad_avl_delete(reply_pairs, DA_FALL_THROUGH);
        grad_avl_delete(reply_pairs, DA_MATCH_PROFILE);
        
        return 0;
}

/*
 * Standalone parser for the output of Exec-Process-Wait calls
 */

/* States of the automaton
 */
#define PS_LHS 0     /* expect left-hand side*/
#define PS_OPS 1     /*  --"-- operation */
#define PS_RHS 2     /*  --"-- right=hand side */
#define PS_END 3     /*  --"-- end of input */

#define isws(c) (c == ' ' || c == '\t')
#define isdelim(c) (isws(c) || c == '\n' || c == ',' || c == '=')

/*
 * Obtain next token from the input string
 */

static int
nextkn(char **sptr, char *token, int toksize)
{
        char *start;
        
        /* skip whitespace */
        while (**sptr && isws(**sptr))
                ++(*sptr);
        if (!**sptr)
                return -1;
        start = token;
        if (**sptr == '"') {
                (*sptr)++;
                while (toksize && **sptr) {
                        if (**sptr == '\\' && (*sptr)[1]) {
                                switch (*++*sptr) {
                                default:
                                        *token++ = **sptr;
                                        break;
                                case 'a':
                                        *token++ = '\a';
                                        break;
                                case 'b':
                                        *token++ = '\b';
                                        break;
                                case 'f':
                                        *token++ = '\f';
                                        break;
                                case 'n':
                                        *token++ = '\n';
                                        break;
                                case 'r':
                                        *token++ = '\r';
                                        break;
                                case 't':
                                        *token++ = '\t';
                                        break;
                                case 'v':
                                        *token++ = '\v';
                                }
                                ++*sptr;
                                toksize--;
                        } else if (**sptr == '"') {
                                ++*sptr;
                                break;
                        } else 
                                *token++ = *(*sptr)++;
                }
        } else if (**sptr == ',' || **sptr == '=' || **sptr == '\n') {
                *token++ = *(*sptr)++;
        } else {
                while (toksize && **sptr && !isdelim(**sptr)) {
                        *token++ = *(*sptr)++;
                        toksize--;
                }
        }
        *token = 0;
        return start[0];
}

/*
 * Parse buffer as a pairlist. Put resulting pairs into the variable pointed
 * to by first_pair. 
 * Return 0 if OK, otherwise return -1 and put error message (if any)
 * in errmsg.
 */
int
userparse(char *buffer, grad_avp_t **first_pair, char **errmsg)
{
        int             state;
        grad_dict_attr_t *attr = NULL;
        grad_avp_t      *pair;
        int             op;
        static char errbuf[512];
        char token[256];
	grad_locus_t loc;

	loc.file = "<stdin>"; /* FIXME */
	loc.line = 0;
	
        state = PS_LHS;
        while (nextkn(&buffer, token, sizeof(token)) >= 0) {
                switch (state) {
                case PS_LHS:
                        if (token[0] == '\n' || token[0] == '#')
                                continue;
                        if (!(attr = grad_attr_name_to_dict(token))) {
                                snprintf(errbuf, sizeof(errbuf),
                                        _("unknown attribute `%s/%s'"), 
                                        token, buffer);
                                *errmsg = errbuf; 
                                return -1;
                        }
                        state = PS_OPS;
                        break;
                        
                case PS_OPS:
                        op = grad_xlat_keyword(op_tab, token, -1);
                        if (op == -1) {
                                snprintf(errbuf, sizeof(errbuf),
					 _("expected %s, but found %s"),
/* TRANSLATORS: This is used in place of the first %s in the message
   "expected %s, but found %s", i.e. it acts as a direct object to
   the verb 'expect'. Please bear this in mind when translating */
					 _("opcode"), token);
                                *errmsg = errbuf; 
                                return -1;
                        }
                        state = PS_RHS;
                        break;
                        
                case PS_RHS:
                        pair = grad_create_pair(&loc, attr->name, op, token);
                        if (!pair) {
                                snprintf(errbuf, sizeof(errbuf),
                                         _("grad_create_pair failed on %s"),
                                         attr->name);
                                return -1;
                        }
                        grad_avl_merge(first_pair, &pair);
                        state = PS_END;
                        break;
                        
                case PS_END:
                        if (token[0] != ',' && token[0] != '\n') {
                                snprintf(errbuf, sizeof(errbuf),
					 _("expected %s, but found %s"),
					 "','", token);
                                *errmsg = errbuf;
                                return -1;
                        }
                        state = PS_LHS;
                        break;
                }
        }
        return 0;
}

/* ***************************************************************************
 * raddb/hints
 */

/* Provide a support for backward-compatible attributes Replace-User-Name
   and Rewrite-Function */
static void
hints_eval_compat(grad_request_t *req, grad_avp_t *name_pair, grad_matching_rule_t *rule)
{
        grad_avp_t      *tmp;

	/* Let's see if we need to further modify the username */
	if ((tmp = grad_avl_find(rule->rhs, DA_REPLACE_USER_NAME))
	    || (tmp = grad_avl_find(rule->lhs, DA_REPLACE_USER_NAME))) {
		char *ptr;
		struct obstack hints_stk;
 
		obstack_init(&hints_stk);
		ptr = radius_xlate(&hints_stk, tmp->avp_strvalue,
				   req, NULL);
		if (ptr) 
			grad_string_replace(&name_pair->avp_strvalue, ptr);
		obstack_free(&hints_stk, NULL);
	}
                
	/* Is the rewrite function specified? */
	if ((tmp = grad_avl_find(rule->rhs, DA_REWRITE_FUNCTION))
	    || (tmp = grad_avl_find(rule->lhs, DA_REWRITE_FUNCTION))) {
		if (rewrite_eval(tmp->avp_strvalue, req, NULL, NULL)) {
			grad_log_loc(L_ERR, &rule->loc, "%s(): %s",
				     tmp->avp_strvalue,
				     _("not defined"));
		}
	}
}

/* Add hints to the info sent by the terminal server
   based on the pattern of the username and the contents of the incoming
   request */
int
hints_setup(grad_request_t *req)
{
        grad_avp_t      *request_pairs = req->request;
        char            newname[AUTH_STRING_LEN];
        grad_avp_t      *name_pair;
        grad_avp_t      *orig_name_pair;
        grad_avp_t      *tmp;
        grad_matching_rule_t   *rule;
        int             matched = 0;
	grad_iterator_t *itr;
	
        /* Add Proxy-Replied pair if necessary */
        switch (req->code) {
        case RT_ACCESS_ACCEPT:
        case RT_ACCESS_REJECT:
        case RT_ACCOUNTING_RESPONSE:
        case RT_ACCESS_CHALLENGE:
                tmp = grad_avp_create_integer(DA_PROXY_REPLIED, 1);
                grad_avl_merge(&request_pairs, &tmp);
                grad_avp_free(tmp);
                break;

        case RT_STATUS_SERVER:
                return 0;
        }
        
        if (hints == NULL)
                return 0;

        /* 
         *      Check for valid input, zero length names not permitted 
         */
        if ((name_pair = grad_avl_find(request_pairs, DA_USER_NAME)) == NULL) {
                name_pair = grad_avp_create_string(DA_USER_NAME, "");
                orig_name_pair = NULL;
        } else {
                orig_name_pair = grad_avp_create_string(DA_ORIG_USER_NAME,
						      name_pair->avp_strvalue);
        }

        debug(1, ("called for `%s'", name_pair->avp_strvalue));
        
        /* if Framed-Protocol is present but Service-Type is missing, add
           Service-Type = Framed-User. */
        if (grad_avl_find(request_pairs, DA_FRAMED_PROTOCOL) != NULL &&
            grad_avl_find(request_pairs, DA_SERVICE_TYPE) == NULL) {
                tmp = grad_avp_create_integer(DA_SERVICE_TYPE,
					      DV_SERVICE_TYPE_FRAMED_USER);
                if (tmp) 
                        grad_avl_merge(&request_pairs, &tmp);
        }

	itr = grad_iterator_create(hints);
	for (rule = grad_iterator_first(itr); rule; rule = grad_iterator_next(itr)) {
                int do_strip;
                grad_avp_t *add;
                
                if (matches(req, name_pair->avp_strvalue, rule, newname))
                        continue;

                matched++;
                radius_req_register_locus(req, &rule->loc);
		
                debug(1, ("matched %s at %s:%lu", rule->name, rule->loc.file,
			  (unsigned long) rule->loc.line));
        
                add = grad_avl_dup(rule->rhs);
        
                /* See if we need to adjust the name. */
                do_strip = 1;
                if ((tmp = grad_avl_find(rule->rhs, DA_STRIP_USER_NAME))
		         != NULL
                    || (tmp = grad_avl_find(rule->lhs, DA_STRIP_USER_NAME))
		            != NULL)
                        do_strip = tmp->avp_lvalue;
                
                if (do_strip) 
                        grad_string_replace(&name_pair->avp_strvalue, newname);

		/* Handle Rewrite-Function and Replace-User-Name */
		hints_eval_compat(req, name_pair, rule);
		/* Evaluate hints */
		radius_eval_avl(req, add); /*FIXME: return value?*/
		
                debug(1, ("new name is `%s'", name_pair->avp_strvalue));

                /* fix-up the string length */
                name_pair->avp_strlength = strlen(name_pair->avp_strvalue);

                /* Add all attributes to the request list, except
		   DA_STRIP_USER_NAME and DA_REPLACE_USER_NAME */
                grad_avl_delete(&add, DA_STRIP_USER_NAME);
                grad_avl_delete(&add, DA_REPLACE_USER_NAME);
                grad_avl_delete(&add, DA_REWRITE_FUNCTION);
                grad_avl_merge(&request_pairs, &add);
                grad_avl_free(add);
                
                /* Ok, let's see if we need to further check the
                   hint's rules */
                if (((tmp = grad_avl_find(rule->rhs, DA_FALL_THROUGH))
		          != NULL
                     || (tmp = grad_avl_find(rule->lhs, DA_FALL_THROUGH))
		             != NULL)
                    && tmp->avp_lvalue)
                        continue;
                break;
        }

	grad_iterator_destroy(&itr);
	
        if (matched) {
                if (orig_name_pair)
                        grad_avl_add_pair(&request_pairs, orig_name_pair);
		/* A rewrite function might have installed the new
		   User-Name. Take care not to discard it. */
                else if (!grad_avl_find(request_pairs, DA_USER_NAME))
                        grad_avl_add_pair(&request_pairs, name_pair);
		else
			grad_avp_free(name_pair);
        } else {
                if (orig_name_pair)
                        grad_avp_free(orig_name_pair);
                else
                        grad_avp_free(name_pair);
        }

        req->request = request_pairs;
        
        return 0;
}

/* ***************************************************************************
 * raddb/huntgroups
 */

/*
 * See if the huntgroup matches.
 */
int
huntgroup_match(grad_request_t *req, char *huntgroup)
{
        grad_matching_rule_t *rule;
	grad_iterator_t *itr;

        if (!huntgroups)
		return 0;
	itr = grad_iterator_create(huntgroups);
	for (rule = grad_iterator_first(itr); rule; rule = grad_iterator_next(itr)) {
                if (strcmp(rule->name, huntgroup) != 0)
                        continue;
                if (paircmp(req, rule->lhs, NULL) == 0) {
                        debug(1, ("matched %s at %s:%lu",
				  rule->name, rule->loc.file,
				  (unsigned long) rule->loc.line));
                        break;
                }
        }
	grad_iterator_destroy(&itr);
        return (rule != NULL);
}


/*
 * See if we have access to the huntgroup.
 * Return:  0 if we don't have access.
 *          1 if we do have access.
 *         -1 on error.
 */
int
huntgroup_access(grad_request_t *radreq, grad_locus_t *loc)
{
        grad_avp_t      *pair;
        grad_matching_rule_t   *rule;
	grad_iterator_t *itr;
        int             r = 1;

        if (huntgroups == NULL)
                return 1;

	itr = grad_iterator_create(huntgroups);
	for (rule = grad_iterator_first(itr); rule; rule = grad_iterator_next(itr)) {
                /*
                 *      See if this entry matches.
                 */
                if (paircmp(radreq, rule->lhs, NULL) != 0)
                        continue;
                debug(1, ("matched huntgroup at %s:%lu", rule->loc.file,
			  (unsigned long) rule->loc.line));
                r = paircmp(radreq, rule->rhs, NULL) == 0;
                break;
        }
	grad_iterator_destroy(&itr);
	
        if (rule) {
		if (loc)
			*loc = rule->loc;
		radius_req_register_locus(radreq, &rule->loc);
#ifdef DA_REWRITE_FUNCTION
		if ((pair = grad_avl_find(rule->lhs, DA_REWRITE_FUNCTION))
		          != NULL
		    && rewrite_eval(pair->avp_strvalue, radreq, NULL, NULL)) {
                        grad_log_loc(L_ERR, &rule->loc, "%s(): %s",
				     pair->avp_strvalue,
				     _("not defined"));
                }
#endif  
	}
        debug(1, ("returning %d", r));
        return r;
}

int
read_naslist_file(char *file)
{
#ifdef USE_SNMP 
        snmp_init_nas_stat();
#endif
        return grad_nas_read_file(file);
}

/* ***************************************************************************
 * raddb/clients
 */

/*
 * parser
 */
/*ARGSUSED*/
int
read_clients_entry(void *u ARG_UNUSED, int fc, char **fv, grad_locus_t *loc)
{
        CLIENT *cp;
        
        if (fc != 2) {
                grad_log_loc(L_ERR, loc, "%s", _("wrong number of fields"));
                return -1;
        }

        cp = grad_emalloc(sizeof(CLIENT));

	if (strcmp(fv[0], "DEFAULT") == 0) 
		cp->netdef.ipaddr = cp->netdef.netmask = 0;
	else
		grad_ip_getnetaddr(fv[0], &cp->netdef);
        cp->secret = grad_estrdup(fv[1]);
        if (fc == 3)
                GRAD_STRING_COPY(cp->shortname, fv[2]);
        grad_ip_gethostname(cp->netdef.ipaddr, cp->longname, sizeof(cp->longname));
        grad_list_append(clients, cp);
        return 0;
}

static int
client_free(void *item, void *data ARG_UNUSED)
{
        CLIENT *cl = item;
        grad_free(cl->secret);
        grad_free(cl);
        return 0;
}

/*
 * Read the clients file.
 */
int
read_clients_file(char *file)
{
        grad_list_destroy(&clients, client_free, NULL);
        clients = grad_list_create();
        return grad_read_raddb_file(file, 1, NULL, read_clients_entry, NULL);
}


/*
 * Find a client in the CLIENTS list.
 */
CLIENT *
client_lookup_ip(grad_uint32_t ipaddr)
{
        CLIENT *cl;
        grad_iterator_t *itr = grad_iterator_create(clients);

        if (!itr)
                return NULL;
        for (cl = grad_iterator_first(itr); cl; cl = grad_iterator_next(itr))
		if (grad_ip_in_net_p(&cl->netdef, ipaddr)) {
			char ipbuf[DOTTED_QUAD_LEN];
			char maskbuf[DOTTED_QUAD_LEN];

			debug(10,
			      ("Found secret for %s/%s (%s): %s",
			       grad_ip_iptostr(cl->netdef.ipaddr, ipbuf),
			       grad_ip_iptostr(cl->netdef.netmask, maskbuf),
			       cl->longname,
			       cl->secret));  
			break;
		}
	
        grad_iterator_destroy(&itr);
        return cl;
}


/*
 * Find the name of a client (prefer short name).
 */
char *
client_lookup_name(grad_uint32_t ipaddr, char *buf, size_t bufsize)
{
        CLIENT *cl;

        if ((cl = client_lookup_ip(ipaddr)) != NULL) {
                if (cl->shortname[0])
                        return cl->shortname;
                else
                        return cl->longname;
        }
        return grad_ip_gethostname(ipaddr, buf, bufsize);
}

/* ****************************************************************************
 * raddb/nastypes
 */


/*
 * parser
 */
/*ARGSUSED*/
int
read_nastypes_entry(void *u ARG_UNUSED, int fc, char **fv, grad_locus_t *loc)
{
        RADCK_TYPE *mp;
        int method;
	int i;
	
        if (fc < 2) {
                grad_log_loc(L_ERR, loc, "%s", _("too few fields"));
                return -1;
        }

        if (strcmp(fv[1], "finger") == 0)
                method = METHOD_FINGER;
        else if (strcmp(fv[1], "snmp") == 0)
                method = METHOD_SNMP;
        else if (strcmp(fv[1], "ext") == 0)
                method = METHOD_EXT;
	else if (strcmp(fv[1], "guile") == 0)
		method = METHOD_GUILE;
        else {
                grad_log_loc(L_ERR, loc, "%s", _("unknown method"));
                return -1;
        }
                        
        mp = grad_emalloc(sizeof(*mp));
        mp->type = grad_estrdup(fv[0]);
        mp->method = method;
	mp->args = NULL;

	for (i = 2; i < fc; i++) {
		if (fv[i][0] == ',')
			continue;
		
		if (fc - i > 2 && fv[i+1][0] == '=') {
			grad_envar_assign(fv[i], fv[i+2], &mp->args);
			i += 2;
		} else {
			grad_envar_assign(fv[i], NULL, &mp->args);
		}
	}
	grad_list_append(radck_type, mp);
        return 0;
}
        
static int
free_radck_type(void *item, void *data ARG_UNUSED)
{
        RADCK_TYPE *rp = item;

        grad_free(rp->type);
        grad_envar_free_list(&rp->args);
	grad_free(rp);
	return 0;
}

int
read_nastypes_file(char *file)
{
	grad_list_destroy(&radck_type, free_radck_type, NULL);
	radck_type = grad_list_create();
        return grad_read_raddb_file(file, 0, ",=", read_nastypes_entry, NULL);
}

RADCK_TYPE *
find_radck_type(char *name)
{
        RADCK_TYPE *tp;
       	grad_iterator_t *itr = grad_iterator_create(radck_type);

        if (!itr)
        	return NULL;
        for (tp = grad_iterator_first(itr);
	     tp && strcmp(tp->type, name);
	     tp = grad_iterator_next(itr))
                ;
        grad_iterator_destroy(&itr);
        return tp;
}
                

/* ****************************************************************************
 * raddb/access.deny
 */

/*
 * parser
 */
void
add_deny(char *user)
{
        grad_sym_install(deny_tab, user);
}

/*ARGSUSED*/
int
read_denylist_entry(void *closure, int fc, char **fv, grad_locus_t *loc)
{
	int *denycnt = closure;
	if (fc != 1) {
                grad_log_loc(L_ERR, loc, "%s", _("wrong number of fields"));
                return -1;
        }

        if (get_deny(fv[0]))
                grad_log_loc(L_ERR, loc, _("user `%s' already found in %s"),
			   fv[0], RADIUS_DENY);
        else {
                add_deny(fv[0]);
                (*denycnt)++;
        }
        return 0;
}

void
read_deny_file()
{
        int denycnt;
        char *name;
        
        name = grad_mkfilename(radius_dir, RADIUS_DENY);
        if (deny_tab)
                grad_symtab_clear(deny_tab);
        else
                deny_tab = grad_symtab_create(sizeof(grad_symbol_t), NULL);
        denycnt = 0;

        grad_read_raddb_file(name, 0, NULL, read_denylist_entry, &denycnt);
        grad_free(name);
        if (denycnt)
                grad_log(L_INFO,
		         ngettext ("%d user disabled",
			  	   "%d users disabled", denycnt), denycnt);
}

/*
 * Return 1 if the given user should be denied access
 */
int
get_deny(char *user)
{
        return grad_sym_lookup(deny_tab, user) != NULL;
}


/* ***************************************************************************
 * Various utilities, local to this module
 */

/*
 *      See if a grad_avp_t list contains Fall-Through = Yes
 */
int
fallthrough(grad_avp_t *vp)
{
        grad_avp_t *tmp;

        return (tmp = grad_avl_find(vp, DA_FALL_THROUGH)) ?
		         tmp->avp_lvalue : 0;
}

/*
 *      Compare a portno with a range.
 */
int
portcmp(grad_avp_t *check, grad_avp_t *request)
{
        char buf[AUTH_STRING_LEN];
        char *s, *p, *save;
        int lo, hi;
        int port = request->avp_lvalue;

        strcpy(buf, check->avp_strvalue);
        s = strtok_r(buf, ",", &save);
        while(s) {
                if ((p = strchr(s, '-')) != NULL)
                        p++;
                else
                        p = s;
                lo = atoi(s);
                hi = atoi(p);
                if (lo <= port && port <= hi) {
                        return 0;
                }
                s = strtok_r(NULL, ",", &save);
        }

        return -1;
}


int
uidcmp(grad_avp_t *check, char *username)
{
        struct passwd pw, *pwd;
	char buffer[512];

        if (!(pwd = grad_getpwnam_r(username, &pw, buffer, sizeof buffer)))
                return -1;

        return pwd->pw_uid - check->avp_lvalue;
}

static int
system_groupcmp(grad_request_t *req, char *groupname, char *username)
{
        struct passwd pw, *pwd;
        struct group *grp;
        char **member;
        char pwbuf[512];
        int retval;

        if (!(pwd = grad_getpwnam_r(username, &pw, pwbuf, sizeof pwbuf)))
                return 1;

        if (!(grp = grad_getgrnam(groupname)))
                return 1;

        retval = !(pwd->pw_gid == grp->gr_gid);
	for (member = grp->gr_mem; *member && retval; member++) {
		if (strcmp(*member, pwd->pw_name) == 0)
			retval = 0;
	}
	grad_free(grp);
	
	return retval;
}

/* Return 0 if user `username' is a member of group `groupname' */
int
groupcmp(grad_request_t *req, char *groupname, char *username)
{
        int retval;

	return system_groupcmp(req, groupname, username)
		&& radiusd_sql_checkgroup(req, groupname);
}

/*
 *      Compare prefix/suffix.
 */
int
presufcmp(grad_avp_t *check, char *name, char *rest)
{
        int len, namelen;
        int ret = -1;

        debug(1, ("comparing %s and %s, check->attr is %d",
                 name, check->avp_strvalue, check->attribute));

        len = strlen(check->avp_strvalue);
        switch (check->attribute) {
                case DA_PREFIX:
                        ret = strncmp(name, check->avp_strvalue, len);
                        if (ret == 0 && rest)
                                strcpy(rest, name + len);
                        break;
                case DA_SUFFIX:
                        namelen = strlen(name);
                        if (namelen < len)
                                break;
                        ret = strcmp(name + namelen - len, check->avp_strvalue);
                        if (ret == 0 && rest) {
                                strncpy(rest, name, namelen - len);
                                rest[namelen - len] = 0;
                        }
                        break;
        }
        debug(1, ("returning %d", ret));
        return ret;
}

/*
 * Attributes we skip during comparison.
 * These are "server" check items.
 */
static int server_check_items[] = {
        DA_EXPIRATION,
        DA_LOGIN_TIME,
        DA_USER_PASSWORD,
        DA_CRYPT_PASSWORD,
        DA_PASSWORD_LOCATION,
        DA_AUTH_TYPE,
        DA_PAM_AUTH,
        DA_SIMULTANEOUS_USE,
        DA_STRIP_USER_NAME,
        DA_REPLACE_USER_NAME,
#ifdef DA_REWRITE_FUNCTION
        DA_REWRITE_FUNCTION,
#endif  
#ifdef DA_ACCT_TYPE
        DA_ACCT_TYPE,
#endif
#ifdef DA_LOG_MODE_MASK
        DA_LOG_MODE_MASK,
#endif
        DA_MENU,
        DA_TERMINATION_MENU,
        DA_GROUP_NAME,
        DA_MATCH_PROFILE,
        DA_AUTH_DATA
};

int
server_attr(int attr)
{
        int i;

        for (i = 0; i < NITEMS(server_check_items); i++) 
                if (server_check_items[i] == attr)
                        return 1;       
        return 0;
}

/*
 * Compare two pair lists except for the internal server items.
 * Return 0 on match.
 */
int
paircmp(grad_request_t *request, grad_avp_t *check, char *pusername)
{
        grad_avp_t *check_item = check;
        grad_avp_t *auth_item;
        char username[AUTH_STRING_LEN];
        int result = 0;
        int compare;
        char *save;

	if (!pusername)
		pusername = username;
	
        while (result == 0 && check_item != NULL) {
                if (server_attr(check_item->attribute)) {  
                        check_item = check_item->next;
                        continue;
                }

                if (debug_on(20)) {
                        grad_log(L_DEBUG, 
                                 "check_item: %s", 
                                 grad_format_pair(check_item, 1, &save));
                        free(save);
                }

                /*
                 *      See if this item is present in the request.
                 */
                for (auth_item = request->request; auth_item; 
                                auth_item = auth_item->next) {
                        debug(30, ("trying %d", auth_item->attribute));

                        switch (check_item->attribute) {
                        case DA_PREFIX:
                        case DA_SUFFIX:
                        case DA_GROUP_NAME:
                        case DA_GROUP:
                                if (auth_item->attribute != DA_USER_NAME)
                                        continue;
                                /*FALLTHRU*/
				
                        case DA_HUNTGROUP_NAME:
                        case DA_USER_UID:
                                break;
				
                        case DA_HINT:
                                if (auth_item->attribute != check_item->attribute)
                                        continue;
                                if (strcmp(check_item->avp_strvalue,
                                           auth_item->avp_strvalue) != 0)
                                        continue;
                                break;
				
                        default:
                                if (auth_item->attribute !=
                                    check_item->attribute)
                                        continue;
                        }
                        break;
                }
                if (auth_item == NULL) {
                        result = -1;
                        continue;
                }

                if (debug_on(20)) {
                        grad_log(L_DEBUG,
                                 "auth_item: %s",
                                 grad_format_pair(auth_item, 1, &save));
                        free(save);
                }

                /*
                 *      OK it is present now compare them.
                 */
                
                compare = 0;    /* default result */
                switch (check_item->type) {
                case TYPE_STRING:
                        switch (check_item->attribute) {
                        case DA_PREFIX:
                        case DA_SUFFIX:
                                strcpy(pusername, auth_item->avp_strvalue);
                                compare = presufcmp(check_item,
                                                    auth_item->avp_strvalue,
                                                    pusername);
                                break;
				
                        case DA_NAS_PORT_ID:
                                compare = portcmp(check_item, auth_item);
                                break;
				
                        case DA_GROUP_NAME:
                        case DA_GROUP:
                                strcpy(username, auth_item->avp_strvalue);
                                compare = groupcmp(request,
                                                   check_item->avp_strvalue,
                                                   username);
                                break;
				
                        case DA_HUNTGROUP_NAME:
                                compare = !huntgroup_match(request,
                                                         check_item->avp_strvalue);
                                break;
				
                        default:
                                compare = strcmp(auth_item->avp_strvalue,
                                                 check_item->avp_strvalue);
                        }
                        break;

                case TYPE_INTEGER:
                        switch (check_item->attribute) {
                        case DA_USER_UID:
                                compare = uidcmp(check_item, username);
                                break;
                        }
                        /*FALLTHRU*/
			
                case TYPE_IPADDR:
                        compare = auth_item->avp_lvalue - check_item->avp_lvalue;
                        break;
                        
                default:
                        return -1;
                        break;
                }

                debug(20, ("compare: %d", compare));

                result = comp_op(check_item->operator, compare);

                if (result == 0)
                        check_item = check_item->next;
        }

        debug(20, ("returning %d", result));
        return result;
}

/*
 * Free a grad_matching_rule_t
 */
static int
matching_rule_free(void *item, void *data ARG_UNUSED)
{
	grad_matching_rule_t *p = item;
	
	if (p->name)
		grad_free(p->name);
	if (p->lhs)
		grad_avl_free(p->lhs);
	if (p->rhs)
		grad_avl_free(p->rhs);
	grad_free(p);
}

/* ***************************************************************************
 * a *very* restricted version  of wildmat
 */
char *
wild_start(char *str)
{
        char *p;

        p = str;
        while (*p) {
                switch (*p) {
                case '*':
                case '?':
                        return p;
                        
                case '\\':
                        if (p[1] == '(' || p[1] == ')')
                                return p;
                        /*FALLTHRU*/
                default:
                        p++;
                }
        }
        return NULL;
}

int
match_any_chars(char **expr, char **name)
{
        char *exprp, *expr_start, *p, *namep;
        int length;
        
        exprp = *expr;
        while (*exprp && *exprp == '*')
                exprp++;
        
        expr_start = exprp;
        while (exprp[0] == '\\' && (exprp[1] == '(' || exprp[1] == ')'))
                exprp += 2;
        
        p = wild_start(exprp);
        
        if (p) 
                length = p - exprp;
        else
                length = strlen(exprp);

        if (length == 0) {
                *name += strlen(*name);
        } else {
                namep = *name + strlen(*name) - 1;
                while (namep > *name) {
                        if (strncmp(namep, exprp, length) == 0) {
                                *name = namep;
                                break;
                        }
                        namep--;
                }
        }
        *expr = (exprp == expr_start) ? p : expr_start;
        return 0;
}

int
wild_match(char *expr, char *name, char *return_name)
{
        char *curp;
        char *start_pos, *end_pos;
        int c;
        
        strcpy(return_name, name);
        start_pos = end_pos = NULL;
        curp = name;
        while (expr && *expr) {
                switch (*expr) {
                case '*':
                        expr++;
                        if (match_any_chars(&expr, &curp))
                                return curp - name + 1;
                        break;
                        
                case '?':
                        expr++;
                        if (*curp == 0)
                                return curp - name + 1;
                        curp++;
                        break;
                        
                case '\\':
                        if (expr[1] == 0) 
                                goto def;
                        c = *++expr; expr++;
                        if (c == '(') {
                                start_pos = curp;
                        } else if (c == ')') {
                                end_pos = curp;
                                if (start_pos) {
                                        int len = end_pos - start_pos;
                                        strncpy(return_name, start_pos, len);
                                        return_name += len;
                                        *return_name = 0;
                                }
                        } else {
                                if (*curp != c)
                                        return curp - name + 1;
                                curp++;
                        }
                        break;
                        
                default:
                def:
                        if (*expr != *curp)
                                return curp - name + 1;
                        expr++;
                        curp++;
                }
        }
        return *curp != 0;
}

/* ************************************************************************* */

/*
 * Match a username with a wildcard expression.
 */
int
matches(grad_request_t *req, char *name, grad_matching_rule_t *pl, char *matchpart)
{
	memcpy(matchpart, name, AUTH_STRING_LEN);
        if (strncmp(pl->name, "DEFAULT", 7) == 0
	    || wild_match(pl->name, name, matchpart) == 0)
                return paircmp(req, pl->lhs, matchpart);
        return 1;
}       
        
/* ****************************************************************************
 * Read all configuration files
 */

#if USE_DBM
/*
 *      See if a potential DBM file is present.
 */
static int
checkdbm(char *users, char *ext)
{
        char *buffer;
        struct stat st;
        int rc;
        
        buffer = grad_emalloc(strlen(users) + strlen(ext) + 1);
        strcat(strcpy(buffer, users), ext);
        rc = stat(buffer, &st);
        grad_free(buffer);
        return rc;
}
#endif

static int reload_data(enum reload_what what, int *do_radck);

static int
realm_set_secret(grad_server_t *srv)
{
	CLIENT *client;

	if ((client = client_lookup_ip(srv->addr)) == NULL) 
		return 1;
	srv->secret = client->secret;
	return 0;
}

int
reload_data(enum reload_what what, int *do_radck)
{
        char *path;
        int   rc = 0;
        
        switch (what) {
        case reload_all:
		/* reload_sql implies reloading dictionaries
		   and matching rules (users, hints, huntgroups) */
                rc += reload_data(reload_sql, do_radck);
		
                rc += reload_data(reload_clients, do_radck);
                rc += reload_data(reload_naslist, do_radck);
                rc += reload_data(reload_realms, do_radck);
                rc += reload_data(reload_deny, do_radck);
		break;

        case reload_users:
                grad_symtab_clear(user_tab);
                path = grad_mkfilename(radius_dir, RADIUS_USERS);
        
#if USE_DBM
                if (use_dbm && radius_mode != MODE_BUILDDBM) {
                        if (access(path, 0) == 0) {
                                grad_log(L_WARN,
                                         _("using only dbm: USERS NOT LOADED"));
                        }
                        *do_radck = 0;
                } else {
                        if (radius_mode != MODE_BUILDDBM
                            && (checkdbm(path, ".dir") == 0
                                || checkdbm(path, ".db") == 0))
                                grad_log(L_WARN,
                    _("DBM files found but no -b flag given - NOT using DBM"));
                
#endif
                if (read_users(path)) {
                        grad_log(L_CRIT, _("can't load %s: exited"), path);
                        exit(1);
                } else
                        grad_log(L_INFO, _("%s reloaded."), path);
                *do_radck = 1;
#if USE_DBM
                }
#endif
                grad_free(path);
                break;

        case reload_dict:
                /* Non-zero result from dict_init means there was some real
                 * trouble.
                 */
                if (grad_dict_init())
                        rc = 1;

                /* reload_rewrite implies reloading users, huntgroups
		   and hints */
                rc += reload_data(reload_rewrite, do_radck);
                break;
                
        case reload_huntgroups:
                grad_list_destroy(&huntgroups, matching_rule_free, NULL);
                path = grad_mkfilename(radius_dir, RADIUS_HUNTGROUPS);
                huntgroups = file_read(CF_HUNTGROUPS, path);
                grad_free(path);
                break;
                
        case reload_hints:
                grad_list_destroy(&hints, matching_rule_free, NULL);
                path = grad_mkfilename(radius_dir, RADIUS_HINTS);
                hints = file_read(CF_HINTS, path);
                grad_free(path);
                if (!use_dbm) 
                        *do_radck = 1;
                break;
                
        case reload_clients:
                path = grad_mkfilename(radius_dir, RADIUS_CLIENTS);
                if (read_clients_file(path) < 0)
                        rc = 1;
                grad_free(path);
                break;

        case reload_naslist:
                /*FIXME*/
		path = grad_mkfilename(radius_dir, RADIUS_NASTYPES);
                read_nastypes_file(path);
                grad_free(path);
                /*EMXIF*/

                path = grad_mkfilename(radius_dir, RADIUS_NASLIST);
                if (read_naslist_file(path) < 0)
                        rc = 1;
                grad_free(path);
                break;

        case reload_realms:
                path = grad_mkfilename(radius_dir, RADIUS_REALMS);
                if (grad_read_realms(path, auth_port, acct_port,
				     realm_set_secret) < 0)
                        rc = 1;
                grad_free(path);
                break;
                
        case reload_deny:
                read_deny_file();
                break;

        case reload_sql:
                if (radiusd_sql_config() != 0) {
                        grad_log(L_CRIT,
                           _("SQL Error: SQL client could not be initialized"));
                        rc = 1;
                } 
                /* This implies reloading rewrite, users, huntgroups 
		   and hints */
                rc += reload_data(reload_dict, do_radck);
                break;

        case reload_rewrite:
		rewrite_load_all();
                rc += reload_data(reload_users, do_radck);
                rc += reload_data(reload_huntgroups, do_radck);
                rc += reload_data(reload_hints, do_radck);
                break;
                
        default:
                grad_log(L_CRIT, _("INTERNAL ERROR: unknown reload code: %d"),
                         what);
        }
                
        return rc;
}

int
reload_config_file(enum reload_what what)
{
        int do_radck;
        int rc;

        rc = reload_data(what, &do_radck);
        if (rc == 0 && do_radck)
                radck();
        return rc;
}

/* ****************************************************************************
 * Debugging functions
 */
void
dump_matching_rules(FILE *fp, char *header, grad_list_t *list)
{
	grad_matching_rule_t *rule;
	grad_iterator_t *itr = grad_iterator_create(list);

        fprintf(fp, "%s {\n", header);
 	for (rule = grad_iterator_first(itr); rule; rule = grad_iterator_next(itr)) {
                fprintf(fp, "\t%s:\n", rule->name);
                fprintf(fp, "\tlhs {\n");
		grad_avl_fprint(fp, "\t\t", 1, rule->lhs);
                fprintf(fp, "\t}\n");

                fprintf(fp, "\trhs {\n"); 
		grad_avl_fprint(fp, "\t\t", 1, rule->rhs);
                fprintf(fp, "\t}\n");
        }
	grad_iterator_destroy(&itr);
	
        fprintf(fp, "}\n");
}

int
dump_user(FILE *fp, User_symbol *sym)
{
        fprintf(fp, "\t%s:\n", sym->name);
        fprintf(fp, "\tlhs {\n");
	grad_avl_fprint(fp, "\t\t", 1, sym->check);
        fprintf(fp, "\t}\n");

        fprintf(fp, "\trhs {\n");
	grad_avl_fprint(fp, "\t\t", 1, sym->reply);
        fprintf(fp, "\t}\n");
        
        return 0;
}

void
dump_users_db()
{
        FILE *fp;
        char *name = grad_mkfilename(radlog_dir, RADIUS_DUMPDB_NAME);
        
        fp = fopen(name, "w");
        if (!fp) {
                grad_log(L_ERR|L_PERROR,
                         _("can't create parser output file `%s'"),
                         RADIUS_DUMPDB_NAME);
                grad_free(name);
                return;
        }

        fchmod(fileno(fp), S_IRUSR|S_IWUSR);

        fprintf(fp, "%s {\n", "users");
        grad_symtab_iterate(user_tab, dump_user, fp);
        fprintf(fp, "}\n");

        dump_matching_rules(fp, "huntgroups", huntgroups);
        dump_matching_rules(fp, "hints", hints);
        grad_log(L_INFO, _("dumped users database to %s"), RADIUS_DUMPDB_NAME);
        fclose(fp);
        grad_free(name);
}

/* ***************************************************************************
 * Various utils exported to other modules
 */

void
strip_username(int do_strip, char *name, grad_avp_t *check_item,
	       char *stripped_name)
{
        char tmpname[AUTH_STRING_LEN];
        char *source_ptr = name;
        grad_avp_t *presuf_item, *tmp;
        
        /*
         *      See if there was a Prefix or Suffix included.
         */
        if ((presuf_item = grad_avl_find(check_item, DA_PREFIX)) == NULL)
                presuf_item = grad_avl_find(check_item, DA_SUFFIX);
        if (presuf_item) {
                if (tmp = grad_avl_find(check_item, DA_STRIP_USER_NAME))
                        do_strip = tmp->avp_lvalue;
                if (do_strip) { 
                        if (presufcmp(presuf_item, name, tmpname) == 0)
                                source_ptr = tmpname;
                }
        }
                
        strcpy(stripped_name, source_ptr);
}





