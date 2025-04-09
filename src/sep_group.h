/**
 * @file sep_group.h
 * @brief
 * Separation by group languages.
 */

#ifndef SEP_GROUP_H
#define SEP_GROUP_H

#include "graphs_tarjan.h"
#include "nfa.h"
#include "nfa_intersec.h"
#include "printing.h"
#include "type_dlist.h"
#include "type_partitions.h"
#include <stdbool.h>
#include <stdlib.h>

/*  ____                             _   _                  ____ */
/* / ___|  ___ _ __   __ _ _ __ __ _| |_(_) ___  _ __  _   / ___|_ __ ___  _   _
 * _ __   */
/* \___ \ / _ \ '_ \ / _` | '__/ _` | __| |/ _ \| '_ \(_) | |  _| '__/ _ \| | |
 * | '_ \  */
/*  ___) |  __/ |_) | (_| | | | (_| | |_| | (_) | | | |_  | |_| | | | (_) | |_|
 * | |_) | */
/* |____/ \___| .__/ \__,_|_|  \__,_|\__|_|\___/|_| |_(_)  \____|_|  \___/
 * \__,_| .__/  */
/* | | __ _ _ |_|  __ _ _   _  __ _  __ _  ___  ___ |_|     */
/* | |/ _` | '_ \ / _` | | | |/ _` |/ _` |/ _ \/ __| */
/* | | (_| | | | | (_| | |_| | (_| | (_| |  __/\__ \ */
/* |_|\__,_|_| |_|\__, |\__,_|\__,_|\__, |\___||___/ */
/*                |___/             |___/ */

/***********************/
/*+ Inverse extension +*/
/***********************/

/**
 * @brief
 * Computation of the inverse transitions inside the SCCs of an NFA.
 *
 * @remark
 * If there are already inverse transitions, they are deleted and computed
 * again. Epsilon transitions are eliminated before the computation.
 *
 * @return
 * The partition into SCCs used for the computation (it is often useful for
 * further computations).
 */
parti *nfa_inv_ext(nfa * //!< The NFA.
);

/**
 * @brief
 * Remove all inverse transitions in an NFA.
 */
void nfa_remove_inv(nfa * //!< The NFA.
);

/***********************************/
/*+ Separation by group languages +*/
/***********************************/

/**
 * @brief
 * Computation of the partition obtained by the Stallings folding in SCCs.
 *
 * @attention
 * Inverse transitions must be already computed.
 *
 * @return
 * The partition.
 */
parti *nfa_stal_fold(nfa *, //!< The NFA (inverse transitions must be computed).
                     parti * //!< The partition into SCCs of the states.
);

/**
 * @brief
 * Computation of the NFA used in the group separation algorithm by adding
 * epsilon transitions between pairs of states connected by a word in the Dyck
 * language.
 *
 * @attention
 * Inverse transitions must already be computed.
 *
 * @return
 * The NFA.
 */
nfa *nfa_dyck_ext(
    nfa *,  //!< The NFA (inverse transitions must be computed).
    parti * //!< The partition corresponding to the folding of the SCCs.
);

/**
 * @brief
 * GR-separation.
 *
 * @remark
 * Results of the computation are displayed on a stream given in parameter.
 * A `NULL` pointer should be given if no display is wanted. It
 * is possible to ask for details on the computation using the Boolean.
 *
 * @return
 * A Boolean indicating whether the two input languages are GR-separable.
 */
bool decid_grp_sep(nfa *, //!< First NFA.
                   nfa *, //!< Second NFA.
                   bool,  //!< Should further details be displayed?
                   FILE * //!< The stream.
);

/************************************/
/*+ Separation by modulo languages +*/
/************************************/

/**
 * @brief
 * Projection of a NFA over a one-letter alphabet.
 *
 * @return
 * The projection.
 */
nfa *nfa_proj_unary(nfa * //!< The NFA.
);

/**
 * @brief
 * MOD-separation.
 *
 * @remark
 * Results of the computation are displayed on a stream given as input.
 * A `NULL` pointer should be given if no display is wanted. It
 * is possible to ask for details on the computation using the Boolean.
 *
 * @return
 * A Boolean indicating whether the two input languages are MOD-separable.
 */
bool decid_mod_sep(nfa *, //!< First NFA.
                   nfa *, //!< Second NFA.
                   bool,  //!< Should further details be displayed?
                   FILE * //!< The stream.
);

/*************************************/
/*+ Separation by trivial languages +*/
/*************************************/

/**
 * @brief
 * ST-separation.
 *
 * @remark
 * Results of the computation are displayed on a stream given as input.
 * A `NULL` pointer should be given if no display is wanted.
 *
 * @return
 * A Boolean indicating whether the two input languages are ST-separable.
 */
bool decid_st_sep(nfa *, //!< First NFA.
                  nfa *, //!< Second NFA.
                  FILE * //!< The stream.
);

#endif
