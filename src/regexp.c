#include "regexp.h"

short symbolic_count = 0;
char** symbolic_names = NULL;

short symbolic_index(char* varname) {
    for (short i = 0; i < symbolic_count;i++) {
        if (strcmp(varname, symbolic_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}


uint display_syletter_utf8(syletter l, FILE* out) {
    switch (l.lab) {
    case ' ':
        fprintf(out, "␠");
        break;
    case '\n':
        fprintf(out, "⏎");
        break;
    default:
        fprintf(out, "%c", l.lab);
        break;
    }
    if (l.dec == 0) {
        print_color("ᵢ", BLUE, out);
    }
    else {
        print_color("ᵢ₋", BLUE, out);
        fprint_subsc_utf8(l.dec, out);
    }
    return 1;
}

uint display_syvar_utf8(syvariable v, FILE* out) {
    if (v.ind >= symbolic_count) {
        fprintf(out, "?");
    }
    else {
        fprintf(out, "%s", symbolic_names[v.ind]);
    }
    if (v.dec == 0) {
        print_color("ᵢ", BLUE, out);
    }
    else {
        print_color("ᵢ₋", BLUE, out);
        fprint_subsc_utf8(v.dec, out);
    }
    return 1;
}

bool reg_has_symbolic(regexp* exp) {
    if (!exp) {
        return false;
    }

    switch (exp->op) {
    case SYCHAR:
    case SYVAR:
        return true;
        break;
    case EPSILON:
    case EMPTY:
    case WORD:
    case CHAR:
        return false;
        break;
    case CONCAT:
    case UNION:
        return reg_has_symbolic(exp->left) || reg_has_symbolic(exp->right);
        break;
    case STAR:
    case PLUS:
        return reg_has_symbolic(exp->left);
    default:
        return false;
        break;
    }
}

bool reg_issimple(regexp* exp) {
    if (!exp) {
        return false;
    }

    switch (exp->op) {
    case EPSILON:
    case EMPTY:
    case WORD:
    case CHAR:
        return true;
        break;
    case CONCAT:
    case UNION:
        return reg_issimple(exp->left) && reg_issimple(exp->right);
        break;
    case STAR:
    case PLUS:
        return reg_issimple(exp->left);
    default:
        return false;
        break;
    }
}

void reg_free(regexp* r) {
    TRACE("reg_free");

    if (r == NULL) {
        return;
    }
    if (r->op == WORD) {
        delete_word(r->word);
    }

    reg_free(r->left);
    reg_free(r->right);
    free(r);
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
    if (r->op == CHAR) {
        copy->letter = r->letter;
    }
    if (r->op == WORD) {
        copy->word = create_empty_word();
        concatenate_word(copy->word, r->word);
    }
    if (r->op == SYCHAR) {
        copy->sylet = r->sylet;
    }
    if (r->op == SYVAR) {
        copy->syvar = r->syvar;
    }
    return copy;
}

static regexp* reg_binary(regelem op, regexp* left, regexp* right) {
    TRACE("reg_binary");

    if (left == NULL || right == NULL) {
        WARNING("NULL regexp");
        reg_free(left);
        reg_free(right);
        return NULL;
    }

    regexp* nexp;
    MALLOC(nexp, 1);
    nexp->op = op;
    nexp->left = left;
    nexp->right = right;
    return nexp;
}

static regexp* reg_unary(regelem op, regexp* left) {
    TRACE("reg_unary");

    if (left == NULL) {
        WARNING("NULL regexp");
        return NULL;
    }
    regexp* nexp;
    MALLOC(nexp, 1);
    nexp->op = op;
    nexp->left = left;
    nexp->right = NULL;
    return nexp;
}

static regexp* reg_nullary(regelem op) {
    TRACE("reg_nullary");
    regexp* r;
    MALLOC(r, 1);
    r->op = op;
    r->left = NULL;
    r->right = NULL;
    return r;
}

regexp* reg_empty(void) {
    TRACE("reg_empty");
    return reg_nullary(EMPTY);
}

regexp* reg_epsilon(void) {
    TRACE("reg_epsilon");
    return reg_nullary(EPSILON);
}

regexp* reg_letter(uchar c) {
    TRACE("reg_letter(%c)", c);

    regexp* nr = reg_nullary(CHAR);
    nr->letter.lab = c;
    nr->letter.num = -1;
    return nr;
}

regexp* reg_letter_ext(letter l) {
    regexp* nr = reg_nullary(CHAR);
    nr->letter = l;
    return nr;
}

regexp* reg_letter_numbered(uchar c, uchar index) {
    regexp* nr = reg_nullary(CHAR);
    nr->letter.lab = c;
    nr->letter.num = index; // Assumes that info is an integer for now.
    return nr;
}

regexp* reg_letter_symbolic(uchar c, uchar number) {
    if (symbolic_count <= 0) {
        fprintf(stderr, "Error: Symbolic letters can only be used to setup recursive definitions.\n");
        return NULL;
    }
    for (uchar i = 0; i < symbolic_count;i++) {
        if (strlen(symbolic_names[i]) == 1 && symbolic_names[i][0] == c) {
            regexp* nr = reg_nullary(SYVAR);
            nr->syvar.ind = i;
            nr->syvar.dec = number; // Assumes that info is an integer for now.
            return nr;
        }
    }


    regexp* nr = reg_nullary(SYCHAR);
    nr->sylet.lab = c;
    nr->sylet.dec = number; // Assumes that info is an integer for now.
    return nr;
}

regexp* reg_var_symbolic(char* s, uchar number) {
    if (symbolic_count <= 0) {
        fprintf(stderr, "Error: Symbolic variables can only be used to setup recursive definitions.\n");
        return NULL;
    }

    short j = symbolic_index(s);
    if (j == -1) {
        fprintf(stderr, "Error: The symbolic variable \"%s\" is unknown.\n", s);
        return NULL;
    }
    else {
        regexp* nr = reg_nullary(SYVAR);
        nr->syvar.ind = j;
        nr->syvar.dec = number; // Assumes that info is an integer for now.
        return nr;
    }
}

regexp* reg_union(regexp* left, regexp* right) {
    TRACE("reg_union");

    if ((left == NULL) || (right == NULL)) {
        reg_free(left);
        reg_free(right);
        return NULL;
    }

    if (left->op == EMPTY) {
        reg_free(left);
        return right;
    }

    if (right->op == EMPTY) {
        reg_free(right);
        return left;
    }

    return reg_binary(UNION, left, right);
}

regexp* reg_inter(regexp* left, regexp* right) {
    TRACE("reg_inter");

    if ((left == NULL) || (right == NULL)) {
        reg_free(left);
        reg_free(right);
        return NULL;
    }

    return reg_binary(INTER, left, right);
}

regexp* reg_concat(regexp* left, regexp* right) {
    TRACE("reg_concat");
    ;
    if ((left == NULL) || (right == NULL)) {
        reg_free(left);
        reg_free(right);
        return NULL;
    }

    if (left->op == EPSILON) {
        reg_free(left);
        return right;
    }

    if (right->op == EPSILON) {
        reg_free(right);
        return left;
    }

    if (left->op == CHAR) {
        if (right->op == CHAR) {
            word* word = create_empty_word();
            rigcon_word(left->letter, word);
            rigcon_word(right->letter, word);
            left->op = WORD;
            left->word = word;
            reg_free(right);
            return left;
        }
        if (right->op == WORD) {
            lefcon_word(left->letter, right->word);
            reg_free(left);
            return right;
        }
        if (right->op == CONCAT && (right->left->op == WORD || right->left->op == CHAR)) {
            right->left = reg_concat(left, right->left);
            return right;
        }
    }

    if (left->op == WORD) {
        if (right->op == CHAR) {
            rigcon_word(right->letter, left->word);
            reg_free(right);
            return left;
        }
        if (right->op == WORD) {
            concatenate_word(left->word, right->word);
            reg_free(right);
            return left;
        }
        if (right->op == CONCAT && (right->left->op == WORD || right->left->op == CHAR)) {
            right->left = reg_concat(left, right->left);
            return right;
        }
    }

    if (left->op == CONCAT && (left->right->op == WORD || left->right->op == CHAR)) {
        if (right->op == CHAR || right->op == WORD) {
            left->right = reg_concat(left->right, right->left);
            right->left = left;
            return right;
        }
    }

    return reg_binary(CONCAT, left, right);
}

regexp* reg_star(regexp* left) {
    TRACE("reg_star");

    if (left == NULL) {
        return NULL;
    }

    if (left->op == STAR || left->op == PLUS) {
        left->op = STAR;
        return left;
    }

    return reg_unary(STAR, left);
}

regexp* reg_plus(regexp* left) {
    TRACE("reg_plus");

    if (left == NULL) {
        return NULL;
    }

    if (left->op == STAR || left->op == PLUS) {
        return left;
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

static void reg_print_helper(regexp* r, regelem parent) {
    TRACE("reg_print_helper");

    if (r == NULL) {
        return;
    }

    switch (r->op) {
    case CHAR:
        fprint_letter_utf8(r->letter, stdout);
        break;
    case SYCHAR:
        display_syletter_utf8(r->sylet, stdout);
        break;
    case SYVAR:
        display_syvar_utf8(r->syvar, stdout);
        break;
    case WORD:
        display_word(r->word, stdout);
        break;

    case EMPTY:
        print_color("∅", BLUE, stdout);
        break;

    case EPSILON:
        print_color("ε", YELLOW, stdout);
        break;

    case UNION:
        DEBUG("Union");

        if (parent != UNION && parent != NONE) {
            print_color("(", CYAN, stdout);
        }
        reg_print_helper(r->left, UNION);
        print_color(" + ", RED, stdout);
        reg_print_helper(r->right, UNION);
        if (parent != UNION && parent != NONE) {
            print_color(")", CYAN, stdout);
        }
        break;

    case INTER:
        if (parent != INTER && parent != NONE) {
            print_color("(", CYAN, stdout);
        }
        reg_print_helper(r->left, INTER);
        printf(" ∩ ");
        reg_print_helper(r->right, INTER);
        if (parent != INTER && parent != NONE) {
            print_color(")", CYAN, stdout);
        }
        break;

    case CONCAT:
        DEBUG("Concat");

        if (parent != CONCAT && parent != UNION && parent != NONE) {
            print_color("(", CYAN, stdout);
        }
        reg_print_helper(r->left, CONCAT);
        reg_print_helper(r->right, CONCAT);
        if (parent != CONCAT && parent != UNION && parent != NONE) {
            print_color(")", CYAN, stdout);
        }
        break;

    case STAR:
        DEBUG("Star");
        if (parent == STAR) {
            reg_print_helper(r->left, STAR);
        }
        else if (r->left->op == WORD && size_word(r->left->word) > 1) {
            print_color("(", CYAN, stdout);
            reg_print_helper(r->left, STAR);
            print_color(")*", CYAN, stdout);
        }
        else {
            reg_print_helper(r->left, STAR);
            print_color("*", CYAN, stdout);
        }
        break;

    case PLUS:
        DEBUG("Plus");
        if (parent == PLUS) {
            reg_print_helper(r->left, PLUS);
        }
        else if (r->left->op == WORD && size_word(r->left->word) > 1) {
            print_color("(", CYAN, stdout);
            reg_print_helper(r->left, PLUS);
            print_color(")⁺", CYAN, stdout);
        }
        else {
            reg_print_helper(r->left, PLUS);
            print_color("⁺", CYAN, stdout);
        }
        break;
    case COMPLEMENT:
        if (parent != NONE) {
            print_color("(", CYAN, stdout);
        }
        print_color("¬", RED, stdout);
        reg_print_helper(r->left, COMPLEMENT);
        if (parent != NONE) {
            print_color(")", CYAN, stdout);
        }
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




bool reg_symbolic_loops(regexp* exp, ushort max, uchar num, bool* cycle) {
    if (!exp) {
        return true;
    }
    switch (exp->op) {
    case EPSILON:
    case EMPTY:
    case WORD:
    case CHAR:
    case SYCHAR:
        return true;
        break;
    case CONCAT:
    case UNION:
        return reg_symbolic_loops(exp->left, max, num, cycle) && reg_symbolic_loops(exp->right, max, num, cycle);
        break;
    case STAR:
    case PLUS:
        return reg_symbolic_loops(exp->left, max, num, cycle);
    case SYVAR:
        if (exp->syvar.dec > max) {
            return false;
        }
        if (exp->syvar.dec == 0) {
            cycle[exp->syvar.ind] = true;
        }
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;
}
/*


   regexp* reg_simplify(regexp* exp) {
    if (!exp) {
        return NULL;
    }

    switch (exp->op) {
    case EPSILON:
    case EMPTY:
    case CHAR:
        return exp;
        break;
    case INTER:
    case UNION:
        exp->left = reg_simplify(exp->left);
        exp->right = reg_simplify(exp->right);
        return exp;
        break;
    case COMPLEMENT:
    case STAR:
        exp->left = reg_simplify(exp->left);
        return exp;
        break;
    case CONCAT:
        exp->left = reg_simplify(exp->left);
        exp->right = reg_simplify(exp->right);
        if (exp->left->op == EPSILON) {
            regexp* ret = exp->right;
            free(exp->left);
            free(exp);
            return ret;
        }

        if (exp->right->op == EPSILON) {
            regexp* ret = exp->left;
            free(exp->right);
            free(exp);
            return ret;
        }
        return exp;
        break;
    default:
        return NULL;
        break;
    }
   } */
