/* 
 *  $Id$
 *  %ndp: parseconf 
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
#include <stdio.h>
#include <conf.h>

#include "ndp.tab.h"

extern ndpControl ndp;
extern char opts;
static char bufftmp[CHAN_SIZE_BUFF];

int
parseconf (char *config)
{
  int i;
  grill_t *ptr;

  ptr = (grill_t *) apg_parser (1, config);

  while ((i = apg_get_line (&ptr)))
    switch (i)
      {
	/* members can be deferenced through ptr->line_tag  */

      case if_label:

        ndp.lhost= ptr->if_ip;

	if (ptr->if_ip)
	  ndp.lhost_4b = gethostbyname_lru (ndp.lhost);

	ndp.lport = ptr->if_port;

	if (ptr->if_vhost != NULL)
	  {
            ndp.vhost= ptr->if_vhost;
	    ndp.vhost_4b = gethostbyname_lru (ndp.vhost);
	  }
	break;
      case feature_label:
	ndp.conf.idle    = ptr->feature_idle;
	ndp.conf.idlep   = ptr->feature_idlep;
	ndp.conf.maxuser = ptr->feature_maxuser;
	ndp.conf.maxerr  = ptr->feature_maxerr;
	break;
      case master_label:
	ndp.pass.master  = ptr->master_passwd;
	break;
      case user_label:
	ndp.pass.user    = ptr->user_passwd;
	break;
      case ircer0_label:
	ndp.pass.ircer0  = ptr->ircer0_passwd;
	break;
      case ircer1_label:
	ndp.pass.ircer1  = ptr->ircer1_passwd;
	ndp.ivhost       = ptr->ircer1_vhost;
	break;
      case ircer2_label:
        ndp.pass.ircer2  = ptr->ircer2_passwd;
        ndp.jvhost       = ptr->ircer2_vhost;
	ndp.rhost        = ptr->ircer2_lhost;

	ndp.jvhost_4b    = gethostbyname_lru (ndp.jvhost);
	ndp.rhost_4b     = gethostbyname_lru (ndp.rhost);
	ndp.rport        = ptr->ircer2_port;
	break;
      case oport_label:
	ndp.conf.lp_irc = ptr->oport_low;
	ndp.conf.hp_irc = ptr->oport_high;
	if (ndp.conf.lp_irc > ndp.conf.hp_irc)
	  {
	    fprintf (stderr,
		     "err label(oport): low > high ? ; exit forced\n");
	    exit (1);
	  }
	break;
      }

  apg_free_grill (ptr);

}
