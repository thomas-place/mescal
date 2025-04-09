/**
 * @file type_binheap.h
 * @brief
 * Implementation of generic binary heaps.
 */
#ifndef BINHEAP_H_
#define BINHEAP_H_

 /*  ____  _                          _   _                       */
 /* | __ )(_)_ __   __ _ _ __ _   _  | | | | ___  __ _ _ __  ___  */
 /* |  _ \| | '_ \ / _` | '__| | | | | |_| |/ _ \/ _` | '_ \/ __| */
 /* | |_) | | | | | (_| | |  | |_| | |  _  |  __/ (_| | |_) \__ \ */
 /* |____/|_|_| |_|\__,_|_|   \__, | |_| |_|\___|\__,_| .__/|___/ */
 /*                           |___/                   |_|         */

#include <stdio.h>
#include <stdbool.h>
#include "alloc.h"
#include "type_basic.h"


/**
 * @brief
 * Type used to represent a binary heap.
 */
typedef struct {
    void** array;               //!< Array which stores all values inside the heap.
    uint size_array;            //!< Size of the array.
    uint size_heap;             //!< Size of the heap (number of values which are currently stored).
    int (*fc)(void*, void*);    //!< Comparison function utilized by the heap.
} binheap;

/***********************/
/* Auxiliary functions */
/***********************/

/**
 * @brief
 * Comparison function for integers.
 *
 * @return
 * The comparison
 */
int fcmp_int(void* x, void* y);

/**
 * @brief
 * Comparison function for usigned integers.
 *
 * @return
 * The comparison.
 */
int fcmp_uint(void* x, void* y);


/************************/
/* Fonctions primitives */
/************************/

/**
 * @brief
 * Creation of an empty binary heap.
 *
 * @return
 * The binary heap.
 */
binheap* create_binheap(int (*) (void*, void*) //!< The comparison function.
);

/**
 * @brief
 * Release of a binary heap.
 *
 * @attention
 * The values inside the heap are not released themselves.
 */
void delete_binheap(binheap* //!< The binary heap.
);

/**
 * @brief
 * Tests if a binary heap is empty.
 *
 * @return
 * A Boolean indicating if the binary heap is empty.
 */
bool isempty_binheap(binheap* //!< The binary heap.
);

/**
 * @brief
 * Computes the size of of a binary heap.
 *
 * @return
 * The size of the binary heap.
 */
int getsize_binheap(binheap* //!< The binary heap.
);

/**
 * @brief
 * Inserts a new value inside a binary heap.
 */
void push_binheap(binheap*, //!< The binary heap.
    void*                //!< The value.
);

/**
 * @brief
 * Computes the minimum value of a binary heap without removing it.
 *
 * @return
 * The minimum value.
 */
void* peekmin_binheap(binheap* //!< The binary heap.
);

/**
 * @brief
 * Computes the minimum value of a binary heap and removes it.
 *
 * @return
 * The minimum value.
 */
void* popmin_binheap(binheap* //!< The binary heap.
);

#endif
