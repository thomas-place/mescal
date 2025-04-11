#include "sep_group.h"

/***********************/
/*+ Inverse extension +*/
/***********************/

parti *nfa_inv_ext(nfa *A) {
    // If there are inverse transitions, we delete them.
    if (A->trans_i != NULL) {
        delete_lgraph(A->trans_i);
        A->trans_i = NULL;
    }

    // If there are epsilon transitions, we delete them.
    if (A->trans_e != NULL) {
        nfa_elimeps_mod(A);
    }

    // Calculating strongly connected components.
    parti *P = ltarjan(A->trans);

    // We can now compute the reverse transitions.
    A->trans_i = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);

    // For transition (q,a,r)
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(A->trans->edges[q][a]); i++) {
                uint r = lefread_dequeue(A->trans->edges[q][a], i);
                // If q and r are in the same SCC, we add the transition (i,a^{-1},q)
                if (P->numcl[q] == P->numcl[r]) {
                    insert_dequeue(A->trans_i->edges[r][a], q);
                }
            }
        }
    }
    // We return the partition in SCCS
    return P;
}

void nfa_remove_inv(nfa *A) {
    if (A->trans_i != NULL) {
        delete_lgraph(A->trans_i);
        A->trans_i = NULL;
    }
}

/***********************************/
/*+ Separation by group languages +*/
/***********************************/

parti *nfa_stal_fold(nfa *A, parti *sccs) {
    if (A->trans_e != NULL) {
        fprintf(stderr, "Stallings folding only works for NFAs without epsilon transitions. Returned NULL.\n");
        return NULL;
    }
    if (A->trans_i == NULL) {
        fprintf(stderr, "Stallings folding only works for NFAs whose inverse transitions have already been computed. Returned NULL.\n");
        return NULL;
    }

    // Union-find for the partition
    ufind *merge = create_ufind(A->trans->size_graph);

    // Double-linked lists for classic edges
    dlist ***ltrans;
    MALLOC(ltrans, A->trans->size_graph);

    // Double-linked lists for inverse transitions
    dlist ***litrans;
    MALLOC(litrans, A->trans->size_graph);

    // Fill lists with original transitions (only transitions internal to sccs are kept).
    for (uint q = 0; q < A->trans->size_graph; q++) {
        MALLOC(ltrans[q], A->trans->size_alpha);
        MALLOC(litrans[q], A->trans->size_alpha);
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            ltrans[q][a] = create_dlist();
            litrans[q][a] = create_dlist();
            for (uint i = 0; i < size_dequeue(A->trans->edges[q][a]); i++) {
                uint r = lefread_dequeue(A->trans->edges[q][a], i);
                if (sccs->numcl[r] == sccs->numcl[q]) {
                    insertprevious_dlist(ltrans[q][a], ltrans[q][a]->rsent, r);
                }
            }
            for (uint i = 0; i < size_dequeue(A->trans_i->edges[q][a]); i++) {
                insertprevious_dlist(litrans[q][a], litrans[q][a]->rsent, lefread_dequeue(A->trans_i->edges[q][a], i));
            }
        }
    }

    // Une file qui contient une liste de de sommets à traiter
    // Elle contient initialement tous les sommets
    dequeue *tofold = create_dequeue();
    for (uint q = 0; q < A->trans->size_graph; q++) {
        rigins_dequeue(q, tofold);
    }

    // Tant qu'il reste un sommet à traiter
    while (!isempty_dequeue(tofold)) {
        // On prend le représentant d'un sommet non-traité
        uint q = find_ufind(lefpull_dequeue(tofold), merge);

        bool folded = false;
        uint a = 0;
        while (!folded && a < A->trans->size_alpha) {
            if (ltrans[q][a]->size > 1) // Si il y a 2 arêtes sortantes étiquetées par a
            {
                // On prend les représentant des destinations des deux premières
                // arêtes qu'on va fusionner (si ça n'a pas été fait avant)
                uint r = find_ufind(ltrans[q][a]->lsent->next->val, merge);
                uint s = find_ufind(ltrans[q][a]->lsent->next->next->val, merge);
                deletecell_dlist(ltrans[q][a], ltrans[q][a]->lsent->next); // On supprime la première des deux edges
                if (r != s)                                                // Si les deux sommets adjacents n'avaient pas encore étés fusionnés
                {
                    union_ufind(r, s, merge);                       // On effectue la fusion
                    for (uint b = 0; b < A->trans->size_alpha; b++) // On concatène leur listes de sommets adjacents
                    {
                        concat_dlist(ltrans[r][b], ltrans[s][b]);
                        concat_dlist(litrans[r][b], litrans[s][b]);
                        if (find_ufind(r, merge) == r) // On ne garde que la liste du nouveau représentant de la classe
                        {
                            free(ltrans[s][b]);
                            ltrans[s][b] = NULL;
                            free(litrans[s][b]);
                            litrans[s][b] = NULL;
                        } else {
                            free(ltrans[r][b]);
                            ltrans[r][b] = NULL;
                            free(litrans[r][b]);
                            litrans[r][b] = NULL;
                        }
                    }
                    rigins_dequeue(find_ufind(r, merge), tofold); // On va devoir éventuelement traiter la nouvelle classe
                    if (find_ufind(r, merge) != find_ufind(q, merge)) {
                        rigins_dequeue(find_ufind(q, merge), tofold);
                    }
                    folded = true;
                } else {
                    rigins_dequeue(find_ufind(q, merge), tofold);
                }
            } else if (litrans[q][a]->size > 1) // Si il y a 2 arêtes sortantes étiquetées par a-1
            {
                // On prend les représentant des destinations des deux premières
                // arêtes qu'on va fusionner (si ça n'a pas été fait avant)
                uint r = find_ufind(litrans[q][a]->lsent->next->val, merge);
                uint s = find_ufind(litrans[q][a]->lsent->next->next->val, merge);
                deletecell_dlist(litrans[q][a], litrans[q][a]->lsent->next); // On supprime la première des deux edges
                if (r != s)                                                  // Si les deux sommets adjacents n'avaient pas encore étés fusionnés
                {
                    union_ufind(r, s, merge); // On
                    // effectue
                    // la
                    // fusion
                    for (uint b = 0; b < A->trans_i->size_alpha; b++) // On concatène leur listes de sommets adjacents
                    {
                        concat_dlist(ltrans[r][b], ltrans[s][b]);
                        concat_dlist(litrans[r][b], litrans[s][b]);
                        if (find_ufind(r, merge) == r) // On ne garde que la liste du nouveau représentant de la classe
                        {
                            free(ltrans[s][b]);
                            ltrans[s][b] = NULL;
                            free(litrans[s][b]);
                            litrans[s][b] = NULL;
                        } else {
                            free(ltrans[r][b]);
                            ltrans[r][b] = NULL;
                            free(litrans[r][b]);
                            litrans[r][b] = NULL;
                        }
                    }
                    rigins_dequeue(find_ufind(r, merge), tofold); // On va devoir éventuellement traiter la nouvelle classe
                    if (find_ufind(r, merge) != find_ufind(q, merge)) {
                        rigins_dequeue(find_ufind(q, merge), tofold);
                    }
                    folded = true;
                } else {
                    rigins_dequeue(find_ufind(q, merge), tofold);
                }
            }
            a++;
        }
    }

    // Cleanup
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            delete_dlist(ltrans[q][a]);
            delete_dlist(litrans[q][a]);
        }
        free(ltrans[q]);
        free(litrans[q]);
    }
    free(ltrans);
    free(litrans);
    delete_dequeue(tofold);
    parti *result = ufind_to_parti_refined(merge, sccs);
    delete_ufind(merge);
    return result;
}

// Calcul du NFA obtenu en effectuant le folding et en ajoutant ensuite
// des transitions espilon entre les paires d'états liées par un mot de
// Dyck.
// Les transitions inverses doivent déjà avoir été calculées et on doit
// fournir les partitions en SCCS et celle correspondant au folding.
nfa *nfa_dyck_ext(nfa *A, parti *FOLD) {
    if (!FOLD) {
        return NULL;
    }
    if (A->trans_e != NULL) {
        fprintf(stderr, "Adding Dyck transitions only works for NFAs without epsilon transitions. Returned NULL.\n");
        return NULL;
    }
    if (A->trans_i == NULL) {
        fprintf(stderr, "Inverse transitions must have already been computed. Returned NULL.\n");
        return NULL;
    }

    nfa *B = nfa_merge_states(A, FOLD);
    nfa *Bm = nfa_mirror(B);

    graph *eps = create_graph_noedges(B->trans->size_graph);
    graph *epsm = create_graph_noedges(B->trans->size_graph);

    dequeue *in = create_dequeue();
    dequeue *out = create_dequeue();
    for (uint i = 0; i < B->trans->size_graph; i++) {
        rigins_dequeue(i, in);
        rigins_dequeue(i, out);
    }

    while (!isempty_dequeue(in)) {
        uint q = lefpull_dequeue(in);
        uint r = lefpull_dequeue(out);
        // printf("%d %d\n", q, r);
        if (mem_dequeue_sorted(r, eps->edges[q], NULL)) {
            continue;
        }

        insert_dequeue(eps->edges[q], r);
        insert_dequeue(epsm->edges[r], q);

        // aa-1 et a-1 a
        for (uint a = 0; a < B->trans->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(Bm->trans->edges[q][a]); i++) {
                for (uint j = 0; j < size_dequeue(B->trans_i->edges[r][a]); j++) {

                    rigins_dequeue(lefread_dequeue(Bm->trans->edges[q][a], i), in);
                    rigins_dequeue(lefread_dequeue(B->trans_i->edges[r][a], j), out);
                }
            }

            for (uint i = 0; i < size_dequeue(Bm->trans_i->edges[q][a]); i++) {
                for (uint j = 0; j < size_dequeue(B->trans->edges[r][a]); j++) {
                    rigins_dequeue(lefread_dequeue(Bm->trans_i->edges[q][a], i), in);
                    rigins_dequeue(lefread_dequeue(B->trans->edges[r][a], j), out);
                }
            }
        }

        // transclos
        for (uint i = 0; i < size_dequeue(eps->edges[r]); i++) {
            rigins_dequeue(q, in);
            rigins_dequeue(lefread_dequeue(eps->edges[r], i), out);
        }

        for (uint i = 0; i < size_dequeue(epsm->edges[q]); i++) {
            rigins_dequeue(lefread_dequeue(epsm->edges[q], i), in);
            rigins_dequeue(r, out);
        }
    }
    delete_dequeue(in);
    delete_dequeue(out);
    delete_graph(epsm);
    B->trans_e = eps;

    return B;
}

// Procédure de séparation complète
bool decid_grp_sep(nfa *I1, nfa *I2, bool details, FILE *out) {
    // Gestion des cas triviaux
    if (isempty_dequeue(I1->finals)) {
        if (out && details) {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(I2->finals)) {
        if (out && details) {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (I1 == I2) {
        if (out && details) {
            fprintf(out, "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }
    if (out) {
        fprintf(out, "#### Using the GR-separation algorithm based on inverse extension.\n\n");
    }

    // Phase 1: Calcul des transitions inverses
    parti *SCCS1 = nfa_inv_ext(I1);
    parti *SCCS2 = nfa_inv_ext(I2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 1: adding the inverse transitions inside the strongly connected components.\n");
        fprintf(out, "#### NFA of the first input language extended with inverse transitions.\n");
        view_nfa(I1);
        fprintf(out, "#### NFA of the second input language extended with inverse transitions.\n");
        view_nfa(I2);
    }

    // Phase 2: calcul des transitions induites par les mots de Dyck puis
    // éliminations de celles-ci
    parti *FOLD1 = nfa_stal_fold(I1, SCCS1);
    delete_parti(SCCS1);
    nfa *A1 = nfa_dyck_ext(I1, FOLD1);
    delete_parti(FOLD1);
    nfa_remove_inv(I1);
    nfa_elimeps_mod(A1);
    parti *FOLD2 = nfa_stal_fold(I2, SCCS2);
    delete_parti(SCCS2);
    nfa *A2 = nfa_dyck_ext(I2, FOLD2);
    delete_parti(FOLD2);
    nfa_remove_inv(I2);
    nfa_elimeps_mod(A2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "\n****************** Phase 2: Computing the epsilon transitions induced by Dyck words and eliminating them.\n");
        fprintf(out, "#### Construction on the NFA of the first input language.\n");
        view_nfa(A1);
        fprintf(out, "#### Construction on the NFA of the second input language.\n");
        view_nfa(A2);
    }

    // Phase 3: Calcul de l'intersection

    nfa *INTERSECT = nfa_intersect(A1, A2, true);
    delete_nfa(A1);
    delete_nfa(A2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "\n****************** Phase 3: Computing the intersection of the resulting NFAs.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_dequeue(INTERSECT->finals)) {
        if (out && details) {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        delete_nfa(INTERSECT);
        return true;
    } else {
        if (out && details) {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        delete_nfa(INTERSECT);
        return false;
    }
}

/*******************************************/
/*+ Séparation par les langages de modulo +*/
/*******************************************/

nfa *nfa_proj_unary(nfa *A) {
    if (A->trans_i != NULL) {
        fprintf(stderr, "Cannot project a NFA with inverse transitions. Returned NULL.\n");
        return NULL;
    }
    // Élimination des éventuelles transitions epsilon
    nfa_elimeps_mod(A);

    // Création du nouveau NFA
    nfa *B = nfa_init();
    copy_dequeue_right(B->initials, A->initials, 0);
    copy_dequeue_right(B->finals, A->finals, 0);
    B->trans = create_lgraph_noedges(A->trans->size_graph, 1);

    B->state_names = nfa_copy_all_names(A);

    MALLOC(B->alphabet, 1);
    B->alphabet[0].lab = '$';
    B->alphabet[0].num = -1;

    // Création des transitions
    for (uint q = 0; q < B->trans->size_graph; q++) {
        dequeue *array[A->trans->size_alpha];
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            array[a] = A->trans->edges[q][a];
        }
        merge_array_sorted_dequeue(B->trans->edges[q][0], array, A->trans->size_alpha);
    }
    return B;
}

bool decid_mod_sep(nfa *I1, nfa *I2, bool details, FILE *out) {
    // Gestion des cas triviaux
    if (isempty_dequeue(I1->finals)) {
        if (out && details) {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(I2->finals)) {
        if (out && details) {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (I1 == I2) {
        if (out && details) {
            fprintf(out, "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }
    if (out) {
        fprintf(out, "#### Using the reduction to GR-separation (over a unary alphabet).\n\n");
    }

    nfa_remove_inv(I1);
    nfa_remove_inv(I2);

    // Projection sur un alphabet unaire
    nfa *U1 = nfa_proj_unary(I1);
    nfa *U2 = nfa_proj_unary(I2);

    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "#### Reduction to GR-separation: projection of the two inputs on a unary alphabet.\n");
        fprintf(out, "#### Projection of the first input language:\n");
        view_nfa(U1);
        fprintf(out, "#### Projection of the second input language:\n");
        view_nfa(U2);
        fprintf(out, "#### Now applying the GR-separation algorithm based on inverse extension to the resulting languages.\n\n");
    }

    // Phase 1: Calcul des transitions inverses
    parti *SCCS1 = nfa_inv_ext(U1);
    parti *SCCS2 = nfa_inv_ext(U2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 1: adding the inverse transitions inside the strongly connected components.\n");
        fprintf(out, "#### NFA of the first input language extended with inverse transitions.\n");
        view_nfa(U1);
        fprintf(out, "#### NFA of the second input language extended with inverse transitions.\n");
        view_nfa(U2);
    }

    // Phase 2: calcul des transitions induites par les mots de Dyck puis
    // éliminations de celles-ci
    parti *FOLD1 = nfa_stal_fold(U1, SCCS1);
    delete_parti(SCCS1);
    nfa *A1 = nfa_dyck_ext(U1, FOLD1);
    delete_parti(FOLD1);
    delete_nfa(U1);
    nfa_elimeps_mod(A1);
    parti *FOLD2 = nfa_stal_fold(U2, SCCS2);
    delete_parti(SCCS2);
    nfa *A2 = nfa_dyck_ext(U2, FOLD2);
    delete_parti(FOLD2);
    delete_nfa(U2);
    nfa_elimeps_mod(A2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "\n****************** Phase 2: Computing the epsilon transitions induced by Dyck words and eliminating them.\n");
        fprintf(out, "#### Construction on the NFA of the first input language.\n");
        view_nfa(A1);
        fprintf(out, "#### Construction on the NFA of the second input language.\n");
        view_nfa(A2);
    }

    // Phase 3: Calcul de l'intersection

    nfa *INTERSECT = nfa_intersect(A1, A2, true);
    delete_nfa(A1);
    delete_nfa(A2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "\n****************** Phase 3: Computing the intersection of the resulting NFAs.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_dequeue(INTERSECT->finals)) {
        if (out && details) {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        delete_nfa(INTERSECT);
        return true;
    } else {
        if (out && details) {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        delete_nfa(INTERSECT);
        return false;
    }
}

/******************************************/
/*+ Séparation par les langages triviaux +*/
/******************************************/

bool decid_st_sep(nfa *I1, nfa *I2, FILE *out) {
    if (out) {
        fprintf(out, "#### The input languages are ST-separable if and only if one of them is empty.\n\n");
    }
    // Gestion des cas triviaux
    if (isempty_dequeue(I1->finals)) {
        if (out) {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(I2->finals)) {
        if (out) {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (out) {
        fprintf(out, "#### Both inputs languages are nonempty.\n");
    }
    return false;
}
