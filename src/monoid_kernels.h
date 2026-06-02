/**
 * @file monoid_kernels.h
 * @brief
 * Computation of kernels.
 */

#ifndef MONOID_KERNELS_H
#define MONOID_KERNELS_H

/*  _  __                    _      */
/* | |/ /___ _ __ _ __   ___| |___  */
/* | ' // _ \ '__| '_ \ / _ \ / __| */
/* | . \  __/ |  | | | |  __/ \__ \ */
/* |_|\_\___|_|  |_| |_|\___|_|___/ */

#include "flint/fmpz.h"
#include "flint/fmpz_mat.h"
#include "monoid.h"
#include "monoid_sub.h"
#include "sep_group.h"

// #define DEBUG_KERNEL

// /**
//  * @brief
//  * Computes a DFA from the right Cayley graph of a morphism. Discards all
//  * transitions which are not internal to the R-classes.
//  *
//  * @remark
//  * Used to compute the restriction of Kernels to regular elements.
//  *
//  * @return
//  * The DFA.
//  */
// nfa *morphism_to_dfa_rcl(morphism * //!< The morphism.
// );

// /**
//  * @brief
//  * Computes a DFA from the left Cayley graph of a morphism. Discards all
//  * transitions which are not internal to the L-classes.
//  *
//  * @remark
//  * Used to compute the restriction of Kernels to regular elements.
//  *
//  * @return
//  * The DFA.
//  */
// nfa *morphism_to_dfa_lcl(morphism * //!< The morphism.
// );

/**
 * @brief
 * Computes the kernel of a morphism for one of the three bases MOD, AMT or GR.
 *
 * @remark
 * The available computation levels are LV_REG and LV_FULL. If LV_GREG is used,
 * the computation defaults to LV_FULL.
 *
 * @attention
 * LV_FULL is not available for AMT-kernels. If used, the computation default to LV_REG.
 *
 * @return
 * The kernel.
 */
subsemi *get_kernel(morphism *M,     //!< The morphism.
                    sub_level level, //!< The desired computation level of the subsemigroup.
                    basis ba         //!< The basis of the kernel (BA_MOD, BA_AMT or BA_GR). If BA_ST is used, an error is raised.
);

#endif
