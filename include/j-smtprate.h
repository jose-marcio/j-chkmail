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

#ifndef __JSMTPRATE_H__

#if 1
#define USE_SMTPRATE  1
#endif

bool                smtprate_init (size_t, size_t);
void                smtprate_free();
bool                smtprate_resize(size_t , size_t);

bool                smtprate_cleanup_table(time_t now, time_t win);
int                 smtprate_update_table (time_t);

void                smtprate_log_table ();
void                smtprate_print_table (int, int, int, int, time_t, uint32_t, int);
void                smtprate_save_table (char *);
int                 smtprate_read_table (char *);

uint32_t            smtprate_str2flags(char *);


#define    RATE_CONN             0
#define    RATE_RCPT             1
#define    RATE_BOUNCE           2
#define    RATE_MSGS             3
#define    RATE_HAM              4
#define    RATE_SPAM             5
#define    RATE_SCORE            6
#define    RATE_XFILES           7
#define    RATE_VOLUME           8
#define    RATE_SVCTIME          9

#define    RATE_FROM_CONN       10
#define    RATE_FROM_MSGS       11
#define    RATE_FROM_RCPT       12

#define    RATE_AUTH_CONN       13
#define    RATE_AUTH_MSGS       14
#define    RATE_AUTH_RCPT       15

#define    RATE_DIM             20

#define    DEFAULT_WINDOW       -1

int                 smtprate_add_entry (int, char *, char *, int, time_t);
int                 smtprate_check (int, char *, time_t);


extern unsigned int smtprate_interval;  /* 1 minute */
extern unsigned int smtprate_window;  /* 10 minutes */



#define __JSMTPRATE_H__
#endif


