#include "regexp.h"
#include "macros_alloc.h"
#include "macros_error.h"
#include "shell_words.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

regexp* reg_binary(enum RegElem op, regexp* left, regexp* right) {
  TRACE("reg_binary");

  regexp* r;
  if (left == NULL || right == NULL) {
    return NULL;
  }
  MALLOC(r, 1);
  r->op = op;
  r->left = left;
  r->right = right;
  r->size_alph = max(left->size_alph, right->size_alph);
  r->word = NULL;
  return r;
}

regexp* reg_unary(enum RegElem op, regexp* left) {
  TRACE("reg_unary");

  regexp* r;
  if (left == NULL) {
    return NULL;
  }
  MALLOC(r, 1);
  r->op = op;
  r->left = left;
  r->right = NULL;
  r->size_alph = left->size_alph;
  r->word = NULL;
  return r;
}

regexp* reg_nullary(enum RegElem op) {
  TRACE("reg_nullary");

  regexp* r;
  MALLOC(r, 1);
  r->op = op;
  r->left = NULL;
  r->right = NULL;
  r->size_alph = 0;
  r->word = NULL;
  return r;
}

regexp* reg_copy(regexp* r) {
  TRACE("reg_copy");

  if (r == NULL) {
    return NULL;
  }

  regexp* left = reg_copy(r->left);
  regexp* right = reg_copy(r->right);

  regexp* copy;
  MALLOC(copy, 1);

  copy->op = r->op;
  copy->left = left;
  copy->right = right;
  copy->size_alph = r->size_alph;
  if (r->word == NULL || r->op != WORD) {
    copy->word = NULL;
  }
  else {
    copy->word = strdup(r->word);
  }

  return copy;
}

void reg_free(regexp* r) {
  TRACE("reg_free");

  if (r == NULL) {
    return;
  }

  reg_free(r->left);
  reg_free(r->right);
  free(r->word);
  free(r);
}

regexp* reg_word(char* w) {
  TRACE("reg_word(%s)", w);

  regexp* r = reg_nullary(WORD);
  r->word = strdup(w);
  r->size_alph = alphabet_size(w);
  return r;
}

regexp* reg_alphabet(char c) {
  TRACE("reg_alphabet");

  assert(islower(c));
  regexp* r = reg_word("a");
  for (int i = 1; i < c - 'a' + 1; i++) {
    char* w = calloc(2, 1);
    w[0] = 'a' + i;
    regexp* tmp = reg_word(w);
    r = reg_union(r, tmp);
  }
  return r;
}

regexp* reg_epsilon(void) {
  TRACE("reg_epsilon");

  return reg_word("");
}

regexp* reg_empty(void) {
  TRACE("reg_empty");

  return reg_nullary(EMPTY);
}

regexp* reg_word_star(char* w) {
  TRACE("reg_word_star(%s)", w);

  if ( !
       (strlen(w) > 1 && (w[strlen(w) - 1] == '*' || w[strlen(w) - 1] == '#'))
       ) {
    FATAL("Word %s is not a valid word", w);
  }

  int len = strlen(w);

  for (int i = len - 1; i > 0 && (w[i] == '#' || w[i] == '*'); i--) {
    w[i] = 0;
  }

  len = strlen(w);

  if (len == 1) {
    return reg_star(reg_word(w));
  }

  uint size = alphabet_size(w);

  char* last = calloc(2, 1);
  last[0] = w[len - 1];
  w[len - 1] = 0;

  regexp* r = reg_concat(reg_word(w), reg_star(reg_word(last)));
  r->size_alph = size;
  return r;
}

regexp* reg_word_plus(char* w) {
  TRACE("reg_word_plus(%s)", w);

  if (!(strlen(w) > 1 && w[strlen(w) - 1] == '#')) {
    FATAL("Word %s is not a valid word", w);
  }

  int len = strlen(w);

  for (int i = len - 1; i > 0 && w[i] == '#'; i--) {
    w[i] = 0;
  }

  len = strlen(w);

  if (len == 1) {
    return reg_plus(reg_word(w));
  }

  uint size = alphabet_size(w);

  char* last = calloc(2, 1);
  last[0] = w[len - 1];
  w[len - 1] = 0;

  regexp* r = reg_concat(reg_word(w), reg_plus(reg_word(last)));
  r->size_alph = size;
  return r;
}

regexp* reg_union(regexp* left, regexp* right) {
  TRACE("reg_union");

  if ((left == NULL) || (right == NULL)) {
    return NULL;
  }

  return reg_binary(UNION, left, right);
}

regexp* reg_inter(regexp* left, regexp* right) {
  TRACE("reg_inter");

  if ((left == NULL) || (right == NULL)) {
    return NULL;
  }

  return reg_binary(INTER, left, right);
}

regexp* reg_left_quotient(regexp* left, regexp* right) {
  TRACE("reg_left_quotient");

 if ((left == NULL) || (right == NULL)) {
    return NULL;
  }

  return reg_binary(LEFT_QUOTIENT, left, right);
}

regexp* reg_right_quotient(regexp* left, regexp* right) {
  TRACE("reg_right_quotient");

  if ((left == NULL) || (right == NULL)) {
    return NULL;
  }

  return reg_binary(RIGHT_QUOTIENT, left, right);
}

regexp* reg_difference(regexp* left, regexp* right) {
    TRACE("reg_difference");

  if ((left == NULL) || (right == NULL)) {
    return NULL;
  }

  return reg_binary(DIFFERENCE, left, right);
}

regexp* reg_concat(regexp* left, regexp* right) {
  TRACE("reg_concat");;

  if ((left == NULL) || (right == NULL)) {
    return NULL;
  }

  return reg_binary(CONCAT, left, right);
}

regexp* reg_star(regexp* left) {
  TRACE("reg_star");

  if (left == NULL) {
    return NULL;
  }

  return reg_unary(STAR, left);
}

regexp* reg_plus(regexp* left) {
  TRACE("reg_plus");

  if (left == NULL) {
    return NULL;
  }

  return reg_unary(PLUS, left);
}

regexp* reg_complement(regexp* left) {
  TRACE("reg_complement");

  if (left == NULL) {
    return NULL;
  }

  return reg_unary(COMPLEMENT, left);
}

regexp* reg_simplify(regexp* r) {
  if (r == NULL) {
    TRACE("reg_simplify(NULL)");
    return NULL;
  }
  TRACE("reg_simplify(%d)", r->op);
  regexp* left = reg_simplify(r->left);
  regexp* right = reg_simplify(r->right);

  switch (r->op) {
  case WORD:
    return r;

  case CONCAT:
    if (left->op == WORD && strcmp(left->word, "") == 0) {
      reg_free(left);
      regexp* temp = r->right;
      r->op = temp->op;
      r->word = temp->word;
      r->left = temp->left;
      r->right = temp->right;
      r->size_alph = temp->size_alph;
      free(temp);
      return r;
    }

    if (right->op == WORD && strcmp(right->word, "") == 0) {
      reg_free(right);
      regexp* temp = r->left;
      r->op = temp->op;
      r->word = temp->word;
      r->left = temp->left;
      r->right = temp->right;
      r->size_alph = temp->size_alph;
      free(temp);
      return r;
    }

    if (left->op == WORD && right->op == WORD) {
      char* w = calloc(strlen(left->word) + strlen(right->word) + 1, 1);
      memcpy(w, left->word, strlen(left->word));
      memcpy(w + strlen(left->word), right->word, strlen(right->word));

      reg_free(left);
      reg_free(right);

      r->op = WORD;
      r->word = w;
      r->size_alph = alphabet_size(w);
      r->left = NULL;
      r->right = NULL;
      return r;
    }

    if (left->op == WORD && right->op == STAR && right->left->op == WORD &&
      strcmp(left->word, right->left->word) == 0) {
      free(right);
      r->op = PLUS;
      r->right = NULL;
      r->size_alph = left->size_alph;
    }

    if (right->op == WORD && left->op == STAR && left->left->op == WORD &&
      strcmp(right->word, left->left->word) == 0) {
      free(r->left);
      r->op = PLUS;
      r->left = r->right;
      r->right = NULL;
      r->size_alph = left->size_alph;
    }

    if (left->op == EMPTY || right->op == EMPTY) {
      reg_free(left);
      reg_free(right);
      r->op = EMPTY;
      r->word = NULL;
      r->left = NULL;
      r->right = NULL;
      r->size_alph = 0;
      return r;
    }
    return r;

  case STAR:
    DEBUG("STAR");
    if (left->op == WORD && strcmp(left->word, "") == 0) {
      DEBUG("STAR: WORD");
      reg_free(r);
      return reg_epsilon();
    }
    if (left->op == STAR || left->op == PLUS) {
      DEBUG("STAR: STAR");
      regexp *temp = r->left;
      r->left = r->left->left;
      free(temp);
      return r;
    }
    return r;

  case PLUS:
    DEBUG("PLUS");
    if (left->op == PLUS) {
      DEBUG("PLUS: PLUS");
      regexp *temp = r->left;
      r->left = r->left->left;
      free(temp);
      return r;
    }
    if (left->op == STAR) {
      DEBUG("PLUS: STAR");
      regexp *temp = r->left;
      r->op = STAR;
      r->left = r->left->left;
      free(temp);
      return r;
    }
    return r;

  default:
    return r;
  }
}

static void reg_print_helper(regexp* r, enum RegElem parent) {
  TRACE("reg_print_helper");

  if (r == NULL) {
    return;
  }

  switch (r->op) {
  case WORD:
    print_facto_word(r->word, stdout);
    // if (strlen(r->word) == 0) {
    //   printf("ε");
    // } else {
    //   printf("%s", r->word);
    // }
    break;

  case EMPTY:
    printf("∅");
    break;

  case UNION:
    if (parent != UNION && parent != NONE) {
      printf("(");
    }
    reg_print_helper(r->left, UNION);
    printf(" + ");
    reg_print_helper(r->right, UNION);
    if (parent != UNION && parent != NONE) {
      printf(")");
    }
    break;

  case INTER:
    if (parent != INTER && parent != NONE) {
      printf("(");
    }
    reg_print_helper(r->left, INTER);
    printf(" ∩ ");
    reg_print_helper(r->right, INTER);
    if (parent != INTER && parent != NONE) {
      printf(")");
    }
    break;

  case DIFFERENCE:
    printf("(");
    reg_print_helper(r->left, DIFFERENCE);
    printf(" - ");
    reg_print_helper(r->right, DIFFERENCE);
    printf(")");
    break;

  case CONCAT:
    if (parent != CONCAT && parent != UNION && parent != NONE) {
      printf("(");
    }
    reg_print_helper(r->left, CONCAT);
    // printf(".");
    reg_print_helper(r->right, CONCAT);
    if (parent != CONCAT && parent != UNION && parent != NONE) {
      printf(")");
    }
    break;

  case STAR:
    if (parent == STAR) {
      reg_print_helper(r->left, STAR);
    }
    else if (r->left->op == WORD && strlen(r->left->word) > 1) {
      printf("(");
      reg_print_helper(r->left, STAR);
      printf(")*");
    }
    else {
      reg_print_helper(r->left, STAR);
      printf("*");
    }
    break;

  case PLUS:
    if (r->left->op == WORD && strlen(r->left->word) > 1) {
      printf("(");
      reg_print_helper(r->left, PLUS);
      printf(")⁺");
    }
    else {
      reg_print_helper(r->left, PLUS);
      printf("⁺");
    }
    break;

  case COMPLEMENT:
    printf("¬(");
    reg_print_helper(r->left, COMPLEMENT);
    printf(")");
    break;

  default:
    CRITICAL("Unknown regexp operator (%d)", r->op);
    break;
  }
}

void reg_print(regexp* r) {
  TRACE("reg_print");

  reg_print_helper(r, NONE);
  printf("\n");
}

void reg_print_full(regexp* r) {
  TRACE("reg_print_full");

  if (r == NULL) {
    return;
  }

  switch (r->op) {
  case WORD:
    printf("WORD(%s)", r->word);
    break;
  case EMPTY:
    printf("EMPTY");
    break;
  case UNION:
    printf("UNION(");
    reg_print_full(r->left);
    printf(", ");
    reg_print_full(r->right);
    printf(")");
    break;
  case INTER:
    printf("INTER(");
    reg_print_full(r->left);
    printf(", ");
    reg_print_full(r->right);
    printf(")");
    break;
  case DIFFERENCE:
    printf("DIFFERENCE(");
    reg_print_full(r->left);
    printf(", ");
    reg_print_full(r->right);
    printf(")");
    break;
  case CONCAT:
    printf("CONCAT(");
    reg_print_full(r->left);
    printf(", ");
    reg_print_full(r->right);
    printf(")");
    break;
  case STAR:
    printf("STAR(");
    reg_print_full(r->left);
    printf(")");
    break;
  case PLUS:
    printf("PLUS(");
    reg_print_full(r->left);
    printf(")");
    break;
  case COMPLEMENT:
    printf("COMPLEMENT(");
    reg_print_full(r->left);
    printf(")");
    break;
  case LEFT_QUOTIENT:
    printf("LEFT_QUOTIENT(");
    reg_print_full(r->left);
    printf(", ");
    reg_print_full(r->right);
    printf(")");
    break;
  case RIGHT_QUOTIENT:
    printf("RIGHT_QUOTIENT(");
    reg_print_full(r->left);
    printf(", ");
    reg_print_full(r->right);
    printf(")");
    break;
  default:
    CRITICAL("Unknown regexp operator (%d)", r->op);
    break;
  }
}

void reg_to_string(regexp* r, char* buffer) {
  TRACE("reg_to_string");

  if (r == NULL) {
    return;
  }

  switch (r->op) {
  case WORD:
    if (strlen(r->word) == 0) {
      strcat(buffer, "1");
    }
    else {
      strcat(buffer, r->word);
    }

    break;
  case EMPTY:
    strcat(buffer, "0");
    break;
  case UNION:
    strcat(buffer, "(");
    reg_to_string(r->left, buffer);
    strcat(buffer, " + ");
    reg_to_string(r->right, buffer);
    strcat(buffer, ")");
    break;
  case INTER:
    strcat(buffer, "(");
    reg_to_string(r->left, buffer);
    strcat(buffer, " & ");
    reg_to_string(r->right, buffer);
    strcat(buffer, ")");
    break;
  case CONCAT:
    strcat(buffer, "(");
    reg_to_string(r->left, buffer);
    strcat(buffer, " ");
    reg_to_string(r->right, buffer);
    strcat(buffer, ")");
    break;
  case STAR:
    strcat(buffer, "(");
    reg_to_string(r->left, buffer);
    strcat(buffer, ")*");
    break;
  case PLUS:
    strcat(buffer, "(");
    reg_to_string(r->left, buffer);
    strcat(buffer, ")#");
    break;
  case COMPLEMENT:
    strcat(buffer, "!(");
    reg_to_string(r->left, buffer);
    strcat(buffer, ")");
    break;
  default:
    CRITICAL("Unknown regexp operator (%d)", r->op);
    break;
  }
}
