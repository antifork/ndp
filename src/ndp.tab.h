/* apg.db 01/06/18 rel 02 $Id$ */
/* 
 *  $Id$
 *  apg.tab.h
 *  Generated automatically by apg: autoparser generator.
 *
 *  Sources are available at 	
 *		ftp://ftp.antifork.org/apg/ 
 *		cvs -d :pserver:anoncvs@cvs.antifork.org:/CVS checkout apg
 *	
 *  Copyright (c) 1999 Bonelli Nicola  <bonelli@antifork.org>
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

#ifndef _APG_TAB_H
#define _APG_TAB_H

#ifndef _APG_PARSER_C
extern int apg_errno;
#endif

#define APG_MAXERR      3

#define APG_EOK         0
#define APG_EEMPTY	1
#define APG_ENULL       2
#define APG_EEOG        3


#define ircer0_label	1
#define ircer2_label	2
#define oport_label	3
#define ircer1_label	4
#define user_label	5
#define if_label	6
#define feature_label	7
#define master_label	8


typedef struct __type_line__ {
  int	type_line;
  union {
     struct {
	char *passwd;
        } ircer0_line;
     struct {
	char *passwd;
	char *vhost;
	char *lhost;
	short port;
        } ircer2_line;
     struct {
	short low;
	short high;
        } oport_line;
     struct {
	char *passwd;
	char *vhost;
        } ircer1_line;
     struct {
	char *passwd;
        } user_line;
     struct {
	char *ip;
	short port;
	char *vhost;
        } if_line;
     struct {
	int idle;
	int idlep;
	int maxuser;
	int maxerr;
        } feature_line;
     struct {
	char *passwd;
        } master_line;
    } line_dun;
  struct __type_line__	*next;
} grill_t;



#define ircer0_passwd		line_dun.ircer0_line.passwd
#define ircer2_passwd		line_dun.ircer2_line.passwd
#define ircer2_vhost		line_dun.ircer2_line.vhost
#define ircer2_lhost		line_dun.ircer2_line.lhost
#define ircer2_port		line_dun.ircer2_line.port
#define oport_low		line_dun.oport_line.low
#define oport_high		line_dun.oport_line.high
#define ircer1_passwd		line_dun.ircer1_line.passwd
#define ircer1_vhost		line_dun.ircer1_line.vhost
#define user_passwd		line_dun.user_line.passwd
#define if_ip		line_dun.if_line.ip
#define if_port		line_dun.if_line.port
#define if_vhost		line_dun.if_line.vhost
#define feature_idle		line_dun.feature_line.idle
#define feature_idlep		line_dun.feature_line.idlep
#define feature_maxuser		line_dun.feature_line.maxuser
#define feature_maxerr		line_dun.feature_line.maxerr
#define master_passwd		line_dun.master_line.passwd

/* prototypes */

/* Use prototypes unless the compiler is old-fashioned.  */

#ifdef __STDC__
#define P(x) x
#else
#define P(x) ()
#endif

char *apg_strerror P ((int));
grill_t *apg_parser P ((int, ...));
int apg_get_line P ((grill_t **));
void apg_free_grill P ((grill_t *));
void apg_free_pragma P ((void));

/* parser header side */

#ifdef _APG_PARSER_C
/* includes */

#include <stdlib.h>

#ifndef _STDIO_H
#include <stdio.h>
#endif

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#include <string.h>
#include <errno.h>
#include <ctype.h>

/* apg types */

#ifndef _HAVE_GRAMMAR_TAB_H
#define T_INT   	0x01
#define T_SHORT 	0x02
#define T_CHAR 		0x03
#define T_U_32 		0x04
#define T_U_16 		0x05
#define T_U_8  		0x06
#define T_STR  		0x07
#define T_HOST  	0x08
#define T_IPV4 		0x09
#endif

/* apg type size */

static const int types_size[] =
  { 0, 	sizeof (int), 	sizeof (short), sizeof (char),
  	sizeof (long), 	sizeof (short), sizeof (char),
  	sizeof (void *),sizeof (void *),sizeof (void *)
};

/* apg type id */

static const char *const types_id[] =
  { NULL, "INT", "SHORT", "CHAR", "U_32", "U_16", "U_8", "STR", "HOST", "IPV4"
};

/* apg table */

#define APG_MAXARG 5
static const int apgtb[][5][5] = {
{ {         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         7,          0,          0,          1,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         7,          0,          0,          1,          0},
{         8,          0,        255,          1,          0},
{         8,          0,        255,          1,          0},
{         2,     -32768,      32767,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         2,     -32768,      32767,          0,          0},
{         2,     -32768,      32767,          0,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         7,          0,          0,          1,          0},
{         8,          0,        255,          1,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         7,          0,          0,          1,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         8,          0,        255,          0,          0},
{         2,     -32768,      32767,          1,          0},
{         8,          0,        255,          0,          0},
{         0,          0,          0,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         1,          0,    1000000,          0,          0},
{         1,          0,     100000,          0,          0},
{         1,          0,        100,          0,          0},
{         1,          0,        100,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         7,          0,          0,          1,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
},
};


/* apg offset */

static int apg_offset[][APG_MAXARG]={
{ -1, -1, -1, -1, -1, },
{ -1, 0, -1, -1, -1, }, 
{ -1, 0, 4, 8, 12, }, 
{ -1, 0, 2, -1, -1, }, 
{ -1, 0, 4, -1, -1, }, 
{ -1, 0, -1, -1, -1, }, 
{ -1, 0, 4, 8, -1, }, 
{ -1, 0, 4, 8, 12, }, 
{ -1, 0, -1, -1, -1, }, 
};

/* typedef: seg_t and line_t */

typedef struct
{
  int hash;
  char *id;
}
line_t;

typedef struct i_seg_t
{
  char *ptr;
  struct i_seg_t *next;
}
seg_t;

/* line_t list */

static const line_t line_v[] = { { 0, NULL }, { 44751134U, "ircer0" }, { 1461453315U, "ircer2" }, { 1650802934U, "oport" }, { 2245943175U, "ircer1" }, { 3097438796U, "user" }, { 3200026375U, "if" }, { 3579935232U, "feature" }, { 3708426930U, "master" }, };

#define QMAX_ELEM	8

/* rep limits */

static long rep_limits[][2] = { {0,0},
{    -1,     1 },
{    -1,     1 },
{    -1,     1 },
{    -1,     1 },
{    -1,     1 },
{     1,     1 },
{     1,     1 },
{     1,     1 },
};

static unsigned long rep_counter[] ={
 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* custom delimiters */

#define APG_SEPLINE  '='
#define APG_SEPTOKEN ':'

#endif /* _APG_PARSER_C */
#endif /* _APG_TAB_H */

