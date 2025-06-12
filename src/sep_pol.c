#include "sep_pol.h"



// Commence par éliminer les éventuelles transitions epsilon.
// Puis, double toute transition existante d'une transition epsilon,
// dans un nouveau NFA.
static nfa* nfa_add_eps(nfa* A)
{
    if (!A) {
        return NULL;
    }
    nfa* B = nfa_copy(A);
    // Si il y a des transitions inverses, on les supprime
    if (B->trans_i != NULL)
    {
        nfa_remove_inv(B);
    }
    nfa_elimeps_mod(B);

    if (!B->trans_e) {
        B->trans_e = create_graph_noedges(B->trans->size_graph);
    }


    for (uint q = 0; q < B->trans_e->size; q++)
    {
        dequeue* array[B->trans->size_alpha + 1];
        array[0] = A->trans_e->edges[q];
        for (uint a = 0; a < B->trans->size_alpha; a++)
        {
            array[a + 1] = B->trans->edges[q][a];
        }

        merge_array_sorted_dequeue(B->trans_e->edges[q], array, B->trans->size_alpha + 1);
    }

    return B;
}


bool decid_polst_sep(nfa* A, nfa* B, bool details, FILE* out)
{
    // Gestion des cas triviaux
    if (isempty_dequeue(A->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(B->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (A == B)
    {
        if (out && details)
        {
            fprintf(out,
                "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }

    // Phase 1
    nfa* BEPS = nfa_add_eps(B);
    nfa_elimeps_mod(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(
            out,
            "#### Phase 1: Building ε-transitions in the second input language.\n");
        fprintf(
            out,
            "####          Each original transition induces an ε-transition.\n");
        fprintf(out, "####          These ε-transitions are then eliminated to "
            "simplify the computations.\n");
        view_nfa(BEPS);
    }

    // Phase 2
    nfa* INTERSECT = nfa_intersect(A, BEPS, true);
    nfa_delete(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 2: Intersecting the resulting NFA with that of "
            "the first input language.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_dequeue(INTERSECT->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        nfa_delete(INTERSECT);
        return true;
    }
    else
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        nfa_delete(INTERSECT);
        return false;
    }
}

// Pol(MOD)-separation of A from B
bool decid_polmod_sep(nfa* A, nfa* B, bool details, FILE* out)
{
    // Gestion des cas triviaux
    if (isempty_dequeue(A->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(B->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (A == B)
    {
        if (out && details)
        {
            fprintf(out,
                "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }

    // Phase 1
    nfa* BPROJ = nfa_proj_unary(B);
    parti* SCCS = nfa_inv_ext(BPROJ);
    parti* FOLD = nfa_stal_fold(BPROJ, SCCS);
    delete_parti(SCCS);
    nfa* BEPS = nfa_merge_states(B, FOLD);
    nfa* BPROJEPS = nfa_dyck_ext(BPROJ, FOLD);
    delete_parti(FOLD);
    nfa_delete(BPROJ);
    BEPS->trans_e = BPROJEPS->trans_e;
    BPROJEPS->trans_e = NULL;
    nfa_delete(BPROJEPS);
    nfa_remove_inv(BEPS);
    nfa_elimeps_mod(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(
            out,
            "#### Phase 1: Building ε-transitions in the second input language.\n");
        fprintf(out, "####          An ε-transition is added for each pair of "
            "states inducing\n");
        fprintf(out,
            "####          a language which is not MOD-separable from {ε}\n");
        fprintf(out, "####          These ε-transitions are then eliminated to "
            "simplify the computations.\n");
        view_nfa(BEPS);
    }

    // Phase 2
    nfa* INTERSECT = nfa_intersect(A, BEPS, true);
    nfa_delete(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 2: Intersecting the resulting NFA with that of "
            "the first input language.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_dequeue(INTERSECT->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        nfa_delete(INTERSECT);
        return true;
    }
    else
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        nfa_delete(INTERSECT);
        return false;
    }
}

// Pol(GR)-separation of A from B
bool decid_polgr_sep(nfa* A, nfa* B, bool details, FILE* out)
{
    // Gestion des cas triviaux
    if (isempty_dequeue(A->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(B->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (A == B)
    {
        if (out && details)
        {
            fprintf(out,
                "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }

    // Phase 1
    parti* SCCS = nfa_inv_ext(B);
    parti* FOLD = nfa_stal_fold(B, SCCS);
    delete_parti(SCCS);
    nfa* BEPS = nfa_dyck_ext(B, FOLD);
    nfa_remove_inv(B);
    delete_parti(FOLD);
    nfa_remove_inv(BEPS);
    nfa_elimeps_mod(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(
            out,
            "#### Phase 1: Building ε-transitions in the second input language.\n");
        fprintf(out, "####          An ε-transition is added for each pair of "
            "states inducing\n");
        fprintf(out,
            "####          a language which is not GR-separable from {ε}\n");
        fprintf(out, "####          These ε-transitions are then eliminated to "
            "simplify the computations.\n");
        view_nfa(BEPS);
    }

    // Phase 2
    nfa* INTERSECT = nfa_intersect(A, BEPS, true);
    nfa_delete(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 2: Intersecting the resulting NFA with that of "
            "the first input language.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_dequeue(INTERSECT->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        nfa_delete(INTERSECT);
        return true;
    }
    else
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        nfa_delete(INTERSECT);
        return false;
    }
}
