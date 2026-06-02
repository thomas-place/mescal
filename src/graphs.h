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

#include "type_basic.h"
#include "type_boolarray.h"
#include "type_dequeue.h"
#include "type_partitions.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief
 * Type used to represent a directed unlabeled graph.
 *
 * @remark
 * For each vertex `q`, the vertices that are connected to `q` by an edge are stored
 * in the array `storage` between the indices `intervals[q]` (included) and `intervals[q+1]`
 * (excluded). This list must be sorted in increasing order.
 */
typedef struct
{
    uint size;       //!< Number of vertices.
    uint size_edges; //!< Number of edges.
    uint *intervals; //!< The intervals to locate the edges for a given vertex (one dimension array of size size + 1).
    uint *storage;   //!< The storage of the edges (one dimension array of size size_edges).
} graph;

#define GSTART(g, q) (g->intervals[(q)])
#define GEND(g, q) (g->intervals[(q) + 1])

/**
 * @brief
 * Type used to represent a directed labeled graph.
 *
 * @remark
 * For each vertex `q` and each letter `a`, the vertices that are connected to `q` by an edge
 * labeled by `a` are stored in the array `storage` between the indices `intervals[q * size_alpha + a]`
 * (included) and `intervals[q * size_alpha + a + 1]` (excluded). This list must be sorted in increasing order.
 */
typedef struct
{
    uint size_graph; //!< Number of vertices.
    uint size_alpha; //!< Number of labels.
    uint size_edges; //!< Number of edges.
    uint *intervals; //!< The intervals to locate the edges for a given vertex and a given label (one dimension array of size size_graph * size_alpha + 1).
    uint *storage;   //!< The storage of the edges (one dimension array of size size_edges).
} lgraph;

#define LGSTART(g, q, a) (g->intervals[(q) * g->size_alpha + (a)])
#define LGEND(g, q, a) (g->intervals[(q) * g->size_alpha + (a) + 1])

/**
 * @brief
 * Type used to represent a deterministic directed labeled graph.
 *
 * @remark
 * The set of edges is represented by the arrays. For each vertex "q" and each letter "a"
 * the cell "edge[q][a]" (stored in storage) contains the unique vertex connected to "q"
 * by an edge labeled by "a" (or UINT_MAX if no such edge exists).
 */
typedef struct
{
    uint size_graph; //!< Number of vertices.
    uint size_alpha; //!< Number of labels.
    uint size_edges; //!< Number of edges.
    uint **edges;    //!< The edges: two dimensions array of size "size_graph * size_alpha" (actually contains pointers to the storage).
    uint *storage;   //!< The storage of the edges (one dimension array of size size_graph * size_alpha).
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
graph *create_graph_noedges(uint size //!< Number of vertices.
);

/**
 * @brief
 * Releases a directed unlabeled graph.
 */
void delete_graph(graph *g //<! The graph that needs to be freed.
);

/**
 * @brief
 * Makes a copy of a directed graph.
 *
 * @return
 * The copy.
 */
graph *copy_graph(graph *g //!< The graph.
);

/**
 * @brief
 * Creates a directed graph without edges.
 *
 * @return
 * The created graph.
 */
lgraph *create_lgraph_noedges(uint size_graph, //!< Number of vertices.
                              uint size_alpha  //!< Number of labels.
);

/**
 * @brief
 * Releases a directed graph.
 */
void delete_lgraph(lgraph *g //<! The graph that needs to be freed.
);

/**
 * @brief
 * Makes a copy of a labeled graph.
 *
 * @return
 * The copy.
 */
lgraph *copy_lgraph(lgraph *g //!< The graph.
);

/**
 * @brief
 * Creates a deterministic directed graph without edges.
 *
 * @return
 * The created graph.
 */
dgraph *create_dgraph_noedges(uint size_graph, //!< Number of vertices.
                              uint size_alpha  //!< Number of labels.
);

/**
 * @brief
 * Release of a deterministic directed graph.
 */
void delete_dgraph(dgraph *g //<! The graph that needs to be freed.
);

/**
 * @brief
 * Makes a copy of a deterministic directed graph.
 *
 * @return
 * The copy.
 */
dgraph *copy_dgraph(dgraph *g //!< The graph.
);

/*******************/
/* Basic functions */
/*******************/

/**
 * @brief
 * Converts a deterministic directed labeled graph into a non-deterministic one.
 */
lgraph *dgraph_to_lgraph(dgraph *g //!< The graph.
);

/**************/
/*+ Products +*/
/**************/

/**
 * @brief
 * Computes the direct product of two deterministic directed labeled graphs.
 *
 * @remark
 * The vertex (q1,q2) is is encoded as the integer q1 * g2->size_graph + q2 in the resulting graph.
 *
 *
 * @attention
 * The two graphs must have the same alphabet size.
 *
 * @return
 * The resulting graph.
 */
dgraph *dgraph_direct_product(dgraph *g1, //!< The first graph.
                              dgraph *g2  //!< The second graph.
);

/***********/
/* Mirrors */
/***********/

/**
 * @brief
 * Computes the mirror of a directed unlabeled graph.
 *
 * @return
 * The mirror of the graph.
 */
graph *graph_mirror(graph *g //<! The graph.
);

/**
 * @brief
 * Computes the mirror of a directed labeled graph.
 *
 * @return
 * The mirror of the graph.
 */
lgraph *lgraph_mirror(lgraph *g //<! The graph.
);

/**
 * @brief
 * Computes the mirror of a deterministic directed labeled graph.
 *
 * @return
 * The mirror of the graph (an lgraph since the mirror need not be deterministic)
 */
lgraph *dgraph_mirror(dgraph *g //<! The graph.
);

/*************/
/*+ Merging +*/
/*************/

graph *dgraph_merge(dgraph *g,
                    parti *p);
/************/
/* Searches */
/************/

/**
 * @brief
 * Names for the two available algorithms for searches.
 */
typedef enum
{
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
                         graph *,     //!< The graph.
                         dequeue *,   //!< The starting set of vertices.
                         bool *       //!< A set of already visited vertices. It is updated by the fucntion.
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
dequeue *graph_search(graph_stype, //!< The type of search that has to be used.
                      graph *,     //!< The graph.
                      dequeue *,   //!< The starting set of vertices.
                      bool *       //!< An array indexed by the vertices. Used to restrict the list of reachable vertices.
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
                          lgraph *,    //!< The graph.
                          dequeue *,   //!< The starting set of vertices.
                          bool *,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
                          bool *       //!< A set of already visited vertices. It is updated by the fucntion.
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
dequeue *lgraph_search(graph_stype, //!< The type of search that has to be used.
                       lgraph *,    //!< The graph.
                       dequeue *,   //!< The starting set of vertices.
                       bool *,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
                       bool *       //!< An array indexed by the vertices. Used to restrict the list of reachable vertices.
);

/**
 * @brief
 * Search in a desterministic labeled graph. A set of already visited vertices is taken as input.
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
                          dgraph *,    //!< The graph.
                          dequeue *,   //!< The starting set of vertices.
                          bool *,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
                          bool *       //!< A set of already visited vertices. It is updated by the fucntion.
);

/**
 * @brief
 * Search in a desterministic labeled graph. Reachable vertices are returned inside a
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
dequeue *dgraph_search(graph_stype, //!< The type of search that has to be used.
                       dgraph *,    //!< The graph.
                       dequeue *,   //!< The starting set of vertices.
                       bool *,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
                       bool *       //!< An array indexed by the vertices. Used to restrict the list of reachable vertices.
);

/**
 * @brief
 * Search in two deterministic labeled graphs (same set of vertices, same alphabet). A set of already visited vertices is taken as input.
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
void twin_dgraph_search_update(graph_stype, //!< The type of search that has to be used.
                               dgraph *,    //!< The first graph.
                               dgraph *,    //!< The second graph.
                               dequeue *,   //!< The starting set of vertices.
                               bool *,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
                               bool *       //!< A set of already visited vertices. It is updated by the fucntion.
);

/**
 * @brief
 * Search in two deterministic labeled graphs (same set of vertices, same alphabet). Reachable vertices are returned inside a
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
dequeue *twin_dgraph_search(graph_stype, //!< The type of search that has to be used.
                            dgraph *,    //!< The first graph.
                            dgraph *,    //!< The second graph.
                            dequeue *,   //!< The starting set of vertices.
                            bool *,      //!< An array indexed by the labels. Used to restrict the edges that can be used.
                            bool *       //!< An array indexed by the vertices. Used to restrict the list of reachable vertices.
);

/**
 * @brief
 * Computes a path from a given strating vertex to all other reachable vertices in a directed unlabeled graph.
 *
 * @remark
 * The paths are computed using a breadth-first search.
 *
 * @return
 * A graph containing the back edges of the paths.
 */
dgraph *dgraph_paths(dgraph *G, //!< The graph.
                     uint start //!< The starting vertex.
);

/***************************************/
/*+ Computation of adjacents vertices +*/
/***************************************/

/**
 * @brief
 * Given a labeled graph, a list of vertices in this graph and a label, computes the
 * list of all vertices connected to a vertex in the input list by an edge labeled by
 * the input label.
 */
void lgraph_reachable(lgraph *g, //!< The graph.
                      uint *in,  //!< The input list of vertices (represented by a Boolean array).
                      uint *out, //!< The output list of vertices (represented by a Boolean array).
                      uint a     //!< The label.
);

void lgraph_reachable_eps(lgraph *G, uint *in, uint *out, uint a, uint ieps);

/**********************************/
/*+ Paths computation in dgraphs +*/
/**********************************/

/**
 * @brief
 * Tests the existence of a path between two vertices in a deterministic directed labeled graph.
 *
 * @remark
 * If such a path exists, the word corresponding to this path is returned via the pointer "word"
 * (if not NULL) as an array of labels ending with UINT_MAX.
 *
 * @remark
 * If the Boolean "strict" is set to true, then the path must be strict (i.e., it cannot be of length 0
 * even when the starting and ending states are the same).
 *
 * @return
 * A Boolean indicating whether a path exists between the two given vertices.
 */
bool dgraph_exists_path(dgraph *g,   //!< The graph
                        uint s,      //!< Starting state in the graph.
                        uint e,      //!< Ending state in the graph.
                        bool strict, //!< A Boolean indicating whether the path must be strict.
                        uint **word  //!< The word that is the intersection path (NULL if not needed).
);

/**
 * @brief
 * Tests the existence of a path between two vertices in a deterministic directed labeled graph. Extended
 * variant which can restrict the labels that can be used in the path.
 *
 * @remark
 * If such a path exists, the word corresponding to this path is returned via the pointer "word"
 * (if not NULL) as an array of labels ending with UINT_MAX.
 *
 * @remark
 * If the Boolean "strict" is set to true, then the path must be strict (i.e., it cannot be of length 0
 * even when the starting and ending states are the same).
 *
 * @remark
 * The array "alpha" is indexed by the labels and indicates which labels can be used in the path. If
 * no restriction is needed, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether a path exists between the two given vertices.
 */
bool dgraph_exists_path_alpha(dgraph *g,   //!< The graph
                              uint s,      //!< Starting state in the graph.
                              uint e,      //!< Ending state in the graph.
                              bool strict, //!< A Boolean indicating whether the path must be strict.
                              bool *alpha, //!< The alphabet to use for the path (NULL if not needed).
                              uint **word  //!< The word that is the intersection path (NULL if not needed).
);

/**
 * @brief
 * Tests the existence of a path from a given starting vertex to a letter in a deterministic directed labeled graph.
 *
 * @details
 * The functions looks path from the starting vertex "s" to any vertex "q" such that there exists an edge labeled by
 * "b" from "q".
 *
 * @remark
 * If such a path exists, the word corresponding to this path is returned via the pointer "word"
 * (if not NULL) as an array of labels ending with UINT_MAX.
 *
 * @remark
 * The array "alpha" is indexed by the labels and indicates which labels can be used in the path. If
 * no restriction is needed, a NULL pointer should be given.
 *
 * @return
 * The ending state reached by following the path to the letter (UINT_MAX if no such path exists).
 */
uint dgraph_exists_path_to_letter(dgraph *g,   //!< The graph
                                  uint s,      //!< Starting state in the graph.
                                  uint b,      //!< Letter searched.
                                  bool *alpha, //!< The alphabet to use for the path (NULL if not needed).
                                  uint **word  //!< The word that is the intersection path (NULL if not needed).
);

/**
 * @brief
 * Tests the existence of a loop on a given state in a deterministic directed labeled graph. This loop
 * must contain all letters (and only these letters) in the given alphabet.
 *
 * @remark
 * If such a loop exists, the word corresponding to this loop is returned via the pointer "word" (if not NULL)
 * as an array of labels ending with UINT_MAX.
 *
 * @return
 * True if such a loop exists, false otherwise.
 */
bool dgraph_getloop_alpha(dgraph *g,   //!< The dgraph to extract the word from.
                          parti *sccs, //<! The partition of the dgraph into strongly connected components (in order to stay within the SCC of the state).
                          bool *alph,  //!< The alphabet to use for the word (an array indexed by the letters).
                          uint s,      //!< The state to loop on.
                          uint **word  //!< The word that loops on the given state (NULL if no such word exists).
);

/****************************/
/*+ Edge lists computation +*/
/****************************/

/**
 * @brief
 * Type used to represent an edge in a directed unlabeled graph.
 */
typedef struct
{
    uint from; //!< The source vertex.
    uint to;   //!< The destination vertex.
} edge_pair;

/**
 * @brief
 * Comparison function to sort edges in an edge list of a directed unlabeled graph.
 *
 * @return
 * - A negative integer if e1 < e2
 * - Zero if e1 == e2
 * - A positive integer if e1 > e2
 */
int edge_pair_sort(const void *e1, //!< First edge.
                   const void *e2  //!< Second edge.
);

/**
 * @brief
 * Computes the edge list of a directed graph.
 *
 * @return
 * The edge list.
 */
edge_pair *graph_to_edge_list(graph *g, //!< The graph.
                              bool inv  //!< Whether to compute the inverse edge list.
);

/**
 * @brief
 * Constructs a directed labeled graph from an edge list.
 *
 * @return
 * The resulting labeled graph.
 */
graph *edge_list_to_graph(edge_pair *elist, //!< The edge list.
                          uint elist_size,  //!< The size of the edge list (must be a multiple of 3).
                          uint num_vertices //!< The number of vertices in the graph.
);

/**
 * @brief
 * Dynamic array used to store edges in a directed unlabeled graph.
 */
typedef struct
{
    edge_pair *array; //!< The array of edges.
    uint size;        //!< The size of the array.
    uint capacity;    //!< The capacity of the array.
} dyn_edge_pair;

/**
 * @brief
 * Creates a dynamic array to store edges in a directed unlabeled graph.
 */
dyn_edge_pair *create_dyn_edge_pair(void);

/**
 * @brief
 * Releases a dynamic array used to store edges in a directed unlabeled graph.
 */
void delete_dyn_edge_pair(dyn_edge_pair *dep //!< The dynamic array to be freed.
);

/**
 * @brief
 * Adds an edge to a dynamic array used to store edges in a directed unlabeled graph.
 */
void dyn_edge_pair_add(dyn_edge_pair *dep, //<! The dynamic array.
                       uint from,          //<! The source vertex.
                       uint to             //<! The destination vertex.
);

/**
 * @brief
 * Type used to represent an edge in a directed labeled graph.
 */
typedef struct
{
    uint from; //!< The source vertex.
    uint lab;  //!< The label.
    uint to;   //!< The destination vertex.
} edge_triple;

/**
 * @brief
 * Comparison function to sort edges in an edge list of a directed labeled graph.
 *
 * @return
 * - A negative integer if e1 < e2
 * - Zero if e1 == e2
 * - A positive integer if e1 > e2
 */
int edge_triple_sort(const void *e1, //!< First edge.
                     const void *e2  //!< Second edge.
);

/**
 * @brief
 * Computes the edge list of a directed deterministic labeled graph.
 *
 * @return
 * The edge list.
 */
edge_triple *dgraph_to_edge_list(dgraph *g, //!< The graph.
                                 bool inv   //!< Whether to compute the inverse edge list.
);

/**
 * @brief
 * Computes the edge list of a directed labeled graph.
 *
 * @return
 * The edge list.
 */
edge_triple *lgraph_to_edge_list(lgraph *g, //!< The graph.
                                 bool inv   //!< Whether to compute the inverse edge list.
);

/**
 * @brief
 * Constructs a directed labeled graph from an edge list.

 * @attention
 * The size of the edge list must be a multiple of 3.
 *
 * @return
 * The resulting labeled graph.
 */
lgraph *edge_list_to_lgraph(edge_triple *elist, //!< The edge list.
                            uint elist_size,    //!< The size of the edge list (must be a multiple of 3).
                            uint num_vertices,  //!< The number of vertices.
                            uint num_labels     //!< The number of labels.
);

/**
 * @brief
 * Dynamic array used to store edges in a directed labeled graph.
 */
typedef struct
{
    edge_triple *array; //!< The array of edges.
    uint size;          //!< The size of the array.
    uint capacity;      //!< The capacity of the array.
} dyn_edge_triple;

/**
 * @brief
 * Creates a dynamic array to store edges in a directed labeled graph.
 */
dyn_edge_triple *create_dyn_edge_triple(void);

/**
 * @brief
 * Releases a dynamic array used to store edges in a directed labeled graph.
 */
void delete_dyn_edge_triple(dyn_edge_triple *det //!< The dynamic array to be freed.
);

/**
 * @brief
 * Adds an edge to a dynamic array used to store edges in a directed labeled graph.
 */
void dyn_edge_triple_add(dyn_edge_triple *det, //<! The dynamic array.
                         uint from,            //!< The source vertex.
                         uint lab,             //!< The label.
                         uint to               //!< The destination vertex.
);

#endif
