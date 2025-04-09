#include "nfa_minimization.h"

/***************/
/*+ Auxiliary +*/
/***************/

nfa* nfa_mini_canonical_copy(nfa* A) {
    if (!A) {
        return NULL;
    }

    // Computation of the canonical ordering on the states
    int map[A->trans->size_graph];
    int imap[A->trans->size_graph];
    for (uint i = 0; i < A->trans->size_graph;i++) {
        map[i] = -1;
    }
    dequeue* queue = create_dequeue();
    rigins_dequeue(lefread_dequeue(A->initials, 0), queue);
    uint count = 0;

    while (!isempty_dequeue(queue)) {
        uint q = lefpull_dequeue(queue);
        if (map[q] != -1) {
            continue;
        }
        map[q] = count;
        imap[count] = q;
        count++;
        for (uint a = 0; a < A->trans->size_alpha;a++) {
            rigins_dequeue(lefread_dequeue(A->trans->edges[q][a], 0), queue);
        }
    }
    delete_dequeue(queue);

    nfa* B = nfa_init();
    B->alphabet = nfa_duplicate_alpha(A);
    B->trans = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
    for (uint q = 0; q < A->trans->size_graph;q++) {
        for (uint a = 0; a < A->trans->size_alpha;a++) {
            rigins_dequeue(map[lefread_dequeue(A->trans->edges[imap[q]][a], 0)], B->trans->edges[q][a]);
        }
    }

    rigins_dequeue(0, B->initials);
    for (uint i = 0; i < size_dequeue(A->finals);i++) {
        rigins_dequeue(map[lefread_dequeue(A->finals, i)], B->finals);
    }
    return B;

}


/******************************/
/*+ Algorithme de Brzozowski +*/
/******************************/

nfa* nfa_brzozowski(nfa* A) {
    nfa* B = nfa_mirror(A);
    nfa* C = nfa_determinize(B, false);
    delete_nfa(B);
    B = nfa_mirror(C);
    delete_nfa(C);
    C = nfa_determinize(B, false);
    delete_nfa(B);
    return C;
}

/****************************/
/*+ Algorithme de Hopcroft +*/
/****************************/



static dequeue* nfa_create_hopcroft_candidate(uint* array, uint l, uint r) {
    dequeue* new = create_dequeue();
    for (uint i = l; i < r;i++) {
        rigins_dequeue(array[i], new);
    }
    return new;
}

hopcroft_partition* nfa_hopcroft_initial(uint size_auto, dequeue* finals) {
    if (isempty_dequeue(finals) || size_dequeue(finals) == size_auto) {
        fprintf(stderr, "Error in Hopcroft's algorithm: should not compute a partition when the set of final states is trivial.\n");
        return NULL;
    }

    hopcroft_partition* new;
    MALLOC(new, 1);
    new->size_set = size_auto;
    new->size_par = 2;
    MALLOC(new->classes, new->size_set);
    MALLOC(new->parray, new->size_set);
    MALLOC(new->parray_i, new->size_set);
    MALLOC(new->lindex, new->size_set);
    MALLOC(new->rindex, new->size_set);

    uint i = 0;
    uint l = 0;
    uint r = new->size_set - 1;

    // Pour chaque état (pris dans l'ordre)
    for (uint q = 0; q < new->size_set;q++) {

        // Si cet état est final
        if (i < size_dequeue(finals) && lefread_dequeue(finals, i) == q) {
            new->parray[l] = q;
            new->parray_i[q] = l;
            new->classes[q] = 0;
            l++;
            i++;
        }
        else {
            new->parray[r] = q;
            new->parray_i[q] = r;
            new->classes[q] = 1;
            r--;
        }

    }

    new->lindex[0] = 0;
    new->rindex[0] = l;
    new->lindex[1] = l;
    new->rindex[1] = new->size_set;
    return new;
}

nfa* nfa_hopcroft_genauto(nfa* D, hopcroft_partition* p) {
    if (!p || !D) {
        return NULL;
    }

    nfa* MINI;
    MALLOC(MINI, 1);
    MINI->alphabet = nfa_duplicate_alpha(D);
    MINI->state_names = NULL;

    MINI->trans = create_lgraph_noedges(p->size_par, D->trans->size_alpha);

    for (uint cq = 0; cq < p->size_par;cq++) {
        for (uint a = 0; a < D->trans->size_alpha;a++) {
            uint q = p->parray[p->lindex[cq]];
            rigins_dequeue(p->classes[lefread_dequeue(D->trans->edges[q][a], 0)], MINI->trans->edges[cq][a]);
        }
    }

    MINI->initials = create_dequeue();
    rigins_dequeue(p->classes[lefread_dequeue(D->initials, 0)], MINI->initials);


    bool* finals;
    CALLOC(finals, p->size_par);
    for (uint i = 0; i < size_dequeue(D->finals);i++) {
        uint q = lefread_dequeue(D->finals, i);
        finals[p->classes[q]] = true;
    }
    MINI->finals = create_dequeue();
    for (uint cq = 0; cq < p->size_par;cq++) {
        if (finals[cq]) {
            rigins_dequeue(cq, MINI->finals);
        }
    }
    free(finals);
    return MINI;
}


void nfa_hopcroft_free(hopcroft_partition* p) {
    if (!p) {
        return;
    }
    free(p->classes);
    free(p->parray);
    free(p->parray_i);
    free(p->lindex);
    free(p->rindex);
    free(p);
    return;
}


/* static void print_class(hopcroft_partition* p, uint c) {
    printf("class: %d\n", c);
    printf("elements: ");
    for (uint k = p->lindex[c]; k < p->rindex[c];k++) {
        printf("%2d ", p->parray[k]);
    }
    printf("\n");
} */


nfa* nfa_hopcroft(nfa* A) {
    if (!A) {
        return NULL;
    }
    // Calcule de l'automate déterminisé complet
    A = nfa_determinize(A, false);
    //view_nfa(A);

    // Traitement du cas où l'ensemble des états finaux est trivial (on retourne un automate trivial).
    if (isempty_dequeue(A->finals) || size_dequeue(A->finals) == A->trans->size_graph) {
        nfa* MINI;
        MALLOC(MINI, 1);
        MINI->state_names = NULL;
        MINI->alphabet = nfa_duplicate_alpha(A);
        MINI->trans = create_lgraph_noedges(1, A->trans->size_alpha);
        for (uint a = 0; a < A->trans->size_alpha;a++) {
            rigins_dequeue(0, MINI->trans->edges[0][a]);
        }
        MINI->initials = create_dequeue();
        rigins_dequeue(0, MINI->initials);
        MINI->finals = create_dequeue();
        if (!isempty_dequeue(A->finals)) {
            rigins_dequeue(0, MINI->finals);
        }
        delete_nfa(A);
        return MINI;
    }

    // On sait maintenant que l'ensemble des états finaux est non-trivial.
    // Calcul de la partition initiale
    hopcroft_partition* p = nfa_hopcroft_initial(A->trans->size_graph, A->finals);
    // Calcul du miroir de l'automate (utile pour l'algorithme)
    nfa* MA = nfa_mirror(A);

    // Création de la pile des classes à traiter (on empile la plus petite des deux)
    dequeue_gen* thestack = create_dequeue_gen();
    if (p->rindex[0] - p->lindex[0] < p->rindex[1] - p->lindex[1]) {
        rigins_dequeue_gen(nfa_create_hopcroft_candidate(p->parray, p->lindex[0], p->rindex[0]), thestack);
    }
    else {
        rigins_dequeue_gen(nfa_create_hopcroft_candidate(p->parray, p->lindex[1], p->rindex[1]), thestack);
    }

    // Création d'une table temporaire des indices droits
    uint* rtemp;
    MALLOC(rtemp, p->size_set);
    rtemp[0] = p->rindex[0];
    rtemp[1] = p->rindex[1];

    // Boucle principale
    while (!isempty_dequeue_gen(thestack)) {

        // On prend une classe en haut de la pile
        dequeue* cand = (dequeue*)rigpull_dequeue_gen(thestack);




        for (uint a = 0; a < A->trans->size_alpha;a++) {
            dequeue* visited = create_dequeue();
            // Pour chaque antécédent r de la classe c par la lettre a (c'est ici que set le miroir)
            for (uint i = 0; i < size_dequeue(cand);i++) {
                for (uint j = 0; j < size_dequeue(MA->trans->edges[lefread_dequeue(cand, i)][a]);j++) {


                    // L'antécédent
                    uint r = lefread_dequeue(MA->trans->edges[lefread_dequeue(cand, i)][a], j);


                    // La classe de r
                    uint cr = p->classes[r];


                    // Si on n'a pas encore traité la classe cr, on la marque comme visitée 
                    if (rtemp[cr] == p->rindex[cr]) {
                        rigins_dequeue(cr, visited);
                    }

                    // On swap r avec l'élément à la fin du tableau des éléments
                    uint q = p->parray[rtemp[cr] - 1];

                    p->parray[p->parray_i[r]] = q;
                    p->parray_i[q] = p->parray_i[r];

                    p->parray[rtemp[cr] - 1] = r;
                    p->parray_i[r] = rtemp[cr] - 1;

                    // On décrémente le marqueur de fin de la classe
                    rtemp[cr]--;
                }
            }


            // Mise à jour des classes visitées (si nécessaire)
            for (uint i = 0; i < size_dequeue(visited);i++) {
                // La classe visitée
                uint d = lefread_dequeue(visited, i);
                // Si tous les éléments de d étaient envoyés vers c par la lettre a la classe ne doit pas être divisée
                if (rtemp[d] == p->lindex[d]) {
                    // On réinitialise rtemp[d] pour le cycle suivant
                    rtemp[d] = p->rindex[d];
                    continue;
                }


                // Sinon, on créé un nouvelle classe dans laquelle on met une partie des éléments de d (son numéro est la taille de la partition actuelle)
                uint nd = p->size_par;
                p->size_par++;
                p->lindex[nd] = rtemp[d];
                p->rindex[nd] = p->rindex[d];
                rtemp[nd] = p->rindex[nd];
                p->rindex[d] = p->lindex[nd];

                // Mise à jour du tableau des classes
                for (uint j = p->lindex[nd]; j < p->rindex[nd];j++) {
                    p->classes[p->parray[j]] = nd;
                }

                // On ajoute une des deux classes (la plus petite) à la pile. 
                if (p->rindex[nd] - p->lindex[nd] < p->rindex[d] - p->lindex[d]) {
                    rigins_dequeue_gen(nfa_create_hopcroft_candidate(p->parray, p->lindex[d], p->rindex[d]), thestack);
                }
                else {
                    rigins_dequeue_gen(nfa_create_hopcroft_candidate(p->parray, p->lindex[nd], p->rindex[nd]), thestack);
                }


            }
            delete_dequeue(visited);
        }
        delete_dequeue(cand);

    }

    free(rtemp);
    delete_dequeue_gen(thestack);

    nfa* MINI = nfa_hopcroft_genauto(A, p);

    delete_nfa(A);
    delete_nfa(MA);
    nfa_hopcroft_free(p);
    return MINI;
}
