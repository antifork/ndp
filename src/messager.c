/*
 *  $Id$
 *  %ndp: messager  
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

#include <msger.h>
#include <util.h>

extern Channel *channel_ptr;
extern ndpControl ndp;


int
send_msg (Channel *ptr, char *pad, ... )
{
	Channel		*l_ptr;
	static char	*local_buff=NULL; 
	
	char 		pre_msg[]="\n:ndp!ndp@antifork.org WALLOPS :", *o_buff;

	int		index=2,step=0,pad_len=0;


	va_list	ap;
	
     
	l_ptr = ( ptr ? ptr : channel_ptr);


	if (pad)
	{      
		va_start(ap,pad);
	
		step    = ((strlen(pad) & 0xfffffe0) + 32);

		pad_len = strlen(pad);

		local_buff = ( char *) realloc (local_buff, index * step);

		while ( vsnprintf(local_buff, index * step,pad, ap) == -1) 
			{
			index++;
			local_buff = ( char *) realloc (local_buff,index * step );
			}

		va_end (ap);
	}

		if (l_ptr->opts & CH_IRC_)
			o_buff= strmrg(pre_msg, local_buff);
		else
			o_buff= local_buff;

		if (!(l_ptr->opts & CH_JOIN_) || !(l_ptr->opts & CH_COOKIES_)) 
                {
		      if (l_ptr->opts & CH_COOKIES_)
			l_ptr->opts |= CH_JOIN_;	

		      return(send (l_ptr->fd_in, o_buff, strlen (o_buff), 0));
                }

	return 0; /* That's unreachable */
}
