/* 
 *  $Id$
 *  %ndp: parsers 
 *
 *  Copyright (c) 1999 bonelli `awgn' nicola <awgn@antifork.org>
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
extern char sep;

int
parse_input (char *input, long *host, unsigned short *port)
{
      register char *ptr;

      *host = 0;
      *port = 0;



      while ((*(input) == 0x20))
	    input++;

      if (!*input)
	    return -1;

      if (!(ptr = strchr (input, ':')))
	    return -1;

      *(ptr++) = '\0';

      if (strcasecmp (input, "all"))
	{
	      if (!(*host = getlongbyname (input)))
		    return -1;
	}

      if (Isdigit (ptr))
	    *port = (unsigned short) atoi (ptr);
      else
	    return -1;


      return 0;


}

#define	CR_LF_COOKIES	0x01

int
parse_irc (char *buff, char **argz)
{
      register short int i = 0;
      register short int j = 0;

      short int   out = 0;


      argz[i++] = buff;
      while (*buff && (i < _MAXARGLINE_ - 1))
	{

	      switch (*buff)
		{
		case 0x20:
		      *(buff++) = 0x00;
		      while ((*buff == 0x020))
			    buff++;
		      argz[i++] = buff;
		      break;
		case 0x0a:
		case 0x0d:
		      *(buff++) = 0x00;
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

      out = i--;
      memset (&argz[i], 0, (_MAXARGLINE_ - i) * sizeof (argz[0]));

      return out;

}

int
parse_conf (char *input, char **argz)
{
      register short i = 0, cookies = 0, out = 0;

      argz[i++] = input;

      while ((*input) && (i < _MAXARGLINE_))
	{

	      if ((*input == 0x3a) || (*input == 0x20) || (*input == 0x0a))
		{
		      *(input++) = 0x00;
		      argz[i] = input;
		      cookies = 0x01;
		}
	      else
		{
		      if (cookies)
			{
			      i++;
			      cookies = 0x00;
			}
		      input++;
		}
	}

      out = i;
      memset (&argz[i], 0, (_MAXARGLINE_ - i) * sizeof (argz[0]));

      return out;
}
