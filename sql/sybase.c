/* This file is part of GNU RADIUS.
   Copyright (C) 2000,2001 Sergey Poznyakoff
   Copyright (C) 2002 Steve Bleazard
  
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.*/

#define RADIUS_MODULE_SYBASE_C
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef lint
static char rcsid[] =
 "@(#) $Id$" ;
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <radiusd.h>
#include <radsql.h>

#ifdef USE_SQL_SYBASE

#include <ctpublic.h>

struct sybase_connection {
	CS_CONTEXT *context;
	CS_CONNECTION *connection;
	CS_COMMAND *cmd;
};

static int rad_sybase_reconnect(int type, struct sql_connection *conn);
static void rad_sybase_disconnect(struct sql_connection *conn);
static int rad_sybase_query(struct sql_connection *conn, char *query, int *return_count);
static char *rad_sybase_getpwd(struct sql_connection *conn, char *query);
static void *rad_sybase_exec(struct sql_connection *conn, char *query);
static char *rad_sybase_column(void *data, int ncol);
static int rad_sybase_next_tuple(struct sql_connection *conn, void *data);
static void rad_sybase_free(struct sql_connection *conn, void *data);

/* a suitable exit macro to handle the closedown and any messages */

#define CLEANUP_ON_FAIL(conn, ret, func, dbg) \
	if (ret != CS_SUCCEED) { \
		debug(1, dbg); \
		radlog(L_ERR, _(func " failed: %d"), ret); \
		if (conn != NULL) { \
			rad_sql_need_reconnect(conn->type); \
		} \
	}

#define RETURN_ON_FAIL(conn, ret, func, dbg, retval) \
	if (ret != CS_SUCCEED) { \
		CLEANUP_ON_FAIL(conn, ret, func, dbg) \
		return retval; \
	}

/** Error handler helper functions 
 */
static void
force_close_sybase_connection(sconn)
	struct sybase_connection *sconn;
{
	CS_RETCODE ret;

	if (sconn->cmd) {
		ret = ct_cmd_drop(sconn->cmd);
		if (ret != CS_SUCCEED) {
			debug(1, ("ct_cmd_drop failed: %d", ret));
			radlog(L_ERR, _("ct_cmd_drop failed: %d"), ret);
		}
		sconn->cmd = NULL;
	}
	if (sconn->connection) {
		ret = ct_close(sconn->connection, CS_FORCE_CLOSE);
		if (ret != CS_SUCCEED) {
			debug(1, ("ct_close failed: %d", ret));
			radlog(L_ERR, _("ct_close failed: %d"), ret);
		}
		ret = ct_con_drop(sconn->connection);
		if (ret != CS_SUCCEED) {
			debug(1, ("ct_con_drop failed: %d", ret));
			radlog(L_ERR, _("ct_con_drop failed: %d"), ret);
		}
		sconn->connection = NULL;
	}
}


/** Error callback functions - just log errors */

CS_RETCODE
clientmsg_callback(context, conn, emsgp)
	CS_CONTEXT *context;
	CS_CONNECTION *conn;
	CS_CLIENTMSG *emsgp;
{
	debug(1, ("sybase client library error: severity(%ld) layer(%ld) "
		  "origin(%ld) number(%ld), %s",
		  (long) CS_SEVERITY(emsgp->severity),
		  (long) CS_LAYER(emsgp->msgnumber),
		  (long) CS_ORIGIN(emsgp->msgnumber),
		  (long) CS_NUMBER(emsgp->msgnumber),
		  emsgp->msgstring));

	radlog(L_ERR,
	       _("sybase error: severity(%ld) layer(%ld) origin(%ld) "
		 "number(%ld), %s"),
	       (long) CS_SEVERITY(emsgp->severity),
	       (long) CS_LAYER(emsgp->msgnumber),
	       (long) CS_ORIGIN(emsgp->msgnumber),
	       (long) CS_NUMBER(emsgp->msgnumber),
	       emsgp->msgstring);

	if (emsgp->osstringlen > 0) {
		debug(1, ("sybase client library OS error: %ld (%s)",
			  emsgp->osnumber, emsgp->osstring));
		radlog(L_ERR, _("sybase os error: %ld (%s)"), 
		       (long) emsgp->osnumber, emsgp->osstring);
	}

	return CS_SUCCEED;
}

CS_RETCODE
csmsg_callback(context, conn, emsgp)
	CS_CONTEXT *context;
	CS_CLIENTMSG *emsgp;
{
	debug(1, ("sybase CS library error: severity(%ld) layer(%ld) "
		  "origin(%ld) number(%ld), %s",
		  (long) CS_SEVERITY(emsgp->severity),
		  (long) CS_LAYER(emsgp->msgnumber),
		  (long) CS_ORIGIN(emsgp->msgnumber),
		  (long) CS_NUMBER(emsgp->msgnumber),
		  emsgp->msgstring));

	radlog(L_ERR,
	       _("sybase error: severity(%ld) layer(%ld) origin(%ld) "
		 "number(%ld), %s"),
	       (long) CS_SEVERITY(emsgp->severity),
	       (long) CS_LAYER(emsgp->msgnumber),
	       (long) CS_ORIGIN(emsgp->msgnumber),
	       (long) CS_NUMBER(emsgp->msgnumber),
	       emsgp->msgstring);

	if (emsgp->osstringlen > 0) {
		debug(1, ("sybase CS library OS error: %ld (%s)",
			  (long)emsgp->osnumber, emsgp->osstring));
		radlog(L_ERR, _("sybase os error: %ld (%s)"), 
		       (long) emsgp->osnumber, emsgp->osstring);
	}

	return CS_SUCCEED;
}

CS_RETCODE
servermsg_callback(context, conn, smsgp)
	CS_CONTEXT *context;
	CS_CONNECTION *conn;
	CS_SERVERMSG *smsgp;
{
	char *s, *p, *sf, *pf;

	s = p = sf = pf = "";
	if (smsgp->svrnlen > 0) {
		s = smsgp->svrname;
		sf = ", server: ";
	}
	if (smsgp->proclen > 0) {
		s = smsgp->proc;
		sf = ", proc: ";
	}

	debug(1, ("sybase server message: number(%ld) severity(%ld) "
		  "state(%ld) line(%ld)%s%s%s%s, %s",
		  smsgp->msgnumber, smsgp->severity, smsgp->state, 
		  smsgp->line, sf, s, pf, p, smsgp->text));

	radlog(L_ERR, _("sybase server message: number(%ld) severity(%ld) "
		        "state(%ld) line(%ld)%s%s%s%s, %s"),
			smsgp->msgnumber, smsgp->severity, smsgp->state, 
			smsgp->line, sf, s, pf, p, smsgp->text);

	return CS_SUCCEED;


}

/** Helper functions */
/* TODO */

static CS_COMMAND *
sybase_alloc_cmd(sybcon, conn) 
	struct sybase_connection *sybcon;
	struct sql_connection *conn;
{
	CS_RETCODE ret;
	CS_COMMAND *cmd;

	ret = ct_cmd_alloc(sybcon->connection, &cmd);
	RETURN_ON_FAIL(conn, ret, "ct_cmd_alloc",
			("ct_cmnd_alloc failed: %d", ret), NULL);

	return cmd;
}

static int
sybase_db_use(sybcon, database, conn)
	struct sybase_connection *sybcon;
	char *database;
	struct sql_connection *conn;
{
	CS_COMMAND *cmd = sybase_alloc_cmd(sybcon, conn);
	CS_RETCODE ret;
	CS_INT restype;
	char statement[255];

	if (!cmd)
		return -1;

	snprintf(statement, sizeof statement, "use %s", database);
	ret = ct_command(cmd, CS_LANG_CMD, statement, CS_NULLTERM, CS_UNUSED);
	RETURN_ON_FAIL(conn, ret, "ct_command",
			("ct_command failed: %d", ret), -1);

	ret = ct_send(cmd);
	RETURN_ON_FAIL(conn, ret, "ct_send",
			("ct_send failed: %d", ret), -1);

	while ((ret = ct_results(cmd, &restype)) == CS_SUCCEED) {
		if (restype == CS_CMD_FAIL)
			radlog(L_ERR, _("'%s' failed: %d"), statement, ret);
	}
	ret = ct_cmd_drop(cmd);
	RETURN_ON_FAIL(conn, ret, "ct_cmd_drop",
			("ct_cmd_drop failed: %d", ret), -1);

	return 0;
}


/* ************************************************************************* */
/* Interface routines */
int
rad_sybase_reconnect(type, conn)
	int    type;
	struct sql_connection *conn;
{
	struct sybase_connection *sybcon;
	char *dbname;
	CS_RETCODE ret;
#define XRETURN_ON_FAIL(ret, func, dbg) \
	if (ret != CS_SUCCEED) { \
		debug(1, dbg); \
		radlog(L_ERR, _(func " failed: %d"), ret); \
		force_close_sybase_connection(sybcon); \
		if (sybcon->context != NULL) { \
			ct_exit(sybcon->context, CS_FORCE_EXIT); \
			cs_ctx_drop(sybcon->context); \
			sybcon->context = NULL; \
		} \
		conn->data = NULL; \
		conn->connected = 0; \
		efree(sybcon); \
		return -1; \
	}

	switch (type) {
	case SQL_AUTH:
		dbname = sql_cfg.auth_db;
		break;
		
	case SQL_ACCT:
		dbname = sql_cfg.acct_db;
		break;
	}

	/* Create a context and initialise */

	conn->data = sybcon = emalloc(sizeof *sybcon);
	conn->connected = 0;
	sybcon->context = NULL;

	debug(10, ("Initialising sybase cs/ct library"));
	ret = cs_ctx_alloc(CS_VERSION_100, &sybcon->context);
	XRETURN_ON_FAIL(ret,"cs_ctx_alloc",("cs_ctx_alloc failed: %d",ret));

	ret = ct_init(sybcon->context, CS_VERSION_100);
	XRETURN_ON_FAIL(ret, "ct_init", ("ct_init failed: %d", ret));

	/* install callbacks */

	debug(10, ("Installing callbacks"));
	ret = cs_config(sybcon->context, CS_SET, CS_MESSAGE_CB,
			(CS_VOID *)csmsg_callback, CS_UNUSED, NULL);
	XRETURN_ON_FAIL(ret, "cs_config",
		("cs_config failed installing cs callback: %d", ret));

	ret = ct_callback(sybcon->context, NULL, CS_SET, CS_CLIENTMSG_CB,
			  (CS_VOID *)clientmsg_callback);
	XRETURN_ON_FAIL(ret, "ct_callback",
		("ct_callback failed installing client callback: %d", ret));

	ret = ct_callback(sybcon->context, NULL, CS_SET, CS_SERVERMSG_CB,
			  (CS_VOID *)servermsg_callback);
	XRETURN_ON_FAIL(ret, "ct_callback",
		("ct_callback failed installing server callback: %d", ret));

	/* Connect to the server, first alloc a connection structure and
	 * set the username and password.
	 */

	debug(10, ("connecting to server '%s' with username/pw: %s/%s",
		   sql_cfg.server, sql_cfg.login, sql_cfg.password));
	ret = ct_con_alloc(sybcon->context, &sybcon->connection);
	XRETURN_ON_FAIL(ret,"ct_con_alloc",("ct_con_alloc failed: %d", ret));
	
	ret = ct_con_props(sybcon->connection, CS_SET, CS_USERNAME,
			   sql_cfg.login, CS_NULLTERM, NULL);
	XRETURN_ON_FAIL(ret, "ct_con_props",
			("ct_con_props(username) failed: %d", ret));

	ret = ct_con_props(sybcon->connection, CS_SET, CS_PASSWORD,
			   sql_cfg.password, CS_NULLTERM, NULL);
	XRETURN_ON_FAIL(ret, "ct_con_props",
			("ct_con_props(password) failed: %d", ret));

	/** Now connect to the server.  Note that the server connection
	 *  details (ipaddress, port etc) are contained in the Sybase
	 *  "interfaces" file and the server name is simply a reference into
	 *  this file.
	 */

	ret = ct_connect(sybcon->connection, sql_cfg.server, CS_NULLTERM);
	XRETURN_ON_FAIL(ret, "ct_connect", ("ct_connect failed: %d", ret));

	/* connection complete - mark the connection connected */

	debug(10, ("connected to database"));
	conn->connected = 1;
	
	/* force the correct database */

	if (sybase_db_use(sybcon, dbname, conn) < 0) {
		conn->data = NULL;
		conn->connected = 0;
		efree(sybcon);
		return -1;
	}

	return 0;

#undef XRETURN_ON_FAIL
}

void 
rad_sybase_disconnect(conn)
	struct sql_connection *conn;
{
	struct sybase_connection *sybcon;
	CS_RETCODE ret;

	conn->connected = 0;
	if (!conn->data)
		return;
	sybcon = conn->data;
	conn->data = NULL;

	if (sybcon->context != NULL) {
		force_close_sybase_connection(sybcon);

		ret = ct_exit(sybcon->context, CS_FORCE_EXIT);
		if (ret != CS_SUCCEED) {
			debug(1, ("ct_exit failed: %d", ret));
			radlog(L_ERR, _("ct_exit failed: %d"), ret);
		}

		cs_ctx_drop(sybcon->context);
		if (ret != CS_SUCCEED) {
			debug(1, ("cs_ctx_drop failed: %d", ret));
			radlog(L_ERR, _("cs_ctx_drop failed: %d"), ret);
		}
		sybcon->context = NULL;
	}

	efree(sybcon);
}

static int
sybase_do_query(conn, query)
	struct sql_connection *conn;
	char *query;
{
	struct sybase_connection *sybcon;
	CS_RETCODE ret;

	sybcon = conn->data;
	if (!conn->connected || sybcon == NULL || sybcon->context == NULL) {
		if (rad_sybase_reconnect(conn->type, conn) < 0)
			return -1;
		sybcon = conn->data;
	}

	if ((sybcon->cmd = sybase_alloc_cmd(sybcon, conn)) == NULL)
		return -1;

	ret = ct_command(sybcon->cmd,CS_LANG_CMD,query,CS_NULLTERM,CS_UNUSED);
	RETURN_ON_FAIL(conn, ret, "ct_command",
			("ct_command failed: %d", ret), -1);

	ret = ct_send(sybcon->cmd);
	RETURN_ON_FAIL(conn, ret, "ct_send", ("ct_send failed: %d", ret), -1);

	return 0;
}

/** rad_sybase_query - action a query without returning data.  This is usually
 *  used for insert queries.  Return the number of rows effected.
 */
int
rad_sybase_query(conn, query, return_count)
	struct sql_connection *conn;
	char *query;
	int *return_count;
{
	struct sybase_connection *sybcon;
	CS_RETCODE ret, res_ret;
	CS_INT result_type;
	CS_INT count, ocnt, fcnt;

	if (!conn)
		return -1;
	debug(2, ("query: %s", query));

	if (sybase_do_query(conn, query) < 0)
		return -1;

	sybcon = conn->data;
	count = -1;

	/* run through the rows until complete */

	while ((res_ret = ct_results(sybcon->cmd, &result_type))==CS_SUCCEED) {
		switch (result_type) {
		case CS_ROW_RESULT:
		case CS_PARAM_RESULT:
		case CS_STATUS_RESULT:
		case CS_CURSOR_RESULT:
		case CS_COMPUTE_RESULT:
			debug(1, ("results return in rad_sybase_query"));
			while ((ret = ct_fetch(sybcon->cmd, CS_UNUSED,
						CS_UNUSED, CS_UNUSED,
						&fcnt)) == CS_SUCCEED
			       || ret == CS_ROW_FAIL) {

				/* report any row errors */
				if (ret == CS_ROW_FAIL) {
					debug(1, ("ct_fetch row error", ret));
					radlog(L_ERR, _("ct_fetch row error"));
				}
			}
			if (ret != CS_END_DATA)	{
				RETURN_ON_FAIL(conn, ret, "ct_fetch",
					("ct_fetch failed: %d", ret), -1);
			}
			break;
			
		case CS_CMD_SUCCEED:
		case CS_CMD_DONE:
			debug(1, ("ct_results cmd success"));
			ret = ct_res_info(sybcon->cmd, CS_ROW_COUNT, &count,
						CS_UNUSED, &ocnt);
			RETURN_ON_FAIL(conn, ret, "ct_res_info",
					("ct_res_info failed: %d", ret), -1);
			break;
			
		case CS_CMD_FAIL:
			debug(1, ("ct_results cmd fail"));
			break;
			
		default:
			RETURN_ON_FAIL(conn, CS_FAIL, "ct_results",
					("ct_results failed: %d", res_ret),-1);

		}
	}

	if (res_ret == CS_END_RESULTS) {
		if (count == -1
		    && (result_type == CS_CMD_SUCCEED
			|| result_type == CS_CMD_DONE)) {
			ret = ct_res_info(sybcon->cmd, CS_ROW_COUNT, &count,
					  CS_UNUSED, &ocnt);
			RETURN_ON_FAIL(conn, ret, "ct_res_info",
				       ("ct_res_info failed: %d", ret), -1);
		}
	} else {
		RETURN_ON_FAIL(conn, res_ret, "ct_results",
				("ct_results failed: %d", res_ret), -1);
	}

	if (count == -1)
		count = 0;

	ret = ct_cmd_drop(sybcon->cmd);
	sybcon->cmd = NULL;
	RETURN_ON_FAIL(conn, ret, "ct_cmd_drop",
			("ct_cmd_drop failed: %d", ret), -1);

	debug(2, ("query effected %d row(s)", count));
	if (return_count)
		*return_count = count;
	
	return 0;
}

char *
rad_sybase_getpwd(conn, query)
	struct sql_connection *conn;
	char *query;
{
	struct sybase_connection *sybcon;
	CS_RETCODE ret, res_ret;
	CS_INT result_type;
	CS_INT fcnt;
	CS_DATAFMT passwd_data;
	CS_SMALLINT indicator;
	char passwd[512+1];
	char *return_passwd = NULL;

	if (!conn)
		return NULL;

	debug(1, ("query: %s", query));

	if (sybase_do_query(conn, query) < 0)
		return NULL;

	sybcon = conn->data;

	/* run through the rows getting the first row data */

	while ((res_ret = ct_results(sybcon->cmd, &result_type))==CS_SUCCEED) {
		switch (result_type) {
		case CS_ROW_RESULT:
		case CS_PARAM_RESULT:
		case CS_STATUS_RESULT:
		case CS_CURSOR_RESULT:
		case CS_COMPUTE_RESULT:
			/* Bind the first column to the password buffer */

			passwd_data.datatype = CS_CHAR_TYPE;
			passwd_data.format = CS_FMT_NULLTERM;
			passwd_data.maxlength = sizeof passwd - 1;
			passwd_data.count = 1;
			passwd_data.locale = NULL;
			ret = ct_bind(sybcon->cmd, 1, &passwd_data, passwd,
				      NULL, &indicator);
			RETURN_ON_FAIL(conn, ret, "cs_bind",
					("ct_bind failed: %d", ret), NULL);

			while ((ret = ct_fetch(sybcon->cmd, CS_UNUSED,
						CS_UNUSED, CS_UNUSED,
						&fcnt)) == CS_SUCCEED
			       || ret == CS_ROW_FAIL) {

				/* report any row errors */
				if (ret == CS_ROW_FAIL) {
					debug(1, ("ct_fetch row error", ret));
					radlog(L_ERR, _("ct_fetch row error"));
				}
				if (return_passwd == NULL && indicator >= 0)
					return_passwd = estrdup(passwd);
			}
			if (ret != CS_END_DATA) {
				RETURN_ON_FAIL(conn, ret, "ct_fetch",
					("ct_fetch failed: %d", ret), NULL);
			}
			break;
			
		case CS_CMD_SUCCEED:
		case CS_CMD_DONE:
		case CS_CMD_FAIL:
			break;
			
		default:
			RETURN_ON_FAIL(conn, CS_FAIL, "ct_results",
				       ("ct_results failed: %d",res_ret),
				       NULL);

		}
	}

	ret = ct_cmd_drop(sybcon->cmd);
	sybcon->cmd = NULL;
	RETURN_ON_FAIL(conn, ret, "ct_cmd_drop",
			("ct_cmd_drop failed: %d", ret), NULL);

	debug(2, ("password %sfound", return_passwd ? "" : "not "));
	debug(3, ("returning password:[%s]", return_passwd?return_passwd:""));

	return return_passwd;
}

#define SYBASE_EXEC_STATE_START		0
#define SYBASE_EXEC_STATE_RESULTS	1
#define SYBASE_EXEC_STATE_FETCH		2
#define SYBASE_EXEC_STATE_END		3
#define SYBASE_EXEC_STATE_ERROR		4

struct colinfo {
	CS_SMALLINT indicator;
	char *data;
	struct colinfo *next;
};

struct sybase_exec_data {
	int state;
	CS_RETCODE results_ret;
	CS_INT result_type;
	CS_RETCODE fetch_ret;
	struct colinfo *coldata;
	CS_COMMAND *cmd;
};

void *
rad_sybase_exec(conn, query)
	struct sql_connection *conn;
	char *query;
{
	struct sybase_connection *sybcon;
	struct sybase_exec_data *sinfo;

	if (!conn)
		return NULL;
	debug(2, ("query: %s", query));

	if (sybase_do_query(conn, query) < 0)
		return NULL;
	
	sybcon = conn->data;
	sinfo = emalloc(sizeof *sinfo);
	sinfo->state = SYBASE_EXEC_STATE_START;
	sinfo->cmd = sybcon->cmd;
	sinfo->results_ret = CS_SUCCEED;
	sinfo->result_type = CS_CMD_SUCCEED;
	sinfo->fetch_ret = CS_SUCCEED;
	sinfo->coldata = NULL;
	sybcon->cmd = NULL;

	return (void*)sinfo;
}

char *
rad_sybase_column(data, ncol)
	void *data;
	int ncol;
{
	struct sybase_exec_data *sinfo;
	struct colinfo *cp;
	int i;

	if (!data)
		return NULL;

	sinfo = (struct sybase_exec_data *)data;
	for (i=0, cp=sinfo->coldata; i<ncol && cp!=NULL; cp=cp->next, i++)
		;
	
	if (cp == NULL) {
		radlog(L_ERR,
		       _("too few columns returned (%d req'd)"), ncol);
		return NULL;
	}

	debug(10, ("col %d:[%s]", ncol, cp->data ? cp->data : ""));
	return cp->data;
}

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define MAX_CHAR_BUF 1024

static CS_INT
data_len(column)
CS_DATAFMT *column;
{
	CS_INT len;

	switch ((int) column->datatype) {
	case CS_CHAR_TYPE:
	case CS_VARCHAR_TYPE:
	case CS_TEXT_TYPE:
	case CS_IMAGE_TYPE:
		len = MIN(column->maxlength, MAX_CHAR_BUF);
		break;
		
	case CS_BINARY_TYPE:
	case CS_VARBINARY_TYPE:
		len = MIN((2 * column->maxlength) + 2, MAX_CHAR_BUF);
		break;
		
	case CS_BIT_TYPE:
	case CS_TINYINT_TYPE:
		len = 3;
		break;
		
	case CS_SMALLINT_TYPE:
		len = 6;
		break;
		
	case CS_INT_TYPE:
		len = 11;
		break;
		
	case CS_REAL_TYPE:
	case CS_FLOAT_TYPE:
		len = 20;
		break;
		
	case CS_MONEY_TYPE:
	case CS_MONEY4_TYPE:
		len = 24;
		break;
		
	case CS_DATETIME_TYPE:
	case CS_DATETIME4_TYPE:
		len = 30;
		break;
		
	case CS_NUMERIC_TYPE:
	case CS_DECIMAL_TYPE:
		len = (CS_MAX_PREC + 2);
		break;
		
	default:
		len = column->maxlength;
		break;
	}
    
	return MAX(strlen(column->name) + 1, len);
}

static struct colinfo *
describe(cmd)
	CS_COMMAND *cmd;
{
	
	CS_RETCODE ret;
	CS_INT num_cols, i;
	CS_DATAFMT datafmt;
	struct colinfo start, *curr;

	ret = ct_res_info(cmd, CS_NUMDATA, &num_cols, CS_UNUSED, NULL);
	if (ret != CS_SUCCEED) {
		debug(1, ("ct_res_info failed: %d", ret));
		radlog(L_ERR, _("ct_res_info failed: %d"), ret);
		return NULL;
	}
	if (num_cols <= 0) {
		debug(1, ("num_cols <= 0: %d", num_cols));
		radlog(L_ERR, _("num_cols <= 0: %d"), num_cols);
		return NULL;
	}

	debug(10, ("num_cols: %d", num_cols));
	for (curr = &start, i = 0; i < num_cols; i++) {
		if((ret = ct_describe(cmd, (i+1), &datafmt)) != CS_SUCCEED) {
			debug(1, ("ct_describe failed: %d", ret));
			radlog(L_ERR, _("ct_describe failed: %d"), ret);
			break;
		}
		datafmt.maxlength = data_len(&datafmt) + 1;
		datafmt.datatype = CS_CHAR_TYPE;
		datafmt.format   = CS_FMT_NULLTERM;
		debug(10, ("col %d len: %d", i, datafmt.maxlength));

		curr->next = emalloc(sizeof(struct colinfo));
		curr = curr->next;
		curr->data = emalloc(datafmt.maxlength);

		if ((ret = ct_bind(cmd, (i+1), &datafmt, curr->data, NULL,
				   &curr->indicator)) != CS_SUCCEED) {
			debug(1, ("ct_bind failed: %d", ret));
			radlog(L_ERR, _("ct_bind failed: %d"), ret);
			break;
		}
	}

	curr->next = NULL;
	return start.next;
}

static void
free_coldata(data)
	struct colinfo *data;
{
	struct colinfo *curr, *junk;

	for (curr = data; curr != NULL; ) {
		if (curr->data != NULL)
			efree(curr->data);
		junk = curr;
		curr = curr->next;
		efree(junk);
	}
}

/*ARGSUSED*/
int
rad_sybase_next_tuple(conn, data)
	struct sql_connection *conn;
	void *data;
{
	struct sybase_connection *sybcon;
	struct sybase_exec_data *sinfo;
	CS_INT fcnt;
	int loop = 1;

	if (!conn || !data)
		return -1;

	sybcon = conn->data;
	sinfo = (struct sybase_exec_data *)data;

	while (loop) {
		debug(10, ("state = %d", sinfo->state));
		switch (sinfo->state) {
		case SYBASE_EXEC_STATE_START:
		case SYBASE_EXEC_STATE_RESULTS:
			sinfo->state = SYBASE_EXEC_STATE_RESULTS;
			sinfo->results_ret =
				ct_results(sinfo->cmd, &sinfo->result_type);
			switch (sinfo->results_ret) {
			case CS_SUCCEED:
				switch (sinfo->result_type) {
				case CS_ROW_RESULT:
				case CS_PARAM_RESULT:
				case CS_STATUS_RESULT:
				case CS_CURSOR_RESULT:
				case CS_COMPUTE_RESULT:
					/* Setup the column bindings */
					if (sinfo->coldata != NULL)
						free_coldata(sinfo->coldata);
					sinfo->coldata = describe(sinfo->cmd);
					sinfo->state = SYBASE_EXEC_STATE_FETCH;
					break;
					
				case CS_CMD_SUCCEED:
				case CS_CMD_DONE:
					break;
					
				case CS_CMD_FAIL:
					/* error reported via callback */
					break;
					
				default:
					debug(2, ("ct_results err: %d",
						  sinfo->result_type));
					break;
				}
				break;
				
			case CS_END_RESULTS:
				sinfo->state = SYBASE_EXEC_STATE_END;
				break;
				
			default:
				sinfo->state = SYBASE_EXEC_STATE_ERROR;
				RETURN_ON_FAIL(conn, sinfo->results_ret,
					"ct_results", ("ct_results failed: %d",
					 	sinfo->results_ret), -1);
				loop = 0;
				return -1;
			}
			break;
			
		case SYBASE_EXEC_STATE_FETCH:
			loop = 0;
			sinfo->fetch_ret = ct_fetch(sinfo->cmd,
						    CS_UNUSED,
						    CS_UNUSED,
						    CS_UNUSED,
						    &fcnt);
			switch (sinfo->fetch_ret) {
			case CS_SUCCEED:
				break;
				
			case CS_ROW_FAIL:
				debug(2, ("ct_fetch row fail: %d",
					  sinfo->fetch_ret));
				break;
				
			case CS_END_DATA:
				sinfo->state = SYBASE_EXEC_STATE_RESULTS;
				loop = 1;
				break;
				
			default:
				debug(2,("ct_fetch fail: %d",sinfo->fetch_ret));
				sinfo->state = SYBASE_EXEC_STATE_ERROR;
				loop = 1;
				break;
			}
			break;
			
		case SYBASE_EXEC_STATE_END:
			loop = 0;
			return 1;
			break;
			
		case SYBASE_EXEC_STATE_ERROR:
			loop = 0;
			return -1;
		}
	}

	return 0;
}

/*ARGSUSED*/
void
rad_sybase_free(conn, data)
	struct sql_connection *conn;
	void *data;
{
	struct sybase_connection *sybcon;
	struct sybase_exec_data *sinfo;
	CS_RETCODE ret;

	if (!data)
		return;

	debug(20, ("freeing command data"));
	sinfo = (struct sybase_exec_data *)data;

	if (conn && conn->data)	{
		if (sinfo->state != SYBASE_EXEC_STATE_END) {
			debug(20, ("cancelling pending command"));
			ret = ct_cancel(NULL, sinfo->cmd, CS_CANCEL_ALL);
			debug(20, ("cancelled pending command"));
		}
		ret = ct_cmd_drop(sinfo->cmd);
	}

	if (sinfo->coldata)
		free_coldata(sinfo->coldata);

	debug(20, ("data freed"));

	efree(sinfo);
}

SQL_DISPATCH_TAB sybase_dispatch_tab[] = {
	"sybase",     
	3000,
	rad_sybase_reconnect,
	rad_sybase_disconnect,
	rad_sybase_query,
	rad_sybase_getpwd,
	rad_sybase_exec,
	rad_sybase_column,
	rad_sybase_next_tuple,
	rad_sybase_free
};

#endif

