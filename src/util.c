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

#define MAX_LRU         16


/*** table ***/

typedef struct raw
{

  unsigned short v;
  unsigned long addr;
  char *host;

}
lru;

static lru direct[MAX_LRU];
static lru reverse[MAX_LRU];

/*** private fuctions ***/

static void
fatalerr (char *pattern, ...)
{
  va_list ap;

  va_start (ap, pattern);
  vfprintf (stderr, pattern, ap);
  fprintf (stderr, "; exit forced.\n");
  va_end (ap);

  exit (-1);

}


static unsigned long
search_direct_lru (const char *host)
{
  register int i = 0;
  register long ret = -1;

  while (i < MAX_LRU)
    {
      if (direct[i].v)
        {
          if (!strcmp (host, direct[i].host))
            ret = direct[i].addr;

        }

      direct[i].v <<= 1;

      if (direct[i].v == 0)
        {
          if (direct[i].host)
            {
              free (direct[i].host);
              direct[i].host = NULL;
            }

          direct[i].addr = 0;
        }

      i++;
    }

  return ret;
}

static void
insert_direct_lru (const char *host, const unsigned long addr)
{
  register int i = 0;
  register int j = 0;

  while ((i < MAX_LRU) && direct[j].v)
    {
      if (direct[i].v < direct[j].v)
        j = i;

      i++;
    }

  if (direct[j].host)
    free (direct[j].host);

  /* insert */

  direct[j].v = 1;
  direct[j].addr = addr;
  direct[j].host = strdup (host);

}


static char *
search_reverse_lru (const unsigned long addr)
{
  register int i = 0;
  static char *ret = NULL;

  if (ret)
    free (ret);

  ret = NULL;

  while (i < MAX_LRU)
    {

      if (reverse[i].v)
        {
          if (addr == reverse[i].addr)
            ret = strdup (reverse[i].host);

          reverse[i].v <<= 1;

        }

      if (reverse[i].v == 0)
        {
          if (reverse[i].host)
            {
              free (reverse[i].host);
              reverse[i].host = NULL;
            }

          reverse[i].addr = 0;
        }

      i++;
    }

  return ret;
}


static void
insert_reverse_lru (const char *h, const unsigned long addr)
{
  register int i = 0;
  register int j = 0;

  while (i < MAX_LRU && reverse[j].v)
    {
      if (reverse[i].v < reverse[j].v)
        j = i;
      i++;
    }

  if (reverse[j].host)
    free (reverse[j].host);


  /* insert */

  reverse[j].v = 1;
  reverse[j].addr = addr;
  reverse[j].host = strdup (h);

  return;
}

/*** public function ***/


unsigned long
gethostbyname_lru (const char *host)
{
  struct in_addr addr;
  struct hostent *host_ent;

  long ret;

  if (host)
    {

      if ((ret = search_direct_lru (host)) != -1)
        /* hit */
        {
          return ret;
        }

      /* fail */

      if ((addr.s_addr = inet_addr (host)) == -1)
        {
          if (!(host_ent = gethostbyname (host)))
            fatalerr ("(gethostbyname_lru) err:%s", strerror (errno));

          bcopy (host_ent->h_addr, (char *) &addr.s_addr, host_ent->h_length);
        }

      insert_direct_lru (host, addr.s_addr);
      return addr.s_addr;

    }
  else
    fatalerr ("(gethostbyname_lru) err: %s pointer", (char *) NULL);

  return 0;

}

char *
gethostbyaddr_lru (unsigned long addr)
{
  static char *ret = NULL;
  struct hostent *hostname;

  if (ret)
    free (ret);
  ret = NULL;

      if ((ret = search_reverse_lru (addr)) != NULL)
        /* hit */
        {
          ret = strdup (ret);
          return ret;
        }
        /* fail */

      if ((hostname = gethostbyaddr ((char *) &addr, 0x04, AF_INET)) != NULL)
        ret = strdup (hostname->h_name);
      else
        ret = strdup (inet_ntoa (*(struct in_addr *) &addr));

  if (ret)
    insert_reverse_lru (ret, addr);
  else
    fatalerr ("(gethostbyaddr_lru) err:%s", strerror (errno));

  return ret;
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
