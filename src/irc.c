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

extern chan_t *p_chan;

static char bufftmp[CHAN_SBUFF];
static char *irc_commands[] = {"NICK", "USER", "ISON", "PING", "QUIT", "JOIN", "NOTICE", NULL};
static int irc_comm_len[] = {4, 4, 4, 4, 4, 4, 6, 0};

char sep = 0xff;

int
match_irc(buff)
	char *buff;
{
	register int i = 0;

	while (irc_commands[i] && i < MAXARGLINE) {
		if (!strncmp(buff, irc_commands[i], irc_comm_len[i]))
			break;
		i++;
	}
	return ((i == 7) ? 0 : (i + 1));
}


int
irc_postlogin()
{
	char *ptr = NULL;

	ptr = strmrg((char *) NULL, "NICK ");

	if (*p_chan->nick != '\0')
		ptr = strmrg(ptr, p_chan->nick);
	else
		ptr = strmrg(ptr, "ndp-");

	ptr = strmrg(ptr, "\nUSER ");

	if (*p_chan->name != '\0')
		ptr = strmrg(ptr, p_chan->name);
	else
		ptr = strmrg(ptr, "ndp ndp ndp :fake client using ndp. http://awgn.antifork.org");

	ptr = strmrg(ptr, "\n");
	return (send(p_chan->fd_out, ptr, strlen(ptr), 0));
}


#define IRC_COMM_NICK	0x01
#define IRC_COMM_USER	0x02

void
irc_controller(ptr)
	char *ptr;
{

	char *argirc[MAXARGLINE];

	register int i = 0, irc_command = 0;

	memset(&argirc, 0, sizeof(argirc));
	memset(bufftmp, 0, CHAN_SBUFF);
	strncpy(bufftmp, ptr, CHAN_SBUFF - 1);

	(void) parse_irc(bufftmp, argirc);
	p_chan->opts &= ~CH_COOKIES;

	for (i = 0; argirc[i] && i < MAXARGLINE; i++) {

		if ((irc_command = match_irc(argirc[i]) == 0))
			continue;

		/* IRC command matched */
		p_chan->opts |= CH_IRC;
		p_chan->opts |= CH_COOKIES;

		switch (irc_command) {
		case IRC_COMM_NICK:
			memset(p_chan->nick, 0, CHAN_SNICK);
			if (argirc[i + 1] != NULL)
				strncpy(p_chan->nick, argirc[i + 1], CHAN_SNICK - 1);
			break;

		case IRC_COMM_USER:
			memset(p_chan->name, 0, CHAN_SUSR);
			while ((argirc[i + 1] != &sep) && (argirc[i + 1] != NULL)) {
				if ((CHAN_SUSR - strlen(p_chan->name) - 2) > 0) {
					strncat(p_chan->name, argirc[i + 1], CHAN_SUSR - strlen(p_chan->name) - 2);
					strcat(p_chan->name, " ");
				}
				i++;
			}

		}

	}

	if (p_chan->opts & CH_COOKIES)
		p_chan->trial--;

	return;
}



void
stream_dissector(ptr)
	char *ptr;
{
	register int i = 0;

	while (irc_commands[i]) {
		if (strstr(ptr, irc_commands[i++]))
			p_chan->opts |= CH_LINE;
	}

	if (!(p_chan->opts & CH_LINE))
		p_chan->opts |= CH_CHAR;

	p_chan->opts &= ~CH_UNKNOWN;
	return;
}
