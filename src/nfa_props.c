#include "nfa_props.h"
#include "nfa_intersec.h"
#include "monoid_display.h"
#include "limits.h"
#include "nfa_patterns.h"
#include "nfa_minimization.h"
#include "printing.h"

/***********************/
/* Auxiliary functions */
/***********************/



uint* dfacexa_get_alphaloop(dgraph* g, parti* sccs, bool* alph, uint s) {
    uint* inv_sccs = parti_compute_inv(sccs);
    dgraph* local = dgraph_extract(g, sccs, inv_sccs, sccs->numcl[s]);
    uint* words[g->size_alpha];

    uint t = inv_sccs[s];
    uint length = 0;
    uint i = 0;

    // We seek a path including all letters in the alphabet
    for (uint a = 0; a < g->size_alpha; a++) {
        words[a] = NULL;
        if (!alph[a]) {
            continue; // We only need letters in the given alphabet.
        }
        //printf("Letter %u\n", a);
        t = dgraph_exists_path_letter_alpha(local, t, a, alph, &words[a]);
        if (t == UINT_MAX) {
            free(local);
            for (uint c = 0; c <= a; c++) {
                free(words[c]);
            }
            return NULL; // No path found for this letter
        }
        //printf("Found path for letter %u: \n", a);
        t = local->edges[t][a];
        i = 0;
        while (words[a][i] != UINT_MAX) {
            i++;
        }
        length += (i + 1); // +1 for the letter itself
    }

    // We go back to the starting state
    uint* end;
    if (!dgraph_exists_path(local, t, inv_sccs[s], false, &end)) {
        free(local);
        for (uint a = 0; a < g->size_alpha; a++) {
            free(words[a]);
        }
        return NULL; // No path found to the starting state
    }
    i = 0;
    while (end[i] != UINT_MAX) {
        i++;
    }
    length += i;



    uint* word;
    MALLOC(word, length + 1);
    word[length] = UINT_MAX; // End of the word
    uint j = 0;
    for (uint a = 0; a < g->size_alpha; a++) {
        if (!alph[a]) {
            continue;
        }
        i = 0;
        while (words[a][i] != UINT_MAX) {
            word[j] = words[a][i];
            i++;
            j++;
        }
        word[j] = a; // Add the letter
        j++;
        free(words[a]);
    }
    i = 0;
    while (end[i] != UINT_MAX) {
        word[j] = end[i];
        i++;
        j++;
    }
    free(end);
    free(inv_sccs);

    delete_dgraph(local);
    return word; // Return the word that contains all letters in the alphabet
}



/***********/
/* Trivial */
/***********/

// Pour l'automate minimal
bool is_trivial_dfa(dfa* A, FILE* out) {
    if (A->trans->size_graph == 1) {
        if (out != NULL) {
            fprintf(out, "#### This automaton has a single state.\n");
        }
        return true;
    }
    else {
        if (out != NULL) {
            fprintf(out, "#### This automaton has more than one state.\n");
        }
        return false;
    }
}

bool is_ddtrivial_dfa(dfa* A, FILE* out) {
    if (A->trans->size_graph == 1) {
        return true;
    }

    if (A->trans->size_graph == 2) {
        uint q = A->initial;
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            if (A->trans->edges[q][a] == q) {
                if (out) {
                    for (uint b = 0; b < A->trans->size_alpha; b++) {
                        if (A->trans->edges[q][b] != q) {
                            uint states[4] = { q, q, q, A->trans->edges[q][b] };
                            char vars[2] = { 'u', 'v' };
                            uint u[2] = { a, UINT_MAX };
                            uint v[2] = { b, UINT_MAX };

                            uint* words[2] = {
                                u,
                                v,
                            };
                            view_pattern_example(A, 4, states, 2, vars, words, false, out);
                            return false;
                        }
                    }
                }
                return false;
            }
        }
        return true;
    }

    if (out) {
        uint q = A->initial;
        uint r = A->trans->edges[q][0];
        for (uint a = 1; a < A->trans->size_alpha; a++) {
            if (A->trans->edges[q][a] != r) {
                uint states[4] = { q, r, q, A->trans->edges[q][a] };
                char vars[2] = { 'u', 'v' };
                uint u[2] = { 0, UINT_MAX };
                uint v[2] = { a, UINT_MAX };

                uint* words[2] = {
                    u,
                    v,
                };
                view_pattern_example(A, 4, states, 2, vars, words, false, out);
                return false;
            }
        }


        for (uint a = 0; a < A->trans->size_alpha; a++) {
            if (A->trans->edges[r][a] != r) {
                uint states[4] = { q, r, r, A->trans->edges[r][a] };
                char vars[2] = { 'u', 'v' };
                uint u[2] = { 0, UINT_MAX };
                uint v[2] = { a, UINT_MAX };

                uint* words[2] = {
                    u,
                    v,
                };
                view_pattern_example(A, 4, states, 2, vars, words, false, out);
                return false;
            }
        }


    }


    return false;
}






// Pour l'automate minimal
bool is_letterind_dfa(dfa* A, int*, FILE* out) {
    for (uint s = 0; s < A->trans->size_graph; s++) {
        for (uint a = 1; a < A->trans->size_alpha; a++) {
            if (A->trans->edges[s][0] != A->trans->edges[s][a]) {
                if (out) {
                    fprintf(out, "#### The DFA contains the transitions ");
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
    return true;
}



bool is_permutation_dfa(dfa* A, int*, FILE* out) {

    uint* reached;
    MALLOC(reached, A->trans->size_graph);

    for (uint a = 0; a < A->trans->size_alpha; a++) {
        for (uint q = 0; q < A->trans->size_graph; q++) {
            reached[q] = UINT_MAX;
        }

        for (uint q = 0; q < A->trans->size_graph; q++) {
            uint r = A->trans->edges[q][a];
            if (reached[r] == UINT_MAX) {
                reached[r] = q;
            }
            else {
                if (out) {
                    uint states[3] = { r, reached[r], q };
                    char vars[1] = { 'w' };
                    uint w[2] = { a, UINT_MAX };
                    uint* words[1] = { w };
                    view_pattern_example(A, 3, states, 1, vars, words, false, out);
                }
                free(reached);
                return false;
            }
        }
    }
    free(reached);
    return true;
}



bool is_permutationplus_dfa(dfa* A, int*, FILE* out) {
    uint* reached;
    MALLOC(reached, A->trans->size_graph);

    bool iniprec = false;
    for (uint a = 0; a < A->trans->size_alpha; a++) {
        for (uint q = 0; q < A->trans->size_graph; q++) {
            if (A->trans->edges[q][a] == A->initial) {
                iniprec = true;
            }
        }
    }

    for (uint a = 0; a < A->trans->size_alpha; a++) {
        for (uint q = 0; q < A->trans->size_graph; q++) {
            reached[q] = UINT_MAX;
        }

        for (uint q = 0; q < A->trans->size_graph; q++) {
            if (!iniprec && q == A->initial) {
                continue;
            }

            uint r = A->trans->edges[q][a];

            if (reached[r] == UINT_MAX) {
                reached[r] = q;
                continue;
            }


            if (out) {
                uint states[4] = { A->initial, reached[r], q, r };
                uint* u;
                uint* v;
                dgraph_exists_path(A->trans, A->initial, reached[r], true, &u);
                dgraph_exists_path(A->trans, A->initial, q, true, &v);
                char vars[3] = { 'u', 'v' ,'w' };
                uint w[2] = { a, UINT_MAX };
                uint* words[3] = { u, v, w };
                view_pattern_example(A, 4, states, 3, vars, words, false, out);
                free(u);
                free(v);
            }
            free(reached);
            return false;

        }
    }
    free(reached);


    for (uint q = 0; q < A->trans->size_graph; q++) {
        if (q == A->initial) {
            continue;
        }
        if (A->trans->edges[q][0] == A->trans->edges[A->initial][0]) {
            for (uint a = 1; a < A->trans->size_alpha; a++) {
                if (A->trans->edges[q][a] != A->trans->edges[A->initial][a]) {
                    if (out) {
                        uint states[5] = { A->initial, q, A->trans->edges[q][0], A->trans->edges[q][a], A->trans->edges[A->initial][a] };
                        char vars[2] = { 'v', 'w' };
                        uint u[2] = { 0, UINT_MAX };
                        uint v[2] = { a, UINT_MAX };
                        uint* words[2] = { u, v };
                        view_pattern_example(A, 5, states, 2, vars, words, false, out);
                    }
                    return false;
                }
            }
        }
        else {
            for (uint a = 1; a < A->trans->size_alpha; a++) {
                if (A->trans->edges[q][a] == A->trans->edges[A->initial][a]) {
                    if (out) {
                        uint states[5] = { A->initial, q, A->trans->edges[q][a], A->trans->edges[q][0], A->trans->edges[A->initial][0] };
                        char vars[2] = { 'v', 'w' };
                        uint u[2] = { a, UINT_MAX };
                        uint v[2] = { 0, UINT_MAX };
                        uint* words[2] = { u, v };
                        view_pattern_example(A, 5, states, 2, vars, words, false, out);
                    }
                    return false;
                }
            }
        }
    }



    return true;


}



// Tests if a DFA is commutative
bool is_comm_dfa(dfa* A, int*, FILE* out) {

    for (uint a = 0; a < A->trans->size_alpha; a++) {
        for (uint b = a + 1; b < A->trans->size_alpha; b++) {
            for (uint q = 0; q < A->trans->size_graph; q++) {
                uint qa = A->trans->edges[q][a];
                uint qb = A->trans->edges[q][b];
                uint qab = A->trans->edges[qa][b];
                uint qba = A->trans->edges[qb][a];
                if (qab != qba) {
                    if (out) {
                        uint states[5] = { q, qa, qab, qb, qba };
                        char vars[2] = { 'w', 'v' };
                        uint w[2] = { a, UINT_MAX };
                        uint v[2] = { b, UINT_MAX };

                        uint* words[2] = { w, v };

                        view_pattern_example(A, 5, states, 2, vars, words, false, out);
                    }
                    return false;
                }
            }
        }
    }
    return true;
}

// Tests if a DFA is idempotent
bool is_alphaidem_dfa(dfa* A, int*, FILE* out) {

    for (uint a = 0; a < A->trans->size_alpha; a++) {
        for (uint q = 0; q < A->trans->size_graph; q++) {
            uint r = A->trans->edges[q][a];
            uint s = A->trans->edges[r][a];
            if (r != s) {
                if (out) {
                    uint states[3] = { q, r, s };
                    char vars[1] = { 'w' };
                    uint w[2] = { a, UINT_MAX };
                    uint* words[1] = {
                        w
                    };
                    view_pattern_example(A, 3, states, 1, vars, words, false, out);
                }
                return false;
            }
        }
    }
    return true;
}


bool is_ddidem_dfa(dfa* A, int*, FILE* out) {
    parti* scca = dtarjan(A->trans, NULL, false);
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    dgraph_discard_nonscc_edges(inter, scci);

    for (uint q = 0; q < A->trans->size_graph;q++) {
        for (uint r = 0; r < A->trans->size_graph;r++) {
            uint qr = q * A->trans->size_graph + r;
            if (!dgraph_ntrivial_loop(inter, scci, qr)) {
                continue;
            }
            for (uint s = 0; s < A->trans->size_graph;s++) {
                uint qs = q * A->trans->size_graph + s;
                uint rs = r * A->trans->size_graph + s;
                if (s == r || !dgraph_ntrivial_loop(inter, scci, qs) || !dgraph_ntrivial_loop(inter, scci, rs)) {
                    continue;
                }
                uint* loop = NULL;
                if (!dgraph_exists_intersec_path(inter, inter, qr, rs, qr, rs, true, &loop)) {
                    continue;
                }

                uint* path = NULL;
                if (!dgraph_exists_intersec_path(A->trans, A->trans, q, r, r, s, true, &path)) {
                    free(loop);
                    continue;
                }

                if (out) {
                    uint states[3] = { q, r, s };
                    char vars[2] = { 'w', 'z' };
                    uint* words[2] = {
                        path,
                        loop,
                    };
                    view_pattern_example(A, 3, states, 2, vars, words, false, out);
                }

                free(loop);
                free(path);
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                return false;




            }
        }
    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}


static uint* word_from_morphism(morphism* M, uint el) {
    uint n = 0;
    uint t = el;;
    while (t != ONE) {
        n++;
        t = M->pred_ele[t];
    }
    uint* word;
    MALLOC(word, n + 1);
    word[n] = UINT_MAX; // We add a sentinel value at the end of the word
    t = el;
    while (t != ONE) {
        n--;
        word[n] = M->pred_lab[t];
        t = M->pred_ele[t];
    }
    return word;
}

static uint state_from_word(dfa* A, uint state, uint* word) {
    uint i = 0;
    while (word[i] != UINT_MAX) {
        state = A->trans->edges[state][word[i]];
        i++;
    }
    return state;
}


bool is_ddcomm_dfa(dfa* A, int* error, FILE* out) {
    uint* funs;
    morphism* M = dfa_to_morphism(A, NULL, error, &funs);
    orbits* ddorbs = compute_ddorbits(M);
    uint c[3];
    if (is_com_orbmono(ddorbs, c)) {
        delete_morphism(M);
        free(funs);
        delete_orbits(ddorbs);
        return true;
    }
    else {
        if (out) {
            uint* w = word_from_morphism(M, c[0]);
            uint* v = word_from_morphism(M, c[1]);
            uint* z = word_from_morphism(M, c[2]);

            uint el1 = mor_mult_gen(M, 5, c[2], c[0], c[2], c[1], c[2]);
            uint el2 = mor_mult_gen(M, 5, c[2], c[1], c[2], c[0], c[2]);
            uint q0 = 0, q1, q2, q3, q4;
            for (uint qi = 0; qi < A->trans->size_graph; qi++) {
                if (funs[A->trans->size_graph * el1 + qi] != funs[A->trans->size_graph * el2 + qi]) {
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

            uint states[5] = { q0, q1, q2, q3, q4 };
            char vars[3] = { 'w', 'v', 'z' };
            uint* words[3] = { w, v, z };
            view_pattern_example(A, 5, states, 3, vars, words, false, out);


            free(w);
            free(v);
            free(z);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(ddorbs);
        return false;
    }
    return true;

}


bool is_lttcomm_dfa(dfa* A, int* error, FILE* out) {
    uint* funs;
    morphism* M = dfa_to_morphism(A, NULL, error, &funs);
    orbits* ddorbs = compute_ddorbits(M);
    uint c[5];
    if (is_comm_ltt_mono(ddorbs, c)) {
        delete_morphism(M);
        free(funs);
        delete_orbits(ddorbs);
        return true;
    }
    else {
        if (out) {

            uint* u = word_from_morphism(M, c[0]);
            uint* v = word_from_morphism(M, c[1]);
            uint* w = word_from_morphism(M, c[2]);
            uint* y = word_from_morphism(M, c[3]);
            uint* z = word_from_morphism(M, c[4]);

            uint el1 = mor_mult_gen(M, 7, c[3], c[0], c[4], c[1], c[3], c[2], c[4]);
            uint el2 = mor_mult_gen(M, 7, c[3], c[2], c[4], c[1], c[3], c[0], c[4]);
            uint q0 = 0, q1, q2, q3, q4, q5, q6;
            for (uint qi = 0; qi < A->trans->size_graph; qi++) {
                if (funs[A->trans->size_graph * el1 + qi] != funs[A->trans->size_graph * el2 + qi]) {
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

            uint states[7] = { q0, q1, q2, q3, q4, q5, q6 };
            char vars[5] = { 'u', 'v', 'w', 'y', 'z' };
            uint* words[5] = { u, v, w, y, z };
            view_pattern_example(A, 7, states, 5, vars, words, false, out);


            free(u);
            free(v);
            free(w);
            free(y);
            free(z);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(ddorbs);
        return false;
    }
    return true;

}


// Generates a DFA from a single SCC of an input DFA. A sink state is added to the end of the transition graph.
static dfa* dfa_from_scc(dfa* A, parti* sccs, uint p) {
    dfa* D;
    CALLOC(D, 1);
    D->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    D->trans = create_dgraph_noedges(sccs->cl_size[p] + 1, A->trans->size_alpha);
    D->nb_finals = 0;

    for (uint i = 0; i < sccs->cl_size[p]; i++) {
        uint q = sccs->cl_elems[p][i];
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            uint r = A->trans->edges[q][a];
            if (sccs->numcl[r] != p) {
                D->trans->edges[i][a] = sccs->cl_size[p];
            }
            else {
                D->trans->edges[i][a] = ((uint*)bsearch(&r, sccs->cl_elems[p], sccs->cl_size[p], sizeof(uint), &compare_uint)) - sccs->cl_elems[p];
            }
        }
    }

    for (uint a = 0; a < A->trans->size_alpha; a++) {
        D->trans->edges[sccs->cl_size[p]][a] = sccs->cl_size[p];
    }

    D->initial = 0; // L'état initial est le premier de la liste

    return D;
}



static bool is_counterfree_dfa(dfa* A, int* error, FILE* out) {



    /* We consider each SCC in the automaton, and check that none of them contains a counter. */

    // First compute the SCCs of the DFA.
    parti* PSCCS = dtarjan(A->trans, NULL, false);

    // Consider each SCC independently.
    for (uint c = 0; c < PSCCS->size_par; c++) {
        // Compute the automaton obtained by only keeping this SCC
        dfa* D = dfa_from_scc(A, PSCCS, c);

        // We compute the associated morphism and save the function table used to construct it.
        *error = 0;

        uint* funs;
        morphism* M = dfa_to_morphism(D, NULL, error, &funs);
        if (*error < 0) {
            return false;
        }

        // On calcule ses relations de Green
        green* GREL = M->rels;

        // If there exists a non-trivial group in the automaton, then it contains a counter.
        if (GREL->HCL->size_set != GREL->HCL->size_par) {
            // If a counterexample is requested, we will print it.
            if (out) {
                uint e = 0;
                uint s = 0;
                // We look for a non-trivial group in the automaton.
                for (uint i = 0; i < M->nb_idems; i++) {
                    e = M->idem_list[i];
                    uint cl = GREL->HCL->numcl[e];
                    if (GREL->HCL->cl_size[cl] > 1) {
                        // We take an element of the class which is not an idempotent.
                        s = GREL->HCL->cl_elems[cl][0];
                        if (M->idem_array[s]) {
                            s = GREL->HCL->cl_elems[cl][1];
                        }
                        break;
                    }
                }
                uint* w = word_from_morphism(M, s);

                // We now look for the counter itself.
                uint q = 0;
                for (uint j = 0; j < D->trans->size_graph; j++) {
                    if (funs[D->trans->size_graph * e + j] == j && funs[D->trans->size_graph * s + j] != j) {
                        q = PSCCS->cl_elems[c][j];
                        break;
                    }
                }

                // The counter starts at state q, and we have the word w that leads to the counter.
                // We now compute the length of the counter.
                uint n = 0;
                uint r = state_from_word(A, q, w);
                while (r != q) {
                    n++;
                    r = state_from_word(A, r, w);
                }

                uint states[n + 1];
                states[0] = q;
                r = state_from_word(A, q, w);
                for (uint h = 1; h <= n; h++) {
                    states[h] = r;
                    r = state_from_word(A, r, w);
                }

                char vars[1] = { 'w' };
                uint* words[1] = { w };

                view_pattern_example(A, n + 1, states, 1, vars, words, true, out);

                free(w);

            }
            dfa_delete(D);
            free(funs);
            delete_morphism(M);
            return false;
        }

        dfa_delete(D);
        free(funs);
        delete_morphism(M);
    }
    return true;
}


bool is_counterfreegp_dfa(dfa* A, basis mode, int* error, FILE* out) {

    // If the basis is ST, we use the dedicated function.
    if (mode == BA_ST) {
        return is_counterfree_dfa(A, error, out);
    }
    /* We consider each SCC in the automaton, and check that none of them contains a counter. */

    // First compute the SCCs of the DFA.
    parti* PSCCS = dtarjan(A->trans, NULL, false);

    // Consider each SCC independently.
    for (uint c = 0; c < PSCCS->size_par; c++) {
        // Compute the automaton obtained by only keeping this SCC
        dfa* D = dfa_from_scc(A, PSCCS, c);

        // We compute the associated morphism and save the function table used to construct it.
        *error = 0;

        uint* funs;
        morphism* M = dfa_to_morphism(D, NULL, error, &funs);
        if (*error < 0) {
            return false;
        }

        subsemi* ker = get_kernel(M, LV_REG, mode);

        // On calcule ses relations de Green

        green* GREL = ker->rels;

        // If there exists a non-trivial group, then the automaton contains a G-counter.
        if (GREL->HCL->size_set != GREL->HCL->size_par) {
            // If a counterexample is requested, we will print it.
            if (out) {
                uint e = 0;
                uint s = 0;

                // We look for a non-trivial group in the automaton.
                for (uint i = 0; i < M->nb_idems; i++) {
                    e = M->idem_list[i];
                    uint sube = ker->mono_to_sub[e];
                    uint cl = GREL->HCL->numcl[sube];
                    if (GREL->HCL->cl_size[cl] > 1) {
                        // We take an element of the class which is not an idempotent.
                        uint subs = GREL->HCL->cl_elems[cl][0];
                        if (ker->idem_array[subs]) {
                            subs = GREL->HCL->cl_elems[cl][1];
                        }
                        s = ker->sub_to_mono[subs];
                        break;
                    }
                }

                uint* w = word_from_morphism(M, s);

                // We now look for the counter itself.
                uint q = 0;
                for (uint j = 0; j < D->trans->size_graph; j++) {
                    if (funs[D->trans->size_graph * e + j] == j && funs[D->trans->size_graph * s + j] != j) {
                        q = PSCCS->cl_elems[c][j];
                        break;
                    }
                }

                // The counter starts at state q, and we have the word w that leads to the counter.
                // We now compute the length of the counter.
                uint n = 0;
                uint r = state_from_word(A, q, w);
                while (r != q) {
                    n++;
                    r = state_from_word(A, r, w);
                }

                uint states[n + 1];
                states[0] = q;
                r = state_from_word(A, q, w);
                for (uint h = 1; h <= n; h++) {
                    states[h] = r;
                    r = state_from_word(A, r, w);
                }


                view_pattern_example(A, n + 1, states, 0, NULL, NULL, true, out);

                free(w);

            }
            dfa_delete(D);
            free(funs);
            delete_morphism(M);
            return false;
        }
        dfa_delete(D);
        free(funs);
        delete_morphism(M);
    }
    return true;
}











bool is_nosmallcountertwo_dfa(dfa* A) {
    parti* scca = dtarjan(A->trans, NULL, false);
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);

    for (uint i = 0; i < scca->size_par; i++) {
        if (scca->cl_size[i] == 1) {
            continue;
        }

        for (uint j = 0; j < scca->cl_size[i]; j++) {
            uint q = scca->cl_elems[i][j];
            for (uint h = j + 1; h < scca->cl_size[i]; h++) {
                uint r = scca->cl_elems[i][h];
                if (scci->numcl[q + r * A->trans->size_graph] == scci->numcl[r + q * A->trans->size_graph]) {
                    delete_dgraph(inter);
                    delete_parti(scca);
                    delete_parti(scci);
                    return false;
                }
            }
        }
    }

    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}



bool is_nosimple_counter_dfa(dfa* A, int*) {
    bool alpha[A->trans->size_alpha];
    for (uint a = 0; a < A->trans->size_alpha; a++) {
        alpha[a] = false;
    }
    for (uint a = 0; a < A->trans->size_alpha; a++) {
        alpha[a] = true;
        parti* sccs = dtarjan(A->trans, alpha, false);
        if (sccs->size_par != sccs->size_set) {
            // If the number of SCCs is not equal to the number of states, then there is a counter.
            delete_parti(sccs);
            return false;
        }
        delete_parti(sccs);
        alpha[a] = false;
    }

    return true;
}

/*******/
/* Pol */
/*******/

static bool is_ppt_dfa(dfa* A, int*, FILE* out) {
    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            // If the pattern equation is satisfied we move to the next transition
            if (A->order[q][A->trans->edges[q][a]]) {
                continue;
            }

            // If the pattern equation is not satisfied, we print a counterexample if requested
            if (out) {
                uint states[2] = { q, A->trans->edges[q][a] };
                char vars[1] = { 'w' };
                uint w[2] = { a, UINT_MAX };
                uint* words[1] = { w };
                view_pattern_example(A, 2, states, 1, vars, words, false, out);
            }
            return false;
        }
    }
    return true;
}

static bool is_polmod_dfa(dfa* A, int* error, FILE* out) {
    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    uint* funs;
    morphism* M = dfa_to_morphism(A, true, error, &funs);
    subsemi* mkernel = get_kernel(M, LV_GREG, BA_MOD);
    uint c[1];
    if (is_jsat_subsemi(mkernel, ONE, c)) {
        delete_morphism(M);
        free(funs);
        delete_subsemi(mkernel);
        return true;
    }
    else {
        if (out) {

            uint* w = word_from_morphism(M, c[0]);

            uint el = c[0];
            uint q0 = ONE;
            for (uint qi = 0; qi < A->trans->size_graph; qi++) {
                if (!A->order[funs[A->trans->size_graph * ONE + qi]][funs[A->trans->size_graph * el + qi]]) {
                    q0 = qi;
                    break;
                }
            }

            uint q1 = state_from_word(A, q0, w);

            uint states[2] = { q0, q1 };
            view_pattern_example(A, 2, states, 0, NULL, NULL, false, out);


            free(w);
        }
        delete_morphism(M);
        free(funs);
        delete_subsemi(mkernel);
        return false;
    }
    return true;
}

static bool is_polgr_dfa(dfa* A, int*, FILE* out) {
    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint r = 0; r < A->trans->size_graph; r++) {
            if (A->order[q][r]) {
                continue;
            }

            uint* word;
            if (dgraph_exists_intersec_path(A->trans, A->trans, q, r, r, r, false, &word)) {
                if (out) {
                    uint states[2] = { q, r };
                    char vars[1] = { 'w' };
                    uint* words[1] = { word };
                    view_pattern_example(A, 2, states, 1, vars, words, false, out);
                }
                free(word);
                return false;
            }
        }
    }
    return true;
}

bool is_polgp_dfa(dfa* A, basis mode, int* error, FILE* out) {
    switch (mode)
    {
    case BA_ST:
        return is_ppt_dfa(A, error, out);
        break;
    case BA_MOD:
        return is_polmod_dfa(A, error, out);
        break;
    case BA_GR:
        return is_polgr_dfa(A, error, out);
        break;
    default:
        fprintf(stderr, "Error: Invalid mode for is_polgp_dfa.\n");
        *error = -1;
        exit(EXIT_FAILURE);
        break;
    }
}


static bool is_poldd_dfa(dfa* A, int*, FILE* out) {
    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);

    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint r = 0; r < A->trans->size_graph; r++) {
            uint qr = q * A->trans->size_graph + r;

            if (!dgraph_ntrivial_loop(inter, scci, qr) || A->order[q][r]) {
                continue;
            }

            uint* w;
            if (dgraph_exists_path(A->trans, q, r, true, &w)) {
                if (out) {
                    uint states[2] = { q, r };
                    char vars[2] = { 'w', 'z' };
                    uint* z;
                    uint p = q * A->trans->size_graph + r;
                    dgraph_exists_path(inter, p, p, true, &z);
                    uint* words[2] = { w, z };
                    view_pattern_example(A, 2, states, 2, vars, words, false, out);
                    free(z);
                }
                free(w);
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                return false;
            }
        }
    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;
}

bool is_polgpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_poldd_dfa(A, error, out);
    }

    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    uint* funs;
    morphism* M = dfa_to_morphism(A, true, error, &funs);
    subsemi* kernel = get_kernel(M, LV_GREG, mode);
    orbits* gplusorbs = compute_gplusorbits(kernel);
    uint c[2];
    if (is_jsat_orbmono(gplusorbs, c)) {
        delete_morphism(M);
        free(funs);
        delete_subsemi(kernel);
        delete_orbits(gplusorbs);
        return true;
    }
    else {
        if (out) {

            uint* w = word_from_morphism(M, c[0]);
            uint* z = word_from_morphism(M, c[1]);

            uint el = c[0];
            uint elz = c[1];
            uint q0 = ONE;
            for (uint qi = 0; qi < A->trans->size_graph; qi++) {
                if (!A->order[funs[A->trans->size_graph * elz + qi]][funs[A->trans->size_graph * el + qi]]) {
                    q0 = qi;
                    break;
                }
            }

            q0 = state_from_word(A, q0, z);
            uint q1 = state_from_word(A, q0, w);
            q1 = state_from_word(A, q1, z);

            uint states[2] = { q0, q1 };
            char vars[1] = { 'z' };
            uint* words[1] = { z };
            view_pattern_example(A, 2, states, 1, vars, words, false, out);

            free(z);
            free(w);
        }
        delete_morphism(M);
        free(funs);
        delete_subsemi(kernel);
        delete_orbits(gplusorbs);
        return false;
    }
    return true;
}


static bool is_pol2st_dfa(dfa* A, int*, FILE* out) {

    dfa_mini_canonical_ordering(A);
    parti* scca = dtarjan(A->trans, NULL, false);
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);


    bool alpha[A->trans->size_alpha];
    for (uint i = 0; i < scci->size_par; i++) {
        uint qr = scci->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;
        if (A->order[q][r] || scca->numcl[q] > scca->numcl[r]) {
            continue;
        }

        dgraph_compute_alph_scc(inter, scci, i, alpha);
        uint* u = NULL;;
        if (dgraph_exists_path_alpha(A->trans, q, r, false, alpha, &u)) {
            if (out) {
                uint* v = dfacexa_get_alphaloop(inter, scci, alpha, qr);
                uint states[2] = { q, r };
                char vars[2] = { 'u', 'v' };
                uint* words[2] = { u, v };
                view_pattern_example(A, 2, states, 2, vars, words, false, out);
                free(v);
            }
            free(u);
            delete_dgraph(inter);
            delete_parti(scca);
            delete_parti(scci);
            return false;
        }
    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;
}



bool is_pol2gp_dfa(dfa* A, basis mode, int*, FILE* out) {
    if (mode == BA_ST) {
        return is_pol2st_dfa(A, NULL, out);
    }

    dfa_mini_canonical_ordering(A);

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);


    // Folding the SCCs according to the mode
    parti* fold = dgraph_stal_fold(inter, scci, mode);
    dgraph* g_fold = dgraph_implement_fold(inter, scci, fold);


    for (uint i = 0; i < scci->size_par; i++) {
        uint qr = scci->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;
        if (A->order[q][r] || scca->numcl[q] > scca->numcl[r]) {
            continue;
        }

        if (dgraph_exists_intersec_path(g_fold, A->trans, fold->numcl[qr], q, fold->numcl[qr], r, false, NULL)) {
            if (out) {
                uint states[2] = { q, r };
                view_pattern_example(A, 2, states, 0, NULL, NULL, false, out);
            }

            delete_dgraph(g_fold);
            delete_parti(fold);
            delete_dgraph(inter);
            delete_parti(scca);
            delete_parti(scci);
            return false;
        }
    }
    delete_dgraph(g_fold);
    delete_parti(fold);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    //free(inv_scci);
    return true;
}

bool is_pol2gpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    // Computes the canonical ordering of the minimal DFA (does nothing if the ordering is already computed)
    dfa_mini_canonical_ordering(A);
    uint* funs;
    morphism* M = dfa_to_morphism(A, true, error, &funs);
    orbits* bpolorbs = compute_bpgplusorbits(M, LV_GREG, mode);
    uint c[2];
    if (is_jsat_orbmono(bpolorbs, c)) {
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        return true;
    }
    else {
        if (out) {

            uint* w = word_from_morphism(M, c[0]);
            uint* z = word_from_morphism(M, c[1]);

            uint el = c[0];
            uint elz = c[1];
            uint q0 = ONE;
            for (uint qi = 0; qi < A->trans->size_graph; qi++) {
                if (!A->order[funs[A->trans->size_graph * elz + qi]][funs[A->trans->size_graph * el + qi]]) {
                    q0 = qi;
                    break;
                }
            }

            q0 = state_from_word(A, q0, z);
            uint q1 = state_from_word(A, q0, w);
            q1 = state_from_word(A, q1, z);

            uint states[2] = { q0, q1 };
            view_pattern_example(A, 2, states, 0, NULL, NULL, false, out);

            free(z);
            free(w);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        return false;
    }
    return true;
}

/****************/
/*+ R-trivial. +*/
/****************/


static bool is_rtrivial_dfa(dfa* A, int*, FILE* out) {
    parti* scca = dtarjan(A->trans, NULL, false);
    if (scca->size_par == A->trans->size_graph) {
        delete_parti(scca);
        return true;
    }

    if (out) {
        for (uint i = 0; i < scca->size_par; i++) {
            if (scca->cl_size[i] > 1) {
                uint q0 = scca->cl_elems[i][0];
                uint q1 = scca->cl_elems[i][1];
                uint* u;
                uint* v;
                dgraph_exists_path(A->trans, q0, q1, false, &u);
                dgraph_exists_path(A->trans, q1, q0, false, &v);
                uint states[2] = { q0, q1 };
                char vars[2] = { 'u', 'v' };
                uint* words[2] = {
                    u,
                    v,
                };
                view_pattern_example(A, 2, states, 2, vars, words, false, out);
                free(u);
                free(v);
                break;
            }
        }

    }
    delete_parti(scca);
    return false;
}

bool is_rtrivialgp_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_rtrivial_dfa(A, error, out);
    }
    parti* scca = dtarjan(A->trans, NULL, false);
    parti* fold = dgraph_stal_fold(A->trans, scca, mode);

    // If no states have been folded, then the pattern is satisfied.
    if (fold->size_par == fold->size_set) {
        delete_parti(fold);
        delete_parti(scca);
        return true;
    }

    if (out) {
        for (uint i = 0; i < fold->size_par; i++) {
            if (fold->cl_size[i] > 1) {
                uint q0 = fold->cl_elems[i][0];
                uint q1 = fold->cl_elems[i][1];
                uint states[2] = { q0, q1 };
                view_pattern_example(A, 2, states, 0, NULL, NULL, false, out);
            }
        }
    }

    delete_parti(fold);
    delete_parti(scca);
    return false;
}



static bool is_rtrivialplus_dfa(dfa* A, int*, FILE* out) {

    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // For each SCC of the graph
    for (uint i = 0; i < scca->size_par; i++) {
        // If the SCC is trivial, it cannot yield a counterexample.
        if (scca->cl_size[i] <= 1) {
            continue;
        }

        dgraph* thescc = dgraph_extract(A->trans, scca, inv_scca, i);
        dgraph* product = dgraph_direct_product(thescc, thescc);
        parti* sccp = dtarjan(product, NULL, false);
        delete_dgraph(thescc);

        for (uint j = 0; j < sccp->size_par; j++) {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / scca->cl_size[i];
            uint r = qr % scca->cl_size[i];
            if (q < r && dgraph_ntrivial_loop(product, sccp, qr)) {
                if (out) {
                    uint* u;
                    uint* v;
                    uint* z;
                    dgraph_exists_path(A->trans, scca->cl_elems[i][q], scca->cl_elems[i][r], true, &u);
                    dgraph_exists_path(A->trans, scca->cl_elems[i][r], scca->cl_elems[i][q], true, &v);
                    dgraph_exists_path(product, qr, qr, true, &z);
                    uint states[2] = { scca->cl_elems[i][q], scca->cl_elems[i][r] };
                    char vars[3] = { 'u', 'v', 'z' };
                    uint* words[3] = {
                        u,
                        v,
                        z,
                    };
                    view_pattern_example(A, 2, states, 3, vars, words, false, out);
                    free(u);
                    free(v);
                    free(z);
                }
                delete_parti(sccp);
                delete_dgraph(product);
                delete_parti(scca);
                free(inv_scca);
                return false;
            }
        }
        delete_parti(sccp);
        delete_dgraph(product);
    }
    delete_parti(scca);
    free(inv_scca);
    return true;
}


bool is_rtrivialgpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_rtrivialplus_dfa(A, error, out);
    }

    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);
    parti* fold = dgraph_stal_fold(A->trans, scca, mode);
    // uint* inv_fold = parti_compute_inv(fold);

    //dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    //parti* scci = dtarjan(inter, NULL, false);
    //delete_parti(scca);

    // We check each fold class to see if it contains a non-trivial common loop.
    // between two distinct states in the class.
    for (uint i = 0; i < scca->size_par; i++) {
        // If the fold class is trivial, it cannot yield a counterexample.
        if (scca->cl_size[i] <= 1) {
            continue;
        }



        dgraph* thescc = dgraph_extract(A->trans, scca, inv_scca, i);
        dgraph* product = dgraph_direct_product(thescc, thescc);
        parti* sccp = dtarjan(product, NULL, false);
        delete_dgraph(thescc);

        //parti* fold_product = dgraph_stal_fold(product, sccp, mode);

        for (uint j = 0; j < sccp->size_par; j++) {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / scca->cl_size[i];
            uint r = qr % scca->cl_size[i];

            if (fold->numcl[scca->cl_elems[i][q]] == fold->numcl[scca->cl_elems[i][r]] && q < r && dgraph_ntrivial_loop(product, sccp, qr)) {
                if (out) {
                    uint* z;
                    dgraph_exists_path(product, qr, qr, true, &z);
                    uint states[2] = { scca->cl_elems[i][q], scca->cl_elems[i][r] };
                    char vars[1] = { 'z' };
                    uint* words[1] = { z };
                    view_pattern_example(A, 2, states, 1, vars, words, false, out);
                    free(z);
                }
                delete_parti(sccp);
                delete_dgraph(product);
                delete_parti(scca);
                free(inv_scca);
                return false;
            }
        }
        delete_parti(sccp);
        delete_dgraph(product);
    }
    delete_parti(scca);
    free(inv_scca);
    delete_parti(fold);

    return true;
}



static bool is_rtrivialbpst_dfa(dfa* A, int*, FILE* out) {
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    bool alpha[A->trans->size_alpha];
    for (uint i = 0; i < scca->size_par; i++) {
        if (scca->cl_size[i] <= 1) {
            continue;
        }

        dgraph* thescc = dgraph_extract(A->trans, scca, inv_scca, i);
        dgraph* product = dgraph_direct_product(thescc, thescc);
        parti* sccp = dtarjan(product, NULL, false);

        for (uint j = 0; j < sccp->size_par; j++) {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / scca->cl_size[i];
            uint r = qr % scca->cl_size[i];
            if (q >= r) {
                continue;
            }
            dgraph_compute_alph_scc(product, sccp, j, alpha);
            uint* u = NULL;
            uint* v = NULL;
            if (dgraph_exists_path_alpha(thescc, q, r, false, alpha, &u) && dgraph_exists_path_alpha(thescc, r, q, false, alpha, &v)) {
                // If the two states are connected in both directions, we have a cycle.
                // We can print a counterexample if requested.
                if (out) {
                    uint states[2] = { scca->cl_elems[i][q], sccp->cl_elems[i][r] };
                    char vars[3] = { 'u', 'v', 'w' };
                    uint* w = dfacexa_get_alphaloop(thescc, sccp, alpha, qr);
                    uint* words[3] = { u, v, w };
                    view_pattern_example(A, 2, states, 3, vars, words, false, out);
                    free(w);

                }
                free(u);
                free(v);
                delete_dgraph(product);
                delete_dgraph(thescc);
                delete_parti(scca);
                delete_parti(sccp);
                free(inv_scca);
                return false;
            }
            free(u);
            free(v);
        }
        delete_dgraph(product);
        delete_dgraph(thescc);
        delete_parti(sccp);
    }
    delete_parti(scca);
    free(inv_scca);
    return true;
}

bool is_rtrivialbpgp_dfa(dfa* A, basis mode, int*, FILE* out) {
    if (mode == BA_ST) {
        return is_rtrivialbpst_dfa(A, NULL, out);
    }
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);



    for (uint i = 0; i < scca->size_par; i++) {
        if (scca->cl_size[i] <= 1) {
            continue;
        }
        dgraph* thescc = dgraph_extract(A->trans, scca, inv_scca, i);
        dgraph* product = dgraph_direct_product(thescc, thescc);
        parti* sccp = dtarjan(product, NULL, false);

        parti* fold = dgraph_stal_fold(product, sccp, mode);
        dgraph* g_fold = dgraph_implement_fold(product, sccp, fold);


        for (uint j = 0; j < sccp->size_par; j++) {
            uint qr = sccp->cl_elems[j][0];
            uint q = qr / scca->cl_size[i];
            uint r = qr % scca->cl_size[i];
            if (q >= r) {
                continue;
            }

            if (dgraph_exists_intersec_path(g_fold, thescc, fold->numcl[qr], q, fold->numcl[qr], r, false, NULL) && dgraph_exists_intersec_path(g_fold, thescc, fold->numcl[qr], r, fold->numcl[qr], q, false, NULL)) {
                if (out) {
                    uint states[2] = { q, r };
                    view_pattern_example(A, 2, states, 0, NULL, NULL, false, out);
                }
                delete_dgraph(thescc);
                delete_dgraph(product);
                delete_parti(sccp);
                delete_dgraph(g_fold);
                delete_parti(fold);
                delete_parti(scca);
                free(inv_scca);
                return false;
            }

        }
        delete_dgraph(thescc);
        delete_dgraph(product);
        delete_parti(sccp);
        delete_dgraph(g_fold);
        delete_parti(fold);
    }
    delete_parti(scca);
    free(inv_scca);
    return true;
}

bool is_rtrivialbpgpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    uint* funs;
    morphism* M = dfa_to_morphism(A, true, error, &funs);
    orbits* bpolorbs = compute_bpgplusorbits(M, LV_GREG, mode);
    uint c[3];
    if (is_gtrivial_orbmono(bpolorbs, R_GREEN, c)) {
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        return true;
    }
    else {
        if (out) {
            uint elu = c[0];
            uint elv = c[1];
            uint q = ONE;
            for (uint qi = 0; qi < A->trans->size_graph; qi++) {
                if (funs[A->trans->size_graph * elu + qi] != funs[A->trans->size_graph * elv + qi]) {
                    q = qi;
                    break;
                }
            }

            uint q0 = funs[A->trans->size_graph * elu + q];
            uint q1 = funs[A->trans->size_graph * elv + q];

            uint states[2] = { q0, q1 };
            view_pattern_example(A, 2, states, 0, NULL, NULL, false, out);

        }
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        return false;
    }
    return true;
}

/****************/
/*+ L-trivial. +*/
/****************/

static bool is_ltrivial_dfa(dfa* A, int*, FILE* out) {
    // The SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Intersection of the DFA with itself and its SCCs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);

    bool alpha[A->trans->size_alpha];
    for (uint i = 0; i < scci->size_par; i++) {
        uint qr = scci->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;

        // If q = r, this cannot yield a counterexample.
        // if q > r, the state (r,q) is in another scc (rq < qr in this case and the 
        // lists of elements are sorted in increasing order). Hence, (q,r) is treated
        // by symmetry with this other scc.
        if (q >= r) {
            continue;
        }
        dgraph_compute_alph_scc(inter, scci, i, alpha);

        for (uint s = 0; s < A->trans->size_graph; s++) {
            if (scca->numcl[q] < scca->numcl[s] || scca->numcl[r] < scca->numcl[s]) {
                continue;
            }
            // Check if there is a path from q to r that does not pass through s
            uint* u = NULL;
            uint* v = NULL;
            if (dgraph_exists_path_alpha(A->trans, s, q, false, alpha, &u) &&
                dgraph_exists_path_alpha(A->trans, s, r, false, alpha, &v)) {
                if (out) {
                    uint states[3] = { s, q, r };
                    char vars[3] = { 'u', 'v', 'w' };
                    uint* w = dfacexa_get_alphaloop(inter, scci, alpha, qr);
                    uint* words[3] = { u, v, w };
                    view_pattern_example(A, 3, states, 3, vars, words, false, out);
                    free(w);
                }
                free(u);
                free(v);
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                return false;
            }
            free(u);
            free(v);
        }
    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;
}

bool is_ltrivialgp_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_ltrivial_dfa(A, error, out);
    }

    // The SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Intersection of the DFA with itself and its SCCs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    // Folding the SCCs according to the mode
    parti* fold = dgraph_stal_fold(inter, scci, mode);
    dgraph* g_fold = dgraph_implement_fold(inter, scci, fold);
    delete_dgraph(inter);
    delete_parti(scci);

    bool* done;
    CALLOC(done, fold->size_par);

    for (uint i = 0; i < fold->size_par; i++) {
        uint qr = fold->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;

        // If q = r, this cannot yield a counterexample.
        // if q > r, the state (r,q) is in another scc (rq < qr in this case and the 
        // lists of elements are sorted in increasing order). Hence, (q,r) is treated
        // by symmetry with this other scc.
        if (q >= r) {
            continue;
        }

        for (uint s = 0; s < A->trans->size_graph; s++) {
            if (scca->numcl[q] < scca->numcl[s] || scca->numcl[r] < scca->numcl[s]) {
                continue;
            }
            if (dgraph_exists_intersec_path(A->trans, g_fold, s, fold->numcl[qr], q, fold->numcl[qr], false, NULL) &&
                dgraph_exists_intersec_path(A->trans, g_fold, s, fold->numcl[qr], r, fold->numcl[qr], false, NULL)) {
                if (out) {
                    uint states[3] = { s, q, r };
                    view_pattern_example(A, 3, states, 0, NULL, NULL, false, out);
                }
                delete_parti(fold);
                delete_dgraph(g_fold);
                delete_parti(scca);
                free(done);
                return false;
            }
        }

    }

    delete_parti(fold);
    delete_dgraph(g_fold);
    delete_parti(scca);
    free(done);
    return true;
}


static bool is_ltrivialdd_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);


    // We loop over all SCCs in the product graph (order 2) (this serves for the state variables x1, x2, x3, x4)
    for (uint i = 0; i < scci->size_par; i++) {

        uint t1 = scci->cl_elems[i][0] / A->trans->size_graph;
        uint t2 = scci->cl_elems[i][0] % A->trans->size_graph;
        // If the SCC is made of equal pairs of states, it cannot yield a counterexample.
        if (t1 == t2) {
            continue;
        }

        // We compute product graph of order 4 for this SCC
        dgraph* thescctwo = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescctwo, thescctwo);
        parti* sccq = dtarjan(quad, NULL, false);
        uint* inv_sccq = parti_compute_inv(sccq);

        for (uint h = 0; h < sccq->size_par; h++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[h][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[h][0];
            uint p1 = qstate / scci->cl_size[i];
            uint p3 = qstate % scci->cl_size[i];
            uint pp1 = scci->cl_elems[i][p1] / A->trans->size_graph;
            uint pp3 = scci->cl_elems[i][p3] / A->trans->size_graph;
            if (pp3 < pp1) {
                continue;
            }


            dgraph* thesccquad = dgraph_extract(quad, sccq, inv_sccq, h);


            // We loop over all sccs in the original graph (order 1) (this serves for the state variable x0)
            for (uint j = 0; j < scca->size_par; j++) {
                // If the scc j is not before the product scc i, we skip it (cannot yield a counterexample).
                if (scca->numcl[t1] < j || scca->numcl[t2] < j) {
                    continue;
                }
                // We extract the subgraph of the DFA corresponding to the scc j
                dgraph* thesccone = dgraph_extract(A->trans, scca, inv_scca, j);

                // We compute the product graph of order 5 for this SCC
                dgraph* five = dgraph_direct_product(thesccquad, thesccone);
                parti* sccfive = dtarjan(five, NULL, false);
                delete_dgraph(thesccone);

                // For each scc in the product graph of order 5 (this corresponds to the z loop)
                for (uint k = 0; k < sccfive->size_par; k++) {

                    uint x1x4x2x3x0 = sccfive->cl_elems[k][0];
                    if (!dgraph_ntrivial_loop(five, sccfive, x1x4x2x3x0)) {
                        continue;
                    }

                    uint x1x4x2x3 = x1x4x2x3x0 / scca->cl_size[j];
                    uint x1x4 = sccq->cl_elems[h][x1x4x2x3] / scci->cl_size[i];
                    uint x2x3 = sccq->cl_elems[h][x1x4x2x3] % scci->cl_size[i];
                    // We can skip this case (the symmetrical case is treated with another iteration of the loop)



                    uint x0 = scca->cl_elems[j][x1x4x2x3x0 % scca->cl_size[j]];
                    uint x1 = scci->cl_elems[i][x1x4] / A->trans->size_graph;
                    uint x4 = scci->cl_elems[i][x1x4] % A->trans->size_graph;
                    uint x2 = scci->cl_elems[i][x2x3] / A->trans->size_graph;
                    uint x3 = scci->cl_elems[i][x2x3] % A->trans->size_graph;

                    if (x3 < x1) {
                        continue;
                    }
                    uint* u = NULL;
                    uint* v = NULL;

                    if (dgraph_exists_intersec_path(thescctwo, A->trans, x2x3, x0, x1x4, x1, true, &u) &&
                        dgraph_exists_intersec_path(thescctwo, A->trans, x1x4, x0, x2x3, x3, true, &v)) {
                        if (out)
                        {
                            uint* z = NULL;
                            dgraph_exists_path(five, x1x4x2x3x0, x1x4x2x3x0, true, &z);
                            uint states[5] = { x0, x1, x2, x3, x4 };
                            char vars[3] = { 'u', 'v', 'z' };
                            uint* words[3] = { u, v, z };
                            view_pattern_example(A, 5, states, 3, vars, words, false, out);
                            free(z);

                        }

                        free(u);
                        free(v);
                        delete_dgraph(five);
                        delete_parti(sccfive);
                        delete_dgraph(thescctwo);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        delete_parti(scca);
                        free(inv_scca);
                        delete_dgraph(inter);
                        delete_parti(scci);
                        free(inv_scci);
                        delete_dgraph(thesccquad);
                        free(inv_sccq);
                        return false;
                    }

                    free(u);
                    free(v);

                }
                delete_dgraph(five);
                delete_parti(sccfive);
            }
            delete_dgraph(thesccquad);
        }
        delete_dgraph(thescctwo);
        delete_dgraph(quad);
        delete_parti(sccq);
        free(inv_sccq);
    }
    delete_parti(scca);
    free(inv_scca);
    delete_dgraph(inter);
    delete_parti(scci);
    free(inv_scci);
    return true;
}


bool is_ltrivialgpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_ltrivialdd_dfa(A, error, out);
    }

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);
    parti* fold = dgraph_stal_fold(inter, scci, mode);






    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {
        // We skip the last class, which is the one containing the artificial sink

        uint t1 = scci->cl_elems[i][0] / A->trans->size_graph;
        uint t2 = scci->cl_elems[i][0] % A->trans->size_graph;
        // If the SCC is made of equal pairs of states, it cannot yield a counterexample.
        if (t1 == t2) {
            continue;
        }

        // We compute product graph of order 4 for this SCC
        dgraph* thescctwo = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescctwo, thescctwo);
        parti* sccq = dtarjan(quad, NULL, false);
        uint* inv_sccq = parti_compute_inv(sccq);


        for (uint h = 0; h < sccq->size_par; h++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[h][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[h][0];
            uint p1 = qstate / scci->cl_size[i];
            uint p3 = qstate % scci->cl_size[i];
            uint pp1 = scci->cl_elems[i][p1] / A->trans->size_graph;
            uint pp3 = scci->cl_elems[i][p3] % A->trans->size_graph;
            if (pp3 < pp1 || fold->numcl[scci->cl_elems[i][p1]] != fold->numcl[scci->cl_elems[i][p3]]) {
                continue;
            }
            //pp3 < pp1 ||

            dgraph* thesccquad = dgraph_extract(quad, sccq, inv_sccq, h);


            // We loop over all sccs in the original graph (order 1) (this serves for the state variable x0)
            for (uint j = 0; j < scca->size_par; j++) {
                // If the scc j is not before the product scc i, we skip it (cannot yield a counterexample).
                if (scca->numcl[t1] < j || scca->numcl[t2] < j) {
                    continue;
                }
                // We extract the subgraph of the DFA corresponding to the scc j
                dgraph* thesccone = dgraph_extract(A->trans, scca, inv_scca, j);

                // We compute the product graph of order 5 for this SCC
                dgraph* five = dgraph_direct_product(thesccquad, thesccone);
                parti* sccfive = dtarjan(five, NULL, false);
                delete_dgraph(thesccone);


                // For each scc in the product graph of order 5 (this corresponds to the z loop)
                for (uint k = 0; k < sccfive->size_par; k++) {

                    uint x1x4x2x3x0 = sccfive->cl_elems[k][0];
                    if (!dgraph_ntrivial_loop(five, sccfive, x1x4x2x3x0)) {
                        continue;
                    }

                    uint x1x4x2x3 = x1x4x2x3x0 / scca->cl_size[j];
                    uint x1x4 = sccq->cl_elems[h][x1x4x2x3] / scci->cl_size[i];
                    uint x2x3 = sccq->cl_elems[h][x1x4x2x3] % scci->cl_size[i];
                    if (fold->numcl[scci->cl_elems[i][x1x4]] != fold->numcl[scci->cl_elems[i][x2x3]]) {
                        continue;
                    }

                    // We can skip this case (the symmetrical case is treated with another iteration of the loop)

                    uint x0 = scca->cl_elems[j][x1x4x2x3x0 % scca->cl_size[j]];
                    uint x1 = scci->cl_elems[i][x1x4] / A->trans->size_graph;
                    uint x4 = scci->cl_elems[i][x1x4] % A->trans->size_graph;
                    uint x2 = scci->cl_elems[i][x2x3] / A->trans->size_graph;
                    uint x3 = scci->cl_elems[i][x2x3] % A->trans->size_graph;

                    if (x3 < x1) {
                        continue;
                    }

                    if (dgraph_exists_intersec_path(thescctwo, A->trans, x2x3, x0, x1x4, x1, false, NULL) &&
                        dgraph_exists_intersec_path(thescctwo, A->trans, x1x4, x0, x2x3, x3, false, NULL)) {
                        if (out)
                        {
                            uint* z = NULL;
                            dgraph_exists_path(five, x1x4x2x3x0, x1x4x2x3x0, true, &z);
                            uint states[5] = { x0, x1, x2, x3, x4 };
                            char vars[1] = { 'z' };
                            uint* words[1] = { z };
                            view_pattern_example(A, 5, states, 1, vars, words, false, out);
                            free(z);

                        }

                        delete_dgraph(five);
                        delete_parti(sccfive);
                        delete_dgraph(thesccquad);
                        delete_dgraph(thescctwo);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        free(inv_sccq);
                        delete_dgraph(inter);
                        delete_parti(scca);
                        delete_parti(scci);
                        free(inv_scci);
                        free(inv_scca);
                        return false;
                    }


                }
                delete_dgraph(five);
                delete_parti(sccfive);
            }
            delete_dgraph(thesccquad);
        }
        delete_dgraph(thescctwo);
        delete_dgraph(quad);
        delete_parti(sccq);
        free(inv_sccq);

    }
    delete_parti(fold);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    free(inv_scci);
    free(inv_scca);
    return true;

}




static bool is_ltrivialbpst_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);

    bool alpha[A->trans->size_alpha];
    // We loop over all SCCs in the product graph (order 2) (this serves for the state variables x1, x2, x3, x4)
    for (uint i = 0; i < scci->size_par; i++) {

        uint t1 = scci->cl_elems[i][0] / A->trans->size_graph;
        uint t2 = scci->cl_elems[i][0] % A->trans->size_graph;
        // If the SCC is made of equal pairs of states, it cannot yield a counterexample.
        if (t1 == t2) {
            continue;
        }

        // We compute product graph of order 4 for this SCC
        dgraph* thescctwo = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescctwo, thescctwo);
        parti* sccq = dtarjan(quad, NULL, false);
        uint* inv_sccq = parti_compute_inv(sccq);

        for (uint h = 0; h < sccq->size_par; h++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[h][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[h][0];
            uint p1 = qstate / scci->cl_size[i];
            uint p3 = qstate % scci->cl_size[i];
            uint pp1 = scci->cl_elems[i][p1] / A->trans->size_graph;
            uint pp3 = scci->cl_elems[i][p3] / A->trans->size_graph;
            if (pp3 < pp1) {
                continue;
            }


            dgraph* thesccquad = dgraph_extract(quad, sccq, inv_sccq, h);


            // We loop over all sccs in the original graph (order 1) (this serves for the state variable x0)
            for (uint j = 0; j < scca->size_par; j++) {
                // If the scc j is not before the product scc i, we skip it (cannot yield a counterexample).
                if (scca->numcl[t1] < j || scca->numcl[t2] < j) {
                    continue;
                }
                // We extract the subgraph of the DFA corresponding to the scc j
                dgraph* thesccone = dgraph_extract(A->trans, scca, inv_scca, j);

                // We compute the product graph of order 5 for this SCC
                dgraph* five = dgraph_direct_product(thesccquad, thesccone);
                parti* sccfive = dtarjan(five, NULL, false);
                delete_dgraph(thesccone);

                // For each scc in the product graph of order 5 (this corresponds to the z loop)
                for (uint k = 0; k < sccfive->size_par; k++) {

                    uint x1x4x2x3x0 = sccfive->cl_elems[k][0];
                    if (!dgraph_ntrivial_loop(five, sccfive, x1x4x2x3x0)) {
                        continue;
                    }
                    dgraph_compute_alph_scc(five, sccfive, k, alpha);

                    uint x1x4x2x3 = x1x4x2x3x0 / scca->cl_size[j];
                    uint x1x4 = sccq->cl_elems[h][x1x4x2x3] / scci->cl_size[i];
                    uint x2x3 = sccq->cl_elems[h][x1x4x2x3] % scci->cl_size[i];
                    // We can skip this case (the symmetrical case is treated with another iteration of the loop)



                    uint x0 = scca->cl_elems[j][x1x4x2x3x0 % scca->cl_size[j]];
                    uint x1 = scci->cl_elems[i][x1x4] / A->trans->size_graph;
                    uint x4 = scci->cl_elems[i][x1x4] % A->trans->size_graph;
                    uint x2 = scci->cl_elems[i][x2x3] / A->trans->size_graph;
                    uint x3 = scci->cl_elems[i][x2x3] % A->trans->size_graph;

                    if (x3 < x1) {
                        continue;
                    }
                    uint* u = NULL;
                    uint* v = NULL;

                    if (dgraph_exists_intersec_path_alpha(thescctwo, A->trans, x2x3, x0, x1x4, x1, false, alpha, &u) &&
                        dgraph_exists_intersec_path_alpha(thescctwo, A->trans, x1x4, x0, x2x3, x3, false, alpha, &v)) {
                        if (out)
                        {
                            uint* z = dfacexa_get_alphaloop(five, sccfive, alpha, x1x4x2x3x0);
                            uint states[5] = { x0, x1, x2, x3, x4 };
                            char vars[3] = { 'u', 'v', 'z' };
                            uint* words[3] = { u, v, z };
                            view_pattern_example(A, 5, states, 3, vars, words, false, out);
                            free(z);

                        }

                        free(u);
                        free(v);
                        delete_dgraph(five);
                        delete_parti(sccfive);
                        delete_dgraph(thescctwo);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        delete_parti(scca);
                        free(inv_scca);
                        delete_dgraph(inter);
                        delete_parti(scci);
                        free(inv_scci);
                        delete_dgraph(thesccquad);
                        free(inv_sccq);
                        return false;
                    }

                    free(u);
                    free(v);

                }
                delete_dgraph(five);
                delete_parti(sccfive);
            }
            delete_dgraph(thesccquad);
        }
        delete_dgraph(thescctwo);
        delete_dgraph(quad);
        delete_parti(sccq);
        free(inv_sccq);
    }
    delete_parti(scca);
    free(inv_scca);
    delete_dgraph(inter);
    delete_parti(scci);
    free(inv_scci);
    return true;
}

bool is_ltrivialbpgp_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_ltrivialbpst_dfa(A, error, out);
    }

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);

    // We loop over all SCCs in the product graph (order 2) (this serves for the state variables x1, x2, x3, x4)
    for (uint i = 0; i < scci->size_par; i++) {

        uint t1 = scci->cl_elems[i][0] / A->trans->size_graph;
        uint t2 = scci->cl_elems[i][0] % A->trans->size_graph;
        // If the SCC is made of equal pairs of states, it cannot yield a counterexample.
        if (t1 == t2) {
            continue;
        }

        // We compute product graph of order 4 for this SCC
        dgraph* thescctwo = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescctwo, thescctwo);
        parti* sccq = dtarjan(quad, NULL, false);
        uint* inv_sccq = parti_compute_inv(sccq);

        dgraph* big = dgraph_direct_product(thescctwo, A->trans);

        for (uint h = 0; h < sccq->size_par; h++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[h][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[h][0];
            uint p1 = qstate / scci->cl_size[i];
            uint p3 = qstate % scci->cl_size[i];
            uint pp1 = scci->cl_elems[i][p1] / A->trans->size_graph;
            uint pp3 = scci->cl_elems[i][p3] / A->trans->size_graph;
            if (pp3 < pp1) {
                continue;
            }


            dgraph* thesccquad = dgraph_extract(quad, sccq, inv_sccq, h);


            // We loop over all sccs in the original graph (order 1) (this serves for the state variable x0)
            for (uint j = 0; j < scca->size_par; j++) {
                // If the scc j is not before the product scc i, we skip it (cannot yield a counterexample).
                if (scca->numcl[t1] < j || scca->numcl[t2] < j) {
                    continue;
                }
                // We extract the subgraph of the DFA corresponding to the scc j
                dgraph* thesccone = dgraph_extract(A->trans, scca, inv_scca, j);

                // We compute the product graph of order 5 for this SCC
                dgraph* five = dgraph_direct_product(thesccquad, thesccone);
                parti* sccfive = dtarjan(five, NULL, false);
                delete_dgraph(thesccone);

                parti* fold = dgraph_stal_fold(five, sccfive, mode);
                dgraph* g_fold = dgraph_implement_fold(five, sccfive, fold);

                // For each scc in the product graph of order 5 (this corresponds to the z loop)
                for (uint k = 0; k < sccfive->size_par; k++) {

                    uint x1x4x2x3x0 = sccfive->cl_elems[k][0];
                    if (!dgraph_ntrivial_loop(five, sccfive, x1x4x2x3x0)) {
                        continue;
                    }

                    uint x1x4x2x3 = x1x4x2x3x0 / scca->cl_size[j];
                    uint x1x4 = sccq->cl_elems[h][x1x4x2x3] / scci->cl_size[i];
                    uint x2x3 = sccq->cl_elems[h][x1x4x2x3] % scci->cl_size[i];
                    // We can skip this case (the symmetrical case is treated with another iteration of the loop)



                    uint x0 = scca->cl_elems[j][x1x4x2x3x0 % scca->cl_size[j]];
                    uint x1 = scci->cl_elems[i][x1x4] / A->trans->size_graph;
                    uint x4 = scci->cl_elems[i][x1x4] % A->trans->size_graph;
                    uint x2 = scci->cl_elems[i][x2x3] / A->trans->size_graph;
                    uint x3 = scci->cl_elems[i][x2x3] % A->trans->size_graph;

                    if (x3 < x1) {
                        continue;
                    }

                    uint bx2x3x0 = x2x3 * A->trans->size_graph + x0;
                    uint bx1x4x1 = x1x4 * A->trans->size_graph + x1;
                    uint bx1x4x0 = x1x4 * A->trans->size_graph + x0;
                    uint bx2x3x3 = x2x3 * A->trans->size_graph + x3;

                    if (dgraph_exists_intersec_path(g_fold, big, fold->numcl[x1x4x2x3x0], bx2x3x0, fold->numcl[x1x4x2x3x0], bx1x4x1, false, NULL) &&
                        dgraph_exists_intersec_path(g_fold, big, fold->numcl[x1x4x2x3x0], bx1x4x0, fold->numcl[x1x4x2x3x0], bx2x3x3, false, NULL)) {
                        if (out)
                        {
                            uint states[5] = { x0, x1, x2, x3, x4 };
                            view_pattern_example(A, 5, states, 0, NULL, NULL, false, out);

                        }
                        delete_dgraph(five);
                        delete_parti(sccfive);
                        delete_parti(fold);
                        delete_dgraph(g_fold);
                        delete_dgraph(thesccquad);
                        delete_dgraph(thescctwo);
                        delete_dgraph(quad);
                        delete_parti(sccq);
                        free(inv_sccq);
                        delete_dgraph(big);
                        delete_parti(scca);
                        free(inv_scca);
                        delete_dgraph(inter);
                        delete_parti(scci);
                        free(inv_scci);
                        return false;
                    }

                }
                delete_dgraph(five);
                delete_parti(sccfive);
                delete_parti(fold);
                delete_dgraph(g_fold);
            }
            delete_dgraph(thesccquad);
        }
        delete_dgraph(thescctwo);
        delete_dgraph(quad);
        delete_parti(sccq);
        free(inv_sccq);
        delete_dgraph(big);
    }
    delete_parti(scca);
    free(inv_scca);
    delete_dgraph(inter);
    delete_parti(scci);
    free(inv_scci);
    return true;
}


// bool is_ltrivialbpgp_dfa(dfa* A, basis mode, int* error, FILE* out) {
//     if (mode == BA_ST) {
//         return is_ltrivialbpst_dfa(A, error, out);
//     }

//     // SCCs of the DFA
//     parti* scca = dtarjan(A->trans, NULL, false);

//     // Compute the product graph of the DFA with itself and its sccs
//     dgraph* inter = dgraph_direct_product(A->trans, A->trans);
//     parti* scci = dtarjan(inter, NULL, false);

//     uint* inv_scci = parti_compute_inv(scci);

//     // For each SCC in the product graph
//     for (uint i = 0; i < scci->size_par; i++) {
//         uint t = scci->cl_elems[i][0];
//         uint t1 = t / A->trans->size_graph;
//         uint t2 = t % A->trans->size_graph;
//         if (t1 == t2) {
//             continue;
//         }

//         // We compute the local double product graph for this SCC
//         dgraph* thescc = dgraph_extract(inter, scci, inv_scci, i);
//         dgraph* quad = dgraph_direct_product(thescc, thescc);
//         parti* sccq = dtarjan(quad, NULL, false);
//         dgraph_discard_nonscc_edges(quad, sccq);
//         dgraph* big = dgraph_direct_product(thescc, A->trans);
//         delete_dgraph(thescc);

//         dgraph* five = dgraph_direct_product(quad, A->trans);
//         parti* sccfive = dtarjan(five, NULL, false);
//         //view_dgraph(five);
//         parti* fold = dgraph_stal_fold(five, sccfive, mode);
//         dgraph* g_fold = dgraph_implement_fold(five, sccfive, fold);
//         //view_dgraph(g_fold);



//         // We pick a pair of states in the SCC. 
//         for (uint j = 0; j < scci->cl_size[i]; j++) {
//             for (uint k = 0; k < scci->cl_size[i]; k++) {
//                 // We first check if there is a common self-loop using the double product graph

//                 uint qstate = j * scci->cl_size[i] + k;
//                 if (!dgraph_ntrivial_loop(quad, sccq, qstate)) {
//                     continue;
//                 }

//                 uint x1x4 = scci->cl_elems[i][j];
//                 uint x2x3 = scci->cl_elems[i][k];

//                 uint x1 = x1x4 / A->trans->size_graph;
//                 uint x4 = x1x4 % A->trans->size_graph;
//                 uint x2 = x2x3 / A->trans->size_graph;
//                 uint x3 = x2x3 % A->trans->size_graph;

//                 // We can skip this case (the symmetrical case is treated with another iteration of the loop)
//                 if (x1 < x3) {
//                     continue;
//                 }

//                 for (uint x0 = 0; x0 < A->trans->size_graph; x0++) {
//                     if (scca->numcl[x2] < scca->numcl[x0] || scca->numcl[x4] < scca->numcl[x0]) {
//                         continue;
//                     }


//                     uint fstatec = fold->numcl[qstate * A->trans->size_graph + x0];



//                     if (dgraph_exists_intersec_path(big, g_fold, k * A->trans->size_graph + x0, fstatec, j * A->trans->size_graph + x1, fstatec, false, NULL) &&
//                         dgraph_exists_intersec_path(big, g_fold, j * A->trans->size_graph + x0, fstatec, k * A->trans->size_graph + x3, fstatec, false, NULL)) {
//                         if (out)
//                         {
//                             uint states[5] = { x0, x1, x2, x3, x4 };
//                             view_pattern_example(A, 5, states, 0, NULL, NULL, false, out);
//                         }
//                         delete_parti(fold);
//                         delete_dgraph(g_fold);
//                         delete_dgraph(big);
//                         delete_dgraph(five);
//                         delete_parti(sccfive);
//                         delete_dgraph(inter);
//                         delete_parti(scca);
//                         delete_parti(scci);
//                         delete_dgraph(quad);
//                         delete_parti(sccq);
//                         free(inv_scci);
//                         return false;
//                     }

//                 }
//             }
//         }
//         delete_parti(fold);
//         delete_dgraph(g_fold);
//         delete_dgraph(big);
//         delete_dgraph(quad);
//         delete_parti(sccq);
//         delete_dgraph(five);
//         delete_parti(sccfive);

//     }
//     delete_dgraph(inter);
//     delete_parti(scca);
//     delete_parti(scci);
//     free(inv_scci);
//     return true;
// }






bool is_ltrivialbpgpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    uint* funs;
    morphism* M = dfa_to_morphism(A, true, error, &funs);
    orbits* bpolorbs = compute_bpgplusorbits(M, LV_GREG, mode);
    uint c[2];

    for (uint i = 0; i < bpolorbs->nb_computed;i++) {
        subsemi* S = bpolorbs->orbits[i];
        if (is_gtrivial_subsemi(S, L_GREEN, c)) {
            continue;
        }
        if (out) {
            uint e = c[0];
            uint s = c[1];
            uint esub = S->mono_to_sub[e];
            uint r = ONE;
            for (uint j = 0; j < S->rels->RCL->cl_size[S->rels->RCL->numcl[esub]]; j++) {
                uint rsub = S->rels->RCL->cl_elems[S->rels->RCL->numcl[esub]][j];
                r = S->sub_to_mono[rsub];
                if (mor_mult(M, r, s) == e) {
                    break;
                }
            }

            uint q0 = ONE;
            for (uint qi = 0; qi < A->trans->size_graph; qi++) {
                if (funs[A->trans->size_graph * e + qi] != funs[A->trans->size_graph * s + qi]) {
                    q0 = qi;
                    break;
                }
            }
            uint q2 = funs[A->trans->size_graph * e + q0];
            uint q1 = funs[A->trans->size_graph * r + q2];
            uint q3 = funs[A->trans->size_graph * s + q0];
            uint q4 = funs[A->trans->size_graph * r + q3];
            uint states[5] = { q0, q1, q2, q3, q4 };
            view_pattern_example(A, 5, states, 0, NULL, NULL, false, out);

        }
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        return false;

    }
    return true;
}


/********/
/*+ DA +*/
/********/

static bool is_da_dfa(dfa* A, int*, FILE* out) {

    // The SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Intersection of the DFA with itself and its SCCs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);


    bool alpha[A->trans->size_alpha];
    for (uint i = 0; i < scci->size_par; i++) {
        uint qr = scci->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;
        if (q == r || scca->numcl[q] > scca->numcl[r]) {
            continue;
        }

        dgraph_compute_alph_scc(inter, scci, i, alpha);
        uint* u = NULL;
        if (dgraph_exists_path_alpha(A->trans, q, r, false, alpha, &u)) {
            if (out) {
                uint* v = dfacexa_get_alphaloop(inter, scci, alpha, qr);
                uint states[2] = { q, r };
                char vars[2] = { 'u', 'v' };
                uint* words[2] = { u, v };
                view_pattern_example(A, 2, states, 2, vars, words, false, out);
                free(v);
            }
            free(u);
            delete_dgraph(inter);
            delete_parti(scca);
            delete_parti(scci);
            return false;
        }
        free(u);
    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}



bool is_dagp_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_da_dfa(A, error, out);
    }


    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);

    // Folding the SCC according to the mode
    parti* fold = dgraph_stal_fold(inter, scci, mode);
    dgraph* g_fold = dgraph_implement_fold(inter, scci, fold);
    delete_dgraph(inter);

    for (uint i = 0; i < scci->size_par; i++) {
        uint qr = scci->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;
        if (q == r || scca->numcl[q] > scca->numcl[r]) {
            continue;
        }
        if (dgraph_exists_intersec_path(g_fold, A->trans, fold->numcl[qr], q, fold->numcl[qr], r, false, NULL)) {
            if (out) {
                uint states[2] = { q, r };
                view_pattern_example(A, 2, states, 0, NULL, NULL, false, out);
            }
            delete_dgraph(g_fold);
            delete_parti(fold);
            delete_parti(scca);
            delete_parti(scci);
            return false;
        }
    }
    delete_dgraph(g_fold);
    delete_parti(fold);
    delete_parti(scca);
    delete_parti(scci);
    return true;
}


static bool is_dadd_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);


    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {
        // We skip the last class, which is the one containing the artificial sink

        uint t = scci->cl_elems[i][0];
        uint t1 = t / A->trans->size_graph;
        uint t2 = t % A->trans->size_graph;
        if (t1 == t2 || scca->numcl[t1] > scca->numcl[t2]) {
            continue;
        }

        // We compute the local double product graph for this SCC
        dgraph* thescc = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescc, thescc);
        parti* sccq = dtarjan(quad, NULL, false);
        //dgraph_make_product_sccs_local(inter, scci, i, &quad, &sccq);

        // We pick a pair of distinct states in the SCC. 
        for (uint j = 0; j < sccq->size_par; j++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[j][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[j][0];
            uint x0x2 = qstate / scci->cl_size[i];
            uint x1x3 = qstate % scci->cl_size[i];

            uint x0 = scci->cl_elems[i][x0x2] / A->trans->size_graph;
            uint x2 = scci->cl_elems[i][x0x2] % A->trans->size_graph;
            uint x1 = scci->cl_elems[i][x1x3] / A->trans->size_graph;
            uint x3 = scci->cl_elems[i][x1x3] % A->trans->size_graph;

            uint* v = NULL;

            if (dgraph_exists_intersec_path(thescc, A->trans, x1x3, x0, x0x2, x2, true, &v)) {
                if (out) {
                    uint states[4] = { x0, x1, x2, x3 };
                    char vars[3] = { 'u', 'v', 'z' };
                    uint* u = NULL;
                    dgraph_exists_path(thescc, x0x2, x1x3, true, &u);
                    uint* z = NULL;
                    dgraph_exists_path(quad, qstate, qstate, true, &z);
                    uint* words[3] = { u, v, z };
                    view_pattern_example(A, 4, states, 3, vars, words, false, out);
                    free(u);
                    free(z);
                }
                free(v);
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                free(inv_scci);
                delete_dgraph(thescc);
                delete_dgraph(quad);
                delete_parti(sccq);
                return false;
            }
            free(v);

        }
        delete_dgraph(thescc);
        delete_dgraph(quad);
        delete_parti(sccq);

    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    free(inv_scci);
    return true;

}




bool is_dagpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_dadd_dfa(A, error, out);
    }

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);

    parti* fold = dgraph_stal_fold(inter, scci, mode);
    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {
        // We skip the last class, which is the one containing the artificial sink

        uint t = scci->cl_elems[i][0];
        uint t1 = t / A->trans->size_graph;
        uint t2 = t % A->trans->size_graph;
        if (t1 == t2 || scca->numcl[t1] > scca->numcl[t2]) {
            continue;
        }

        // We compute the local double product graph for this SCC
        dgraph* thescc = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescc, thescc);
        parti* sccq = dtarjan(quad, NULL, false);

        for (uint j = 0; j < sccq->size_par; j++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[j][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[j][0];
            uint x0x2 = qstate / scci->cl_size[i];
            uint x1x3 = qstate % scci->cl_size[i];

            if (fold->numcl[scci->cl_elems[i][x0x2]] != fold->numcl[scci->cl_elems[i][x1x3]]) {
                continue;
            }

            uint x0 = scci->cl_elems[i][x0x2] / A->trans->size_graph;
            uint x2 = scci->cl_elems[i][x0x2] % A->trans->size_graph;
            uint x1 = scci->cl_elems[i][x1x3] / A->trans->size_graph;
            uint x3 = scci->cl_elems[i][x1x3] % A->trans->size_graph;


            if (dgraph_exists_intersec_path(thescc, A->trans, x1x3, x0, x0x2, x2, false, NULL)) {
                if (out) {
                    uint states[4] = { x0, x1, x2, x3 };
                    char vars[1] = { 'z' };
                    uint* z = NULL;
                    dgraph_exists_path(quad, qstate, qstate, true, &z);
                    uint* words[1] = { z };
                    view_pattern_example(A, 4, states, 1, vars, words, false, out);
                    free(z);
                }
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                free(inv_scci);
                delete_dgraph(thescc);
                delete_dgraph(quad);
                delete_parti(sccq);
                return false;
            }

        }
        delete_dgraph(thescc);
        delete_dgraph(quad);
        delete_parti(sccq);
    }
    delete_parti(fold);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    free(inv_scci);
    return true;
}




static bool is_dabpst_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);

    bool alpha[A->trans->size_alpha];
    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {
        // We skip the last class, which is the one containing the artificial sink

        uint t = scci->cl_elems[i][0];
        uint t1 = t / A->trans->size_graph;
        uint t2 = t % A->trans->size_graph;
        if (t1 == t2 || scca->numcl[t1] > scca->numcl[t2]) {
            continue;
        }

        // We compute the local double product graph for this SCC
        dgraph* thescc = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescc, thescc);
        parti* sccq = dtarjan(quad, NULL, false);
        //dgraph_make_product_sccs_local(inter, scci, i, &quad, &sccq);

        // We pick a pair of distinct states in the SCC. 
        for (uint j = 0; j < sccq->size_par; j++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[j][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[j][0];
            uint x0x2 = qstate / scci->cl_size[i];
            uint x1x3 = qstate % scci->cl_size[i];

            uint x0 = scci->cl_elems[i][x0x2] / A->trans->size_graph;
            uint x2 = scci->cl_elems[i][x0x2] % A->trans->size_graph;
            uint x1 = scci->cl_elems[i][x1x3] / A->trans->size_graph;
            uint x3 = scci->cl_elems[i][x1x3] % A->trans->size_graph;

            dgraph_compute_alph_scc(quad, sccq, sccq->numcl[qstate], alpha);

            uint* u = NULL;
            uint* v = NULL;

            if (dgraph_exists_path_alpha(thescc, x0x2, x1x3, false, alpha, &u) &&
                dgraph_exists_intersec_path_alpha(thescc, A->trans, x1x3, x0, x0x2, x2, false, alpha, &v)) {
                if (out) {
                    uint states[4] = { x0, x1, x2, x3 };
                    char vars[3] = { 'u', 'v', 'z' };

                    uint* z = dfacexa_get_alphaloop(quad, sccq, alpha, qstate);
                    uint* words[3] = { u, v, z };
                    view_pattern_example(A, 4, states, 3, vars, words, false, out);
                    free(z);
                }
                free(u);
                free(v);
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                free(inv_scci);
                delete_dgraph(thescc);
                delete_dgraph(quad);
                delete_parti(sccq);
                return false;
            }
            free(u);
            free(v);

        }
        delete_dgraph(thescc);
        delete_dgraph(quad);
        delete_parti(sccq);

    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    free(inv_scci);
    return true;

}

bool is_dabpgp_dfa(dfa* A, basis mode, int* error, FILE* out) {
    // If the mode is BA_ST, we call the dedicated function.
    if (mode == BA_ST) {
        return is_dabpst_dfa(A, error, out);
    }

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);


    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {
        // We skip the last class, which is the one containing the artificial sink

        uint t = scci->cl_elems[i][0];
        uint t1 = t / A->trans->size_graph;
        uint t2 = t % A->trans->size_graph;
        if (t1 == t2 || scca->numcl[t1] > scca->numcl[t2]) {
            continue;
        }

        // We compute the local double product graph for this SCC
        dgraph* thescc = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescc, thescc);
        parti* sccq = dtarjan(quad, NULL, false);
        //dgraph_make_product_sccs_local(inter, scci, i, &quad, &sccq);


        // Folding the double product graph according to the mode.
        parti* fold = dgraph_stal_fold(quad, sccq, mode);
        dgraph* g_fold = dgraph_implement_fold(quad, sccq, fold);


        dgraph* big = dgraph_direct_product(thescc, A->trans);

        // We pick a pair of distinct states in the SCC. 
        for (uint j = 0; j < sccq->size_par; j++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[j][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[j][0];
            uint x0x2 = qstate / scci->cl_size[i];
            uint x1x3 = qstate % scci->cl_size[i];

            uint x0 = scci->cl_elems[i][x0x2] / A->trans->size_graph;
            uint x2 = scci->cl_elems[i][x0x2] % A->trans->size_graph;
            uint x1 = scci->cl_elems[i][x1x3] / A->trans->size_graph;
            uint x3 = scci->cl_elems[i][x1x3] % A->trans->size_graph;

            uint big1 = x1x3 * A->trans->size_graph + x0;
            uint big2 = x0x2 * A->trans->size_graph + x2;



            if (dgraph_exists_intersec_path(thescc, g_fold, x0x2, fold->numcl[qstate], x1x3, fold->numcl[qstate], false, NULL) &&
                dgraph_exists_intersec_path(big, g_fold, big1, fold->numcl[qstate], big2, fold->numcl[qstate], false, NULL)) {
                if (out) {
                    uint states[4] = { x0, x1, x2, x3 };
                    view_pattern_example(A, 4, states, 0, NULL, NULL, false, out);
                }
                delete_dgraph(g_fold);
                delete_parti(fold);
                delete_dgraph(big);
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                free(inv_scci);
                delete_dgraph(thescc);
                delete_dgraph(quad);
                delete_parti(sccq);
                return false;
            }

        }
        delete_dgraph(g_fold);
        delete_parti(fold);
        delete_dgraph(big);
        delete_dgraph(thescc);
        delete_dgraph(quad);
        delete_parti(sccq);

    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    free(inv_scci);
    return true;



}






bool is_dabpgpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    uint* funs;
    morphism* M = dfa_to_morphism(A, true, error, &funs);
    orbits* bpolorbs = compute_bpgplusorbits(M, LV_GREG, mode);
    uint c[2];

    for (uint i = 0; i < bpolorbs->nb_computed;i++) {
        subsemi* S = bpolorbs->orbits[i];
        if (is_da_subsemi(S, c)) {
            continue;
        }
        if (out) {
            uint t = c[0];
            uint tsub = S->mono_to_sub[t];
            uint flsub = ONE;
            uint lclass = S->rels->LCL->numcl[tsub];
            for (uint j = 0; j < S->rels->LCL->cl_size[lclass]; j++) {
                flsub = S->rels->LCL->cl_elems[lclass][j];
                if (S->idem_array[flsub]) {
                    break;
                }
            }
            uint f = S->sub_to_mono[flsub];


            uint frsub = ONE;
            uint rclass = S->rels->RCL->numcl[tsub];
            for (uint j = 0; j < S->rels->RCL->cl_size[rclass]; j++) {
                frsub = S->rels->RCL->cl_elems[rclass][j];
                if (S->idem_array[frsub]) {
                    break;
                }
            }


            uint r = 0;
            uint hfl = 0, hfr = 0;
            while (hfl < S->rels->RCL->cl_size[S->rels->RCL->numcl[flsub]] && hfr < S->rels->LCL->cl_size[S->rels->LCL->numcl[frsub]]) {
                uint rsubl = S->rels->RCL->cl_elems[S->rels->RCL->numcl[flsub]][hfl];
                uint rsubr = S->rels->LCL->cl_elems[S->rels->LCL->numcl[frsub]][hfr];
                if (rsubl < rsubr) {
                    hfl++;
                    continue;
                }

                if (rsubl > rsubr) {
                    hfr++;
                    continue;
                }
                if (mor_mult(M, S->sub_to_mono[rsubr], t) == f) {
                    r = S->sub_to_mono[rsubr];
                    break;
                }
                hfl++;
                hfr++;
            }


            uint ftf = mor_mult_gen(M, 3, f, t, f);
            uint q = ONE;
            for (uint qi = 0; qi < A->trans->size_graph; qi++) {
                if (funs[A->trans->size_graph * f + qi] != funs[A->trans->size_graph * ftf + qi]) {
                    q = qi;
                    break;
                }
            }



            uint x0 = funs[A->trans->size_graph * f + q];
            uint x1 = funs[A->trans->size_graph * r + x0];
            uint x2 = funs[A->trans->size_graph * t + x0];
            x2 = funs[A->trans->size_graph * f + x2];
            uint x3 = funs[A->trans->size_graph * r + x2];


            uint states[4] = { x0, x1, x2, x3 };
            view_pattern_example(A, 4, states, 0, NULL, NULL, false, out);

        }
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        return false;

    }
    return true;
}


/**********/
/*+ BPol +*/
/**********/

static void is_piecewise_aloop(dgraph* g, uint q, bool* alpha) {
    for (uint a = 0; a < g->size_alpha; a++) {
        alpha[a] = (q == g->edges[q][a]);
    }
}

static bool is_piecewise_comp(dgraph* g, bool* alpha1, bool* alpha2) {
    for (uint a = 0; a < g->size_alpha; a++) {
        if (alpha1[a] && !alpha2[a]) {
            return false;
        }
    }
    return true;
}

bool is_piecewise_dfa(dfa* A, int*) {
    parti* scca = dtarjan(A->trans, NULL, false);
    if (scca->size_par != A->trans->size_graph) {
        delete_parti(scca);
        return false;
    }
    delete_parti(scca);
    // Mirror for the DFA
    lgraph* mirror = dgraph_mirror(A->trans);



    bool alpha1[A->trans->size_alpha];
    bool alpha2[A->trans->size_alpha];

    dequeue* queue = create_dequeue();

    for (uint q = 0; q < A->trans->size_graph; q++) {
        //dfa_print_state(A, q, stdout);
        //printf("\n");
        is_piecewise_aloop(A->trans, q, alpha1);

        bool* visited;
        CALLOC(visited, A->trans->size_graph);
        for (uint a = 0; a < A->trans->size_alpha; a++) {

            if (alpha1[a]) {
                rigins_dequeue(A->trans->edges[q][a], queue);
                for (uint i = 0; i < size_dequeue(mirror->edges[q][a]);i++) {
                    rigins_dequeue(lefread_dequeue(mirror->edges[q][a], i), queue);
                }
            }
        }
        visited[q] = true;
        while (!isempty_dequeue(queue)) {
            uint r = lefpull_dequeue(queue);
            if (visited[r]) {
                continue;
            }
            visited[r] = true;
            is_piecewise_aloop(A->trans, r, alpha2);
            if (is_piecewise_comp(A->trans, alpha1, alpha2)) {
                free(visited);
                delete_lgraph(mirror);
                delete_dequeue(queue);
                return false;
            }
            for (uint a = 0; a < A->trans->size_alpha; a++) {
                if (alpha1[a]) {
                    rigins_dequeue(A->trans->edges[r][a], queue);
                    for (uint i = 0; i < size_dequeue(mirror->edges[r][a]);i++) {
                        rigins_dequeue(lefread_dequeue(mirror->edges[r][a], i), queue);
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






bool is_bpolmod_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    parti* foldone = dgraph_stal_fold(A->trans, scca, BA_MOD);
    dgraph* g_foldone = shrink_mod(A->trans, foldone, scca);


    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    dgraph_discard_nonscc_edges(inter, scci);
    parti* foldtwo = dgraph_stal_fold(inter, scci, BA_MOD);
    dgraph* g_foldtwo = shrink_mod(inter, foldtwo, scci);

    // For each SCC of the original graph
    for (uint j = 0; j < scca->size_par;j++) {
        // For each SCC of the product graph
        for (uint i = 0; i < scci->size_par;i++) {
            uint p = scci->cl_elems[i][0];
            uint p1 = p / A->trans->size_graph;
            uint p2 = p % A->trans->size_graph;

            // If p is not reachable from q, this cannot yield a counterexample
            if (p1 == p2 || j > scca->numcl[p1] || j > scca->numcl[p2]) {
                // printf("continued\n");
                continue;
            }

            // For every two states in the SCC j of A
            for (uint k = 0; k < scca->cl_size[j];k++) {
                uint x0 = scca->cl_elems[j][k];
                for (uint l = 0; l < scca->cl_size[j]; l++) {
                    uint x1 = scca->cl_elems[j][l];
                    if (x0 > x1) {
                        continue;
                    }

                    // For every two states in the SCC i of the product
                    for (uint m = 0; m < scci->cl_size[i];m++) {
                        uint x2x5 = scci->cl_elems[i][m];
                        uint x2 = x2x5 / A->trans->size_graph;
                        uint x5 = x2x5 % A->trans->size_graph;


                        for (uint n = 0; n < scci->cl_size[i]; n++) {
                            uint x3x4 = scci->cl_elems[i][n];
                            uint x3 = x3x4 / A->trans->size_graph;
                            uint x4 = x3x4 % A->trans->size_graph;


                            uint* x;



                            if (dgraph_exists_intersec_path(g_foldtwo, g_foldone, foldtwo->numcl[x3x4], foldone->numcl[x0], foldtwo->numcl[x2x5], foldone->numcl[x1], false, NULL) &&
                                dgraph_exists_intersec_path(inter, A->trans, x3x4, x0, x2x5, x2, false, NULL) &&
                                dgraph_exists_intersec_path(inter, A->trans, x2x5, x1, x3x4, x4, false, &x)) {
                                if (out) {
                                    uint* v;
                                    dgraph_exists_path(A->trans, x1, x0, false, &v);
                                    uint states[6] = { x0, x1, x2, x3, x4, x5 };
                                    char vars[2] = { 'v', 'x' };
                                    uint* words[2] = { v, x };
                                    view_pattern_example(A, 6, states, 2, vars, words, false, out);
                                    free(v);
                                }

                                delete_dgraph(inter);
                                delete_dgraph(g_foldone);
                                delete_dgraph(g_foldtwo);
                                delete_parti(scca);
                                delete_parti(scci);
                                delete_parti(foldone);
                                delete_parti(foldtwo);
                                return false;


                            }

                        }
                    }

                }
            }

        }

    }

    delete_dgraph(inter);
    delete_dgraph(g_foldone);
    delete_dgraph(g_foldtwo);
    delete_parti(scca);
    delete_parti(scci);
    delete_parti(foldone);
    delete_parti(foldtwo);
    return true;

}






bool is_bpolamt_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    num_span_forest* spanone = compute_span_forest(A->trans, scca, NULL);


    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    dgraph_discard_nonscc_edges(inter, scci);
    num_span_forest* spantwo = compute_span_forest(inter, scci, NULL);

    int target[A->trans->size_graph];

    // For each SCC of the original graph
    for (uint j = 0; j < scca->size_par;j++) {
        // For each SCC of the product graph
        for (uint i = 0; i < scci->size_par;i++) {
            uint p = scci->cl_elems[i][0];
            uint p1 = p / A->trans->size_graph;
            uint p2 = p % A->trans->size_graph;

            // If p is not reachable from q, this cannot yield a counterexample
            if (p1 == p2 || j > scca->numcl[p1] || j > scca->numcl[p2]) {
                // printf("continued\n");
                continue;
            }

            fmpz_mat_t MAT;
            build_hnf_matrix_two(A->trans, inter, spanone, spantwo, scca->cl_elems[j][0], p, MAT);

            // For every two states in the SCC j of A
            for (uint k = 0; k < scca->cl_size[j];k++) {
                uint x0 = scca->cl_elems[j][k];
                for (uint l = 0; l < scca->cl_size[j]; l++) {

                    uint x1 = scca->cl_elems[j][l];
                    if (x0 > x1) {
                        continue;
                    }

                    // For every two states in the SCC i of the product
                    for (uint m = 0; m < scci->cl_size[i];m++) {
                        uint x2x5 = scci->cl_elems[i][m];
                        uint x2 = x2x5 / A->trans->size_graph;
                        uint x5 = x2x5 % A->trans->size_graph;


                        for (uint n = 0; n < scci->cl_size[i]; n++) {
                            uint x3x4 = scci->cl_elems[i][n];
                            uint x3 = x3x4 / A->trans->size_graph;
                            uint x4 = x3x4 % A->trans->size_graph;



                            uint* x;

                            for (uint a = 0; a < A->trans->size_alpha; a++) {
                                target[a] = (spanone->span_forest[x1][a] - spanone->span_forest[x0][a]) -
                                    (spantwo->span_forest[x2x5][a] - spantwo->span_forest[x3x4][a]);
                            }



                            if (solve_system_amt(MAT, target, fmpz_mat_nrows(MAT), fmpz_mat_ncols(MAT)) &&
                                dgraph_exists_intersec_path(inter, A->trans, x3x4, x0, x2x5, x2, false, NULL) &&
                                dgraph_exists_intersec_path(inter, A->trans, x2x5, x1, x3x4, x4, false, &x)) {
                                if (out) {
                                    uint* v;
                                    dgraph_exists_path(A->trans, x1, x0, false, &v);
                                    uint states[6] = { x0, x1, x2, x3, x4, x5 };
                                    char vars[2] = { 'v', 'x' };
                                    uint* words[2] = { v, x };
                                    view_pattern_example(A, 6, states, 2, vars, words, false, out);
                                    free(v);
                                }

                                delete_dgraph(inter);
                                delete_parti(scca);
                                delete_parti(scci);
                                delete_span_forest(spanone);
                                delete_span_forest(spantwo);
                                fmpz_mat_clear(MAT);
                                return false;


                            }

                        }
                    }

                }
            }
            fmpz_mat_clear(MAT);

        }

    }
    delete_span_forest(spanone);
    delete_span_forest(spantwo);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}



bool is_bpoldd_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    dgraph_discard_nonscc_edges(inter, scci);
    uint* inv_scci = parti_compute_inv(scci);


    // For each SCC of the original graph
    for (uint j = 0; j < scca->size_par;j++) {
        // For each SCC of the product graph
        for (uint i = 0; i < scci->size_par;i++) {
            uint p = scci->cl_elems[i][0];
            uint p1 = p / A->trans->size_graph;
            uint p2 = p % A->trans->size_graph;

            // If p is not reachable from q, this cannot yield a counterexample
            if (p1 >= p2 || j > scca->numcl[p1] || j > scca->numcl[p2]) {
                // printf("continued\n");
                continue;
            }

            dgraph* sccone = dgraph_extract(A->trans, scca, inv_scca, j);
            dgraph* scctwo = dgraph_extract(inter, scci, inv_scci, i);



            dgraph* six = dgraph_direct_product(sccone, scctwo);
            parti* sccsix = dtarjan(six, NULL, false);
            delete_dgraph(sccone);
            delete_dgraph(scctwo);

            uint* ntloop;
            MALLOC(ntloop, sccsix->size_par);
            uint nb_ntloop = 0;

            for (uint l = 0; l < sccsix->size_par; l++) {
                if (dgraph_ntrivial_loop(six, sccsix, sccsix->cl_elems[l][0])) {
                    ntloop[nb_ntloop++] = l;
                }
            }

            for (uint n1 = 0; n1 < nb_ntloop; n1++) {
                uint l = ntloop[n1];

                for (uint n2 = 0; n2 < nb_ntloop; n2++) {
                    uint k = ntloop[n2];

                    uint x1x2x5 = sccsix->cl_elems[l][0];
                    //printf("x1x2x5 = %d\n", x1x2x5);
                    uint x1 = scca->cl_elems[j][x1x2x5 / scci->cl_size[i]];
                    uint x2x5 = scci->cl_elems[i][x1x2x5 % scci->cl_size[i]];
                    uint x2 = x2x5 / A->trans->size_graph;
                    uint x5 = x2x5 % A->trans->size_graph;

                    //for (uint y = 0; y < sccsix->cl_size[k]; y++) {

                    uint x0x3x4 = sccsix->cl_elems[k][0];
                    if (x0x3x4 > x1x2x5) {
                        continue;
                    }
                    //printf("x0x3x4 = %d\n", x0x3x4);
                    uint x0 = scca->cl_elems[j][x0x3x4 / scci->cl_size[i]];
                    uint x3x4 = scci->cl_elems[i][x0x3x4 % scci->cl_size[i]];
                    uint x3 = x3x4 / A->trans->size_graph;
                    uint x4 = x3x4 % A->trans->size_graph;

                    uint* w = NULL;
                    uint* x = NULL;

                    if (dgraph_exists_intersec_path(inter, A->trans, x3x4, x0, x2x5, x2, true, &w) &&
                        dgraph_exists_intersec_path(inter, A->trans, x2x5, x1, x3x4, x4, true, &x)) {
                        if (out) {
                            uint* u;
                            uint* v;
                            uint* yy;
                            uint* zz;
                            dgraph_exists_path(A->trans, x0, x1, true, &u);
                            dgraph_exists_path(A->trans, x1, x0, true, &v);
                            dgraph_exists_path(six, x0x3x4, x0x3x4, true, &yy);
                            dgraph_exists_path(six, x1x2x5, x1x2x5, true, &zz);
                            uint states[6] = { x0, x1, x2, x3, x4, x5 };
                            char vars[6] = { 'u', 'v', 'w', 'x', 'y', 'z' };
                            uint* words[6] = { u, v, w, x, yy, zz };
                            view_pattern_example(A, 6, states, 6, vars, words, false, out);
                            free(u);
                            free(v);
                            free(yy);
                            free(zz);
                        }

                        free(w);
                        free(x);
                        delete_dgraph(six);
                        delete_parti(sccsix);
                        free(inv_scca);
                        free(inv_scci);
                        delete_dgraph(inter);
                        delete_parti(scca);
                        delete_parti(scci);
                        free(ntloop);
                        return false;


                    }
                    free(w);
                    free(x);
                }
            }
            delete_dgraph(six);
            delete_parti(sccsix);
            free(ntloop);
        }
    }
    free(inv_scca);
    free(inv_scci);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}




bool is_bpolmodplus_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);
    parti* foldone = dgraph_stal_fold(A->trans, scca, BA_MOD);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    dgraph_discard_nonscc_edges(inter, scci);
    uint* inv_scci = parti_compute_inv(scci);
    parti* foldtwo = dgraph_stal_fold(inter, scci, BA_MOD);


    // For each SCC of the original graph
    for (uint j = 0; j < scca->size_par;j++) {
        // For each SCC of the product graph
        for (uint i = 0; i < scci->size_par;i++) {
            uint p = scci->cl_elems[i][0];
            uint p1 = p / A->trans->size_graph;
            uint p2 = p % A->trans->size_graph;

            // If p is not reachable from q, this cannot yield a counterexample
            if (p1 >= p2 || j > scca->numcl[p1] || j > scca->numcl[p2]) {
                // printf("continued\n");
                continue;
            }
            dgraph* sccone = dgraph_extract(A->trans, scca, inv_scca, j);
            dgraph* scctwo = dgraph_extract(inter, scci, inv_scci, i);
            dgraph* six = dgraph_direct_product(sccone, scctwo);
            parti* sccsix = dtarjan(six, NULL, false);
            delete_dgraph(sccone);
            delete_dgraph(scctwo);

            parti* foldsix = dgraph_stal_fold(six, sccsix, BA_MOD);

            uint* ntloop;
            MALLOC(ntloop, foldsix->size_par);
            uint nb_ntloop = 0;

            for (uint l = 0; l < foldsix->size_par; l++) {
                if (dgraph_ntrivial_loop(six, sccsix, foldsix->cl_elems[l][0])) {
                    ntloop[nb_ntloop++] = l;
                }
            }
            delete_parti(sccsix);

            for (uint n1 = 0; n1 < nb_ntloop; n1++) {
                uint l = ntloop[n1];

                for (uint n2 = 0; n2 < nb_ntloop; n2++) {
                    uint k = ntloop[n2];

                    uint x1x2x5 = foldsix->cl_elems[l][0];

                    uint x1 = scca->cl_elems[j][x1x2x5 / scci->cl_size[i]];
                    uint x2x5 = scci->cl_elems[i][x1x2x5 % scci->cl_size[i]];
                    uint x2 = x2x5 / A->trans->size_graph;
                    uint x5 = x2x5 % A->trans->size_graph;

                    uint x0x3x4 = foldsix->cl_elems[k][0];
                    if (x0x3x4 > x1x2x5) {
                        continue;
                    }

                    uint x0 = scca->cl_elems[j][x0x3x4 / scci->cl_size[i]];
                    uint x3x4 = scci->cl_elems[i][x0x3x4 % scci->cl_size[i]];
                    uint x3 = x3x4 / A->trans->size_graph;
                    uint x4 = x3x4 % A->trans->size_graph;

                    if (foldone->numcl[x0] != foldone->numcl[x1] ||
                        foldtwo->numcl[x2x5] != foldtwo->numcl[x3x4]) {
                        continue;
                    }


                    if (dgraph_exists_intersec_path(inter, A->trans, x3x4, x0, x2x5, x2, false, NULL) &&
                        dgraph_exists_intersec_path(inter, A->trans, x2x5, x1, x3x4, x4, false, NULL)) {
                        if (out) {
                            uint* yy;
                            uint* zz;
                            dgraph_exists_path(six, x0x3x4, x0x3x4, true, &yy);
                            dgraph_exists_path(six, x1x2x5, x1x2x5, true, &zz);
                            uint states[6] = { x0, x1, x2, x3, x4, x5 };
                            char vars[2] = { 'y', 'z' };
                            uint* words[2] = { yy, zz };
                            view_pattern_example(A, 6, states, 2, vars, words, false, out);
                            free(yy);
                            free(zz);
                        }
                        delete_dgraph(six);
                        free(inv_scca);
                        free(inv_scci);
                        delete_dgraph(inter);
                        delete_parti(scca);
                        delete_parti(scci);
                        delete_parti(foldone);
                        delete_parti(foldtwo);
                        delete_parti(foldsix);
                        free(ntloop);
                        return false;

                    }
                }




            }

            delete_dgraph(six);
            delete_parti(foldsix);
            free(ntloop);

        }
    }

    free(inv_scca);
    free(inv_scci);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    delete_parti(foldone);
    delete_parti(foldtwo);
    return true;

}



bool is_bpolamtplus_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);

    // Foldings
    num_span_forest* spanone = compute_span_forest(A->trans, scca, NULL);
    num_span_forest* spantwo = compute_span_forest(inter, scci, NULL);


    int target[A->trans->size_alpha];

    // For each SCC of the product graph
    for (uint i = 0; i < scci->size_par;i++) {
        uint p = scci->cl_elems[i][0];
        uint p1 = p / A->trans->size_graph;
        uint p2 = p % A->trans->size_graph;

        // We extract the graph of the SCC i of the product graph
        dgraph* scctwo = dgraph_extract(inter, scci, inv_scci, i);

        // For each SCC of the original graph
        for (uint j = 0; j < scca->size_par;j++) {

            // If p is not reachable from q, this cannot yield a counterexample
            if (p1 >= p2 || j > scca->numcl[p1] || j > scca->numcl[p2]) {
                continue;
            }

            // We extract the graph of the SCC j of the original graph
            dgraph* sccone = dgraph_extract(A->trans, scca, inv_scca, j);

            // We compute the direct product of the two SCCs (for the loops)
            dgraph* six = dgraph_direct_product(sccone, scctwo);
            parti* sccsix = dtarjan(six, NULL, false);
            delete_dgraph(sccone);

            fmpz_mat_t MAT;
            build_hnf_matrix_two(A->trans, inter, spanone, spantwo, scca->cl_elems[j][0], p, MAT);

            // We compute the indices of non-trivial sccs in six
            uint* ntloop;
            MALLOC(ntloop, sccsix->size_par);
            uint nb_ntloop = 0;
            for (uint l = 0; l < sccsix->size_par; l++) {
                if (dgraph_ntrivial_loop(six, sccsix, sccsix->cl_elems[l][0])) {
                    ntloop[nb_ntloop++] = l;
                }
            }

            // We now iterate over the non-trivial sccs in six
            for (uint n1 = 0; n1 < nb_ntloop; n1++) {

                uint x1x2x5 = sccsix->cl_elems[ntloop[n1]][0];
                // printf("x1x2x5 = %d\n", x1x2x5 / scctwo->size_graph);
                uint x1 = scca->cl_elems[j][x1x2x5 / scctwo->size_graph]; // State in A.
                uint x2x5 = x1x2x5 % scctwo->size_graph; // State in scctwo.
                uint inter_x2x5 = scci->cl_elems[i][x2x5]; // State in inter.
                uint x2 = inter_x2x5 / A->trans->size_graph; // State in A.
                uint x5 = inter_x2x5 % A->trans->size_graph; // State in A.

                for (uint n2 = 0; n2 < nb_ntloop; n2++) {

                    uint x0x3x4 = sccsix->cl_elems[ntloop[n2]][0];
                    // printf("x0x3x4 = %d\n", x0x3x4 / scctwo->size_graph);
                    uint x0 = scca->cl_elems[j][x0x3x4 / scctwo->size_graph]; // State in A.
                    uint x3x4 = x0x3x4 % scctwo->size_graph; // State in scctwo.
                    uint inter_x3x4 = scci->cl_elems[i][x3x4]; // State in inter.
                    uint x3 = inter_x3x4 / A->trans->size_graph; // State in A.
                    uint x4 = inter_x3x4 % A->trans->size_graph; // State in A.

                    uint* x = NULL;

                    for (uint a = 0; a < A->trans->size_alpha; a++) {
                        target[a] = (spanone->span_forest[x1][a] - spanone->span_forest[x0][a]) -
                            (spantwo->span_forest[inter_x2x5][a] - spantwo->span_forest[inter_x3x4][a]);
                    }

                    if (solve_system_amt(MAT, target, fmpz_mat_nrows(MAT), fmpz_mat_ncols(MAT)) &&
                        dgraph_exists_intersec_path(scctwo, A->trans, x3x4, x0, x2x5, x2, false, NULL) &&
                        dgraph_exists_intersec_path(scctwo, A->trans, x2x5, x1, x3x4, x4, true, &x)) {
                        if (out) {
                            uint* v;
                            uint* y;
                            uint* z;
                            dgraph_exists_path(A->trans, x1, x0, true, &v);
                            dgraph_exists_path(six, x0x3x4, x0x3x4, true, &y);
                            dgraph_exists_path(six, x1x2x5, x1x2x5, true, &z);
                            uint states[6] = { x0, x1, x2, x3, x4, x5 };
                            char vars[4] = { 'v', 'x', 'y', 'z' };
                            uint* words[4] = { v, x, y, z };
                            view_pattern_example(A, 6, states, 4, vars, words, false, out);
                            free(v);
                            free(y);
                            free(z);
                        }

                        free(x);
                        delete_dgraph(six);
                        delete_parti(sccsix);
                        free(ntloop);
                        delete_dgraph(scctwo);
                        free(inv_scca);
                        free(inv_scci);
                        delete_dgraph(inter);
                        delete_parti(scca);
                        delete_parti(scci);
                        delete_span_forest(spanone);
                        delete_span_forest(spantwo);
                        fmpz_mat_clear(MAT);
                        return false;
                    }
                    free(x);
                }
            }
            delete_dgraph(six);
            delete_parti(sccsix);
            fmpz_mat_clear(MAT);
            free(ntloop);

        }
        delete_dgraph(scctwo);
    }

    free(inv_scca);
    free(inv_scci);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    delete_span_forest(spanone);
    delete_span_forest(spantwo);
    return true;
}




bool is_bpolgrplus_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);

    // Foldings
    parti* foldone = dgraph_stal_fold(A->trans, scca, BA_GR);
    dgraph* g_foldone = shrink_grp(A->trans, foldone, scca);
    parti* foldtwo = dgraph_stal_fold(inter, scci, BA_GR);
    dgraph* g_foldtwo = shrink_grp(inter, foldtwo, scci);




    // For each SCC of the product graph
    for (uint i = 0; i < scci->size_par;i++) {
        uint p = scci->cl_elems[i][0];
        uint p1 = p / A->trans->size_graph;
        uint p2 = p % A->trans->size_graph;

        // We extract the graph of the SCC i of the product graph
        dgraph* scctwo = dgraph_extract(inter, scci, inv_scci, i);

        // For each SCC of the original graph
        for (uint j = 0; j < scca->size_par;j++) {

            // If p is not reachable from q, this cannot yield a counterexample
            if (p1 >= p2 || j > scca->numcl[p1] || j > scca->numcl[p2]) {
                continue;
            }

            // We extract the graph of the SCC j of the original graph
            dgraph* sccone = dgraph_extract(A->trans, scca, inv_scca, j);

            // We compute the direct product of the two SCCs (for the loops)
            dgraph* six = dgraph_direct_product(sccone, scctwo);
            parti* sccsix = dtarjan(six, NULL, false);
            delete_dgraph(sccone);

            // We compute the indices of non-trivial sccs in six
            uint* ntloop;
            MALLOC(ntloop, sccsix->size_par);
            uint nb_ntloop = 0;
            for (uint l = 0; l < sccsix->size_par; l++) {
                if (dgraph_ntrivial_loop(six, sccsix, sccsix->cl_elems[l][0])) {
                    ntloop[nb_ntloop++] = l;
                }
            }

            // We now iterate over the non-trivial sccs in six
            for (uint n1 = 0; n1 < nb_ntloop; n1++) {

                uint x1x2x5 = sccsix->cl_elems[ntloop[n1]][0];
                // printf("x1x2x5 = %d\n", x1x2x5 / scctwo->size_graph);
                uint x1 = scca->cl_elems[j][x1x2x5 / scctwo->size_graph]; // State in A.
                uint x2x5 = x1x2x5 % scctwo->size_graph; // State in scctwo.
                uint inter_x2x5 = scci->cl_elems[i][x2x5]; // State in inter.
                uint x2 = inter_x2x5 / A->trans->size_graph; // State in A.
                uint x5 = inter_x2x5 % A->trans->size_graph; // State in A.

                for (uint n2 = 0; n2 < nb_ntloop; n2++) {

                    uint x0x3x4 = sccsix->cl_elems[ntloop[n2]][0];
                    // printf("x0x3x4 = %d\n", x0x3x4 / scctwo->size_graph);
                    uint x0 = scca->cl_elems[j][x0x3x4 / scctwo->size_graph]; // State in A.
                    uint x3x4 = x0x3x4 % scctwo->size_graph; // State in scctwo.
                    uint inter_x3x4 = scci->cl_elems[i][x3x4]; // State in inter.
                    uint x3 = inter_x3x4 / A->trans->size_graph; // State in A.
                    uint x4 = inter_x3x4 % A->trans->size_graph; // State in A.

                    uint* x = NULL;

                    if (dgraph_exists_intersec_path(g_foldtwo, g_foldone, foldtwo->numcl[inter_x3x4], foldone->numcl[x0], foldtwo->numcl[inter_x2x5], foldone->numcl[x1], false, NULL) &&
                        dgraph_exists_intersec_path(scctwo, A->trans, x3x4, x0, x2x5, x2, false, NULL) &&
                        dgraph_exists_intersec_path(scctwo, A->trans, x2x5, x1, x3x4, x4, true, &x)) {
                        if (out) {
                            uint* v;
                            uint* y;
                            uint* z;
                            dgraph_exists_path(A->trans, x1, x0, true, &v);
                            dgraph_exists_path(six, x0x3x4, x0x3x4, true, &y);
                            dgraph_exists_path(six, x1x2x5, x1x2x5, true, &z);
                            uint states[6] = { x0, x1, x2, x3, x4, x5 };
                            char vars[4] = { 'v', 'x', 'y', 'z' };
                            uint* words[4] = { v, x, y, z };
                            view_pattern_example(A, 6, states, 4, vars, words, false, out);
                            free(v);
                            free(y);
                            free(z);
                        }

                        free(x);
                        delete_dgraph(six);
                        delete_parti(sccsix);
                        free(ntloop);
                        delete_dgraph(scctwo);
                        free(inv_scca);
                        free(inv_scci);
                        delete_dgraph(inter);
                        delete_parti(scca);
                        delete_parti(scci);
                        delete_parti(foldone);
                        delete_parti(foldtwo);
                        delete_dgraph(g_foldone);
                        delete_dgraph(g_foldtwo);
                        return false;
                    }
                    free(x);
                }
            }
            delete_dgraph(six);
            delete_parti(sccsix);
            free(ntloop);

        }
        delete_dgraph(scctwo);
    }

    free(inv_scca);
    free(inv_scci);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    delete_parti(foldone);
    delete_parti(foldtwo);
    delete_dgraph(g_foldone);
    delete_dgraph(g_foldtwo);
    return true;

}

bool is_knastgpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_bpoldd_dfa(A, error, out);
    }


    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);
    parti* foldone = dgraph_stal_fold(A->trans, scca, mode);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    dgraph_discard_nonscc_edges(inter, scci);
    uint* inv_scci = parti_compute_inv(scci);
    parti* foldtwo = dgraph_stal_fold(inter, scci, mode);


    // For each SCC of the original graph
    for (uint j = 0; j < scca->size_par;j++) {
        // For each SCC of the product graph
        for (uint i = 0; i < scci->size_par;i++) {
            uint p = scci->cl_elems[i][0];
            uint p1 = p / A->trans->size_graph;
            uint p2 = p % A->trans->size_graph;

            // If p is not reachable from q, this cannot yield a counterexample
            if (p1 >= p2 || j > scca->numcl[p1] || j > scca->numcl[p2]) {
                // printf("continued\n");
                continue;
            }
            dgraph* sccone = dgraph_extract(A->trans, scca, inv_scca, j);
            dgraph* scctwo = dgraph_extract(inter, scci, inv_scci, i);
            dgraph* six = dgraph_direct_product(sccone, scctwo);
            parti* sccsix = dtarjan(six, NULL, false);
            delete_dgraph(sccone);
            delete_dgraph(scctwo);

            parti* foldsix = dgraph_stal_fold(six, sccsix, mode);

            uint* ntloop;
            MALLOC(ntloop, foldsix->size_par);
            uint nb_ntloop = 0;

            for (uint l = 0; l < foldsix->size_par; l++) {
                if (dgraph_ntrivial_loop(six, sccsix, foldsix->cl_elems[l][0])) {
                    ntloop[nb_ntloop++] = l;
                }
            }
            delete_parti(sccsix);

            for (uint n1 = 0; n1 < nb_ntloop; n1++) {
                uint l = ntloop[n1];

                for (uint n2 = 0; n2 < nb_ntloop; n2++) {
                    uint k = ntloop[n2];

                    uint x1x2x5 = foldsix->cl_elems[l][0];

                    uint x1 = scca->cl_elems[j][x1x2x5 / scci->cl_size[i]];
                    uint x2x5 = scci->cl_elems[i][x1x2x5 % scci->cl_size[i]];
                    uint x2 = x2x5 / A->trans->size_graph;
                    uint x5 = x2x5 % A->trans->size_graph;

                    uint x0x3x4 = foldsix->cl_elems[k][0];
                    if (x0x3x4 > x1x2x5) {
                        continue;
                    }

                    uint x0 = scca->cl_elems[j][x0x3x4 / scci->cl_size[i]];
                    uint x3x4 = scci->cl_elems[i][x0x3x4 % scci->cl_size[i]];
                    uint x3 = x3x4 / A->trans->size_graph;
                    uint x4 = x3x4 % A->trans->size_graph;

                    if (foldone->numcl[x0] != foldone->numcl[x1] ||
                        foldtwo->numcl[x2x5] != foldtwo->numcl[x3x4]) {
                        continue;
                    }


                    if (dgraph_exists_intersec_path(inter, A->trans, x3x4, x0, x2x5, x2, false, NULL) &&
                        dgraph_exists_intersec_path(inter, A->trans, x2x5, x1, x3x4, x4, false, NULL)) {
                        if (out) {
                            uint* yy;
                            uint* zz;
                            dgraph_exists_path(six, x0x3x4, x0x3x4, true, &yy);
                            dgraph_exists_path(six, x1x2x5, x1x2x5, true, &zz);
                            uint states[6] = { x0, x1, x2, x3, x4, x5 };
                            char vars[2] = { 'y', 'z' };
                            uint* words[2] = { yy, zz };
                            view_pattern_example(A, 6, states, 2, vars, words, false, out);
                            free(yy);
                            free(zz);
                        }
                        delete_dgraph(six);
                        free(inv_scca);
                        free(inv_scci);
                        delete_dgraph(inter);
                        delete_parti(scca);
                        delete_parti(scci);
                        delete_parti(foldone);
                        delete_parti(foldtwo);
                        delete_parti(foldsix);
                        free(ntloop);
                        return false;

                    }
                }




            }

            delete_dgraph(six);
            delete_parti(foldsix);
            free(ntloop);

        }
    }

    free(inv_scca);
    free(inv_scci);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    delete_parti(foldone);
    delete_parti(foldtwo);
    return true;
}


bool is_knastat_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    dgraph_discard_nonscc_edges(inter, scci);
    uint* inv_scci = parti_compute_inv(scci);

    bool alphabet[A->trans->size_alpha];
    // For each SCC of the original graph
    for (uint j = 0; j < scca->size_par;j++) {
        // For each SCC of the product graph
        for (uint i = 0; i < scci->size_par;i++) {
            uint p = scci->cl_elems[i][0];
            uint p1 = p / A->trans->size_graph;
            uint p2 = p % A->trans->size_graph;

            // If p is not reachable from q, this cannot yield a counterexample
            if (p1 == p2 || j > scca->numcl[p1] || j > scca->numcl[p2]) {
                // printf("continued\n");
                continue;
            }

            dgraph* sccone = dgraph_extract(A->trans, scca, inv_scca, j);
            dgraph* scctwo = dgraph_extract(inter, scci, inv_scci, i);


            dgraph* six = dgraph_direct_product(sccone, scctwo);
            parti* sccsix = dtarjan(six, NULL, false);
            uint* inv_sccsix = parti_compute_inv(sccsix);
            delete_dgraph(sccone);
            delete_dgraph(scctwo);

            //view_dgraph(six);
            for (uint l = 0; l < sccsix->size_par; l++) {
                //printf("l = %d\n", l);

                if (!dgraph_ntrivial_loop(six, sccsix, sccsix->cl_elems[l][0])) {
                    continue;
                }

                for (uint k = 0; k < sccsix->size_par; k++) {
                    //printf("k = %d\n", k);

                    if (!dgraph_ntrivial_loop(six, sccsix, sccsix->cl_elems[k][0])) {
                        continue;
                    }



                    for (uint z = 0; z < sccsix->cl_size[l]; z++) {

                        uint x1x2x5 = sccsix->cl_elems[l][z];
                        //printf("x1x2x5 = %d\n", x1x2x5);
                        uint x1 = scca->cl_elems[j][x1x2x5 / scci->cl_size[i]];
                        uint x2x5 = scci->cl_elems[i][x1x2x5 % scci->cl_size[i]];
                        uint x2 = x2x5 / A->trans->size_graph;
                        uint x5 = x2x5 % A->trans->size_graph;

                        for (uint y = 0; y < sccsix->cl_size[k]; y++) {



                            uint x0x3x4 = sccsix->cl_elems[k][y];

                            if (x0x3x4 > x1x2x5) {
                                continue;
                            }

                            //printf("x0x3x4 = %d\n", x0x3x4);
                            uint x0 = scca->cl_elems[j][x0x3x4 / scci->cl_size[i]];
                            uint x3x4 = scci->cl_elems[i][x0x3x4 % scci->cl_size[i]];
                            uint x3 = x3x4 / A->trans->size_graph;
                            uint x4 = x3x4 % A->trans->size_graph;

                            dgraph_common_alph_loop(six, sccsix, inv_sccsix, x0x3x4, x1x2x5, alphabet);

                            uint* u = NULL;
                            uint* v = NULL;
                            uint* w = NULL;
                            uint* x = NULL;

                            if (dgraph_exists_path_alpha(A->trans, x0, x1, false, alphabet, &u) &&
                                dgraph_exists_path_alpha(A->trans, x1, x0, false, alphabet, &v) &&
                                dgraph_exists_intersec_path_alpha(inter, A->trans, x3x4, x0, x2x5, x2, false, alphabet, &w) &&
                                dgraph_exists_intersec_path_alpha(inter, A->trans, x2x5, x1, x3x4, x4, false, alphabet, &x)) {
                                if (out) {
                                    //parti* ressccsix = dtarjan(six, alphabet, false);
                                    uint* yy = dfacexa_get_alphaloop(six, sccsix, alphabet, x0x3x4);
                                    uint* zz = dfacexa_get_alphaloop(six, sccsix, alphabet, x1x2x5);
                                    //delete_parti(ressccsix);
                                    uint states[6] = { x0, x1, x2, x3, x4, x5 };
                                    char vars[6] = { 'u', 'v', 'w', 'x', 'y', 'z' };
                                    uint* words[6] = { u, v, w, x, yy, zz };
                                    view_pattern_example(A, 6, states, 6, vars, words, false, out);
                                    free(yy);
                                    free(zz);
                                }

                                free(u);
                                free(v);
                                free(w);
                                free(x);
                                delete_dgraph(six);
                                delete_parti(sccsix);
                                free(inv_scca);
                                free(inv_scci);
                                delete_dgraph(inter);
                                delete_parti(scca);
                                delete_parti(scci);
                                free(inv_sccsix);
                                return false;


                            }
                            free(u);
                            free(v);
                            free(w);
                            free(x);
                        }
                    }
                }




            }

            delete_dgraph(six);
            delete_parti(sccsix);
            free(inv_sccsix);

        }
    }

    free(inv_scca);
    free(inv_scci);
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}




/**********/
/*+ UPol +*/
/**********/


static bool is_upoldd_dfa(dfa* A, int*, FILE* out) {
    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);

    for (uint i = 0; i < scci->size_par; i++) {
        uint qr = scci->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;
        if (q == r || scca->numcl[q] > scca->numcl[r] || !dgraph_ntrivial_loop(inter, scci, qr)) {
            continue;
        }

        uint* u = NULL;
        if (dgraph_exists_path(A->trans, q, r, true, &u)) {
            if (out) {
                uint states[2] = { q, r };
                char vars[2] = { 'u', 'z' };
                uint* z = NULL;
                dgraph_exists_path(inter, qr, qr, true, &z);
                uint* words[2] = { u, z };
                view_pattern_example(A, 2, states, 2, vars, words, false, out);
                free(z);
            }
            free(u);
            delete_dgraph(inter);
            delete_parti(scca);
            delete_parti(scci);
            return false;
        }



    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}

bool is_upolgpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    if (mode == BA_ST) {
        return is_upoldd_dfa(A, error, out);
    }


    parti* scca = dtarjan(A->trans, NULL, false);
    uint* inv_scca = parti_compute_inv(scca);
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);


    for (uint i = 0; i < scca->size_par; i++) {
        if (scca->cl_size[i] <= 1) {
            continue;
        }
        // Folding the SCC according to the mode
        dgraph* thescc = dgraph_extract(A->trans, scca, inv_scca, i);
        parti* fold = dgraph_stal_fold(thescc, NULL, mode);
        delete_dgraph(thescc);

        for (uint j = 0; j < scca->cl_size[i]; j++) {
            for (uint k = j + 1; k < scca->cl_size[i]; k++) {
                if (fold->numcl[j] != fold->numcl[k]) {
                    // If the two elements are not in the same fold class, we can continue.
                    continue;
                }
                uint q = scca->cl_elems[i][j];
                uint r = scca->cl_elems[i][k];
                uint qr = q * A->trans->size_graph + r;
                if (dgraph_ntrivial_loop(inter, scci, qr)) {
                    if (out) {
                        uint* z;
                        dgraph_exists_intersec_path(A->trans, A->trans, q, r, q, r, true, &z);
                        uint states[2] = { q, r };
                        char vars[1] = { 'z' };
                        uint* words[1] = { z };
                        view_pattern_example(A, 2, states, 1, vars, words, false, out);
                        free(z);
                    }
                    delete_dgraph(inter);
                    delete_parti(scca);
                    delete_parti(scci);
                    delete_parti(fold);
                    free(inv_scca);
                    return false;
                }

            }
        }
        delete_parti(fold);
    }


    for (uint i = 0; i < scci->size_par; i++) {
        uint qr = scci->cl_elems[i][0];

        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;

        if (q == r || scca->numcl[q] > scca->numcl[r] || !dgraph_ntrivial_loop(inter, scci, qr)) {
            continue;
        }
        uint* u = NULL;
        if (dgraph_exists_intersec_path(A->trans, A->trans, r, q, r, r, true, &u)) {
            if (out) {
                uint states[2] = { q, r };
                char vars[2] = { 'u', 'z' };
                uint* z = NULL;
                dgraph_exists_path(inter, qr, qr, true, &z);
                uint* words[2] = { u, z };
                view_pattern_example(A, 2, states, 2, vars, words, false, out);
                free(z);
            }
            free(u);
            delete_dgraph(inter);
            delete_parti(scca);
            delete_parti(scci);
            return false;
        }
    }

    delete_dgraph(inter);
    delete_parti(scci);
    delete_parti(scca);
    free(inv_scca);
    return true;
}




static bool is_upolbp2st_dfa(dfa* A, int*, FILE* out) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);

    // Array used to store alphabets.
    bool alpha[A->trans->size_alpha];

    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {

        // We discard the SCC if it cannot give rise to a counterexample.
        // (pairs of the form (q, q) are not interesting)
        // (pairs of the form (q, r) where scca->numcl[q] > scca->numcl[r] are not interesting either as r is not reachable from r in this case).
        uint t = scci->cl_elems[i][0];
        uint t1 = t / A->trans->size_graph;
        uint t2 = t % A->trans->size_graph;
        if (t1 == t2 || scca->numcl[t1] > scca->numcl[t2]) {
            continue;
        }

        // We compute the local double product graph for this SCC
        dgraph* thescc = dgraph_extract(inter, scci, inv_scci, i);
        dgraph* quad = dgraph_direct_product(thescc, thescc);
        parti* sccq = dtarjan(quad, NULL, false);


        for (uint j = 0; j < sccq->size_par; j++) {
            uint x0x2x1x3 = sccq->cl_elems[j][0];
            if (!dgraph_ntrivial_loop(quad, sccq, x0x2x1x3)) {
                continue;
            }
            dgraph_compute_alph_scc(quad, sccq, j, alpha);
            uint x0x2 = x0x2x1x3 / thescc->size_graph;
            uint x1x3 = x0x2x1x3 % thescc->size_graph;
            uint x0 = scci->cl_elems[i][x0x2] / A->trans->size_graph;
            uint x2 = scci->cl_elems[i][x0x2] % A->trans->size_graph;
            uint x1 = scci->cl_elems[i][x1x3] / A->trans->size_graph;
            uint x3 = scci->cl_elems[i][x1x3] % A->trans->size_graph;

            uint* v = NULL;
            uint* u = NULL;

            if (dgraph_exists_path_alpha(thescc, x0x2, x1x3, false, alpha, &u) &&
                dgraph_exists_intersec_path(thescc, A->trans, x1x3, x0, x0x2, x2, false, &v)) {
                if (out) {
                    uint states[4] = { x0, x1, x2, x3 };
                    char vars[3] = { 'u', 'v', 'w' };
                    uint* w = dfacexa_get_alphaloop(quad, sccq, alpha, x0x2x1x3);
                    uint* words[3] = { u, v, w };
                    view_pattern_example(A, 4, states, 3, vars, words, false, out);
                    free(w);
                }
                free(u);
                free(v);
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                delete_dgraph(quad);
                delete_parti(sccq);
                delete_dgraph(thescc);
                free(inv_scci);
                return false;
            }
            free(u);
            free(v);

        }
        delete_dgraph(thescc);
        delete_dgraph(quad);
        delete_parti(sccq);

    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    free(inv_scci);
    return true;

}



bool is_upolbp2gp_dfa(dfa* A, basis mode, int* error, FILE* out) {
    // If the mode is BA_ST, we call the dedicated function.
    if (mode == BA_ST) {
        return is_upolbp2st_dfa(A, error, out);
    }

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    dgraph* inter = dgraph_direct_product(A->trans, A->trans);
    parti* scci = dtarjan(inter, NULL, false);
    uint* inv_scci = parti_compute_inv(scci);

    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {

        // We discard the SCC if it cannot give rise to a counterexample.
        // (pairs of the form (q, q) are not interesting)
        // (pairs of the form (q, r) where scca->numcl[q] > scca->numcl[r] are not interesting either as r is not reachable from r in this case).
        uint t = scci->cl_elems[i][0];
        uint t1 = t / A->trans->size_graph;
        uint t2 = t % A->trans->size_graph;
        if (t1 == t2 || scca->numcl[t1] > scca->numcl[t2]) {
            continue;
        }


        // We extract the SCC as a single dgraph.
        dgraph* thescc = dgraph_extract(inter, scci, inv_scci, i);

        // We compute the local double product graph for this SCC
        dgraph* quad = dgraph_direct_product(thescc, thescc);
        parti* sccq = dtarjan(quad, NULL, false);

        // We compute the folding of the double product graph according to the mode.
        parti* fold = dgraph_stal_fold(quad, sccq, mode);
        dgraph* g_fold = dgraph_implement_fold(quad, sccq, fold);

        // dgraph* big = dgraph_direct_product(thescc, A->trans);

        // We pick a pair of distinct states in the SCC. 
        for (uint j = 0; j < sccq->size_par; j++) {
            if (!dgraph_ntrivial_loop(quad, sccq, sccq->cl_elems[j][0])) {
                continue;
            }
            uint qstate = sccq->cl_elems[j][0];
            uint x0x2 = qstate / scci->cl_size[i];
            uint x1x3 = qstate % scci->cl_size[i];

            uint x0 = scci->cl_elems[i][x0x2] / A->trans->size_graph;
            uint x2 = scci->cl_elems[i][x0x2] % A->trans->size_graph;
            uint x1 = scci->cl_elems[i][x1x3] / A->trans->size_graph;
            uint x3 = scci->cl_elems[i][x1x3] % A->trans->size_graph;

            // uint big1 = x1x3 * A->trans->size_graph + x0;
            // uint big2 = x0x2 * A->trans->size_graph + x2;

            uint* v = NULL;

            if (dgraph_exists_intersec_path(thescc, g_fold, x0x2, fold->numcl[qstate], x1x3, fold->numcl[qstate], false, NULL) &&
                dgraph_exists_intersec_path(thescc, A->trans, x1x3, x0, x0x2, x2, false, &v)) {
                if (out) {
                    uint states[4] = { x0, x1, x2, x3 };
                    char vars[1] = { 'v' };
                    uint* words[1] = { v };
                    view_pattern_example(A, 4, states, 1, vars, words, false, out);
                }
                free(v);
                delete_dgraph(g_fold);
                delete_parti(fold);
                delete_dgraph(inter);
                delete_parti(scca);
                delete_parti(scci);
                free(inv_scci);
                delete_dgraph(thescc);
                delete_dgraph(quad);
                delete_parti(sccq);
                return false;
            }
            free(v);

        }
        delete_dgraph(g_fold);
        delete_parti(fold);
        delete_dgraph(thescc);
        delete_dgraph(quad);
        delete_parti(sccq);
    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    free(inv_scci);
    return true;

}




bool is_upolbp2gpplus_dfa(dfa* A, basis mode, int* error, FILE* out) {
    uint* funs;
    morphism* M = dfa_to_morphism(A, true, error, &funs);
    orbits* bpolorbs = compute_bpgplusorbits(M, LV_GREG, mode);
    uint c[4];

    if (is_upbp_mono(bpolorbs, c)) {
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        return true;
    }
    else {
        if (out) {
            uint ese = c[0];
            uint t = c[1];
            uint f = c[3];
            uint ftf = mor_mult_gen(M, 3, f, t, f);

            uint qi = 0;
            while (qi < A->trans->size_graph) {
                if (funs[A->trans->size_graph * f + qi] != funs[A->trans->size_graph * ftf + qi]) {
                    break;
                }
                qi++;
            }

            uint q0 = funs[A->trans->size_graph * f + qi];
            uint q1 = funs[A->trans->size_graph * ese + q0];
            uint q2 = funs[A->trans->size_graph * t + q0];
            uint q3 = funs[A->trans->size_graph * ese + q2];

            uint states[4] = { q0, q1, q2, q3 };
            char vars[1] = { 'v' };
            uint* v = word_from_morphism(M, t);
            uint* words[1] = { v };
            view_pattern_example(A, 4, states, 1, vars, words, false, out);
            free(v);
        }
        delete_morphism(M);
        free(funs);
        delete_orbits(bpolorbs);
        return false;
    }

}
