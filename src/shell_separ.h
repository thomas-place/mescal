/*********************************************/
/* Procèdures de séparation pour l'affichage */
/*********************************************/

#ifndef SHELL_SEPAR_H
#define SHELL_SEPAR_H

#include "alloc.h"
#include "shell_sclass.h"
#include "sep_group.h"
#include "sep_pol.h"
#include "shell_errors.h"

#define BUFFERSIZE 200

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_TEST    "\x1b[48;5;68m\x1b[38;5;226m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/**
 * @brief
 * Array containing the separation functions for all classes.
 */
extern bool (*class_separation[CL_END]) (nfa*, nfa*, bool, FILE*);

void init_separation(void);





#endif
