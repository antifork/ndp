/* 
 *  $Id$
 *  %ndp: shell's command  
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

#include <command.h>
#include <util.h>

extern ndpControl ndp;
extern Channel *channel_ptr;
extern Channel *first_channel;

extern char *class[];
extern char opts;


int
get_class (int i)
{
      register short int pheer = 0;

      while (i >>= 0x01)
	    pheer++;
      return pheer;

}


void
welcome (void)
{
      char       *ip;

      ip = multi_inet_nbotoa (channel_ptr->caller_addr.sin_addr.s_addr);

      if (ndp.log_level & LOG_LEVEL2)
	    syslog (LOG_INFO, "%s logged from %s with gecos irc %s!%s",
		    class[get_class (channel_ptr->class)],
		    (char *) ip,
		    (*(char *) (channel_ptr->usernick) ? (char *) channel_ptr->usernick : "unknown"),
		    (*(char *) (channel_ptr->username) ? (char *) channel_ptr->username : "unknown")
		  );

      send_msg (NULL, MSG_MOTD_);
      send_msg (NULL, "%s logged from %s\n",class[get_class (channel_ptr->class)], (char *) ip);

      return;

}


int
c_link (int _argc, char **_argv, char *null)
{

      unsigned long rhost_dyn = 0, vhost_dyn = 0;
      unsigned short rport_dyn = 0;

      if ((_argc < 3) || !(rhost_dyn = getlongbyname (_argv[1])))
	    return -1;

      if ((rport_dyn = (u_short) atoi (_argv[2])) > 0)

	    switch (channel_ptr->class)
	      {
	      case CL_IRC1:
	      case CL_IRC2:
	      case CL_IRC3:
		    if ((rport_dyn < ndp.conf.lp_irc) || (rport_dyn > ndp.conf.hp_irc))
		      {
			    send_msg (NULL, "ndp: you are not allowed to connect that port\n");
			    return 0;
		      }
		    break;

	      }
      else
	    return -1;

      if (_argv[3] != NULL)
	    switch (channel_ptr->class)
	      {
	      case CL_IRC2:
	      case CL_IRC3:
		    send_msg (NULL, "ndp: you are not allowed to bind interfaces\n");
		    break;

	      default:
		    if (!(vhost_dyn = getlongbyname (_argv[3])))
			  return -1;
		    break;
	      }

      if (!vhost_dyn)
	    switch (channel_ptr->class)
	      {
	      case CL_MASTER:
	      case CL_USER:
	      case CL_IRC1:
		    if (ndp.vhost_4b)
			  vhost_dyn = ndp.vhost_4b;
		    else
			  vhost_dyn = INADDR_ANY;
		    break;
	      case CL_IRC2:
		    if (ndp.ivhost_4b)
			  vhost_dyn = ndp.ivhost_4b;
		    else
			  vhost_dyn = INADDR_ANY;
		    break;
	      case CL_IRC3:
		    vhost_dyn = ndp.jvhost_4b;
		    break;
	      }

      create_dialer_sock (channel_ptr, vhost_dyn, rhost_dyn, rport_dyn);

      send_msg (NULL,MSG_CONNECTING_);
      rehash_time ();

      return 1;
}

int
c_ifbrowser(int _argc, char **_argv, char *null)
{

        char            buffer[10240];
        int             sd;
        struct ifreq   *ifr, *iflast;
        struct ifconf   ifc;
        struct sockaddr_in *ptr_if;

        memset(buffer, 0, 10240);

        /* dummy dgram socket for ioctl */

        if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
                send_msg(NULL, "%s!\n", strerror(errno));
                return 1;
        }

        ifc.ifc_len = sizeof(buffer);
        ifc.ifc_buf = buffer;

        /* getting ifs: this fills ifconf structure. */

        if (ioctl(sd, SIOCGIFCONF, &ifc) < 0) {
                send_msg(NULL, "%s\n", strerror(errno));
                return 1;
        }
        close(sd);

        /* line_up ifreq structure */

        ifr = (struct ifreq *) buffer;
        iflast = (struct ifreq *) ((char *) buffer + ifc.ifc_len);

        if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
                send_msg(NULL, "%s!\n", strerror(errno));
                return 1;
        }
#if HAVE_SOCKADDR_SALEN
        for (; ifr < iflast;
             (char *) ifr += sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len)
#else
        for (; ifr < iflast;
         (char *) ifr += sizeof(ifr->ifr_name) + sizeof(struct sockaddr_in))
#endif
        {
                if (*(char *) ifr) {
                        ptr_if = (struct sockaddr_in *) & ifr->ifr_addr;
                        send_msg (NULL,"%s\t%s(%s)\n", ifr->ifr_name,
                               getnamebynbo(ptr_if->sin_addr.s_addr),multi_inet_nbotoa(ptr_if->sin_addr.s_addr));

                        }

        }

        close(sd);
        return 1;
}

#define cnz(x) ((x > 1) ? 1 : 0)

int
c_who (int _argc, char **_argv, char *null)
{
      char       *cl[] = {"M", "U", "I", "i", "j", "^"}, *sip, *rip, null_ip[] = "-", me = '*', other = ' ';

      register short int ct = 1, tfd = 0;
      unsigned short rpt = 0;

      Channel    *ptr;

      ptr = first_channel;

      send_msg (NULL, "CL  FD  FROM            TO             port  gecos\n");

      while (ptr != NULL)
	{
	      if (ptr->flag)
		{
		      sip = multi_inet_nbotoa (ptr->caller_addr.sin_addr.s_addr);

		      if (ptr->flag & _ESTABLISHED_)
			{
			      rip = multi_inet_nbotoa (ptr->output_addr.sin_addr.s_addr);
			      rpt = ntohs (ptr->output_addr.sin_port);
			}
		      else
			{
			      rip = (char *) null_ip;
			      rpt = 0;
			}

		      send_msg (NULL, "%s%c  %2d  %-15.15s %-15.15s%-5d %.25s...\n",
			       cl[get_class (ptr->class)],
			       ((ptr == channel_ptr) ? me : other),
			       ptr->fd_in,
			       sip,
			       rip,
			       rpt,
			       ptr->username);

		      tfd += (cnz (ptr->fd_in) + cnz (ptr->fd_out));

		      ct++;
		}

	      ptr = ptr->next;
	}

      send_msg (NULL, "users %d of %d, total fds:%d\n", ct - 1, ndp.conf.n, tfd);

      return 1;

}

int
c_msg (int _argc, char **_argv, char *input)
{
      register Channel *ptr;

      if ( (_argc < 3) || !Isdigit (_argv[1]) )
	    return -1;

      ptr = first_channel;

      while ((*input != 0x00) && (*input == 0x20)) input++;
      input += 3;

      while ((*input != 0x00) && (*input == 0x20)) input++;
      while ((*input != 0x00) && (*input != 0x20)) input++;
      while ((*input != 0x00) && (*input == 0x20)) input++;

      {
	    register short int tg = 0;

	    tg = atoi (_argv[1]);

	    while ((ptr->next != NULL) && (ptr->fd_in != tg))
		  ptr = ptr->next;

	    if (ptr->next != NULL)
		    {
		    send_msg (ptr, "%s\n",input);
		    return 1;
		    }

	    return -1;

      }

}

int
c_kill (int _argc, char **_argv, char *null)
{
      register Channel *ptr;

      if ( (_argc < 2) || !Isdigit (_argv[1]) )
	    return -1;

      {
	    register short int ct = 0;

	    ct = atoi (_argv[1]);
	    ptr = first_channel;

	    while ((ptr->next != NULL) && (ptr->fd_in != ct))
		  ptr = ptr->next;

	    if (ptr->next != NULL)
	      {
		    send_msg (ptr, "Closing link requested\n");
		    shutdown_ (ptr);
		    return 1;
	      }

	    else
		  return -1;
      }
}

int
c_exit (int _argc, char **_argv, char *null)
{

      send_msg  (channel_ptr, "logout\n");
      shutdown_ (channel_ptr);
      return 1;

}

item command[] = {
 {"w"         , c_who,      CL_MASTER|CL_USER , 		"w,who                  - show users and filedescriptors" },
 {"iflist"    , c_ifbrowser,CL_MASTER|CL_USER|CL_IRC1 ,         "iflist                 - show interfaces list" },
 {"kill"      , c_kill,     CL_MASTER,                          "kill <fd>              - reset <fd> connection" },
 {"msg"       , c_msg,      CL_MASTER|CL_USER , 		"msg  <fd> message      - send a message to <fd>" },
 {"link"      , c_link,     CL_MASTER|CL_USER|CL_IRC1| CL_IRC2, "link host:port:[vhost] - connect remote host" },
 {"exit"      , c_exit,     CL_MASTER|CL_USER|CL_IRC1|CL_IRC2, 	"exit,logout            - close link" },
 {"help"      , c_help,     CL_MASTER|CL_USER|CL_IRC1|CL_IRC2, 	"help                   - print this help" },

 /* alias */

 {"who"       , c_who,      CL_MASTER|CL_USER ,                 NULL },
 {"send"      , c_msg,      CL_MASTER|CL_USER ,                 NULL },
 {"connect"   , c_link,     CL_MASTER|CL_USER|CL_IRC1| CL_IRC2, NULL },
 {"logout"    , c_exit,     CL_MASTER|CL_USER|CL_IRC1|CL_IRC2,  NULL },

 {NULL, NULL, 0, 0 }};

int
c_help (int _argc, char **_argv, char *null)
{
register int i=0;

	while( command[i].descr != NULL )
		{ if (channel_ptr->class & command[i].perm)
              		send_msg (NULL,"%s\n",command[i].descr);
		i++;
		}
      return 1;
}


int
command_shell (char *input)
{
      char       *argz[_MAXARGLINE_];
      char       *s_input;
      short       n = 0, i = 0;

      s_input = (char *) alloca (strlen (input) + 1);

      memset (s_input, 0, strlen (input) + 1);
      memcpy (s_input, input, strlen (input));

      /* Firstaid -> Guessing what kind of client it is.  */

      while ((*input == 0x20)) input++;

      if (*input == 0x00) return -1;

      n = parse_conf (input, argz);

      if (!n) return -1;

      while (command[i].c_name != NULL)
	{
	      if (!strcasecmp (argz[0], command[i].c_name))
		    break;
	      i++;
	}

      /* syntax error ? */

      if ( command[i].c_func == NULL)
		return -1;

      /* Permission */

      if (! (channel_ptr->class & command[i].perm ) )
		return -1;

      /* Command execution */

      return (*command[i].c_func) (n, argz, s_input);

}
