/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *  
 * $Id$
 *  
 */

#if defined(HAVE_CONFIG_H)
# include <config.h>
#endif

#include <unistd.h>
#include <stdio.h>
#include <sysdep.h>
#include <radius/md5.h>
#if defined(HAVE_CRYPT_H)
# include <crypt.h>
#endif

static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void to64(char *, unsigned long, int);

static void
to64(s, v, n)
        char *s;
        unsigned long v;
        int n;
{
        while (--n >= 0) {
                *s++ = itoa64[v&0x3f];
                v >>= 6;
        }
}

LOCK_DECLARE(lock)

/*
 * UNIX password
 *  
 * Use MD5 for what it is best at...
 */

char *
grad_md5crypt(const char *pw, const char *salt, char *passwd, size_t size)
{
        static char magic[] = "$1$"; /* This string is magic for
                                      * this algorithm.  Having
                                      * it this way, we can get
                                      * better later on
                                      */
        char *p;
        static const char *sp, *ep;
        unsigned char   final[16];
        int sl,pl,i;
        MD5_CTX ctx,ctx1;
        unsigned long l;

        /* Refine the Salt first */
        sp = salt;

        /* If it starts with the magic string, then skip that */
        if (!strncmp(sp, magic, strlen(magic)))
                sp += strlen(magic);
        else {
                LOCK_SET(lock);
                strncpy(passwd, crypt(pw, salt), size);
                LOCK_RELEASE(lock);
                return passwd;
        }
                

        /* It stops at the first '$', max 8 chars */
        for(ep=sp;*ep && *ep != '$' && ep < (sp+8);ep++)
                continue;

        /* get the length of the true salt */
        sl = ep - sp;

        /* Check if the buffer can accomodate the whole password */
        if (sizeof(magic)+sl+1+22 > size)
                return NULL;
        
        grad_MD5Init(&ctx);

        /* The password first, since that is what is most unknown */
        grad_MD5Update(&ctx, (unsigned char*) pw, strlen(pw));

        /* Then our magic string */
        grad_MD5Update(&ctx,(unsigned char*) magic,strlen(magic));

        /* Then the raw salt */
        grad_MD5Update(&ctx, (unsigned char*)sp, sl);

        /* Then just as many characters of the MD5(pw,salt,pw) */
        grad_MD5Init(&ctx1);
        grad_MD5Update(&ctx1, (unsigned char*)pw, strlen(pw));
        grad_MD5Update(&ctx1, (unsigned char*)sp, sl);
        grad_MD5Update(&ctx1, (unsigned char*)pw, strlen(pw));
        grad_MD5Final(final,&ctx1);
        for(pl = strlen(pw); pl > 0; pl -= 16)
                grad_MD5Update(&ctx,final,pl>16 ? 16 : pl);

        /* Don't leave anything around in vm they could use. */
        memset(final,0,sizeof final);

        /* Then something really weird... */
        for (i = strlen(pw); i ; i >>= 1)
                if(i&1)
                    grad_MD5Update(&ctx, final, 1);
                else
                    grad_MD5Update(&ctx, (unsigned char*)pw, 1);

        /* Now make the output string */
        strcpy(passwd,magic);
        strncat(passwd,sp,sl);
        strcat(passwd,"$");

        grad_MD5Final(final,&ctx);

        /*
         * and now, just to make sure things don't run too fast
         * On a 60 Mhz Pentium this takes 34 msec, so you would
         * need 30 seconds to build a 1000 entry dictionary...
         */
        for(i=0;i<1000;i++) {
                grad_MD5Init(&ctx1);
                if(i & 1)
                        grad_MD5Update(&ctx1, (unsigned char*)pw, strlen(pw));
                else
                        grad_MD5Update(&ctx1,final,16);

                if(i % 3)
                        grad_MD5Update(&ctx1, (unsigned char*)sp, sl);

                if(i % 7)
                        grad_MD5Update(&ctx1, (unsigned char*)pw, strlen(pw));

                if(i & 1)
                        grad_MD5Update(&ctx1,final,16);
                else
                        grad_MD5Update(&ctx1, (unsigned char*)pw, strlen(pw));
                grad_MD5Final(final,&ctx1);
        }

        p = passwd + strlen(passwd);

        l = (final[ 0]<<16) | (final[ 6]<<8) | final[12]; to64(p,l,4); p += 4;
        l = (final[ 1]<<16) | (final[ 7]<<8) | final[13]; to64(p,l,4); p += 4;
        l = (final[ 2]<<16) | (final[ 8]<<8) | final[14]; to64(p,l,4); p += 4;
        l = (final[ 3]<<16) | (final[ 9]<<8) | final[15]; to64(p,l,4); p += 4;
        l = (final[ 4]<<16) | (final[10]<<8) | final[ 5]; to64(p,l,4); p += 4;
        l =                    final[11]                ; to64(p,l,2); p += 2;
        *p = '\0';

        /* Don't leave anything around in vm they could use. */
        memset(final,0,sizeof final);

        return passwd;
}

