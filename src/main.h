#ifndef MAIN_H
#define MAIN_H

// ----------------------------//
// NE PAS MODIFIER CE FICHIER //
// ----------------------------//

#include <assert.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "macros_alloc.h"

#define PROMPT ">>> "

// Affiche un message d'aide.
void usage(char* argv[]);

// Parse les arguments de la ligne de commande.
void parse_argv(int argc, char* argv[]);

// Analyse la ligne de commande.
int shell_parse(void);

#endif /* MAIN_H */
