/* This file is part of GNU RADIUS.
 * Copyright (C) 2000, Sergey Poznyakoff
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
#define RADIUS_MODULE 3
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(USE_SQL) 
#if USE_SQL == SQL_POSTGRES

#ifndef lint
static char rcsid[] =
 "@(#) $Id$" ;
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <radiusd.h>
#include <radsql.h>

#define CI_HOST     "host="
#define CI_DBNAME   "dbname="
#define CI_USER     "user="
#define CI_PASSWORD "password="

#define CI_SIZE sizeof(CI_HOST) +\
                sizeof(CI_DBNAME) +\
                sizeof(CI_USER) +\
                sizeof(CI_PASSWORD)

static char *
postgres_conninfo(type)
	int type;
{
	char *dbname;
	char *conninfo;
	int  len;
	
	switch (type) {
	case SQL_AUTH:
		dbname = sql_cfg.auth_db;
		break;
	case SQL_ACCT:
		dbname = sql_cfg.acct_db;
		break;
	}

	len = CI_SIZE +
		strlen(sql_cfg.server) +
		strlen(dbname) +
		strlen(sql_cfg.login) +
		strlen(sql_cfg.password) + 1;
	conninfo = emalloc(len);

	sprintf(conninfo,
		"%s%s %s%s %s%s %s%s",
		CI_HOST, sql_cfg.server,
		CI_DBNAME, dbname,
		CI_USER, sql_cfg.login,
		CI_PASSWORD, sql_cfg.password);
	return conninfo;
}

/* ************************************************************************* */
/* Interface routines */
int
rad_sql_reconnect(type, conn)
	int    type;
	struct sql_connection *conn;
{
#if 1
	PGconn  *pgconn;
	char *dbname;
	char portbuf[16];
	char *portstr;
	
	switch (type) {
	case SQL_AUTH:
		dbname = sql_cfg.auth_db;
		break;
	case SQL_ACCT:
		dbname = sql_cfg.acct_db;
		break;
	}

	if (sql_cfg.port == 0)
		portstr = NULL;
	else {
		portstr = portbuf;
		snprintf(portbuf, sizeof portbuf, "%d", sql_cfg.port);
	}
		
	pgconn = PQsetdbLogin(sql_cfg.server, portstr, NULL, NULL,
			      dbname,
			      sql_cfg.login, sql_cfg.password);
	
	if (PQstatus(pgconn) == CONNECTION_BAD) {
		radlog(L_ERR,
		       _("PQconnectStart failed: %s"), PQerrorMessage(pgconn));
		PQfinish(pgconn);
		return -1;
	}

	conn->data = pgconn;
	conn->connected = 1;
	return 0;

#else
	PGconn  *pgconn;
	int     sockfd;
	int     rc, active;
	struct timeval tv;
	fd_set  fds;
	char    *p;

	p = postgres_conninfo(type);
	debug(1, ("conninfo = \"%s\"", p));
	pgconn = PQconnectStart(p);
	efree(p);
	if (!pgconn) {
		radlog(L_ERR,
		       _("PQconnectStart failed"));
		return -1;
	}

	if (PQstatus(pgconn) == CONNECTION_BAD) {
		radlog(L_ERR,
		       _("PQconnectStart failed: %s"), PQerrorMessage(pgconn));
		PQfinish(pgconn);
	}
	
	sockfd = PQsocket(pgconn);
	active = 0;
	while ((rc = PQconnectPoll(pgconn)) != PGRES_POLLING_OK) {
		switch (rc) {
		case PGRES_POLLING_ACTIVE:
			active++;
			break;
			
		case PGRES_POLLING_READING:
			FD_ZERO(&fds);
			FD_SET(sockfd, &fds);
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			rc = select(sockfd+1, &fds, NULL, NULL, &tv);
			if (rc == -1) 
				goto err;
			active = FD_ISSET(sockfd, &fds);
			break;
				
		case PGRES_POLLING_WRITING:
			FD_ZERO(&fds);
			FD_SET(sockfd, &fds);
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			rc = select(sockfd+1, NULL, &fds, NULL, &tv);
			if (rc == -1)
				goto err;
			active = FD_ISSET(sockfd, &fds);
			break;
			
		case PGRES_POLLING_FAILED:
	err:
			PQfinish(pgconn);
			return -1;
		}
	} 

	conn->data = pgconn;
	conn->connected = 1;
	return 0;
#endif
}

void 
rad_sql_disconnect(conn)
	struct sql_connection *conn;
{
	if (!conn->data)
		return ;
	PQfinish(conn->data);
	conn->data = NULL;
	conn->connected = 0;
}

int
rad_sql_query(conn, query, return_count)
	struct sql_connection *conn;
	char *query;
	int *return_count;
{
	PGresult       *res;
	ExecStatusType stat;
	int            rc;

	if (!conn || !conn->data)
		return -1;

	debug(1, ("query: %s", query));
	
	res = PQexec((PGconn*)conn->data, query);
	if (res == NULL) {
		radlog(L_ERR,
		       _("PQexec: %s"),
		       PQerrorMessage((PGconn*)conn->data));
		return -1;
	}
	
	stat = PQresultStatus(res);

	debug(1,
	      ("status: %s",
	      PQresStatus(stat)));

	if (stat == PGRES_COMMAND_OK) {
		if (return_count)
			*return_count = atoi(PQcmdTuples(res));
		rc = 0;
	} else {
		radlog(L_ERR,
		       ("PQexec returned %s"),
		       PQresStatus(stat));
		rc = -1;
	}
	PQclear(res);
	return rc;
}

char *
rad_sql_getpwd(conn, query)
	struct sql_connection *conn;
	char *query;
{
	PGresult       *res;
	ExecStatusType stat;
	char           *return_passwd = NULL;
	
	if (!conn || !conn->data)
		return NULL;

	debug(1, ("query: %s", query));
	
	res = PQexec((PGconn*)conn->data, query);
	if (res == NULL) {
		radlog(L_ERR,
		       _("PQexec: %s"),
		       PQerrorMessage((PGconn*)conn->data));
		return NULL;
	}
	
	stat = PQresultStatus(res);

	debug(1,
	      ("status: %s",
	      PQresStatus(stat)));

	if (stat == PGRES_TUPLES_OK) {
		if (PQntuples(res) != 1 && PQnfields(res)) {
			radlog(L_ERR,
			       _("malformed query: %s"),
			       query);
		} else {
			return_passwd = estrdup(PQgetvalue(res, 0, 0));
		}
	} else {
		radlog(L_ERR,
		       _("PQexec returned %s"),
		       PQresStatus(stat));
	}
	PQclear(res);
	return return_passwd;
}

#endif
#endif

