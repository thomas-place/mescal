/**
 * @file nfa_determi.h
 * @brief
 * Determinization and complementation of NFAs
 */

#ifndef NFA_DETERMI_H
#define NFA_DETERMI_H

 /*  _   _ _____ _        ____  _____ _____ _____ ____  __  __ ___ _   _ ___ _____   _  _____ ___ ___  _   _  */
 /* | \ | |  ___/ \   _  |  _ \| ____|_   _| ____|  _ \|  \/  |_ _| \ | |_ _|__  /  / \|_   _|_ _/ _ \| \ | | */
 /* |  \| | |_ / _ \ (_) | | | |  _|   | | |  _| | |_) | |\/| || ||  \| || |  / /  / _ \ | |  | | | | |  \| | */
 /* | |\  |  _/ ___ \ _  | |_| | |___  | | | |___|  _ <| |  | || || |\  || | / /_ / ___ \| |  | | |_| | |\  | */
 /* |_| \_|_|/_/   \_(_) |____/|_____| |_| |_____|_| \_\_|  |_|___|_| \_|___/____/_/   \_\_| |___\___/|_| \_| */

#include "nfa.h"
#include "printing.h"

/**
 * @brief
 * Determinization of a NFA with the subset construction.
 *
 * @remark
 * The input Boolean is used to indicate whether the names of the states have to be saved (this only
 * impacts display). A stated is named by the corresponding set of states in the subset construction.
 *
 * @return
 * The complete DFA built with the subset construction.
 */
dfa* nfa_determinize(nfa*, //!< The NFA.
    bool                   //!< A Boolean indicating whether the state names have to be saved.
);


/**
 * @brief
 * Represents the required information for determinization of the mirror of a DFA.
 */
typedef struct {
    uint* edges; //!< The edges of the mirror DFA (size size_alpha * size_graph).
    uint* st_edges; //!< For each state q and each label a st_edges[q * size_alpha + a] is the index of
    //!< the first edge in edges that starts from q with label a.
    uint* ed_edges; //!< For each state q and each label a ed_edges[q * size_alpha + a] is the index
    //!< following that of the last edge in edges that starts from q with label a.
} dfa_mirror_info;

/**
 * @brief
 * Initializes the dfa_mirror_info structure from a dfa.
 */
void dfa_get_mirror_info(dfa* A, //!< The DFA.
    dfa_mirror_info* mirror //!< The structure to be initialized (arrays are allocated by the function).
);


/**
 * @brief
 * Determinization of the mirror of a DFA with the subset construction.
 *
 * @remark
 * The input Boolean is used to indicate whether the names of the states have to be saved (this only
 * impacts display). A stated is named by the corresponding set of states in the subset construction.
 *
 * @return
 * The complete DFA built from the mirror with the subset construction.
 */
dfa* dfa_determinize_mirror(dfa*, //!< The DFA.
    bool                   //!< A Boolean indicating whether the state names have to be saved.
);


/**
 * @brief
 * Complementation of a NFA.
 *
 * @return
 * A complete DFA recognizing the complement of the input language.
 */
nfa* nfa_complement(nfa* //!< The NFA.
);


/**
 * @brief
 * Complementation of a DFA.
 *
 * @return
 * A complete DFA recognizing the complement of the input language.
 */
dfa* dfa_complement(dfa* //!< The DFA.
);


#endif
