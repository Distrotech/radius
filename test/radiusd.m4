# This file is part of GNU RADIUS.
# Copyright (C) 2000,2001 Sergey Poznyakoff
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# $Id$

TOOLDIR(radiusd)
BEGIN

SEQUENCE(Auth,
Checking Basic Authentication Types,
TEST(send auth 1 User-Name = QUOTE(accept), expect 2)
TEST(send auth 1 User-Name = QUOTE(reject), expect 3)
TEST(send auth 1 User-Name = QUOTE(local) Password = QUOTE(guessme), expect 2)
TEST(send auth 1 User-Name = QUOTE(local) Password = QUOTE(bad), expect 3)
TEST(send auth 1 User-Name = QUOTE(crypt) Password = QUOTE(hamlet), expect 2))

SEQUENCE(Reply,
Checking Reply Attributes,
TEST(send auth 1 User-Name = QUOTE(reply) Password = QUOTE(guessme),
     expect 2 Service-Type = 2 Framed-Protocol = 1)
TEST(send auth 1 User-Name = QUOTE(framed-ip) NAS-Port-Id = 10,
     expect 2 Service-Type = 2 Framed-Protocol = 1 Framed-IP-Address = 127.0.0.11))

SEQUENCE(Begin,
[Checking BEGIN Keyword],
TEST(send auth 1 User-Name = QUOTE(accept) NAS-IP-Address = 127.0.0.2 NAS-Port-Id = 2,
     expect 2 Framed-IP-Address = 127.10.0.3)) 

SEQUENCE(Default,
Checking DEFAULT Keyword,
TEST(send auth 1 User-Name = QUOTE(no-such-user) \
                    NAS-IP-Address = 127.0.0.1 \
                    NAS-Port-Id = 2,
    expect 3)
TEST(send auth 1 User-Name = QUOTE(no-such-user) \
                    NAS-IP-Address = 127.0.0.3 \
                    NAS-Port-Id = 2, 
    expect 2 Reply-Message = QUOTE(OK. Come in.))
TEST(send auth 1 User-Name = QUOTE(no-such-user) \
                    NAS-IP-Address = 127.0.0.4 \
                    NAS-Port-Id = 2,
    expect 3 Reply-Message = QUOTE(Wrong NAS)))

SEQUENCE(Match,
Checking Match-Profile,
TEST(send auth 1 User-Name = QUOTE(match1) \
                    NAS-IP-Address = 127.0.0.1 \
                    NAS-Port-Id = 1 \
                    NAS-Identifier = QUOTE(en),
    expect 2 Service-Type = Framed-User \
               Framed-Protocol = PPP \
               Framed-IP-Address = 127.10.10.1 )
TEST(send auth 1 User-Name = QUOTE(match1) \
                    NAS-IP-Address = 127.0.0.1 \
                    NAS-Port-Id = 1 \
                    NAS-Identifier = QUOTE(to),
    expect 2 Service-Type = Framed-User \
               Framed-Protocol = PPP \
               Framed-IP-Address = 127.10.10.2)
TEST(send auth 1 User-Name = QUOTE(match1) \
                    NAS-IP-Address = 127.0.0.1 \
                    NAS-Port-Id = 2 \
                    NAS-Identifier = QUOTE(een),
     expect 3)
TEST(send auth 1 User-Name = QUOTE(match1) \
                    NAS-IP-Address = 127.0.0.1 \
                    NAS-Identifier = QUOTE(een),
     expect 3))

SEQUENCE(Hints,
Checking Hints,
TEST(send auth 1 User-Name = QUOTE(hint.1),
     expect 2 Reply-Message = QUOTE(Hint 1))
TEST(send auth 1 User-Name = QUOTE(hint.2),
     expect 2 Reply-Message = QUOTE(Hint 2))
TEST(send auth 1 User-Name = QUOTE(hint),
     expect 3 Reply-Message = QUOTE(No suffix)))

SEQUENCE(Huntgroups,
Checking Huntgroups,
TEST(send auth 1 User-Name = QUOTE(accept) \
             NAS-IP-Address = 127.0.0.4 \
	     NAS-Port-Id = 2,
     expect 2)
TEST(send auth 1 User-Name = QUOTE(accept) \
             NAS-IP-Address = 127.0.0.4 \
	     NAS-Port-Id = 5,
        expect 3))

IFSEQUENCE(Scheme-Auth, USE_SERVER_GUILE,
Checking Scheme Authentication,
TEST(send auth 1 User-Name = QUOTE(scheme) \
	             NAS-IP-Address = 127.0.0.1,
	 expect 2 Framed-MTU = 8096) 
TEST(send auth 1 User-Name = QUOTE(scheme) \
	                NAS-IP-Address = 127.0.0.2,
         expect 2 Framed-MTU = 256))

SEQUENCE(Access,
Checking Access Lists,
[TEST(send auth 1 User-Name = QUOTE(bad-one),
 expect 3 Reply-Message = QUOTE([[Sorry, your account is currently closed]]ESC(\r,\n)))])

SEQUENCE(Expiration,
Checking Expiration Attribute,
[TEST(send auth 1 User-Name = QUOTE(expire) Password = QUOTE(expire),
 expect 3 Reply-Message = QUOTE([[Password Has Expired]]ESC(\r,\n)))
 TEST(send auth 1 User-Name = QUOTE(no-expire) Password = QUOTE(expire),
 expect 2)])

IFSEQUENCE(Menu, USE_LIVINGSTON_MENUS,
Checking Menus,
[TEST(send auth 1 User-Name = QUOTE(menu),
     print DEREF(REPLY,Reply-Message*),
     [MENU1(1-PPP, 2-CSLIP, 3-SLIP, 4-Login, 5-Second, 6-Exit)])
TEST(send auth 1 User-Name = QUOTE(menu) \
                     Password = QUOTE(5) State = QUOTE(MENU=menu1),
     print DEREF(REPLY,Reply-Message*),
     [MENU2(ra,sol,weevil,top,quit)]) 
TEST(send auth 1 User-Name = QUOTE(menu) \
                     Password = QUOTE(sol) \
                     State = QUOTE(MENU=menu2),
        expect 2 Service-Type = 1 \
                  Login-IP-Host = 127.0.0.1 \
                  State = QUOTE(MENU=menu2) \
                  Termination-Action = 1)]) 

SEQUENCE(Acct-start,
Checking Accountng Start,
TEST(send acct 4 User-Name = QUOTE(simuse) \
                  NAS-IP-Address = 127.0.0.1 \
                  NAS-Port-Id = 1 \
                  Acct-Session-Id = QUOTE(0001) \
                  Acct-Status-Type = Start,
      expect 5)
DISPLAY(Checking Simultaneous-Use Attribute)
TEST(send auth 1 User-Name = QUOTE(simuse),
     expect 3 Reply-Message = QUOTE(ESC(\r,\n)You are already logged in - access denied[]ESC(\r,\n))))


SEQUENCE(Acct-Stop,
Checking Accountng Stop,
TEST(send acct 4 User-Name = QUOTE(simuse) \
                  NAS-IP-Address = 127.0.0.1 \
                  NAS-Port-Id = 1 \
                  Acct-Session-Id = QUOTE(0001) \
                  Acct-Status-Type = Stop,
      expect 5)
DISPLAY(Checking Simultaneous-Use Attribute)
TEST(send auth 1 User-Name = QUOTE(simuse),
	    expect 2 Service-Type = 2))

SEQUENCE(Rewrite,
Checking Rewrite Rules,
[
TEST(send auth 1 User-Name = rewrite_max \
	          NAS-IP-Address = 127.0.0.5 \
	          NAS-Port-Id = 20212,
     expect 2 Reply-Message = OK)
TEST(send auth 1 User-Name = QUOTE(WGROUP[]ESC(\,\)rewrite_nt) \
	          NAS-IP-Address = 127.0.0.5 \
	          NAS-Port-Id = 2,
     expect 2 Reply-Message = OK)
TEST(send auth 1 User-Name = QUOTE(rewrite_je/tstream) \
	            NAS-IP-Address = 127.0.0.5 \
		    NAS-Port-Id = 3,
            expect 2 Reply-Message = OK)		   
TEST(send auth 1 User-Name = QUOTE(rewrite_cisco) \
	          NAS-IP-Address = 127.0.0.5 \
	          Cisco-PRI-Circuit = QUOTE(ISDN 2:D:123) \
Acct-Session-Id = QUOTE(120104/18:02:06.020 EEST Thu Dec 7 2000/hostname/1B22539E 86E6603F 0 19C974C0/answer/Telephony////),
     expect 2 Reply-Message = OK)])

SEQUENCE(Exec-Program-Wait,
Checking Exec-Program-Wait Attribute,
[
TEST(send auth 1 User-Name = QUOTE(execwait) NAS-Port-Id = 0,
     expect 2 Service-Type = 1 Reply-Message = QUOTE([[Welcome, execwait]]))
TEST(send auth 1 User-Name = QUOTE(execwait) NAS-Port-Id = 1, 
     expect 3)])

IFSEQUENCE(Scheme-Acct, USE_SERVER_GUILE,
Checking Scheme Accounting,
TEST(send acct 4 User-Name = QUOTE(scheme) \
	                NAS-IP-Address = 127.0.0.1 \
                        NAS-Port-Id = 1 \
                        Acct-Status-Type = Start \
			Acct-Session-Id = QUOTE(0001),
	        expect 5)
# Fixme: check file
)

END
