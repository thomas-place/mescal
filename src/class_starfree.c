/******************************************/
/* Fonctions sur les langages sans-étoile */
/******************************************/

#include "class_starfree.h"

// Pour un graphe de cayley
bool is_aperiodic_mono(p_cayley M, p_green GREL, FILE* out)
{
    if (out) {
        fprintf(stdout, "#### Checking if the monoid is aperiodic.\n");
    }
    // Si il y a autant d'éléments que de H-classes, le monoide est H-trivial
    if (GREL->HCL->size_set == GREL->HCL->size_par)
    {
        if (out != NULL)
        {
            fprintf(out, "#### This is an aperiodic monoid (it is H-trivial).\n");
        }
        return true;
    }

    // Sinon (si on doit afficher), on va chercher un contre-exemple
    if (out != NULL)
    {
        // On regarde toutes les H-classes qui sont des groupes dans le monoide
        for (uint i = 0; i < size_vertices(GREL->group_list); i++)
        {
            // On prend la première qui est non-triviale
            if (size_vertices(GREL->HCL->cl[lefread_vertices(GREL->group_list, i)]) >= 2)
            {
                uint s = lefread_vertices(GREL->HCL->cl[lefread_vertices(GREL->group_list, i)], 0);
                uint t = lefread_vertices(GREL->HCL->cl[lefread_vertices(GREL->group_list, i)], 1);
                fprintf(out, "#### This is NOT an aperiodic monoid (it is not H-trivial).\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, s, out);
                fprintf(out, " H ");
                cayley_print_name(M, t, out);
                fprintf(out, ".\n");

                return false;
            }
        }
    }
    return false;
}

// version sous-monoide
bool is_aperiodic_submono(p_green_sub S, char* name, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s is aperiodic. Displaying the %s.\n", name, name);
        print_full_green_sub(S, false, stdout);
    }

    p_cayley M = S->original;
    p_green G = S->originalrels;

    // Si le sous-monoide est le monoide entier
    if (S->mono_in_sub == NULL) {
        return is_aperiodic_mono(M, G, out);
    }

    // Si il y a autant d'éléments que de H-classes, le monoide est H-trivial
    if (S->rels->HCL->size_set == S->rels->HCL->size_par)
    {
        if (out != NULL)
        {
            fprintf(out, "#### The monoid is aperiodic (it is H-trivial).\n");
        }
        return true;
    }

    // Sinon (si on doit afficher), on va chercher un contre-exemple
    if (out != NULL)
    {
        // On regarde toutes les H-classes qui sont des groupes dans le monoide
        for (uint i = 0; i < size_vertices(S->rels->group_list); i++)
        {
            // On prend la première qui est non-triviale
            if (size_vertices(S->rels->HCL->cl[lefread_vertices(S->rels->group_list, i)]) >= 2)
            {
                uint s = lefread_vertices(S->rels->HCL->cl[lefread_vertices(S->rels->group_list, i)], 0);
                uint t = lefread_vertices(S->rels->HCL->cl[lefread_vertices(S->rels->group_list, i)], 1);
                fprintf(out, "#### The monoid is NOT aperiodic (it is not H-trivial).\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, S->sub_to_mono[s], out);
                fprintf(out, " H ");
                cayley_print_name(M, S->sub_to_mono[t], out);
                fprintf(out, ".\n");

                return false;
            }
        }
    }
    return false;
}

bool is_counterfree_dfa(p_nfa A, FILE* out)
{
    /* On cnsidère chaque SCCS de l'automate: on vérifie qu'aucune d'entre elles ne contient de compteur. */

    // On commence par calculer les SCCS de l'automate
    p_parti PSCCS = ltarjan(A->trans);

    // On calcule le graphe des transitions obtenu en ne gardant que celles internes aux SCCS
    p_lgraph G = nfa_get_lab_parti(A, PSCCS);

    // On regarde chaque SCC indépendemment
    for (uint c = 0; c < PSCCS->size_par; c++)
    {
        // On calcule le NFA obtenu en ne gardant que la SCC
        p_nfa SCC = lgraph_to_nfa(G, lefread_vertices(PSCCS->cl[c], 0));

        // On calcule le morphisme associé
        p_cayley M = dfa_to_right_cayley(SCC);

        // On calcule ses relations de Green
        p_green GREL = cayley_green_compute(M);

        // Si il existe une H-classe non-triviale, on sait que la SCC contient un compteur
        if (GREL->HCL->size_set != GREL->HCL->size_par)
        {
            // Si on doit afficher un exemple de compteur
            if (out != NULL)
            {

                // On cherche un groupe non-trivial dans le monoide
                for (uint i = 0; i < size_vertices(GREL->group_list); i++)
                {
                    if (size_vertices(GREL->HCL->cl[lefread_vertices(GREL->group_list, i)]) > 1)
                    {
                        // On prend un élément de ce groupe qui n'est pas le neutre
                        uint s = lefread_vertices(GREL->HCL->cl[lefread_vertices(GREL->group_list, i)], 0);
                        if (M->idem_array[s])
                        {
                            s = lefread_vertices(GREL->HCL->cl[lefread_vertices(GREL->group_list, i)], 1);
                        }

                        // Cet élément satisfiait s^omega != s^omega+1. Donc son nom étiquette un compteur
                        p_vertices word = M->names[s];
                        fprintf(out, "#### Found a counter in this automaton.\n");
                        fprintf(out, "#### Word labeling the counter: ");

                        for (uint j = 0; j < size_vertices(word); j++)
                        {
                            printf("%c", lefread_vertices(word, j) + 'a');
                        }
                        printf(".\n");
                        // On va maintenant trouver le compteur. On prend d'abord le graph obtenu à partir du DFA en ne lisant que word
                        fprintf(out, "#### The counter itself: ");

                        p_graph GW = create_graph_noedges(SCC->trans->size_graph);
                        for (uint q = 0; q < GW->size; q++)
                        {
                            lefins_vertices(dfa_function(SCC, q, word), GW->edges[q]);
                        }
                        // graph_printing_test(GW, stdout);

                        // Le compteur recherché est une SCC non triviale dans ce graphe (qui sera un cycle)
                        p_parti CYCLES = tarjan(GW);
                        for (uint j = 0; j < CYCLES->size_par; j++)
                        {
                            if (size_vertices(CYCLES->cl[j]) > 1)
                            {
                                uint q = lefread_vertices(CYCLES->cl[j], 0);
                                printf("%d", ((uint*)SCC->names)[q]);
                                uint r = lefread_vertices(GW->edges[q], 0);
                                while (r != q)
                                {
                                    printf(" -> %d", ((uint*)SCC->names)[r]);
                                    r = lefread_vertices(GW->edges[r], 0);
                                }
                                printf(" -> %d.\n", ((uint*)SCC->names)[q]);
                            }
                        }
                        delete_parti(CYCLES);
                        delete_graph(GW);
                        delete_nfa(SCC);
                        free_cayley(M);
                        delete_green(GREL);
                        return false;
                    }
                }
            }
            delete_nfa(SCC);
            free_cayley(M);
            delete_green(GREL);
            return false;
        }
    }
    if (out != NULL)
    {
        fprintf(stdout, "#### This automaton is counter-free.\n");
    }
    return true;
}
