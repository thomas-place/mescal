#include "nfa_props.h"
#include "monoid_display.h"

// Pour l'automate minimal
bool is_trivial_dfa(nfa* A, FILE* out) {
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
bool is_letterind_dfa(nfa* A, FILE* out) {
    for (uint s = 0; s < A->trans->size_graph; s++) {
        for (uint a = 1; a < A->trans->size_alpha; a++) {
            if (lefread_dequeue(A->trans->edges[s][0], 0) != lefread_dequeue(A->trans->edges[s][a], 0)) {
                if (out != NULL) {
                    fprintf(out, "#### There are letters which have distinct actions on the states.\n");
                    fprintf(out, "#### For instance, we have the transitions (%d,%c,%d) and (%d,%c,%d).\n", s, 0 + 'a', lefread_dequeue(A->trans->edges[s][0], 0), s, a + 'a',
                        lefread_dequeue(A->trans->edges[s][a], 0));
                    return false;
                }
            }
        }
    }

    if (out != NULL) {
        fprintf(out, "#### All letters have the same action on the states.\n");
    }
    return true;
}

// Teste si un DFA est commutatif
bool dfa_is_comm(nfa* A, FILE* out) {
    if (!nfa_is_det(A)) {
        if (out) {
            fprintf(out, "#### Error: the commutativity test is only meaningful for DFAs.\n");
        }
        return false;
    }
    if (out) {
        fprintf(out, "#### Checking if the DFA is commutative.\n");
    }
    for (uint a = 0; a < A->trans->size_alpha; a++) {
        for (uint b = a + 1; b < A->trans->size_alpha; b++) {
            for (uint q = 0; q < A->trans->size_graph; q++) {
                uint r = lefread_dequeue(A->trans->edges[lefread_dequeue(A->trans->edges[q][b], 0)][a], 0);
                uint s = lefread_dequeue(A->trans->edges[lefread_dequeue(A->trans->edges[q][a], 0)][b], 0);
                if (r != s) {
                    if (out) {
                        fprintf(out, "#### The DFA is NOT commutative.\n");
                        fprintf(out, "#### For instance, (");
                        nfa_print_state(A, q, out);
                        fprintf(out, ",%c%c) ⟶ ", a + 'a', b + 'a');
                        nfa_print_state(A, s, out);
                        fprintf(out, " and (");
                        nfa_print_state(A, q, out);
                        fprintf(out, ",%c%c) ⟶ ", b + 'a', a + 'a');
                        nfa_print_state(A, r, out);
                        fprintf(out, ".\n");
                    }
                    return false;
                }
            }
        }
    }
    if (out) {
        fprintf(out, "#### The DFA is commutative.\n");
    }
    return true;
}

bool is_permutation_dfa(nfa* A, FILE* out) {
    if (!nfa_is_det(A)) {
        fprintf(stderr, "#### This automaton is not deterministic and therefore not a permutation DFA.\n");
        return false;
    }

    if (!nfa_is_comp(A)) {
        fprintf(stderr, "#### This automaton is not complete and therefore not a permutation DFA.\n");
        return false;
    }

    for (uint s = 0; s < A->trans->size_graph; s++) {
        for (uint t = 0; t < A->trans->size_graph; t++) {
            if (s != t) {
                for (uint b = 0; b < A->trans->size_alpha; b++) {
                    if (lefread_dequeue(A->trans->edges[s][b], 0) == lefread_dequeue(A->trans->edges[t][b], 0)) {
                        if (out != NULL) {
                            fprintf(out, "#### This is NOT a permutation automaton.\n");

                            fprintf(out, "#### For instance, it contains the transitions (%d,", s);
                            nfa_fprint_letter_utf8(A, b, out);
                            fprintf(out, ",%d) and (%d,", lefread_dequeue(A->trans->edges[s][b], 0), t);
                            nfa_fprint_letter_utf8(A, b, out);
                            fprintf(out, ",%d).\n", lefread_dequeue(A->trans->edges[t][b], 0));
                        }
                        return false;
                    }
                }
            }
        }
    }

    if (out != NULL) {
        fprintf(out, "#### This is a permutation automaton.\n");
    }
    return true;
}

// Retourne l'état atteint à partir de s en lisant le mot w dans le DFA A
// (le déterminisme n'est pas vérifié)
static uint dfa_function(nfa* A, uint s, dequeue* w) {
    uint e = s;
    for (uint i = 0; i < size_dequeue(w); i++) {
        e = lefread_dequeue(A->trans->edges[e][lefread_dequeue(w, i)], 0);
    }
    return e;
}

// Génération d'un nfa à partir d'un lgraph et d'un unique état initial
// Seuls les états accessibles sont conservés. Si l'automate n'est pas
// complet, celui-ci est complété en ajoutant un état supplémentaire.
static nfa* lgraph_to_nfa(lgraph* G, uint ini, letter* anames) {
    // La pile pour le parcours en profondeur
    dequeue* thestack = create_dequeue();

    // Un tableau pour mémoriser les états recontrés
    bool visited[G->size_graph];
    for (uint q = 0; q < G->size_graph; q++) {
        visited[q] = false;
    }

    // Un Booléen pour mémoriser si l'automate est complet
    bool is_complete = true;

    // Un compteur qui compte le nombre d'états rencontrés
    uint num = 0;

    // Empilement de l'état initial
    rigins_dequeue(ini, thestack);
    visited[ini] = true;
    num++;

    while (!isempty_dequeue(thestack)) {
        uint state = rigpull_dequeue(thestack);
        for (uint a = 0; a < G->size_alpha; a++) {
            if (isempty_dequeue(G->edges[state][a])) {
                is_complete = false;
            }
            else {
                for (uint i = 0; i < size_dequeue(G->edges[state][a]); i++) {
                    if (!visited[lefread_dequeue(G->edges[state][a], i)]) {
                        rigins_dequeue(lefread_dequeue(G->edges[state][a], i), thestack);
                        num++;
                        visited[lefread_dequeue(G->edges[state][a], i)] = true;
                    }
                }
            }
        }
    }

    // Calcul de la taille de l'automate
    uint size_auto = num;
    if (!is_complete) { // Si l'automate n'était pas complet, on rajoute un état
        size_auto++;
    }

    // Création de l'automate
    nfa* A = nfa_init();
    A->trans = create_lgraph_noedges(size_auto, G->size_alpha);
    A->alphabet = anames;

    // Calcul de l'association entre les nouveaux états et les anciens
    MALLOC(A->ancestors, size_auto);

    uint GTOA[G->size_graph];
    uint t = 0;
    for (uint q = 0; q < G->size_graph; q++) {
        if (visited[q]) {
            GTOA[q] = t;
            A->ancestors[t] = q;
            if (q == ini) {
                rigins_dequeue(t, A->initials);
            }
            t++;
        }
    }
    // Affectation des transtions
    for (uint s = 0; s < num; s++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            if (isempty_dequeue(G->edges[A->ancestors[s]][a])) {
                rigins_dequeue(num, A->trans->edges[s][a]);
            }
            else {
                for (uint i = 0; i < size_dequeue(G->edges[A->ancestors[s]][a]); i++) {
                    rigins_dequeue(GTOA[lefread_dequeue(G->edges[A->ancestors[s]][a], i)], A->trans->edges[s][a]);
                }
            }
        }
    }
    if (!is_complete) {
        for (uint a = 0; a < G->size_alpha; a++) {
            rigins_dequeue(num, A->trans->edges[num][a]);
        }
    }

    return A;
}

bool is_counterfree_dfa(nfa* A, int* error, FILE* out) {
    if (!nfa_is_det(A)) {
        fprintf(stderr, "#### This automaton is not deterministic.\n");
        return false;
    }

    /* We consider each SCC in the automaton, and check that none of them contains a counter. */

    // First compute the SCCs of the DFA.
    parti* PSCCS = ltarjan(A->trans);

    // Compute the transition graph obtained by keeping only those internal to SCCs.
    lgraph* G = nfa_get_lab_parti(A, PSCCS);

    // Consider each SCC independently.
    for (uint c = 0; c < PSCCS->size_par; c++) {
        // Compute the automaton obtained by only keeping this SCC
        nfa* SCC = lgraph_to_nfa(G, lefread_dequeue(PSCCS->cl[c], 0), nfa_duplicate_alpha(A));

        // We compute the associated morphism.

        *error = 0;

        morphism* M = dfa_to_morphism(SCC, NULL, error);
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
                for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
                    uint cl = GREL->HCL->numcl[lefread_dequeue(M->idem_list, i)];
                    if (size_dequeue(GREL->HCL->cl[cl]) > 1) {
                        // On prend un élément de ce groupe qui n'est pas le neutre
                        uint s = lefread_dequeue(GREL->HCL->cl[cl], 0);
                        if (M->idem_array[s]) {
                            s = lefread_dequeue(GREL->HCL->cl[cl], 1);
                        }

                        // Cet élément satisfiait s^omega != s^omega+1. Donc son nom étiquette un compteur

                        fprintf(out, "#### Found a counter in this automaton.\n");
                        fprintf(out, "#### Word labeling the counter: ");
                        mor_fprint_name_utf8(M, s, out);
                        printf(".\n");
                        // On va maintenant trouver le compteur. On prend d'abord le graph obtenu à partir du DFA en ne lisant que word
                        fprintf(out, "#### The counter itself: ");

                        graph* GW = create_graph_noedges(SCC->trans->size_graph);
                        for (uint q = 0; q < GW->size; q++) {
                            lefins_dequeue(dfa_function(SCC, q, M->names[s]), GW->edges[q]);
                        }
                        // graph_printing_test(GW, stdout);

                        // Le compteur recherché est une SCC non triviale dans ce graphe (qui sera un cycle)
                        parti* CYCLES = tarjan(GW);
                        for (uint j = 0; j < CYCLES->size_par; j++) {
                            if (size_dequeue(CYCLES->cl[j]) > 1) {
                                uint q = lefread_dequeue(CYCLES->cl[j], 0);
                                nfa_print_state(A, SCC->ancestors[q], out);
                                uint r = lefread_dequeue(GW->edges[q], 0);
                                while (r != q) {
                                    //  printf(" -> %d", ((uint*)SCC->names)[r]);
                                    fprintf(out, " -> ");
                                    nfa_print_state(A, SCC->ancestors[r], out);
                                    r = lefread_dequeue(GW->edges[r], 0);
                                }
                                fprintf(out, " -> ");
                                nfa_print_state(A, SCC->ancestors[q], out);
                                fprintf(out, "\n");
                                break;
                                // printf(" -> %d.\n", ((uint*)SCC->names)[q]);
                            }
                        }
                        delete_parti(CYCLES);
                        delete_graph(GW);
                        delete_nfa(SCC);

                        delete_morphism(M);
                        delete_green(GREL);
                        return false;
                    }
                }
            }
            delete_nfa(SCC);
            delete_morphism(M);
            delete_green(GREL);
            return false;
        }
    }
    if (out != NULL) {
        fprintf(stdout, "#### This automaton is counter-free.\n");
    }
    return true;
}
