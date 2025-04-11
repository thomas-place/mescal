#ifndef PARSER_H_
#define PARSER_H_

#include "shell_help.h"
#include "shell_languages.h"
#include "shell_commands.h"
#include "shell_errors.h"
#include "shell_test.h"
#include "main.h"
#include "nfa.h"
#include "printing.h"
#include "regexp_tonfa.h"
#include "regexp.h"
#include "tools.h"
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
