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

#include <j-sys.h>

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */

#if USE_PCRE
#define J_PCRE_FLAGS           (PCRE_CASELESS | PCRE_DOTALL)
#else
#define J_PCRE_FLAGS            0
#endif

#define JREGCOMP_FLAGS         (REG_ICASE | REG_EXTENDED)
#define JREGEXEC_FLAGS          0

#include "j-libjc.h"

#include <j-regex.h>

#if USE_PCRE
static bool         use_pcre = TRUE;
#else
static bool         use_pcre = FALSE;
#endif             /* USE_PCRE */


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
jreg_comp(re, expr, flags)
     jregex_t           *re;
     char               *expr;
     int                 flags;
{
  if ((re == NULL) || (expr == NULL))
    return FALSE;

#if USE_PCRE
  use_pcre = TRUE;
#else
  use_pcre = FALSE;
#endif             /* USE_PCRE */

  MESSAGE_INFO(20, "Using PCRE : %s", use_pcre ? "YES" : "NO"); 

  if (re->signature == SIGNATURE)
    jreg_free(re);

#if USE_PCRE
  if (use_pcre)
  {

    if (re->signature != SIGNATURE)
    {
      const char         *errptr = NULL;
      int                 erroffset = 0;

      re->pcre_rebase = pcre_compile(expr, J_PCRE_FLAGS, &errptr, &erroffset, NULL);
      if (re->pcre_rebase == NULL)
        LOG_MSG_ERROR("pcre_compile error : %s", errptr);
      if (re->pcre_rebase != NULL)
      {
        re->pcre_rextra = pcre_study(re->pcre_rebase, 0, &errptr);
        if (re->pcre_rextra == NULL)
          LOG_MSG_INFO(12, "pcre_study error : %s", errptr);
      }
      re->re_ok = (re->pcre_rebase != NULL);
    }
  } else
#endif
  {
    re->result = regcomp(&re->re, expr, flags);
  }

  if (re->result == 0)
  {
    /*
    ** See this latter if shall save expression
    */
#if 1
    re->expr = strdup(expr);
#endif
    re->signature = SIGNATURE;
  }
  return (re->result == 0);
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
#define    DIM_VECTOR     (3 * 32)

bool
jreg_exec(re, s, pi, pf, flags)
     jregex_t           *re;
     char               *s;
     long               *pi;
     long               *pf;
     int                 flags;
{
  regmatch_t          pm;

  if ((re == NULL) || (re->signature != SIGNATURE) || (s == NULL))
    return FALSE;

  re->result = -1;
#if USE_PCRE
  if (use_pcre)
  {
    int                 ovector[DIM_VECTOR];
    int                 rc;
    
    rc = pcre_exec(re->pcre_rebase, re->pcre_rextra, s, strlen(s),
                   0, 0, ovector, DIM_VECTOR);
    if (rc >= 0)
    {
      re->result = 0;
      if (pi != NULL)
        *pi = ovector[0];
      if (pf != NULL)
        *pf = ovector[1];
    }
  } else
#endif
  {
    re->result = regexec(&re->re, s, 1, &pm, flags);
    if (re->result == 0)
    {
      if (pi != NULL)
        *pi = pm.rm_so;
      if (pf != NULL)
        *pf = pm.rm_eo;
    }
  }

  return (re->result == 0);
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
void
jreg_free(re)
     jregex_t           *re;
{
  if ((re == NULL) || (re->signature != SIGNATURE))
    return;

#if USE_PCRE
  if (use_pcre)
  {
    if (re->pcre_rebase != NULL)
      pcre_free(re->pcre_rebase);
    if (re->pcre_rextra != NULL)
      pcre_free(re->pcre_rextra);
    re->pcre_rebase = NULL;
    re->pcre_rextra = NULL;
  } else
#endif
  {
    regfree(&re->re);
  }

  FREE(re->expr);
  re->signature = 0;
  memset(re, 0, sizeof (jregex_t));
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
int
jreg_count(s, expr)
     char               *s;
     char               *expr;
{
  int                 nb = 0;
  long                pf = 0;
  char               *p;
  jregex_t            re;

  if ((s == NULL) || (expr == NULL))
    return FALSE;

  memset(&re, 0, sizeof (re));

  if (jreg_comp(&re, expr, JREGCOMP_FLAGS))
  {
    for (p = s; jreg_exec(&re, p, NULL, &pf, 0); p += pf)
      nb++;
    jreg_free(&re);
  }

  return nb;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
jreg_lookup(s, expr, pi, pf)
     char               *s;
     char               *expr;
     long               *pi;
     long               *pf;
{
  jregex_t            re;
  bool                found = FALSE;
  long                xi, xf;

  if ((s == NULL) || (expr == NULL))
    return FALSE;

  xi = xf = 0;
  memset(&re, 0, sizeof (re));

  if (jreg_comp(&re, expr, JREGCOMP_FLAGS))
  {
    found = jreg_exec(&re, s, &xi, &xf, JREGEXEC_FLAGS);
    jreg_free(&re);
    if (found)
    {
      if (pi != NULL)
        *pi = xi;
      if (pf != NULL)
        *pf = xf;
    }
  }
  return found;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
char               *
jreg_error(re)
     jregex_t           *re;
{

  return NULL;
}
