#include "shell_memb.h"
#include "monoid_display.h"

bool (*class_membership[CL_END])(int, FILE*) = { NULL };

void init_membership(void) {

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
    class_membership[CL_JORBAMTP] = shell_membership_amtp;
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

/*********************/
/* General functions */
/*********************/

uint m_cexa[6];

static uint* get_counter(FILE* out) {
    if (out) {
        return m_cexa;
    }
    return NULL;
}

char m_buffers[6][100];

static char* m_sprint(int j, uint i) {
    mor_sprint_name_utf8(objects[j]->mor->obj, m_cexa[i], m_buffers[i]);
    return m_buffers[i];
}


bool shell_morprop_monotriv(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is trivial.\n", name);
    }
    if (is_trivial_monoid(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is trivial.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is NOT trivial.\n", name);
            fprintf(out, "#### For instance, the elements %s and %s are distinct.\n", m_sprint(j, 0), m_sprint(j, 1));
        }
        return false;
    }
}

bool shell_morprop_semitriv(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is trivial.\n", name);
    }
    if (is_trivial_semigroup(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is trivial.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is NOT trivial.\n", name);
            fprintf(out, "#### For instance, the elements %s and %s are distinct.\n", m_sprint(j, 0), m_sprint(j, 1));
        }
        return false;
    }
}

bool shell_morprop_orbtriv(int j, orbits_type type, char* orbs, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s-orbits for the %s are trivial.\n", orbs, name);
    }
    if (is_trivial_orbmono(shell_compute_orbits(j, type, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-orbits are trivial.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-orbit of %s is not trivial.\n", orbs, m_sprint(j, 2));
            fprintf(out, "#### For instance, the elements %s and %s are distinct.\n", m_sprint(j, 0), m_sprint(j, 1));
        }
        return false;
    }
    return true;
}


bool shell_morprop_letterind(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s maps every letter a ∊ A to the same element.\n", name);
    }

    if (is_letterind_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s maps every letter a ∊ A to the same element.\n", name);
        }
        return true;

    }
    else {
        if (out) {
            fprintf(out, "#### The %s maps the letters ", name);
            fprint_letter_utf8(objects[j]->mor->obj->alphabet[0], out);
            fprintf(out, " and ");
            fprint_letter_utf8(objects[j]->mor->obj->alphabet[m_cexa[0]], out);
            fprintf(out, " to distinct elements.\n");
        }
        return false;
    }
}

bool shell_morprop_monogroup(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is a group.\n", name);
    }
    if (is_group_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is a group.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not a group.\n", name);
            fprintf(out, "#### For instance, the element %s has no inverse.\n", m_sprint(j, 0));
        }
        return false;
    }
    return true;
}

bool shell_morprop_semigroup(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is a group.\n", name);
    }
    if (is_group_semigroup(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is a group.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not a group.\n", name);
            fprintf(out, "#### For instance, the elements %s and %s are not J-equivalent.\n", m_sprint(j, 0), m_sprint(j, 1));
        }
        return false;
    }
    return true;
}


bool shell_morprop_monocom(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is commutative.\n", name);
    }
    if (is_comm_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is commutative.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not commutative.\n", name);
            fprintf(out, "#### For instance, %s%s ≠ %s%s.\n", m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3));
        }
        return false;
    }
    return true;
}

bool shell_morprop_kercom(int j, kernel_type type, char* ker, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s-kernel of the %s is commutative.\n", ker, name);
    }
    if (is_comm_subsemi(shell_compute_ker(j, type, LV_GREG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-kernel is commutative.\n", ker);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-kernel is not commutative.\n", ker);
            fprintf(out, "#### For instance, %s%s ≠ %s%s.\n", m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3));
        }
        return false;
    }
    return true;
}


bool shell_morprop_orbcom(int j, orbits_type type, char* orbs, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s-orbits for the %s are commutative.\n", orbs, name);
    }
    if (is_com_orbmono(shell_compute_orbits(j, type, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-orbits are commutative.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-orbit of %s is not commutative.\n", orbs, m_sprint(j, 4));
            fprintf(out, "#### For instance, %s%s ≠ %s%s.\n", m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3));
        }
        return false;
    }
    return true;
}


bool shell_morprop_semigencom(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s S satisfies the equation\n", name);
        fprintf(out, "     erfsetf = etfserf for all r,s,t ∊ S and e,f ∊ E(S).\n");
    }
    if (is_comm_ltt_mono(shell_compute_orbits(j, ORB_DD, LV_GREG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s satisfies the equation.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The equation fails for r = %s, s = %s, t = %s, e = %s and f = %s.\n", m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3), m_sprint(j, 4));
        }
        return false;
    }
    return true;
}


bool shell_morprop_monoidem(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is idempotent.\n", name);
    }
    if (is_idem_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is idempotent.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not idempotent.\n", name);
            fprintf(out, "#### For instance, %s ≠ %s%s.\n", m_sprint(j, 0), m_sprint(j, 0), m_sprint(j, 0));
        }
        return false;
    }
    return true;
}

bool shell_morprop_keridem(int j, kernel_type type, char* ker, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s-kernel of the %s is idempotent.\n", ker, name);
    }
    if (is_idem_subsemi(shell_compute_ker(j, type, LV_GREG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-kernel is idempotent.\n", ker);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-kernel is not idempotent.\n", ker);
            fprintf(out, "#### For instance, %s%s ≠ %s.\n", m_sprint(j, 0), m_sprint(j, 0), m_sprint(j, 0));
        }
        return false;
    }
    return true;
}


bool shell_morprop_orbidem(int j, orbits_type type, char* orbs, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s-orbits for the %s are idempotent.\n", orbs, name);
    }
    if (is_idem_orbmono(shell_compute_orbits(j, type, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-orbits are idempotent.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-orbit of %s is not idempotent.\n", orbs, m_sprint(j, 4));
            fprintf(out, "#### For instance, %s%s ≠ %s.\n", m_sprint(j, 0), m_sprint(j, 0), m_sprint(j, 0));
        }
        return false;
    }
    return true;
}



bool shell_morprop_monogreen(int j, green_relation R, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is %c-trivial.\n", name, green_rel_array[R]);
    }
    if (is_gtrivial_mono(objects[j]->mor->obj, R, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is %c-trivial.\n", name, green_rel_array[R]);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not %c-trivial.\n", name, green_rel_array[R]);
            fprintf(out, "#### For instance, %s %c %s.\n", m_sprint(j, 0), green_rel_array[R], m_sprint(j, 1));
        }
        return false;
    }
    return true;
}

bool shell_morprop_kergreen(int j, green_relation R, kernel_type type, char* ker, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s-kernel of the %s is %c-trivial.\n", ker, name, green_rel_array[R]);
    }
    if (is_gtrivial_subsemi(shell_compute_ker(j, type, LV_REG), R, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-kernel is %c-trivial.\n", ker, green_rel_array[R]);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-kernel is not %c-trivial.\n", ker, green_rel_array[R]);
            fprintf(out, "#### For instance, %s %c %s.\n", m_sprint(j, 0), green_rel_array[R], m_sprint(j, 1));
        }
        return false;
    }
    return true;
}


bool shell_morprop_orbgreen(int j, green_relation R, orbits_type type, char* orbs, char* name, FILE* out) {

    if (out) {
        fprintf(out, "#### Checking if the %s-orbits for the %s are %c-trivial.\n", orbs, name, green_rel_array[R]);
    }
    if (is_gtrivial_orbmono(shell_compute_orbits(j, type, LV_REG), R, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-orbits are %c-trivial.\n", orbs, green_rel_array[R]);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-orbit of %s is not %c-trivial.\n", orbs, m_sprint(j, 2), green_rel_array[R]);
            fprintf(out, "#### For instance, %s %c %s.\n", m_sprint(j, 0), green_rel_array[R], m_sprint(j, 1));
        }
        return false;
    }
    return true;
}


bool shell_morprop_monoda(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is in DA.\n", name);
    }
    if (is_da_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is in DA.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not in DA.\n", name);
            fprintf(out, "#### For instance, %s is a non-idempotent regular element.\n", m_sprint(j, 0));
        }
        return false;
    }
    return true;
}

bool shell_morprop_kerda(int j, kernel_type type, char* ker, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s-kernel of the %s is in DA.\n", ker, name);
    }
    if (is_da_subsemi(shell_compute_ker(j, type, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-kernel is in DA.\n", ker);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-kernel is not in DA.\n", ker);
            fprintf(out, "#### For instance, %s is a non-idempotent regular element.\n", m_sprint(j, 0));
        }
        return false;
    }
    return true;
}


bool shell_morprop_orbda(int j, orbits_type type, char* orbs, char* name, FILE* out) {

    if (out) {
        fprintf(out, "#### Checking if the %s-orbits for the %s are in DA.\n", orbs, name);
    }
    if (is_da_orbmono(shell_compute_orbits(j, type, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s-orbits are in DA.\n", orbs);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s-orbit of %s is not in DA.\n", orbs, m_sprint(j, 2));
            fprintf(out, "#### For instance, %s is a non-idempotent regular element.\n", m_sprint(j, 0));
        }
        return false;
    }
    return true;
}



bool shell_morprop_monojsat(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s M satisfies the inequation 1 ⩽ s for all s ∊ M.\n", name);
    }
    if (is_jsat_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The inequation is satisfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The inequation fails for s = %s.\n", m_sprint(j, 0));
        }
        return false;
    }
    return true;
}

bool shell_morprop_monoejsat(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s M satisfies the inequation 1 ⩽ e for all e ∊ E(M).\n", name);
    }
    if (is_ejsat_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The inequation is satisfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The inequation fails for e = %s.\n", m_sprint(j, 0));
        }
        return false;
    }
    return true;
}

bool shell_morprop_kerjsat(int j, kernel_type type, char* ker, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s-kernel N of the %s satisfies\n", ker, name);
        fprintf(out, "     the inequation 1 ⩽ s for all s ∊ N.\n");
    }
    if (is_jsat_subsemi(shell_compute_ker(j, type, LV_GREG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The inequation is satisfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The inequation fails for s = %s ∊ N.\n", m_sprint(j, 0));
        }
        return false;
    }
    return true;
}


bool shell_morprop_orbjsat(int j, orbits_type type, char* orbs, char* name, FILE* out) {

    if (out) {
        fprintf(out, "#### Checking if every %s-orbit Mₑ for the %s satisfies\n", orbs, name);
        fprintf(out, "     the inequation e ⩽ s for all s ∊ Mₑ.\n");
    }
    if (is_jsat_orbmono(shell_compute_orbits(j, type, LV_GREG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### All %s-orbits satisfy the inequation.\n", orbs);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The inequation fails in the %s-orbit of e = %s for s = %s ∊ Mₑ.\n", orbs, m_sprint(j, 1), m_sprint(j, 0));
        }
        return false;
    }
    return true;
}



bool shell_morprop_bpgroupeq(int j, kernel_type type, char* ker, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s ⍺:A* → M satisfies the equation (qr)ʷst(st)ʷ = (qr)ʷqt(st)ʷ\n", name);
        fprintf(out, "     for all %s-pairs (q,s) ∊ M² and all r,t ∊ M.\n", ker);
    }

    bool res;
    switch (type) {
    case KER_MOD:
        res = is_bpolmod_mono(shell_compute_ker(j, type, LV_REG), get_counter(out));
        break;
    case KER_AMT:
        res = is_bpolamt_mono(shell_compute_ker(j, type, LV_REG), get_counter(out));
        break;
    default:
        res = false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The equation is satsfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The equation fails for the %s-pair (q,s) = (%s,%s), r = %s and t = %s.\n", ker, m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3));
        }
        return false;
    }
    return true;
}

bool shell_morprop_blockg(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s is a block group.\n", name);
    }

    if (is_blockg_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is a block group.\n", name);
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not a block group.\n", name);
            fprintf(out, "#### For instance, the idempotents %s and %s are %c-equivalent.\n", m_sprint(j, 0), m_sprint(j, 1), green_rel_array[m_cexa[2]]);
        }
        return false;
    }
    return true;
}

bool shell_mprop_semiknast(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s S satisfies Knast's equation:\n", name);
        fprintf(out, "     (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ S and e,f ∊ E(S).\n");
    }

    if (is_knast_mono(shell_compute_orbits(j, ORB_DD, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### Knast's equation is satisfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### Knast's equation fails for q = %s, r = %s, s = %s, t = %s, e = %s and f = %s.\n", m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3), m_sprint(j, 4), m_sprint(j, 5));
        }
        return false;
    }
    return true;
}

bool shell_morprop_kerknast(int j, kernel_type type, char* ker, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the strict %s-kernel S of the %s satisfies Knast's equation:\n", ker, name);
        fprintf(out, "     (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ S and e,f ∊ E(S).\n");
    }

    orbits_type otype;
    switch (type) {
    case KER_MOD:
        otype = ORB_MODP;
        break;
    case KER_AMT:
        otype = ORB_AMTP;
        break;
    case KER_GR:
        otype = ORB_GRP;
        break;
    default:
        otype = ORB_GRP;
        break;
    }


    if (is_knast_ker(shell_compute_orbits(j, otype, LV_REG), shell_compute_ker(j, type, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### Knast's equation is satisfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### Knast's equation fails for q = %s, r = %s, s = %s, t = %s, e = %s and f = %s.\n", m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3), m_sprint(j, 4), m_sprint(j, 5));
        }
        return false;
    }
    return true;
}


bool shell_morprop_knastat(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s ⍺:A* → M satisfies the equation (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all\n", name);
        fprintf(out, "     and q,r,s,t ∊ M such that {q,e,f}, {r,e,f}, {s,e,f} and {t,e,f} are PT-sets and all e,f ∊ E(M).\n");
    }

    if (is_knast_at_mono(shell_compute_orbits(j, ORB_PT, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The equation is satisfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The equation fails for q = %s, r = %s, s = %s, t = %s, e = %s and f = %s.\n", m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3), m_sprint(j, 4), m_sprint(j, 5));
        }
        return false;
    }
    return true;
}

bool shell_morprop_bpgroupeqplus(int j, orbits_type type, char* orbs, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s ⍺:A* → M satisfies the equation (eqfre)ʷsft(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ\n", name);
        fprintf(out, "     for all %s-pairs (q,s) ∊ M², all r,t ∊ M and all e,f ∊ E(⍺(A⁺)).\n", orbs);
    }

    bool res;
    switch (type) {
    case ORB_AMTP:
        res = is_bpolamtp_mono(shell_compute_orbits(j, type, LV_REG), get_counter(out));
        break;
    case ORB_GRP:
        res = is_bpolgrp_mono(shell_compute_orbits(j, type, LV_REG), get_counter(out));
    default:
        res = false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The equation is satsfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The equation fails for the %s-pair (q,s) = (%s,%s), r = %s and t = %s.\n", orbs, m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2), m_sprint(j, 3));
        }
        return false;
    }
    return true;
}


bool shell_morprop_ubp2eq(int j, orbits_type type, char* orbs, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the %s ⍺:A* → M satisfies the equation (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ\n", name);
        fprintf(out, "      all s,t ∊ M and e ∊ E(M) where s is in the %s-orbit of e.\n", orbs);
    }

    if (is_upbp_mono(shell_compute_orbits(j, type, LV_REG), get_counter(out))) {
        if (out) {
            fprintf(out, "#### The equation is satisfied\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The equation fails for s = %s, t = %s and e = %s.\n", m_sprint(j, 0), m_sprint(j, 1), m_sprint(j, 2));
        }
        return false;
    }
    return true;
}


bool shell_membership_reg(int, FILE*) { return true; }

/*****************/
/* Group classes */
/*****************/

bool shell_membership_st(int j, FILE* out) {
    return shell_morprop_monotriv(j, "syntactic monoid", out);
}

bool shell_membership_dd(int j, FILE* out) {
    return shell_morprop_semitriv(j, "syntactic semigroup", out);
}

bool shell_membership_mod(int j, FILE* out) {
    return shell_morprop_letterind(j, "syntactic morphism", out) && shell_morprop_monogroup(j, "syntactic monoid", out);
}

bool shell_membership_modp(int j, FILE* out) {
    return shell_morprop_letterind(j, "syntactic morphism", out) && shell_morprop_semigroup(j, "syntactic semigroup", out);
}

bool shell_membership_amt(int j, FILE* out) {
    return shell_morprop_monocom(j, "syntactic monoid", out) && shell_morprop_monogroup(j, "syntactic monoid", out);
}

bool shell_membership_amtp(int j, FILE* out) {
    return shell_morprop_monocom(j, "syntactic semigroup", out) && shell_morprop_semigroup(j, "syntactic semigroup", out);
}

bool shell_membership_gr(int j, FILE* out) {
    return shell_morprop_monogroup(j, "syntactic monoid", out);
}

bool shell_membership_grp(int j, FILE* out) {
    return shell_morprop_semigroup(j, "syntactic semigroup", out);
    return true;
}

/*****************/
/* AT-ATT-LT-LTT */
/*****************/

bool shell_membership_at(int j, FILE* out) {
    return shell_morprop_monocom(j, "syntactic monoid", out) && shell_morprop_monoidem(j, "syntactic monoid", out);
}

bool shell_membership_att(int j, FILE* out) {
    return shell_morprop_monocom(j, "syntactic monoid", out) && shell_morprop_monogreen(j, H_GREEN, "syntactic monoid", out);
}

bool shell_membership_lt(int j, FILE* out) {
    return shell_morprop_orbcom(j, ORB_DD, "DD", "syntactic morphism", out) && shell_morprop_orbidem(j, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_ltt(int j, FILE* out) {
    return shell_morprop_monogreen(j, H_GREEN, "syntactic semigroup", out) && shell_morprop_semigencom(j, "syntactic semigroup", out);
}

/*********************/
/* Star-free closure */
/*********************/

bool shell_membership_sf(int j, FILE* out) {
    return shell_morprop_monogreen(j, H_GREEN, "syntactic monoid", out);
}

bool shell_membership_sfmod(int j, FILE* out) {
    return shell_morprop_kergreen(j, H_GREEN, KER_MOD, "MOD", "syntactic monoid", out);
}

bool shell_membership_sfamt(int j, FILE* out) {
    return shell_morprop_kergreen(j, H_GREEN, KER_AMT, "AMT", "syntactic monoid", out);
}

bool shell_membership_sfgr(int j, FILE* out) {
    return shell_morprop_kergreen(j, H_GREEN, KER_GR, "GR", "syntactic monoid", out);
}

/**********************/
/* Polynomial closure */
/**********************/

bool shell_membership_ppt(int j, FILE* out) {
    return shell_morprop_monojsat(j, "syntactic monoid", out);
}

bool shell_membership_polmod(int j, FILE* out) {
    return shell_morprop_kerjsat(j, KER_MOD, "MOD-kernel", "syntactic morphism", out);
}

bool shell_membership_polgr(int j, FILE* out) {
    return shell_morprop_monoejsat(j, "syntactic monoid", out);
}



bool shell_membership_poldd(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_polmodp(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_polgrp(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_pol2st(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_PT, "PT", "syntactic morphism", out);
}

bool shell_membership_pol2mod(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_BPMOD, "BPol(MOD)", "syntactic morphism", out);
}

bool shell_membership_pol2amt(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_BPAMT, "BPol(AMT)", "syntactic morphism", out);
}

bool shell_membership_pol2gr(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_BPGR, "BPol(GR)", "syntactic morphism", out);
}

bool shell_membership_pol2dd(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_BPDD, "BPol(DD)", "syntactic morphism", out);
}

bool shell_membership_pol2modp(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_BPMODP, "BPol(MOD⁺)", "syntactic morphism", out);
}

bool shell_membership_pol2amtp(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_BPAMTP, "BPol(AMT⁺)", "syntactic morphism", out);
}

bool shell_membership_pol2grp(int j, FILE* out) {
    return shell_morprop_orbjsat(j, ORB_BPGRP, "BPol(GR⁺)", "syntactic morphism", out);
}

/******************************/
/* Boolean polynomial closure */
/******************************/

bool shell_membership_pt(int j, FILE* out) {
    return shell_morprop_monogreen(j, J_GREEN, "syntactic monoid", out);
}

bool shell_membership_bpolmod(int j, FILE* out) {
    return shell_morprop_bpgroupeq(j, KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_bpolamt(int j, FILE* out) {
    return shell_morprop_bpgroupeq(j, KER_AMT, "AMT", "syntactic morphism", out);
}

bool shell_membership_bpolgr(int j, FILE* out) {
    return shell_morprop_blockg(j, "syntactic monoid", out);
}

bool shell_membership_bpoldd(int j, FILE* out) {
    return shell_mprop_semiknast(j, "syntactic semigroup", out);
}

bool shell_membership_bpolmodp(int j, FILE* out) {
    return shell_morprop_kerknast(j, KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_bpolamtp(int j, FILE* out) {
    return shell_morprop_bpgroupeqplus(j, ORB_AMTP, "AMT", "syntactic morphism", out);
}


bool shell_membership_bpolgrp(int j, FILE* out) {
    return shell_morprop_bpgroupeqplus(j, ORB_GRP, "GR", "syntactic morphism", out);
}

bool shell_membership_bpol2st(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_PT, "PT", "syntactic morphism", out) && shell_morprop_knastat(j, "syntactic morphism", out);
}

bool shell_membership_jorbmod(int j, FILE* out) {
    return shell_morprop_kergreen(j, J_GREEN, KER_MOD, "MOD", "syntactic monoid", out);
}

bool shell_membership_jorbamt(int j, FILE* out) {
    return shell_morprop_kergreen(j, J_GREEN, KER_AMT, "AMT", "syntactic monoid", out);
}

bool shell_membership_jorbmodp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, J_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_jorbamtp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, J_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_jorbgrp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, J_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_jorbdd(int j, FILE* out) {
    return shell_morprop_orbgreen(j, J_GREEN, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_jorbat(int j, FILE* out) {
    return shell_morprop_orbgreen(j, J_GREEN, ORB_PT, "PT", "syntactic morphism", out);
}

bool shell_membership_knastat(int j, FILE* out) {
    return shell_morprop_knastat(j, "syntactic morphism", out);
}

/**********************************/
/* Unambiguous polynomial closure */
/**********************************/

bool shell_membership_upoldd(int j, FILE* out) {
    return shell_morprop_orbtriv(j, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_upolmodp(int j, FILE* out) {
    return shell_morprop_orbtriv(j, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_upolamtp(int j, FILE* out) {
    return shell_morprop_orbtriv(j, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_upolgrp(int j, FILE* out) {
    return shell_morprop_orbtriv(j, ORB_GRP, "GR⁺", "syntactic morphism", out);
}


bool shell_membership_ubpol2st(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_PT, "PT", "syntactic morphism", out);
}

bool shell_membership_ubpol2mod(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_BPMOD, "BPol(MOD)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2amt(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_BPAMT, "BPol(AMT)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2gr(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_BPGR, "BPol(GR)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2dd(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_BPDD, "BPol(DD)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2modp(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_BPMODP, "BPol(MOD⁺)-orbit", "syntactic morphism", out);
}


bool shell_membership_ubpol2amtp(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_BPAMTP, "BPol(AMT⁺)-orbit", "syntactic morphism", out);
}

bool shell_membership_ubpol2grp(int j, FILE* out) {
    return shell_morprop_ubp2eq(j, ORB_BPGRP, "BPol(GR⁺)-orbit", "syntactic morphism", out);
}

/******************/
/* Temporal logic */
/******************/

bool shell_membership_ul(int j, FILE* out) {
    return shell_morprop_monoda(j, "syntactic monoid", out);
}

bool shell_membership_tlmod(int j, FILE* out) {
    return shell_morprop_kerda(j, KER_MOD, "MOD-kernel", "syntactic morphism", out);
}

bool shell_membership_tlamt(int j, FILE* out) {
    return shell_morprop_kerda(j, KER_AMT, "AMT-kernel", "syntactic morphism", out);
}

bool shell_membership_tlgr(int j, FILE* out) {
    return shell_morprop_kerda(j, KER_GR, "GR-kernel", "syntactic morphism", out);
}

bool shell_membership_tldd(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_tlmodp(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_tlamtp(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_tlgrp(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_tl2st(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_PT, "TL(ST)", "syntactic morphism", out);
}

bool shell_membership_tl2mod(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_BPMOD, "TL(MOD)", "syntactic morphism", out);
}

bool shell_membership_tl2amt(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_BPAMT, "TL(AMT)", "syntactic morphism", out);
}

bool shell_membership_tl2gr(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_BPGR, "TL(GR)", "syntactic morphism", out);
}

bool shell_membership_tl2dd(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_BPDD, "TL(DD)", "syntactic morphism", out);
}

bool shell_membership_tl2modp(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_BPMODP, "TL(MOD⁺)", "syntactic morphism", out);
}

bool shell_membership_tl2amtp(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_BPAMTP, "TL(AMT⁺)", "syntactic morphism", out);
}

bool shell_membership_tl2grp(int j, FILE* out) {
    return shell_morprop_orbda(j, ORB_BPGRP, "TL(GR⁺)", "syntactic morphism", out);
}


bool shell_membership_fl(int j, FILE* out) {
    return shell_morprop_monogreen(j, L_GREEN, "syntactic monoid", out);
}

bool shell_membership_flmod(int j, FILE* out) {
    return shell_morprop_kergreen(j, L_GREEN, KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_flamt(int j, FILE* out) {
    return shell_morprop_kergreen(j, L_GREEN, KER_AMT, "AMT", "syntactic morphism", out);
}

bool shell_membership_flgr(int j, FILE* out) {
    return shell_morprop_kergreen(j, L_GREEN, KER_GR, "GR", "syntactic morphism", out);
}

bool shell_membership_fldd(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_flmodp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_flamtp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_flgrp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_fl2st(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_PT, "FL(ST)", "syntactic morphism", out);
}

bool shell_membership_fl2mod(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_BPMOD, "FL(MOD)", "syntactic morphism", out);
}

bool shell_membership_fl2amt(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_BPAMT, "FL(AMT)", "syntactic morphism", out);
}

bool shell_membership_fl2gr(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_BPGR, "FL(GR)", "syntactic morphism", out);
}

bool shell_membership_fl2dd(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_BPDD, "FL(DD)", "syntactic morphism", out);
}

bool shell_membership_fl2modp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_BPMODP, "FL(MOD⁺)", "syntactic morphism", out);
}

bool shell_membership_fl2amtp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_BPAMTP, "FL(AMT⁺)", "syntactic morphism", out);
}

bool shell_membership_fl2grp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, L_GREEN, ORB_BPGRP, "FL(GR⁺)", "syntactic morphism", out);
}

bool shell_membership_pl(int j, FILE* out) {
    return shell_morprop_monogreen(j, R_GREEN, "syntactic monoid", out);
}

bool shell_membership_plmod(int j, FILE* out) {
    return shell_morprop_kergreen(j, R_GREEN, KER_MOD, "MOD", "syntactic morphism", out);
}

bool shell_membership_plamt(int j, FILE* out) {
    return shell_morprop_kergreen(j, R_GREEN, KER_AMT, "AMT", "syntactic morphism", out);
}

bool shell_membership_plgr(int j, FILE* out) {
    return shell_morprop_kergreen(j, R_GREEN, KER_GR, "GR", "syntactic morphism", out);
}

bool shell_membership_pldd(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_DD, "DD", "syntactic morphism", out);
}

bool shell_membership_plmodp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_MODP, "MOD⁺", "syntactic morphism", out);
}

bool shell_membership_plamtp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_AMTP, "AMT⁺", "syntactic morphism", out);
}

bool shell_membership_plgrp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_GRP, "GR⁺", "syntactic morphism", out);
}

bool shell_membership_pl2st(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_PT, "PL(ST)", "syntactic morphism", out);
}

bool shell_membership_pl2mod(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_BPMOD, "PL(MOD)", "syntactic morphism", out);
}

bool shell_membership_pl2amt(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_BPAMT, "PL(AMT)", "syntactic morphism", out);
}

bool shell_membership_pl2gr(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_BPGR, "PL(GR)", "syntactic morphism", out);
}

bool shell_membership_pl2dd(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_BPDD, "PL(DD)", "syntactic morphism", out);
}

bool shell_membership_pl2modp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_BPMODP, "PL(MOD⁺)", "syntactic morphism", out);
}

bool shell_membership_pl2amtp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_BPAMTP, "PL(AMT⁺)", "syntactic morphism", out);
}

bool shell_membership_pl2grp(int j, FILE* out) {
    return shell_morprop_orbgreen(j, R_GREEN, ORB_BPGRP, "PL(GR⁺)", "syntactic morphism", out);
}

/***********************/
/* Procédures globales */
/***********************/

bool shell_membership(com_parameters* pars) {
    int n = com_nbparams(pars);
    if (n != 2) {
        shell_error_nbparams(keywordtostring(KY_MEMB), 2);
        return false;
    }

    print_title_box(0, true, stdout, 1, "Membership");
    // La classe
    classes cl = command_to_class(com_getparam(pars, 0));

    if (cl == CL_END) {
        fprintf(stdout, "#### The class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, " is either unknown or unsupported.\n\n");
        return false;
    }

    if (class_membership[cl] == NULL) {
        fprintf(stdout, "#### Membership is unsupported for the class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, ".\n\n");
        return false;
    }

    fprintf(stdout, "#### ");
    print_command(com_getparam(pars, 0), stdout);
    fprintf(stdout, " is equal to the class %s.\n", class_names[cl]);
    print_class_info(cl, stdout);

    // Récupération de l'argument.
    bool saved;
    int i = com_apply_command(com_getparam(pars, 1), NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return false;
    }
    print_info_input(i, stdout);

    // Caclcul du morphisme syntactique
    int j = shell_compute_syntac(i);
    if (j == -1) {
        return false;
    }

    print_title_box(10, true, stdout, 1, "The syntactic morphism.");
    shell_view_object(objects[j], false);
    print_conclusion_comp(stdout, class_membership[cl](j, stdout), class_names[cl]);
    if (saved) {
        object_free(i);
    }
    return false;
}

/********************************/
/* Summary for all main classes */
/********************************/

static void summary_answer(bool res) {
    if (res) {
        fprintf(stdout, "║" ANSI_COLOR_GREEN "     YES      " ANSI_COLOR_RESET);

    }
    else {
        fprintf(stdout, "║" ANSI_COLOR_RED "     NO       " ANSI_COLOR_RESET);
    }
}

bool shell_chiera_summary(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_CHIERA), 1);
        return false;
    }
    // Récupération de l'input et de son morphisme syntactique.
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);

    if (i == -1) {
        return false;
    }
    int j = shell_compute_syntac(i);
    if (j == -1) {
        return false;
    }
    /***************************/
    /* Calculs pour la base ST */
    /***************************/

    bool res_st = false;
    bool res_sfst = false;
    bool res_polst = false;
    bool res_bpolst = false;
    bool res_tlst = false;
    bool res_pol2st = false;
    bool res_bpol2st = false;
    bool res_upolbpol2st = false;
    bool res_tl2st = false;

    res_sfst = shell_membership_sf(j, NULL); // Apériodicité
    if (res_sfst) {
        res_st = shell_membership_st(j, NULL);                  // ST
        res_polst = shell_membership_ppt(j, NULL);              // Pol(ST)
        res_bpolst = res_polst || shell_membership_pt(j, NULL); // BPol(ST)

        res_tlst = res_bpolst || shell_membership_ul(j, NULL);     // TL(ST)
        res_pol2st = res_tlst || shell_membership_pol2st(j, NULL); // Pol2(ST)

        res_upolbpol2st = res_pol2st || shell_membership_ubpol2st(j, NULL);
        res_bpol2st = res_pol2st || (res_upolbpol2st && shell_membership_knastat(j, NULL));
        res_tl2st = res_upolbpol2st || shell_membership_tl2st(j, NULL);
    }

    fprintf(stdout, "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(stdout, "║                                          Concatenation hierarchies: membership tests summary                                         ║\n");
    fprintf(stdout, "╠══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╣\n");
    fprintf(stdout, "║  Basis: ST   ║    Pol(ST)   ║   BPol(ST)   ║    TL(ST)    ║   Pol₂(ST)   ║   BPol₂(ST)  ║  UBPol₂(ST)  ║   TL₂(ST)    ║    SF(ST)    ║\n");
    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(stdout, "║             ");
    summary_answer(res_st);
    summary_answer(res_polst);
    summary_answer(res_bpolst);
    summary_answer(res_tlst);

    summary_answer(res_pol2st);

    summary_answer(res_bpol2st);

    summary_answer(res_upolbpol2st);

    summary_answer(res_tl2st);

    summary_answer(res_sfst);
    fprintf(stdout, "║\n");

    /***************************/
    /* Calculs pour la base DD */
    /***************************/

    // On initialise les paramètres avec ceux de la base ST
    bool res_dd = res_st;
    bool res_sfdd = res_sfst;
    bool res_poldd = res_polst;
    bool res_bpoldd = res_bpolst;
    bool res_tldd = res_tlst;
    bool res_pol2dd = res_pol2st;
    bool res_upolbpol2dd = res_upolbpol2st;
    bool res_tl2dd = res_tl2st;

    // Les résultats ne seront différent de ceux pour la base ST que si le langage est dans SF et que le neutre n'a que le mot vide pour antécédent
    if (res_sfdd && !mor_nonempty_neutral(objects[j]->mor->obj)) {
        res_dd = res_st || shell_membership_dd(j, NULL);
        res_poldd = res_polst || shell_membership_poldd(j, NULL);
        res_bpoldd = res_bpolst || res_poldd || shell_membership_bpoldd(j, NULL);
        res_tldd = res_tlst || res_bpoldd || shell_membership_tldd(j, NULL);
        res_pol2dd = res_pol2st || res_tldd || shell_membership_pol2dd(j, NULL);
        res_upolbpol2dd = res_upolbpol2st || res_pol2dd || shell_membership_ubpol2dd(j, NULL);
        res_tl2dd = res_tl2st || res_upolbpol2dd || shell_membership_tl2dd(j, NULL);
    }

    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(stdout, "║  Basis: DD   ║    Pol(DD)   ║   BPol(DD)   ║    TL(DD)    ║   Pol₂(DD)   ║   BPol₂(DD)  ║  UBPol₂(DD)  ║   TL₂(DD)    ║    SF(DD)    ║\n");
    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(stdout, "║             ");
    summary_answer(res_dd);
    summary_answer(res_poldd);
    summary_answer(res_bpoldd);
    summary_answer(res_tldd);

    summary_answer(res_pol2dd);

    if (res_pol2dd || res_bpol2st) {
        summary_answer(res_pol2dd || res_bpol2st);

    }
    else if (!res_upolbpol2dd) {
        summary_answer(res_upolbpol2dd);

    }
    else if (mor_nonempty_neutral(objects[j]->mor->obj)) {
        summary_answer(res_bpol2st);
    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_upolbpol2dd);

    summary_answer(res_tl2dd);

    summary_answer(res_sfdd);
    fprintf(stdout, "║\n");

    /***************************/
    /* Calculs pour la base GR */
    /***************************/

    // On initialise les paramètres avec ceux de la base ST
    bool res_gr = res_st;
    bool res_sfgr = res_sfst;
    bool res_polgr = res_polst;
    bool res_bpolgr = res_bpolst;
    bool res_tlgr = res_tlst;
    bool res_pol2gr = res_pol2st;
    bool res_upolbpol2gr = res_upolbpol2st;
    bool res_tl2gr = res_tl2st;

    res_sfgr = res_sfst || shell_membership_sfgr(j, NULL); // Apériodicité
    if (res_sfgr) {
        res_gr = res_st || shell_membership_gr(j, NULL);
        res_polgr = shell_membership_polgr(j, NULL); // Pol(GR)
        res_bpolgr = res_polgr || shell_membership_bpolgr(j, NULL);
        res_tlgr = res_bpolgr || shell_membership_tlgr(j, NULL);
        res_pol2gr = res_tlgr || shell_membership_pol2gr(j, NULL);
        res_upolbpol2gr = res_pol2gr || shell_membership_ubpol2gr(j, NULL);
        res_tl2gr = res_upolbpol2gr || shell_membership_tl2gr(j, NULL);
    }

    /****************************/
    /* Calculs pour la base MOD */
    /****************************/

    // On initialise les paramètres avec ceux de la base ST
    bool res_mod = res_st;
    bool res_polmod = res_polst;
    bool res_bpolmod = res_bpolst;
    bool res_tlmod = res_tlst;
    bool res_pol2mod = res_pol2st;
    bool res_upolbpol2mod = res_upolbpol2st;
    bool res_tl2mod = res_tl2st;
    bool res_sfmod = res_sfst;

    if (!res_sfst && res_sfgr && !mor_neutral_letter(objects[j]->mor->obj, NULL)) {
        res_sfmod = shell_membership_sfmod(j, NULL);
    }

    if (res_sfmod) {
        res_mod = res_st || shell_membership_mod(j, NULL);
        res_polmod = res_polgr && (res_polst || shell_membership_polmod(j, NULL));
        res_tlmod = res_tlgr && (res_tlst || res_polmod || shell_membership_tlmod(j, NULL));
        res_bpolmod = res_bpolgr && res_tlmod && (res_polmod || res_bpolst || shell_membership_bpolmod(j, NULL));
        res_pol2mod = res_pol2gr && (res_tlmod || res_pol2st || shell_membership_pol2mod(j, NULL));
        res_upolbpol2mod = res_upolbpol2gr && (res_pol2mod || res_upolbpol2st || shell_membership_ubpol2mod(j, NULL));
        res_tl2mod = res_tl2gr && (res_upolbpol2mod || res_tl2st || shell_membership_tl2mod(j, NULL));
    }

    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(stdout, "║  Basis: MOD  ║   Pol(MOD)   ║  BPol(MOD)   ║   TL(MOD)    ║  Pol₂(MOD)   ║  BPol₂(MOD)  ║ UBPol₂(MOD)  ║  TL₂(MOD)    ║   SF(MOD)    ║\n");
    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(stdout, "║             ");
    summary_answer(res_mod);
    summary_answer(res_polmod);
    summary_answer(res_bpolmod);
    summary_answer(res_tlmod);

    summary_answer(res_pol2mod);

    if (res_pol2mod || res_bpol2st) {
        summary_answer(res_pol2mod || res_bpol2st);

    }
    else if (!res_upolbpol2mod) {
        summary_answer(res_upolbpol2mod);

    }
    else if (mor_neutral_letter(objects[j]->mor->obj, NULL)) {
        summary_answer(res_bpol2st);
    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_upolbpol2mod);

    summary_answer(res_tl2mod);

    summary_answer(res_sfmod);
    fprintf(stdout, "║\n");

    // Calculs pour la base MODP

    bool res_modp = res_mod;
    bool res_polmodp = res_polmod;
    bool res_bpolmodp = res_bpolmod;
    bool res_tlmodp = res_tlmod;
    bool res_pol2modp = res_pol2mod;
    bool res_upolbpol2modp = res_upolbpol2mod;
    bool res_tl2modp = res_tl2mod;
    bool res_sfmodp = res_sfmod;

    if (res_sfmod && !mor_neutral_letter(objects[j]->mor->obj, NULL)) {
        res_modp = res_mod || shell_membership_modp(j, NULL);
        res_polmodp = res_polmod || shell_membership_polmodp(j, NULL);
        res_bpolmodp = res_bpolmod || res_polmodp || shell_membership_bpolmodp(j, NULL);
        res_tlmodp = res_tlmod || res_bpolmodp || shell_membership_tlmodp(j, NULL);
        res_pol2modp = res_pol2mod || res_tlmodp || shell_membership_pol2modp(j, NULL);
        res_upolbpol2modp = res_upolbpol2mod || res_pol2modp || shell_membership_ubpol2modp(j, NULL);
        res_tl2modp = res_tl2mod || res_upolbpol2modp || shell_membership_tl2modp(j, NULL);
    }

    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(stdout, "║  Basis: MOD⁺ ║   Pol(MOD⁺)  ║  BPol(MOD⁺)  ║   TL(MOD⁺)   ║  Pol₂(MOD⁺)  ║  BPol₂(MOD⁺) ║ UBPol₂(MOD⁺) ║  TL₂(MOD⁺)   ║   SF(MOD⁺)   ║\n");
    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(stdout, "║             ");
    summary_answer(res_modp);
    summary_answer(res_polmodp);
    summary_answer(res_bpolmodp);
    summary_answer(res_tlmodp);

    summary_answer(res_pol2modp);

    if (res_pol2modp || res_bpol2st) {
        summary_answer(res_pol2modp || res_bpol2st);

    }
    else if (!res_upolbpol2modp) {
        summary_answer(res_upolbpol2modp);

    }
    else if (mor_neutral_letter(objects[j]->mor->obj, NULL)) {
        summary_answer(res_bpol2st);
    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_upolbpol2modp);

    summary_answer(res_tl2modp);

    summary_answer(res_sfmodp);
    fprintf(stdout, "║\n");



    /****************************/
    /* Calculs pour la base AMT */
    /****************************/

    // On initialise les paramètres avec ceux de la base MOD
    bool res_amt = res_mod;
    bool res_bpolamt = res_bpolmod;
    bool res_tlamt = res_tlmod;
    bool res_pol2amt = res_pol2mod;
    bool res_upolbpol2amt = res_upolbpol2mod;
    bool res_tl2amt = res_tl2mod;
    bool res_sfamt = res_sfmod;

    if (!res_sfamt && res_sfgr) {
        res_sfamt = shell_membership_sfamt(j, NULL);
    }


    if (res_sfamt) {

        res_amt = res_amt || shell_membership_amt(j, NULL);
        res_bpolamt = res_bpolgr && (res_amt || res_bpolamt || shell_membership_bpolamt(j, NULL));
        res_tlamt = res_tlgr && (res_tlamt || shell_membership_tlamt(j, NULL));
        res_pol2amt = res_pol2gr && (res_tlamt || res_pol2amt || shell_membership_pol2amt(j, NULL));
        res_upolbpol2amt = res_upolbpol2gr && (res_pol2amt || res_upolbpol2amt || shell_membership_ubpol2amt(j, NULL));
        res_tl2amt = res_tl2gr && (res_upolbpol2amt || res_tl2amt || shell_membership_tl2amt(j, NULL));
    }

    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(stdout, "║  Basis: AMT  ║   Pol(AMT)   ║  BPol(AMT)   ║   TL(AMT)    ║  Pol₂(AMT)   ║  BPol₂(AMT)  ║ UBPol₂(AMT)  ║  TL₂(AMT)    ║   SF(AMT)    ║\n");
    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(stdout, "║             ");
    summary_answer(res_amt);

    if (res_amt || res_polmod) {
        summary_answer(true);

    }
    else if (!res_bpolamt || !res_polgr) {
        summary_answer(false);

    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_bpolamt);

    summary_answer(res_tlamt);

    summary_answer(res_pol2amt);

    if (res_pol2amt || res_bpol2st) {
        summary_answer(res_pol2amt || res_bpol2st);

    }
    else if (!res_upolbpol2amt) {
        summary_answer(res_upolbpol2amt);

    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_upolbpol2amt);

    summary_answer(res_tl2amt);

    summary_answer(res_sfamt);
    fprintf(stdout, "║\n");

    /***************************/
    /* Calculs pour la base GRP */
    /***************************/

    // On initialise les paramètres avec ceux de la base GR
    bool res_grp = res_gr;
    bool res_sfgrp = res_sfgr;
    bool res_polgrp = res_polgr;
    bool res_bpolgrp = res_bpolgr;
    bool res_tlgrp = res_tlgr;
    bool res_pol2grp = res_pol2gr;
    bool res_upolbpol2grp = res_upolbpol2gr;
    bool res_tl2grp = res_tl2gr;

    if (res_sfgrp) {
        res_grp = res_gr || shell_membership_grp(j, NULL);
        res_polgrp = res_polgr || res_polmodp || shell_membership_polgrp(j, NULL);
        res_tlgrp = res_tlgr || res_tlmodp || shell_membership_tlgrp(j, NULL);
        res_bpolgrp = res_tlgrp && (res_polgrp || res_bpolgr || res_bpolmodp || shell_membership_bpolgr(j, NULL));
        res_pol2grp = res_tlgrp || res_pol2gr || res_pol2modp || shell_membership_pol2grp(j, NULL);
        res_upolbpol2grp = res_pol2grp || res_upolbpol2gr || res_upolbpol2modp || shell_membership_ubpol2grp(j, NULL);
        res_tl2grp = res_upolbpol2grp || res_tl2gr || res_tl2modp || shell_membership_tl2grp(j, NULL);
    }

    // Calculs pour la base AMTP

    bool res_amtp = res_amt;
    // bool res_polamtp = res_polamt;
    bool res_bpolamtp = res_bpolamt;
    bool res_tlamtp = res_tlamt;
    bool res_pol2amtp = res_pol2amt;
    bool res_upolbpol2amtp = res_upolbpol2amt;
    bool res_tl2amtp = res_tl2amt;
    bool res_sfamtp = res_sfamt;

    if (res_sfamt) {
        res_amtp = res_amt || shell_membership_amtp(j, NULL);
        //res_polamtp = res_polamt || shell_membership_polamtp(j, NULL);
        res_bpolamtp = res_bpolamt || res_amtp || shell_membership_bpolamtp(j, NULL);
        res_tlamtp = res_tlamtp || res_tlmodp || shell_membership_tlamtp(j, NULL);
        res_pol2amtp = res_pol2amtp || res_tlamtp || res_pol2modp || shell_membership_pol2amtp(j, NULL);
        res_upolbpol2amtp = res_upolbpol2amtp || res_pol2amtp || res_upolbpol2modp || shell_membership_ubpol2amtp(j, NULL);
        res_tl2amtp = res_tl2amtp || res_tl2modp || res_upolbpol2amtp || shell_membership_tl2amtp(j, NULL);
    }

    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(stdout, "║  Basis: AMT⁺ ║   Pol(AMT⁺)  ║  BPol(AMT⁺)  ║   TL(AMT⁺)   ║  Pol₂(AMT⁺)  ║  BPol₂(AMT⁺) ║ UBPol₂(AMT⁺) ║  TL₂(AMT⁺)   ║   SF(AMT⁺)   ║\n");
    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(stdout, "║             ");
    summary_answer(res_amtp);

    if (res_amtp || res_polmodp) {
        summary_answer(true);

    }
    else if (!res_bpolamtp || !res_polgrp) {
        summary_answer(false);

    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_bpolamtp);

    summary_answer(res_tlamtp);

    summary_answer(res_pol2amtp);

    if (res_pol2amtp || res_bpol2st) {
        summary_answer(res_pol2amtp || res_bpol2st);

    }
    else if (!res_upolbpol2amtp) {
        summary_answer(res_upolbpol2amtp);

    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_upolbpol2amtp);

    summary_answer(res_tl2amtp);

    summary_answer(res_sfamtp);
    fprintf(stdout, "║\n");






    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(stdout, "║  Basis: GR   ║    Pol(GR)   ║   BPol(GR)   ║    TL(GR)    ║   Pol₂(GR)   ║   BPol₂(GR)  ║  UBPol₂(GR)  ║   TL₂(GR)    ║    SF(GR)    ║\n");
    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(stdout, "║             ");
    summary_answer(res_gr);
    summary_answer(res_polgr);
    summary_answer(res_bpolgr);
    summary_answer(res_tlgr);

    summary_answer(res_pol2gr);

    if (res_pol2gr || res_bpol2st) {
        summary_answer(res_pol2gr || res_bpol2st);

    }
    else if (!res_upolbpol2gr) {
        summary_answer(res_upolbpol2gr);

    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_upolbpol2gr);

    summary_answer(res_tl2gr);

    summary_answer(res_sfgr);
    fprintf(stdout, "║\n");



    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    fprintf(stdout, "║  Basis: GR⁺  ║    Pol(GR⁺)  ║   BPol(GR⁺)  ║    TL(GR⁺)   ║   Pol₂(GR⁺)  ║  BPol₂(GR⁺)  ║  UBPol₂(GR⁺) ║   TL₂(GR⁺)   ║    SF(GR⁺)   ║\n");
    fprintf(stdout, "╠══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
    // fprintf(stdout, "║             ");
    summary_answer(res_grp);
    summary_answer(res_polgrp);
    summary_answer(res_bpolgrp);
    summary_answer(res_tlgrp);
    summary_answer(res_pol2grp);

    if (res_pol2grp || res_bpol2st) {
        summary_answer(true);

    }
    else if (!res_upolbpol2grp) {
        summary_answer(false);

    }
    else {
        fprintf(stdout, "║  Unavailable ");
    }

    summary_answer(res_upolbpol2grp);

    summary_answer(res_tl2grp);

    summary_answer(res_sfgrp);
    fprintf(stdout, "║\n");

    fprintf(stdout, "╚══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╝\n");

    if (saved) {
        object_free(i);
    }

    return 1;
}

static void make_subscript_index(uint n, char* message) {
    if (n != 0) {
        uint d = n % 10;
        make_subscript_index(n / 10, message);
        switch (d) {
        case 0:
            strcat(message, "₀");
            break;
        case 1:
            strcat(message, "₁");
            break;
        case 2:
            strcat(message, "₂");
            break;
        case 3:
            strcat(message, "₃");
            break;
        case 4:
            strcat(message, "₄");
            break;
        case 5:
            strcat(message, "₅");
            break;
        case 6:
            strcat(message, "₆");
            break;
        case 7:
            strcat(message, "₇");
            break;
        case 8:
            strcat(message, "₈");
            break;
        case 9:
            strcat(message, "₉");
            break;
        default:
            break;
        }
    }
}

static void make_superscript_index(uint n, char* message) {
    if (n != 0) {
        uint d = n % 10;
        make_subscript_index(n / 10, message);
        switch (d) {
        case 0:
            strcat(message, "⁰");
            break;
        case 1:
            strcat(message, "¹");
            break;
        case 2:
            strcat(message, "²");
            break;
        case 3:
            strcat(message, "³");
            break;
        case 4:
            strcat(message, "⁴");
            break;
        case 5:
            strcat(message, "⁵");
            break;
        case 6:
            strcat(message, "⁶");
            break;
        case 7:
            strcat(message, "⁷");
            break;
        case 8:
            strcat(message, "⁸");
            break;
        case 9:
            strcat(message, "⁹");
            break;
        default:
            break;
        }
    }
}

bool shell_neghiera(com_parameters* pars) {
    // Il doit y avoir deux arguments:
    // - la classe de base
    // - la variable qui donne le langage ou le morphisme input
    if (com_nbparams(pars) != 2) {
        shell_error_nbparams(keywordtostring(KY_NHIERA), 2);
        return false;
    }
    // Récupération de l'input et de son morphisme syntactique.
    bool saved;
    int i = com_apply_command(pars->next->param, NULL, MODE_DEFAULT, &saved);

    if (i == -1) {
        return false;
    }
    int j = shell_compute_syntac(i);
    if (j == -1) {
        return false;
    }

    // On prend le nom de la classe
    com_keyword key = key_from_string_chain_single(pars->param->main);

    // short level;
    bool inunion;
    switch (key) {
    case KY_ST:
        print_infooper_neghiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL");
        inunion = shell_membership_ul(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ.\n");
            fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(ST) and MPol.\n\n");
            ufind* C = parti_to_ufind(objects[j]->mor->obj->rels->JCL);
            uint level = hdet_mpol_level(objects[j]->mor->obj, C, stdout);
            delete_ufind(C);
            char subscript[10];
            subscript[0] = '\0';
            make_subscript_index(level, subscript);
            char message[150];
            sprintf(message, "Conclusion: the least level containing the input is TL%s.", subscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;
    case KY_DD:
        print_infooper_neghiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL(DD)");
        inunion = shell_membership_tldd(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL(DD)");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ(DD).\n");
            fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(DD) and MPol.\n\n");
            ufind* C = iden_knast_mono(shell_compute_orbits(j, ORB_DD, LV_REG));
            uint level = hdet_mpol_level(objects[j]->mor->obj, C, stdout);
            delete_ufind(C);
            char subscript[10];
            subscript[0] = '\0';
            make_subscript_index(level, subscript);
            char message[150];
            sprintf(message, "Conclusion: the least level containing the input is TL%s(DD).", subscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;
    case KY_MOD:
        print_infooper_neghiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL(MOD)");
        inunion = shell_membership_tlmod(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL(MOD)");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ(MOD).\n");
            fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(MOD) and MPol.\n\n");
            ufind* C = iden_bpolmod_mono(objects[j]->mor->obj);
            uint level = hdet_mpol_level(objects[j]->mor->obj, C, stdout);
            delete_ufind(C);
            char subscript[10];
            subscript[0] = '\0';
            make_subscript_index(level, subscript);
            char message[150];
            sprintf(message, "Conclusion: the least level containing the input is TL%s(MOD).", subscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;
    case KY_MODP:
        print_infooper_neghiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL(MOD⁺)");
        inunion = shell_membership_tlmodp(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL(MOD⁺)");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ(MOD⁺).\n");
            fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(MOD⁺) and MPol.\n\n");
            ufind* C = iden_qknast_mono(shell_compute_orbits(j, ORB_MODP, LV_REG), shell_compute_ker(j, KER_MOD, LV_REG));
            uint level = hdet_mpol_level(objects[j]->mor->obj, C, stdout);
            delete_ufind(C);
            char subscript[10];
            subscript[0] = '\0';
            make_subscript_index(level, subscript);
            char message[150];
            sprintf(message, "Conclusion: the least level containing the input is TL%s(MOD⁺).", subscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;
    case KY_GR:
        print_infooper_neghiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL(GR)");
        fprintf(stdout, "#### Checking if the GR-kernel belongs to DA.\n");
        inunion = shell_membership_tlgr(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL(GR)");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ(GR).\n");
            fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(GR) and MPol.\n\n");
            ufind* C = iden_blockg_mono(objects[j]->mor->obj);
            uint level = hdet_mpol_level(objects[j]->mor->obj, C, stdout);
            delete_ufind(C);
            char subscript[10];
            subscript[0] = '\0';
            make_subscript_index(level, subscript);
            char message[150];
            sprintf(message, "Conclusion: the least level containing the input is TL%s(GR).", subscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;
    default:
        if (saved) {
            object_free(i);
        }
        return false;
        break;
    }
    if (saved) {
        object_free(i);
    }
    return true;
}

bool shell_fphiera(com_parameters* pars) {
    // Il doit y avoir deux arguments:
    // - la classe de base
    // - la variable qui donne le langage ou le morphisme input
    if (com_nbparams(pars) != 2) {
        shell_error_nbparams(keywordtostring(KY_FPHIERA), 2);
        return false;
    }
    // Récupération de l'input et de son morphisme syntactique.
    bool saved;
    int i = com_apply_command(pars->next->param, NULL, MODE_DEFAULT, &saved);

    if (i == -1) {
        return false;
    }
    int j = shell_compute_syntac(i);
    if (j == -1) {
        return false;
    }

    // On prend le nom de la classe
    com_keyword key = key_from_string_chain_single(pars->param->main);

    // short level;
    short minf;
    short minp;
    bool inunion;
    switch (key) {
    case KY_ST:
        print_infooper_fphiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL");
        inunion = shell_membership_ul(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least level FLⁿ and PLⁿ containing the input.\n");
            fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
            ufind* C = parti_to_ufind(objects[j]->mor->obj->rels->JCL);
            hdet_lrpol_level(objects[j]->mor->obj, C, stdout, &minf, &minp);
            delete_ufind(C);
            char fscript[10];
            char pscript[10];
            fscript[0] = '\0';
            pscript[0] = '\0';
            make_superscript_index(minf, fscript);
            make_superscript_index(minp, pscript);
            char message[150];
            sprintf(message, "Conclusion: the least levels containing the input are FL%s and PL%s", fscript, pscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;

    case KY_DD:
        print_infooper_fphiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL(DD)");
        inunion = shell_membership_tldd(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL(DD)");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least level FLⁿ(DD) and PLⁿ(DD) containing the input.\n");
            fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
            ufind* C = iden_knast_mono(shell_compute_orbits(j, ORB_DD, LV_REG));
            hdet_lrpol_level(objects[j]->mor->obj, C, stdout, &minf, &minp);
            delete_ufind(C);
            char fscript[10];
            char pscript[10];
            fscript[0] = '\0';
            pscript[0] = '\0';
            make_superscript_index(minf, fscript);
            make_superscript_index(minp, pscript);
            char message[150];
            sprintf(message, "Conclusion: the least levels containing the input are FL%s(DD) and PL%s(DD)", fscript, pscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }

        break;
    case KY_MOD:
        print_infooper_fphiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL(MOD)");
        inunion = shell_membership_tlmod(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL(MOD)");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least level FLⁿ(MOD) and PLⁿ(MOD) containing the input.\n");
            fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
            ufind* C = iden_bpolmod_mono(objects[j]->mor->obj);
            hdet_lrpol_level(objects[j]->mor->obj, C, stdout, &minf, &minp);
            delete_ufind(C);
            char fscript[10];
            char pscript[10];
            fscript[0] = '\0';
            pscript[0] = '\0';
            make_superscript_index(minf, fscript);
            make_superscript_index(minp, pscript);
            char message[150];
            sprintf(message, "Conclusion: the least levels containing the input are FL%s(MOD) and PL%s(MOD)", fscript, pscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;
    case KY_MODP:
        print_infooper_fphiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL(MOD⁺)");
        inunion = shell_membership_tlmodp(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL(MOD⁺)");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least level FLⁿ(MOD⁺) and PLⁿ(MOD⁺) containing the input.\n");
            fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
            ufind* C = iden_qknast_mono(shell_compute_orbits(j, ORB_MODP, LV_REG), shell_compute_ker(j, KER_MOD, LV_REG));
            hdet_lrpol_level(objects[j]->mor->obj, C, stdout, &minf, &minp);
            delete_ufind(C);
            char fscript[10];
            char pscript[10];
            fscript[0] = '\0';
            pscript[0] = '\0';
            make_superscript_index(minf, fscript);
            make_superscript_index(minp, pscript);
            char message[150];
            sprintf(message, "Conclusion: the least levels containing the input are FL%s(MOD⁺) and PL%s(MOD⁺)", fscript, pscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;
    case KY_GR:
        print_infooper_fphiera(key, stdout);
        print_info_input(i, stdout);
        print_start_comp(stdout, "TL(GR)");
        fprintf(stdout, "#### Checking if the GR-kernel belongs to DA.\n");
        inunion = shell_membership_tlgr(j, stdout);
        print_conclusion_comp(stdout, inunion, "TL(GR)");
        if (inunion) {
            fprintf(stdout, "#### We can now determine the least level FLⁿ(GR) and PLⁿ(GR) containing the input.\n");
            fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
            ufind* C = iden_blockg_mono(objects[j]->mor->obj);
            hdet_lrpol_level(objects[j]->mor->obj, C, stdout, &minf, &minp);
            delete_ufind(C);
            char fscript[10];
            char pscript[10];
            fscript[0] = '\0';
            pscript[0] = '\0';
            make_superscript_index(minf, fscript);
            make_superscript_index(minp, pscript);
            char message[150];
            sprintf(message, "Conclusion: the least levels containing the input are FL%s(GR) and PL%s(GR)", fscript, pscript);
            print_dtitle_box(100, true, stdout, 1, message);
        }
        break;
    default:
        return false;
        break;
    }

    return true;
}

bool shell_exsearch(com_parameters* pars) {
    int n = com_nbparams(pars);
    if (n < 6) {
        shell_error_leastparams(keywordtostring(KY_EXSEARCH), 6);
        return false;
    }

    // Les classes
    classes low[n - 5];
    for (int i = 0; i < n - 5; i++) {
        low[i] = command_to_class(pars->param);
        if (low[i] == CL_END) {
            fprintf(stdout, "#### The class ");
            print_command(pars->param, stdout);
            fprintf(stdout, " is either unknown or unsupported.\n\n");
            return false;
        }
        if (class_membership[low[i]] == NULL) {
            fprintf(stdout, "#### Membership is unsupported for the class ");
            print_command(pars->param, stdout);
            fprintf(stdout, ".\n\n");
            return false;
        }
        // printf("low: %s\n", class_names[low[i]]);
        pars = pars->next;
    }

    classes cl = command_to_class(com_getparam(pars, 0));

    if (cl == CL_END) {
        fprintf(stdout, "#### The class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, " is either unknown or unsupported.\n\n");
        return false;
    }

    if (class_membership[cl] == NULL) {
        fprintf(stdout, "#### Membership is unsupported for the class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, ".\n\n");
        return false;
    }

    if (!com_single(com_getparam(pars, 1))) {
        shell_error_numpar(keywordtostring(KY_EXSEARCH), 1);
        return false;
    }
    char* end;
    int cycle = strtol(com_getparam(pars, 1)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_EXSEARCH), n - 4);
        return false;
    }

    int count = 0;
    char buffer[64];

    for (int i = 0; i < cycle; i++) {
        sprintf(buffer, "EXA%04d", count);
        int j = shell_random_dfa(buffer, pars->next->next);
        int k = shell_compute_syntac(j);
        if (k == -1) {
            fprintf(stdout, "#### Test %d: Syntactic monoid too large.\n", i + 1);
            continue;
        }
        bool not = false;
        for (int h = 0; h < n - 5; h++) {

            not = not || class_membership[low[h]](k, NULL);
        }
        // view_nfa(objects[j]->nfa);
        if (j != -1 && !not&& class_membership[cl](k, NULL)) {
            fprintf(stdout, "#### Test %d: Found an example. Stored in variable %s.\n", i + 1, buffer);
            count++;
        }
        else if (j != -1) {
            fprintf(stdout, "#### Test %d: Not an example.\n", i + 1);
            object_free(j);
        }
    }

    return false;
}

/*
static bool shell_exall_aux(dgraph* arrow) {

    for (uint a = 0; a < arrow->size_alpha - 1;a++) {
        if (arrow->edges[0][a] == arrow->edges[0][a + 1]) {
            arrow->edges[0][a] = 0;
        }
        else {
            arrow->edges[0][a]++;
            return true;
        }
    }
    if (arrow->edges[0][arrow->size_alpha - 1] == arrow->size_graph) {
        arrow->edges[0][arrow->size_alpha - 1] = 0;
    }
    else {
        arrow->edges[0][arrow->size_alpha - 1]++;
        return true;
    }


    for (uint q = 1; q < arrow->size_graph;q++) {
        for (uint a = 0; a < arrow->size_alpha;a++) {
            if (arrow->edges[q][a] == arrow->size_graph) {
                arrow->edges[q][a] = 0;
            }
            else {
                arrow->edges[q][a]++;
                return true;
            }
        }
    }
    return false;
} */

static int nfa_compare_minimal(void* N1, void* N2) {
    nfa* A1 = (nfa*)N1;
    nfa* A2 = (nfa*)N2;
    int sizeg = A1->trans->size_graph - A2->trans->size_graph;
    if (sizeg != 0) {
        return sizeg;
    }
    int sizea = A1->trans->size_alpha - A2->trans->size_alpha;
    if (sizea != 0) {
        return sizea;
    }

    for (uint q = 0; q < A1->trans->size_graph; q++) {
        for (uint a = 0; a < A1->trans->size_alpha; a++) {
            int difft = lefread_dequeue(A1->trans->edges[q][a], 0) - lefread_dequeue(A2->trans->edges[q][a], 0);
            if (difft != 0) {
                return difft;
            }
        }
    }

    int sizef = size_dequeue(A1->finals) - size_dequeue(A2->finals);
    if (sizef != 0) {
        return sizef;
    }

    for (uint i = 0; i < size_dequeue(A1->finals); i++) {
        int diff = lefread_dequeue(A1->finals, i) - lefread_dequeue(A2->finals, i);
        if (diff != 0) {
            return diff;
        }
    }

    return 0;
}

static void avl_to_objarray(avlnode* set, uint* num) {
    if (!set) {
        return;
    }
    avl_to_objarray(set->left, num);
    char buffer[64];
    sprintf(buffer, "EXA%04d", *num);
    *num += 1;
    object_add_automaton(buffer, set->value);
    avl_to_objarray(set->right, num);
    free(set);
}

static void nfa_release(void* A) { delete_nfa((nfa*)A); }

bool shell_exall(com_parameters* pars) {
    int n = com_nbparams(pars);
    if (n != 4) {
        shell_error_nbparams(keywordtostring(KY_EXALL), 4);
        return false;
    }

    // Classes to exclude
    uchar nblow;
    com_keyword lkey = key_from_string_chain_single(com_getparam(pars, 0)->main);
    if (lkey == KY_OUTSIDE) {
        nblow = com_nbparams(com_getparam(pars, 0)->params);
    }
    else {
        nblow = 1;
    }

    if (nblow == 0) {
        fprintf(stderr, "#### Error: At least one forbidden class of languages is required.\n\n");
        return false;
    }

    classes low[nblow];

    if (lkey != KY_OUTSIDE) {
        low[0] = command_to_class(com_getparam(pars, 0));
    }
    else {
        com_parameters* lpars = com_getparam(pars, 0)->params;
        for (int i = 0; i < nblow; i++) {
            low[i] = command_to_class(com_getparam(lpars, i));
        }
    }

    for (int i = 0; i < nblow; i++) {
        if (low[i] == CL_END) {
            fprintf(stdout, "#### Error: Unknow or unsupported class\n\n");
            return false;
        }
        if (class_membership[low[i]] == NULL) {
            fprintf(stdout, "#### Error: Membership is unsupported for the class %s.\n\n", class_names[low[i]]);
            return false;
        }

        // printf("low: %s\n", class_names[low[i]]);
    }

    // Classes to include
    uchar nbhigh;
    com_keyword hkey = key_from_string_chain_single(com_getparam(pars, 1)->main);
    if (hkey == KY_INSIDE) {
        nbhigh = com_nbparams(com_getparam(pars, 1)->params);
    }
    else {
        nbhigh = 1;
    }

    if (nbhigh == 0) {
        fprintf(stderr, "#### Error: At least one constraining class of languages is required.\n\n");
        return false;
    }

    classes high[nbhigh];

    if (hkey != KY_INSIDE) {
        high[0] = command_to_class(com_getparam(pars, 1));
    }
    else {
        com_parameters* hpars = com_getparam(pars, 1)->params;
        for (int i = 0; i < nbhigh; i++) {
            high[i] = command_to_class(com_getparam(hpars, i));
        }
    }

    for (int i = 0; i < nbhigh; i++) {
        if (high[i] == CL_END) {
            fprintf(stdout, "#### Error: Unknow or unsupported class\n\n");
            return false;
        }
        if (class_membership[high[i]] == NULL) {
            fprintf(stdout, "#### Error: Membership is unsupported for the class %s.\n\n", class_names[high[i]]);
            return false;
        }

        // printf("high: %s\n", class_names[high[i]]);
    }

    if (!com_single(com_getparam(pars, 2))) {
        shell_error_numpar(keywordtostring(KY_EXALL), 3);
        return false;
    }
    char* end;
    uint states = strtol(com_getparam(pars, 2)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_EXALL), 3);
        return false;
    }

    if (!com_single(com_getparam(pars, 3))) {
        shell_error_numpar(keywordtostring(KY_EXALL), 4);
        return false;
    }
    uint alpha = strtol(com_getparam(pars, 3)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_EXALL), 4);
        return false;
    }

    nfa_enum* E = nfa_enum_init(states, alpha);
    uint count = 0;
    uint exa = 0;
    avlnode* set = NULL;

    //    nfa_enum_print(E);

    while (nfa_enum_next(E)) {

        //       nfa_enum_print(E);
        nfa* A = nfa_enum_to_nfa(E);

        int j = object_add_automaton(NULL, A);


        // TODO: Fix when shell_compute_syntac fails
        int k = shell_compute_syntac(j);

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
        // view_nfa(objects[j]->nfa);

        if (j != -1 && res) {
            set = avl_insert(nfa_mini_canonical_copy(objects[shell_compute_minimal(j)]->nfa), set, nfa_compare_minimal, nfa_release);
            exa++;
        }

        if (count % 20000 == 0) {
            fprintf(stdout, "#### %d tests done so far. %d were positive\n", count, exa);
        }

        object_free(j);
    }

    nfa_enum_free(E);

    int found = getsize_avl(set);
    fprintf(stdout, "#### %d tests in total. Found %d example languages.\n", count, found);

    uint num = 0;
    avl_to_objarray(set, &num);

    return false;
}
