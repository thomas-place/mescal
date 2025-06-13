#include "shell_autoprops.h"
#include "printing.h"


static bool object_make_dfa(int j, int* error, FILE* out) {
    if (j < 0 || objects[j]->type != AUTOMATON) {
        if (out) {
            fprintf(out, "#### Error, the input is not an automaton.\n");
            *error = -1;
        }
        return false;
    }

    if (objects[j]->aut->dfa) {
        return true; // Already a DFA
    }

    nfa* A = objects[j]->aut->obj_nfa;
    dfa* D = detnfa_to_dfa(A);
    if (!D) {
        if (out) {
            fprintf(out, "#### Error, the input is not a complete DFA.\n");
            *error = -1;
        }
        return false;
    }
    nfa_delete(A);
    objects[j]->aut->obj_dfa = D;
    objects[j]->aut->dfa = true;
    return true;
}


bool shell_autoprop_trivial(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s is trivial.\n", name);
    }
    bool res = is_trivial_dfa(objects[j]->aut->obj_dfa, out);
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

bool shell_autoprop_letterind(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s is letter uniform.\n", name);
    }
    bool res = is_letterind_dfa(objects[j]->aut->obj_dfa, error, out);
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
        fprintf(out, "#### Checking if the %s is commutative.\n", name);
    }
    bool res = is_comm_dfa(objects[j]->aut->obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    if (res) {
        if (out) {
            fprintf(out, "#### The %s is commutative.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not commutative.\n", name);
        }
        return false;
    }

}



bool shell_autoprop_permut(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s is a permutation automaton.\n", name);
    }
    bool res = is_permutation_dfa(objects[j]->aut->obj_dfa, error, out);
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

bool shell_autoprop_cfree(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s is counter-free.\n", name);
    }
    bool res = is_counterfree_dfa(objects[j]->aut->obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }

    if (res) {
        if (out) {
            fprintf(out, "#### The %s is counter-free.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not counter-free.\n", name);
        }
        return false;
    }

}


bool shell_autoprop_dapat(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s satisfies the DA pattern equation.\n", name);
    }
    bool res = is_da_dfa(objects[j]->aut->obj_dfa, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the DA pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the DA pattern equation.\n", name);
        }
        return false;
    }

}


bool shell_autoprop_dapluspat(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s satisfies the DA(DD) pattern equation.\n", name);
    }
    bool res = is_daplus_dfa(objects[j]->aut->obj_dfa, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the DA(DD) pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the DA(DD) pattern equation.\n", name);
        }
        return false;
    }

}


bool shell_autoprop_dagppat(int j, dfagp_mode mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        switch (mode)
        {
        case DFAGP_MOD:
            fprintf(out, "#### Checking if the %s satisfies the DA(MOD) pattern equation.\n", name);
            break;
        case DFAGP_AMT:
            fprintf(out, "#### Checking if the %s satisfies the DA(AMT) pattern equation.\n", name);
            break;
        case DFAGP_GR:
            fprintf(out, "#### Checking if the %s satisfies the DA(GR) pattern equation.\n", name);
            break;
        default:
            fprintf(out, "#### Checking if the %s satisfies an DA(UNKNOWN) pattern equation.\n", name);
            break;
        }
    }
    bool res = is_dagp_dfa(objects[j]->aut->obj_dfa, mode, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the pattern equation.\n", name);
        }
        return false;
    }

}

bool shell_autoprop_daplusgppat(int j, dfagp_mode mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        switch (mode)
        {
        case DFAGP_MOD:
            fprintf(out, "#### Checking if the %s satisfies the DA(MOD⁺) pattern equation.\n", name);
            break;
        case DFAGP_AMT:
            fprintf(out, "#### Checking if the %s satisfies the DA(AMT⁺) pattern equation.\n", name);
            break;
        case DFAGP_GR:
            fprintf(out, "#### Checking if the %s satisfies the DA(GR⁺) pattern equation.\n", name);
            break;
        default:
            fprintf(out, "#### Checking if the %s satisfies an DA(UNKNOWN) pattern equation.\n", name);
            break;
        }
    }
    bool res = is_daplusgp_dfa(objects[j]->aut->obj_dfa, mode, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the pattern equation.\n", name);
        }
        return false;
    }

}


bool shell_autoprop_cyclet(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s satisfies the R pattern equation.\n", name);
    }
    bool res = is_cyclet_dfa(objects[j]->aut->obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the pattern equation.\n", name);
        }
        return false;
    }

}


bool shell_autoprop_cycletplus(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s satisfies the R(DD) pattern equation.\n", name);
    }
    bool res = is_cycletplus_dfa(objects[j]->aut->obj_dfa, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the pattern equation.\n", name);
        }
        return false;
    }

}


bool shell_autoprop_cycletgp(int j, dfagp_mode mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        switch (mode)
        {
        case DFAGP_MOD:
            fprintf(out, "#### Checking if the %s satisfies the R(MOD) pattern equation.\n", name);
            break;
        case DFAGP_AMT:
            fprintf(out, "#### Checking if the %s satisfies the R(AMT) pattern equation.\n", name);
            break;
        case DFAGP_GR:
            fprintf(out, "#### Checking if the %s satisfies the R(GR) pattern equation.\n", name);
            break;
        default:
            fprintf(out, "#### Checking if the %s satisfies an R(UNKNOWN) pattern equation.\n", name);
            break;
        }
    }
    bool res = is_cycletgp_dfa(objects[j]->aut->obj_dfa, mode, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the pattern equation.\n", name);
        }
        return false;
    }

}

bool shell_autoprop_cycletplusgp(int j, dfagp_mode mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        switch (mode)
        {
        case DFAGP_MOD:
            fprintf(out, "#### Checking if the %s satisfies the R(MOD⁺) pattern equation.\n", name);
            break;
        case DFAGP_AMT:
            fprintf(out, "#### Checking if the %s satisfies the R(AMT⁺) pattern equation.\n", name);
            break;
        case DFAGP_GR:
            fprintf(out, "#### Checking if the %s satisfies the R(GR⁺) pattern equation.\n", name);
            break;
        default:
            fprintf(out, "#### Checking if the %s satisfies an R(UNKNOWN) pattern equation.\n", name);
            break;
        }
    }
    bool res = is_cycletplusgp_dfa(objects[j]->aut->obj_dfa, mode, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the pattern equation.\n", name);
        }
        return false;
    }

}



bool shell_autoprop_cycletbp(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s satisfies the R(BPol(ST)) pattern equation.\n", name);
    }
    bool res = is_cycletbp_dfa(objects[j]->aut->obj_dfa, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the pattern equation.\n", name);
        }
        return false;
    }

}


bool shell_autoprop_cycletbpgp(int j, dfagp_mode mode, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        switch (mode)
        {
        case DFAGP_MOD:
            fprintf(out, "#### Checking if the %s satisfies the R(BPol(MOD)) pattern equation.\n", name);
            break;
        case DFAGP_AMT:
            fprintf(out, "#### Checking if the %s satisfies the R(BPol(AMT)) pattern equation.\n", name);
            break;
        case DFAGP_GR:
            fprintf(out, "#### Checking if the %s satisfies the R(BPol(GR)) pattern equation.\n", name);
            break;
        default:
            fprintf(out, "#### Checking if the %s satisfies an R(UNKNOWN) pattern equation.\n", name);
            break;
        }
    }
    bool res = is_cycletbpgp_dfa(objects[j]->aut->obj_dfa, mode, error);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s satisfies the pattern equation.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s does not satisfy the pattern equation.\n", name);
        }
        return false;
    }

}



bool shell_autoprop_nosimplecounter(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s has no simple counter.\n", name);
    }
    bool res = is_nosimple_counter(objects[j]->aut->obj_dfa, error);
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
