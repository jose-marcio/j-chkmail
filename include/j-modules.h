/*
 *
 * j-chkmail - Mail Server Filter for sendmail
 *
 * Copyright (c) 2001-2004 - Ecole des Mines de Paris
 *
 *  Auteur       : Jose Marcio Martins da Cruz
 *                 Jose-Marcio.Martins@ensmp.fr
 *
 *  Historique   :
 *  Creation     : Sat Jun  2 22:18:08 CEST 2007
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


#ifndef J_MODULES_H

#define MOD_API_MAJOR           0x00
#define MOD_API_MINOR           0x02
#define MOD_API_PATCH           0x00

#define MOD_VERSION ((MOD_API_MAJOR << 16) | (MOD_API_MINOR << 8) | MOD_API_PATCH)

#define MOD_API_VERSION      "0.01.00"

#define MODR_CONTINUE       0x0000
#define MODR_REJECT         0x0001
#define MODR_TEMPFAIL       0x0002

#define MODF_QUARANTINE     0x0100
#define MODF_STOP_CHECKS    0x0200

#define MOD_QUARANTINE(flag)  (((flag) & MODF_QUARANTINE) != 0)
#define MOD_STOP_CHECKS(flag) (((flag) & MODF_STOP_CHECKS) != 0)


typedef struct
{
  char               *name;
  char               *author;
  char               *version;
  uint32_t            calloffer;
  uint32_t            callrequest;
} mod_info_T;


typedef struct
{
  int                 callback;
  char               *id;
  char               *claddr;
  char               *clname;
  char               *helo;
  char               *from;
  char               *rcpt;
  char               *sfile;
  bool                xfiles;
  msg_scores_T       *raw_scores;
  msg_scores_T       *net_scores;
  int                 result;
  int                 flags;
  char                code[8];
  char                xcode[8];
  char                reply[64];
  char                modname[32];
} mod_ctx_T;

typedef struct
{
  char               *moddir;
  bool                enable;
  char               *args;
  uint32_t            callrequest;
  uint32_t            calloffer;
} mod_open_T;


bool                load_all_modules(char *cfdir, char *modcf, char *moddir);

bool                module_info();

bool                module_call(int callback, int step, mod_ctx_T * arg);

bool                module_service(int why);

# define J_MODULES_H    1
#endif             /* J_MODULES_H */
