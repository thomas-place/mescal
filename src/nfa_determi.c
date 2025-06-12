#include "nfa_determi.h"
#include "type_hash.h"


// Storage of the subsets for the subset construction
static ulong det_cons_size = 0; // Full size of the det_sets array.
static ulong det_cons_elem = 0; // Number of subsets already stored in the det_sets array.
static uint det_cons_states = 0; // Number of states in the original nfa.
static uint det_cons_letters = 0; // Number of letters in the original nfa (size of the alphabet).
static bool* det_cons_sets = NULL; // Array that stores the sets (a single set uses det_cons_states cells). Size: det_cons_size * det_cons_states.
static uint* det_cons_next = NULL; //!< Array that stores the computed transitions (a single transition uses det_cons_letters cells). Size: det_cons_size * det_cons_letters.

// Initialize the arrays used in the subset construction.
static void det_cons_init(uint size, uint states, uint letters) {
    size = max(size, 2); // Ensures that the size is at least 2.
    det_cons_size = size;
    det_cons_elem = 0;
    det_cons_states = states;
    det_cons_letters = letters;
    MALLOC(det_cons_sets, det_cons_size * det_cons_states);
    MALLOC(det_cons_next, det_cons_size * det_cons_letters);
}

// Deletes the arrays used in the subset construction and resets the variables.
static void det_cons_delete() {
    free(det_cons_sets);
    det_cons_sets = NULL;
    free(det_cons_next);
    det_cons_next = NULL;
    det_cons_size = 0;
    det_cons_elem = 0;
    det_cons_states = 0;
    det_cons_letters = 0;
}

// Doubles the size of the arrays used in the subset construction if necessary.
static void det_cons_grow() {
    if (det_cons_size <= det_cons_elem) {
        det_cons_size <<= 1;
        REALLOC(det_cons_sets, det_cons_size * det_cons_states);
        REALLOC(det_cons_next, det_cons_size * det_cons_letters);
    }
}

// Checks if two sets stored in the det_cons_sets array are equal.
static bool det_cons_equal(uint i, uint j) {
    ulong zi = i * det_cons_states; // The index of the first element.
    ulong zj = j * det_cons_states; // The index of the second element.
    for (uint h = 0; h < det_cons_states; h++) {
        if (det_cons_sets[zi + h] != det_cons_sets[zj + h]) {
            return false; // If any element is different, the two sets are not equal.
        }
    }
    return true;
}

// Hash function for the sets stored in the det_cons_sets array.
static uint det_cons_hash(uint i, uint size_hash) {
    ulong e = i * det_cons_states; // The index of the element in the mor_perms array.
    uint hash = 0;

    uint a = 0x9e3779b9; // fractional bits of the golden ratio

    //uint count = 0;

    for (uint j = 0; j < det_cons_states; j++) {
        if (!det_cons_sets[e + j]) {
            hash = (hash * 3 + 1 * a) % size_hash;
            //continue; // If the state is not in the set, we skip it.
        }
        else
        {
            hash = (hash * 3 + 2 * a) % size_hash;
        }

        //hash = (hash * (det_cons_states + 1) + j * a) % size_hash;
        //count++;
    }
    //hash = (hash * (det_cons_states + 1) + count * a) % size_hash;
    return hash;
}

// Construction of the state names from the sets stored in the det_cons_sets array.
static char** det_cons_names(char** oldnames) {
    if (det_cons_elem == 0) {
        return NULL; // If no states have been constructed, we return NULL.
    }
    char** names;
    MALLOC(names, det_cons_elem);

    for (uint q = 0; q < det_cons_elem; q++) {
        uint string_size = 2; // We start with 2 for the brackets.
        for (uint i = 0; i < det_cons_states; i++) {
            if (det_cons_sets[q * det_cons_states + i]) {
                if (oldnames) {
                    string_size += strlen(oldnames[i]) + 1;
                }
                else {
                    string_size += get_uint_length(i) + 1;
                }
            }
        }

        if (string_size < 3) {
            names[q] = strdup("∅");
            continue;
        }

        MALLOC(names[q], string_size);
        sprintf(names[q], "{");
        char aux[10];
        bool first = true;
        for (uint i = 0; i < det_cons_states; i++) {
            if (det_cons_sets[q * det_cons_states + i]) {
                if (first) {
                    first = false;
                }
                else {
                    strcat(names[q], ",");
                }
                if (oldnames) {
                    strcat(names[q], oldnames[i]);
                }
                else {
                    sprintf(aux, "%d", i);
                    strcat(names[q], aux);
                }
            }
        }
        strcat(names[q], "}");
    }

    return names;
}


dfa* nfa_determinize(nfa* A, bool names) {

    if (!A) {
        return NULL;
    }

    // First we eliminate the (possible) epsilon transitions
    A = nfa_elimeps(A);

    uchar power = get_uint_lbinary(A->trans->size_graph) + 2; // We compute the initial power of two for the size of the hash table.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    det_cons_init(thesize, A->trans->size_graph, A->trans->size_alpha);

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &det_cons_hash, &det_cons_equal);

    // Stack containing the elements to be processed.
    dequeue* thestack = create_dequeue();

    // Create the first element: the set of initial states of the NFA.
    uint ini = 0;
    for (uint i = 0; i < A->trans->size_graph; i++) {
        if (ini < size_dequeue(A->initials) && lefread_dequeue(A->initials, ini) == i) {
            // If the state i is an initial state of the NFA, we add it to the set.
            det_cons_sets[i] = true;
            ini++;
        }
        else {
            // If the state i is not an initial state of the NFA, we do not add it to the set.
            det_cons_sets[i] = false;
        }
    }
    det_cons_elem++;
    hash_table_insert(thehash, 0); // Insert this set in the hash table.
    rigins_dequeue(0, thestack); // Add the identity to the stack.

    while (!isempty_dequeue(thestack)) {
        // We retrieve the state to be processed.
        uint s = rigpull_dequeue(thestack);

        // We calculate the transitions from this state.
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            ulong i = det_cons_elem * det_cons_states; // The number of the new element is the next one.
            ulong j = s * det_cons_states;
            for (uint q = 0; q < A->trans->size_graph; q++) {
                det_cons_sets[i + q] = false; // Initialize the new set to false.
            }
            for (uint q = 0; q < A->trans->size_graph; q++) {
                if (!det_cons_sets[j + q]) {
                    continue; // If the state q is not in the set, we skip it.
                }
                for (uint h = 0; h < size_dequeue(A->trans->edges[q][a]); h++) {
                    uint r = lefread_dequeue(A->trans->edges[q][a], h); // For each state q in the set of states of the current state s,
                    det_cons_sets[i + r] = true; // we add the states reachable from q by the letter a to the new set.
                }
            }

            uint h = hash_table_insert(thehash, det_cons_elem); // Try to insert the new state in the hash table.

            if (h == det_cons_elem) {
                // The state was not already constructed.
                rigins_dequeue(det_cons_elem, thestack); // We add it to the stack for future processing.


                // Prepare the next state in the table.
                det_cons_elem++; // Increment the number of states constructed.
                det_cons_grow(); // If the number of states constructed is larger than the size of the table, we double the size.
            }

            // Assign the next element of s for a.
            ulong sa = s * det_cons_letters + a;
            det_cons_next[sa] = h;
        }
    }
    // We have finished the depth-first search. We can delete the stack and the hash table.
    delete_hash_table(thehash); // Delete the hash table.
    delete_dequeue(thestack);

    // We can now build the DFA.
    dfa* DFA;
    CALLOC(DFA, 1);
    DFA->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha); // Copy letter names
    DFA->trans = create_dgraph_noedges(det_cons_elem, A->trans->size_alpha); // Create the graph.

    DFA->initial = 0; // The initial state of the DFA is the first state constructed.

    // Computing the final states.
    bool* tempfinals;
    CALLOC(tempfinals, det_cons_elem);
    DFA->nb_finals = 0;
    for (uint i = 0; i < det_cons_elem; i++) {
        // For each state in the DFA, we check if it contains a final state of the NFA.
        for (uint j = 0; j < size_dequeue(A->finals); j++) {
            if (det_cons_sets[i * det_cons_states + lefread_dequeue(A->finals, j)]) {
                tempfinals[i] = true; // If it contains a final state, we mark it as a final state.
                DFA->nb_finals++;
                break;
            }
        }
    }

    // Assigning the finals states.
    uint h = 0;
    MALLOC(DFA->finals, DFA->nb_finals); // Allocate the finals array.
    for (uint i = 0; i < det_cons_elem; i++) {
        if (tempfinals[i]) {
            DFA->finals[h] = i; // If the state is a final state, we add it to the list of finals states of the DFA.
            h++;
        }
    }
    free(tempfinals); // We can delete the temporary array used to store the final states.

    // Computing the transitions of the DFA.
    DFA->trans = create_dgraph_noedges(det_cons_elem, A->trans->size_alpha);
    for (uint i = 0; i < det_cons_elem; i++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            DFA->trans->edges[i][a] = det_cons_next[i * det_cons_letters + a]; // The next state for the letter a.
        }
    }

    // Computation of the state names    
    if (names) {
        DFA->state_names = det_cons_names(A->state_names); // We compute the names of the states of the DFA.
    }
    else {
        DFA->state_names = NULL;
    }


    // We can delete the arrays used in the subset construction.
    det_cons_delete();
    nfa_delete(A);
    return DFA;
}











// Complementation
nfa* nfa_complement(nfa* A) {
    dfa* B = nfa_determinize(A, true);

    uint sizef = B->trans->size_graph - B->nb_finals;
    uint* finals;
    MALLOC(finals, sizef);

    uint i = 0;
    uint j = 0;
    for (uint q = 0; q < B->trans->size_graph; q++) {
        if (B->finals[i] == q) {
            // If the state q is a final state in the original DFA, we skip it.
            i++;
        }
        else {
            // If the state q is not a final state in the original DFA, we add it to the list of final states in the new DFA.
            finals[j] = q;
            j++;
        }
    }

    free(B->finals); // Free the old finals array.
    B->finals = finals; // Assign the new finals array.
    B->nb_finals = sizef; // Update the number of final states.

    nfa* C = dfa_to_nfa(B); // Convert the DFA to an NFA.
    dfa_delete(B); // Delete the DFA.

    return C;
}



// Complementation
dfa* dfa_complement(dfa* A) {
    dfa* B;
    CALLOC(B, 1);
    B->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha); // Copy letter names
    B->initial = A->initial; // The initial state is the same as in the original DFA.
    B->trans = copy_dgraph(A->trans); // Copy the transitions from the original DFA.

    B->nb_finals = A->trans->size_graph - A->nb_finals; // The number of final states is the total number of states minus the number of final states in the original DFA.
    MALLOC(B->finals, B->nb_finals); // Allocate the finals array.
    uint ja = 0;
    uint jb = 0;
    for (uint q = 0; q < A->trans->size_graph; q++) {
        if (q == A->finals[ja]) {
            // If the state q is a final state in the original DFA, we skip it.
            ja++;
        }
        else {
            // If the state q is not a final state in the original DFA, we add it to the list of final states in the new DFA.
            B->finals[jb] = q;
            jb++;
        }
    }

    // Noms des états
    B->state_names = copy_all_names(A->state_names, A->trans->size_graph);

    return B;
}
