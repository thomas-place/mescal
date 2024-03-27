%{
#include "parser.h"
%}

%code{
void yyerror(char *s){
  if (s) fprintf(stderr, "Syntax error in command\n");
}

}

%union {
  regexp *regexp;
  string_chain* string_chain;
  com_parameters* com_parameters;
  com_command* com_command;
  char *name;
  char carac;
}

%type <com_parameters> cp
%type <com_command> cc
%token <name>COMMAND <name>FILENAME EXP
%token LANGUAGES AUTOS MORPHISMS
%token HELP <carac>ALPHABET
%token TEST EXIT
%token LEXICAL_ERROR
%right '='

%%

s : s ';' {}

| s LANGUAGES { object_print_langs(); }

| s AUTOS { object_print_autos(); }

| s MORPHISMS { object_print_morps(); }

| s TEST { test(); }

| s HELP { help(); }

| s EXIT { return 0; }

| s COMMAND[var] '=' cc[com] ';' {
  com_apply_link_command($var, $com);
  com_free_command($com);
  free($var);
}

| s cc[com] ';' {
  com_apply_command($com);
  com_free_command($com);
}

| s LEXICAL_ERROR ';' {
  yyerrok;
}

| s error ';' {
  yyerrok;
}

| %empty {}
;

cp[res] : cc[par] ')' { $res = com_make_parameters($par, NULL);}

| cc[par] ',' cp[rec] { $res = com_make_parameters($par, $rec);}
;

cc[res] : COMMAND[str] '.' cc[com] { $res  = com_make_command($str,$com);}

| COMMAND[str] '(' cp[par] { $res = com_init_command($str,$par);}

| COMMAND[str] { $res = com_init_command($str,NULL);}

| FILENAME[str] { $res = com_init_rawcommand($str);}
;

%%

/* Local Variables:    */
/* apheleia-mode: nil  */
/* apheleia-inhibit: t */
/* End:                */
