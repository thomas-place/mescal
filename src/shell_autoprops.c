#include "shell_autoprops.h"
#include "printing.h"

static bool object_make_dfa(int j, int *error, FILE *out)
{
    if (j < 0 || (objects[j].type != NAUTOMATON && objects[j].type != DAUTOMATON))
    {
        if (out)
        {
            fprintf(out, "#### Error, the input is not an automaton.\n");
            *error = -1;
        }
        return false;
    }

    if (objects[j].type == DAUTOMATON)
    {
        return true; // Already a DFA
    }

    nfa *A = objects[j].obj_nfa;
    dfa *D = detnfa_to_dfa(A);
    if (!D)
    {
        if (out)
        {
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

bool shell_autoprop_letterind(int j, int *error, FILE *out)
{
    if (!object_make_dfa(j, error, out))
    {
        return false; // Error in making DFA
    }
    return is_letterind_dfa(objects[j].obj_dfa, out);
}

bool shell_autoprop_commut(int j, int *error, FILE *out)
{
    if (!object_make_dfa(j, error, out))
    {
        return false; // Error in making DFA
    }
    return is_comm_dfa(objects[j].obj_dfa, out);
}

bool shell_autoprop_permut(int j, int *error, FILE *out)
{
    if (!object_make_dfa(j, error, out))
    {
        return false; // Error in making DFA
    }
    return is_permutation_dfa(objects[j].obj_dfa, out);
}

bool shell_autoprop_permutplus(int j, int *error, FILE *out)
{
    if (!object_make_dfa(j, error, out))
    {
        return false; // Error in making DFA
    }

    return is_permutationplus_dfa(objects[j].obj_dfa, out);
}

bool shell_autoprop_sfgp(int j, basis mode, int *error, FILE *out)
{
    if (!object_make_dfa(j, error, out))
    {
        return false; // Error in making DFA
    }
    return is_counterfreegp_dfa(objects[j].obj_dfa, mode, error, out);
}

bool shell_autoprop_rtrivialgp(int j, basis mode, int *error, FILE *out)
{
    if (!object_make_dfa(j, error, out))
    {
        return false; // Error in making DFA
    }
    return is_rtrivialgp_dfa(objects[j].obj_dfa, mode, out);
}

bool shell_autoprop_dagp(int j, basis mode, int *error, FILE *out)
{
    if (!object_make_dfa(j, error, out))
    {
        return false; // Error in making DFA
    }
    return is_dagp_dfa(objects[j].obj_dfa, mode, out);
}

// bool shell_autoprop_dagp_opti(int j, basis mode, int *error, FILE *out)
// {
//     if (!object_make_dfa(j, error, out))
//     {
//         return false; // Error in making DFA
//     }
//     return make_pattern_conclusion_default(is_dagp_opti_dfa(objects[j].obj_dfa, mode, out), out);
// }

bool shell_autoprop_nosimplecounter(int j, int *error, FILE *out)
{
    if (!object_make_dfa(j, error, out))
    {
        return false; // Error in making DFA
    }
    if (out)
    {
        fprintf(out, "#### Checking if the automaton has no simple counter.\n");
    }
    bool res = is_nosimple_counter_dfa(objects[j].obj_dfa, error);
    if (*error < 0)
    {
        return false;
    }
    if (res)
    {
        if (out)
        {
            fprintf(out, "#### The automaton does not contain a simple counter.\n");
        }
        return true;
    }
    else
    {
        if (out)
        {
            fprintf(out, "#### The automaton contains a simple counter.\n");
        }
        return false;
    }
}
