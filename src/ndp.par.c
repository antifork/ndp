
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

/* standard includes
*/

#define APG_PARSER_C

#include "ndp.tab.h"
/* mealy prototypes
*/

static int _ac_0 (unsigned char **, unsigned char **, unsigned char **);
static int _ac_1 (unsigned char **, unsigned char **, unsigned char **);
static int _ac_2 (unsigned char **, unsigned char **, unsigned char **);
static int _ac_3 (unsigned char **, unsigned char **, unsigned char **);
static int _ac_4 (unsigned char **, unsigned char **, unsigned char **);

/* static variables
*/

static char *file_name;
static int apg_flags, apg_buff_line, apg_token;
static int apg_state_code, apg_input_code;
static unsigned char *token_buff;
static grill_t *apg_stream;
static grill_t *apg_arena;
static seg_t *root_seg;
static seg_t *head_seg;

int apg_errno;

/* common defines and internal flags
*/

#define   _APG_OPEN              0
#define   _APG_CLOSE             1

#define   _ACK_           	0x06

/* internal flags */

#define _APG_PARSED_GRILL     0x01	/* prevent cycles in apg_get_line(); */
#define _APG_RESET_STREAM     0x02
#define _APG_OCT_TOKEN        0x04
#define _APG_HEX_TOKEN        0x08

/* msg error codes */

#define _APG_TYPE_ERR   0x01
#define _APG_NULL_ERR	0x02
#define _APG_OFFSET_ERR 0x03
#define _APG_BOUND_ERR  0x04
#define _APG_ESC_ERR    0x05
#define _APG_IPV4_ERR   0x06
#define _APG_HOST_ERR   0x07

/* macros
*/

int c_index[256] =
  {['#'] 1,['='] 2,[':'] 3,['\''] 4,['\\'] 5,[' '] 6,['\t'] 6,['\n'] 7 };

#define AUTO_STRTOL(token,addr_endptr) ( apg_flags & \
(_APG_OCT_TOKEN|_APG_HEX_TOKEN) ? strtol(token,addr_endptr,0) : \
strtol(token,addr_endptr,10))

#define C_BOUND(x,v,y) ( (x|y) ? ( (x<=v) && (v<=y) ) : (1) )

#define O_PUSH(base,offset,buff,type) ( *(type *)((char *)base+offset+\
SIZEOF_INT)= *(type *)buff )

#define P_PUSH(base,offset,ptr) (*(long *)((char *)base+offset+\
SIZEOF_INT)=(long)ptr)

#define ISODIGIT(x) ( '0' <= x && x <= '3' ? 2 : ( '4' <= x && x <= '7' ? 1 : 0 ) )

/* private functions, mealy fun & table
*/

static void
fatalerr (char *pattern, ...)
{
  va_list ap;
  va_start (ap, pattern);
  vfprintf (stderr, pattern, ap);
  fprintf (stderr, "; exit forced.\n");
  va_end (ap);
  exit (-1);

}

static char *
ioctl_buffer (char *fn, int flag)
{
  int fd;
  struct stat lstat;

  static char *static_ptr;


  switch (flag)
    {
    case _APG_OPEN:

      file_name = (char *) malloc (strlen (fn) + 1);
      strcpy (file_name, fn);

      if (stat (fn, &lstat) == -1)
	return (char *) NULL;

      if ((static_ptr = (char *) realloc (static_ptr, lstat.st_size + 1)) ==
	  NULL)
	fatalerr ("err: %s", strerror (errno));

      if ((fd = open (fn, O_RDONLY)) == -1)
	fatalerr ("err: %s", strerror (errno));
      read (fd, static_ptr, lstat.st_size);
      *(static_ptr + lstat.st_size) = 0;

      close (fd);

      return static_ptr;

      break;
    case _APG_CLOSE:

      if (file_name)
	free (file_name);
      if (static_ptr == NULL)
	fatalerr ("err: apg_buffer already closed");

      free (static_ptr);

      return static_ptr = (char *) NULL;
      break;
    }

  return (char *) NULL;		/* unreachable */
}


static int
_ac_0 (unsigned char **tok, unsigned char **tok_0, unsigned char **strm)
{
  if (**strm == '\n')
    apg_buff_line++;
  (*strm)++;
  return 1;
}

static int
_ac_1 (unsigned char **tok, unsigned char **tok_0, unsigned char **strm)
{
  *((*tok_0)++) = *((*strm)++);
  return 1;
}

static int
_ac_2 (unsigned char **tok, unsigned char **tok_0, unsigned char **strm)
{
  if (**strm == '\n')
    apg_buff_line++;
  *((*tok_0)++) = '\0';
  (*strm)++;
  apg_token++;
  return 0;
}

char escape_chars[256] =
  {['a'] '\a',['b'] '\b',['t'] '\t',['n'] '\n',['v'] '\v',['f'] '\f',
    ['r'] '\r' };

static int
_ac_3 (unsigned char **tok, unsigned char **tok_0, unsigned char **strm)
{
  register char c;
  (*strm)++;

  if ((c = escape_chars[**strm]))
    {
      /* escape */
      **tok_0 = c;
    }
  else
    {
      switch (**strm)
	{
	case '\n':
	  memmove (*strm, *strm + 1, strlen (*strm + 1));
	  **tok_0 = **strm;
	  break;
	case '0':
	  if (*(*strm + 1) == 'x' || *(*strm + 1) == 'X')
	    apg_flags |= _APG_HEX_TOKEN;
	  else
	    apg_flags |= _APG_OCT_TOKEN;

	  **tok_0 = _ACK_;
	  (*strm)--;
	  break;

	default:
	  **tok_0 = **strm;
	  break;

	}

    }

  (*tok_0)++;
  (*strm)++;
  return 1;

}

static int
_ac_4 (unsigned char **tok, unsigned char **tok_0, unsigned char **strm)
{
  char *eptr;

  apg_buff_line++;
  (*strm)--;
  if ((eptr = strchr (*strm, '\n')) != NULL)
    *eptr = '\0';
  fatalerr ("%s:%d: parse error near -> \"%s\" ", file_name, apg_buff_line,
	    *strm);

  return 1;
}

static const short apg_mealy_state_table[9][8] = {
  {1, 4, 6, 6, 6, 6, 0, 0},
  {1, 6, 2, 6, 6, 6, 7, 6},
  {3, 6, 6, 6, 5, 3, 2, 6},
  {3, 4, 6, 2, 5, 3, 8, 0},
  {4, 4, 4, 4, 4, 4, 4, 0},
  {5, 5, 5, 5, 3, 5, 5, 0},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {6, 6, 2, 6, 6, 6, 7, 6},
  {6, 4, 6, 2, 6, 6, 8, 0}
};

static int (*apg_mealy_action_table[9][8]) (unsigned char **,
					    unsigned char **,
					    unsigned char **) =
{
  { _ac_1, _ac_0, _ac_4, _ac_4, _ac_4, _ac_4, _ac_0, _ac_0} ,
  { _ac_1, _ac_4, _ac_2, _ac_4, _ac_4, _ac_4, _ac_0, _ac_4} ,
  { _ac_1, _ac_4, _ac_4, _ac_4, _ac_0, _ac_3, _ac_0, _ac_4} ,
  { _ac_1, _ac_2, _ac_4, _ac_2, _ac_0, _ac_3, _ac_0, _ac_2} ,
  { _ac_0, _ac_0, _ac_0, _ac_0, _ac_0, _ac_0, _ac_0, _ac_0} ,
  { _ac_1, _ac_1, _ac_1, _ac_1, _ac_0, _ac_3, _ac_1, _ac_2} ,
  { _ac_4, _ac_4, _ac_4, _ac_4, _ac_4, _ac_4, _ac_0, _ac_4} ,
  { _ac_4, _ac_4, _ac_2, _ac_4, _ac_4, _ac_4, _ac_0, _ac_4} ,
  { _ac_4, _ac_2, _ac_4, _ac_2, _ac_4, _ac_4, _ac_0, _ac_2} };

static char *
get_token (char *ptr)
{
  register int reg_input, reg_state;

  static unsigned char *token, *token_0;
  static unsigned char *stream;

  if (token == NULL)
    {
      if (
	  (token_buff = token = token_0 =
	   (char *) calloc (strlen (ptr), sizeof (char))) == NULL)
	fatalerr ("fatalerr: %s", strerror (errno));

      stream = ptr;
      reg_input = apg_input_code = c_index[*stream];
    }

  token = token_0;

	/*** loading registers ***/

  reg_state = apg_mealy_state_table[apg_state_code][apg_input_code];
  reg_input = c_index[*stream];

  while ((*apg_mealy_action_table[reg_state][reg_input])
	 (&token, &token_0, &stream))
    {
      if (!*stream)
	return (char *) NULL;

      if ((reg_state = apg_mealy_state_table[reg_state][reg_input]) == 1)
	apg_token = 0;

      reg_input = c_index[*stream];
    }

#ifdef APG_DEBUG
  (token ? printf ("[%s] token=%d\n", token, apg_token) : (printf ("EOF\n")));
#endif

	/*** saving registers ***/

  apg_input_code = reg_input;
  apg_state_code = reg_state;

  return token;

}

/* arena menagement
*/

static grill_t *
alloc_segment (grill_t * p_ptr)
{

  grill_t *q_ptr;

#ifdef APG_DEBUG
  fprintf (stderr, "new segment allocation\n");
#endif
  for (q_ptr = p_ptr; q_ptr != NULL; q_ptr = q_ptr->next)
    p_ptr = q_ptr;
  q_ptr = (grill_t *) calloc (1, sizeof (grill_t));

  if (p_ptr == NULL)
    return (apg_arena = apg_stream = q_ptr);
  else
    return (apg_stream = p_ptr->next = q_ptr);

}

static int
get_lineid (char *frame)
{
  register int i = 1, fd = 1;

  while (line_list[i] && fd)
    fd = strcmp (line_list[i++], frame);

  if (fd)
    fatalerr ("%s:%d: `%s' unknown line label",
	      file_name, apg_buff_line + 1, frame);

  return (i - 1);

}

/* token err interface
*/

static void
token_fatalerr (int line_id, int token_id, int type, int errn0, int low,
		 int high, char *token)
{
  char *p = token;


  while ((p = strchr (p, _ACK_)))
    *p = '\\';

  switch (errn0)
    {
    case _APG_TYPE_ERR:
      fprintf (stderr,
	       "%s:%d: label=%s,token=%d -> {%s} is designed to be a %s type; exit forced.\n",
	       file_name, apg_buff_line + 1, line_list[line_id], token_id,
	       token, types_id[type]);
      break;
    case _APG_NULL_ERR:
      fprintf (stderr,
               "%s:%d: label=%s,token=%d -> {%s} isn't an optional argument; exit forced.\n",
               file_name, apg_buff_line + 1, line_list[line_id], token_id,
               token, types_id[type]);

      break;
    case _APG_OFFSET_ERR:
            fprintf (stderr, 
		    "%s:%d: label=%s,token=%d -> {%s} too many tokens; exit forced.\n",
	            file_name, apg_buff_line + 1, line_list[line_id], token_id,
	            token);

      break;
    case _APG_BOUND_ERR:
      fprintf (stderr,
	       "%s:%d: label=%s,token=%d -> {%s} is designed to be a %s[%d,%d]. Out of range; exit forced.\n",
	       file_name, apg_buff_line + 1, line_list[line_id], token_id,
	       token, types_id[type], low, high);

      break;
    case _APG_ESC_ERR:
      fprintf (stderr,
	       "%s:%d: label=%s,token=%d -> {%s} bad escape sequence; exit forced.\n",
	       file_name, apg_buff_line + 1, line_list[line_id], token_id,
	       token);
      break;

    case _APG_IPV4_ERR:
      fprintf (stderr,
	       "%s:%d: label=%s,token=%d -> {%s} is designed to be a valid ipv4; exit forced.\n",
	       file_name, apg_buff_line + 1, line_list[line_id], token_id,
	       token);

      break;


    case _APG_HOST_ERR:
      fprintf (stderr,
	       "%s:%d: label=%s,token=%d -> {%s} is designed to be a valid host; exit forced.\n",
	       file_name, apg_buff_line + 1, line_list[line_id], token_id,
	       token);

      break;


    }
  exit (1);
}

/* strings function
*/

static void
add_segment (char *r)
{
  seg_t *p = head_seg, *new_p;

  new_p       = (seg_t *) malloc (sizeof (seg_t));
  new_p->ptr  = r;
  new_p->next = NULL;

  if (p)
    p->next = new_p = head_seg ;
  else
    root_seg = new_p;

}

static void *
ymalloc (size_t size)
{

  register void *p;
  p = (void *) malloc (size);
  add_segment (p);

  return p;

}


static int
strholen (char *p)
{
  register int c = 1, s = 0;
  register char *P = p;

  if (*P++ != '0')
    return 0;

  if (*P == 'x' || *P == 'X')
    {
      c++;
      P++;
      while (isxdigit (*P++) && c < 4)
	c++;
    }
  else
    {
      if ((s = ISODIGIT (*P)))
	{
	  P++;
	  c++;
	  while (ISODIGIT (*P) && c < (2 + s))
	    {
	      P++;
	      c++;
	    }
	}
    }
  return c;
}


static char *
proc_string (char *token, int line_id, int token_id)
{
  char *ptr, bufftemp[6] = "";
  static char *safe_token;

  safe_token = ptr = (char *) realloc (safe_token, strlen (token) + 1);
  strcpy (safe_token, token);

  while ((ptr = (char *) strchr (ptr, _ACK_)))
    {
      register int i = strholen (ptr + 1);
      register int j = strlen (ptr + i);

      strncpy (bufftemp, ptr + 1, i);
      bufftemp[i] = 0;
      *ptr = (char) strtol (bufftemp, NULL, 0);

      if (!*ptr || !i)
	token_fatalerr (line_id, token_id, T_STR, _APG_ESC_ERR, 0, 0, token);

      memmove (ptr + 1, ptr + i + 1, j);
      *(ptr + j + 1) = 0;
      ptr++;

    }
  return safe_token;

}

/* apg type checks
*/

#define PROC_OBJECT(p) ( *p == _ACK_ ? (p+1) : (p)  )

#define L_TYPE(l,t)    apgtb[l][t][0]
#define L_LOW(l,t)     apgtb[l][t][1] 
#define L_HIGH(l,t)    apgtb[l][t][2] 
#define L_REGEX(l,t)   apgtb[l][t][3] 

static void
token_analysis (char *token, int line_id, int token_id)
{
  char *endptr, *pp=NULL;
  static void *sp;
  int offset = 0;

  /* first step */
  
  if( (offset= apg_offset[line_id][token_id])== -1)
      token_fatalerr (line_id, token_id, L_TYPE (line_id,token_id), _APG_OFFSET_ERR,
                                   L_LOW (line_id,token_id), L_HIGH (line_id,token_id), token);

  if (token && !*token)
    {
      /* (NULL) token */
      
      if ( L_LOW(line_id,token_id) )
       token_fatalerr (line_id, token_id, L_TYPE (line_id,token_id), _APG_NULL_ERR,
                                   L_LOW (line_id,token_id), L_HIGH (line_id,token_id), token);
			
    return;

    }

    switch (L_TYPE (line_id,token_id))
      {
      case T_STR:
      case T_HOST:
      case T_IPV4:

	pp = (char *) ymalloc (strlen (token) + 1);
	break;

      default:
	token = PROC_OBJECT (token);
	sp = (void *) realloc (sp, sizeof (long));
	break;
      }

    switch (L_TYPE (line_id,token_id))
      {


      case T_INT:
      case T_U_32:
      case T_SHORT:
      case T_U_16:
      case T_CHAR:
      case T_U_8:

	if (L_TYPE (line_id,token_id) == T_CHAR && strlen (token) == 1)
	  {
	    O_PUSH (apg_stream, offset, token, char);
	    return;
	  }

	*(long *) sp = AUTO_STRTOL (token, &endptr);

	if (!*endptr)
	  {
	    if (C_BOUND (L_LOW (line_id,token_id), *(long *) sp, L_HIGH (line_id,token_id)))
	      {
		switch (L_TYPE (line_id,token_id))
		  {
		  case T_INT:
		    O_PUSH (apg_stream, offset, sp, int);
		    break;
		  case T_U_32:
		    O_PUSH (apg_stream, offset, sp, u_int);
		    break;
		  case T_SHORT:
		    O_PUSH (apg_stream, offset, sp, short);
		    break;
		  case T_U_16:
		    O_PUSH (apg_stream, offset, sp, u_short);
		    break;
		  case T_CHAR:
		    O_PUSH (apg_stream, offset, sp, char);
		    break;
		  case T_U_8:
		    O_PUSH (apg_stream, offset, sp, u_char);
		    break;
		  }
		return;
	      }
	    else
	      token_fatalerr (line_id, token_id, L_TYPE (line_id,token_id), _APG_BOUND_ERR,
			       L_LOW (line_id,token_id), L_HIGH (line_id,token_id), token);
	  }
	else
	  token_fatalerr (line_id, token_id, L_TYPE (line_id,token_id), _APG_TYPE_ERR,
			   L_LOW (line_id,token_id), L_HIGH (line_id,token_id), token);
	return;
	break;


      case T_STR:
	{
	  char *new_token = proc_string (token, line_id, token_id);


	  if (!(L_LOW (line_id,token_id) || L_HIGH (line_id,token_id))
	      || C_BOUND (L_LOW (line_id,token_id), strlen (new_token), L_HIGH (line_id,token_id)))
	    {
	      strcpy (pp, new_token);
	      P_PUSH (apg_stream, offset, pp);
	      return;

	    }
	  else
	    token_fatalerr (line_id, token_id, L_TYPE (line_id,token_id), _APG_BOUND_ERR,
			     L_LOW (line_id,token_id), L_HIGH (line_id,token_id), token);

	  return;
	}
	break;


      case T_HOST:
      case T_IPV4:

#define C_1(x) ( '0' <= x && x <= '9' )
#define C_2(x) ( 'A' <= x && x <= 'Z' )
#define C_3(x) ( 'a' <= x && x <= 'z' )
#define C_4(x) ( x == '.' )
#define C_5(x) ( x == '-' )
#define C_$(x) ( C_1(x) || C_2(x) || C_3(x) || C_4(x) || C_5(x) )
#define SCAN_IPV4(x,ip) (sscanf(x,"%u.%u.%u.%u",ip,ip+1,ip+2,ip+3))
#define CHECK_IPV4(ip)  ( ( \
(ip[0]<=255) && (ip[1]<=255) \
&& (ip[2]<=255) && (ip[3]<=255\
)) ?  (1) : (0) )

	{
	  unsigned int ipv4[4];
	  register char *i = token;

	  if (SCAN_IPV4 (token, ipv4) == 4 && CHECK_IPV4 (ipv4))
	    {
	      strcpy (pp, token);
	      P_PUSH (apg_stream, offset, pp);
	    }

	  else if (L_TYPE (line_id,token_id) == T_IPV4)
	    token_fatalerr (line_id, token_id, L_TYPE (line_id,token_id), _APG_IPV4_ERR,
			     L_LOW (line_id,token_id), L_HIGH (line_id,token_id), token);
	  else
	    {
	      while (*i != 0 && C_$ (*i))
		i++;

	      if (*i)
		token_fatalerr (line_id, token_id, L_TYPE (line_id,token_id),
				 _APG_HOST_ERR, L_LOW (line_id,token_id), L_HIGH (line_id,token_id),
				 token);
	      else
		{
		  strcpy (pp, token);
		  P_PUSH (apg_stream, offset, pp);
		  return;
		}
	    }
	}

	break;


  }
  return;
}

/* error api
*/

static char *err_table[] = {
  "apg: ok",
  "apg: grill arena is empty",
  "apg: NULL pointer",
  "apg: end of grill",

};

char *
apg_strerror (int errnum)
{
  static char *sb;
  register i = errnum;

  if (errnum < 1 || errnum > APG_MAXERR)
    i = 0;
  sb = (char *) realloc (sb, strlen (err_table[i]) + 1);
  strcpy (sb, err_table[i]);

  return sb;

}

/* standard api
*/

void
apg_free_grill (grill_t * p_ptr)
{
  grill_t *s_ptr;

  if (!p_ptr)
    p_ptr = apg_arena;

  while (p_ptr != NULL)
    {
      s_ptr = p_ptr->next;
      free (p_ptr);
      p_ptr = s_ptr;

    }
}

void
apg_free_pragma ()
{
  register seg_t *p = root_seg, *q = p;

  while (p = q)
    {
      if (p->ptr)
	free (p->ptr);
      q = p->next;
      free (p);
    }
  root_seg = NULL;
}

int
apg_get_line (grill_t ** apg_user_ptr)
{

  apg_errno = APG_EOK;

  if (!(apg_flags & _APG_PARSED_GRILL))
    {
      apg_errno = APG_EEMPTY;
      return 0;
    }

  if (!*apg_user_ptr)
    {
      apg_errno = APG_ENULL;
      return 0;
    }

  if (!(apg_flags & _APG_RESET_STREAM))
    {
      apg_flags |= _APG_RESET_STREAM;
      apg_stream = *apg_user_ptr;
      return apg_stream->type_line;
    }

  /* check if apg_stream != NULL */

  if (apg_stream && (*apg_user_ptr = apg_stream = apg_stream->next))
    return apg_stream->type_line;
  else
    {
      apg_errno = APG_EEOG;
      return 0;
    }

}

/* parser
*/

grill_t *
apg_parser (int q, ...)
{
  register int i = q;
  va_list ap;
  char *b_stream = NULL, *tk = NULL;
  char *file;

  va_start (ap, q);

  /* cleaning apg_flags */

  apg_flags = 0;
  apg_errno = APG_EOK;


  file = va_arg (ap, char *);

  while (i-- && !(b_stream = ioctl_buffer (file, _APG_OPEN)))
    file = va_arg (ap, char *);

  if (!b_stream)
    {
      i = q;
      va_start (ap, q);

      fprintf (stderr, "err: ");
      while (i--)
	fprintf (stderr, "%s ", va_arg (ap, char *));

      fprintf (stderr, "; no such file[s] to parse; exits forced.\n");

      exit (1);
    }

  va_end (ap);

  while ((tk = get_token (b_stream)))
    {

      if (apg_token == 1)
	{
	  /* label line */
	  apg_stream = alloc_segment (apg_stream);
	  apg_stream->type_line = get_lineid (tk);

/* man rep
*/

	  rep_counter[apg_stream->type_line]++;

	  if (rep_limits[apg_stream->type_line][1] != -1 &&
	      rep_counter[apg_stream->type_line] >
	      rep_limits[apg_stream->type_line][1])
	    fatalerr
	      ("%s:%d: label=%s is designed to occur no more than %d time[s]",
	       file_name, apg_buff_line + 1, line_list[apg_stream->type_line],
	       rep_limits[apg_stream->type_line][1]);


	}
      else
	/* token */
	token_analysis (tk, apg_stream->type_line, apg_token - 1);

    }

/* min rep
*/

  {
    register int i = 1;
    for (; line_list[i]; i++)
      if (rep_limits[i][0] != -1 && rep_counter[i] < rep_limits[i][0])
	fatalerr ("%s: label=%s is designed to occur al least %d time[s]",
		  file_name, line_list[i], rep_limits[i][0]);

  }


  free (token_buff);
  ioctl_buffer (NULL, _APG_CLOSE);

  apg_flags |= _APG_PARSED_GRILL;
  return apg_arena;

}


