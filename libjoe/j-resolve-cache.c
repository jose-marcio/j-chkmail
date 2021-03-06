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
 *  Creation     : Tue Dec 20 22:45:42 CET 2005
 *
 * This program is free software, but with restricted license 
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
#include <j-resolve-cache.h>

static void         resolve_cache_close();
static void        *resolve_cache_clean(void *);

static MAP_T        map = MAP_INITIALIZER;

static char        *mapname = J_WDBDIR "/j-res-cache.db";
static size_t       mapcachesize = 2 * 1024 * 1024;

#define  VALUE_LEN_MAX        256

#define  RESOLVE_DT_REFRESH      (1 DAYS)
#define  RESOLVE_DT_EXPIRE       (3 DAYS)
#define  RESOLVE_DT_CHECK        (1 HOURS)
#define  RESOLVE_DT_LOOP         (1 MINUTES)
#define  RESOLVE_DT_SYNC         (2 MINUTES)

static time_t       dt_refresh = RESOLVE_DT_REFRESH;
static time_t       dt_sync = RESOLVE_DT_SYNC;
static time_t       dt_check = RESOLVE_DT_CHECK;
static time_t       dt_expire = RESOLVE_DT_EXPIRE;

/* TODO
**
** What fields shall be there ???
** - value (the real value)
** - result
** - timestamp
** - ...
*/

static int          resolve_cyclic_task(void *arg);

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
resolve_cache_check(prefix, key, value, size)
     char               *prefix;
     char               *key;
     char               *value;
     size_t              size;
{
  bool                res = FALSE;
  char               *s = NULL;
  size_t              sz;
  char                buf[VALUE_LEN_MAX];

  ASSERT(key);
  ASSERT(value);
  ASSERT(size >= 0);

  map_lock(&map);

  if (!map_ok(&map) && !resolve_cache_init(NULL, RESOLVE_CACHE_RD))
    goto fin;

  sz = strlen(prefix) + strlen(key) + 4;
  if ((s = malloc(sz)) != NULL)
  {
    /* XXX Add record creation and update time stamps */

    snprintf(s, sz, "%s:%s", prefix, key);
    res = map_lookup(&map, s, buf, sizeof (buf));
    if (res)
    {
      char               *argv[8];
      int                 argc;

      memset(argv, 0, sizeof (argv));
      argc = str2tokens(buf, 8, argv, ";");

      if (argv[2] != NULL)
        strlcpy(value, argv[2], size);
      else
        res = FALSE;
    }
  } else
    LOG_SYS_ERROR("malloc error");
  FREE(s);

fin:
  map_unlock(&map);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
resolve_cache_add(prefix, key, value)
     char               *prefix;
     char               *key;
     char               *value;
{
  bool                res = FALSE;
  char               *s = NULL;
  size_t              sz;

  ASSERT(prefix);
  ASSERT(key);
  ASSERT(value);

  map_lock(&map);

  if (!map_ok(&map) && !resolve_cache_init(NULL, RESOLVE_CACHE_WR))
    goto fin;

  sz = strlen(prefix) + strlen(key) + 4;
  if ((s = malloc(sz)) != NULL)
  {
    char                buf[VALUE_LEN_MAX];
    time_t              first, last, now;
    char               *hostname = value;

    first = now = time(NULL);
    last = 0;

    /* XXX Add record creation and update time stamps */

    memset(buf, 0, sizeof (buf));
    snprintf(s, sz, "%s:%s", prefix, key);

    if (map_lookup(&map, s, buf, sizeof (buf)))
    {
      char               *argv[8];
      int                 argc;
      char               *name = NULL;
      unsigned long       tl;

      memset(argv, 0, sizeof (argv));
      argc = str2tokens(buf, 8, argv, ";");

      if (argv[0] != NULL)
        first = str2ulong(argv[0], NULL, 0);
      if (argv[1] != NULL)
        last = str2ulong(argv[1], NULL, 0);
      hostname = argv[2];
    }

    if (last + dt_refresh < now || !STRCASEEQUAL(value, hostname))
    {
      last = now;
      /* snprintf(s, sz, "%s:%s", prefix, key); */
      snprintf(buf, sizeof (buf), "%lu;%lu;%s", first, last, value);
      res = map_add(&map, s, buf, strlen(buf) + 1);
    }

  } else
    LOG_SYS_ERROR("malloc error");
  FREE(s);

fin:
  map_unlock(&map);

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool
resolve_cache_times(p_sync, p_check, p_expire)
     time_t              p_sync;
     time_t              p_check;
     time_t              p_expire;
{
  if (p_expire > 6 HOURS)
    dt_expire = p_expire;
  if (p_sync > 30 SECONDS)
    dt_sync = p_sync;
  if (p_check > 10 MINUTES)
    dt_check = p_check;

  return TRUE;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
static char         browsekey[256];

bool
resolve_cache_init(dbdir, rwmode)
     char               *dbdir;
     int                 rwmode;
{
  bool                res = TRUE;

  if (!map_ok(&map))
  {
    bool                rdonly;
    char                name[512];

    if (dbdir == NULL || strlen(dbdir) == 0)
      dbdir = J_WDBDIR;
    snprintf(name, sizeof (name), "%s/%s", dbdir, "j-res-cache.db");

    rdonly = rwmode == RESOLVE_CACHE_RD;

    res = map_open(&map, work_db_env, name, rdonly, mapcachesize);

    if (!rdonly)
    {
      (void) atexit(resolve_cache_close);

      memset(browsekey, 0, sizeof (browsekey));
      MESSAGE_INFO(10, "*** Registering resolve cache cyclic task...");
      cyclic_tasks_register(resolve_cyclic_task, NULL, RESOLVE_DT_LOOP);
    }
  }

  return res;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
static void
resolve_cache_close()
{

  (void) map_close(&map);
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
static              bool
resolve_cache_sync()
{
  bool                res = TRUE;

  if (!map_ok(&map))
    goto fin;

  res = map_flush(&map);

fin:
  return res;
}


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
static int
clean_up_cache(key, val, arg)
     char               *key;
     char               *val;
     void               *arg;
{
  int                 r = MAP_BROWSE_CONTINUE;
  time_t             *now = (time_t *) arg;

  if (key == NULL || val == NULL)
    return r;

  MESSAGE_INFO(11, "CALLBACK : key=%-20s val=%s", key, val);

  {
    char               *argv[8];
    int                 argc;
    unsigned long       tl;
    time_t              first, last;

    first = last = (time_t) 0;
    memset(argv, 0, sizeof (argv));
    argc = str2tokens(val, 8, argv, ";");

    if (argv[0] != NULL)
      first = str2ulong(argv[0], NULL, 0);
    if (argv[1] != NULL)
      last = str2ulong(argv[1], NULL, 0);

    if (last + dt_expire < *now)
    {
      MESSAGE_INFO(11, "Resolve cache entry expired %s %lu %lu", key, last);
      r |= MAP_BROWSE_DELETE;
    }
  }

  return r;
}


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
bool                resolve_cache_show_cyclic_task = TRUE;

static int
resolve_cyclic_task(arg)
     void               *arg;
{
  static time_t       last_check = 0, last_sync = 0;
  time_t              now;

  now = time(NULL);
  if (last_check == 0)
    last_check = now;
  if (last_sync == 0)
    last_sync = now;

  MESSAGE_INFO(11, "Cleaning up and syncing map cyclic task...");

  map_lock(&map);

  if (last_check + dt_check <= now)
  {
    timems_T            ti, tf;

    ti = time_ms();

    map_browse(&map, clean_up_cache, &now, browsekey, sizeof (browsekey), 1000);
    last_check = now;

    if (resolve_cache_show_cyclic_task)
    {
      tf = time_ms();
      MESSAGE_INFO(10, "Cleaning up resolve cache map dt = %lu", (long) (tf - ti));
    }
  }
#if 0
  if (last_sync + dt_sync <= now)
  {
    timems_T            ti, tf;
    static int          i = 0;

    ti = time_ms();

    resolve_cache_sync();
    last_sync = now;

    if (resolve_cache_show_cyclic_task && (++i % 6) == 0)
    {
      tf = time_ms();
      MESSAGE_INFO(10, "Syncing resolve cache map dt = %lu", (long) (tf - ti));
    }
  }
#endif

  map_unlock(&map);

  return 0;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
void
resolve_cache_log_enable(enable)
     bool                enable;
{
  resolve_cache_show_cyclic_task = enable;
}
