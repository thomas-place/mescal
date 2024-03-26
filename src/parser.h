#ifndef PARSER_H_
#define PARSER_H_

#include "shell_languages.h"
#include "shell_commands.h"
#include "main.h"
#include "nfa.h"
#include "printing.h"
#include "reg2aut.h"
#include "regexp.h"
#include "tools.h"
#include "type_unionfind.h"
#include "graphs_tarjan.h"
#include "shell_test.h"
#include "shell_help.h"
#include "shell_sepmem.h"
#include "class_group.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int yylex(void);

void shell_error(char* s);

void prompt(void);

void parse_string(const char* in);

#endif // PARSER_H_
