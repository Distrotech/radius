include(SRCDIR/radscripts.m4)dnl
#! BINDIR/radtest -f
# $Id$
# This file is part of GNU Radius.
# Copyright (C) 2001,2003 Free Software Foundation, Inc.
#
# Written by Sergey Poznyakoff
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# GNU Radius is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

send auth 1 User-Name = ${1:?} \
            User-Password = ${2:&Password: } \
            NAS-Port-Id = ${pid:-0}
expect 2
