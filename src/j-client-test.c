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
#include <j-chkmail.h>


/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
#define             DEF_TUPLE         "NET,HOST,FULL"
#define             DEF_TIMES         "0,1000,0,1000"

struct conf_T
{
  char               *workdir;
  int                 count;

  char               *times;
  char               *tuples;
};

typedef struct conf_T conf_T;

#define CONF_INITIALIZER   {"/tmp/j-grey-bench", 10000, DEF_TIMES, DEF_TUPLE}

static char        *workdir = "/var/jgreyd";

static char        *ntuple = NULL;

static char        *tconst = NULL;

static int          do_test(conf_T * cargs);

static void         usage(char *);


int
main(argc, argv)
     int                 argc;
     char              **argv;
{
  int                 i;

  conf_T              cargs = CONF_INITIALIZER;

  set_log_output(TRUE, TRUE);

  log_level = 7;

  /*
   ** 1. Read configuration parameters
   */
  {
    const char         *args = "ht:dvw:n:l:c:";
    int                 c;

    while ((c = getopt(argc, argv, args)) != -1)
    {
      switch (c)
      {
          /* help */
        case 'h':
          usage(argv[0]);
          exit(0);
          break;

          /* time constants */
        case 't':
          tconst = optarg;
          break;

          /* verbose */
        case 'v':
          log_level++;
          break;

          /* group */
        case 'l':
          log_level = atoi(optarg);
          break;

        case 'c':
          cargs.count = atoi(optarg);
          break;

          /* Work directory */
        case 'w':
          workdir = optarg;
          break;

        case 'n':
          ntuple = optarg;
          break;

        default:
          usage(argv[0]);
          printf("Error ... \n");
          exit(0);
      }
    }

  }

  configure("j-client-test", conf_file, FALSE);

  {
    client_T client = CLIENT_INITIALIZER;
    char  buf[1024];
    int i;
    int fd;
    char fname[512];
    int tfd;
    FILE *fin;

    printf("* Connect \n");
    if (!client_connect(&client, "inet:2012@127.0.0.1", 10))
      goto fin;

    printf("* Receive \n");
    memset(buf, 0, sizeof(buf));
    if (!client_recv(&client, buf, sizeof(buf)))
      ;

    printf("* Send \n");
    strlcpy(buf, "download pending\r\n", sizeof(buf));
    if (!client_send(&client, buf, strlen(buf)))
      ;

    printf("* Receive \n");
    strlcpy(fname, "pending.XXXXXX", sizeof(fname));
    fd = mkstemp(fname);
    for (;;) {
      memset(buf, 0, sizeof(buf));
      if (!client_recv(&client, buf, sizeof(buf)))
	break;
      printf("%s", buf);
      write(fd, buf, strlen(buf));
    }
    close(fd);

    printf("* Send \n");
    strlcpy(buf, "download valid\r\n", sizeof(buf));
    if (!client_send(&client, buf, strlen(buf)))
      ;

    printf("* Receive \n");
    strlcpy(fname, "valid.XXXXXX", sizeof(fname));
    fd = mkstemp(fname);
    for (;;) {
      memset(buf, 0, sizeof(buf));
#if 1
      if (!client_readln(&client, buf, sizeof(buf)))
	break;
      printf("%s\n", buf);
      write(fd, buf, strlen(buf));
      write(fd, "\n", 1);
#else
      if (!client_recv(&client, buf, sizeof(buf)))
	break;
      printf("%s", buf);
      write(fd, buf, strlen(buf));
#endif
    }
    close(fd);

    printf("* Send \n");
    strlcpy(buf, "download white\r\n", sizeof(buf));
    if (!client_send(&client, buf, strlen(buf)))
      ;

    printf("* Receive \n");
    strlcpy(fname, "white.XXXXXX", sizeof(fname));
    fd = mkstemp(fname);
    for (;;) {
      memset(buf, 0, sizeof(buf));
      if (!client_recv(&client, buf, sizeof(buf)))
	break;
      printf("%s", buf);
      write(fd, buf, strlen(buf));
    }
    close(fd);

    printf("* Send \n");
    strlcpy(buf, "quit\r\n", sizeof(buf));
    if (!client_send(&client, buf, strlen(buf)))
      ;

  fin:
    client_disconnect(&client, FALSE);
  }

  return 0;
}



/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
static int
do_test(conf_T * cargs)
{
  char               *ip = "", *from = "", *to = "", *hostname = "";
  int                 n = 0;
  char                buf[256];

  time_t              ti, tf;

  ip = "1.1.1.1";
  from = "nobody@j-chkmail.org";
  hostname = "nowhere.j-chkmail.org";
  to = "grey@j-chkmail.org";

  ti = time_ms();
  for (n = 0; n < cargs->count; n++)
  {
    int                 r = GREY_OK;
    char               *s = NULL;
    bool                new = FALSE;
    bool                can_validate = TRUE;

    snprintf(buf, sizeof (buf), "grey-%d@j-chkmail.org", n);
    to = buf;

    r = remote_grey_check(ip, from, to, hostname);
    switch (r)
    {
      case GREY_OK:
        s = "OK";
        break;
      case GREY_WAIT:
        s = "WAIT";
        break;
      case GREY_ERROR:
        s = "ERROR";
        break;
      case GREY_REJECT:
        s = "REJECT";
        break;
    }
  }
  tf = time_ms();

  MESSAGE_INFO(1, "Entries checked : %5d\n", n);
  MESSAGE_INFO(1, "Time elapsed    : %5d ms", tf - ti);

  return 0;
}



/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
void
usage(arg)
     char               *arg;
{
  printf("Usage : %s options\n"
         "  %s\n" "  Compiled on %s\n", arg, PACKAGE, __DATE__ " " __TIME__);
}
