#include "sep_pol.h"

// Commence par éliminer les éventuelles transitions epsilon.
// Puis, double toute transition existante d'une transition epsilon,
// dans un nouveau NFA.
static nfa *nfa_add_eps(nfa *A)
{
    if (!A)
    {
        return NULL;
    }
    if (A->epsilon)
    {
        A = nfa_elimeps(A);
    }

    dyn_edge_triple *trans = create_dyn_edge_triple();
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->nb_letters; a++)
        {
            uint start = A->trans->intervals[q * A->trans->size_alpha + a];
            uint end = A->trans->intervals[q * A->trans->size_alpha + a + 1];
            for (uint i = start; i < end; i++)
            {
                dyn_edge_triple_add(trans, q, a, A->trans->storage[i]);
                dyn_edge_triple_add(trans, q, A->nb_letters, A->trans->storage[i]);
            }
        }
    }

    nfa *B;
    CALLOC(B, 1);
    B->nb_letters = A->nb_letters;
    B->alphabet = duplicate_alphabet(A->alphabet, A->nb_letters);
    B->nb_initials = A->nb_initials;
    CALLOC(B->initials, B->nb_initials);
    for (uint i = 0; i < B->nb_initials; i++)
    {
        B->initials[i] = A->initials[i];
    }
    B->nb_finals = A->nb_finals;
    CALLOC(B->finals, B->nb_finals);
    for (uint i = 0; i < B->nb_finals; i++)
    {
        B->finals[i] = A->finals[i];
    }
    B->state_names = NULL;
    B->epsilon = true;
    B->inverse = false;
    B->trans = edge_list_to_lgraph(trans->array, trans->size, A->trans->size_graph, B->nb_letters + 1);
    return B;
}

bool decid_polst_sep(nfa *A, nfa *B, bool details, FILE *out)
{
    // Gestion des cas triviaux
    if (A->nb_finals == 0)
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (B->nb_finals == 0)
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
    nfa *BEPS = nfa_add_eps(B);
    nfa *eBEPS = nfa_elimeps(BEPS);
    nfa_delete(BEPS);
    BEPS = eBEPS;
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
    nfa *INTERSECT = nfa_intersect(A, BEPS, true);
    nfa_delete(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 2: Intersecting the resulting NFA with that of "
                     "the first input language.\n");
        view_nfa(INTERSECT);
    }
    if (INTERSECT->nb_finals == 0)
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

static bool decid_genpolg_sep(nfa *A, nfa *B, bool grp, bool details, FILE *out)
{
    details = true;
    // Gestion des cas triviaux
    if (A->nb_finals == 0)
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (B->nb_finals == 0)
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
    nfa *BEPS = nfa_dyck_ext(B, grp, out && details, NULL, NULL);
    nfa *eBEPS = nfa_remove_inv(BEPS);
    nfa_delete(BEPS);
    BEPS = eBEPS;

    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 1: Building ε-transitions in the second input language.\n");
        fprintf(out, "####          An ε-transition is added for each pair of states inducing\n");
        if (grp)
        {
            fprintf(out, "####          a language which is not GR-separable from {ε}\n");
        }
        else
        {
            fprintf(out, "####          a language which is not MOD-separable from {ε}\n");
        }
        view_nfa(BEPS);
    }

    // Phase 2
    nfa *INTERSECT = nfa_intersect(A, BEPS, out && details);
    nfa_delete(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 2: Intersecting the resulting NFA with that of "
                     "the first input language.\n");
        view_nfa(INTERSECT);
    }
    if (INTERSECT->nb_finals == 0)
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

bool decid_polmod_sep(nfa *A, nfa *B, bool details, FILE *out)
{
    return decid_genpolg_sep(A, B, false, details, out);
}

bool decid_polgr_sep(nfa *A, nfa *B, bool details, FILE *out)
{
    return decid_genpolg_sep(A, B, true, details, out);
}

// void temp_fun(dgraph *ge, dgraph *gf, basis mode, uint, uint, bool, FILE *)
// {
//     if (mode == BA_ST)
//     {
//         fprintf(stderr, "ST\n");
//         return;
//     }
//     parti *sccse = dtarjan(ge, NULL, false);
//     parti *sccsf = dtarjan(gf, NULL, false);
//     parti *folde = dgraph_stal_fold(ge, sccse, mode);
//     parti *foldf = dgraph_stal_fold(gf, sccsf, mode);

//     uint inisize = folde->size_par * foldf->size_par;
//     dgraph *inig = create_dgraph_noedges(inisize, ge->size_alpha);

//     for (uint a = 0; a < ge->size_alpha; a++)
//     {

//         for (uint i = 0; i < ge->size_graph; i++)
//         {
//             uint ia = ge->edges[i][a];
//             if (ia == UINT_MAX)
//             {
//                 continue;
//             }
//             uint ci = folde->numcl[i];
//             uint cia = folde->numcl[ia];

//             for (uint j = 0; j < gf->size_graph; j++)
//             {
//                 uint ja = gf->edges[j][a];
//                 if (ja == UINT_MAX)
//                 {
//                     continue;
//                 }
//                 uint cj = foldf->numcl[j];
//                 uint cja = foldf->numcl[ja];

//                 inig->edges[ci * foldf->size_par + cj][a] = cia * foldf->size_par + cja;
//             }
//         }
//     }

//     do
//     {
//         dgraph *prode = dgraph_direct_product(ge, inig);
//         dgraph *prodf = dgraph_direct_product(gf, inig);
//         parti *sccsprode = dtarjan(prode, NULL, false);
//         parti *sccsprodf = dtarjan(prodf, NULL, false);
//         parti *foldeprode = dgraph_stal_fold(prode, sccsprode, mode);
//         parti *foldfprodf = dgraph_stal_fold(prodf, sccsprodf, mode);
//         delete_dgraph(inig);
//         ufind *uf = create_ufind(ge->size_graph * gf->size_graph);

//         for (uint i = 0; i < ge->size_graph; i++)
//         {
//             for (uint j = 0; j < gf->size_graph; j++)
//             {
//                 uint p = i * gf->size_graph + j;

//                 uint ci = foldeprode->numcl[i];
//                 uint cj = foldfprodf->numcl[j];
//                 if (ci == cj)
//                 {
//                     ufind_union(uf, i * gf->size_graph + j, 0);
//                 }
//             }
//         }

//     } while (true);
// }
