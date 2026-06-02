/**
 * @file monoid.h
 *
 * @brief
 * Implementation of morphisms into finite monoids.
 *
 * @details
 * Contains the definition of the types "green" and "morphism".
 * The type "green" is used to represent the Green relations of a finite monoid.
 * The type "morphism" is used to represent a morphism into a finite monoid (it
 * includes the Green relations of this monoid).
 * Includes the functions used to create new morphisms and their Green relations
 * from a complete DFA as well as the function manipulating morphisms.
 *
 */

#ifndef _MONOID_H_
#define _MONOID_H_

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#include "graphs.h"
#include "graphs_tarjan.h"
#include "nfa.h"
#include "tools.h"
#include "type_basic.h"
#include "type_dequeue.h"
#include "type_dequeue_gen.h"
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*  __  __                   _     _                       _  */
/* |  \/  | ___  _ __   ___ (_) __| |___    __ _ _ __   __| | */
/* | |\/| |/ _ \| '_ \ / _ \| |/ _` / __|  / _` | '_ \ / _` | */
/* | |  | | (_) | | | | (_) | | (_| \__ \ | (_| | | | | (_| | */
/* |_|  |_|\___/|_| |_|\___/|_|\__,_|___/  \__,_|_| |_|\__,_| */
/*  _ __ ___   ___  _ __ _ __ | |__ (_)___ _ __ ___  ___      */
/* | '_ ` _ \ / _ \| '__| '_ \| '_ \| / __| '_ ` _ \/ __|     */
/* | | | | | | (_) | |  | |_) | | | | \__ \ | | | | \__ \     */
/* |_| |_| |_|\___/|_|  | .__/|_| |_|_|___/_| |_| |_|___/     */
/*                      |_|                                   */

// #define DEBUG_MONO

/**
 * @brief
 * In every monoid, the neutral element is at index 0.
 */
#define ONE 0

/**
 * @brief
 * Type used to represent the Green relations of a finite monoid.
 */
typedef struct
{

    /* Partitions */
    parti *JCL; //!< J-classes.
    parti *LCL; //!< L-classes.
    parti *RCL; //!< R-classes.
    parti *HCL; //!< H-classes.

    /* Informations on regular elements */
    uint nb_regular_elems; //!< Number of regular elements.
    bool *regular_array;   //!< Array of Booleans indexed by the elements of the monoid. Marks the regular elements.

    /* Information on groups */
    bool *group_array; //!< Array of Booleans indexed by the elements of the monoid. Marks the ones that belong to a group.
} green;

/**
 * @brief
 * The type used to represent a morphism into a finite monoid.
 */
typedef struct
{
    /* Mandatory fields */
    letter *alphabet; //!< An array indexed by the letters indices (generators). Assigns its letter to each index.
    uint *pred_ele;   //!< Array of preceding elements (for the naming as a product of generators).
    uint *pred_lab;   //!< Array of preceding letters (for the naming as a product of generators).
    dgraph *r_cayley; //!< The right Cayley graph of the morphism (stores the number of elements and the numbers of letters).
    dgraph *l_cayley; //!< The left Cayley graph of the morphism.

    uint nb_idems;      //!< Number of idempotents in the morphism.
    uint *idem_list;    //!< The list of all idempotents, sorted in increasing order.
    bool *idem_array;   //!< An array of Booleans indexed by the elements. Marks the idempotents.
    uint nb_accept;     //!< Number of accepting elements in the morphism.
    uint *accept_list;  //!< The list of all accepting elements, sorted in increasing order.
    bool *accept_array; //!< An array of Booleans indexed by the elements. Marks the accepting elements.
    green *rels;        //!< The Green relations of the monoid.

    uint nb_regular_jcl;     //!< Number of regular J-classes.
    uint nb_min_regular_jcl; //!< The number of "strict minimal" J-classes (no smaller regular J-class has a nonempty antecedent).
    uint *regular_idems;     //!< Array indexed by the regular J-classes (sorted in topological order). Associates a member idempotent to each of them (the one with the least index). The minimal J-classes are at the beginning.

    /* Optional fields (NULL if not computed) */
    uint *mult;          //!< The multiplication table size r_cayley->size_graph * r_cayley->size_graph (`NULL` if not computed).
    bool **order;        //!< The ordering of the monoid (NULL if not computed). order[i][j] is true if the element i is smaller than the element j in the syntactic order.
    bool *order_storage; //!< Storage for the previous table (one dimension array of size r_cayley->size_graph * r_cayley->size_graph).
} morphism;

/*******************/
/* Basic functions */
/*******************/

/**
 * @brief
 * Release of the Green relations.
 */
void delete_green(green * //!< The Green relations.
);

/**
 * @brief
 * Release of a morphism.
 */
void delete_morphism(morphism * //!< The morphism.
);

/**
 * @brief
 * Copy the alphabet of a morphism.
 *
 * @return
 * A copy of the alphabet array.
 */
letter *mor_duplicate_alpha(const morphism * //!< The morphism.
);

/**
 * @brief
 * Retrieves the index of a letter in the alphabet of a morphism.
 *
 * @remark
 * If the letter does not belong to the alphabet the invalid index
 * UINT_MAX is returned.
 *
 * @return
 * The index of the letter.
 */
uint mor_letter_index(const morphism *, //!< The morphism.
                      letter            //!< The letter.
);

/**
 * @brief
 * Given as input a regular element, retrieves the index of the
 */

uint mor_regular_jclass(const morphism *, //!< The morphism.
                        uint              //!< The element.
);

/**********************************************/
/* Preliminary functions for the construction */
/**********************************************/

/**
 * @brief
 * Computation of the left Cayley graph of a morphism.
 *
 * @remark
 * The right Cayley graph of the morphism must be computed.
 */
void mor_compute_leftcayley(morphism * //!< The morphism.
);

/**
 * @brief
 * Computes an array containing all elements in a given J-class sorted according to
 * the indices of their R-class (first) and L-classes (second).
 *
 * @return
 * The sorted array.
 */
uint *green_sorted_jclass(green *, //!< The Green relations.
                          uint     //!< The index of the J-class.
);

/**
 * @brief
 * Computation of the relation H from J, L and R (which must be already computed).
 */
void h_green_compute(green * //!< The Green relations (J, R et L must be computed).
);

/**
 * @brief
 * Computation of informations on regular elements and groups (the Green relations themselves must be already computed).
 *
 * @remark
 * The computation requires the list of idempotent elements.
 */
void gr_green_compute(uint *, //!< The list of idempotents.
                      uint,   //!< The number of idempotents.
                      green * //!< The Green relations.
);

/**
 * @brief
 * Computation of list of representative idempotents for the morphism.
 *
 * @remark
 * The computation requires the full computation of the Green relations.
 */
void mor_compute_rep(morphism *);

/**
 * @brief
 * Allocates and computes the Green relations of a morphism.
 *
 * @remark
 * All other mandatorty fields of the morphism must have been computed.
 */
void mor_compute_green(morphism * //!< The morphism.
);

/**
 * @brief
 * Computation of the number of strict J-classes
 *
 * @remark
 * The Green relations must have been computed.
 */
void mor_compute_min_regular_jcl(morphism * //!< The morphism.
);

/************************************/
/* Construction from a complete DFA */
/************************************/

/**
 * @brief
 * Computation of the transition morphism associated to a complete DFA.
 *
 * @return
 * The transition morphism.
 */
morphism *dfa_to_morphism(dfa *,  //!< The complete DFA.
                          int *,  //!< Error code to be filled if not NULL.
                          uint ** //!< A pointer to return function table used to construct the monoid (NULL if not used).
);

/**
 * @brief
 * Computes the size of the morphism associated to a complete DFA without storing
 * the actual morphism.
 *
 * @return
 * The size of the morphism.
 */
uint dfa_to_morphism_size(dfa *A //!< The complete DFA.
);

/**
 * @brief
 * Computation of a complete DFA from the right Cayley graph of a morphism.
 *
 * @return
 * The complete DFA.
 */
dfa *morphism_to_dfa(morphism * //!< The morphism.
);

/**
 * @brief
 * Computation of a complete DFA from the left Cayley graph of a morphism.
 *
 * @return
 * The complete DFA.
 */
dfa *left_morphism_to_dfa(morphism * //!< The morphism.
);

/***************************************/
/* Computing information on a morphism */
/***************************************/

/**
 * @brief
 * Computation of the multiplication table of a morphism.
 *
 * @remark
 * The computation is only made if the multiplication table has not already been computed.
 */
void mor_compute_mult(morphism * //!< The morphism.
);

/**
 * @brief
 * Computation of the mirror of the right Cayley graph.
 *
 * @return
 * The mirror.
 */
lgraph *mor_rmirror(morphism * //!< The morphism.
);

/**
 * @brief
 * Computation of the mirror of the left Cayley graph.
 *
 * @return
 * The mirror.
 */
lgraph *mor_lmirror(morphism * //!< The morphism.
);

/**
 * @brief
 * Computation of the syntactic order table of a morphism.
 *
 * @attention
 * The morphism has to be a syntactic morphism.
 *
 * @remark
 * The computation is only made if the syntactic order has not already been computed.
 */
void mor_compute_order(morphism * //!< The morphism.
);

/***************************/
/* Operations on morphisms */
/***************************/

/**
 * @brief
 * Return the name of an element (product of generators) in a dequeue.
 *
 * @return
 * The name of the element.
 */
dequeue *mor_name(morphism *, //!< The morphism.
                  uint        //!< The element.
);

/**
 * @brief
 * Multiplication of two elements
 *
 * @remark
 * More efficient if the multiplication table has been computed.
 *
 * @return
 * The resulting element.
 */
uint mor_mult(morphism *, //!< The morphism.
              uint,       //!< First element.
              uint        //!< Second element.
);

/**
 * @brief
 * Multiplication of an arbitrary number of elements.
 *
 * @remark
 * More efficient if the multiplication table has been computed.
 *
 * @return
 * The resulting element.
 */
uint mor_mult_gen(morphism *, //!< The morphism.
                  uint,       //!< The number of elements to multiply.
                  ...         //!< The elements.
);

/**
 * @brief
 * Computes the omega power of an element.
 *
 * @return
 * The omega power.
 */
uint mor_omega(morphism *, //!< The morphism.
               uint        //!< The element.
);

/**
 * @brief
 * Computes the index of an idempotent element.
 *
 * @details
 * Returns false if the element given as input is not an idempotent. The index
 * is returned using the third parameter.
 *
 * @return
 * A Boolean indicating if the element is an idempotent.
 */
bool mor_num_idem(morphism *, //!< The morphism.
                  uint,       //!< The element.
                  uint *      //!< A pointer used to return the index of the idempotent.
);

/**************/
/* Properties */
/**************/

/**
 * @brief
 * Tests if there exists a letter mapped to the neutral element.
 *
 * @remark
 * Can dislpay the results of the test on a stream given as input.
 *
 * @return
 * A Boolean indicating whether there exists a letter mapped to the neutral element.
 */
bool mor_neutral_letter(morphism *, //!< The morphism.
                        FILE *      //!< The stream (NULL if no display is desired).
);

/**
 * @brief
 * Tests if there exists a nonempty antecedent of the neutral element in a morphism.
 *
 * @return
 * A Boolean indicating whether there exists a nonempty antecedent of the neutral element.
 */
bool mor_nonempty_neutral(morphism * //!< The morphism.
);

/**
 * @brief
 * Tests if all elements in a morphism are regular.
 *
 * @return
 * A Boolean indicating whether all elements are regular.
 */
bool mor_all_regular(morphism * //!< The morphism.
);

/**
 * @brief
 * Computes the image of a word.
 *
 * @remark
 * If the word is not defined over the alphabet of the morphism, the (invalid)
 * element M->r_cayley->size_graph is returned.
 *
 * @return
 * The image
 */
uint mor_compute_image(morphism *, //!< The morphism.
                       word *      //!< The word.
);

/**
 * @brief
 * Returns the graph of a single R-class
 *
 * @remark
 * Transitions that go out of the R-class are mapped to UINT_MAX.
 *
 * @return
 * The graph of the R-class.
 */
dgraph *mor_extract_rcl(morphism *M, //!< The morphism.
                        uint rcl     //!< The index of the R-class.
);

typedef enum
{
    FACTO_LEAF,
    FACTO_BINARY,
    FACTO_IDEM,
} facto_type;

typedef struct
{
    facto_type type; //!< The type of the node.
    uint elem;       //!< The value of the node.
    uint letter;     //!< The letter associated to the node (only for leaf nodes).
    int st_children; //!< First index of the children in the array of children (only for binary and idempotent nodes).
    int nb_children; //!< The number of children.
} facto_node;

typedef struct
{
    facto_node *nodes; //!< Array of nodes of the forest.
    int size_nodes;    //!< Size of the array of nodes.
    int nb_nodes;      //!< Number of nodes in the forest.

    int *childrens;     //!< Array storing the lists of children.
    int nb_childrens;   //!< Number of children in the forest.
    int size_childrens; //!< Size of the array storing the lists of children.

    int root; //!< The root of the forest (index of the node in the array of nodes).
} facto_forest;

facto_forest *mor_compute_facto_forest(morphism *M, word *w);

/*

// Récupération des éléments intersectant chaque alphabet dans un tableau
dequeue** at_table_cayley(morphism*);

// Affichage du tableau
void print_at_table(morphism* M, dequeue** table, FILE* out); */

#endif // _MONOID_H_
