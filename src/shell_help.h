/**
 * @file shell_help.h
 * @brief
 * Single function which displays the help of the program.
 */

#ifndef HELP_H_
#define HELP_H_

 /*  ____  _          _ _     _   _      _        */
 /* / ___|| |__   ___| | |_  | | | | ___| |_ __   */
 /* \___ \| '_ \ / _ \ | (_) | |_| |/ _ \ | '_ \  */
 /*  ___) | | | |  __/ | |_  |  _  |  __/ | |_) | */
 /* |____/|_| |_|\___|_|_(_) |_| |_|\___|_| .__/  */
 /*                                       |_|     */

#include <stdio.h>
#include "tools.h"
#include "shell_sclass.h"
#include "shell_memb.h"
#include "shell_separ.h"

 /**
  * @brief
  * Function called by the help command in the shell.
  */
void help(void);


/**
 * @brief
 * Function printing all recognized classes.
 */
void print_classes(void);

#endif // HELP_H_
