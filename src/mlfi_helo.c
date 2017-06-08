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
sfsistat
mlfi_helo(ctx, helohost)
     SMFICTX            *ctx;
     char               *helohost;
{
  CTXPRIV_T          *priv = MLFIPRIV(ctx);
  int                 result = SMFIS_CONTINUE;

  INIT_CALLBACK_DELAY();

  if (priv == NULL)
  {
    result = SMFIS_TEMPFAIL;
    goto fin;
  }

  INIT_CALLBACK(priv, CALLBACK_EHLO);

  sm_macro_update(ctx, priv->sm);

  if (helohost == NULL || strlen(helohost) == 0) {
    helohost = "empty.empty";
    log_msg_context(ctx, "Empty helo parameter");
  }

  FREE(priv->helohost);
  priv->helohost = NULL;

  if ((priv->helohost = strdup(helohost)) == NULL)
  {
    LOG_SYS_ERROR("strdup(helohost) error");
    result = SMFIS_TEMPFAIL;

    goto fin;
  }

#if _FFR_DELAYED_REJECT
  if (priv->delayed_result.result != SMFIS_CONTINUE)
  {
    if (priv->callback >= priv->delayed_result.callback)
    {
      (void) jsmfi_setreply(ctx, "550", "5.7.1", priv->delayed_result.reply);
      log_msg_context(ctx, priv->delayed_result.why);
      goto fin;
    }
  }
#endif

  result = check_cpu_load(ctx, CONNID_STR(priv->id), priv->peer_addr,
                          priv->netclass.class);
  if (result != SMFIS_CONTINUE)
  {
    log_msg_context(ctx, "Global load control reject");

    goto fin;
  }

  result = check_filter_open_connections(ctx, CONNID_STR(priv->id),
                                         priv->peer_addr, priv->netclass.class);
  if (result != SMFIS_CONTINUE)
  {
    log_msg_context(ctx, "Global open connections limit");

    goto fin;
  }

  priv->ehlo_flags = check_ehlo_value(ctx);

  result = validate_connection(ctx);
  if (result != SMFIS_CONTINUE)
    goto fin;

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

  /* continue processing */
  return result;
}
