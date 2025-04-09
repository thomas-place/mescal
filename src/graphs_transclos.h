/**
 * @file graphs_transclos.h
 * @brief
 * Computation of transitive closures in graphs.
 */

#ifndef TRANSCLOS_H
#define TRANSCLOS_H

 /*   ____                 _             _                       _ _   _            */
 /*  / ___|_ __ __ _ _ __ | |__  ___ _  | |_ _ __ __ _ _ __  ___(_) |_(_)_   _____  */
 /* | |  _| '__/ _` | '_ \| '_ \/ __(_) | __| '__/ _` | '_ \/ __| | __| \ \ / / _ \ */
 /* | |_| | | | (_| | |_) | | | \__ \_  | |_| | | (_| | | | \__ \ | |_| |\ V /  __/ */
 /*  \____|_|  \__,_| .__/|_| |_|___(_)  \__|_|  \__,_|_| |_|___/_|\__|_| \_/ \___| */
 /*   ___| | ___  __|_|   _ _ __ ___                                                */
 /*  / __| |/ _ \/ __| | | | '__/ _ \                                               */
 /* | (__| | (_) \__ \ |_| | | |  __/_                                              */
 /*  \___|_|\___/|___/\__,_|_|  \___(_)                                             */

#include <stdbool.h>
#include "graphs.h"
#include "graphs_tarjan.h"
#include "type_binheap.h"

/***************************************************/
/* Functions restricted to directed acyclic graphs */
/***************************************************/

/**
 * @brief
 * Given as input an arbitrary graph and its partition into SCCs, computes the
 * associated DAG of SCCs (each class of the partition is a vertex).
 *
 * @return
 * The DAG of SCCs.
 */
graph* compute_dag_of_sccs(graph*, //!< An arbitray graph.
    parti*                      //!< The partition into SCCs.
);

/**
 * @brief
 * Given a DAG as input, computes a topological ordering on its vertices.
 *
 * @attention
 * The input graph must be a DAG (the algorithm does not terminate otherwise).
 *
 * @return
 * A list of all vertices in the graph sorted according to a topological ordering.
 */
dequeue* topo_sort_dag(graph* //!< A directed acyclic graph.
);


/**
 * @brief
 * Given a DAG as input and a vertex of this graph, computes a topological ordering on
 * the vertices that are reachable from the input vertex.
 *
 * @attention
 * The input graph must be a DAG (the algorithm does not terminate otherwise).
 *
 * @return
 * A list of all reachable vertices sorted according to a topological ordering.
 */
dequeue* topo_sort_dag_start(graph*, //!< A directed acyclic graph.
    uint                          //!< A vertex.
);


/**
 * @brief
 * Computes the transitive closure of a DAG.
 *
 * @attention
 * A list of all vertices sorted in topological order must be given as input.
 *
 * @return
 * The transitive closure.
 */
graph* compute_tclos_dag(graph*, //!< A directed acyclic graph.
    dequeue*                  //!< A list of all vertices sorted according to a topological ordering.
);

/**********************************/
/* Functions for arbitrary graphs */
/**********************************/

/**
 * @brief
 * Makes the transitive closure of the input graph.
 */
void make_tclos_graph(graph* //!< An arbitrary graph.
);

#endif
