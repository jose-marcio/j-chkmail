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
 *  Creation     : Tue Jan 17 14:47:33 CET 2006
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
#include <j-filter.h>
#include <j-log-virus.h>


/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static LOG_T        logt = LOG_INITIALIZER;

/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
bool
log_virus_reopen()
{
  char               *logname = cf_get_str(CF_VIRUS_LOG_FILE);
  char                path[1024];
  char               *wkdir = cf_get_str(CF_WORKDIR);
  bool                res = TRUE;

  ADJUST_LOG_NAME(path, logname, wkdir, "none:");

  MUTEX_LOCK(&mutex);

  if (!log_ready(&logt))
    res = log_open(&logt, path);
  else
    res = log_reopen(&logt);

  MUTEX_UNLOCK(&mutex);

  return res;
}

/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
void
log_virus(id, ip, virus)
     char               *id;
     char               *ip;
     char               *virus;
{
  time_t              t = time(NULL);
  int                 r;

  char               *logname = cf_get_str(CF_VIRUS_LOG_FILE);

  id = STRNULL(id, "00000000.000");
  ip = STRNULL(ip, "0.0.0.0");

  if ((logname == NULL) || (strlen(logname) == 0))
    return;

  if (strcasecmp(logname, "NONE") == 0)
    return;

  if (!log_ready(&logt) && !log_virus_reopen())
    return;

  MUTEX_LOCK(&mutex);

#if 0
  if (!log_ready(&logt))
  {
    if (!log_open(&logt, logname))
      goto fin;
  }
#endif

  log_printf(&logt, "%10ld %-12s %s %s\n", (long) t, id, ip, virus);

fin:
  MUTEX_UNLOCK(&mutex);
}
