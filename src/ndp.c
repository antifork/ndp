/*
 * $Id$ %ndp- ndp is an a
 * antifork.org project.
 * 
 * Copyright (c) 1999 Bonelli Nicola <bonelli@antifork.org>
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#include        <stdlib.h>
#include 	<ndp_local.h>
#include	<version.h>
#include	<util.h>

#define ID	"$Id$"

/*
 * Global vars
 */


ctrl_t    	ndp;
chan_t      *f_chan;
chan_t      *p_chan;

struct timeval mytime;

int           n_chan;
int           fd_in;

char          bufftmp[CHAN_SBUFF];
char         *class[] = { "master", "user", "ircer+", "ircer", "ircer-", "unknown" };


int
accept_client (void)
{
    int           callersize;
    int           opt = 1;

    /*
     * SO_LINGER the system will process the close in a manner that
     * allows to continue as quickly as possible
     */

    struct linger linger_ = { 0, 0 };
    struct timeval delay = { 0, 100 };
    fd_set        r_set;

    FD_ZERO (&r_set);
    FD_SET (fd_in, &r_set);

    switch (select (FD_SETSIZE, &r_set, NULL, NULL, &delay))
	{
	 case 0:
	     return S_READY;
	     break;
	 case -1:
	     shutdown_ (NULL);
	     return S_READY;
	     break;
	}


    if (n_chan < (ndp.conf.maxuser + 1))
	{

	    callersize = sizeof (p_chan->c_addr);

	    p_chan->fd_in = accept (fd_in, (struct sockaddr *) &(p_chan->c_addr), &callersize);

	    if (p_chan->fd_in > 0)
		{

		    setsockopt (p_chan->fd_in, SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof (linger_));
		    setsockopt (p_chan->fd_in, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));
		    if (ndp.opts & OPT_CONF)
			{
			    {
				struct passwd *pwd;

				pwd = getpwuid (getuid ());

				memset (p_chan->prompt, 0, sizeof (p_chan->prompt));
				snprintf (p_chan->prompt, CHAN_SPRPT - 1, "%s-(fd%d)# ", pwd->pw_name, p_chan->fd_in);
				rehash_time ();
				return S_LOGIN;
			    }

			}
		    else
			{
			    rehash_time ();
			    return S_INPRGR;
			}
		}
	    else
		{
		    shutdown_ (NULL);
		    return S_READY;
		}

	}
    return S_NULL;
}


int
ushell (void)
{

    struct timeval delay = { 0, 100 };
    char          last_char = 0;
    fd_set        r_set,
                  w_set;

    FD_ZERO (&r_set);
    FD_ZERO (&w_set);

    FD_SET (p_chan->fd_in, &w_set);
    FD_SET (p_chan->fd_in, &r_set);

    switch (select (FD_SETSIZE, &r_set, &w_set, NULL, &delay))
	{
	 case -1:
	     /* select error */
	     shutdown_ (NULL);
	     return S_READY;
	     break;
	 case 0:
	     /* Nothing to do */
	     return S_NULL;
	     break;

	 default:
	     /* Check MAXUSER exception */

	     if ((p_chan->state & S_SHELL) && (n_chan > (ndp.conf.maxuser + 1)) && !(p_chan->class & CL_MASTER))
		 {
		     send_msg (NULL, MSG_MAXUSER);
		     shutdown_ (NULL);
		     return S_READY;

		 }
	     /* Check MAXTRIAL exception */

	     if ((p_chan->trial >= ndp.conf.maxerr) && (p_chan->state & S_LOGIN))
		 {
		     send_msg (NULL, MSG_MAXTRIAL);

		     if (ndp.log_level)
			 {
			     if (ndp.log_level)
				 syslog (LOG_INFO,
					 "possible intruder from:%s with gecos irc %s!%s",
					 (char *) multi_inet_nbotoa (p_chan->
								     c_addr.sin_addr.
								     s_addr),
					 (*(char *) (p_chan->nick) ? (char *) p_chan->nick : "unknown"), (*(char *) (p_chan->name) ? (char *) p_chan->name : "unknown"));
			 }
		     shutdown_ (NULL);
		     return S_READY;
		 }

	     /* Send msg login or prompt */

	     if (!(p_chan->opts & CH_PROMPT))
		 {

		     p_chan->opts |= CH_PROMPT;

		     if (p_chan->state & S_LOGIN)
			 {
			     if (p_chan->opts & CH_IRC)
				 send_msg (NULL, MSG_LOGIN_IRC);
			     else
				 send_msg (NULL, MSG_LOGIN_RAW);
			 }
		     else
			 {

			     if (p_chan->opts & CH_IRC)
				 {
				     if (p_chan->class & (CL_MASTER | CL_USER | CL_IRC1))
					 send_msg (NULL, MSG_PROMPT_IRCV);
				     else
					 send_msg (NULL, MSG_PROMPT_IRC);

				 }
			     else
				 send_msg (NULL, MSG_PROMPT_RAW);

			 }
		 }

	     /* Watch to see if characters become available for reading */

	     if (FD_ISSET (p_chan->fd_in, &r_set))
		 {
		     rehash_time ();

		     if (read_chan (p_chan->fd_in, p_chan->bufin) == -1)
			 return S_NULL;

		     irc_controller (p_chan->bufin);

		     last_char = getlastchar (p_chan->bufin);

		     if ((last_char == 0x06) && !(p_chan->opts & CH_IRC))
			 {
			     /* sign_break came in telnet only */
			     shutdown_ (NULL);
			     return S_READY;
			 }
		     if ((last_char == 0x0a))
			 {

			     p_chan->opts &= ~CH_PROMPT;

			     void_cr_lf (p_chan->bufin, 0);

			     if (p_chan->state & S_LOGIN)
				 {

				     p_chan->class = (checkpass (p_chan->bufin));

				     if (p_chan->class & ~CL_UNKNOWN)
					 {

					     if ((ndp.opts & OPT_CONF) && !(p_chan->class & CL_IRC3))
						 {
						     memset (p_chan->bufin, 0, CHAN_SBUFF);
						     welcome ();
						     return S_SHELL;
						 }
					     else
						 {
						     memset (p_chan->bufin, 0, CHAN_SBUFF);
						     return S_INPRGR;

						 }
					 }
				     else
					 {
					     (p_chan->trial)++;
					     memset (p_chan->bufin, 0, CHAN_SBUFF);
					     return S_LOGIN;
					 }
				 }
			     else
				 {
				     if (command_shell (p_chan->bufin) >= 0)
					 return S_NULL;
				     else
					 {
					     if (strlen (p_chan->bufin))
						 send_msg (NULL, MSG_ERRPARSE);
					     memset (p_chan->bufin, 0, CHAN_SBUFF);
					     return S_SHELL;

					 }

				 }
			 }
		 }
	     /* Check Timeout exception */

#ifdef HAVE_GETTIMEOFDAY
	     gettimeofday (&delay, NULL);
#else
	     delay.tv_sec = time (NULL);
	     delay.tv_usec = 0;
#endif

	     if (diff_time (delay.tv_sec, p_chan->sec) > ndp.conf.idlep)
		 {
		     send_msg (NULL, MSG_IDLE);
		     shutdown_ (NULL);
		     return S_READY;
		 }
	     return S_NULL;

	     break;
	}

}


void
push_buffer_cache (char *segment)
{
    if (segment)
	{
	    register int  seglen = strlen (segment);
	    char         *ptr;


	    if (p_chan->tmp == NULL)
		{
		    p_chan->tmp_size = ((seglen + p_chan->tmp_size + 1) | 0xff);
		    p_chan->tmp = (u_char *) calloc (p_chan->tmp_size, 1);
		}
	    else
		{

		    if ((strlen (p_chan->tmp) + seglen + 1) > p_chan->tmp_size)
			{

			    p_chan->tmp_size = ((seglen + p_chan->tmp_size + 1) | 0xff);
			    ptr = strdup (p_chan->tmp);
			    p_chan->tmp = (u_char *) realloc (p_chan->tmp, p_chan->tmp_size);
			    memcpy (p_chan->tmp, ptr, strlen (ptr) + 1);
			    free (ptr);
			}
		}
	    strcat (p_chan->tmp, segment);

	}
    return;
}

int
flush_buffer_cache (void)
{
    char         *ptr;

    if ((ptr = strrchr (p_chan->tmp, '\n')) != NULL)
	{
	    if (write_chan (p_chan->fd_in, p_chan->tmp, (int) ((u_char *) ptr - (u_char *) p_chan->tmp) + 1) == -1)
		return S_READY;

	    memmove (p_chan->tmp, ptr + 1, strlen (ptr) + 1);
	}
    return S_NULL;
}

int
handle_link (void)
{
    struct timeval delay = { 0, 100 };
    int           bt;

    fd_set        r_set,
                  w_set;


    FD_ZERO (&r_set);
    FD_ZERO (&w_set);

    FD_SET (p_chan->fd_in, &w_set);
    FD_SET (p_chan->fd_in, &r_set);
    FD_SET (p_chan->fd_out, &r_set);
    FD_SET (p_chan->fd_out, &w_set);


    switch (select (FD_SETSIZE, &r_set, &w_set, NULL, &delay))
	{
	 case 0:
	     return S_ESTABL;
	     break;
	 case -1:
	     shutdown_ (NULL);
	     return S_READY;
	     break;
	}

    memset (p_chan->bufin, 0, CHAN_SBUFF);
    memset (p_chan->bufout, 0, CHAN_SBUFF);


    if (FD_ISSET (p_chan->fd_out, &r_set) && FD_ISSET (p_chan->fd_in, &w_set))
	{

	    rehash_time ();

	    if (!(p_chan->opts & (CH_IRC | CH_LINE)) && !(ndp.opts & OPT_LINE))
		{

		    /* no-buffered client (raw-telnet) */

		    if ((bt = read_chan (p_chan->fd_out, p_chan->bufin)) == -1)
			return S_READY;

		    void_cr_lf (p_chan->bufin, 1);

		    if (write_chan (p_chan->fd_in, p_chan->bufin, bt) == -1)
			return S_READY;

		}
	    else
		{

		    /* line-buffered client (irc) */

		    if ((bt = read_chan (p_chan->fd_out, p_chan->bufin)) == -1)
			return S_READY;

		    push_buffer_cache (p_chan->bufin);

		    if (flush_buffer_cache () == S_READY)
			return S_READY;

		}

	}
    if (FD_ISSET (p_chan->fd_in, &r_set) && FD_ISSET (p_chan->fd_out, &w_set))
	{

	    rehash_time ();

	    if ((bt = read_chan (p_chan->fd_in, p_chan->bufout)) == -1)
		return S_READY;

	    /*
	     * Guessing the stream's type: irc client needs a
	     * line-buffered...
	     */

	    if ((p_chan->opts & CH_UNKNOWN) && (ndp.opts & OPT_AUTO))
		stream_guesser (p_chan->bufout);

	    if (write_chan (p_chan->fd_out, p_chan->bufout, bt) == -1)
		return S_READY;

	}
#ifdef HAVE_GETTIMEOFDAY
    gettimeofday (&delay, NULL);
#else
    delay.tv_sec = time (NULL);
    delay.tv_usec = 0;
#endif
    if (diff_time (delay.tv_sec, p_chan->sec) > ndp.conf.idle)
	{
	    send_msg (NULL, MSG_IDLE);
	    shutdown_ (NULL);
	    return S_READY;
	}
    return S_NULL;

}


int
setup_dialer (void)
{
    if (!(ndp.opts & OPT_CONF))
	{

	    if (ndp.nbo_vhost)
		create_dialer_sock (p_chan, ndp.nbo_vhost, ndp.nbo_rhost, ndp.rport);
	    else
		create_dialer_sock (p_chan, INADDR_ANY, ndp.nbo_rhost, ndp.rport);
	}
    if (p_chan->class & CL_IRC3)
	create_dialer_sock (p_chan, ndp.nbo_vhost, ndp.nbo_rhost, ndp.rport);

    rehash_time ();

    return S_CONN;

}


int
connect_target (void)
{
    int           connect_out;
    int           opt = 1;
    struct timeval now = { 0, 0 };

    /*
     * SO_LINGER the system will process the close in a manner that
     * allows to continue as quickly as possible
     */

    struct linger linger_ = { 0, 0 };

    connect_out = connect (p_chan->fd_out, (struct sockaddr *) &(p_chan->o_addr), sizeof (p_chan->o_addr));

#ifdef HAVE_GETTIMEOFDAY
    gettimeofday (&now, NULL);
#else
    now.tv_sec = time (NULL);
    now.tv_usec = 0;
#endif

    if (diff_time (now.tv_sec, p_chan->sec) > OPT_TIMEOCONN )
	{

	    send_msg (NULL, MSG_CONTIMO);

	    if ((p_chan->class == CL_IRC3) || (ndp.opts & (OPT_LOCAL | OPT_REMOTE)))
		{
		    shutdown_ (NULL);
		    return S_READY;
		}
	    else
		{
		    halfshutdown_ (NULL);
		    return S_SHELL;
		}

	}
    if (connect_out == 0)
	{

	    set_block (p_chan->fd_out);

	    setsockopt (p_chan->fd_out, SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof (linger_));
	    setsockopt (p_chan->fd_out, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));

	    if (ndp.opts & OPT_CONF)
		send_msg (NULL, MSG_ESTABLISHED);

	    if (ndp.log_level & LOG_LEVEL2)
		syslog (LOG_INFO, "%s[%s] linked to %s:%d",
			class[get_class (p_chan->class)],
			multi_inet_nbotoa (p_chan->c_addr.sin_addr.s_addr), multi_inet_nbotoa (p_chan->o_addr.sin_addr.s_addr), ntohs (p_chan->o_addr.sin_port));

	    if (p_chan->opts & CH_IRC)
		if (irc_postlogin () <= 0)
		    return S_READY;

	    rehash_time ();

	    return S_ESTABL;
	}
    switch (errno)
	{

	 case EINPROGRESS:
	 case EALREADY:
	     return S_CONN;
	     break;

	 case EISCONN:
	     setsockopt (p_chan->fd_out, SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof (linger_));

	     set_block (p_chan->fd_out);
	     setsockopt (p_chan->fd_out, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));

	     if (ndp.opts & OPT_CONF)
		 send_msg (NULL, MSG_ESTABLISHED);

	     if (ndp.log_level & LOG_LEVEL2)
		 syslog (LOG_INFO, "%s[%s] linked to %s:%d",
			 class[get_class (p_chan->class)],
			 multi_inet_nbotoa (p_chan->c_addr.sin_addr.s_addr), multi_inet_nbotoa (p_chan->o_addr.sin_addr.s_addr), ntohs (p_chan->o_addr.sin_port));

	     if (p_chan->opts & CH_IRC)
		 if (irc_postlogin () <= 0)
		     return S_READY;

	     rehash_time ();

	     return S_ESTABL;
	     break;

	 default:

	     send_msg (NULL, "err: %s", strerror (errno));

	     if ((p_chan->class == CL_IRC3) || (ndp.opts & (OPT_LOCAL | OPT_REMOTE)))
		 {
		     shutdown_ (NULL);
		     return S_READY;
		 }
	     else
		 {
		     halfshutdown_ (NULL);
		     return S_SHELL;
		 }

	     break;
	}

}


void
reset_chan (chan_t * ptr)
{
    long          saved;

    saved = (long) ptr->next;

    memset (ptr, 0, sizeof (chan_t));

    ptr->state         = S_READY;
    ptr->fd_in         = -1;
    ptr->tmp           = NULL;
    ptr->tmp_size      = 0;
    ptr->class         = CL_UNKNOWN;
    ptr->opts         |= CH_UNKNOWN;
    ptr->next          = (chan_t *) saved;	/* This save the channel-queue */
    return;

}


void
add_chan (chan_t * ptr)
{
    ptr->next = (chan_t *) malloc (sizeof (chan_t));
    ptr = ptr->next;
    ptr->next = (chan_t *) NULL;
    reset_chan (ptr);

    return;

}


void
server (void)
{
    int           opt = 1;
    struct sockaddr_in locale_addr;

    /*
     * SO_LINGER the system will process the close in a manner that
     * allows to continue as quickly as possible
     */

    struct linger linger_ = { 0, 0 };

    int           (*state_action[]) (void) =
    {
    accept_client, ushell, ushell, setup_dialer, connect_target, handle_link};


    f_chan = p_chan = (chan_t *) malloc (sizeof (chan_t));
    reset_chan (p_chan);

    openlog ("ndp", LOG_CONS | LOG_PID, LOG_LOCAL1);


    if ((fd_in = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
	    if (ndp.log_level)
		syslog (LOG_ERR, "Cannot create a socket:%s", strerror (errno));
	    exit (-1);
	}
    memset (&locale_addr, 0, sizeof (locale_addr));

    locale_addr.sin_family = AF_INET;
    locale_addr.sin_port = htons ((u_short) ndp.lport);

    if (!(ndp.nbo_lhost))
	locale_addr.sin_addr.s_addr = INADDR_ANY;
    else
	locale_addr.sin_addr.s_addr = ndp.nbo_lhost;

    setsockopt (fd_in, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));	/* jwk@lords.com */
    setsockopt (fd_in, SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof (linger_));


    if (bind (fd_in, (struct sockaddr *) &(locale_addr), sizeof (locale_addr)) == -1)
	{
	    fprintf (stderr, "bind err:%s. Choose another local port\n", strerror (errno));

	    if (ndp.log_level)
		syslog (LOG_ERR, "bind err:%s. Choose another local port", strerror (errno));
	    exit (-1);
	}
    if (listen (fd_in, 1))
	{
	    if (ndp.log_level)
		syslog (LOG_ERR, "listen err:%s while setting up the main socket.", strerror (errno));
	    exit (-1);
	}
    syslog (LOG_INFO, "started.");

    {

#define        free_chan        0x01

	register int  ready = 0,
	              tmp_state = 0;

	chan_t       *ptr_1 = NULL,
	             *ptr_save = NULL;

	p_chan = (chan_t *) NULL;

	for (;;)
	    {
		/* reset cycle */

		if (p_chan == NULL)
		    {
			ptr_1 = (chan_t *) NULL;
			p_chan = f_chan;
			ready = 0;
		    }
		if (p_chan->state == S_READY)
		    {

			if (ready & free_chan)
			    {
				ptr_save = p_chan;
				p_chan = p_chan->next;
				ptr_1->next = p_chan;
				free (ptr_save);

			    }
			else
			    ready |= free_chan;

		    }
		/* State action */

		if (p_chan != NULL)
		    {
			if ((tmp_state = (*state_action[p_chan->state]) ()) != S_NULL)
			    p_chan->state = tmp_state;

			ptr_1 = p_chan;
			p_chan = p_chan->next;

		    }
		if ((p_chan == NULL) && !(ready & free_chan))
		    {
			add_chan (ptr_1);
			p_chan = ptr_1->next;
		    }

		usleep(OPT_FREETIME);

	    }
    }

    exit (0);
}

void
_quit (int no)
{
    chan_t      *ptr = f_chan;

    while (ptr)
	{

	    switch (ptr->state)
		{
		 case S_ESTABL:

		     shutdown (ptr->fd_in, 2);
		     shutdown (ptr->fd_out, 2);

		     close (ptr->fd_in);
		     close (ptr->fd_out);

		     break;
		 default:
		     shutdown (ptr->fd_in, 2);
		     close (ptr->fd_in);
		     break;

		}
	    ptr = ptr->next;

	}

    shutdown (fd_in, 2);
    close (fd_in);

    fprintf (stderr, "done.\n");
    syslog (LOG_INFO, "killed with HUP signal.");
    exit (0);
}


int
main (int argc, char *argv[])
{
    char          CONF[80];

    int           pid_child = 0,
                  c = 0;
    static char  *exec_name;
    FILE         *pidf;

    static struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{"line", no_argument, NULL, 'l'},

	{"conf", required_argument, NULL, 'c'},
	{"local", required_argument, NULL, 'L'},
	{"remote", required_argument, NULL, 'R'},
	{"virtual", required_argument, NULL, 'V'},
	{0, 0, 0, 0}
    };


    /*
     * standard value
     */

    memset (CONF, 0, sizeof (CONF));
    memset (&ndp, 0, sizeof (ndp));

    ndp.opts        |= OPT_AUTO;
    ndp.conf.idle    = OPT_TIMEO;
    ndp.conf.idlep   = OPT_PTIMEO;
    ndp.conf.maxuser = OPT_MUSER;
    ndp.conf.maxerr  = OPT_MERRPASS;

    ndp.log_level = 2;

    exec_name = argv[0];

    if (argc == 1)
	usage (argv[0], VERSION, ID);


    while ((c = getopt_long (argc, argv, "hs:l:vc:L:R:V:", long_options, (int *) 0)) != EOF)
	switch (c)
	    {

	     case 'h':
		 usage (exec_name, VERSION, ID);
		 break;
	     case 'c':
		 ndp.opts |= OPT_CONF;
		 strncpy (CONF, optarg, 79);
		 break;
	     case 'l':
		 switch (*optarg)
		     {
		      case '0':
			  ndp.log_level = 0;
			  break;
		      case '1':
			  ndp.log_level = LOG_LEVEL1;
			  break;
		      case '2':
			  ndp.log_level = LOG_LEVEL2;
			  break;
		      default:
			  fatalerr ("err log level: -l[0-2] only");
			  break;
		     }
		 break;
	     case 's':
		 switch (*optarg)
		     {
		      case 'l':
			  ndp.opts &= ~OPT_AUTO;
			  ndp.opts &= ~OPT_CHAR;
			  ndp.opts |= OPT_LINE;
			  break;
		      case 'c':
			  ndp.opts &= ~OPT_AUTO;
			  ndp.opts &= ~OPT_LINE;
			  ndp.opts |= OPT_CHAR;
			  break;
		      default:
			  fatalerr ("err stream type: -sl or -sc olny");
			  break;
		     }

		 break;

	     case 'v':
		 fprintf (stderr, "%s %s\n", VERSION, ID);
		 exit (0);
		 break;

	     case 'L':
		 ndp.opts |= OPT_LOCAL;
		 if (parse_input (optarg, &ndp.nbo_lhost, &ndp.lport) == -1)
		     {
			 fprintf (stderr, "parse error! %s ?\nType ndp -h to get help.\n", optarg);
			 exit (-1);
		     }
		 ndp.lhost = strdup (optarg);
		 break;

	     case 'R':
		 ndp.opts |= OPT_REMOTE;
		 if (parse_input (optarg, &ndp.nbo_rhost, &ndp.rport) == -1)
		     {
			 fprintf (stderr, "parse error! %s ?\nType ndp -h to get help.\n", optarg);
			 exit (-1);

		     }
		 ndp.rhost = strdup (optarg);
		 break;

	     case 'V':
		 ndp.vhost = strdup (optarg);
		 ndp.nbo_vhost = gethostbyname_lru (ndp.vhost);
		 break;

	    }

    if ((ndp.opts & OPT_CONF) && (ndp.opts & (OPT_LOCAL | OPT_REMOTE)))
	    fatalerr ("ndp: -L-R and -c are mutually exclusive and cannot be used at the same time");

    if ((~ndp.opts | ~OPT_CONF) >> 2 & (ndp.opts & OPT_REMOTE) >> 1 & (~ndp.opts | ~OPT_LOCAL) & 0x01)
	    fatalerr ("ndp: -L option is required with -R");

    if ((~ndp.opts | ~OPT_CONF) >> 2 & (~ndp.opts | ~OPT_REMOTE) >> 1 & (ndp.opts & OPT_LOCAL) & 0x01)
	    fatalerr ("ndp: -R option is required with -L");

    if (!(ndp.opts & OPT_CONF) && !(ndp.opts & (OPT_LOCAL | OPT_REMOTE)))
	    fatalerr ("ndp: -L-R or -c options are required");

    signal (SIGHUP, _quit);

    signal (SIGSEGV, SIG_IGN);
    signal (SIGPIPE, SIG_IGN);	/* write/send to a broken pipe terminates the process */
    signal (SIGURG,  SIG_IGN);

    if (ndp.opts & OPT_CONF)
	parseconf (CONF);

    switch (pid_child = fork ())
	{

	 case -1:
	     fprintf (stderr, "fork() err:\n");
	     exit (-1);
	 case 0:
	     nice (19);
	     setsid ();
	     server ();
	     break;
	 default:
	     pidf = fopen (PIDFILE, "w");
	     fprintf (pidf, "%d\n", pid_child);
	     fclose (pidf);

	     fprintf (stderr, "\n--(%s on %s\n--(%s %s\n\n", VERSION, CPU_MFR_OPSYS, ID, MAIL_SUPPORT);
	     fprintf (stderr, "Server            : %s:%d\n", (ndp.lhost ? ndp.lhost : "INADDR_ANY"), ndp.lport);
	     fprintf (stderr, "Vhost             : %s\n", (ndp.vhost ? ndp.vhost : getenv ("HOSTNAME")));
	     fprintf (stderr, "Target            : %s:%d\n", (ndp.rhost ? ndp.rhost : "INTERACTIVE"), ndp.rport);
	     fprintf (stderr, "Password          : %s\n", ((ndp.opts & OPT_CONF) ? "on" : "off"));

	     if (ndp.opts & OPT_AUTO)
		 fprintf (stderr, "Stream-type       : auto\n");
	     else
		 fprintf (stderr, "Stream-type       : %s\n", ((ndp.opts & OPT_LINE) ? "line" : "char"));

	     fprintf (stderr, "Idle-time         : %d\n", ndp.conf.idle);
	     if (ndp.opts & OPT_CONF)
		 {
		     fprintf (stderr, "pass-idle         : %d\n", ndp.conf.idlep);
		     fprintf (stderr, "Maxuser           : %d\n", ndp.conf.maxuser);
		     fprintf (stderr, "masstrial pass    : %d\n", ndp.conf.maxerr);
		     fprintf (stderr, "Classes loaded    : %s%s%s%s%s\n",
			      ((ndp.pass.master) ? "master " : ""),
			      ((ndp.pass.user) ? "user " : ""), ((ndp.pass.ircer0) ? "ircer0 " : ""), ((ndp.pass.ircer1) ? "ircer1 " : ""), ((ndp.pass.ircer2) ? "ircer2 " : ""));
		 }
	     fprintf (stderr, "Successfully forked, spawns pid (%d)\n\n", pid_child);
	     break;
	}

    exit (0);
}
