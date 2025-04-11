%{
#include "interrupt.h"
#include "parser.h"
%}

%code{
void yyerror(char *s){
  if (s) fprintf(stderr, "Syntax error in command.\n");
}

}

%union {
  regexp *regexp;
  string_chain* string_chain;
  com_parameters* com_parameters;
  com_command* com_command;
  char *name;
  char carac;
  int integer;
}

%type <com_parameters> cp
%type <com_command> cc
%token <name>COMMAND <name>FILENAME EXP
%token JEP <integer>TIMEOUT SHOW_TIMEOUT <integer>LIMIT SHOW_LIMIT <integer>HISTORY SHOW_HISTORY
%token HELP <carac>ALPHABET
%token CLASSES TEST EXIT
%token LEXICAL_ERROR
%right '='

%%

s : s ';' {}

| s TEST { test(); }

| s HELP { help(); }

| s CLASSES { print_classes(); }

| s EXIT { return 0; }

| s JEP { shell_jep(); }

| s SHOW_TIMEOUT { printf("Current timeout value: %d.\n", timeout_value); }

| s TIMEOUT[num] {
  int new_timeout = $num;
  if (new_timeout > 0){
    timeout_value = new_timeout;
    printf("Timeout set to %d.\n", new_timeout);
  }
  else if (new_timeout == 0){
    timeout_value = new_timeout;
    printf("Timeout cancelled (use the 'timeout' command with a positive number to set a new timeout).\n");
  }
  else {
    timeout_value = TIMEOUT_DEFAULT;
    printf("Timeout reset to default value %d.\n", TIMEOUT_DEFAULT);
  }
}

| s SHOW_LIMIT { printf("Current limit value: %d.\n", limit_value); }

| s LIMIT[num] {
  int new_limit = $num;
  if (new_limit > 0){
    limit_value = new_limit;
    printf("Limit set to %d.\n", new_limit);
  }
  else if (new_limit == 0){
    limit_value = new_limit;
    printf("Limit cancelled (use the 'limit' command with a positive number to set a new limit).\n");
  }
  else {
    limit_value = LIMIT_DEFAULT;
    printf("Limit reset to default value %d.\n", LIMIT_DEFAULT);
  }
}

| s SHOW_HISTORY { printf("Current history value: %d.\n", history_size); }

| s HISTORY[num] {

  int new_history = $num;
  if (new_history > 0){
    history_size = new_history;
    printf("History size set to %d.\n", new_history);
  }
  else {
    history_size = DEFAULT_HISTORY_SIZE;
    printf("History reset to default value %d.\n", DEFAULT_HISTORY_SIZE);
  }
}



| s COMMAND[var] '.' COMMAND[sub] '[' COMMAND[par] ']' '=' cc[com] ';' {
  com_setrec_command($var, $sub, $par, $com);
  free($var);
  free($sub);
  free($par);
  com_free_command($com);
}

| s COMMAND[var] '=' cc[com] ';' {
  com_apply_command($com,$var,MODE_COPY,NULL);
  free($var);
  com_free_command($com);
}

| s cc[com] ';' {
  com_apply_command($com,NULL,MODE_PRINT,NULL);
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

cc[res] : COMMAND[str] '.' cc[com] { $res  = com_make_command($str, $com); }

        | COMMAND[str] '(' cp[par] { $res = com_init_command($str,$par);}

        | COMMAND[str] '[' COMMAND[par] ']' { $res = com_init_indexing($str,$par);}

        | COMMAND[str] { $res = com_init_command($str,NULL);}

        | FILENAME[str] { $res = com_init_rawcommand($str);}
;



%%

/* Local Variables:    */
/* apheleia-mode: nil  */
/* apheleia-inhibit: t */
/* End:                */
