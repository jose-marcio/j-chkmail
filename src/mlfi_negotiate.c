
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


/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
#if HAVE_XXFI_NEGOTIATE
static              sfsistat
mlfi_negotiate(SMFICTX * ctx,
               unsigned long f0, unsigned long f1,
               unsigned long f2, unsigned long f3,
               unsigned long *pf0, unsigned long *pf1,
               unsigned long *pf2, unsigned long *pf3)
{
  CTXPRIV_T          *priv = NULL;
  sfsistat            result = SMFIS_ALL_OPTS;

  *pf0 = f0;
  *pf1 = f1;
  *pf1 = 0;

#if 0
  priv = malloc(sizeof (CTXPRIV_T));
  if (priv != NULL) {
    if (smfi_setpriv(ctx, priv) != MI_SUCCESS) {
      FREE(priv);
      LOG_MSG_ERROR("smfi_setpriv(priv) error");
      (void) jsmfi_setreply(ctx, "421", "4.5.1",
                            "I'm too busy now. Try again later !");
      result = SMFIS_TEMPFAIL;
      goto fin;
    }
  }
#endif

  /*
   ** Ask the MTA to send even rejected recipients 
   */
  if ((f1 & SMFIP_RCPT_REJ) == 0) {
    static int          n = 0;

    if (n++ < 5)
      MESSAGE_INFO(10, "MTA unable to send rejected recipients information");
  } else
    *pf1 |= SMFIP_RCPT_REJ;
#if 0
  *pf1 |= SMFIP_NR_HDR;
#endif
  *pf2 = 0;
  *pf3 = 0;
  if (priv != NULL) {
    priv->mta_caps.f0 = f0;
    priv->mta_caps.f1 = f1;
    priv->mta_caps.f2 = f2;
    priv->mta_caps.f3 = f3;
  }

fin:
#if 1
  return SMFIS_CONTINUE;
#else
  return SMFIS_ALL_OPTS;
#endif
}
#endif
