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

	strncpy (ndp.lhost, ptr->if_ip, 79);

	if (!strcasecmp (ptr->if_ip, "ALL"))
	  ndp.lhost_4b = getlongbyname (ndp.lhost);

	ndp.lport = ptr->if_port;

	if (ptr->if_vhost != NULL)
	  {
	    strncpy (ndp.vhost, ptr->if_vhost, 79);
	    ndp.vhost_4b = getlongbyname (ndp.vhost);
	  }
	break;
      case feature_label:
	ndp.conf.T = ptr->feature_idle;
	ndp.conf.t = ptr->feature_idlep;
	ndp.conf.n = ptr->feature_maxuser;
	ndp.conf.e = ptr->feature_maxerr;
	break;
      case master_label:
	strncpy ((unsigned char *) ndp.pass.M,
		 (unsigned char *) ptr->master_passwd, 40);
	break;
      case user_label:
	strncpy ((unsigned char *) ndp.pass.U,
		 (unsigned char *) ptr->user_passwd, 40);
	break;
      case ircer0_label:
	strncpy ((unsigned char *) ndp.pass.I,
		 (unsigned char *) ptr->ircer0_passwd, 40);
	break;
      case ircer1_label:
	strncpy ((unsigned char *) ndp.pass.i,
		 (unsigned char *) ptr->ircer1_passwd, 40);
	strncpy ((unsigned char *) ndp.ivhost,
		 (unsigned char *) ptr->ircer1_vhost, 79);
	break;
      case ircer2_label:
	strncpy ((unsigned char *) ndp.pass.j,
		 (unsigned char *) ptr->ircer2_passwd, 40);
	strncpy ((unsigned char *) ndp.jvhost,
		 (unsigned char *) ptr->ircer2_vhost, 79);
	strncpy ((unsigned char *) ndp.rhost,
		 (unsigned char *) ptr->ircer2_lhost, 79);
	ndp.jvhost_4b = getlongbyname (ndp.jvhost);
	ndp.rhost_4b = getlongbyname (ndp.rhost);
	ndp.rport = ptr->ircer2_port;
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
  apg_free_pragma ();

}
