/*
 *  $Id$ 
 *  %ndp ndp.h 
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

#include "version.h"

#define  PIDFILE        "ndp.pid"

#define  free_time      (usleep(10000))

#define _MAXARGLINE_    30


#define _TIMEOUT_		1200	/* max-idle time (sec) */
#define _PASSTIMEOUT_   	60	/* max-idle time in login */
#define _TIMEOUTCONN_   	30
#define _MAXUSER_       	2	/* max number of user  */
#define _MAXERRONPASS_  	5

// bits for channel_ptr->flag


#define _NULL_			0xff	/* keep the current state */

#define _READY_         	0x00	/* free channel */
#define _LOGIN_			0x01	/* password checking */
#define _SHELL_			0x02	/* waiting for a link */
#define _INPROGRESS_    	0x03	/* connection in progress */
#define _CONNECTING_		0x04	/* .. */
#define _ESTABLISHED_   	0x05	/* channel busy */

// bits for ndp.opts 

#define OPT_LOCAL_		0x01
#define OPT_REMOTE_		0x02
#define OPT_CONF_		0x04
#define OPT_LINE_		0x08	/* line by line stream's type (fixed for all channel) */
#define OPT_CHAR_		0x10
#define OPT_AUTO_		0x20

// bits for channel_ptr->opts

#define CH_SHELL_		0x0001
#define CH_IRC_         	0x0002
#define CH_PROMPT_		0x0004
#define CH_COOKIES_		0x0008
#define CH_JOIN_		0x0010
#define CH_LINE_		0x0020	/* stream: line buffered */
#define CH_CHAR_		0x0040	/* stream: no buffered   */ 
#define CH_UNKNOWN_		0x0080	/* strean: unknown       */

#define LOG_LEVEL0		0x00
#define LOG_LEVEL1		0x01
#define LOG_LEVEL2		0x02

// messages...

#define MSG_MOTD_		"Welcome to %s\n",VERSION
#define MSG_LOGIN_RAW       	"ndp's password: "
#define MSG_LOGIN_IRC		"Enter password. Type /quote pass <pass>\n"	

#define MSG_PROMPT_RAW		(channel_ptr->prompt)
#define MSG_PROMPT_IRCV		"Type /quote link host:port:<vhost> to connect\n"
#define MSG_PROMPT_IRC		"Type /quote link host:port to connect\n"

#define MSG_IDLE_        	"idle time exceed\n"	
#define MSG_MAXUSER_		"number of user exceed. Try later\n" 
#define MSG_MAXTRIAL_    	"MAXERRONPASS: bye\n"	
#define MSG_ERRPARSE_		"syntax error. retry\n"    	
#define MSG_CONNECTING_  	"connecting %s:%d ...\n",multi_inet_nbotoa (channel_ptr->output_addr.sin_addr.s_addr), \
				ntohs (channel_ptr->output_addr.sin_port)
	
#define MSG_ESTABLISHED_ 	"connection established\n"	
#define MSG_CONTIMO_     	"attempt connection reached timeout\n"	

#define IRC_LOGIN_       	0x0300
#define IRC_PROMPT_      	0x0400

// user's class

#define CL_MASTER	0x01
#define CL_USER		0x02
#define CL_IRC1		0x04
#define CL_IRC2		0x08
#define CL_IRC3		0x10
#define CL_UNKNOWN   	0x20 


