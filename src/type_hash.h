#ifndef TYPE_HASH_H
#define TYPE_HASH_H


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "alloc.h"
#include "error.h"
#include "type_basic.h"

//#define DEBUG_HASH

typedef struct {
    uint size;          //!< Size of the hash table.
    uint num_elements;  //!< Number of elements in the hash table.
    uint inserts;
    uint collisions;   //!< Number of collisions in the hash table.
    uint* table;        //!< Pointer to the hash table.
    uint(*hash_func)(uint, uint); //!< Hash function to compute the index for a given key.
    bool (*comp)(uint, uint); //!< Comparison function for the values in the hash table.
} hash_table;

hash_table* create_hash_table(uchar init, //!< Initial power of two for the size of the hash table.
    uint(*hash_func)(uint, uint),  //!< Hash function to compute the index for a given key.
    bool (*comp)(uint, uint) //!< Comparison function for the values in the hash table.
);


void delete_hash_table(hash_table* ht //!< The hash table to delete.
);

uint hash_table_insert(hash_table* ht, //!< The hash table.
    uint index //!< Index of the object in the object
);




// typedef struct {
//     uint* elem;                  //!< An element: a permutation of {0,...,n-1}.
//     uint num;                    //!< Index of the element in the future monoid.
//     uint pred_ele;               //!< The preceding element of the state.
//     uint pred_lab;               //!< The preceding letter of the state.
//     uint* next;                  //!< Array of size size_alpha containing the transitions, one for each letter.
// } mor_elem;

// typedef struct {
//     uint nb_elems;
//     uint nb_states;
//     uint nb_letters;
//     uint size;
//     uint* permuts;                  //!< size * nb_states.
//     uint* next;                     //!< size * nb_letters.
//     uint* pred_ele;                //!< size.
//     uint* pred_lab;                //!< size.
// } mor_object;

// int comp_mor_elem(uint e1, uint e2, void* param);

// uint hash_mor_elem(uint i, uint size, void* param);

// mor_object* mor_object_init(uint nb_states, uint nb_letters, uchar init);

// void mor_object_free(mor_object* M);

// void mor_object_grow(mor_object* M //!< The morphism object.
// );



#endif // TYPE_HASH_H
