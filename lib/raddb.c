/* This file is part of GNU Radius.
   Copyright (C) 2000,2001,2002,2003,2004,2005 Free Software Foundation, Inc.

   Written by Sergey Poznyakoff
  
   GNU Radius is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
  
   GNU Radius is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public
   License along with GNU Radius; if not, write to the Free
   Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <common.h>

#if !HAVE_DECL_GETLINE
int getline (char **lineptr, size_t *n, FILE *stream);
#endif

int
grad_read_raddb_file(char *filename, int vital, char *delim,
		     int (*fun)(), void *closure)
{
        int    argc;
        char **argv;
        FILE *input;
        char *lineptr = NULL;
        size_t bsize = 0;
        int nread;
        grad_locus_t loc;
	
        input = fopen(filename, "r");
        if (!input) {
                if (vital) {
                        grad_log(L_ERR|L_PERROR, _("can't open file `%s'"),
                                 filename);
                        return -1;
                } else {
                        grad_log(L_NOTICE|L_PERROR, _("can't open file `%s'"),
                                 filename);
                        return 0;
                }
        }

	if (!delim)
		delim = "";
	loc.file = filename;
        loc.line = 0;
        while (getline(&lineptr, &bsize, input) > 0) {
                nread = strlen(lineptr);
                if (nread == 0)
                        break;
                if (lineptr[nread-1] == '\n') {
			loc.line++;
			lineptr[nread-1] = 0;
		}
		if (lineptr[0] == 0)
			continue;
                if (grad_argcv_get(lineptr, delim, NULL, &argc, &argv) == 0) {
                        int n;
                        for (n = 0; n < argc && argv[n][0] != '#'; n++)
                                ;
                        if (n)
                                fun(closure, n, argv, &loc);
                }
                if (argv)
                        grad_argcv_free(argc, argv);
        }

        if (lineptr)
                free(lineptr);
        fclose(input);

        return 0;
}
