#include "nfa_props.h"
#include "limits.h"
#include "monoid_display.h"
#include "monoid_props.h"
#include "nfa_intersec.h"
#include "nfa_minimization.h"
#include "printing.h"

generic_pattern *make_generic_pattern(void)
{
    generic_pattern *newpattern;
    MALLOC(newpattern, 1);
    for (uint i = 0; i < PATTERN_MAXSIZE; i++)
    {
        newpattern->states[i] = UINT_MAX;
        newpattern->words[i] = NULL;
        newpattern->vars[i] = 'u' + i;
        newpattern->types[i] = PAT_NONE;
    }
    return newpattern;
}

void free_generic_pattern(generic_pattern *pattern)
{
    if (pattern)
    {
        for (uint i = 0; i < PATTERN_MAXSIZE; i++)
        {
            if (pattern->words[i])
            {
                free(pattern->words[i]);
            }
        }
        free(pattern);
    }
}
uint *make_singleton_word(uint letter)
{
    uint *word;
    MALLOC(word, 2);
    word[0] = letter;
    word[1] = UINT_MAX;
    return word;
}

uint *make_epsilon_word(void)
{
    uint *word;
    MALLOC(word, 1);
    word[0] = UINT_MAX;
    return word;
}

uint *copy_word(uint *word)
{
    if (!word)
    {
        return NULL;
    }
    uint len = 0;
    while (word[len] != UINT_MAX)
    {
        len++;
    }
    uint *newword;
    MALLOC(newword, len + 1);
    for (uint i = 0; i < len; i++)
    {
        newword[i] = word[i];
    }
    newword[len] = UINT_MAX;
    return newword;
}

/***********/
/* Trivial */
/***********/

// Pour l'automate minimal
bool is_trivial_dfa(dfa *A, FILE *out)
{
    if (out)
    {
        fprintf(out, "#### Checking if this automaton is trivial (i.e., has a single state).\n");
    }

    if (A->trans->size_graph == 1)
    {
        if (out != NULL)
        {
            fprintf(out, "#### This automaton has a single state.\n");
        }
        return true;
    }
    else
    {
        if (out != NULL)
        {
            fprintf(out, "#### This automaton has more than one state.\n");
        }
        return false;
    }
}

bool is_ddtrivial_dfa(dfa *A, FILE *out)
{
    // Printing the pattern if there is a stream to print to.
    MAKE_PATTERN_INTRO_STRICT(DD_PATTERN, out);

    // An automaton with a single state statisfies the DD pattern equation.
    if (A->trans->size_graph == 1)
    {
        return true;
    }

    // We pick an arbitrary transition to check the letter actions.
    uint r = A->trans->edges[0][0];

    // We look for any transition that does not map to the same state (in which case, the pattern is not satisfied).
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            if (A->trans->edges[q][a] != r)
            {
                if (out)
                {
                    generic_pattern *mypattern = make_generic_pattern();
                    mypattern->states[0] = 0;
                    mypattern->states[1] = r;
                    mypattern->states[2] = q;
                    mypattern->states[3] = A->trans->edges[q][a];
                    mypattern->vars[0] = 'u';
                    mypattern->vars[1] = 'v';
                    mypattern->types[0] = PAT_WORD;
                    mypattern->types[1] = PAT_WORD;
                    mypattern->words[0] = make_singleton_word(0);
                    mypattern->words[1] = make_singleton_word(a);
                    view_pattern(A, mypattern, dd_pattern_print);
                    CONCLUSION_NEGATIVE(out);
                    free_generic_pattern(mypattern);
                }
                return false;
            }
        }
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

// Pour l'automate minimal
bool is_letterind_dfa(dfa *A, FILE *out)
{
    if (out)
    {
        fprintf(out, "#### Checking if this automaton is letter uniform.\n");
    }

    // We look for any state where two letters have different actions (in which case, the pattern is not satisfied).
    for (uint s = 0; s < A->trans->size_graph; s++)
    {
        for (uint a = 1; a < A->trans->size_alpha; a++)
        {
            if (A->trans->edges[s][0] != A->trans->edges[s][a])
            {
                if (out)
                {
                    fprintf(out, "#### The automaton is not letter uniform as it contains the transitions ");
                    fprintf(out, "(");
                    dfa_print_state(A, s, out);
                    fprintf(out, ",");
                    fprint_letter_utf8(A->alphabet[0], out);
                    fprintf(out, ",");
                    dfa_print_state(A, A->trans->edges[s][0], out);
                    fprintf(out, ") and (");
                    dfa_print_state(A, s, out);
                    fprintf(out, ",");
                    fprint_letter_utf8(A->alphabet[a], out);
                    fprintf(out, ",");
                    dfa_print_state(A, A->trans->edges[s][a], out);
                    fprintf(out, ")\n");
                }
                return false;
            }
        }
    }
    if (out)
    {
        fprintf(out, "#### The automaton is letter uniform.\n");
    }
    return true;
}

bool is_permutation_dfa(dfa *A, FILE *out)
{

    MAKE_PATTERN_INTRO(GR_PATTERN, out);

    uint *reached;
    MALLOC(reached, A->trans->size_graph);

    // For each letter a, we look for two a-transitions leading to the same state.
    for (uint a = 0; a < A->trans->size_alpha; a++)
    {

        // We remember which states have already been reached by an a-transition.
        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            reached[q] = UINT_MAX;
        }

        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            uint r = A->trans->edges[q][a];
            if (reached[r] == UINT_MAX) // No a-transition to r was found yet.
            {
                reached[r] = q;
            }
            else // An a-transition to r was already found.
            {
                if (out)
                {
                    generic_pattern *mypattern = make_generic_pattern();
                    mypattern->states[0] = r;
                    mypattern->states[1] = reached[r];
                    mypattern->states[2] = q;
                    mypattern->vars[0] = 'u';
                    mypattern->types[0] = PAT_WORD;
                    mypattern->words[0] = make_singleton_word(a);
                    view_pattern(A, mypattern, gr_pattern_print);
                    CONCLUSION_NEGATIVE(out);
                    free_generic_pattern(mypattern);
                }
                free(reached);
                return false;
            }
        }
    }
    free(reached);
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_permutationplus_dfa(dfa *A, FILE *out)
{
    uint *reached;
    MALLOC(reached, A->trans->size_graph);

    bool iniprec = false;
    for (uint a = 0; a < A->trans->size_alpha; a++)
    {
        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            if (A->trans->edges[q][a] == A->initial)
            {
                iniprec = true;
            }
        }
    }

    MAKE_PATTERN_INTRO_STRICT(GRPLUS1_PATTERN, out);

    for (uint a = 0; a < A->trans->size_alpha; a++)
    {
        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            reached[q] = UINT_MAX;
        }

        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            if (!iniprec && q == A->initial)
            {
                continue;
            }

            uint r = A->trans->edges[q][a];

            if (reached[r] == UINT_MAX)
            {
                reached[r] = q;
                continue;
            }

            if (out)
            {
                generic_pattern *mypattern = make_generic_pattern();
                mypattern->states[0] = r;
                mypattern->states[1] = reached[r];
                mypattern->states[2] = q;
                mypattern->states[3] = A->initial;
                mypattern->states[4] = A->initial;
                mypattern->vars[0] = 'u';
                mypattern->vars[1] = 'v';
                mypattern->vars[2] = 'w';
                mypattern->types[0] = PAT_WORD;
                mypattern->types[1] = PAT_WORD;
                mypattern->types[2] = PAT_WORD;
                mypattern->words[0] = make_singleton_word(a);
                dgraph_exists_path(A->trans, A->initial, reached[r], true, &mypattern->words[1]);
                dgraph_exists_path(A->trans, A->initial, q, true, &mypattern->words[2]);
                view_pattern(A, mypattern, grp1_pattern_print);
                CONCLUSION_NEGATIVE(out);
                free_generic_pattern(mypattern);
            }
            free(reached);
            return false;
        }
    }
    free(reached);

    MAKE_PATTERN_INTRO_STRICT(GRPLUS2_PATTERN, out);

    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (q == A->initial)
        {
            continue;
        }
        if (A->trans->edges[q][0] == A->trans->edges[A->initial][0])
        {
            for (uint a = 1; a < A->trans->size_alpha; a++)
            {
                if (A->trans->edges[q][a] != A->trans->edges[A->initial][a])
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = A->initial;
                        mypattern->states[1] = q;
                        mypattern->states[2] = A->trans->edges[q][0];
                        mypattern->states[3] = A->trans->edges[q][a];
                        mypattern->states[4] = A->trans->edges[A->initial][a];
                        mypattern->vars[0] = 'u';
                        mypattern->vars[1] = 'v';
                        mypattern->types[0] = PAT_WORD;
                        mypattern->types[1] = PAT_WORD;
                        mypattern->words[0] = make_singleton_word(0);
                        mypattern->words[1] = make_singleton_word(a);
                        view_pattern(A, mypattern, grp2_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    return false;
                }
            }
        }
        else
        {
            for (uint a = 1; a < A->trans->size_alpha; a++)
            {
                if (A->trans->edges[q][a] == A->trans->edges[A->initial][a])
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = A->initial;
                        mypattern->states[1] = q;
                        mypattern->states[2] = A->trans->edges[q][a];
                        mypattern->states[3] = A->trans->edges[q][0];
                        mypattern->states[4] = A->trans->edges[A->initial][0];
                        mypattern->vars[0] = 'u';
                        mypattern->vars[1] = 'v';
                        mypattern->types[0] = PAT_WORD;
                        mypattern->types[1] = PAT_WORD;
                        mypattern->words[0] = make_singleton_word(a);
                        mypattern->words[1] = make_singleton_word(0);
                        view_pattern(A, mypattern, grp2_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    return false;
                }
            }
        }
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_comm_dfa(dfa *A, FILE *out)
{
    MAKE_PATTERN_INTRO(COMM_PATTERN, out);
    // We look for two letters a and b and a state q such that the a-transition followed by the b-transition
    // does not lead to the same state as the b-transition followed by the a-transition.
    for (uint a = 0; a < A->trans->size_alpha; a++)
    {
        for (uint b = a + 1; b < A->trans->size_alpha; b++)
        {
            for (uint q = 0; q < A->trans->size_graph; q++)
            {
                uint qa = A->trans->edges[q][a];
                uint qb = A->trans->edges[q][b];
                uint qab = A->trans->edges[qa][b];
                uint qba = A->trans->edges[qb][a];
                if (qab != qba)
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = q;
                        mypattern->states[1] = qa;
                        mypattern->states[2] = qab;
                        mypattern->states[3] = qb;
                        mypattern->states[4] = qba;
                        mypattern->vars[0] = 'u';
                        mypattern->vars[1] = 'v';
                        mypattern->types[0] = PAT_WORD;
                        mypattern->types[1] = PAT_WORD;
                        mypattern->words[0] = make_singleton_word(a);
                        mypattern->words[1] = make_singleton_word(b);
                        view_pattern(A, mypattern, com_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    return false;
                }
            }
        }
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_alphaidem_dfa(dfa *A, FILE *out)
{
    MAKE_PATTERN_INTRO(IDEM_PATTERN, out);
    // We look for any state q and letter a such that the a-transition from q followed by another a-transition
    // does not lead to the same state as the single a-transition from q.  This only tests idempotency of each
    // letter (which is equivalent to full idempotency in the commutative case).
    for (uint a = 0; a < A->trans->size_alpha; a++)
    {
        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            uint r = A->trans->edges[q][a];
            uint s = A->trans->edges[r][a];
            if (r != s)
            {
                if (out)
                {
                    generic_pattern *mypattern = make_generic_pattern();
                    mypattern->states[0] = q;
                    mypattern->states[1] = r;
                    mypattern->states[2] = s;
                    mypattern->vars[0] = 'u';
                    mypattern->types[0] = PAT_WORD;
                    mypattern->words[0] = make_singleton_word(a);
                    view_pattern(A, mypattern, idem_pattern_print);
                    free_generic_pattern(mypattern);
                    CONCLUSION_NEGATIVE(out);
                }
                return false;
            }
        }
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_ddidem_dfa(dfa *A, FILE *out)
{
    MAKE_PATTERN_INTRO_STRICT(LTIDEM_PATTERN, out);
    dgraph *inter = dgraph_direct_product(A->trans, A->trans);
    parti *scci = dtarjan(inter, NULL, false);
    dgraph_discard_nonscc_edges(inter, scci);

    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint r = 0; r < A->trans->size_graph; r++)
        {
            uint qr = q * A->trans->size_graph + r;
            if (!dgraph_ntrivial_loop(inter, scci, qr))
            {
                continue;
            }
            for (uint s = 0; s < A->trans->size_graph; s++)
            {
                uint qs = q * A->trans->size_graph + s;
                uint rs = r * A->trans->size_graph + s;
                if (s == r || !dgraph_ntrivial_loop(inter, scci, qs) || !dgraph_ntrivial_loop(inter, scci, rs))
                {
                    continue;
                }
                if (dgraph_exists_intersec_path(inter, inter, qr, rs, qr, rs, true, NULL) &&
                    dgraph_exists_intersec_path(A->trans, A->trans, q, r, r, s, true, NULL))
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = q;
                        mypattern->states[1] = r;
                        mypattern->states[2] = s;
                        mypattern->vars[0] = 'u';
                        mypattern->vars[1] = 'z';
                        mypattern->types[0] = PAT_WORD;
                        mypattern->types[1] = PAT_WORD;
                        dgraph_exists_intersec_path(A->trans, A->trans, q, r, r, s, true, &mypattern->words[0]);
                        dgraph_exists_intersec_path(inter, inter, qr, rs, qr, rs, true, &mypattern->words[1]);
                        view_pattern(A, mypattern, idem_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    delete_dgraph(inter);
                    delete_parti(scci);
                    return false;
                }
            }
        }
    }
    delete_dgraph(inter);
    delete_parti(scci);
    CONCLUSION_POSITIVE(out);
    return true;
}

static uint *word_from_morphism(morphism *M, uint el)
{
    uint n = 0;
    uint t = el;
    ;
    while (t != ONE)
    {
        n++;
        t = M->pred_ele[t];
    }
    uint *word;
    MALLOC(word, n + 1);
    word[n] = UINT_MAX; // We add a sentinel value at the end of the word
    t = el;
    while (t != ONE)
    {
        n--;
        word[n] = M->pred_lab[t];
        t = M->pred_ele[t];
    }
    return word;
}

static uint state_from_word(dfa *A, uint state, uint *word)
{
    uint i = 0;
    while (word[i] != UINT_MAX)
    {
        state = A->trans->edges[state][word[i]];
        i++;
    }
    return state;
}

bool is_ddcomm_dfa(dfa *A, int *error, FILE *out)
{
    MAKE_PATTERN_INTRO_STRICT(LTCOMM_PATTERN, out);
    uint *funs;
    morphism *M = dfa_to_morphism(A, error, &funs);
    orbits *ddorbs = compute_ddorbits(M);
    uint c[3];
    if (is_com_orbmono(ddorbs, c))
    {
        delete_morphism(M);
        free(funs);
        delete_orbits(ddorbs);
        CONCLUSION_POSITIVE(out);
        return true;
    }
    else
    {
        if (out)
        {
            uint *w = word_from_morphism(M, c[0]);
            uint *v = word_from_morphism(M, c[1]);
            uint *z = word_from_morphism(M, c[2]);

            uint el1 = mor_mult_gen(M, 5, c[2], c[0], c[2], c[1], c[2]);
            uint el2 = mor_mult_gen(M, 5, c[2], c[1], c[2], c[0], c[2]);
            uint q0 = 0, q1, q2, q3, q4;
            for (uint qi = 0; qi < A->trans->size_graph; qi++)
            {
                if (funs[A->trans->size_graph * el1 + qi] != funs[A->trans->size_graph * el2 + qi])
                {
                    q0 = qi;
                    break;
                }
            }

            q0 = state_from_word(A, q0, z);
            q1 = state_from_word(A, q0, w);
            q1 = state_from_word(A, q1, z);
            q2 = state_from_word(A, q1, v);
            q2 = state_from_word(A, q2, z);
            q3 = state_from_word(A, q0, v);
            q3 = state_from_word(A, q3, z);
            q4 = state_from_word(A, q3, w);
            q4 = state_from_word(A, q4, z);

            generic_pattern *mypattern = make_generic_pattern();
            mypattern->states[0] = q0;
            mypattern->states[1] = q1;
            mypattern->states[2] = q2;
            mypattern->states[3] = q3;
            mypattern->states[4] = q4;
            mypattern->vars[0] = 'u';
            mypattern->vars[1] = 'v';
            mypattern->vars[2] = 'z';
            mypattern->types[0] = PAT_WORD;
            mypattern->types[1] = PAT_WORD;
            mypattern->types[2] = PAT_WORD;
            mypattern->words[0] = w;
            mypattern->words[1] = v;
            mypattern->words[2] = z;
            view_pattern(A, mypattern, com_pattern_print);
            free_generic_pattern(mypattern);
            CONCLUSION_NEGATIVE(out);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(ddorbs);
        return false;
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_lt_opti_dfa(dfa *A)
{
    // Computing the SCCs of the automaton if necessary.
    dfa_compute_sccs(A);

    // For each SCC of the graph
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // If the SCC is trivial, it cannot yield a counterexample and we skip it.
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }
        // We extract the SCC from the graph.
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        // We compute the direct product of the SCC with itself.
        dgraph *product = dgraph_direct_product(thescc, thescc);
        // We compute the SCCs of the product graph (used to compute non-trivial loops).
        parti *sccp = dtarjan(product, NULL, false);
        // We no longer need the original SCC graph.
        delete_dgraph(thescc);

        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / A->sccs->cl_size[i];
            uint r = qr % A->sccs->cl_size[i];

            // If q and r are distinct and there is a non-trivial common loop on q and r, then we have found a counterexample.
            if (q != r && dgraph_ntrivial_loop(product, sccp, qr))
            {
                delete_parti(sccp);
                delete_dgraph(product);
                return false;
            }
        }
        delete_parti(sccp);
        delete_dgraph(product);
    }

    // We now perform the main loop to check the stability condition.

    // We need the transitive closure of the DFA on its SCCs.
    dfa_compute_tclos(A);

    // We create the adjacency matrix of the SCCs.
    bool *adj_matrix;
    CALLOC(adj_matrix, A->sccs->size_par * A->sccs->size_par);

    dequeue *queue = create_dequeue();
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        adj_matrix[i * A->sccs->size_par + i] = true;
        dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);
        for (uint j = start; j < end; j++)
        {
            uint k = A->tclos->storage[j];
            adj_matrix[i * A->sccs->size_par + k] = true;
            dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, k);
            dgraph *prod = dgraph_direct_product(scc1, scc2);
            parti *sccp = dtarjan(prod, NULL, false);
            for (uint l = 0; l < sccp->size_par; l++)
            {
                uint qr = sccp->cl_elems[l][0];
                if (dgraph_ntrivial_loop(prod, sccp, qr))
                {
                    uint q = qr / A->sccs->cl_size[k];
                    uint r = qr % A->sccs->cl_size[k];
                    uint qo = A->sccs->cl_elems[i][q];
                    uint ro = A->sccs->cl_elems[k][r];
                    rigins_dequeue(qo * A->trans->size_graph + ro, queue);
                }
            }
            delete_parti(sccp);
            delete_dgraph(prod);
            delete_dgraph(scc2);
        }
        delete_dgraph(scc1);
    }
    bool *visited;
    CALLOC(visited, A->trans->size_graph * A->trans->size_graph);
    // printf("Starting the main loop with %u elements in the queue.\n", queue->right - queue->left);

    while (!isempty_dequeue(queue))
    {
        uint qr = lefpull_dequeue(queue);
        if (visited[qr])
        {
            continue;
        }
        visited[qr] = true;
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint qs = A->trans->edges[q][a];
            uint rs = A->trans->edges[r][a];

            bool stayq = adj_matrix[A->sccs->numcl[qs] * A->sccs->size_par + A->sccs->numcl[r]];
            bool stayr = A->sccs->numcl[rs] == A->sccs->numcl[r];
            if (stayq && stayr)
            {
                uint qrs = qs * A->trans->size_graph + rs;
                rigins_dequeue(qrs, queue);
            }
            else if (!stayq && !stayr)
            {
                continue;
            }
            else
            {
                delete_dequeue(queue);
                free(visited);
                free(adj_matrix);
                return false;
            }
        }
    }
    delete_dequeue(queue);
    free(adj_matrix);
    free(visited);
    return true;
}

bool is_lttcomm_dfa(dfa *A, int *error, FILE *out)
{
    MAKE_PATTERN_INTRO_STRICT(LTTCOMM_PATTERN, out);
    uint *funs;
    morphism *M = dfa_to_morphism(A, error, &funs);
    orbits *ddorbs = compute_ddorbits(M);
    uint c[5];
    if (is_comm_ltt_mono(ddorbs, c))
    {
        delete_morphism(M);
        free(funs);
        delete_orbits(ddorbs);
        CONCLUSION_POSITIVE(out);
        return true;
    }
    else
    {
        if (out)
        {

            uint *u = word_from_morphism(M, c[0]);
            uint *v = word_from_morphism(M, c[1]);
            uint *w = word_from_morphism(M, c[2]);
            uint *y = word_from_morphism(M, c[3]);
            uint *z = word_from_morphism(M, c[4]);

            uint el1 = mor_mult_gen(M, 7, c[3], c[0], c[4], c[1], c[3], c[2], c[4]);
            uint el2 = mor_mult_gen(M, 7, c[3], c[2], c[4], c[1], c[3], c[0], c[4]);
            uint q0 = 0, q1, q2, q3, q4, q5, q6;
            for (uint qi = 0; qi < A->trans->size_graph; qi++)
            {
                if (funs[A->trans->size_graph * el1 + qi] != funs[A->trans->size_graph * el2 + qi])
                {
                    q0 = qi;
                    break;
                }
            }

            q0 = state_from_word(A, q0, y);
            q1 = state_from_word(A, q0, u);
            q1 = state_from_word(A, q1, z);
            q2 = state_from_word(A, q1, v);
            q2 = state_from_word(A, q2, y);
            q3 = state_from_word(A, q2, w);
            q3 = state_from_word(A, q3, z);

            q4 = state_from_word(A, q0, w);
            q4 = state_from_word(A, q4, z);
            q5 = state_from_word(A, q4, v);
            q5 = state_from_word(A, q5, y);
            q6 = state_from_word(A, q5, u);
            q6 = state_from_word(A, q6, z);

            generic_pattern *mypattern = make_generic_pattern();
            mypattern->states[0] = q0;
            mypattern->states[1] = q1;
            mypattern->states[2] = q2;
            mypattern->states[3] = q3;
            mypattern->states[4] = q4;
            mypattern->states[5] = q5;
            mypattern->states[6] = q6;
            mypattern->vars[0] = 'u';
            mypattern->vars[1] = 'v';
            mypattern->vars[2] = 'w';
            mypattern->vars[3] = 'y';
            mypattern->vars[4] = 'z';
            mypattern->types[0] = PAT_WORD;
            mypattern->types[1] = PAT_WORD;
            mypattern->types[2] = PAT_WORD;
            mypattern->types[3] = PAT_WORD;
            mypattern->types[4] = PAT_WORD;
            mypattern->words[0] = u;
            mypattern->words[1] = w;
            mypattern->words[2] = v;
            mypattern->words[3] = y;
            mypattern->words[4] = z;
            view_pattern(A, mypattern, lttcom_pattern_print);
            free_generic_pattern(mypattern);
            CONCLUSION_NEGATIVE(out);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(ddorbs);
        return false;
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

// Generates a DFA from a single SCC of an input DFA. A sink state is added to the end of the transition graph.
static dfa *dfa_from_scc(dfa *A, parti *sccs, uint p)
{
    dfa *D;
    CALLOC(D, 1);
    D->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    D->trans = create_dgraph_noedges(sccs->cl_size[p] + 1, A->trans->size_alpha);
    D->nb_finals = 0;

    for (uint i = 0; i < sccs->cl_size[p]; i++)
    {
        uint q = sccs->cl_elems[p][i];
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint r = A->trans->edges[q][a];
            if (sccs->numcl[r] != p)
            {
                D->trans->edges[i][a] = sccs->cl_size[p];
            }
            else
            {
                D->trans->edges[i][a] = ((uint *)bsearch(&r, sccs->cl_elems[p], sccs->cl_size[p], sizeof(uint), &compare_uint)) - sccs->cl_elems[p];
            }
        }
    }

    for (uint a = 0; a < A->trans->size_alpha; a++)
    {
        D->trans->edges[sccs->cl_size[p]][a] = sccs->cl_size[p];
    }

    D->initial = 0; // L'état initial est le premier de la liste

    return D;
}

/*************************/
/* Counter-free variants */
/*************************/

bool is_counterfreegp_dfa(dfa *A, basis mode, int *error, FILE *out)
{
    // Printing the pattern if there is a stream to print to.
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(SF_PATTERN_ST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(SF_PATTERN_MOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(SF_PATTERN_AMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(SF_PATTERN_GR, out);
            break;
        default:
            break;
        }
    }

    // First compute the SCCs of the DFA.
    dfa_compute_sccs(A);

    // Consider each SCC independently.
    for (uint c = 0; c < A->sccs->size_par; c++)
    {
        // If the SCC consists of a single state, it cannot contain a counter and we skip it.
        if (A->sccs->cl_size[c] <= 1)
        {
            continue;
        }

        // Compute the automaton obtained by only keeping this SCC
        dfa *D = dfa_from_scc(A, A->sccs, c);

        // We compute the associated morphism and save the function table used to construct it.
        *error = 0;

        uint *funs;
        morphism *M = dfa_to_morphism(D, NULL, &funs);
        if (*error < 0)
        {
            return false;
        }

        // We compute the G-lernel if the basis G is not ST (otherwise, the ST-kernel is the whole monoid and we can directly use the Green relations of the monoid).
        subsemi *ker = mode == BA_ST ? NULL : get_kernel(M, LV_REG, mode);
        green *GREL = mode == BA_ST ? M->rels : ker->rels;

        // If there exists a non-trivial group, then the automaton contains a G-counter.
        if (GREL->HCL->size_set != GREL->HCL->size_par)
        {
            // If a counterexample is requested, we will print it.
            if (out)
            {
                uint e = 0;
                uint s = 0;

                // We look for a non-trivial group in the G-kernel and pick a non-idempotent element of this group.
                for (uint i = 0; i < M->nb_idems; i++)
                {
                    e = M->idem_list[i];
                    uint sube = ker ? ker->mono_to_sub[e] : e;
                    uint cl = GREL->HCL->numcl[sube];
                    if (GREL->HCL->cl_size[cl] > 1)
                    {
                        // We take an element of the class which is not an idempotent.
                        uint subs = GREL->HCL->cl_elems[cl][0];
                        if (ker ? ker->idem_array[subs] : M->idem_array[subs])
                        {
                            subs = GREL->HCL->cl_elems[cl][1];
                        }
                        s = ker ? ker->sub_to_mono[subs] : subs;
                        break;
                    }
                }

                // We pick a word w that leads to the element s of the monoid.
                uint *w = word_from_morphism(M, s);

                // We now look for the counter itself.
                uint q = 0;
                for (uint j = 0; j < D->trans->size_graph; j++)
                {
                    if (funs[D->trans->size_graph * e + j] == j && funs[D->trans->size_graph * s + j] != j)
                    {
                        q = A->sccs->cl_elems[c][j];
                        break;
                    }
                }

                // The counter starts at state q, and we have the word w that leads to the counter.
                // We now compute the length of the counter.
                uint n = 0;
                uint r = state_from_word(A, q, w);
                while (r != q)
                {
                    n++;
                    r = state_from_word(A, r, w);
                }

                uint states[n + 1];
                states[0] = q;
                r = state_from_word(A, q, w);
                for (uint h = 1; h <= n; h++)
                {
                    states[h] = r;
                    r = state_from_word(A, r, w);
                }
                view_sfc_pattern(A, states, n + 1, mode == BA_ST ? w : NULL, mode == BA_ST ? 'w' : 'W');
                free(w);
                CONCLUSION_NEGATIVE(out);
            }
            delete_subsemi(ker);
            dfa_delete(D);
            free(funs);
            delete_morphism(M);
            return false;
        }
        dfa_delete(D);
        free(funs);
        delete_morphism(M);
        delete_subsemi(ker);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_nosmallcountertwo_dfa(dfa *A)
{
    // Computing the SCCs of the automaton.
    dfa_compute_sccs(A);

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        if (A->sccs->cl_size[i] == 1)
        {
            continue;
        }
        // We compute the direct product of the SCC with itself.
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

        // We compute the direct product of the SCC with itself and its SCCs.
        dgraph *inter = dgraph_direct_product(thescc, thescc);
        parti *scci = dtarjan(inter, NULL, false);

        for (uint q = 0; q < thescc->size_graph; q++)
        {
            for (uint r = q + 1; r < thescc->size_graph; r++)
            {
                if (scci->numcl[q + r * thescc->size_graph] == scci->numcl[r + q * thescc->size_graph])
                {
                    delete_dgraph(thescc);
                    delete_dgraph(inter);
                    delete_parti(scci);
                    return false;
                }
            }
        }
        delete_dgraph(thescc);
        delete_dgraph(inter);
        delete_parti(scci);
    }
    return true;
}

bool is_nosimple_counter_dfa(dfa *A, int *)
{
    bool alpha[A->trans->size_alpha];
    MEMSET(alpha, false, A->trans->size_alpha);
    for (uint a = 0; a < A->trans->size_alpha; a++)
    {
        alpha[a] = true;
        parti *sccs = dtarjan(A->trans, alpha, false);
        if (sccs->size_par != sccs->size_set)
        {
            // If the number of SCCs is not equal to the number of states, then there is a counter.
            delete_parti(sccs);
            return false;
        }
        delete_parti(sccs);
        alpha[a] = false;
    }

    return true;
}

/****************/
/*+ R-trivial. +*/
/****************/

bool is_rtrivialgp_dfa_noprint(dfa *A, basis mode, generic_pattern *cexa)
{
    // Computing the SCCs of the automaton.
    dfa_compute_sccs(A);
    if (mode == BA_ST)
    {
        // If all SCCs are trivial, then the pattern is satisfied.
        if (A->sccs->size_par != A->trans->size_graph)
        {
            if (cexa)
            {
                for (uint i = 0; i < A->sccs->size_par; i++)
                {
                    if (A->sccs->cl_size[i] > 1)
                    {
                        cexa->states[0] = A->sccs->cl_elems[i][0];
                        cexa->states[1] = A->sccs->cl_elems[i][1];
                        cexa->vars[0] = 'u';
                        cexa->vars[1] = 'v';
                        cexa->types[0] = PAT_WORD;
                        cexa->types[1] = PAT_WORD;
                        dgraph_exists_path(A->trans, cexa->states[0], cexa->states[1], false, &cexa->words[0]);
                        dgraph_exists_path(A->trans, cexa->states[1], cexa->states[0], false, &cexa->words[1]);
                        break;
                    }
                }
            }
            return false;
        }
        return true;
    }

    if (mode == BA_GR)
    {

        for (uint i = 0; i < A->sccs->size_par; i++)
        {
            if (A->sccs->cl_size[i] <= 1)
            {
                continue;
            }
            for (uint a = 0; a < A->trans->size_alpha; a++)
            {
                uint reached[A->sccs->cl_size[i]];
                MEMSET(reached, UINT_MAX, A->sccs->cl_size[i]);
                for (uint j = 0; j < A->sccs->cl_size[i]; j++)
                {
                    uint q = A->sccs->cl_elems[i][j];
                    uint r = A->trans->edges[q][a];
                    if (A->sccs->numcl[r] != i)
                    {
                        continue;
                    }
                    else if (reached[A->sccs_inv[r]] != UINT_MAX)
                    {
                        if (cexa)
                        {
                            cexa->states[0] = reached[A->sccs_inv[r]];
                            cexa->states[1] = q;
                            cexa->vars[0] = 'U';
                            cexa->vars[1] = 'V';
                            cexa->types[0] = PAT_LANG;
                            cexa->types[1] = PAT_LANG;
                        }
                        return false;
                    }
                    else
                    {
                        reached[A->sccs_inv[r]] = q;
                    }
                }
            }
        }
        return true;
    }

    if (mode == BA_MOD)
    {

        for (uint i = 0; i < A->sccs->size_par; i++)
        {
            if (A->sccs->cl_size[i] <= 1)
            {
                continue;
            }
            for (uint jq = 0; jq < A->sccs->cl_size[i]; jq++)
            {
                uint q = A->sccs->cl_elems[i][jq];
                uint r = UINT_MAX;
                for (uint a = 0; a < A->trans->size_alpha; a++)
                {
                    uint s = A->trans->edges[q][a];
                    if (A->sccs->numcl[s] != i)
                    {
                        continue;
                    }
                    else if (r != UINT_MAX && r != s)
                    {
                        if (cexa)
                        {
                            cexa->states[0] = r;
                            cexa->states[1] = s;
                            cexa->vars[0] = 'U';
                            cexa->vars[1] = 'V';
                            cexa->types[0] = PAT_LANG;
                            cexa->types[1] = PAT_LANG;
                        }
                        return false;
                    }
                    else
                    {
                        r = s;
                    }
                }
            }

            uint reached[A->sccs->cl_size[i]];
            MEMSET(reached, UINT_MAX, A->sccs->cl_size[i]);
            for (uint j = 0; j < A->sccs->cl_size[i]; j++)
            {
                uint q = A->sccs->cl_elems[i][j];

                for (uint a = 0; a < A->trans->size_alpha; a++)
                {
                    uint r = A->trans->edges[q][a];
                    if (A->sccs->numcl[r] != i)
                    {
                        continue;
                    }
                    else if (reached[A->sccs_inv[r]] != UINT_MAX && reached[A->sccs_inv[r]] != q)
                    {
                        if (cexa)
                        {
                            cexa->states[0] = reached[A->sccs_inv[r]];
                            cexa->states[1] = q;
                            cexa->vars[0] = 'U';
                            cexa->vars[1] = 'V';
                            cexa->types[0] = PAT_LANG;
                            cexa->types[1] = PAT_LANG;
                        }
                        return false;
                    }
                    else
                    {
                        reached[A->sccs_inv[r]] = q;
                    }
                }
            }
        }
        return true;
    }

    // Folding according to the basis which is either MOD, AMT or GR.
    parti *fold = dgraph_stal_fold(A->trans, A->sccs, mode);

    // If no states have been folded, then the pattern is satisfied.
    if (fold->size_par != fold->size_set)
    {
        // If a counterexample is requested, we return two states from a folded class.
        if (cexa)
        {
            for (uint i = 0; i < fold->size_par; i++)
            {
                if (fold->cl_size[i] > 1)
                {
                    cexa->states[0] = A->sccs->cl_elems[i][0];
                    cexa->states[1] = A->sccs->cl_elems[i][1];
                    cexa->vars[0] = 'U';
                    cexa->vars[1] = 'V';
                    cexa->types[0] = PAT_LANG;
                    cexa->types[1] = PAT_LANG;
                    break;
                }
            }
        }
        delete_parti(fold);
        return false;
    }
    delete_parti(fold);
    return true;
}

bool is_rtrivialgp_dfa(dfa *A, basis mode, FILE *out)
{
    if (!out)
    {
        return is_rtrivialgp_dfa_noprint(A, mode, NULL);
    }

    switch (mode)
    {
    case BA_ST:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_ST, out);
        break;
    case BA_MOD:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_MOD, out);
        break;
    case BA_AMT:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_AMT, out);
        break;
    case BA_GR:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_GR, out);
        break;
    default:
        break;
    }

    generic_pattern *cexa = make_generic_pattern();
    if (!is_rtrivialgp_dfa_noprint(A, mode, cexa))
    {
        view_pattern(A, cexa, rtriv_pattern_print);
        free_generic_pattern(cexa);
        CONCLUSION_NEGATIVE(out);
        return false;
    }
    free_generic_pattern(cexa);
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_rtrivialgpplus_dfa_noprint(dfa *A, basis mode, generic_pattern *cexa)
{

    // Compute the SCCs of the DFA.
    dfa_compute_sccs(A);

    // We check each fold class to see if it contains a non-trivial common loop.
    // between two distinct states in the class.
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // If the fold class is trivial, it cannot yield a counterexample.
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }

        // We extract the SCC from the graph.
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        // We compute its folding
        parti *foldscc = mode == BA_ST ? NULL : dgraph_stal_fold(thescc, NULL, mode);
        // We compute the direct product of the SCC with itself.
        dgraph *product = dgraph_direct_product(thescc, thescc);
        // We compute the SCCs of the product graph (used to compute non-trivial loops).
        parti *sccp = dtarjan(product, NULL, false);
        // We no longer need the original SCC graph.
        delete_dgraph(thescc);

        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / A->sccs->cl_size[i];
            uint r = qr % A->sccs->cl_size[i];

            if (q != r && (!foldscc || foldscc->numcl[q] == foldscc->numcl[r]) && dgraph_ntrivial_loop(product, sccp, qr))
            {
                if (cexa)
                {
                    cexa->states[0] = A->sccs->cl_elems[i][q];
                    cexa->states[1] = A->sccs->cl_elems[i][r];
                    cexa->vars[0] = 'U';
                    cexa->vars[1] = 'V';
                    cexa->vars[2] = 'z';
                    cexa->types[0] = PAT_LANG;
                    cexa->types[1] = PAT_LANG;
                    cexa->types[2] = PAT_WORD;
                    dgraph_exists_path(product, qr, qr, true, &cexa->words[2]);
                }
                delete_parti(foldscc);
                delete_parti(sccp);
                delete_dgraph(product);
                return false;
            }
        }
        delete_parti(foldscc);
        delete_parti(sccp);
        delete_dgraph(product);
    }
    return true;
}

bool is_rtrivialgpplus_dfa(dfa *A, basis mode, FILE *out)
{
    if (!out)
    {
        return is_rtrivialgpplus_dfa_noprint(A, mode, NULL);
    }

    switch (mode)
    {
    case BA_ST:
        MAKE_PATTERN_INTRO_STRICT(RTRIV_PATTERN_ST_PLUS, out);
        break;
    case BA_MOD:
        MAKE_PATTERN_INTRO_STRICT(RTRIV_PATTERN_MOD_PLUS, out);
        break;
    case BA_AMT:
        MAKE_PATTERN_INTRO_STRICT(RTRIV_PATTERN_AMT_PLUS, out);
        break;
    case BA_GR:
        MAKE_PATTERN_INTRO_STRICT(RTRIV_PATTERN_GR_PLUS, out);
        break;
    default:
        break;
    }

    generic_pattern *cexa = make_generic_pattern();
    if (!is_rtrivialgpplus_dfa_noprint(A, mode, cexa))
    {
        view_pattern(A, cexa, rtriv_pattern_print);
        free_generic_pattern(cexa);
        CONCLUSION_NEGATIVE(out);
        return false;
    }
    free_generic_pattern(cexa);
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_rtrivialbpgp_dfa_noprint(dfa *A, basis mode, generic_pattern *cexa)
{
    // Compute the SCCs of the DFA.
    dfa_compute_sccs(A);

    if (mode == BA_ST)
    {
        // Array to store the alphabet of an SCC in the product graph
        bool alpha[A->trans->size_alpha];
        // For each non-trivial SCC
        for (uint i = 0; i < A->sccs->size_par; i++)
        {
            if (A->sccs->cl_size[i] <= 1)
            {
                continue;
            }
            // Extract the SCC and compute its product graph
            dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
            dgraph *product = dgraph_direct_product(thescc, thescc);
            parti *sccp = dtarjan(product, NULL, false);

            // For each SCC in the product graph
            for (uint j = 0; j < sccp->size_par; j++)
            {
                uint qr = sccp->cl_elems[j][0];
                uint q = qr / A->sccs->cl_size[i];
                uint r = qr % A->sccs->cl_size[i];
                if (q == r || !dgraph_compute_alph_scc(product, sccp, j, alpha))
                {
                    continue;
                }
                if (dgraph_exists_path_alpha(thescc, q, r, false, alpha, NULL) && dgraph_exists_path_alpha(thescc, r, q, false, alpha, NULL))
                {
                    if (cexa)
                    {
                        cexa->states[0] = A->sccs->cl_elems[i][q];
                        cexa->states[1] = A->sccs->cl_elems[i][r];
                        cexa->vars[0] = 'u';
                        cexa->vars[1] = 'v';
                        cexa->vars[2] = 'z';
                        cexa->types[0] = PAT_WORD;
                        cexa->types[1] = PAT_WORD;
                        cexa->types[2] = PAT_WORD;
                        dgraph_exists_path_alpha(thescc, q, r, false, alpha, &cexa->words[0]);
                        dgraph_exists_path_alpha(thescc, r, q, false, alpha, &cexa->words[1]);
                        dgraph_getloop_alpha(product, sccp, alpha, qr, &cexa->words[2]);
                    }
                    delete_dgraph(product);
                    delete_parti(sccp);
                    delete_dgraph(thescc);
                    return false;
                }
            }
            delete_dgraph(thescc);
            delete_dgraph(product);
            delete_parti(sccp);
        }
        return true;
    }

    // We now know that the basis is either MOD, AMT or GR.
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }
        // Extract the SCC from the graph.
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

        // COmpute the direct product of the SCC with itself.
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);

        // Compute the folding of the direct product according to the basis.
        parti *fold = dgraph_stal_fold(product, sccp, mode);
        dgraph *g_fold = dgraph_implement_fold(product, sccp, fold);

        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / A->sccs->cl_size[i];
            uint r = qr % A->sccs->cl_size[i];
            if (q >= r || !dgraph_ntrivial_loop(product, sccp, qr))
            {
                continue;
            }

            if (dgraph_exists_intersec_path(g_fold, thescc, fold->numcl[qr], q, fold->numcl[qr], r, false, NULL) && dgraph_exists_intersec_path(g_fold, thescc, fold->numcl[qr], r, fold->numcl[qr], q, false, NULL))
            {
                if (cexa)
                {
                    cexa->states[0] = A->sccs->cl_elems[i][q];
                    cexa->states[1] = A->sccs->cl_elems[i][r];
                    cexa->vars[0] = 'U';
                    cexa->vars[1] = 'V';
                    cexa->vars[2] = 'W';
                    cexa->types[0] = PAT_SING;
                    cexa->types[1] = PAT_SING;
                    cexa->types[2] = PAT_LANG;
                    dgraph_exists_intersec_path(g_fold, thescc, fold->numcl[qr], q, fold->numcl[qr], r, false, &cexa->words[0]);
                    dgraph_exists_intersec_path(g_fold, thescc, fold->numcl[qr], r, fold->numcl[qr], q, false, &cexa->words[1]);
                }
                delete_dgraph(thescc);
                delete_dgraph(product);
                delete_parti(sccp);
                delete_dgraph(g_fold);
                delete_parti(fold);
                return false;
            }
        }
        delete_dgraph(thescc);
        delete_dgraph(product);
        delete_parti(sccp);
        delete_dgraph(g_fold);
        delete_parti(fold);
    }
    return true;
}

bool is_rtrivialbpgp_dfa(dfa *A, basis mode, FILE *out)
{
    if (!out)
    {
        return is_rtrivialbpgp_dfa_noprint(A, mode, NULL);
    }

    switch (mode)
    {
    case BA_ST:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_BPST, out);
        break;
    case BA_MOD:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_BPMOD, out);
        break;
    case BA_AMT:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_BPAMT, out);
        break;
    case BA_GR:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_BPGR, out);
        break;
    default:
        break;
    }

    generic_pattern *cexa = make_generic_pattern();
    if (!is_rtrivialbpgp_dfa_noprint(A, mode, cexa))
    {
        view_pattern(A, cexa, rtriv_pattern_print);
        free_generic_pattern(cexa);
        CONCLUSION_NEGATIVE(out);
        return false;
    }
    free_generic_pattern(cexa);
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_rtrivialbpgpplus_dfa_noprint(dfa *A, basis mode, generic_pattern *cexa)
{
    // The SCCs of the automaton are computed.
    dfa_compute_sccs(A);

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }

        // We extract the SCC from the graph.
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        // We compute the direct product of the SCC with itself and its own SCCs (used to check synchronized self-loops).
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);
        uint *inv_sccp = parti_compute_inv(sccp);

        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / A->sccs->cl_size[i];
            uint r = qr % A->sccs->cl_size[i];
            if (q >= r || !dgraph_ntrivial_loop(product, sccp, qr))
            {
                continue;
            }
            // We now need to check the non-separability conditions between q,r and qr

            // So we extract the SCC of the product graph containing (q,r).
            dgraph *theloop = dgraph_extract(product, sccp, inv_sccp, j);

            dgraph *thelooptwo = dgraph_direct_product(theloop, theloop);
            dgraph *theloopprod = dgraph_direct_product(thescc, thelooptwo);
            parti *scc_loopprod = dtarjan(theloopprod, NULL, false);
            delete_dgraph(thelooptwo);

            parti *theloop_fold = mode == BA_ST ? NULL : dgraph_stal_fold(theloop, NULL, mode);

            // If we found a counter-example
            if (dgraph_exists_intersec_path_pgplus(thescc, theloop, q, 0, r, 0, false, theloopprod, scc_loopprod, theloop_fold) &&
                dgraph_exists_intersec_path_pgplus(thescc, theloop, r, 0, q, 0, false, theloopprod, scc_loopprod, theloop_fold))
            {
                if (cexa)
                {
                    cexa->states[0] = A->sccs->cl_elems[i][q];
                    cexa->states[1] = A->sccs->cl_elems[i][r];
                    cexa->vars[0] = 'U';
                    cexa->vars[1] = 'V';
                    cexa->vars[2] = 'W';
                    cexa->types[0] = PAT_LANG;
                    cexa->types[1] = PAT_LANG;
                    cexa->types[2] = PAT_LANG;
                }
                delete_dgraph(product);
                delete_dgraph(thescc);
                delete_parti(sccp);
                delete_dgraph(theloop);
                delete_dgraph(theloopprod);
                delete_parti(scc_loopprod);
                delete_parti(theloop_fold);
                free(inv_sccp);
                return false;
            }
            delete_dgraph(theloop);
            delete_dgraph(theloopprod);
            delete_parti(scc_loopprod);
            delete_parti(theloop_fold);
        }
        delete_dgraph(product);
        delete_dgraph(thescc);
        delete_parti(sccp);
        free(inv_sccp);
    }
    return true;
}

bool is_rtrivialbpgpplus_dfa(dfa *A, basis mode, FILE *out)
{
    if (!out)
    {
        return is_rtrivialbpgpplus_dfa_noprint(A, mode, NULL);
    }

    switch (mode)
    {
    case BA_ST:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_BPST_PLUS, out);
        break;
    case BA_MOD:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_BPMOD_PLUS, out);
        break;
    case BA_AMT:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_BPAMT_PLUS, out);
        break;
    case BA_GR:
        MAKE_PATTERN_INTRO(RTRIV_PATTERN_BPGR_PLUS, out);
        break;
    default:
        break;
    }

    generic_pattern *cexa = make_generic_pattern();
    if (!is_rtrivialbpgpplus_dfa_noprint(A, mode, cexa))
    {
        view_pattern(A, cexa, rtriv_pattern_print);
        free_generic_pattern(cexa);
        CONCLUSION_NEGATIVE(out);
        return false;
    }
    free_generic_pattern(cexa);
    CONCLUSION_POSITIVE(out);
    return true;
}

/****************/
/*+ L-trivial. +*/
/****************/

bool is_ltrivialgp_dfa(dfa *A, basis mode, FILE *out)
{

    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_ST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_MOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_AMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_GR, out);
            break;
        default:
            break;
        }
    }

    // Transitive closure of the DFA is computed.
    dfa_compute_tclos(A);
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);
        for (uint j = start; j < end; j++)
        {
            uint i1 = A->tclos->storage[j];
            dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);
            // printf("Checking SCC1 %u\n", i1);
            // view_dgraph(scc1);
            for (uint h = j; h < end; h++)
            {
                uint i2 = A->tclos->storage[h];
                dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i2);
                // printf("Checking SCC2 %u\n", i2);
                // view_dgraph(scc2);
                dgraph *inter = dgraph_direct_product(scc1, scc2);
                parti *scc_inter = dtarjan(inter, NULL, false);
                parti *fold_inter = mode == BA_ST ? scc_inter : dgraph_stal_fold(inter, scc_inter, mode);
                for (uint k = 0; k < fold_inter->size_par; k++)
                {
                    uint qr = fold_inter->cl_elems[k][0];
                    uint q = qr / scc2->size_graph;
                    uint r = qr % scc2->size_graph;
                    if (A->sccs->cl_elems[i1][q] == A->sccs->cl_elems[i2][r])
                    {
                        continue;
                    }
                    for (uint s = 0; s < fold_inter->cl_size[k]; s++)
                    {
                        uint x2x3 = fold_inter->cl_elems[k][s];
                        uint x2 = A->sccs->cl_elems[i1][x2x3 / scc2->size_graph];
                        uint x3 = A->sccs->cl_elems[i2][x2x3 % scc2->size_graph];
                        for (uint t = 0; t < fold_inter->cl_size[k]; t++)
                        {
                            uint x1x4 = fold_inter->cl_elems[k][t];
                            uint x1 = A->sccs->cl_elems[i1][x1x4 / scc2->size_graph];
                            uint x4 = A->sccs->cl_elems[i2][x1x4 % scc2->size_graph];
                            for (uint p = 0; p < A->sccs->cl_size[i]; p++)
                            {
                                uint x0 = A->sccs->cl_elems[i][p];

                                // printf("Checking states %u, %u, %u, %u, %u\n", x0, x1, x2, x3, x4);
                                if (dgraph_exists_intersec_path(A->trans, inter, x0, x2x3, x1, x1x4, false, NULL) &&
                                    dgraph_exists_intersec_path(A->trans, inter, x0, x1x4, x3, x2x3, false, NULL))
                                {
                                    if (out)
                                    {
                                        generic_pattern *mypattern = make_generic_pattern();
                                        mypattern->states[0] = x0;
                                        mypattern->states[1] = x1;
                                        mypattern->states[2] = x2;
                                        mypattern->states[3] = x3;
                                        mypattern->states[4] = x4;
                                        mypattern->vars[0] = mode == BA_ST ? 'u' : 'U';
                                        mypattern->vars[1] = mode == BA_ST ? 'v' : 'V';
                                        mypattern->types[0] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                                        mypattern->types[1] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                                        dgraph_exists_intersec_path(A->trans, inter, x0, x2x3, x1, x1x4, false, &mypattern->words[0]);
                                        dgraph_exists_intersec_path(A->trans, inter, x0, x1x4, x3, x2x3, false, &mypattern->words[1]);
                                        view_pattern(A, mypattern, ltriv_pattern_print);
                                        free_generic_pattern(mypattern);
                                        CONCLUSION_NEGATIVE(out);
                                    }
                                    delete_dgraph(inter);
                                    delete_parti(scc_inter);
                                    if (mode != BA_ST)
                                    {
                                        delete_parti(fold_inter);
                                    }
                                    delete_dgraph(scc1);
                                    delete_dgraph(scc2);
                                    return false;
                                }
                            }
                        }
                    }
                }
                delete_dgraph(inter);
                delete_parti(scc_inter);
                if (mode != BA_ST)
                {
                    delete_parti(fold_inter);
                }
                delete_dgraph(scc2);
            }
            delete_dgraph(scc1);
        }
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_ltrivialgp_opti_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_ST_OPTI, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_MOD_OPTI, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_AMT_OPTI, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_GR_OPTI, out);
            break;
        default:
            break;
        }
    }

    // The transitive closure and the sccs of the DFA
    dfa_compute_sccs(A);

    if (mode == BA_ST)
    {
        // Intersection of the DFA with itself and its SCCs
        dgraph *inter = dgraph_direct_product(A->trans, A->trans);
        parti *scci = dtarjan(inter, NULL, false);

        bool alpha[A->trans->size_alpha];
        for (uint i = 0; i < scci->size_par; i++)
        {
            uint qr = scci->cl_elems[i][0];
            uint q = qr / A->trans->size_graph;
            uint r = qr % A->trans->size_graph;

            // If q = r, this cannot yield a counterexample.
            // if q > r, the state (r,q) is in another scc (rq < qr in this case and the
            // lists of elements are sorted in increasing order). Hence, (q,r) is treated
            // by symmetry with this other scc.
            if (q >= r)
            {
                continue;
            }
            dgraph_compute_alph_scc(inter, scci, i, alpha);

            for (uint s = 0; s < A->trans->size_graph; s++)
            {
                if (A->sccs->numcl[q] < A->sccs->numcl[s] || A->sccs->numcl[r] < A->sccs->numcl[s])
                {
                    continue;
                }
                // Check if there is a path from q to r that does not pass through s
                uint *u = NULL;
                uint *v = NULL;
                if (dgraph_exists_path_alpha(A->trans, s, q, false, alpha, &u) &&
                    dgraph_exists_path_alpha(A->trans, s, r, false, alpha, &v))
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = s;
                        mypattern->states[1] = q;
                        mypattern->states[2] = r;
                        mypattern->vars[0] = 'u';
                        mypattern->vars[1] = 'v';
                        mypattern->vars[2] = 'w';
                        mypattern->types[0] = PAT_WORD;
                        mypattern->types[1] = PAT_WORD;
                        mypattern->types[2] = PAT_WORD;
                        mypattern->words[0] = u;
                        mypattern->words[1] = v;
                        dgraph_getloop_alpha(inter, scci, alpha, qr, &mypattern->words[2]);
                        view_pattern(A, mypattern, ltriv_opti_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    free(u);
                    free(v);
                    delete_dgraph(inter);
                    delete_parti(scci);
                    return false;
                }
                free(u);
                free(v);
            }
        }
        delete_dgraph(inter);
        delete_parti(scci);
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // Intersection of the DFA with itself and its SCCs
    dgraph *inter = dgraph_direct_product(A->trans, A->trans);
    parti *scci = dtarjan(inter, NULL, false);
    // Folding the SCCs according to the mode
    parti *fold = dgraph_stal_fold(inter, scci, mode);
    dgraph *g_fold = dgraph_implement_fold(inter, scci, fold);
    delete_dgraph(inter);
    delete_parti(scci);

    bool *done;
    CALLOC(done, fold->size_par);

    for (uint i = 0; i < fold->size_par; i++)
    {
        uint qr = fold->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;

        // If q = r, this cannot yield a counterexample.
        // if q > r, the state (r,q) is in another scc (rq < qr in this case and the
        // lists of elements are sorted in increasing order). Hence, (q,r) is treated
        // by symmetry with this other scc.
        if (q >= r)
        {
            continue;
        }

        for (uint s = 0; s < A->trans->size_graph; s++)
        {
            if (A->sccs->numcl[q] < A->sccs->numcl[s] || A->sccs->numcl[r] < A->sccs->numcl[s])
            {
                continue;
            }
            if (dgraph_exists_intersec_path(A->trans, g_fold, s, fold->numcl[qr], q, fold->numcl[qr], false, NULL) &&
                dgraph_exists_intersec_path(A->trans, g_fold, s, fold->numcl[qr], r, fold->numcl[qr], false, NULL))
            {
                if (out)
                {
                    generic_pattern *mypattern = make_generic_pattern();
                    mypattern->states[0] = s;
                    mypattern->states[1] = q;
                    mypattern->states[2] = r;
                    mypattern->vars[0] = 'U';
                    mypattern->vars[1] = 'V';
                    mypattern->vars[2] = 'W';
                    mypattern->types[0] = PAT_SING;
                    mypattern->types[1] = PAT_SING;
                    mypattern->types[2] = PAT_LANG;
                    dgraph_exists_intersec_path(A->trans, g_fold, s, fold->numcl[qr], q, fold->numcl[qr], false, &mypattern->words[0]);
                    dgraph_exists_intersec_path(A->trans, g_fold, s, fold->numcl[qr], r, fold->numcl[qr], false, &mypattern->words[1]);
                    view_pattern(A, mypattern, ltriv_opti_pattern_print);
                    free_generic_pattern(mypattern);
                    CONCLUSION_NEGATIVE(out);
                }
                delete_parti(fold);
                delete_dgraph(g_fold);
                free(done);
                return false;
            }
        }
    }

    delete_parti(fold);
    delete_dgraph(g_fold);
    free(done);
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_ltrivialgp_mirror_dfa(dfa *A, basis mode)
{
    dfa *mirror = dfa_determinize_mirror(A, false);
    bool res = is_rtrivialgp_dfa_noprint(mirror, mode, NULL);
    dfa_delete(mirror);
    return res;
}

bool is_ltrivialgp_rt_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_ST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_MOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_AMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_GR, out);
            break;
        default:
            break;
        }
    }
    // We compute the transitive closure and the SCCs of the DFA
    dfa_compute_tclos(A);

    // For each SCC of the DFA
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // We extract the SCC
        dgraph *thescc0 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // We loop over the reachable SCCs
        for (uint j = start; j < end; j++)
        {
            int i1 = A->tclos->storage[j];
            dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);
            for (uint h = j; h < end; h++)
            {
                int i2 = A->tclos->storage[h];
                dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i2);
                dgraph *product = dgraph_direct_product(thescc1, thescc2);
                parti *sccp = dtarjan(product, NULL, false);
                uint *sccp_inv = parti_compute_inv(sccp);
                for (uint k = 0; k < sccp->size_par; k++)
                {
                    uint q1 = sccp->cl_elems[k][0] / thescc2->size_graph;
                    uint q2 = sccp->cl_elems[k][0] % thescc2->size_graph;
                    uint q1o = A->sccs->cl_elems[i1][q1];
                    uint q2o = A->sccs->cl_elems[i2][q2];
                    if (q1o == q2o || !dgraph_ntrivial_scc(product, sccp, k))
                    {
                        continue;
                    }
                    dgraph *local = dgraph_extract(product, sccp, sccp_inv, k);
                    for (uint x0 = 0; x0 < thescc0->size_graph; x0++)
                    {
                        uint x0o = A->sccs->cl_elems[i][x0];
                        for (uint x1x2 = 0; x1x2 < local->size_graph; x1x2++)
                        {
                            uint x1 = sccp->cl_elems[k][x1x2] / thescc2->size_graph;
                            uint x2 = sccp->cl_elems[k][x1x2] % thescc2->size_graph;
                            uint x1o = A->sccs->cl_elems[i1][x1];
                            uint x2o = A->sccs->cl_elems[i2][x2];
                            if (dgraph_exists_intersec_path(A->trans, local, x0o, x1x2, x1o, x1x2, false, NULL) &&
                                dgraph_exists_intersec_path(A->trans, local, x0o, x1x2, x2o, x1x2, false, NULL))
                            {
                                if (out)
                                {
                                    generic_pattern *mypattern = make_generic_pattern();
                                    mypattern->states[0] = x0o;
                                    mypattern->states[1] = x1o;
                                    mypattern->states[2] = x1o;
                                    mypattern->states[3] = x2o;
                                    mypattern->states[4] = x2o;
                                    if (mode == BA_ST)
                                    {
                                        mypattern->vars[0] = 'u';
                                        mypattern->vars[1] = 'v';
                                        mypattern->types[0] = PAT_WORD;
                                        mypattern->types[1] = PAT_WORD;
                                        dgraph_exists_intersec_path(A->trans, local, x0o, x1x2, x1o, x1x2, false, &mypattern->words[0]);
                                        dgraph_exists_intersec_path(A->trans, local, x0o, x1x2, x2o, x1x2, false, &mypattern->words[1]);
                                    }
                                    else
                                    {
                                        mypattern->vars[0] = 'U';
                                        mypattern->vars[1] = 'V';
                                        mypattern->types[0] = PAT_LANG;
                                        mypattern->types[1] = PAT_LANG;
                                    }
                                    view_pattern(A, mypattern, ltriv_pattern_print);
                                    free_generic_pattern(mypattern);
                                }
                                delete_dgraph(local);
                                delete_dgraph(product);
                                delete_parti(sccp);
                                delete_dgraph(thescc0);
                                delete_dgraph(thescc1);
                                delete_dgraph(thescc2);
                                CONCLUSION_NEGATIVE(out);
                                return false;
                            }
                        }
                    }

                    delete_dgraph(local);
                }
                delete_dgraph(thescc2);
                delete_dgraph(product);
                delete_parti(sccp);
            }
            delete_dgraph(thescc1);
        }
        delete_dgraph(thescc0);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_ltrivialgpplus_dfa(dfa *A, basis mode, FILE *out)
{

    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO_STRICT(LTRIV_PATTERN_ST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO_STRICT(LTRIV_PATTERN_MOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO_STRICT(LTRIV_PATTERN_AMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO_STRICT(LTRIV_PATTERN_GR_PLUS, out);
            break;
        default:
            break;
        }
    }

    // SCCs and transitive closure of the DFA
    dfa_compute_tclos(A);

    // For each starting SCC
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
        {
            continue;
        }
        dgraph *scc0 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // We pick two reachable SCCs (not necessarily distinct)
        for (uint j = start; j < end; j++)
        {
            uint i1 = A->tclos->storage[j];
            if (!dgraph_ntrivial_scc(A->trans, A->sccs, i1))
            {
                continue;
            }
            dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);
            // printf("Checking SCC1 %u\n", i1);
            // view_dgraph(scc1);
            for (uint h = j; h < end; h++)
            {
                uint i2 = A->tclos->storage[h];
                if (!dgraph_ntrivial_scc(A->trans, A->sccs, i2))
                {
                    continue;
                }
                dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i2);
                // printf("Checking SCC2 %u\n", i2);
                // view_dgraph(scc2);

                // Product of the two reachable SCCs
                dgraph *inter = dgraph_direct_product(scc1, scc2);
                parti *scc_inter = dtarjan(inter, NULL, false);
                parti *fold_inter = mode == BA_ST ? NULL : dgraph_stal_fold(inter, scc_inter, mode);
                uint *inv_scc_inter = parti_compute_inv(scc_inter);
                for (uint k = 0; k < scc_inter->size_par; k++)
                {
                    uint qr = scc_inter->cl_elems[k][0];
                    uint q = qr / scc2->size_graph;
                    uint r = qr % scc2->size_graph;
                    if (A->sccs->cl_elems[i1][q] == A->sccs->cl_elems[i2][r] || !dgraph_ntrivial_scc(inter, scc_inter, k))
                    {
                        continue;
                    }

                    // Extaction of the SCC of the product graph containing (q,r)
                    dgraph *thesccp = dgraph_extract(inter, scc_inter, inv_scc_inter, k);

                    // Product of this SCC with itself and its own SCCs (used to check synchronized self-loops).
                    dgraph *dblprod = dgraph_direct_product(thesccp, thesccp);
                    parti *sccdblprod = dtarjan(dblprod, NULL, false);
                    uint *inv_sccdblprod = parti_compute_inv(sccdblprod);
                    for (uint p = 0; p < sccdblprod->size_par; p++)
                    {
                        if (!dgraph_ntrivial_scc(dblprod, sccdblprod, p))
                        {
                            continue;
                        }
                        // Extraction and product with the starting SCC to check synchronized self-loops.
                        dgraph *thesccdblprod = dgraph_extract(dblprod, sccdblprod, inv_sccdblprod, p);
                        dgraph *ultiprod = dgraph_direct_product(scc0, thesccdblprod);
                        parti *sccultiprod = dtarjan(ultiprod, NULL, false);
                        for (uint m = 0; m < sccultiprod->size_par; m++)
                        {
                            if (!dgraph_ntrivial_scc(ultiprod, sccultiprod, m))
                            {
                                continue;
                            }
                            //
                            uint x0 = A->sccs->cl_elems[i][sccultiprod->cl_elems[m][0] / thesccdblprod->size_graph];
                            uint quad1 = sccultiprod->cl_elems[m][0] % thesccdblprod->size_graph;
                            uint quad2 = sccdblprod->cl_elems[p][quad1];
                            uint x2x3 = scc_inter->cl_elems[k][quad2 / thesccp->size_graph];
                            uint x1x4 = scc_inter->cl_elems[k][quad2 % thesccp->size_graph];
                            uint x2 = A->sccs->cl_elems[i1][x2x3 / scc2->size_graph];
                            uint x3 = A->sccs->cl_elems[i2][x2x3 % scc2->size_graph];
                            uint x1 = A->sccs->cl_elems[i1][x1x4 / scc2->size_graph];
                            uint x4 = A->sccs->cl_elems[i2][x1x4 % scc2->size_graph];
                            if ((!fold_inter || fold_inter->numcl[x1x4] == fold_inter->numcl[x2x3]) &&
                                dgraph_exists_intersec_path(A->trans, inter, x0, x2x3, x1, x1x4, false, NULL) &&
                                dgraph_exists_intersec_path(A->trans, inter, x0, x1x4, x3, x2x3, false, NULL))
                            {
                                if (out)
                                {
                                    generic_pattern *mypattern = make_generic_pattern();
                                    mypattern->states[0] = x0;
                                    mypattern->states[1] = x1;
                                    mypattern->states[2] = x2;
                                    mypattern->states[3] = x3;
                                    mypattern->states[4] = x4;
                                    mypattern->vars[0] = mode == BA_ST ? 'u' : 'U';
                                    mypattern->vars[1] = mode == BA_ST ? 'v' : 'V';
                                    mypattern->vars[2] = 'z';
                                    mypattern->types[0] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                                    mypattern->types[1] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                                    mypattern->types[2] = PAT_WORD;
                                    dgraph_exists_intersec_path(A->trans, inter, x0, x2x3, x1, x1x4, true, &mypattern->words[0]);
                                    dgraph_exists_intersec_path(A->trans, inter, x0, x1x4, x3, x2x3, true, &mypattern->words[1]);
                                    dgraph_exists_path(ultiprod, sccultiprod->cl_elems[m][0], sccultiprod->cl_elems[m][0], true, &mypattern->words[2]);
                                    view_pattern(A, mypattern, ltriv_pattern_print);
                                    free_generic_pattern(mypattern);
                                    CONCLUSION_NEGATIVE(out);
                                }
                                delete_dgraph(inter);
                                delete_parti(scc_inter);
                                delete_parti(fold_inter);
                                free(inv_scc_inter);
                                delete_dgraph(scc0);
                                delete_dgraph(scc1);
                                delete_dgraph(scc2);
                                delete_dgraph(thesccp);
                                delete_dgraph(dblprod);
                                delete_parti(sccdblprod);
                                free(inv_sccdblprod);
                                delete_dgraph(thesccdblprod);
                                delete_dgraph(ultiprod);
                                delete_parti(sccultiprod);
                                return false;
                            }
                        }
                        delete_dgraph(thesccdblprod);
                        delete_dgraph(ultiprod);
                        delete_parti(sccultiprod);
                    }
                    delete_dgraph(thesccp);
                    delete_dgraph(dblprod);
                    delete_parti(sccdblprod);
                    free(inv_sccdblprod);
                }
                delete_dgraph(inter);
                delete_parti(scc_inter);
                delete_parti(fold_inter);
                free(inv_scc_inter);
                delete_dgraph(scc2);
            }
            delete_dgraph(scc1);
        }
        delete_dgraph(scc0);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_ltrivialgpplus_mirror_dfa(dfa *A, basis mode)
{
    dfa *mirror = dfa_determinize_mirror(A, false);
    bool res = is_rtrivialgpplus_dfa_noprint(mirror, mode, NULL);
    dfa_delete(mirror);
    return res;
}

bool is_ltrivialbpgp_dfa(dfa *A, basis mode, FILE *out)
{

    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_BPST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_BPMOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_BPAMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_BPGR, out);
            break;
        default:
            break;
        }
    }

    // SCCs and transitive closure of the DFA
    dfa_compute_tclos(A);

    if (mode == BA_ST)
    {
        bool alpha[A->trans->size_alpha];
        // For each starting SCC
        for (uint i = 0; i < A->sccs->size_par; i++)
        {
            if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
            {
                continue;
            }
            dgraph *scc0 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
            uint start = GSTART(A->tclos, i);
            uint end = GEND(A->tclos, i);

            // We pick two reachable SCCs (not necessarily distinct)
            for (uint j = start; j < end; j++)
            {
                uint i1 = A->tclos->storage[j];
                if (!dgraph_ntrivial_scc(A->trans, A->sccs, i1))
                {
                    continue;
                }
                dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);
                // printf("Checking SCC1 %u\n", i1);
                // view_dgraph(scc1);
                for (uint h = j; h < end; h++)
                {
                    uint i2 = A->tclos->storage[h];
                    if (!dgraph_ntrivial_scc(A->trans, A->sccs, i2))
                    {
                        continue;
                    }
                    dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i2);
                    // printf("Checking SCC2 %u\n", i2);
                    // view_dgraph(scc2);

                    // Product of the two reachable SCCs
                    dgraph *inter = dgraph_direct_product(scc1, scc2);
                    parti *scc_inter = dtarjan(inter, NULL, false);
                    uint *inv_scc_inter = parti_compute_inv(scc_inter);
                    for (uint k = 0; k < scc_inter->size_par; k++)
                    {
                        uint qr = scc_inter->cl_elems[k][0];
                        uint q = qr / scc2->size_graph;
                        uint r = qr % scc2->size_graph;
                        if (A->sccs->cl_elems[i1][q] == A->sccs->cl_elems[i2][r] || !dgraph_ntrivial_scc(inter, scc_inter, k))
                        {
                            continue;
                        }

                        // Extaction of the SCC of the product graph containing (q,r)
                        dgraph *thesccp = dgraph_extract(inter, scc_inter, inv_scc_inter, k);

                        // Product of this SCC with itself and its own SCCs (used to check synchronized self-loops).
                        dgraph *dblprod = dgraph_direct_product(thesccp, thesccp);
                        parti *sccdblprod = dtarjan(dblprod, NULL, false);
                        uint *inv_sccdblprod = parti_compute_inv(sccdblprod);
                        for (uint p = 0; p < sccdblprod->size_par; p++)
                        {
                            if (!dgraph_ntrivial_scc(dblprod, sccdblprod, p))
                            {
                                continue;
                            }
                            // Extraction and product with the starting SCC to check synchronized self-loops.
                            dgraph *thesccdblprod = dgraph_extract(dblprod, sccdblprod, inv_sccdblprod, p);
                            dgraph *ultiprod = dgraph_direct_product(scc0, thesccdblprod);
                            parti *sccultiprod = dtarjan(ultiprod, NULL, false);
                            for (uint m = 0; m < sccultiprod->size_par; m++)
                            {
                                if (!dgraph_compute_alph_scc(ultiprod, sccultiprod, m, alpha))
                                {
                                    continue;
                                }
                                //
                                uint x0 = A->sccs->cl_elems[i][sccultiprod->cl_elems[m][0] / thesccdblprod->size_graph];
                                uint quad1 = sccultiprod->cl_elems[m][0] % thesccdblprod->size_graph;
                                uint quad2 = sccdblprod->cl_elems[p][quad1];
                                uint x2x3 = scc_inter->cl_elems[k][quad2 / thesccp->size_graph];
                                uint x1x4 = scc_inter->cl_elems[k][quad2 % thesccp->size_graph];
                                uint x2 = A->sccs->cl_elems[i1][x2x3 / scc2->size_graph];
                                uint x3 = A->sccs->cl_elems[i2][x2x3 % scc2->size_graph];
                                uint x1 = A->sccs->cl_elems[i1][x1x4 / scc2->size_graph];
                                uint x4 = A->sccs->cl_elems[i2][x1x4 % scc2->size_graph];
                                if (dgraph_exists_intersec_path_alpha(A->trans, inter, x0, x2x3, x1, x1x4, true, alpha, NULL) &&
                                    dgraph_exists_intersec_path_alpha(A->trans, inter, x0, x1x4, x3, x2x3, true, alpha, NULL))
                                {
                                    if (out)
                                    {
                                        generic_pattern *mypattern = make_generic_pattern();
                                        mypattern->states[0] = x0;
                                        mypattern->states[1] = x1;
                                        mypattern->states[2] = x2;
                                        mypattern->states[3] = x3;
                                        mypattern->states[4] = x4;
                                        mypattern->vars[0] = 'u';
                                        mypattern->vars[1] = 'v';
                                        mypattern->vars[2] = 'w';
                                        mypattern->types[0] = PAT_WORD;
                                        mypattern->types[1] = PAT_WORD;
                                        mypattern->types[2] = PAT_WORD;
                                        dgraph_exists_intersec_path_alpha(A->trans, inter, x0, x2x3, x1, x1x4, true, alpha, &mypattern->words[0]);
                                        dgraph_exists_intersec_path_alpha(A->trans, inter, x0, x1x4, x3, x2x3, true, alpha, &mypattern->words[1]);
                                        dgraph_getloop_alpha(ultiprod, sccultiprod, alpha, sccultiprod->cl_elems[m][0], &mypattern->words[2]);
                                        view_pattern(A, mypattern, ltriv_pattern_print);
                                        free_generic_pattern(mypattern);
                                        CONCLUSION_NEGATIVE(out);
                                    }
                                    delete_dgraph(inter);
                                    delete_parti(scc_inter);
                                    free(inv_scc_inter);
                                    delete_dgraph(scc0);
                                    delete_dgraph(scc1);
                                    delete_dgraph(scc2);
                                    delete_dgraph(thesccp);
                                    delete_dgraph(dblprod);
                                    delete_parti(sccdblprod);
                                    free(inv_sccdblprod);
                                    delete_dgraph(thesccdblprod);
                                    delete_dgraph(ultiprod);
                                    delete_parti(sccultiprod);
                                    return false;
                                }
                            }
                            delete_dgraph(thesccdblprod);
                            delete_dgraph(ultiprod);
                            delete_parti(sccultiprod);
                        }
                        delete_dgraph(thesccp);
                        delete_dgraph(dblprod);
                        delete_parti(sccdblprod);
                        free(inv_sccdblprod);
                    }
                    delete_dgraph(inter);
                    delete_parti(scc_inter);
                    free(inv_scc_inter);
                    delete_dgraph(scc2);
                }
                delete_dgraph(scc1);
            }
            delete_dgraph(scc0);
        }
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // For each starting SCC
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
        {
            continue;
        }
        dgraph *scc0 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // We pick two reachable SCCs (not necessarily distinct)
        for (uint j = start; j < end; j++)
        {
            uint i1 = A->tclos->storage[j];
            if (!dgraph_ntrivial_scc(A->trans, A->sccs, i1))
            {
                continue;
            }
            dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);
            // printf("Checking SCC1 %u\n", i1);
            // view_dgraph(scc1);
            for (uint h = j; h < end; h++)
            {
                uint i2 = A->tclos->storage[h];
                if (!dgraph_ntrivial_scc(A->trans, A->sccs, i2))
                {
                    continue;
                }
                dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i2);
                // printf("Checking SCC2 %u\n", i2);
                // view_dgraph(scc2);

                // Product of the two reachable SCCs
                dgraph *inter = dgraph_direct_product(scc1, scc2);
                parti *scc_inter = dtarjan(inter, NULL, false);
                uint *inv_scc_inter = parti_compute_inv(scc_inter);
                for (uint k = 0; k < scc_inter->size_par; k++)
                {
                    uint qr = scc_inter->cl_elems[k][0];
                    uint q = qr / scc2->size_graph;
                    uint r = qr % scc2->size_graph;
                    if (A->sccs->cl_elems[i1][q] == A->sccs->cl_elems[i2][r] || !dgraph_ntrivial_scc(inter, scc_inter, k))
                    {
                        continue;
                    }

                    // Extaction of the SCC of the product graph containing (q,r)
                    dgraph *thesccp = dgraph_extract(inter, scc_inter, inv_scc_inter, k);

                    // Product of this SCC with itself and its own SCCs (used to check synchronized self-loops).
                    dgraph *dblprod = dgraph_direct_product(thesccp, thesccp);
                    parti *sccdblprod = dtarjan(dblprod, NULL, false);
                    uint *inv_sccdblprod = parti_compute_inv(sccdblprod);
                    for (uint p = 0; p < sccdblprod->size_par; p++)
                    {
                        if (!dgraph_ntrivial_scc(dblprod, sccdblprod, p))
                        {
                            continue;
                        }
                        // Extraction and product with the starting SCC to check synchronized self-loops.
                        dgraph *thesccdblprod = dgraph_extract(dblprod, sccdblprod, inv_sccdblprod, p);
                        dgraph *ultiprod = dgraph_direct_product(scc0, thesccdblprod);
                        parti *sccultiprod = dtarjan(ultiprod, NULL, false);
                        uint *inv_sccultiprod = parti_compute_inv(sccultiprod);
                        for (uint m = 0; m < sccultiprod->size_par; m++)
                        {
                            if (!dgraph_ntrivial_scc(ultiprod, sccultiprod, m))
                            {
                                continue;
                            }
                            dgraph *myloop = dgraph_extract(ultiprod, sccultiprod, inv_sccultiprod, m);
                            parti *myloop_fold = dgraph_stal_fold(myloop, NULL, mode);
                            dgraph *myloop_fold_red = dgraph_implement_fold(myloop, NULL, myloop_fold);
                            delete_dgraph(myloop);
                            delete_parti(myloop_fold);
                            dgraph *check_prod = dgraph_direct_product(myloop_fold_red, inter);
                            delete_dgraph(myloop_fold_red);
                            //
                            uint x0 = A->sccs->cl_elems[i][sccultiprod->cl_elems[m][0] / thesccdblprod->size_graph];
                            uint quad1 = sccultiprod->cl_elems[m][0] % thesccdblprod->size_graph;
                            uint quad2 = sccdblprod->cl_elems[p][quad1];
                            uint x2x3 = scc_inter->cl_elems[k][quad2 / thesccp->size_graph];
                            uint x1x4 = scc_inter->cl_elems[k][quad2 % thesccp->size_graph];
                            uint x2 = A->sccs->cl_elems[i1][x2x3 / scc2->size_graph];
                            uint x3 = A->sccs->cl_elems[i2][x2x3 % scc2->size_graph];
                            uint x1 = A->sccs->cl_elems[i1][x1x4 / scc2->size_graph];
                            uint x4 = A->sccs->cl_elems[i2][x1x4 % scc2->size_graph];

                            // printf("Checking states x0=%u, x1=%u, x2=%u, x3=%u, x4=%u\n", x0, x1, x2, x3, x4);
                            // view_dgraph(check_prod);

                            if (dgraph_exists_intersec_path(A->trans, check_prod, x0, x2x3, x1, x1x4, false, NULL) &&
                                dgraph_exists_intersec_path(A->trans, check_prod, x0, x1x4, x3, x2x3, false, NULL))
                            {
                                if (out)
                                {
                                    generic_pattern *mypattern = make_generic_pattern();
                                    mypattern->states[0] = x0;
                                    mypattern->states[1] = x1;
                                    mypattern->states[2] = x2;
                                    mypattern->states[3] = x3;
                                    mypattern->states[4] = x4;
                                    mypattern->vars[0] = 'U';
                                    mypattern->vars[1] = 'V';
                                    mypattern->vars[2] = 'W';
                                    mypattern->types[0] = PAT_SING;
                                    mypattern->types[1] = PAT_SING;
                                    mypattern->types[2] = PAT_LANG;
                                    dgraph_exists_intersec_path(A->trans, check_prod, x0, x2x3, x1, x1x4, false, &mypattern->words[0]);
                                    dgraph_exists_intersec_path(A->trans, check_prod, x0, x1x4, x3, x2x3, false, &mypattern->words[1]);
                                    view_pattern(A, mypattern, ltriv_pattern_print);
                                    free_generic_pattern(mypattern);
                                    CONCLUSION_NEGATIVE(out);
                                }
                                delete_dgraph(inter);
                                delete_parti(scc_inter);
                                free(inv_scc_inter);
                                delete_dgraph(scc0);
                                delete_dgraph(scc1);
                                delete_dgraph(scc2);
                                delete_dgraph(thesccp);
                                delete_dgraph(dblprod);
                                delete_parti(sccdblprod);
                                free(inv_sccdblprod);
                                delete_dgraph(thesccdblprod);
                                delete_dgraph(ultiprod);
                                delete_parti(sccultiprod);
                                delete_dgraph(check_prod);
                                free(inv_sccultiprod);
                                return false;
                            }
                            delete_dgraph(check_prod);
                        }
                        delete_dgraph(thesccdblprod);
                        delete_dgraph(ultiprod);
                        delete_parti(sccultiprod);
                        free(inv_sccultiprod);
                    }
                    delete_dgraph(thesccp);
                    delete_dgraph(dblprod);
                    delete_parti(sccdblprod);
                    free(inv_sccdblprod);
                }
                delete_dgraph(inter);
                delete_parti(scc_inter);
                free(inv_scc_inter);
                delete_dgraph(scc2);
            }
            delete_dgraph(scc1);
        }
        delete_dgraph(scc0);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_ltrivialbpgp_mirror_dfa(dfa *A, basis mode)
{
    dfa *mirror = dfa_determinize_mirror(A, false);
    bool res = is_rtrivialbpgp_dfa_noprint(mirror, mode, NULL);
    dfa_delete(mirror);
    return res;
}

bool is_ltrivialbpgpplus_dfa(dfa *A, basis mode, int *error, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_BPST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_BPMOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_BPAMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(LTRIV_PATTERN_BPGR_PLUS, out);
            break;
        default:
            break;
        }
    }

    uint *funs = NULL;
    morphism *M = dfa_to_morphism(A, error, &funs);
    orbits *bpolorbs = compute_bpgplusorbits(M, LV_GREG, mode);
    uint c[2];

    for (uint i = 0; i < bpolorbs->nb_computed; i++)
    {
        subsemi *S = bpolorbs->orbits[i];
        if (is_gtrivial_subsemi(S, L_GREEN, c))
        {
            continue;
        }
        if (out)
        {
            uint e = c[0];
            uint s = c[1];
            uint esub = S->mono_to_sub[e];
            uint r = ONE;
            for (uint j = 0; j < S->rels->RCL->cl_size[S->rels->RCL->numcl[esub]]; j++)
            {
                uint rsub = S->rels->RCL->cl_elems[S->rels->RCL->numcl[esub]][j];
                r = S->sub_to_mono[rsub];
                if (mor_mult(M, r, s) == e)
                {
                    break;
                }
            }

            uint q0 = ONE;
            for (uint qi = 0; qi < A->trans->size_graph; qi++)
            {
                if (funs[A->trans->size_graph * e + qi] != funs[A->trans->size_graph * s + qi])
                {
                    q0 = qi;
                    break;
                }
            }
            uint q2 = funs[A->trans->size_graph * e + q0];
            uint q1 = funs[A->trans->size_graph * r + q2];
            uint q3 = funs[A->trans->size_graph * s + q0];
            uint q4 = funs[A->trans->size_graph * r + q3];

            generic_pattern *mypattern = make_generic_pattern();
            mypattern->states[0] = q0;
            mypattern->states[1] = q1;
            mypattern->states[2] = q2;
            mypattern->states[3] = q3;
            mypattern->states[4] = q4;
            mypattern->vars[0] = 'U';
            mypattern->vars[1] = 'V';
            mypattern->vars[2] = 'W';
            mypattern->types[0] = PAT_LANG;
            mypattern->types[1] = PAT_LANG;
            mypattern->types[2] = PAT_LANG;
            view_pattern(A, mypattern, ltriv_pattern_print);
            free_generic_pattern(mypattern);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        CONCLUSION_NEGATIVE(out);
        return false;
    }
    delete_morphism(M);
    free(funs);
    delete_orbits(bpolorbs);
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_ltrivialbpgpplus_mirror_dfa(dfa *A, basis mode)
{
    dfa *mirror = dfa_determinize_mirror(A, false);
    bool res = is_rtrivialbpgpplus_dfa_noprint(mirror, mode, NULL);
    dfa_delete(mirror);
    return res;
}

/*******/
/* Pol */
/*******/

static bool is_ppt_dfa(dfa *A, bool copol, FILE *out)
{
    MAKE_PATTERN_INTRO(copol ? COPOL_PATTERN_ST : POL_PATTERN_ST, out);

    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgp_dfa_noprint(A, BA_ST, cexa))
        {
            generic_pattern *mypattern = make_generic_pattern();
            dfa_mini_canonical_ordering(A);
            if (A->order[copol ? cexa->states[1] : cexa->states[0]][copol ? cexa->states[0] : cexa->states[1]])
            {
                mypattern->states[0] = cexa->states[1];
                mypattern->states[1] = cexa->states[0];
                mypattern->vars[0] = 'u';
                mypattern->types[0] = PAT_WORD;
                mypattern->words[0] = copy_word(cexa->words[1]);
            }
            else
            {
                mypattern->states[0] = cexa->states[0];
                mypattern->states[1] = cexa->states[1];
                mypattern->vars[0] = 'u';
                mypattern->types[0] = PAT_WORD;
                mypattern->words[0] = copy_word(cexa->words[0]);
            }
            view_pattern(A, mypattern, pol_pattern_print);
            free_generic_pattern(mypattern);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgp_dfa_noprint(A, BA_ST, NULL))
        {
            return false;
        }
    }

    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            // If the pattern equation is satisfied we move to the next transition
            if (A->order[copol ? A->trans->edges[q][a] : q][copol ? q : A->trans->edges[q][a]])
            {
                continue;
            }

            // If the pattern equation is not satisfied, we print a counterexample if requested
            if (out)
            {
                generic_pattern *mypattern = make_generic_pattern();
                mypattern->states[0] = q;
                mypattern->states[1] = A->trans->edges[q][a];
                mypattern->vars[0] = 'u';
                mypattern->types[0] = PAT_WORD;
                mypattern->words[0] = make_singleton_word(a);
                view_pattern(A, mypattern, pol_pattern_print);
                free_generic_pattern(mypattern);
                CONCLUSION_NEGATIVE(out);
            }
            return false;
        }
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

static bool is_polgrgen_dfa(dfa *A, bool grp, bool copol, FILE *out)
{
    // We first check the pattern inside the sccs of the DFA
    if (grp)
    {
        MAKE_PATTERN_INTRO(copol ? COPOL_PATTERN_GR : POL_PATTERN_GR, out);
    }
    else
    {
        MAKE_PATTERN_INTRO(copol ? COPOL_PATTERN_MOD : POL_PATTERN_MOD, out);
    }

    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgp_dfa_noprint(A, grp ? BA_GR : BA_MOD, cexa))
        {
            generic_pattern *mypattern = make_generic_pattern();
            dfa_mini_canonical_ordering(A);
            if (A->order[copol ? cexa->states[1] : cexa->states[0]][copol ? cexa->states[0] : cexa->states[1]])
            {
                mypattern->states[0] = cexa->states[1];
                mypattern->states[1] = cexa->states[0];
                mypattern->vars[0] = 'U';
                mypattern->types[0] = PAT_LANG;
            }
            else
            {
                mypattern->states[0] = cexa->states[0];
                mypattern->states[1] = cexa->states[1];
                mypattern->vars[0] = 'U';
                mypattern->types[0] = PAT_LANG;
            }
            view_pattern(A, mypattern, pol_pattern_print);
            free_generic_pattern(mypattern);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgp_dfa_noprint(A, grp ? BA_GR : BA_MOD, NULL))
        {
            return false;
        }
    }

    // We now check pairs of distinct sccs
    dfa_compute_sccs(A);
    parti *fold = dgraph_stal_fold(A->trans, A->sccs, grp ? BA_GR : BA_MOD);
    bool *EXT = dgraph_dyck_ext_eps(A->trans, grp, A->sccs, fold);

    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);

    // For each state q
    for (uint i = 0; i < fold->size_par; i++)
    {
        for (uint j = 0; j < fold->size_par; j++)
        {
            if (i == j || !EXT[i * fold->size_par + j])
            {
                continue;
            }
            uint q = fold->cl_elems[i][0];
            uint r = fold->cl_elems[j][0];
            if (!A->order[copol ? r : q][copol ? q : r])
            {
                if (out)
                {
                    generic_pattern *mypattern = make_generic_pattern();
                    mypattern->states[0] = q;
                    mypattern->states[1] = r;
                    mypattern->vars[0] = 'U';
                    mypattern->types[0] = PAT_LANG;
                    view_pattern(A, mypattern, pol_pattern_print);
                    free_generic_pattern(mypattern);
                    CONCLUSION_NEGATIVE(out);
                }
                delete_parti(fold);
                free(EXT);
                return false;
            }
        }
    }
    delete_parti(fold);
    free(EXT);
    CONCLUSION_POSITIVE(out);
    return true;
}

static bool is_polgr_dfa(dfa *A, bool copol, FILE *out)
{
    MAKE_PATTERN_INTRO(copol ? COPOL_PATTERN_GR_SPE : POL_PATTERN_GR_SPE, out);
    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint r = 0; r < A->trans->size_graph; r++)
        {
            if (A->order[copol ? r : q][copol ? q : r])
            {
                continue;
            }

            if (dgraph_exists_intersec_path(A->trans, A->trans, q, r, r, r, false, NULL))
            {
                if (out)
                {
                    generic_pattern *mypattern = make_generic_pattern();
                    mypattern->states[0] = q;
                    mypattern->states[1] = r;
                    mypattern->vars[0] = 'u';
                    mypattern->types[0] = PAT_WORD;
                    dgraph_exists_intersec_path(A->trans, A->trans, q, r, r, r, false, &mypattern->words[0]);
                    view_pattern(A, mypattern, polgr_pattern_print);
                    free_generic_pattern(mypattern);
                    CONCLUSION_NEGATIVE(out);
                }
                return false;
            }
        }
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_polgp_dfa(dfa *A, bool copol, basis mode, FILE *out)
{

    switch (mode)
    {
    case BA_ST:
        return is_ppt_dfa(A, copol, out);
        break;
    case BA_MOD:
        return is_polgrgen_dfa(A, false, copol, out);
        break;
    case BA_GR:
        return is_polgr_dfa(A, copol, out);
        break;
    default:
        fprintf(stderr, "Error: Invalid mode for is_polgp_dfa.\n");
        exit(EXIT_FAILURE);
        break;
    }
}

bool is_polgp_opti_dfa(dfa *A, bool copol, basis mode, FILE *out)
{
    switch (mode)
    {
    case BA_ST:
        return is_ppt_dfa(A, copol, out);
        break;
    case BA_MOD:
        return is_polgrgen_dfa(A, false, copol, out);
        break;
    case BA_GR:
        return is_polgrgen_dfa(A, true, copol, out);
        break;
    default:
        fprintf(stderr, "Error: Invalid mode for is_polgp_dfa.\n");
        exit(EXIT_FAILURE);
        break;
    }
}

bool is_polgpplus_dfa(dfa *A, bool copol, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO_STRICT(copol ? COPOL_PATTERN_ST_PLUS : POL_PATTERN_ST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO_STRICT(copol ? COPOL_PATTERN_MOD_PLUS : POL_PATTERN_MOD_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO_STRICT(copol ? COPOL_PATTERN_GR_PLUS : POL_PATTERN_GR_PLUS, out);
            break;
        default:
            break;
        }
    }
    // The only supported modes for this function are ST, MOD and GR.
    if (mode == BA_AMT)
    {
        fprintf(stderr, "Error: Invalid mode for is_polgpplus_dfa_opti.\n");
        exit(EXIT_FAILURE);
    }

    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgpplus_dfa_noprint(A, mode, cexa))
        {
            generic_pattern *mypattern = make_generic_pattern();
            dfa_mini_canonical_ordering(A);
            mypattern->vars[0] = mode == BA_ST ? 'u' : 'U';
            mypattern->vars[1] = 'z';
            mypattern->types[0] = mode == BA_ST ? PAT_WORD : PAT_LANG;
            mypattern->types[1] = PAT_WORD;
            mypattern->words[1] = copy_word(cexa->words[2]);
            if (A->order[copol ? cexa->states[1] : cexa->states[0]][copol ? cexa->states[0] : cexa->states[1]])
            {
                mypattern->states[0] = cexa->states[1];
                mypattern->states[1] = cexa->states[0];
                mypattern->words[0] = copy_word(cexa->words[1]);
            }
            else
            {
                mypattern->states[0] = cexa->states[0];
                mypattern->states[1] = cexa->states[1];
                mypattern->words[0] = copy_word(cexa->words[0]);
            }
            view_pattern(A, mypattern, pol_pattern_print);
            free_generic_pattern(mypattern);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgpplus_dfa_noprint(A, mode, NULL))
        {
            return false;
        }
    }

    // We compute the transitive closure of the SCC graph
    dfa_compute_tclos(A);

    // The epsilon transitions generated by Dyck Extension (no self-loops).
    parti *fold = mode == BA_ST ? NULL : dgraph_stal_fold(A->trans, A->sccs, mode);
    bool *EXT = mode == BA_ST ? NULL : dgraph_dyck_ext_eps(A->trans, mode == BA_GR, A->sccs, fold);

    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);

    // For each pair of SCCs (scc1,scc2) such that there is a path from scc1 to scc2 (checked with the transitive closure)
    for (uint i = 0; i < A->tclos->size; i++)
    {
        // First scc
        dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);
        for (uint j = start; j < end; j++)
        {
            if (i == A->tclos->storage[j])
            {
                continue;
            }
            // Second scc
            dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);
            // Product graph of the two SCCs (to check for common loops)
            dgraph *product = dgraph_direct_product(scc1, scc2);
            parti *sccp = dtarjan(product, NULL, false);

            // For each SCC in the product graph
            for (uint k = 0; k < sccp->size_par; k++)
            {
                uint qr = sccp->cl_elems[k][0];
                // If there is no non-trivial loop, we skip this SCC
                if (!dgraph_ntrivial_loop(product, sccp, qr))
                {
                    continue;
                }

                // Otherwise, we check the pattern condition, if it fails we have a counterexample
                uint q = qr / scc2->size_graph;
                uint r = qr % scc2->size_graph;
                uint qo = A->sccs->cl_elems[i][q];
                uint ro = A->sccs->cl_elems[A->tclos->storage[j]][r];
                if (!A->order[copol ? ro : qo][copol ? qo : ro] && (!EXT || EXT[fold->numcl[qo] * fold->size_par + fold->numcl[ro]]))
                {
                    // If the pattern equation is not satisfied, we print a counterexample if requested
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = qo;
                        mypattern->states[1] = ro;
                        mypattern->vars[0] = mode == BA_ST ? 'u' : 'U';
                        mypattern->vars[1] = 'z';
                        mypattern->types[0] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                        mypattern->types[1] = PAT_WORD;
                        dgraph_exists_path(A->trans, qo, ro, true, &mypattern->words[0]);
                        dgraph_exists_path(product, qr, qr, true, &mypattern->words[1]);
                        view_pattern(A, mypattern, pol_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }

                    delete_dgraph(product);
                    delete_parti(sccp);
                    delete_dgraph(scc2);
                    delete_dgraph(scc1);
                    free(EXT);
                    delete_parti(fold);
                    return false;
                }
            }
            delete_dgraph(product);
            delete_parti(sccp);
            delete_dgraph(scc2);
        }
        delete_dgraph(scc1);
    }
    free(EXT);
    delete_parti(fold);
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_pol2gp_dfa(dfa *A, bool copol, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(copol ? COPOL2_PATTERN_ST : POL2_PATTERN_ST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(copol ? COPOL2_PATTERN_MOD : POL2_PATTERN_MOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(copol ? COPOL2_PATTERN_AMT : POL2_PATTERN_AMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(copol ? COPOL2_PATTERN_GR : POL2_PATTERN_GR, out);
            break;
        default:
            break;
        }
    }
    if (mode == BA_ST)
    {

        if (out)
        {
            generic_pattern *cexa = make_generic_pattern();
            if (!is_rtrivialbpgp_dfa_noprint(A, mode, cexa))
            {
                generic_pattern *mypattern = make_generic_pattern();
                dfa_mini_canonical_ordering(A);
                if (A->order[copol ? cexa->states[1] : cexa->states[0]][copol ? cexa->states[0] : cexa->states[1]])
                {
                    mypattern->states[0] = cexa->states[1];
                    mypattern->states[1] = cexa->states[0];
                    mypattern->vars[0] = 'u';
                    mypattern->vars[1] = 'v';
                    mypattern->types[0] = PAT_WORD;
                    mypattern->types[1] = PAT_WORD;
                    mypattern->words[0] = copy_word(cexa->words[1]);
                    mypattern->words[1] = copy_word(cexa->words[2]);
                }
                else
                {
                    mypattern->states[0] = cexa->states[0];
                    mypattern->states[1] = cexa->states[1];
                    mypattern->vars[0] = 'u';
                    mypattern->vars[1] = 'v';
                    mypattern->types[0] = PAT_WORD;
                    mypattern->types[1] = PAT_WORD;
                    mypattern->words[0] = copy_word(cexa->words[0]);
                    mypattern->words[1] = copy_word(cexa->words[2]);
                }
                view_pattern(A, mypattern, pol_pattern_print);
                free_generic_pattern(mypattern);
                free_generic_pattern(cexa);
                CONCLUSION_NEGATIVE(out);
                return false;
            }
        }
        else
        {
            if (!is_rtrivialbpgp_dfa_noprint(A, mode, NULL))
            {
                return false;
            }
        }

        // Array to store the alphabet of an SCC in the product graph
        bool calpha[A->trans->size_alpha];

        // We now compute the canonical ordering
        dfa_mini_canonical_ordering(A);

        // We compute the transitive closure
        dfa_compute_tclos(A);

        for (uint i = 0; i < A->tclos->size; i++)
        {
            dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
            uint start = GSTART(A->tclos, i);
            uint end = GEND(A->tclos, i);
            for (uint j = start; j < end; j++)
            {
                dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);
                dgraph *product = dgraph_direct_product(scc1, scc2);
                parti *sccp = dtarjan(product, NULL, false);
                for (uint k = 0; k < sccp->size_par; k++)
                {
                    uint qr = sccp->cl_elems[k][0];
                    uint q = qr / scc2->size_graph;
                    uint r = qr % scc2->size_graph;
                    uint qo = A->sccs->cl_elems[i][q];
                    uint ro = A->sccs->cl_elems[A->tclos->storage[j]][r];
                    if (A->order[copol ? ro : qo][copol ? qo : ro] || !dgraph_compute_alph_scc(product, sccp, k, calpha))
                    {
                        continue;
                    }

                    if (dgraph_exists_path_alpha(A->trans, qo, ro, false, calpha, NULL))
                    {
                        if (out)
                        {
                            generic_pattern *mypattern = make_generic_pattern();
                            mypattern->states[0] = qo;
                            mypattern->states[1] = ro;
                            mypattern->vars[0] = 'u';
                            mypattern->vars[1] = 'v';
                            mypattern->types[0] = PAT_WORD;
                            mypattern->types[1] = PAT_WORD;
                            dgraph_exists_path_alpha(A->trans, qo, ro, false, calpha, &mypattern->words[0]);
                            dgraph_getloop_alpha(product, sccp, calpha, qr, &mypattern->words[1]);
                            view_pattern(A, mypattern, pol_pattern_print);
                            free_generic_pattern(mypattern);
                            CONCLUSION_NEGATIVE(out);
                        }
                        delete_dgraph(product);
                        delete_dgraph(scc2);
                        delete_dgraph(scc1);
                        delete_parti(sccp);
                        return false;
                    }
                }
                delete_dgraph(product);
                delete_dgraph(scc2);
                delete_parti(sccp);
            }
            delete_dgraph(scc1);
        }
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // Computing the sccs
    dfa_compute_sccs(A);

    // Array to store the alphabet of an SCC in the product graph
    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialbpgp_dfa_noprint(A, mode, cexa))
        {
            generic_pattern *mypattern = make_generic_pattern();
            dfa_mini_canonical_ordering(A);
            if (A->order[copol ? cexa->states[1] : cexa->states[0]][copol ? cexa->states[0] : cexa->states[1]])
            {
                mypattern->states[0] = cexa->states[1];
                mypattern->states[1] = cexa->states[0];
                mypattern->vars[0] = 'U';
                mypattern->vars[1] = 'V';
                mypattern->types[0] = PAT_SING;
                mypattern->types[1] = PAT_LANG;
                mypattern->words[0] = copy_word(cexa->words[1]);
            }
            else
            {
                mypattern->states[0] = cexa->states[0];
                mypattern->states[1] = cexa->states[1];
                mypattern->vars[0] = 'U';
                mypattern->vars[1] = 'V';
                mypattern->types[0] = PAT_SING;
                mypattern->types[1] = PAT_LANG;
                mypattern->words[0] = copy_word(cexa->words[0]);
            }
            view_pattern(A, mypattern, pol_pattern_print);
            free_generic_pattern(mypattern);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialbpgp_dfa_noprint(A, mode, NULL))
        {
            return false;
        }
    }

    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);

    // We now check pairs of distinct SCCs

    // The transitive closure of the SCCs
    dfa_compute_tclos(A);
    for (uint i = 0; i < A->tclos->size; i++)
    {
        dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);
        for (uint j = start; j < end; j++)
        {

            dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);
            dgraph *product = dgraph_direct_product(scc1, scc2);
            parti *sccp = dtarjan(product, NULL, false);
            parti *foldscc = dgraph_stal_fold(product, sccp, mode);
            dgraph *g_fold = dgraph_implement_fold(product, sccp, foldscc);
            for (uint k = 0; k < sccp->size_par; k++)
            {
                uint qr = sccp->cl_elems[k][0];
                uint q = qr / scc2->size_graph;
                uint r = qr % scc2->size_graph;
                uint qo = A->sccs->cl_elems[i][q];
                uint ro = A->sccs->cl_elems[A->tclos->storage[j]][r];
                if (A->order[copol ? ro : qo][copol ? qo : ro])
                {
                    continue;
                }

                uint qrc = foldscc->numcl[qr];
                if (dgraph_exists_intersec_path(g_fold, A->trans, qrc, qo, qrc, ro, false, NULL))
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = qo;
                        mypattern->states[1] = ro;
                        mypattern->vars[0] = 'U';
                        mypattern->vars[1] = 'V';
                        mypattern->types[0] = PAT_SING;
                        mypattern->types[1] = PAT_LANG;
                        dgraph_exists_intersec_path(g_fold, A->trans, qrc, qo, qrc, ro, false, &mypattern->words[0]);
                        view_pattern(A, mypattern, pol_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    delete_dgraph(g_fold);
                    delete_parti(foldscc);
                    delete_dgraph(product);
                    delete_dgraph(scc2);
                    delete_dgraph(scc1);
                    delete_parti(sccp);
                    return false;
                }
            }
            delete_dgraph(g_fold);
            delete_parti(foldscc);
            delete_dgraph(product);
            delete_dgraph(scc2);
            delete_parti(sccp);
        }
        delete_dgraph(scc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_pol2gpplus_dfa(dfa *A, bool copol, basis mode, FILE *out)
{

    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(copol ? COPOL2_PATTERN_ST_PLUS : POL2_PATTERN_ST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(copol ? COPOL2_PATTERN_MOD_PLUS : POL2_PATTERN_MOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(copol ? COPOL2_PATTERN_AMT_PLUS : POL2_PATTERN_AMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(copol ? COPOL2_PATTERN_GR_PLUS : POL2_PATTERN_GR_PLUS, out);
            break;
        default:
            break;
        }
    }

    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialbpgpplus_dfa_noprint(A, mode, cexa))
        {
            generic_pattern *mypattern = make_generic_pattern();
            dfa_mini_canonical_ordering(A);
            if (A->order[copol ? cexa->states[1] : cexa->states[0]][copol ? cexa->states[0] : cexa->states[1]])
            {
                mypattern->states[0] = cexa->states[1];
                mypattern->states[1] = cexa->states[0];
            }
            else
            {
                mypattern->states[0] = cexa->states[0];
                mypattern->states[1] = cexa->states[1];
            }
            mypattern->vars[0] = 'U';
            mypattern->vars[1] = 'V';
            mypattern->types[0] = PAT_LANG;
            mypattern->types[1] = PAT_LANG;
            view_pattern(A, mypattern, pol_pattern_print);
            free_generic_pattern(mypattern);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialbpgpplus_dfa_noprint(A, mode, NULL))
        {
            return false;
        }
    }

    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    // The transitive closure of the SCCs
    dfa_compute_tclos(A);

    for (uint i = 0; i < A->tclos->size; i++)
    {
        dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);
        for (uint j = start; j < end; j++)
        {
            dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);
            dgraph *product = dgraph_direct_product(scc1, scc2);
            parti *sccp = dtarjan(product, NULL, false);
            uint *sccp_inv = parti_compute_inv(sccp);
            for (uint k = 0; k < sccp->size_par; k++)
            {
                uint qr = sccp->cl_elems[k][0];
                uint q = qr / scc2->size_graph;
                uint r = qr % scc2->size_graph;
                uint qo = A->sccs->cl_elems[i][q];
                uint ro = A->sccs->cl_elems[A->tclos->storage[j]][r];
                if (A->order[copol ? ro : qo][copol ? qo : ro] || !dgraph_ntrivial_loop(product, sccp, qr))
                {
                    continue;
                }

                dgraph *theloop = dgraph_extract(product, sccp, sccp_inv, k);

                dgraph *thelooptwo = dgraph_direct_product(theloop, theloop);
                dgraph *theloopprod = dgraph_direct_product(A->trans, thelooptwo);
                parti *scc_loopprod = dtarjan(theloopprod, NULL, false);
                delete_dgraph(thelooptwo);

                parti *fold_loop = mode == BA_ST ? NULL : dgraph_stal_fold(theloop, NULL, mode);

                if (dgraph_exists_intersec_path_pgplus(A->trans, theloop, qo, 0, ro, 0, false, theloopprod, scc_loopprod, fold_loop))
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = qo;
                        mypattern->states[1] = ro;
                        mypattern->vars[0] = 'U';
                        mypattern->vars[1] = 'V';
                        mypattern->types[0] = PAT_LANG;
                        mypattern->types[1] = PAT_LANG;
                        view_pattern(A, mypattern, pol_pattern_print);
                        free_generic_pattern(mypattern);
                    }
                    delete_dgraph(theloop);
                    delete_dgraph(theloopprod);
                    delete_parti(scc_loopprod);
                    delete_parti(fold_loop);
                    delete_dgraph(product);
                    delete_dgraph(scc2);
                    delete_dgraph(scc1);
                    delete_parti(sccp);
                    free(sccp_inv);
                    CONCLUSION_NEGATIVE(out);
                    return false;
                }
                delete_parti(fold_loop);
                delete_dgraph(theloop);
                delete_dgraph(theloopprod);
                delete_parti(scc_loopprod);
            }
            free(sccp_inv);
            delete_dgraph(product);
            delete_dgraph(scc2);
            delete_parti(sccp);
        }
        delete_dgraph(scc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

/********/
/*+ DA +*/
/********/

bool is_dagp_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(DA_PATTERN_ST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(DA_PATTERN_MOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(DA_PATTERN_AMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(DA_PATTERN_GR, out);
            break;
        default:
            break;
        }
    }

    // We compute the SCCs of the DFA and its transitive closure (to check reachability between SCCs)
    dfa_compute_tclos(A);

    // We loop over the SCCs of the DFA, and for each non-trivial SCC we check the pattern inside it. If a counterexample is found, we return false.
    for (uint i1 = 0; i1 < A->sccs->size_par; i1++)
    {
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i1))
        {
            continue;
        }
        dgraph *scc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);
        uint start = GSTART(A->tclos, i1);
        uint end = GEND(A->tclos, i1);
        for (uint j = start; j < end; j++)
        {
            uint i2 = A->tclos->storage[j];
            if (!dgraph_ntrivial_scc(A->trans, A->sccs, i2))
            {
                continue;
            }
            dgraph *scc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i2);
            dgraph *product = dgraph_direct_product(scc1, scc2);
            parti *sccp = dtarjan(product, NULL, false);
            parti *fold = mode == BA_ST ? NULL : dgraph_stal_fold(product, sccp, mode);
            parti *thepar = mode == BA_ST ? sccp : fold;
            for (uint k = 0; k < thepar->size_par; k++)
            {
                // If the states are the same, we skip.
                uint qr = thepar->cl_elems[k][0];
                uint q = qr / scc2->size_graph;
                uint r = qr % scc2->size_graph;
                if (A->sccs->cl_elems[i1][q] == A->sccs->cl_elems[i2][r])
                {
                    continue;
                }

                for (uint h1 = 0; h1 < thepar->cl_size[k]; h1++)
                {
                    uint q0q2 = thepar->cl_elems[k][h1];
                    uint q0 = q0q2 / scc2->size_graph;
                    uint q2 = q0q2 % scc2->size_graph;
                    for (uint h2 = 0; h2 < thepar->cl_size[k]; h2++)
                    {
                        if (h1 == h2)
                        {
                            continue;
                        }
                        uint q1q3 = thepar->cl_elems[k][h2];
                        uint q1 = q1q3 / scc2->size_graph;
                        uint q3 = q1q3 % scc2->size_graph;
                        if (dgraph_exists_intersec_path(A->trans, product, A->sccs->cl_elems[i1][q0], q1q3, A->sccs->cl_elems[i2][q2], q0q2, false, NULL))
                        {
                            if (out)
                            {
                                generic_pattern *mypattern = make_generic_pattern();
                                mypattern->states[0] = A->sccs->cl_elems[i1][q0];
                                mypattern->states[1] = A->sccs->cl_elems[i1][q1];
                                mypattern->states[2] = A->sccs->cl_elems[i2][q2];
                                mypattern->states[3] = A->sccs->cl_elems[i2][q3];
                                mypattern->vars[0] = mode == BA_ST ? 'u' : 'U';
                                mypattern->vars[1] = mode == BA_ST ? 'v' : 'V';
                                mypattern->types[0] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                                mypattern->types[1] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                                dgraph_exists_path(product, q0q2, q1q3, false, &mypattern->words[0]);
                                dgraph_exists_intersec_path(A->trans, product, A->sccs->cl_elems[i1][q0], q1q3, A->sccs->cl_elems[i2][q2], q0q2, false, &mypattern->words[1]);
                                view_pattern(A, mypattern, da_pattern_print);
                                free_generic_pattern(mypattern);
                            }

                            delete_dgraph(product);
                            delete_parti(fold);
                            delete_parti(sccp);
                            delete_dgraph(scc2);
                            delete_dgraph(scc1);
                            CONCLUSION_NEGATIVE(out);
                            return false;
                        }
                    }
                }
            }
            delete_dgraph(product);
            delete_parti(fold);
            delete_parti(sccp);
            delete_dgraph(scc2);
        }
        delete_dgraph(scc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

static bool is_da_compare_alpha(bool *a, bool *b, uint size_alpha)
{
    for (uint i = 0; i < size_alpha; i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

bool is_dagp_opti_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(DA_PATTERN_ST_OPTI, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(DA_PATTERN_MOD_OPTI, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(DA_PATTERN_AMT_OPTI, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(DA_PATTERN_GR_OPTI, out);
            break;
        default:
            break;
        }
    }

    // The SCCs of the DFA
    dfa_compute_sccs(A);

    if (mode == BA_ST)
    {
        bool alphap[A->trans->size_alpha];
        bool alphal[A->trans->size_alpha];

        for (uint i = 0; i < A->sccs->size_par; i++)
        {
            // If contains only one state, skip it
            if (A->sccs->cl_size[i] <= 1)
            {
                continue;
            }
            dgraph_compute_alph_scc(A->trans, A->sccs, i, alphap);
            // Check that no transition labeled by a letter in the alphabet of the SCC goes outside the SCC
            // If so, a contradiction is found
            for (uint k = 0; k < A->sccs->cl_size[i]; k++)
            {
                uint state = A->sccs->cl_elems[i][k];
                for (uint a = 0; a < A->trans->size_alpha; a++)
                {
                    if (alphap[a] && i != A->sccs->numcl[A->trans->edges[state][a]])
                    {
                        CONCLUSION_NEGATIVE(out);
                        return false;
                    }
                }
            }

            // We now check the pattern inside the SCC
            dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
            dgraph *product = dgraph_direct_product(thescc, thescc);
            parti *sccp = dtarjan(product, NULL, false);
            for (uint l = 0; l < sccp->size_par; l++)
            {
                uint qr = sccp->cl_elems[l][0];
                uint q = qr / thescc->size_graph;
                uint r = qr % thescc->size_graph;
                if (q == r || !dgraph_compute_alph_scc(product, sccp, l, alphal))
                {

                    continue;
                }

                if (is_da_compare_alpha(alphap, alphal, A->trans->size_alpha) || dgraph_exists_path_alpha(thescc, q, r, false, alphal, NULL))
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = A->sccs->cl_elems[i][q];
                        mypattern->states[1] = A->sccs->cl_elems[i][r];
                        mypattern->vars[0] = 'u';
                        mypattern->vars[1] = 'v';
                        mypattern->types[0] = PAT_WORD;
                        mypattern->types[1] = PAT_WORD;
                        uint qo = A->sccs->cl_elems[i][q];
                        uint ro = A->sccs->cl_elems[i][r];
                        dgraph_exists_path_alpha(A->trans, qo, ro, false, alphal, &mypattern->words[0]);
                        dgraph_getloop_alpha(product, sccp, alphal, qr, &mypattern->words[1]);
                        view_pattern(A, mypattern, pol_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    delete_parti(sccp);
                    delete_dgraph(product);
                    delete_dgraph(thescc);
                    return false;
                }
            }
            delete_parti(sccp);
            delete_dgraph(product);
            delete_dgraph(thescc);
        }
        CONCLUSION_POSITIVE(out);
        return true;
    }

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // If contains only one state, skip it
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }

        // We extract the SCC
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        // We fold the SCC according to the mode (no need to provide the sccs as we are folding only one SCC)
        parti *fold = dgraph_stal_fold(thescc, NULL, mode);
        dgraph *g_fold = dgraph_implement_fold(thescc, NULL, fold);

        for (uint k = 0; k < thescc->size_graph; k++)
        {
            uint c = fold->numcl[k];
            for (uint a = 0; a < A->trans->size_alpha; a++)
            {
                if (thescc->edges[k][a] == UINT_MAX && g_fold->edges[c][a] != UINT_MAX)
                {
                    delete_dgraph(g_fold);
                    delete_parti(fold);
                    delete_dgraph(thescc);
                    CONCLUSION_NEGATIVE(out);
                    return false;
                }
            }
        }

        delete_dgraph(g_fold);
        delete_parti(fold);

        // We now check the pattern inside the SCC
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);
        parti *foldp = dgraph_stal_fold(product, sccp, mode);
        dgraph *g_foldp = dgraph_implement_fold(product, sccp, foldp);
        for (uint l = 0; l < sccp->size_par; l++)
        {
            uint qr = sccp->cl_elems[l][0];
            uint q = qr / thescc->size_graph;
            uint r = qr % thescc->size_graph;
            if (q == r)
            {
                continue;
            }

            if (dgraph_exists_intersec_path(g_foldp, thescc, foldp->numcl[qr], q, foldp->numcl[qr], r, false, NULL))
            {
                if (out)
                {
                    generic_pattern *mypattern = make_generic_pattern();
                    mypattern->states[0] = A->sccs->cl_elems[i][q];
                    mypattern->states[1] = A->sccs->cl_elems[i][r];
                    mypattern->vars[0] = 'U';
                    mypattern->vars[1] = 'V';
                    mypattern->types[0] = PAT_SING;
                    mypattern->types[1] = PAT_LANG;
                    dgraph_exists_intersec_path(g_foldp, thescc, foldp->numcl[qr], q, foldp->numcl[qr], r, false, &mypattern->words[0]);
                    view_pattern(A, mypattern, pol_pattern_print);
                    free_generic_pattern(mypattern);
                    CONCLUSION_POSITIVE(out);
                }
                delete_dgraph(thescc);
                delete_dgraph(product);
                delete_parti(sccp);
                delete_dgraph(g_foldp);
                delete_parti(foldp);
                return false;
            }
        }
        delete_dgraph(product);
        delete_parti(sccp);
        delete_dgraph(g_foldp);
        delete_parti(foldp);
        delete_dgraph(thescc);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_dagpplus_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_ST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_MOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_AMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_GR_PLUS, out);
            break;
        default:
            break;
        }
    }

    // SCCs and transitive closure of the DFA
    dfa_compute_tclos(A);

    for (uint i0 = 0; i0 < A->sccs->size_par; i0++)
    {
        // If contains only one state, skip it
        if (A->sccs->cl_size[i0] <= 1)
        {
            continue;
        }
        // We extract the SCC
        dgraph *thescc0 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i0);

        uint start = GSTART(A->tclos, i0);
        uint end = GEND(A->tclos, i0);
        for (uint n = start; n < end; n++)
        {
            uint i1 = A->tclos->storage[n];
            dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);
            // Direct product of the two SCCs
            dgraph *product = dgraph_direct_product(thescc0, thescc1);
            parti *sccp = dtarjan(product, NULL, false);
            uint *sccp_inv = parti_compute_inv(sccp);

            for (uint j = 0; j < sccp->size_par; j++)
            {
                uint t = sccp->cl_elems[j][0];
                uint t1 = t / thescc1->size_graph;
                uint t2 = t % thescc1->size_graph;
                if ((i0 == i1 && t1 == t2) || !dgraph_ntrivial_loop(product, sccp, t))
                {
                    continue;
                }
                dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
                parti *foldp = BA_ST ? NULL : dgraph_stal_fold(thesccp, NULL, mode);
                // view_dgraph(thesccp);
                dgraph *quad = dgraph_direct_product(thesccp, thesccp);
                parti *sccq = dtarjan(quad, NULL, false);
                for (uint h = 0; h < sccq->size_par; h++)
                {
                    uint qstate = sccq->cl_elems[h][0];
                    if (!dgraph_ntrivial_loop(quad, sccq, qstate))
                    {
                        continue;
                    }
                    // Decode the states in thesccp
                    uint x0x2 = qstate / thesccp->size_graph;
                    uint x1x3 = qstate % thesccp->size_graph;

                    uint x0 = A->sccs->cl_elems[i0][sccp->cl_elems[j][x0x2] / thescc1->size_graph];
                    uint x2 = A->sccs->cl_elems[i1][sccp->cl_elems[j][x0x2] % thescc1->size_graph];
                    uint x1 = A->sccs->cl_elems[i0][sccp->cl_elems[j][x1x3] / thescc1->size_graph];
                    uint x3 = A->sccs->cl_elems[i1][sccp->cl_elems[j][x1x3] % thescc1->size_graph];

                    if ((!foldp || foldp->numcl[x0x2] == foldp->numcl[x1x3]) && dgraph_exists_intersec_path(thesccp, A->trans, x1x3, x0, x0x2, x2, true, NULL))
                    {
                        if (out)
                        {
                            generic_pattern *mypattern = make_generic_pattern();
                            mypattern->states[0] = x0;
                            mypattern->states[1] = x1;
                            mypattern->states[2] = x2;
                            mypattern->states[3] = x3;
                            mypattern->vars[0] = mode == BA_ST ? 'u' : 'U';
                            mypattern->vars[1] = mode == BA_ST ? 'v' : 'V';
                            mypattern->vars[2] = 'z';
                            mypattern->types[0] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                            mypattern->types[1] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                            mypattern->types[2] = PAT_WORD;
                            dgraph_exists_path(thesccp, x0x2, x1x3, true, &mypattern->words[0]);
                            dgraph_exists_intersec_path(thesccp, A->trans, x1x3, x0, x0x2, x2, true, &mypattern->words[1]);
                            dgraph_exists_path(quad, qstate, qstate, true, &mypattern->words[2]);
                            view_pattern(A, mypattern, da_pattern_print);
                            free_generic_pattern(mypattern);
                            CONCLUSION_NEGATIVE(out);
                        }
                        delete_dgraph(product);
                        delete_parti(sccp);
                        free(sccp_inv);
                        delete_dgraph(thescc0);
                        delete_dgraph(thescc1);
                        delete_dgraph(thesccp);
                        delete_parti(foldp);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        return false;
                    }
                }
                delete_dgraph(thesccp);
                delete_parti(foldp);
                delete_dgraph(quad);
                delete_parti(sccq);
            }

            free(sccp_inv);
            delete_dgraph(product);
            delete_parti(sccp);
            delete_dgraph(thescc1);
        }

        delete_dgraph(thescc0);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_dagpplus_opti_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_ST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_MOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_AMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_GR_PLUS, out);
            break;
        default:
            break;
        }
    }

    // SCCs of the DFA
    dfa_compute_sccs(A);

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // If contains only one state, skip it
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }
        // We extract the SCC and compute its folding according to the mode
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        parti *fold = mode == BA_ST ? NULL : dgraph_stal_fold(thescc, NULL, mode);

        // Direct product of the SCC with itself and the SCCs of the product
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);

        // We look at all synchronized loops on two states of thescc.
        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / thescc->size_graph;
            uint r = qr % thescc->size_graph;
            // printf("Checking states (%u,%u)\n", A->sccs->cl_elems[i][q], A->sccs->cl_elems[i][r]);
            if (q == r || (fold && fold->numcl[q] != fold->numcl[r]) || !dgraph_ntrivial_loop(product, sccp, qr))
            {
                continue;
            }
            dequeue *stack = create_dequeue();
            rigins_dequeue(qr, stack);
            bool *visited;
            CALLOC(visited, product->size_graph);
            while (!isempty_dequeue(stack))
            {
                uint st = rigpull_dequeue(stack);
                if (visited[st])
                {
                    continue;
                }
                visited[st] = true;
                uint s = st / thescc->size_graph;
                uint t = st % thescc->size_graph;
                // printf("Visiting states (%u,%u)\n", A->sccs->cl_elems[i][s], A->sccs->cl_elems[i][t]);
                for (uint a = 0; a < A->trans->size_alpha; a++)
                {
                    uint news = thescc->edges[s][a];
                    uint newt = thescc->edges[t][a];
                    if (newt != UINT_MAX && news == UINT_MAX)
                    {
                        delete_dgraph(product);
                        delete_parti(sccp);
                        delete_dgraph(thescc);
                        free(visited);
                        delete_dequeue(stack);
                        delete_parti(fold);
                        CONCLUSION_NEGATIVE(out);
                        return false;
                    }
                    else if (news != UINT_MAX && newt != UINT_MAX)
                    {

                        // printf("  Following letter %u to (%u,%u)\n", a, A->sccs->cl_elems[i][news], A->sccs->cl_elems[i][newt]);
                        uint newst = news * thescc->size_graph + newt;
                        rigins_dequeue(newst, stack);
                    }
                }
            }
            free(visited);
            delete_dequeue(stack);
        }
        delete_parti(fold);
        uint *sccp_inv = parti_compute_inv(sccp);

        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint t = sccp->cl_elems[j][0];
            uint t1 = t / thescc->size_graph;
            uint t2 = t % thescc->size_graph;
            if (t1 == t2 || !dgraph_ntrivial_loop(product, sccp, t))
            {
                continue;
            }
            dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
            parti *foldp = mode == BA_ST ? NULL : dgraph_stal_fold(thesccp, NULL, mode);
            // view_dgraph(thesccp);
            dgraph *quad = dgraph_direct_product(thesccp, thesccp);
            parti *sccq = dtarjan(quad, NULL, false);
            // view_dgraph(quad);
            for (uint h = 0; h < sccq->size_par; h++)
            {
                // The state in the quad graph
                uint qstate = sccq->cl_elems[h][0];
                if (!dgraph_ntrivial_loop(quad, sccq, qstate))
                {
                    continue;
                }

                // Decode the states in thesccp
                uint x0x2 = qstate / thesccp->size_graph;
                uint x1x3 = qstate % thesccp->size_graph;
                if (foldp && foldp->numcl[x0x2] != foldp->numcl[x1x3])
                {
                    continue;
                }

                uint x0 = sccp->cl_elems[j][x0x2] / thescc->size_graph;
                uint x2 = sccp->cl_elems[j][x0x2] % thescc->size_graph;
                uint x1 = sccp->cl_elems[j][x1x3] / thescc->size_graph;
                uint x3 = sccp->cl_elems[j][x1x3] % thescc->size_graph;

                if (dgraph_exists_intersec_path(thesccp, thescc, x1x3, x0, x0x2, x2, true, NULL))
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = A->sccs->cl_elems[i][x0];
                        mypattern->states[1] = A->sccs->cl_elems[i][x1];
                        mypattern->states[2] = A->sccs->cl_elems[i][x2];
                        mypattern->states[3] = A->sccs->cl_elems[i][x3];
                        mypattern->vars[0] = mode == BA_ST ? 'u' : 'U';
                        mypattern->vars[1] = mode == BA_ST ? 'v' : 'V';
                        mypattern->vars[2] = 'z';
                        mypattern->types[0] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                        mypattern->types[1] = mode == BA_ST ? PAT_WORD : PAT_LANG;
                        mypattern->types[2] = PAT_WORD;
                        dgraph_exists_path(thesccp, x0x2, x1x3, true, &mypattern->words[0]);
                        dgraph_exists_intersec_path(thesccp, thescc, x1x3, x0, x0x2, x2, true, &mypattern->words[1]);
                        dgraph_exists_path(quad, qstate, qstate, true, &mypattern->words[2]);
                        view_pattern(A, mypattern, da_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    delete_dgraph(product);
                    delete_parti(sccp);
                    free(sccp_inv);
                    delete_dgraph(thescc);
                    delete_dgraph(thesccp);
                    delete_dgraph(quad);
                    delete_parti(sccq);
                    delete_parti(foldp);
                    return false;
                }
            }
            delete_dgraph(thesccp);
            delete_dgraph(quad);
            delete_parti(sccq);
            delete_parti(foldp);
        }
        free(sccp_inv);
        delete_dgraph(product);
        delete_parti(sccp);
        delete_dgraph(thescc);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_dabpgp_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_BPST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_BPMOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_BPAMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO_STRICT(DA_PATTERN_BPGR, out);
            break;
        default:
            break;
        }
    }

    // Computing the SCCs and the transitive closure of the DFA
    dfa_compute_tclos(A);
    // If the mode is BA_ST, we call the dedicated function.
    if (mode == BA_ST)
    {
        bool alpha[A->trans->size_alpha];
        for (uint i0 = 0; i0 < A->sccs->size_par; i0++)
        {
            // If contains only one state, skip it
            if (A->sccs->cl_size[i0] <= 1)
            {
                continue;
            }
            // We extract the SCC
            dgraph *thescc0 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i0);

            uint start = GSTART(A->tclos, i0);
            uint end = GEND(A->tclos, i0);
            for (uint n = start; n < end; n++)
            {
                uint i1 = A->tclos->storage[n];
                dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);

                // Direct product of the two SCCs
                dgraph *product = dgraph_direct_product(thescc0, thescc1);
                parti *sccp = dtarjan(product, NULL, false);
                uint *sccp_inv = parti_compute_inv(sccp);

                for (uint j = 0; j < sccp->size_par; j++)
                {
                    uint t = sccp->cl_elems[j][0];
                    uint t0 = t / thescc1->size_graph;
                    uint t1 = t % thescc1->size_graph;
                    if (A->sccs->cl_elems[i0][t0] == A->sccs->cl_elems[i1][t1] || !dgraph_ntrivial_loop(product, sccp, t))
                    {
                        continue;
                    }
                    dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
                    // view_dgraph(thesccp);
                    dgraph *quad = dgraph_direct_product(thesccp, thesccp);
                    parti *sccq = dtarjan(quad, NULL, false);
                    // view_dgraph(quad);
                    for (uint h = 0; h < sccq->size_par; h++)
                    {

                        if (!dgraph_compute_alph_scc(quad, sccq, h, alpha))
                        {
                            continue;
                        }
                        // The state in the quad graph
                        uint qstate = sccq->cl_elems[h][0];

                        // Decode the states in thesccp
                        uint x0x2 = qstate / thesccp->size_graph;
                        uint x1x3 = qstate % thesccp->size_graph;

                        uint x0 = A->sccs->cl_elems[i0][sccp->cl_elems[j][x0x2] / thescc1->size_graph];
                        uint x2 = A->sccs->cl_elems[i1][sccp->cl_elems[j][x0x2] % thescc1->size_graph];
                        uint x1 = A->sccs->cl_elems[i0][sccp->cl_elems[j][x1x3] / thescc1->size_graph];
                        uint x3 = A->sccs->cl_elems[i1][sccp->cl_elems[j][x1x3] % thescc1->size_graph];

                        if (dgraph_exists_path_alpha(thesccp, x0x2, x1x3, true, alpha, NULL) && dgraph_exists_intersec_path_alpha(thesccp, A->trans, x1x3, x0, x0x2, x2, true, alpha, NULL))
                        {
                            if (out)
                            {
                                generic_pattern *mypattern = make_generic_pattern();
                                mypattern->states[0] = x0;
                                mypattern->states[1] = x1;
                                mypattern->states[2] = x2;
                                mypattern->states[3] = x3;
                                mypattern->vars[0] = 'u';
                                mypattern->vars[1] = 'v';
                                mypattern->vars[2] = 'z';
                                mypattern->types[0] = PAT_WORD;
                                mypattern->types[1] = PAT_WORD;
                                mypattern->types[2] = PAT_WORD;
                                dgraph_exists_path_alpha(thesccp, x0x2, x1x3, true, alpha, &mypattern->words[0]);
                                dgraph_exists_intersec_path_alpha(thesccp, A->trans, x1x3, x0, x0x2, x2, true, alpha, &mypattern->words[1]);
                                dgraph_getloop_alpha(quad, sccq, alpha, qstate, &mypattern->words[2]);
                                view_pattern(A, mypattern, da_pattern_print);
                                free_generic_pattern(mypattern);
                                CONCLUSION_NEGATIVE(out);
                            }
                            delete_dgraph(product);
                            delete_parti(sccp);
                            free(sccp_inv);
                            delete_dgraph(thescc0);
                            delete_dgraph(thescc1);
                            delete_dgraph(thesccp);
                            delete_dgraph(quad);
                            delete_parti(sccq);
                            return false;
                        }
                    }
                    delete_dgraph(thesccp);
                    delete_dgraph(quad);
                    delete_parti(sccq);
                }
                free(sccp_inv);
                delete_dgraph(product);
                delete_parti(sccp);
                delete_dgraph(thescc1);
            }

            delete_dgraph(thescc0);
        }
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // For the other modes
    for (uint i0 = 0; i0 < A->sccs->size_par; i0++)
    {
        // printf("Starting checks for SCC %u\n", i);
        //  If contains only one state, skip it
        if (A->sccs->cl_size[i0] <= 1)
        {
            continue;
        }
        // We extract the SCC
        dgraph *thescc0 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i0);
        uint start = GSTART(A->tclos, i0);
        uint end = GEND(A->tclos, i0);

        for (uint n = start; n < end; n++)
        {
            uint i1 = A->tclos->storage[n];
            dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);

            // Direct product of the SCC with itself and the SCCs of the product
            dgraph *product = dgraph_direct_product(thescc0, thescc1);
            parti *sccp = dtarjan(product, NULL, false);
            uint *sccp_inv = parti_compute_inv(sccp);

            for (uint j = 0; j < sccp->size_par; j++)
            {
                //   printf("  Checking pair %u/%u\n", j + 1, sccp->size_par);
                uint t = sccp->cl_elems[j][0];
                uint t0 = t / thescc1->size_graph;
                uint t1 = t % thescc1->size_graph;
                if (A->sccs->cl_elems[i0][t0] == A->sccs->cl_elems[i1][t1] || !dgraph_ntrivial_loop(product, sccp, t))
                {
                    continue;
                }
                dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
                // view_dgraph(thesccp);
                dgraph *quad = dgraph_direct_product(thesccp, thesccp);
                parti *sccq = dtarjan(quad, NULL, false);
                uint *sccq_inv = parti_compute_inv(sccq);

                dgraph *big = dgraph_direct_product(thesccp, A->trans);
                // view_dgraph(quad);
                for (uint h = 0; h < sccq->size_par; h++)
                {
                    // printf("    Checking quad SCC %u/%u\n", h + 1, sccq->size_par);

                    if (!dgraph_ntrivial_scc(quad, sccq, h))
                    {
                        continue;
                    }
                    dgraph *myloop = dgraph_extract(quad, sccq, sccq_inv, h);
                    parti *fold = dgraph_stal_fold(myloop, NULL, mode);
                    dgraph *g_fold = dgraph_implement_fold(myloop, NULL, fold);
                    uint myclass = fold->numcl[0]; // The class of qstate (0 in the numbering of the loop)
                    delete_dgraph(myloop);
                    delete_parti(fold);

                    // The state in the quad graph
                    uint qstate = sccq->cl_elems[h][0];

                    // Decode the states in thesccp
                    uint x0x2 = qstate / thesccp->size_graph;
                    uint x1x3 = qstate % thesccp->size_graph;

                    uint x0 = A->sccs->cl_elems[i0][sccp->cl_elems[j][x0x2] / thescc1->size_graph];
                    uint x2 = A->sccs->cl_elems[i1][sccp->cl_elems[j][x0x2] % thescc1->size_graph];
                    uint x1 = A->sccs->cl_elems[i0][sccp->cl_elems[j][x1x3] / thescc1->size_graph];
                    uint x3 = A->sccs->cl_elems[i1][sccp->cl_elems[j][x1x3] % thescc1->size_graph];

                    uint big1 = x1x3 * A->trans->size_graph + x0;
                    uint big2 = x0x2 * A->trans->size_graph + x2;

                    if (dgraph_exists_intersec_path(thesccp, g_fold, x0x2, myclass, x1x3, myclass, false, NULL) &&
                        dgraph_exists_intersec_path(big, g_fold, big1, myclass, big2, myclass, false, NULL))
                    {
                        if (out)
                        {
                            generic_pattern *mypattern = make_generic_pattern();
                            mypattern->states[0] = x0;
                            mypattern->states[1] = x1;
                            mypattern->states[2] = x2;
                            mypattern->states[3] = x3;
                            mypattern->vars[0] = 'U';
                            mypattern->vars[1] = 'V';
                            mypattern->vars[2] = 'W';
                            mypattern->types[0] = PAT_SING;
                            mypattern->types[1] = PAT_SING;
                            mypattern->types[2] = PAT_LANG;
                            dgraph_exists_intersec_path(thesccp, g_fold, x0x2, myclass, x1x3, myclass, false, &mypattern->words[0]);
                            dgraph_exists_intersec_path(big, g_fold, big1, myclass, big2, myclass, false, &mypattern->words[1]);
                            view_pattern(A, mypattern, da_pattern_print);
                            free_generic_pattern(mypattern);
                            CONCLUSION_NEGATIVE(out);
                        }
                        delete_dgraph(g_fold);
                        delete_dgraph(big);
                        delete_dgraph(product);
                        delete_parti(sccp);
                        free(sccp_inv);
                        delete_dgraph(thescc0);
                        delete_dgraph(thescc1);
                        delete_dgraph(thesccp);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        free(sccq_inv);
                        return false;
                    }
                    delete_dgraph(g_fold);
                }
                delete_dgraph(big);
                delete_dgraph(thesccp);
                delete_dgraph(quad);
                delete_parti(sccq);
                free(sccq_inv);
            }
            free(sccp_inv);
            delete_dgraph(product);
            delete_parti(sccp);
            delete_dgraph(thescc1);
            // printf("Finished checks for SCC %u\n", i);
        }
        delete_dgraph(thescc0);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_dabpgp_opti_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(DA_PATTERN_BPST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(DA_PATTERN_BPMOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(DA_PATTERN_BPAMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(DA_PATTERN_BPGR, out);
            break;
        default:
            break;
        }
    }

    dfa_compute_sccs(A);
    // If the mode is BA_ST, we call the dedicated function.
    if (mode == BA_ST)
    {
        bool alpha[A->trans->size_alpha];
        for (uint i = 0; i < A->sccs->size_par; i++)
        {
            // If contains only one state, skip it
            if (A->sccs->cl_size[i] <= 1)
            {
                continue;
            }
            // We extract the SCC
            dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

            // Direct product of the SCC with itself and the SCCs of the product
            dgraph *product = dgraph_direct_product(thescc, thescc);
            parti *sccp = dtarjan(product, NULL, false);

            // We look at all synchronized loops on two states of thescc.
            for (uint j = 0; j < sccp->size_par; j++)
            {
                uint qr = sccp->cl_elems[j][0];
                uint q = qr / thescc->size_graph;
                uint r = qr % thescc->size_graph;
                // printf("Checking states (%u,%u)\n", A->sccs->cl_elems[i][q], A->sccs->cl_elems[i][r]);
                if (q == r || !dgraph_compute_alph_scc(product, sccp, j, alpha))
                {
                    continue;
                }
                parti *sccp_alph = dtarjan(thescc, alpha, false);
                if (sccp_alph->numcl[q] != sccp_alph->numcl[r])
                {
                    delete_parti(sccp_alph);
                    continue;
                }
                uint c = sccp_alph->numcl[q];

                dequeue *stack = create_dequeue();
                rigins_dequeue(qr, stack);
                bool *visited;
                CALLOC(visited, product->size_graph);
                while (!isempty_dequeue(stack))
                {
                    uint st = rigpull_dequeue(stack);
                    if (visited[st])
                    {
                        continue;
                    }
                    visited[st] = true;
                    uint s = st / thescc->size_graph;
                    uint t = st % thescc->size_graph;
                    // printf("Visiting states (%u,%u)\n", A->sccs->cl_elems[i][s], A->sccs->cl_elems[i][t]);
                    for (uint a = 0; a < A->trans->size_alpha; a++)
                    {
                        if (!alpha[a])
                        {
                            continue;
                        }
                        uint news = thescc->edges[s][a];
                        uint newt = thescc->edges[t][a];
                        if (newt != UINT_MAX && sccp_alph->numcl[newt] == c && (news == UINT_MAX || sccp_alph->numcl[news] != c))
                        {
                            delete_dgraph(product);
                            delete_parti(sccp);
                            delete_dgraph(thescc);
                            free(visited);
                            delete_dequeue(stack);
                            delete_parti(sccp_alph);
                            CONCLUSION_NEGATIVE(out);
                            return false;
                        }
                        else if (news != UINT_MAX && sccp_alph->numcl[news] == c && newt != UINT_MAX && sccp_alph->numcl[newt] == c)
                        {

                            // printf("  Following letter %u to (%u,%u)\n", a, A->sccs->cl_elems[i][news], A->sccs->cl_elems[i][newt]);
                            uint newst = news * thescc->size_graph + newt;
                            rigins_dequeue(newst, stack);
                        }
                    }
                }
                delete_parti(sccp_alph);
                free(visited);
                delete_dequeue(stack);
            }

            uint *sccp_inv = parti_compute_inv(sccp);

            for (uint j = 0; j < sccp->size_par; j++)
            {
                uint t = sccp->cl_elems[j][0];
                uint t1 = t / thescc->size_graph;
                uint t2 = t % thescc->size_graph;
                if (t1 == t2 || !dgraph_ntrivial_loop(product, sccp, t))
                {
                    continue;
                }
                dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
                // view_dgraph(thesccp);
                dgraph *quad = dgraph_direct_product(thesccp, thesccp);
                parti *sccq = dtarjan(quad, NULL, false);
                // view_dgraph(quad);
                for (uint h = 0; h < sccq->size_par; h++)
                {

                    if (!dgraph_compute_alph_scc(quad, sccq, h, alpha))
                    {
                        continue;
                    }
                    // The state in the quad graph
                    uint qstate = sccq->cl_elems[h][0];

                    // Decode the states in thesccp
                    uint x0x2 = qstate / thesccp->size_graph;
                    uint x1x3 = qstate % thesccp->size_graph;

                    uint x0 = sccp->cl_elems[j][x0x2] / thescc->size_graph;
                    uint x2 = sccp->cl_elems[j][x0x2] % thescc->size_graph;
                    uint x1 = sccp->cl_elems[j][x1x3] / thescc->size_graph;
                    uint x3 = sccp->cl_elems[j][x1x3] % thescc->size_graph;

                    if (dgraph_exists_path_alpha(thesccp, x0x2, x1x3, true, alpha, NULL) && dgraph_exists_intersec_path_alpha(thesccp, thescc, x1x3, x0, x0x2, x2, true, alpha, NULL))
                    {
                        if (out)
                        {
                            generic_pattern *mypattern = make_generic_pattern();
                            mypattern->states[0] = A->sccs->cl_elems[i][x0];
                            mypattern->states[1] = A->sccs->cl_elems[i][x1];
                            mypattern->states[2] = A->sccs->cl_elems[i][x2];
                            mypattern->states[3] = A->sccs->cl_elems[i][x3];
                            mypattern->vars[0] = 'u';
                            mypattern->vars[1] = 'v';
                            mypattern->vars[2] = 'z';
                            mypattern->types[0] = PAT_WORD;
                            mypattern->types[1] = PAT_WORD;
                            mypattern->types[2] = PAT_WORD;
                            dgraph_exists_path_alpha(thesccp, x0x2, x1x3, true, alpha, &mypattern->words[0]);
                            dgraph_exists_intersec_path_alpha(thesccp, thescc, x1x3, x0, x0x2, x2, true, alpha, &mypattern->words[1]);
                            dgraph_getloop_alpha(quad, sccq, alpha, qstate, &mypattern->words[2]);
                            view_pattern(A, mypattern, da_pattern_print);
                            free_generic_pattern(mypattern);
                            CONCLUSION_NEGATIVE(out);
                        }
                        delete_dgraph(product);
                        delete_parti(sccp);
                        free(sccp_inv);
                        delete_dgraph(thescc);
                        delete_dgraph(thesccp);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        return false;
                    }
                }
                delete_dgraph(thesccp);
                delete_dgraph(quad);
                delete_parti(sccq);
            }
            free(sccp_inv);
            delete_dgraph(product);
            delete_parti(sccp);
            delete_dgraph(thescc);
        }
        CONCLUSION_POSITIVE(out);
        return true;
    }

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // printf("Starting checks for SCC %u\n", i);
        //  If contains only one state, skip it
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }
        // We extract the SCC
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

        // Direct product of the SCC with itself and the SCCs of the product
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);
        uint *sccp_inv = parti_compute_inv(sccp);

        // We look at all synchronized loops on two states of thescc.
        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / thescc->size_graph;
            uint r = qr % thescc->size_graph;
            // printf("Checking states (%u,%u)\n", A->sccs->cl_elems[i][q], A->sccs->cl_elems[i][r]);
            if (q == r || !dgraph_ntrivial_scc(product, sccp, j))
            {
                continue;
            }
            // We extract the loop and fold it according to the mode.
            dgraph *theloop = dgraph_extract(product, sccp, sccp_inv, j);
            parti *foldloop = dgraph_stal_fold(theloop, NULL, mode);
            dgraph *foldloop_g = dgraph_implement_fold(theloop, NULL, foldloop);
            uint jl = foldloop->numcl[0]; // The class of qr (0 in the numbering of the loop)
            delete_dgraph(theloop);
            delete_parti(foldloop);

            dgraph *myinter = dgraph_direct_product(foldloop_g, thescc);
            parti *mysccs = dtarjan(myinter, NULL, false);

            uint myclass = mysccs->numcl[jl * thescc->size_graph + q];

            if (myclass != mysccs->numcl[jl * thescc->size_graph + r])
            {
                delete_dgraph(foldloop_g);
                delete_parti(mysccs);
                delete_dgraph(myinter);
                continue;
            }

            dequeue *stack = create_dequeue();
            rigins_dequeue(jl * product->size_graph + qr, stack);
            bool *visited;
            CALLOC(visited, foldloop_g->size_graph * product->size_graph);
            while (!isempty_dequeue(stack))
            {
                uint cst = rigpull_dequeue(stack);
                if (visited[cst])
                {
                    continue;
                }
                visited[cst] = true;
                uint c = cst / product->size_graph;
                uint st = cst % product->size_graph;
                uint s = st / thescc->size_graph;
                uint t = st % thescc->size_graph;
                uint cs = c * thescc->size_graph + s;
                uint ct = c * thescc->size_graph + t;
                // printf("Visiting states (%u,%u)\n", A->sccs->cl_elems[i][s], A->sccs->cl_elems[i][t]);
                for (uint a = 0; a < A->trans->size_alpha; a++)
                {
                    uint newcs = myinter->edges[cs][a];
                    uint newct = myinter->edges[ct][a];
                    if (newct != UINT_MAX && mysccs->numcl[newct] == myclass && (newcs == UINT_MAX || mysccs->numcl[newcs] != myclass))
                    {
                        delete_dgraph(product);
                        delete_parti(sccp);
                        delete_dgraph(thescc);
                        free(visited);
                        delete_dequeue(stack);
                        delete_dgraph(foldloop_g);
                        delete_parti(mysccs);
                        delete_dgraph(myinter);
                        free(sccp_inv);
                        CONCLUSION_NEGATIVE(out);
                        return false;
                    }
                    else if (newcs != UINT_MAX && mysccs->numcl[newcs] == myclass && newct != UINT_MAX && mysccs->numcl[newct] == myclass)
                    {

                        uint newc = newcs / thescc->size_graph;
                        uint news = newcs % thescc->size_graph;
                        uint newt = newct % thescc->size_graph;
                        uint newst = news * thescc->size_graph + newt;
                        uint newcst = newc * product->size_graph + newst;
                        rigins_dequeue(newcst, stack);
                    }
                }
            }
            free(visited);
            delete_dequeue(stack);
            delete_dgraph(foldloop_g);
            delete_parti(mysccs);
            delete_dgraph(myinter);
        }
        // printf("Finished first part for SCC %u\n", i);
        //  Second part: looking for the pattern inside the sccs.

        for (uint j = 0; j < sccp->size_par; j++)
        {
            //   printf("  Checking pair %u/%u\n", j + 1, sccp->size_par);
            uint t = sccp->cl_elems[j][0];
            uint t1 = t / thescc->size_graph;
            uint t2 = t % thescc->size_graph;
            if (t1 == t2 || !dgraph_ntrivial_loop(product, sccp, t))
            {
                continue;
            }
            dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
            // view_dgraph(thesccp);
            dgraph *quad = dgraph_direct_product(thesccp, thesccp);
            parti *sccq = dtarjan(quad, NULL, false);
            uint *sccq_inv = parti_compute_inv(sccq);

            dgraph *big = dgraph_direct_product(thesccp, thescc);
            // view_dgraph(quad);
            for (uint h = 0; h < sccq->size_par; h++)
            {
                // printf("    Checking quad SCC %u/%u\n", h + 1, sccq->size_par);

                if (!dgraph_ntrivial_scc(quad, sccq, h))
                {
                    continue;
                }
                dgraph *myloop = dgraph_extract(quad, sccq, sccq_inv, h);
                parti *fold = dgraph_stal_fold(myloop, NULL, mode);
                dgraph *g_fold = dgraph_implement_fold(myloop, NULL, fold);
                uint myclass = fold->numcl[0]; // The class of qstate (0 in the numbering of the loop)
                delete_dgraph(myloop);
                delete_parti(fold);

                // The state in the quad graph
                uint qstate = sccq->cl_elems[h][0];

                // Decode the states in thesccp
                uint x0x2 = qstate / thesccp->size_graph;
                uint x1x3 = qstate % thesccp->size_graph;

                uint x0 = sccp->cl_elems[j][x0x2] / thescc->size_graph;
                uint x2 = sccp->cl_elems[j][x0x2] % thescc->size_graph;
                uint x1 = sccp->cl_elems[j][x1x3] / thescc->size_graph;
                uint x3 = sccp->cl_elems[j][x1x3] % thescc->size_graph;

                uint big1 = x1x3 * thescc->size_graph + x0;
                uint big2 = x0x2 * thescc->size_graph + x2;

                if (dgraph_exists_intersec_path(thesccp, g_fold, x0x2, myclass, x1x3, myclass, false, NULL) &&
                    dgraph_exists_intersec_path(big, g_fold, big1, myclass, big2, myclass, false, NULL))
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = A->sccs->cl_elems[i][x0];
                        mypattern->states[1] = A->sccs->cl_elems[i][x1];
                        mypattern->states[2] = A->sccs->cl_elems[i][x2];
                        mypattern->states[3] = A->sccs->cl_elems[i][x3];
                        mypattern->vars[0] = 'U';
                        mypattern->vars[1] = 'V';
                        mypattern->vars[2] = 'W';
                        mypattern->types[0] = PAT_SING;
                        mypattern->types[1] = PAT_SING;
                        mypattern->types[2] = PAT_LANG;
                        dgraph_exists_intersec_path(thesccp, g_fold, x0x2, myclass, x1x3, myclass, false, &mypattern->words[0]);
                        dgraph_exists_intersec_path(big, g_fold, big1, myclass, big2, myclass, false, &mypattern->words[1]);
                        view_pattern(A, mypattern, da_pattern_print);
                        free_generic_pattern(mypattern);
                        CONCLUSION_NEGATIVE(out);
                    }
                    delete_dgraph(g_fold);
                    delete_dgraph(big);
                    delete_dgraph(product);
                    delete_parti(sccp);
                    free(sccp_inv);
                    delete_dgraph(thescc);
                    delete_dgraph(thesccp);
                    delete_dgraph(quad);
                    delete_parti(sccq);
                    free(sccq_inv);
                    return false;
                }
                delete_dgraph(g_fold);
            }
            delete_dgraph(big);
            delete_dgraph(thesccp);
            delete_dgraph(quad);
            delete_parti(sccq);
            free(sccq_inv);
        }
        free(sccp_inv);
        delete_dgraph(product);
        delete_parti(sccp);
        delete_dgraph(thescc);
        // printf("Finished checks for SCC %u\n", i);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_dabpgpplus_dfa(dfa *A, basis mode, int *error, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(DA_PATTERN_BPST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(DA_PATTERN_BPMOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(DA_PATTERN_BPAMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(DA_PATTERN_BPGR_PLUS, out);
            break;
        default:
            break;
        }
    }

    uint *funs;
    morphism *M = dfa_to_morphism(A, error, &funs);
    orbits *bpolorbs = compute_bpgplusorbits(M, LV_GREG, mode);
    uint c[2];

    for (uint i = 0; i < bpolorbs->nb_computed; i++)
    {
        subsemi *S = bpolorbs->orbits[i];
        if (is_da_subsemi(S, c))
        {
            continue;
        }
        if (out)
        {
            uint t = c[0];
            uint tsub = S->mono_to_sub[t];
            uint flsub = ONE;
            uint lclass = S->rels->LCL->numcl[tsub];
            for (uint j = 0; j < S->rels->LCL->cl_size[lclass]; j++)
            {
                flsub = S->rels->LCL->cl_elems[lclass][j];
                if (S->idem_array[flsub])
                {
                    break;
                }
            }
            uint f = S->sub_to_mono[flsub];

            uint frsub = ONE;
            uint rclass = S->rels->RCL->numcl[tsub];
            for (uint j = 0; j < S->rels->RCL->cl_size[rclass]; j++)
            {
                frsub = S->rels->RCL->cl_elems[rclass][j];
                if (S->idem_array[frsub])
                {
                    break;
                }
            }

            uint r = 0;
            uint hfl = 0, hfr = 0;
            while (hfl < S->rels->RCL->cl_size[S->rels->RCL->numcl[flsub]] && hfr < S->rels->LCL->cl_size[S->rels->LCL->numcl[frsub]])
            {
                uint rsubl = S->rels->RCL->cl_elems[S->rels->RCL->numcl[flsub]][hfl];
                uint rsubr = S->rels->LCL->cl_elems[S->rels->LCL->numcl[frsub]][hfr];
                if (rsubl < rsubr)
                {
                    hfl++;
                    continue;
                }

                if (rsubl > rsubr)
                {
                    hfr++;
                    continue;
                }
                if (mor_mult(M, S->sub_to_mono[rsubr], t) == f)
                {
                    r = S->sub_to_mono[rsubr];
                    break;
                }
                hfl++;
                hfr++;
            }

            uint ftf = mor_mult_gen(M, 3, f, t, f);
            uint q = ONE;
            for (uint qi = 0; qi < A->trans->size_graph; qi++)
            {
                if (funs[A->trans->size_graph * f + qi] != funs[A->trans->size_graph * ftf + qi])
                {
                    q = qi;
                    break;
                }
            }

            uint x0 = funs[A->trans->size_graph * f + q];
            uint x1 = funs[A->trans->size_graph * r + x0];
            uint x2 = funs[A->trans->size_graph * t + x0];
            x2 = funs[A->trans->size_graph * f + x2];
            uint x3 = funs[A->trans->size_graph * r + x2];

            generic_pattern *mypattern = make_generic_pattern();
            mypattern->states[0] = x0;
            mypattern->states[1] = x1;
            mypattern->states[2] = x2;
            mypattern->states[3] = x3;
            mypattern->vars[0] = 'U';
            mypattern->vars[1] = 'V';
            mypattern->vars[2] = 'W';
            mypattern->types[0] = PAT_LANG;
            mypattern->types[1] = PAT_LANG;
            mypattern->types[2] = PAT_LANG;
            view_pattern(A, mypattern, da_pattern_print);
            free_generic_pattern(mypattern);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        CONCLUSION_NEGATIVE(out);
        return false;
    }
    delete_morphism(M);
    free(funs);
    delete_orbits(bpolorbs);
    CONCLUSION_POSITIVE(out);
    return true;
}

/**********/
/*+ BPol +*/
/**********/

static void is_piecewise_aloop(dgraph *g, uint q, bool *alpha)
{
    for (uint a = 0; a < g->size_alpha; a++)
    {
        alpha[a] = (q == g->edges[q][a]);
    }
}

static bool is_piecewise_comp(dgraph *g, bool *alpha1, bool *alpha2)
{
    for (uint a = 0; a < g->size_alpha; a++)
    {
        if (alpha1[a] && !alpha2[a])
        {
            return false;
        }
    }
    return true;
}

bool is_piecewise_dfa(dfa *A)
{
    dfa_compute_sccs(A);
    if (A->sccs->size_par != A->trans->size_graph)
    {
        return false;
    }
    // Mirror for the DFA
    lgraph *mirror = dgraph_mirror(A->trans);

    bool alpha1[A->trans->size_alpha];
    bool alpha2[A->trans->size_alpha];

    dequeue *queue = create_dequeue();

    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        // dfa_print_state(A, q, stdout);
        // printf("\n");
        is_piecewise_aloop(A->trans, q, alpha1);

        bool *visited;
        CALLOC(visited, A->trans->size_graph);
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {

            if (alpha1[a])
            {
                rigins_dequeue(A->trans->edges[q][a], queue);
                uint start = mirror->intervals[q * mirror->size_alpha + a];
                uint end = mirror->intervals[q * mirror->size_alpha + a + 1];
                for (uint i = start; i < end; i++)
                {
                    rigins_dequeue(mirror->storage[i], queue);
                }
            }
        }
        visited[q] = true;
        while (!isempty_dequeue(queue))
        {
            uint r = lefpull_dequeue(queue);
            if (visited[r])
            {
                continue;
            }
            visited[r] = true;
            is_piecewise_aloop(A->trans, r, alpha2);
            if (is_piecewise_comp(A->trans, alpha1, alpha2))
            {
                free(visited);
                delete_lgraph(mirror);
                delete_dequeue(queue);
                return false;
            }
            for (uint a = 0; a < A->trans->size_alpha; a++)
            {
                if (alpha1[a])
                {
                    rigins_dequeue(A->trans->edges[r][a], queue);
                    uint start = mirror->intervals[r * mirror->size_alpha + a];
                    uint end = mirror->intervals[r * mirror->size_alpha + a + 1];
                    for (uint i = start; i < end; i++)
                    {
                        rigins_dequeue(mirror->storage[i], queue);
                    }
                }
            }
        }
        free(visited);
    }
    delete_lgraph(mirror);
    delete_dequeue(queue);
    return true;
}

bool is_bpolmod_dfa(dfa *A, FILE *out)
{
    MAKE_PATTERN_INTRO(BPOL_PATTERN_MOD, out);

    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgp_dfa_noprint(A, BA_MOD, cexa))
        {
            cexa->states[2] = cexa->states[0];
            cexa->states[3] = cexa->states[0];
            cexa->states[4] = cexa->states[1];
            cexa->states[5] = cexa->states[1];
            cexa->vars[0] = 'U';
            cexa->vars[1] = 'v';
            cexa->vars[2] = 'W';
            cexa->vars[3] = 'x';
            cexa->types[0] = PAT_LANG;
            cexa->types[1] = PAT_WORD;
            cexa->types[2] = PAT_SING;
            cexa->types[3] = PAT_WORD;
            dgraph_exists_path(A->trans, cexa->states[1], cexa->states[0], false, &cexa->words[1]);
            cexa->words[2] = make_epsilon_word();
            cexa->words[3] = make_epsilon_word();
            view_pattern(A, cexa, knast_pattern_print);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgp_dfa_noprint(A, BA_MOD, NULL))
        {
            return false;
        }
    }
    // If there are less than two SCCs, no contradiction can arise.
    dfa_compute_sccs(A);
    if (A->sccs->size_par <= 1)
    {
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // Transitive closure of the DFA
    dfa_compute_tclos(A);

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // We extract the SCC from the graph.
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        // printf("Checking SCC1 %u/%u\n", i + 1, scca->size_par);
        // view_dgraph(thescc1);

        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // Unary projection of thescc1 (remains deterministic because it is a cycle, this is ensured by the first step)
        // We shall use it to check MOD-separation.
        dgraph *thescc1_unary = dgraph_to_unary(thescc1);

        // We now consider every pair of distinct reachable SCCs from the current one.
        for (uint j = start; j < end; j++)
        {
            if (A->tclos->storage[j] == i)
            {
                continue;
            }
            // We extract the second SCC
            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);
            // printf("Checking SCC2 %u/%u\n", tclos->storage[j] + 1, scca->size_par);
            // view_dgraph(thescc2);

            for (uint k = j; k < end; k++)
            {
                if (A->tclos->storage[k] == i)
                {
                    continue;
                }
                // We extract the third SCC
                dgraph *thescc3 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[k]);
                // printf("Checking SCC3 %u/%u\n", tclos->storage[k] + 1, scca->size_par);
                // view_dgraph(thescc3);

                // We compute the direct product of thescc2 and thescc3 and its SCCs
                dgraph *inter23 = dgraph_direct_product(thescc2, thescc3);
                parti *scc_inter23 = dtarjan(inter23, NULL, false);
                uint *scc_inter23_inv = parti_compute_inv(scc_inter23);
                // printf("Direct product of SCC2 and SCC3 has %u SCCs\n", scc_inter23->size_par);
                // view_dgraph(inter23);

                // For each SCC in the direct product of thescc2 and thescc3
                for (uint h = 0; h < scc_inter23->size_par; h++)
                {
                    // We extract the scc
                    dgraph *local23 = dgraph_extract(inter23, scc_inter23, scc_inter23_inv, h);

                    // Unary projection of local23 (remains deterministic because we are inside an SCC)
                    // Used to check MOD-separation
                    dgraph *local23_unary = dgraph_to_unary(local23);

                    // We compute the direct product of thescc1_unary and local23_unary and its SCCs
                    // This is the MOD-separation check
                    dgraph *sepinter = dgraph_direct_product(thescc1_unary, local23_unary);
                    parti *scc_sepinter = dtarjan(sepinter, NULL, false);
                    // printf("Direct product of SCC1 and local23 has %u SCCs\n", scc_sepinter->size_par);
                    // view_dgraph(sepinter);

                    // For each scc and each pair of states
                    for (uint l = 0; l < scc_sepinter->size_par; l++)
                    {
                        // printf(" Checking SCC of the separation intersection %u/%u\n", l + 1, scc_sepinter->size_par);
                        for (uint li = 0; li < scc_sepinter->cl_size[l]; li++)
                        {
                            uint x0x3x4 = scc_sepinter->cl_elems[l][li];
                            uint x0 = x0x3x4 / local23_unary->size_graph;                   // Number in thescc1
                            uint x3x4 = x0x3x4 % local23_unary->size_graph;                 // Number in local23
                            uint x3 = scc_inter23->cl_elems[h][x3x4] / thescc3->size_graph; // Number in thescc2
                            uint x4 = scc_inter23->cl_elems[h][x3x4] % thescc3->size_graph; // Number in thescc3

                            if (A->sccs->cl_elems[A->tclos->storage[j]][x3] == A->sccs->cl_elems[A->tclos->storage[k]][x4])
                            {
                                // printf("  States in SCC2 and SCC3 are equal, continuing: %d and %d.\n", scca->cl_elems[tclos->storage[j]][x3], scca->cl_elems[tclos->storage[k]][x4]);
                                continue;
                            }
                            // printf(" State in x0 SCC1: %d.\n", scca->cl_elems[i][x0]);
                            // printf("  States in x3 SCC2 and x4 SCC3 are distinct: %d and %d.\n", scca->cl_elems[tclos->storage[j]][x3], scca->cl_elems[tclos->storage[k]][x4]);
                            for (uint lj = 0; lj < scc_sepinter->cl_size[l]; lj++)
                            {
                                uint x1x2x5 = scc_sepinter->cl_elems[l][lj];
                                uint x1 = x1x2x5 / local23_unary->size_graph;                   // Number in thescc1
                                uint x2x5 = x1x2x5 % local23_unary->size_graph;                 // Number in local23
                                uint x2 = scc_inter23->cl_elems[h][x2x5] / thescc3->size_graph; // Number in thescc2
                                uint x5 = scc_inter23->cl_elems[h][x2x5] % thescc3->size_graph; // Number in thescc3
                                // printf(" State in x1 SCC1: %d.\n", scca->cl_elems[i][x1]);
                                // printf("  States in x2 SCC2 and x5 SCC3 are distinct: %d and %d.\n", scca->cl_elems[tclos->storage[j]][x2], scca->cl_elems[tclos->storage[k]][x5]);
                                // printf("   Checking paths... %d and %d\n", x3x4, x2x5);
                                // view_dgraph(inter23);
                                if (dgraph_exists_intersec_path(A->trans, local23, A->sccs->cl_elems[i][x0], x3x4, A->sccs->cl_elems[A->tclos->storage[j]][x2], x2x5, false, NULL) &&
                                    dgraph_exists_intersec_path(A->trans, local23, A->sccs->cl_elems[i][x1], x2x5, A->sccs->cl_elems[A->tclos->storage[k]][x4], x3x4, false, NULL))
                                {
                                    if (out)
                                    {
                                        generic_pattern *mypattern = make_generic_pattern();
                                        mypattern->states[0] = A->sccs->cl_elems[i][x0];
                                        mypattern->states[1] = A->sccs->cl_elems[i][x1];
                                        mypattern->states[2] = A->sccs->cl_elems[A->tclos->storage[j]][x2];
                                        mypattern->states[3] = A->sccs->cl_elems[A->tclos->storage[j]][x3];
                                        mypattern->states[4] = A->sccs->cl_elems[A->tclos->storage[k]][x4];
                                        mypattern->states[5] = A->sccs->cl_elems[A->tclos->storage[k]][x5];
                                        mypattern->vars[0] = 'U';
                                        mypattern->vars[1] = 'v';
                                        mypattern->vars[2] = 'W';
                                        mypattern->vars[3] = 'x';
                                        mypattern->types[0] = PAT_LANG;
                                        mypattern->types[1] = PAT_WORD;
                                        mypattern->types[2] = PAT_LANG;
                                        mypattern->types[3] = PAT_WORD;
                                        dgraph_exists_path(A->trans, A->sccs->cl_elems[i][x1], A->sccs->cl_elems[i][x0], false, &mypattern->words[1]);
                                        dgraph_exists_intersec_path(A->trans, local23, A->sccs->cl_elems[i][x1], x2x5, A->sccs->cl_elems[A->tclos->storage[k]][x4], x3x4, false, &mypattern->words[3]);
                                        view_pattern(A, mypattern, knast_pattern_print);
                                        free_generic_pattern(mypattern);
                                    }
                                    // Clean_up
                                    delete_dgraph(sepinter);
                                    delete_parti(scc_sepinter);
                                    delete_dgraph(local23_unary);
                                    delete_dgraph(local23);
                                    delete_dgraph(inter23);
                                    delete_parti(scc_inter23);
                                    free(scc_inter23_inv);
                                    delete_dgraph(thescc3);
                                    delete_dgraph(thescc2);
                                    delete_dgraph(thescc1_unary);
                                    delete_dgraph(thescc1);
                                    CONCLUSION_NEGATIVE(out);
                                    return false;
                                }
                            }
                        }
                    }
                    delete_dgraph(sepinter);
                    delete_parti(scc_sepinter);
                    delete_dgraph(local23_unary);
                    delete_dgraph(local23);
                }
                delete_dgraph(inter23);
                delete_parti(scc_inter23);
                free(scc_inter23_inv);
                delete_dgraph(thescc3);
            }
            delete_dgraph(thescc2);
        }
        delete_dgraph(thescc1_unary);
        delete_dgraph(thescc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_bpolamt_dfa(dfa *A, FILE *out)
{
    MAKE_PATTERN_INTRO(BPOL_PATTERN_AMT, out);

    // SCCs of the DFA
    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgp_dfa_noprint(A, BA_AMT, cexa))
        {
            cexa->states[2] = cexa->states[0];
            cexa->states[3] = cexa->states[0];
            cexa->states[4] = cexa->states[1];
            cexa->states[5] = cexa->states[1];
            cexa->vars[0] = 'U';
            cexa->vars[1] = 'v';
            cexa->vars[2] = 'W';
            cexa->vars[3] = 'x';
            cexa->types[0] = PAT_LANG;
            cexa->types[1] = PAT_WORD;
            cexa->types[2] = PAT_SING;
            cexa->types[3] = PAT_WORD;
            dgraph_exists_path(A->trans, cexa->states[1], cexa->states[0], false, &cexa->words[1]);
            cexa->words[2] = make_epsilon_word();
            cexa->words[3] = make_epsilon_word();
            view_pattern(A, cexa, knast_pattern_print);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgp_dfa_noprint(A, BA_AMT, NULL))
        {
            return false;
        }
    }

    dfa_compute_sccs(A);
    // If there are less than two SCCs, no contradiction can arise.
    if (A->sccs->size_par <= 1)
    {
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // Transitive closure of the DFA
    dfa_compute_tclos(A);

    int target[A->trans->size_alpha];
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // We extract the SCC from the graph.
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        // printf("Checking SCC1 %u/%u\n", i + 1, scca->size_par);
        // view_dgraph(thescc1);

        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // A contradiction cannot arise if there are less than two reachable SCCs from the current one.
        if (end - start < 1)
        {
            // printf("  Less than two reachable SCCs, continuing\n");
            delete_dgraph(thescc1);
            continue;
        }
        // Span forest of thescc1 (used to check AMT-separation)
        num_span_forest *spanone = compute_span_forest(thescc1, NULL, NULL);

        // We now consider every pair of distinct reachable SCCs from the current one.
        for (uint j = start; j < end; j++)
        {
            if (A->tclos->storage[j] == i)
            {
                continue;
            }
            // We extract the second SCC
            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);
            // printf("Checking SCC2 %u/%u\n", A->tclos->storage[j] + 1, A->sccs->size_par);
            // view_dgraph(thescc2);

            for (uint k = j; k < end; k++)
            {
                if (A->tclos->storage[k] == i)
                {
                    continue;
                }
                // We extract the third SCC
                dgraph *thescc3 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[k]);
                // printf("Checking SCC3 %u/%u\n", A->tclos->storage[k] + 1, A->sccs->size_par);
                // view_dgraph(thescc3);

                // We compute the direct product of thescc2 and thescc3 and its SCCs
                dgraph *inter23 = dgraph_direct_product(thescc2, thescc3);
                parti *scc_inter23 = dtarjan(inter23, NULL, false);
                uint *scc_inter23_inv = parti_compute_inv(scc_inter23);
                // printf("Direct product of SCC2 and SCC3 has %u SCCs\n", scc_inter23->size_par);
                // view_dgraph(inter23);

                // For each SCC in the direct product of thescc2 and thescc3
                for (uint h = 0; h < scc_inter23->size_par; h++)
                {
                    uint u2 = scc_inter23->cl_elems[h][0] / thescc3->size_graph; // Number in thescc2
                    uint u3 = scc_inter23->cl_elems[h][0] % thescc3->size_graph; // Number in thescc3
                    uint u2i = A->sccs->cl_elems[A->tclos->storage[j]][u2];
                    uint u3i = A->sccs->cl_elems[A->tclos->storage[k]][u3];
                    if (u2i == u3i)
                    {
                        // printf("  States in SCC2 and SCC3 are equal, continuing.\n");
                        continue;
                    }
                    // We extract the scc
                    dgraph *local23 = dgraph_extract(inter23, scc_inter23, scc_inter23_inv, h);

                    // Span forest of local23 (used to check AMT-separation)
                    num_span_forest *spanlocal = compute_span_forest(local23, NULL, NULL);

                    fmpz_mat_t MAT;
                    build_hnf_matrix_two(thescc1, local23, spanone, spanlocal, 0, 0, MAT);

                    for (uint x0 = 0; x0 < thescc1->size_graph; x0++)
                    {
                        uint x0i = A->sccs->cl_elems[i][x0];
                        // printf(" Checking x0 state %d\n", x0i);
                        for (uint x1 = 0; x1 < thescc1->size_graph; x1++)
                        {
                            uint x1i = A->sccs->cl_elems[i][x1];
                            for (uint x3x4 = 0; x3x4 < local23->size_graph; x3x4++)
                            {
                                uint x3 = scc_inter23->cl_elems[h][x3x4] / thescc3->size_graph; // Number in thescc2
                                uint x4 = scc_inter23->cl_elems[h][x3x4] % thescc3->size_graph; // Number in thescc3
                                uint x3i = A->sccs->cl_elems[A->tclos->storage[j]][x3];
                                uint x4i = A->sccs->cl_elems[A->tclos->storage[k]][x4];
                                for (uint x2x5 = 0; x2x5 < local23->size_graph; x2x5++)
                                {
                                    uint x2 = scc_inter23->cl_elems[h][x2x5] / thescc3->size_graph; // Number in thescc2
                                    uint x5 = scc_inter23->cl_elems[h][x2x5] % thescc3->size_graph; // Number in thescc3
                                    uint x2i = A->sccs->cl_elems[A->tclos->storage[j]][x2];
                                    uint x5i = A->sccs->cl_elems[A->tclos->storage[k]][x5];

                                    for (uint a = 0; a < A->trans->size_alpha; a++)
                                    {
                                        target[a] = (spanone->span_forest[x1][a] - spanone->span_forest[x0][a]) -
                                                    (spanlocal->span_forest[x2x5][a] - spanlocal->span_forest[x3x4][a]);
                                    }
                                    // printf(" Checking states %d, %d, %d, %d, %d, %d\n", x0i, x1i, x2i, x3i, x4i, x5i);

                                    if (solve_system_amt(MAT, target, fmpz_mat_nrows(MAT), fmpz_mat_ncols(MAT)) &&
                                        dgraph_exists_intersec_path(local23, A->trans, x3x4, x0i, x2x5, x2i, false, NULL) &&
                                        dgraph_exists_intersec_path(local23, A->trans, x2x5, x1i, x3x4, x4i, false, NULL))
                                    {
                                        if (out)
                                        {
                                            generic_pattern *mypattern = make_generic_pattern();
                                            mypattern->states[0] = x0i;
                                            mypattern->states[1] = x1i;
                                            mypattern->states[2] = x2i;
                                            mypattern->states[3] = x3i;
                                            mypattern->states[4] = x4i;
                                            mypattern->states[5] = x5i;
                                            mypattern->vars[0] = 'U';
                                            mypattern->vars[1] = 'v';
                                            mypattern->vars[2] = 'W';
                                            mypattern->vars[3] = 'x';
                                            mypattern->types[0] = PAT_LANG;
                                            mypattern->types[1] = PAT_WORD;
                                            mypattern->types[2] = PAT_LANG;
                                            mypattern->types[3] = PAT_WORD;
                                            dgraph_exists_path(A->trans, x1i, x0i, false, &mypattern->words[1]);
                                            dgraph_exists_intersec_path(A->trans, local23, x1i, x2x5, x4i, x3x4, false, &mypattern->words[3]);
                                            view_pattern(A, mypattern, knast_pattern_print);
                                            free_generic_pattern(mypattern);
                                        }
                                        // Clean_up
                                        fmpz_mat_clear(MAT);
                                        delete_span_forest(spanlocal);
                                        delete_span_forest(spanone);
                                        delete_dgraph(local23);
                                        delete_dgraph(inter23);
                                        delete_parti(scc_inter23);
                                        free(scc_inter23_inv);
                                        delete_dgraph(thescc3);
                                        delete_dgraph(thescc2);

                                        delete_dgraph(thescc1);
                                        CONCLUSION_NEGATIVE(out);
                                        return false;
                                    }
                                }
                            }
                        }
                    }
                    delete_dgraph(local23);
                    fmpz_mat_clear(MAT);
                    delete_span_forest(spanlocal);
                }
                delete_dgraph(inter23);
                delete_parti(scc_inter23);
                free(scc_inter23_inv);
                delete_dgraph(thescc3);
            }
            delete_dgraph(thescc2);
        }
        delete_span_forest(spanone);
        delete_dgraph(thescc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_bpoldd_dfa(dfa *A, FILE *out)
{
    MAKE_PATTERN_INTRO_STRICT(BPOL_PATTERN_ST_PLUS, out);
    // SCCs of the DFA
    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgpplus_dfa_noprint(A, BA_ST, cexa))
        {
            cexa->states[2] = cexa->states[0];
            cexa->states[3] = cexa->states[0];
            cexa->states[4] = cexa->states[1];
            cexa->states[5] = cexa->states[1];
            cexa->vars[0] = 'u';
            cexa->vars[1] = 'v';
            cexa->vars[2] = 'w';
            cexa->vars[3] = 'x';
            cexa->vars[4] = 'y';
            cexa->vars[5] = 'z';
            cexa->types[0] = PAT_WORD;
            cexa->types[1] = PAT_WORD;
            cexa->types[2] = PAT_WORD;
            cexa->types[3] = PAT_WORD;
            cexa->types[4] = PAT_WORD;
            cexa->types[5] = PAT_WORD;
            cexa->words[3] = copy_word(cexa->words[2]);
            cexa->words[4] = copy_word(cexa->words[2]);
            cexa->words[5] = copy_word(cexa->words[2]);
            view_pattern(A, cexa, knast_pattern_print);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgpplus_dfa_noprint(A, BA_ST, NULL))
        {
            return false;
        }
    }

    // No contradiction can arise at this point if there are less than three SCCs.
    dfa_compute_sccs(A);
    if (A->sccs->size_par <= 2)
    {
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // The transitive closure
    dfa_compute_tclos(A);

    // For each SCC of the graph
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // printf("Checking SCC %u/%u\n", i + 1, A->sccs->size_par);
        //  If the SCC is trivial, it cannot yield a counterexample and we skip it.
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
        {
            // printf("  Trivial SCC, continuing\n");
            continue;
        }

        // We extract the SCC from the graph.
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // A contradiction cannot arise if there are less than two reachable SCCs from the current one.
        if (end - start < 2)
        {
            // printf("  Less than two reachable SCCs, continuing\n");
            delete_dgraph(thescc1);
            continue;
        }

        // We now consider every pair of distinct reachable SCCs from the current one.
        for (uint j = start; j < end; j++)
        {
            // printf("  Checking first reachable SCC %u/%u\n", j - start + 1, end - start);
            if (A->tclos->storage[j] == i || !dgraph_ntrivial_scc(A->trans, A->sccs, A->tclos->storage[j]))
            {
                // printf("    First reachable SCC is trivial, continuing\n");
                continue;
            }

            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);

            for (uint k = start; k < end; k++)
            {
                // printf("    Checking second reachable SCC %u/%u\n", k - start + 1, end - start);
                if (A->tclos->storage[k] == i || A->tclos->storage[k] == j || !dgraph_ntrivial_scc(A->trans, A->sccs, A->tclos->storage[k]))
                {
                    // printf("      Second reachable SCC is trivial, continuing\n");
                    continue;
                }

                dgraph *thescc3 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[k]);
                dgraph *inter23 = dgraph_direct_product(thescc2, thescc3);
                parti *scc_inter23 = dtarjan(inter23, NULL, false);
                dgraph *inter123 = dgraph_direct_product(thescc1, inter23);
                parti *scc_inter123 = dtarjan(inter123, NULL, false);

                // printf("      Checking intersections of SCCs\n");
                for (uint h = 0; h < scc_inter123->size_par; h++)
                {
                    if (!dgraph_ntrivial_scc(inter123, scc_inter123, h))
                    {
                        continue;
                    }
                    uint x0x3x4 = scc_inter123->cl_elems[h][0];
                    uint x0 = x0x3x4 / inter23->size_graph;
                    uint x3x4 = x0x3x4 % inter23->size_graph;
                    uint x3 = x3x4 / thescc3->size_graph;
                    uint x4 = x3x4 % thescc3->size_graph;

                    if (A->sccs->cl_elems[A->tclos->storage[j]][x3] == A->sccs->cl_elems[A->tclos->storage[k]][x4])
                    {
                        continue;
                    }

                    // printf("        Considering states (%u,%u,%u)\n", A->sccs->cl_elems[i][x0], A->sccs->cl_elems[A->tclos->storage[j]][x3], A->sccs->cl_elems[A->tclos->storage[k]][x4]);
                    for (uint g = h; g < scc_inter123->size_par; g++)
                    {
                        if (!dgraph_ntrivial_scc(inter123, scc_inter123, g))
                        {
                            continue;
                        }
                        uint x1x2x5 = scc_inter123->cl_elems[g][0];
                        uint x1 = x1x2x5 / inter23->size_graph;
                        uint x2x5 = x1x2x5 % inter23->size_graph;
                        if (scc_inter23->numcl[x3x4] != scc_inter23->numcl[x2x5])
                        {
                            continue;
                        }
                        uint x2 = x2x5 / thescc3->size_graph;
                        uint x5 = x2x5 % thescc3->size_graph;

                        // printf("        Considering states (%u,%u,%u)\n", A->sccs->cl_elems[i][x1], A->sccs->cl_elems[A->tclos->storage[j]][x2], A->sccs->cl_elems[A->tclos->storage[k]][x5]);

                        if (dgraph_exists_intersec_path(A->trans, inter23, A->sccs->cl_elems[i][x0], x3x4, A->sccs->cl_elems[A->tclos->storage[j]][x2], x2x5, false, NULL) &&
                            dgraph_exists_intersec_path(A->trans, inter23, A->sccs->cl_elems[i][x1], x2x5, A->sccs->cl_elems[A->tclos->storage[k]][x4], x3x4, false, NULL))
                        {
                            if (out)
                            {
                                generic_pattern *mypattern = make_generic_pattern();
                                mypattern->states[0] = A->sccs->cl_elems[i][x0];
                                mypattern->states[1] = A->sccs->cl_elems[i][x1];
                                mypattern->states[2] = A->sccs->cl_elems[A->tclos->storage[j]][x2];
                                mypattern->states[3] = A->sccs->cl_elems[A->tclos->storage[k]][x4];
                                mypattern->states[4] = A->sccs->cl_elems[A->tclos->storage[k]][x4];
                                mypattern->states[5] = A->sccs->cl_elems[A->tclos->storage[k]][x5];
                                mypattern->vars[0] = 'u';
                                mypattern->vars[1] = 'v';
                                mypattern->vars[2] = 'w';
                                mypattern->vars[3] = 'x';
                                mypattern->vars[4] = 'y';
                                mypattern->vars[5] = 'z';
                                mypattern->types[0] = PAT_WORD;
                                mypattern->types[1] = PAT_WORD;
                                mypattern->types[2] = PAT_WORD;
                                mypattern->types[3] = PAT_WORD;
                                mypattern->types[4] = PAT_WORD;
                                mypattern->types[5] = PAT_WORD;
                                dgraph_exists_path(A->trans, A->sccs->cl_elems[i][x0], A->sccs->cl_elems[i][x1], true, &mypattern->words[0]);
                                dgraph_exists_path(A->trans, A->sccs->cl_elems[i][x1], A->sccs->cl_elems[i][x0], true, &mypattern->words[1]);
                                dgraph_exists_intersec_path(A->trans, inter23, A->sccs->cl_elems[i][x0], x3x4, A->sccs->cl_elems[A->tclos->storage[j]][x2], x2x5, true, &mypattern->words[2]);
                                dgraph_exists_intersec_path(A->trans, inter23, A->sccs->cl_elems[i][x1], x2x5, A->sccs->cl_elems[A->tclos->storage[k]][x4], x3x4, true, &mypattern->words[3]);
                                dgraph_exists_path(inter123, x0x3x4, x0x3x4, true, &mypattern->words[4]);
                                dgraph_exists_path(inter123, x1x2x5, x1x2x5, true, &mypattern->words[5]);
                                view_pattern(A, mypattern, knast_pattern_print);
                                free_generic_pattern(mypattern);
                            }
                            delete_dgraph(thescc3);
                            delete_dgraph(inter23);
                            delete_parti(scc_inter23);
                            delete_dgraph(inter123);
                            delete_parti(scc_inter123);
                            delete_dgraph(thescc2);
                            delete_dgraph(thescc1);
                            CONCLUSION_NEGATIVE(out);
                            return false;
                        }
                    }
                }
                delete_dgraph(thescc3);
                delete_dgraph(inter23);
                delete_parti(scc_inter23);
                delete_dgraph(inter123);
                delete_parti(scc_inter123);
            }
            delete_dgraph(thescc2);
        }
        delete_dgraph(thescc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_bpolamtplus_dfa(dfa *A, FILE *out)
{
    MAKE_PATTERN_INTRO_STRICT(BPOL_PATTERN_AMT_PLUS, out);
    // For each SCC of the graph
    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgpplus_dfa_noprint(A, BA_AMT, cexa))
        {
            cexa->states[2] = cexa->states[0];
            cexa->states[3] = cexa->states[0];
            cexa->states[4] = cexa->states[1];
            cexa->states[5] = cexa->states[1];
            cexa->vars[0] = 'U';
            cexa->vars[1] = 'v';
            cexa->vars[2] = 'W';
            cexa->vars[3] = 'x';
            cexa->vars[4] = 'y';
            cexa->vars[5] = 'z';
            cexa->types[0] = PAT_LANG;
            cexa->types[1] = PAT_WORD;
            cexa->types[2] = PAT_LANG;
            cexa->types[3] = PAT_WORD;
            cexa->types[4] = PAT_WORD;
            cexa->types[5] = PAT_WORD;
            dgraph_exists_path(A->trans, cexa->states[1], cexa->states[0], true, &cexa->words[1]);
            cexa->words[3] = copy_word(cexa->words[2]);
            cexa->words[4] = copy_word(cexa->words[2]);
            cexa->words[5] = copy_word(cexa->words[2]);
            view_pattern(A, cexa, knast_pattern_print);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgpplus_dfa_noprint(A, BA_AMT, NULL))
        {
            return false;
        }
    }

    // If there are less than two SCCs, no contradiction can arise.
    dfa_compute_sccs(A);
    if (A->sccs->size_par <= 1)
    {
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // Transitive closure of the DFA
    dfa_compute_tclos(A);
    int target[A->trans->size_alpha];

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // We extract the SCC from the graph.
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        // printf("Checking SCC1 %u/%u\n", i + 1, scca->size_par);
        // view_dgraph(thescc1);

        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // A contradiction cannot arise if there are less than two reachable SCCs from the current one.
        if (end - start < 1)
        {
            // printf("  Less than two reachable SCCs, continuing\n");
            delete_dgraph(thescc1);
            continue;
        }
        // Span forest of thescc1 (used to check AMT-separation)
        num_span_forest *spanone = compute_span_forest(thescc1, NULL, NULL);

        // We now consider every pair of distinct reachable SCCs from the current one.
        for (uint j = start; j < end; j++)
        {
            // We extract the second SCC
            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);
            // printf("Checking SCC2 %u/%u\n", tclos->storage[j] + 1, scca->size_par);
            // view_dgraph(thescc2);

            for (uint k = start; k < end; k++)
            {
                // We extract the third SCC
                dgraph *thescc3 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[k]);
                // printf("Checking SCC3 %u/%u\n", tclos->storage[k] + 1, scca->size_par);
                // view_dgraph(thescc3);

                // We compute the direct product of thescc2 and thescc3 and its SCCs
                dgraph *inter23 = dgraph_direct_product(thescc2, thescc3);
                parti *scc_inter23 = dtarjan(inter23, NULL, false);
                uint *scc_inter23_inv = parti_compute_inv(scc_inter23);

                // printf("Direct product of SCC2 and SCC3 has %u SCCs\n", scc_inter23->size_par);
                // view_dgraph(inter23);

                // For each SCC in the direct product of thescc2 and thescc3
                for (uint h = 0; h < scc_inter23->size_par; h++)
                {
                    // If the SCC is trivial, it cannot yield a counterexample and we skip it.
                    if (!dgraph_ntrivial_scc(inter23, scc_inter23, h))
                    {
                        continue;
                    }
                    uint u2 = scc_inter23->cl_elems[h][0] / thescc3->size_graph; // Number in thescc2
                    uint u3 = scc_inter23->cl_elems[h][0] % thescc3->size_graph; // Number in thescc3
                    uint u2i = A->sccs->cl_elems[A->tclos->storage[j]][u2];
                    uint u3i = A->sccs->cl_elems[A->tclos->storage[k]][u3];
                    if (u2i == u3i)
                    {
                        // printf("  States in SCC2 and SCC3 are equal, continuing.\n");
                        continue;
                    }
                    // We extract the scc
                    dgraph *local23 = dgraph_extract(inter23, scc_inter23, scc_inter23_inv, h);
                    // view_dgraph(local23);

                    // Span forest of local23 (used to check AMT-separation)
                    num_span_forest *spanlocal = compute_span_forest(local23, NULL, NULL);

                    fmpz_mat_t MAT;
                    build_hnf_matrix_two(thescc1, local23, spanone, spanlocal, 0, 0, MAT);

                    // Triple product (for synchronized loops)
                    dgraph *inter123 = dgraph_direct_product(thescc1, local23);
                    parti *scc_inter123 = dtarjan(inter123, NULL, false);

                    for (uint c1 = 0; c1 < scc_inter123->size_par; c1++)
                    {
                        if (!dgraph_ntrivial_scc(inter123, scc_inter123, c1))
                        {
                            continue;
                        }
                        uint x0x3x4 = scc_inter123->cl_elems[c1][0];
                        uint x0 = x0x3x4 / local23->size_graph;
                        uint x0o = A->sccs->cl_elems[i][x0];
                        uint x3x4 = x0x3x4 % local23->size_graph;
                        uint x3 = scc_inter23->cl_elems[h][x3x4] / thescc3->size_graph;
                        uint x4 = scc_inter23->cl_elems[h][x3x4] % thescc3->size_graph;
                        uint x3o = A->sccs->cl_elems[A->tclos->storage[j]][x3];
                        uint x4o = A->sccs->cl_elems[A->tclos->storage[k]][x4];

                        // printf(" Considering states (%u,%u,%u)\n", x0o, x3o, x4o);

                        if (x3o == x4o)
                        {
                            continue;
                        }

                        for (uint c2 = 0; c2 < scc_inter123->size_par; c2++)
                        {
                            if (!dgraph_ntrivial_scc(inter123, scc_inter123, c2))
                            {
                                continue;
                            }
                            uint x1x2x5 = scc_inter123->cl_elems[c2][0];
                            uint x1 = x1x2x5 / local23->size_graph;
                            uint x1o = A->sccs->cl_elems[i][x1];
                            uint x2x5 = x1x2x5 % local23->size_graph;
                            uint x2 = scc_inter23->cl_elems[h][x2x5] / thescc3->size_graph;
                            uint x5 = scc_inter23->cl_elems[h][x2x5] % thescc3->size_graph;
                            uint x2o = A->sccs->cl_elems[A->tclos->storage[j]][x2];
                            uint x5o = A->sccs->cl_elems[A->tclos->storage[k]][x5];

                            // printf(" Considering states (%u,%u,%u)\n", x1o, x2o, x5o);
                            // printf(" Checking states %u, %u, %u, %u, %u, %u\n", x0o, x1o, x2o, x3o, x4o, x5o);

                            // printf("x3x4: %u, x2x5: %u\n", x3x4, x2x5);
                            if (x2o == x5o)
                            {
                                continue;
                            }

                            for (uint a = 0; a < A->trans->size_alpha; a++)
                            {
                                target[a] = (spanone->span_forest[x1][a] - spanone->span_forest[x0][a]) -
                                            (spanlocal->span_forest[x2x5][a] - spanlocal->span_forest[x3x4][a]);
                                // printf("  Target[%u] = %d\n", a, target[a]);
                            }

                            if (solve_system_amt(MAT, target, fmpz_mat_nrows(MAT), fmpz_mat_ncols(MAT)) &&
                                dgraph_exists_intersec_path(local23, A->trans, x3x4, x0o, x2x5, x2o, false, NULL) &&
                                dgraph_exists_intersec_path(local23, A->trans, x2x5, x1o, x3x4, x4o, false, NULL))
                            {
                                if (out)
                                {
                                    generic_pattern *mypattern = make_generic_pattern();
                                    mypattern->states[0] = x0o;
                                    mypattern->states[1] = x1o;
                                    mypattern->states[2] = x2o;
                                    mypattern->states[3] = x3o;
                                    mypattern->states[4] = x4o;
                                    mypattern->states[5] = x5o;
                                    mypattern->vars[0] = 'U';
                                    mypattern->vars[1] = 'v';
                                    mypattern->vars[2] = 'W';
                                    mypattern->vars[3] = 'x';
                                    mypattern->vars[4] = 'y';
                                    mypattern->vars[5] = 'z';
                                    mypattern->types[0] = PAT_LANG;
                                    mypattern->types[1] = PAT_WORD;
                                    mypattern->types[2] = PAT_LANG;
                                    mypattern->types[3] = PAT_WORD;
                                    mypattern->types[4] = PAT_WORD;
                                    mypattern->types[5] = PAT_WORD;
                                    dgraph_exists_path(A->trans, x1o, x0o, false, &mypattern->words[1]);
                                    dgraph_exists_intersec_path(A->trans, local23, x1o, x2x5, x4o, x3x4, false, &mypattern->words[3]);
                                    dgraph_exists_path(inter123, x0x3x4, x0x3x4, true, &mypattern->words[4]);
                                    dgraph_exists_path(inter123, x1x2x5, x1x2x5, true, &mypattern->words[5]);
                                    view_pattern(A, mypattern, knast_pattern_print);
                                    free_generic_pattern(mypattern);
                                }
                                // Clean_up
                                delete_dgraph(inter123);
                                delete_parti(scc_inter123);
                                fmpz_mat_clear(MAT);
                                delete_span_forest(spanlocal);
                                delete_span_forest(spanone);
                                delete_dgraph(local23);
                                delete_dgraph(inter23);
                                delete_parti(scc_inter23);
                                free(scc_inter23_inv);
                                delete_dgraph(thescc3);
                                delete_dgraph(thescc2);
                                delete_dgraph(thescc1);
                                CONCLUSION_NEGATIVE(out);
                                return false;
                            }
                        }
                    }
                    delete_dgraph(local23);
                    delete_dgraph(inter123);
                    delete_parti(scc_inter123);
                    fmpz_mat_clear(MAT);
                    delete_span_forest(spanlocal);
                }

                delete_dgraph(inter23);
                delete_parti(scc_inter23);
                free(scc_inter23_inv);
                delete_dgraph(thescc3);
            }
            delete_dgraph(thescc2);
        }
        delete_span_forest(spanone);
        delete_dgraph(thescc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_bpolgpplus_dfa(dfa *A, bool grp, FILE *out)
{
    MAKE_PATTERN_INTRO_STRICT(grp ? BPOL_PATTERN_GR_PLUS : BPOL_PATTERN_MOD_PLUS, out);

    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgpplus_dfa_noprint(A, grp ? BA_GR : BA_MOD, cexa))
        {
            cexa->states[2] = cexa->states[0];
            cexa->states[3] = cexa->states[0];
            cexa->states[4] = cexa->states[1];
            cexa->states[5] = cexa->states[1];
            cexa->vars[0] = 'U';
            cexa->vars[1] = 'v';
            cexa->vars[2] = 'W';
            cexa->vars[3] = 'x';
            cexa->vars[4] = 'y';
            cexa->vars[5] = 'z';
            cexa->types[0] = PAT_LANG;
            cexa->types[1] = PAT_WORD;
            cexa->types[2] = PAT_LANG;
            cexa->types[3] = PAT_WORD;
            cexa->types[4] = PAT_WORD;
            cexa->types[5] = PAT_WORD;
            dgraph_exists_path(A->trans, cexa->states[1], cexa->states[0], true, &cexa->words[1]);
            cexa->words[3] = copy_word(cexa->words[2]);
            cexa->words[4] = copy_word(cexa->words[2]);
            cexa->words[5] = copy_word(cexa->words[2]);
            view_pattern(A, cexa, knast_pattern_print);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgpplus_dfa_noprint(A, grp ? BA_GR : BA_MOD, NULL))
        {
            return false;
        }
    }

    // If there are less than two SCCs, no contradiction can arise.
    dfa_compute_sccs(A);
    if (A->sccs->size_par <= 1)
    {
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // Transitive closure of the DFA
    dfa_compute_tclos(A);

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // We extract the SCC from the graph.
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        // printf("Checking SCC1 %u/%u\n", i + 1, scca->size_par);
        // view_dgraph(thescc1);

        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // A contradiction cannot arise if there are less than two reachable SCCs from the current one.
        if (end - start < 1)
        {
            // printf("  Less than two reachable SCCs, continuing\n");
            delete_dgraph(thescc1);
            continue;
        }
        // Folding of thescc1 with inverse transitions (used to check GR-separation)
        parti *fold1 = dgraph_stal_fold(thescc1, NULL, grp ? BA_GR : BA_MOD);
        dgraph *fold1_g = grp ? shrink_grp(thescc1, fold1, NULL) : shrink_mod(thescc1, fold1, NULL);
        // num_span_forest *spanone = compute_span_forest(thescc1, NULL, NULL);

        // We now consider every pair of distinct reachable SCCs from the current one.
        for (uint j = start; j < end; j++)
        {
            // We extract the second SCC
            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);
            // printf("Checking SCC2 %u/%u\n", A->tclos->storage[j] + 1, A->sccs->size_par);
            // view_dgraph(thescc2);

            for (uint k = start; k < end; k++)
            {
                // We extract the third SCC
                dgraph *thescc3 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[k]);
                // printf("Checking SCC3 %u/%u\n", A->tclos->storage[k] + 1, A->sccs->size_par);
                // view_dgraph(thescc3);

                // We compute the direct product of thescc2 and thescc3 and its SCCs
                dgraph *inter23 = dgraph_direct_product(thescc2, thescc3);
                parti *scc_inter23 = dtarjan(inter23, NULL, false);
                uint *scc_inter23_inv = parti_compute_inv(scc_inter23);

                // printf("Direct product of SCC2 and SCC3 has %u SCCs\n", scc_inter23->size_par);
                // view_dgraph(inter23);

                // For each SCC in the direct product of thescc2 and thescc3
                for (uint h = 0; h < scc_inter23->size_par; h++)
                {
                    // If the SCC is trivial, it cannot yield a counterexample and we skip it.
                    if (!dgraph_ntrivial_scc(inter23, scc_inter23, h))
                    {
                        continue;
                    }
                    uint u2 = scc_inter23->cl_elems[h][0] / thescc3->size_graph; // Number in thescc2
                    uint u3 = scc_inter23->cl_elems[h][0] % thescc3->size_graph; // Number in thescc3
                    uint u2i = A->sccs->cl_elems[A->tclos->storage[j]][u2];
                    uint u3i = A->sccs->cl_elems[A->tclos->storage[k]][u3];
                    if (u2i == u3i)
                    {
                        // printf("  States in SCC2 and SCC3 are equal, continuing.\n");
                        continue;
                    }
                    // We extract the scc
                    dgraph *local23 = dgraph_extract(inter23, scc_inter23, scc_inter23_inv, h);
                    // view_dgraph(local23);

                    // Folding of the scc with inverse transitions (used to check GR-separation)
                    parti *foldlocal = dgraph_stal_fold(local23, NULL, grp ? BA_GR : BA_MOD);
                    dgraph *foldlocal_g = grp ? shrink_grp(local23, foldlocal, NULL) : shrink_mod(local23, foldlocal, NULL);

                    // Triple product (for synchronized loops)
                    dgraph *inter123 = dgraph_direct_product(thescc1, local23);
                    parti *scc_inter123 = dtarjan(inter123, NULL, false);

                    for (uint c1 = 0; c1 < scc_inter123->size_par; c1++)
                    {
                        if (!dgraph_ntrivial_scc(inter123, scc_inter123, c1))
                        {
                            continue;
                        }
                        uint x0x3x4 = scc_inter123->cl_elems[c1][0];
                        uint x0 = x0x3x4 / local23->size_graph;
                        uint x0o = A->sccs->cl_elems[i][x0];
                        uint x3x4 = x0x3x4 % local23->size_graph;
                        uint x3 = scc_inter23->cl_elems[h][x3x4] / thescc3->size_graph;
                        uint x4 = scc_inter23->cl_elems[h][x3x4] % thescc3->size_graph;
                        uint x3o = A->sccs->cl_elems[A->tclos->storage[j]][x3];
                        uint x4o = A->sccs->cl_elems[A->tclos->storage[k]][x4];

                        // printf(" Considering states (%u,%u,%u)\n", x0o, x3o, x4o);

                        if (x3o == x4o)
                        {
                            continue;
                        }

                        for (uint c2 = 0; c2 < scc_inter123->size_par; c2++)
                        {
                            if (!dgraph_ntrivial_scc(inter123, scc_inter123, c2))
                            {
                                continue;
                            }
                            uint x1x2x5 = scc_inter123->cl_elems[c2][0];
                            uint x1 = x1x2x5 / local23->size_graph;
                            uint x1o = A->sccs->cl_elems[i][x1];
                            uint x2x5 = x1x2x5 % local23->size_graph;
                            uint x2 = scc_inter23->cl_elems[h][x2x5] / thescc3->size_graph;
                            uint x5 = scc_inter23->cl_elems[h][x2x5] % thescc3->size_graph;
                            uint x2o = A->sccs->cl_elems[A->tclos->storage[j]][x2];
                            uint x5o = A->sccs->cl_elems[A->tclos->storage[k]][x5];

                            // printf(" Considering states (%u,%u,%u)\n", x1o, x2o, x5o);
                            // printf(" Checking states %u, %u, %u, %u, %u, %u\n", x0o, x1o, x2o, x3o, x4o, x5o);

                            // printf("x3x4: %u, x2x5: %u\n", x3x4, x2x5);
                            if (x2o == x5o)
                            {
                                continue;
                            }

                            if (dgraph_exists_intersec_path(foldlocal_g, fold1_g, foldlocal->numcl[x3x4], fold1->numcl[x0], foldlocal->numcl[x2x5], fold1->numcl[x1], false, NULL) &&
                                dgraph_exists_intersec_path(local23, A->trans, x3x4, x0o, x2x5, x2o, false, NULL) &&
                                dgraph_exists_intersec_path(local23, A->trans, x2x5, x1o, x3x4, x4o, false, NULL))
                            {
                                if (out)
                                {
                                    generic_pattern *mypattern = make_generic_pattern();
                                    mypattern->states[0] = x0o;
                                    mypattern->states[1] = x1o;
                                    mypattern->states[2] = x2o;
                                    mypattern->states[3] = x3o;
                                    mypattern->states[4] = x4o;
                                    mypattern->states[5] = x5o;
                                    mypattern->vars[0] = 'U';
                                    mypattern->vars[1] = 'v';
                                    mypattern->vars[2] = 'W';
                                    mypattern->vars[3] = 'x';
                                    mypattern->vars[4] = 'y';
                                    mypattern->vars[5] = 'z';
                                    mypattern->types[0] = PAT_LANG;
                                    mypattern->types[1] = PAT_WORD;
                                    mypattern->types[2] = PAT_LANG;
                                    mypattern->types[3] = PAT_WORD;
                                    mypattern->types[4] = PAT_WORD;
                                    mypattern->types[5] = PAT_WORD;
                                    dgraph_exists_path(A->trans, x1o, x0o, false, &mypattern->words[1]);
                                    dgraph_exists_intersec_path(A->trans, local23, x1o, x2x5, x4o, x3x4, false, &mypattern->words[3]);
                                    dgraph_exists_path(inter123, x0x3x4, x0x3x4, true, &mypattern->words[4]);
                                    dgraph_exists_path(inter123, x1x2x5, x1x2x5, true, &mypattern->words[5]);
                                    view_pattern(A, mypattern, knast_pattern_print);
                                    free_generic_pattern(mypattern);
                                }
                                // Clean_up
                                delete_dgraph(inter123);
                                delete_parti(scc_inter123);
                                delete_parti(foldlocal);
                                delete_dgraph(foldlocal_g);
                                delete_parti(fold1);
                                delete_dgraph(fold1_g);
                                delete_dgraph(local23);
                                delete_dgraph(inter23);
                                delete_parti(scc_inter23);
                                free(scc_inter23_inv);
                                delete_dgraph(thescc3);
                                delete_dgraph(thescc2);
                                delete_dgraph(thescc1);
                                CONCLUSION_NEGATIVE(out);
                                return false;
                            }
                        }
                    }
                    delete_dgraph(local23);
                    delete_dgraph(inter123);
                    delete_parti(scc_inter123);
                    delete_parti(foldlocal);
                    delete_dgraph(foldlocal_g);
                }

                delete_dgraph(inter23);
                delete_parti(scc_inter23);
                free(scc_inter23_inv);
                delete_dgraph(thescc3);
            }
            delete_dgraph(thescc2);
        }
        delete_parti(fold1);
        delete_dgraph(fold1_g);
        delete_dgraph(thescc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_knastgpplus_dfa(dfa *A, basis mode, FILE *out)
{
    if (mode == BA_ST)
    {
        return is_bpoldd_dfa(A, out);
    }

    if (out)
    {
        switch (mode)
        {
        case BA_MOD:
            MAKE_PATTERN_INTRO_STRICT(KNAST_PATTERN_MOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO_STRICT(KNAST_PATTERN_AMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO_STRICT(KNAST_PATTERN_GR_PLUS, out);
            break;
        default:
            break;
        }
    }

    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialgpplus_dfa_noprint(A, mode, cexa))
        {
            cexa->states[2] = cexa->states[0];
            cexa->states[3] = cexa->states[0];
            cexa->states[4] = cexa->states[1];
            cexa->states[5] = cexa->states[1];
            cexa->vars[0] = 'U';
            cexa->vars[1] = 'V';
            cexa->vars[2] = 'W';
            cexa->vars[3] = 'X';
            cexa->vars[4] = 'Y';
            cexa->vars[5] = 'Z';
            cexa->types[0] = PAT_LANG;
            cexa->types[1] = PAT_LANG;
            cexa->types[2] = PAT_LANG;
            cexa->types[3] = PAT_LANG;
            cexa->types[4] = PAT_WORD;
            cexa->types[5] = PAT_WORD;
            cexa->words[4] = copy_word(cexa->words[2]);
            cexa->words[5] = copy_word(cexa->words[2]);
            view_pattern(A, cexa, knast_pattern_print);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgpplus_dfa_noprint(A, mode, NULL))
        {
            return false;
        }
    }

    // If there are less than two SCCs, no contradiction can arise.
    dfa_compute_sccs(A);
    if (A->sccs->size_par <= 1)
    {
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // The transitive closure
    dfa_compute_tclos(A);

    // For each SCC of the graph
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // printf("Checking SCC %u/%u\n", i + 1, scca->size_par);
        //  If the SCC is trivial, it cannot yield a counterexample and we skip it.
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
        {
            // printf("  Trivial SCC, continuing\n");
            continue;
        }

        // We extract the SCC from the graph.
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        if (end - start < 1)
        {
            // printf("  Less than two reachable SCCs, continuing\n");
            delete_dgraph(thescc1);
            continue;
        }

        parti *fold_thescc1 = dgraph_stal_fold(thescc1, NULL, mode);

        // We now consider every pair of distinct reachable SCCs from the current one.
        for (uint j = start; j < end; j++)
        {
            // printf("  Checking first reachable SCC %u/%u\n", j - start + 1, end - start);
            if (!dgraph_ntrivial_scc(A->trans, A->sccs, A->tclos->storage[j]))
            {
                // printf("    First reachable SCC is trivial, continuing\n");
                continue;
            }

            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);

            for (uint k = j; k < end; k++)
            {
                // printf("    Checking second reachable SCC %u/%u\n", k - start + 1, end - start);
                if (!dgraph_ntrivial_scc(A->trans, A->sccs, A->tclos->storage[k]))
                {
                    // printf("      Second reachable SCC is trivial, continuing\n");
                    continue;
                }

                dgraph *thescc3 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[k]);
                dgraph *inter23 = dgraph_direct_product(thescc2, thescc3);
                parti *scc_inter23 = dtarjan(inter23, NULL, false);
                parti *fold_inter23 = dgraph_stal_fold(inter23, scc_inter23, mode);
                dgraph *inter123 = dgraph_direct_product(thescc1, inter23);
                // view_dgraph(inter123);
                parti *scc_inter123 = dtarjan(inter123, NULL, false);

                parti *fold_inter123 = dgraph_stal_fold(inter123, scc_inter123, mode);

                // printf("      Checking intersections of SCCs\n");
                // for (uint h = 0; h < scc_inter123->size_par; h++)
                for (uint h = 0; h < fold_inter123->size_par; h++)
                {
                    uint x0x3x4 = fold_inter123->cl_elems[h][0];
                    if (!dgraph_ntrivial_loop(inter123, scc_inter123, x0x3x4))
                    {
                        continue;
                    }
                    // uint x0x3x4 = scc_inter123->cl_elems[h][0];
                    uint x0 = x0x3x4 / inter23->size_graph;
                    uint x3x4 = x0x3x4 % inter23->size_graph;
                    uint x3 = x3x4 / thescc3->size_graph;
                    uint x4 = x3x4 % thescc3->size_graph;
                    if (A->sccs->cl_elems[A->tclos->storage[j]][x3] == A->sccs->cl_elems[A->tclos->storage[k]][x4])
                    {
                        continue;
                    }

                    // printf("Part 1: Considering states (%u,%u,%u)\n", A->sccs->cl_elems[i][x0], A->sccs->cl_elems[A->tclos->storage[j]][x3], A->sccs->cl_elems[A->tclos->storage[k]][x4]);

                    for (uint g = 0; g < fold_inter123->size_par; g++) // for (uint g = 0; g < scc_inter123->size_par; g++)
                    {
                        uint x1x2x5 = fold_inter123->cl_elems[g][0];
                        if (!dgraph_ntrivial_loop(inter123, scc_inter123, x1x2x5))
                        {
                            continue;
                        }

                        uint x1 = x1x2x5 / inter23->size_graph;
                        uint x2x5 = x1x2x5 % inter23->size_graph;
                        uint x2 = x2x5 / thescc3->size_graph;
                        uint x5 = x2x5 % thescc3->size_graph;

                        //// printf("PrePart 2: Considering states (%u,%u,%u)\n", A->sccs->cl_elems[i][x1], A->sccs->cl_elems[A->tclos->storage[j]][x2], A->sccs->cl_elems[A->tclos->storage[k]][x5]);
                        if (fold_inter23->numcl[x3x4] != fold_inter23->numcl[x2x5] || fold_thescc1->numcl[x0] != fold_thescc1->numcl[x1])
                        {
                            continue;
                        }

                        // printf("Part 2: Considering states (%u,%u,%u)\n", A->sccs->cl_elems[i][x1], A->sccs->cl_elems[A->tclos->storage[j]][x2], A->sccs->cl_elems[A->tclos->storage[k]][x5]);

                        if (dgraph_exists_intersec_path(A->trans, inter23, A->sccs->cl_elems[i][x0], x3x4, A->sccs->cl_elems[A->tclos->storage[j]][x2], x2x5, false, NULL) &&
                            dgraph_exists_intersec_path(A->trans, inter23, A->sccs->cl_elems[i][x1], x2x5, A->sccs->cl_elems[A->tclos->storage[k]][x4], x3x4, false, NULL))
                        {
                            if (out)
                            {
                                generic_pattern *mypattern = make_generic_pattern();
                                mypattern->states[0] = A->sccs->cl_elems[i][x0];
                                mypattern->states[1] = A->sccs->cl_elems[i][x1];
                                mypattern->states[2] = A->sccs->cl_elems[A->tclos->storage[j]][x2];
                                mypattern->states[3] = A->sccs->cl_elems[A->tclos->storage[j]][x3];
                                mypattern->states[4] = A->sccs->cl_elems[A->tclos->storage[k]][x4];
                                mypattern->states[5] = A->sccs->cl_elems[A->tclos->storage[k]][x5];
                                mypattern->vars[0] = 'U';
                                mypattern->vars[1] = 'V';
                                mypattern->vars[2] = 'W';
                                mypattern->vars[3] = 'X';
                                mypattern->vars[4] = 'Y';
                                mypattern->vars[5] = 'Z';
                                dgraph_exists_path(inter123, x0x3x4, x0x3x4, true, &mypattern->words[4]);
                                dgraph_exists_path(inter123, x1x2x5, x1x2x5, true, &mypattern->words[5]);
                                view_pattern(A, mypattern, knast_pattern_print);
                                free_generic_pattern(mypattern);
                            }
                            delete_dgraph(thescc3);
                            delete_dgraph(inter23);
                            delete_parti(scc_inter23);
                            delete_parti(fold_inter23);
                            delete_dgraph(inter123);
                            delete_parti(scc_inter123);
                            delete_parti(fold_inter123);
                            delete_dgraph(thescc2);
                            delete_dgraph(thescc1);
                            delete_parti(fold_thescc1);
                            CONCLUSION_NEGATIVE(out);
                            return false;
                        }
                    }
                }
                delete_dgraph(thescc3);
                delete_dgraph(inter23);
                delete_parti(scc_inter23);
                delete_parti(fold_inter23);
                delete_dgraph(inter123);
                delete_parti(scc_inter123);
                delete_parti(fold_inter123);
            }
            delete_dgraph(thescc2);
        }
        delete_dgraph(thescc1);
        delete_parti(fold_thescc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_knastat_dfa(dfa *A, FILE *out)
{
    MAKE_PATTERN_INTRO(BPOL2_PATTERN_ST, out);
    if (out)
    {
        generic_pattern *cexa = make_generic_pattern();
        if (!is_rtrivialbpgp_dfa_noprint(A, BA_ST, cexa))
        {
            cexa->states[2] = cexa->states[0];
            cexa->states[3] = cexa->states[0];
            cexa->states[4] = cexa->states[1];
            cexa->states[5] = cexa->states[1];
            cexa->vars[0] = 'u';
            cexa->vars[1] = 'v';
            cexa->vars[2] = 'w';
            cexa->vars[3] = 'x';
            cexa->vars[4] = 'y';
            cexa->vars[5] = 'z';
            cexa->types[0] = PAT_WORD;
            cexa->types[1] = PAT_WORD;
            cexa->types[2] = PAT_WORD;
            cexa->types[3] = PAT_WORD;
            cexa->types[4] = PAT_WORD;
            cexa->types[5] = PAT_WORD;
            cexa->words[3] = copy_word(cexa->words[2]);
            cexa->words[4] = copy_word(cexa->words[2]);
            cexa->words[5] = copy_word(cexa->words[2]);
            view_pattern(A, cexa, knast_pattern_print);
            free_generic_pattern(cexa);
            CONCLUSION_NEGATIVE(out);
            return false;
        }
    }
    else
    {
        if (!is_rtrivialgpplus_dfa_noprint(A, BA_ST, NULL))
        {
            return false;
        }
    }

    bool alpha[A->trans->size_alpha];

    // The transitive closure
    dfa_compute_tclos(A);

    // For each SCC of the graph
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // printf("Checking SCC %u/%u\n", i + 1, scca->size_par);
        //  If the SCC is trivial, it cannot yield a counterexample and we skip it.
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
        {
            // printf("  Trivial SCC, continuing\n");
            continue;
        }

        // We extract the SCC from the graph.
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);

        // We now consider every pair of distinct reachable SCCs from the current one.
        for (uint j = start; j < end; j++)
        {
            // printf("  Checking first reachable SCC %u/%u\n", j - start + 1, end - start);
            if (!dgraph_ntrivial_scc(A->trans, A->sccs, A->tclos->storage[j]))
            {
                // printf("    First reachable SCC is trivial, continuing\n");
                continue;
            }

            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[j]);

            for (uint k = start; k < end; k++)
            {
                // printf("    Checking second reachable SCC %u/%u\n", k - start + 1, end - start);
                if (!dgraph_ntrivial_scc(A->trans, A->sccs, A->tclos->storage[k]))
                {
                    // printf("      Second reachable SCC is trivial, continuing\n");
                    continue;
                }

                dgraph *thescc3 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, A->tclos->storage[k]);
                dgraph *inter23 = dgraph_direct_product(thescc2, thescc3);
                parti *scc_inter23 = dtarjan(inter23, NULL, false);
                dgraph *inter123 = dgraph_direct_product(thescc1, inter23);
                parti *scc_inter123 = dtarjan(inter123, NULL, false);
                uint *scc_inter123_inv = parti_compute_inv(scc_inter123);

                // printf("      Checking intersections of SCCs\n");
                for (uint h = 0; h < scc_inter123->size_par; h++)
                {
                    if (!dgraph_ntrivial_scc(inter123, scc_inter123, h))
                    {
                        continue;
                    }
                    uint x0x3x4 = scc_inter123->cl_elems[h][0];
                    uint x0 = x0x3x4 / inter23->size_graph;
                    uint x3x4 = x0x3x4 % inter23->size_graph;
                    uint x3 = x3x4 / thescc3->size_graph;
                    uint x4 = x3x4 % thescc3->size_graph;

                    if (A->sccs->cl_elems[A->tclos->storage[j]][x3] == A->sccs->cl_elems[A->tclos->storage[k]][x4])
                    {
                        continue;
                    }

                    // printf("        Considering states (%u,%u,%u)\n", scca->cl_elems[i][x0], scca->cl_elems[tclos->storage[j]][x3], scca->cl_elems[tclos->storage[k]][x4]);
                    for (uint g = h; g < scc_inter123->size_par; g++)
                    {
                        if (!dgraph_ntrivial_scc(inter123, scc_inter123, g))
                        {
                            continue;
                        }
                        uint x1x2x5 = scc_inter123->cl_elems[g][0];
                        uint x1 = x1x2x5 / inter23->size_graph;
                        uint x2x5 = x1x2x5 % inter23->size_graph;
                        if (scc_inter23->numcl[x3x4] != scc_inter23->numcl[x2x5])
                        {
                            continue;
                        }
                        uint x2 = x2x5 / thescc3->size_graph;
                        uint x5 = x2x5 % thescc3->size_graph;

                        dgraph_common_alph_loop(inter123, scc_inter123, scc_inter123_inv, x0x3x4, x1x2x5, alpha);

                        // printf("        Considering states (%u,%u,%u)\n", scca->cl_elems[i][x1], scca->cl_elems[tclos->storage[j]][x2], scca->cl_elems[tclos->storage[k]][x5]);

                        if (dgraph_exists_path_alpha(thescc1, x0, x1, false, alpha, NULL) &&
                            dgraph_exists_path_alpha(thescc1, x1, x0, false, alpha, NULL) &&
                            dgraph_exists_intersec_path_alpha(A->trans, inter23, A->sccs->cl_elems[i][x0], x3x4, A->sccs->cl_elems[A->tclos->storage[j]][x2], x2x5, false, alpha, NULL) &&
                            dgraph_exists_intersec_path_alpha(A->trans, inter23, A->sccs->cl_elems[i][x1], x2x5, A->sccs->cl_elems[A->tclos->storage[k]][x4], x3x4, false, alpha, NULL))
                        {
                            if (out)
                            {
                                generic_pattern *mypattern = make_generic_pattern();
                                mypattern->states[0] = A->sccs->cl_elems[i][x0];
                                mypattern->states[1] = A->sccs->cl_elems[i][x1];
                                mypattern->states[2] = A->sccs->cl_elems[A->tclos->storage[j]][x2];
                                mypattern->states[3] = A->sccs->cl_elems[A->tclos->storage[j]][x3];
                                mypattern->states[4] = A->sccs->cl_elems[A->tclos->storage[k]][x4];
                                mypattern->states[5] = A->sccs->cl_elems[A->tclos->storage[k]][x5];
                                mypattern->vars[0] = 'u';
                                mypattern->vars[1] = 'v';
                                mypattern->vars[2] = 'w';
                                mypattern->vars[3] = 'x';
                                mypattern->vars[4] = 'y';
                                mypattern->vars[5] = 'z';
                                mypattern->types[0] = PAT_WORD;
                                mypattern->types[1] = PAT_WORD;
                                mypattern->types[2] = PAT_WORD;
                                mypattern->types[3] = PAT_WORD;
                                mypattern->types[4] = PAT_WORD;
                                mypattern->types[5] = PAT_WORD;
                                dgraph_exists_path_alpha(thescc1, x0, x1, false, alpha, &mypattern->words[0]);
                                dgraph_exists_path_alpha(thescc1, x1, x0, false, alpha, &mypattern->words[1]);
                                dgraph_exists_intersec_path_alpha(A->trans, inter23, A->sccs->cl_elems[i][x0], x3x4, A->sccs->cl_elems[A->tclos->storage[j]][x2], x2x5, true, alpha, &mypattern->words[2]);
                                dgraph_exists_intersec_path_alpha(A->trans, inter23, A->sccs->cl_elems[i][x1], x2x5, A->sccs->cl_elems[A->tclos->storage[k]][x4], x3x4, true, alpha, &mypattern->words[3]);
                                dgraph_getloop_alpha(inter123, scc_inter123, alpha, x0x3x4, &mypattern->words[4]);
                                dgraph_getloop_alpha(inter123, scc_inter123, alpha, x1x2x5, &mypattern->words[5]);
                                view_pattern(A, mypattern, knast_pattern_print);
                                free_generic_pattern(mypattern);
                            }
                            delete_dgraph(thescc3);
                            delete_dgraph(inter23);
                            delete_parti(scc_inter23);
                            free(scc_inter123_inv);
                            delete_dgraph(inter123);
                            delete_parti(scc_inter123);
                            delete_dgraph(thescc2);
                            delete_dgraph(thescc1);
                            CONCLUSION_NEGATIVE(out);
                            return false;
                        }
                    }
                }
                delete_dgraph(thescc3);
                delete_dgraph(inter23);
                delete_parti(scc_inter23);
                free(scc_inter123_inv);
                delete_dgraph(inter123);
                delete_parti(scc_inter123);
            }
            delete_dgraph(thescc2);
        }
        delete_dgraph(thescc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

/**********/
/*+ UPol +*/
/**********/

bool is_upolgpplus_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO_STRICT(UPOL_PATTERN_ST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO_STRICT(UPOL_PATTERN_MOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO_STRICT(UPOL_PATTERN_AMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO_STRICT(UPOL_PATTERN_GR_PLUS, out);
            break;
        default:
            break;
        }
    }
    // SCCs of the DFA
    dfa_compute_sccs(A);
    // We first check for the pattern inside an SCC
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
        {
            continue;
        }

        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        parti *thefold = mode == BA_ST ? NULL : dgraph_stal_fold(thescc, NULL, mode);
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);
        for (uint k = 0; k < sccp->size_par; k++)
        {
            uint qr = sccp->cl_elems[k][0];
            uint q = qr / thescc->size_graph;
            uint r = qr % thescc->size_graph;
            if (q != r && (!thefold || thefold->numcl[q] == thefold->numcl[r]) && dgraph_ntrivial_loop(product, sccp, qr))
            {
                if (out)
                {
                    generic_pattern *mypattern = make_generic_pattern();
                    mypattern->states[0] = A->sccs->cl_elems[i][q];
                    mypattern->states[1] = A->sccs->cl_elems[i][r];
                    mypattern->vars[1] = 'z';
                    mypattern->types[1] = PAT_WORD;
                    dgraph_exists_path(product, qr, qr, true, &mypattern->words[1]);
                    if (mode == BA_ST)
                    {
                        mypattern->vars[0] = 'u';
                        mypattern->types[0] = PAT_WORD;
                        dgraph_exists_path(thescc, q, r, true, &mypattern->words[0]);
                    }
                    else
                    {
                        mypattern->vars[0] = 'U';
                        mypattern->types[0] = PAT_LANG;
                    }
                    view_pattern(A, mypattern, pol_pattern_print);
                    free_generic_pattern(mypattern);
                }
                delete_dgraph(product);
                delete_dgraph(thescc);
                delete_parti(thefold);
                delete_parti(sccp);
                CONCLUSION_NEGATIVE(out);
                return false;
            }
        }
        delete_parti(thefold);
        delete_dgraph(product);
        delete_dgraph(thescc);
        delete_parti(sccp);
    }

    // We now check for the pattern between two distinct SCCs reachable.
    // At the point, if a counterexample exists, one for UPol(GRP) exists as well.
    dfa_compute_tclos(A);
    parti *fold = dgraph_stal_fold(A->trans, A->sccs, BA_GR);
    bool *EXT = dgraph_dyck_ext_eps(A->trans, true, A->sccs, fold);
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
        {
            continue;
        }
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        uint start = GSTART(A->tclos, i);
        uint end = GEND(A->tclos, i);
        // view_dgraph(thescc1);
        for (uint k = start; k < end; k++)
        {
            uint j = A->tclos->storage[k];
            if (i == j || !dgraph_ntrivial_scc(A->trans, A->sccs, j))
            {
                continue;
            }
            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, j);
            // view_dgraph(thescc2);
            dgraph *product = dgraph_direct_product(thescc1, thescc2);
            parti *sccp = dtarjan(product, NULL, false);
            for (uint h = 0; h < sccp->size_par; h++)
            {
                if (!dgraph_ntrivial_scc(product, sccp, h))
                {
                    continue;
                }
                uint qr = sccp->cl_elems[h][0];
                uint q = qr / product->size_graph;
                uint r = qr % product->size_graph;
                uint fq = fold->numcl[A->sccs->cl_elems[i][q]];
                uint fr = fold->numcl[A->sccs->cl_elems[j][r]];

                // printf("Checking states (%u,%u) with fold classes (%u,%u)\n", A->sccs->cl_elems[i][q], A->sccs->cl_elems[j][r], fq, fr);
                if (EXT[fq * fold->size_par + fr])
                {
                    if (out)
                    {
                        generic_pattern *mypattern = make_generic_pattern();
                        mypattern->states[0] = A->sccs->cl_elems[i][q];
                        mypattern->states[1] = A->sccs->cl_elems[j][r];
                        mypattern->vars[1] = 'z';
                        mypattern->types[1] = PAT_WORD;
                        dgraph_exists_path(product, qr, qr, true, &mypattern->words[1]);
                        if (mode == BA_ST)
                        {
                            mypattern->vars[0] = 'u';
                            mypattern->types[0] = PAT_WORD;
                            dgraph_exists_path(A->trans, A->sccs->cl_elems[i][q], A->sccs->cl_elems[j][r], true, &mypattern->words[0]);
                        }
                        else
                        {
                            mypattern->vars[0] = 'U';
                            mypattern->types[0] = PAT_LANG;
                        }
                        view_pattern(A, mypattern, pol_pattern_print);
                        free_generic_pattern(mypattern);
                    }
                    delete_dgraph(product);
                    delete_dgraph(thescc1);
                    delete_dgraph(thescc2);
                    delete_parti(sccp);
                    delete_parti(fold);
                    free(EXT);
                    CONCLUSION_NEGATIVE(out);
                    return false;
                }
            }
        }
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_upolgpplus_opti_dfa(dfa *A, basis mode)
{

    // SCCs of the DFA
    dfa_compute_sccs(A);
    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i))
        {
            continue;
        }

        for (uint j = 0; j < A->sccs->cl_size[i]; j++)
        {
            uint q = A->sccs->cl_elems[i][j];
            for (uint a = 0; a < A->trans->size_alpha; a++)
            {
                uint r = A->trans->edges[q][a];
                if (A->sccs->numcl[r] != i)
                {
                    return false;
                }
            }
        }

        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);
        parti *thefold = mode == BA_ST ? NULL : dgraph_stal_fold(thescc, NULL, mode);
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);
        for (uint k = 0; k < sccp->size_par; k++)
        {
            uint qr = sccp->cl_elems[k][0];
            uint q = qr / thescc->size_graph;
            uint r = qr % thescc->size_graph;
            if (q != r && (!thefold || thefold->numcl[q] == thefold->numcl[r]) && dgraph_ntrivial_loop(product, sccp, qr))
            {
                delete_dgraph(product);
                delete_dgraph(thescc);
                delete_parti(thefold);
                delete_parti(sccp);
                return false;
            }
        }
        delete_parti(thefold);
        delete_dgraph(product);
        delete_dgraph(thescc);
        delete_parti(sccp);
    }
    return true;
}

bool is_upolbp2gp_dfa(dfa *A, basis mode, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(UPOL3_PATTERN_ST, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(UPOL3_PATTERN_MOD, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(UPOL3_PATTERN_AMT, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(UPOL3_PATTERN_GR, out);
            break;
        default:
            break;
        }
    }
    // SCCs and transitive closure of the DFA
    dfa_compute_tclos(A);

    // If the mode is BA_ST, we use a dedicated procedure
    if (mode == BA_ST)
    {
        // An array to store sub-alphabets
        bool alpha[A->trans->size_alpha];

        for (uint i1 = 0; i1 < A->sccs->size_par; i1++)
        {
            // If the SCC is trivial (one state and no loop), it cannot yield a counterexample and we skip it.
            if (!dgraph_ntrivial_scc(A->trans, A->sccs, i1))
            {
                continue;
            }
            // We extract the SCC
            dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);
            // printf("Checking SCC %u/%u\n", i1 + 1, A->sccs->size_par);
            // view_dgraph(thescc1);

            uint start = GSTART(A->tclos, i1);
            uint end = GEND(A->tclos, i1);
            for (uint n = start; n < end; n++)
            {
                uint i2 = A->tclos->storage[n];
                if (!dgraph_ntrivial_scc(A->trans, A->sccs, i2) || (i1 == i2 && A->sccs->cl_size[i1] < 2))
                {
                    continue;
                }
                dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i2);
                // printf("  Checking reachable SCC %u/%u\n", n - start + 1, end - start);
                // view_dgraph(thescc2);

                dgraph *product = dgraph_direct_product(thescc1, thescc2);
                parti *sccp = dtarjan(product, NULL, false);

                uint *sccp_inv = parti_compute_inv(sccp);

                for (uint j = 0; j < sccp->size_par; j++)
                {
                    uint t = sccp->cl_elems[j][0];
                    uint t1 = t / thescc2->size_graph;
                    uint t2 = t % thescc2->size_graph;
                    if (A->sccs->cl_elems[i1][t1] == A->sccs->cl_elems[i2][t2] || !dgraph_ntrivial_loop(product, sccp, t))
                    {
                        continue;
                    }
                    dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
                    // view_dgraph(thesccp);
                    dgraph *quad = dgraph_direct_product(thesccp, thesccp);
                    parti *sccq = dtarjan(quad, NULL, false);
                    // view_dgraph(quad);
                    for (uint h = 0; h < sccq->size_par; h++)
                    {

                        if (!dgraph_compute_alph_scc(quad, sccq, h, alpha))
                        {
                            continue;
                        }
                        // The state in the quad graph
                        uint qstate = sccq->cl_elems[h][0];

                        // Decode the states in thesccp
                        uint x0x2 = qstate / thesccp->size_graph;
                        uint x1x3 = qstate % thesccp->size_graph;

                        uint x0 = sccp->cl_elems[j][x0x2] / thescc2->size_graph;
                        uint x2 = sccp->cl_elems[j][x0x2] % thescc2->size_graph;
                        uint x1 = sccp->cl_elems[j][x1x3] / thescc2->size_graph;
                        uint x3 = sccp->cl_elems[j][x1x3] % thescc2->size_graph;

                        if (dgraph_exists_path_alpha(thesccp, x0x2, x1x3, true, alpha, NULL) &&
                            dgraph_exists_intersec_path(thesccp, A->trans, x1x3, A->sccs->cl_elems[i1][x0], x0x2, A->sccs->cl_elems[i2][x2], true, NULL))
                        {
                            if (out)
                            {
                                generic_pattern *mypattern = make_generic_pattern();
                                mypattern->states[0] = A->sccs->cl_elems[i1][x0];
                                mypattern->states[1] = A->sccs->cl_elems[i1][x1];
                                mypattern->states[2] = A->sccs->cl_elems[i2][x2];
                                mypattern->states[3] = A->sccs->cl_elems[i2][x3];
                                mypattern->vars[0] = 'u';
                                mypattern->vars[1] = 'v';
                                mypattern->vars[2] = 'w';
                                mypattern->types[0] = PAT_WORD;
                                mypattern->types[1] = PAT_WORD;
                                mypattern->types[2] = PAT_WORD;
                                dgraph_exists_path_alpha(thesccp, x0x2, x1x3, true, alpha, &mypattern->words[0]);
                                dgraph_exists_intersec_path(thesccp, A->trans, x1x3, A->sccs->cl_elems[i1][x0], x0x2, A->sccs->cl_elems[i2][x2], true, &mypattern->words[1]);
                                dgraph_getloop_alpha(quad, sccq, alpha, qstate, &mypattern->words[2]);
                                view_pattern(A, mypattern, da_pattern_print);
                                free_generic_pattern(mypattern);
                            }
                            delete_dgraph(product);
                            delete_parti(sccp);
                            free(sccp_inv);
                            delete_dgraph(thescc1);
                            delete_dgraph(thescc2);
                            delete_dgraph(thesccp);
                            delete_dgraph(quad);
                            delete_parti(sccq);
                            CONCLUSION_NEGATIVE(out);
                            return false;
                        }
                    }
                    delete_dgraph(thesccp);
                    delete_dgraph(quad);
                    delete_parti(sccq);
                }
                delete_dgraph(thescc2);
                delete_dgraph(product);
                delete_parti(sccp);
                free(sccp_inv);
            }
            delete_dgraph(thescc1);
        }
        CONCLUSION_POSITIVE(out);
        return true;
    }

    // We now know that the mode is BA_MOD, BA_AMT or BA_GR
    for (uint i1 = 0; i1 < A->sccs->size_par; i1++)
    {
        // If the SCC is trivial (one state and no loop), it cannot yield a counterexample and we skip it.
        if (!dgraph_ntrivial_scc(A->trans, A->sccs, i1))
        {
            continue;
        }
        // We extract the SCC
        dgraph *thescc1 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i1);

        uint start = GSTART(A->tclos, i1);
        uint end = GEND(A->tclos, i1);
        for (uint n = start; n < end; n++)
        {
            uint i2 = A->tclos->storage[n];
            if (!dgraph_ntrivial_scc(A->trans, A->sccs, i2))
            {
                continue;
            }
            dgraph *thescc2 = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i2);
            dgraph *product = dgraph_direct_product(thescc1, thescc2);
            parti *sccp = dtarjan(product, NULL, false);
            uint *sccp_inv = parti_compute_inv(sccp);

            for (uint j = 0; j < sccp->size_par; j++)
            {
                uint t = sccp->cl_elems[j][0];
                uint t1 = t / thescc2->size_graph;
                uint t2 = t % thescc2->size_graph;
                if (A->sccs->cl_elems[i1][t1] == A->sccs->cl_elems[i2][t2] || !dgraph_ntrivial_loop(product, sccp, t))
                {
                    continue;
                }
                dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
                // view_dgraph(thesccp);
                dgraph *quad = dgraph_direct_product(thesccp, thesccp);
                parti *sccq = dtarjan(quad, NULL, false);
                uint *sccq_inv = parti_compute_inv(sccq);
                // view_dgraph(quad);
                for (uint h = 0; h < sccq->size_par; h++)
                {

                    if (!dgraph_ntrivial_scc(quad, sccq, h))
                    {
                        continue;
                    }
                    // The state in the quad graph
                    uint qstate = sccq->cl_elems[h][0];

                    // Decode the states in thesccp
                    uint x0x2 = qstate / thesccp->size_graph;
                    uint x1x3 = qstate % thesccp->size_graph;

                    uint x0 = sccp->cl_elems[j][x0x2] / thescc2->size_graph;
                    uint x2 = sccp->cl_elems[j][x0x2] % thescc2->size_graph;
                    uint x1 = sccp->cl_elems[j][x1x3] / thescc2->size_graph;
                    uint x3 = sccp->cl_elems[j][x1x3] % thescc2->size_graph;

                    dgraph *myscc = dgraph_extract(quad, sccq, sccq_inv, h);
                    parti *myfold = dgraph_stal_fold(myscc, NULL, mode);
                    dgraph *myfold_g = dgraph_implement_fold(myscc, NULL, myfold);
                    uint sl = myfold->numcl[0];
                    delete_dgraph(myscc);
                    delete_parti(myfold);

                    if (dgraph_exists_intersec_path(thesccp, myfold_g, x0x2, sl, x1x3, sl, true, NULL) &&
                        dgraph_exists_intersec_path(thesccp, A->trans, x1x3, A->sccs->cl_elems[i1][x0], x0x2, A->sccs->cl_elems[i2][x2], true, NULL))
                    {
                        if (out)
                        {
                            generic_pattern *mypattern = make_generic_pattern();
                            mypattern->states[0] = A->sccs->cl_elems[i1][x0];
                            mypattern->states[1] = A->sccs->cl_elems[i1][x1];
                            mypattern->states[2] = A->sccs->cl_elems[i2][x2];
                            mypattern->states[3] = A->sccs->cl_elems[i2][x3];
                            mypattern->vars[0] = 'U';
                            mypattern->vars[1] = 'v';
                            mypattern->vars[2] = 'W';
                            mypattern->types[0] = PAT_SING;
                            mypattern->types[1] = PAT_WORD;
                            mypattern->types[2] = PAT_LANG;
                            dgraph_exists_intersec_path(thesccp, myfold_g, x0x2, sl, x1x3, sl, true, &mypattern->words[0]);
                            dgraph_exists_intersec_path(thesccp, A->trans, x1x3, A->sccs->cl_elems[i1][x0], x0x2, A->sccs->cl_elems[i2][x2], true, &mypattern->words[1]);
                            view_pattern(A, mypattern, da_pattern_print);
                            free_generic_pattern(mypattern);
                        }
                        delete_dgraph(myfold_g);
                        delete_dgraph(product);
                        delete_parti(sccp);
                        free(sccp_inv);
                        delete_dgraph(thescc1);
                        delete_dgraph(thescc2);
                        delete_dgraph(thesccp);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        free(sccq_inv);
                        CONCLUSION_NEGATIVE(out);
                        return false;
                    }
                    delete_dgraph(myfold_g);
                }
                delete_dgraph(thesccp);
                delete_dgraph(quad);
                delete_parti(sccq);
                free(sccq_inv);
            }
            delete_dgraph(thescc2);
            delete_dgraph(product);
            delete_parti(sccp);
            free(sccp_inv);
        }
        delete_dgraph(thescc1);
    }
    CONCLUSION_POSITIVE(out);
    return true;
}

bool is_upolbp2gp_opti_dfa(dfa *A, basis mode)
{

    // SCCs of the DFA
    dfa_compute_sccs(A);

    // If the mode is BA_ST, we call the dedicated function.
    if (mode == BA_ST)
    {
        bool alpha[A->trans->size_alpha];
        for (uint i = 0; i < A->sccs->size_par; i++)
        {
            // If contains only one state, skip it
            if (A->sccs->cl_size[i] <= 1)
            {
                continue;
            }
            // We extract the SCC
            dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

            // Direct product of the SCC with itself and the SCCs of the product
            dgraph *product = dgraph_direct_product(thescc, thescc);
            parti *sccp = dtarjan(product, NULL, false);

            // We look at all synchronized loops on two states of thescc.
            for (uint j = 0; j < sccp->size_par; j++)
            {
                uint qr = sccp->cl_elems[j][0];
                uint q = qr / thescc->size_graph;
                uint r = qr % thescc->size_graph;
                // printf("Checking states (%u,%u)\n", scca->cl_elems[i][q], scca->cl_elems[i][r]);
                //  printf("Checking states (%u,%u)\n", scca->cl_elems[i][q], scca->cl_elems[i][r]);
                if (q == r || !dgraph_compute_alph_scc(product, sccp, j, alpha))
                {
                    continue;
                }
                bool rtoq = dgraph_exists_path_alpha(thescc, r, q, false, alpha, NULL);
                bool qtor = dgraph_exists_path_alpha(thescc, q, r, false, alpha, NULL);
                if (!rtoq && !qtor)
                {
                    continue;
                }
                // printf("Found path\n");

                dequeue *stack = create_dequeue();
                rigins_dequeue(qr, stack);
                bool *visited;
                CALLOC(visited, product->size_graph);
                while (!isempty_dequeue(stack))
                {
                    uint st = rigpull_dequeue(stack);
                    if (visited[st])
                    {
                        continue;
                    }
                    visited[st] = true;
                    uint s = st / thescc->size_graph;
                    uint t = st % thescc->size_graph;
                    // printf("Visiting states (%u,%u)\n", scca->cl_elems[i][s], scca->cl_elems[i][t]);
                    for (uint a = 0; a < A->trans->size_alpha; a++)
                    {
                        uint news = thescc->edges[s][a];
                        uint newt = thescc->edges[t][a];
                        // printf("  On letter %u to (%u,%u)\n", a, news == UINT_MAX ? news : scca->cl_elems[i][news], newt == UINT_MAX ? newt : scca->cl_elems[i][newt]);
                        if (qtor && newt != UINT_MAX && news == UINT_MAX)
                        {
                            delete_dgraph(product);
                            delete_parti(sccp);
                            delete_dgraph(thescc);
                            free(visited);
                            delete_dequeue(stack);
                            return false;
                        }
                        else if (rtoq && news != UINT_MAX && newt == UINT_MAX)
                        {
                            delete_dgraph(product);
                            delete_parti(sccp);
                            delete_dgraph(thescc);
                            free(visited);
                            delete_dequeue(stack);
                            return false;
                        }
                        else if (news != UINT_MAX && newt != UINT_MAX)
                        {

                            // printf("  Following letter %u to (%u,%u)\n", a, scca->cl_elems[i][news], scca->cl_elems[i][newt]);
                            uint newst = news * thescc->size_graph + newt;
                            rigins_dequeue(newst, stack);
                        }
                    }
                }
                free(visited);
                delete_dequeue(stack);
            }

            uint *sccp_inv = parti_compute_inv(sccp);

            for (uint j = 0; j < sccp->size_par; j++)
            {
                uint t = sccp->cl_elems[j][0];
                uint t1 = t / thescc->size_graph;
                uint t2 = t % thescc->size_graph;
                if (t1 == t2 || !dgraph_ntrivial_loop(product, sccp, t))
                {
                    continue;
                }
                dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
                // view_dgraph(thesccp);
                dgraph *quad = dgraph_direct_product(thesccp, thesccp);
                parti *sccq = dtarjan(quad, NULL, false);
                // view_dgraph(quad);
                for (uint h = 0; h < sccq->size_par; h++)
                {

                    if (!dgraph_compute_alph_scc(quad, sccq, h, alpha))
                    {
                        continue;
                    }
                    // The state in the quad graph
                    uint qstate = sccq->cl_elems[h][0];

                    // Decode the states in thesccp
                    uint x0x2 = qstate / thesccp->size_graph;
                    uint x1x3 = qstate % thesccp->size_graph;

                    uint x0 = sccp->cl_elems[j][x0x2] / thescc->size_graph;
                    uint x2 = sccp->cl_elems[j][x0x2] % thescc->size_graph;

                    if (dgraph_exists_path_alpha(thesccp, x0x2, x1x3, true, alpha, NULL) && dgraph_exists_intersec_path(thesccp, thescc, x1x3, x0, x0x2, x2, true, NULL))
                    {
                        delete_dgraph(product);
                        delete_parti(sccp);
                        free(sccp_inv);
                        delete_dgraph(thescc);
                        delete_dgraph(thesccp);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        return false;
                    }
                }
                delete_dgraph(thesccp);
                delete_dgraph(quad);
                delete_parti(sccq);
            }
            free(sccp_inv);
            delete_dgraph(product);
            delete_parti(sccp);
            delete_dgraph(thescc);
        }
        return true;
    }

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // If contains only one state, skip it
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }
        // We extract the SCC
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

        // Direct product of the SCC with itself and the SCCs of the product
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);
        uint *sccp_inv = parti_compute_inv(sccp);
        // We look at all synchronized loops on two states of thescc.
        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / thescc->size_graph;
            uint r = qr % thescc->size_graph;
            // printf("Checking states (%u,%u)\n", A->sccs->cl_elems[i][q], A->sccs->cl_elems[i][r]);
            //  printf("Checking states (%u,%u)\n", A->sccs->cl_elems[i][q], A->sccs->cl_elems[i][r]);
            if (q == r || !dgraph_ntrivial_scc(product, sccp, j))
            {
                continue;
            }

            dgraph *theloop = dgraph_extract(product, sccp, sccp_inv, j);
            parti *thefold = dgraph_stal_fold(theloop, NULL, mode);
            dgraph *thefold_g = dgraph_implement_fold(theloop, NULL, thefold);
            delete_dgraph(theloop);

            bool rtoq = dgraph_exists_intersec_path(thefold_g, thescc, thefold->numcl[0], r, thefold->numcl[0], q, false, NULL);
            bool qtor = dgraph_exists_intersec_path(thefold_g, thescc, thefold->numcl[0], q, thefold->numcl[0], r, false, NULL);
            delete_dgraph(thefold_g);
            delete_parti(thefold);
            if (!rtoq && !qtor)
            {
                continue;
            }
            // printf("Found path\n");

            dequeue *stack = create_dequeue();
            rigins_dequeue(qr, stack);
            bool *visited;
            CALLOC(visited, product->size_graph);
            while (!isempty_dequeue(stack))
            {
                uint st = rigpull_dequeue(stack);
                if (visited[st])
                {
                    continue;
                }
                visited[st] = true;
                uint s = st / thescc->size_graph;
                uint t = st % thescc->size_graph;
                // printf("Visiting states (%u,%u)\n", scca->cl_elems[i][s], scca->cl_elems[i][t]);
                for (uint a = 0; a < A->trans->size_alpha; a++)
                {
                    uint news = thescc->edges[s][a];
                    uint newt = thescc->edges[t][a];
                    // printf("  On letter %u to (%u,%u)\n", a, news == UINT_MAX ? news : scca->cl_elems[i][news], newt == UINT_MAX ? newt : scca->cl_elems[i][newt]);
                    if (qtor && newt != UINT_MAX && news == UINT_MAX)
                    {
                        delete_dgraph(product);
                        delete_parti(sccp);
                        delete_dgraph(thescc);
                        free(visited);
                        delete_dequeue(stack);
                        return false;
                    }
                    else if (rtoq && news != UINT_MAX && newt == UINT_MAX)
                    {
                        delete_dgraph(product);
                        delete_parti(sccp);
                        delete_dgraph(thescc);
                        free(visited);
                        delete_dequeue(stack);
                        return false;
                    }
                    else if (news != UINT_MAX && newt != UINT_MAX)
                    {

                        // printf("  Following letter %u to (%u,%u)\n", a, scca->cl_elems[i][news], scca->cl_elems[i][newt]);
                        uint newst = news * thescc->size_graph + newt;
                        rigins_dequeue(newst, stack);
                    }
                }
            }
            free(visited);
            delete_dequeue(stack);
        }

        // printf("Finished first pass on SCC %u\n", i);

        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint t = sccp->cl_elems[j][0];
            uint t1 = t / thescc->size_graph;
            uint t2 = t % thescc->size_graph;
            if (t1 == t2 || !dgraph_ntrivial_loop(product, sccp, t))
            {
                continue;
            }
            dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
            // view_dgraph(thesccp);
            dgraph *quad = dgraph_direct_product(thesccp, thesccp);
            parti *sccq = dtarjan(quad, NULL, false);
            uint *sccq_inv = parti_compute_inv(sccq);
            // view_dgraph(quad);
            for (uint h = 0; h < sccq->size_par; h++)
            {

                if (!dgraph_ntrivial_scc(quad, sccq, h))
                {
                    continue;
                }
                // The state in the quad graph
                uint qstate = sccq->cl_elems[h][0];

                // Decode the states in thesccp
                uint x0x2 = qstate / thesccp->size_graph;
                uint x1x3 = qstate % thesccp->size_graph;

                uint x0 = sccp->cl_elems[j][x0x2] / thescc->size_graph;
                uint x2 = sccp->cl_elems[j][x0x2] % thescc->size_graph;

                dgraph *myscc = dgraph_extract(quad, sccq, sccq_inv, h);
                parti *myfold = dgraph_stal_fold(myscc, NULL, mode);
                dgraph *myfold_g = dgraph_implement_fold(myscc, NULL, myfold);
                uint sl = myfold->numcl[0];
                delete_dgraph(myscc);
                delete_parti(myfold);

                if (dgraph_exists_intersec_path(thesccp, myfold_g, x0x2, sl, x1x3, sl, true, NULL) && dgraph_exists_intersec_path(thesccp, thescc, x1x3, x0, x0x2, x2, true, NULL))
                {
                    delete_dgraph(myfold_g);
                    free(sccq_inv);
                    delete_dgraph(product);
                    delete_parti(sccp);
                    free(sccp_inv);
                    delete_dgraph(thescc);
                    delete_dgraph(thesccp);
                    delete_dgraph(quad);
                    delete_parti(sccq);
                    return false;
                }
                delete_dgraph(myfold_g);
            }
            free(sccq_inv);
            delete_dgraph(thesccp);
            delete_dgraph(quad);
            delete_parti(sccq);
        }
        free(sccp_inv);
        delete_dgraph(product);
        delete_parti(sccp);
        delete_dgraph(thescc);
    }
    return true;
}

bool is_upolbp2gpplus_dfa(dfa *A, basis mode, int *error, FILE *out)
{
    if (out)
    {
        switch (mode)
        {
        case BA_ST:
            MAKE_PATTERN_INTRO(UPOL3_PATTERN_ST_PLUS, out);
            break;
        case BA_MOD:
            MAKE_PATTERN_INTRO(UPOL3_PATTERN_MOD_PLUS, out);
            break;
        case BA_AMT:
            MAKE_PATTERN_INTRO(UPOL3_PATTERN_AMT_PLUS, out);
            break;
        case BA_GR:
            MAKE_PATTERN_INTRO(UPOL3_PATTERN_GR_PLUS, out);
            break;
        default:
            break;
        }
    }
    uint *funs;
    morphism *M = dfa_to_morphism(A, error, &funs);
    orbits *bpolorbs = compute_bpgplusorbits(M, LV_GREG, mode);
    uint c[4];

    if (is_upbp_mono(bpolorbs, c))
    {
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        CONCLUSION_POSITIVE(out);
        return true;
    }
    else
    {
        if (out)
        {
            uint ese = c[0];
            uint t = c[1];
            uint f = c[3];
            uint ftf = mor_mult_gen(M, 3, f, t, f);

            uint qi = 0;
            while (qi < A->trans->size_graph)
            {
                if (funs[A->trans->size_graph * f + qi] != funs[A->trans->size_graph * ftf + qi])
                {
                    // printf("distinct states: %u and %u\n", funs[A->trans->size_graph * f + qi], funs[A->trans->size_graph * ftf + qi]);
                    break;
                }
                qi++;
            }

            uint q0 = funs[A->trans->size_graph * f + qi];
            uint q1 = funs[A->trans->size_graph * ese + q0];
            uint q2 = funs[A->trans->size_graph * ftf + qi];
            uint q3 = funs[A->trans->size_graph * ese + q2];

            generic_pattern *mypattern = make_generic_pattern();
            mypattern->states[0] = q0;
            mypattern->states[1] = q1;
            mypattern->states[2] = q2;
            mypattern->states[3] = q3;
            mypattern->vars[0] = 'U';
            mypattern->vars[1] = 'v';
            mypattern->vars[2] = 'W';
            mypattern->types[0] = PAT_LANG;
            mypattern->types[1] = PAT_WORD;
            mypattern->types[2] = PAT_LANG;
            mypattern->words[1] = word_from_morphism(M, t);
            view_pattern(A, mypattern, da_pattern_print);
            free_generic_pattern(mypattern);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        CONCLUSION_NEGATIVE(out);
        return false;
    }
}

bool is_upolbp2gpplus_opti_dfa(dfa *A, basis mode)
{

    // SCCs of the DFA
    dfa_compute_sccs(A);

    for (uint i = 0; i < A->sccs->size_par; i++)
    {
        // If contains only one state, skip it
        if (A->sccs->cl_size[i] <= 1)
        {
            continue;
        }
        // We extract the SCC
        dgraph *thescc = dgraph_extract(A->trans, A->sccs, A->sccs_inv, i);

        // Direct product of the SCC with itself and the SCCs of the product
        dgraph *product = dgraph_direct_product(thescc, thescc);
        parti *sccp = dtarjan(product, NULL, false);
        uint *sccp_inv = parti_compute_inv(sccp);

        // We look at all synchronized loops on two states of thescc.
        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / thescc->size_graph;
            uint r = qr % thescc->size_graph;
            // printf("Checking states (%u,%u)\n", scca->cl_elems[i][q], scca->cl_elems[i][r]);
            //  printf("Checking states (%u,%u)\n", scca->cl_elems[i][q], scca->cl_elems[i][r]);
            if (q == r || !dgraph_ntrivial_scc(product, sccp, j))
            {
                continue;
            }

            // So we extract the SCC of the product graph containing (q,r).
            dgraph *theloop = dgraph_extract(product, sccp, sccp_inv, j);

            dgraph *thelooptwo = dgraph_direct_product(theloop, theloop);
            dgraph *theloopprod = dgraph_direct_product(thescc, thelooptwo);
            parti *scc_loopprod = dtarjan(theloopprod, NULL, false);
            delete_dgraph(thelooptwo);

            parti *thefold = mode == BA_ST ? NULL : dgraph_stal_fold(theloop, NULL, mode);

            bool rtoq = dgraph_exists_intersec_path_pgplus(thescc, theloop, r, 0, q, 0, false, theloopprod, scc_loopprod, thefold);
            bool qtor = dgraph_exists_intersec_path_pgplus(thescc, theloop, q, 0, r, 0, false, theloopprod, scc_loopprod, thefold);
            if (!rtoq && !qtor)
            {
                continue;
            }
            delete_dgraph(theloop);
            delete_dgraph(theloopprod);
            delete_parti(scc_loopprod);
            delete_parti(thefold);

            dequeue *stack = create_dequeue();
            rigins_dequeue(qr, stack);
            bool *visited;
            CALLOC(visited, product->size_graph);
            while (!isempty_dequeue(stack))
            {
                uint st = rigpull_dequeue(stack);
                if (visited[st])
                {
                    continue;
                }
                visited[st] = true;
                uint s = st / thescc->size_graph;
                uint t = st % thescc->size_graph;
                // printf("Visiting states (%u,%u)\n", scca->cl_elems[i][s], scca->cl_elems[i][t]);
                for (uint a = 0; a < A->trans->size_alpha; a++)
                {
                    uint news = thescc->edges[s][a];
                    uint newt = thescc->edges[t][a];
                    // printf("  On letter %u to (%u,%u)\n", a, news == UINT_MAX ? news : scca->cl_elems[i][news], newt == UINT_MAX ? newt : scca->cl_elems[i][newt]);
                    if (qtor && newt != UINT_MAX && news == UINT_MAX)
                    {
                        delete_dgraph(product);
                        delete_parti(sccp);
                        delete_dgraph(thescc);
                        free(visited);
                        delete_dequeue(stack);
                        return false;
                    }
                    else if (rtoq && news != UINT_MAX && newt == UINT_MAX)
                    {
                        delete_dgraph(product);
                        delete_parti(sccp);
                        delete_dgraph(thescc);
                        free(visited);
                        delete_dequeue(stack);
                        return false;
                    }
                    else if (news != UINT_MAX && newt != UINT_MAX)
                    {

                        // printf("  Following letter %u to (%u,%u)\n", a, scca->cl_elems[i][news], scca->cl_elems[i][newt]);
                        uint newst = news * thescc->size_graph + newt;
                        rigins_dequeue(newst, stack);
                    }
                }
            }
            free(visited);
            delete_dequeue(stack);
        }

        for (uint j = 0; j < sccp->size_par; j++)
        {
            uint t = sccp->cl_elems[j][0];
            uint t1 = t / thescc->size_graph;
            uint t2 = t % thescc->size_graph;
            if (t1 == t2 || !dgraph_ntrivial_loop(product, sccp, t))
            {
                continue;
            }
            dgraph *thesccp = dgraph_extract(product, sccp, sccp_inv, j);
            // view_dgraph(thesccp);
            dgraph *quad = dgraph_direct_product(thesccp, thesccp);
            parti *sccq = dtarjan(quad, NULL, false);
            uint *sccq_inv = parti_compute_inv(sccq);
            // view_dgraph(quad);
            for (uint h = 0; h < sccq->size_par; h++)
            {

                if (!dgraph_ntrivial_scc(quad, sccq, h))
                {
                    continue;
                }

                // The state in the quad graph
                uint qstate = sccq->cl_elems[h][0];

                // Decode the states in thesccp
                uint x0x2 = qstate / thesccp->size_graph;
                uint x1x3 = qstate % thesccp->size_graph;

                uint x0 = sccp->cl_elems[j][x0x2] / thescc->size_graph;
                uint x2 = sccp->cl_elems[j][x0x2] % thescc->size_graph;

                if (!dgraph_exists_intersec_path(thesccp, thescc, x1x3, x0, x0x2, x2, true, NULL))
                {
                    continue;
                }

                // Extraction of the loop SCC
                dgraph *theloop = dgraph_extract(quad, sccq, sccq_inv, h);
                parti *thefold = dgraph_stal_fold(theloop, NULL, mode);
                dgraph *thelooptwo = dgraph_direct_product(theloop, theloop);
                dgraph *theloopprod = dgraph_direct_product(thesccp, thelooptwo);
                parti *scc_loopprod = dtarjan(theloopprod, NULL, false);
                delete_dgraph(thelooptwo);

                if (dgraph_exists_intersec_path_pgplus(thesccp, theloop, x0x2, 0, x1x3, 0, false, theloopprod, scc_loopprod, thefold))
                {
                    delete_dgraph(product);
                    delete_parti(sccp);
                    free(sccp_inv);
                    delete_dgraph(thescc);
                    delete_dgraph(thesccp);
                    delete_dgraph(quad);
                    delete_parti(sccq);
                    delete_dgraph(theloop);
                    delete_dgraph(theloopprod);
                    delete_parti(scc_loopprod);
                    delete_parti(thefold);
                    return false;
                }
                delete_dgraph(theloop);
                delete_parti(thefold);
                delete_dgraph(theloopprod);
                delete_parti(scc_loopprod);
            }
            delete_dgraph(thesccp);
            delete_dgraph(quad);
            delete_parti(sccq);
            free(sccq_inv);
        }
        free(sccp_inv);
        delete_dgraph(product);
        delete_parti(sccp);
        delete_dgraph(thescc);
    }
    return true;
}

/************************************************/
/*+ Additional ad-hoc properties for filtering +*/
/************************************************/

bool is_nosink_dfa(dfa *A)
{

    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        bool is_sink = true;
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            uint r = A->trans->edges[q][a];
            if (r != q)
            {
                is_sink = false;
                break;
            }
        }
        if (is_sink)
        {
            return false;
        }
    }
    return true;
}

bool is_noselfloops_dfa(dfa *A)
{

    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            if (A->trans->edges[q][a] == q)
            {
                return false;
            }
        }
    }
    return true;
}
