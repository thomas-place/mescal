%{
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "macros_error.h"
#include "macros_alloc.h"
#include "regexp.h"
#include "shell_commands.h"

#include <readline/readline.h>
#include <readline/history.h>

#include "parser.tab.h"

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
       fprintf(stderr,"input line too long\n");
       return YY_NULL;
    }
    sprintf(buf,"%s\n",line);
    add_history(line);
    free(line);
    return strlen(buf);
}

void yyerror(char *);
%}

%option noinput nounput

%option noyywrap

%x IN_FILE

COMMAND_NAME [[:alnum:]][_[:alnum:]]*

%%

help {
  return HELP;
}

exit {
  return EXIT;
}

languages {
  return LANGUAGES;
}

automata {
  return AUTOS;
}

morphisms {
  return MORPHISMS;
}

test {
  return TEST;
}

{COMMAND_NAME} {
  shell_lval.name = strdup(yytext);
  return COMMAND;
}

\n return ';';

[[:space:]] ;

\"  BEGIN IN_FILE;

. return yytext[0];

<IN_FILE>;|\n {
  BEGIN INITIAL;
  return ';';
}

<IN_FILE>[^\"\n]+ {
  shell_lval.name = strdup(yytext);
  return FILENAME;
}

<IN_FILE>\"  { BEGIN INITIAL; }

%%

