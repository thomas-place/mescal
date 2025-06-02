#include "monoid.h"
#include "monoid_display.h"
#include "interrupt.h"
#include "shell_errors.h"
#include "monoid_ideals.h"
#include "type_hash.h"
#include <time.h>



/*******************/
/* Basic functions */
/*******************/


void delete_green(green* G) {
    if (G == NULL) {
        return;
    }
    if (G->regular_array) {
        free(G->regular_array);
    }
    if (G->group_array) {
        free(G->group_array);
    }
    delete_parti(G->HCL);
    delete_parti(G->RCL);
    delete_parti(G->LCL);
    delete_parti(G->JCL);
    free(G);

}

void delete_morphism(morphism* M) {
    if (!M) {
        return;
    }
    /* Mandatory fields */

    // Free all names.
    //for (uint i = 0; i < M->r_cayley->size_graph; i++) {
    //    delete_dequeue(M->names[i]);
    //}
    //free(M->names);

    free(M->pred_ele);
    free(M->pred_lab);

    // Free accepting elements.
    free(M->accept_array);
    free(M->accept_list);

    // Free idempotents.
    free(M->idem_array);
    free(M->idem_list);

    // Free generators.
    free(M->alphabet);

    // Free Green relations.
    delete_green(M->rels);

    // Free jord
    //delete_lgraph(M->j_order);

    // Free the representatives
    free(M->regular_idems);

    /* Optional fields */


    // Free Cayley graphs
    delete_dgraph(M->l_cayley);
    delete_dgraph(M->r_cayley);

    // Free optionnal fields.
    free(M->mult);
    free(M->order);
    free(M->order_size);
    free(M->order_storage);

    free(M);
}

letter* mor_duplicate_alpha(const morphism* M) {
    if (!M) {
        return NULL;
    }
    letter* alphabet;
    MALLOC(alphabet, M->r_cayley->size_alpha);
    for (uint i = 0; i < M->r_cayley->size_alpha; i++) {
        alphabet[i] = M->alphabet[i];
    }
    return alphabet;
}

uint mor_letter_index(const morphism* M, letter l) {
    const letter* p = bsearch(&l, M->alphabet, M->r_cayley->size_alpha, sizeof(letter), compare_letters);
    if (p) {
        return p - M->alphabet;
    }
    else {
        return UINT_MAX;
    }
}

/**********************************************/
/* Preliminary functions for the construction */
/**********************************************/


void mor_compute_leftcayley(morphism* M) {

    // As a precaution, we delete the left Cayley graph if it already exists.
    if (M->l_cayley) {
        delete_dgraph(M->l_cayley);
    }


    // Création du graphe.
    M->l_cayley = create_dgraph_noedges(M->r_cayley->size_graph, M->r_cayley->size_alpha);
    dequeue* fromlet = create_dequeue();
    dequeue* fromone = create_dequeue();
    bool* visited;
    MALLOC(visited, M->r_cayley->size_graph);

    // For each letter in the alphabet.
    for (uint a = 0; a < M->r_cayley->size_alpha; a++) {

        // Initialize the queues and the visited array.
        makeempty_dequeue(fromlet);
        makeempty_dequeue(fromone);
        for (uint s = 0; s < M->r_cayley->size_graph; s++) {
            visited[s] = false;
        }
        rigins_dequeue(ONE, fromone);
        rigins_dequeue(M->r_cayley->edges[ONE][a], fromlet);
        visited[ONE] = true;

        while (!isempty_dequeue(fromone)) {
            uint s = rigpull_dequeue(fromone);
            uint as = rigpull_dequeue(fromlet);
            M->l_cayley->edges[s][a] = as;
            for (uint b = 0; b < M->r_cayley->size_alpha; b++) {
                if (!visited[M->r_cayley->edges[s][b]]) {
                    rigins_dequeue(M->r_cayley->edges[s][b], fromone);
                    rigins_dequeue(M->r_cayley->edges[as][b], fromlet);
                    visited[M->r_cayley->edges[s][b]] = true;
                }
            }
        }
    }
    free(visited);
    delete_dequeue(fromlet);
    delete_dequeue(fromone);
}



// Quick sort algorithm between the indices i and j - 1 (inclusive)
static void aux_quick_sort_green(uint* array, uint i, uint j, uint* rnums, uint* lnums) {

    if (i + 1 >= j)
    {
        return;
    }
    else {
        uint pivot = array[i];
        uint l = j;

        for (uint k = j - 1; i < k; k--) {
            if (rnums[pivot] < rnums[array[k]] || (rnums[pivot] == rnums[array[k]] && lnums[pivot] < lnums[array[k]])) {
                l--;
                uint temp = array[l];
                array[l] = array[k];
                array[k] = temp;
            }
        }
        uint temp = array[l - 1];
        array[l - 1] = array[i];
        array[i] = temp;

        aux_quick_sort_green(array, i, l - 1, rnums, lnums);
        aux_quick_sort_green(array, l, j, rnums, lnums);
    }
}


uint* green_sorted_jclass(green* G, uint i) {
    uint* sorted_jclass;
    MALLOC(sorted_jclass, G->JCL->cl_size[i]);
    for (uint j = 0; j < G->JCL->cl_size[i]; j++) {
        sorted_jclass[j] = G->JCL->cl_elems[i][j];
    }
    aux_quick_sort_green(sorted_jclass, 0, G->JCL->cl_size[i], G->RCL->numcl, G->LCL->numcl);
    return sorted_jclass;
}


void h_green_compute(green* GREL) {
    if (GREL->RCL == NULL || GREL->LCL == NULL || GREL->JCL == NULL) {
        fprintf(stderr, "Error. Cannot compute the H-classes without the J-classes, the "
            "L-classes and the R-classes\n");
        return;
    }

    // If the H-classes already exist, we delete them (should not happen).
    if (GREL->HCL) {
        delete_parti(GREL->HCL);
        return;
    }

    uint num = 0;

    // Computes the H-classes
    // MALLOC(GREL->HCL, 1);
    // GREL->HCL->size_set = GREL->JCL->size_set;
    // MALLOC(GREL->HCL->numcl, GREL->HCL->size_set);
    uint* hnumcl;
    MALLOC(hnumcl, GREL->JCL->size_set);


    // For each J-class.
    for (uint cr = 0; cr < GREL->JCL->size_par; cr++) {
        // We store the elements of the J-class in an array.
        // We sort these elements according to their index in the R-class and then in the L-class.
        // This is done to ensure that the elements of the same H-class are contiguous in the array.
        uint* thejclass = green_sorted_jclass(GREL, cr);


        uint elem = thejclass[0];
        hnumcl[elem] = num;
        num++;
        for (uint i = 1; i < GREL->JCL->cl_size[cr]; i++) {
            if (GREL->LCL->numcl[thejclass[i]] == GREL->LCL->numcl[elem] && GREL->RCL->numcl[thejclass[i]] == GREL->RCL->numcl[elem]) {
                hnumcl[thejclass[i]] = hnumcl[elem];
            }
            else {
                hnumcl[thejclass[i]] = num;
                num++;
                elem = thejclass[i];
            }
        }
        free(thejclass);

    }

    // We now know the number of H-classes.

    GREL->HCL = create_parti(GREL->JCL->size_set, num, hnumcl);
    // GREL->HCL->size_par = num;

    // // It remains to build the table of classes.
    // MALLOC(GREL->HCL->cl, GREL->HCL->size_par);

    // for (uint c = 0; c < GREL->HCL->size_par; c++) {
    //     GREL->HCL->cl[c] = create_dequeue();
    // }
    // for (uint v = 0; v < GREL->HCL->size_set; v++) {
    //     rigins_dequeue(v, GREL->HCL->cl[GREL->HCL->numcl[v]]);
    // }

}




void gr_green_compute(uint* idem_list, uint nb_idems, green* G) {

    // First, we compute the regular elements.
    CALLOC(G->regular_array, G->JCL->size_set);
    G->nb_regular_elems = 0;

    // For each idempotent, we mark all elements of its J-class has regular. We
    // skip an idempotent whose J-class has already been marked as regular.

    for (uint i = 0; i < nb_idems; i++) {
        uint c = G->JCL->numcl[idem_list[i]];
        if (G->regular_array[G->JCL->cl_elems[c][0]]) {
            continue;
        }
        for (uint j = 0; j < G->JCL->cl_size[c]; j++) {
            G->regular_array[G->JCL->cl_elems[c][j]] = true;
        }
        G->nb_regular_elems += G->JCL->cl_size[c];
    }

    // Building information on groups.
    CALLOC(G->group_array, G->HCL->size_set);
    for (uint i = 0; i < nb_idems; i++) {
        uint j = G->HCL->numcl[idem_list[i]];
        for (uint k = 0; k < G->HCL->cl_size[j]; k++) {
            G->group_array[G->HCL->cl_elems[j][k]] = true;
        }
    }
}



void mor_compute_green(morphism* M) {
    if (M->rels) {
        delete_green(M->rels);
    }
#ifdef DEBUG_MONO
    printf("Computing the Green relations of the monoid.\n");
    ulong thetime = time(NULL);
#endif
    // Initialization of the structure.
    CALLOC(M->rels, 1);


    // Computing the R equivalence (strongly connected components of the right cayley graph).
    M->rels->RCL = dtarjan(M->r_cayley, NULL, true);
#ifdef DEBUG_MONO
    printf("R done. Time: %f\n", difftime(time(NULL), thetime));
#endif


    // Computing the L equivalence (strongly connected components of the left cayley graph).
    M->rels->LCL = dtarjan(M->l_cayley, NULL, true);

#ifdef DEBUG_MONO
    printf("L done. Time: %f\n", difftime(time(NULL), thetime));
#endif


    // Computing the J equivalence (strongly connected components of the J order).
    //graph* j_order = ldgraphs_to_graph(0, 0, 2, 2, M->r_cayley, M->l_cayley); // Compute the J-order.


    M->rels->JCL = dualdtarjan(M->r_cayley, M->l_cayley, NULL, true);
    //M->rels->JCL = tarjan(j_order);
    //printf("J done. Time: %f\n", difftime(time(NULL), thetime));
    //delete_graph(j_order);
#ifdef DEBUG_MONO
    printf("J done. Time: %f\n", difftime(time(NULL), thetime));
#endif



    // Computing the relation H.
    h_green_compute(M->rels);
#ifdef DEBUG_MONO
    printf("H done. Time: %f\n", difftime(time(NULL), thetime));
#endif

    // Computing the regular elements and the groups.
    gr_green_compute(M->idem_list, M->nb_idems, M->rels);
#ifdef DEBUG_MONO
    printf("Regular elements and groups done. Time: %f\n", difftime(time(NULL), thetime));
    printf("the end\n\n");
#endif
}


void mor_compute_rep(morphism* M) {
    if (!M || !M->rels) {
        fprintf(stderr, "Error in mor_compute_min_regular_jcl.\n");
        exit(EXIT_FAILURE);
    }

    // We first count the number of regular J-classes. Moreover, for each regular J-class,
    // we compute a member idempotent e in regular_jcls_idems[i](the one with the least index).
    // If the J-class is not regular, we store UINT_MAX.
    uint* regular_jcls_idems;
    M->nb_regular_jcl = 0;
    MALLOC(regular_jcls_idems, M->rels->JCL->size_par);
    for (uint i = 0; i < M->rels->JCL->size_par; i++) {
        regular_jcls_idems[i] = UINT_MAX;
    }
    for (uint i = 0; i < M->nb_idems;i++) {
        uint e = M->idem_list[i];
        if (regular_jcls_idems[M->rels->JCL->numcl[e]] == UINT_MAX) {
            M->nb_regular_jcl++;
        }
        regular_jcls_idems[M->rels->JCL->numcl[e]] = e;
    }

    // We may now allocate the table of representatives.
    MALLOC(M->regular_idems, M->nb_regular_jcl);

    // We may now compute the representatives. We have to put the ones of minimal
    // strict regular J-classes first. There are two cases.


    // We first handle the case when 1 has a nonempty antecedent.
    // In this case, the only minimal strict regular J-class is the one of 1.
    if (mor_nonempty_neutral(M)) {
        M->nb_min_regular_jcl = 1;
        M->regular_idems[0] = ONE;
        uint j = 1;
        for (uint i = 1; i < M->rels->JCL->size_par; i++) {
            if (regular_jcls_idems[i] != UINT_MAX) {
                M->regular_idems[j] = regular_jcls_idems[i];
                j++;
            }
        }
        free(regular_jcls_idems);
        return;
    }

    // We are now in the case when the only antecedent of 1 is ε
    // We compute the non-minimal J-classes using a BFS on the J-order.
    // These are the ones reachable from a regular J-class (which is not the one of 1).

    // The queue for the BFS
    dequeue* queue = create_dequeue();

    // Array that will mark the visited elements in the BFS.
    bool* visited;
    CALLOC(visited, M->r_cayley->size_graph);

    // We start from the elements that follow a regular J-class (which is not the one of 1)
    for (uint i = 1; i < M->rels->JCL->size_par; i++) {
        if (regular_jcls_idems[i] != UINT_MAX) {
            for (uint j = 0; j < M->rels->JCL->cl_size[i]; j++) {
                uint s = M->rels->JCL->cl_elems[i][j];
                for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
                    if (M->rels->JCL->numcl[s] != M->rels->JCL->numcl[M->r_cayley->edges[s][a]]) {
                        rigins_dequeue(M->r_cayley->edges[s][a], queue);
                    }
                    if (M->rels->JCL->numcl[s] != M->rels->JCL->numcl[M->l_cayley->edges[s][a]]) {
                        rigins_dequeue(M->l_cayley->edges[s][a], queue);
                    }
                }
            }
        }
    }

    while (!isempty_dequeue(queue)) {
        uint s = lefpull_dequeue(queue);
        if (visited[s]) {
            continue;
        }
        visited[s] = true;
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            rigins_dequeue(M->r_cayley->edges[s][a], queue);
            rigins_dequeue(M->l_cayley->edges[s][a], queue);
        }
    }

    M->nb_min_regular_jcl = 0;
    for (uint i = 0; i < M->rels->JCL->size_par; i++) {
        uint s = M->rels->JCL->cl_elems[i][0];
        if (regular_jcls_idems[i] != UINT_MAX && !visited[s]) {
            M->regular_idems[M->nb_min_regular_jcl] = regular_jcls_idems[i];
            M->nb_min_regular_jcl++;
        }
    }

    uint j = M->nb_min_regular_jcl;
    for (uint i = 0; i < M->rels->JCL->size_par; i++) {
        uint s = M->rels->JCL->cl_elems[i][0];
        if (regular_jcls_idems[i] != UINT_MAX && visited[s]) {
            M->regular_idems[j] = regular_jcls_idems[i];
            j++;
        }
    }
    free(visited);
    delete_dequeue(queue);
    free(regular_jcls_idems);
}






/*
void mor_compute_min_regular_jcl(morphism* M) {
    if (!M || !M->rels) {
        fprintf(stderr, "Error in mor_compute_min_regular_jcl.\n");
        exit(EXIT_FAILURE);
    }

    M->nb_min_regular_jcl = 1;

    // If the neutral element has a nonempty antecedent, the only minimal strict J-class is the one
    // containing the neutral element.
    if (mor_nonempty_neutral(M)) {
        MALLOC(M->min_regular_idems, 1);
        M->min_regular_idems[0] = ONE;
        return;
    }

    // We now know that the J-class of 1 is a singleton.


    bool* visited;
    CALLOC(visited, M->r_cayley->size_graph);
    visited[ONE] = true;
    bool* min_regjcl;
    CALLOC(min_regjcl, M->rels->JCL->size_par);
    min_regjcl[M->rels->JCL->numcl[ONE]] = true;

    dequeue* thestack = create_dequeue();

    for (uint i = 0; i < M->rels->JCL->size_par; i++) {
        uint s = lefread_dequeue(M->rels->JCL->cl[i], 0);
        // If the J-class has already been visited or is not regular, we skip it.
        if (visited[s] || !M->rels->regular_set[s]) {
            continue;
        }
        // Otherwise, we found a minimal regular strict J-class.
        M->nb_min_regular_jcl++;
        min_regjcl[M->rels->JCL->numcl[s]] = true;

        // We mark all elements larger than the J-class as visited.
        // This enables us to skip the larger J-classes in the counting.
        rigins_dequeue(s, thestack);
        while (!isempty_dequeue(thestack)) {
            uint t = rigpull_dequeue(thestack);
            if (visited[t]) {
                continue;
            }
            visited[t] = true;
            for (uint a = 0; a < M->j_order->size_alpha; a++) {
                for (uint j = 0; j < size_dequeue(M->j_order->edges[t][a]); j++) {
                    uint u = lefread_dequeue(M->j_order->edges[t][a], j);
                    rigins_dequeue(u, thestack);
                }
            }
        }
    }
    free(visited);
    delete_dequeue(thestack);

    uint j = 0;

    CALLOC(M->min_regular_idems, M->nb_min_regular_jcl);
    for (uint i = 0; i < M->rels->nb_regular_jcl;i++) {
        if (min_regjcl[M->rels->JCL->numcl[M->rels->regular_idems[i]]]) {
            M->min_regular_idems[j] = M->rels->regular_idems[i];
            j++;
        }
    }
    free(min_regjcl);
}*/




/************************************/
/* Construction from a complete DFA */
/************************************/





// Storage of the permutations of DFA states
static ulong mor_cons_size = 0; // Size of the mor_perms array.
static ulong mor_cons_elem = 0; // Number of permutations in the mor_perms array.
static uint mor_cons_states = 0; // Number of states in the DFA.
static uint mor_cons_letters = 0; // Number of letters in the DFA (size of the alphabet).
static uint* mor_cons_perms = NULL; // Array of permutations (a single permutation takes mor_cons_states cells). Size: mor_cons_size * mor_cons_states.
static uint* mor_cons_next = NULL;  // Array of transitions (a single transition takes mor_cons_letters cells). Size: mor_cons_size * mor_cons_letters.
static uint* mor_cons_prede = NULL; // Array of predecessor elements. Size: mor_cons_size.       
static uint* mor_cons_predl = NULL; // Array of predecessor letters. Size: mor_cons_size.

// Initialization of the arrays used in the construction of the morphism.
static void mor_cons_init(uint size, uint states, uint letters) {
    size = max(size, 2); // Ensure that the size is at least 2.
    mor_cons_size = size;
    mor_cons_elem = 0;
    mor_cons_states = states;
    mor_cons_letters = letters;
    MALLOC(mor_cons_perms, mor_cons_size * mor_cons_states);
    MALLOC(mor_cons_next, mor_cons_size * mor_cons_letters);
    MALLOC(mor_cons_prede, mor_cons_size);
    MALLOC(mor_cons_predl, mor_cons_size);
}

// Initialization of the arrays used in the construction of the morphism (weak version: skips prede and predl).
static void mor_cons_init_weak(uint size, uint states, uint letters) {
    size = max(size, 2); // Ensure that the size is at least 2.
    mor_cons_size = size;
    mor_cons_elem = 0;
    mor_cons_states = states;
    mor_cons_letters = letters;
    MALLOC(mor_cons_perms, mor_cons_size * mor_cons_states);
    MALLOC(mor_cons_next, mor_cons_size * mor_cons_letters);
}

// Deletes the arrays used in the construction of the morphism and resets the variables.
static void mor_cons_delete() {
    free(mor_cons_perms);
    mor_cons_perms = NULL;
    free(mor_cons_next);
    mor_cons_next = NULL;
    free(mor_cons_prede);
    mor_cons_prede = NULL;
    free(mor_cons_predl);
    mor_cons_predl = NULL;
    mor_cons_size = 0;
    mor_cons_elem = 0;
    mor_cons_states = 0;
    mor_cons_letters = 0;
}

// Doubles the size of the arrays used in the construction of the morphism if they are full.
static void mor_cons_grow() {
    if (mor_cons_elem < mor_cons_size) {
        return; // If the arrays are not full, we do nothing.
    }

#ifdef DEBUG_MONO
    printf("Doubling the size of the morphism construction arrays.\n");
    printf("Current size: %lu.\n", mor_cons_size);
#endif
    // Double the size of the arrays used in the construction of the morphism.
    mor_cons_size <<= 1; // Double the size of the arrays.
#ifdef DEBUG_MONO
    printf("New size: %lu\n\n", mor_cons_size);
#endif
    // Reallocate the arrays with the new size.
    REALLOC(mor_cons_perms, mor_cons_size * mor_cons_states);
    REALLOC(mor_cons_next, mor_cons_size * mor_cons_letters);
    REALLOC(mor_cons_prede, mor_cons_size);
    REALLOC(mor_cons_predl, mor_cons_size);
}

// Doubles the size of the arrays used in the construction of the morphism if they are full (weak version: skips prede and predl).
static void mor_cons_grow_weak() {
    if (mor_cons_elem < mor_cons_size) {
        return; // If the arrays are not full, we do nothing.
    }

#ifdef DEBUG_MONO
    printf("Doubling the size of the morphism construction arrays.\n");
    printf("Current size: %lu.\n", mor_cons_size);
#endif
    // Double the size of the arrays used in the construction of the morphism.
    mor_cons_size <<= 1; // Double the size of the arrays.
#ifdef DEBUG_MONO
    printf("New size: %lu\n\n", mor_cons_size);
#endif
    // Reallocate the arrays with the new size.
    REALLOC(mor_cons_perms, mor_cons_size * mor_cons_states);
    REALLOC(mor_cons_next, mor_cons_size * mor_cons_letters);
}


// Checks if two permutations stored in the array are equal.
static bool mor_cons_equal(uint i, uint j) {
    ulong zi = i * mor_cons_states; // The index of the first element in the mor_perms array.
    ulong zj = j * mor_cons_states; // The index of the second element in the mor_perms array.
    for (uint h = 0; h < mor_cons_states; h++) {
        if (mor_cons_perms[zi + h] != mor_cons_perms[zj + h]) {
            return false; // If any element is different, the two permutations are not equal.
        }
    }
    return true;
}

// Hashing function for the morphism construction.
static uint mor_cons_hash(uint i, uint size_hash) {
    ulong e = i * mor_cons_states; // The index of the element in the mor_perms array.
    uint hash = 0;
    uint a = 0x9e3779b9; // fractional bits of the golden ratio
    for (uint j = 0; j < mor_cons_states; j++) {
        hash = (hash * (mor_cons_states + 1) + mor_cons_perms[e + j] * a) % size_hash;
    }
    return hash;
}


// Computes (partial information on) the syntactic ordering from the array of permutations and the ordering on the input DFA.
static void mor_cons_order(morphism* M, bool** order) {

#ifdef DEBUG_MONO
    printf("Computing the syntactic ordering of the morphism.\n");
#endif

    MALLOC(M->order, M->nb_regular_jcl);
    MALLOC(M->order_size, M->nb_regular_jcl);
    uint size_storage = 0;
    dequeue** order_temp;
    MALLOC(order_temp, M->nb_regular_jcl);

#ifdef DEBUG_MONO
    printf("Finished allocation.\n");
#endif

    for (uint i = 0; i < M->nb_regular_jcl; i++) {
        order_temp[i] = create_dequeue();
        ulong e = M->regular_idems[i];
        dequeue* eM = compute_r_ideal(M, e, NULL);
        dequeue* Me = compute_l_ideal(M, e, NULL);
        dequeue* eMe = make_inter_sorted_dequeue(eM, Me);
        delete_dequeue(eM);
        delete_dequeue(Me);


        for (uint j = 0; j < size_dequeue(eMe); j++) {
            uint q = lefread_dequeue(eMe, j) * (ulong)mor_cons_states;
            bool found = true;
            for (uint k = 0; k < mor_cons_states; k++) {
                if (!order[mor_cons_perms[e + k]][mor_cons_perms[q + k]]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                rigins_dequeue(q, order_temp[i]);
            }
        }
        M->order_size[i] = size_dequeue(order_temp[i]);
        size_storage += M->order_size[i];
    }

#ifdef DEBUG_MONO
    printf("Finished computation.\n");
#endif

    MALLOC(M->order_storage, size_storage);
    uint pos = 0;
    for (uint i = 0; i < M->nb_regular_jcl; i++) {
        M->order[i] = M->order_storage + pos;
        pos += M->order_size[i];
        for (uint j = 0; j < size_dequeue(order_temp[i]); j++) {
            M->order[i][j] = lefread_dequeue(order_temp[i], j);
        }
        delete_dequeue(order_temp[i]);
    }
    free(order_temp);

#ifdef DEBUG_MONO
    printf("All finished.\n");
#endif
}


morphism* dfa_to_morphism(dfa* A, bool** order, int*) {

#ifdef DEBUG_MONO
    ulong thetime = time(NULL);
#endif

    uchar power = get_uint_lbinary(A->trans->size_graph) + 2; // We add 2 to ensure that the hash table is large enough.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    // Initialization of the arrays used in the construction of the morphism.
    mor_cons_init(thesize, A->trans->size_graph, A->trans->size_alpha);

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &mor_cons_hash, &mor_cons_equal);

    // Queue containing the elements to be processed.
    dequeue* thequeue = create_dequeue();

    // Create the first element: the identity (which has number 0).
    for (uint i = 0; i < A->trans->size_graph; i++) {
        mor_cons_perms[i] = i; // The identity is the permutation of the states.
    }
    mor_cons_elem++;
    hash_table_insert(thehash, 0); // Insert the identity in the hash table.
    rigins_dequeue(0, thequeue); // Add the identity to the queue.

#ifdef DEBUG_MONO
    printf("Size automaton: %d, size hash table: %d, initial power: %d\n", A->trans->size_graph, thesize, power);
#endif

    while (!isempty_dequeue(thequeue)) {

        uint s = lefpull_dequeue(thequeue); // Get the element to be processed.

        // Computes the transitions from this element.
        for (uint a = 0; a < A->trans->size_alpha; a++) {

            // Create the (potential) new permutation in the next cell of the table.
            ulong i = mor_cons_elem * mor_cons_states; // Position of the next element in the mor_perms array.
            ulong j = s * mor_cons_states; // Position of the current element in the mor_perms array.
            for (uint q = 0; q < A->trans->size_graph; q++) {
                mor_cons_perms[i + q] = A->trans->edges[mor_cons_perms[j + q]][a];
            }

            // Try to insert the new element in the hash table.
            uint h = hash_table_insert(thehash, mor_cons_elem);
            if (h == mor_cons_elem) {

                // The element was not already constructed.
                rigins_dequeue(mor_cons_elem, thequeue); // We add it to the queue for future processing.

                // We initialize the new element.
                mor_cons_prede[mor_cons_elem] = s; // The predecessor of this element is the one we are treating.
                mor_cons_predl[mor_cons_elem] = a; // The predecessor letter is the one we are treating.

                // Prepare the next element in the table.
                mor_cons_elem++; // Increment the number of elements constructed.
                mor_cons_grow(); // If the number of elements constructed is larger than the size of the table, we double the size.
            }

            // Assign the next element of s for a.
            ulong sa = s * mor_cons_letters + a;
            mor_cons_next[sa] = h;
        }
    }

    delete_dequeue(thequeue); // Free the queue.
    delete_hash_table(thehash); // Free the hash table.

#ifdef DEBUG_MONO
    printf("BFS done. Time: %f\n Elements: %lu", difftime(time(NULL), thetime), mor_cons_elem);
#endif

    // Initialization of the morphism.
    morphism* M;
    CALLOC(M, 1);
    M->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);  // Copy letter names.
    M->r_cayley = create_dgraph_noedges(mor_cons_elem, mor_cons_letters); // Create the graph.
    MALLOC(M->pred_ele, mor_cons_elem);                                   // Predecessor elements.
    MALLOC(M->pred_lab, mor_cons_elem);                                   // Predecessor letters.
    MALLOC(M->accept_array, mor_cons_elem);                               // Array representing the accepting set.
    MALLOC(M->idem_array, mor_cons_elem);                                 // Array of idempotents.
    M->order = NULL;                                                      // The ordering is not computed yet.

    // Computation of the right Cayley graph, predecessors, the idempotents and the accepting elements.
    M->nb_idems = 0; // Number of idempotents.
    M->nb_accept = 0; // Number of accepting elements.

    // For all elements
    for (uint i = 0; i < mor_cons_elem; i++) {

        // Right Cayley graph.
        ulong j = i * (ulong)mor_cons_letters; // The index of the element in the table.
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            M->r_cayley->edges[i][a] = mor_cons_next[j + a]; // Assign the transitions.
        }

        // Predecessors and predecessor letters.
        M->pred_ele[i] = mor_cons_prede[i]; // Predecessor elements.
        M->pred_lab[i] = mor_cons_predl[i]; // Predecessor letters.

        // Is the element an accepting one ?
        ulong d = i * (ulong)mor_cons_states;
        if (bsearch(&mor_cons_perms[d + A->initial], A->finals, A->nb_finals, sizeof(uint), &compare_uint)) {
            M->accept_array[i] = true; // If the element is accepting, we set it to true.
            M->nb_accept++; // Increment the number of accepting elements.
        }
        else {
            M->accept_array[i] = false; // Otherwise, we set it to false.
        }

        // Is the element an idempotent one?
        M->idem_array[i] = true; // We assume it is idempotent.
        uint q = 0;
        while (q < A->trans->size_graph) {
            if (mor_cons_perms[d + q] != mor_cons_perms[d + mor_cons_perms[d + q]]) {
                M->idem_array[i] = false;
                break;
            }
            q++;
        }
        if (M->idem_array[i]) {
            M->nb_idems++; // If the element is idempotent, we increment the number of idempotents.
        }
    }

    // Creation of the lists of idempotents and accepting elements.
    MALLOC(M->idem_list, M->nb_idems);
    MALLOC(M->accept_list, M->nb_accept);
    uint j = 0;
    uint h = 0;
    for (uint i = 0; i < mor_cons_elem; i++) {
        if (M->idem_array[i]) {
            M->idem_list[j] = i;
            j++;
        }
        if (M->accept_array[i]) {
            M->accept_list[h] = i;
            h++;
        }
    }

#ifdef DEBUG_MONO
    printf("Right Cayley graph, idempotent elements and accepting elements done. Time: %f\n", difftime(time(NULL), thetime));
#endif

    // Compute the left Cayley graph.
    mor_compute_leftcayley(M);

#ifdef DEBUG_MONO
    printf("Left Cayley graph done. Time: %f\n", difftime(time(NULL), thetime));
#endif

    // Compute the Green relations.
    mor_compute_green(M);

#ifdef DEBUG_MONO
    printf("Green relations done. Time: %f\n", difftime(time(NULL), thetime));
#endif
    mor_compute_rep(M);

#ifdef DEBUG_MONO
    printf("Representatives idempotents for the J-classes done. Time: %f\n", difftime(time(NULL), thetime));
#endif

    // If we need to compute the ordering, we do it now.
    if (order) {
        mor_cons_order(M, order);
#ifdef DEBUG_MONO
        printf("Ordering done. Time: %f\n", difftime(time(NULL), thetime));
#endif
    }

    // We free the arrays used in the construction of the morphism.
    mor_cons_delete();

    ignore_interrupt();
    normal_mode();
    return M;

}

uint dfa_to_morphism_size(dfa* A) {

    uchar power = get_uint_lbinary(A->trans->size_graph) + 2; // We add 2 to ensure that the hash table is large enough.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    // Initialization of the arrays used in the construction of the morphism.
    mor_cons_init_weak(thesize, A->trans->size_graph, A->trans->size_alpha);

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &mor_cons_hash, &mor_cons_equal);

    // Queue containing the elements to be processed.
    dequeue* thequeue = create_dequeue();

    // Create the first element: the identity (which has number 0).
    for (uint i = 0; i < A->trans->size_graph; i++) {
        mor_cons_perms[i] = i; // The identity is the permutation of the states.
    }
    mor_cons_elem++;
    hash_table_insert(thehash, 0); // Insert the identity in the hash table.
    rigins_dequeue(0, thequeue); // Add the identity to the queue.


    while (!isempty_dequeue(thequeue)) {

        uint s = lefpull_dequeue(thequeue); // Get the element to be processed.

        // Computes the transitions from this element.
        for (uint a = 0; a < A->trans->size_alpha; a++) {

            // Create the (potential) new permutation in the next cell of the table.
            ulong i = mor_cons_elem * mor_cons_states; // Position of the next element in the mor_perms array.
            ulong j = s * mor_cons_states; // Position of the current element in the mor_perms array.
            for (uint q = 0; q < A->trans->size_graph; q++) {
                mor_cons_perms[i + q] = A->trans->edges[mor_cons_perms[j + q]][a];
            }

            // Try to insert the new element in the hash table.
            uint h = hash_table_insert(thehash, mor_cons_elem);
            if (h == mor_cons_elem) {

                // The element was not already constructed.
                rigins_dequeue(mor_cons_elem, thequeue); // We add it to the queue for future processing.


                // Prepare the next element in the table.
                mor_cons_elem++; // Increment the number of elements constructed.
                mor_cons_grow_weak(); // If the number of elements constructed is larger than the size of the table, we double the size.
            }

            // Assign the next element of s for a.
            ulong sa = s * mor_cons_letters + a;
            mor_cons_next[sa] = h;
        }
    }

    delete_dequeue(thequeue); // Free the queue.
    delete_hash_table(thehash); // Free the hash table.

    uint num = (uint)mor_cons_elem;
    mor_cons_delete();

    return num;
}

dfa* morphism_to_dfa(morphism* M) {
    dfa* D = dfa_init(M->r_cayley->size_graph, M->r_cayley->size_alpha, M->nb_accept, M->alphabet);
    for (uint i = 0; i < M->r_cayley->size_graph; i++) {
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            D->trans->edges[i][a] = M->r_cayley->edges[i][a];
        }
    }
    D->initial = ONE;
    for (uint i = 0; i < M->nb_accept; i++) {
        D->finals[i] = M->accept_list[i];
    }
    return D;
}

dfa* left_morphism_to_dfa(morphism* M) {
    dfa* D = dfa_init(M->l_cayley->size_graph, M->l_cayley->size_alpha, M->nb_accept, M->alphabet);
    for (uint i = 0; i < M->l_cayley->size_graph; i++) {
        for (uint a = 0; a < M->l_cayley->size_alpha; a++) {
            D->trans->edges[i][a] = M->l_cayley->edges[i][a];
        }
    }
    D->initial = ONE;
    for (uint i = 0; i < M->nb_accept; i++) {
        D->finals[i] = M->accept_list[i];
    }
    return D;
}


/************************************************/
/* Calculs des informations sur un morphism graph */
/************************************************/



void mor_compute_mult(morphism* M) {
    if (M->mult != NULL) // Si la table a déjà été calculée
    {
        return;
    }
    // Création du tableau
    MALLOC(M->mult, M->r_cayley->size_graph);
    dequeue* fromcur = create_dequeue();
    dequeue* fromone = create_dequeue();
    bool visited[M->r_cayley->size_graph];
    // Pour chaque élément
    for (uint q = 0; q < M->r_cayley->size_graph; q++) {
        // Création du tableau des multiplications de q
        MALLOC(M->mult, M->r_cayley->size_graph * M->r_cayley->size_graph);

        // Initialisation des strucutures
        makeempty_dequeue(fromcur);
        makeempty_dequeue(fromone);
        for (uint s = 0; s < M->r_cayley->size_graph; s++) {
            visited[s] = false;
        }

        rigins_dequeue(ONE, fromone);
        rigins_dequeue(q, fromcur);
        visited[ONE] = true;

        while (!isempty_dequeue(fromone)) {
            uint s = rigpull_dequeue(fromone);
            uint qs = rigpull_dequeue(fromcur);
            M->mult[q * M->r_cayley->size_graph + s] = qs;
            for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
                if (!visited[M->r_cayley->edges[s][a]]) {
                    rigins_dequeue(M->r_cayley->edges[s][a], fromone);
                    rigins_dequeue(M->r_cayley->edges[qs][a], fromcur);
                    visited[M->r_cayley->edges[s][a]] = true;
                }
            }
        }
    }
    delete_dequeue(fromcur);
    delete_dequeue(fromone);
}

lgraph* mor_rmirror(morphism* M) {
    return dgraph_mirror(M->r_cayley);
}

lgraph* mor_lmirror(morphism* M) {
    return dgraph_mirror(M->l_cayley);
}


dequeue** mor_compute_order(morphism* M) {
    uint thesize = M->r_cayley->size_graph;

    // Array that marks the visited pairs.
    bool** visited;
    MALLOC(visited, thesize);
    for (uint i = 0; i < thesize; i++) {
        CALLOC(visited[i], thesize);
    }

    // Stacks for the DFS which computes all pairs incomparable of elements.
    // A pair (q, r) is incomparable if q is NOT smaller than r for the syntactic order.
    // First stack stores element 1 in the pair, second stack stores element 2 in the pair.
    dequeue* stack_one = create_dequeue();
    dequeue* stack_two = create_dequeue();

    // We push the starting pairs (accepting, non-accepting) which are clearly incomparable.
    for (uint i = 0; i < M->nb_accept; i++)
    {
        for (uint q = 0; q < thesize; q++) {
            // We skip q if it is an accepting element.
            if (M->accept_array[q]) {
                continue;
            }
            // We loop over the accepting elements.

            rigins_dequeue(M->accept_list[i], stack_one);
            rigins_dequeue(q, stack_two);
        }
    }

    // Computation of the mirror graphs (used in the DFS).
    lgraph* rmirror = mor_rmirror(M);
    lgraph* lmirror = mor_lmirror(M);

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
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(rmirror->edges[q][a]); i++) {
                for (uint j = 0; j < size_dequeue(rmirror->edges[r][a]); j++) {
                    rigins_dequeue(lefread_dequeue(rmirror->edges[q][a], i), stack_one);
                    rigins_dequeue(lefread_dequeue(rmirror->edges[r][a], j), stack_two);
                }
            }

            for (uint i = 0; i < size_dequeue(lmirror->edges[q][a]); i++) {
                for (uint j = 0; j < size_dequeue(lmirror->edges[r][a]); j++) {
                    rigins_dequeue(lefread_dequeue(lmirror->edges[q][a], i), stack_one);
                    rigins_dequeue(lefread_dequeue(lmirror->edges[r][a], j), stack_two);

                }
            }
        }
    }

    dequeue** order;
    // We may now compute the syntactic order on the elements.
    MALLOC(order, M->r_cayley->size_graph);
    for (uint q = 0; q < M->r_cayley->size_graph; q++) {
        order[q] = create_dequeue();
        for (uint r = 0; r < M->r_cayley->size_graph; r++) {
            if (!visited[q][r]) {
                rigins_dequeue(r, order[q]);
            }
        }
        free(visited[q]);
    }
    free(visited);

    delete_dequeue(stack_one);
    delete_dequeue(stack_two);
    delete_lgraph(rmirror);
    delete_lgraph(lmirror);
    return order;
}


/*********************************/
/* Opérations sur les morphismes */
/*********************************/

dequeue* mor_name(morphism* M, uint  q) {
    dequeue* res = create_dequeue();
    while (q != ONE) {
        lefins_dequeue(M->pred_lab[q], res);
        q = M->pred_ele[q];
    }
    return res;
}


uint mor_mult(morphism* M, uint s, uint t) {
    if (s >= M->r_cayley->size_graph || t >= M->r_cayley->size_graph) {
        fprintf(stderr, "Error, these are not elements of the monoid\n");
        return M->r_cayley->size_graph;
    }
    if (M->mult != NULL) {
        return M->mult[M->r_cayley->size_graph * s + t];
    }
    while (s != ONE) {
        t = M->l_cayley->edges[t][M->pred_lab[s]];
        s = M->pred_ele[s];
    }
    return t;


}

uint mor_mult_gen(morphism* M, uint n, ...) {
    va_list liste;
    va_start(liste, n);

    uint elem = ONE;

    // Calcul
    for (uint i = 0; i < n; i++) {
        elem = mor_mult(M, elem, va_arg(liste, uint));
    }

    va_end(liste);
    return elem;
}

uint mor_omega(morphism* M, uint s) {
    if (s >= M->r_cayley->size_graph) {
        fprintf(stderr, "Error, these are not elements of the monoid\n");
        return M->r_cayley->size_graph;
    }
    uint s2 = mor_mult(M, s, s);
    uint q = s;
    uint r = s2;
    while (q != r) {
        q = mor_mult(M, q, s);
        r = mor_mult(M, r, s2);
    }
    return q;
}

//
bool mor_num_idem(morphism* M, uint s, uint* res) {
    for (uint i = 0; i < M->nb_idems; i++) {
        if (M->idem_list[i] == s) {
            *res = i;
            return true;
        }
    }
    return false;
}




/*
bool morphism_elem_from_string(morphism* M, char* w, uint* res)
{
    uint len = strlen(w);
    *res = ONE;
    for (uint i = 0; i < len; i++)
    {
        uint a = w[i] - 'a';
        if (w[i] >= 'a' && a < M->r_cayley->size_alpha)
        {
            *res = M->r_cayley->edges[*res][a];
        }
        else
        {
            return false;
        }
    }
    return true;
}

// Image d'un mot passé sous ma forme d'une chaîne de caractère
void morphism_print_image(morphism* M, char* w, FILE* out)
{
    uint s;
    if (morphism_elem_from_string(M, w, &s))
    {
        fprintf(out, "The image of the word %s is ", w);
        morphism_print_name(M, s, out);
        fprintf(out, ".\n");
    }
    else
    {
        printf("Error: this is not a valid word for this morphism.\n");
        return;
    }
} */

/**********************************/
/* Tests de propriétés classiques */
/**********************************/

bool mor_neutral_letter(morphism* M, FILE* out) {
    for (uint b = 0; b < M->r_cayley->size_alpha; b++) {
        if (M->r_cayley->edges[ONE][b] == ONE) {
            if (out) {
                fprintf(out, "#### The letter ");
                fprint_letter_utf8(M->alphabet[b], out);
                fprintf(out, " is neutral.\n");
            }
            return true;
        }
    }
    if (out) {
        fprintf(out, "#### No neutral letter.\n");
    }
    return false;
}


bool mor_nonempty_neutral(morphism* M) {
    green* G = M->rels;
    if (G->HCL->cl_size[G->HCL->numcl[ONE]] == 1) {
        return false;
    }
    return true;
}

bool mor_all_regular(morphism* M) {
    return M->rels->nb_regular_elems == M->r_cayley->size_graph;
}



// Calcule l'image d'un mot par un morphisme.
uint mor_compute_image(morphism* M, word* w) {

    uint temp = ONE;
    for (uint i = 0; i < size_word(w); i++) {
        uint letter_index = mor_letter_index(M, lefread_word(w, i));
        if (letter_index == M->r_cayley->size_alpha) {
            return M->r_cayley->size_graph;
        }
        temp = M->r_cayley->edges[temp][letter_index];
    }
    return temp;
}


dgraph* mor_extract_rcl(morphism* M, uint rcl) {
    // Création du graphe
    dgraph* res = create_dgraph_noedges(M->rels->RCL->cl_size[rcl], M->r_cayley->size_alpha);
    // Pour chaque élément du morphisme
    for (uint i = 0; i < M->rels->RCL->cl_size[rcl]; i++) {
        uint q = M->rels->RCL->cl_elems[rcl][i];
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            uint r = M->r_cayley->edges[q][a];
            if (M->rels->RCL->numcl[r] == rcl) {
                mem_array_sorted(r, M->rels->RCL->cl_elems[rcl], M->rels->RCL->cl_size[rcl], &res->edges[i][a]);
            }
            else {
                res->edges[i][a] = UINT_MAX; // Si l'élément n'est pas dans la classe rcl, on met la taille du graphe
            }
        }
    }
    return res;
}



/*
// Structure pout stocker les paires rencontrée dans le DFA à l'intérieur d'un AVL
struct at_explore
{
    // Élément du monoide
    uint elem;

    // // Taille de l'alphabet
    // uint size_alpha;

    // Alphabet calculé: le ième bit de poids faible détermine si la ième lettre est présente
    ulong thealph;

    // // Pour chaque lettre, un pointeur sur l'état atteint
    // struct at_explore **next;
};

typedef struct at_explore* p_at_explore;

// Fonction de comparaison pour les AVLs
static int at_explore_comp(void* e1, void* e2)
{
    if (((p_at_explore)e1)->thealph < ((p_at_explore)e2)->thealph)
    {
        return 1;
    }
    else if (((p_at_explore)e1)->thealph > ((p_at_explore)e2)->thealph)
    {
        return -1;
    }
    else
    {
        if (((p_at_explore)e1)->elem < ((p_at_explore)e2)->elem)
        {
            return 1;
        }
        else if (((p_at_explore)e1)->elem > ((p_at_explore)e2)->elem)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

// Extraction de l'information de l'AVL une fois le DFS fini
static void at_avl_to_table(avlnode* thetree, dequeue** table)
{
    if (thetree == NULL)
    {
        return;
    }
    else
    {
        at_avl_to_table(thetree->left, table);
        rigins_dequeue(((p_at_explore)thetree->value)->elem, table[((p_at_explore)thetree->value)->thealph]);

        at_avl_to_table(thetree->right, table);
        free(thetree);
    }
} */

/*
dequeue** at_table_cayley(morphism* M)
{
    // Création de la pile pour le DFS
    dequeue_gen* thestack = create_dequeue_gen();

    // Création de l'AVL pour mémoriser les sommets déjà visités
    avlnode* thetree = NULL;

    // Création du sommet initial
    p_at_explore inode;
    MALLOC(inode, 1);
    inode->elem = ONE;
    // inode->size_alpha = M->r_cayley->size_alpha;
    inode->thealph = 0; // Le mot vide a un alphabet vide
    // inode->next = NULL; // On ne connait pas encore les sommets adjacents

    rigins_dequeue_gen(inode, thestack);
    thetree = avl_insert(inode, thetree, &at_explore_comp);

    while (!isempty_dequeue_gen(thestack))
    {
        // On prend un sommet non-traité
        p_at_explore thenode = rigpull_dequeue_gen(thestack);
        // MALLOC(thenode->next, M->r_cayley->size_alpha);
        for (uint a = 0; a < M->r_cayley->size_alpha; a++)
        {
            // printf("Letter %d\n", a);
            // Création du potentiel nouveau sommet
            p_at_explore newnode;
            MALLOC(newnode, 1);
            newnode->elem = M->r_cayley->edges[thenode->elem][a];
            ulong mask = 1 << a;

            newnode->thealph = thenode->thealph | mask;
            // newnode->size_alpha = M->r_cayley->size_alpha;
            // newnode->next = NULL;

            // Recherche dans les sommets déjà visités
            avlnode* found = avl_search(newnode, thetree, &at_explore_comp);

            if (found == NULL)
            {
                // printf("test 1\n");
                // thenode->next[a] = newnode;
                rigins_dequeue_gen(newnode, thestack);
                thetree = avl_insert(newnode, thetree, &at_explore_comp);
                // printf("test 11\n");
            }
            else
            {
                // printf("test 2\n");

                // thenode->next[a] = found->value;
                free(newnode);

                // printf("test 22\n");
            }
        }
    }

    // Création de la table à retourner
    dequeue** theresult;
    ulong result_size = 1 << M->r_cayley->size_alpha;
    MALLOC(theresult, result_size);
    for (uint i = 0; i < result_size; i++)
    {
        theresult[i] = create_dequeue();
    }
    at_avl_to_table(thetree, theresult);
    delete_dequeue_gen(thestack);
    return theresult;
}

void print_at_table(morphism* M, dequeue** table, FILE* out)
{
    uint max_length = 100; // Longueur d'une ligne
    print_title_box(100, false, out, 2, "Computation on the syntactic morphism. For each sub-alphabet, lists the elements", "which are the image of a word with this sub-alphabet.");

    // Récupération d'information pour l'alignement
    ulong table_size = 1 << M->r_cayley->size_alpha;    // Nombre de sous-alphabets
    uint prefix_size = M->r_cayley->size_alpha * 2 + 2; // Longueur-maximale de l'affichage d'un sous-alphabet

    // L'unique élément d'antécédent eps (d'alphabet vide) est 1
    // fprintf(out, "Number of sub-alphabets: %lu\n", table_size);
    fprintf(out, "│∅");
    for (uint i = 0; i < prefix_size - 1; i++)
    {
        fprintf(out, " ");
    }
    fprintf(out, ": 1");
    for (uint i = 0; i < max_length - prefix_size - 3; i++)
    {
        fprintf(out, " ");
    }
    fprintf(out, "│\n");

    // Pour les alphabets non-vides
    for (ulong i = 1; i < table_size; i++)
    {
        fprintf(out, "│{");
        ulong j = i;
        uint length = 1;
        for (uint a = 0; a < M->r_cayley->size_alpha; a++)
        {
            if (j % 2 == 1)
            {
                length++;
                fprintf(out, "%c", a + 'a');
                if (j / 2 != 0)
                {
                    fprintf(out, ",");
                    length++;
                }
            }
            j = j / 2;
        }
        printf("}");
        length++;
        for (uint k = 0; k < prefix_size - length; k++)
        {
            fprintf(out, " ");
        }
        printf(": ");
        mor_print_sub_aligned(M, table[i], max_length, prefix_size + 2, out);
    }

    // Fin du tableau
    print_bot_line(max_length, out);
}
 */
