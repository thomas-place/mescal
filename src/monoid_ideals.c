/*********************/
/* Ideals of monoids */
/*********************/

#include "monoid_ideals.h"

/**************************/
/* Computations of ideals */
/**************************/

// Calcul d'un idéal droit (la liste triée des éléments de l'ensemble sM)
p_vertices compute_r_ideal(p_cayley M, uint s, bool* rest)
{
    p_vertices ini = create_vertices();
    rigins_vertices(s, ini);
    p_vertices res = dgraph_dfs(M->trans, ini, NULL, rest);
    delete_vertices(ini);
    return res;
}

// Calcul d'un idéal gauche (la liste triée des éléments de l'ensemble Ms)
p_vertices compute_l_ideal(p_cayley M, uint s, bool* rest)
{
    // On commence par calculer (si besoin) le graphe gauche
    compute_left_cayley(M);

    p_vertices ini = create_vertices();
    rigins_vertices(s, ini);
    p_vertices res = dgraph_dfs(M->leftgraph, ini, NULL, rest);
    delete_vertices(ini);
    return res;
}

// Calcul d'un idéal droit restreint par l'alphabet.
p_vertices compute_r_ideal_alph(p_cayley M, uint s, bool* alph, bool* rest)
{
    p_vertices ini = create_vertices();
    rigins_vertices(s, ini);
    p_vertices res = dgraph_dfs(M->trans, ini, alph, rest);
    delete_vertices(ini);
    return res;
}

// Calcul d'un idéal gauche restreint par l'alphabet.
p_vertices compute_l_ideal_alph(p_cayley M, uint s, bool* alph, bool* rest)
{
    // On commence par calculer (si besoin) le graphe gauche
    compute_left_cayley(M);

    p_vertices ini = create_vertices();
    rigins_vertices(s, ini);
    p_vertices res = dgraph_dfs(M->leftgraph, ini, alph, rest);
    delete_vertices(ini);
    return res;
}

/***********************/
/* Gestion des orbites */
/***********************/

p_orbits init_orbits(p_green_sub S) {
    p_orbits res;
    MALLOC(res, 1);
    res->oneonly = true;
    res->sub = S;
    res->nb_idems = size_vertices(S->original->idem_list);
    return res;
}

// Suppression (le Booléen indque si on doit aussi supprimer sub)
void delete_orbits(p_orbits L, bool delsub)
{
    if (L == NULL)
    {
        return;
    }
    if (L->sub != NULL && delsub) {
        delete_green_sub(L->sub);
    }
    if (L->orbits == NULL)
    {
        free(L);
        return;
    }

    for (uint i = 0; i < L->nb_idems; i++)
    {
        delete_green_sub(L->orbits[i]);
    }
    free(L->orbits);
    free(L);
}

// Affichage d'un ensemble d'orbites
void print_all_orbs(p_orbits L, char* namecl, FILE* out) {
    fprintf(out, "#### The list of all idempotents:\n");
    cayley_print_idems(L->sub->original, out);
    if (L->oneonly) {
        fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Displaying only the latter.\n", namecl, namecl);
        print_sep_line(100, out);
        fprintf(out, "#### %s-orbit of the idempotent 1.\n", namecl);
        print_full_green(L->sub->original, L->sub->originalrels, false, out);
        return;
    }
    else {
        fprintf(out, "#### Printing the %s-orbits of all idempotents:\n", namecl);
        for (uint i = 0; i < L->nb_idems; i++)
        {
            print_sep_line(100, out);
            fprintf(out, "#### %s-orbit of the idempotent ", namecl);
            cayley_print_name(L->sub->original, lefread_vertices(L->sub->original->idem_list, i), out);
            printf(".\n");
            print_full_green_sub(L->orbits[i], false, out);
        }

    }
    return;

}

void print_one_orb(p_orbits L, char* namecl, uint idem_num, FILE* out) {
    if (L->oneonly) {
        fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Displaying the latter.\n", namecl, namecl);
        print_sep_line(100, out);
        fprintf(out, "#### %s-orbit of the idempotent 1.\n", namecl);
        print_full_green(L->sub->original, L->sub->originalrels, false, out);
        return;
    }
    print_full_green_sub(L->orbits[idem_num], false, out);
}

/**************/
/* DD-orbites */
/**************/

// Calcule la DD-orbite associée à l'idempotent e
p_green_sub compute_one_ddorb(p_cayley M, p_green G, uint e)
{
    p_green_sub S = init_submono(M, G);

    // Si le neutre a un antécédent non-vide, l'orbite de 1 est le monoide complet
    if (e == ONE && cayley_neutstrict(M, G)) {
        return S;
    }


    // Calcul de l'ensemble des éléments
    // Si e est l'élément neutre et qu'il n'a pas d'antécédent non-vide
    // Alors son orbite est le singleton {1}
    if (e == ONE && !cayley_neutstrict(M, G))
    {
        S->size = 1;
        MALLOC(S->mono_in_sub, M->trans->size_graph);
        MALLOC(S->mono_to_sub, M->trans->size_graph);
        S->mono_in_sub[ONE] = true;
        S->mono_to_sub[ONE] = ONE;
        for (uint q = 1; q < M->trans->size_graph; q++)
        {
            S->mono_in_sub[q] = false;
        }

        MALLOC(S->sub_to_mono, 1);
        S->sub_to_mono[ONE] = ONE;
        S->neut = ONE;
    }
    else
    {

        p_vertices eM = compute_r_ideal(M, e, NULL);
        p_vertices Me = compute_l_ideal(M, e, NULL);
        p_vertices eMe = make_inter_sorted_vertices(eM, Me);

        S->size = size_vertices(eMe);

        MALLOC(S->sub_to_mono, size_vertices(eMe));
        MALLOC(S->mono_in_sub, M->trans->size_graph);
        MALLOC(S->mono_to_sub, M->trans->size_graph);

        for (uint q = 0; q < M->trans->size_graph; q++)
        {
            S->mono_in_sub[q] = false;
        }
        for (uint i = 0; i < size_vertices(eMe); i++)
        {
            S->sub_to_mono[i] = lefread_vertices(eMe, i);
            S->mono_to_sub[lefread_vertices(eMe, i)] = i;
            S->mono_in_sub[lefread_vertices(eMe, i)] = true;
        }
        S->neut = S->mono_to_sub[e];
        delete_vertices(eM);
        delete_vertices(Me);
        delete_vertices(eMe);
    }

    // Calcul des listes d'idempotents
    S->idem_list = create_vertices();
    MALLOC(S->idem_array, S->size);
    for (uint s = 0; s < S->size; s++)
    {
        if (M->idem_array[S->sub_to_mono[s]])
        {
            S->idem_array[s] = true;
            rigins_vertices(s, S->idem_list);
        }
        else
        {
            S->idem_array[s] = false;
        }
    }

    // Calcul des relations de Green (restriction de celles du monoide original)
    MALLOC(S->rels, 1);
    uint num;

    // R-classes
    MALLOC(S->rels->RCL, 1);
    S->rels->RCL->size_set = S->size;
    MALLOC(S->rels->RCL->numcl, S->size);
    num = 0;
    for (uint i = 0; i < G->RCL->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(G->RCL->cl[i]); j++)
        {
            if (S->mono_in_sub[lefread_vertices(G->RCL->cl[i], j)])
            {
                inter = true;
                S->rels->RCL->numcl[S->mono_to_sub[lefread_vertices(G->RCL->cl[i], j)]] = num;
            }
        }
        if (inter)
        {
            num++;
        }
    }

    MALLOC(S->rels->RCL->cl, num);
    S->rels->RCL->size_par = num;
    for (uint i = 0; i < num; i++)
    {
        S->rels->RCL->cl[i] = create_vertices();
    }

    for (uint s = 0; s < S->size; s++)
    {
        rigins_vertices(s, S->rels->RCL->cl[S->rels->RCL->numcl[s]]);
    }

    // L-classes
    MALLOC(S->rels->LCL, 1);
    S->rels->LCL->size_set = S->size;
    MALLOC(S->rels->LCL->numcl, S->size);
    num = 0;
    for (uint i = 0; i < G->LCL->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(G->LCL->cl[i]); j++)
        {
            if (S->mono_in_sub[lefread_vertices(G->LCL->cl[i], j)])
            {
                inter = true;
                S->rels->LCL->numcl[S->mono_to_sub[lefread_vertices(G->LCL->cl[i], j)]] = num;
            }
        }
        if (inter)
        {
            num++;
        }
    }
    MALLOC(S->rels->LCL->cl, num);
    S->rels->LCL->size_par = num;
    for (uint i = 0; i < num; i++)
    {
        S->rels->LCL->cl[i] = create_vertices();
    }

    for (uint s = 0; s < S->size; s++)
    {
        rigins_vertices(s, S->rels->LCL->cl[S->rels->LCL->numcl[s]]);
    }

    // J-classes
    MALLOC(S->rels->JCL, 1);
    S->rels->JCL->size_set = S->size;
    MALLOC(S->rels->JCL->numcl, S->size);
    num = 0;
    for (uint i = 0; i < G->JCL->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(G->JCL->cl[i]); j++)
        {
            if (S->mono_in_sub[lefread_vertices(G->JCL->cl[i], j)])
            {
                inter = true;
                S->rels->JCL->numcl[S->mono_to_sub[lefread_vertices(G->JCL->cl[i], j)]] = num;
            }
        }
        if (inter)
        {
            num++;
        }
    }
    MALLOC(S->rels->JCL->cl, num);
    S->rels->JCL->size_par = num;
    for (uint i = 0; i < num; i++)
    {
        S->rels->JCL->cl[i] = create_vertices();
    }

    for (uint s = 0; s < S->size; s++)
    {
        rigins_vertices(s, S->rels->JCL->cl[S->rels->JCL->numcl[s]]);
    }

    // H-classes
    MALLOC(S->rels->HCL, 1);
    S->rels->HCL->size_set = S->size;
    MALLOC(S->rels->HCL->numcl, S->size);
    num = 0;
    for (uint i = 0; i < G->HCL->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(G->HCL->cl[i]); j++)
        {
            if (S->mono_in_sub[lefread_vertices(G->HCL->cl[i], j)])
            {
                inter = true;
                S->rels->HCL->numcl[S->mono_to_sub[lefread_vertices(G->HCL->cl[i], j)]] = num;
            }
        }
        if (inter)
        {
            num++;
        }
    }
    MALLOC(S->rels->HCL->cl, num);
    S->rels->HCL->size_par = num;
    for (uint i = 0; i < num; i++)
    {
        S->rels->HCL->cl[i] = create_vertices();
    }

    for (uint s = 0; s < S->size; s++)
    {
        rigins_vertices(s, S->rels->HCL->cl[S->rels->HCL->numcl[s]]);
    }

    // Groupes et J-classes régulières
    gr_green_compute(S->idem_list, S->rels);
    return S;
}

// Calcule toutes les DD-orbites associées
p_orbits compute_all_ddorbs(p_cayley M, p_green G)
{

    p_orbits res = init_orbits(init_submono(M, G));


    // Si le neutre a un antécédent non-vide, toutes les DD-orbites
    // sont des sous-semigroupes de celle de 1 (qui est le monoide entier).
    // On n'effectue donc le calcul que dans le cas contraire
    if (!cayley_neutstrict(M, G)) {
        res->oneonly = false;
        MALLOC(res->orbits, res->nb_idems);
        for (uint i = 0; i < res->nb_idems; i++)
        {
            uint e = lefread_vertices(M->idem_list, i);
            res->orbits[i] = compute_one_ddorb(M, G, e);
        }


    }
    return res;
}



/********************************/
/* Liens entre deux idempotents */
/********************************/

// // Pour deux idempotents e,f calcule tous le sous-ensemble eMf
// p_vertices compute_ef_links(p_cayley M, uint e, uint f, bool *edone, bool *fdone)
// {
//     // Calcul de l'ensemble des éléments
//     p_vertices eM = compute_r_ideal(M, e);
//     if (edone != NULL)
//     {
//         for (uint i = 0; i < size_vertices(eM); i++)
//         {
//             edone[lefread_vertices(eM, i)] = true;
//         }
//     }
//     p_vertices Mf = compute_l_ideal(M, f);
//     if (fdone != NULL)
//     {
//         for (uint i = 0; i < size_vertices(Mf); i++)
//         {
//             fdone[lefread_vertices(Mf, i)] = true;
//         }
//     }
//     return make_inter_sorted_vertices(eM, Mf);
// }
