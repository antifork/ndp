/* 
 *  $Id$
 *  %ndp: string utils 
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


#include <ctrl.h>

void
void_cr_lf (ptr, mode)
     u_char       *ptr;
     int           mode;
{
    char         *ptr_1,
                 *ptr_2;

    if (ptr == NULL)
	return;

    while (*ptr)
	{

	    ptr_1 = ptr + 1;
	    ptr_2 = ptr + 2;

	    if (mode)
		{

		    if ((*ptr == 0xf2) && (!*ptr_1 || (*ptr_1 == 0x0d)))
			*ptr = 0x00;
		}

	    else
		{
		    if (((*ptr == 0x0a) || (*ptr == 0x0d)) && (!*ptr_1 || !*ptr_2))
			{
			    *ptr = 0x00;
			    return;
			}
		}

	    ptr++;

	}
    return;
}


char
getlastchar (buff)
     u_char       *buff;
{
    if (buff == NULL)
	return 0;

    while (*buff != '\0')
	{
	    if (*buff == '\n')
		*(buff + 1) = '\0';
	    buff++;
	}

    return (char) *(--buff);
}
