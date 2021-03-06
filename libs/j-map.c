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
 *  Creation     : Mon Dec 19 13:53:17 CET 2005
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
#include <j-libjc.h>
#include <j-map.h>


/* TODO ...
**   Verify locks : use map_lock instead of jdb_lock
*/

#if 0
# define     MAP_LOCK(map)          map_lock(map)
# define     MAP_UNLOCK(map)        map_unlock(map)
#else
# define     MAP_LOCK(map)          jdb_lock(&map->db)
# define     MAP_UNLOCK(map)        jdb_unlock(&map->db)
#endif

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_init(map)
     MAP_T              *map;
{
  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);

  return TRUE;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_ok(map)
     MAP_T              *map;
{
  bool                res = FALSE;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);

  res = jdb_ok(&map->db);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_open(map, env, name, rdonly, cache_size)
     MAP_T              *map;
     JDB_ENV_T          *env;
     char               *name;
     int                 rdonly;
     size_t              cache_size;
{
  bool                res = TRUE;
  long                pf = 0;
  char               *s;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);
  ASSERT(name != NULL);
  ASSERT(strlen(name) > 0);

  if (jdb_ok(&map->db))
    return TRUE;

  map->rdonly = rdonly;
  FREE(map->name);
  map->name = strdup(name);
  if (map->name == NULL)
    LOG_SYS_ERROR("strdup(%s)", name);

  s = "ldap:";
  if (strncasecmp(map->name, s, strlen(s)) == 0)
    ;
  s = "db:";
  if (strncasecmp(map->name, s, strlen(s)) == 0)
    ;
  s = "btree:";
  if (strncasecmp(map->name, s, strlen(s)) == 0)
    ;
  s = "hash:";
  if (strncasecmp(map->name, s, strlen(s)) == 0)
    ;
  s = "inet:";
  if (strncasecmp(map->name, s, strlen(s)) == 0)
    ;
  s = "unix:";
  if (strncasecmp(map->name, s, strlen(s)) == 0)
    ;
  s = "local:";
  if (strncasecmp(map->name, s, strlen(s)) == 0)
    ;

  if (strexpr(map->name, "^/.*", NULL, &pf, TRUE))
    ;

  map->cache_size = cache_size;
  map->env = env;

  if (!jdb_ok(&map->db))
    res = jdb_open(&map->db, env, name, (map->rdonly ? 0444 : 0644),
                   map->rdonly, TRUE, map->cache_size);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_close(map)
     MAP_T              *map;
{
  bool                res = TRUE;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);

  if (!jdb_ok(&map->db))
    return res;

  if (jdb_ok(&map->db))
    res = jdb_close(&map->db);

  FREE(map->name);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_reopen(map)
     MAP_T              *map;
{
  bool                res = FALSE;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);

  if (jdb_ok(&map->db))
    res = jdb_close(&map->db);

  res = jdb_open(&map->db, map->env, map->name, (map->rdonly ? 0444 : 0644),
                 map->rdonly, FALSE, map->cache_size);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_lookup(map, key, value, size)
     MAP_T              *map;
     char               *key;
     char               *value;
     size_t              size;
{
  bool                res = FALSE;
  char                k[256];
  char                v[1024];

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);
  ASSERT(key != NULL);

  if (!jdb_ok(&map->db))
  {
    LOG_MSG_ERROR("Lookup on a closed map...");
    return res;
  }

  (void) strlcpy(k, key, sizeof (k));
  (void) strtolower(k);
  memset(v, 0, sizeof (v));

  if (jdb_get_rec(&map->db, k, v, sizeof (v)))
  {
    if (value != NULL && size > 0)
      strlcpy(value, v, size);

    res = TRUE;
  }

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_add(map, key, value, size)
     MAP_T              *map;
     char               *key;
     char               *value;
     size_t              size;
{
  char                k[256];
  bool                res = FALSE;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);
  ASSERT(key != NULL);
  ASSERT(value != NULL);

  (void) strlcpy(k, key, sizeof (k));
  (void) strtolower(k);
  res = jdb_add_rec(&map->db, k, value, size);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_delete(map, key)
     MAP_T              *map;
     char               *key;
{
  bool                res = FALSE;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);
  ASSERT(key != NULL);

  res = jdb_del_rec(&map->db, key);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
#define BSZ     256

bool
map_browse(map, func, arg, skey, ksz, tmax)
     MAP_T              *map;
     MAP_BROWSE_F        func;
     void               *arg;
     char               *skey;
     size_t              ksz;
     timems_T            tmax;
{
  bool                res = FALSE;
  bool                ok;
  char                kbuf[BSZ], vbuf[BSZ];
  timems_T            ti;
  int                 nb = 0;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);

  res = jdb_cursor_open(&map->db, map->rdonly);

  memset(kbuf, 0, sizeof (kbuf));
  memset(vbuf, 0, sizeof (vbuf));
  if (skey != NULL)
    strlcpy(kbuf, skey, sizeof (kbuf));

  ti = time_ms();

  DB_BTREE_SEQ_START();

  for (ok = jdb_cursor_get_first(&map->db, kbuf, BSZ, vbuf, BSZ);
       ok; ok = jdb_cursor_get_next(&map->db, kbuf, BSZ, vbuf, BSZ))
  {
    nb++;
    MESSAGE_INFO(19, "MAP : %-20s %s", kbuf, vbuf);

    DB_BTREE_SEQ_CHECK(kbuf, map->db.database);

    if (func != NULL)
    {
      int                 r = MAP_BROWSE_CONTINUE;

      r = func(kbuf, vbuf, arg);

      if ((r & MAP_BROWSE_DELETE) != 0)
      {
        if (!jdb_cursor_del(&map->db))
          ;
      }
      if ((r & MAP_BROWSE_STOP) != 0)
        break;
    }
    if (nb % 1000 == 0 && tmax > 0)
    {
      timems_T            tf;

      tf = time_ms();
      if (tf - ti > tmax)
        break;
    }
  }
  DB_BTREE_SEQ_END();
  if (!ok)
  {
    if (skey != NULL && ksz > 0)
      *skey = '\0';
  } else
  {
    if (skey != NULL && ksz > 0)
      strlcpy(skey, kbuf, ksz);
  }

  res = jdb_cursor_close(&map->db);

  return TRUE;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_lock(map)
     MAP_T              *map;
{
  bool                res = TRUE;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);

  MUTEX_LOCK(&map->mutex);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_unlock(map)
     MAP_T              *map;
{
  bool                res = TRUE;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);

  MUTEX_UNLOCK(&map->mutex);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
map_flush(map)
     MAP_T              *map;
{
  bool                res = FALSE;

  ASSERT(map != NULL);
  ASSERT(map->signature == SIGNATURE);

  res = jdb_flush(&map->db);

  return res;
}
