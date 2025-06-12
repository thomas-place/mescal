#include "nfa_minimization.h"

/***************/
/*+ Auxiliary +*/
/***************/

dfa* dfa_mini_canonical_copy(dfa* A) {
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
    rigins_dequeue(A->initial, queue);
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
            rigins_dequeue(A->trans->edges[q][a], queue);
        }
    }
    delete_dequeue(queue);

    dfa* B;
    CALLOC(B, 1);
    B->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    B->trans = create_dgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
    for (uint q = 0; q < A->trans->size_graph;q++) {
        for (uint a = 0; a < A->trans->size_alpha;a++) {
            B->trans->edges[q][a] = map[A->trans->edges[imap[q]][a]];
        }
    }
    B->initial = 0;
    B->nb_finals = A->nb_finals;
    MALLOC(B->finals, A->nb_finals);
    for (uint i = 0; i < A->nb_finals;i++) {
        B->finals[i] = map[A->finals[i]];
    }
    return B;

}


/******************************/
/*+ Algorithme de Brzozowski +*/
/******************************/

dfa* nfa_brzozowski(nfa* A) {
    nfa* B = nfa_mirror(A);
    dfa* C = nfa_determinize(B, false);
    nfa_delete(B);
    B = dfa_mirror(C);
    dfa_delete(C);
    C = nfa_determinize(B, false);
    nfa_delete(B);
    return C;
}

dfa* dfa_brzozowski(dfa* A) {
    nfa* B = dfa_mirror(A);
    dfa* C = nfa_determinize(B, false);
    nfa_delete(B);
    B = dfa_mirror(C);
    dfa_delete(C);
    C = nfa_determinize(B, false);
    nfa_delete(B);
    return C;
}


/****************************/
/*+ Algorithme de Hopcroft +*/
/****************************/



static dequeue* dfa_create_hopcroft_candidate(uint* array, uint l, uint r) {
    dequeue* new = create_dequeue();
    for (uint i = l; i < r;i++) {
        rigins_dequeue(array[i], new);
    }
    return new;
}

hopcroft_partition* dfa_hopcroft_initial(uint size_auto, uint* finals, uint nb_finals) {
    if (nb_finals == 0 || nb_finals == size_auto) {
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
        if (i < nb_finals && finals[i] == q) {
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

dfa* dfa_hopcroft_genauto(dfa* D, hopcroft_partition* p) {
    if (!p || !D) {
        return NULL;
    }

    //Computation of the final states
    bool* finals;
    uint nb_finals = 0;
    CALLOC(finals, p->size_par);
    for (uint i = 0; i < D->nb_finals;i++) {
        finals[p->classes[D->finals[i]]] = true;
        nb_finals++;
    }


    dfa* MINI;
    CALLOC(MINI, 1);
    MINI->alphabet = duplicate_alphabet(D->alphabet, D->trans->size_alpha);
    MINI->trans = create_dgraph_noedges(p->size_par, D->trans->size_alpha);

    for (uint cq = 0; cq < p->size_par;cq++) {
        for (uint a = 0; a < D->trans->size_alpha;a++) {
            uint q = p->parray[p->lindex[cq]];
            MINI->trans->edges[cq][a] = p->classes[D->trans->edges[q][a]];
        }
    }
    MINI->initial = p->classes[D->initial];

    MALLOC(MINI->finals, nb_finals);
    MINI->nb_finals = nb_finals;
    uint j = 0;
    for (uint cq = 0; cq < p->size_par;cq++) {
        if (finals[cq]) {
            MINI->finals[j] = cq;
            j++;
        }
    }
    free(finals);
    return MINI;
}


void dfa_hopcroft_free(hopcroft_partition* p) {
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


dfa* dfa_hopcroft(dfa* A) {
    if (!A) {
        return NULL;
    }

    A = dfa_trim(A);
    // Traitement du cas où l'ensemble des états finaux est trivial (on retourne un automate trivial).
    if (A->nb_finals == 0 || A->nb_finals == A->trans->size_graph) {
        dfa* MINI;
        CALLOC(MINI, 1);
        MINI->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
        MINI->trans = create_dgraph_noedges(1, A->trans->size_alpha);
        MINI->initial = 0;
        for (uint a = 0; a < A->trans->size_alpha;a++) {
            MINI->trans->edges[0][a] = 0;
        }
        if (A->nb_finals > 0) {
            MALLOC(MINI->finals, 1);
            MINI->nb_finals = 1;
            MINI->finals[0] = 0;
        }
        else {
            MINI->nb_finals = 0;
            MINI->finals = NULL;
        }
        dfa_delete(A);
        return MINI;
    }

    // On sait maintenant que l'ensemble des états finaux est non-trivial.
    // Calcul de la partition initiale
    hopcroft_partition* p = dfa_hopcroft_initial(A->trans->size_graph, A->finals, A->nb_finals);
    // Calcul du miroir de l'automate (utile pour l'algorithme)
    nfa* MA = dfa_mirror(A);

    // Création de la pile des classes à traiter (on empile la plus petite des deux)
    dequeue_gen* thestack = create_dequeue_gen();
    if (p->rindex[0] - p->lindex[0] < p->rindex[1] - p->lindex[1]) {
        rigins_dequeue_gen(dfa_create_hopcroft_candidate(p->parray, p->lindex[0], p->rindex[0]), thestack);
    }
    else {
        rigins_dequeue_gen(dfa_create_hopcroft_candidate(p->parray, p->lindex[1], p->rindex[1]), thestack);
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
                    rigins_dequeue_gen(dfa_create_hopcroft_candidate(p->parray, p->lindex[d], p->rindex[d]), thestack);
                }
                else {
                    rigins_dequeue_gen(dfa_create_hopcroft_candidate(p->parray, p->lindex[nd], p->rindex[nd]), thestack);
                }


            }
            delete_dequeue(visited);
        }
        delete_dequeue(cand);

    }

    free(rtemp);
    delete_dequeue_gen(thestack);

    dfa* MINI = dfa_hopcroft_genauto(A, p);

    dfa_delete(A);
    nfa_delete(MA);
    dfa_hopcroft_free(p);
    return MINI;
}


/************************/
/** Canonical ordering **/
/************************/

bool** dfa_mini_canonical_ordering(dfa* A) {
    if (!A) {
        return NULL;
    }


    uint thesize = A->trans->size_graph;

    // Array that marks the visited pairs.
    bool** visited;
    MALLOC(visited, thesize);
    for (uint i = 0; i < thesize; i++) {
        CALLOC(visited[i], thesize);
    }

    // Stacks for the DFS which computes all pairs incomparable of states
    // A pair (q,r) is incomparable if q is NOT smaller than r for the canonical order.
    // First stack stores element 1 in the pair, second stack stores element 2 in the pair.
    dequeue* stack_one = create_dequeue();
    dequeue* stack_two = create_dequeue();


    // We push the starting pairs (final, non-final) which are clearly incomparable.
    for (uint i = 0; i < A->nb_finals; i++)
    {
        uint j = 0;
        for (uint q = 0; q < thesize; q++) {
            // We skip q if it is a final state.
            if (j < A->nb_finals && A->finals[j] == q) {
                j++;
                continue;
            }
            rigins_dequeue(A->finals[i], stack_one);
            rigins_dequeue(q, stack_two);
        }
    }


    // Computation of the mirror automaton (used in the DFS).
    lgraph* mirror = dgraph_mirror(A->trans);

    // The DFS
    while (!isempty_dequeue(stack_one)) {

        // We pop a pair (q, r) from the stacks.
        uint q = rigpull_dequeue(stack_one);
        uint r = rigpull_dequeue(stack_two);

        // We skip the pair if it has already been visited.
        if (visited[q][r]) {
            continue;
        }

        // We mark the pair as visited.
        visited[q][r] = true;


        // We push all pairs from which we can reach (q, r) in either the left or right Cayley graph
        // These pairs are also incomparable.
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(mirror->edges[q][a]); i++) {
                for (uint j = 0; j < size_dequeue(mirror->edges[r][a]); j++) {
                    rigins_dequeue(lefread_dequeue(mirror->edges[q][a], i), stack_one);
                    rigins_dequeue(lefread_dequeue(mirror->edges[r][a], j), stack_two);
                }
            }
        }
    }

    // We may now inverse the visted array to get the canonical order.
    for (uint q = 0; q < thesize; q++) {
        for (uint r = 0; r < thesize; r++) {
            visited[q][r] = !visited[q][r];
        }
    }


    delete_dequeue(stack_one);
    delete_dequeue(stack_two);
    delete_lgraph(mirror);
    return visited;
}
