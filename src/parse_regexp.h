#ifndef PARSE_REGEXP_H_
#define PARSE_REGEXP_H_

#include "main.h"
#include "nfa.h"
#include "printing.h"
#include "regexp.h"
#include "regexp_tonfa.h"
#include "shell_commands.h"
#include "shell_help.h"
#include "shell_languages.h"
#include "shell_test.h"
#include "tools.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int regexp_lex(void);

void regexp_error(regexp **thereg, char *s);

regexp *parse_string_regexp(const char *in);

#endif // PARSE_REGEXP_H_
