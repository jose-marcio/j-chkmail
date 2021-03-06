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
 *  Creation     : Tue Feb 21 22:09:21 CET 2006
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
#include <j-rcpt-list.h>


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
rcpt_addr_T   *
rcpt_list_free(head)
     rcpt_addr_T   *head;
{
  rcpt_addr_T   *p;

  if (head == NULL)
    return NULL;

  while (head != NULL)
  {
    p = head->next;
    FREE(head->rcpt);
    FREE(head->email);
    FREE(head->host);
    FREE(head->user);

    FREE(head);
    head = p;
  }
  return head;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
rcpt_addr_T   *
rcpt_list_add(head, rcpt, access)
     rcpt_addr_T  **head;
     char          *rcpt;
     int            access;
{
  rcpt_addr_T   *p, *q;
  bool           res = TRUE;
  rcpt_addr_T   *new = NULL;

  char          *s;

  p = q = new = NULL;

  if (rcpt == NULL || strlen(rcpt) == 0)
    return FALSE;

  if ((p = malloc(sizeof (rcpt_addr_T))) == NULL)
  {
    LOG_SYS_ERROR("malloc(rcpt_addr_T)");
    res = FALSE;
    goto fin;
  }

  memset(p, 0, sizeof (*p));

  if ((p->rcpt = strdup(rcpt)) == NULL)
  {
    LOG_SYS_ERROR("strdup(rcpt)");
    res = FALSE;
    goto fin;
  }

  if ((p->email = strdup(rcpt)) == NULL)
  {
    LOG_SYS_ERROR("strdup(rcpt)");
    res = FALSE;
    goto fin;
  }
  extract_email_address(p->email, rcpt, strlen(p->email) + 1);

  if ((p->user = strdup(p->email)) == NULL)
  {
    LOG_SYS_ERROR("strdup(p->email)");
    res = FALSE;
    goto fin;
  }
  if ((s = strrchr(p->user, '@')) != NULL)
    *s = '\0';

  if ((p->host = strdup(p->email)) == NULL)
  {
    LOG_SYS_ERROR("strdup(p->email)");
    res = FALSE;
    goto fin;
  }
  if ((s = strrchr(p->email, '@')) != NULL)
    strlcpy(p->host, ++s, strlen(p->host) + 1);
  else
    strlcpy(p->host, "", strlen(p->host) + 1);

  if (*head == NULL)
  {
    *head = p;
  } else
  {
    q = *head;
    while (q->next != NULL)
      q = q->next;
    q->next = p;
  }

fin:
  if (!res)
  {
    if (p != NULL)
    {
      FREE(p->rcpt);
      FREE(p->email);
      FREE(p->user);
      FREE(p->host);
    }
    FREE(p);
  }

  new = p;
  return new;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
#if 0
int
rcpt_list_del(head, rcpt)
     rcpt_addr_T  **head;
     char          *rcpt;
{
  rcpt_addr_T   *p, *q;

  if (rcpt == NULL || strlen(rcpt) == 0)
    return 0;
  if (*head == NULL)
    return 0;

  return 0;
}
#endif
