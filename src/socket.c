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
set_nonblock (int sock)
{
  register int val = 0;

  val = fcntl (sock, F_GETFL, val);
  val |= O_NONBLOCK;
  fcntl (sock, F_SETFL, val);
  return;

}

void
set_block (int sock)
{
  register int val = 0;

  val = fcntl (sock, F_GETFL, val);
  val &= ~O_NONBLOCK;
  fcntl (sock, F_SETFL, val);
  return;

}


void
create_dialer_sock (Channel * ptr, long vhost, long rhost, int rport)
{
  /* SO_LINGER the system will process the close in a manner
     * that allows to continue as quickly as possible */

  struct linger linger_;
  register int k = 0;

  linger_.l_onoff = 0;


  while ((k++ < 10)
	 && ((ptr->fd_out = socket (AF_INET, SOCK_STREAM, 0)) == -1))
    usleep (1000);

  if (ptr->fd_out == -1)
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
halfshutdown_ (void)
{
  struct linger linger_;
  register int k = 0;

  if (channel_ptr->fd_out)
    {

      close (channel_ptr->fd_out);

      linger_.l_onoff = 0;


      while ((k++ < 10)
	     && ((channel_ptr->fd_out = socket (AF_INET, SOCK_STREAM, 0)) ==
		 -1))
	usleep (1000);

      if (channel_ptr->fd_out == -1)
	{
	  if (ndp.log_level)
	    syslog (LOG_ERR, "Cannot create a socket:%s", strerror (errno));
	  shutdown_ (NULL);
	  return;
	}

      setsockopt (channel_ptr->fd_out,
		  SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof (linger_));

    }

  return;
}


void
shutdown_ (Channel * parg)
{

  register Channel *ptr;

  if (parg == NULL)
    ptr = channel_ptr;
  else
    ptr = parg;

  if (ptr->fd_out > 0)
    close (ptr->fd_out);
  if (ptr->fd_in > 0)
    close (ptr->fd_in);

  if (ptr->buff_tmp)
    free (ptr->buff_tmp);

  reset_chan (ptr);

  return;
}


int
read_from_channel (int fd, char *buffer)
{
  register int nbyt = 0;

  nbyt = recv (fd, buffer, CHAN_SIZE_BUFF - 1, 0);

  if (nbyt <= 0)
    {
      shutdown_ (NULL);
      return -1;
    }

  return nbyt;
}


int
write_to_channel (int fd, char *buffer, int bt)
{
  register int wbyt = 0;
  wbyt = send (fd, buffer, bt, 0);

  if ((wbyt <= 0))
    {
      shutdown_ (NULL);
      return -1;
    }

  return wbyt;
}
