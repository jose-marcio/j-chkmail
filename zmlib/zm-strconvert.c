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
 *  Creation     : Fri Oct 27 10:46:40 CEST 2006
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


#include <zm-sys.h>
#include <zmlibs.h>


/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
long
str2long(s, eno, dval)
     char               *s;
     int                *eno;
     long                dval;
{
  char               *eptr;
  long                r = 0;

  if (s == NULL)
    return 0;

  errno = 0;
#if HAVE_STRTOL
  r = strtol(s, &eptr, 10);

  if (eno != NULL)
    *eno = errno;
  switch (errno)
  {
    case ERANGE:
    case EINVAL:
      return dval;
      break;
  }

  if (strlen(eptr) > 0)
  {
    eptr += strspn(eptr, " \t");
    switch (*eptr)
    {
      case 'K':
        r *= 1024;
        break;
      case 'M':
        r *= 1024 * 1024;
        break;
      case 'G':
        r *= 1024 * 1024 * 1024;
        break;
      case 's':
        r *= 1;
        break;
      case 'm':
        r *= 60;
        break;
      case 'h':
        r *= 60 * 60;
        break;
      case 'd':
        r *= 60 * 60 * 24;
        break;
      case 'w':
        r *= 7 * 60 * 60 * 24;
        break;
    }
  }
#else
  r = atol(s);
#endif

  return r;
}

/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
unsigned long
str2ulong(s, eno, dval)
     char               *s;
     int                *eno;
     unsigned long       dval;
{
  unsigned long       r = 0;

  if (s == NULL)
    return 0;

  errno = 0;
#if HAVE_STRTOUL
  r = strtoul(s, NULL, 10);

  if (eno != NULL)
    *eno = errno;
  switch (errno)
  {
    case ERANGE:
    case EINVAL:
      return dval;
      break;
  }
#else
  r = atoul(s);
#endif

  return r;
}

/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
unsigned long long
str2ulonglong(s, eno, dval)
     char               *s;
     int                *eno;
     unsigned long long  dval;
{
  unsigned long long  r = 0;

  if (s == NULL)
    return 0;

  errno = 0;
#if HAVE_STRTOULL
  r = strtoull(s, NULL, 10);

  if (eno != NULL)
    *eno = errno;
  switch (errno)
  {
    case ERANGE:
    case EINVAL:
      return dval;
      break;
  }
#else
  r = (unsigned long long) atoll(s);
#endif

  return r;
}

/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
long long
str2longlong(s, eno, dval)
     char               *s;
     int                *eno;
     long long           dval;
{
  long long           r = 0;

  if (s == NULL)
    return 0;

  errno = 0;
#if HAVE_STRTOLL
  r = strtoll(s, NULL, 10);

  if (eno != NULL)
    *eno = errno;
  switch (errno)
  {
    case ERANGE:
    case EINVAL:
      return dval;
      break;
  }
#else
  r = atoll(s);
#endif

  return r;
}

/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
double
str2double(s, eno, dval)
     char               *s;
     int                *eno;
     double              dval;
{
  double              r = 0;

  if (s == NULL)
    return 0;

  errno = 0;
#if HAVE_STRTOD
  r = strtod(s, NULL);

  if (eno != NULL)
    *eno = errno;
  switch (errno)
  {
    case ERANGE:
    case EINVAL:
      return dval;
      break;
  }
#else
  r = atof(s);
#endif

  return r;
}


/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
size_t
str2size(s, eno, dval)
     char               *s;
     int                *eno;
     size_t              dval;
{
  char               *eptr;
  size_t              r = 0;

  if (s == NULL)
    return 0;

  errno = 0;
#if HAVE_STRTOUL
  r = strtoul(s, &eptr, 10);

  if (eno != NULL)
    *eno = errno;
  switch (errno)
  {
    case ERANGE:
    case EINVAL:
      return dval;
      break;
  }
#else
  r = atoul(s);
  eptr = s + strspn(s, "01234567890");
#endif

  if (strlen(eptr) > 0)
  {
    eptr += strspn(eptr, " \t");
    switch (*eptr)
    {
      case 'K':
      case 'k':
        r *= 1024;
        break;
      case 'M':
      case 'm':
        r *= 1024 * 1024;
        break;
      case 'G':
      case 'g':
        r *= 1024 * 1024 * 1024;
        break;
    }
  }

  return r;
}

/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
time_t
str2time(s, eno, dval)
     char               *s;
     int                *eno;
     time_t              dval;
{
  char               *eptr;
  time_t              r = 0;

  if (s == NULL)
    return 0;

  errno = 0;
#if HAVE_STRTOUL
  r = strtoul(s, &eptr, 10);

  if (eno != NULL)
    *eno = errno;
  switch (errno)
  {
    case ERANGE:
    case EINVAL:
      return dval;
      break;
  }
#else
  r = atoul(s);
  eptr = s + strspn(s, "01234567890");
#endif

  if (strlen(eptr) > 0)
  {
    eptr += strspn(eptr, " \t");
    switch (*eptr)
    {
      case 's':
        r *= 1;
        break;
      case 'm':
        r *= 60;
        break;
      case 'h':
        r *= 60 * 60;
        break;
      case 'd':
        r *= 60 * 60 * 24;
        break;
      case 'w':
        r *= 7 * 60 * 60 * 24;
        break;
    }
  }

  return r;
}
