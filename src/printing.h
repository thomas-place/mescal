#ifndef PRINTING_H
#define PRINTING_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "monoid.h"
#include "type_dequeue.h"
#include "nfa.h"
#include "alloc.h"
#include "type_basic.h"



/********************************/
/*+ Computation of multi-edges +*/
/********************************/
/**
 * @brief
 * Type used to represent a multi-edge in a graph.
 *
 * @remark
 * A multi-edge is an edge that can have multiple labels. This type is used for
 * display purposes: when two vertices are connected by multiple edges, we
 * simplify the presentation by drawing only one edge that carries all the
 * labels of the actual edges.
 */
typedef struct {
    uint in;                          //!< Initial state of the edge
    bool eps;                         //!< Epsilon transition.
    dequeue* lab;                     //!< Sorted list of labels.
    dequeue* lab_i;                   //!< Sorted list of inverse labels.
    uint out;                         //!< Final state of the edge
} multi_edge;

/**
 * @brief
 * Compute the multi-edges of a NFA.
 *
 * @return
 * The list of multi-edges of the NFA stored in a dequeue.
 */
dequeue_gen* nfa_to_multi_edges(nfa* //!< The NFA.
);


/**
 * @brief
 * Compute the multi-edges of a deterministic graph (used for morphisms).
 *
 * @return
 * The list of multi-edges of the graph stored in a dequeue.
 */
dequeue_gen* dgraph_to_multi_edges(dgraph* //!< The deterministic graph.
);

/***********************/
/* Graphviz generation */
/***********************/

void named_lgedges_print(dequeue_gen* theedges, nfa* A, FILE* out);

void named_dedges_print(dequeue_gen* theedges, morphism* M, FILE* out);

void nfa_print(nfa* A, FILE* out);

void cayley_print(morphism* mor, FILE* out);

void cayley_left_print(morphism* mor, FILE* out);

/****************/
/* Shell output */
/****************/


void view_nfa(nfa* nfa);

void view_cayley(morphism* mor);

void view_left_cayley(morphism* mor);


/********************/
/* Latex generation */
/********************/

void latex_init(void);

void latex_print_automaton(nfa* A, FILE* out);

void latex_print_cayley(morphism* M, FILE* out);

void latex_print_lcayley(morphism* M, FILE* out);

#endif
