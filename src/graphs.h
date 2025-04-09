/**
 * @file graphs.h
 * @brief
 * Implementation of graphs.
 *
 * @details
 * Contains the various types used to represent graphs and some functions that can
 * manipulate them.
 */

#ifndef GRAPHS_H
#define GRAPHS_H

 /*   ____                 _          */
 /*  / ___|_ __ __ _ _ __ | |__  ___  */
 /* | |  _| '__/ _` | '_ \| '_ \/ __| */
 /* | |_| | | | (_| | |_) | | | \__ \ */
 /*  \____|_|  \__,_| .__/|_| |_|___/ */
 /*                 |_|               */

#include "type_abr.h"
#include "type_basic.h"
#include "type_binheap.h"
#include "type_dequeue.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief
 * Type used to represent a directed unlabeled graph.
 *
 * @remark
 * The set of edges is represented by the array of dequeues `edges`.
 * For each vertex `q`, the cell `edge[q]` contains a list of all vertices that
 * are connected to `q` by an edge.
 *
 * This list must be sorted in increasing order.
 */
typedef struct {
    uint size;       //!< Number of vertices.
    dequeue** edges; //!< The edges: an array of size "size".
} graph;

/**
 * @brief
 * Type used to represent a directed labeled graph.
 *
 * @remark
 * The set of edges is represented by the array of arrays of dequeues `edges`.
 * For each vertex `q` and each letter `a` the cell `edge[q][a]` contains a list
 * of all vertices that are connected to `q` by an edge labeled by `a`.
 * This list must be sorted in increasing order.
 */
typedef struct {
    uint size_graph;  //!< Number of vertices.
    uint size_alpha;  //!< Number of labels.
    dequeue*** edges; //!< The edges: an array of arrays of size `size_graph * size_alpha`.
} lgraph;

/**
 * @brief
 * Type used to represent a complete deterministic directed labeled graph.
 *
 * @remark
 * The set of edges is represented by the array of arrays of usigned integers `edges`.
 * For each vertex `q` and each letter `a` the cell `edge[q][a]` contains the
 * unique vertex connected to `q` by an edge labeled by `a`.
 */
typedef struct {
    uint size_graph; //!< Number of vertices.
    uint size_alpha; //!< Number of labels.
    uint** edges;    //!< The edges: an array of arrays of size "size_graph * size_alpha".
} dgraph;

/**************/
/* Allocation */
/**************/

/**
 * @brief
 * Creates a directed unlabeled graph without edges.
 *
 * @return
 * The created graph.
 */
graph* create_graph_noedges(uint //!< Number of vertices.
);

/**
 * @brief
 * Creates a directed unlabeled graph whose only edges are self-loops.
 *
 * @return
 * The created graph.
 */
graph* create_graph_selfloops(uint //!< Number of vertices.
);

/**
 * @brief
 * Releases a directed unlabeled graph.
 */
void delete_graph(graph* //<! The graph that needs to be freed.
);

/**
 * @brief
 * Creates a directed graph without edges.
 *
 * @return
 * The created graph.
 */
lgraph* create_lgraph_noedges(uint, //!< Number of vertices.
    uint //!< Number of labels.
);

/**
 * @brief
 * Releases a directed graph.
 */
void delete_lgraph(lgraph* //<! The graph that needs to be freed.
);

/**
 * @brief
 * Creates a complete deterministic directed graph without edges.
 *
 * @return
 * The created graph.
 */
dgraph* create_dgraph_noedges(uint, //!< Number of vertices.
    uint //!< Number of labels.
);

/**
 * @brief
 * Release of a complete deterministic directed graph.
 */
void delete_dgraph(dgraph* //<! The graph that needs to be freed.
);


/*******************/
/* Basic functions */
/*******************/

/**
 * @brief
 * Computes the number of edges in a directed unlabeled graph.
 * @return
 * The number of edges in the graph.
 */
int graph_nb_edges(graph* //<! The graph.
);

/**
 * @brief
 * Computes the number of edges in a directed labeled graph.
 * @return
 * The number of edges in the graph.
 */
int lgraph_nb_edges(lgraph* //<! The graph.
);

/**
 * @brief
 * Computes the number of edges in a complete deterministic directed labeled graph.
 * @return
 * The number of edges in the graph.
 */
int dgraph_nb_edges(dgraph* //<! The graph.
);

/************/
/* Searches */
/************/

/**
 * @brief
 * Names for the two available algorithms for searches.
 */
typedef enum {
    DFS,
    BFS,
} graph_stype;

/**
 * @brief
 * Searches in an unlabeled graph. A set of already visited vertices is taken as input.
 * This set is updated by the search.
 *
 * @attention
 * The starting set is emptied by the function (it serves as a stack or a queue).
 */
void graph_search_update(graph_stype, //!< The type of search that has to be used.
    graph*,     //!< The graph.
    dequeue*,   //!< The starting set of vertices.
    bool*       //!< A set of already visited vertices. It is updated by the fucntion.
);

/**
 * @brief
 * Searches in an unlabeled graph. Reachable vertices are returned inside a dequeue sorted in increasing
 * order.
 *
 * @attention
 * The starting set is emptied by the function (it serves as a stack or a queue).
 *
 * @remark
 * An array of Booleans is taken as input. It is indexed by the vertices in the graph and represents
 * a subset of these vertices. It is used to restrict the list of reachable vertices to those in the
 * given set. If no restriction is needed, a NULL pointer should be given as input.
 *
 * @return
 * The list of reachable vertices sorted in increasing order.
 */
dequeue* graph_search(graph_stype, //!< The type of search that has to be used.
    graph*,     //!< The graph.
    dequeue*,   //!< The starting set of vertices.
    bool*       //!< An array indexed by the vertices. Used to restrict the list of reachable vertices.
);

/**
 * @brief
 * Search in a labeled graph. A set of already visited vertices is taken as input. This set
 * is updated by the search.
 *
 * @remark
 * An additional array of Booleans is taken as input. It is indexed by the labels and represents a
 * set of labels. It restricts the edges that can be used to those labeled by a label in this set.
 * If no restriction is needed, a NULL pointer should be given as input.
 *
 * @attention
 * The starting set is emptied by the function (it serves as a stack or a queue).
 */
void lgraph_search_update(graph_stype, //!< The type of search that has to be used.
    lgraph*,    //!< The graph.
    dequeue*,   //!< The starting set of vertices.
    bool*,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
    bool*       //!< A set of already visited vertices. It is updated by the fucntion.
);

/**
 * @brief
 * Search in a labeled graph. Reachable vertices are returned inside a dequeue sorted in increasing
 * order.
 *
 * @attention
 * The starting set is emptied by the function (it serves as a stack or a queue).
 *
 * @remark
 * Two arrays of Booleans are taken as input. The first one is indexed by the labels. It represents
 * a set of labels which restricts the edges that can be used to those labeled by a label in this set.
 * The second array is indexed by the vertices. It represents a subset of these vertices which is used
 * to restrict the list of reachable vertices to those in the given set. In both cases, if no restriction
 * is needed, a NULL pointer should be given as input.
 *
 * @return
 * The list of reachable vertices sorted in increasing order.
 */
dequeue* lgraph_search(graph_stype, //!< The type of search that has to be used.
    lgraph*,    //!< The graph.
    dequeue*,   //!< The starting set of vertices.
    bool*,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
    bool*       //!< An array indexed by the vertices. Used to restrict the list of reachable vertices.
);

/**
 * @brief
 * Search in a desterministic complete labeled graph. A set of already visited vertices is taken as input.
 * This set is updated by the search.
 *
 * @remark
 * An additional array of Booleans is taken as input. It is indexed by the labels and represents a
 * set of labels. It restricts the edges that can be used to those labeled by a label in this set.
 * If no restriction is needed, a NULL pointer should be given as input.
 *
 * @attention
 * The starting set is emptied by the function (it serves as a stack or a queue).
 */
void dgraph_search_update(graph_stype, //!< The type of search that has to be used.
    dgraph*,    //!< The graph.
    dequeue*,   //!< The starting set of vertices.
    bool*,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
    bool*       //!< A set of already visited vertices. It is updated by the fucntion.
);

/**
 * @brief
 * Search in a desterministic complete labeled graph. Reachable vertices are returned inside a
 * dequeue sorted in increasing order.
 *
 * @attention
 * The starting set is emptied by the function (it serves as a stack or a queue).
 *
 * @remark
 * Two arrays of Booleans are taken as input. The first one is indexed by the labels. It represents
 * a set of labels which restricts the edges that can be used to those labeled by a label in this set.
 * The second array is indexed by the vertices. It represents a subset of these vertices which is used
 * to restrict the list of reachable vertices to those in the given set. In both cases, if no restriction
 * is needed, a NULL pointer should be given as input.
 *
 * @return
 * The list of reachable vertices sorted in increasing order.
 */
dequeue* dgraph_search(graph_stype, //!< The type of search that has to be used.
    dgraph*,    //!< The graph.
    dequeue*,   //!< The starting set of vertices.
    bool*,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
    bool*       //!< An array indexed by the vertices. Used to restrict the list of reachable vertices.
);

/********************************/
/*+ Disjoint merging of graphs +*/
/********************************/

/**
 * @brief
 * Disjoint merge of an arbitrary number of unlabeled graphs.
 *
 * @remark
 * If one of the input graphs is NULL, it is not taken into account. If all of the
 * input graphs are NULL, the function returns NULL.
 *
 * @return
 * The resulting unlabeled graph.
 */
graph* merge_graphs(uint*, //!< An array indexed by the input graphs. It is used to return the index of the first vertex of each graph in the merged graph.
    uint,   //!< The number of graphs given as input.
    ...     //!< The graphs.
);

/**
 * @brief
 * Disjoint merge of an arbitrary number of labeled graphs.
 *
 * @remark
 * If one of the input graphs is NULL, it is not taken into account. If all of the
 * input graphs are NULL, the function returns NULL.
 *
 * @attention
 * The input graphs must have the same number of labels.
 *
 * @return
 * The resulting labeled graph.
 */
lgraph* merge_lgraphs(uint*, //!< An array indexed by the input graphs. It is used to return the index of the first vertex of each graph in the merged graph.
    uint, //!< The number of graphs given as input.
    ... //!< The graphs.
);

/**************************************************/
/*+ Merging graphs with the same set of vertices +*/
/**************************************************/

/**
 * @brief
 * Merging an arbitrary number of graphs using the same set of vertices into a single unlabeled graph.
 *
 * @remark
 * It is possible to merge arbitrary kinds of graphs. The unlabeled inputs must be given first, then the
 * labeled one, then the complete deterministic labeled ones.
 *
 * @attention
 * All graphs must have the same size.
 *
 * @return
 * The resulting unlabeled graph.
 */
graph* ldgraphs_to_graph(uint, //!< Number of input unlabeled graphs.
    uint, //!< Number of input labeled graphs.
    uint, //!< Number of input complete deterministic labeled graphs.
    uint, //!< Total number of input graphs (sum of the three).
    ...   //!< The graphs.
);

/**
 * @brief
 * Merging an arbitrary number of labeled graphs using the same set of vertices into a single labeled graph.
 *
 * @remark
 * It is possible to merge standard labeled graphs with complete deterministic labeled graphs. The former must
 * be given first, then the latter.
 *
 * @attention
 * All graphs must have the same size.
 *
 * @return
 * The resulting labeled graph.
 */
lgraph* ldgraphs_to_lgraph(uint, //!< Number of input labeled graphs.
    uint, //!< Number of input complete deterministic labeled graphs.
    uint, //!< Total number of input graphs (sum of the two).
    ...   //!< The graphs.
);

/**
 * @brief
 * Conversion of a complete deterministic labeled graph into a standard labeled graph.
 *
 * @return
 * The labeled graph.
 */
lgraph* dgraph_to_lgraph(dgraph* //!< The complete deterministic labeled graph.
);

/***************************************/
/*+ Computation of adjacents vertices +*/
/***************************************/

/**
 * @brief
 * Given a labeled graph, a list of vertices in this graph and a label, computed the
 * list of all vertices connected to a vertex in the input list by an edge labeled by
 * the input label.
 *
 * @return
 * The list of all connected vertices sorted in increasing order.
 */
dequeue* lgraph_reachable(lgraph*,  //!< The graph.
    dequeue*, //!< The input list of vertices.
    uint       //!< The label.
);

#endif
