/* This file is part of GNU RADIUS.
   Copyright (C) 2001 Sergey Poznyakoff
  
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#define RADIUS_MODULE_SECURID_C

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef USE_SECURID

/** SecurID interface.
 */

#ifndef lint
static char rcsid[] = "@(#) $Id$";
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <pwd.h>
#include <time.h>
#include <ctype.h>

#include <radiusd.h>

#ifdef USE_SECURID_LEGACY
#include "sdi_athd.h"
#include "sdi_size.h"
#include "sdi_type.h"
#include "sdi_defs.h"
#include "sdconf.h"
#include "sdacmvls.h"

unsigned long strtoul ();

union config_record configure;
#else
#include "acexport.h"
#endif

#ifndef SDACE_FILE
#define SDACE_FILE "/etc/sdace.txt"
#endif

#ifdef USE_SECURID_LEGACY

/** Fake the new (5.x onward) synchronous functions
 */
typedef struct SD_CLIENT *SDI_HANDLE;

int
AceInitialize()
{
	return creadcfg() < 0 ? 0 : 1;
}

int
SD_Init(hdlp)
	SDI_HANDLE *hdlp;
{
	SDI_HANDLE hdl;

	*hdlp = hdl = calloc(1, sizeof(struct SD_CLIENT));
	if (sd_init(hdl) == 0)
		return ACM_OK;
	free(hdl);

	return 1;
}

int
SD_Check(hdl, passcode, name)
	SDI_HANDLE hdl;
	char *passcode;
	char *name;
{
	char *spasscode;
	int ret;

	/* sd_check overwrites the argument, but the 5.x SD_Check does not */

	spasscode = estrdup(passcode);
	ret = sd_check(spasscode, name, hdl);
	memset(spasscode, '\0', strlen(spasscode));
	efree(spasscode);

	return ret;
}

int
SD_ClientCheck(hdl, passcode, name, client)
	SDI_HANDLE hdl;
	char *passcode;
	char *name;
	unsigned long client;
{
	/* not implemented - treat as a normal non-client specific check */

	return SD_Check(hdl, passcode, name);
}

int SD_Close(hdl)
	SDI_HANDLE hdl;
{

	sd_close();
	free(hdl);
}
#endif /* USE_SECURID_LEGACY */

int
securid_pass(name, passcode, reply_msg, radreq)
	char *name;
	char *passcode;
	char **reply_msg;
	RADIUS_REQ *radreq;
{
	char buf[1024];
	SDI_HANDLE acehdl;
	int status;
	time_t t;

	debug(1, ("securid_pass: %s", name));
	
	time(&t);

	*reply_msg = NULL;
	if (!AceInitialize()) {
		debug(1, ("AceInitialize failed"));
	  	*reply_msg = make_string("internal error: AceInitialize");
		return 1;
	}

	if ((status = SD_Init(&acehdl)) != ACM_OK) {
		debug(1, ("SD_init failed: %d", status));
	  	*reply_msg = make_string("internal error: SD_Init");
		return 1;
	}

	if (ace_client_ip)
		status = SD_ClientCheck(acehdl, passcode, name,
					radreq->ipaddr);
	else
		status = SD_Check(acehdl, passcode, name);
	SD_Close(acehdl);

	switch (status)	{
	case ACM_OK:
		debug(1, ("%s/%s, access allowed",name,passcode));
		return 0;

#ifndef USE_SECURID_LEGACY
	case ACE_UNDEFINED_PASSCODE:
	case ACE_UNDEFINED_USERNAME:
	case ACE_ERR_INVALID_HANDLE:
		debug(1, ("%s/%s, internal error - status: %d",
			  name, passcode, status));
	  	*reply_msg = make_string("internal error");
		break;
#endif
		
	case ACM_ACCESS_DENIED:
		debug(1, ("%s/%s, access denied",name,passcode));
	  	*reply_msg = make_string("access denied");
		break;
		
	case ACM_NEXT_CODE_REQUIRED:
		debug(1,("%s/%s, next token mode",name,passcode));
		radlog(L_INFO, _("SecurID token in next token mode: %s"),
		       name);
	  	*reply_msg = make_string("next token mode");
		break;
		
	case ACM_NEW_PIN_REQUIRED:
		debug(1,("%s/%s, new pin mode", name, passcode));
		radlog(L_INFO, _("SecurID token in new pin mode: %s"), name);
	  	*reply_msg = make_string("new pin mode");
		break;
		
	}
	return 1;
}

#endif
