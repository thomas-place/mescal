/**
 * @file type_dlist.h
 * @brief
 * Implementation of doubly linked lists.
 */

#ifndef DLIST_H_
#define DLIST_H_

 /*  ____              _     _         _ _       _            _   _ _     _       */
 /* |  _ \  ___  _   _| |__ | |_   _  | (_)_ __ | | _____  __| | | (_)___| |_ ___ */
 /* | | | |/ _ \| | | | '_ \| | | | | | | | '_ \| |/ / _ \/ _` | | | / __| __/__| */
 /* | |_| | (_) | |_| | |_) | | |_| | | | | | | |   <  __/ (_| | | | \__ \ |_\__\ */
 /* |____/ \___/ \__,_|_.__/|_|\__, | |_|_|_| |_|_|\_\___|\__,_| |_|_|___/\__|___/*/
 /*                             |___/                                             */

#include "alloc.h"
#include "type_basic.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief
 * Type used to represent a single cell inside a doubly linked list.
 */
typedef struct dcell {
    uint val; //!< The value of the cell (not meaningful if the cell is a
    //!< sentinel).
    struct dcell* next; //!< Pointer to the next cell (NULL if this is the right
    //!< sentinel).
    struct dcell* previous; //!< Pointer to the previous cell (NULL if this is
    //!< the left sentinel).
} dcell;

/**
 * @brief
 * Type used to represent a doubly linked list.
 */
typedef struct {
    uint size;    //!< Size of the list.
    dcell* lsent; //!< A pointer to the left sentinel.
    dcell* rsent; //!< A pointer to the right sentinel.
} dlist;

/**
 * @brief
 * Creation of an empty list.
 *
 * @return
 * The empty list.
 */
dlist* create_dlist(void);

/**
 * @brief
 * Release of a list.
 */
void delete_dlist(dlist* //!< The list.
);

/**
 * @brief
 * Insertion of a new cell after a given one.
 */
void insertnext_dlist(dlist*, //!< The list.
    dcell*, //!< The cell.
    int      //!< The value of the new cell.
);

/**
 * @brief
 * Insertion of a new cell before a given one.
 */
void insertprevious_dlist(dlist*, //!< The list.
    dcell*, //!< The cell.
    int      //!< The value of the new cell.
);

/**
 * @brief
 * Release of a single cell.
 */
void deletecell_dlist(dlist*, //!< The list.
    dcell*  //!< The cell.
);

/**
 * @brief
 * Merging of two lists.
 *
 * @remark
 * The merge is done in the first list. The second list is released.
 */
void concat_dlist(dlist*, //!< The first list.
    dlist*  //!< The second list.
);

#endif // DLIST_H_
