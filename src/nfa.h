/**
 * @file nfa.h
 * @brief
 * Implementation of NFAs.
 */

#ifndef NFA_H_
#define NFA_H_

 /*  _   _ _____ _     */
 /* | \ | |  ___/ \    */
 /* |  \| | |_ / _ \   */
 /* | |\  |  _/ ___ \  */
 /* |_| \_|_|/_/   \_\ */

#include "graphs.h"
#include "graphs_transclos.h"
#include "type_abr.h"
#include "type_basic.h"
#include "type_binheap.h"
#include "type_boolarray.h"
#include "type_dequeue.h"
#include "words.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief
 * Type used to represent a NFA.
 *
 * @details
 * The last four fields are not mandatory (if not used, they must be set to a
 * NULL pointer). The letters in the alphabet are sorted in increasing order.
 *
 * @remark
 * The array of state names is not mandatory. If it is set to a NULL pointer,
 * each set will be named by its index when displaying the NFA.
 */
typedef struct {
    /* Mandatory */
    lgraph* trans; //!< Graph of transitions (also stores the number of states and the size of the alphabet).
    dequeue* initials; //!< The list of initial states (sorted in increasing order).
    dequeue* finals; //!< The list of final states (sorted in increasing order).
    letter* alphabet; //!< Array indexed by the indices of letters. Maps each index to its actual letter (NULL if the alphabet is empty).

    /* Optional */
    graph* trans_e; //!< Graph of espilon transitions (NULL if there are no such transitions).
    lgraph* trans_i;    //!< Graph of inverse transitions (NULL if there are no such transitions).
    char** state_names; //!< Array of state names (only utilized when displaying the NFA). Each state is mapped to its name (NULL if unused).
    uint* ancestors; //!< Array of ancestors. Useful when the NFA has been built from another NFA by merging and/or removing states. Maps each state to an ancestor state in the original NFA (NULL if unused).
} nfa;


/**
 * @brief
 * Type used to represent a complete DFA.
 */
typedef struct {
    /* Mandatory */
    dgraph* trans; //!< Graph of transitions (also stores the number of states and the size of the alphabet). Undefined transitions are mapped to UINT_MAX.
    uint initial; //!< The index of the initial state.
    uint nb_finals; //!< Number of final states.
    uint* finals; //!< Array of final states (sorted in increasing order).
    letter* alphabet; //!< Array indexed by the indices of letters. Maps each index to its actual letter (NULL if the alphabet is empty).

    /* Optional */
    char** state_names; //!< Array of state names (only utilized when displaying the DFA). Each state is mapped to its name (NULL if unused).
    uint* ancestors; //!< Array of ancestors. Useful when the DFA has been built from another NFA by merging and/or removing states. (NULL if unused).
} dfa;


/*************/
/* Alphabets */
/*************/

/**
 * @brief
 * Displays a letter in the alphabet of a NFA on a given stream: utf8 version.
 */
void nfa_fprint_letter_utf8(const nfa*, //!< The NFA.
    uint,        //!< Index of the letter.
    FILE*       //!< The stream.
);

/**
 * @brief
 * Displays a letter in the alphabet of a NFA on a given stream: graphviz
 * version.
 *
 * @remark
 * Includes an option for displaying an inverse power "-1" on the letter.
 * This is used for displaying inverse transitions.
 */
void nfa_fprint_letter_gviz(
    const nfa*, //!< The NFA.
    uint, //!< Index of the letter.
    FILE*, //!< The stream.
    bool //!< True if an inverse power has to be displayed, false otherwise.
);

/**
 * @brief
 * Copies the alphabet array of a NFA.
 *
 * @remark
 * Auxiliary function utilized when copying a NFA.
 *
 * @return
 * A copy of the alphabet array.
 */
letter* nfa_duplicate_alpha(const nfa* //!< The NFA.
);

/**
 * @brief
 * Retrieves the index of a letter in the alphabet of a NFA.
 *
 * @remark
 * If the letter does not belong to the alphabet the invalid index
 * A->trans->size_alpha is returned
 *
 * @return
 * The index of the letter.
 */
uint nfa_letter_index(const nfa*, //!< The NFA.
    letter       //!< The letter.
);

/***************/
/* State names */
/***************/

/**
 * @brief
 * Displays the name of a state in a NFA on a given stream.
 *
 * @remark
 * If no names are defined for the states, each state is named by its index.
 */
void nfa_print_state(const nfa*, //!< The NFA.
    uint,        //!< Index of the state.
    FILE*       //!< The stream.
);

/**
 * @brief
 * Release the state names in a NFA (if there are states names).
 */
void nfa_reset_state_names(nfa* // The NFA.
);


/**
 * @brief
 * Returns a copy of the array of state names.
 *
 *
 * @return
 * A copy of the array of state names.
 */
char** copy_all_names(char** names, //!< The array of state names. 
    uint size //!< Size of the array of state names.
);

/*************/
/* Ancestors */
/*************/

/**
 * @brief
 * Returns a copy of the array of ancstors in a NFA.
 *
 * @remark
 * If no ancestors are defined for the states, the function returns a NULL
 * pointer.
 *
 * @return
 * A copy of the array of ancestors.
 */
uint* nfa_copy_ancestors(const nfa* //!< The NFA.
);

/******************************/
/* Initialization and release */
/******************************/

/**
 * @brief
 * Initialization of an NFA.
 *
 * @attention
 * Transitions are not initialized (this would require the number of states
 * and the size of the alphabets). On the other hand, the dequeues containing
 * the intial and final states are initialized.
 *
 * @return
 * Le NFA.
 */
nfa* nfa_init(void);

/**
 * @brief
 * Computes a NFA recognizing the empty language.
 *
 * @remark
 * The computed NFA is defined over an empty alphabet.
 *
 * @return
 * The NFA.
 */
nfa* create_emptylang(void);

/**
 * @brief
 * Computes a NFA recognizing the singleton language {ε}.
 *
 * @remark
 * The computed NFA is defined over an empty alphabet.
 *
 * @return
 * The NFA.
 */
nfa* create_sing_epsilon(void);

/**
 * @brief
 * Computes a NFA recognizing a singleton language {a} containing a single
 * letter word.
 *
 * @remark
 * The computed NFA is defined over the singleton alphabet {a}.
 *
 * @return
 * The NFA.
 */
nfa* create_sing_letter(letter //!< The letter.
);

/**
 * @brief
 * Computes a NFA recognizing a singleton language {w} for some input word w.
 *
 * @remark
 * The computed NFA is defined over the alphabet containing only the letters
 * occurring in w.
 *
 * @return
 * The NFA.
 */
nfa* create_sing_word(word* //!< The word.
);

/**
 * @brief
 * Release of a NFA.
 */
void nfa_delete(nfa* //!< The NFA.
);

/**
 * @brief
 * Overwrites a NFA by copying another NFA and releasing this other NFA.
 */
void nfa_overwrite(nfa*, //!< The NFA that is being overwritten (its original
    //!< fields are released).
    nfa*  //!< The NFA being copied (it is completely released).
);

/*****************************/
/* Simple operations on NFAs */
/*****************************/

/**
 * @brief
 * Copy of a NFA.
 *
 * @return
 * The copy
 */
nfa* nfa_copy(nfa* //!< The NFA.
);

/**
 * @brief
 * Extension of the alphabet of a NFA.
 *
 * @remark
 * The input array contains the letters that should be added to the alphabet. It
 * may contain letters which are already in the alphabet.  The new alphabet is
 * the union between the old one and the set of letters contains in this array.
 *
 * @attention
 * The array containing the new letter must be sorted in increasing order.
 *
 * @return
 * A copy of the original NFA with its alphabet extended.
 */
nfa* nfa_copy_exalpha(nfa*,    //!< The NFA.
    letter*, //!< Array containing the new letters (sorted in increasing order).
    uint      //!< Size of the array.
);

/**
 * @brief
 * Union of two automata (NFAs or DFAs) into a single NFA.
 *
 * @remark
 * The two automata may have dsitinct alphabets. In this case, the alphabet of the
 * computed NFA is the union of the two alphabets.
 *
 * @return
 * A NFA recognizing the union of the two input languages.
 */
nfa* nfa_union(void* I1, //!< First NFA or DFA.
    bool is_dfa_I1, //!< True if the first input is a DFA, false if it is a NFA.
    void* I2, //!< Second NFA or DFA.
    bool is_dfa_I2 //!< True if the second input is a DFA, false if it is a NFA.
);

/**
 * @brief
 * Concatenation of two NFAs.
 *
 * @remark
 * The two NFAs may have dsitinct alphabets. In this case, the alphabet of the
 * computed NFA is the union of the two alphabets.
 *
 * @return
 * A NFA recognizing the concatenation of the two input languages.
 */
nfa* nfa_concat(void* I1, //!< First NFA or DFA.
    bool is_dfa_I1, //!< True if the first input is a DFA, false if it is a NFA.
    void* I2, //!< Second NFA or DFA.
    bool is_dfa_I2 //!< True if the second input is a DFA, false if it is a NFA.
);

/**
 * @brief
 * Kleene star of a NFA.
 *
 * @return
 * A NFA recognizing the Kleene star of the input language.
 */
nfa* nfa_star(nfa* //!< The NFA.
);

/**
 * @brief
 * Kleene plus of a NFA.
 *
 * @return
 * A NFA recognizing the Kleene plus of the input language.
 */
nfa* nfa_plus(nfa* //!< The NFA.
);

/**
 * @brief
 * Mirror of a NFA.
 *
 * @return
 * A NFA recognizing the mirror of the input language.
 */
nfa* nfa_mirror(nfa* //!< The NFA.
);

/**
 * @brief
 * Elimination of the epsilon transitions in a NFA.
 *
 * @return
 * A copy of the input NFA with its epsilon transitions eliminated.
 */
nfa* nfa_elimeps(nfa* //!< The NFA.
);

/**
 * @brief
 * Elimination of the epsilon transitions in a NFA. Overwrites the input NFA.
 */
void nfa_elimeps_mod(nfa* //!< The NFA.
);

/**
 * @brief
 * Elimination of all states that are not simultaneously reachable from an
 * initial state and co-reachable from a final state.
 *
 * @return
 * A copy of the input NFA with its useless states eliminated.
 */
nfa* nfa_trim(nfa* //!< The NFA.
);

/**
 * @brief
 * Elimination of all states that are not simultaneously reachable from an
 * initial state and co-reachable from a final state. Overwrites the input NFA.
 */
void nfa_trim_mod(nfa* //!< The NFA.
);

/*****************************/
/* Generation of random NFAs */
/*****************************/

/**
 * @brief
 * Generates a random NFA.
 *
 * @return
 * The random NFA.
 */
nfa* nfa_random(uint, //!< Size of the alphabet.
    uint, //!< Minimum number of states.
    uint  //!< Maximum number of states.
);


/***********************/
/* Information on NFAs */
/***********************/

/**
 * @brief
 * Computes the number of transitions in a NFA.
 *
 * @return
 * The number of transitions.
 */
int nfa_nb_trans(nfa* //!< The NFA.
);

/**
 * @brief
 * Tests if a NFA is deterministic.
 *
 * @return
 * A Boolean indicating whether the NFA is deterministic.
 */
bool nfa_is_det(nfa* //!< The NFA.
);

/**
 * @brief
 * Tests if a NFA is complete.
 *
 * @return
 * A Boolean indicating whether the NFA is complete.
 */
bool nfa_is_comp(nfa* //!< The NFA.
);

/**
 * @brief
 * Tests if a NFA recognizes the empty language.
 *
 * @return
 * A Boolean indicating whether the NFA recognizes the empty language.
 */
bool nfa_is_empty(nfa* //!< The NFA.
);

/**
 * @brief
 * Tests if a word is accepted by a NFA.
 *
 * @return
 * A Boolean indicating whether the word is accepted.
 */
bool nfa_accepts(nfa*, //!< The NFA.
    word* //!< The word.
);

/**
 * @brief
 * Computes the set of states reached by a word in a NFA.
 *
 * @return
 * The list of states reached by the word.
 */
dequeue* nfa_compute_runs(nfa*, //!< The NFA.
    word* //!< The word.
);




/************************/
/* Partitions of states */
/************************/

/**
 * @brief
 * Merges the states of a NFA according to the partition given as input.
 *
 * @details
 * The new states are the classes of the partition. Given two classes c,c'
 * and a letter a, a transition (c,a,c') is added if and only if there are
 * two states q in c and q' in c' and a transition (q,a,q') in the original
 * automaton. Similarly, a class c is initial (resp. final) if and only if
 * it contains an intial (resp.final) state.
 *
 * @return
 * The merged NFA.
 */
nfa* nfa_merge_states(nfa*,  //!< The NFA.
    parti* //!< Partition of the states.
);



/**********/
/*+ DFAs +*/
/**********/

/**
 * @brief
 * Initializes a DFA.
 *
 * @return
 * The DFA.
 */
dfa* dfa_init(uint size_graph, //!< Number of states.
    uint size_alpha, //!< Size of the alphabet.
    uint nb_finals, //!< Number of final states.
    letter* alphabet //!< The alphabet (NULL if the alphabet is empty). This will be copied.
);

/**
 * @brief
 * Release the state names in a DFA (if there are states names).
 */
void dfa_reset_state_names(dfa* A //!< The DFA.
);

/**
 * @brief
 * Copies a DFA.
 *
 * @return
 * A copy of the DFA.
 */
dfa* dfa_copy(dfa* A //!< The DFA.
);

/**
 * @brief
 * Copies a DFA with an extended alphabet.
 *
 * @remark
 * If there indeed new letters in the alphabet, the new DFA is ectended with a new sink
 * state.
 *
 * @return
 * A copy of the DFA with an extended alphabet.
 */
dfa* dfa_copy_exalpha(dfa* A, //!< The DFA.
    letter* alpha, //!< Array containing the new letters (sorted in increasing order).
    uint size //!< Size of the array.
);



/**
 * @brief
 * Deletes a DFA.
 */
void dfa_delete(dfa* A //!< The DFA.
);

/**
 * @brief
 * Converts a NFA which is deterministic into the complete DFA representation.
 *
 * @remark
 * The function fails if the NFA is not deterministic.
 *
 * @remark
 * If the NFA is not complete, an additional sink state is added.
 *
 * @return
 * The complete DFA.
 */
dfa* detnfa_to_dfa(nfa* A//!< The NFA.
);

/**
 * @brief
 * Converts a DFA into a NFA.
 *
 * @return
 * The NFA.
 */
nfa* dfa_to_nfa(dfa* A //!< The DFA.
);

/**
 * @brief
 * Converts a DFA into a NFA with an extended alphabet.
 *
 * @return
 * The NFA.
 */
nfa* dfa_to_nfa_exalpha(dfa* A, letter* alpha, uint size);


/**
 * @brief
 * Mirror of a DFA.
 *
 * @return
 * A NFA recognizing the mirror of the input language.
 */
nfa* dfa_mirror(dfa* //!< The NFA.
);


/**
 * @brief
 * Kleene star of a DFA.
 *
 * @return
 * A NFA recognizing the Kleene star of the input language.
 */
nfa* dfa_star(dfa* //!< The DFA.
);


/**
 * @brief
 * Generates a random DFA.
 *
 * @return
 * The random DFA.
 */
dfa* dfa_random(uint, //!< Size of the alphabet.
    uint, //!< Minimum number of states.
    uint  //!< Maximum number of states.
);


/**
 * @brief
 * Removes all non-accessible states from a DFA.
 *
 * @return
 * The trimmed DFA.
 */
dfa* dfa_trim(dfa* A //!< The DFA.
);


/**
 * @brief
 * Displays the name of a state in a DFA on a given stream.
 *
 * @remark
 * If no names are defined for the states, each state is named by its index.
 */
void dfa_print_state(const dfa*, //!< The DFA.
    uint,        //!< Index of the state.
    FILE*       //!< The stream.
);

/**
 * @brief
 * Displays a letter in the alphabet of a DFA on a given stream: utf8 version.
 */
void dfa_fprint_letter_utf8(const dfa*, //!< The DFA.
    uint,        //!< Index of the letter.
    FILE*       //!< The stream.
);

/**
 * @brief
 * Checks if there exists a path between two states in a DFA.
 *
 * @attention
 * The function does not check whether the input is indeed a DFA.
 *
 * @return
 * A Boolean indicating whether there exists a path.
 */
bool dfa_exists_path(dfa*, //!< The DFA.
    uint,   //!< The source state.
    uint,   //!< The target state.
    bool* //!< The restriction of the alphabet (NULL if no restriction).
);

/**
 * @brief
 * Computes the run of a DFA on a word.
 *
 * @return
 * The final state reached by the DFA after reading the word.
 * If the word contains an invalid letter, UINT_MAX is returned.
 */
uint dfa_compute_run(dfa* A, //!< The DFA.
    word* w //!< The word.
);









#endif // NFA_H_
