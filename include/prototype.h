
#ifndef PROTOTYPE_H
#define PROTOTYPE_H
#include <channel.h>

char *mystrstr(char *, char *);
char *ndp_strstr(char *, char *);
char getlastchar(u_char *);
int accept_client(void);
int auth_user(char *pass);
int c_exit(int, char **, char *);
int c_help(int, char **, char *);
int c_if(int, char **, char *);
int c_kill(int, char **, char *);
int c_link(int, char **, char *);
int c_msg(int, char **, char *);
int c_who(int, char **, char *);
int checkpass(u_char *);
int checkpass(unsigned char *);
int command_shell(char *);
int connect_target(void);
int diff_time(long, long);
int get_class(int);
int handle_link(void);
int irc_postlogin(void);
int match_irc(char *);
int ndp_strcmp(char *, char *, int);
int parse_comm(char *, char **);
int parse_input(char *, long *, unsigned short *);
int parse_irc(char *, char **);
int parse_config(char *);
int read_chan(int, char *);
int send_msg(chan_t *, char *,...);
int setup_dialer(void);
int ushell(void);
int write_chan(int, char *, int);
long getlongbyname(unsigned char *);
void _quit(int);
void add_chan(chan_t *);
void create_dialer_sock(chan_t *, long, long, int);
void halfshutdown_(chan_t *);
void irc_controller(char *);
void readconf(char *);
void rehash_time(void);
void reset_chan(chan_t *);
void server(void);
void set_block(int);
void set_nonblock(int);
char *strmrg(char *, char *);
void shutdown_(chan_t *);
void stream_dissector(char *);
void usage(char *, char *, char *);
void void_chan(chan_t *);
void void_cr_lf(u_char *, int);
void welcome(void);

#endif                          /* PROTOTYPE_H */

