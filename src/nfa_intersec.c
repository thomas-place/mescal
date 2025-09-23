/************************/
/* Intersection of NFAs */
/************************/

#include "nfa_intersec.h"
#include "type_hash.h"
#include <limits.h>


// Storage of the pairs for the product automaton construction.
static uint inter_size = 0; // Full size of the inter_pairs array.
static uint inter_elem = 0; // Number of tuples stored in the inter_pairs array.
static uint inter_auto = 0; // Sum of the number of states in the two NFAs to be intersected.
static uint inter_alph = 0; // Size of the alphabet of the product automaton.
static uint inter_copy = 0; // Number of copies of the product automaton (used in the power product construction).
static uint* inter_pairs = NULL; // Array that stores the pairs of states (a single set uses 2 cells). Size: inter_size * 2.
static uint* inter_trans = NULL; //!< Array that stores the computed transitions. Size: inter_size * inter_alph.
static uint* inter_back = NULL; //!< Array that stores the computed transitions in the reverse direction. Size: inter_size * 2.

// Initialize the arrays used in the product construction.
static void inter_init(uint size, uint states1, uint states2, uint asize) {
    size = max(size, 2); // Ensures that the size is at least 2.
    inter_size = size;
    inter_elem = 0;
    inter_alph = asize;
    inter_auto = states1 + states2;
    MALLOC(inter_pairs, inter_size << 1);
    MALLOC(inter_trans, inter_size * inter_alph);
}

static void inter_init_power(uint size, uint states, uint asize, uint num) {
    size = max(size, 2); // Ensures that the size is at least 2.
    inter_size = size;
    inter_elem = 0;
    inter_alph = asize;
    inter_auto = states;
    inter_copy = num;
    MALLOC(inter_pairs, inter_size * inter_copy);
    MALLOC(inter_trans, inter_size * inter_alph);
}



static void inter_init_weak(uint size, uint states1, uint states2, uint asize) {
    size = max(size, 2); // Ensures that the size is at least 2.
    inter_size = size;
    inter_elem = 0;
    inter_alph = asize;
    inter_auto = states1 + states2;
    MALLOC(inter_pairs, inter_size << 1);
    MALLOC(inter_back, inter_size << 1);
}

// Deletes the arrays used in the product construction and resets the variables.
static void inter_delete() {
    free(inter_pairs);
    free(inter_trans);
    free(inter_back);
    inter_pairs = NULL;
    inter_trans = NULL;
    inter_back = NULL;
    inter_size = 0;
    inter_elem = 0;
    inter_auto = 0;
    inter_alph = 0;
    inter_copy = 0;
}


// Doubles the size of the arrays used in the subset construction if necessary.
static void inter_grow() {
    if (inter_size <= inter_elem) {
        inter_size <<= 1;
        REALLOC(inter_pairs, inter_size << 1);
        REALLOC(inter_trans, inter_size * inter_alph);
    }
}

static void inter_grow_power() {
    if (inter_size <= inter_elem) {
        inter_size <<= 1;
        REALLOC(inter_pairs, inter_size * inter_copy);
        REALLOC(inter_trans, inter_size * inter_alph);
    }
}

static void inter_grow_weak() {
    if (inter_size <= inter_elem) {
        inter_size <<= 1;
        REALLOC(inter_pairs, inter_size << 1);
        REALLOC(inter_back, inter_size << 1);
    }
}

// Checks if two pairs stored in the inter_pairs array are equal.
static bool inter_equal(uint i, uint j) {
    uint zi = i << 1; // The index of the first element.
    uint zj = j << 1; // The index of the second element.
    return (inter_pairs[zi] == inter_pairs[zj]) && (inter_pairs[zi + 1] == inter_pairs[zj + 1]);
}

static bool inter_equal_power(uint i, uint j) {
    uint zi = i * inter_copy; // The index of the first element.
    uint zj = j * inter_copy; // The index of the second element.
    for (uint k = 0; k < inter_copy; k++) {
        if (inter_pairs[zi + k] != inter_pairs[zj + k]) {
            return false; // If any of the copies are not equal, return false.
        }
    }
    return true;
}

// Hash function for the pairs stored in the inter_pairs array.
static uint inter_hash(uint i, uint size_hash) {
    uint e = i << 1; // The index of the element in the mor_perms array.
    uint hash = 0;

    uint a = 0x9e3779b9; // fractional bits of the golden ratio


    hash = inter_pairs[e] * a % size_hash; // Hashing the first element of the pair
    hash = (hash * (inter_auto + 1) + inter_pairs[e + 1] * a) % size_hash; // Hashing the second element of the pair
    return hash;
}


static uint inter_hash_power(uint i, uint size_hash) {
    uint e = i * inter_copy; // The index of the element in the mor_perms array.
    uint hash = 0;

    uint a = 0x9e3779b9; // fractional bits of the golden ratio
    for (uint n = 0; n < inter_copy; n++)
    {
        hash = (hash * (inter_auto + 1) + inter_pairs[e + n] * a) % size_hash;
    }
    return hash;
}

// Intersection
nfa* nfa_intersect(nfa* A1, nfa* A2, bool names) {

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Intersection of two NFAs.\n");
#endif

    // On étend les alphabet des deux automates
    A1 = nfa_copy_exalpha(A1, A2->alphabet, A2->trans->size_alpha);
    A2 = nfa_copy_exalpha(A2, A1->alphabet, A1->trans->size_alpha);


    // Calcul du nouvel alphabet
    uint thealph = A1->trans->size_alpha;


    // On teste si il existe des transitions epsilon dans l'un des deux automates
    if (A1->trans_e || A2->trans_e)
    {
        if (!A1->trans_e) {
            A1->trans_e = create_graph_noedges(A1->trans->size_graph);
        }
        if (!A2->trans_e) {
            A2->trans_e = create_graph_noedges(A2->trans->size_graph);
        }
    }

    // On teste si il existe des transitions epsilon dans les deux automates
    if (!A1->trans_i || !A2->trans_i) // Si au moins l'un des NFAs n'a pas de transitions inverses
    {
        delete_lgraph(A1->trans_i); // On supprime les transitions inverses dans les deux NFAs
        A1->trans_i = NULL;
        delete_lgraph(A2->trans_i);
        A2->trans_i = NULL;
    }


    uchar power = get_uint_lbinary(A1->trans->size_graph + A2->trans->size_graph) + 2; // We compute the initial power of two for the size of the hash table.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    inter_init(thesize, A1->trans->size_graph, A2->trans->size_graph, thealph); // Initialize the arrays used in the product construction.

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &inter_hash, &inter_equal);

    // Stack containing the elements to be processed.
    dequeue* thestack = create_dequeue();

    // Create the first element: the set of initial states of the NFA.
    for (uint i1 = 0; i1 < size_dequeue(A1->initials); i1++) {
        for (uint i2 = 0; i2 < size_dequeue(A2->initials); i2++) {
            uint q = lefread_dequeue(A1->initials, i1); // Read the first initial state from A1
            uint r = lefread_dequeue(A2->initials, i2); // Read the second initial state from A2
            inter_pairs[inter_elem << 1] = q; // Store the first state in the pair
            inter_pairs[(inter_elem << 1) + 1] = r; // Store the second state in the pair
            hash_table_insert(thehash, inter_elem); // Insert this pair in the hash table.
            rigins_dequeue(inter_elem, thestack); // Add the pair to the stack.
            inter_elem++; // Increment the number of pairs constructed
            inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.
        }
    }

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Initial states done.\n");
#endif

    // DFS
    while (!isempty_dequeue(thestack)) {
        uint i = rigpull_dequeue(thestack); // Retrieve the pair to be processed.

        uint q1 = inter_pairs[i << 1]; // The first state of the pair.
        uint q2 = inter_pairs[(i << 1) + 1]; // The second state of the pair.

        // Handling standard transitions
        for (uint a = 0; a < thealph; a++) {
            for (uint i1 = 0; i1 < size_dequeue(A1->trans->edges[q1][a]); i1++) {
                for (uint i2 = 0; i2 < size_dequeue(A2->trans->edges[q2][a]); i2++) {
                    inter_pairs[inter_elem << 1] = lefread_dequeue(A1->trans->edges[q1][a], i1); // Read the first state from A1
                    inter_pairs[(inter_elem << 1) + 1] = lefread_dequeue(A2->trans->edges[q2][a], i2); // Read the second state from A2
                    uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                    if (j == inter_elem) { // If the pair was not already constructed
                        rigins_dequeue(inter_elem, thestack); // We add it to the stack for future processing.
                        inter_elem++; // Increment the number of pairs constructed
                        inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.
                    }
                }
            }
        }

        // Handling epsilon transitions
        if (A1->trans_e)
        {
            for (uint i2 = 0; i2 < size_dequeue(A2->trans_e->edges[q2]); i2++) {
                inter_pairs[inter_elem << 1] = q1; // Read the first state from A1
                inter_pairs[(inter_elem << 1) + 1] = lefread_dequeue(A2->trans_e->edges[q2], i2); // Read the second state from A2
                uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                if (j == inter_elem) { // If the pair was not already constructed
                    rigins_dequeue(inter_elem, thestack); // We add it to the stack for future processing.
                    inter_elem++; // Increment the number of pairs constructed
                    inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.
                }
            }
            for (uint i1 = 0; i1 < size_dequeue(A1->trans_e->edges[q1]); i1++) {
                inter_pairs[inter_elem << 1] = lefread_dequeue(A1->trans_e->edges[q1], i1); // Read the first state from A1
                inter_pairs[(inter_elem << 1) + 1] = q2; // Read the second state from A2
                uint h = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                if (h == inter_elem) { // If the pair was not already constructed
                    rigins_dequeue(inter_elem, thestack); // We add it to the stack for future processing.
                    inter_elem++; // Increment the number of pairs constructed
                    inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.
                }
                for (uint i2 = 0; i2 < size_dequeue(A2->trans_e->edges[q2]); i2++) {
                    inter_pairs[inter_elem << 1] = lefread_dequeue(A1->trans_e->edges[q1], i1); // Read the first state from A1
                    inter_pairs[(inter_elem << 1) + 1] = lefread_dequeue(A2->trans_e->edges[q2], i2); // Read the second state from A2
                    uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                    if (j == inter_elem) { // If the pair was not already constructed
                        rigins_dequeue(inter_elem, thestack); // We add it to the stack for future processing.
                        inter_elem++; // Increment the number of pairs constructed
                        inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.
                    }
                }
            }
        }

        // Handling inverse transitions
        if (A1->trans_i)
        {
            for (uint a = 0; a < thealph; a++) {
                for (uint i1 = 0; i1 < size_dequeue(A1->trans_i->edges[q1][a]); i1++) {
                    for (uint i2 = 0; i2 < size_dequeue(A2->trans_i->edges[q2][a]); i2++) {
                        inter_pairs[inter_elem << 1] = lefread_dequeue(A1->trans_i->edges[q1][a], i1); // Read the first state from A1
                        inter_pairs[(inter_elem << 1) + 1] = lefread_dequeue(A2->trans_i->edges[q2][a], i2); // Read the second state from A2
                        uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                        if (j == inter_elem) { // If the pair was not already constructed
                            rigins_dequeue(inter_elem, thestack); // We add it to the stack for future processing.
                            inter_elem++; // Increment the number of pairs constructed
                            inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.
                        }
                    }
                }
            }
        }
    }
    // DFS completed. We can delete the stack and the hash table.
    delete_dequeue(thestack);

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### DFS Done.\n");
#endif


    // Construction of the product NFA.
    nfa* PROD;
    CALLOC(PROD, 1); // Allocate memory for the product NFA
    PROD->initials = create_dequeue(); // Create the dequeue for the initial states of the product NFA
    PROD->finals = create_dequeue(); // Create the dequeue for the final states of the product NFA

    PROD->alphabet = duplicate_alphabet(A1->alphabet, A1->trans->size_alpha); // The alphabet of the product NFA is the same as the alphabets of A1 and A2.

    // Initial states.
    uint num = 0;
    for (uint i1 = 0; i1 < size_dequeue(A1->initials); i1++) {
        for (uint i2 = 0; i2 < size_dequeue(A2->initials); i2++) {
            rigins_dequeue(num, PROD->initials); // Add the initial state to the product NFA
            num++; // Increment the number of initial states in the product NFA
        }
    }




    // Construction du nouveau NFA (continued)
    PROD->trans = create_lgraph_noedges(inter_elem, thealph); // Création du graph
    for (uint p = 0; p < inter_elem; p++) {
        uint q1 = inter_pairs[p << 1]; // The first state of the pair.
        uint q2 = inter_pairs[(p << 1) + 1]; // The second state of the pair.

        // We fill the transitions table
        for (uint a = 0; a < thealph; a++) {
            for (uint i1 = 0; i1 < size_dequeue(A1->trans->edges[q1][a]); i1++) {
                for (uint i2 = 0; i2 < size_dequeue(A2->trans->edges[q2][a]); i2++) {
                    inter_pairs[inter_elem << 1] = lefread_dequeue(A1->trans->edges[q1][a], i1); // Read the first state from A1
                    inter_pairs[(inter_elem << 1) + 1] = lefread_dequeue(A2->trans->edges[q2][a], i2); // Read the second state from A2
                    uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                    rigins_dequeue(j, PROD->trans->edges[p][a]); // We add it to the stack for future processing.
                }
            }
            sort_dequeue_norepeat(PROD->trans->edges[p][a]);
        }
    }


    if (A1->trans_e)
    {
        PROD->trans_e = create_graph_noedges(inter_elem);
        for (uint p = 0; p < inter_elem; p++) {
            uint q1 = inter_pairs[p << 1]; // The first state of the pair.
            uint q2 = inter_pairs[(p << 1) + 1]; // The second state of the pair.

            for (uint i2 = 0; i2 < size_dequeue(A2->trans_e->edges[q2]); i2++) {
                inter_pairs[inter_elem << 1] = q1; // Read the first state from A1
                inter_pairs[(inter_elem << 1) + 1] = lefread_dequeue(A2->trans_e->edges[q2], i2); // Read the second state from A2
                uint j = hash_table_insert(thehash, inter_elem); // Get the destination index for the pair in the hash table.
                rigins_dequeue(j, PROD->trans_e->edges[p]);
            }
            for (uint i1 = 0; i1 < size_dequeue(A1->trans_e->edges[q1]); i1++) {
                inter_pairs[inter_elem << 1] = lefread_dequeue(A1->trans_e->edges[q1], i1); // Read the first state from A1
                inter_pairs[(inter_elem << 1) + 1] = q2; // Read the second state from A2
                uint h = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                rigins_dequeue(h, PROD->trans_e->edges[p]);
                for (uint i2 = 0; i2 < size_dequeue(A2->trans_e->edges[q2]); i2++) {
                    inter_pairs[inter_elem << 1] = lefread_dequeue(A1->trans_e->edges[q1], i1); // Read the first state from A1
                    inter_pairs[(inter_elem << 1) + 1] = lefread_dequeue(A2->trans_e->edges[q2], i2); // Read the second state from A2
                    uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                    rigins_dequeue(j, PROD->trans_e->edges[p]);
                }
            }
            sort_dequeue_norepeat(PROD->trans_e->edges[p]);
        }
    }
    else
    {
        PROD->trans_e = NULL;
    }
    if (A1->trans_i)
    {
        PROD->trans_i = create_lgraph_noedges(inter_elem, thealph);
        for (uint p = 0; p < inter_elem; p++) {
            uint q1 = inter_pairs[p << 1]; // The first state of the pair.
            uint q2 = inter_pairs[(p << 1) + 1]; // The second state of the pair.

            // We fill the transitions table
            for (uint a = 0; a < thealph; a++) {
                for (uint i1 = 0; i1 < size_dequeue(A1->trans_i->edges[q1][a]); i1++) {
                    for (uint i2 = 0; i2 < size_dequeue(A2->trans_i->edges[q2][a]); i2++) {
                        inter_pairs[inter_elem << 1] = lefread_dequeue(A1->trans_i->edges[q1][a], i1); // Read the first state from A1
                        inter_pairs[(inter_elem << 1) + 1] = lefread_dequeue(A2->trans_i->edges[q2][a], i2); // Read the second state from A2
                        uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
                        rigins_dequeue(j, PROD->trans_i->edges[p][a]); // We add it to the stack for future processing.
                    }
                }
                sort_dequeue_norepeat(PROD->trans_i->edges[p][a]);
            }
        }
    }
    else
    {
        PROD->trans_i = NULL;
    }

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Transitions done.\n");
#endif

    // Final states.
    bool* finals1;
    bool* finals2;
    CALLOC(finals1, A1->trans->size_graph);
    CALLOC(finals2, A2->trans->size_graph);
    for (uint i1 = 0; i1 < size_dequeue(A1->finals); i1++) {
        finals1[lefread_dequeue(A1->finals, i1)] = true; // Mark it as final
    }
    for (uint i2 = 0; i2 < size_dequeue(A2->finals); i2++) {
        finals2[lefread_dequeue(A2->finals, i2)] = true; // Mark it as final
    }

    for (uint p = 0; p < inter_elem; p++) {
        uint q1 = inter_pairs[p << 1]; // The first state of the pair.
        uint q2 = inter_pairs[(p << 1) + 1]; // The second state of the pair.

        if (finals1[q1] && finals2[q2]) { // If both states are final
            rigins_dequeue(p, PROD->finals); // Add the pair to the list of final states
        }
    }
    free(finals1); // Free the memory allocated for the final states of A1
    free(finals2); // Free the memory allocated for the final states of A2
    // We can now delete the hash table.
    delete_hash_table(thehash);

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Final states done.\n");
#endif

    if (names) {                                       // Si on doit mémoriser les noms
        MALLOC(PROD->state_names, inter_elem);
        char buffer[256]; // Buffer for state names
        for (uint p = 0; p < inter_elem; p++) {
            uint q1 = inter_pairs[p << 1]; // The first state of the pair.
            uint q2 = inter_pairs[(p << 1) + 1]; // The second state of the pair.
            if (A1->state_names && A2->state_names) { // If both NFAs have names
                sprintf(buffer, "%s,%s", A1->state_names[q1], A2->state_names[q2]);
            }
            else if (A1->state_names) { // If only the first NFA has names
                sprintf(buffer, "%s,%d", A1->state_names[q1], q2);
            }
            else if (A2->state_names) { // If only the second NFA has names
                sprintf(buffer, "%d,%s", q1, A2->state_names[q2]);
            }
            else { // If neither NFA has names
                sprintf(buffer, "%d,%d", q1, q2);
            }
            PROD->state_names[p] = strdup(buffer); // Store the name in the product NFA
        }
    }
    else {
        PROD->state_names = NULL;
    }

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Fully done.\n");
#endif


    inter_delete();
    nfa_delete(A1);
    nfa_delete(A2);
    return PROD;
}










// Intersection
dfa* dfa_intersect(dfa* A1, dfa* A2, bool names) {

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Intersection of two DFAs.\n");
#endif

    // On étend les alphabet des deux automates
    A1 = dfa_copy_exalpha(A1, A2->alphabet, A2->trans->size_alpha);
    A2 = dfa_copy_exalpha(A2, A1->alphabet, A1->trans->size_alpha);


    // Calcul du nouvel alphabet
    uint thealph = A1->trans->size_alpha;



    uchar power = get_uint_lbinary(A1->trans->size_graph + A2->trans->size_graph) + 2; // We compute the initial power of two for the size of the hash table.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    inter_init(thesize, A1->trans->size_graph, A2->trans->size_graph, thealph); // Initialize the arrays used in the product construction.

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &inter_hash, &inter_equal);

    // Stack containing the elements to be processed.
    dequeue* thestack = create_dequeue();

    // Create the first element: the pair of initial states.

    inter_pairs[0] = A1->initial; // Store the first state in the pair
    inter_pairs[1] = A2->initial; // Store the second state in the pair
    hash_table_insert(thehash, 0); // Insert this pair in the hash table.
    rigins_dequeue(0, thestack); // Add the pair to the stack.
    inter_elem++; // Increment the number of pairs constructed
    inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Initial states done.\n");
#endif

    // DFS
    while (!isempty_dequeue(thestack)) {
        uint i = rigpull_dequeue(thestack); // Retrieve the pair to be processed.

        uint q1 = inter_pairs[i << 1]; // The first state of the pair.
        uint q2 = inter_pairs[(i << 1) + 1]; // The second state of the pair.

        // Handling standard transitions
        for (uint a = 0; a < thealph; a++) {
            inter_pairs[inter_elem << 1] = A1->trans->edges[q1][a]; // Read the first state from A1
            inter_pairs[(inter_elem << 1) + 1] = A2->trans->edges[q2][a]; // Read the second state from A2
            uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
            if (j == inter_elem) { // If the pair was not already constructed
                rigins_dequeue(inter_elem, thestack); // We add it to the stack for future processing.
                inter_elem++; // Increment the number of pairs constructed
                inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.
            }
            inter_trans[i * thealph + a] = j; // Store the transition
        }

    }
    // DFS completed. We can delete the stack and the hash table.
    delete_dequeue(thestack);

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### DFS Done.\n");
#endif


    // Construction of the product NFA.
    dfa* PROD;
    CALLOC(PROD, 1); // Allocate memory for the product DFA
    PROD->alphabet = duplicate_alphabet(A1->alphabet, A1->trans->size_alpha); // The alphabet of the product DFA is the same as the alphabets of A1 and A2.
    PROD->trans = create_dgraph_noedges(inter_elem, thealph); // Create the graph for the product DFA with the number of states and the size of the alphabet.


    // Initial state.
    PROD->initial = 0; // The initial state of the product DFA is the first state in the inter_pairs array.

    // Transitions and final states
    bool* finals1;
    bool* finals2;
    CALLOC(finals1, A1->trans->size_graph);
    CALLOC(finals2, A2->trans->size_graph);
    for (uint i1 = 0; i1 < A1->nb_finals; i1++) {
        finals1[A1->finals[i1]] = true; // Mark it as final
    }
    for (uint i2 = 0; i2 < A2->nb_finals; i2++) {
        finals2[A2->finals[i2]] = true; // Mark it as final
    }
    bool* finals;
    CALLOC(finals, inter_elem); // Allocate memory for the final states of the product DFA
    PROD->nb_finals = 0; // Initialize the number of final states to 0
    for (uint p = 0; p < inter_elem; p++) {
        for (uint a = 0; a < thealph; a++) {
            PROD->trans->edges[p][a] = inter_trans[p * thealph + a];
        }
        uint q1 = inter_pairs[p << 1]; // The first state of the pair.
        uint q2 = inter_pairs[(p << 1) + 1]; // The second state of the pair.
        if (finals1[q1] && finals2[q2]) { // If both states are final
            finals[p] = true;
            PROD->nb_finals++; // Increment the number of final states in the product DFA
        }
    }

    free(finals1); // Free the memory allocated for the final states of A1
    free(finals2); // Free the memory allocated for the final states of A2

    uint num = 0;
    MALLOC(PROD->finals, PROD->nb_finals); // Allocate memory for the final states of the product DFA
    for (uint p = 0; p < inter_elem; p++) {
        if (finals[p]) { // If the state is final
            PROD->finals[num] = p; // Add the pair to the list of final states
            num++; // Increment the number of final states in the product DFA
        }
    }
    free(finals); // Free the memory allocated for the final states of the product DFA


#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Transitions and final states done.\n");
#endif

    delete_hash_table(thehash);


    if (names) {                                       // Si on doit mémoriser les noms
        MALLOC(PROD->state_names, inter_elem);
        char buffer[256]; // Buffer for state names
        for (uint p = 0; p < inter_elem; p++) {
            uint q1 = inter_pairs[p << 1]; // The first state of the pair.
            uint q2 = inter_pairs[(p << 1) + 1]; // The second state of the pair.
            if (A1->state_names && A2->state_names) { // If both NFAs have names
                sprintf(buffer, "%s,%s", A1->state_names[q1], A2->state_names[q2]);
            }
            else if (A1->state_names) { // If only the first NFA has names
                sprintf(buffer, "%s,%d", A1->state_names[q1], q2);
            }
            else if (A2->state_names) { // If only the second NFA has names
                sprintf(buffer, "%d,%s", q1, A2->state_names[q2]);
            }
            else { // If neither NFA has names
                sprintf(buffer, "%d,%d", q1, q2);
            }
            PROD->state_names[p] = strdup(buffer); // Store the name in the product NFA
        }
    }
    else {
        PROD->state_names = NULL;
    }

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Fully done.\n");
#endif


    inter_delete();
    dfa_delete(A1);
    dfa_delete(A2);
    return PROD;
}


void* nfa_intersect_mixed(void* I1, bool is_dfa_I1, void* I2, bool is_dfa_I2, bool names) {
    if (is_dfa_I1 && is_dfa_I2) {
        return dfa_intersect((dfa*)I1, (dfa*)I2, names);
    }

    nfa* A1, * A2;

    if (is_dfa_I1) {
        A1 = dfa_to_nfa((dfa*)I1);
    }
    else {
        A1 = (nfa*)I1;
    }

    if (is_dfa_I2) {
        A2 = dfa_to_nfa((dfa*)I2);
    }
    else {
        A2 = (nfa*)I2;
    }

    nfa* INTER = nfa_intersect(A1, A2, names);

    if (is_dfa_I1) {
        nfa_delete(A1); // Delete the NFA created from the DFA I1
    }
    if (is_dfa_I2) {
        nfa_delete(A2); // Delete the NFA created from the DFA I2
    }

    return INTER;
}

dfa* dfa_power_prod(dfa* A, uint n, int* initial_states, int* final_states) {
    if (n == 0) {
        return NULL;
    }

    uchar power = get_uint_lbinary(A->trans->size_graph) + 2; // We compute the initial power of two for the size of the hash table.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    inter_init_power(thesize, A->trans->size_graph, A->trans->size_alpha, n); // Initialize the arrays used in the product construction.

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &inter_hash_power, &inter_equal_power);

    // Stack containing the elements to be processed.
    dequeue* thestack = create_dequeue();

    // Create the first element: the tuple of initial states.



    for (uint i = 0; i < n; i++) {
        inter_pairs[i] = (uint)initial_states[i]; // Read the initial state from the array
    }
    hash_table_insert(thehash, 0); // Insert this tuple in the hash table.
    rigins_dequeue(0, thestack); // Add the tuple to the stack.
    inter_elem++; // Increment the number of tuples constructed
    inter_grow_power(); // If the number of tuples constructed is larger than the size of the table, we double the size.

    // DFS
    while (!isempty_dequeue(thestack)) {
        uint i = rigpull_dequeue(thestack); // Retrieve the pair to be processed.

        // Handling transitions
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            for (uint h = 0; h < n; h++) {
                inter_pairs[inter_elem * n + h] = A->trans->edges[inter_pairs[i * n + h]][a]; // Read the transitions for the current state
            }
            uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
            if (j == inter_elem) { // If the pair was not already constructed
                rigins_dequeue(inter_elem, thestack); // We add it to the stack for future processing.
                inter_elem++; // Increment the number of pairs constructed
                inter_grow_power(); // If the number of pairs constructed is larger than the size of the table, we double the size.
            }
            inter_trans[i * A->trans->size_alpha + a] = j; // Store the transition
        }

    }
    delete_dequeue(thestack); // DFS completed. We can delete the stack and the hash table.


    // Construction of the product NFA.
    dfa* POWER;
    CALLOC(POWER, 1); // Allocate memory for the product DFA
    POWER->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    POWER->trans = create_dgraph_noedges(inter_elem, A->trans->size_alpha);


    // Initial state.
    POWER->initial = 0; // The initial state of the product DFA is the first state in the inter_pairs array.

    uint final = UINT_MAX;
    for (uint p = 0; p < inter_elem; p++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            POWER->trans->edges[p][a] = inter_trans[p * A->trans->size_alpha + a];
        }
        if (final == UINT_MAX && final_states) {
            bool is_final = true;
            for (uint i = 0; i < n; i++) {
                if (inter_pairs[p * n + i] != (uint)final_states[i]) { // If the state is final
                    is_final = false; // The state is not final
                    break;
                }
            }
            if (is_final) {
                final = p; // Store the index of the final state
            }
        }
    }

    if (final != UINT_MAX) {
        POWER->nb_finals = 1; // Set the number of final states to 1
        MALLOC(POWER->finals, 1); // Allocate memory for the final states
        POWER->finals[0] = final; // Set the final state
    }
    else {
        POWER->nb_finals = 0; // No final states
        POWER->finals = NULL; // No final states
    }

    delete_hash_table(thehash);


    MALLOC(POWER->state_names, inter_elem);
    for (uint p = 0; p < inter_elem; p++) {
        uint size = n - 1;
        for (uint i = 0; i < n; i++) {
            if (A->state_names) {
                size += strlen(A->state_names[inter_pairs[p * n + i]]); // Calculate the size needed for the state names
            }
            else {
                size += get_uint_length(inter_pairs[p * n + i]); // Calculate the size needed for the state names
            }
        }
        MALLOC(POWER->state_names[p], size);
        POWER->state_names[p][0] = '\0';
        for (uint i = 0; i < n; i++) {
            if (i > 0) {
                strcat(POWER->state_names[p], ","); // Add a comma between state names
            }
            if (A->state_names) {
                strcat(POWER->state_names[p], A->state_names[inter_pairs[p * n + i]]); // Concatenate the state name
            }
            else {
                char num[20];
                sprintf(num, "%d", inter_pairs[p * n + i]); // Convert the state number to string
                strcat(POWER->state_names[p], num); // Concatenate the state number
            }
        }
    }
    inter_delete();
    return POWER;
}


prod_pair* dgraph_intersec(dgraph* g1, dgraph* g2, uint s1, uint s2, uint* psize) {
    if (g1->size_alpha != g2->size_alpha) {
        return NULL;
    }


#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Intersection of two dgraphs.\n");
#endif


    // Calcul du nouvel alphabet
    uint thealph = g1->size_alpha;



    uchar power = get_uint_lbinary(g1->size_graph + g2->size_graph) + 2; // We compute the initial power of two for the size of the hash table.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    inter_init(thesize, g1->size_graph, g2->size_graph, thealph); // Initialize the arrays used in the product construction.

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &inter_hash, &inter_equal);

    // Stack containing the elements to be processed.
    dequeue* thestack = create_dequeue();

    // Create the first element: the pair of initial states.

    inter_pairs[0] = s1; // Store the first state in the pair
    inter_pairs[1] = s2; // Store the second state in the pair
    hash_table_insert(thehash, 0); // Insert this pair in the hash table.
    rigins_dequeue(0, thestack); // Add the pair to the stack.
    inter_elem++; // Increment the number of pairs constructed
    inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Starting states done.\n");
#endif

    // DFS
    while (!isempty_dequeue(thestack)) {
        uint i = rigpull_dequeue(thestack); // Retrieve the pair to be processed.

        uint q1 = inter_pairs[i << 1]; // The first state of the pair.
        uint q2 = inter_pairs[(i << 1) + 1]; // The second state of the pair.

        // Handling standard transitions
        for (uint a = 0; a < thealph; a++) {
            if (g1->edges[q1][a] == UINT_MAX || g2->edges[q2][a] == UINT_MAX) {
                continue; // If there are no transitions for this symbol, skip it
            }
            inter_pairs[inter_elem << 1] = g1->edges[q1][a]; // Read the first state from A1
            inter_pairs[(inter_elem << 1) + 1] = g2->edges[q2][a]; // Read the second state from A2
            uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
            if (j == inter_elem) { // If the pair was not already constructed
                rigins_dequeue(inter_elem, thestack); // We add it to the stack for future processing.
                inter_elem++; // Increment the number of pairs constructed
                inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.
            }
        }

    }
    // DFS completed. We can delete the stack and the hash table.
    delete_dequeue(thestack);

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### DFS Done.\n");
#endif

    prod_pair* result;
    MALLOC(result, inter_elem); // Allocate memory for the result array
    for (uint p = 0; p < inter_elem; p++) {
        result[p].q1 = inter_pairs[p << 1]; // The first state of the pair.
        result[p].q2 = inter_pairs[(p << 1) + 1]; // The second state of the pair.
    }
    delete_hash_table(thehash);

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Fully done.\n");
#endif

    * psize = inter_elem; // Set the size of the result array
    inter_delete(); // Delete the arrays used in the product construction
    return result;





}

bool dgraph_exists_path(dgraph* g, uint s, uint e, bool strict, uint** word) {
    if (!strict && s == e) {
        if (word) {
            MALLOC(*word, 1); // Allocate memory for the path
            (*word)[0] = UINT_MAX; // Set the path to empty
        }
        return true; // If the starting states are the same as the ending states, we found an intersection path
    }


    // The alphabet
    uint thealph = g->size_alpha;

    bool* visited;
    CALLOC(visited, g->size_graph); // Allocate memory for the visited states
    uint* pred_state;
    CALLOC(pred_state, g->size_graph); // Allocate memory for the predecessor states
    uint* pred_symbol;
    CALLOC(pred_symbol, g->size_graph); // Allocate memory for the predecessor symbols


    // Queue containing the elements to be processed.
    dequeue* thequeue = create_dequeue();

    // Create the starting element
    visited[s] = true; // Mark the starting state as visited
    rigins_dequeue(s, thequeue); // Add the pair to the queue.

    // BFS
    while (!isempty_dequeue(thequeue)) {
        uint q = lefpull_dequeue(thequeue); // Retrieve the state to be processed.

        // Handling transitions
        for (uint a = 0; a < thealph; a++) {
            if (g->edges[q][a] == UINT_MAX) {
                continue; // If there are no transitions for this symbol, skip it
            }
            if (g->edges[q][a] == e) {
                delete_dequeue(thequeue); // If we reach the final states, we can stop the search
                if (word) {
                    uint r = q;
                    uint k = 1;
                    while (r != s) {
                        k++; // Count the number of transitions in the path
                        r = pred_state[r]; // Backtrack to find the previous pair
                    }
                    r = q;
                    MALLOC(*word, k + 1); // Allocate memory for the path
                    (*word)[k] = UINT_MAX;
                    k--;
                    (*word)[k] = a; // Store the last transition in the path
                    while (r != s) { // Backtrack to find the path
                        k--; // Decrement the index for the path
                        (*word)[k] = pred_symbol[r]; // Store the transition leading to this pair
                        r = pred_state[r]; // Backtrack to the previous pair
                    }

                }
                free(visited); // Free the memory allocated for the visited states
                free(pred_state); // Free the memory allocated for the predecessor states
                free(pred_symbol); // Free the memory allocated for the predecessor symbols
                return true; // We found an intersection path
            }

            if (!visited[g->edges[q][a]]) { // If the state has not been visited yet
                visited[g->edges[q][a]] = true; // Mark it as visited
                rigins_dequeue(g->edges[q][a], thequeue); // Add the state to the queue for future processing.
                pred_state[g->edges[q][a]] = q; // Store the predecessor state
                pred_symbol[g->edges[q][a]] = a; // Store the symbol leading to this state
            }
        }

    }



    // DFS completed. We can delete the queue and the hash table.
    delete_dequeue(thequeue);
    free(visited); // Free the memory allocated for the visited states
    free(pred_state); // Free the memory allocated for the predecessor states
    free(pred_symbol); // Free the memory allocated for the predecessor symbols

    return false; // No intersection path found



}

bool dgraph_exists_path_alpha(dgraph* g, uint s, uint e, bool strict, bool* alpha, uint** word) {
    if (!strict && s == e) {
        if (word) {
            MALLOC(*word, 1); // Allocate memory for the path
            (*word)[0] = UINT_MAX; // Set the path to empty
        }
        return true; // If the starting states are the same as the ending states, we found an intersection path
    }


    // The alphabet
    uint thealph = g->size_alpha;

    bool* visited;
    CALLOC(visited, g->size_graph); // Allocate memory for the visited states
    uint* pred_state;
    CALLOC(pred_state, g->size_graph); // Allocate memory for the predecessor states
    uint* pred_symbol;
    CALLOC(pred_symbol, g->size_graph); // Allocate memory for the predecessor symbols


    // Queue containing the elements to be processed.
    dequeue* thequeue = create_dequeue();

    // Create the starting element
    visited[s] = true; // Mark the starting state as visited
    rigins_dequeue(s, thequeue); // Add the pair to the queue.

    // BFS
    while (!isempty_dequeue(thequeue)) {
        uint q = lefpull_dequeue(thequeue); // Retrieve the state to be processed.

        // Handling transitions
        for (uint a = 0; a < thealph; a++) {
            if (alpha && !alpha[a]) {
                continue; // If the symbol is not in the alphabet, skip it
            }

            if (g->edges[q][a] == UINT_MAX) {
                continue; // If there are no transitions for this symbol, skip it
            }
            if (g->edges[q][a] == e) {
                delete_dequeue(thequeue); // If we reach the final states, we can stop the search
                if (word) {
                    uint r = q;
                    uint k = 1;
                    while (r != s) {
                        k++; // Count the number of transitions in the path
                        r = pred_state[r]; // Backtrack to find the previous pair
                    }
                    r = q;
                    MALLOC(*word, k + 1); // Allocate memory for the path
                    (*word)[k] = UINT_MAX;
                    k--;
                    (*word)[k] = a; // Store the last transition in the path
                    while (r != s) { // Backtrack to find the path
                        k--; // Decrement the index for the path
                        (*word)[k] = pred_symbol[r]; // Store the transition leading to this pair
                        r = pred_state[r]; // Backtrack to the previous pair
                    }

                }
                free(visited); // Free the memory allocated for the visited states
                free(pred_state); // Free the memory allocated for the predecessor states
                free(pred_symbol); // Free the memory allocated for the predecessor symbols
                return true; // We found an intersection path
            }

            if (!visited[g->edges[q][a]]) { // If the state has not been visited yet
                visited[g->edges[q][a]] = true; // Mark it as visited
                rigins_dequeue(g->edges[q][a], thequeue); // Add the state to the queue for future processing.
                pred_state[g->edges[q][a]] = q; // Store the predecessor state
                pred_symbol[g->edges[q][a]] = a; // Store the symbol leading to this state
            }
        }

    }



    // DFS completed. We can delete the queue and the hash table.
    delete_dequeue(thequeue);
    free(visited); // Free the memory allocated for the visited states
    free(pred_state); // Free the memory allocated for the predecessor states
    free(pred_symbol); // Free the memory allocated for the predecessor symbols


    return false; // No intersection path found



}



uint dgraph_exists_path_letter_alpha(dgraph* g, uint s, uint b, bool* alpha, uint** word) {


    if (g->edges[s][b] != UINT_MAX) {
        if (word) {
            MALLOC(*word, 1); // Allocate memory for the path
            (*word)[0] = UINT_MAX; // Set the path to empty
        }
        return s; // If the starting states are the same as the ending states, we found an intersection path
    }


    // The alphabet
    uint thealph = g->size_alpha;

    bool* visited;
    CALLOC(visited, g->size_graph); // Allocate memory for the visited states
    uint* pred_state;
    CALLOC(pred_state, g->size_graph); // Allocate memory for the predecessor states
    uint* pred_symbol;
    CALLOC(pred_symbol, g->size_graph); // Allocate memory for the predecessor symbols


    // Queue containing the elements to be processed.
    dequeue* thequeue = create_dequeue();

    // Create the starting element
    visited[s] = true; // Mark the starting state as visited
    rigins_dequeue(s, thequeue); // Add the pair to the queue.

    // BFS
    while (!isempty_dequeue(thequeue)) {
        uint q = lefpull_dequeue(thequeue); // Retrieve the state to be processed.

        // Handling transitions
        for (uint a = 0; a < thealph; a++) {
            if (alpha && !alpha[a]) {
                continue; // If the symbol is not in the alphabet, skip it
            }

            if (g->edges[q][a] == UINT_MAX) {
                continue; // If there are no transitions for this symbol, skip it
            }
            if (g->edges[g->edges[q][a]][b] != UINT_MAX) {
                delete_dequeue(thequeue); // If we reach the final states, we can stop the search
                if (word) {
                    uint r = q;
                    uint k = 1;
                    while (r != s) {
                        k++; // Count the number of transitions in the path
                        r = pred_state[r]; // Backtrack to find the previous pair
                    }
                    r = q;
                    MALLOC(*word, k + 1); // Allocate memory for the path
                    (*word)[k] = UINT_MAX;
                    k--;
                    (*word)[k] = a; // Store the last transition in the path
                    while (r != s) { // Backtrack to find the path
                        k--; // Decrement the index for the path
                        (*word)[k] = pred_symbol[r]; // Store the transition leading to this pair
                        r = pred_state[r]; // Backtrack to the previous pair
                    }

                }
                free(visited); // Free the memory allocated for the visited states
                free(pred_state); // Free the memory allocated for the predecessor states
                free(pred_symbol); // Free the memory allocated for the predecessor symbols
                return g->edges[q][a]; // We found a path ending at this state.
            }

            if (!visited[g->edges[q][a]]) { // If the state has not been visited yet
                visited[g->edges[q][a]] = true; // Mark it as visited
                rigins_dequeue(g->edges[q][a], thequeue); // Add the state to the queue for future processing.
                pred_state[g->edges[q][a]] = q; // Store the predecessor state
                pred_symbol[g->edges[q][a]] = a; // Store the symbol leading to this state
            }
        }

    }



    // DFS completed. We can delete the queue and the hash table.
    delete_dequeue(thequeue);
    free(visited); // Free the memory allocated for the visited states
    free(pred_state); // Free the memory allocated for the predecessor states
    free(pred_symbol); // Free the memory allocated for the predecessor symbols


    return UINT_MAX; // No intersection path found



}

bool dgraph_exists_intersec_path(dgraph* g1, dgraph* g2, uint s1, uint s2, uint e1, uint e2, bool strict, uint** word) {
    // fprintf(stderr, "#### Checking for intersection path between two dgraphs.\n");
    if (g1->size_alpha != g2->size_alpha) {
        return false; // If the alphabets are not the same, we cannot find an intersection path
    }

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Checking intersection of paths in two dgraphs.\n");
#endif

    if (!strict && s1 == e1 && s2 == e2) {
        if (word) {
            MALLOC(*word, 1); // Allocate memory for the path
            (*word)[0] = UINT_MAX; // Set the path to empty
        }
        return true; // If the starting states are the same as the ending states, we found an intersection path
    }


    // The alphabet
    uint thealph = g1->size_alpha;



    uchar power = get_uint_lbinary(g1->size_graph + g2->size_graph) + 2; // We compute the initial power of two for the size of the hash table.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    inter_init_weak(thesize, g1->size_graph, g2->size_graph, thealph); // Initialize the arrays used in the product construction.

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &inter_hash, &inter_equal);

    // Queue containing the elements to be processed.
    dequeue* thequeue = create_dequeue();

    // Create the first element: the pair of initial states.

    inter_pairs[0] = s1; // Store the first state in the pair
    inter_pairs[1] = s2; // Store the second state in the pair
    hash_table_insert(thehash, 0); // Insert this pair in the hash table.
    rigins_dequeue(0, thequeue); // Add the pair to the queue.
    inter_elem++; // Increment the number of pairs constructed
    inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Starting states done.\n");
#endif

    // BFS
    while (!isempty_dequeue(thequeue)) {
        uint i = lefpull_dequeue(thequeue); // Retrieve the pair to be processed.

        uint q1 = inter_pairs[i << 1]; // The first state of the pair.
        uint q2 = inter_pairs[(i << 1) + 1]; // The second state of the pair.

        // Handling standard transitions
        for (uint a = 0; a < thealph; a++) {
            if (g1->edges[q1][a] == UINT_MAX || g2->edges[q2][a] == UINT_MAX) {
                continue; // If there are no transitions for this symbol, skip it
            }
            if (g1->edges[q1][a] == e1 && g2->edges[q2][a] == e2) {
                delete_dequeue(thequeue); // If we reach the final states, we can stop the search
                delete_hash_table(thehash); // Delete the hash table
                if (word) {
                    uint j = i;
                    uint k = 1;
                    while (j != 0) {
                        k++; // Count the number of transitions in the path
                        j = inter_back[j << 1]; // Backtrack to find the previous pair
                    }
                    j = i;
                    MALLOC(*word, k + 1); // Allocate memory for the path
                    (*word)[k] = UINT_MAX;
                    k--;
                    (*word)[k] = a; // Store the last transition in the path
                    while (j != 0) { // Backtrack to find the path
                        k--; // Decrement the index for the path
                        (*word)[k] = inter_back[(j << 1) + 1]; // Store the transition leading to this pair
                        j = inter_back[j << 1]; // Backtrack to the previous pair
                    }

                }
                inter_delete(); // Delete the arrays used in the product construction
                return true; // We found an intersection path
            }


            inter_pairs[inter_elem << 1] = g1->edges[q1][a]; // Read the first state from A1
            inter_pairs[(inter_elem << 1) + 1] = g2->edges[q2][a]; // Read the second state from A2
            uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
            if (j == inter_elem) { // If the pair was not already constructed
                inter_back[inter_elem << 1] = i;
                inter_back[(inter_elem << 1) + 1] = a; // Store the transition leading to this pair
                rigins_dequeue(inter_elem, thequeue); // We add it to the stack for future processing.
                inter_elem++; // Increment the number of pairs constructed
                inter_grow_weak(); // If the number of pairs constructed is larger than the size of the table, we double the size.
            }
        }

    }


#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### DFS Done.\n");
#endif

    // DFS completed. We can delete the queue and the hash table.
    delete_dequeue(thequeue);
    delete_hash_table(thehash);

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Fully done.\n");
#endif

    inter_delete();

    return false; // No intersection path found
}



bool dgraph_exists_intersec_path_alpha(dgraph* g1, dgraph* g2, uint s1, uint s2, uint e1, uint e2, bool strict, bool* alpha, uint** word) {
    // fprintf(stderr, "#### Checking for intersection path between two dgraphs.\n");
    if (g1->size_alpha != g2->size_alpha) {
        return false; // If the alphabets are not the same, we cannot find an intersection path
    }

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Checking intersection of paths in two dgraphs.\n");
#endif

    if (!strict && s1 == e1 && s2 == e2) {
        if (word) {
            MALLOC(*word, 1); // Allocate memory for the path
            (*word)[0] = UINT_MAX; // Set the path to empty
        }
        return true; // If the starting states are the same as the ending states, we found an intersection path
    }


    // The alphabet
    uint thealph = g1->size_alpha;



    uchar power = get_uint_lbinary(g1->size_graph + g2->size_graph) + 2; // We compute the initial power of two for the size of the hash table.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    inter_init_weak(thesize, g1->size_graph, g2->size_graph, thealph); // Initialize the arrays used in the product construction.

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &inter_hash, &inter_equal);

    // Queue containing the elements to be processed.
    dequeue* thequeue = create_dequeue();

    // Create the first element: the pair of initial states.

    inter_pairs[0] = s1; // Store the first state in the pair
    inter_pairs[1] = s2; // Store the second state in the pair
    hash_table_insert(thehash, 0); // Insert this pair in the hash table.
    rigins_dequeue(0, thequeue); // Add the pair to the queue.
    inter_elem++; // Increment the number of pairs constructed
    inter_grow(); // If the number of pairs constructed is larger than the size of the table, we double the size.

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Starting states done.\n");
#endif

    // BFS
    while (!isempty_dequeue(thequeue)) {
        uint i = lefpull_dequeue(thequeue); // Retrieve the pair to be processed.

        uint q1 = inter_pairs[i << 1]; // The first state of the pair.
        uint q2 = inter_pairs[(i << 1) + 1]; // The second state of the pair.

        // Handling standard transitions
        for (uint a = 0; a < thealph; a++) {
            if (alpha && !alpha[a]) {
                continue; // If the symbol is not in the alphabet, skip it
            }

            if (g1->edges[q1][a] == UINT_MAX || g2->edges[q2][a] == UINT_MAX) {
                continue; // If there are no transitions for this symbol, skip it
            }
            if (g1->edges[q1][a] == e1 && g2->edges[q2][a] == e2) {
                delete_dequeue(thequeue); // If we reach the final states, we can stop the search
                delete_hash_table(thehash); // Delete the hash table
                if (word) {
                    uint j = i;
                    uint k = 1;
                    while (j != 0) {
                        k++; // Count the number of transitions in the path
                        j = inter_back[j << 1]; // Backtrack to find the previous pair
                    }
                    j = i;
                    MALLOC(*word, k + 1); // Allocate memory for the path
                    (*word)[k] = UINT_MAX;
                    k--;
                    (*word)[k] = a; // Store the last transition in the path
                    while (j != 0) { // Backtrack to find the path
                        k--; // Decrement the index for the path
                        (*word)[k] = inter_back[(j << 1) + 1]; // Store the transition leading to this pair
                        j = inter_back[j << 1]; // Backtrack to the previous pair
                    }

                }
                inter_delete(); // Delete the arrays used in the product construction
                return true; // We found an intersection path
            }


            inter_pairs[inter_elem << 1] = g1->edges[q1][a]; // Read the first state from A1
            inter_pairs[(inter_elem << 1) + 1] = g2->edges[q2][a]; // Read the second state from A2
            uint j = hash_table_insert(thehash, inter_elem); // Try to insert the new pair in the hash table.
            if (j == inter_elem) { // If the pair was not already constructed
                inter_back[inter_elem << 1] = i;
                inter_back[(inter_elem << 1) + 1] = a; // Store the transition leading to this pair
                rigins_dequeue(inter_elem, thequeue); // We add it to the stack for future processing.
                inter_elem++; // Increment the number of pairs constructed
                inter_grow_weak(); // If the number of pairs constructed is larger than the size of the table, we double the size.
            }
        }

    }


#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### DFS Done.\n");
#endif

    // DFS completed. We can delete the queue and the hash table.
    delete_dequeue(thequeue);
    delete_hash_table(thehash);

#ifdef NFA_INTER_DEBUG
    fprintf(stderr, "#### Fully done.\n");
#endif

    inter_delete();

    return false; // No intersection path found
}
