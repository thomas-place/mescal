#include "shell_memb.h"
#include "files.h"
#include "monoid_display.h"
#include "shell_autoprops.h"
#include "shell_morprops.h"
#include "type_hash.h"

membership_mode memb_mode = MEMB_MINIMAL;

bool (*class_membership[CL_END])(int, membership_mode, FILE *) = {NULL};

void init_membership(void)
{
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

    class_membership[CL_COPPT] = shell_membership_coppt;
    class_membership[CL_COPOLMOD] = shell_membership_copolmod;
    class_membership[CL_COPOLAMT] = NULL;
    class_membership[CL_COPOLGR] = shell_membership_copolgr;
    class_membership[CL_COPOLDD] = shell_membership_copoldd;
    class_membership[CL_COPOLMODP] = shell_membership_copolmodp;
    class_membership[CL_COPOLAMTP] = NULL;
    class_membership[CL_COPOLGRP] = shell_membership_copolgrp;
    class_membership[CL_COPOL2ST] = shell_membership_copol2st;
    class_membership[CL_COPOL2MOD] = shell_membership_copol2mod;
    class_membership[CL_COPOL2AMT] = shell_membership_copol2amt;
    class_membership[CL_COPOL2GR] = shell_membership_copol2gr;
    class_membership[CL_COPOL2DD] = shell_membership_copol2dd;
    class_membership[CL_COPOL2MODP] = shell_membership_copol2modp;
    class_membership[CL_COPOL2AMTP] = shell_membership_copol2amtp;
    class_membership[CL_COPOL2GRP] = shell_membership_copol2grp;

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

    class_membership[CL_KNASTAMTP] = shell_membership_knastamtp;
    class_membership[CL_KNASTGRP] = shell_membership_knastgrp;
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
    class_membership[CL_PLAMTP] = shell_membership_plamtp;
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
    class_membership[CL_EMPTY] = shell_membership_empty;
}

bool shell_membership_needs_order(classes cl)
{
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
    case CL_COPPT:
    case CL_COPOLMOD:
    case CL_COPOLAMT:
    case CL_COPOLGR:
    case CL_COPOLDD:
    case CL_COPOLMODP:
    case CL_COPOLAMTP:
    case CL_COPOLGRP:
    case CL_COPOL2ST:
    case CL_COPOL2MOD:
    case CL_COPOL2AMT:
    case CL_COPOL2GR:
    case CL_COPOL2DD:
    case CL_COPOL2MODP:
    case CL_COPOL2AMTP:
    case CL_COPOL2GRP:
        return true;
        break;
    default:
        return false;
        break;
    }
}

// static void shell_print_syntac(int j, FILE* out) {
//     print_title_box(10, true, stdout, 1, "The syntactic morphism.");
//     shell_view_object(objects[shell_compute_syntac(j)], false);
// }

// static void shell_print_mini(int j, FILE* out) {
//     print_title_box(10, true, stdout, 1, "The minimal automaton.");
//     shell_view_object(objects[shell_compute_minimal(j)], false);
// }

/*********************/
/* General functions */
/*********************/

bool shell_membership_reg(int, membership_mode, FILE *) { return true; }

bool shell_membership_empty(int, membership_mode, FILE *) { return false; }

bool shell_membership_htgen(int j, membership_mode, FILE *out)
{
    return shell_morprop_htgentriv(shell_compute_syntac(j), "syntactic monoid", out);
}

/*****************/
/* Group classes */
/*****************/

bool shell_membership_st(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:;
        return is_trivial_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monotriv(shell_compute_syntac(j), "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_dd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_ddtrivial_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_semitriv(shell_compute_syntac(j), "syntactic semigroup", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_mod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_letterind_dfa(objects[shell_compute_minimal(j)].obj_dfa, out) && is_permutation_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_letterind(shell_compute_syntac(j), "syntactic morphism", out) && shell_morprop_monogroup(shell_compute_syntac(j), "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_modp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_letterind_dfa(objects[shell_compute_minimal(j)].obj_dfa, out) && is_permutationplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_letterind(shell_compute_syntac(j), "syntactic morphism", out) && shell_morprop_semigroup(shell_compute_syntac(j), "syntactic semigroup", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_amt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_comm_dfa(objects[shell_compute_minimal(j)].obj_dfa, out) && is_permutation_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monocom(shell_compute_syntac(j), "syntactic morphism", out) && shell_morprop_monogroup(shell_compute_syntac(j), "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_amtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_comm_dfa(objects[shell_compute_minimal(j)].obj_dfa, out) && is_permutationplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monocom(shell_compute_syntac(j), "syntactic morphism", out) && shell_morprop_semigroup(shell_compute_syntac(j), "syntactic semigroup", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_gr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_permutation_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monogroup(shell_compute_syntac(j), "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_grp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_permutationplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_semigroup(shell_compute_syntac(j), "syntactic semigroup", out);
        break;
    default:
        return false;
        break;
    }
}

/*****************/
/* AT-ATT-LT-LTT */
/*****************/

bool shell_membership_at(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_comm_dfa(objects[shell_compute_minimal(j)].obj_dfa, out) && is_alphaidem_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monocom(shell_compute_syntac(j), "syntactic monoid", out) && shell_morprop_monoidem(shell_compute_syntac(j), "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_att(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_comm_dfa(objects[shell_compute_minimal(j)].obj_dfa, out) && is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monocom(shell_compute_syntac(j), "syntactic monoid", out) && shell_morprop_monogreen(shell_compute_syntac(j), H_GREEN, "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_lt(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_lt_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa);
        break;
    case MEMB_MINIMAL:
        return is_ddidem_dfa(objects[shell_compute_minimal(j)].obj_dfa, out) && is_ddcomm_dfa(objects[shell_compute_minimal(j)].obj_dfa, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbcom(shell_compute_syntac(j), ORB_DD, "DD", "syntactic morphism", out) && shell_morprop_orbidem(shell_compute_syntac(j), ORB_DD, "DD", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_ltt(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out) && is_lttcomm_dfa(objects[shell_compute_minimal(j)].obj_dfa, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monogreen(shell_compute_syntac(j), H_GREEN, "syntactic semigroup", out) && shell_morprop_semigencom(shell_compute_syntac(j), "syntactic semigroup", out);
        break;
    default:
        return false;
        break;
    }
}

/*********************/
/* Star-free closure */
/*********************/

bool shell_membership_sf(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_nosimple_counter_dfa(objects[shell_compute_minimal(j)].obj_dfa, &error) && is_counterfreegp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, &error, out);
        break;
    case MEMB_MINIMAL:
        return is_counterfreegp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monogreen(shell_compute_syntac(j), H_GREEN, "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_sfmod(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_MINIMAL:
    case MEMB_OPTIMAL:
        return is_counterfreegp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), H_GREEN, KER_MOD, "MOD", "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_sfamt(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_MINIMAL:
    case MEMB_OPTIMAL:
        return is_counterfreegp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), H_GREEN, KER_AMT, "AMT", "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_sfgr(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_MINIMAL:
    case MEMB_OPTIMAL:
        return is_counterfreegp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), H_GREEN, KER_GR, "GR", "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

/**********************/
/* Polynomial closure */
/**********************/

bool shell_membership_ppt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_polgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_ST, out);
        break;
    case MEMB_MINIMAL:
        return is_polgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monojsat(shell_compute_syntac(j), "syntactic monoid", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_coppt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_polgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_ST, out);
        break;
    case MEMB_MINIMAL:
        return is_polgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monojsat(shell_compute_syntac(j), "syntactic monoid", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_polmod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_polgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_MOD, out);
        break;
    case MEMB_MINIMAL:
        return is_polgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kerjsat(shell_compute_syntac(j), KER_MOD, "MOD", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copolmod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_polgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_MOD, out);
        break;
    case MEMB_MINIMAL:
        return is_polgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kerjsat(shell_compute_syntac(j), KER_MOD, "MOD", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_polgr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_polgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_GR, out);
        break;
    case MEMB_MINIMAL:
        return is_polgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monoejsat(shell_compute_syntac(j), "syntactic monoid", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copolgr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_polgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_GR, out);
        break;
    case MEMB_MINIMAL:
        return is_polgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monoejsat(shell_compute_syntac(j), "syntactic monoid", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_poldd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_polgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_DD, "DD", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copoldd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_polgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_DD, "DD", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_polmodp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_polgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_MODP, "MOD⁺", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copolmodp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_polgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_MODP, "MOD⁺", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_polgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_polgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_GRP, "GR⁺", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copolgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_polgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_GRP, "GR⁺", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_pol2st(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_PT, "PT", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copol2st(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_PT, "PT", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_pol2mod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPMOD, "BPol(MOD)", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copol2mod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPMOD, "BPol(MOD)", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_pol2amt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPAMT, "BPol(AMT)", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copol2amt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPAMT, "BPol(AMT)", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_pol2gr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPGR, "BPol(GR)", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copol2gr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPGR, "BPol(GR)", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_pol2dd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPDD, "BPol(DD)", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copol2dd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPDD, "BPol(DD)", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_pol2modp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPMODP, "BPol(MOD⁺)", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copol2modp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPMODP, "BPol(MOD⁺)", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_pol2amtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPAMTP, "BPol(AMT⁺)", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copol2amtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPAMTP, "BPol(AMT⁺)", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_pol2grp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPGRP, "BPol(GR⁺)", "syntactic morphism", false, out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_copol2grp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_pol2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbjsat(shell_compute_syntac(j), ORB_BPGRP, "BPol(GR⁺)", "syntactic morphism", true, out);
        break;
    default:
        return false;
        break;
    }
}

/******************************/
/* Boolean polynomial closure */
/******************************/

bool shell_membership_pt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_piecewise_dfa(objects[shell_compute_minimal(j)].obj_dfa);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out) &&
               is_ltrivialgp_rt_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monogreen(shell_compute_syntac(j), J_GREEN, "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_bpolmod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_bpolmod_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_bpgroupeq(shell_compute_syntac(j), KER_MOD, "MOD", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_bpolamt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_bpolamt_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_bpgroupeq(shell_compute_syntac(j), KER_AMT, "AMT", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_bpolgr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out) &&
               is_ltrivialgp_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out) &&
               is_ltrivialgp_rt_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_blockg(shell_compute_syntac(j), "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_bpoldd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_bpoldd_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_mprop_semiknast(shell_compute_syntac(j), "syntactic semigroup", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_bpolmodp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_bpolgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, false, out);
        break;
    case MEMB_MINIMAL:
        return is_knastgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kerknast(shell_compute_syntac(j), KER_MOD, "MOD", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_bpolamtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_bpolamtplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_bpgroupeqplus(shell_compute_syntac(j), ORB_AMTP, "AMT", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_bpolgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_bpolgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, true, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_bpgroupeqplus(shell_compute_syntac(j), ORB_GRP, "GR", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_bpol2st(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST) &&
               is_knastat_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out) &&
               is_knastat_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_PT, "PT", "syntactic morphism", out) &&
               shell_morprop_knastat(shell_compute_syntac(j), "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_jorbmod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out) &&
               is_ltrivialgp_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out) &&
               is_ltrivialgp_rt_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), J_GREEN, KER_MOD, "MOD", "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_jorbamt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out) &&
               is_ltrivialgp_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out) &&
               is_ltrivialgp_rt_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), J_GREEN, KER_AMT, "AMT", "syntactic monoid", out);
        break;
    default:
        return false;
        break;
    }
}

bool shell_membership_jorbmodp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out) &&
               is_ltrivialgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out) &&
               is_ltrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), J_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_jorbamtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out) &&
               is_ltrivialgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out) &&
               is_ltrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), J_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_jorbgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out) &&
               is_ltrivialgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out) &&
               is_ltrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), J_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_jorbdd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out) &&
               is_ltrivialgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out) &&
               is_ltrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), J_GREEN, ORB_DD, "DD", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_jorbat(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_rtrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out) &&
               is_ltrivialbpgp_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST);
        break;
    case MEMB_MINIMAL:
        return is_rtrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out) &&
               is_ltrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), J_GREEN, ORB_PT, "PT", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_knastamtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_knastgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kerknast(shell_compute_syntac(j), KER_AMT, "AMT", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_knastgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_knastgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kerknast(shell_compute_syntac(j), KER_GR, "GR", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_knastat(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_knastat_dfa(objects[shell_compute_minimal(j)].obj_dfa, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_knastat(shell_compute_syntac(j), "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

/**********************************/
/* Unambiguous polynomial closure */
/**********************************/

bool shell_membership_upoldd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolgpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST);
        break;
    case MEMB_MINIMAL:
        return is_upolgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbtriv(shell_compute_syntac(j), ORB_DD, "DD", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_upolmodp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolgpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD);
        break;
    case MEMB_MINIMAL:
        return is_upolgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbtriv(shell_compute_syntac(j), ORB_MODP, "MOD⁺", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_upolamtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolgpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT);
        break;
    case MEMB_MINIMAL:
        return is_upolgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbtriv(shell_compute_syntac(j), ORB_AMTP, "AMT⁺", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_upolgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolgpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR);
        break;
    case MEMB_MINIMAL:
        return is_upolgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbtriv(shell_compute_syntac(j), ORB_GRP, "GR⁺", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_ubpol2st(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_PT, "PT", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_ubpol2mod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_BPMOD, "BPol(MOD)-orbit", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_ubpol2amt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_BPAMT, "BPol(AMT)-orbit", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_ubpol2gr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_BPGR, "BPol(GR)-orbit", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_ubpol2dd(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_BPDD, "BPol(DD)-orbit", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_ubpol2modp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_BPMODP, "BPol(MOD⁺)-orbit", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_ubpol2amtp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_BPAMTP, "BPol(AMT⁺)-orbit", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

bool shell_membership_ubpol2grp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_upolbp2gpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR);
        break;
    case MEMB_MINIMAL:
        return is_upolbp2gpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_ubp2eq(shell_compute_syntac(j), ORB_BPGRP, "BPol(GR⁺)-orbit", "syntactic morphism", out);
        break;
    default:
        return false;
        break;
    }
    return false;
}

/******************/
/* Temporal logic */
/******************/

bool shell_membership_ul(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dagp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_MINIMAL:
        return is_dagp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monoda(shell_compute_syntac(j), "syntactic monoid", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tlmod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dagp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_MINIMAL:
        return is_dagp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kerda(shell_compute_syntac(j), KER_MOD, "MOD", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tlamt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dagp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_MINIMAL:
        return is_dagp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kerda(shell_compute_syntac(j), KER_AMT, "AMT", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tlgr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dagp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_MINIMAL:
        return is_dagp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kerda(shell_compute_syntac(j), KER_GR, "GR", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tldd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dagpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_MINIMAL:
        return is_dagpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_DD, "DD", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tlmodp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dagpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_MINIMAL:
        return is_dagpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_MODP, "MOD⁺", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tlamtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dagpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_MINIMAL:
        return is_dagpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_AMTP, "AMT⁺", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tlgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dagpplus_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_MINIMAL:
        return is_dagpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_GRP, "GR⁺", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tl2st(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dabpgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_MINIMAL:
        return is_dabpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_PT, "TL(ST)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tl2mod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dabpgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_MINIMAL:
        return is_dabpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_BPMOD, "TL(MOD)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tl2amt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dabpgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_MINIMAL:
        return is_dabpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_BPAMT, "TL(AMT)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tl2gr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        return is_dabpgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_MINIMAL:
        return is_dabpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_BPGR, "TL(GR)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tl2dd(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_MINIMAL:
        return is_dabpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, &error, out);
        break;
    case MEMB_OPTIMAL:
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_BPDD, "TL(DD)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tl2modp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_MINIMAL:
        return is_dabpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, &error, out);
        break;
    case MEMB_OPTIMAL:
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_BPMODP, "TL(MOD⁺)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tl2amtp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_MINIMAL:
        return is_dabpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, &error, out);
        break;
    case MEMB_OPTIMAL:
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_BPAMTP, "TL(AMT⁺)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_tl2grp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_MINIMAL:
        return is_dabpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, &error, out);
        break;
    case MEMB_OPTIMAL:
    case MEMB_SYNTAC:
        return shell_morprop_orbda(shell_compute_syntac(j), ORB_BPGRP, "TL(GR⁺)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialgp_mirror_dfa(objects[j].obj_dfa, BA_ST);
        }
        else
        {
            return is_ltrivialgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monogreen(shell_compute_syntac(j), L_GREEN, "syntactic monoid", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_flmod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialgp_mirror_dfa(objects[j].obj_dfa, BA_MOD);
        }
        else
        {
            return is_ltrivialgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), L_GREEN, KER_MOD, "MOD", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_flamt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialgp_mirror_dfa(objects[j].obj_dfa, BA_AMT);
        }
        else
        {
            return is_ltrivialgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), L_GREEN, KER_AMT, "AMT", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_flgr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialgp_mirror_dfa(objects[j].obj_dfa, BA_GR);
        }
        else
        {
            return is_ltrivialgp_opti_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), L_GREEN, KER_GR, "GR", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fldd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialgpplus_mirror_dfa(objects[j].obj_dfa, BA_ST);
        }
        else
        {
            return is_ltrivialgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_DD, "DD", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_flmodp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialgpplus_mirror_dfa(objects[j].obj_dfa, BA_MOD);
        }
        else
        {
            return is_ltrivialgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_flamtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialgpplus_mirror_dfa(objects[j].obj_dfa, BA_AMT);
        }
        else
        {
            return is_ltrivialgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_flgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialgpplus_mirror_dfa(objects[j].obj_dfa, BA_GR);
        }
        else
        {
            return is_ltrivialgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl2st(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialbpgp_mirror_dfa(objects[j].obj_dfa, BA_ST);
        }
        else
        {
            return is_ltrivialbpgp_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_PT, "FL(ST)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl2mod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialbpgp_mirror_dfa(objects[j].obj_dfa, BA_MOD);
        }
        else
        {
            return is_ltrivialbpgp_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD);
        }
        break;
        break;
    case MEMB_MINIMAL:
        return is_ltrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_BPMOD, "FL(MOD)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl2amt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialbpgp_mirror_dfa(objects[j].obj_dfa, BA_AMT);
        }
        else
        {
            return is_ltrivialbpgp_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT);
        }
        break;
        break;
    case MEMB_MINIMAL:
        return is_ltrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_BPAMT, "FL(AMT)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl2gr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialbpgp_mirror_dfa(objects[j].obj_dfa, BA_GR);
        }
        else
        {
            return is_ltrivialbpgp_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_BPGR, "FL(GR)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl2dd(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialbpgpplus_mirror_dfa(objects[j].obj_dfa, BA_ST);
        }
        else
        {
            return is_ltrivialbpgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialbpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_BPDD, "FL(DD)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl2modp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialbpgpplus_mirror_dfa(objects[j].obj_dfa, BA_MOD);
        }
        else
        {
            return is_ltrivialbpgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialbpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_BPMODP, "FL(MOD⁺)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl2amtp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialbpgpplus_mirror_dfa(objects[j].obj_dfa, BA_AMT);
        }
        else
        {
            return is_ltrivialbpgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialbpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_BPAMTP, "FL(AMT⁺)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_fl2grp(int j, membership_mode mode, FILE *out)
{
    int error = 0;
    switch (mode)
    {
    case MEMB_OPTIMAL:
        if (objects[j].type == DAUTOMATON)
        {
            return is_ltrivialbpgpplus_mirror_dfa(objects[j].obj_dfa, BA_GR);
        }
        else
        {
            return is_ltrivialbpgpplus_mirror_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR);
        }
        break;
    case MEMB_MINIMAL:
        return is_ltrivialbpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, &error, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), L_GREEN, ORB_BPGRP, "FL(GR⁺)", "syntactic morphism", out);
        break;

    default:
        break;
    }
    return false;
}

bool shell_membership_pl(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_monogreen(shell_compute_syntac(j), R_GREEN, "syntactic monoid", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_plmod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), R_GREEN, KER_MOD, "MOD", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_plamt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), R_GREEN, KER_AMT, "AMT", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_plgr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_kergreen(shell_compute_syntac(j), R_GREEN, KER_GR, "GR", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pldd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_DD, "DD", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_plmodp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_plamtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_plgrp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pl2st(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_PT, "PL(ST)", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pl2mod(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_BPMOD, "PL(MOD)", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pl2amt(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_BPAMT, "PL(AMT)", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pl2gr(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialbpgp_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_BPGR, "PL(GR)", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pl2dd(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialbpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_ST, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_BPDD, "PL(DD)", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pl2modp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialbpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_MOD, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_BPMODP, "PL(MOD⁺)", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pl2amtp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialbpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_AMT, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_BPAMTP, "PL(AMT⁺)", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

bool shell_membership_pl2grp(int j, membership_mode mode, FILE *out)
{
    switch (mode)
    {
    case MEMB_OPTIMAL:
    case MEMB_MINIMAL:
        return is_rtrivialbpgpplus_dfa(objects[shell_compute_minimal(j)].obj_dfa, BA_GR, out);
        break;
    case MEMB_SYNTAC:
        return shell_morprop_orbgreen(shell_compute_syntac(j), R_GREEN, ORB_BPGRP, "PL(GR⁺)", "syntactic morphism", out);
    default:
        break;
    }
    return false;
}

/********************************/
/* Summary for all main classes */
/********************************/

typedef enum
{
    ANS_YES,
    ANS_NO,
    ANS_UNKNOWN
} ans_type;

enum
{
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

enum
{
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

static void populate_table_star(int j, int op, int bs, bool (*fun)(int, membership_mode, FILE *), ans_type res[CLT_SIZE][BSI_SIZE])
{
    if (res[op][bs] != ANS_UNKNOWN)
    {
        return;
    }
    bool val = fun(j, MEMB_OPTIMAL, NULL);
    if (val)
    {
        for (int g = op; g < CLT_SIZE; g++)
        {
            for (int h = bs; h < BSI_SIZE; h++)
            {
                res[g][h] = ANS_YES;
            }
        }
    }
    else
    {
        for (int g = op; g >= 0; g--)
        {
            for (int h = bs; h >= 0; h -= 2)
            {
                res[g][h] = ANS_NO;
            }
        }
    }
}

static void populate_table_plus(int j, int op, int bs, bool (*fun)(int, membership_mode, FILE *), ans_type res[CLT_SIZE][BSI_SIZE])
{
    if (res[op][bs] != ANS_UNKNOWN)
    {
        return;
    }
    bool val = fun(j, MEMB_OPTIMAL, NULL);
    if (val)
    {
        for (int g = op; g < CLT_SIZE; g++)
        {
            for (int h = bs; h < BSI_SIZE; h += 2)
            {
                res[g][h] = ANS_YES;
            }
        }
    }
    else
    {
        for (int g = op; g >= 0; g--)
        {
            for (int h = bs; h >= 0; h--)
            {
                res[g][h] = ANS_NO;
            }
        }
    }
}

static void populate_line_from_sf(int bs, ans_type ans, ans_type res[CLT_SIZE][BSI_SIZE])
{
    if (ans == ANS_YES)
    {
        res[CLT_SF][bs] = ANS_YES;
        return;
    }
    for (int g = 0; g < CLT_SIZE; g++)
    {
        res[g][bs] = ANS_NO;
    }
}

static void summary_print_answer(ans_type res, FILE *out)
{
    switch (res)
    {
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

void shell_chiera_summary(int i, FILE *out)
{
    int j = shell_compute_syntac(i);

    if (j < 0)
    {
        return;
    }

    ans_type res[CLT_SIZE][BSI_SIZE];

    for (int k = 0; k < CLT_SIZE; k++)
    {
        for (int l = 0; l < BSI_SIZE; l++)
        {
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
    for (uint h = 0; h < CLT_SIZE; h++)
    {
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

    for (uint h = 0; h < CLT_SIZE; h++)
    {
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

    for (uint h = 0; h < CLT_SIZE; h++)
    {
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

    for (uint h = 0; h < CLT_SIZE; h++)
    {
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

    for (uint h = 0; h < CLT_SIZE; h++)
    {
        summary_print_answer(res[h][BSI_AMT], out);
    }

    fprintf(out, "║\n");

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: AMT⁺ ║   Pol(AMT⁺)  ║  BPol(AMT⁺)  ║   TL(AMT⁺)   ║  Pol₂(AMT⁺)  ║  BPol₂(AMT⁺) ║ UBPol₂(AMT⁺) ║  TL₂(AMT⁺)   ║   SF(AMT⁺)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < CLT_SIZE; h++)
    {
        summary_print_answer(res[h][BSI_AMTP], out);
    }

    fprintf(out, "║\n");

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: GR   ║    Pol(GR)   ║   BPol(GR)   ║    TL(GR)    ║   Pol₂(GR)   ║   BPol₂(GR)  ║  UBPol₂(GR)  ║   TL₂(GR)    ║    SF(GR)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    for (uint h = 0; h < CLT_SIZE; h++)
    {
        summary_print_answer(res[h][BSI_GR], out);
    }

    fprintf(out, "║\n");

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: GR⁺  ║    Pol(GR⁺)  ║   BPol(GR⁺)  ║    TL(GR⁺)   ║   Pol₂(GR⁺)  ║  BPol₂(GR⁺)  ║  UBPol₂(GR⁺) ║   TL₂(GR⁺)   ║    SF(GR⁺)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(out, "║             ");
    for (uint h = 0; h < CLT_SIZE; h++)
    {
        summary_print_answer(res[h][BSI_GRP], out);
    }

    fprintf(out, "║\n");

    fprintf(out, "╚══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╝\n");

    return;
}

static void summary_print_answer_nav(bool res, FILE *out)
{
    if (res)
    {
        fprintf(out, "║" ANSI_COLOR_GREEN "     YES      " ANSI_COLOR_RESET);
        return;
    }
    else
    {
        fprintf(out, "║" ANSI_COLOR_RED "     NO       " ANSI_COLOR_RESET);
        return;
    }
}

void shell_navhiera_summary(int j, FILE *out)
{

    if (j < 0)
    {
        return;
    }

    bool res[8][BSI_SIZE];

    /************/
    /* Basis ST */
    /************/

    res[0][BSI_ST] = shell_membership_st(j, MEMB_OPTIMAL, NULL);
    res[1][BSI_ST] = res[0][BSI_ST] || shell_membership_fl(j, MEMB_OPTIMAL, NULL);
    res[2][BSI_ST] = res[0][BSI_ST] || shell_membership_pl(j, MEMB_OPTIMAL, NULL);
    res[3][BSI_ST] = res[1][BSI_ST] || res[2][BSI_ST] || shell_membership_ul(j, MEMB_OPTIMAL, NULL);
    res[4][BSI_ST] = res[3][BSI_ST] || shell_membership_fl2st(j, MEMB_OPTIMAL, NULL);
    res[5][BSI_ST] = res[3][BSI_ST] || shell_membership_pl2st(j, MEMB_OPTIMAL, NULL);
    res[6][BSI_ST] = res[4][BSI_ST] || res[5][BSI_ST] || shell_membership_tl2st(j, MEMB_OPTIMAL, NULL);
    res[7][BSI_ST] = res[6][BSI_ST] || shell_membership_sf(j, MEMB_OPTIMAL, NULL);

    fprintf(out, "╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(out, "║                                    Navigational hierarchies: membership tests summary                                 ║\n");
    fprintf(out, "╠══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╣\n");
    fprintf(out, "║  Basis: ST   ║    FL(ST)    ║    PL(ST)    ║    TL(ST)    ║    FL₂(ST)   ║    PL₂(ST)   ║    TL₂(ST)   ║    SF(ST)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(out, "║             ");
    for (uint h = 0; h < 8; h++)
    {
        summary_print_answer_nav(res[h][BSI_ST], out);
    }
    fprintf(out, "║\n");

    /************/
    /* Basis DD */
    /************/

    res[0][BSI_DD] = res[7][BSI_ST] && (res[0][BSI_ST] || shell_membership_dd(j, MEMB_OPTIMAL, NULL));
    res[1][BSI_DD] = res[7][BSI_ST] && (res[1][BSI_ST] || res[0][BSI_DD] || shell_membership_fldd(j, MEMB_OPTIMAL, NULL));
    res[2][BSI_DD] = res[7][BSI_ST] && (res[2][BSI_ST] || res[0][BSI_DD] || shell_membership_pldd(j, MEMB_OPTIMAL, NULL));
    res[3][BSI_DD] = res[7][BSI_ST] && (res[3][BSI_ST] || res[1][BSI_DD] || res[2][BSI_DD] || shell_membership_tldd(j, MEMB_OPTIMAL, NULL));
    res[4][BSI_DD] = res[7][BSI_ST] && (res[4][BSI_ST] || res[3][BSI_DD] || shell_membership_fl2dd(j, MEMB_OPTIMAL, NULL));
    res[5][BSI_DD] = res[7][BSI_ST] && (res[5][BSI_ST] || res[3][BSI_DD] || shell_membership_pl2dd(j, MEMB_OPTIMAL, NULL));
    res[6][BSI_DD] = res[7][BSI_ST] && (res[6][BSI_ST] || res[4][BSI_DD] || res[5][BSI_DD] || shell_membership_tl2dd(j, MEMB_OPTIMAL, NULL));
    res[7][BSI_DD] = res[7][BSI_ST];

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: DD   ║    FL(DD)    ║    PL(DD)    ║    TL(DD)    ║    FL₂(DD)   ║    PL₂(DD)   ║    TL₂(DD)   ║    SF(DD)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < 8; h++)
    {
        summary_print_answer_nav(res[h][BSI_DD], out);
    }
    fprintf(out, "║\n");

    /*************/
    /* Basis MOD */
    /*************/

    res[0][BSI_MOD] = res[0][BSI_ST] || shell_membership_mod(j, MEMB_OPTIMAL, NULL);
    res[1][BSI_MOD] = res[1][BSI_ST] || res[0][BSI_MOD] || shell_membership_flmod(j, MEMB_OPTIMAL, NULL);
    res[2][BSI_MOD] = res[2][BSI_ST] || res[0][BSI_MOD] || shell_membership_plmod(j, MEMB_OPTIMAL, NULL);
    res[3][BSI_MOD] = res[3][BSI_ST] || res[1][BSI_MOD] || res[2][BSI_MOD] || shell_membership_tlmod(j, MEMB_OPTIMAL, NULL);
    res[4][BSI_MOD] = res[4][BSI_ST] || res[3][BSI_MOD] || shell_membership_fl2mod(j, MEMB_OPTIMAL, NULL);
    res[5][BSI_MOD] = res[5][BSI_ST] || res[3][BSI_MOD] || shell_membership_pl2mod(j, MEMB_OPTIMAL, NULL);
    res[6][BSI_MOD] = res[6][BSI_ST] || res[4][BSI_MOD] || res[5][BSI_MOD] || shell_membership_tl2mod(j, MEMB_OPTIMAL, NULL);
    res[7][BSI_MOD] = res[7][BSI_ST] || res[6][BSI_MOD] || shell_membership_sfmod(j, MEMB_OPTIMAL, NULL);

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: MOD  ║    FL(MOD)   ║    PL(MOD)   ║    TL(MOD)   ║    FL₂(MOD)  ║    PL₂(MOD)  ║    TL₂(MOD)  ║    SF(MOD)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < 8; h++)
    {
        summary_print_answer_nav(res[h][BSI_MOD], out);
    }
    fprintf(out, "║\n");

    /*************/
    /* Basis MOD⁺ */
    /*************/

    res[0][BSI_MODP] = res[7][BSI_MOD] && (res[0][BSI_DD] || res[0][BSI_MOD] || shell_membership_modp(j, MEMB_OPTIMAL, NULL));
    res[1][BSI_MODP] = res[7][BSI_MOD] && (res[1][BSI_DD] || res[1][BSI_MOD] || res[0][BSI_MODP] || shell_membership_flmodp(j, MEMB_OPTIMAL, NULL));
    res[2][BSI_MODP] = res[7][BSI_MOD] && (res[2][BSI_DD] || res[2][BSI_MOD] || res[0][BSI_MODP] || shell_membership_plmodp(j, MEMB_OPTIMAL, NULL));
    res[3][BSI_MODP] = res[7][BSI_MOD] && (res[3][BSI_DD] || res[3][BSI_MOD] || res[1][BSI_MODP] || res[2][BSI_MODP] || shell_membership_tlmodp(j, MEMB_OPTIMAL, NULL));
    res[4][BSI_MODP] = res[7][BSI_MOD] && (res[4][BSI_DD] || res[4][BSI_MOD] || res[3][BSI_MODP] || shell_membership_fl2modp(j, MEMB_OPTIMAL, NULL));
    res[5][BSI_MODP] = res[7][BSI_MOD] && (res[5][BSI_DD] || res[5][BSI_MOD] || res[3][BSI_MODP] || shell_membership_pl2modp(j, MEMB_OPTIMAL, NULL));
    res[6][BSI_MODP] = res[7][BSI_MOD] && (res[6][BSI_DD] || res[6][BSI_MOD] || res[4][BSI_MODP] || res[5][BSI_MODP] || shell_membership_tl2modp(j, MEMB_OPTIMAL, NULL));
    res[7][BSI_MODP] = res[7][BSI_MOD];

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║ Basis: MOD⁺  ║   FL(MOD⁺)   ║   PL(MOD⁺)   ║   TL(MOD⁺)   ║   FL₂(MOD⁺)  ║   PL₂(MOD⁺)  ║   TL₂(MOD⁺)  ║   SF(MOD⁺)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < 8; h++)
    {
        summary_print_answer_nav(res[h][BSI_MODP], out);
    }
    fprintf(out, "║\n");

    /*************/
    /* Basis AMT */
    /*************/

    res[0][BSI_AMT] = res[0][BSI_MOD] || shell_membership_amt(j, MEMB_OPTIMAL, NULL);
    res[1][BSI_AMT] = res[1][BSI_MOD] || res[0][BSI_AMT] || shell_membership_flamt(j, MEMB_OPTIMAL, NULL);
    res[2][BSI_AMT] = res[2][BSI_MOD] || res[0][BSI_AMT] || shell_membership_plamt(j, MEMB_OPTIMAL, NULL);
    res[3][BSI_AMT] = res[3][BSI_MOD] || res[1][BSI_AMT] || res[2][BSI_AMT] || shell_membership_tlamt(j, MEMB_OPTIMAL, NULL);
    res[4][BSI_AMT] = res[4][BSI_MOD] || res[3][BSI_AMT] || shell_membership_fl2amt(j, MEMB_OPTIMAL, NULL);
    res[5][BSI_AMT] = res[5][BSI_MOD] || res[3][BSI_AMT] || shell_membership_pl2amt(j, MEMB_OPTIMAL, NULL);
    res[6][BSI_AMT] = res[6][BSI_MOD] || res[4][BSI_AMT] || res[5][BSI_AMT] || shell_membership_tl2amt(j, MEMB_OPTIMAL, NULL);
    res[7][BSI_AMT] = res[7][BSI_MOD] || res[6][BSI_AMT] || shell_membership_sfamt(j, MEMB_OPTIMAL, NULL);

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: AMT  ║    FL(AMT)   ║    PL(AMT)   ║    TL(AMT)   ║    FL₂(AMT)  ║    PL₂(AMT)  ║    TL₂(AMT)  ║    SF(AMT)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < 8; h++)
    {
        summary_print_answer_nav(res[h][BSI_AMT], out);
    }
    fprintf(out, "║\n");

    /*************/
    /* Basis AMT⁺ */
    /*************/

    res[0][BSI_AMTP] = res[7][BSI_AMT] && (res[0][BSI_MODP] || res[0][BSI_AMT] || shell_membership_amtp(j, MEMB_OPTIMAL, NULL));
    res[1][BSI_AMTP] = res[7][BSI_AMT] && (res[1][BSI_MODP] || res[1][BSI_AMT] || res[0][BSI_AMTP] || shell_membership_flamtp(j, MEMB_OPTIMAL, NULL));
    res[2][BSI_AMTP] = res[7][BSI_AMT] && (res[2][BSI_MODP] || res[2][BSI_AMT] || res[0][BSI_AMTP] || shell_membership_plamtp(j, MEMB_OPTIMAL, NULL));
    res[3][BSI_AMTP] = res[7][BSI_AMT] && (res[3][BSI_MODP] || res[3][BSI_AMT] || res[1][BSI_AMTP] || res[2][BSI_AMTP] || shell_membership_tlamtp(j, MEMB_OPTIMAL, NULL));
    res[4][BSI_AMTP] = res[7][BSI_AMT] && (res[4][BSI_MODP] || res[4][BSI_AMT] || res[3][BSI_AMTP] || shell_membership_fl2amtp(j, MEMB_OPTIMAL, NULL));
    res[5][BSI_AMTP] = res[7][BSI_AMT] && (res[5][BSI_MODP] || res[5][BSI_AMT] || res[3][BSI_AMTP] || shell_membership_pl2amtp(j, MEMB_OPTIMAL, NULL));
    res[6][BSI_AMTP] = res[7][BSI_AMT] && (res[6][BSI_MODP] || res[6][BSI_AMT] || res[4][BSI_AMTP] || res[5][BSI_AMTP] || shell_membership_tl2amtp(j, MEMB_OPTIMAL, NULL));
    res[7][BSI_AMTP] = res[7][BSI_AMT];

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║ Basis: AMT⁺  ║   FL(AMT⁺)   ║   PL(AMT⁺)   ║   TL(AMT⁺)   ║   FL₂(AMT⁺)  ║   PL₂(AMT⁺)  ║   TL₂(AMT⁺)  ║   SF(AMT⁺)   ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < 8; h++)
    {
        summary_print_answer_nav(res[h][BSI_AMTP], out);
    }
    fprintf(out, "║\n");

    /*************/
    /* Basis GR */
    /*************/

    res[0][BSI_GR] = res[0][BSI_AMT] || shell_membership_gr(j, MEMB_OPTIMAL, NULL);
    res[1][BSI_GR] = res[1][BSI_AMT] || res[0][BSI_GR] || shell_membership_flgr(j, MEMB_OPTIMAL, NULL);
    res[2][BSI_GR] = res[2][BSI_AMT] || res[0][BSI_GR] || shell_membership_plgr(j, MEMB_OPTIMAL, NULL);
    res[3][BSI_GR] = res[3][BSI_AMT] || res[1][BSI_GR] || res[2][BSI_GR] || shell_membership_tlgr(j, MEMB_OPTIMAL, NULL);
    res[4][BSI_GR] = res[4][BSI_AMT] || res[3][BSI_GR] || shell_membership_fl2gr(j, MEMB_OPTIMAL, NULL);
    res[5][BSI_GR] = res[5][BSI_AMT] || res[3][BSI_GR] || shell_membership_pl2gr(j, MEMB_OPTIMAL, NULL);
    res[6][BSI_GR] = res[6][BSI_AMT] || res[4][BSI_GR] || res[5][BSI_GR] || shell_membership_tl2gr(j, MEMB_OPTIMAL, NULL);
    res[7][BSI_GR] = res[7][BSI_AMT] || res[6][BSI_GR] || shell_membership_sfgr(j, MEMB_OPTIMAL, NULL);

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║   Basis: GR  ║    FL(GR)    ║    PL(GR)    ║    TL(GR)    ║    FL₂(GR)   ║    PL₂(GR)   ║    TL₂(GR)   ║    SF(GR)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < 8; h++)
    {
        summary_print_answer_nav(res[h][BSI_GR], out);
    }
    fprintf(out, "║\n");

    /*************/
    /* Basis GR⁺ */
    /*************/

    res[0][BSI_GRP] = res[7][BSI_GR] && (res[0][BSI_AMTP] || res[0][BSI_GR] || shell_membership_grp(j, MEMB_OPTIMAL, NULL));
    res[1][BSI_GRP] = res[7][BSI_GR] && (res[1][BSI_AMTP] || res[1][BSI_GR] || res[0][BSI_GRP] || shell_membership_flgrp(j, MEMB_OPTIMAL, NULL));
    res[2][BSI_GRP] = res[7][BSI_GR] && (res[2][BSI_AMTP] || res[2][BSI_GR] || res[0][BSI_GRP] || shell_membership_plgrp(j, MEMB_OPTIMAL, NULL));
    res[3][BSI_GRP] = res[7][BSI_GR] && (res[3][BSI_AMTP] || res[3][BSI_GR] || res[1][BSI_GRP] || res[2][BSI_GRP] || shell_membership_tlgrp(j, MEMB_OPTIMAL, NULL));
    res[4][BSI_GRP] = res[7][BSI_GR] && (res[4][BSI_AMTP] || res[4][BSI_GR] || res[3][BSI_GRP] || shell_membership_fl2grp(j, MEMB_OPTIMAL, NULL));
    res[5][BSI_GRP] = res[7][BSI_GR] && (res[5][BSI_AMTP] || res[5][BSI_GR] || res[3][BSI_GRP] || shell_membership_pl2grp(j, MEMB_OPTIMAL, NULL));
    res[6][BSI_GRP] = res[7][BSI_GR] && (res[6][BSI_AMTP] || res[6][BSI_GR] || res[4][BSI_GRP] || res[5][BSI_GRP] || shell_membership_tl2grp(j, MEMB_OPTIMAL, NULL));
    res[7][BSI_GRP] = res[7][BSI_GR];

    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(out, "║  Basis: GR⁺  ║    FL(GR⁺)   ║    PL(GR⁺)   ║    TL(GR⁺)   ║   FL₂(GR⁺)   ║   PL₂(GR⁺)   ║   TL₂(GR⁺)   ║   SF(GR⁺)    ║\n");
    fprintf(out, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");

    for (uint h = 0; h < 8; h++)
    {
        summary_print_answer_nav(res[h][BSI_GRP], out);
    }
    fprintf(out, "║\n");

    fprintf(out, "╚══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╝\n");

    return;
}

static short shell_neghiera_aux(int i, classes cl)
{
    ufind *theuf = NULL;

    int j = 0;
    switch (cl)
    {
    case CL_ST:
        if (!shell_membership_ul(i, MEMB_OPTIMAL, NULL))
        {
            return -1;
        }
        j = shell_compute_syntac(i);
        theuf = iden_green_mono(objects[j].mor->obj, J_GREEN);
        break;
    case CL_MOD:
        if (!shell_membership_tlmod(i, MEMB_OPTIMAL, NULL))
        {
            return -1;
        }
        j = shell_compute_syntac(i);
        theuf = iden_bpolmod_mono(objects[j].mor->obj);
        break;
    case CL_AMT:
        if (!shell_membership_tlamt(i, MEMB_OPTIMAL, NULL))
        {
            return -1;
        }
        j = shell_compute_syntac(i);
        theuf = iden_bpolamt_mono(objects[j].mor->obj);
        break;
    case CL_GR:
        if (!shell_membership_tlgr(i, MEMB_OPTIMAL, NULL))
        {
            return -1;
        }
        j = shell_compute_syntac(i);
        theuf = iden_blockg_mono(objects[j].mor->obj);
        break;
    case CL_DD:
        if (!shell_membership_tldd(i, MEMB_OPTIMAL, NULL))
        {
            return -1;
        }
        j = shell_compute_syntac(i);
        theuf = iden_knast_mono(shell_compute_orbits(j, ORB_DD, LV_REG));
        break;
    case CL_MODP:
        if (!shell_membership_tlmodp(i, MEMB_OPTIMAL, NULL))
        {
            return -1;
        }
        j = shell_compute_syntac(i);
        theuf = iden_qknast_mono(shell_compute_orbits(j, ORB_MODP, LV_REG), shell_compute_ker(j, KER_MOD, LV_REG));
        break;
    case CL_AMTP:
        if (!shell_membership_tlamtp(i, MEMB_OPTIMAL, NULL))
        {
            return -1;
        }
        j = shell_compute_syntac(i);
        theuf = iden_bpolamtp_mono(shell_compute_orbits(j, ORB_AMTP, LV_REG));
        break;
    case CL_GRP:
        if (!shell_membership_tlgrp(i, MEMB_OPTIMAL, NULL))
        {
            return -1;
        }
        j = shell_compute_syntac(i);
        theuf = iden_bpolgrp_mono(shell_compute_orbits(j, ORB_GRP, LV_REG));
        break;
    default:
        return -1;
        break;
    }

    short level = 1;
    parti *thepr;
    while (true)
    {
        if (theuf->size_par == theuf->size_set)
        {
            break;
        }
        level++;
        thepr = ufind_to_parti(theuf);
        delete_ufind(theuf);
        theuf = iden_mpolc_mono(objects[j].mor->obj, thepr);
        delete_parti(thepr);
    }
    delete_ufind(theuf);
    return level;
}

bool shell_neghiera(classes cl, int i, FILE *out)
{

    if (i == -1)
    {
        return false;
    }

    if (!class_is_basis(cl))
    {
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
    if (level == -1)
    {
        sprintf(message, "The input does not belong to TL(%s).", class_names[cl]);
    }
    else
    {
        char supscript[20];
        sprint_power_utf8(level, supscript);
        sprintf(message, "The least level containing the input is TL%s(%s).", supscript, class_names[cl]);
    }

    print_dtitle_box(100, true, out, 1, message);

    return true;
}

static void shell_fphiera_aux(int i, classes cl, short *fllv, short *pllv)
{
    *fllv = -1;
    *pllv = -1;

    ufind *pluf = NULL;
    ufind *fluf = NULL;

    int j;
    switch (cl)
    {
    case CL_ST:
        if (!shell_membership_ul(i, MEMB_OPTIMAL, NULL))
        {
            return;
        }

        j = shell_compute_syntac(i);
        fluf = iden_green_mono(objects[j].mor->obj, L_GREEN);
        pluf = iden_green_mono(objects[j].mor->obj, R_GREEN);
        break;
    case CL_MOD:
        if (!shell_membership_tlmod(i, MEMB_OPTIMAL, NULL))
        {
            return;
        }
        j = shell_compute_syntac(i);
        fluf = iden_green_subsemi(shell_compute_ker(j, KER_MOD, LV_REG), L_GREEN);
        pluf = iden_green_subsemi(shell_compute_ker(j, KER_MOD, LV_REG), R_GREEN);
        break;
    case CL_AMT:
        if (!shell_membership_tlamt(i, MEMB_OPTIMAL, NULL))
        {
            return;
        }
        j = shell_compute_syntac(i);
        fluf = iden_green_subsemi(shell_compute_ker(j, KER_AMT, LV_REG), L_GREEN);
        pluf = iden_green_subsemi(shell_compute_ker(j, KER_AMT, LV_REG), R_GREEN);
        break;
    case CL_GR:
        if (!shell_membership_tlgr(i, MEMB_OPTIMAL, NULL))
        {
            return;
        }
        j = shell_compute_syntac(i);
        fluf = iden_green_subsemi(shell_compute_ker(j, KER_GR, LV_REG), L_GREEN);
        pluf = iden_green_subsemi(shell_compute_ker(j, KER_GR, LV_REG), R_GREEN);
        break;
    case CL_DD:
        if (!shell_membership_tldd(i, MEMB_OPTIMAL, NULL))
        {
            return;
        }
        j = shell_compute_syntac(i);
        fluf = iden_green_orbmono(shell_compute_orbits(j, ORB_DD, LV_REG), L_GREEN);
        pluf = iden_green_orbmono(shell_compute_orbits(j, ORB_DD, LV_REG), R_GREEN);
        break;
    case CL_MODP:
        if (!shell_membership_tlmodp(i, MEMB_OPTIMAL, NULL))
        {
            return;
        }
        j = shell_compute_syntac(i);
        fluf = iden_green_orbmono(shell_compute_orbits(j, ORB_MODP, LV_REG), L_GREEN);
        pluf = iden_green_orbmono(shell_compute_orbits(j, ORB_MODP, LV_REG), R_GREEN);
        break;
    case CL_AMTP:
        if (!shell_membership_tlamtp(i, MEMB_OPTIMAL, NULL))
        {
            return;
        }
        j = shell_compute_syntac(i);
        fluf = iden_green_orbmono(shell_compute_orbits(j, ORB_AMTP, LV_REG), L_GREEN);
        pluf = iden_green_orbmono(shell_compute_orbits(j, ORB_AMTP, LV_REG), R_GREEN);
        break;
    case CL_GRP:
        if (!shell_membership_tlgrp(i, MEMB_OPTIMAL, NULL))
        {
            return;
        }
        j = shell_compute_syntac(i);
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

    parti *flpr;
    parti *plpr;
    while (true)
    {
        if (fluf->size_par != fluf->size_set)
        {
            (*fllv)++;
        }
        if (pluf->size_par != pluf->size_set)
        {
            (*pllv)++;
        }
        if (fluf->size_par == fluf->size_set || pluf->size_par == pluf->size_set)
        {
            break;
        }
        flpr = ufind_to_parti(fluf);
        plpr = ufind_to_parti(pluf);
        delete_ufind(fluf);
        delete_ufind(pluf);
        if (*fllv % 2 == 0)
        {
            fluf = iden_lpolc_mono(objects[j].mor->obj, flpr);
            pluf = iden_rpolc_mono(objects[j].mor->obj, plpr);
        }
        else
        {
            fluf = iden_rpolc_mono(objects[j].mor->obj, flpr);
            pluf = iden_lpolc_mono(objects[j].mor->obj, plpr);
        }
        delete_parti(flpr);
        delete_parti(plpr);
    }
    delete_ufind(fluf);
    delete_ufind(pluf);
}

bool shell_fphiera(classes cl, int i, FILE *out)
{

    if (i == -1)
    {
        return false;
    }
    // int j = shell_compute_syntac(i);
    // if (j == -1) {
    //     return false;
    // }

    // The name of the base class is given by the first argument.
    // La classe

    if (!class_is_basis(cl))
    {
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
    if (lvfl == -1)
    {
        sprintf(message, "The input does not belong to TL(%s).", class_names[cl]);
    }
    else if (lvfl == lvpl)
    {
        sprint_power_utf8(lvfl, supscript);
        sprintf(message, "The least level containing the input is FL%s(%s) ⋂ PL%s(%s).", supscript, class_names[cl], supscript, class_names[cl]);
    }
    else
    {
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

static ulong exa_size = 0;     // Size of exa_array.
static ulong exa_elem = 0;     // Number of DFAs stored in the exa_array.
static dfa **exa_array = NULL; // Array that store the example DFAs (size exa_size).

static void exa_init(uint size)
{
    size = max(size, 2);         // Ensure the size is at least 2.
    exa_size = size;             // The size of the array is the number of states in the morphism.
    exa_elem = 0;                // The number of elements in the array is initially 0.
    MALLOC(exa_array, exa_size); // Allocate the array of DFAs.
}

static void exa_delete(void)
{
    free(exa_array);  // Free the array of DFAs.
    exa_array = NULL; // Reset the pointer to NULL.
    exa_size = 0;     // Reset the size of the array.
    exa_elem = 0;     // Reset the number of elements in the array.
}

static void exa_grow(void)
{
    exa_size <<= 1;               // Double the size of the array.
    REALLOC(exa_array, exa_size); // Reallocate the array to the new size.
}

static bool exa_equal(uint i, uint j)
{
    dfa *A1 = exa_array[i];
    dfa *A2 = exa_array[j];

    if (A1->trans->size_graph != A2->trans->size_graph || A1->trans->size_alpha != A2->trans->size_alpha || A1->nb_finals != A2->nb_finals || A1->initial != A2->initial)
    {
        return false; // If the sizes of the graphs or alphabets are different, the DFAs are not equal.
    }
    for (uint q = 0; q < A1->trans->size_graph; q++)
    {
        for (uint a = 0; a < A1->trans->size_alpha; a++)
        {
            if (A1->trans->edges[q][a] != A2->trans->edges[q][a])
            {
                return false;
            }
        }
    }
    for (uint h = 0; h < A1->nb_finals; h++)
    {
        if (A1->finals[h] != A2->finals[h])
        {
            return false; // If any final state is different, the DFAs are not equal.
        }
    }
    return true;
}

static uint exa_hash(uint i, uint size_hash)
{
    uint hash = 0;

    uint nb = exa_array[i]->trans->size_graph * exa_array[i]->trans->size_alpha + exa_array[i]->nb_finals + 1;
    uint a = 0x9e3779b9; // fractional bits of the golden ratio

    hash = (hash * (nb + 1) + exa_array[i]->initial * a) % size_hash;

    for (uint j = 0; j < exa_array[i]->trans->size_graph; j++)
    {
        for (uint b = 0; b < exa_array[i]->trans->size_alpha; b++)
        {
            hash = (hash * (nb + 1) + exa_array[i]->trans->edges[j][b] * a) % size_hash;
        }
    }

    for (uint j = 0; j < exa_array[i]->nb_finals; j++)
    {
        hash = (hash * (nb + 1) + exa_array[i]->finals[j] * a) % size_hash;
    }

    return hash;
}

static void shell_memb_file_exall(exall_profile *theprofile)
{
    fprintf(stdout, "#### Mode: Class separation.\n");
    fprintf(stdout, "#### Forbidden classes: ");
    for (uint i = 0; i < theprofile->nblow; i++)
    {
        fprintf(stdout, "%s ", class_names[theprofile->low[i]]);
    }
    fprintf(stdout, "\n#### Required classes: ");
    for (uint i = 0; i < theprofile->nbhigh; i++)
    {
        fprintf(stdout, "%s ", class_names[theprofile->high[i]]);
    }

    fprintf(stdout, "\n#### Number of states: %d\n", theprofile->states);
    fprintf(stdout, "#### Alphabet size: %d\n", theprofile->alpha);
    fprintf(stdout, "#### Starting from test number %llu.\n", (unsigned long long)theprofile->done);
    fprintf(stdout, "#### Already found %llu example languages.\n", (unsigned long long)theprofile->nb_found);

    dfa_enum *E = dfa_enum_init(theprofile->states, theprofile->alpha);
    uint64_t count = 0;
    uint64_t posit = 0;

    exa_init(128);
    hash_table *thehash = create_hash_table(8, &exa_hash, &exa_equal);

    // Skipping the automata already handled and loading them into the hash table.
    while (count < theprofile->done)
    {
        if (count == theprofile->found[posit])
        {
            dfa *A = dfa_enum_to_dfa(E);
            dfa *MINI = dfa_hopcroft(A);
            dfa_delete(A);
            exa_array[exa_elem] = dfa_mini_canonical_copy(MINI);
            dfa_delete(MINI);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem)
            {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size)
                {
                    exa_grow(); // Grow the array if needed.
                }
                posit++;
            }
            else
            {
                dfa_delete(exa_array[exa_elem]);
            }

            posit++;
        }
        count++;
        dfa_enum_next(E);
    }

    count = 0;
    posit = 0;

    listen_interrupt(); // Start listening for user interruptions.

    // Tests
    do
    {
        if (interrupt_flag)
        {
            fprintf(stdout, "#### User interruption detected. Stopping the example generation.\n");
            break;
        }

        dfa *A = dfa_enum_to_dfa(E);
        int j = object_add_automaton_dfa(NULL, A);

        bool res = true;
        for (uint h = 0; h < theprofile->nbhigh; h++)
        {
            res = res && class_membership[theprofile->high[h]](j, MEMB_OPTIMAL, NULL);
            if (!res)
            {
                break; // If the automaton does not belong to a required class, we stop checking.
            }
        }

        if (res)
        {
            for (uint h = 0; h < theprofile->nblow; h++)
            {
                res = res && !class_membership[theprofile->low[h]](j, MEMB_OPTIMAL, NULL);
                if (!res)
                {
                    break; // If the automaton belongs to a forbidden class, we stop checking.
                }
            }
        }

        if (j != -1 && res)
        {
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(j)].obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem)
            {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size)
                {
                    exa_grow(); // Grow the array if needed.
                }
                if (theprofile->nb_found >= theprofile->size_found)
                {
                    // If the array is full, we grow it.
                    theprofile->size_found <<= 1;
                    REALLOC(theprofile->found, theprofile->size_found);
                }
                theprofile->found[theprofile->nb_found] = theprofile->done;
                posit++;
                theprofile->nb_found++;
            }
            else
            {
                dfa_delete(exa_array[exa_elem]);
            }
        }

        if (count > 0 && count % 20000 == 0)
        {
            fprintf(stdout, "#### %llu tests done so far. %llu examples found.\n", (unsigned long long)count, (unsigned long long)posit);
        }
        theprofile->done++;
        count++;

        object_free(j);
    } while (dfa_enum_next(E));

    ignore_interrupt(); // Stop listening for interrupts.
    if (!E->run)
    {
        theprofile->finished = true; // If the enumeration is finished, we set the profile as finished.
    }
    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.

    char buffer[64];
    for (uint i = 0; i < exa_elem; i++)
    {
        sprintf(buffer, "EXA%04d", i);
        object_add_automaton_dfa(buffer, exa_array[i]);
    }

    exa_delete(); // Free the example array.

    fprintf(stdout, "#### %llu new tests in total. Found %llu new example languages.\n", (unsigned long long)count, (unsigned long long)posit);
}

static void shell_memb_file_fpdet(exall_profile *theprofile)
{
    switch (theprofile->mode)
    {
    case EXAGEN_FPHIERA:
        fprintf(stdout, "#### Mode: Future/Past hierarchy.\n");
        break;
    case EXAGEN_DETHIERA:
        fprintf(stdout, "#### Mode: Negation hierarchy.\n");
        break;
    default:
        fprintf(stdout, "#### Error: Unknown mode.\n");
        free(theprofile->found);
        return;
        break;
    }

    if (!class_is_basis(theprofile->low[0]))
    {
        fprintf(stdout, "#### The class %s is not a valid basis.\n", class_names[theprofile->low[0]]);
        return;
    }

    fprintf(stdout, "#### Basis: %s.\n", class_names[theprofile->low[0]]);
    fprintf(stdout, "#### Desired level: %u.\n", theprofile->nblow);

    fprintf(stdout, "#### Number of states: %d\n", theprofile->states);
    fprintf(stdout, "#### Alphabet size: %d\n", theprofile->alpha);
    fprintf(stdout, "#### Starting from test number %llu.\n", (unsigned long long)theprofile->done);
    fprintf(stdout, "#### Already found %llu example languages.\n", (unsigned long long)theprofile->nb_found);

    dfa_enum *E = dfa_enum_init(theprofile->states, theprofile->alpha);
    uint64_t count = 0;
    uint64_t posit = 0;

    exa_init(128);
    hash_table *thehash = create_hash_table(8, &exa_hash, &exa_equal);

    // Skipping the automata already handled and loading them into the hash table.
    while (count < theprofile->done)
    {
        if (count == theprofile->found[posit])
        {
            dfa *A = dfa_enum_to_dfa(E);
            dfa *MINI = dfa_hopcroft(A);
            dfa_delete(A);
            exa_array[exa_elem] = dfa_mini_canonical_copy(MINI);
            dfa_delete(MINI);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem)
            {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size)
                {
                    exa_grow(); // Grow the array if needed.
                }
                posit++;
            }
            else
            {
                dfa_delete(exa_array[exa_elem]);
            }

            posit++;
        }
        count++;
        dfa_enum_next(E);
    }

    count = 0;
    posit = 0;

    listen_interrupt(); // Start listening for user interruptions.

    bool fp = (theprofile->mode == EXAGEN_FPHIERA);

    // Tests
    do
    {
        if (interrupt_flag)
        {
            fprintf(stdout, "#### User interruption detected. Stopping the example generation.\n");
            break;
        }

        //       dfa_enum_print(E);
        dfa *A = dfa_enum_to_dfa(E);

        int i = object_add_automaton_dfa(NULL, A);

        short levelj;
        if (fp)
        {
            short pllv;
            short fllv;
            shell_fphiera_aux(i, theprofile->low[0], &fllv, &pllv);
            levelj = max(pllv, fllv);
        }
        else
        {
            levelj = shell_neghiera_aux(i, theprofile->low[0]);
        }

        // view_nfa(objects[j].aut->obj_nfa);

        if (i != -1 && levelj >= (short)theprofile->nblow)
        {
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(i)].obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem)
            {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size)
                {
                    exa_grow(); // Grow the array if needed.
                }
                if (theprofile->nb_found >= theprofile->size_found)
                {
                    // If the array is full, we grow it.
                    theprofile->size_found <<= 1;
                    REALLOC(theprofile->found, theprofile->size_found);
                }
                theprofile->found[theprofile->nb_found] = theprofile->done;
                posit++;
                theprofile->nb_found++;
            }
            else
            {
                dfa_delete(exa_array[exa_elem]);
            }
        }
        object_free(i);

        if (count > 0 && count % 20000 == 0)
        {
            fprintf(stdout, "#### %llu tests done so far. %llu examples found.\n", (unsigned long long)count, (unsigned long long)posit);
        }
        theprofile->done++;
        count++;

    } while (dfa_enum_next(E));

    ignore_interrupt(); // Stop listening for interrupts.
    if (!E->run)
    {
        theprofile->finished = true; // If the enumeration is finished, we set the profile as finished.
    }
    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.

    char buffer[64];
    for (uint i = 0; i < exa_elem; i++)
    {
        sprintf(buffer, "EXA%04d", i);
        object_add_automaton_dfa(buffer, exa_array[i]);
    }

    exa_delete(); // Free the example array.

    fprintf(stdout, "#### %llu new tests in total. Found %llu new example languages.\n", (unsigned long long)count, (unsigned long long)posit);
}

void shell_memb_file(const char *filename)
{
    exall_profile theprofile;
    theprofile.found = NULL;
    files_read_exall(filename, &theprofile);
    if (theprofile.states < 1)
    {
        fprintf(stderr, "#### The number of states must be at least 1.\n");
        free(theprofile.found);
        return;
    }
    if (theprofile.alpha < 1)
    {
        fprintf(stderr, "#### The alphabet size must be at least 1.\n");
        free(theprofile.found);
        return;
    }

    if (theprofile.finished)
    {
        fprintf(stdout, "#### The example generation is already finished.\n");
        fprintf(stdout, "#### Found %llu example languages.\n", (unsigned long long)theprofile.nb_found);
        free(theprofile.found);
        return;
    }
    fprintf(stdout, "#### Continuing example generation.\n");
    switch (theprofile.mode)
    {
    case EXAGEN_ALL:
        shell_memb_file_exall(&theprofile);
        break;
    case EXAGEN_FPHIERA:
        shell_memb_file_fpdet(&theprofile);
        break;
    default:
        fprintf(stdout, "#### Error: Unknown mode.\n");
        free(theprofile.found);
        return;
        break;
    }

    files_save_exall(filename, &theprofile); // Save the profile to the file.
    free(theprofile.found);
}

void shell_file_retrieve(const char *filename, const char *pref)
{
    exall_profile theprofile;
    files_read_exall(filename, &theprofile);

    dfa_enum *E = dfa_enum_init(theprofile.states, theprofile.alpha);
    uint64_t count = 0;
    uint64_t posit = 0;

    theprofile.nb_found = 0;

    dfa **temp;
    uint64_t sizetab = 1024;
    MALLOC(temp, sizetab);

    listen_interrupt(); // Start listening for user interruptions.

    // Tests
    while (count < theprofile.done)
    {
        if (interrupt_flag)
        {
            fprintf(stdout, "#### User interruption detected.\n");
            break;
        }
        if (count == theprofile.found[posit])
        {
            if (posit >= sizetab)
            {
                sizetab <<= 1;
                REALLOC(temp, sizetab);
            }
            temp[posit++] = dfa_enum_to_dfa(E);
            // object_add_automaton_dfa_nocheck(buffer, A);
        }
        count++;
        dfa_enum_next(E);
    }
    ignore_interrupt(); // Stop listening for interrupts.
    object_add_automaton_dfa_family(pref, temp, posit);
    free(temp);

    dfa_enum_free(E);

    free(theprofile.found);
}

void shell_exall(classes *low, int nblow, classes *high, int nbhigh, int states, int alpha, const char *prefix)
{

    states = max(states, 1);
    alpha = max(alpha, 1);

    dfa_enum *E = dfa_enum_init(states, alpha);
    ulong count = 0;

    exa_init(128);
    hash_table *thehash = create_hash_table(8, &exa_hash, &exa_equal);

    // listening_mode(timeout_value);
    listen_interrupt();

    while (dfa_enum_next(E))
    {

        if (interrupt_flag)
        {
            fprintf(stdout, "#### User interruption detected. Stopping the example generation.\n");
            break;
        }

        //       dfa_enum_print(E);
        dfa *A = dfa_enum_to_dfa(E);
        // view_dfa(A);

        int j = object_add_automaton_dfa(NULL, A);

        bool res = true;

        for (int h = 0; h < nbhigh; h++)
        {
            if (!class_membership[high[h]](j, MEMB_OPTIMAL, NULL))
            {
                res = false;

                break; // If the automaton does not belong to a required class, we stop checking.
            }
        }

        if (res)
        {
            for (int h = 0; h < nblow; h++)
            {
                if (class_membership[low[h]](j, MEMB_OPTIMAL, NULL))
                {
                    res = false;
                    break; // If the automaton belongs to a forbidden class, we stop checking.
                }
            }
        }

        count++;

        if (j != -1 && res)
        {
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(j)].obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem)
            {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size)
                {
                    exa_grow(); // Grow the array if needed.
                }
            }
            else
            {
                dfa_delete(exa_array[exa_elem]);
            }
        }

        if (count % 20000 == 0)
        {
            fprintf(stdout, "#### %lu tests done so far. %lu examples found.\n", count, exa_elem);
        }

        object_free(j);
    }

    ignore_interrupt(); // Stop listening for interrupts.

    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.

    fprintf(stdout, "#### %lu tests in total. Found %lu example languages.\n", count, exa_elem);

    object_add_automaton_dfa_family(prefix, exa_array, exa_elem);

    exa_delete(); // Free the example array.

    return;
}

void shell_exall_dethiera(classes cl, int level, int states, int alpha, const char *prefix, bool neg)
{
    if (!class_is_basis(cl))
    {
        fprintf(stdout, "#### The class %s is not a valid basis.\n", class_names[cl]);
        return;
    }

    dfa_enum *E = dfa_enum_init(states, alpha);
    uint count = 0;

    exa_init(128);
    hash_table *thehash = create_hash_table(8, &exa_hash, &exa_equal);

    while (dfa_enum_next(E))
    {

        //       dfa_enum_print(E);
        dfa *A = dfa_enum_to_dfa(E);

        int i = object_add_automaton_dfa(NULL, A);

        // // TODO: Fix when shell_compute_syntac fails
        // int k = shell_compute_syntac(j);

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
        if (neg)
        {
            levelj = shell_neghiera_aux(i, cl);
        }
        else
        {
            short pllv;
            short fllv;
            shell_fphiera_aux(i, cl, &fllv, &pllv);
            levelj = max(pllv, fllv);
        }

        count++;
        // view_nfa(objects[j].aut->obj_nfa);

        if (i != -1 && levelj >= level)
        {
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(i)].obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem)
            {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size)
                {
                    exa_grow(); // Grow the array if needed.
                }
            }
            else
            {
                dfa_delete(exa_array[exa_elem]);
            }
        }

        if (count % 20000 == 0)
        {
            fprintf(stdout, "#### %d tests done so far. %lu were positive\n", count, exa_elem);
        }

        object_free(i);
    }

    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.

    fprintf(stdout, "#### %d tests in total. Found %lu example languages.\n", count, exa_elem);

    object_add_automaton_dfa_family(prefix, exa_array, exa_elem);
    // object_delete_prefix(prefix);
    // char buffer[64];
    // for (uint i = 0; i < exa_elem; i++) {
    //     sprintf(buffer, "%s%04d", prefix, i);
    //     object_add_automaton_dfa_nocheck(buffer, exa_array[i]);
    // }

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
//         int k = shell_compute_syntac(j);
//         if (k == -1) {
//             fprintf(stdout, "#### Test %d: Syntactic monoid too large.\n", i + 1);
//             continue;
//         }
//         bool not = false;
//         for (int h = 0; h < n - 5; h++) {

//             not = not || class_membership[low[h]](k, NULL);
//         }
//         // view_nfa(objects[j].aut->obj_nfa);
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

#define SECNANO 1000000000UL

// #define STOPMY 97547

void shell_make_timestats(classes cl, int states, int alpha)
{

    states = max(states, 1);
    alpha = max(alpha, 1);

    dfa_enum *E = dfa_enum_init(states, alpha);
    ulong count = 0;

    // listening_mode(timeout_value);
    listen_interrupt();

    struct timespec before;
    struct timespec after;
    ulong nano = 0;
    ulong positive = 0;

    while (dfa_enum_next(E))
    {
        // if (count == STOPMY)
        // {
        //     fprintf(stdout, "#### Warmup phase done 1.\n");
        // }

        if (interrupt_flag)
        {
            fprintf(stdout, "#### User interruption detected. Stopping the example generation.\n");
            break;
        }

        //       dfa_enum_print(E);
        dfa *A = dfa_enum_to_dfa(E);

        int j = object_add_automaton_dfa(NULL, A);
        // if (count == STOPMY)
        // {
        //     files_save_object(&objects[j], "toto.dfa");
        // }
        timespec_get(&before, TIME_UTC);
        bool res = class_membership[cl](j, memb_mode, NULL);
        timespec_get(&after, TIME_UTC);
        nano += (after.tv_nsec - before.tv_nsec) + (after.tv_sec - before.tv_sec) * SECNANO;
        count++;
        if (res)
        {
            positive++;
        }

        // if (count == STOPMY)
        // {
        //     fprintf(stdout, "#### Warmup phase done 2.\n");
        //     view_dfa(objects[j].obj_dfa);
        //     files_save_object(&objects[j], "warmup.dfa");
        // }

        if (count % 20000000 == 0)
        {
            fprintf(stdout, "#### %lu tests done so far. %lu positive results.\n", count, positive);
        }

        object_free(j);

        // if (count == STOPMY)
        // {
        //     fprintf(stdout, "#### Warmup phase done 3.\n");
        // }
    }

    ignore_interrupt(); // Stop listening for interrupts.

    fprintf(stdout, "#### %lu tests in total. %lu positive results.\n", count, positive);

    ulong res = nano / count;
    ulong micro = res / 1000;
    ulong rem = res % 1000;
    switch (memb_mode)
    {
    case MEMB_OPTIMAL:
        fprintf(stdout, "#### Average time of a single test (optimized mode): %lu.%lu microseconds.\n", micro, rem);
        break;
    case MEMB_MINIMAL:
        fprintf(stdout, "#### Average time of a single test (automata mode): %lu.%lu microseconds.\n", micro, rem);
        break;
    case MEMB_SYNTAC:
        fprintf(stdout, "#### Average time of a single test (monoids mode): %lu.%lu microseconds.\n", micro, rem);
        break;
    default:
        break;
    }

    return;
}

void shell_bugsearch(classes cl, int states, int alpha, const char *prefix)
{

    states = max(states, 1);
    alpha = max(alpha, 1);

    dfa_enum *E = dfa_enum_init(states, alpha);
    uint count = 0;

    exa_init(128);
    hash_table *thehash = create_hash_table(8, &exa_hash, &exa_equal);

    // listening_mode(timeout_value);
    listen_interrupt();

    while (dfa_enum_next(E))
    {

        if (interrupt_flag)
        {
            fprintf(stdout, "#### User interruption detected. Stopping the example generation.\n");
            break;
        }

        //       dfa_enum_print(E);
        dfa *A = dfa_enum_to_dfa(E);

        // view_dfa(A);

        int j = object_add_automaton_dfa(NULL, A);

        // files_save_object(&objects[j], "last.dfa");
        //  if (count == 888)
        //  {
        //      files_save_object(&objects[j], "bug888.dfa");
        //  }

        bool resauto = class_membership[cl](j, MEMB_MINIMAL, NULL);
        bool resmono = class_membership[cl](j, MEMB_OPTIMAL, NULL);
        count++;

        if (j != -1 && resauto != resmono)
        {
            // fprintf(stdout, "#### Test %d: Automaton %d. Auto: %d. Mono: %d.\n", count, j, resauto, resmono);
            // view_dfa(A);
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(j)].obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem)
            {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size)
                {
                    exa_grow(); // Grow the array if needed.
                }
            }
            else
            {
                dfa_delete(exa_array[exa_elem]);
            }
        }

        if (count % 20000 == 0)
        {
            fprintf(stdout, "#### %d tests done so far. %lu bugs found.\n", count, exa_elem);
        }

        object_free(j);
    }

    ignore_interrupt(); // Stop listening for interrupts.

    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.

    fprintf(stdout, "#### %d tests in total. Found %lu bugs.\n", count, exa_elem);

    object_add_automaton_dfa_family(prefix, exa_array, exa_elem);
    // object_delete_prefix(prefix); // Delete all objects with chosen prefix.

    // char buffer[64];
    // for (uint i = 0; i < exa_elem; i++) {
    //     sprintf(buffer, "%s%04d", prefix, i);
    //     object_add_automaton_dfa_nocheck(buffer, exa_array[i]);
    // }
    exa_delete(); // Free the example array.

    return;
}

bool shell_exall_dfatest(void)
{

    dfa_enum *E = dfa_enum_init(4, 2);
    uint count = 0;

    exa_init(128);
    hash_table *thehash = create_hash_table(8, &exa_hash, &exa_equal);

    while (dfa_enum_next(E))
    {

        dfa *A = dfa_enum_to_dfa(E);

        int j = object_add_automaton_dfa(NULL, A);

        int jm = shell_compute_minimal(j);
        int k = shell_compute_syntac(j);

        if (k == MEMORY_LIMIT)
        {
            fprintf(stdout, "#### Test %d: Syntactic monoid too large.\n", count);
            continue;
        }
        if (k == TIMEOUT_OCCURRED)
        {
            fprintf(stdout, "#### Test %d: timeout occurred.\n", count);
            continue;
        }
        if (k == INTERRUPTION)
        {
            fprintf(stdout, "#### Test %d: user interruption.\n", count);
            break;
        }
        bool resmono = class_membership[CL_TL2MOD](k, MEMB_SYNTAC, NULL);
        // bool resauto = class_membership[CL_UL](j, NULL);
        bool resauto = is_dabpgp_dfa(objects[jm].obj_dfa, BA_MOD, NULL);
        // bool resauto = is_rtrivialrivialbpgp_dfa(objects[jm].aut->obj_dfa, BA_GR, NULL);
        // bool resauto = is_rtrivialrivialgp_dfa(objects[jm].aut->obj_dfa, BA_AMT, NULL);

        count++;

        if (resmono != resauto)
        {
            exa_array[exa_elem] = dfa_mini_canonical_copy(objects[shell_compute_minimal(j)].obj_dfa);
            uint x = hash_table_insert(thehash, exa_elem);
            if (x == exa_elem)
            {
                // If this was a new automaton, we add it to the set.
                exa_elem++;
                if (exa_elem >= exa_size)
                {
                    exa_grow(); // Grow the array if needed.
                }
            }
            else
            {
                dfa_delete(exa_array[exa_elem]);
            }
        }

        if (count % 20000 == 0)
        {
            fprintf(stdout, "#### %d tests done so far. %lu were positive\n", count, exa_elem);
        }

        object_free(j);
    }

    dfa_enum_free(E);
    delete_hash_table(thehash); // Delete the hash table.

    fprintf(stdout, "#### %d tests in total. Found %lu example languages.\n", count, exa_elem);
    char buffer[64];
    for (uint i = 0; i < exa_elem; i++)
    {
        sprintf(buffer, "EXA%04d", i);
        object_add_automaton_dfa(buffer, exa_array[i]);
    }

    exa_delete(); // Free the example array.

    return false;
}
