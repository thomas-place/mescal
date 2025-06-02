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





