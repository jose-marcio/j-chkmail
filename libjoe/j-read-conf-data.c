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
 *  Creation     : Tue May 23 13:43:08 CEST 2006
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


#include <j-sys.h>
#include <j-chkmail.h>
#include <j-read-conf-data.h>


bool
read_conf_data_file(cfdir, fname, dfile, func)
     char          *cfdir;
     char          *fname;
     char          *dfile;
     read_conf_data_file_F func;
{
  char           path[1024];

  int            i;
  int            argc;
  char          *argv[32];
  char           fbuf[1024];

  bool           result = TRUE;

  ASSERT(func != NULL);

  if (cfdir == NULL || strlen(cfdir) == 0)
    cfdir = J_CONFDIR;
  memset(fbuf, 0, sizeof (fbuf));
  if (fname != NULL)
    strlcpy(fbuf, fname, sizeof (fbuf));
  argc = str2tokens(fbuf, 32, argv, " ,");

  result = TRUE;
  for (i = 0; i < argc && result; i++)
  {
    char          *tag;

    tag = strchr(argv[i], ':');
    if (tag != NULL)
      *tag++ = '\0';
    ADJUST_FILENAME(path, argv[i], cfdir, dfile);
    MESSAGE_INFO(11, "  * Loading file path=(%s) tag=(%s)",
                 path, STRNULL(tag, "(null)"));

    result = func(path, tag);
    if (!result)
      break;
  }

  return result && i > 0;
}
