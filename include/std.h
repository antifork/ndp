/*
 *  $Id$ 
 *  %ndp std.h
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


#define ISDIGIT(x)              ( (atoi(x)) > 0 ? (1) : (0) )

/*
Standard includes
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*
Check if the system supports POSIX.1 
*/

#if HAVE_UNISTD_H
# include <sys/types.h>
# include <unistd.h>
#endif

/*
Getoptlong is POSIX.2 compliant
*/

#if HAVE_GETOPT_H
#include <getopt.h>
#else
#define getopt getopt_loser
#include "getopt.h"
#undef getopt
#endif

/*
signal, calls, errors and logs.
*/

#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>

/*
See AC_HEADER_STDC in 'info autoconf'
*/

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr (), *strrchr ();
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# endif
#endif

/* 
See AC_HEADER_TIME in 'info autoconf' 
*/

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

/*
from autoconf documentation
*/

#if defined(__GNUC__) && !defined(HAVE_ALLOCA_H)
# define alloca __builtin_alloca
#else
# if HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifdef _AIX
#pragma alloca
#  else
#   ifndef alloca
char *alloca();
#   endif
#  endif
# endif
#endif

/*
Check sys/wait include
*/

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#include <ctype.h>

/*
network headers 
*/

#include <netdb.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <net/if.h>

