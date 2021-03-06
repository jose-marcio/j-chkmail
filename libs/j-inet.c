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

#include "j-libjc.h"
#include "j-inet.h"

/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
char               *
jinet_ntop(family, addr, cp, size)
     int                 family;
     void               *addr;
     char               *cp;
     size_t              size;
{
#if HAVE_INET_NTOP
  char               *p = NULL;

  if (family == AF_UNSPEC)
    family = AF_INET;

  if ((p = (char *) inet_ntop(family, addr, cp, size)) == NULL)
    LOG_SYS_ERROR("inet_ntop error");
  return p;
#else
#if HAVE_INET_NTOA
  char               *p, *s;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  s = NULL;
  MUTEX_LOCK(&mutex);
  if ((p = inet_ntoa(addr)) != NULL)
  {
    strlcpy(cp, p, size);
    s = cp;
  }
  MUTEX_UNLOCK(&mutex);

  return s;
#else
  return NULL;
#endif
#endif
}

/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
int
jinet_pton(family, cp, addr)
     sa_family_t         family;
     char               *cp;
     void               *addr;
{
  int                 res = 0;

#if HAVE_INET_PTON
  res = inet_pton(family, cp, addr);
  if (res < 0)
    LOG_SYS_ERROR("inet_pton");
  if (res == 0)
    LOG_MSG_ERROR("inet_pton : /%s/ isn't a valid address string",
                  cp != NULL ? cp : "(NULL)");
#else
#ifdef HAVE_INET_ATON
  res = inet_aton(cp, addr) == 0 ? 0 : 1;
  if (res == 0)
    LOG_MSG_ERROR("inet_aton : %s isn't a valid address string",
                  cp != NULL ? cp : "(NULL)");
#endif
#endif
  return res;
}


/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
bool
jsock_ntop(sa, salen, buf, size)
     struct sockaddr    *sa;
     socklen_t           salen;
     char               *buf;
     size_t              size;
{
  switch (sa->sa_family)
  {
    case AF_INET:
      {
        struct sockaddr_in *sin = (struct sockaddr_in *) sa;

        if (jinet_ntop(sa->sa_family, &sin->sin_addr.s_addr, buf, size) == NULL)
          return FALSE;
        return TRUE;
      }
      break;
    case AF_INET6:
      {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;

        if (jinet_ntop(sa->sa_family, &sin6->sin6_addr, buf, size) == NULL)
          return FALSE;
        return TRUE;
      }
      break;
  }
  return FALSE;
}

/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
int
ip_strcmp(a, b)
     char               *a;
     char               *b;
{
  bool                aipv6, bipv6 = FALSE;

  aipv6 = (strchr(a, ':') != NULL);
  bipv6 = (strchr(b, ':') != NULL);

  if (aipv6 && bipv6)
  {
    return strcasecmp(a, b);
  }

  if (!aipv6 && !bipv6)
  {
    struct in_addr      aa, ab;

    /* XXX a voir */
    if (jinet_pton(AF_INET, b, &ab.s_addr)
        && jinet_pton(AF_INET, a, &aa.s_addr))
    {
      if (htonl(aa.s_addr) > htonl(ab.s_addr))
        return 1;
      if (htonl(aa.s_addr) < htonl(ab.s_addr))
        return -1;
      return 0;
    }
  }

  if (aipv6)
    return 1;
  return -1;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
get_hostname(host, size)
     char               *host;
     size_t              size;
{
  ASSERT(host != NULL);
  ASSERT(size > 0);

  memset(host, 0, size);

#if HAVE_GETHOSTNAME
  LOG_MSG_DEBUG(15, "Using gethostname to get host name");

  if (gethostname(host, size) < 0)
  {
    LOG_SYS_ERROR("sysinfo(SI_HOSTNAME) error");
    return FALSE;
  }
  return TRUE;
#else

# if HAVE_UNAME
  {
    struct utsname      udata;

    LOG_MSG_DEBUG(15, "Using uname to get host name");
    if (uname(&udata) < 0)
    {
      LOG_SYS_ERROR("uname error");
      return FALSE;
    }
    strlcpy(host, udata.nodename, size);
    return TRUE;
  }
# else

#  if SYSINFO_NODENAME
  SYSINFO             LOG_MSG_DEBUG(15, "Using sysinfo to get host name");

  if (sysinfo(SI_HOSTNAME, host, sizeof (host)) < 0)
  {
    LOG_SYS_ERROR("sysinfo(SI_HOSTNAME) error");
    return FALSE;
  }
  return TRUE;
#  endif

# endif

#endif
  return FALSE;
}


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
#ifndef NI_MAXHOST
#define NI_MAXHOST  1025
#endif
#ifndef NI_MAXSERV
#define NI_MAXSERV    32
#endif

static              bool
convNameAddr(bout, bin, size, conv2addr)
     char               *bout;
     char               *bin;
     size_t              size;
     bool                conv2addr;
{
  struct addrinfo     hints, *res, *rp;
  bool                ok = TRUE;
  int                 r;

  ASSERT(bout != NULL);
  ASSERT(size > 0);
  memset(bout, 0, size);

  memset(&hints, 0, sizeof (hints));
  hints.ai_family = AF_UNSPEC;  /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
  hints.ai_flags = AI_PASSIVE;  /* For wildcard IP address */
  hints.ai_protocol = 0;        /* Any protocol */
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  r = getaddrinfo(bin, NULL, &hints, &res);
  if (r != 0)
  {
    if (conv2addr && r == EAI_NONAME)
    {
      ok = FALSE;
      goto fin;
    }

    LOG_SYS_ERROR("getaddrinfo(%s,%s) : %d %s",
                  bin,
                  STRBOOL(conv2addr, "name -> addr", "addr -> name"),
                  r, gai_strerror(r));
    ok = FALSE;
    goto fin;
  }

  for (rp = res; rp != NULL; rp = rp->ai_next)
  {
    int                 r;
    char                buf[NI_MAXHOST];

    r = getnameinfo(rp->ai_addr, rp->ai_addrlen,
                    buf, sizeof (buf), NULL, 0, conv2addr ? NI_NUMERICHOST : 0);

    if (r != 0)
    {
      LOG_SYS_ERROR("getnameinfo(%s,%s) : %d %s",
                    bin,
                    STRBOOL(conv2addr, "name -> addr", "addr -> name"),
                    r, gai_strerror(r));
      ok = FALSE;
      goto fin;
    }

    MESSAGE_INFO(12, "IN : %s - OUT : %s", bin, buf);

    if (!conv2addr && STRCASEEQUAL(buf, bin))
      snprintf(bout, size, "[%s]", buf);
    else
      strlcpy(bout, buf, size);

    break;
  }
fin:
  return ok;
}


/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
bool
get_hostbyaddr(ip, name, len)
     char               *ip;
     char               *name;
     int                 len;
{
  return convNameAddr(name, ip, len, FALSE);
}

/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
bool
get_hostbyname(name, ip, len)
     char               *name;
     char               *ip;
     int                 len;
{
  return convNameAddr(ip, name, len, TRUE);
}

/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
char               *
inet_n2p(family, addr, cp, size)
     int                 family;
     void               *addr;
     char               *cp;
     size_t              size;
{
#if HAVE_INET_NTOP
  char               *p = NULL;

  if (family == AF_UNSPEC)
    family = AF_INET;

  if ((p = (char *) inet_ntop(family, addr, cp, size)) == NULL)
    LOG_SYS_ERROR("inet_ntop error");
  return p;
#else
#if HAVE_INET_NTOA
  char               *p, *s;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  s = NULL;
  MUTEX_LOCK(&mutex);
  if ((p = inet_ntoa(addr)) != NULL)
  {
    strlcpy(cp, p, size);
    s = cp;
  }
  MUTEX_UNLOCK(&mutex);

  return s;
#else
  return NULL;
#endif
#endif
}

/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
int
inet_p2n(family, cp, addr)
     sa_family_t         family;
     char               *cp;
     void               *addr;
{
  int                 res = 0;

#if HAVE_INET_PTON
  res = inet_pton(family, cp, addr);
  if (res < 0)
    LOG_SYS_ERROR("inet_pton");
  if (res == 0)
    LOG_MSG_ERROR("inet_pton : /%s/ isn't a valid address string",
                  cp != NULL ? cp : "(NULL)");
#else
#ifdef HAVE_INET_ATON
  res = inet_aton(cp, addr) == 0 ? 0 : 1;
  if (res == 0)
    LOG_MSG_ERROR("inet_aton : %s isn't a valid address string",
                  cp != NULL ? cp : "(NULL)");
#endif
#endif
  return res;
}


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
get_hostbysock(sock, slen, addr, alen, name, nlen)
     struct sockaddr    *sock;
     socklen_t           slen;
     char               *addr;
     size_t              alen;
     char               *name;
     size_t              nlen;
{
  char                buf[NI_MAXHOST];
  int                 r;
  bool                ok = TRUE;

  if (addr != NULL && alen > 0)
  {
    memset(buf, 0, sizeof (buf));
    memset(addr, 0, alen);
    r = getnameinfo(sock, slen, buf, sizeof (buf), NULL, 0, NI_NUMERICHOST);
    if (r != 0)
    {
      LOG_SYS_ERROR("getnameinfo(%s) : %d %s", "name -> addr", r,
                    gai_strerror(r));
      ok = FALSE;
      goto fin;
    }
    strlcpy(addr, buf, alen);
  }

  if (name != NULL && nlen > 0)
  {
    memset(buf, 0, sizeof (buf));
    memset(name, 0, nlen);
    r = getnameinfo(sock, slen, buf, sizeof (buf), NULL, 0, 0);
    if (r != 0)
    {
      LOG_SYS_ERROR("getnameinfo(%s) : %d %s", "addr -> name", r,
                    gai_strerror(r));
      ok = FALSE;
      goto fin;
    }
#if 0
    if (STRCASEEQUAL(buf, bin))
      snprintf(bout, size, "[%s]", buf);
    else
      strlcpy(bout, buf, size);
#else
    strlcpy(name, buf, nlen);
#endif
  }

fin:
  return ok;
}


/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
int
Ip_strcmp(a, b)
     char               *a;
     char               *b;
{
  bool                aip, bip = FALSE;

  aip = (strchr(a, ':') != NULL);
  bip = (strchr(b, ':') != NULL);

  if (aip && bip)
  {
    return strcasecmp(a, b);
  }

  if (!aip && !bip)
  {
    struct in_addr      aa, ab;

    /* XXX a voir */
    if (inet_p2n(AF_INET, b, &ab.s_addr) && inet_p2n(AF_INET, a, &aa.s_addr))
    {
      if (htonl(aa.s_addr) > htonl(ab.s_addr))
        return 1;
      if (htonl(aa.s_addr) < htonl(ab.s_addr))
        return -1;
      return 0;
    }
  }

  if (aip)
    return 1;
  return -1;
}


/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
#if HAVE_POLL
# define J_POLL_RD_FLAGS (POLLIN | POLLPRI | POLLHUP)
# define J_POLL_WR_FLAGS (POLLOUT | POLLHUP)
#endif             /* HAVE_POLL */

#ifndef FDREADY_LOG_LEVEL
#define FDREADY_LOG_LEVEL 0
#endif

int
jfd_ready(sd, fdmode, to)
     int                 sd;
     bool                fdmode;
     long                to;
{
#if HAVE_POLL
  struct pollfd       pfd;
  int                 r;
  time_t              now = time(NULL);
  int                 nerr = 0;

  int                 flags;

  flags = (fdmode == J_SOCK_READ ? J_POLL_RD_FLAGS : J_POLL_WR_FLAGS);

  for (;;)
  {
    pfd.fd = sd;
    pfd.events = flags;
    pfd.revents = 0;
    r = poll(&pfd, 1, to);
    if (r == 0)
      return J_SOCK_TIMEOUT;

#if 0
    if ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0)
    {
      MESSAGE_INFO(FDREADY_LOG_LEVEL, " POLL ERR/HUP/NVAL = %s/%s/%s",
                   STRBOOL((pfd.revents & POLLERR) == TRUE, "T", "F"),
                   STRBOOL((pfd.revents & POLLHUP) == TRUE, "T", "F"),
                   STRBOOL((pfd.revents & POLLNVAL) == TRUE, "T", "F"));
    }
#endif
    if (r < 0)
    {
      if (errno == EINTR)
      {
        if (++nerr > 100)
          return J_SOCK_ERROR;
        continue;
      }
      LOG_SYS_ERROR("poll(%ld)", (long) pfd.fd);
      return J_SOCK_ERROR;
    }
    nerr = 0;
    if ((pfd.revents & flags) != 0)
      return J_SOCK_READY;

    if ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0)
      return J_SOCK_ERROR;

    if (time(NULL) > (now + to / 1000))
      return J_SOCK_TIMEOUT;
  }
#if 0
  LOG_MSG_ERR
    ("ERROR=poll:mi_rd_socket_ready, socket=%d, r=%d, errno=%d revents=%d", sd,
     r, errno, pfd.revents);
  return J_SOCK_ERROR;
#endif
#else              /* HAVE_POLL */
  fd_set              rdfs, excfs;
  int                 r;
  struct timeval      timeout;

  for (;;)
  {
    timeout.tv_usec = 1000 * (to % 1000);
    timeout.tv_sec = to / 1000;
    FD_ZERO(&rdfs);
    FD_SET((unsigned int) sd, &rdfs);
    FD_ZERO(&excfs);
    FD_SET((unsigned int) sd, &excfs);
    if (fdmode == J_SOCK_READ)
      r = select(sd + 1, &rdfs, NULL, &excfs, timeout);
    else
      r = select(sd + 1, NULL, &rdfs, &excfs, timeout);
    if (r == 0)
      return J_SOCK_TIMEOUT;
    if (r < 0)
    {
      if (errno == EINTR)
        continue;
      return J_SOCK_ERROR;
    }
    if (FD_ISSET(sd, &excfs))
      return J_SOCK_ERROR;
    if (FD_ISSET(sd, &rdfs))
      return J_SOCK_READY;
  }
#endif             /* HAVE_POLL */
  return J_SOCK_ERROR;
}

/* ****************************************************************************
 *                                                                            * 
 *                                                                            *
 **************************************************************************** */
#define MAX_EINTR_ERRORS   24

bool
sd_printf(int sd, char *format, ...)
{
  va_list             arg;
  char                str[1024];
  int                 result = FALSE;
  int                 nerr = 0;

  memset(str, 0, sizeof(str));
  va_start(arg, format);
  (void) vsnprintf(str, sizeof (str), format, arg);
  va_end(arg);

  while (TRUE)
  {
    int                 r;

    r = sendto(sd, str, strlen(str), 0, NULL, 0);
    if (r == -1)
    {
      if (nerr++ > MAX_EINTR_ERRORS)
        break;

      if (errno == EINTR)
        continue;

      LOG_SYS_ERROR("sendto error (r = %d)", r);
      break;
    }
    if (r == 0)
    {
      LOG_MSG_ERROR("sendto error (r = %d)", r);
      break;
    }

    return TRUE;
  }

  return result;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
int
sd_readln(fd, buf, size)
     int                 fd;
     char               *buf;
     size_t              size;
{
  char               *p = buf;

  if (fd < 0)
    return -1;

  *p = '\0';
  while (size > 0)
  {
    int                 n;

    n = recvfrom(fd, p, 1, MSG_DONTWAIT, NULL, NULL);
    if (n == 0)
      break;
    if (n < 0)
    {
      if (errno == EINTR || errno == EAGAIN)
        continue;
      LOG_SYS_ERROR("read error");
      break;
    }

    if (*p == '\r')
      continue;
    if (*p == '\n')
      break;
    p++;
    size--;
  }
  *p = '\0';
  return strlen(buf);
}
