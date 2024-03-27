/*****************************/
/* Deterministic hierarchies */
/*****************************/

#include "class_deth.h"

/*************************/
/* Calcul de congruences */
/*************************/

// Calcule la plus petite partition contenant celle prise en entrée et
// compatible avec les transitions du graphe
// c'est-àdire que si q ≣ r, on doit avoir qa ≣ ra
static void cong_fold(p_dgraph G, p_ufind uf)
{
    // Une file qui contient une liste de de sommets à traiter
    // Elle contient initialement tous les sommets
    p_vertices tofold = create_vertices();

    // Des listes doublement chaînées pour mémorise les arêtes
    // On ne va remplir les listes que pour les représentants dans le union-find
    // original
    p_dlist ltrans[G->size_graph][G->size_alpha];
    for (uint q = 0; q < G->size_graph; q++)
    {
        if (q == find_ufind(q, uf))
        {
            rigins_vertices(q, tofold);
            for (uint a = 0; a < G->size_alpha; a++)
            {
                ltrans[q][a] = create_dlist();
            }
        }
        else
        {
            for (uint a = 0; a < G->size_alpha; a++)
            {
                ltrans[q][a] = NULL;
            }
        }
    }

    for (uint q = 0; q < G->size_graph; q++) // Remplissage des listes avec les
                                             // transitions d'origine
    {
        uint r = find_ufind(q, uf);          // On ne remplit que les listes des
                                             // représentant dans la partition
                                             // initiale
        // Pour chaque lettre
        for (uint a = 0; a < G->size_alpha; a++)
        {
            // On ajoute à r une transition vers qa
            insertprevious_dlist(ltrans[r][a], ltrans[r][a]->rsent, G->edges[q][a]);
        }
    }
    // Tant qu'il reste un sommet à traiter
    while (!isempty_vertices(tofold))
    {
        // On prend le représentant d'un sommet non-traité
        uint s      = find_ufind(lefpull_vertices(tofold), uf);

        bool folded = false;
        uint a      = 0;
        while (!folded && a < G->size_alpha)
        {
            if (ltrans[s][a]->size > 1)                                    // Si
                                                                           // il
                                                                           // y a
                                                                           // 2 arêtes
                                                                           // sortantes
                                                                           // étiquetées
                                                                           // par
                                                                           // a
            {
                // On prend les représentants des destinations des deux
                // premières arêtes qu'on va fusionner (si ça n'a pas été fait
                // avant)
                uint r = find_ufind(ltrans[s][a]->lsent->next->val, uf);
                uint t = find_ufind(ltrans[s][a]->lsent->next->next->val, uf);
                deletecell_dlist(ltrans[s][a], ltrans[s][a]->lsent->next); // On
                                                                           // supprime
                                                                           // la
                                                                           // première
                                                                           // des
                                                                           // deux
                                                                           // edges
                if (r != t)                                                // Si
                                                                           // les
                                                                           // deux
                                                                           // sommets
                                                                           // adjacents
                                                                           // n'avaient
                                                                           // pas
                                                                           // encore
                                                                           // étés
                                                                           // fusionnés
                {
                    union_ufind(r, t, uf);                                 // On
                                                                           // effectue
                                                                           // la
                                                                           // fusion
                    for (uint b = 0; b < G->size_alpha; b++)               // On
                                                                           // concatène
                                                                           // leur
                                                                           // listes
                                                                           // de
                                                                           // sommets
                                                                           // adjacents
                    {
                        concat_dlist(ltrans[r][b], ltrans[t][b]);
                        if (find_ufind(r, uf) == r)                        // On
                                                                           // ne
                                                                           // garde
                                                                           // que
                                                                           // la
                                                                           // liste
                                                                           // du
                                                                           // nouveau
                                                                           // représentant
                                                                           // de
                                                                           // la
                                                                           // classe
                        {
                            free(ltrans[t][b]);
                            ltrans[t][b] = NULL;
                        }
                        else
                        {
                            free(ltrans[r][b]);
                            ltrans[r][b] = NULL;
                        }
                    }
                    rigins_vertices(find_ufind(r, uf), tofold); // On va devoir
                                                                // éventuelement
                                                                // traiter la
                                                                // nouvelle
                                                                // classe
                    if (find_ufind(r, uf) != find_ufind(s, uf))
                    {
                        rigins_vertices(find_ufind(s, uf), tofold);
                    }
                    folded = true;
                }
                else
                {
                    rigins_vertices(find_ufind(s, uf), tofold);
                }
            }
            a++;
        }
    }

    // Suppression des listes
    for (uint q = 0; q < G->size_graph; q++)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {
            delete_dlist(ltrans[q][a]);
        }
    }
    delete_vertices(tofold);
}

// Calcule de la plus petit congruence contenant une partition quelconque
// (donnée sous la forme d'un union-find)
void compute_leastcong(p_cayley M, p_ufind uf)
{
    cong_fold(M->trans, uf);
    cong_fold(M->leftgraph, uf);
}

/****************************************/
/* Partitions générées par une équation */
/****************************************/

// Retourne l'équivalence obtenue en idientifiant les éléments qui devraient
// être égaux d'après l'équation de BPol(MOD)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// BPol(DD) sur le monoide
p_ufind iden_bpolmod_mono(p_cayley M, p_green G)
{
    // Gestion du cas pariculier où 1 a un antécédent non-vide (dans ce cas on
    // retourne simplement les J-classes)
    if (M->neutlet)
    {
        return parti_to_ufind(G->JCL);
    }

    p_ufind uf   = create_ufind(M->trans->size_graph);
    // Calcul préalable des MOD-pairs
    p_nfa A      = cayley_to_dfa(M);
    p_nfa U      = nfa_proj_unary(A);
    delete_nfa(A);
    p_parti SCCS = nfa_inv_ext(U);
    p_parti FOLD = nfa_stal_fold(U, SCCS);
    delete_parti(SCCS);
    p_nfa B      = nfa_dyck_ext(U, FOLD);
    delete_nfa(U);
    nfa_delete_names(B);
    p_nfa INTERSECT = nfa_intersect(B, B, true);
    delete_nfa(B);
    // Les MOD-pairs sont les états accessibles dans B

    for (uint p = 0; p < INTERSECT->trans->size_graph; p++)
    {
        uint c = ((uint **)INTERSECT->names)[p][0];
        uint d = ((uint **)INTERSECT->names)[p][1];
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
                    uint r  = lefread_vertices(G->JCL->cl[G->JCL->numcl[q]], j);
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
                                for (uint l = 0; l < size_vertices(G->JCL->cl[G->JCL->numcl[s]]);
                                     l++)
                                {
                                    uint t  = lefread_vertices(G->JCL->cl[G->JCL->numcl[s]], l);
                                    uint st = cayley_mult(M, s, t);
                                    // Il suffit de tester les st idempotents et
                                    // tels que st J t
                                    if (M->idem_array[st] && G->JCL->numcl[st] == G->JCL->numcl[s])
                                    {
                                        union_ufind(cayley_mult(M, qr, st),
                                                    cayley_mult_gen(M, 4, qr, q, t, st), uf);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    delete_nfa(INTERSECT);
    delete_parti(FOLD);

    return uf;
}

// Retourne l'équivalence obtenue en idientifiant les éléments qui devraient
// être égaux d'après l'équation de BPol(GR)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// BPol(DD) sur le monoide
p_ufind iden_blockg_mono(p_cayley M, p_green G)
{
    p_ufind uf = create_ufind(M->trans->size_graph);
    for (uint i = 0; i < size_vertices(M->idem_list) - 1; i++)
    {
        for (uint j = i + 1; j < size_vertices(M->idem_list); j++)
        {
            uint e = lefread_vertices(M->idem_list, i);
            uint f = lefread_vertices(M->idem_list, j);

            if (G->LCL->numcl[e] == G->LCL->numcl[f] || G->RCL->numcl[e] == G->RCL->numcl[f])
            {
                union_ufind(e, f, uf);
            }
        }
    }
    return uf;
}

// Retourne l'équivalence obtenue en idientifiant les éléments qui devraient
// être égaux d'après l'équation de Knast
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// BPol(DD) sur le monoide
p_ufind iden_knast_mono(p_cayley M, p_green G)
{
    // Gestion du cas partitculier où 1 a un antécédent non-vide (dans ce cas,
    // on retourne simplement la partition en J-classes)
    if (cayley_neutstrict(M, G))
    {
        return parti_to_ufind(G->JCL);
    }

    // Sinon, on créé le union-find en utilisant l'équation de Knast
    p_ufind uf = create_ufind(M->trans->size_graph);

    // Tableau qui mémorise les idempotents e déjà traités (il arrive que
    // certains soient traités simultanément)
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
            // L'équation étant symmétrique, il suffit de considérer les f tels
            // que e <= f

            // Tableau qui mémorise les idempotents f déjà traités pour ce e (il
            // arrive que certains soient traités simultanément)
            bool fdone[M->trans->size_graph];
            for (uint j = i; j < size_vertices(M->idem_list); j++)
            {
                fdone[lefread_vertices(M->idem_list, j)] = false;
            }

            // Calcul des ensembles eM et Me (on ne garde que les éléments
            // réguliers dans ces ensembles)
            // On marque les idempotents e' dans eM et Me: ils sont traités
            // automatiquement en même temps que e
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

            // On commence par considérer le cas f = e: on doit vérifier
            // (esete)^w = (esete)^w ese et (esete)^w = ete(esete)^w pour tous
            // s,t
            // On calcule eMe
            p_vertices eMe = make_inter_sorted_vertices(eM, Me);

            // Boucles sur les éléments ese
            for (uint k = 0; k < size_vertices(eMe) - 1; k++)
            {
                uint ese = lefread_vertices(eMe, k);
                // Boucle sur les éléments ete
                for (uint l = k; l < size_vertices(eMe); l++)
                {
                    uint ete = lefread_vertices(eMe, l);
                    // Il suffit de considérer les éléments ete tels que ese J
                    // ete
                    if (G->JCL->numcl[ese] == G->JCL->numcl[ete])
                    {
                        uint esete = cayley_mult(M, ese, ete);
                        // Il suffit de considérer les cas où esete J ese et
                        // esete est idempotent
                        if (M->idem_array[esete] && G->JCL->numcl[ese] == G->JCL->numcl[esete])
                        {
                            union_ufind(esete, cayley_mult(M, esete, ese), uf);
                            union_ufind(esete, cayley_mult(M, ete, esete), uf);
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
                    // Calcul des ensembles fM et Mf (on ne garde que les
                    // éléments réguliers dans ces ensembles)
                    // On marque les idempotents f' dans fM et Mf: ils sont
                    // traités automatiquement en même temps que f
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
                                // Il suffit de considérer les cas où eqfre J
                                // eqf et eqfre est idempotent
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
                                            // Il suffit de considérer les cas
                                            // où eqf J fre
                                            if (G->JCL->numcl[esf] == G->JCL->numcl[fte])
                                            {
                                                uint esfte = cayley_mult(M, esf, fte);
                                                // fprintf(out, "esfte: ");
                                                // cayley_print_name(M, esfte,
                                                // out);
                                                // fprintf(out, "\n");
                                                // Il suffit de considérer les
                                                // cas où esfte J esf et esfte
                                                // est idempotent
                                                if (M->idem_array[esfte] &&
                                                    G->JCL->numcl[esf] == G->JCL->numcl[esfte])
                                                {
                                                    union_ufind(cayley_mult(M, eqfre, esfte),
                                                                cayley_mult_gen(M, 4, eqfre, eqf,
                                                                                fte, esfte), uf);
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
    return uf;
}

// Retourne l'équivalence obtenue en idientifiant les éléments qui devraient
// être égaux d'après l'équation de BPol(MOD⁺)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// BPol(DD) sur le monoide
p_ufind iden_qknast_mono(p_cayley M, p_green G, p_green_sub mker)
{
    // Gestion du cas pariculier où 1 a un antécédent non-vide (dans ce cas on
    // retourne simplement les J-classes)
    if (cayley_neutstrict(M, G))
    {
        return iden_bpolmod_mono(M, G);
    }

    p_ufind uf = create_ufind(M->trans->size_graph);

    // Tableau qui marque les éléments du MOD-kernel qui sont réguliers dans
    // celui-ci
    bool reginker[M->trans->size_graph];
    for (uint s = 0; s < M->trans->size_graph; s++)
    {
        reginker[s] = mker->mono_in_sub[s] && mker->rels->regular_array[mker->mono_to_sub[s]];
    }

    // Tableau qui mémorise les idempotents e déjà traités (il arrive que
    // certains soient traités simultanément)
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
            // L'équation étant symmétrique, il suffit de considérer les f tels
            // que e <= f

            // Tableau qui mémorise les idempotents f déjà traités pour ce e (il
            // arrive que certains soient traités simultanément)
            bool fdone[M->trans->size_graph];
            for (uint j = i; j < size_vertices(M->idem_list); j++)
            {
                fdone[lefread_vertices(M->idem_list, j)] = false;
            }

            // Calcul des ensembles eM et Me (on ne garde que les éléments
            // réguliers dans ces ensembles)
            // On marque les idempotents e' dans eM et Me: ils sont traités
            // automatiquement en même temps que e
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

            // On commence par considérer le cas f = e: on doit vérifier
            // (esete)^w = (esete)^w ese et (esete)^w = ete(esete)^w pour tous
            // s,t
            // On calcule eMe
            p_vertices eMe = make_inter_sorted_vertices(eM, Me);

            // Boucles sur les éléments ese
            for (uint k = 0; k < size_vertices(eMe) - 1; k++)
            {
                uint ese = lefread_vertices(eMe, k);
                // Boucle sur les éléments ete
                for (uint l = k; l < size_vertices(eMe); l++)
                {
                    uint ete = lefread_vertices(eMe, l);
                    // Il suffit de considérer les éléments ete tels que ese J
                    // ete (dans le MOD-kernel)
                    if (mker->rels->JCL->numcl[mker->mono_to_sub[ese]] ==
                        mker->rels->JCL->numcl[mker->mono_to_sub[ete]])
                    {
                        uint esete = cayley_mult(M, ese, ete);
                        // Il suffit de considérer les cas où esete J ese  (dans
                        // le MOD-kernel) et esete est idempotent
                        if (M->idem_array[esete] &&
                            mker->rels->JCL->numcl[mker->mono_to_sub[ese]] ==
                            mker->rels->JCL->numcl[mker->mono_to_sub[esete]])
                        {
                            union_ufind(esete, ese, uf);
                            union_ufind(ete, esete, uf);
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
                    // Calcul des ensembles fM et Mf (on ne garde que les
                    // éléments réguliers du MOD-kernel dans ces ensembles)
                    // On marque les idempotents f' dans fM et Mf: ils sont
                    // traités automatiquement en même temps que f
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
                            // Il suffit de considérer les cas où eqf J fre
                            // (dans le MOD-kernel)
                            if (mker->rels->JCL->numcl[mker->mono_to_sub[eqf]] ==
                                mker->rels->JCL->numcl[mker->mono_to_sub[fre]])
                            {
                                uint eqfre = cayley_mult(M, eqf, fre);
                                // Il suffit de considérer les cas où eqfre J
                                // eqf et eqfre est idempotent
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
                                            // Il suffit de considérer les cas
                                            // où eqf J fre (dans le MOD-kenrel)
                                            if (mker->rels->JCL->numcl[mker->mono_to_sub[esf]] ==
                                                mker->rels->JCL->numcl[mker->mono_to_sub[fte]])
                                            {
                                                uint esfte = cayley_mult(M, esf, fte);
                                                // Il suffit de considérer les
                                                // cas où esfte J esf et esfte
                                                // est idempotent
                                                if (M->idem_array[esfte] &&
                                                    mker->rels->JCL->numcl[mker->mono_to_sub[esf]]
                                                    == mker->rels->JCL->numcl[mker->mono_to_sub[
                                                                                  esfte]])
                                                {
                                                    union_ufind(cayley_mult(M, eqfre, esfte),
                                                                cayley_mult_gen(M, 4, eqfre, eqf,
                                                                                fte, esfte), uf);
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
    return uf;
}

// Retourne l'équivalence obtenue en identifiant qui devraient être égauc selon
// l'équation de MPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// MPol(C) sur le monoide
// La congruence C canonique est passée en entrée
// Ne fonctionne que si le morphisme est dans UPol(C)
p_ufind iden_mpolc_mono(p_cayley M, p_parti C)
{
    // Création du Union-Find
    p_ufind theuf = create_ufind(M->trans->size_graph);

    // Pour toute classe de l'équivalence C
    for (uint c = 0; c < C->size_par; c++)
    {
        // On prend un premier élément s dans la classe
        for (uint i = 0; i < size_vertices(C->cl[c]); i++)
        {
            uint s = lefread_vertices(C->cl[c], i);

            // On prend un sécond élément distinct t dans la classe
            for (uint j = 0; j < size_vertices(C->cl[c]); j++)
            {
                if (i != j)
                {
                    uint t = lefread_vertices(C->cl[c], j);
                    // printf(" s: ");
                    // cayley_print_name(M, s, stdout);
                    // printf(".\n");
                    // printf(" t: ");
                    // cayley_print_name(M, t, stdout);
                    // printf(".\n");

                    // On prend un élément sq idempotent
                    p_vertices sM = compute_r_ideal(M, s, M->idem_array);
                    for (uint k = 0; k < size_vertices(sM); k++)
                    {
                        uint sq = lefread_vertices(sM, k);

                        // On prend un élément rt idempotent
                        p_vertices Mt = compute_l_ideal(M, t, M->idem_array);
                        for (uint l = 0; l < size_vertices(Mt); l++)
                        {
                            uint rt = lefread_vertices(Mt, l);
                            // printf(" sq: ");
                            // cayley_print_name(M, sq, stdout);
                            // printf(".\n");
                            // printf(" rt: ");
                            // cayley_print_name(M, rt, stdout);
                            // printf(".\n");
                            // Il suffit de considérer le cas où rt est
                            // idempotent

                            uint sqsrt = cayley_mult_gen(M, 3, sq, s, rt);
                            uint sqtrt = cayley_mult_gen(M, 3, sq, t, rt);
                            if (sqsrt != sqtrt)
                            {
                                // printf("Merging ");
                                // cayley_print_name(M, sqsrt, stdout);
                                // printf(" and ");
                                // cayley_print_name(M, sqtrt, stdout);
                                // printf(".\n");
                                // On identifie sqsrt et sqtrt
                                union_ufind(cayley_mult_gen(M, 3, sq, s, rt),
                                            cayley_mult_gen(M, 3, sq, t, rt), theuf);
                            }
                        }
                        delete_vertices(Mt);
                    }
                    delete_vertices(sM);
                }
            }
        }
    }
    return theuf;
}

// Retourne l'équivalence obtenue en identifiant qui devraient être égaux selon
// l'équation de LPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// LPol(C) sur le monoide
// La congruence C canonique est passée en entrée
// Ne fonctionne que si le morphisme est dans UPol(C)
p_ufind iden_lpolc_mono(p_cayley M, p_parti C)
{
    // Création du Union-Find
    p_ufind theuf = create_ufind(M->trans->size_graph);

    // Pour toute classe de l'équivalence C
    for (uint c = 0; c < C->size_par; c++)
    {
        // On prend un premier élément idempotent e dans la classe
        for (uint i = 0; i < size_vertices(C->cl[c]); i++)
        {
            uint e = lefread_vertices(C->cl[c], i);
            if (M->idem_array[e])
            {
                // On prend un sécond élément quelconque t dans la classe
                for (uint j = 0; j < size_vertices(C->cl[c]); j++)
                {
                    if (i != j)
                    {
                        uint t = lefread_vertices(C->cl[c], j);
                        union_ufind(e, cayley_mult(M, e, t), theuf);
                    }
                }
            }
        }
    }
    return theuf;
}

// Retourne l'équivalence obtenue en identifiant qui devraient être égaux selon
// l'équation de RPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// RPol(C) sur le monoide
// La congruence C canonique est passée en entrée
// Ne fonctionne que si le morphisme est dans UPol(C)
p_ufind iden_rpolc_mono(p_cayley M, p_parti C)
{ // Création du Union-Find
    p_ufind theuf = create_ufind(M->trans->size_graph);

    // Pour toute classe de l'équivalence C
    for (uint c = 0; c < C->size_par; c++)
    {
        // On prend un premier élément idempotent e dans la classe
        for (uint i = 0; i < size_vertices(C->cl[c]); i++)
        {
            uint e = lefread_vertices(C->cl[c], i);
            if (M->idem_array[e])
            {
                // On prend un sécond élément quelconque t dans la classe
                for (uint j = 0; j < size_vertices(C->cl[c]); j++)
                {
                    if (i != j)
                    {
                        uint t = lefread_vertices(C->cl[c], j);
                        union_ufind(e, cayley_mult(M, t, e), theuf);
                    }
                }
            }
        }
    }
    return theuf;
}

/**********************************************************************************/
/* Calcul du niveau dans une hiérarchie de négations en logique temporelle
   unaire */
/**********************************************************************************/

// Calcul du niveau dans la hiérarchie MPol. On passe un "prototype" de la
// congruence canonique C en entrée.
// (la plus petit congruence contenant la paertition doit être la congruence
// canonique C)
// On suppose que l'input est dans UPol(C) (ne termine pas dans le cas
// contraire)
short hdet_mpol_level(p_cayley M, p_ufind C, FILE *out)
{
    if (C->size_par == C->size_set)
    {
        if (out)
        {
            fprintf(out, "#### The language has level 1.\n");
        }
        return 1;
    }
    if (out)
    {
        fprintf(out, "#### The language does not have level 1.\n");
    }
    p_ufind theuf;
    compute_leastcong(M, C);
    p_parti thepr = ufind_to_parti(C);
    short level   = 2;
    while (true)
    {
        theuf = iden_mpolc_mono(M, thepr);
        delete_parti(thepr);
        if (theuf->size_par == theuf->size_set)
        {
            delete_ufind(theuf);
            if (out)
            {
                fprintf(out, "#### The language has level %d.\n", level);
            }
            return level;
        }
        if (out)
        {
            fprintf(out, "#### The language does not have level %d.\n", level);
        }
        level++;
        compute_leastcong(M, theuf);
        thepr = ufind_to_parti(theuf);
        delete_ufind(theuf);
    }
}

// Calcul du niveau dans les hiéarchies LPol/RPol de base C: 1 = RPol(C),LPol(C)
// 2 = RPol₂(C),LPol₂(C)...
void hdet_lrpol_level(p_cayley M, p_ufind C, FILE *out, short *minf, short *minp)
{
    if (C->size_par == C->size_set)
    {
        if (out)
        {
            fprintf(out,
                    "#### The language has level 1 in both the future hierarchy and the past hierarchy.\n");
        }
        *minf = 1;
        *minp = 1;
    }
    p_ufind ufleft;
    p_ufind ufright;
    compute_leastcong(M, C);
    p_parti pleft  = ufind_to_parti(C);
    p_parti pright = ufind_to_parti(C);
    short level    = 1;
    while (true)
    {
        ufleft  = iden_lpolc_mono(M, pright);
        ufright = iden_rpolc_mono(M, pleft);
        delete_parti(pright);
        delete_parti(pleft);
        if (ufleft->size_par == ufleft->size_set && ufright->size_par == ufright->size_set)
        {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out)
            {
                fprintf(out,
                        "#### The language has level %d in both the future hierarchy and the past hierarchy.\n",
                        level);
            }
            *minf = level;
            *minp = level;
            return;
        }
        else if (ufleft->size_par == ufleft->size_set)
        {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out && level % 2)
            {
                fprintf(out,
                        "#### The language has level %d in the past hierarchy but not in the future hierarchy.\n",
                        level);
                fprintf(out, "#### The language has level %d in the future hierarchy.\n",
                        level + 1);
                *minf = level + 1;
                *minp = level;
            }
            else if (out)
            {
                fprintf(out,
                        "#### The language has level %d in the future hierarchy but not in the past hierarchy.\n",
                        level);
                fprintf(out, "#### The language has level %d in the past hierarchy.\n", level + 1);
                *minf = level;
                *minp = level + 1;
            }
            return;
        }
        else if (ufright->size_par == ufright->size_set)
        {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out && level % 2)
            {
                fprintf(out,
                        "#### The language has level %d in the future hierarchy but not in the past hierarchy.\n",
                        level);
                fprintf(out, "#### The language has level %d in the past hierarchy.\n", level + 1);
                *minf = level;
                *minp = level + 1;
            }
            else if (out)
            {
                fprintf(out,
                        "#### The language has level %d in the past hierarchy but not in the future hierarchy.\n",
                        level);
                fprintf(out, "#### The language has level %d in the future hierarchy.\n",
                        level + 1);
                *minf = level + 1;
                *minp = level;
            }

            return;
        }

        if (out)
        {
            fprintf(out, "#### The language does not have level %d in either hierarchy.\n", level);
        }
        level++;
        compute_leastcong(M, ufleft);
        compute_leastcong(M, ufright);
        pleft  = ufind_to_parti(ufleft);
        pright = ufind_to_parti(ufright);
        delete_ufind(ufleft);
        delete_ufind(ufright);
    }
}
