/* 
 *  $Id$
 *  %ndp: irc implementation
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

#include <irc.h>
#include <util.h>

extern Channel *channel_ptr;

static char *irc_commands[] = { "NICK", "USER", "ISON", "PING", "QUIT", "JOIN", "NOTICE", NULL };
static char bufftmp[CHAN_SIZE_BUFF];

char sep = 0xff;

int
match_irc (buff)
   char *buff;
{
  register int i = 0;

  while (irc_commands[i] && (i < _MAXARGLINE_))
    {
      if (!strncmp (buff, irc_commands[i], strlen (irc_commands[i])))
	break;
      i++;
    }

  return ((i == 7) ? 0 : (i + 1));
}


int
irc_postlogin ()
{
  char *ptr = NULL;

  ptr = strmrg ((char *) NULL, "NICK ");

  if (*channel_ptr->usernick != '\0' )
    ptr = strmrg (ptr, channel_ptr->usernick);
  else
    ptr = strmrg (ptr, "ndp-");

  ptr = strmrg (ptr, "\nUSER ");

  if (*channel_ptr->username != '\0')
    ptr = strmrg (ptr, channel_ptr->username);
  else
    ptr =
      strmrg (ptr, "ndp ndp ndp :fake client using ndp. http://awgn.antifork.org");

  ptr = strmrg (ptr, "\n");

  return (send (channel_ptr->fd_out, ptr, strlen (ptr), 0));

}


#define IRC_COMM_NICK	0x01
#define IRC_COMM_USER	0x02

void
irc_controller (ptr)
   char *ptr;
{

  char *argirc[_MAXARGLINE_];

  register int i = 0, irc_command = 0;

  memset (&argirc, 0, sizeof (argirc));
  memset (bufftmp, 0, CHAN_SIZE_BUFF);

  strncpy (bufftmp, ptr, CHAN_SIZE_BUFF - 1);

  (void) parse_irc (bufftmp, argirc);

  channel_ptr->opts &= ~CH_COOKIES_;

  while (argirc[i] && (i < _MAXARGLINE_))
    {
      if ((irc_command = match_irc (argirc[i])))
	{

	  channel_ptr->opts |= CH_IRC_;
	  channel_ptr->opts |= CH_COOKIES_;

	  switch (irc_command)
	    {

	    case IRC_COMM_NICK:
	      memset (channel_ptr->usernick, 0, CHAN_SIZE_NICK);

	      if (argirc[i + 1] != NULL)
		strncpy (channel_ptr->usernick, argirc[i + 1],
			 CHAN_SIZE_NICK - 1);
	      break;
	    case IRC_COMM_USER:
	      memset (channel_ptr->username, 0, CHAN_SIZE_USR);

	      while ((argirc[i + 1] != &sep) && (argirc[i + 1] != NULL))
		{

		  if ((CHAN_SIZE_USR - strlen (channel_ptr->username) - 2) >
		      0)
		    {
		      strncat (channel_ptr->username, argirc[i + 1],
			       CHAN_SIZE_USR -
			       strlen (channel_ptr->username) - 2);
		      strcat (channel_ptr->username, " ");
		    }

		  i++;

		}
	      break;
	    }
	}
      i++;
    }


  if (channel_ptr->opts & CH_COOKIES_)
    channel_ptr->trial--;

  return;
}



void
stream_guesser (ptr)
   char *ptr;
{

  register int i = 0;

  while (irc_commands[i])
    {
      if (strstr (ptr, irc_commands[i++]))
	channel_ptr->opts |= CH_LINE_;
    }

  if (!(channel_ptr->opts & CH_LINE_))
    channel_ptr->opts |= CH_CHAR_;

  channel_ptr->opts &= ~CH_UNKNOWN_;

  return;

}
