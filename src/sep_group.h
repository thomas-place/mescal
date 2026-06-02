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
#include <flint/fmpz.h>
#include <flint/fmpz_mat.h>
#include <stdbool.h>
#include <stdlib.h>

/*  ____                             _   _                  ____                        */
/* / ___|  ___ _ __   __ _ _ __ __ _| |_(_) ___  _ __  _   / ___|_ __ ___  _    __ __   */
/* \___ \ / _ \ '_ \ / _` | '__/ _` | __| |/ _ \| '_ \(_) | |  _| '__/ _ \| | | | '_ \  */
/*  ___) |  __/ |_) | (_| | | | (_| | |_| | (_) | | | |_  | |_| | | | (_) | |_| | |_) | */
/* |____/ \___| .__/ \__,_|_|  \__,_|\__|_|\___/|_| |_(_)  \____|_|  \___/\__,_ | .__/  */
/* | | __ _ _ |_|  __ _ _   _  __ _  __ _  ___  ___                             |_|     */
/* | |/ _` | '_ \ / _` | | | |/ _` |/ _` |/ _ \/ __|                                    */
/* | | (_| | | | | (_| | |_| | (_| | (_| |  __/\__ \                                    */
/* |_|\__,_|_| |_|\__, |\__,_|\__,_|\__, |\___||___/                                    */
/*                |___/             |___/                                               */

/********************/
/*+ AMT-separation +*/
/********************/

bool solve_system_amt(fmpz_mat_t MAT, int *target, uint nb_rows, uint nb_cols);

/**
 * @brief
 * Type used to store the information needed to solve AMT-separation on the SCCs of a dgraph.
 *
 * @details
 * Stores a spanning forest. It contains at most one tree by SCC of the dgraph (this
 * tree is a spanning tree of the SCC). Some SCCs can be skipped.
 *
 * @remark
 * Only partial information is stored for each spanning tree: span_forest[s][a] is the number
 * of occurrences of the letter a on the path from the root of the tree to s.
 *
 * @remark
 * For each tree i, the root of the tree is stored in root[i]. The dequeue dropped[i]
 * contains the edges of the dgraph that are not part of the spanning tree.
 * An edge (r,a,s) is represented by the integer r * size_alpha + a
 * (this is a code, since the graph is deterministic).
 */
typedef struct
{
    uint size_graph;   //!< Number of vertices in the dgraph.
    uint size_alpha;   //!< Number of labels in the dgraph.
    uint nb_trees;     //!< Maximal number of trees in the spanning forest (at most the number of SCCs in the dgraph).
    int **span_forest; //!< The spanning forest. For each state q and each letter a, span_forest[q][a] is the number of
                       //!<  occurrences of a on the path from the root of the tree to q in the spanning tree associated to the SCC of q.
    uint *numtree;     //!< Array indexed by the states of the dgraph. For each state q, num_tree[q] is the index of the tree of q in the spanning forest
    uint *root;        //!< Array indexed by the trees of the spanning forest. For each tree i, root[i] is the root of the tree.
    dequeue **dropped; //!< Indexed by the trees of the spanning forest. For each tree i, dropped[i] is the list of all
                       //!< edges within the corresponding SCC that are not used in the spanning tree.
} num_span_forest;

/**
 * @brief
 * Computes a spanning forest from a dgraph.
 *
 * @remark
 * The sccs are not mandatory. If a `NULL` pointer is given, it is assumed that the dgraph consists of a single SCC
 * (if this is not the case, the result is undefined) and the returned forest will contain a single tree whose root
 * is the first state of the dgraph.
 *
 * @return
 * The spanning forest.
 */
num_span_forest *compute_span_forest(
    dgraph *G,    //!< The dgraph
    parti *sccs,  //!< The partition into SCCs of the states (can be `NULL`).
    bool *allowed //!< Array of booleans indexed by the states. If allowed[q] is true, the state q is allowed to be a root of a tree in the spanning forest.
                  //!< Can be `NULL`, in which case all states are allowed to be roots of the trees in the spanning forest. Only considered when sccs is not `NULL`.
);

/**
 * @brief
 * Deletes the structure used to store the spanning forest.
 */
void delete_span_forest(num_span_forest *forest //!< The structure to delete.
);

/**
 * @brief
 * Computes the folding of a dgraph according to the AMT-separation.
 *
 * @remark
 * The partition into sccs is not mandatory. If a `NULL` pointer is given, it
 * is assumed that the dgraph consists of a single SCC (if this is not the case, the result is undefined).
 */
parti *dgraph_amt_fold(dgraph *g,  //!< The dgraph to fold.
                       parti *sccs //!< The partition into SCCs of the states (can be `NULL`).
);

/**
 * @brief
 * Computes the regular elements of the AMT-kernel in a morphism (non-regular elements are ignored).
 */
void compute_amt_kernel_regular(
    morphism *, //!< The morphism.
    bool *,     //!< The array to fill with the elements of the kernel.
    uint *      //!< Used to return the size of the kernel.
);

void build_hnf_matrix_two(dgraph *g1, dgraph *g2, num_span_forest *span1, num_span_forest *span2, uint q1, uint q2, fmpz_mat_t MAT);

/**
 * @brief
 * Computes the anti AMT-pairs (q,t) where q is in the R-class of e and t is in
 * the L-class of f.
 */
void compute_amt_pairs_regular(
    morphism *M,            //!< The morphism.
    num_span_forest *rspan, //!< The spanning forest of the R-classes
    num_span_forest *lspan, //!< The spanning forest of the L-classes
    uint e,                 //!< The idempotent e.
    uint f,                 //!< The idempotent f.
    dequeue *first,         //!< The dequeue to fill with the first elements of each anti-pair.
    dequeue *second         //!< The dequeue to fill with the second elements of each anti-pair.
);

/**************************/
/*+ Dealing with dgraphs +*/
/**************************/

/**
 * @brief
 * Computes the Stallings' partition obtained from a dgraph.
 *
 * @remark
 * In group mode, the alphabet is preserved. In modulo mode, it is reduced to a single letter.
 *
 * @remark
 * The partition into sccs is not mandatory. If a `NULL` pointer is given, it is assumed that all
 * existing transitions are internal to the SCCs (if not, the result is undefined). A transition
 * is nonexistent if its outgoing edge is `UINT_MAX`.
 *
 * @return
 * The partition.
 */
parti *dgraph_stal_fold(dgraph *G,   //!< The graph to fold.
                        parti *sccs, //!< The partition into SCCs of the states.
                        basis ba     //!< The basis mode (ST, MOD, AMT or GR).
);

/**
 * @brief
 * Computes the graph obtained by folding a right or left Cayley graph according to the Stallings' partition.
 * Modulo mode: the alphabet is reduced to a single letter.
 *
 * @remark
 * Undefined transitions (which go outside of an SCC) are mapped to `UINT_MAX`.
 *
 * @return
 * The folded graph.
 */
dgraph *shrink_mod(dgraph *g,   //!< The graph to fold (right or left Cayley graph of a morphism).
                   parti *fold, //!< The Stallings' partition.
                   parti *sccs  //!< The partition into SCCs of the states (R-classes or L-classes).
);

/**
 * @brief
 * Computes the graph obtained by folding a right or left Cayley graph according to the Stalling partition.
 * Modulo mode: the alphabet is reduced to a single letter.
 * Mirror mode: the transitions are reversed.
 *
 * @remark
 * Undefined transitions (which go outside of an SCC) are mapped to `UINT_MAX`.
 *
 * @return
 * The folded graph.
 */
dgraph *shrink_mod_mirror(dgraph *g,   //!< The graph to fold (right or left Cayley graph of a morphism).
                          parti *fold, //!< The Stallings' partition.
                          parti *sccs  //!< The partition into SCCs of the states (R-classes or L-classes).
);

/**
 * @brief
 * Computes the graph obtained by folding a right or left Cayley graph according to the Stalling partition.
 * Group mode: the alphabet is preserved.
 *
 * @remark
 * Undefined transitions (which go outside of a SCC) are mapped to `UINT_MAX`.
 *
 * @return
 * The folded graph.
 */
dgraph *shrink_grp(dgraph *g,   //!< The graph to fold (right or left Cayley graph of a morphism).
                   parti *fold, //!< The Stallings' partition.
                   parti *sccs  //!< The partition into SCCs of the states (R-classes or L-classes).
);

/**
 * @brief
 * Computes the graph obtained by folding a right or left Cayley graph according to the Stalling partition.
 * Group mode: the alphabet is preserved.
 * Mirror mode: the transitions are reversed.
 *
 * @remark
 * Undefined transitions (which go outside of a SCC) are mapped to `UINT_MAX`.
 *
 * @return
 * The folded graph.
 */
dgraph *shrink_grp_mirror(dgraph *g,   //!< The graph to fold (right or left Cayley graph of a morphism).
                          parti *fold, //!< The Stallings' partition.
                          parti *sccs  //!< The partition into SCCs of the states (R-classes or L-classes).
);

/**
 * @brief
 * Folds a single SCC of a dgraph according to a folding partition generated from MOD, AMT or GR.
 *
 * @remark
 * The sccs are not mandatory. If a `NULL` pointer is given, it is assumed that the dgraph contain
 * no edge outside of the SCCs (if this is not the case, the result is undefined). If the sccs
 * are given, the edges outside of the SCCs are not copied to the folded graph.
 *
 * @return
 * The folded graph.
 */
dgraph *dgraph_implement_fold(dgraph *g,   //!< The dgraph to fold.
                              parti *sccs, //!< The partition into SCCs of the states.
                              parti *fold  //!< The partition to use for the fold.
);

/**
 * @brief
 * Computes the Dyck extension of a dgraph.
 */
lgraph *dgraph_dyck_ext(dgraph *G,      //!< The dgraph.
                        bool grp,       //!< Is it in group mode (true) or modulo mode (false)?
                        parti **p_sccs, //!< If not NULL, will be filled with the partition into SCCs of the NFA.
                        parti **p_fold  //!< If not NULL, will be filled with the Stallings' folding partition of the NFA.
);

/**
 * @brief
 * Computes the Dyck extension of a dgraph. Return only the graph of epsilon transitions.
 */
bool *dgraph_dyck_ext_eps(dgraph *G,   //!< The dgraph.
                          bool grp,    //!< Is it in group mode (true) or modulo mode (false)?
                          parti *sccs, //!< The SCCs of the dgraph (can be NULL, in which case the sccs will be computed internally).
                          parti *fold  //!< The Stallings' folding partition of the dgraph (can be NULL, in which case it will be computed internally).
);

dfa *dfa_compute_folding(dfa *A,  //!< The DFA to fold.
                         basis ba //!< The basis mode (ST, MOD, AMT or GR).
);

/***********************/
/*+ Inverse extension +*/
/***********************/

/**
 * @brief
 * Computation of the inverse transitions inside the SCCs of an NFA.
 *
 * @remark
 * If there are already inverse transitions, a copy of the nfa is returned.
 * @return
 * A copy of the NFA with inverse transitions added.
 */
nfa *nfa_inv_ext(nfa *A,     //!< The NFA.
                 parti *sccs //!< The partition into SCCs of the states (can be NULL, in which case it will be computed).
);

/**
 * @brief
 * Removal of the inverse transitions of an NFA.
 *
 * @return
 * A copy of the NFA without inverse transitions.
 */
nfa *nfa_remove_inv(nfa *A //!< The NFA.
);

/***********************************/
/*+ Separation by group languages +*/
/***********************************/

/**
 * @brief
 * Computation of the partition obtained by the Stallings' folding in SCCs.
 *
 * @attention
 * Inverse transitions must be already computed.
 *
 * @return
 * The partition.
 */
parti *nfa_stal_fold(nfa *A,      //!< The NFA (inverse transitions must be computed).
                     parti *SCCS, //!< The partition into SCCs of the states.
                     bool grp     //!< Is it in group mode (true) or modulo mode (false)?
);

/**
 * @brief
 * Computation of the NFA used in the group separation algorithm by adding epsilon transitions
 * between pairs of states connected by a word evaluating to 1 in the free group.
 *
 * @attention
 * Inverse transitions must already be computed.
 *
 * @return
 * The NFA.
 */
nfa *nfa_dyck_ext(nfa *A,         //!< The NFA (inverse transitions must be computed).
                  bool grp,       //!< //!< Is it in group mode (true) or modulo mode (false)?
                  bool names,     //!< Shoul names be computed for the states?
                  parti **p_sccs, //!< If not NULL, will be filled with the partition into SCCs of the NFA.
                  parti **p_fold  //!< If not NULL, will be filled with the Stallings' folding partition of the NFA.
);

nfa *dfa_dyck_ext(dfa *A,         //!< The DFA.
                  bool grp,       //!< Is it in group mode (true) or modulo mode (false)?
                  bool names,     //!< Should names be computed for the states?
                  parti **p_sccs, //!< If not NULL, will be filled with the partition into SCCs of the NFA.
                  parti **p_fold  //!< If not NULL, will be filled with the Stallings' folding partition of the NFA.
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
