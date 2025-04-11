%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "main.h"
#include "error.h"
#include "alloc.h"
#include "regexp.h"
#include "shell_commands.h"
#include "readline.h"

#include "parser.tab.h"

#define PROMPT ">>> "

#define YY_INPUT(buf, result, max_size) result = mygetinput(buf, max_size);

enum modes {START_LINE, AFTER_EQ, AFTER_DOT};

enum modes mode = START_LINE;

static int mygetinput(char *buf, unsigned int size) {
    char *line;

    if (feof(yyin))
      return YY_NULL;

    line = readline(PROMPT);

    if(!line)
      return YY_NULL;

    if(strlen(line) + 2 > size){
       fprintf(stderr, "Input line too long\n");
       return YY_NULL;
    }

    sprintf(buf,"%s\n",line);

    int prev_match = history_search (line, -1);

    if (prev_match != -1) {
      free_history_entry(remove_history(prev_match));
      while (next_history() != NULL) { }
    }

    add_history(line);

    free(line);

    return strlen(buf);
}

void yyerror(char *);
%}

%option noinput nounput

%option noyywrap

%x IN_STRING

COMMAND_NAME [[:alnum:]][_[:alnum:]]*

%%

help {
  return HELP;
}

exit {
  return EXIT;
}

test {
  return TEST;
}

classes {
  return CLASSES;
}

jep {
  return JEP;
}

timeout {
  return SHOW_TIMEOUT;
}

timeout" "*=" "*[[:digit:]]+ {
  char *s = yytext;
  while (!isdigit(*s))
    s++;
  shell_lval.integer = atoi(s);
  return TIMEOUT;
}

limit {
  return SHOW_LIMIT;
}

limit" "*=" "*[[:digit:]]+ {
  char *s = yytext;
  while (!isdigit(*s))
    s++;
  shell_lval.integer = atoi(s);
  return LIMIT;
}

history {
  return SHOW_HISTORY;
}

history" "*=" "*[[:digit:]]+ {
  char *s = yytext;
  while (!isdigit(*s))
    s++;
  shell_lval.integer = atoi(s);
  return HISTORY;
}

{COMMAND_NAME} {
  shell_lval.name = strdup(yytext);
  return COMMAND;
}

\n return ';';

[[:space:]] ;

\"  BEGIN IN_STRING;

. return yytext[0];

<IN_STRING>;|\n {
  BEGIN INITIAL;
  return ';';
}

<IN_STRING>([^\"\n]|"\\\"")+ {
  shell_lval.name = strdup(yytext);
  return FILENAME;
}

<IN_STRING>\"  {
  BEGIN INITIAL; }

%%

/* Local Variables: */
/* apheleia-mode: nil */
/* End: */
