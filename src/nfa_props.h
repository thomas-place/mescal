/**
 * @file monoid_props.h
 * @brief
 * Tests of properties on automata.
 */

#ifndef NFA_PROPS_H
#define NFA_PROPS_H

#include "monoid_props.h"
#include "nfa_props.h"
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
  * @return
  * A Boolean indicating whether the complete DFA is trivial.
  */
bool is_trivial_dfa(dfa*, //!< The DFA.
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
 * @return
 * A Boolean indicating whether all letters have the same action.
 */
bool is_letterind_dfa(dfa*, //!< The DFA.
    int* error, //!< The error code.
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
 * @return
 * A Boolean indicating whether the complete DFA is commutative.
 */
bool is_comm_dfa(dfa*, //!< The DFA.
    int* error, //!< The error code.
    FILE* //!< The stream.
);

/**
 * @brief
 * Tests is an arbitrary NFA is a permutation automata.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * A stream is also taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether the NFA is a permutation automata.
 */
bool is_permutation_dfa(dfa* A, //!< The NFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is counterfree.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * A stream is also taken as input. It is used to display a counter if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether the NFA is counterfree DFA.
 */
bool is_counterfree_dfa(dfa*,      //!< The NFA.
    int* error, // !< The error code.
    FILE*      //!< The stream.
);

typedef enum {
    DFAGP_MOD,
    DFAGP_AMT,
    DFAGP_GR,
} dfagp_mode;

/**
 * @brief
 * Tests if a complete DFA satisfies the DA pattern equation.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the DA pattern equation.
 */
bool is_da_dfa(dfa* A, //!< The complete DFA.
    int* error //!< The error code.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the LDA pattern equation.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_daplus_dfa(dfa* A, //!< The complete DFA.
    int* error //!< The error code.
);



/**
 * @brief
 * Tests if a complete DFA satisfies the restricted DA pattern equation for one
 * of the group prevarieties MOD, AMT or GR.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_dagp_dfa(dfa* A, //!< The complete DFA.
    dfagp_mode mode, //!< The mode indicating which group prevariety to test.
    int* //!< The error code.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted LDA pattern equation for one
 * of the group prevarieties MOD, AMT or GR.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_daplusgp_dfa(dfa* A, //!< The complete DFA.
    dfagp_mode mode, //!< The mode indicating which group prevariety to test.
    int* //!< The error code.
);

/**
 * @brief
 * Tests if a DFA is cycle trivial..
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * A stream is also taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether the DFA is cycle trivial.
 */
bool is_cyclet_dfa(dfa* A, //!< The DFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

bool is_cycletplus_dfa(dfa* A, //!< The DFA.
    int* error //!< The error code.
);

bool is_cycletgp_dfa(dfa* A, //!< The DFA.
    dfagp_mode mode, //!< The mode of the cycle triviality test.
    int* error //!< The error code.
);

bool is_cycletplusgp_dfa(dfa* A, //!< The DFA.
    dfagp_mode mode, //!< The mode of the cycle triviality test.
    int* error //!< The error code.
);

bool is_cycletbp_dfa(dfa* A, int*);


bool is_cycletbpgp_dfa(dfa* A, //!< The DFA.
    dfagp_mode mode, //!< The mode of the cycle triviality test.
    int* error //!< The error code.
);


bool is_nosimple_counter(dfa* A, //!< The DFA.
    int* error //!< The error code.
);



#endif
