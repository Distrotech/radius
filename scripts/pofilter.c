/* This file is part of GNU RADIUS.
   Copyright (C) 2000,2001 Sergey Poznyakoff
  
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

#include <stdio.h>

enum state {
	st_init,
	st_m,
	st_s,
	st_g,
	st_s1,
	st_t,
	st_hide,
	st_echo,
	st_newline
};

main()
{
	int c;
	int msgstr_count = 0;
	enum state state = st_init;

	while ((c = getchar()) != EOF) {
		
		switch (state) {
		case st_init:
			switch (c) {
			default:
				if (!isspace(c))
					state = st_hide;
				break;
				
			case '#':
				state = st_hide;
				break;
				
			case 'm':
				state = st_m;
				break;
			}
				
			c = ' ';
			break;

		case st_m:
			state = (c == 's') ? st_s : st_hide;
			c = ' ';
			break;
				
		case st_s:
			state = (c == 'g') ? st_g : st_hide;
			c = ' ';
			break;

		case st_g:
			state = (c == 's') ? st_s1 : st_hide;
			c = ' ';
			break;

		case st_s1:
			state = (c == 't') ? st_t : st_hide;
			c = ' ';
			break;

		case st_t:
			state = (c == 'r' && msgstr_count > 0) ?
				st_echo : st_hide;
			msgstr_count++;
			c = ' ';
			break;

		case st_hide:
			if (c == '\n')
				state = st_init;
			else 
				c = ' '; 
			break;

		case st_newline:
			switch (c) {
			case '"':
				state = st_echo;
				break;
				
			default:
				if (!isspace(c)) {
					state = st_hide;
					c = ' ';
				}
				break;

			}
			break;
			
		case st_echo:
			if (c == '\n')
				state = st_newline;
			break;
		}
	
		putchar(c);
	}

	return 0;
}