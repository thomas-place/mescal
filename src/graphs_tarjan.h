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


#endif // TARJAN_H_
