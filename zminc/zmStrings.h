/*
 *
 * j-chkmail - Mail Server Filter for sendmail
 *
 * Copyright (c) 2001-2014 - Ecole des Mines de Paris
 *
 *  Auteur       : Jose Marcio Martins da Cruz
 *                 Jose-Marcio.Martins@ensmp.fr
 *
 *  Historique   :
 *  Creation     : Sun Jun 15 21:10:02 CEST 2014
 *
 * This program is free software - GPL v2., 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */


#ifndef ZMSTRINGS_H

char               *zmStrJoin(char *, int, char **);
bool                zmStrRegex(char *, char *, long *, long *, bool);

#if 0
#define STRCASEEQUAL(a,b)                                               \
  ((a) != NULL && (b) != NULL ? strcasecmp((a),(b)) == 0 : ((a) == (b)))

#define STRNCASEEQUAL(a,b,n)                                            \
  ((a) != NULL && (b) != NULL ? strcasecmp((a),(b),(n)) == 0 : ((a) == (b)))

#define STREQUAL(a,b)                                                   \
  ((a) != NULL && (b) != NULL ? strcmp((a),(b)) == 0 : ((a) == (b)))

#endif
#define STRNULL(x,r)             ((x) != NULL ? (x) : (r))
#define STREMPTY(x,r)            ((x) != NULL && strlen(x) > 0 ? (x) : (r))
#define STRBOOL(x,t,f)           ((x) ? t : f)


# define ZMSTRINGS_H    1
#endif             /* ZMSTRINGS_H */
