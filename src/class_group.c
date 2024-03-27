#include "class_group.h"

/**************/
/* Membership */
/**************/

// Langages de groupes

// Pour le monoide syntaxique
bool is_group_mono(p_cayley M, p_green G, FILE *out)
{
    if (out)
    {
        fprintf(out, "#### Checking if the monoid is a group.\n");
    }
    // Si il n'y a qu'une seule J-classe (celle de 1, le monoide est un groupe)
    if (G->JCL->size_par == 1)
    {
        if (out != NULL)
        {
            fprintf(out, "#### This monoid is a group.\n");
        }
        return true;
    }
    else
    {
        // Sinon, on va chercher un élément non J-équivalent à 1
        if (out != NULL)
        {
            // On prend un élément dans la seconde J-classe du monoide
            uint s = lefread_vertices(G->JCL->cl[1], 0);
            fprintf(out, "#### This monoid is NOT a group.\n");
            fprintf(out, "#### For instance, ");
            cayley_print_name(M, s, out);
            fprintf(out, " is not J-equivalent to 1.\n");
        }

        return false;
    }
}

// Pour l'automate minimal
bool is_permutation_dfa(p_nfa A, FILE *out)
{
    for (uint s = 0; s < A->trans->size_graph; s++)
    {
        for (uint t = 0; t < A->trans->size_graph; t++)
        {
            if (s != t)
            {
                for (uint b = 0; b < A->trans->size_alpha; b++)
                {
                    if (lefread_vertices(A->trans->edges[s][b],
                                         0) == lefread_vertices(A->trans->edges[t][b], 0))
                    {
                        if (out != NULL)
                        {
                            fprintf(out, "#### This is NOT a permutation automaton.\n");

                            fprintf(out,
                                    "#### For instance, we have the transitions (%d,%c,%d) and (%d,%c,%d).\n",
                                    s, b + 'a', lefread_vertices(A->trans->edges[s][b], 0), t,
                                    b + 'a',
                                    lefread_vertices(A->trans->edges[t][b], 0));
                        }
                        return false;
                    }
                }
            }
        }
    }

    if (out != NULL)
    {
        fprintf(out, "#### This is a permutation automaton.\n");
    }
    return true;
}

// Langages modulo

// Pour un graphe de cayley
bool is_letterind_mono(p_cayley M, FILE *out)
{
    if (out)
    {
        fprintf(out,
                "#### Checking if the morphism maps every letter a ∊ A to the same element.\n");
    }
    for (uint a = 1; a < M->trans->size_alpha; a++)
    {
        if (M->trans->edges[0][a] != M->trans->edges[0][0])
        {
            if (out != NULL)
            {
                fprintf(out, "#### There are letters mapped to distinct elements.\n");
                fprintf(out,
                        "#### For instance, \"a\" and \"%c\" are mapped to distinct elements.\n",
                        a + 'a');
            }
            return false;
        }
    }
    if (out != NULL)
    {
        fprintf(out, "#### All letters are mapped to the same element.\n");
    }
    return true;
}

// Pour l'automate minimal
bool is_letterind_dfa(p_nfa A, FILE *out)
{
    for (uint s = 0; s < A->trans->size_graph; s++)
    {
        for (uint a = 1; a < A->trans->size_alpha; a++)
        {
            if (lefread_vertices(A->trans->edges[s][0],
                                 0) != lefread_vertices(A->trans->edges[s][a], 0))
            {
                if (out != NULL)
                {
                    fprintf(out,
                            "#### There are letters which have distinct actions on the states.\n");
                    fprintf(out,
                            "#### For instance, we have the transitions (%d,%c,%d) and (%d,%c,%d).\n",
                            s, 0 + 'a', lefread_vertices(A->trans->edges[s][0], 0), s, a + 'a',
                            lefread_vertices(A->trans->edges[s][a], 0));
                    return false;
                }
            }
        }
    }

    if (out != NULL)
    {
        fprintf(out, "#### All letters have the same action on the states.\n");
    }
    return true;
}

// Langages triviaux

// Pour le monoide syntaxique
bool is_trivial_monoid(p_cayley M, FILE *out)
{
    if (out)
    {
        fprintf(out, "#### Checking if the monoid is trivial.\n");
    }
    if (M->trans->size_graph == 1)
    {
        if (out != NULL)
        {
            fprintf(out, "#### The is a trivial monoid.\n");
        }
        return true;
    }
    else
    {
        if (out != NULL)
        {
            fprintf(out, "#### The is a non-trivial monoid.\n");
        }

        return false;
    }
}

// Pour l'automate minimal
bool is_trivial_dfa(p_nfa A, FILE *out)
{
    if (A->trans->size_graph == 1)
    {
        if (out != NULL)
        {
            fprintf(out, "#### This automaton has a single state.\n");
        }
        return true;
    }
    else
    {
        if (out != NULL)
        {
            fprintf(out, "#### This automaton has more than one state.\n");
        }
        return false;
    }
}

/*****************************************************/
/* Constructions impliquant les transitions inverses */
/*****************************************************/

// Construction des transitions inverses à l'intérieur des SCCs. Si
// il y a déjà des transitions inverses, elles seront supprimées et
// recalculées.
// Les transitions epsilon sont éliminées avant le calcul.
// La partition en SCCS utilisée pour le calcul est retournée (elle
// est souvent utile pour les calculs suivants).
p_parti nfa_inv_ext(p_nfa A)
{
    // Si il y a déjà des transitions inverses, on les supprime
    if (A->itrans != NULL)
    {
        delete_lgraph(A->itrans);
        A->itrans = NULL;
    }

    // Si il y a des transitions epsilon, on les élimine
    if (A->etrans != NULL)
    {
        nfa_elimeps_mod(A);
    }

    // Calcul des composantes fortement connexes
    p_parti P = ltarjan(A->trans);

    // On peut maintenant calculer les transitions inverses
    A->itrans = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);

    // Pour chaque état q
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        // Pour chaque lettre a
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            // Pour chaque état r tel que (q,a,r) est une transition
            for (uint i = 0; i < size_vertices(A->trans->edges[q][a]); i++)
            {
                // Si q et r sont dans la même SCC
                if (P->numcl[q] == P->numcl[lefread_vertices(A->trans->edges[q][a], i)])
                {
                    // On ajoute la transition (r,a-1,q)
                    insert_vertices(A->itrans->edges[lefread_vertices(A->trans->edges[q][a], i)][a],
                                    q);
                }
            }
        }
        //// Inversion des transitions epsilon
        // if (A->etrans != NULL)
        // {
        // for (uint i = 0; i < size_vertices(A->etrans->edges[q]); i++)
        // {
        // if (part->numcl[q] ==
        // part->numcl[lefread_vertices(A->etrans->edges[q], i)])
        // {
        // insert_vertices(A->etrans->edges[lefread_vertices(A->etrans->edges[q],
        // i)], q);
        // }
        // }
        // }
    }
    // On retourne la partition en SCCS
    return P;
}

// Suppression des transitions inverses
void nfa_remove_inv(p_nfa A)
{
    if (A->itrans != NULL)
    {
        delete_lgraph(A->itrans);
        A->itrans = NULL;
    }
}

/*******************************************/
/* Séparation pour les langages de groupes */
/*******************************************/

// Calcul de la partition obtenue en effectuant le folding à l'intérieur
// des SCCS. Les transitions inverses doivent déjà avoir été calculées et
// la partition prise en entrée doit être celle des SCCS.
p_parti nfa_stal_fold(p_nfa A, p_parti sccs)
{
    if (A->etrans != NULL)
    {
        printf("Folding only works for NFAs without epsilon transitions. Returned NULL.\n");
        return NULL;
    }
    if (A->itrans == NULL)
    {
        printf("Inverse transitions must have already been computed. Returned NULL.\n");
        return NULL;
    }

    p_ufind merge = create_ufind(A->trans->size_graph);           // Union-find
                                                                  // pour la
                                                                  // partition
    p_dlist ltrans[A->trans->size_graph][A->trans->size_alpha];   // Des listes
                                                                  // doublement
                                                                  // chaînées
                                                                  // pour les
                                                                  // arrêtes
                                                                  // classiques
    p_dlist litrans[A->itrans->size_graph][A->trans->size_alpha]; // Des listes
                                                                  // doublement
                                                                  // chaînées
                                                                  // pour les
                                                                  // arrêtes
                                                                  // inverses
    for (uint q = 0; q < A->trans->size_graph; q++)               // Remplissage
                                                                  // des listes
                                                                  // avec les
                                                                  // transitions
                                                                  // d'origine
                                                                  // (on ne
                                                                  // garde que
                                                                  // les
                                                                  // transitions
                                                                  // internes
                                                                  // aux sccs)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            ltrans[q][a]  = create_dlist();
            litrans[q][a] = create_dlist();
            for (uint i = 0; i < size_vertices(A->trans->edges[q][a]); i++)
            {
                // printf("r: %d\n", lefread_vertices(A->trans->edges[q][a],
                // i));
                if (sccs->numcl[lefread_vertices(A->trans->edges[q][a], i)] == sccs->numcl[q])
                {
                    insertprevious_dlist(ltrans[q][a], ltrans[q][a]->rsent,
                                         lefread_vertices(A->trans->edges[q][a], i));
                }
                // printf("Test\n");
            }
            for (uint i = 0; i < size_vertices(A->itrans->edges[q][a]); i++)
            {
                insertprevious_dlist(litrans[q][a], litrans[q][a]->rsent,
                                     lefread_vertices(A->itrans->edges[q][a], i));
            }
        }
    }

    // Une file qui contient une liste de de sommets à traiter
    // Elle contient initialement tous les sommets
    p_vertices tofold = create_vertices();
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        rigins_vertices(q, tofold);
    }

    // Tant qu'il reste un sommet à traiter
    while (!isempty_vertices(tofold))
    {
        // On prend le représentant d'un sommet non-traité
        uint q      = find_ufind(lefpull_vertices(tofold), merge);

        bool folded = false;
        uint a      = 0;
        while (!folded && a < A->trans->size_alpha)
        {
            if (ltrans[q][a]->size > 1)                                    // Si
                                                                           // il
                                                                           // y a
                                                                           // 2 arêtes
                                                                           // sortantes
                                                                           // étiquetées
                                                                           // par
                                                                           // a
            {
                // On prend les représentant des destinations des deux premières
                // arêtes qu'on va fusionner (si ça n'a pas été fait avant)
                uint r = find_ufind(ltrans[q][a]->lsent->next->val, merge);
                uint s = find_ufind(ltrans[q][a]->lsent->next->next->val, merge);
                deletecell_dlist(ltrans[q][a], ltrans[q][a]->lsent->next); // On
                                                                           // supprime
                                                                           // la
                                                                           // première
                                                                           // des
                                                                           // deux
                                                                           // edges
                if (r != s)                                                // Si
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
                    union_ufind(r, s, merge);                              // On
                                                                           // effectue
                                                                           // la
                                                                           // fusion
                    for (uint b = 0; b < A->trans->size_alpha; b++)        // On
                                                                           // concatène
                                                                           // leur
                                                                           // listes
                                                                           // de
                                                                           // sommets
                                                                           // adjacents
                    {
                        concat_dlist(ltrans[r][b], ltrans[s][b]);
                        concat_dlist(litrans[r][b], litrans[s][b]);
                        if (find_ufind(r, merge) == r)                     // On
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
                            free(ltrans[s][b]);
                            ltrans[s][b]  = NULL;
                            free(litrans[s][b]);
                            litrans[s][b] = NULL;
                        }
                        else
                        {
                            free(ltrans[r][b]);
                            ltrans[r][b]  = NULL;
                            free(litrans[r][b]);
                            litrans[r][b] = NULL;
                        }
                    }
                    rigins_vertices(find_ufind(r, merge), tofold); // On va
                                                                   // devoir
                                                                   // éventuelement
                                                                   // traiter la
                                                                   // nouvelle
                                                                   // classe
                    if (find_ufind(r, merge) != find_ufind(q, merge))
                    {
                        rigins_vertices(find_ufind(q, merge), tofold);
                    }
                    folded = true;
                }
                else
                {
                    rigins_vertices(find_ufind(q, merge), tofold);
                }
            }
            else if (litrans[q][a]->size > 1)                                // Si
                                                                             // il
                                                                             // y
                                                                             // a
                                                                             // 2
                                                                             // arêtes
                                                                             // sortantes
                                                                             // étiquetées
                                                                             // par
                                                                             // a-1
            {
                // On prend les représentant des destinations des deux premières
                // arêtes qu'on va fusionner (si ça n'a pas été fait avant)
                uint r = find_ufind(litrans[q][a]->lsent->next->val, merge);
                uint s = find_ufind(litrans[q][a]->lsent->next->next->val, merge);
                deletecell_dlist(litrans[q][a], litrans[q][a]->lsent->next); // On
                                                                             // supprime
                                                                             // la
                                                                             // première
                                                                             // des
                                                                             // deux
                                                                             // edges
                if (r != s)                                                  // Si
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
                    union_ufind(r, s, merge);                                // On
                                                                             // effectue
                                                                             // la
                                                                             // fusion
                    for (uint b = 0; b < A->itrans->size_alpha; b++)         // On
                                                                             // concatène
                                                                             // leur
                                                                             // listes
                                                                             // de
                                                                             // sommets
                                                                             // adjacents
                    {
                        concat_dlist(ltrans[r][b], ltrans[s][b]);
                        concat_dlist(litrans[r][b], litrans[s][b]);
                        if (find_ufind(r, merge) == r)                       // On
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
                            free(ltrans[s][b]);
                            ltrans[s][b]  = NULL;
                            free(litrans[s][b]);
                            litrans[s][b] = NULL;
                        }
                        else
                        {
                            free(ltrans[r][b]);
                            ltrans[r][b]  = NULL;
                            free(litrans[r][b]);
                            litrans[r][b] = NULL;
                        }
                    }
                    rigins_vertices(find_ufind(r, merge), tofold); // On va
                                                                   // devoir
                                                                   // éventuellement
                                                                   // traiter la
                                                                   // nouvelle
                                                                   // classe
                    if (find_ufind(r, merge) != find_ufind(q, merge))
                    {
                        rigins_vertices(find_ufind(q, merge), tofold);
                    }
                    folded = true;
                }
                else
                {
                    rigins_vertices(find_ufind(q, merge), tofold);
                }
            }
            a++;
        }
    }
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            delete_dlist(ltrans[q][a]);
            delete_dlist(litrans[q][a]);
        }
    }

    delete_vertices(tofold);
    p_parti result = ufind_to_parti_refined(merge, sccs);
    delete_ufind(merge);
    return result;
}

// Calcul du NFA obtenu en effectuant le folding et en ajoutant ensuite
// des transitions espilon entre les paires d'états liées par un mot de
// Dyck.
// Les transitions inverses doivent déjà avoir été calculées et on doit
// fournir les partitions en SCCS et celle correspondant au folding.
p_nfa nfa_dyck_ext(p_nfa A, p_parti FOLD)
{
    if (A->etrans != NULL)
    {
        printf(
            "Adding Dyck transitions only works for NFAs without epsilon transitions. Returned NULL.\n");
        return NULL;
    }
    if (A->itrans == NULL)
    {
        printf("Inverse transitions must have already been computed. Returned NULL.\n");
        return NULL;
    }

    p_nfa B;
    if (FOLD != NULL)
    {
        // Calcul du NFA obtenu en effectuant le folding
        B = nfa_merge_states(A, FOLD);
    }
    else
    {
        B = A;
    }

    // Initialisation des transitions epsilon.
    // (en particulier, on a la transition (q,q) pour tout état q)
    init_epstrans(B);

    // Flag pour terminer le point fixe quand on ne modifie plus rien
    bool modified = true;
    // Le point fixe qui calcule les transitions epsilon
    while (modified)
    {
        modified = false;

        // Ajout de nouvelles transitions epsilon avec les aa-1 a-1a
        for (uint q = 0; q < B->etrans->size; q++)
        {
            uint oldsize = size_vertices(B->etrans->edges[q]); // Ancien nombre
                                                               // de transitions
                                                               // epsilon
                                                               // partant de q
            for (uint a = 0; a < B->trans->size_alpha; a++)
            {
                uint r, s;
                // Prise en compte des chemins a 1 a-1
                // Boucle sur les états tels que B contient une transtion
                // (q,a,r)
                for (uint i = 0; i < size_vertices(B->trans->edges[q][a]); i++)
                {
                    r = lefread_vertices(B->trans->edges[q][a], i);

                    // Boucle sur les s tels qu'on a déjà la transition epsilon
                    // (r,s)
                    for (uint j = 0; j < size_vertices(B->etrans->edges[r]); j++)
                    {
                        s = lefread_vertices(B->etrans->edges[r], j);

                        // Pour chaque état t tel qu'il existe une transition
                        // (s,a-1,t) dans B,
                        // on ajoute toutes les transitions epsilon (q,t)
                        merge_sorted_vertices(B->etrans->edges[q], B->itrans->edges[s][a]);
                    }
                }
                // Prise en compte des chemins a-1 1 a
                // Boucle sur les r tels que B contient une transtion (q,a-1,r)
                for (uint i = 0; i < size_vertices(B->itrans->edges[q][a]); i++)
                {
                    r = lefread_vertices(B->itrans->edges[q][a], i);
                    // Boucle sur les s tel qu'on a déjà la transition epsilon
                    // (r,s)
                    for (uint j = 0; j < size_vertices(B->etrans->edges[r]); j++)
                    {
                        s = lefread_vertices(B->etrans->edges[r], j);

                        // Pour chaque état t tel qu'il existe une transition
                        // (s,a,t) dans B,
                        // on ajoute toutes les transitions epsilon (q,t)
                        merge_sorted_vertices(B->etrans->edges[q], B->trans->edges[s][a]);
                    }
                }
            }

            if (oldsize != size_vertices(B->etrans->edges[q])) // Si au moins
                                                               // une nouvelle
                                                               // transition
                                                               // epsilon créée
            {
                modified = true;                               // On devra faire
                                                               // un autre tour
                                                               // de boucle
                make_tclos_graph(B->etrans);                   // On fait la
                                                               // clôture
                                                               // transitive des
                                                               // transitions
                                                               // epsilon pour
                                                               // le tour
                                                               // suivant
            }
        }
    }
    return B;
}

// Calcul du G-kernel
p_green_sub get_grp_kernel(p_cayley M, p_green G)
{
    // Création du sous-monoide
    p_green_sub gker = init_submono(M, G);
    MALLOC(gker->rels, 1);

    /* On va utiliser l'algorithme de GR-separation */
    p_nfa A      = cayley_to_dfa(M);
    p_parti SCCS = nfa_inv_ext(A);
    p_parti FOLD = nfa_stal_fold(A, SCCS);
    delete_parti(SCCS);
    p_nfa B      = nfa_dyck_ext(A, FOLD);
    delete_nfa(A);

    // Calcul des éléments du GR-kernel
    // Ce sont les éléments qui se trouvent dans une classe de FOLD telle qu'il
    // existe
    // une pesilon transition vers cette classe dans B
    gker->size = 0;
    MALLOC(gker->mono_in_sub, M->trans->size_graph);
    for (uint s = 0; s < M->trans->size_graph; s++)
    {
        gker->mono_in_sub[s] = false;
    }

    for (uint i = 0; i < size_vertices(B->etrans->edges[ONE]); i++)
    {
        gker->size = gker->size + size_vertices(FOLD->cl[lefread_vertices(B->etrans->edges[ONE],
                                                                          i)]);
        for (uint j = 0; j < size_vertices(FOLD->cl[lefread_vertices(B->etrans->edges[ONE], i)]);
             j++)
        {
            uint s = lefread_vertices(FOLD->cl[lefread_vertices(B->etrans->edges[ONE], i)], j);
            gker->mono_in_sub[s] = true;
        }
    }

    MALLOC(gker->mono_to_sub, M->trans->size_graph);
    MALLOC(gker->sub_to_mono, gker->size);
    uint num = 0;

    for (uint s = 0; s < M->trans->size_graph; s++)
    {
        if (gker->mono_in_sub[s])
        {
            gker->mono_to_sub[s]   = num;
            gker->sub_to_mono[num] = s;
            num++;
        }
    }

    // Calcul des listes d'idempotents
    gker->idem_list = create_vertices();
    MALLOC(gker->idem_array, gker->size);
    for (uint s = 0; s < gker->size; s++)
    {
        if (M->idem_array[gker->sub_to_mono[s]])
        {
            gker->idem_array[s] = true;
            rigins_vertices(s, gker->idem_list);
        }
        else
        {
            gker->idem_array[s] = false;
        }
    }

    // Calcul des classes du GR-Kernel

    // On ordonne les éléments du GR-Kernel selon l'ordre J du monoide original
    // (utile pour avoir les R,L,J-classes du Kernel dans un ordre topologique)
    uint jord[gker->size];
    num = 0;
    // p_vertices jord = create_vertices();
    for (uint cjo = 0; cjo < G->JCL->size_par; cjo++)
    {
        for (uint i = 0; i < size_vertices(G->JCL->cl[cjo]); i++)
        {
            if (gker->mono_in_sub[lefread_vertices(G->JCL->cl[cjo], i)])
            {
                jord[num] = gker->mono_to_sub[lefread_vertices(G->JCL->cl[cjo], i)];
                num++;
            }
        }
    }
    delete_nfa(B);

    // Calcul des R-classes
    // Ce sont les classes de FOLD incluses dans le G-kernel

    MALLOC(gker->rels->RCL, 1);
    gker->rels->RCL->size_set = gker->size;
    MALLOC(gker->rels->RCL->numcl, gker->size);

    bool done[gker->size];

    for (uint s = 0; s < gker->size; s++)
    {
        done[s] = false;
    }
    num = 0;

    for (uint i = 0; i < gker->size; i++)
    {
        // On prend le ième élément dans l'ordre donné par les J-classes
        if (!done[jord[i]])
        {
            // Le numéro de l'élément correspondant dans le monoide original
            uint ms = gker->sub_to_mono[jord[i]];

            // Construction de la R-classe de jord[i] (il s'agit de la
            // FOLD-class de ms)
            uint c = FOLD->numcl[ms];
            for (uint j = 0; j < size_vertices(FOLD->cl[c]); j++)
            {
                uint r = gker->mono_to_sub[lefread_vertices(FOLD->cl[c], j)];
                done[r]                   = true;
                gker->rels->RCL->numcl[r] = num;
            }
            num++;
        }
    }
    gker->rels->RCL->size_par = num;
    MALLOC(gker->rels->RCL->cl, num);

    for (uint i = 0; i < num; i++)
    {
        gker->rels->RCL->cl[i] = create_vertices();
    }
    for (uint s = 0; s < gker->size; s++)
    {
        rigins_vertices(s, gker->rels->RCL->cl[gker->rels->RCL->numcl[s]]);
    }

    delete_parti(FOLD);

    // Calcul des L-classes
    A    = left_cayley_to_dfa(M);
    SCCS = nfa_inv_ext(A);
    FOLD = nfa_stal_fold(A, SCCS);
    delete_parti(SCCS);

    MALLOC(gker->rels->LCL, 1);
    gker->rels->LCL->size_set = gker->size;
    MALLOC(gker->rels->LCL->numcl, gker->size);

    for (uint s = 0; s < gker->size; s++)
    {
        done[s] = false;
    }
    num = 0;

    for (uint i = 0; i < gker->size; i++)
    {
        // On prend le ième élément dans l'ordre donné par les J-classes
        if (!done[jord[i]])
        {
            // Le numéro de l'élément correspondant dans le monoide original
            uint ms = gker->sub_to_mono[jord[i]];

            // Construction de la L-classe de jord[i]
            uint c = FOLD->numcl[ms];
            for (uint j = 0; j < size_vertices(FOLD->cl[c]); j++)
            {
                uint r = gker->mono_to_sub[lefread_vertices(FOLD->cl[c], j)];
                done[r]                   = true;
                gker->rels->LCL->numcl[r] = num;
            }
            num++;
        }
    }
    gker->rels->LCL->size_par = num;
    MALLOC(gker->rels->LCL->cl, num);

    for (uint i = 0; i < num; i++)
    {
        gker->rels->LCL->cl[i] = create_vertices();
    }
    for (uint s = 0; s < gker->size; s++)
    {
        rigins_vertices(s, gker->rels->LCL->cl[gker->rels->LCL->numcl[s]]);
    }

    delete_parti(FOLD);

    // Calcul des J-classes

    // Création de la partition en J-classes
    MALLOC(gker->rels->JCL, 1);
    gker->rels->JCL->size_set = gker->size;
    MALLOC(gker->rels->JCL->numcl, gker->size);

    for (uint s = 0; s < gker->size; s++)
    {
        done[s] = false;
    }
    num = 0;

    for (uint i = 0; i < gker->size; i++)
    {
        if (!done[jord[i]])
        {
            uint cr = gker->rels->RCL->numcl[jord[i]];
            for (uint j = 0; j < size_vertices(gker->rels->RCL->cl[cr]); j++)
            {
                uint s = lefread_vertices(gker->rels->RCL->cl[cr], j);
                if (!done[s])
                {
                    uint cl = gker->rels->LCL->numcl[s];
                    for (uint k = 0; k < size_vertices(gker->rels->LCL->cl[cl]); k++)
                    {
                        gker->rels->JCL->numcl[lefread_vertices(gker->rels->LCL->cl[cl], k)] = num;
                        done[lefread_vertices(gker->rels->LCL->cl[cl], k)]                   = true;
                    }
                }
            }
            num++;
        }
    }

    gker->rels->JCL->size_par = num;
    MALLOC(gker->rels->JCL->cl, num);
    for (uint i = 0; i < num; i++)
    {
        gker->rels->JCL->cl[i] = create_vertices();
    }
    for (uint s = 0; s < gker->size; s++)
    {
        rigins_vertices(s, gker->rels->JCL->cl[gker->rels->JCL->numcl[s]]);
    }

    // Création des H-classes
    h_green_compute(gker->rels);

    // Il reste à remplir les listes des H-classes qui sont des groupes et des
    // J-classes régulières
    gr_green_compute(gker->idem_list, gker->rels);
    return gker;
}

// Procédure de séparation complète
bool decid_grp_sep(p_nfa I1, p_nfa I2, bool details, FILE *out)
{
    // Gestion des cas triviaux
    if (isempty_vertices(I1->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_vertices(I2->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (I1 == I2)
    {
        if (out && details)
        {
            fprintf(out, "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }
    if (out)
    {
        fprintf(out, "#### Using the GR-separation algorithm based on inverse extension.\n\n");
    }

    // Phase 1: Calcul des transitions inverses
    p_parti SCCS1 = nfa_inv_ext(I1);
    p_parti SCCS2 = nfa_inv_ext(I2);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out,
                "#### Phase 1: adding the inverse transitions inside the strongly connected components.\n");
        fprintf(out, "#### NFA of the first input language extended with inverse transitions.\n");
        view_nfa(I1);
        fprintf(out, "#### NFA of the second input language extended with inverse transitions.\n");
        view_nfa(I2);
    }

    // Phase 2: calcul des transitions induites par les mots de Dyck puis
    // éliminations de celles-ci
    p_parti FOLD1 = nfa_stal_fold(I1, SCCS1);
    delete_parti(SCCS1);
    p_nfa A1      = nfa_dyck_ext(I1, FOLD1);
    delete_parti(FOLD1);
    nfa_remove_inv(I1);
    nfa_elimeps_mod(A1);
    p_parti FOLD2 = nfa_stal_fold(I2, SCCS2);
    delete_parti(SCCS2);
    p_nfa A2      = nfa_dyck_ext(I2, FOLD2);
    delete_parti(FOLD2);
    nfa_remove_inv(I2);
    nfa_elimeps_mod(A2);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out,
                "\n****************** Phase 2: Computing the epsilon transitions induced by Dyck words and eliminating them.\n");
        fprintf(out, "#### Construction on the NFA of the first input language.\n");
        view_nfa(A1);
        fprintf(out, "#### Construction on the NFA of the second input language.\n");
        view_nfa(A2);
    }

    // Phase 3: Calcul de l'intersection

    p_nfa INTERSECT = nfa_intersect(A1, A2, true);
    delete_nfa(A1);
    delete_nfa(A2);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out,
                "\n****************** Phase 3: Computing the intersection of the resulting NFAs.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_vertices(INTERSECT->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        delete_nfa(INTERSECT);
        return true;
    }
    else
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        delete_nfa(INTERSECT);
        return false;
    }
}

/***************************************/
/* Séparation pour les langages modulo */
/***************************************/

// Projection d'un NFA sur un alphabet à une lettre (on
// ramène la MOD-separation à la GR-separation).
// Les transitions epsilon sont éliminées
p_nfa nfa_proj_unary(p_nfa A)
{
    if (A->itrans != NULL)
    {
        printf("Cannot project a NFA with inverse transitions. Returned NULL.\n");
        return NULL;
    }
    // Élimination des éventuelles transitions epsilon
    nfa_elimeps_mod(A);

    // Création du nouveau NFA
    p_nfa B;
    MALLOC(B, 1);
    B->initials = create_vertices();
    copy_vertices_right(B->initials, A->initials, 0);
    B->finals   = create_vertices();
    copy_vertices_right(B->finals, A->finals, 0);
    B->trans    = create_lgraph_noedges(A->trans->size_graph, 1);
    B->etrans   = NULL;
    B->itrans   = NULL;

    // Noms: On copie ceux de A
    if (A->ntype == NONAME)
    {
        B->ntype = NUMBER;
    }
    else
    {
        B->ntype = A->ntype;
    }
    nfa_init_names(B);
    for (uint q = 0; q < B->trans->size_graph; q++)
    {
        nfa_copy_name(B, q, A, q);
    }

    // Création des transitions
    for (uint q = 0; q < B->trans->size_graph; q++)
    {
        p_vertices array[A->trans->size_alpha];
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            array[a] = A->trans->edges[q][a];
        }
        merge_array_sorted_vertices(B->trans->edges[q][0], array, A->trans->size_alpha);
    }
    return B;
}

// Calcul du MOD-kernel
p_green_sub get_mod_kernel(p_cayley M, p_green G)
{
    // Création du sous-monoide
    p_green_sub mker = init_submono(M, G);
    // Si il y a une lettre neutre, le MOD-kernel est tout le monoide
    if (M->neutlet)
    {
        mker->size = M->trans->size_graph;
        mker->rels = G;
        return mker;
    }

    // Sinon on calcule tout le MOD-kernel
    MALLOC(mker->rels, 1);

    // On utilise l'algorithme de MD-separation
    p_nfa A      = cayley_to_dfa(M);
    p_nfa U      = nfa_proj_unary(A);
    delete_nfa(A);
    p_parti SCCS = nfa_inv_ext(U);
    p_parti FOLD = nfa_stal_fold(U, SCCS);
    delete_parti(SCCS);
    p_nfa B      = nfa_dyck_ext(U, FOLD);

    delete_nfa(U);

    // Calcul des éléments du MOD-kernel;

    mker->size = 0;
    MALLOC(mker->mono_in_sub, M->trans->size_graph);
    for (uint s = 0; s < M->trans->size_graph; s++)
    {
        mker->mono_in_sub[s] = false;
    }

    for (uint i = 0; i < size_vertices(B->etrans->edges[ONE]); i++)
    {
        mker->size = mker->size + size_vertices(FOLD->cl[lefread_vertices(B->etrans->edges[ONE],
                                                                          i)]);
        for (uint j = 0; j < size_vertices(FOLD->cl[lefread_vertices(B->etrans->edges[ONE], i)]);
             j++)
        {
            uint s = lefread_vertices(FOLD->cl[lefread_vertices(B->etrans->edges[ONE], i)], j);
            mker->mono_in_sub[s] = true;
        }
    }

    MALLOC(mker->mono_to_sub, M->trans->size_graph);
    MALLOC(mker->sub_to_mono, mker->size);
    uint num = 0;

    for (uint s = 0; s < M->trans->size_graph; s++)
    {
        if (mker->mono_in_sub[s])
        {
            mker->mono_to_sub[s]   = num;
            mker->sub_to_mono[num] = s;
            num++;
        }
    }

    // Calcul des listes d'idempotents
    mker->idem_list = create_vertices();
    MALLOC(mker->idem_array, mker->size);
    for (uint s = 0; s < mker->size; s++)
    {
        if (M->idem_array[mker->sub_to_mono[s]])
        {
            mker->idem_array[s] = true;
            rigins_vertices(s, mker->idem_list);
        }
        else
        {
            mker->idem_array[s] = false;
        }
    }

    // Calcul des classes du MOD-Kernel

    // On ordonne les éléments du MOD-Kernel selon l'ordre J du monoide original
    // (utile pour avoir les R,L,J-classes du Kernel dans un ordre topologique)
    uint jord[mker->size];
    num = 0;
    // p_vertices jord = create_vertices();
    for (uint cjo = 0; cjo < G->JCL->size_par; cjo++)
    {
        for (uint i = 0; i < size_vertices(G->JCL->cl[cjo]); i++)
        {
            if (mker->mono_in_sub[lefread_vertices(G->JCL->cl[cjo], i)])
            {
                jord[num] = mker->mono_to_sub[lefread_vertices(G->JCL->cl[cjo], i)];
                num++;
            }
        }
    }
    delete_nfa(B);

    // Calcul des R-classes
    // Ce sont les classes de FOLD incluses dans le MOD-kernel

    MALLOC(mker->rels->RCL, 1);
    mker->rels->RCL->size_set = mker->size;
    MALLOC(mker->rels->RCL->numcl, mker->size);

    bool done[mker->size];

    for (uint s = 0; s < mker->size; s++)
    {
        done[s] = false;
    }
    num = 0;

    for (uint i = 0; i < mker->size; i++)
    {
        // On prend le ième élément dans l'ordre donné par les J-classes
        if (!done[jord[i]])
        {
            // Le numéro de l'élément correspondant dans le monoide original
            uint ms = mker->sub_to_mono[jord[i]];

            // Construction de la R-classe de jord[i] (il s'agit de la
            // FOLD-class de ms)
            uint c = FOLD->numcl[ms];
            for (uint j = 0; j < size_vertices(FOLD->cl[c]); j++)
            {
                uint r = mker->mono_to_sub[lefread_vertices(FOLD->cl[c], j)];
                done[r]                   = true;
                mker->rels->RCL->numcl[r] = num;
            }
            num++;
        }
    }
    mker->rels->RCL->size_par = num;
    MALLOC(mker->rels->RCL->cl, num);

    for (uint i = 0; i < num; i++)
    {
        mker->rels->RCL->cl[i] = create_vertices();
    }
    for (uint s = 0; s < mker->size; s++)
    {
        rigins_vertices(s, mker->rels->RCL->cl[mker->rels->RCL->numcl[s]]);
    }

    delete_parti(FOLD);

    // Calcul des L-classes

    A    = left_cayley_to_dfa(M);
    U    = nfa_proj_unary(A);
    delete_nfa(A);
    SCCS = nfa_inv_ext(U);
    FOLD = nfa_stal_fold(U, SCCS);
    delete_parti(SCCS);
    delete_nfa(U);

    MALLOC(mker->rels->LCL, 1);
    mker->rels->LCL->size_set = mker->size;
    MALLOC(mker->rels->LCL->numcl, mker->size);

    for (uint s = 0; s < mker->size; s++)
    {
        done[s] = false;
    }
    num = 0;

    for (uint i = 0; i < mker->size; i++)
    {
        // On prend le ième élément dans l'ordre donné par les J-classes
        if (!done[jord[i]])
        {
            // Le numéro de l'élément correspondant dans le monoide original
            uint ms = mker->sub_to_mono[jord[i]];

            // Construction de la L-classe de jord[i]
            uint c = FOLD->numcl[ms];
            for (uint j = 0; j < size_vertices(FOLD->cl[c]); j++)
            {
                uint r = mker->mono_to_sub[lefread_vertices(FOLD->cl[c], j)];
                done[r]                   = true;
                mker->rels->LCL->numcl[r] = num;
            }
            num++;
        }
    }
    mker->rels->LCL->size_par = num;
    MALLOC(mker->rels->LCL->cl, num);

    for (uint i = 0; i < num; i++)
    {
        mker->rels->LCL->cl[i] = create_vertices();
    }
    for (uint s = 0; s < mker->size; s++)
    {
        rigins_vertices(s, mker->rels->LCL->cl[mker->rels->LCL->numcl[s]]);
    }

    delete_parti(FOLD);

    // Calcul des J-classes

    // Création de la partition en J-classes
    MALLOC(mker->rels->JCL, 1);
    mker->rels->JCL->size_set = mker->size;
    MALLOC(mker->rels->JCL->numcl, mker->size);

    for (uint s = 0; s < mker->size; s++)
    {
        done[s] = false;
    }
    num = 0;

    for (uint i = 0; i < mker->size; i++)
    {
        if (!done[jord[i]])
        {
            uint cr = mker->rels->RCL->numcl[jord[i]];
            for (uint j = 0; j < size_vertices(mker->rels->RCL->cl[cr]); j++)
            {
                uint s = lefread_vertices(mker->rels->RCL->cl[cr], j);
                if (!done[s])
                {
                    uint cl = mker->rels->LCL->numcl[s];
                    for (uint k = 0; k < size_vertices(mker->rels->LCL->cl[cl]); k++)
                    {
                        mker->rels->JCL->numcl[lefread_vertices(mker->rels->LCL->cl[cl], k)] = num;
                        done[lefread_vertices(mker->rels->LCL->cl[cl], k)]                   = true;
                    }
                }
            }
            num++;
        }
    }

    mker->rels->JCL->size_par = num;
    MALLOC(mker->rels->JCL->cl, num);
    for (uint i = 0; i < num; i++)
    {
        mker->rels->JCL->cl[i] = create_vertices();
    }
    for (uint s = 0; s < mker->size; s++)
    {
        rigins_vertices(s, mker->rels->JCL->cl[mker->rels->JCL->numcl[s]]);
    }

    // Création des H-classes
    h_green_compute(mker->rels);

    // Il reste à remplir les listes des H-classes qui sont des groupes et des
    // J-classes régulières
    gr_green_compute(mker->idem_list, mker->rels);
    // mker->rels->->group_list = create_vertices();
    // mker->rels->group_list = create_vertices();

    //// Pour chaque H-classe
    // for (uint c = 0; c < mker->rels->HCL->size_par; c++)
    // {
    //// On prend un élément de la H-classe
    // uint s = lefread_vertices(mker->rels->HCL->cl[c], 0);
    //// Si ss H s, alors la H-class c est un groupe

    // if (mker->rels->HCL->numcl[mker->mono_to_sub[cayley_mult(M,
    // mker->sub_to_mono[s], mker->sub_to_mono[s])]] ==
    // mker->rels->HCL->numcl[s])
    // {
    //// On insère donc c dans la liste des groupes
    // rigins_vertices(c, mker->rels->group_list);

    //// On sait également que la J-classe de s est régulière, on l'insère donc
    // dans la liste si ce n'est pas déjà fait
    // if (isempty_vertices(mker->rels->->group_list) ||
    // rigread_vertices(mker->rels->->group_list, 0) !=
    // mker->rels->JCL->numcl[s])
    // {
    // rigins_vertices(mker->rels->JCL->numcl[s], mker->rels->->group_list);
    // }
    // }
    // }
    return mker;
}

// Calcul d'une MODPlus-orbite d'un morphisme (le MOD-kernel doit être passé en
// entrée)
p_green_sub get_one_modporb(p_green_sub mker, uint e)
{
    // On prend le morphisme original et ses relations de green
    p_cayley M = mker->original;
    p_green G  = mker->originalrels;

    // Si e est l'élément neutre et qu'il a un antécédent non-vide
    // Alors son orbite est le MOD-kernel entier
    if (e == ONE && cayley_neutstrict(M, G))
    {
        return mker;
    }

    p_green_sub S = init_submono(M, G);

    // Calcul de l'ensemble des éléments (le calcul des idéaux est restreint aux
    // éléments dans le MD-kernel)
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
        S->neut             = ONE;
    }
    else
    {
        p_vertices eM  = compute_r_ideal(M, e, mker->mono_in_sub);
        p_vertices Me  = compute_l_ideal(M, e, mker->mono_in_sub);
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
            S->sub_to_mono[i]                        = lefread_vertices(eMe, i);
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

    // Calcul des relations de Green (restriction de celles du MOD-kernel)
    MALLOC(S->rels, 1);
    uint num;

    // R-classes
    MALLOC(S->rels->RCL, 1);
    S->rels->RCL->size_set = S->size;
    MALLOC(S->rels->RCL->numcl, S->size);
    num                    = 0;
    for (uint i = 0; i < mker->rels->RCL->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(mker->rels->RCL->cl[i]); j++)
        {
            if (S->mono_in_sub[mker->sub_to_mono[lefread_vertices(mker->rels->RCL->cl[i], j)]])
            {
                inter
                                                                               =
                        true;
                S->rels->RCL->numcl[S->mono_to_sub[mker->sub_to_mono[lefread_vertices(
                                                                         mker->rels->RCL->cl[i],
                                                                         j)]]] = num;
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
    num                    = 0;
    for (uint i = 0; i < mker->rels->LCL->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(mker->rels->LCL->cl[i]); j++)
        {
            if (S->mono_in_sub[mker->sub_to_mono[lefread_vertices(mker->rels->LCL->cl[i], j)]])
            {
                inter
                                                                               =
                        true;
                S->rels->LCL->numcl[S->mono_to_sub[mker->sub_to_mono[lefread_vertices(
                                                                         mker->rels->LCL->cl[i],
                                                                         j)]]] = num;
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
    num                    = 0;
    for (uint i = 0; i < mker->rels->JCL->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(mker->rels->JCL->cl[i]); j++)
        {
            if (S->mono_in_sub[mker->sub_to_mono[lefread_vertices(mker->rels->JCL->cl[i], j)]])
            {
                inter
                                                                               =
                        true;
                S->rels->JCL->numcl[S->mono_to_sub[mker->sub_to_mono[lefread_vertices(
                                                                         mker->rels->JCL->cl[i],
                                                                         j)]]] = num;
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
    num                    = 0;
    for (uint i = 0; i < mker->rels->HCL->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(mker->rels->HCL->cl[i]); j++)
        {
            if (S->mono_in_sub[mker->sub_to_mono[lefread_vertices(mker->rels->HCL->cl[i], j)]])
            {
                inter
                                                                               =
                        true;
                S->rels->HCL->numcl[S->mono_to_sub[mker->sub_to_mono[lefread_vertices(
                                                                         mker->rels->HCL->cl[i],
                                                                         j)]]] = num;
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

// Calcul de toutes les  MODPlus-orbites d'un morphisme (le MOD-kernel doit être
// passé en entrée)
p_orbits get_all_modporbs(p_green_sub mker)
{
    p_orbits res = init_orbits(mker);

    // Si le neutre a un antécédent non-vide, toutes les MODP-orbites
    // sont des sous-semigroupes de celle de 1 (qui est le MOD-kernel entier).
    // On n'effectue donc le calcul que dans le cas contraire
    if (!cayley_neutstrict(mker->original, mker->originalrels))
    {
        res->oneonly = false;
        MALLOC(res->orbits, res->nb_idems);
        for (uint i = 0; i < res->nb_idems; i++)
        {
            uint e = lefread_vertices(mker->original->idem_list, i);
            res->orbits[i] = get_one_modporb(mker, e);
        }
    }
    return res;
}

// Affiche toutes les orbites enregistrées
void print_all_modporbs(p_orbits L, bool title, FILE *out)
{
    if (title)
    {
        print_title_box(0, true, out, 1, "MOD⁺-orbits of the morphism.");
    }

    fprintf(out, "#### The list of all idempotents:\n");
    cayley_print_idems(L->sub->original, out);

    if (L->oneonly)
    {
        fprintf(out,
                "#### Since the neutral element has a nonempty antecedent all DD-orbits are\n");
        fprintf(out, "     subsemigroups of the MOD⁺-orbit of 1 (which is the full MOD-kernel).\n");
        print_sep_line(100, out);
        fprintf(out, "#### MOD⁺-orbit of the idempotent 1:");
        print_full_green_sub(L->sub, false, out);
        return;
    }
    else
    {
        fprintf(out, "#### Printing the MOD⁺-orbits of all idempotents:\n");
        for (uint i = 0; i < L->nb_idems; i++)
        {
            print_sep_line(100, out);
            fprintf(out, "#### MOD⁺-orbit of the idempotent ");
            cayley_print_name(L->sub->original, lefread_vertices(L->sub->original->idem_list, i),
                              out);
            printf(".\n");
            print_full_green_sub(L->orbits[i], false, out);
        }
    }
    return;
}

// Procédure de séparation complète
bool decid_mod_sep(p_nfa I1, p_nfa I2, bool details, FILE *out)
{
    // Gestion des cas triviaux
    if (isempty_vertices(I1->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_vertices(I2->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (I1 == I2)
    {
        if (out && details)
        {
            fprintf(out, "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }
    if (out)
    {
        fprintf(out, "#### Using the reduction to GR-separation (over a unary alphabet).\n\n");
    }

    // Projection sur un alphabet unaire
    p_nfa U1 = nfa_proj_unary(I1);
    p_nfa U2 = nfa_proj_unary(I2);

    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out,
                "#### Reduction to GR-separation: projection of the two inputs on a unary alphabet.\n");
        fprintf(out, "#### Projection of the first input language:\n");
        view_nfa(U1);
        fprintf(out, "#### Projection of the second input language:\n");
        view_nfa(U2);
        fprintf(out,
                "#### Now applying the GR-separation algorithm based on inverse extension to the resulting languages.\n\n");
    }

    // Phase 1: Calcul des transitions inverses
    p_parti SCCS1 = nfa_inv_ext(U1);
    p_parti SCCS2 = nfa_inv_ext(U2);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out,
                "#### Phase 1: adding the inverse transitions inside the strongly connected components.\n");
        fprintf(out, "#### NFA of the first input language extended with inverse transitions.\n");
        view_nfa(U1);
        fprintf(out, "#### NFA of the second input language extended with inverse transitions.\n");
        view_nfa(U2);
    }

    // Phase 2: calcul des transitions induites par les mots de Dyck puis
    // éliminations de celles-ci
    p_parti FOLD1 = nfa_stal_fold(U1, SCCS1);
    delete_parti(SCCS1);
    p_nfa A1      = nfa_dyck_ext(U1, FOLD1);
    delete_parti(FOLD1);
    delete_nfa(U1);
    nfa_elimeps_mod(A1);
    p_parti FOLD2 = nfa_stal_fold(U2, SCCS2);
    delete_parti(SCCS2);
    p_nfa A2      = nfa_dyck_ext(U2, FOLD2);
    delete_parti(FOLD2);
    delete_nfa(U2);
    nfa_elimeps_mod(A2);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out,
                "\n****************** Phase 2: Computing the epsilon transitions induced by Dyck words and eliminating them.\n");
        fprintf(out, "#### Construction on the NFA of the first input language.\n");
        view_nfa(A1);
        fprintf(out, "#### Construction on the NFA of the second input language.\n");
        view_nfa(A2);
    }

    // Phase 3: Calcul de l'intersection

    p_nfa INTERSECT = nfa_intersect(A1, A2, true);
    delete_nfa(A1);
    delete_nfa(A2);
    if (out && details)
    {
        print_sep_line(100, out);
        fprintf(out,
                "\n****************** Phase 3: Computing the intersection of the resulting NFAs.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_vertices(INTERSECT->finals))
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        delete_nfa(INTERSECT);
        return true;
    }
    else
    {
        if (out && details)
        {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        delete_nfa(INTERSECT);
        return false;
    }
}

/*****************************************/
/* Séparation pour les langages triviaux */
/*****************************************/

// Procédure de séparation complète
bool decid_st_sep(p_nfa I1, p_nfa I2, FILE *out)
{
    if (out)
    {
        fprintf(out,
                "#### The input languages are ST-separable if and only if one of them is empty.\n\n");
    }
    // Gestion des cas triviaux
    if (isempty_vertices(I1->finals))
    {
        if (out)
        {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_vertices(I2->finals))
    {
        if (out)
        {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (out)
    {
        fprintf(out, "#### Both inputs languages are nonempty.\n");
    }
    return false;
}

// Commence par éliminer les éventuelles transitions epsilon.
// Puis, double toute transition existante d'une transition epsilon,
// dans un nouveau NFA.
p_nfa nfa_add_eps(p_nfa A)
{
    // Si il y a des transitions inverses, on les supprime
    if (A->itrans != NULL)
    {
        delete_lgraph(A->itrans);
        A->itrans = NULL;
    }

    init_epstrans(A);

    // Nouveau graphe des transitions epsilon
    p_nfa B;
    MALLOC(B, 1);
    uint temp;
    B->trans    = merge_lgraphs(&temp, 1, &A->trans);
    B->initials = create_vertices();
    B->finals   = create_vertices();
    copy_vertices_right(B->initials, A->initials, 0);
    copy_vertices_right(B->finals, A->finals, 0);
    B->itrans = NULL;
    B->ntype  = NONAME;
    B->names  = NULL;
    B->etrans = create_graph_noedges(B->trans->size_graph);

    for (uint q = 0; q < B->etrans->size; q++)
    {
        p_vertices array[B->trans->size_alpha + 1];
        array[0] = A->etrans->edges[q];
        for (uint a = 0; a < B->trans->size_alpha; a++)
        {
            array[a + 1] = B->trans->edges[q][a];
        }

        merge_array_sorted_vertices(B->etrans->edges[q], array, B->trans->size_alpha + 1);
    }

    return B;
}
