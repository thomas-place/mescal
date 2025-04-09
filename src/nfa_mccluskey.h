/**
 * @file nfa_mccluskey.h
 * @brief
 * Computation of a regular expression from a NFA with the Brzozowski-McCluskey algorithm.
 */

#ifndef MCCLUSKEY_H
#define MCCLUSKEY_H

 /*  _   _ _____ _        ____                                    _    _   __  __       ____ _           _               */
 /* | \ | |  ___/ \   _  | __ ) _ __ _______ _________      _____| | _(_) |  \/  | ___ / ___| |_   _ ___| | _____ _   _  */
 /* |  \| | |_ / _ \ (_) |  _ \| '__|_  / _ \_  / _ \ \ /\ / / __| |/ / | | |\/| |/ __| |   | | | | / __| |/ / _ \ | | | */
 /* | |\  |  _/ ___ \ _  | |_) | |   / / (_) / / (_) \ V  V /\__ \   <| | | |  | | (__| |___| | |_| \__ \   <  __/ |_| | */
 /* |_| \_|_|/_/   \_(_) |____/|_|  /___\___/___\___/ \_/\_/ |___/_|\_\_| |_|  |_|\___|\____|_|\__,_|___/_|\_\___|\__, | */
 /*   __ _| | __ _  ___  _ __(_) |_| |__  _ __ ___                                                                |___/  */
 /*  / _` | |/ _` |/ _ \| '__| | __| '_ \| '_ ` _ \                                                                      */
 /* | (_| | | (_| | (_) | |  | | |_| | | | | | | | |                                                                     */
 /*  \__,_|_|\__, |\___/|_|  |_|\__|_| |_|_| |_| |_|                                                                     */
 /*          |___/                                                                                                       */

#include "nfa.h"
#include "regexp.h"


/**
 * @brief
 * Type used to represent the generalized automata used in the algorithm.
 *
 * @remark
 * In a generalized automaton, there is a single initial state and a single final state.
 * Furthermore, there exists a most one transition between two states which is labeld by
 * a regular expression.
 *
 * The transistions are represented by a two dimensional array:
 * - The state 0 is the initial one.
 * - The state 1 is the final one.
 * - For two states "q" and "r", the celk "matrix[q][r] contains the regular expression which labels
 *   the unique transition from "q" to "r" (if there is no such transition, the cell contains a NULL
 *   pointer).
 */
typedef struct {
    uint size;        //!< The size of this automaton.
    regexp*** matrix; //!< The two-dimensional array (size * size) which stores the transitions.
} mccluskey_auto;


/**
 * @brief
 * Computes a generalized automaton from an arbitrary NFA.
 *
 * @return
 * A generalized automaton recognizing the same language as the input NFA.
 **/
mccluskey_auto* nfa_to_mccluskey(nfa* //!< The NFA.
);

/**
 * @brief
 * Brzozowski-McCluskey algorithm.
 *
 * @return
 * A regular expression recognizing the same language as the input NFA.
 **/
regexp* nfa_mccluskey(nfa* //!< The NFA.
);



#endif
