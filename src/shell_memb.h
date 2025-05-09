#ifndef SHELL_MEMB_H
#define SHELL_MEMB_H

#include "alloc.h"
#include "shell_sclass.h"
#include "shell_languages.h"
#include "shell_commands.h"
#include "monoid_props.h"
#include "monoid_congruences.h"
#include "nfa_enum.h"

/**
 * @brief
 * Array containing the membership functions for all classes.
 */
extern bool (*class_membership[CL_END]) (int, FILE*);

/**
 * @brief
 * Initializes the array of membership functions.
 */
void init_membership(void);

/**
 * @brief
 * Checks if testing membership for a classes requires the syntactic ordering
 *
 * @return
 * True if the membership function requires the syntactic ordering, false otherwise.
 */
bool shell_membership_needs_order(classes //!< The class to check.
);


/*********************/
/* General functions */
/*********************/

/**
 * @brief
 * Checks if the H-classes of 1 and all generators are trivial.
 *
 * @return
 * True if the H-classes of 1 and all generators are trivial, false otherwise.
 */
bool shell_morprop_htgentriv(int, //!< Index of the morphism in the objects array.
    char*, //!< Name of the object (for the display)
    FILE* //!< Output stream (NULL is no output is desired).
);




/**
 * @brief
 * Checks if a monoid is trivial.
 *
 * @return
 * True if the monoid is trivial, false otherwise.
 */
bool shell_morprop_monotriv(int, //!< Index of the morphism in the objects array.
    char*, //!< Name of the object (for the display)
    FILE* //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a semigroup is trivial.
 *
 * @return
 * True if the semigroup is trivial, false otherwise.
 */
bool shell_morprop_semitriv(int, //!< Index of the morphism in the objects array.
    char*, //!< Name of the object (for the display)
    FILE* //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a set of orbits is trivial.
 *
 * @return
 * True if the orbits are trivial, false otherwise.
 */
bool shell_morprop_orbtriv(int, //!< Index of the morphism in the objects array.
    orbits_type, //!< The orbits that need to be checked.
    char*, //!< Name of the orbits (for the display).
    char*, //!< Name of the object (for the display)
    FILE* //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Checks if a morphism maps all letters to the same element.
 *
 * @return
 * True if the morphism maps all letters to the same element, false otherwise.
 */
bool shell_morprop_letterind(int, //!< Index of the morphism in the objects array.
    char*, //!< Name of the object (for the display)
    FILE* //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a monoid is a group.
 *
 * @return
 * True if the monoid is a group, false otherwise.
 */
bool shell_morprop_monogroup(int, //!< Index of the morphism in the objects array.
    char*, //!< Name of the object (for the display)
    FILE* //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a semigroup is a group.
 *
 * @return
 * True if the msemigroup is a group, false otherwise.
 */
bool shell_morprop_semigroup(int, //!< Index of the morphism in the objects array.
    char*, //!< Name of the object (for the display)
    FILE* //!< Output stream (NULL is no output is desired).
);



/**
 * @brief
 * Checks if a monoid is a group.
 *
 * @return
 * True if the monoid is a group, false otherwise.
 */
bool shell_morprop_monocom(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a kernel is commutative.
 *
 * @return
 * True if the kernel is commutative, false otherwise.
 */
bool shell_morprop_kercom(int j, //!< Index of the morphism in the objects array.
    kernel_type type, //!< The type of kernel to check.
    char* ker, //!< The name of the kernel (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if orbits are commutative.
 *
 * @return
 * True if the orbits are commutative, false otherwise.
 */
bool shell_morprop_orbcom(int j, //!< Index of the morphism in the objects array.
    orbits_type type, //!< The type of orbits to check.
    char* orbs, //!< The name of the orbits (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a semigroup is commutative.
 *
 * @return
 * True if the semigroup is commutative, false otherwise.
 */
bool shell_morprop_semigencom(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a monoid is idempotent.
 *
 * @return
 * True if the monoid is idempotent, false otherwise.
 */
bool shell_morprop_monoidem(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a kernel is idempotent.
 *
 * @return
 * True if the kernel is idempotent, false otherwise.
 */
bool shell_morprop_keridem(int j, //!< Index of the morphism in the objects array.
    kernel_type type, //!< The type of kernel to check.
    char* ker, //!< The name of the kernel (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if orbits are idempotent.
 *
 * @return
 * True if the orbits are idempotent, false otherwise.
 */
bool shell_morprop_orbidem(int j, //!< Index of the morphism in the objects array.
    orbits_type type, //!< The type of orbits to check.
    char* orbs, //!< The name of the orbits (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a Green relation is trivial in a monoid.
 *
 * @return
 * True if the relation is trivial, false otherwise.
 */
bool shell_morprop_monogreen(int j, //!< Index of the morphism in the objects array.
    green_relation R, //!< The relation that needs to be checked.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a Green relation is trivial in a kernel.
 *
 * @return
 * True if the relation is trivial, false otherwise.
 */
bool shell_morprop_kergreen(int j, //!< Index of the morphism in the objects array.
    green_relation R, //!< The relation that needs to be checked.
    kernel_type type, //!< The type of kernel to check.
    char* ker, //!< The name of the kernel (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a Green relation is trivial in a set of orbits.
 *
 * @return
 * True if the relation is trivial for all orbits, false otherwise.
 */
bool shell_morprop_orbgreen(int j, //!< Index of the morphism in the objects array.
    green_relation R, //!< The relation that needs to be checked.
    orbits_type type, //!< The type of orbits to check.
    char* orbs, //!< The name of the orbits (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a monoid is in DA.
 *
 * @return
 * True if the monoid is in DA, false otherwise.
 */
bool shell_morprop_monoda(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a kernel is in DA.
 *
 * @return
 * True if the kernelis in DA, false otherwise.
 */
bool shell_morprop_kerda(int j, //!< Index of the morphism in the objects array.
    kernel_type type, //!< The type of kernel to check.
    char* ker, //!< The name of the kernel (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if orbits are in DA.
 *
 * @return
 * True if the orbits are all in DA, false otherwise.
 */
bool shell_morprop_orbda(int j, //!< Index of the morphism in the objects array.
    orbits_type type, //!< The type of orbits to check.
    char* orbs, //!< The name of the orbits (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a monoid satisfies the J-saturation property.
 *
 * @return
 * True if the monoid satisfies the J-saturation property, false otherwise.
 */
bool shell_morprop_monojsat(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a monoid satisfies J-saturation property restricted to idempotents.
 *
 * @return
 * True if the monoid satisfies the property, false otherwise.
 */
bool shell_morprop_monoejsat(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a kernel satisfies the J-saturation property.
 *
 * @return
 * True if the kernel satisfies the J-saturation property, false otherwise.
 */
bool shell_morprop_kerjsat(int j, //!< Index of the morphism in the objects array.
    kernel_type type, //!< The type of kernel to check.
    char* ker, //!< The name of the kernel (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if orbits satisfy the J-saturation property.
 *
 * @return
 * True if the orbits satisfy the J-saturation property, false otherwise.
 */
bool shell_morprop_orbjsat(int j, //!< Index of the morphism in the objects array.
    orbits_type type, //!< The type of orbits to check.
    char* orbs, //!< The name of the orbits (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a morphism satisfies the BPol(G)-equation.
 *
 * @return
 * True if the morphism satisfies the BPol(G)-equation, false otherwise.
 */
bool shell_morprop_bpgroupeq(int j, //!< Index of the morphism in the objects array.
    kernel_type type, //!< The type of kernel to check.
    char* ker, //!< The name of the kernel (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a monoid satisfies the block-group property.
 *
 * @return
 * True if the monoid satisfies the block-group property, false otherwise.
 */
bool shell_morprop_blockg(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a semigroup satisfies Knast's equation.
 *
 * @return
 * True if the semigroup satisfies Knast's equation, false otherwise.
 */
bool shell_mprop_semiknast(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a strict kernel satisfies Knast's equation.
 *
 * @return
 * True if the strict kernel satisfies Knast's equation, false otherwise.
 */
bool shell_morprop_kerknast(int j, //!< Index of the morphism in the objects array.
    kernel_type type, //!< The type of kernel to check.
    char* ker, //!< The name of the kernel (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a monoid satisfies Knast's equation for AT sets.
 *
 * @return
 * True if the monoid satisfies Knast's equation for AT-sets, false otherwise.
 */
bool shell_morprop_knastat(int j, //!< Index of the morphism in the objects array.
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if a kernel satisfies the BPol(G⁺)-equation.
 *
 * @return
 * True if the kernel satisfies the BPol(G⁺)-equation, false otherwise.
 */
bool shell_morprop_bpgroupeqplus(int j, //!< Index of the morphism in the objects array.
    orbits_type type, //!< The type of orbits to check.
    char* orbs, //!< The name of the orbits (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Checks if orbits satisfy the UBP2 equality property.
 *
 * @return
 * True if the orbits satisfy the UBP2 equality property, false otherwise.
 */
bool shell_morprop_ubp2eq(int j, //!< Index of the morphism in the objects array.
    orbits_type type, //!< The type of orbits to check.
    char* orbs, //!< The name of the orbits (for the display).
    char* name, //!< Name of the object (for the display)
    FILE* out //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class of all regular languages.
 *
 * @remark
 * This function always returns true.
 *
 * @return
 * True.
 */
bool shell_membership_reg(int //!< Index of the language in the objects array.
    , FILE*                   //!< Output stream (NULL is no output is desired).
);



bool shell_membership_htgen(int  //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);




/*****************/
/* Group classes */
/*****************/

/**
 * @brief
 * Membership function for the trivial class.
 *
 * @return
 * True if the language is trivial, false otherwise.
 */
bool shell_membership_st(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class DD.
 *
 * @return
 * True if the language is in DD, false otherwise.
 */
bool shell_membership_dd(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class MOD.
 *
 * @return
 * True if the language is in MOD, false otherwise.
 */
bool shell_membership_mod(int //!< Index of the language in the objects array.
    , FILE*                    //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class MOD⁺.
 *
 * @return
 * True if the language is in MOD⁺, false otherwise.
 */
bool shell_membership_modp(int //!< Index of the language in the objects array.
    , FILE*                 //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class AMT.
 *
 * @return
 * True if the language is in AMT, false otherwise.
 */
bool shell_membership_amt(int //!< Index of the language in the objects array.
    , FILE*                   //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class AMT⁺.
 *
 * @return
 * True if the language is in AMT⁺, false otherwise.
 */
bool shell_membership_amtp(int //!< Index of the language in the objects array.
    , FILE*                    //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class GR.
 *
 * @return
 * True if the language is in GR, false otherwise.
 */
bool shell_membership_gr(int  //!< Index of the language in the objects array.
    , FILE*                   //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class GR⁺.
 *
 * @return
 * True if the language is in GR⁺, false otherwise.
 */
bool shell_membership_grp(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);



/*****************/
/* AT-ATT-LT-LTT */
/*****************/


/**
 * @brief
 * Membership function for the class AT.
 *
 * @return
 * True if the language is in AT, false otherwise.
 */
bool shell_membership_at(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class ATT.
 *
 * @return
 * True if the language is in ATT, false otherwise.
 */
bool shell_membership_att(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class LT.
 *
 * @return
 * True if the language is in LT, false otherwise.
 */
bool shell_membership_lt(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class LTT.
 *
 * @return
 * True if the language is in LTT, false otherwise.
 */
bool shell_membership_ltt(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);


/*********************/
/* Star-free closure */
/*********************/

/**
 * @brief
 * Membership function for the class SF.
 *
 * @return
 * True if the language is in SF, false otherwise.
 */
bool shell_membership_sf(int //!< Index of the language in the objects array.
    , FILE*                 //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class SF(MOD).
 *
 * @return
 * True if the language is in SF(MOD), false otherwise.
 */
bool shell_membership_sfmod(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class SF(AMT).
 *
 * @return
 * True if the language is in SF(AMT), false otherwise.
 */
bool shell_membership_sfamt(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class SF(GR).
 *
 * @return
 * True if the language is in SF(GR), false otherwise.
 */
bool shell_membership_sfgr(int //!< Index of the language in the objects array.
    , FILE*                    //!< Output stream (NULL is no output is desired).
);

/**********************/
/* Polynomial closure */
/**********************/

/**
 * @brief
 * Membership function for the class PPT.
 *
 * @return
 * True if the language is in PPT, false otherwise.
 */
bool shell_membership_ppt(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(MOD).
 *
 * @return
 * True if the language is in Pol(MOD), false otherwise.
 */
bool shell_membership_polmod(int //!< Index of the language in the objects array.
    , FILE*                      //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(GR).
 *
 * @return
 * True if the language is in Pol(GR), false otherwise.
 */
bool shell_membership_polgr(int //!< Index of the language in the objects array.
    , FILE*                    //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(DD).
 *
 * @return
 * True if the language is in Pol(DD), false otherwise.
 */
bool shell_membership_poldd(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(MOD⁺).
 *
 * @return
 * True if the language is in Pol(MOD⁺), false otherwise.
 */
bool shell_membership_polmodp(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(GR⁺).
 *
 * @return
 * True if the language is in Pol(GR⁺), false otherwise.
 */
bool shell_membership_polgrp(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(ST).
 *
 * @return
 * True if the language is in Pol₂(ST), false otherwise.
 */
bool shell_membership_pol2st(int //!< Index of the language in the objects array.
    , FILE*                   //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(MOD).
 *
 * @return
 * True if the language is in Pol₂(MOD), false otherwise.
 */
bool shell_membership_pol2mod(int //!< Index of the language in the objects array.
    , FILE*                       //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(AMT).
 *
 * @return
 * True if the language is in Pol₂(AMT), false otherwise.
 */
bool shell_membership_pol2amt(int //!< Index of the language in the objects array.
    , FILE*                       //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(GR).
 *
 * @return
 * True if the language is in Pol₂(GR), false otherwise.
 */
bool shell_membership_pol2gr(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(DD).
 *
 * @return
 * True if the language is in Pol₂(DD), false otherwise.
 */
bool shell_membership_pol2dd(int //!< Index of the language in the objects array.
    , FILE*                      //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(MOD⁺).
 *
 * @return
 * True if the language is in Pol₂(MOD⁺), false otherwise.
 */
bool shell_membership_pol2modp(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(AMT⁺).
 *
 * @return
 * True if the language is in Pol₂(AMT⁺), false otherwise.
 */
bool shell_membership_pol2amtp(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(GR⁺).
 *
 * @return
 * True if the language is in Pol₂(GR⁺), false otherwise.
 */
bool shell_membership_pol2grp(int //!< Index of the language in the objects array.
    , FILE*                        //!< Output stream (NULL is no output is desired).
);

/******************************/
/* Boolean polynomial closure */
/******************************/


/**
 * @brief
 * Membership function for the class PT.
 *
 * @return
 * True if the language is in PT, false otherwise.
 */
bool shell_membership_pt(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(MOD).
 *
 * @return
 * True if the language is in BPol(MOD), false otherwise.
 */
bool shell_membership_bpolmod(int //!< Index of the language in the objects array.
    , FILE*                       //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(AMT).
 *
 * @return
 * True if the language is in BPol(AMT), false otherwise.
 */
bool shell_membership_bpolamt(int //!< Index of the language in the objects array.
    , FILE*                       //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(GR).
 *
 * @return
 * True if the language is in BPol(GR), false otherwise.
 */
bool shell_membership_bpolgr(int //!< Index of the language in the objects array.
    , FILE*                      //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(GR⁺).
 *
 * @return
 * True if the language is in BPol(GR⁺), false otherwise.
 */
bool shell_membership_bpolgrp(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(DD).
 *
 * @return
 * True if the language is in BPol(DD), false otherwise.
 */
bool shell_membership_bpoldd(int //!< Index of the language in the objects array.
    , FILE*                      //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(MOD⁺).
 *
 * @return
 * True if the language is in BPol(MOD⁺), false otherwise.
 */
bool shell_membership_bpolmodp(int //!< Index of the language in the objects array.
    , FILE*                        //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class BPol(AMT⁺).
 *
 * @return
 * True if the language is in BPol(AMT⁺), false otherwise.
 */
bool shell_membership_bpolamtp(int //!< Index of the language in the objects array.
    , FILE*                        //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol₂(ST).
 *
 * @return
 * True if the language is in BPol₂(ST), false otherwise.
 */
bool shell_membership_bpol2st(int //!< Index of the language in the objects array.
    , FILE*                       //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class JOrb(MOD).
 *
 * @return
 * True if the language is in JOrb(MOD), false otherwise.
 */
bool shell_membership_jorbmod(int //!< Index of the language in the objects array.
    , FILE*                       //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(AMT).
 *
 * @return
 * True if the language is in JOrb(AMT), false otherwise.
 */
bool shell_membership_jorbamt(int //!< Index of the language in the objects array.
    , FILE*                       //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(DD).
 *
 * @return
 * True if the language is in JOrb(DD), false otherwise.
 */
bool shell_membership_jorbdd(int //!< Index of the language in the objects array.
    , FILE*                      //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(MOD⁺).
 *
 * @return
 * True if the language is in JOrb(MOD⁺), false otherwise.
 */
bool shell_membership_jorbmodp(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(AMT⁺).
 *
 * @return
 * True if the language is in JOrb(AMT⁺), false otherwise.
 */
bool shell_membership_jorbamtp(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(GR⁺).
 *
 * @return
 * True if the language is in JOrb(GR⁺), false otherwise.
 */
bool shell_membership_jorbgrp(int //!< Index of the language in the objects array.
    , FILE*                          //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(AT).
 *
 * @return
 * True if the language is in JOrb(AT), false otherwise.
 */
bool shell_membership_jorbat(int //!< Index of the language in the objects array.
    , FILE*                     //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class KNASTAT.
 *
 * @return
 * True if the language is in KNASTAT, false otherwise.
 */
bool shell_membership_knastat(int //!< Index of the language in the objects array.
    , FILE*                       //!< Output stream (NULL is no output is desired).
);

/**********************************/
/* Unambiguous polynomial closure */
/**********************************/

/**
 * @brief
 * Membership function for the class UPol(DD).
 *
 * @return
 * True if the language is in UPol(DD), false otherwise.
 */
bool shell_membership_upoldd(int //!< Index of the language in the objects array.
    , FILE*                      //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(MOD⁺).
 *
 * @return
 * True if the language is in UPol(MOD⁺), false otherwise.
 */
bool shell_membership_upolmodp(int //!< Index of the language in the objects array.
    , FILE*                        //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(AMT⁺).
 *
 * @return
 * True if the language is in UPol(AMT⁺), false otherwise.
 */
bool shell_membership_upolamtp(int //!< Index of the language in the objects array.
    , FILE*                        //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(GR⁺).
 *
 * @return
 * True if the language is in UPol(GR⁺), false otherwise.
 */
bool shell_membership_upolgrp(int //!< Index of the language in the objects array.
    , FILE*                          //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(ST)).
 *
 * @return
 * True if the language is in UPol(BPol₂(ST)), false otherwise.
 */
bool shell_membership_ubpol2st(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(MOD)).
 *
 * @return
 * True if the language is in UPol(BPol₂(MOD)), false otherwise.
 */
bool shell_membership_ubpol2mod(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(AMT)).
 *
 * @return
 * True if the language is in UPol(BPol₂(AMT)), false otherwise.
 */
bool shell_membership_ubpol2amt(int //!< Index of the language in the objects array.
    , FILE*                         //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(GR)).
 *
 * @return
 * True if the language is in UPol(BPol₂(GR)), false otherwise.
 */
bool shell_membership_ubpol2gr(int //!< Index of the language in the objects array.
    , FILE*                        //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(DD)).
 *
 * @return
 * True if the language is in UPol(BPol₂(DD)), false otherwise.
 */
bool shell_membership_ubpol2dd(int //!< Index of the language in the objects array.
    , FILE*                        //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(MOD⁺)).
 *
 * @return
 * True if the language is in UPol(BPol₂(MOD⁺), false otherwise.
 */
bool shell_membership_ubpol2modp(int //!< Index of the language in the objects array.
    , FILE*                          //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(AMT⁺)).
 *
 * @return
 * True if the language is in UPol(BPol₂(AMT⁺), false otherwise.
 */
bool shell_membership_ubpol2amtp(int //!< Index of the language in the objects array.
    , FILE*                          //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(GR⁺)).
 *
 * @return
 * True if the language is in UPol(BPol₂(GR⁺), false otherwise.
 */
bool shell_membership_ubpol2grp(int //!< Index of the language in the objects array.
    , FILE*                            //!< Output stream (NULL is no output is desired).
);

/******************/
/* Temporal logic */
/******************/

/**
 * @brief
 * Membership function for the class UL.
 *
 * @return
 * True if the language is in UL, false otherwise.
 */
bool shell_membership_ul(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(MOD).
 *
 * @return
 * True if the language is in TL(MOD), false otherwise.
 */
bool shell_membership_tlmod(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(AMT).
 *
 * @return
 * True if the language is in TL(AMT), false otherwise.
 */
bool shell_membership_tlamt(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(GR).
 *
 * @return
 * True if the language is in TL(GR), false otherwise.
 */
bool shell_membership_tlgr(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(DD).
 *
 * @return
 * True if the language is in TL(DD), false otherwise.
 */
bool shell_membership_tldd(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(MOD⁺).
 *
 * @return
 * True if the language is in TL(MOD⁺), false otherwise.
 */
bool shell_membership_tlmodp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(AMT⁺).
 *
 * @return
 * True if the language is in TL(AMT⁺), false otherwise.
 */
bool shell_membership_tlamtp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(GR⁺).
 *
 * @return
 * True if the language is in TL(GR⁺), false otherwise.
 */
bool shell_membership_tlgrp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class TL₂(ST).
 *
 * @return
 * True if the language is in TL₂(ST), false otherwise.
 */
bool shell_membership_tl2st(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(MOD).
 *
 * @return
 * True if the language is in TL₂(MOD), false otherwise.
 */
bool shell_membership_tl2mod(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(AMT).
 *
 * @return
 * True if the language is in TL₂(AMT), false otherwise.
 */
bool shell_membership_tl2amt(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(GR).
 *
 * @return
 * True if the language is in TL₂(GR), false otherwise.
 */
bool shell_membership_tl2gr(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(DD).
 *
 * @return
 * True if the language is in TL₂(DD), false otherwise.
 */
bool shell_membership_tl2dd(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(MOD⁺).
 *
 * @return
 * True if the language is in TL₂(MOD⁺), false otherwise.
 */
bool shell_membership_tl2modp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(AMT⁺).
 *
 * @return
 * True if the language is in TL₂(AMT⁺), false otherwise.
 */
bool shell_membership_tl2amtp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(GR⁺).
 *
 * @return
 * True if the language is in TL₂(GR⁺), false otherwise.
 */
bool shell_membership_tl2grp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL.
 *
 * @return
 * True if the language is in FL, false otherwise.
 */
bool shell_membership_fl(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(MOD).
 *
 * @return
 * True if the language is in FL(MOD), false otherwise.
 */
bool shell_membership_flmod(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(AMT).
 *
 * @return
 * True if the language is in FL(AMT), false otherwise.
 */
bool shell_membership_flamt(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(GR).
 *
 * @return
 * True if the language is in FL(GR), false otherwise.
 */
bool shell_membership_flgr(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(DD).
 *
 * @return
 * True if the language is in FL(DD), false otherwise.
 */
bool shell_membership_fldd(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(MOD⁺).
 *
 * @return
 * True if the language is in FL(MOD⁺), false otherwise.
 */
bool shell_membership_flmodp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(AMT⁺).
 *
 * @return
 * True if the language is in FL(AMT⁺), false otherwise.
 */
bool shell_membership_flamtp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(GR⁺).
 *
 * @return
 * True if the language is in FL(GR⁺), false otherwise.
 */
bool shell_membership_flgrp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(ST).
 *
 * @return
 * True if the language is in FL₂(ST), false otherwise.
 */
bool shell_membership_fl2st(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(MOD).
 *
 * @return
 * True if the language is in FL₂(MOD), false otherwise.
 */
bool shell_membership_fl2mod(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(AMT).
 *
 * @return
 * True if the language is in FL₂(AMT), false otherwise.
 */
bool shell_membership_fl2amt(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(GR).
 *
 * @return
 * True if the language is in FL₂(GR), false otherwise.
 */
bool shell_membership_fl2gr(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(DD).
 *
 * @return
 * True if the language is in FL₂(DD), false otherwise.
 */
bool shell_membership_fl2dd(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(MOD⁺).
 *
 * @return
 * True if the language is in FL₂(MOD⁺), false otherwise.
 */
bool shell_membership_fl2modp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(AMT⁺).
 *
 * @return
 * True if the language is in FL₂(AMT⁺), false otherwise.
 */
bool shell_membership_fl2amtp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(GR⁺).
 *
 * @return
 * True if the language is in FL₂(GR⁺), false otherwise.
 */
bool shell_membership_fl2grp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL.
 *
 * @return
 * True if the language is in PL, false otherwise.
 */
bool shell_membership_pl(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(MOD).
 *
 * @return
 * True if the language is in PL(MOD), false otherwise.
 */
bool shell_membership_plmod(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(AMT).
 *
 * @return
 * True if the language is in PL(AMT), false otherwise.
 */
bool shell_membership_plamt(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(GR).
 *
 * @return
 * True if the language is in PL(GR), false otherwise.
 */
bool shell_membership_plgr(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(DD).
 *
 * @return
 * True if the language is in PL(DD), false otherwise.
 */
bool shell_membership_pldd(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(MOD⁺).
 *
 * @return
 * True if the language is in PL(MOD⁺), false otherwise.
 */
bool shell_membership_plmodp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(AMT⁺).
 *
 * @return
 * True if the language is in PL(AMT⁺), false otherwise.
 */
bool shell_membership_plamtp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(GR⁺).
 *
 * @return
 * True if the language is in PL(GR⁺), false otherwise.
 */
bool shell_membership_plgrp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(ST).
 *
 * @return
 * True if the language is in PL₂(ST), false otherwise.
 */
bool shell_membership_pl2st(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(MOD).
 *
 * @return
 * True if the language is in PL₂(MOD), false otherwise.
 */
bool shell_membership_pl2mod(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(AMT).
 *
 * @return
 * True if the language is in PL₂(AMT), false otherwise.
 */
bool shell_membership_pl2amt(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(GR).
 *
 * @return
 * True if the language is in PL₂(GR), false otherwise.
 */
bool shell_membership_pl2gr(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(DD).
 *
 * @return
 * True if the language is in PL₂(DD), false otherwise.
 */
bool shell_membership_pl2dd(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(MOD⁺).
 *
 * @return
 * True if the language is in PL₂(MOD⁺), false otherwise.
 */
bool shell_membership_pl2modp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(AMT⁺).
 *
 * @return
 * True if the language is in PL₂(AMT⁺), false otherwise.
 */
bool shell_membership_pl2amtp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(GR⁺).
 *
 * @return
 * True if the language is in PL₂(GR⁺), false otherwise.
 */
bool shell_membership_pl2grp(int //!< Index of the language in the objects array.
    , FILE*                  //!< Output stream (NULL is no output is desired).
);




/***********************/
/* Global procedures */
/***********************/

/**
 * @brief
 * Generic membership function called by the shell.
 *
 * @return
 * True if the language is in the class, false otherwise.
 */
bool shell_membership(com_parameters* //!< Command parameters (should include the class and the input language).
);

/**
 * @brief
 * Summary of all available tests for concatenation hierarchies.
 */
bool shell_chiera_summary(com_parameters* //!< Command parameters (should include the input language).
);


/*****************************/
/* Deterministic hierarchies */
/*****************************/



/**
 * @brief
 * Computes the level of a language inside a negation hierarchy.
 */
bool shell_neghiera(com_parameters* //!< Command parameters (should include the base class and the input language).
);

/**
 * @brief
 * Computes the level of a language inside a future/past hierarchy.
 */
bool shell_fphiera(com_parameters* //!< Command parameters (should include the base class and the input language).
);


/***********************/
/* Examples generators */
/***********************/

/**
 * @brief
 * Searches for all DFAs of a given size satisfying a given set of membership constraints.
 */
bool shell_exall(com_parameters* //!< Command parameters (should include the size of the DFAs and the membership constraints).
);

/**
 * @brief
 * Searches for random DFAs satisfying a given set of membership constraints.
 */
bool shell_exsearch(com_parameters* //!< Command parameters (should include the size of the DFAs and the membership constraints).
);

bool shell_exall_dethiera(com_parameters* pars, bool neg);



#endif
