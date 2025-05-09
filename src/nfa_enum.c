#include "nfa_enum.h"

nfa_enum* nfa_enum_init(short states, short alpha) {
    nfa_enum* A;
    CALLOC(A, 1);
    A->states = states;
    A->alpha = alpha;
    MALLOC(A->graph, states);
    MALLOC(A->used, states);
    MALLOC(A->outlabs, states);
    CALLOC(A->parti, states);
    CALLOC(A->parsize, states);
    for (short q = 0; q < states; q++) {
        CALLOC(A->graph[q], alpha);
        CALLOC(A->parti[q], alpha);
        A->used[q] = 1;
        A->outlabs[q] = 0;
        for (short a = 0; a < alpha; a++) {
            A->graph[q][a] = -1;
            A->parti[q][a] = 0;
        }
    }
    A->final = 0;
    A->inisep = 0;
    return A;
}

void nfa_enum_free(nfa_enum* A) {
    for (short q = 0; q < A->states; q++) {
        free(A->graph[q]);
        free(A->parti[q]);
    }
    free(A->graph);
    free(A->used);
    free(A->outlabs);
    free(A->parti);
    free(A->parsize);
    free(A);
}

bool nfa_enum_next(nfa_enum* A) {
    /* if (A->final < A->used[A->states - 1] - 1) {
        A->final++;
        return true;
    }
    else {
        A->final = 0;
    } */

    for (short i = A->states - 1; i > 0; i--) {
        if (A->used[i] < i + 1) {
            continue;
        }

        //printf("start %d\n", i);
        // We first look at the edges that stay in the used state space
        // If we did not enumerate all cases for them, we iterate and return.
        // Otherwise, they are all reinitialized to the sink state and we look for another way to iterate.
        for (short a = 0;a < A->alpha;a++) {
            if (A->graph[i][a] >= A->used[i]) {
                continue;
            }

            if (A->graph[i][a] == i) {
                A->graph[i][a] = -1;
            }
            else {
                A->graph[i][a]++;
                return true;
            }
        }

        //  printf("mid1 %d\n", i);

          // We try to modify the edges that go out of the used state space
          // First, we try to iterate the integer partition that symbolizes these edges. 
        if (A->outlabs[i] > 0 && A->parsize[i] < A->outlabs[i]) {

            // The number of available states for the outgoing edges.
            short avail = A->states - A->used[i];

            // We iterate the partition until we find a valid one or we reach the end.
            A->parsize[i] = iterate_integer_partition(A->parti[i], A->outlabs[i]);
            while (A->parsize[i] > avail && A->parsize[i] < A->outlabs[i]) {
                A->parsize[i] = iterate_integer_partition(A->parti[i], A->outlabs[i]);
            }

            // If we found a valid partition we return.
            if (A->parsize[i] <= avail) {
                return true;
            }
        }


        // printf("mid %d\n", i);


         // We now try to iterate on the partition between ingoing and outgoing edges.
        if (i < A->states - 1 && A->outlabs[i] < A->alpha) {
            //printf("old: %d\n", A->outlabs[i]);
            for (short a = 0;a < A->alpha;a++) {
                if (A->graph[i][a] >= A->used[i]) {
                    A->graph[i][a] = -1;
                    A->outlabs[i]--;
                    continue;
                }
                if (A->graph[i][a] == -1) {
                    A->graph[i][a] = A->used[i];
                    A->outlabs[i]++;
                    break;
                }
            }
            // printf("new: %d\n", A->outlabs[i]);


             // We reinitialize the integer partition symbolizing the outgoing edges.
            A->parsize[i] = 1;
            A->parti[i][0] = A->outlabs[i];
            for (short a = 1;a < A->outlabs[i];a++) {
                A->parti[i][a] = 0;
            }

            // We reinitialize the used states for the states that follow.
            for (short j = i + 1; j < A->states;j++) {
                A->used[j] = A->used[i] + A->parsize[i];
            }
            return true;
        }


        //printf("end %d\n", i);



        // Finally, if we were on the last iteration for this state, we reinitialize and
        // move to the previous state. 
        for (short a = 0;a < A->alpha;a++) {
            A->graph[i][a] = -1;
        }
        A->outlabs[i] = 0;
        for (short j = i + 1; j < A->states;j++) {
            A->used[j] = A->used[i];
        }
    }



    // Special case for the initial state (can exploit symmtries between labels)

    if (A->inisep < A->alpha - A->outlabs[0]) {
        A->inisep++;
        return true;
    }

    A->inisep = 0;
    if (A->outlabs[0] > 0 && A->parsize[0] < A->outlabs[0]) {

        // The number of available states for the outgoing edges.
        short avail = A->states - A->used[0];

        // We iterate the partition until we find a valid one or we reach the end.
        A->parsize[0] = iterate_integer_partition(A->parti[0], A->outlabs[0]);
        while (A->parsize[0] > avail && A->parsize[0] < A->outlabs[0]) {
            A->parsize[0] = iterate_integer_partition(A->parti[0], A->outlabs[0]);
        }
        // If we found a valid partition we return.
        if (A->parsize[0] <= avail) {
            return true;
        }
    }

    if (A->outlabs[0] < A->alpha && A->states > 1) {
        A->outlabs[0]++;
        A->parsize[0] = 1;
        A->parti[0][0] = A->outlabs[0];
        for (short a = 1;a < A->outlabs[0];a++) {
            A->parti[0][a] = 0;
        }
        for (short j = 1; j < A->states;j++) {
            A->used[j] = A->used[0] + A->parsize[0];
        }
        return true;
    }

    return false;

}



short iterate_integer_partition(short* part, short n) {
    short i = 0;
    while (i < n && part[i] > 1) {
        i++;
    }
    if (i == 0) {
        return n;
    }
    short k = i - 1;
    part[k]--;
    short N = 0;
    while (i < n && part[i] == 1) {
        N++;
        i++;
    }
    short q = (N + 1) / part[k];
    short r = (N + 1) % part[k];
    short end = k + q + 1;
    for (short j = k + 1;j < end;j++) {
        part[j] = part[k];
    }
    if (r != 0) {
        part[end] = r;
        end++;
    }
    short j = end;
    while (j < n && part[j] != 0) {
        part[j] = 0;
        j++;
    }


    return end;
}



nfa* nfa_enum_to_nfa(nfa_enum* E) {
    nfa* A = nfa_init();
    rigins_dequeue(0, A->initials);
    rigins_dequeue(E->used[E->states - 1] - 1, A->finals);
    A->trans = create_lgraph_noedges(E->states, E->alpha);

    MALLOC(A->alphabet, E->alpha);
    for (short a = 0; a < E->alpha; a++) {
        A->alphabet[a].lab = a + 'a';
        A->alphabet[a].num = -1;
    }

    for (short a = 0; a < E->inisep; a++) {
        rigins_dequeue(0, A->trans->edges[0][a]);
    }

    short b = E->alpha - E->outlabs[0];
    for (short i = 1; i <= E->parsize[0]; i++) {
        for (short j = 0; j < E->parti[0][i - 1]; j++) {
            rigins_dequeue(i, A->trans->edges[0][b]);
            b++;
        }
    }

    for (short q = 1; q < E->states; q++) {
        //printf("State %d\n", q);
        short ind = 0;
        short con = 0;

        for (short a = 0; a < E->alpha; a++) {
            //  printf("Letter %d\n", a);
            if (E->graph[q][a] == -1) {
                continue;
            }
            if (E->graph[q][a] < E->used[q]) {
                rigins_dequeue(E->graph[q][a], A->trans->edges[q][a]);
                continue;
            }

            //   printf("there\n");

            if (con < E->parti[q][ind]) {
                rigins_dequeue(E->used[q] + ind, A->trans->edges[q][a]);
                con++;
            }
            else {
                con = 0;
                ind++;
                rigins_dequeue(E->used[q] + ind, A->trans->edges[q][a]);
            }

        }
    }
    //printf("Done\n");

    return A;

}


void nfa_enum_print(nfa_enum* E) {
    printf("States: %d\n", E->states);
    printf("Alphabet: %d\n", E->alpha);
    printf("Graph:\n");
    for (short q = 0; q < E->states; q++) {
        printf("State %d: ", q);
        for (short a = 0; a < E->alpha; a++) {
            printf("%d ", E->graph[q][a]);
        }
        printf("\n");
    }
    printf("Used states: ");
    for (short q = 0; q < E->states; q++) {
        printf("%d ", E->used[q]);
    }
    printf("\n");
    printf("Outgoing labels: ");
    for (short q = 0; q < E->states; q++) {
        printf("%d ", E->outlabs[q]);
    }
    printf("\n");
    printf("Partition:\n");
    for (short q = 0; q < E->states; q++) {
        printf("State %d: ", q);
        for (short a = 0; a < E->parsize[q]; a++) {
            printf("%d ", E->parti[q][a]);
        }
        printf("\n");
    }
    printf("Initial separation: %d\n", E->inisep);
    printf("Final state: %d\n", E->final);
}
