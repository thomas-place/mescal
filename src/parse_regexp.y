%{
#include "parse_regexp.h"
%}

%parse-param {regexp** thereg}

%code{
void regexp_error(regexp**, char *s){
  if (s)
    fprintf(stderr, "Syntax error in restricted regexp.\n");
}
}

%union {
  regexp *regexp;
  char *name;
  unsigned char small_int;
  char carac;
}

%type <regexp> e
%token <name>VARIABLE <carac>LETTER <regexp>EPSILON_TOK <regexp>EMPTY_TOK <carac>PARAM <small_int>INDEX
%token OPEN_TOK CLOSE_TOK BRK_OPEN BRK_CLOSE
%left PLUS_TOK
%left INTER_TOK
%left CONCAT_TOK
%right STAR_TOK PLUSPLUS_TOK
%nonassoc COMPL_TOK

%%

s : error { *thereg = NULL;}
  | e[exp] { *thereg = $exp; }
  ;

e[res] :  e[exp1] PLUS_TOK e[exp2] { $res = reg_union($exp1, $exp2); }

| e[exp1] e[exp2] %prec CONCAT_TOK { $res = reg_concat($exp1, $exp2); }

| e[exp] STAR_TOK { $res = reg_star($exp); }

| e[exp] PLUSPLUS_TOK { $res = reg_plus($exp); }

| COMPL_TOK e[exp] { $res = reg_complement($exp); }
| e[exp1] INTER_TOK e[exp2] { $res = reg_inter($exp1, $exp2); }


| OPEN_TOK e[exp] CLOSE_TOK { $res = $exp; }

| LETTER[letter] { $res = reg_letter($letter); }

| LETTER[letter] BRK_OPEN INDEX[index] BRK_CLOSE { $res = reg_letter_numbered($letter, $index); }
| LETTER[letter] BRK_OPEN 'i' BRK_CLOSE { $res = reg_letter_symbolic($letter,  0); }
| LETTER[letter] BRK_OPEN 'i' '-' INDEX[index] BRK_CLOSE { $res = reg_letter_symbolic($letter,  $index); }


| EPSILON_TOK { $res = reg_epsilon(); }

| EMPTY_TOK { $res = reg_empty(); }

| VARIABLE[var] {
    DEBUG("RVALUE %s", $var);
    int i = object_get_from_name($var);
    if (i == -1 || objects[i]->type != REGEXP) {
      fprintf(stderr,"Error: %s is not a valid regular expression variable.\n",$var);
      $res = NULL;
    }
    else {
    DEBUG("Language found");
    $res = reg_copy(objects[i]->exp);
  }
}
| VARIABLE[var] BRK_OPEN 'i' BRK_CLOSE { $res = reg_var_symbolic($var,  0); }
| VARIABLE[var] BRK_OPEN 'i' '-' INDEX[index] BRK_CLOSE { $res = reg_var_symbolic($var,  $index); }

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
