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

#ifdef LRU
#error "LRU already defined in some header."
#else
/* LRU must be a power of 2, and cannot be longer than 65536 elements. 
   Better performances can be obtained with a size over 32 ( 64 or 128).
*/
#define LRU  64 
#endif


/* test */

#if!(LRU == 2)&&!(LRU == 4)&&!(LRU == 8)&&!(LRU == 16)&&!(LRU == 32)&&!(LRU == 64)&&!(LRU == 128)&&!(LRU == 256)&&!(LRU == 512)&&!(LRU == 1024)&&!(LRU == 2048)&&!(LRU == 4096)&&!(LRU == 8192)&&!(LRU == 16384)&&!(LRU == 32768)&&!(LRU == 65536)
#error "LRU must be a power of 2, and no longer than 65536 elements."
#endif

#define HASHMASK \
(\
LRU >> 1 |\
LRU >> 2 |\
LRU >> 3 |\
LRU >> 4 |\
LRU >> 5 |\
LRU >> 6 |\
LRU >> 7 |\
LRU >> 8 |\
LRU >> 9 |\
LRU >> 10 |\
LRU >> 11 |\
LRU >> 12 |\
LRU >> 13 |\
LRU >> 14 |\
LRU >> 15 ) 

#define SFREE(x) \
\
({\
if(x)\
free(x); \
x=NULL;}) 

/*** table ***/

typedef struct _lru_
{

  unsigned int yday; /* day of the year */
  unsigned long addr;
  char *host;

}
lru;

static lru hostbyname[LRU];
static lru hostbyaddr[LRU];

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

static
#ifdef __GNUC__
  __inline
#else
#endif
  unsigned long
hash (const char *key, int m)
{
  int hash, i;

  if (key == NULL ) return 0;

  for (hash = 0, i = 0; i<m ; ++i)
    {
      hash += key[i];
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return (hash & HASHMASK);
}

/* gethostbyname utils */

static unsigned long
search_hostbyname (const char *host)
{
  register int i = 0;
  register long ret = -1;

  i = hash(host, strlen(host));

  /* null */

  if ( hostbyname[i].host == NULL )
        return ret;

  if ( !strcmp(host,hostbyname[i].host ))
        ret= hostbyname[i].addr;

        return ret;

}

static void
insert_hostbyname (const char *h, const unsigned long addr)
{
  register int i;

  i = hash(h, strlen(h));

  SFREE(hostbyname[i].host);

  hostbyname[i].host = strdup (h);
  hostbyname[i].addr = addr;

  return;
}

/* gethostbyaddr utils */

static char *
search_hostbyaddr (const unsigned long addr)
{
  register int i;
  static char *ret = NULL;

  SFREE (ret);

  if (!addr) return ret;

  i= hash ((char *)&addr,sizeof(long int));

  if (addr == hostbyaddr[i].addr)
      {
      ret = strdup (hostbyaddr[i].host);
      }

  return ret;
}

static void
insert_hostbyaddr (const char *h, const unsigned long addr)
{
  register int i;

  i = hash ((char *)&addr,4);

  SFREE (hostbyaddr[i].host);

  hostbyaddr[i].host = strdup (h);
  hostbyaddr[i].addr = addr;

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

      if ((ret = search_hostbyname (host)) != -1)
        /* hit */
        {
          return ret;
        }

      /* fail */

      if ((addr.s_addr = inet_addr (host)) == -1)
        {
          if (!(host_ent = gethostbyname (host)))
            fatalerr ("gethostbyname_lru err:%s", strerror (errno));

          bcopy (host_ent->h_addr, (char *) &addr.s_addr, host_ent->h_length);
        }

      insert_hostbyname (host, addr.s_addr);
      return addr.s_addr;

    }
  else
    fatalerr ("gethostbyname_lru err: %s pointer", (char *) NULL);

  return 0; /* unreachable */

}


char *
gethostbyaddr_lru (unsigned long addr)
{
  static char *ret = NULL;
  struct hostent *hostname;


  SFREE (ret);

  if (addr == 0) return "0.0.0.0";


  if (!(opt & OPT_NUM))
    {

      if ((ret = search_hostbyaddr (addr)) != NULL)
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

    }
  else
    ret = strdup (inet_ntoa (*(struct in_addr *) &addr));

  if (ret && addr)
    insert_hostbyaddr (ret, addr);
  else
    fatalerr ("gethostbyaddr_lru err:%s", strerror (errno));

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
