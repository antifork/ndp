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

extern Channel *channel_ptr;
extern ndpControl ndp;

extern int Nchannel, fd_in;

void
set_nonblock (sock)
   int sock;
{
  register int val = 0;

  val = fcntl (sock, F_GETFL, val);
  val |= O_NONBLOCK;
  fcntl (sock, F_SETFL, val);
  return;
}

void
set_block (sock)
   int sock;
{
  register int val = 0;

  val = fcntl (sock, F_GETFL, val);
  val &= ~O_NONBLOCK;
  fcntl (sock, F_SETFL, val);
  return;
}


void
create_dialer_sock (ptr, vhost, rhost, rport)
   Channel * ptr;
   long vhost;
   long rhost;
   int  rport;
{
  /* SO_LINGER the system will process the close in a manner
     * that allows to continue as quickly as possible */

  struct linger linger_ = { 0, 0 };

  if ((ptr->fd_out = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      if (ndp.log_level)
	syslog (LOG_ERR, "Cannot create a socket:%s", strerror (errno));
      shutdown_ (NULL);
      return;
    }

  setsockopt (ptr->fd_out,
	      SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof (linger_));

  /* setting up vhost */

  memset (&(ptr->output_addr), 0, sizeof (struct sockaddr_in));

  ptr->output_addr.sin_family = AF_INET;
  ptr->output_addr.sin_addr.s_addr = vhost;
  ptr->output_addr.sin_port = htons ((u_short) 0);

  if (bind
      (ptr->fd_out, (struct sockaddr *) &(ptr->output_addr),
       sizeof (ptr->output_addr)) == -1)
    send_msg (NULL, "bind: %s %s\n",
	      multi_inet_nbotoa (ptr->output_addr.sin_addr.s_addr),
	      strerror (errno));

  /* setting up host to connect */

  ptr->output_addr.sin_family = AF_INET;
  ptr->output_addr.sin_port = htons ((u_short) rport);
  ptr->output_addr.sin_addr.s_addr = rhost;

  set_nonblock (ptr->fd_out);

  ptr->flag = _CONNECTING_;
  return;
}

void
shutdown_ (parg)
   Channel * parg;
{

  register Channel *ptr;

  ptr = ((parg == NULL) ? (channel_ptr) : (parg));

  if (ptr->fd_out > 2)
    {
      shutdown (ptr->fd_out, 2);
      close (ptr->fd_out);
    }

  if (ptr->fd_in > 2)
    {
      shutdown (ptr->fd_in, 2);
      close (ptr->fd_in);
    }

  if (ptr->buff_tmp)
    free (ptr->buff_tmp);

  reset_chan (ptr);

  return;
}

void
halfshutdown_ (parg)
   Channel * parg;
{
  struct linger linger_ = { 0, 0 };
  register Channel *ptr;

  ptr = ((parg == NULL) ? (channel_ptr) : (parg));

  if (ptr->fd_out > 2)
    {

      shutdown (ptr->fd_out, 2);
      close (ptr->fd_out);

      if ((ptr->fd_out = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
	  if (ndp.log_level)
	    syslog (LOG_ERR, "Cannot create a socket:%s", strerror (errno));
	  shutdown_ (NULL);
	  return;
	}

      setsockopt (ptr->fd_out, SOL_SOCKET, SO_LINGER, (char *) &linger_,
		  sizeof (linger_));

    }

  return;
}


int
read_from_channel (fd, buff)
   int fd;
   char *buff;
{
  register int nbyt = 0;

  if ((nbyt = recv (fd, buff, CHAN_SIZE_BUFF - 1, 0)) <= 0)
    {
      shutdown_ (NULL);
      return -1;
    }

  return nbyt;
}


int
write_to_channel (fd, buff, bt)
   int fd;
   char *buff;
   int bt;
{
  register int wbyt = 0;

  if ((wbyt = send (fd, buff, bt, 0)) <= 0)
    {
      shutdown_ (NULL);
      return -1;
    }

  return wbyt;
}
