/**
 * @file nfa_minimization.h
 * @brief
 * Minimization.
 *
 * @details
 * Two independent algorithms are implemented: Brzozowski and Hopcroft.
 */

#ifndef MINIMIZATION_H
#define MINIMIZATION_H

 /*  _   _ _____ _        __  __ _       _           _          _   _              */
 /* | \ | |  ___/ \   _  |  \/  (_)_ __ (_)_ __ ___ (_)______ _| |_(_) ___  _ __   */
 /* |  \| | |_ / _ \ (_) | |\/| | | '_ \| | '_ ` _ \| |_  / _` | __| |/ _ \| '_ \  */
 /* | |\  |  _/ ___ \ _  | |  | | | | | | | | | | | | |/ / (_| | |_| | (_) | | | | */
 /* |_| \_|_|/_/   \_(_) |_|  |_|_|_| |_|_|_| |_| |_|_/___\__,_|\__|_|\___/|_| |_| */


#include "nfa_determi.h"
#include "printing.h"

/***************/
/*+ Auxiliary +*/
/***************/

/**
 * @brief
 * Makes a copy of a minimal DFA. The ordering between the states is changed
 * to make the copy canonical.
 *
 * @attention
 * The input DFA is assumed to be minimal. This is not checked.
 *
 * @return
 * The canonical copy of the input DFA.
 */
dfa* dfa_mini_canonical_copy(dfa*  //!< The DFA to copy.
);


/****************/
/*+ Brzozowski +*/
/****************/

/**
 * @brief
 * Brzozowski's minimization algorithm (for input NFAs)
 *
 * @return
 * The minimal automaton of the input language.
 */
dfa* nfa_brzozowski(nfa* //!< The NFA.
);


/**
 * @brief
 * Brzozowski's minimization algorithm (fo rinput DFAs)
 *
 * @return
 * The minimal automaton of the input language.
 */
dfa* dfa_brzozowski(dfa* //!< The DFA.
);


/**************/
/*+ Hopcroft +*/
/**************/

/**
 * @brief
 * Type used to represent the state partition used in the Hopcroft algorithm.
 */
typedef struct {
    uint size_set;      //!< The size of the partitioned set.
    uint size_par;      //!< The size of the partition.
    uint* classes;      //!< Array of classes: maps each element to its class.
    uint* parray;       //!< Array containing all elements of the set grouped by classes (elements belonging to the same class are next to each other).
    uint* parray_i;     //!< Inverse of the previous array: each element in the set is mapped to each index in parray.
    uint* rindex;       //!< Array indexed by the classes: each class is mapped to the index following this class in parray.
    uint* lindex;       //!< Array indexed by the classes: each class is mapped to the index at which this class starts in parray.
} hopcroft_partition;



/**
 * @brief
 * Computes the initial partition of the states in Hopcroft's algorithm. The set of final states is assumed
 * to be non-trivial.
 *
 * @remark
 * The computed partition has two classes since the set of final states is non-trivial.
 *
 *
 * @return
 * The initial partition.
 */
hopcroft_partition* dfa_hopcroft_initial(uint, //!< The number of states in the automaton.
    uint*,                                   //!< The list of final states (assumed to be non-trivial).
    uint //!< The number of final states (assumed to be non-trivial).
);

/**
 * @brief
 * Computation of the minimal automaton from the partition computed by the Hopcroft's algorithm.
 *
 * @return
 * The minimal automaton.
 */
dfa* dfa_hopcroft_genauto(dfa*,  //!< The original complete DFA.
    hopcroft_partition*          //!< The partition. 
);

/**
 * @brief
 * Release of a Hopcroft's partition.
 */
void dfa_hopcroft_free(hopcroft_partition*  //!< The partition.
);

/**
 * @brief
 * Hopcroft's minimization algorithm.
 *
 * @return
 * The minimal automaton of the input DFA.
 */
dfa* dfa_hopcroft(dfa*   //!< The DFA.
);


/************************/
/** Canonical ordering **/
/************************/

/**
 * @brief
 * Computes the canonical ordering of the states of a minimal DFA.
 *
 * @return
 * The canonical ordering of the states.
 */
bool** dfa_mini_canonical_ordering(dfa*  //!< The DFA to order (must be minimal).
);

#endif
