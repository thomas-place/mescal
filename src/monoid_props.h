/**
 * @file monoid_props.h
 * @brief
 * Tests of properties on morphisms.
 */

#ifndef MONO_PROPS_H
#define MONO_PROPS_H

#include <stdbool.h>
#include <stdlib.h>
#include "nfa.h"
#include "monoid.h"
#include "monoid_orbits.h"
#include "monoid_kernels.h"
#include "sep_group.h"

 /*  __  __                   _     _         ____                            _   _            */
 /* |  \/  | ___  _ __   ___ (_) __| |___ _  |  _ \ _ __ ___  _ __   ___ _ __| |_(_) ___  ___  */
 /* | |\/| |/ _ \| '_ \ / _ \| |/ _` / __(_) | |_) | '__/ _ \| '_ \ / _ \ '__| __| |/ _ \/ __| */
 /* | |  | | (_) | | | | (_) | | (_| \__ \_  |  __/| | | (_) | |_) |  __/ |  | |_| |  __/\__ \ */
 /* |_|  |_|\___/|_| |_|\___/|_|\__,_|___(_) |_|   |_|  \___/| .__/ \___|_|   \__|_|\___||___/ */
 /*                                                          |_|                               */

/**
 * @brief
 * The four Green relations.
 */
typedef enum
{
    H_GREEN,
    L_GREEN,
    R_GREEN,
    J_GREEN,
} green_relation;

extern char green_rel_array[4];


/***********/
/* Trivial */
/***********/

/**
 * @brief
 * Tests if a monoid is trivial.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two distinct elements in the monoid.
 *
 * @return
 * A Boolean indicating whether the monoid is trivial.
 */
bool is_trivial_monoid(morphism*,    //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a semigroup (image of A⁺) is trivial.
 *
* @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two distinct elements in the semigroup.
 *
 * @return
 * A Boolean indicating whether the syntactic semigroup is trivial.
 */
bool is_trivial_semigroup(morphism*, //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a subsemigroup is trivial.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two distinct elements in the subsemigroup.
 *
 * @return
 * A Boolean indicating whether the subsemigroup is trivial.
 */
bool is_trivial_subsemi(subsemi*, //!< The subsemigroup.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if all orbits are trivial.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two distinct elements in an orbit plus the idempotent defining this orbit.
 *
 * @return
 * A Boolean indicating whether all orbits are trivial.
 */
bool is_trivial_orbmono(orbits*,   //!< The orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);




/**********/
/* Groups */
/**********/

/**
 * @brief
 * Tests if a monoid is a group.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample (an element with no inverse).
 *
 * @return
 * A Boolean indicating whether the monoid is a group.
 */
bool is_group_mono(morphism*, //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a semigroup (image of A⁺) is a group.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two elements in the semigroup which are not J-equivalent.
 *
 * @return
 * A Boolean indicating whether the syntactic semigroup is a group.
 */
bool is_group_semigroup(morphism*, //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a subsemigroup is a group.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: an element with no inverse.
 *
 * @return
 * A Boolean indicating whether the subsemigroup is a group.
 */
bool is_group_subsemi(subsemi*, //!< The subsemigroup.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if all orbits are groups.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: an element with no inverse plus the idempotent defining the orbit.
 *
 * @return
 * A Boolean indicating whether all orbits are groups.
 */
bool is_group_orbmono(orbits*,    //!< The orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/**
 * @brief
 * Tests if a morphism maps all letters to the same element.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: a letter which has a distinct image than the letter of index 0.
 *
 * @return
 * A Boolean indicating whether the morphism maps all letters to the same element.
 */
bool is_letterind_mono(morphism*,    //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);



/*****************/
/* Commutativity */
/*****************/

/**
 * @brief
 * Tests is a monoid is commutative.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two elements that do not commute.
 *
 * @return
 * A Boolean indicating whether the monoid is commutative.
 */
bool is_comm_mono(morphism*, //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/**
 * @brief
 * Tests is a subsemigroup is commutative.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two elements that do not commute.
 *
 * @return
 * A Boolean indicating whether the subsemigroup is commutative.
 */
bool is_comm_subsemi(subsemi*, //!< The subsemigroup.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if all orbits are commutative.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two elements that do not commute plus the idempotent defining the orbit.
 *
 * @return
 * A Boolean indicating whether the orbits are commuatative.
 */
bool is_com_orbmono(orbits*,        //!< The orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a morphism satisfies the specific commutativity equation of
 * the class LTT.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements r,s,t,e,f that do not satisfy the equation.
 *
 * @return
 * A Boolean indicating whether the equation is satisfied.
 */
bool is_comm_ltt_mono(orbits*,     //!< The DD-orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/***************/
/* Idempotence */
/***************/

/**
 * @brief
 * Tests if a monoid is idempotent.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: an element that is not idempotent.
 *
 * @return
 * A Boolean indicating whether the monoid is idempotent.
 */
bool is_idem_mono(morphism*, //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a subsemigroup is idempotent.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: an element that is not idempotent.
 *
 * @return
 * A Boolean indicating whether the subsemigroup is idempotent.
 */
bool is_idem_subsemi(subsemi*, //!< The subsemigroup.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/**
 * @brief
 * Tests if all orbits are simultaneously commutative and idempotent.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: an element that is not idempotent plus the idempotent defining the orbit.
 *
 * @return
 * A Boolean indicating whether the orbits are simultaneously commutative and idempotent.
 */
bool is_idem_orbmono(orbits*,    //!< The orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/*******************/
/* H,R,L,J-trivial */
/*******************/


/**
 * @brief
 * Tests if a monoid is P-trivial where P is one of the four Green relations (H,R,L,J).
 *
 * @brief
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two elements that are P-equivalent (the first is idempotent).
 *
 * @return
 * A Boolean indicating whether the monoide is P-trivial.
 */
bool is_gtrivial_mono(morphism*, //!< The morphism.
    green_relation,            //!< The relation to test (H,R,L or J).
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a subsemigroup is P-trivial where P is one of the four Green relations (H,R,L,J).
 *
 * @brief
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two elements that are P-equivalent (the first is idempotent).
 *
 * @return
 * A Boolean indicating whether the subsemigroupe is P-trivial.
 */
bool is_gtrivial_subsemi(subsemi*, //!< Le subsemigroup.   
    green_relation,                //!< The relation to test (H,R,L or J).
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if all orbits are P-trivial where P is one of the four Green relations (H,R,L,J).
 *
 * @brief
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two elements that are P-equivalent (the first is idempotent) plus
 * the idempotent defining the orbit.
 *
 * @return
 * A Boolean indicating whether all orbits are P-trivial.
 */
bool is_gtrivial_orbmono(orbits*,    //!< The orbits.
    green_relation,                //!< The relation to test (H,R,L or J).
    uint* //!< Pointer on a uint array to return a counterexample.
);


/**
 * @brief
 * Tests if the H-classes of 1 and all generators are trivial.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample.
 *
 * @return
 * A Boolean indicating whether the H-classes of 1 and all generators are trivial.
 */
bool is_htrivial_generators(morphism*, //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/******/
/* DA */
/******/


/**
 * @brief
 * Tests if a monoid is in DA.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: a regular element which is not idempotent.
 *
 * @return
 * A Boolean indicating whether the monoide is in DA.
 */
bool is_da_mono(morphism*,       //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests is a subsemigroup is in DA.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: a regular element which is not idempotent.
 *
 * @return
 * A Boolean indicating whether the subsemigroupe is in DA.
 */
bool is_da_subsemi(subsemi*,      //!< The subsemigroup.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if all orbits are in DA.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: a regular element which is not idempotent plus the idempotent
 * defining the orbit.
 *
 * @return
 * A Boolean indicating whether all orbits are in DA.
 */
bool is_da_orbmono(orbits*,    //!< The orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/****************/
/* J-saturated  */
/****************/

/**
 * @brief
 * Tests if a morphism satisfies the equation 1 ≤ s.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the element s which fails the equation.
 *
 * @return
 * A Boolean indicating whether si the morphism satisfies the equation 1 ≤ s.
 */
bool is_jsat_mono(morphism*, //!< The morphism.
    dequeue*, //!< The list of elements larger than 1.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a morphism satisfies the equation 1 ≤ e (for every idempotent e).
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the idempotent e which fails the equation.
 *
 * @return
 * A Boolean indicating whether the morphisme satisfies the equation 1 ≤ e (for every idempotent e).
 */
bool is_ejsat_mono(morphism*, //!< The morphism.
    dequeue*, //!< The list of elements larger than 1.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a subsemigroup satisfies the equation 1 ≤ s.
 *
 * @remark
 * If the test fails and the third parameter is not NULL, it will be set to a
 * counterexample: the element s which fails the equation.
 *
 * @return
 * A Boolean indicating whether the subsemigroup satisfies the equation 1 ≤ s.
 */
bool is_jsat_subsemi(subsemi*,  //!< The subsemigroup.
    dequeue*, //!< The list of elements larger than 1 in the original monoid.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/**
 * @brief
 * Tests if all orbits satisfy the equation 1 ≤ s.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the element s which fails the equation plus the idempotent
 * defining the orbit.
 *
 * @return
 * A Boolean indicating whether all orbits satisfy the equation 1 ≤ s.
 */
bool is_jsat_orbmono(orbits*,   //!< The orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);





/*********/
/* Knast */
/*********/

/**
 * @brief
 * Tests if a monoid is a block group
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: two idempotents that are either L- or R-equivalent.
 *
 * @return
 * A Boolean indicating whether the monoide is a block group.
 */
bool is_blockg_mono(morphism*,   //!< The morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a morphism satisfies the BPol(MOD) equation.
 *
 * @remark
 * Takes the MOD-kernel as input. We first check if it is J-trivial as this is
 * faster.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements q,r,s,t that do not satisfy the equation.
 *
 * @return
 * A Boolean indicating whether the morphisme satisfies the BPol(MOD) equation.
 */
bool is_bpolmod_mono(subsemi*,  //!< The MOD-kernel.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/**
 * @brief
 * Tests if a morphism satisfies the BPol(AMT) equation.
 *
 * @remark
 * Takes the AMT-kernel as input. We first check if it is J-trivial as this is
 * faster.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements q,r,s,t that do not satisfy the equation.
 *
 * @return
 * A Boolean indicating whether the morphisme satisfies the BPol(AMT) equation.
 */
bool is_bpolamt_mono(subsemi*,  //!< The morphism. 
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a morphism satisfies Knast's equation.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements q,r,s,t,e,f that do not satisfy the equation.
 *
 * @return
 * A Boolean indicating whether the morphism satisfies Knast's equation.
 */
bool is_knast_mono(orbits*, //!< The DD-orbits of the morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if the G-kernel satisfies Knast's equation for a group vari G.
 *
 * @remark
 * Takes the G⁺-orbits as input. We first check that they are J-trivial as this is
 * faster.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements q,r,s,t,e,f that do not satisfy the equation.
 *
 * @return
 * A Boolean indicating whether the MOD-kernel satisfies Knast's equation.
 */
bool is_knast_ker(orbits*, //!< The G⁺-orbits.
    subsemi*,          //!< The G-kernel.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/**
 * @brief
 * Tests if a morphism satisfies the BPol(AMT⁺) equation.
 *
 * @remark
 * Takes the AMT⁺-orbits as input. We first check that they are J-trivial as this is
 * faster.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements q,r,s,t,e,f that do not satisfy the equation.
 *
 *
 * @return
 * A Boolean indicating whether the morphisme satisfies the BPol(AMT⁺) equation.
 */
bool is_bpolamtp_mono(orbits*, //!< The AMT⁺-orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);

/**
 * @brief
 * Tests if a morphism satisfies the BPol(GR⁺) equation.
 *
 * @remark
 * Takes the GR⁺-orbits as input. We first check that they are J-trivial as this is
 * faster.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements q,r,s,t,e,f that do not satisfy the equation.
 *
 * @return
 * A Boolean indicating whether the morphisme satisfies the BPol(GR⁺) equation.
 */
bool is_bpolgrp_mono(orbits*, //!< The GR⁺-orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);




/**
 * @brief
 * Tests if a morphism satisfies the AT-variant of Knast's equation.
 *
 * @remark
 * Takes the AT-orbits as input. We first check that they are J-trivial as this is
 * faster.
 *
 * @remark
 * If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements q,r,s,t,e,f that do not satisfy the equation.
 *
 * @return
 * A Boolean indicating whether si the morphism satisfies the AT-variant of Knast's equation.
 */
bool is_knast_at_mono(orbits*,   //!< The AT-orbits.
    uint* //!< Pointer on a uint array to return a counterexample.
);


/*********************/
/* UPolBPol Equation */
/*********************/

/**
 * @brief
 * Tests if a morphism satisfies the UPol(BPol(C)) equation for a given class C.
 * The C-orbits are taken as input.
 *
 * @remark
 *  If the test fails and the second parameter is not NULL, it will be set to a
 * counterexample: the elements s,t,e that do not satisfy the equation.
 *
 * @return
 * A Boolean indicating whether si the morphism satisfies the UPol(BPol(C)) equation.
 */
bool is_upbp_mono(orbits*, //!< The C-orbits of the morphism.
    uint* //!< Pointer on a uint array to return a counterexample.
);


#endif
