/*
 *  $Id$
 *  %ndp: socket tools
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

#include <stdlib.h>
#include <socket.h>
#include <util.h>
#include <ns-cache.h>

extern chan_t *p_chan;
extern ctrl_t ndp;
extern int Nchannel, fd_in;

void
set_nonblock(sock)
	int sock;
{
	register int val = 0;
	val = fcntl(sock, F_GETFL, val);
	val |= O_NONBLOCK;
	fcntl(sock, F_SETFL, val);
	return;
}

void
set_block(sock)
	int sock;
{
	register int val = 0;
	val = fcntl(sock, F_GETFL, val);
	val &= ~O_NONBLOCK;
	fcntl(sock, F_SETFL, val);
	return;
}


void
create_dialer_sock(ptr, vhost, rhost, rport)
	chan_t *ptr;
	long vhost;
	long rhost;
	int rport;
{
	/*
	 * SO_LINGER the system will process the close in a manner that
	 * allows to continue as quickly as possible
	 */

	struct linger linger_ = {0, 0};

	if ((ptr->fd_out = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		if (ndp.log_level)
			syslog(LOG_ERR, "Cannot create a socket:%s", strerror(errno));

		shutdown_(NULL);
		return;
	}
	setsockopt(ptr->fd_out, SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof(linger_));

	/* setting up vhost */

	memset(&(ptr->o_addr), 0, sizeof(struct sockaddr_in));
	ptr->o_addr.sin_family = AF_INET;
	ptr->o_addr.sin_addr.s_addr = vhost;
	ptr->o_addr.sin_port = htons((u_short) 0);

	if (bind(ptr->fd_out, (struct sockaddr *) & (ptr->o_addr), sizeof(ptr->o_addr)) == -1)
		send_msg(NULL, "bind: %s %s\n", safe_inet_ntoa(ptr->o_addr.sin_addr.s_addr), strerror(errno));

	/* setting up host to connect */

	ptr->o_addr.sin_family = AF_INET;
	ptr->o_addr.sin_addr.s_addr = rhost;
	ptr->o_addr.sin_port = htons((u_short) rport);
	set_nonblock(ptr->fd_out);
	ptr->state = S_CONN;

	return;
}

void
shutdown_(parg)
	chan_t *parg;
{

	register chan_t *ptr;

	ptr = ((parg == NULL) ? (p_chan) : (parg));

	if (ptr->fd_out > 2) {
		shutdown(ptr->fd_out, 2);
		close(ptr->fd_out);
	}
	if (ptr->fd_in > 2) {
		shutdown(ptr->fd_in, 2);
		close(ptr->fd_in);
	}
	if (ptr->tmp)
		free(ptr->tmp);

	reset_chan(ptr);
	return;
}


void
halfshutdown_(parg)
	chan_t *parg;
{
	struct linger linger_ = {0, 0};
	register chan_t *ptr;

	ptr = ((parg == NULL) ? (p_chan) : (parg));

	if (ptr->fd_out > 2) {
		shutdown(ptr->fd_out, 2);
		close(ptr->fd_out);

		if ((ptr->fd_out = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			if (ndp.log_level)
				syslog(LOG_ERR, "Cannot create a socket:%s", strerror(errno));
			shutdown_(NULL);
			return;
		}
		setsockopt(ptr->fd_out, SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof(linger_));
	}
	return;
}


int
read_chan(fd, buff)
	int fd;
	char *buff;
{
	register int nbyt = 0;

	if ((nbyt = recv(fd, buff, CHAN_SBUFF - 1, 0)) <= 0) {
		shutdown_(NULL);
		return -1;
	}
	return nbyt;
}


int
write_chan(fd, buff, bt)
	int fd;
	char *buff;
	int bt;
{
	register int wbyt = 0;

	if ((wbyt = send(fd, buff, bt, 0)) <= 0) {
		shutdown_(NULL);
		return -1;
	}
	return wbyt;
}
