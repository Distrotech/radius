/* This file is part of GNU RADIUS.
 * Copyright (C) 2000, Sergey Poznyakoff
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */
/* $Id$ */

#ifndef __ippool_h
#define __ippool_h

struct ip_cell {
	int port;
	UINT4 addr;
};

typedef struct {
	int cnt;
	struct ip_cell ip[1];
} IP_POOL;

IP_POOL * alloc_pool(UINT4 start_ip, int ip_cnt, int start_port);
void free_pool(IP_POOL *ptr);

#endif
