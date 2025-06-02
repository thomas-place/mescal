/**
 * @file type_dequeue.h
 * @brief
 * Implementation of dequeues of unsigned integers.
 */
#ifndef dequeue_H
#define dequeue_H

 /*  ____                                         */
 /* |  _ \  ___  __ _ _   _  ___ _   _  ___  ___  */
 /* | | | |/ _ \/ _` | | | |/ _ \ | | |/ _ \/ __| */
 /* | |_| |  __/ (_| | |_| |  __/ |_| |  __/\__ \ */
 /* |____/ \___|\__, |\__,_|\___|\__,_|\___||___/ */
 /*                |_|                            */

#include <stdbool.h>
#include <stdio.h>
#include "type_basic.h"



/**
 * @brief
 * Type used to represent a dequeue of unsigned integers.
 */
typedef struct {
  uint* array;     //!< Array of values.
  uint size_array; //!< Size of the array of values.
  uint left;       //!< Index of the leftmost value in the dequeue.
  uint right;      //!< Index following the rightmost value in the dequeue.
  bool empty;      //!< Boolean indicating whether the dequeue is empty.
} dequeue;

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
dequeue* create_dequeue(void);

/**
 * @brief
 * Release of a dequeue.
 */
void delete_dequeue(dequeue*  //!< The dequeue.
);

/**
 * @brief
 * Computes the size of a dequeue.
 *
 * @return
 * The size.
 */
uint size_dequeue(dequeue*  //!< The dequeue.
);

/**
 * @brief
 * Tests whether a dequeue is empty.
 *
 * @return.
 * A Boolean indicating whether the dequeue is empty.
 */
bool isempty_dequeue(dequeue*  //!< The dequeue.
);

/**
 * @brief
 * Empties a dequeue.
 */
void makeempty_dequeue(dequeue*  //!< The dequeue.
);

/**
 * @brief
 * Inserts a new value on the left of a dequeue.
 */
void lefins_dequeue(uint,  //!< The value.
  dequeue*                 //!< The dequeue.
);

/**
 * @brief
 * Inserts a new value on the right of a dequeue.
 */
void rigins_dequeue(uint,  //!< The value.
  dequeue*                 //!< The dequeue.
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
uint lefread_dequeue(dequeue*,  //!< The dequeue.
  uint                          //!< The index of the value.
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
uint rigread_dequeue(dequeue*,  //!< The dequeue.
  uint                          //!< The index of the value.
);

/**
 * @brief
 * Removes the leftmost value of a dequeue.
 *
 * @return
 * The removed value.
 */
uint lefpull_dequeue(dequeue*  //!< The dequeue.
);

/**
 * @brief
 * Removes the rightmost value of a dequeue.
 *
 * @return
 * The removed value.
 */
uint rigpull_dequeue(dequeue*  //!< The dequeue.
);

/**
 * @brief
 * Copies all elements in a dequeue to the right of another dequeue. Shifts all values by an
 * input number.
 */
void copy_dequeue_right(dequeue*, //!< The dequeue inside which the values are copied.
  dequeue*,                       //!< The dequeue being copied.
  uint                            //!< The shift to apply.
);

/**
 * @brief
 * Tests if a value occurs inside a dequeue.
 *
 * @return
 * A Boolean indicating whether the value occurs.
 */
bool mem_dequeue(uint,  //!< The value.
  dequeue*              //!< The dequeue.
);

/**
 * @brief
 * Displays a dequeue on the standard output.
 */
void print_dequeue(dequeue*  //!< The dequeue.
);

/**
 * @brief
 * Sorts a dequeue in increasing order and keeps only a single copy
 *  of each occuring value.
 */
void sort_dequeue_norepeat(dequeue*  //!< The dequeue.
);

/************************************************************/
/* Function specific to sorted dequeues with no repetitions */
/************************************************************/

/**
 * @brief
 * Tests if a value occurs inside a sorted dequeue with no repetition.
 *
 * @return
 * A Boolean indicating whether the value occurs.
 */
bool mem_dequeue_sorted(uint,  //!< The value.
  dequeue*,  //!< The dequeue.
  uint*                      //!< Returns the index at which the value occurs (from the left).
);


/**
 * @brief
 * Merge of two sorted dequeues with no repetitions.
 *
 * @remark
 * The first dequeue is modified: it contains the merge.
 *
 * @attention
 * The merge is sorted with no repetition.
 */
void merge_sorted_dequeue(dequeue*,  //!< The first dequeue (modified)
  dequeue*                           //!< The second dequeue.
);

/**
 * @brief
 * Intersection of two sorted dequeues with no repetitions.
 *
 * @return
 * A new sorted dequeue with no repetitions corresponding to the intersection.
 */
dequeue* make_inter_sorted_dequeue(dequeue*, //!< The first dequeue.
  dequeue*                                   //!< The second dequeue.
);

dequeue* make_inter_sorted_dequeue_array(dequeue* p, uint* arr, uint size);

/**
 * @brief
 * Tests if two sorted dequeues with no repetitions have a nonempty intersection.
 *
 * @return
 * A Boolean indicating whether the two sorted dequeues with no repetitions have a nonempty intersection.
 */
bool intersec_dequeue(dequeue*,  //!< The first dequeue.
  dequeue*                       //!< The second dequeue.
);


/**
 * @brief
 * Inserts a new value inside a sorted dequeue with no repetitions.
 *
 * @return
 * The dequeue remains sorted with no repetition.
 */
void insert_dequeue(dequeue*, //!< The dequeue.
  uint                        //!< The value.
);

/**
 * @brief
 * Takes two dequeues sorted in increasing order with no repetition as input such that the
 * first one is a subset of the second one. Computes the list of indices at which elements
 * of the first dequeue occur in the second one.
 *
 * @return
 * The list of indices.
 */
dequeue* dequeue_to_indices(dequeue*, //!< The first dequeue (included in the second one).
  dequeue*                         //!< The second dequeue.
);


/**
 * @brief
 * Merges all dequeues in the array (all of them sorted with no repetitions) into a single
 * dequeue (which is assumed to be empty).
 */
void merge_array_sorted_dequeue(dequeue*, //!< The dequeue inside which the merge is done.
  dequeue**,                     //!< The array containing the dequques to merge
  uint                                 //!< The size of the array.
);
#endif // dequeue_H_
