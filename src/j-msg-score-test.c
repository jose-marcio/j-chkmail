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

#if 1
#define COEFS "KBAYES=1.0, KURLBL=0.04, KREGEX=0.02, KORACLE=0.08  "
#define SCALE "SSCORE=7."
#endif

int
main(argc, argv)
     int                 argc;
     char              **argv;
{
  int                 i;

  int                 res = 0;
  extern int          log_level;

  bool                eval = FALSE;
  char               *s;
  double              score = 0.;
  char                buf[256];

  msg_scores_T        scores;

  memset(&scores, 0, sizeof (scores));
  scores.bayes = 0.4;
  scores.urlbl = 10;
  scores.body = 0;
  scores.headers = 0;
  scores.oracle = 0;


  log_level = 10;
  set_log_output(FALSE, TRUE);

  MESSAGE_INFO(9, "\n --- ");
  s = "VECTOR; " COEFS;

  eval = register_msg_action(MSG_ACTION_OK, "REGEX:");
  printf("EVAL = %d\n", eval);
  eval = register_msg_action(MSG_ACTION_REJECT, "REGEX:");
  printf("EVAL = %d\n", eval);
  eval = register_msg_action(MSG_ACTION_DISCARD, "REGEX:");
  printf("EVAL = %d\n", eval);
  eval = register_msg_action(MSG_ACTION_QUARANTINE, "REGEX:");
  printf("EVAL = %d\n", eval);
  eval = register_msg_action(MSG_ACTION_HEADER_HAM, "REGEX:");
  printf("EVAL = %d\n", eval);
  eval = register_msg_action(MSG_ACTION_HEADER_SPAM_LO, "REGEX:");
  printf("EVAL = %d\n", eval);
  eval = register_msg_action(MSG_ACTION_HEADER_SPAM_HI, "THRESHOLD:0.650");
  printf("EVAL = %d\n", eval);

  s = "REGEX: j-chkmail score : XXX.*U=##";
  eval = register_msg_action(MSG_ACTION_HEADER_NEUTRAL, s);
  printf("EVAL = %d\n", eval);

  eval = evaluate_msg_action(MSG_ACTION_HEADER_SPAM_HI, NULL, 0.7, NULL);
  printf("ACTION = %d\n", eval);
  eval = evaluate_msg_action(MSG_ACTION_HEADER_SPAM_HI, NULL, 0.8, NULL);
  printf("ACTION = %d\n", eval);

  eval = evaluate_msg_action(MSG_ACTION_HEADER_NEUTRAL, NULL, 0.8, NULL);
  printf("ACTION = %d\n", eval);

  s =
    "MSGID : NOID on UNKNOWN : j-chkmail score : XX : R=. U=# O=. B=0.400 -> S=0.566";
  eval = evaluate_msg_action(MSG_ACTION_HEADER_NEUTRAL, NULL, 0.8, s);
  printf("ACTION = %d\n", eval);

  s =
    "MSGID : NOID on UNKNOWN : j-chkmail score : XXX : R=. U=## O=. B=0.400 -> S=0.566";
  eval = evaluate_msg_action(MSG_ACTION_HEADER_NEUTRAL, NULL, 0.8, s);
  printf("ACTION = %d\n", eval);


  s = NULL;
  eval = configure_msg_eval_function(s);
  printf("* %-60s : %s\n", STRNULL(s, "NULL"), STRBOOL(eval, "OK", "KO"));
  display_msg_eval();

  score = compute_msg_score(&scores);
  MESSAGE_INFO(9, "  SCORE : %7.3f", score);
  create_msg_score_header(buf, sizeof (buf), NULL, NULL, &scores);
  MESSAGE_INFO(9, "  HEADER : %s", buf);

#if 0
  MESSAGE_INFO(9, "\n --- ??? ");
  score = compute_msg_score(regex, oracle, urlbl, -1.);
  MESSAGE_INFO(9, "  SCORE : %7.3f", score);
  create_msg_score_header(buf, sizeof (buf), NULL, NULL, regex, oracle, urlbl, -1);
  MESSAGE_INFO(9, "  HEADER : %s", buf);

  MESSAGE_INFO(9, "\n --- ??? ");
  score = evaluate_msg_score(regex, oracle, urlbl, -1.);
  MESSAGE_INFO(9, "  SCORE : %7.3f", score);
  create_msg_score_header(buf, sizeof (buf), NULL, NULL, regex, oracle, urlbl, -1);
  MESSAGE_INFO(9, "  HEADER : %s", buf);
#endif

#if 0
  MESSAGE_INFO(9, "\n --- ");
  s = "VEC; " COEFS;
  eval = configure_msg_eval_function(s);
  printf("* %-60s : %s\n", STRNULL(s, "NULL"), STRBOOL(eval, "OK", "KO"));
  display_msg_eval();

  score = evaluate_msg_score(regex, oracle, urlbl, bayes);
  MESSAGE_INFO(9, "  SCORE : %7.3f", score);
  create_msg_score_header(buf, sizeof (buf), NULL, NULL,
                          regex, oracle, urlbl, bayes);
  MESSAGE_INFO(9, "  HEADER : %s", buf);
#endif

#if 0
  MESSAGE_INFO(9, "\n --- ");
  s = "VECTOR; " COEFS;
  eval = configure_msg_eval_function(s);
  printf("* %-60s : %s\n", STRNULL(s, "NULL"), STRBOOL(eval, "OK", "KO"));
  display_msg_eval();

  score = evaluate_msg_score(regex, oracle, urlbl, bayes);
  MESSAGE_INFO(9, "  SCORE : %7.3f", score);
  create_msg_score_header(buf, sizeof (buf), NULL, NULL,
                          regex, oracle, urlbl, bayes);
  MESSAGE_INFO(9, "  HEADER : %s", buf);
#endif

  return 0;
}
