/**
 * @file monoid_orbits.h
 * @brief
 * Computation of orbits.
 */
#ifndef MONOID_ORBITS_H
#define MONOID_ORBITS_H

#include "monoid.h"
#include "monoid_ideals.h"
#include "monoid_sub.h"
#include "sep_group.h"

 /*  __  __                   _     _          ___       _     _ _        */
 /* |  \/  | ___  _ __   ___ (_) __| |___ _   / _ \ _ __| |__ (_) |_ ___  */
 /* | |\/| |/ _ \| '_ \ / _ \| |/ _` / __(_) | | | | '__| '_ \| | __/ __| */
 /* | |  | | (_) | | | | (_) | | (_| \__ \_  | |_| | |  | |_) | | |_\__ \ */
 /* |_|  |_|\___/|_| |_|\___/|_|\__,_|___(_)  \___/|_|  |_.__/|_|\__|___/ */



/**
 * @brief
 * Type used to represent C-orbits of a morphism.
 *
 * @remark
 * Depending on the class C, it may not be required to compute all orbits.
 * If an orbit is not computed, the corresponding cell in the array is NULL.
 */
typedef struct {
    morphism* original;  //!< The original morphism.
    subsemi** orbits;    //!< Array of orbits indexed by the regular J-classes. A cell has the NULL value if the orbit of the corresponding idempotent has not been computed.
    uint nb_computed;    //!< The number of computed orbits (only those required for tests are computed, this depends on the class C). This is the size of the array.
    sub_level level;     //!< The computation level used for the orbits.
} orbits;

/*********************/
/* General functions */
/*********************/

/**
 * @brief
 * Initialization of an orbits set.
 *
 * @return
 * The orbits set.
 */
orbits* init_orbits(morphism* //!< The morphism.
);

/**
 * @brief
 * Release of an orbits set.
 *
 * @attention
 * The original morphism and its Green relations are not released.
 */
void delete_orbits(orbits* //!< The orbits set.
);



/*************/
/* DD-orbits */
/*************/

/**
 * @brief
 * Computation of the DD-orbit associated to a given idempotent (local monoid).
 *
 * @remark
 * The computation level always defaults to LV_FULL. Hence, it is not a parameter.
 *
 * @return
 * The DD-orbit.
 */
subsemi* compute_one_ddorb(morphism*, //!< The morphism.
    uint //!< The idempotent.
);

/**
 * @brief
 * Initialization of the DD-orbits set.
 *
 * @details
 * Computes one orbit per minimal strict regular J-class.
 *
 * @remark
 * The computation level always defaults to LV_FULL. Hence, it is not a parameter.
 *
 * @return
 * The (partial) set of DD-orbits.
 */
orbits* compute_ddorbits(morphism* //!< The morphism.
);



/*************/
/* G⁺-orbits */
/*************/

/**
 * @brief
 * Computes the G⁺-orbit associated to a given idempotent for group class G. The G-kernel
 * needs to be given as an input.
 *
 * @remark
 * The computation level is determined by the G-kernel (which can be either LV_FULL or LV_REG).
 *
 * @return
 * The G⁺-orbit.
 */
subsemi* compute_one_gplusorb(subsemi*, //!< The G-kernel.
    uint //!< The idempotent.
);

/**
 * @brief
 * Initialization of the G⁺-orbits set.
 *
 * @details
 * Computes one orbit per minimal strict regular J-class.
 *
 * @remark
 * The computation level is determined by the G-kernel (which can be either LV_FULL or LV_REG).
 *
 * @return
 * The set of G⁺-orbits.
 */
orbits* compute_gplusorbits(subsemi* //!< The G-kernel.
);



/********************************************/
/* PT-orbits (which are also the AT-orbits) */
/********************************************/

/**
 * @brief
 * Given an element of a monoid, computes the set of letters which label a transition in the
 * strongly connected component of this element in the right Cayley graph.
 *
 * @return
 * An array of Boolean indexed by the letters marking those in the set.
 */
bool* compute_maxalph_scc(morphism*, //!< The morphism.
    uint //!< The element.
);

/**
 * @brief
 * Computes the maximal shared alphabet between two idempotents.
 *
 * @return
 * An array of Boolean indexed by the letters marking those in the set (NULL is not sharded alphabet exists).
 */
bool* compute_maxalph_com_scc(morphism*,//!< The morphism.
    uint, //!< The first idempotent.
    uint //!< The second idempotent.
);

/**
 * @brief
 * Computes the PT-orbit associated to a given idempotent.
 *
 * @remark
 * The computation level always defaults to LV_FULL. Hence, it is not a parameter.
 *
 * @return
 * The PT-orbit.
 */
subsemi* compute_one_ptorb(morphism*, //!< The morphism.
    uint //!< The idempotent.
);

/**
 * @brief
 * Initialization of the PT-orbits set.
 *
 * @remark
 * The computation level always defaults to LV_FULL. Hence, it is not a parameter.
 *
 * @details
 * Computes one PT-orbit per regular J-class.
 *
 * @return
 * The (partial) set of PT-orbits.
 */
orbits* compute_ptorbits(morphism* //!< The morphism.
);



/******************/
/* BPol(G)-orbits */
/******************/

/**
 * @brief
 * Given an idempotent e, computes the associated BPol(C)-orbit from a relevant subset of the
 * Pol(C)-pairs.
 *
 * @attention
 * Only the Pol(C)-pairs (s,t) such that s belongs to eM and t R e are given as input (this is
 * enough information to compute the BPol(C)-orbit of e). They are given by a two-dimensional
 * array of Booleans indexed by the indices of the right ideal eM (first) and the indices of the
 * R-class of e (second).
 *
 * @remark
 * The procedure exploits the fact that the BPol(C)-orbites are the same as the co-Pol(C)-orbits.
 *
 * @remark
 * The available computation levels are LV_FULL and LV_GREG. If LV_REG is chosen, the computation
 * defaults to LV_GREG.
 *
 * @return
 * The BPol(C)-orbit.
 */
subsemi* compute_one_orbit_from_pairs(morphism*, //!< The morphism.
    uint, //!< The index of the idempotent e.
    dequeue*, //!< The right ideal eM associated to e.
    bool**, //!< Two-dimensional array indicating the Pol(C)-pairs.
    sub_level //!< Desired computation level.
);

/**
 * @brief
 * Given a monoid, computes the partial multiplication table between all elements within a
 * subset and all elements within a J-class that must contain the subset. Only the products
 * evaluating to an element of the J-class are computed.
 *
 * @remark
 * The returned multiplication table is indexed by the indices of the elements of the subset
 * in the input list (first) and by the indices of the J-class (second).  Its cells contain
 * the result of every product in the original monoid. If a product does not fall within the
 * J-class, it is given the value ->M->r_cayley->size_graph (which is not a valid element).
 *
 * @return
 * The multiplication table.
 */
uint** compute_jmult(morphism*, //!< The morphism.
    dequeue*, //!< The subset.
    dequeue* //!< The J-class that contains the subset.
);

/**
 * @brief
 * Computes a subset of the Pol(G)-pairs associated to a given morphism. The comutation depends
 * on the G-kernel which is given as input.
 *
 * @attention
 * A regular R-class is given as input. Only the Pol(G)-pairs (s,t) such that s is in the right
 * ideal of the R-class and t is in the R-class itself are computed.
 *
 * @return
 * A two-dimensional array of Booleans indexed by the indices of the right ideal (first) and the
 * indices of the R-class (second). It marks the Pol(G)-pairs.
 */
bool** compute_polgpairs(subsemi*, //!< The G-kernel.
    dequeue*, //!< The right ideal associated to the R-class.
    uint //!< The index of the R-class.
);

/**
 * @brief
 * Computes the BPol(G)-orbit associated to a given idempotent for group class G. The G-kernel
 * needs to be given as an input.
 *
 * @remark
 * The available computation levels are LV_FULL and LV_GREG. If LV_REG is chosen, the computation
 * defaults to LV_GREG.
 *
 * @return
 * The BPol(G)-orbit.
 */
subsemi* compute_one_bpgorb(subsemi*, //!< The G-kernel.
    uint, //!< The idempotent.
    sub_level //!< Desired computation level.
);

/**
 * @brief
 * Initialization of the BPol(G)-orbits set.
 *
 * @details
 * Computes one BPol(G)-orbits per regular J-class.
 *
 * @remark
 * The available computation levels are LV_FULL and LV_GREG. If LV_REG is chosen, the computation
 * defaults to LV_GREG.
 *
 * @return
 * The (partial) set of BPol(G)-orbits.
 */
orbits* compute_bpgorbits(subsemi*, //!< The G-kernel.
    sub_level //!< Desired computation level.
);



/*******************/
/* BPol(G⁺)-orbits */
/*******************/

/**
 * @brief
 * Computes a subset of the Pol(G⁺)-pairs associated to a given morphism. The comutation depends
 * on the G⁺-orbits which are given as input.
 *
 * @attention
 * A regular R-class is given as input. Only the Pol(G⁺)-pairs (s,t) such that s is in the right
 * ideal of the R-class and t is in the R-class itself are computed.
 *
 * @return
 * A two-dimensional array of Booleans indexed by the indices of the right ideal (first) and the
 * indices of the R-class (second). It marks the Pol(G)-pairs.
 */
bool** compute_polgpluspairs(orbits*, //!< The G⁺-orbits.
    dequeue*, //!< The right ideal associated to the R-class.
    uint //!< The index of the R-class.
);

/**
 * @brief
 * Computes the BPol(G⁺)-orbit associated to a given idempotent for group class G. The G⁺-orbits
 * needs to be given as an input.
 *
 * @remark
 * The available computation levels are LV_FULL and LV_GREG. If LV_REG is chosen, the computation
 * defaults to LV_GREG.
 *
 * @return
 * The BPol(G⁺)-orbit.
 */
subsemi* compute_one_bpgplusorb(orbits*, //!< The G⁺-orbits.
    uint, //!< The idempotent.
    sub_level //!< Desired computation level.
);

/**
 * @brief
 * Initialization of the BPol(G⁺)-orbits set.
 *
 * @details
 * Computes one BPol(G⁺)-orbits per regular J-class.
 *
 * @remark
 * The available computation levels are LV_FULL and LV_GREG. If LV_REG is chosen, the computation
 * defaults to LV_GREG.
 *
 * @return
 * The set of BPol(G⁺)-orbits.
 */
orbits* compute_bpgplusorbits(orbits*, //!< The G⁺-orbits.
    sub_level //!< Desired computation level.
);


#endif
