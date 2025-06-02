#include "shell_memb.h"
#include "shell_morprops.h"
#include "shell_autoprops.h"
#include "type_hash.h"
#include "monoid_display.h"

bool (*class_membership[CL_END])(int, FILE*) = { NULL };

void init_membership(void) {

    class_membership[CL_HTGEN] = shell_membership_htgen;

    // Star-free
    class_membership[CL_SF] = shell_membership_sf;
    class_membership[CL_SFMOD] = shell_membership_sfmod;
    class_membership[CL_SFAMT] = shell_membership_sfamt;
    class_membership[CL_SFGR] = shell_membership_sfgr;

    // Polynomial closure
    class_membership[CL_PPT] = shell_membership_ppt;
    class_membership[CL_POLMOD] = shell_membership_polmod;
    class_membership[CL_POLAMT] = NULL;
    class_membership[CL_POLGR] = shell_membership_polgr;
    class_membership[CL_POLDD] = shell_membership_poldd;
    class_membership[CL_POLMODP] = shell_membership_polmodp;
    class_membership[CL_POLAMTP] = NULL;
    class_membership[CL_POLGRP] = shell_membership_polgrp;
    class_membership[CL_POL2ST] = shell_membership_pol2st;
    class_membership[CL_POL2MOD] = shell_membership_pol2mod;
    class_membership[CL_POL2AMT] = shell_membership_pol2amt;
    class_membership[CL_POL2GR] = shell_membership_pol2gr;
    class_membership[CL_POL2DD] = shell_membership_pol2dd;
    class_membership[CL_POL2MODP] = shell_membership_pol2modp;
    class_membership[CL_POL2AMTP] = shell_membership_pol2amtp;
    class_membership[CL_POL2GRP] = shell_membership_pol2grp;

    // Boolean Polynomial closure
    class_membership[CL_PT] = shell_membership_pt;
    class_membership[CL_BPOLMOD] = shell_membership_bpolmod;
    class_membership[CL_BPOLAMT] = shell_membership_bpolamt;
    class_membership[CL_BPOLGR] = shell_membership_bpolgr;
    class_membership[CL_BPOLDD] = shell_membership_bpoldd;
    class_membership[CL_BPOLMODP] = shell_membership_bpolmodp;
    class_membership[CL_BPOLAMTP] = shell_membership_bpolamtp;
    class_membership[CL_BPOLGRP] = shell_membership_bpolgrp;
    class_membership[CL_BPOL2ST] = shell_membership_bpol2st;
    class_membership[CL_BPOL2MOD] = NULL;
    class_membership[CL_BPOL2AMT] = NULL;
    class_membership[CL_BPOL2GR] = NULL;
    class_membership[CL_BPOL2DD] = NULL;
    class_membership[CL_BPOL2MODP] = NULL;
    class_membership[CL_BPOL2AMTP] = NULL;
    class_membership[CL_BPOL2GRP] = NULL;

    class_membership[CL_JORBMOD] = shell_membership_jorbmod;
    class_membership[CL_JORBAMT] = shell_membership_jorbamt;
    class_membership[CL_JORBMODP] = shell_membership_jorbmodp;
    class_membership[CL_JORBAMTP] = shell_membership_jorbamtp;
    class_membership[CL_JORBGRP] = shell_membership_jorbgrp;
    class_membership[CL_JORBDD] = shell_membership_jorbdd;
    class_membership[CL_JORBAT] = shell_membership_jorbat;

    class_membership[CL_KNASTAT] = shell_membership_knastat;

    // Unambiguous Polynomial closure
    class_membership[CL_UPOLDD] = shell_membership_upoldd;
    class_membership[CL_UPOLMODP] = shell_membership_upolmodp;
    class_membership[CL_UPOLAMTP] = shell_membership_upolamtp;
    class_membership[CL_UPOLGRP] = shell_membership_upolgrp;

    class_membership[CL_UBPOL2ST] = shell_membership_ubpol2st;
    class_membership[CL_UBPOL2MOD] = shell_membership_ubpol2mod;
    class_membership[CL_UBPOL2AMT] = shell_membership_ubpol2amt;
    class_membership[CL_UBPOL2GR] = shell_membership_ubpol2gr;
    class_membership[CL_UBPOL2DD] = shell_membership_ubpol2dd;
    class_membership[CL_UBPOL2MODP] = shell_membership_ubpol2modp;
    class_membership[CL_UBPOL2AMTP] = shell_membership_ubpol2amtp;
    class_membership[CL_UBPOL2GRP] = shell_membership_ubpol2grp;

    // UTL closure
    class_membership[CL_UL] = shell_membership_ul;
    class_membership[CL_TLMOD] = shell_membership_tlmod;
    class_membership[CL_TLAMT] = shell_membership_tlamt;
    class_membership[CL_TLGR] = shell_membership_tlgr;
    class_membership[CL_TLDD] = shell_membership_tldd;
    class_membership[CL_TLMODP] = shell_membership_tlmodp;
    class_membership[CL_TLAMTP] = shell_membership_tlamtp;
    class_membership[CL_TLGRP] = shell_membership_tlgrp;
    class_membership[CL_TL2ST] = shell_membership_tl2st;
    class_membership[CL_TL2MOD] = shell_membership_tl2mod;
    class_membership[CL_TL2AMT] = shell_membership_tl2amt;
    class_membership[CL_TL2GR] = shell_membership_tl2gr;
    class_membership[CL_TL2DD] = shell_membership_tl2dd;
    class_membership[CL_TL2MODP] = shell_membership_tl2modp;
    class_membership[CL_TL2AMTP] = shell_membership_tl2amtp;
    class_membership[CL_TL2GRP] = shell_membership_tl2grp;

    // FTL closure
    class_membership[CL_FLST] = shell_membership_fl;
    class_membership[CL_FLMOD] = shell_membership_flmod;
    class_membership[CL_FLAMT] = shell_membership_flamt;
    class_membership[CL_FLGR] = shell_membership_flgr;
    class_membership[CL_FLDD] = shell_membership_fldd;
    class_membership[CL_FLMODP] = shell_membership_flmodp;
    class_membership[CL_FLAMTP] = shell_membership_flamtp;
    class_membership[CL_FLGRP] = shell_membership_flgrp;
    class_membership[CL_FL2ST] = shell_membership_fl2st;
    class_membership[CL_FL2MOD] = shell_membership_fl2mod;
    class_membership[CL_FL2AMT] = shell_membership_fl2amt;
    class_membership[CL_FL2GR] = shell_membership_fl2gr;
    class_membership[CL_FL2DD] = shell_membership_fl2dd;
    class_membership[CL_FL2MODP] = shell_membership_fl2modp;
    class_membership[CL_FL2AMTP] = shell_membership_fl2amtp;
    class_membership[CL_FL2GRP] = shell_membership_fl2grp;

    // PTL closure
    class_membership[CL_PLST] = shell_membership_pl;
    class_membership[CL_PLMOD] = shell_membership_plmod;
    class_membership[CL_PLAMT] = shell_membership_plamt;
    class_membership[CL_PLGR] = shell_membership_plgr;
    class_membership[CL_PLDD] = shell_membership_pldd;
    class_membership[CL_PLMODP] = shell_membership_plmodp;
    class_membership[CL_PLAMTP] = shell_membership_pl2amtp;
    class_membership[CL_PLGRP] = shell_membership_plgrp;
    class_membership[CL_PL2ST] = shell_membership_pl2st;
    class_membership[CL_PL2MOD] = shell_membership_pl2mod;
    class_membership[CL_PL2AMT] = shell_membership_pl2amt;
    class_membership[CL_PL2GR] = shell_membership_pl2gr;
    class_membership[CL_PL2DD] = shell_membership_pl2dd;
    class_membership[CL_PL2MODP] = shell_membership_pl2modp;
    class_membership[CL_PL2AMTP] = shell_membership_pl2amtp;
    class_membership[CL_PL2GRP] = shell_membership_pl2grp;

    // Single classes
    class_membership[CL_AT] = shell_membership_at;
    class_membership[CL_ATT] = shell_membership_att;
    class_membership[CL_LT] = shell_membership_lt;
    class_membership[CL_LTT] = shell_membership_ltt;
    class_membership[CL_ST] = shell_membership_st;
    class_membership[CL_DD] = shell_membership_dd;
    class_membership[CL_MOD] = shell_membership_mod;
    class_membership[CL_MODP] = shell_membership_modp;
    class_membership[CL_AMT] = shell_membership_amt;
    class_membership[CL_AMTP] = shell_membership_amtp;
    class_membership[CL_GR] = shell_membership_gr;
    class_membership[CL_GRP] = shell_membership_grp;
    class_membership[CL_REG] = shell_membership_reg;
}

bool shell_membership_needs_order(classes cl) {
    switch (cl)
    {
    case CL_PPT:
    case CL_POLMOD:
    case CL_POLAMT:
    case CL_POLGR:
    case CL_POLDD:
    case CL_POLMODP:
    case CL_POLAMTP:
    case CL_POLGRP:
    case CL_POL2ST:
    case CL_POL2MOD:
    case CL_POL2AMT:
    case CL_POL2GR:
    case CL_POL2DD:
    case CL_POL2MODP:
    case CL_POL2AMTP:
    case CL_POL2GRP:
        return true;
        break;
    default:
        return false;
        break;
    }
}


// static void shell_print_syntac(int j, FILE* out) {
//     print_title_box(10, true, stdout, 1, "The syntactic morphism.");
//     shell_view_object(objects[shell_compute_syntac(j, false)], false);
// }

// static void shell_print_mini(int j, FILE* out) {
//     print_title_box(10, true, stdout, 1, "The minimal automaton.");
//     shell_view_object(objects[shell_compute_minimal(j)], false);
// }


/*********************/
/* General functions */
/*********************/



bool shell_membership_reg(int, FILE*) { return true; }

bool shell_membership_htgen(int j, FILE* out) {
    return shell_morprop_htgentriv(j, "syntactic monoid", out);
}

/*****************/
/* Group classes */
/*****************/

bool shell_membership_st(int j, FILE* out) {
    return shell_morprop_monotriv(shell_compute_syntac(j, false), "syntactic monoid", out);
}

bool shell_membership_dd(int j, FILE* out) {
    return shell_morprop_semitriv(shell_compute_syntac(j, false), "syntactic semigroup", out);
}

bool shell_membership_mod(int j, FILE* out) {
    return shell_morprop_letterind(shell_compute_syntac(j, false), "syntactic morphism", out) && shell_morprop_monogroup(shell_compute_syntac(j, false), "syntactic monoid", out);
}

bool shell_membership_modp(int j, FILE* out) {
    return shell_morprop_letterind(shell_compute_syntac(j, false), "syntactic morphism", out) && shell_morprop_semigroup(shell_compute_syntac(j, false), "syntactic semigroup", out);
}

bool shell_membership_amt(int j, FILE* out) {
    return shell_morprop_monocom(shell_compute_syntac(j, false), "syntactic monoid", out) && shell_morprop_monogroup(shell_compute_syntac(j, false), "syntactic monoid", out);
}

bool shell_membership_amtp(int j, FILE* out) {
    return shell_morprop_monocom(shell_compute_syntac(j, false), "syntactic semigroup", out) && shell_morprop_semigroup(shell_compute_syntac(j, false), "syntactic semigroup", out);
}

bool shell_membership_gr(int j, FILE* out) {
    return shell_morprop_monogroup(shell_compute_syntac(j, false), "syntactic monoid", out);
}

bool shell_membership_grp(int j, FILE* out) {
    return shell_morprop_semigroup(shell_compute_syntac(j, false), "syntactic semigroup", out);
    return true;
}

/*****************/
/* AT-ATT-LT-LTT */
/*****************/

bool shell_membership_at(int j, FILE* out) {
    return shell_morprop_monocom(shell_compute_syntac(j, false), "syntactic monoid", out) && shell_morprop_monoidem(shell_compute_syntac(j, false), "syntactic monoid", out);
}

bool shell_membership_att(int j, FILE* out) {
    return shell_morprop_monocom(shell_compute_syntac(j, false), "syntactic monoid", out) && shell_morprop_monogreen(shell_compute_syntac(j, false), H_GREEN, "syntactic monoid", out);
}

bool shell_membership_lt(int j, FILE* out) {
    return shell_morprop_orbcom(shell_compute_syntac(j, false), ORB_DD, "DD", "syntactic morphism", out) && shell_morprop_orbidem(shell_compute_syntac(j, false), ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_ltt(int j, FILE* out) {
    return shell_morprop_monogreen(shell_compute_syntac(j, false), H_GREEN, "syntactic semigroup", out) && shell_morprop_semigencom(shell_compute_syntac(j, false), "syntactic semigroup", out);
}

/*********************/
/* Star-free closure */
/*********************/

bool shell_membership_sf(int j, FILE* out) {
    return shell_morprop_monogreen(shell_compute_syntac(j, false), H_GREEN, "syntactic monoid", out);
}

bool shell_membership_sfmod(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), H_GREEN, KER_MOD, "MOD", "syntactic monoid", out);
}

bool shell_membership_sfamt(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), H_GREEN, KER_AMT, "AMT", "syntactic monoid", out);
}

bool shell_membership_sfgr(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), H_GREEN, KER_GR, "GR", "syntactic monoid", out);
}

/**********************/
/* Polynomial closure */
/**********************/

bool shell_membership_ppt(int j, FILE* out) {
    return shell_morprop_monojsat(shell_compute_syntac(j, true), "syntactic monoid", out);
}

bool shell_membership_polmod(int j, FILE* out) {
    return shell_morprop_kerjsat(shell_compute_syntac(j, true), KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_polgr(int j, FILE* out) {
    return shell_morprop_monoejsat(shell_compute_syntac(j, true), "syntactic monoid", out);
}



bool shell_membership_poldd(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_polmodp(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_polgrp(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_pol2st(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_PT, "PT", "syntactic morphism", out);
}

bool shell_membership_pol2mod(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_BPMOD, "BPol(MOD)", "syntactic morphism", out);
}

bool shell_membership_pol2amt(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_BPAMT, "BPol(AMT)", "syntactic morphism", out);
}

bool shell_membership_pol2gr(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_BPGR, "BPol(GR)", "syntactic morphism", out);
}

bool shell_membership_pol2dd(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_BPDD, "BPol(DD)", "syntactic morphism", out);
}

bool shell_membership_pol2modp(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_BPMODP, "BPol(MOD⁺)", "syntactic morphism", out);
}

bool shell_membership_pol2amtp(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_BPAMTP, "BPol(AMT⁺)", "syntactic morphism", out);
}

bool shell_membership_pol2grp(int j, FILE* out) {
    return shell_morprop_orbjsat(shell_compute_syntac(j, true), ORB_BPGRP, "BPol(GR⁺)", "syntactic morphism", out);
}

/******************************/
/* Boolean polynomial closure */
/******************************/

bool shell_membership_pt(int j, FILE* out) {
    return shell_morprop_monogreen(shell_compute_syntac(j, false), J_GREEN, "syntactic monoid", out);
}

bool shell_membership_bpolmod(int j, FILE* out) {
    return shell_morprop_bpgroupeq(shell_compute_syntac(j, false), KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_bpolamt(int j, FILE* out) {
    return shell_morprop_bpgroupeq(shell_compute_syntac(j, false), KER_AMT, "AMT", "syntactic morphism", out);
}

bool shell_membership_bpolgr(int j, FILE* out) {
    return shell_morprop_blockg(shell_compute_syntac(j, false), "syntactic monoid", out);
}

bool shell_membership_bpoldd(int j, FILE* out) {
    return shell_mprop_semiknast(shell_compute_syntac(j, false), "syntactic semigroup", out);
}

bool shell_membership_bpolmodp(int j, FILE* out) {
    return shell_morprop_kerknast(shell_compute_syntac(j, false), KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_bpolamtp(int j, FILE* out) {
    return shell_morprop_bpgroupeqplus(shell_compute_syntac(j, false), ORB_AMTP, "AMT", "syntactic morphism", out);
}


bool shell_membership_bpolgrp(int j, FILE* out) {
    return shell_morprop_bpgroupeqplus(shell_compute_syntac(j, false), ORB_GRP, "GR", "syntactic morphism", out);
}

bool shell_membership_bpol2st(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_PT, "PT", "syntactic morphism", out) && shell_morprop_knastat(shell_compute_syntac(j, false), "syntactic morphism", out);
}

bool shell_membership_jorbmod(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), J_GREEN, KER_MOD, "MOD", "syntactic monoid", out);
}

bool shell_membership_jorbamt(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), J_GREEN, KER_AMT, "AMT", "syntactic monoid", out);
}

bool shell_membership_jorbmodp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), J_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_jorbamtp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), J_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_jorbgrp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), J_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_jorbdd(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), J_GREEN, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_jorbat(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), J_GREEN, ORB_PT, "PT", "syntactic morphism", out);
}

bool shell_membership_knastat(int j, FILE* out) {
    return shell_morprop_knastat(shell_compute_syntac(j, false), "syntactic morphism", out);
}

/**********************************/
/* Unambiguous polynomial closure */
/**********************************/

bool shell_membership_upoldd(int j, FILE* out) {
    return shell_morprop_orbtriv(shell_compute_syntac(j, false), ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_upolmodp(int j, FILE* out) {
    return shell_morprop_orbtriv(shell_compute_syntac(j, false), ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_upolamtp(int j, FILE* out) {
    return shell_morprop_orbtriv(shell_compute_syntac(j, false), ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_upolgrp(int j, FILE* out) {
    return shell_morprop_orbtriv(shell_compute_syntac(j, false), ORB_GRP, "GR⁺", "syntactic morphism", out);
}


bool shell_membership_ubpol2st(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_PT, "PT", "syntactic morphism", out);
}

bool shell_membership_ubpol2mod(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_BPMOD, "BPol(MOD)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2amt(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_BPAMT, "BPol(AMT)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2gr(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_BPGR, "BPol(GR)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2dd(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_BPDD, "BPol(DD)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2modp(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_BPMODP, "BPol(MOD⁺)-orbit", "syntactic morphism", out);
}


bool shell_membership_ubpol2amtp(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_BPAMTP, "BPol(AMT⁺)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2grp(int j, FILE* out) {
    return shell_morprop_ubp2eq(shell_compute_syntac(j, false), ORB_BPGRP, "BPol(GR⁺)-orbit", "syntactic morphism", out);
}

/******************/
/* Temporal logic */
/******************/

bool shell_membership_ul(int j, FILE* out) {
    if (objects[j]->type == MORPHISM) {
        return shell_morprop_monoda(shell_compute_syntac(j, false), "syntactic monoid", out);
    }
    else {
        int error = 0;
        return shell_autoprop_dapat(shell_compute_minimal(j), "minimal automaton", &error, out);
    }
}

bool shell_membership_tlmod(int j, FILE* out) {
    return shell_morprop_kerda(shell_compute_syntac(j, false), KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_tlamt(int j, FILE* out) {
    return shell_morprop_kerda(shell_compute_syntac(j, false), KER_AMT, "AMT", "syntactic morphism", out);
}

bool shell_membership_tlgr(int j, FILE* out) {
    return shell_morprop_kerda(shell_compute_syntac(j, false), KER_GR, "GR", "syntactic morphism", out);
}

bool shell_membership_tldd(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_tlmodp(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_tlamtp(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_tlgrp(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_tl2st(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_PT, "TL(ST)", "syntactic morphism", out);
}

bool shell_membership_tl2mod(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_BPMOD, "TL(MOD)", "syntactic morphism", out);
}

bool shell_membership_tl2amt(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_BPAMT, "TL(AMT)", "syntactic morphism", out);
}

bool shell_membership_tl2gr(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_BPGR, "TL(GR)", "syntactic morphism", out);
}

bool shell_membership_tl2dd(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_BPDD, "TL(DD)", "syntactic morphism", out);
}

bool shell_membership_tl2modp(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_BPMODP, "TL(MOD⁺)", "syntactic morphism", out);
}

bool shell_membership_tl2amtp(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_BPAMTP, "TL(AMT⁺)", "syntactic morphism", out);
}

bool shell_membership_tl2grp(int j, FILE* out) {
    return shell_morprop_orbda(shell_compute_syntac(j, false), ORB_BPGRP, "TL(GR⁺)", "syntactic morphism", out);
}


bool shell_membership_fl(int j, FILE* out) {
    return shell_morprop_monogreen(shell_compute_syntac(j, false), L_GREEN, "syntactic monoid", out);
}

bool shell_membership_flmod(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), L_GREEN, KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_flamt(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), L_GREEN, KER_AMT, "AMT", "syntactic morphism", out);
}

bool shell_membership_flgr(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), L_GREEN, KER_GR, "GR", "syntactic morphism", out);
}

bool shell_membership_fldd(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_flmodp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_flamtp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_flgrp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_fl2st(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_PT, "FL(ST)", "syntactic morphism", out);
}

bool shell_membership_fl2mod(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_BPMOD, "FL(MOD)", "syntactic morphism", out);
}

bool shell_membership_fl2amt(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_BPAMT, "FL(AMT)", "syntactic morphism", out);
}

bool shell_membership_fl2gr(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_BPGR, "FL(GR)", "syntactic morphism", out);
}

bool shell_membership_fl2dd(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_BPDD, "FL(DD)", "syntactic morphism", out);
}

bool shell_membership_fl2modp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_BPMODP, "FL(MOD⁺)", "syntactic morphism", out);
}

bool shell_membership_fl2amtp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_BPAMTP, "FL(AMT⁺)", "syntactic morphism", out);
}

bool shell_membership_fl2grp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), L_GREEN, ORB_BPGRP, "FL(GR⁺)", "syntactic morphism", out);
}

bool shell_membership_pl(int j, FILE* out) {
    return shell_morprop_monogreen(shell_compute_syntac(j, false), R_GREEN, "syntactic monoid", out);
}

bool shell_membership_plmod(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), R_GREEN, KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_plamt(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), R_GREEN, KER_AMT, "AMT", "syntactic morphism", out);
}

bool shell_membership_plgr(int j, FILE* out) {
    return shell_morprop_kergreen(shell_compute_syntac(j, false), R_GREEN, KER_GR, "GR", "syntactic morphism", out);
}

bool shell_membership_pldd(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_plmodp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_plamtp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_plgrp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_pl2st(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_PT, "PL(ST)", "syntactic morphism", out);
}

bool shell_membership_pl2mod(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_BPMOD, "PL(MOD)", "syntactic morphism", out);
}

bool shell_membership_pl2amt(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_BPAMT, "PL(AMT)", "syntactic morphism", out);
}

bool shell_membership_pl2gr(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_BPGR, "PL(GR)", "syntactic morphism", out);
}

bool shell_membership_pl2dd(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_BPDD, "PL(DD)", "syntactic morphism", out);
}

bool shell_membership_pl2modp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_BPMODP, "PL(MOD⁺)", "syntactic morphism", out);
}

bool shell_membership_pl2amtp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_BPAMTP, "PL(AMT⁺)", "syntactic morphism", out);
}

bool shell_membership_pl2grp(int j, FILE* out) {
    return shell_morprop_orbgreen(shell_compute_syntac(j, false), R_GREEN, ORB_BPGRP, "PL(GR⁺)", "syntactic morphism", out);
}



/********************************/
/* Summary for all main classes */
/********************************/

typedef enum {
    ANS_YES,
    ANS_NO,
    ANS_UNKNOWN
} ans_type;

enum {
    CLT_BASIS = 0,
    CLT_POL,
    CLT_BPOL,
    CLT_TL,
    CLT_POL2,
    CLT_BPOL2,
    CLT_UBPOL2,
    CLT_TL2,
    CLT_SF,
    CLT_SIZE
};

enum {
    BSI_ST = 0,
    BSI_DD,
    BSI_MOD,
    BSI_MODP,
    BSI_AMT,
    BSI_AMTP,
    BSI_GR,
    BSI_GRP,
    BSI_SIZE
};

static void populate_table_star(int j, int op, int bs, bool (*fun) (int, FILE*), ans_type res[CLT_SIZE][BSI_SIZE]) {
    if (res[op][bs] != ANS_UNKNOWN) {
        return;
    }
    bool val = fun(j, NULL);
    if (val) {
        for (int g = op; g < CLT_SIZE; g++) {
            for (int h = bs; h < BSI_SIZE; h++) {
                res[g][h] = ANS_YES;
            }
        }

    }
    else {
        for (int g = op; g >= 0; g--) {
            for (int h = bs; h >= 0; h -= 2) {
                res[g][h] = ANS_NO;
            }
        }
    }
}

static void populate_table_plus(int j, int op, int bs, bool (*fun) (int, FILE*), ans_type res[CLT_SIZE][BSI_SIZE]) {
    if (res[op][bs] != ANS_UNKNOWN) {
        return;
    }
    bool val = fun(j, NULL);
    if (val) {
        for (int g = op; g < CLT_SIZE; g++) {
            for (int h = bs; h < BSI_SIZE; h += 2) {
                res[g][h] = ANS_YES;
            }
        }

    }
    else {
        for (int g = op; g >= 0; g--) {
            for (int h = bs; h >= 0; h--) {
                res[g][h] = ANS_NO;
            }
        }
    }
}

static void populate_line_from_sf(int bs, ans_type ans, ans_type res[CLT_SIZE][BSI_SIZE]) {
    if (ans == ANS_YES) {
        res[CLT_SF][bs] = ANS_YES;
        return;
    }
    for (int g = 0; g < CLT_SIZE; g++) {
        res[g][bs] = ANS_NO;
    }
}



static void summary_print_answer(ans_type res, FILE* out) {
    switch (res) {
    case ANS_YES:
        fprintf(out, "║" ANSI_COLOR_GREEN "     YES      " ANSI_COLOR_RESET);
        break;
    case ANS_NO:
        fprintf(out, "║" ANSI_COLOR_RED "     NO       " ANSI_COLOR_RESET);
        break;
    case ANS_UNKNOWN:
        fprintf(out, "║" ANSI_COLOR_YELLOW "    UNKNOWN   " ANSI_COLOR_RESET);
        break;
    default:
        break;
    }
}

void shell_chiera_summary(int i, FILE* out) {
    int j = shell_compute_syntac(i, true);

    if (j < 0) {
        return;
    }

    ans_type res[CLT_SIZE][BSI_SIZE];

    for (int k = 0; k < CLT_SIZE; k++) {
        for (int l = 0; l < BSI_SIZE; l++) {
            res[k][l] = ANS_UNKNOWN;
        }
    }

    /************/
    /* Basis ST */
    /************/

    populate_table_star(j, CLT_SF, BSI_ST, shell_membership_sf, res);
    populate_table_star(j, CLT_BASIS, BSI_ST, shell_membership_st, res);
    populate_table_star(j, CLT_BPOL, BSI_ST, shell_membership_pt, res);
    populate_table_star(j, CLT_POL, BSI_ST, shell_membership_ppt, res);
    populate_table_star(j, CLT_TL, BSI_ST, shell_membership_ul, res);
    populate_table_star(j, CLT_TL2, BSI_ST, shell_membership_tl2st, res);
    populate_table_star(j, CLT_UBPOL2, BSI_ST, shell_membership_ubpol2st, res);
    populate_table_star(j, CLT_BPOL2, BSI_ST, shell_membership_bpol2st, res);
    populate_table_star(j, CLT_POL2, BSI_ST, shell_membership_pol2st, res);


    fprintf(out, "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(out, "║                                          Concatenation hierarchies: membership tests summary                                         ║\n");
    fprintf(out, "╠══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╣\n");
    fprintf(out, "║  Basis: ST   ║    Pol(ST)   ║   BPol(ST)   ║    TL(ST)    ║   Pol₂(ST)   ║   BPol₂(ST)  ║  UBPol₂(ST)  ║   TL₂(ST)    ║    SF(ST)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(out, "║             ");
    for (uint h = 0; h < CLT_SIZE;h++) {
        summary_print_answer(res[h][BSI_ST], out);
    }
    fprintf(out, "║\n");

    /************/
    /* Basis DD */
    /************/

    populate_line_from_sf(BSI_DD, res[CLT_SF][BSI_ST], res);

    populate_table_plus(j, CLT_BASIS, BSI_DD, shell_membership_dd, res);
    populate_table_plus(j, CLT_BPOL, BSI_DD, shell_membership_bpoldd, res);
    populate_table_plus(j, CLT_POL, BSI_DD, shell_membership_poldd, res);
    populate_table_plus(j, CLT_TL, BSI_DD, shell_membership_tldd, res);
    populate_table_plus(j, CLT_TL2, BSI_DD, shell_membership_tl2dd, res);
    populate_table_plus(j, CLT_UBPOL2, BSI_DD, shell_membership_ubpol2dd, res);
    populate_table_plus(j, CLT_POL2, BSI_DD, shell_membership_pol2dd, res);

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: DD   ║    Pol(DD)   ║   BPol(DD)   ║    TL(DD)    ║   Pol₂(DD)   ║   BPol₂(DD)  ║  UBPol₂(DD)  ║   TL₂(DD)    ║    SF(DD)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < CLT_SIZE;h++) {
        summary_print_answer(res[h][BSI_DD], out);
    }
    fprintf(out, "║\n");

    /*************/
    /* Basis MOD */
    /*************/

    populate_table_star(j, CLT_SF, BSI_MOD, shell_membership_sfmod, res);
    populate_table_star(j, CLT_BASIS, BSI_MOD, shell_membership_mod, res);
    populate_table_star(j, CLT_BPOL, BSI_MOD, shell_membership_bpolmod, res);
    populate_table_star(j, CLT_POL, BSI_MOD, shell_membership_polmod, res);
    populate_table_star(j, CLT_TL, BSI_MOD, shell_membership_tlmod, res);
    populate_table_star(j, CLT_TL2, BSI_MOD, shell_membership_tl2mod, res);
    populate_table_star(j, CLT_UBPOL2, BSI_MOD, shell_membership_ubpol2mod, res);
    populate_table_star(j, CLT_POL2, BSI_MOD, shell_membership_pol2mod, res);

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: MOD  ║   Pol(MOD)   ║  BPol(MOD)   ║   TL(MOD)    ║  Pol₂(MOD)   ║  BPol₂(MOD)  ║ UBPol₂(MOD)  ║  TL₂(MOD)    ║   SF(MOD)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < CLT_SIZE;h++) {
        summary_print_answer(res[h][BSI_MOD], out);
    }

    fprintf(out, "║\n");
    /**************/
    /* Basis MODP */
    /**************/
    populate_line_from_sf(BSI_MODP, res[CLT_SF][BSI_MOD], res);
    populate_table_plus(j, CLT_BASIS, BSI_MODP, shell_membership_modp, res);
    populate_table_plus(j, CLT_BPOL, BSI_MODP, shell_membership_bpolmodp, res);
    populate_table_plus(j, CLT_POL, BSI_MODP, shell_membership_polmodp, res);
    populate_table_plus(j, CLT_TL, BSI_MODP, shell_membership_tlmodp, res);
    populate_table_plus(j, CLT_TL2, BSI_MODP, shell_membership_tl2modp, res);
    populate_table_plus(j, CLT_UBPOL2, BSI_MODP, shell_membership_ubpol2modp, res);
    populate_table_plus(j, CLT_POL2, BSI_MODP, shell_membership_pol2modp, res);

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: MOD⁺ ║   Pol(MOD⁺)  ║  BPol(MOD⁺)  ║   TL(MOD⁺)   ║  Pol₂(MOD⁺)  ║  BPol₂(MOD⁺) ║ UBPol₂(MOD⁺) ║  TL₂(MOD⁺)   ║   SF(MOD⁺)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < CLT_SIZE;h++) {
        summary_print_answer(res[h][BSI_MODP], out);
    }

    fprintf(out, "║\n");

    /*************/
    /* Basis AMT */
    /*************/

    populate_table_star(j, CLT_SF, BSI_AMT, shell_membership_sfamt, res);
    populate_table_star(j, CLT_BASIS, BSI_AMT, shell_membership_amt, res);
    populate_table_star(j, CLT_BPOL, BSI_AMT, shell_membership_bpolamt, res);
    populate_table_star(j, CLT_TL, BSI_AMT, shell_membership_tlamt, res);
    populate_table_star(j, CLT_TL2, BSI_AMT, shell_membership_tl2amt, res);
    populate_table_star(j, CLT_UBPOL2, BSI_AMT, shell_membership_ubpol2amt, res);
    populate_table_star(j, CLT_POL2, BSI_AMT, shell_membership_pol2amt, res);



    /**************/
    /* Basis AMTP */
    /**************/

    populate_line_from_sf(BSI_AMTP, res[CLT_SF][BSI_AMT], res);
    populate_table_plus(j, CLT_BASIS, BSI_AMTP, shell_membership_amtp, res);
    populate_table_plus(j, CLT_BPOL, BSI_AMTP, shell_membership_bpolamtp, res);
    populate_table_plus(j, CLT_TL, BSI_AMTP, shell_membership_tlamtp, res);
    populate_table_plus(j, CLT_TL2, BSI_AMTP, shell_membership_tl2amtp, res);
    populate_table_plus(j, CLT_UBPOL2, BSI_AMTP, shell_membership_ubpol2amtp, res);
    populate_table_plus(j, CLT_POL2, BSI_AMTP, shell_membership_pol2amtp, res);


    /************/
    /* Basis GR */
    /************/


    populate_table_star(j, CLT_SF, BSI_GR, shell_membership_sfgr, res);
    populate_table_star(j, CLT_BASIS, BSI_GR, shell_membership_gr, res);
    populate_table_star(j, CLT_BPOL, BSI_GR, shell_membership_bpolgr, res);
    populate_table_star(j, CLT_POL, BSI_GR, shell_membership_polgr, res);
    populate_table_star(j, CLT_TL, BSI_GR, shell_membership_tlgr, res);
    populate_table_star(j, CLT_TL2, BSI_GR, shell_membership_tl2gr, res);
    populate_table_star(j, CLT_UBPOL2, BSI_GR, shell_membership_ubpol2gr, res);
    populate_table_star(j, CLT_POL2, BSI_GR, shell_membership_pol2gr, res);



    /*************/
   /* Basis GRP */
   /*************/

    populate_line_from_sf(BSI_GRP, res[CLT_SF][BSI_GR], res);
    populate_table_plus(j, CLT_BASIS, BSI_GRP, shell_membership_grp, res);
    populate_table_plus(j, CLT_BPOL, BSI_GRP, shell_membership_bpolgrp, res);
    populate_table_plus(j, CLT_POL, BSI_GRP, shell_membership_polgrp, res);
    populate_table_plus(j, CLT_TL, BSI_GRP, shell_membership_tlgrp, res);
    populate_table_plus(j, CLT_TL2, BSI_GRP, shell_membership_tl2grp, res);
    populate_table_plus(j, CLT_UBPOL2, BSI_GRP, shell_membership_ubpol2grp, res);
    populate_table_plus(j, CLT_POL2, BSI_GRP, shell_membership_pol2grp, res);

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: AMT  ║   Pol(AMT)   ║  BPol(AMT)   ║   TL(AMT)    ║  Pol₂(AMT)   ║  BPol₂(AMT)  ║ UBPol₂(AMT)  ║  TL₂(AMT)    ║   SF(AMT)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < CLT_SIZE;h++) {
        summary_print_answer(res[h][BSI_AMT], out);
    }

    fprintf(out, "║\n");

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: AMT⁺ ║   Pol(AMT⁺)  ║  BPol(AMT⁺)  ║   TL(AMT⁺)   ║  Pol₂(AMT⁺)  ║  BPol₂(AMT⁺) ║ UBPol₂(AMT⁺) ║  TL₂(AMT⁺)   ║   SF(AMT⁺)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < CLT_SIZE;h++) {
        summary_print_answer(res[h][BSI_AMTP], out);
    }

    fprintf(out, "║\n");


    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: GR   ║    Pol(GR)   ║   BPol(GR)   ║    TL(GR)    ║   Pol₂(GR)   ║   BPol₂(GR)  ║  UBPol₂(GR)  ║   TL₂(GR)    ║    SF(GR)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    for (uint h = 0; h < CLT_SIZE;h++) {
        summary_print_answer(res[h][BSI_GR], out);
    }

    fprintf(out, "║\n");

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: GR⁺  ║    Pol(GR⁺)  ║   BPol(GR⁺)  ║    TL(GR⁺)   ║   Pol₂(GR⁺)  ║  BPol₂(GR⁺)  ║  UBPol₂(GR⁺) ║   TL₂(GR⁺)   ║    SF(GR⁺)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(out, "║             ");
    for (uint h = 0; h < CLT_SIZE;h++) {
        summary_print_answer(res[h][BSI_GRP], out);
    }

    fprintf(out, "║\n");

    fprintf(out, "╚══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╝\n");

    return;
}


static short shell_neghiera_aux(int i, classes cl) {
    ufind* theuf = NULL;

    int j = 0;
    switch (cl) {
    case CL_ST:
        if (!shell_membership_ul(i, NULL)) {
            return -1;
        }
        j = shell_compute_syntac(i, false);
        theuf = iden_green_mono(objects[j]->mor->obj, J_GREEN);
        break;
    case CL_MOD:
        if (!shell_membership_tlmod(i, NULL)) {
            return -1;
        }
        j = shell_compute_syntac(i, false);
        theuf = iden_bpolmod_mono(objects[j]->mor->obj);
        break;
    case CL_AMT:
        if (!shell_membership_tlamt(i, NULL)) {
            return -1;
        }
        j = shell_compute_syntac(i, false);
        theuf = iden_bpolamt_mono(objects[j]->mor->obj);
        break;
    case CL_GR:
        if (!shell_membership_tlgr(i, NULL)) {
            return -1;
        }
        j = shell_compute_syntac(i, false);
        theuf = iden_blockg_mono(objects[j]->mor->obj);
        break;
    case CL_DD:
        if (!shell_membership_tldd(i, NULL)) {
            return -1;
        }
        j = shell_compute_syntac(i, false);
        theuf = iden_knast_mono(shell_compute_orbits(j, ORB_DD, LV_REG));
        break;
    case CL_MODP:
        if (!shell_membership_tlmodp(i, NULL)) {
            return -1;
        }
        j = shell_compute_syntac(i, false);
        theuf = iden_qknast_mono(shell_compute_orbits(j, ORB_MODP, LV_REG), shell_compute_ker(j, KER_MOD, LV_REG));
        break;
    case CL_AMTP:
        if (!shell_membership_tlamtp(i, NULL)) {
            return -1;
        }
        j = shell_compute_syntac(i, false);
        theuf = iden_bpolamtp_mono(shell_compute_orbits(j, ORB_AMTP, LV_REG));
        break;
    case CL_GRP:
        if (!shell_membership_tlgrp(i, NULL)) {
            return -1;
        }
        j = shell_compute_syntac(i, false);
        theuf = iden_bpolgrp_mono(shell_compute_orbits(j, ORB_GRP, LV_REG));
        break;
    default:
        return -1;
        break;
    }


    short level = 1;
    parti* thepr;
    while (true) {
        if (theuf->size_par == theuf->size_set) {
            break;
        }
        level++;
        thepr = ufind_to_parti(theuf);
        delete_ufind(theuf);
        theuf = iden_mpolc_mono(objects[j]->mor->obj, thepr);
        delete_parti(thepr);
    }
    delete_ufind(theuf);
    return level;
}


bool shell_neghiera(classes cl, int i, FILE* out) {



    if (i == -1) {
        return false;
    }

    if (!class_is_basis(cl)) {
        fprintf(out, "#### The class %s is not a valid input class for negation hierarchies.\n", class_names[cl]);
        return false;
    }

    print_infooper_neghiera(cl, out);
    print_info_input(i, out);

    fprintf(out, "#### Checking if the input belongs to TL(%s).\n", class_names[cl]);
    fprintf(out, "#### If so, computing the least level n such that the input belongs to TLⁿ(%s).\n", class_names[cl]);
    fprintf(out, "#### The algorithm is based on the characterizations of TL(%s), BPol(%s) and MPol.\n", class_names[cl], class_names[cl]);

    short level = shell_neghiera_aux(i, cl);
    char message[150];
    if (level == -1) {
        sprintf(message, "The input does not belong to TL(%s).", class_names[cl]);
    }
    else {
        char supscript[20];
        sprint_power_utf8(level, supscript);
        sprintf(message, "The least level containing the input is TL%s(%s).", supscript, class_names[cl]);
    }


    print_dtitle_box(100, true, out, 1, message);

    return true;
}

static void shell_fphiera_aux(int i, classes cl, short* fllv, short* pllv) {
    *fllv = -1;
    *pllv = -1;


    ufind* pluf = NULL;
    ufind* fluf = NULL;

    int j;
    switch (cl) {
    case CL_ST:
        if (!shell_membership_ul(i, NULL)) {
            return;
        }

        j = shell_compute_syntac(i, false);
        fluf = iden_green_mono(objects[j]->mor->obj, L_GREEN);
        pluf = iden_green_mono(objects[j]->mor->obj, R_GREEN);
        break;
    case CL_MOD:
        if (!shell_membership_tlmod(i, NULL)) {
            return;
        }
        j = shell_compute_syntac(i, false);
        fluf = iden_green_subsemi(shell_compute_ker(j, KER_MOD, LV_REG), L_GREEN);
        pluf = iden_green_subsemi(shell_compute_ker(j, KER_MOD, LV_REG), R_GREEN);
        break;
    case CL_AMT:
        if (!shell_membership_tlamt(i, NULL)) {
            return;
        }
        j = shell_compute_syntac(i, false);
        fluf = iden_green_subsemi(shell_compute_ker(j, KER_AMT, LV_REG), L_GREEN);
        pluf = iden_green_subsemi(shell_compute_ker(j, KER_AMT, LV_REG), R_GREEN);
        break;
    case CL_GR:
        if (!shell_membership_tlgr(i, NULL)) {
            return;
        }
        j = shell_compute_syntac(i, false);
        fluf = iden_green_subsemi(shell_compute_ker(j, KER_GR, LV_REG), L_GREEN);
        pluf = iden_green_subsemi(shell_compute_ker(j, KER_GR, LV_REG), R_GREEN);
        break;
    case CL_DD:
        if (!shell_membership_tldd(i, NULL)) {
            return;
        }
        j = shell_compute_syntac(i, false);
        fluf = iden_green_orbmono(shell_compute_orbits(j, ORB_DD, LV_REG), L_GREEN);
        pluf = iden_green_orbmono(shell_compute_orbits(j, ORB_DD, LV_REG), R_GREEN);
        break;
    case CL_MODP:
        if (!shell_membership_tlmodp(i, NULL)) {
            return;
        }
        j = shell_compute_syntac(i, false);
        fluf = iden_green_orbmono(shell_compute_orbits(j, ORB_MODP, LV_REG), L_GREEN);
        pluf = iden_green_orbmono(shell_compute_orbits(j, ORB_MODP, LV_REG), R_GREEN);
        break;
    case CL_AMTP:
        if (!shell_membership_tlamtp(i, NULL)) {
            return;
        }
        j = shell_compute_syntac(i, false);
        fluf = iden_green_orbmono(shell_compute_orbits(j, ORB_AMTP, LV_REG), L_GREEN);
        pluf = iden_green_orbmono(shell_compute_orbits(j, ORB_AMTP, LV_REG), R_GREEN);
        break;
    case CL_GRP:
        if (!shell_membership_tlgrp(i, NULL)) {
            return;
        }
        j = shell_compute_syntac(i, false);
        fluf = iden_green_orbmono(shell_compute_orbits(j, ORB_GRP, LV_REG), L_GREEN);
        pluf = iden_green_orbmono(shell_compute_orbits(j, ORB_GRP, LV_REG), R_GREEN);
        break;
    default:
        j = 0;
        return;
        break;
    }

    *fllv = 1;
    *pllv = 1;

    parti* flpr;
    parti* plpr;
    while (true) {
        if (fluf->size_par != fluf->size_set) {
            (*fllv)++;
        }
        if (pluf->size_par != pluf->size_set) {
            (*pllv)++;
        }
        if (fluf->size_par == fluf->size_set || pluf->size_par == pluf->size_set) {
            break;
        }
        flpr = ufind_to_parti(fluf);
        plpr = ufind_to_parti(pluf);
        delete_ufind(fluf);
        delete_ufind(pluf);
        if (*fllv % 2 == 0) {
            fluf = iden_lpolc_mono(objects[j]->mor->obj, flpr);
            pluf = iden_rpolc_mono(objects[j]->mor->obj, plpr);
        }
        else {
            fluf = iden_rpolc_mono(objects[j]->mor->obj, flpr);
            pluf = iden_lpolc_mono(objects[j]->mor->obj, plpr);
        }
        delete_parti(flpr);
        delete_parti(plpr);
    }
    delete_ufind(fluf);
    delete_ufind(pluf);

}

bool shell_fphiera(classes cl, int i, FILE* out) {


    if (i == -1) {
        return false;
    }
    // int j = shell_compute_syntac(i, false);
    // if (j == -1) {
    //     return false;
    // }

    // The name of the base class is given by the first argument.
 // La classe


    if (!class_is_basis(cl)) {
        fprintf(out, "#### The class %s is not a valid input class for future/past hierarchies.\n", class_names[cl]);
        return false;
    }

    print_infooper_fphiera(cl, out);
    print_info_input(i, out);


    fprintf(out, "#### Checking if the input belongs to TL(%s).\n", class_names[cl]);
    fprintf(out, "#### If so, computing the least level containing the input in the future/past hierarchy of TL(%s).\n", class_names[cl]);
    fprintf(out, "#### The algorithm is based on the characterizations of TL(%s), LPol and RPol.\n", class_names[cl]);



    short lvfl;
    short lvpl;
    shell_fphiera_aux(i, cl, &lvfl, &lvpl);






    char supscript[10];
    char supscript2[10];
    char message[150];
    if (lvfl == -1) {
        sprintf(message, "The input does not belong to TL(%s).", class_names[cl]);
    }
    else if (lvfl == lvpl) {
        sprint_power_utf8(lvfl, supscript);
        sprintf(message, "The least level containing the input is FL%s(%s) ⋂ PL%s(%s).", supscript, class_names[cl], supscript, class_names[cl]);
    }
    else {
        sprint_power_utf8(lvfl, supscript);
        sprint_power_utf8(lvpl, supscript2);
        sprintf(message, "The least levels containing the input are FL%s(%s) and PL%s(%s).", supscript, class_names[cl], supscript2, class_names[cl]);

    }
    print_dtitle_box(100, true, out, 1, message);





    return true;
}


/***********************/
/* Examples generators */
/***********************/


static ulong exa_size = 0; // Size of exa_array.
static ulong exa_elem = 0; // Number of DFAs stored in the exa_array.
static dfa** exa_array = NULL; // Array that store the example DFAs (size exa_size).

static void exa_init(uint size) {
    size = max(size, 2); // Ensure the size is at least 2.
    exa_size = size; // The size of the array is the number of states in the morphism.
    exa_elem = 0; // The number of elements in the array is initially 0.
    MALLOC(exa_array, exa_size); // Allocate the array of DFAs.
}

static void exa_delete() {
    free(exa_array); // Free the array of DFAs.
    exa_array = NULL; // Reset the pointer to NULL.
    exa_size = 0; // Reset the size of the array.
    exa_elem = 0; // Reset the number of elements in the array.
}

static void exa_grow() {
    exa_size <<= 1; // Double the size of the array.
    REALLOC(exa_array, exa_size); // Reallocate the array to the new size.
}

static bool exa_equal(uint i, uint j) {
    dfa* A1 = exa_array[i];
    dfa* A2 = exa_array[j];

    if (A1->trans->size_graph != A2->trans->size_graph || A1->trans->size_alpha != A2->trans->size_alpha || A1->nb_finals != A2->nb_finals || A1->initial != A2->initial) {
        return false; // If the sizes of the graphs or alphabets are different, the DFAs are not equal.
    }
    for (uint q = 0; q < A1->trans->size_graph; q++) {
        for (uint a = 0; a < A1->trans->size_alpha; a++) {
            if (A1->trans->edges[q][a] != A2->trans->edges[q][a]) {
                return false;
            }
        }
    }
    for (uint h = 0; h < A1->nb_finals; h++) {
        if (A1->finals[h] != A2->finals[h]) {
            return false; // If any final state is different, the DFAs are not equal.
        }
    }
    return true;
}

static uint exa_hash(uint i, uint size_hash) {
    uint hash = 0;

    uint nb = exa_array[i]->trans->size_graph * exa_array[i]->trans->size_alpha + exa_array[i]->nb_finals + 1;
    uint a = 0x9e3779b9; // fractional bits of the golden ratio

    hash = (hash * (nb + 1) + exa_array[i]->initial * a) % size_hash;


    for (uint j = 0; j < exa_array[i]->trans->size_graph; j++) {
        for (uint b = 0; b < exa_array[i]->trans->size_alpha; b++) {
            hash = (hash * (nb + 1) + exa_array[i]->trans->edges[j][b] * a) % size_hash;
        }
    }

    for (uint j = 0; j < exa_array[i]->nb_finals; j++) {
        hash = (hash * (nb + 1) + exa_array[i]->finals[j] * a) % size_hash;
    }

    return hash;
}


void shell_exall(classes* low, int nblow, classes* high, int nbhigh, int states, int alpha) {
    states = max(states, 1);
    alpha = max(alpha, 1);

    bool order = false;
    for (int i = 0; i < nblow; i++) {
        if (shell_membership_needs_order(low[i])) {
            order = true;
            break;
        }
    }
    if (!order) {
        for (int i = 0; i < nbhigh; i++) {
            if (shell_membership_needs_order(high[i])) {
                order = true;
                break;
            }
        }
    }


    dfa_enum* E = dfa_enum_init(states, alpha);
    uint count = 0;

    exa_init(128);
    hash_table* thehash = create_hash_table(8, &exa_hash, &exa_equal);



    while (dfa_enum_next(E)) {

        //       dfa_enum_print(E);
        dfa* A = dfa_enum_to_dfa(E);

        //view_nfa(A);

        int j = object_add_automaton_dfa(NULL, A);


        // TODO: Fix when shell_compute_syntac fails
        //fprintf(stdout, "#### Test %d: Testing automaton %d.\n", count, j);
        int k = shell_compute_syntac(j, order);
        //fprintf(stdout, "#### Test %d: Syntactic monoid computed.\n", count);

        if (k == MEMORY_LIMIT) {
            fprintf(stdout, "#### Test %d: Syntactic monoid too large.\n", count);
            continue;
        }
        if (k == TIMEOUT_OCCURRED) {
            fprintf(stdout, "#### Test %d: timeout occurred.\n", count);
            continue;
        }
        if (k == INTERRUPTION) {
            fprintf(stdout, "#### Test %d: user interruption.\n", count);
            break;
        }
        bool res = true;
        for (int h = 0; h < nblow; h++) {

            res = res && !class_membership[low[h]](k, NULL);
        }

        for (int h = 0; h < nbhigh; h++) {

            res = res && class_membership[high[h]](k, NULL);
        }

        count++;


        if (j != -1 && res) {
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(j)]->aut->obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem) {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size) {
                    exa_grow(); // Grow the array if needed.
                }
            }
            else {
                dfa_delete(exa_array[exa_elem]);
            }
        }

        if (count % 20000 == 0) {
            fprintf(stdout, "#### %d tests done so far. %lu examples found.\n", count, exa_elem);
        }

        object_free(j);
    }

    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.


    fprintf(stdout, "#### %d tests in total. Found %lu example languages.\n", count, exa_elem);

    char buffer[64];
    for (uint i = 0; i < exa_elem; i++) {
        sprintf(buffer, "EXA%04d", i);
        object_add_automaton_dfa(buffer, exa_array[i]);
    }

    exa_delete(); // Free the example array.

    return;
}









void shell_exall_dethiera(classes cl, int level, int states, int alpha, bool neg) {

    if (!class_is_basis(cl)) {
        fprintf(stdout, "#### The class %s is not a valid basis.\n", class_names[cl]);
        return;
    }





    dfa_enum* E = dfa_enum_init(states, alpha);
    uint count = 0;

    exa_init(128);
    hash_table* thehash = create_hash_table(8, &exa_hash, &exa_equal);

    while (dfa_enum_next(E)) {

        //       dfa_enum_print(E);
        dfa* A = dfa_enum_to_dfa(E);

        int i = object_add_automaton_dfa(NULL, A);





        // // TODO: Fix when shell_compute_syntac fails
        // int k = shell_compute_syntac(j, false);

        // if (k == MEMORY_LIMIT) {
        //     fprintf(stdout, "#### Test %d: Syntactic monoid too large.\n", count);
        //     continue;
        // }
        // if (k == TIMEOUT_OCCURRED) {
        //     fprintf(stdout, "#### Test %d: timeout occurred.\n", count);
        //     continue;
        // }
        // if (k == INTERRUPTION) {
        //     fprintf(stdout, "#### Test %d: user interruption.\n", count);
        //     break;
        // }

        short levelj;
        if (neg) {
            levelj = shell_neghiera_aux(i, cl);
        }
        else {
            short pllv;
            short fllv;
            shell_fphiera_aux(i, cl, &fllv, &pllv);
            levelj = max(pllv, fllv);
        }



        count++;
        // view_nfa(objects[j]->aut->obj_nfa);

        if (i != -1 && levelj >= level) {
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(i)]->aut->obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem) {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size) {
                    exa_grow(); // Grow the array if needed.
                }
            }
            else {
                dfa_delete(exa_array[exa_elem]);
            }
        }

        if (count % 20000 == 0) {
            fprintf(stdout, "#### %d tests done so far. %lu were positive\n", count, exa_elem);
        }

        object_free(i);
    }

    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.


    fprintf(stdout, "#### %d tests in total. Found %lu example languages.\n", count, exa_elem);

    char buffer[64];
    for (uint i = 0; i < exa_elem; i++) {
        sprintf(buffer, "EXA%04d", i);
        object_add_automaton_dfa(buffer, exa_array[i]);
    }

    exa_delete(); // Free the example array.

    return;
}





// bool shell_exsearch(com_parameters* pars) {
//     int n = com_nbparams(pars);
//     if (n < 6) {
//         shell_error_leastparams(keywordtostring(KY_EXSEARCH), 6);
//         return false;
//     }

//     // Les classes
//     classes low[n - 5];
//     for (int i = 0; i < n - 5; i++) {
//         low[i] = command_to_class(pars->param);
//         if (low[i] == CL_END) {
//             fprintf(stdout, "#### The class ");
//             print_command(pars->param, stdout);
//             fprintf(stdout, " is either unknown or unsupported.\n\n");
//             return false;
//         }
//         if (class_membership[low[i]] == NULL) {
//             fprintf(stdout, "#### Membership is unsupported for the class ");
//             print_command(pars->param, stdout);
//             fprintf(stdout, ".\n\n");
//             return false;
//         }
//         // printf("low: %s\n", class_names[low[i]]);
//         pars = pars->next;
//     }

//     classes cl = command_to_class(com_getparam(pars, 0));

//     if (cl == CL_END) {
//         fprintf(stdout, "#### The class ");
//         print_command(com_getparam(pars, 0), stdout);
//         fprintf(stdout, " is either unknown or unsupported.\n\n");
//         return false;
//     }

//     if (class_membership[cl] == NULL) {
//         fprintf(stdout, "#### Membership is unsupported for the class ");
//         print_command(com_getparam(pars, 0), stdout);
//         fprintf(stdout, ".\n\n");
//         return false;
//     }

//     if (!com_single(com_getparam(pars, 1))) {
//         shell_error_numpar(keywordtostring(KY_EXSEARCH), 1);
//         return false;
//     }
//     char* end;
//     int cycle = strtol(com_getparam(pars, 1)->main->string, &end, 10);
//     if (*end != '\0') {
//         shell_error_numpar(keywordtostring(KY_EXSEARCH), n - 4);
//         return false;
//     }

//     int count = 0;
//     char buffer[64];

//     for (int i = 0; i < cycle; i++) {
//         sprintf(buffer, "EXA%04d", count);
//         int j = -1;//shell_random_dfa(buffer, pars->next->next);
//         int k = shell_compute_syntac(j, false);
//         if (k == -1) {
//             fprintf(stdout, "#### Test %d: Syntactic monoid too large.\n", i + 1);
//             continue;
//         }
//         bool not = false;
//         for (int h = 0; h < n - 5; h++) {

//             not = not || class_membership[low[h]](k, NULL);
//         }
//         // view_nfa(objects[j]->aut->obj_nfa);
//         if (j != -1 && !not&& class_membership[cl](k, NULL)) {
//             fprintf(stdout, "#### Test %d: Found an example. Stored in variable %s.\n", i + 1, buffer);
//             count++;
//         }
//         else if (j != -1) {
//             fprintf(stdout, "#### Test %d: Not an example.\n", i + 1);
//             object_free(j);
//         }
//     }

//     return false;
// }



bool shell_exall_dfatest(void) {

    dfa_enum* E = dfa_enum_init(3, 3);
    uint count = 0;

    exa_init(128);
    hash_table* thehash = create_hash_table(8, &exa_hash, &exa_equal);


    while (dfa_enum_next(E)) {

        dfa* A = dfa_enum_to_dfa(E);


        int j = object_add_automaton_dfa(NULL, A);

        int k = shell_compute_syntac(j, false);

        if (k == MEMORY_LIMIT) {
            fprintf(stdout, "#### Test %d: Syntactic monoid too large.\n", count);
            continue;
        }
        if (k == TIMEOUT_OCCURRED) {
            fprintf(stdout, "#### Test %d: timeout occurred.\n", count);
            continue;
        }
        if (k == INTERRUPTION) {
            fprintf(stdout, "#### Test %d: user interruption.\n", count);
            break;
        }
        bool resmono = class_membership[CL_UL](k, NULL);
        bool resauto = class_membership[CL_UL](j, NULL);


        count++;


        if (resmono != resauto) {
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(j)]->aut->obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem) {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size) {
                    exa_grow(); // Grow the array if needed.
                }
            }
            else {
                dfa_delete(exa_array[exa_elem]);
            }
        }

        if (count % 20000 == 0) {
            fprintf(stdout, "#### %d tests done so far. %lu were positive\n", count, exa_elem);
        }

        object_free(j);
    }

    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.


    fprintf(stdout, "#### %d tests in total. Found %lu example languages.\n", count, exa_elem);

    char buffer[64];
    for (uint i = 0; i < exa_elem; i++) {
        sprintf(buffer, "EXA%04d", i);
        object_add_automaton_dfa(buffer, exa_array[i]);
    }

    exa_delete(); // Free the example array.

    return false;
}


