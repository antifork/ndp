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

#define PIDFILE        "ndp.pid"
#define MAXARGLINE		30

#define OPT_FREETIME      	1000
#define OPT_TIMEO		1200	/* max-idle time (sec) */
#define OPT_PTIMEO   		60	/* max-idle time in login */
#define OPT_TIMEOCONN   	30
#define OPT_MUSER       	2	/* max number of user  */
#define OPT_MERRPASS  		5

// bits for p_chan->flag


#define S_NULL			0xff	/* keep the current state */

#define S_READY         	0	/* free channel */
#define S_LOGIN			1	/* password checking */
#define S_SHELL			2	/* waiting for a link */
#define S_INPRGR    		3	/* connection in progress */
#define S_CONN			4	/* .. */
#define S_ESTABL   		5	/* channel busy */

// bits for ndp.opts 

#define OPT_LOCAL		0x01
#define OPT_REMOTE		0x02
#define OPT_CONF		0x04
#define OPT_LINE		0x08	/* line by line stream's type (fixed for all channel) */
#define OPT_CHAR		0x10
#define OPT_AUTO		0x20

// bits for p_chan->opts

#define CH_SHELL		0x0001
#define CH_IRC         		0x0002
#define CH_PROMPT		0x0004
#define CH_COOKIES		0x0008
#define CH_JOIN			0x0010
#define CH_LINE			0x0020	/* stream: line buffered */
#define CH_CHAR			0x0040	/* stream: no buffered   */ 
#define CH_UNKNOWN		0x0080	/* strean: unknown       */

#define LOG_LEVEL0		0x00
#define LOG_LEVEL1		0x01
#define LOG_LEVEL2		0x02

// messages...

#define MSG_MOTD		"Welcome to %s\n",VERSION
#define MSG_LOGIN_RAW       	"ndp's password: "
#define MSG_LOGIN_IRC		"Enter password. Type /quote pass <pass>\n"	

#define MSG_PROMPT_RAW		(p_chan->prompt)
#define MSG_PROMPT_IRCV		"Type /quote link host:port:<vhost> to connect\n"
#define MSG_PROMPT_IRC		"Type /quote link host:port to connect\n"

#define MSG_IDLE        	"idle time exceed\n"	
#define MSG_MAXUSER		"number of user exceed. Try later\n" 
#define MSG_MAXTRIAL    	"MAXERRONPASS: bye\n"	
#define MSG_ERRPARSE		"syntax error. retry\n"    	
#define MSG_CONNECTING  	"connecting %s:%d ...\n",multi_inet_nbotoa (p_chan->o_addr.sin_addr.s_addr), \
				ntohs (p_chan->o_addr.sin_port)
	
#define MSG_ESTABLISHED 	"connection established\n"	
#define MSG_CONTIMO     	"attempt connection reached timeout\n"	

#define IRC_LOGIN       	0x0300
#define IRC_PROMPT      	0x0400

// user's class

#define CL_MASTER	0x01
#define CL_USER		0x02
#define CL_IRC1		0x04
#define CL_IRC2		0x08
#define CL_IRC3		0x10
#define CL_UNKNOWN   	0x20 


