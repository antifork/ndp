/*
 *  $Id$
 *  %ndp channel.h 
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

#ifndef CHANNEL_H
#define CHANNEL_H

#define	CHAN_SBUFF	1024
#define CHAN_SNICK 	12
#define CHAN_SUSR	80
#define CHAN_SPRPT	80 

typedef struct _channel_ {
	u_char  bufin [CHAN_SBUFF];
	u_char  bufout[CHAN_SBUFF];
        u_char  nick  [CHAN_SNICK];
        u_char  name  [CHAN_SUSR ];
        u_char  prompt[CHAN_SPRPT];
	u_char  *tmp;

        long    sec;

        int     state;
	int	tmp_size;
	int     fd_out;
	int     fd_in;
	int     opts;

	struct sockaddr_in 	o_addr;
	struct sockaddr_in 	c_addr;
	struct _channel_ 	*next;

	char    trial;
        char    class;

} chan_t;

typedef struct _ndp_conf_ {
	struct {
		unsigned short  idle;
		unsigned short  idlep;
		unsigned short  maxuser;
		unsigned short  maxerr;
		unsigned short  lp_irc;
		unsigned short  hp_irc;

	} conf;

	struct {
		unsigned char* master;
		unsigned char* user;
		unsigned char* ircer0;
		unsigned char* ircer1;
		unsigned char* ircer2;
	} pass;

	char            *lhost;
	char            *vhost;
	char            *rhost;
	char		*ivhost;
	char		*jvhost;

	unsigned long   nbo_lhost;
	unsigned long   nbo_vhost;
	unsigned long   nbo_ivhost;
	unsigned long   nbo_jvhost;
	unsigned long   nbo_rhost;
	unsigned short  lport;
	unsigned short  rport;

	int		opts;

        short           log_level;

} ctrl_t;

#endif /* CHANNEL_H */
