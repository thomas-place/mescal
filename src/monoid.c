#include "monoid.h"
#include "monoid_display.h"
#include "interrupt.h"
#include "shell_errors.h"
#include "monoid_ideals.h"



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
    delete_dequeue(M->accept_list);

    // Free idempotents.
    free(M->idem_array);
    delete_dequeue(M->idem_list);

    // Free generators.
    free(M->alphabet);

    // Free Green relations.
    delete_green(M->rels);

    // Free jord
    //delete_lgraph(M->j_order);

    // Free the representatives
    free(M->regular_idems);

    /* Optional fields */

    // Free multiplication table (if needed).
    if (M->mult != NULL) {
        for (uint i = 0; i < M->r_cayley->size_graph; i++) {
            free(M->mult[i]);
        }
        free(M->mult);
    }

    // Free monoid order (if needed)
    if (M->order != NULL) {
        for (uint i = 0; i < M->nb_regular_jcl; i++) {
            delete_dequeue(M->order[i]);
        }
    }

    // Free Cayley graphs
    delete_dgraph(M->l_cayley);
    delete_dgraph(M->r_cayley);

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
    MALLOC(sorted_jclass, size_dequeue(G->JCL->cl[i]));
    for (uint j = 0; j < size_dequeue(G->JCL->cl[i]); j++) {
        sorted_jclass[j] = lefread_dequeue(G->JCL->cl[i], j);
    }
    aux_quick_sort_green(sorted_jclass, 0, size_dequeue(G->JCL->cl[i]), G->RCL->numcl, G->LCL->numcl);
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
    MALLOC(GREL->HCL, 1);
    GREL->HCL->size_set = GREL->JCL->size_set;
    MALLOC(GREL->HCL->numcl, GREL->HCL->size_set);


    // For each J-class.
    for (uint cr = 0; cr < GREL->JCL->size_par; cr++) {
        // We store the elements of the J-class in an array.
        // We sort these elements according to their index in the R-class and then in the L-class.
        // This is done to ensure that the elements of the same H-class are contiguous in the array.
        uint* thejclass = green_sorted_jclass(GREL, cr);


        uint elem = thejclass[0];
        GREL->HCL->numcl[elem] = num;
        num++;
        for (uint i = 1; i < size_dequeue(GREL->JCL->cl[cr]); i++) {
            if (GREL->LCL->numcl[thejclass[i]] == GREL->LCL->numcl[elem] && GREL->RCL->numcl[thejclass[i]] == GREL->RCL->numcl[elem]) {
                GREL->HCL->numcl[thejclass[i]] = GREL->HCL->numcl[elem];
            }
            else {
                GREL->HCL->numcl[thejclass[i]] = num;
                num++;
                elem = thejclass[i];
            }
        }
        free(thejclass);

    }

    // We now know the number of H-classes.
    GREL->HCL->size_par = num;

    // It remains to build the table of classes.
    MALLOC(GREL->HCL->cl, GREL->HCL->size_par);

    for (uint c = 0; c < GREL->HCL->size_par; c++) {
        GREL->HCL->cl[c] = create_dequeue();
    }
    for (uint v = 0; v < GREL->HCL->size_set; v++) {
        rigins_dequeue(v, GREL->HCL->cl[GREL->HCL->numcl[v]]);
    }

}




void gr_green_compute(dequeue* idem_list, green* G) {

    // First, we compute the regular elements.
    CALLOC(G->regular_array, G->JCL->size_set);
    G->nb_regular_elems = 0;

    // For each idempotent, we mark all elements of its J-class has regular. We
    // skip an idempotent whose J-class has already been marked as regular.

    for (uint i = 0; i < size_dequeue(idem_list); i++) {
        uint e = lefread_dequeue(idem_list, i);
        uint c = G->JCL->numcl[e];
        if (G->regular_array[lefread_dequeue(G->JCL->cl[c], 0)]) {
            continue;
        }
        for (uint j = 0; j < size_dequeue(G->JCL->cl[c]); j++) {
            G->regular_array[lefread_dequeue(G->JCL->cl[c], j)] = true;
        }
        G->nb_regular_elems += size_dequeue(G->JCL->cl[c]);
    }

    // Building information on groups.
    CALLOC(G->group_array, G->HCL->size_set);
    for (uint i = 0; i < size_dequeue(idem_list); i++) {
        uint j = G->HCL->numcl[lefread_dequeue(idem_list, i)];
        for (uint k = 0; k < size_dequeue(G->HCL->cl[j]); k++) {
            G->group_array[lefread_dequeue(G->HCL->cl[j], k)] = true;
        }
    }
}



void mor_compute_green(morphism* M) {
    if (M->rels) {
        delete_green(M->rels);
    }

    // Initialization of the structure.
    CALLOC(M->rels, 1);


    // Computing the R equivalence (strongly connected components of the right cayley graph).
    M->rels->RCL = dtarjan(M->r_cayley);

    // Computing the L equivalence (strongly connected components of the left cayley graph).
    M->rels->LCL = dtarjan(M->l_cayley);


    // Computing the J equivalence (strongly connected components of the J order).
    graph* j_order = ldgraphs_to_graph(0, 0, 2, 2, M->r_cayley, M->l_cayley); // Compute the J-order.
    M->rels->JCL = tarjan(j_order);
    delete_graph(j_order);


    // Computing the relation H.
    h_green_compute(M->rels);


    // Computing the regular elements and the groups.
    gr_green_compute(M->idem_list, M->rels);
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
    for (uint i = 0; i < size_dequeue(M->idem_list);i++) {
        uint e = rigread_dequeue(M->idem_list, i);
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
            for (uint j = 0; j < size_dequeue(M->rels->JCL->cl[i]); j++) {
                uint s = lefread_dequeue(M->rels->JCL->cl[i], j);
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
        uint s = lefread_dequeue(M->rels->JCL->cl[i], 0);
        if (regular_jcls_idems[i] != UINT_MAX && !visited[s]) {
            M->regular_idems[M->nb_min_regular_jcl] = regular_jcls_idems[i];
            M->nb_min_regular_jcl++;
        }
    }

    uint j = M->nb_min_regular_jcl;
    for (uint i = 0; i < M->rels->JCL->size_par; i++) {
        uint s = lefread_dequeue(M->rels->JCL->cl[i], 0);
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

// Representation of a state of the morphism graph built from a
// complete DFA and of the outgoing transitions from this state.
typedef struct morphism_state {
    uint size_graph;              //!< Number of states of the DFA used in the construction.
    uint* state;                  //!< A state: a permutation of {0, ..., size_graph-1}.
    uint pred_ele;                //!< The preceding element of the state.
    uint pred_lab;                //!< The preceding letter of the state.
    uint size_alpha;              //!< The size of the alphabet.
    uint num;                     //!< The number of the state in the future monoid.
    struct morphism_state** next; //!< Array of size size_alpha containing the transitions, one for each letter.
} morphism_state;

// Release function
static void delete_morphism_state(void* p) {
    if (p == NULL) {
        return;
    }
    free(((morphism_state*)p)->state); // Free the permutation
    free(((morphism_state*)p)->next);  // Free the transition table
    free(p);
}

static void delete_morphism_state_weak(void* p) {
    if (p == NULL) {
        return;
    }
    free(((morphism_state*)p)->next);
    free(p);
}

static void delete_morphism_state_strong(void* p) {
    if (p == NULL) {

        return;
    }
    free(((morphism_state*)p)->state);
    free(((morphism_state*)p)->next);
    free(p);
}


// Function comparing two objects of type morphism_state. Useful for comparing states in AVLs.
static int comp_morphism_state(void* p1, void* p2) {
    if (((morphism_state*)p1)->size_graph != ((morphism_state*)p2)->size_graph) {
        fprintf(stderr, "Error, the morphism states should have the same size.\n");
        exit(EXIT_FAILURE);
    }
    for (uint i = 0; i < ((morphism_state*)p1)->size_graph; i++) {
        if (((morphism_state*)p1)->state[i] < ((morphism_state*)p2)->state[i]) {
            return 1;
        }
        if (((morphism_state*)p1)->state[i] > ((morphism_state*)p2)->state[i]) {
            return -1;
        }
    }
    return 0;
}

// Auxiliary function used in dfa_to_morphism. The AVL contains morphism states.
static void morphism_avl_to_table(avlnode* tree, nfa* A, morphism* thegraph, uint** permuts, bool* newfinals, bool* theidems) {
    if (tree == NULL) {
        return;
    }

    morphism_avl_to_table(tree->left, A, thegraph, permuts, newfinals, theidems);
    morphism_avl_to_table(tree->right, A, thegraph, permuts, newfinals, theidems);

    morphism_state* thestate = (morphism_state*)tree->value;

    // Assign transitions in the right Cayley graph.
    for (uint a = 0; a < thegraph->r_cayley->size_alpha; a++) {
        thegraph->r_cayley->edges[thestate->num][a] = thestate->next[a]->num;
    }

    // Is the state an accepting element?
    if (mem_dequeue_sorted(thestate->state[lefread_dequeue(A->initials, 0)], A->finals, NULL)) {
        newfinals[thestate->num] = true;
    }
    else {
        newfinals[thestate->num] = false;
    }

    // Is the state an idempotent?
    theidems[thestate->num] = true;
    uint q = 0;
    while (theidems[thestate->num] && q < thestate->size_graph) {
        if (thestate->state[q] != thestate->state[thestate->state[q]]) {
            theidems[thestate->num] = false;
        }
        else {
            q++;
        }
    }

    // Store the name of the element in the morphism.
    thegraph->pred_ele[thestate->num] = thestate->pred_ele;
    thegraph->pred_lab[thestate->num] = thestate->pred_lab;

    // We save the permutation defining the state (might be used for ordering computation).
    permuts[thestate->num] = thestate->state;


}

static void compute_ordering_partial(morphism* M, uint size, bool** order, uint** permuts) {
    MALLOC(M->order, M->nb_regular_jcl);
    for (uint i = 0; i < M->nb_regular_jcl; i++) {
        M->order[i] = create_dequeue();
        uint e = M->regular_idems[i];
        dequeue* eM = compute_r_ideal(M, e, NULL);
        dequeue* Me = compute_l_ideal(M, e, NULL);
        dequeue* eMe = make_inter_sorted_dequeue(eM, Me);
        delete_dequeue(eM);
        delete_dequeue(Me);

        for (uint j = 0; j < size_dequeue(eMe); j++) {
            uint q = lefread_dequeue(eMe, j);
            bool found = true;
            for (uint k = 0; k < size; k++) {
                if (!order[permuts[e][k]][permuts[q][k]]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                rigins_dequeue(q, M->order[i]);
            }
        }


    }

}

// Works only with a complete input DFA
morphism* dfa_to_morphism(nfa* A, bool** order, int* error) {
    if (!nfa_is_det(A) || !nfa_is_comp(A)) {
        fprintf(stderr, "Error: The construction of the transition morphism requires a Complete DFA. Returned NULL\n");
        if (error != NULL) {
            *error = INVALID_OBJECT;
        }
        return NULL;
    }

    // Fifo containing the elements to be processed.
    dequeue_gen* thequeue = create_dequeue_gen();

    // AVL containing all elements already constructed.
    avlnode* thetree = NULL;

    // Count the number of elements constructed.
    uint num = 0;

    // Stack the first element: the identity (which has number 0).
    morphism_state* neutral;
    MALLOC(neutral, 1);
    neutral->num = num;
    num++;
    //neutral->name = create_dequeue();           // The name of this element is the empty word.
    neutral->size_alpha = A->trans->size_alpha; // Number of letters.
    neutral->size_graph = A->trans->size_graph; // Number of states.
    neutral->next = NULL;                       // The next elements are not yet computed.
    MALLOC(neutral->state, neutral->size_graph);
    for (uint q = 0; q < neutral->size_graph; q++) // This element is the identity.
    {
        neutral->state[q] = q;
    }

    // Insertion in the set of already met elements.
    thetree = avl_insert(neutral, thetree, &comp_morphism_state, NULL);

    // Treat this element.
    lefins_dequeue_gen(neutral, thequeue);

    // Potentially long loop, react to interrupts and timeout.
    listening_mode(timeout_value);

    while (!isempty_dequeue_gen(thequeue)) {
        // Free allocated memory if the limit is reached, or if a yser
        // interruption occurs, or if the timeout is reached.
        if ((limit_value > 0 && num > limit_value) || interrupt_flag || timeout_flag) {
            delete_dequeue_gen(thequeue);
            avl_free_strong(thetree, &delete_morphism_state);

            if (error != NULL) {
                if ((limit_value > 0 && num > limit_value)) {
                    *error = MEMORY_LIMIT;
                }
                if (interrupt_flag) {
                    INFO("INTERRUPT");
                    *error = INTERRUPTION;
                }
                if (timeout_flag) {
                    *error = TIMEOUT_OCCURRED;
                }

                // Go back to normal mode, where interruptions and timeouts are ignored.
                normal_mode();
                return NULL;
            }
        }

        // Get the element to be processed.
        morphism_state* theelem = (morphism_state*)rigpull_dequeue_gen(thequeue);

        // Compute the transitions from this element.
        MALLOC(theelem->next, theelem->size_alpha);

        for (uint a = 0; a < theelem->size_alpha; a++) {
            // Create the (potential) new element, computed by multiplying the old one on the right.
            morphism_state* new;
            MALLOC(new, 1);
            new->size_graph = A->trans->size_graph;
            MALLOC(new->state, new->size_graph);

            // Compute the corresponding permutation.
            for (uint q = 0; q < neutral->size_graph; q++) {
                new->state[q] = lefread_dequeue(A->trans->edges[theelem->state[q]][a], 0);
            }

            // Check if this state already exists.
            avlnode* thenode = avl_search(new, thetree, &comp_morphism_state);

            // If not, initialize a new element.
            if (thenode == NULL) {
                new->size_alpha = A->trans->size_alpha;
                new->next = NULL; // For now, we don't know the next elements.
                new->num = num;
                num++;
                new->pred_ele = theelem->num; // The predecessor of this element is the one we are treating.
                new->pred_lab = a;           // The predecessor letter is the one we are treating.
                theelem->next[a] = new;                                         // Assign it as a successor for letter a.
                thetree = avl_insert(new, thetree, &comp_morphism_state, NULL); // Store it in the set of known elements.
                lefins_dequeue_gen(new, thequeue);                              // This new element has to be treated in the future..
            }
            else
                // The element was already constructed.
            {
                theelem->next[a] = ((morphism_state*)thenode->value); // We assign the version already built as an a-successor.
                free(new->state);                                      // We free the copy we've just created.
                free(new);
            }
        }
    }
    delete_dequeue_gen(thequeue);

    // Construction of the morphism.
    morphism* M;
    CALLOC(M, 1);
    M->alphabet = nfa_duplicate_alpha(A);                           // Copy letter names.
    M->r_cayley = create_dgraph_noedges(num, A->trans->size_alpha); // Create the graph.
    MALLOC(M->pred_ele, num);                                         // Predecessor elements.
    MALLOC(M->pred_lab, num);                                         // Predecessor letters.
    MALLOC(M->accept_array, num);                                   // Array representing the accepting set.
    MALLOC(M->idem_array, num);                                     // Array of idempotents.

    uint** permuts;
    MALLOC(permuts, num); // Array of permutations defining the elements.
    // Creation of transitions, idempotents and detection of accepting elements (simultaneously releases the AVL).
    morphism_avl_to_table(thetree, A, M, permuts, M->accept_array, M->idem_array);


    // Free the AVL
    avl_free_strong(thetree, &delete_morphism_state_weak);


    // Creation of lists of idempotents and accepting elements.
    M->idem_list = create_dequeue();
    M->accept_list = create_dequeue();
    for (uint i = 0; i < num; i++) {
        if (M->accept_array[i]) {
            rigins_dequeue(i, M->accept_list);
        }
        if (M->idem_array[i]) {
            rigins_dequeue(i, M->idem_list);
        }
    }

    // Compute the left Cayley graph.
    mor_compute_leftcayley(M);


    // Compute the Green relations.
    //M->j_order = ldgraphs_to_lgraph(0, 2, 2, M->r_cayley, M->l_cayley); // Compute the J-order.


    mor_compute_green(M);


    mor_compute_rep(M);


    if (order) {
        compute_ordering_partial(M, A->trans->size_graph, order, permuts);

    }


    // Release of the permuts table
    for (uint i = 0; i < num; i++) {
        free(permuts[i]);
    }
    free(permuts);


    ignore_interrupt();
    normal_mode();
    return M;
}






int dfa_to_morphism_opti(nfa* A) {

    // Fifo containing the elements to be processed.
    dequeue_gen* thequeue = create_dequeue_gen();

    // AVL containing all elements already constructed.
    avlnode* thetree = NULL;

    // Count the number of elements constructed.
    uint num = 0;

    // Stack the first element: the identity (which has number 0).
    morphism_state* neutral;
    MALLOC(neutral, 1);
    neutral->num = num;
    num++;
    //neutral->name = create_dequeue();           // The name of this element is the empty word.
    neutral->size_alpha = A->trans->size_alpha; // Number of letters.
    neutral->size_graph = A->trans->size_graph; // Number of states.
    neutral->next = NULL;                       // The next elements are not yet computed.
    MALLOC(neutral->state, neutral->size_graph);
    for (uint q = 0; q < neutral->size_graph; q++) // This element is the identity.
    {
        neutral->state[q] = q;
    }

    // Insertion in the set of already met elements.
    thetree = avl_insert(neutral, thetree, &comp_morphism_state, NULL);

    // Treat this element.
    lefins_dequeue_gen(neutral, thequeue);


    while (!isempty_dequeue_gen(thequeue)) {
        // Free allocated memory if the limit is reached, or if a yser
        // interruption occurs, or if the timeout is reached.


        // Get the element to be processed.
        morphism_state* theelem = (morphism_state*)rigpull_dequeue_gen(thequeue);

        // Compute the transitions from this element.
        MALLOC(theelem->next, theelem->size_alpha);

        for (uint a = 0; a < theelem->size_alpha; a++) {
            // Create the (potential) new element, computed by multiplying the old one on the right.
            morphism_state* new;
            MALLOC(new, 1);
            new->size_graph = A->trans->size_graph;
            MALLOC(new->state, new->size_graph);

            // Compute the corresponding permutation.
            for (uint q = 0; q < neutral->size_graph; q++) {
                new->state[q] = lefread_dequeue(A->trans->edges[theelem->state[q]][a], 0);
            }

            // Check if this state already exists.
            avlnode* thenode = avl_search(new, thetree, &comp_morphism_state);

            // If not, initialize a new element.
            if (thenode == NULL) {
                new->size_alpha = A->trans->size_alpha;
                new->next = NULL; // For now, we don't know the next elements.
                new->num = num;
                num++;
                new->pred_ele = theelem->num; // The predecessor of this element is the one we are treating.
                new->pred_lab = a;           // The predecessor letter is the one we are treating.
                theelem->next[a] = new;                                         // Assign it as a successor for letter a.
                thetree = avl_insert(new, thetree, &comp_morphism_state, NULL); // Store it in the set of known elements.
                lefins_dequeue_gen(new, thequeue);                              // This new element has to be treated in the future..
            }
            else
                // The element was already constructed.
            {
                theelem->next[a] = ((morphism_state*)thenode->value); // We assign the version already built as an a-successor.
                free(new->state);                                      // We free the copy we've just created.
                free(new);
            }
        }
    }
    delete_dequeue_gen(thequeue);





    // Free the AVL
    avl_free_strong(thetree, &delete_morphism_state_strong);



    return num;
}

nfa* morphism_to_dfa(morphism* M) {
    nfa* A = nfa_init();
    A->alphabet = mor_duplicate_alpha(M);
    A->trans = dgraph_to_lgraph(M->r_cayley);
    rigins_dequeue(0, A->initials);
    copy_dequeue_right(A->finals, M->accept_list, 0);
    return A;
}

nfa* left_morphism_to_dfa(morphism* M) {
    nfa* A = nfa_init();
    A->alphabet = mor_duplicate_alpha(M);
    A->trans = dgraph_to_lgraph(M->l_cayley);
    rigins_dequeue(0, A->initials);
    copy_dequeue_right(A->finals, M->accept_list, 0);
    return A;
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
        MALLOC(M->mult[q], M->r_cayley->size_graph);

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
            M->mult[q][s] = qs;
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
    for (uint i = 0; i < size_dequeue(M->accept_list); i++)
    {
        for (uint q = 0; q < thesize; q++) {
            // We skip q if it is an accepting element.
            if (M->accept_array[q]) {
                continue;
            }
            // We loop over the accepting elements.

            rigins_dequeue(lefread_dequeue(M->accept_list, i), stack_one);
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
        return M->mult[s][t];
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
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        if (lefread_dequeue(M->idem_list, i) == s) {
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
    if (size_dequeue(G->HCL->cl[G->HCL->numcl[ONE]]) == 1) {
        return false;
    }
    return true;
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
