/**************************************************/
/* Fonctions sur les hiérarchies de concatenation */
/**************************************************/

#include "class_concath.h"

/********************/
/* Membership - Pol */
/********************/

bool is_jsat_mono(p_cayley M, FILE* out)
{
    if (M->order == NULL)
    {
        printf("Error. The order is not computed.\n");
        return false;
    }

    if (out) {
        fprintf(out, "#### Checking if the ordered monoid M satisfies the equation 1 ≤ s for all s ∊ M.\n");
        fprintf(out, "#### The ordering :\n");
        cayley_print_order(M, out);
    }

    if (M->trans->size_graph != size_vertices(M->order[ONE]))
    {
        if (out)
        {
            fprintf(out, "#### The monoid does not satisfy the equation 1 ≤ s for "
                "all s ∊ M\n");
            uint i = 0;
            while (i < size_vertices(M->order[ONE]) &&
                lefread_vertices(M->order[ONE], i) == i) {
                i++;
            }
            fprintf(out, "#### For instance, 1 ≰ ");
            cayley_print_name(M, i, out);
            fprintf(out, ".\n");
        }
        return false;
    }
    if (out) {
        fprintf(out, "#### The monoid satisfies the equation 1 ≤ s for all s ∊ M.\n");
    }
    return true;
}

bool is_ejsat_mono(p_cayley M, FILE* out)
{


    if (M->order == NULL)
    {
        printf("Error. The order is not computed.\n");
        return false;
    }

    if (out) {
        fprintf(out, "#### Checking if the ordered monoid M satisfies the equation 1 ≤ e for all e ∊ E(M).\n");
        fprintf(out, "#### The ordering :\n");
        cayley_print_order(M, out);
    }

    uint i = 0;
    uint j = 0;
    while (i < size_vertices(M->idem_list)) {
        if (j >= size_vertices(M->order[ONE]) ||
            lefread_vertices(M->order[ONE], j) >
            lefread_vertices(M->idem_list, i))
        {
            if (out)
            {
                fprintf(out, "#### The monoid does not satisfy the equation 1 ≤ e for "
                    "all idempotents e.\n");
                fprintf(out, "#### For instance, 1 ≰ ");
                cayley_print_name(M, lefread_vertices(M->idem_list, i), out);
                fprintf(out, ".\n");
            }
            return false;
        }
        else if (lefread_vertices(M->order[ONE], j) <
            lefread_vertices(M->idem_list, i))
        {
            j++;
        }
        else
        {
            i++;
            j++;
        }
    }
    if (out) {
        fprintf(out, "#### The monoid satisfies the equation 1 ≦ e for all idempotents e.\n");
    }
    return true;
}

bool is_jsat_submono(p_green_sub S, char* name, FILE* out)
{
    p_cayley M = S->original;
    if (M->order == NULL)
    {
        if (out)
        {
            fprintf(out, "Error. The order was not computed.\n");
        }

        return false;
    }

    if (out) {
        fprintf(out, "#### Checking whether the %s satisfies the equation 1 ≤ s for all s. Displaying the %s.\n", name, name);
        print_full_green_sub(S, false, stdout);
        submono_print_order(S, out);
    }

    if (S->mono_in_sub == NULL) {
        return is_jsat_mono(M, out);
    }

    uint i = 0;
    uint j = 0;
    while (i < S->size) {
        if (j >= size_vertices(M->order[S->sub_to_mono[S->neut]]) ||
            lefread_vertices(M->order[S->sub_to_mono[S->neut]], j) >
            S->sub_to_mono[i])
        {
            if (out)
            {
                fprintf(out, "#### The monoid does not satisfy the equation 1 ≤ s.\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, S->sub_to_mono[S->neut], out);
                fprintf(out, " ≰ ");
                cayley_print_name(M, S->sub_to_mono[i], out);
                fprintf(out, " and ");
                cayley_print_name(M, S->sub_to_mono[S->neut], out);
                fprintf(out, " is the neutral element.\n");
            }
            return false;
        }
        else if (lefread_vertices(M->order[S->sub_to_mono[S->neut]], j) <
            S->sub_to_mono[i])
        {
            j++;
        }
        else
        {
            i++;
            j++;
        }
    }
    if (out)
    {
        fprintf(out, "#### The monoid satisfies the equation 1 ≤ s for all s.\n");
    }
    return true;
}

// Equation 1 <= s pour un ensemble de C-orbites
// (la chaîne de caractère sert à afficher le nom de la classe C)
bool is_jsat_orbmono(p_orbits L, char* class, FILE* out)
{

    if (out) {
        fprintf(out, "#### Checking whether the %s-orbits satisfy the equation 1 ≤ s for all s.\n", class);
        fprintf(out, "#### The list of all idempotents:\n");
        cayley_print_idems(L->sub->original, out);
    }
    char nameorb[100];

    if (L->oneonly) {
        if (out) {
            fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Checking only the latter.\n", class, class);
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of 1", class);
        return is_jsat_submono(L->sub, nameorb, out);
    }

    p_cayley M = L->sub->original;

    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        if (out)
        {
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of %s", class, cayley_get_name(M, e));

        if (!is_jsat_submono(L->orbits[i], nameorb, out))
        {
            return false;
        }
    }
    return true;
}

/*************************/
/* Membership - UPolBPol */
/*************************/

// Membership in UPol(BPol(C)) (les C-orbites du monoide doivent être passées en
// entrée)
bool is_upbp_mono(p_orbits L, char* class, FILE* out)
{

    p_cayley M = L->sub->original;
    p_green G = L->sub->originalrels;
    if (out) {
        fprintf(out, "#### Checking if the morphism satisfies the equation (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ\n");
        fprintf(out, "     for all s, t ∊ M and e ∊ E(M) such that (e,s) is an %s-pair.\n", class);
    }

    // Boucle sur tous les idempotents e
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);

        // Calcul de l'ensemble eMe restreint aux éléments réguliers
        p_vertices eM = compute_r_ideal(M, e, G->regular_array);
        p_vertices Me = compute_l_ideal(M, e, G->regular_array);
        p_vertices eMe = make_inter_sorted_vertices(eM, Me);
        delete_vertices(Me);
        delete_vertices(eM);

        // Pour tout élément ese dans l'orbite de e
        for (uint k = 0; k < L->orbits[i]->size; k++)
        {
            uint ese = L->orbits[i]->sub_to_mono[k];

            // Pour tout élément ete dans eMe tel que esete est idempotent et
            // satisfait esete J ete
            for (uint l = 0; l < size_vertices(eMe); l++)
            {
                uint ete = lefread_vertices(eMe, l);
                uint esete = cayley_mult(M, ese, ete);
                if (G->JCL->numcl[esete] == G->JCL->numcl[ete] &&
                    M->idem_array[esete])
                {
                    if (esete != cayley_mult_gen(M, 3, esete, ete, esete))
                    {
                        if (out)
                        {
                            fprintf(out, "#### This morphism does not satisfy the equation "
                                "(esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ\n");
                            fprintf(
                                out,
                                "     for all %s-pairs (e,s) ∊ M² and all elements t ∊ M.\n",
                                class);
                            fprintf(out, "#### For instance, it fails for e = ");
                            cayley_print_name(M, e, out);
                            fprintf(out, ", s = ");
                            cayley_print_name(M, ese, out);
                            fprintf(out, " and t = ");
                            cayley_print_name(M, ete, out);
                            fprintf(out, ".\n");
                        }
                        return false;
                    }
                }
            }
        }
        delete_vertices(eMe);
    }
    if (out)
    {
        fprintf(out, "#### This morphism satisfies the equation (esete)ʷ⁺¹ = "
            "(esete)ʷete(esete)ʷ\n");
        fprintf(out, "     for all %s-pairs (e,s) ∊ M² and all elements t ∊ M.\n",
            class);
    }
    return true;
}

/*********************/
/* Membership - BPol */
/*********************/

// J-triviality (PT)
bool is_jtrivial_mono(p_cayley M, p_green G, FILE* out)
{
    if (out) {
        fprintf(stdout, "#### Checking if the monoid is J-trivial.\n");
    }
    // Si il y a autant d'éléments que de J-classes, le monoide est HJ-trivial
    if (G->JCL->size_set == G->JCL->size_par)
    {
        if (out)
        {
            fprintf(out, "#### This is a J-trivial monoid.\n");
        }
        return true;
    }

    // Sinon (si on doit afficher), on va chercher un contre-exemple
    if (out)
    {
        // On regarde toutes les J-classes
        for (uint i = 0; i < G->JCL->size_par; i++)
        {
            // On prend la première qui est non-triviale
            if (size_vertices(G->JCL->cl[i]) >= 2)
            {
                uint s = lefread_vertices(G->JCL->cl[i], 0);
                uint t = lefread_vertices(G->JCL->cl[i], 1);
                fprintf(out, "#### This is NOT a J-trivial monoid.\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, s, out);
                fprintf(out, " J ");
                cayley_print_name(M, t, out);
                fprintf(out, ".\n");
                return false;
            }
        }
    }
    return false;
}

bool is_jtrivial_submono(p_cayley M, p_green_sub S, FILE* out)
{
    // Si il y a autant d'éléments que de J-classes, le monoide est J-trivial
    if (S->rels->JCL->size_set == S->rels->JCL->size_par)
    {
        if (out)
        {
            fprintf(out, "#### This is a J-trivial monoid.\n");
        }
        return true;
    }

    // Sinon (si on doit afficher), on va chercher un contre-exemple
    if (out)
    {
        // On regarde toutes les J-classes
        for (uint i = 0; i < S->rels->JCL->size_par; i++)
        {
            // On prend la première qui est non-triviale
            if (size_vertices(S->rels->JCL->cl[i]) >= 2)
            {
                uint s = lefread_vertices(S->rels->JCL->cl[i], 0);
                uint t = lefread_vertices(S->rels->JCL->cl[i], 1);
                fprintf(out, "#### This is NOT an J-trivial monoid.\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, S->sub_to_mono[s], out);
                fprintf(out, " L ");
                cayley_print_name(M, S->sub_to_mono[t], out);
                fprintf(out, ".\n");
                return false;
            }
        }
    }
    return false;
}

// Block-group
bool is_blockg_mono(p_cayley M, p_green G, FILE* out)
{
    if (out) {
        fprintf(stdout, "#### Checking if the monoid is a block group.\n");
    }
    uint ridem[G->RCL->size_par];
    for (uint i = 0; i < G->RCL->size_par; i++)
    {
        ridem[i] = ONE;
    }
    uint lidem[G->LCL->size_par];
    for (uint i = 0; i < G->LCL->size_par; i++)
    {
        lidem[i] = ONE;
    }

    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        uint cr = G->RCL->numcl[e];
        if (ridem[cr] != ONE)
        {
            if (out)
            {
                fprintf(out, "#### This monoid is not a block group.\n");
                fprintf(out, "#### For instance, the idempotents ");
                cayley_print_name(M, ridem[cr], out);
                fprintf(out, " and ");
                cayley_print_name(M, e, out);
                fprintf(out, " satisfy ");
                cayley_print_name(M, ridem[cr], out);
                fprintf(out, " R ");
                cayley_print_name(M, e, out);
                fprintf(out, ".\n");
            }
            return false;
        }
        else
        {
            ridem[cr] = e;
        }
        uint cl = G->LCL->numcl[e];
        if (lidem[cl] != ONE)
        {
            if (out)
            {
                fprintf(out, "#### This monoid is not a block group.\n");
                fprintf(out, "#### For instance, the idempotents ");
                cayley_print_name(M, lidem[cl], out);
                fprintf(out, " and ");
                cayley_print_name(M, e, out);
                fprintf(out, " satisfy ");
                cayley_print_name(M, lidem[cl], out);
                fprintf(out, " L ");
                cayley_print_name(M, e, out);
                fprintf(out, ".\n");
            }
            return false;
        }
        else
        {
            lidem[cl] = e;
        }
    }
    if (out)
    {
        fprintf(out, "#### This monoid is a block group.\n");
    }
    return true;
}

// static p_nfa auto_from_rclass(p_cayley M, p_green G, uint e) {

//     // Création du nouveau NFA
//     p_nfa A;
//     MALLOC(A, 1);
//     A->initials = create_vertices();
//     A->finals = create_vertices();
//     rigins_vertices(e, A->initials);
//     A->trans = create_lgraph_noedges(size_vertices(G->RCL->cl[e]), 1);
//     A->etrans = NULL;
//     A->itrans = NULL;
//     A->ntype == NUMBER;
//     nfa_init_names(A);
//     for (uint i = 0; i < size_vertices(G->RCL->cl[e]); i++)
//     {
//         uint s = lefread_vertices(G->RCL->cl[e], i);
//         ((uint*)A->names)[i] = s;
//         if (s == e) {
//             rigins_vertices(i, A->initials);
//         }
//         for (uint a = 0; a < A->trans->size_alpha; a++)
//         {
//             if (G->RCL->numcl[e] == G->RCL->numcl[M->trans->edges[s][a]]) {

//             }
//         }
//     }


//     return A;
// }


// Mod
bool is_bpolmod_mono(p_cayley M, p_green G, FILE* out)
{
    if (out) {
        fprintf(stdout, "#### Checking if the morphism satisfies the equation (qr)ʷst(st)ʷ = (qr)ʷqt(st)ʷ\n");
        fprintf(stdout, "     for all q,r,s,t ∊ M such that (q,s) is a MOD-pair.\n");
    }

    // Calcul des MOD-paires
    p_nfa A = cayley_to_dfa(M);
    p_nfa U = nfa_proj_unary(A);
    delete_nfa(A);
    p_parti SCCS = nfa_inv_ext(U);
    p_parti FOLD = nfa_stal_fold(U, SCCS);
    delete_parti(SCCS);
    p_nfa B = nfa_dyck_ext(U, FOLD);
    delete_nfa(U);
    nfa_delete_names(B);
    p_nfa INTER = nfa_intersect(B, B, true);
    delete_nfa(B);

    // // Pour tout idempotent e
    // for (uint i = 0; i < size_vertices(M->idem_list); i++) {
    //     uint e = lefread_vertices(M->idem_list, i);

    //     // Pour tout idempotent f
    //     for (uint j = 0; j < size_vertices(M->idem_list); j++) {
    //         uint f = lefread_vertices(M->idem_list, j);


    //     }

    // }



    for (uint p = 0; p < INTER->trans->size_graph; p++)
    {
        uint c = ((uint**)INTER->names)[p][0];
        uint d = ((uint**)INTER->names)[p][1];
        // printf("c: %d, d: %d\n", c, d);

        for (uint i = 0; i < size_vertices(FOLD->cl[c]); i++)
        {
            uint q = lefread_vertices(FOLD->cl[c], i);
            // cayley_print_name(M, q, out);
            // printf("  num: %d \n", q);
            // Il suffit de tester les q réguliers
            if (G->regular_array[q])
            {
                // Boucle sur les r tels que q J r
                for (uint j = 0; j < size_vertices(G->JCL->cl[G->JCL->numcl[q]]); j++)
                {
                    uint r = lefread_vertices(G->JCL->cl[G->JCL->numcl[q]], j);
                    uint qr = cayley_mult(M, q, r);
                    // Il suffit de tester les qr idempotents et tels que qr J q
                    if (M->idem_array[qr] && G->JCL->numcl[qr] == G->JCL->numcl[q])
                    {
                        for (uint k = 0; k < size_vertices(FOLD->cl[d]); k++)
                        {
                            uint s = lefread_vertices(FOLD->cl[d], k);
                            // cayley_print_name(M, s, out);
                            // printf("  num: %d \n", s);
                            // Il suffit de tester les s réguliers
                            if (G->regular_array[s])
                            {
                                // Boucle sur les t tels que s J t
                                for (uint l = 0;
                                    l < size_vertices(G->JCL->cl[G->JCL->numcl[s]]); l++)
                                {
                                    uint t = lefread_vertices(G->JCL->cl[G->JCL->numcl[s]], l);
                                    uint st = cayley_mult(M, s, t);
                                    // Il suffit de tester les st idempotents et tels que st J t
                                    if (M->idem_array[st] &&
                                        G->JCL->numcl[st] == G->JCL->numcl[s])
                                    {
                                        if (cayley_mult(M, qr, st) !=
                                            cayley_mult_gen(M, 4, qr, q, t, st))
                                        {
                                            if (out)
                                            {
                                                fprintf(out,
                                                    "#### This morphism does not satisfy the "
                                                    "equation (qr)ʷst(st)ʷ = (qr)ʷqt(st)ʷ for "
                                                    "all MOD-pairs (q,s).\n");
                                                fprintf(out,
                                                    "#### For instance, it fails for q = ");
                                                cayley_print_name(M, q, out);
                                                fprintf(out, ", r = ");
                                                cayley_print_name(M, r, out);
                                                fprintf(out, ", s = ");
                                                cayley_print_name(M, s, out);
                                                fprintf(out, " and t = ");
                                                cayley_print_name(M, t, out);
                                                fprintf(out, ".\n");
                                            }
                                            return false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    delete_nfa(INTER);
    delete_parti(FOLD);
    if (out)
    {
        fprintf(out, "#### This morphism satisfies the equation (qr)ʷst(st)ʷ = "
            "(qr)ʷqt(st)ʷ for all MOD-pairs (q,s).\n");
    }
    return true;
}

// Knast
bool is_knast_mono(p_cayley M, p_green G, FILE* out)
{
    if (out) {
        fprintf(stdout, "#### Checking if the morphism satisfies Knast's equation.\n");
    }
    if (cayley_neutstrict(M, G)) {
        if (out) {
            fprintf(stdout, "#### Since the neutral element 1 has a nonempty antecedent, Knast's equation holds if and only if\n");
            fprintf(stdout, "     the monoid is J-trivial.\n");
        }
        return is_jtrivial_mono(M, G, stdout);
    }




    // Tableau qui mémorise les idempotents e déjà traités (il arrive que certains
    // soient traités simultanément)
    bool edone[M->trans->size_graph];
    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        edone[lefread_vertices(M->idem_list, i)] = false;
    }

    // Boucles sur tous les idempotents e
    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);

        // Si e n'a pas déjà été traité dans les calculs précédent
        if (!edone[e])
        {
            // fprintf(out, "e: ");
            // cayley_print_name(M, e, out);
            // fprintf(out, "\n");
            // Il faut maintenant considérer tous les idempotents f.
            // L'équation étant symmétrique, il suffit de considérer les f tels que e
            // <= f

            // Tableau qui mémorise les idempotents f déjà traités pour ce e (il
            // arrive que certains soient traités simultanément)
            bool fdone[M->trans->size_graph];
            for (uint j = i; j < size_vertices(M->idem_list); j++)
            {
                fdone[lefread_vertices(M->idem_list, j)] = false;
            }

            // Calcul des ensembles eM et Me (on ne garde que les éléments réguliers
            // dans ces ensembles) On marque les idempotents e' dans eM et Me: ils
            // sont traités automatiquement en même temps que e
            p_vertices eM = compute_r_ideal(M, e, G->regular_array);
            for (uint p = 0; p < size_vertices(eM); p++)
            {
                edone[lefread_vertices(eM, p)] = true;
                fdone[lefread_vertices(eM, p)] = true;
            }
            p_vertices Me = compute_l_ideal(M, e, G->regular_array);
            for (uint p = 0; p < size_vertices(Me); p++)
            {
                edone[lefread_vertices(Me, p)] = true;
                fdone[lefread_vertices(Me, p)] = true;
            }

            // On commence par considérer le cas f = e: on doit vérifier (esete)^w =
            // (esete)^w ese et (esete)^w = ete(esete)^w pour tous s,t On calcule eMe
            p_vertices eMe = make_inter_sorted_vertices(eM, Me);

            // Boucles sur les éléments ese
            for (uint k = 0; k < size_vertices(eMe) - 1; k++)
            {
                uint ese = lefread_vertices(eMe, k);
                // Boucle sur les éléments ete
                for (uint l = k; l < size_vertices(eMe); l++)
                {
                    uint ete = lefread_vertices(eMe, l);
                    // Il suffit de considérer les éléments ete tels que ese J ete
                    if (G->JCL->numcl[ese] == G->JCL->numcl[ete])
                    {
                        uint esete = cayley_mult(M, ese, ete);
                        // Il suffit de considérer les cas où esete J ese et esete est
                        // idempotent
                        if (M->idem_array[esete] &&
                            G->JCL->numcl[ese] == G->JCL->numcl[esete])
                        {
                            if (esete != cayley_mult(M, esete, ese))
                            {
                                if (out)
                                {
                                    fprintf(out, "#### This semigroup does not satisfy Knast's "
                                        "equation.\n");
                                    fprintf(out,
                                        "#### For instance, it fails for e = f = s = t = ");
                                    cayley_print_name(M, e, out);
                                    fprintf(out, ", q = ");
                                    cayley_print_name(M, ese, out);
                                    fprintf(out, " and r = ");
                                    cayley_print_name(M, ete, out);
                                    fprintf(out, ".\n");
                                }
                                return false;
                            }
                            if (esete != cayley_mult(M, ete, esete))
                            {
                                if (out)
                                {
                                    fprintf(out, "#### This semigroup does not satisfy Knast's "
                                        "equation.\n");
                                    fprintf(out,
                                        "#### For instance, it fails for e = f = q = r = ");
                                    cayley_print_name(M, e, out);
                                    fprintf(out, ", s = ");
                                    cayley_print_name(M, ese, out);
                                    fprintf(out, " and t = ");
                                    cayley_print_name(M, ete, out);
                                    fprintf(out, ".\n");
                                }
                                return false;
                            }
                        }
                    }
                }
            }
            // Le cas e = f a été traité, on n'a plus besoin de eMe
            delete_vertices(eMe);

            // On passe aux idempotents f > e
            for (uint j = i + 1; j < size_vertices(M->idem_list); j++)
            {
                uint f = lefread_vertices(M->idem_list, j);
                if (!fdone[f])
                {
                    // fprintf(out, "f: ");
                    // cayley_print_name(M, f, out);
                    // fprintf(out, "\n");
                    // Calcul des ensembles fM et Mf (on ne garde que les éléments
                    // réguliers dans ces ensembles) On marque les idempotents f' dans fM
                    // et Mf: ils sont traités automatiquement en même temps que f
                    p_vertices fM = compute_r_ideal(M, f, G->regular_array);
                    for (uint p = 0; p < size_vertices(fM); p++)
                    {
                        fdone[lefread_vertices(fM, p)] = true;
                    }
                    p_vertices Mf = compute_l_ideal(M, f, G->regular_array);
                    for (uint p = 0; p < size_vertices(Mf); p++)
                    {
                        fdone[lefread_vertices(Mf, p)] = true;
                    }

                    // Calculs des ensembles eMf et fMe
                    p_vertices eMf = make_inter_sorted_vertices(eM, Mf);
                    p_vertices fMe = make_inter_sorted_vertices(fM, Me);
                    delete_vertices(fM);
                    delete_vertices(Mf);
                    // Boucle sur les éléments eqf
                    for (uint k = 0; k < size_vertices(eMf); k++)
                    {
                        uint eqf = lefread_vertices(eMf, k);
                        // fprintf(out, "q: ");
                        // cayley_print_name(M, eqf, out);
                        // fprintf(out, "\n");
                        // Boucle sur les éléments fre
                        for (uint l = 0; l < size_vertices(fMe); l++)
                        {
                            uint fre = lefread_vertices(fMe, l);
                            // fprintf(out, "r: ");
                            // cayley_print_name(M, fre, out);
                            // fprintf(out, "\n");
                            // Il suffit de considérer les cas où eqf J fre
                            if (G->JCL->numcl[eqf] == G->JCL->numcl[fre])
                            {
                                uint eqfre = cayley_mult(M, eqf, fre);
                                // Il suffit de considérer les cas où eqfre J eqf et eqfre est
                                // idempotent
                                if (M->idem_array[eqfre] &&
                                    G->JCL->numcl[eqf] == G->JCL->numcl[eqfre])
                                {
                                    // Boucle sur les éléments esf
                                    for (uint m = 0; m < size_vertices(eMf); m++)
                                    {
                                        uint esf = lefread_vertices(eMf, m);
                                        // fprintf(out, "s: ");
                                        // cayley_print_name(M, esf, out);
                                        // fprintf(out, "\n");

                                        // Boucle sur les éléments fte
                                        for (uint n = 0; n < size_vertices(fMe); n++)
                                        {
                                            uint fte = lefread_vertices(fMe, n);
                                            // fprintf(out, "t: ");
                                            // cayley_print_name(M, fte, out);
                                            // fprintf(out, "\n");
                                            // Il suffit de considérer les cas où eqf J fre
                                            if (G->JCL->numcl[esf] == G->JCL->numcl[fte])
                                            {
                                                uint esfte = cayley_mult(M, esf, fte);
                                                // fprintf(out, "esfte: ");
                                                // cayley_print_name(M, esfte, out);
                                                // fprintf(out, "\n");
                                                // Il suffit de considérer les cas où esfte J esf et
                                                // esfte est idempotent
                                                if (M->idem_array[esfte] &&
                                                    G->JCL->numcl[esf] == G->JCL->numcl[esfte])
                                                {
                                                    if (cayley_mult(M, eqfre, esfte) !=
                                                        cayley_mult_gen(M, 4, eqfre, eqf, fte,
                                                            esfte))
                                                    {
                                                        if (out)
                                                        {
                                                            fprintf(out,
                                                                "#### This semigroup does not "
                                                                "satisfy Knast's equation.\n");
                                                            fprintf(out,
                                                                "#### For instance, the equality "
                                                                "fails for e = ");
                                                            cayley_print_name(M, e, out);
                                                            fprintf(out, ", f = ");
                                                            cayley_print_name(M, f, out);
                                                            fprintf(out, ", q = ");
                                                            cayley_print_name(M, eqf, out);
                                                            fprintf(out, ", r = ");
                                                            cayley_print_name(M, fre, out);
                                                            fprintf(out, ", s = ");
                                                            cayley_print_name(M, esf, out);
                                                            fprintf(out, " and t = ");
                                                            cayley_print_name(M, fte, out);
                                                            fprintf(out, ".\n");
                                                        }
                                                        return false;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    delete_vertices(eMf);
                    delete_vertices(fMe);
                }
            }
            delete_vertices(eM);
            delete_vertices(Me);
        }
    }
    if (out)
    {
        fprintf(out, "#### This semigroup satisfies Knast's equation.\n");
    }
    return true;
}

// Knast on the MOD-kernel
bool is_qknast_mono(p_green_sub mker, FILE* out)
{
    p_cayley M = mker->original;
    p_green G = mker->originalrels;
    if (out) {
        fprintf(stdout, "#### Checking whether the strict MOD-kernel of the morphism satisfies Knast's equation:\n");
        fprintf(stdout, "     for all q,r,s,t ∊ S and all e,f ∊ E(S), we must have (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ.\n");

    }

    if (M->neutlet) {
        if (out) {
            fprintf(stdout, "#### Since a letter maps to the neutral element 1 has a nonempty antecedent, the equation holds\n");
            fprintf(stdout, "      if and only if the monoid is J-trivial.\n");
        }
        return is_jtrivial_mono(M, G, stdout);
    }

    if (out) {
        fprintf(out, "#### Displaying the MOD-kernel.\n");
        print_full_green_sub(mker, false, stdout);
    }


    // Tableau qui marque les éléments du MOD-kernel qui sont réguliers dans
    // celui-ci
    bool reginker[M->trans->size_graph];
    for (uint s = 0; s < M->trans->size_graph; s++)
    {
        reginker[s] =
            mker->mono_in_sub[s] && mker->rels->regular_array[mker->mono_to_sub[s]];
    }

    // Tableau qui mémorise les idempotents e déjà traités (il arrive que certains
    // soient traités simultanément)
    bool edone[M->trans->size_graph];
    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        edone[lefread_vertices(M->idem_list, i)] = false;
    }

    // Boucles sur tous les idempotents e
    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);

        // Si e n'a pas déjà été traité dans les calculs précédent
        if (!edone[e])
        {
            // Il faut maintenant considérer tous les idempotents f.
            // L'équation étant symmétrique, il suffit de considérer les f tels que e
            // <= f

            // Tableau qui mémorise les idempotents f déjà traités pour ce e (il
            // arrive que certains soient traités simultanément)
            bool fdone[M->trans->size_graph];
            for (uint j = i; j < size_vertices(M->idem_list); j++)
            {
                fdone[lefread_vertices(M->idem_list, j)] = false;
            }

            // Calcul des ensembles eM et Me (on ne garde que les éléments réguliers
            // dans ces ensembles) On marque les idempotents e' dans eM et Me: ils
            // sont traités automatiquement en même temps que e
            p_vertices eM = compute_r_ideal(M, e, reginker);
            for (uint p = 0; p < size_vertices(eM); p++)
            {
                edone[lefread_vertices(eM, p)] = true;
                fdone[lefread_vertices(eM, p)] = true;
            }
            p_vertices Me = compute_l_ideal(M, e, reginker);
            for (uint p = 0; p < size_vertices(Me); p++)
            {
                edone[lefread_vertices(Me, p)] = true;
                fdone[lefread_vertices(Me, p)] = true;
            }

            // On commence par considérer le cas f = e: on doit vérifier (esete)^w =
            // (esete)^w ese et (esete)^w = ete(esete)^w pour tous s,t On calcule eMe
            p_vertices eMe = make_inter_sorted_vertices(eM, Me);

            // Boucles sur les éléments ese
            for (uint k = 0; k < size_vertices(eMe) - 1; k++)
            {
                uint ese = lefread_vertices(eMe, k);
                // Boucle sur les éléments ete
                for (uint l = k; l < size_vertices(eMe); l++)
                {
                    uint ete = lefread_vertices(eMe, l);
                    // Il suffit de considérer les éléments ete tels que ese J ete (dans
                    // le MOD-kernel)
                    if (mker->rels->JCL->numcl[mker->mono_to_sub[ese]] ==
                        mker->rels->JCL->numcl[mker->mono_to_sub[ete]])
                    {
                        uint esete = cayley_mult(M, ese, ete);
                        // Il suffit de considérer les cas où esete J ese  (dans le
                        // MOD-kernel) et esete est idempotent
                        if (M->idem_array[esete] &&
                            mker->rels->JCL->numcl[mker->mono_to_sub[ese]] ==
                            mker->rels->JCL->numcl[mker->mono_to_sub[esete]])
                        {
                            if (esete != cayley_mult(M, esete, ese))
                            {
                                if (out)
                                {
                                    fprintf(out, "#### This semigroup does not satisfy Knast's "
                                        "equation.\n");
                                    fprintf(out,
                                        "#### For instance, it fails for e = f = s = t = ");
                                    cayley_print_name(M, e, out);
                                    fprintf(out, ", q = ");
                                    cayley_print_name(M, ese, out);
                                    fprintf(out, " and r = ");
                                    cayley_print_name(M, ete, out);
                                    fprintf(out, ".\n");
                                }
                                return false;
                            }
                            if (esete != cayley_mult(M, ete, esete))
                            {
                                if (out)
                                {
                                    fprintf(out, "#### This semigroup does not satisfy Knast's "
                                        "equation.\n");
                                    fprintf(out,
                                        "#### For instance, it fails for e = f = q = r = ");
                                    cayley_print_name(M, e, out);
                                    fprintf(out, ", s = ");
                                    cayley_print_name(M, ese, out);
                                    fprintf(out, " and t = ");
                                    cayley_print_name(M, ete, out);
                                    fprintf(out, ".\n");
                                }
                                return false;
                            }
                        }
                    }
                }
            }
            // Le cas e = f a été traité, on n'a plus besoin de eMe
            delete_vertices(eMe);

            // On passe aux idempotents f > e
            for (uint j = i + 1; j < size_vertices(M->idem_list); j++)
            {
                uint f = lefread_vertices(M->idem_list, j);
                if (!fdone[f])
                {
                    // Calcul des ensembles fM et Mf (on ne garde que les éléments
                    // réguliers du MOD-kernel dans ces ensembles) On marque les
                    // idempotents f' dans fM et Mf: ils sont traités automatiquement en
                    // même temps que f
                    p_vertices fM = compute_r_ideal(M, f, reginker);
                    for (uint p = 0; p < size_vertices(fM); p++)
                    {
                        fdone[lefread_vertices(fM, p)] = true;
                    }
                    p_vertices Mf = compute_l_ideal(M, f, reginker);
                    for (uint p = 0; p < size_vertices(Mf); p++)
                    {
                        fdone[lefread_vertices(Mf, p)] = true;
                    }

                    // Calculs des ensembles eMf et fMe
                    p_vertices eMf = make_inter_sorted_vertices(eM, Mf);
                    p_vertices fMe = make_inter_sorted_vertices(fM, Me);
                    delete_vertices(fM);
                    delete_vertices(Mf);
                    // Boucle sur les éléments eqf
                    for (uint k = 0; k < size_vertices(eMf); k++)
                    {
                        uint eqf = lefread_vertices(eMf, k);
                        // Boucle sur les éléments fre
                        for (uint l = 0; l < size_vertices(fMe); l++)
                        {
                            uint fre = lefread_vertices(fMe, l);
                            // Il suffit de considérer les cas où eqf J fre (dans le
                            // MOD-kernel)
                            if (mker->rels->JCL->numcl[mker->mono_to_sub[eqf]] ==
                                mker->rels->JCL->numcl[mker->mono_to_sub[fre]])
                            {
                                uint eqfre = cayley_mult(M, eqf, fre);
                                // Il suffit de considérer les cas où eqfre J eqf et eqfre est
                                // idempotent
                                if (M->idem_array[eqfre] &&
                                    mker->rels->JCL->numcl[mker->mono_to_sub[eqf]] ==
                                    mker->rels->JCL->numcl[mker->mono_to_sub[eqfre]])
                                {
                                    // Boucle sur les éléments esf
                                    for (uint m = 0; m < size_vertices(eMf); m++)
                                    {
                                        uint esf = lefread_vertices(eMf, m);

                                        // Boucle sur les éléments fte
                                        for (uint n = 0; n < size_vertices(fMe); n++)
                                        {
                                            uint fte = lefread_vertices(fMe, n);
                                            // Il suffit de considérer les cas où eqf J fre (dans le
                                            // MOD-kenrel)
                                            if (mker->rels->JCL->numcl[mker->mono_to_sub[esf]] ==
                                                mker->rels->JCL->numcl[mker->mono_to_sub[fte]])
                                            {
                                                uint esfte = cayley_mult(M, esf, fte);
                                                // Il suffit de considérer les cas où esfte J esf et
                                                // esfte est idempotent
                                                if (M->idem_array[esfte] &&
                                                    mker->rels->JCL->numcl[mker->mono_to_sub[esf]]
                                                    ==
                                                    mker->rels->JCL
                                                    ->numcl[mker->mono_to_sub[esfte]])
                                                {
                                                    if (cayley_mult(M, eqfre, esfte) !=
                                                        cayley_mult_gen(M, 4, eqfre, eqf, fte,
                                                            esfte))
                                                    {
                                                        if (out)
                                                        {
                                                            fprintf(out,
                                                                "#### This semigroup does not "
                                                                "satisfy Knast's equation.\n");
                                                            fprintf(out,
                                                                "#### For instance, the equality "
                                                                "fails for e = ");
                                                            cayley_print_name(M, e, out);
                                                            fprintf(out, ", f = ");
                                                            cayley_print_name(M, f, out);
                                                            fprintf(out, ", q = ");
                                                            cayley_print_name(M, eqf, out);
                                                            fprintf(out, ", r = ");
                                                            cayley_print_name(M, fre, out);
                                                            fprintf(out, ", s = ");
                                                            cayley_print_name(M, esf, out);
                                                            fprintf(out, " and t = ");
                                                            cayley_print_name(M, fte, out);
                                                            fprintf(out, ".\n");
                                                        }
                                                        return false;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    delete_vertices(eMf);
                    delete_vertices(fMe);
                }
            }
            delete_vertices(eM);
            delete_vertices(Me);
        }
    }
    if (out)
    {
        fprintf(out, "#### This semigroup satisfies Knast's equation.\n");
    }
    return true;
}

// Knast for at-sets
bool is_knast_at_mono(p_cayley M, p_green G, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking if the morphism satisfies the equation (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all\n");
        fprintf(out, "     q,r,s,t ∊ M and e, f ∊ E(M) such that {q,e,f}, {r,e,f}, {s,e,f} and {t,e,f} are AT-sets.\n");
    }

    // Boucles sur tous les idempotents e
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        // Il faut maintenant considérer tous les idempotents f.
        // L'équation étant symmétrique, il suffit de considérer les f tels que e <=
        // f

        // On commence par considérer le cas f = e: on doit vérifier (esete)^w =
        // (esete)^w ese et (esete)^w = ete(esete)^w

        // On calcule l'alphabet maximum de e
        bool* ealph = compute_maxalph_scc(M, G->RCL, e);

        // On calcule maintenant la restriction de eMe aux élément réguliers qui ont
        // un antécédent d'alphabet ealph
        p_vertices eM = compute_r_ideal_alph(M, e, ealph, G->regular_array);
        p_vertices Me = compute_l_ideal_alph(M, e, ealph, G->regular_array);
        // On calcule eMe
        p_vertices eMe = make_inter_sorted_vertices(eM, Me);

        delete_vertices(eM);
        delete_vertices(Me);

        // Boucles sur les éléments ese
        for (uint k = 0; k < size_vertices(eMe) - 1; k++)
        {
            // printf("toto:%d \n", size_vertices(eMe) - 1);
            uint ese = lefread_vertices(eMe, k);
            // Boucle sur les éléments ete
            for (uint l = k; l < size_vertices(eMe); l++)
            {
                // printf("toto1\n");
                uint ete = lefread_vertices(eMe, l);
                // Il suffit de considérer les éléments ete tels que ese J ete
                if (G->JCL->numcl[ese] == G->JCL->numcl[ete])
                {
                    uint esete = cayley_mult(M, ese, ete);
                    // Il suffit de considérer les cas où esete J ese et esete est
                    // idempotent
                    if (M->idem_array[esete] &&
                        G->JCL->numcl[ese] == G->JCL->numcl[esete])
                    {
                        if (esete != cayley_mult(M, esete, ese))
                        {
                            if (out)
                            {
                                fprintf(
                                    out,
                                    "#### This morphism does not satisfy (eqfre)ʷ(esfte)ʷ = "
                                    "(eqfre)ʷqft(esfte) for all q,r,s,t ∊ M and e,f ∊ E(M)\n");
                                fprintf(out, "     such that {q,e,f} {r,e,f}, {s,e,f} and "
                                    "{t,e,f} are AT-sets.\n");
                                fprintf(out,
                                    "#### For instance, it fails for e = f = s = t = ");
                                cayley_print_name(M, e, out);
                                fprintf(out, ", q = ");
                                cayley_print_name(M, ese, out);
                                fprintf(out, " and r = ");
                                cayley_print_name(M, ete, out);
                                fprintf(out, ".\n");
                                fprintf(out, "#### These elements form AT-sets since they all "
                                    "have an antecedent with the alphabet ");
                                print_booltab_alph(ealph, M->trans->size_alpha, out);
                            }
                            return false;
                        }
                        if (esete != cayley_mult(M, ete, esete))
                        {
                            if (out)
                            {
                                fprintf(
                                    out,
                                    "#### This morphism does not satisfy (eqfre)ʷ(esfte)ʷ = "
                                    "(eqfre)ʷqft(esfte) for all q,r,s,t ∊ M and e,f ∊ E(M)\n");
                                fprintf(out, "     such that {q,e,f} {r,e,f}, {s,e,f} and "
                                    "{t,e,f} are AT-sets.\n");
                                fprintf(out,
                                    "#### For instance, it fails for e = f = q = r = ");
                                cayley_print_name(M, e, out);
                                fprintf(out, ", s = ");
                                cayley_print_name(M, ese, out);
                                fprintf(out, " and t = ");
                                cayley_print_name(M, ete, out);
                                fprintf(out, ".\n");
                                fprintf(out, "#### These elements form AT-sets since they all "
                                    "have an antecedent with the alphabet ");
                                print_booltab_alph(ealph, M->trans->size_alpha, out);
                            }
                            return false;
                        }
                    }
                }
            }
        }
        // Le cas e = f a été traité, on n'a plus besoin de eMe
        free(ealph);
        delete_vertices(eMe);

        // On passe aux idempotents f > e
        for (uint j = i + 1; j < size_vertices(M->idem_list); j++)
        {
            uint f = lefread_vertices(M->idem_list, j);

            // On calcule l'alphabet commun maximal de e et f
            bool* efalph = compute_maxalph_com_scc(M, G->RCL, e, f);
            if (efalph)
            {
                // On calcule maintenant la restriction de eMf et fMe aux élément
                // réguliers qui ont un antécédent d'alphabet efalph
                eM = compute_r_ideal_alph(M, e, efalph, G->regular_array);
                Me = compute_l_ideal_alph(M, e, efalph, G->regular_array);
                p_vertices fM = compute_r_ideal_alph(M, f, efalph, G->regular_array);
                p_vertices Mf = compute_l_ideal_alph(M, f, efalph, G->regular_array);
                p_vertices eMf = make_inter_sorted_vertices(eM, Mf);
                p_vertices fMe = make_inter_sorted_vertices(fM, Me);
                delete_vertices(eM);
                delete_vertices(Me);
                delete_vertices(fM);
                delete_vertices(Mf);

                // fprintf(out, "e: ");
                // cayley_print_name(M, e, out);
                // fprintf(out, " f: ");
                // cayley_print_name(M, f, out);
                // fprintf(out, "\n");
                // Boucle sur les éléments eqf
                for (uint k = 0; k < size_vertices(eMf); k++)
                {
                    uint eqf = lefread_vertices(eMf, k);
                    // fprintf(out, " q: ");
                    // cayley_print_name(M, eqf, out);
                    // fprintf(out, "\n");
                    // Boucle sur les éléments fre
                    for (uint l = 0; l < size_vertices(fMe); l++)
                    {
                        uint fre = lefread_vertices(fMe, l);
                        // fprintf(out, " r: ");
                        // cayley_print_name(M, fre, out);
                        // fprintf(out, "\n");
                        // Il suffit de considérer les cas où eqf J fre
                        if (G->JCL->numcl[eqf] == G->JCL->numcl[fre])
                        {
                            uint eqfre = cayley_mult(M, eqf, fre);
                            // fprintf(out, " eqfre: ");
                            // cayley_print_name(M, eqfre, out);
                            // fprintf(out, "\n");
                            // Il suffit de considérer les cas où eqfre J eqf et eqfre est
                            // idempotent
                            if (M->idem_array[eqfre] &&
                                G->JCL->numcl[eqf] == G->JCL->numcl[eqfre])
                            {
                                // Boucle sur les éléments esf
                                for (uint m = 0; m < size_vertices(eMf); m++)
                                {
                                    uint esf = lefread_vertices(eMf, m);
                                    // fprintf(out, " s: ");
                                    // cayley_print_name(M, esf, out);
                                    // fprintf(out, "\n");

                                    // Boucle sur les éléments fte
                                    for (uint n = 0; n < size_vertices(fMe); n++)
                                    {
                                        uint fte = lefread_vertices(fMe, n);
                                        // fprintf(out, " t: ");
                                        // cayley_print_name(M, fte, out);
                                        // fprintf(out, "\n");
                                        // Il suffit de considérer les cas où eqf J fre
                                        if (G->JCL->numcl[esf] == G->JCL->numcl[fte])
                                        {
                                            uint esfte = cayley_mult(M, esf, fte);
                                            // Il suffit de considérer les cas où esfte J esf et esfte
                                            // est idempotent
                                            if (M->idem_array[esfte] &&
                                                G->JCL->numcl[esf] == G->JCL->numcl[esfte])
                                            {
                                                if (cayley_mult(M, eqfre, esfte) !=
                                                    cayley_mult_gen(M, 4, eqfre, eqf, fte, esfte))
                                                {
                                                    if (out)
                                                    {
                                                        fprintf(out,
                                                            "#### This morphism does not satisfy "
                                                            "(eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte) for "
                                                            "all q,r,s,t ∊ M and e,f ∊ E(M)\n");
                                                        fprintf(out,
                                                            "     such that {q,e,f} {r,e,f}, "
                                                            "{s,e,f} and {t,e,f} are AT-sets.\n");
                                                        fprintf(out,
                                                            "#### For instance, it fails for e = ");
                                                        cayley_print_name(M, e, out);
                                                        fprintf(out, ", f = ");
                                                        cayley_print_name(M, f, out);
                                                        fprintf(out, ", q = ");
                                                        cayley_print_name(M, eqf, out);
                                                        fprintf(out, ", r = ");
                                                        cayley_print_name(M, fre, out);
                                                        fprintf(out, ", s = ");
                                                        cayley_print_name(M, esf, out);
                                                        fprintf(out, " and t = ");
                                                        cayley_print_name(M, fte, out);
                                                        fprintf(out, ".\n");
                                                        fprintf(
                                                            out,
                                                            "#### These elements form AT-sets since they "
                                                            "all have an antecedent with the alphabet ");
                                                        print_booltab_alph(efalph,
                                                            M->trans->size_alpha,
                                                            out);
                                                    }
                                                    return false;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                free(efalph);
                delete_vertices(eMf);
                delete_vertices(fMe);
            }
        }
    }
    if (out)
    {
        fprintf(out, "#### This morphism does satisfies (eqfre)ʷ(esfte)ʷ = "
            "(eqfre)ʷqft(esfte) for all q,r,s,t ∊ M and e,f ∊ E(M)\n");
        fprintf(
            out,
            "     such that {q,e,f} {r,e,f}, {s,e,f} and {t,e,f} are AT-sets.\n");
    }
    return true;
}


/********************/
/* Pol - Separation */
/********************/

// Pol(ST) of A from B
bool decid_polst_sep(p_nfa A, p_nfa B, bool details, FILE* out)
{
    // Gestion des cas triviaux
    if (isempty_vertices(A->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_vertices(B->finals))
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
    p_nfa BEPS = nfa_add_eps(B);
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
    p_nfa INTER = nfa_intersect(A, BEPS, true);
    delete_nfa(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 2: Intersecting the resulting NFA with that of "
            "the first input language.\n");
        view_nfa(INTER);
    }
    if (isempty_vertices(INTER->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        delete_nfa(INTER);
        return true;
    }
    else
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        delete_nfa(INTER);
        return false;
    }
}

// Pol(MOD)-separation of A from B
bool decid_polmod_sep(p_nfa A, p_nfa B, bool details, FILE* out)
{
    // Gestion des cas triviaux
    if (isempty_vertices(A->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_vertices(B->finals))
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
    p_nfa BPROJ = nfa_proj_unary(B);
    p_parti SCCS = nfa_inv_ext(BPROJ);
    p_parti FOLD = nfa_stal_fold(BPROJ, SCCS);
    delete_parti(SCCS);
    p_nfa BEPS = nfa_merge_states(B, FOLD);
    p_nfa BPROJEPS = nfa_dyck_ext(BPROJ, FOLD);
    delete_parti(FOLD);
    delete_nfa(BPROJ);
    BEPS->etrans = BPROJEPS->etrans;
    BPROJEPS->etrans = NULL;
    delete_nfa(BPROJEPS);
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
    p_nfa INTER = nfa_intersect(A, BEPS, true);
    delete_nfa(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 2: Intersecting the resulting NFA with that of "
            "the first input language.\n");
        view_nfa(INTER);
    }
    if (isempty_vertices(INTER->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        delete_nfa(INTER);
        return true;
    }
    else
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        delete_nfa(INTER);
        return false;
    }
}

// Pol(GR)-separation of A from B
bool decid_polgr_sep(p_nfa A, p_nfa B, bool details, FILE* out)
{
    // Gestion des cas triviaux
    if (isempty_vertices(A->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_vertices(B->finals))
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
    p_parti SCCS = nfa_inv_ext(B);
    p_parti FOLD = nfa_stal_fold(B, SCCS);
    delete_parti(SCCS);
    p_nfa BEPS = nfa_dyck_ext(B, FOLD);
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
    p_nfa INTER = nfa_intersect(A, BEPS, true);
    delete_nfa(BEPS);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 2: Intersecting the resulting NFA with that of "
            "the first input language.\n");
        view_nfa(INTER);
    }
    if (isempty_vertices(INTER->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        delete_nfa(INTER);
        return true;
    }
    else
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        delete_nfa(INTER);
        return false;
    }
}

// Calcul des BPol(G)-orbites à partir du G-kernel (G = GR ou G = MOD)
p_orbits get_bpgorbs(p_cayley M, p_green G, p_green_sub ker)
{
    // Calcul de la table de multiplication de M (nécessaire pour ne pas recalculer
    // plein de fois la même chose).
    compute_mult(M);

    // Initialisation de la structure qui contiendra les orbites
    p_orbits res = init_orbits(init_submono(M, G));
    res->nb_idems = size_vertices(M->idem_list);
    res->oneonly = false;
    MALLOC(res->orbits, res->nb_idems);

    // On créé un tableau permettant de récupérer rapidemment le numéro d'un idempotent
    uint* num_idem;
    MALLOC(num_idem, M->trans->size_graph);
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        num_idem[lefread_vertices(M->idem_list, i)] = i;
    }

    // Calcul des orbites: on fait une boucle sur toutes les R-classes
    for (uint c = 0; c < G->RCL->size_par; c++)
    {
        // On va calculer les BPol(G)-orbites de tous les idempotents dans la R-classe c
        // On va d'abord tester si il existe bien un idempotent dans c: les éléments dans
        // c sont-ils réguliers ?
        if (G->regular_array[lefread_vertices(G->RCL->cl[c], 0)])
        {
            // Pour calculer les BPol(G)-orbites, on a besoin des Pol(G)-paires de la forme (q,r) telles
            // q R c et r est dans cM. On va les calculer avec un parcours en largeur du graphe de Cayley

            // On calcule l'ensemble cM
            p_vertices cM = compute_r_ideal(M, lefread_vertices(G->RCL->cl[c], 0), NULL);
            uint* inv_cM;

            // Un tableau qui associe à chaque élément dans cM son numéro dans la liste ci-dessus
            MALLOC(inv_cM, M->trans->size_graph);
            for (uint j = 0; j < size_vertices(cM); j++)
            {
                inv_cM[lefread_vertices(cM, j)] = j;
            }

            // Un tableau qui associe à chaque élément dans c son numéro dans la R-classe
            uint* inv_c;
            MALLOC(inv_c, M->trans->size_graph);
            for (uint j = 0; j < size_vertices(G->RCL->cl[c]); j++)
            {
                inv_c[lefread_vertices(G->RCL->cl[c], j)] = j;
            }

            // On créé un tableau (de taille cM x c) pour mémoriser les paires visitées dans le parcours en largeur
            bool** visited;
            MALLOC(visited, size_vertices(cM));
            for (uint q = 0; q < size_vertices(cM); q++)
            {
                MALLOC(visited[q], size_vertices(G->RCL->cl[c]));
                for (uint r = 0; r < size_vertices(G->RCL->cl[c]); r++)
                {
                    visited[q][r] = false;
                }
            }

            // On calcule les éléments du G-kernel qui J-équivalents à c
            p_vertices jc_inker = create_vertices();
            for (uint s = 0; s < ker->size; s++)
            {
                if (G->JCL->numcl[lefread_vertices(G->RCL->cl[c],
                    0)] == G->JCL->numcl[ker->sub_to_mono[s]])
                {
                    rigins_vertices(s, jc_inker);
                }
            }

            // Piles pour enregistrer les nouvelles paires à traiter
            p_vertices topstack = create_vertices();
            p_vertices botstack = create_vertices();

            // On part d'une paire (q,q) avec q dans c (on prend q le premier élément dans c)
            visited[inv_cM[lefread_vertices(G->RCL->cl[c], 0)]][0] = true;
            rigins_vertices(lefread_vertices(G->RCL->cl[c], 0), topstack);
            rigins_vertices(lefread_vertices(G->RCL->cl[c], 0), botstack);
            // Parcours en largeur
            while (!isempty_vertices(topstack)) {
                uint q = rigpull_vertices(topstack);
                uint r = rigpull_vertices(botstack);

                // Pour toute lettre a on prend la paire (qa,ra)
                for (uint a = 0; a < M->trans->size_alpha; a++)
                {
                    // On ne s'intéresse à cette paire que si ra R e et qu'on ne l'a pas déjà visitée
                    if (G->RCL->numcl[M->trans->edges[r][a]] == c &&
                        !visited[inv_cM[M->trans->edges[q][a]]][inv_c[M->trans->edges[r][a]]])
                    {
                        visited[inv_cM[M->trans->edges[q][a]]][inv_c[M->trans->edges[r][a]]] = true;
                        rigins_vertices(M->trans->edges[q][a], topstack);
                        rigins_vertices(M->trans->edges[r][a], botstack);
                    }
                }

                // Pour tout élement s du kernel tel que s J e on prend la paire (q,rs)
                for (uint j = 0; j < size_vertices(jc_inker); j++)
                {
                    uint rs = cayley_mult(M, r, ker->sub_to_mono[lefread_vertices(jc_inker, j)]);

                    // On ne s'intéresse à cette paire que si rs R e et qu'on ne l'a pas déjà visitée
                    if (G->RCL->numcl[rs] == c && !visited[inv_cM[q]][inv_c[rs]])
                    {
                        visited[inv_cM[q]][inv_c[rs]] = true;
                        rigins_vertices(q, topstack);
                        rigins_vertices(rs, botstack);
                    }
                }
            }
            delete_vertices(jc_inker);
            delete_vertices(topstack);
            delete_vertices(botstack);

            // On peut maintenant calculer les orbites des idempotents dans c
            for (uint i = 0; i < size_vertices(G->RCL->cl[c]); i++)
            {
                uint e = lefread_vertices(G->RCL->cl[c], i);
                // Si e est un idempotent dans c
                if (M->idem_array[e])
                {
                    // On prend le numéro de e
                    uint j = num_idem[e];

                    // On initialise l'orbite de e
                    res->orbits[j] = init_submono(M, G);
                    // MALLOC(res->orbits[j], 1);
                    res->orbits[j]->size = 0; // On calculera la taille plus tard
                    MALLOC(res->orbits[j]->mono_in_sub, M->trans->size_graph);
                    MALLOC(res->orbits[j]->mono_to_sub, M->trans->size_graph);
                    for (uint s = 0; s < M->trans->size_graph; s++)
                    {
                        res->orbits[j]->mono_in_sub[s] = false;
                    }

                    // Calcul de l'orbite de e
                    // si (e,ese) est une co-Pol(G)-paire, alors ese est dans la BPol(G)-orbite de e
                    p_vertices Me = compute_l_ideal(M, e, NULL);
                    p_vertices eMe = make_inter_sorted_vertices(cM, Me);
                    delete_vertices(Me);

                    for (uint k = 0; k < size_vertices(eMe); k++)
                    {
                        uint ese = lefread_vertices(eMe, k);

                        if (visited[inv_cM[ese]][inv_c[e]])
                        {
                            res->orbits[j]->mono_in_sub[ese] = true;
                            res->orbits[j]->mono_to_sub[ese] = res->orbits[j]->size;
                            res->orbits[j]->size++;
                        }
                    }

                    delete_vertices(eMe);

                    MALLOC(res->orbits[j]->sub_to_mono, res->orbits[j]->size);
                    uint n = 0;
                    for (uint s = 0; s < M->trans->size_graph; s++)
                    {
                        if (res->orbits[j]->mono_in_sub[s])
                        {
                            res->orbits[j]->sub_to_mono[n] = s;
                            n++;
                        }
                    }
                    res->orbits[j]->neut = res->orbits[j]->mono_to_sub[e];
                }
            }

            // Libérations
            for (uint q = 0; q < size_vertices(cM); q++)
            {
                free(visited[q]);
            }
            free(inv_cM);
            free(inv_c);

            delete_vertices(cM);
        }
    }
    free(num_idem);

    // Calcul des idempotents dans chaque orbite

    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        MALLOC(res->orbits[i]->idem_array, res->orbits[i]->size);
        for (uint j = 0; j < res->orbits[i]->size; j++)
        {
            res->orbits[i]->idem_array[j] = false;
        }
        res->orbits[i]->idem_list = create_vertices();
        for (uint j = 0; j < res->orbits[i]->size; j++)
        {
            uint e = res->orbits[i]->sub_to_mono[j];
            if (M->idem_array[e])
            {
                res->orbits[i]->idem_array[j] = true;
                rigins_vertices(j, res->orbits[i]->idem_list);
            }
        }
    }

    // Calcul des relations de Green des orbites
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        // Calcul des R,L,J-classes
        jlr_green_compute_sub(M, res->orbits[i]);

        // Calcul des H-classes
        h_green_compute(res->orbits[i]->rels);

        // Calcul de la groupes et des J- classes régulières
        gr_green_compute(res->orbits[i]->idem_list, res->orbits[i]->rels);
    }

    return res;
}

// Calcul des BPol(G⁺)-orbites à partir des G⁺-orbites
// (on ajoute aussi le G-kernel pour le cas où 1 a un antécédent non-vide:
// il suffit alors de calculer les BPol(G)-orbites).
p_orbits get_bpgporbs(p_cayley M, p_green G, p_orbits orbs)
{
    // Calcul de la table de multiplication de M (nécessaire pour ne pas recalculer
    // plein de fois la même chose).
    compute_mult(M);

    // Initialisation de la structure qui contiendra les orbites
    p_orbits res = init_orbits(init_submono(M, G));
    res->nb_idems = size_vertices(M->idem_list);
    res->oneonly = false;
    MALLOC(res->orbits, res->nb_idems);

    // On créé un tableau permettant de récupérer rapidemment le numéro d'un idempotent
    uint* num_idem;
    MALLOC(num_idem, M->trans->size_graph);
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        num_idem[lefread_vertices(M->idem_list, i)] = i;
    }

    // Calcul des orbites: on fait une boucle sur toutes les R-classes
    for (uint c = 0; c < G->RCL->size_par; c++)
    {
        // printf("yeah?\n");

        // On va calculer les BPol(G)-orbites de tous les idempotents dans la R-classe c
        // On va d'abord tester si il existe bien un idempotent dans c: les éléments dans
        // c sont-ils réguliers ?
        if (G->regular_array[lefread_vertices(G->RCL->cl[c], 0)])
        {
            // Pour calculer les BPol(G)-orbites, on a besoin des Pol(G)-paires de la forme (q,r) telles
            // q R c et r est dans cM. On va les calculer avec un parcours en largeur du graphe de Cayley

            // On calcule l'ensemble cM
            p_vertices cM = compute_r_ideal(M, lefread_vertices(G->RCL->cl[c], 0), NULL);
            uint* inv_cM;

            // Un tableau qui associe à chaque élément dans cM son numéro dans la liste ci-dessus
            MALLOC(inv_cM, M->trans->size_graph);
            for (uint j = 0; j < size_vertices(cM); j++)
            {
                inv_cM[lefread_vertices(cM, j)] = j;
            }

            // Un tableau qui associe à chaque élément dans c son numéro dans la R-classe
            uint* inv_c;
            MALLOC(inv_c, M->trans->size_graph);
            for (uint j = 0; j < size_vertices(G->RCL->cl[c]); j++)
            {
                inv_c[lefread_vertices(G->RCL->cl[c], j)] = j;
            }

            // On créé un tableau (de taille cM x c) pour mémoriser les paires visitées dans le parcours en largeur
            bool** visited;
            MALLOC(visited, size_vertices(cM));
            for (uint q = 0; q < size_vertices(cM); q++)
            {
                MALLOC(visited[q], size_vertices(G->RCL->cl[c]));
                for (uint r = 0; r < size_vertices(G->RCL->cl[c]); r++)
                {
                    visited[q][r] = false;
                }
            }

            // Pour chaque idempotent f, on calcule les éléments de la GP-orbite de f qui sont J-équivalents à c
            p_vertices* jc_inorb;
            MALLOC(jc_inorb, size_vertices(M->idem_list));
            for (uint j = 0; j < size_vertices(M->idem_list); j++)
            {
                jc_inorb[j] = create_vertices();
                for (uint s = 0; s < orbs->orbits[j]->size; s++)
                {
                    if (G->JCL->numcl[lefread_vertices(G->RCL->cl[c],
                        0)] ==
                        G->JCL->numcl[orbs->orbits[j]->sub_to_mono[s]])
                    {
                        rigins_vertices(s, jc_inorb[j]);
                    }
                }
            }

            // Piles pour enregistrer les nouvelles paires à traiter
            p_vertices topstack = create_vertices();
            p_vertices botstack = create_vertices();

            // On part d'une paire (q,q) avec q dans c (on prend q le premier élément dans c)
            visited[inv_cM[lefread_vertices(G->RCL->cl[c], 0)]][0] = true;
            rigins_vertices(lefread_vertices(G->RCL->cl[c], 0), topstack);
            rigins_vertices(lefread_vertices(G->RCL->cl[c], 0), botstack);
            // Parcours en largeur
            while (!isempty_vertices(topstack)) {
                uint q = rigpull_vertices(topstack);
                uint r = rigpull_vertices(botstack);

                // Pour toute lettre a on prend la paire (qa,ra)
                for (uint a = 0; a < M->trans->size_alpha; a++)
                {
                    // On ne s'intéresse à cette paire que si ra R e et qu'on ne l'a pas déjà visitée
                    if (G->RCL->numcl[M->trans->edges[r][a]] == c &&
                        !visited[inv_cM[M->trans->edges[q][a]]][inv_c[M->trans->edges[r][a]]])
                    {
                        visited[inv_cM[M->trans->edges[q][a]]][inv_c[M->trans->edges[r][a]]] = true;
                        rigins_vertices(M->trans->edges[q][a], topstack);
                        rigins_vertices(M->trans->edges[r][a], botstack);
                    }
                }

                // Pour chaque idempotent f
                for (uint j = 0; j < size_vertices(M->idem_list); j++)
                {
                    // Pour chaque élément s dans l'orbite G⁺ de cet idempotent f tel que s J c on prend la paire
                    // (qf,rs)
                    for (uint k = 0; k < size_vertices(jc_inorb[j]); k++)
                    {
                        uint qf = cayley_mult(M, q, lefread_vertices(M->idem_list, j));
                        uint rs = cayley_mult(M, r,
                            orbs->orbits[j]->sub_to_mono[lefread_vertices(
                                jc_inorb[j], k)]);
                        if (G->RCL->numcl[rs] == c && !visited[inv_cM[qf]][inv_c[rs]])
                        {
                            visited[inv_cM[qf]][inv_c[rs]] = true;
                            rigins_vertices(qf, topstack);
                            rigins_vertices(rs, botstack);
                        }
                    }
                }
            }

            for (uint j = 0; j < size_vertices(M->idem_list); j++)
            {
                delete_vertices(jc_inorb[j]);
            }
            free(jc_inorb);
            delete_vertices(topstack);
            delete_vertices(botstack);

            // On peut maintenant calculer les orbites des idempotents dans c
            for (uint i = 0; i < size_vertices(G->RCL->cl[c]); i++)
            {
                uint e = lefread_vertices(G->RCL->cl[c], i);
                // Si e est un idempotent dans c
                if (M->idem_array[e])
                {
                    // On prend le numéro de e
                    uint j = num_idem[e];

                    // On initialise l'orbite de e
                    res->orbits[j] = init_submono(M, G);
                    // MALLOC(res->orbits[j], 1);
                    res->orbits[j]->size = 0; // On calculera la taille plus tard
                    MALLOC(res->orbits[j]->mono_in_sub, M->trans->size_graph);
                    MALLOC(res->orbits[j]->mono_to_sub, M->trans->size_graph);
                    for (uint s = 0; s < M->trans->size_graph; s++)
                    {
                        res->orbits[j]->mono_in_sub[s] = false;
                    }

                    // Calcul de l'orbite de e
                    // si (e,ese) est une co-Pol(G)-paire, alors ese est dans la BPol(G)-orbite de e
                    p_vertices Me = compute_l_ideal(M, e, NULL);
                    p_vertices eMe = make_inter_sorted_vertices(cM, Me);
                    delete_vertices(Me);

                    for (uint k = 0; k < size_vertices(eMe); k++)
                    {
                        uint ese = lefread_vertices(eMe, k);

                        if (visited[inv_cM[ese]][inv_c[e]])
                        {
                            res->orbits[j]->mono_in_sub[ese] = true;
                            res->orbits[j]->mono_to_sub[ese] = res->orbits[j]->size;
                            res->orbits[j]->size++;
                        }
                    }

                    delete_vertices(eMe);

                    MALLOC(res->orbits[j]->sub_to_mono, res->orbits[j]->size);
                    uint n = 0;
                    for (uint s = 0; s < M->trans->size_graph; s++)
                    {
                        if (res->orbits[j]->mono_in_sub[s])
                        {
                            res->orbits[j]->sub_to_mono[n] = s;
                            n++;
                        }
                    }
                    res->orbits[j]->neut = res->orbits[j]->mono_to_sub[e];
                }
            }

            // Libérations
            for (uint q = 0; q < size_vertices(cM); q++)
            {
                free(visited[q]);
            }
            free(inv_cM);
            free(inv_c);

            delete_vertices(cM);
        }
    }
    free(num_idem);

    // Calcul des idempotents dans chaque orbite

    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        MALLOC(res->orbits[i]->idem_array, res->orbits[i]->size);
        for (uint j = 0; j < res->orbits[i]->size; j++)
        {
            res->orbits[i]->idem_array[j] = false;
        }
        res->orbits[i]->idem_list = create_vertices();
        for (uint j = 0; j < res->orbits[i]->size; j++)
        {
            uint e = res->orbits[i]->sub_to_mono[j];
            if (M->idem_array[e])
            {
                res->orbits[i]->idem_array[j] = true;
                rigins_vertices(j, res->orbits[i]->idem_list);
            }
        }
    }

    // Calcul des relations de Green des orbites
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        // Calcul des R,L,J-classes
        jlr_green_compute_sub(M, res->orbits[i]);

        // Calcul des H-classes
        h_green_compute(res->orbits[i]->rels);

        // Calcul de la groupes et des J- classes régulières
        gr_green_compute(res->orbits[i]->idem_list, res->orbits[i]->rels);
    }

    return res;
}


/***********************/
/* BPol(G) - Separation */
/***********************/

// // BPol(GR)-separation of A from B (bugged TODO)
// bool decid_bpolgr_sep(p_nfa A, p_nfa B, bool details, FILE* out)
// {
//     // Gestion des cas triviaux
//     if (isempty_vertices(A->finals))
//     {
//         if (out && details)
//         {
//             fprintf(out, "#### The first input language is empty.\n");
//         }
//         return true;
//     }
//     if (isempty_vertices(B->finals))
//     {
//         if (out && details)
//         {
//             fprintf(out, "#### The second input language is empty.\n");
//         }
//         return true;
//     }
//     if (A == B)
//     {
//         if (out && details)
//         {
//             fprintf(out,
//                 "#### These two inputs refer to the same nonempty language.\n");
//         }
//         return false;
//     }

//     // Calcul du NFA intersecté de départ
//     p_parti SCCSA = nfa_inv_ext(A);
//     delete_parti(SCCSA);
//     nfa_dyck_ext(A, NULL);
//     nfa_remove_inv(A);
//     p_parti SCCSB = nfa_inv_ext(B);
//     delete_parti(SCCSB);
//     nfa_dyck_ext(B, NULL);
//     nfa_remove_inv(B);
//     p_nfa INTER = nfa_intersect(A, B, true);
//     delete_graph(A->etrans);
//     delete_graph(B->etrans);
//     A->etrans = NULL;
//     B->etrans = NULL;

//     // Début du point fixe
//     bool modified = true;
//     while (modified) {
//         // On n'a pas encore fait de modification
//         modified = false;

//         // Calcul des deux intersections
//         p_nfa TRIPA = nfa_intersect(A, INTER, true);
//         p_nfa TRIPB = nfa_intersect(B, INTER, true);

//         // Tableaux qui stockeront les numéros des états intéressants dans TRIPA,
//         // TRIPB triplets (qa,qa,qb) dans TRIPA ppour goala et (qb,qa,qb) dans TRIPB
//         // pou goalb
//         uint goala[INTER->etrans->size];
//         uint goalb[INTER->etrans->size];

//         // Initialisation, on met un numéro fictif pour marquer les triplets qui
//         // n'étaient pas accessibles dans la construction
//         for (uint p = 0; p < INTER->etrans->size; p++)
//         {
//             goala[p] = TRIPA->trans->size_graph;
//             goalb[p] = TRIPB->trans->size_graph;
//         }

//         // On remplit maintenant goala et goalb
//         for (uint t = 0; t < TRIPA->trans->size_graph; t++)
//         {
//             // Récupération du nom de l'état t dans TRIPA
//             // Numéro d'état provenant de A
//             uint qa = ((uint**)TRIPA->names)[t][0];
//             // Numéro d'état provenant de INTER
//             uint qi = ((uint**)TRIPA->names)[t][1];

//             // Récupération du nom de l'état qi dans INTER
//             // Numéro d'état provenant de A
//             uint pia = ((uint**)INTER->names)[qi][0];
//             if (pia == qa)
//             {
//                 goala[qi] = t;
//             }
//         }

//         for (uint t = 0; t < TRIPB->trans->size_graph; t++)
//         {
//             // Récupération du nom de l'état t dans TRIPA
//             // Numéro d'état provenant de B
//             uint qb = ((uint**)TRIPB->names)[t][0];
//             // Numéro d'état provenant de INTER
//             uint qi = ((uint**)TRIPB->names)[t][1];

//             // Récupération du nom de l'état p dans INTER
//             // Numéro d'état provenant de B
//             uint qib = ((uint**)INTER->names)[qi][1];
//             if (qib == qb)
//             {
//                 goalb[qi] = t;
//             }
//         }

//         // Calcul de GT-separation dans TRIP A et TRIP B

//         SCCSA = nfa_inv_ext(TRIPA);
//         p_parti FOLDA = nfa_stal_fold(TRIPA, SCCSA);
//         delete_parti(SCCSA);
//         p_nfa ETRIPA = nfa_dyck_ext(TRIPA, FOLDA);
//         nfa_remove_inv(TRIPA);
//         delete_parti(FOLDA);
//         nfa_remove_inv(ETRIPA);

//         SCCSB = nfa_inv_ext(TRIPB);
//         p_parti FOLDB = nfa_stal_fold(TRIPB, SCCSB);
//         delete_parti(SCCSB);
//         p_nfa ETRIPB = nfa_dyck_ext(TRIPB, FOLDB);
//         nfa_remove_inv(TRIPB);
//         delete_parti(FOLDB);
//         nfa_remove_inv(ETRIPB);

//         delete_nfa(TRIPA); // TRIP A et TRIPB ne sont plus nécessaires.
//         delete_nfa(TRIPB);

//         // Calcul des nouvelles paires non-BPOL(G)-sep dans INTER
//         for (uint q = 0; q < INTER->etrans->size; q++)
//         {
//             uint tempsize = size_vertices(INTER->etrans->edges[q]);
//             if (tempsize != 0)
//             {
//                 uint tqa = goala[q];
//                 uint tqb = goalb[q];
//                 if (tqa != TRIPA->trans->size_graph &&
//                     tqb != TRIPB->trans->size_graph)
//                 {
//                     for (uint i = 0; i < tempsize; i++)
//                     {
//                         uint r = lefpull_vertices(INTER->etrans->edges[q]);
//                         uint tra = goala[r];
//                         uint trb = goalb[r];
//                         if (tra != TRIPA->trans->size_graph &&
//                             trb != TRIPB->trans->size_graph &&
//                             mem_vertices_sorted(tra, ETRIPA->etrans->edges[tqa]) &&
//                             mem_vertices_sorted(trb, ETRIPB->etrans->edges[tqb]))
//                         {
//                             rigins_vertices(r, INTER->etrans->edges[q]);
//                         }
//                         else
//                         {
//                             modified = true;
//                         }
//                     }
//                 }
//                 else
//                 {
//                     makeempty_vertices(INTER->etrans->edges[q]);
//                     modified = true;
//                 }
//             }
//         }
//         delete_nfa(ETRIPA);
//         delete_nfa(ETRIPB);
//     }

//     nfa_trim(INTER);
//     bool result = true;
//     if (nfa_is_empty(INTER))
//     {
//         result = false;
//     }
//     delete_nfa(INTER);

//     return result;
// }

// bool nfa_bpolg_separ(p_nfa A, p_nfa B, p_graph (*gext)(p_nfa))
// {

//// Simplification de A et B (éliminations des transitions epsilon et
// élagage) nfa_elimeps(A); nfa_elimeps(B);

//// Ajout des transitions epsilon dans B pour les paires d'états
// non-séparable de epsilon A->etrans = (*gext)(A); B->etrans = (*gext)(B);

// delete_lgraph(A->itrans); // Les transitions inverses ne servent à rien
// une fois le calcul fait. delete_lgraph(B->itrans); A->itrans = NULL;
// B->itrans = NULL;

//// Intersection de A et B et de l'association entre les anciens états et
// les nouveaux p_nfa INTER = nfa_intersect(A, B, true); nfa_trim(INTER); //
// Suppression des états non-coaccessibles delete_graph(A->etrans); // Les
// transitions epsilon dans A et B ne sont maintenant plus utiles
// delete_graph(B->etrans);
// A->etrans = NULL;
// B->etrans = NULL;

// bool modified = true;
// while (modified)
// {
//// On n'a pas encore fait de modification
// modified = false;

//// Calcul des deux intersections
// p_nfa TRIPA = nfa_intersect(A, INTER, true);
// p_nfa TRIPB = nfa_intersect(B, INTER, true);

//// Tableaux qui stockeront les numéros des triplets intéressants dans
// TRIPA, TRIPB
//// (qa,qa,qb) dans TRIPA et (qb,qa,qb) dans TRIPB
// uint goala[INTER->etrans->size];
// uint goalb[INTER->etrans->size];

//// Initialisation, on met un numéro fictif pour détecter les triplets
// qui n'étaient pas accessibles dans la construction for (uint p = 0; p
// < INTER->etrans->size; p++)
// {
// goala[p] = TRIPA->trans->size_graph + 1;
// goalb[p] = TRIPB->trans->size_graph + 1;
// }

//// On remplit maintenant goala et goalb
// for (uint t = 0; t < TRIPA->trans->size_graph; t++)
// {
//// Récupération du nom de l'état t dans TRIPA
//// Numéro d'état provenant de A
// uint qa = ((uint *)TRIPA->names[t])[0];
//// Numéro d'état provenant de INTER
// uint pi = ((uint *)TRIPA->names[t])[1];

//// Récupération du nom de l'état p dans INTER
//// Numéro d'état provenant de A
// uint pia = ((uint *)INTER->names[pi])[0];
// if (pia == qa)
// {
// goala[pi] = t;
// }
// }

// for (uint t = 0; t < TRIPB->trans->size_graph; t++)
// {
//// Récupération du nom de l'état t dans TRIPA
//// Numéro d'état provenant de A
// uint qb = ((uint *)TRIPB->names[t])[0];
//// Numéro d'état provenant de INTER
// uint pi = ((uint *)TRIPB->names[t])[1];

//// Récupération du nom de l'état p dans INTER
//// Numéro d'état provenant de B
// uint pib = ((uint *)INTER->names[pi])[1];
// if (pib == qb)
// {
// goalb[pi] = t;
// }
// }

//// Calcul des paires non-séparable par G dans TRIP A et TRIP B
// p_graph ETRIPA = (*gext)(TRIPA);
// p_graph ETRIPB = (*gext)(TRIPB);

// delete_nfa(TRIPA); // TRIP A et TRIPB ne sont plus nécessaires.
// delete_nfa(TRIPB);

//// Calcul des nouvelles paires non-BPOL(G)-sep dans INTER
// for (uint q = 0; q < INTER->etrans->size; q++)
// {
// uint tempsize = size_vertices(INTER->etrans->edges[q]);
// if (tempsize == 0)
// {
// uint tqa = goala[q];
// uint tqb = goalb[q];
// if (tqa != TRIPA->trans->size_graph && tqb !=
// TRIPB->trans->size_graph)
// {

// for (uint i = 0; i < tempsize; i++)
// {
// uint r = lefpull_vertices(INTER->etrans->edges[q]);
// uint tra = goala[r];
// uint trb = goalb[r];
// if (tra != TRIPA->trans->size_graph && trb !=
// TRIPB->trans->size_graph && mem_vertices_sorted(tra,
// ETRIPA->edges[tqa]) && mem_vertices_sorted(trb,
// ETRIPB->edges[tqb]))
// {
// rigins_vertices(r, INTER->etrans->edges[q]);
// }
// else
// {
// modified = true;
// }
// }
// }
// else
// {
// makeempty_vertices(INTER->etrans->edges[q]);
// modified = true;
// }
// }
// }
// delete_graph(ETRIPA);
// delete_graph(ETRIPB);
// }

// nfa_trim(INTER);
// bool result = true;
// if (nfa_is_empty(INTER))
// {
// result = false;
// }
// delete_nfa(INTER);

// return result;
// }

// Calcul des BPol(MOD)-orbites
// p_orbits compute_bpmod_orbs(p_cayley M)
// {
//// Initialisation
// p_orbits res;
// MALLOC(res, 1);
// res->nb_idems = size_vertices(M->idem_list);
// MALLOC(res->orbits, res->nb_idems);

//// Première partie: Calcul des orbites elle-mêmes (on remplit les
// tableaux mono_in_sub et mono_to_sub)
//// (basé sur la co-Pol(MOD)-séparation)

//// Calcul du NFA qui capture les co-Pol(MOD)-paires
// p_nfa R = cayley_to_dfa(M);             // NFA associé au cayley droit
// p_nfa RU = nfa_proj_unary(R);           // Sa projection sur un alphabet
// unaire p_parti RSCC = nfa_inv_ext(RU);         // La construction des
// transitions inverses p_parti RFOL = nfa_stal_fold(RU, RSCC); // La
// partition correspondant au folding dans les SCCS delete_parti(RSCC); //
// Plus besoin des SCCS p_nfa RUEPS = nfa_dyck_ext(RU, RFOL);   // Folding
// et calcul des transitions Epsilon dans RU delete_nfa(RU); // Plus besoin
// de RU p_nfa REPS = nfa_merge_states(R, RFOL); // Folding dans le NFA du
// cayley droit original nfa_delete_names(REPS);                 // Pas
// besoin des noms (les numéros des états sont ceux des classes de RFOL)
// REPS->etrans = RUEPS->etrans;           // Copie des epsilon transitions
// dans ce NFA RUEPS->etrans = NULL;                   // Plus besoin du NFA
// sur l'alphabet Unaire delete_nfa(RUEPS);

// p_nfa RINTER = nfa_intersect(REPS, R, true); // Le NFA qui calcule les
// co-Pol(MOD)-paires delete_nfa(REPS); delete_nfa(R);

//// Tableau qui stocke les numéros des idempotents
// uint idemind[M->trans->size_graph];
// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// idemind[lefread_vertices(M->idem_list, i)] = i;
// }

//// On va marquer les co-Pol(MOD)-paires (e,s) avec e idempotent dans un
// tableau temporaire bool *nsept[M->trans->size_graph]; for (uint i = 0; i
// < size_vertices(M->idem_list); i++)
// {
// uint e = lefread_vertices(M->idem_list, i);
// MALLOC(nsept[e], M->trans->size_graph);
// for (uint s = 0; s < M->trans->size_graph; s++)
// {
// nsept[e][s] = false;
// }
// }
// for (uint p = 0; p < RINTER->trans->size_graph; p++)
// {
// uint c = ((uint **)RINTER->names)[p][0]; // La classe de RFOL
// correspondant à l'état p de RINTER uint s = ((uint
// **)RINTER->names)[p][1]; // L'élément de M correspondant à l'état p
// de RINTER

// for (uint i = 0; i < size_vertices(RFOL->cl[c]); i++)
// {
// uint e = lefread_vertices(RFOL->cl[c], i);
// if (M->idem_array[e])
// {
// nsept[e][s] = true;
// }
// }
// }

// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// uint e = lefread_vertices(M->idem_list, i);
// MALLOC(res->orbits[i], 1);
// res->orbits[i]->size = 0; // On calculera la taille plus tard
// MALLOC(res->orbits[i]->mono_in_sub, M->trans->size_graph);
// MALLOC(res->orbits[i]->mono_to_sub, M->trans->size_graph);
// for (uint s = 0; s < M->trans->size_graph; s++)
// {
// res->orbits[i]->mono_in_sub[s] = false;
// }

//// Calcul de l'ensemble des éléments
// p_vertices eM = compute_r_ideal(M, e, NULL);
// p_vertices Me = compute_l_ideal(M, e, NULL);
// p_vertices eMe = make_inter_sorted_vertices(eM, Me);
// delete_vertices(eM);
// delete_vertices(Me);

//// cayley_print_name(M, e, stdout);
//// printf("\n");

// for (uint j = 0; j < size_vertices(eMe); j++)
// {
// uint ese = lefread_vertices(eMe, j);
// if (nsept[e][ese])
// {
// res->orbits[i]->mono_in_sub[ese] = nsept[e][ese];
// res->orbits[i]->mono_to_sub[ese] = res->orbits[i]->size;
// res->orbits[i]->size++;
// }
// }

// MALLOC(res->orbits[i]->sub_to_mono, res->orbits[i]->size);
// uint n = 0;
// for (uint s = 0; s < M->trans->size_graph; s++)
// {

// if (res->orbits[i]->mono_in_sub[s])
// {
// res->orbits[i]->sub_to_mono[n] = s;
// n++;
// }
// }

// res->orbits[i]->neut = res->orbits[i]->mono_to_sub[e];
// delete_vertices(eMe);
// free(nsept[e]);
// }

//// Seconde partie: Calcul des idempotents dans chaque orbite

// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// MALLOC(res->orbits[i]->idem_array, res->orbits[i]->size);
// for (uint j = 0; j < res->orbits[i]->size; j++)
// {
// res->orbits[i]->idem_array[j] = false;
// }
// res->orbits[i]->idem_list = create_vertices();
// for (uint j = 0; j < res->orbits[i]->size; j++)
// {
// uint e = res->orbits[i]->sub_to_mono[j];
// if (M->idem_array[e])
// {
// res->orbits[i]->idem_array[j] = true;
// rigins_vertices(j, res->orbits[i]->idem_list);
// }
// }
// }

//// Troisième partie, calcul des relations de Green

//// Compteurs qu'on utilisera pour compter le nombre de classes dans une
// relation relation (un compteur pour chaque orbite) uint
// num[res->nb_idems];

//// Initialisation des structures
// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
//// Global

// MALLOC(res->orbits[i]->rels, 1);

//// R-classes
// MALLOC(res->orbits[i]->rels->RCL, 1);
// res->orbits[i]->rels->RCL->size_set = res->orbits[i]->size;
// MALLOC(res->orbits[i]->rels->RCL->numcl, res->orbits[i]->size);

//// L-classes
// MALLOC(res->orbits[i]->rels->LCL, 1);
// res->orbits[i]->rels->LCL->size_set = res->orbits[i]->size;
// MALLOC(res->orbits[i]->rels->LCL->numcl, res->orbits[i]->size);
// }

//// R-classes
// p_parti RCL = ltarjan(RINTER->trans);

//// Initialisation des compteurs
// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// num[i] = 0;
// }

//// pour chaque SCC de RINTER
// for (uint n = 0; n < RCL->size_par; n++)
// {

// bool inter[res->nb_idems];
// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// inter[i] = false;
// }

//// On regarde les état de la SCC
// for (uint j = 0; j < size_vertices(RCL->cl[n]); j++)
// {
// uint c = ((uint **)RINTER->names)[lefread_vertices(RCL->cl[n],
// j)][0]; // La classe de RFOL correspondant à l'état de RINTER
// uint s = ((uint **)RINTER->names)[lefread_vertices(RCL->cl[n],
// j)][1]; // L'élément de M correspondant à l'état de RINTER

//// On regarde les idempotents dans la classe c de RFOL
// for (uint k = 0; k < size_vertices(RFOL->cl[c]); k++)
// {
// uint e = lefread_vertices(RFOL->cl[c], k);
// if (M->idem_array[e])
// {
// uint i = idemind[e];
// if (res->orbits[i]->mono_in_sub[s])
// {
// inter[i] = true;
// res->orbits[i]->rels->RCL->numcl[res->orbits[i]->mono_to_sub[s]]
// = num[i];
// }
// }
// }
// }

// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// if (inter[i])
// {
// num[i]++;
// }
// }
// }

// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// MALLOC(res->orbits[i]->rels->RCL->cl, num[i]);
// res->orbits[i]->rels->RCL->size_par = num[i];
// for (uint c = 0; c < num[i]; c++)
// {
// res->orbits[i]->rels->RCL->cl[c] = create_vertices();
// }

// for (uint s = 0; s < res->orbits[i]->size; s++)
// {
// rigins_vertices(s,
// res->orbits[i]->rels->RCL->cl[res->orbits[i]->rels->RCL->numcl[s]]);
// }
// }

// delete_nfa(RINTER); // plus besoin de RINTER
// delete_parti(RFOL);

//// L-classes

//// Calcul du NFA symmétrique à R-inter basé le sur le cayley gauche
// p_nfa L = left_cayley_to_dfa(M);        // NFA associé au cayley gauche
// p_nfa LU = nfa_proj_unary(L);           // Sa projection sur un alphabet
// unaire p_parti LSCC = nfa_inv_ext(LU);         // La construction des
// transitions inverses p_parti LFOL = nfa_stal_fold(LU, LSCC); // La
// partition correspondant au folding dans les SCCS delete_parti(LSCC); //
// Plus besoin des SCCS p_nfa LUEPS = nfa_dyck_ext(LU, LFOL);   // Folding
// et calcul des transitions Epsilon dans LU delete_nfa(LU); // Plus besoin
// de LU p_nfa LEPS = nfa_merge_states(L, LFOL); // Folding dans le NFA du
// cayley droit original nfa_delete_names(LEPS);                 // Pas
// besoin des noms (les numéros des états sont ceux des classes de RFOL)
// LEPS->etrans = LUEPS->etrans;           // Copie des epsilon transitions
// dans ce NFA LUEPS->etrans = NULL;                   // Plus besoin du NFA
// sur l'alphabet Unaire delete_nfa(LUEPS);

// p_nfa LINTER = nfa_intersect(LEPS, L, true); // Le NFA qui calcule les
// co-Pol(MOD)-paires delete_nfa(LEPS); delete_nfa(L);

//// On peut maintenant passer au calcul des L-classes
// p_parti LCL = ltarjan(LINTER->trans);

//// Rénitialisation des compteurs
// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// num[i] = 0;
// }

//// pour chaque SCC de LINTER
// for (uint n = 0; n < LCL->size_par; n++)
// {

// bool inter[res->nb_idems];
// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// inter[i] = false;
// }

//// On regarde les états de la SCC
// for (uint j = 0; j < size_vertices(LCL->cl[n]); j++)
// {
// uint c = ((uint **)LINTER->names)[lefread_vertices(LCL->cl[n],
// j)][0]; // La classe de LFOL correspondant à l'état de LINTER
// uint s = ((uint **)LINTER->names)[lefread_vertices(LCL->cl[n],
// j)][1]; // L'élément de M correspondant à l'état de LINTER

//// On regarde les idempotents dans la classe c de LFOL
// for (uint k = 0; k < size_vertices(LFOL->cl[c]); k++)
// {
// uint e = lefread_vertices(LFOL->cl[c], k);
// if (M->idem_array[e])
// {
// uint i = idemind[e];
// if (res->orbits[i]->mono_in_sub[s])
// {
// inter[i] = true;
// res->orbits[i]->rels->LCL->numcl[res->orbits[i]->mono_to_sub[s]]
// = num[i];
// }
// }
// }
// }

// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// if (inter[i])
// {
// num[i]++;
// }
// }
// }

// for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// MALLOC(res->orbits[i]->rels->LCL->cl, num[i]);
// res->orbits[i]->rels->LCL->size_par = num[i];
// for (uint c = 0; c < num[i]; c++)
// {
// res->orbits[i]->rels->LCL->cl[c] = create_vertices();
// }

// for (uint s = 0; s < res->orbits[i]->size; s++)
// {
// rigins_vertices(s,
// res->orbits[i]->rels->LCL->cl[res->orbits[i]->rels->LCL->numcl[s]]);
// }
// }

// delete_nfa(LINTER); // plus besoin de LINTER
// delete_parti(LFOL);

//// J-classes (l'ordre n'est pas topologique), H-classes, idempotents et
// éléments réguliers for (uint i = 0; i < size_vertices(M->idem_list); i++)
// {
// j_green_compute(res->orbits[i]->rels);
// h_green_compute(res->orbits[i]->rels);
// gr_green_compute(res->orbits[i]->idem_list, res->orbits[i]->rels);
// }

// return res;
// }
