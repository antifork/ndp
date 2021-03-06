#line 2 "/usr/local/share/apg/apg.db"
/* $Id$ */
#line 139 "/usr/local/share/apg/apg.db"
/*
 *  $Id$
 *  apg.par.c
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

/* standard includes */

#line 167 "/usr/local/share/apg/apg.db"
#define _APG_PARSER_C
#line 1 "ndp.par.c"
#include "ndp.tab.h"
#line 169 "/usr/local/share/apg/apg.db"
/* mealy prototypes */

#line 170 "/usr/local/share/apg/apg.db"
static int ac_0 __P((void));
static int ac_1 __P((void));
static int ac_2 __P((void));
static int ac_3 __P((void));
static int ac_4 __P((void));
/* static variables */

#line 176 "/usr/local/share/apg/apg.db"
static char *file_name;
static char *file_image;
static int apg_flags;
static int apg_currentline;
static int apg_buffline;
static int apg_statecode;
static int apg_inputcode;
static int apg_tokenid;
static int apg_stream_len;

#ifdef APG_PERF
static int apg_global;
#endif
static u_char *b_token;
static u_char *p_token;
static u_char *p_stream;
static grill_t *g_stream;
static grill_t *g_arena;
static seg_t *head_seg;
static seg_t *tail_seg;
int apg_errno;
/* common defines and internal flags */

#line 198 "/usr/local/share/apg/apg.db"
#define APG_OPEN                0
#define APG_CLOSE               1
#define APG_ACK          	0x06

/* general flags */
#define APG_PARSED		0x01
#define APG_RESET		0x02
#define APG_OCTT		0x04
#define APG_HEXT		0x08
#define APG_FATAL		0x10

/* msg error codes */
#define APG_ETYPE		1
#define APG_ENULL		2
#define APG_EOFFSET		3
#define APG_ELIMIT		4
#define APG_EESC		5
#define APG_EFEW		6
#define APG_EMANY		7
#define APG_EIPV4		8
#define APG_EHOST		9

/* macros */

#line 221 "/usr/local/share/apg/apg.db"
#if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__ANSI__) && !defined(__cplusplus)
static int c_index[256] = { ['#'] 1,[APG_SEPLINE] 2,[APG_SEPTOKEN] 3,['\''] 4,['\\'] 5,[' '] 6,['\t'] 6,['\n'] 7
};
#else
static int c_index[256];
#endif

/* Macro */
#define MAX(x,y) ( (x)>(y) ? (x) : (y) )
#define MIN(x,y) ( (x)<(y) ? (x) : (y) )

#define O_PUSH(base, offset, obj ,t) 	\
( *(t *)((u_char *)base+offset+sizeof(u_long)) = (t)*obj)

#define P_PUSH(base, offset, ptr) 	\
( *(u_long *)((u_char *)base+offset+sizeof(u_long))=(u_long)ptr)

#define O_DIGIT(x) 			\
( '0' <= x && x <= '3' ? 2 : ( '4' <= x && x <= '7' ? 1 : 0 ) )

#define C_LIMIT(t,x,v,y) 		\
( (x|y) ? ((t == T_U_32) ? ((u_int)x<=(u_int)v ) && ((u_int)v<=(u_int)y) : ((x<=v) && (v<=y))) : (1) )

#define M_STRTOL(token,addr_endptr) 	\
( apg_flags & (APG_OCTT|APG_HEXT) ? strtol(token,addr_endptr,0) :  strtol(token,addr_endptr,10))

/* private functions, mealy tables */

#line 248 "/usr/local/share/apg/apg.db"
#if __STDC__
static void fatalerr(char *pattern,...) __attribute__((noreturn));
static void
fatalerr(char *pattern,...)
#else
static void
fatalerr(pattern, va_alist)
char *pattern;
va_dcl
#endif
{
	va_list ap;

#if __STDC__
	va_start(ap, pattern);
#else
	va_start(ap);
#endif
	vfprintf(stderr, pattern, ap);
	fprintf(stderr, "; exit forced.\n");
	va_end(ap);
	exit(1);
}

/* xmalloc, xcalloc, xrealloc */
static void *
xmalloc(u_int s)
{
	void *result;

	result = malloc(s);
	if (result == 0)
		fatalerr("xmalloc: virtual memory exhausted");
	return result;
}

static void *
xrealloc(void *ptr, u_int s)
{
	void *result;

	/* Some older implementations of realloc() don't conform to ANSI.  */
	result = ptr ? realloc(ptr, s) : malloc(s);

	if (result == 0)
		fatalerr("xrealloc: virtual memory exhausted");
	return result;
}

static void *
xcalloc(u_int nelem, u_int s)
{
	void *result = (char *) calloc(nelem, s);

	if (result == 0)
		fatalerr("xcalloc: virtual memory exhausted");
	return result;
}

static char *
strprg(char * p, int i)
{
	register int l;
	register char *q;

	if (p == NULL)
		return NULL;

	l = MIN(i, (int) strlen(p));
	q = p;

	while (*(q + l) != 0) {
		*q = *(q + l);
		q++;
	}

	*q = 0;
	return p;
}

static char *
mmap_ctrl(char * fn, int flag)
{
        struct stat f_stat;
	int fd;

	switch (flag) {
	case APG_OPEN:
		file_name = (char *) xmalloc(strlen(fn) + 1);
		strcpy(file_name, fn);

		/* no TOCTOU */
		if (stat(fn, &f_stat) == -1)
			return (char *) NULL;

		apg_stream_len = (int) f_stat.st_size;
		if ((fd = open(fn, O_RDONLY)) == -1)
			fatalerr("err: %s", strerror(errno));

		file_image=(char *)mmap(NULL, apg_stream_len, PROT_READ, MAP_PRIVATE, fd, 0);
		if (file_image == NULL)
			fatalerr("err: mmap error?!");

		close(fd);
		return file_image;
	case APG_CLOSE:
		if (file_name != NULL)
			free(file_name);
		if (file_image == NULL || apg_stream_len == 0)
			fatalerr("err: apg_buffer already closed/not open yet");

		munmap(file_image, apg_stream_len);
		apg_stream_len = 0;

		return file_image = NULL;
	}

	return (char *) NULL;	/* unreachable */
}

static int
ac_0()
{
	if (*p_stream == '\\')
		p_stream++;
	if (*p_stream++ == '\n')
		apg_buffline++;

	return 1;
}

static int
ac_1()
{
	*(p_token++) = *(p_stream++);
	return 1;
}

static int
ac_2()
{
	if (*p_stream == '\n')
		apg_buffline++;

	*p_token++ = '\0';
	p_stream++;
	apg_tokenid++;

	return 0;
}

#if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
static char c_escape[256] = {['a'] '\a',['b'] '\b',['t'] '\t',['n'] '\n',['v'] '\v',['f'] '\f',['r'] '\r'};
#else
static char c_escape[256];
#endif

static int
ac_3()
{
	register char c;

	p_stream++;

	if ((c = c_escape[*p_stream])) {
		/* escape */
		*p_token = c;
	} else {
		switch (*p_stream) {
		case '\n':
			strprg((char *) p_stream, 1);
			*p_token = *p_stream;
			break;
		case '0':
			if (*(p_stream + 1) == 'x' || *(p_stream + 1) == 'X')
				apg_flags |= APG_HEXT;
			else
				apg_flags |= APG_OCTT;

			*p_token = APG_ACK;
			p_stream--;
			break;
		default:
			*p_token = *p_stream;
			break;
		}
	}

	p_token++;
	p_stream++;
	return 1;
}

static int
ac_4()
{
	char *eptr;

	p_stream--;

	if ((eptr = strchr((const char *) p_stream, '\n')) != NULL)
		*eptr = '\0';

	while (*p_stream != '\n' && (char *) p_stream > (char *) file_image)
		p_stream--;

	fatalerr("%s:%d: parse error near -> \"%s\" ", file_name, apg_buffline + 1, ++p_stream);
	return 1;
}

static const short apg_mealy_state_table[9][8] = {
	{1, 4, 6, 6, 6, 6, 0, 0},
	{1, 6, 2, 6, 6, 6, 7, 6},
	{3, 4, 6, 2, 5, 3, 2, 0},
	{3, 4, 6, 2, 5, 3, 8, 0},
	{4, 4, 4, 4, 4, 4, 4, 0},
	{5, 5, 5, 5, 3, 5, 5, 0},
	{6, 6, 6, 6, 6, 6, 6, 6},
	{6, 6, 2, 6, 6, 6, 7, 6},
	{6, 4, 6, 2, 6, 6, 8, 0}};

static int (*apg_mealy_action_table[9][8]) () =
{
	{ ac_1, ac_0, ac_4, ac_4, ac_4, ac_4, ac_0, ac_0 }, 
	{ ac_1, ac_4, ac_2, ac_4, ac_4, ac_4, ac_0, ac_4 },
	{ ac_1, ac_2, ac_4, ac_2, ac_0, ac_3, ac_0, ac_2 },
	{ ac_1, ac_2, ac_4, ac_2, ac_0, ac_3, ac_0, ac_2 },
	{ ac_0, ac_0, ac_0, ac_0, ac_0, ac_0, ac_0, ac_0 },
	{ ac_1, ac_1, ac_1, ac_1, ac_0, ac_3, ac_1, ac_2 },
	{ ac_4, ac_4, ac_4, ac_4, ac_4, ac_4, ac_0, ac_4 },
	{ ac_4, ac_4, ac_2, ac_4, ac_4, ac_4, ac_0, ac_4 },
	{ ac_4, ac_2, ac_4, ac_2, ac_4, ac_4, ac_0, ac_2 }
};

static char *
get_token()
{
	register int reg_input;
	register int reg_state;
	u_char *c_token;

	c_token = p_token;
	apg_currentline = apg_buffline;

	/*** load registers ***/

	reg_state = apg_mealy_state_table[apg_statecode][apg_inputcode];
	reg_input = c_index[*p_stream];

	while ((*apg_mealy_action_table[reg_state][reg_input]) ()) {
		if (*p_stream == 0)
			return (char *) NULL;	/* EOF */

		if ((reg_state = apg_mealy_state_table[reg_state][reg_input]) == 1)
			apg_tokenid = 0;

		reg_input = c_index[*p_stream];
	}

	/*** save registers ***/

	apg_inputcode = reg_input;
	apg_statecode = reg_state;

#ifdef APG_PERF
	apg_global++;
#endif

	return (char *) c_token;
}

/* arena menagement */

#line 520 "/usr/local/share/apg/apg.db"
static grill_t *
alloc_segment(grill_t * p)
{
	grill_t *q;

	/* p reasonably points to the last grill_t. In case it doesn't ... */
	for (q = p; q != NULL; q = q->next)
		p = q;

	/* we must be sure the segment is empty: xcalloc() is required then. */
	q = (grill_t *) xcalloc(1, sizeof(grill_t));

	if (p == NULL)
		return (g_arena = g_stream = q);
	else
		return (g_stream = p->next = q);
}

#define FNV32_prime 16777619UL
#define FNV32_init  2166136261UL

static
#ifdef __GNUC__
__inline
#else
#endif
unsigned long
fnv(char *p)
{
	unsigned long h = FNV32_init;
	int i;

	for (i = 0; p[i] != 0; i++) {
		h = h * FNV32_prime;
		h = h ^ p[i];
	}

	return h;
}

static
#ifdef __GNUC__
       __inline
#else
#endif
int
b_search(char * key)
{
	register int high;
	register int low;
	register int i;

	unsigned long hash;

	hash = fnv(key);

	for (low = -1, high = QMAX_ELEM + 1; high - low > 1;) {
		i = (high + low) >> 1;
		if (hash <= line_v[i].hash)
			high = i;
		else
			low = i;
	}

	if (hash == line_v[high].hash)
		return (high);

	fatalerr("%s:%d: `%s' unknown line label", file_name, apg_buffline + 1, key);

	return (-1);		/* unreachable */

}

/* token err interface */

#line 594 "/usr/local/share/apg/apg.db"
#define PREFIX1_	stderr, "%s:%d: label=%s,token=%d {%s}"	/* label & token */
#define PREFIX2_     	stderr, "%s: label=%s"  		/* label only */
#define ARGFIX1_	file_name, apg_currentline + 1, line_v[l_id].id, t_id, token
#define ARGFIX2_	file_name, line_v[l_id].id

static void
token_fatalerr(int l_id, int t_id, int type, int errn0, int low, int high, char * token)
    {
	char *p;

	p = token;

	while (p && (p = strchr(p, APG_ACK)))
		*p = '\\';

	switch (errn0) {
	case APG_ETYPE:
		fprintf(PREFIX1_ " is designed to be a %s type;\n", ARGFIX1_ , types_id[type]);
		break;

	case APG_ENULL:
		fprintf(PREFIX1_ " isn't an optional argument;\n", ARGFIX1_);
		break;

	case APG_EOFFSET:
		fprintf(PREFIX1_ " too many tokens;\n", ARGFIX1_);
		break;

	case APG_ELIMIT:
		fprintf(PREFIX1_ " is designed to be a %s[%d,%d]. Out of range;\n", ARGFIX1_, types_id[type], low, high);
		break;

	case APG_EESC:
		fprintf(PREFIX1_ " bad escape sequence;\n", ARGFIX1_);
		break;

	case APG_EMANY:
		fprintf(PREFIX2_ " is designed to occur no more than %d time;\n", ARGFIX2_, high);
		break;

	case APG_EFEW:
		fprintf(PREFIX2_ " is designed to occur at least %d time;\n", ARGFIX2_, low);
		break;
#line 638 "/usr/local/share/apg/apg.db"
	case APG_EIPV4:
		fprintf(PREFIX2_ " is designed to be a valid ipv4;\n", ARGFIX2_);
		break;
#line 642 "/usr/local/share/apg/apg.db"
	case APG_EHOST:
		fprintf(PREFIX2_ " is designed to be a valid host;\n", ARGFIX2_);
		break;
#line 646 "/usr/local/share/apg/apg.db"
	}

	apg_flags |= APG_FATAL;
}

/* ymalloc & seg_t */

#line 652 "/usr/local/share/apg/apg.db"
static void
alloc_seg_t(char * r)
{
	seg_t *p;
	seg_t *q;

	p = tail_seg;

	q = (seg_t *) xmalloc(sizeof(seg_t));
	q->ptr = r;
	q->next = NULL;

	if (p != NULL)
		p->next = tail_seg = q;
	else {
		head_seg = q;
		tail_seg = q;
	}
}

static void *
ymalloc(size_t s)
{
	void *p;

	p = (void *) xmalloc(s);
	alloc_seg_t((char *) p);

	return p;
}

/* strings function */

#line 684 "/usr/local/share/apg/apg.db"
#ifdef __GNUC__
    __inline
#else
#endif
static int
strholen(char * p)
{
	register int c;
	register int s;
	register char *q;

	c = 1;
	s = 0;
	q = p;

	if (*q++ != '0')
		return 0;

	if (*q == 'x' || *q == 'X') {
		c++;
		q++;
		while (isxdigit(*q++) && c < 4)
			c++;
	} else {
		if ((s = O_DIGIT(*q))) {
			q++;
			c++;
			while (O_DIGIT(*q) && c < (2 + s)) {
				q++;
				c++;
			}
		}
	}
	return c;
}

/* apg type checks */

#line 721 "/usr/local/share/apg/apg.db"
#define APG_PROC_OBJECT(p) ( *p == APG_ACK ? (p+1) : (p)  )

#define LTYPE(l,t)    apgtb[l][t][0]
#define LLOW(l,t)     apgtb[l][t][1]
#define LHIGH(l,t)    apgtb[l][t][2]
#define LOPT(l,t)     apgtb[l][t][3]
#define LREGEX(l,t)   apgtb[l][t][4]

#define EARG_0	l_id, t_id, LTYPE (l_id, t_id), APG_EOFFSET, LLOW (l_id, t_id), LHIGH (l_id, t_id), token
#define EARG_1 	l_id, t_id, LTYPE (l_id, t_id), APG_ELIMIT,  LLOW (l_id, t_id), LHIGH (l_id, t_id), token
#define EARG_2 	l_id, t_id, LTYPE (l_id, t_id), APG_ETYPE,   LLOW (l_id, t_id), LHIGH (l_id, t_id), token
#define EARG_3 	l_id, t_id, T_STR, APG_EESC, 0, 0, token
#define EARG_4 	l_id, t_id, T_HOST, APG_EHOST, LLOW (l_id, t_id), LHIGH (l_id, t_id), token
#define EARG_5 	l_id, t_id, T_IPV4, APG_EIPV4, LLOW (l_id, t_id), LHIGH (l_id, t_id), token
#define EARG_6 	g_stream->type, j, LTYPE (g_stream->type, j), APG_ENULL, LLOW (g_stream->type, j), LHIGH (g_stream->type, j), NULL
#define EARG_7 	g_stream->type, 0, 0, APG_EMANY, 0, rep_limits[g_stream->type][1], NULL
#define EARG_8 	i, 0, 0, APG_EFEW, rep_limits[i][0], 0, NULL

static void
token_analysis(char * token, int l_id, int t_id)
{
	char *endptr;
	char *pp;
	long sp;
	int offset;

	pp = (char *) NULL;
	offset = 0;

	/* first step */

	if ((offset = apg_offset[l_id][t_id]) == -1)
		token_fatalerr(EARG_0);

	if (token != NULL && *token == '\0')
		return;		/* NULL token */

	switch (LTYPE(l_id, t_id)) {
	case T_STR:
	case T_HOST:
	case T_IPV4:
		pp = (char *) ymalloc(strlen(token) + 1);
		break;
	default:
		token = APG_PROC_OBJECT(token);
		break;
	}

	switch (LTYPE(l_id, t_id)) {
#line 771 "/usr/local/share/apg/apg.db"
	case T_INT:
	case T_U_32:
	case T_SHORT:
	case T_U_16:
	case T_CHAR:
	case T_U_8:
		if (LTYPE(l_id, t_id) == T_CHAR && strlen(token) == 1) {
			O_PUSH(g_stream, offset, token, char);
			return;
		}
		sp = M_STRTOL(token, &endptr);
		if (*endptr == '\0') {
			if (C_LIMIT(LTYPE(l_id, t_id), LLOW(l_id, t_id), sp, LHIGH(l_id, t_id))) {
				switch (LTYPE(l_id, t_id)) {
				case T_INT:
					O_PUSH(g_stream, offset, &sp, int);
					break;
				case T_U_32:
					O_PUSH(g_stream, offset, &sp, u_int);
					break;
				case T_SHORT:
					O_PUSH(g_stream, offset, &sp, short);
					break;
				case T_U_16:
					O_PUSH(g_stream, offset, &sp, u_short);
					break;
				case T_CHAR:
					O_PUSH(g_stream, offset, &sp, char);
					break;
				case T_U_8:
					O_PUSH(g_stream, offset, &sp, u_char);
					break;
				}
				return;
			} else
				token_fatalerr(EARG_1);
		} else
			token_fatalerr(EARG_2);

		return;
		break;

#line 814 "/usr/local/share/apg/apg.db"
	case T_STR:
		{
			strcpy(pp, token);
			if (strchr(token, APG_ACK)) {
				char *ptr = pp, bufftemp[6] = "";
				while ((ptr = (char *) strchr(ptr, APG_ACK))) {
					int i;

					i = strholen(ptr + 1);

					strncpy(bufftemp, ptr + 1, i);
					bufftemp[i] = 0;

					*ptr = (char) strtol(bufftemp, NULL, 0);

					if (*ptr == '\0' || i == 0)
						token_fatalerr(EARG_3);

					strprg((char *) ptr + 1, i);
					ptr++;

				}
		}
#line 863 "/usr/local/share/apg/apg.db"
		if (!(LLOW(l_id, t_id) || 
			LHIGH(l_id, t_id)) || 
			C_LIMIT(LTYPE(l_id, t_id), LLOW(l_id, t_id), (int) strlen(pp), LHIGH(l_id, t_id))) {
			P_PUSH(g_stream, offset, pp);
			return;
		}
		else
			token_fatalerr(EARG_1);

		return;
	}
	break;

/* host/ipv4 regmacro */

#line 877 "/usr/local/share/apg/apg.db"
#define HALPHA    	0x01
#define HDOT      	0x02
#define HMIN      	0x04
#define HERR      	0x08
#define H2LD      	0x10
#define HL_ALPHA  	0x20

#define R09(x)      	( '0' <= x && x <= '9' )
#define RAZ(x)      	( 'A' <= x && x <= 'Z' )
#define Raz(x)      	( 'a' <= x && x <= 'z' )
#define RALPHANUM(x) 	( R09(x) || RAZ(x) || Raz(x) )
#define RALPHA(x)    	( RAZ(x) || Raz(x) )
#line 890 "/usr/local/share/apg/apg.db"
	case T_HOST:
		{
			register int state;
			register int t_counter;
			char *q;

			state = HDOT;
			t_counter = 1;
			q = (char *) token;

			while (*q != '\0') {
				switch (*q) {
				case '.':
					if (state & (HDOT | HMIN))
						state |= HERR;
					state |= (HDOT | H2LD);
					state &= ~(HL_ALPHA | HMIN);
					t_counter++;
					break;
				case '-':
					if (state & (HDOT | HMIN))
						state |= HERR;
					state |= HMIN;
					break;
				default:
					state &= ~(HDOT | HMIN);
					if (!RALPHANUM(*q))
						state |= HERR;
					if (RALPHA(*q))
						state |= (HALPHA | HL_ALPHA);
					break;
				}
				q++;
			}

			if (*q != '\0' || state & (HERR | HDOT | HMIN) || ~state & H2LD || (state & HALPHA && ~state & HL_ALPHA)) {
				token_fatalerr(EARG_4);
				return;
			} else if (state & HALPHA) {
				strcpy(pp, token);
				P_PUSH(g_stream, offset, pp);
				return;
			}
		}
#line 935 "/usr/local/share/apg/apg.db"
	case T_IPV4:
		    {
			register int state, t_counter, t_byte;
			char *q;

			state = HDOT;
			t_counter = 1;
			t_byte = 0;
			q = (char *) token;

			while (*q != '\0') {
				switch (*q) {
				case '.':
					if (state & HDOT)
						state |= HERR;
					state |= HDOT;
					t_counter++;
					t_byte = 0;
					break;
				default:
					state &= ~HDOT;
					if (!R09(*q))
						state |= HERR;

					/* t_byte = t_byte * 10 + val(*q) */
					t_byte = (t_byte << 3) + (t_byte << 1) + (*q - '0');
					if (t_byte & 0xffffff00U)
						state |= HERR;
					break;
				}
				q++;
			}

			if (*q != '\0' || state & (HERR | HDOT) || t_counter != 4) {
				token_fatalerr(EARG_5);
				return;
			} else {
				strcpy(pp, token);
				P_PUSH(g_stream, offset, pp);
				return;
			}
		}
		break;
#line 979 "/usr/local/share/apg/apg.db"
	}
	return;
}

/* error api */

#line 984 "/usr/local/share/apg/apg.db"
static char *err_table[] = { 
"apg: ok", "apg: grill arena is empty", "apg: NULL pointer", "apg: end of grill",
};

char *
apg_strerror(int errnum)
{
	static char *sb;
	int i;

	i = errnum;
	if (errnum < 1 || errnum > APG_MAXERR)
		i = 0;

	sb = (char *) xrealloc(sb, strlen(err_table[i]) + 1);
	strcpy(sb, err_table[i]);
	return sb;
}

/* standard api */

#line 1004 "/usr/local/share/apg/apg.db"
void
apg_free_grill(grill_t * p)
{
	grill_t *s;

	if (p == NULL)
		p = g_arena;

	while (p != NULL) {
		s = p->next;
		free(p);
		p = s;
	}
}

void
apg_free_pragma()
{
	register seg_t *p, *q;

	q = head_seg;

	while ((p = q)) {	/* its not a BUG */
		if (p->ptr != NULL)
			free(p->ptr);
		q = p->next;
		free(p);
	}
	head_seg = NULL;
}


int
apg_get_line(grill_t ** apg_user_ptr)
{
	apg_errno = APG_EOK;

	if (!(apg_flags & APG_PARSED)) {
		apg_errno = APG_EEMPTY;
		return 0;
	}
	if (*apg_user_ptr == '\0') {
		apg_errno = APG_ENULL;
		return 0;
	}
	if (!(apg_flags & APG_RESET)) {
		apg_flags |= APG_RESET;
		g_stream = *apg_user_ptr;
		return g_stream->type;
	}
	/* check if g_stream != NULL */

	if (g_stream != NULL && (*apg_user_ptr = g_stream = g_stream->next))
		return g_stream->type;
	else {
		apg_errno = APG_EEOG;
		return 0;
	}

}

/* parser */

#line 1106 "/usr/local/share/apg/apg.db"
grill_t *
#if __STDC__
apg_parser(int q,...)
#else
apg_parser(q, va_alist)
int q;
va_dcl
#endif
{
	char *tk, *file, *b_stream;
	register long shift_reg;
	va_list ap;
	int i;
	int j;
#line 1126 "/usr/local/share/apg/apg.db"
        b_stream = tk = NULL;
	shift_reg = 0;
	i = q;
	j = 0;

#if __STDC__
	va_start(ap, q);
#else
	va_start(ap);
#endif
	/* cleaning apg_flags */
	apg_flags = 0;
	apg_errno = APG_EOK;
	file = va_arg(ap, char *);

	while (i-- && (b_stream = mmap_ctrl(file, APG_OPEN)) == NULL)
		file = va_arg(ap, char *);

	if (b_stream == NULL) {
		i = q;
		va_start(ap, q);
		fprintf(stderr, "err: ");

		while (i--)
			fprintf(stderr, "%s ", va_arg(ap, char *));

		fprintf(stderr, "; no such file[s] to parse; exit forced.\n");
		exit(1);
	}
	va_end(ap);

/* ansicare vector setup */
#if !defined (__GNUC__) || defined (__STRICT_ANSI__) || defined (__ANSI__) || defined (__cplusplus)
	c_index['#'] = 1;
	c_index[APG_SEPLINE] = 2;
	c_index[APG_SEPTOKEN] = 3;
	c_index['\''] = 4;
	c_index['\\'] = 5;
	c_index[' '] = 6;
	c_index['\t'] = 6;
	c_index['\n'] = 7;

	c_escape['a'] = '\a';
	c_escape['b'] = '\b';
	c_escape['t'] = '\t';
	c_escape['n'] = '\n';
	c_escape['v'] = '\v';
	c_escape['f'] = '\f';
	c_escape['r'] = '\r';
#endif

	/* apg get token setup */
	if (p_token == NULL) {
		b_token = p_token = (u_char *) xcalloc(apg_stream_len, sizeof(char));
		p_stream = (u_char *) b_stream;
		apg_inputcode = c_index[*p_stream];
	}

	while ((tk = get_token())) {
		switch (apg_tokenid) {
		case 0:	/* null */
			break;
		case 1:	/* label line */
			/*
			 * before parsing the new line, we check the shift
			 * register in order to detect which token can be
			 * NULL
			 */
			for (j = 1; g_stream != NULL && j < APG_MAXARG; j++)
				if (LOPT(g_stream->type, j) && ~shift_reg & (1 << j))
					token_fatalerr(EARG_6);

			shift_reg = 0;	/* clear */
			g_stream = alloc_segment(g_stream);
			g_stream->type = b_search(tk);

/* man rep */

#line 1203 "/usr/local/share/apg/apg.db"
			rep_counter[g_stream->type]++;
			if (rep_limits[g_stream->type][1] != -1 && rep_counter[g_stream->type] > rep_limits[g_stream->type][1])
				token_fatalerr(EARG_7);

#line 1208 "/usr/local/share/apg/apg.db"
			break;
		default:	/* token */

			/* shift_reg setup: (apg_tokenid == 1 ? l_id : t_id) */
			if (*tk != '\0')
				shift_reg |= 1 << (apg_tokenid - 1);

			token_analysis(tk, g_stream->type, apg_tokenid - 1);
			break;
		}
	}

	/* now we check the last line */
	for (j = 1; g_stream != NULL && j < APG_MAXARG; j++)
		if (LOPT(g_stream->type, j) && ~shift_reg & (1 << j))
			token_fatalerr(EARG_6);


/* min rep */

#line 1227 "/usr/local/share/apg/apg.db"
	{ 	register int i;

		for (i = 1; i <= QMAX_ELEM; i++)
			if (rep_limits[i][0] != -1 && rep_counter[i] < rep_limits[i][0])
				token_fatalerr(EARG_8);
	}

#line 1235 "/usr/local/share/apg/apg.db"
	free(b_token);
	mmap_ctrl(NULL, APG_CLOSE);

	apg_flags |= APG_PARSED;

	if (apg_flags & APG_FATAL)
		fatalerr("apg_parser(): encontered some errors");

#line 1260 "/usr/local/share/apg/apg.db"

	    return g_arena;
}


