/* 
 *  $Id$
 *  %ndp: time tools 
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


#include <defs.h>
#include <std.h>
#include <channel.h>

extern Channel *channel_ptr;
extern struct timeval mytime;

int diff_time (long, long);
void rehash_time (void);

int
diff_time (long now, long old)
{
  return ((now - old) > 0 ? (now - old) : (0));
}

void
rehash_time (void)
{
/* rehash mytime: sec/usec or sec/0 due to the presence of GETTIMEOFDAY or less */

#ifdef HAVE_GETTIMEOFDAY
  gettimeofday (&mytime, NULL);
#else
  mytime.tv_sec = time (NULL);
  mytime.tv_usec = 0;
#endif

  channel_ptr->sec = mytime.tv_sec;
  return;

}
