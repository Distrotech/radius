divert(-1)dnl
dnl This file is part of GNU RADIUS.
dnl Copyright (C) 2001 Sergey Poznyakoff
dnl  
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl  
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl  
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software Foundation,
dnl Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
define({ARGS},{ifdef({server}, -h{}server){}ifdef({port}, -p{}port){}dnl
ifdef({CREATOR},-U CREATOR){}ifdef({CREATOR_PASSWORD},{ -W})})

define({CREATEDATABASE},ifelse(MODE,{CREATE}, {
createdb ARGS {$1}
psql ARGS -d $1 < pgsql.struct
}))

define({CREATETABLE},{ifelse(MODE,{STRUCT},{
divert
define({TABLENAME},{$1})
CREATE TABLE $1 ($2);
undivert(1)})})

define({BYTE_T},{int2})dnl
define({SHORTINT_T},{int2})dnl
define({INT_T},{int8})dnl
define({LONGINT_T},{int8})dnl
define({CHAR_T},{character($1)})dnl
define({VARCHAR_T},{character($1)})dnl
define({CI})dnl
define({TIME_T},{timestamp})dnl
define({ENUM_T},{ifelse({$1},1,char,character({$1}))})

define({INDEX},{divert(1)
CREATE {INDEX} $1 on TABLENAME (shift($@));
divert})
define({UNIQUE},{divert(1)
CREATE {UNIQUE INDEX} $1 on TABLENAME (shift($@));
divert})
define({COMMA})

define({DB_PRIV},{
ifelse(MODE,{STRUCT},
{
CREATE USER "DB_USER" WITH PASSWORD 'DB_PWD';
REVOKE ALL on "calls" from PUBLIC;
GRANT INSERT,UPDATE,DELETE,SELECT on "calls" to "DB_USER";
REVOKE ALL on "passwd" from PUBLIC;
GRANT SELECT on "passwd" to "DB_USER";
REVOKE ALL on "groups" from PUBLIC;
GRANT SELECT on "groups" to "DB_USER";
REVOKE ALL on "attrib" from PUBLIC;
GRANT SELECT on "attrib" to "DB_USER";
})})
divert{}dnl
