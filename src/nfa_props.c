#include "nfa_props.h"
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
                    dfa_fprint_letter_utf8(A, 0, out);
                    fprintf(out, ",");
                    dfa_print_state(A, A->trans->edges[s][0], out);
                    fprintf(out, ") and (");
                    dfa_print_state(A, s, out);
                    fprintf(out, ",");
                    dfa_fprint_letter_utf8(A, a, out);
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
                        dfa_fprint_letter_utf8(A, a, out);
                        fprintf(out, ",");
                        dfa_print_state(A, qa, out);
                        fprintf(out, ") and (");
                        dfa_print_state(A, qa, out);
                        fprintf(out, ",");
                        dfa_fprint_letter_utf8(A, b, out);
                        fprintf(out, ",");
                        dfa_print_state(A, qab, out);
                        fprintf(out, ")\n");

                        fprintf(out, "     (");
                        dfa_print_state(A, q, out);
                        fprintf(out, ",");
                        dfa_fprint_letter_utf8(A, b, out);
                        fprintf(out, ",");
                        dfa_print_state(A, qb, out);
                        fprintf(out, ") and (");
                        dfa_print_state(A, qb, out);
                        fprintf(out, ",");
                        dfa_fprint_letter_utf8(A, a, out);
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
                    dfa_fprint_letter_utf8(A, a, out);
                    fprintf(out, ",");
                    dfa_print_state(A, r, out);
                    fprintf(out, ") and (");
                    dfa_print_state(A, q, out);
                    fprintf(out, ",");
                    dfa_fprint_letter_utf8(A, a, out);
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

    dfa* D = dfa_init(sccs->cl_size[p] + 1, A->trans->size_alpha, 0, A->alphabet);

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





bool is_da_dfa(dfa* A, int*) {

    parti* scca = dtarjan(A->trans, NULL, false);

    uint size = A->trans->size_graph * A->trans->size_graph;

    dgraph* inter = create_dgraph_noedges(size + 1, A->trans->size_alpha);


    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint r = 0; r < A->trans->size_graph; r++) {
            for (uint a = 0; a < A->trans->size_alpha; a++) {
                uint qa = A->trans->edges[q][a];
                uint ra = A->trans->edges[r][a];
                if (scca->numcl[q] == scca->numcl[qa] && scca->numcl[r] == scca->numcl[ra]) {
                    inter->edges[q * A->trans->size_graph + r][a] = qa * A->trans->size_graph + ra;
                }
                else {
                    inter->edges[q * A->trans->size_graph + r][a] = size;
                }
            }
        }
    }
    for (uint a = 0; a < A->trans->size_alpha; a++) {
        inter->edges[size][a] = size;
    }


    parti* scci = dtarjan(inter, NULL, false);


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




bool is_cycletrivial_dfa(dfa* A, int*, FILE* out) {
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

