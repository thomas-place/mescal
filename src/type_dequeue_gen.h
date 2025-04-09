/**
 * @file type_dequeue_gen.h
 * @brief
 * Implementation of generic dequeues which store void pointers.
 */

#ifndef DEQUEUE_GEN_H_
#define DEQUEUE_GEN_H_

 /*  ____                                                                       _       */
 /* |  _ \  ___  __ _ _   _  ___ _   _  ___  ___ _    __ _  ___ _ __   ___ _ __(_) ___  */
 /* | | | |/ _ \/ _` | | | |/ _ \ | | |/ _ \/ __(_)  / _` |/ _ \ '_ \ / _ \ '__| |/ __| */
 /* | |_| |  __/ (_| | |_| |  __/ |_| |  __/\__ \_  | (_| |  __/ | | |  __/ |  | | (__  */
 /* |____/ \___|\__, |\__,_|\___|\__,_|\___||___(_)_ \__, |\___|_| |_|\___|_|  |_|\___| */
 /* (_)_ __ ___  _ |_|| | ___ _ __ ___   ___ _ __ | ||___/_| |_(_) ___  _ __            */
 /* | | '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \           */
 /* | | | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |          */
 /* |_|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|          */
 /*             |_|                                                                     */

#include <stdbool.h>
#include "type_basic.h"

/**********************/
/* Définition du type */
/**********************/

/**
 * @brief
 * Type used to represent a generic dequeue.
 */
typedef struct {
    void** array;    //!< Array of values.
    uint size_array; //!< Size of the array of values.
    uint left;       //!< Index of the leftmost value in the dequeue.
    uint right;      //!< Index following the rightmost value in the dequeue.
    bool empty;      //!< Boolean indicating whether the dequeue is empty.
} dequeue_gen;



/**************/
/* Primitives */
/**************/

/**
 * @brief
 * Creation of an empty dequeue.
 *
 * @return
 * The dequeue.
 */
dequeue_gen* create_dequeue_gen(void);

/**
 * @brief
 * Release of a dequeue.
 */
void delete_dequeue_gen(dequeue_gen* //!< The dequeue.
);

/**
 * @brief
 * Computes the size of a dequeue.
 *
 * @return
 * The size.
 */
uint size_dequeue_gen(dequeue_gen* //!< The dequeue.
);

/**
 * @brief
 * Tests whether a dequeue is empty.
 *
 * @return.
 * A Boolean indicating whether the dequeue is empty.
 */
bool isempty_dequeue_gen(dequeue_gen* //!< The dequeue.
);

/**
 * @brief
 * Empties a dequeue.
 */
void makeempty_dequeue_gen(dequeue_gen* //!< The dequeue.
);

/**
 * @brief
 * Inserts a new value on the left of a dequeue.
 */
void lefins_dequeue_gen(void*, //!< The value.
    dequeue_gen*            //!< The dequeue.
);

/**
 * @brief
 * Inserts a new value on the right of a dequeue.
 */
void rigins_dequeue_gen(void*, //!< The value.
    dequeue_gen*            //!< The dequeue.
);

/**
 * @brief
 * Read a value inside a dequeue without removing it (left-right version).
 *
 * @remark
 * Values are indexed from left to right (the leftmost one has index 0).
 *
 * @return
 * The value.
 */
void* lefread_dequeue_gen(dequeue_gen*, //!< The dequeue.
    uint                             //!< The index of the value.
);

/**
 * @brief
 * Read a value inside a dequeue without removing it (right-left version).
 *
 * @remark
 * Values are indexed from right to left (the rightmost one has index 0).
 *
 * @return
 * The value.
 */
void* rigread_dequeue_gen(dequeue_gen*,//!< The dequeue.
    uint                             //!< The index of the value.
);


/**
 * @brief
 * Removes the leftmost value of a dequeue.
 *
 * @return
 * The removed value.
 */
void* lefpull_dequeue_gen(dequeue_gen* //!< The dequeue.
);

/**
 * @brief
 * Removes the rightmost value of a dequeue.
 *
 * @return
 * The removed value.
 */
void* rigpull_dequeue_gen(dequeue_gen* //!< The dequeue.
);



#endif 
