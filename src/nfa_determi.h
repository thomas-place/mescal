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
