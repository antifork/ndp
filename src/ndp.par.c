/* apg.db 01/02/03 rel 02 $Id$ */
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

#define _APG_PARSER_C
#include "ndp.tab.h"
/* mealy prototypes
*/

static int ac_0 P((void));
static int ac_1 P((void));
static int ac_2 P((void));
static int ac_3 P((void));
static int ac_4 P((void));

/* static variables
*/

static char *file_name;
static int apg_flags, apg_current_line, apg_buff_line, apg_token;
static int apg_state_code, apg_input_code;

#ifdef APG_PERF
static int global_token;
#endif

static unsigned char *token_basebuffer;

static char *file_image;

static grill_t *apg_stream;
static grill_t *apg_arena;
static seg_t *head_seg;
static seg_t *tail_seg;

int apg_errno;

/* common defines and internal flags
*/

#define APG_OPEN                0
#define APG_CLOSE               1

#define APG_ACK_CHR          0x06

/* token option */

#define TOKEN_REQUIRED	     0x01

/* internal flags */

#define APG_PARSED_GRILL     0x01	
#define APG_RESET_STREAM     0x02
#define APG_OCT_TOKEN        0x04
#define APG_HEX_TOKEN        0x08
#define APG_FATALERR	     0x10

/* msg error codes */

#define APG_TYPE_ERR         0x01
#define APG_NULL_ERR	     0x02
#define APG_OFFSET_ERR       0x03
#define APG_BOUND_ERR        0x04
#define APG_ESC_ERR          0x05
#define APG_FEW_ERR	     0x06
#define APG_MANY_ERR 	     0x07
#define APG_IPV4_ERR         0x08
#define APG_HOST_ERR         0x09

/* macros
*/

#if defined (__GNUC__) && !defined (__STRICT_ANSI__)
int c_index[256] =
  {['#'] 1,[APG_SEPLINE] 2,[APG_SEPTOKEN] 3,['\''] 4,['\\'] 5,[' '] 6,['\t'] 6,['\n'] 7 };
#else
int c_index[256];
#endif

#define AUTO_STRTOL(token,addr_endptr) ( apg_flags & \
(APG_OCT_TOKEN|APG_HEX_TOKEN) ? strtol(token,addr_endptr,0) : \
strtol(token,addr_endptr,10))

#define C_BOUND(x,v,y) ( (x|y) ? ( (x<=v) && (v<=y) ) : (1) )

#define O_PUSH(base,offset,buff,type) ( *(type *)((char *)base+offset+\
sizeof(int))= *(type *)buff )

#define P_PUSH(base,offset,ptr) (*(long *)((char *)base+offset+\
sizeof(int))=(long)ptr)

#define ISODIGIT(x) ( '0' <= x && x <= '3' ? 2 : ( '4' <= x && x <= '7' ? 1 : 0 ) )

/* private functions, mealy tables
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

/* xmalloc, xcalloc, xrealloc */

static void *
xmalloc (size)
     unsigned int size;
{
  void *result =  malloc (size);
  if (result == 0)
    fatalerr ("xmalloc: virtual memory exhausted");
  return result;
}


static void *
xrealloc (ptr, size)
     void *ptr;
     unsigned int size;
{
  void *result;

  /* Some older implementations of realloc() don't conform to ANSI.  */
  result = ptr ? realloc (ptr, size) : malloc (size);
  if (result == 0)
    fatalerr ("xrealloc: virtual memory exhausted");
  return result;
}

static void *
xcalloc (nelem, elsize)
      unsigned int nelem;
      unsigned int elsize;	
{
  void *result = (char *) calloc (nelem,elsize);
  if (result == 0)
    fatalerr ("xcalloc: virtual memory exhausted");
  return result;
}


static char *
ioctl_buffer (char *fn, int flag)
{
  int fd;
  struct stat f_stat;

  switch (flag)
    {
    case APG_OPEN:

      file_name = (char *) xmalloc (strlen (fn) + 1);
      strcpy (file_name, fn);

      if (stat (fn, &f_stat) == -1)
	return (char *) NULL;

      file_image = (char *) xrealloc (file_image, f_stat.st_size + 1);

      if ((fd = open (fn, O_RDONLY)) == -1)
	fatalerr ("err: %s", strerror (errno));
      read (fd, file_image, f_stat.st_size);
      *(file_image + f_stat.st_size) = 0;

      close (fd);

      return file_image;

      break;
    case APG_CLOSE:

      if (file_name)
	free (file_name);
      if (file_image == NULL)
	fatalerr ("err: apg_buffer already closed");

      free (file_image);

      return file_image = (char *) NULL;
      break;
    }

  return (char *) NULL;		/* unreachable */
}


static unsigned char *jmp_token, *jmp_token_0;
static unsigned char *jmp_stream;


static int ac_0 (void)
{
  if (*jmp_stream == '\\') (jmp_stream)++;
  if (*jmp_stream == '\n') apg_buff_line++;

  jmp_stream++;

  return 1;
}

static int ac_1 (void)
{
  *(jmp_token_0++) = *(jmp_stream++);

  return 1;
}

static int ac_2 (void)
{
  if (*jmp_stream == '\n') apg_buff_line++;
  *jmp_token_0++ = '\0';
  jmp_stream++;
  apg_token++;

  return 0;
}

#if defined (__GNUC__) && !defined (__STRICT_ANSI__)
char c_escape[256] =
  {['a'] '\a',['b'] '\b',['t'] '\t',['n'] '\n',['v'] '\v',['f'] '\f',
    ['r'] '\r' };
#else
int c_escape[256];
#endif


static int
ac_3 (void)
{
  register char c;

  jmp_stream++;

  if ((c = c_escape[*jmp_stream]))
    {
      /* escape */
      *jmp_token_0 = c;
    }
  else
    {
      switch (*jmp_stream)
	{
	case '\n':
	  memmove (jmp_stream, jmp_stream + 1, strlen ((const char *)(jmp_stream + 1)));
	  *jmp_token_0 = *jmp_stream;
	  break;
	case '0':
	  if (*(jmp_stream + 1) == 'x' || *(jmp_stream + 1) == 'X')
	    apg_flags |= APG_HEX_TOKEN;
	  else
	    apg_flags |= APG_OCT_TOKEN;

	  *jmp_token_0 = APG_ACK_CHR;
	  jmp_stream--;
	  break;

	default:
	  *jmp_token_0 = *jmp_stream;
	  break;

	}

    }

  jmp_token_0++;
  jmp_stream++;

  return 1;

}

static int
ac_4 (void)
{
  char *eptr;

  jmp_stream--;

  if ((eptr = strchr ((const char *)jmp_stream, '\n')) != NULL) *eptr = '\0';

  while ( *jmp_stream != '\n' && (char *)jmp_stream > (char *)file_image ) jmp_stream--;

  fatalerr ("%s:%d: parse error near -> \"%s\" ", file_name, apg_buff_line+1,
	    ++jmp_stream);

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
  {6, 4, 6, 2, 6, 6, 8, 0}
};

static int (*apg_mealy_action_table[9][8]) (void) =
{
  { ac_1, ac_0, ac_4, ac_4, ac_4, ac_4, ac_0, ac_0} ,
  { ac_1, ac_4, ac_2, ac_4, ac_4, ac_4, ac_0, ac_4} ,
  { ac_1, ac_2, ac_4, ac_2, ac_0, ac_3, ac_0, ac_2} ,
  { ac_1, ac_2, ac_4, ac_2, ac_0, ac_3, ac_0, ac_2} ,
  { ac_0, ac_0, ac_0, ac_0, ac_0, ac_0, ac_0, ac_0} ,
  { ac_1, ac_1, ac_1, ac_1, ac_0, ac_3, ac_1, ac_2} ,
  { ac_4, ac_4, ac_4, ac_4, ac_4, ac_4, ac_0, ac_4} ,
  { ac_4, ac_4, ac_2, ac_4, ac_4, ac_4, ac_0, ac_4} ,
  { ac_4, ac_2, ac_4, ac_2, ac_4, ac_4, ac_0, ac_2} };

static char *
get_token (void)
{
  register int reg_input, reg_state;

  apg_current_line = apg_buff_line;
  jmp_token        = jmp_token_0;

	/*** load registers ***/

  reg_state = apg_mealy_state_table[apg_state_code][apg_input_code];
  reg_input = c_index[*jmp_stream];

  while ((*apg_mealy_action_table[reg_state][reg_input]) ())
    {
      if (!*jmp_stream)
	return (char *) NULL;

      if ((reg_state = apg_mealy_state_table[reg_state][reg_input]) == 1)
	apg_token = 0;

      reg_input = c_index[*jmp_stream];
    }

	/*** save registers ***/

  apg_input_code = reg_input;
  apg_state_code = reg_state;

#ifdef APG_PERF
  global_token++;
#endif

  return (char *)jmp_token;

}

/* arena menagement
*/

static grill_t *
alloc_segment (grill_t * p)
{

  grill_t *q;

  /* in case p doesn't point to the last grill_t, usually it doesn't happen */

  for (q=p; q != NULL; q = q->next) p=q;  
 
  /* we must be sure the segment is empty: calloc() is required. */

  q = (grill_t *) xcalloc (1,sizeof (grill_t));

  if (p == NULL)
    return (apg_arena = apg_stream = q);
  else
    return (apg_stream = p->next = q);

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


  while (p && (p = strchr (p, APG_ACK_CHR)))
    *p = '\\';

  switch (errn0)
    {
    case APG_TYPE_ERR:
     fprintf (stderr,
             "%s:%d: label=%s,token=%d -> {%s} is designed to be a %s type;\n",
	     file_name, apg_current_line + 1, line_list[line_id], token_id,
	     token, types_id[type]);
      break;
    case APG_NULL_ERR:
      fprintf (stderr,
              "%s:%d: label=%s,token=%d -> {%s} isn't an optional argument;\n",
              file_name, apg_current_line , line_list[line_id], token_id,
              token);

      break;
    case APG_OFFSET_ERR:
      fprintf (stderr, 
	       "%s:%d: label=%s,token=%d -> {%s} too many tokens;\n",
	       file_name, apg_current_line + 1, line_list[line_id], token_id,
	       token);

      break;
    case APG_BOUND_ERR:
      fprintf (stderr,
	       "%s:%d: label=%s,token=%d -> {%s} is designed to be a %s[%d,%d]. Out of range;\n",
	       file_name, apg_current_line + 1, line_list[line_id], token_id,
	       token, types_id[type], low, high);

      break;
    case APG_ESC_ERR:
      fprintf (stderr,
	       "%s:%d: label=%s,token=%d -> {%s} bad escape sequence;\n",
	       file_name, apg_current_line + 1, line_list[line_id], token_id,
	       token);
      break;

    case APG_MANY_ERR:
      fprintf (stderr,
	       "%s: label=%s is designed to occur no more than %d time;\n",
               file_name, line_list[line_id], high);
      break;

    case APG_FEW_ERR:
      fprintf (stderr,
               "%s: label=%s is designed to occur at least %d time;\n",
               file_name, line_list[line_id], low);
      break;

    case APG_IPV4_ERR:
      fprintf (stderr,
	  "%s:%d: label=%s,token=%d -> {%s} is designed to be a valid ipv4;\n",
          file_name, apg_current_line + 1, line_list[line_id], token_id,
	  token);

      break;

    case APG_HOST_ERR:
      fprintf (stderr,
	  "%s:%d: label=%s,token=%d -> {%s} is designed to be a valid host;\n",
	  file_name, apg_current_line + 1, line_list[line_id], token_id,
	  token);

      break;

    }
 
   apg_flags |= APG_FATALERR;
}

/* ymalloc & seg_t
*/

static void
alloc_seg_t (char *r)
{
  seg_t *p = tail_seg, *q;

  q       = (seg_t *) xmalloc (sizeof (seg_t));
  q->ptr  = r;
  q->next = NULL;

  if (p)
    p->next  = tail_seg = q;
  else
    { 
     head_seg = q;
     tail_seg = q;
    }
}

static void *
ymalloc (size_t size)
{
  register void *p;

  p = (void *) xmalloc (size);
  alloc_seg_t (p);

  return p;
}
/* strings function
*/

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

  safe_token = ptr = (char *) xrealloc (safe_token, strlen (token) + 1);

  strcpy (safe_token, token);

  while ((ptr = (char *) strchr (ptr, APG_ACK_CHR)))
    {
      register int i = strholen (ptr + 1);
      register int j = strlen (ptr + i);

      strncpy (bufftemp, ptr + 1, i);
      bufftemp[i] = 0;
      *ptr = (char) strtol (bufftemp, NULL, 0);

      if (!*ptr || !i)
	token_fatalerr (line_id, token_id, T_STR, APG_ESC_ERR, 0, 0, token);

      memmove (ptr + 1, ptr + i + 1, j);
      *(ptr + j + 1) = 0;
      ptr++;

    }
  return safe_token;

}

/* apg type checks
*/

#define PROC_OBJECT(p) ( *p == APG_ACK_CHR ? (p+1) : (p)  )

#define L_TYPE(l,t)    apgtb[l][t][0]
#define L_LOW(l,t)     apgtb[l][t][1] 
#define L_HIGH(l,t)    apgtb[l][t][2] 
#define L_OPT(l,t)     apgtb[l][t][3]
#define L_REGEX(l,t)   apgtb[l][t][4] 

static void
token_analysis (char *token, int line_id, int token_id)
{
  char *endptr, *pp=NULL;
  static void *sp;
  int offset = 0;


  /* first step */
  
  if( (offset= apg_offset[line_id][token_id])== -1)
   token_fatalerr (line_id, token_id, 
                   L_TYPE (line_id,token_id), APG_OFFSET_ERR,
                   L_LOW (line_id,token_id), L_HIGH (line_id,token_id), token);

  
  if (token && !*token) return; /* NULL token */

    switch (L_TYPE (line_id,token_id))
      {
      case T_STR:
      case T_HOST:
      case T_IPV4:

	pp = (char *) ymalloc (strlen (token) + 1);
	break;

      default:
	token = PROC_OBJECT (token);
	sp = (void *) xrealloc (sp, sizeof (long));
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
		    O_PUSH (apg_stream, offset, sp, unsigned int);
		    break;
		  case T_SHORT:
		    O_PUSH (apg_stream, offset, sp, short);
		    break;
		  case T_U_16:
		    O_PUSH (apg_stream, offset, sp, unsigned short);
		    break;
		  case T_CHAR:
		    O_PUSH (apg_stream, offset, sp, char);
		    break;
		  case T_U_8:
		    O_PUSH (apg_stream, offset, sp, unsigned char);
		    break;
		  }
		return;
	      }
	    else
	      token_fatalerr (line_id, token_id, 
                              L_TYPE (line_id,token_id), APG_BOUND_ERR,
			      L_LOW (line_id,token_id), 
                              L_HIGH (line_id,token_id), token);
	  }
	else
	  token_fatalerr (line_id, token_id, 
                          L_TYPE (line_id,token_id), APG_TYPE_ERR,
			  L_LOW (line_id,token_id), 
                          L_HIGH (line_id,token_id), token);
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
	    token_fatalerr (line_id, token_id, 
                            L_TYPE (line_id,token_id), APG_BOUND_ERR,
			    L_LOW (line_id,token_id), 
                            L_HIGH (line_id,token_id), token);

	  return;
	}
	break;

/* host/ipv4 regmacro
*/

#define R_0_9(x)      ( '0' <= x && x <= '9' )
#define R_A_Z(x)      ( 'A' <= x && x <= 'Z' )
#define R_a_z(x)      ( 'a' <= x && x <= 'z' )

#define R_ALPHANUM(x) ( R_0_9(x) || R_A_Z(x) || R_a_z(x) )
#define R_ALPHA(x)    ( R_A_Z(x) || R_a_z(x) )

#define H_ALPHA    0x01
#define H_DOT      0x02
#define H_MIN      0x04
#define H_ERR      0x08
#define H_2LD      0x10
#define H_L_ALPHA  0x20
    case T_HOST:
      {
        register int state = H_DOT;
        register int t_counter = 1;
        char *P = (char *) token;

        while (*P)
          {
            switch (*P)
              {
              case '.':
                if (state & (H_DOT | H_MIN))
                  state |= H_ERR;
                state |= (H_DOT | H_2LD);
                state &= ~(H_L_ALPHA | H_MIN);
                t_counter++;
                break;
              case '-':
                if (state & (H_DOT | H_MIN))
                  state |= H_ERR;
                state |= H_MIN;
                break;
              default:
                state &= ~(H_DOT | H_MIN);
                if (!R_ALPHANUM (*P))
                  state |= H_ERR;
                if (R_ALPHA (*P))
                  state |= (H_ALPHA | H_L_ALPHA);
                break;
              }
            P++;

          }

        if (*P ||
            state & (H_ERR|H_DOT|H_MIN) ||
            ~state & H_2LD || (state & H_ALPHA && ~state & H_L_ALPHA ) )
          {
            token_fatalerr (line_id, token_id, T_HOST,
                            APG_HOST_ERR, L_LOW (line_id, token_id),
                            L_HIGH (line_id, token_id), token);
            return;
          }

        else if (state & H_ALPHA)
          {
            strcpy (pp, token);
            P_PUSH (apg_stream, offset, pp);
            return;
          }

      }
    case T_IPV4:
      {
        register int state = H_DOT;
        register int t_counter = 1;
        register int t_byte = 0;
        char *P = (char *) token;

        while (*P)
          {
            switch (*P)
              {
              case '.':
                if (state & H_DOT)
                  state |= H_ERR;
                state |= H_DOT;
                t_counter++;
                t_byte = 0;
                break;
              default:
                state &= ~H_DOT;
                if (!R_0_9 (*P))
                  state |= H_ERR;

		/* t_byte = t_byte * 10 + val(*P) */

                t_byte = (t_byte<<3) + (t_byte<<1) + (*P - '0');

                if (t_byte & 0xffffff00)
                  state |= H_ERR;
                break;
              }
            P++;

          }

        if (*P || state & (H_ERR|H_DOT) || t_counter != 4)
          {
            token_fatalerr (line_id, token_id, T_IPV4,
                            APG_IPV4_ERR, L_LOW (line_id, token_id),
                            L_HIGH (line_id, token_id), token);
            return;
          }

        else
          {
            strcpy (pp, token);
            P_PUSH (apg_stream, offset, pp);
            return;
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
  register int i = errnum;

  if (errnum < 1 || errnum > APG_MAXERR)
    i = 0;
  sb = (char *) xrealloc (sb, strlen (err_table[i]) + 1);
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
apg_free_pragma (void)
{
  register seg_t *p = head_seg, *q = p;

  while ((p = q))
    {
      if (p->ptr)
	free (p->ptr);
      q = p->next;
      free (p);
    }
  head_seg = NULL;
}

int
apg_get_line (grill_t ** apg_user_ptr)
{
  apg_errno = APG_EOK;

  if (!(apg_flags & APG_PARSED_GRILL))
    {
      apg_errno = APG_EEMPTY;
      return 0;
    }

  if (!*apg_user_ptr)
    {
      apg_errno = APG_ENULL;
      return 0;
    }

  if (!(apg_flags & APG_RESET_STREAM))
    {
      apg_flags |= APG_RESET_STREAM;
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

#ifdef APG_PERF
#include <sys/time.h>
#include <unistd.h>

typedef struct
{
  int hour;
  int min;
  int sec;
  int dec;
}
elapsed_val;

int
difftimeofday (now, old, diff)
     struct timeval now;
     struct timeval old;
     elapsed_val *diff;

{
  register int r1, r2, r3, r4;

  r1 = (now.tv_usec - old.tv_usec) / 10000;
  r2 = (now.tv_sec - old.tv_sec) % 60;
  r3 = ((now.tv_sec - old.tv_sec) / 60) % 60;
  r4 = ((now.tv_sec - old.tv_sec) / 3600);

  if (r1 < 0)
    {
      r1 += 100;
      r2--;
      if (r2 < 0)
        {
          r2 += 60;
          r3--;
          if (r3 < 0)
            {
              r3 += 60;
              r4--;

              if (r4 < 0)
                return -1;
            }
        }
    }
  diff->dec = r1;
  diff->sec = r2;
  diff->min = r3;
  diff->hour = r4;

  return 0;
}

#endif

grill_t *
apg_parser (int q, ...)
{
  register long shift_reg=0;
  register int i = q, j=0;

  va_list ap;
  char *b_stream = NULL, *tk = NULL;
  char *file;
#ifdef APG_PERF
  struct timeval start_time, end_time;

  gettimeofday (&start_time, NULL); 
#endif

  va_start (ap, q);

  /* cleaning apg_flags */

  apg_flags = 0;
  apg_errno = APG_EOK;


  file = va_arg (ap, char *);

  while (i-- && !(b_stream = ioctl_buffer (file, APG_OPEN)))
    file = va_arg (ap, char *);

  if (!b_stream)
    {
      i = q;
      va_start (ap, q);

      fprintf (stderr, "err: ");
      while (i--)
	fprintf (stderr, "%s ", va_arg (ap, char *));

      fprintf (stderr, "; no such file[s] to parse; exit forced.\n");

      exit (1);
    }

  va_end (ap);

/* ansicare vector setup */

#if !defined (__GNUC__) || defined (__STRICT_ANSI__)

c_index['#' ]=1;
c_index[APG_SEPLINE]=2;
c_index[APG_SEPTOKEN]=3;
c_index['\'']=4;
c_index['\\']=5;
c_index[' ' ]=6;
c_index['\t']=6;
c_index['\n']=7;

c_escape['a']='\a';
c_escape['b']='\b';
c_escape['t']='\t';
c_escape['n']='\n';
c_escape['v']='\v';
c_escape['f']='\f';
c_escape['r']='\r';

#endif

  /* apg get token setup */

  if (jmp_token == NULL)
    {
      token_basebuffer = jmp_token = jmp_token_0 = 
	(unsigned char *) xcalloc (strlen (b_stream), sizeof (char));

      jmp_stream     = (unsigned char *)b_stream;
      apg_input_code = c_index[*jmp_stream];
    }

  while ((tk = get_token ()))
    {

	switch(apg_token)
	{

	case 0: /* null */
	break;
	
	case 1: /* label line */

	  /* before parsing the new line, we check the shift register in
             order to detect non-optional NULL token */

          if (apg_stream)
          for (j=1; j< APG_MAXARG ; j++)
                  if ( L_OPT(apg_stream->type_line,j) && ~shift_reg & (1<<j) )
                  token_fatalerr (apg_stream->type_line,j, 
                                  L_TYPE (apg_stream->type_line,j), 
                                  APG_NULL_ERR, 
                                  L_LOW (apg_stream->type_line,j), 
                                  L_HIGH (apg_stream->type_line,j), NULL);


          shift_reg = 0; /* clear */

	  apg_stream = alloc_segment (apg_stream);
	  apg_stream->type_line = get_lineid (tk);

/* man rep
*/

	  rep_counter[apg_stream->type_line]++;

	  if (rep_limits[apg_stream->type_line][1] != -1 &&
	      rep_counter[apg_stream->type_line] >
	      rep_limits[apg_stream->type_line][1])
       		token_fatalerr ( apg_stream->type_line,  
                	        0, 
                        	0,
                        	APG_MANY_ERR,
                        	0, 
                        	rep_limits[apg_stream->type_line][1],
                        	NULL);

	break;
	default: /* token */

        /* shift_reg setup: (apg_token == 1 ? line_id : token_id) */

        if (tk && *tk)
        shift_reg |= 1 << (apg_token-1);

	token_analysis (tk, apg_stream->type_line, apg_token - 1);

	break;
	}
    }

    /* now we check the last line */

          if (apg_stream)
          for (j=1; j< APG_MAXARG ; j++)
                  if ( L_OPT(apg_stream->type_line,j) && ~shift_reg & (1<<j) )
                  token_fatalerr (apg_stream->type_line,j, 
                                  L_TYPE (apg_stream->type_line,j), 
                                  APG_NULL_ERR,
                                  L_LOW (apg_stream->type_line,j), 
                                  L_HIGH (apg_stream->type_line,j), NULL);


/* min rep
*/

  {
    register int i = 1;
    for (; line_list[i]; i++)
      if (rep_limits[i][0] != -1 && rep_counter[i] < rep_limits[i][0])
       token_fatalerr (	i, 
			0, 
			0, 
			APG_FEW_ERR, 
			rep_limits[i][0], 
			0, 
			NULL);
  }

  free (token_basebuffer);
  ioctl_buffer (NULL, APG_CLOSE);

  apg_flags |= APG_PARSED_GRILL;

  if ( apg_flags & APG_FATALERR )
	fatalerr("apg_parser(): encontered some errors"); 

#ifdef APG_PERF
  {
  elapsed_val diff;
  double d=0, r=0;

  gettimeofday (&end_time, NULL);
  difftimeofday (end_time, start_time, &diff);

  d = (double)(end_time.tv_sec - start_time.tv_sec) +
      (double)(end_time.tv_usec - start_time.tv_usec)/1000000;

  r = (d ? (double)global_token/d : 999999999 );

  fprintf(stderr,"apg_parser(): token=%d overal_time=%02d:%02d,%02d tps=%d\n",
                global_token,diff.min, diff.sec, diff.dec, (int)r);

  }
#endif

  return apg_arena;

}


