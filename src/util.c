/*
 *  $Id$ 
 *  %ndp util 
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
#include <defs.h>
#include <std.h>
#include <util.h>


unsigned long
getlongbyname (unsigned char *host)
{
  struct in_addr addr;
  struct hostent *host_ent;

  if ((addr.s_addr = inet_addr (host)) == -1)
    {
      if (!(host_ent = gethostbyname (host)))
	return 0;
      bcopy (host_ent->h_addr, (char *) &addr.s_addr, host_ent->h_length);
    }
  return addr.s_addr;
}

char *
getnamebynbo (u_long addr)
{
  struct hostent *hostname;

  if ((hostname = gethostbyaddr ((char *) &addr, 0x04, AF_INET)) != NULL)
    return (char *) hostname->h_name;
  return (char *) multi_inet_nbotoa (addr);
}

char *
multi_inet_nbotoa (unsigned long address)
{

#define N_STATIC_BUFF   4
  static unsigned int bit_flag_;
  static char *buff[N_STATIC_BUFF];

  bit_flag_++;

  buff[bit_flag_ % N_STATIC_BUFF] =
    (char *) realloc (buff[bit_flag_ % N_STATIC_BUFF], 16);
  memset (buff[bit_flag_ % N_STATIC_BUFF], 0, 16);
  memcpy (buff[bit_flag_ % N_STATIC_BUFF],
	  (char *) inet_ntoa (*(struct in_addr *) &address), 15);

  return (char *) buff[bit_flag_ % N_STATIC_BUFF];

}

char *
strmrg (char *buff_1, char *buff_2)
{
  register int s_1 = 0, s_2 = 0;
  register char *_ptr1 = buff_1, *_ptr2 = buff_2, *_ptr = NULL;

  static char *_pbuf;

  if (_ptr1)
    {

      s_1 = strlen (_ptr1);

      if (_ptr1 == _pbuf)
	{
	  _ptr1 = alloca (s_1 + 1);
	  *(_ptr1 + s_1) = '\0';
	  memcpy (_ptr1, buff_1, s_1);
	}
    }

  if (_ptr2)
    {

      s_2 = strlen (_ptr2);

      if (_ptr2 == _pbuf)
	{
	  _ptr2 = alloca (s_2 + 1);
	  *(_ptr2 + s_2) = '\0';
	  memcpy (_ptr2, buff_2, s_2);
	}
    }


  _ptr = _pbuf = realloc (_pbuf, s_1 + s_2 + 1);

  if (_ptr1)
    {
      while ((*_ptr++ = *_ptr1++));
      _ptr--;
    }

  if (_ptr2)
    while ((*_ptr++ = *_ptr2++));

  return (char *) _pbuf;

}
