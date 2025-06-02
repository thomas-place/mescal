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





/**************************************/
/* Membership functions for languages */
/**************************************/


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
bool shell_membership_reg(int, //!< Index of the language in the objects array.
    FILE*                   //!< Output stream (NULL is no output is desired).
);



bool shell_membership_htgen(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
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
bool shell_membership_st(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class DD.
 *
 * @return
 * True if the language is in DD, false otherwise.
 */
bool shell_membership_dd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class MOD.
 *
 * @return
 * True if the language is in MOD, false otherwise.
 */
bool shell_membership_mod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class MOD⁺.
 *
 * @return
 * True if the language is in MOD⁺, false otherwise.
 */
bool shell_membership_modp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class AMT.
 *
 * @return
 * True if the language is in AMT, false otherwise.
 */
bool shell_membership_amt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class AMT⁺.
 *
 * @return
 * True if the language is in AMT⁺, false otherwise.
 */
bool shell_membership_amtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class GR.
 *
 * @return
 * True if the language is in GR, false otherwise.
 */
bool shell_membership_gr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class GR⁺.
 *
 * @return
 * True if the language is in GR⁺, false otherwise.
 */
bool shell_membership_grp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
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
bool shell_membership_at(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class ATT.
 *
 * @return
 * True if the language is in ATT, false otherwise.
 */
bool shell_membership_att(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class LT.
 *
 * @return
 * True if the language is in LT, false otherwise.
 */
bool shell_membership_lt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class LTT.
 *
 * @return
 * True if the language is in LTT, false otherwise.
 */
bool shell_membership_ltt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
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
bool shell_membership_sf(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class SF(MOD).
 *
 * @return
 * True if the language is in SF(MOD), false otherwise.
 */
bool shell_membership_sfmod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class SF(AMT).
 *
 * @return
 * True if the language is in SF(AMT), false otherwise.
 */
bool shell_membership_sfamt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class SF(GR).
 *
 * @return
 * True if the language is in SF(GR), false otherwise.
 */
bool shell_membership_sfgr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
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
bool shell_membership_ppt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(MOD).
 *
 * @return
 * True if the language is in Pol(MOD), false otherwise.
 */
bool shell_membership_polmod(int, //!< Index of the language in the objects array.
    FILE*                      //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(GR).
 *
 * @return
 * True if the language is in Pol(GR), false otherwise.
 */
bool shell_membership_polgr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(DD).
 *
 * @return
 * True if the language is in Pol(DD), false otherwise.
 */
bool shell_membership_poldd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(MOD⁺).
 *
 * @return
 * True if the language is in Pol(MOD⁺), false otherwise.
 */
bool shell_membership_polmodp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol(GR⁺).
 *
 * @return
 * True if the language is in Pol(GR⁺), false otherwise.
 */
bool shell_membership_polgrp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(ST).
 *
 * @return
 * True if the language is in Pol₂(ST), false otherwise.
 */
bool shell_membership_pol2st(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(MOD).
 *
 * @return
 * True if the language is in Pol₂(MOD), false otherwise.
 */
bool shell_membership_pol2mod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(AMT).
 *
 * @return
 * True if the language is in Pol₂(AMT), false otherwise.
 */
bool shell_membership_pol2amt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(GR).
 *
 * @return
 * True if the language is in Pol₂(GR), false otherwise.
 */
bool shell_membership_pol2gr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(DD).
 *
 * @return
 * True if the language is in Pol₂(DD), false otherwise.
 */
bool shell_membership_pol2dd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(MOD⁺).
 *
 * @return
 * True if the language is in Pol₂(MOD⁺), false otherwise.
 */
bool shell_membership_pol2modp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(AMT⁺).
 *
 * @return
 * True if the language is in Pol₂(AMT⁺), false otherwise.
 */
bool shell_membership_pol2amtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class Pol₂(GR⁺).
 *
 * @return
 * True if the language is in Pol₂(GR⁺), false otherwise.
 */
bool shell_membership_pol2grp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
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
bool shell_membership_pt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(MOD).
 *
 * @return
 * True if the language is in BPol(MOD), false otherwise.
 */
bool shell_membership_bpolmod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(AMT).
 *
 * @return
 * True if the language is in BPol(AMT), false otherwise.
 */
bool shell_membership_bpolamt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(GR).
 *
 * @return
 * True if the language is in BPol(GR), false otherwise.
 */
bool shell_membership_bpolgr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(GR⁺).
 *
 * @return
 * True if the language is in BPol(GR⁺), false otherwise.
 */
bool shell_membership_bpolgrp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(DD).
 *
 * @return
 * True if the language is in BPol(DD), false otherwise.
 */
bool shell_membership_bpoldd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol(MOD⁺).
 *
 * @return
 * True if the language is in BPol(MOD⁺), false otherwise.
 */
bool shell_membership_bpolmodp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class BPol(AMT⁺).
 *
 * @return
 * True if the language is in BPol(AMT⁺), false otherwise.
 */
bool shell_membership_bpolamtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class BPol₂(ST).
 *
 * @return
 * True if the language is in BPol₂(ST), false otherwise.
 */
bool shell_membership_bpol2st(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class JOrb(MOD).
 *
 * @return
 * True if the language is in JOrb(MOD), false otherwise.
 */
bool shell_membership_jorbmod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(AMT).
 *
 * @return
 * True if the language is in JOrb(AMT), false otherwise.
 */
bool shell_membership_jorbamt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(DD).
 *
 * @return
 * True if the language is in JOrb(DD), false otherwise.
 */
bool shell_membership_jorbdd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(MOD⁺).
 *
 * @return
 * True if the language is in JOrb(MOD⁺), false otherwise.
 */
bool shell_membership_jorbmodp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(AMT⁺).
 *
 * @return
 * True if the language is in JOrb(AMT⁺), false otherwise.
 */
bool shell_membership_jorbamtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(GR⁺).
 *
 * @return
 * True if the language is in JOrb(GR⁺), false otherwise.
 */
bool shell_membership_jorbgrp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class JOrb(AT).
 *
 * @return
 * True if the language is in JOrb(AT), false otherwise.
 */
bool shell_membership_jorbat(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class KNASTAT.
 *
 * @return
 * True if the language is in KNASTAT, false otherwise.
 */
bool shell_membership_knastat(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
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
bool shell_membership_upoldd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(MOD⁺).
 *
 * @return
 * True if the language is in UPol(MOD⁺), false otherwise.
 */
bool shell_membership_upolmodp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(AMT⁺).
 *
 * @return
 * True if the language is in UPol(AMT⁺), false otherwise.
 */
bool shell_membership_upolamtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(GR⁺).
 *
 * @return
 * True if the language is in UPol(GR⁺), false otherwise.
 */
bool shell_membership_upolgrp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(ST)).
 *
 * @return
 * True if the language is in UPol(BPol₂(ST)), false otherwise.
 */
bool shell_membership_ubpol2st(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(MOD)).
 *
 * @return
 * True if the language is in UPol(BPol₂(MOD)), false otherwise.
 */
bool shell_membership_ubpol2mod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(AMT)).
 *
 * @return
 * True if the language is in UPol(BPol₂(AMT)), false otherwise.
 */
bool shell_membership_ubpol2amt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(GR)).
 *
 * @return
 * True if the language is in UPol(BPol₂(GR)), false otherwise.
 */
bool shell_membership_ubpol2gr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(DD)).
 *
 * @return
 * True if the language is in UPol(BPol₂(DD)), false otherwise.
 */
bool shell_membership_ubpol2dd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(MOD⁺)).
 *
 * @return
 * True if the language is in UPol(BPol₂(MOD⁺), false otherwise.
 */
bool shell_membership_ubpol2modp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(AMT⁺)).
 *
 * @return
 * True if the language is in UPol(BPol₂(AMT⁺), false otherwise.
 */
bool shell_membership_ubpol2amtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class UPol(BPol₂(GR⁺)).
 *
 * @return
 * True if the language is in UPol(BPol₂(GR⁺), false otherwise.
 */
bool shell_membership_ubpol2grp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
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
bool shell_membership_ul(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(MOD).
 *
 * @return
 * True if the language is in TL(MOD), false otherwise.
 */
bool shell_membership_tlmod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(AMT).
 *
 * @return
 * True if the language is in TL(AMT), false otherwise.
 */
bool shell_membership_tlamt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(GR).
 *
 * @return
 * True if the language is in TL(GR), false otherwise.
 */
bool shell_membership_tlgr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(DD).
 *
 * @return
 * True if the language is in TL(DD), false otherwise.
 */
bool shell_membership_tldd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(MOD⁺).
 *
 * @return
 * True if the language is in TL(MOD⁺), false otherwise.
 */
bool shell_membership_tlmodp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(AMT⁺).
 *
 * @return
 * True if the language is in TL(AMT⁺), false otherwise.
 */
bool shell_membership_tlamtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL(GR⁺).
 *
 * @return
 * True if the language is in TL(GR⁺), false otherwise.
 */
bool shell_membership_tlgrp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);


/**
 * @brief
 * Membership function for the class TL₂(ST).
 *
 * @return
 * True if the language is in TL₂(ST), false otherwise.
 */
bool shell_membership_tl2st(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(MOD).
 *
 * @return
 * True if the language is in TL₂(MOD), false otherwise.
 */
bool shell_membership_tl2mod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(AMT).
 *
 * @return
 * True if the language is in TL₂(AMT), false otherwise.
 */
bool shell_membership_tl2amt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(GR).
 *
 * @return
 * True if the language is in TL₂(GR), false otherwise.
 */
bool shell_membership_tl2gr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(DD).
 *
 * @return
 * True if the language is in TL₂(DD), false otherwise.
 */
bool shell_membership_tl2dd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(MOD⁺).
 *
 * @return
 * True if the language is in TL₂(MOD⁺), false otherwise.
 */
bool shell_membership_tl2modp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(AMT⁺).
 *
 * @return
 * True if the language is in TL₂(AMT⁺), false otherwise.
 */
bool shell_membership_tl2amtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class TL₂(GR⁺).
 *
 * @return
 * True if the language is in TL₂(GR⁺), false otherwise.
 */
bool shell_membership_tl2grp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL.
 *
 * @return
 * True if the language is in FL, false otherwise.
 */
bool shell_membership_fl(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(MOD).
 *
 * @return
 * True if the language is in FL(MOD), false otherwise.
 */
bool shell_membership_flmod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(AMT).
 *
 * @return
 * True if the language is in FL(AMT), false otherwise.
 */
bool shell_membership_flamt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(GR).
 *
 * @return
 * True if the language is in FL(GR), false otherwise.
 */
bool shell_membership_flgr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(DD).
 *
 * @return
 * True if the language is in FL(DD), false otherwise.
 */
bool shell_membership_fldd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(MOD⁺).
 *
 * @return
 * True if the language is in FL(MOD⁺), false otherwise.
 */
bool shell_membership_flmodp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(AMT⁺).
 *
 * @return
 * True if the language is in FL(AMT⁺), false otherwise.
 */
bool shell_membership_flamtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL(GR⁺).
 *
 * @return
 * True if the language is in FL(GR⁺), false otherwise.
 */
bool shell_membership_flgrp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(ST).
 *
 * @return
 * True if the language is in FL₂(ST), false otherwise.
 */
bool shell_membership_fl2st(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(MOD).
 *
 * @return
 * True if the language is in FL₂(MOD), false otherwise.
 */
bool shell_membership_fl2mod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(AMT).
 *
 * @return
 * True if the language is in FL₂(AMT), false otherwise.
 */
bool shell_membership_fl2amt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(GR).
 *
 * @return
 * True if the language is in FL₂(GR), false otherwise.
 */
bool shell_membership_fl2gr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(DD).
 *
 * @return
 * True if the language is in FL₂(DD), false otherwise.
 */
bool shell_membership_fl2dd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(MOD⁺).
 *
 * @return
 * True if the language is in FL₂(MOD⁺), false otherwise.
 */
bool shell_membership_fl2modp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(AMT⁺).
 *
 * @return
 * True if the language is in FL₂(AMT⁺), false otherwise.
 */
bool shell_membership_fl2amtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class FL₂(GR⁺).
 *
 * @return
 * True if the language is in FL₂(GR⁺), false otherwise.
 */
bool shell_membership_fl2grp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL.
 *
 * @return
 * True if the language is in PL, false otherwise.
 */
bool shell_membership_pl(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(MOD).
 *
 * @return
 * True if the language is in PL(MOD), false otherwise.
 */
bool shell_membership_plmod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(AMT).
 *
 * @return
 * True if the language is in PL(AMT), false otherwise.
 */
bool shell_membership_plamt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(GR).
 *
 * @return
 * True if the language is in PL(GR), false otherwise.
 */
bool shell_membership_plgr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(DD).
 *
 * @return
 * True if the language is in PL(DD), false otherwise.
 */
bool shell_membership_pldd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(MOD⁺).
 *
 * @return
 * True if the language is in PL(MOD⁺), false otherwise.
 */
bool shell_membership_plmodp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(AMT⁺).
 *
 * @return
 * True if the language is in PL(AMT⁺), false otherwise.
 */
bool shell_membership_plamtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL(GR⁺).
 *
 * @return
 * True if the language is in PL(GR⁺), false otherwise.
 */
bool shell_membership_plgrp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(ST).
 *
 * @return
 * True if the language is in PL₂(ST), false otherwise.
 */
bool shell_membership_pl2st(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(MOD).
 *
 * @return
 * True if the language is in PL₂(MOD), false otherwise.
 */
bool shell_membership_pl2mod(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(AMT).
 *
 * @return
 * True if the language is in PL₂(AMT), false otherwise.
 */
bool shell_membership_pl2amt(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(GR).
 *
 * @return
 * True if the language is in PL₂(GR), false otherwise.
 */
bool shell_membership_pl2gr(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(DD).
 *
 * @return
 * True if the language is in PL₂(DD), false otherwise.
 */
bool shell_membership_pl2dd(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(MOD⁺).
 *
 * @return
 * True if the language is in PL₂(MOD⁺), false otherwise.
 */
bool shell_membership_pl2modp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(AMT⁺).
 *
 * @return
 * True if the language is in PL₂(AMT⁺), false otherwise.
 */
bool shell_membership_pl2amtp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Membership function for the class PL₂(GR⁺).
 *
 * @return
 * True if the language is in PL₂(GR⁺), false otherwise.
 */
bool shell_membership_pl2grp(int,  //!< Index of the language in the objects array.
    FILE*                  //!< Output stream (NULL is no output is desired).
);




/***************/
/* Hierarchies */
/***************/



/**
 * @brief
 * Summary of all available tests for concatenation hierarchies.
 */
void shell_chiera_summary(int i, //!< Index of the language in the objects array.
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Computes the level of a language inside a negation hierarchy.
 */
bool shell_neghiera(classes cl, //!< Base class of the hierarchy.
    int i, //!< Index of the language in the objects array.
    FILE* out //!< Output stream (NULL is no output is desired).
);

/**
 * @brief
 * Computes the level of a language inside a future/past hierarchy.
 */
bool shell_fphiera(classes cl, //!< Base class of the hierarchy.
    int i, //!< Index of the language in the objects array.
    FILE* out //!< Output stream (NULL is no output is desired).
);


/***********************/
/* Examples generators */
/***********************/

/**
 * @brief
 * Searches for all DFAs of a given size satisfying a given set of membership constraints.
 */
void shell_exall(classes* low,  //!< Classes for negative tests.
    int nblow, //!< Number of classes for negative tests.
    classes* high, //!< Classes for positive tests.
    int nbhigh, //!< Number of classes for positive tests.
    int states, //!< Number of states of the DFAs to be generated.
    int alpha //!< Size of the alphabet of the DFAs to be generated.
);

/**
 * @brief
 * Searches for all DFAs of a given size at a given level in a negation or future/past hierarchy.
 */
void shell_exall_dethiera(classes cl, //!< Base class of the hierarchy.
    int level, //!< Desired level for the examples.
    int states, //!< Number of states of the DFAs to be generated.
    int alpha, //!< Size of the alphabet of the DFAs to be generated.
    bool neg //!< True if the hierarchy is negation-based, false if it is future/past-based.
);


// /**
//  * @brief
//  * Searches for random DFAs satisfying a given set of membership constraints.
//  */
// bool shell_exsearch(com_parameters* //!< Command parameters (should include the size of the DFAs and the membership constraints).
// );

bool shell_exall_dfatest(void);


#endif
