/* 
 *  $Id$
 *  %ndp: usage
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
#include <stdio.h>

/* prototype */
void usage    __P ((char *, char *, char *));

void
usage (name, VERSION, ID)
     char         *name;
     char         *VERSION;
     char         *ID;
{
    fprintf (stderr, "usage: %s [option]\n", name);

    fprintf (stderr,
	     "   -L  --local     HOST:PORT   local host and listening port (all=INADDR_ANY)\n"
	     "   -R  --remote    HOST:PORT   remote host & port\n"
	     "   -V  --virtual   HOST        default vhost for outgoing connection\n\n"
	     "   -c  --conf      file        interactive mode (pseudo shell)\n"
	     "   -s [cl]                     stream (c=char-buffered l=line-buffered)\n" "   -l [0-2]                    log level (default=2)\n" "   -v  --version\n" "   -h  --help\n");

    exit (0);
}
