#include "shell_morprops.h"
#include "monoid_display.h"




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


bool shell_morprop_htgentriv(int j, char* name, FILE* out) {
    if (out) {
        fprintf(out, "#### Checking if the H-classes of 1 and all generators in the %s are trivial.\n", name);
    }
    if (is_htrivial_generators(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The H-classes of 1 and all generators in the %s are trivial.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The H-classes of %s is not trivial (for instance, it contains %s).\n", m_sprint(j, 0), m_sprint(j, 1));
        }
        return false;
    }

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
        fprintf(out, "#### Checking if the %s is letter uniform.\n", name);
    }

    if (is_letterind_mono(objects[j]->mor->obj, get_counter(out))) {
        if (out) {
            fprintf(out, "#### The %s is letter uniform.\n", name);
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
            fprintf(out, "#### For instance, %s%s ≠ %s%s.\n", m_sprint(j, 0), m_sprint(j, 1), m_buffers[1], m_buffers[0]);
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
            fprintf(out, "#### For instance, %s %s ≠ %s %s.\n", m_sprint(j, 0), m_sprint(j, 1), m_buffers[1], m_buffers[0]);
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
            fprintf(out, "#### The %s-orbit of %s is not commutative.\n", orbs, m_sprint(j, 2));
            fprintf(out, "#### For instance, %s %s ≠ %s %s.\n", m_sprint(j, 0), m_sprint(j, 1), m_buffers[1], m_buffers[0]);
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
            fprintf(out, "#### The %s-orbit of %s is not in DA.\n", orbs, m_sprint(j, 1));
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
    if (is_jsat_subsemi(shell_compute_ker(j, type, LV_GREG), ONE, get_counter(out))) {
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
            fprintf(out, "#### The equation is satisfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The equation fails for the %s-pair (q,s) = (%s,%s), r = %s and t = %s.\n", ker, m_sprint(j, 0), m_sprint(j, 2), m_sprint(j, 1), m_sprint(j, 3));
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
        fprintf(out, "#### Checking if the %s ⍺:A* → M satisfies the equation (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ\n", name);
        fprintf(out, "     for all q,r,s,t ∊ M such that {q,e,f}, {r,e,f}, {s,e,f} and {t,e,f} are AT-sets and all e,f ∊ E(M).\n");
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
        break;
    default:
        res = false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The equation is satisfied.\n");
        }
    }
    else {
        if (out) {
            fprintf(out, "#### The equation fails for the %s-pair (q,s) = (%s,%s), r = %s, t = %s, e = %s and f = %s.\n", orbs, m_sprint(j, 0), m_sprint(j, 2), m_sprint(j, 1), m_sprint(j, 3), m_sprint(j, 4), m_sprint(j, 5));
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

    if (is_upbp_mono(shell_compute_orbits(j, type, LV_GREG), get_counter(out))) {
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
