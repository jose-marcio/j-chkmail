/*
 *
 * j-chkmail - Mail Server Filter for sendmail
 *
 * Copyright (c) 2001, 2002, 2003, 2004 - Ecole des Mines de Paris
 *
 *  Auteur     : Jose Marcio Martins da Cruz
 *               Jose-Marcio.Martins@ensmp.fr
 *
 *  Historique :
 *  Creation     : janvier 2002
 *
 * This program is free software, but with restricted license :
 *
 * - j-chkmail is distributed only to registered users
 * - j-chkmail license is available only non-commercial applications,
 *   this means, you can use j-chkmail if you make no profit with it.
 * - redistribution of j-chkmail in any way : binary, source in any
 *   media, is forbidden
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * More details about j-chkmail license can be found at j-chkmail
 * web site : http://j-chkmail.ensmp.fr
 */


#ifndef __JBUFFER_H__

long                bufspn (char *, long, char *);
long                bufcspn (char *, long, char *);

size_t              buf_clean_rc (char *, size_t );

int                 buf_get_line (char *, long, char *, long);

char               *read_text_file (char *, size_t *);

char               *buf_get_next_line (char *, char *, size_t);

bool                text_word_length(char *, kstats_T *, size_t);

long                text_buf_histogram(char *, size_t, long *);


#define __JBUFFER_H__
#endif
