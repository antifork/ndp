/* 
 *  $Id$
 *  %ndp command.h 
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

typedef struct _item_
{
      char        *c_name;
      int        (*c_func) (int, char **, char *);
      int	  perm;
      char	  *descr;
}
item;

/*
*/
void            welcome         (void);

int             c_help          (int ,char **, char*);
int             c_who           (int ,char **, char*);
int             c_link          (int ,char **, char*);
int             c_msg           (int ,char **, char*);
int             c_kill          (int ,char **, char*);
int		c_exit		(int, char **, char*);
int		c_ifbrowser     (int, char **, char*);

int             get_class       	(int);
int             command_shell   	(char *);
void            set_nonblock 		(int);
int             send_msg        	(Channel *, char *, ...);
void            rehash_time 		(void);
int             get_class		(int );
void            shutdown_       	(Channel *);
int             parse_conf 		(char *, char **);
int             ndp_strcmp 		(char *, char *, int);
void            create_dialer_sock	(Channel *,long,long,int);

