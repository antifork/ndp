/*
 *  $Id$
 *  %ndp: md5 utils
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

#include <defs.h>
#include <md5.h>

/*

 * - MINPWDLEN   Minimum password lenght
 * - MAXPWDLEN   Maximum password lenght
 * - MAXTRYTIME  Max num. of try to change the pass.
 *
 */

#define MINPASSWDLEN 3
#define MAXPASSWDLEN 99
#define MAXTRYTIME 5

#include <stdio.h>
#include <stdlib.h>

#if STDC_HEADERS
#include <string.h>
#else
#ifndef HAVE_STRCHR
#define strchr index
#define strrchr rindex
#endif
char *strchr(), *strrchr();
#ifndef HAVE_MEMCPY
#define memcpy(d, s, n) bcopy ((s), (d), (n))
#define memmove(d, s, n) bcopy ((s), (d), (n))
#endif
#endif

#if HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif

unsigned char pass[MAXPASSWDLEN + 1], r_pass[MAXPASSWDLEN + 1], md5[17], buff[33];

void
md5_printable(key, p)
	unsigned char *key;
	unsigned char *p;
{
	unsigned char st[3];
	register int i, j;

	*p = '\0';

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			sprintf(st, "%02X", *(key + (i * 4) + j));
			strcat(p, st);
		}
	}
	return;
}

int
main(int argc, char **argv)
{
	int ct = 0;
	struct md5_ctx hash;

	printf("NOTE: password must be long at least %i chars.\n\n", MINPASSWDLEN);
	do {
		if ((ct++) == MAXTRYTIME) {
			printf("Nothing was changed.\n");
			exit(0);
		}
		strncpy(pass, getpass("Password: "), MAXPASSWDLEN);
		strncpy(r_pass, getpass("Re-enter password: "), MAXPASSWDLEN);
	}
	while (strncmp(r_pass, pass, MAXPASSWDLEN)
	       || (strlen(pass) < MINPASSWDLEN));

	md5_init_ctx(&hash);
	md5_process_bytes(pass, strlen(pass), &hash);
	md5_finish_ctx(&hash, md5);
	md5_printable(md5, buff);

	printf("md5: %s\n", buff);
	exit(0);
}
