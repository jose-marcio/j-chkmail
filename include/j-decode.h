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


#ifndef __JDECODE_H__

bool                is_rfc1521_encoded (char *);
int                 decode_rfc1521 (char *, char *, size_t);

bool                is_rfc2047_encoded (char *);
int                 decode_rfc2047 (char *, char *, size_t);

bool                is_rfc2231_encoded (char *);
int                 decode_rfc2231 (char *, char *, size_t);

#if 1
#define TSPECIALS "()<>@,;:\\/[]?=\""
#else
#define TSPECIALS "<>@,;:\\/[]?=\""
#endif

int                 strascii (char *, char *, char *);

#define __JDECODE_H__
#endif
