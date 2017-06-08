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

#ifndef __JSIGNAL_H__

bool                setup_filter_signal_handler(void);

void                launch_periodic_tasks_thread(void);
void                remove_milter_unix_sock (void);

bool                reopen_all_log_files();

#define LIFESIGN_CLEAR      0
#define LIFESIGN_GREY       1
#define LIFESIGN_LOAD       2
#define LIFESIGN_CTRL       4

void                lifesign_set(uint32_t);

#define __JSIGNAL_H__
#endif
