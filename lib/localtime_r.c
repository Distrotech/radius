/* This file is part of GNU Radius.
   Copyright (C) 2000, 2001, 2002, 2003, 2007, 2010, 2013 Free Software
   Foundation, Inc.

   Written by Sergey Poznyakoff
  
   GNU Radius is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
  
   GNU Radius is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with GNU Radius.  If not, see <http://www.gnu.org/licenses/>. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <time.h>

LOCK_DECLARE(lock)

struct tm *
localtime_r(const time_t *timep, struct tm *res)
{
        struct tm *tm;

        LOCK_SET(lock);
        tm = localtime(timep);
        memcpy(res, tm, sizeof(*res));
        LOCK_RELEASE(lock);
        return res;
}
