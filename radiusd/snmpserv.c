/* This file is part of GNU RADIUS.
 * Copyright (C) 2000,2001, Sergey Poznyakoff
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */
#define RADIUS_MODULE 15
#if defined(HAVE_CONFIG_H)
# include <config.h>
#endif

#ifdef USE_SNMP

#ifndef lint
static char rcsid[] = "@(#) $Id$";
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <asn1.h>
#include <snmp.h>

#include <sysdep.h>
#include <radiusd.h>
#include <radutmp.h>
#include <radsnmp.h>
#include <radmib.h>
#include <varargs.h>

#define MAXOIDLEN 64
#define XVAL 0

Mib_tree_node *mib_tree;

Mib_tree_node * snmp_add_node();
Mib_tree_node * snmp_add_simple_node();

struct snmp_pdu * snmp_agent_response(struct snmp_pdu *pdu, int access);
oid_t snmp_create_oid();

int snmp_decode(SNMP_REQ *req, u_char *buf, int len);
int variable_cmp(struct snmp_var *v1, struct snmp_var *v2);

char *snmp_strdup(char*);
	
ACL *snmp_acl, *snmp_acl_tail;
Community *commlist, *commlist_tail;
Server_stat *server_stat;
extern PORT_STAT *stat_base;


/* ************************************************************************ */
/* ACL fiddling */

void
snmp_add_community(str, access)
	char *str;
	int access;
{
	Community *p = alloc_entry(sizeof(*p));
	p->name = estrdup(str);
	p->access = access;
	if (commlist_tail)
		commlist_tail->next = p;
	else
		commlist = p;
	commlist_tail = p;
}

Community *
snmp_find_community(str)
	char *str;
{
	Community *p;

	for (p = commlist; p; p = p->next)
		if (strcmp(p->name, str) == 0)
			return p;
	return NULL;
}

void
snmp_free_communities()
{
	Community *p = commlist, *next;
	while (p) {
		next = p->next;
		efree(p->name);
		free_entry(p);
		p = next;
	}
	commlist = commlist_tail = NULL;
}

int
check_acl(ip, community)
	UINT4 ip;
	char *community;
{
	ACL *acl;

	for (acl = snmp_acl; acl; acl = acl->next) {
		if (acl->ipaddr == (ip & acl->netmask)) {
			if (!acl->community)
				return 0;
			else if (strcmp(acl->community->name, community) == 0)
				return acl->community->access;
		}
	}
	return 0;
}

void
snmp_add_acl(acl, community)
	ACL *acl;
	Community *community;
{
	ACL *new_acl;
	
	for (; acl; acl = acl->next) {
		new_acl = alloc_entry(sizeof(*new_acl));
		memcpy(new_acl, acl, sizeof(ACL));
		new_acl->community = community;
		if (snmp_acl_tail)
			snmp_acl_tail->next = new_acl;
		else
			snmp_acl = new_acl;
		snmp_acl_tail = new_acl;
	}
}

void
snmp_free_acl()
{
	free_acl(snmp_acl);
	snmp_acl = snmp_acl_tail = NULL;
}

void
free_acl(acl)
	ACL *acl;
{
	ACL *next;

	while (acl) {
		next = acl->next;
		free_entry(acl);
		acl = next;
	}
}

/* ************************************************************************ */
/* MIB tree functions */

Mib_tree_node *
snmp_lookup(node, oid, len)
	Mib_tree_node *node;
	oid_t oid;
	int len;
{
	int i;

	for (i = 0; i < len && node; i++) {
		if (OIDLEN(node->name) == i) {
			if ((node = node->son) == NULL)
				break;
		}
		if (OIDLEN(node->name)-1 == i && node->get) {
			node = node->get(node, oid);
			break;
		}
		while (SUBID(node->name,i) != SUBID(oid,i)) {
			if ((node = node->brother) == NULL)
				break;
		}
	}
	return node;
}

/*VARARGS6*/
Mib_tree_node *
snmp_add_node(name, parser, set, get, getnext, numchld, va_alist)
	oid_t name;
	oid_pf parser;	
	oid_sf set;
	oid_gf get;
	oid_gf getnext;
	int numchld;
	va_dcl
{
	va_list ap;
	Mib_tree_node *node;
	char buf[150];
	
	debug(5,("SNMP ADD: %s",
		 sprint_oid(buf, sizeof(buf), name)));
	
	node = alloc_entry(sizeof(*node));
	node->name = name;
	node->parser = parser;
	node->set = set;
	node->get = get;
	node->getnext = getnext;
	node->son = node->brother = NULL;
	if (numchld) {
		int i;
		Mib_tree_node *sibling, *last = NULL, *son = NULL;

		va_start(ap);

		for (i = 0; i < numchld; i++) {
			sibling = va_arg(ap, Mib_tree_node *);
			sibling->parent = node;
			if (last)
				last->brother = sibling;
			last = sibling;
			if (!son)
				son = sibling;
		}
		node->son = son;
		va_end(ap);
	}
	return node;
}

/*VARARGS3*/
Mib_tree_node *
snmp_add_simple_node(name, parser, numchld, va_alist)
	oid_t name;
	oid_pf parser;	
	int numchld;
	va_dcl
{
	va_list ap;
	Mib_tree_node *node;
	char buf[150];
	
	debug(5,("SNMP ADD: %s",
		 sprint_oid(buf, sizeof(buf), name)));

	node = alloc_entry(sizeof(*node));
	node->name = name;
	node->parser = parser;
	node->set = NULL;
	node->get = NULL;
	node->getnext = NULL;
	node->son = node->brother = NULL;
	if (numchld) {
		int i;
		Mib_tree_node *sibling, *last = NULL, *son = NULL;

		va_start(ap);

		for (i = 0; i < numchld; i++) {
			sibling = va_arg(ap, Mib_tree_node *);
			sibling->parent = node;
			if (last)
				last->brother = sibling;
			last = sibling;
			if (!son)
				son = sibling;
		}
		node->son = son;
		va_end(ap);
	}
	return node;
}

/*VARARGS1*/
oid_t
snmp_create_oid(length, va_alist)
	int length;
	va_dcl
{
	int i;
	va_list ap;
	subid_t *id;
	
	va_start(ap);
	id = emalloc(sizeof(*id) * (length+1));
	id[0] = length;
	for (i = 0; i < length; i++) 
		id[i+1] = va_arg(ap, int);
	
	va_end(ap);
	return (oid_t)id;
}

oid_pf 
snmp_tree_pf(tree, name)
	Mib_tree_node *tree;
	oid_t name;
{
	Mib_tree_node *node;

	node = snmp_lookup(tree, name, OIDLEN(name));
	return (node && node->parser) ? node->parser : NULL;
}

oid_sf 
snmp_tree_sf(tree, name)
	Mib_tree_node *tree;
	oid_t name;
{
	Mib_tree_node *node;

	node = snmp_lookup(tree, name, OIDLEN(name));
	return (node && node->set) ? node->set : NULL;
}

oid_pf 
snmp_tree_next(tree, name, nextoid)
	Mib_tree_node *tree;
	oid_t name;
	oid_t *nextoid;
{
	Mib_tree_node *node, *ptr;
	char buf[MAXOIDLEN];
	
	debug(2,
		("SNMP GETNEXT: %s",
		 sprint_oid(buf, sizeof(buf), name)));

	if ((node = snmp_lookup(tree, name, OIDLEN(name))) == NULL)
		return NULL;

again:
	if (!node->parser) {
		debug(2,("going down"));
		while (node && !node->parser) 
			node = node->son;
	
		if (!node)
			return NULL;
		ptr = node;
	} else if (ptr = node->brother) {
		if (!ptr->parser) {
			debug(2,("restaring"));
			node = ptr;
			goto again;
		}
	} else if (!node->getnext) {
	rollback:
		/* Roll back */
		debug(2,("rolling back"));
		ptr = node;
		while ((ptr = ptr->parent) && !ptr->brother)
			;
		
		if (ptr) {
			ptr = ptr->brother;
			while (ptr && !ptr->parser) 
				ptr = ptr->son;
			
			if (!ptr)
				return NULL;
		} else
			return NULL;
	} else
		ptr = node;

	if (ptr->getnext) {
		debug(2,("following getnext"));
		node = ptr;
		ptr = ptr->getnext(ptr, name);
		if (!ptr) 
			goto rollback;
	}
	
	debug(2, ("next oid %s",
		 sprint_oid(buf, sizeof(buf), ptr->name)));

	*nextoid = ptr->name;
		 
	return (ptr && ptr->parser) ? ptr->parser : NULL;

}

/* ************************************************************************ */
/* Application-specific */

#define N snmp_add_simple_node
#define F snmp_add_node
#define ID snmp_create_oid
#define MIB(name) ID(LEN_##name, MIB_##name)

void
snmp_build_acct_tree()
{
	mib_tree =
		N(ID(1, 1), NULL, 1,
		N(ID(2, 1, 3), NULL, 1,
		N(ID(3, 1, 3, 6), NULL, 1,
		N(ID(4, 1, 3, 6, 1), NULL, 1,
		N(ID(5, 1, 3, 6, 1, 2), NULL, 1,
		N(ID(6, 1, 3, 6, 1, 2, 1), NULL, 1,
		N(ID(7, 1, 3, 6, 1, 2, 1, 67), NULL, 3,
		  /* Authentication */
		N(MIB(radiusAuthentication), NULL, 1,
		N(MIB(radiusAuthServMIB), NULL, 1,
	        N(MIB(radiusAuthServMIBObjects), NULL, 1,
		   N(MIB(radiusAuthServ), NULL, 15,
		     N(MIB(radiusAuthServIdent), snmp_auth_var, 0),
		     N(MIB(radiusAuthServUpTime), snmp_auth_var, 0),
		     N(MIB(radiusAuthServResetTime), snmp_auth_var, 0),
		     F(MIB(radiusAuthServConfigReset), snmp_auth_var, snmp_auth_set, NULL, NULL, 0),
		     N(MIB(radiusAuthServTotalAccessRequests), snmp_auth_var, 0),
		     N(MIB(radiusAuthServTotalInvalidRequests), snmp_auth_var, 0),      
		     N(MIB(radiusAuthServTotalDupAccessRequests), snmp_auth_var, 0),          
		     N(MIB(radiusAuthServTotalAccessAccepts), snmp_auth_var, 0),            
		     N(MIB(radiusAuthServTotalAccessRejects), snmp_auth_var, 0),            
		     N(MIB(radiusAuthServTotalAccessChallenges), snmp_auth_var, 0),            
		     N(MIB(radiusAuthServTotalMalformedAccessRequests), snmp_auth_var, 0),    
		     N(MIB(radiusAuthServTotalBadAuthenticators), snmp_auth_var, 0),
		     N(MIB(radiusAuthServTotalPacketsDropped), snmp_auth_var, 0), 
		     N(MIB(radiusAuthServTotalUnknownTypes), snmp_auth_var, 0), 
		     N(MIB(radiusAuthClientTable), NULL, 1,
		       N(MIB(radiusAuthClientEntry), NULL, 11,
			 N(MIB(radiusAuthClientIndex), NULL, 1,
			   F(MIB(radiusAuthClientIndex_X), snmp_auth_client_var, NULL, snmp_auth_get, snmp_auth_getnext, 0)),
			 N(MIB(radiusAuthClientAddress), NULL, 1,
			   F(MIB(radiusAuthClientAddress_X), snmp_auth_client_var, NULL, snmp_auth_get, snmp_auth_getnext, 0)),   
			 N(MIB(radiusAuthClientID), NULL, 1,
			   F(MIB(radiusAuthClientID_X), snmp_auth_client_var, NULL, snmp_auth_get, snmp_auth_getnext, 0)), 
			 N(MIB(radiusAuthServAccessRequests), NULL, 1,
			   F(MIB(radiusAuthServAccessRequests_X), snmp_auth_client_var,  NULL, snmp_auth_get, snmp_auth_getnext, 0)),      
			 N(MIB(radiusAuthServDupAccessRequests), NULL, 1,
			   F(MIB(radiusAuthServDupAccessRequests_X), snmp_auth_client_var,  NULL, snmp_auth_get, snmp_auth_getnext, 0)),      
			 N(MIB(radiusAuthServAccessAccepts), NULL, 1,
			   F(MIB(radiusAuthServAccessAccepts_X), snmp_auth_client_var, NULL, snmp_auth_get, snmp_auth_getnext, 0)),
			 N(MIB(radiusAuthServAccessRejects), NULL, 1,
			   F(MIB(radiusAuthServAccessRejects_X), snmp_auth_client_var,  NULL, snmp_auth_get, snmp_auth_getnext, 0)),
			 N(MIB(radiusAuthServAccessChallenges), NULL, 1,
			   F(MIB(radiusAuthServAccessChallenges_X), snmp_auth_client_var,  NULL, snmp_auth_get, snmp_auth_getnext, 0)),  
			 N(MIB(radiusAuthServMalformedAccessRequests), NULL, 1,
			   F(MIB(radiusAuthServMalformedAccessRequests_X), snmp_auth_client_var,  NULL, snmp_auth_get, snmp_auth_getnext, 0)),  
			 N(MIB(radiusAuthServBadAuthenticators), NULL, 1,
			   F(MIB(radiusAuthServBadAuthenticators_X), snmp_auth_client_var,  NULL, snmp_auth_get, snmp_auth_getnext, 0)), 
			 N(MIB(radiusAuthServPacketsDropped), NULL, 1,
			   F(MIB(radiusAuthServPacketsDropped), snmp_auth_client_var,  NULL, snmp_auth_get, snmp_auth_getnext, 0)),
			 N(MIB(radiusAuthServUnknownTypes), NULL, 1,
			   F(MIB(radiusAuthServUnknownTypes_X), snmp_auth_client_var,  NULL, snmp_auth_get, snmp_auth_getnext, 0)))))))), /*radiusAuthServ*/

		  
		  /* Accounting */
		N(MIB(radiusAccounting), NULL, 1,
		N(MIB(radiusAccServMIB), NULL, 1,
	        N(MIB(radiusAccServMIBObjects), NULL, 1,
		  N(MIB(radiusAccServ), NULL, 14,
		     N(MIB(radiusAccServIdent), snmp_acct_var, 0),
		     N(MIB(radiusAccServUpTime), snmp_acct_var, 0),
		     N(MIB(radiusAccServResetTime), snmp_acct_var, 0),
		     F(MIB(radiusAccServConfigReset), snmp_acct_var, snmp_acct_set, NULL, NULL, 0),
		     N(MIB(radiusAccServTotalRequests), snmp_acct_var, 0),
		     N(MIB(radiusAccServTotalInvalidRequests), snmp_acct_var, 0),      
		     N(MIB(radiusAccServTotalDupRequests), snmp_acct_var, 0),          
		     N(MIB(radiusAccServTotalResponses), snmp_acct_var, 0),            
		     N(MIB(radiusAccServTotalMalformedRequests), snmp_acct_var, 0),    
		     N(MIB(radiusAccServTotalBadAuthenticators), snmp_acct_var, 0),
		     N(MIB(radiusAccServTotalPacketsDropped), snmp_acct_var, 0), 
		     N(MIB(radiusAccServTotalNoRecords), snmp_acct_var, 0), 
		     N(MIB(radiusAccServTotalUnknownTypes), snmp_acct_var, 0),
		     N(MIB(radiusAccClientTable), NULL, 1,
		       N(MIB(radiusAccClientEntry), NULL, 11,
			 N(MIB(radiusAccClientIndex), NULL, 1,
			   F(MIB(radiusAccClientIndex_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)),
			 N(MIB(radiusAccClientAddress), NULL, 1,
			   F(MIB(radiusAccClientAddress_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)),   
			 N(MIB(radiusAccClientID), NULL, 1,
			   F(MIB(radiusAccClientID_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)), 
			 N(MIB(radiusAccServPacketsDropped), NULL, 1,
			   F(MIB(radiusAccServPacketsDropped_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)),      
			 N(MIB(radiusAccServRequests), NULL, 1,
			   F(MIB(radiusAccServRequests_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)),
			 N(MIB(radiusAccServDupRequests), NULL, 1,
			   F(MIB(radiusAccServRequests_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)),  
			 N(MIB(radiusAccServResponses), NULL, 1,
			   F(MIB(radiusAccServResponses_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)),
			 N(MIB(radiusAccServBadAuthenticators), NULL, 1,
			   F(MIB(radiusAccServBadAuthenticators_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)), 
			 N(MIB(radiusAccServMalformedRequests), NULL, 1,
			   F(MIB(radiusAccServMalformedRequests_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)), 
			 N(MIB(radiusAccServNoRecords), NULL, 1,
			   F(MIB(radiusAccServNoRecords_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0)),
			 N(MIB(radiusAccServUnknownTypes), NULL, 1,
			   F(MIB(radiusAccServUnknownTypes_X), snmp_acct_client_var, NULL, snmp_acct_get, snmp_acct_getnext, 0))))))) /*radiusAccServ*/ ),

		  /* Statistics */
		  N(MIB(radiusStatistics), NULL, 1,
		    N(MIB(radiusStatMIB), NULL, 8,
		      N(MIB(radiusStatIdent), snmp_stat_var, 0),
		      N(MIB(radiusStatUpTime), snmp_stat_var, 0),
		      N(MIB(radiusStatConfigReset), snmp_stat_var, 0),
		      N(MIB(radiusStatTotalLines), snmp_stat_var, 0),
		      N(MIB(radiusStatTotalLinesInUse), snmp_stat_var, 0),
		      N(MIB(radiusStatTotalLinesIdle), snmp_stat_var, 0),
		      N(MIB(radiusStatNASTable), NULL, 1,
			N(MIB(radiusStatNASEntry), NULL, 6,
			  N(MIB(NASIndex), NULL, 1,
			    F(MIB(NASIndex_X), snmp_stat_client_var, NULL, snmp_stat_get, snmp_stat_getnext, 0)),
			  N(MIB(NASAddress), NULL, 1,
			    F(MIB(NASAddress_X), snmp_stat_client_var, NULL, snmp_stat_get, snmp_stat_getnext, 0)),
			  N(MIB(NASID), NULL, 1,
			    F(MIB(NASID_X), snmp_stat_client_var, NULL, snmp_stat_get, snmp_stat_getnext, 0)),
			  N(MIB(NASLines), NULL, 1,
			    F(MIB(NASLines_X), snmp_stat_client_var, NULL, snmp_stat_get, snmp_stat_getnext, 0)),
			  N(MIB(NASLinesInUse), NULL, 1,
			    F(MIB(NASLinesInUse_X), snmp_stat_client_var, NULL, snmp_stat_get, snmp_stat_getnext, 0)),
			  N(MIB(NASLinesIdle), NULL, 1,
			    F(MIB(NASLinesIdle_X), snmp_stat_client_var, NULL, snmp_stat_get, snmp_stat_getnext, 0)))),

		      N(MIB(radiusStatNASPortTable), NULL, 1,
			N(MIB(radiusStatNASPortEntry), NULL, 16,
			  N(MIB(radiusStatNASIndex), NULL, 1,
			    F(MIB(radiusStatNASIndex_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortID), NULL, 1,
			    F(MIB(radiusStatPortID_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortFramedAddress), NULL, 1,
			    F(MIB(radiusStatPortFramedAddress_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortTotalLogins), NULL, 1,
			    F(MIB(radiusStatPortTotalLogins_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortStatus), NULL, 1,
			    F(MIB(radiusStatPortStatus_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortStatusDate), NULL, 1,
			    F(MIB(radiusStatPortStatusDate_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortUpTime), NULL, 1,
			    F(MIB(radiusStatPortUpTime_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortLastLoginName), NULL, 1,
			    F(MIB(radiusStatPortLastLoginName_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortLastLoginDate), NULL, 1,
			    F(MIB(radiusStatPortLastLoginDate_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortLastLogoutDate), NULL, 1,
			    F(MIB(radiusStatPortLastLogoutDate_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortIdleTotalTime), NULL, 1,
			    F(MIB(radiusStatPortIdleTotalTime_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortIdleMaxTime), NULL, 1,
			    F(MIB(radiusStatPortIdleMaxTime_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortIdleMaxDate), NULL, 1,
			    F(MIB(radiusStatPortIdleMaxDate_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortInUseTotalTime), NULL, 1,
			    F(MIB(radiusStatPortInUseTotalTime_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortInUseMaxTime), NULL, 1,
			    F(MIB(radiusStatPortInUseMaxTime_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)),
			  N(MIB(radiusStatPortInUseMaxDate), NULL, 1,
			    F(MIB(radiusStatPortInUseMaxDate_X), snmp_stat_port_var, NULL, snmp_stat_port_get, snmp_stat_port_getnext, 0)))))))))))));

}

void
snmp_tree_init()
{
	snmp_init(0, 0, emalloc, efree);
	snmp_build_acct_tree();
}

struct nas_stat *
find_nas_stat(ip_addr)
	UINT4 ip_addr;
{
	int i;
	struct nas_stat *nasstat = (struct nas_stat *)(server_stat + 1);
	
	for (i = 0; i < server_stat->nas_count; i++, nasstat++)
		if (nasstat->ipaddr == ip_addr)
			return nasstat;
	return NULL;
}

struct nas_stat *
snmp_nasstat(num)
	int num;
{
	return (struct nas_stat *)(server_stat + 1) + num;
}

void
snmp_init_nas_stat()
{
	server_stat->nas_index = 1;
}

void
snmp_attach_nas_stat(nas, master)
	NAS *nas;
	int master;
{
	struct nas_stat *nasstat;

	nasstat = find_nas_stat(nas->ipaddr);
	if (!nasstat) {
		if (server_stat->nas_count >= STAT_MAX_NAS_COUNT) {
			radlog(L_ERR, _("too many NASes: increase STAT_MAX_NAS_COUNT"));
			return;
		}
		nasstat = snmp_nasstat(server_stat->nas_count++);
		nasstat->ipaddr = nas->ipaddr;
	}
	if (master)
		nasstat->index = server_stat->nas_index++;
	nas->nas_stat = nasstat;
}

void
snmp_auth_server_reset()
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	server_stat->auth.reset_time = tv;
}

void
snmp_acct_server_reset()
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	server_stat->acct.reset_time = tv;
}

static int		i_send_buffer[1024];
static char		*send_buffer = (char *)i_send_buffer;

SNMP_REQ *
rad_snmp_respond(buf, len, sa)
	u_char *buf;
	int len;
	struct sockaddr_in *sa;
{
	SNMP_REQ *req;
	char ipbuf[DOTTED_QUAD_LEN];

	req = alloc_entry(sizeof *req);
	req->sa = *sa;

	debug(1,
		("got %d bytes from %I",
		 len,
		 ntohl(req->sa.sin_addr.s_addr)));
	
	if (snmp_decode(req, buf, len)) {
		free_entry(req);
		req = NULL;
	}
	return req;
}

int
snmp_decode(req, buf, len)
	SNMP_REQ *req;
	u_char *buf;
	int len;
{
	struct snmp_pdu *pdu;
	struct snmp_session sess;
	int access;
	char ipbuf[DOTTED_QUAD_LEN];
	char comm[128];
	int comm_len;
	
	if ((pdu = snmp_pdu_create(0)) == NULL) {
		radlog(L_ERR,
		       _("can't create SNMP PDU: %s"),
			 snmp_strerror(snmp_errno));
		return -1;
	}
	comm_len = sizeof(comm);
	if (snmp_decode_request(&sess, pdu, buf, len, comm, &comm_len)) {
		radlog(L_ERR,
		       _("can't decode SNMP packet from %s: %s"),
			 ip_hostname(ntohl(req->sa.sin_addr.s_addr)),
			 snmp_strerror(snmp_errno));
		return -1;
	}

	access = check_acl(req->sa.sin_addr.s_addr, comm);
	if (!access) {
		radlog(L_NOTICE,
		       _("DENIED attempt to access community %s from %s (%I)"),
		       comm,
		       ip_hostname(ntohl(req->sa.sin_addr.s_addr)),
		       ntohl(req->sa.sin_addr.s_addr));
		return 1;
	}
	req->pdu = pdu;
	req->community = estrdup(comm);
	req->access = access;
	return 0;
}

int
snmp_req_cmp(a, b)
	SNMP_REQ *a, *b;
{
	return !(a->sa.sin_addr.s_addr == b->sa.sin_addr.s_addr &&
		 variable_cmp(a->pdu->var, b->pdu->var) == 0);
}

void
snmp_req_free(req)
	SNMP_REQ *req;
{
	snmp_pdu_free(req->pdu);
	efree(req->community);
	free_entry(req);
}

void
snmp_req_drop(type, req, status_str)
	int type;
	SNMP_REQ *req;
	char *status_str;
{
	radlog(L_NOTICE,
	       _("Dropping SNMP request from client %I: %s"),
	       ntohl(req->sa.sin_addr.s_addr),
	       status_str);
}

int
snmp_answer(req, sock)
	SNMP_REQ *req;
	int sock;
{
	struct snmp_session session;
	struct snmp_pdu *pdu;
	int len;

	pdu = snmp_agent_response(req->pdu, req->access);
	if (pdu) {
		session.version = SNMP_VERSION_1;
		session.community.str = req->community;
		session.community.len = strlen(req->community);
		len = sizeof(i_send_buffer);
		if (snmp_encode_request(&session, pdu, send_buffer, &len)==0) {
			sendto(sock,
			       send_buffer, len,
			       0, (struct sockaddr *) &req->sa,
			       sizeof(req->sa));
		}
		snmp_pdu_free(pdu);
	}
	return 0;
}


struct snmp_pdu *
snmp_agent_response(pdu, access)
	struct snmp_pdu *pdu;
	int access;
{
	struct snmp_pdu *answer = NULL;
	oid_pf pf = NULL;
	oid_sf sf = NULL;
	oid_t nextoid = NULL;
	struct snmp_var *vp, *vnew = NULL, **vpp;
	struct snmp_var **vresp;
	int index = 0;

	if ((answer = snmp_pdu_create(SNMP_PDU_RESPONSE))) {
		answer->req_id = pdu->req_id;
		answer->err_ind = 0;
		switch (pdu->type) {

		case SNMP_PDU_SET:
			/* First, check for the consistency of
			 * the request (see rfc1157, 4.1.5):
			 */
			debug(1, ("SetRequest-PDU"));
			answer->var = pdu->var;
			if (access == SNMP_RO) {
				answer->err_stat = SNMP_ERR_GENERR;
				answer->err_ind = 1;
				debug(1, ("bad access mode"));
				return answer;
			}
			for (vp = pdu->var; vp; vp = vp->next) {
				index++;
				sf = snmp_tree_sf(mib_tree, vp->name);
				
				if (!sf) {
					answer->err_stat = SNMP_ERR_NOSUCHNAME;
					debug(1, ("No such oid"));
					break;
				} else if (!(*sf)(vp, &answer->err_stat)) 
					break;
			}

			if (answer->err_stat != SNMP_ERR_NOERROR) {
				answer->err_ind = index;
				debug(1, ("returning error"));
				return answer;
			}

			/* Do real work */
			vresp = &answer->var;
			/* Loop through all variables */
			for (vpp = &pdu->var;
			     *vpp;
			     vpp = &(*vpp)->next) {
				vp = *vpp;

				sf = snmp_tree_sf(mib_tree, vp->name);
				
				vnew = (*sf)(vp, NULL);
				*vresp = vnew;
				vresp = &vnew->next;
			}

			debug(1, ("success"));
			return answer;

		case SNMP_PDU_GET:
			debug(1, ("GetRequest-PDU"));

			vresp = &answer->var;
			/* Loop through all variables */
			for (vpp = &pdu->var;
			     *vpp;
			     vpp = &(*vpp)->next) {
				vp = *vpp;

				index++;

				pf = snmp_tree_pf(mib_tree, vp->name);
				
				if (!pf) {
					answer->err_stat = SNMP_ERR_NOSUCHNAME;
					debug(1, ("No such oid"));
				} else
					vnew = (*pf)(vp, &answer->err_stat);

				/* Was there an error? */
				if (answer->err_stat != SNMP_ERR_NOERROR ||
				    vnew == NULL) {
					answer->err_ind = index;
					debug(1, ("returning"));
					/* Just copy the rest of the variables.  Quickly. */
					*vresp = vp;
					*vpp = NULL;
					return answer;
				}
				/* No error.
				 * Insert this var at the end, and move on
				 * to the next.
				 */
				*vresp = vnew;
				vresp = &vnew->next;
			}
			return answer;

		case SNMP_PDU_GETNEXT:
			debug(1, ("GetNextRequest-PDU"));
			
			pf = snmp_tree_next(mib_tree, pdu->var->name,
					    &nextoid);

			if (!pf) {
				answer->err_stat = SNMP_ERR_NOSUCHNAME;
				debug(1, ("No such oid"));
			} else {
				snmp_free(pdu->var->name);
				pdu->var->name = oid_dup(nextoid);
				vnew = (*pf)(pdu->var, &answer->err_stat);
			}

			/* Was there an error? */
			if (answer->err_stat != SNMP_ERR_NOERROR) {
				answer->err_ind = 1;
				answer->var = pdu->var;
				pdu->var = NULL;
			} else {
				answer->var = vnew;
			}
			break;

		default:
			snmp_pdu_free(answer);
			answer = NULL;
		}
	}
	return answer;
}


counter
timeval_diff(tva, tvb)
	struct timeval *tva, *tvb;
{
	return  (tva->tv_sec - tvb->tv_sec)*100 +
		(tva->tv_usec - tvb->tv_usec)/10000;
}


struct snmp_var *
snmp_acct_var(list, errp)
	struct snmp_var *list;
	int *errp;
{
	struct snmp_var *ret;
	struct timeval tv;
	struct timezone tz;
	char *p;
	
	ret = snmp_var_create(list->name);
	*errp = SNMP_ERR_NOERROR;
	switch (SUBID(list->name,MIB_POS_radiusAccServ)) {
	case MIB_KEY_radiusAccServIdent:
		p = make_server_ident();
		ret->type = ASN_OCTET_STR;
		ret->val_length = strlen(p);
		ret->var_str = snmp_strdup(p);
		efree(p);
		break;

	case MIB_KEY_radiusAccServUpTime:
		gettimeofday(&tv, &tz);
		ret->type = SMI_TIMETICKS;
		ret->val_length = sizeof(counter);
		ret->var_int = timeval_diff(&tv, &server_stat->start_time);
		break;
		
	case MIB_KEY_radiusAccServResetTime:
		gettimeofday(&tv, &tz);
		ret->type = SMI_TIMETICKS;
		ret->val_length = sizeof(counter);
		ret->var_int = timeval_diff(&tv, &server_stat->acct.reset_time);
		break;

	case MIB_KEY_radiusAccServConfigReset:
		ret->type = ASN_INTEGER;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.status;
		break;
		
	case MIB_KEY_radiusAccServTotalRequests:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_req;
		break;
		
	case MIB_KEY_radiusAccServTotalInvalidRequests:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_invalid_req;
		break;
		
	case MIB_KEY_radiusAccServTotalDupRequests:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_dup_req;
		break;
		
	case MIB_KEY_radiusAccServTotalResponses:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_resp;
		break;
		
	case MIB_KEY_radiusAccServTotalMalformedRequests:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_bad_req;
		break;
		
	case MIB_KEY_radiusAccServTotalBadAuthenticators:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_bad_sign;
		break;
		
	case MIB_KEY_radiusAccServTotalPacketsDropped:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_dropped;
		break;
		
	case MIB_KEY_radiusAccServTotalNoRecords:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_norecords;
		break;
		
	case MIB_KEY_radiusAccServTotalUnknownTypes:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->acct.num_unknowntypes;
		break;
		
	default:
		*errp = SNMP_ERR_NOSUCHNAME;
		snmp_var_free(ret);
		return NULL;
	}
	return ret;
}

Mib_tree_node *
snmp_acct_get(node, name)
	Mib_tree_node *node;
	oid_t name;
{
	int ind = SUBID(name,MIB_POS_radiusAccClient_INDEX);
	NAS *nas = findnasbyindex(ind);

	if (!nas)
		return 0;
	SUBID(node->name,MIB_POS_radiusAccClient_INDEX) = ind;
	return node;
}

int
oidcmp(a, b, len)
	subid_t *a, *b;
	int len;
{
	for (; len; len--) {
		if (*a++ != *b++)
			return len;
	}
	return 0;
}

/* Compair oids of two variable lists. Order is significant.
 * Return 0 if both lists match, 1 otherwise.
 */
int
variable_cmp(v1, v2)
	struct snmp_var *v1, *v2;
{
	while (v1) {
		if (OIDLEN(v1->name) != OIDLEN(v2->name) ||
		    oidcmp(OIDPTR(v1->name), OIDPTR(v2->name),
			   OIDLEN(v1->name)))
		    return 1;
		v1 = v1->next;
		v2 = v2->next;
	}
	return !(v1 == NULL && v2 == NULL);
}

Mib_tree_node *
snmp_acct_getnext(node, name)
	Mib_tree_node *node;
	oid_t name;
{
	int ind;
	NAS *nas;
	char buf1[MAXOIDLEN], buf2[MAXOIDLEN];
	
	debug(2, ("node %s, name %s",
		  sprint_oid(buf1, sizeof(buf1), node->name),
		  sprint_oid(buf2, sizeof(buf2), name)));

	if (oidcmp(OIDPTR(name), OIDPTR(node->name), OIDLEN(node->name)-1)) {
		ind = 1;
	} else {
		ind = SUBID(name,MIB_POS_radiusAccClient_INDEX) + 1;
	} 
	nas = findnasbyindex(ind);
	
	if (!nas) {
		debug(2,("no more nodes"));
 		return 0;
	}
	debug(2,("NAS %d: %s", ind, nas->shortname));

	SUBID(node->name,MIB_POS_radiusAccClient_INDEX) = ind;
	return node;
}

void
get_acct_nasstat(nas, var, ind)
	NAS *nas;
	struct snmp_var *var;
	int ind;
{
	switch (ind) {
	case MIB_KEY_radiusAccClientIndex:
		var->type = ASN_INTEGER;
		var->val_length = sizeof(int);
		var->var_int = nas->nas_stat->index;
		break;

	case MIB_KEY_radiusAccClientAddress:
		var->type = SMI_IPADDRESS;
		var->val_length = sizeof(UINT4);
		var->var_str = snmp_alloc(sizeof(UINT4));
		*(UINT4*)var->var_str = ntohl(nas->nas_stat->ipaddr);
		break;

	case MIB_KEY_radiusAccClientID:
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(nas->longname);
		var->var_str = snmp_strdup(nas->longname);
		break;

	case MIB_KEY_radiusAccServPacketsDropped:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->acct.num_dropped;
		break;

	case MIB_KEY_radiusAccServRequests:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->acct.num_req;
		break;

	case MIB_KEY_radiusAccServDupRequests:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->acct.num_dup_req;
		break;

	case MIB_KEY_radiusAccServResponses:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->acct.num_resp;
		break;

	case MIB_KEY_radiusAccServBadAuthenticators:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->acct.num_bad_sign;
		break;

	case MIB_KEY_radiusAccServMalformedRequests:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->acct.num_bad_req;
		break;

	case MIB_KEY_radiusAccServNoRecords:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->acct.num_norecords;
		break;

	case MIB_KEY_radiusAccServUnknownTypes:         
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->acct.num_unknowntypes;
		break;
	}
}

struct snmp_var *
snmp_acct_client_var(list, errp)
	struct snmp_var *list;
	int *errp;
{
	struct snmp_var *ret;
	NAS *nas;
	
	ret = snmp_var_create(list->name);
	*errp = SNMP_ERR_NOERROR;
	switch (SUBID(list->name,MIB_POS_radiusAuthClient)) {
	case MIB_KEY_radiusAccClientIndex:
	case MIB_KEY_radiusAccClientAddress:
	case MIB_KEY_radiusAccClientID:
	case MIB_KEY_radiusAccServPacketsDropped:
	case MIB_KEY_radiusAccServRequests:
	case MIB_KEY_radiusAccServDupRequests:
	case MIB_KEY_radiusAccServResponses:
	case MIB_KEY_radiusAccServBadAuthenticators:
	case MIB_KEY_radiusAccServMalformedRequests:
	case MIB_KEY_radiusAccServNoRecords:
	case MIB_KEY_radiusAccServUnknownTypes:         
		if (OIDLEN(list->name) == MIB_POS_radiusAccClient_INDEX+1 &&
		    (nas = findnasbyindex(
			    SUBID(list->name,MIB_POS_radiusAccClient_INDEX)))
		    != NULL &&
		    nas->nas_stat) {
 			get_acct_nasstat(nas, ret,
					 SUBID(list->name,
					       MIB_POS_radiusAccClient));
			break;
		}
		/*FALLTHRU*/
	default:
		*errp = SNMP_ERR_NOSUCHNAME;
		snmp_var_free(ret);
		return NULL;
	}
	return ret;
}

struct snmp_var *
snmp_auth_compliance(list, errp)
	struct snmp_var *list;
	int *errp;
{
	*errp = SNMP_ERR_NOERROR;
	return NULL;
}

struct snmp_var *
snmp_auth_mib_group(list, errp)
	struct snmp_var *list;
	int *errp;
{
	*errp = SNMP_ERR_NOERROR;
	return NULL;
}

struct snmp_var *
snmp_auth_var(list, errp)
	struct snmp_var *list;
	int *errp;
{
	struct snmp_var *ret;
	struct timeval tv;
	struct timezone tz;
	char *p;
	
	ret = snmp_var_create(list->name);
	*errp = SNMP_ERR_NOERROR;
	switch (SUBID(list->name,MIB_POS_radiusAuthServ)) {
		
	case MIB_KEY_radiusAuthServIdent:
		p = make_server_ident();
		ret->type = ASN_OCTET_STR;
		ret->val_length = strlen(p);
		ret->var_str = snmp_strdup(p);
		efree(p);
		break;

	case MIB_KEY_radiusAuthServUpTime:
		gettimeofday(&tv, &tz);
		ret->type = SMI_TIMETICKS;
		ret->val_length = sizeof(counter);
		ret->var_int = timeval_diff(&tv, &server_stat->start_time);
		break;

	case MIB_KEY_radiusAuthServResetTime:
		gettimeofday(&tv, &tz);
		ret->type = SMI_TIMETICKS;
		ret->val_length = sizeof(counter);
		ret->var_int = timeval_diff(&tv, &server_stat->auth.reset_time);
		break;

	case MIB_KEY_radiusAuthServConfigReset:
		ret->type = ASN_INTEGER;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.status;
		break;

	case MIB_KEY_radiusAuthServTotalAccessRequests:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_access_req;
		break;

	case MIB_KEY_radiusAuthServTotalInvalidRequests:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_invalid_req;
		break;

	case MIB_KEY_radiusAuthServTotalDupAccessRequests:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_dup_req;
		break;

	case MIB_KEY_radiusAuthServTotalAccessAccepts:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_accepts;
		break;

	case MIB_KEY_radiusAuthServTotalAccessRejects:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_rejects;
		break;

	case MIB_KEY_radiusAuthServTotalAccessChallenges:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_challenges;
		break;

	case MIB_KEY_radiusAuthServTotalMalformedAccessRequests:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_bad_req;
		break;

	case MIB_KEY_radiusAuthServTotalBadAuthenticators:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_bad_auth;
		break;
		
	case MIB_KEY_radiusAuthServTotalPacketsDropped:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_dropped;
		break;

	case MIB_KEY_radiusAuthServTotalUnknownTypes:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		ret->var_int = server_stat->auth.num_unknowntypes;
		break;
		
	default:
		*errp = SNMP_ERR_NOSUCHNAME;
		snmp_var_free(ret);
		return NULL;
	}
	return ret;
}

Mib_tree_node *
snmp_auth_get(node, name)
	Mib_tree_node *node;
	oid_t name;
{
	int ind = SUBID(name,MIB_POS_radiusAuthClient_INDEX);
	NAS *nas = findnasbyindex(ind);

	if (!nas)
		return 0;
	SUBID(node->name,MIB_POS_radiusAuthClient_INDEX) = ind;
	return node;
}

Mib_tree_node *
snmp_auth_getnext(node, name)
	Mib_tree_node *node;
	oid_t name;
{
	int ind;
	NAS *nas;
	char buf1[MAXOIDLEN], buf2[MAXOIDLEN];
	
	debug(2,
		("node %s, name %s",
		 sprint_oid(buf1, sizeof(buf1), node->name),
		 sprint_oid(buf2, sizeof(buf2), name)));

	if (oidcmp(OIDPTR(name), OIDPTR(node->name),
		   OIDLEN(node->name)-1)) {
		ind = 1;
	} else {
		ind = SUBID(name,MIB_POS_radiusAuthClient_INDEX) + 1;
	} 
	nas = findnasbyindex(ind);

	if (!nas) {
		debug(2,("no more nodes"));
		return 0;
	}
	debug(2, ("NAS %d: %s", ind, nas->shortname));

	SUBID(node->name,MIB_POS_radiusAuthClient_INDEX) = ind;
	return node;
}

void
get_nasstat(nas, var, ind)
	NAS *nas;
	struct snmp_var *var;
	int ind;
{
	switch (ind) {
	case MIB_KEY_radiusAuthClientIndex:
		var->type = ASN_INTEGER;
		var->val_length = sizeof(int);
		var->var_int = nas->nas_stat->index;
		break;
		
	case MIB_KEY_radiusAuthClientAddress:
		var->type = SMI_IPADDRESS;
		var->val_length = sizeof(UINT4);
		var->var_str = snmp_alloc(sizeof(UINT4));
		*(UINT4*)var->var_str = ntohl(nas->nas_stat->ipaddr);
		break;

	case MIB_KEY_radiusAuthClientID:
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(nas->longname);
		var->var_str = snmp_strdup(nas->longname);
		break;

	case MIB_KEY_radiusAuthServAccessRequests:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_access_req;
		break;

	case MIB_KEY_radiusAuthServDupAccessRequests:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_dup_req;
		break;

	case MIB_KEY_radiusAuthServAccessAccepts:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_accepts;
		break;

	case MIB_KEY_radiusAuthServAccessRejects:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_rejects;
		break;

	case MIB_KEY_radiusAuthServAccessChallenges:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_challenges;
		break;

	case MIB_KEY_radiusAuthServMalformedAccessRequests:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_bad_req;
		break;

	case MIB_KEY_radiusAuthServBadAuthenticators:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_bad_auth;
		break;

	case MIB_KEY_radiusAuthServPacketsDropped:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_dropped;
		break;

	case MIB_KEY_radiusAuthServUnknownTypes:
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->auth.num_unknowntypes;
		break;
		
	}
}


struct snmp_var *
snmp_auth_client_var(list, errp)
	struct snmp_var *list;
	int *errp;
{
	struct snmp_var *ret;
	NAS *nas;
	
	ret = snmp_var_create(list->name);
	*errp = SNMP_ERR_NOERROR;
	switch (SUBID(list->name,MIB_POS_radiusAuthClient)) {
	case MIB_KEY_radiusAuthClientIndex:
	case MIB_KEY_radiusAuthClientAddress:
	case MIB_KEY_radiusAuthClientID:
	case MIB_KEY_radiusAuthServAccessRequests:
	case MIB_KEY_radiusAuthServDupAccessRequests:
	case MIB_KEY_radiusAuthServAccessAccepts:
	case MIB_KEY_radiusAuthServAccessRejects:
	case MIB_KEY_radiusAuthServAccessChallenges:
	case MIB_KEY_radiusAuthServMalformedAccessRequests:
	case MIB_KEY_radiusAuthServBadAuthenticators:
	case MIB_KEY_radiusAuthServPacketsDropped:
	case MIB_KEY_radiusAuthServUnknownTypes:
		if (OIDLEN(list->name) == MIB_POS_radiusAuthClient_INDEX+1 &&
		    (nas = findnasbyindex(
			    SUBID(list->name,MIB_POS_radiusAuthClient_INDEX)))
		    != NULL &&
		    nas->nas_stat) {
 			get_nasstat(nas, ret,
				    SUBID(list->name,
					  MIB_POS_radiusAuthClient));
			break;
		}
		/*FALLTHRU*/
	default:
		*errp = SNMP_ERR_NOSUCHNAME;
		snmp_var_free(ret);
		return NULL;
	}
	return ret;
}

struct snmp_var *
snmp_auth_set(vp, errp)
	struct snmp_var *vp;
	int *errp;
{
	if (errp) { /* just test */
		*errp = SNMP_ERR_NOERROR;
		switch (SUBID(vp->name,MIB_POS_radiusAuthServ)) {
		
		case MIB_KEY_radiusAuthServConfigReset:
			if (vp->type != ASN_INTEGER ||
			    vp->var_int != serv_reset) {
				*errp = SNMP_ERR_BADVALUE;
				vp = NULL;
			}
			break;
		default:
			*errp = SNMP_ERR_BADVALUE;
			vp = NULL;
		}
	} else {
		vp = snmp_var_dup(vp);
		
		switch (SUBID(vp->name,MIB_POS_radiusAuthServ)) {

		case MIB_KEY_radiusAuthServConfigReset:
			server_stat->auth.status = serv_init;
			radlog(L_INFO,
			     _("auth server re-initializing on SNMP request"));
			break;

		}
	}
	return vp;
}

struct snmp_var *
snmp_acct_set(vp, errp)
	struct snmp_var *vp;
	int *errp;
{
	if (errp) { /* just test */
		*errp = SNMP_ERR_NOERROR;
		switch (SUBID(vp->name,MIB_POS_radiusAccServ)) {
		
		case MIB_KEY_radiusAccServConfigReset:
			if (vp->type != ASN_INTEGER ||
			    vp->var_int != serv_reset) {
				*errp = SNMP_ERR_BADVALUE;
				vp = NULL;
			}
			break;

		default:
			*errp = SNMP_ERR_BADVALUE;
			vp = NULL;
		}
	} else {
		vp = snmp_var_dup(vp);
		
		switch (SUBID(vp->name,MIB_POS_radiusAccServ)) {

		case MIB_KEY_radiusAccServConfigReset:
			server_stat->auth.status = serv_init;
			radlog(L_INFO, _("acct server re-initializing on SNMP request"));
			break;

		}
	}
	return vp;
}

struct snmp_var *
snmp_stat_var(list, errp)
	struct snmp_var *list;
	int *errp;
{
	struct snmp_var *ret;
	struct timeval tv;
	struct timezone tz;
	char *p;
	
	ret = snmp_var_create(list->name);
	*errp = SNMP_ERR_NOERROR;

	switch (SUBID(list->name,MIB_POS_radiusStatServ)) {
		
	case MIB_KEY_radiusStatIdent:
		p = make_server_ident();
		ret->type = ASN_OCTET_STR;
		ret->val_length = strlen(p);
		ret->var_str = snmp_strdup(p);
		efree(p);
		break;

	case MIB_KEY_radiusStatUpTime:
		gettimeofday(&tv, &tz);
		ret->type = SMI_TIMETICKS;
		ret->val_length = sizeof(counter);
		ret->var_int = timeval_diff(&tv, &radstat.start_time);
		break;

	case MIB_KEY_radiusStatConfigReset:
		ret->type = ASN_INTEGER;
		ret->val_length = sizeof(counter);
		ret->var_int = serv_running;;
		break;

	case MIB_KEY_radiusStatTotalLines:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		stat_count_ports();
		ret->var_int = radstat.port_active_count
			    + radstat.port_idle_count;
		break;

	case MIB_KEY_radiusStatTotalLinesInUse:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		stat_count_ports();
		ret->var_int = radstat.port_active_count;
		break;

	case MIB_KEY_radiusStatTotalLinesIdle:
		ret->type = SMI_COUNTER32;
		ret->val_length = sizeof(counter);
		stat_count_ports();
		ret->var_int = radstat.port_idle_count;
		break;
		
	default:
		*errp = SNMP_ERR_NOSUCHNAME;
		snmp_var_free(ret);
		return NULL;
	}
	return ret;
}

void
get_stat_nasstat(nas, var, ind)
	NAS *nas;
	struct snmp_var *var;
	int ind;
{
	switch (ind) {
	case MIB_KEY_NASIndex:
		var->type = ASN_INTEGER;
		var->val_length = sizeof(int);
		var->var_int = nas->nas_stat->index;
		break;

	case MIB_KEY_NASAddress:
		var->type = SMI_IPADDRESS;
		var->val_length = sizeof(UINT4);
		var->var_str = snmp_alloc(sizeof(UINT4));
		*(UINT4*)var->var_str = ntohl(nas->nas_stat->ipaddr);
		break;

	case MIB_KEY_NASID:
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(nas->longname);
		var->var_str = snmp_strdup(nas->longname);
		break;

	case MIB_KEY_NASLines:
		stat_count_ports();
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->ports_active +
			       nas->nas_stat->ports_idle;
		break;

	case MIB_KEY_NASLinesInUse:
		stat_count_ports();
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->ports_active;
		break;

	case MIB_KEY_NASLinesIdle:
		stat_count_ports();
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = nas->nas_stat->ports_idle;
		break;

	}
}

struct snmp_var *
snmp_stat_client_var(list, errp)
	struct snmp_var *list;
	int *errp;
{
	struct snmp_var *ret;
	NAS *nas;
	
	ret = snmp_var_create(list->name);
	*errp = SNMP_ERR_NOERROR;

	switch (SUBID(list->name,MIB_POS_radiusStatNASEntry)) {
	case MIB_KEY_NASIndex:
	case MIB_KEY_NASAddress:
	case MIB_KEY_NASID:
	case MIB_KEY_NASLines:
	case MIB_KEY_NASLinesInUse:
	case MIB_KEY_NASLinesIdle:
		if (OIDLEN(list->name) == MIB_POS_radiusStatNASEntry_INDEX+1 &&
		    (nas = findnasbyindex(
			  SUBID(list->name,MIB_POS_radiusStatNASEntry_INDEX)))
		    != NULL &&
		    nas->nas_stat) {
 			get_stat_nasstat(nas, ret,
					 SUBID(list->name,
					       MIB_POS_radiusStatNASEntry));
			break;
		}
		/*FALLTHRU*/
	default:
		*errp = SNMP_ERR_NOSUCHNAME;
		snmp_var_free(ret);
		return NULL;
	}
	return ret;
}

Mib_tree_node *
snmp_stat_get(node, name)
	Mib_tree_node *node;
	oid_t name;
{
	int ind = SUBID(name,MIB_POS_radiusStatNASEntry_INDEX);
	NAS *nas = findnasbyindex(ind);

	if (!nas)
		return 0;
	SUBID(node->name,MIB_POS_radiusStatNASEntry_INDEX) = ind;
	return node;
}

Mib_tree_node *
snmp_stat_getnext(node, name)
	Mib_tree_node *node;
	oid_t name;
{
	int ind;
	NAS *nas;
	char buf1[MAXOIDLEN], buf2[MAXOIDLEN];
	
	debug(2, ("node %s, name %s",
		 sprint_oid(buf1, sizeof(buf1), node->name),
		 sprint_oid(buf2, sizeof(buf2), name)));

	if (oidcmp(OIDPTR(name), OIDPTR(node->name),
		   OIDLEN(node->name)-1)) {
		ind = 1;
	} else {
		ind = SUBID(name,MIB_POS_radiusStatNASEntry_INDEX) + 1;
	} 
	nas = findnasbyindex(ind);
	
	if (!nas) {
		debug(2,("no more nodes"));
 		return 0;
	}
	
	debug(2, ("NAS %d: %s", ind, nas->shortname));

	SUBID(node->name,MIB_POS_radiusStatNASEntry_INDEX) = ind;
	return node;
}

#define TDIFF(tv, time) (tv.tv_sec - time)*100 + tv.tv_usec/10000;

char *
timestr(time)
	time_t time;
{
	char *p;
	int len;

	if (time == 0)
		return "N/A";
	p = ctime(&time);
	len = strlen(p);
	if (len > 1) 
		p[--len] = 0;
	return p;
}

void
get_port_stat(port, var, ind)
	PORT_STAT *port;
	struct snmp_var *var;
	int ind;
{
	struct timeval tv;
	struct timezone tz;
	char *p;
	NAS *nas;
	
	switch (ind) {

	case MIB_KEY_radiusStatNASIndex:
		nas = nas_find(port->ip);
		var->type = ASN_INTEGER;
		var->val_length = sizeof(counter);
		if (nas)
			var->var_int = nas->nas_stat->index;
		else
			var->var_int = 0;
		break;

	case MIB_KEY_radiusStatPortID:
		var->type = ASN_INTEGER;
		var->val_length = sizeof(counter);
		var->var_int = port->port_no;
		break;

	case MIB_KEY_radiusStatPortFramedAddress:
		var->type = SMI_IPADDRESS;
		var->val_length = sizeof(UINT4);
		var->var_str = snmp_alloc(sizeof(UINT4));
		*(UINT4*)var->var_str = port->framed_address;
		break;

	case MIB_KEY_radiusStatPortTotalLogins:                 
		var->type = SMI_COUNTER32;
		var->val_length = sizeof(counter);
		var->var_int = port->count;
		break;
		
	case MIB_KEY_radiusStatPortStatus:        
		var->type = ASN_INTEGER;
		var->val_length = sizeof(counter);
		var->var_int = port->active ? port_active : port_idle;
		break;

	case MIB_KEY_radiusStatPortStatusDate:
		p = timestr(port->start);
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(p);
		var->var_str = snmp_strdup(p);
		break;

	case MIB_KEY_radiusStatPortUpTime:         
		gettimeofday(&tv, &tz);
		var->type = SMI_TIMETICKS;
		var->val_length = sizeof(counter);
		var->var_int = TDIFF(tv, port->start);
		break;
		
	case MIB_KEY_radiusStatPortLastLoginName:             
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(port->login)+1;
		var->var_str = snmp_strdup(port->login);
		break;

	case MIB_KEY_radiusStatPortLastLoginDate:
		p = timestr(port->lastin);
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(p);
		var->var_str = snmp_strdup(p);
		break;

	case MIB_KEY_radiusStatPortLastLogoutDate:      
		p = timestr(port->lastout);
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(p);
		var->var_str = snmp_strdup(p);
		break;

	case MIB_KEY_radiusStatPortIdleTotalTime:     
		var->type = SMI_TIMETICKS;
		var->val_length = sizeof(counter);
		var->var_int = port->idle * 100;
		break;
		
	case MIB_KEY_radiusStatPortIdleMaxTime:      
		var->type = SMI_TIMETICKS;
		var->val_length = sizeof(counter);
		var->var_int = port->maxidle.time * 100;
		break;
		
	case MIB_KEY_radiusStatPortIdleMaxDate:
		p = timestr(port->maxidle.start);
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(p);
		var->var_str = snmp_strdup(p);
		break;

	case MIB_KEY_radiusStatPortInUseTotalTime:        
		var->type = SMI_TIMETICKS;
		var->val_length = sizeof(counter);
		var->var_int = port->inuse * 100;
		break;
		
	case MIB_KEY_radiusStatPortInUseMaxTime:     
		var->type = SMI_TIMETICKS;
		var->val_length = sizeof(counter);
		var->var_int = port->maxinuse.time * 100;
		break;
		
	case MIB_KEY_radiusStatPortInUseMaxDate:
		p = timestr(port->maxinuse.start);
		var->type = ASN_OCTET_STR;
		var->val_length = strlen(p);
		var->var_str = snmp_strdup(p);
		break;
	}
}

struct snmp_var *
snmp_stat_port_var(list, errp)
	struct snmp_var *list;
	int *errp;
{
	struct snmp_var *ret;
	PORT_STAT *port;
	
	ret = snmp_var_create(list->name);
	*errp = SNMP_ERR_NOERROR;

	switch (SUBID(list->name,MIB_POS_NASPortEntry)) {

	case MIB_KEY_radiusStatPortIndex:
	case MIB_KEY_radiusStatNASIndex:
	case MIB_KEY_radiusStatPortID:
	case MIB_KEY_radiusStatPortFramedAddress:
	case MIB_KEY_radiusStatPortTotalLogins:                 
	case MIB_KEY_radiusStatPortStatus:        
	case MIB_KEY_radiusStatPortStatusDate:             
	case MIB_KEY_radiusStatPortUpTime:         
	case MIB_KEY_radiusStatPortLastLoginName:             
	case MIB_KEY_radiusStatPortLastLoginDate:      
	case MIB_KEY_radiusStatPortLastLogoutDate:      
	case MIB_KEY_radiusStatPortIdleTotalTime:     
	case MIB_KEY_radiusStatPortIdleMaxTime:      
	case MIB_KEY_radiusStatPortIdleMaxDate:        
	case MIB_KEY_radiusStatPortInUseTotalTime:        
	case MIB_KEY_radiusStatPortInUseMaxTime:     
	case MIB_KEY_radiusStatPortInUseMaxDate:       

		if (OIDLEN(list->name) == MIB_POS_NASPortEntry_INDEX+1 &&
		    (port = findportbyindex(
			 SUBID(list->name,MIB_POS_NASPortEntry_INDEX)))
		    != NULL) {
 			get_port_stat(port,
				      ret,
				      SUBID(list->name,
					    MIB_POS_radiusStatNASEntry));
			break;
		}
		/*FALLTHRU*/

	default:
		*errp = SNMP_ERR_NOSUCHNAME;
		snmp_var_free(ret);
		return NULL;
	}
	return ret;
}

Mib_tree_node *
snmp_stat_port_get(node, name)
	Mib_tree_node *node;
	oid_t name;
{
	int port_ind = SUBID(name,MIB_POS_NASPortEntry_INDEX);

	if (!findportbyindex(port_ind))
		return 0;
	
	SUBID(node->name,MIB_POS_NASPortEntry_INDEX) = port_ind;
	return node;
}

Mib_tree_node *
snmp_stat_port_getnext(node, name)
	Mib_tree_node *node;
	oid_t name;
{
	int ind;
	PORT_STAT *port;
	char buf1[MAXOIDLEN], buf2[MAXOIDLEN];
	
	debug(2, ("node %s, name %s",
		 sprint_oid(buf1, sizeof(buf1), node->name),
		 sprint_oid(buf2, sizeof(buf2), name)));

	if (oidcmp(OIDPTR(name), OIDPTR(node->name),
		   OIDLEN(node->name)-1)) {
		ind = 1;
	} else {
		ind = SUBID(name,MIB_POS_NASPortEntry_INDEX) + 1;
	} 

	port = findportbyindex(ind);
	
	if (!port) {
		debug(2,("no more nodes"));
 		return 0;
	}
	
	debug(2,
		("PORT %d: %d", ind, port->port_no));

	SUBID(node->name,MIB_POS_NASPortEntry_INDEX) = ind;
	return node;
}

#endif






