/* 
 *  $Id$
 *  %ndp: passwd utils 
 *
 *  Copyright (c) 1999 Bonelli Nicola <bonelli@antifork.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */


#include <passwd.h>
#include <md5.h>

extern ctrl_t ndp;

#ifndef HAVE_STRCASECMP

int
strcasecmp (a, b)
     char         *a;
     char         *b;
{
    while ((tolower (*a) == tolower (*b)) && (*a != '\0') && (*b != '\0'))
	{
	    a++;
	    b++;
	}

    return ((*a) - (*b));
}

#endif

char         *
ndp_strstr (a, b)
     char         *a;
     char         *b;
{

    register char *orig = b;

    while (*a != '\0' && *b != '\0')
	{
	    if (tolower (*a) == tolower (*b))
		{
		    b++;
		    a++;
		}
	    else
		{
		    if (b == orig)
			a++;
		    else
			b = orig;
		}

	}

    if (*b == '\0')
	return (char *) (a - strlen (orig));
    else
	return (char *) NULL;

}

int
auth_user (pass)
     char         *pass;
{

    if (strcmp (pass, ndp.pass.master) == 0)
	return CL_MASTER;
    else if (strcmp (pass, ndp.pass.user) == 0)
	return CL_USER;
    else if (strcmp (pass, ndp.pass.ircer0) == 0)
	return CL_IRC1;
    else if (strcmp (pass, ndp.pass.ircer1) == 0)
	return CL_IRC2;
    else if (strcmp (pass, ndp.pass.ircer2) == 0)
	return CL_IRC3;
    else
	return CL_UNKNOWN;

}

void
md5_printable (key, p)
     unsigned char *key;
     unsigned char *p;
{
    unsigned char st[3];
    register int  i,
                  j;

    *p = 0;

    for (i = 0; i < 4; i++)
	{
	    for (j = 0; j < 4; j++)
		{
		    sprintf (st, "%02X", *(key + (i * 4) + j));
		    strcat (p, st);
		}
	}

    return;

}

int
checkpass (a)
     unsigned char *a;
{
    struct md5_ctx hash;
    char          md5[17];	/* 16+1 */
    char          buff[33];	/* 32+1 */
    char         *ptr;

    memset (md5, 0, 17);
    memset (buff, 0, 33);

    ptr = (char *) a;

    while (*ptr == ' ')
	ptr++;

    if ((ptr == ndp_strstr (ptr, "pass ")))
	{
	    /* we're serving a irc-client */

	    ptr += 5;

	    while ((*ptr == ' '))
		ptr++;

	    if (!*ptr)
		return CL_UNKNOWN;

	}
    else
	ptr = a;

    md5_init_ctx (&hash);
    md5_process_bytes (ptr, strlen (ptr), &hash);
    md5_finish_ctx (&hash, md5);
    md5_printable (md5, buff);

    return auth_user (buff);

}
