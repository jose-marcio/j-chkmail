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


#ifndef __JCHKMAIL_H__

#include "version.h"
#include "defs.h"

#include "j-sys.h"

#include <j-libjc.h>


typedef struct CONNID_T CONNID_T;
typedef struct spamchk_T spamchk_T;

struct CONNID_T
{
  uint32_t            t[2];
  char                id[16];
};

#include "j-modules.h"
#include "j-read-conf-data.h"
#include "j-headers.h"
#include "j-oracle-scores.h"
#include "j-chkcontent.h"
#include "j-oracle.h"
#include "j-fileexp.h"
#include "j-cf.h"
#include "j-log.h"
#include "j-proc-witness.h"
#include "j-config.h"
#include "j-stats.h"
#include "mimelist.h"
#include "scanmail.h"
#include "j-signal.h"
#include <j-callback.h>
#include "j-ndc-help.h"
#include "j-ndc-server.h"
#include "j-lr-init.h"
#include "j-main.h"
#include "j-cleanspool.h"
#include "j-throttle.h"
#include "j-smtprate.h"
#include "j-connopen.h"
#include "j-livehistory.h"
#include "j-entropy.h"
#include "j-dns-iprbwl.h"
#include "j-dns-urlbl.h"
#include "j-resource.h"
#include "j-load.h"
#include "j-dbbl.h"
#include "j-rcpt-list.h"
#include "j-dbpolicy.h"
#include "j-policy.h"

#include "j-dbrcpt.h"
#include "j-rcpt.h"
#include "j-grey.h"
#include "j-grey-client.h"
#include "j-grey-cleanup.h"
#include "j-netclass.h"
#include "j-mailregex.h"

#include "zmChkLib.h"
/* #include "zmPolicy.h" */

#include "macros.h"

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
#define DEBUG     0

#define SZ_CHUNK         8192

#define MAX_LINE_LEN     2048


int                 j_chkmail();

int                 count_connections(int);

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */


#define  CONNID_STR(connid)       ((char *) (connid).id)
#define  CONNID_INT(connid)       ((uint32_t) (connid).t[0])


int                 active_connections(int);

extern time_t       tlongconn;

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */

#if 0
#define           CALLBACK_FIRST        0
#define           CALLBACK_CONNECT      0
#define           CALLBACK_EHLO         1
#define           CALLBACK_MAIL         2
#define           CALLBACK_RCPT         3
#define           CALLBACK_DATA         4
#define           CALLBACK_HEADER       5
#define           CALLBACK_EOH          6
#define           CALLBACK_BODY         7
#define           CALLBACK_EOM          8
#define           CALLBACK_ABORT        9
#define           CALLBACK_CLOSE        10
#define           CALLBACK_UNKNOWN      11
#define           CALLBACK_LAST         11

#define  CALLBACK_LABEL(i)			\
  ((i) == CALLBACK_CONNECT ? "CONNECT" :	\
   (i) == CALLBACK_EHLO ? "HELO" :		\
   (i) == CALLBACK_MAIL ? "MAIL" :		\
   (i) == CALLBACK_RCPT ? "RCPT" :		\
   (i) == CALLBACK_DATA ? "DATA" :		\
   (i) == CALLBACK_HEADER ? "HEADER" :		\
   (i) == CALLBACK_EOH ? "EOH" :		\
   (i) == CALLBACK_BODY ? "BODY" :		\
   (i) == CALLBACK_EOM ? "EOM" :		\
   (i) == CALLBACK_ABORT ? "ABORT" :		\
   (i) == CALLBACK_CLOSE ? "CLOSE" : "UNKNOWN")

#define  CALLBACK_VALUE(label)						\
  (STRCASEEQUAL((label), "CONNECT") ? CALLBACK_CONNECT :		\
   STRCASEEQUAL((label), "HELO") ?    CALLBACK_EHLO :			\
   STRCASEEQUAL((label), "EHLO") ?    CALLBACK_EHLO :			\
   STRCASEEQUAL((label), "MAIL") ?    CALLBACK_MAIL :			\
   STRCASEEQUAL((label), "RCPT") ?    CALLBACK_RCPT :			\
   STRCASEEQUAL((label), "DATA") ?    CALLBACK_DATA :			\
   STRCASEEQUAL((label), "HEADER") ?  CALLBACK_HEADER :			\
   STRCASEEQUAL((label), "EOH") ?     CALLBACK_EOH :			\
   STRCASEEQUAL((label), "BODY") ?    CALLBACK_BODY :			\
   STRCASEEQUAL((label), "EOM") ?     CALLBACK_EOM :			\
   STRCASEEQUAL((label), "ABORT") ?   CALLBACK_ABORT :			\
   STRCASEEQUAL((label), "CLOSE") ?   CALLBACK_CLOSE : CALLBACK_UNKNOWN)
#endif

#define RESOLVE_NULL      -1
#define RESOLVE_OK        0
#define RESOLVE_FAIL      1
#define RESOLVE_FORGED    2
#define RESOLVE_TEMPFAIL  4

#define RESOLVE_ALL   (RESOLVE_FAIL | RESOLVE_FORGED | RESOLVE_TEMPFAIL)

#define  RESOLVE_VAL(i)   ((i) == RESOLVE_OK ? "OK" : \
                           (i) == RESOLVE_FAIL ? "FAIL" : \
                           (i) == RESOLVE_FORGED ? "FORGED" : \
                           (i) == RESOLVE_TEMPFAIL ? "TEMPFAIL" : "NULL")


#define JCOMBUFSZ         1024

#define CACHE_GETHOSTNAMEBYADDR(ip,name,size,query)	\
  do {							\
    *name = '\0';					\
    if (cf_get_int(CF_RESOLVE_CACHE_ENABLE) == OPT_YES)	\
      resolve_cache_check("PTR", ip, name, size);	\
    if (strlen(name) == 0 && query)			\
      get_hostbyaddr(ip,name,size);                     \
  } while (0);


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */

/* _FFR 

_FFR_MSG_ENTROPY
_FFR_LOAD_CONTROL
_FFR_GLOBAL_OPEN_CONNECTIONS_CONTROL
_FFR_RECIPIENT_OPTIONS
_FFR_RFC2046_MSGS_ARE_XFILES

*/

/* to be removed from C files */
#ifndef _FFR_CLEAN_MSG_BUF
# define _FFR_CLEAN_MSG_BUF                     1
#endif

#define _FFR_RFC2046_MSGS_ARE_XFILES           1

#define __JCHKMAIL_H__
#endif
