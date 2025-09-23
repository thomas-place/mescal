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

//#define NFA_INTER_DEBUG


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
 * Intersection of two DFAs with the product automaton construction.
 *
 * @remark
 * The input Boolean is used to indicate whether the names of the states have to be saved (this only
 * impacts display).  The name of a state is the pair of states to which it corresponds.
 *
 * @return
 * The product automaton of the two inputs.
 */
dfa* dfa_intersect(dfa*, //!< The first DFA.
    dfa*,                //!< The second DFA.
    bool                 //!< A Boolean indicating whether the state names have to be saved.
);

/**
 * @brief
 * Intersection of two NFAs or DFAs.
 *
 * @remark
 * The two inputs may have distinct alphabets. In this case, the alphabet of the
 * computed NFA is the union of the two alphabets.
 *
 * @attention
 * The two inputs must be either both NFAs or both DFAs. If one of the inputs is a DFA and the other is a NFA, the function will return a NFA.
 *
 * @return
 * A NFA or DFA recognizing the intersection of the two input languages.
 */
void* nfa_intersect_mixed(void* I1, //!< First NFA or DFA.
    bool is_dfa_I1, //!< True if the first input is a DFA, false if it is a NFA.
    void* I2, //!< Second NFA or DFA.
    bool is_dfa_I2, //!< True if the second input is a DFA, false if it is a NFA.
    bool names //!< A Boolean indicating whether the state names have to be saved.
);

/**
 * @brief
 * Computes the power product of a DFA (for specified initial states).
 *
 * @remark
 * The input DFA must be deterministic and complete.
 *
 * @return
 * A DFA that is the power product of the input DFA with itself n times.
 */
dfa* dfa_power_prod(dfa* A, //!< The DFA to be iterated.
    uint n, //!< The number of copies of the DFA to be used in the product.
    int* initial_states, //!< List of initial states for each copy of the DFA (the number of initial states must be equal to n).
    int* final_states //!< List of final states for each copy of the DFA (the number of final states must be equal to n). Optionnal parameter, can be NULL.
);



/**
 * @brief
 * Type used for representing a pair of states.
 */
typedef struct {
    uint q1; //!< First state.
    uint q2; //!< Second state.
} prod_pair;


prod_pair* dgraph_intersec(dgraph*, //!< The first graph
    dgraph*,                         //!< The second graph
    uint,                          //!< Starting state in the first graph.
    uint,                           //!< Starting state in the second graph.
    uint*                            //!< Pointer used to return the size of the computed array.
);

bool dgraph_exists_path(dgraph*, //!< The graph
    uint,                          //!< Starting state in the graph.
    uint,                           //!< Ending state in the graph.
    bool strict,                     //!< A Boolean indicating whether the path must be strict (no self-loops).
    uint** word //!< The word that is the intersection path (NULL if not needed).
);

bool dgraph_exists_path_alpha(dgraph*, //!< The graph
    uint s,                          //!< Starting state in the graph.
    uint e,                           //!< Ending state in the graph.
    bool strict,                     //!< A Boolean indicating whether the path must be strict (no self-loops).
    bool* alpha, //!< The alphabet to use for the path (NULL if not needed).
    uint** word //!< The word that is the intersection path (NULL if not needed).
);

uint dgraph_exists_path_letter_alpha(dgraph* g, //!< The graph
    uint s,                          //!< Starting state in the graph.
    uint b,                           //!< Letter searched.
    bool* alpha, //!< The alphabet to use for the path (NULL if not needed).
    uint** word //!< The word that is the intersection path (NULL if not needed).
);




bool dgraph_exists_intersec_path(dgraph*, //!< The first graph
    dgraph*,                         //!< The second graph
    uint,                          //!< Starting state in the first graph.
    uint,                           //!< Starting state in the second graph.
    uint,                           //!< Ending state in the first graph.
    uint,                            //!< Ending state in the second graph.
    bool strict,                     //!< A Boolean indicating whether the path must be strict (no self-loops).
    uint** word //!< The word that is the intersection path (NULL if not needed).
);

bool dgraph_exists_intersec_path_alpha(dgraph*, //!< The first graph
    dgraph*,                         //!< The second graph
    uint,                          //!< Starting state in the first graph.
    uint,                           //!< Starting state in the second graph.
    uint,                           //!< Ending state in the first graph.
    uint,                            //!< Ending state in the second graph.
    bool strict,                     //!< A Boolean indicating whether the path must be strict (no self-loops).
    bool* alpha, //!< The alphabet to use for the path (NULL if not needed).
    uint** word //!< The word that is the intersection path (NULL if not needed).
);



#endif
