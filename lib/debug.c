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

/* debug.c      Debugging module. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <common.h>

static struct keyword auth_codes[] = {
#define D(a)  {#a, a}
        D(RT_ACCESS_REQUEST),
        D(RT_ACCESS_ACCEPT),
        D(RT_ACCESS_REJECT),
        D(RT_ACCOUNTING_REQUEST),
        D(RT_ACCOUNTING_RESPONSE),
        D(RT_ACCOUNTING_STATUS),
        D(RT_PASSWORD_REQUEST),
        D(RT_PASSWORD_ACK),
        D(RT_PASSWORD_REJECT),
        D(RT_ACCOUNTING_MESSAGE),
        D(RT_ACCESS_CHALLENGE),
        D(RT_STATUS_SERVER),
        D(RT_ASCEND_TERMINATE_SESSION),
        { 0 }
#undef D        
};

const char *
auth_code_str(int code)
{
        struct keyword *p;

        for (p = auth_codes; p->name; p++)
                if (p->tok == code)
                        return p->name;
        return NULL;
}

static struct keyword auth_codes_abbr[] = {
        { "Access-Request", RT_ACCESS_REQUEST }, 
        { "Access-Accept", RT_ACCESS_ACCEPT },     
        { "Access-Reject", RT_ACCESS_REJECT },  
        { "Accounting-Request", RT_ACCOUNTING_REQUEST },     
        { "Accounting-Response", RT_ACCOUNTING_RESPONSE },    
        { "Accounting-Status", RT_ACCOUNTING_STATUS },      
        { "Password-Request", RT_PASSWORD_REQUEST },       
	{ "Password-Ack", RT_PASSWORD_ACK },           
        { "Password-Reject", RT_PASSWORD_REJECT },        
        { "Accounting-Message", RT_ACCOUNTING_MESSAGE },     
        { "Access-Challenge", RT_ACCESS_CHALLENGE },
	{ "Status-Server", RT_STATUS_SERVER },
	{ "Status-Client", RT_STATUS_CLIENT },
        { "Ascend-Terminate-Session",   RT_ASCEND_TERMINATE_SESSION },
	{ "Ascend-Event-Request", RT_ASCEND_EVENT_REQUEST },
	{ "Ascend-Event-Response", RT_ASCEND_EVENT_RESPONSE },
        { "Ascend-Allocate-IP", RT_ASCEND_ALLOCATE_IP },
        { "Ascend-Release-IP", RT_ASCEND_RELEASE_IP },
        { NULL }
};

const char *
auth_code_abbr(int code)
{
        struct keyword *p;
        for (p = auth_codes_abbr; p->name; p++)
                if (p->tok == code)
                        return p->name;
	return "Unknown";
}

#if RADIUS_DEBUG

struct debug_module {
	char *name;
	int level;
};

static grad_list_t *_grad_debug_list;

static int
debug_name_cmp(const void *item, const void *data)
{
	const struct debug_module *lp = item;
	return strcmp(lp->name, (char*)data);
}

static int
debug_mod_cmp(const void *item, const void *data)
{
	const struct debug_module *mod = item;
	const struct debug_module *sample = data;
	int len = strlen(mod->name);

	if (strlen(mod->name) > len)
		return 1;
	if (strncmp(mod->name, sample->name, len))
		return 1;
	return mod->level < sample->level;
}

static int
free_debug_module(void *item, void *data)
{
	grad_free(item);
	return 0;
}

int
set_module_debug_level(char *name, int level)
{
	struct debug_module *lp;
	
        if (level == -1)
                level = MAX_DEBUG_LEVEL;

	if (!_grad_debug_list)
		_grad_debug_list = grad_list_create();

	lp = grad_list_locate(_grad_debug_list, name, debug_name_cmp);
	if (!lp) {
		lp = grad_emalloc(sizeof(*lp));
		lp->name = grad_estrdup(name);
		grad_list_append(_grad_debug_list, lp);
	}

	if (lp->level < level)
		lp->level = level;
        return 0;
}

void
set_debug_levels(char *str)
{
        char *tok, *p, *save;
        int  level;

        for (tok = strtok_r(str, ",", &save); tok; 
             tok = strtok_r(NULL, ",", &save)) {
                p = strchr(tok, '=');
                if (p) {
                        *p++ = 0;
                        level  = atoi(p);
                } else {
                        level  = MAX_DEBUG_LEVEL;
                }
		set_module_debug_level(tok, level);
        }
}

void
clear_debug()
{
	grad_list_destroy(&_grad_debug_list, free_debug_module, NULL);
}


int
grad_debug_p(char *name, int level)
{
	struct debug_module mod;
	char *p = strrchr(name, '/');

	mod.name = p ? p + 1 : name;
	mod.level = level;
	return grad_list_locate(_grad_debug_list, &mod, debug_mod_cmp) != NULL;
}

#else

/*ARGSUSED*/
int
set_module_debug_level(char *name, int level)
{
        grad_log(L_ERR, _("compiled without debugging support"));
}

/*ARGSUSED*/
void
set_debug_levels(char *str)
{
        grad_log(L_ERR, _("compiled without debugging support"));
}

void
clear_debug()
{
}

int
grad_debug_p(char *name, int level)
{
	return 0;
}

#endif
