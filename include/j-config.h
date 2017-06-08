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


#ifndef __JCONFIG_H__

/*
 *
 */

void            dump_j_conf (int );

/*
 *
 */

typedef struct {
  bool            arg_h;        /* help */
  int             arg_v;        /* version */

  bool            arg_m;
  bool            arg_n;

  char           *arg_p;        /* */
  char           *arg_i;        /* */
  char           *arg_u;        /* */
  char           *arg_c;        /* */
  char           *arg_l;        /* */
  char            arg_q;        /* designated quarantine */
  bool            arg_t;        /* dump tables */
  bool            arg_z;        /* enable core dump */
} OPT_REC_T;


extern OPT_REC_T  cf_opt;



#define __JCONFIG_H__
#endif
