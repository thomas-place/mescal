
#include "regexp_tonfa.h"

/****************************/
/*+ Algorithme de Glushkov +*/
/****************************/

uint reg_countletters(regexp *exp)
{
    if (!exp)
    {
        return 0;
    }
    if (exp->op == CHAR)
    {
        return 1;
    }
    if (exp->op == WORD)
    {
        return size_word(exp->word);
    }
    return reg_countletters(exp->left) + reg_countletters(exp->right);
}

glushkov_info *reg_create_gk_emp(uint size)
{
    glushkov_info *new;
    MALLOC(new, 1);
    new->size = size;
    new->epsilon = false;
    if (size)
    {
        new->first = create_barray(size);
        new->last = create_barray(size);
        new->follow = create_barray(size * size);
    }
    else
    {
        new->first = NULL;
        new->last = NULL;
        new->follow = NULL;
    }
    return new;
}

static glushkov_info *reg_create_gk_eps(uint size)
{
    glushkov_info *new = reg_create_gk_emp(size);
    new->epsilon = true;
    return new;
}

static glushkov_info *reg_create_gk_let(letter a, uint *i, letter *map, uint size)
{
    glushkov_info *info = reg_create_gk_emp(size);
    info->epsilon = false;
    settrue_barray(info->first, *i);
    settrue_barray(info->last, *i);
    map[*i] = a;
    *i += 1;
    return info;
}

static glushkov_info *reg_create_gk_word(word *w, uint *i, letter *map, uint size)
{
    glushkov_info *info = reg_create_gk_emp(size);
    info->epsilon = false;
    settrue_barray(info->first, *i);
    settrue_barray(info->last, *i + size_word(w) - 1);
    for (uint j = 0; j < size_word(w) - 1; j++)
    {

        settrue_barray(info->follow, (*i + 1) * size + *i);
        map[*i] = lefread_word(w, j);
        *i += 1;
    }
    settrue_barray(info->last, *i);
    map[*i] = rigread_word(w, 0);
    *i += 1;
    return info;
}

static glushkov_info *reg_gk_star(glushkov_info *info)
{
    info->epsilon = true;
    for (uint i = 0; i < info->size; i++)
    {
        for (uint j = 0; j < info->size; j++)
        {
            if (getval_barray(info->last, i) && getval_barray(info->first, j))
            {
                settrue_barray(info->follow, j * info->size + i);
            }
        }
    }
    return info;
}

static glushkov_info *reg_gk_plus(glushkov_info *info)
{
    for (uint i = 0; i < info->size; i++)
    {
        for (uint j = 0; j < info->size; j++)
        {
            if (getval_barray(info->last, i) && getval_barray(info->first, j))
            {
                settrue_barray(info->follow, j * info->size + i);
            }
        }
    }
    return info;
}

static glushkov_info *reg_gk_union(glushkov_info *infol, glushkov_info *infor)
{
    infol->epsilon |= infor->epsilon;
    or_barray_mod(infol->first, infor->first);
    or_barray_mod(infol->last, infor->last);
    or_barray_mod(infol->follow, infor->follow);
    reg_gk_delete(infor);
    return infol;
}

static glushkov_info *reg_gk_concat(glushkov_info *infol, glushkov_info *infor)
{

    or_barray_mod(infol->follow, infor->follow);
    for (uint i = 0; i < infol->size; i++)
    {
        for (uint j = 0; j < infor->size; j++)
        {
            if (getval_barray(infol->last, i) && getval_barray(infor->first, j))
            {
                settrue_barray(infol->follow, j * infol->size + i);
            }
        }
    }

    if (infol->epsilon)
    {
        or_barray_mod(infol->first, infor->first);
    }

    if (infor->epsilon)
    {
        or_barray_mod(infol->last, infor->last);
    }
    else
    {
        for (uint i = 0; i < infol->last->size_array; i++)
        {
            infol->last->array[i] = infor->last->array[i];
        }
    }

    infol->epsilon &= infor->epsilon;

    reg_gk_delete(infor);
    return infol;
}

void reg_gk_delete(glushkov_info *info)
{
    if (info)
    {
        delete_barray(info->first);
        delete_barray(info->last);
        delete_barray(info->follow);
        free(info);
    }
}

glushkov_info *gk_indexleaves(regexp *exp, uint size, uint *i, letter *map)
{
    if (!exp)
    {
        return NULL;
    }
    switch (exp->op)
    {
    case EMPTY:
        return reg_create_gk_emp(size);
        break;
    case EPSILON:
        return reg_create_gk_eps(size);
        break;
    case CHAR:
        return reg_create_gk_let(exp->letter, i, map, size);
        break;
    case WORD:
        return reg_create_gk_word(exp->word, i, map, size);
        break;
    case STAR:
        return reg_gk_star(gk_indexleaves(exp->left, size, i, map));
        break;
    case PLUS:
        return reg_gk_plus(gk_indexleaves(exp->left, size, i, map));
        break;
    case UNION:
        return reg_gk_union(gk_indexleaves(exp->left, size, i, map), gk_indexleaves(exp->right, size, i, map));
        break;
    case CONCAT:
        return reg_gk_concat(gk_indexleaves(exp->left, size, i, map), gk_indexleaves(exp->right, size, i, map));
        break;
    default:
        break;
    }
    return NULL;
}

nfa *reg_glushkov(regexp *exp)
{
    if (!reg_issimple(exp))
    {
        fprintf(stderr, "Error: extended regular expressions are not compatible with Glushkov's algorithm.\n");
        return NULL;
    }

    uint nleaves = reg_countletters(exp);
    letter *tab;
    MALLOC(tab, nleaves);
    uint num = 0;
    glushkov_info *infos = gk_indexleaves(exp, nleaves, &num, tab);

    // Calcul de l'alphabet

    letter *alphabet;
    MALLOC(alphabet, nleaves);
    for (uint i = 0; i < nleaves; i++)
    {
        alphabet[i] = tab[i];
    }
    qsort(alphabet, nleaves, sizeof(letter), compare_letters);

    uint a_size = 0;
    uint j = 1;
    while (j < nleaves)
    {
        while (j < nleaves && compare_letters(&alphabet[a_size], &alphabet[j]) == 0)
        {
            j++;
        }
        if (j < nleaves)
        {
            a_size++;
            alphabet[a_size] = alphabet[j];
            j++;
        }
    }
    a_size++;
    REALLOC(alphabet, a_size);

    nfa *A;
    CALLOC(A, 1);
    A->nb_letters = a_size;
    A->epsilon = false;
    A->inverse = false;
    A->alphabet = alphabet;
    A->state_names = NULL;

    // Initial state
    A->nb_initials = 1;
    MALLOC(A->initials, 1);
    A->initials[0] = 0;

    // Final states
    A->nb_finals = 0;
    if (infos->epsilon)
    {
        A->nb_finals++;
    }
    for (uint i = 0; i < nleaves; i++)
    {
        if (getval_barray(infos->last, i))
        {
            A->nb_finals++;
        }
    }
    MALLOC(A->finals, A->nb_finals);
    uint idx = 0;
    if (infos->epsilon)
    {
        A->finals[idx] = 0;
        idx++;
    }
    for (uint i = 0; i < nleaves; i++)
    {
        if (getval_barray(infos->last, i))
        {
            A->finals[idx] = i + 1;
            idx++;
        }
    }

    // Transitions
    dyn_edge_triple *transitions = create_dyn_edge_triple();

    for (uint i = 0; i < nleaves; i++)
    {
        if (getval_barray(infos->first, i))
        {
            letter *p = bsearch(&tab[i], A->alphabet, a_size, sizeof(letter), compare_letters);
            uint a = p - A->alphabet;
            dyn_edge_triple_add(transitions, 0, a, i + 1);
        }
    }

    for (uint k = 0; k < nleaves * nleaves; k++)
    {
        if (getval_barray(infos->follow, k))
        {
            uint q = k % nleaves + 1;
            uint r = k / nleaves + 1;
            letter *p = bsearch(&tab[r - 1], A->alphabet, a_size, sizeof(letter), compare_letters);
            uint a = p - A->alphabet;
            dyn_edge_triple_add(transitions, q, a, r);
        }
    }

    A->trans = edge_list_to_lgraph(transitions->array, transitions->size, nleaves + 1, a_size);

    delete_dyn_edge_triple(transitions);
    reg_gk_delete(infos);
    free(tab);

    return A;
}

/****************************/
/*+ Algorithme de Thompson +*/
/****************************/

nfa *reg_thompson(regexp *expr)
{
    if (!expr)
    {
        return NULL;
    }

    nfa *aleft = NULL;
    nfa *aright = NULL;
    if (expr->left)
    {
        aleft = reg_thompson(expr->left);
    }

    if (expr->right)
    {
        aright = reg_thompson(expr->right);
    }
    nfa *ret;

    switch (expr->op)
    {
    case EMPTY:
        ret = create_emptylang();
        break;

    case EPSILON:
        ret = create_sing_epsilon();
        break;

    case CHAR:
        ret = create_sing_letter(expr->letter);
        break;
    case WORD:
        ret = create_sing_word(expr->word);
        break;

    case UNION:
        ret = nfa_union(aleft, false, aright, false);
        break;

    case INTER:
        ret = nfa_intersect(aleft, aright, false);
        break;

    case CONCAT:
        ret = nfa_concat(aleft, false, aright, false);
        break;

    case COMPLEMENT:
        ret = nfa_complement(aleft);
        break;

    case STAR:
        ret = nfa_star(aleft);
        break;

    case PLUS:
        ret = nfa_plus(aleft);
        break;

    default:
        ret = NULL;
        break;
    }
    nfa_delete(aleft);
    nfa_delete(aright);
    return ret;
}
