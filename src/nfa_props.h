/**
 * @file monoid_props.h
 * @brief
 * Tests of properties on automata.
 */

#ifndef NFA_PROPS_H
#define NFA_PROPS_H

#include "monoid_props.h"
#include "nfa.h"
#include "nfa_determi.h"
#include "nfa_intersec.h"

 /*  _   _ _____ _        ____                            _   _            */
 /* | \ | |  ___/ \   _  |  _ \ _ __ ___  _ __   ___ _ __| |_(_) ___  ___  */
 /* |  \| | |_ / _ \ (_) | |_) | '__/ _ \| '_ \ / _ \ '__| __| |/ _ \/ __| */
 /* | |\  |  _/ ___ \ _  |  __/| | | (_) | |_) |  __/ |  | |_| |  __/\__ \ */
 /* |_| \_|_|/_/   \_(_) |_|   |_|  \___/| .__/ \___|_|   \__|_|\___||___/ */
 /*                                      |_|                               */

 /**
  * @brief
  * Tests if a complete DFA is trivial.
  *
  * @remark
  * A stream is taken as input. It is utilized to display the results of the computations.
  * If this is not desired, a NULL pointer should be given.
  *
  * @attention
  * The function does not check whether the input is indeed a complete DFA.
  *
  * @return
  * A Boolean indicating whether the complete DFA is trivial.
  */
bool is_trivial_dfa(nfa*, //!< The DFA.
    FILE* //!< The stream.
);

/**
 * @brief
 * Tests if all letters have the same action in a complete DFA.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The function does not check whether the input is indeed a complete DFA.
 *
 * @return
 * A Boolean indicating whether all letters have the same action.
 */
bool is_letterind_dfa(nfa*, //!< The DFA.
    FILE* //!< The stream.
);

/**
 * @brief
 * Tests is a complete DFA is commutative.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The function does not check whether the input is indeed a complete DFA.
 *
 * @return
 * A Boolean indicating whether the complete DFA is commutative.
 */
bool dfa_is_comm(nfa*, //!< The DFA.
    FILE* //!< The stream.
);

/**
 * @brief
 * Tests is an arbitrary NFA is a permutation automata.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether the NFA is a permutation automata.
 */
bool is_permutation_dfa(nfa*, //!< The NFA.
    FILE* //!< The stream.
);

/**
 * @brief
 * Tests if an arbitrary NFA is a counterfree DFA.
 *
 * @remark
 *
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * A stream is also taken as input. It is used to display the results of the
 * computations. If this is not desired, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether the NFA is counterfree DFA.
 */
bool is_counterfree_dfa(nfa*,      //!< The NFA.
    int* error, // !< The error code.
    FILE*      //!< The stream.
);



#endif
