/* This file is part of GNU Radius
   Copyright (C) 2000,2002,2003,2004 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <ctype.h>

#include <sysdep.h>
#include <radiusd.h>
#include <rewrite.h>

/* obstack_grow with quoting of potentially dangerous characters */
static void
obstack_grow_quoted(struct obstack *obp, char *str, int len)
{
        for (; len > 0; len--, str++) {
                switch (*str) {
                case '"':
                case '\'':
                case '\\':
                        obstack_1grow(obp, '\\');
			obstack_1grow(obp, *str);
			break;
			
                default:
			if (isprint(*str))
				obstack_1grow(obp, *str);
			else {
				char buf[4];
				snprintf(buf, sizeof(buf), "%03o",
					 *(u_char*)str);
				obstack_1grow(obp, '\\');
				obstack_grow(obp, str, 3);
			}
                }
        }
}

/*
 *      Replace %<whatever> in a string.
 *
 *      %Cnum          attribute number `num' from request pairs
 *      %C{attr-name}  attribute `attr-name' from request pairs
 *      %Rnum          attribute number `num' from reply pairs
 *      %R{attr-name}  attribute `attr-name' from reply pairs
 *      %D             current date/time (localtime)
 *      %G             current date/time (GMT)
 *      Shortcuts:
 *      %p             Port number
 *      %n             NAS IP address
 *      %f             Framed IP address
 *      %u             User name
 *      %c             Callback-Number
 *      %i             Calling-Station-Id
 *      %t             MTU
 *      %a             Protocol (SLIP/PPP)
 *      %s             Speed (DA_CONNECT_INFO)
 *
 */

/* Find attribute `attr' in pairlist `pairlist' and store it's formatted
 * value into obstack.
 * If no attribute found, store the provided default value (defval). If
 * the latter is NULL, store "unknown" for string type and "0" for
 * others.
 */
static void
attr_to_str(struct obstack *obp, grad_request_t *req, grad_avp_t *pairlist,
            grad_dict_attr_t  *attr, char *defval)
{
        grad_avp_t *pair;
        int len;
        char tmp[AUTH_STRING_LEN + 1];
        char *str;
	
        if (!attr) {
                grad_log_req(L_ERR, req, "attribute not found");
                return;
        }
        
        if ((pair = grad_avl_find(pairlist, attr->value)) == NULL) {
                if (!defval) {
                        if (attr->type == TYPE_STRING)
                                defval = "-";
                        else
                                defval = "-0";
                }
                
                switch (*defval++) {
                case '-':
                        len = strlen(defval);
                        obstack_grow(obp, defval, len);
                        break;
                case '+':
                        break;
                case '?':
                        if (*defval == 0)
                                defval = "Attribute is not present";
                        grad_log_req(L_ERR, req, "%s: %s",
				     attr->name, defval);
                        break;
                case '=':
                        if (pairlist) {
				grad_locus_t loc;
				loc.file = __FILE__; /*FIXME*/
				loc.line = __LINE__;
                                pair = grad_create_pair(&loc,
							attr->name,
							grad_operator_equal,
							defval);
                                if (pair)
                                        grad_avl_add_list(&pairlist, pair);
                        }
                        break;
                default:
                        if (defval)
                                grad_log(L_ERR, "invalid : substitution: %s",
                                         defval);
                        else
                                grad_log(L_ERR, "null : substitution");
                        break;
                }
                
                if (!pair)
                        return;
        } else if (defval && *defval == '+') {
                defval++;
                len = strlen(defval);
                obstack_grow(obp, defval, len);
                return;
        }

        tmp[AUTH_STRING_LEN-1] = 0;
        switch (attr->type) {
        case TYPE_STRING:
                if ((attr->prop & AP_ENCRYPT) && req) {
                        req_decrypt_password(tmp, req, pair);
			str = tmp;
		} else
			str = pair->avp_strvalue;

		if (attr->prop & AP_BINARY_STRING) 
			len = pair->avp_strlength;
		else
			/* strvalue might include terminating zero
			   character, so we need to recalculate it */
			len = strlen(str);
		obstack_grow_quoted(obp, str, len);
                break;
		
        case TYPE_INTEGER:
                snprintf(tmp, sizeof(tmp), "%lu", pair->avp_lvalue);
                len = strlen(tmp);
                obstack_grow(obp, tmp, len);
                break;
		
        case TYPE_IPADDR:
                grad_ip_iptostr(pair->avp_lvalue, tmp);
                len = strlen(tmp);
                obstack_grow(obp, tmp, len);
                break;
		
        case TYPE_DATE:
                snprintf(tmp, sizeof(tmp), "%ld", pair->avp_lvalue);
                len = strlen(tmp);
                obstack_grow(obp, tmp, len);
                break;
		
        default:
                grad_log(L_CRIT,
                         _("INTERNAL ERROR (%s:%d): attribute %d has bad type (%d)"),
                         __FILE__, __LINE__,
                         attr->value, attr->type);
                break;
        }
}

static void
curtime_to_str(struct obstack *obp, grad_avp_t *request, int gmt)
{
        time_t curtime;
        struct tm *tm, tms;
        grad_avp_t *pair;
        char tbuf[AUTH_STRING_LEN];
        int len;
        
        curtime = time(NULL);
        if (pair = grad_avl_find(request, DA_ACCT_DELAY_TIME))
                curtime -= pair->avp_lvalue;
        if (gmt)
                tm = gmtime(&curtime);
        else
                tm = localtime_r(&curtime, &tms);
                                
        len = strftime(tbuf, AUTH_STRING_LEN, "%Y-%m-%d %H:%M:%S", tm);
        obstack_grow(obp, tbuf, len);
}

/* Find attribute number `attr_no' in pairlist `pairlist' and store it's
 * formatted value into obstack.
 * If no attribute found, use provided default value (see comment to
 * attr_to_str)
 */
static void
attrno_to_str(struct obstack *obp, grad_request_t *req, grad_avp_t *pairlist,
              int attr_no, char *defval)
{
        attr_to_str(obp, req, pairlist,
		    grad_attr_number_to_dict(attr_no), defval);
}

static grad_dict_attr_t *
parse_dict_attr(char *p, char **endp, char **defval)
{
        char namebuf[MAX_DICTNAME];
        
        *defval = NULL;
        if (isdigit(*p)) {
                return grad_attr_number_to_dict(strtol(p, endp, 10));
        }

        if (*p == '{') {
                int len, off;
                
                p++;
                len = strlen(p);
                off = strcspn(p, ":}");

                if (off == len || off >= sizeof namebuf)
                        return NULL;

                strncpy(namebuf, p, off);
                namebuf[off] = 0;

                p += off;
                if (*p == ':') {
                        int size;
                        char *start = p+1;

                        for (; *p && *p != '}'; p++) {
                                if (*p == '\\' && *++p == 0)
                                        break;
                        }
                        if (*p == 0)
                                return NULL;

                        size = p - start + 1;
                        *defval = grad_emalloc(size);
                        memcpy(*defval, start, size-1);
                        (*defval)[size] = 0;
                }
                *endp = p + 1;
                return grad_attr_name_to_dict(namebuf);
        }
        *endp = p;
        return NULL;
}

void
radius_xlate0(struct obstack *obp, char *str, grad_request_t *req,
              grad_avp_t *reply)
{
        int c;
        char *p;
        grad_dict_attr_t *da;
        char *defval;

        for (p = str; *p; ) {
                switch (c = *p++) {
                default:
                        obstack_1grow(obp, c);
                        break;
			
                case 0:
                        goto end;
			
		case '\n':
			obstack_1grow(obp, '\r');
			obstack_1grow(obp, c);
			break;
			
                case '%':
                        if (!req) {
                                obstack_1grow(obp, c);
                                break;
                        }
                        switch (c = *p++) {
                        case '%':
                                obstack_1grow(obp, c);
                                break;
				
                        case 'D':
                                curtime_to_str(obp, req->request, 0);
                                break;
				
                        case 'G':
                                curtime_to_str(obp, req->request, 1);
                                break;
				
                        case 'f': /* Framed IP address */
                                attrno_to_str(obp, NULL, reply,
                                              DA_FRAMED_IP_ADDRESS, NULL);
                                break;
				
                        case 'n': /* NAS IP address */
                                attrno_to_str(obp, req, req->request,
                                              DA_NAS_IP_ADDRESS, NULL);
                                break;
				
                        case 't': /* MTU */
                                attrno_to_str(obp, NULL, reply,
                                              DA_FRAMED_MTU, NULL);
                                break;
				
                        case 'p': /* Port number */
                                attrno_to_str(obp, req, req->request,
                                              DA_NAS_PORT_ID, NULL);
                                break;
				
                        case 'u': /* User name */
                                attrno_to_str(obp, req, req->request,
                                              DA_USER_NAME, NULL);
                                break;
				
                        case 'c': /* Callback-Number */
                                attrno_to_str(obp, NULL, reply,
                                              DA_CALLBACK_NUMBER, NULL);
                                break;
				
                        case 'i': /* Calling station ID */
                                attrno_to_str(obp, req, req->request,
                                              DA_CALLING_STATION_ID, NULL);
                                break;
				
                        case 'a': /* Protocol: SLIP/PPP */
                                attrno_to_str(obp, NULL, reply,
                                              DA_FRAMED_PROTOCOL, NULL);
                                break;
				
                        case 's': /* Speed */
                                attrno_to_str(obp, req, req->request,
                                              DA_CONNECT_INFO, NULL);
                                break;
				
                        case 'C':
                                /* Check pair */
                                da = parse_dict_attr(p, &p, &defval);
                                attr_to_str(obp, req, req->request,
                                            da, defval);
                                grad_free(defval);
                                break;
				
                        case 'R':
                                /* Reply pair */
                                da = parse_dict_attr(p, &p, &defval);
                                attr_to_str(obp, NULL,
                                            reply, da, defval);
                                break;
				
                        default:                                        
                                obstack_1grow(obp, '%');
                                obstack_1grow(obp, c);
                                break;
                        }
                        break;
                        
                case '\\':
                        switch (c = *p++) {
                        case 'a':
                                obstack_1grow(obp, '\a');
                                break;
				
                        case 'b':
                                obstack_1grow(obp, '\b');
                                break;
				
                        case 'f':
                                obstack_1grow(obp, '\f');
                                break;
				
                        case 'e':
                                obstack_1grow(obp, '\033');
                                break;
				
                        case 'n':
                                obstack_1grow(obp, '\n');
                                break;
				
                        case 'r':
                                obstack_1grow(obp, '\r');
                                break;
				
                        case 't':
                                obstack_1grow(obp, '\t');
                                break;
				
                        case 0:
                                goto end;
				
                        default:
                                obstack_1grow(obp, '\\');
                                obstack_1grow(obp, c);
                                break;
                        }
                }
        }
end:
        return;
}

char *
radius_xlate(struct obstack *obp, char *str, grad_request_t *req, grad_avp_t *reply)
{
        radius_xlate0(obp, str, req, reply);
        obstack_1grow(obp, 0);
        return obstack_finish(obp);
}

static void
pair_set_value(grad_avp_t *p, Datatype type, Datum *datum)
{
	grad_free(p->avp_strvalue);
	switch (type) {
	case Integer:
		p->avp_lvalue = datum->ival;
		break;
		
	case String:
		p->avp_strvalue = datum->sval;
		p->avp_strlength = strlen(p->avp_strvalue);
		break;
		
	default:
		grad_insist_fail("bad Datatype");
	}
	p->eval_type = eval_const;
}

int
radius_eval_avl(grad_request_t *req, grad_avp_t *p)
{
	int errcnt = 0;
	for (; p; p = p->next) {
		Datatype type;
		Datum datum;
		
		switch (p->eval_type) {
		case eval_const:
			break;
			
		case eval_interpret:
			if (rewrite_interpret(p->avp_strvalue,
					      req, &type, &datum)) {
                                errcnt++;
                                continue;
                        }
			pair_set_value(p, type, &datum);
			break;

		case eval_compiled:
			if (rewrite_eval(p->avp_strvalue,
					 req, &type, &datum)) {
                                errcnt++;
                                continue;
                        }
			pair_set_value(p, type, &datum);
			break;

		default:
			grad_insist_fail("bad eval_type");
                }
        }

	return errcnt++;
}

