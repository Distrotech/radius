/* This file is generated automatically.
 * Generator: ./scripts/debug.sh $Id$
 * Skeleton:  ./scripts/debugmod.m4
 */


/* $Id$ */
#if defined(HAVE_CONFIG_H)
# include <config.h>
#endif
#if RADIUS_DEBUG
#include <log.h>

int debug_level[4];

struct debug_module debug_module[4+1] = {

       "leakdetect.c", 0,
       "mem.c", 1,
       "mysql.c", 2,
       "postgres.c", 3,



	(char*)0, 0
};

#endif
