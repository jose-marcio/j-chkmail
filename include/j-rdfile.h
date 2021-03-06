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

#ifndef __JRDFILE_H_

#define            RD_ONE_COLUMN   1
#define            RD_TWO_COLUMN   2

#define            RD_NORMAL       0
#define            RD_REVERSE      1

int                 j_rd_text_file (char *, int, int, char *,
                                    int (*)(void *, void *));

typedef int (*RDFILE_F)(void *, void *);

int                 j_rd_file (char *fname, char *tag, RDFILE_F f, void *arg);

#define __JRDFILE_H_
#endif
