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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <pwd.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <pwd.h>
#include <grp.h>

#include <common.h>

grad_request_t *
grad_request_alloc()
{
        return grad_emalloc(sizeof(grad_request_t));
}

/* Free a grad_request_t struct.
 */
void 
grad_request_free(grad_request_t *radreq)
{
	grad_list_destroy(&radreq->locus_list, NULL, NULL);
	grad_free(radreq->remote_user);
        grad_avl_free(radreq->reply_pairs);
        grad_free(radreq->reply_msg);
        grad_avl_free(radreq->server_reply);
        grad_avl_free(radreq->request);
        grad_free(radreq);
}


/* Turn printable string (dictionary type DATE) into correct tm struct entries
 */
static char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

int
grad_parse_time_string(char *valstr, struct tm *tm)
{
        int     i;

        /* Get the month */
        for (i = 0; i < 12; i++) {
                if (strncasecmp(months[i], valstr, 3) == 0) {
                        tm->tm_mon = i;
                        break;
                }
        }

        if (i == 12)
                return -1;
        
        valstr += 3;
        while (*valstr && isspace(*valstr))
                valstr++;

        if (!*valstr)
                return -1;
        
        /* Get the Day */
        tm->tm_mday = strtol(valstr, &valstr, 10);

        while (*valstr && isspace(*valstr))
                valstr++;
        if (!*valstr)
                return -1;
        
        /* Now the year */
        tm->tm_year = strtol(valstr, &valstr, 10) - 1900;

        return 0;
}

/* Lock `size' bytes on the file descriptor `fd' starting from `offset'.
 * `whence' determines from where the offset is counted (seek-like)
 */
void
grad_lock_file(int fd, size_t size, off_t offset, int whence)
{
        struct flock fl;

        fl.l_type = F_RDLCK;
        fl.l_whence = whence;
        fl.l_start = offset;
        fl.l_len = size;
        fcntl(fd, F_SETLKW, &fl);
}

/* Unlock `size' bytes on the file descriptor `fd' starting from `offset'.
 * `whence' determines from where the offset is counted (seek-like)
 */
void
grad_unlock_file(int fd, size_t size, off_t offset, int whence)
{
        struct flock fl;

        fl.l_type = F_UNLCK;
        fl.l_whence = whence;
        fl.l_start = offset;
        fl.l_len = size;
        fcntl(fd, F_SETLKW, &fl);
}

/* Find a struct keyword matching the given string. Return keyword token
 * number if found. Otherwise return default value `def'.
 */
int
grad_xlat_keyword(struct keyword *kw, char *str, int def)
{
        for ( ; kw->name; kw++) 
                if (strcmp(str, kw->name) == 0)
                        return kw->tok;
        return def;
}

/* compose a full pathname from given path and filename
 */
char *
grad_mkfilename(char *dir, char *name)
{
        int len = strlen(dir) + strlen(name);
        char *p = grad_emalloc(len+2);
        sprintf(p, "%s/%s", dir, name);
        return p;
}

/* compose a full pathname from given path, subdirectory and filename
 */
char *
grad_mkfilename3(char *dir, char *subdir, char *name)
{
        int len = strlen(dir) + strlen(subdir) + strlen(name);
        char *p = grad_emalloc(len+3); /* two intermediate slashes and
                                   * terminating zero
                                   */
        sprintf(p, "%s/%s/%s", dir, subdir, name);
        return p;
}

/* Convert second character of a backslash sequence to its ASCII
   value: */
int
grad_decode_backslash(int c)
{
        static char transtab[] = "a\ab\bf\fn\nr\rt\t";
        char *p;

        for (p = transtab; *p; p += 2) {
                if (*p == c)
                        return p[1];
        }
        return c;
}

#define to_num(c) \
  (isdigit(c) ? c - '0' : (isxdigit(c) ? toupper(c) - 'A' + 10 : -1 ))

void
grad_obstack_grow_backslash_num(struct obstack *stk, char *text,
				int len, int base)
{
	int i;
	int c = 0;

	for (i = len-1; i >= 0 && text[i] != '\\'; i--)
		;
	if (i)
		obstack_grow(stk, text, i);
	if (base == 16)
		i++;
	for (i++; i < len; i++) 
		c = c*base + to_num(text[i]);
	obstack_1grow(stk, c);
}


void
grad_obstack_grow_backslash(struct obstack *stk, char *text, char **endp)
{
	int c, i;
	
	switch (text[1]) {
	case '\\':
		obstack_1grow(stk, text[1]);
		text += 2;
		break;
		
	default:
		c = grad_decode_backslash(text[1]);
		obstack_1grow(stk, c);
		text += 2;
		break;
		
	case '0':
		for (i = 0; i < 3 && isdigit(text[i+1]) && text[i+1] < '8'; i++)
			;
		if (i != 3)
			break;
		grad_obstack_grow_backslash_num(stk, text, 4, 8);
		text += 4;
		break;
		
	case 'x':
	case 'X':
		for (i = 0; i < 2 && isxdigit(text[i+2]); i++)
			;
		if (i != 2)
			break;
		grad_obstack_grow_backslash_num(stk, text, 3, 16);
		text += 3;
		break;
	}
	*endp = text;
}

void
grad_string_copy(char *d, char *s, int len)
{
        int slen = strlen(s);
        
        if (slen > len) 
                grad_log(L_ERR, _("string too long: %s"), s);
        strncpy(d, s, len);
        d[len] = 0;
}

char *
grad_op_to_str(enum grad_operator op)
{
        switch (op) {
        case grad_operator_equal:         return "=";
        case grad_operator_not_equal:     return "!=";
        case grad_operator_less_than:     return "<";
        case grad_operator_greater_than:  return ">";
        case grad_operator_less_equal:    return "<=";
        case grad_operator_greater_equal: return ">=";
	default:
		break;
        }
        return "?";
}

enum grad_operator
grad_str_to_op(char *str)
{
        int op = NUM_OPERATORS;
        switch (*str++) {
        case '=':
                op = grad_operator_equal;
                break;
        case '!':
                if (*str++ == '=')
                        op = grad_operator_not_equal;
                break;
        case '<':
                if (*str == 0)
                        op = grad_operator_less_than;
                else if (*str++ == '=')
                        op = grad_operator_less_equal;
                break;
        case '>':
                if (*str == 0)
                        op = grad_operator_greater_than;
                else if (*str++ == '=')
                        op = grad_operator_greater_equal;
                break;
        }
        if (*str)
                op = NUM_OPERATORS;
        return op;
}

static int
flush_seg(char **bufp, char *seg, char *ptr, int runlen)
{
        int outbytes = 0;
        char *buf = *bufp;
        
        if (ptr - seg >= runlen) {
                outbytes += ptr - seg;
                if (buf)
                        while (seg < ptr)
                                *buf++ = *seg++;
        } else {
                outbytes += 4*(ptr - seg);
                if (buf)
                        while (seg < ptr) {
                                sprintf(buf, "\\%03o", *(u_char*)seg);
                                seg++;
                                buf += 4;
                        }
        }
        *bufp = buf;
        return outbytes;
}

/* Print LEN characters from STR to buffer BUF. If a sequence of RUNLEN
   or more printable characters is encountered, it is printed as is,
   otherwise, each character is printed as a three-digit octal number,
   preceeded by a backslash (\%03o).
   Return number of characters, _output_ to BUF. If BUF is NULL, no
   printing is done, but the number of characters that would be output
   (not counting null terminator) is returned. */

int
grad_format_string_visual(char *buf, int runlen, char *str, int len)
{
        char *seg, *ptr;
        int outbytes = 0;

        seg = NULL;
        ptr = str;
        while (len) {
                if (isprint(*ptr)) {
                        if (!seg)
                                seg = ptr;
                } else {
                        if (seg) {
                                outbytes += flush_seg(&buf, seg, ptr, runlen);
                                seg = NULL;
                        }
                        if (buf) {
                                sprintf(buf, "\\%03o", *(u_char*)ptr);
                                buf += 4;
                        }
                        outbytes += 4;
                }
                len--;
                ptr++;
        }
        /* Make last segment printable no matter how many chars it contains */
        if (seg) {
                outbytes += ptr - seg;
                if (buf)
                        while (seg < ptr) 
                                *buf++ = *seg++;
        }
        if (buf)
                *buf++ = 0;
        return outbytes;
}

int
grad_format_vendor_pair(char *buf, grad_avp_t *pair)
{
        int n;
        grad_uint32_t vendor;
        u_char *ptr = (u_char*)pair->avp_strvalue;
        char buf1[64];
        char *bufp = buf;
        
        memcpy(&vendor, ptr, 4);
        ptr += 4;
        n = snprintf(buf1, sizeof(buf1), "V%d", (int)ntohl(vendor));
        if (n < 0)
                return -1;

        if (bufp) {
                memcpy(bufp, buf1, n);
                bufp += n;
        }
        
        return n + grad_format_string_visual(bufp, 4, ptr,
					     pair->avp_strlength - 4);
}
                
char *
grad_format_pair(grad_avp_t *pair, int typeflag, char **savep)
{
        char *buf1 = NULL;
        char *buf2ptr = NULL;
        char buf2[4*AUTH_STRING_LEN+1]; /* Enough to hold longest possible
                                           string value all converted to
                                           octal */
        grad_dict_value_t *dval;
        struct tm tm;
        char *type = "";
	
        *savep = NULL;

        switch (pair->eval_type == eval_const ? pair->type : TYPE_STRING) {
        case TYPE_STRING:
                if (pair->attribute != DA_VENDOR_SPECIFIC) {
                        int len = strlen (pair->avp_strvalue);
                        if (len != pair->avp_strlength-1)
                                len = pair->avp_strlength;
                        grad_format_string_visual(buf2, 4,
						  pair->avp_strvalue, len);
                } else if (pair->avp_strlength < 6) 
                        snprintf(buf2, sizeof(buf2),
                                 "[invalid length: %d]", pair->avp_strlength);
                else {
                        int len = grad_format_vendor_pair(NULL, pair);
                        buf2ptr = malloc(len+1);
                        if (!buf2ptr) {
                                grad_log(L_ERR,
                                         "%s:%d: can't alloc %d bytes",
                                         __FILE__, __LINE__, len+1);
                                buf2[0] = 0;
                        } else
                                grad_format_vendor_pair(buf2ptr, pair);
                }
                break;
                                        
        case TYPE_INTEGER:
                if (pair->name && (pair->prop & AP_TRANSLATE))
                        dval = grad_value_lookup(pair->avp_lvalue, pair->name);
                else
                        dval = NULL;
                
                if (!dval)
                        snprintf(buf2, sizeof(buf2), "%lu", pair->avp_lvalue);
                else
                        snprintf(buf2, sizeof(buf2), "%s", dval->name);
                break;
                
        case TYPE_IPADDR:
                grad_ip_iptostr(pair->avp_lvalue, buf2);
                break;
                
        case TYPE_DATE:
                strftime(buf2, sizeof(buf2), "\"%b %e %Y\"",
                         localtime_r((time_t *)&pair->avp_lvalue, &tm));
                break;
        default:
                strncpy(buf2, "[UNKNOWN DATATYPE]", sizeof(buf2));
        }

	if (typeflag) {
		switch (pair->type) {
		case TYPE_STRING:
			type = "(STRING) ";
			break;
			
		case TYPE_INTEGER:
			type = "(INTEGER) ";
			break;
			
		case TYPE_IPADDR:
			type = "(IPADDR) ";
			break;
			
		case TYPE_DATE:
			type = "(DATE) ";
			break;
		}
	}
	
        if (pair->name)
                asprintf(&buf1, "%s %s %s%s",
                         pair->name,
                         grad_op_to_str(pair->operator),
			 type,
                         buf2ptr ? buf2ptr : buf2);
        else
                asprintf(&buf1, "%d %s %s%s",
                         pair->attribute,
                         grad_op_to_str(pair->operator),
			 type,
                         buf2ptr ? buf2ptr : buf2);

        if (buf2ptr)
                free(buf2ptr);
        
        *savep = buf1;
        return buf1;
}

