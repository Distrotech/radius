/*
   Copyright (C) 2001, Sergey Poznyakoff.

   This file is part of GNU Radius SNMP Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <asn1.h>
#include <snmp.h>

char *snmp_errlist[] = {
        "no error",
        "ASN.1 encode",
        "ASN.1 decode",
        "not enough memory",
        "malformed OID",
        "bad IP address or hostname",
        "can't open socket",
        "can't bind socket",
        "send",
        "operation or type not supported",
        "SNMP decode",
        "bad version number",
        "unknown request type",
        "unknown variable type",
};
int snmp_nerr = sizeof(snmp_errlist)/sizeof(snmp_errlist[0]);

char *
snmp_strerror(en)
        int en;
{
        if (en < snmp_nerr)
                return snmp_errlist[en];
        return "unknown error";
}
