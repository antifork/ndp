/* Replacements for routines missing on some systems.
   Copyright (C) 1995, 1996, 1997 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <defs.h>
#include <std.h>

#ifndef HAVE_STRERROR
/* A strerror() clone, for systems that don't have it.  */
char *
strerror (int err)
{
  /* This loses on a system without `sys_errlist'.  */
  extern char **sys_errlist;
  return sys_errlist[err];
}
#endif /* not HAVE_STRERROR */


/* Some systems don't have some str* functions in libc.  Here we
   define them. */

#ifndef HAVE_STRCASECMP
/* From GNU libc.  */
/* Compare S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexiographically less than,
   equal to or greater than S2.  */
int
strcasecmp (const char *s1, const char *s2)
{
  register const unsigned char *p1 = (const unsigned char *) s1;
  register const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (p1 == p2)
    return 0;

  do
    {
      c1 = tolower (*p1++);
      c2 = tolower (*p2++);
      if (c1 == '\0')
	break;
    }
  while (c1 == c2);

  return c1 - c2;
}
#endif /* not HAVE_STRCASECMP */


#ifndef HAVE_STRNCASECMP
/* From GNU libc.  */
/* Compare no more than N characters of S1 and S2,
   ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less
   than, equal to or greater than S2.  */
int
strncasecmp (const char *s1, const char *s2, size_t n)
{
  register const unsigned char *p1 = (const unsigned char *) s1;
  register const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (p1 == p2 || n == 0)
    return 0;

  do
    {
      c1 = tolower (*p1++);
      c2 = tolower (*p2++);
      if (c1 == '\0' || c1 != c2)
	return c1 - c2;
    }
  while (--n > 0);

  return c1 - c2;
}
#endif /* not HAVE_STRNCASECMP */


#ifndef HAVE_STRSTR
/* From GNU libc 2.0.6.  */
/* Return the first ocurrence of NEEDLE in HAYSTACK.  */
/*
 * My personal strstr() implementation that beats most other algorithms.
 * Until someone tells me otherwise, I assume that this is the
 * fastest implementation of strstr() in C.
 * I deliberately chose not to comment it.  You should have at least
 * as much fun trying to understand it, as I had to write it :-).
 *
 * Stephen R. van den Berg, berg@pool.informatik.rwth-aachen.de	*/
typedef unsigned chartype;

char *
strstr (phaystack, pneedle)
     const char *phaystack;
     const char *pneedle;
{
  register const unsigned char *haystack, *needle;
  register chartype b, c;

  haystack = (const unsigned char *) phaystack;
  needle = (const unsigned char *) pneedle;

  b = *needle;
  if (b != '\0')
    {
      haystack--;		/* possible ANSI violation */
      do
	{
	  c = *++haystack;
	  if (c == '\0')
	    goto ret0;
	}
      while (c != b);

      c = *++needle;
      if (c == '\0')
	goto foundneedle;
      ++needle;
      goto jin;

      for (;;)
	{
	  register chartype a;
	  register const unsigned char *rhaystack, *rneedle;

	  do
	    {
	      a = *++haystack;
	      if (a == '\0')
		goto ret0;
	      if (a == b)
		break;
	      a = *++haystack;
	      if (a == '\0')
		goto ret0;
	  shloop:}
	  while (a != b);

	jin:a = *++haystack;
	  if (a == '\0')
	    goto ret0;

	  if (a != c)
	    goto shloop;

	  rhaystack = haystack-- + 1;
	  rneedle = needle;
	  a = *rneedle;

	  if (*rhaystack == a)
	    do
	      {
		if (a == '\0')
		  goto foundneedle;
		++rhaystack;
		a = *++needle;
		if (*rhaystack != a)
		  break;
		if (a == '\0')
		  goto foundneedle;
		++rhaystack;
		a = *++needle;
	      }
	    while (*rhaystack == a);

	  needle = rneedle;	/* took the register-poor approach */

	  if (a == '\0')
	    break;
	}
    }
foundneedle:
  return (char *) haystack;
ret0:
  return 0;
}
#endif /* not HAVE_STRSTR */
