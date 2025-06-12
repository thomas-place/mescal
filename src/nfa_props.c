#include "nfa_props.h"
#include "nfa_intersec.h"
#include "monoid_display.h"
#include "limits.h"

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

// Pour l'automate minimal
bool is_letterind_dfa(dfa* A, int*, FILE* out) {
    for (uint s = 0; s < A->trans->size_graph; s++) {
        for (uint a = 1; a < A->trans->size_alpha; a++) {
            if (A->trans->edges[s][0] != A->trans->edges[s][a]) {
                if (out != NULL) {
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
                    return false;
                }
            }
        }
    }
    return true;
}

// Teste si un DFA est commutatif
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
                        fprintf(out, "#### The DFA contains the following transitions: \n");
                        fprintf(out, "     (");
                        dfa_print_state(A, q, out);
                        fprintf(out, ",");
                        fprint_letter_utf8(A->alphabet[a], out);
                        fprintf(out, ",");
                        dfa_print_state(A, qa, out);
                        fprintf(out, ") and (");
                        dfa_print_state(A, qa, out);
                        fprintf(out, ",");
                        fprint_letter_utf8(A->alphabet[b], out);
                        fprintf(out, ",");
                        dfa_print_state(A, qab, out);
                        fprintf(out, ")\n");

                        fprintf(out, "     (");
                        dfa_print_state(A, q, out);
                        fprintf(out, ",");
                        fprint_letter_utf8(A->alphabet[b], out);
                        fprintf(out, ",");
                        dfa_print_state(A, qb, out);
                        fprintf(out, ") and (");
                        dfa_print_state(A, qb, out);
                        fprintf(out, ",");
                        fprint_letter_utf8(A->alphabet[a], out);
                        fprintf(out, ",");
                        dfa_print_state(A, qba, out);
                        fprintf(out, ")\n");
                    }
                    return false;
                }
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
                if (out != NULL) {
                    fprintf(out, "#### The DFA contains the transitions (");
                    dfa_print_state(A, reached[r], out);
                    fprintf(out, ",");
                    fprint_letter_utf8(A->alphabet[a], out);
                    fprintf(out, ",");
                    dfa_print_state(A, r, out);
                    fprintf(out, ") and (");
                    dfa_print_state(A, q, out);
                    fprintf(out, ",");
                    fprint_letter_utf8(A->alphabet[a], out);
                    fprintf(out, ",");
                    dfa_print_state(A, r, out);
                    fprintf(out, ").\n");
                }
                free(reached);
                return false;
            }
        }



    }
    free(reached);
    return true;
}

// Retourne l'état atteint à partir de s en lisant le mot w dans le DFA A
// (le déterminisme n'est pas vérifié)
static uint dfa_function(dfa* A, uint s, dequeue* w) {
    uint e = s;
    for (uint i = 0; i < size_dequeue(w); i++) {
        e = A->trans->edges[e][lefread_dequeue(w, i)];
    }
    return e;
}

// Génération d'un nfa à partir d'un lgraph et d'un unique état initial
// Seuls les états accessibles sont conservés. Si l'automate n'est pas
// complet, celui-ci est complété en ajoutant un état supplémentaire.
static dfa* dfa_from_scc(dfa* A, parti* sccs, uint p) {
    dfa* D;
    CALLOC(D, 1);
    D->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    D->trans = create_dgraph_noedges(sccs->cl_size[p] + 1, A->trans->size_alpha);
    A->nb_finals = 0;

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



bool is_counterfree_dfa(dfa* A, int* error, FILE* out) {


    /* We consider each SCC in the automaton, and check that none of them contains a counter. */

    // First compute the SCCs of the DFA.
    parti* PSCCS = dtarjan(A->trans, NULL, false);

    // Consider each SCC independently.
    for (uint c = 0; c < PSCCS->size_par; c++) {
        // Compute the automaton obtained by only keeping this SCC
        dfa* D = dfa_from_scc(A, PSCCS, c);

        // We compute the associated morphism.

        *error = 0;

        morphism* M = dfa_to_morphism(D, NULL, error);
        if (*error < 0) {
            return false;
        }

        // On calcule ses relations de Green
        green* GREL = M->rels;

        // Si il existe une H-classe non-triviale, on sait que la SCC contient un compteur
        if (GREL->HCL->size_set != GREL->HCL->size_par) {
            // Si on doit afficher un exemple de compteur
            if (out != NULL) {
                // On cherche un groupe non-trivial dans le monoide
                for (uint i = 0; i < M->nb_idems; i++) {
                    uint cl = GREL->HCL->numcl[M->idem_list[i]];
                    if (GREL->HCL->cl_size[cl] > 1) {
                        // On prend un élément de ce groupe qui n'est pas le neutre
                        uint s = GREL->HCL->cl_elems[cl][0];
                        if (M->idem_array[s]) {
                            s = GREL->HCL->cl_elems[cl][1];
                        }

                        // Cet élément satisfiait s^omega != s^omega+1. Donc son nom étiquette un compteur

                        fprintf(out, "#### Found a counter in this automaton.\n");
                        fprintf(out, "#### Word labeling the counter: ");
                        mor_fprint_name_utf8(M, s, out);
                        printf(".\n");
                        // On va maintenant trouver le compteur. On prend d'abord le graph obtenu à partir du DFA en ne lisant que word
                        fprintf(out, "#### The counter itself: ");

                        dequeue* name = mor_name(M, s);

                        graph* GW = create_graph_noedges(D->trans->size_graph);
                        for (uint q = 0; q < GW->size; q++) {
                            lefins_dequeue(dfa_function(D, q, name), GW->edges[q]);
                        }
                        delete_dequeue(name);
                        // graph_printing_test(GW, stdout);

                        // Le compteur recherché est une SCC non triviale dans ce graphe (qui sera un cycle)
                        parti* CYCLES = tarjan(GW);
                        for (uint j = 0; j < CYCLES->size_par; j++) {
                            if (CYCLES->cl_size[j] > 1) {
                                uint q = CYCLES->cl_elems[j][0];
                                dfa_print_state(A, PSCCS->cl_elems[c][q], out);
                                uint r = lefread_dequeue(GW->edges[q], 0);
                                while (r != q) {
                                    fprintf(out, " -> ");
                                    dfa_print_state(A, PSCCS->cl_elems[c][r], out);
                                    r = lefread_dequeue(GW->edges[r], 0);
                                }
                                fprintf(out, " -> ");
                                dfa_print_state(A, PSCCS->cl_elems[c][q], out);
                                fprintf(out, "\n");
                                break;
                            }
                        }
                        delete_parti(CYCLES);
                        delete_graph(GW);
                        dfa_delete(D);

                        delete_morphism(M);
                        return false;
                    }
                }
            }
            dfa_delete(D);
            delete_morphism(M);
            return false;
        }
    }
    return true;
}



static void compute_alph_scc_dgraph(dgraph* G, parti* SCC, uint num, bool* alph) {


    for (uint a = 0; a < G->size_alpha; a++) {
        alph[a] = false;
    }
    for (uint i = 0; i < SCC->cl_size[num]; i++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            if (SCC->numcl[G->edges[SCC->cl_elems[num][i]][a]] == num) {
                alph[a] = true;
            }
        }
    }
}



static void dgraph_make_product_sccs(dgraph* g, parti* scca, dgraph** inter, parti** scci) {
    uint size = g->size_graph * g->size_graph;
    *inter = create_dgraph_noedges(size + 1, g->size_alpha);
    for (uint q = 0; q < g->size_graph; q++) {
        for (uint r = 0; r < g->size_graph; r++) {
            for (uint a = 0; a < g->size_alpha; a++) {
                uint qa = g->edges[q][a];
                uint ra = g->edges[r][a];
                if (scca->numcl[q] == scca->numcl[qa] && scca->numcl[r] == scca->numcl[ra]) {
                    (*inter)->edges[q * g->size_graph + r][a] = qa * g->size_graph + ra;
                }
                else {
                    (*inter)->edges[q * g->size_graph + r][a] = size;
                }
            }
        }
    }
    for (uint a = 0; a < g->size_alpha; a++) {
        (*inter)->edges[size][a] = size;
    }

    // Computes the SCCs of the product graph
    *scci = dtarjan(*inter, NULL, false);

    // Discards all edges that go from a state to a different SCC in the product graph
    // (we make them go to the artificial sink)
    for (uint s = 0; s < (*inter)->size_graph; s++) {
        for (uint a = 0; a < (*inter)->size_alpha; a++) {
            uint t = (*inter)->edges[s][a];
            if ((*scci)->numcl[t] != (*scci)->numcl[s]) {
                (*inter)->edges[s][a] = size;
            }
        }
    }
}



bool is_da_dfa(dfa* A, int*) {

    parti* scca = dtarjan(A->trans, NULL, false);

    uint size = A->trans->size_graph * A->trans->size_graph;
    dgraph* inter;
    parti* scci;
    dgraph_make_product_sccs(A->trans, scca, &inter, &scci);


    bool alpha[A->trans->size_alpha];
    for (uint i = 0; i < scci->size_par; i++) {
        if (i == scci->numcl[size]) {
            continue;
        }
        uint qr = scci->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;
        if (q == r || scca->numcl[q] > scca->numcl[r]) {
            continue;
        }

        compute_alph_scc_dgraph(inter, scci, i, alpha);
        if (dfa_exists_path(A, q, r, alpha)) {
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



static dgraph* dgraph_extract(dgraph* G, parti* P, uint j) {
    // Création du graphe
    dgraph* res = create_dgraph_noedges(P->cl_size[j], G->size_alpha);
    // Pour chaque élément du morphisme
    for (uint i = 0; i < P->cl_size[j]; i++) {
        uint q = P->cl_elems[j][i];
        for (uint a = 0; a < G->size_alpha; a++) {
            uint r = G->edges[q][a];
            if (P->numcl[r] == j) {
                mem_array_sorted(r, P->cl_elems[j], P->cl_size[j], &res->edges[i][a]);
            }
            else {
                res->edges[i][a] = UINT_MAX;
            }
        }
    }
    return res;
}


static void dgraph_make_product_sccs_local(dgraph* G, parti* P, uint j, dgraph** rinter, parti** rscci) {
    dgraph* res = dgraph_extract(G, P, j);
    uint size = P->cl_size[j] * P->cl_size[j];
    dgraph* inter = create_dgraph_noedges(size + 1, G->size_alpha);
    for (uint q = 0; q < P->cl_size[j]; q++) {
        for (uint r = 0; r < P->cl_size[j]; r++) {
            for (uint a = 0; a < G->size_alpha; a++) {
                uint qa = res->edges[q][a];
                uint ra = res->edges[r][a];
                if (qa != UINT_MAX && ra != UINT_MAX) {
                    inter->edges[q * P->cl_size[j] + r][a] = qa * P->cl_size[j] + ra;
                }
                else {
                    inter->edges[q * P->cl_size[j] + r][a] = size;
                }
            }
        }
    }
    for (uint a = 0; a < G->size_alpha; a++) {
        inter->edges[size][a] = size;
    }
    delete_dgraph(res);
    *rinter = inter;
    *rscci = dtarjan(inter, NULL, false);
}


bool is_daplus_dfa(dfa* A, int*) {

    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    uint size = A->trans->size_graph * A->trans->size_graph;
    dgraph* inter;
    parti* scci;
    dgraph_make_product_sccs(A->trans, scca, &inter, &scci);

    // The double product of the DFA with itself and its SCCs (used to check the self-loops)


    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {
        // We skip the last class, which is the one containing the artificial sink
        if (i == scci->numcl[size]) {
            continue;
        }

        uint t = scci->cl_elems[i][0];
        uint t1 = t / A->trans->size_graph;
        uint t2 = t % A->trans->size_graph;
        if (t1 == t2 || scca->numcl[t1] > scca->numcl[t2]) {
            continue;
        }

        // We compute the local double product graph for this SCC
        dgraph* quad;
        parti* sccq;
        dgraph_make_product_sccs_local(inter, scci, i, &quad, &sccq);

        // We pick a pair of distinct states in the SCC. 
        for (uint j = 0; j < scci->cl_size[i]; j++) {
            for (uint k = j + 1; k < scci->cl_size[i]; k++) {
                // We first check if there is a common self-loop using the double product graph
                uint pq = scci->cl_elems[i][j];
                uint pr = scci->cl_elems[i][k];
                uint qstate = j * scci->cl_size[i] + k;
                bool has_self_loop = false;
                if (sccq->cl_size[sccq->numcl[qstate]] > 1) {
                    has_self_loop = true;
                }
                else {
                    for (uint a = 0; a < quad->size_alpha; a++) {
                        if (quad->edges[qstate][a] == qstate) {
                            has_self_loop = true;
                            break;
                        }
                    }
                }
                if (!has_self_loop) {
                    continue;
                }

                // We now know that there is a self-loop on the pair (pq, pr).

                uint q1 = pq / A->trans->size_graph;
                uint q2 = pq % A->trans->size_graph;
                uint r1 = pr / A->trans->size_graph;
                uint r2 = pr % A->trans->size_graph;

                if (dgraph_exists_intersec_path(inter, A->trans, pq, r1, pr, r2) || dgraph_exists_intersec_path(inter, A->trans, pr, q1, pq, q2)) {
                    delete_dgraph(inter);
                    delete_parti(scca);
                    delete_parti(scci);
                    delete_dgraph(quad);
                    delete_parti(sccq);
                    return false;
                }
            }
        }
        delete_dgraph(quad);
        delete_parti(sccq);

    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}




static void dfagp_fold(dgraph* g, parti* sccs, uint cl, dfagp_mode mode, parti** fold, dgraph** g_fold) {

    dgraph* sub = dgraph_extract(g, sccs, cl);
    switch (mode) {
    case DFAGP_MOD:
        *fold = dgraph_stal_fold(sub, false);
        break;
    case DFAGP_AMT:
        *fold = compute_amt_fold(sub);
        break;
    case DFAGP_GR:
        *fold = dgraph_stal_fold(sub, true);
        break;
    default:
        delete_dgraph(sub);
        fprintf(stderr, "dagp_fold: Unknown mode %d.\n", mode);
        exit(EXIT_FAILURE);
    }


    // dgraph* sub = dgraph_extract(inter, scci, i);
    // //printf("SCC extracted\n");
    // parti* fold;
    // dgraph* g_fold;
    // damod_fold(sub, &fold, &g_fold);
    // //printf("Folded graph created with %u classes.\n", fold->size_par);
    // delete_dgraph(sub);
    //*fold = dgraph_stal_fold(sub, false);
    if (g_fold) {
        *g_fold = create_dgraph_noedges((*fold)->size_par, sub->size_alpha);
        for (uint i = 0; i < (*fold)->size_par; i++) {
            for (uint a = 0; a < sub->size_alpha; a++) {
                (*g_fold)->edges[i][a] = UINT_MAX;
            }
        }

        for (uint i = 0; i < sub->size_graph;i++) {
            for (uint a = 0; a < sub->size_alpha; a++) {
                if (sub->edges[i][a] != UINT_MAX) {
                    (*g_fold)->edges[(*fold)->numcl[i]][a] = (*fold)->numcl[sub->edges[i][a]];
                }
            }
        }
    }
    delete_dgraph(sub);
}


bool is_dagp_dfa(dfa* A, dfagp_mode mode, int*) {


    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    uint size = A->trans->size_graph * A->trans->size_graph;
    dgraph* inter;
    parti* scci;
    dgraph_make_product_sccs(A->trans, scca, &inter, &scci);

    for (uint i = 0; i < scci->size_par; i++) {
        if (i == scci->numcl[size]) {
            continue;
        }
        uint qr = scci->cl_elems[i][0];
        uint q = qr / A->trans->size_graph;
        uint r = qr % A->trans->size_graph;
        if (q == r || scca->numcl[q] > scca->numcl[r]) {
            continue;
        }
        parti* fold;
        dgraph* g_fold;
        dfagp_fold(inter, scci, i, mode, &fold, &g_fold);

        if (dgraph_exists_intersec_path(g_fold, A->trans, 0, q, 0, r)) {
            delete_dgraph(g_fold);
            delete_parti(fold);
            delete_dgraph(inter);
            delete_parti(scca);
            delete_parti(scci);
            return false;
        }
        delete_dgraph(g_fold);
        delete_parti(fold);
    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;
}


bool is_daplusgp_dfa(dfa* A, dfagp_mode mode, int*) {
    // SCCs of the DFA
    parti* scca = dtarjan(A->trans, NULL, false);

    // Compute the product graph of the DFA with itself and its sccs
    uint size = A->trans->size_graph * A->trans->size_graph;
    dgraph* inter;
    parti* scci;
    dgraph_make_product_sccs(A->trans, scca, &inter, &scci);





    // We now check the pattern

    // For each SCC in the product graph
    for (uint i = 0; i < scci->size_par; i++) {
        // We skip the last class, which is the one containing the artificial sink
        if (i == scci->numcl[size]) {
            continue;
        }

        uint t = scci->cl_elems[i][0];
        uint t1 = t / A->trans->size_graph;
        uint t2 = t % A->trans->size_graph;
        if (t1 == t2 || scca->numcl[t1] > scca->numcl[t2]) {
            continue;
        }
        // We compute the local double product graph for this SCC
        dgraph* quad;
        parti* sccq;
        dgraph_make_product_sccs_local(inter, scci, i, &quad, &sccq);

        // Folding according to the mode
        parti* fold = NULL;
        dfagp_fold(inter, scci, i, mode, &fold, NULL);

        // We pick a pair of distinct states in the SCC. 
        for (uint j = 0; j < scci->cl_size[i]; j++) {
            for (uint k = j + 1; k < scci->cl_size[i]; k++) {

                // We skip the pair if they are not in the same class in the fold
                if (fold->numcl[j] != fold->numcl[k]) {
                    continue;
                }

                // We first check if there is a common self-loop using the double product graph
                uint pq = scci->cl_elems[i][j];
                uint pr = scci->cl_elems[i][k];
                uint qstate = j * scci->cl_size[i] + k;
                bool has_self_loop = false;
                if (sccq->cl_size[sccq->numcl[qstate]] > 1) {
                    has_self_loop = true;
                }
                else {
                    for (uint a = 0; a < quad->size_alpha; a++) {
                        if (quad->edges[qstate][a] == qstate) {
                            has_self_loop = true;
                            break;
                        }
                    }
                }
                if (!has_self_loop) {
                    continue;
                }

                // We now know that there is a self-loop on the pair (pq, pr).

                uint q1 = pq / A->trans->size_graph;
                uint q2 = pq % A->trans->size_graph;
                uint r1 = pr / A->trans->size_graph;
                uint r2 = pr % A->trans->size_graph;

                if (dgraph_exists_intersec_path(inter, A->trans, pq, r1, pr, r2) || dgraph_exists_intersec_path(inter, A->trans, pr, q1, pq, q2)) {
                    delete_dgraph(inter);
                    delete_parti(scca);
                    delete_parti(scci);
                    delete_dgraph(quad);
                    delete_parti(sccq);
                    delete_parti(fold);
                    return false;
                }
            }
        }
        delete_parti(fold);
        delete_dgraph(quad);
        delete_parti(sccq);

    }
    delete_dgraph(inter);
    delete_parti(scca);
    delete_parti(scci);
    return true;

}

bool is_cyclet_dfa(dfa* A, int*, FILE* out) {
    parti* scca = dtarjan(A->trans, NULL, false);
    if (scca->size_par == A->trans->size_graph) {
        delete_parti(scca);
        return true;
    }

    if (out) {
        for (uint i = 0; i < scca->size_par; i++) {
            if (scca->cl_size[i] > 1) {
                fprintf(out, "#### The states ");
                dfa_print_state(A, scca->cl_elems[i][0], out);
                fprintf(out, " and ");
                dfa_print_state(A, scca->cl_elems[i][1], out);
                fprintf(out, " are in the same strongly connected component\n");
                delete_parti(scca);
                return false;

            }
        }

    }
    delete_parti(scca);
    return false;
}


bool is_cycletplus_dfa(dfa* A, int*) {

    parti* scca = dtarjan(A->trans, NULL, false);
    dgraph* inter;
    parti* scci;
    dgraph_make_product_sccs(A->trans, scca, &inter, &scci);


    for (uint i = 0; i < scca->size_par; i++) {
        if (scca->cl_size[i] <= 1) {
            continue;
        }
        for (uint j = 0; j < scca->cl_size[i]; j++) {
            for (uint k = j + 1; k < scca->cl_size[i]; k++) {
                uint q = scca->cl_elems[i][j];
                uint r = scca->cl_elems[i][k];
                uint qr = q * A->trans->size_graph + r;
                if (scci->cl_size[scci->numcl[qr]] > 1) {
                    // If the class of (q,r) contains more than one element, we have a cycle labeled by a nonempty word.
                    // This contradicts the pattern of the DFA.
                    delete_dgraph(inter);
                    delete_parti(scca);
                    delete_parti(scci);
                    return false;
                }
                for (uint a = 0; a < A->trans->size_alpha; a++) {
                    if (qr == inter->edges[qr][a]) {
                        // If there is an a-labeled loop on (q,r), we have a cycle labeled by a nonempty word.
                        delete_dgraph(inter);
                        delete_parti(scca);
                        delete_parti(scci);
                        return false;
                    }
                }
            }
        }
    }
    delete_dgraph(inter);
    delete_parti(scci);
    delete_parti(scca);
    return true;
}


bool is_cycletgp_dfa(dfa* A, dfagp_mode mode, int*) {
    parti* scca = dtarjan(A->trans, NULL, false);

    // For each class in the partition, we check if it can be folded.
    for (uint i = 0; i < scca->size_par; i++) {
        if (scca->cl_size[i] <= 1) {
            continue;
        }
        parti* fold;
        dfagp_fold(A->trans, scca, i, mode, &fold, NULL);
        if (fold->size_par < fold->size_set) {
            // If two states were folded, the DFA is not cycle-trivial for G
            delete_parti(fold);
            delete_parti(scca);
            return false;
        }
        delete_parti(fold);
    }
    delete_parti(scca);
    return true;
}

bool is_cycletplusgp_dfa(dfa* A, dfagp_mode mode, int*) {
    parti* scca = dtarjan(A->trans, NULL, false);
    dgraph* inter;
    parti* scci;
    dgraph_make_product_sccs(A->trans, scca, &inter, &scci);


    for (uint i = 0; i < scca->size_par; i++) {
        if (scca->cl_size[i] <= 1) {
            continue;
        }
        parti* fold;
        dfagp_fold(A->trans, scca, i, mode, &fold, NULL);
        for (uint j = 0; j < scca->cl_size[i]; j++) {
            for (uint k = j + 1; k < scca->cl_size[i]; k++) {
                if (fold->numcl[j] != fold->numcl[k]) {
                    // If the two elements are not in the same fold class, we can continue.
                    continue;
                }
                uint q = scca->cl_elems[i][j];
                uint r = scca->cl_elems[i][k];
                uint qr = q * A->trans->size_graph + r;
                if (scci->cl_size[scci->numcl[qr]] > 1) {
                    // If the class of (q,r) contains more than one element, we have a cycle labeled by a nonempty word.
                    // This contradicts the pattern of the DFA.
                    delete_dgraph(inter);
                    delete_parti(scca);
                    delete_parti(scci);
                    delete_parti(fold);
                    return false;
                }
                for (uint a = 0; a < A->trans->size_alpha; a++) {
                    if (qr == inter->edges[qr][a]) {
                        // If there is an a-labeled loop on (q,r), we have a cycle labeled by a nonempty word.
                        delete_dgraph(inter);
                        delete_parti(scca);
                        delete_parti(scci);
                        delete_parti(fold);
                        return false;
                    }
                }
            }
        }
        delete_parti(fold);
    }
    delete_dgraph(inter);
    delete_parti(scci);
    delete_parti(scca);
    return true;
}


bool is_cycletbp_dfa(dfa* A, int*) {
    parti* scca = dtarjan(A->trans, NULL, false);
    dgraph* inter;
    parti* scci;
    dgraph_make_product_sccs(A->trans, scca, &inter, &scci);



    bool alpha[A->trans->size_alpha];
    for (uint i = 0; i < scca->size_par; i++) {
        if (scca->cl_size[i] <= 1) {
            continue;
        }
        for (uint j = 0; j < scca->cl_size[i]; j++) {
            for (uint k = j + 1; k < scca->cl_size[i]; k++) {
                uint q = scca->cl_elems[i][j];
                uint r = scca->cl_elems[i][k];
                uint qr = q * A->trans->size_graph + r;

                compute_alph_scc_dgraph(inter, scci, scci->numcl[qr], alpha);
                if (dfa_exists_path(A, q, r, alpha) && dfa_exists_path(A, r, q, alpha)) {
                    delete_dgraph(inter);
                    delete_parti(scca);
                    delete_parti(scci);
                    return false;
                }
            }
        }
    }
    delete_dgraph(inter);
    delete_parti(scci);
    delete_parti(scca);
    return true;
}

bool is_cycletbpgp_dfa(dfa* A, dfagp_mode mode, int*) {
    parti* scca = dtarjan(A->trans, NULL, false);
    dgraph* inter;
    parti* scci;
    dgraph_make_product_sccs(A->trans, scca, &inter, &scci);

    // printf("Size of the inter graph: %u\n", inter->size_graph);


    for (uint i = 0; i < scca->size_par; i++) {
        // printf("Class %u\n", i);
        if (scca->cl_size[i] <= 1) {
            continue;
        }
        for (uint j = 0; j < scca->cl_size[i]; j++) {
            for (uint k = j + 1; k < scca->cl_size[i]; k++) {
                uint q = scca->cl_elems[i][j];
                uint r = scca->cl_elems[i][k];
                uint qr = q * A->trans->size_graph + r;

                parti* fold;
                dgraph* g_fold;
                // printf("Processing class %u (%u,%u)\n", i, q, r);
                dfagp_fold(inter, scci, scci->numcl[qr], mode, &fold, &g_fold);
                uint l = ((uint*)bsearch(&qr, scci->cl_elems[scci->numcl[qr]], scci->cl_size[scci->numcl[qr]], sizeof(uint), &compare_uint)) - scci->cl_elems[scci->numcl[qr]];
                uint qrc = fold->numcl[l];
                // printf("the fold has %u classes, qrc: %d\n", fold->size_par, qrc);
                if (dgraph_exists_intersec_path(g_fold, A->trans, qrc, q, qrc, r) && dgraph_exists_intersec_path(g_fold, A->trans, qrc, r, qrc, q)) {
                    // printf("Found a cycle in the folded graph.\n");
                    delete_dgraph(g_fold);
                    delete_parti(fold);
                    delete_dgraph(inter);
                    delete_parti(scca);
                    delete_parti(scci);
                    return false;
                }
                // printf("No cycle found in the folded graph.\n");
                delete_dgraph(g_fold);
                delete_parti(fold);
            }
        }
    }
    delete_dgraph(inter);
    delete_parti(scci);
    delete_parti(scca);
    return true;
}
