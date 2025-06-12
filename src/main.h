/**
 * @file main.h
 * @brief
 * The main file of the program.
 */

#ifndef MAIN_H
#define MAIN_H

#include <assert.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alloc.h"
#include "interrupt.h"
#include "nfa.h"
#include "printing.h"
#include "readline.h"
#include "regexp.h"
#include "shell_keywords.h"
#include "string.h"
#include "time.h"
#include "tools.h"

 /**
  * @brief
  * Print the usage of the program.
  */
void usage(char* argv[]);

/**
 * @brief
 * Parse the command line arguments.
 */
void parse_argv(int argc, char* argv[]);

/**
 * @brief
 * Parse the command line of the shell.
 */
int shell_parse(void);

#endif /* MAIN_H */
