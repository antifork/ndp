/* 
 *  $Id$
 *  %ndp shell.h
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
#include "std.h"
#include "channel.h"
#include "ndp.h"


/*
*/

void    set_nonblock    	(int );
void    set_block       	(int );
void    halfshutdown_   	(void);
void    shutdown_       	(Channel *);
int     read_from_channel       (int , char *);
int     write_to_channel        (int , char *, int);
void    reset_chan         	(Channel * );
int     send_msg 		(Channel *, char *, ...);