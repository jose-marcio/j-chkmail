/*
 *
 * j-chkmail - Mail Server Filter for sendmail
 *
 * Copyright (c) 2001-2007 - Ecole des Mines de Paris
 *
 *  Auteur       : Jose Marcio Martins da Cruz
 *                 Jose-Marcio.Martins@ensmp.fr
 *
 *  Historique   :
 *  Creation     : Sun Aug 19 22:21:21 CEST 2007
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
#include <j-bestof-n.h>

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
static int
dblcmp(a, b)
     const void         *a;
     const void         *b;
{
  double             *ra, *rb;

  ra = (double *) a;
  rb = (double *) b;

  return (int) (fabs(*ra) - fabs(*rb));
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
bestof_init(b, dim, bcmp)
     bestof_T           *b;
     int                 dim;
     bestcomp_F          bcmp;
{
  ASSERT(b != NULL);

  memset(b, 0, sizeof (*b));

  b->dim = dim;
  b->n = 0;
  if (bcmp != NULL)
    b->bcmp = bcmp;
  else
    b->bcmp = dblcmp;

  return TRUE;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
bestof_add(b, v)
     bestof_T           *b;
     double              v;
{
  if (b->n < b->dim)
  {
    b->best[b->n++] = v;
    return TRUE;
  }

  qsort(b->best, b->n, sizeof (double), b->bcmp);
  if (b->bcmp(&v, &b->best[0]) > 0)
    b->best[0] = v;

  return TRUE;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
double
bestof_average(b)
     bestof_T           *b;
{
  int                 i;
  double              v = 0.;

  if (b->n <= 0)
    return 0.;

  for (i = 0; i < b->n; i++)
    v += b->best[i];

  return v / b->n;
}
