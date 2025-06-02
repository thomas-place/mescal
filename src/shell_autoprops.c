#include "shell_autoprops.h"
#include "nfa_props.h"
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


bool shell_autoprop_cycletrivial(int j, char* name, int* error, FILE* out) {
    if (!object_make_dfa(j, error, out)) {
        return false; // Error in making DFA
    }
    if (out) {
        fprintf(out, "#### Checking if the %s is cycle trivial.\n", name);
    }
    bool res = is_cycletrivial_dfa(objects[j]->aut->obj_dfa, error, out);
    if (*error < 0) {
        return false;
    }
    if (res) {
        if (out) {
            fprintf(out, "#### The %s is cycle trivial.\n", name);
        }
        return true;
    }
    else {
        if (out) {
            fprintf(out, "#### The %s is not cycle trivial.\n", name);
        }
        return false;
    }

}
