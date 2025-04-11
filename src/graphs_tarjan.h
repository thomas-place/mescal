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
parti* tarjan(graph* //!< The unlabeled graph.
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
parti* ltarjan(lgraph* //!< The labeled graph.
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
parti* dtarjan(dgraph* //!< The complete deterministic labeled graph.
);

/*********************************************/
/* Restriction of algorithm to sub-alphabets */
/*********************************************/


/**
 * @brief
 * Tarjan's algorihtm for labeled graphs. Restricted variant.
 *
 * @remark
 * The classes are sorted in topological order.
 *
 * @return
 * The partition of the graph into strongly connected components.
 */
parti* ltarjan_alph(lgraph*, //!< The labeled graph.
    bool*                 //!< An array of Booleans indexed by the labels. Only the edges labeled by a letter marked true are considered.
);

/**
 * @brief
 * Tarjan's algorihtm for complete deterministic labeled graphs. Restricted variant.
 *
 * @remark
 * The classes are sorted in topological order.
 *
 * @return
 * The partition of the graph into strongly connected components.
 */
parti* dtarjan_alph(dgraph*, //!< The complete deterministic labeled graph.
    bool*                 //!< An array of Booleans indexed by the labels. Only the edges labeled by a letter marked true are considered.
);

#endif // TARJAN_H_
