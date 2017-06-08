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


#ifndef __JLINKEDLIST_H__

typedef struct LISTR_T LISTR_T;

struct LISTR_T
{
  LISTR_T            *prev;
  LISTR_T            *next;
  char               *key;
  void               *data;
  size_t              size;
  int                 count;
};

typedef void        (*LISTCLEAR_F) (void *);

LISTR_T            *linked_list_add(LISTR_T *, char *, int, void *, size_t);
LISTR_T            *linked_list_set(LISTR_T *, char *, int, void *, size_t);
bool                linked_list_remove(LISTR_T *, char *, LISTCLEAR_F);
LISTR_T            *linked_list_find(LISTR_T *, char *);
bool                linked_list_clear(LISTR_T *, LISTCLEAR_F);

int                 linked_list_count_set(LISTR_T *, char *, int);
int                 linked_list_count_get(LISTR_T *, char *);
int                 linked_list_count_inc(LISTR_T *, char *);

#define __JLINKEDLIST_H__
#endif
