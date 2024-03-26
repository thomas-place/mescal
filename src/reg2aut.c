#include <stdlib.h>
#include "reg2aut.h"
#include "shell_words.h"
#include "macros_error.h"

nfa *reg2nfa(regexp *r)
{
  if (r == NULL)
    return NULL;

  switch (r->op)
  {
  case EMPTY:
    return create_emptylang(max(1, r->size_alph));
    break;

  case WORD:
    return create_sing(max(1, r->size_alph), make_word(r->word), strlen(r->word));
    break;

  case UNION:
    return nfa_union(reg2nfa(r->left), reg2nfa(r->right));
    break;

  case INTER:
    return nfa_intersect(reg2nfa(r->left), reg2nfa(r->right), true);
    break;

  case CONCAT:
    return nfa_concat(reg2nfa(r->left), reg2nfa(r->right));
    break;

  case COMPLEMENT:
    return nfa_complement(reg2nfa(r->left));
    break;

  case STAR:
    return nfa_star(reg2nfa(r->left));
    break;

  case PLUS:
    return nfa_star_plus(reg2nfa(r->left));

  default:
    return NULL;
  }
}

nfa *reg2dfa(regexp *r)
{
  return nfa_minimize(reg2nfa(r));
}
