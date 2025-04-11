#include "shell_separ.h"

#include "shell_commands.h"

// Message d'erreur pour les automates
/* static void shell_membmorp_error(void) {
    fprintf(stderr, "Error: Separation is not supported for input morphisms.\n");
} */

bool (*class_separation[CL_END]) (nfa*, nfa*, bool, FILE*) = { NULL };

void init_separation(void) {

    class_separation[CL_ST] = NULL;
    class_separation[CL_MOD] = decid_mod_sep;
    class_separation[CL_GR] = decid_grp_sep;
    class_separation[CL_PPT] = decid_polst_sep;
    class_separation[CL_POLMOD] = decid_polmod_sep;
    class_separation[CL_POLGR] = decid_polgr_sep;


}


static void shell_display_inputs(nfa* A1, nfa* A2, FILE* out) {
    fprintf(out, "#### Displaying NFAs recognizing the two inputs.\n");
    fprintf(out, "#### First input language.\n");
    view_nfa(A1);
    fprintf(out, "#### Second input language.\n");
    view_nfa(A2);
}


// Procédure principale
bool shell_separation(com_parameters* pars) {
    int n = com_nbparams(pars);
    // Il doit y avoir entre trois et quatre arguments:
    // - un pour la classe
    // - les deux variables qui donnent les langages ou les automates pris en input
    // - un argument éventuel pour demander des détails
    if (n < 3 || n > 4) {
        shell_error_rangeparams(keywordtostring(KY_SEPAR), 3, 4);
        return false;
    }

    bool details = false;
    if (n == 4) {
        com_keyword dets = key_from_string_chain_single(com_getparam(pars, 3)->main);
        if (dets == KY_INFO) {
            details = true;
        }
        else {
            fprintf(stderr, "Error: Unknown parameter 4 in the command \"%s\".\n", keywordtostring(KY_SEPAR));
            return false;
        }
    }


    print_title_box(0, true, stdout, 1, "Separation");
    // La classe
    classes cl = command_to_class(com_getparam(pars, 0));

    if (cl == CL_END) {
        fprintf(stdout, "#### The class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, " is either unknown or unsupported.\n\n");
        return false;
    }

    if (class_separation[cl] == NULL) {
        fprintf(stdout, "#### Separation is unsupported for the class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, ".\n\n");
        return false;
    }

    fprintf(stdout, "#### ");
    print_command(com_getparam(pars, 0), stdout);
    fprintf(stdout, " is equal to the class %s.\n", class_names[cl]);
    print_class_info(cl, stdout);

    // On commence par récupérer les deux variables (deux derniers arguments)
    bool saved1;
    int i1 = com_apply_command(com_getparam(pars, 1), NULL, MODE_DEFAULT, &saved1);
    if (i1 == -1) {
        return false;
    }
    bool saved2;
    int i2 = com_apply_command(com_getparam(pars, 2), NULL, MODE_DEFAULT, &saved2);
    if (i2 == -1) {
        if (saved1) {
            object_free(i1);
        }
        return false;
    }
    // On calcule maintenant deux automates correspondant aux inputs
    nfa* A1;
    nfa* A2;
    switch (objects[i1]->type)
    {
    case AUTOMATON:
        A1 = nfa_elimeps(objects[i1]->nfa);
        nfa_reset_state_names(A1);
        break;
    case REGEXP:
        A1 = reg_thompson(objects[i1]->exp);
        nfa_elimeps_mod(A1);
        nfa_trim_mod(A1);
        nfa_reset_state_names(A1);
        break;
    case MORPHISM:
        A1 = nfa_copy(objects[shell_compute_minimal(i1)]->nfa);
        break;
    default:
        if (saved1) {
            object_free(i1);
        }
        if (saved2) {
            object_free(i2);
        }
        return false;
        break;
    }
    switch (objects[i2]->type)
    {
    case AUTOMATON:
        A2 = nfa_elimeps(objects[i2]->nfa);
        nfa_reset_state_names(A2);
        break;
    case REGEXP:
        A2 = reg_thompson(objects[i2]->exp);
        nfa_elimeps_mod(A2);
        nfa_trim_mod(A2);
        nfa_reset_state_names(A2);
        break;
    case MORPHISM:
        A2 = nfa_copy(objects[shell_compute_minimal(i2)]->nfa);
        break;
    default:
        delete_nfa(A1);
        if (saved1) {
            object_free(i1);
        }
        if (saved2) {
            object_free(i2);
        }
        return false;
        break;
    }
    shell_display_inputs(A1, A2, stdout);


    print_conclusion_separ(stdout, class_separation[cl](A1, A2, details, stdout), class_names[cl]);
    if (saved1) {
        object_free(i1);
    }
    if (saved2) {
        object_free(i2);
    }
    delete_nfa(A1);
    delete_nfa(A2);
    return false;
}



