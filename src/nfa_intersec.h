/**
 * @file nfa_intersec.h
 * @brief
 * Intersection of NFAs.
 */

#ifndef NFA_INTERSEC_H
#define NFA_INTERSEC_H

 /*  _   _ _____ _        ___       _                          _   _              */
 /* | \ | |  ___/ \   _  |_ _|_ __ | |_ ___ _ __ ___  ___  ___| |_(_) ___  _ __   */
 /* |  \| | |_ / _ \ (_)  | || '_ \| __/ _ \ '__/ __|/ _ \/ __| __| |/ _ \| '_ \  */
 /* | |\  |  _/ ___ \ _   | || | | | ||  __/ |  \__ \  __/ (__| |_| | (_) | | | | */
 /* |_| \_|_|/_/   \_(_) |___|_| |_|\__\___|_|  |___/\___|\___|\__|_|\___/|_| |_| */

#include "nfa.h"
#include "type_dequeue_gen.h"

 /**
  * @brief
  * Intersection of two NFAs with the product automaton construction.
  *
  * @remark
  * The input Boolean is used to indicate whether the names of the states have to be saved (this only
  * impacts display).  The name of a state is the pair of states to which it corresponds.
  *
  * @return
  * The product automaton of the two inputs.
  */
nfa* nfa_intersect(nfa*, //!< The first NFA.
    nfa*,                //!< The second NFA.
    bool                 //!< A Boolean indicating whether the state names have to be saved.
);


/**
 * @brief
 * Type used for representing a pair of states.
 */
typedef struct {
    uint q1; //!< First state.
    uint q2; //!< Second state.
} nfa_prod_pair;


/**
 * @brief
 * Intersection of two NFAs with the product automaton construction. Returns only
 * the pairs of reachable states.
 *
 * @remark
 * The size of the computed array is returned via the pointer size.
 *
 * @return
 * An array containing the pairs of reachable states in the product automaton.
 */
nfa_prod_pair* nfa_intersec_reach(nfa*, //!< The first NFA.
    nfa*,                         //!< The second NFA.
    uint,                          //!< Starting state in the first NFA.
    uint,                           //!< Starting state in the second NFA.
    uint*                            //!< Pointer used to return the size of the computed array.
);

#endif
