/* 
 *  $Id$
 *  %ndp: configuration reader  
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

#include <stdlib.h>
#include <conf.h>

extern ndpControl ndp;
extern char opts;
static char bufftmp[CHAN_SIZE_BUFF];

void 
readconf (char *CONFIG)
{

      FILE       *conf;
      int         counter = 0, n = 0;
      char       *argz[_MAXARGLINE_];


      if (!(conf = fopen (CONFIG, "r")))
	{

	      if (!(conf = fopen ("ndp.conf", "r")))
		{
		      fprintf (stderr, "Can't read .conf file. Create a ndp.conf file or run -c filename.\n");
		      exit (-1);
		}
	      else
		    fprintf (stderr, "Can't read any .conf file. Running with default settings!?!?.\n");
	      return;
	}


      while (fgets (bufftmp, 1024, conf) > 0)
	{
	      ++counter;
	      n = parse_conf (bufftmp, argz);
	      if (strstr (argz[0], "#") == NULL)
		{
		      switch (*argz[0])
			{

			case '\n':
			case '\r':
			case '\0':
			case ' ':
			      break;

			case 'S':
			      if (n < 3)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      if (!Isdigit (argz[2]))
				    Warning ("port must be an integer. Error at line %d in %s!\n", counter, CONFIG);

			      strncpy (ndp.lhost, argz[1], 79);

			      if (!strcasecmp (argz[1], "ALL"))
				    ndp.lhost_4b = getlongbyname (ndp.lhost);

			      ndp.lport = (unsigned short) atoi (argz[2]);
			      if (argz[3] != NULL)
				{
				      strncpy (ndp.vhost, argz[3], 79);
				      ndp.vhost_4b = getlongbyname (ndp.vhost);
				}
			      break;

			case 'T':
			      if (n < 2)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      if (!Isdigit (argz[1]))
				    Warning ("T's arg must be an integer. Error at line %d in %s!\n", counter, CONFIG);
			      ndp.conf.T = (short) atoi (argz[1]);
			      break;
			case 't':
			      if (n < 2)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      if (!Isdigit (argz[1]))
				    Warning ("t's arg must be an integer. Error at line %d in %s!\n", counter, CONFIG);
			      ndp.conf.t = (short) atoi (argz[1]);
			      break;

			case 'n':
			      if (n < 2)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      if (!Isdigit (argz[1]))
				    Warning ("n's arg must be an integer. Error at line %d in %s!\n", counter, CONFIG);
			      ndp.conf.n = (short) atoi (argz[1]);
			      break;
			case 'e':
			      if (n < 2)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      if (!Isdigit (argz[1]))
				    Warning ("e's arg must be an integer. Error at line %d in %s!\n", counter, CONFIG);
			      ndp.conf.e = (short) atoi (argz[1]);
			      break;
			case 'M':
			      if (n < 2)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      strncpy ((unsigned char *) ndp.pass.M, (unsigned char *) argz[1], 40);
			      break;

			case 'U':
			      if (n < 2)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      strncpy ((unsigned char *) ndp.pass.U, (unsigned char *) argz[1], 40);
			      break;

			case 'I':
			      if (n < 2)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      strncpy ((unsigned char *) ndp.pass.I, (unsigned char *) argz[1], 40);
			      break;
			case 'i':
			      if (n < 3)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      strncpy ((unsigned char *) ndp.pass.i, (unsigned char *) argz[1], 40);
			      strncpy ((unsigned char *) ndp.ivhost, (unsigned char *) argz[2], 79);
			      ndp.ivhost_4b = getlongbyname (ndp.ivhost);
			      break;
			case 'j':
			      if (n < 4)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      if (!Isdigit (argz[4]))
				    Warning ("j's port must be integers. Error at line %d in %s!\n", counter, CONFIG);

			      strncpy ((unsigned char *) ndp.pass.j, (unsigned char *) argz[1], 40);
			      strncpy ((unsigned char *) ndp.jvhost, (unsigned char *) argz[2], 79);
			      strncpy ((unsigned char *) ndp.rhost, (unsigned char *) argz[3], 79);
			      ndp.jvhost_4b = getlongbyname (ndp.jvhost);
			      ndp.rhost_4b = getlongbyname (ndp.rhost);
			      ndp.rport = atoi (argz[4]);
			      break;
			case 'P':
			      if (n < 3)
				    Warning ("Too few arguments in line %d in %s!\n", counter, CONFIG);
			      if (!Isdigit (argz[1]) || !Isdigit (argz[2]))
				    Warning ("P's args must be integers. Error at line %d in %s!\n", counter, CONFIG);
			      ndp.conf.lp_irc = (short) atoi (argz[1]);
			      ndp.conf.hp_irc = (short) atoi (argz[2]);
			      if (ndp.conf.lp_irc > ndp.conf.hp_irc)
				    Warning ("lowport must be <= hiport. Error at line %d in %s!\n", counter, CONFIG);
			      break;


			}
		}
	}
      fclose (conf);

      return;

}
