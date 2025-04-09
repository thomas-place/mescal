%{
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "error.h"
#include "alloc.h"
#include "regexp.h"
#include "shell_commands.h"

#include <readline/readline.h>
#include <readline/history.h>

#include "parse_regexp.tab.h"

void lex_error(char *s);

void lex_error(char *s){
  fprintf(stderr,"Lexical error in regexp, token '%s' ignored\n", s);
}

%}

%option noinput nounput

%option noyywrap

LANGNAME \{[[:upper:]][_[:alnum:]]*\}

%x BRK

%%

\n return ';';

[[:space:]] ;

\\["] {
  regexp_lval.carac = '\"';
  return LETTER;
}

"\\ " {
  regexp_lval.carac = ' ';
  return LETTER;
}

"++" |
"#" {
  return PLUSPLUS_TOK;
}

"\[" {
  BEGIN BRK;
  return BRK_OPEN;
}

<BRK>"\]" {
  BEGIN INITIAL;
  return BRK_CLOSE;
}

"+" {
  return PLUS_TOK;
}


"!"  {
  return COMPL_TOK;
}

"&"  {
  return INTER_TOK;
}

"*" {
  return STAR_TOK;
}

"(" {
  return OPEN_TOK;
}

")" {
  return CLOSE_TOK;
}

"\\1" {
  return EPSILON_TOK;
}

"\\0" {
  return EMPTY_TOK;
}

{LANGNAME} {
  regexp_text[regexp_leng-1] = 0;
  regexp_lval.name = strdup(regexp_text + 1);
  return VARIABLE;
}

"\\"[#+!&*(){}\]\[] {
  regexp_lval.carac = regexp_text[1];
  return LETTER;
}

. {
  regexp_lval.carac = regexp_text[0];
  return LETTER;
}

<BRK>[0-9]* {
  regexp_lval.small_int = atoi(regexp_text);
  return INDEX;
}

<BRK>[a-z] {
  return regexp_text[0];
  //regexp_lval.carac = regexp_text[0];
  //return PARAM;
}

<BRK>[-+] {
  return regexp_text[0];
}

<BRK>[[:space:]] ;

<BRK>.|\n {
  return yytext[0];
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
