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

#define	CHAN_SIZE_BUFF	1024
#define CHAN_SIZE_NICK 	12
#define CHAN_SIZE_USR	80
#define CHAN_SIZE_PRPT	80 


typedef struct _channel_ {
	int             flag;
	u_char   	buffer_in [CHAN_SIZE_BUFF];
	u_char   	buffer_out[CHAN_SIZE_BUFF];
	u_char   	*buff_tmp;
	int		buff_tmp_size;
	int             fd_out;
	int             fd_in;
	long            sec;
	char            trial;
	int             opts;
	char            class;
	u_char          usernick[CHAN_SIZE_NICK];
	u_char          username[CHAN_SIZE_USR ];
	u_char		prompt  [CHAN_SIZE_PRPT];
	struct sockaddr_in output_addr;
	struct sockaddr_in caller_addr;
	struct _channel_ *next;
} Channel;

typedef struct _ndp_conf_ {
	struct {
		unsigned short  T;
		unsigned short  t;
		unsigned short  c;
		unsigned short  n;
		unsigned short  e;
		unsigned short  lp_irc;
		unsigned short  hp_irc;

	} conf;
	struct {
		unsigned char   M[41];
		unsigned char   U[41];
		unsigned char   I[41];
		unsigned char   i[41];
		unsigned char   j[41];
	} pass;

	char            lhost[80];
	char            vhost[80];
	char            rhost[80];
	char		ivhost[80];
	char		jvhost[80];

	unsigned long   lhost_4b;
	unsigned long   vhost_4b;
	unsigned long   ivhost_4b;
	unsigned long   jvhost_4b;
	unsigned long   rhost_4b;

	unsigned short  lport;
	unsigned short  rport;

	short		log_level;
	int		opts;

} ndpControl;
