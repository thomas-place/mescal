#include "nfa_determi.h"



// Temporary representation of a state of the deterministic automaton
// during the depth-first search. We also store the adjacent states
// found for the construction of the DFA afterwards.
typedef struct detstate {
    dequeue* set;           //!< The state of the DFA: a sorted set of states of the original NFA.
    uint size_alpha;        //!< The size of the alphabet.
    uint num;               //!< The number of the state in the future DFA.
    struct detstate** next; //!< An array of size size_alph containing the transitions from the state for each letter.
} detstate;


// Release functions for a detstate. 
static void free_detstate(void* p) {
    if (p == NULL) {
        return;
    }
    delete_dequeue(((detstate*)p)->set);
    free(((detstate*)p)->next);

    free(p);
}

// For the depth-first search, we store the states encountered
// in an AVL. We therefore need a comparison function.
// Return, O if =, 1 if <, -1 if >.
static int comdequeuedetstate(void* e1, void* e2) {
    dequeue* s1 = ((detstate*)e1)->set;
    dequeue* s2 = ((detstate*)e2)->set;

    // First we compare the size.
    if (size_dequeue(s1) < size_dequeue(s2)) {
        return 1;
    }
    if (size_dequeue(s1) > size_dequeue(s2)) {
        return -1;
    }
    // If the size is the same, we compare the elements in lexicogrpahic order.
    for (uint i = 0; i < size_dequeue(s1); i++) {
        if (lefread_dequeue(s1, i) < lefread_dequeue(s2, i)) {
            return 1;
        }
        if (lefread_dequeue(s1, i) > lefread_dequeue(s2, i)) {
            return 1;
        }
    }
    return 0;
}


// Function that fills the transition table of the final DFA from the AVL
// built during the depth-first search.
// The function also calculates the new final states: oldfinals is the list
// of final states of the original NFA and boolfinals is an array of Booleans
// in which we record the new final states.
// The names Boolean indicates whether the names of the new states (sets of states
// of the old NFA) should be saved.
static void dfa_avl_to_table(avlnode* AVL, nfa* DFA, dequeue* oldfinals, bool* newfinals,
    bool names, char** oldnames) {
    if (AVL == NULL) {
        return;
    }
    else {
        dfa_avl_to_table(AVL->left, DFA, oldfinals, newfinals, names, oldnames);
        dfa_avl_to_table(AVL->right, DFA, oldfinals, newfinals, names, oldnames);
        detstate* thestate = (detstate*)AVL->value;

        for (uint a = 0; a < DFA->trans->size_alpha; a++) {
            lefins_dequeue(thestate->next[a]->num, DFA->trans->edges[thestate->num][a]);
        }

        if (intersec_dequeue(oldfinals, thestate->set)) {
            newfinals[thestate->num] = true;
        }
        else {
            newfinals[thestate->num] = false;
        }

        if (names) {
            if (isempty_dequeue(thestate->set)) {
                MALLOC(DFA->state_names[thestate->num], 6);
                sprintf(DFA->state_names[thestate->num], "∅");
            }
            else {
                uint string_size = 2 + size_dequeue(thestate->set);
                for (uint i = 0; i < size_dequeue(thestate->set); i++) {
                    if (oldnames) {
                        string_size += strlen(oldnames[lefread_dequeue(thestate->set, i)]);
                    }
                    else {
                        string_size += get_uint_length(lefread_dequeue(thestate->set, i));
                    }
                }

                MALLOC(DFA->state_names[thestate->num], string_size);
                sprintf(DFA->state_names[thestate->num], "{");
                char aux[10];
                for (uint i = 0; i < size_dequeue(thestate->set) - 1; i++) {
                    if (oldnames) {
                        strcat(DFA->state_names[thestate->num],
                            oldnames[lefread_dequeue(thestate->set, i)]);
                        strcat(DFA->state_names[thestate->num], ",");
                    }
                    else {
                        sprintf(aux, "%d,", lefread_dequeue(thestate->set, i));
                        strcat(DFA->state_names[thestate->num], aux);
                    }
                }
                if (oldnames) {
                    strcat(DFA->state_names[thestate->num],
                        oldnames[rigread_dequeue(thestate->set, 0)]);
                    strcat(DFA->state_names[thestate->num], "}");
                }
                else {
                    sprintf(aux, "%d}", rigread_dequeue(thestate->set, 0));
                    strcat(DFA->state_names[thestate->num], aux);
                }
            }
        }
    }
}

// Determinization procedure. The Boolean names indicates whether the names of the new states
// (sets of states of the old NFA) should be saved in the DFA.
nfa* nfa_determinize(nfa* A, bool names) {

    // First we eliminate the (possible) epsilon transitions
    A = nfa_elimeps(A);


    // We create the stack used for the depth-first search.
    dequeue_gen* thestack = create_dequeue_gen();

    // We use the AVL to store the states of the future DFA already built during
    // the depth-first search.
    avlnode* tree = NULL;

    // Counter that will be used to count and number the states created.
    int num = 0;

    // Construction of the initial state of the future DFA.
    detstate* start;
    MALLOC(start, 1);
    start->set = create_dequeue();
    copy_dequeue_right(start->set, A->initials, 0);    // The initial state of the DFA is the set of initial states of the NFA
    start->size_alpha = A->trans->size_alpha;
    start->num = num;                     // The initial state is the state 0
    num++;
    start->next = NULL;     // The transitions are not yet known.
    tree = avl_insert(start, tree, &comdequeuedetstate, NULL); // We store the initial state in the AVL.
    rigins_dequeue_gen(start, thestack);                    // We put the initial state in the stack to explore it.

    while (!isempty_dequeue_gen(thestack)) {

        // We retrieve the state to be processed.
        detstate* thestate = (detstate*)rigpull_dequeue_gen(thestack);

        // We calculate the transitions from thestate.
        MALLOC(thestate->next, A->trans->size_alpha);
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            // For each letter a
            // We create a (potential) new state to be processed for the transition labeled by a.
            // We only assign the corresponding set of states in the original NFA.
            // In fact, this information is sufficient to compare this state with those in the AVL.
            detstate* new;
            MALLOC(new, 1);
            new->set = lgraph_reachable(A->trans, thestate->set, a);

            // We check if this state has already been built in the AVL.
            avlnode* thenode = avl_search(new, tree, &comdequeuedetstate);
            if (thenode == NULL) {
                // If this state is new, we finish its construction.
                new->size_alpha = A->trans->size_alpha;
                new->num = num;
                num++;
                new->next = NULL;

                // This new state is adjacent to thestate for the letter a.
                thestate->next[a] = new;

                // We store this new state in the AVL and in the stack to explore it later.
                tree = avl_insert(new, tree, &comdequeuedetstate, NULL);
                rigins_dequeue_gen(new, thestack);
            }
            else {
                // If the state is not new, we assign the version already built as adjacent for the letter a.
                thestate->next[a] = ((detstate*)thenode->value);

                // We delete the copy we just created.
                delete_dequeue(new->set);
                free(new);
            }
        }
    }
    // We have finished the depth-first search. We can delete the stack.
    delete_dequeue_gen(thestack);

    // We have built the AVL. We can now build the DFA.
    nfa* DFA = nfa_init();



    DFA->alphabet = nfa_duplicate_alpha(A);
    lefins_dequeue(0, DFA->initials);
    DFA->trans = create_lgraph_noedges(num, A->trans->size_alpha);
    if (names) {
        MALLOC(DFA->state_names, DFA->trans->size_graph);
    }
    else {
        DFA->state_names = NULL;
    }




    bool tempfinals[num];                                                      // Temporary array to store the final states
    dfa_avl_to_table(tree, DFA, A->finals, tempfinals, names, A->state_names); // We fill the transition table.

    for (int i = 0; i < num; i++) {                                            // We fill the list of final states.
        if (tempfinals[i]) {
            rigins_dequeue(i, DFA->finals);
        }
    }

    delete_nfa(A);
    avl_free_strong(tree, &free_detstate);

    return DFA;
}

// Complementation
nfa* nfa_complement(nfa* A) {
    nfa* B = nfa_determinize(A, false);
    dequeue* newfinals = create_dequeue();
    for (uint q = 0; q < B->trans->size_graph; q++) {
        if (isempty_dequeue(B->finals) || q < lefread_dequeue(B->finals, 0)) {
            rigins_dequeue(q, newfinals);
        }
        else {
            lefpull_dequeue(B->finals);
        }
    }
    delete_dequeue(B->finals);
    B->finals = newfinals;
    return B;
}
