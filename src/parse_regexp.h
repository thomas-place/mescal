#ifndef PARSE_REGEXP_H_
#define PARSE_REGEXP_H_

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

int regexp_lex(void);

void regexp_error(regexp** thereg, char* s);

regexp* parse_string_regexp(const char* in);

#endif // PARSE_REGEXP_H_
