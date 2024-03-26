%{
#include "parse_regexp.h"
%}

%parse-param {regexp** thereg}

%code{
void regexp_error(regexp**, char *s){
  if (s)
    fprintf(stderr, "Syntax error in regexp.\n");
}
}

%union {
  regexp *regexp;
  char *name;
  char carac;
}

%type <regexp> e
%token EXP <name>WORD_TOK <regexp>EPSILON_TOK <regexp>EMPTY_TOK <name>WORD_STAR <name>WORD_PLUS <name>VARIABLE
%token <carac>ALPHABET
%token LEXICAL_ERROR
%left '+' '&' '|' '\\' '/' '-'
%left '.' CONCATENATION
%right '*' '#'
%nonassoc '!' NEG

%%

s : error { *thereg = NULL;}
  | e[exp] { *thereg = $exp; }
  ;

e[res] :  e[exp1] '+' e[exp2] { $res = reg_union($exp1, $exp2); }

| e[exp1] e[exp2] %prec CONCATENATION { $res = reg_concat($exp1, $exp2); }

| e[exp] '*' { $res = reg_star($exp); }

| e[exp] '#' { $res = reg_plus($exp); }

| e[exp1] '-' e[exp2] { $res = reg_difference($exp1, $exp2); }

| e[exp1] '&' e[exp2] { $res = reg_inter($exp1, $exp2); }

| e[exp1] '\\' e[exp2] { $res = reg_right_quotient($exp1, $exp2); }

| e[exp1] '/' e[exp2] { $res = reg_left_quotient($exp1, $exp2); }

| '(' e[exp] ')' { $res = $exp; }

| '!' e[exp] %prec NEG { $res = reg_complement($exp); }

| WORD_TOK[word] { $res = reg_word($word); }

| WORD_STAR[ws] { $res = reg_word_star($ws); }

| WORD_PLUS[ws] { $res = reg_word_plus($ws); }

| EPSILON_TOK { $res = reg_epsilon(); }

| ALPHABET[car] { $res = reg_alphabet($car); }

| EMPTY_TOK { $res = reg_empty(); }

| VARIABLE[var] {
    DEBUG("RVALUE %s", $var);
    int i = object_get_from_name($var);
    if (i == -1 || objects[i]->type != LANGUAGE) {
      fprintf(stderr,"Error: %s is not a valid language variable.\n",$var);
      $res = NULL;
    }
    else if (objects[i]->theob.lan->type != SPE_REG) {
      fprintf(stderr,"Error: %s is specified by an automaton.\n",$var);
      $res = NULL;
    }
    else {
    DEBUG("Language found");
    $res = reg_copy(objects[i]->theob.lan->spe.reg);
  }
}
;

%%

/* Declarations */

void set_input_string(const char* in);
void end_lexical_scan(void);

/* This function parses a string */
regexp* parse_string_regexp(const char* in){
  set_input_string(in);
  regexp* res;
  regexp_parse(&res);
  end_lexical_scan();
  return res;
}
