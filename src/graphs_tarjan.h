/**
 * @file graphs_tarjan.h
 * @brief
 * Implementation of Tarjan's algorithm.
 */
#ifndef TARJAN_H_
#define TARJAN_H_

 /*   ____                 _             _____          _             _      */
 /*  / ___|_ __ __ _ _ __ | |__  ___ _  |_   _|_ _ _ __(_) __ _ _ __ ( )___  */
 /* | |  _| '__/ _` | '_ \| '_ \/ __(_)   | |/ _` | '__| |/ _` | '_ \|// __| */
 /* | |_| | | | (_| | |_) | | | \__ \_    | | (_| | |  | | (_| | | | | \__ \ */
 /*  \____|_|_ \__,_| .__/|_| |_|___(_)   |_|\__,_|_| _/ |\__,_|_| |_| |___/ */
 /*    / \  | | __ _|_|__  _ __(_) |_| |__  _ __ ___ |__/                    */
 /*   / _ \ | |/ _` |/ _ \| '__| | __| '_ \| '_ ` _ \                        */
 /*  / ___ \| | (_| | (_) | |  | | |_| | | | | | | | |                       */
 /* /_/   \_\_|\__, |\___/|_|  |_|\__|_| |_|_| |_| |_|                       */
 /*            |___/                                                         */

//#define DEBUG_TARJAN


#include "type_partitions.h"
#include "graphs.h"
#include <stdbool.h>

/***********************/
/* Classical algorithm */
/***********************/

/**
 * @brief
 * Tarjan's algorihtm for unlabeled graphs.
 *
 * @remark
 * The classes are sorted in topological order.
 *
 * @return
 * The partition of the graph into strongly connected components.
 */
parti* tarjan(graph* g //!< The unlabeled graph.
);

/**
 * @brief
 * Tarjan's algorihtm for labeled graphs.
 *
 * @remark
 * The classes are sorted in topological order.
 *
 * @return
 * The partition of the graph into strongly connected components.
 */
parti* ltarjan(lgraph* g, //!< The labeled graph.
    bool* alph //!< An array of Booleans indexed by the labels. Only the edges labeled by a letter marked true are considered. NULL means all labels are considered.
);

/**
 * @brief
 * Tarjan's algorihtm for complete deterministic labeled graphs.
 *
 * @remark
 * The classes are sorted in topological order.
 *
 * @return
 * The partition of the graph into strongly connected components.
 */
parti* dtarjan(dgraph* g, //!< The complete deterministic labeled graph.
    bool* alph, //!< An array of Booleans indexed by the labels. Only the edges labeled by a letter marked true are considered. NULL means all labels are considered.
    bool ismor      //!< If true, the algorithm is run on a Cayley graph (all vertices are reachable from ONE = 0).
);


/**
 * @brief
 * Tarjan's algorihtm for two complete deterministic labeled graphs sharing the same state set and alphabet.
 * Computation is done in the merge of the two graphs.
 *
 * @remark
 * The classes are sorted in topological order.
 *
 * @return
 * The partition of the graph into strongly connected components.
 */
parti* dualdtarjan(dgraph* g1, //!< The first complete deterministic labeled graph.
    dgraph* g2, //!< The second complete deterministic labeled graph.
    bool* alph, //!< An array of Booleans indexed by the labels. Only the edges labeled by a letter marked true are considered. NULL means all labels are considered.
    bool ismor //!< If true, the algorithm is run on Cayley graphs (all vertices are reachable from ONE = 0).
);


/**************************/
/*+ Computations on SCCS +*/
/**************************/


/**
 * @brief
 * Extracts a single SCC of a dgraph and makes it a new dgraph.
 *
 * @return
 * A pointer to the extracted dgraph.
 */
dgraph* dgraph_extract(dgraph* g, //!< The dgraph to extract the SCC from.
    parti* P, //!< The partition of the dgraph.
    uint* inv, //!< The inverse mapping of the partition.
    uint j //!< The index of the SCC to extract.
);

/**
 * @brief
 * Computes the alphabet of a strongly connected component of a labeled graph.
 *
 * @remark
 * The alphabet is computed as an array of Booleans indexed by the letters. This
 * array muste be allocated by the caller and must have a size equal to the number
 * of labels in the graph.
 */
void dgraph_compute_alph_scc(dgraph* g, //!< The graph.
    parti* sccs, //!< The partition of the graph into strongly connected components.
    uint scc, //!< The index of the SCC.
    bool* alph //!< The array used to store the computed alphabet (an array indexed by the letters).
);


/**
 * @brief
 * Computes the alphabet of a strongly connected component of a labeled graph.
 *
 * @remark
 * The alphabet is computed as an array of Booleans indexed by the letters. If a
 * letter is in the alphabet, the corresponding cell is set to true.
 *
 * @return
 * True if the alphabet is not empty, false otherwise.
 */
bool dgraph_common_alph_loop(dgraph* g, //!< The graph.
    parti* sccs, //!< The partition of the graph into strongly connected components.
    uint* inv_sccs, //!< The inverse mapping of the partition.
    uint q1, //!< The index of the state in the first SCC.
    uint q2, //!< The index of the state in the second SCC.
    bool* alph //!< The alphabet of the SCC (an array indexed by the letters).
);


/**
 * @brief
 * Modifies a dgraph by discarding all edges that are not internal to an SCC.
 *
 * @remark
 * Discarded edges are give the destination value UINT_MAX.
 */
void dgraph_discard_nonscc_edges(dgraph* g, //!< The dgraph to modify.
    parti* sccs //!< The partition of the dgraph into sccs.
);

/**
 * @brief
 * Copies a dgraph while discarding all edges that go from a state to a different SCC.
 *
 * @return
 * A pointer to the resulting dgraph.
 */
dgraph* dgraph_copy_discard_nonscc_edges(dgraph* g, //!< The dgraph to copy.
    parti* sccs //!< The SCCs of the dgraph.
);


/**
 * @brief
 * Tests if there exists a non-trivial loop on a given state q of a dgraph.
 *
 * @remark
 * This function checks if the scc of q contains at least two states or if there is an edge from
 * q to itself labeled by a letter in the alphabet.
 *
 * @return
 * A Boolean indicating whether there exists a non-trivial loop on the state q.
 */
bool dgraph_ntrivial_loop(dgraph* g, //!< The dgraph to test.
    parti* sccs, //!< The SCCs of the dgraph.
    uint q //!< The state q.
);



#endif // TARJAN_H_
