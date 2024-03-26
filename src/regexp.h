#ifndef REGEXP_H_
#define REGEXP_H_

#include "main.h"
#include "type_basic.h"
#include "tools.h"

enum RegElem
{
  NONE,
  EMPTY,
  WORD,
  UNION,
  INTER,
  DIFFERENCE,
  COMPLEMENT,
  CONCAT,
  STAR,
  PLUS,
  LEFT_QUOTIENT,
  RIGHT_QUOTIENT
};

// Should be a union
typedef struct regexp
{
  char* word;
  enum RegElem op;

  struct regexp* left, * right;
  uint size_alph;
} regexp;

regexp* reg_binary(enum RegElem op, regexp* left, regexp* right);

regexp* reg_copy(regexp* r);

void reg_free(regexp* r);

regexp* reg_unary(enum RegElem op, regexp* left);

regexp* reg_nullary(enum RegElem op);

regexp* reg_word(char* w);

regexp* reg_alphabet(char c);

regexp* reg_epsilon(void);

regexp* reg_empty(void);

regexp* reg_word_star(char* w);

regexp* reg_word_plus(char* w);

regexp* reg_union(regexp* left, regexp* right);

regexp* reg_inter(regexp* left, regexp* right);

regexp* reg_left_quotient(regexp* left, regexp* right);

regexp* reg_right_quotient(regexp* left, regexp* right);

regexp* reg_difference(regexp* left, regexp* right);

regexp* reg_concat(regexp* left, regexp* right);

regexp* reg_star(regexp* left);

regexp* reg_plus(regexp* left);

regexp* reg_simplify(regexp* regexp);

regexp* reg_complement(regexp* left);

void reg_print(regexp* r);

void reg_print_full(regexp* r);

void reg_to_string(regexp* r, char* buffer);

#endif // REGEXP_H_
