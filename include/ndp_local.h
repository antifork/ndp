/* 
 *  $Id$
 *  %ndp ndp_local.h 
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


#include "defs.h"
#include "ndp.h"
#include "std.h"
#include "channel.h"
#include <pwd.h>

#if defined(HAVE_SYS_SELECT_H) 
#include <sys/select.h>
#endif

/*
*/

int 	accept_client      	(void);
int 	handle_link        	(void);
int 	setup_dialer       	(void);
int 	connect_target     	(void);
int 	ushell			(void);


int  	get_class       	(int);

void 	void_chan          	(Channel * );
void 	reset_chan         	(Channel * );
void 	add_chan           	(Channel * );

void 	server             	(void);
void 	_quit              	(int);

void	rehash_time 		(void);
void	halfshutdown_ 		(void);
void	shutdown_       	(Channel *);
void	set_nonblock 		(int);
void	set_block		(int);
void	usage 			(char *, char *, char *);
int	irc_postlogin 		(void);
void	stream_guesser  	(char *);
void	readconf        	(char *);
void	create_dialer_sock	(Channel *,long,long,int);

void	rehash_time             (void);
void	irc_controller          (u_char *);
void	welcome                 (void);
void	void_cr_lf              (u_char *, int);
int	parse_input     	(u_char *, long *, unsigned short *);
int	checkpass               (u_char *);
int	command_shell           (u_char *);
int	read_from_channel       (int , u_char *);
int	write_to_channel        (int , u_char *, int);
int	send_msg                (Channel *, char *, ...);
int	diff_time               (long, long);

u_char	getlastchar             (u_char *);

