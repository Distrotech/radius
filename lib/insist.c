/* This file is part of GNU RADIUS.
   Copyright (C) 2000,2001, Sergey Poznyakoff
  
   This file is free software; as a special exception the author gives
   unlimited permission to copy and/or distribute it, with or without
   modifications, as long as this notice is preserved.
  
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
   implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. */
 
#ifndef lint
static char rcsid[] = "@(#) $Id$";
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <radius.h>

int
__insist_failure(str, file, line)
        char *str;
        char *file;
        int   line;
{
        radlog(L_CRIT,
               "INSIST FAILURE: %s at %s:%d", str, file, line);
        abort();
        /*NOTREACHED*/
}