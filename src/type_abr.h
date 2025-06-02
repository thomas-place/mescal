/**
 * @file type_abr.h
 * @brief
 * Implementation of AVLs for representing sets of objects.
 *
 * @remark
 * Two versions are implemented. The first one is generic and stores void pointers.
 * The second one is lighter and stores unsigned integers only.
 */

#ifndef ABR_H
#define ABR_H


 /*     ___     ___          */
 /*    / \ \   / / |    ___  */
 /*   / _ \ \ / /| |   / __| */
 /*  / ___ \ V / | |___\__ \ */
 /* /_/   \_\_/  |_____|___/ */

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "type_basic.h"
#include "type_dequeue.h"
#include "alloc.h"

/*******************/
/* Generic version */
/*******************/


/**
 * @brief
 * Type used to represent a single node in a generic AVL.
 */
typedef struct avlnode {
    int height;                 //!< Height of the AVL rooted in this node.
    void* value;                //!< Value of the node.
    struct avlnode* left;       //!< Pointer to left child (NULL if no left child).
    struct avlnode* right;      //!< Pointer to right child (NULL if no right child).
} avlnode;



/**
 * @brief
 * Searches for a value inside an generic AVL tree.
 *
 * @return
 * A pointer to the node containing the searched value or NULL if the
 * value wad not found.
 */
avlnode* avl_search(void*,      //!< The searched value.
    avlnode*,      //!< Root node of the AVL tree.
    int (*f)(void*, void*)    //!< Comparison function.
);

/**
 * @brief
 * Insertion of a value inside a generic AVL tree.
 *
 * @return
 * A pointer to the new root node of the AVL tree.
 */
avlnode* avl_insert(void*,   //!< The value that needs to be inserted.
    avlnode*,  //!< Root node of the AVL tree.
    int (*f)(void*, void*), //!< Comparison function.
    void (*d)(void*)       //!< Function used to release the values when they are already in the tree (NULL if no release is desired).
);

/**
 * @brief
 * Computes the size of a generic AVL tree.
 *
 * @return
 * The size of the AVL tree.
 */
int getsize_avl(avlnode* //!< Root node of the AVL tree.
);


/**
 * @brief
 * Release of a generic AVL tree.
 *
 * @remark
 * The input function is used to release the values contained
 * in the AVL tree.
 */
void avl_free_strong(avlnode*, //!< Root node of the AVL tree.
    void (*f)(void*)        //!< Function used to release the values.
);

int avl_height(avlnode* //!< Root node of the AVL tree.
);


/***********************************************/
/* Light version specific to unsigned integers */
/***********************************************/


/**
 * @brief
 * Type used to represent a single node in a light AVL.
 */
typedef struct uint_avlnode {
    int height;                      //!< Height of the AVL rooted in this node.
    uint value;                      //!< Value of the node.
    struct uint_avlnode* left;       //!< Pointer to left child (NULL if no left child).
    struct uint_avlnode* right;      //!< Pointer to right child (NULL if no right child).
} uint_avlnode;

/**
 * @brief
 * Searches for a value inside an light AVL tree.
 *
 * @return
 * A pointer to the node containing the searched value or NULL if the
 * value wad not found.
 */
uint_avlnode* uint_avl_search(uint, //!< The searched value.
    uint_avlnode*                //!< Root node of the AVL tree.
);

/**
 * @brief
 * Insertion of a value inside a light AVL tree.
 *
 * @return
 * A pointer to the new root node of the AVL tree.
 */
uint_avlnode* uint_avl_insert(uint, //!< The value that needs to be inserted.
    uint_avlnode*                //!< Root node of the AVL tree.
);

/**
 * @brief
 * Computes the size of a light AVL tree.
 *
 * @return
 * The size of the AVL tree.
 */
int getsize_uint_avl(uint_avlnode* //!< Root node of the AVL tree.
);

/**
 * @brief
 * Extracts all values in a light AVL tree and stores on the right of the input
 * dequeue in increasing order.
 */
void uint_avl_to_dequeue(uint_avlnode*, //!< Root node of the AVL tree.
    dequeue*                         //!< The dequeue.
);

#endif
