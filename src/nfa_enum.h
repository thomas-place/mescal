/**
 * @file nfa_enum.h
 * @brief
 * Enumeration of NFAs.
 */
#ifndef NFA_ENUM_H
#define NFA_ENUM_H

#include "nfa.h"


typedef struct
{
    short states;   //!< Number of states.
    short alpha;    //!< Number of letters in the alphabet.
    short** graph;  //!< Transition graph.
    short* used;    //!< Used states at each point.
    short* outlabs; //!< Number of letters going out of the used states.
    short** parti;  //!< Integer partition of the outgoing letters.
    short* parsize; //!< Sizes of the partition.
    short inisep;   //!< Number of letters looping on the initial state. 
    short final;    //!< Final state.
} nfa_enum;



/**
 * @brief
 * Initializes an enumeration of NFAs.
 */
nfa_enum* nfa_enum_init(short, //!< Number of states.
    short                   //!< Number of letters in the alphabet.
);

/**
 * @brief
 * Frees the memory allocated for an enumeration of NFAs.
 */
void nfa_enum_free(nfa_enum* //!< The enumeration.
);

/**
 * @brief
 * Computes the next NFA in the enumeration.
 */
bool nfa_enum_next(nfa_enum* //!< The enumeration.
);



short iterate_integer_partition(short* //!< The partition.
    , short                             //!< The partitioned integer
);

nfa* nfa_enum_to_nfa(nfa_enum* //!< The enumeration.
);


void nfa_enum_print(nfa_enum* E);



#endif // NFA_ENUM_H
