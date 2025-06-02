/**
 * @file shell_morprops.h
 * @brief Shell functions testing properties of morphisms.
 *
 * @attention
 * All functions must be called on indices of the objects array that refer to morphisms.
 * This is not checked in the functions.
 */


#ifndef SHELL_MORPROPS_H
#define SHELL_MORPROPS_H

#include "alloc.h"
#include "shell_languages.h"
#include "monoid_props.h"


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



#endif // SHELL_MORPROPS_H
