#ifndef _MONOID_SUB_H_
#define _MONOID_SUB_H_

/*  ____        _                        _                                  */
/* / ___| _   _| |__  ___  ___ _ __ ___ (_) __ _ _ __ ___  _   _ _ __  ___  */
/* \___ \| | | | '_ \/ __|/ _ \ '_ ` _ \| |/ _` | '__/ _ \| | | | '_ \/ __| */
/*  ___) | |_| | |_) \__ \  __/ | | | | | | (_| | | | (_) | |_| | |_) \__ \ */
/* |____/ \__,_|_.__/|___/\___|_| |_| |_|_|\__, |_|  \___/ \__,_| .__/|___/ */
/*                                         |___/                |_|         */

#include "monoid.h"
#include "tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief
 *  Computation levels available for subsemigroups.
 */
typedef enum {
    LV_REG,   //!< Only regular elements are computed.
    LV_GREG,  //!< All elements are computed, but the Green relations are only computed for the regular elements (other elements are put into singletons).
    LV_FULL,  //!< Full subsemigroup.
} sub_level;

/**
 * @brief
 * Type used to represent a subsemigroup and its Green relations.
 *
 * @remark
 * The subsemigroup has to be a monoid but its neutral element needs not be the one of the
 * original monoid.
 */
typedef struct {
    sub_level level;     //!< Level of computation.
    morphism* original;  //!< The original morphism.
    uint size;           //!< Size of the subsemigroup (or the number of regular elements if full is false).

    uint* sub_to_mono;  //!< Array indexed by the elements of the subsemgroup. Maps each one to the corresponding element in the original monoid.
    bool* mono_in_sub;  //!< Array of Booleans indexed by the elements of the original monoid. Marks the elements inside the subsemigroup.
    uint* mono_to_sub;  //!< Array of Booleans indexed by the elements of the original monoid. Maps each elements to the orrepsonding one in the subsemigroup (if it exists).
    uint neut;          //!< Index of the neutral element in the subsemigroup.
    dequeue* idem_list; //!< List of all idempotents.
    bool* idem_array;   //!< Array of Booleans indexed by the elements of the subsemgroup. Marks the idempotents.
    green* rels;        //!< The Green relations of the subsemigroup.
} subsemi;

/******************************/
/* Manipulating subsemigroups */
/******************************/

/**
 * @brief
 * Intitialization of a subsemigroup.
 *
 * @return
 * The subsemigroup.
 */
subsemi* init_subsemi(morphism* //!< The original morphism.
);




/**
 * @brief
 * Allocates and fills the tables sub_to_mono and mono_to_sub of a subsemigroup.
 *
 * @remark
 * The size of the subsemigroup must be known and the table mono_in_sub must be
 * computed.
 */
void compute_maps_subsemi(subsemi* //!< The subsemigroup.
);

/**
 * @brief
 * Computes the idempotents inside a subsemigroup.
 */
void compute_idems_subsemi(subsemi* //!< The subsemigroup.
);

/**
 * @brief
 * Computes an array containing all elements in the subsemigroup
 * ordered according to the J-order of the original monoid.
 */
uint* compute_jord_subsemi(subsemi* //!< The subsemigroup.
);


/**
 * @brief
 * Computes the green relation J of a subsemigroup.
 *
 * @remark
 * The other fields in the subsemigroup must have already been computed.
 * The relations R and L must be computed.
 *
 */
void compute_jrel_subsemi(subsemi*, //!< The subsemigroup.
    uint* //!< The array of elements ordered according to the J-order of the original monoid (cimputed with the previous function).
);


/**
 * @brief
 * Release of a subsemigroup.
 *
 * @attention
 * The original morphism and its Green relations are not released.
 */
void delete_subsemi(subsemi* //!< The subsemigroup.
);

/**
 * @brief
 * Given a list of elements in a monoid and a subsemigroup of a monoid, returns the sublist
 * restricted to the elements inside the subsemigroup.
 *
 * @return
 * The restricted list.
 */
dequeue* restrict_list_to_subsemi(subsemi*, //!< The subsemigroup.
    dequeue* //!< The list of element in the original monoid.
);





/**
 * @brief
 * Computes the relations R, L and J associated to a subsemigroup.
 *
 * @remark
 * Other fields in the subsemigroup must have already been computed.
 *
 * @attention
 * Inefficient. Only use if no alternative is available.
 */
void green_compute_sub(subsemi* //!< The sub-semigroup.
);

/**
 * @brief
 * Computes the relations R, L and J associated to a subsemigroup restricted to the regular elements.
 *
 * @remark
 * Other fields in the subsemigroup must have already been computed.
 *
 * @attention
 * Efficient but does not compute the non-regular classes (all non-regular elements are put into singletons).
 */
void green_compute_sub_reg(subsemi* //!< The sub-semigroup.
);




#endif // _MONOID_SUB_H_
