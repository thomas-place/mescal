/**
 * @file monoid_ideals.h
 * @brief
 * Computations of ideals in monoids.
 */

#ifndef IDEALS_H
#define IDEALS_H

 /*  __  __                   _     _         ___    _            _      */
 /* |  \/  | ___  _ __   ___ (_) __| |___ _  |_ _|__| | ___  __ _| |___  */
 /* | |\/| |/ _ \| '_ \ / _ \| |/ _` / __(_)  | |/ _` |/ _ \/ _` | / __| */
 /* | |  | | (_) | | | | (_) | | (_| \__ \_   | | (_| |  __/ (_| | \__ \ */
 /* |_|  |_|\___/|_| |_|\___/|_|\__,_|___(_) |___\__,_|\___|\__,_|_|___/ */

#include <stdbool.h>
#include "monoid.h"



/**
 * @brief
 * Computation of the right ideal of an element.
 *
 * @remark
 * The third parameter is an array of Booleans indexed by the elements of
 * the monoid. It restricts the elements in the returned list to those which
 * are marked true by the array. When no restriction is needed a NULL pointer
 * should be given as input.
 *
 * @return
 * The right ideal sorted in increasing order.
 */
dequeue* compute_r_ideal(morphism*, //!< The morphism.
    uint,    //!< The element
    bool*                        //!< An array of Booleans indexed by the elements. Can be used to restrict the output.
);

/**
 * @brief
 * Computation of the left ideal of an element.
 *
 * @remark
 * The third parameter is an array of Booleans indexed by the elements of
 * the monoid. It restricts the elements in the returned list to those which
 * are marked true by the array. When no restriction is needed a NULL pointer
 * should be given as input.
 *
 * @return
 * The left ideal sorted in increasing order.
 */
dequeue* compute_l_ideal(morphism*, //!< The morphism.
    uint,                         //!< The element
    bool*                        //!< An array of Booleans indexed by the elements. Can be used to restrict the output.
);

/**
 * @brief
 * Computation of the two-sided ideal of an element.
 *
 * @remark
 * The third parameter is an array of Booleans indexed by the elements of
 * the monoid. It restricts the elements in the returned list to those which
 * are marked true by the array. When no restriction is needed a NULL pointer
 * should be given as input.
 *
 * @return
 * The two-sided ideal sorted in increasing order.
 */
dequeue* compute_j_ideal(morphism*, //!< The morphism.
    uint,                      //!< The element
    bool*                     //!< An array of Booleans indexed by the elements. Can be used to restrict the output.
);


/**
 * @brief
 * Computation of a given restricted right ideal of an element. The restriction is
 * given by a sub-alphabet: the only available transitions are those labeled by a
 * letter in this sub-alphabet.
 *
 * @remark
 * The fourth parameter is an array of Booleans indexed by the elements of
 * the monoid. It restricts the elements in the returned list to those which
 * are marked true by the array. When no restriction is needed a NULL pointer
 * should be given as input.
 *
 * @return
 * The restricted right ideal sorted in increasing order.
 */
dequeue* compute_r_ideal_alph(morphism*, //!< The morphism.
    uint,                              //!< The element
    bool*,                           //!< An array of Booleans indexed by the letters. Marks the letters inside the sub-alphabet
    bool*                             //!< An array of Booleans indexed by the elements. Can be used to restrict the output.
);

/**
 * @brief
 * Computation of a given restricted left ideal of an element. The restriction is
 * given by a sub-alphabet: the only available transitions are those labeled by a
 * letter in this sub-alphabet.
 *
 * @remark
 * The fourth parameter is an array of Booleans indexed by the elements of
 * the monoid. It restricts the elements in the returned list to those which
 * are marked true by the array. When no restriction is needed a NULL pointer
 * should be given as input.
 *
 * @return
 * The restricted left ideal sorted in increasing order.
 */
dequeue* compute_l_ideal_alph(morphism*, //!< The morphism.
    uint,                  //!< The element
    bool*,                    //!< An array of Booleans indexed by the letters. Marks the letters inside the sub-alphabet
    bool*                             //!< An array of Booleans indexed by the elements. Can be used to restrict the output.
);


/**
 * @brief
 * Computation of a given restricted left/right ideal of an element. The restriction is
 * given by a sub-alphabet: the only available transitions are those labeled by a
 * letter in this sub-alphabet.
 *
 * @remark
 * The fourth parameter is an array of Booleans indexed by the elements of
 * the monoid. It restricts the elements in the returned list to those which
 * are marked true by the array. When no restriction is needed a NULL pointer
 * should be given as input.
 *
 * @return
 * The restricted left ideal sorted in increasing order.
 */
dequeue* compute_j_ideal_alph(morphism*, //!< The morphism.
    uint,                      //!< The element
    bool*,                         //!< An array of Booleans indexed by the letters. Marks the letters inside the sub-alphabet
    bool*                             //!< An array of Booleans indexed by the elements. Can be used to restrict the output.
);


/**
 * @brief
 * Given an idempotent g and an element q such that g R q, compute an element
 * s such that g = qr.
 *
 * @return
 * The element r.
 */
uint get_rlink(morphism*, //!< The morphism.
    parti*,       //!< The R-classes.
    uint,            //!< Idempotent.
    uint               //!< Start element.
);

/**
 * @brief
 * Given an idempotent h and an element t such that h R t, compute an element
 * s such that h = st.
 *
 * @return
 * The element s.
 */
uint get_llink(morphism*, //!< The morphism.
    parti*,         //!< The L-classes.
    uint,             //!< Idempotent.
    uint               //!< Start element.
);



#endif
