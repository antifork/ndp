/*
 *  $Id$
 *  %ndp: shell's command
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

#include <command.h>
#include <util.h>
#include <ns-cache.h>

extern ctrl_t ndp;
extern chan_t *p_chan;
extern chan_t *f_chan;
extern char *class[];
extern char opts;

int
get_class(int i)
{
	register short int cl = 0;
	while (i >>= 1)
		cl++;

	return cl;
}


void
welcome()
{
	char *ip;

	ip = safe_inet_ntoa(p_chan->c_addr.sin_addr.s_addr);

	if (ndp.log_level & LOG_LEVEL2)
		syslog(LOG_INFO, "%s logged from %s with gecos irc %s!%s",
		       class[get_class(p_chan->class)], (char *) ip,
		       (*(char *) (p_chan->nick) ? (char *) p_chan->nick : "unknown"),
		       (*(char *) (p_chan->name) ? (char *) p_chan->name : "unknown"));

	send_msg(NULL, MSG_MOTD);
	send_msg(NULL, "%s logged from %s\n", class[get_class(p_chan->class)], (char *) ip);

	return;
}


int
c_link(int _argc, char **_argv, char *null)
{
	unsigned long rhost_dyn = 0;
	unsigned long vhost_dyn = 0;
	unsigned short rport_dyn = 0;

	if ((_argc < 3) || !(rhost_dyn = gethostbyname_cache(_argv[1])))
		return -1;

	if ((rport_dyn = (u_short) atoi(_argv[2])) > 0)
		switch (p_chan->class) {
		case CL_IRC1:
		case CL_IRC2:
		case CL_IRC3:
			if ((rport_dyn < ndp.conf.lp_irc) || (rport_dyn > ndp.conf.hp_irc)) {
				send_msg(NULL, "ndp: you are not allowed to connect that port\n");
				return 0;
			}
			break;
		}
	else
		return -1;

	if (_argv[3] != NULL)
		switch (p_chan->class) {
		case CL_IRC2:
		case CL_IRC3:
			send_msg(NULL, "ndp: you are not allowed to bind interfaces\n");
			break;

		default:
			if (!(vhost_dyn = gethostbyname_cache(_argv[3])))
				return -1;
			break;
		}

	if (!vhost_dyn)
		switch (p_chan->class) {
		case CL_MASTER:
		case CL_USER:
		case CL_IRC1:
			if (ndp.nbo_vhost)
				vhost_dyn = ndp.nbo_vhost;
			else
				vhost_dyn = INADDR_ANY;
			break;
		case CL_IRC2:
			if (ndp.nbo_ivhost)
				vhost_dyn = ndp.nbo_ivhost;
			else
				vhost_dyn = INADDR_ANY;
			break;
		case CL_IRC3:
			vhost_dyn = ndp.nbo_jvhost;
			break;
		}

	create_dialer_sock(p_chan, vhost_dyn, rhost_dyn, rport_dyn);
	send_msg(NULL, MSG_CONNECTING);
	rehash_time();

	return 1;
}

int
c_if(int _argc, char **_argv, char *null)
{

	char buffer[10240];
	int sd;
	struct ifreq *ifr, *iflast, ifreq_io;
	struct ifconf ifc;
	struct sockaddr_in *ptr;

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
	for (; ifr < iflast; (char *) ifr += sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len)
#else
	for (; ifr < iflast; (char *) ifr += sizeof(ifr->ifr_name) + sizeof(struct sockaddr_in))
#endif
	{
		if (*(char *) ifr == 0)
			continue;

		/* setup ifreq structure, needed by SIOCGIFFLAGS */

		ptr = (struct sockaddr_in *) & ifr->ifr_addr;
		memcpy(&ifreq_io, ifr, sizeof(ifr->ifr_name) + sizeof(struct sockaddr_in));

		/*
		 * The SIOCGIFFLAGS gets ifreq_io (IO call) and fills it with
		 * new values
		 */

		if (ioctl(sd, SIOCGIFFLAGS, &ifreq_io) < 0) {
			send_msg(NULL, "SIOCGIFFLAGS:%s\n", strerror(errno));
			close(sd);
			return 1;
		}
		if ((ifreq_io.ifr_flags & IFF_UP) == 0)
			continue;

		if (ifr->ifr_addr.sa_family != AF_INET)
			continue;

		send_msg(NULL, "%s:flags=<%hx> %s(%s)\n", \
			 ifr->ifr_name, ifreq_io.ifr_flags, \
			 gethostbyaddr_cache(ptr->sin_addr.s_addr), \
			 safe_inet_ntoa(ptr->sin_addr.s_addr));

	}

	close(sd);
	return 1;
}

#define cnz(x) ((x > 1) ? 1 : 0)

int
c_who(int _argc, char **_argv, char *null)
{
	char *cl[] = {"M", "U", "I", "i", "j", "^"}, *sip, *rip, null_ip[] = "-", me = '*', other = ' ';
	register short int ct = 1, tfd = 0;
	unsigned short rpt = 0;

	chan_t *ptr;
	ptr = f_chan;

	send_msg(NULL, "CL  FD  FROM            TO             port  gecos\n");

	for (; ptr != NULL; ptr = ptr->next) {

		if (!(ptr->state))
			continue;

		sip = safe_inet_ntoa(ptr->c_addr.sin_addr.s_addr);

		if (ptr->state & S_ESTABL) {
			rip = safe_inet_ntoa(ptr->o_addr.sin_addr.s_addr);
			rpt = ntohs(ptr->o_addr.sin_port);
		} else {
			rip = (char *) null_ip;
			rpt = 0;
		}

		send_msg(NULL, "%s%c  %2d  %-15.15s %-15.15s%-5d %.25s...\n",
		  cl[get_class(ptr->class)], ((ptr == p_chan) ? me : other),
			 ptr->fd_in, sip, rip, rpt, ptr->name);

		tfd += (cnz(ptr->fd_in) + cnz(ptr->fd_out));
		ct++;
	}

	send_msg(NULL, "users %d of %d, total fds:%d\n", ct - 1, ndp.conf.maxuser, tfd);
	return 1;
}

int
c_msg(int _argc, char **_argv, char *input)
{
	register short int tg = 0;
	register chan_t *ptr;

	if ((_argc < 3) || !ISDIGIT(_argv[1]))
		return -1;

	ptr = f_chan;

	while (*input != 0 && *input == ' ')
		input++;

	input += 3;

	while (*input != 0 && *input == ' ')
		input++;
	while (*input != 0 && *input != ' ')
		input++;
	while (*input != 0 && *input == ' ')
		input++;

	tg = atoi(_argv[1]);

	while ((ptr->next != NULL) && (ptr->fd_in != tg))
		ptr = ptr->next;

	if (ptr->next == NULL)
		return (-1);

	send_msg(ptr, "%s\n", input);

	return 1;
}

int
c_kill(int _argc, char **_argv, char *null)
{
	register short int ct = 0;
	register chan_t *ptr;

	if ((_argc < 2) || !ISDIGIT(_argv[1]))
		return -1;

	ct = atoi(_argv[1]);
	ptr = f_chan;

	while ((ptr->next != NULL) && (ptr->fd_in != ct))
		ptr = ptr->next;

	if (ptr->next != NULL) {
		send_msg(ptr, "Closing link requested\n");
		shutdown_(ptr);
		return 1;
	} 

	return(-1);
}

item command[] = {
        {c_who, CL_MASTER | CL_USER, "w,who                  - show users and filedescriptors"},
        {c_if, CL_MASTER | CL_USER | CL_IRC1, "iflist                 - show interfaces list"},
        {c_kill, CL_MASTER, "kill <fd>              - reset <fd> connection"},
        {c_msg, CL_MASTER | CL_USER, "msg  <fd> message      - send a message to <fd>"},
        {c_link, CL_MASTER | CL_USER | CL_IRC1 | CL_IRC2, "link host:port:[vhost] - connect remote host"},
        {c_exit, CL_MASTER | CL_USER | CL_IRC1 | CL_IRC2, "exit,logout            - close link"},
        {c_help, CL_MASTER | CL_USER | CL_IRC1 | CL_IRC2, "help                   - print this help"},

        /* alias */

        {c_who, CL_MASTER | CL_USER, NULL},
        {c_msg, CL_MASTER | CL_USER, NULL},
        {c_link, CL_MASTER | CL_USER | CL_IRC1 | CL_IRC2, NULL},
        {c_exit, CL_MASTER | CL_USER | CL_IRC1 | CL_IRC2, NULL},
{0, 0, 0}};

int
c_help(int _argc, char **_argv, char *null)
{
	register int i = 0;

	for ( ; command[i].descr != NULL;i++) {
		if (p_chan->class & command[i].perm)
			send_msg(NULL, "%s\n", command[i].descr);
	}
	return 1;
}


int
c_exit(int _argc, char **_argv, char *null)
{
	send_msg(p_chan, "logout\n");
	shutdown_(p_chan);
	return 1;
}


/* qhash */

typedef struct {
	char *key;
	int id;
}      tab_t;

#define TABLE_SIZE (sizeof(tab)/sizeof(tab[0]))  /* 29 */

static tab_t tab[]={
{"connect",9 },{"",-1 },{"logout",10 },{"",-1 },{"send",8 },{"link",4 },{"msg",3 },{"",-1 },{"",-1 },{"help",6 },{"w",7 },
{"",-1 },{"",-1 },{"who",0 },{"",-1 },{"",-1 },{"",-1 },{"",-1 },{"",-1 },{"iflist",1 },{"kill",2 },
{"exit",5 },{"",-1 },{"",-1 },{"",-1 },{"",-1 },{"",-1 },{"",-1 },{"",-1 },};

 

/*
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *      chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy!     :-)
 */

#define FNV32_prime 16777619UL
#define FNV32_init  2166136261UL

unsigned long
hash(char *p)
{
	unsigned long h = FNV32_init;
	int i = 0;

	for (; p[i]; i++) {
		h = h * FNV32_prime;
		h = h ^ p[i];
	}

	return h;
}

/* <-- EOF --> */

char *
TL(char *p)
{
	char *ret = p;

	if (p == NULL)
		return NULL;

	while ((*p = tolower(*p)))
		p++;

	return (ret);
}

#ifdef __GNUC__
__inline
#endif
int
qsearch(p)
	const char *p;
{
	unsigned int i;
	if (p == NULL)
		return -1;
	i = (unsigned int) hash((char *) p) % TABLE_SIZE;
	if (*tab[i].key == *p && strcmp(tab[i].key, p) == 0)
		return tab[i].id;
	else
		return -1;
}

int
command_shell(char *input)
{
	char *argz[MAXARGLINE];
	char *s_input;
	short n = 0, i = 0;

	s_input = (char *) alloca(strlen(input) + 1);

	memset(s_input, 0, strlen(input) + 1);
	memcpy(s_input, input, strlen(input));

	/* skip spaces */
	while ((*input == ' '))
		input++;

	if (*input == 0)
		return -1;

	if ((n = parse_comm(input, argz)) == 0)
		return -1;

	/* convert to lowercase */
	TL(argz[0]);
	i = qsearch(argz[0]);

	if ( i == -1)
		return -1;

	/* Permission */
	if (!(p_chan->class & command[i].perm))
		return -1;

	/* Command execution */
	return (*command[i].c_handler) (n, argz, s_input);
}
