/*
   Copyright (C) 2001, Sergey Poznyakoff.

   This file is part of GNU Radius SNMP Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <asn1.h>
#include <snmp.h>
#include <snmp_intern.h>
#include <stdlib.h>
#include <string.h>

snmp_alloc_t __snmp_alloc_fp = (snmp_alloc_t) malloc;
snmp_free_t  __snmp_free_fp  = (snmp_free_t) free;

void *
snmp_alloc(size)
        size_t size;
{
        return (*__snmp_alloc_fp)(size);
}

void
snmp_free(ptr)
        void *ptr;
{
        if (ptr)
                (*__snmp_free_fp)(ptr);
}

char *
snmp_strdup(str)
        char *str;
{
        int len = strlen(str)+1;
        char *p = snmp_alloc(len);
        if (p)
                strcpy(p, str);
        return p;
}

