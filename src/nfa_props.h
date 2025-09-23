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


/***********************/
/* Auxiliary functions */
/***********************/

/**
 * @brief
 * Computes a word that loops on a given state and contains all letters (and only these letters)
 * in the given alphabet.
 *
 * @remark
 * The word is computed as an array of integers indexed by the letters. The last element of
 * the array is set to UINT_MAX to indicate the end of the word.
 *
 * @return
 * A pointer to the word that loops on the given state and contains all letters in the alphabet (NULL if no such word exists).
 */
uint* dfacexa_get_alphaloop(dgraph* g, //!< The dgraph to extract the word from.
    parti* sccs, //<! The partition of the dgraph into strongly connected components (in order to stay within the SCC of the state).
    bool* alph, //!< The alphabet to use for the word (an array indexed by the letters).
    uint s //!< The state to loop on.
);











/***********/
/* Trivial */
/***********/

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
 * Tests if a complete DFA satisfies the DD pattern equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the DD pattern equation.
 */
bool is_ddtrivial_dfa(dfa*, //!< The DFA.
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

/*********/
/* Group */
/*********/

/**
 * @brief
 * Tests is an arbitrary DFA is a permutation automata.
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
 * Tests is an arbitrary DFA satisfies the GR⁺ pattern equation.
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
 * A Boolean indicating whether the DFA satisfies the GR⁺ pattern equation.
 */
bool is_permutationplus_dfa(dfa* A, //!< The NFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/*********/
/* Local */
/*********/

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
 * Tests is a complete commutative DFA is idempotent for the alphabet aa = a (used in AT).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The result is undefined if the DFA is not commutative.
 *
 * @return
 * A Boolean indicating whether the complete DFA is idempotent for the alphabet.
 */
bool is_alphaidem_dfa(dfa*, //!< The DFA.
    int* error, //!< The error code.
    FILE* //!< The stream.
);


/**
 * @brief
 * Tests is a complete DFA satisfies the DD idempotent pattern equation (used in LT).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the DD idempotent pattern equation
 */
bool is_ddidem_dfa(dfa*, //!< The DFA.
    int* error, //!< The error code.
    FILE* //!< The stream.
);

/**
 * @brief
 * Tests is a complete DFA satisfies the DD commutative pattern equation (used in LT).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the DD commutative pattern equation
 */
bool is_ddcomm_dfa(dfa*, //!< The DFA.
    int* error, //!< The error code.
    FILE* //!< The stream.
);


/**
 * @brief
 * Tests is a complete DFA satisfies the LTT commutative pattern equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the LTT commutative pattern equation
 */
bool is_lttcomm_dfa(dfa*, //!< The DFA.
    int* error, //!< The error code.
    FILE* //!< The stream.
);


/*************************/
/* Counter-free variants */
/*************************/

/**
 * @brief
 * Tests if a DFA is G-counterfree for one of the group prevarieties ST, MOD, AMT or GR.
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
bool is_counterfreegp_dfa(dfa*,      //!< The DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, // !< The error code.
    FILE*      //!< The stream.
);

/**
 * @brief
 * Tests if a DFA does not contain any counter of size at most two.
 */
bool is_nosmallcountertwo_dfa(dfa* A);

bool is_nosimple_counter_dfa(dfa* A, //!< The DFA.
    int* error //!< The error code.
);

/*******/
/* Pol */
/*******/

/**
 * @brief
 * Tests if a complete DFA satisfies the Pol(G) pattern equation for one of the group prevarieties ST, MOD or GR.
 *
 * @remark
 * Does not work for the group prevariety AMT.
 *
 * @attention
 * Inefficient for MOD: computes the transition morphism in this case.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the Pol(G) pattern equation.
 */
bool is_polgp_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);


/**
 * @brief
 * Tests if a complete DFA satisfies the Pol(G⁺) pattern equation where G is MOD or GR.
 *
 * @attention
 * Inefficient for MOD and GR: computes the transition morphism. Efficient for ST.
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the Pol(G⁺) pattern equation.
 */
bool is_polgpplus_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);



/**
 * @brief
 * Tests if a complete DFA satisfies the Pol₂(G) pattern equation for one of the group prevarieties MOD, AMT or GR.
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the Pol₂(G) pattern equation.
 */
bool is_pol2gp_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the Pol₂(G⁺) pattern equation for one of the group prevarieties MOD, AMT or GR.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the Pol₂(G⁺) pattern equation.
 */
bool is_pol2gpplus_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/****************/
/*+ R-trivial. +*/
/****************/



/**
 * @brief
 * Tests if a DFA is R-trivial for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the group prevarieties
 */
bool is_rtrivialgp_dfa(dfa* A, //!< The DFA.
    basis mode, //!< The basis
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_rtrivialgpplus_dfa(dfa* A, //!< The DFA.
    basis mode, //!< The basis
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the four prevarieties BPol(ST), BPol(MOD), BPol(AMT) or BPol(GR).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_rtrivialbpgp_dfa(dfa* A, //!< The DFA.
    basis mode, //!< The mode of the cycle triviality test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the four prevarieties BPol(DD), BPol(MOD⁺), BPol(AMT⁺) or BPol(GR⁺).
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_rtrivialbpgpplus_dfa(dfa* A, //!< The DFA.
    basis mode, //!< The mode of the cycle triviality test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/****************/
/*+ L-trivial. +*/
/****************/

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the group prevarieties
 */
bool is_ltrivialgp_dfa(dfa* A, //!< The DFA.
    basis mode, //!< The basis
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialgpplus_dfa(dfa* A, //!< The DFA.
    basis mode, //!< The basis
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialbpgp_dfa(dfa* A, //!< The DFA.
    basis mode, //!< The basis
    int* error, //!< The error code.
    FILE* out //!< The stream.
);


/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialbpgpplus_dfa(dfa* A, //!< The DFA.
    basis mode, //!< The basis
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/********/
/*+ DA +*/
/********/

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
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
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
bool is_dagpplus_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted BpolDA pattern equation for one
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
bool is_dabpgp_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted BpolplusDA pattern equation for one
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
bool is_dabpgpplus_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**********/
/*+ BPol +*/
/**********/

/**
 * @brief
 * Tests if a complete DFA satisfies the PT equation.
 *
 * @remark
 * No display (use ltrivial + rtrivial for a counterexample if needed).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the PT equation.
 */
bool is_piecewise_dfa(dfa* A, //!< The DFA.
    int* error //!< The error code.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(MOD) equation.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(MOD) equation.
 */
bool is_bpolmod_dfa(dfa* A, //!< The complete DFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);


/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(AMT) equation.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(AMT) equation.
 */
bool is_bpolamt_dfa(dfa* A, //!< The complete DFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(DD) equation.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(DD) equation.
 */
bool is_bpoldd_dfa(dfa* A, //!< The complete DFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(MOD⁺) equation.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(MOD⁺) equation.
 */
bool is_bpolmodplus_dfa(dfa* A, //!< The complete DFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);


/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(AMT⁺) equation.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(AMT⁺) equation.
 */
bool is_bpolamtplus_dfa(dfa* A, //!< The complete DFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);


/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(GR⁺) equation.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(GR⁺) equation.
 */
bool is_bpolgrplus_dfa(dfa* A, //!< The complete DFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);


/**
 * @brief
 * Tests if a complete DFA satisfies the restricted Knast(G⁺) pattern equation for one of the
 * four group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the equation.
 */
bool is_knastgpplus_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);


/**
 * @brief
 * Tests if a complete DFA satisfies the restricted Knast(AT) pattern equation.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_knastat_dfa(dfa* A, //!< The complete DFA.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

/**********/
/*+ UPol +*/
/**********/


/**
 * @brief
 * Tests if a complete DFA satisfies the restricted UPol pattern equation for one
 * of the group prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @remark
 * A pointer to an allocated integer is taken as input. If the computation fails
 * (because of a timeout or an interruption), the function fills this integer
 * variable with an error code.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_upolgpplus_dfa(dfa* A, //!< The complete DFA.
    basis mode, //!< The mode indicating which group prevariety to test.
    int* error, //!< The error code.
    FILE* out //!< The stream.
);

bool is_upolbp2gp_dfa(dfa* A, basis mode, int* error, FILE* out);

bool is_upolbp2gpplus_dfa(dfa* A, basis mode, int* error, FILE* out);

#endif
