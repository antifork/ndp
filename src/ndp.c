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


ndpControl ndp;
Channel *first_channel, *channel_ptr;

struct timeval mytime;
int Nchannel, fd_in;

char bufftmp[CHAN_SIZE_BUFF], *class[] =
  { "master", "user", "ircer+", "ircer", "ircer-", "unknown" };


int
accept_client (void)
{
  int callersize;
  int opt = 1;

  /*
   * SO_LINGER the system will process the close in a manner that
   * allows to continue as quickly as possible
   */

  struct linger linger_= { 0, 0 };
  struct timeval delay = { 0, 100 };
  fd_set r_set;

  FD_ZERO (&r_set);
  FD_SET (fd_in, &r_set);

  switch (select (FD_SETSIZE, &r_set, NULL, NULL, &delay))
    {
    case 0:
      return _READY_;
      break;
    case -1:
      shutdown_ (NULL);
      return _READY_;
      break;
    }


  if (Nchannel < (ndp.conf.maxuser + 1))
    {

      callersize = sizeof (channel_ptr->caller_addr);

      channel_ptr->fd_in =
	accept (fd_in, (struct sockaddr *) &(channel_ptr->caller_addr),
		&callersize);

      if (channel_ptr->fd_in > 0)
	{

	  setsockopt (channel_ptr->fd_in, SOL_SOCKET, SO_LINGER,
		      (char *) &linger_, sizeof (linger_));
	  setsockopt (channel_ptr->fd_in, SOL_SOCKET, SO_REUSEADDR,
		      (char *) &opt, sizeof (opt));
	  if (ndp.opts & OPT_CONF_)
	    {
	      {
		struct passwd *pwd;

		pwd = getpwuid (getuid ());

		memset (channel_ptr->prompt, 0, sizeof (channel_ptr->prompt));
		snprintf (channel_ptr->prompt, CHAN_SIZE_PRPT - 1,
			  "%s-(fd%d)# ", pwd->pw_name, channel_ptr->fd_in);
		rehash_time ();
		return _LOGIN_;
	      }

	    }
	  else
	    {
	      rehash_time ();
	      return _INPROGRESS_;
	    }
	}
      else
	{
	  shutdown_ (NULL);
	  return _READY_;
	}

    }
  return _NULL_;
}


int
ushell (void)
{

  struct timeval delay = { 0, 100 };
  char last_char = 0;
  fd_set r_set, w_set;

  FD_ZERO (&r_set);
  FD_ZERO (&w_set);

  FD_SET (channel_ptr->fd_in, &w_set);
  FD_SET (channel_ptr->fd_in, &r_set);

  switch (select (FD_SETSIZE, &r_set, &w_set, NULL, &delay))
    {
    case -1:
      /* select error */
      shutdown_ (NULL);
      return _READY_;
      break;
    case 0:
      /* Nothing to do */
      return _NULL_;
      break;

    default:
      /* Check MAXUSER exception */

      if ((channel_ptr->flag & _SHELL_) && (Nchannel > (ndp.conf.maxuser + 1))
	  && !(channel_ptr->class & CL_MASTER))
	{
	  send_msg (NULL, MSG_MAXUSER_);
	  shutdown_ (NULL);
	  return _READY_;

	}
      /* Check MAXTRIAL exception */

      if ((channel_ptr->trial >= ndp.conf.maxerr)
	  && (channel_ptr->flag & _LOGIN_))
	{
	  send_msg (NULL, MSG_MAXTRIAL_);

	  if (ndp.log_level)
	    {
	      if (ndp.log_level)
		syslog (LOG_INFO,
			"possible intruder from:%s with gecos irc %s!%s",
			(char *) multi_inet_nbotoa (channel_ptr->
						    caller_addr.sin_addr.
						    s_addr),
			(*(char *) (channel_ptr->usernick) ? (char *)
			 channel_ptr->usernick : "unknown"),
			(*(char *) (channel_ptr->username) ? (char *)
			 channel_ptr->username : "unknown"));
	    }
	  shutdown_ (NULL);
	  return _READY_;
	}

      /* Send msg login or prompt */

      if (!(channel_ptr->opts & CH_PROMPT_))
	{

	  channel_ptr->opts |= CH_PROMPT_;

	  if (channel_ptr->flag & _LOGIN_)
	    {
	      if (channel_ptr->opts & CH_IRC_)
		send_msg (NULL, MSG_LOGIN_IRC);
	      else
		send_msg (NULL, MSG_LOGIN_RAW);
	    }
	  else
	    {

	      if (channel_ptr->opts & CH_IRC_)
		{
		  if (channel_ptr->class & (CL_MASTER | CL_USER | CL_IRC1))
		    send_msg (NULL, MSG_PROMPT_IRCV);
		  else
		    send_msg (NULL, MSG_PROMPT_IRC);

		}
	      else
		send_msg (NULL, MSG_PROMPT_RAW);

	    }
	}

      /* Watch to see if characters become available for reading */

      if (FD_ISSET (channel_ptr->fd_in, &r_set))
	{
	  rehash_time ();

	  if (read_from_channel (channel_ptr->fd_in, channel_ptr->buffer_in)
	      == -1)
	    return _NULL_;

	  irc_controller (channel_ptr->buffer_in);

	  last_char = getlastchar (channel_ptr->buffer_in);

	  if ((last_char == 0x06) && !(channel_ptr->opts & CH_IRC_))
	    {
	      /* sign_break came in telnet only */
	      shutdown_ (NULL);
	      return _READY_;
	    }
	  if ((last_char == 0x0a))
	    {

	      channel_ptr->opts &= ~CH_PROMPT_;
	      (void) void_cr_lf (channel_ptr->buffer_in, 0);

	      if (channel_ptr->flag & _LOGIN_)
		{

		  channel_ptr->class = (checkpass (channel_ptr->buffer_in));

		  if (channel_ptr->class & ~CL_UNKNOWN)
		    {

		      if ((ndp.opts & OPT_CONF_)
			  && !(channel_ptr->class & CL_IRC3))
			{
			  memset (channel_ptr->buffer_in, 0, CHAN_SIZE_BUFF);
			  welcome ();
			  return _SHELL_;
			}
		      else
			{
			  memset (channel_ptr->buffer_in, 0, CHAN_SIZE_BUFF);
			  return _INPROGRESS_;

			}
		    }
		  else
		    {
		      (channel_ptr->trial)++;
		      memset (channel_ptr->buffer_in, 0, CHAN_SIZE_BUFF);
		      return _LOGIN_;
		    }
		}
	      else
		{
		  if (command_shell (channel_ptr->buffer_in) >= 0)
		    return _NULL_;
		  else
		    {
		      if (strlen (channel_ptr->buffer_in))
			send_msg (NULL, MSG_ERRPARSE_);
		      memset (channel_ptr->buffer_in, 0, CHAN_SIZE_BUFF);
		      return _SHELL_;

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

      if (diff_time (delay.tv_sec, channel_ptr->sec) > ndp.conf.idlep)
	{
	  send_msg (NULL, MSG_IDLE_);
	  shutdown_ (NULL);
	  return _READY_;
	}
      return _NULL_;

      break;
    }

}


void
push_buffer_cache (char *segment)
{
  if (segment)
    {
      register int seglen = strlen (segment);
      char *ptr;


      if (channel_ptr->buff_tmp == NULL)
	{
	  channel_ptr->buff_tmp_size =
	    ((seglen + channel_ptr->buff_tmp_size + 1) | 0xff);
	  channel_ptr->buff_tmp =
	    (u_char *) calloc (channel_ptr->buff_tmp_size, 1);
	}
      else
	{

	  if ((strlen (channel_ptr->buff_tmp) + seglen + 1) >
	      channel_ptr->buff_tmp_size)
	    {

	      channel_ptr->buff_tmp_size =
		((seglen + channel_ptr->buff_tmp_size + 1) | 0xff);
	      ptr = strdup (channel_ptr->buff_tmp);
	      channel_ptr->buff_tmp =
		(u_char *) realloc (channel_ptr->buff_tmp,
				    channel_ptr->buff_tmp_size);
	      memcpy (channel_ptr->buff_tmp, ptr, strlen (ptr) + 1);
	      free (ptr);
	    }
	}
      strcat (channel_ptr->buff_tmp, segment);

    }
  return;
}

int
flush_buffer_cache (void)
{
  char *ptr;


  if ((ptr = strrchr (channel_ptr->buff_tmp, '\n')) != NULL)
    {

      if (write_to_channel (channel_ptr->fd_in,
			    channel_ptr->buff_tmp,
			    (int) ((u_char *) ptr -
				   (u_char *) channel_ptr->buff_tmp) + 1) ==
	  -1)
	return _READY_;
      memmove (channel_ptr->buff_tmp, ptr + 1, strlen (ptr) + 1);
    }
  return _NULL_;
}

int
handle_link (void)
{
  struct timeval delay = { 0, 100 };
  int bt;

  fd_set r_set, w_set;


  FD_ZERO (&r_set);
  FD_ZERO (&w_set);

  FD_SET (channel_ptr->fd_in, &w_set);
  FD_SET (channel_ptr->fd_in, &r_set);
  FD_SET (channel_ptr->fd_out, &r_set);
  FD_SET (channel_ptr->fd_out, &w_set);


  switch (select (FD_SETSIZE, &r_set, &w_set, NULL, &delay))
    {
    case 0:
      return _ESTABLISHED_;
      break;
    case -1:
      shutdown_ (NULL);
      return _READY_;
      break;
    }

  memset (channel_ptr->buffer_in, 0, CHAN_SIZE_BUFF);
  memset (channel_ptr->buffer_out, 0, CHAN_SIZE_BUFF);


  if (FD_ISSET (channel_ptr->fd_out, &r_set)
      && FD_ISSET (channel_ptr->fd_in, &w_set))
    {

      rehash_time ();

      if (!(channel_ptr->opts & (CH_IRC_ | CH_LINE_))
	  && !(ndp.opts & OPT_LINE_))
	{

	  /* no-buffered client (raw-telnet) */

	  if (
	      (bt =
	       read_from_channel (channel_ptr->fd_out,
				  channel_ptr->buffer_in)) == -1)
	    return _READY_;

	  void_cr_lf (channel_ptr->buffer_in, 1);

	  if (write_to_channel
	      (channel_ptr->fd_in, channel_ptr->buffer_in, bt) == -1)
	    return _READY_;

	}
      else
	{

	  /* line-buffered client (irc) */

	  if (
	      (bt =
	       read_from_channel (channel_ptr->fd_out,
				  channel_ptr->buffer_in)) == -1)
	    return _READY_;

	  push_buffer_cache (channel_ptr->buffer_in);

	  if (flush_buffer_cache () == _READY_)
	    return _READY_;

	}

    }
  if (FD_ISSET (channel_ptr->fd_in, &r_set)
      && FD_ISSET (channel_ptr->fd_out, &w_set))
    {

      rehash_time ();

      if (
	  (bt =
	   read_from_channel (channel_ptr->fd_in,
			      channel_ptr->buffer_out)) == -1)
	return _READY_;

      /*
       * Guessing the stream's type: irc client needs a
       * line-buffered...
       */

      if ((channel_ptr->opts & CH_UNKNOWN_) && (ndp.opts & OPT_AUTO_))
	stream_guesser (channel_ptr->buffer_out);

      if (write_to_channel (channel_ptr->fd_out, channel_ptr->buffer_out, bt)
	  == -1)
	return _READY_;

    }
#ifdef HAVE_GETTIMEOFDAY
  gettimeofday (&delay, NULL);
#else
  delay.tv_sec = time (NULL);
  delay.tv_usec = 0;
#endif
  if (diff_time (delay.tv_sec, channel_ptr->sec) > ndp.conf.idle)
    {
      send_msg (NULL, MSG_IDLE_);
      shutdown_ (NULL);
      return _READY_;
    }
  return _NULL_;

}


int
setup_dialer (void)
{
  if (!(ndp.opts & OPT_CONF_))
    {

      if (ndp.vhost_4b)
	create_dialer_sock (channel_ptr, ndp.vhost_4b, ndp.rhost_4b,
			    ndp.rport);
      else
	create_dialer_sock (channel_ptr, INADDR_ANY, ndp.rhost_4b, ndp.rport);
    }
  if (channel_ptr->class & CL_IRC3)
    create_dialer_sock (channel_ptr, ndp.jvhost_4b, ndp.rhost_4b, ndp.rport);

  rehash_time ();

  return _CONNECTING_;

}


int
connect_target (void)
{
  int connect_out;
  int opt = 1;
  struct timeval now = { 0, 0 };

  /*
   * SO_LINGER the system will process the close in a manner that
   * allows to continue as quickly as possible
   */

  struct linger linger_= { 0, 0 };

  connect_out = connect (channel_ptr->fd_out,
			 (struct sockaddr *) &(channel_ptr->output_addr),
			 sizeof (channel_ptr->output_addr));

#ifdef HAVE_GETTIMEOFDAY
  gettimeofday (&now, NULL);
#else
  now.tv_sec = time (NULL);
  now.tv_usec = 0;
#endif

  if (diff_time (now.tv_sec, channel_ptr->sec) > _TIMEOUTCONN_)
    {

      send_msg (NULL, MSG_CONTIMO_);

      if ((channel_ptr->class == CL_IRC3)
	  || (ndp.opts & (OPT_LOCAL_ | OPT_REMOTE_)))
	{
	  shutdown_ (NULL);
	  return _READY_;
	}
      else
	{
	  halfshutdown_ (NULL);
	  return _SHELL_;
	}

    }
  if (connect_out == 0)
    {

      set_block (channel_ptr->fd_out);

      setsockopt (channel_ptr->fd_out,
		  SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof (linger_));
      setsockopt (channel_ptr->fd_out, SOL_SOCKET, SO_REUSEADDR,
		  (char *) &opt, sizeof (opt));

      if (ndp.opts & OPT_CONF_)
	send_msg (NULL, MSG_ESTABLISHED_);

      if (ndp.log_level & LOG_LEVEL2)
	syslog (LOG_INFO, "%s[%s] linked to %s:%d",
		class[get_class (channel_ptr->class)],
		multi_inet_nbotoa (channel_ptr->caller_addr.sin_addr.s_addr),
		multi_inet_nbotoa (channel_ptr->output_addr.sin_addr.s_addr),
		ntohs (channel_ptr->output_addr.sin_port));

      if (channel_ptr->opts & CH_IRC_)
	if (irc_postlogin () <= 0)
	  return _READY_;

      rehash_time ();

      return _ESTABLISHED_;
    }
  switch (errno)
    {

    case EINPROGRESS:
    case EALREADY:
      return _CONNECTING_;
      break;

    case EISCONN:
      setsockopt (channel_ptr->fd_out,
		  SOL_SOCKET, SO_LINGER, (char *) &linger_, sizeof (linger_));

      set_block (channel_ptr->fd_out);
      setsockopt (channel_ptr->fd_out, SOL_SOCKET, SO_REUSEADDR,
		  (char *) &opt, sizeof (opt));

      if (ndp.opts & OPT_CONF_)
	send_msg (NULL, MSG_ESTABLISHED_);

      if (ndp.log_level & LOG_LEVEL2)
	syslog (LOG_INFO, "%s[%s] linked to %s:%d",
		class[get_class (channel_ptr->class)],
		multi_inet_nbotoa (channel_ptr->caller_addr.sin_addr.s_addr),
		multi_inet_nbotoa (channel_ptr->output_addr.sin_addr.s_addr),
		ntohs (channel_ptr->output_addr.sin_port));

      if (channel_ptr->opts & CH_IRC_)
	if (irc_postlogin () <= 0)
	  return _READY_;

      rehash_time ();

      return _ESTABLISHED_;
      break;

    default:

      send_msg (NULL, "err: %s", strerror (errno));

      if ((channel_ptr->class == CL_IRC3)
	  || (ndp.opts & (OPT_LOCAL_ | OPT_REMOTE_)))
	{
	  shutdown_ (NULL);
	  return _READY_;
	}
      else
	{
	  halfshutdown_ (NULL);
	  return _SHELL_;
	}

      break;
    }

}


void
reset_chan (Channel * ptr)
{
  long saved;

  saved = (long) ptr->next;

  memset (ptr, 0, sizeof (Channel));
  ptr->flag = _READY_;
  ptr->fd_in = -1;
  ptr->buff_tmp = NULL;
  ptr->buff_tmp_size = 0;
  ptr->class = CL_UNKNOWN;
  ptr->opts |= CH_UNKNOWN_;
  ptr->next = (Channel *) saved;	/* This save the channel-queue */
  return;

}


void
add_chan (Channel * ptr)
{
  ptr->next = (Channel *) malloc (sizeof (Channel));
  ptr = ptr->next;
  ptr->next = (Channel *) NULL;
  reset_chan (ptr);

  return;

}


void
server (void)
{
  int opt = 1;
  struct sockaddr_in locale_addr;

  /*
   * SO_LINGER the system will process the close in a manner that
   * allows to continue as quickly as possible
   */

  struct linger linger_= { 0, 0 };

  int (*state_action[]) (void) =
    { accept_client, ushell, ushell, setup_dialer, connect_target,
    handle_link
  };


  first_channel = channel_ptr = (Channel *) malloc (sizeof (Channel));
  reset_chan (channel_ptr);

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

  if (!(ndp.lhost_4b))
    locale_addr.sin_addr.s_addr = INADDR_ANY;
  else
    locale_addr.sin_addr.s_addr = ndp.lhost_4b;

  setsockopt (fd_in, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));	/* jwk@lords.com */
  setsockopt (fd_in, SOL_SOCKET, SO_LINGER, (char *) &linger_,
	      sizeof (linger_));


  if (bind (fd_in, (struct sockaddr *) &(locale_addr), sizeof (locale_addr))
      == -1)
    {
      fprintf (stderr, "bind err:%s. Choose another local port\n",
	       strerror (errno));
      if (ndp.log_level)
	syslog (LOG_ERR, "bind err:%s. Choose another local port",
		strerror (errno));
      exit (-1);
    }
  if (listen (fd_in, 1))
    {
      if (ndp.log_level)
	syslog (LOG_ERR, "listen err:%s while setting up the main socket.",
		strerror (errno));
      exit (-1);
    }
  syslog (LOG_INFO, "started.");

  {

#define        free_chan        0x01

    register int ready = 0, tmp_state = 0;

    Channel *ptr_1 = NULL, *ptr_save = NULL;
    channel_ptr = (Channel *) NULL;

    for (;;)
      {
	/* reset cycle */

	if (channel_ptr == NULL)
	  {
	    ptr_1 = (Channel *) NULL;
	    channel_ptr = first_channel;
	    ready = 0;
	  }
	if (channel_ptr->flag == _READY_)
	  {

	    if (ready & free_chan)
	      {
		ptr_save = channel_ptr;
		channel_ptr = channel_ptr->next;
		ptr_1->next = channel_ptr;
		free (ptr_save);

	      }
	    else
	      ready |= free_chan;

	  }
	/* State action */

	if (channel_ptr != NULL)
	  {
	    if ((tmp_state = (*state_action[channel_ptr->flag]) ()) != _NULL_)
	      channel_ptr->flag = tmp_state;

	    ptr_1 = channel_ptr;
	    channel_ptr = channel_ptr->next;

	  }
	if ((channel_ptr == NULL) && !(ready & free_chan))
	  {
	    add_chan (ptr_1);
	    channel_ptr = ptr_1->next;
	  }
	free_time;

      }
  }

  exit (0);
}

void
_quit (int no)
{
  Channel *ptr = first_channel;

  while (ptr)
    {

      switch (ptr->flag)
	{
	case _ESTABLISHED_:
	  shutdown (ptr->fd_in , 2);
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
  char CONF[80];

  int pid_child = 0, c = 0;
  static char *exec_name;
  FILE *pidf;

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

  ndp.opts |= OPT_AUTO_;
  ndp.conf.idle = _TIMEOUT_;
  ndp.conf.idlep = _PASSTIMEOUT_;
  ndp.conf.maxuser = _MAXUSER_;
  ndp.conf.maxerr = _MAXERRONPASS_;

  ndp.log_level = 2;

  exec_name = argv[0];

  if (argc == 1)
    usage (argv[0], VERSION, ID);


  while (
	 (c =
	  getopt_long (argc, argv, "hs:l:vc:L:R:V:", long_options,
		       (int *) 0)) != EOF)
    switch (c)
      {

      case 'h':
	usage (exec_name, VERSION, ID);
	break;
      case 'c':
	ndp.opts |= OPT_CONF_;
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
	    fprintf (stderr, "err log level: -l[0-2] only.\n");
	    exit (-1);
	    break;
	  }
	break;
      case 's':
	switch (*optarg)
	  {
	  case 'l':
	    ndp.opts &= ~OPT_AUTO_;
	    ndp.opts &= ~OPT_CHAR_;
	    ndp.opts |= OPT_LINE_;
	    break;
	  case 'c':
	    ndp.opts &= ~OPT_AUTO_;
	    ndp.opts &= ~OPT_LINE_;
	    ndp.opts |= OPT_CHAR_;
	    break;
	  default:
	    fprintf (stderr, "err stream type: -sl or -sc olny\n");
	    exit (-1);
	    break;
	  }

	break;
      case 'v':
	fprintf (stderr, "%s %s\n", VERSION, ID);
	exit (0);
	break;

      case 'L':
	ndp.opts |= OPT_LOCAL_;
	if (parse_input (optarg, &ndp.lhost_4b, &ndp.lport) == -1)
	  {
	    fprintf (stderr, "parse error! %s ?\nType ndp -h to get help.\n",
		     optarg);
	    exit (-1);
	  }
	strncpy (ndp.lhost, optarg, 79);
	break;
      case 'R':
	ndp.opts |= OPT_REMOTE_;
	if (parse_input (optarg, &ndp.rhost_4b, &ndp.rport) == -1)
	  {
	    fprintf (stderr, "parse error! %s ?\nType ndp -h to get help.\n",
		     optarg);
	    exit (-1);

	  }
	strncpy (ndp.rhost, optarg, 79);
	break;

      case 'V':
	strncpy (ndp.vhost, optarg, 79);
	ndp.vhost_4b = getlongbyname (ndp.vhost);
	break;

      }

  if ((ndp.opts & OPT_CONF_) && (ndp.opts & (OPT_LOCAL_ | OPT_REMOTE_)))
    {
      fprintf (stderr,
	       "ndp: -L-R and -c are mutually exclusive and cannot be used at the same time.\n");
      exit (-1);
    }
  if ((~ndp.opts | ~OPT_CONF_) >> 2 & (ndp.opts & OPT_REMOTE_) >> 1 &
      (~ndp.opts | ~OPT_LOCAL_) & 0x01)
    {
      fprintf (stderr, "ndp: -L option is required with -R.\n");
      exit (-1);
    }
  if ((~ndp.opts | ~OPT_CONF_) >> 2 & (~ndp.opts | ~OPT_REMOTE_) >> 1 &
      (ndp.opts & OPT_LOCAL_) & 0x01)
    {
      fprintf (stderr, "ndp: -R option is required with -L.\n");
      exit (-1);
    }
  if (!(ndp.opts & OPT_CONF_) && !(ndp.opts & (OPT_LOCAL_ | OPT_REMOTE_)))
    {
      fprintf (stderr, "ndp: -L-R or -c options are required.\n");
      exit (-1);
    }
  signal (SIGHUP, _quit);

  /*
   * troubles with SIGHUP: if your system cares hangup signals after
   * your logout, you can easly remove the signal crap changing the
   * handler in:
   * 
   * signal (SIGHUP, SIG_IGN);
   * 
   */

  if (ndp.opts & OPT_CONF_)
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

      fprintf (stderr, "\n--(%s on %s\n--(%s %s\n\n", VERSION, CPU_MFR_OPSYS,
	       ID, MAIL_SUPPORT);
      fprintf (stderr, "Server            : %s:%d\n",
	       (ndp.lhost ? ndp.lhost : "INADDR_ANY"), ndp.lport);
      fprintf (stderr, "Vhost             : %s\n",
	       (ndp.vhost ? ndp.vhost : getenv ("HOSTNAME")));
      fprintf (stderr, "Target            : %s:%d\n",
	       (ndp.rhost ? ndp.rhost : "INTERACTIVE"), ndp.rport);
      fprintf (stderr, "Password          : %s\n",
	       ((ndp.opts & OPT_CONF_) ? "on" : "off"));

      if (ndp.opts & OPT_AUTO_)
	fprintf (stderr, "Stream-type       : auto\n");
      else
	fprintf (stderr, "Stream-type       : %s\n",
		 ((ndp.opts & OPT_LINE_) ? "line" : "char"));

      fprintf (stderr, "Idle-time         : %d\n", ndp.conf.idle);
      if (ndp.opts & OPT_CONF_)
	{
	  fprintf (stderr, "pass-idle         : %d\n", ndp.conf.idlep);
	  fprintf (stderr, "Maxuser           : %d\n", ndp.conf.maxuser);
	  fprintf (stderr, "masstrial pass    : %d\n", ndp.conf.maxerr);
	  fprintf (stderr, "Classes loaded    : %s%s%s%s%s\n",
		   ((ndp.pass.master) ? "master " : ""),
		   ((ndp.pass.user) ? "user " : ""),
		   ((ndp.pass.ircer0) ? "ircer0 " : ""),
		   ((ndp.pass.ircer1) ? "ircer1 " : ""),
		   ((ndp.pass.ircer2) ? "ircer2 " : ""));
	}
      fprintf (stderr, "Successfully forked, spawns pid (%d)\n\n", pid_child);
      break;
    }

  exit (0);
}
