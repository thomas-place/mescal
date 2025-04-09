/**
 * @file monoid_kernels.h
 * @brief
 * Computation of kernels.
 */

#ifndef MONOID_KERNELS_H
#define MONOID_KERNELS_H

/*  _  __                    _      */
/* | |/ /___ _ __ _ __   ___| |___  */
/* | ' // _ \ '__| '_ \ / _ \ / __| */
/* | . \  __/ |  | | | |  __/ \__ \ */
/* |_|\_\___|_|  |_| |_|\___|_|___/ */

#include "monoid.h"
#include "monoid_sub.h"
#include "sep_group.h"

/**
 * @brief
 * Computes a DFA from the right Cayley graph of a morphism. Discards all
 * transitions which entering a new regular R-class.
 *
 * @remark
 * Used to compute full Kernels.
 *
 * @return
 * The DFA.
 */
nfa *morphism_to_dfa_kernel(morphism * //!< The morphism.
);

/**
 * @brief
 * Computes a DFA from the right Cayley graph of a morphism. Discards all
 * transitions which are not internal to the R-classes.
 *
 * @remark
 * Used to compute the restriction of Kernels to regular elements.
 *
 * @return
 * The DFA.
 */
nfa *morphism_to_dfa_rcl(morphism * //!< The morphism.
);

/**
 * @brief
 * Computes a DFA from the left Cayley graph of a morphism. Discards all
 * transitions which are not internal to the L-classes.
 *
 * @remark
 * Used to compute the restriction of Kernels to regular elements.
 *
 * @return
 * The DFA.
 */
nfa *morphism_to_dfa_lcl(morphism * //!< The morphism.
);

/**
 * @brief
 * Computes the GR-kernel of a morphism.
 *
 * @remark
 * The available computation levels are LV_REG and LV_FULL. If LV_GREG is used,
 * the computation defaults to LV_FULL.
 *
 * @return
 * The GR-kernel.
 */
subsemi *
get_grp_kernel(morphism *, //!< The morphism.
               sub_level //!< The desired computation level of the subsemigroup.
);

/**
 * @brief
 * Computes the MOD-kernel of a morphism.
 *
 * @remark
 * The available computation levels are LV_REG and LV_FULL. If LV_GREG is used,
 * the computation defaults to LV_FULL.
 *
 * @return
 * The MOD-kernel.
 */
subsemi *
get_mod_kernel(morphism *, //!< The morphism.
               sub_level //!< The desired computation level of the subsemigroup.
);

/*****************/
/** AMT-kernels **/
/*****************/

/**
 * @brief
 * Type used to store the information needed to compute the AMT-kernel.
 *
 * @details
 * Stores a spanning tree for each regular R- or L-class (depending on the
 * arguments) plus the list of dropped edges in the construction.
 *
 * @remark
 * Only partial information is stored for each spanning tree: span_trees[s][a]
 * is the number of occurrences of the letter a on the path from the root of the
 * tree to s (the root being an arbitrary fixed idempotent in the R or L class
 * of s).
 *
 * @remark
 * The dropped edges are stored in dequeues. An edge (r,a,s) is stored in the
 * dequeue of the class of r and represented by the integer r * size_alpha + a
 * (this is a code since the graph is deteministic).
 */
typedef struct {
    dgraph *cay; //!< The Cayley graph used to compute the spanning trees.
    parti *P;    //!< The partition into strongly connected components of the
                 //!< Cayley graph.
    int **span_trees; //!< The spanning trees. Indexed by the elements, then the
                      //!< letters.
    dequeue **dropped; //!< The dropped edges (we use them to close the cycles).
                       //!< Indexed by the classes of the partition. NULL if the
                       //!< class is not regular.
} num_span_trees;

/**
 * @brief
 * Computes the spanning trees from a morphism.
 *
 * @return
 * The spanning trees.
 */
num_span_trees *compute_num_span_trees(
    morphism *, //!< The morphism.
    bool //!< True if the spanning trees are computed from the right Cayley
         //!< graph, false if they are computed from the left one.
);

/**
 * @brief
 * Deletes the structure used to store the spanning trees.
 */
void delete_num_span_trees(num_span_trees * //!< The structure to delete.
);

/**
 * @brief
 * Computes the regular elements of the AMT-kernel in a morphism.
 */
void compute_amt_kernel_regular(
    morphism *, //!< The morphism.
    bool *,     //!< The array to fill with the elements of the kernel.
    uint *      //!< Used to return the size of the kernel.
);

/**
 * @brief
 * Computes the anti AMT-pairs (q,t) where q is in the R-class of e and t is in
 * the L-class of f.
 */
void compute_amt_pairs_regular(
    num_span_trees *, //!< The spanning trees of the R-classes
    num_span_trees *, //!< The spanning trees of the L-classes
    uint,             //!< The idempotent e.
    uint,             //!< The idempotent f.
    dequeue
        *, //!< The dequeue to fill with the first elements of each anti-pair.
    dequeue
        * //!< The dequeue to fill with the second elements of each anti-pair.
);

/**
 * @brief
 * Computes the AMT-kernel of a morphism.
 *
 * @remark
 * The only available computation level is LV_REG. If another level is passed,
 * the computation defaults to LV_REG.
 *
 * @return
 * The AMT-kernel (restricted to its regular elements).
 */
subsemi *
get_amt_kernel(morphism *, //!< The morphism.
               sub_level //!< The desired computation level of the subsemigroup.
);

#endif
