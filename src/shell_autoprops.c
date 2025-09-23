#include "shell_autoprops.h"
#include "printing.h"
#include "nfa_patterns.h"



static bool object_make_dfa(int j, int* error, FILE* out) {
    if (j < 0 || (objects[j].type != NAUTOMATON && objects[j].type != DAUTOMATON)) {
        if (out) {
            fprintf(out, "#### Error, the input is not an automaton.\n");
            *error = -1;
        }
        return false;
    }

    if (objects[j].type == DAUTOMATON) {
        return true; // Already a DFA
    }

    nfa* A = objects[j].obj_nfa;
    dfa* D = detnfa_to_dfa(A);
    if (!D) {
        if (out) {
            fprintf(out, "#### Error, the input is not a complete DFA.\n");
            *error = -1;
        }
        return false;
    }
    nfa_delete(A);
    objects[j].obj_dfa = D;
    objects[j].type = DAUTOMATON;
    return true;
}




bool shell_autoprop_trivial(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s is trivial.\n", name);
    }
    bool res = is_trivial_dfa(objects[j].obj_dfa, out);
    if (*error < 0) {
        return false;
    }

    if (res) {
        if (out) {
            fprintf(out, "#### The %s is trivial.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not trivial.\n", name);
        }
        return false;
    }

}

bool shell_autoprop_ddtrivial(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_dd();
    }
    bool res = is_ddtrivial_dfa(objects[j].obj_dfa, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_letterind(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s is letter uniform.\n", name);
    }
    bool res = is_letterind_dfa(objects[j].obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    if (res) {
        if (out) {
            fprintf(out, "#### The %s is letter uniform.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not letter uniform.\n", name);
        }
        return false;
    }

}

bool shell_autoprop_commut(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_commutative();
    }
    bool res = is_comm_dfa(objects[j].obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_alphaidem(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_idem();
    }
    bool res = is_alphaidem_dfa(objects[j].obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_ddidem(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_ddidem();
    }
    bool res = is_ddidem_dfa(objects[j].obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_ddcomm(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_ddcomm();
    }
    bool res = is_ddcomm_dfa(objects[j].obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}


bool shell_autoprop_lttcomm(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_lttcomm();
    }
    bool res = is_lttcomm_dfa(objects[j].obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}


bool shell_autoprop_permut(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s satisfies the permutation automaton pattern equation:\n", name);
        view_pattern_group();
    }
    bool res = is_permutation_dfa(objects[j].obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    if (res) {
        if (out) {
            fprintf(out, "#### The %s is a permutation automaton.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not a permutation automaton.\n", name);
        }
        return false;
    }

}

bool shell_autoprop_permutplus(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s satisfies the two following pattern equations for strict matchings:\n", name);
        view_pattern_grplus();
    }
    bool res = is_permutationplus_dfa(objects[j].obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_cfreegp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_cfreegp(mode);
    }
    bool res = is_counterfreegp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);

}




bool shell_autoprop_polgp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_polgp(mode);
    }
    bool res = is_polgp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_polgpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_polgpplus(mode);
    }
    bool res = is_polgpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_pol2gp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_pol2gp(mode);
    }
    bool res = is_pol2gp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_pol2gpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_pol2gpplus(mode);
    }
    bool res = is_pol2gpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }

    return make_pattern_conclusion_default(res, out);
}






bool shell_autoprop_rtrivialgp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_rtrivialgp(mode);
    }
    bool res = is_rtrivialgp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_rtrivialgpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_rtrivialgpplus(mode);
    }
    bool res = is_rtrivialgpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}

bool shell_autoprop_rtrivialbpgp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_rtrivialbpgp(mode);
    }
    bool res = is_rtrivialbpgp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_rtrivialbpgpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_rtrivialbpgpplus(mode);
    }
    bool res = is_rtrivialbpgpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_ltrivialgp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_ltrivialgp(mode);
    }
    bool res = is_ltrivialgp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_ltrivialgp_opti(int j, basis mode, int* error) {
    if (!object_make_dfa(j, error, NULL)) {
        return false; // Error in making DFA
    }
    dfa* temp = dfa_determinize_mirror(objects[j].obj_dfa, false);
    bool res = is_rtrivialgp_dfa(temp, mode, error, NULL);
    dfa_delete(temp);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, NULL);
}

bool shell_autoprop_ltrivialgpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_ltrivialgpplus(mode);
    }
    bool res = is_ltrivialgpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}

bool shell_autoprop_ltrivialgpplus_opti(int j, basis mode, int* error) {
    if (!object_make_dfa(j, error, NULL)) {
        return false; // Error in making DFA
    }
    dfa* temp = dfa_determinize_mirror(objects[j].obj_dfa, false);
    bool res = is_rtrivialgpplus_dfa(temp, mode, error, NULL);
    dfa_delete(temp);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, NULL);
}

bool shell_autoprop_ltrivialbpgp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_ltrivialbpgp(mode);
    }
    bool res = is_ltrivialbpgp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}


bool shell_autoprop_ltrivialbpgp_opti(int j, basis mode, int* error) {
    if (!object_make_dfa(j, error, NULL)) {
        return false; // Error in making DFA
    }
    dfa* temp = dfa_determinize_mirror(objects[j].obj_dfa, false);
    bool res = is_rtrivialbpgp_dfa(temp, mode, error, NULL);
    dfa_delete(temp);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, NULL);
}

bool shell_autoprop_ltrivialbpgpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_ltrivialbpgpplus(mode);
    }
    bool res = is_ltrivialbpgpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}


bool shell_autoprop_dagp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_dagp(mode);
    }
    bool res = is_dagp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}

bool shell_autoprop_dagpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_dagpplus(mode);
    }
    bool res = is_dagpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}


bool shell_autoprop_dabpgp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_dabpgp(mode);
    }
    bool res = is_dabpgp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}


bool shell_autoprop_dabpgpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_dabpgpplus(mode);
    }
    bool res = is_dabpgpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}



bool shell_autoprop_bpolmod(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_bpolgp(BA_MOD);
    }
    bool res = is_bpolmod_dfa(objects[j].obj_dfa, error, out);

    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}

bool shell_autoprop_bpolamt(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_bpolgp(BA_MOD);
    }
    bool res = is_bpolamt_dfa(objects[j].obj_dfa, error, out);

    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}


bool shell_autoprop_bpolamtplus(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_bpolgpplus(BA_AMT);
    }
    bool res = is_bpolamtplus_dfa(objects[j].obj_dfa, error, out);

    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}

bool shell_autoprop_bpolgrplus(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_bpolgpplus(BA_GR);
    }
    bool res = is_bpolgrplus_dfa(objects[j].obj_dfa, error, out);

    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}


bool shell_autoprop_knastgpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_knastgpplus(mode);
    }
    bool res = is_knastgpplus_dfa(objects[j].obj_dfa, mode, error, out);

    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);

}



bool shell_autoprop_piecewise(int j, int* error) {
    if (!object_make_dfa(j, error, NULL)) {
        return false; // Error in making DFA
    }
    bool res = is_piecewise_dfa(objects[j].obj_dfa, error);

    if (*error < 0) {
        return false;
    }
    return res;
}

bool shell_autoprop_knastat(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_knastat();
    }
    bool res = is_knastat_dfa(objects[j].obj_dfa, error, out);

    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}


bool shell_autoprop_upolgpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_strict(out, name);
        view_pattern_upolgpplus(mode);
    }
    bool res = is_upolgpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}


bool shell_autoprop_upolbp2gp(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_upolbp2gp(mode);
    }
    bool res = is_upolbp2gp_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}


bool shell_autoprop_upolbp2gpplus(int j, basis mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        view_pattern_intro_default(out, name);
        view_pattern_upolbp2gpplus(mode);
    }
    bool res = is_upolbp2gpplus_dfa(objects[j].obj_dfa, mode, error, out);
    if (*error < 0) {
        return false;
    }
    return make_pattern_conclusion_default(res, out);
}


bool shell_autoprop_nosimplecounter(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s has no simple counter.\n", name);
    }
    bool res = is_nosimple_counter_dfa(objects[j].obj_dfa, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s does not contain a simple counter.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s contains a simple counter.\n", name);
        }
        return false;
    }

}
