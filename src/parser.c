/* 
 *  $Id$
 *  %ndp: parsers 
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

#include <parser.h>
extern char   sep;

int
parse_input (input, host, port)
     char         *input;
     long         *host;
     unsigned short *port;
{
    register char *ptr;

    *host = 0;
    *port = 0;

    while ((*(input) == ' '))
	input++;

    if (!*input)
	return -1;

    if (!(ptr = strchr (input, ':')))
	return -1;

    *(ptr++) = '\0';

    if (strcasecmp (input, "all"))
	{
	    *host = gethostbyname_lru (input);

	    if (*host == 0)
		return -1;
	}

    if (ISDIGIT (ptr))
	*port = (unsigned short) atoi (ptr);
    else
	return -1;


    return 0;
}

#define	CR_LF_COOKIES	1

int
parse_irc (buff, argz)
     char         *buff;
     char        **argz;
{
    register short int i = 0;
    register short int j = 0;

    argz[i++] = buff;

    while (*buff && (i < MAXARGLINE - 1))
	{

	    switch (*buff)
		{
		 case ' ':
		     *(buff++) = 0;
		     while ((*buff == ' '))
			 buff++;
		     argz[i++] = buff;

		     break;

		 case '\n':
		 case '\r':
		     *(buff++) = 0;
		     argz[i++] = &sep;
		     j |= CR_LF_COOKIES;

		     break;

		 default:
		     if (j & CR_LF_COOKIES)
			 {
			     argz[i++] = buff;
			     j &= ~CR_LF_COOKIES;
			 }
		     buff++;
		     break;

		}

	}

    argz[i] = &sep;

    memset (&argz[i-1], 0, (MAXARGLINE - i -1) * sizeof (argz[0]));

    return i;

}

int
parse_conf (input, argz)
     char         *input;
     char        **argz;
{
    register short i = 0,
                   c = 0;

    argz[i++] = input;

    while ((*input) && (i < MAXARGLINE))
	{

	    if ((*input == ':') || (*input == ' ') || (*input == '\n') || (*input == '\r'))
		{
		    *(input++) = 0;
		    argz[i]    = input;
		    c          = 1;
		}
	    else
		{
		    if (c != 0 )
			{
			    i++;
			    c= 0;
			}
		    input++;
		}
	}

    memset (&argz[i], 0, (MAXARGLINE - i) * sizeof (argz[0]));

    return i;
}
