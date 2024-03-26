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

#include "parse_regexp.tab.h"

void lex_error(char *s);

void lex_error(char *s){
  fprintf(stderr,"Lexical error, token %s ignored\n", s);
}

%}

%option noinput nounput

%option noyywrap

LANGNAME [[:upper:]][[:upper:][:digit:]]*

LETTER_SEQ [[:lower:]][[:lower:]]*

STARS [#*]*[*][#*]*

%%


\n return ';';

[[:space:]] ;

[,!+&.*#/()\\=-] {
  return regexp_text[0];
}

1|EPS {
  return EPSILON_TOK;
}

0 {
  return EMPTY_TOK;
}

"["[[:lower:]]"]" {
  regexp_lval.carac = regexp_text[1];
  return ALPHABET;
}

{LETTER_SEQ}{STARS} {
  regexp_lval.name = strdup(regexp_text);
  return WORD_STAR;
}

{LETTER_SEQ}[#]+ {
  regexp_lval.name = strdup(regexp_text);
  return WORD_PLUS;
}

{LETTER_SEQ} {
  regexp_lval.name = strdup(regexp_text);
  return WORD_TOK;
}

{LANGNAME} {
  regexp_lval.name = strdup(regexp_text);
  return VARIABLE;
}

. {
  lex_error(yytext);
}

%%

void set_input_string(const char* in);

void end_lexical_scan(void);

void set_input_string(const char * in){
  regexp__scan_string(in);
}

void end_lexical_scan(void){
  regexp_lex_destroy();
}
