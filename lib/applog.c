/* This file is part of GNU RADIUS.
   Copyright (C) 2000, 2001, Sergey Poznyakoff
  
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

#ifndef lint
static char rcsid[] = 
"$Id$";
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <radius.h>

int debug_flag;
char *progname;

void
initlog(name)
        char *name;
{
        progname = strrchr(name, '/');
        if (progname)
                progname++;
        else
                progname = name;
}

void vlog(int lvl, char *file, int line, char *func_name, int en,
          char *fmt, va_list ap);
#define SP(p) ((p)?(p):"")

static char *priname[] = { /* priority names */
        "emerg",
        "alert",
        "crit",
        "error",
        "warning",
        "notice",
        "info",
        "debug"
};

void
vlog(level, file, line, func_name, en, fmt, ap)
        int level;
        char *file;
        int line;
        char *func_name;
        int en;
        char *fmt;
        va_list ap;
{
        fprintf(stderr, "%s: %s: ", progname, priname[level & L_PRIMASK]);
        if (file) 
                fprintf(stderr, "%s:%d:%s: ", file, line, SP(func_name));
        vfprintf(stderr, fmt, ap);
        if (en)
                fprintf(stderr, ": %s", strerror(en));
        fprintf(stderr, "\n");
}
