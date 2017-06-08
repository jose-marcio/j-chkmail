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
#define   MAX_UNKNOWN_CMD   2

static              sfsistat
mlfi_unknown(ctx, cmd)
     SMFICTX            *ctx;
     const char         *cmd;

{
  CTXPRIV_T          *priv = MLFIPRIV(ctx);
  int                 result = SMFIS_CONTINUE;
  int                 ip_class;

  INIT_CALLBACK_DELAY();
  if (priv == NULL)
  {
    result = SMFIS_TEMPFAIL;
    return result;
  }

  INIT_CALLBACK(priv, CALLBACK_UNKNOWN);

  ip_class = priv->netclass.class;
  sm_macro_update(ctx, priv->sm);
  MESSAGE_INFO(9, "%s : %s Unknown command : %s", CONNID_STR(priv->id),
               priv->peer_addr, STRNULL(cmd, "NULL"));

  priv->nb_unknown_cmd++;
  if (IS_UNKNOWN(ip_class) && priv->nb_unknown_cmd > MAX_UNKNOWN_CMD)
  {
    char                buf[256];

    (void) jsmfi_setreply(ctx, "421", "4.5.1", "Too many errors");
    snprintf(buf, sizeof (buf), "Too many unknown SMTP commands : %d (%s)",
             priv->nb_unknown_cmd, CTX_NETCLASS_LABEL(priv));
    log_msg_context(ctx, buf);

    result = SMFIS_TEMPFAIL;
    priv->result = result;
  }
  if (result != SMFIS_CONTINUE)
    goto fin;

  /*
   **
   **
   **
   */
#if _FFR_MODULES
  /* 
   ** j-chkmail modules
   **
   */
  if (do_module_callback(ctx, 0, &result))
    goto fin;
  if (result != SMFIS_CONTINUE)
    goto fin;
#endif             /* _FFR_MODULES */
fin:
  CHECK_CALLBACK_DELAY();
  return result;
}
