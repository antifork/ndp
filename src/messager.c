/*
 *  $Id$
 *  %ndp: messager
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

#include <msger.h>
#include <util.h>

extern chan_t *p_chan;
extern ctrl_t ndp;

int
send_msg(chan_t * ptr, char *pad,...)
{
	chan_t *l_ptr;
	static char *local_buff;
	char pre_msg[] = "\n:ndp!ndp@antifork.org WALLOPS :";
	char *o_buff;
	va_list ap;

	l_ptr = (ptr ? ptr : p_chan);

	if (local_buff)
		free(local_buff);

	va_start(ap, pad);
	vasprintf(&local_buff, pad, ap);
	va_end(ap);

	if (local_buff == NULL)
		return 0;

	if (l_ptr->opts & CH_IRC)
		o_buff = strmrg(pre_msg, local_buff);
	else
		o_buff = local_buff;

	if (!(l_ptr->opts & CH_JOIN) || !(l_ptr->opts & CH_COOKIES)) {
		if (l_ptr->opts & CH_COOKIES)
			l_ptr->opts |= CH_JOIN;

		return (send(l_ptr->fd_in, o_buff, strlen(o_buff), 0));
	}
	return 0;		/* That's unreachable */
}
