#line 2 "/usr/local/share/apg/apg.db"
/* apg.db,v 1.14 2001/07/26 21:42:51 awgn Exp $ */
#line 4 "/usr/local/share/apg/apg.db"
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

/* Use prototypes unless the compiler is old-fashioned.  */
#include <sys/types.h>
#include <sys/cdefs.h>

#if !defined (__P)
# ifdef __STDC__
# define __P(x) x
# else
# define __P(x) ()
# endif
#endif

#if defined(__ANSI__) || defined (__STRICT_ANSI__)
#define u_char  unsigned char
#define u_short unsigned short
#define u_int   unsigned int
#define u_long  unsigned long
#endif

/* functions declarations */

#if defined(__cplusplus) || defined(__ANSI__) || defined (__STRICT_ANSI__) 

/* g++ / ANSI */

#define AAINT_ARG        int
#define AAUINT_ARG	 u_int
#define AASIZE_ARG       size_t 
#define AACHARx_ARG      char *
#define AAVOIDx_ARG      void *
#define AAGRILLx_ARG     grill_t *
#define AAGRILLxx_ARG	 grill_t **
#define AATIMEVAL_ARG	 struct timeval
#define AAELAPSEDx_ARG   elapsed_val *

#define AAINT_DECL(a)
#define AAUINT_DECL(a)   
#define AASIZE_DECL(a)    
#define AACHARx_DECL(a)
#define AAVOIDx_DECL(a)
#define AAGRILLx_DECL(a)
#define AAGRILLxx_DECL(a)
#define AATIMEVAL_DECL(a)
#define AAELAPSEDx_DECL(a)

#else
/* GNU c */

#define AAINT_ARG
#define AAUINT_ARG       
#define AASIZE_ARG       
#define AACHARx_ARG    
#define AAVOIDx_ARG      
#define AAGRILLx_ARG     
#define AAGRILLxx_ARG 
#define AATIMEVAL_ARG
#define AAELAPSEDx_ARG    

#define AAINT_DECL(a)      int a;
#define AAUINT_DECL(a)     u_int a; 
#define AASIZE_DECL(a)     size_t a; 
#define AACHARx_DECL(a)    char *a;
#define AAVOIDx_DECL(a)    void *a;
#define AAGRILLx_DECL(a)   grill_t *a;
#define AAGRILLxx_DECL(a)  grill_t **a;
#define AATIMEVAL_DECL(a)  struct timeval a;
#define AAELAPSEDx_DECL(a) elapsed_val *a;

#endif


#define ircer0_label	1
#define ircer2_label	2
#define ircer1_label	3
#define feature_label	4
#define user_label	5
#define if_label	6
#define oport_label	7
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
	char *passwd;
	char *vhost;
        } ircer1_line;
     struct {
	int idle;
	int idlep;
	int maxuser;
	int maxerr;
        } feature_line;
     struct {
	char *passwd;
        } user_line;
     struct {
	char *ip;
	short port;
	char *vhost;
        } if_line;
     struct {
	short low;
	short high;
        } oport_line;
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
#define ircer1_passwd		line_dun.ircer1_line.passwd
#define ircer1_vhost		line_dun.ircer1_line.vhost
#define feature_idle		line_dun.feature_line.idle
#define feature_idlep		line_dun.feature_line.idlep
#define feature_maxuser		line_dun.feature_line.maxuser
#define feature_maxerr		line_dun.feature_line.maxerr
#define user_passwd		line_dun.user_line.passwd
#define if_ip		line_dun.if_line.ip
#define if_port		line_dun.if_line.port
#define if_vhost		line_dun.if_line.vhost
#define oport_low		line_dun.oport_line.low
#define oport_high		line_dun.oport_line.high
#define master_passwd		line_dun.master_line.passwd

/* prototypes */

#line 116 "/usr/local/share/apg/apg.db"
/* We don't care about __THROW gcc 2.8.x optimizations */
#ifdef   __THROW
#undef   __THROW
#define  __THROW
#endif

char 	*apg_strerror __P ((int));
grill_t *apg_parser  __P ((int, ...));
int 	apg_get_line __P ((grill_t **));
void 	apg_free_grill __P ((grill_t *));
void 	apg_free_pragma __P ((void));

/* parser header side */

#line 131 "/usr/local/share/apg/apg.db"
#ifdef _APG_PARSER_C
/* header */

#line 133 "/usr/local/share/apg/apg.db"
#include <stdlib.h>

#ifndef _STDIO_H
#include <stdio.h>
#endif

#include <limits.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <string.h>
#include <errno.h>
#include <ctype.h>

/* apg types */

#line 160 "/usr/local/share/apg/apg.db"
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

#line 173 "/usr/local/share/apg/apg.db"
static const int types_size[] =
  { 0, sizeof (int), sizeof (short), sizeof (char), sizeof (long), sizeof (short), sizeof (char), sizeof (void *), sizeof (void *), sizeof (void *)
};

/* apg type id */

#line 178 "/usr/local/share/apg/apg.db"
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
{         7,          0,          0,          1,          0},
{         8,          0,        255,          1,          0},
{         0,          0,          0,          0,          0},
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
{{         0,          0,          0,          0,          0},
{         8,          0,        255,          0,          0},
{         2,     -32768,      32767,          1,          0},
{         8,          0,        255,          0,          0},
{         0,          0,          0,          0,          0},
},
{{         0,          0,          0,          0,          0},
{         2,     -32768,      32767,          0,          0},
{         2,     -32768,      32767,          0,          0},
{         0,          0,          0,          0,          0},
{         0,          0,          0,          0,          0},
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
{ -1, 0, 4, -1, -1, }, 
{ -1, 0, 4, 8, 12, }, 
{ -1, 0, -1, -1, -1, }, 
{ -1, 0, 4, 8, -1, }, 
{ -1, 0, 2, -1, -1, }, 
{ -1, 0, -1, -1, -1, }, 
};

/* typedef: seg_t and line_t */

#line 183 "/usr/local/share/apg/apg.db"
typedef struct
{
  u_long hash;
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

static const line_t line_v[] = { { 0, NULL }, { 44751134U, "ircer0" }, { 1461453315U, "ircer2" }, { 2245943175U, "ircer1" }, { 2453648968U, "feature" }, { 3097438796U, "user" }, { 3200026375U, "if" }, { 3313241630U, "oport" }, { 4062030002U, "master" }, };

#define QMAX_ELEM	8

/* rep limits */

static long rep_limits[][2] = { {0,0},
{    -1,     1 },
{    -1,     1 },
{    -1,     1 },
{     1,     1 },
{    -1,     1 },
{     1,     1 },
{    -1,     1 },
{     1,     1 },
};

static long int rep_counter[] ={
 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* custom delimiters */

#define APG_SEPLINE  '='
#define APG_SEPTOKEN ':'

#line 198 "/usr/local/share/apg/apg.db"
#endif /* _APG_PARSER_C */
#endif /* _APG_TAB_H */

