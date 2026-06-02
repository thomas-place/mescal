/***************************/
/* Implémentation des NFAs */
/***************************/

#include "nfa.h"
#include "error.h"
#include "printing.h"

/***************/
/* State names */
/***************/

void nfa_print_state(const nfa *A, uint q, FILE *out)
{
    if (!A || !out)
    {
        return;
    }
    if (A->state_names)
    {
        fprintf(out, "%s", A->state_names[q]);
    }
    else
    {
        fprintf(out, "%d", q);
    }
}

void dfa_print_state(const dfa *A, uint q, FILE *out)
{
    if (!A || !out || q >= A->trans->size_graph)
    {
        return;
    }
    if (A->state_names)
    {
        fprintf(out, "%s", A->state_names[q]);
    }
    else
    {
        fprintf(out, "%d", q);
    }
}

void nfa_reset_state_names(nfa *A)
{
    if (!A || !A->state_names)
    {
        return;
    }

    for (uint i = 0; i < A->trans->size_graph; i++)
    {
        free(A->state_names[i]);
    }
    free(A->state_names);
    A->state_names = NULL;
}

void dfa_reset_state_names(dfa *A)
{
    if (!A || !A->state_names)
    {
        return;
    }

    for (uint i = 0; i < A->trans->size_graph; i++)
    {
        free(A->state_names[i]);
    }
    free(A->state_names);
    A->state_names = NULL;
}

char **copy_all_names(char **names, uint size)
{
    if (!names)
    {
        return NULL;
    }
    char **new_names;
    MALLOC(new_names, size);
    for (uint q = 0; q < size; q++)
    {
        new_names[q] = strdup(names[q]);
    }
    return new_names;
}

char **names_from_partition(char **state_names, parti *P)
{
    if (!P)
    {
        return NULL;
    }

    char **ret;
    MALLOC(ret, P->size_par);

    for (uint q = 0; q < P->size_par; q++)
    {
        uint stringsize = 0;

        for (uint i = 0; i < P->cl_size[q]; i++)
        {
            uint r = P->cl_elems[q][i];
            if (state_names)
            {
                stringsize += strlen(state_names[r]) + 1;
            }
            else
            {
                stringsize += get_uint_length(r) + 1;
            }
        }
        MALLOC(ret[q], stringsize);
        bool first = true;
        char aux[64];
        for (uint i = 0; i < P->cl_size[q]; i++)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                strcat(ret[q], ",");
            }

            if (state_names)
            {
                strcat(ret[q], state_names[P->cl_elems[q][i]]);
            }
            else
            {
                sprintf(aux, "%d", P->cl_elems[q][i]);
                strcat(ret[q], aux);
            }
        }
    }

    return ret;
}

/**********************/
/*+ Copy and release +*/
/**********************/

void nfa_delete(nfa *A)
{
    if (A == NULL)
    {
        return;
    }

    // Reseting the names first (the graph is needed)
    nfa_reset_state_names(A);

    delete_lgraph(A->trans);
    free(A->initials);
    free(A->finals);
    free(A->alphabet);

    free(A);
}

void dfa_delete(dfa *A)
{
    if (!A)
    {
        return;
    }
    if (A->order)
    {
        free(A->order);
        free(A->order_storage);
    }
    if (A->sccs)
    {
        delete_parti(A->sccs);
    }
    if (A->sccs_inv)
    {
        free(A->sccs_inv);
    }
    if (A->tclos)
    {
        delete_graph(A->tclos);
    }

    free(A->alphabet);
    free(A->finals);
    dfa_reset_state_names(A);
    delete_dgraph(A->trans);
    free(A);
}

nfa *nfa_copy(nfa *A)
{
    if (!A)
    {
        return NULL;
    }
    nfa *B;
    CALLOC(B, 1);

    // Transitions
    B->trans = copy_lgraph(A->trans);
    B->epsilon = A->epsilon;
    B->inverse = A->inverse;
    B->nb_letters = A->nb_letters;

    B->nb_finals = A->nb_finals;
    // Final states
    if (A->nb_finals > 0)
    {
        MALLOC(B->finals, A->nb_finals);
        MEMCPY(B->finals, A->finals, A->nb_finals);
    }

    B->nb_initials = A->nb_initials;
    // Initial states
    if (A->nb_initials > 0)
    {
        MALLOC(B->initials, A->nb_initials);
        MEMCPY(B->initials, A->initials, A->nb_initials);
    }

    // Noms des lettres
    B->alphabet = duplicate_alphabet(A->alphabet, A->nb_letters);

    // Noms des états
    B->state_names = copy_all_names(A->state_names, A->trans->size_graph);

    return B;
}

dfa *dfa_copy(dfa *A)
{
    if (!A)
    {
        return NULL;
    }
    // dfa* B = dfa_init(A->trans->size_graph, A->trans->size_alpha, A->nb_finals, A->alphabet);
    dfa *B;
    CALLOC(B, 1);
    B->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    B->trans = copy_dgraph(A->trans);
    B->initial = A->initial;
    B->nb_finals = A->nb_finals;
    if (B->nb_finals == 0)
    {
        B->finals = NULL;
    }
    else
    {
        MALLOC(B->finals, A->nb_finals);
        for (uint i = 0; i < A->nb_finals; i++)
        {
            B->finals[i] = A->finals[i];
        }
    }

    B->state_names = copy_all_names(A->state_names, A->trans->size_graph);
    return B;
}

void union_alphabet(letter *alpha1, uint size1, uint *indmap1, letter *alpha2, uint size2, uint *indmap2, uint *newsize, letter **newalpha)
{
    uint i = 0, j = 0, c = 0;
    while (i < size1 && j < size2)
    {
        if (compare_letters(&alpha1[i], &alpha2[j]) < 0)
        {
            indmap1[i++] = c;
        }
        else if (compare_letters(&alpha1[i], &alpha2[j]) > 0)
        {
            indmap2[j++] = c;
        }
        else
        {
            indmap1[i++] = c;
            indmap2[j++] = c;
        }
        c++;
    }

    while (i < size1)
    {
        indmap1[i++] = c++;
    }

    while (j < size2)
    {
        indmap2[j++] = c++;
    }

    *newsize = c;

    MALLOC(*newalpha, *newsize);

    for (uint k = 0; k < size1; k++)
    {
        (*newalpha)[indmap1[k]] = alpha1[k];
    }

    for (uint k = 0; k < size2; k++)
    {
        (*newalpha)[indmap2[k]] = alpha2[k];
    }
}

void inter_alphabet(letter *alpha1, uint size1, letter *alpha2, uint size2, uint *newsize, letter **newalpha, uint **revmap1, uint **revmap2)
{
    uint i = 0, j = 0, c = 0;

    uint indmap1[size1];
    uint indmap2[size2];

    while (i < size1 && j < size2)
    {
        if (compare_letters(&alpha1[i], &alpha2[j]) < 0)
        {
            indmap1[i++] = UINT_MAX;
        }
        else if (compare_letters(&alpha1[i], &alpha2[j]) > 0)
        {
            indmap2[j++] = UINT_MAX;
        }
        else
        {
            indmap1[i++] = c;
            indmap2[j++] = c;
        }
        c++;
    }

    while (i < size1)
    {
        indmap1[i++] = UINT_MAX;
    }

    while (j < size2)
    {
        indmap2[j++] = UINT_MAX;
    }

    *newsize = c;

    MALLOC(*newalpha, *newsize);
    MALLOC(*revmap1, size1);
    MALLOC(*revmap2, size2);

    for (uint k = 0; k < size1; k++)
    {
        if (indmap1[k] != UINT_MAX)
        {
            (*revmap1)[indmap1[k]] = k;
            (*newalpha)[indmap1[k]] = alpha1[k];
        }
    }

    for (uint k = 0; k < size2; k++)
    {
        if (indmap2[k] != UINT_MAX)
        {
            (*revmap2)[indmap2[k]] = k;
        }
    }
}

/**************************************************************/
/*+ Computation of basic NFAs (used in Thompson's algorithm) +*/
/**************************************************************/

nfa *create_emptylang(void)
{
    nfa *A;
    CALLOC(A, 1);
    A->nb_letters = 0;
    A->nb_initials = 0;
    A->nb_finals = 0;
    A->trans = create_lgraph_noedges(0, 0);
    return A;
}

nfa *create_sing_epsilon(void)
{
    nfa *A;
    CALLOC(A, 1);
    A->nb_letters = 0;
    A->nb_initials = 1;
    A->nb_finals = 1;
    CALLOC(A->initials, 1);
    CALLOC(A->finals, 1);
    A->initials[0] = 0;
    A->finals[0] = 0;
    A->trans = create_lgraph_noedges(1, 0);
    A->epsilon = false;
    A->inverse = false;
    return A;
}

nfa *create_sing_letter(letter the_letter)
{
    nfa *A;
    CALLOC(A, 1);
    A->nb_letters = 1;
    A->nb_initials = 1;
    A->nb_finals = 1;
    MALLOC(A->initials, 1);
    MALLOC(A->finals, 1);
    A->initials[0] = 0;
    A->finals[0] = 1;
    A->trans = create_lgraph_noedges(2, 1);
    A->trans->size_edges = 1;
    MALLOC(A->trans->storage, 1);
    A->trans->storage[0] = 1;
    MALLOC(A->trans->intervals, 3);
    A->trans->intervals[0] = 0;
    A->trans->intervals[1] = 1;
    A->trans->intervals[2] = 1;
    A->epsilon = false;
    A->inverse = false;
    MALLOC(A->alphabet, 1);
    A->alphabet[0] = the_letter;
    return A;
}

nfa *create_sing_word(word *the_word)
{
    nfa *A;
    CALLOC(A, 1);
    A->nb_initials = 1;
    A->nb_finals = 1;
    MALLOC(A->initials, 1);
    MALLOC(A->finals, 1);
    A->initials[0] = 0;
    A->finals[0] = size_word(the_word);
    A->alphabet = get_alphabet_word(the_word, &A->nb_letters);

    edge_triple *elist;
    MALLOC(elist, size_word(the_word));
    for (uint i = 0; i < size_word(the_word); i++)
    {
        letter l = lefread_word(the_word, i);
        letter *p = bsearch(&l, A->alphabet, A->nb_letters, sizeof(letter), compare_letters);
        elist[i].from = i;
        elist[i].lab = p - A->alphabet;
        elist[i].to = i + 1;
    }

    A->trans = edge_list_to_lgraph(elist, size_word(the_word), size_word(the_word) + 1, A->nb_letters);
    A->epsilon = false;
    A->inverse = false;
    free(elist);
    return A;
}

/*******************************/
/*+ Simple operations on NFAs +*/
/*******************************/

static void store_nfa_edges(nfa *A, edge_triple *elist, uint nb_letters, uint size_alpha, uint offset, uint *indmap)
{
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint start = LGSTART(A->trans, q, a);
            uint end = LGEND(A->trans, q, a);
            for (uint j = start; j < end; j++)
            {
                elist[j].from = q + offset;
                elist[j].lab = indmap ? indmap[a] : a;
                elist[j].to = A->trans->storage[j] + offset;
            }
        }
        if (A->inverse)
        {
            for (uint a = 0; a < A->nb_letters; a++)
            {
                uint start = LGSTART(A->trans, q, a + A->nb_letters);
                uint end = LGEND(A->trans, q, a + A->nb_letters);
                for (uint j = start; j < end; j++)
                {
                    elist[j].from = q + offset;
                    elist[j].lab = (indmap ? indmap[a] : a) + nb_letters;
                    elist[j].to = A->trans->storage[j] + offset;
                }
            }
        }
        if (A->epsilon)
        {
            uint start = LGSTART(A->trans, q, A->trans->size_alpha - 1);
            uint end = LGEND(A->trans, q, A->trans->size_alpha - 1);
            for (uint j = start; j < end; j++)
            {
                elist[j].from = q + offset;
                elist[j].lab = size_alpha - 1;
            }
        }
    }
}

nfa *nfa_union(void *I1, bool is_dfa_I1, void *I2, bool is_dfa_I2)
{
    if (!I1 || !I2)
    {
        return NULL;
    }
    nfa *A1 = is_dfa_I1 ? dfa_to_nfa((dfa *)I1) : (nfa *)I1;
    nfa *A2 = is_dfa_I2 ? dfa_to_nfa((dfa *)I2) : (nfa *)I2;

    nfa *UNION;
    CALLOC(UNION, 1);
    UNION->inverse = A1->inverse || A2->inverse;
    UNION->epsilon = A1->epsilon || A2->epsilon;

    // Transitions
    uint *indmap1;
    uint *indmap2;
    MALLOC(indmap1, A1->nb_letters);
    MALLOC(indmap2, A2->nb_letters);

    union_alphabet(A1->alphabet, A1->nb_letters, indmap1, A2->alphabet, A2->nb_letters, indmap2, &UNION->nb_letters, &UNION->alphabet);

    uint newalphasize = (UNION->nb_letters << (UNION->inverse ? 1 : 0)) + ((UNION->epsilon) ? 1 : 0);
    uint newedgesize = A1->trans->size_edges + A2->trans->size_edges;
    edge_triple *elist;
    MALLOC(elist, newedgesize);
    store_nfa_edges(A1, elist, UNION->nb_letters, newalphasize, 0, indmap1);
    store_nfa_edges(A2, elist + A1->trans->size_edges, UNION->nb_letters, newalphasize, A1->trans->size_graph, indmap2);
    UNION->trans = edge_list_to_lgraph(elist, newedgesize, A1->trans->size_graph + A2->trans->size_graph, newalphasize);
    free(elist);
    free(indmap1);
    free(indmap2);

    // Initial states
    UNION->nb_initials = A1->nb_initials + A2->nb_initials;
    MALLOC(UNION->initials, UNION->nb_initials);
    MEMCPY(UNION->initials, A1->initials, A1->nb_initials);
    for (uint i = 0; i < A2->nb_initials; i++)
    {
        UNION->initials[i + A1->nb_initials] = A2->initials[i] + A1->trans->size_graph;
    }

    // Final states
    UNION->nb_finals = A1->nb_finals + A2->nb_finals;
    MALLOC(UNION->finals, UNION->nb_finals);
    MEMCPY(UNION->finals, A1->finals, A1->nb_finals);
    for (uint i = 0; i < A2->nb_finals; i++)
    {
        UNION->finals[i + A1->nb_finals] = A2->finals[i] + A1->trans->size_graph;
    }

    // State names
    UNION->state_names = NULL;

    if (is_dfa_I1)
    {
        nfa_delete(A1);
    }
    if (is_dfa_I2)
    {
        nfa_delete(A2);
    }

    return UNION;
}

nfa *nfa_concat(void *I1, bool is_dfa_I1, void *I2, bool is_dfa_I2)
{
    if (!I1 || !I2)
    {
        return NULL;
    }
    nfa *A1 = is_dfa_I1 ? dfa_to_nfa((dfa *)I1) : (nfa *)I1;
    nfa *A2 = is_dfa_I2 ? dfa_to_nfa((dfa *)I2) : (nfa *)I2;

    nfa *CONCAT;
    CALLOC(CONCAT, 1);
    CONCAT->inverse = A1->inverse || A2->inverse;
    CONCAT->epsilon = true;

    // Transitions
    uint *indmap1;
    uint *indmap2;
    MALLOC(indmap1, A1->trans->size_alpha);
    MALLOC(indmap2, A2->trans->size_alpha);

    union_alphabet(A1->alphabet, A1->nb_letters, indmap1, A2->alphabet, A2->nb_letters, indmap2, &CONCAT->nb_letters, &CONCAT->alphabet);

    uint newalphasize = (CONCAT->nb_letters << (CONCAT->inverse ? 1 : 0)) + 1;
    uint nbeps = A1->nb_finals * A2->nb_initials;
    uint newedgesize = A1->trans->size_edges + A2->trans->size_edges + nbeps;
    edge_triple *elist;
    MALLOC(elist, newedgesize);
    store_nfa_edges(A1, elist, CONCAT->nb_letters, newalphasize, 0, indmap1);
    store_nfa_edges(A2, elist + A1->trans->size_edges, CONCAT->nb_letters, newalphasize, A1->trans->size_graph, indmap2);
    uint idx = A1->trans->size_edges + A2->trans->size_edges;
    for (uint i = 0; i < A1->nb_finals; i++)
    {
        for (uint j = 0; j < A2->nb_initials; j++)
        {
            elist[idx].from = A1->finals[i];
            elist[idx].lab = newalphasize - 1; // Epsilon transition
            elist[idx].to = A2->initials[j] + A1->trans->size_graph;
            idx++;
        }
    }
    CONCAT->trans = edge_list_to_lgraph(elist, newedgesize, A1->trans->size_graph + A2->trans->size_graph, newalphasize);
    free(elist);
    free(indmap1);
    free(indmap2);

    // Initial states
    CONCAT->nb_initials = A1->nb_initials;
    MALLOC(CONCAT->initials, CONCAT->nb_initials);
    MEMCPY(CONCAT->initials, A1->initials, A1->nb_initials);

    // Final states
    CONCAT->nb_finals = A2->nb_finals;
    MALLOC(CONCAT->finals, CONCAT->nb_finals);
    for (uint i = 0; i < A2->nb_finals; i++)
    {
        CONCAT->finals[i] = A2->finals[i] + A1->trans->size_graph;
    }

    // State names
    CONCAT->state_names = NULL;

    if (is_dfa_I1)
    {
        nfa_delete(A1);
    }
    if (is_dfa_I2)
    {
        nfa_delete(A2);
    }

    return CONCAT;
}

static nfa *nfa_star_plus(nfa *A, bool is_plus)
{

    // Création de l'automate
    nfa *STAR;
    CALLOC(STAR, 1);
    STAR->epsilon = true;
    STAR->inverse = A->inverse;
    STAR->nb_letters = A->nb_letters;
    STAR->nb_initials = 1;
    MALLOC(STAR->initials, 1);
    STAR->initials[0] = A->trans->size_graph;

    if (is_plus)
    {
        STAR->nb_finals = A->nb_finals;
        MALLOC(STAR->finals, STAR->nb_finals);
        MEMCPY(STAR->finals, A->finals, STAR->nb_finals);
    }
    else
    {
        STAR->nb_finals = 1;
        MALLOC(STAR->finals, 1);
        STAR->finals[0] = A->trans->size_graph;
    }

    // printf("Creating star automaton:\n");

    edge_triple *elist;
    MALLOC(elist, A->trans->size_edges + A->nb_initials + A->nb_finals); // No need to store edges here.
    store_nfa_edges(A, elist, STAR->nb_letters, (STAR->nb_letters << (STAR->inverse ? 1 : 0)) + 1, 0, NULL);

    uint epsind = (STAR->nb_letters << (STAR->inverse ? 1 : 0));

    // printf("Epsilon index: %d\n", epsind);

    uint idx = A->trans->size_edges;
    for (uint i = 0; i < A->nb_initials; i++)
    {
        elist[idx].from = A->trans->size_graph;
        elist[idx].lab = epsind; // Epsilon transition
        elist[idx].to = A->initials[i];
        idx++;
    }

    // printf("Added initial epsilon transitions.\n");

    for (uint i = 0; i < A->nb_finals; i++)
    {
        elist[idx].from = A->finals[i];
        elist[idx].lab = epsind; // Epsilon transition
        elist[idx].to = A->trans->size_graph;
        idx++;
    }

    STAR->trans = edge_list_to_lgraph(elist, idx, A->trans->size_graph + 1, epsind + 1);
    free(elist);
    STAR->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    STAR->state_names = NULL;
    return STAR;
}

nfa *nfa_star(nfa *A)
{
    return nfa_star_plus(A, false);
}

nfa *dfa_star(dfa *A)
{

    nfa *temp = dfa_to_nfa(A);
    nfa *STAR = nfa_star(temp);
    nfa_delete(temp);
    return STAR;
}

nfa *nfa_plus(nfa *A)
{
    return nfa_star_plus(A, true);
}

// Miroir
nfa *nfa_mirror(nfa *A)
{
    // Création du miroir
    nfa *themirror;
    CALLOC(themirror, 1);
    themirror->inverse = A->inverse;
    themirror->epsilon = A->epsilon;
    themirror->nb_letters = A->nb_letters;
    themirror->nb_initials = A->nb_finals;
    themirror->nb_finals = A->nb_initials;
    MALLOC(themirror->initials, themirror->nb_initials);
    MALLOC(themirror->finals, themirror->nb_finals);
    MEMCPY(themirror->initials, A->finals, themirror->nb_initials);
    MEMCPY(themirror->finals, A->initials, themirror->nb_finals);

    themirror->trans = lgraph_mirror(A->trans);

    themirror->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    themirror->state_names = NULL;

    return themirror;
}

nfa *dfa_mirror(dfa *A)
{
    // Creation of the mirror
    nfa *themirror;
    CALLOC(themirror, 1);
    themirror->inverse = false;
    themirror->epsilon = false;

    themirror->nb_letters = A->trans->size_alpha;
    themirror->nb_initials = A->nb_finals;
    themirror->nb_finals = 1;
    MALLOC(themirror->initials, themirror->nb_initials);
    MALLOC(themirror->finals, themirror->nb_finals);
    themirror->finals[0] = A->initial;
    MEMCPY(themirror->initials, A->finals, themirror->nb_initials);
    themirror->trans = dgraph_mirror(A->trans);

    themirror->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    themirror->state_names = NULL;

    return themirror;
}

// Élimination des états non-accessibles et non-co-accessibles
// Le NFA produit n'est pas nécessairement complet
nfa *nfa_trim(nfa *A, bool names)
{
    if (!A)
    {
        return NULL;
    }

    bool *reach;
    CALLOC(reach, A->trans->size_graph);

    // ACCESSIBLE STATES
    dequeue *thestack = create_dequeue();
    for (uint i = 0; i < A->nb_initials; i++)
    {
        rigins_dequeue(A->initials[i], thestack);
    }
    lgraph_search_update(DFS, A->trans, thestack, NULL, reach);

    // CO-ACCESSIBLE STATES
    lgraph *themirror = lgraph_mirror(A->trans);
    bool *coreach;
    CALLOC(coreach, A->trans->size_graph);

    for (uint i = 0; i < A->nb_finals; i++)
    {
        rigins_dequeue(A->finals[i], thestack);
    }
    lgraph_search_update(DFS, themirror, thestack, NULL, coreach);
    delete_dequeue(thestack);
    delete_lgraph(themirror);

    // Intersection
    for (uint i = 0; i < A->trans->size_graph; i++)
    {
        reach[i] &= coreach[i];
    }
    free(coreach);

    // New numbering
    uint *newnums;
    MALLOC(newnums, A->trans->size_graph);
    uint nb_keeped = 0;
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (reach[q])
        {
            newnums[q] = nb_keeped;
            nb_keeped++;
        }
        else
        {
            newnums[q] = UINT_MAX;
        }
    }

    // Computing the remaining edges
    uint nb_edges = 0;
    edge_triple *elist;
    MALLOC(elist, A->trans->size_edges);
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (!reach[q])
        {
            continue;
        }
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint start = LGSTART(A->trans, q, a);
            uint end = LGEND(A->trans, q, a);
            for (uint j = start; j < end; j++)
            {
                if (!reach[A->trans->storage[j]])
                {
                    continue;
                }
                elist[nb_edges].from = newnums[q];
                elist[nb_edges].lab = a;
                elist[nb_edges].to = newnums[A->trans->storage[j]];
                nb_edges++;
            }
        }
    }

    // Creating the trimmed NFA
    nfa *TRIM;
    CALLOC(TRIM, 1);
    TRIM->epsilon = A->epsilon;
    TRIM->inverse = A->inverse;
    TRIM->nb_letters = A->nb_letters;

    TRIM->trans = edge_list_to_lgraph(elist, nb_edges, nb_keeped, A->trans->size_alpha);
    free(elist);

    // Initial states
    MALLOC(TRIM->initials, A->nb_initials);
    for (uint i = 0; i < A->nb_initials; i++)
    {
        if (reach[A->initials[i]])
        {
            TRIM->initials[TRIM->nb_initials++] = newnums[A->initials[i]];
        }
    }
    if (TRIM->nb_initials < A->nb_initials)
    {
        REALLOC(TRIM->initials, TRIM->nb_initials);
    }

    // Final states
    MALLOC(TRIM->finals, A->nb_finals);
    for (uint i = 0; i < A->nb_finals; i++)
    {
        if (reach[A->finals[i]])
        {
            TRIM->finals[TRIM->nb_finals++] = newnums[A->finals[i]];
        }
    }
    if (TRIM->nb_finals < A->nb_finals)
    {
        REALLOC(TRIM->finals, TRIM->nb_finals);
    }

    // Alphabet
    TRIM->alphabet = duplicate_alphabet(A->alphabet, A->nb_letters);

    if (names)
    {
        // Gestion des noms
        MALLOC(TRIM->state_names, TRIM->trans->size_graph);
        if (A->state_names)
        {
            for (uint q = 0; q < A->trans->size_graph; q++)
            {
                if (reach[q])
                {
                    TRIM->state_names[newnums[q]] = strdup(A->state_names[q]);
                }
            }
        }
        else
        {
            for (uint q = 0; q < A->trans->size_graph; q++)
            {
                if (reach[q])
                {
                    TRIM->state_names[newnums[q]] = uint_to_string(q);
                }
            }
        }
    }
    else
    {
        TRIM->state_names = NULL;
    }

    free(reach);
    free(newnums);

    return TRIM;
}

dfa *dfa_trim(dfa *A, bool names, bool add_sink)
{
    if (!A)
    {
        return NULL;
    }

    // A Boolean that will indicate if the trimmed DFA is complete (for now, we check whether the input DFA is complete).
    bool complete = dfa_is_comp(A);

    // ACCESSIBLE STATES (if there is an initial state)
    bool *reach;
    CALLOC(reach, A->trans->size_graph);
    if (A->initial < A->trans->size_graph)
    {
        dequeue *thestack = create_dequeue();
        rigins_dequeue(A->initial, thestack);
        dgraph_search_update(DFS, A->trans, thestack, NULL, reach);
        delete_dequeue(thestack);
    }

    // CO-ACCESSIBLE STATES (if there are final states)
    bool *coreach;
    CALLOC(coreach, A->trans->size_graph);
    if (A->nb_finals > 0)
    {
        dequeue *thestack = create_dequeue();
        lgraph *themirror = dgraph_mirror(A->trans);
        for (uint i = 0; i < A->nb_finals; i++)
        {
            rigins_dequeue(A->finals[i], thestack);
        }
        lgraph_search_update(DFS, themirror, thestack, NULL, coreach);
        delete_dequeue(thestack);
        delete_lgraph(themirror);
    }

    // Intersection
    for (uint i = 0; i < A->trans->size_graph; i++)
    {
        // If a reachable state is not co-reachable, then the trimmed DFA will not be complete.
        if (reach[i] && !coreach[i])
        {
            complete = false;
        }
        reach[i] &= coreach[i];
    }
    free(coreach);

    uint *newnums;
    MALLOC(newnums, A->trans->size_graph);
    uint nb_keeped = 0;
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (reach[q])
        {
            newnums[q] = nb_keeped;
            nb_keeped++;
        }
    }

    dfa *TRIM;
    CALLOC(TRIM, 1);
    bool sink = add_sink && !complete;
    uint new_size = nb_keeped + (sink ? 1 : 0);
    TRIM->trans = create_dgraph_noedges(new_size, A->trans->size_alpha);

    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (!reach[q])
        {
            continue;
        }
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            if (A->trans->edges[q][a] != UINT_MAX && reach[A->trans->edges[q][a]])
            {
                TRIM->trans->edges[newnums[q]][a] = newnums[A->trans->edges[q][a]];
                TRIM->trans->size_edges++;
            }
            else if (sink)
            {
                TRIM->trans->edges[newnums[q]][a] = nb_keeped; // Sink state
                TRIM->trans->size_edges++;
            }
        }
    }

    if (sink)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            TRIM->trans->edges[nb_keeped][a] = nb_keeped; // Sink state
        }
        TRIM->trans->size_edges += A->trans->size_alpha;
    }

    // Initial state
    TRIM->initial = UINT_MAX;
    if (A->initial != UINT_MAX && reach[A->initial])
    {
        TRIM->initial = newnums[A->initial];
    }

    // Final states
    for (uint i = 0; i < A->nb_finals; i++)
    {
        if (reach[A->finals[i]])
        {
            TRIM->nb_finals++;
        }
    }

    if (TRIM->nb_finals > 0)
    {
        MALLOC(TRIM->finals, TRIM->nb_finals);
        uint idx = 0;
        for (uint i = 0; i < A->nb_finals; i++)
        {
            if (reach[A->finals[i]])
            {
                TRIM->finals[idx++] = newnums[A->finals[i]];
            }
        }
    }

    TRIM->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    if (names)
    { // Gestion des noms
        MALLOC(TRIM->state_names, TRIM->trans->size_graph);
        if (A->state_names)
        {
            for (uint q = 0; q < A->trans->size_graph; q++)
            {
                if (reach[q])
                {
                    TRIM->state_names[newnums[q]] = strdup(A->state_names[q]);
                }
            }
        }
        else
        {
            for (uint q = 0; q < A->trans->size_graph; q++)
            {
                if (reach[q])
                {
                    TRIM->state_names[newnums[q]] = uint_to_string(q);
                }
            }
        }
        if (sink)
        {
            TRIM->state_names[nb_keeped] = strdup("s");
        }
    }
    else
    {
        TRIM->state_names = NULL;
    }

    free(reach);
    free(newnums);

    return TRIM;
}

// Élimination des transitions epsilon
nfa *nfa_elimeps(nfa *A)
{
    if (!A->epsilon)
    {
        // Si il n'y a pas de transitions epsilon
        return nfa_copy(A);
    }

    nfa *ELIM;
    CALLOC(ELIM, 1);
    ELIM->epsilon = false;
    ELIM->inverse = A->inverse;
    ELIM->nb_letters = A->nb_letters;
    ELIM->alphabet = duplicate_alphabet(A->alphabet, A->nb_letters);
    ELIM->state_names = NULL;

    bool *final;
    CALLOC(final, A->trans->size_graph);
    bool *newfinal;
    CALLOC(newfinal, A->trans->size_graph);
    for (uint i = 0; i < A->nb_finals; i++)
    {
        final[A->finals[i]] = true;
    }

    dyn_edge_triple *dea = create_dyn_edge_triple();

    dequeue *thestack = create_dequeue();
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        bool *visited;
        CALLOC(visited, A->trans->size_graph);
        rigins_dequeue(q, thestack);
        while (!isempty_dequeue(thestack))
        {
            uint r = rigpull_dequeue(thestack);
            if (visited[r])
            {
                continue;
            }
            visited[r] = true;
            if (final[r])
            {
                newfinal[q] = true;
            }

            for (uint a = 0; a < A->trans->size_alpha - 1; a++)
            {
                uint start = A->trans->intervals[r * A->trans->size_alpha + a];
                uint end = A->trans->intervals[r * A->trans->size_alpha + a + 1];
                for (uint j = start; j < end; j++)
                {
                    dyn_edge_triple_add(dea, q, a, A->trans->storage[j]);
                }
            }
            uint start = A->trans->intervals[r * A->trans->size_alpha + A->trans->size_alpha - 1];
            uint end = A->trans->intervals[r * A->trans->size_alpha + A->trans->size_alpha];
            for (uint j = start; j < end; j++)
            {
                rigins_dequeue(A->trans->storage[j], thestack);
            }
        }
        free(visited);
    }

    ELIM->trans = edge_list_to_lgraph(dea->array, dea->size, A->trans->size_graph, A->trans->size_alpha - 1);

    ELIM->nb_initials = A->nb_initials;
    MALLOC(ELIM->initials, ELIM->nb_initials);
    for (uint i = 0; i < A->nb_initials; i++)
    {
        ELIM->initials[i] = A->initials[i];
    }

    // FInals
    for (uint i = 0; i < A->trans->size_graph; i++)
    {
        if (newfinal[i])
        {
            ELIM->nb_finals++;
        }
    }
    MALLOC(ELIM->finals, ELIM->nb_finals);
    uint idx = 0;
    for (uint i = 0; i < A->trans->size_graph; i++)
    {
        if (newfinal[i])
        {
            ELIM->finals[idx++] = i;
        }
    }
    free(final);
    free(newfinal);
    delete_dyn_edge_triple(dea);
    delete_dequeue(thestack);

    return ELIM;
}

dfa *dfa_direct_product(dfa *A, dfa *B)
{
    if (!A || !B || A->trans->size_alpha != B->trans->size_alpha)
    {
        return NULL;
    }

    dfa *PROD;
    CALLOC(PROD, 1);
    PROD->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    PROD->initial = 0;
    PROD->nb_finals = A->nb_finals * B->nb_finals;
    MALLOC(PROD->finals, PROD->nb_finals);
    PROD->trans = dgraph_direct_product(A->trans, B->trans);
    uint k = 0;
    for (uint i = 0; i < A->nb_finals; i++)
    {
        for (uint j = 0; j < B->nb_finals; j++)
        {
            PROD->finals[k++] = A->finals[i] * B->trans->size_graph + B->finals[j];
        }
    }
    PROD->order = NULL;

    MALLOC(PROD->state_names, PROD->trans->size_graph);
    for (uint i = 0; i < PROD->trans->size_graph; i++)
    {
        uint size = 4;
        if (A->state_names)
        {
            size += strlen(A->state_names[i / B->trans->size_graph]);
        }
        else
        {
            size += get_uint_length(i / B->trans->size_graph);
        }
        if (B->state_names)
        {
            size += strlen(B->state_names[i % B->trans->size_graph]);
        }
        else
        {
            size += get_uint_length(i % B->trans->size_graph);
        }
        MALLOC(PROD->state_names[i], size);
        PROD->state_names[i][0] = '\0';
        char aux[64];
        strcat(PROD->state_names[i], "(");
        if (A->state_names)
        {
            strcat(PROD->state_names[i], A->state_names[i / B->trans->size_graph]);
        }
        else
        {
            sprintf(aux, "%d", i / B->trans->size_graph);
            strcat(PROD->state_names[i], aux);
        }
        strcat(PROD->state_names[i], ",");
        if (B->state_names)
        {
            strcat(PROD->state_names[i], B->state_names[i % B->trans->size_graph]);
        }
        else
        {
            sprintf(aux, "%d", i % B->trans->size_graph);
            strcat(PROD->state_names[i], aux);
        }
        strcat(PROD->state_names[i], ")");
    }
    return PROD;
}

void dfa_compute_sccs(dfa *A)
{
    if (!A || A->sccs)
    {
        return;
    }
    A->sccs = dtarjan(A->trans, NULL, false);
    if (A->sccs_inv)
    {
        free(A->sccs_inv);
    }
    A->sccs_inv = parti_compute_inv(A->sccs);
}

void dfa_compute_tclos(dfa *A)
{
    dfa_compute_sccs(A);
    if (!A || A->tclos)
    {
        return;
    }
    A->tclos = compute_tclos_dgraph(A->trans, A->sccs, true);
}

/*****************************/
/* Generation of random NFAs */
/*****************************/

nfa *nfa_random(uint size_alpha, uint min_size, uint max_size)
{
    min_size = max(1, min_size);
    uint sizea = max(1, size_alpha);

    uint size = min_size + (rand() % (1 + max_size - min_size));

    nfa *A;
    CALLOC(A, 1);

    MALLOC(A->alphabet, sizea);
    for (uint i = 0; i < sizea; i++)
    {
        A->alphabet[i].lab = i + 'a';
        A->alphabet[i].num = -1;
    }

    dyn_edge_triple *dea = create_dyn_edge_triple();

    for (uint q = 0; q < size; q++)
    {
        for (uint a = 0; a < sizea; a++)
        {
            if (rand() % 8 == 0)
            {
                dyn_edge_triple_add(dea, q, a, q);
            }

            uint count = 0;
            for (uint r = q + 1; r < size; r++)
            {
                if (rand() % (2 + count++) == 0)
                {
                    dyn_edge_triple_add(dea, q, a, r);
                }
            }
            for (uint r = 0; r < q; r++)
            {
                if (rand() % (2 + count++) == 0)
                {
                    dyn_edge_triple_add(dea, q, a, r);
                }
            }
        }
    }

    A->trans = edge_list_to_lgraph(dea->array, dea->size, size, sizea);
    delete_dyn_edge_triple(dea);

    A->nb_initials = 1 + (rand() % (size / 2));
    MALLOC(A->initials, A->nb_initials);
    A->nb_finals = 1 + (rand() % (size / 2));
    MALLOC(A->finals, A->nb_finals);

    uint choice[size];
    for (uint i = 0; i < size; i++)
    {
        choice[i] = i;
    }

    for (uint i = 0; i < A->nb_initials; i++)
    {
        uint j = (rand() % (size - i));
        uint temp = choice[j];
        choice[size - i - 1] = choice[j];
        choice[j] = temp;
        A->initials[i] = choice[size - i - 1];
    }

    qsort(A->initials, A->nb_initials, sizeof(uint), compare_uint);

    for (uint i = 0; i < size; i++)
    {
        choice[i] = i;
    }

    for (uint i = 0; i < A->nb_finals; i++)
    {
        uint j = (rand() % (size - i));
        uint temp = choice[j];
        choice[size - i - 1] = choice[j];
        choice[j] = temp;
        A->finals[i] = choice[size - i - 1];
    }

    qsort(A->finals, A->nb_finals, sizeof(uint), compare_uint);
    return A;
}

dfa *dfa_random(uint size_alpha, uint min_size, uint max_size)
{
    min_size = max(1, min_size);
    uint sizea = max(1, size_alpha);

    uint size = min_size + (rand() % (1 + max_size - min_size));

    dfa *A;
    CALLOC(A, 1);
    MALLOC(A->alphabet, sizea);
    for (uint i = 0; i < sizea; i++)
    {
        A->alphabet[i].lab = i + 'a';
        A->alphabet[i].num = -1;
    }
    A->state_names = NULL;
    A->initial = 0;

    dequeue *finals = create_dequeue();
    int cf = 0;
    for (int i = size - 1; i >= 0; i--)
    {
        if (!(rand() % (2 + cf++)))
        {
            lefins_dequeue(i, finals);
        }
    }
    if (isempty_dequeue(finals))
    {
        rigins_dequeue(size - 1, finals);
    }
    A->nb_finals = size_dequeue(finals);
    MALLOC(A->finals, A->nb_finals);
    for (uint i = 0; i < A->nb_finals; i++)
    {
        A->finals[i] = lefread_dequeue(finals, i);
    }
    delete_dequeue(finals);

    A->trans = create_dgraph_noedges(size, sizea);
    for (uint q = 0; q < size; q++)
    {
        for (uint a = 0; a < sizea; a++)
        {
            uint count = 0;
            uint r = (q + 1) % size;
            ;
            while (count == 0)
            {
                if (r >= q && rand() % 2 == 0)
                {
                    A->trans->edges[q][a] = r;
                    count = 1;
                }
                else if (rand() % 8 == 0)
                {
                    A->trans->edges[q][a] = r;
                    count = 1;
                }
                else
                {
                    r = (r + 1) % size;
                }
            }
        }
    }
    return A;
}

/*****************/
/*+ Information +*/
/*****************/

int nfa_nb_trans(nfa *A)
{
    return A->trans->size_edges;
}

bool nfa_is_det(nfa *A)
{
    if (A->nb_initials > 1 || A->epsilon || A->inverse)
    {
        return false;
    }
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint start = LGSTART(A->trans, q, a);
            uint end = LGEND(A->trans, q, a);
            if (end - start > 1)
            {
                return false;
            }
        }
    }
    return true;
}

bool dfa_is_comp(dfa *A)
{
    return A->initial < A->trans->size_graph && A->trans->size_edges == A->trans->size_graph * A->trans->size_alpha;
}

bool nfa_is_comp(nfa *A)
{
    if (A->nb_initials == 0 || A->epsilon || A->inverse)
    {
        return false;
    }
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint start = LGSTART(A->trans, q, a);
            uint end = LGEND(A->trans, q, a);
            if (end == start)
            {
                return false;
            }
        }
    }
    return true;
}

bool nfa_is_empty(nfa *A)
{
    nfa *B = nfa_trim(A, false);
    bool res = B->nb_finals == 0;
    nfa_delete(B);
    return res;
}

bool nfa_accepts(nfa *A, word *w)
{
    if (A->nb_initials == 0 || A->nb_finals == 0)
    {
        return false;
    }
    uint *reached = nfa_compute_runs(A, w);
    for (uint i = 0; i < A->nb_finals; i++)
    {
        if (reached[A->finals[i] / sizeof(uint)] & (1 << (A->finals[i] % sizeof(uint))))
        {
            free(reached);
            return true;
        }
    }
    free(reached);
    return false;
}

// Calcule les états qui sont atteints par un mot dans un NFA.
uint *nfa_compute_runs(nfa *A, word *w)
{

    uint size_array = (A->trans->size_graph + sizeof(uint) - 1) / sizeof(uint);
    uint *in;

    CALLOC(in, size_array);
    if (A->nb_initials == 0)
    {
        return in;
    }

    for (uint i = 0; i < A->nb_initials; i++)
    {
        in[A->initials[i] / sizeof(uint)] |= (1 << (A->initials[i] % sizeof(uint)));
    }

    uint *out;
    CALLOC(out, size_array);

    // Lecture du mot
    for (uint i = 0; i < size_word(w); i++)
    {
        uint ind = letter_index(lefread_word(w, i), A->alphabet, A->nb_letters);
        if (ind >= A->nb_letters)
        {
            free(in);
            free(out);
            return NULL;
        }
        if (A->epsilon)
        {
            lgraph_reachable_eps(A->trans, in, out, ind, A->trans->size_alpha - 1);
        }
        else
        {

            lgraph_reachable(A->trans, in, out, ind);
        }
        uint *temp = in;
        in = out;
        out = temp;
        MEMSET(out, 0, size_array);
    }
    free(out);

    return in;
}

// Calcule les états qui sont atteints par un mot dans un NFA.
uint dfa_compute_run(dfa *A, word *w)
{

    uint q = A->initial;

    // Lecture du mot
    for (uint i = 0; i < size_word(w); i++)
    {
        letter l = lefread_word(w, i);
        uint letter_index = ((letter *)bsearch(&l, A->alphabet, A->trans->size_alpha, sizeof(letter), compare_letters)) - A->alphabet;
        if (letter_index == A->trans->size_alpha)
        {
            return UINT_MAX - 1; // Invalid letter.
        }
        q = A->trans->edges[q][letter_index];
        if (q == UINT_MAX)
        {
            return UINT_MAX; // No transition.
        }
    }
    return q;
}

nfa *nfa_left_quotient(nfa *A, word *w)
{
    uint *reached = nfa_compute_runs(A, w);
    if (!reached)
    {
        return NULL;
    }

    nfa *LQ;
    CALLOC(LQ, 1);
    LQ->epsilon = A->epsilon;
    LQ->inverse = A->inverse;
    LQ->nb_letters = A->nb_letters;
    LQ->alphabet = duplicate_alphabet(A->alphabet, A->nb_letters);
    LQ->trans = copy_lgraph(A->trans);
    // Initial states
    LQ->nb_initials = 0;
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (reached[q / sizeof(uint)] & (1 << (q % sizeof(uint))))
        {
            LQ->nb_initials++;
        }
    }
    MALLOC(LQ->initials, LQ->nb_initials);
    uint idx = 0;
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (reached[q / sizeof(uint)] & (1 << (q % sizeof(uint))))
        {
            LQ->initials[idx++] = q;
        }
    }

    // Final states
    LQ->nb_finals = A->nb_finals;
    MALLOC(LQ->finals, LQ->nb_finals);
    MEMCPY(LQ->finals, A->finals, LQ->nb_finals);
    free(reached);
    return LQ;
}

nfa *nfa_right_quotient(nfa *A, word *w)
{
    nfa *Ainv = nfa_mirror(A);
    nfa *LQinv = nfa_left_quotient(Ainv, w);
    nfa *RQ = nfa_mirror(LQinv);

    nfa_delete(Ainv);
    nfa_delete(LQinv);
    return RQ;
}

dfa *dfa_left_quotient(dfa *A, word *w)
{
    uint q = dfa_compute_run(A, w);
    if (q == UINT_MAX - 1)
    {
        return NULL;
    }

    dfa *LQ = dfa_copy(A);
    LQ->initial = q;
    return LQ;
}

dfa *dfa_right_quotient(dfa *A, word *w)
{
    nfa *Ainv = dfa_mirror(A);
    nfa *LQinv = nfa_left_quotient(Ainv, w);
    nfa *RQnd = nfa_mirror(LQinv);
    dfa *RQ = detnfa_to_dfa(RQnd);
    nfa_delete(RQnd);
    nfa_delete(Ainv);
    nfa_delete(LQinv);
    return RQ;
}
/************************/
/* Partitions of states */
/************************/

nfa *nfa_merge_states(nfa *A, parti *P)
{
    // Création de l'automate fusionné
    nfa *B;
    CALLOC(B, 1);
    B->epsilon = A->epsilon;
    B->inverse = A->inverse;
    B->nb_letters = A->nb_letters;

    B->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);

    // Transitions
    dyn_edge_triple *dea = create_dyn_edge_triple();
    for (uint q = 0; q < A->trans->size_graph; q++)
    {

        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint start = LGSTART(A->trans, q, a);
            uint end = LGEND(A->trans, q, a);
            for (uint j = start; j < end; j++)
            {
                if (A->epsilon && a == A->trans->size_alpha - 1 && P->numcl[q] == P->numcl[A->trans->storage[j]])
                {
                    continue;
                }
                dyn_edge_triple_add(dea, P->numcl[q], a, P->numcl[A->trans->storage[j]]);
            }
        }
    }
    B->trans = edge_list_to_lgraph(dea->array, dea->size, P->size_par, A->trans->size_alpha);
    delete_dyn_edge_triple(dea);

    // Initial states
    bool *ini_array;
    CALLOC(ini_array, P->size_par);
    for (uint i = 0; i < A->nb_initials; i++)
    {
        ini_array[P->numcl[A->initials[i]]] = true;
    }
    B->nb_initials = 0;
    for (uint q = 0; q < P->size_par; q++)
    {
        if (ini_array[q])
        {
            B->nb_initials++;
        }
    }
    MALLOC(B->initials, B->nb_initials);
    uint idx = 0;
    for (uint q = 0; q < P->size_par; q++)
    {
        if (ini_array[q])
        {
            B->initials[idx++] = q;
        }
    }

    // Final states
    bool *fin_array;
    CALLOC(fin_array, P->size_par);
    for (uint i = 0; i < A->nb_finals; i++)
    {
        fin_array[P->numcl[A->finals[i]]] = true;
    }
    B->nb_finals = 0;
    for (uint q = 0; q < P->size_par; q++)
    {
        if (fin_array[q])
        {
            B->nb_finals++;
        }
    }
    MALLOC(B->finals, B->nb_finals);
    idx = 0;
    for (uint q = 0; q < P->size_par; q++)
    {
        if (fin_array[q])
        {
            B->finals[idx++] = q;
        }
    }

    return B;
}

/****************/
/*+ Conversion +*/
/****************/

dfa *detnfa_to_dfa(nfa *A)
{
    if (!nfa_is_det(A))
    {
        return NULL;
    }
    uint size = A->trans->size_graph;

    dfa *D;
    CALLOC(D, 1);
    D->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    if (A->nb_initials == 1)
    {
        D->initial = A->initials[0];
    }
    else
    {
        D->initial = UINT_MAX;
    }
    D->nb_finals = A->nb_finals;
    MALLOC(D->finals, D->nb_finals);
    for (uint i = 0; i < D->nb_finals; i++)
    {
        D->finals[i] = A->finals[i];
    }

    D->trans = create_dgraph_noedges(size, A->trans->size_alpha);
    D->trans->size_edges = A->trans->size_edges;
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint start = LGSTART(A->trans, q, a);
            uint end = LGEND(A->trans, q, a);
            if (end == start + 1)
            {
                D->trans->edges[q][a] = A->trans->storage[start];
            }
            else
            {
                D->trans->edges[q][a] = UINT_MAX;
            }
        }
    }

    return D;
}

nfa *dfa_to_nfa(dfa *A)
{

    if (!A)
    {
        return NULL;
    }

    // Initialize the NFA
    nfa *N;
    CALLOC(N, 1);
    N->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    N->nb_letters = A->trans->size_alpha;
    N->epsilon = false;
    N->inverse = false;

    // Initial states
    if (A->initial == UINT_MAX)
    {
        N->nb_initials = 0;
        N->initials = NULL;
    }
    else
    {
        N->nb_initials = 1;
        MALLOC(N->initials, 1);
        N->initials[0] = A->initial;
    }

    // Final states
    N->nb_finals = A->nb_finals;
    if (N->nb_finals == 0)
    {
        N->finals = NULL;
    }
    else
    {
        MALLOC(N->finals, N->nb_finals);
        for (uint i = 0; i < A->nb_finals; i++)
        {
            N->finals[i] = A->finals[i];
        }
    }

    N->trans = dgraph_to_lgraph(A->trans);

    // Gestion des noms d'états
    if (A->state_names)
    {
        N->state_names = copy_all_names(A->state_names, A->trans->size_graph);
    }

    return N;
}
