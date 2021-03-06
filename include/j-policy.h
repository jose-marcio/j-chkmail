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

#ifndef __JPOLICY_H_

bool                lookup_policy(char *, char *, char *, size_t, bool);

bool                check_policy(char *prefix, char *key, char *buf,
                                 size_t size, bool cdef);

bool                check_host_policy(char *prefix, char *addr, char *name,
                                      char *class, char *buf, size_t size,
                                      bool cdef);

bool                check_email_policy(char *prefix, char *email,
                                       char *buf, size_t size, bool cdef);

bool                check_generic_policy(char *prefix, char *key,
                                         char *buf, size_t size, bool cdef);

bool                check_policy_tuple(char *prefix, char *ip, char *name,
                                       char *netclass,
                                       char *from, char *to, bool result);

bool                check_policy_all_rcpts(char *prefix, char *ip,
                                           char *name, char *netclass,
                                           char *from, rcpt_addr_T * rcpt,
                                           bool result, int conflict);

long                check_limit_tuple(char *prefix, char *ip, char *name,
                                      char *netclass,
                                      char *from, char *to, long result);

long                check_limit_all_rcpts(char *prefix, char *ip,
                                          char *name, char *netclass,
                                          char *from, rcpt_addr_T * rcpt,
                                          long defval);

bool                policy_init();
bool                policy_close();
bool                policy_reopen();

#define JC_DEFAULT              0
#define JC_OK                   1
#define JC_REJECT               2

int                 policy_decode(char *);


#define __JPOLICY_H_
#endif
